#include <m_eng.h>
#include <l_misclibm.h>

#include "u_s_occserver.h"

namespace {
shape* rl;
shape* focus,*oldfocus;
pbut* pbutquit;
pbut* pbutlogon,*pbutlogoff;
edit* editgame,*editplayer,*editpassword,*editchat;
text* textservername,*textserverip,*textmyip,*textmyname,*textstatus;
con32* con;
U32 serverip;
S32 firstchat;
socker* cly;
C8* loginstr="loginstr";
S32 loginstrlen;

#define USERBUFFSIZE 2000
C8 inputbuff[USERBUFFSIZE];
C8 outputbuff[USERBUFFSIZE];

//S32 serverport=1236;

enum bus {BUSY_NONE,BUSY_GETMYIP,BUSY_NAME2IP,BUSY_LOGONSEND1,BUSY_LOGONSEND2,BUSY_CONNECTED};
bus busy;

// player position stuff
#define POFFX 430
#define POFFY 190
#define PSX 300
#define PSY 300
#define MAXNOHEARTIME 150
#define HEARTBEAT 30
S32 hbcounter;

struct ppos {
	S32 x,y,z;
	S16 level;
	S32 noheartime;
};
ppos players[MAXPLAYERS];
S32 playeryou;

void initplayers(S32 pn)
{
	memset(players,0,sizeof(players));
	S32 i;
	for (i=0;i<MAXPLAYERS;++i)
		players[i].level=-1;
	players[pn].level=0;
	if (pn&1)
		players[pn].x=PSX-1;
	else
		players[pn].x=0;
	if (pn&2)
		players[pn].y=PSY-1;
	else
		players[pn].y=0;
	playeryou=pn;
}

void moveplayers()
{
	S32 i;
	for (i=0;i<MAXPLAYERS;++i) {
		++players[i].noheartime;
		if (players[i].noheartime>MAXNOHEARTIME)
			players[i].noheartime=MAXNOHEARTIME;
	}
	if (wininfo.keystate[K_RIGHT]|wininfo.keystate[K_NUMRIGHT])
		++players[playeryou].x;
	if (wininfo.keystate[K_LEFT]|wininfo.keystate[K_NUMLEFT])
		--players[playeryou].x;
	if (wininfo.keystate[K_DOWN]|wininfo.keystate[K_NUMDOWN])
		++players[playeryou].y;
	if (wininfo.keystate[K_UP]|wininfo.keystate[K_NUMUP])
		--players[playeryou].y;
	players[playeryou].x=range(0,players[playeryou].x,PSX-1);
	players[playeryou].y=range(0,players[playeryou].y,PSY-1);
	if (busy==BUSY_CONNECTED && hbcounter==0) {
		outputbuff[0]=C2S_POS;
		writeU32(&outputbuff[1],players[playeryou].x);
		writeU32(&outputbuff[5],players[playeryou].y);
		writeU32(&outputbuff[9],players[playeryou].z);
		writeU16(&outputbuff[13],players[playeryou].level);
		tcpwritetoken32(cly,0,outputbuff,15);
	}
}

void drawplayers()
{
	cliprect32(B32,POFFX,POFFY,PSX,PSY,C32GREEN);
	S32 i;
	for (i=0;i<MAXPLAYERS;++i) {
		if (players[i].level>=0 && (i==playeryou || players[i].noheartime<MAXNOHEARTIME)) {
			S32 px=POFFX+players[i].x;
			S32 py=POFFY+players[i].y;
			clipcircle32(B32,px,py,6,i==playeryou ? C32WHITE : C32YELLOW);
			outtextxyf32(B32,px-4,py-4,C32BLACK,"%d",i+1);
		}
	}
}

}

