/*#include <string.h>

#include <engine7cpp.h>

#include "facevertscpp.h"
#include "polylistcpp.h"
#include "usefulcpp.h"
#include "meshtestcpp.h" */

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "n_deflatecpp.h"
#include "n_usefulcpp.h" 
#include "n_constructorcpp.h"
#include "n_facevertscpp.h"
#include "n_polylistcpp.h"
#include "n_meshtestcpp.h"
#include "n_carenalobbycpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "n_newconstructorcpp.h"

#define MAXPOLYSIZE 10000 // how big lines are for inside outside test (careful)
#define MAXINTLIST 600
#define MAXIL2 5 // when error hits, increase to 12 (one time test)
#define PERT .00002f // cheap way to handle degenerates
#define PERTANGSTEP TWOPI/5	// in radians


// for bool
struct intlist {
	pointf3 v;
	uv iuv;
	float crs;
	float t;
	int idx;
	int visited;
	int id;
};

// for special
struct intlist2 {
	pointf3 v;
	uv uvs;
	float t;
};

/*
static int numalloc(int n)
{
	return (n+ALLOCSIZE-1)&~(ALLOCSIZE-1);
}

static polylist* newpolylist()
{
//	return (polylist *)memzalloc(sizeof(polylist));
	polylist* r=new polylist;
//	r->npolys=0;
	return r;
}
*/
void freepolylist(polylist *plb)
{
/*	int i;
	poly *p;
	for (i=0,p=plb->polys;i<plb->npolys;i++,p++) {
		if (p->verts)
//			memfree(p->verts);
			delete[] p->verts;
		if (p->uvs)
//			memfree(p->uvs);
			delete[] p->uvs;
	}
	if (plb->polys)
		delete[]  plb->polys;
//		memfree(plb->polys);
//	memfree(plb); */
	delete plb;
}

// add another vert to a poly
static void growverts(poly *p,const pointf3 *v,const uv *uvs)
{
/*	int ov=p->nverts;
	if (ov<0)
		errorexit("ov < 0");
	int nv=ov+1;
	int oa=numalloc(ov);
	int na=numalloc(nv);
	if (oa!=na) {
		p->verts=(pointf3 *)memrealloc(p->verts,na*sizeof(VEC));
		if (uvs)
			p->uvs=(uv *)memrealloc(p->uvs,na*sizeof(uv));
	}
	p->verts[ov]=*v;
	if (uvs)
		p->uvs[ov]=*uvs;
	p->nverts=nv; */
	p->verts.push_back(*v);
	if (uvs)
		p->uvs.push_back(*uvs);
}

// add another poly to a polylist
static poly *growpolys(polylist *pl)
{
	poly apoly=poly();
//	poly apoly;
//	poly apoly2(apoly);
/*	int op=pl->npolys;
	int np=op+1;
	int oa=numalloc(op);
	int na=numalloc(np);
	if (oa!=na)
		pl->polys=(poly *)memrealloc(pl->polys,na*sizeof(poly));
	memset(&pl->polys[op],0,sizeof(poly));
	pl->npolys=np;
	return &pl->polys[op]; */
	S32 ns=pl->polys.size()+1;
	pl->polys.resize(ns);
	return &pl->polys[pl->polys.size()-1];
}

void setm1tobackgnd(polylist *c)
{
	int i,j,np=c->polys.size();
	for (i=0;i<np;i++) {
		int nv=c->polys[i].verts.size();
		for (j=0;j<nv;j++) {
			if (c->polys[i].verts[j].vmatidx) {
//				c->polys[i].verts[j].y=0;
				getbacky(&c->polys[i].verts[j]);
			}
		}
	}
}

void addlinearuvspolylist(polylist *pl,float offx,float offz,float x2u,float z2v)
{
//	poly *p;
	vector<poly>::iterator p;
//	pointf3 *v;
//	uv *uvs;
//	int /*i,j,*/npolys=pl->polys.size();
//	for (p=pl->polys.begin(),j=0;j<npolys;j++,p++) {
	for (p=pl->polys.begin();p!=pl->polys.end();++p) {
//		if (p->uvs)
//			memfree(p->uvs);
//		p->uvs=(uv *)memalloc(sizeof(uv)*p->nverts);
		p->uvs.resize(p->verts.size());
		vector<pointf3>::iterator v;
		vector<uv>::iterator u;
//		for (v=p->verts,uvs=p->uvs,i=0;i<p->nverts;i++,v++,uvs++) {
		for (v=p->verts.begin(),u=p->uvs.begin();v!=p->verts.end();++v,++u) {
			u->u=x2u*(v->x-offx);
			u->v=z2v*(v->z-offz);
		}
	}
	pl->hasuvs=true;
}

void zeroypolylist(polylist *c)
{
//	int i,j;
//	poly *p;
//	pointf3 *v;
	vector<poly>::iterator p;
	vector<pointf3>::iterator v;
//	for (i=0,p=c->polys;i<c->npolys;i++,p++)
//		for (j=0,v=p->verts;j<p->nverts;j++,v++)
	for (p=c->polys.begin();p!=c->polys.end();++p)
		for (v=p->verts.begin();v!=p->verts.end();++v)
			v->y=0;
}

void rotpolylist(polylist *c,float rot)
{
//	int i,j;
//	poly *p;
//	pointf3 *v;
	vector<poly>::iterator p;
	vector<pointf3>::iterator v;
	float t;
//	for (i=0,p=c->polys;i<c->npolys;i++,p++) {
//		for (j=0,v=p->verts;j<p->nverts;j++,v++) {
	for (p=c->polys.begin();p!=c->polys.end();++p) {
		for (v=p->verts.begin();v!=p->verts.end();++v) {
			t=v->x*cosf(rot)+v->z*sinf(rot);
			v->z=-v->x*sinf(rot)+v->z*cosf(rot);
			v->x=t;
		}
	}
}

