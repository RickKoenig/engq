// simple example of networked game using socker
// uses tcp for login/logout and udp for position updates..
#include <m_eng.h>
#include <l_misclibm.h>

#include "match_game1.h"
#include "match_logon.h"
#include "../u_states.h"

// game sockets

// server
static socker *gametcpsv,*gametcpcls[MAXGAMERS],*gameudpsv;
// client
static socker *gametcpcl,*gameudpcl;
static int numgameclients;

//static struct reslist *rl;
//static int ks;
//extern int enablenumkeychangestate;
static int keepsock;

static C32 acol;


struct player {
	int active;
	int x,y;
	C32 c;
};
static struct player players[MAXGAMERS];
static int you;
static int down,rright;

enum {PACK_NONE,PACK_INFO,PACK_POS,PACK_CHAT,PACK_MOVE};

#include <pshpack1.h>
// server sends these
static struct playinfopacket {
	char head;
	int pn;
	C32 col;
	int you;
	int alive;
	int x,y;
} pip={PACK_INFO};

static struct pospacket {
	char head;
	int pn;
	int x;
	int y;
} pp={PACK_POS};
// clients send these
static struct movepacket {
	char head;
	signed char down;
	signed char rright;
} mp={PACK_MOVE};

#include <poppack.h>
#define MAXRB 100
static char readbuff[MAXRB];

void freegamesockers()
{
	int i;
	if (gametcpsv) {
		freesocker(gametcpsv);
		gametcpsv=NULL;
	}
	if (gametcpcl) {
		freesocker(gametcpcl);
		gametcpcl=NULL;
	}
	if (gameudpcl) {
		freesocker(gameudpcl);
		gameudpcl=NULL;
	}
	for (i=0;i<MAXGAMERS;i++) {
		if (gametcpcls[i]) {
			freesocker(gametcpcls[i]);
			gametcpcls[i]=NULL;
		}
	}
	if (gameudpsv) {
		freesocker(gameudpsv);
		gameudpsv=NULL;
	}
	numgameclients=0;
	cgi.isvalid=false;
}

// send gameinfo back to matching server
static void gameinfoupdate()
{
	if (matchcl && gametcpsv) { // if here from matchlobby and a host, then send a gamepicked token to matching server
// gamepicked hostname gamename curplayers ip port
		const C8* ipstr=ip2str(cgi.ip);
		C8 token[300];
		const C8* gn=getgamename(cgi.gameidx);
		sprintf(token,"gamepicked %s pn=%d %s %d",gn,numgameclients,ipstr,gametcpsv->port);
		U32 len=strlen(token);
		tcpwritetoken32(matchcl,0,token,len);
		tcpsendwritebuff(matchcl);
	}
}

void match_game1init()
{
	if (cgi.isvalid) {
		if (cgi.ishost) {
			gametcpsv=makecgisock(&gameudpsv);
		} else {
			gametcpcl=makecgisock(&gameudpcl);
		}
	}
	if (gametcpsv==NULL && gametcpcl==NULL)
		return;

	logger("game1 init\n");
	video_setupwindow(200,150);
//	ks=enablenumkeychangestate;
//	enablenumkeychangestate=0;
	keepsock=0;
	you=-1;
	memset(players,0,sizeof(players));
	if (gametcpsv) {
		numgameclients++;
		you=0;
		players[0].active=1;
		players[0].c=C32WHITE;
		players[0].x=0;
		players[0].y=0;
	}
	if (gametcpsv)
		logger("gametcpsv: ip %s, port %d\n",ip2str(gametcpsv->ip),gametcpsv->port);
	if (gameudpsv)
		logger("gameudpsv: ip %s, port %d\n",ip2str(gameudpsv->ip),gameudpsv->port);
	if (gametcpcl)
		logger("gametcpcl: ip %s, port %d\n",ip2str(gametcpcl->ip),gametcpcl->port);
	if (gameudpcl)
		logger("gameudpcl: ip %s, port %d\n",ip2str(gameudpcl->ip),gameudpcl->port);

	gameinfoupdate();
}

