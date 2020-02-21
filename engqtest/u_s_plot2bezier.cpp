// first test of plotter2
#include <float.h>

#include <m_eng.h>

#include "u_plotter2.h"

using namespace u_plotter2;

#define DONAMESPACE
#ifdef DONAMESPACE
namespace bezier {
#endif

const float P0 = 5/16.0f;
const float P1 = 8/16.0f;
const float C0 = 1/16.0f;
const float C1 = 6/16.0f;

pointf3 BEZPARMS = pointf3x(P0,P1,C0,C1);
pointf3 COEFFS;
pointf3 HALFCOEFFS;
pointf3 HALFBEZPARMS;
mat4 BEZPARMS2COEFFS;
mat4 COEFFS2BEZPARMS;
float det;
S32 c2p;
S32 halfway = 1;

//const float A,B,C,D; // coefficients for cubic polynomial function for bezier

// for debvars
struct menuvar bezierdv[]={
	{"@lightred@--- bezier ---",NULL,D_VOID,0},
	{"P0",&BEZPARMS.x,D_FLOAT,FLOATUP/32},
	{"P1",&BEZPARMS.y,D_FLOAT,FLOATUP/32},
	{"C0",&BEZPARMS.z,D_FLOAT,FLOATUP/32},
	{"C1",&BEZPARMS.w,D_FLOAT,FLOATUP/32},
	{"A",&COEFFS.x,D_FLOAT,FLOATUP/32},
	{"B",&COEFFS.y,D_FLOAT,FLOATUP/32},
	{"C",&COEFFS.z,D_FLOAT,FLOATUP/32},
	{"D",&COEFFS.w,D_FLOAT,FLOATUP/32},
	{"det",&det,D_FLOAT|D_RDONLY},
	{"coeffs to points",&c2p,D_INT,1},
	{"do halfway",&halfway,D_INT,1},

};
const int nbezierdv = NUMELEMENTS(bezierdv);

#define METHOD2

float cubic(float t)
{
	//return A*t*t*t + B*t*t + C*t + D;
	//return t*(t*A*t + B*t) + C) + D;
	return t*(t*(t*COEFFS.x + COEFFS.y) + COEFFS.z) + COEFFS.w;
}

float cubichalf(float t)
{
	//return A*t*t*t + B*t*t + C*t + D;
	//return t*(t*A*t + B*t) + C) + D;
	return t*(t*(t*HALFCOEFFS.x + HALFCOEFFS.y) + HALFCOEFFS.z) + HALFCOEFFS.w;
}

float funtest1(float t)
{
	return t*(1-t)*4;
}

void calcbezier()
{
	c2p &= 1;
	halfway &= 1;
	if (c2p)
		mulmatvec4(&COEFFS2BEZPARMS,&COEFFS,&BEZPARMS);
	else {
#define DOMAT
#ifdef DOMAT
		mulmatvec4(&BEZPARMS2COEFFS,&BEZPARMS,&COEFFS);
#else
#ifdef METHOD2
		COEFFS.x = -BEZPARMS.x + BEZPARMS.y + 3*BEZPARMS.z - 3*BEZPARMS.w; // -P0 + P1 + 3*C0 - 3*C1;		// A
		COEFFS.y = 3*BEZPARMS.x - 6*BEZPARMS.z + 3*BEZPARMS.w; // 2*P1 - 6*C0 + 3*P0 + 3*C1;	// B
		COEFFS.z = -3*BEZPARMS.x + 3*BEZPARMS.z; // 3*C0 - 3*P0;					// C
		COEFFS.w = BEZPARMS.x; // P0;						// D
#else
		COEFFS.x = BEZPARMS.x - BEZPARMS.y + BEZPARMS.z - BEZPARMS.w; // P0 - P1 + C0 - C1;		// A
		COEFFS.y = 2*BEZPARMS.y - 2*BEZPARMS.z - BEZPARMS.x + BEZPARMS.w; // 2*P1 - 2*C0 - P0 + C1;	// B
		COEFFS.z = BEZPARMS.z - BEZPARMS.x; // C0 - P0;					// C
		COEFFS.w = BEZPARMS.x; // P0;						// D
#endif
#endif
	}
	if (halfway) {
		HALFCOEFFS.x = COEFFS.x/8;
		HALFCOEFFS.y = COEFFS.y/4;
		HALFCOEFFS.z = COEFFS.z/2;
		HALFCOEFFS.w = COEFFS.w;
	}

}

void drawbezier()
{
	pointf2 P0XY = pointf2x(0,BEZPARMS.x);
	pointf2 P1XY = pointf2x(1,BEZPARMS.y);
	pointf2 C0XY = pointf2x(1,BEZPARMS.z);
	pointf2 C1XY = pointf2x(0,BEZPARMS.w);

	drawfline(P0XY,C0XY,C32BLUE);
	drawfline(P1XY,C1XY,C32BLUE);

	drawfpoint(P0XY,C32RED);
	drawfpoint(P1XY,C32RED);
	drawfpoint(C0XY,C32GREEN);
	drawfpoint(C1XY,C32GREEN);

	//drawfunctionrange(funtest1,0,1);
	drawfunctionrange(cubic,0,1);
	if (halfway) {
		drawfunctionrange(cubichalf,0,1,C32RED);
	}
}

#ifdef DONAMESPACE
} // end namespace bezier
using namespace bezier;
#endif

