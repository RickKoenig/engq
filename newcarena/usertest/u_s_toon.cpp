// simple sphere with spinning lights
#define INCLUDE_WINDOWS
#define D2_3D
#include <m_eng.h>
#include <d3dx9core.h>
#include <m_vid_dx9.h>
#include "m_perf.h"

#include "system/u_states.h"
#include "utils/modelutil.h"
#include "d2_font.h"

#include "u_s_toon.h"

static S32 curcar=0;
static  const char* const carnames[]={
	"coin01.bwo",
	"badmudd.bws",
	"duneitup.bws",
	"draggintail.bws",
	"covelight.bws",
};
static  const char* const cardirs[]={
	"cointest",
	"toon/toontestcars/easy",
	"toon/toontestcars/easy",
	"toon/toontestcars/tough",
	"toon/toontestcars/tough",
};
static const S32 NCARS = sizeof(carnames)/sizeof(carnames[0]);
static U32 togvidmode;

static tree2* roottree;

static bool defsave;

// a single tri
// newer one with textures
static pointf3 otverts[]={
	{-10, 10,0},
	{ 10, 10,0},
	{  0,-10,0},
};

static struct uv otuvs0[]={
	{0,0},
	{2,0},
	{0,2},
};

static struct uv otuvs1[]={
	{0,0},
	{1,0},
	{0,1},
};

static pointf3 otnorms[]={
	{ 0, 0,-1},
	{ 0, 0,-1},
	{ 0, 0,-1},
};

static struct face otface[]={
	{{0,1,2},0},
};
// end of a single tri

static tree2* buildsphere(const C8* modelname,const C8* matname)
{
	modelb* mod2=model_create("sphere3");
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,35,35,3,3,spheref_surf(20),"bark.tga",matname);
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree3");
	spheretree2->setmodel(mod2);
	return spheretree2;
}

static tree2* singletri()
{
	tree2* singletri=new tree2("oneface1");
	modelb* mod;
	mod=model_create("onetri");
	if (model_getrc(mod)==1) {
		mod->copyverts(otverts,3);
		mod->copyuvs0(otuvs0,3);
		mod->copyuvs1(otuvs1,3);
		mod->copynorms(otnorms,3);
		textureb* texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		mod->addfaces(otface,1,0);
		mod->addmat("toon",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,30,1,3);
//		mod->addmat("stan",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,30,1,3);
		mod->close();
	}
	singletri->setmodel(mod);
	return singletri;
}

