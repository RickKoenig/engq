#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "hw35_line2road.h"
#include "m_perf.h"

namespace hw35_line2road {
float testheight=-4;
//#define EXTRAGRIDCHECK
//#include "gamestate.h"
struct colltree l2r_colltrees[MAXCOLLTREES];
int l2r_ncolltrees;

//#define USEJUSTPLANEXZ
#define ANOTHERPASS

// quick and clean, stubs line2road
// temporary, just for testing only, supposed to return a tree pointer
#ifdef USEJUSTPLANEXZ
TREE *st2_line2road(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	float t;
	if (top->y>=testheight+EPSILON && bot->y<=testheight-EPSILON) { // line does cross 0 plane
		if (bestnorm) {
			bestnorm->x=0;
			bestnorm->y=1;
			bestnorm->z=0;
		}
		t=(testheight-top->y)/(bot->y-top->y);
		interp3d(top,bot,t,bestintsect);
		return 1;
	}
	return 0;
}

TREE *st2_line2roadlo(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	float t;
	if (top->y>=testheight+EPSILON && bot->y<=testheight-EPSILON) { // line does cross 0 plane
		if (bestnorm) {
			bestnorm->x=0;
			bestnorm->y=1;
			bestnorm->z=0;
		}
		t=(testheight-top->y)/(bot->y-top->y);
		interp3d(top,bot,t,bestintsect);
		return 1;
	}
	return 0;
}


void st2_setupmatfuncs(struct smatfunc *mf,int nmf)
{
}

void st2_enablesurffunc()
{
}

void st2_callsurffunc()
{
}

// loads 1 object (init)
void st2_buildcolgrid(TREE *t)
{
}

// load alot of objects (init)
void st2_loadcollist(char *filename,TREE *root)
{
}

void st2_freecollgrids()
{
}

int gettrackchecksum()
{
	return 0;
}

#else

#define QFACEALLOCSTEP 4
//#define QCOLLGRIDX 64
//#define QCOLLGRIDZ 64
//#define QCOLLGRIDX 128
//#define QCOLLGRIDZ 128
#define QCOLLGRIDX 256
#define QCOLLGRIDZ 256
//#define QCOLLGRIDX 512
//#define QCOLLGRIDZ 512
static struct smatfunc *local_mf;
static int local_nmf;

static tree2 **qtreelist;
static int ntreelist;
static int ntreelistalloced;

struct onegrid {
	int nface;
	int nfacealloced;
	FACE *faceidx; // nfaceidx
	TREE **colobj; // nfaceidx
	VEC *facenorms;
	VEC minv,maxv; // only x and z are used..
};

typedef struct
{
	VEC colbboxmin,colbboxmax;
	int nvert;
	int totfaces,totgridfaces;
	VEC *verts;
	float offx,offz;
	float mulx,mulz;
	struct onegrid grid[QCOLLGRIDZ][QCOLLGRIDX];
} QCOLLGRID;

static QCOLLGRID qlwos;

#define MAXFUNCS 30

static struct qmatfunc qmatfuncs[MAXFUNCS];
static S32 nqmatfuncs;

///////// lolevel routines
static int tri2pointxz(VEC *t0,VEC *t1,VEC *t2,VEC *p)
{
	VEC r0,r1,r2;
	float c0,c1,c2;
	r0.x=t0->x-p->x;
	r0.z=t0->z-p->z;
	r1.x=t1->x-p->x;
	r1.z=t1->z-p->z;
	r2.x=t2->x-p->x;
	r2.z=t2->z-p->z;
	c0=r0.x*r1.z-r0.z*r1.x;
	c1=r1.x*r2.z-r1.z*r2.x;
	c2=r2.x*r0.z-r2.z*r0.x;
	if (c0>=0 && c1>=0 && c2>=0)
		return 1;
	if (c0<=0 && c1<=0 && c2<=0)
		return 1;
	return 0;
}

static VEC *vecbase;
static int line2tri(FACE *tri,VEC *top,VEC *bot,VEC *intsect,VEC *norm)
{
	float fudge=-1e-5f;
	VEC *v0,*v1,*v2;
	VEC del,topmv0;
	int *vp;
	float t,n,d;
	VEC v0mi,v1mi,v2mi;
	VEC c0,c1,c2;
	float d0,d1,d2;
// solve for intersection of line with plane
	del.x=bot->x-top->x;
	del.y=bot->y-top->y;
	del.z=bot->z-top->z;
	d=dot3d(&del,norm);
	if (d>-EPSILON)
		return 0;
	vp=tri->vertidx;
//	v0=&qlwos.verts[vp[0]];
	v0=&vecbase[vp[0]];
	topmv0.x=top->x-v0->x;
	topmv0.y=top->y-v0->y;
	topmv0.z=top->z-v0->z;
	n=dot3d(&topmv0,norm);
	if (n<0 || n>-d)
//	if (t<0 || t>1) // this is what t would do, save division
		return 0;
	t=-n/d;
	intsect->x=top->x+t*del.x;
	intsect->y=top->y+t*del.y;
	intsect->z=top->z+t*del.z;
// now check to see if intsect is on the triangle
	v1=&vecbase[vp[1]];
	v2=&vecbase[vp[2]];
	v0mi.x=v0->x-intsect->x;
	v0mi.y=v0->y-intsect->y;
	v0mi.z=v0->z-intsect->z;
	v1mi.x=v1->x-intsect->x;
	v1mi.y=v1->y-intsect->y;
	v1mi.z=v1->z-intsect->z;
	v2mi.x=v2->x-intsect->x;
	v2mi.y=v2->y-intsect->y;
	v2mi.z=v2->z-intsect->z;
	cross3d(&v0mi,&v1mi,&c0);
	cross3d(&v1mi,&v2mi,&c1);
	cross3d(&v2mi,&v0mi,&c2);
	d0=dot3d(&c0,norm);
	d1=dot3d(&c1,norm);
	d2=dot3d(&c2,norm);
	if (d0>=fudge && d1>=fudge && d2>=fudge) {
		return 1;
	}
	return 0;
}

// only check xz
static int tri2rectxz(VEC *t0,VEC *t1,VEC *t2,VEC *b0,VEC *b1)
{
	VEC rp;
	float ix,iz,del;
// see if any tripoints inside rectangle
	if (t0->x>=b0->x && t0->x<=b1->x && t0->z>=b0->z && t0->z<=b1->z)
		return 1;
	if (t1->x>=b0->x && t1->x<=b1->x && t1->z>=b0->z && t1->z<=b1->z)
		return 1;
	if (t2->x>=b0->x && t2->x<=b1->x && t2->z>=b0->z && t2->z<=b1->z)
		return 1;
// see if any rectpoints inside triangle
	if (tri2pointxz(t0,t1,t2,b0))
		return 1;
	if (tri2pointxz(t0,t1,t2,b1))
		return 1;
	rp.x=b0->x;
	rp.z=b1->z;
	if (tri2pointxz(t0,t1,t2,&rp))
		return 1;
	rp.x=b1->x;
	rp.z=b0->z;
	if (tri2pointxz(t0,t1,t2,&rp))
		return 1;
// now check all intersections of rect to tri
//t0-t1 z
	del=t0->z-t1->z;
	if (del<-EPSILON || del>EPSILON) {
		ix=t0->x+(t1->x-t0->x)*(b0->z-t0->z)/(t1->z-t0->z);
		if (ix>=b0->x && ix<=b1->x)
			return 1;
		ix=t0->x+(t1->x-t0->x)*(b1->z-t0->z)/(t1->z-t0->z);
		if (ix>=b0->x && ix<=b1->x)
			return 1;
	}
//t0-t1 x
	del=t0->x-t1->x;
	if (del<-EPSILON || del>EPSILON) {
		iz=t0->z+(t1->z-t0->z)*(b0->x-t0->x)/(t1->x-t0->x);
		if (iz>=b0->z && iz<=b1->z)
			return 1;
		iz=t0->z+(t1->z-t0->z)*(b1->x-t0->x)/(t1->x-t0->x);
		if (iz>=b0->z && iz<=b1->z)
			return 1;
	}
//t1-t2 x
	del=t1->z-t2->z;
	if (del<-EPSILON || del>EPSILON) {
		ix=t1->x+(t2->x-t1->x)*(b0->z-t1->z)/(t2->z-t1->z);
		if (ix>=b0->x && ix<=b1->x)
			return 1;
		ix=t1->x+(t2->x-t1->x)*(b1->z-t1->z)/(t2->z-t1->z);
		if (ix>=b0->x && ix<=b1->x)
			return 1;
	}
//t1-t2 z
	del=t1->x-t2->x;
	if (del<-EPSILON || del>EPSILON) {
		iz=t1->z+(t2->z-t1->z)*(b0->x-t1->x)/(t2->x-t1->x);
		if (iz>=b0->z && iz<=b1->z)
			return 1;
		iz=t1->z+(t2->z-t1->z)*(b1->x-t1->x)/(t2->x-t1->x);
		if (iz>=b0->z && iz<=b1->z)
			return 1;
	}
//t2-t0 x
	del=t2->z-t0->z;
	if (del<-EPSILON || del>EPSILON) {
		ix=t2->x+(t0->x-t2->x)*(b0->z-t2->z)/(t0->z-t2->z);
		if (ix>=b0->x && ix<=b1->x)
			return 1;
		ix=t2->x+(t0->x-t2->x)*(b1->z-t2->z)/(t0->z-t2->z);
		if (ix>=b0->x && ix<=b1->x)
			return 1;
	}
//t2-t0 z
	del=t2->x-t0->x;
	if (del<-EPSILON || del>EPSILON) {
		iz=t2->z+(t0->z-t2->z)*(b0->x-t2->x)/(t0->x-t2->x);
		if (iz>=b0->z && iz<=b1->z)
			return 1;
		iz=t2->z+(t0->z-t2->z)*(b1->x-t2->x)/(t0->x-t2->x);
		if (iz>=b0->z && iz<=b1->z)
			return 1;
	}
	return 0;
}


////////// grid conversion /////
// vec 2 grid
static void getgridxz(VEC *v,int *x,int *z)
{
	*x=(int)(qlwos.mulx*v->x+qlwos.offx);
	*z=(int)(qlwos.mulz*v->z+qlwos.offz);
/*
#ifdef EXTRAGRIDCHECK
	if (*x<0 || *x>=QCOLLGRIDX || *z<0 || *z>=QCOLLGRIDZ)
		errorexit("err: grid outa bounds %d %d",*x,*z);
#endif
*/
}

// grid 2 vec
static void getvecfromgridxz(VEC *v,int x,int z)
{
	v->x=((float)x-qlwos.offx)/qlwos.mulx;
	v->z=((float)z-qlwos.offz)/qlwos.mulz;
}

// build collision grid
static void st2_buildcollgrid(TREE *t)
{
	S32 i;
	U32 k;
	vector<S32> mattrans;
	int gx,gz,gxmin,gzmin,gxmax,gzmax;
	BODY *b;
	VEC /*uvec,vvec,*/*v0,*v1,*v2;//,*n;
	VEC *v;
	FACE f;
	struct onegrid *g;
	U32 nf;
	int nv;
	b=t->mod;
	if (!b)
		return;
//	logger("<<<<< buildcolgrid '%s'\n",t->mod->name);
//	memset(mattrans,0,sizeof(mattrans));
// run thru surface callbacks
	for (k=0;k<b->mats.size();k++) {
		for (i=0;i<nqmatfuncs;i++) { // see if mat is in qmatfuncs
			if (!my_stricmp(qmatfuncs[i].matname,b->mats[k].name.c_str())) {
				if (qmatfuncs[i].useline2road)
					mattrans.push_back(i+1); // pos uses line2road
				else
					mattrans.push_back(-(i+1)); // neg doesn't use line2road
				break;
			}
		}
		if (i==nqmatfuncs) {
			mattrans.push_back(0); // nothing
		}
	}
	v=&qlwos.verts[qlwos.nvert];
	nf=b->faces.size();
	nv=b->verts.size();
	qlwos.totfaces+=nf;
	obj2worldn(t,&*b->verts,v,nv);
// pass 1, run thru the tris and count them up
	vector<face>::iterator of;
	for (k=0,of=b->faces.begin();k<nf;++k,++of) {
		f.vertidx[0]=of->vertidx[0]+qlwos.nvert;
		f.vertidx[1]=of->vertidx[1]+qlwos.nvert;
		f.vertidx[2]=of->vertidx[2]+qlwos.nvert;
		if (mattrans.size())
			f.fmatidx=mattrans[of->fmatidx%mattrans.size()];
		else
			f.fmatidx=0;
//		if (f.matidx>=0)
//			logger("matidx of %d found in '%s'\n",f.matidx,t->name);
//		if (f.matidx<-1 || f.matidx>=NMATFUNCS)
//			errorexit("bad matfunc");
		v0=&qlwos.verts[f.vertidx[0]];
		v1=&qlwos.verts[f.vertidx[1]];
		v2=&qlwos.verts[f.vertidx[2]];
		getgridxz(v0,&gxmin,&gzmin);
#ifdef EXTRAGRIDCHECK
	if (gxmin<0 || gxmin>=QCOLLGRIDX || gzmin<0 || gzmin>=QCOLLGRIDZ)
		errorexit("buildcollgrid: grid outa bounds #v1 %d %d",gxmin,gzmin);
#endif
		gxmax=gxmin;
		gzmax=gzmin;
		getgridxz(v1,&gx,&gz);
#ifdef EXTRAGRIDCHECK
	if (gx<0 || gx>=QCOLLGRIDX || gz<0 || gz>=QCOLLGRIDZ)
		errorexit("buildcollgrid: grid outa bounds #v2 %d %d",gx,gz);
#endif
		if (gx<gxmin)
			gxmin=gx;
		if (gz<gzmin)
			gzmin=gz;
		if (gx>gxmax)
			gxmax=gx;
		if (gz>gzmax)
			gzmax=gz;
		getgridxz(v2,&gx,&gz);
#ifdef EXTRAGRIDCHECK
	if (gx<0 || gx>=QCOLLGRIDX || gz<0 || gz>=QCOLLGRIDZ)
		errorexit("buildcollgrid: grid outa bounds #v3 %d %d",gx,gz);
#endif
		if (gx<gxmin)
			gxmin=gx;
		if (gz<gzmin)
			gzmin=gz;
		if (gx>gxmax)
			gxmax=gx;
		if (gz>gzmax)
			gzmax=gz;

		for (gz=gzmin;gz<=gzmax;gz++)
			for (gx=gxmin;gx<=gxmax;gx++) {
//				if (gx==108 && gz==116)
//					logger("hit it 1\n");
				g=&qlwos.grid[gz][gx];
				if (tri2rectxz( v0,v1,v2,&g->minv,&g->maxv)) {
#ifndef ANOTHERPASS
					if (g->nface>=g->nfacealloced) {
						g->nfacealloced+=QFACEALLOCSTEP;
						g->faceidx=(struct face *)memrealloc("line2road.c",g->faceidx,sizeof(struct face)*g->nfacealloced);
						g->colobj=(TREE **)memrealloc("line2road.c",g->colobj,sizeof(TREE *)*g->nfacealloced);
						g->facenorms=(struct pointf3 *)memrealloc("line2road.c",g->facenorms,sizeof(struct pointf3)*g->nfacealloced);
					}
// getface
					g->faceidx[g->nface]=f;
// getnormal
					n=&g->facenorms[g->nface];
					uvec.x=v1->x-v0->x;
					uvec.y=v1->y-v0->y;
					uvec.z=v1->z-v0->z;
					vvec.x=v2->x-v0->x;
					vvec.y=v2->y-v0->y;
					vvec.z=v2->z-v0->z;
					cross3d(&uvec,&vvec,n);
					normalize3d(n,n);
					g->colobj[g->nface]=t;
					qlwos.totgridfaces++;
#endif
					g->nface++;
				}
			}
	}
	qlwos.nvert+=nv;
}

#ifdef ANOTHERPASS
static void st2_buildcollgrid2(TREE *t)
{
	S32 i;
	U32 k;
	vector<S32> mattrans;
	int gx,gz,gxmin,gzmin,gxmax,gzmax;
	BODY *b;
	VEC uvec,vvec,*v0,*v1,*v2,*n;
	VEC *v;
	FACE f;
	struct onegrid *g;
	U32 nf;
	U32 nv;
	b=t->mod;
	if (!b)
		return;
//	logger("<<<<< buildcolgrid '%s'\n",t->mod->name);
//	memset(mattrans,0,sizeof(mattrans));
	for (k=0;k<b->mats.size();k++) {
		for (i=0;i<nqmatfuncs;i++) { // see if mat is in qmatfuncs
			if (!my_stricmp(qmatfuncs[i].matname,b->mats[k].name.c_str())) {
				if (qmatfuncs[i].useline2road)
					mattrans.push_back(i+1); // pos uses line2road
				else
					mattrans.push_back(-(i+1)); // neg doesn't use line2road
				break;
			}
		}
		if (i==nqmatfuncs) {
			mattrans.push_back(0); // nothing
		}
	}
	v=&qlwos.verts[qlwos.nvert];
	nf=b->faces.size();
	nv=b->verts.size();
//	qlwos.totfaces+=nf;
	obj2worldn(t,&*b->verts,v,nv);
// pass 1, run thru the tris and count them up
	vector<face>::iterator of;
	for (k=0,of=b->faces.begin();k<nf;++k,++of) {
		f.vertidx[0]=of->vertidx[0]+qlwos.nvert;
		f.vertidx[1]=of->vertidx[1]+qlwos.nvert;
		f.vertidx[2]=of->vertidx[2]+qlwos.nvert;
		if (mattrans.size())
			f.fmatidx=mattrans[of->fmatidx%mattrans.size()];
		else
			f.fmatidx=0;
//		if (f.matidx>=0)
//			logger("matidx of %d found in '%s'\n",f.matidx,t->name);
//		if (f.matidx<-1 || f.matidx>=NMATFUNCS)
//			errorexit("bad matfunc");
		v0=&qlwos.verts[f.vertidx[0]];
		v1=&qlwos.verts[f.vertidx[1]];
		v2=&qlwos.verts[f.vertidx[2]];
		getgridxz(v0,&gxmin,&gzmin);
#ifdef EXTRAGRIDCHECK
	if (gxmin<0 || gxmin>=QCOLLGRIDX || gzmin<0 || gzmin>=QCOLLGRIDZ)
		errorexit("buildcollgrid: grid outa bounds #v1 %d %d",gxmin,gzmin);
#endif
		gxmax=gxmin;
		gzmax=gzmin;
		getgridxz(v1,&gx,&gz);
#ifdef EXTRAGRIDCHECK
	if (gx<0 || gx>=QCOLLGRIDX || gz<0 || gz>=QCOLLGRIDZ)
		errorexit("buildcollgrid: grid outa bounds #v2 %d %d",gx,gz);
#endif
		if (gx<gxmin)
			gxmin=gx;
		if (gz<gzmin)
			gzmin=gz;
		if (gx>gxmax)
			gxmax=gx;
		if (gz>gzmax)
			gzmax=gz;
		getgridxz(v2,&gx,&gz);
#ifdef EXTRAGRIDCHECK
	if (gx<0 || gx>=QCOLLGRIDX || gz<0 || gz>=QCOLLGRIDZ)
		errorexit("buildcollgrid: grid outa bounds #v3 %d %d",gx,gz);
#endif
		if (gx<gxmin)
			gxmin=gx;
		if (gz<gzmin)
			gzmin=gz;
		if (gx>gxmax)
			gxmax=gx;
		if (gz>gzmax)
			gzmax=gz;

		for (gz=gzmin;gz<=gzmax;gz++)
			for (gx=gxmin;gx<=gxmax;gx++) {
//				if (gx==108 && gz==116)
//					logger("hit it 3\n");
				g=&qlwos.grid[gz][gx];
				if (tri2rectxz( v0,v1,v2,&g->minv,&g->maxv)) {
/*					if (g->nface>=g->nfacealloced) {
						g->nfacealloced+=QFACEALLOCSTEP;
						g->faceidx=(struct face *)memrealloc("line2road.c",g->faceidx,sizeof(struct face)*g->nfacealloced);
						g->colobj=(TREE **)memrealloc("line2road.c",g->colobj,sizeof(TREE *)*g->nfacealloced);
						g->facenorms=(struct pointf3 *)memrealloc("line2road.c",g->facenorms,sizeof(struct pointf3)*g->nfacealloced);
					}
// getface
*/
					g->faceidx[g->nface]=f;
// getnormal
					n=&g->facenorms[g->nface];
					uvec.x=v1->x-v0->x;
					uvec.y=v1->y-v0->y;
					uvec.z=v1->z-v0->z;
					vvec.x=v2->x-v0->x;
					vvec.y=v2->y-v0->y;
					vvec.z=v2->z-v0->z;
					cross3d(&uvec,&vvec,n);
					normalize3d(n,n);
					g->colobj[g->nface]=t;
					qlwos.totgridfaces++;
					g->nface++;
				}
			}
	}
	qlwos.nvert+=nv;
}
#endif
// free
void st2_freecollgrids()
{
	int k;
	struct onegrid *g=&qlwos.grid[0][0];
	for (k=0;k<QCOLLGRIDZ*QCOLLGRIDX;k++,g++) {
		if (g->nface) {
			memfree(g->faceidx);
			memfree(g->colobj);
			memfree(g->facenorms);
		}
	}
	if (qlwos.nvert) {
		memfree(qlwos.verts);
		memfree(qtreelist);
	}
}

void st2_setupmatfuncs(struct smatfunc *mf,int nmf)
{
	local_mf=mf;
	local_nmf=nmf;
	nqmatfuncs=0;
}

//#define MAXSURFQUE 20
//static int surfque[MAXSURFQUE];
int surfenabled;
void st2_enablesurffunc()
{
	surfenabled=1;
}

void st2_callsurffunc() // call only if line2road returns true (not true anymore)
{
	int i;
//	if (opt==SURF_FIRST && surfidx>0)
//		surfidx=1;
	for (i=0;i<nqmatfuncs;i++)
		if (qmatfuncs[i].call && qmatfuncs[i].matfunc) {
			(*qmatfuncs[i].matfunc)();
			qmatfuncs[i].call=0;
		}
	surfenabled=0;
}

static TREE *foundtri;
//int totnfaces;
TREE *st2_line2road(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	struct colltree *ct,*bct=NULL;
	int i,j,k,p,idx=-1,idx2=-1,midx=-1;
	VEC intsect;
	int nface;
	int gxmin,gxmax,gzmin,gzmax,gx,gz;
	struct onegrid *g,*bestg=0;
	float dist,bestdist=0,mbestdist=0;
	FACE *f;
	perf_start(LINE2ROAD);
#ifdef TESTNUMFACES
	totnfaces=0;
#endif
	foundtri=NULL;
	getgridxz(top,&gxmin,&gzmin);
	gxmax=gxmin;
	gzmax=gzmin;
	getgridxz(bot,&gx,&gz);
	if (gx>gxmax)
		gxmax=gx;
	else
		gxmin=gx;
	if (gz>gzmax)
		gzmax=gz;
	else
		gzmin=gz;
	if (gxmin<0) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gxmax>=QCOLLGRIDX) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gzmin<0) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gzmax>=QCOLLGRIDZ) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	for (gz=gzmin;gz<=gzmax;gz++)
		for (gx=gxmin;gx<=gxmax;gx++) {
			g=&qlwos.grid[gz][gx];
			nface=g->nface;
#ifdef TESTNUMFACES
			totnfaces+=nface;
#endif
			f=g->faceidx;
			for (i=0;i<nface;i++) {
				if (line2tri(&f[i],top,bot,&intsect,&g->facenorms[i])) {
					j=f[i].fmatidx;
					dist=dist3dsq(top,&intsect);
					if (j>=0) { // j=0: standard, j>0 material call, j<0 same material call but no collision(water)
						if (idx<0 || dist<bestdist) {
	 						idx=i;
							bestg=g;
							bestdist=dist;
							*bestintsect=intsect;
						}
					}
					if (j && surfenabled) {
						if (midx<0 || dist<mbestdist) {
							if (j<0)
								k=-j;
							else
								k=j;
							midx=k-1;
							mbestdist=dist;
						}
					}
				}
			}
		}
