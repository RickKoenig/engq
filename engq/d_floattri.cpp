#define D2_3D
// triangle routines (test)
#include <m_eng.h>
#include "m_perf.h"
//#define USEASM
#include "d2_software.h"
#define CULLBACK // don't draw backfaced tris

static U32 xlumask;
static struct bitmap32* ft_bg;
static struct bitmap32* ft_wb;
static struct bitmap32* ft_tex;	// texture data, assume a power of 2
static struct mater2* mat;
static struct pointf3* verts;
static struct pointf3* cverts;
static struct uv* uvs;
static struct pointf3* ovs[3]; // ordered verts
static U32 ft_texmasku,ft_texmaskv;//,ft_texmaskvs;
static U32 ft_texlogu;

static struct mat2 p2i;	// x,y to "unit"
static struct mat2 i2t; // unit to texture
static struct mat2 ft_p2t;	// points to texture

static struct pointf2 pdel;
static struct pointf2 tstart;
static struct pointf3 rgbstart;
static float wstart;

struct texedge {
	S32 left,right;
	float startu,startv;
	float w;
	float r,g,b;
};

#define MAXEDGE 4096
static struct texedge ft_texedgetable[MAXEDGE];
static S32 ft_yi[3];	// start/current and end y's (S32)
static struct face nf;
static struct texedge *et;
enum {TOP,MID,BOT};
static S32 topidx,mididx,botidx,startidx,si;
static struct uv safeuvs[3];
static U32 redmask,greenmask,bluemask;
static float redmul,greenmul,bluemul;
static float matxr,matxg,matxb,matyr,matyg,matyb;
static float ft_matxw;
static float matyw;
static C32 ft_C32color;

// matrix functions
static void nullfunc();
static void mfsetup();
static void mftex();
static void mfshade();
static void mfwbuf();
//static void sethicolor();
static void mfperstex();

// edge functions
static void lefitex();
static void lefrtex();
static void lefishade();
static void lefrshade();
static void lefiwbuf();
static void lefrwbuf();
static void lefiperstex();
// rendering functions
static void renderflattris();
static void renderflattrisw();
static void rendershadetris();
static void rendershadetrisw();
static void rendershadetrisxlu();
static void rendershadetriswxlu();
static void rendertextris();
static void rendertextrisfont(); // special for coloredfonts
static void rendertextrisw();
static void rendertexxlutris();
static void rendertexxlutrisw();

static void renderflattrisxlu();
static void renderflattriswxlu();

static void renderperstextris();
static void renderperstexxlutris();
static void renderperstextrisw();
static void renderperstexxlutrisw();


static void rendertextrisnck();
static void rendertextriswnck();
static void rendertexxlutrisnck();
static void rendertexxlutriswnck();

static void renderperstextrisnck();
static void renderperstexxlutrisnck();
static void renderperstextriswnck();
static void renderperstexxlutriswnck();

#ifdef USEASM
void renderflattrisa();
void renderperstextriswncka();
void rendertextriswncka();
void rendertextrisncka();
#endif

//SMAT_RENDERMASK
struct trifunc {
	S32 iserror;
	void (*matrixfunc0)();
	void (*matrixfunc1)();
	void (*matrixfunc2)();
	void (*ledgeinitfunc0)();
	void (*ledgeinitfunc1)();
	void (*ledgerepfunc0)();
	void (*ledgerepfunc1)();
	void (*renderfunc)();
};

static struct trifunc *trifuncptr;
static struct trifunc *slowshadetrifuncptr;
static struct trifunc *fastshadetrifuncptr;
// setup for font stuff
static struct trifunc fontfuncs={
//	0,nullfunc,nullfunc,nullfunc,
//		nullfunc,nullfunc,nullfunc,nullfunc,renderflattris, // who uses asm anymore?
	0,mfsetup,mftex,nullfunc,
		lefitex,nullfunc,lefrtex,nullfunc,rendertextrisfont,
};
// colorkey and nopers
static struct trifunc trifuncs[16]={
//  0 flat, sometimes not mentioned
	{0,nullfunc,nullfunc,nullfunc,
#ifdef USEASM
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisa},
#else
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattris},
#endif
//  1 tex
	{0,mfsetup,mftex,nullfunc,
		lefitex,nullfunc,lefrtex,nullfunc,rendertextris},
//  2 shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetris},
//  3 tex and shade, go with shade
	{1},
//  4 flat xlu
	{0,nullfunc,nullfunc,nullfunc,
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisxlu},
//  5 xlu and tex
	{0,mfsetup,mftex,nullfunc,
		lefitex,nullfunc,lefrtex,nullfunc,rendertexxlutris},
//  6 xlu and shade, go with shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetrisxlu},
//  7 xlu and shade and tex, go with shade
	{1},
//  8 flat and wbuff
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattrisw},
//  9 tex and wbuff
	{0,mfsetup,mftex,mfwbuf,
		lefitex,lefiwbuf,lefrtex,lefrwbuf,rendertextrisw},
// 10 shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetrisw},
// 11 tex and shade and wbuff, go with shade and wbuff
	{1},
// 12 xlu and wbuff, go with wbuff, no, this just in, a new renderfunction 'renderflattriswxlu'
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattriswxlu},
// 13 xlu and tex and wbuff
	{0,mfsetup,mftex,mfwbuf,
		lefitex,lefiwbuf,lefrtex,lefrwbuf,rendertexxlutrisw},
// 14 xlu and shade and wbuff, go with shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetriswxlu},
// 15 xlu and shade and tex and wbuff, go with shade and wbuff
	{1},
};

// colorkey and pers
static struct trifunc perstrifuncs[16]={
//  0 flat
	{0,nullfunc,nullfunc,nullfunc,
#ifdef USEASM
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisa},
#else
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattris},
#endif
//  1 tex
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstextris},
//  2 shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetris},
//  3 tex and shade, go with shade
	{1},
//  4 flat xlu
	{0,nullfunc,nullfunc,nullfunc,
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisxlu},
//  5 xlu and tex
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstexxlutris},
//  6 xlu and shade, go with shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetrisxlu},
//  7 xlu and shade and tex, go with shade
	{1},
//  8 flat and wbuff
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattrisw},
//  9 tex and wbuff
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstextrisw},
// 10 shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetrisw},
// 11 tex and shade and wbuff, go with shade and wbuff
	{1},
// 12 xlu and wbuff, go with wbuff, no, this just in, a new renderfunction 'renderflattriswxlu'
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattriswxlu},
// 13 xlu and tex and wbuff
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstexxlutrisw},
// 14 xlu and shade and wbuff, go with shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetriswxlu},
// 15 xlu and shade and tex and wbuff, go with shade and wbuff
	{1},
};

// nocolorkey and nopers
static struct trifunc trifuncsnck[16]={
//  0 flat, sometimes not mentioned
	{0,nullfunc,nullfunc,nullfunc,
#ifdef USEASM
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisa},
#else
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattris},
#endif
//  1 tex
	{0,mfsetup,mftex,nullfunc,
#ifdef USEASM
		lefitex,nullfunc,lefrtex,nullfunc,rendertextrisncka},
#else
		lefitex,nullfunc,lefrtex,nullfunc,rendertextrisnck},
#endif
//  2 shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetris},
//  3 tex and shade, go with shade
	{1},
//  4 flat xlu
	{0,nullfunc,nullfunc,nullfunc,
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisxlu},
//  5 xlu and tex
	{0,mfsetup,mftex,nullfunc,
		lefitex,nullfunc,lefrtex,nullfunc,rendertexxlutrisnck},
//  6 xlu and shade, go with shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetrisxlu},
//  7 xlu and shade and tex, go with shade
	{1},
