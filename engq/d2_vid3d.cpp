#define D2_3D
#include <m_eng.h>
#include <d3d9.h>
#include <m_perf.h>

#include "d2_software.h" // used for gdi and directdraw
#include "d2_dx9.h" // used for windowed and fullscreen d3d
#include "d2_font.h"

vi3d video3dinfo;

// internal
struct video3dapi {
	void (*video_startscenefunc)();
	void (*video_setviewportfunc)(viewport2* vp);
	bool dohalfpix;
//	void (*video_drawscenefunc)(tree2* t);
	void (*video_endscenefunc)(bool dotextdump);
//	enum clipresult (*video_drawmodelfunc) (tree2* t,viewport2* v,mat4* o2c);
	void (*video_initfunc)();
	void (*video_uninitfunc)();
	modelb* (*modelcreatefunc)(const C8* name);
	textureb* (*texturecreatefunc)(const C8* name);
	void (*rendertargetmainfunc)();
	bool (*rendertargetshadowmapfunc)();
//	void (*reload_mat2shader)(const C8* name);
};

//#define D3DDOHALFPIX false
#define D3DDOHALFPIX true
static video3dapi video3dapis[VIDEO_NSUPPORTED_DEVICES]={
// standard gdi
#if 1
	{software_beginscene,software_setviewport,false,software_endscene,
	software_init,software_uninit,software_createmodel,software_createtexture,
	software_rendertargetmainfunc,software_rendertargetshadowmapfunc},
// directdraw
	{software_beginscene,software_setviewport,false,software_endscene,
	software_init,software_uninit,software_createmodel,software_createtexture,
	software_rendertargetmainfunc,software_rendertargetshadowmapfunc},
// direct3d windowed
	{dx9_beginscene,dx9_setviewport,D3DDOHALFPIX,dx9_endscene,
	dx9_init,dx9_uninit,dx9_createmodel,dx9_createtexture,
	dx9_rendertargetmainfunc,dx9_rendertargetshadowmapfunc},
// direct3d fullscreen
	{dx9_beginscene,dx9_setviewport,D3DDOHALFPIX,dx9_endscene,
	dx9_init,dx9_uninit,dx9_createmodel,dx9_createtexture,
	dx9_rendertargetmainfunc,dx9_rendertargetshadowmapfunc},
#endif
};

void logrc()
{
	logger_indent();
    logger("show model list\n");
	modelb::rc.showrc();
	logger("show texture list\n");
	textureb::rc.showrc();
    logger_unindent();
}

static modelb* dum;
static textureb* dumtex;

void d2_3d_init()
{
#if 0
	pushandsetdir("common");
	video3dinfo.sysfont=gfxread32("sysfont.pcx");
	popdir();
#endif
	if (videoinfo.video_maindriver>=0) {
		video3dapis[videoinfo.video_maindriver].video_initfunc();
	}
	dum=model_create("dum"); // gets rid of 'class modelb RCLIST now at 0' message in logfile.txt
	dumtex=texture_create("dumtex");
}

void d2_3d_uninit()
{
	if (videoinfo.video_maindriver>=0) {
		video3dapis[videoinfo.video_maindriver].video_uninitfunc();
	}
	fntlist.clear();
#if 0
	if (video3dinfo.sysfont)
		bitmap32free(video3dinfo.sysfont);
	video3dinfo.sysfont=0;
#endif
	if (dum) {
		dum->rc.deleterc(dum);
		dum = 0;
	}
	if (dumtex) {
		dumtex->rc.deleterc(dumtex);
		dumtex = 0;
	}
}

