#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "m_vid_dx9.h"
#include "d2_dx9.h"
//#include "d2_dx9b.h"
#include "m_perf.h"

namespace {
// handy
S32 getrefcount(IUnknown* i)
{
	i->AddRef();
	return i->Release();
}

// create vertex and pixel shader
D3DVERTEXELEMENT9 decl[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
	{ 0, 12,D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0,},
	{ 0, 16,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0,},
	D3DDECL_END()
};
const char* BasicVertexShader = 
		"vs_1_1             \n"
		"dcl_position v0    \n"
		"dcl_color  v1      \n"
		"dcl_texcoord  v2      \n"
		"m4x4 oPos, v0, c0  \n"
		"add oT0, v2,c16 \n"
		"mov oD0, v1        \n;";
const char* BasicPixelShader = 
		"ps_1_1             \n"
		"def c0, 1.0f, 1.0f, 1.0f, 1.0f \n"
		"tex t0    \n"
		"mul r0,t0,v0    \n"
		"sub r0,c0,r0\n";
struct CUSTOMVERTEXpnt3
{
    D3DVECTOR pos;//float x,y,z;      // The untransformed, 3D position for the vertex
    D3DCOLOR col;//float x,y,z;      // The untransformed, 3D position for the vertex
	float tu,tv;
};

IDirect3DVertexDeclaration9* m_pVertexDeclaration;
IDirect3DVertexShader9* m_pVertexShader;
IDirect3DPixelShader9* m_pPixelShader;
//#define D3DFVF_CUSTOMVERTEXpnt3 (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
}

void createshaders()
{
	logger("in create shaders\n");
	ID3DXBuffer *pShaderBuffer;
	ID3DXBuffer *pShaderErrors;
//	IDirect3DVertexShader9* m_pVertexShader;
//	IDirect3DVertexDeclaration9* m_pVertexDeclaration;
	HRESULT hr;
/// create vertex shader
//	HRESULT hr=D3DXAssembleShader(BasicVertexShader, (UINT)strlen(BasicVertexShader), 0, NULL, D3DXSHADER_DEBUG,
	hr=D3DXAssembleShader(BasicVertexShader, (UINT)strlen(BasicVertexShader), 0, NULL, 0,
		&pShaderBuffer, &pShaderErrors);
	if (hr!=D3D_OK)
		errorexit("bad vertex assembleshader %x %s",hr,pShaderErrors->GetBufferPointer());

//	logger("refcount before %d\n",getrefcount(pShaderBuffer));
	hr=videoinfodx9.d3d9device->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(),&m_pVertexShader);
	if (hr!=D3D_OK)
		errorexit("bad createvertexshader %x",hr);
//	logger("refcount after %d\n",getrefcount(pShaderBuffer));


	hr=videoinfodx9.d3d9device->CreateVertexDeclaration(decl, &m_pVertexDeclaration);
	if (hr!=D3D_OK)
		errorexit("bad createvertexdeclaration %x",hr);

//	return m_pD3DDevice->SetVertexShader(m_pVertexShader);

	if (pShaderBuffer) {
//		logger("sizeof shaderbuffer is %d\n",pShaderBuffer->GetBufferSize());
		hr=pShaderBuffer->Release();
		if (hr!=D3D_OK)
			errorexit("bad release vshaderbuffer %d",hr);
	}

	if (pShaderErrors) {
		logger("sizeof errorbuffer is %d\n",pShaderErrors->GetBufferSize());
		hr=pShaderErrors->Release();
		if (hr!=D3D_OK)
			errorexit("bad release verrorbuffer %d",hr);
	}
//	const float offset[2]={0,0};
	const float foffset[2]={.125f,.25f};
//	HRESULT hr;
	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(16,(float*)&foffset, 2);
	if (hr!=D3D_OK)
		errorexit("can't set vertex constant");

/// now create pixel shader
	hr=D3DXAssembleShader(BasicPixelShader, (UINT)strlen(BasicPixelShader), 0, NULL, 0,
		&pShaderBuffer, &pShaderErrors);
	if (hr!=D3D_OK)
		errorexit("bad pixel assembleshader %x %s",hr,pShaderErrors->GetBufferPointer());

//	logger("refcount before %d\n",getrefcount(pShaderBuffer));
	hr=videoinfodx9.d3d9device->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(),&m_pPixelShader);
	if (hr!=D3D_OK)
		errorexit("bad pixel createpixelshader %x",hr);
//	logger("refcount after %d\n",getrefcount(pShaderBuffer));

	if (pShaderBuffer) {
//		logger("sizeof shaderbuffer is %d\n",pShaderBuffer->GetBufferSize());
		hr=pShaderBuffer->Release();
		if (hr!=D3D_OK)
			errorexit("bad release pshaderbuffer %d",hr);
	}

	if (pShaderErrors) {
		logger("sizeof errorbuffer is %d\n",pShaderErrors->GetBufferSize());
		hr=pShaderErrors->Release();
		if (hr!=D3D_OK)
			errorexit("bad release perrorbuffer %d",hr);
	}
