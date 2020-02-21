// first test of plotter2 

// todo: this needs to be optimized, now runs O2, should be O1 !!!
#include <m_eng.h>
#include "m_perf.h"
#include <l_misclibm.h>

//#include <complex>

//#define USEFLOAT
#define USEDOUBLE
#ifdef USEFLOAT
typedef compf compT; // compT, typedef for a complex float
//#define D_COMPF D_FLOAT
#endif
#ifdef USEDOUBLE
typedef compd compT; // compT, typedef for a complex double
//#define D_COMPF D_DOUBLE
#endif

#include "u_plotter2.h"

using namespace u_plotter2;

//namespace zetap2 {

const S32 maxzetaiter = 80000;
S32 zetaiter = 20;//10000;
pointf2* terms;

int pointiterx = 0;
int pointitery = 0;
int lineratio = 4;
int lineiterx;
int lineitery;
const float linetest = 0;
//const float linetest = .01f;
float circsize = .00005f;
float fcircsize;
int csum = 0; // use Cesaro sum

//pointf2 basep = {.5f,14.134653f};//{3,0};//{2,0};//{-1.25f,0};//{2,0};
pointf2 basep = {.5f,7005.062988f};//{3,0};//{2,0};//{-1.25f,0};//{2,0};
pointf2 basesepp = {.0625f,.0625f};
pointf2 resultp;

//int useangmag = 0; // 0 use std complex, 1 use custom
double lastmag,lastang; // last
double lastmag2,lastang2; // 2nd to the last
double delmag,delang;
int zetafunction = 2; // which version of zeta to use, 0 normal to 1, 1 Euler to 0, 2 Reimann reflect
int showscalefactor = 1; // use the factor when coverting between compTunct 0 and compTunct 1
void compfunc0(const compT& s,int ziter); // regular s > 1
void compfunc1(const compT& s,int ziter); // alternating s > 0
void compfunc2(const compT& s,int ziter); // avg last 2 of compTunc1
void compfunc3(const compT& s,int ziter); // prime product formula
void (*compfunc)(const compT& s,int ziter);// = compTunc0;
void (*compfuncarr[])(const compT& s,int ziter) = {
	compfunc0,
	compfunc1,
	compfunc2,
	compfunc3,
};
const int ncompfuncarr = NUMELEMENTS(compfuncarr);

int primes[maxzetaiter];

void calcprimes()
{
	primes[0] = 0;
	int curprimeidx = 1;
	//var maxprime = 100000;
// calculate the primes
	//txt = "" + maxd;
	S32 n = 2;
	//var cnt = 0;
	//for (n=2;n<maxprime;++n) {
	while(true) {
		S32 d = 2;
		bool isprime = true;
		while(true) {
			S32 q = n / d;
			if (q < d) // done testing, it's a prime
				break;
			if (n % d == 0) {
				isprime = false;
				break;
			}
			++d;
		}
		if (isprime) {
			//txt = txt + " " + n;
			//++cnt;
			primes[curprimeidx++] = n;
			if (curprimeidx == maxzetaiter)
				break;
		}
		++n;
	}
}

// for debvars
struct menuvar zetadv[]={
	{"@lightred@--- PLOTTER2 USER VARS for zeta ---",NULL,D_VOID,0},
	{"circlesize",&circsize,D_FLOAT,FLOATUP/512},
	{"pointiterx",&pointiterx,D_INT,1},
	{"pointitery",&pointitery,D_INT,1},
	{"lineratio",&lineratio,D_INT,1},
	{"basesep.x",&basesepp.x,D_FLOAT,FLOATUP/16},
	{"basesep.y",&basesepp.y,D_FLOAT,FLOATUP/16},
	{"zetafunction",&zetafunction,D_INT,1},
	//{"useangmag",&useangmag,D_INT,1},
	{"showscalefactor",&showscalefactor,D_INT,1},
	{"base.x",&basep.x,D_FLOAT,FLOATUP/4},
	{"base.y",&basep.y,D_FLOAT,FLOATUP/1024},
	{"zetaiter",&zetaiter,D_INT,1},
	{"csum",&csum,D_INT,1},
	{"result.x",&resultp.x,D_FLOAT|D_RDONLY},
	{"result.y",&resultp.y,D_FLOAT|D_RDONLY},

	{"lastmag1",&lastmag,D_DOUBLE|D_RDONLY},
	//{"lastmag2",&lastmag2,D_DOUBLE|D_RDONLY},
	{"delmag",&delmag,D_DOUBLE|D_RDONLY},
	{"lastang1",&lastang,D_DOUBLE|D_RDONLY},
	//{"lastang2",&lastang2,D_DOUBLE|D_RDONLY},
	{"delang",&delang,D_DOUBLE|D_RDONLY},
};
const int nzetadv = NUMELEMENTS(zetadv);

