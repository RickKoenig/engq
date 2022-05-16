// first test of plotter2
#include <float.h>

#include <m_eng.h>

#include "u_plotter2.h"

using namespace u_plotter2;

#define DONAMESPACE
#ifdef DONAMESPACE
namespace fourducks {
#endif

#define DUCKS
//#define TRIANGLEPUZZLE

#ifdef DUCKS
#define NUMDUCKS 4
#define MAXDUCKS 40
float cducks[MAXDUCKS]; // from 0 to 359 for circle
float gducks[MAXDUCKS]; // from 0 to 359 for grid
//float testmod;
#endif
#ifdef TRIANGLEPUZZLE
float angA = 40; // in degrees
pointf2 p3I;
float should3;
float isY;
#endif
// for debvars
struct menuvar ducksdv[]={
	{"@lightred@--- four duck pond brilliant tests ---",NULL,D_VOID,0},
#ifdef DUCKS
	{"cir D0",&cducks[0],D_FLOAT,FLOATUP},
	{"cir D1",&cducks[1],D_FLOAT,FLOATUP},
	{"cir D2",&cducks[2],D_FLOAT,FLOATUP},
	{"cir D3",&cducks[3],D_FLOAT,FLOATUP},
	{"grid D3",&gducks[3],D_FLOAT,FLOATUP},
	//{"testmod",&testmod,D_FLOAT|D_RDONLY},
#endif
#ifdef TRIANGLEPUZZLE
	{"angA",&angA,D_FLOAT,FLOATUP},
	{"intersection X",&p3I.x,D_FLOAT|D_RDONLY},
	{"intersection Y",&p3I.y,D_FLOAT|D_RDONLY},
	{"dist p3I to p2, should be 3",&should3,D_FLOAT|D_RDONLY},
	{"dist p3I to p1, isY",&isY,D_FLOAT|D_RDONLY},
#endif
};
const int nducksdv = NUMELEMENTS(ducksdv);

#ifdef DUCKS
// all in degrees

bool ducksOnOneSide(const float* ducks,U32 numducks)
{
	if (numducks <= 2)
		return true;
	float sducks[MAXDUCKS];
	copy(ducks,ducks+numducks,sducks);
	U32 i;
	for (i=0;i<numducks;++i) {
		sducks[i] = fmodf(sducks[i],360);
		if (sducks[i] < 0)
			sducks[i] += 360;
		if (sducks[i] == 0)
			sducks[i] = 0; // get rid of -0 in floating point for display
	}
	//testmod = sducks[numducks-1];
	sort(sducks,sducks+numducks);
	for (i=0;i<numducks-1;++i) {
		if (sducks[i] + 180 <= sducks[i+1]) {
			return true; // hey there's a gap of >= 180 degrees, must all be on one side
		}
	}
	// wrap around
	if (sducks[numducks-1] <= sducks[0] + 180) { // add full circle to sducks[0] and simplify
		return true; // hey there's a gap of >= 180 degrees, must all be on one side
	}
	return false;
}

void runducks(U32 ndcks)
{
	float dcks[MAXDUCKS];
	U32 iternum = 4;
	U32 iters[MAXDUCKS];
	fill(iters,iters+ndcks,0);
	U32 dig = 0;
	U32 tcnt = 0;
	U32 pcnt = 0;
	//U32 ncnt = 0;
	// multi dimensional for loops
	while(dig != ndcks) {
		// convert to ducks
//#define VERBOSE
#ifdef VERBOSE
		logger("iter\n");
#endif
		//float test[3] = {0,170,190};
		U32 i;
		for (i=1;i<ndcks;++i) {
			//dcks[i] = /*test[i];//*/iters[i]*(360.0f/(iternum-1));
			dcks[i] = (iters[i]+.5f)*(360.0f/iternum) + .001f*i;
#ifdef VERBOSE
			logger("\tit[%u] = %f\n",i,dcks[i]);
#endif
		}

		// do this combo
		if (ducksOnOneSide(dcks,ndcks))
			++pcnt;
		//else
		//	++ncnt;
		++tcnt;

		// move to next
		dig = 1; // duck 0 is fixed
		while (dig != ndcks) {
			++iters[dig];
			if (iters[dig] != iternum) {

				break;
			}
			iters[dig] = 0;
			++dig;
			if (dig == ndcks)
				break;
		}

	}
	U32 div = gcd(pcnt,tcnt);
	U32 rpcnt = pcnt/div;
	U32 rtcnt = tcnt/div;
	logger("\tcount = %u/%u = %u/%u = %f\n",pcnt,tcnt,rpcnt,rtcnt,float(pcnt)/tcnt);
}

// do more ducks
void calcducks()
{
	logger("calc ducks ------------------->>>>>>>>>>>>>>>>\n");
	U32 nd;
	for (nd=1;nd<=9;++nd) {
		logger("\tnum ducks = %u\n",nd);
		runducks(nd);
	}
	logger("end calc ducks ------------------->>>>>>>>>>>>>>>>\n");
}

void drawducksgrid()
{
	const S32 iter = 72;
	const float step = 360.0f/iter;
	for (gducks[1] = 0;gducks[1] <= 360;gducks[1] += step) {
		for (gducks[2] = 0;gducks[2] <= 360;gducks[2] += step) {
			const bool res = ducksOnOneSide(gducks,NUMDUCKS);
			if (res) {
				pointf2 c = pointf2x(gducks[1]/360,gducks[2]/360);
				drawfpoint(c,C32BLACK,2);
			}
		}
	}
}

void drawduckscircle()
{
	drawfcirclef(pointf2x(-.5f,-.5f),C32BLACK,.5f);
	U32 i;
	for (i=0;i<NUMDUCKS;++i) {
		float a = cducks[i]*PIOVER180;
		pointf2 dck = pointf2x(-.5f*cosf(a) - .5f,.5f*sinf(a) - .5f);
		drawfpoint(dck,C32BLUE,5);
		drawfpoint(dck,C32YELLOW);
	}
	C32 res = ducksOnOneSide(cducks,NUMDUCKS) ? C32GREEN : C32RED;
	drawfpoint(pointf2x(-.5f,-.5f),res,20);
}

#endif

#ifdef TRIANGLEPUZZLE
pointf2 intsect2d(const pointf2& p1,const pointf2& p2,const pointf2& p3,const pointf2& p4)
{
	pointf2 ret;
	float x1y2 = p1.x*p2.y;
	float y1x2 = p1.y*p2.x;
	float x3y4 = p3.x*p4.y;
	float y3x4 = p3.y*p4.x;
	float numx = (x1y2 - y1x2)*(p3.x - p4.x) - (p1.x - p2.x)*(x3y4 - y3x4);
	float numy = (x1y2 - y1x2)*(p3.y - p4.y) - (p1.y - p2.y)*(x3y4 - y3x4);
	float den = (p1.x-p2.x)*(p3.y-p4.y)-(p1.y-p2.y)*(p3.x-p4.x);
	ret.x = numx/den;
	ret.y = numy/den;
	return ret;
}

void drawtrianglepuzzle()
{
	drawfline(pointf2x(0,0),pointf2x(7,0),C32BLACK);
	pointf2 p0 = {0,0};
	pointf2 p1 = {7,0};
	pointf2 p2,p3;
	mat2 rot,rot2;
	float rdn = angA*PIOVER180;
	float rdn2 = rdn*2;
	float sina = sinf(rdn);
	float cosa = cosf(rdn);
	float sina2 = sinf(rdn2);
	float cosa2 = cosf(rdn2);
	rot.e[0][0]  =  cosa;
	rot.e[0][1]  =  sina;
	rot.e[1][0]  = -sina;
	rot.e[1][1]  =  cosa;
	rot2.e[0][0] =  cosa2;
	rot2.e[0][1] =  sina2;
	rot2.e[1][0] = -sina2;
	rot2.e[1][1] =  cosa2;
	pointf2 ten = {10,0};
	pointf2 five = {5,0};
	mulmatvec2(&rot,&ten,&p3);
	mulmatvec2(&rot2,&five,&p2);
	drawfline(p0,p2,C32BLACK);
	drawfline(p1,p2,C32BLACK);
	drawfline(p0,p3,C32BLACK);
	//pointf2x p3I(p3.x + 1,p3.y + 1);
	p3I = intsect2d(p0,p3,p1,p2);

	should3 = dist2d(&p3I,&p2);
	isY = dist2d(&p3I,&p1);

	drawfpoint(p3I,C32RED);
}
#endif

#ifdef DONAMESPACE
} // end namespace fourducks
using namespace fourducks;
#endif

