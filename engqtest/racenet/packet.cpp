/* general packet types
// sv to cl: 
	from 
	kind
	blen
	data[blen]

// cl to sv: 
	to
	kind
	blen
	data[blen]

/// specific packet types
HERE: acknowledge existence
	len 0

KEY: player input
	len 3
	keybits 1
	timestamp 2
*/
/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <engine1.h>
#include <misclib.h>

#include "stubhelper.h"
//#include "pieces.h"
#include "line2road.h"
#include "online_uplay.h"
#include "packet.h"

#include "gamestate.h"
*/
#define D2_3D
#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "../engine7test/n_usefulcpp.h"
//#include "../engine7test/n_line2roadcpp.h"
#include "../hw35_line2road.h"

#include "gameinfo.h"
#include "online_uplay.h"
#include "gamestate.h"
#include "packet.h"

#define MAXUDPINPUTS 300
#define UDPHEADERSIZE 6
#define MAXCUDPLEN (MAXUDPINPUTS*2*2+UDPHEADERSIZE)

int rlecomp(C8 *ubuff,C8 *cbuff,int blen)
{
	int i=0;
	int clen=0;
	int rep=0;
	int c=0;
	while(i<blen) {
		if (rep==0) {
			c=ubuff[i];
			rep=1;
		} else if (c==ubuff[i] && rep<255) {
			rep++;
		} else {
			cbuff[clen++]=rep;
			cbuff[clen++]=c;
			c=ubuff[i];
			rep=1;
		}
		i++;
	}
	if (rep) {
		cbuff[clen++]=rep;
		cbuff[clen++]=c;
	}
	return clen;
}

int rledecomp(C8 *cbuff,C8 *ubuff,int clen)
{
	int i=0,j=0;
	int rep,c;
	while(i<clen) {
		rep=cbuff[i++];
		c=cbuff[i++];
		while(rep>0) {
			ubuff[j++]=c;
			rep--;
		}
	}
	return j;
}

int buildudppacket(int pid,C8 *cbuff,int nitosend)
{
	int tsoff;
	int ni;
	int d,i;
	int clen;
	unsigned int *p;
	C8 ubuff[2*MAXUDPINPUTS];
	if (nitosend>MAXUDPINPUTS)
		nitosend=MAXUDPINPUTS;
	cbuff[0]=od.gamenum;
	cbuff[1]=pid;
	tsoff=od.playernet[pid].tsoff;
	ni=od.playernet[pid].ninput;
	d=tsoff+ni; // last valid key minus 1
	if (d<nitosend)
		nitosend=d;
	p=(unsigned int *)(cbuff+2);
	*p=d-nitosend;
	d=od.playernet[pid].offset+ni;
	d-=nitosend;
	for (i=0;i<nitosend;i++) {
		ubuff[i]=od.playernet[pid].inputs[(i+d)&(MAXRECINPUT-1)].key;
		ubuff[nitosend+i]=od.playernet[pid].inputs[(i+d)&(MAXRECINPUT-1)].weapkey;
	}
	clen=rlecomp(ubuff,cbuff+UDPHEADERSIZE,nitosend<<1);
	return UDPHEADERSIZE+clen;
//	return UDPHEADERSIZE+(nitosend<<1);
}

int parseudppacket(C8 *cbuff,int buffsize,int *from,int *unlen)
{
	int nirec;
	int tsoff;
	int off;
	int i,ni;
	int clk;
	int frm;
	int *p;
	int d,d2,nc;
	C8 ubuff[MAXUDPINPUTS*2];
//	if (buffsize&1)
//		return 0;
	if (buffsize<UDPHEADERSIZE)
		return 0;
	if (od.gamenum!=cbuff[0])
		return 0;
	frm=cbuff[1];
	if (frm<0 || frm>=od.numcars)
		return 0;
	p=(int *)(cbuff+2);
	clk=*p;
	ni=od.playernet[frm].ninput;
	tsoff=od.playernet[frm].tsoff;
	off=od.playernet[frm].offset;
	if (clk>tsoff+ni) {	// not enough overlap
		od.markfordisconnect=1;
		return 0;
	}
	d=off+ni;
	nirec=rledecomp(cbuff+UDPHEADERSIZE,ubuff,buffsize-UDPHEADERSIZE);
	nirec>>=1;
	*unlen=nirec;
	nc=(clk+nirec)-(tsoff+ni);
	if (nc+ni>MAXRECINPUT) {
		od.markfordisconnect=1;
		return 0;
	}
	d2=tsoff+ni-clk;
	for (i=0;i<nc;i++) {
		od.playernet[frm].inputs[(i+d)&(MAXRECINPUT-1)].key=ubuff[d2+i];
		od.playernet[frm].inputs[(i+d)&(MAXRECINPUT-1)].weapkey=ubuff[d2+i+nirec];
	}
	od.playernet[frm].ninput=ni+nc;
	*from=frm;
	return 1;
}

void requestrematch()
{
	logger("%d sent a REQREMATCH at %d\n",od.playerid,opa[0].clocktickcount);
	sendapacket(od.playerid,-1,PKT_REQREMATCH,0,NULL);
}

void sendapacket(int from,int to,int kind,int len,char *str) // if to is -1 then to all
{
	int i;
	char bstr[150];
	if (od.playernet[0].tcpclients) { // cl
		bstr[0]=to;
//		bstr[1]=kind;
		memcpy(&bstr[1],str,len);
//		sprintf(bstr,"%d %s",to,str);
		tcpwritetoken8(od.playernet[0].tcpclients,kind,bstr,len+1);//strlen(bstr));
/*
		udpputbuffdata(od.udp,(char *)&from,4,ol_opt.serverip,ol_opt.port);
		if (od.udp) {
			logger("udp: sending 'hi' from '%s' %d, ",ip2str(od.udp->ip),od.udp->port);
			logger("to '%s' %d\n",ip2str(ol_opt.serverip),ol_opt.port);
		}
*/
#ifdef USECON
//		con16_printf(od.ucon,"sendap %d: from %d to %d: '%d'\n",od.playerid,from,to,kind);
#endif
	} else {
		for (i=1;i<OL_MAXPLAYERS;i++) { // sv
			if (od.playernet[i].tcpclients && ( to == -1 || to == i)) {
//				sprintf(bstr,"%d %s",from,str);
				bstr[0]=from;
//				bstr[1]=kind;
				memcpy(&bstr[1],str,len);
				tcpwritetoken8(od.playernet[i].tcpclients,kind,bstr,len+1);//,strlen(bstr));
/*
				if (od.playernet[i].udpip) {
					udpputbuffdata(od.udp,(char *)&from,4,od.playernet[i].udpip,od.playernet[i].udpport);
#ifdef USECON
//				con16_printf(od.ucon,"sendap %d: from %d to %d: '%d\n",od.playerid,from,to,kind);
#endif
					if (od.udp) {
						logger("udp: sending 'ho' from '%s' %d, ",ip2str(od.udp->ip),od.udp->port);
						logger("to '%s' %d\n",ip2str(od.playernet[i].udpip),od.playernet[i].udpport);
					}
				}
*/
			}
		}
	}
}

int getapacket(int *from,int *kind,unsigned char *str) // returns len
{
	char ret[150];
	//,*tok,
//	char bstr[150];
	int len,i,p,to;
	if (od.playernet[0].tcpclients) { // cl
		len=tcpreadtoken8(od.playernet[0].tcpclients,kind,ret,150);
		if (len>0) {
//			ret[len]='\0';
#ifdef USECON
//			con16_printf(od.ucon,"rdtok %d:\n",od.playerid);
#endif
//			tok=strtok(ret," ");
//			if (tok) {
			len-=1;
			*from=ret[0];
//			*kind=ret[1];
			memcpy(str,ret+1,len);
			return len;
/*				*from=atoi(tok);
				tok=strtok(NULL,"");
				strcpy(str,tok);
				return strlen(str);
			} */
		} else
			return -1;
	} else { // sv
		len=0;
		for (p=1;p<OL_MAXPLAYERS;p++) { 
			if (od.playernet[p].tcpclients) {
				len=tcpreadtoken8(od.playernet[p].tcpclients,kind,ret,150);
				if (len>0)
					break;
			}
		}
		if (od.playernet[p].onebotplayer) // ignore packets from the bot player..
			len=0;
		if (len<=0)
			return -1;
//		ret[len]='\0';
#ifdef USECON
//		con16_printf(od.ucon,"rdtok %d:\n",od.playerid);
#endif
//		tok=strtok(ret," ");
//		if (tok) {
//			to=atoi(tok);
//			tok=strtok(NULL,"");
//			strcpy(str,tok);
		to=ret[0];
//		*kind=ret[1];
		len-=1;
		memcpy(str,ret+1,len);
		for (i=1;i<OL_MAXPLAYERS;i++) { 
			if (od.playernet[i].tcpclients && i!=p && ( to == -1 || to == i)) {
//					sprintf(bstr,"%d %s",p,str);
				ret[0]=p;
				tcpwritetoken8(od.playernet[i].tcpclients,*kind,ret,len+1);
#ifdef USECON
//				con16_printf(od.ucon,">>> %d: from %d to %d\n",od.playerid,p,i);
#endif
			}
		}
		if (to==0 || to==-1) {
			*from=p;
			return len;
		}
		return -1;
	}
}

