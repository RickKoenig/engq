/// test bbox
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

#include <d3d9.h>
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "u_modelutil.h"

static U32 togvidmode;
static tree2* roottree;

static tree2* buildbackgroundmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("sbm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
//		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
	    buildpatch(modb,1,1,1,1,planexz_surf(1.0f,1.0f),"maptestnck.tga",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
//		popdir();
	}
	r->setmodel(modb);
	r->trans.z=1.5f;
	r->rot.x=-PI/2;
	return r;
}

////////////////////////// main
void tritest5init()
{
	video_setupwindow(GX,GY);
	roottree=new tree2("roottree");
	pushandsetdir("gfxtest");
// build a cube
	tree2* onefacemodeltree=buildbackgroundmodel("bm","tex");
//	tree2* onefacemodeltree=buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
//	onefacemodeltree->trans=pointf3x(0,0,1.5f);
//	onefacemodeltree->rotvel.x=.005f/2;
//	onefacemodeltree->rotvel.y=.05f/2;
	roottree->linkchild(onefacemodeltree);
// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=200;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;

	mainvp.zfront=.375f;
	mainvp.zback=200;
	mainvp.camtrans.x=-.61826f;
	mainvp.camtrans.y=.590f;
	mainvp.camtrans.z=1.1925f-1;
	mainvp.camrot.x=.693f;
	mainvp.camrot.y=.687f;
}

void tritest5proc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_TRITEST5);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_TRITEST5);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
		changestate(STATE_TRITEST5);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		changestate(STATE_TRITEST5);
	}
	mainvp.xres=WX/2;
	mainvp.yres=WY/2;
	mainvp.xstart=WX/4;
	mainvp.ystart=WY/4;
	roottree->proc();
	doflycam(&mainvp);
}

void tritest5draw3d()
{
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
}

void tritest5exit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	popdir();
}
