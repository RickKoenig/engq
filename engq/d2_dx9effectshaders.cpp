#include "d2_dx9shaderswitch.h"
#ifdef USEEFFECTSSHADERS
#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "m_vid_dx9.h"
#include "d2_dx9.h"
//#include "d2_dx9b.h"
#include "m_perf.h"
//#define HALFPIXEL	// when defined, fix half pixel problem
//#include "../engqtest/u_modelutil.h"
#include "d2_font.h"
#include "d2_dx9_font.h"

#ifndef D3DXFX_LARGEADDRESSAWARE
#define D3DXFX_LARGEADDRESSAWARE 0
#endif
#define USEEFFECTPOOL

namespace d2_dx9 {
LPDIRECT3DTEXTURE9      g_pShadowMap;    // Texture to which the shadow map is rendered
LPDIRECT3DSURFACE9      g_pDSShadow;     // Depth-stencil buffer for rendering to shadow map
S32 makeshadowmapidx;	// override for light relative scene render
S32 useshadowmapidx;	// reference to 
S32 pdiffidx;			// if can't shadowmap
//S32 envidx;
// handy
pointf3 worldEyePos;
textureb* whitetex,*sysfonttex,*sysfonttex2;
//bool canshadow;
// effects NEW!
LPD3DXEFFECTPOOL effectpool;
//LPD3DXEFFECT firsteffect;

// globals for effects
enum FX_GLOBALS {
	o2cmat,
	o2wmat,
	tex,
	tex2,
	material_color,
	material_color2,

	dirlightdir,
	dirlightcol,
	eyeposobj,
	eyeposworld,
	specpow,

	amblightcol,
	toonedgesize,
	stanx,
	stany,
	edgesize,

	edgezoff,
	o2lcmat,
	envv2texg,
	uvoffsetu,
	uvoffsetv,

	lightpos,

	FX_NUMGLOBALS,
};

C8* fx_names[FX_NUMGLOBALS]={
	"g_mWorldViewProjection",	// o2c
	"g_mWorld",					// o2w
	"g_MeshTexture",
	"g_MeshTexture2",
	"g_material_color",
	"g_material_color2",

	"g_dirlightdir",
	"g_dirlightcol",
	"g_eyeposobj",
	"g_eyeposworld",
	"g_specpow",

	"g_amblightcol",
	"g_toonedgesize",
	"g_stanx",
	"g_stany",
	"g_edgesize",

	"g_edgezoff",
	"g_o2lcmat",		// o2lc
	"g_envv2tex",		// global env map texture
	"g_uvoffsetu",
	"g_uvoffsetv",

	"g_lightpos",
};

struct fx_global {
	LPD3DXEFFECT gfx;
	D3DXHANDLE gh;
};

fx_global fx_globals[FX_NUMGLOBALS];

//D3DXHANDLE fx_o2cmat; // object to camera matrix, used by most vertex shaders
//D3DXHANDLE fx_tex;		// main texture
//	D3DXHANDLE fx_technique;
//D3DXHANDLE fx_material_color;	// rgba material color

//D3DXHANDLE fx_dirlightdir;
//D3DXHANDLE fx_dirlightcol;
//D3DXHANDLE fx_amblightcol;

//	m_pEffect->GetParameterBySemantic(NULL, "WORLDVIEWPROJ");


// // curently all handled in m_vid_dx9.cpp (2d)
S32 getrefcount(IUnknown* i)
{
	i->AddRef();
	S32 ret=i->Release();
	if (ret!=1)
		errorexit("refcount IUnknown != 1 it is instead a %d",ret);
	return ret;
}

// create vertex and pixel shader
struct CUSTOMVERTEX_VCT
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DCOLOR col;//float x,y,z;      // The untransformed, 3D position for the vertex
	float tu,tv;
};
struct CUSTOMVERTEX_VT
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
	float tu,tv;
};
struct CUSTOMVERTEX_VT2
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
	float tu0,tv0;
	float tu1,tv1;
};
struct CUSTOMVERTEX_VNT
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DVECTOR norm;//float x,y,z;      // The vertex normal, normalized of course!
	float tu,tv;
};
struct CUSTOMVERTEX_VNTM
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DVECTOR norm;//float x,y,z;      // The vertex normal, normalized of course!
	D3DVECTOR tangent;
	D3DVECTOR binorm;
	float tu,tv;
};
// stan
struct CUSTOMVERTEX_VN
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DVECTOR norm;//float x,y,z;      // The vertex normal, normalized of course!
};
// old (bad) edge system based on uvs
struct CUSTOMVERTEX_VNT2
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DVECTOR norm;//float x,y,z;      // The vertex normal, normalized of course!
	float tu0,tv0;
	float tu1,tv1;
};
// new edge system based on 2 positions
struct CUSTOMVERTEX_VE
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DVECTOR pos2;//float x,y,z;      // The untransformed, 3D position for the other vertex (almost)
};

