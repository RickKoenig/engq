#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
//#include <d3d9.h>
//#include <d3dx9.h>
//#include "m_vid_dx9.h"
//#include "d2_dx9.h"
//#include "d2_dx9b.h"
#include "m_perf.h"
//#define HALFPIXEL	// when defined, fix half pixel problem
#include "../engqtest/u_modelutil.h"
#include "d2_font.h"
#include "d2_dx9_font.h"

#define MAXCHARS 5000

//#define FRESX 640.0f
//#define FRESY 480.0f

static viewport2 drawtextvp={
	0,		// backcolor
	0,1,	// zfront,zback
	0,0,	// xstart,ystart
	0,0,	// xres,yres // WX,WY
	1,1,	// xsrc,ysrc
	{.5f,-.5f,0,0},{0,0,0,0}, // camtrans,camrot
	{0},	// c2w
	false,	// usec2w
	1,		// camzoom
	0,		// flags
	true,	// isortho
	1,		// ortho_size
	0,		// camattach
	false,	// usecamattach
//	0,		// lookat
};
/*
static pointf3 textverts[4]={
	{          0,           0,.5f,0},
	{8.0f/640.0f,           0,.5f,0},
	{          0,-8.0f/480.0f,.5f,0},
	{8.0f/640.0f,-8.0f/480.0f,.5f,0},
};
static uv textuvs[4]={
	{7.0f/16.0f,4.0f/8.0f},
	{8.0f/16.0f,4.0f/8.0f},
	{7.0f/16.0f,5.0f/8.0f},
	{8.0f/16.0f,5.0f/8.0f},
//	{0,0},
//	{1,0},
//	{0,1},
//	{1,1},
};
*/
//static float fsx,fsy;

static pointf3 fverts[MAXCHARS*4];
static uv fuvs[MAXCHARS*4];
static S32 nc,npc;

/*#define chrnx 16.0f
#define RSHIFT 4
#define chrny 8.0f
#define CHSIZEX 8.0f
#define CHSIZEY 8.0f
*/
static void d2_drawchars(const fntque& fq)
{
	const C8* p=fq.t.c_str();
	float x=(float)fq.x;
	float y=(float)fq.y;
#if 1
	S32 chrnx = fq.font->cx; // num columns
	S32 chrny = fq.font->cy;  // num rows
	S32 chsizex = fq.font->gx; // glyph sizex
	S32 chsizey = fq.font->gy; // glyph sizey
#else
	S32 chrnx = 16; // num columns
	S32 chrny = 8;  // num rows
	S32 chsizex = 8; // glyph sizex
	S32 chsizey = 8; // glyph sizey
#endif
	while(nc<MAXCHARS) {
		U32 c=*p;
		if (!c)
			break;
/*		float ch=(float)(c&((1<<RSHIFT)-1));
		float cv=(float)(c>>RSHIFT); */
		float ch=(float)(c%chrnx);
		float cv=(float)(c/chrnx);
		S32 fcc=4*nc;
		float mux=1.0f;///WX;
		float muy=1.0f;///WY;
//		float mux=1.0f/FRESX;
//		float muy=1.0f/FRESY;
		fverts[fcc+0]=pointf3x(x*mux            ,-(y*muy            ),0);
		fverts[fcc+1]=pointf3x(x*mux+chsizex*mux,-(y*muy            ),0);
		fverts[fcc+2]=pointf3x(x*mux            ,-(y*muy+chsizey*muy),0);
		fverts[fcc+3]=pointf3x(x*mux+chsizex*mux,-(y*muy+chsizey*muy),0);

		fuvs[fcc+0].u=ch*(1.0f/chrnx);
		fuvs[fcc+0].v=cv*(1.0f/chrny);

		fuvs[fcc+1].u=ch*(1.0f/chrnx)+(1.0f/chrnx);
		fuvs[fcc+1].v=cv*(1.0f/chrny);

		fuvs[fcc+2].u=ch*(1.0f/chrnx);
		fuvs[fcc+2].v=cv*(1.0f/chrny)+(1.0f/chrny);

		fuvs[fcc+3].u=ch*(1.0f/chrnx)+(1.0f/chrnx);
		fuvs[fcc+3].v=cv*(1.0f/chrny)+(1.0f/chrny);
//		clipfore32(video3dinfo.sysfont,B32,ch*8,cv*8,x,y,8,8,fq.colf);
		++nc;
		++p;
		x+=chsizex;
	}
/*	if (fq.hascolb) {
		while(c=*p) {
			U32 ch=c&0xf;
			U32 cv=c>>4;
			clipforeback32(video3dinfo.sysfont,B32,ch*8,cv*8,x,y,8,8,fq.colf,fq.colb);
			++p;
			x+=8;
		}
	} else {
		while(c=*p) {
			U32 ch=c&0xf;
			U32 cv=c>>4;
			clipfore32(video3dinfo.sysfont,B32,ch*8,cv*8,x,y,8,8,fq.colf);
			++p;
			x+=8;
		}
	}
*/
}