// old attempt, (move uvs in) bad at vertices..
static void toonify(tree2* t)
{
// show old model
	modelb* tm=t->mod;
	if (!tm)
		return;
	U32 i,j,k,el;
	const pointf3* v;
	U32 nv;
	v=tm->getverts(&nv);
	for (i=0;i<nv;++i)
		logger("toon vert %4d: (%f,%f,%f)\n",i,v[i].x,v[i].y,v[i].z);

	const uv* uvs0;
	U32 nuvs0;
	uvs0=tm->getuvs0(&nuvs0);
	for (i=0;i<nuvs0;++i)
		logger("toon uvs0 %4d: (%f,%f)\n",i,uvs0[i].u,uvs0[i].v);

	const pointf3* n;
	U32 nn;
	n=tm->getnorms(&nn);
	for (i=0;i<nn;++i)
		logger("toon norm %4d: (%f,%f,%f)\n",i,n[i].x,n[i].y,n[i].z);

	const face* f;
	U32 nf;
	f=tm->getfaces(&nf);
	for (i=0;i<nf;++i)
		logger("toon face %4d: (%d,%d,%d)\n",i,f[i].vertidx[0],f[i].vertidx[1],f[i].vertidx[2]);


// indep. vertex model
	vector<pointf3> iv;
	vector<uv> iuvs;
	vector<pointf3> inorms;
	for (i=0;i<nf;++i) {
		for (k=0;k<3;++k) {
			j=f[i].vertidx[k];
			iv.push_back(v[j]);
			iuvs.push_back(uvs0[j]);
			inorms.push_back(n[j]);
		}
	}
// find edges, for now brute force it.
	static const float cosang=.7f;
//	static const float cosang=-1;
	static const float muv=.2f;
	vector<uv> iuvs1(nf*3);
	for (i=0;i<nf;++i) {
		iuvs1[3*i+0].u=0;
		iuvs1[3*i+0].v=0;
		iuvs1[3*i+1].u=1;
		iuvs1[3*i+1].v=0;
		iuvs1[3*i+2].u=0;
		iuvs1[3*i+2].v=1;
	}
	for (i=0;i<nf;++i) {
		for (k=0;k<3;++k) {
			U32 k2=(k+1)%3;
			const pointf3& e0_0=iv[i*3+k];
			const pointf3& e0_1=iv[i*3+k2];
			for (j=0;j<nf;++j) {
				for (el=0;el<3;++el) {
					U32 el2=(el+1)%3;
					const pointf3& e1_0=iv[j*3+el];
					const pointf3& e1_1=iv[j*3+el2];
					if (e0_0.x==e1_1.x && e0_0.y==e1_1.y && e0_0.z==e1_1.z) {
						if (e0_1.x==e1_0.x && e0_1.y==e1_0.y && e0_1.z==e1_0.z) {
							float d0=dot3d(&inorms[i*3+k],&inorms[j*3+el2]);
							float d1=dot3d(&inorms[i*3+k2],&inorms[j*3+el]);
							if (d0>cosang && d1>cosang) {
								logger("smooth edge found at face %d: %d-%d and face %d: %d-%d\n",
								  i,k,k2,j,el,el2);
								switch(k) {
								case 0:
									iuvs1[3*i+0].v+=muv;
									iuvs1[3*i+1].u-=muv;
									iuvs1[3*i+1].v+=muv;
									break;
								case 1:
									iuvs1[3*i+1].u-=muv;
									iuvs1[3*i+2].v-=muv;
									break;
								case 2:
									iuvs1[3*i+0].u+=muv;
									iuvs1[3*i+2].u+=muv;
									iuvs1[3*i+2].v-=muv;
									break;
								}
							} else {
								logger("hard edge found at face %d: %d-%d and face %d: %d-%d\n",
								  i,k,k2,j,el,el2);
							}
						}
					}
				}
			}
		}
	}
// replace model
	modelb* mod2;
	mod2=model_create("toon2");
	if (model_getrc(mod2)==1) {
		mod2->copyverts(iv);
		mod2->copyuvs0(iuvs);
		mod2->copyuvs1(iuvs1);
		mod2->copynorms(inorms);
		textureb* texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		for (i=0;i<nf;++i) {
			mod2->addface(3*i,3*i+1,3*i+2);
		}
		mod2->addmat("toon",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,30,nf,nf*3);
//		mod->addmat("stan",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,30,1,3);
		mod2->close();
	}
	t->setmodel(mod2);
}