void packetaddinput(int from,int k,int wk) //,int ts)
{
	int ni,off;
	if (od.markfordisconnect)
		return;
	ni=od.playernet[from].ninput;
	off=od.playernet[from].offset;
	if (ni>=MAXRECINPUT-1 || ni>=olracecfg.gameplaytimeout*60) {
		od.markfordisconnect=1;
//		errorexit("too many inputs\n");
	}
//	logger("packet add input from %d key %d wk %d\n",from,k,wk);
	od.playernet[from].inputs[(ni+off)&(MAXRECINPUT-1)].key=k;
	od.playernet[from].inputs[(ni+off)&(MAXRECINPUT-1)].weapkey=wk;
	od.playernet[from].ninput=ni+1;
}

void removeinput(int from,int ts) // this timestamp and earlier not needed anymore..
{
	int ni,off,tsoff,d;
	ni=od.playernet[from].ninput;
	off=od.playernet[from].offset;
	tsoff=od.playernet[from].tsoff;
	if (ni==0)
		return;
	d=ts+1-tsoff;
	if (d<=0)
		return;
	if (d>ni)
		d=ni;
	od.playernet[from].ninput=ni-d;
	od.playernet[from].tsoff=tsoff+d;
	od.playernet[from].offset=(MAXRECINPUT-1)&(off+d);
}

void resetinputp(int from)
{
	od.playernet[from].ninput=0;
	od.playernet[from].tsoff=0;
	od.playernet[from].offset=0;
}

// read and process all packets
void packetread()
{
	U32 fip,fport;
	int frm,i,len,kind,gn;
	unsigned char str[500];
	if (od.numplayers-od.numbots==1 && ol_opt.nt!=NET_BCLIENT)
		return;
	for (i=0;i<OL_MAXPLAYERS;i++)
		if (od.playernet[i].tcpclients)
			tcpfillreadbuff(od.playernet[i].tcpclients);
	while((len=getapacket(&frm,&kind,str))>=0) {
#ifdef USECON
//		con16_printf(od.ucon,"pkrd %d: from %d, '%d'\n",od.playerid,frm,kind);
#endif
		switch(kind) {
		case PKT_HERE:
			memcpy(&gn,str,4);
			logger("got a 'here' packet value %d, from %d\n",gn,frm);
			if (gn!=od.gamenum) {
				setuprematch();
			}
			if (gn>0) {
				pk_restorepstate0(frm);
				resetinputp(frm); // erase ALL input that has been recorded
//		restorepstate0(i);
//		removeinput(i,2000000000); // erase ALL input that has been recorded
			}
			if (od.numcurplayers<od.numplayers-od.numbots)
				od.numcurplayers++;
//			if (od.numcurplayers+od.botplayers==od.numplayers) {
//				od.cscoret[1]=0;
//				od.tscoret[1]=0;
//				od.cscoret[2]=0;
//				od.tscoret[2]=0;
//				od.cscoret[3]=0;
//				od.tscoret[3]=0;
//				od.numcurplayers=od.numplayers;
//			}
			break;
		case PKT_KEY:
			if (od.numcurplayers==od.numplayers-od.numbots)
				packetaddinput(frm,str[0],str[1]);//,str[1]+(str[2]<<8));
//			od.playernet[frm].input=str[0];
//			od.playernet[frm].timestamp=str[1]+(str[2]<<8);
			break;
		case PKT_REQREMATCH:
//			if (od.playerid==0)
				++od.rematchplayers;
//			else
//				errorexit("rematch can only be sent to the server");
			break;
//		case PKT_DISCONNECT:
//			od.markfordisconnect=1;
//			break;
		case PKT_GSTATE:
			checkgstate(frm,str);
			break;
		case PKT_DISCONNECT:
			GAMENEWS->printf("player %d, discon at %d",frm,opa[0].clocktickcount);
			if (!od.playernet[frm].onebotplayer) {
				od.playernet[frm].onebotplayer=1;
				++od.numdiscon;
			} 
			break;
		}
	}
	if (olracecfg.udpenable) {
		int unclen;
		C8 udppacket[MAXCUDPLEN];
		if (od.playerid==0) { // sv
			while(len=udpgetbuffdata(od.udp,udppacket,MAXCUDPLEN,&fip,&fport)) {
				if (parseudppacket(udppacket,len,&frm,&unclen)) {
//					logger("sv got a udp from '%s' %d frm %d, clen %d, unclen %d\n",ip2str(fip),fport,frm,len,unclen);
					od.playernet[frm].udpip=fip;
					od.playernet[frm].udpport=fport;
				}
			}
		} else { // cl
			while(len=udpgetbuffdata(od.udp,udppacket,MAXCUDPLEN,&fip,&fport)) {
				if (parseudppacket(udppacket,len,&frm,&unclen)) {
//					logger("cl got a udp from '%s' %d frm %d, len %d, unclen %d\n",ip2str(fip),fport,frm,len,unclen);
				}
			}
		}
	}
}

// create and write out all packets
int packetwrite() // returns allwriten
{
//	char str[50];
	int i,j;
	int aw=1;
	static int nopcnt;
	nopcnt++;
	nopcnt&=127;
	if (od.numplayers-od.numbots==1 && ol_opt.nt!=NET_BCLIENT)
		return 1;
// done only once
	if (od.playernet[0].retrytimers==-1) {
		for (i=0;i<OL_MAXPLAYERS;i++)
			od.playernet[i].retrytimers=0;
//		if (od.playerid!=i) {
//			sprintf(str,"here");
#ifdef USECON
//			con16_printf(od.ucon,"pkwt %d: to %d, '%d'\n",od.playerid,-1,PKT_HERE);
#endif
		if (!od.udp) {
			if (od.playernet[0].tcpclients)
				od.udp=udpmake(0,0);
			else
				od.udp=udpmake(0,ol_opt.port);
			if (od.udp)
				logger("make a udp packet, ip '%s', port %d\n",ip2str(od.udp->ip),od.udp->port);
		}
//		logger("sending 'here' packet %d\n",od.gamenum);
		logger("%d sent a HERE at %d, gamenum %d\n",od.playerid,opa[0].clocktickcount,od.gamenum);
//		if (od.playerid==1)
//			error("Hold!"); // test 'waiting for opponents', hold off on sending that 'here' packet
		sendapacket(od.playerid,-1,PKT_HERE,4,(char *)&od.gamenum);
		if (ol_opt.nt!=NET_BCLIENT) {
			for (i=0;i<OL_MAXPLAYERS;i++)
				if (od.playernet[i].onebotplayer)
					sendapacket(i,-1,PKT_HERE,4,(char *)&od.gamenum);
		}
//		}
	}
/*	if (od.numcurolplayers==od.numolplayers) {
		if (od.notspec) {
			if (od.playernet[od.playerid])
			if (od.playernet[].lastinputstr[0]=od.playernet[od.playerid].input;
			str[1]=0xff&od.playernet[od.playerid].timestamp;
			str[2]=od.playernet[od.playerid].timestamp>>8;
			packetputinput(sendapacket(od.playerid,-1,PKT_KEY,1,str);
		}
	} */
	if (od.numplayers-od.numbots!=od.numcurplayers && nopcnt==0) { // keepalive, do we need it?
		for (i=0;i<OL_MAXPLAYERS;i++)
			if (od.playerid!=i) {
				logger("sending 'nop' packet %d\n",od.gamenum);
				sendapacket(od.playerid,-1,PKT_NOP,0,NULL);
			}
	}
	for (i=0;i<OL_MAXPLAYERS;i++)
		if (od.playernet[i].tcpclients)
			if (!tcpsendwritebuff(od.playernet[i].tcpclients))
				aw=0;
// udp
	if (olracecfg.udpenable) {
		C8 udppacket[MAXCUDPLEN];
		int len;
		if (od.playerid==0) { // sv
			for (j=0;j<od.numcars;j++) { // from
				len=buildudppacket(j,udppacket,olracecfg.udpserversize);
				if (len) {
					for (i=1;i<od.numcars;i++) // to
						if (od.playernet[i].tcpclients && i!=j && od.playernet[i].udpip) {
							udpputbuffdata(od.udp,udppacket,len,od.playernet[i].udpip,od.playernet[i].udpport);
						}
				}
			}
		} else { // cl
			len=buildudppacket(od.playerid,udppacket,olracecfg.udpclientsize);
			if (len)
				udpputbuffdata(od.udp,udppacket,len,ol_opt.serverip,ol_opt.port);
		}
	}
// end udp
	return aw;
}


