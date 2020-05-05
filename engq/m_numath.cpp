#include <stdlib.h>
#include <cmath>
#include <float.h>
#include <m_eng.h>

//pointf2 zerov2d;

S32 userinta,userintb,userintc,userintd;
float userfloata,userfloatb,userfloatc,userfloatd;
pointf3 userpointf3a,userpointf3b,userpointf3c,userpointf3d;

// mingw's modf only works with doubles (sigh), fixed, using <cmath>
float modf2(float v,float* ip)
{
	float dip;
	float r=modf(v,&dip);
	*ip=dip;
	return r;
}

float dsin(float deg)
{
	return sinf(deg*PIOVER180);
}

float dcos(float deg)
{
	return cosf(deg*PIOVER180);
}

float normalangdeg(float deg)	// returns 0<=ang<360
{
	nanerr("normalangdeg",deg);
	if (deg<0.0f)
		deg+=360.0f;
	if (deg>=360.0f)
		deg-=360.0f;
	return deg;
}

float normalangrad(float deg)	// returns 0<=ang<2*PI
{
	nanerr("normalangrad",deg);
	if (deg<0.0f)
		deg+=TWOPI;
	if (deg>=TWOPI)
		deg-=TWOPI;
	return deg;
}

float snormalangdeg(float deg)	// returns -180<=ang<180
{
	nanerr("snormalangdeg",deg);
	if (deg<-180.0f)
		deg+=360.0f;
	if (deg>=180.0f)
		deg-=360.0f;
	return deg;
}

float snormalangrad(float deg)	// returns -PI<=ang<PI
{
	nanerr("snormalangrad",deg);
	if (deg<-PI)
		deg+=TWOPI;
	if (deg>=PI)
		deg-=TWOPI;
	return deg;
}

S32 isqrt(S32 v)
{
	S32 d=v,q=v;
	if (v<0)
		return -1;
	if (v==0)
		return 0;
	do {
		d=(d+q)>>1;
		q=v/d;
	} while((q-d>1)||(q-d<-1));
	return min(q,d);
}

/*S32 isqrtrnd(S32 v) // test before using
{
	S32 d=v,q=v;
	if (v<=0)
		return -1;
	do {
		d=(d+q)>>1;
		q=1+v/d;
	} while((q-d>1)||(q-d<-1));
	return min(q,d);
}*/

S32 ilog2(U32 t)
{
	S32 r=0;
	if (t==0)
		return -1; // error, can YOU take the log of 0?
	// there's a 1 out there somewhere
	while (t) {
		t>>=1;
		r++;
	}
	return r-1;
}

// round up to power of 2
S32 pow2up(S32 i)
{
	S32 o=ilog2(i);
	if ((1<<o)==i)
		return i;
	return 2<<o;
}

S32 ipow(S32 b,S32 e)
{
	S32 r = 1;
	while(e>0) {
		r *= b;
		--e;
	}
	return r;
}

float dot2d(const struct pointf2 *a,const struct pointf2 *b)
{
	float px,py;
	px = a->x*b->x;
	py = a->y*b->y;
	float acc = px;
	acc += py;
	return acc;
}

float cross2d(const struct pointf2 *a,const struct pointf2 *b)
{
	return a->x*b->y - a->y*b->x;
}

// return length
float normalize2d(const struct pointf2 *a,struct pointf2 *b)
{
	float r,ir;
	r=a->x*a->x+a->y*a->y;
	r=sqrtf(r);
	if (r<EPSILON)
		return 0;
	if (my_isnan(r))
		return 0;
	ir=1.0f/r;
	b->x=ir*a->x;
	b->y=ir*a->y;
	nanerr("normalize3d",r);
	return r;
}

float dist2d(const struct pointf2 *a,const struct pointf2 *b)
{
	struct pointf2 d;
	d.x=a->x-b->x;
	d.y=a->y-b->y;
	return sqrtf(d.x*d.x+d.y*d.y);
}

float dist2dsq(const struct pointf2 *a,const struct pointf2 *b)
{
	struct pointf2 d;
	d.x=a->x-b->x;
	d.y=a->y-b->y;
	return d.x*d.x+d.y*d.y;
}

float len2d(const struct pointf2 *a)
{
	return sqrtf(a->x*a->x+a->y*a->y);
}

float len2dsq(const struct pointf2 *a)
{
	return a->x*a->x+a->y*a->y;
}

// devstud has no -ffloat-store option, (does have /fp:strict), but what's the use
// try using no temporary variables, hmm all math functions ?
float dot3d(const struct pointf3 *a,const struct pointf3 *b)
{
//	logger("dot3d %g %g %g, %g %g %g\n",a->x,a->y,a->z,b->x,b->y,b->z);
	float px,py,pz;
	px = a->x*b->x;
	py = a->y*b->y;
	pz = a->z*b->z;
	float acc = px;
	acc += py;
	acc += pz;
	return acc;
}

void cross3d(const struct pointf3 *a,const struct pointf3 *b,struct pointf3 *c)
{
	struct pointf3x t;
	t.z=a->x*b->y-a->y*b->x;
	t.x=a->y*b->z-a->z*b->y;
	t.y=a->z*b->x-a->x*b->z;
	*c=t;
}

