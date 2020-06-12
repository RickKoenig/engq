// handy routines for building various 3d objects
#define D2_3D
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

#include "helperobj.h"

S32 usehelper = 0;

#ifdef USEHELPER
// model building functions
static const float thinscale=.015f;
// -1 to 1 for xyz, for box
static modelb* buildaprism()
{
	modelb* m=model_create("helper_prism_mod");
	if (model_getrc(m)==1) {
        pushandsetdir("gfxtest");
		m->copyverts(prismverts,nprismverts); // -1,-1,-1 to 1,1,1
		m->copyuvs0(prismuvs,nprismverts);
 // create a texture
		textureb* texmat0;
		texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1)
			texmat0->load();
		m->addmat("tex",SMAT_HASWBUFF|SMAT_HASTEX,texmat0,0,50,nprismfaces,nprismverts);
		m->addfaces(prismfaces,nprismfaces,0);
		m->close();
	    popdir();
	}
	return m;
}

// -1 to 1 for xz, 0 to 1 for y, for line
static modelb* buildaprismy()
{
	modelb* m=model_create("helper_prism_mody");
	if (model_getrc(m)==1) {
        pushandsetdir("gfxtest");
		vector<pointf3> vs(prismverts,prismverts+24);
		vector<uv> uvs(prismuvs,prismuvs+24);
		S32 i;
		const float thinscaler=1.0f/thinscale;
		for (i=0;i<24;++i) {
			vs[i].y=.5f*vs[i].y+.5f;
			uvs[i].u*=1;
			uvs[i].v*=.5f*thinscaler;
		}
		m->copyverts(vs); // -1,0,-1 to 1,1,1
		m->copyuvs0(uvs);
 // create a texture
		textureb* texmat0;
		texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1)
			texmat0->load();
		m->addmat("tex",SMAT_HASWBUFF|SMAT_HASTEX,texmat0,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();
	    popdir();
	}
	return m;
}

