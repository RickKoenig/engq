// plotter system
// search for includes, to turn on and off various switches / includes

#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"
#include "rungekutta.h"
#include <math_abstract.h>

//#include <complex>

#include "d2_font.h" // font queue

#include "u_ar_scan.h"
#include "u_ar_parse.h"

#define USEPLOTTERNAMESPACE // doesn't work with doar::doar
#ifdef USEPLOTTERNAMESPACE
namespace u_s_plotter {
#endif

// geom
float zoom = .21f;//.15f;//.25f;
float lzoom = logf(zoom); // yes, zoom
pointf2 center={0,0};
//pointf2 center={.5f,0};
// system colors
C32  backcolor;
C32  axiscolor;
C32 gridcolor1;
C32 gridcolor2;
C32 gridcolor3;
// user colors
C32  funccolor;
C32 slopecolor;
S32 nsteps=500;
pointf2 plot; // location of mouse in plot coords
#if 0
float le=1;
float scl=1;
float off=0;
#endif
pointi2 math2screen(const pointf2& p)
{
	pointi2 r;
	r.x=static_cast<int>(WX/2 + zoom * WY/2 * (p.x-center.x));
	r.y=static_cast<int>(WY/2 - zoom * WY/2 * (p.y-center.y)); // '-' is from math to screen (flip)
	return r;
}

float math2screen(float p)
{
	return zoom * WY/2 * p + 1;
}

pointf2 screen2math(const pointi2& i)
{
	pointf2 r;
	r.x = center.x + (i.x - WX/2)/(zoom*WY/2);
	r.y = center.y + (i.y - WY/2)/(-zoom*WY/2);
	return r;
}

pointf2 newcenter(const pointi2& i,const pointf2& p)
{
	pointf2 nc;
	nc.x = p.x - (i.x - WX/2)/(zoom*WY/2);
	nc.y = p.y - (i.y - WY/2)/(-zoom*WY/2);
	return nc;
}

// lower left point on screen
pointf2 getminvisxy()
{
	pointf2 r;
	r.x=-float(WX)/WY/zoom+center.x;
	r.y=-1.0f/zoom+center.y;
	return r;
}

// upper right point on screen
pointf2 getmaxvisxy()
{
	pointf2 r;
	r.x=float(WX)/WY/zoom+center.x;
	r.y=1.0f/zoom+center.y;
	return r;
}

void drawfpoint(const pointf2& c,C32 clr,float rad = 5)
{
	pointi2 ci=math2screen(c);
//	clipputpixel32(B32,ci.x,ci.y,clr);
	clipcircle32(B32,ci.x,ci.y,(S32)rad,clr);
}

void drawfcircle(const pointf2& c,C32 clr,S32 r)
{
	pointi2 ci=math2screen(c);
	drawfpoint(c,clr);
	clipcircleo32(B32,ci.x,ci.y,r,clr);
}

void drawfhair(const pointf2& c,C32 clr)
{
	pointi2 ci=math2screen(c);
	clipline32(B32,ci.x-4,ci.y,ci.x+4,ci.y,clr);
	clipline32(B32,ci.x,ci.y-4,ci.x,ci.y+4,clr);
}

void drawfline(const pointf2& a,const pointf2& b,C32 c)
{
	pointi2 ai=math2screen(a),bi=math2screen(b);
	clipline32(B32,ai.x  ,ai.y  ,bi.x  ,bi.y  ,c);
	clipline32(B32,ai.x  ,ai.y-1,bi.x  ,bi.y-1,c);
	clipline32(B32,ai.x  ,ai.y+1,bi.x  ,bi.y+1,c);
	clipline32(B32,ai.x-1,ai.y  ,bi.x-1,bi.y  ,c);
	clipline32(B32,ai.x+1,ai.y  ,bi.x+1,bi.y  ,c);
}


void drawflinec(const pointf2& a,const pointf2& b,C32 c)
{
	pointi2 ai=math2screen(a),bi=math2screen(b);
	clipline32(B32,ai.x  ,ai.y  ,bi.x  ,bi.y  ,c);
	clipline32(B32,ai.x  ,ai.y-1,bi.x  ,bi.y-1,c);
	clipline32(B32,ai.x  ,ai.y+1,bi.x  ,bi.y+1,c);
	clipline32(B32,ai.x-1,ai.y  ,bi.x-1,bi.y  ,c);
	clipline32(B32,ai.x+1,ai.y  ,bi.x+1,bi.y  ,c);
	drawfpoint(a,c);
	drawfpoint(b,c);
}

void drawflineh(const pointf2& a,const pointf2& b,C32 c)
{
	pointi2 ai=math2screen(a),bi=math2screen(b);
	clipline32(B32,ai.x,ai.y,bi.x,bi.y,c);
	drawfhair(a,c);
	drawfhair(b,c);
}

void drawbox2(const pointf2& a,const pointf2& b,C32 c)
{
	drawfline(a,pointf2x(b.x,a.y),c);
	drawfline(pointf2x(a.x,b.y),b,c);
	drawfline(a,pointf2x(a.x,b.y),c);
	drawfline(pointf2x(b.x,a.y),b,c);
}


const int maxgrid = 100;
const int tens = maxgrid/10;
void drawaxis()
{
	pointf2 a,b;
	a.x=0;
	a.y=-maxgrid;
	b.x=0;
	b.y=maxgrid;
	drawfline(a,b,axiscolor);
	a.x=-maxgrid;
	a.y=0;
	b.x=maxgrid;
	b.y=0;
	drawfline(a,b,axiscolor);
}

void drawgrid()
{
	for (int i=-4*maxgrid;i<=4*maxgrid;++i) {
		pointf2 a,b;
		a.x=static_cast<float>(i)/4;
		a.y=-maxgrid;
		b.x=static_cast<float>(i)/4;
		b.y=maxgrid;
		drawfline(a,b,gridcolor1);
	}
	for (int j=-4*maxgrid;j<=4*maxgrid;++j) {
		pointf2 a,b;
		a.x=-maxgrid;
		a.y=static_cast<float>(j)/4;
		b.x=maxgrid;
		b.y=static_cast<float>(j)/4;
		drawfline(a,b,gridcolor1);
	}
	for (int i=-maxgrid;i<=maxgrid;++i) {
		pointf2 a,b;
		a.x=static_cast<float>(i);
		a.y=-maxgrid;
		b.x=static_cast<float>(i);
		b.y=maxgrid;
		drawfline(a,b,gridcolor2);
	}
	for (int j=-maxgrid;j<=maxgrid;++j) {
		pointf2 a,b;
		a.x=-maxgrid;
		a.y=static_cast<float>(j);
		b.x=maxgrid;
		b.y=static_cast<float>(j);
		drawfline(a,b,gridcolor2);
	}
	for (int i=-tens;i<=tens;++i) {
		pointf2 a,b;
		a.x=static_cast<float>(10*i);
		a.y=-maxgrid;
		b.x=static_cast<float>(10*i);
		b.y=maxgrid;
		drawfline(a,b,gridcolor3);
	}
	for (int j=-tens;j<=tens;++j) {
		pointf2 a,b;
		a.x=-maxgrid;
		a.y=static_cast<float>(10*j);
		b.x=maxgrid;
		b.y=static_cast<float>(10*j);
		drawfline(a,b,gridcolor3);
	}
}

// end grid system
/*float prea=0;
float prem=1;
float posta=0;
float postm=1;*/

static const float maxchange = 10000.0f;
// y = f(x) drawer
void drawfunction(float (*f)(float))
{
	pointf2 minxy=getminvisxy();
	pointf2 maxxy=getmaxvisxy();
	S32 i;
	pointf2 p;
	p.x=minxy.x; // previous
	p.y=f(p.x);
	for (i=1;i<=nsteps;++i) {
		pointf2 c;
		c.x=(maxxy.x-minxy.x)*i/nsteps+minxy.x;
//		c.y=f(c.x*prem+prea);
//		c.y=postm*c.y+posta;
		c.y=f(c.x);
		if (fabs(c.y-p.y)<maxchange)
			drawfline(p,c,funccolor);
		p=c;
	}
}

// y = f[n](x) drawer
void drawfunction_n(float (*f)(float,S32),S32 n,C32 col)
{
	pointf2 minxy=getminvisxy();
	pointf2 maxxy=getmaxvisxy();
	S32 i;
	pointf2 p;
	p.x=minxy.x; // previous
	p.y=f(p.x,n);
	for (i=1;i<=nsteps;++i) {
		pointf2 c;
		c.x=(maxxy.x-minxy.x)*i/nsteps+minxy.x;
//		c.y=f(c.x*prem+prea);
//		c.y=postm*c.y+posta;
		c.y=f(c.x,n);
		if (fabs(c.y-p.y)<maxchange)
			drawfline(p,c,col);
		p=c;
	}
}

// y = f(x) drawer
void drawfunctionrange(float (*f)(float),float start,float end)
{
	pointf2 minxy=getminvisxy();
	pointf2 maxxy=getmaxvisxy();

	minxy.x = max(minxy.x,start);
	maxxy.x = min(maxxy.x,end);
	if (minxy.x >= maxxy.x)
		return;

	S32 i;
	pointf2 p;
	p.x=minxy.x; // previous
	p.y=f(p.x);
	for (i=1;i<=nsteps;++i) {
		pointf2 c;
		c.x=(maxxy.x-minxy.x)*i/nsteps+minxy.x;
//		c.y=f(c.x*prem+prea);
//		c.y=postm*c.y+posta;
		c.y=f(c.x);
		if (fabs(c.y-p.y)<maxchange) // skip big jumps
			drawfline(p,c,funccolor);
		p=c;
	}
}

// y = f[n](x) drawer
void drawfunctionrange_n(float (*f)(float,S32),float start,float end,S32 n)
{
	pointf2 minxy=getminvisxy();
	pointf2 maxxy=getmaxvisxy();

	minxy.x = max(minxy.x,start);
	maxxy.x = min(maxxy.x,end);
	if (minxy.x >= maxxy.x)
		return;

	S32 i;
	pointf2 p;
	p.x=minxy.x; // previous
	p.y=f(p.x,n);
	for (i=1;i<=nsteps;++i) {
		pointf2 c;
		c.x=(maxxy.x-minxy.x)*i/nsteps+minxy.x;
//		c.y=f(c.x*prem+prea);
//		c.y=postm*c.y+posta;
		c.y=f(c.x,n);
		if (fabs(c.y-p.y)<maxchange) // skip big jumps
			drawfline(p,c,funccolor);
		p=c;
	}
}

// end y = f(x) drawer

// x,y = fxy(t) parametric drawer
void drawfunction2(pointf2 (*f)(float t))
{
	S32 i;
	pointf2 p;
	p=f(0.0f);
	for (i=1;i<=nsteps;++i) {
		float t=float(i)/float(nsteps);
		pointf2 c=f(t);
		drawfline(p,c,funccolor);
		p=c;
	}
}

// x,y = fxy(t) parametric drawer
void drawfunction2(const vector<pointf2>& f)
{
	if (!f.size())
		return;
	S32 nsteps = f.size() - 1; // number of line segments
	S32 i;
	pointf2 p = f[0];
	for (i=1;i<=nsteps;++i) {
		pointf2 c=f[i];
		drawfline(p,c,funccolor);
		p=c;
	}
}

// 1 2d x,y point influenced by x,y,t

enum {POSX,POSY,POSU,NOFFSETS};
static float objstate[NOFFSETS]; // rungekutta x,y
static int offsets[NOFFSETS]={POSX,POSY,POSU};

void drawfunctionrk2d(float* objs, S32* offs,S32 noffs,
					void (*initfunc)(float *os),
					void (*changefunc)(float *os,float *newos,float time))
{
	initfunc(objs);
	pointf2 p=pointf2x(objs[POSX],objs[POSY]);
	S32 i;
	float tinc=1.0f/nsteps;
	for (i=1;i<=nsteps;++i) {
		float t=float(i)*tinc;
		dorungekutta(objs,offs,noffs,t,tinc,changefunc);
		pointf2 c=pointf2x(objs[POSX],objs[POSY]);
		drawfline(p,c,funccolor);
		p=c;
	}
}

// end x,y = fxy(t) parametric drawer

// begin user section
/*const S32 MAXPNTS=2;
pointf2 pnts[MAXPNTS]={
	{0,0},
	{4,3},
};
*/

con32* con;
//float linm,linb;

// user functions
float sinfun(float a)
{
	return sinf(a);
}

float expe(float a)
{
	return expf(a);
}

float exp10(float a)
{
	return powf(10.0f,a);
}

/*float exp10le(float a)
{
	a*=le;
	return scl*powf(10.0f,a)+off;
}*/

static float r3=sqrtf(3.0f)*.5f;
float fc(float a)
{
	return expf(-.5f*a)*cosf(r3*a);
}

float fs(float a)
{
	return expf(-.5f*a)*sinf(r3*a);
}

float root3_0(float c)
{
	return fc(c);
}

float root3_1(float c)
{
	return -.5f*fc(c)-r3*fs(c);
}

float root3_2(float c)
{
	return -.5f*fc(c)+r3*fs(c);
}

float xs(float c)
{
	return c*c;
}

float beercan(float x)
{
	return (25+2*x*x)/(5+4*x);
}

float quintic(float x)
{
	return x*x*x*x*x-5*x*x*x*x+11*x*x*x-8*x+20;
}

float seventh(float x)
{
	return -x*x*x*x*x*x*x/5040+x*x*x*x*x/120-x*x*x/6+x;
}

// user function2s

pointf2 circ(float t)
{
	pointf2 r;
	t*=TWOPI;
	r.x=cosf(t);
	r.y=sinf(t);
	return r;
}

// user function rk


const float vel=TWOPI*359/360;
static void initf(float *os)
{
	os[POSX]=0;
	os[POSY]=1;
}

static void changef(float *os,float *delos,float curt)
{
	delos[POSX]=vel* os[POSY]; // dx/dt =  v * y
	delos[POSY]=vel*-os[POSX]; // dy/dt = -v * x
}

// here 't' == 'x'
float energy=9*PI*PI;
float qn; // estimated quantum number
float initslope=1.0f;
float v1start=.25f;
float v1end=.75f;
float v1=0.0f;
float vscale=1.0f/100.0f;
static void initfs(float *os)
{
	os[POSX]=0; // t or x
	os[POSY]=0; // psi(x)
	qn=sqrt(fabsf(energy))/PI;
	if (qn<1.0f)
		qn=1.0f;
	os[POSU]=PI*qn*initslope; // d/dx psi(x) // initial slope
}

static void changefs(float *os,float *delos,float curt)
{
	float vx;
	if (os[POSX]>=v1start && os[POSX]<=v1end)
		vx=v1;
	else
		vx=0;
	delos[POSX]=1;						// dx/dt =  1 // x == t
	delos[POSY]= os[POSU];				// dy/dt =  u // u is subst for dy/dx
	delos[POSU]=(vx-energy)*os[POSY];	// d2y/dx2 = -energy * y
}

#if 0
void calcstuff()
{
//	con32_clear(con);
/*	S32 i;
//		con32_printf(con,"PNT%c (%.3f,%.3f)\n",'A'+i,pnts[i].x,pnts[i].y);
// linear y=mx+b
	linm = (pnts[1].y-pnts[0].y)/(pnts[1].x-pnts[0].x);
	linb = pnts[0].y-linm*pnts[0].x;
	con32_printf(con,"Y = %.3fX + %.3f\n",linm,linb); */
//	pointf2 minxy=getminvisxy();
//	pointf2 maxxy=getmaxvisxy();
//	con32_printf(con,"minxy %f %f, maxxy %f %f\n",minxy.x,minxy.y,maxxy.x,maxxy.y);
//	con32_printf(con,"y=%f*10^(%f*x)+%f\n",scl,le,off);
}
#endif

// includes
// test projects, to include or not

//#include "plotter_code/decimal_rat.cpp"
//#include "plotter_code/make_penrose.cpp"
//#include "plotter_code/lens.cpp"
//#include "plotter_code/bell_small_angle.cpp"
//#include "plotter_code/sort2test.cpp"
//#include "plotter_code/regex.cpp"
//#include "plotter_code/quant.cpp"
//#include "plotter_code/quantsho.cpp"
//#include "plotter_code/linreg.cpp" // linear regression
//#include "plotter_code/line2sphere.cpp" // ray vs sphere
//#include "plotter_code/depthLUT.cpp" // find a function for depth map
//#include "plotter_code/cpptest.cpp" // test various c++ code
//#include "plotter_code/fib.cpp" // recurrence relations
//#include "plotter_code/lindiff.cpp" // linear differential equations
//#include "plotter_code/macrotest.cpp" // advanced macro stuff
//#include "plotter_code/cubic.cpp" // advanced macro stuff
//#include "plotter_code/ar_plot.cpp" // AR plot
//#include "plotter_code/test3dmatrix_old.cpp" // test 3d matrix pipleline
//#include "plotter_code/hough_test.cpp" // test hough lines
//#include "plotter_code/pers.cpp" // test perspective correction
//#include "plotter_code/dds2png.cpp" // converts dds and tga's to png's
#include "plotter_code/rotrect.cpp" // find bounding box of rotated rectangle
//#include "plotter_code/testline2btri.cpp" // test line to triangle
//#include "plotter_code/power.cpp" // complex power
//#include "plotter_code/zeta.cpp" // study zeta function
//#include "plotter_code/poles.cpp" // study complex analysis
//#include "plotter_code/makeasmallfont.cpp" // very small font
#include "plotter_code/passport.cpp"


void calcstuff()
{
#ifdef INTERP
	calcpoly();
#endif
#ifdef CUBIC
	calccubic();
#endif
#ifdef BOX2BOX
	calcbox2box();
#endif
#ifdef CIRCLE2BOX
	calcpr(box0);
#endif
#ifdef CIRCLE2CIRCLE
#endif
#ifdef TANG
	tang = vsub2vv(vr,vmul2sv(sdot2vv(vr,nrm),nrm));
#endif
#ifdef BELL
	using namespace bell;
	calc_bell();
#endif
}

void drawstuff()
{
#ifdef POWER
	power_draw();
#endif
#ifdef ZETAFILE
	zeta_draw();
#endif
#ifdef POLES
	poles_draw();
#endif
#ifdef LINE2BTRI
	line2btri_draw();
#endif
#ifdef ROTRECT
	rotrect_draw();
#endif
#ifdef LINDIFF
	drawlindiff();
#endif
#ifdef LINE2SPHERE
	drawline2sphere();
#endif
#ifdef QUANT
	drawquant();
#endif

#ifdef QUANTSHO
	drawquantsho();
#endif

#ifdef LENS
	drawlens();
#endif
#ifdef AR_PLOT
	draw_arplot();
#endif
#ifdef HOUGHTEST
	drawhoughtest();
#endif
#ifdef PERS
	drawpers();
#endif
#ifdef LINREG
	drawlinreg();
#endif

#ifdef INTERP
	drawinterp();
#endif
#ifdef CUBIC
	drawcubic();
#endif
#ifdef COLLUTILS
	drawcollutils(); // in coll2d.cpp
#endif

#ifdef TANG
	drawtang();
#endif
}
// end user section

#ifdef BELL
using namespace bell;
#endif

struct menuvar edv[]={
// user vars
	{"@lightred@--- PLOTTER USER VARS ---",NULL,D_VOID,0},
#ifdef ZETAFILE
	{"circlesize",&circsize,D_FLOAT,FLOATUP/64},
	{"pointiterx",&pointiterx,D_INT},
	{"pointitery",&pointitery,D_INT},
	{"lineratio",&lineratio,D_INT},
/*	{"lineiterx",&lineiterx,D_INT},
	{"lineitery",&lineitery,D_INT}, */
	{"base.x",&base.x,D_FLOAT,FLOATUP/4},
	{"base.y",&base.y,D_FLOAT,FLOATUP/4},
	{"basesep.x",&basesep.x,D_FLOAT,FLOATUP/16},
	{"basesep.y",&basesep.y,D_FLOAT,FLOATUP/16},
#ifdef DOPOWER
	{"exponent.x",&exponent.x,D_FLOAT,FLOATUP/4},
	{"exponent.y",&exponent.y,D_FLOAT,FLOATUP/4},
#endif
#ifdef DOZETA
	{"zetaiter",&zetaiter,D_INT},
	{"csum",&csum,D_INT},
#endif
	{"result.x",&result.x,D_FLOAT|D_RDONLY},
	{"result.y",&result.y,D_FLOAT|D_RDONLY},
#endif
#ifdef POLES
	{"curfuncstr",&curfuncstr,D_STRING},
	{"curfunc",&curfunc,D_INT},
	{"circlesize",&circsize,D_FLOAT,FLOATUP/64},
	{"poleiter",&poleiter,D_INT},
	{"base.x",&base.x,D_FLOAT,FLOATUP/16},
	{"base.y",&base.y,D_FLOAT,FLOATUP/16},
	{"radius",&radius,D_FLOAT,FLOATUP/64},
	{"result.x",&result.x,D_FLOAT|D_RDONLY},
	{"result.y",&result.y,D_FLOAT|D_RDONLY},
#endif
#ifdef POWER
	{"base.x",&base.x,D_FLOAT,FLOATUP/4},
	{"base.y",&base.y,D_FLOAT,FLOATUP/4},
	{"exponent.x",&exponent.x,D_FLOAT,FLOATUP/4},
	{"exponent.y",&exponent.y,D_FLOAT,FLOATUP/4},
	{"basesep.x",&basesep.x,D_FLOAT,FLOATUP/16},
	{"basesep.y",&basesep.y,D_FLOAT,FLOATUP/16},
	{"exponentsep.x",&exponentsep.x,D_FLOAT,FLOATUP/16},
	{"exponentsep.y",&exponentsep.y,D_FLOAT,FLOATUP/16},
/*	{"resulta.x",&resulta.x,D_FLOAT|D_RDONLY},
	{"resulta.y",&resulta.y,D_FLOAT|D_RDONLY},
	{"resultb.x",&resultb.x,D_FLOAT|D_RDONLY},
	{"resultb.y",&resultb.y,D_FLOAT|D_RDONLY}, */
	{"resultc.x",&resultc.x,D_FLOAT|D_RDONLY},
	{"resultc.y",&resultc.y,D_FLOAT|D_RDONLY},
/*	{"resultd.x",&resultd.x,D_FLOAT|D_RDONLY},
	{"resultd.y",&resultd.y,D_FLOAT|D_RDONLY}, */
#endif
#ifdef ROTRECT
	{"rect rot",&rrp.rot,D_FLOAT,FLOATUP/16},
	{"rect factor",&rrp.factor,D_FLOAT|D_RDONLY},
	{"rect diagonal",&rrp.diag,D_FLOAT|D_RDONLY},
	{"rect width",&rrp.width,D_FLOAT,FLOATUP},
	{"rect height",&rrp.height,D_FLOAT,FLOATUP},
#endif
#ifdef LINDIFF
	{"Y'' + P1*Y' + P0*Y = I * sin(W*X)",NULL,D_VOID,0},
	{"Lin Parm 1",&ldp.p1,D_FLOAT,FLOATUP/4},
	{"Lin Parm 0",&ldp.p0,D_FLOAT,FLOATUP/4},
	{"Intensity",&ldp.amp,D_FLOAT,FLOATUP/4},
	{"omega W",&ldp.w,D_FLOAT,FLOATUP/4},
	{"start y pos",&ldp.ypos,D_FLOAT,FLOATUP/4},
	{"start y vel",&ldp.yvel,D_FLOAT,FLOATUP/4},
	{"@brown@rkdeltime",&ldp.rkdeltime,D_FLOAT,FLOATUP/64},
	{"rkmaxx",&ldp.rkmaxx,D_FLOAT,FLOATUP/64},
//	{"transient",&ldp.transient,D_INT},
	{"@red@is imaginary",&ldr.root_imag,D_INT|D_RDONLY},
	{"is same",&ldr.root_same,D_INT|D_RDONLY},
	{"resonant",&ldr.resonant,D_INT|D_RDONLY},
	{"root0 real",&ldr.r0,D_FLOAT|D_RDONLY},
	{"root0 imag",&ldr.i0,D_FLOAT|D_RDONLY},
	{"root1 real",&ldr.r1,D_FLOAT|D_RDONLY},
	{"root1 imag",&ldr.i1,D_FLOAT|D_RDONLY},
	{"response",&ldr.response,D_FLOAT|D_RDONLY},
	{"phase",&ldr.phase,D_FLOAT|D_RDONLY},
#endif
#ifdef LINE2SPHERE
	{"rs.x",&rs.x,D_FLOAT,FLOATUP/16},
	{"rs.y",&rs.y,D_FLOAT,FLOATUP/16},
	{"rd.x",&rd.x,D_FLOAT,FLOATUP/16},
	{"rd.y",&rd.y,D_FLOAT,FLOATUP/16},
	{"extend",&extend,D_FLOAT,FLOATUP/16},
	{"pos.x",&pos.x,D_FLOAT,FLOATUP/16},
	{"pos.y",&pos.y,D_FLOAT,FLOATUP/16},
	{"radius",&radius,D_FLOAT,FLOATUP/16},
	{"--- SOLUTION ---",NULL,D_VOID,0},
	{"t0",&t0,D_FLOAT|D_RDONLY},
	{"t1",&t1,D_FLOAT|D_RDONLY},
	{"p0.x",&p0.x,D_FLOAT|D_RDONLY},
	{"p0.y",&p0.y,D_FLOAT|D_RDONLY},
	{"p1.x",&p1.x,D_FLOAT|D_RDONLY},
	{"p1.y",&p1.y,D_FLOAT|D_RDONLY},
#endif
#ifdef QUANT
	//{"A",&A,D_FLOAT,FLOATUP/16},
	//{"B",&B,D_FLOAT,FLOATUP/16},
	//{"K",&K,D_FLOAT,FLOATUP/16},
	//{"W",&W,D_FLOAT,FLOATUP/16},
	{"qnumber",&qnumber,D_INT},
	{"T0",&T0,D_FLOAT,FLOATUP/16},
	{"length",&length,D_FLOAT,FLOATUP/16},
	{"showcalc",&showcalc,D_INT},
	{"showsol",&showsol,D_INT},
#endif
#ifdef QUANTSHO
	{"Q number",&qnumber,D_INT},
#endif
#ifdef LINREG
	{"npnts",&npnts,D_INT},
	{"pnt0x",&pnts[0].x,D_FLOAT,FLOATUP/4},
	{"pnt0y",&pnts[0].y,D_FLOAT,FLOATUP/4},
	{"pnt1x",&pnts[1].x,D_FLOAT,FLOATUP/4},
	{"pnt1y",&pnts[1].y,D_FLOAT,FLOATUP/4},
	{"pnt2x",&pnts[2].x,D_FLOAT,FLOATUP/4},
	{"pnt2y",&pnts[2].y,D_FLOAT,FLOATUP/4},
	{"pnt3x",&pnts[3].x,D_FLOAT,FLOATUP/4},
	{"pnt3y",&pnts[3].y,D_FLOAT,FLOATUP/4},
	{"pnt4x",&pnts[4].x,D_FLOAT,FLOATUP/4},
	{"pnt4y",&pnts[4].y,D_FLOAT,FLOATUP/4},
	{"linregM",&lrm,D_FLOAT|D_RDONLY},
	{"linregB",&lrb,D_FLOAT|D_RDONLY},
#endif
#ifdef LENS
	{"focus",&focus,D_FLOAT,FLOATUP/16},
	{"objx",&objx,D_FLOAT,FLOATUP/16},
	{"objy",&objy,D_FLOAT,FLOATUP/16},
	{"imgx",&imgx,D_FLOAT|D_RDONLY},
	{"imgy",&imgy,D_FLOAT|D_RDONLY},
#endif
#ifdef BELL
	// namespace bell
	{"constrain",&constrain,D_INT,1},
	{"D",&D,D_FLOAT,fu},
	{"S",&S,D_FLOAT|D_RDONLY},
	{"a",&a,D_FLOAT,fu},
	{"b",&b,D_FLOAT,fu},
	{"c",&c,D_FLOAT,fu},
	{"d",&d,D_FLOAT,fu},
	{"e",&e,D_FLOAT,fu},
	{"f",&f,D_FLOAT,fu},
	{"g",&g,D_FLOAT,fu},
	{"h",&h,D_FLOAT,fu},
	{"Pone ",&Pone,D_FLOAT|D_RDONLY},
	{"Pxy ",&Pxy,D_FLOAT|D_RDONLY},
	{"Pyz ",&Pyz,D_FLOAT|D_RDONLY},
	{"Pxz ",&Pxz,D_FLOAT|D_RDONLY},
	//{"Pxresult",&Pxresult,D_FLOAT|D_RDONLY},
#endif
#ifdef INTERP
	{"x1",&x1,D_FLOAT,FLOATUP/16},
	{"x2",&x2,D_FLOAT,FLOATUP/16},
	{"x3",&x3,D_FLOAT,FLOATUP/16},
	{"a",&a,D_FLOAT|D_RDONLY},
	{"b",&b,D_FLOAT|D_RDONLY},
	{"c",&c,D_FLOAT|D_RDONLY},
#endif
#ifdef BOX2BOX
	{"box0x",&box0.pos.x,D_FLOAT,FLOATUP/16},
	{"box0y",&box0.pos.y,D_FLOAT,FLOATUP/16},
	{"box0r",&box0.rot,D_FLOAT,FLOATUP/16},
	{"box0w",&box0.size.x,D_FLOAT,FLOATUP/16},
	{"box0h",&box0.size.y,D_FLOAT,FLOATUP/16},
	{"box1x",&box1.pos.x,D_FLOAT,FLOATUP/16},
	{"box1y",&box1.pos.y,D_FLOAT,FLOATUP/16},
	{"box1r",&box1.rot,D_FLOAT,FLOATUP/16},
	{"box1w",&box1.size.x,D_FLOAT,FLOATUP/16},
	{"box1h",&box1.size.y,D_FLOAT,FLOATUP/16},
#ifdef BOXPOINT
	{"point0x",&point0.x,D_FLOAT,FLOATUP/16},
	{"point0y",&point0.y,D_FLOAT,FLOATUP/16},
	{"line0x",&la.x,D_FLOAT,FLOATUP/16},
	{"line0y",&la.y,D_FLOAT,FLOATUP/16},
	{"line1x",&lb.x,D_FLOAT,FLOATUP/16},
	{"line1y",&lb.y,D_FLOAT,FLOATUP/16},
	{"ldist",&ldist,D_FLOAT|D_RDONLY},
#endif
	{"abestcpx",&abestcp.x,D_FLOAT|D_RDONLY},
	{"abestcpy",&abestcp.y,D_FLOAT|D_RDONLY},
	{"abestpendirx",&abestpendir.x,D_FLOAT|D_RDONLY},
	{"abestpendiry",&abestpendir.y,D_FLOAT|D_RDONLY},
	{"acollide",&acollide,D_INT|D_RDONLY},
	{"penm",&penm,D_FLOAT|D_RDONLY},
	{"pcollide",&pcollide,D_INT|D_RDONLY},
#endif
#ifdef CIRCLE2BOX
	{"cir0x",&cir0.pos.x,D_FLOAT,FLOATUP/16},
	{"cir0y",&cir0.pos.y,D_FLOAT,FLOATUP/16},
	{"cir0rad",&cir0.rad,D_FLOAT,FLOATUP/16},
	{"box0x",&box0.pos.x,D_FLOAT,FLOATUP/16},
	{"box0y",&box0.pos.y,D_FLOAT,FLOATUP/16},
	{"box0rot",&box0.rot,D_FLOAT,FLOATUP/16},
	{"box0wid",&box0.size.x,D_FLOAT,FLOATUP/16},
	{"box0hit",&box0.size.y,D_FLOAT,FLOATUP/16},
#endif
#ifdef CIRCLE2CIRCLE
	{"cir0x",&cir0.pos.x,D_FLOAT,FLOATUP/16},
	{"cir0y",&cir0.pos.y,D_FLOAT,FLOATUP/16},
	{"cir0rad",&cir0.rad,D_FLOAT,FLOATUP/16},
	{"cir1x",&cir1.pos.x,D_FLOAT,FLOATUP/16},
	{"cir1y",&cir1.pos.y,D_FLOAT,FLOATUP/16},
	{"cir1rad",&cir1.rad,D_FLOAT,FLOATUP/16},
#endif
#ifdef CUBIC
	{"rootsep",&rootsep,D_FLOAT,FLOATUP/64},
	{"drawres",&drawres,D_INT,1},
	{"drawpow",&drawpow,D_INT,1},
	{"drawroots",&drawroots,D_INT,1},
	{"permnum",&permnum,D_INT,1},
	{"resnum",&resnum,D_INT,1},
	{"respow",&resp,D_INT,1},
	{"nroots",&nroots,D_INT,1},
	{"nperms",&nperms,D_INT|D_RDONLY,1},
	{"Root A r",&roots[0].x,D_FLOAT,FLOATUP/16},
	{"Root A i",&roots[0].y,D_FLOAT,FLOATUP/16},
	{"Root B r",&roots[1].x,D_FLOAT,FLOATUP/16},
	{"Root B i",&roots[1].y,D_FLOAT,FLOATUP/16},
	{"Root C r",&roots[2].x,D_FLOAT,FLOATUP/16},
	{"Root C i",&roots[2].y,D_FLOAT,FLOATUP/16},
	{"Root D r",&roots[3].x,D_FLOAT,FLOATUP/16},
	{"Root D i",&roots[3].y,D_FLOAT,FLOATUP/16},
	{"Root E r",&roots[4].x,D_FLOAT,FLOATUP/16},
	{"Root E i",&roots[4].y,D_FLOAT,FLOATUP/16},
#endif
/*	{"energy",&energy,D_FLOAT,FLOATUP/16},
	{"initslope",&initslope,D_FLOAT,FLOATUP/64},
	{"n ",&qn,D_FLOAT|D_RDONLY,FLOATUP},
	{"v1start",&v1start,D_FLOAT,FLOATUP/64},
	{"v1end",&v1end,D_FLOAT,FLOATUP/64},
	{"v1",&v1,D_FLOAT,FLOATUP/64},
	{"vscale",&vscale,D_FLOAT,FLOATUP/1024}, */
#if 0
	{"le",&le,D_FLOAT,FLOATUP/16},
	{"off",&off,D_FLOAT,FLOATUP/16},
	{"scl",&scl,D_FLOAT,FLOATUP/16},
	{"prem",&prem,D_FLOAT,FLOATUP/16},
	{"prea",&prea,D_FLOAT,FLOATUP/16},
	{"postm",&postm,D_FLOAT,FLOATUP/16},
	{"posta",&posta,D_FLOAT,FLOATUP/16},
	{"Point A x",&pnts[0].x,D_FLOAT,FLOATUP/4},
	{"Point A y",&pnts[0].y,D_FLOAT,FLOATUP/4},
	{"Point B x",&pnts[1].x,D_FLOAT,FLOATUP/4},
	{"Point B y",&pnts[1].y,D_FLOAT,FLOATUP/4},
#endif
/*	{"Point C x",&pnts[2].x,D_FLOAT,FLOATUP/4},
	{"Point C y",&pnts[2].y,D_FLOAT,FLOATUP/4},
	{"Point D x",&pnts[3].x,D_FLOAT,FLOATUP/4},
	{"Point D y",&pnts[3].y,D_FLOAT,FLOATUP/4},
	{"Point E x",&pnts[4].x,D_FLOAT,FLOATUP/4},
	{"Point E y",&pnts[4].y,D_FLOAT,FLOATUP/4}, */
#ifdef TANG
	{"vr.x",&vr.x,D_FLOAT,FLOATUP/4},
	{"vr.y",&vr.y,D_FLOAT,FLOATUP/4},
	{"nrm.x",&nrm.x,D_FLOAT,FLOATUP/4},
	{"nrm.y",&nrm.y,D_FLOAT,FLOATUP/4},
	{"tang.x",&tang.x,D_FLOAT|D_RDONLY},
	{"tang.y",&tang.y,D_FLOAT|D_RDONLY},
#endif
#ifdef AR_PLOT
	{"calc",&calcmatch,D_INT,1},
	{"fastcalc",&fastcalcmatch,D_INT,1},
	{"reset",&resetrottrans,D_INT,1},