//	const float offset[2]={0,0};
	const float poffset[2]={.125f,.25f};
//	HRESULT hr;
	hr=videoinfodx9.d3d9device->SetPixelShaderConstantF(16,(float*)&poffset, 2);
	if (hr!=D3D_OK)
		errorexit("can't set pixel constant");

}

void destroyshaders()
{
	HRESULT hr;
	if (m_pVertexShader) {
//		logger("shaderv refcount before %d\n",getrefcount(m_pVertexShader));
		hr=m_pVertexShader->Release();
//		logger("shaderv refcount after %d\n",getrefcount(m_pVertexShader));
		if (hr!=D3D_OK)
			errorexit("bad vshader release %d",hr);
	}
	if (m_pVertexDeclaration) {
		hr=m_pVertexDeclaration->Release();
		if (hr!=D3D_OK)
			errorexit("bad vdecl release %d",hr);
	}
	if (m_pPixelShader) {
//		logger("shaderp refcount before %d\n",getrefcount(m_pPixelShader));
		hr=m_pVertexShader->Release();
//		logger("shaderv refcount after %d\n",getrefcount(m_pPixelShader));
		if (hr!=D3D_OK)
			errorexit("bad pshader release %d",hr);
	}
}

#if 1
//extern float hard_zfront,hard_zback;
//extern mat4 hard_vpw2c;	// includes cam zoom factors for bbox check
// curently all handled in m_vid_dx9.cpp (2d)
#endif
#if 0
void dx9_init()
{
/*float c[4] = {0.0f,0.5f,1.0f,2.0f};
DWORD dwDecl0[] = {
  D3DVSD_STREAM(0),
  D3DVSD_REG(0, D3DVSDT_FLOAT3 ),    // input register v0
  D3DVSD_REG(5, D3DVSDT_D3DCOLOR ),  // input Register v5
                                     // set a few constants
  D3DVSD_CONST(0,1),*(DWORD*)&c[0],*(DWORD*)&c[1],*(DWORD*)&c[2],*(DWORD*)&c[3],
  D3DVSD_END()
};*/
LPD3DXEFFECT anEffect;
D3DXCreateEffectFromFile(videoinfodx9.d3d9device,"fxfname.fx",NULL,NULL,0,NULL,&anEffect,NULL);

	
HRESULT hr=D3DXCompileShader(
  0,//LPCSTR pSrcData,
  0,//UINT srcDataLen,
  0,//CONST D3DXMACRO* pDefines,
  0,//LPD3DXINCLUDE pInclude,
  0,//LPCSTR pFunctionName,
  0,//LPCSTR pProfile,
  0,//DWORD Flags,
  0,//LPD3DXBUFFER* ppShader,
  0,//LPD3DXBUFFER* ppErrorMsgs,
  0//LPD3DXCONSTANTTABLE * ppConstantTable
);
/*HRESULT D3DXCompileShader(
  LPCSTR pSrcData,
  UINT srcDataLen,
  CONST D3DXMACRO* pDefines,
  LPD3DXINCLUDE pInclude,
  LPCSTR pFunctionName,
  LPCSTR pProfile,
  DWORD Flags,
  LPD3DXBUFFER* ppShader,
  LPD3DXBUFFER* ppErrorMsgs,
  LPD3DXCONSTANTTABLE * ppConstantTable
);
*/
}
#endif


//FVF
/*struct BASIC_SHADER_VERTEX
{
	float x, y, z;
	DWORD color;
};
*/


/*static D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0,},
		{ 0, 12,D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0,},
			D3DDECL_END()
	}; */
// : ;// {}
#if 0
model3b::model3b(string namea) : model3(namea),m_pVertexShader(0),m_pVertexDeclaration(0)
//void model3b::createshader()
{
}

/*void model3b::destroyshader()
{
}
*/
#endif
#if 0


		return E_FAIL;

	if(FAILED(m_pD3DDevice->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(),
		&m_pVertexShader)))
		return E_FAIL;

	pShaderBuffer->Release();

	if(FAILED(m_pD3DDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration)))
		return E_FAIL;

	return m_pD3DDevice->SetVertexShader(m_pVertexShader);

//create vertex buffer
	if(FAILED(m_pD3DDevice->CreateVertexBuffer(BASIC_SHADER_VERTEX_NUM * 
		sizeof(BASIC_SHADER_VERTEX), D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_IN_SHADER_1,
		D3DPOOL_DEFAULT, &m_pVertexBuffer, 0)))
		return FALSE;

	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(BASIC_SHADER_VERTEX));
	FillVertexBuffer();

