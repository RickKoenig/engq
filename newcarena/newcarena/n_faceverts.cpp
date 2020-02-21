//#include <engine7cpp.h>
//#include "facevertscpp.h"

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "n_deflatecpp.h"
#include "enums.h" 
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "n_faceverts.h"
#include "n_polylist.h"
#include "n_meshtest.h"
#include "carenalobby.h"
#include "tracklist.h"
#include "newconstructor.h"

void getbacky(pointf3* v)
{
	v->y=(v->x-10)*(v->z-10)/15/15*6;
	v->y/=10;
//	v->y=yheight;
}

facevert *buildbackgndsectionfv(backgndsectioninfo *bs)
//void buildbackgndsection(backgndsectioninfo *bs,
//								pointf3* bverts,int maxverts,int *nverts,
//								face *bfaces,int maxfaces,int *nfaces,
//								edge *bedges,int maxedges,int *nedges)
{
//	facevert *fv=(facevert *)memzalloc(sizeof(facevert));
	facevert* fv=new facevert;
	int i,j;
//	int mf,mv,me;
	pointf3 stx;//,*v;
//	face *f;
//	edge *e;
	S32 nface=2*bs->nstepsx*bs->nstepsz;
//	if (mf>maxfaces)
//		errorexit("buildbackgndsection: too many faces, need %d have %d",mf,maxfaces);
	S32 nvert=(bs->nstepsx+1)*(bs->nstepsz+1);
//	if (mv>maxverts)
//		errorexit("buildbackgndsection: too many verts, need %d have %d",mv,maxverts);
	S32 nedge=(bs->nstepsx+1)*bs->nstepsz+bs->nstepsx*(bs->nstepsz+1)+bs->nstepsx*bs->nstepsz;
//	if (me>maxedges)
//		errorexit("buildbackgndsection: too many edges, need %d have %d",me,maxedges);
//	*nverts=mv;
//	*nfaces=mf;
//	*nedges=me;
// verts
//	fv->verts=(pointf3* )memzalloc(sizeof(VEC)*fv->nvert);
//	fv->verts=new pointf3[fv->nvert];
	fv->verts.resize(nvert);
//	pointf3* v=fv->verts;
	vector<pointf3>::iterator v=fv->verts.begin();
	stx.z=bs->start.z;
	for (j=0;j<bs->nstepsz+1;j++) {
		stx.x=bs->start.x;
		for (i=0;i<bs->nstepsx+1;i++) {
//			getbacky(&stx);
//			if ((j&1)==0)
//				stx.y=2*float(i);
//			else
//				stx.y=2*float(2-i);
			stx.y=0;
			*v=stx;
			v++;
			stx.x+=bs->step.x;
		}
		stx.z+=bs->step.z;
	}
// faces
//	fv->faces=(face *)memzalloc(sizeof(face)*fv->nface);
//	fv->faces=new face[fv->nface];
	fv->faces.resize(nface);
//	face *f=fv->faces;
	vector<face>::iterator f=fv->faces.begin();
	for (j=0;j<bs->nstepsz;j++) {
		for (i=0;i<bs->nstepsx;i++) {
			f->vertidx[0]=i+j*(bs->nstepsx+1);
			f->vertidx[1]=i+1+(j+1)*(bs->nstepsx+1);
			f->vertidx[2]=i+1+j*(bs->nstepsx+1);
			f->fmatidx=0;
			f++;
			f->vertidx[0]=i+j*(bs->nstepsx+1);
			f->vertidx[1]=i+(j+1)*(bs->nstepsx+1);
			f->vertidx[2]=i+1+(j+1)*(bs->nstepsx+1);
			f->fmatidx=0;
			f++;
		}
	}
// edges
//	fv->edges=(edge *)memzalloc(sizeof(edge)*fv->nedge);
//	fv->edges=new edge[fv->nedge];
	fv->edges.resize(nedge);
//	edge *e=fv->edges;
	vector<edge>::iterator e=fv->edges.begin();
	for (j=0;j<bs->nstepsz+1;j++)
		for (i=0;i<bs->nstepsx;i++) {
			e->ei[0]=i+j*(bs->nstepsx+1);
			e->ei[1]=i+1+j*(bs->nstepsx+1);
			e++;
		}
	for (j=0;j<bs->nstepsz;j++)
		for (i=0;i<bs->nstepsx+1;i++) {
			e->ei[0]=i+j*(bs->nstepsx+1);
			e->ei[1]=i+(j+1)*(bs->nstepsx+1);
			e++;
		}
	for (j=0;j<bs->nstepsz;j++)
		for (i=0;i<bs->nstepsx;i++) {
			e->ei[0]=i+j*(bs->nstepsx+1);
			e->ei[1]=i+1+(j+1)*(bs->nstepsx+1);
			e++;
		}
	return fv;
}