void offsetpolylist(polylist *c,pointf3 *off)
{
//	int i,j;
//	poly *p;
//	pointf3 *v;
//	for (i=0,p=c->polys;i<c->npolys;i++,p++) {
//		for (j=0,v=p->verts;j<p->nverts;j++,v++) {
	vector<poly>::iterator p;
	vector<pointf3>::iterator v;
	for (p=c->polys.begin();p!=c->polys.end();++p) {
		for (v=p->verts.begin();v!=p->verts.end();++v) {
			v->x+=off->x;
			v->z+=off->z;
		}
	}
}

void backypolylist(polylist *c)
{
//	int i,j;
//	poly *p;
//	pointf3 *v;
	vector<poly>::iterator p;
	vector<pointf3>::iterator v;
//	for (i=0,p=c->polys;i<c->npolys;i++,p++)
//		for (j=0,v=p->verts;j<p->nverts;j++,v++)
	for (p=c->polys.begin();p!=c->polys.end();++p)
		for (v=p->verts.begin();v!=p->verts.end();++v)
			getbacky(&v[0]);
}

polylist *copypolylist(polylist *src)
{
/*	polylist *dst=newpolylist();
	poly *psrc,*pdst;
	int i,j;
	for (j=0,psrc=src->polys;j<src->npolys;j++,psrc++) {
		pdst=growpolys(dst);
		pdst->matid=psrc->matid;
		pdst->ptype=psrc->ptype;
		for (i=0;i<psrc->nverts;i++)
			if (src->hasuvs)
				growverts(pdst,&psrc->verts[i],&psrc->uvs[i]);
			else
				growverts(pdst,&psrc->verts[i],0);
	}
	return dst; */
	polylist* dst=new polylist;
	dst->polys=src->polys;
	dst->hasuvs=src->hasuvs;
	return dst;
}

static void copypoly(const poly *src,poly *dst)
{
/*	dst->matid=src->matid;
	dst->nverts=src->nverts;
	dst->ptype=src->ptype;
	dst->verts=(pointf3 *)memalloc(sizeof(VEC)*src->nverts);;
	memcpy(dst->verts,src->verts,sizeof(VEC)*src->nverts);
	if (src->uvs) {
		dst->uvs=(uv *)memalloc(sizeof(uv)*src->nverts);;
		memcpy(dst->uvs,src->uvs,sizeof(uv)*src->nverts);
	} else
		dst->uvs=0; */
	dst->matid=src->matid;
	dst->ptype=src->ptype;
	dst->verts=src->verts;
	dst->uvs=src->uvs;
}

/*
static void freepoly(poly *p)
{
	if (p->uvs)
		memfree(p->uvs);
	if (p->verts)
		memfree(p->verts);
}
*/
polylist *convertfaceverts2polylist(facevert *fv)//pointf3 *v,uv *uvs,int nv,face *f,int nf,polylist *c)
{
//	polylist *c=newpolylist();
	polylist* c=new polylist;
	int i,j;
	poly *p;
	if (fv->uvs.size())
		c->hasuvs=true;
//	face *f=fv->faces;
	vector<face>::iterator f=fv->faces.begin();
	S32 nface=fv->faces.size();
	for (i=0;i<nface;i++,f++) {
		p=growpolys(c);
		p->ptype=PT_POLY;
		p->matid=f->fmatidx;
		for (j=0;j<3;j++) {
			int vi=f->vertidx[j];
			if (c->hasuvs)
				growverts(p,&fv->verts[vi],&fv->uvs[vi]);
			else
				growverts(p,&fv->verts[vi],0);
		}
	}
	return c;
}

// makes an outline out of a mesh, by counting edges, only faces with 1 edge survive..
polylist *convertfaceverts2polylisto(facevert *fv)
//void convertfaceverts2polylisto(pointf3 *v,uv *uvs,int nv,face *f,int nf,polylist *c)
{
	static int pw[3]={1,2,0};
	int i,j,k,jp;
	int npo;
//	int nv=fv->nvert;
	int nv=fv->verts.size();
//	int *ec=(int *)memzalloc(sizeof(int)*nv*nv);
	int* ec=new int[nv*nv];
	fill(ec,ec+nv*nv,0);
//	face *f=fv->faces;
	vector<face>::iterator f=fv->faces.begin();
	S32 nface=fv->faces.size();
	for (i=0;i<nface;i++,f++) {
		for (j=0;j<3;j++) {
			int v1=f->vertidx[j];
			jp=pw[j];
			int v2=f->vertidx[jp];
			ec[nv*v1+v2]++;
		}
	}
	npo=3*nface; // start out with this many edges..
	for (j=0;j<nv;j++)
		for (i=0;i<nv;i++)
			if (ec[nv*j+i] && ec[nv*i+j]) {
				ec[nv*j+i]=ec[nv*i+j]=0;
				npo-=2;
			}
//	logger("%d edges\n",npo);
//	for (j=0;j<nv;j++)
//		for (i=0;i<nv;i++)
//			if (ec[nv*j+i])
//				logger("ec[%2d][%2d]=%2d\n",j,i,ec[nv*j+i]);
	for (i=0;i<nv;i++)
		for (j=0;j<nv;j++)
			if (ec[nv*i+j])
				goto out;
out:
	if (i==nv || j==nv)
		errorexit("can't find edge");
//	polylist *c=newpolylist();
	polylist* c=new polylist;
	if (fv->uvs.size())
		c->hasuvs=true;
	poly *pc=growpolys(c);
	for (k=0;k<npo;k++) {
		if (c->hasuvs)
			growverts(pc,&fv->verts[i],&fv->uvs[i]);
		else
			growverts(pc,&fv->verts[i],0);
		i=j;
		for (j=0;j<nv;j++)
			if (ec[nv*i+j])
				break;
		if (j==nv)
			errorexit("can't find vert");
	}
//	for (i=0;i<npo;i++)
//		logger("edge %2d: %f %f %f\n",i,c->polys[0].verts[i].x,c->polys[0].verts[i].y,c->polys[0].verts[i].z);
//	memfree(ec);
	delete[] ec;
	return c;
}

