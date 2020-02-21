// test half pixel stuff
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

#include <d3d9.h>
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "u_modelutil.h"

static U32 togvidmode;
static tree2* roottree;
static S32 uselightssave;

// newer one with textures
static struct pointf3 rectverts[]={
//	{64.0-1.0,1.0,0},
//	{64.0- .2,1.0,0},
//	{64.0-1.0, .2,0},
//	{64.0- .2, .2,0},
	{ .2f,48.0f- .2f,0},
	{1.0f,48.0f- .2f,0},
	{ .2f,48.0f-1.0f,0},
	{1.0f,48.0f-1.0f,0},
//	{10,20,0},
//	{50,25,0},
//	{20,30,0},
};

static struct uv rectuvs[]={
//	{0,0},
//	{.5f,0},
//	{0,.5f},
//	{.5f,.5f},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
};

static struct face rectfaces[]={
	{{0,1,2},0},
	{{2,1,3},0},
};

static struct menuvar evars[]={
	{"point 0 x",&rectverts[0].x,D_FLOAT,FLOATUP/8},
	{"point 0 y",&rectverts[0].y,D_FLOAT,FLOATUP/8},
	{"point 1 x",&rectverts[1].x,D_FLOAT,FLOATUP/8},
	{"point 1 y",&rectverts[1].y,D_FLOAT,FLOATUP/8},
	{"point 2 x",&rectverts[2].x,D_FLOAT,FLOATUP/8},
	{"point 2 y",&rectverts[2].y,D_FLOAT,FLOATUP/8},
	{"point 3 x",&rectverts[3].x,D_FLOAT,FLOATUP/8},
	{"point 3 y",&rectverts[3].y,D_FLOAT,FLOATUP/8},
	{"uv 0 x",&rectuvs[0].u,D_FLOAT,FLOATUP/32},
	{"uv 0 y",&rectuvs[0].v,D_FLOAT,FLOATUP/32},
	{"uv 1 x",&rectuvs[1].u,D_FLOAT,FLOATUP/32},
	{"uv 1 y",&rectuvs[1].v,D_FLOAT,FLOATUP/32},
	{"uv 2 x",&rectuvs[2].u,D_FLOAT,FLOATUP/32},
	{"uv 2 y",&rectuvs[2].v,D_FLOAT,FLOATUP/32},
	{"uv 3 x",&rectuvs[3].u,D_FLOAT,FLOATUP/32},
	{"uv 3 y",&rectuvs[3].v,D_FLOAT,FLOATUP/32},
};
static const S32 nevars=sizeof(evars)/sizeof(evars[0]);
static tree2* onefacemodeltree;
static modelb* dumm;

static modelb* buildthemodel()
{
	modelb* modb=model_create("amod");
	if (model_getrc(modb)==1) {
		modb->copyverts(rectverts,4);
		modb->copyuvs0(rectuvs,4);
		pushandsetdir("gfxtest");
//		textureb* texmat=texture_create("bark.tga");
		textureb* texmat=texture_create("test8x8.pcx");
		if (texture_getrc(texmat)==1) {
//			colorkeyinfo.usecolorkey=0;
			texmat->load();
//			colorkeyinfo.usecolorkey=1;
		}
		popdir();
//		modb->addmat("tex",0,1.0f,texmat,&F32RED,2,4);
		modb->addmat("tex",SMAT_HASTEX,texmat,&F32WHITE,30,2,4);
//		modb->addmat("amat",0,1.0f,0,&F32YELLOW,1,3);
		modb->addfaces(rectfaces,2,0);
		modb->close();
	}
	return modb;
}

////////////////////////// main
void tritest2init()
{
	video_setupwindow(GX,GY);
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=0;
	roottree=new tree2("roottree");
	onefacemodeltree=new tree2("built");
	dumm=model_create("dumm");
	pushandsetdir("gfxtest");
	textureb* texmat=texture_create("maptest.tga");
	if (texture_getrc(texmat)==1) {
//		colorkeyinfo.usecolorkey=0;
		texmat->load();
//		colorkeyinfo.usecolorkey=1;
	}
	popdir();
	dumm->addmat("tex",SMAT_HASTEX,texmat,&F32WHITE,30,0,0);
	dumm->close();
	modelb* mod=buildthemodel();
	onefacemodeltree->setmodel(mod);
	roottree->linkchild(onefacemodeltree);
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=10000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.x=32.0;
	mainvp.camtrans.y=24.0;
	mainvp.camtrans.z=-24.0;
	mainvp.camrot=zerov;
	mainvp.flags=VP_CLEARBG;//|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	togvidmode=videoinfo.video_maindriver;
	extradebvars(evars,nevars);
}

void tritest2proc()
{
	delete onefacemodeltree;
	onefacemodeltree=new tree2("built");
	modelb* mod=buildthemodel();
	onefacemodeltree->setmodel(mod);
	roottree->linkchild(onefacemodeltree);
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
		changestate(STATE_TRITEST2);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST2);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST2);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST2);
	}
	mainvp.xres=WX;
	mainvp.yres=WY;
	roottree->proc();
	doflycam(&mainvp);
}

void tritest2draw3d()
{
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
}

void tritest2draw2d()
{
//	video_lock();
	S32 i,j;
	for (j=0;j<16;++j)
		for (i=0;i<16;++i) {
			C32 c=clipgetpixel32(B32,i,j);
//			C32 c=clipgetpixel32(B32,i+640-16,j+480-16);
			if (c.r==C32LIGHTBLUE.r && c.g==C32LIGHTBLUE.g && c.b==C32LIGHTBLUE.b)
				c=C32BLACK;
			else
				;//logger("c=%d,%d,%d\n",c.r,c.g,c.b);
			cliprect32(B32,16+i*16,16+j*16,15,15,c);
//			cliprect32(B32,350+i*16,150+j*16,15,15,c);
		}
//	video_unlock(); 
}

void tritest2exit()
{
	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	lightinfo.uselights=uselightssave;
	modelb::rc.deleterc(dumm);
//	model_destroy(dumm);
}
