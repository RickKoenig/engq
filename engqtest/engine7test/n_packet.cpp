/*
#include <engine7cpp.h>
#include <misclib7cpp.h>

#include "usefulcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "carenagamecpp.h"
#include "packetcpp.h"
#include "carenaconnectcpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_carclass.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"
#include "n_aicpp.h"
#include "n_scrlinecpp.h"
#include "n_physicscpp.h"
#include "n_rematchcpp.h"
#include "n_carenaconnectcpp.h"
#include "n_line2roadcpp.h"
#include "n_twobjcpp.h"

static void packetbroadcast(int id,void *data,int len);

struct packhere {
	unsigned char from;
	unsigned char isbot;
//	int gamenum;
};

// key bit positions
#define PK_UP 0
#define PK_DOWN 1
#define PK_LEFT 2
#define PK_RIGHT 3
#define PK_RESETGAME 4
#define PK_REMATCHYES 5
#define PK_ISBOT 6
#define PK_MAXKEYBITS 7

struct packkey {
	unsigned char from;
//	unsigned char isbot;
	unsigned char keybits;
//	int tis; // timestamp
};

static twgg twggs0;
static twcaro twcarobjs0[MAX_GAMEOBJ];

twgg twggs;
twcaro twcarobjs[MAX_GAMEOBJ];

struct kb {
	unsigned char *keybuffdata;
	int ts;
	int size;
	int off;
};

static struct kb kbs[MAX_PLAYERS];
#define KEYBUFFSIZE 2400	// in 1/60 secs, set for 400 secs... for now overflow will error, later it will bot


void drawkeybuff()
{
	struct kb *k;
	int i;
	int lf,rt;
	cliprect32(B32,256,WY-16,256+240,WY-1,C32DARKGRAY);
	for (i=0;i<gp.gt.gi.humplayers;i++) {
		if (gos[i].av.ownerid==-1)
			continue;
		k=&kbs[i];
		lf=k->off;
		rt=k->off+k->size;
		if (rt>KEYBUFFSIZE) {
			rt-=KEYBUFFSIZE;
			cliprect32(B32,256+lf/10,WY-16+2*i,KEYBUFFSIZE/10-lf/10+1,1,C32WHITE);
			cliprect32(B32,256,WY-16+2*i,rt/10+1,1,C32WHITE);
		} else {
			cliprect32(B32,256+lf/10,WY-16+2*i,rt/10-lf/10+1,1,C32WHITE);
		}
	}
}

///////////////// keystroke buffers //////////////////////////////
// convert keybits to something loggable
#if 0
static char *ktoa(int keybits)
{
	static char retstr[PK_MAXKEYBITS+1];
	int i;
	for (i=PK_MAXKEYBITS-1;i>=0;i--)
		retstr[i]='0'+(1&(keybits>>i));
	return retstr;
}
#endif
void resetkeybuff()
{
	int i;
	for (i=0;i<MAX_PLAYERS;i++) {
		if (kbs[i].keybuffdata) {
//			memfree(kbs[i].keybuffdata);
			delete[] kbs[i].keybuffdata;
			kbs[i].keybuffdata=0;
			kbs[i].off=kbs[i].size=kbs[i].ts=0;
		}
	}
}

static void addkeybuff(struct packkey *pk) // puts the key in the buffer, ignores ts
{
	int i;
	int p;
	struct kb *k;
	i=pk->from;
	k=&kbs[i];
	if (!k->keybuffdata)
//		k->keybuffdata=(unsigned char *)memalloc(KEYBUFFSIZE);
		k->keybuffdata = new U8[KEYBUFFSIZE];
	if (k->size==KEYBUFFSIZE)
		errorexit("buffer overflow player %d",i);
	p=k->off+k->size;
	if (p>=KEYBUFFSIZE)
		p-=KEYBUFFSIZE;
	k->keybuffdata[p]=pk->keybits;
	if (gg.yourid!=pk->from && k->ts+k->size>gg.pingclock) { // save most recent ping
		gg.pingclock=k->ts+k->size;
	}
	k->size++;
//	con32_printf(gg.con,"akb ts %d, size %d, off %d\n",k->ts,k->size,k->off);
}

// let the 'langoliers' handle this, too far into the past, eat up everything less than timest
void remkeybuff(int timest) // this time stamp and earlier will no longer be needed, try to keep at least one
{
	int nrem;
	int i;
	for (i=0;i<gp.gt.gi.humplayers;i++) {
		struct kb *k=&kbs[i];
		if (!k->size)
			continue;
//		con32_printf(gg.con,"rkb ts %d\n",timest);
		nrem=timest-k->ts+1;
		if (nrem<0)
			nrem=0;
		if (nrem>=k->size) // keep at least one key 
 			nrem=k->size-1;
		k->ts+=nrem;
		k->off+=nrem;
		if (k->off>=KEYBUFFSIZE)
			k->off-=KEYBUFFSIZE;
		k->size-=nrem;
	}
}

static int firstinvalidkeybuffpn(int pn)
{
	struct kb *k;
	k=&kbs[pn];
	return k->ts+k->size;
}

int firstinvalidkeybuff() // this time stamp doesn't have all the data (yet)
{
	int i,j;
	int ret=gg.clock+1000000; // that should keep a bot game happy!
	if (gp.gamenet==GAMENET_BOTGAME)
		return ret; 
	for (i=0;i<gp.gt.gi.humplayers;i++)
		if (gos[i].av.ownerid!=-1) {
//		if (gg.retrytimers[i]==-1) {
//		if (gos[i].av.ownerid!=-1 && gos[i].av.cntl==CNTL_HUMAN) { 
			j=firstinvalidkeybuffpn(i);
			if (j<ret)
				ret=j;
		}
	return ret;
}

void getkeybuff(int pn)	 // gets key (or predicts) and puts it in gg.ku,gg.kd etc. looks at gg.clock
{
	int p,v;
	struct kb *k=&kbs[pn];
	int c=gg.clock;
	if (k->size==0) { // just drive (predict)
		gg.pk.ku=1;
		gg.pk.kd=0;
		gg.pk.kl=0;
		gg.pk.kr=0;
		gg.pk.krematchyes=gg.pk.kresetgame=0;
		return;
	}
	p=c-k->ts;
	if (p<0)
		errorexit("getkeybuff: clock  %d < earliest %d",c,k->ts);
	if (p>=k->size)
		p=k->size-1;
	p+=k->off;
	if (p>=KEYBUFFSIZE)
		p-=KEYBUFFSIZE;
	v=k->keybuffdata[p];
	gg.pk.ku=(v>>PK_UP)&1;
	gg.pk.kd=(v>>PK_DOWN)&1;
	gg.pk.kl=(v>>PK_LEFT)&1;
	gg.pk.kr=(v>>PK_RIGHT)&1;
	gg.pk.kresetgame=(v>>PK_RESETGAME)&1;
	gg.pk.krematchyes=(v>>PK_REMATCHYES)&1;
	gg.pk.wentbot=(v>>PK_ISBOT)&1;
//	gg.krematchno=(v>>PK_REMATCHNO)&1;
//	con32_printf(gg.con,"pn %d, keybits %s\n",pn,ktoa(v));
}

///////////////// save and restore game states ////////////////////
static void twsave(struct twgg *g,struct twcaro *cs)
{
	int i;
	g->clock=gg.clock;
	g->clockoffset=gg.clockoffset;
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
		cs[i].seekmode=gos[i].seekmode;
		cs[i].pos=gos[i].pos;
		cs[i].vel=gos[i].vel;
		cs[i].rot=gos[i].rot;
		cs[i].rotvel=gos[i].rotvel;
		cs[i].rematchyes=gos[i].rematchyes;
//		cs[i].rematchno=gos[i].rematchno;
	}
}

static void twrestore(struct twgg *g,struct twcaro *cs)
{
	int i;
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
		gos[i].seekmode=cs[i].seekmode;
		gos[i].pos=cs[i].pos;
		gos[i].vel=cs[i].vel;
		gos[i].rot=cs[i].rot;
		gos[i].rotvel=cs[i].rotvel;
		gos[i].rematchyes=cs[i].rematchyes;
//		gos[i].rematchno=cs[i].rematchno;
	}
}

// remember initial clock0 gamestate
void twsave0()
{
	twsave(&twggs0,&twcarobjs0[0]);
	resetkeybuff();
}

// reset a game by: 
// 1) setting clockoffset to start of next game (leaving alone clock)
// 2) restoring all gamestate to clock0 gamestate
void twrestore0()
{
	gg.clockoffset=gg.clock;
	twrestore(&twggs0,&twcarobjs0[0]);
//	resetkeybuff();
}

void twsaven()
{
	twsave(&twggs,&twcarobjs[0]);
}

void twrestoren()
{
	gg.clock=twggs.clock;
	gg.clockoffset=twggs.clockoffset;
	twrestore(&twggs,&twcarobjs[0]);
}

static void packetsendkeybot(int from)
{
	struct packkey pk;
	pk.from=from;
	pk.keybits=1<<PK_ISBOT;
//	pk.isbot=1;
//	pk.
	addkeybuff(&pk);
	packetbroadcast(PACK_KEY,&pk,sizeof(pk));
}

// read the input network stream and process all packets, called from proc
void packetreadbuff()
{
	int i,j,id,n;
	struct packhere *ph;
	struct packkey *pk;
	char tokbuff[1000];
// read data from all sockets
	gg.newdata=0;
	for (i=0;i<gp.gt.gi.humplayers;i++) {
		tcpfillreadbuff(gg.sock[i]); // READBUFF
		while(1) {
			n=tcpreadtoken32(gg.sock[i],&id,tokbuff,sizeof(tokbuff));
			if (n<=0)
				break;
			if (gp.gamenet==GAMENET_SERVER) // server broadcasts to all players not self
				for (j=1;j<gp.gt.gi.humplayers;j++)
					if (i!=j)
						tcpwritetoken32(gg.sock[j],id,tokbuff,n);
			switch(id) {
			case PACK_HERE: // here packets are used to start a game going after a load (sync/wait)
				if (!gg.gamestart) {
					ph=(struct packhere *)tokbuff;
					if (gp.gamenet==GAMENET_SERVER)
						con32_printf(gg.con,"S: rd here from %d\n",i);
					else
						con32_printf(gg.con,"C: rd here from %d\n",ph->from);
					if (ph->isbot) { // also signals that a player dropped out during load
//						gos[ph->from].av.cntl=CNTL_AI;
						gos[ph->from].av.ownerid=-1; // noone can change
//						gp.gt.botplayers++;
					} 
					gg.retrytimers[ph->from]=-1; // freeze timer of here sender
					gg.numcurplayers++;
					if (gg.numcurplayers==gp.gt.gi.humplayers) {
						if (gp.gamenet==GAMENET_SERVER)
							con32_printf(gg.con,"S: start game, bp %d\n",gp.gt.gi.botplayers);
						else
							con32_printf(gg.con,"C: start game, bp %d\n",gp.gt.gi.botplayers);
						startgame();
					}
				}
				break;
			case PACK_KEY:
				if (gg.gamestart) {
					gg.newdata=1;
					pk=(struct packkey *)tokbuff;
//					if (gp.gamenet!=GAMENET_CLIENT && pk->isbot)
//						errorexit("server: got a isbot keypacket");
//					con32_printf(gg.con,"key at %d frm %d, ib %d, '%s'\n",pk->tis,pk->from,pk->isbot,ktoa(pk->keybits));
//					if (pk->isbot) { // also signals that a player dropped out during load, amazing if i can make server send me this
//						gos[pk->from].av.cntl=CNTL_AI;
//						gos[pk->from].av.ownerid=-1; // noone can change
//						gp.gt.botplayers++;
//					} else
						addkeybuff(pk);
				}
				break;
	/*		case PACK_RESTART:
				break; */
			}
		}
	}
	if (gg.gamestart) {
// check for disconnected players during game
		if (gp.gamenet==GAMENET_SERVER) {
			for (i=1;i<gp.gt.gi.humplayers;i++) {
				if (gg.sock[i] && gos[i].av.ownerid!=-1) {
					if ((gg.sock[i]->flags&SF_CLOSED) || gg.clock-firstinvalidkeybuffpn(i)>TICKRATE*gcfg.ingametimeout ) {
						freesocker(gg.sock[i]);
						gg.sock[i]=0;
						packetsendkeybot(i);
//						gos[i].av.ownerid=-1;
						for (j=1;j<gp.gt.gi.humplayers;j++)
							if (gg.sock[j])
								break;
						if (j==gp.gt.gi.humplayers) {
							cleansocks();
							gp.gamenet=GAMENET_BOTGAME;
						}
					}
				}
			}
		} else if (gp.gamenet==GAMENET_CLIENT) {
			if (gg.sock[0] && gos[0].av.ownerid!=-1) {
				if ((gg.sock[0]->flags&SF_CLOSED) || gg.clock-firstinvalidkeybuffpn(0)>TICKRATE*gcfg.ingametimeout ) {
					cleansocks();
//					resetkeybuff();
					gp.gamenet=GAMENET_BOTGAME;
					for (i=0;i<gp.gt.gi.humplayers;i++) {
						if (gg.yourid!=i)
							gos[i].av.ownerid=-1;
					}
				}
			}
		}
	} else { // during connect
		if (gp.gamenet==GAMENET_SERVER) { // server: make unconnecting players bots..
			for (i=1;i<gp.gt.gi.humplayers;i++)
// check 'here' packet timeouts
				if ((gg.sock[i] && (gg.sock[i]->flags&SF_CLOSED)) || gg.retrytimers[i]!=-1 && gg.milli>=gg.retrytimers[i]) { // make a bot
					struct packhere ph;
					con32_printf(gg.con,"S: player %d timeout\n",i);
					if (gg.sock[i]&&gg.sock[i]->flags&SF_CLOSED)
						con32_printf(gg.con,"socket closed\n");
					if (gg.retrytimers[i]!=-1 && gg.milli>=gg.retrytimers[i])
						con32_printf(gg.con,"milli %d, retry %d\n",gg.retrytimers[i]);
//					gos[i].av.cntl=CNTL_AI;
					gos[i].av.ownerid=-1; // noone can change
					if (gg.sock[i]) {
						freesocker(gg.sock[i]);
						gg.sock[i]=0;
					}
					ph.from=i;
					ph.isbot=1;
					packetbroadcast(PACK_HERE,&ph,sizeof(ph)); // tell everyone that this is a bot
					gg.numcurplayers++;
					gg.retrytimers[i]=-1;
//					gp.gt.botplayers++;
					if (gg.numcurplayers==gp.gt.gi.humplayers) {
						con32_printf(gg.con,"S: timeout start game, bp %d\n",gp.gt.gi.botplayers);
//						gp.gamenet=GAMENET_BOTGAME;
						gp.gamenet=GAMENET_SERVER;
						startgame();
					}
				}
		} else { // client:
			if ((gg.sock[0] && (gg.sock[0]->flags&SF_CLOSED)) || gg.milli>=gg.timeout) { // dobotgame
				con32_printf(gg.con,"C: timeout\n");
				if (gg.sock[0]) {
					freesocker(gg.sock[0]);
					gg.sock[0]=0;
				}
				for (i=0;i<gp.gt.gi.humplayers;i++)
					if (i!=gg.yourid)
						gos[i].av.ownerid=-1;
//					else
//						gos[i].av.ownerid=0;
//				gg.yourid=0;
				gp.gamenet=GAMENET_BOTGAME;
				gg.gamestart=1;
//				gp.gt.botplayers=gp.gt.numplayers-1;
				con32_printf(gg.con,"C: start game, bp %d\n",gp.gt.gi.botplayers);
			}
		}
	}
}