// converges if > 1
void compfunc0(const compT& s,int ziter)
{
	compT sum = compT();
	compT sum1 = compT();
	terms[0] = pointf2x();
	lastang = lastmag = 0;
	lastang2 = lastmag2 = 0;
	double ang = 0,mag = 0;
	for (S32 i=1;i<=ziter;++i) {
		perf_start(ZETATERM);
		compT term;
		/*/if (useangmag) {
			compT base = compT((float)i,0);
			compT exp = compT(-s);
			term = pow(base,exp);
		} else {*/
			ang = -log((double)i)*s.imag();
			mag = pow(i,-s.real());
			term = compT(mag*cos(ang),mag*sin(ang));
			lastang2 = lastang;
			lastmag2 = lastmag;
			lastang = ang;
			lastmag = mag;
		//}
		sum += term;
		if (csum) { // Cesaro sum
			sum1 += sum;
			terms[i] = pointf2x(float(sum1.real()/i),float(sum1.imag()/i));
		} else {
			terms[i] = pointf2x(float(sum.real()),float(sum.imag()));
		}
		perf_end(ZETATERM);
	}
}

// alternating + and -
// converges if  > 0
void compfunc1(const compT& s,int ziter)
{
	compT fact = showscalefactor ? compT(1)/(compT(1) - pow(compT(2),compT(1) - s)) : 1;
	compT sum = compT();
	compT sum1 = compT();
	terms[0] = pointf2x();
	lastang = lastmag = 0;
	lastang2 = lastmag2 = 0;
	double ang = 0,mag = 0;
	for (S32 i=1;i<=ziter;++i) {
		perf_start(ZETATERM);
		compT term;
		/*/if (useangmag) {
			compT base = compT((float)i,0);
			compT exp = compT(-s);
			term = pow(base,exp);
		} else {*/
			ang = -log((double)i)*s.imag();
			mag = pow(i,-s.real());
			term = compT(mag*cos(ang),mag*sin(ang));
			lastang2 = lastang;
			lastmag2 = lastmag;
			lastang = ang;
			lastmag = mag;
		//}
		//term *= fact;
		if (i & 1)
			sum += term*fact;
		else
			sum -= term*fact;
		terms[i] = pointf2x(float(sum.real()),float(sum.imag()));
		perf_end(ZETATERM);
	}
	//return sum;
}

// alternating + and -
// converges if  > 0 avg last 2 partial sums
void compfunc2(const compT& s,int ziter)
{
	compT fact = showscalefactor ? compT(1)/(compT(1) - pow(compT(2),compT(1) - s)) : 1;
	compT sum;
	compT sum1;
	compT lastsum;
	terms[0] = pointf2x();
	lastang = lastmag = 0;
	lastang2 = lastmag2 = 0;
	double ang = 0,mag = 0;
	for (S32 i=1;i<=ziter;++i) {
		perf_start(ZETATERM);
		compT term;
		/*/if (useangmag) {
			compT base = compT((float)i,0);
			compT exp = compT(-s);
			term = pow(base,exp);
		} else {*/
			ang = -log((double)i)*s.imag();
			mag = pow(i,-s.real());
			term = compT(mag*cos(ang),mag*sin(ang));
			lastang2 = lastang;
			lastmag2 = lastmag;
			lastang = ang;
			lastmag = mag;
		//}
		//term *= fact;
		lastsum = sum;
		if (i & 1)
			sum += term*fact;
		else
			sum -= term*fact;
		compT at = (sum + lastsum) * (compT).5f;
		terms[i] = pointf2x(float(at.real()),float(at.imag()));
		perf_end(ZETATERM);
	}
	//return (sum + lastsum) * (compT).5f;
}

// converges if > 1
void compfunc3(const compT& s,int ziter)
{
	compT prod = compT(1,0);
	terms[0] = pointf2x();
	lastang = lastmag = 0;
	lastang2 = lastmag2 = 0;
	double ang = 0,mag = 0;
	for (S32 i=1;i<=ziter;++i) {
		perf_start(ZETATERM);
		compT p = primes[i];
		compT num = pow(p,s);
		compT den = num - 1.0;
		compT fact = num/den;
		prod *= fact;
		terms[i] = pointf2x(float(prod.real()),float(prod.imag()));
		perf_end(ZETATERM);
	}
}