D3DVERTEXELEMENT9 decl_vct[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0,},
	{ 0, 16,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vt[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vt2[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	{ 0, 20,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vnt[] = {
	{ 0,  0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0,},
	{ 0, 24,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vntm[] = {
	{ 0,  0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0,},
	{ 0, 24,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0,},
	{ 0, 36,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0,},
	{ 0, 48,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 decl_vn[] = {
	{ 0,  0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vnt2[] = {
	{ 0,  0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	  0,},
	{ 0, 24,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	{ 0, 32,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_ve[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};

struct shadeslot {
//	IDirect3DVertexShader9* vs;
//	IDirect3DPixelShader9* ps;
	LPD3DXEFFECT fx;
	S32 vdi; /// vertex declaration index
	string matname;
};

vector<shadeslot> shaders;

} /// end anonymous namespace
using namespace d2_dx9;

struct model3::vdecl model3::vdecls[]=
{
	{"VCT",decl_vct,sizeof(CUSTOMVERTEX_VCT),&model3::close_VCT,0},
	{"VT",decl_vt,sizeof(CUSTOMVERTEX_VT),&model3::close_VT,0},
	{"VT2",decl_vt2,sizeof(CUSTOMVERTEX_VT2),&model3::close_VT2,0},
	{"VNT",decl_vnt,sizeof(CUSTOMVERTEX_VNT),&model3::close_VNT,0},
	{"VNTM",decl_vntm,sizeof(CUSTOMVERTEX_VNTM),&model3::close_VNTM,0},

	{"VN",decl_vn,sizeof(CUSTOMVERTEX_VN),&model3::close_VN,0},
	{"VNT2",decl_vnt2,sizeof(CUSTOMVERTEX_VNT2),&model3::close_VNT2,0},
	{"VE",decl_ve,sizeof(CUSTOMVERTEX_VE),&model3::close_VE,0},
};
const S32 model3::nvdecls=sizeof(model3::vdecls)/sizeof(model3::vdecls[0]);

void model3::createshaders()
{
#define SHADECOL 3 // number of columns in shadelistfx.txt
	memset (fx_globals,0,sizeof(fx_globals));
	HRESULT hr;
	pushandsetdir("shaders");
	logger("in create shaders\n");
//	firsteffect=0;
/// first create the vertex declarations
	U32 i,j,n;
	for (i=0;i<(U32)nvdecls;++i) {
		hr=videoinfodx9.d3d9device->CreateVertexDeclaration(vdecls[i].ve, &vdecls[i].vd);
		if (hr!=D3D_OK)
			errorexit("bad createvertexdeclaration %x",hr);
	}
/// now create the effects
#ifdef USEEFFECTPOOL
	hr=D3DXCreateEffectPool(&effectpool);
	if (hr!=D3D_OK)
		errorexit("can't create global effectpool");
#endif
// global effects pool
	script shadelist("shadelistfx.txt");
//	mat2shader=new script("mat2shader.txt");
//	S32 na=mat2shader->num();
//	if (na%2)
//		errorexit("bad mat2shader.txt");
	n=shadelist.num();
	if ((n%SHADECOL)!=0)
		errorexit("bad shadelistfx.txt");
#if 1
	for (i=0;i<n;i+=SHADECOL) {
		shadeslot ss;
/// setup vertex decl first
		for (j=0;j<(U32)nvdecls;++j) {
			if (!strcmp(shadelist.idx(i+2).c_str(),vdecls[j].name)) {
				ss.vdi=j;
				break;
			}
		}
		if (j==(U32)nvdecls)
			errorexit("can't find vertex declaration '%s'",shadelist.idx(i+2).c_str());
//		logger("shader tok is '%s'\n",shadelist.idx(i).c_str());
		ID3DXBuffer *pShaderBuffer=0;
//		ID3DXBuffer *pShaderErrors=0;
		LPD3DXEFFECT neweffect;
		if (!fileexist(shadelist.idx(i+1).c_str()))
			errorexit("can't find effect '%s' file not found",shadelist.idx(i+1).c_str());
		hr=D3DXCreateEffectFromFile(
			videoinfodx9.d3d9device ,
			shadelist.idx(i+1).c_str(), 
			0, // CONST D3DXMACRO* pDefines,
			0, // LPD3DXINCLUDE pInclude,
			D3DXFX_NOT_CLONEABLE|D3DXFX_LARGEADDRESSAWARE, 
			effectpool, // LPD3DXEFFECTPOOL pPool,
			&neweffect, 
			&pShaderBuffer );
		if (hr!=D3D_OK)
			if (pShaderBuffer)
				errorexit("can't create aneffect %08x %s",hr,pShaderBuffer->GetBufferPointer());
			else
				errorexit("can't create aneffect %08x '%s' error unknown",hr,shadelist.idx(i+1).c_str());
//		if (!firsteffect)
//			firsteffect=neweffect;
//		fx_technique=neweffect->GetTechniqueByName("RenderScene");
//		if (!fx_technique)
//			errorexit("can't find name RenderScene");
//		hr=neweffect->SetTechnique(fx_technique);
//		if (hr!=D3D_OK)
//			errorexit("can't set technique %08x ",hr);
		ss.matname=shadelist.idx(i+0);
		ss.fx=neweffect;
		shaders.push_back(ss);
	}
	makeshadowmapidx=findshaderidx("makeshadowmap");
	useshadowmapidx=findshaderidx("useshadowmap");
	pdiffidx=findshaderidx("pdiff"); // for downgrade if no shadowmap
//	envidx=findshaderidx("env");
#endif

	for (i=0;i<FX_NUMGLOBALS;++i) {
		for (j=0;j<shaders.size();++j) {
			fx_globals[i].gh=shaders[j].fx->GetParameterByName(0, fx_names[i]);
			if (fx_globals[i].gh) {
				fx_globals[i].gfx=shaders[j].fx;
				break;
			}
		} 
		if (!fx_globals[i].gh)
			errorexit("can't find effect global name '%s'",fx_names[i]);
	}
	popdir();
}

void model3::destroyshaders()
{
	HRESULT hr;
/// free up vertex declarations
	S32 i;
	for (i=0;i<nvdecls;++i) {
		if (vdecls[i].vd) {
			hr=vdecls[i].vd->Release();
			if (hr!=D3D_OK)
				errorexit("bad vdecl release %d",hr);
			vdecls[i].vd=0;
		}
	}
	S32 n=shaders.size();
	for (i=0;i<n;++i) {
		if (shaders[i].fx)
			shaders[i].fx->Release();
	}
	shaders.clear();
//	delete mat2shader;
//	mat2shader=0;

#ifdef USEEFFECTPOOL
	if (effectpool) {
		hr=effectpool->Release();
		if (hr!=D3D_OK)
			errorexit("can't release effectpool"); 
		effectpool=0;
	}
#endif
}

S32 model3::findshaderidx(const C8* namem)
{
// first do a mat2shader, mats can be like this 'pdiffspec_higloss' or 'pdiffspec'
	const C8* names;
	S32 i,n;
	if (!videoinfo.mat2shader)
		errorexit("can't find mat2shader");
	n=videoinfo.mat2shader->num();
	if (!n)
		errorexit("findshaderidx not inited");
	for (i=0;i<n;i+=2) {
		string ss=videoinfo.mat2shader->idx(i);
		string sn=namem;
		U32 si=sn.find("_");
		sn=sn.substr(0,si);
		if (ss==sn)
			break;
		if (videoinfo.mat2shader->idx(i)=="*")
			break;
	}
	if (i==n)
		errorexit("can't convert mat '%s' to a shader",namem);
	names=videoinfo.mat2shader->idx(i+1).c_str();
// now a shader2idx
	n=shaders.size();
	if (n<=0)
		errorexit("no shaders!");
	for (i=0;i<n;++i)
		if (!strcmp(shaders[i].matname.c_str(),names))
			break;
	if (i==n) {
		logger("WARNING: can't find shaderidx for '%s', using '%s' instead\n",names,shaders[0].matname.c_str());
		return 0; /// use first shader if not found
	}
	return i;
}

const C8* model3::getnextshadername(const C8* oldname)
{
	S32 i,n=shaders.size();
	if (n<=0)
		return(0);
	for (i=0;i<n;++i)
		if (!strcmp(shaders[i].matname.c_str(),oldname))
			break;
	if (i==n)
//		errorexit("can't find shaderidx for '%s'",name);
		return shaders[0].matname.c_str(); /// use first shader if not found
	++i;
	if (i==n)
		i=0;
	return shaders[i].matname.c_str();
}

void release_d3dpool_default()
{
	if (g_pShadowMap) {
		logger("ref of shadow buffer %d\n",getrefcount(g_pShadowMap));
		g_pShadowMap->Release();
		g_pShadowMap=0;
	}
	if (g_pDSShadow) {
		logger("ref of shadow depth %d\n",getrefcount(g_pDSShadow));
		g_pDSShadow->Release();
		g_pDSShadow=0;
	}
	video3dinfo.canshadow=false;
}

void reset_d3dpool_default()
{
// Create the shadow map texture
	g_pShadowMap=0;
	g_pDSShadow=0;
	video3dinfo.canshadow=false;
#ifdef SHADOWMAP
#if 1
	HRESULT hr=videoinfodx9.d3d9device->CreateTexture( SHADOWMAP_SIZE, SHADOWMAP_SIZE,
                                         1, D3DUSAGE_RENDERTARGET,
//                                       D3DFMT_X8R8G8B8,
		                                 D3DFMT_R32F,
                                         D3DPOOL_DEFAULT,
                                         &g_pShadowMap,
                                         NULL ); 
#else
	HRESULT hr=D3D_OK+1;
#endif
#if 1
	HRESULT hr2=videoinfodx9.d3d9device->CreateDepthStencilSurface( SHADOWMAP_SIZE,
                                                     SHADOWMAP_SIZE,
                                                     videoinfodx9.pp.AutoDepthStencilFormat,
                                                     D3DMULTISAMPLE_NONE,
                                                     0,
                                                     TRUE,
                                                     &g_pDSShadow,
                                                     NULL );

#else
	HRESULT hr2=D3D_OK+1;
#endif

	if (hr!=D3D_OK) {
		logger("can't create shadow map texture %08x\n",hr);
		g_pShadowMap=0;
	}
	if (hr2!=D3D_OK) {
		logger("can't create shadow map depth buffer %08x\n",hr2);
		g_pDSShadow=0;
	}
	if (hr!=D3D_OK || hr2!=D3D_OK) {
		if (g_pShadowMap) {
			logger("ref of shadow buffer %d\n",getrefcount(g_pShadowMap));
			g_pShadowMap->Release();
			g_pShadowMap=0;
		}
		if (g_pDSShadow) {
			logger("ref of shadow depth %d\n",getrefcount(g_pDSShadow));
			g_pDSShadow->Release();
			g_pDSShadow=0;
		}
		video3dinfo.canshadow=false;
	} else {
		video3dinfo.canshadow=true;
	}
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_BORDERCOLOR ,C32WHITE.c32);
#if 0
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR);
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR);
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR);
#endif
#endif
}

void dx9_init()
{
	model3::createshaders();

// create a white texture
	whitetex=texture_create("white.pcx");
	if (texture_getrc(whitetex)==1) {
		colorkeyinfo.usecolorkey=0;
		pushandsetdir("common");
		whitetex->load();
		popdir();
		colorkeyinfo.usecolorkey=1;
	}
// create a sysfont texture
	sysfonttex=texture_create("sysfont.pcx");
	if (texture_getrc(sysfonttex)==1) {
		colorkeyinfo.usecolorkey=1; // 0 for opaque fonts, 1 for alpha fonts
		pushandsetdir("common");
		sysfonttex->load();
		popdir();
		colorkeyinfo.usecolorkey=1;
	}
// create a larger sysfont texture
	sysfonttex2=texture_create("font0.png");
	if (texture_getrc(sysfonttex2)==1) {
		colorkeyinfo.usecolorkey=1; // 0 for opaque fonts, 1 for alpha fonts
		pushandsetdir("common");
		sysfonttex2->load();
		popdir();
		colorkeyinfo.usecolorkey=1;
	}
// Create the shadow map texture
	reset_d3dpool_default();
//	videoinfodx9.d3d9device->SetRenderState(D3DRS_CLIPPING,false); // play with no clipping
//	videoinfodx9.d3d9device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE); // fixes hardware vert bug (sigh)
	video3dinfo.cantoon=true;
	video3dinfo.is3dhardware=true;
	float pointSize = 3.0f;
	videoinfodx9.d3d9device->SetRenderState(D3DRS_POINTSIZE,*((DWORD*)&pointSize));
}

void dx9_uninit()
{
	model3::destroyshaders();
	textureb::rc.deleterc(whitetex); // move this to d2_vid3d
	textureb::rc.deleterc(sysfonttex);
	textureb::rc.deleterc(sysfonttex2);
	whitetex=0;
	sysfonttex=0;
	sysfonttex2=0;
	release_d3dpool_default();
	video3dinfo.cantoon=false;
	video3dinfo.is3dhardware=false;
}

//#define DOQUICKS
void dx9_beginscene()
{
		videoinfodx9.d3d9device->BeginScene();
}

//#define DOQUICKS
void dx9_setviewport(viewport2* vpi)
{
	viewport2 vp=*vpi;
	C32 c=vp.backcolor;
#ifdef DOQUICKS
	static bool quick;
	if (quick) {
		if ((vp.flags&(VP_CLEARBG|VP_CLEARWB))==(VP_CLEARBG|VP_CLEARWB))
			videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, c.c32, 1, 0 );
		else if (vp.flags&VP_CLEARWB)
			videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_ZBUFFER, 0, 1, 0 );
		else if (vp.flags&VP_CLEARBG)
			videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_TARGET, c.c32, 1, 0 );
		if (first)
			videoinfodx9.d3d9device->BeginScene();
		return;
	}
	quick=true;
#endif
	if (video3dinfo.inshadowdraw) {
		video3dinfo.w2lc=video3dinfo.w2c;
	}
	worldEyePos.x=vp.v2w.e[3][0];	worldEyePos.y=vp.v2w.e[3][1];	worldEyePos.z=vp.v2w.e[3][2];
	D3DVIEWPORT9 vp9;
	vp9.X=vp.xstart;
	vp9.Y=vp.ystart;
    vp9.Width=vp.xres;
    vp9.Height=vp.yres;
    vp9.MinZ=0.0f;
    vp9.MaxZ=1.0f;
	HRESULT hr;
	hr=videoinfodx9.d3d9device->SetViewport(&vp9);
	if (hr!=D3D_OK)
		errorexit("bad SetViewport");
	if ((vp.flags&(VP_CLEARBG|VP_CLEARWB))==(VP_CLEARBG|VP_CLEARWB))
		videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, c.c32, 1, 0 );
	else if (vp.flags&VP_CLEARWB)
		videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_ZBUFFER, 0, 1, 0 );
	else if (vp.flags&VP_CLEARBG)
		videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_TARGET, c.c32, 1, 0 );
	if (video3dinfo.zenable==1) {
		hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZENABLE,TRUE);
		hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	} else if (video3dinfo.zenable==0) {
		hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZENABLE,FALSE);
		hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	}
	videoinfodx9.d3d9device->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS ,*(DWORD *)(&videoinfodx9.miplodbias));
	videoinfodx9.d3d9device->SetSamplerState(1,D3DSAMP_MIPMAPLODBIAS ,*(DWORD *)(&videoinfodx9.miplodbias));