facevert *buildtracksectionfv(tracksectioninfo *ts)
//void buildtracksection(tracksectioninfo *ts,int maxfaces,int *nfaces,face *tfaces,pointf3* tverts,uv *tuvs,int maxverts,int *nverts)
//facevert *buildtracksection(tracksectioninfo *ts)
{
//	pointf3* tverts;
//	uv *tuvs;
//	face *tfaces;
	vector<pointf3>::iterator tverts;
	vector<uv>::iterator tuvs;
	vector<face>::iterator tfaces;
//	facevert *fv=(facevert *)memzalloc(sizeof(facevert));
	facevert* fv=new facevert;
//	fv->nface=ts->nsteps*8;
	S32 nface=ts->nsteps*8;
//	fv->nvert=(ts->nsteps+1)*5;
	S32 nvert=(ts->nsteps+1)*5;
//	if (ufaces>maxfaces)
//		errorexit("need %d faces, %d avail",ufaces,maxfaces);
//	if (uverts>maxverts)
//		errorexit("need %d verts, %d avail",uverts,maxverts);
	int i,j;
// faces
//	fv->faces=(face *)memzalloc(sizeof(face)*fv->nface);
//	fv->faces=new face[fv->nface];
	fv->faces.resize(nface);
//	fv->verts=(pointf3* )memalloc(sizeof(VEC)*fv->nvert);
//	fv->verts=new pointf3[fv->nvert];
	fv->verts.resize(nvert);
//	fv->uvs=(uv *)memalloc(sizeof(uv)*fv->nvert);
//	fv->uvs=new uv[fv->nvert];
	fv->uvs.resize(nvert);
	tverts=fv->verts.begin();
	tfaces=fv->faces.begin();
	tuvs=fv->uvs.begin();
	for (j=0;j<ts->nsteps;j++) {
		for (i=0;i<4;i++) {
			tfaces->vertidx[0]=i+j*5;
			tfaces->vertidx[1]=i+(j+1)*5+1;
			tfaces->vertidx[2]=i+j*5+1;
			if (i==0 || i==3)
				tfaces->fmatidx=1;
			else
				tfaces->fmatidx=0;
			tfaces++;
			tfaces->vertidx[0]=i+j*5;
			tfaces->vertidx[1]=i+(j+1)*5;
			tfaces->vertidx[2]=i+(j+1)*5+1;
			if (i==0 || i==3)
				tfaces->fmatidx=1;
			else
				tfaces->fmatidx=0;
			tfaces++;
		}
	}
// verts and uvs
	for (j=0;j<ts->nsteps+1;j++) {
		for (i=0;i<5;i++) {
			tverts->x=5*float(i);
			tverts->y=3;
			tverts->z=5*float(j);
			if (i==0 || i==4)
				tverts->vmatidx=1;
			else
				tverts->vmatidx=0;
			tuvs->u=float(i);
			tuvs->v=float(-j);
			tverts++;
			tuvs++;
		}
	}
	return fv;
}

/*void freefacevert(facevert *bfv)
{
	if (bfv->faces)
//		memfree(bfv->faces);
		delete[] bfv->faces;
	if (bfv->edges)
//		memfree(bfv->edges);
		delete[] bfv->edges;
	if (bfv->uvs)
//		memfree(bfv->uvs);
		delete[] bfv->uvs;
	if (bfv->verts)
//		memfree(bfv->verts);
		delete[] bfv->verts;
//	memfree(bfv);
	delete bfv;
}
*/
