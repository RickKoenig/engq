#define RES3D // alternate shape class
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
//#include <l_res3d.h>
#include "../u_states.h"

static shape *rl;
static shape* focus;
static listbox* LISTBOX2; // server ip list
static edit* EDIT2; // edit ip address
static pbut* PBUT4; // remove server ip list
static listbox* LISTBOX3; // server port list
static edit* EDIT3; // edit ports
static pbut* PBUT5; // remove server port list
static pbut* PBUT6; // quit

static fontq* afont;

void match_pickserverinit()
{
	int i;
	logger("logon init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
	pushandsetdir("fonts");
	afont=new fontq("med");
	afont->setspace(20);
	afont->setfixedwidth(20);
	afont->setscale(.5f);
	popdir();
	init_res3d(800,600);
	rl = res_loadfile("pickserverres.txt");
	S32 nsc;
	LISTBOX2=rl->find<listbox>("LISTBOX2");
	EDIT2=rl->find<edit>("EDIT2");
	PBUT4=rl->find<pbut>("PBUT4");
	LISTBOX3=rl->find<listbox>("LISTBOX3");
	EDIT3=rl->find<edit>("EDIT3");
	PBUT5=rl->find<pbut>("PBUT5");
	PBUT6=rl->find<pbut>("PBUT6");
	script* sc;
	sc=new script("pickservercfg.txt");
	nsc=sc->num();
	for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"ip"))
			LISTBOX2->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"port"))
			LISTBOX3->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"ipidx"))
			LISTBOX2->setidxc(atoi(sc->idx(i+1).c_str()));
		else if (!strcmp(sc->idx(i).c_str(),"portidx"))
			LISTBOX3->setidxc(atoi(sc->idx(i+1).c_str()));
		else
			errorexit("unknown logon cfg command '%s'",sc->idx(i).c_str());
	}
	delete sc;
	initsocker();
	focus=EDIT2;
	EDIT2->settname(LISTBOX2->getidxname());
	EDIT3->settname(LISTBOX3->getidxname());
}
	
void match_pickserverproc()
{
	if (KEY==K_ESCAPE) {
		changestate(STATE_MATCHTITLE);
	}
	int i;
// ui
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		if (KEY==K_TAB) {
			if (focus==EDIT2)
				focus=EDIT3;
			else if (focus==EDIT3)
				focus=EDIT2;
		}
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0;
// ips
		if (focusa==LISTBOX2) {
			EDIT2->settname(LISTBOX2->getidxname());
		} else if (focusa==EDIT2) {
			i=LISTBOX2->findstring(EDIT2->gettname());	// -1 if not found
			if (i>=0) {
				LISTBOX2->setidxc(i);
			} else {
				LISTBOX2->addstring(EDIT2->gettname());
				LISTBOX2->sort(listbox::SK_IPS);
				LISTBOX2->setidxc();
			}
			EDIT2->settname("");
		} else if (focusa==PBUT4) {
			LISTBOX2->removeidx();
// ports
		} else if (focusa==LISTBOX3) {
			EDIT3->settname(LISTBOX3->getidxname());
		} else if (focusa==EDIT3) {
			i=LISTBOX3->findstring(EDIT3->gettname());	// -1 if not found
			if (i>=0) {
				LISTBOX3->setidxc(i);
			} else {
				LISTBOX3->addstring(EDIT3->gettname());
				LISTBOX3->sort(listbox::SK_INTVAL);
				LISTBOX3->setidxc();
			}
			EDIT3->settname("");
		} else if (focusa==PBUT5) {
			LISTBOX3->removeidx();
// back	
		} else if (focusa==PBUT6) {
			popstate();
		}
	}
// end ui
}

#if 0
void match_pickserverdraw2d()
{
	cliprect32(B32,0,0,WX,WY,C32BLACK);
	rl->draw();
	outtextxyf32(B32,WX/2-25*4,10,C32WHITE,"pick server");
	if (LISTBOX2->getidx()>=0)
		outtextxyf32(B32,WX/2-25*4,40,C32WHITE,"ip: '%s'",LISTBOX2->getidxname());
	if (LISTBOX3->getidx()>=0)
		outtextxyf32(B32,WX/2-25*4,50,C32WHITE,"port '%s'",LISTBOX3->getidxname());
	outtextxyf32(B32,0,0,C32WHITE,"focus = %p",focus);
}
#endif

#define FSX 400
#define FSY 12
void match_pickserverdraw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
//	afont->print(50,50,50,20,F32YELLOW,"LAP");
	afont->print((float)WX/2-25*4,10,FSX,FSY,F32WHITE,"pick server");
	if (LISTBOX2->getidx()>=0)
		afont->print((float)WX/2-25*4,40,FSX,FSY,F32WHITE,"ip: '%s'",LISTBOX2->getidxname());
	if (LISTBOX3->getidx()>=0)
		afont->print((float)WX/2-25*4,50,FSX,FSY,F32WHITE,"port '%s'",LISTBOX3->getidxname());
	afont->print(0,0,FSX,FSY,F32WHITE,"focus = %p",focus);
	video_sprite_end(); 
}

void match_pickserverexit()
{
	FILE *fp;
	int i,nlb;
	fp=fopen2("pickservercfg.txt","wb");
	nlb=LISTBOX2->getnumidx();
	for (i=0;i<nlb;i++)
		fprintf(fp,"ip \"%s\"\n",LISTBOX2->getidxname(i));
	nlb=LISTBOX3->getnumidx();
	for (i=0;i<nlb;i++)
		fprintf(fp,"port \"%s\"\n",LISTBOX3->getidxname(i));
	fprintf(fp,"ipidx %d\n",LISTBOX2->getidx());
	fprintf(fp,"portidx %d\n",LISTBOX3->getidx());
	fclose(fp);
	popdir();
	delete rl;
	delete afont;
	afont=0;
	exit_res3d();
}