// play with fog
	{

#if 1 // pixel fog, good for camzooms of 2 or greater. (dist aprox. equal z on high zooms) ('rotate cam to see' effect)
		D3DCOLOR Color=pointf3toC32(&videoinfodx9.fog.color).c32;
		if (videoinfodx9.fog.enable && !video3dinfo.inshadowdraw) {
// Enable fog blending.
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, TRUE);
// Set the fog color.
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGCOLOR, Color);
// Set fog parameters.
			videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGTABLEMODE, videoinfodx9.fog.mode);
			if( videoinfodx9.fog.mode == D3DFOG_LINEAR ) {
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&videoinfodx9.fog.start));
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&videoinfodx9.fog.end));
			} else {
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&videoinfodx9.fog.density));
			}
		} else {
// Disable fog blending.
			videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, FALSE);
			videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		}
	}
#else // vertex fog, set oFog in vertex shader, ranges from '0 fog'  to  '1 no fog' (not as good as pixel)
		if (videoinfodx9.fogenable) {
			D3DCOLOR Color=0;
//		bool enable=true;

//		enable=(userinta!=0);
			Color=floatcolortoC32(&userpointf3a).c32;
//		if (enable && !inshadowdraw) {
// Enable fog blending.
//		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, TRUE);
// Set the fog color.
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGCOLOR, Color);
		}
// Set fog parameters.
/*
			if( Mode == D3DFOG_LINEAR ) {
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
			} else {
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
				videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
			}
//			videoinfodx9.d3d9device->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
*/
//		} else {
// Disable fog blending.
	//		videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, FALSE);
//			videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
//		}
	}
#endif
	videoinfodx9.d3d9device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	videoinfodx9.d3d9device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//	videoinfodx9.d3d9device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ); // try some additive blending, fire
//	videoinfodx9.d3d9device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_BLENDFACTOR);
//	videoinfodx9.d3d9device->SetRenderState( D3DRS_TEXTUREFACTOR , 0x00000000 ); // doesn't work
	if (!treeinfo.treedissolvecutoffenable) {
		S32 ac=(S32)(255.0f*treeinfo.defaultdissolvecutoff);
		videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHAREF, ac);
	}
	videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
	videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	videoinfodx9.d3d9device->SetRenderState(D3DRS_CULLMODE,videoinfodx9.cullmode);
	videoinfodx9.d3d9device->SetRenderState(D3DRS_FILLMODE,videoinfodx9.fillmode);
// aye, extra special purpose custom (user) stuff
	hr=fx_globals[toonedgesize].gfx->SetFloat(fx_globals[toonedgesize].gh,videoinfodx9.toonedgesize);
	if (hr!=D3D_OK)
		errorexit("can't set toonedgesize");
	hr=fx_globals[stanx].gfx->SetFloat(fx_globals[stanx].gh,videoinfodx9.stanx);
	if (hr!=D3D_OK)
		errorexit("can't set stanx");
	hr=fx_globals[stany].gfx->SetFloat(fx_globals[stany].gh,videoinfodx9.stany);
	if (hr!=D3D_OK)
		errorexit("can't set stany"); 
	hr=fx_globals[edgesize].gfx->SetFloat(fx_globals[edgesize].gh,videoinfodx9.edgesize);
	if (hr!=D3D_OK)
		errorexit("can't set edgesize");
	hr=fx_globals[edgezoff].gfx->SetFloat(fx_globals[edgezoff].gh,videoinfodx9.edgezoff);
	if (hr!=D3D_OK)
		errorexit("can't set edgezoff"); 

	texture3 *t=dynamic_cast<texture3*>(get_envv2tex());
	if (t) {
		hr=fx_globals[envv2texg].gfx->SetTexture(fx_globals[envv2texg].gh,t->gettexhandle());
		if (hr!=D3D_OK)
			errorexit("can't set envv2texg"); 
	}
}

void dx9_endscene(bool dotextdump) // nothing right now
{
//	if (last) {
//		if (!video3dinfo.inshadowdraw) {
//			dx9_drawcursor();
	if (dotextdump)
		d2_drawtextque_do();
//		}
	videoinfodx9.d3d9device->EndScene();
//	}
}

#ifdef SHADOWMAP

static LPDIRECT3DSURFACE9 pOldRT;
static LPDIRECT3DSURFACE9 pOldDS;