void calccompfuncp(const pointf2& s,int mxziter)
{
	perf_start(ZETAFUNC);
	compT ac(s.x,s.y);
	/*compT rc = */compfunc(ac,mxziter);
	delang = lastang - lastang2;
	delmag = lastmag - lastmag2;
	perf_end(ZETAFUNC);
	//return pointf2x((float)rc.real(),(float)rc.imag());
}

//pointf2 compfuncp(const pointf2& s,int ziter)
pointf2 compfuncp(int ziter)
{
//	compT ac(s.x,s.y);
//	compT rc = compfunc(ac,ziter);
//	return pointf2x((float)rc.real(),(float)rc.imag());
	return terms[ziter];
}

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
	if (pointiterx == 0 && pointitery == 0)
		return;
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
		calccompfuncp(piter,zetaiter);
		pointf2 res = compfuncp(/*piter,*/zetaiter);
		pointf2 last;
		for (int i=-lineiterx+1;i<=lineiterx;++i) { // lines
			last = res;
			piter.x = center.x + i*stepline.x;
			pointf2 last2;
			calccompfuncp(piter,zetaiter);
			res = compfuncp(/*piter,*/zetaiter);
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
		calccompfuncp(piter,zetaiter);
		pointf2 res = compfuncp(/*piter,*/zetaiter);
		pointf2 last;
		for (int j=-lineitery+1;j<=lineitery;++j) { // lines
			last = res;
			piter.y = center.y + j*stepline.y;
			pointf2 last2;
			calccompfuncp(piter,zetaiter);
			res = compfuncp(/*piter,*/zetaiter);
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
			calccompfuncp(piter,zetaiter);
			pointf2 res = compfuncp(/*piter,*/zetaiter);
			drawfpoint(res,color,fcircsize);
		}
	}
}

void zeta_draw()
{
	circsize = range(.00001f,circsize,.0625f);
	fcircsize = math2screen(circsize);
	float centersize = fcircsize*2.0f;
	float itersize = fcircsize*.75f;


	drawgrid(basep,basesepp,C32RED);
	drawgridfunc(basep,basesepp,C32GREEN);

	calccompfuncp(basep,zetaiter);
	resultp = compfuncp(/*basep,*/zetaiter);

	drawfpoint(basep,C32RED,centersize);
// draw iterations of zeta function
	S32 it;
	pointf2 lastiterp = compfuncp(/*basep,*/0);
	drawfpoint(lastiterp,C32BROWN,itersize);
	for (it=1;it<=zetaiter;++it) {
		resultp = compfuncp(/*basep,*/it);
		drawfline(lastiterp,resultp,C32BROWN);
		drawfpoint(resultp,C32BROWN,itersize);
		lastiterp = resultp;
	}
	drawfpoint(resultp,C32BLUE,centersize);
}

void floattest()
{
	double d = 1<<30;
	double inc = 1.0;
	while(inc < 1e20) {
		double dinc = d + inc;
		string s = dinc == d ? "==" : "!=";
		logger("d = %f, inc = %f, dinc = %f, ' %s '\n",d,inc,dinc,s.c_str());
		inc *= 2;
	}
}

//} // end namespace zetap2

//using namespace zetap2;

void plot2zetainit()
{
	adddebvars("zeta",zetadv,nzetadv);
	plotter2init();
	logger("sizeof char = %d\n",sizeof(char));
	logger("sizeof short = %d\n",sizeof(short));
	logger("sizeof int = %d\n",sizeof(int));
	logger("sizeof long = %d\n",sizeof(long));
	logger("sizeof long long = %d\n",sizeof(long long));
	logger("sizeof float = %d\n",sizeof(float));
	logger("sizeof double = %d\n",sizeof(double));
	logger("sizeof long double = %d\n",sizeof(long double));
	terms = new pointf2[maxzetaiter];
	//floattest();
	calcprimes();
}

void plot2zetaproc()
{
	// limit range of debprint
	pointiterx = range(0,pointiterx,20);
	pointitery = range(0,pointitery,20);
	lineratio = range(0,lineratio,10);
	lineiterx = lineratio*pointiterx;
	lineitery = lineratio*pointitery;
	showscalefactor = showscalefactor&1;
	if (zetaiter < 1)
		zetaiter = 1;
	if (zetaiter >= maxzetaiter) // one less for array
		zetaiter = maxzetaiter - 1;
	if (zetafunction < 0)
		zetafunction = ncompfuncarr - 1;
	else if (zetafunction >= ncompfuncarr)
		zetafunction = 0;
	compfunc = compfuncarr[zetafunction];
	// interact with graph paper
	plotter2proc();
	// calc zeta
}

void plot2zetadraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw zeta
	zeta_draw();
}

void plot2zetaexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("zeta");
	delete[] terms;
}
