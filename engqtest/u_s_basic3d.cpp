// minimal code for 3d graphics

// engq API
#include <m_eng.h>

// list of user states
#include "u_states.h"

// for buildprism, or other 3d primitive objects
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
	roottree = new tree2("roottree"); // main tree
	//obj1=buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
	obj1 = buildplane_xy(pointf2x(1,1),"maptestnck.tga","tex","basicplane");
	roottree->trans.z = .5f; // move world forward to see objects
	roottree->linkchild(obj1);
	popdir();

// setup viewport
	mainvp = viewport2x(); // a good default viewport
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
