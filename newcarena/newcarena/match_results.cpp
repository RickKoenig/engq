#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_logon.h"
#include "match_game1.h"

//static struct reslist *rl;
static shape* rl;
static shape* focus;
static pbut* PBUT1; // pop back to lobby / quickgame
static text* TEXT1; // game 'a' results
static listbox* LISTBOX1; // list of results

//static int keepsock;

void match_results_init()
{
	int i,j,k;
	char name[50],str[50];
//	if (gametcpsv==NULL && gametcpcl==NULL)
//		return;
//	freegamesockers();
//	keepsock=0;
	logger("results init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
//	rl=loadres("resultsres.txt");
	factory2<shape> fact;
	script* sc=new script("resultsres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	popdir();
	TEXT1=rl->find<text>("TEXT1");
	PBUT1=rl->find<pbut>("PBUT1");
	LISTBOX1=rl->find<listbox>("LISTBOX1");
//	setresname(rl,TEXT1,"Game 'A' results");
	TEXT1->settname("Game 'A' results");
	for (i=0;i<8;i++) {
		k=mt_random(10)+3;
		for (j=0;j<k;j++)
			name[j]=mt_random(26)+'a';
		name[0]+='A'-'a';
		name[k]='\0';
		sprintf(str,"%15s %d",name,mt_random(10000));
//		addlistboxname(rl,LISTBOX1,-1,str,-1);
		LISTBOX1->addstring(str);
	}
	focus=0;
}

void match_results_proc()
{
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
//	struct rmessage rm;
// shouldn't be here
//	if (gametcpsv==NULL && gametcpcl==NULL) {
//		popstate();
//		return;
//	}
	if (KEY==K_ESCAPE) {
		popstate();
//		keepsock=1;
	}
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
	if (focus) {
//		int comlistidx=-1;
		S32 ret=focus->proc();
		shape* focusb = ret ? focus : 0;
//		shape* focush = ret>=0 ? focus : 0;
//		shape* focusl = ret>=0 ? focus : 0;
//		shape* focuse = ret>=0 ? focus : 0;
//		switch(rm.id) {
//		switch(rm.id) {
//		case PBUT1:
		if (focusb==PBUT1) {
			popstate();
//			keepsock=1;
//			break;
		}
	}
}

void match_results_draw2d()
{
	cliprect32(B32,0,0,WX-1,WY-1,C32BLACK);
	rl->draw();
	outtextxyf32(B32,WX/2-7*4,10,C32WHITE,"results");
}

void match_results_exit()
{
	if (wininfo.closerequested && matchcl) {
		freesocker(matchcl);
		matchcl=0;
	}
//	if (gametcpsv==NULL && gametcpcl==NULL)
//		return;
/*	if (!keepsock) {
		if (matchcl) {
			freesocker(matchcl);
			matchcl=NULL;
		}
		uninitsocker();
	} else if (!matchcl) // quickgame, free socket system
		uninitsocker(); */
//	popdir();
//	freeres(rl);
	delete rl;
}

