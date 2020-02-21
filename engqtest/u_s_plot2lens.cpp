// first test of plotter2 
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_plotter2.h"

using namespace u_plotter2;

namespace lensp2 {

float focus = 3;
float objx = -8;//-6;
float objy = 2;
float lensheight;
float imgx;
float imgy;

// for debvars
struct menuvar lensdv[]={
	{"@lightred@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
	{"focus",&focus,D_FLOAT,FLOATUP/16},
	{"objx",&objx,D_FLOAT,FLOATUP/16},
	{"objy",&objy,D_FLOAT,FLOATUP/16},
	{"imgx",&imgx,D_FLOAT|D_RDONLY},
	{"imgy",&imgy,D_FLOAT|D_RDONLY},
};
const int nlensdv = NUMELEMENTS(lensdv);

} // end namespace lensp2

using namespace lensp2;

void plot2lensinit()
{
	adddebvars("lens",lensdv,nlensdv);
	plotter2init();
}

void plot2lensproc()
{
	// interact with graph paper
	plotter2proc();
	// calc the image
	imgx = 1.0f/(1.0f/objx + 1.0f/focus);
	imgy = imgx*objy/objx;
}

void plot2lensdraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw the lens
	lensheight = max(abs(objy),abs(imgy))*1.2f;
	pointf2x a(-.05f*lensheight,0);
	pointf2x b(0,lensheight);
	pointf2x c(.05f*lensheight,0);
	pointf2x d(0,-lensheight);
	drawfline(a,b,C32GREEN);
	drawfline(b,c,C32GREEN);
	drawfline(c,d,C32GREEN);
	drawfline(d,a,C32GREEN);
	// draw the foci
	drawfpoint(pointf2x(focus,0),C32RED);
	drawfpoint(pointf2x(-focus,0),C32RED);
	// draw the object
	pointf2x e(objx-objy*.1f,0);
	pointf2x f(objx,objy);
	pointf2x g(objx+objy*.1f,0);
	drawfline(e,f,C32DARKGRAY);
	drawfline(f,g,C32DARKGRAY);
	drawfline(g,e,C32DARKGRAY);
	pointf2x h(imgx-imgy*.1f,0);
	pointf2x i(imgx,imgy);
	pointf2x j(imgx+imgy*.1f,0);
	drawfline(h,i,C32BLUE);
	drawfline(i,j,C32BLUE);
	drawfline(j,h,C32BLUE);
	// draw the image
	if (objx < -focus) { // real
		drawfline(pointf2x(objx,objy),pointf2x(imgx,imgy),C32BLACK);
		drawfline(pointf2x(objx,objy),pointf2x(0,objy),C32BLACK);
		drawfline(pointf2x(imgx,imgy),pointf2x(0,imgy),C32BLACK);
		drawfline(pointf2x(imgx,imgy),pointf2x(0,objy),C32BLACK);
		drawfline(pointf2x(objx,objy),pointf2x(0,imgy),C32BLACK);
	} else { // virtual
		drawfline(pointf2x(imgx,imgy),pointf2x(-imgx,imgy),C32BLACK);
		drawfline(pointf2x(imgx,imgy),pointf2x(focus,0),C32BLACK);
		drawfline(pointf2x(imgx,imgy),pointf2x(-imgx,-imgy),C32BLACK);
		drawfline(pointf2x(objx,objy),pointf2x(0,objy),C32BLACK);
		drawfline(pointf2x(-focus,0),pointf2x(0,imgy),C32BLACK);
	}
}

void plot2lensexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("lens");
}
