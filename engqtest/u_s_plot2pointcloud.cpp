#include <float.h> // for _isnan

// first test of plotter2 
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_plotter2.h"

#include "u_pointcloudkdtree.h"

using namespace u_plotter2;

namespace plot2pointcloudp2 {



// closest point
//#define CP
//#define CPI

// points less than a given radius
#define LTRAD
//#define LTRADI

//#define SHOWSPLIT // test first median split amongst the points

#ifdef SHOWSPLIT
vector<pointf2> dleft,dmiddle,dright; // from level 0, for display debugging
U32 dleftsize,dmiddlesize,drightsize; // from level 0, display sizes in debprint menu
#endif

#ifdef CP
pointf2 closestpt;
pointf2 closestptb;
#endif

#ifdef CPI
U32 closestpti;
U32 closestptbi;
#endif

#ifdef LTRAD
vector<pointf2> insp;
vector<pointf2> inspb;
#endif

#ifdef LTRADI
vector<U32> inspi;
vector<U32> inspbi;
#endif


float radu = 2.5f;
float radk,radb;

const int debnpnts = 10;

#define TEST1
//#define TEST2
//#define TEST3

#ifdef TEST1
pointf2 testpt = {3.5f,4.5f};
const int npnts = 10;
pointf2 pnts[debnpnts] = {
	{3,6},
	{6,7},
	{7,5},
	{7,1},
	{3,1},
	{1,2},
	{5,2},
	{2,4},
	{6,4},
	{4,5},
};
#endif
#ifdef TEST2
pointf2 testpt = {1.25f,1.25f};
const int npnts = 3;
pointf2 pnts[debnpnts] = {
	{3,5},
	{2,3},
	{1,1},
};
#endif
#ifdef TEST3
pointf2 testpt = {3.5f,4.5f};
const int npnts = 10;
pointf2 pnts[debnpnts] = {
	{1,3},
	{2,3},
	{1,3},
	{1,3},
	{1,3},
	{1,3},
	{1,3},
	{2,3},
	{1,3},
	{1,3},
};
#endif
//const int npnts = NUMELEMENTS(pnts);


// for debvars
struct menuvar pointcloudedv[]={
	{"@lightred@--- PLOTTER2 USER VARS POINT CLOUD TEST ---",NULL,D_VOID,0},
	{"testpt.x",&testpt.x,D_FLOAT,FLOATUP/4},
	{"testpt.y",&testpt.y,D_FLOAT,FLOATUP/4},
//#ifdef LTRAD
//	{"radlt",&rad,D_FLOAT,FLOATUP/4},
//#endif
//#ifdef LTRADI
	{"radu",&radu,D_FLOAT,FLOATUP/4},
//#endif
//#ifdef CP
	{"radk",&radk,D_FLOAT|D_RDONLY},
//#endif
//#ifdef CPI
	{"radb",&radb,D_FLOAT|D_RDONLY},
//#endif
#ifdef SHOWSPLIT
	{"dleftsize",&dleftsize,D_INT|D_RDONLY},
	{"dmiddlesize",&dmiddlesize,D_INT|D_RDONLY},
	{"drightsize",&drightsize,D_INT|D_RDONLY},
#endif
	{"p[0].x",&pnts[0].x,D_FLOAT,FLOATUP/4},
	{"p[0].y",&pnts[0].y,D_FLOAT,FLOATUP/4},
	{"p[1].x",&pnts[1].x,D_FLOAT,FLOATUP/4},
	{"p[1].y",&pnts[1].y,D_FLOAT,FLOATUP/4},
	{"p[2].x",&pnts[2].x,D_FLOAT,FLOATUP/4},
	{"p[2].y",&pnts[2].y,D_FLOAT,FLOATUP/4},
	{"p[3].x",&pnts[3].x,D_FLOAT,FLOATUP/4},
	{"p[3].y",&pnts[3].y,D_FLOAT,FLOATUP/4},
	{"p[4].x",&pnts[4].x,D_FLOAT,FLOATUP/4},
	{"p[4].y",&pnts[4].y,D_FLOAT,FLOATUP/4},
	{"p[5].x",&pnts[5].x,D_FLOAT,FLOATUP/4},
	{"p[5].y",&pnts[5].y,D_FLOAT,FLOATUP/4},
	{"p[6].x",&pnts[6].x,D_FLOAT,FLOATUP/4},
	{"p[6].y",&pnts[6].y,D_FLOAT,FLOATUP/4},
	{"p[7].x",&pnts[7].x,D_FLOAT,FLOATUP/4},
	{"p[7].y",&pnts[7].y,D_FLOAT,FLOATUP/4},
	{"p[8].x",&pnts[8].x,D_FLOAT,FLOATUP/4},
	{"p[8].y",&pnts[8].y,D_FLOAT,FLOATUP/4},
	{"p[9].x",&pnts[9].x,D_FLOAT,FLOATUP/4},
	{"p[9].y",&pnts[9].y,D_FLOAT,FLOATUP/4},
};
const int npointcloudedv = NUMELEMENTS(pointcloudedv);

} // end namespace plot2pointcloudp2