/*static void checkmat(polylist *c,char *phrase)
{
	int i;
	for (i=0;i<c->npolys;i++)
		if (c->polys[i].matid!=0)
			logger("'%s' poly %d has matid %d\n",phrase,i,c->polys[i].matid);
}
*/
/*static void getbacky(pointf3 *v)
{
	v->y=(v->x-15)*(v->z-15)/15/15*6;
//	v->y=yheight;
}
*/
// build models and trees from a polylist and tex data
tree2* buildapolylist(polylist *p,char *modelname,char *texname,char *texname2)
{
	int i,j;
	int bi=-1,bj=-1;
	float bd=0,d,x,z;
	tree2* bmt=0;
//	int *nv;
//	pointf3 **v;
//	uv **ppuvs;
//	int *pmatid;
	S32 npolys=p->polys.size();
	if (!npolys)
		return 0;
	if (npolys==2 && p->polys[0].ptype==PT_POLY && p->polys[1].ptype==PT_HOLE) { // special 1 hole
//		if (!p->polys[0].nverts || !p->polys[1].nverts) // just in case..
		S32 p0nv=p->polys[0].verts.size();
		S32 p1nv=p->polys[1].verts.size();
		if (!p0nv || !p1nv) // just in case..
			return 0;
//		for (j=0;j<p->polys[1].nverts;j++) { // find closest 2 points.. inner
		for (j=0;j<p1nv;j++) { // find closest 2 points.. inner
			pointf3 nm,np;	// get unnormalized normals
//			int jp1=(j+1)%p->polys[1].nverts;
			int jp1=(j+1)%p1nv;
//			int jm1=(p->polys[1].nverts+j-1)%p->polys[1].nverts;
			int jm1=(p1nv+j-1)%p1nv;
			np.x=-(p->polys[1].verts[jp1].z-p->polys[1].verts[j].z);
			np.z=p->polys[1].verts[jp1].x-p->polys[1].verts[j].x;
			nm.x=-(p->polys[1].verts[j].z-p->polys[1].verts[jm1].z);
			nm.z=p->polys[1].verts[j].x-p->polys[1].verts[jm1].x;
			float crs=nm.x*np.z-nm.z*np.x; // cross of those normals
//			for (i=0;i<p->polys[0].nverts;i++) { // outer
			for (i=0;i<p0nv;i++) { // outer
				x=p->polys[1].verts[j].x-p->polys[0].verts[i].x;
				z=p->polys[1].verts[j].z-p->polys[0].verts[i].z;
				d=x*x+z*z;
				if (bi==-1 || d<bd) {
					int doit=0;
					float dotm,dotp;
					dotp=np.x*x+np.z*z;
					dotm=nm.x*x+nm.z*z;
					if (crs<0) { // check to see if closest points are connected by solid
						if (dotp>0 && dotm>0)
							doit=1;
					} else {
						if (dotp>0 || dotm>0)
							doit=1;
					}
					if (doit) {
						bd=d;
						bi=i;
						bj=j;
					}
				}
			}
		}
//		int ncv=2+p->polys[0].nverts+p->polys[1].nverts;
		int ncv=2+p0nv+p1nv;
//		pointf3 *cv=(pointf3 *)memalloc(sizeof(VEC)*ncv);
		pointf3* cv=new pointf3[ncv];
		uv *cuvs=0;
		if (p->hasuvs)
//			cuvs=(uv *)memalloc(sizeof(uv)*ncv);
			cuvs=new uv[ncv];
//		logger("special hole, nv0 = %d, nv1 = %d, bi = %d, bj = %d\n",
//			p->polys[0].nverts,p->polys[1].nverts,bi,bj);
//		for (i=0;i<=p->polys[0].nverts;i++) {
		for (i=0;i<=p0nv;i++) {
//			logger("copying from 0:%d to %d\n",(bi+i)%p->polys[0].nverts,i);
//			cv[i]=p->polys[0].verts[(bi+i)%p->polys[0].nverts];
			cv[i]=p->polys[0].verts[(bi+i)%p0nv];
			if (p->hasuvs)
//				cuvs[i]=p->polys[0].uvs[(bi+i)%p->polys[0].nverts];
				cuvs[i]=p->polys[0].uvs[(bi+i)%p0nv];
		}
//		for (i=0;i<=p->polys[1].nverts;i++) {
		for (i=0;i<=p1nv;i++) {
//			logger("copying from 1:%d to %d\n",
//				(bj+i)%p->polys[1].nverts,i+p->polys[0].nverts+1);
//			cv[i+p->polys[0].nverts+1]=p->polys[1].verts[(bj+i)%p->polys[1].nverts];
			cv[i+p0nv+1]=p->polys[1].verts[(bj+i)%p1nv];
			if (p->hasuvs)
//				cuvs[i+p->polys[0].nverts+1]=p->polys[1].uvs[(bj+i)%p->polys[1].nverts];
				cuvs[i+p0nv+1]=p->polys[1].uvs[(bj+i)%p1nv];
		}
//		for (i=0;i<ncv;i++)
//			logger("%2d: %f %f %f\n",i,cv[i].x,cv[i].y,cv[i].z);
//		buildmtstart(modelname);
//		if (buildmt(texname,cv,cuvs,ncv))
//			bmt=buildmtend();
		bmt=buildmt(modelname,texname,cv,cuvs,ncv);
//		memfree(cv);
		delete[] cv;
		if (p->hasuvs)
//			memfree(cuvs);
			delete[] cuvs;
		return bmt;
	}
/*	v=(pointf3 **)memalloc(sizeof(pointf3 *)*p->npolys);
	nv=(int *)memalloc(sizeof(int)*p->npolys);
	pmatid=(int *)memalloc(sizeof(int)*p->npolys);
	if (p->hasuvs)
		ppuvs=(uv **)memalloc(sizeof(uv *)*p->npolys);
	else
		ppuvs=0;
	for (i=0;i<p->npolys;i++) {
		S32 pnv;
		if (!p->polys[i].nverts)
			break;
		v[i]=p->polys[i].verts;
		nv[i]=p->polys[i].nverts;
		pmatid[i]=p->polys[i].matid;
		if (p->hasuvs)
			ppuvs[i]=p->polys[i].uvs;
	} */
//	if (i==p->npolys) { // every poly has verts
	//	bmt=buildmt(modelname,texname,p->polys[0].verts,p->polys[0].nverts);
//		buildmtstart(modelname);
//		if (buildmts(texname,v,ppuvs,pmatid,nv,p->npolys,texname2))
//			bmt=buildmtend();
//		bmt=buildmts(modelname,texname,cv,cuvs,pmatid,ncv,texname2);
		bmt=buildmts(modelname,texname,*p,texname2);
//	}
/*	memfree(v);
	memfree(nv);
	memfree(pmatid); */
//	if (p->hasuvs)
//		memfree(ppuvs);
	return bmt;
//	return new tree2("buildapolylist");
}

