struct uv {
	float u, v;//,w,pad;
};

struct uvx : public uv {
	//uvx(float ua = 0, float va = 0, float wa = 0 ,float pada = 0) {u = ua;v = va;w = wa; pad = pada;}
	uvx(float ua = 0, float va = 0) {u = ua;v = va;}
};

struct face {
	S32 vertidx[3];
	S32 fmatidx;									// this is redundant with the group structure, but pads size to power of 2 and might be handy
};
typedef face FACE;

#define SMAT_HASTEX 1
#define SMAT_HASSHADE 2
#define SMAT_HASXLU 4 // set by mat trans
#define SMAT_HASWBUFF 8
#define SMAT_RENDERMASK 15
// other things that factor in is perscorrect and hascolorkey
#define SMAT_CALCLIGHTS 16
#define SMAT_ISSKY 64 // for setmodeldraworder, sets different matrix,and ignores back clipping, good for sky domes/cubes
// for hardware
#define SMAT_CLAMPU 128
#define SMAT_HASNOFOG 256
#define SMAT_NOALPHA 512 // don't use alpha blending at all
#define SMAT_FONT 1024 // special, merges alpha with matcol for colored fonts
#define SMAT_CLAMPV 2048
#define SMAT_CLAMP (SMAT_CLAMPU|SMAT_CLAMPV)
#define SMAT_SHADOW 4096 // special for cars, sets drawpri to 4
#define SMAT_POINTS 8192
#define SMAT_LINES 16384

void registertribackdrop(struct bitmap32* b32);	// where to draw tris (required)
void registermat(struct mater2* m);	// material to draw tris with (required)
void registerverts(struct pointf3* verts); // verts to draw tris with (required)
void registertriwb(struct bitmap32* wb); // wbuffer for tris (optional)
void registercverts(struct pointf3* cverts); // colored verts to draw tris with (optional)
void registeruvs(struct uv* uvs); // uv's to draw tris with (optional)
void drawtris(face *f,S32 ntris);
void drawtris(vector<face>& f,S32 foffset,S32 ntris);

// for reference
#if 0
//#define NAMESIZE 32 .. defined in nustring.h
//#define NAMESIZE 32
//#define MAT_HASTRANS 1
//#define MAT_USESLIGHTS 2
//#define MAT_HASENVMAP 4
//#define SMAT_CALCENVMAP 32


//#define SMAT_WRAPU 128
//#define SMAT_WRAPV 256
//#define SMAT_TEXVELS 512
//#define SMAT_HASIFLS 1024
//#define SMAT_LISAGUE 2048
//#define SMAT_NOFOG 4096

//#define MAT_HASCLAMP 16
//#define MAT_HASCVERTS 32
//#define MAT_MASK 0xff

//#define TEX_HAS4BALPHA 256
//#define TEX_HAS1BALPHA 512
//#define TEX_HAS565 1024	
//#define TEX_MASK 0xff00

/*struct lisague {
	float fu,fv; // frequencies
	float au,av; // amplitudes
	float ou; // offset time for u
	float px,py,pz; // position alterers
	float tu,tv;
};

struct ifl {
	S32 inited;
	float stepu,stepv;
	S32 curcount;
	S32 maxcount;
	S32 curframeu,curframev;
	S32 curframe;
	S32 maxframeu,maxframev;
	S32 maxframe;
};
*/
/*
struct mat {
// software (lo level)
//	S32 mflags;			// currently this is // 0 flat
											 // 1 if textured
											 // 2 if shaded
											 // 4 50% translucent tex
											 // 8 zbuffer 
// hardware (hi level)
	S32 msflags;		// hardware flags
	S32 msflags1;		// hardware flags
	S32 msflags2;		// hardware flags
	S32 msflags3;		// hardware flags
//	S32 mid;			// hardware texture id
	float mtrans;			// 0 xpar, 1 opaque
// texture info
//	S32 mlogw, mlogh;	// log2(w), log2(h); w & h must be powers of 2
	U8* reflecttexname;
	struct texture *thetex;
	struct texture *thetex1;
	struct texture *thetex2;
	struct texture *thetex3;
	struct uv texvel;
	struct uv texvel1;
	struct uv texvel2;
	struct uv texvel3;
//	struct lisague lis;
//	S32 tmaskx,tmasky;
//	struct bitmap16 *tex;
	string name;
//	C8 name[NAMESIZE];
// flat info
	struct pointf3 color;
// ifl info
//	struct ifl iflinfo;
};

*/
// non clipped tris 

//extern S32 perscorrect;
//typedef struct face FACE;
//typedef struct mat MAT;

// some globals
//extern S32 favorshading;
#endif