using namespace plot2pointcloudp2;

void plot2pointcloudinit()
{
	adddebvars("plot2pointcloud",pointcloudedv,npointcloudedv);
	plotter2init();
	//insp = new vector<pointf2>;
}

void plot2pointcloudproc()
{
	// interact with graph paper
	plotter2proc();

	// do kdtree tests
	pc_kdtree2d kdt(pnts,npnts);

#ifdef LTRAD
	// anything less than rad
	inspb = kdt.insidecirclebrute(testpt,radu);
	insp = kdt.insidecircle(testpt,radu);
#endif

#ifdef CP
	// closest point
	closestptb = kdt.closestbrute(testpt,radb);
	closestpt = kdt.closest(testpt,radk);
#endif

#ifdef LTRADI
	// anything less than rad index
	inspbi = kdt.insidecirclebrutei(&testpt,rad);
	inspi = kdt.insidecirclei(&testpt,rad);
#endif

#ifdef CPI
	// closest point index
	closestptbi = kdt.closestbrutei(&testpt,&rad);
	closestpti = kdt.closesti(&testpt,&rad);
#endif

#ifdef SHOWSPLIT
	kdt.getsplitdata(dleft,dmiddle,dright);
	dleftsize = dleft.size();
	dmiddlesize = dmiddle.size();
	drightsize = dright.size();
#endif
}

void plot2pointclouddraw2d()
{
	// draw graph paper
	plotter2draw2d();

	// draw test point
	//drawfpoint(testpt,C32RED);
#if defined LTRAD || defined LTRADI
	drawfcirclef(testpt,C32GREEN,radu);
#endif


	// draw the points
	U32 i;
	for (i=0;i<npnts;++i) {
		drawfpoint(pnts[i],C32BLACK);
	}
#ifdef SHOWSPLIT
	// use different colors for 3 splits
	U32 n = dleft.size();
	for (i=0;i<n;++i) {
		drawfpoint(dleft[i],C32LIGHTRED);
	}
	n = dmiddle.size();
	for (i=0;i<n;++i) {
		drawfpoint(dmiddle[i],C32LIGHTGREEN);
	}
	n = dright.size();
	for (i=0;i<n;++i) {
		drawfpoint(dright[i],C32LIGHTBLUE);
	}
#endif

#ifdef LTRAD
	U32 npnts = inspb.size();
	// draw the points inside the circle
	for (i=0;i<npnts;++i) {
		drawfpoint(inspb[i],C32GREEN,6);
	}
	npnts = insp.size();
	for (i=0;i<npnts;++i) {
		drawfpoint(insp[i],C32RED);
	}
#endif

#ifdef CP
		float mts = screen2math(1.5f);
		drawfcirclef(testpt,C32GREEN,radb + mts);
		drawfcirclef(testpt,C32GREEN,radb);
		drawfcirclef(testpt,C32GREEN,radb - mts);
		drawfcirclef(testpt,C32MAGENTA,radk);
		drawfpoint(closestptb,C32GREEN,6);
		drawfpoint(closestpt,C32MAGENTA);
#endif

#ifdef LTRADI
	U32 npntsi = inspi.size();
	// draw the points inside the circle
	//const pointf2* dc = inspi.getd
	for (i=0;i<npntsi;++i) {
		drawfpoint(pnts[inspi[i]],C32RED);
	}
#endif

#ifdef CPI
	const pointf2& cpb = pnts[closestptbi];
	drawfpoint(cpb,C32GREEN,6);
	const pointf2& cp = pnts[closestpti];
	drawfpoint(cp,C32MAGENTA);

#endif
	drawfpoint(testpt,C32RED);

}

void plot2pointcloudexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("plot2pointcloud");
}