static void sortintlist2(struct intlist2 *il,int nil)
{
	int j=0;
	struct intlist2 til;
	while(j<nil-1) {
		if (il[j].t>il[j+1].t) {
			til=il[j];
			il[j]=il[j+1];
			il[j+1]=til;
			j--;
			if (j<0)
				j=0;
		} else
			j++;
	}
}

polylist *boolabintsectm1(const polylist* pla,const vector<pointf3>& ev,const vector<edge>& e)//,int nedge); // special
//polylist *boolabintsectm1(polylist *pla,pointf3 *ev,edge *e);//,int ne)
//void boolabintsectm1(polylist *pla,pointf3 *ev,edge *e,int ne,polylist *plc)
{
	S32 ne=e.size();
	intlist2 il2[MAXIL2];
//	struct poly *pa,*pc;
	struct poly* pc;
	const pointf3 *v;
	const uv *uvs=0;
//	polylist *plc=newpolylist();
	polylist* plc=new polylist;
//	int i;
	int vi,vi2,vi3,k;
	const pointf3 *ve1,*ve2;
	const pointf3 *tr1,*tr2,*tr3;
	const uv *uvs3;
//	logger("special...\n");
	plc->hasuvs=pla->hasuvs;
	vector<poly>::const_iterator pa;
	for (pa=pla->polys.begin();pa!=pla->polys.end();++pa) {
//	for (pa=&pla->polys[0],i=0;i<pla->npolys;i++,pa++) {
//		if (pa->nverts!=3)
		if (pa->verts.size()!=3)
			errorexit("non tri in special");
//		pc->matid=pa->matid;
		vi=-1; // find edge with 2 mi's set
		v=&pa->verts[0];
		if (pla->hasuvs)
			uvs=&pa->uvs[0];
		if (v[0].vmatidx && v[1].vmatidx) {
			vi=0;vi2=1,vi3=2;
		} else if (v[1].vmatidx && v[2].vmatidx) {
			vi=1;vi2=2,vi3=0;
		} else if (v[2].vmatidx && v[0].vmatidx) {
			vi=2;vi2=0,vi3=1;
		}
		if (vi>=0) { // found a valid track edge..
			tr1=&v[vi];
			tr2=&v[vi2];
			tr3=&v[vi3];
			uvs3=&uvs[vi3];
			il2[0].t=0;
			il2[0].v=v[vi];
			il2[1].t=1;
			il2[1].v=v[vi2];
			if (pla->hasuvs) {
				il2[0].uvs=uvs[vi];
				il2[1].uvs=uvs[vi2];
			}
			int nil=2;
			for (k=0;k<ne;k++) {
				ve1=&ev[e[k].ei[0]];
				ve2=&ev[e[k].ei[1]];
				pointf3 intsect;
				float t;
				if (intersectline2d(tr1,tr2,ve1,ve2,&intsect,&t,0)>0) {
					intsect.vmatidx=1;
					if (nil>=MAXIL2)
						errorexit("max intsect in special");
//					logger("intersection found for face %d, edge (%d,%d)\n",i,vi,vi2);
					il2[nil].t=t;
					il2[nil].v=intsect;
					if (pla->hasuvs) {
						il2[nil].uvs.u=(1-t)*uvs[vi].u+t*uvs[vi2].u;
						il2[nil].uvs.v=(1-t)*uvs[vi].v+t*uvs[vi2].v;
					}
					nil++;
				}
			}
			if (nil>2)
				sortintlist2(il2,nil);
			for (k=0;k<nil-1;k++) {
				pc=growpolys(plc);
				pc->matid=pa->matid;
				if (pla->hasuvs) {
					growverts(pc,&il2[k].v,&il2[k].uvs);
					growverts(pc,&il2[k+1].v,&il2[k+1].uvs);
					growverts(pc,tr3,uvs3);
				} else {
					growverts(pc,&il2[k].v,0);
					growverts(pc,&il2[k+1].v,0);
					growverts(pc,tr3,0);
				}
			}
		} else { // pass tri thru
//			pc=growpolys(plc);
//			pc->matid=pa->matid;
//			pc->nverts=3;
//			memcpy(pc->verts,v,sizeof(VEC)*3);
//			if (pla->hasuvs)
//				memcpy(pc->uvs,uvs,sizeof(uv)*3);
			pc=growpolys(plc);
			pc->matid=pa->matid;
			if (pla->hasuvs) {
				growverts(pc,v,uvs);
				growverts(pc,v+1,uvs+1);
				growverts(pc,v+2,uvs+2);
			} else {
				growverts(pc,v,0);
				growverts(pc,v+1,0);
				growverts(pc,v+2,0);
			}
		
		}
	}
//	return new polylist;
	return plc;
}

