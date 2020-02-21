// make sure these are right !?
typedef unsigned char U8;
typedef signed char S8;
typedef char C8;

typedef unsigned short U16;
typedef short S16;

typedef unsigned int U32;
typedef int S32;

typedef unsigned long long U64;
typedef long long S64;

// color as a U32 or distinct r,g,b,a values
struct C32 {
	union {
		struct {
			U8 b,g,r,a;
		};
		U32 c32;
	};
	C32(U8 ra,U8 ga,U8 ba,U8 aa=0xff) {r=ra;g=ga;b=ba;a=aa;}
	C32(U32 c32a) {c32=c32a;}
	C32(){}
	bool operator==(C32 rhs){return c32==rhs.c32;}
	bool operator!=(C32 rhs){return c32!=rhs.c32;}
};