// scan for dynamic objects
	ct=&l2r_colltrees[0];
	for (p=0;p<l2r_ncolltrees;p++,ct++) {
		VEC *facenormbase;
		modelb *m;
		VEC topl,botl;
		if (top->x>ct->aabbmax.x && bot->x>ct->aabbmax.x)
			continue;
		if (top->y>ct->aabbmax.y && bot->y>ct->aabbmax.y)
			continue;
		if (top->z>ct->aabbmax.z && bot->z>ct->aabbmax.z)
			continue;
		if (top->x<ct->aabbmin.x && bot->x<ct->aabbmin.x)
			continue;
		if (top->y<ct->aabbmin.y && bot->y<ct->aabbmin.y)
			continue;
		if (top->z<ct->aabbmin.z && bot->z<ct->aabbmin.z)
			continue;
		xformvec(&ct->ctw2o,top,&topl);
		xformvec(&ct->ctw2o,bot,&botl);
		m=ct->t->mod;
		nface=m->faces.size();
		vecbase = & *m->verts;
		facenormbase=ct->facenorms;
		f = &m->faces[0];
		for (i=0;i<nface;i++) {
			if (line2tri(&f[i],&topl,&botl,&intsect,&facenormbase[i])) {
				dist=dist3dsq(&topl,&intsect);
				if (idx<0 || dist<bestdist) {
 					idx2=i;
					idx=0;
					bestdist=dist;
					*bestintsect=intsect;
					bct=ct;
				}
			}
		}
	}
	vecbase=qlwos.verts;
	if (bct) {
		if (bestnorm)
			xformdir(&bct->cto2w,&bct->facenorms[idx2],bestnorm);
		xformvec(&bct->cto2w,bestintsect,bestintsect);
		return bct->t; // returns master tree (not an instance)
	}
	if (idx>=0) {
		if (midx>=0 && mbestdist<=bestdist)
			qmatfuncs[midx].call=1;
		if (bestnorm)
			*bestnorm=bestg->facenorms[idx];
		foundtri=bestg->colobj[idx];
/*		if (surfenabled)// && surfidx<MAXSURFQUE) {
			f=bestg->faceidx;
			i=f[idx].matidx;
			if (i>=0)
				;//surfque[surfidx++]=i;
} */
	}
	perf_end(LINE2ROAD);
	return foundtri;
}

