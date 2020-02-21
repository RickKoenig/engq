/// surface functions/functors
/// sphere function
void sphere1_surf(float p,float q,pointf3* v,pointf3* n);
/// sphere functor
class spheref_surf {
    float rad;
public:
    spheref_surf(float rada) : rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
        float sanglo,canglo,sangla,cangla;
        p=2*PI*p;
        q=PI/2-PI*q;
        sanglo=sinf(p);
        canglo=cosf(p);
        sangla=sinf(q);
        cangla=cosf(q);
        n->x=sanglo*cangla;
        n->z=-canglo*cangla;
        n->y=sangla;
        v->x=rad*n->x;
		v->y=rad*n->y;
        v->z=rad*n->z;
    }
};

// for tangent space
/// sphere functor
class spheref_surf_tan {
    float rad;
public:
    spheref_surf_tan(float rada) : rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n,pointf3* t,pointf3* b)
    {
        float sanglo,canglo,sangla,cangla;
        p=2*PI*p;
        q=PI/2-PI*q;
        sanglo=sinf(p);
        canglo=cosf(p);
        sangla=sinf(q);
        cangla=cosf(q);
        n->x=sanglo*cangla;
        n->z=-canglo*cangla;
        n->y=sangla;
        v->x=rad*n->x;
		v->y=rad*n->y;
        v->z=rad*n->z;
		t->x=canglo;
		t->y=0;
		t->z=sanglo;
		b->x=-sangla*-sanglo;
		b->y=-cangla;
		b->z=-sangla*canglo;
    }
};
/// torus functor
class torusxzf_surf {
    float frad0;
	float frad1;
public:
    torusxzf_surf(float rad0a,float rad1a) : frad0(rad0a),frad1(rad1a) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
	p*=2*PI;
	q*=2*PI;
	float sp=sinf(p);
	float cp=cosf(p);
	float sq=sinf(q);
	float cq=cosf(q);
	v->y=-frad1*sq;
	const float radv=frad0+frad1*cq;
	v->x=radv*cp;
	v->z=radv*sp;
	n->y=-sq;
	n->x=cq*cp;
	n->z=cq*sp;
    }
};
/// squarexy function
//void square1xy_surf(float p,float q,pointf3* v,pointf3* n);

/// cylinder middle
class cylinderxz_mid_surf {
    float height,rad;
public:
    cylinderxz_mid_surf(float heighta,float rada) : height(heighta),rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
        float sa,ca;
        p=2*PI*p;
        q=height*(1-q);
		sa=sinf(p);
		ca=cosf(p);
		v->x=ca;
		v->z=sa;
		v->y=0;
		*n=*v;
		v->x*=rad;
		v->z*=rad;
		v->y=q;
    }
};

/// cylinder top
class cylinderxz_top_surf {
    float height,rad;
public:
    cylinderxz_top_surf(float heighta,float rada) : height(heighta),rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
        float sa,ca;
        p=2*PI*p;
		sa=sinf(p);
		ca=cosf(p);
		v->x=ca*q*rad;
		v->z=sa*q*rad;
		v->y=height;
		n->x=0;
		n->z=0;
		n->y=1;
     }
};

/// cylinder bottom
class cylinderxz_bot_surf {
    float rad;
public:
    cylinderxz_bot_surf(float heighta,float rada) : rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
        float sa,ca;
        p=2*PI*p;
		sa=sinf(p);
		ca=cosf(p);
		v->x=ca*(1-q)*rad;
		v->z=sa*(1-q)*rad;
		v->y=0;
		n->x=0;
		n->z=0;
		n->y=-1;
    }
};

/// plane
class planexz_surf {
    float xsize,zsize;
public:
    planexz_surf(float xsizea,float zsizea) : xsize(xsizea),zsize(zsizea) {}
    void operator()(float p,float q,pointf3* v,pointf3* n);
};

class planexy_surf {
    float xsize,ysize;
public:
    planexy_surf(float xsizea,float ysizea) : xsize(xsizea),ysize(ysizea) {}
    void operator()(float p,float q,pointf3* v,pointf3* n);
};

class planexy_surf0 {
    float xsize,ysize;
public:
    planexy_surf0(float xsizea,float ysizea) : xsize(xsizea),ysize(ysizea) {}
    void operator()(float p,float q,pointf3* v,pointf3* n);
};

