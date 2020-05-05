//#include <windows.h>
//#include <math.h>

//#include <engine1.h>
#include <m_eng.h>
#include "st2_glue.h"

#include "st2_roadheight.h"
#include "st2_uplay3_internal.h"
//#include "tensor.h"
//#include "macros.h"
//#include "../3dlib/mesh.h"
//#include "data.h"
//#include "misc.h"

namespace st2 {


float crossf2(VEC *a,VEC *b)
{
	return a->x*b->z - a->z*b->x;
}

// return 1 if inside or on the triangle
int insidetri(VEC *vbase,FACE *t,VEC *p)
{
	VEC v0,v1,v2;
	float c0,c1,c2;
	v0.x=vbase[t->vertidx[0]].x-p->x;
	v0.z=vbase[t->vertidx[0]].z-p->z;
	v1.x=vbase[t->vertidx[1]].x-p->x;
	v1.z=vbase[t->vertidx[1]].z-p->z;
	v2.x=vbase[t->vertidx[2]].x-p->x;
	v2.z=vbase[t->vertidx[2]].z-p->z;
	c0=crossf2(&v0,&v1);
	c1=crossf2(&v1,&v2);
	c2=crossf2(&v2,&v0);
//	if (c0>=0 && c1>=0 && c2>=0)  // ccw
//		return 1;
	if (c0<=0 && c1<=0 && c2<=0) // cw
		return 1;
	return 0;
}

// returns y
float getroadinfo(VEC *vbase,FACE *tri,VEC *xz,VEC *norm)
{
	VEC uvec,vvec;
	VEC del;
	VEC uv;
	float a,b,c,d;
	float det;
//	xy->x=505;
//	xy->y=277;
	uvec.x=vbase[tri->vertidx[1]].x-vbase[tri->vertidx[0]].x; 
	uvec.y=vbase[tri->vertidx[1]].y-vbase[tri->vertidx[0]].y; 
	uvec.z=vbase[tri->vertidx[1]].z-vbase[tri->vertidx[0]].z; 
	vvec.x=vbase[tri->vertidx[2]].x-vbase[tri->vertidx[0]].x; 
	vvec.y=vbase[tri->vertidx[2]].y-vbase[tri->vertidx[0]].y; 
	vvec.z=vbase[tri->vertidx[2]].z-vbase[tri->vertidx[0]].z; 
	// invert a 2d matrix
	det=uvec.x*vvec.z-uvec.z*vvec.x;
	if (det==0.0f) {
		logger("degenrate tri found\n");
		setVEC(norm,0,1,0);
		return vbase[tri->vertidx[0]].y;
	}
	d= uvec.x/det;
	b=-uvec.z/det;
	c=-vvec.x/det;
	a= vvec.z/det;
	del.x = xz->x - vbase[tri->vertidx[1]].x;
	del.z = xz->z - vbase[tri->vertidx[1]].z;
	uv.x = a*del.x + c*del.z;
	uv.z = b*del.x + d*del.z;
	// dy/dx
//	slope->x=a*uvec.y+b*vvec.y;
	// dy/dz
//	slope->z=c*uvec.y+d*vvec.y;
	cross3d(&uvec, &vvec, norm);
	normalize3d(norm,norm);
	return vbase[tri->vertidx[1]].y + uv.x*uvec.y + uv.z*vvec.y;
}

int line2tri(VEC *vbase,FACE *tri,VEC *top,VEC *bot,VEC *intsect,VEC *norm)
{
	VEC uvec,vvec;
	VEC *v0,*v1,*v2;
	VEC del,topmv0;
	int *vp;
	float t,n,d;
	VEC v0mi,v1mi,v2mi;
	VEC c0,c1,c2;
	float d0,d1,d2;
// getdata
	vp=tri->vertidx;
	v0=&vbase[vp[0]];
	v1=&vbase[vp[1]];
	v2=&vbase[vp[2]];
	uvec.x=v1->x-v0->x; 
	uvec.y=v1->y-v0->y; 
	uvec.z=v1->z-v0->z; 
	vvec.x=v2->x-v0->x; 
	vvec.y=v2->y-v0->y; 
	vvec.z=v2->z-v0->z; 
// getnormal
	cross3d(&uvec, &vvec, norm);
// solve for intersection of line with plane
	topmv0.x=top->x-v0->x;
	topmv0.y=top->y-v0->y;
	topmv0.z=top->z-v0->z;
	del.x=bot->x-top->x;
	del.y=bot->y-top->y;
	del.z=bot->z-top->z;
	d=dot3d(&del,norm);
	if (d>-EPSILON)
		return 0;
	n=dot3d(&topmv0,norm);
	t=-n/d;
	if (t<0 || t>1)
		return 0;
	intsect->x=top->x+t*del.x;
	intsect->y=top->y+t*del.y;
	intsect->z=top->z+t*del.z;
// now check to see if intsect is on the triangle	
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
	if (d0>=0 && d1>=0 && d2>=0) {
//	if (d0>0 && d1>0 && d2>0) {
		normalize3d(norm,norm);
		return 1;
	}
	return 0;
}

int line2piece(model *b,VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	int i;
	VEC intsect,norm;
	int foundtri=0;
	int nface;
	float dist,bestdist;
	vector<FACE> f;
	VEC *vb;
//	struct model *b;
//	b=&bodies[bodyid];
	f = b->faces;
	nface=f.size();
	vb=b->verts;
	for (i=0;i<nface;i++) {
		if (line2tri(vb,&f[i],top,bot,&intsect,&norm)) {
			dist=dist3dsq(top,&intsect);
			if (!foundtri || dist<bestdist) {
 			    foundtri=1;
				bestdist=dist;
				*bestintsect=intsect;
				*bestnorm=norm;
			}
		}
	}
	return foundtri;
}

int checkmoretris;
int line2piecegrid(COLLGRID *cg,VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm)
{
	int i;
	int bestmat;
	VEC intsect,norm;
	int foundtri=0;
	int nface;
	int gxmin,gxmax,gzmin,gzmax,gx,gz;
	float dist,bestdist;
	FACE **f;
	VEC *vb;
	vb=cg->verts;
	getgridxz(cg,top,&gxmin,&gzmin);
	gxmax=gxmin;
	gzmax=gzmin;
	getgridxz(cg,bot,&gx,&gz);
	if (gx>gxmax)
		gxmax=gx;
	else
		gxmin=gx;
	if (gz>gzmax)
		gzmax=gz;
	else
		gzmin=gz;
	if (gxmin<0)
		gxmin=0;
	if (gxmax>=ST2_COLLGRIDX)
		gxmax=ST2_COLLGRIDX-1;
	if (gzmin<0)
		gzmin=0;
	if (gzmax>=ST2_COLLGRIDZ)
		gzmax=ST2_COLLGRIDZ-1;
/*	if (checkmoretris) {
		gzmin=0;
		gxmin=0;
		gzmax=ST2_COLLGRIDZ-1;
		gxmax=ST2_COLLGRIDX-1;
	} */
	for (gz=gzmin;gz<=gzmax;gz++)
		for (gx=gxmin;gx<=gxmax;gx++) {
			nface=cg->nfaceidx[gz][gx];
			f=cg->faceidx[gz][gx];
			for (i=0;i<nface;i++,f++) {
				if (line2tri(vb,*f,top,bot,&intsect,&norm)) {
					dist=dist3dsq(top,&intsect);
					if (!foundtri || dist<bestdist) {
 						foundtri=1;
						bestmat=(*f)->fmatidx;
						bestdist=dist;
						*bestintsect=intsect;
						*bestnorm=norm;
					}
				}
			}
		}
	if (foundtri)
		return bestmat+1;
	return 0;
}

float doroadheighto(TREE **t,int nt,VEC *pos,VEC *maxnorm)
{
	int i,j;
	//float maxheight=-1000000;
	float maxheight=1000000;
	float curheight;
	VEC curnorm;
	int foundtri=0;
	int nface;
	vector<FACE> f;
	VEC *vb;
	model *b;
	setVEC(maxnorm,0,1,0);
	//ErrMBExit("heY","%d",nt);
	for (j=0;j<nt;j++) {
		//ErrMBExit("HeY","%d",t[j]->bodyid);
//		b=&bodies[t[j]->bodyid];
		b = t[j]->mod;
		f = b->faces;
		nface=f.size();
		vb=b->verts;
		for (i=0;i<nface;i++)
			if (insidetri(vb,&f[i],pos)) {
				curheight=getroadinfo(vb,&f[i],pos,&curnorm);
				if (curheight<maxheight) { // for now the highest point is of interest
					*maxnorm=curnorm;
					maxheight=curheight;
					foundtri=1;
				}
			}
	}
	if (foundtri)
		return maxheight;
	else
		return 0;
}

float doroadheight2(model *b,VEC *pos,VEC *maxnorm, float lrh)
{
	int i;
	float maxheight=1000000;
	float curheight;
	VEC curnorm;
	int foundtri=0;
	int nface;
	vector<FACE> f;
	VEC *vb;
//	struct model *b;

	setVEC(maxnorm,0,1,0);

//	b=&bodies[bodyid];
	f = b->faces;
	nface=f.size();
	vb=b->verts;

	for (i=0;i<nface;i++)
	{
		if (insidetri(vb,&f[i],pos)) {
			curheight=getroadinfo(vb,&f[i],pos,&curnorm);
			if (curheight<=maxheight) { // for now the highest point is of interest
				*maxnorm=curnorm;
				maxheight=curheight;
 			    foundtri=1;
			}

		}
	}

	if (foundtri)
		return maxheight;
	else
		return 0;
}



float doroadheight(TREE **t,int nt,VEC *pos,VEC *maxnorm, float lrh, float *close)
{
	int i,j;
	float maxheight=-1000000;
	float maxheight2=-1000000;
	//oat maxheight=1000000;
	float diff2,diff=10000000;
	float curheight;
	VEC curnorm;
	int foundtri=0;
	int nface;
//	float height2;
	vector<FACE> f;
	VEC *vb;
	modelb *b;
	setVEC(maxnorm,0,1,0);
	for (j=0;j<nt;j++) {
		b=t[j]->mod;
		f = b->faces;
		nface = f.size();
		vb=b->verts;
		for (i=0;i<nface;i++)
			if (insidetri(vb,&f[i],pos)) {
				curheight=getroadinfo(vb,&f[i],pos,&curnorm);
				
				diff2 = abs(curheight - lrh);

				if (diff2<diff)
				{
			 	 *maxnorm=curnorm;
				 *close = curheight;
				 foundtri=1;
				 diff = diff2;
			 	 maxheight=curheight;
				 *close = diff;
				}
				
				//if (curheight>maxheight) { // for now the highest point is of interest
				//	*maxnorm=curnorm;
				//	*close = curheight;
				//	maxheight=curheight;
				//	foundtri=1;
			//	}
			}
	}
	if (foundtri)
		return maxheight;
	else
		return 0;
}
///////// end road height code

///////// begin obj transform to world ///////////
void converttoworld(TREE *t)
{
	VEC *v;
	model *b;
	b=t->mod;
	v=b->verts;
	obj2worldn(t, v, v, b->verts.size());
	b->setbbox();
	// normals ???
}

void zerokeyframes(TREE *t)
{
	t->userproc=NULL;
	t->trans=zerov;
	t->rot=zerov;
	t->scale=onev;
}

float st2_getpitchfromnorm(VEC *n)
{
	return (float)((180/PI)*atan2(sqrt(n->x*n->x+n->z*n->z),n->y));
}

float st2_getyawfromnorm(VEC *n)
{
	return (float)((180/PI)*atan2(n->x,n->z));
}
////////// end obj transform to world ///////////////////


TREE* duptree(TREE* t)
{
	return t->newdup();
}

void linkchildtoparent(TREE* c, TREE* p) 
{
	p->linkchild(c);
}

}