int inside(poly *pal,pointf3 *p)
{
	U32 i,ip,j;
	int oddcount=0,evencount=0;
	pointf3 pe;
	static pointf3 dirs[4]={{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},};
	for (j=0;j<4;j++) {
		int cnt=0;
		pe.x=dirs[j].x*MAXPOLYSIZE+p->x;
		pe.z=dirs[j].z*MAXPOLYSIZE+p->z;
		for (i=0;i<pal->verts.size();i++) {
			ip=i+1;
			if (ip==pal->verts.size())
				ip=0;
			if (intersectline2d(p,&pe,&pal->verts[i],&pal->verts[ip],NULL,NULL,NULL)>0)
				cnt++;
		}
		if (cnt&1)
			oddcount++;
		else
			evencount++;
	}
	if (oddcount>=evencount)
		return 1;
	return 0;
}

static void convertxz2uv(mat3 *p2uv,pointf3 *in,uv *out)
{
	pointf3 in2;
	pointf3 out2;
	in2.x=in->x;
	in2.y=1;
	in2.z=in->z;
	mulmatvec3(p2uv,&in2,&out2);
	out->u=out2.x;
	in->y=out2.y;
//	in->y=1;
	out->v=out2.z;
}

static void convertxz2justy(mat3 *p2uv,pointf3 *in)
{
	pointf3 in2;
	pointf3 out2;
	in2.x=in->x;
	in2.y=1;
	in2.z=in->z;
	mulmatvec3(p2uv,&in2,&out2);
	in->y=out2.y;
}

static void makexz2uyv(pointf3 *p0,pointf3 *p1,pointf3 *p2,uv *uv0,uv *uv1,uv *uv2,mat3 *p2uv)
{
	mat3 b2p,b2uv,p2b;
	identmat3(&b2p);
	identmat3(&b2uv);
	b2p.e[0][0]=p1->x-p0->x; // b.m to p.x
	b2p.e[0][1]=0;				// b.m to p.y
	b2p.e[0][2]=p1->z-p0->z; // b.m to p.z
	b2p.e[1][0]=p0->x;				// b.1 to 1 
	b2p.e[1][1]=1;				// b.1 to 1
	b2p.e[1][2]=p0->z;				// b.1 to 1
	b2p.e[2][0]=p2->x-p0->x;		 // b.n to p.x
	b2p.e[2][1]=0;					// b.n to p.y
	b2p.e[2][2]=p2->z-p0->z;		 // b.n to p.z
	b2uv.e[0][0]=uv1->u-uv0->u;		// b.m to u
	b2uv.e[0][1]=p1->y-p0->y;		// b.m to  py
	b2uv.e[0][2]=uv1->v-uv0->v;		// b.m to v
	b2uv.e[1][0]=uv0->u;		// b.1 to u
	b2uv.e[1][1]=p0->y;		// b.1 to  py
	b2uv.e[1][2]=uv0->v;		// b.1 to v
	b2uv.e[2][0]=uv2->u-uv0->u;			// b.n to u
	b2uv.e[2][1]=p2->y-p0->y;			// b.n to  py
	b2uv.e[2][2]=uv2->v-uv0->v;			// b.n to v
	inversemat3(&b2p,&p2b);
	mulmat3(&p2b,&b2uv,p2uv);
}

static void makexz2y(pointf3 *p0,pointf3 *p1,pointf3 *p2,mat3 *p2uv)
{
	mat3 b2p,b2uv,p2b;
	identmat3(&b2p);
	identmat3(&b2uv);
	b2p.e[0][0]=p1->x-p0->x; // b.m to p.x
	b2p.e[0][1]=0;				// b.m to p.y
	b2p.e[0][2]=p1->z-p0->z; // b.m to p.z
	b2p.e[1][0]=p0->x;				// b.1 to 1 
	b2p.e[1][1]=1;				// b.1 to 1
	b2p.e[1][2]=p0->z;				// b.1 to 1
	b2p.e[2][0]=p2->x-p0->x;		 // b.n to p.x
	b2p.e[2][1]=0;					// b.n to p.y
	b2p.e[2][2]=p2->z-p0->z;		 // b.n to p.z
	b2uv.e[0][0]=0;		// b.m to u
	b2uv.e[0][1]=p1->y-p0->y;		// b.m to  py
	b2uv.e[0][2]=0;		// b.m to v
	b2uv.e[1][0]=0;		// b.1 to u
	b2uv.e[1][1]=p0->y;		// b.1 to  py
	b2uv.e[1][2]=0;		// b.1 to v
	b2uv.e[2][0]=0;			// b.n to u
	b2uv.e[2][1]=p2->y-p0->y;			// b.n to  py
	b2uv.e[2][2]=0;			// b.n to v
	inversemat3(&b2p,&p2b);
	mulmat3(&p2b,&b2uv,p2uv);
}