class spotlightxy_surf {
	float rat,fos,foe;
	pointf2 norm2;
public:
	spotlightxy_surf(float theta,float falloffstart,float falloffend) : rat(tanf(theta*.5f)),fos(falloffstart),foe(falloffend)
	{
		norm2 = pointf2x(1,rat);
		normalize2d(&norm2,&norm2);
	}
    void operator()(float p,float q,pointf3* v,pointf3* n);
};


///
class conexy_surf {
	float rat,fos,foe;
	pointf2 norm2;
public:
	conexy_surf(float theta,float falloffstart,float falloffend) : rat(tanf(theta*.5f)),fos(falloffstart),foe(falloffend)
	{
		norm2 = pointf2x(1,rat);
		normalize2d(&norm2,&norm2);
	}
    void operator()(float p,float q,pointf3* v,pointf3* n);
};

/*
class conexy_surf {
	float rat,fos,foe;
	pointf2 norm2;
public:
	conexy_surf(float theta,float falloffstart,float falloffend) : rat(tanf(theta*.5f)),fos(falloffstart),foe(falloffend)
	{
		norm2 = pointf2x(1,rat);
		normalize2d(&norm2,&norm2);
	}
    void operator()(float p,float q,pointf3* v,pointf3* n);
};
*/
/// strand utility functions
extern pointf3 brd2norm; // a global norm used for all 3 braid2 functions
/// returns an axis aligned vector close to a normal of vector
pointf3 findnormaxis(pointf3& p);
/// uses brd2norm and newdir returns newnorm
pointf3 adjustnorm(pointf3& oldnorm,pointf3& newdir);
pointf3 findanorm(pointf3& dir);

/// call in order of bot,mid,top
/// brd2 base
const float STAND_EPSILON=.0001f;
template<typename T>
class strand_mid_surf {
    T fun;
    float rad;
public:
    strand_mid_surf(T funa,float rada) : fun(funa),rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
//    	logger("in braid2 with p %f, q %f\n",p,q);
    	const float h=STAND_EPSILON;
//    	const float hr=1/h;
    	pointf3 b=fun(p);
    	pointf3 bp=fun(p+h/2);
    	pointf3 bm=fun(p-h/2);
		pointf3 dir;
		pointf3 anorm;
		dir.x=bp.x-bm.x;
		dir.y=bp.y-bm.y;
		dir.z=bp.z-bm.z;
		normalize3d(&dir,&dir);
//    	anorm=findanorm(dir);
//		anorm=brd2norm;
		anorm=adjustnorm(brd2norm,dir);
        q=2*PI*q;
    	dir.w=-q;
    	pointf3 qt;
    	rotaxis2quat(&dir,&qt);
     	quatrot(&qt,&anorm,n);
		v->x=b.x+rad*n->x;
		v->y=b.y+rad*n->y;
		v->z=b.z+rad*n->z;
		brd2norm=anorm; // remember last norm
    }
};

/// brd2 bot
template<typename T>
class strand_bot_surf {
	T fun;
	float rad;
public:
    strand_bot_surf(T funa,float rada) : fun(funa),rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n);
/*    {
//    	logger("in braid2cap0 with p %f, q %f\n",p,q);
    	const float h=STAND_EPSILON;
//    	const float hr=1/h;
    	pointf3 b=fun(0.0f);
    	pointf3 bp=fun(0.0f+h/2);
    	pointf3 bm=fun(0.0f-h/2);
		pointf3 dir;
		pointf3 anorm;
		dir.x=bp.x-bm.x;
		dir.y=bp.y-bm.y;
		dir.z=bp.z-bm.z;
    	normalize3d(&dir,&dir);
    	anorm=findanorm(dir);
        p*=rad;
        q=2*PI*q;
    	dir.w=-q;
    	pointf3 qt;
    	rotaxis2quat(&dir,&qt);
     	quatrot(&qt,&anorm,n);
		v->x=b.x+p*n->x;
		v->y=b.y+p*n->y;
		v->z=b.z+p*n->z;
		n->x=-dir.x;
		n->y=-dir.y;
		n->z=-dir.z;
		brd2norm=anorm; // remember last norm
    } */
};