//  8 flat and wbuff
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattrisw},
//  9 tex and wbuff
	{0,mfsetup,mftex,mfwbuf,
#ifdef USEASM
	lefitex,lefiwbuf,lefrtex,lefrwbuf,rendertextriswncka},
#else
	lefitex,lefiwbuf,lefrtex,lefrwbuf,rendertextriswnck},
#endif
// 10 shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetrisw},
// 11 tex and shade and wbuff, go with shade and wbuff
	{1},
// 12 xlu and wbuff, go with wbuff, no, this just in, a new renderfunction 'renderflattriswxlu'
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattriswxlu},
// 13 xlu and tex and wbuff
	{0,mfsetup,mftex,mfwbuf,
		lefitex,lefiwbuf,lefrtex,lefrwbuf,rendertexxlutriswnck},
// 14 xlu and shade and wbuff, go with shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetriswxlu},
// 15 xlu and shade and tex and wbuff, go with shade and wbuff
	{1},
};

// nocolorkey and pers
static struct trifunc perstrifuncsnck[16]={
//  0 flat
	{0,nullfunc,nullfunc,nullfunc,
#ifdef USEASM
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisa},
#else
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattris},
#endif
//  1 tex
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstextrisnck},
//  2 shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetris},
//  3 tex and shade, go with shade
	{1},
//  4 flat xlu
	{0,nullfunc,nullfunc,nullfunc,
		nullfunc,nullfunc,nullfunc,nullfunc,renderflattrisxlu},
//  5 xlu and tex
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstexxlutrisnck},
//  6 xlu and shade, go with shade
	{0,mfsetup,mfshade,nullfunc,
		lefishade,nullfunc,lefrshade,nullfunc,rendershadetrisxlu},
//  7 xlu and shade and tex, go with shade
	{1},
//  8 flat and wbuff
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattrisw},
//  9 tex and wbuff
	{0,mfsetup,mfwbuf,mfperstex,
#ifdef USEASM
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstextriswncka},
#else
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstextriswnck},
#endif
// 10 shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetrisw},
// 11 tex and shade and wbuff, go with shade and wbuff
	{1},
// 12 xlu and wbuff, go with wbuff, no, this just in, a new renderfunction 'renderflattriswxlu'
	{0,mfsetup,nullfunc,mfwbuf,
		lefiwbuf,nullfunc,lefrwbuf,nullfunc,renderflattriswxlu},
// 13 xlu and tex and wbuff
	{0,mfsetup,mfwbuf,mfperstex,
		lefiwbuf,lefiperstex,lefrwbuf,lefrtex,renderperstexxlutriswnck},
// 14 xlu and shade and wbuff, go with shade and wbuff
	{0,mfsetup,mfshade,mfwbuf,
		lefishade,lefiwbuf,lefrshade,lefrwbuf,rendershadetriswxlu},
// 15 xlu and shade and tex and wbuff, go with shade and wbuff
	{1},
};

void registertribackdrop(struct bitmap32 *b32) // ,S32 type)	// where to draw tris (required)
{
	ft_bg=b32;
	xlumask=0x7f7f7f;
	redmask=0xff0000;
	greenmask=0xff00;
	bluemask=0xff;
	redmul=256*256*256;
	greenmul=256*256;
	bluemul=256;
}

void registertriwb(struct bitmap32* wbu) // wbuffer for tris (optional)
{
	ft_wb=wbu;
}

void registermat(mater2 *m)	// material to draw tris with (required)
{
	S32 rendertype,fastrendertype; // fastrendertype is used when the 3 verts all have the same color (disable shade)
	S32 nck=0;
	mat=m;
	rendertype=mat->msflags & SMAT_RENDERMASK;
	texture2 *t=0;
	if (m->thetexarr[0])
		t=dynamic_cast<texture2*>(m->thetexarr[0]->snapshot());
//	if (!(rendertype&SMAT_HASTEX))
		ft_C32color=pointf3toC32(&mat->color);
	if (t && t->texdata) {
		ft_tex=t->texdata;
		ft_texmasku=ft_tex->size.x-1;
		ft_texmaskv=ft_tex->size.y-1;
		ft_texlogu=t->logu;
		if (t->texformat==TEX32NCK)
			nck=1;
		if (m->msflags&SMAT_NOALPHA)
			nck=1;
	} else
		rendertype&=~SMAT_HASTEX;
	if ((mat->msflags&SMAT_FONT) && t && t->texdata) { // special override for 'fonts'
//		logger("software font found\n");
		fastshadetrifuncptr=slowshadetrifuncptr=&fontfuncs;
		return;
	}
	if (video3dinfo.zenable==1)
		rendertype|=SMAT_HASWBUFF;
	else if (!video3dinfo.zenable)
		rendertype&=~SMAT_HASWBUFF;
// can't have shaded textures
	if ((rendertype & (SMAT_HASSHADE | SMAT_HASTEX))==(SMAT_HASSHADE | SMAT_HASTEX)) {
		if (video3dinfo.favorshading)
			rendertype&=~SMAT_HASTEX;
		else
			rendertype&=~SMAT_HASSHADE;
	}
	fastrendertype=rendertype;
// setup for slow shaded triangles first
	if (video3dinfo.perscorrect)
		if (nck)
			slowshadetrifuncptr=&perstrifuncsnck[rendertype];
		else
			slowshadetrifuncptr=&perstrifuncs[rendertype];
	else
		if (nck)
			slowshadetrifuncptr=&trifuncsnck[rendertype];
		else
			slowshadetrifuncptr=&trifuncs[rendertype];
	if (slowshadetrifuncptr->iserror)
		errorexit("slow shade bad software tri render mode\n");
// setup for fast shaded triangles next (all verts same color)
	if (fastrendertype&SMAT_HASSHADE)
		fastrendertype&=~SMAT_HASSHADE; // eliminate shade
	if (video3dinfo.perscorrect)
		if (nck)
			fastshadetrifuncptr=&perstrifuncsnck[fastrendertype];
		else
			fastshadetrifuncptr=&perstrifuncs[fastrendertype];
	else
		if (nck)
			fastshadetrifuncptr=&trifuncsnck[fastrendertype];
		else
			fastshadetrifuncptr=&trifuncs[fastrendertype];
	if (fastshadetrifuncptr->iserror)
		errorexit("fast shade bad software tri render mode\n");
}

void registerverts(struct pointf3 *v) // verts to draw tris with (required)
{
	verts=v;
}

void registercverts(struct pointf3 *cv) // colored verts to draw tris with (optional)
{
	cverts=cv;
}

void registeruvs(struct uv *_uvs) // uv's to draw tris with (optional)
{
	uvs=_uvs;
}

// matrix functions
static void nullfunc()
{
}

static void mfsetup()
{
//	S32 i;
	struct mat2 i2p;
	perf_start(MFSETUP);
//	logger("verts\n");
//	for (i=0;i<8;++i)
//		logger("%d (%f,%f,%f)\n",i,verts[i].x,verts[i].y,verts[i].z);
	i2p.e[0][0]=verts[mididx].x-verts[topidx].x;
	i2p.e[0][1]=verts[mididx].y-verts[topidx].y;
	i2p.e[1][0]=verts[botidx].x-verts[topidx].x;
	i2p.e[1][1]=verts[botidx].y-verts[topidx].y;
	if (!inversemat2(&i2p,&p2i)) {
//		errorexit("no p2i");
		p2i.e[0][0]=0;
		p2i.e[0][1]=0;
		p2i.e[1][0]=0;
		p2i.e[1][1]=0;
	}
	perf_end(MFSETUP);
//	logmat2(&i2p,"i2p");
//	logmat2(&p2i,"p2i");
}