void plot2fourducksinit()
{
	adddebvars("ducks",ducksdv,nducksdv);
	plotter2init();
	//lzoom = -2.5f;
	calcducks();
}

void plot2fourducksproc()
{
	// interact with graph paper
	plotter2proc();
	// calc the image
	//mulmatvec2(&A,&inVec,&outVec);
	//xformgrid(&A,ingrid,outgrid);
	//calceigen();
	//result1 = quadratic(coa,cob,coc,r0,r1);
	//result2 = quadratic(coa,cob,coc,cr0,cr1);
	//quadratic(ccoa,ccob,ccoc,ccr0,ccr1);
}

void plot2fourducksdraw2d()
{
	// draw graph paper
	plotter2draw2d();
#ifdef DUCKS
	drawducksgrid();
	drawduckscircle();
#endif
#ifdef TRIANGLEPUZZLE
	drawtrianglepuzzle();
#endif
	/*if (eigenreal) {
		drawfline(pointf2x(),pointf2x(eigenvalue0*eigenvector0.x,eigenvalue0*eigenvector0.y),C32LIGHTRED);
		drawfline(pointf2x(),pointf2x(eigenvalue1*eigenvector1.x,eigenvalue1*eigenvector1.y),C32LIGHTGREEN);
		drawfcircle(pointf2x(0,0),C32LIGHTRED,S32(math2screen(abs(eigenvalue0))));
		drawfcircle(pointf2x(0,0),C32LIGHTGREEN,S32(math2screen(abs(eigenvalue1))));
	}
	drawfline(pointf2x(),inVec,C32BLUE);
	drawfline(pointf2x(),outVec,C32LIGHTBLUE);*/
}

void plot2fourducksexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("ducks");
}