	{"rotx",&arpi.arrot.x,D_FLOAT|D_RDONLY},
	{"roty",&arpi.arrot.y,D_FLOAT|D_RDONLY},
	{"rotz",&arpi.arrot.z,D_FLOAT|D_RDONLY},
	{"transx",&arpi.artrans.x,D_FLOAT|D_RDONLY},
	{"transy",&arpi.artrans.y,D_FLOAT|D_RDONLY},
	{"transz",&arpi.artrans.z,D_FLOAT|D_RDONLY},
	{"match error",&arpi.matcherror,D_FLOAT|D_RDONLY},
	{"stable",&arpi.matchstable,D_INT|D_RDONLY},
	{"bigerror",&arpi.bigerror,D_INT|D_RDONLY},
	{"step",&arpi.matchstep,D_FLOAT|D_RDONLY},
	{"current",&arpi.currentstr,D_STRING}, // |D_RDONLY, all strings are read only
//	{"pospitch",&arpi.pospitch,D_INT|D_RDONLY},
//	{"negpitch",&arpi.negpitch,D_INT|D_RDONLY},

	{"@lightred@--- pers and viewport ---",NULL,D_VOID,0},
	{"scan index",&scanidx,D_INT,1}, // select a sample scan
/*	{"zoomfactor",&zoomfactor,D_FLOAT|D_RDONLY},
	{"znear",&znear,D_FLOAT|D_RDONLY},
	{"zfar",&zfar,D_FLOAT|D_RDONLY},
	{"xres",&xres,D_FLOAT|D_RDONLY},
	{"yres",&yres,D_FLOAT|D_RDONLY},
	{"aspect ratio",&asp,D_FLOAT|D_RDONLY}, */
#endif
#ifdef HOUGHTEST
	{"PX",&hp.x,D_FLOAT,FLOATUP/16},
	{"PY",&hp.y,D_FLOAT,FLOATUP/16},
	{"theta",&htheta,D_FLOAT,FLOATUP/16},
	{"QX",&hq.x,D_FLOAT|D_RDONLY},
	{"QY",&hq.y,D_FLOAT|D_RDONLY},
	{"R",&hr,D_FLOAT|D_RDONLY},
#endif
#ifdef PERS
	{"t",&t,D_FLOAT,FLOATUP/16},
	{"s",&s,D_FLOAT,FLOATUP/16},
	{"p0x",&p0.x,D_FLOAT,FLOATUP/16},
	{"p0y",&p0.y,D_FLOAT,FLOATUP/16},
	{"p1x",&p1.x,D_FLOAT,FLOATUP/16},
	{"p1y",&p1.y,D_FLOAT,FLOATUP/16},
#endif
// plotter system vars
	{"@lightcyan@--- PLOTTER SYSTEM VARS ----",NULL,D_VOID,0},
	{"lzoom",&lzoom,D_FLOAT,FLOATUP/16},
	{"zoom",&zoom,D_FLOAT|D_RDONLY,FLOATUP},
	{"centerx",&center.x,D_FLOAT,FLOATUP/16},
	{"centery",&center.y,D_FLOAT,FLOATUP/16},
	{"plotx",&plot.x,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"ploty",&plot.y,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"nsteps",&nsteps,D_INT,1},
	{"@lightmagenta@--- PLOTTER SYSTEM COLORS ---",NULL,D_VOID,0},
	{" backcolor.r", &backcolor.r,D_CHAR,4},
	{" backcolor.g", &backcolor.g,D_CHAR,4},
	{" backcolor.b" ,&backcolor.b,D_CHAR,4},
	{" axiscolor.r" ,&axiscolor.r,D_CHAR,4},
	{" axiscolor.g", &axiscolor.g,D_CHAR,4},
	{" axiscolor.b", &axiscolor.b,D_CHAR,4},
	{"gridcolor1.r",&gridcolor1.r,D_CHAR,4},
	{"gridcolor1.g",&gridcolor1.g,D_CHAR,4},
	{"gridcolor1.b",&gridcolor1.b,D_CHAR,4},
	{"gridcolor2.r",&gridcolor2.r,D_CHAR,4},
	{"gridcolor2.g",&gridcolor2.g,D_CHAR,4},
	{"gridcolor2.b",&gridcolor2.b,D_CHAR,4},
	{"gridcolor3.r",&gridcolor3.r,D_CHAR,4},
	{"gridcolor3.g",&gridcolor3.g,D_CHAR,4},
	{"gridcolor3.b",&gridcolor3.b,D_CHAR,4},
	{ "funccolor.r", &funccolor.r,D_CHAR,4},
	{ "funccolor.g", &funccolor.g,D_CHAR,4},
	{ "funccolor.b", &funccolor.b,D_CHAR,4},
	{"slopecolor.r",&slopecolor.r,D_CHAR,4},
	{"slopecolor.g",&slopecolor.g,D_CHAR,4},
	{"slopecolor.b",&slopecolor.b,D_CHAR,4},
};
const int nedv = NUMELEMENTS(edv);

int cntr;

#ifdef USEPLOTTERNAMESPACE
} // namespace u_s_plotter