/*		udpputbuffdata(od.udp,(char *)&from,4,ol_opt.serverip,ol_opt.port);
		if (od.udp) {
			logger("udp: sending 'hi' from '%s' %d, ",ip2str(od.udp->ip),od.udp->port);
			logger("to '%s' %d\n",ip2str(ol_opt.serverip),ol_opt.port);
		}



				if (od.playernet[i].udpip) {
					udpputbuffdata(od.udp,(char *)&from,4,od.playernet[i].udpip,od.playernet[i].udpport);
#ifdef USECON
//				con16_printf(od.ucon,"sendap %d: from %d to %d: '%d\n",od.playerid,from,to,kind);
#endif
					if (od.udp) {
						logger("udp: sending 'ho' from '%s' %d, ",ip2str(od.udp->ip),od.udp->port);
						logger("to '%s' %d\n",ip2str(od.playernet[i].udpip),od.playernet[i].udpport);
					}
				}
*/
/*void getbackupinput(struct ol_playerdata *p)
{
	static int inited=0;
	char **sc;
	int nsc;
	int i=0,pn,ni;
	int inp;
	int bi;
	unsigned int ts,ts0;
	if (!inited) {
		pushandsetdir("packetlog");
		sc=loadscript("logfileb.txt",&nsc);
		popdir();
		while(i<nsc) {
			ni=od.playernet[pn].ninput;
			pn=atoi(sc[i+2]);
			od.playernet[pn].inputs[ni]=atoi(sc[i+4]);
			od.playernet[pn].timestamps[ni]=atoi(sc[i+6]);
			od.playernet[pn].ninput=ni+1;
			i+=7;
		}
		inited=1;
		freescript(sc,nsc);
	}
	pn=p->pnum;
//	if (pn ==od.playerid)
//		return; // don't need input on yourself
	bi=od.playernet[pn].ninput;
	if (!bi)
		errorexit("shouldn't be here..\n");
//		return;
	ts0=od.playernet[pn].timestamps[0];
	if ((unsigned)p->ol_clocktickcount>ts0)
		inp=0;
	else {
		bi--; // point at last timestamp
		ts=od.playernet[pn].timestamps[bi];
		if (od.playernet[pn].timestamps[bi]>p->ol_clocktickcount) {
			errorexit("shouldn't be here..\n");
	//		return;
		}
		while(1) {
			ts=od.playernet[pn].timestamps[bi];
			if (ts>=(unsigned)p->ol_clocktickcount) {
//				bi++;
				inp=od.playernet[pn].inputs[bi];
				break;
			}
			bi--;
			if (bi<0) {
				if (ts0!=(unsigned)p->ol_clocktickcount)
					errorexit("bi < 0");
				inp=od.playernet[pn].inputs[0];
			}
		}
	}
	p->pi.ol_uup=1&(inp);
	p->pi.ol_udown=1&(inp>>1);
	p->pi.ol_uright=1&(inp>>2);
	p->pi.ol_uleft=1&(inp>>3);
	p->pi.ol_urollleft=1&(inp>>4);
	p->pi.ol_urollright=1&(inp>>5);
	p->pi.uspace=1&(inp>>6);

}
*/
////////// send and recieve input packets, complete with timestamps
void getinput(struct ol_playerdata *p)
{
	int inp;
	int ni;
	int tsoff,off;
	int pn;
	if (od.numplayers-od.numbots==1 && ol_opt.nt!=NET_BCLIENT)
		return;
	if (p->carid>=od.numcars-od.numbots)
		return; // no input for bots
	if (od.playernet[p->carid].onebotplayer)
		return;
//	pn=p->carid;
	pn=p->playerid;
//	if (pn ==od.playerid)
//		return; // don't need input on yourself
	ni=od.playernet[pn].ninput;
	if (!ni) {
//		errorexit("shouldn't be here..\n");
		inp=0;
		p->pi.weapkey=0;
	} else {
		tsoff=od.playernet[pn].tsoff;
		off=od.playernet[pn].offset;
		if (p->clocktickcount<tsoff) // this should not happen..
			inp=1;
		else if (p->clocktickcount>tsoff+ni-1) {
			inp=od.playernet[pn].inputs[(MAXRECINPUT-1)&(ni-1+off)].key;
			p->pi.weapkey=od.playernet[pn].inputs[(MAXRECINPUT-1)&(ni-1+off)].weapkey;
		} else {
			inp=od.playernet[pn].inputs[(MAXRECINPUT-1)&(p->clocktickcount-tsoff+off)].key;
			p->pi.weapkey=od.playernet[pn].inputs[(MAXRECINPUT-1)&(p->clocktickcount-tsoff+off)].weapkey;
		}
	}
	p->pi.ol_uup=1&(inp);
	p->pi.ol_udown=1&(inp>>1);
	p->pi.ol_uright=1&(inp>>2);
	p->pi.ol_uleft=1&(inp>>3);
	p->pi.ol_urollleft=1&(inp>>4);
	p->pi.ol_urollright=1&(inp>>5);
	p->pi.uspace=1&(inp>>6);
//	logger("packetlog p %d k %d ",p->pnum,inp);
}
//	int ninput;
//	int linput;
//	unsigned char inputs[MAXRECINPUT];
//	unsigned short timestamps[MAXRECINPUT];

void sendinput(struct ol_playerdata *p)
{
	int inp;
	int ts;
	char str[100];
	inp=
		 p->pi.ol_uup+
		(p->pi.ol_udown<<1)+
		(p->pi.ol_uright<<2)+
		(p->pi.ol_uleft<<3)+
		(p->pi.ol_urollleft<<4)+
		(p->pi.ol_urollright<<5)+
		(p->pi.uspace<<6);
//	if (od.lastinput!=inp) {
		ts=p->clocktickcount;
//		logger("packetlog p %d k %d ts %d\n",p->pnum,inp,ts);
		if (od.numplayers-od.numbots==1)
			return;
	if (opa[0].clocktickcount==2) { // clear the waiting for opponents
		od.cscoret[1]=0;
		od.tscoret[1]=0;
		od.cscoret[2]=0;
		od.tscoret[2]=0;
		od.cscoret[3]=0;
		od.tscoret[3]=0;
	}
	if (!od.notspec)
		return;
		str[0]=inp;
//		str[1]=0;
//		str[2]=0;
		str[1]=p->pi.weapkey;
//		str[1]=0xff&ts;
//		str[2]=ts>>8;
// test udp
		if (!olracecfg.tcpdisable) {
			sendapacket(op->playerid,-1,PKT_KEY,2,str); 
//			logger("%d sent a KEY at %d,%02X,%02X\n",od.playerid,opa[0].clocktickcount,str[0],str[1]);
		}

		od.lastinput=inp;
//	}
}

int someinput(struct ol_playerdata *p,int k)	// return last valid clocktickount input ready on getinput
{
	int lts;
	int pn;
	int ninput;
	if (od.numplayers-od.numbots==1 && ol_opt.nt!=NET_BCLIENT)
		return p->clocktickcount;
	pn=p->playerid;
	if (pn==od.playerid && k)
		return p->clocktickcount; // don't need input on yourself
	if (od.playernet[p->carid].onebotplayer && k) // new 2 lines
		return p->clocktickcount;
	ninput=od.playernet[pn].ninput;
	if (ninput) {
		lts=od.playernet[pn].tsoff+od.playernet[pn].ninput-1;
		return lts;
	}
	return -1;	// 2 minutes
}

