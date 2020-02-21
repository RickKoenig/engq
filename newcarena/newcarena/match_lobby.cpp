#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_logon.h"
#include "match_game1.h"

static shape* rl;
static shape* focus;
static listbox* LISTBOX1; // users
static listbox* LISTBOX2; // game host name ...
static listbox* LISTBOX3; // server messages
static listbox* LISTBOX4; // game ips ... 
static listbox* LISTBOX5; // game ports ... 
static listbox* LISTBOX6; // game game name ...
static listbox* LISTBOX7; // game curnum players (status)
static edit* EDIT1; // chat
static pbut* PBUT1; // hiscores
static pbut* PBUT2; // create game (host/server)
static pbut* PBUT3; // join game (client)
static pbut* PBUT5; // bail (quit/logoff)
static text* TEXT1; // your name
static text* TEXT5; // print chat bot rate (test)
static hscroll* HSLIDE1; // set chat bot rate

// #define PORTSTART 5415


static int keepsock;

#define MAXTOKEN 1024 // maybe match st_server.c
static int botrate,botcount,boterror; // chatbots

/*
// space and ' also are end of string
static int strspcmp(const char *a,const char *b)
{
	while(*a==*b && *a!='\0' && *a!=' ' && *a!='\'' && *b!='\0' && *b!=' ' && *b !='\'') {
		a++;
		b++;
	}
	if (*a=='\0' || *a==' ' || *a=='\'')
		if (*b=='\0' || *b==' ' || *b=='\'')
			return 0;
	return *a-*b;
}
*/