void plot2bezierinit()
{
	adddebvars("bezier",bezierdv,nbezierdv);
	plotter2init();
	//lzoom = -2.5f;
	//initgrid(ingrid);
#ifdef METHOD2
	BEZPARMS2COEFFS.e[0][0] = -1;
	BEZPARMS2COEFFS.e[0][1] = 3;
	BEZPARMS2COEFFS.e[0][2] = -3;
	BEZPARMS2COEFFS.e[0][3] = 1;

	BEZPARMS2COEFFS.e[1][0] = 1;
	BEZPARMS2COEFFS.e[1][1] = 0;
	BEZPARMS2COEFFS.e[1][2] = 0;
	BEZPARMS2COEFFS.e[1][3] = 0;

	BEZPARMS2COEFFS.e[2][0] = 3;
	BEZPARMS2COEFFS.e[2][1] = -6;
	BEZPARMS2COEFFS.e[2][2] = 3;
	BEZPARMS2COEFFS.e[2][3] = 0;

	BEZPARMS2COEFFS.e[3][0] = -3;
	BEZPARMS2COEFFS.e[3][1] = 3;
	BEZPARMS2COEFFS.e[3][2] = 0;
	BEZPARMS2COEFFS.e[3][3] = 0;
#else
	BEZPARMS2COEFFS.e[0][0] = 1;
	BEZPARMS2COEFFS.e[0][1] = -1;
	BEZPARMS2COEFFS.e[0][2] = -1;
	BEZPARMS2COEFFS.e[0][3] = 1;

	BEZPARMS2COEFFS.e[1][0] = -1;
	BEZPARMS2COEFFS.e[1][1] = 2;
	BEZPARMS2COEFFS.e[1][2] = 0;
	BEZPARMS2COEFFS.e[1][3] = 0;

	BEZPARMS2COEFFS.e[2][0] = 1;
	BEZPARMS2COEFFS.e[2][1] = -2;
	BEZPARMS2COEFFS.e[2][2] = 1;
	BEZPARMS2COEFFS.e[2][3] = 0;

	BEZPARMS2COEFFS.e[3][0] = -1;
	BEZPARMS2COEFFS.e[3][1] = 1;
	BEZPARMS2COEFFS.e[3][2] = 0;
	BEZPARMS2COEFFS.e[3][3] = 0;
#endif

	det = detmat4(&BEZPARMS2COEFFS);
	inversemat4(&BEZPARMS2COEFFS,&COEFFS2BEZPARMS);
}

void plot2bezierproc()
{
	// interact with graph paper
	plotter2proc();
	// calc the image
	calcbezier();
}

void plot2bezierdraw2d()
{
	// draw graph paper
	plotter2draw2d();
	drawbezier();
}

void plot2bezierexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("bezier");
}