using namespace u_s_plotter;
#endif



void plotterinit()
{
	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_80);
	//aclass* pc = 0;
	//pc->amethod();
	cntr = 0;
	video_setupwindow(GX,GY);
	extradebvars(edv,nedv);
// read the cfg file (if exists)
	pushandsetdir("plotter");
	if (fileexist("plotter.bin")) {
		FILE* fp=fopen2("plotter.bin","rb");
		fread( &backcolor,4,1,fp);
		fread( &axiscolor,4,1,fp);
		fread(&gridcolor1,4,1,fp);
		fread(&gridcolor2,4,1,fp);
		fread(&gridcolor3,4,1,fp);
		fread( &funccolor,4,1,fp);
		fread(&slopecolor,4,1,fp);
		fclose(fp);
	}
	con=con32_alloc(600,300,C32WHITE,C32BLACK);
#ifdef POLES
	poles_init();
#endif
#ifdef MACROTEST
	macrotest();
#endif
#ifdef LINDIFF
	dolindiff();
#endif
#ifdef FIB
	dofib();
#endif
//#define LEAKTEST
#ifdef LEAKTEST
	U8* leak = new U8[100];
//	leak[-1] = 33;
//	leak[100] = 33;
//	delete[] leak;
	delete[] leak;
	leak = 0;
	//memfree(leak);