/*void saveplayerstate2(struct ol_playerdata *p,struct ol_playerdata *pk)
{
	if (p->ol_clocktickcount==0)
		return;
	logger("saving %d\n",p->ol_clocktickcount);
	*pk=*p;
}

void restoreplayerstate2(struct ol_playerdata *pk,struct ol_playerdata *p)
{
	struct soundhandle *ms;
	if (pk->ol_clocktickcount==0)
		return;
	logger("restoring %d over %d\n",pk->ol_clocktickcount,p->ol_clocktickcount);
	ms=p->ol_motorsh;
	*p=*pk;
	p->ol_motorsh=ms;
}
*/
// some redundancies
void saveplayerstate(struct ol_playerdata *p,struct playerstatepacket *pk)
{
//	if (p->clocktickcount==0)
//		return;
// boxai 2009
	pk->ncoins_caught=p->ncoins_caught;
	pk->ntrophies_caught=p->ntrophies_caught;
	pk->nsuperenergies_caught=p->nsuperenergies_caught;
	pk->wrongway2=p->wrongway2;
	pk->ol_crashresetbox=p->ol_crashresetbox;
	pk->pos=p->pos;
	pk->carvel=p->carvel;
	pk->carang=p->carang;
	pk->carvelang=p->carvelang;
	pk->shocklen[0]=p->shocklen[0];
	pk->shocklen[1]=p->shocklen[1];
	pk->shocklen[2]=p->shocklen[2];
	pk->shocklen[3]=p->shocklen[3];
/*	pk->wheelang[0]=p->wheelang[0];
	pk->wheelang[1]=p->wheelang[1];
	pk->wheelang[2]=p->wheelang[2];
	pk->wheelang[3]=p->wheelang[3]; */
	pk->wheelvel[0]=p->wheelvel[0];
	pk->wheelvel[1]=p->wheelvel[1];
	pk->wheelvel[2]=p->wheelvel[2];
	pk->wheelvel[3]=p->wheelvel[3];
	pk->wheelvel[4]=p->wheelvel[4];
	pk->wheelvel[5]=p->wheelvel[5]; 
	pk->wheelyaw=p->wheelyaw;
	pk->ol_lastairtime=p->ol_lastairtime;
//-------- 116

	pk->ol_airtime=p->ol_airtime;
	pk->ol_cbairtime=p->ol_cbairtime;
	pk->ol_littleg=p->ol_littleg;
	pk->ol_validroadnorm=p->ol_validroadnorm;
//--------- 12

	pk->ol_carnorm=p->ol_carnorm;
	pk->ol_cardir=p->ol_cardir;
	pk->ol_carvelnorm=p->ol_carvelnorm;
	pk->ol_carvelmag=p->ol_carvelmag;
	pk->ol_shockmag=p->ol_shockmag;
	pk->ol_shocknorm=p->ol_shocknorm;
	pk->ol_lastroadnorm=p->ol_lastroadnorm;
	pk->ol_accelspin=p->ol_accelspin;
	pk->ol_drivemode=p->ol_drivemode;
	pk->ol_hifrictime=p->ol_hifrictime;
	pk->ol_ulastloc=p->ol_ulastloc;
	pk->ol_carbodymag=p->ol_carbodymag;
// -------  96
 
	pk->ol_ufliptime=p->ol_ufliptime;
	pk->ol_doacrashreset=p->ol_doacrashreset;
	pk->ol_flymode=p->ol_flymode;
	pk->ol_utotalcrashes=p->ol_utotalcrashes;
	pk->ol_utotalstunts=p->ol_utotalstunts;
	pk->ol_startstunt=p->ol_startstunt;
	pk->ol_carstuntrpy=p->ol_carstuntrpy;
// -------- 36

	pk->ol_steertime=p->ol_steertime;
	pk->ol_uextraheading=p->ol_uextraheading;
//	pk->ol_lastkludge=p->ol_lastkludge;
	pk->ol_cantdrive=p->ol_cantdrive;
	pk->ol_crashresetloc=p->ol_crashresetloc;
	pk->ol_crashresetrot=p->ol_crashresetrot;
	pk->ol_crashresetframe=p->ol_crashresetframe;
	pk->ol_uready=p->ol_uready;
//	pk->ol_cantstartdrive=p->ol_cantstartdrive;
	pk->ol_ulastpos=p->ol_ulastpos;
// boxai 2009, any more?
	pk->boxchk=p->boxchk;
	pk->aiturnval=p->aiturnval;

	pk->ol_carboost=p->ol_carboost;
	pk->ol_landspot=p->ol_landspot;
// --------- 68 

	pk->ol_rollpie[0]=p->ol_rollpie[0];
	pk->ol_rollpie[1]=p->ol_rollpie[1];
	pk->ol_rollpie[2]=p->ol_rollpie[2];
	pk->ol_rollpie[3]=p->ol_rollpie[3];
	pk->ol_pitchpie[0]=p->ol_pitchpie[0];
	pk->ol_pitchpie[1]=p->ol_pitchpie[1];
	pk->ol_pitchpie[2]=p->ol_pitchpie[2];
	pk->ol_pitchpie[3]=p->ol_pitchpie[3];
	pk->ol_yawpie[0]=p->ol_yawpie[0];
	pk->ol_yawpie[1]=p->ol_yawpie[1];
	pk->ol_yawpie[2]=p->ol_yawpie[2];
	pk->ol_yawpie[3]=p->ol_yawpie[3];
	pk->ol_rollpiesum=p->ol_rollpiesum;
	pk->ol_pitchpiesum=p->ol_pitchpiesum;
	pk->ol_yawpiesum=p->ol_yawpiesum;
	pk->ol_cantaddroll=p->ol_cantaddroll;
	pk->ol_cantaddpitch=p->ol_cantaddpitch;
	pk->ol_cantaddyaw=p->ol_cantaddyaw;
	pk->ol_udidroll=p->ol_udidroll;
	pk->ol_udidyaw=p->ol_udidyaw;
	pk->ol_udidpitch=p->ol_udidpitch;
	pk->piol_uup=p->pi.ol_uup;
	pk->piol_udown=p->pi.ol_udown;
	pk->weapkey=p->pi.weapkey;
//----------- 88

//32  struct pinput pi	// should be 1
//---------------- 1

	pk->ol_ucarrotvel=p->ol_ucarrotvel;
	pk->ol_ttp=p->ol_ttp;
	pk->dofinish=p->dofinish; 
//	pk->cntl=p->cntl;
	pk->seekframe=p->seekframe;
	pk->botbigscan=0;
	pk->chklap=p->chklap;
	pk->chkpnt=p->chkpnt;
	pk->stuntgoals=p->stuntgoals;
	pk->curstuntangs=p->curstuntangs;
//---------------- 48 

	pk->doaistunt[0]=p->doaistunt[0];
	pk->doaistunt[1]=p->doaistunt[1];
	pk->doaistunt[2]=p->doaistunt[2];
//	pk->lapready=p->lapready;
//	pk->lapsdone=p->lapsdone;
	pk->finplace=p->finplace;
	pk->cartspassed=p->cartspassed;
	pk->stuckwatchdog=p->stuckwatchdog;
	pk->clocktickcount=p->clocktickcount;
/*	pk->curweap=p->curweap;
	pk->lastcurweap=p->lastcurweap;
	pk->weapframe=p->weapframe;
	pk->extrashieldsup=p->extrashieldsup;
//	memcpy(&pk->missileinfos,&p->missileinfos,sizeof(p->missileinfos));
	pk->enableicrms=p->enableicrms;
	pk->icrmsparm=p->icrmsparm;
	pk->randomizeron=p->randomizeron;
	pk->weapbooston=p->weapbooston;
	pk->inoil=p->inoil;
	pk->inecto=p->inecto;
	memcpy(pk->vweapstolen,p->vweapstolen,sizeof(p->vweapstolen));
//	memcpy(pk->weapstolenfrom=p->weapstolenfrom,sizeof(p->weapstolenfrom));
	pk->tweapstolenfrom=p->tweapstolenfrom;
	pk->tweapstolenslot=p->tweapstolenslot;
	memcpy(pk->energies,p->energies,sizeof(p->energies)); */
	pk->xcurweapkind=p->xcurweapkind;
	pk->xcurweapslot=p->xcurweapslot;
	pk->xcurweapstate=p->xcurweapstate;
	pk->lastactiveweapslot=p->lastactiveweapslot;
	pk->lastspecialweapslot=p->lastspecialweapslot;
	pk->curweapvar=p->curweapvar;
	pk->xweapframe=p->xweapframe;
	pk->extrashieldsup=p->extrashieldsup;
//	pk->vicrmsparm=p->vicrmsparm;
	pk->venableicrms=p->venableicrms;
	pk->tweapbooston=p->tweapbooston;
	pk->tshieldbooston=p->tshieldbooston;
	pk->vinoil=p->vinoil;
	pk->vinemb=p->vinemb;
	pk->vinecto=p->vinecto;
	pk->vinleech=p->vinleech;
	pk->vinflash=p->vinflash;
	pk->vunderbigtire=p->vunderbigtire;
	pk->vinbuzzspin=p->vinbuzzspin;

	pk->vinsonic=p->vinsonic;
	pk->vinthermo=p->vinthermo;
	pk->vinfusion=p->vinfusion;
	pk->inmagnet=p->inmagnet;
	pk->inghook=p->inghook;
	pk->thooknum=p->thooknum;
	pk->vhooknum=p->vhooknum;
	pk->magnum=p->magnum;

	memcpy(pk->vvweapstolen,p->vvweapstolen,sizeof(pk->vvweapstolen));
	pk->ttweapstolenslot=p->ttweapstolenslot;
	pk->ttweapstolenfrom=p->ttweapstolenfrom;

	pk->numflags = p->ol_numflags;

	memcpy(pk->tenergies,p->tenergies,sizeof(pk->tenergies));
//	memcpy(pk->lenergies,p->lenergies,sizeof(pk->lenergies));

	pk->shockextra=p->shockextra;
	pk->extrabigtires=p->extrabigtires;
	pk->extrapouncer=p->extrapouncer;
	pk->seekframe2=p->seekframe2;
	pk->laps2=p->laps2;
#if 1
	pk->pos=p->pos;
	pk->carvel=p->carvel;
	pk->carang=p->carang;
	pk->carvelang=p->carvelang;
	pk->shocklen[0]=p->shocklen[0];
	pk->shocklen[1]=p->shocklen[1];
	pk->shocklen[2]=p->shocklen[2];
	pk->shocklen[3]=p->shocklen[3];
/*	pk->wheelang[0]=p->wheelang[0];
	pk->wheelang[1]=p->wheelang[1];
	pk->wheelang[2]=p->wheelang[2];
	pk->wheelang[3]=p->wheelang[3]; */
	pk->wheelvel[0]=p->wheelvel[0];
	pk->wheelvel[1]=p->wheelvel[1];
	pk->wheelvel[2]=p->wheelvel[2];
	pk->wheelvel[3]=p->wheelvel[3];
	pk->wheelvel[4]=p->wheelvel[4];
	pk->wheelvel[5]=p->wheelvel[5];
	pk->wheelyaw=p->wheelyaw;
	pk->ol_lastairtime=p->ol_lastairtime;
//-------- 116

	pk->ol_airtime=p->ol_airtime;
	pk->ol_cbairtime=p->ol_cbairtime;
	pk->ol_littleg=p->ol_littleg;
	pk->ol_validroadnorm=p->ol_validroadnorm;
//--------- 12

	pk->ol_carnorm=p->ol_carnorm;
	pk->ol_cardir=p->ol_cardir;
	pk->ol_carvelnorm=p->ol_carvelnorm;
	pk->ol_carvelmag=p->ol_carvelmag;
	pk->ol_shockmag=p->ol_shockmag;
	pk->ol_shocknorm=p->ol_shocknorm;
	pk->ol_lastroadnorm=p->ol_lastroadnorm;
	pk->ol_accelspin=p->ol_accelspin;
	pk->ol_drivemode=p->ol_drivemode;
	pk->ol_hifrictime=p->ol_hifrictime;
	pk->ol_ulastloc=p->ol_ulastloc;
	pk->ol_carbodymag=p->ol_carbodymag;
// -------  96
 
	pk->ol_ufliptime=p->ol_ufliptime;
	pk->ol_doacrashreset=p->ol_doacrashreset;
	pk->ol_flymode=p->ol_flymode;
	pk->ol_utotalcrashes=p->ol_utotalcrashes;
	pk->ol_utotalstunts=p->ol_utotalstunts;
	pk->ol_startstunt=p->ol_startstunt;
	pk->ol_carstuntrpy=p->ol_carstuntrpy;
// -------- 36

	pk->ol_steertime=p->ol_steertime;
	pk->ol_uextraheading=p->ol_uextraheading;
//	pk->ol_lastkludge=p->ol_lastkludge;
	pk->ol_cantdrive=p->ol_cantdrive;
	pk->ol_crashresetloc=p->ol_crashresetloc;
	pk->ol_crashresetbox=p->ol_crashresetbox;
	pk->ol_crashresetrot=p->ol_crashresetrot;
	pk->ol_crashresetframe=p->ol_crashresetframe;
	pk->ol_uready=p->ol_uready;
//	pk->ol_cantstartdrive=p->ol_cantstartdrive;
	pk->ol_ulastpos=p->ol_ulastpos;
	pk->ol_carboost=p->ol_carboost;
	pk->ol_landspot=p->ol_landspot;
// --------- 68 

	pk->ol_rollpie[0]=p->ol_rollpie[0];
	pk->ol_rollpie[1]=p->ol_rollpie[1];
	pk->ol_rollpie[2]=p->ol_rollpie[2];
	pk->ol_rollpie[3]=p->ol_rollpie[3];
	pk->ol_pitchpie[0]=p->ol_pitchpie[0];
	pk->ol_pitchpie[1]=p->ol_pitchpie[1];
	pk->ol_pitchpie[2]=p->ol_pitchpie[2];
	pk->ol_pitchpie[3]=p->ol_pitchpie[3];
	pk->ol_yawpie[0]=p->ol_yawpie[0];
	pk->ol_yawpie[1]=p->ol_yawpie[1];
	pk->ol_yawpie[2]=p->ol_yawpie[2];
	pk->ol_yawpie[3]=p->ol_yawpie[3];
	pk->ol_rollpiesum=p->ol_rollpiesum;
	pk->ol_pitchpiesum=p->ol_pitchpiesum;
	pk->ol_yawpiesum=p->ol_yawpiesum;
	pk->ol_cantaddroll=p->ol_cantaddroll;
	pk->ol_cantaddpitch=p->ol_cantaddpitch;
	pk->ol_cantaddyaw=p->ol_cantaddyaw;
	pk->ol_udidroll=p->ol_udidroll;
	pk->ol_udidyaw=p->ol_udidyaw;
	pk->ol_udidpitch=p->ol_udidpitch;
	pk->piol_uup=p->pi.ol_uup;
	pk->piol_udown=p->pi.ol_udown;
	pk->weapkey=p->pi.weapkey;
//----------- 88

//32  struct pinput pi	// should be 1
//---------------- 1

	pk->ol_ucarrotvel=p->ol_ucarrotvel;
	pk->ol_ttp=p->ol_ttp;
	pk->dofinish=p->dofinish; 
//	pk->cntl=p->cntl;
	pk->seekframe=p->seekframe;
//	pk->botbigscan=p->botbigscan;
	pk->botbigscan=0;
	pk->chklap=p->chklap;
	pk->chkpnt=p->chkpnt;
	pk->stuntgoals=p->stuntgoals;
	pk->curstuntangs=p->curstuntangs;
//---------------- 48 

	pk->doaistunt[0]=p->doaistunt[0];
	pk->doaistunt[1]=p->doaistunt[1];
	pk->doaistunt[2]=p->doaistunt[2];
//	pk->lapready=p->lapready;
//	pk->lapsdone=p->lapsdone;
	pk->finplace=p->finplace;
	pk->cartspassed=p->cartspassed;
	pk->stuckwatchdog=p->stuckwatchdog;
	pk->clocktickcount=p->clocktickcount;
/*	pk->curweap=p->curweap;
	pk->lastcurweap=p->lastcurweap;
	pk->weapframe=p->weapframe;
	pk->extrashieldsup=p->extrashieldsup;
//	memcpy(&pk->missileinfos,&p->missileinfos,sizeof(p->missileinfos));
	pk->enableicrms=p->enableicrms;
	pk->icrmsparm=p->icrmsparm;
	pk->randomizeron=p->randomizeron;
	pk->weapbooston=p->weapbooston;
	pk->inoil=p->inoil;
	pk->inecto=p->inecto;
	memcpy(pk->vweapstolen,p->vweapstolen,sizeof(p->vweapstolen));
//	memcpy(pk->weapstolenfrom=p->weapstolenfrom,sizeof(p->weapstolenfrom));
	pk->tweapstolenfrom=p->tweapstolenfrom;
	pk->tweapstolenslot=p->tweapstolenslot;
	memcpy(pk->energies,p->energies,sizeof(p->energies)); */
	pk->xcurweapkind=p->xcurweapkind;
	pk->xcurweapslot=p->xcurweapslot;
	pk->xcurweapstate=p->xcurweapstate;
	pk->lastactiveweapslot=p->lastactiveweapslot;
	pk->lastspecialweapslot=p->lastspecialweapslot;
	pk->curweapvar=p->curweapvar;
	pk->xweapframe=p->xweapframe;
	pk->extrashieldsup=p->extrashieldsup;
//	pk->vicrmsparm=p->vicrmsparm;
	pk->venableicrms=p->venableicrms;
	pk->tweapbooston=p->tweapbooston;
	pk->tshieldbooston=p->tshieldbooston;
	pk->vinoil=p->vinoil;
	pk->vinemb=p->vinemb;
	pk->vinecto=p->vinecto;
	pk->vinleech=p->vinleech;
	pk->vinflash=p->vinflash;
	pk->vunderbigtire=p->vunderbigtire;
	pk->vinbuzzspin=p->vinbuzzspin;

	pk->vinsonic=p->vinsonic;
	pk->vinthermo=p->vinthermo;
	pk->vinfusion=p->vinfusion;
	pk->inmagnet=p->inmagnet;
	pk->inghook=p->inghook;
	pk->thooknum=p->thooknum;
	pk->vhooknum=p->vhooknum;
	pk->magnum=p->magnum;

	memcpy(pk->vvweapstolen,p->vvweapstolen,sizeof(pk->vvweapstolen));
	pk->ttweapstolenslot=p->ttweapstolenslot;
	pk->ttweapstolenfrom=p->ttweapstolenfrom;

	pk->numflags = p->ol_numflags;


	pk->shockextra=p->shockextra;
	pk->extrabigtires=p->extrabigtires;
	pk->extrapouncer=p->extrapouncer;
	pk->seekframe2=p->seekframe2;
	pk->laps2=p->laps2;
	pk->boxchk=p->boxchk;
	pk->speedup=p->speedup;
	pk->slowdown=p->slowdown;
	pk->onice=p->onice;
	pk->quicksand=p->quicksand;
#endif
}