bool dx9_rendertargetshadowmapfunc()
{
	if (!video3dinfo.canshadow)
		return false;
     HRESULT hr;

	{ // this gets rid of the 
					// render target was detected as bound, 
					// but couldn't detect if texture was actually used in rendering
	  // message, not sure why      ??
		hr=videoinfodx9.d3d9device->SetTexture(0,0);
		if (hr!=D3D_OK)
			errorexit("raw can't unset  texture0 %08x",hr);
		hr=videoinfodx9.d3d9device->SetTexture(1,0);
		if (hr!=D3D_OK)
			errorexit("raw can't unset  texture1 %08x",hr); 
//		logger("end set tex0 to NULL\n");
//		logger("end set tex1 to NULL\n");
	}
	pOldRT = 0;
	hr=videoinfodx9.d3d9device->GetRenderTarget( 0, &pOldRT );
	if (hr!=D3D_OK)
		errorexit("can't get rendertarget for dx9_rendertargetshadowmapfunc %08x",hr);
 
	LPDIRECT3DSURFACE9 pShadowSurf;
    hr= g_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf );
	if (hr!=D3D_OK)
		errorexit("can't get GetSurfaceLevel for dx9_rendertargetshadowmapfunc");
 
	hr=videoinfodx9.d3d9device->SetRenderTarget( 0, pShadowSurf );
	if (hr!=D3D_OK)
		errorexit("can't set SetRenderTarget for dx9_rendertargetshadowmapfunc");
	pShadowSurf->Release();
 
	pOldDS=0;
	hr=videoinfodx9.d3d9device->GetDepthStencilSurface( &pOldDS );
	if (hr!=D3D_OK)
		errorexit("can't get GetDepthStencilSurface for dx9_rendertargetshadowmapfunc");
 
	hr=videoinfodx9.d3d9device->SetDepthStencilSurface( g_pDSShadow );
	if (hr!=D3D_OK)
		errorexit("can't set SetDepthStencilSurface for dx9_rendertargetshadowmapfunc");
	video3dinfo.inshadowdraw=true;
	return true;
}

void dx9_rendertargetmainfunc()
{
    HRESULT hr;
	
	if( pOldDS ) {
        hr=videoinfodx9.d3d9device->SetDepthStencilSurface( pOldDS );
		if (hr!=D3D_OK)
			errorexit("can't set SetDepthStencilSurface for dx9_rendertargetmainfunc");
        pOldDS->Release();
    }
    hr=videoinfodx9.d3d9device->SetRenderTarget( 0, pOldRT );
	if (hr!=D3D_OK)
		errorexit("can't set SetRenderTarget for dx9_rendertargetmainfunc");
	pOldRT->Release();
	video3dinfo.inshadowdraw=false;
}

#else
bool dx9_rendertargetshadowmapfunc()
{
	return false;
}

void dx9_rendertargetmainfunc()
{
}

#endif

void model3::close()
{
	if (verts.size()==0)
		return;
/// get shader idx into dx9mater;
	S32 i,n=mats.size();
	S32 cvdi=-1;
// get shader idx and vertdecl idx
	dx9mater2.clear();
	for (i=0;i<n;++i) {
		dx9_mater2 m;
		m.shaderidx=findshaderidx(mats[i].name.c_str());
		if (i==0) {
			cvdi=shaders[m.shaderidx].vdi;
			if (cvdi<0)
				errorexit("can't find a vert decl for '%s'",mats[0].name.c_str());
		} else {
			if (cvdi!=shaders[m.shaderidx].vdi)
				errorexit("different shader vertdecls in model '%s'",name.c_str());
		}
		dx9mater2.push_back(m);
	}
	vertdeclidx=cvdi;

	S32 vsize=vdecls[vertdeclidx].sizeofvertex;
// Create the vertex buffer.
	S32 nv=verts.size();
	if (!videoinfodx9.hiendmachine && nv>=65536)
		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
	vertbytesize=nv*vsize;
	HRESULT hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
	if (hr!=D3D_OK)
		errorexit("can't make vertex buffer");
	hardvertbytesize = vertbytesize;
// create and copy verts to vertex buffer (custom functions)
	(this->*vdecls[vertdeclidx].cfunp)();
	updatefaces = true; // initial creation of model requires face update
	close_ib();
}

