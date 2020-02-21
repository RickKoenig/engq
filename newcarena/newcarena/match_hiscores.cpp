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
static pbut* PBUT1;
static text* TEXT1;
static listbox* LISTBOX1;

static int keepsock=0;

void match_hiscoresinit()
{
	int i,j,k;
	char str[100];
	char name[100];
//	if (!matchcl)
//		return;
	keepsock=0;
	logger("hiscores init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
//	rl=loadres("hiscoresres.txt");
	factory2<shape> fact;
	script* sc=new script("hiscoresres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	TEXT1=rl->find<text>("TEXT1");
	PBUT1=rl->find<pbut>("PBUT1");
	LISTBOX1=rl->find<listbox>("LISTBOX1");
	for (i=0;i<1000;i++) {
		k=mt_random(10)+3;
		for (j=0;j<k;j++)
			name[j]=mt_random(26)+'a';
		name[0]+='A'-'a';
		name[k]='\0';
		sprintf(str,"%05d %s",mt_random(10000),name);
//		addlistboxname(rl,LISTBOX1,-1,str,SCL1);
		LISTBOX1->addstring(str);
	}
//	sortlistbox(rl,LISTBOX1,-1,-1,-1,1);
	LISTBOX1->sort(listbox::SK_REVERSE);
	LISTBOX1->setidxv(0);
	focus=0;
}

void match_hiscoresproc()
{
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
//	struct rmessage rm;
//	if (!matchcl) {
//		changestatefunc(st_logoninit);
//		return;
//	}
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
//		case PBUT1:
		if (focusb==PBUT1) {
			keepsock=1;
			changestate(STATE_MATCHLOBBY);
//			break;
		}
//		case SCL1:
//			setlistboxoffset(rl,LISTBOX1,rm.val);
//			break;
	}
} 

void match_hiscoresdraw2d()
{
	cliprect32(B32,0,0,WX-1,WY-1,C32BLACK);
	rl->draw(); //drawres(rl);
	outtextxyf32(B32,WX/2-9*4,10,C32WHITE,"hi scores");
}

void match_hiscoresexit()
{
//	if (!matchcl)
//		return;
	if (!keepsock) {
		freesocker(matchcl);
		matchcl=NULL;
//		uninitsocker();
	}
	popdir();
//	freeres(rl);
	delete rl;
}
