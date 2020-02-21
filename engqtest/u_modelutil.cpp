// handy routines for building various 3d objects
#define D2_3D
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

//const int nprismverts=24;
//const int nprismfaces=12;
/// surface functions
void sphere1_surf(float p,float q,pointf3* v,pointf3* n)
{
	const float rad=10.0f;
	float sanglo,canglo,sangla,cangla;
	p=2*PI*p;
	q=PI/2-PI*q;
	sanglo=sinf(p);
	canglo=-cosf(p);
	sangla=sinf(q);
	cangla=cosf(q);
	v->x=sanglo*cangla;
	v->z=canglo*cangla;
	v->y=sangla;
	*n=*v;
	v->x*=rad;
	v->y*=rad;
	v->z*=rad;
}

#if 0
void square1xy_surf(float p,float q,pointf3* v,pointf3* n)
{
	const float size=10.0f;
	v->x=p*size;
	v->z=0;
	v->y=-q*size;
	n->x=0;
	n->y=0;
	n->z=-1;
}
#endif
void planexz_surf::operator()(float p,float q,pointf3* v,pointf3* n)
{
	v->x=(p-.5f)*xsize;
	v->z=(.5f-q)*zsize;
	v->y=0;
	n->x=0;
	n->y=1;
	n->z=0;
}

void planexy_surf::operator()(float p,float q,pointf3* v,pointf3* n)
{
	v->x=(p-.5f)*xsize;
	v->y=(.5f-q)*ysize;
	v->z=0;
	n->x=0;
	n->y=0;
	n->z=-1;
}

void planexy_surf0::operator()(float p,float q,pointf3* v,pointf3* n)
{
	v->x = p*xsize;
	v->y = -q*ysize;
	v->z = 0;
	n->x = 0;
	n->y = 0;
	n->z = -1;
}

void spotlightxy_surf::operator()(float p,float q,pointf3* v,pointf3* c)
{
	c->x = 1;
	c->y = 1;
	c->z = 1;
	c->w = 1.0f - q;
	q *= fos;
	p *= 2*PI;
	float rad = q*rat;
	float sp = sinf(p);
	float cp = cosf(p);
	v->x = rad*cp;
	v->y = rad*sp;
	v->z = q;
#if 0
	n->x = cp*norm2.x;
	n->y = sp*norm2.x;
	n->z = -norm2.y;
#endif
}

/// strand utility functions
pointf3 brd2norm; // a global norm used for all 3 braid2 functions
/// returns an axis aligned vector close to a normal of vector
pointf3 findnormaxis(pointf3& p)
{
	static pointf3 xa={1,0,0,0};
	static pointf3 ya={0,1,0,0};
	static pointf3 za={0,0,1,0};
	float a=fabs(p.x);
	float b=fabs(p.y);
	float c=fabs(p.z);
	if (a<b) {
		if (a<c)
			return xa;
		else
			return za;
	} else {
		if (b<c)
			return ya;
		else
			return za;
	}
}
/// uses brd2norm and newdir returns newnorm
pointf3 adjustnorm(pointf3& oldnorm,pointf3& newdir)
{
	float k=-dot3d(&newdir,&oldnorm);
	pointf3 ret;
	ret.x=oldnorm.x+k*newdir.x;
	ret.y=oldnorm.y+k*newdir.y;
	ret.z=oldnorm.z+k*newdir.z;
	normalize3d(&ret,&ret);
	return ret;
}
pointf3 findanorm(pointf3& dir)
{
	pointf3 prb=findnormaxis(dir);
	pointf3 ret=adjustnorm(prb,dir);
	return ret;
}


const pointf3 prismverts[24]={
// front
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1,-1},
	{ 1,-1,-1},
// back
	{ 1, 1, 1},
	{-1, 1, 1},
	{ 1,-1, 1},
	{-1,-1, 1},
// right
	{ 1, 1,-1},
	{ 1, 1, 1},
	{ 1,-1,-1},
	{ 1,-1, 1},
// left
	{-1, 1, 1},
	{-1, 1,-1},
	{-1,-1, 1},
	{-1,-1,-1},