template<typename T>
void strand_bot_surf<T>::operator()(float p,float q,pointf3* v,pointf3* n)
{
// 	logger("in braid2cap0 with p %f, q %f\n",p,q);
   	const float h=STAND_EPSILON;
//	const float hr=1/h;
	pointf3 b=fun(0.0f);
	pointf3 bp=fun(0.0f+h/2);
	pointf3 bm=fun(0.0f-h/2);
	pointf3 dir;
	pointf3 anorm;
	dir.x=bp.x-bm.x;
	dir.y=bp.y-bm.y;
	dir.z=bp.z-bm.z;
	normalize3d(&dir,&dir);
	anorm=findanorm(dir);
    p*=rad;
    q=2*PI*q;
	dir.w=-q;
	pointf3 qt;
	rotaxis2quat(&dir,&qt);
 	quatrot(&qt,&anorm,n);
	v->x=b.x+p*n->x;
	v->y=b.y+p*n->y;
	v->z=b.z+p*n->z;
	n->x=-dir.x;
	n->y=-dir.y;
	n->z=-dir.z;
	brd2norm=anorm; // remember last norm
}

/// brd2 top
template<typename T>
class strand_top_surf {
	T fun;
	float rad;
public:
    strand_top_surf(T funa,float rada) : fun(funa),rad(rada) {}
    void operator()(float p,float q,pointf3* v,pointf3* n)
    {
//		logger("in braid2cap1 with p %f, q %f\n",p,q);
    	const float h=STAND_EPSILON;
//    	const float hr=1/h;
    	pointf3 b=fun(1.0f);
    	pointf3 bp=fun(1.0f+h/2);
    	pointf3 bm=fun(1.0f-h/2);
		pointf3 dir;
		pointf3 anorm;
		dir.x=bp.x-bm.x;
		dir.y=bp.y-bm.y;
		dir.z=bp.z-bm.z;
    	normalize3d(&dir,&dir);
//    	anorm=findanorm(dir);
		anorm=brd2norm;
        p=(1.0f-p)*rad;
        q=2*PI*q;
    	dir.w=-q;
    	pointf3 qt;
    	rotaxis2quat(&dir,&qt);
     	quatrot(&qt,&anorm,n);
		v->x=b.x+p*n->x;
		v->y=b.y+p*n->y;
		v->z=b.z+p*n->z;
		*n=dir;
    }
};

template<typename T>
void buildpatch(modelb* mod,int np,int nq,float tileu,float tilev,
  T paramfunc,const C8* texname,const C8* matname)
{
/// build the verts, uvs and norms for the model
	S32 nv=(np+1)*(nq+1);
	S32 nf=np*nq*2;
	struct pointf3* verts=new pointf3[nv];
	struct uv* uvs=new uv[nv];
	struct pointf3* norms=new pointf3[nv];
	S32 i,j;
	pointf3* vp=verts;
	pointf3* nrmp=norms;
	uv* uvp=uvs;
	float trati=tileu/np;
	float tratj=tilev/nq;
	for (i=0;i<=np;++i) {
	    float inti0=(float)i/np;
		for (j=0;j<=nq;++j) {
			float intj0=(float)j/nq;
			paramfunc(inti0,intj0,&vp[0],&nrmp[0]);
 			uvp->u=i*trati;
 			uvp->v=j*tratj;
			++vp;
			++nrmp;
			++uvp;
		}
	}
	mod->copyverts(verts,nv);
	mod->copyuvs0(uvs,nv);
	mod->copynorms(norms,nv);
	delete[] verts;
	delete[] uvs;
	delete[] norms;
 // create a texture
	if (texname) {
		textureb* texmat0=texture_create(texname);
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
/// create a material with the texture
		mod->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,50,nf,nv);
	} else
		mod->addmat(matname,SMAT_HASWBUFF,0,&F32WHITE,50,nf,nv);
/// make all the faces
	for (i=0;i<np;++i) {
		for (j=0;j<nq;++j) {
			S32 f0=j+(nq+1)*i;
			S32 f1=f0+(nq+1);
			S32 f2=f0+1;
			S32 f3=f1+1;
			mod->addface(f0,f1,f2);
			mod->addface(f3,f2,f1);
		}
	}
#if 0 // test calc uvs and calc norms, right now used in u_platonic.cpp
	//mod->calcuvs(pointf3x(0,100,0)); // test xyz to uv mapper,  move center up just for XZ planes, otherwise v will be constant and look bad
	mod->calcuvs(); // test xyz to uv mapper,  keep center at origin for spheres
	mod->calcnorms(); // test verts,faces to norms