TREE *st2_line2roadlo(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	int i,j,idx=-1;
	VEC intsect;
	int nface;
	int gxmin,gxmax,gzmin,gzmax,gx,gz;
	struct onegrid *g,*bestg=0;
	float dist,bestdist=0;
	FACE *f;
	perf_start(LINE2ROAD);
	foundtri=NULL;
	getgridxz(top,&gxmin,&gzmin);
	gxmax=gxmin;
	gzmax=gzmin;
	getgridxz(bot,&gx,&gz);
	if (gx>gxmax)
		gxmax=gx;
	else
		gxmin=gx;
	if (gz>gzmax)
		gzmax=gz;
	else
		gzmin=gz;
	if (gxmin<0) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gxmax>=QCOLLGRIDX) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gzmin<0) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gzmax>=QCOLLGRIDZ) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	for (gz=gzmin;gz<=gzmax;gz++)
		for (gx=gxmin;gx<=gxmax;gx++) {
			g=&qlwos.grid[gz][gx];
			nface=g->nface;
			f=g->faceidx;
			for (i=0;i<nface;i++) {
				if (line2tri(&f[i],top,bot,&intsect,&g->facenorms[i])) {
					j=f[i].fmatidx;
					if (j>=0) {
						dist=dist3dsq(bot,&intsect);
						if (idx<0 || dist<bestdist) {
	 						idx=i;
							bestg=g;
							bestdist=dist;
							*bestintsect=intsect;
						}
					}
				}
			}
		}
	if (idx>=0) {
		if (bestnorm)
			*bestnorm=bestg->facenorms[idx];
		foundtri=bestg->colobj[idx];
/*		if (surfenabled)// && surfidx<MAXSURFQUE) {
			f=bestg->faceidx;
			i=f[idx].matidx;
			if (i>=0)
				;//surfque[surfidx++]=i;
} */
	}
	perf_end(LINE2ROAD);
	return foundtri;
}
/* without water and lava effect
TREE *st2_line2road(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	int i,idx=-1;
	VEC intsect;
	int nface;
	int gxmin,gxmax,gzmin,gzmax,gx,gz;
	struct onegrid *g,*bestg;
	float dist,bestdist;
	FACE *f;
	perf_start(LINE2ROAD);
	foundtri=NULL;
	getgridxz(top,&gxmin,&gzmin);
	gxmax=gxmin;
	gzmax=gzmin;
	getgridxz(bot,&gx,&gz);
	if (gx>gxmax)
		gxmax=gx;
	else
		gxmin=gx;
	if (gz>gzmax)
		gzmax=gz;
	else
		gzmin=gz;
	if (gxmin<0) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gxmax>=QCOLLGRIDX) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gzmin<0) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	if (gzmax>=QCOLLGRIDZ) {
		perf_end(LINE2ROAD);
		return NULL;
	}
	for (gz=gzmin;gz<=gzmax;gz++)
		for (gx=gxmin;gx<=gxmax;gx++) {
			g=&qlwos.grid[gz][gx];
			nface=g->nface;
			f=g->faceidx;
			for (i=0;i<nface;i++) {
				if (line2tri(&f[i],top,bot,&intsect,&g->facenorms[i])) {
					dist=dist3dsq(top,&intsect);
					if (idx<0 || dist<bestdist) {
 						idx=i;
						bestg=g;
						bestdist=dist;
						*bestintsect=intsect;
					}
				}
			}
		}
	if (idx>=0) {
		if (bestnorm)
			*bestnorm=bestg->facenorms[idx];
		foundtri=bestg->colobj[idx];
		if (surfenabled && surfidx<MAXSURFQUE) {
			f=bestg->faceidx;
			i=f[idx].matidx;
			if (i>=0)
				surfque[surfidx++]=i;
		}
	}
	perf_end(LINE2ROAD);
	return foundtri;
}
*/
//////////////// end new roadheight code /////
// now returns radians..
/////////////
//static char **colscript;
//static int numcolscript;
static script* colscript;