// top
	{-1, 1, 1},
	{ 1, 1, 1},
	{-1, 1,-1},
	{ 1, 1,-1},
// bot
	{-1,-1,-1},
	{ 1,-1,-1},
	{-1,-1, 1},
	{ 1,-1, 1},
};
const pointf3 prismnorms[24]={
// front
	{ 0, 0,-1},
	{ 0, 0,-1},
	{ 0, 0,-1},
	{ 0, 0,-1},
// back
	{ 0, 0, 1},
	{ 0, 0, 1},
	{ 0, 0, 1},
	{ 0, 0, 1},
// right
	{ 1, 0, 0},
	{ 1, 0, 0},
	{ 1, 0, 0},
	{ 1, 0, 0},
// left
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
// top
	{ 0, 1, 0},
	{ 0, 1, 0},
	{ 0, 1, 0},
	{ 0, 1, 0},
// bot
	{ 0,-1, 0},
	{ 0,-1, 0},
	{ 0,-1, 0},
	{ 0,-1, 0},
};
const uv prismuvs[24]={
// front
	{0,0},
	{1,0},
	{0,1},
	{1,1},
// back
#if 0 // normal back face
	{0,0},
	{1,0},
	{0,1},
	{1,1},
#else // mirror back face
	{1,0},
	{0,0},
	{1,1},
	{0,1},
#endif
// right
	{0,0},
	{1,0},
	{0,1},
	{1,1},
// left
	{0,0},
	{1,0},
	{0,1},
	{1,1},
// top
	{0,0},
	{1,0},
	{0,1},
	{1,1},
// bot
	{0,0},
	{1,0},
	{0,1},
	{1,1},
};
const face prismfaces[12]={
// front
	{ 0, 1, 2},
	{ 3, 2, 1},
// back
	{ 4, 5, 6},
	{ 7, 6, 5},
// right
	{ 8, 9,10},
	{11,10, 9},
// left
	{12,13,14},
	{15,14,13},
// top
	{16,17,18},
	{19,18,17},
// bot
	{20,21,22},
	{23,22,21},
};

/// buildpyramid

const pointf3 pyramidverts[16]={
// front
	{ 0, 1, 0},
	{ 1, 0,-1},
	{-1, 0,-1},
// back
	{ 0, 1, 0},
	{-1, 0, 1},
	{ 1, 0, 1},
// right
	{ 0, 1, 0},
	{ 1, 0, 1},
	{ 1, 0,-1},
// left
	{ 0, 1, 0},
	{-1, 0,-1},
	{-1, 0, 1},
// bot
	{-1, 0, -1},
	{ 1, 0, -1},
	{-1, 0, 1},
	{ 1, 0, 1},
};

const face pyramidfaces[6]={ // clockwise order
// front
	{ 0, 1, 2},
// back
	{ 3, 4, 5},
// right
	{ 6, 7, 8},
// left
	{9, 10, 11},
// bot
	{12, 13, 14},
	{15, 14, 13},
};

const pointf3 pyramidnorms[16]={
// front
	{ 0, sqrtf(.5),-sqrtf(.5)},
	{ 0, sqrtf(.5),-sqrtf(.5)},
	{ 0, sqrtf(.5),-sqrtf(.5)},
// back
	{ 0, sqrtf(.5),sqrtf(.5)},
	{ 0, sqrtf(.5),sqrtf(.5)},
	{ 0, sqrtf(.5),sqrtf(.5)},
// right
	{ sqrtf(.5),sqrtf(.5),0},
	{ sqrtf(.5),sqrtf(.5),0},
	{ sqrtf(.5),sqrtf(.5),0},
// left
	{ -sqrtf(.5),sqrtf(.5),0},
	{ -sqrtf(.5),sqrtf(.5),0},
	{ -sqrtf(.5),sqrtf(.5),0},
// bot
	{ 0, -1, 0},
	{ 0, -1, 0},
	{ 0, -1, 0},
	{ 0, -1, 0},
};

