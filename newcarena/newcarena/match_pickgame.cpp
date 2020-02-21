#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_game1.h"
#include "match_logon.h"

static shape* rl;
static shape* focus;
static listbox* LISTBOX1; // list of games to pick
static pbut* PBUT1; // move on to options
static pbut* PBUT2; // back to lobby or quickgame
//static int ks;
//extern int enablenumkeychangestate;
static int keepsock;

void match_pickgameinit()
{
//	if (gametcpsv==NULL)
//		return;
//	ks=enablenumkeychangestate;
//	enablenumkeychangestate=0;
	keepsock=0;
	logger("pickgame init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
//	rl=loadres("pickgameres.txt");
	factory2<shape> fact;
	script* sc=new script("pickgameres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	LISTBOX1=rl->find<listbox>("LISTBOX1");
	PBUT1=rl->find<pbut>("PBUT1");
	PBUT2=rl->find<pbut>("PBUT2");
//	LISTBOX1->addstring("testA");
//	if (matchcl)
//		LISTBOX1->addstring("Carena");
	S32 i,n; // build a game list
	n=getngames();
	for (i=0;i<n;++i) {
		bool q=canbequick(i);
		if (q || matchcl)
			LISTBOX1->addstring(getgamename(i));
	}
//	setcurlistbox(rl,LISTBOX1,0,SCL1);
	LISTBOX1->setidxc(LISTBOX1->getnumidx()-1); // last one, prevent carena from quickgame
	focus=0;
}

void match_pickgameproc()
{
	if (KEY==K_ESCAPE) {
		keepsock=1;
		popstate();
	}
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
//	int len;
//	char token[150];
//	char numstr[20];
//	struct rmessage rm;
/*	if (gametcpsv==NULL) {
		popstate();
		return;
	} */
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
//		switch(rm.id) {
// ui
	if (focus) {
		S32 ret=focus->proc();
		shape* focusb = ret ? focus : 0;
//		shape* focush = ret>=0 ? focus : 0;
//		shape* focusl = ret>=0 ? focus : 0;
//		shape* focuse = ret>=0 ? focus : 0;
//		switch(rm.id) {
		if (focusb==PBUT1) { // move on to options
//		case PBUT1:
			keepsock=2;
/*			if (matchcl) { // if here from matchlobby then send a gamepicked token to matching server
// gameinfo hostname gamename ip port,
				strcpy(token,"gamepicked ");
//				strcat(token,findlistboxname(rl,LISTBOX1,getcurlistbox(rl,LISTBOX1)));
				strcat(token,LISTBOX1->getidxname());
				strcat(token," ");
				strcat(token,ip2str(gametcpsv->ip));
				strcat(token," ");
				strcat(token,my_itoa(gametcpsv->port,numstr,10));
				len=strlen(token);
				tcpwritetoken(matchcl,0,token,len);
				tcpsendwritebuff(matchcl);
			} */
//			changestatefunc(st_optionsinit);
			const C8* gn=LISTBOX1->getidxname();
			S32 gidx=getgameidx(gn);
			cgi.gameidx=gidx;
			S32 ns=getgamehoststate(gidx);
			changestate(ns);
			cgi.port=getgamemasterport(gidx);
//			break;
//		case PBUT2:
		} else if (focusb==PBUT2) { // back to lobby/quickgame
			keepsock=1;
			cgi.isvalid=false;
			popstate();
//			break;
/*		case SCL1:
			setlistboxoffset(rl,LISTBOX1,rm.val);
			break; */
		}
	}
// end ui
}

void match_pickgamedraw2d()
{
	cliprect32(B32,0,0,WX-1,WY-1,C32BLACK);
	rl->draw(); //drawres(rl);
	outtextxyf32(B32,WX/2-9*4,10,C32WHITE,"pick game");
}

void match_pickgameexit()
{
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
/* else if keepsock==2 then going to game, no freeing is necc. */
	popdir();
//	freeres(rl);
	delete rl;
}