// reload vertex buffer, and face index buffer
void model3::update()
// TODO: lock and update faces if necessary
{
	S32 vsize=vdecls[vertdeclidx].sizeofvertex; // sizeof a hardare vertex element
	S32 newvertbytesize=verts.size()*vsize;

// grow hardware vertex buffer if necessary
	if (newvertbytesize > hardvertbytesize) {
		if (vb)
			vb->Release();
		//vb = 0;
		HRESULT hr=videoinfodx9.d3d9device->CreateVertexBuffer(newvertbytesize,
			D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
		if (hr!=D3D_OK)
			errorexit("update can't make vertex buffer");
		hardvertbytesize = newvertbytesize;
	}
	vertbytesize = newvertbytesize;
	// copy over vertex data to vertex buffer
	(this->*vdecls[vertdeclidx].cfunp)();

// grow hardware face index buffer if necessary
	close_ib();
#if 0
	U32 nf = faces.size();
	if (nf) {
		S32 sizeOfIndex = videoinfodx9.hiendmachine ? 4 : 2;
		S32 newfacebytesize=nf*3*sizeOfIndex;
		D3DFORMAT fmt = videoinfodx9.hiendmachine ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
		if (newfacebytesize > hardfacebytesize) {
			if (ib)
				ib->Release();
			ib = 0;
			HRESULT hr=videoinfodx9.d3d9device->CreateIndexBuffer(newfacebytesize,
				D3DUSAGE_WRITEONLY,fmt,D3DPOOL_MANAGED,&ib,0 );
			if (hr!=D3D_OK)
				errorexit("update can't make index buffer");
			hardfacebytesize = newfacebytesize;
		}
		facebytesize = newfacebytesize;
	}
#endif
}

void model3::close_VCT()
{
//	logger("in close VCT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VCT *vbc=new CUSTOMVERTEX_VCT[nv];
//	static U32 rndcol;
//	logger("nv = %d, tnv = %d\n",nv,tnv);
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
		vbc[i].col=0xffffffff;//rndcol; ARGB
//		rndcol+=0x58649321;
	}
	S32 tnv=uvs0.size();
	if (nv && tnv) {
		for (i=0;i<nv;++i) {
			vbc[i].tu=uvs0[i].u;
			vbc[i].tv=uvs0[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu=0;
			vbc[i].tv=0;
		}
	}
	S32 n=cverts.size();
	if (n != nv && n != 0)
		errorexit("n != nv");
	for (i=0;i<n;++i) {
		vbc[i].col=pointf3toC32(&cverts[i]).c32;
	} 
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VT()
{
//	logger("in close VT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VT *vbc=new CUSTOMVERTEX_VT[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	if (uvs0.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu=uvs0[i].u;
			vbc[i].tv=uvs0[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu=1/3.0f;
			vbc[i].tv=1/3.0f;
		}
	}
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VT2()
{
//	logger("in close VT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VT2 *vbc=new CUSTOMVERTEX_VT2[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	if (uvs0.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu0=uvs0[i].u;
			vbc[i].tv0=uvs0[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu0=1/3.0f;
			vbc[i].tv0=1/3.0f;
		}
	}
	if (uvs1.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu1=uvs1[i].u;
			vbc[i].tv1=uvs1[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu1=1/3.0f;
			vbc[i].tv1=1/3.0f;
		}
	}
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VNT()
{
//	logger("in close VNT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VNT *vbc=new CUSTOMVERTEX_VNT[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	if (uvs0.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu=uvs0[i].u;
			vbc[i].tv=uvs0[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu=1/3.0f;
			vbc[i].tv=1/3.0f;
		}
	}
	S32 n=norms.size();
	for (i=0;i<n;++i) {
		vbc[i].norm.x=norms[i].x;
		vbc[i].norm.y=norms[i].y;
		vbc[i].norm.z=norms[i].z;
	} 
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VNTM()
{
//	logger("in close VNT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VNTM *vbc=new CUSTOMVERTEX_VNTM[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	if (uvs0.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu=uvs0[i].u;
			vbc[i].tv=uvs0[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu=1/3.0f;
			vbc[i].tv=1/3.0f;
		}
	}
	S32 n=norms.size();
	for (i=0;i<n;++i) {
		vbc[i].norm.x=norms[i].x;
		vbc[i].norm.y=norms[i].y;
		vbc[i].norm.z=norms[i].z;
	} 
	n=tangents.size();
	for (i=0;i<n;++i) {
		vbc[i].tangent.x=tangents[i].x;
		vbc[i].tangent.y=tangents[i].y;
		vbc[i].tangent.z=tangents[i].z;
	} 
	n=binorms.size();
	for (i=0;i<n;++i) {
		vbc[i].binorm.x=binorms[i].x;
		vbc[i].binorm.y=binorms[i].y;
		vbc[i].binorm.z=binorms[i].z;
	} 
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VN()
{
//	logger("in close VNT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VN *vbc=new CUSTOMVERTEX_VN[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	S32 n=norms.size();
	for (i=0;i<n;++i) {
		vbc[i].norm.x=norms[i].x;
		vbc[i].norm.y=norms[i].y;
		vbc[i].norm.z=norms[i].z;
	} 
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VNT2()
{
//	logger("in close VNT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VNT2 *vbc=new CUSTOMVERTEX_VNT2[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	if (uvs0.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu0=uvs0[i].u;
			vbc[i].tv0=uvs0[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu0=1/3.0f;
			vbc[i].tv0=1/3.0f;
		}
	}
	if (uvs1.size()) {
		for (i=0;i<nv;++i) {
			vbc[i].tu1=uvs1[i].u;
			vbc[i].tv1=uvs1[i].v;
		}
	} else {
		for (i=0;i<nv;++i) {
			vbc[i].tu1=1/3.0f;
			vbc[i].tv1=1/3.0f;
		}
	}
	S32 n=norms.size();
	for (i=0;i<n;++i) {
		vbc[i].norm.x=norms[i].x;
		vbc[i].norm.y=norms[i].y;
		vbc[i].norm.z=norms[i].z;
	} 
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

void model3::close_VE()
{
//	logger("in close VNT\n");
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=verts.size();
	if (!nv)
		return;
	CUSTOMVERTEX_VE *vbc=new CUSTOMVERTEX_VE[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=verts[i].x;
		vbc[i].pos.y=verts[i].y;
		vbc[i].pos.z=verts[i].z;
	}
	nv=verts2.size();
	for (i=0;i<nv;++i) {
		vbc[i].pos2.x=verts2[i].x;
		vbc[i].pos2.y=verts2[i].y;
		vbc[i].pos2.z=verts2[i].z;
	}
	HRESULT hr;
// Create the vertex buffer.
//	if (!videoinfodx9.hiendmachine && nv>=65536)
//		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
//	S32 nbytes=nv*sizeof(CUSTOMVERTEX_VCT);
//	hr=videoinfodx9.d3d9device->CreateVertexBuffer(vertbytesize,
//		D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&vb,0 );
//	if (hr!=D3D_OK)
//		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,vertbytesize,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,vertbytesize);
	vb->Unlock();
	delete[] vbc;
}

// Create the index buffer.
// now used for update faces
void model3::close_ib()
{
	S32 i;
	HRESULT hr;
	S32 nf=faces.size();
	if (!nf)
		return;
	if (!updatefaces)
		return;
	updatefaces = false;
	S32 sizeOfIndex = videoinfodx9.hiendmachine ? 4 : 2;
	S32 newfacebytesize = sizeOfIndex*3*nf;
	if (videoinfodx9.hiendmachine) {
// 32 bit
		if (newfacebytesize > hardfacebytesize) {
			if (ib)
				ib->Release();
			hr=videoinfodx9.d3d9device->CreateIndexBuffer(newfacebytesize,
				D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&ib,0);
			if (hr!=D3D_OK)
				errorexit("can't make index buffer");
			hardfacebytesize = newfacebytesize;
		}
		U32* id=new U32[nf*3];
		U32 *idp;
		for (i=0,idp=id;i<nf;++i,idp+=3) {
			idp[0]=faces[i].vertidx[0];
			idp[1]=faces[i].vertidx[1];
			idp[2]=faces[i].vertidx[2];
		}
// Fill the index buffer.
		void* pIndices;
		hr=ib->Lock(0,0,&pIndices,0);
		if (hr!=D3D_OK)
			errorexit("can't lock index buffer");
		memcpy(pIndices,id,4*3*nf);
		delete[] id;
	} else {
// 16 bit
// Create the index buffer.
		if (newfacebytesize > hardfacebytesize) {
			if (ib)
				ib->Release();
			hr=videoinfodx9.d3d9device->CreateIndexBuffer(newfacebytesize,
				D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&ib,0);
			if (hr!=D3D_OK)
				errorexit("can't make index buffer");
			hardfacebytesize = newfacebytesize;
		}
		U16* id=new U16[nf*3];
		U16 *idp;
		for (i=0,idp=id;i<nf;++i,idp+=3) {
			idp[0]=faces[i].vertidx[0];
			idp[1]=faces[i].vertidx[1];
			idp[2]=faces[i].vertidx[2];
		}
// Fill the index buffer.
		void* pIndices;
		hr=ib->Lock(0,0,&pIndices,0);
		if (hr!=D3D_OK)
			errorexit("can't lock index buffer");
		memcpy(pIndices,id,2*3*nf);
		delete[] id;
	}
	ib->Unlock();
	facebytesize = newfacebytesize;
}
// apply lights to cverts
void model3::dolightsdestrec(mat4* o2w)
{
	struct pointf3 locallightdirs[MAXLIGHTS];
	struct pointf3 locallightpos[MAXLIGHTS];
	if (!lightinfo.uselights)
		return;
	perf_start(DOLIGHTS);
// convert lights from world to local, this should use a different matrix, (don't assume orthogonal)
	S32 nl;
	mat4 w2o;
	inversemat3d(o2w,&w2o);
	pointf3 localEyePos;
	xformvec(&w2o,&worldEyePos,&localEyePos);
	for (nl=0;nl<lightinfo.ndirlights;nl++) {
		pointf3* d=(pointf3*)(&lightinfo.light2world[nl].e[2][0]);
		xformdirinv(o2w,d,&locallightdirs[nl]);
		normalize3d(&locallightdirs[nl],&locallightdirs[nl]);
		const float* lp = lightinfo.light2world[nl].e[3];
		xformvec(&w2o,(const pointf3*)lp,&locallightpos[nl]);
		/*
		locallightpos[nl].x = lp[0];
		locallightpos[nl].y = lp[1];
		locallightpos[nl].z = lp[2];
		locallightpos[nl].w = lp[3];*/
	}
// get light info into some shader constants
	HRESULT hr;
// vertex shader
	hr=fx_globals[amblightcol].gfx->SetVector(fx_globals[amblightcol].gh,(D3DXVECTOR4*)&lightinfo.ambcolor);
	if (hr!=D3D_OK)
		errorexit("can't set amb color");
	hr=fx_globals[dirlightcol].gfx->SetVector(fx_globals[dirlightcol].gh,(D3DXVECTOR4*)&lightinfo.lightcolors[0]);
	if (hr!=D3D_OK)
		errorexit("can't set dirlit color");
	hr=fx_globals[dirlightdir].gfx->SetVector(fx_globals[dirlightdir].gh,(D3DXVECTOR4*)&locallightdirs[0]);
	if (hr!=D3D_OK)
		errorexit("can't set dirlit dir");
#define NEWLIGHTPOS
#ifdef NEWLIGHTPOS
	hr=fx_globals[lightpos].gfx->SetVector(fx_globals[lightpos].gh,(D3DXVECTOR4*)&locallightpos[0]);
	if (hr!=D3D_OK)
		errorexit("can't set light pos");
#endif
	hr=fx_globals[eyeposworld].gfx->SetVector(fx_globals[eyeposworld].gh,(D3DXVECTOR4*)&worldEyePos);
	if (hr!=D3D_OK)
		errorexit("can't set worldeyepos");
	hr=fx_globals[eyeposobj].gfx->SetVector(fx_globals[eyeposobj].gh,(D3DXVECTOR4*)&localEyePos);
	if (hr!=D3D_OK)
		errorexit("can't set localeyepos");

// done get light info into some shader constants
	perf_end(DOLIGHTS);
}
//#define DOQUICK // dangerous test for hardware vertex processing bug
// object to world
// object to clip (the complete matrix, standard for vertex shaders)
// tree color
// tree texture
// tree texture offset
// noclip flag, CLIP_IN draw everything(noclip), CLIP_CLIP investigate model further, CLIP_OUT don't draw
//		hardware ignores this flag
void model3::draw(mat4* o2w,mat4* o2c,pointf3* tc,float dco,textureb* tt,pointf2* tto,clipresult noclip)
{
	perf_start(DRAWTRIS);
#ifdef DOQUICK
	static bool quick=false;
	if (quick) goto qu1;
#endif
// should draw something by now...
//	logmat4(o2w,"o2w");
	dolightsdestrec(o2w);
	HRESULT hr;
	float uo=tto->x,vo=tto->y; // tex vel tex offsets
	hr=fx_globals[uvoffsetu].gfx->SetFloat(fx_globals[uvoffsetu].gh,uo);
	if (hr!=D3D_OK)
		errorexit("can't set tex u offset");
	hr=fx_globals[uvoffsetv].gfx->SetFloat(fx_globals[uvoffsetv].gh,vo);
	if (hr!=D3D_OK)
		errorexit("can't set tex v offset");
	hr=fx_globals[o2cmat].gfx->SetMatrix(fx_globals[o2cmat].gh,(D3DXMATRIX*)o2c);
	if (hr!=D3D_OK)
		errorexit("can't set effect matrix o2c");
	hr=fx_globals[o2wmat].gfx->SetMatrix(fx_globals[o2wmat].gh,(D3DXMATRIX*)o2w);
	if (hr!=D3D_OK)
		errorexit("can't set effect matrix o2w");
	if (dx9mater2[0].shaderidx==useshadowmapidx && !video3dinfo.inshadowdraw) {
// set o2lc shadowmap matrix
		mat4 o2lc;//,o2lct; // o2w * w2c * matProj
		mulmat4(o2w,&video3dinfo.w2lc,&o2lc);
//		logmat4(&o2lc,"o2lc");
		hr=fx_globals[o2lcmat].gfx->SetMatrix(fx_globals[o2lcmat].gh,(D3DXMATRIX*)&o2lc);
		if (hr!=D3D_OK)
			errorexit("can't set o2lc matrix");
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_POINT );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_POINT );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_POINT );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_ADDRESSU ,D3DTADDRESS_BORDER);
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_ADDRESSV ,D3DTADDRESS_BORDER);
	}
	hr=videoinfodx9.d3d9device->SetIndices(ib);
	if (hr!=D3D_OK)
		errorexit("can't set indices");
	if (!treeinfo.treedissolvecutoffenable) {
		dco = treeinfo.defaultdissolvecutoff; // override tree dissolve cutoff, use default
	}
	if (treeinfo.treedissolvecutoffenable) { // not default, use tree dco
		S32 ac=(S32)(255.0f*dco);
		videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHAREF, ac);
	}
	U32 npasses;
	const shadeslot* ssrs=0;
	if (video3dinfo.inshadowdraw) {
		S32 si=makeshadowmapidx;
//		logger("si = %d\n",si);
		ssrs=&shaders[si];
		hr=ssrs->fx->Begin(&npasses,D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE);
		if (hr!=D3D_OK)
			errorexit("rs: can't set effect begin %08x",hr);
		hr=ssrs->fx->BeginPass(0);
		if (hr!=D3D_OK)
			errorexit("rs: can't set effect beginpass %08x",hr);
	}