// o to w to v to c
// calcs w2v,v2c and matViewProj 
// from camtrans, camrot, xsrc, ysrc, isortho,, zback, zfront 
// ortho only:: ortho_size
// pers only:: camzoom
// dohalfpixel only:: xres, yres
// o object
// w world
// v camview
// c clip
// assumes left handed, (negated z)
void calcviewportmats(viewport2* vp,bool dohalfpixel)
{
// calc w2v
	if (!vp->usev2w) {
		identmat4(&vp->v2w);
		buildrottrans3d(&vp->camrot,&vp->camtrans,&vp->v2w);
	}
	if (vp->camattach && vp->useattachcam) {
		mulmat4(&vp->v2w,&vp->camattach->o2w,&vp->v2w);
	}
	video3dinfo.w2v=vp->v2w;
	inversemat3d(&video3dinfo.w2v,&video3dinfo.w2v);
	mat4 w2v_sky=video3dinfo.w2v;
	w2v_sky.e[3][0]=w2v_sky.e[3][1]=w2v_sky.e[3][2]=0;
// calc v2c
	identmat4(&video3dinfo.v2c);
	float asp=(float)vp->ysrc/(float)vp->xsrc;
	if (vp->isortho) {
//		inortho=true;
		video3dinfo.v2c.e[1][1]=2.0f/(vp->ortho_size);
		video3dinfo.v2c.e[0][0]=video3dinfo.v2c.e[1][1]*asp;
		video3dinfo.v2c.e[2][2]=1.0f/(vp->zback-vp->zfront);
		if (dohalfpixel) {
			video3dinfo.v2c.e[3][0]=-1.0f/vp->xres;	// these 2 lines fix the d3d half pixel problem
			video3dinfo.v2c.e[3][1]=1.0f/vp->yres;
		}
		video3dinfo.v2c.e[3][2]=-vp->zfront*video3dinfo.v2c.e[2][2];
	} else {
//		inortho=false;
		video3dinfo.v2c.e[0][0]=vp->camzoom*asp; // ? maybe get the camzoom from attachcamera too
		video3dinfo.v2c.e[1][1]=vp->camzoom;
		if (dohalfpixel) {
			video3dinfo.v2c.e[2][0]=-1.0f/vp->xres;	// these 2 lines fix the d3d half pixel problem
			video3dinfo.v2c.e[2][1]=1.0f/vp->yres;
		}
		video3dinfo.v2c.e[2][2]=vp->zback/(vp->zback-vp->zfront);
		video3dinfo.v2c.e[2][3]=1; // 'w friendly'
		video3dinfo.v2c.e[3][2]=-vp->zfront*video3dinfo.v2c.e[2][2];
		video3dinfo.v2c.e[3][3]=0;
	}
// calc w2c
	mulmat4(&video3dinfo.w2v,&video3dinfo.v2c,&video3dinfo.w2c);
	mulmat4(&w2v_sky,&video3dinfo.v2c,&video3dinfo.w2c_sky);
}

void video_beginscene()
{
//	buildrottrans3d(&vp->camrot,&vp->camtrans,&vp->w2c);
//	inversemat3d(&vp->w2c,&vp->w2c);
	if (videoinfo.video_maindriver>=0)
		video3dapis[videoinfo.video_maindriver].video_startscenefunc();
}

void video_setviewport(struct viewport2 *vp)
{
//	buildrottrans3d(&vp->camrot,&vp->camtrans,&vp->w2c);
//	inversemat3d(&vp->w2c,&vp->w2c);
	if (videoinfo.video_maindriver>=0)
		calcviewportmats(vp,video3dapis[videoinfo.video_maindriver].dohalfpix);
		video3dapis[videoinfo.video_maindriver].video_setviewportfunc(vp);
}

void video_drawscene(tree2* t)
{
//	if (videoinfo.video_maindriver>=0)
//		video3dapis[videoinfo.video_maindriver].video_drawscenefunc(t);
	perf_start(DRAWTREE);
//	textureb::animtex();
// recursively draw the tree..
	treeinfo.ndrawtree=0;
	video_buildtreelist(t); // builds up t->o2w
	video_sorttreelist();
//	buildworldmats(t);
//	dolights();	//	logmat4(&curvp->w2c,"w2c");
	video_drawtreelist();
	perf_end(DRAWTREE);
}

void video_endscene(bool dotextdump)
{
	if (videoinfo.video_maindriver>=0)
		video3dapis[videoinfo.video_maindriver].video_endscenefunc(dotextdump);
}

void video_rendertargetmain()
{
	if (videoinfo.video_maindriver>=0)
		video3dapis[videoinfo.video_maindriver].rendertargetmainfunc();
}

bool video_rendertargetshadowmap()
{
	if (videoinfo.video_maindriver>=0)
		return video3dapis[videoinfo.video_maindriver].rendertargetshadowmapfunc();
	return 0;
}

/*S32 video_drawmodel(class tree2 *t,struct viewport2 *v,struct mat4 *o2c)
{
	if (videoinfo.video_maindriver>=0)
		return video3dapis[videoinfo.video_maindriver].video_drawmodelfunc(t,v,o2c);
	return 0;
}*/

modelb* model_create(const C8* namea)
{
	if (videoinfo.video_maindriver>=0)
		return video3dapis[videoinfo.video_maindriver].modelcreatefunc(namea);
	else
		errorexit("can't create model without a video device set");
	return 0;
}

textureb* texture_create(const C8* namea) // factory for textureb's
{
	textureb* ret=0;
	const C8* newname;
	newname=namea;
	if (isfileext(newname,"ifl"))
		ret=ifl_createtexture(newname); // go and create a generic ifl texture
	else if (videoinfo.video_maindriver>=0)
		ret=video3dapis[videoinfo.video_maindriver].texturecreatefunc(newname);
	else
		errorexit("can't create texture without a video device set");
	return ret;
}

S32 model_getrc(modelb* m)
{
	return modelb::rc.getrc(m);
}

S32 texture_getrc(textureb* tex)
{
	return textureb::rc.getrc(tex);
}

static textureb* vid_envv2tex;
void make_envv2tex(const C8* texname)
{
	vid_envv2tex=texture_create(texname);
	vid_envv2tex->load();
}

void free_envv2tex()
{
	if (vid_envv2tex) {
		textureb::rc.deleterc(vid_envv2tex);
		vid_envv2tex=0;
	}
}

textureb* get_envv2tex()
{
	return vid_envv2tex;
}