void occclientinit()
{
	con=con32_alloc(380,360,C32WHITE,C32BLACK);
	initsocker();
	video_setupwindow(800,600);
	factory2<shape> fact;
	pushandsetdir("occ");
	script sc("occclientres.txt");
	popdir();
	rl=fact.newclass_from_handle(sc);
	pbutquit=rl->find<pbut>("PBUTQUIT");
	pbutlogon=rl->find<pbut>("PBUTLOGON");
	pbutlogoff=rl->find<pbut>("PBUTLOGOFF");
	editgame=rl->find<edit>("EDITGAME");
	editplayer=rl->find<edit>("EDITPLAYER");
	editchat=rl->find<edit>("EDITCHAT");
	editpassword=rl->find<edit>("EDITPASSWORD");
	textservername=rl->find<text>("TEXTSERVERNAME");
	textserverip=rl->find<text>("TEXTSERVERIP");
	textmyip=rl->find<text>("TEXTMYIP");
	textmyname=rl->find<text>("TEXTMYNAME");
	textstatus=rl->find<text>("TEXTSTATUS");
	focus=oldfocus=0;
	serverip=0;
	busy=BUSY_GETMYIP;
	firstchat=1;
	loginstrlen=strlen(loginstr);
	initplayers(0);
	hbcounter=0;
}