//void syncweap(struct playerstatepacket *pk,struct ol_playerdata *p)
//{
//}
// some redundancies
void restoreplayerstate(struct playerstatepacket *pk,struct ol_playerdata *p)
{
//	if (pk->ol_clocktickcount==0)
//		return;
	p->ncoins_caught=pk->ncoins_caught;
	p->ntrophies_caught=pk->ntrophies_caught;
	p->nsuperenergies_caught=pk->nsuperenergies_caught;
	p->wrongway2=pk->wrongway2;
	p->ol_crashresetbox=pk->ol_crashresetbox;
	p->pos=pk->pos;
	p->carvel=pk->carvel;
	p->carang=pk->carang;
	p->carvelang=pk->carvelang;
	p->shocklen[0]=pk->shocklen[0];
	p->shocklen[1]=pk->shocklen[1];
	p->shocklen[2]=pk->shocklen[2];
	p->shocklen[3]=pk->shocklen[3];
/*	p->wheelang[0]=pk->wheelang[0];
	p->wheelang[1]=pk->wheelang[1];
	p->wheelang[2]=pk->wheelang[2];
	p->wheelang[3]=pk->wheelang[3]; */
	p->wheelvel[0]=pk->wheelvel[0];
	p->wheelvel[1]=pk->wheelvel[1];
	p->wheelvel[2]=pk->wheelvel[2];
	p->wheelvel[3]=pk->wheelvel[3];
	p->wheelvel[4]=pk->wheelvel[4];
	p->wheelvel[5]=pk->wheelvel[5];
	p->wheelyaw=pk->wheelyaw;
	p->ol_lastairtime=pk->ol_lastairtime;
//-------- 116

	p->ol_airtime=pk->ol_airtime;
	p->ol_cbairtime=pk->ol_cbairtime;
	p->ol_littleg=pk->ol_littleg;
	p->ol_validroadnorm=pk->ol_validroadnorm;
//--------- 12

	p->ol_carnorm=pk->ol_carnorm;
	p->ol_cardir=pk->ol_cardir;
	p->ol_carvelnorm=pk->ol_carvelnorm;
	p->ol_carvelmag=pk->ol_carvelmag;
	p->ol_shockmag=pk->ol_shockmag;
	p->ol_shocknorm=pk->ol_shocknorm;
	p->ol_lastroadnorm=pk->ol_lastroadnorm;
	p->ol_accelspin=pk->ol_accelspin;
	p->ol_drivemode=pk->ol_drivemode;
	p->ol_hifrictime=pk->ol_hifrictime;
	p->ol_ulastloc=pk->ol_ulastloc;
	p->ol_carbodymag=pk->ol_carbodymag;
// -------  96
 
	p->ol_ufliptime=pk->ol_ufliptime;
	p->ol_doacrashreset=pk->ol_doacrashreset;
	p->ol_flymode=pk->ol_flymode;
	p->ol_utotalcrashes=pk->ol_utotalcrashes;
	p->ol_utotalstunts=pk->ol_utotalstunts;
	p->ol_startstunt=pk->ol_startstunt;
	p->ol_carstuntrpy=pk->ol_carstuntrpy;
// -------- 36

	p->ol_steertime=pk->ol_steertime;
	p->ol_uextraheading=pk->ol_uextraheading;
//	p->ol_lastkludge=pk->ol_lastkludge;
	p->ol_cantdrive=pk->ol_cantdrive;
	p->ol_crashresetloc=pk->ol_crashresetloc;
	p->ol_crashresetrot=pk->ol_crashresetrot;
	p->ol_crashresetframe=pk->ol_crashresetframe;
	p->ol_uready=pk->ol_uready;
//	p->cantstartdrive=pk->cantstartdrive;
	p->ol_ulastpos=pk->ol_ulastpos;
// 2009 boxai any more?
	p->boxchk=pk->boxchk;
	p->aiturnval=pk->aiturnval;

	p->ol_carboost=pk->ol_carboost;
	p->ol_landspot=pk->ol_landspot;
// --------- 68 

	p->ol_rollpie[0]=pk->ol_rollpie[0];
	p->ol_rollpie[1]=pk->ol_rollpie[1];
	p->ol_rollpie[2]=pk->ol_rollpie[2];
	p->ol_rollpie[3]=pk->ol_rollpie[3];
	p->ol_pitchpie[0]=pk->ol_pitchpie[0];
	p->ol_pitchpie[1]=pk->ol_pitchpie[1];
	p->ol_pitchpie[2]=pk->ol_pitchpie[2];
	p->ol_pitchpie[3]=pk->ol_pitchpie[3];
	p->ol_yawpie[0]=pk->ol_yawpie[0];
	p->ol_yawpie[1]=pk->ol_yawpie[1];
	p->ol_yawpie[2]=pk->ol_yawpie[2];
	p->ol_yawpie[3]=pk->ol_yawpie[3];
	p->ol_rollpiesum=pk->ol_rollpiesum;
	p->ol_pitchpiesum=pk->ol_pitchpiesum;
	p->ol_yawpiesum=pk->ol_yawpiesum;
	p->ol_cantaddroll=pk->ol_cantaddroll;
	p->ol_cantaddpitch=pk->ol_cantaddpitch;
	p->ol_cantaddyaw=pk->ol_cantaddyaw;
	p->ol_udidroll=pk->ol_udidroll;
	p->ol_udidyaw=pk->ol_udidyaw;
	p->ol_udidpitch=pk->ol_udidpitch;
	p->pi.ol_uup=pk->piol_uup;
	p->pi.ol_udown=pk->piol_udown;
	p->pi.weapkey=pk->weapkey;
//----------- 88

//32  struct pinput pi	// should be 1
//---------------- 1

	p->ol_ucarrotvel=pk->ol_ucarrotvel;
	p->ol_ttp=pk->ol_ttp;
	p->dofinish=pk->dofinish; 
//	p->cntl=pk->cntl;
	p->seekframe=pk->seekframe;
//	p->botbigscan=pk->botbigscan;
	p->chklap=pk->chklap;
	p->chkpnt=pk->chkpnt;
	p->stuntgoals=pk->stuntgoals;
	p->curstuntangs=pk->curstuntangs;
//---------------- 48 

	p->doaistunt[0]=pk->doaistunt[0];
	p->doaistunt[1]=pk->doaistunt[1];
	p->doaistunt[2]=pk->doaistunt[2];
//	p->lapready=pk->lapready;
//	p->lapsdone=pk->lapsdone;
	p->finplace=pk->finplace;
	p->cartspassed=pk->cartspassed;
	p->stuckwatchdog=pk->stuckwatchdog;
	p->clocktickcount=pk->clocktickcount;
//	syncweap(p,pk);
/*	p->curweap=pk->curweap;
	p->lastcurweap=pk->lastcurweap;
	p->weapframe=pk->weapframe;
	p->extrashieldsup=pk->extrashieldsup;
//	memcpy(&p->missileinfos,&pk->missileinfos,sizeof(p->missileinfos));
	p->enableicrms=pk->enableicrms;
	p->icrmsparm=pk->icrmsparm;
	p->randomizeron=pk->randomizeron;
	p->weapbooston=pk->weapbooston;
	p->inoil=pk->inoil;
	p->inecto=pk->inecto;
	memcpy(p->vweapstolen,pk->vweapstolen,sizeof(pk->vweapstolen));
//	memcpy(p->weapstolenfrom=pk->weapstolenfrom,sizeof(pk->weapstolenfrom));
	p->tweapstolenfrom=pk->tweapstolenfrom;
	p->tweapstolenslot=pk->tweapstolenslot;
	memcpy(p->energies,pk->energies,sizeof(pk->energies)); */

	p->xcurweapkind=pk->xcurweapkind;
	p->xcurweapslot=pk->xcurweapslot;
	p->xcurweapstate=pk->xcurweapstate;
	p->lastactiveweapslot=pk->lastactiveweapslot;
	p->lastspecialweapslot=pk->lastspecialweapslot;
	p->curweapvar=pk->curweapvar;
	p->xweapframe=pk->xweapframe;
	p->extrashieldsup=pk->extrashieldsup;
//	p->vicrmsparm=pk->vicrmsparm;
	p->venableicrms=pk->venableicrms;
	p->tweapbooston=pk->tweapbooston;
	p->tshieldbooston=pk->tshieldbooston;
	p->vinoil=pk->vinoil;
	p->vinemb=pk->vinemb;
	p->vinecto=pk->vinecto;
	p->vinleech=pk->vinleech;
	p->vinflash=pk->vinflash;
	p->vunderbigtire=pk->vunderbigtire;
	p->vinbuzzspin=pk->vinbuzzspin;

	p->vinsonic=pk->vinsonic;
	p->vinthermo=pk->vinthermo;
	p->vinfusion=pk->vinfusion;
	p->inmagnet=pk->inmagnet;
	p->inghook=pk->inghook;
	p->thooknum=pk->thooknum;
	p->vhooknum=pk->vhooknum;
	p->magnum=pk->magnum;

	memcpy(p->vvweapstolen,pk->vvweapstolen,sizeof(pk->vvweapstolen));
	p->ttweapstolenslot=pk->ttweapstolenslot;
	p->ttweapstolenfrom=pk->ttweapstolenfrom;

	p->ol_numflags = pk->numflags;

	memcpy(p->tenergies,pk->tenergies,sizeof(pk->tenergies));
//	memcpy(p->lenergies,pk->lenergies,sizeof(pk->lenergies));

	p->shockextra=pk->shockextra;
	p->extrabigtires=pk->extrabigtires;
	p->extrapouncer=pk->extrapouncer;
	p->seekframe2=pk->seekframe2;
	p->laps2=pk->laps2;
#if 1
	p->pos=pk->pos;
	p->carvel=pk->carvel;
	p->carang=pk->carang;
	p->carvelang=pk->carvelang;
	p->shocklen[0]=pk->shocklen[0];
	p->shocklen[1]=pk->shocklen[1];
	p->shocklen[2]=pk->shocklen[2];
	p->shocklen[3]=pk->shocklen[3];
/*	p->wheelang[0]=pk->wheelang[0];
	p->wheelang[1]=pk->wheelang[1];
	p->wheelang[2]=pk->wheelang[2];
	p->wheelang[3]=pk->wheelang[3]; */
	p->wheelvel[0]=pk->wheelvel[0];
	p->wheelvel[1]=pk->wheelvel[1];
	p->wheelvel[2]=pk->wheelvel[2];
	p->wheelvel[3]=pk->wheelvel[3];
	p->wheelvel[4]=pk->wheelvel[4];
	p->wheelvel[5]=pk->wheelvel[5];
	p->wheelyaw=pk->wheelyaw;
	p->ol_lastairtime=pk->ol_lastairtime;
//-------- 116

	p->ol_airtime=pk->ol_airtime;
	p->ol_cbairtime=pk->ol_cbairtime;
	p->ol_littleg=pk->ol_littleg;
	p->ol_validroadnorm=pk->ol_validroadnorm;
//--------- 12

	p->ol_carnorm=pk->ol_carnorm;
	p->ol_cardir=pk->ol_cardir;
	p->ol_carvelnorm=pk->ol_carvelnorm;
	p->ol_carvelmag=pk->ol_carvelmag;
	p->ol_shockmag=pk->ol_shockmag;
	p->ol_shocknorm=pk->ol_shocknorm;
	p->ol_lastroadnorm=pk->ol_lastroadnorm;
	p->ol_accelspin=pk->ol_accelspin;
	p->ol_drivemode=pk->ol_drivemode;
	p->ol_hifrictime=pk->ol_hifrictime;
	p->ol_ulastloc=pk->ol_ulastloc;
	p->ol_carbodymag=pk->ol_carbodymag;
// -------  96
 
	p->ol_ufliptime=pk->ol_ufliptime;
	p->ol_doacrashreset=pk->ol_doacrashreset;
	p->ol_flymode=pk->ol_flymode;
	p->ol_utotalcrashes=pk->ol_utotalcrashes;
	p->ol_utotalstunts=pk->ol_utotalstunts;
	p->ol_startstunt=pk->ol_startstunt;
	p->ol_carstuntrpy=pk->ol_carstuntrpy;
// -------- 36

	p->ol_steertime=pk->ol_steertime;
	p->ol_uextraheading=pk->ol_uextraheading;
//	p->ol_lastkludge=pk->ol_lastkludge;
	p->ol_cantdrive=pk->ol_cantdrive;
	p->ol_crashresetloc=pk->ol_crashresetloc;
	p->ol_crashresetbox=pk->ol_crashresetbox;
	p->ol_crashresetrot=pk->ol_crashresetrot;
	p->ol_crashresetframe=pk->ol_crashresetframe;
	p->ol_uready=pk->ol_uready;
//	p->cantstartdrive=pk->cantstartdrive;
	p->ol_ulastpos=pk->ol_ulastpos;
	p->ol_carboost=pk->ol_carboost;
	p->ol_landspot=pk->ol_landspot;
// --------- 68 

	p->ol_rollpie[0]=pk->ol_rollpie[0];
	p->ol_rollpie[1]=pk->ol_rollpie[1];
	p->ol_rollpie[2]=pk->ol_rollpie[2];
	p->ol_rollpie[3]=pk->ol_rollpie[3];
	p->ol_pitchpie[0]=pk->ol_pitchpie[0];
	p->ol_pitchpie[1]=pk->ol_pitchpie[1];
	p->ol_pitchpie[2]=pk->ol_pitchpie[2];
	p->ol_pitchpie[3]=pk->ol_pitchpie[3];
	p->ol_yawpie[0]=pk->ol_yawpie[0];
	p->ol_yawpie[1]=pk->ol_yawpie[1];
	p->ol_yawpie[2]=pk->ol_yawpie[2];
	p->ol_yawpie[3]=pk->ol_yawpie[3];
	p->ol_rollpiesum=pk->ol_rollpiesum;
	p->ol_pitchpiesum=pk->ol_pitchpiesum;
	p->ol_yawpiesum=pk->ol_yawpiesum;
	p->ol_cantaddroll=pk->ol_cantaddroll;
	p->ol_cantaddpitch=pk->ol_cantaddpitch;
	p->ol_cantaddyaw=pk->ol_cantaddyaw;
	p->ol_udidroll=pk->ol_udidroll;
	p->ol_udidyaw=pk->ol_udidyaw;
	p->ol_udidpitch=pk->ol_udidpitch;
	p->pi.ol_uup=pk->piol_uup;
	p->pi.ol_udown=pk->piol_udown;
	p->pi.weapkey=pk->weapkey;
//----------- 88

//32  struct pinput pi	// should be 1
//---------------- 1

	p->ol_ucarrotvel=pk->ol_ucarrotvel;
	p->ol_ttp=pk->ol_ttp;
	p->dofinish=pk->dofinish; 
//	p->cntl=pk->cntl;
	p->seekframe=pk->seekframe;
//	p->botbigscan=pk->botbigscan;
	p->chklap=pk->chklap;
	p->chkpnt=pk->chkpnt;
	p->stuntgoals=pk->stuntgoals;
	p->curstuntangs=pk->curstuntangs;
//---------------- 48 

	p->doaistunt[0]=pk->doaistunt[0];
	p->doaistunt[1]=pk->doaistunt[1];
	p->doaistunt[2]=pk->doaistunt[2];
//	p->lapready=pk->lapready;
//	p->lapsdone=pk->lapsdone;
	p->finplace=pk->finplace;
	p->cartspassed=pk->cartspassed;
	p->stuckwatchdog=pk->stuckwatchdog;
	p->clocktickcount=pk->clocktickcount;
//	syncweap(p,pk);
/*	p->curweap=pk->curweap;
	p->lastcurweap=pk->lastcurweap;
	p->weapframe=pk->weapframe;
	p->extrashieldsup=pk->extrashieldsup;
//	memcpy(&p->missileinfos,&pk->missileinfos,sizeof(p->missileinfos));
	p->enableicrms=pk->enableicrms;
	p->icrmsparm=pk->icrmsparm;
	p->randomizeron=pk->randomizeron;
	p->weapbooston=pk->weapbooston;
	p->inoil=pk->inoil;
	p->inecto=pk->inecto;
	memcpy(p->vweapstolen,pk->vweapstolen,sizeof(pk->vweapstolen));
//	memcpy(p->weapstolenfrom=pk->weapstolenfrom,sizeof(pk->weapstolenfrom));
	p->tweapstolenfrom=pk->tweapstolenfrom;
	p->tweapstolenslot=pk->tweapstolenslot;
	memcpy(p->energies,pk->energies,sizeof(pk->energies)); */

	p->xcurweapkind=pk->xcurweapkind;
	p->xcurweapslot=pk->xcurweapslot;
	p->xcurweapstate=pk->xcurweapstate;
	p->lastactiveweapslot=pk->lastactiveweapslot;
	p->lastspecialweapslot=pk->lastspecialweapslot;
	p->curweapvar=pk->curweapvar;
	p->xweapframe=pk->xweapframe;
	p->extrashieldsup=pk->extrashieldsup;
//	p->vicrmsparm=pk->vicrmsparm;
	p->venableicrms=pk->venableicrms;
	p->tweapbooston=pk->tweapbooston;
	p->tshieldbooston=pk->tshieldbooston;
	p->vinoil=pk->vinoil;
	p->vinemb=pk->vinemb;
	p->vinecto=pk->vinecto;
	p->vinleech=pk->vinleech;
	p->vinflash=pk->vinflash;
	p->vunderbigtire=pk->vunderbigtire;
	p->vinbuzzspin=pk->vinbuzzspin;

	p->vinsonic=pk->vinsonic;
	p->vinthermo=pk->vinthermo;
	p->vinfusion=pk->vinfusion;
	p->inmagnet=pk->inmagnet;
	p->inghook=pk->inghook;
	p->thooknum=pk->thooknum;
	p->vhooknum=pk->vhooknum;
	p->magnum=pk->magnum;

	memcpy(p->vvweapstolen,pk->vvweapstolen,sizeof(pk->vvweapstolen));
	p->ttweapstolenslot=pk->ttweapstolenslot;
	p->ttweapstolenfrom=pk->ttweapstolenfrom;

	p->ol_numflags = pk->numflags;

	p->shockextra=pk->shockextra;
	p->extrabigtires=pk->extrabigtires;
	p->extrapouncer=pk->extrapouncer;
	p->seekframe2=pk->seekframe2;
	p->laps2=pk->laps2;
	p->boxchk=pk->boxchk;
	p->aiturnval=pk->aiturnval;

//	p->ol_nulightnings=pk->ol_nulightnings;
//	p->ol_lightningframe=pk->ol_lightningframe;
	p->speedup=pk->speedup;
	p->slowdown=pk->slowdown;
	p->onice=pk->onice;
	p->quicksand=pk->quicksand;
#endif
}