const float TI=3;  // tile
const uv pyramiduvs[16]={
// front
	{.5f*TI,0},
	{TI,TI},
	{0,TI},
// back
	{.5f*TI,0},
	{TI,TI},
	{0,TI},
// right
	{.5f*TI,0},
	{TI,TI},
	{0,TI},
// left
	{.5f*TI,0},
	{TI,TI},
	{0,TI},
// bot
	{0,0},
	{TI,0},
	{0,TI},
	{TI,TI},
};

// TODO: move to cubemap texture for hardware, multi material for software
#define OT (1.0f/3.0f)
#define TT (2.0f/3.0f)
#define ER1 (1.0f/4000.0f)
#define ER2 (1.0f/400.0f) // eliminate white lines
const static uv skyboxuvs1[24]={
// front
	{  1-ER1,OT+ER1},
	{.75f+ER1,OT+ER1},
	{  1-ER1,TT-ER1},
	{.75f+ER1,TT-ER1},
// back
	{ .5f-ER1,OT+ER1},
	{.25f+ER1,OT+ER1},
	{ .5f-ER1,TT-ER1},
	{.25f+ER1,TT-ER1},
// right
	{.75f-ER1,OT+ER1},
	{ .5f+ER1,OT+ER1},
	{.75f-ER1,TT-ER1},
	{ .5f+ER1,TT-ER1},
// left
	{.25f-ER1,OT+ER1},
	{  0+ER1,OT+ER1},
	{.25f-ER1,TT-ER1},
	{  0+ER1,TT-ER1},
// top
	{.25f+ER1,OT-ER1},
	{ .5f-ER1,OT-ER1},
	{.25f+ER1,0 +ER1},
	{ .5f-ER1,0 +ER1},
// bot
	{.25f+ER1,1-ER1},
	{ .5f-ER1,1-ER1},
	{.25f+ER1,TT+ER1},
	{ .5f-ER1,TT+ER1},
};
const static uv skyboxuvs2[24]={
// front
	{  1-ER2,OT+ER2},
	{.75f+ER2,OT+ER2},
	{  1-ER2,TT-ER2},
	{.75f+ER2,TT-ER2},
// back
	{ .5f-ER2,OT+ER2},
	{.25f+ER2,OT+ER2},
	{ .5f-ER2,TT-ER2},
	{.25f+ER2,TT-ER2},
// right
	{.75f-ER2,OT+ER2},
	{ .5f+ER2,OT+ER2},
	{.75f-ER2,TT-ER2},
	{ .5f+ER2,TT-ER2},
// left
	{.25f-ER2,OT+ER2},
	{  0+ER2,OT+ER2},
	{.25f-ER2,TT-ER2},
	{  0+ER2,TT-ER2},
// top
	{.25f+ER2,OT-ER2},
	{ .5f-ER2,OT-ER2},
	{.25f+ER2,0 +ER2},
	{ .5f-ER2,0 +ER2},
// bot
	{.25f+ER2,1-ER2},
	{ .5f-ER2,1-ER2},
	{.25f+ER2,TT+ER2},
	{ .5f-ER2,TT+ER2},
};
const static face skyboxfaces[12]={
// front
	{ 2, 1, 0},
	{ 1, 2, 3},
// back
	{ 6, 5, 4},
	{ 5, 6, 7},
// right
	{10, 9, 8},
	{ 9,10,11},
// left
	{14,13,12},
	{13,14,15},
// top
	{18,17,16},
	{17,18,19},
// bot
	{22,21,20},
	{21,22,23},
};

tree2* buildpyramid(const C8* texname,const C8* matname,const C8* modname)
{   // now is a pyramid
	tree2* t=new tree2("pyramid");
	modelb* m;
	if (modname)
		m=model_create(modname);
	else
		m=model_create(unique());
	if (model_getrc(m)==1) {
		m->copyverts(pyramidverts,16); // from pyramid table to model
		m->copynorms(pyramidnorms,16);
		m->copyuvs0(pyramiduvs,16);
 // create a texture // we commented out
		//texname = 0; // no tex name, no texture, comment out if want a texture
		if (texname) {
			textureb* texmat0;
			texmat0=texture_create(texname);
			if (texture_getrc(texmat0)==1) {
				texmat0->load();
			}
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,6,16);
		} else
			m->addmat(matname,SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,50,6,16); // add
		m->addfaces(pyramidfaces,6,0);
		m->close(); // copy everything to the gpu in hardware mode, if CALCLIGHTS, calc cverts for software mode
	}
	t->setmodel(m);
	return t;
}