qu1:
	textureb* tss=0;
	if (tt) {
		tss=mats[0].thetexarr[0];
		mats[0].thetexarr[0]=tt;
	}
	vector<mater2>::const_iterator mtr=mats.begin();
	vector<group2>::const_iterator grp=groups.begin();
	S32 i,n=groups.size();
	for (i=0;i<n;++i,++mtr,++grp) {
		float dc=tc->w*mats[i].color.w;
		if (dc<dco)
			continue;
#ifdef DOQUICK
		if (quick) goto qu2;
#endif
		{
			bool hb = (mats[i].thetexarr[0] && mats[i].thetexarr[0]->hasalpha) || dc<treeinfo.dissolvestart;
			if (hb && !video3dinfo.inshadowdraw && !(mats[i].msflags&SMAT_NOALPHA)) {
				videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE ,true);
				videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, true); 
			} else {
				videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE ,false);
				videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, false); 
			}
		}
		
		S32 si=dx9mater2[i].shaderidx;
		if (!video3dinfo.canshadow && si==useshadowmapidx)
			si=pdiffidx; // downgrade
		const shadeslot& ss=shaders[si];
		if (!video3dinfo.inshadowdraw) {
//			logger("si = %d\n",si);
			hr=ss.fx->Begin(&npasses,D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE);
			if (hr!=D3D_OK)
				errorexit("can't set effect begin %08x",hr);
			hr=ss.fx->BeginPass(0);
			if (hr!=D3D_OK)
				errorexit("can't set effect beginpass %08x",hr);
		}

		const vdecl& vds=vdecls[ss.vdi];
		hr=videoinfodx9.d3d9device->SetVertexDeclaration(vds.vd);
		if (hr!=D3D_OK)
			errorexit("can't set vertex declaration");
		hr=videoinfodx9.d3d9device->SetStreamSource(0,vb,0,vds.sizeofvertex);
		if (hr!=D3D_OK)
			errorexit("can't set streamsource");
		if (video3dinfo.zenable==2) {
			if (mtr->msflags&SMAT_HASWBUFF) {
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZENABLE,TRUE);
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			} else {
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZENABLE,FALSE);
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			}
		}
		if (mtr->msflags&SMAT_CLAMPU)
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_CLAMP); // color defaults to 0,0,0,0
//			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_BORDER); // color defaults to 0,0,0,0
		if (mtr->msflags&SMAT_CLAMPV)
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_CLAMP); // which is what we want
//			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_BORDER); // which is what we want
		{
			float sp[4]={0,0,0,0};
			sp[0]=mats[i].specpow;
			sp[1]=mats[i].refl; // was videoinfodx9.tex01blend;
			sp[2]=videoinfodx9.shadowepsilon;
			sp[3]=mats[i].specstrength;
			hr=fx_globals[specpow].gfx->SetVector(fx_globals[specpow].gh,(D3DXVECTOR4*)sp);
			if (hr!=D3D_OK)
				errorexit("can't set effect specpow");
		}
		pointf3 mc;
		mc.x=tc->x*mats[i].color.x;
		mc.y=tc->y*mats[i].color.y;
		mc.z=tc->z*mats[i].color.z;
		mc.w=dc;
		pointf3 mc2;
		mc2.x=tc->x*mats[i].colorb.x;
		mc2.y=tc->y*mats[i].colorb.y;
		mc2.z=tc->z*mats[i].colorb.z;
		mc2.w=tc->w*mats[i].colorb.w;
		if (video3dinfo.inshadowdraw)
			mc.w=dco/dc; // shadow cutoff, real cool!
		hr=fx_globals[material_color].gfx->SetVector(fx_globals[material_color].gh,(D3DXVECTOR4*)&mc);
		if (hr!=D3D_OK)
			errorexit("can't set effect material color");
		hr=fx_globals[material_color2].gfx->SetVector(fx_globals[material_color2].gh,(D3DXVECTOR4*)&mc2);
		if (hr!=D3D_OK)
			errorexit("can't set effect material color2");
