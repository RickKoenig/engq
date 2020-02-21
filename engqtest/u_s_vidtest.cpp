// drag and drop bwo and bws files, test d3d and software and bwo and bws, currently depends of occusim
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

#include "helperobj.h"

static U32 togvidmode; // 0,1,2,3 gdi,ddraw,dx9win,dx9fs
//static S32 uselightssave;
static tree2* roottree;

//static C8 dir[MAX_PATH]="fortpoint";
//static C8 fname[MAX_PATH]="fp7opt.bws";
//static C8 fname[MAX_PATH]="entrance_sign.bwo";

//static C8 dir[MAX_PATH]="c:/srcw32/occusim/game/data/npc/sam_shortback/model";
//static C8 fname[MAX_PATH]="sam_shortback.bws";
// static C8 fname[MAX_PATH]="torso_g.bwo";

//static C8 dir[MAX_PATH]="toon";
//static C8 fname[MAX_PATH]="teapot.bws";

//static C8 dir[MAX_PATH]="headon";
//static C8 fname[MAX_PATH]="headon.bws";

static C8 dir[MAX_PATH]="gfxtest";
static C8 fname[MAX_PATH]="spectest.bws";

//static C8 dir[MAX_PATH]="cointest";
//static C8 fname[MAX_PATH]="coin01.bwo";

//static C8 dir[MAX_PATH]="gfxtest/coliseum";
//static C8 fname[MAX_PATH]="coliseum.bws";

//static C8 dir[MAX_PATH]="racenetdata/prehistoric";
//static C8 fname[MAX_PATH]="prehistoric.bws";

//static C8 dir[MAX_PATH]="racenetdata/thefalls";
//static C8 fname[MAX_PATH]="thefalls.bws";

//static C8 dir[MAX_PATH]="headon";
//static C8 fname[MAX_PATH]="beachhead.bwo";

static helperobj* ho;
static pointf3 pnts[2];
static struct menuvar edv[]={
// user vars
	{"@lightred@---- HELPEROBJ USER VARS -----------------",NULL,D_VOID,0},
	{"Point A x",&pnts[0].x,D_FLOAT,FLOATUP/4},
	{"Point A y",&pnts[0].y,D_FLOAT,FLOATUP/4},
	{"Point A z",&pnts[0].z,D_FLOAT,FLOATUP/4},
	{"Point B x",&pnts[1].x,D_FLOAT,FLOATUP/4},
	{"Point B y",&pnts[1].y,D_FLOAT,FLOATUP/4},
	{"Point B z",&pnts[1].z,D_FLOAT,FLOATUP/4},
};
#define NEDV sizeof(edv)/sizeof(edv[0])

////////////////////////// main
void vidtestinit()
{
/*	C8* test=(C8*)memalloc(5);
	S32 i;
	for (i=0;i<5;++i)
		test[i]=1+i;
	test=(C8*)memrealloc(test,6);
	test[5]=6;
	for (i=0;i<6;++i)
		logger("memtest %d: %d\n",i,test[i]);
	memfree(test); */
	video_setupwindow(GX,GY);
	extradebvars(edv,NEDV);
//	uselightssave=lightinfo.uselights;
//	lightinfo.uselights=0;
	togvidmode=videoinfo.video_maindriver;
// setup trees
	roottree=new tree2("roottree");
	pushandsetdir(dir);
//	unchunktest(fname,10);
	tree2* onefacemodeltree=new tree2(fname); // a name with a .bwo or .bws will load and build them
	popdir();
	roottree->linkchild(onefacemodeltree);
	seq_start(roottree);

#if 0
// make more copies (1 of 2)
	onefacemodeltree=onefacemodeltree->newdup();
	onefacemodeltree->trans.x=-30;
	roottree->linkchild(onefacemodeltree);
// make more copies (2 of 2)
	onefacemodeltree=onefacemodeltree->newdup();
	onefacemodeltree->trans.x=30;
	onefacemodeltree->rotvel.y=.05f; // a little spin
	roottree->linkchild(onefacemodeltree);
#endif
// setup viewport
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	static bool once=true;
	if (once) {
		mainvp.backcolor=C32LIGHTBLUE;
		mainvp.zfront=.125f;
		mainvp.zback=20000;
		mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
		mainvp.camtrans.z=0;//-100;
		mainvp.camtrans.x=0;
		mainvp.camtrans.y=0;// 50;
		mainvp.camrot=pointf3x(0,0,0);
		once=false;
	}
	mainvp.flags=VP_CLEARBG|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
pushandsetdir("skybox");
		make_envv2tex("CUB_cubemap_mountains.jpg");
popdir();
	ho=new helperobj(/*roottree*/);
	pnts[0]=pointf3x(0,0,3);
	pnts[1]=pointf3x(1,1,4);
}

void vidtestproc()
{
	ho->reset();
//	ho->addsphere(roottree,pnts[1],.1f,F32WHITE);
//	ho->addjack(roottree,pnts[0],.6f,F32LIGHTMAGENTA);
//	ho->addsphere(roottree,pointf3x(3,0,0),2.0f,pointf3x(1,0,0));
//	ho->addsphere(roottree,pointf3x(8,0,0),3.0f,pointf3x(0,1,0));
//	ho->addsphere(roottree,pointf3x(15,0,0),4.0f,pointf3x(0,0,1));
	ho->addvector(roottree,pnts[0],pnts[1],pointf3x(0,1,0));
	pointf3x p0(pnts[0].x+.2f,pnts[0].y,pnts[0].z);
	pointf3x p1(pnts[1].x+.2f,pnts[1].y,pnts[1].z);
	ho->addline(roottree,p0,p1,pointf3x(1,1,0));
//	ho->addbox(roottree,pnts[0],pnts[1],pointf3x(1,1,0,.5f));
// input
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_VIDTEST);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_VIDTEST);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_VIDTEST);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_VIDTEST);
	}
	if (wininfo.justdropped) {
//		script* sc=wininfo.args;
		mgetpath((*wininfo.args).idx(0).c_str(),dir);
		mgetnameext((*wininfo.args).idx(0).c_str(),fname);
		changestate(STATE_VIDTEST);
	}
// incase window changed size
	mainvp.xres=WX;
	mainvp.yres=WY;
// proc
	roottree->proc();
	doflycam(&mainvp);
}

void vidtestdraw3d()
{
// draw
	video_buildworldmats(roottree); // help dolights
	dolights();
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
}

void vidtestexit()
{
	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree; // free the main man
//	lightinfo.uselights=uselightssave;
	free_envv2tex();
	delete ho;
}