static void mfperstex()
{
#define W0 1.0f
//#define W0 .999f
#define W1 (1.0f-W0)
#define W1_3 (W1/3.0f)
	struct uv miduv;
	perf_start(MFPERSTEX);
	miduv.u=(uvs[topidx].u+uvs[mididx].u+uvs[botidx].u)*W1_3;
	miduv.v=(uvs[topidx].v+uvs[mididx].v+uvs[botidx].v)*W1_3;
	safeuvs[TOP].u=uvs[topidx].u*W0+miduv.u;
	safeuvs[TOP].v=uvs[topidx].v*W0+miduv.v;
	safeuvs[MID].u=uvs[mididx].u*W0+miduv.u;
	safeuvs[MID].v=uvs[mididx].v*W0+miduv.v;
	safeuvs[BOT].u=uvs[botidx].u*W0+miduv.u;
	safeuvs[BOT].v=uvs[botidx].v*W0+miduv.v;
	i2t.e[0][0]=(safeuvs[MID].u*verts[mididx].z-safeuvs[TOP].u*verts[topidx].z)*ft_tex->size.x;
	i2t.e[0][1]=(safeuvs[MID].v*verts[mididx].z-safeuvs[TOP].v*verts[topidx].z)*ft_tex->size.y;
	i2t.e[1][0]=(safeuvs[BOT].u*verts[botidx].z-safeuvs[TOP].u*verts[topidx].z)*ft_tex->size.x;
	i2t.e[1][1]=(safeuvs[BOT].v*verts[botidx].z-safeuvs[TOP].v*verts[topidx].z)*ft_tex->size.y;
//	i2t.e[0][0]=(uvs[mididx].u-uvs[topidx].u)*tex->x;
//	i2t.e[0][1]=(uvs[mididx].v-uvs[topidx].v)*tex->y;
//	i2t.e[1][0]=(uvs[botidx].u-uvs[topidx].u)*tex->x;
//	i2t.e[1][1]=(uvs[botidx].v-uvs[topidx].v)*tex->y;
	mulmat2(&p2i,&i2t,&ft_p2t);
	perf_end(MFPERSTEX);
}

static void mftex()
{
	struct uv miduv;
	perf_start(MFTEX);
	miduv.u=(uvs[topidx].u+uvs[mididx].u+uvs[botidx].u)*W1_3;
	miduv.v=(uvs[topidx].v+uvs[mididx].v+uvs[botidx].v)*W1_3;
	safeuvs[TOP].u=uvs[topidx].u*W0+miduv.u;
	safeuvs[TOP].v=uvs[topidx].v*W0+miduv.v;
	safeuvs[MID].u=uvs[mididx].u*W0+miduv.u;
	safeuvs[MID].v=uvs[mididx].v*W0+miduv.v;
	safeuvs[BOT].u=uvs[botidx].u*W0+miduv.u;
	safeuvs[BOT].v=uvs[botidx].v*W0+miduv.v;
	i2t.e[0][0]=(safeuvs[MID].u-safeuvs[TOP].u)*ft_tex->size.x;
	i2t.e[0][1]=(safeuvs[MID].v-safeuvs[TOP].v)*ft_tex->size.y;
	i2t.e[1][0]=(safeuvs[BOT].u-safeuvs[TOP].u)*ft_tex->size.x;
	i2t.e[1][1]=(safeuvs[BOT].v-safeuvs[TOP].v)*ft_tex->size.y;
//	i2t.e[0][0]=(uvs[mididx].u-uvs[topidx].u)*tex->x;
//	i2t.e[0][1]=(uvs[mididx].v-uvs[topidx].v)*tex->y;
//	i2t.e[1][0]=(uvs[botidx].u-uvs[topidx].u)*tex->x;
//	i2t.e[1][1]=(uvs[botidx].v-uvs[topidx].v)*tex->y;
	mulmat2(&p2i,&i2t,&ft_p2t);
	perf_end(MFTEX);
}

static void mfshade()
{
	float mat0r,mat0g,mat0b,mat1r,mat1g,mat1b;
	perf_start(MFSHADE);
	mat0r=(cverts[mididx].x-cverts[topidx].x)*redmul;
	mat0g=(cverts[mididx].y-cverts[topidx].y)*greenmul;
	mat0b=(cverts[mididx].z-cverts[topidx].z)*bluemul;
	mat1r=(cverts[botidx].x-cverts[topidx].x)*redmul;
	mat1g=(cverts[botidx].y-cverts[topidx].y)*greenmul;
	mat1b=(cverts[botidx].z-cverts[topidx].z)*bluemul;

	matxr=p2i.e[0][0]*mat0r+p2i.e[0][1]*mat1r;
	matxg=p2i.e[0][0]*mat0g+p2i.e[0][1]*mat1g;
	matxb=p2i.e[0][0]*mat0b+p2i.e[0][1]*mat1b;
	matyr=p2i.e[1][0]*mat0r+p2i.e[1][1]*mat1r;
	matyg=p2i.e[1][0]*mat0g+p2i.e[1][1]*mat1g;
	matyb=p2i.e[1][0]*mat0b+p2i.e[1][1]*mat1b;
	perf_end(MFSHADE);
}

static void mfwbuf()
{
	float mat0w,mat1w;
	perf_start(MFWBUF);
	mat0w=verts[mididx].z-verts[topidx].z;
	mat1w=verts[botidx].z-verts[topidx].z;
	ft_matxw=p2i.e[0][0]*mat0w+p2i.e[0][1]*mat1w;
	matyw=p2i.e[1][0]*mat0w+p2i.e[1][1]*mat1w;
	perf_end(MFWBUF);
//	logger("x2w %f, y2w %f\n",ft_matxw,matyw);
}

// edge functions
static void setleftgenedge(S32 s,S32 f,float im)
{
	S32 xinc,yinc,yend; // test for the failed F2INT
	float xf;
	perf_start(SETLEFTGENEDGE);
	yinc=ft_yi[s];
	yend=ft_yi[f];
//	if (yinc<0)
//		logger("bad yinc %d\n",yinc);
	et=&ft_texedgetable[yinc];
	si=s;
	s=nf.vertidx[s];
	startidx=s;
	pdel.y=(yinc+.5f)-verts[s].y;
	xf=verts[s].x+im*pdel.y;
	xf-=.5f;
//	xf=12345678.0f;
//	xinc=0;
	F2INT(xf,xinc);
//	xinc=(S32)xf;
//	logger("f2S32 of %f is %d\n",xf,xinc);
//	exit(0);
//	xinc=f2S32(xf);
	pdel.x=(xinc+.5f)-verts[s].x;
	(trifuncptr->ledgeinitfunc0)();
	(trifuncptr->ledgeinitfunc1)();
//	dely=1; // implied
	while(yinc<yend) {
//		et->left=f2S32(xf);
		et->left=xinc;
		xf+=im;
		F2INT(xf,xinc);
//		xinc=f2S32(xf);
		pdel.x=(float)(xinc-et->left);
//		logger("xf %f, im %f, xinc %d, pdel.x %f\n",xf,im,xinc,pdel.x);
//		xinc=et->left;
//		logger("yinc = %d\n",yinc);
		(trifuncptr->ledgerepfunc0)();
		(trifuncptr->ledgerepfunc1)();
		yinc++;
		et++;
	}
	perf_end(SETLEFTGENEDGE);
}

static void lefiperstex()
{
	struct uv topuv;
	topuv.u=ft_tex->size.x*safeuvs[si].u*verts[startidx].z;
	topuv.v=ft_tex->size.y*safeuvs[si].v*verts[startidx].z;
	mulmatvec2(&ft_p2t,&pdel,&tstart);
	tstart.x+=topuv.u;
	tstart.y+=topuv.v;
//	tstart.x-=1.0f;
//	tstart.y-=1.0f;
//	tstart.x-=1.0f/wstart;
//	tstart.y-=1.0f/wstart;
}