static void sortintlist(struct intlist *il,int nil)
{
	int j=0;
	struct intlist til;
	while(j<nil-1) {
		if (il[j].idx>il[j+1].idx || il[j].idx==il[j+1].idx && il[j].t>il[j+1].t) {
			til=il[j];
			il[j]=il[j+1];
			il[j+1]=til;
			j--;
			if (j<0)
				j=0;
		} else
			j++;
	}
}

static void sortintlistr(struct intlist *il,int nil)
{
	int j=0;
	struct intlist til;
	while(j<nil-1) {
		if (il[j].idx<il[j+1].idx || il[j].idx==il[j+1].idx && il[j].t<il[j+1].t) {
			til=il[j];
			il[j]=il[j+1];
			il[j+1]=til;
			j--;
			if (j<0)
				j=0;
		} else
			j++;
	}
}

// doesn't handle very complex cases, (or degenerates)
void boolabs(const poly *pal,const poly *pbl,polylist *cl,boolop bo)
{
#if 1
	mat3 xz2uv;
	int id;
	float ang=0;
	int retries=0;
	int hasint;
	poly plal,plbl;
	int nintlista,nintlistb;
	intlist intlista[MAXINTLIST],intlistb[MAXINTLIST];
	int i,j,ip,jp,k,f;//,/*vis,*/curpoly=cl->npolys;
	pointf3 *a,*b,*ap,*bp;
	uv *auv=0,*auvp=0;
//	pointf3 perta,pertap;
// run though all combinations of line segments to find intersections, and build unsorted intlista,intlistb
//	logger("intersection...............................................\n");
//	if (al->npolys!=1 || bl->npolys!=1)
//		errorexit("intersectionab: can't handle multi polys (yet)");
	copypoly(pal,&plal);
	copypoly(pbl,&plbl);
	a=&plal.verts[0];
	if (cl->hasuvs) {
		auv=&plal.uvs[0];
		makexz2uyv(a,a+1,a+2,auv,auv+1,auv+2,&xz2uv);
	} else
		makexz2y(a,a+1,a+2,&xz2uv);
	S32 plalnverts=plal.verts.size();
	S32 plblnverts=plbl.verts.size();
	while(1) {
retry2:
		retries++;
		ang+=PERTANGSTEP;
		if (ang>=TWOPI)
			ang-=TWOPI;
		if (retries>10)
			errorexit("too many retries, perturb");
		nintlista=nintlistb=hasint=0;
		id=1;
		for (i=0;i<plalnverts;i++) {
			ip=i+1;
			if (ip==plalnverts)
				ip=0;
			a=&plal.verts[i];
			ap=&plal.verts[ip];
			if (nintlista>=MAXINTLIST)
				errorexit("too many int list a (v)");
			intlista[nintlista].t=0;
			intlista[nintlista].crs=0;
			intlista[nintlista].id=0;
			intlista[nintlista].idx=i;
			intlista[nintlista].visited=0;
/*			perta.x=a->x;//+PERTERBX;
			perta.y=a->y;
			perta.z=a->z;//+PERTERBZ;
			pertap.x=ap->x;//+PERTERBX;
			pertap.y=ap->y;
			pertap.z=ap->z;//+PERTERBZ; */
			if (cl->hasuvs) {
				auv=&plal.uvs[i];
				auvp=&plal.uvs[ip];
				intlista[nintlista].iuv=*auv;
			}
			intlista[nintlista++].v=*a;
			for (j=0;j<plblnverts;j++) {
				float t,u;
				pointf3 intsect;
				jp=j+1;
				if (jp==plblnverts)
					jp=0;
				b=&plbl.verts[j];
				bp=&plbl.verts[jp];
				if (i==0) {
					if (nintlistb>=MAXINTLIST)
						errorexit("too many int list b (v)");
					intlistb[nintlistb].t=0;
					intlistb[nintlistb].crs=0;
					intlistb[nintlistb].id=0;
					intlistb[nintlistb].idx=j;
					if (cl->hasuvs) {
						convertxz2uv(&xz2uv,b,&intlistb[nintlistb].iuv);
					} else
						convertxz2justy(&xz2uv,b);
					intlistb[nintlistb++].v=*b;
				}
				int r=intersectline2d(a,ap,b,bp,&intsect,&t,&u);
				if (r>0) {
					if (t==0) {
						a->x+=PERT*cosf(ang);
						a->z+=PERT*sinf(ang);
						goto retry2;
					} 
					if (t==1) {
						ap->x+=PERT*cosf(ang);
						ap->z+=PERT*sinf(ang);
						goto retry2;
//						errorexit("t== %f",t);
					}
					if (u==0) {
						b->x+=PERT*cosf(ang);
						b->z+=PERT*sinf(ang);
						goto retry2;
					}
					if (u==1) {
						bp->x+=PERT*cosf(ang);
						bp->z+=PERT*sinf(ang);
						goto retry2;
//						errorexit("u== %f",u);
					}
					hasint=1;
					float crs=(ap->x-a->x)*(bp->z-b->z)-(bp->x-b->x)*(ap->z-a->z);
	//				logger("intersection found at %f %f %f, t %f, u %f,crs %f\n",intsect.x,intsect.y,intsect.z,t,u,crs);
					if (nintlista>=MAXINTLIST)
						errorexit("too many int list a (i)");
					intlista[nintlista].t=t;
					intlista[nintlista].idx=i;
					intlista[nintlista].crs=crs;
					intlista[nintlista].visited=0;
					intlista[nintlista].id=id;
					if (cl->hasuvs) {
						intlista[nintlista].iuv.u=(1-t)*auv->u+t*auvp->u;
						intlista[nintlista].iuv.v=(1-t)*auv->v+t*auvp->v;
						intlistb[nintlistb].iuv=intlista[nintlista].iuv;
					}
					intlista[nintlista++].v=intsect;
					if (nintlistb>=MAXINTLIST)
						errorexit("too many int list b (i)");
					intlistb[nintlistb].t=u;
					intlistb[nintlistb].idx=j;
					intlistb[nintlistb].crs=-crs;
					intlistb[nintlistb].id=id++;
					intlistb[nintlistb++].v=intsect;
				}
			}
		}
// sort both lists
		sortintlist(intlista,nintlista);
		float crssign=0;
		int npcrs=0,nmcrs=0;
		for (i=0;i<nintlista;i++) {
			if (intlista[i].crs>0) {
				if (crssign==0) {
					npcrs++;
				} else if (crssign>0) {
					npcrs++;
				} else {
					goto retry2;
				}
			} else if (intlista[i].crs<0) {
				if (crssign==0) {
					nmcrs++;
				} else if (crssign>0) {
					goto retry2;
				} else {
					nmcrs++;
				}
			}
		}
		if (nmcrs!=npcrs)
			goto retry2;
		switch(bo) {
		case OP_INTERSECTION:
			sortintlist(intlistb,nintlistb);
			break;
		case OP_MINUS:
			sortintlistr(intlistb,nintlistb);
			break;
		default:
			errorexit("unknown op");
		}
		crssign=0;
		npcrs=nmcrs=0;
		for (i=0;i<nintlistb;i++) {
			if (intlistb[i].crs>0) {
				if (crssign==0) {
					npcrs++;
				} else if (crssign>0) {
					npcrs++;
				} else {
					goto retry2;
				}
			} else if (intlistb[i].crs<0) {
				if (crssign==0) {
					nmcrs++;
				} else if (crssign>0) {
					goto retry2;
				} else {
					nmcrs++;
				}
			}
		}
		if (nmcrs!=npcrs)
			goto retry2;
		break;
	}
	if (!hasint) {
		for (i=0;i<plalnverts;i++) {
			if (!inside(&plbl,&plal.verts[i]))
				break;
		}
		switch(bo) {
		case OP_INTERSECTION:
			if (i==plalnverts) { // then a is inside b
				poly *pc=growpolys(cl);
//				if (cl->npolys>=MAXPOLYS)
//					errorexit("too many polys");
				copypoly(&plal,pc);
//				freepoly(&plal);
//				freepoly(&plbl);
				return;
			}
			for (i=0;i<plblnverts;i++) {
				if (!inside(&plal,&plbl.verts[i]))
					break;
			}
			if (i==plblnverts) { // then b is inside a
//				if (cl->npolys>=MAXPOLYS)
//					errorexit("too many polys");
//				if (cl->npolys>=MAXPOLYS)
//					errorexit("too many polys");
				poly *pc=growpolys(cl);
				copypoly(&plbl,pc);
//				cl->npolys++;
//				freepoly(&plal);
//				freepoly(&plbl);
				return;
			} 
// disjoint
//			cl->npolys=0;
//			freepoly(&plal);
//			freepoly(&plbl);
			return;
//			break;
		case OP_MINUS:
			if (i==plalnverts) { // then a is inside b
//				cl->npolys=0;
//				freepoly(&plal);
//				freepoly(&plbl);
				return;
			}
			for (i=0;i<plblnverts;i++) {
				if (!inside(&plal,&plbl.verts[i]))
					break;
			}
			if (i==plblnverts) { // then b is inside a, a with b hole
				poly *pc=growpolys(cl);
//				cl->npolys=2;
//				cl->polys[0].nverts=plal.nverts;
//				cl->polys[0].ptype=PT_POLY;
/*				for (i=0;i<plal.nverts;i++) {
					if (cl->hasuvs)
						growverts(pc,&plal.verts[i],&plal.uvs[i]);
					else
						growverts(pc,,&plal.verts[i],0);
					cl->polys[0].verts[i]=plal.verts[i];
						cl->polys[0].uvs[i]=plal.uvs[i];
				} */
				copypoly(&plal,pc);
				pc=growpolys(cl);
//				cl->polys[1].nverts=plbl.nverts;
				pc->ptype=PT_HOLE;
				for (i=0;i<plblnverts;i++)
					if (cl->hasuvs)
						growverts(pc,&plbl.verts[plblnverts-1-i],&plbl.uvs[plblnverts-1-i]);
					else
						growverts(pc,&plbl.verts[plblnverts-1-i],0);
//				freepoly(&plal);
//				freepoly(&plbl);
				return;
			}
// disjoint
//			if (cl->npolys>=MAXPOLYS)
//				errorexit("too many polys");
			poly *pc=growpolys(cl);
			copypoly(&plal,pc);
//			cl->npolys++;
//			freepoly(&plal);
//			freepoly(&plbl);
			return; 
//			break;
		}
	}
/*	for (i=0;i<nintlista;i++)
		logger("lista %d: idx %d, t %g, crs %f, intsect %g %f %g, id %d\n",
			i,intlista[i].idx,intlista[i].t,intlista[i].crs,intlista[i].v.x,intlista[i].v.y,intlista[i].v.z,
			intlista[i].id);
	for (i=0;i<nintlistb;i++)
		logger("listb %d: idx %d, t %g, crs %f, intsect %g %f %g, id %d\n",
			i,intlistb[i].idx,intlistb[i].t,intlistb[i].crs,intlistb[i].v.x,intlistb[i].v.y,intlistb[i].v.z,
			intlistb[i].id);
*/
// now start generating intersection polylist
//	cl->npolys=1;
//	cl->polys[0].nverts=0;
	i=0;
//	vis=0;
	while(1) {
//		if (curpoly>=MAXPOLYS)
//			errorexit("too many polys");
		while(i<nintlista) { // find first penetration
			if (bo==OP_INTERSECTION) {
				if (!intlista[i].visited && intlista[i].crs>0)
					break;
			} else /*if (bo==OP_MINUS)*/ {
				if (!intlista[i].visited && intlista[i].crs<0)
					break;
			}
			i++;
		}
		if (i==nintlista)
			break;//errorexit("can't find intsect");
		poly *pc=growpolys(cl);
		intlista[i].visited=1;
		k=i; // remember index into 'a' list
//		cl->polys[curpoly].nverts=0;
		if (cl->hasuvs)
			growverts(pc,&intlista[i].v,&intlista[i].iuv);
		else
			growverts(pc,&intlista[i].v,0);
//		cl->polys[curpoly].verts[0]=intlista[i].v; // add first intsect to list
//			cl->polys[curpoly].uvs[0]=intlista[i].iuv;
//		cl->polys[curpoly].nverts=1;
//		cl->polys[curpoly].ptype=PT_POLY;
		i++; // on to next vert
		if (i==nintlista) // shouldn't happen so early
			i=0;
		j=0; // 0 'a', 1 'b'
		while(1) {
			if (j==0) { // walk on a 
				if (intlista[i].crs) { // intersection
					for (f=0;f<nintlistb;f++)
						if (intlistb[f].id==intlista[i].id)
							break;
					if (f==nintlistb)
						errorexit("can't find intsect find b");
//					if (cl->polys[curpoly].nverts>=MAXVERTICES)
//						errorexit("too many verts intsect a");
					if (cl->hasuvs)
						growverts(pc,&intlista[i].v,&intlista[i].iuv);
					else
						growverts(pc,&intlista[i].v,0);
//					cl->polys[curpoly].verts[cl->polys[curpoly].nverts]=intlista[i].v;
//						cl->polys[curpoly].uvs[cl->polys[curpoly].nverts]=intlista[i].iuv;
					intlista[i].visited=1;
					i=f; // switch lists a to b
					j=1;
					if (i==k && j==0)
						break;
					i++; // on to next vert
					if (i==nintlistb)
						i=0;
				} else {
//					if (cl->polys[curpoly].nverts>=MAXVERTICES)
//						errorexit("too many verts non intsect a");
					if (cl->hasuvs)
						growverts(pc,&intlista[i].v,&intlista[i].iuv);
					else
						growverts(pc,&intlista[i].v,0);
//					cl->polys[curpoly].verts[cl->polys[curpoly].nverts]=intlista[i].v;
//						cl->polys[curpoly].uvs[cl->polys[curpoly].nverts]=intlista[i].iuv;
					if (i==k && j==0)
						break;
					i++; // on to next vert
					if (i==nintlista)
						i=0;
				}
			} else { // walk on b
				if (intlistb[i].crs) { // intersection
					for (f=0;f<nintlista;f++)
						if (intlista[f].id==intlistb[i].id)
							break;
					if (f==nintlista)
						errorexit("can't find intsect find a");
//					if (cl->polys[curpoly].nverts>=MAXVERTICES)
//						errorexit("too many verts intsect b");
					if (cl->hasuvs)
						growverts(pc,&intlistb[i].v,&intlista[f].iuv);
					else
						growverts(pc,&intlistb[i].v,0);
//					cl->polys[curpoly].verts[cl->polys[curpoly].nverts]=intlistb[i].v;
//						cl->polys[curpoly].uvs[cl->polys[curpoly].nverts]=intlista[f].iuv;
					i=f; // switch lists b to a
					intlista[i].visited=1;
					j=0;
					if (i==k && j==0)
						break;
					i++; // on to next vert
					if (i==nintlista)
						i=0;
				} else {
//					if (cl->polys[curpoly].nverts>=MAXVERTICES)
//						errorexit("too many verts non intsect b");
//					if (cl->hasuvs)
						growverts(pc,&intlistb[i].v,&intlistb[i].iuv);
//					else
//						growverts(pc,&intlistb[i].v,0);
//					cl->polys[curpoly].verts[cl->polys[curpoly].nverts]=intlistb[i].v;
//						cl->polys[curpoly].uvs[cl->polys[curpoly].nverts]=intlistb[i].iuv;
					if (i==k && j==0)
						break;
					i++; // on to next vert
					if (i==nintlistb)
						i=0;
				}
			}
//			cl->polys[curpoly].nverts++;
		}
		pc->matid=pal->matid;
//		curpoly++;
		i++;
//		break;
	}
//	cl->npolys=curpoly;
/*	logger("npolys %2d\n",cl->npolys);
	for (i=0;i<cl->npolys;i++) {
		logger("    poly %2d: nverts %2d\n",i,cl->polys[i].nverts);
		for (j=0;j<cl->polys[i].nverts;j++)
			logger("       vert %2d: %f %f %f\n",j,cl->polys[i].verts[j].x,cl->polys[i].verts[j].y,cl->polys[i].verts[j].z);
	}
*/	
//	freepoly(&plal);
//	freepoly(&plbl);
#endif
}
//			if (cl->polys[curpoly].nverts>=MAXVERTICES)
//				errorexit("too many verts intsect pen");
polylist *boolab(polylist *al,polylist *bl,boolop bo)
//void boolab(polylist *al,polylist *bl,polylist *cl,boolop bo)
{
	U32 i,j;
//	polylist *cl=newpolylist();
	polylist* cl=new polylist;
	cl->hasuvs=al->hasuvs;
//	cl->hasuvs=0;
//	if (bo==OP_INTERSECTION) {
//		for (i=0;i<1;i++)
//			for (j=0;j<1;j++)
	for (i=0;i<al->polys.size();i++)
		for (j=0;j<bl->polys.size();j++)
			boolabs(&al->polys[i],&bl->polys[j],cl,bo);
//	}
	return cl;
}