void saveglobalstate(struct globalstate *g)
{
	memcpy(g->weapflys,od.weapflys,sizeof(od.weapflys));
	//	g->dum=od.bigmove;
	g->flagidx = od.ol_flagidx;
	g->ncolltrees=l2r_ncolltrees;
	memcpy(g->colltrees,l2r_colltrees,sizeof(l2r_colltrees));
	memcpy(g->coins_caught,od.coins_caught,sizeof(od.coins_caught));
	memcpy(g->trophies_caught,od.trophies_caught,sizeof(od.trophies_caught));
	memcpy(g->superenergies_caught,od.superenergies_caught,sizeof(od.superenergies_caught));
}

void restoreglobalstate(struct globalstate *g)
{
	od.ol_flagidx = g->flagidx;
	l2r_ncolltrees=g->ncolltrees;
	memcpy(l2r_colltrees,g->colltrees,sizeof(l2r_colltrees));
//	od.bigmove=g->dum;
	memcpy(od.weapflys,g->weapflys,sizeof(od.weapflys));
	memcpy(od.coins_caught,g->coins_caught,sizeof(od.coins_caught));
	memcpy(od.trophies_caught,g->trophies_caught,sizeof(od.trophies_caught));
	memcpy(od.superenergies_caught,g->superenergies_caught,sizeof(od.superenergies_caught));
}