void edgeify(tree2* tb)
{
	if (!video3dinfo.cantoon)
		return;
// show old model
	list<tree2*>::iterator it;
	for (it=tb->children.begin();it!=tb->children.end();++it) {
		edgeify(*it);
//		break;
	}
	tree2* t=new tree2("toonedge");
	t->trans=pointf3x();
	t->rot=pointf3x();
	t->scale=pointf3x(1,1,1);
	t->buildo2p=O2P_NONE;
	tb->linkchild(t);
	modelb* tm=tb->mod;
	if (!tm)
		return;
	U32 i,j,k,el;
	const pointf3* v;
	U32 nv;
	v=tm->getverts(&nv);
//	for (i=0;i<nv;++i)
//		logger("toon vert %4d: (%f,%f,%f)\n",i,v[i].x,v[i].y,v[i].z);

	const uv* uvs0;
	U32 nuvs0;
	uvs0=tm->getuvs0(&nuvs0);
//	for (i=0;i<nuvs0;++i)
//		logger("toon uvs0 %4d: (%f,%f)\n",i,uvs0[i].u,uvs0[i].v);

	const pointf3* n;
	U32 nn;
	n=tm->getnorms(&nn);
//	for (i=0;i<nn;++i)
//		logger("toon norm %4d: (%f,%f,%f)\n",i,n[i].x,n[i].y,n[i].z);

	const face* f;
	U32 nf;
	f=tm->getfaces(&nf);
//	for (i=0;i<nf;++i)
//		logger("toon face %4d: (%d,%d,%d)\n",i,f[i].vertidx[0],f[i].vertidx[1],f[i].vertidx[2]);

// indep. vertex model
	vector<pointf3> iv;
	vector<uv> iuvs;
	vector<pointf3> inorms;
	for (i=0;i<nf;++i) {
		for (k=0;k<3;++k) {
			j=f[i].vertidx[k];
			iv.push_back(v[j]);
			iuvs.push_back(uvs0[j]);
			inorms.push_back(n[j]);
		}
	}
// find edges, for now brute force it.
//	static const float cosang=-1;
//	static const float muv=.2f;
//	vector<uv> iuvs1(nf*3);
	vector<bool> marked(nf*3);
	vector<bool> ishard(nf*3);

/*
	for (i=0;i<nf;++i) {
		iuvs1[3*i+0].u=0;
		iuvs1[3*i+0].v=0;
		iuvs1[3*i+1].u=1;
		iuvs1[3*i+1].v=0;
		iuvs1[3*i+2].u=0;
		iuvs1[3*i+2].v=1;
	}
*/
	for (i=0;i<nf;++i) {
		for (k=0;k<3;++k) {
			if (marked[i*3+k])
				continue;
			U32 k2=(k+1)%3;
			const pointf3& e0_0=iv[i*3+k];
			const pointf3& e0_1=iv[i*3+k2];
			for (j=i+1;j<nf;++j) {
				for (el=0;el<3;++el) {
					U32 el2=(el+1)%3;
					const pointf3& e1_0=iv[j*3+el];
					const pointf3& e1_1=iv[j*3+el2];
					if (e0_0.x==e1_1.x && e0_0.y==e1_1.y && e0_0.z==e1_1.z) {
						if (e0_1.x==e1_0.x && e0_1.y==e1_0.y && e0_1.z==e1_0.z) {
							float d0=dot3d(&inorms[i*3+k],&inorms[j*3+el2]);
							float d1=dot3d(&inorms[i*3+k2],&inorms[j*3+el]);
							if (d0>videoinfodx9.edgecosang && d1>videoinfodx9.edgecosang) {
//							if (true) {
//								logger("smooth edge found at face %d: %d-%d and face %d: %d-%d\n",
//								  i,k,k2,j,el,el2);
/*								switch(k) {
								case 0:
									iuvs1[3*i+0].v+=muv;
									iuvs1[3*i+1].u-=muv;
									iuvs1[3*i+1].v+=muv;
									break;
								case 1:
									iuvs1[3*i+1].u-=muv;
									iuvs1[3*i+2].v-=muv;
									break;
								case 2:
									iuvs1[3*i+0].u+=muv;
									iuvs1[3*i+2].u+=muv;
									iuvs1[3*i+2].v-=muv;
									break;
								}
								switch(el) {
								case 0:
									iuvs1[3*j+0].v+=muv;
									iuvs1[3*j+1].u-=muv;
									iuvs1[3*j+1].v+=muv;
									break;
								case 1:
									iuvs1[3*j+1].u-=muv;
									iuvs1[3*j+2].v-=muv;
									break;
								case 2:
									iuvs1[3*j+0].u+=muv;
									iuvs1[3*j+2].u+=muv;
									iuvs1[3*j+2].v-=muv;
									break;
								}
								issmooth[i*3+k]=true;
								issmooth[j*3+el]=true; */
							} else {
//								logger("hard edge found at face %d: %d-%d and face %d: %d-%d\n",
//								  i,k,k2,j,el,el2);
								ishard[i*3+k]=true;
							}
							marked[j*3+el]=true;
							break;
						}
					}
				}
				if (el!=3)
					break;
			}
			if (j==nf) {
//				logger("can't find edge match at face %d: %d-%d\n",i,k,k2);
				ishard[i*3+k]=true;
			}
		}
	}
	vector<pointf3> newedges;
	vector<pointf3> newedges2;
	pointf3 d0,d1,d2,d3;
	for (i=0;i<nf;++i) {
		for (k=0;k<3;++k) {
			if (ishard[i*3+k]) {
				U32 k2=(k+1)%3;
//				logger("hard edge at face %d: %d-%d\n",i,k,k2);
				pointf3 vt0,vt1;
				vt0=iv[i*3+k];
				vt1=iv[i*3+k2];
#define DIFFSIZE_E 1
				d0.x=vt0.x+DIFFSIZE_E*(vt1.x-vt0.x);
				d0.y=vt0.y+DIFFSIZE_E*(vt1.y-vt0.y);
				d0.z=vt0.z+DIFFSIZE_E*(vt1.z-vt0.z);
				d1.x=vt1.x+DIFFSIZE_E*(vt1.x-vt0.x);
				d1.y=vt1.y+DIFFSIZE_E*(vt1.y-vt0.y);
				d1.z=vt1.z+DIFFSIZE_E*(vt1.z-vt0.z);
				d2.x=vt0.x+DIFFSIZE_E*(vt0.x-vt1.x);
				d2.y=vt0.y+DIFFSIZE_E*(vt0.y-vt1.y);
				d2.z=vt0.z+DIFFSIZE_E*(vt0.z-vt1.z);
				d3.x=vt1.x+DIFFSIZE_E*(vt0.x-vt1.x);
				d3.y=vt1.y+DIFFSIZE_E*(vt0.y-vt1.y);
				d3.z=vt1.z+DIFFSIZE_E*(vt0.z-vt1.z);
				newedges.push_back(vt0);
				newedges.push_back(vt1);
				newedges.push_back(vt0);
				newedges.push_back(vt1);
				newedges2.push_back(d0);
				newedges2.push_back(d1);
				newedges2.push_back(d2);
				newedges2.push_back(d3);
			}
		}
	}
	U32 nnewedges=newedges.size();
// replace model
	modelb* mod2;
	static S32 uniq;
	C8 str[50];
	sprintf(str,"toonedge_%d",uniq++);
	mod2=model_create(str);
	if (model_getrc(mod2)==1) {
		mod2->copyverts(newedges);
		mod2->copyverts2(newedges2);
//		mod2->copyuvs0(iuvs);
//		mod2->copyuvs1(iuvs1);
//		mod2->copynorms(inorms);
		pushandsetdir("toon");
//		textureb* texmat0=texture_create("maptestnck.tga");
		textureb* texmat0=texture_create("maptest.tga");
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		popdir();
		for (i=0;i<nnewedges/4;++i) {
			mod2->addface(4*i  ,4*i+1,4*i+2);
			mod2->addface(4*i+3,4*i+2,4*i+1);
		}
		mod2->addmat("edge",SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,
			texmat0,&F32BLACK,30,nnewedges/2,nnewedges);
//		mod->addmat("stan",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,30,1,3);
		mod2->close();
	}
	t->setmodel(mod2);
	t->isedge=true;
//	t->treecolor.w=.95f; // turn on some blending
}