// write out write buffers
void packetwritebuff()
{
	int i;
//	static int nopwt;
//	nopwt++;
//	nopwt&=63;
//	if (!nopwt)
//		packetbroadcast(PACK_NOP,0,0);
	for (i=0;i<gp.gt.gi.humplayers;i++) {
		if (gg.sock[i])
			tcpsendwritebuff(gg.sock[i]); // WRITEBUFF
	}
}

// send data to all other players
static void packetbroadcast(int id,void *data,int len)
{
	int i,n;
	if (gp.gamenet==GAMENET_SERVER) { // server
		for (i=1;i<gp.gt.gi.humplayers;i++)
			n=tcpwritetoken32(gg.sock[i],id,(char *)data,len);
	} else // client
		n=tcpwritetoken32(gg.sock[0],id,(char *)data,len);
}

// send a here packet
void packetsendhere()
{
	struct packhere ph;
	ph.from=gg.yourid;
	ph.isbot=0;
	packetbroadcast(PACK_HERE,&ph,sizeof(ph));
}

void packetsendkey(int ts,int u,int d,int l,int r,int resetgame,int rematchyes)//,int rematchno) // must pass 0 or 1 only
{
	struct packkey pk;
//	pk.tis=ts;
	pk.from=gg.yourid;
//	pk.isbot=0;
	pk.keybits=(u<<PK_UP)+(d<<PK_DOWN)+(l<<PK_LEFT)+(r<<PK_RIGHT)+(resetgame<<PK_RESETGAME)+
		(rematchyes<<PK_REMATCHYES);//+(rematchno<<PK_REMATCHNO);
	packetbroadcast(PACK_KEY,&pk,sizeof(pk));
	addkeybuff(&pk);
}

