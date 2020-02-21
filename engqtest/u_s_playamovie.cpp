#define D2_3D
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

namespace playamovie {
tree2 *tf; // for quat tests
int fpswantedsave;
char* movielist[]={
	"cartoon.ogv",
	"hw_grand_prix.ogv",
	"hw_volcano.ogv",
	"hw_island.ogv",
	"hw_arctic.ogv",
	"hw_jungle.ogv",
	"hw_streets.ogv",
	"hw_desert.ogv",
	"hw_sky.ogv",
	"hw_intro.ogv",
};
#define NMOVIELIST sizeof(movielist)/sizeof(movielist[0])
U32 curmovie;
bitmap32* curframe; // reference to movieframe
tree2* roottree;
S32 movx,movy;
static U32 togvidmode;
static bool deflitsave;
textureb* movietex;

} // end namespace playamovie

using namespace playamovie;

/*void dosubtxt()
{
	textureb* subtxt;
	subtxt=texture_create("movietex");
	if (texture_getrc(subtxt)) {
//		pushandsetdir("gfxtest");
//		subtxt->load();
		colorkeyinfo.lasthascolorkey=0;
		subtxt->build(pow2up(movx),pow2up(movy));
//		popdir();
	}
//	textureb* oldtex=*modtxt;
	textureb::rc.deleterc(oldtex);
//	*modtxt=textureb::rc.newrc(subtxt);
//	textureb::rc.deleterc(subtxt);
	*modtxt=subtxt;
}
*/

static tree2* buildsphere()
{
	modelb* mod2=model_create("sphere3");
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,35,35,1,1,spheref_surf(.25f),"maptestnck.tga","tex");
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree3");
	spheretree2->setmodel(mod2);
	return spheretree2;
}

#if 0
	pointf3 qa = {.1f,.2f,.3f};
	pointf3 qb = {.4f,.5f,-.55f};
#elif 0
	pointf3 qa = {0,0,.5f};
	pointf3 qb = {.5f,0,0};
#elif 1
	pointf3 qa = {0,0,1};
	pointf3 qb = {1,0,0};
#endif
pointf3 qi;
pointf3 qi2;
float twt;
int doqi2;
struct menuvar medv[]={
	{"@lightcyan@--- QUAT TEST ----",NULL,D_VOID,0},
	{"qa.x",&qa.x,D_FLOAT,FLOATUP/16},
	{"qa.y",&qa.y,D_FLOAT,FLOATUP/16},
	{"qa.z",&qa.z,D_FLOAT,FLOATUP/16},
	{"qa.w",&qa.w,D_FLOAT,FLOATUP/16},
	{"qb.x",&qb.x,D_FLOAT,FLOATUP/16},
	{"qb.y",&qb.y,D_FLOAT,FLOATUP/16},
	{"qb.z",&qb.z,D_FLOAT,FLOATUP/16},
	{"qb.w",&qb.w,D_FLOAT,FLOATUP/16},
	{"qi.x",&qi.x,D_FLOAT|D_RDONLY},
	{"qi.y",&qi.y,D_FLOAT|D_RDONLY},
	{"qi.z",&qi.z,D_FLOAT|D_RDONLY},
	{"qi.w",&qi.w,D_FLOAT|D_RDONLY},
	{"qi2.x",&qi2.x,D_FLOAT|D_RDONLY},
	{"qi2.y",&qi2.y,D_FLOAT|D_RDONLY},
	{"qi2.z",&qi2.z,D_FLOAT|D_RDONLY},
	{"qi2.w",&qi2.w,D_FLOAT|D_RDONLY},
	{"twt",&twt,D_FLOAT,FLOATUP/16},
	{"doqi2",&doqi2,D_INT},
};
const int nmedv=sizeof(medv)/sizeof(medv[0]);