static void scanpass2()
{
	struct onegrid *g;
	int i,j;
	for (i=0;i<ntreelist;i++)
		st2_buildcollgrid(qtreelist[i]);
#ifdef ANOTHERPASS
	g=&qlwos.grid[0][0];
	for (j=0;j<QCOLLGRIDZ;j++)
		for (i=0;i<QCOLLGRIDX;i++) {
//			if (i==108 && j==116)
//				logger("hit it 2\n");
			if (g->nface) {
				g->faceidx=(struct face *)memalloc(sizeof(struct face)*g->nface);
				g->colobj=(TREE **)memalloc(sizeof(TREE *)*g->nface);
				g->facenorms=(struct pointf3 *)memalloc(sizeof(struct pointf3)*g->nface);
				g->nface=0;
			}
			g++;
		}
	qlwos.nvert=0;
	for (i=0;i<ntreelist;i++)
		st2_buildcollgrid2(qtreelist[i]);
#endif
}

void addbbox2collgrid(tree2* t)
{
	S32 i;
	modelb* m;
	pointf3* v;
	m=t->mod;
	if (!m)
		return;
	if (ntreelist>=ntreelistalloced) {
		ntreelistalloced+=10;
		qtreelist=(tree2 **)memrealloc(qtreelist,sizeof(struct tree *)*ntreelistalloced);
	}
	qtreelist[ntreelist++]=t;
	v=(struct pointf3 *)memalloc(sizeof(struct pointf3)*m->verts.size());
	obj2worldn(t,&*m->verts,v,m->verts.size());
	for (i=0;i<m->verts.size();++i) {
		if (v[i].x<qlwos.colbboxmin.x)
			qlwos.colbboxmin.x=v[i].x;
		if (v[i].x>qlwos.colbboxmax.x)
			qlwos.colbboxmax.x=v[i].x;
		if (v[i].y<qlwos.colbboxmin.y)
			qlwos.colbboxmin.y=v[i].y;
		if (v[i].y>qlwos.colbboxmax.y)
			qlwos.colbboxmax.y=v[i].y;
		if (v[i].z<qlwos.colbboxmin.z)
			qlwos.colbboxmin.z=v[i].z;
		if (v[i].z>qlwos.colbboxmax.z)
			qlwos.colbboxmax.z=v[i].z;
	}
	memfree(v);
	qlwos.nvert+=m->verts.size();
//	loggamestate("addbbox2collgrid highway35 '%s' nvert %d\n",t->name,m->nvert);
}