void normalify(tree2* tb,U32 method)
{
// show old model
	list<tree2*>::iterator it;
	for (it=tb->children.begin();it!=tb->children.end();++it) {
		normalify(*it,method);
	}
	tree2* t=new tree2("toonedge");
	t->trans=pointf3x();
	t->rot=pointf3x();
	t->scale=pointf3x(1,1,1);
	t->buildo2p=O2P_NONE;
	tb->linkchild(t);
	modelb* tm=tb->mod;
	if (!tm)
		return;
	const pointf3* v,*n;
	U32 nv,nn;
	switch(method) {
		case 0:
		default:
			n=tm->getnorms(&nn);
		break;
		case 1:
			n=tm->gettangents(&nn);
		break;
		case 2:
			n=tm->getbinorms(&nn);
		break;
	}
	if (!nn)
		return;
	v=tm->getverts(&nv);

	vector<pointf3> newedges;
	vector<pointf3> newedges2;
	pointf3 d0,d1,d2,d3;
	U32 i;
	for (i=0;i<nn;++i) {
		pointf3 vt0,vt1;
		vt0=v[i];
		vt1.x=vt0.x+n[i].x*videoinfodx9.normallength;
		vt1.y=vt0.y+n[i].y*videoinfodx9.normallength;
		vt1.z=vt0.z+n[i].z*videoinfodx9.normallength;
#define DIFFSIZE_N 1
		d0.x=vt0.x+DIFFSIZE_N*(vt1.x-vt0.x);
		d0.y=vt0.y+DIFFSIZE_N*(vt1.y-vt0.y);
		d0.z=vt0.z+DIFFSIZE_N*(vt1.z-vt0.z);
		d1.x=vt1.x+DIFFSIZE_N*(vt1.x-vt0.x);
		d1.y=vt1.y+DIFFSIZE_N*(vt1.y-vt0.y);
		d1.z=vt1.z+DIFFSIZE_N*(vt1.z-vt0.z);
		d2.x=vt0.x+DIFFSIZE_N*(vt0.x-vt1.x);
		d2.y=vt0.y+DIFFSIZE_N*(vt0.y-vt1.y);
		d2.z=vt0.z+DIFFSIZE_N*(vt0.z-vt1.z);
		d3.x=vt1.x+DIFFSIZE_N*(vt0.x-vt1.x);
		d3.y=vt1.y+DIFFSIZE_N*(vt0.y-vt1.y);
		d3.z=vt1.z+DIFFSIZE_N*(vt0.z-vt1.z);
		newedges.push_back(vt0);
		newedges.push_back(vt1);
		newedges.push_back(vt0);
		newedges.push_back(vt1);
		newedges2.push_back(d0);
		newedges2.push_back(d1);
		newedges2.push_back(d2);
		newedges2.push_back(d3);
	}
	U32 nnewedges=newedges.size();
// replace model
	modelb* mod2;
	static S32 uniq;
	C8 str[50];
	sprintf(str,"toonedge_%d",uniq++);
	mod2=model_create(str);
	if (model_getrc(mod2)==1) {
		mod2->copyverts(newedges);
		mod2->copyverts2(newedges2);
		textureb* texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		for (i=0;i<nnewedges/4;++i) {
			mod2->addface(4*i  ,4*i+1,4*i+2);
			mod2->addface(4*i+3,4*i+2,4*i+1);
		}
		mod2->addmat("edge",SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,
			texmat0,&F32LIGHTGREEN,30,nnewedges/2,nnewedges);
//		mod->addmat("stan",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,30,1,3);
		mod2->close();
	}
	t->setmodel(mod2);
//	t->treecolor.w=.95f; // turn on some blending
}