void match_game1proc()
{
//	extern int sleeper;
	int i,j;
	struct socker *ts;
// shouldn't be here
	if (gametcpsv==NULL && gametcpcl==NULL) {
		popstate();
		return;
	}
// quit
	wininfo.sleeper=10; // keep this task alive
	if (KEY==K_ESCAPE || (gametcpcl && (gametcpcl->flags & SF_CLOSED)) || 
		(gametcpsv && (gametcpsv->flags & SF_CLOSED))) {
		keepsock=1;
		if (matchcl) {
// gamepicked
			char* gp="gamepicked";
			tcpwritetoken32(matchcl,0,gp,strlen(gp));
			tcpsendwritebuff(matchcl);
		}
//		changestatefunc(st_resultsinit);
		changestate(STATE_MATCHRESULTS);
		return;
	}
// server side recv data
	if (gametcpsv) {
// log out old users
		for (i=1;i<MAXGAMERS;i++) {
			if (players[i].active && (gametcpcls[i]->flags&SF_CLOSED)) {
				freesocker(gametcpcls[i]);
				gametcpcls[i]=NULL;
//				freesocker(gameudpcls[i]);
//				gameudpcls[i]=NULL;
				players[i].active=0;
				numgameclients--;
				gameinfoupdate();
				pip.pn=i;
				pip.alive=0;
				for (j=1;j<MAXGAMERS;j++)
					if (i!=j && players[j].active)
						tcpwritetoken32(gametcpcls[j],0,(char *)&pip,sizeof(pip));
			}
		}
// log in new users
		if (numgameclients<MAXGAMERS) {
			ts=tcpgetaccept(gametcpsv);
			if (ts) {
				for (i=1;i<MAXGAMERS;i++) {
					if (gametcpcls[i]==NULL) {
						gametcpcls[i]=ts;
						players[i].x=i*20;
						players[i].y=i*20;
						acol.r=128+mt_random(128);
						acol.g=128+mt_random(128);
						acol.b=128+mt_random(128);
						players[i].c=acol;
						players[i].active=1;
						pip.x=players[i].x;
						pip.y=players[i].y;
						pip.pn=i;
						pip.col=players[i].c;
						pip.you=1;
						pip.alive=1;
						tcpwritetoken32(ts,0,(char *)&pip,sizeof(pip)); // send new player his playnum
						pip.you=0;
// send other players your playnum
						for (j=1;j<MAXGAMERS;j++)
							if (i!=j && players[j].active)
								tcpwritetoken32(gametcpcls[j],0,(char *)&pip,sizeof(pip));
// send new player all other players
						for (j=0;j<MAXGAMERS;j++)
							if (i!=j && players[j].active) {
								pip.x=players[j].x;
								pip.y=players[j].y;
								pip.pn=j;
								pip.col=players[j].c;
								pip.alive=1;
								tcpwritetoken32(ts,0,(char *)&pip,sizeof(pip)); // send new player his playnum
							}
//						gameudpcls[i]=udpmake(myip,myport);	// port of 0 is any, -1 none (unbound)
//						if (!gameudpcls[i])
//							errorexit("cant make udp");
						numgameclients++;
						gameinfoupdate();
						break;
					}
				}
				if (i==MAXGAMERS)
					errorexit("can't find client slot!");
			}
		}
// read in all data from users
		for (i=1;i<MAXGAMERS;i++) {
			if (players[i].active) {
				tcpfillreadbuff(gametcpcls[i]); // winsock ----> buff
				while (1) {
					j=tcpreadtoken32(gametcpcls[i],0,readbuff,MAXRB);
					if (!j)
						break;
/*					switch(readbuff[0]) {
					case PACK_MOVE:
						memcpy(&mp,readbuff,sizeof(mp));
						players[i].x+=mp.rright;
						players[i].y+=mp.down;
						break;
					} */
				}
				while(1) {
					U32 rip,rp,k;
					j=udpgetbuffdata(gameudpsv,readbuff,sizeof(readbuff),&rip,&rp);
					if (!j)
						break;
					for (k=1;k<MAXGAMERS;k++) {
						if (players[k].active) {
							if (gametcpcls[k]->remoteip==rip && gametcpcls[k]->remoteport==rp) {
								switch(readbuff[0]) {
								case PACK_MOVE:
									memcpy(&mp,readbuff,sizeof(mp));
									players[k].x+=mp.rright;
									players[k].y+=mp.down;
									break;
								}
							}
						}
					}
				}
			}
		}
// client side
	} else {
		tcpfillreadbuff(gametcpcl);
		while (1) {
			i=tcpreadtoken32(gametcpcl,0,readbuff,MAXRB);
			if (!i)
				break;
			switch(readbuff[0]) {
			case PACK_INFO:
				memcpy(&pip,readbuff,sizeof(pip));
				if (!pip.alive)
					players[pip.pn].active=0;
				else {
					players[pip.pn].active=1;
					players[pip.pn].c=pip.col;
					players[pip.pn].x=pip.x;
					players[pip.pn].y=pip.y;
				}
				if (pip.you)
					you=pip.you;
				break;
/*			case PACK_POS:
				memcpy(&pp,readbuff,sizeof(pp));
				players[pp.pn].active=1;
				players[pp.pn].x=pp.x;
				players[pp.pn].y=pp.y;
				break; */
			}
		}
		while(1) {
			U32 rip,rp;
			i=udpgetbuffdata(gameudpcl,readbuff,sizeof(readbuff),&rip,&rp);
			if (!i)
				break;
			if (rip==gametcpcl->remoteip && rp==gametcpcl->remoteport) {
				switch(readbuff[0]) {
				case PACK_POS:
					memcpy(&pp,readbuff,sizeof(pp));
					players[pp.pn].active=1;
					players[pp.pn].x=pp.x;
					players[pp.pn].y=pp.y;
					break;
				}
			}
		}
	}
// gameplay
	rright=down=0;
	if (you>=0) {
		if (wininfo.keystate[K_LEFT] || wininfo.keystate[K_NUMLEFT])
			rright--;
		if (wininfo.keystate[K_RIGHT] || wininfo.keystate[K_NUMRIGHT])
			rright++;
		if (wininfo.keystate[K_UP] || wininfo.keystate[K_NUMUP])
			down--;
		if (wininfo.keystate[K_DOWN] || wininfo.keystate[K_NUMDOWN])
			down++;
	}
// send data
// server
	if (gametcpsv) {
		players[0].x+=rright;
		players[0].y+=down;
		for (i=1;i<MAXGAMERS;i++) {
			if (players[i].active) {
				for (j=0;j<MAXGAMERS;j++)
					if (players[j].active) {
						pp.pn=j;
						pp.x=players[j].x;
						pp.y=players[j].y;
						udpputbuffdata(gameudpsv,(char *)&pp,sizeof(pp),gametcpcls[i]->remoteip,gametcpcls[i]->remoteport);
//						tcpwritetoken(gametcpcls[i],(char *)&pp,sizeof(pp));
					}
				tcpsendwritebuff(gametcpcls[i]);
			}
		}
// client
	} else {
		if (down || rright) {
			mp.down=down;
			mp.rright=rright;
			udpputbuffdata(gameudpcl,(char *)&mp,sizeof(mp),gametcpcl->remoteip,gametcpcl->remoteport);
//			tcpwritetoken(gametcpcl,(char *)&mp,sizeof(mp));
		}	
		tcpsendwritebuff(gametcpcl);
	}
}

void match_game1draw2d()
{
	cliprect32(B32,0,0,WX,WY,C32BLUE);
	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"game 1");
	S32 i;
	for (i=0;i<MAXGAMERS;i++)
		if (players[i].active)
			clipcircle32(B32,players[i].x,players[i].y,5,players[i].c);
}

void match_game1exit()
{
//	if (gametcpsv==NULL && gametcpcl==NULL)
//		return;
//	enablenumkeychangestate=ks;
	freegamesockers();
	if (!keepsock) { // total bail
		if (matchcl) {
			freesocker(matchcl);
			matchcl=0;
		}
	}
	if (!matchcl) {
		uninitsocker();
	}
}