#endif
#ifdef ROTRECT
	rotrect_init();
#endif
#ifdef LINREG
	initlinreg();
#endif
#ifdef STRTOK
	strtoktest();
#endif
#ifdef SUMTEST
	dosumtest();
#endif
#ifdef CUBIC2
	docubic();
#endif
#ifdef RAT
	dorat();
#endif
#ifdef QUANT
	quantinit();
#endif
#ifdef QUANTSHO
	quantshoinit();
#endif
#ifdef WRITEBIN
	writebin();
#endif
#ifdef PROVIDER
	con32_printf(con,"sizeof contdata = %d\n",sizeof(contdata));
#endif
#ifdef PASSPORT
	u_s_plotter::dopassport();
#endif
#ifdef VISA
	u_s_plotter::dovisa();
#endif
#ifdef DOPCXHEATHOUSE
	dopcxheathouse();
#endif
#ifdef PALETTE_DPAINT2
	doPalette();
#endif
#ifdef NEEDLE
	doneedle();
#endif
#ifdef MAKESMALLFONT
	makesmallfont();
#endif
#ifdef MAKEMAPTEST
	makemaptest();
#endif
#ifdef MAKESQUID
	makesquid();
#endif
#ifdef INTERSTELLAR
	interstellar();
#endif
#ifdef MAKEPENROSE
	makepenrose();