#if 0
LPD3DXLINE linehand;
D3DXVECTOR2 vl2[3];
D3DXVECTOR3 vl3[3];
U32 vlc=3;

static void lines_init()
{
//	if (!videoinfodx9.d3d9device)
		return;
	HRESULT hr=D3DXCreateLine(videoinfodx9.d3d9device,&linehand);
	if (hr!=D3D_OK)
		errorexit("can't create line");
#if 1
	vl2[0].x=.100f;
	vl2[0].y=.100f;
	vl2[1].x=.300f;
	vl2[1].y=.300f;
	vl2[2].x=.500f;
	vl2[2].y=.100f;
#else
	vl2[0].x=100;
	vl2[0].y=100;
	vl2[1].x=300;
	vl2[1].y=300;
	vl2[2].x=500;
	vl2[2].y=100;
#endif
}

static void lines_draw()
{
//	if (!videoinfodx9.d3d9device)
		return;
	D3DXMATRIX matr;
	HRESULT hr;
	hr=linehand->SetWidth(1);
	if (hr!=0)
		errorexit("can't set line width hr %08x",hr);
	hr=linehand->Begin();
	if (hr!=0)
		errorexit("can't set line begin");
	hr=linehand->Draw(vl2,vlc,0);
	if (hr!=0)
		errorexit("can't set line draw");
/*
	hr=linehand->DrawTransform(vl3,vlc,&matr,0);
	if (hr!=0)
		errorexit("can't set line drawtransform");
*/
	hr=linehand->End();
	if (hr!=0)
		errorexit("can't set line end");

}

