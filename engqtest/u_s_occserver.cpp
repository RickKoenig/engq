#include <time.h>

#include <m_eng.h>
#include <l_misclibm.h>

#include "u_s_occserver.h"

S32 serverport=1236;
//#define MEANSERVER // if defined must use correct game/player, the g/p redirect is disabled, server kicks you
#define UPDATETIME 9000	// about every 5 minutes
//#define UPDATETIME 108000 // 1 hour
namespace {
shape* rl;
shape* focus,*oldfocus;
pbut* pbutquit,*pbutweek1,*pbutweek2,*pbutweek3,*pbutweek4;
text* textmyip,*textmyname,*textstatus,*textweek;
con32* con;
S32 cnt;
socker* master;//,*cly;
#define USERBUFFSIZE 2000
C8 inputbuff[USERBUFFSIZE];
C8 outputbuff[USERBUFFSIZE];
S32 week=1;
bool changeweek=false;
S32 tscounter;
int numclients;
int minnumclients,maxnumclients,sumnumclients;

struct ci {
	S32 loggedin,kill;
	S32 game,player;
	socker* cly;
};
ci clientinfo[MAXCLIENTS];
ci* clientarray[MAXGAMES][MAXPLAYERS];

S32 serverip;

enum bus {BUSY_NONE,BUSY_NAME2IP};
bus busy;
//S32 loggedin,kill;
S32 echomesscnt;
S32 usermesscnt;
S32 hb;

// adds a '\n' to the end
const C8* timestamp()
{
	time_t t=time(0); // returns the time since 00:00:00 GMT, Jan. 1, 1970, measured in seconds.
	return ctime(&t);
}

// server log file
FILE* svflogger;
void svlogger(const C8* fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	if (svflogger) {
		vfprintf(svflogger,fmt,arglist);
	}
	va_end(arglist);
}

void svloggerinit()
{
	if (!svflogger) {
		svflogger=fopen2("svlogfile.txt","a");
		svlogger("\nopen svlogfile.txt\n");
	}
}

void svloggerexit()
{
	svlogger("close svlogfile\n");
	fclose(svflogger);
	svflogger=0;
}

// save to logfile, reopen, append
void svloggerrefresh()
{
	fclose(svflogger);
	svflogger=0;
	svflogger=fopen2("svlogfile.txt","a");
	svlogger("\nrefresh svlogfile.txt\n");
}

void serverstats()
{
	con32_printf(con,"stats: min %d, max %d, avg %d, %s",
	  minnumclients,maxnumclients,(sumnumclients+(tscounter>>1))/tscounter,timestamp());
	svlogger("stats: min %d, max %d, avg %d, %s",
	  minnumclients,maxnumclients,(sumnumclients+(tscounter>>1))/tscounter,timestamp());
	svloggerrefresh();
}

void statsinit()
{
	tscounter=0;
	minnumclients=1000000;
	maxnumclients=0;
	sumnumclients=0;
}

void statsupdate()
{
	if (numclients<minnumclients)
		minnumclients=numclients;
	if (numclients>maxnumclients)
		maxnumclients=numclients;
	sumnumclients+=numclients;
	++tscounter;
}

void servermessagevs(socker* sock,const C8* fmt,va_list arglist)
{
	C8 str[4096];
	vsprintf(str,fmt,arglist);
	if (!sock)
		return;
	S32 len=strlen(str);
	outputbuff[0]=S2C_MESS;
	memcpy(&outputbuff[1],str,len);
	tcpwritetoken32(sock,0,outputbuff,len+1);
	con32_printf(con,"servermess: '%s'\n",str);
	svlogger("servermess: '%s'\n",str);
}

void servermessage(socker* sock,const C8* fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	servermessagevs(sock,fmt,arglist);
	va_end(arglist);
}

/*void chatmessagevs(socker* sock,S32 g,S32 pfrom,S32 pto,const C8* fmt,va_list arglist)
{
	C8 str[4096];
	vsprintf(str,fmt,arglist);
	if (!sock)
		return;
	S32 len=strlen(str);
	outputbuff[0]=SRV_CHAT;
	outputbuff[1]=pfrom;
	memcpy(&outputbuff[2],str,len);
	tcpwritetoken(sock,outputbuff,len+2);
	con32_printf(con,"chatmess: g%d:p%d to g%d:p%d'%s'\n",g+1,pfrom+1,g+1,pto+1,str);
}

void chatmessage(socker* sock,S32 g,S32 pfrom,S32 pto,const C8* fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	chatmessagevs(sock,g,pfrom,pto,fmt,arglist);
	va_end(arglist);
}
*/

void echomessage(socker* sock,U32 mt,S32 g,S32 pfrom,S32 pto,const C8* data,S32 datalen)
{
//	con32_printf(con,"user message %d, from (%d:%d), to (%d:%d), len %d\n",
//		mt,g+1,pfrom+1,g+1,pto+1,datalen);
	++echomesscnt;
	outputbuff[0]=mt;
	outputbuff[1]=pfrom;
	memcpy(&outputbuff[2],data,datalen-1);
	tcpwritetoken32(sock,0,outputbuff,datalen+1);
}

void weekmessage(socker* sock)
{
	outputbuff[0]=S2C_WEEK;
	outputbuff[1]=week;
	tcpwritetoken32(sock,0,outputbuff,2);
}

void serverslot(socker* sock,S32 game,S32 player)
{
	con32_printf(con,"server slot %d %d\n",game,player);
	svlogger("server slot %d %d\n",game,player);
	outputbuff[0]=S2C_SLOT;
	outputbuff[1]=player;
	writeU16(&outputbuff[2],game);
	outputbuff[4]=week;
	tcpwritetoken32(sock,0,outputbuff,5);
}

// 1 yes, 0 recommend, -1 full
S32 isemptygameslot(S32* g,S32* p)
{
	S32 i,j;
// first see if slot empty
	if (!clientarray[*g][*p])
		return 1;
// next find and empty slot in current game
	for (i=0;i<MAXPLAYERS;++i) {
		if (!clientarray[*g][i]) {
			*p=i;
			return 0;
		}
	}
// next try to find any empty slot
	for (j=0;j<MAXGAMES;++j) {
		for (i=0;i<MAXPLAYERS;++i) {
			if (!clientarray[j][i]) {
				*p=i;
				*g=j;
				return 0;
			}
		}
	}
// all slots full
	return -1;
}

void readandparsetoken(ci* cli)
{
	S32 tid;
	S32 len=tcpreadtoken32(cli->cly,&tid,inputbuff,200);
#if 0
	if (len<0)
		errorexit("occserver: len<0");
#endif
	if (len>0) {
		inputbuff[len]='\0';
		U32 tok=inputbuff[0];
		if (!cli->loggedin) {
// login if not logged in
			if (tok==C2S_LOGON) {
				if (len>3) {
					U32 gameb=readU16(&inputbuff[1]); // little endian
					cli->game=gameb/MAXPLAYERS;
					cli->player=gameb%MAXPLAYERS;
					cli->game=range(0,cli->game,MAXGAMES-1);
					cli->player=range(0,cli->player,MAXPLAYERS-1);
					if (!strcmp("dummy",&inputbuff[3])) {
						S32 eg=isemptygameslot(&cli->game,&cli->player);
						if (eg>0) {
							serverslot(cli->cly,cli->game,cli->player);
//							servermessage(cli->cly,"game %d, player %d successfully logged in",cli->game+1,cli->player+1);
							clientarray[cli->game][cli->player]=cli;
							cli->loggedin=1;
						} else if (eg==0) {
#ifdef MEANSERVER
							servermessage(cli->cly,"slot used, try game %d, player %d",cli->game,cli->player);
#else
							serverslot(cli->cly,cli->game,cli->player);
							clientarray[cli->game][cli->player]=cli;
							cli->loggedin=1;
#endif
						} else {
							servermessage(cli->cly,"server full!",cli->game,cli->player);
						}
					} else {
						servermessage(cli->cly,"bad password");
					}
				} else {
					servermessage(cli->cly,"logon too short");
				}
			} else {
				servermessage(cli->cly,"not logged in yet, token %d",tok);
			}
		} else {
// another login token after already logged in
			if (tok==C2S_LOGON) {
				servermessage(cli->cly,"logged in already");
				cli->loggedin=0;
// chat token, broadcast to other players in same game
			} else if (tok>=C2S_CHAT) {
				S32 gn=cli->game;
				S32 pn=cli->player;
				if (tok==C2S_CHAT) {
					con32_printf(con,"chat from g%d:p%d '%s'\n",gn,pn,&inputbuff[1]);
					svlogger("chat from g%d:p%d '%s'\n",gn,pn,&inputbuff[1]);
				}
//				else
				++usermesscnt;
//				con32_printf(con,"usertok %d from g%d:p%d datalen %d\n",tok,gn,pn,len);
				S32 j;
// echo
				for (j=0;j<MAXPLAYERS;++j) {
					ci* clj=clientarray[gn][j];
					if (pn!=j && clj && clj->loggedin)
//						chatmessage(clj->cly,gn,pn,j,"'%s'",&inputbuff[1]);
						echomessage(clj->cly,tok,gn,pn,j,&inputbuff[1],len);
				}
// unknown token
			} else {
				servermessage(cli->cly,"unknown token %d",tok);
				cli->loggedin=0;
			}
		}
// kill clients not logged in by now
		if (!cli->loggedin) {
			cli->kill=1;
		}
	}
}

}