void playamovieinit()
{
	deflitsave=lightinfo.dodefaultlights;
	lightinfo.dodefaultlights=1;
	video_setupwindow(GX,GY);
	int moviefps;
	fpswantedsave=wininfo.fpswanted;
	wininfo.fpswanted=30;

	logger("newlobby init\n");
	pushandsetdir("movies");
	moviefps=movieinit(movielist[curmovie],movx,movy);
	logger("movie size is %d %d\n",movx,movy);
	++curmovie;
	if (curmovie>=NMOVIELIST)
		curmovie=0;
	popdir();
	logger("moviefps %d\n",moviefps);
	wininfo.fpswanted = moviefps;

	roottree=new tree2("theroot");
	uv uv0,uv1;
	uv0.u=0;
	uv0.v=0;
	uv1.u=(float)movx/pow2up(movx);
	uv1.v=(float)(movy-4)/pow2up(movy);
	pushandsetdir("gfxtest");
	tree2* tt=buildprismuvs(pointf3x(9,4,1),"maptestnck.tga","pdiffspec",uv0,uv1);
	popdir();
//	tt=new tree2("built");
//	tt->setmodel(mod);
	tt->trans=pointf3x(0,-8,0);
//	tt->trans=pointf3x(5,-5,15);
	tt->rotvel.x=.005f/2;
	tt->rotvel.y=.05f/2;
	roottree->linkchild(tt);
/// build the spheretree model if this is the first one
	tf = buildsphere();
	tf->buildo2p=O2P_FROMTRANSQUATSCALE;	//tf->trans.x = -5;
	tf->trans = pointf3x(0,0,-6);
	roottree->linkchild(tf);
// setup main viewport
	memset(&mainvp,0,sizeof(mainvp));
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.125f;
	mainvp.zback=400;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
//	mainvp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
//	mainvp.useattachcam=true;
//	mainvp.lookat=0;
//	mainvp.camtrans.y=2.5;
	mainvp.camtrans=pointf3x(0,0,-8);
	mainvp.usev2w=false;
//	mainvp.camattach=0;

	mainvp.camrot.x=0;//1e-20f;
//	modtxt=&tt->mod->mats[0].thetexarr[0];
	movietex=texture_create("movietex");
	if (texture_getrc(movietex)) {
		colorkeyinfo.lasthascolorkey=0;
		movietex->build(pow2up(movx),pow2up(movy));
	}
//	dosubtxt();
	tt->mod->changetex(movietex);
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	extradebvars(medv,nmedv);
}

void playamovieproc()
{
	twt = range(0.0f,twt,1.0f);
	doqi2 = range(0,doqi2,1);
	quatnormalize(&qa,&qa);
	quatnormalize(&qb,&qb);
	quatinterp(&qa,&qb,twt,&qi);
	quatinterp2(&qa,&qb,twt,&qi2);
	if (doqi2)
		tf->rot = qi2;
	else
		tf->rot = qi;
	curframe=moviegetframe();
//	if (0) {
	if (!curframe || KEY==' ') {
		int moviefps;
		movieexit();
		pushandsetdir("movies");
		moviefps=movieinit(movielist[curmovie],movx,movy); // assume movies a all same size..
		popdir();
		++curmovie;
		if (curmovie>=NMOVIELIST)
			curmovie=0;
		wininfo.fpswanted=moviefps;
//		dosubtxt();
		curframe=moviegetframe();
	}
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 's':
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//			video_setupwindow(GX,GY);
		changestate(STATE_PLAYMOVIE);
		break;
	case '=':
		changeglobalxyres(1);
//			video_setupwindow(GX,GY);
		changestate(STATE_PLAYMOVIE);
		break;
	case '-':
		changeglobalxyres(-1);
//			video_setupwindow(GX,GY);
		changestate(STATE_PLAYMOVIE);
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_PLAYMOVIE);
	}
	if (curframe) {
		bitmap32* lt=movietex->locktexture();
		clipblit32(curframe,lt,0,0,0,0,curframe->size.x,curframe->size.y-4);
		movietex->unlocktexture();
	}
	roottree->proc();
	doflycam(&mainvp);
}

void playamoviedraw3d()
{
	video_buildworldmats(roottree); // help dolights
	dolights();
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
}

// not called anymore
void playamoviedraw2d()
{
	C32 col;
//	clipclear32(B32,C32(0,0,255));	
	outtextxyf32(B32,8,WY-16,C32LIGHTRED,"Movie Player");
	if (curframe)
		clipblit32(curframe,B32,0,0,8,8,curframe->size.x,curframe->size.y-4); // hack, bottom 4 pixels look hammered
}

void playamovieexit()
{
	extradebvars(0,0);
	wininfo.fpswanted=fpswantedsave;
	movieexit();
// log and free
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	textureb::rc.deleterc(movietex);
	lightinfo.dodefaultlights=deflitsave;
}