// y pyramid base almost +-2 , 0, +-2 top is almost 0,1,0, for vector
static modelb* buildavectory()
{
	modelb* m=model_create("helper_vector");
	if (model_getrc(m)==1) {
        pushandsetdir("gfxtest");
		vector<pointf3> v(prismverts,prismverts+24);
		vector<uv> uvs(prismuvs,prismuvs+24);
		S32 i;
		const float thinscaler=1.0f/thinscale;
		for (i=0;i<24;++i) {
			v[i].y=.5f*prismverts[i].y+.5f;
			v[i].x -= v[i].x*v[i].y*.8f;
			v[i].z -= v[i].z*v[i].y*.8f;
			uvs[i].u*=1;
			uvs[i].v*=.5f*thinscaler;
		} 
		m->copyverts(v); // taper to (0,1,0) at y = 1
		m->copyuvs0(uvs);
 // create a texture
		textureb* texmat0;
		texmat0=texture_create("maptestnck.tga");
		if (texture_getrc(texmat0)==1)
			texmat0->load();
		m->addmat("tex",SMAT_HASWBUFF|SMAT_HASTEX,texmat0,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();
	    popdir();
	}
	return m;
}

// helperobj class functions
helperobj::helperobj()
{
	preload=new tree2("helper_preload");
	addbox(preload,pointf3x(),pointf3x(1,1,1),F32WHITE);
	addsphere(preload,pointf3x(),1,F32WHITE);
	addline(preload,pointf3x(),pointf3x(1,1,1),F32WHITE);
	addvector(preload,pointf3x(),pointf3x(1,1,1),F32WHITE);
	helplist.clear(); // 'keep' 'preload'
}

void helperobj::addbox(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c)
{
	if (!usehelper)
		return;
	pointf3 pos,scl;
	pos.x=(p0.x+p1.x)*.5f;
	pos.y=(p0.y+p1.y)*.5f;
	pos.z=(p0.z+p1.z)*.5f;
	scl.x=(p1.x-p0.x)*.5f;
	scl.y=(p1.y-p0.y)*.5f;
	scl.z=(p1.z-p0.z)*.5f;
	modelb* modpris=buildaprism();
	tree2* lin=new tree2("helper_box");
	lin->buildo2p=O2P_FROMTRANSQUATSCALE;
	lin->setmodel(modpris);
	lin->trans=pos;
	lin->scale=scl;
	lin->treecolor=c;
	rt->linkchild(lin);
	if (!linger)
		helplist.push_back(lin);
}

void helperobj::addline(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c)
{
	if (!usehelper)
		return;
	pointf3 nrm;
	nrm.x=p1.x-p0.x;
	nrm.y=p1.y-p0.y;
	nrm.z=p1.z-p0.z;
	float f=normalize3d(&nrm,&nrm);
	if (f==0)
		return;
	modelb* modpris=buildaprismy();
	tree2* lin=new tree2("helper_line");
	lin->buildo2p=O2P_FROMTRANSQUATSCALE;
	lin->setmodel(modpris);
	lin->trans=p0;
	pointf3x up(0,1,0);
	pointf3 q = pointf3x(0,0,0,1);
	normal2quat(&up,&nrm,&q);
	lin->rot=q;

	lin->scale=pointf3x(thinscale*f,f,thinscale*f);
	lin->treecolor=c;
	rt->linkchild(lin);
	if (!linger)
		helplist.push_back(lin);
}

void helperobj::addvector(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c)
{
	if (!usehelper)
		return;
	pointf3 nrm;
	nrm.x=p1.x-p0.x;
	nrm.y=p1.y-p0.y;
	nrm.z=p1.z-p0.z;
	float f=normalize3d(&nrm,&nrm);
	if (f==0)
		return;
	modelb* modpris=buildavectory();
	tree2* lin=new tree2("helper_vector");
	lin->buildo2p=O2P_FROMTRANSQUATSCALE;
	lin->setmodel(modpris);
	lin->trans=p0;
	pointf3x up(0,1,0);
	pointf3 q = pointf3x(0,0,0,1);
	normal2quat(&up,&nrm,&q);
	lin->rot=q;

	lin->scale=pointf3x(thinscale*f,f,thinscale*f);
	lin->treecolor=c;
	rt->linkchild(lin);
	if (!linger)
		helplist.push_back(lin);
}

void helperobj::addsphere(tree2* rt,const pointf3& p,float s,const pointf3& c)
{
	if (!usehelper)
		return;
	modelb* modsph=model_create("helper_sphere_mod");
	if (model_getrc(modsph)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(modsph,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(modsph,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(modsph,32,32,8,8,spheref_surf(1),"maptestnck.tga","tex");
	    popdir();
	}
	tree2* sph=new tree2("helper_sphere");
	sph->setmodel(modsph);
	sph->trans=p;
	sph->scale=pointf3x(s,s,s);
	sph->treecolor=c;
	rt->linkchild(sph);
	if (!linger)
		helplist.push_back(sph);
}

void helperobj::addjack(tree2* rt,const pointf3& p,float s,const pointf3& c)
{
	if (!usehelper)
		return;
	pointf3 p0,p1;

	p0.x=p.x-s;	p0.y=p.y;			p0.z=p.z;
	p1.x=p.x+s;	p1.y=p.y;			p1.z=p.z;
	addvector(rt,p0,p1,F32LIGHTRED);

	p0.x=p.x;	p0.y=p.y-s;	p0.z=p.z;
	p1.x=p.x;	p1.y=p.y+s;			p1.z=p.z;
	addvector(rt,p0,p1,F32LIGHTGREEN);

	p0.x=p.x;	p0.y=p.y;			p0.z=p.z-s;
	p1.x=p.x;	p1.y=p.y;			p1.z=p.z+s;
	addvector(rt,p0,p1,F32LIGHTBLUE);

	addsphere(rt,p,s*.125f,c);
}

void helperobj::reset()
{
	vector<tree2*>::iterator i;
	for (i=helplist.begin();i!=helplist.end();++i)
		delete *i;
	helplist.clear();
}

helperobj::~helperobj()
{
	delete preload;
}

void helperobj::setlinger(bool la)
{
	linger=la;
}

bool helperobj::linger;

#else // disable

helperobj::helperobj() /*: roottree(root)*/
{
}

void helperobj::addbox(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c)
{
}

void helperobj::addline(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c)
{
}

void helperobj::addvector(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c)
{
}

void helperobj::addsphere(tree2* rt,const pointf3& p,float s,const pointf3& c)
{
}

void helperobj::addjack(tree2* rt,const pointf3& p,float s,const pointf3& c)
{
}

void helperobj::reset()
{
}

helperobj::~helperobj()
{
}

void helperobj::setlinger(bool la)
{
}

#endif

