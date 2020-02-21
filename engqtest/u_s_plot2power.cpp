// first test of plotter2 
#include <m_eng.h>
#include <l_misclibm.h>

//#include <complex>

#include "u_plotter2.h"

using namespace u_plotter2;

namespace powerp2 {

//#define DOZETA
#define DOPOWER

pointf2 base = {2,1};//{3,0};//{2,0};//{-1.25f,0};//{2,0};
pointf2 basesep = {.0625f,.0625f};
#ifdef DOPOWER
pointf2 exponent = {2.0f,0};
#endif
#ifdef DOZETA
S32 zetaiter = 10;
#endif
pointf2 result;
int pointiterx = 1;
int pointitery = 1;
int lineratio = 4;
int lineiterx;
int lineitery;
const float linetest = 0;
//const float linetest = .01f;
float circsize = .01f;
float fcircsize;
int csum = 0; // use Cesaro sum

// for debvars
struct menuvar powerdv[]={
	{"@lightred@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
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
};
const int npowerdv = NUMELEMENTS(powerdv);

pointf2 complexSum(const pointf2& a,const pointf2& b)
{
	pointf2 ret;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	return ret;
}

pointf2 complexMultiply(const pointf2& a,const pointf2& b)
{
	pointf2 ret;
	ret.x = a.x*b.x - a.y*b.y;
	ret.y = a.x*b.y + a.y*b.x;
	return ret;
}

// return e^(it)
pointf2 eiPow(float t)
{
	pointf2 ret;
	ret.x = cosf(t);
	ret.y = sinf(t);
	return ret;
}

// (a + bi) ^ (c + di) , everything, complex base to complex exponent
pointf2 complexPowc(const pointf2& b,const pointf2& e)
{
	pointf2 ret = {0,0};
	// result =  b.x+b.y ^  e.x+e.y
	// result = (a + bi) ^ (c + di)
	// result = (a + bi) ^ c   *   (a + bi) ^ (di)
	// r * e ^ (it) = a + bi
	// r = a*a + b*b
	// t = atan2(b,a)
	float r = sqrtf(b.x*b.x + b.y*b.y);
	if (r < EPSILON)
		return ret;
	float t = atan2f(b.y,b.x);
	// result = (r * (e ^ (it))) ^ c   *   (r * (e ^ (it))) ^ (di)
	// restult = r^c * e^(itc)   *   r^(di) * e^(-td)
	// restult = r^c * e^(itc)   *   e^(-td) * r^(di)
	// restult = r^c * e^(itc)   *   e^(-td) * e^(ln(r)*di)
	// result = e^((ln(r)*c) * e^(itc)   *   e^(-td) * e^(ln(r)*di)
	// result = e^((ln(r)*c + -td) * e^(i(tc + ln(r)*d))
	//float rc = powf(r,e.x);
	pointf2 eitcrdi = eiPow(t*e.x + logf(r)*e.y);
	//float etd = expf(-t*e.y);
	float rctd = expf(logf(r)*e.x - t*e.y);
	ret.x = rctd*eitcrdi.x;
	ret.y = rctd*eitcrdi.y;
	return ret;
}

// (a + bi) ^ (c + di) , everything, complex base to complex exponent, using Complex library
pointf2 complexPowd(const pointf2& b,const pointf2& e)
{
	std::complex<float> Cb(b.x,b.y);
	std::complex<float> Ce(e.x,e.y);
	std::complex<float> Cret = std::pow(Cb,Ce);
	pointf2 ret = {Cret.real(),Cret.imag()};
	return ret;
}

pointf2 cpow(const pointf2& b,const pointf2& e)
{
	return complexPowc(b,e);
	//return complexPowd(b,e);
}

#ifdef DOPOWER
pointf2 compfunc(const pointf2& a,const pointf2& b)
{
	//return complexSum(a,b);
	//return complexMultiply(a,b);
	return cpow(a,b);
}
#endif

#ifdef DOZETA
pointf2 compfunc(const pointf2& a,int ziter)
{
	pointf2 sum = {0,0};
	pointf2 sum1 = {0,0};
	for (S32 i=1;i<=ziter;++i) {
		pointf2 base;
		base.x = (float)i;
		base.y = 0;
		pointf2 exp;
		exp.x = -a.x;
		exp.y = -a.y;
		pointf2 term = cpow(base,exp);
		sum.x += term.x;
		sum.y += term.y;
		if (csum) {
			sum1.x += sum.x;
			sum1.y += sum.y;
		}
	}
	if (csum) {
		sum1.x /= ziter;
		sum1.y /= ziter;
		return sum1;
	} else {
		return sum;
	}
}
#endif

// draw a grid of points and lines
void drawgrid(const pointf2& center,const pointf2& sep,C32 color)
{
	pointf2 steppoint;
	pointf2 stepline;
	if (pointiterx == 0)
		steppoint.x = 0;
	else
		steppoint.x = sep.x/pointiterx;
	if (pointitery == 0)
		steppoint.y = 0;
		steppoint.y = sep.y/pointitery;
	if (lineiterx == 0)
		stepline.x = 0;
	else
		stepline.x = sep.x/lineiterx;
	if (lineitery == 0)
		stepline.y = 0;
	else
		stepline.y = sep.y/lineitery;

	// draw lots of lines in x
	for (int j=-pointitery;j<=pointitery;++j) { // points
		pointf2 piter;
		piter.x = center.x + -lineiterx*stepline.x;
		piter.y = center.y + j*steppoint.y;
		pointf2 last;
		for (int i=-lineiterx+1;i<=lineiterx;++i) { // lines
			last = piter;
			piter.x = center.x + i*stepline.x;
			pointf2 last2;
			last2.x = last.x;
			last2.y = last.y + linetest;
			drawfline(last2,piter,C32BLACK);
		}
	}

	// draw lots of lines in y
	for (int i=-pointiterx;i<=pointiterx;++i) { // points
		pointf2 piter;
		piter.x = center.x + i*steppoint.x;
		piter.y = center.y + -lineitery*stepline.y;
		pointf2 last;
		for (int j=-lineitery+1;j<=lineitery;++j) { // lines
			last = piter;
			piter.y = center.y + j*stepline.y;
			pointf2 last2;
			last2.x = last.x + linetest;
			last2.y = last.y;
			drawfline(last2,piter,C32BLACK);
		}
	}

	// draw some points 2D
	for (int j=-pointitery;j<=pointitery;++j) { // points
		pointf2 piter;
		piter.y = center.y + j*steppoint.y;
		for (int i=-pointiterx;i<=pointiterx;++i) { // points
			piter.x = center.x + i*steppoint.x;
			drawfpoint(piter,color,fcircsize);
		}
	}
}

// draw a grid of points and lines
void drawgridfunc(const pointf2& center,const pointf2& sep,C32 color)
{
	pointf2 steppoint;
	pointf2 stepline;
	if (pointiterx == 0)
		steppoint.x = 0;
	else
		steppoint.x = sep.x/pointiterx;
	if (pointitery == 0)
		steppoint.y = 0;
	else
		steppoint.y = sep.y/pointitery;
	if (lineiterx == 0)
		stepline.x = 0;
	else
		stepline.x = sep.x/lineiterx;
	if (lineitery == 0)
		stepline.y = 0;
	else
		stepline.y = sep.y/lineitery;

	// draw lots of lines in x
	for (int j=-pointitery;j<=pointitery;++j) { // points
		pointf2 piter;
		piter.x = center.x + -lineiterx*stepline.x;
		piter.y = center.y + j*steppoint.y;
#ifdef DOPOWER
		pointf2 res = compfunc(piter,exponent);
#endif
#ifdef DOZETA
		pointf2 res = compfunc(piter,zetaiter);
#endif
		pointf2 last;
		for (int i=-lineiterx+1;i<=lineiterx;++i) { // lines
			last = res;
			piter.x = center.x + i*stepline.x;
			pointf2 last2;
#ifdef DOPOWER
			res = compfunc(piter,exponent);
#endif
#ifdef DOZETA
			res = compfunc(piter,zetaiter);
#endif
			last2.x = last.x;
			last2.y = last.y + linetest;
			drawfline(last2,res,C32BLACK);
		}
	}

	// draw lots of lines in y
	for (int i=-pointiterx;i<=pointiterx;++i) { // points
		pointf2 piter;
		piter.x = center.x + i*steppoint.x;
		piter.y = center.y + -lineitery*stepline.y;
#ifdef DOPOWER
		pointf2 res = compfunc(piter,exponent);
#endif
#ifdef DOZETA
		pointf2 res = compfunc(piter,zetaiter);
#endif
		pointf2 last;
		for (int j=-lineitery+1;j<=lineitery;++j) { // lines
			last = res;
			piter.y = center.y + j*stepline.y;
			pointf2 last2;
#ifdef DOPOWER
			res = compfunc(piter,exponent);
#endif
#ifdef DOZETA
		res = compfunc(piter,zetaiter);
#endif
			last2.x = last.x + linetest;
			last2.y = last.y;
			drawfline(last2,res,C32BLACK);
		}
	}

	// draw some points 2D
	for (int j=-pointitery;j<=pointitery;++j) { // points
		pointf2 piter;
		piter.y = center.y + j*steppoint.y;
		for (int i=-pointiterx;i<=pointiterx;++i) { // points
			piter.x = center.x + i*steppoint.x;
#ifdef DOPOWER
			pointf2 res = compfunc(piter,exponent);
#endif
#ifdef DOZETA
			pointf2 res = compfunc(piter,zetaiter);
#endif
			drawfpoint(res,color,fcircsize);
		}
	}
}

void power_draw()
{
	circsize = range(.0001f,circsize,.0625f);
	fcircsize = math2screen(circsize);
	float centersize = fcircsize*2.0f;
	float itersize = fcircsize*.75f;
	pointiterx = range(0,pointiterx,20);
	pointitery = range(0,pointitery,20);
	lineratio = range(0,lineratio,10);
	lineiterx = lineratio*pointiterx;
	lineitery = lineratio*pointitery;


	drawgrid(base,basesep,C32RED);
	drawgridfunc(base,basesep,C32BLUE);

#ifdef DOPOWER
	result = compfunc(base,exponent);
#endif
#ifdef DOZETA
	if (zetaiter < 0)
		zetaiter = 0;
	if (zetaiter > 10000)
		zetaiter = 10000;
	result = compfunc(base,zetaiter);
#endif

	drawfpoint(base,C32RED,centersize);
#ifdef DOPOWER
	drawfpoint(exponent,C32GREEN,centersize);
#endif
	drawfpoint(result,C32BLUE,centersize);
// draw iterations of zeta function
#ifdef DOZETA
	S32 it;
	pointf2 lastiter = compfunc(base,0);
	drawfpoint(lastiter,C32BROWN,itersize);
	for (it=1;it<=zetaiter;++it) {
		result = compfunc(base,it);
		drawfline(lastiter,result,C32BROWN);
		drawfpoint(result,C32BROWN,itersize);
		lastiter = result;
	}
#endif
}

} // end namespace powerp2

using namespace powerp2;

void plot2powerinit()
{
	adddebvars("power",powerdv,npowerdv);
	plotter2init();
}

void plot2powerproc()
{
	// interact with graph paper
	plotter2proc();
	// calc power
}

void plot2powerdraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw power
	power_draw();
}

void plot2powerexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("power");
}