// get the right texture, main,white or shadowmap
		if (dx9mater2[0].shaderidx==useshadowmapidx && !video3dinfo.inshadowdraw && video3dinfo.canshadow) {
			texture3 *t;
			if (mtr->thetexarr[0])
				t=dynamic_cast<texture3*>(mtr->thetexarr[0]->snapshot());
			else
				t=0;
			if (! (t && t->texdata) )
				t=dynamic_cast<texture3*>(whitetex);
			if (t && t->texdata) {
				hr=fx_globals[tex].gfx->SetTexture(fx_globals[tex].gh,t->texdata);
				if (hr!=D3D_OK)
					errorexit("can't set effect texture");
//				logger("set tex0 to '%s'\n",t->name.c_str());
			}
			if (!video3dinfo.inshadowdraw) {
				if (video3dinfo.canshadow) {
					hr=fx_globals[tex2].gfx->SetTexture(fx_globals[tex2].gh,g_pShadowMap);
				} else {
					t=dynamic_cast<texture3*>(whitetex);
					if (t && t->texdata)
						hr=fx_globals[tex2].gfx->SetTexture(fx_globals[tex2].gh,t->texdata);
				}
				if (hr!=D3D_OK)
					errorexit("can't set shadowmap texture");
//				logger("set tex1 to shadowmap\n");
			} 
		} else {
			texture3 *t;
			if (mtr->thetexarr[0])
				t=dynamic_cast<texture3*>(mtr->thetexarr[0]->snapshot());
			else
				t=0;
			if (!(t && (t->texdata || t->isshadowmap)))
				t=dynamic_cast<texture3*>(whitetex);
			if (video3dinfo.inshadowdraw) // && (t->isshadowmap || t->iscubemap))
				t=dynamic_cast<texture3*>(whitetex);
			if (t->isshadowmap) {
				hr=fx_globals[tex].gfx->SetTexture(fx_globals[tex].gh,g_pShadowMap);
				if (hr!=D3D_OK)
					errorexit("can't set shadowmap texture0");
//				logger("set tex0 to shadowmap\n");
			} else {
				if (t && t->texdata) {
				hr=fx_globals[tex].gfx->SetTexture(fx_globals[tex].gh,t->texdata);
					if (hr!=D3D_OK)
						errorexit("can't set texture0 '%s'",t->name.c_str());
//				logger("set tex0 to '%s'\n",t->name.c_str());
				}
			}
			if (mtr->thetexarr[1])
				t=dynamic_cast<texture3*>(mtr->thetexarr[1]->snapshot());
			else
				t=0;
			if (t && t->texdata) {
				hr=fx_globals[tex2].gfx->SetTexture(fx_globals[tex2].gh,t->texdata);
				if (hr!=D3D_OK)
					errorexit("can't set texture1 '%s'",t->name.c_str());
//				logger("set tex1 to '%s'\n",t->name.c_str());
			} else {
				hr=fx_globals[tex2].gfx->SetTexture(fx_globals[tex2].gh,0);
				if (hr!=D3D_OK)
					errorexit("can't unset texture1");
//				logger("set tex1 to NULL\n");
			} 
		}
		if (videoinfodx9.fog.enable && !(mats[i].msflags&SMAT_HASNOFOG) && !video3dinfo.inshadowdraw)
// Enable fog blending.
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, TRUE);
		else
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, FALSE);
qu2:
		hr=D3D_OK;
		if (video3dinfo.inshadowdraw) {
			if (i==0)
				hr=ssrs->fx->CommitChanges();
		} else
			hr=ss.fx->CommitChanges();
		if (hr!=D3D_OK)
			errorexit("can't commit! %08x",hr);
//		if (!video3dinfo.inshadowdraw) {
			D3DPRIMITIVETYPE pt;
			if (mtr->msflags&SMAT_POINTS) {
				pt = D3DPT_POINTLIST;
				hr=videoinfodx9.d3d9device->DrawPrimitive(pt,
					grp->vertidx,grp->nvert);
			} else if (mtr->msflags&SMAT_LINES) {
				pt = D3DPT_LINELIST;
				hr=videoinfodx9.d3d9device->DrawPrimitive(pt,
					grp->vertidx,grp->nvert/2);
			} else { // triangles
				pt = D3DPT_TRIANGLELIST;
				hr=videoinfodx9.d3d9device->DrawIndexedPrimitive(pt,
					0,grp->vertidx,grp->nvert,3*grp->faceidx,grp->nface);
			}
			if (hr!=D3D_OK)
				errorexit("can't draw! %08x",hr);
/*
			if (video3dinfo.inshadowdraw)
				logger("drawing model '%s', material '%s'\n",name.c_str(),ssrs->matname.c_str());
			else
				logger("drawing model '%s', material '%s'\n",name.c_str(),ss.matname.c_str());
*/
//		}
#ifndef DOQUICK
		if (mtr->msflags&SMAT_CLAMPU)
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_WRAP);
		if (mtr->msflags&SMAT_CLAMPV)
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_WRAP);
#endif
		if (!video3dinfo.inshadowdraw) {
			hr=ss.fx->EndPass();
			if (hr!=D3D_OK)
				errorexit("can't set effect endpass %08x",hr);
			hr=ss.fx->End();
			if (hr!=D3D_OK)
				errorexit("can't set effect end %08x",hr);
		}
	}
#ifndef DOQUICK
	if (dx9mater2[0].shaderidx==useshadowmapidx && !video3dinfo.inshadowdraw) {
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_ADDRESSU ,D3DTADDRESS_WRAP);
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_ADDRESSV ,D3DTADDRESS_WRAP);
	} 
#endif
	perf_end(DRAWTRIS);
#ifdef DOQUICK
	quick=true;
#endif
	if (video3dinfo.inshadowdraw) {
/*		hr=fx_globals[tex].gfx->SetTexture(fx_globals[tex].gh,0);
		if (hr!=D3D_OK)
			errorexit("can't unset texture end");
		hr=fx_globals[tex2].gfx->SetTexture(fx_globals[tex2].gh,0);
		if (hr!=D3D_OK)
			errorexit("can't unset texture1 end");
		hr=ssrs->fx->CommitChanges();
		if (hr!=D3D_OK)
			errorexit("can't commit 2"); */
		hr=ssrs->fx->EndPass();
		if (hr!=D3D_OK)
			errorexit("rs: can't set effect endpass %08x",hr);
		hr=ssrs->fx->End();
		if (hr!=D3D_OK)
			errorexit("rs: can't set effect end %08x",hr);
	}
// why do these work better at unbinding texture render targets?
	if (!video3dinfo.inshadowdraw) {
/*		hr=videoinfodx9.d3d9device->SetTexture(0,0);
		if (hr!=D3D_OK)
			errorexit("raw can't set shadowmap texture0");
		hr=videoinfodx9.d3d9device->SetTexture(1,0);
		if (hr!=D3D_OK)
			errorexit("raw can't set shadowmap texture1"); 
		logger("end set tex0 to NULL\n");
		logger("end set tex1 to NULL\n"); */
	}
	if (tt) {
		mats[0].thetexarr[0]=tss;
	}
	//return CLIP_IN;
}

model3::~model3()
{
	if(vb)
		vb->Release();
	if (ib)
		ib->Release();
}

modelb* dx9_createmodel(const C8* n)
{
	return modelb::rc.newrc<model3>(n);
}

///////////////// dx9 texture //////////////////////////
struct texinfo {
	string name;
	void* texdata;
	pointi2 size;
	S32 levels;
	S32 format;
	S32 alphabits;
	texinfo(const C8* name);
	~texinfo();
	void showline();
//	static void setmipmap(bool a) {usemipmaps=a;}
//	static bool usemipmaps;
};

struct af {
	S32 fmt,alphabits;
};

static af afl[]={
	{D3DFMT_A8R8G8B8,8,},
	{D3DFMT_X8R8G8B8,0,},
	{MAKEFOURCC('D','X','T','1'),1},
	{MAKEFOURCC('D','X','T','3'),4},
	{MAKEFOURCC('D','X','T','5'),8},
};
#define NFMTS (sizeof(afl)/sizeof(afl[0]))

const C8* texture3::print4cc(U32 f)
{
	static C8 fcc[5];
	S32 i;
	for (i=0;i<4;++i) {
		if (!isprint(f&0xff)) {
			fcc[0]='\0';
			return fcc;
		}
		fcc[i]=f&0xff;
		f>>=8;
	}
	return fcc;
}

D3DCUBEMAP_FACES texture3::curcubeface;

texture3::~texture3()
{
	if (!isshadowmap)
		if (texdata)
			texdata->Release();
}

void texture3::load()
{
#ifdef SHADOWMAP
	if (!strcmp(name.c_str(),"shadowmap")) {
		isshadowmap=1;
		return;
	}
#endif
	string outstr;
	bool cub,six;
	bool res=checkname(name,outstr,cub,six);
	if (!res)
		return;
	bitmap32* b;
	b=gfxread32c(outstr.c_str(),six);
	if (cub) {
		if (b->size.x*3!=b->size.y*4)
			errorexit("bad cube map '%s' %d %d",outstr.c_str(),b->size.x,b->size.y);
	}
	addbitmap(b,cub);
	bitmap32free(b);
}