tree2* buildprism(const pointf3& size,const C8* texname,const C8* matname,const C8* modname)
{
	tree2* t=new tree2("prism");
	modelb* m;
	if (modname)
		m=model_create(modname);
	else
		m=model_create(unique());
	if (model_getrc(m)==1) {
		pointf3* tv=new pointf3[24];
		S32 i;
		for (i=0;i<24;++i) {
			tv[i].x=prismverts[i].x*size.x*.5f;
			tv[i].y=prismverts[i].y*size.y*.5f;
			tv[i].z=prismverts[i].z*size.z*.5f;
		}
		m->copyverts(tv,24);
		delete[] tv;
		m->copynorms(prismnorms,24);
 // create a texture
		if (texname) {
			textureb* texmat0;
			m->copyuvs0(prismuvs,24);
			texmat0=texture_create(texname);
			if (texture_getrc(texmat0)==1) {
				texmat0->load();
			}
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,12,24);
		} else
			m->addmat(matname,SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();
	}
	t->setmodel(m);
	return t;
}

tree2* buildprismuvs(const pointf3& size,const C8* texname,const C8* matname,const uv& uv0,const uv& uv1)
{
	tree2* t=new tree2("prism");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
 // create a texture
		textureb* texmat0;
		texmat0=texture_create(texname);
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		pointf3* tv=new pointf3[24];
		S32 i;
		for (i=0;i<24;++i) {
			tv[i].x=prismverts[i].x*size.x*.5f;
			tv[i].y=prismverts[i].y*size.y*.5f;
			tv[i].z=prismverts[i].z*size.z*.5f;
		}
		m->copyverts(tv,24);
		delete[] tv;
		m->copynorms(prismnorms,24);
		uv prismuvsm[24];
		memset(prismuvsm,0,sizeof(prismuvsm));
		for (i=0;i<24;++i) {
			prismuvsm[i].u=uv0.u+(uv1.u-uv0.u)*prismuvs[i].u;
			prismuvsm[i].v=uv0.v+(uv1.v-uv0.v)*prismuvs[i].v;
		}
		m->copyuvs0(prismuvsm,24);
		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();
	}
	t->setmodel(m);
	return t;
}

tree2* buildprism2t(const pointf3& size,const C8* texname,const C8* texname2,const C8* matname)
{
	tree2* t=new tree2("prism2t");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
 // create a texture
		textureb* texmat0;
		texmat0=texture_create(texname);
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		textureb* texmat1;
		texmat1=texture_create(texname2);
		if (texture_getrc(texmat1)==1) {
			texmat1->load();
		}
		pointf3* tv=new pointf3[24];
		S32 i;
		for (i=0;i<24;++i) {
			tv[i].x=prismverts[i].x*size.x*.5f;
			tv[i].y=prismverts[i].y*size.y*.5f;
			tv[i].z=prismverts[i].z*size.z*.5f;
		}
		m->copyverts(tv,24);
		delete[] tv;
		m->copynorms(prismnorms,24);
		m->copyuvs0(prismuvs,24);
		m->addmat2t(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,texmat1,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();
	}
	t->setmodel(m);
	return t;
}

tree2* buildprism6t(const pointf3& size,const C8* texname[],const C8* matname,const C8* modname)
{
#define DOUBLESIDED
	tree2* t=new tree2("prism");
	modelb* m;
	if (modname)
		m=model_create(modname);
	else
		m=model_create(unique());
	if (model_getrc(m)==1) {
		pointf3* tv=new pointf3[24];
		S32 i;
		for (i=0;i<24;++i) {
			tv[i].x=prismverts[i].x*size.x*.5f;
			tv[i].y=prismverts[i].y*size.y*.5f;
			tv[i].z=prismverts[i].z*size.z*.5f;
		}
		m->copyverts(tv,24);
		delete[] tv;
		m->copynorms(prismnorms,24);
		// create 6 textures
		for (U32 i=0;i<6;++i) {
			if (texname && texname[i]) {
				textureb* texmat0=texture_create(texname[i]);
				if (texture_getrc(texmat0)==1) {
					texmat0->load();
				}
#ifdef DOUBLESIDED
				m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,4,4);
#else
				m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,2,4);
#endif
			} else
#ifdef DOUBLESIDED
				m->addmat(matname,SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,50,4,4);
#else
				m->addmat(matname,SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE,0,0,50,2,4);
#endif
		}
