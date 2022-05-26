// self driving car
#include <m_eng.h>
#include <l_misclibm.h>
#include "../u_states.h"

static bitmap32* threatbm2;
static shape *rl, *focus, *oldfocus;
static hscroll* hslide1, *hslide2;
static vscroll* vslide1, *vslide2, *vslideiter;
static text* textiter, *textzoom, *text1, *text2;
static pbut* pbutzoomin, *pbutzoomout, *pbutquit;

void selfdrivinginit()
{
	//changeWindowTitle("this is self driving");
	video_setupwindow(1024, 768);
	pushandsetdir("scratch");
	threatbm2 = gfxread32("dhs_threat1.jpg");
	popdir();
	pushandsetdir("selfdriving");
	script sc;
	rl = res_loadfile("selfdriving.txt");
	hslide1 = rl->find<hscroll>("HSLIDE1");
	hslide2 = rl->find<hscroll>("HSLIDE2");
	vslide1 = rl->find<vscroll>("VSLIDE1");
	vslide2 = rl->find<vscroll>("VSLIDE2");
	vslideiter = rl->find<vscroll>("VSLIDEITER");
	textiter = rl->find<text>("TEXTITER");
	textzoom = rl->find<text>("TEXTZOOM");
	text1 = rl->find<text>("TEXT1");
	text2 = rl->find<text>("TEXT2");
	pbutzoomin = rl->find<pbut>("PBUTZOOMIN");
	pbutzoomout = rl->find<pbut>("PBUTZOOMOUT");
	pbutquit = rl->find<pbut>("PBUTQUIT");
	//	setresrange(rl,HSLIDE1,-256,256);
	hslide1->setminmaxval(-1024, 1024);
	//	setresrange(rl,HSLIDE2,-256,256);
	hslide2->setminmaxval(-1024, 1024);
	//	setresrange(rl,VSLIDE1,-256,256);
	vslide1->setminmaxval(-1024, 1024);
	//	setresrange(rl,VSLIDE2,-256,256);
	vslide2->setminmaxval(-1024, 1024);
	//	setresrange(rl,ITER,1,10000);
	vslideiter->setminmaxval(1, 10000);
	//xc = xf = yc = yf = 0;
	//	setresval(rl,HSLIDE1,xc);
	hslide1->setidx(10);
	//	setresval(rl,HSLIDE2,xf);
	hslide2->setidx(20);
	//	setresval(rl,VSLIDE1,yc);
	vslide1->setidx(30);
	//	setresval(rl,VSLIDE2,yf);
	vslide2->setidx(40);
	//	setresval(rl,ITER,10000);
	{
		char str[100];
		int iter = 1234;
		vslideiter->setidx(10001 - iter);
		iter = 10001 - vslideiter->getidx();
		sprintf(str, "%d", iter);
		textiter->settname(str);
	}
	//	setresname(rl,ITERTEXT,str);
	//dozoom(0);
	focus = oldfocus = 0;
}

void selfdrivingproc()
{
	//	changestate(NOSTATE);
	if (KEY == K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	// handle buttons and editboxes
	if (wininfo.mleftclicks || wininfo.mrightclicks) {
		focus = rl->getfocus();
	}
	S32 ret = -1;
	if (focus) {
		ret = focus->proc();
	}
	if (oldfocus && oldfocus != focus) {
		oldfocus->deactivate();
	}
	oldfocus = focus;
	if (focus == pbutquit) {
		if (ret == 1 || ret == 2) {
			poporchangestate(STATE_MAINMENU);
		}
	} else if (focus == vslideiter) {
		int iter = 10001 - ret;
		char str[100];
		sprintf(str, "%d", iter);
		//		setresname(rl,ITERTEXT,str);
		textiter->settname(str);
	}
}

void selfdrivingdraw2d()
{
	C32 col;
	clipclear32(B32, C32(0, 0, 255));
	clipblit32(threatbm2, B32, 0, 0, 20, 20, threatbm2->size.x, threatbm2->size.y);
	rl->draw();
}

void selfdrivingexit()
{
	bitmap32free(threatbm2);
	delete rl;
	popdir();
}