U32 readU16(const C8* buff)
{
	U16* ubuff=(U16*)buff;
	return *ubuff;
}

U32 readU32(const C8* buff)
{
	U32* ubuff=(U32*)buff;
	return *ubuff;
}

float readfloat(const C8* buff)
{
	float* ubuff=(float*)buff;
	return *ubuff;
}

void writeU16(C8* buff,U32 val)
{
	U16* ubuff=(U16*)buff;
	*ubuff=val;
}

void writeU32(C8* buff,U32 val)
{
	U32* ubuff=(U32*)buff;
	*ubuff=val;
}

void writefloat(C8* buff,float val)
{
	float* ubuff=(float*)buff;
	*ubuff=val;
}

void occserverinit()
{
	statsinit();
	numclients=0;
	C8 abuff[4];
	writeU32(&abuff[0],131);
	logger("abuff %d %d %d %d\n",abuff[0],abuff[1],abuff[2],abuff[3]);
	S32 resu=readU32(&abuff[0]);
	logger("resu = %d\n",resu);
	con=con32_alloc(535,360,C32WHITE,C32BLACK);
	initsocker();
	video_setupwindow(600,700);
	factory2<shape> fact;
	pushandsetdir("occ");
	svloggerinit();
	svlogger("server up at %s",timestamp());
	script sc("occserverres.txt");
	rl=fact.newclass_from_handle(sc);
	pbutquit=rl->find<pbut>("PBUTQUIT");
	pbutweek1=rl->find<pbut>("PBUTWEEK1");
	pbutweek2=rl->find<pbut>("PBUTWEEK2");
	pbutweek3=rl->find<pbut>("PBUTWEEK3");
	pbutweek4=rl->find<pbut>("PBUTWEEK4");
	textmyip=rl->find<text>("TEXTMYIP");
	textweek=rl->find<text>("TEXTWEEK");
	textmyname=rl->find<text>("TEXTMYNAME");
	textstatus=rl->find<text>("TEXTSTATUS");
	textstatus->settname("myname2ip");
	focus=oldfocus=0;
	cnt=0;
	busy=BUSY_NAME2IP;
//	loggedin=kill=0;
	memset(clientinfo,0,sizeof(clientinfo));
	memset(clientarray,0,sizeof(clientarray));
	echomesscnt=0;
	usermesscnt=0;
	hb=0;
}