static void lefitex()
{
	struct uv topuv;
	topuv.u=ft_tex->size.x*safeuvs[si].u;
	topuv.v=ft_tex->size.y*safeuvs[si].v;
	mulmatvec2(&ft_p2t,&pdel,&tstart);
	tstart.x+=topuv.u;
	tstart.y+=topuv.v;
	tstart.x-=1.0f;
	tstart.y-=1.0f;
}

static void lefrtex()
{
	et->startu=tstart.x;
	et->startv=tstart.y;
//	mulmatvec2(&p2t,&pdel,&tdel);
	tstart.x+=ft_p2t.e[0][0]*pdel.x;//tdel.x;
	tstart.y+=ft_p2t.e[0][1]*pdel.x;//tdel.y;
	tstart.x+=ft_p2t.e[1][0];//tdel.x;
	tstart.y+=ft_p2t.e[1][1];//tdel.y;
}

static void lefishade()
{
	struct pointf3 toprgb;
	toprgb.x=redmul*cverts[startidx].x;
	toprgb.y=greenmul*cverts[startidx].y;
	toprgb.z=bluemul*cverts[startidx].z;
	rgbstart.x=matxr*pdel.x+matyr*pdel.y;
	rgbstart.y=matxg*pdel.x+matyg*pdel.y;
	rgbstart.z=matxb*pdel.x+matyb*pdel.y;
	rgbstart.x+=toprgb.x;
	rgbstart.y+=toprgb.y;
	rgbstart.z+=toprgb.z;
//	rgbstart.x-=.5f;
//	rgbstart.y-=.5f;
//	rgbstart.z-=.5f;
}

static void lefrshade()
{
	et->r=rgbstart.x;
	et->g=rgbstart.y;
	et->b=rgbstart.z;
	rgbstart.x+=matxr*pdel.x;//tdel.x;
	rgbstart.y+=matxg*pdel.x;//tdel.y;
	rgbstart.z+=matxb*pdel.x;//tdel.y;
	rgbstart.x+=matyr;//tdel.x;
	rgbstart.y+=matyg;//tdel.y;
	rgbstart.z+=matyb;//tdel.y;
}

static void lefiwbuf()
{
	float topw;
	topw=verts[startidx].z;
	wstart=ft_matxw*pdel.x+matyw*pdel.y;
	wstart+=topw;
//	logger("wstart i %f\n",wstart);
}

static void lefrwbuf()
{
	et->w=wstart;
	wstart+=ft_matxw*pdel.x;//tdel.x;
	wstart+=matyw;//tdel.x;
//	logger("wstart r %f, ft_matx %f, pdel.x %f, matyw %f\n",wstart,ft_matxw,pdel.x,matyw);
}

static void setrightgenedge(S32 s,S32 f,float im)
{
//	struct texedge *et;
	S32 yinc,yend;
	float xf;
//	float dely;
	perf_start(SETRIGHTGENEDGE);
	yinc=ft_yi[s];
	yend=ft_yi[f];
	et=&ft_texedgetable[yinc];
	s=nf.vertidx[s];
	pdel.y=(yinc+.5f)-verts[s].y;
	xf=verts[s].x+im*pdel.y;
	xf-=.5f;
	while(yinc<yend) {
		F2INT(xf,et->right);
//		et->right=f2S32(xf);
		xf+=im;
		yinc++;
		et++;
	}
	perf_end(SETRIGHTGENEDGE);
}

// rendering functions
//just a start on it, just to see how it works
static void renderperstextris()
{
	C32* sp,*dp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel,newtdel;
	perf_start(RENDERPERSTEX);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			S32 shx,shy;
			fzval=1.0f/fwval;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			F2INT(newtdel.x,shx);
			F2INT(newtdel.y,shy);
			color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//			color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
			if (color.a)
				dp[xinc]=color;
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEX);
}

static void renderperstextrisnck()
{
	C32* sp,*dp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel,newtdel;
	perf_start(RENDERPERSTEXNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			S32 shx,shy;
			fzval=1.0f/fwval;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			F2INT(newtdel.x,shx);
			F2INT(newtdel.y,shy);
			color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//			color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
			dp[xinc]=color;
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXNCK);
}

static void renderperstexxlutris()
{
	C32* sp,*dp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color,color2;
	struct pointf2 tdel,newtdel;
	perf_start(RENDERPERSTEXXLU);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			S32 shx,shy;
			fzval=1.0f/fwval;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			F2INT(newtdel.x,shx);
			F2INT(newtdel.y,shy);
			color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//			color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
			if (color.a) {
				color.c32>>=1;
				color.c32&=xlumask;
				color2=dp[xinc];
				color2.c32>>=1;
				color2.c32&=xlumask;
				color2.c32+=color.c32;
				dp[xinc]=color2;
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXXLU);
}

static void renderperstexxlutrisnck()
{
	C32* sp,*dp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color,color2;
	struct pointf2 tdel,newtdel;
	perf_start(RENDERPERSTEXXLUNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			S32 shx,shy;
			fzval=1.0f/fwval;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			F2INT(newtdel.x,shx);
			F2INT(newtdel.y,shy);
			color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//			color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
			color.c32>>=1;
			color.c32&=xlumask;
			color2=dp[xinc];
			color2.c32>>=1;
			color2.c32&=xlumask;
			color2.c32+=color.c32;
			dp[xinc]=color2;
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXXLUNCK);
}

static void renderperstextrisw()
{
	C32* sp,*dp,*wp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend,wval,wread;
	C32 color;
	struct pointf2 tdel,newtdel;
	perf_start(RENDERPERSTEXW);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			fzval=1.0f/fwval;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
			wread=wp[xinc].c32;
			if (wval>wread) {
				S32 shx,shy;
				F2INT(newtdel.x,shx);
				F2INT(newtdel.y,shy);
				color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//				color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
				if (color.a) {
					dp[xinc]=color;
					wp[xinc].c32=wval;
//				} else {
//					dp[xinc]=C32GREEN;
//					wp[xinc].c32=wval;
				}
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXW);
}