static void scanpass1(TREE *t)
{
	int i,k=0;
	const C8* s;
//	if (!(t->flags&TF_KILLMEC)) {
	if (true) {
		s=t->name.c_str();
		if (s[0]=='D' && s[1]=='_') // duptree and dup model
			s+=2;
		if (s[0]=='P' && s[1]=='_') { // duptree and split model, process children
			s+=2;
			k=1;
		}
		for (i=0;i<colscript->num();i++) {
			if (colscript->idx(i).c_str()[0]=='0' || colscript->idx(i).c_str()[0]=='1')
				break;
			if (!my_stricmp(s,colscript->idx(i).c_str())) {
				if (k) {
					list<tree2*>::iterator ji;
					for (ji=t->children.begin();ji!=t->children.end();++ji)
						addbbox2collgrid(*ji);
				} else
					addbbox2collgrid(t);
				break;
			}
		}
	}
	if (!k) {
		list<tree2*>::iterator ji;
		for (ji=t->children.begin();ji!=t->children.end();++ji)
			scanpass1(*ji);
	}
}

// call once at init
void st2_loadcollist(char *filename,TREE *root)
{
	int nvertp1;
	int i,j;//,k;
//	int *ip;
	struct onegrid *g;
// init structure
	memset(&qlwos,0,sizeof(qlwos));
//	colscript=loadscript(filename,&numcolscript);
	colscript=new script(filename);
	for (i=0;i<colscript->num();i++)
		if (colscript->idx(i).c_str()[0]=='0' || colscript->idx(i).c_str()[0]=='1')
			break;
	if (i!=colscript->num()) {
		if ((colscript->num()-i)%3)
			errorexit("collist '%s' extra not valid",filename);
		while(i!=colscript->num()) {
			if (nqmatfuncs<MAXFUNCS) {
				for (j=0;j<local_nmf;j++) {
					if (!my_stricmp(local_mf[j].matname,colscript->idx(i+2).c_str())) {
						qmatfuncs[nqmatfuncs].useline2road=colscript->idx(i).c_str()[0]-'0';
						qmatfuncs[nqmatfuncs].matfunc=local_mf[j].matfunc;
						strcpy(qmatfuncs[nqmatfuncs].matname,colscript->idx(i+1).c_str());
						nqmatfuncs++;
						break;
					}
				}
				if (j==local_nmf)
					errorexit("collist '%s' extra not a keyword '%s'\n",filename,colscript->idx(i+2).c_str());
			}
			i+=3;
		}
	}
	qtreelist=NULL;
	ntreelist=0;
	ntreelistalloced=0;
//	init bbox
	qlwos.nvert=0;
	qlwos.totfaces=qlwos.totgridfaces=0;
	qlwos.colbboxmin.x=1e20f;
	qlwos.colbboxmin.y=1e20f;
	qlwos.colbboxmin.z=1e20f;
	qlwos.colbboxmax.x=-1e20f;
	qlwos.colbboxmax.y=-1e20f;
	qlwos.colbboxmax.z=-1e20f;
// scan for names in collist, get BIG bbox and count up verts
	scanpass1(root);
	nvertp1=qlwos.nvert;
	logger("scanpass1 '%s' nvert %d\n",filename,nvertp1);
//	numcol=0;
// make bbox alittle bigger
	qlwos.mulx=(QCOLLGRIDX-.2f)/(qlwos.colbboxmax.x-qlwos.colbboxmin.x); // safety factor .1 on each side
	qlwos.offx=.1f-qlwos.mulx*qlwos.colbboxmin.x;
	qlwos.mulz=(QCOLLGRIDZ-.2f)/(qlwos.colbboxmax.z-qlwos.colbboxmin.z); // safety factor .1 on each side
	qlwos.offz=.1f-qlwos.mulz*qlwos.colbboxmin.z;
	g=&qlwos.grid[0][0];
	for (j=0;j<QCOLLGRIDZ;j++)
		for (i=0;i<QCOLLGRIDX;i++) {
			getvecfromgridxz(&g->minv,i,j);
			getvecfromgridxz(&g->maxv,i+1,j+1);
			g++;
		}
	if (!qlwos.nvert)
		errorexit("no objects found in collist");
	qlwos.verts=(struct pointf3 *)memalloc(sizeof(struct pointf3)*qlwos.nvert);
	vecbase=qlwos.verts;
	qlwos.nvert=0;
	scanpass2();
	if (nvertp1!=qlwos.nvert)
		errorexit("loadcollist nvert mismatch p1 %d, p2 %d",nvertp1,qlwos.nvert);
//	freescript(colscript,numcolscript);
	delete colscript;

	logger("total faces %d, total grid faces %d\n",qlwos.totfaces,qlwos.totgridfaces);
	for (j=0;j<QCOLLGRIDZ;j++) {
		for (i=0;i<QCOLLGRIDX;i++)
			logger("%5d",qlwos.grid[j][i].nface);
		logger("\n");
	}

/*	ip=(int*)&qlwos.colbboxmin;
	loggamestate("colbboxmin %08x %08x %08x ",
		ip[0],ip[1],ip[2]);
	ip=(int*)&qlwos.colbboxmax;
	loggamestate("colbboxmax %08x %08x %08x\n",
		ip[0],ip[1],ip[2]);

	ip=(int *)&qlwos.offx;
	loggamestate("offxz %08x ",*ip);
	ip=(int *)&qlwos.offz;
	loggamestate("%08x\n",*ip);
	ip=(int *)&qlwos.mulx;
	loggamestate("mulxz %08x ",*ip);
	ip=(int *)&qlwos.mulz;
	loggamestate("%08x\n",*ip);
	for (j=56;j<64;j++)
//	for (j=0;j<QCOLLGRIDZ;j++)
		for (i=0;i<QCOLLGRIDX;i++) {
			g=&qlwos.grid[j][i];
			loggamestate("%3d %3d ------------\n",i,j);
			ip=(int*)&g->minv;
			loggamestate("nface %d\n",g->nface);
			loggamestate("minv %08x %08x %08x ",
				ip[0],ip[1],ip[2]);
			ip=(int*)&g->maxv;
			loggamestate("maxv %08x %08x %08x\n",
				ip[0],ip[1],ip[2]);
			for (k=0;k<g->nface;k++) {
				ip=(int*)&g->facenorms[k];
				loggamestate("   face %4d: facenorm %08x %08x %08x\n",k,
					ip[0],ip[1],ip[2]);
				ip=(int*)qlwos.verts;
				loggamestate("             v0 %08x %08x %08x ",
					ip[4*g->faceidx[k].vertidx[0]],ip[4*g->faceidx[k].vertidx[0]+1],ip[4*g->faceidx[k].vertidx[0]+2]);
				loggamestate("             v1 %08x %08x %08x ",
					ip[4*g->faceidx[k].vertidx[1]],ip[4*g->faceidx[k].vertidx[1]+1],ip[4*g->faceidx[k].vertidx[1]+2]);
				loggamestate("             v2 %08x %08x %08x mi %d\n",
					ip[4*g->faceidx[k].vertidx[2]],ip[4*g->faceidx[k].vertidx[2]+1],ip[4*g->faceidx[k].vertidx[2]+2],g->faceidx[k].matidx);
			}
		} */
}
//void initfwgs()

