#include "d2_dx9shaderswitch.h"
#ifdef USEASMSHADERS
#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "m_vid_dx9.h"
#include "d2_dx9.h"
//#include "d2_dx9b.h"
#include "m_perf.h"
//#define HALFPIXEL	// when defined, fix half pixel problem
#include "../engqtest/u_modelutil.h"
#include "d2_font.h"
#include "d2_dx9_font.h"

namespace d2_dx9 {
LPDIRECT3DTEXTURE9      g_pShadowMap;    // Texture to which the shadow map is rendered
LPDIRECT3DSURFACE9      g_pDSShadow;     // Depth-stencil buffer for rendering to shadow map
S32 makeshadowmapidx;	// override for light relative scene render
S32 useshadowmapidx;	// reference to 
S32 pdiffidx;			// if can't shadowmap
//S32 envidx;
// handy
pointf3 worldEyePos;
textureb* whitetex,*sysfonttex;
//bool canshadow;

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

D3DVERTEXELEMENT9 decl_vct[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0,},
	{ 0, 16,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vt[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vnt[] = {
	{ 0,  0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0,},
	{ 0, 24,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
D3DVERTEXELEMENT9 decl_vntm[] = {
	{ 0,  0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0,},
	{ 0, 24,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0,},
	{ 0, 36,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0,},
	{ 0, 48,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};

struct shadeslot {
	IDirect3DVertexShader9* vs;
	IDirect3DPixelShader9* ps;
	S32 vdi; /// vertex declaration index
	string matname;
};

vector<shadeslot> shaders;
//script* mat2shader;

} /// end anonymous namespace
using namespace d2_dx9;

struct model3::vdecl model3::vdecls[]=
{
	{"VCT",decl_vct,sizeof(CUSTOMVERTEX_VCT),&model3::close_VCT,0},
	{"VT",decl_vt,sizeof(CUSTOMVERTEX_VT),&model3::close_VT,0},
	{"VNT",decl_vnt,sizeof(CUSTOMVERTEX_VNT),&model3::close_VNT,0},
	{"VNTM",decl_vntm,sizeof(CUSTOMVERTEX_VNTM),&model3::close_VNTM,0},
};
const S32 model3::nvdecls=sizeof(model3::vdecls)/sizeof(model3::vdecls[0]);

void model3::createshaders()
{
#define SHADECOL 4 // number of columns in shadelistasm.txt
	HRESULT hr;
	pushandsetdir("shaders");
	logger("in create shaders\n");
/// first create the vertex declarations
	S32 i,j,n;
	for (i=0;i<nvdecls;++i) {
		hr=videoinfodx9.d3d9device->CreateVertexDeclaration(vdecls[i].ve, &vdecls[i].vd);
		if (hr!=D3D_OK)
			errorexit("bad createvertexdeclaration %x",hr);
	}
/// now create the shaders
	script shadelist("shadelistasm.txt");
/*	mat2shader=new script("mat2shader.txt");
	S32 na=mat2shader->num();
	if (na%2)
		errorexit("bad mat2shader.txt"); */
	n=shadelist.num();
	if ((n%SHADECOL)!=0)
		errorexit("bad shadelistasm.txt");
	for (i=0;i<n;i+=SHADECOL) {
		shadeslot ss;
/// setup vertex decl first
		for (j=0;j<nvdecls;++j) {
			if (!strcmp(shadelist.idx(i+3).c_str(),vdecls[j].name)) {
				ss.vdi=j;
				break;
			}
		}
		if (j==nvdecls)
			errorexit("can't find vertex declaration '%s'",shadelist.idx(i+3).c_str());
//		logger("shader tok is '%s'\n",shadelist.idx(i).c_str());
		ID3DXBuffer *pShaderBuffer=0;
		ID3DXBuffer *pShaderErrors=0;
/// vertex shader
		const C8* buff=fileload(shadelist.idx(i+1).c_str());
		hr=D3DXAssembleShader(buff, (UINT)strlen(buff), 0, NULL, 0,
			&pShaderBuffer, &pShaderErrors);
		delete[] buff;
		if (hr!=D3D_OK)
			errorexit("bad vertex assembleshader %x '%s' %s",hr,shadelist.idx(i+1).c_str(),pShaderErrors->GetBufferPointer());
//		logger("refcount before %d\n",getrefcount(pShaderBuffer));
		hr=videoinfodx9.d3d9device->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(),&ss.vs);
		if (hr!=D3D_OK)
			errorexit("bad createvertexshader %x for '%s'",hr,shadelist.idx(i+1).c_str());
		if (pShaderBuffer) {
//			logger("sizeof shaderbuffer is %d\n",pShaderBuffer->GetBufferSize());
			hr=pShaderBuffer->Release();
			if (hr!=D3D_OK)
				errorexit("bad release vshaderbuffer %d",hr);
			pShaderBuffer=0;
		}
		if (pShaderErrors) {
			logger("sizeof errorbuffer is %d\n",pShaderErrors->GetBufferSize());
			hr=pShaderErrors->Release();
			if (hr!=D3D_OK)
				errorexit("bad release verrorbuffer %d",hr);
			pShaderErrors=0;
		}
/// pixel shader
		buff=fileload(shadelist.idx(i+2).c_str());
		hr=D3DXAssembleShader(buff, (UINT)strlen(buff), 0, NULL, 0,
			&pShaderBuffer, &pShaderErrors);
		delete[] buff;
		if (hr!=D3D_OK)
			errorexit("bad pixel assembleshader %x '%s' %s",hr,shadelist.idx(i+2).c_str(),pShaderErrors->GetBufferPointer());
//		logger("refcount before %d\n",getrefcount(pShaderBuffer));
		hr=videoinfodx9.d3d9device->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(),&ss.ps);
		if (hr!=D3D_OK)
			errorexit("bad pixel createpixelshader %x",hr,shadelist.idx(i+2).c_str());
//		logger("refcount after %d\n",getrefcount(pShaderBuffer));
		if (pShaderBuffer) {
//			logger("sizeof shaderbuffer is %d\n",pShaderBuffer->GetBufferSize());
			hr=pShaderBuffer->Release();
			if (hr!=D3D_OK)
				errorexit("bad release pshaderbuffer %d",hr);
			pShaderBuffer=0;
		}
		if (pShaderErrors) {
			logger("sizeof errorbuffer is %d\n",pShaderErrors->GetBufferSize());
			hr=pShaderErrors->Release();
			if (hr!=D3D_OK)
				errorexit("bad release perrorbuffer %d",hr);
			pShaderErrors=0;
		}
		ss.matname=shadelist.idx(i+0);
		shaders.push_back(ss);
	}
	popdir();
	makeshadowmapidx=findshaderidx("makeshadowmap");
	useshadowmapidx=findshaderidx("useshadowmap");
	pdiffidx=findshaderidx("pdiff"); // for downgrade if no shadowmap
//	envidx=findshaderidx("env");
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
		if (shaders[i].vs) {
			hr=shaders[i].vs->Release();
			if (hr!=D3D_OK)
				errorexit("bad vshader release %d",hr);
		}
		if (shaders[i].ps) {
	//		logger("shaderp refcount before %d\n",getrefcount(m_pPixelShader));
			hr=shaders[i].ps->Release();
	//		logger("shaderv refcount after %d\n",getrefcount(m_pPixelShader));
			if (hr!=D3D_OK)
				errorexit("bad pshader release %d",hr);
		}
	}
	shaders.clear();
//	delete mat2shader;
//	mat2shader=0;
}

S32 model3::findshaderidx(const C8* namem)
{
// first do a mat2shader
	const C8* names;
	S32 i,n;
	if (!videoinfo.mat2shader)
		errorexit("can't find mat2shader");
	n=videoinfo.mat2shader->num();
	if (!n)
		errorexit("findshaderidx not inited");
	for (i=0;i<n;i+=2) {
		if (videoinfo.mat2shader->idx(i)==namem)
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
	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );
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
// Create the shadow map texture
	reset_d3dpool_default();
//	videoinfodx9.d3d9device->SetRenderState(D3DRS_CLIPPING,false); // play with no clipping
//	videoinfodx9.d3d9device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE); // fixes hardware vert bug (sigh)
	video3dinfo.cantoon=false;
	video3dinfo.is3dhardware=true;
}

void dx9_uninit()
{
	model3::destroyshaders();
	textureb::rc.deleterc(whitetex);
	textureb::rc.deleterc(sysfonttex);
	whitetex=0;
	sysfonttex=0;
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
	videoinfodx9.d3d9device->SetViewport(&vp9);
	if ((vp.flags&(VP_CLEARBG|VP_CLEARWB))==(VP_CLEARBG|VP_CLEARWB))
		videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, c.c32, 1, 0 );
	else if (vp.flags&VP_CLEARWB)
		videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_ZBUFFER, 0, 1, 0 );
	else if (vp.flags&VP_CLEARBG)
		videoinfodx9.d3d9device->Clear( 0,0, D3DCLEAR_TARGET, c.c32, 1, 0 );
	HRESULT hr;
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

#if 1 // pixel
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
#else // vertex, set oFog in vertex shader, ranges from '0 fog'  to  '1 no fog'
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
//	S32 ac=(S32)(255.0f*treeinfo.dissolvecutoff);
	if (!treeinfo.treedissolvecutoffenable) {
		S32 ac=(S32)(255.0f*treeinfo.defaultdissolvecutoff);
		videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHAREF, ac);
	}
