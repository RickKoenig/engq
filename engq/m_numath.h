// for debprint, user adjustable variables
extern S32 userinta,userintb,userintc,userintd;
extern float userfloata,userfloatb,userfloatc,userfloatd;
extern struct pointf3 userpointf3a,userpointf3b,userpointf3c,userpointf3d;
////////// constants ///////////
#define PI 3.1415926f
#define TWOPI (PI*2.0f)
#define PIOVER180 (PI/180.0f)
#define PIUNDER180 (180.0f/PI)

extern struct pointf3 zerov;
extern struct pointf3 onev;
// from mathasm.asm
float flog(float);
#define FPUCW_RND_NEAR	0
#define FPUCW_RND_DOWN	(1<<10)
#define FPUCW_RND_UP	(2<<10)
#define FPUCW_RND_0		(3<<10)
#define FPUCW_PREC_32 	0
#define FPUCW_PREC_RES	(1<<8)
#define FPUCW_PREC_64	(2<<8)
#define FPUCW_PREC_80	(3<<8)
void setfpuroundprec(U32 rndtype,U32 prectype);
void defaultfpucontrol();
//enum {RND_NEAR,RND_DOWN,RND_UP,RND_0};
//enum {PREC_32,PREC_RES,PREC_64,PREC_80}; // clean up
//void cdecl setfpucontrol(U32); // maybe i'll get rid of cdecl's in favor of speed, compare with flog
//U32 cdecl getfpucontrol();
//U32 cdecl getfpustatus();
//S32 cdecl f2S32(float);


///////////// functions ////////////////
float modf2(float v,float* ip); // use to get around mingw bug in modf, floats

float rasin(float v);	// radians
float racos(float v);	// radians
float ratan2(float y,float x);	// returns radians

float dsin(float deg);	 // degrees
float dcos(float deg);	 // degrees

S32 isqrt(S32); // round down, returns -1 if undefined
S32 ilog2(U32 t); // round down, returns -1 if undefined
S32 pow2up(S32 i); // round up to a power of 2
S32 ipow(S32 b,S32 e); // b^e

// unsigned
float normalangdeg(float deg);	// returns 0 <=ang < 360
float normalangrad(float rad);	// returns 0 <= ang < TWOPI
// signed
float snormalangdeg(float deg);	// returns -180 <= ang < 180
float snormalangrad(float rad);	// returns -PI <= ang < PI

//////// geometry
// 2d integer point
struct pointi2 {
	S32 x;
	S32 y;
};

struct pointi2x : public pointi2 {
	pointi2x(S32 xa=0,S32 ya=0) { x=xa;y=ya;}
	pointi2x(const pointi2& p) : pointi2(p) {}
	bool operator==(const pointi2& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const pointi2& rhs) const
	{
		return !(*this == rhs);
	}
};

// 2d integer point 8 bit
struct pointi2b8 {
	S8 x;
	S8 y;
};

struct pointi2b8x : public pointi2b8 {
	pointi2b8x(S8 xa=0,S8 ya=0) { x=xa;y=ya;}
	pointi2b8x(const pointi2b8& p) : pointi2b8(p) {}
	bool operator==(const pointi2b8& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const pointi2b8& rhs) const
	{
		return !(*this == rhs);
	}
};

// 2d integer point 8 bit
struct pointi2b4 {
	S8 x:4;
	S8 y:4;
};

struct pointi2b4x : public pointi2b4 {
	pointi2b4x(S8 xa=0,S8 ya=0) { x=xa;y=ya;}
	pointi2b4x(const pointi2b4& p) : pointi2b4(p) {}
	bool operator==(const pointi2b4& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const pointi2b4& rhs) const
	{
		return !(*this == rhs);
	}
};

// 2d integer rectangle
struct recti2 {
	struct pointi2 topleft;
	struct pointi2 size;
};

struct recti2x : public recti2 {
	recti2x(S32 tlx=0,S32 tly=0,S32 sx=0,S32 sy=0) { topleft.x=tlx;topleft.y=tly;size.x=sx;size.y=sy;}
};

// 2d floating point
struct pointf2 {
	float x;
	float y;
	float getmemberindex(U32 idx) const
	{
		float* fa = (float*)(&x);
		return fa[idx];
//		return 0;
	}
};

struct pointf2x : public pointf2 {
	pointf2x(float xa=0,float ya=0) {x=xa;y=ya;}
	pointf2x(const pointf2& p) : pointf2(p) {}
};

//extern pointf2 zerov2d;

