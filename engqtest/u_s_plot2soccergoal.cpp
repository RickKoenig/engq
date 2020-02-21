#include <m_eng.h>
#include "m_perf.h"
#include <l_misclibm.h>

#include "u_plotter2.h"

// figure out the best angle for a goal shot in soccer (biggest angle)
using namespace u_plotter2;

namespace soccergoal {

// implied line segment from 0 to 1
	// x: over to the right some more
	// y: move up and down
	pointf2 p{ 3.0f, .5f };
	float ang0, ang1, angd, sumdang, calch;

// for debvars
struct menuvar soccergoaldv[] = {
	{"@lightgreen@--- PLOTTER2 USER VARS for soccergoal ---",NULL,D_VOID,0},
	{"w",&p.x,D_FLOAT,FLOATUP / 16},
	{"h",&p.y,D_FLOAT,FLOATUP / 16},
	{"ang0",&ang0,D_FLOAT | D_RDONLY},
	{"ang1",&ang1,D_FLOAT | D_RDONLY},
	{"angd",&angd,D_FLOAT | D_RDONLY},
	{"sumdang",&sumdang,D_FLOAT | D_RDONLY},
	{"calch",&calch,D_FLOAT | D_RDONLY},
};
const int nsoccergoaldv = NUMELEMENTS(soccergoaldv);

} // end namespace soccergoal

using namespace soccergoal;

void plot2soccergoalinit()
{
	adddebvars("soccergoal", soccergoaldv, nsoccergoaldv);
	plotter2init();
	lzoom = -1.5f;
}

void plot2soccergoalproc()
{
	// interact with graph paper
	plotter2proc();
	// calc soccer
	ang0 = atan2(p.y, p.x);// *PIUNDER180;
	ang1 = atan2(p.y, p.x - 1);// *PIUNDER180;
	angd = ang1 - ang0;
	//sumdang = (1 / p.x) * (1 / (1 + p.y*p.y / (p.x*p.x)));
	//sumdang -= (1 / (p.x - 1)) * (1 / (1 + p.y*p.y / ((p.x-1)*(p.x-1))));
	sumdang = p.x / (p.x*p.x + p.y*p.y) - (p.x - 1) / ((p.x - 1)*(p.x - 1) + p.y*p.y);
	// formula to calc best angle for height
	calch = sqrt(p.x*p.x - p.x);
}

void plot2soccergoaldraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw soccer
	drawfline(pointf2x(0, 0), pointf2x(1, 0), C32BLACK);
	drawfpoint(p, C32RED, 4.0f);
	drawfpoint(pointf2x(p.x,calch), C32MAGENTA, 4.0f);
	drawfline(pointf2x(0, 0), p, C32BLUE);
	drawfline(pointf2x(1, 0), p, C32BLUE);
}

void plot2soccergoalexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("soccergoal");
}