#endif
#ifdef MILLIBEL
	millibel();
#endif
#ifdef ROTATEPIC
	rotatepic();
#endif
#ifdef ROTATEPIC2
	rotatepic2();
#endif
#ifdef ROTATEPIC3
	rotatepic3();
#endif
#ifdef CENTROID
	centroid();
#endif
#ifdef PROVIDER
	doprovider();
#endif
#ifdef TANG
#endif
#ifdef DDS2PNG
	dds2png();
#endif
#ifdef ALPHATEST
	doalphatest();
#endif
#ifdef GLYPH
	showglyph();
#endif
#ifdef GLYPH2
	showglyph2();
#endif
#ifdef GLYPH3
	showglyph3();
#endif
#ifdef GLYPH4
	showglyph4();
#endif
#ifdef GLYPH5
	showglyph5();
#endif
#ifdef GLYPH6
	showglyph6();
#endif
#ifdef SLERPTEST
	slerptest();
#endif
#ifdef CONTAINERTEST
	containertest();
#endif
#ifdef SORT2TEST
	sort2test();
#endif
#ifdef REGEX
	doregextest();
#endif
#ifdef DEPTHLUT
	do_depthlut();
#endif
#ifdef CPPTEST
	do_cpptest();
#endif
#ifdef AR_PLOT
	init_arplot();