//	videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHAREF, ac);
	videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
	videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	videoinfodx9.d3d9device->SetRenderState(D3DRS_CULLMODE,videoinfodx9.cullmode);
	videoinfodx9.d3d9device->SetRenderState(D3DRS_FILLMODE,videoinfodx9.fillmode);
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

	pOldRT = 0;
	hr=videoinfodx9.d3d9device->GetRenderTarget( 0, &pOldRT );
	if (hr!=D3D_OK)
		errorexit("can't get rendertarget for dx9_rendertargetshadowmapfunc");
 
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
// create and copy verts to vertex buffer (custom functions)
	(this->*vdecls[vertdeclidx].cfunp)();
	close_ib();
}

// reload vertex buffer
void model3::update()
{
	(this->*vdecls[vertdeclidx].cfunp)();
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
		vbc[i].col=0xffffffff;//rndcol;
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

// Create the index buffer.
void model3::close_ib()
{
	S32 i;
	HRESULT hr;
	S32 nf=faces.size();
	if (!nf)
		return;
	if (videoinfodx9.hiendmachine) {
// 32 bit
		U32* id=new U32[nf*3];
		U32 *idp;
		for (i=0,idp=id;i<nf;++i,idp+=3) {
			idp[0]=faces[i].vertidx[0];
			idp[1]=faces[i].vertidx[1];
			idp[2]=faces[i].vertidx[2];
		}
		hr=videoinfodx9.d3d9device->CreateIndexBuffer(4*3*nf,
			D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&ib,0);
		if (hr!=D3D_OK)
			errorexit("can't make index buffer");
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
		U16* id=new U16[nf*3];
		U16 *idp;
		for (i=0,idp=id;i<nf;++i,idp+=3) {
			idp[0]=faces[i].vertidx[0];
			idp[1]=faces[i].vertidx[1];
			idp[2]=faces[i].vertidx[2];
		}
		hr=videoinfodx9.d3d9device->CreateIndexBuffer(2*3*nf,
			D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&ib,0);
		if (hr!=D3D_OK)
			errorexit("can't make index buffer");
// Fill the index buffer.
		void* pIndices;
		hr=ib->Lock(0,0,&pIndices,0);
		if (hr!=D3D_OK)
			errorexit("can't lock index buffer");
		memcpy(pIndices,id,2*3*nf);
		delete[] id;
	}
	ib->Unlock();
}

// apply lights to cverts
void model3::dolightsdestrec(mat4* o2w)
{
	struct pointf3 locallightdirs[MAXLIGHTS];
	if (!lightinfo.uselights)
		return;
	perf_start(DOLIGHTS);
// convert lights from world to local, this should use a different matrix, (don't assume orthogonal)
	S32 nl;
	mat4 w2o;
	inversemat3d(o2w,&w2o);
	pointf3 localEyePos;
	xformvec(&w2o,&d2_dx9::worldEyePos,&localEyePos);
	for (nl=0;nl<lightinfo.ndirlights;nl++) {
		pointf3* d=(pointf3*)(&lightinfo.light2world[nl].e[2][0]);
		xformdirinv(o2w,d,&locallightdirs[nl]);
		normalize3d(&locallightdirs[nl],&locallightdirs[nl]);
	}
// get light info into some shader constants
	HRESULT hr;
// vertex shader
	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(12,(float*)&lightinfo.ambcolor,1);
	if (hr!=D3D_OK)
		errorexit("can't set vertex ambcolor");

	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(5,(float*)&lightinfo.lightcolors[0], 1);
	if (hr!=D3D_OK)
		errorexit("can't set vertex light0color");

	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(4,(float*)&locallightdirs[0], 1);
	if (hr!=D3D_OK)
		errorexit("can't set vertex light0dir");

	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(14,(float*)&d2_dx9::worldEyePos,1);
	if (hr!=D3D_OK)
		errorexit("can't set vertex worldeyepos");

	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(11,(float*)&localEyePos,1);
	if (hr!=D3D_OK)
		errorexit("can't set vertex localeyepos");

// pixel shader
	hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(5,(float*)&lightinfo.ambcolor,1);
	if (hr!=D3D_OK)
		errorexit("can't set pixel lightambcolor");

	hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(2,(float*)&lightinfo.lightcolors[0],1);
	if (hr!=D3D_OK)
		errorexit("can't set pixel light0color");

	hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(1,(float*)&locallightdirs[0],1);
	if (hr!=D3D_OK)
		errorexit("can't set pixel light0dir");
// done get light info into some shader constants
	perf_end(DOLIGHTS);
}
//#define DOQUICK // dangerous test for hardware vertex processing bug
clipresult model3::draw(mat4* o2w,mat4* o2c,pointf3* tc,float dco,textureb* tt,pointf2* tto,clipresult noclip)
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
	mat4 o2ct; // o2w * w2c * matProj
	transposemat4(o2c, &o2ct);
	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(0,(float*)&o2ct, 4);
	mat4 o2wt; // o2w * w2c * matProj
	transposemat4(o2w, &o2wt);
	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(7,(float*)&o2wt, 4);
	if (dx9mater2[0].shaderidx==useshadowmapidx && !video3dinfo.inshadowdraw) {
// set o2lc shadowmap matrix
		mat4 o2lc,o2lct; // o2w * w2c * matProj
		mulmat4(o2w,&video3dinfo.w2lc,&o2lc);
//		logmat4(&o2lc,"o2lc");
		transposemat4(&o2lc, &o2lct);
		hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(16,(float*)&o2lct, 4);
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_POINT );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_POINT );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_POINT );
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_ADDRESSU ,D3DTADDRESS_BORDER);
		videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_ADDRESSV ,D3DTADDRESS_BORDER);
	}
	hr=videoinfodx9.d3d9device->SetIndices(ib);
	if (hr!=D3D_OK)
		errorexit("can't set indices");
	if (video3dinfo.inshadowdraw) {
		S32 si=makeshadowmapidx;
//		logger("si = %d\n",si);
		const shadeslot& ss=shaders[si];
		hr=videoinfodx9.d3d9device->SetVertexShader(ss.vs);
		if (hr!=D3D_OK)
			errorexit("can't set vertex shader");
		hr=videoinfodx9.d3d9device->SetPixelShader(ss.ps);
		if (hr!=D3D_OK)
			errorexit("can't set pixel shader");
	}