#if 0
static int getsum(unsigned int *data,int ndata)
{
	int i,cs=0;
	for (i=0;i<ndata;i++)
		cs+=data[i];
	if (cs==0)
		cs=1;
	if (cs==-1)
		cs=1;
	return cs;
}

int gettrackchecksum()
{
	struct onegrid *og;
	int i,j;
	int cs=0;
//	cs+=getsum((int *)qlwos.verts,sizeof(qlwos.verts)/sizeof(int));
	cs+=qlwos.nvert;
//	cs+=getsum((int*)&qlwos.offx,1);
//	cs+=getsum((int*)&qlwos.offz,1);
//	cs+=getsum((int*)&qlwos.mulx,1);
//	cs+=getsum((int*)&qlwos.mulz,1);
	for (j=0;j<QCOLLGRIDZ;j++)
		for (i=0;i<QCOLLGRIDX;i++) {
			og=&qlwos.grid[j][i];
//			cs+=og->nface;
//			cs+=getsum((int*)&og->facenorms,og->nface*sizeof(VEC)/sizeof(int));
//			cs+=getsum((int*)&og->minv.x,1);
//			cs+=getsum((int*)&og->minv.z,1);
//			cs+=getsum((int*)&og->maxv.x,1);
//			cs+=getsum((int*)&og->maxv.z,1);
//			cs+=getsum((int*)&og->faceidx,og->nface*sizeof(FACE)/sizeof(int));
		}
	return cs;
}
#endif
#endif