// return length
float normalize3d(const struct pointf3 *a,struct pointf3 *b)
{
	float r,ir;
	r=a->x*a->x+a->y*a->y+a->z*a->z;
	r=sqrtf(r);
	if (r<EPSILON)
		return 0;
	if (my_isnan(r))
		return 0;
	ir=1.0f/r;
	b->x=ir*a->x;
	b->y=ir*a->y;
	b->z=ir*a->z;
	nanerr("normalize3d",r);
	return r;
}

struct pointf3 zerov;
struct pointf3 onev={1,1,1,1};

S32 isequal3d(const struct pointf3 *a,const struct pointf3 *b)
{
	return a->x==b->x && a->y==b->y && a->z==b->z;
}

S32 iszero3d(const struct pointf3 *a)
{
	return a->x==0 && a->y==0 && a->z==0;
}

void nanerr(const C8* str,float f) // temporary mod, put back
{
#if 1
	if (my_isnan(f))
//		*((U8*)0x87654321)=0xaa; // force a crash to get the debugger up
		errorexit("err: %s is a nan %f",str,f);
#endif
}

float dist3d(const struct pointf3 *a,const struct pointf3 *b)
{
	struct pointf3 d;
	d.x=a->x-b->x;
	d.y=a->y-b->y;
	d.z=a->z-b->z;
	return sqrtf(d.x*d.x+d.y*d.y+d.z*d.z);
}

float dist3dsq(const struct pointf3 *a,const struct pointf3 *b)
{
	struct pointf3 d;
	d.x=a->x-b->x;
	d.y=a->y-b->y;
	d.z=a->z-b->z;
	return d.x*d.x+d.y*d.y+d.z*d.z;
}

float dist3dsq2(const struct pointf3 *a,const struct pointf3 *b)
{
	struct pointf3 d;
	d.x=a->x-b->x;
	d.z=a->z-b->z;
	return d.x*d.x+d.z*d.z;
}


float len3d(const struct pointf3 *a)
{
	return sqrtf(a->x*a->x+a->y*a->y+a->z*a->z);
}

float len3dsq(const struct pointf3 *a)
{
	return a->x*a->x+a->y*a->y+a->z*a->z;
}

S32 proj3d(const struct pointf3 *a,const struct pointf3 *b,struct pointf3 *pr)
{
	float k,d;
	d=dot3d(b,b);
//	logger("d = %g\n",d);
	if (d<EPSILON)
		return 0;
	k=dot3d(a,b);
//	logger("k = %g\n",k);
	k=k/d;
//	logger("k/d = %g\n",k);
	pr->x=k*b->x;
	pr->y=k*b->y;
	pr->z=k*b->z;
//	logger("pr %g %g %g\n",pr->x,pr->y,pr->z);
	return 1;
}

S32 mylcd(S32 a,S32 b)
{
	if (a<0)
		a=-a;
	if (b<0)
		b=-b;
	while(1) {
		if (a==0)
			return b;
		if (b==0)
			return a;
		if (a>b)
				a=a%b;
			else
				b=b%a;
	}
}

// new math api
void interp3d(const struct pointf3* a,const struct pointf3* b,float t,struct pointf3* i)
{
	i->x=a->x+(b->x-a->x)*t;
	i->y=a->y+(b->y-a->y)*t;
	i->z=a->z+(b->z-a->z)*t;
}

/*
///// hexfloat prints out a floating point number in exact hex floating point into a string
/// if i need it again i'll know where to look
//// example 3.0f would become 1.8Hx2^1
U8* hf(float f)
{
	U8[2];
	S32 exp;
	static S32 c;
	static U8 s[16][32];
	U32 *ip;
	ip=(U32*)&f;
	c++;
	c&=0xf;
	if (*ip & 0x80000000) {
		sign[0]='-';
		sign[1]='\0';
	} else
		sign[0]='\0';
	exp=((*ip >> 23)&0xff);
	if (exp==0) {
		sprintf(s[c],"%s0",sign);
		return s[c];
	} else if (exp==0xff) {
		sprintf(s[c],"%sNAN",sign);
		return s[c];
	}
	exp-=0x7f;
	sprintf(s[c],"%s1.%XHx2^%d",sign,(*ip & 0x7fffff)<<1,exp);
	return s[c];
}
*/
#define PRECMASK 0x300
#define RNDMASK 0xc00
static U32 hasdefault;
static U32 defaultfpu;

void setfpuroundprec(U32 rndtype,U32 prectype)
{
	defaultfpu&=~(PRECMASK|RNDMASK);
	defaultfpu|=rndtype|prectype;
//	logger("setfpuroundprc: set fpu control to $%08x\n",defaultfpu);
	WRITECW(defaultfpu);
}