qu1:
	textureb* tss;
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
		bool hb = (mats[i].thetexarr[0] && mats[i].thetexarr[0]->hasalpha) || dc<treeinfo.dissolvestart;
		if (hb && !video3dinfo.inshadowdraw && !(mats[i].msflags&SMAT_NOALPHA)) {
			videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE ,true);
			videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, true); 
		} else {
			videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE ,false);
			videoinfodx9.d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, false); 
		}
		S32 si=dx9mater2[i].shaderidx;
		if (!video3dinfo.canshadow && si==useshadowmapidx)
			si=pdiffidx; // downgrade
		const shadeslot& ss=shaders[si];
		if (!video3dinfo.inshadowdraw) {
//			logger("si = %d\n",si);
			hr=videoinfodx9.d3d9device->SetVertexShader(ss.vs);
			if (hr!=D3D_OK)
				errorexit("can't set vertex shader");
			hr=videoinfodx9.d3d9device->SetPixelShader(ss.ps);
			if (hr!=D3D_OK)
				errorexit("can't set pixel shader");
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
		if (mtr->msflags&SMAT_CLAMP) {
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_CLAMP);
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_CLAMP);
		}
		{
			float sp[4]={0,0,0,0};
			sp[0]=mats[i].specpow;
			sp[1]=mats[i].refl;
			sp[2]=videoinfodx9.shadowepsilon;
			hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(13,sp,1);
			if (hr!=D3D_OK)
				errorexit("can't set vertex specpower");
	
			hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(8,sp,1);
			if (hr!=D3D_OK)
				errorexit("can't set pixel specpower");
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
		hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(6,(float*)&mc,1);
		if (hr!=D3D_OK)
			errorexit("can't set pixel matcolor");
		hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(9,(float*)&mc2,1);
		if (hr!=D3D_OK)
			errorexit("can't set pixel matcolor2");
// get the right texture, main,white or shadowmap
		if (dx9mater2[0].shaderidx==useshadowmapidx && !video3dinfo.inshadowdraw) { // draw the shadow from main,shadowmap
			texture3 *t;
			t=dynamic_cast<texture3*>(mtr->thetexarr[0]);
			if (!(t && t->texdata))
				t=dynamic_cast<texture3*>(whitetex);
			if (t && t->texdata) {
				hr=videoinfodx9.d3d9device->SetTexture(0,t->texdata);
				if (hr!=D3D_OK)
					errorexit("can't set texture0 '%s'",t->name.c_str());
			}
			hr=videoinfodx9.d3d9device->SetTexture(1,g_pShadowMap);
			if (hr!=D3D_OK)
				errorexit("can't set shadowmap texture");
		} else {
			texture3 *t;
			t=dynamic_cast<texture3*>(mtr->thetexarr[0]);
			if (!(t && (t->texdata || t->isshadowmap)))
				t=dynamic_cast<texture3*>(whitetex);
			if (video3dinfo.inshadowdraw && (t->isshadowmap || t->iscubemap))
				t=dynamic_cast<texture3*>(whitetex);
			if (t->isshadowmap) {
				hr=videoinfodx9.d3d9device->SetTexture(0,g_pShadowMap);
				if (hr!=D3D_OK)
					errorexit("can't set shadowmap texture0");
			} else {
				if (t && t->texdata) {
					hr=videoinfodx9.d3d9device->SetTexture(0,t->texdata);
					if (hr!=D3D_OK)
						errorexit("can't set texture0 '%s'",t->name.c_str());
				}
			}
			t=dynamic_cast<texture3*>(mtr->thetexarr[1]);
			if (t && t->texdata) {
				hr=videoinfodx9.d3d9device->SetTexture(1,t->texdata);
				if (hr!=D3D_OK)
					errorexit("can't set texture1 '%s'",t->name.c_str());
			} else {
				hr=videoinfodx9.d3d9device->SetTexture(1,0);
				if (hr!=D3D_OK)
					errorexit("can't unset texture1");
			} 
		}
		if (videoinfodx9.fog.enable && !(mats[i].msflags&SMAT_HASNOFOG) && !video3dinfo.inshadowdraw)
// Enable fog blending.
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, TRUE);
		else
		    videoinfodx9.d3d9device->SetRenderState(D3DRS_FOGENABLE, FALSE);
