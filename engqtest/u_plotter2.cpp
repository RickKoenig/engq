// plotter system
// search for includes, to turn on and off various switches / includes

#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"
#include "rungekutta.h"

//#include <complex>

#include "u_plotter2.h"

namespace u_plotter2 {

// geom
//float zoom = .075f;//1.0f; //.510f;//.21f;//.15f;//.25f;
float zoom = .5f;
float lzoom = logf(zoom); // yes, zoom
pointf2 center = {-.5, 0};// {.25f, .25f};//{2,5}
//pointf2 center={.5f,0};
// system colors
C32  backcolor;
C32  axiscolor;
C32 gridcolor1;
C32 gridcolor2;
C32 gridcolor3;
// user colors
C32 funccolor;
C32 slopecolor;
S32 nsteps=500;
pointf2 plot; // location of mouse in plot coords

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

float screen2math(float i)
{
	return i/(zoom*WY/2);
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

void drawfpoint(const pointf2& c,C32 clr,float rad)
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

void drawfcirclef(const pointf2& c,C32 clr,float r)
{
	pointi2 ci=math2screen(c);
	S32 ri = (S32)math2screen(r);
	drawfpoint(c,clr,3);
	clipcircleo32(B32,ci.x,ci.y,ri,clr);
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
void drawfunctionrange(float (*f)(float),float start,float end,C32 col)
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
			drawfline(p,c,col);
		p=c;
	}
}

// y = f[n](x) drawer
void drawfunctionrange_n(float (*f)(float,S32),float start,float end,S32 n, C32 col)
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
			drawfline(p,c,col);
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


con32* con;

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

struct menuvar edv[]={
// for reference, frame rate
	{"@red@---- wininfo timer --",0,D_VOID,0},
	{"fpswanted",&wininfo.fpswanted,D_INT,1},
	{"fpscurrent",&wininfo.fpscurrent,D_FLOAT|D_RDONLY,FLOATUP},
	{"fpsavg",&wininfo.fpsavg,D_FLOAT|D_RDONLY},
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

void plotter2init()
{
	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_64);
	video_setupwindow(GX,GY);
	adddebvars("plotter2",edv,nedv,false);
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
}

void plotter2proc()
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
	if (wininfo.dmz) { // zoom where the mouse is
		center = newcenter(p,plot);
	}
	if (wininfo.mbut) {
		float f = 1.0f / (zoom * WY/2);
		// where is the mouse in float coords
		center.x -= wininfo.dmx*f;
		center.y += wininfo.dmy*f;

	}
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
	//con32_printf(con,"cntr = %d\n",cntr);
}

void plotter2draw2d()
{
	clipclear32(B32,backcolor);
	drawgrid();
	drawaxis();
}

void plotter2exit()
{
	defaultfpucontrol();
	removedebvars("plotter2");
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
}

} // namespace u_s_plotter