void defaultfpucontrol()
{
	if (!hasdefault) {
		READCW(defaultfpu);
		U32 cw=defaultfpu;
		defaultfpu&=~(PRECMASK|RNDMASK);
		defaultfpu|=FPUCW_RND_NEAR|FPUCW_PREC_32;
		logger("initial fpu control word was at $%08x, now setting to $%08x (low precision, round nearest)\n",cw,defaultfpu);
		hasdefault=1;
	} else {
		defaultfpu&=~(PRECMASK|RNDMASK);
		defaultfpu|=FPUCW_RND_NEAR|FPUCW_PREC_32;
	}
	WRITECW(defaultfpu);
}

//#define VERBOSERAC
float racos(float r)	// radians
{
	if (r>=1)
		return 0;
	if (r<=-1)
		return PI;
#ifdef VERBOSERAC
	string ins = float2strhex(r);
#endif
	float x=sqrtf(1-r*r);
	float y=r;
	float a=atan2(x,y);
//	float a=acosf(r);
#ifdef VERBOSERAC
	string outs = float2strhex(r);
	logger("racos('%s') = '%s'\n",ins.c_str(),outs.c_str());
#endif
	return a;
}

//#define VERBOSERAS
float rasin(float r)	// radians
{
	if (r>=1)
		return PI/2;
	if (r<=-1)
		return -PI/2;
#ifdef VERBOSERAS
	string ins = float2strhex(r);
#endif
	float x=r;
	float y=sqrtf(1-r*r);
	float a=atan2(x,y);
//	float a=asinf(r);
#ifdef VERBOSERAS
	string outs = float2strhex(r);
	logger("rasin('%s') = '%s'\n",ins.c_str(),outs.c_str());
#endif
	return a;
}

float ratan2(float y,float x)	// returns degrees 0 to TWOPI
{
	return normalangrad((float)atan2((double)y,(double)x));
}

S32 gcd(S32 a,S32 b)
{
	a=abs(a);
	b=abs(b);
	while(true) {
		if (a==0)
			return b;
		if (b==0)
			return a;
		if (a>b) {
			a%=b;
		} else {
			b%=a;
		}
	}
}

S32 lcm(S32 a,S32 b)
{
	if (a==0 && b==0)
		return 0;
	return (abs(a)/gcd(a,b)*abs(b)); // divides cleanly
}

// returns false if complex
bool quadratic(float a,float b,float c,float& r0,float& r1)
{
	float radsq = b*b - 4.0f*a*c;
	if (radsq < 0.0f)
		return false;
	float rad = sqrt(radsq); // rad is non negative
	// pick more precise root
	if (b > 0)
		rad = -rad; // make b and rad opposite signs
	r0 = (-b + rad)/(2.0f*a);
	if (abs(r0) < EPSILON) // incase r0 is too small (like 0), prevent divide by 0
		r1 = (-b - rad)/(2.0f*a);
	else
		r1 = c/(r0*a);
	return true;
}

// returns false if complex
bool quadratic(float a,float b,float c,compf& r0,compf& r1)
{
	float radsq = b*b - 4.0f*a*c;
	if (radsq < 0.0f) {
		float radi = sqrt(-radsq);
		float i2a = .5f/a;
		r0 = compf(-b*i2a,radi*i2a);
		r1 = compf(-b*i2a,-radi*i2a);
		return false;
	}
	float rad = sqrt(radsq); // rad is non negative
	// pick more precise root
	if (b > 0)
		rad = -rad; // make b and rad opposite signs
	r0 = (-b + rad)/(2.0f*a);
	if (r0.real()*r0.real() + r0.imag()*r0.imag() < EPSILON*EPSILON) // incase r0 is too small (like 0), prevent divide by 0
		r1 = (-b - rad)/(2.0f*a);
	else
		r1 = c/(r0*a);
	return true;
}

// all complex
void quadratic(const compf& a,const compf& b,const compf& c,compf& r0,compf& r1)
{
	compf radsq = b*b - 4.0f*a*c;
	compf rad = sqrt(radsq);
	compf i2a = .5f/a;
	// pick more precise root
	float dot = b.real()*rad.real() + b.imag()*rad.imag();
	if (dot > 0) // make b and rad point in most opposite ways
		rad = -rad;
	r0 = (-b + rad)*i2a;
	if (r0.real()*r0.real() + r0.imag()*r0.imag() < EPSILON*EPSILON) // incase r0 is too small (like 0), prevent divide by 0
		r1 = (-b - rad)*i2a;
	else
		r1 = c/(r0*a);
}

// calc X and Y from
// A*X + B*Y = E
// C*X + D*Y = F
// does no checks for singularities etc.
void solveLinear2(float a, float b, float c, float d, float e, float f, float &x, float &y)
{
	float det = a * d - b * c;
	float invDet = 1.0f / det;
	float xDet = e * d - b * f;
	float yDet = a * f - e * c;
	x = xDet * invDet;
	y = yDet * invDet;
}

void useLinear2(float a, float b, float c, float d, float x, float y, float& e, float& f)
{
	e = a * x + b * y;
	f = c * x + d * y;
}

// returns true if nans or inf
bool my_isnan(float v)
{
	float s = v - v;
	return v != v || s != s;
//#ifdef isnan
    //return isnan(v);
//#else
//	return _isnan(v) != 0;
//#endif
}