#endif
	mod->close();
}

template<typename T>
void buildpatchc(modelb* mod,int np,int nq,float tileu,float tilev,
  T paramfunc,const C8* texname,const C8* matname)
{
/// build the verts, uvs and cverts for the model
	S32 nv=(np+1)*(nq+1);
	S32 nf=np*nq*2;
	struct pointf3* verts=new pointf3[nv];
	struct uv* uvs=new uv[nv];
	struct pointf3* cverts=new pointf3[nv];
	S32 i,j;
	pointf3* vp=verts;
	pointf3* cvertp=cverts;
	uv* uvp=uvs;
	float trati=tileu/np;
	float tratj=tilev/nq;
	for (i=0;i<=np;++i) {
	    float inti0=(float)i/np;
		for (j=0;j<=nq;++j) {
			float intj0=(float)j/nq;
			paramfunc(inti0,intj0,&vp[0],&cvertp[0]);
 			uvp->u=i*trati;
 			uvp->v=j*tratj;
			++vp;
			++cvertp;
			++uvp;
		}
	}
	mod->copyverts(verts,nv);
	mod->copyuvs0(uvs,nv);
	mod->copycverts(cverts,nv);
	delete[] verts;
	delete[] uvs;
	delete[] cverts;
 // create a texture
	if (texname) {
		textureb* texmat0=texture_create(texname);
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
/// create a material with the texture
		mod->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,&F32WHITE,50,/* 2* */nf,nv);
	} else
		mod->addmat(matname,SMAT_HASWBUFF,0,&F32WHITE,50,/* 2* */nf,nv);
/// make all the faces
	for (i=0;i<np;++i) {
		for (j=0;j<nq;++j) {
			S32 f0=j+(nq+1)*i;
			S32 f1=f0+(nq+1);
			S32 f2=f0+1;
			S32 f3=f1+1;
			mod->addface(f0,f1,f2,false);
			mod->addface(f3,f2,f1,false);
		}
	}
	mod->close();
}

template<typename T>
void buildpatch2t(modelb* mod,int np,int nq,float tileu,float tilev,
  T paramfunc,const C8* texname,const C8* texname2,const C8* matname)
{
/// build the verts, uvs and norms for the model
	S32 nv=(np+1)*(nq+1);
	S32 nf=np*nq*2;
	struct pointf3* verts=new pointf3[nv];
	struct uv* uvs=new uv[nv];
	struct pointf3* norms=new pointf3[nv];
	S32 i,j;
	pointf3* vp=verts;
	pointf3* nrmp=norms;
	uv* uvp=uvs;
	float trati=tileu/np;
	float tratj=tilev/nq;
	for (i=0;i<=np;++i) {
		float inti0=(float)i/np;
		for (j=0;j<=nq;++j) {
			float intj0=(float)j/nq;
			paramfunc(inti0,intj0,&vp[0],&nrmp[0]);
 			uvp->u=i*trati;
 			uvp->v=j*tratj;
			++vp;
			++nrmp;
			++uvp;
		}
	}
	mod->copyverts(verts,nv);
	mod->copyuvs0(uvs,nv);
	mod->copynorms(norms,nv);
	delete[] verts;
	delete[] uvs;
	delete[] norms;
 // create a texture
	textureb* texmat0=texture_create(texname);
	if (texture_getrc(texmat0)==1) {
		texmat0->load();
	}
	textureb* texmat1=texture_create(texname2);
	if (texture_getrc(texmat1)==1) {
		texmat1->load();
	}
/// create a material with the texture
	mod->addmat2t(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,texmat1,&F32WHITE,50,nf,nv);
/// make all the faces
	for (i=0;i<np;++i) {
		for (j=0;j<nq;++j) {
			S32 f0=j+(nq+1)*i;
			S32 f1=f0+(nq+1);
			S32 f2=f0+1;
			S32 f3=f1+1;
			mod->addface(f0,f1,f2);
			mod->addface(f3,f2,f1);
		}
	}
	mod->close();
}