static void renderperstextriswnck()
{
	C32* sp,*dp,*wp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend,wval,wread;
	C32 color;
	struct pointf2 tdel,newtdel;
//	return;
	perf_start(RENDERPERSTEXWNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			fzval=1.0f/fwval;
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
			wread=wp[xinc].c32;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			if (wval>wread) {
				S32 shx,shy;
				F2INT(newtdel.x,shx);
				F2INT(newtdel.y,shy);
				color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//				color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
				dp[xinc]=color;
				wp[xinc].c32=wval;
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXWNCK);
}

static void renderperstexxlutrisw()
{
	C32* sp,*dp,*wp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend,wval,wread;
	C32 color;
	struct pointf2 tdel,newtdel;
	perf_start(RENDERPERSTEXWXLU);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			fzval=1.0f/fwval;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
			wread=wp[xinc].c32;
			if (wval>wread) {
				S32 shx,shy;
				F2INT(newtdel.x,shx);
				F2INT(newtdel.y,shy);
				color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//				color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
				if (color.a) {
					C32 color2;
					color.c32>>=1;
					color.c32&=xlumask;
					color2=dp[xinc];
					color2.c32>>=1;
					color2.c32&=xlumask;
					color2.c32+=color.c32;
					dp[xinc]=color2;
					wp[xinc].c32=wval;
				}
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXWXLU);
}

static void renderperstexxlutriswnck()
{
	C32* sp,*dp,*wp;
	float fwval,fzval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend,wval,wread;
	C32 color;
	struct pointf2 tdel,newtdel;
//	return;
	perf_start(RENDERPERSTEXWXLUNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			fzval=1.0f/fwval;
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
			wread=wp[xinc].c32;
			newtdel.x=tdel.x*fzval; // these 2 lines do produce pers corr textures..
			newtdel.y=tdel.y*fzval;
			if (wval>wread) {
				C32 color2;
				S32 shx,shy;
				F2INT(newtdel.x,shx);
				F2INT(newtdel.y,shy);
				color=sp[((shx-1)&ft_texmasku)+(((shy-1)&ft_texmaskv)<<ft_texlogu)];
//				color=sp[((f2S32(newtdel.x)-1)&ft_texmasku)+(((f2S32(newtdel.y)-1)&ft_texmaskv)<<ft_texlogu)];
				color.c32>>=1;
				color.c32&=xlumask;
				color2=dp[xinc];
				color2.c32>>=1;
				color2.c32&=xlumask;
				color2.c32+=color.c32;
				dp[xinc]=color2;
				wp[xinc].c32=wval;
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERPERSTEXWXLUNCK);
}

static void renderflattris()
{
// flat tri
//	struct texedge *et;
	C32* dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	perf_start(RENDERFLAT);
	et=&ft_texedgetable[yinc];
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		while (xinc<xend) {
			dp[xinc]=ft_C32color;
			xinc++;
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERFLAT);
}


static void renderflattrisxlu()
{
// flat tri
//	struct texedge *et;
	C32* dp;
	C32 fshift;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	perf_start(RENDERFLATXLU);
	fshift.c32=(ft_C32color.c32>>1)&xlumask;
	et=&ft_texedgetable[yinc];
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		while (xinc<xend) {
			C32 color2=dp[xinc];
			color2.c32>>=1;
			color2.c32&=xlumask;
			color2.c32+=fshift.c32;
			dp[xinc]=color2;
//			wp[xinc].c32=wval;
			xinc++;
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERFLATXLU);
}

static void renderflattrisw()
{
// flat tri
//	struct texedge *et;
	C32 *dp,*wp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	U32 wval,wread;
	float fwval;
	perf_start(RENDERFLATW);
	et=&ft_texedgetable[yinc];
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			logger("%f %d\n",fwval,wval);
//			wval=f2S32(fwval);
			wread=wp[xinc].c32;
//			wread=clipgetpixel16(wb,xinc,yinc);
			if (wval>wread) {
//				clipputpixel16(ft_bg,xinc,yinc,hicolor);
//				clipputpixel16(wb,xinc,yinc,wval);
//				dp[xinc]=wval;
				dp[xinc]=ft_C32color;
//				dp[xinc].c32=wval;
				wp[xinc].c32=wval;
			}
			xinc++;
			fwval+=ft_matxw;
		}
		yinc++;
		wp+=ft_wb->size.x;
		dp+=ft_bg->size.x;
		et++;
	}
	perf_end(RENDERFLATW);
}

static void renderflattriswxlu()
{
// flat tri
//	struct texedge *et;
	C32 *dp,*wp;
	C32 fshift;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	U32 wval,wread;
	float fwval;
	perf_start(RENDERFLATWXLU);
	fshift.c32=(ft_C32color.c32>>1)&xlumask;
	et=&ft_texedgetable[yinc];
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			logger("%f %d\n",fwval,wval);
//			wval=f2S32(fwval);
			wread=wp[xinc].c32;
//			wread=clipgetpixel16(wb,xinc,yinc);
			if (wval>wread) {
				C32 color2=dp[xinc];
//				clipputpixel16(ft_bg,xinc,yinc,hicolor);
//				clipputpixel16(wb,xinc,yinc,wval);
//				dp[xinc]=wval;
//				dp[xinc]=ft_C32color;
//				dp[xinc].c32=wval;


				color2.c32>>=1;
				color2.c32&=xlumask;
				color2.c32+=fshift.c32;
				dp[xinc]=color2;

				wp[xinc].c32=wval;
			}
			xinc++;
			fwval+=ft_matxw;
		}
		yinc++;
		wp+=ft_wb->size.x;
		dp+=ft_bg->size.x;
		et++;
	}
	perf_end(RENDERFLATWXLU);
}

static void rendershadetris()
{
	C32* dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf3 rgbdel;
	perf_start(RENDERSHADE);
	setfpuroundprec(FPUCW_RND_DOWN,FPUCW_PREC_32);
//	setfpucontrol(fpucw&0xf0ff | (RND_DOWN<<10) | (PREC_32<<8));
	et=&ft_texedgetable[yinc];
//	p2t.e[0][0]*=.9999f; // for right edge
//	p2t.e[0][1]*=.9999f;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		rgbdel.x=et->r;
		rgbdel.y=et->g;
		rgbdel.z=et->b;
		while (xinc<xend) {
			S32 ri,gi,bi;
			F2INT(rgbdel.x,ri);
			F2INT(rgbdel.y,gi);
			F2INT(rgbdel.z,bi);
			color.c32=(ri&redmask)+(gi&greenmask)+(bi&bluemask);
//			color=(f2S32(rgbdel.x)&redmask)+(f2S32(rgbdel.y)&greenmask)+(f2S32(rgbdel.z)&bluemask);
//			clipputpixel16(ft_bg,xinc,yinc,color);
			dp[xinc]=color;
			xinc++;
			rgbdel.x+=matxr;
			rgbdel.y+=matxg;
			rgbdel.z+=matxb;
		}
		yinc++;
		dp+=ft_bg->size.x;
		et++;
	}
	setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
//	setfpucontrol(fpucw&0xf0ff | (RND_UP<<10) | (PREC_32<<8));
	perf_end(RENDERSHADE);
}

static void rendershadetrisxlu()
{
	C32* dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf3 rgbdel;
	perf_start(RENDERSHADEXLU);
	setfpuroundprec(FPUCW_RND_DOWN,FPUCW_PREC_32);
//	setfpucontrol(fpucw&0xf0ff | (RND_DOWN<<10) | (PREC_32<<8));
	et=&ft_texedgetable[yinc];
//	p2t.e[0][0]*=.9999f; // for right edge
//	p2t.e[0][1]*=.9999f;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		rgbdel.x=et->r;
		rgbdel.y=et->g;
		rgbdel.z=et->b;
		while (xinc<xend) {
			C32 color2;
			S32 ri,gi,bi;
			F2INT(rgbdel.x,ri);
			F2INT(rgbdel.y,gi);
			F2INT(rgbdel.z,bi);
			color.c32=(ri&redmask)+(gi&greenmask)+(bi&bluemask);
//			color=(f2S32(rgbdel.x)&redmask)+(f2S32(rgbdel.y)&greenmask)+(f2S32(rgbdel.z)&bluemask);
//			clipputpixel16(ft_bg,xinc,yinc,color);

			color.c32>>=1;
			color.c32&=xlumask;
			color2=dp[xinc];
			color2.c32>>=1;
			color2.c32&=xlumask;
			color2.c32+=color.c32;
			dp[xinc]=color2;

			xinc++;
			rgbdel.x+=matxr;
			rgbdel.y+=matxg;
			rgbdel.z+=matxb;
		}
		yinc++;
		dp+=ft_bg->size.x;
		et++;
	}
	setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
//	setfpucontrol(fpucw&0xf0ff | (RND_UP<<10) | (PREC_32<<8));
	perf_end(RENDERSHADEXLU);
}

