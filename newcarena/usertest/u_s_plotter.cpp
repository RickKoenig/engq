// plotter system

#include <m_eng.h>
#include "system/u_states.h"
//#include "rungekutta.h"
#include "u_groups.h"

namespace u_s_plotter {

// console
con32* con;
// geom
float lzoom=logf(.37f); // yes, zoom
float zoom;
pointf2 center={3,0};
// system colors
C32  backcolor;
C32  axiscolor;
C32 gridcolor1;
C32 gridcolor2;
C32 gridcolor3;
// user colors
C32  funccolor;
C32 slopecolor;
S32 nsteps=100;

pointi2 math2screen(const pointf2& p)
{
	pointi2 r;
	r.x=static_cast<int>(WX/2 + zoom * WY/2 * (p.x-center.x));
	r.y=static_cast<int>(WY/2 - zoom * WY/2 * (p.y-center.y)); // '-' is from math to screen (flip)
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
float maxdiff = 10000.0f;
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
		c.y=f(c.x);
		if (fabs(c.y-p.y)<maxdiff)
			drawfline(p,c,funccolor);
		p=c;
	}
}
// end y = f(x) drawer

// y = f(x) drawer, with bounds
template <class Functor>
void drawfunctionb(Functor f,float x0,float x1)
//void drawfunction(float (*f)(float))
{
	S32 i;
	pointf2 p;
	p.x = x0;
	p.y = f(x0);
	for (i=1;i<=nsteps;++i) {
		pointf2 c;
		c.x=(x1-x0)*i/nsteps+x0;
		c.y=f(c.x);
		if (fabs(c.y-p.y)<maxdiff)
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

/*static bool findt(float x0,float x1,float xt,float* t) // 0 to 1 if intsect, 0 closest
{
	if (x0>=xt && x1<xt) { // went from good to bad
		*t = (xt-x0)/(x1-x0);
		return true;
	} else {
		return false;
	}
}*/

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

#if 0
	class circr inrad = circr(PIECESIZE*.25f);
	class circr outrad = circr(PIECESIZE*.75f);
//	drawfunction2(circ);
	drawfunction2(inrad);
	drawfunction2(outrad);
//	testfunctor(circ);
//	testfunctor(inrad);
//	testfunctor(outrad);
#endif

#if 0
// calc RDS slopes and rotations
float theta=-45,phi=60;
float x2slope=0;
void drawproj()
{
	pointf3x wx(1,0,0);
	pointf3x wy(0,1,0);
	pointf3x wz(0,0,1);
	pointf3x wx2(1,x2slope,0);
	mat4 mattheta;
	mat4 matphi;
	pointf3x rottheta(0,theta*PIOVER180,0);
	pointf3x rotphi(phi*PIOVER180,0,0);
	buildrot3d(&rottheta,&mattheta);
	buildrot3d(&rotphi,&matphi);
	xformvec(&mattheta,&wx,&wx);
	xformvec(&mattheta,&wy,&wy);
	xformvec(&mattheta,&wz,&wz);
	xformvec(&mattheta,&wx2,&wx2);
	xformvec(&matphi,&wx,&wx);
	xformvec(&matphi,&wy,&wy);
	xformvec(&matphi,&wz,&wz);
	xformvec(&matphi,&wx2,&wx2);
	pointf2x a(wx.x,wx.z);
	pointf2x b(wy.x,wy.z);
	pointf2x c(wz.x,wz.z);
	pointf2x a2(wx2.x,wx2.z);
	pointf2x z;
	drawflinec(z,a,C32RED);
	drawflinec(z,b,C32GREEN);
	drawflinec(z,c,C32BLUE);
	drawflinec(a,a2,C32RED);
}
#endif

#if 0
const S32 nlinepnts = 7;
S32 linepnts[nlinepnts]={0,1,3,4,3,1,0};
float xwid = .125;
float yhit = .25;
pointf2 tp={.9f,-.1f}; // testpoint
pointf2 np={.93f,-.13f}; // newpoint

static bool checkline(const pointf2* p0,const pointf2* p1,const pointf2* pt,pointf2* pn)
{
	pointf2x tmp0(pt->x-p0->x,pt->y-p0->y);
	pointf2x p1mp0(p1->x-p0->x,p1->y-p0->y);
	float k=dot2d(&tmp0,&p1mp0)/dot2d(&p1mp0,&p1mp0);
	if (k>=0 && k<=1.0f) {
		float omk = 1.0f - k;
		pn->x = omk*p0->x + k*p1->x;
		pn->y = omk*p0->y + k*p1->y;
		return true;
	}
	return false;

}

static bool pnt2line(const S32* ipnts,S32 npnts,const pointf2* opnt,pointf2* npnt)
{
	pointf2x bestpnt(0,ipnts[0]*yhit);
	float bestdistsq = dist2dsq(&bestpnt,opnt);
	S32 i;
	for (i=1;i<npnts;++i) {
		pointf2x p0(i*xwid,ipnts[i]*yhit);
//		pointf2x p1((i+1)*xwid,ipnts[i+1]*yhit);
		float distsq = dist2dsq(&p0,opnt);
		if (distsq < bestdistsq) {
			bestdistsq = distsq;
			bestpnt = p0;
		}
	}
	for (i=0;i<npnts-1;++i) {
		pointf2 np;
		pointf2x p0(i*xwid,ipnts[i]*yhit);
		pointf2x p1((i+1)*xwid,ipnts[i+1]*yhit);
		if (checkline(&p0,&p1,opnt,&np)) {
			float distsq = dist2dsq(&np,opnt);
			if (distsq < bestdistsq) {
				bestdistsq = distsq;
				bestpnt = np;
			}
		}
	}
	if (opnt->y>bestpnt.y) { // if above road, no loops for now
		*npnt = *opnt;
		return false;
	}
	*npnt = bestpnt;
	return true;
}

void drawlines()
{
	S32 i;
	for (i=0;i<nlinepnts-1;++i) {
		pointf2x p0(i*xwid,linepnts[i]*yhit);
		pointf2x p1((i+1)*xwid,linepnts[i+1]*yhit);
		drawflinec(p0,p1,C32RED);
	}
	drawfpoint(tp,C32GREEN);
	pnt2line(linepnts,nlinepnts,&tp,&np);
	drawfline(tp,np,C32BLACK);
	drawfcircle(np,C32BLACK,3);
}
#endif

#if 0
S32 dim = 5,graycode = 1;

U32 getgraycode(U32 in)
{
	return in ^ (in>>1);
}

U32 gethamdist(U32 a,U32 b)
{
	U32 d = 0;
	while(a!=b) {
		if ((a&1) != (b&1))
			++d;
		a>>=1;
		b>>=1;
	}
	return d;
}

void drawhypercube()
{
	dim = range(1,dim,6);
	graycode = range(0,graycode,1);
	vector<pointf2> pnts;
	U32 i,j,n = 1<<dim;
	for (i=0;i<n;++i) {
		float a = float(i)/n*TWOPI;
		pointf2x np(sinf(a),cosf(a)); // compass dir
		pnts.push_back(np);
	}
	pnts.push_back(pnts[0]);
	for (i=0;i<n;++i) {
		drawfpoint(pnts[i],C32BLACK);
		C8 s[100];
		sprintf(s," %d ",getgraycode(i));
		drawtext(pnts[i],C32WHITE,C32BLACK,s);
	}
	for (j=0;j<n;++j) {
		S32 gcj = graycode!=0 ? getgraycode(j) : j;
		for (i=j+1;i<n;++i) {
			S32 gci = graycode!=0 ? getgraycode(i) : i;
			S32 hd = gethamdist(gci,gcj);
			if (hd == 1)
				drawfline(pnts[i],pnts[j],C32BLACK);
		}
	}
}
#endif

#if 1
float brad = .1875f;
pointf2 pnta = {.5f,.5f};
pointf2 pntb;
pointf2 normb={.5f,.866f};
pointi2 tilea; // tile number
pointf2 ftilea; // 0 to 1
pointi2 stilea; // 0,1,2 sub square
class pce {
public:
	static const S32 NO = 0;
	static const S32 LEFT = 1;
	static const S32 RIGHT = 2;
	static const S32 TOP = 4;
	static const S32 BOT = 8;
	static const S32 TOPLEFT = 16;
	static const S32 TOPRIGHT = 32;
	static const S32 BOTLEFT = 64;
	static const S32 BOTRIGHT = 128;
};


// walls
#define L pce::LEFT
#define R pce::RIGHT
#define T pce::TOP
#define B pce::BOT
// arcs
#define TL pce::TOPLEFT
#define TR pce::TOPRIGHT
#define BL pce::BOTLEFT
#define BR pce::BOTRIGHT

#if 0
const S32 mapx = 8;
const S32 mapy = 8;
S32 amap[mapy][mapx]  = { // inverted y, highest y lowest line number
	{T|L,T|BR,T|B,T|R|BL,T|L|BR,T|B,T|B,T|R|BL},
	{B|L,B|R,T|L|BR,B|R|TL,L|R,T|L|BR,T|R|BL,L|R},
	{T|L,T|R,B|L|TR,T|BL|BR,B|R|TL,R|L,R|L,R|L},
	{L|B,B|TR,T|B,TL|B|R,T|L|BR,TL|R|BL,L|R,L|R},
	{T|L|BR,T|B,T|B,T|B,B|R|TL,L|R,L|R,L|R},
	{B|L|TR,T|B,T|B,T|BL|BR,T|B|R,L|R|B,L|B|TR,R|B|TL},
	{T|L|BR,T|R|BL,T|L|BR,R|TL|BL,T|L|BR,T|R|BL,T|L|R,T|B|R|L},
	{B|L|R,B|L|TR,B|R|TL,B|L|TR,B|R|TL,B|L|TR,B|TL|TR,T|B|R},
};
#else
const S32 mapx = 4;
const S32 mapy = 3;
S32 amap[mapy][mapx]  = { // inverted y, highest y lowest line number
	{T|L|BR,T|R|BL,T|L|BR,T|B|R},
	{L|R,L|R,L|B|TR,T|R|BL},
	{L|R|B,L|B|TR,T|B,B|R|TL}
};
#endif

void drawarc(const pointf2& pntc,float rad,float startang,float endang,int nsegs)
{
	S32 i;
	pointf2 oldp = {pntc.x + rad*cosf(startang) , pntc.y + rad*sinf(startang)};
	pointf2 p;
	for (i=1;i<=nsegs;++i) {
		float ang = startang + i*(endang-startang)/nsegs;
		p.x = pntc.x + rad*cosf(ang);
		p.y = pntc.y + rad*sinf(ang);
		drawfline(oldp,p,C32BLACK);
		oldp = p;
	}
}

void drawarc(const pointf2& pntc,float rad,int quad)
{
	drawarc(pntc,rad,PI*.5f*quad,PI*.5f*(quad+1),20);
}

bool calcarc(pointf2& p,pointf2& n,S32 quadrant)
{
	pointf2 pc = p;
	// to quadrant 0
	switch(quadrant) {
	case 0:
		break;
	case 1:
		pc.x = 1 - p.x;
		break;
	case 2:
		pc.x = 1 - p.x;
		pc.y = 1 - p.y;
		break;
	case 3:
		pc.y = 1 - p.y;
		break;
	}
	float pradsq = pc.x*pc.x + pc.y*pc.y;
	float bradsq = brad*brad;
	if (pradsq >= bradsq) // outside of arc
		return false;
	if (pradsq < bradsq * .01f) // too close to center
		return false;
	float prad = sqrtf(pradsq);
	float q = brad/prad;
	pc.x *= q;
	pc.y *= q;
	normalize2d(&pc,&n);
	// from quadrant 0
	p = pc;
	switch(quadrant) {
	case 0:
		break;
	case 1:
		p.x = 1 - pc.x;
		n.x = -n.x;
		break;
	case 2:
		p.x = 1 - pc.x;
		p.y = 1 - pc.y;
		n.x = -n.x;
		n.y = -n.y;
		break;
	case 3:
		p.y = 1 - pc.y;
		n.y = -n.y;
		break;
	}
	return true;
}

void drawmaze()
{
	// calc
	pointf2 flo = {floorf(pnta.x),floorf(pnta.y)};
	ftilea.x = pnta.x-flo.x;
	ftilea.y = pnta.y-flo.y;
	tilea.x = (S32)flo.x;
	tilea.y = (S32)flo.y;
	if (ftilea.x<brad)
		stilea.x = 0;
	else if (ftilea.x<1.0f-brad)
		stilea.x = 1;
	else
		stilea.x = 2;
	if (ftilea.y<brad)
		stilea.y = 0;
	else if (ftilea.y<1.0f-brad)
		stilea.y = 1;
	else
		stilea.y = 2;
	// collide
	bool iscoll = false;
	if (tilea.x>=0 && tilea.y>=0 && tilea.x<mapx && tilea.y<mapy) {
		S32 val = amap[mapy-1-tilea.y][tilea.x];
		// left
		if (stilea.x == 0) {
			if (stilea.y == 0) { // bot
				if (val&BL) { // botleft arc
					pointf2 fp = ftilea;
					iscoll = calcarc(fp,normb,0);
					if (iscoll) {
						pntb.x = fp.x + flo.x;
						pntb.y = fp.y + flo.y;
					}
				} else if ((val&(B|L))==(B|L)) { // bot and left wall
					pntb.x = brad + flo.x;
					pntb.y = brad + flo.y;
					normb.x = pntb.x-pnta.x;
					normb.y = pntb.y-pnta.y;
					iscoll = normalize2d(&normb,&normb)>0.0f;
				} else if (val&B) { // bot wall
					pntb.x = pnta.x;
					pntb.y = brad + flo.y;
					normb.x = 0;
					normb.y = 1;
					iscoll = true;
				} else if (val&L) { // left wall
					pntb.x = brad + flo.x;
					pntb.y = pnta.y;
					normb.x = 1;
					normb.y = 0;
					iscoll = true;
	//			} else { // none
				}
			} else if (stilea.y == 1) { // middle y
				if (val&L) { // left wall
					pntb.x = brad + flo.x;
					pntb.y = pnta.y;
					normb.x = 1;
					normb.y = 0;
					iscoll = true;
				}
			} else { // stilea.y == 2 top
				if (val&TL) { // topleft arc
					pointf2 fp = ftilea;
					iscoll = calcarc(fp,normb,3);
					if (iscoll) {
						pntb.x = fp.x + flo.x;
						pntb.y = fp.y + flo.y;
					}
				} else if ((val&(T|L))==(T|L)) { // top and left wall
					pntb.x = brad + flo.x;
					pntb.y = 1 - brad + flo.y;
					normb.x = pntb.x-pnta.x;
					normb.y = pntb.y-pnta.y;
					iscoll = normalize2d(&normb,&normb)>0.0f;
				} else if (val&T) { // top wall
					pntb.x = pnta.x;
					pntb.y = 1 - brad + flo.y;
					normb.x = 0;
					normb.y = -1;
					iscoll = true;
				} else if (val&L) { // left wall
					pntb.x = brad + flo.x;
					pntb.y = pnta.y;
					normb.x = 1;
					normb.y = 0;
					iscoll = true;
	//			} else { // none
				}
			}
		// middle x
		} else if (stilea.x == 1) {
			if (stilea.y == 0) { // bot
				if (val&B) { // bot wall
					pntb.x = pnta.x;
					pntb.y = brad + flo.y;
					normb.x = 0;
					normb.y = 1;
					iscoll = true;
				}
			} else if (stilea.y == 1) { // middle y
				pntb = pnta;
			} else { // stilea.y == 2 top
				if (val&T) { // top wall
					pntb.x = pnta.x;
					pntb.y = 1 -brad + flo.y;
					normb.x = 0;
					normb.y = -1;
					iscoll = true;
				}
			}
		// right stilea.x == 2
		} else {
			if (stilea.y == 0) { // bot
				if (val&BR) { // botright arc
					pointf2 fp = ftilea;
					iscoll = calcarc(fp,normb,1);
					if (iscoll) {
						pntb.x = fp.x + flo.x;
						pntb.y = fp.y + flo.y;
					}
				} else if ((val&(B|R))==(B|R)) { // bot and right wall
					pntb.x = 1 - brad + flo.x;
					pntb.y = brad + flo.y;
					normb.x = pntb.x-pnta.x;
					normb.y = pntb.y-pnta.y;
					iscoll = normalize2d(&normb,&normb)>0.0f;
				} else if (val&B) { // bot wall
					pntb.x = pnta.x;
					pntb.y = brad + flo.y;
					normb.x = 0;
					normb.y = 1;
					iscoll = true;
				} else if (val&R) { // right wall
					pntb.x = 1 - brad + flo.x;
					pntb.y = pnta.y;
					normb.x = -1;
					normb.y = 0;
					iscoll = true;
	//			} else { // none
				}
			} else if (stilea.y == 1) { // middle y
				if (val&R) { // right wall
					pntb.x = 1 - brad + flo.x;
					pntb.y = pnta.y;
					normb.x = -1;
					normb.y = 0;
					iscoll = true;
				}
			} else { // stilea.y == 2 top
				if (val&TR) { // topright arc
					pointf2 fp = ftilea;
					iscoll = calcarc(fp,normb,2);
					if (iscoll) {
						pntb.x = fp.x + flo.x;
						pntb.y = fp.y + flo.y;
					}
				} else if ((val&(T|R))==(T|R)) { // top and right wall
					pntb.x = 1 - brad + flo.x;
					pntb.y = 1 - brad + flo.y;
					normb.x = pntb.x-pnta.x;
					normb.y = pntb.y-pnta.y;
					iscoll = normalize2d(&normb,&normb)>0.0f;
				} else if (val&T) { // top wall
					pntb.x = pnta.x;
					pntb.y = 1 - brad + flo.y;
					normb.x = 0;
					normb.y = -1;
					iscoll = true;
				} else if (val&R) { // right wall
					pntb.x = 1 - brad + flo.x;
					pntb.y = pnta.y;
					normb.x = -1;
					normb.y = 0;
					iscoll = true;
	//			} else { // none
				}
			}
		}
	}
	if (!iscoll) {
		pntb = pnta;
	}
	// draw
	S32 i,j;
	for (j=0;j<mapy;++j) {
		for (i=0;i<mapx;++i) {
			S32 val = amap[mapy-1-j][i];
			if (val & pce::BOT) {
				drawfline(pointf2x((float)i,j+brad),pointf2x((float)i+1,j+brad),C32BLACK);
			}
			if (val & pce::LEFT) {
				drawfline(pointf2x(i+brad,(float)j),pointf2x(i+brad,(float)j+1),C32BLACK);
			}
			if (val & pce::TOP) {
				drawfline(pointf2x((float)i,j+1-brad),pointf2x((float)i+1,j+1-brad),C32BLACK);
			}
			if (val & pce::RIGHT) {
				drawfline(pointf2x(i+1-brad,(float)j),pointf2x(i+1-brad,(float)j+1),C32BLACK);
			}
			if (val & pce::BOTLEFT) {
				drawarc(pointf2x((float)i,(float)j),brad,0);
			}
			if (val & pce::BOTRIGHT) {
				drawarc(pointf2x((float)i+1,(float)j),brad,1);
			}
			if (val & pce::TOPRIGHT) {
				drawarc(pointf2x((float)i+1,(float)j+1),brad,2);
			}
			if (val & pce::TOPLEFT) {
				drawarc(pointf2x((float)i,(float)j+1),brad,3);
			}
		}
	}
	drawfpoint(pnta,C32BLACK);
	if (iscoll) {
		drawfcircle(pntb,C32RED,4);
		drawfline(pnta,pntb,C32BLACK);
		pointf2 pntc = {pntb.x+normb.x*.25f,pntb.y+normb.y*.25f};
		drawfline(pntb,pntc,C32GREEN);
	}
//	drawarc(pointf2x(0,1),brad,0);
//	drawarc(pointf2x(1,1),brad,1);
//	drawarc(pointf2x(2,1),brad,2);
//	drawarc(pointf2x(3,1),brad,3);
}
#endif

#if 1

float prob2corr(float p)
{
	return 2*p-1;
}

float qmfunprob(float ang)
{
	float amp = cosf(ang);
	return amp*amp;
}

float hvfunprob(float ang)
{
	ang = fmodf(fabsf(ang),PI);
	if (ang<PI/4 || ang>=3*PI/4)
		return 1;
	return 0;
}

float hiddenfunprobtest(float hidang)
{
	float ang = 0;
	float hidp = hvfunprob(hidang)*hvfunprob(hidang-ang);
//	logger("hiddenfunprobtest (%f) = %f\n",hidang,hidp);
	return hidp;
}

float hiddenfunprob(float ang)
{
	S32 i;
	static bool once;
	S32 nhidsteps = 100;
	float hidpsum = 0;
	for (i=0;i<nhidsteps;++i) {
		float hidang = TWOPI*i/nhidsteps;
		float hidp = hvfunprob(hidang)*hvfunprob(hidang-ang);
		if (ang == 0 && !once) {
			logger("    ang = %f, hidang = %f, hidp = %f\n",ang,hidang,hidp);
		}
		hidpsum += hidp;
	}
	float prob = hidpsum / nhidsteps;
	if (ang == 0 && !once)
		logger("hiddenfunprob (%f) = %f\n",ang,prob);
	once = true;
	return prob;
}

float qmfuncorr(float ang)
{
	return prob2corr(qmfunprob(ang));
}

float hiddenfuncorr(float ang)
{
	return prob2corr(hiddenfunprob(ang));
}

void drawbell()
{
#if 1
	drawfunctionb(qmfunprob,0,TWOPI);
	drawfunctionb(hiddenfunprob,0,TWOPI);
//	drawfunctionb(hiddenfunprobtest,0,TWOPI);
#else
	drawfunctionb(qmfuncorr,0,TWOPI);
	drawfunctionb(hiddenfuncorr,0,TWOPI);
#endif
}

#endif

void drawstuff()
{
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
	drawbell(); // Bell's theorem
}
// end user section

struct menuvar edv[]={
// user vars
#if 0
	{"le",&le,D_FLOAT,FLOATUP/16},
	{"off",&off,D_FLOAT,FLOATUP/16},
	{"scl",&scl,D_FLOAT,FLOATUP/16},
	{"prem",&prem,D_FLOAT,FLOATUP/16},
	{"prea",&prea,D_FLOAT,FLOATUP/16},
	{"postm",&postm,D_FLOAT,FLOATUP/16},
	{"posta",&posta,D_FLOAT,FLOATUP/16},
#endif
// plotter user vars
	{"@yellow@---- PLOTTER USER VARS -----------------",NULL,D_VOID,0},
#if 0
	{"dim",&dim,D_INT,1},
	{"graycode",&graycode,D_INT,1},
#endif
#if 0
	{"linepnts[0]",&linepnts[0],D_INT,1},
	{"linepnts[1]",&linepnts[1],D_INT,1},
	{"linepnts[2]",&linepnts[2],D_INT,1},
	{"linepnts[3]",&linepnts[3],D_INT,1},
	{"linepnts[4]",&linepnts[4],D_INT,1},
	{"linepnts[5]",&linepnts[5],D_INT,1},
	{"linepnts[6]",&linepnts[6],D_INT,1},
	{"xwid",&xwid,D_FLOAT,FLOATUP/16},
	{"yhit",&yhit,D_FLOAT,FLOATUP/16},
	{"testpoint.x",&tp.x,D_FLOAT,FLOATUP/16},
	{"testpoint.y",&tp.y,D_FLOAT,FLOATUP/16},
#endif
#if 1
	{"pnta.x",&pnta.x,D_FLOAT,FLOATUP/16},
	{"pnta.y",&pnta.y,D_FLOAT,FLOATUP/16},
	{"pntb.x",&pntb.x,D_FLOAT,FLOATUP|D_RDONLY},
	{"pntb.y",&pntb.y,D_FLOAT,FLOATUP|D_RDONLY},
	{"normb.x",&normb.x,D_FLOAT,FLOATUP|D_RDONLY},
	{"normb.y",&normb.y,D_FLOAT,FLOATUP|D_RDONLY},
	{"tilea.x",&tilea.x,D_INT|D_RDONLY},
	{"tilea.y",&tilea.y,D_INT|D_RDONLY},
	{"ftilea.x",&ftilea.x,D_FLOAT|D_RDONLY},
	{"ftilea.y",&ftilea.y,D_FLOAT|D_RDONLY},
	{"stilea.x",&stilea.x,D_INT|D_RDONLY},
	{"stilea.y",&stilea.y,D_INT|D_RDONLY},
#endif
#if 0
	{"theta",&theta,D_FLOAT,FLOATUP},
	{"phi",&phi,D_FLOAT,FLOATUP},
	{"x2slope",&x2slope,D_FLOAT,FLOATUP/16},
#endif
// plotter system vars
	{"@lightcyan@---- PLOTTER SYSTEM VARS -----------------",NULL,D_VOID,0},
	{"lzoom",&lzoom,D_FLOAT,FLOATUP/16},
	{"zoom",&zoom,D_FLOAT|D_RDONLY,FLOATUP},
	{"centerx",&center.x,D_FLOAT,FLOATUP/16},
	{"centery",&center.y,D_FLOAT,FLOATUP/16},
	{"nsteps",&nsteps,D_INT,1},
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

// O(n^3)
S32 calcmaxsumbrute(S32 terms[],S32 nterms)
{
	S32 ms = terms[0];
	S32 i,j,k;
	for (i=0;i<nterms;++i) {
		for (j=i;j<nterms;++j) {
			S32 nst = j-i+1;
			S32 s = 0;
			for (k=0;k<nst;++k) {
				s += terms[i+k];
			}
			if (s>ms) {
				ms = s;
			}
		}
	}
	return ms;
}

// O(n)
S32 calcmaxsumfinesse(S32 terms[],S32 nterms)
{
	S32 ms = terms[0];
	S32 cs = 0;
	S32 i;
	for (i=0;i<nterms;++i) {
		S32 ti = terms[i];
		if (ti<0 && ms<0) {
			if (ti>ms) {
				ms = ti;
			}
		} else {
			cs += ti;
			if (cs<0)
				cs = 0;
			if (cs>ms)
				ms = cs;
		}
	}
	return ms;
}

void testadjsums()
{
	const S32 nterms = 10;
	const S32 minsum = -10;
	const S32 maxsum = 10;
	const S32 niters = 1000000;
	S32 terms[nterms];
	S32 i,j;
	//mt_setseed(0);
	//mt_setseed(getmillisec());
	S32 good = 0, bad = 0;
	for (i=0;i<niters;++i) {
		for (j=0;j<nterms;++j) {
			terms[j] = minsum + mt_random(maxsum-minsum+1);
		}
		S32 msb = calcmaxsumbrute(terms,nterms);
		S32 msf = calcmaxsumfinesse(terms,nterms);
//		if (true) {
		if (msb!=msf) {
			logger_disableindent();
			logger("terms ");
			for (j=0;j<nterms;++j) {
				logger("%3d ",terms[j]);
			}
			logger("maxsum brute %3d finesse %3d",msb,msf);
			logger("\n");
			logger_enableindent();
			++bad;
		} else {
			++good;
		}
	}
	logger("good = %d, bad = %d\n",good,bad);
}

} // end namespace u_s_plotter

using namespace u_s_plotter;

void plotterinit()
{
	testgroups();
	testadjsums();
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
	con=con32_alloc(400,100,C32WHITE,C32BLACK);
}

void plotterproc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
//	calcstuff();
}

void plotterdraw2d()
{
	clipclear32(B32,backcolor);
	zoom=expf(lzoom);
	drawgrid();
	drawaxis();
//	bitmap32* cb=con32_getbitmap32(con);
//	clipblit32(cb,B32,0,0,WX-cb->size.x,WY-cb->size.y,cb->size.x,cb->size.y);
	drawstuff();
}

void plotterexit()
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
}