// don't free this texture, just call unlocktexture
bitmap32* texture3::locktexture()
{
	if (!texdata)
		return &dummyb;
	static bitmap32 lockt32;
	D3DLOCKED_RECT lr;
	HRESULT result;
	if (iscubemap) {
		LPDIRECT3DCUBETEXTURE9 texdatad=(LPDIRECT3DCUBETEXTURE9)(texdata);
		result=texdatad->LockRect(
			curcubeface,
			0,
			&lr,
			0,
			0);
	} else {
		LPDIRECT3DTEXTURE9 texdatad=(LPDIRECT3DTEXTURE9)(texdata);
		result=texdatad->LockRect(
			0,
			&lr,
			0,
			0);
	}
	if (result!=D3D_OK)
		errorexit("can't lock texture %08x",result);
	lockt32.cliprect.topleft.x=0;
	lockt32.cliprect.topleft.y=0;
	lockt32.cliprect.size=tsize;
	lockt32.size.x=lr.Pitch>>2;
	lockt32.size.y=tsize.y;
	lockt32.data=(C32*)lr.pBits;
	return &lockt32;
}

void texture3::unlocktexture()
{
	if (!texdata)
		return;
	HRESULT result;
	if (iscubemap) {
		LPDIRECT3DCUBETEXTURE9 texdatad=(LPDIRECT3DCUBETEXTURE9)(texdata);
		result=texdatad->UnlockRect(curcubeface,0);
	} else {
		LPDIRECT3DTEXTURE9 texdatad=(LPDIRECT3DTEXTURE9)(texdata);
		result=texdatad->UnlockRect(0);
	}
	if (result!=D3D_OK)
		errorexit("can't unlock texture %08x",result);
}

void texture3::addbitmap(const bitmap32* ba,bool iscube_a)
{
	bitmap32* b=bitmap32copy(ba);
	origsize=b->size;
// texture reducer to check vid card speedup
	if (videoinfo.texreduce && !iscube_a) { // not cubic map
		videoinfo.texreduce=range(0,videoinfo.texreduce,6);
		S32 i;
		for (i=0;i<videoinfo.texreduce;++i) {
			bitmap32* bt=bitmap32reduce(b);
			bitmap32free(b);
			b=bt;
		}
	}	
	HRESULT result;
	if (texdata)
		texdata->Release();
	D3DFORMAT fmt = colorkeyinfo.lasthascolorkey ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
	hasalpha= colorkeyinfo.lasthascolorkey!=0;
	D3DSURFACE_DESC sd;
	S32 cubsiz=0;
	S32 tr=0; // reduce texture if error occurs, up to 4 times
	bitmap32* br=b; // reference if tr==0 
	if (iscube_a) {
		if (videoinfodx9.ddcaps9.TextureCaps&D3DPTEXTURECAPS_CUBEMAP_POW2) {
			logger("dx9: non power of 2 cubemap textures\n");
			cubsiz=b->size.x>>2;
			S32 logu=ilog2(cubsiz);
			if ((1<<logu)!=cubsiz) {
				logger("WARNING ?, texture3cubemap::addbitmap '%s' not a power of 2, is %d,%d reduced to %d,%d\n",
					name.c_str(),b->size.x,b->size.y,(1<<logu)*4,(1<<logu)*3);
				br=bitmap32alloc((1<<logu)*4,(1<<logu)*3,C32BLACK);
				clipscaleblit32(b,br);
				tr=1;
			}
		}
	} else {
//		if (1) { // force pow2 textures
		if (videoinfodx9.ddcaps9.TextureCaps&(D3DPTEXTURECAPS_POW2|D3DPTEXTURECAPS_NONPOW2CONDITIONAL)) {
//			logger("dx9: non power of 2 textures\n");
			S32 logu=ilog2(b->size.x);
			S32 logv=ilog2(b->size.y);
			if ((1<<logu)!=b->size.x || (1<<logv)!=b->size.y) {
//				if (0) {
				if (b->size.x<=128) {
					//logger("WARNING ?, texture3::addbitmap '%s' not a power of 2, is %d,%d increased to %d,%d\n",
					//	name.c_str(),b->size.x,b->size.y,2<<logu,2<<logv);
					br=bitmap32alloc(2<<logu,2<<logv,C32BLACK);
					clipscaleblit32(b,br);
				} else {
					//logger("WARNING ?, texture3::addbitmap '%s' not a power of 2, is %d,%d reduced to %d,%d\n",
					//	name.c_str(),b->size.x,b->size.y,1<<logu,1<<logv);
					br=bitmap32alloc(1<<logu,1<<logv,C32BLACK);
					clipscaleblit32(b,br);
				}
/*				logger("WARNING ?, texture3::addbitmap '%s' not a power of 2, is %d,%d reduced to %d,%d\n",
					name.c_str(),b->size.x,b->size.y,1<<logu,1<<logv);
				br=bitmap32alloc(1<<logu,1<<logv,C32BLACK);
				clipscaleblit32(b,br); */
				tr=1;
			}
		}
	}
	while(1) {
		cubsiz=br->size.x>>2;
		if (iscube_a) {
			LPDIRECT3DCUBETEXTURE9 texdatad;
			result=videoinfodx9.d3d9device->CreateCubeTexture(cubsiz,1,
				videoinfodx9.usemip ? D3DUSAGE_AUTOGENMIPMAP : 0,fmt,D3DPOOL_MANAGED,&texdatad,0);
			texdata=texdatad;
			if (result!=D3D_OK) {
				if (tr==4)
					errorexit("can't create cube texture '%s' %08x",name.c_str(),result);
				logger("texture reducing from %d,%d to %d,%d\n",br->size.x,br->size.y,br->size.x>>1,br->size.y>>1);
				if (tr==0)
					br=bitmap32reduce(b);
				else {
					bitmap32* brt=br;
					br=bitmap32reduce(brt);
					bitmap32free(brt);
				}
				++tr;
				continue;
			}
			result=texdatad->GetLevelDesc(0,&sd);
		} else {
			LPDIRECT3DTEXTURE9 texdatad;
			result=videoinfodx9.d3d9device->CreateTexture( br->size.x,br->size.y,1,
			  videoinfodx9.usemip ? D3DUSAGE_AUTOGENMIPMAP :0,fmt,D3DPOOL_MANAGED,&texdatad,0);
			texdata=texdatad;
			if (result!=D3D_OK) {
				if (tr==4)
					errorexit("can't create texture '%s' %08x",name.c_str(),result);
				logger("texture reducing from %d,%d to %d,%d\n",br->size.x,br->size.y,br->size.x>>1,br->size.y>>1);
				if (tr==0)
					br=bitmap32reduce(b);
				else {
					bitmap32* brt=br;
					br=bitmap32reduce(brt);
					bitmap32free(brt);
				}
				++tr;
				continue;
			}
			result=texdatad->GetLevelDesc(0,&sd);
		}
		break;
	}
	iscubemap=iscube_a;
	if (result!=D3D_OK)
		errorexit("can't get level desc");
	tsize.x=sd.Width;
	tsize.y=sd.Height;
	texformat=sd.Format;
	U32 i;
	for (i=0;i<NFMTS;++i) {
		if (afl[i].fmt==texformat) {
			alphabits=afl[i].alphabits;
			break;
		}
	}
	if (i==NFMTS) {
		alphabits=0;
		logger("WARNING: unknown format '%s' %d $%08x 4cc '%s'\n",name.c_str(),texformat,texformat,print4cc(texformat));
	}
	bitmap32* lck;
	if (iscubemap) {
		setcurcubeface(D3DCUBEMAP_FACE_POSITIVE_Y);
		lck=locktexture();
		clipblit32(br,lck,cubsiz,0,0,0,cubsiz,cubsiz);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_NEGATIVE_X);
		lck=locktexture();
		clipblit32(br,lck,0,cubsiz,0,0,cubsiz,cubsiz);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_POSITIVE_Z);
		lck=locktexture();
		clipblit32(br,lck,cubsiz,cubsiz,0,0,cubsiz,cubsiz);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_POSITIVE_X);
		lck=locktexture();
		clipblit32(br,lck,2*cubsiz,cubsiz,0,0,cubsiz,cubsiz);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_NEGATIVE_Z);
		lck=locktexture();
		clipblit32(br,lck,3*cubsiz,cubsiz,0,0,cubsiz,cubsiz);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_NEGATIVE_Y);
		lck=locktexture();
		clipblit32(br,lck,cubsiz,2*cubsiz,0,0,cubsiz,cubsiz);
		unlocktexture();
	} else {
		lck=locktexture();
		clipblit32(br,lck,0,0,0,0,br->size.x,br->size.y);
		unlocktexture();
	}
	if (tr)
		bitmap32free(br);
	tsize=lck->size;
	bitmap32free(b);
}

textureb* dx9_createtexture(const C8* n)
{
	return textureb::rc.newrc<texture3>(n);
}
#endif
