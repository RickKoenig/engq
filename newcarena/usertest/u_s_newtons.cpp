// plotter system

#include <m_eng.h>
#include "system/u_states.h"
//#include "rungekutta.h"
#include "u_groups.h"

namespace u_s_newtons {

// console
con32* con;
// geom
float lzoom=logf(.47f); // yes, zoom
float zoom;
pointf2 center={0,0};
// system colors
C32  backcolor;
C32  axiscolor;
C32 gridcolor1;
C32 gridcolor2;
C32 gridcolor3;
// user colors
C32  funccolor;
C32 slopecolor;
S32 nsteps=1000;
float eps1 = .000001f;
float eps2 = .001f;

pointi2 math2screen(const pointf2& p)
{
	pointi2 r;
	r.x=static_cast<int>(WX/2 + zoom * WY/2 * (p.x-center.x));
	r.y=static_cast<int>(WY/2 - zoom * WY/2 * (p.y-center.y)); // '-' is from math to screen (flip)
	return r;
}

pointf2 screen2math(const pointi2& p)
{
	pointf2 r;
	r.x = (p.x-WX/2)/(zoom*WY/2) + center.x;
	r.y = -(p.y-WY/2)/(zoom*WY/2) + center.y;
	return r;
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

void drawtext(const pointf2& c,C32 clrf,C32 clrb,const C8* str)
{
	pointi2 ci=math2screen(c);
//	clipputpixel32(B32,ci.x,ci.y,clr);
	outtextxybf32(B32,ci.x,ci.y,clrf,clrb,"%s",str);
}

void drawfpoint(const pointf2& c,C32 clr)
{
	pointi2 ci=math2screen(c);
//	clipputpixel32(B32,ci.x,ci.y,clr);
	clipcircle32(B32,ci.x,ci.y,(S32)7,clr);
}

void drawfcircle(const pointf2& c,C32 clr,S32 r)
{
	pointi2 ci=math2screen(c);
	clipcircle32(B32,ci.x,ci.y,r,clr);
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
	clipline32(B32,ai.x,ai.y,bi.x,bi.y,c);
}


void drawflinec(const pointf2& a,const pointf2& b,C32 c)
{
	pointi2 ai=math2screen(a),bi=math2screen(b);
	clipline32(B32,ai.x,ai.y,bi.x,bi.y,c);
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

void drawaxis()
{
	pointf2 a,b;
	a.x=0;
	a.y=-20;
	b.x=0;
	b.y=20;
	drawfline(a,b,axiscolor);
	a.x=-20;
	a.y=0;
	b.x=20;
	b.y=0;
	drawfline(a,b,axiscolor);
}

void drawgrid()
{
	for (int i=-80;i<=80;++i) {
		pointf2 a,b;
		a.x=static_cast<float>(i)/4;
		a.y=-20;
		b.x=static_cast<float>(i)/4;
		b.y=20;
		drawfline(a,b,gridcolor1);
	}
	for (int j=-80;j<=80;++j) {
		pointf2 a,b;
		a.x=-20;
		a.y=static_cast<float>(j)/4;
		b.x=20;
		b.y=static_cast<float>(j)/4;
		drawfline(a,b,gridcolor1);
	}
	for (int i=-20;i<=20;++i) {
		pointf2 a,b;
		a.x=static_cast<float>(i);
		a.y=-20;
		b.x=static_cast<float>(i);
		b.y=20;
		drawfline(a,b,gridcolor2);
	}
	for (int j=-20;j<=20;++j) {
		pointf2 a,b;
		a.x=-20;
		a.y=static_cast<float>(j);
		b.x=20;
		b.y=static_cast<float>(j);
		drawfline(a,b,gridcolor2);
	}
	for (int i=-2;i<=2;++i) {
		pointf2 a,b;
		a.x=static_cast<float>(10*i);
		a.y=-20;
		b.x=static_cast<float>(10*i);
		b.y=20;
		drawfline(a,b,gridcolor3);
	}
	for (int j=-2;j<=2;++j) {
		pointf2 a,b;
		a.x=-20;
		a.y=static_cast<float>(10*j);
		b.x=20;
		b.y=static_cast<float>(10*j);
		drawfline(a,b,gridcolor3);
	}
}

// end grid system

// y = f(x) drawer
template <class Functor>
void drawfunction(Functor f)
//void drawfunction(float (*f)(float))
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
		if (fabs(c.y-p.y)<100)
			drawfline(p,c,funccolor);
		p=c;
	}
}
// end y = f(x) drawer

// x,y = fxy(t) parametric drawer
template <class Functor>
void drawfunction2(Functor f)
//void drawfunction2(pointf2 (*f)(float t))
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

// user functions 1
float sinfun(float a)
{
	return sinf(a);
}

float cosfun(float a)
{
	return cosf(a);
}

float tanfun(float a)
{
	return tanf(a);
}

float expe(float a)
{
	return expf(a);
}

float exp10(float a)
{
	return powf(10.0f,a);
}

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

float tant(float x)
{
	float ret;
	ret=-2+4*tanf(PI/4-x);
	return ret;
}

// user functions 2

pointf2 circ(float t)
{
	pointf2 r;
	t*=TWOPI;
	r.x=cosf(t);
	r.y=sinf(t);
	return r;
}

class circr {
	float rad;
public:
	circr(float rada) : rad(rada) {}
	pointf2 operator() (float t)
	{
		pointf2 r;
		t*=TWOPI*.25f;
		r.x=rad*cosf(t);
		r.y=rad*sinf(t);
		return r;
	}
};

template <class Functor>
//void testfunctor(pointf2 (*f)(float t))
void testfunctor(Functor f)
{
	float t;
	for (t=0;t<=1.0f;t+=.125) {
		pointf2 p = f(t);
		logger("t %f, px %f, py %f\n",t,p.x,p.y);
	}
}

#if 1

float c5 = 0;
float c4 = 0;
float c3 = 1;
float c2 = 0;
float c1 = 0;
float c0 = -1;
pointf2 p0 = pointf2x(2,2);
float m = 1;
float cont = 1000;
bool baddiv;
bitmap32* bg;
S32 bgon = 2;
const S32 MAXROOTS = 1000;
pointf2 roots[MAXROOTS];
S32 nroots;

S32 ny;

pointf2 cadd(const pointf2& a,const pointf2& b)
{
	return pointf2x(a.x+b.x,a.y+b.y);
}

pointf2 csub(const pointf2& a,const pointf2& b)
{
	return pointf2x(a.x-b.x,a.y-b.y);
}

pointf2 ctimes(const pointf2& a,float b)
{
	return pointf2x(a.x*b,a.y*b);
}

pointf2 ctimes(const pointf2& a,const pointf2& b)
{
	return pointf2x(a.x*b.x-a.y*b.y,a.y*b.x+a.x*b.y);
}

pointf2 cconj(const pointf2& a)
{
	return pointf2x(a.x,-a.y);
}

float cabs(const pointf2& a)
{
	const float eps = .00000000001f;
	float ret = a.x*a.x + a.y*a.y;
	if (ret < eps) {
		baddiv = true;
		ret = eps;
	} else 
		baddiv = false;
	return ret;
}

pointf2 cdivide(const pointf2& a,const pointf2& b)
{
	return ctimes(ctimes(a,cconj(b)),1.0f/cabs(b));
}

pointf2 cpow(const pointf2& a,S32 p)
{
	if (p==0)
		return pointf2x(1,0);
	pointf2 r = a;
	while(--p>0)
		r = ctimes(r,a);
	return r;
}

pointf2 newtfun(const pointf2& x)
{
	pointf2 c5x5 = ctimes(cpow(x,5),c5);
	pointf2 c4x4 = ctimes(cpow(x,4),c4);
	pointf2 c3x3 = ctimes(cpow(x,3),c3);
	pointf2 c2x2 = ctimes(cpow(x,2),c2);
	pointf2 c1x = ctimes(x,c1);
	return cadd(cadd(cadd(cadd(cadd(c5x5,c4x4),c3x3),c2x2),c1x),pointf2x(c0));
}

pointf2 newtfund(const pointf2& x)
{
	pointf2 c55x4 = ctimes(ctimes(cpow(x,4),c5),5);
	pointf2 c44x3 = ctimes(ctimes(cpow(x,3),c4),4);
	pointf2 c33x2 = ctimes(ctimes(cpow(x,2),c3),3);
	pointf2 c22x = ctimes(ctimes(x,c2),2);
	return cadd(cadd(cadd(cadd(c55x4,c44x3),c33x2),c22x),pointf2x(c1));
}

pointf2 newtstep(const pointf2& p)
{
	pointf2 r = csub(p, ctimes(cdivide(newtfun(p),newtfund(p)),m));
	return r;
}

void drawnewtons()
{
	S32 i = 0;
	pointf2 p = p0;
	for (i=0;i<nsteps;++i) {
		pointf2 np = newtstep(p);
		if (baddiv)
			break;
		drawfline(p,np,funccolor);
		p = np;
	}
}

void testnewtons()
{
	pointf2 a = pointf2x(3,4);
	pointf2 b = pointf2x(5,6);
	pointf2 c = cadd(a,b);
	logger("add %f %f\n",c.x,c.y);
	c = csub(a,b);
	logger("sub %f %f\n",c.x,c.y);
	c = ctimes(a,b);
	logger("times %f %f\n",c.x,c.y);
	c = cdivide(a,b);
	logger("divide %f %f\n",c.x,c.y);
	c = cpow(a,0);
	logger("p0 %f %f\n",c.x,c.y);
	c = cpow(a,1);
	logger("p1 %f %f\n",c.x,c.y);
	c = cpow(a,2);
	logger("p2 %f %f\n",c.x,c.y);
	c = cpow(a,3);
	logger("p3 %f %f\n",c.x,c.y);
}

S32 getrootidx(const pointf2& r)
{
	S32 i;
	for (i=0;i<nroots;++i) {
		float d = cabs(csub(roots[i],r));
		if (d<eps2)
			return i;
	}
	if (nroots == MAXROOTS)
		return 0;
//		errorexit("too many roots!");
	roots[i] = r;
	++nroots;
	return i;
}

#endif

void drawbgnewtons(S32 j)
{
	S32 i;
	for (i=0;i<bg->size.x;++i) {
		pointi2 ps = pointi2x(i,j);
		pointf2 p0 = screen2math(ps);
		S32 k = 0;
		pointf2 p = p0;
		for (k=0;k<nsteps;++k) {
			pointf2 np = newtstep(p);
			if (baddiv) {
				clipputpixel32(bg,i,j,C32YELLOW);
				break;
			}
			float d = cabs(newtfun(np));
			if (d<eps1) { // converged on a root
				S32 ri = getrootidx(np);
				C32 c;
				S32 b;
				if (k&1)
					b = (S32)(230*(1.0f - .75f*k/nsteps));
				else
					b = (S32)(255*(1.0f - .75f*k/nsteps));
				switch(ri) {
					case 0:
						c = C32(b,0,0);
						break;
					case 1:
						c = C32(0,b,0);
						break;
					case 2:
						c = C32(b/3,b/3,b);
						break;
					case 3:
						c = C32(b,b,0);
						break;
					case 4:
						c = C32(0,b,b);
						break;
					case 5:
						c = C32(b,0,b);
						break;
					default:
						c = C32(b,b,b);
						break;
				}
/*				if (p.x>0) {
					c = C32(b,0,0);
				} else if (p.y>0) {
					c = C32(0,b,0);
				} else {
					c = C32(0,0,b);
				} */
				clipputpixel32(bg,i,j,c);
				break;
			}
			p = np;
		}
		if (k==nsteps) {
			clipputpixel32(bg,i,j,C32CYAN);
		}
	}
}

void drawbgroots(S32 j)
{
	S32 i;
	for (i=0;i<bg->size.x;++i) {
		pointi2 ps = pointi2x(i,j);
		pointf2 p0 = screen2math(ps);
		float d = cabs(newtfun(p0));
		const float eps = .001f;
		if (d<eps) // converged on a root
			getrootidx(p0);
		C32 c;
		float g = 1.0f-d*cont;
		c.r=c.g=c.b=range(0,(S32)(g*255),255);
		clipputpixel32(bg,i,j,c);
	}
}

void drawstuff()
{
	if (bgon) {
		S32 i;
		if (ny==0) {
			nroots = 0;
			fill(roots,roots+MAXROOTS,pointf2x(0,0));
		}
		S32 step;
		if (bgon==1)
			step = 5; //100;
		else
			step = bg->size.y;
		if (ny<bg->size.y) {
			for (i=0;i<step;++i) {
				if (bgon==1)
					drawbgnewtons(ny);
				else
					drawbgroots(ny);
				++ny;
				if (ny==bg->size.y)
					break;
			}
		}
	} else
		drawnewtons();
//	drawhypercube();
//	drawproj();
//	drawlines();
//	drawfunction(sinfun);
//	drawfunction(cosfun);
//	drawfunction(tanfun);
//	drawfunction(acosfun);
//	drawfunction(asinfun);
//	drawfunction(atanfun);
//	drawfunction(expe);
//	drawfunction(exp10);
//	drawfunction(exp10le);
//	drawfunction(root3_0);
//	drawfunction(root3_1);
//	drawfunction(root3_2);
//	drawfunction(xs);
//	drawfunction(beercan);
//	drawfunction(quintic);
//	drawfunction(seventh);
//	drawfunction(sinf);
//	drawfunction2(circ);
//	drawfunctionrk2d(objstate,offsets,2,initf,changef);
//	drawfunction(tant);
//	drawmaze();
}
// end user section

struct menuvar edv[]={
// user vars
// newtons user vars
	{"@yellow@---- Newton's Method USER VARS -----------------",NULL,D_VOID,0},
	{"c5",&c5,D_FLOAT,FLOATUP},
	{"c4",&c4,D_FLOAT,FLOATUP},
	{"c3",&c3,D_FLOAT,FLOATUP},
	{"c2",&c2,D_FLOAT,FLOATUP},
	{"c1",&c1,D_FLOAT,FLOATUP},
	{"c0",&c0,D_FLOAT,FLOATUP},
	{"p0r",&p0.x,D_FLOAT,FLOATUP/16},
	{"p0i",&p0.y,D_FLOAT,FLOATUP/16},
	{"mult",&m,D_FLOAT,FLOATUP/16},
	{"cont",&cont,D_FLOAT,FLOATUP*64},
	{"bgon",&bgon,D_INT},
	{"eps1",&eps1,D_FLOAT,FLOATUP/256},
	{"eps2",&eps2,D_FLOAT,FLOATUP/256},
	{"nroots",&nroots,D_INT|D_RDONLY},
	{"roots0r",&roots[0].x,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots0i",&roots[0].y,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots1r",&roots[1].x,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots1i",&roots[1].y,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots2r",&roots[2].x,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots2i",&roots[2].y,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots3r",&roots[3].x,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots3i",&roots[3].y,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots4r",&roots[4].x,D_FLOAT|D_RDONLY,FLOATUP},
	{"roots4i",&roots[4].y,D_FLOAT|D_RDONLY,FLOATUP},
// plotter system vars
	{"@lightcyan@---- PLOTTER SYSTEM VARS -----------------",NULL,D_VOID,0},
	{"nsteps",&nsteps,D_INT,1},
	{"lzoom",&lzoom,D_FLOAT,FLOATUP/16},
	{"zoom",&zoom,D_FLOAT|D_RDONLY,FLOATUP},
	{"centerx",&center.x,D_FLOAT,FLOATUP/16},
	{"centery",&center.y,D_FLOAT,FLOATUP/16},
//	{"nroots",&zoom,D_FLOAT|D_RDONLY,FLOATUP},
	{"@lightmagenta@---- PLOTTER SYSTEM COLORS -----------------",NULL,D_VOID,0},
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
const int nedv=sizeof(edv)/sizeof(edv[0]);

} // end namespace u_s_newtons

using namespace u_s_newtons;

void newtonsinit()
{
	bg = bitmap32alloc(GX,GY,C32BLACK);
//	testgroups();
	testnewtons();
	video_setupwindow(GX,GY);
	extradebvars(edv,nedv);
// read the cfg file (if exists)
	pushandsetdir("newtons");
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
	con=con32_alloc(400,100,C32WHITE,C32BLACK);
}

void newtonsproc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 'n':
		if (bgon==2)
			bgon=0;
		else
			++bgon;
		break;
	}
	if (KEY) {
		ny=0;
	}
//	calcstuff();
}

void newtonsdraw2d()
{
	clipclear32(B32,backcolor);
	zoom=expf(lzoom);
	drawgrid();
	drawaxis();
//	bitmap32* cb=con32_getbitmap32(con);
//	clipblit32(cb,B32,0,0,WX-cb->size.x,WY-cb->size.y,cb->size.x,cb->size.y);
	drawstuff();
	if (bgon)
		clipblit32(bg,B32,0,0,0,0,bg->size.x,bg->size.y);
}

void newtonsexit()
{
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
	bitmap32free(bg);
}
