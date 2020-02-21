#include <float.h> // for _isnan

// plotter2 runs in FPU control high precision
// first test of plotter2
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_plotter2.h"
#define BLOCH
//#define SIMPLE
//#define ISINSIDE
//#define HALFSPACE
//#define XYZtoUV
//#define LOGXY
//#define EARTHCURVE

using namespace u_plotter2;

namespace simplep2 {
#ifdef LOGXY
pointf2 limp = { 1.125f,1.125f};
float convergefunc(float t)
{
	return 1/t;
}
float limpres;
#endif
#ifdef SIMPLE
// an int, 2 points, and an average point
S32 simple = 69;
pointf2 p0 = {.125f,.25f};
pointf2 p1 = {.375f,.875f};
pointf2 avg;
float X,SolX;
#endif
#ifdef EARTHCURVE
double earthRadiusMiles = 3963;
double distToCenterLineMiles = 3; // total of 6 miles
double curveHeightMiles;
double curveHeightInches; // should be around 72 inches
double firstTaylorOrderMiles; // better precision using Taylor approximation
double firstTaylorOrderInches;
double secondTaylorOrderMiles; // better precision using Taylor approximation
double secondTaylorOrderInches;


#endif
#ifdef BLOCH
// 2d bloch, 2 way conversion between bloch sphere and quantum state
const float QUEPSILON = .01f; // more tolerance
//typedef complex<float> compf;
struct pointf2c {
	compf A0; // a|0>
	compf B1; // b|1>
	float normalize()
	{
		float r = sqrtf(norm(A0) + norm(B1));
		if (r<EPSILON)
			return 0;
		if (my_isnan(r))
			return 0;
		float ir=1.0f/r;
		A0 *= ir;
		B1 *= ir;
		nanerr("pointf2c normalize",r);
		return r;
	}
};

struct pointf2cx : public pointf2c {
	pointf2cx(compf xa=0,compf ya=0) {A0=xa;B1=ya;}
};

// return length
// make b vector normal without changing Z value, good for UI quantum
float normalize3dkeepZ(const struct pointf3 *a,struct pointf3 *b)
{
	float r,ir;
	r=a->x*a->x+a->y*a->y;
	r=sqrtf(r);
	if (r<EPSILON)
		return 0;
	if (my_isnan(r))
		return 0;
	ir=sqrtf(1-a->z*a->z)/r;
	if (ir <EPSILON) {
		return normalize3d(a,b);
	}
	b->x=ir*a->x;
	b->y=ir*a->y;
	b->z=a->z;
	//b->z=ir*a->z;
	nanerr("normalize3dkeepZ",r);
	return r;
}

pointf2c ampstate = {1,0}; // a|0> + b|1>, stick with complex numbers, bloch sphere
pointf3x blochstate(1,0,0); // bloch sphere, .x is forward(+X), .y is right(+Y), .z is up(+Z)
pointf2 orig; // (0,0)
S32 bloch2amp;
S32 jamZ;
S32 normalState = 1;
#endif
#ifdef ISINSIDE
pointf2 threep[3] = {{1,1},{2,2},{3,0}};
pointf2 testpoint = {.5,.5};
int isinside;
#endif
#ifdef HALFSPACE
pointf2 hnormal = {3,2};
float hdist = 5;
pointf2 htestpoint = {.5,.5};
int ishalf;
float linesize = 20;
#endif
#ifdef XYZtoUV
pointf3 Pxyz = {0,1,0};
pointf2 Muv;
// to UVS 0,0 to 1,1 (or some multiple maybe)
void convertXYZtoUV(const pointf3* xyz,pointf2* auv)
{
	// assume non-degenerates, but do handle the poles
	float xzlen2 = xyz->x*xyz->x + xyz->z*xyz->z;
	if (xzlen2 <= EPSILON) {
		if (xyz->y >= 0) { // north pole
			auv->x = 0;
			auv->y = 0;
		} else { // south pole
			auv->x = 0;
			auv->y = 1;
		}
	} else { // not a pole
		auv->x = atan2f(xyz->x,-xyz->z);
		float xzlen = sqrtf(xzlen2);
		auv->y = atan2f(xzlen,xyz->y);
		// make uvs go from 0 to 1
		auv->x *= (.5f/PI);
		if (auv->x < 0)
			auv->x += 1;
		auv->y *= (1.0f/PI);

	}
}
#endif
// for debvars
struct menuvar simpledv[]={
	{"@lightred@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
#ifdef BLOCH
	{"|0>R",&((pointf2*)(&ampstate.A0))->x,D_FLOAT,FLOATUP/16},
	{"|0>I",&((pointf2*)(&ampstate.A0))->y,D_FLOAT,FLOATUP/16},
	{"|1>R",&((pointf2*)(&ampstate.B1))->x,D_FLOAT,FLOATUP/16},
	{"|1>I",&((pointf2*)(&ampstate.B1))->y,D_FLOAT,FLOATUP/16},
	{"normalize Qstate",&normalState,D_INT},
	{"X",&blochstate.x,D_FLOAT,FLOATUP/16},
	{"Y",&blochstate.y,D_FLOAT,FLOATUP/16},
	{"Z",&blochstate.z,D_FLOAT,FLOATUP/16},
	{"bloch2amp",&bloch2amp,D_INT},
	{"jamZ",&jamZ,D_INT},
#endif
#ifdef LOGXY
	{"limpx",&limp.x,D_FLOAT,FLOATUP/16},
	{"limpy",&limp.y,D_FLOAT,FLOATUP/16},
	{"limpres",&limpres,D_FLOAT|D_RDONLY},
#endif
#ifdef EARTHCURVE
	{"earthRadiusMiles",&earthRadiusMiles,D_DOUBLE,FLOATUP},
	{"distToCenterLineMiles",&distToCenterLineMiles,D_DOUBLE,FLOATUP/32},
	{"curveHeightMiles",&curveHeightMiles,D_DOUBLE|D_RDONLY},
	{"curveHeightInches",&curveHeightInches,D_DOUBLE|D_RDONLY},
	{"firstTaylorOrderMiles",&firstTaylorOrderMiles,D_DOUBLE|D_RDONLY},
	{"firstTaylorOrderInches",&firstTaylorOrderInches,D_DOUBLE|D_RDONLY},
	{"secondTaylorOrderMiles",&secondTaylorOrderMiles,D_DOUBLE|D_RDONLY},
	{"secondTaylorOrderInches",&secondTaylorOrderInches,D_DOUBLE|D_RDONLY},
#endif
#ifdef ISINSIDE
	{"p0.x",&threep[0].x,D_FLOAT,FLOATUP/16},
	{"p0.y",&threep[0].y,D_FLOAT,FLOATUP/16},
	{"p1.x",&threep[1].x,D_FLOAT,FLOATUP/16},
	{"p1.y",&threep[1].y,D_FLOAT,FLOATUP/16},
	{"p2.x",&threep[2].x,D_FLOAT,FLOATUP/16},
	{"p2.y",&threep[2].y,D_FLOAT,FLOATUP/16},
	{"point.x",&testpoint.x,D_FLOAT,FLOATUP/16},
	{"point.y",&testpoint.y,D_FLOAT,FLOATUP/16},
	{"isinside",&isinside,D_INT|D_RDONLY},
#endif
#ifdef HALFSPACE
	{"half normal.x",&hnormal.x,D_FLOAT,FLOATUP/16},
	{"half normal.y",&hnormal.y,D_FLOAT,FLOATUP/16},
	{"half dist",&hdist,D_FLOAT,FLOATUP/16},
	{"half test point.x",&htestpoint.x,D_FLOAT,FLOATUP/16},
	{"half test point.y",&htestpoint.y,D_FLOAT,FLOATUP/16},
	{"is half",&ishalf,D_INT|D_RDONLY},
	{"line size",&linesize,D_FLOAT,FLOATUP/16},
#endif
#ifdef XYZtoUV
	{"PX in>",&Pxyz.x,D_FLOAT,FLOATUP/4},
	{"PY in>",&Pxyz.y,D_FLOAT,FLOATUP/4},
	{"PZ in>",&Pxyz.z,D_FLOAT,FLOATUP/4},
	{"U out>",&Muv.x,D_FLOAT|D_RDONLY},
	{"V out>",&Muv.y,D_FLOAT|D_RDONLY},
#endif
#ifdef SIMPLE
	{"simple",&simple,D_INT},
	{"p0.x",&p0.x,D_FLOAT,FLOATUP/16},
	{"p0.y",&p0.y,D_FLOAT,FLOATUP/16},
	{"p1.x",&p1.x,D_FLOAT,FLOATUP/16},
	{"p1.y",&p1.y,D_FLOAT,FLOATUP/16},
	{"avg.x",&avg.x,D_FLOAT|D_RDONLY},
	{"avg.y",&avg.y,D_FLOAT|D_RDONLY},
	{"sqrt problem X",&X,D_FLOAT,FLOATUP/16},
	{"sqrt problem SolX",&SolX,D_FLOAT|D_RDONLY},
#endif
};
const int nsimpledv = NUMELEMENTS(simpledv);

#define LOGTEST
#ifdef LOGTEST
float logb(float b,float in)
{
	return log(in)/log(b);
}
#endif

} // end namespace simplep2

using namespace simplep2;

void plot2simpleinit()
{
#ifdef LOGTEST
	logger("LOG TEST\n");
	logger("logb of something = %f\n",logb(2,512));
	float f;
	float m = 1;
	for (f = 2;f<=7;++f) {
		float lr = logb(f,f+1);
		logger("f = %f, logs = %f\n",f,lr);
		m *= lr;
	}
	logger("answer = %f\n",m);
#endif

	adddebvars("simple",simpledv,nsimpledv);
	plotter2init();
}

void plot2simpleproc()
{
	// interact with graph paper
	plotter2proc();
#ifdef LOGXY
	//limpres = limp.x*limp.y;
	limpres = log(limp.x)/log(limp.y) + log(limp.y)/log(limp.x);
#endif
#ifdef SIMPLE
	// calc avg
	avg.x = (p0.x + p1.x) * .5f;
	avg.y = (p0.y + p1.y) * .5f;
	SolX = sqrtf(X+15) + sqrtf(X);
#endif
#ifdef EARTHCURVE
// use Pythagorean
// bad precision maybe
	double hypotMiles = sqrt(earthRadiusMiles*earthRadiusMiles + distToCenterLineMiles*distToCenterLineMiles);

	curveHeightMiles = hypotMiles - earthRadiusMiles;

	// Taylor goes sqrt(1+x) = 1 + x/2
	double rat = distToCenterLineMiles/earthRadiusMiles;
	//firstTaylorOrderMiles = earthRadiusMiles*rat*rat/2;
	firstTaylorOrderMiles = distToCenterLineMiles*distToCenterLineMiles/(2*earthRadiusMiles);

	// Taylor goes sqrt(1+x) = 1 + x/2 - x^2/8
	secondTaylorOrderMiles = firstTaylorOrderMiles - earthRadiusMiles*rat*rat*rat*rat/8;

	curveHeightInches = curveHeightMiles * 5280 * 12; // convert miles to inches
	firstTaylorOrderInches = firstTaylorOrderMiles * 5280 * 12; // convert miles to inches
	secondTaylorOrderInches = secondTaylorOrderMiles * 5280 * 12; // convert miles to inches

#endif
#ifdef BLOCH
	bloch2amp = range(0,bloch2amp,1);
	jamZ = range(0,jamZ,1);
	normalState = range(0,normalState,1);
	// convert ampstate to blochstate
	if (bloch2amp) {
		// fixup blochstate
		float len;
		if (jamZ)
			len = normalize3dkeepZ(&blochstate,&blochstate);
		else
			len = normalize3d(&blochstate,&blochstate);
		if (len == 0)
			blochstate = pointf3x(0,0,1);
		// convert
		if (blochstate.z + 1.0f < QUEPSILON*QUEPSILON) {
			ampstate = pointf2cx(0,1); // close to bottom
		} else {
			float a0 = sqrtf((blochstate.z + 1.0f)*.5f);
			ampstate.A0 = a0;
			float a0ih = .5f/a0;
			ampstate.B1 = compf(blochstate.x*a0ih,blochstate.y*a0ih);
		}
	} else {
		// fixup ampstate
		if (normalState) {
			float len = ampstate.normalize();
			if (len == 0)
				ampstate = pointf2cx(1,0);
		}
#if 0
		// see if a < 0, if so flip whole phase by PI
		if (ampstate.x.real() < 0) {
			ampstate.x = -ampstate.x;
			ampstate.y = -ampstate.y;
		}
		// if a < epsilon, make state b positive
		float norm0 = sqrtf(norm(ampstate.x));
		if (norm0 < QUEPSILON) {
			if (ampstate.y.real() < 0)
				ampstate.y = -ampstate.y;
		}
#endif
		// convert
		// TODO adjust phase
		float alen = abs(ampstate.A0);
		compf ph;
		if (alen > EPSILON) {
			ph = compf(ampstate.A0.real()/alen,-ampstate.A0.imag()/alen);
			float AP = alen;
			compf BP = ampstate.B1 * ph;
			blochstate.z = 2.0f*AP*AP - 1;
			blochstate.x = 2.0f*AP*BP.real();
			blochstate.y = 2.0f*AP*BP.imag();
		} else {
			blochstate = pointf3x(0,0,-1);
		}
	}
#endif
#ifdef ISINSIDE
	pointf2x v0 = pointf2x(threep[0].x-testpoint.x,threep[0].y-testpoint.y);
	pointf2x v1 = pointf2x(threep[1].x-testpoint.x,threep[1].y-testpoint.y);
	pointf2x v2 = pointf2x(threep[2].x-testpoint.x,threep[2].y-testpoint.y);
	float c0 = cross2d(&v0,&v1);
	float c1 = cross2d(&v1,&v2);
	float c2 = cross2d(&v2,&v0);
	bool allpos = c0 > 0 && c1 > 0 && c2 > 0;
	bool allneg = c0 < 0 && c1 < 0 && c2 < 0;
	isinside = allpos || allneg;
#endif
#ifdef HALFSPACE
	normalize2d(&hnormal,&hnormal);
	ishalf = dot2d(&hnormal,&htestpoint) < hdist;
#endif
#ifdef XYZtoUV
	convertXYZtoUV(&Pxyz,&Muv);
#endif
}

void plot2simpledraw2d()
{
	// draw graph paper
	plotter2draw2d();
#ifdef LOGXY
	drawfunctionrange(convergefunc,.125f,8);
	drawfpoint(limp,C32BROWN);
#endif
#ifdef SIMPLE
	// draw the points and avg point and connect with a line
	drawfpoint(p0,C32RED);
	drawfpoint(p1,C32RED);
	drawfline(p0,p1,C32BLUE);
	drawfpoint(avg,C32MAGENTA);
#endif
#ifdef BLOCH
	drawfcirclef(orig,C32(85,0,0),1.0f);
	// |0> is y axis, |1> is x axis
	// draw bloch
	drawfpoint(pointf2x(blochstate.y,blochstate.z),C32BLUE,8);
	drawfpoint(pointf2x(blochstate.x,blochstate.z),C32RED,8);
	// draw qstate
	drawfpoint(pointf2x(ampstate.B1.imag(),ampstate.A0.imag()),C32LIGHTBLUE); // xplot is Z axis, yplot is Y axis
	drawfpoint(pointf2x(ampstate.B1.real(),ampstate.A0.real()),C32LIGHTRED); // xplot is Z axis, yplot is X axis
#endif
#ifdef ISINSIDE
	drawfline(threep[0],threep[1],C32BLACK);
	drawfline(threep[1],threep[2],C32BLACK);
	drawfline(threep[2],threep[0],C32BLACK);
	drawfpoint(testpoint,isinside ? C32RED : C32BLUE);
#endif
#ifdef HALFSPACE
	// draw line calculated from hnormal and hdist
	pointf2x dpoint(hnormal.x*hdist,hnormal.y*hdist);
	drawfpoint(dpoint,C32BROWN);
	pointf2x p0(dpoint.x + hnormal.y*linesize,dpoint.y - hnormal.x*linesize);
	pointf2x p1(dpoint.x - hnormal.y*linesize,dpoint.y + hnormal.x*linesize);
	drawfline(p0,p1,C32BLACK);
	// draw normal
	const pointf2x hnormalend(dpoint.x + hnormal.x,dpoint.y + hnormal.y);
	drawfline(dpoint,hnormalend,C32BLUE);
	drawfpoint(hnormalend,C32GREEN);
	// now draw test point
	drawfpoint(htestpoint,ishalf ? C32RED : C32BLUE);
#endif
}

void plot2simpleexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("simple");
}
