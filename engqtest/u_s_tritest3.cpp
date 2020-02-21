// test multi viewports
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

#include <d3d9.h>
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "u_modelutil.h"

static U32 togvidmode;
static tree2* roottree;
static S32 uselightssave;
static viewport2 mainvp2;

// newer one with textures
static struct pointf3 rectverts[]={
	{-1, 1,1},
	{ 1, 1,1},
	{-1,-1,1},
	{ 1,-1,1},
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
	{"point 0 z",&rectverts[0].z,D_FLOAT,FLOATUP/8},
	{"point 1 x",&rectverts[1].x,D_FLOAT,FLOATUP/8},
	{"point 1 y",&rectverts[1].y,D_FLOAT,FLOATUP/8},
	{"point 1 z",&rectverts[1].z,D_FLOAT,FLOATUP/8},
	{"point 2 x",&rectverts[2].x,D_FLOAT,FLOATUP/8},
	{"point 2 y",&rectverts[2].y,D_FLOAT,FLOATUP/8},
	{"point 2 z",&rectverts[2].z,D_FLOAT,FLOATUP/8},
	{"point 3 x",&rectverts[3].x,D_FLOAT,FLOATUP/8},
	{"point 3 y",&rectverts[3].y,D_FLOAT,FLOATUP/8},
	{"point 3 z",&rectverts[3].z,D_FLOAT,FLOATUP/8},
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
void tritest3init()
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
	dumm->addmat("tex",SMAT_HASTEX,texmat,&F32RED,30,0,0);
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
//	mainvp.camtrans.x=32.0;
//	mainvp.camtrans.y=24.0;
//	mainvp.camtrans.z=-24.0;
	mainvp.flags=VP_CLEARBG;//|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;

	mainvp2.backcolor=C32LIGHTGREEN;
	mainvp2.zfront=.25f;
	mainvp2.zback=10000;
	mainvp2.xres=WX;
	mainvp2.yres=WY;
	mainvp2.xstart=0;
	mainvp2.ystart=0;
	mainvp2.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
//	mainvp2.camtrans.x=32.0;
//	mainvp2.camtrans.y=24.0;
//	mainvp2.camtrans.z=-24.0;
	mainvp2.flags=VP_CLEARBG;//|VP_CHECKER|VP_CLEARWB;
	mainvp2.xsrc=4;
	mainvp2.ysrc=3;
	mainvp2.useattachcam=false;
//	mainvp2.lookat=0;

	togvidmode=videoinfo.video_maindriver;
	extradebvars(evars,nevars);
}

void tritest3proc()
{
	delete onefacemodeltree;
	onefacemodeltree=new tree2("built");
	modelb* mod=buildthemodel();
	onefacemodeltree->setmodel(mod);
	roottree->linkchild(onefacemodeltree);
	if (KEY==K_ESCAPE)
		popstate();
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST3);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST3);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST3);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST3);
	}
	mainvp.xres=WX/2;
	mainvp.yres=WY/2;
	mainvp.xstart=WX/8;
	mainvp.ystart=WY/8;
	roottree->proc();
}

void tritest3draw3d()
{
	doflycam(&mainvp);
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
	mainvp2.xres=WX/4;
	mainvp2.yres=WY/4;
	mainvp2.xstart=6*WX/8;
	mainvp2.ystart=6*WY/8;
//	roottree->proc();
	doflycam(&mainvp2);
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp2); // clear zbuf etc.
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
/*	video_lock();
	S32 i,j;
	for (j=0;j<16;++j)
		for (i=0;i<16;++i) {
			C32 c=clipgetpixel32(B32,i,j);
//			C32 c=clipgetpixel32(B32,i+640-16,j+480-16);
			if (c.r==C32LIGHTBLUE.r && c.g==C32LIGHTBLUE.g && c.b==C32LIGHTBLUE.b)
				c=C32BLACK;
			else
				logger("c=%d,%d,%d\n",c.r,c.g,c.b);
			cliprect32(B32,16+i*16,16+j*16,15,15,c);
//			cliprect32(B32,350+i*16,150+j*16,15,15,c);
		}
	video_unlock(); */
}

void tritest3exit()
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