#ifdef DOUBLESIDED
		m->addfaces(prismfaces,12,0,false,true);
#else
		m->addfaces(prismfaces,12,0);
#endif
		m->copyuvs0(prismuvs,24);
		m->close();
	}
	t->setmodel(m);
	return t;
}

tree2* buildskybox(const pointf3& size,const C8* texname,const C8* matname)
{
	tree2* t=new tree2("skybox");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
 // create a texture
		textureb* texmat0;
		texmat0=texture_create(texname);
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
		pointf3* tv=new pointf3[24];
		S32 i;
		for (i=0;i<24;++i) {
			tv[i].x=prismverts[i].x*size.x*.5f;
			tv[i].y=prismverts[i].y*size.y*.5f;
			tv[i].z=prismverts[i].z*size.z*.5f;
		}
		m->copyverts(tv,24);
		delete[] tv;
		if (texmat0->tsize.x>1024) // hack
			m->copyuvs0(skyboxuvs1,24);
		else
			m->copyuvs0(skyboxuvs2,24);
		m->addmat(matname,SMAT_HASTEX|SMAT_CLAMP|SMAT_ISSKY|SMAT_HASNOFOG,texmat0,0,50,12,24);
		m->addfaces(skyboxfaces,12,0);
		m->close();
	}
	t->setmodel(m);
	return t;
}