void d2_drawtextque_do()
{
//	return;
	tree2* r=new tree2("tfontpatch");
	modelb* modb=model_create("mfontpatch");
	if (model_getrc(modb)==1) {
		pushandsetdir("common");

		nc=npc=0;
		while (fntlist.size()) {
			fntque fq = fntlist.front();
			d2_drawchars(fq);
			fntlist.pop_front();
			const softfont* sf = fq.font;
			textureb* texmat0 = texture_create(sf->name.c_str());
			S32 rc = texture_getrc(texmat0);
			if (rc == 1) {
				texmat0->load();
			}
			S32 f;
			const C8* s;
			if (fq.hascolb) {
				if (fq.colf.w<treeinfo.dissolvestart || fq.colb.w<treeinfo.dissolvestart)
					f=SMAT_HASTEX|SMAT_CLAMP|SMAT_HASNOFOG;
				else
					f=SMAT_HASTEX|SMAT_CLAMP|SMAT_NOALPHA|SMAT_HASNOFOG; // overrides the tex hasalpha
				s = "tex2c";
			} else {
				f=SMAT_HASTEX|SMAT_CLAMP|SMAT_HASNOFOG;
				s = "tex";
			}
			modb->addmat2c(s,f,texmat0,&fq.colf,&fq.colb,50,2*(nc-npc),4*(nc-npc));
			S32 i;
			for (i=npc;i<nc;++i) {
				modb->addface(4*i+0,4*i+1,4*i+2);
				modb->addface(4*i+3,4*i+2,4*i+1);
			}
			npc=nc;
		}
//		fsx=16.0f*8.0f/WX*2.0f;
//		fsy=8.0f*8.0f/WY*2.0f;
		modb->copyverts(fverts,nc*4);
		modb->copyuvs0(fuvs,nc*4);
//		modb->copynorms(textnorms,4);
		modb->close();
		popdir();
	}
	r->setmodel(modb);
//	r->trans=pointf3x(.5f*fsx,-.5f*fsy,.5f);
	drawtextvp.xres=WX;
	drawtextvp.yres=WY;
	drawtextvp.ortho_size=(float)WY;
	drawtextvp.camtrans.x=(float)(WX/2);
	drawtextvp.camtrans.y=(float)(-WY/2);
	drawtextvp.xsrc=WX;
	drawtextvp.ysrc=WY;
	drawtextvp.useattachcam=false;
//	drawtextvp.lookat=0;
//	video_beginscene();
	video_setviewport(&drawtextvp);
//	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MINFILTER ,D3DTEXF_POINT );
//	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MAGFILTER ,D3DTEXF_POINT );
//	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MIPFILTER ,D3DTEXF_POINT ); 
//	videoinfo.didbuildworldmats=false;
	video_buildworldmats(r);
	video_drawscene(r);
//	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );
//	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );
//	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR ); 
//	video_endscene();
	static bool once=true;
	if (once) {
		logger("font: logging roottree\n");
		r->log2();
//		logger("font: logging reference lists\n");
//		logrc();
		once=false;
	}
	delete r;
}

// software can call this too, see u_s_arena2.cpp
modelb* d2_drawtextque_build()
{
//	return;
//	tree2* r=new tree2("tfontpatch");
	modelb* modb=model_create(unique());
	if (model_getrc(modb)==1) {
		pushandsetdir("common");
		nc=npc=0;
		while (fntlist.size()) {
			fntque fq=fntlist.front();
			d2_drawchars(fq);
			fntlist.pop_front();
			//textureb* texmat0=texture_create("sysfont.pcx");
			textureb* texmat0=texture_create(fq.font->name.c_str());
			if (texture_getrc(texmat0)==1) {
				texmat0->load();
			}
			S32 f;
			const C8* s;
			if (fq.hascolb) {
				if (fq.colf.w<treeinfo.dissolvestart || fq.colb.w<treeinfo.dissolvestart)
					f=SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE|SMAT_CLAMP|SMAT_HASNOFOG;
				else
					f=SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE|SMAT_CLAMP|SMAT_NOALPHA|SMAT_HASNOFOG; // overrides the tex hasalpha
				s = "tex2c";
			} else {
				f=SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE|SMAT_CLAMP|SMAT_HASNOFOG;
				s = "tex";
			}
			modb->addmat2c(s,f,texmat0,&fq.colf,&fq.colb,50,2*(nc-npc),4*(nc-npc));
			S32 i;
			for (i=npc;i<nc;++i) {
				modb->addface(4*i+0,4*i+1,4*i+2);
				modb->addface(4*i+3,4*i+2,4*i+1);
			}
			npc=nc;
		}
//		fsx=16.0f*8.0f/WX*2.0f;
//		fsy=8.0f*8.0f/WY*2.0f;
		modb->copyverts(fverts,nc*4);
		modb->copyuvs0(fuvs,nc*4);
//		modb->copynorms(textnorms,4);
		modb->close();
		popdir();
	}
//	r->setmodel(modb);
//	return r;
	return modb;
}
