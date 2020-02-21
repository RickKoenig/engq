// handy routines for building various 3d objects
#define D2_3D
#include <m_eng.h>
#include "system/u_states.h"
#include "modelutil.h"

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


pointf3 prismverts[24]={
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
pointf3 prismnorms[24]={
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
uv prismuvs[24]={
// front
	{0,0},
	{1,0},
	{0,1},
	{1,1},
// back
	{0,0},
	{1,0},
	{0,1},
	{1,1},
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
face prismfaces[12]={
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

#define OT (1.0f/3.0f)
#define TT (2.0f/3.0f)
#define ER1 (1.0f/4000.0f) 
#define ER2 (1.0f/400.0f) // eliminate white lines
static uv skyboxuvs1[24]={
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
#if 1
static uv skyboxuvs2[24]={ // less err where faces join
// front
	{  1-ER2,OT+ER2},
	{.75f+ER1,OT+ER2},
	{  1-ER2,TT-ER2},
	{.75f+ER1,TT-ER2},
// back
	{ .5f-ER1,OT+ER1},
	{.25f+ER1,OT+ER1},
	{ .5f-ER1,TT-ER1},
	{.25f+ER1,TT-ER1},
// right
	{.75f-ER1,OT+ER2},
	{ .5f+ER1,OT+ER2},
	{.75f-ER1,TT-ER2},
	{ .5f+ER1,TT-ER2},
// left
	{.25f-ER1,OT+ER2},
	{  0+ER2,OT+ER2},
	{.25f-ER1,TT-ER2},
	{  0+ER2,TT-ER2},
// top
	{.25f+ER2,OT-ER1},
	{ .5f-ER2,OT-ER1},
	{.25f+ER2,0 +ER2},
	{ .5f-ER2,0 +ER2},
// bot
	{.25f+ER2,1-ER2},
	{ .5f-ER2,1-ER2},
	{.25f+ER2,TT+ER1},
	{ .5f-ER2,TT+ER1},
};
#else
static uv skyboxuvs2[24]={
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
#endif
static face skyboxfaces[12]={
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

tree2* buildprism(const pointf3& size,C8* texname,C8* matname)
{
	tree2* t=new tree2("prism");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
//        pushandsetdir("gfxtest");
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
			m->addmat(matname,SMAT_HASWBUFF,0,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();

//	    popdir();
	}
	t->setmodel(m);
	return t;
}

tree2* buildprismuvs(const pointf3& size,C8* texname,C8* matname,const uv& uv0,const uv& uv1)
{
	tree2* t=new tree2("prism");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
//        pushandsetdir("gfxtest");
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
//			prismuvsm[i].u=prismuvs[i].u;
//			prismuvsm[i].v=prismuvs[i].v;
		}
		m->copyuvs0(prismuvsm,24);
		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,12,24);
		m->addfaces(prismfaces,12,0);
		m->close();

//	    popdir();
	}
	t->setmodel(m);
	return t;
}

tree2* buildprism2t(const pointf3& size,C8* texname,C8* texname2,C8* matname)
{
	tree2* t=new tree2("prism2t");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
//        pushandsetdir("gfxtest");
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

//	    popdir();
	}
	t->setmodel(m);
	return t;
}

tree2* buildskybox(const pointf3& size,const C8* texname,const C8* matname)
{
	tree2* t=new tree2("skybox");
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
//      pushandsetdir("gfxtest");
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
/*		for (i=0;i<24;++i) {
			tv[i].x=-prismnorms[i].x;
			tv[i].y=-prismnorms[i].y;
			tv[i].z=-prismnorms[i].z;
		} 
		m->copynorms(tv,24); */
		delete[] tv;
//		logger("tsize of '%s' is %d\n",texname,texmat0->tsize.x);
		if (texmat0->tsize.x>2000) // hack
			m->copyuvs0(skyboxuvs1,24);
		else
			m->copyuvs0(skyboxuvs2,24);
		m->addmat(matname,SMAT_HASTEX|SMAT_CLAMP|SMAT_ISSKY|SMAT_HASNOFOG,texmat0,0,50,12,24); // no wbuffer, drawn first
//		m->addmat(matname,SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,12,24);
//		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE|SMAT_ISSKY,texmat0,0,50,12,24);
//		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,12,24);
		m->addfaces(skyboxfaces,12,0);
		m->close();

//	    popdir();
	}
	t->setmodel(m);
	return t;
}

tree2* buildcylinder_xz(const pointf3& size,C8* texname,C8* matname)
{
/// build the cylinder root
	tree2* cyltree=new tree2("cyltree");
/// build the cylinder body
	modelb* modcm=model_create("cylm");
	if (model_getrc(modcm)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcm,10,1,1,1,cylinderxz_mid_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreem=new tree2("cyltreem");
	cyltreem->setmodel(modcm);
 	cyltree->linkchild(cyltreem);
/// build cyl cap0
	modelb* modcc0=model_create("cylbot");
	if (model_getrc(modcc0)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcc0,10, 1,1, 1,cylinderxz_bot_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreec0=new tree2("cyltreec0");
	cyltreec0->setmodel(modcc0);
	cyltree->linkchild(cyltreec0);
/// build cyl cap1
	modelb* modcc1=model_create("cyltop");
	if (model_getrc(modcc1)==1) {
//        pushandsetdir("gfxtest");
		buildpatch(modcc1,10, 1,1, 1,cylinderxz_top_surf(size.y,size.x),texname,matname);
//	    popdir();
	}
	tree2* cyltreec1=new tree2("cyltreec1");
	cyltreec1->setmodel(modcc1);
	cyltree->linkchild(cyltreec1);
	return cyltree;
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

tree2* buildsphere(float rad,const C8* texname,const C8* matname)
{
	modelb* m=model_create(unique());
	if (model_getrc(m)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(m,35,35,3,3,spheref_surf(rad),texname,matname);
	    popdir();
	}
	tree2* t=new tree2("sphere");
	t->setmodel(m);
	return t;
}