// 3d floating point
struct pointf3 {
	float x;
	float y;
	float z;
	union {
		float w;
		struct {
			U16 vmatidx; // used during mesh optimization
			U16 bone;
//			U16 oldvertidx;
		};
	};
	float getmemberindex(U32 idx) const
	{
		float* fa = (float*)(&x);
		return fa[idx];
	}
};
typedef pointf3 VEC;

struct pointf3x : public pointf3 {
/*	float x;
	float y;
	float z;
	union {
		float w;
		struct {
			U8 vmatidx; // used during mesh optimization
			U8 bone;
			U16 oldvertidx;
		};
	}; */
	pointf3x(float xa=0,float ya=0,float za=0,float wa=1) {x=xa;y=ya;z=za;w=wa;}
/*	pointf3x(const pointf3& p)
	{
		pointf3* pb = dynamic_cast<pointf3*>(this);
		*pb=p;
	} */
	pointf3x(const pointf3& p) : pointf3(p) {}
//	pointf3x(float xa,float ya,float za,float wa) {x=xa;y=ya;z=za;w=wa;}
//	pointf3x(float xa,float ya,float za,U32 wa) {x=xa;y=ya;z=za;oldvertidx=wa;}
//	operator pointf3 () {return pod;}
};
/*
// 4d floating point
struct pointf4 {
	float x;
	float y;
	float z;
	float w;
};

struct pointf4x : public pointf4 {
	pointf4x(float xa,float ya,float za,float wa) { x=xa;y=ya;z=za;w=wa;}
};
*/
////// random numbers ///////////////////////////////
//S32 random(S32 numpossible); // 0 to numpossible-1, doesnt work for #'s > 30000
//float frand(); // 0 <= ret < 1
//void randomize();
//void setseed(S32 seed);
//float randf(float s, float r);

#define setVEC(v, _x, _y, _z) \
	(v)->x = _x, (v)->y = _y, (v)->z = _z


float dot2d(const struct pointf2 *a,const struct pointf2 *b);
float cross2d(const struct pointf2 *a,const struct pointf2 *b);
float normalize2d(const struct pointf2 *a,struct pointf2 *b);
float dist2d(const struct pointf2 *a,const struct pointf2 *b);
float dist2dsq(const struct pointf2 *a,const struct pointf2 *b);
float len2d(const struct pointf2 *a);
float len2dsq(const struct pointf2 *a);


extern struct pointf3 zerov,onev;
S32 isequal3d(const struct pointf3 *a,const struct pointf3 *b);
S32 iszero3d(const struct pointf3 *a);

float dot3d(const struct pointf3 *a,const struct pointf3 *b);
void cross3d(const struct pointf3 *a,const struct pointf3 *b,struct pointf3 *c);
float normalize3d(const struct pointf3 *a,struct pointf3 *b);
S32 proj3d(const struct pointf3 *a,const struct pointf3 *b,struct pointf3 *p);
float dist3d(const struct pointf3 *a,const struct pointf3 *b);
float dist3dsq(const struct pointf3 *a,const struct pointf3 *b);
float dist3dsq2(const struct pointf3 *a,const struct pointf3 *b);
float len3d(const struct pointf3 *a);
float len3dsq(const struct pointf3 *a);

float dot4d(const struct pointf3 *a, const struct pointf3 *b);

void nanerr(const C8* errstr,float testfloat);
#define EPSILON 1.0e-20f

//typedef struct pointf3 VEC;

//S32 mylcd(S32 a,S32 b);
// new math api
void interp3d(const struct pointf3 *a,const struct pointf3 *b,float t,struct pointf3 *i);

S32 gcd(S32 a,S32 b);
S32 lcm(S32 a,S32 b);

// some complex stuff for quadratic equation
typedef complex<float> compf;
typedef complex<double> compd;
// returns true if roots are real, acoef should be non zero
bool quadratic(float acoef,float bcoef,float ccoef,float& r0,float& r1); // only updates roots if roots are real
bool quadratic(float acoef,float bcoef,float ccoef,compf& r0,compf& r1);
void quadratic(const compf& acoef,const compf& bcoef,const compf& ccoef,compf& r0,compf& r1);

// calc X and Y from
// A*X + B*Y = E
// C*X + D*Y = F
// does no checks for singularities etc.
void solveLinear2(float a, float b, float c, float d, float e, float f, float& x, float& y);
void useLinear2(float a, float b, float c, float d, float x, float y, float& e, float& f);

bool my_isnan(float v);