static void rendershadetrisw()
{
	C32* dp,*wp;
	U32 wval,wread;
	float fwval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf3 rgbdel;
	perf_start(RENDERSHADEW);
	et=&ft_texedgetable[yinc];
//	p2t.e[0][0]*=.9999f; // for right edge
//	p2t.e[0][1]*=.9999f;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		rgbdel.x=et->r;
		rgbdel.y=et->g;
		rgbdel.z=et->b;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
//			wread=clipgetpixel16(wb,xinc,yinc);
			wread=wp[xinc].c32;
			if (wval>wread) {
//				setfpucontrol(fpucw&0xf0ff | (RND_DOWN<<10) | (PREC_32<<8));
				setfpuroundprec(FPUCW_RND_DOWN,FPUCW_PREC_32);
				S32 ri,gi,bi;
				F2INT(rgbdel.x,ri);
				F2INT(rgbdel.y,gi);
				F2INT(rgbdel.z,bi);
				color.c32=(ri&redmask)+(gi&greenmask)+(bi&bluemask);
//				color=(f2S32(rgbdel.x)&redmask)+(f2S32(rgbdel.y)&greenmask)+(f2S32(rgbdel.z)&bluemask);
//				clipputpixel16(ft_bg,xinc,yinc,color);
//				clipputpixel16(wb,xinc,yinc,wval);
				setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
//				setfpucontrol(fpucw&0xf0ff | (RND_UP<<10) | (PREC_32<<8));
				wp[xinc].c32=wval;
				dp[xinc]=color;
			}
			xinc++;
			rgbdel.x+=matxr;
			rgbdel.y+=matxg;
			rgbdel.z+=matxb;
			fwval+=ft_matxw;
		}
		yinc++;
		wp+=ft_wb->size.x;
		dp+=ft_bg->size.x;
		et++;
	}
	perf_end(RENDERSHADEW);
}

static void rendershadetriswxlu()
{
	C32* dp,*wp;
	U32 wval,wread;
	float fwval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf3 rgbdel;
	perf_start(RENDERSHADEWXLU);
	et=&ft_texedgetable[yinc];
//	p2t.e[0][0]*=.9999f; // for right edge
//	p2t.e[0][1]*=.9999f;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		rgbdel.x=et->r;
		rgbdel.y=et->g;
		rgbdel.z=et->b;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
//			wread=clipgetpixel16(wb,xinc,yinc);
			wread=wp[xinc].c32;
			if (wval>wread) {
				C32 color2;
//				setfpucontrol(fpucw&0xf0ff | (RND_DOWN<<10) | (PREC_32<<8));
				setfpuroundprec(FPUCW_RND_DOWN,FPUCW_PREC_32);
				S32 ri,gi,bi;
				F2INT(rgbdel.x,ri);
				F2INT(rgbdel.y,gi);
				F2INT(rgbdel.z,bi);
				color.c32=(ri&redmask)+(gi&greenmask)+(bi&bluemask);
//				color=(f2S32(rgbdel.x)&redmask)+(f2S32(rgbdel.y)&greenmask)+(f2S32(rgbdel.z)&bluemask);
//				clipputpixel16(ft_bg,xinc,yinc,color);
//				clipputpixel16(wb,xinc,yinc,wval);
				setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
//				setfpucontrol(fpucw&0xf0ff | (RND_UP<<10) | (PREC_32<<8));
				color.c32>>=1;
				color.c32&=xlumask;
				color2=dp[xinc];
				color2.c32>>=1;
				color2.c32&=xlumask;
				color2.c32+=color.c32;
				dp[xinc]=color2;
				wp[xinc].c32=wval;
			}
			xinc++;
			rgbdel.x+=matxr;
			rgbdel.y+=matxg;
			rgbdel.z+=matxb;
			fwval+=ft_matxw;
		}
		yinc++;
		wp+=ft_wb->size.x;
		dp+=ft_bg->size.x;
		et++;
	}
	perf_end(RENDERSHADEWXLU);
}

static void rendertextris()
{
	C32 *sp,*dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEX);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		while (xinc<xend) {
			S32 shx,shy;
			F2INT(tdel.x,shx);
			F2INT(tdel.y,shy);
			color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//			color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
			if (color.a)
				dp[xinc]=color;
//			color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//			clipputpixel16(ft_bg,xinc,yinc,color);
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEX);
}

static void rendertextrisfont()
{
	C32 *sp,*dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEX);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		while (xinc<xend) {
			S32 shx,shy;
			F2INT(tdel.x,shx);
			F2INT(tdel.y,shy);
			color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//			color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
			if (color.a)
				dp[xinc]=ft_C32color;
//			color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//			clipputpixel16(ft_bg,xinc,yinc,color);
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEX);
}

static void rendertextrisnck()
{
	C32* sp,*dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		while (xinc<xend) {
			S32 shx,shy;
			F2INT(tdel.x,shx);
			F2INT(tdel.y,shy);
			color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//			color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
			dp[xinc]=color;
//			color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//			clipputpixel16(ft_bg,xinc,yinc,color);
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXNCK);
}

static void rendertextrisw()
{
	C32* sp,*dp,*wp;
	U32 wval,wread;
	float fwval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXW);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	//if (yinc == 383)
	//	logger("yinc = %d\n",yinc);
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
//			wread=clipgetpixel16(wb,xinc,yinc);
			wread=wp[xinc].c32;
			if (wval>wread) {
//				color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//				clipputpixel16(ft_bg,xinc,yinc,color);
//				clipputpixel16(wb,xinc,yinc,wval);
				S32 shx,shy;
				F2INT(tdel.x,shx);
				F2INT(tdel.y,shy);
				color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//				color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
				if (color.a) {
					dp[xinc]=color;
					//dp[xinc]=C32(255,255,255,255);
					wp[xinc].c32=wval;
					//logger("xinc = %d\n",xinc);
				}
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXW);
}

static void rendertextriswnck()
{
	C32 *sp,*dp,*wp;
	U32 wval,wread;
	float fwval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXWNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
//			wread=clipgetpixel16(wb,xinc,yinc);
			wread=wp[xinc].c32;
			if (wval>wread) {
//				color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//				clipputpixel16(ft_bg,xinc,yinc,color);
//				clipputpixel16(wb,xinc,yinc,wval);
				S32 shx,shy;
				F2INT(tdel.x,shx);
				F2INT(tdel.y,shy);
				color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//				color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
				dp[xinc]=color;
				wp[xinc].c32=wval;
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXWNCK);
}

static void rendertexxlutris()
{
	C32 *sp,*dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXXLU);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		while (xinc<xend) {
			S32 shx,shy;
			F2INT(tdel.x,shx);
			F2INT(tdel.y,shy);
			color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//			color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
			if (color.a) {
				C32 color2;
				color.c32>>=1;
				color.c32&=xlumask;
				color2=dp[xinc];
				color2.c32>>=1;
				color2.c32&=xlumask;
				color2.c32+=color.c32;
				dp[xinc]=color2;
			}
//			color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//			clipputpixel16(ft_bg,xinc,yinc,color);
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXXLU);
}

static void rendertexxlutrisnck()
{
	C32 *sp,*dp;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXXLUNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		while (xinc<xend) {
			C32 color2;
			S32 shx,shy;
			F2INT(tdel.x,shx);
			F2INT(tdel.y,shy);
			color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//			color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
			color.c32>>=1;
			color.c32&=xlumask;
			color2=dp[xinc];
			color2.c32>>=1;
			color2.c32&=xlumask;
			color2.c32+=color.c32;
			dp[xinc]=color2;
//			color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//			clipputpixel16(ft_bg,xinc,yinc,color);
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
		}
		dp+=ft_bg->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXXLUNCK);
}

static void rendertexxlutrisw()
{
	C32* sp,*dp,*wp;
	U32 wval,wread;
	float fwval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXWXLU);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
//			wread=clipgetpixel16(wb,xinc,yinc);
			wread=wp[xinc].c32;
			if (wval>wread) {
//				color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//				clipputpixel16(ft_bg,xinc,yinc,color);
//				clipputpixel16(wb,xinc,yinc,wval);
				S32 shx,shy;
				F2INT(tdel.x,shx);
				F2INT(tdel.y,shy);
				color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//				color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
				if (color.a) {
					C32 color2;
					color.c32>>=1;
					color.c32&=xlumask;
					color2=dp[xinc];
					color2.c32>>=1;
					color2.c32&=xlumask;
					color2.c32+=color.c32;
					dp[xinc]=color2;
					wp[xinc].c32=wval;
				}
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXWXLU);
}