// for tangent space
template<typename T>
void buildpatch_tan(modelb* mod,int np,int nq,float tileu,float tilev,
  T paramfunc_tan,const C8* texname,const C8* normalmapname,const C8* matname)
{
/// build the verts, uvs and norms for the model
	S32 nv=(np+1)*(nq+1);
	S32 nf=np*nq*2;
	struct pointf3* verts=new pointf3[nv];
	struct uv* uvs=new uv[nv];
	struct pointf3* norms=new pointf3[nv];
	struct pointf3* tangents=new pointf3[nv];
	struct pointf3* binorms=new pointf3[nv];
	S32 i,j;
	pointf3* vp=verts;
	pointf3* nrmp=norms;
	pointf3* tp=tangents;
	pointf3* bp=binorms;
	uv* uvp=uvs;
	float trati=tileu/np;
	float tratj=tilev/nq;
	for (i=0;i<=np;++i) {
	    float inti0=(float)i/np;
		for (j=0;j<=nq;++j) {
			float intj0=(float)j/nq;
			paramfunc_tan(inti0,intj0,&vp[0],&nrmp[0],&tp[0],&bp[0]);
 			uvp->u=i*trati;
 			uvp->v=j*tratj;
			++vp;
			++nrmp;
			++uvp;
			++tp;
			++bp;
		}
	}
//#define SHOWVERTS
#ifdef SHOWVERTS
	for (i=0;i<nv;++i)
		logger("vert %3d: v (%6.3f,%6.3f,%6.3f), n (%6.3f,%6.3f,%6.3f), t (%6.3f,%6.3f,%6.3f), b (%6.3f,%6.3f,%6.3f), uv (%6.3f,%6.3f)\n",
			i,
			verts[i].x,verts[i].y,verts[i].z,
			norms[i].x,norms[i].y,norms[i].z,
			tangents[i].x,tangents[i].y,tangents[i].z,
			binorms[i].x,binorms[i].y,binorms[i].z,
			uvs[i].u,uvs[i].v);
#endif
	mod->copyverts(verts,nv);
	mod->copyuvs0(uvs,nv);
	mod->copynorms(norms,nv);
	mod->copytangents(tangents,nv);
	mod->copybinorms(binorms,nv);
	delete[] verts;
	delete[] uvs;
	delete[] norms;
	delete[] tangents;
	delete[] binorms;
// create a texture
	textureb* texmat0=texture_create(texname);
	if (texture_getrc(texmat0)==1) {
		texmat0->load();
	}
	pushandsetdir("skybox");
	textureb* texmat1=texture_create(normalmapname);
	if (texture_getrc(texmat1)==1) {
		texmat1->load();
	}
	popdir();
/// create a material with the texture
	mod->addmat2t(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,texmat1,&F32WHITE,50,nf,nv);
/// make all the faces
	for (i=0;i<np;++i) {
		for (j=0;j<nq;++j) {
			S32 f0=j+(nq+1)*i;
			S32 f1=f0+(nq+1);
			S32 f2=f0+1;
			S32 f3=f1+1;
			mod->addface(f0,f1,f2);
			mod->addface(f3,f2,f1);
		}
	}
	mod->close();
}

/// build the braid root with body and caps
template <typename T>
tree2* buildstrand(const T& func,int pres,int qres,int pcapres,float brad2,const C8* matname)
{
	tree2* bt=new tree2("brdtree");
/// bottom cap
	modelb* modcc0b=model_create(unique());
	if (model_getrc(modcc0b)==1) {
//        pushandsetdir("gfxtest");
		buildpatch2t(modcc0b,pcapres,qres,(float)pcapres,(float)qres,strand_bot_surf<T>(func,brad2),"maptest.tga","bark.tga",matname);
//	    popdir();
	}
	tree2* brdtreec0=new tree2("brdtreec0");
	brdtreec0->setmodel(modcc0b);
//	delete brdtreec0;
	bt->linkchild(brdtreec0);
/// body
	modelb* modcmb=model_create(unique());
//	pointf3 dirb={0,1,0,0};
	if (model_getrc(modcmb)==1) {
//        pushandsetdir("gfxtest");
		buildpatch2t(modcmb,pres,qres,(float)pres,(float)qres,strand_mid_surf<T>(func,brad2),"maptest.tga","rengst.jpg",matname);
//	    popdir();
	}
	tree2* brdtreem=new tree2("brdtreem");
	brdtreem->setmodel(modcmb);
 	bt->linkchild(brdtreem);
/// top cap
	modelb* modcc1b=model_create(unique());
	if (model_getrc(modcc1b)==1) {
//        pushandsetdir("gfxtest");
		buildpatch2t(modcc1b,pcapres,qres,(float)pcapres, (float)qres,strand_top_surf<T>(func,brad2),"maptest.tga","bark.tga",matname);
//	    popdir();
	}
	tree2* brdtreec1=new tree2("brdtreec1");
	brdtreec1->setmodel(modcc1b);
	bt->linkchild(brdtreec1);
	return bt;
}

