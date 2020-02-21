#define RES3D // alternate shape class
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "match_title.h"
static shape *rl;
static shape* focus;
static pbut* LOGON; // logon 
static pbut* REGISTER; // edit username
static pbut* QUIT; // quit

static fontq* afont;

void match_titleinit()
{
	focus=0;
	logger("logon init\n");
	video_setupwindow(800,600);
	pushandsetdir("fonts");
	afont=new fontq("med");
	afont->setspace(20);
	afont->setfixedwidth(20);
	afont->setscale(.5f);
	popdir();
	pushandsetdir("lobbysystem");
//	rl=loadres("logonres.txt");
//	nsc=0;
//	sc=NULL;
/*
	factory2<shape> fact;
	script* sc=new script("logonres.txt");
	shape* rls=fact.newclass_from_handle(*sc);
	rl=dynamic_cast<shaperoot*>(rls);
	if (!rl)
		errorexit("can't find shaperoot");
	delete sc;
*/
	init_res3d(800,600);
	rl = res_loadfile("titleres.txt");
	LOGON=rl->find<pbut>("LOGON");
	REGISTER=rl->find<pbut>("REGISTER");
	QUIT=rl->find<pbut>("QUIT");
}
	
void match_titleproc()
{
	if (KEY==K_ESCAPE) {
		poporchangestate(STATE_MAINMENU);
//		popstate();
	}
// ui
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0;
		if (focusa==LOGON) {
			changestate(STATE_MATCHLOGON);
		} else if (focusa==REGISTER) {
			changestate(STATE_MATCHREGISTER);
		} else if (focusa==QUIT) {
			popstate();
		}
	}
// end ui
}

#define FSX 400
#define FSY 12
#if 0
void match_titledraw2d()
{
//	cliprect32(B32,0,0,WX,WY,C32BLACK);
	rl->draw();
//	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"The Racing Network. TRN");
//	outtextxyf32(B32,0,0,C32WHITE,"focus = %p",focus);
}
#endif

void match_titledraw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	afont->print((float)WX/2-35*4,10,FSX,FSY,F32BLUE,"The Racing Network. TRN");
	afont->print(0,0,FSX,FSY,F32BLUE,"focus = %p",focus);
//	afont->print(800/2-25*4,10,FSX,FSY,F32WHITE,"logon to 'TRN' (match server)");
	video_sprite_end(); 
}

void match_titleexit()
{
	delete rl;
	popdir();
	delete afont;
	afont=0;
	exit_res3d();
}