static void rendertexxlutriswnck()
{
	C32* sp,*dp,*wp;
	U32 wval,wread;
	float fwval;
	S32 yinc=ft_yi[TOP];
	S32 yend=ft_yi[BOT];
	S32 xinc,xend;
	C32 color;
	struct pointf2 tdel;
	perf_start(RENDERTEXWXLUNCK);
	et=&ft_texedgetable[yinc];
	sp=ft_tex->data;
	dp=ft_bg->data+ft_bg->size.x*yinc;
	wp=ft_wb->data+ft_wb->size.x*yinc;
	while(yinc<yend) {
		xinc=et->left;
		xend=et->right;
		tdel.x=et->startu;
		tdel.y=et->startv;
		fwval=et->w;
		while (xinc<xend) {
			F2INT(fwval,wval);
//			wval=f2S32(fwval);
//			wread=clipgetpixel16(wb,xinc,yinc);
			wread=wp[xinc].c32;
			if (wval>wread) {
				C32 color2;
//				color=clipgetpixel16(tex,f2S32(tdel.x)&texmasku,f2S32(tdel.y)&texmaskv);
//				clipputpixel16(ft_bg,xinc,yinc,color);
//				clipputpixel16(wb,xinc,yinc,wval);
				S32 shx,shy;
				F2INT(tdel.x,shx);
				F2INT(tdel.y,shy);
				color=sp[(shx&ft_texmasku)+((shy&ft_texmaskv)<<ft_texlogu)];
//				color=sp[(f2S32(tdel.x)&ft_texmasku)+((f2S32(tdel.y)&ft_texmaskv)<<ft_texlogu)];
				color.c32>>=1;
				color.c32&=xlumask;
				color2=dp[xinc];
				color2.c32>>=1;
				color2.c32&=xlumask;
				color2.c32+=color.c32;
				dp[xinc]=color2;
				wp[xinc].c32=wval;
			}
			xinc++;
			tdel.x+=ft_p2t.e[0][0];
			tdel.y+=ft_p2t.e[0][1];
			fwval+=ft_matxw;
		}
		dp+=ft_bg->size.x;
		wp+=ft_wb->size.x;
		yinc++;
		et++;
	}
	perf_end(RENDERTEXWXLUNCK);
}

////// try out some floating point triangle stuff
static void drawgentrif(struct face *f)
{
	S32 f2i;
	float fi;
	struct pointf3 *vs[3];
// handles 0 to xres,yres inclusive (pixels centers lie on .5's)
	float mitm,mitb,mimb; // inverse slopes, not always exact (doesn't matter roundup/rounddown)
// sort points (sort pointers to points later)
#ifdef CULLBACK
	S32 detorder; // draw only clockwise triangles
#endif
	perf_start(DRAWGENTRIA);
	vs[0]=&verts[f->vertidx[0]];
	vs[1]=&verts[f->vertidx[1]];
	vs[2]=&verts[f->vertidx[2]];
//	logger("(%f %f %f) (%f %f %f) (%f %f %f)\n",
//	vs[0]->x,vs[0]->y,vs[0]->z,vs[1]->x,vs[1]->y,vs[1]->z,vs[2]->x,vs[2]->y,vs[2]->z);
	if (vs[0]->y<vs[1]->y) { // 0<1
		if (vs[1]->y<vs[2]->y) { // 0<1 && 1<2
			nf.vertidx[TOP]=f->vertidx[0];
			nf.vertidx[MID]=f->vertidx[1];
			nf.vertidx[BOT]=f->vertidx[2];
			ovs[TOP]=vs[0];
			ovs[MID]=vs[1];
			ovs[BOT]=vs[2];
#ifdef CULLBACK
			detorder=1;
#endif
		} else { // 0<1 && 2<1
			if (vs[0]->y<vs[2]->y) { // 0<1 && 2<1 && 0<2
				nf.vertidx[TOP]=f->vertidx[0];
				nf.vertidx[MID]=f->vertidx[2];
				nf.vertidx[BOT]=f->vertidx[1];
				ovs[TOP]=vs[0];
				ovs[MID]=vs[2];
				ovs[BOT]=vs[1];
#ifdef CULLBACK
				detorder=-1;
#endif
			} else { // 0<1 && 2<1&& 2<0
				nf.vertidx[TOP]=f->vertidx[2];
				nf.vertidx[MID]=f->vertidx[0];
				nf.vertidx[BOT]=f->vertidx[1];
				ovs[TOP]=vs[2];
				ovs[MID]=vs[0];
				ovs[BOT]=vs[1];
#ifdef CULLBACK
				detorder=1;
#endif
			}
		}
	} else { // 1<0
		if (vs[2]->y<vs[1]->y) { // 1<0 && 2<1
			nf.vertidx[TOP]=f->vertidx[2];
			nf.vertidx[MID]=f->vertidx[1];
			nf.vertidx[BOT]=f->vertidx[0];
			ovs[TOP]=vs[2];
			ovs[MID]=vs[1];
			ovs[BOT]=vs[0];
#ifdef CULLBACK
			detorder=-1;
#endif
		} else { // 1<0 && 1<2
			if (vs[0]->y<vs[2]->y) { // 1<0 && 1<2 && 0<2
				nf.vertidx[TOP]=f->vertidx[1];
				nf.vertidx[MID]=f->vertidx[0];
				nf.vertidx[BOT]=f->vertidx[2];
				ovs[TOP]=vs[1];
				ovs[MID]=vs[0];
				ovs[BOT]=vs[2];
#ifdef CULLBACK
				detorder=-1;
#endif
			} else { // 1<0 && 1<2 && 2<0
				nf.vertidx[TOP]=f->vertidx[1];
				nf.vertidx[MID]=f->vertidx[2];
				nf.vertidx[BOT]=f->vertidx[0];
				ovs[TOP]=vs[1];
				ovs[MID]=vs[2];
				ovs[BOT]=vs[0];
#ifdef CULLBACK
				detorder=1;
#endif
			}
		}
	}
	perf_end(DRAWGENTRIA);
	perf_start(DRAWGENTRID);
	fi=ovs[TOP]->y-.5f;
//	logger("top fi %f\n",fi);
//	f2i=0;
	F2INT(fi,f2i);
	ft_yi[TOP]=f2i;
//	F2INT(ovs[TOP]->y-.5f,ft_yi[TOP]);
//	ft_yi[TOP]=f2S32(ovs[TOP]->y-.5f);
	topidx=nf.vertidx[TOP];
	fi=ovs[BOT]->y-.5f;
//	logger("bot fi %f\n",fi);
	f2i=0;
	F2INT(fi,f2i);
	ft_yi[BOT]=f2i;
//	ft_yi[BOT]=f2S32(ovs[BOT]->y-.5f);
	botidx=nf.vertidx[BOT];
	// handle degenerates
//	logger("ft_yi[TOP]=%d, ft_yi[BOT]=%d\n",ft_yi[TOP],ft_yi[BOT]);
	if (ft_yi[TOP]==ft_yi[BOT]) { // horizontal sliver, all y values equal
		perf_end(DRAWGENTRID);
		return;
	}
	if (ft_yi[BOT]<=0) {// above all pixels
		perf_end(DRAWGENTRID);
		return;
	}
	mididx=nf.vertidx[MID];
// speed up triangle by switching from shade to flat for same color on all 3 cverts
	if (mat->msflags&SMAT_HASSHADE)
		if (cverts[topidx].x==cverts[mididx].x && cverts[topidx].x==cverts[botidx].x)
			if (cverts[topidx].y==cverts[mididx].y && cverts[topidx].y==cverts[botidx].y)
				if (cverts[topidx].z==cverts[mididx].z && cverts[topidx].z==cverts[botidx].z) {
					ft_C32color=pointf3toC32(&cverts[topidx]);
					trifuncptr=fastshadetrifuncptr;
				}
	(trifuncptr->matrixfunc0)();
	(trifuncptr->matrixfunc1)();
//	if (tritypechange)
//		(trifuncptr->matrixfunc1)();
	(trifuncptr->matrixfunc2)();
	fi=ovs[MID]->y-.5f;
	F2INT(fi,f2i);
	ft_yi[MID]=f2i;
//	ft_yi[MID]=f2S32(ovs[MID]->y-.5f);
	mitb=(ovs[BOT]->x-ovs[TOP]->x)/
		 (ovs[BOT]->y-ovs[TOP]->y);
	perf_end(DRAWGENTRID);
	perf_start(DRAWGENTRIB);
	if (ft_yi[TOP]==ft_yi[MID]) { // tri with almost or exact horiz top
		perf_start(DRAWGENTRIE);
		mimb=(ovs[BOT]->x-ovs[MID]->x)/
			 (ovs[BOT]->y-ovs[MID]->y);
		if (mimb==mitb) {
			perf_end(DRAWGENTRIE);
			perf_end(DRAWGENTRIB);
			return;	// tri with degen top
		}
		if (mimb>mitb) { // mid---top
#ifdef CULLBACK
			if (detorder!=-1) {
				perf_end(DRAWGENTRIE);
				perf_end(DRAWGENTRIB);
				return;
			}
#endif
//			trikind=1;
			setleftgenedge(MID,BOT,mimb);
			setrightgenedge(TOP,BOT,mitb);
		} else { // top---mid
#ifdef CULLBACK
			if (detorder!=1) {
				perf_end(DRAWGENTRIE);
				perf_end(DRAWGENTRIB);
				return;
			}
#endif
//			trikind=2;
			setleftgenedge(TOP,BOT,mitb);
			setrightgenedge(MID,BOT,mimb);
		}
		perf_end(DRAWGENTRIE);
	} else if (ft_yi[MID]==ft_yi[BOT]) { // tri with almost or exact horiz bottom
		perf_start(DRAWGENTRIF);
		mitm=(ovs[MID]->x-ovs[TOP]->x)/
			 (ovs[MID]->y-ovs[TOP]->y);
		if (mitb==mitm) {
			perf_end(DRAWGENTRIF);
			perf_end(DRAWGENTRIB);
			return;	// tri with degen bot
		}
		if (mitb>mitm) { // mid---bot
#ifdef CULLBACK
			if (detorder!=-1) {
				perf_end(DRAWGENTRIF);
				perf_end(DRAWGENTRIB);
				return;
			}
#endif
//			trikind=3;
			setleftgenedge(TOP,MID,mitm);
			setrightgenedge(TOP,BOT,mitb);
		} else { // bot---mid
#ifdef CULLBACK
			if (detorder!=1) {
				perf_end(DRAWGENTRIF);
				perf_end(DRAWGENTRIB);
				return;
			}
#endif
//			trikind=4;
			setleftgenedge(TOP,BOT,mitb);
			setrightgenedge(TOP,MID,mitm);
		}
		perf_end(DRAWGENTRIF);
	} else { // normal tri, see which side midpoint is on, left or right
		perf_start(DRAWGENTRIC);
		mitm=(ovs[MID]->x-ovs[TOP]->x)/
			 (ovs[MID]->y-ovs[TOP]->y);
		mimb=(ovs[BOT]->x-ovs[MID]->x)/
			 (ovs[BOT]->y-ovs[MID]->y);
		if (mitm==mitb) {
			perf_end(DRAWGENTRIC);
			perf_end(DRAWGENTRIB);
			return; // sliver
		}
		if (mitm<mitb) { // left
#ifdef CULLBACK
			if (detorder!=-1) {
				perf_end(DRAWGENTRIC);
				perf_end(DRAWGENTRIB);
				return;
			}
#endif
//			trikind=5;
			setleftgenedge(TOP,MID,mitm);
			setrightgenedge(TOP,BOT,mitb);
			setleftgenedge(MID,BOT,mimb);
		} else { // right
#ifdef CULLBACK
			if (detorder!=1) {
				perf_end(DRAWGENTRIC);
				perf_end(DRAWGENTRIB);
				return;
			}
#endif
//			trikind=6;
			setleftgenedge(TOP,BOT,mitb);
			setrightgenedge(TOP,MID,mitm);
			setrightgenedge(MID,BOT,mimb);
		}
		perf_end(DRAWGENTRIC);
	}
	(trifuncptr->renderfunc)();
	perf_end(DRAWGENTRIB);
}