//fill vertex buffer
	BASIC_SHADER_VERTEX *pVertices;

	if(FAILED(m_pVertexBuffer->Lock(0, BASIC_SHADER_VERTEX_NUM * 
		sizeof(BASIC_SHADER_VERTEX), reinterpret_cast<void**>(&pVertices), 0)))
	{
		DestroyVertexBuffer();
		return FALSE;
	}

	for(int index = 0; index < BASIC_SHADER_VERTEX_NUM; ++index)
	{
		float Angle = 2.0f * D3DX_PI * ((float)index / (float)BASIC_SHADER_VERTEX_NUM);

		pVertices[index].x = cos(Angle);
		pVertices[index].y = sin(Angle);
		pVertices[index].z = 5.0f;
		pVertices[index].color = 0xffffffff;
	}

	m_pVertexBuffer->Unlock();
//render codes
	D3DXMATRIX RotationMatrix1;
	D3DXMATRIX RotationMatrix2;
	D3DXMATRIX TranslationMatrix;
	D3DXMATRIX ScalingMatrix;

	D3DXMatrixRotationZ(&RotationMatrix1, static_cast<float>(GetTickCount() / 1000.0f));
	D3DXMatrixRotationZ(&RotationMatrix2, static_cast<float>(GetTickCount() / 1000.0f));
	D3DXMatrixTranslation(&TranslationMatrix, 3.0f, 0.0f, 0.0f);
	D3DXMatrixScaling(&ScalingMatrix, 1.0f, 0.5f, 1.0f);

	m_WorldMatrix = ScalingMatrix * RotationMatrix2 * TranslationMatrix 
		* RotationMatrix1;

	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
	
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	m_pD3DDevice->SetVertexShaderConstantF(0,(float*)&ShaderMatrix, 4);

//	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

//	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, BASIC_SHADER_VERTEX_NUM);
//try to move
m_pD3DDevice->SetVertexShader(m_pVertexShader);
//and
m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(BASIC_SHADER_VERTEX));

//inside your rendering loop instead of at creation and

//and I can't find where you call:
m_pD3DDevice->SetVertexDeclaration(m_pVertexDeclaration);
//neither

//try to put it close to SetVertexShader / SetStreamSource
#endif

/*model3b::~model3b()
{
}
*/
// configure for
void model3b::close()
{
// Fill out CUSTOMVERTEXpnt2 array
	S32 i;//,n;
	S32 nv=tempverts.size();
	if (!nv)
		return;
	CUSTOMVERTEXpnt3 *vbc=new CUSTOMVERTEXpnt3[nv];
	for (i=0;i<nv;++i) {
		vbc[i].pos.x=tempverts[i].x;
		vbc[i].pos.y=tempverts[i].y;
		vbc[i].pos.z=tempverts[i].z;
		vbc[i].col=0xffddbb;//.x=0;
//		vbc[i].col=0xff0000;//.x=0;
//		vbc[i].col.y=0;
//		vbc[i].col.z=0;
	}
//	n=tempuvs.size();
	for (i=0;i<nv;++i) {
		vbc[i].tu=tempuvs[i].u;
		vbc[i].tv=tempuvs[i].v;
	}
/*	n=tempnorms.size();
	for (i=0;i<n;++i) {
		vbc[i].norm.x=tempnorms[i].x;
		vbc[i].norm.y=tempnorms[i].y;
		vbc[i].norm.z=tempnorms[i].z;
	} */
	tempverts.clear();
	tempuvs.clear();
	tempnorms.clear();
	nverts=nv;
// Create the vertex buffer.
	if (!videoinfodx9.hiendmachine && nv>=65536)
		errorexit("too many verts for hiendmachine==0 '%s' '%d'",name.c_str(),nv);
	S32 nbytes=nv*sizeof(CUSTOMVERTEXpnt3);
//	HRESULT hr=videoinfodx9.d3d9device->CreateVertexBuffer(nbytes,
//		0,D3DFVF_CUSTOMVERTEXpnt3,D3DPOOL_DEFAULT,&vb,0 );
	HRESULT hr=videoinfodx9.d3d9device->CreateVertexBuffer(nbytes,
		0,0,D3DPOOL_DEFAULT,&vb,0 );
	if (hr!=D3D_OK)
		errorexit("can't make vertex buffer");
// Fill the vertex buffer.
	void* pVertices;
	hr=vb->Lock( 0,nbytes,&pVertices,0);
	if (hr!=D3D_OK)
		errorexit("can't lock vertex buffer");
	memcpy(pVertices,vbc,nbytes);
	vb->Unlock();
	delete[] vbc;
// Create the index buffer.
	S32 nf=tempfaces.size();
	if (videoinfodx9.hiendmachine) {
// 32 bit
		U32* id=new U32[nf*3];
		U32 *idp;
		for (i=0,idp=id;i<nf;++i,idp+=3) {
			idp[0]=tempfaces[i].vertidx[0];
			idp[1]=tempfaces[i].vertidx[1];
			idp[2]=tempfaces[i].vertidx[2];
		}
		tempfaces.clear();
		hr=videoinfodx9.d3d9device->CreateIndexBuffer(4*3*nf,
			0,D3DFMT_INDEX32,D3DPOOL_DEFAULT,&ib,0);
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
			idp[0]=tempfaces[i].vertidx[0];
			idp[1]=tempfaces[i].vertidx[1];
			idp[2]=tempfaces[i].vertidx[2];
		}
		tempfaces.clear();
		hr=videoinfodx9.d3d9device->CreateIndexBuffer(2*3*nf,
			0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&ib,0);
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
	nfaces=nf;
//	createshader();
}

