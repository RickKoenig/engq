/// marching cubes test
#define D2_3D
#include <m_eng.h>
#include "system/u_states.h"
#include "n_marching.h"
#include "utils/json.h"

static U32 togvidmode;
static S32 fpswantedsave,uselightssave;
static tree2* roottree,*marchtree;
static float pot=-1.0f; // isosurface potential
static int marchres = 40; //100;

struct potobj {
	pointf3 pos;
	pointf3 vel;
	float strength;
};

static potobj potobjs[]={
	{{-5,0,0},{.05f,0,0},1},
	{{0,0,0},{0,0,0},1},
};
static const int npotobjs=sizeof(potobjs)/sizeof(potobjs[0]);

static struct menuvar marchingtestvars[]={
	{"@lightred@---- lobby car cam -------------------",0,D_VOID,0},
	{"iso surface potential",&pot,D_FLOAT,FLOATUP/16},
	{"march res",&marchres,D_INT,1},
	{"pot obj 0 x",&potobjs[0].pos.x,D_FLOAT,FLOATUP/16},
	{"pot obj 0 y",&potobjs[0].pos.y,D_FLOAT,FLOATUP/16},
	{"pot obj 0 z",&potobjs[0].pos.z,D_FLOAT,FLOATUP/16},
	{"pot obj 0 vx",&potobjs[0].vel.x,D_FLOAT,FLOATUP/16},
	{"pot obj 0 vy",&potobjs[0].vel.y,D_FLOAT,FLOATUP/16},
	{"pot obj 0 vz",&potobjs[0].vel.z,D_FLOAT,FLOATUP/16},
	{"pot obj 0 strength",&potobjs[0].strength,D_FLOAT,FLOATUP/16},
	{"pot obj 1 x",&potobjs[1].pos.x,D_FLOAT,FLOATUP/16},
	{"pot obj 1 y",&potobjs[1].pos.y,D_FLOAT,FLOATUP/16},
	{"pot obj 1 z",&potobjs[1].pos.z,D_FLOAT,FLOATUP/16},
	{"pot obj 1 vx",&potobjs[1].vel.x,D_FLOAT,FLOATUP/16},
	{"pot obj 1 vy",&potobjs[1].vel.y,D_FLOAT,FLOATUP/16},
	{"pot obj 1 vz",&potobjs[1].vel.z,D_FLOAT,FLOATUP/16},
	{"pot obj 1 strength",&potobjs[1].strength,D_FLOAT,FLOATUP/16},
};
static const S32 nmarchingtestvars=sizeof(marchingtestvars)/sizeof(marchingtestvars[0]);

float isosurface1(float x,float y,float z)
{
	return x*x+y*y+z*z;
}

float isosurface2(float x,float y,float z)
{
	float r2=x*x+y*y+z*z;
	if (r2<.0001f)
		return -100000.0f;
	return -1.0f/sqrtf(r2);
}

float isosurface(float x,float y,float z)
{
	int i;
	float pot=0.0f;
	for (i=0;i<npotobjs;++i) {
		float xd=x-potobjs[i].pos.x;
		float yd=y-potobjs[i].pos.y;
		float zd=z-potobjs[i].pos.z;
		float r2=xd*xd+yd*yd+zd*zd;
		if (r2<.0001f)
			r2=.0001f;
//		pot -= potobjs[i].strength/sqrtf(r2);
		pot -= potobjs[i].strength/r2;
	}
	return pot;
}

static void changepot()
{
	int i;
	for (i=0;i<npotobjs;++i) {
		potobjs[i].pos.x += potobjs[i].vel.x;
		if (potobjs[i].pos.x>10)
			potobjs[i].vel.x=-fabs(potobjs[i].vel.x);
		else if (potobjs[i].pos.x<-10)
			potobjs[i].vel.x=fabs(potobjs[i].vel.x);

		potobjs[i].pos.y += potobjs[i].vel.y;
		if (potobjs[i].pos.y>10)
			potobjs[i].vel.y=-fabs(potobjs[i].vel.y);
		else if (potobjs[i].pos.y<-10)
			potobjs[i].vel.y=fabs(potobjs[i].vel.y);

		potobjs[i].pos.z += potobjs[i].vel.z;
		if (potobjs[i].pos.z>10)
			potobjs[i].vel.z=-fabs(potobjs[i].vel.z);
		else if (potobjs[i].pos.z<-10)
			potobjs[i].vel.z=fabs(potobjs[i].vel.z);
	}
}


static pointf3 boxmin={-10,-10,-10};
static pointf3 boxmax={10,10,10};

////////////////////////// main
void marchingcubestest_init()
{
	float_test();
	json_test();
	logger("\n");
	video_setupwindow(GX,GY);
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=0;
	roottree=new tree2("roottree");
/*
// draw a test march object
	marchtree=march_test(1.0f,20,isosurface);
//	marchtree->trans.x=3.5f;
	roottree->linkchild(marchtree);
*/
// make a viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=100;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z=-10;
	mainvp.camtrans.x=0;
	mainvp.camtrans.y=0;
	mainvp.flags=VP_CLEARBG|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	fpswantedsave=wininfo.fpswanted;
	wininfo.fpswanted=20;
	togvidmode=videoinfo.video_maindriver;
	extradebvars(marchingtestvars,nmarchingtestvars);
}

void marchingcubestest_proc()
{
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_MARCHINGCUBESTEST);
	}
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 'a':
		showcursor(1);
		break;
	case 'h':
		showcursor(0);
		break;
	case 's':
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_MARCHINGCUBESTEST);
		break;
	case '=':
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_MARCHINGCUBESTEST);
		break;
	case '-':
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_MARCHINGCUBESTEST);
		break;
	}
	mainvp.xres=WX;
	mainvp.yres=WY;
	changepot();
// build a test march object
	delete marchtree;
//	marchtree=march_test(1.0f,20,isosurface);
	marchtree=march_test(pot,marchres,boxmin,boxmax,isosurface);
//	marchtree->trans.x=3.5f;
	roottree->linkchild(marchtree);
	roottree->proc();
	doflycam(&mainvp);
}

void marchingcubestest_draw3d()
{
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	video_drawscene(roottree);
}

void marchingcubestest_exit()
{
	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	marchtree=0;
	wininfo.fpswanted=fpswantedsave;
	lightinfo.uselights=uselightssave;
}
