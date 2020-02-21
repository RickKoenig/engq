// minimal code for 3d graphics

// engq API
#include <m_eng.h>

// list of user states
#include "u_states.h"

// for buildprism
#include "u_modelutil.h"

// 3d objects
static tree2* roottree;
static tree2* obj1;

////////////////////////// main
void basic3dinit()
{
// setup window size
	video_setupwindow(GX,GY);

// setup trees
	pushandsetdir("gfxtest"); // default graphics directory
	roottree = new tree2("roottree");
	//obj1=buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
	obj1 = buildplane_xy(pointf2x(1,1),"maptestnck.tga","tex","basicplane");
	roottree->trans.z = .5f; // move world forward to see objects
	roottree->linkchild(obj1);
	popdir();

// setup viewport
	mainvp.xres = WX;
	mainvp.yres = WY;
	mainvp.xstart = 0;
	mainvp.ystart = 0;
	mainvp.backcolor = C32DARKGRAY;
	mainvp.zfront = .125f;
	mainvp.zback = 20000;
	mainvp.camzoom = 1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z = 0;//-100;
	mainvp.camtrans.x = 0;
	mainvp.camtrans.y = 0;// 50;
	mainvp.camrot = pointf3x(0,0,0);
	mainvp.flags = VP_CLEARBG|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc = WX;
	mainvp.ysrc = WY;
	mainvp.useattachcam = false;
}

void basic3dproc()
{
// input
	if (KEY == K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
// proc
	roottree->proc();
	doflycam(&mainvp);
}

void basic3ddraw3d()
{
// draw
	video_buildworldmats(roottree); // help dolights
	dolights();
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
}

void basic3dexit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree; // free the main man
}