static void lines_exit()
{
//	if (!videoinfodx9.d3d9device)
		return;
	HRESULT hr=linehand->Release();
	if (hr!=0)
		errorexit("can't free line");
}
#endif
#if 0
////////////////////////// main
void tooninit()
{
//	carnames[2]="hi";
//	carnames[3][0]='h';
	pushandsetdir("toon");
	defsave=lightinfo.dodefaultlights;
	lightinfo.dodefaultlights=false;
	video_setupwindow(GX,GY);
	video_reload_mat2shader("toon_mat2shader.txt");
	togvidmode=videoinfo.video_maindriver;

	roottree=new tree2("roottree");
	tree2* toonback;
// select object
//	toonback=singletri();
//	toonback=new tree2("box01.bwo"); // cube with hole
//	toonback=new tree2("box03.bwo"); // cube
//	toonback=new tree2("tarpit_body.bwo"); // car body
#if 0
	toonback=buildsphere("asphere","pdiff");
#else // load car
//	toonback=new tree2("car1.bws"); // yes
//	pushandsetdir("racenetdata/2009_cars/fastgassin");
//	toonback=new tree2("fastgassin.bws"); // yes
//	popdir();

	pushandsetdir(cardirs[curcar]);
	toonback=new tree2(carnames[curcar]);
	roottree->linkchild(toonback);
//	toonback->scale=pointf3x(100,100,100);
// rotate wheels
	list<tree2*>::iterator it;
	S32 i;
	for (it=toonback->children.begin(),i=0;it!=toonback->children.end();++it,++i) {
		switch(i) {
		case 2:
		case 4:
		case 5:
		case 6:
			(*it)->buildo2p=O2P_FROMTRANSROTSCALE;
//			(*it)->rotvel.x=-.05f;
			break;
		}
	}
#endif

// build some lights
// amblit
	tree2* amblit=new tree2("amblit");
	amblit->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	amblit->lightcolor=pointf3x(.5,.5,.5,0);
	addlighttolist(amblit);
	roottree->linkchild(amblit);
// dirlit
	tree2* dirlit;
	dirlit=new tree2("dirlit");
	dirlit->name="dirlit";
	dirlit->flags|=TF_ISLIGHT;
	dirlit->lightcolor=pointf3x(.75,.75,.75,0);
	dirlit->rotvel.x=0;
//	dirlit->rotvel.y=.04f;
	dirlit->rotvel.z=0;
	addlighttolist(dirlit);
	roottree->linkchild(dirlit);

// setup viewport
	mainvp.backcolor=C32(79,80,180);
	mainvp.zfront=.0125f;
	mainvp.zback=500;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1; // 3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans=pointf3x(-.75f,1,-1.125);
	mainvp.camrot=pointf3x(.5f,.5f,0);
//	mainvp.camtrans=pointf3x(0,0,-10);
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
	mainvp.lookat=0;

// toonify
//	tree2* toontree;
//	toontree=toonback->newdup();
//// toonify(toontree);
//	edgeify(toontree);
	normalify(toonback);
//	toontree->trans.x+=40;
//	toontree->trans.y+=3;
//	toontree->trans.z+=3;
//	roottree->linkchild(toontree);
//	edgeify(toonback);
//	lines_init();
	popdir();
}

void toonproc()
{
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TOON);
	}
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY== K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TOON);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TOON);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TOON);
	}
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	if (KEY==']') {
		changestate(STATE_TOON);
		++curcar;
		if (curcar==NCARS)
			curcar=0;
	}
	if (KEY=='[') {
		changestate(STATE_TOON);
		--curcar;
		if (curcar<0)
			curcar=NCARS-1;
	}
	mainvp.xres=WX; // incase res has changed..
	mainvp.yres=WY;
	roottree->proc();
	doflycam(&mainvp);
	drawtextque_format_foreback(200,10,F32BLACK,F32WHITE,"%s",carnames[curcar]);
}

void toondraw3d()
{
//	lines_draw();
	dolights();
	video_buildworldmats(roottree);
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
}

void toonexit()
{
	lightinfo.dodefaultlights=defsave;
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
//	checkheaps();
	delete roottree;
	popdir();
//	lines_exit();
}
#endif