tree2* buildprism(const pointf3& size,const C8* texname,const C8* matname,const C8* modname = 0);
tree2* buildpyramid(/*const pointf3& size,*/const C8* texname,const C8* matname,const C8* modname = 0);
tree2* buildprism2t(const pointf3& size,const C8* texname,const C8* texname2,const C8* matname);
tree2* buildprism6t(const pointf3& size,const C8* texname[],const C8* matname,const C8* modname = 0); // an array of texture names for each of the 6 sides, 6 different materials
tree2* buildprismuvs(const pointf3& size,const C8* texname,const C8* matname,const uv& uv0,const uv& vu1);
tree2* buildskybox(const pointf3& size,const C8* texname,const C8* matname);
tree2* buildcylinder_xz(const pointf3& size,const C8* texname,const C8* matname);
tree2* buildcylinderc_xz(const pointf3& size,const C8* texname,const C8* matname, pointf3 bodycolor, pointf3 topcolor, pointf3 botcolor);
tree2* buildplane_xy(const pointf2& sizexy,const C8* texname,const C8* matname,const C8* modname = 0);
tree2* buildplane_xy_uv(const pointf2& sizexy,const pointf2& uvend,const C8* texname,const C8* matname,const C8* modname = 0);
tree2* buildsphere(float rad,const C8* texname,const C8* matname);
tree2* buildtorus_xz(float radi0,float radi1,const C8* texname,const C8* matname,S32 pnum = 20,S32 qnum = 20);
tree2* buildspotlight_xy(float theta,float falloffstart,float falloffend,const C8* matname); // white ghostly model of cone shaped spotlight
tree2* buildcone_xy(float theta,float falloffstart,float falloffend,const C8* matname,pointf3 conebodycolor = F32WHITE);

// rgb to hsv and back
// rgb 0-255
// hsv 0-359,0-255,0-255
S32 supersat(S32 sat,S32 r,S32 g,S32 b,S32* r2,S32* g2,S32* b2); // sat 0-319, 256-319 (colorizes gray), returns sat 0-255
void rgb2hsv(S32 r,S32 g,S32 b,S32* h,S32* s,S32* v);
void hsv2rgb(S32 h,S32 s,S32 v,S32* r,S32* g,S32* b);

const int nprismverts=24;
const int nprismfaces=12;
extern const pointf3 prismverts[nprismverts];
extern const pointf3 prismnorms[nprismverts];
extern const uv prismuvs[nprismverts];
extern const face prismfaces[nprismfaces];

// some software cubemap stuff
// software cubemap class

class cubemap {
	//bitmap32 sides[6];
	bitmap32* cross;
	S32 res; // size of on side of a square
//#define DOPATTERN1
	struct faceinfo {
		S32 xoff; // offset into cross
		S32 yoff;

		S32 largeidx;	// z
		S32 acrossidx;	// x
		S32 downidx;	// y

		S32 flipacross;
		S32 flipdown;

		S32 pat1; // parity of checkerboard pattern

		const C8* name; // name or directory
		C32 color;		// base color if not using textures
	};
	const static S32 ncubeinfo = 6;
	static faceinfo cubeinfo[ncubeinfo];
	enum cubeface {POSX,NEGX,POSY,NEGY,POSZ,NEGZ};

public:
	cubemap(string fname);
	// do an actual cubemap
	C32 textureCube(const pointf3& n) const;
	pointf3 textureCubeFloat(const pointf3& n) const;
	~cubemap()
	{
		bitmap32free(cross);
	}
	const bitmap32* getbitmap() const
	{
		return cross;
	}
	void loginfo() const
	{
		logger("cubemap bitmap size = %d %d, res size = %d\n",cross->size.x,cross->size.y,res);
	}
private:
	static bitmap32* getlores(const C8* fname);
public:
	// -1 to 1 inclusive goes to 0 to res-1
	static S32 float2bitmap(float f,S32 res);
};