/*
////////////////////////// info about player variables /////////////////////
// variables that effect gameplay maybe.. (will be used in total player packets and save restore gamestate.. //
66 variables so far, should be alot less... (currently totaling 412 bytes)
struct ol_playerdata opa[0]
12   VEC pos xyz
12   VEC carvel xyz
16   VEC carang xyzw
16   VEC carvelang xyzw
16   float shocklen[4] has changed 255 times
16   float wheelang[4] has changed 255 times
24   float wheelvel[6] has changed 255 times
 4   float wheelyaw has changed 255 times
-------- 116

 4   int ol_airtime has changed 255 times
 4   int ol_cbairtime has changed 255 times
 4   int ol_validroadnorm has changed 91 times
--------- 12

12   VEC ol_carnorm xyz
12  VEC ol_cardir xyz
12   VEC ol_carvelnorm xyz
 4   float ol_carvelmag has changed 255 times
 4   float ol_shockmag has changed 255 times
12   VEC ol_shocknorm xyz
12   VEC ol_lastroadnorm xyz
 4   float ol_accelspin has changed 255 times
 4   int ol_drivemode has changed 103 times
 4   int ol_hifrictime has changed 255 times
12   VEC ol_ulastloc xyz
 4   float ol_carbodymag has changed 255 times
-------  96
 
 4   int ol_ufliptime has changed 41 times
 4   int ol_doacrashreset has changed 11 times
 4   int ol_flymode has changed 80 times
 4   int ol_utotalcrashes has changed 1 times
 4   int ol_utotalstunts has changed 10 times
 4   int ol_startstunt has changed 82 times
12   VEC ol_carstuntrpy xyz
-------- 36

 4  int ol_steertime has changed 255 times
 4  float ol_uextraheading has changed 255 times
 4  int ol_lastkludge has changed 4 times
 4  int ol_cantdrive has changed 14 times
12   VEC ol_crashresetloc xyz
 4  float ol_crashresetdir has changed 255 times
 4  int ol_uready has changed 10 times
 4  int ol_cantstartdrive has changed 90 times
12 VEC ol_ulastpos xyz
 4   int ol_carboost has changed 10 times
12   VEC ol_landspot xyz
 --------- 68 

16   int ol_rollpie[4] has changed 25 times
16   int ol_pitchpie[4] has changed 26 times
16   int ol_yawpie[4] has changed 11 times
 4   int ol_rollpiesum has changed 29 times
 4   int ol_pitchpiesum has changed 17 times
 4   int ol_yawpiesum has changed 24 times
 4   int ol_cantaddroll has changed 5 times
 4   int ol_cantaddpitch has changed 3 times
 4   int ol_cantaddyaw has changed 3 times
 4   int ol_udidroll has changed 4 times
 4   int ol_udidyaw has changed 2 times
 4   int ol_udidpitch has changed 2 times
----------- 84

12   VEC ol_ucarrotvel xyz
 4  int ol_dofinish has changed 255 times
 4  int cntl has changed 9 times
 4   float seekframe has changed 255 times
12   VEC stuntgoals xyz
12   VEC curstuntangs xyz
---------------- 48 

12   int doaistunt[3] has changed 4 times
 4   int lapready has changed 4 times
 4   int lapsdone has changed 2 times
 4   int finplace has changed 1 times
 4   int cartspassed has changed 20 times
 4   int stuckwatchdog has changed 255 times
 4   int ol_clocktickcount has changed 255 times
----------------- 36
*/