void occclientproc()
{
	tcpfillreadbuff(cly); // winsock ----> buff
// bail
	if (KEY==K_ESCAPE)
		popstate();
// handle buttons and editboxes
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
	moveplayers();
// chat
	if (focus==editchat) {
		if (firstchat) {
			editchat->settname("");
			firstchat=0;
		}
		if (KEY=='\r') {
			con32_printf(con,"'%s'\n",editchat->gettname());
// send chat
			if (cly && busy==BUSY_CONNECTED) {
				const C8* s=editchat->gettname();
				S32 strl=strlen(s);
				outputbuff[0]=C2S_CHAT;
				memcpy(&outputbuff[1],s,strl);
				tcpwritetoken32(cly,0,outputbuff,strl+1);
			}
			editchat->settname("");
		}
	}
	else if (focus == pbutquit) {
		if (ret==1)
			popstate();
	} else if (focus == pbutlogon) {
		if (ret==1) {
// logon, then goto the logon send1 state
			if (busy==BUSY_NONE) {
				textstatus->settname("logging on s1");
				cly=tcpgetconnect(serverip,serverport);
				if (!cly)
					errorexit("can't get connect");
				busy=BUSY_LOGONSEND1;
			}
		}
	} else if (focus == pbutlogoff) {
// logoff
		if (ret==1) {
			if (busy==BUSY_CONNECTED) {
				if (cly) {
					freesocker(cly);
					cly=0;
				}
				textstatus->settname("logged off");
				busy=BUSY_NONE;
			}
		}
	}
// done handle buttons and editboxes
	switch (busy) {
	S32 i,gotmyip;
	U32 rt;
	case BUSY_GETMYIP:
// wait for my ip
		gotmyip=getnmyname();
		for (i=0;i<gotmyip;i++)
			logger("mynames(s) '%s'\n",getmyname(i));
		gotmyip=getnmyip();
		for (i=0;i<gotmyip;i++)
			logger("myip(s) '%s'\n",ip2str(getmyip(i)));
		if (gotmyip) {
			textmyip->settname(ip2str(getmyip(0)));
			textmyname->settname(getmyname(0));
			textstatus->settname("ready");

// now get server name from server ip
			rt=getinfofromname(textservername->gettname()); // initiate getinfo
			if (!rt)
				errorexit("can't get serverip");
			textstatus->settname("server name to ip");
			busy=BUSY_NAME2IP;
		}
		break;
	case BUSY_NAME2IP:
// wait for server name
		if (getninfoip()) {
			serverip=getinfoip(0);
			textserverip->settname(ip2str(serverip));
			textstatus->settname("logged off");
			busy=BUSY_NONE;
		}
		break;
	case BUSY_LOGONSEND1:
// wait for connected then send login data
		if (cly && (cly->flags & SF_CONNECTED)) {
			const C8* pw=editpassword->gettname();
			S32 slen=strlen(pw);
			outputbuff[0]=C2S_LOGON;
			S32 game=atoi(editgame->gettname())-1;
			game=range(0,game,MAXGAMES-1);
			S32 player=atoi(editplayer->gettname())-1;
			player=range(0,player,MAXPLAYERS-1);
			U32 gameb=(game*MAXPLAYERS)+player;
			writeU16(&outputbuff[1],gameb);
//			outputbuff[1]=gameb&0xff; // little endian
//			outputbuff[2]=gameb>>8;
			memcpy(&outputbuff[3],pw,slen);
			S32 len=tcpwritetoken32(cly,0,outputbuff,3+slen);
			if (len>0) {
				textstatus->settname("logging on s2");
				busy=BUSY_LOGONSEND2;
			}
			initplayers(player);
		}
		break;
	case BUSY_LOGONSEND2:
// wait for empty write buffer, (server received the data)
		if (cly && tcpsendwritebuff(cly)==1) {
			textstatus->settname("logging on r1");
			busy=BUSY_CONNECTED; // we're in
			textstatus->settname("connected");
		}
		break;
	case BUSY_CONNECTED:
// while connected, read from server
		if (cly) {
			S32 tid;
			S32 len=tcpreadtoken32(cly,&tid,inputbuff,100); // buff ----> app
#if 1
			if (len<0)
				errorexit("occclient: len<0");
#endif
			if (len>0) {
				inputbuff[len]='\0'; // enough room to null terminate if necessary
				U32 tok=inputbuff[0];
				switch (tok) {
				case S2C_MESS:
					con32_printf(con,"server reply '%s'\n",&inputbuff[1]);
					break;
				case S2C_CHAT:
					con32_printf(con,"player %d says: '%s'\n",inputbuff[1]+1,&inputbuff[2]);
					break;
				case S2C_POS:
					if (len==16) {
						S32 pn=inputbuff[1];
						if (pn>=0 && pn<MAXPLAYERS) {
							players[pn].x=readU32(&inputbuff[2]);
							players[pn].y=readU32(&inputbuff[6]);
							players[pn].z=readU32(&inputbuff[10]);
							players[pn].level=readU16(&inputbuff[14]);
							players[pn].noheartime=0;
							con32_printf(con,"pos token from player %d\n",inputbuff[1]+1);
						}
					} else
						con32_printf(con,"bad pos token from player %d\n",inputbuff[1]+1);
					break;
				case S2C_SLOT:
					if (len==5) {
						C8 dest[10];
						S32 pn=inputbuff[1];
						S32 gn=readU16(&inputbuff[2]);
						S32 wn=inputbuff[4];
						con32_printf(con,"slot token g %d p %d w %d\n",gn+1,pn+1,wn+1);
						initplayers(pn);
						my_itoa(pn+1,dest,10);
						editplayer->settname(dest);
						my_itoa(gn+1,dest,10);
						editgame->settname(dest);
					} else
						con32_printf(con,"bad slot token from server\n");
					break;
				default:
					con32_printf(con,"unknown server token %d, len %d\n",tok,len);
				}
			}
// if the server dies or the server killed us
			if ((cly->flags & SF_CLOSED) && !tcpgetrbuffsize(cly)) {
				con32_printf(con,"server disconnected\n");
				freesocker(cly);
				cly=0;
				textstatus->settname("disconnected");
				busy=BUSY_NONE;
			}
		}
		break;
	case BUSY_NONE:
		break;
	}
	tcpsendwritebuff(cly);
	++hbcounter;
	if (hbcounter>=HEARTBEAT)
		hbcounter=0;
}

void occclientdraw2d()
{
//	video_lock();
	clipclear32(B32,C32BLUE);
	rl->draw();
	clipblit32(con32_getbitmap32(con),B32,0,0,20,190,WX,WY);
	drawplayers();
//	video_unlock();
}

void occclientexit()
{
	delete rl;
	if (cly) {
		freesocker(cly);
		cly=0;
	}
	uninitsocker();
	con32_free(con);
}