void initcolltree()
{
	l2r_ncolltrees=0;
}

void addcolltree(int uid,TREE* t,VEC** facenormsplace,VEC* p,VEC* r,VEC* s)
{
	int i;
	struct colltree* ct;
	VEC v;
	modelb* m;
	logger("in addcolltree %08x %s\n",uid,t->name.c_str());
	if (l2r_ncolltrees>=MAXCOLLTREES)
		errorexit("addcolltree too many %s",t->name.c_str());
//		return;
	m=t->mod;
	if (!m)
		errorexit("addcolltree %s no model",t->name.c_str());
	for (i=0;i<l2r_ncolltrees;i++)
		if (l2r_colltrees[i].uid==uid)
			errorexit("addcolltree id already used %s",t->name.c_str());
	if (!*facenormsplace) {
		int nface;
		VEC *norms;
		logger("building facenorms for object '%s'\n",t->name.c_str());
		*facenormsplace=norms=(VEC *)memalloc(sizeof(VEC)*t->mod->faces.size());
		nface=m->faces.size();
		for (i=0;i<nface;i++) {
			VEC *v0,*v1,*v2,uvec,vvec,*n;
			n=&norms[i];
			v0=&m->verts[m->faces[i].vertidx[0]];
			v1=&m->verts[m->faces[i].vertidx[1]];
			v2=&m->verts[m->faces[i].vertidx[2]];
			uvec.x=v1->x-v0->x;
			uvec.y=v1->y-v0->y;
			uvec.z=v1->z-v0->z;
			vvec.x=v2->x-v0->x;
			vvec.y=v2->y-v0->y;
			vvec.z=v2->z-v0->z;
			cross3d(&uvec,&vvec,n);
			normalize3d(n,n);
		}
	} else
		logger("facenorms for object '%s' found\n",t->name.c_str());
	ct=&l2r_colltrees[l2r_ncolltrees];
	quat2xformnotrans(r,&ct->cto2w);
	ct->cto2w.e[3][0]=p->x;
	ct->cto2w.e[3][1]=p->y;
	ct->cto2w.e[3][2]=p->z;
	premulscale3d(s,&ct->cto2w,&ct->cto2w);
	ct->cto2w.e[0][3]=0;
	ct->cto2w.e[1][3]=0;
	ct->cto2w.e[2][3]=0;
	ct->cto2w.e[3][3]=1;
	inversemat3d(&ct->cto2w,&ct->ctw2o);
// build aabb
	setVEC(&ct->aabbmax,-1e20f,-1e20f,-1e20f);
	setVEC(&ct->aabbmin,1e20f,1e20f,1e20f);
	for (i=0;i<8;i++) {
		v.x=(i&1)?m->boxmax.x:m->boxmin.x;
		v.y=(i&2)?m->boxmax.y:m->boxmin.y;
		v.z=(i&4)?m->boxmax.z:m->boxmin.z;
		xformvec(&ct->cto2w,&v,&v);
		if (v.x>ct->aabbmax.x)
			ct->aabbmax.x=v.x;
		if (v.y>ct->aabbmax.y)
			ct->aabbmax.y=v.y;
		if (v.z>ct->aabbmax.z)
			ct->aabbmax.z=v.z;
		if (v.x<ct->aabbmin.x)
			ct->aabbmin.x=v.x;
		if (v.y<ct->aabbmin.y)
			ct->aabbmin.y=v.y;
		if (v.z<ct->aabbmin.z)
			ct->aabbmin.z=v.z;
	}
	logger("addcolltree '%s' aabb is set to %f %f %f and %f %f %f\n",t->name.c_str(),
		ct->aabbmin.x,ct->aabbmin.y,ct->aabbmin.z,ct->aabbmax.x,ct->aabbmax.y,ct->aabbmax.z);
	ct->uid=uid;
	ct->t=t;
	ct->facenorms=*facenormsplace;
	l2r_ncolltrees++;
}

void remcolltree(int uid)
{
	int i;
	struct colltree *ct,*ct2;
	logger("in remcolltree %08x\n",uid);
	ct=&l2r_colltrees[0];
	for (i=0;i<l2r_ncolltrees;i++,ct++)
		if (uid==ct->uid)
			break;
	if (i==l2r_ncolltrees)
		errorexit("remcolltree can't find uid %08x to free\n",uid);
	l2r_ncolltrees--;
	ct2=&l2r_colltrees[l2r_ncolltrees];
	*ct=*ct2;
}

} // end namespace hw35_line2road