qu2:
//		if (!video3dinfo.inshadowdraw) {
				hr=videoinfodx9.d3d9device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
					0,grp->vertidx,grp->nvert,3*grp->faceidx,grp->nface);
			if (hr!=D3D_OK)
				errorexit("can't draw! %08x",hr);
//		}
#ifndef DOQUICK
		if (mtr->msflags&SMAT_CLAMP) {
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_WRAP);
			videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_WRAP);
		} 
#endif
		hr=videoinfodx9.d3d9device->SetTexture(0,0);
		if (hr!=D3D_OK)
			errorexit("can't unset texture0");
		hr=videoinfodx9.d3d9device->SetTexture(1,0);
		if (hr!=D3D_OK)
			errorexit("can't unset texture1");
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
	if (tt) {
		mats[0].thetexarr[0]=tss;
	}
	return CLIP_IN;
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
	HRESULT result;
	if (texdata)
		texdata->Release();
	D3DFORMAT fmt = colorkeyinfo.lasthascolorkey ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
	hasalpha= colorkeyinfo.lasthascolorkey!=0;
	D3DSURFACE_DESC sd;
	S32 cs=0;
	S32 tr=0; // reduce texture if error occurs, up to 4 times
	bitmap32* br=b; // reference if tr==0 
	if (iscube_a) {
		if (videoinfodx9.ddcaps9.TextureCaps&D3DPTEXTURECAPS_CUBEMAP_POW2) {
			logger("dx9: non power of 2 cubemap textures\n");
			cs=b->size.x>>2;
			S32 logu=ilog2(cs);
			if ((1<<logu)!=cs) {
				logger("WARNING ?, texture3cubemap::addbitmap '%s' not a power of 2, is %d,%d reduced to %d,%d\n",
					name.c_str(),b->size.x,b->size.y,(1<<logu)*4,(1<<logu)*3);
				br=bitmap32alloc((1<<logu)*4,(1<<logu)*3,C32BLACK);
				clipscaleblit32(b,br);
				tr=1;
			}
		}
	} else {
//		if (1) {
		if (videoinfodx9.ddcaps9.TextureCaps&(D3DPTEXTURECAPS_POW2|D3DPTEXTURECAPS_NONPOW2CONDITIONAL)) {
//			logger("dx9: non power of 2 textures\n");
			S32 logu=ilog2(b->size.x);
			S32 logv=ilog2(b->size.y);
			if ((1<<logu)!=b->size.x || (1<<logv)!=b->size.y) {
				if (b->size.x<=128) {
					logger("WARNING ?, texture3::addbitmap '%s' not a power of 2, is %d,%d increased to %d,%d\n",
						name.c_str(),b->size.x,b->size.y,2<<logu,2<<logv);
					br=bitmap32alloc(2<<logu,2<<logv,C32BLACK);
					clipscaleblit32(b,br);
				} else {
					logger("WARNING ?, texture3::addbitmap '%s' not a power of 2, is %d,%d reduced to %d,%d\n",
						name.c_str(),b->size.x,b->size.y,1<<logu,1<<logv);
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
		cs=br->size.x>>2;
		if (iscube_a) {
			LPDIRECT3DCUBETEXTURE9 texdatad;
			result=videoinfodx9.d3d9device->CreateCubeTexture(cs,1,
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
		clipblit32(br,lck,cs,0,0,0,cs,cs);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_NEGATIVE_X);
		lck=locktexture();
		clipblit32(br,lck,0,cs,0,0,cs,cs);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_POSITIVE_Z);
		lck=locktexture();
		clipblit32(br,lck,cs,cs,0,0,cs,cs);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_POSITIVE_X);
		lck=locktexture();
		clipblit32(br,lck,2*cs,cs,0,0,cs,cs);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_NEGATIVE_Z);
		lck=locktexture();
		clipblit32(br,lck,3*cs,cs,0,0,cs,cs);
		unlocktexture();

		setcurcubeface(D3DCUBEMAP_FACE_NEGATIVE_Y);
		lck=locktexture();
		clipblit32(br,lck,cs,2*cs,0,0,cs,cs);
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