void match_lobby_init()
{
	if (!matchcl)
		return;
	logger("lobby init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
//	rl=loadres("lobbyres.txt");
	factory2<shape> fact;
	script* sc=new script("lobbyres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	LISTBOX1=rl->find<listbox>("LISTBOX1");
	LISTBOX2=rl->find<listbox>("LISTBOX2");
	LISTBOX3=rl->find<listbox>("LISTBOX3");
	LISTBOX3->setmax(1000);
	LISTBOX4=rl->find<listbox>("LISTBOX4");
	LISTBOX5=rl->find<listbox>("LISTBOX5");
	LISTBOX6=rl->find<listbox>("LISTBOX6");
	LISTBOX7=rl->find<listbox>("LISTBOX7");
	EDIT1=rl->find<edit>("EDIT1");
	PBUT1=rl->find<pbut>("PBUT1");
	PBUT2=rl->find<pbut>("PBUT2");
	PBUT3=rl->find<pbut>("PBUT3");
	PBUT3->setvis(false);
	PBUT5=rl->find<pbut>("PBUT5");
	TEXT1=rl->find<text>("TEXT1");
	TEXT5=rl->find<text>("TEXT5");
	HSLIDE1=rl->find<hscroll>("HSLIDE1");
	HSLIDE1->setminmaxval(0,100);
	keepsock=0;
	focus=EDIT1;
//	setresname(rl,EDIT1,"");
	EDIT1->settname("");
//	setresname(rl,TEXT1,username);
	TEXT1->settname(username);
//	addlistboxname(rl,LISTBOX1,-1,username,SCL1);
	LISTBOX1->addstring(username);
	botcount=botrate=boterror=0;
	tcpwritetoken32(matchcl,0,"back",4);
}

void match_lobby_proc()
{
	int len,i,n;
	char *tok,*tok2,*tok3;
	char token[MAXTOKEN+1];
//	char str[300];
//	struct rmessage rm;
	if (!matchcl) { // must be logged on
//		changestatefunc(st_logoninit);
		changestate(STATE_MATCHLOGON);
		return;
	}
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	boterror+=botrate; // test chat with chat bot
	if (boterror>=300) {
		boterror-=300;
		sprintf(token,"say count %d",botcount++);
		len=strlen(token);
		tcpwritetoken32(matchcl,0,token,len);
	}
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
// do ui
	PBUT3->setvis(LISTBOX2->getidx()>=0);
	if (focus) {
		int comlistidx=-1;
		S32 ret=focus->proc();
		shape* focusb = ret ? focus : 0;
		shape* focush = ret>=0 ? focus : 0;
		shape* focusl = ret>=0 ? focus : 0;
		shape* focuse = ret>=0 ? focus : 0;
//		switch(rm.id) {
		if (focuse==EDIT1) { // chat
//		case EDIT1:
			strcpy(token,"say ");
//			strcat(token,getresname(rl,EDIT1));
			strcat(token,EDIT1->gettname());
			len=strlen(token);
			tcpwritetoken32(matchcl,0,token,len);
//			setresname(rl,EDIT1,"");
			EDIT1->settname("");
//			break; 
		} else if (focusb==PBUT1) { // look at hi scores, and keep logged on
//		case PBUT1:
			keepsock=1;
			changestate(STATE_MATCHHISCORES);
//			break;
		} else if (focusb==PBUT2) {
//		case PBUT2: // create a game and a server/master/listen socket and a udp socket
#if 0
			if (getnmyip()) {
				for (i=PORTSTART;i<=PORTEND;i++) { // find an open socket..
//					gametcpsv=tcpmakemaster(0,i);
					gametcpsv=tcpmakemaster(getmyip(0),i);
					if (gametcpsv) {
						logger("lobby: tcpmakemaster for game at %s:%d\n",
							ip2str(gametcpsv->ip),gametcpsv->port);
						break;
					}
				}
				if (gametcpsv)
//					gameudpsv=udpmake(0,gametcpsv->port);
					gameudpsv=udpmake(gametcpsv->ip,gametcpsv->port);
				if (gametcpsv && gameudpsv) { // got tcp/udp let's go!
					keepsock=1;
//					pushandchangestatenum(72); // options for gamehost
					pushchangestate(STATE_MATCHPICKGAME); // pick which game to host
				} else { // no can do
					if (gametcpsv) {
						freesocker(gametcpsv);
						gametcpsv=NULL;
					}
					if (gameudpsv) {
						freesocker(gameudpsv);
						gameudpsv=NULL;
					}
				}
			}
#else
			cgi.isvalid=true;
			cgi.ishost=true;
			cgi.ip=getmyip(0);
			cgi.port=0;//PORTSTART;
			cgi.nports=10;
			pushchangestate(STATE_MATCHPICKGAME); // pick which game to host
			cgi.gameidx=0;	// for now
			keepsock=1;
			strcpy(hostname,username);
#endif
//			break;
		} else if (focusb==PBUT3) {
//		case PBUT3: // join a game on the list, make connect tcp and udp sockets
//			if (getcurlistbox(rl,LISTBOX2)>=0) {
			if (LISTBOX2->getidx()>=0) {
#if 0
				gametcpcl=tcpgetconnect(
//					str2ip(findlistboxname(rl,LISTBOX4,getcurlistbox(rl,LISTBOX4))),
//					  atoi(findlistboxname(rl,LISTBOX5,getcurlistbox(rl,LISTBOX5))));
					str2ip(LISTBOX4->getidxname()),atoi(LISTBOX5->getidxname()));
				if (gametcpcl)
					gameudpcl=udpmake(gametcpcl->ip,gametcpcl->port);
				if (gametcpcl && gameudpcl) {
					keepsock=1;
//					pushandchangestatenum(78); // play a hosted game
					pushchangestate(STATE_MATCHGAME1);
				} else {
					if (gametcpcl) {
						freesocker(gametcpcl);
						gametcpcl=NULL;
					}
				}
#else
				cgi.isvalid=true;
				const C8* gn=LISTBOX6->getidxname(); // game name/kind
				strcpy(hostname,LISTBOX2->getidxname());
				S32 gidx=getgameidx(gn);
				cgi.gameidx=gidx;
				S32 ns=getgamejoinstate(gidx);
				cgi.ishost=false;
				cgi.ip=str2ip(LISTBOX4->getidxname());
				cgi.port=atoi(LISTBOX5->getidxname());
				cgi.nports=1;
				pushchangestate(ns);
				keepsock=1;
#endif
			}
//			break;
		} else if (focusb==PBUT5) { // quit/logoff
//		case PBUT5:
			keepsock=0;
//			changestatefunc(st_logoninit);
			changestate(STATE_MATCHLOGON);
//			break;
/*		case SCL1:
			setlistboxoffset(rl,LISTBOX1,rm.val);
			break;
		case SCL2:
			setlistboxoffset(rl,LISTBOX2,rm.val);
			setlistboxoffset(rl,LISTBOX4,rm.val);
			setlistboxoffset(rl,LISTBOX5,rm.val);
			break;
		case SCL3:
			setlistboxoffset(rl,LISTBOX3,rm.val);
			break; */
		} else if (focush==HSLIDE1) { // test chat
//		case HSLIDE1:
			C8 str[300];
			sprintf(str,"chat bot speed %3.1f",HSLIDE1->getidx()/10.0);
			botrate=HSLIDE1->getidx();
//			setresname(rl,TEXT5,str);
			TEXT5->settname(str);
//			break;
		} else if (focusl==LISTBOX2) { // pick from list of hosts
			comlistidx=LISTBOX2->getidx();
		} else if (focusl==LISTBOX6) { // pick from list of hosts
			comlistidx=LISTBOX6->getidx();
		} else if (focusl==LISTBOX7) { // pick from list of hosts
			comlistidx=LISTBOX7->getidx();
		} else if (focusl==LISTBOX4) {
			comlistidx=LISTBOX4->getidx();
		} else if (focusl==LISTBOX5) {
			comlistidx=LISTBOX5->getidx();
//		case LISTBOX2:
//		case LISTBOX4:
//		case LISTBOX5:
//			break;
		}
		if (comlistidx>=0) { // make sure all lists are aligned (hostname,ip,port)
//			setcurlistbox(rl,LISTBOX2,focusl->getidx());
//			setcurlistbox(rl,LISTBOX4,rm.val,-1);
//			setcurlistbox(rl,LISTBOX5,rm.val,-1);
			LISTBOX2->setidxc(comlistidx);
			LISTBOX6->setidxc(comlistidx);
			LISTBOX7->setidxc(comlistidx);
			LISTBOX4->setidxc(comlistidx);
			LISTBOX5->setidxc(comlistidx);
		}
	}
// done ui, now get data from matching server
	tcpfillreadbuff(matchcl);
	if (matchcl->flags&SF_CLOSED) {
		keepsock=0;
//		changestatefunc(st_logoninit);
		changestate(STATE_MATCHLOGON);
	}
// collect tokens
	while(len=tcpreadtoken32(matchcl,0,token,MAXTOKEN)) {
		token[len]='\0';
		tok=strtok(token," ");
// loggedon name, matching server sends to 'all not away' that a player has logged on
		if (!strcmp(tok,"loggedon")) {
			tok=strtok(NULL,"");
			if (tok) {
//				i=findlistboxidx(rl,LISTBOX1,tok);
				i=LISTBOX1->findstring(tok);
				if (i<0) {// -1 if not found
//					addlistboxname(rl,LISTBOX1,-1,tok,SCL1); // -1 bottom, 0 top
					LISTBOX1->addstringstay(tok);
//					sortlistbox(rl,LISTBOX1,-1,-1,-1,0);
					LISTBOX1->sort();
				}
			}
// gameinfo hostname gamename gameip gameport, matching server sends to 'all not away' a new game is forming
// gameinfo hostname
// gameinfo (never happens)
		} else if (!strcmp(tok,"gameinfo")) {
			tok=strtok(NULL," "); // hostname
			tok2=strtok(NULL," "); // gamename
			tok3=strtok(NULL," "); // gamecurplayers
			if (tok) {
//				n=getnumlistbox(rl,LISTBOX2);
				n=LISTBOX2->getnumidx();
				for (i=0;i<n;i++) {
//					if (!strspcmp(findlistboxname(rl,LISTBOX2,i),tok)) {
					if (!strcmp(LISTBOX2->getidxname(i),tok)) { // remove hostname if already in the list
//						dellistboxname(rl,LISTBOX2,i,SCL2);
						LISTBOX2->removeidx(i);
						LISTBOX6->removeidx(i);
						LISTBOX7->removeidx(i);
//						dellistboxname(rl,LISTBOX4,i,-1);
						LISTBOX4->removeidx(i);
//						dellistboxname(rl,LISTBOX5,i,-1);
						LISTBOX5->removeidx(i);
						i--;
						n--;
					}
				}
				if (tok2 && tok3) { // game name
//					sprintf(str,"%s's %s",tok,tok2);
//					addlistboxname(rl,LISTBOX2,-1,str,SCL2); // -1 bottom, 0 top
//					S32 i;
//					for (i=0;i<100;++i)
						LISTBOX2->addstringstay(tok); // hostname
//					for (i=0;i<100;++i)
						LISTBOX6->addstringstay(tok2); // gamename
//					for (i=0;i<100;++i)
						LISTBOX7->addstringstay(tok3); // gamestatus/curplayers
					tok2=strtok(NULL," "); // ip
					if (tok2) {
//						addlistboxname(rl,LISTBOX4,-1,tok2,-1); // -1 bottom, 0 top
//						for (i=0;i<100;++i)
							LISTBOX4->addstringstay(tok2);
						tok2=strtok(NULL," "); // port
						if (tok2) {
//							addlistboxname(rl,LISTBOX5,-1,tok2,-1); // -1 bottom, 0 top
//							for (i=0;i<100;++i)
								LISTBOX5->addstringstay(tok2);
						}
					}
//					sortlistbox(rl,LISTBOX2,LISTBOX4,LISTBOX5,-1,0);
//					LISTBOX2->sort();
//					LISTBOX4->sort();
//					LISTBOX5->sort();
//					LISTBOX6->sort();
//					LISTBOX7->sort();
				}
			}
// loggedoff
		} else if (!strcmp(tok,"loggedoff")) {
			tok=strtok(NULL,"");
			if (tok) {
//				i=findlistboxidx(rl,LISTBOX1,tok);
				i=LISTBOX1->findstring(tok);
				if (i>=0)
//					dellistboxname(rl,LISTBOX1,i,SCL1);
					LISTBOX1->removeidx(i);
//				n=getnumlistbox(rl,LISTBOX2);
				n=LISTBOX2->getnumidx();
				for (i=0;i<n;i++) {
//					if (!strspcmp(findlistboxname(rl,LISTBOX2,i),tok)) {
					if (!strcmp(LISTBOX2->getidxname(i),tok)) {
//						dellistboxname(rl,LISTBOX2,i,SCL2);
						LISTBOX2->removeidx(i);
						LISTBOX6->removeidx(i);
						LISTBOX7->removeidx(i);
//						dellistboxname(rl,LISTBOX4,i,-1);
						LISTBOX4->removeidx(i);
//						dellistboxname(rl,LISTBOX5,i,-1);
						LISTBOX5->removeidx(i);
						i--;
						n--;
					}
				}
			}
// said
		} else if (!strcmp(tok,"said")) {
			tok=tok2=NULL;
			tok=strtok(NULL," ");
			tok2=strtok(NULL,"");
			if (tok && tok2) {
				C8 str[500];
				sprintf(str,"%s:%s",tok,tok2);
//				dofifolist(rl,str,100,LISTBOX3,SCL3);
				LISTBOX3->addstring(str);
			}
		}
	}
	tcpsendwritebuff(matchcl);
}

void match_lobby_draw2d()
{
	cliprect32(B32,0,0,WX-1,WY-1,C32BLACK);
	rl->draw();
	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"lobby");
}

void match_lobby_exit()
{
	if (!matchcl) {
		return;
	}
	tcpwritetoken32(matchcl,0,"away",4);
	tcpsendwritebuff(matchcl);
	if (!keepsock) {
		freesocker(matchcl);
		matchcl=NULL;
		uninitsocker();
	}
	popdir();
//	freeres(rl);
	delete rl;
}
