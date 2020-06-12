// bbox code
// test bbox collisions (new algorithm)
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include <engine7cpp.h>
#include <m_eng.h>

#include "objcollisions.h"

struct bbox3d bboxs3d[MAX3DBOX];

static int colltab[MAX3DBOX][MAX3DBOX];
static int colltabidx[MAX3DBOX][MAX3DBOX];

static int colltabx[MAX3DBOX][MAX3DBOX];
static int colltaby[MAX3DBOX][MAX3DBOX];
static int colltabz[MAX3DBOX][MAX3DBOX];

int ncolpairs;
struct colpair3d colpairs3d[MAX3DBOX*MAX3DBOX];

// odd is end
// even is start
// / 2 is boxnum
static int sweepx[MAX3DBOX*2];
static int sweepy[MAX3DBOX*2];
static int sweepz[MAX3DBOX*2];

static int nboxes;

void init3dbboxes(int nb)
{
	int i;
	if (nb>MAX3DBOX)
		errorexit("set MAX3DBBOX higher, max %d, wanted %d",MAX3DBOX,nb);
	nboxes=nb;
	for (i=0;i<nboxes*2;i++)
		sweepx[i]=i;
	memset(colltabx,0,sizeof(colltabx));
	for (i=0;i<nboxes*2;i++)
		sweepy[i]=i;
	memset(colltaby,0,sizeof(colltaby));
	for (i=0;i<nboxes*2;i++)
		sweepz[i]=i;
	memset(colltabz,0,sizeof(colltaby));
	memset(colltab,0,sizeof(colltab));
	ncolpairs=0;
}

void collide3dboxes()
{
	int i,j,k,old;
	int ise0,ise1,bn0,bn1;
	float p0,p1;
// do x
	//logger("do x\n"); // temp
	for (k=0;k<nboxes*2-1;k++) {
		for (i=k;i>=0;i--) {
			bn0=sweepx[i];
			bn1=sweepx[i+1];
			ise0=bn0&1;
			ise1=bn1&1;
			bn0>>=1;
			bn1>>=1;
			if (ise0)
				p0=bboxs3d[bn0].e.x;
			else
				p0=bboxs3d[bn0].b.x;
			if (ise1)
				p1=bboxs3d[bn1].e.x;
			else
				p1=bboxs3d[bn1].b.x;
			if (p0>p1) {
				j=sweepx[i];
				sweepx[i]=sweepx[i+1];
				sweepx[i+1]=j;
				if (ise0^ise1) {
					colltabx[bn0][bn1]^=1;
					colltabx[bn1][bn0]^=1;
					old=colltab[bn0][bn1];
					colltab[bn0][bn1]=colltab[bn1][bn0]=
						colltabx[bn0][bn1]&colltaby[bn0][bn1]&colltabz[bn0][bn1];
					if (colltab[bn0][bn1] && !old) {
						colltabidx[bn0][bn1]=colltabidx[bn1][bn0]=ncolpairs;
						if (bn0<bn1) {
							colpairs3d[ncolpairs].a=bn0;
							colpairs3d[ncolpairs].b=bn1;
						} else {
							colpairs3d[ncolpairs].a=bn1;
							colpairs3d[ncolpairs].b=bn0;
						}
						ncolpairs++;
					} else if (!colltab[bn0][bn1] && old) {
						int oidx=colltabidx[bn0][bn1];
						struct colpair3d cp;
						ncolpairs--;
						cp=colpairs3d[ncolpairs];
						colpairs3d[oidx]=cp;
						colltabidx[cp.a][cp.b]=colltabidx[cp.b][cp.a]=oidx;
					}
				}
			} else
				break;
		}
	}
// do y
	//logger("do y\n"); // temp
	for (k=0;k<nboxes*2-1;k++) {
		for (i=k;i>=0;i--) {
			bn0=sweepy[i];
			bn1=sweepy[i+1];
			ise0=bn0&1;
			ise1=bn1&1;
			bn0>>=1;
			bn1>>=1;
			if (ise0)
				p0=bboxs3d[bn0].e.y;
			else
				p0=bboxs3d[bn0].b.y;
			if (ise1)
				p1=bboxs3d[bn1].e.y;
			else
				p1=bboxs3d[bn1].b.y;
			if (p0>p1) {
				j=sweepy[i];
				sweepy[i]=sweepy[i+1];
				sweepy[i+1]=j;
				if (ise0^ise1) {
					colltaby[bn0][bn1]^=1;
					colltaby[bn1][bn0]^=1;
					old=colltab[bn0][bn1];
					colltab[bn0][bn1]=colltab[bn1][bn0]=
						colltabx[bn0][bn1]&colltaby[bn0][bn1]&colltabz[bn0][bn1];
					if (colltab[bn0][bn1] && !old) {
						colltabidx[bn0][bn1]=colltabidx[bn1][bn0]=ncolpairs;
						if (bn0<bn1) {
							colpairs3d[ncolpairs].a=bn0;
							colpairs3d[ncolpairs].b=bn1;
						} else {
							colpairs3d[ncolpairs].a=bn1;
							colpairs3d[ncolpairs].b=bn0;
						}
						ncolpairs++;
					} else if (!colltab[bn0][bn1] && old) {
						int oidx=colltabidx[bn0][bn1];
						struct colpair3d cp;
						ncolpairs--;
						cp=colpairs3d[ncolpairs];
						colpairs3d[oidx]=cp;
						colltabidx[cp.a][cp.b]=colltabidx[cp.b][cp.a]=oidx;
					}
				}
			} else
				break;
		}
	}
// do z
	//logger("do z\n"); // temp
	for (k=0;k<nboxes*2-1;k++) {
		for (i=k;i>=0;i--) {
			bn0=sweepz[i];
			bn1=sweepz[i+1];
			ise0=bn0&1;
			ise1=bn1&1;
			bn0>>=1;
			bn1>>=1;
			if (ise0)
				p0=bboxs3d[bn0].e.z;
			else
				p0=bboxs3d[bn0].b.z;
			if (ise1)
				p1=bboxs3d[bn1].e.z;
			else
				p1=bboxs3d[bn1].b.z;
			if (p0>p1) {
				j=sweepz[i];
				sweepz[i]=sweepz[i+1];
				sweepz[i+1]=j;
				if (ise0^ise1) {
					colltabz[bn0][bn1]^=1;
					colltabz[bn1][bn0]^=1;
					old=colltab[bn0][bn1];
					colltab[bn0][bn1]=colltab[bn1][bn0]=
						colltabx[bn0][bn1]&colltaby[bn0][bn1]&colltabz[bn0][bn1];
					if (colltab[bn0][bn1] && !old) {
						colltabidx[bn0][bn1]=colltabidx[bn1][bn0]=ncolpairs;
						if (bn0<bn1) {
							colpairs3d[ncolpairs].a=bn0;
							colpairs3d[ncolpairs].b=bn1;
						} else {
							colpairs3d[ncolpairs].a=bn1;
							colpairs3d[ncolpairs].b=bn0;
						}
						ncolpairs++;
					} else if (!colltab[bn0][bn1] && old) {
						int oidx=colltabidx[bn0][bn1];
						struct colpair3d cp;
						ncolpairs--;
						cp=colpairs3d[ncolpairs];
						colpairs3d[oidx]=cp;
						colltabidx[cp.a][cp.b]=colltabidx[cp.b][cp.a]=oidx;
					}
				}
			} else
				break;
		}
	}
}