clipresult model3b::draw(mat4* o2w,float dissolve,pointf3* color)
{
	perf_start(DRAWTRIS);
	if (!vb) {
		perf_end(DRAWMODEL);
		return CLIP_OUT;
	}
	if (dissolve<treeinfo.dissolvecutoff) {
		perf_end(DRAWMODEL);
		return CLIP_OUT;
	}
	mat4 o2c;
	mulmat3d(o2w,&hard_vpw2c,&o2c);
	clipresult noclip=checkbbox(&o2c);
	if (noclip==CLIP_OUT) {
		perf_end(DRAWMODEL);
		return CLIP_OUT;
	}
//	logmat4(o2w,"o2w");
	HRESULT hr;
//	hr=videoinfodx9.d3d9device->SetTransform(D3DTS_WORLD,(D3DMATRIX*) o2w);
//	if (hr!=D3D_OK)
//		errorexit("cant set world transform");
	hr=videoinfodx9.d3d9device->SetStreamSource(0,vb,0,sizeof(CUSTOMVERTEXpnt3));
	if (hr!=D3D_OK)
		errorexit("cant set streamsource");
#if 0
	hr=videoinfodx9.d3d9device->SetFVF(D3DFVF_CUSTOMVERTEXpnt2);
	if (hr!=D3D_OK)
		errorexit("cant set FVF");
#else
//try to move
	hr=videoinfodx9.d3d9device->SetVertexShader(m_pVertexShader);
	if (hr!=D3D_OK)
		errorexit("cant set vertex shader");
//try to move
	hr=videoinfodx9.d3d9device->SetPixelShader(m_pPixelShader);
	if (hr!=D3D_OK)
		errorexit("cant set pixel shader");
//and
	hr=videoinfodx9.d3d9device->SetVertexDeclaration(m_pVertexDeclaration);
	if (hr!=D3D_OK)
		errorexit("cant set vertex declaration");
//	hr=videoinfodx9.d3d9device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(BASIC_SHADER_VERTEX));
	mat4 ShaderMatrix;// = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
	
	mulmat4(o2w,&matViewPers,&ShaderMatrix);
	mulmat4(&ShaderMatrix,&matProjPers,&ShaderMatrix);
	transposemat4(&ShaderMatrix, &ShaderMatrix);

	hr=videoinfodx9.d3d9device->SetVertexShaderConstantF(0,(float*)&ShaderMatrix, 4);
//	const float offset[2]={.125f,.25f};
#endif
	hr=videoinfodx9.d3d9device->SetIndices(ib);
	if (hr!=D3D_OK)
		errorexit("cant set indices");
	S32 i,n=groups.size();
	vector<mater2>::const_iterator mtr=mats.begin();
	vector<group2>::const_iterator grp=groups.begin();
	for (i=0;i<n;++i,++mtr,++grp) {
		if (video3dinfo.zenable==2) {
			if (mtr->msflags&SMAT_HASWBUFF) {
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZENABLE,TRUE);
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			} else {
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZENABLE,FALSE);
				hr=videoinfodx9.d3d9device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			}
		}
		if (mtr->thetex) {
			texture3 *t=dynamic_cast<texture3*>(mtr->thetex);
			if (t && t->texdata) {
				hr=videoinfodx9.d3d9device->SetTexture(0,t->texdata);
				if (hr!=D3D_OK)
					errorexit("cant set texture '%s'",t->name.c_str());
			}
		} 
		hr=videoinfodx9.d3d9device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0,grp->vertidx,grp->nvert,3*grp->faceidx,grp->nface);
		if (hr!=D3D_OK)
			errorexit("can't draw! %08x",hr);
	}
	perf_end(DRAWTRIS);
	return CLIP_IN;
}