#endif
#ifdef HOUGHTEST
	inithoughtest();
#endif
#ifdef PERS
	initpers();
#endif
}

void plotterproc()
{
	pointi2x p(MX,MY);
	plot = screen2math(p);
	if (wininfo.dmz) { // wheel mouse
		float m = wininfo.dmz > 0 ? 1.0f : -1.0f;
		float lzoomspeed = 1.0f/16.0f;
		if (MBUT & 0x10) // middle button
		//if (true)
			lzoomspeed *= 4;
		lzoom += m * lzoomspeed;
	}
	zoom=expf(lzoom);
	if (wininfo.dmz) {
		center = newcenter(p,plot);
	}
	if (wininfo.mbut) {
		float f = 1.0f / (zoom * WY/2);
		// where is the mouse in float coords
		//float fmx = ;
		//flaot fmy = ;
		center.x -= wininfo.dmx*f;
		center.y += wininfo.dmy*f;

	}
#ifdef PROVIDER
	procprovider();
#endif
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
	calcstuff();
	//con32_printf(con,"cntr = %d\n",cntr);
	++cntr;
#ifdef PROVIDER
	procprovider();
#endif
}

void plotterdraw2d()
{
	clipclear32(B32,backcolor);
	drawgrid();
	drawaxis();
	drawstuff();
#ifdef MAKEPENROSE
	drawpenrose()
#endif
	//bitmap32* cb=con32_getbitmap32(con);
	//clipblit32(cb,B32,0,0,WX-cb->size.x,WY-cb->size.y,cb->size.x,cb->size.y);
}

void plotterexit()
{
	defaultfpucontrol();
	extradebvars(0,0);
// write the cfg file
	FILE* fp=fopen2("plotter.bin","wb");
	fwrite( &backcolor,4,1,fp);
	fwrite( &axiscolor,4,1,fp);
	fwrite(&gridcolor1,4,1,fp);
	fwrite(&gridcolor2,4,1,fp);
	fwrite(&gridcolor3,4,1,fp);
	fwrite( &funccolor,4,1,fp);
	fwrite(&slopecolor,4,1,fp);
	fclose(fp);
	popdir();
	con32_free(con);
#ifdef MAKEPENROSE
	exitmakepenrose();
#endif
#ifdef PROVIDER
	exitprovider();
#endif
#ifdef AR_PLOT
	exit_arplot();
#endif
#ifdef HOUGHTEST
	exithoughtest();
#endif
#ifdef PERS
	exitpers();
#endif
#ifdef POLES
	poles_exit();
#endif
}
