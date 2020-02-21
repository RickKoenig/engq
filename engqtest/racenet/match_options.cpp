#include <m_eng.h>
#include <l_misclibm.h>
#include "../u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_game1.h"
#include "match_logon.h"

//static struct reslist *rl;
static shape* rl;
static shape* focus;
static pbut* PBUT1; // play your hosted game
static pbut* PBUT2; // back to lobby/quickgame

static int keepsock;
//static char token[150];
//static int ks;
//extern int enablenumkeychangestate;

void match_optionsinit()
{
#if 0
	if (gametcpsv==NULL)
		return;
#endif
//	ks=enablenumkeychangestate;
//	enablenumkeychangestate=0;
//	if (!matchcl)
//		return;
	logger("options init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
//	rl=loadres("optionsres.txt");
	factory2<shape> fact;
	script* sc=new script("optionsres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	PBUT1=rl->find<pbut>("PBUT1");
	PBUT2=rl->find<pbut>("PBUT2");
//	S32 nsc;
	keepsock=0;
	if (matchcl)
//		setresname(rl,PBUT2,"Back to Lobby");
		PBUT2->settname("Back to Lobby");
	else
//		setresname(rl,PBUT2,"Back to Quickgame");
		PBUT2->settname("Back to Quickgame");
	focus=0;
}

void match_optionsproc()
{
	if (KEY==K_ESCAPE) {
		cgi.isvalid=false;
		popstate();
	}
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
//	int len;
//	struct rmessage rm;
#if 0
	if (gametcpsv==NULL) {
		popstate();
		return;
	}
#endif
//	if (!matchcl) {
//		changestatefunc(st_logoninit);
//		return;
//	}
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
//		switch(rm.id) {
	if (focus) {
		S32 ret=focus->proc();
		shape* focusb = ret ? focus : 0;
//		shape* focush = ret>=0 ? focus : 0;
//		shape* focusl = ret>=0 ? focus : 0;
//		shape* focuse = ret>=0 ? focus : 0;
		if (focusb==PBUT1) { // play game
//		case PBUT1:


#if 0
			if (getnmyip()) {
				S32 i;
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
#endif

#if 0
			if (matchcl) { // if here from matchlobby then send a gamepicked token to matching server
// gameinfo hostname gamename ip port,
				strcpy(token,"gamepicked ");
//				strcat(token,findlistboxname(rl,LISTBOX1,getcurlistbox(rl,LISTBOX1)));
				strcat(token,"testA");
//				strcat(token,LISTBOX1->getidxname());
				strcat(token," ");
				strcat(token,ip2str(gametcpsv->ip));
				strcat(token," ");
				char numstr[20];
				strcat(token,my_itoa(gametcpsv->port,numstr,10));
				len=strlen(token);
				tcpwritetoken(matchcl,0,token,len);
				tcpsendwritebuff(matchcl);
			}
#endif
//			changestatefunc(st_game1init);
			changestate(STATE_MATCHGAME1);
			keepsock=2;
//			break;
		} else if (focusb==PBUT2) { // back
//		case PBUT2:
			popstate();
			keepsock=1;
/*			if(matchcl) {
				strcpy(token,"gamepicked"); // cancel gamepicked sent back to matching server
				len=strlen(token);
				tcpwritetoken(matchcl,0,token,len);
				tcpsendwritebuff(matchcl);
			} */
//			break;
		}
	}
}

void match_optionsdraw2d()
{
	cliprect32(B32,0,0,WX-1,WY-1,C32BLACK);
	rl->draw(); // drawres(rl);
	outtextxyf32(B32,WX/2-7*4,10,C32WHITE,"options");
}

void match_optionsexit()
{
#if 1
//	if (gametcpsv==NULL)
//		return;
//	enablenumkeychangestate=ks;
	if (keepsock==1) { // go back to lobby or quickgame
		freegamesockers();
		if (!matchcl)
			uninitsocker();
	} else if (keepsock==0) { // total bail
		freegamesockers();
		if (matchcl) {
			freesocker(matchcl);
			matchcl=NULL;
		}
		uninitsocker();
	}
#endif
	popdir();
//	freeres(rl);
	delete rl;
}
