#include <m_eng.h>
#include "m_perf.h"
#include <l_misclibm.h>

#include "u_plotter2.h"

// do bresenham line and circle algorithms
using namespace u_plotter2;
//#define FLOAT_LINE
#define INT_LINE
//#define FLOAT_CIRCLE
//#define INT_CIRCLE

namespace bresenham {

	pointf2 pntItoF(const pointi2& pntI)
	{
		return pointf2x(float(pntI.x), float(pntI.y));
	}
#ifdef FLOAT_LINE
	pointi2 endPoint{ 33,-12 }; // startPoint is at the origin
	float slope; // from 0 to -1
	float err;

	// for debvars
	struct menuvar bresenhamdv[] = {
		{"@cyan@--- PLOTTER2 USER VARS for bresenham ---",NULL,D_VOID,0},
		{"slope",&slope,D_FLOAT | D_RDONLY},
		{"endPointX",&endPoint.x,D_INT},
		{"endPointY",&endPoint.y,D_INT}
	};
	const int nbresenhamdv = NUMELEMENTS(bresenhamdv);
#endif
#ifdef INT_LINE
	pointi2 endPoint{ 33,-12 }; // startPoint is at the origin
	S32 slopey;
	S32 slopex;
	S32 err;

	// for debvars
	struct menuvar bresenhamdv[] = {
		{"@cyan@--- PLOTTER2 USER VARS for bresenham ---",NULL,D_VOID,0},
		{"endPointX",&endPoint.x,D_INT},
		{"endPointY",&endPoint.y,D_INT},
		{"slopey",&slopey,D_INT | D_RDONLY},
		{"slopex",&slopex,D_INT | D_RDONLY},
	};
	const int nbresenhamdv = NUMELEMENTS(bresenhamdv);
#endif
} // end namespace bresenham

using namespace bresenham;

void plot2bresenhaminit()
{
	adddebvars("bresenham", bresenhamdv, nbresenhamdv);
	plotter2init();
	lzoom = -3.5f; // zoom way out
}

void plot2bresenhamproc()
{
	// interact with graph paper
	plotter2proc();
}

void plot2bresenhamdraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// calc and draw bresenham
#ifdef FLOAT_LINE
	endPoint.x = range(0, endPoint.x, 100);
	endPoint.y = range(-endPoint.x, endPoint.y, 0);
	if (endPoint.x != 0) // avoid a divide by zero
		slope = float(endPoint.y) / endPoint.x;
	else
		slope = 0;
	pointf2 pnt = pointf2x();
	err = 0;
	drawfpoint(pnt, C32RED, 4.0f);
	for (auto i = 0; i < endPoint.x; ++i) {
		++pnt.x;
		err += slope;
		// get a better error if we decrement y
		if (err < -.5f) {
			--pnt.y;
			++err;
		}
		drawfpoint(pnt, C32RED, 4.0f);
	}
#endif
#ifdef INT_LINE
	endPoint.x = range(0, endPoint.x, 100);
	endPoint.y = range(-endPoint.x, endPoint.y, 0);
	slopex = endPoint.x;
	slopey = endPoint.y;
	err = 0;
	pointi2 pnt = { 0,0 };
	// err will be multiple of slopex*2
	drawfpoint(pntItoF(pnt), C32RED, 4.0f);
	for (auto i = 0; i < endPoint.x; ++i) {
		++pnt.x;
		err += slopey*2;
		// get a better error if we decrement y
		if (err < -slopex) {
			--pnt.y;
			err += 2*slopex;
		}
		drawfpoint(pntItoF(pnt), C32RED, 4.0f);
	}

#endif
}

void plot2bresenhamexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("bresenham");
}