void drawtris(struct face *f,S32 ntris)
{
//	if (nodraw)
//		return;
//	U32 cw;
//	READCW(cw);
	perf_start(DRAWTRIS);

//	fpucw=getfpucontrol();
	setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
//	setfpucontrol(fpucw&0xf0ff | (RND_UP<<10) | (PREC_32<<8));
//	ft_hicolor=floatcolorto16(&mat->color,bgtype);
	while(--ntris>=0) {
		trifuncptr=slowshadetrifuncptr;
/*
		logger("draw tris (%8.3f %8.3f %8.3f) (%8.3f %8.3f %8.3f) (%8.3f %8.3f %8.3f)\n",
			verts[f->vertidx[0]].x,verts[f->vertidx[0]].y,verts[f->vertidx[0]].z,
			verts[f->vertidx[1]].x,verts[f->vertidx[1]].y,verts[f->vertidx[1]].z,
			verts[f->vertidx[2]].x,verts[f->vertidx[2]].y,verts[f->vertidx[2]].z);
*/
		drawgentrif(f++);
	}
//	setfpucontrol(fpucw);
//	WRITECW(cw);
	defaultfpucontrol();

	perf_end(DRAWTRIS);
}

void drawtris(vector<face>& f,S32 foffset,S32 ntris)
{
//	if (nodraw)
//		return;
//	U32 cw;
//	READCW(cw);
//	S32 i;
//	S32 n=f.size();
//	logger("foffset %d,ntris %d\n",foffset,ntris);
//	for (i=0;i<n;++i)
//		logger("face %d %d %d\n",f[i].vertidx[0],f[i].vertidx[1],f[i].vertidx[2]);
//	return;
	perf_start(DRAWTRIS);
	vector<face>::iterator fis=f.begin()+foffset;//.begin();//&(f[foffset]);
	vector<face>::iterator fie=f.begin()+(foffset+ntris);//.begin();//&(f[foffset]);

//	fpucw=getfpucontrol();
	setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
//	setfpucontrol(fpucw&0xf0ff | (RND_UP<<10) | (PREC_32<<8));
//	ft_hicolor=floatcolorto16(&mat->color,bgtype);
	while(fis<fie) {
//	while(--ntris>=0) {
		trifuncptr=slowshadetrifuncptr;
/*
		logger("draw tris (%8.3f %8.3f %8.3f) (%8.3f %8.3f %8.3f) (%8.3f %8.3f %8.3f)\n",
			verts[fis->vertidx[0]].x,verts[fis->vertidx[0]].y,verts[fis->vertidx[0]].z,
			verts[fis->vertidx[1]].x,verts[fis->vertidx[1]].y,verts[fis->vertidx[1]].z,
			verts[fis->vertidx[2]].x,verts[fis->vertidx[2]].y,verts[fis->vertidx[2]].z);
*/
		drawgentrif(&(*fis));
		++fis;
	}
//	setfpucontrol(fpucw);
//	WRITECW(cw);
	defaultfpucontrol();

	perf_end(DRAWTRIS);
}