void occserverproc()
{
	ci* cli;
	S32 i;//,j;
// update server log every so often
	statsupdate();
	if (tscounter==UPDATETIME) {
		serverstats();
		statsinit();
	}
// read all data from winsock
	int nc=0;
	for (i=0,cli=clientinfo;i<MAXCLIENTS;++i,++cli) {
		if (cli->cly) {
			tcpfillreadbuff(cli->cly); // winsock ----> buff
			++nc;
		}
	}
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
	if (focus == pbutquit) {
		if (ret==1)
			popstate();
	} else if (focus == pbutweek1) {
		if (ret==1) {
			week=1;
			changeweek=true;
			con32_printf(con,"week changed to %d\n",week);
			svlogger("week changed to %d\n",week);
			textweek->settname("WEEK1");
		}
	} else if (focus == pbutweek2) {
		if (ret==1) {
			week=2;
			changeweek=true;
			con32_printf(con,"week changed to %d\n",week);
			svlogger("week changed to %d\n",week);
			textweek->settname("WEEK2");
		}
	} else if (focus == pbutweek3) {
		if (ret==1) {
			week=3;
			changeweek=true;
			con32_printf(con,"week changed to %d\n",week);
			svlogger("week changed to %d\n",week);
			textweek->settname("WEEK3");
		}
	} else if (focus == pbutweek4) {
		if (ret==1) {
			week=4;
			changeweek=true;
			con32_printf(con,"week changed to %d\n",week);
			svlogger("week changed to %d\n",week);
			textweek->settname("WEEK4");
		}
	}
// done handle buttons and editboxes
// wait for my ip
	S32 gotmyip;
	switch(busy) {
	case BUSY_NAME2IP:
		gotmyip=getnmyname();
		for (i=0;i<gotmyip;i++)
			logger("mynames(s) '%s'\n",getmyname(i));
		gotmyip=getnmyip();
		for (i=0;i<gotmyip;i++)
			logger("myip(s) '%s'\n",ip2str(getmyip(i)));
		if (gotmyip) {
			serverip=getmyip(0);
			textmyip->settname(ip2str(serverip));
			textmyname->settname(getmyname(0));
			textstatus->settname("ready");
			logger("got my name on frame %d\n",cnt);
			busy=BUSY_NONE;
		}
		break;
// prelims out of the way
	case BUSY_NONE:
// build listen socket
		if (!master) {
			master=tcpmakemaster(serverip,serverport);
			if (!master)
				errorexit("can't make server master socket");
			con32_printf(con,"listen on port %d\n",serverport);
			svlogger("listen on port %d\n",serverport);
			svloggerrefresh();
		}
// make new connections
		for (i=0,cli=clientinfo;i<MAXCLIENTS;++i,++cli) {
			if (!cli->cly) {
				cli->cly=tcpgetaccept(master);
				if (cli->cly) {
					con32_printf(con,"client connected\n");
					svlogger("client connected\n");
					cli->loggedin=cli->kill=0;
				} else {
					break; // no more connections
				}
			}
		}
// read and parse connections
		for (i=0,cli=clientinfo;i<MAXCLIENTS;++i,++cli) {
			if (cli->cly) {
				readandparsetoken(cli);
			}
		}
// send week, close disconnected connections
		for (i=0,cli=clientinfo;i<MAXCLIENTS;++i,++cli) {
			if (changeweek) {
				if (cli->cly && cli->loggedin)
					weekmessage(cli->cly);
			}
			if (cli->cly && (cli->cly->flags & SF_CLOSED) && !tcpgetrbuffsize(cli->cly)) {
				con32_printf(con,"client %d %d disconnected\n",cli->game,cli->player);
				svlogger("client %d %d disconnected\n",cli->game,cli->player);
				cli->kill=1;
			}
		}
		changeweek=false;
		break;
	}
	for (i=0,cli=clientinfo;i<MAXCLIENTS;++i,++cli) {
// write all data to winsock
		ret=tcpsendwritebuff(cli->cly);
// kick bad users
		if (ret && cli && cli->kill) {
			freesocker(cli->cly);
			cli->cly=0;
			if (cli->loggedin)
				clientarray[cli->game][cli->player]=0;
			cli->kill=cli->loggedin=0;
		}
	}
	++cnt;
	if (hb==0) {
		if (echomesscnt) {
//			con32_printf(con,"echo out messages %d\n",echomesscnt);
			echomesscnt=0;
		}
		if (usermesscnt) {
//			con32_printf(con,"user in messages %d\n",usermesscnt);
			usermesscnt=0;
		}
	}
	++hb;
	if (hb==30)
		hb=0;
	if (numclients!=nc) {
		con32_printf(con,"numusers %d\n",nc);
		svlogger("numusers %d\n",nc);
		numclients=nc;
	}
}

void occserverdraw2d()
{
// video update
//	video_lock();
	clipclear32(B32,C32BLUE);
	outtextxy32(B32,400,50,C32YELLOW,"occ server V2.0");
	rl->draw(); // buttons etc.
	clipblit32(con32_getbitmap32(con),B32,0,0,45,190,WX,WY); // console
/*	for (j=0;j<MAXGAMES;++j) {
		for (i=0;i<MAXPLAYERS;++i) {
			C32 c = clientarray[j][i] ? C32WHITE : C32BLACK;
			clipputpixel32(B32,10+i,2+j+j/10+j/100,c);
//			cliprect32(B32,10+i*8,2+j*8+3*(j/5),6,6,c);
		}
	} */
//	video_unlock();
}

void occserverexit()
{
// free everything
	delete rl;
	S32 i;
	for (i=0;i<MAXCLIENTS;++i)
		if (clientinfo[i].cly)
			freesocker(clientinfo[i].cly);
	if (master) {
		freesocker(master);
		master=0;
	}
	uninitsocker();
	con32_free(con);
	popdir();
	svlogger("server down at %s",timestamp());
	svloggerexit();
}