tree2* buildcylinder_xz(const pointf3& size,const C8* texname,const C8* matname)
{
	static S32 id;
	C8 modname[100];
/// build the cylinder root
	tree2* cyltree=new tree2("cyltree");
/// build cylinder body
	sprintf(modname,"cylmid%d",id++);
	modelb* modcm=model_create(modname);
	if (model_getrc(modcm)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcm,30,1,1,1,cylinderxz_mid_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreem=new tree2("cyltreem");
//    cyltreem->treecolor = bodycolor;
	cyltreem->setmodel(modcm);
 	cyltree->linkchild(cyltreem);
/// build cylinder bottom
	sprintf(modname,"cylbot%d",id++);
	modelb* modcc0=model_create(modname);
	if (model_getrc(modcc0)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcc0,30, 1,1, 1,cylinderxz_bot_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreec0=new tree2("cyltreec0");
//    cyltreec0->treecolor = botcolor;
	cyltreec0->setmodel(modcc0);
	cyltree->linkchild(cyltreec0);
/// build cylinder top
	sprintf(modname,"cyltop%d",id++);
	modelb* modcc1=model_create(modname);
	if (model_getrc(modcc1)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcc1,30, 1,1, 1,cylinderxz_top_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreec1=new tree2("cyltreec1");
//    cyltreec1->treecolor = topcolor;
	cyltreec1->setmodel(modcc1);
	cyltree->linkchild(cyltreec1);
	return cyltree;
}

tree2* buildcylinderc_xz(const pointf3& size,const C8* texname,const C8* matname, pointf3 bodycolor, pointf3 topcolor, pointf3 botcolor)
{
	static S32 id;
	C8 modname[100];
/// build the cylinder root
	tree2* cyltree=new tree2("cyltree");
/// build cylinder body
	sprintf(modname,"cylmid%d",id++);
	modelb* modcm=model_create(modname);
	if (model_getrc(modcm)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcm,30,1,1,1,cylinderxz_mid_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreem=new tree2("cyltreem");
    cyltreem->treecolor = bodycolor;
	cyltreem->setmodel(modcm);
 	cyltree->linkchild(cyltreem);
/// build cylinder bottom
	sprintf(modname,"cylbot%d",id++);
	modelb* modcc0=model_create(modname);
	if (model_getrc(modcc0)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcc0,30, 1,1, 1,cylinderxz_bot_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreec0=new tree2("cyltreec0");
    cyltreec0->treecolor = botcolor;
	cyltreec0->setmodel(modcc0);
	cyltree->linkchild(cyltreec0);
/// build cylinder top
	sprintf(modname,"cyltop%d",id++);
	modelb* modcc1=model_create(modname);
	if (model_getrc(modcc1)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcc1,30, 1,1, 1,cylinderxz_top_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreec1=new tree2("cyltreec1");
    cyltreec1->treecolor = topcolor;
	cyltreec1->setmodel(modcc1);
	cyltree->linkchild(cyltreec1);
	return cyltree;
}

tree2* buildplane_xy(const pointf2& sizexy,const C8* texname,const C8* matname,const C8* modname)
{
	tree2* r=new tree2(unique());
	modelb* modb;
	if (modname)
		modb=model_create(modname);
	else
		modb=model_create(unique());
	if (model_getrc(modb)==1)
	    buildpatch(modb,1,1,1,1,planexy_surf(sizexy.x,sizexy.y),texname,matname);
	r->setmodel(modb);
	return r;
}

tree2* buildplane_xy_uv(const pointf2& sizexy,const pointf2& enduv,const C8* texname,const C8* matname,const C8* modelname)
{
	tree2* r;
	if (modelname)
		r = new tree2(modelname);
	else
		r = new tree2(unique());
	modelb* modb;
	if (modelname)
		modb = model_create(modelname);
	else
		modb = model_create(unique());
	if (model_getrc(modb)==1)
	    buildpatch(modb,1,1,enduv.x,enduv.y,planexy_surf(sizexy.x,sizexy.y),texname,matname);
	r->setmodel(modb);
	return r;
}

tree2* buildsphere(float rad,const C8* texname,const C8* matname)
{
	static S32 id;
	C8 smodname[100];
	sprintf(smodname,"buildsphere%d",id++); // inc ?
	modelb* mod2=model_create(smodname);
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,20,20,3,3,spheref_surf(rad),texname,matname);
//void buildpatch(modelb* mod,int np,int nq,float tileu,float tilev,T paramfunc,const C8* texname,const C8* matname)
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree3");
	spheretree2->setmodel(mod2);
	return spheretree2;
}

tree2* buildtorus_xz(float radi0,float radi1,const C8* texname,const C8* matname,S32 pnum,S32 qnum)
{
	static S32 id;
	C8 smodname[100];
	sprintf(smodname,"buildtorus_xz%d",id++); // inc ?
	modelb* mod2=model_create(smodname);
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,pnum,qnum,3,3,torusxzf_surf(radi0,radi1),texname,matname);
	    popdir();
	}
	tree2* torustree2=new tree2("torustree3");
	torustree2->setmodel(mod2);
	return torustree2;
}

tree2* buildspotlight_xy(float theta,float falloffstart,float falloffend,const C8* matname)
{
	static S32 id;
	C8 smodname[100];
	sprintf(smodname,"buildspotlight_xz%d",id++); // post inc id for the next model of this type
	modelb* mod2=model_create(smodname);
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
		//const C8* texname = "maptestnck.png";
		const C8* texname = "white.pcx";
	    buildpatchc(mod2,35,35,3,3,spotlightxy_surf(theta,falloffstart,falloffend),texname,matname);
	    popdir();
		mod2->mats[0].color = pointf3x(1,1,1,.95f);
	}
	tree2* spotlighttree2=new tree2("spotlighttree3");
	spotlighttree2->setmodel(mod2);
	return spotlighttree2;
}

tree2* buildcone_xy(float theta,float falloffstart,float falloffend,const C8* matname, pointf3 conebodycolor)
{
	static S32 id;
	C8 conemodname[100];
/// build the cone root
	tree2* conetree=new tree2("conetree");
/// build the cone body
	sprintf(conemodname,"conebody_%d",id++); // post inc id for the next model of this type
	modelb* modconebody=model_create(conemodname);
	if (model_getrc(modconebody)==1) {
        pushandsetdir("gfxtest");
		const C8* texname = "white.pcx";
	    buildpatchc(modconebody,30,1,1,1,spotlightxy_surf(theta,falloffstart,falloffend),texname,matname); // mod2,35,35,3,3 same result as these spotlight defaults
	    popdir();
		modconebody->mats[0].color = pointf3x(1,1,1,1.f); // default alpha was .95f , 1.0f gives a solid color
	}
	tree2* conebody=new tree2("conebody");
    conebody->treecolor = conebodycolor;
	conebody->setmodel(modconebody);
	conetree->linkchild(conebody);
/// build cone bottom
/*	sprintf(conemodname,"conebot%d",id++);
	modelb* modconebot=model_create(conemodname);
	if (model_getrc(modconebot)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modconebot,30, 1,1, 1,cylinderxz_bot_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* conebot=new tree2("conebot");
//    conebot->treecolor = conebotcolor;
	conebot->setmodel(conebot);
	conetree->linkchild(conebot);
*/

	return conetree;
}

// rgb to hsv and back

S32 supersat(S32 sat,S32 r,S32 g,S32 b,S32* r2,S32* g2,S32* b2)
{
	*r2=r;
	*g2=g;
	*b2=b;
	if (sat>255) { // try to colorize the gray
		S32 extra=sat-255;
		sat=255;
		*r2+=2*extra;
		if (*r2>255)
			*r2=255;
		*g2-=2*extra;
		if (*g2<0)
			*g2=0;
		*b2-=2*extra;
		if (*b2<0)
			*b2=0;
	}
	return sat;
}

void rgb2hsv(S32 r,S32 g,S32 b,S32* h,S32* s,S32* v)
{
	S32 vmin=min(min(r,g),b);
	S32 vmax=max(max(r,g),b);
	if (vmin==vmax) {
		*h=0;
	} else if (r==vmax) {
		*h=(60*(g-b)/(vmax-vmin)+360)%360;
	} else if (g==vmax) {
		*h=60*(b-r)/(vmax-vmin)+120;
	} else { // b==vmax
		*h=60*(r-g)/(vmax-vmin)+240;
	}
	if (vmax==0)
		*s=0;
	else
		*s=255*(vmax-vmin)/vmax;
	*v=vmax;
}

void hsv2rgb(S32 h,S32 s,S32 v,S32* r,S32* g,S32* b)
{
	S32 hint=h/60%6;
	S32 f60=h-hint*60;
	S32 p=v*(255-s)/255;
	S32 q=v*(60*255-f60*s)/(60*255);
	S32 t=v*(60*255-(59-f60)*s)/(60*255);
	switch(hint) {
		case 0:
			*r=v;
			*g=t;
			*b=p;
			break;
		case 1:
			*r=q;
			*g=v;
			*b=p;
			break;
		case 2:
			*r=p;
			*g=v;
			*b=t;
			break;
		case 3:
			*r=p;
			*g=q;
			*b=v;
			break;
		case 4:
			*r=t;
			*g=p;
			*b=v;
			break;
		case 5:
			*r=v;
			*g=p;
			*b=q;
			break;
		default:
			break;
	}
}

// some software cubemap stuff
cubemap::cubemap(string fname)
{
	const S32 extsize = 10;
	C8 ext[extsize];
	S32 hasext = mgetext(fname.c_str(),ext,extsize);
	if (hasext) { // file
		cross = gfxread32(fname.c_str());
	} else { // directory
		S32 i;
		pushandsetdirdown(fname.c_str());
		for (i=0;i<ncubeinfo;++i) {
			faceinfo* cl = &cubeinfo[i];
			bitmap32* part = getlores(cl->name);
#ifdef DOPATTERN1
			pattern1(part,cl->pat1);
#endif
			if (i == 0) {
				cross = bitmap32alloc(4*part->size.x,3*part->size.y,C32BLACK);
			}
			clipblit32(part,cross,0,0,cl->xoff*part->size.x,cl->yoff*part->size.y,part->size.x,part->size.y);
			bitmap32free(part);
		}
		popdir();
	}
	res = cross->size.x/4;
}

// do an actual cubemap
C32 cubemap::textureCube(const pointf3& n) const
{
	pointf3 absn;
	float* absna = (float*)&absn;
	absn.x = fabsf(n.x);
	absn.y = fabsf(n.y);
	absn.z = fabsf(n.z);
	float* na = (float*)&n; // cast the xyz to 012
	enum cubeface cf;
	if (absn.z >= absn.x) {
		// absn.x not largest
		if (absn.z >= absn.y) {
			// absn.y not largest
			// absn.z largest
			if (n.z >= 0.0f)
				cf = POSZ;
			else
				cf = NEGZ;
		} else {
			// absn.z not largest
			// absn.y largest
			if (n.y >= 0.0f)
				cf = POSY;
			else
				cf = NEGY;
		}
	} else {
		// absn.z not largest
		if (absn.x > absn.y) {
			// absn.y not largest
			// absn.x largest
			if (n.x > 0.0f)
				cf = POSX;
			else
				cf = NEGX;
		} else {
			// absn.x not largest
			// ay largest
			if (n.y > 0.0f)
				cf = POSY;
			else
				cf = NEGY;
		}
	}
	faceinfo* cl = &cubeinfo[cf];
	if (absna[cl->largeidx] < EPSILON) // vector too close to (0,0,0)
		return C32LIGHTMAGENTA;
	// if (cf != POSZ && cf != POSX) {
	// 	return cl->color;
	// }
	// // POSZ, project onto z = 1 plane
	// rearrange xyz to across down depth
	float depth = na[cl->largeidx];
	float across = na[cl->acrossidx];
	float down = na[cl->downidx];
	float idepth = 1.0f/depth;
	across *= idepth;
	down *= idepth;
	S32 sx = float2bitmap(across*cl->flipacross,res);
	S32 sy = float2bitmap( down*   cl->flipdown,res);
	return clipgetpixel32(cross,sx + cl->xoff*res,sy + cl->yoff*res);
}

pointf3 cubemap::textureCubeFloat(const pointf3& n) const
{
	C32 retC32 = textureCube(n);
	pointf3 ret;
	float idiv = 1.0f/256.0f;
	ret.x = retC32.r * idiv;
	ret.y = retC32.g * idiv;
	ret.z = retC32.b * idiv;
	return ret;
}

bitmap32* cubemap::getlores(const C8* fname)
{
	const S32 lorescutoff = 1024;
	bitmap32* o = gfxread32(fname);
	if (o->size.x > lorescutoff) {
		bitmap32* t = bitmap32alloc(lorescutoff,lorescutoff,C32BLACK);
		clipscaleblit32(o,t);
		bitmap32free(o);
		o = t;
	}
	return o;
}

// -1 to 1 inclusive goes to 0 to res-1
S32 cubemap::float2bitmap(float f,S32 res)
{
//#define CONSERVATIVE
#ifdef CONSERVATIVE
	float x = .5f*f + .5f; // 0 to 1
	x *= (res - 1.0f);
	x += .5f;
	return (S32)x;
#else // upto the leading edge
	float slight = .99999f;
	float x = .5f*f + .5f; // 0 to 1
	//x *= res;
	x *= res*slight;
	return (S32)x;
#endif
}

cubemap::faceinfo cubemap::cubeinfo[cubemap::ncubeinfo] = {
	{2,1,  0,2,1, -1,-1,  1,  "posx.jpg",C32GREEN},
	{0,1,  0,2,1, -1, 1,  1,  "negx.jpg",C32YELLOW},
	{1,0,  1,0,2,  1, 1,  1,  "posy.jpg",C32MAGENTA},
	{1,2,  1,0,2, -1, 1,  1,  "negy.jpg",C32LIGHTGRAY},
	{1,1,  2,0,1,  1,-1,  0,  "posz.jpg",C32RED},
	{3,1,  2,0,1,  1, 1,  0,  "negz.jpg",C32BLUE},
};
// end cubemap class
