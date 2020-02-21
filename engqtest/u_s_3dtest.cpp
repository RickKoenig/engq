/// 3d test of zbuffer many overlapping spinning cubes, 1 flat, 4 textured, and 1 cvert no tex
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

static U32 togvidmode;
static S32 fpswantedsave,uselightssave;
static tree2* roottree;

/*static struct pointf3 flatcol[]={
	{0,0,0,{1}},
	{.999f,.999f,.999f,{1}},
	{.999f,0,0,{1}},
	{0,0,0,{1}},
	{.999f,.999f,.999f,{1}},
	{.999f,0,0,{1}}, 
};
*/
// newer one with textures
static struct pointf3 texverts[]={
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1,-1},
	{ 1,-1,-1},
	{ 1, 1, 1},
	{-1, 1, 1},
	{ 1,-1, 1},
	{-1,-1, 1},

	{ 1, 1,-1},
	{ 1, 1, 1},
	{ 1,-1,-1},
	{ 1,-1, 1},
	{-1, 1, 1},
	{-1, 1,-1},
	{-1,-1, 1},
	{-1,-1,-1},

	{-1, 1, 1},
	{ 1, 1, 1},
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1,-1},
	{ 1,-1,-1},
	{-1,-1, 1},
	{ 1,-1, 1},
};

static struct uv texuvs[]={
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
};

static struct pointf3 texcverts[]={
	{.99f,.01f,.01f,{1.0f}},
	{.01f,.99f,.01f,{1.0f}},
	{.01f,.01f,.99f,{1.0f}},
	{.99f,.99f,.99f,{1.0f}},
	{.99f,.01f,.01f,{1.0f}},
	{.01f,.99f,.01f,{1.0f}},
	{.01f,.01f,.99f,{1.0f}},
	{.99f,.99f,.99f,{1.0f}},
	{.99f,.01f,.01f,{1.0f}},
	{.01f,.99f,.01f,{1.0f}},
	{.01f,.01f,.99f,{1.0f}},
	{.99f,.99f,.99f,{1.0f}},
	{.99f,.01f,.01f,{1.0f}},
	{.01f,.99f,.01f,{1.0f}},
	{.01f,.01f,.99f,{1.0f}},
	{.99f,.99f,.99f,{1.0f}},
	{.99f,.01f,.01f,{1.0f}},
	{.01f,.99f,.01f,{1.0f}},
	{.01f,.01f,.99f,{1.0f}},
	{.99f,.99f,.99f,{1.0f}},
	{.99f,.01f,.01f,{1.0f}},
	{.01f,.99f,.01f,{1.0f}},
	{.01f,.01f,.99f,{1.0f}},
	{.99f,.99f,.99f,{1.0f}},
};

static struct face texfaces[]={
	{{0,1,2},0},
	{{3,2,1},0},
};

static C8* texnames[]={
	"--",
	"maptestnck.tga",
	"pencil.jpg",
	"bark.tga",
	"rengst.jpg",
	"--"
};

static C8* matnames[]={
	"cvert",
	"tex",
	"tex",
	"tex",
	"tex",
	"tex",
};

////////////////////////// main
void d3testinit()
{
	S32 i,j,k;
	video_setupwindow(GX,GY);
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=0;
	pushandsetdir("gfxtest");
	roottree=new tree2("roottree");
	tree2* onefacemodeltree=new tree2("built");
	modelb* mod;
//	mod=model2::rc.newrc("amod");
//	if (model2::rc.getrc(mod)==1) {
	mod=model_create("amod");
	if (model_getrc(mod)==1) {
		mod->copyverts(texverts,24);
		mod->copyuvs0(texuvs,24);
		mod->copycverts(texcverts,24);
		for (i=0;i<6;++i) {
//			C8 str[30];
//			sprintf(str,"texmat%d",i);
//			strcpy(str,"tex");
			if (i==0) {
				mod->addmat(matnames[i],SMAT_HASWBUFF|SMAT_HASSHADE,0,0,30,2,4);
			} else if (i==5) {
				mod->addmat(matnames[i],SMAT_HASWBUFF,0,&F32LIGHTRED,30,2,4);
			} else {
//				texture2* texmat0=texture2::rc.newrc(texnames[i]);
//				if (texture2::rc.getrc(texmat0)==1) {
				textureb* texmat0=texture_create(texnames[i]);
				if (texture_getrc(texmat0)==1) {
					colorkeyinfo.usecolorkey=0;
					texmat0->load();
					colorkeyinfo.usecolorkey=1;
				}
				mod->addmat(matnames[i],SMAT_HASWBUFF|SMAT_HASTEX,texmat0,0,30,2,4);
			}
			mod->addfaces(texfaces,2,4*i);
		}
		mod->close();
	}
	onefacemodeltree->setmodel(mod);
	popdir();
	for (i=-5;i<=0;i+=5) {
		for (j=-5;j<=0;j+=5) {
			for (k=-5;k<=0;k+=5) {
				tree2* t=onefacemodeltree->newdup();
				t->trans.x=0;//i;
				t->trans.y=0;//j;
				t->trans.z=0;//k;
				t->rotvel.x=.0115f*i/50+.0125f*j/50+.0135f*k/50;
				t->rotvel.y=.0145f*i/50+.0155f*j/50+.0165f*k/50;
				t->rotvel.z=.0175f*i/50+.0185f*j/50+.0195f*k/50;
				roottree->linkchild(t);
			}
		}
	}
	delete onefacemodeltree;
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=100;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z=-3;
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
}

void d3testproc()
{
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_3DTEST);
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
		changestate(STATE_3DTEST);
		break;
	case '=':
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_3DTEST);
		break;
	case '-':
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_3DTEST);
		break;
	}	
	mainvp.xres=WX;
	mainvp.yres=WY;
	roottree->proc();
	doflycam(&mainvp);
//	buildtreematrices(roottree); //roottree,camtree);
//	video_beginscene(); // clear zbuf etc.
}

void d3testdraw3d()
{
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	video_drawscene(roottree);
}
//	video_endscene(0); // nothing right now
/*	video_lock();
	static S32 x;
	S32 i;
	for (i=0;i<10;++i)
		clipcircle32(B32,x,WY/4+20*i,i,C32CYAN);
	++x;
	if (x>=WX)
		x=0;
	video_unlock(); */


void d3testexit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	wininfo.fpswanted=fpswantedsave;
	lightinfo.uselights=uselightssave;
}
