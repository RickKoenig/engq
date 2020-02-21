//#include <engine1.h>
#include <m_eng.h>

namespace physics3d {
#include "collutil.h"

////////// handy utils
float newellnormal(VEC *pnts,int npnts,VEC *norm)
{
	int i;
	*norm=zerov;
	for (i=0;i<npnts-1;i++) {
		norm->x+=(pnts[i].y-pnts[i+1].y)*(pnts[i].z+pnts[i+1].z);
		norm->y+=(pnts[i].z-pnts[i+1].z)*(pnts[i].x+pnts[i+1].x);
		norm->z+=(pnts[i].x-pnts[i+1].x)*(pnts[i].y+pnts[i+1].y);
	}
	norm->x+=(pnts[i].y-pnts[0].y)*(pnts[i].z+pnts[0].z);
	norm->y+=(pnts[i].z-pnts[0].z)*(pnts[i].x+pnts[0].x);
	norm->z+=(pnts[i].x-pnts[0].x)*(pnts[i].y+pnts[0].y);
	return normalize3d(norm,norm);
}

// given a point and norm (1 unbounded plane), and a pnt and dir (line) return t and i
int line2plane(VEC *pln0,VEC *plnnorm,VEC *lin0,VEC *lin1,VEC *intsect)
{
	float bot,t;
	VEC pnt,lindir;
	lindir.x=lin1->x-lin0->x;
	lindir.y=lin1->y-lin0->y;
	lindir.z=lin1->z-lin0->z;
	bot=dot3d(plnnorm,&lindir);
	if (bot*bot<EPSILON)
		return 0;
	pnt.x=pln0->x-lin0->x;
	pnt.y=pln0->y-lin0->y;
	pnt.z=pln0->z-lin0->z;
	t=dot3d(&pnt,plnnorm)/bot;
	if (t>1 || t<0)
		return 0;
	intsect->x=lin0->x+t*lindir.x;
	intsect->y=lin0->y+t*lindir.y;
	intsect->z=lin0->z+t*lindir.z;
	return 1;
}

// v0 v1 v2 and v0+(v1-v0)+(v2-v0)
int line2bplane(VEC *v0i, VEC *v1i, VEC *v2i, VEC *top,VEC *bot,VEC *intsect)
{
	VEC uvec,vvec;
	VEC nrm;
	float topdn,botdn,pdn,pdi;
	float t,b0,b1;
	VEC np1,np2;
	VEC v0,v1,v2;//,cm;
	v0=*v0i;
	v1=*v1i;
	v2=*v2i;
	uvec.x=v1.x-v0.x; 
	uvec.y=v1.y-v0.y; 
	uvec.z=v1.z-v0.z; 
	vvec.x=v2.x-v0.x; 
	vvec.y=v2.y-v0.y; 
	vvec.z=v2.z-v0.z; 
	cross3d(&uvec, &vvec, &nrm);
// solve for intersection of line with plane
	topdn=dot3d(top,&nrm);
	botdn=dot3d(bot,&nrm);
	pdn=dot3d(&v0,&nrm);
	if (topdn>=pdn && botdn>=pdn)
		return 0;
	if (topdn<pdn && botdn<pdn)
		return 0;
	t=(pdn-topdn)/(botdn-topdn);
	intsect->x=top->x+t*(bot->x-top->x);
	intsect->y=top->y+t*(bot->y-top->y);
	intsect->z=top->z+t*(bot->z-top->z);
// see if on plane
	cross3d(&nrm,&uvec,&np1);
	pdi=dot3d(intsect,&np1);
	b0=dot3d(&v0,&np1);
	b1=dot3d(&v2,&np1);
	if (b0>pdi && b1>pdi)
		return 0;
	if (b0<pdi && b1<pdi)
		return 0;
	cross3d(&nrm,&vvec,&np2);
	pdi=dot3d(intsect,&np2);
	b0=dot3d(&v0,&np2);
	b1=dot3d(&v1,&np2);
	if (b0>pdi && b1>pdi)
		return 0;
	if (b0<pdi && b1<pdi)
		return 0;
	return 1;
}

// v0 v1 v2 and v0+(v1-v0)+(v2-v0)
int line2btri(VEC *v0, VEC *v1, VEC *v2, VEC *top,VEC *bot,VEC *intsect)
{
	VEC uvec,vvec;
	VEC nrm;
	float topdn,botdn,pdn;
	VEC v0mi,v1mi,v2mi,c0,c1,c2;
	float d0,d1,d2;
	float t;
	uvec.x=v1->x-v0->x; 
	uvec.y=v1->y-v0->y; 
	uvec.z=v1->z-v0->z; 
	vvec.x=v2->x-v0->x; 
	vvec.y=v2->y-v0->y; 
	vvec.z=v2->z-v0->z; 
	cross3d(&uvec, &vvec, &nrm);
// solve for intersection of line with plane
	topdn=dot3d(top,&nrm);
	botdn=dot3d(bot,&nrm);
	pdn=dot3d(v0,&nrm);
	if (topdn>=pdn && botdn>=pdn)
		return 0;
	if (topdn<=pdn && botdn<=pdn)
		return 0;
	t=(pdn-topdn)/(botdn-topdn);
	intsect->x=top->x+t*(bot->x-top->x);
	intsect->y=top->y+t*(bot->y-top->y);
	intsect->z=top->z+t*(bot->z-top->z);
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
	d0=dot3d(&c0,&nrm);
	d1=dot3d(&c1,&nrm);
	d2=dot3d(&c2,&nrm);
	if (d0>=0 && d1>=0 && d2>=0)
		return 1;
	if (d0<=0 && d1<=0 && d2<=0)
		return 1;
	return 0; 
}

// v0 v1 v2 and v0+(v1-v0)+(v2-v0)
int line2btricull(VEC *v0, VEC *v1, VEC *v2, VEC *top,VEC *bot,VEC *intsect)
{
	VEC uvec,vvec;
	VEC nrm;
	float topdn,botdn,pdn;
	VEC v0mi,v1mi,v2mi,c0,c1,c2;
	float d0,d1,d2;
	float t;
	uvec.x=v1->x-v0->x; 
	uvec.y=v1->y-v0->y; 
	uvec.z=v1->z-v0->z; 
	vvec.x=v2->x-v0->x; 
	vvec.y=v2->y-v0->y; 
	vvec.z=v2->z-v0->z; 
	cross3d(&uvec, &vvec, &nrm);
// solve for intersection of line with plane
	topdn=dot3d(top,&nrm);
	botdn=dot3d(bot,&nrm);
	pdn=dot3d(v0,&nrm);
	if (topdn<botdn)
		return 0;
	if (topdn>=pdn && botdn>=pdn)
		return 0;
	if (topdn<=pdn && botdn<=pdn)
		return 0;
	t=(pdn-topdn)/(botdn-topdn);
	intsect->x=top->x+t*(bot->x-top->x);
	intsect->y=top->y+t*(bot->y-top->y);
	intsect->z=top->z+t*(bot->z-top->z);
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
	d0=dot3d(&c0,&nrm);
	d1=dot3d(&c1,&nrm);
	d2=dot3d(&c2,&nrm);
	float fudge = 1e-8f;
	if (d0>=-fudge && d1>=-fudge && d2>=-fudge)
		return 1;
	if (d0<=fudge && d1<=fudge && d2<=fudge)
		return 1;
	return 0; 
}

// given 2 sets of 3 points (2 parellelgram planes), return 2 points that define line intersection
int plane2plane(VEC *pl0,VEC *pl1,VEC *res)
{
	int i,npnts=0;
	int bmin=-1,bmax=-1;
	int doxyz; // 0 x 1 y 2 z
	VEC maxpnts,minpnts;
	VEC intpnts[8];
	VEC pl03,pl13;
	VEC maxpl0,minpl0,maxpl1,minpl1;
	VEC delu,delv,nrm0,nrm1,crs,t,del;
	pl03.x=pl0[1].x+pl0[2].x-pl0[0].x;
	pl03.y=pl0[1].y+pl0[2].y-pl0[0].y;
	pl03.z=pl0[1].z+pl0[2].z-pl0[0].z;
	pl13.x=pl1[1].x+pl1[2].x-pl1[0].x;
	pl13.y=pl1[1].y+pl1[2].y-pl1[0].y;
	pl13.z=pl1[1].z+pl1[2].z-pl1[0].z;
	maxpl0=minpl0=pl03;
	for (i=0;i<3;i++) {
		if (pl0[i].x>maxpl0.x)
			maxpl0.x=pl0[i].x;
		if (pl0[i].y>maxpl0.y)
			maxpl0.y=pl0[i].y;
		if (pl0[i].z>maxpl0.z)
			maxpl0.z=pl0[i].z;
		if (pl0[i].x<minpl0.x)
			minpl0.x=pl0[i].x;
		if (pl0[i].y<minpl0.y)
			minpl0.y=pl0[i].y;
		if (pl0[i].z<minpl0.z)
			minpl0.z=pl0[i].z;
	}
	maxpl1=minpl1=pl13;
	for (i=0;i<3;i++) {
		if (pl1[i].x>maxpl1.x)
			maxpl1.x=pl1[i].x;
		if (pl1[i].y>maxpl1.y)
			maxpl1.y=pl1[i].y;
		if (pl1[i].z>maxpl1.z)
			maxpl1.z=pl1[i].z;
		if (pl1[i].x<minpl1.x)
			minpl1.x=pl1[i].x;
		if (pl1[i].y<minpl1.y)
			minpl1.y=pl1[i].y;
		if (pl1[i].z<minpl1.z)
			minpl1.z=pl1[i].z;
	}
	if (maxpl0.x<minpl1.x || maxpl0.y<minpl1.y || maxpl0.z<minpl1.z)
		return 0;
	if (maxpl1.x<minpl0.x || maxpl1.y<minpl0.y || maxpl1.z<minpl0.z)
		return 0;
	if (line2bplane(&pl0[0],&pl0[1],&pl0[2],&pl1[0],&pl1[1],&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl0[0],&pl0[1],&pl0[2],&pl1[1],&pl13,&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl0[0],&pl0[1],&pl0[2],&pl13,&pl1[2],&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl0[0],&pl0[1],&pl0[2],&pl1[2],&pl1[0],&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl1[0],&pl1[1],&pl1[2],&pl0[0],&pl0[1],&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl1[0],&pl1[1],&pl1[2],&pl0[1],&pl03,&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl1[0],&pl1[1],&pl1[2],&pl03,&pl0[2],&intpnts[npnts]))
		npnts++;
	if (line2bplane(&pl1[0],&pl1[1],&pl1[2],&pl0[2],&pl0[0],&intpnts[npnts]))
		npnts++;
	if (npnts<2)
		return 0;
	if (npnts==2) {
		res[0]=intpnts[0];
		res[1]=intpnts[1];
	} else {
		minpnts=maxpnts=intpnts[0];
		for (i=1;i<npnts;i++) {
			if (intpnts[i].x<minpnts.x)
				minpnts.x=intpnts[i].x;
			if (intpnts[i].y<minpnts.y)
				minpnts.y=intpnts[i].y;
			if (intpnts[i].z<minpnts.z)
				minpnts.z=intpnts[i].z;
			if (intpnts[i].x>maxpnts.x)
				maxpnts.x=intpnts[i].x;
			if (intpnts[i].y>maxpnts.y)
				maxpnts.y=intpnts[i].y;
			if (intpnts[i].z>maxpnts.z)
				maxpnts.z=intpnts[i].z;
		}
		if (maxpnts.x-minpnts.x>maxpnts.y-minpnts.y) { // x>y
			if (maxpnts.y-minpnts.y>maxpnts.z-minpnts.z) { // x>y && y>z --> x is largest
				doxyz=0;
			} else // x>y && z>y
				if (maxpnts.x-minpnts.x>maxpnts.z-minpnts.z) // x>y && z>y && x>z --> x is largest
					doxyz=0;
				else // x>y && z>y && z>x --> z is largest 
					doxyz=2;
		} else { // x<y
			if (maxpnts.y-minpnts.y>maxpnts.z-minpnts.z) { // x<y && y>z --> y is largest
				doxyz=1;
			} else // x<y && z>y --> z is largest
				doxyz=2;
		}
		if (doxyz==0) {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].x==minpnts.x) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].x==maxpnts.x) {
					bmax=i;
					break;
				}
			}
		} else if (doxyz==1) {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].y==minpnts.y) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].y==maxpnts.y) {
					bmax=i;
					break;
				}
			}
		} else {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].z==minpnts.z) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].z==maxpnts.z) {
					bmax=i;
					break;
				}
			}
		}
		if (bmin<0 || bmax<0)
			errorexit("this error just shouldn't happen");
		res[0]=intpnts[bmin];
		res[1]=intpnts[bmax];
	}
	delu.x=pl0[1].x-pl0[0].x;
	delu.y=pl0[1].y-pl0[0].y;
	delu.z=pl0[1].z-pl0[0].z;
	delv.x=pl0[2].x-pl0[0].x;
	delv.y=pl0[2].y-pl0[0].y;
	delv.z=pl0[2].z-pl0[0].z;
	cross3d(&delu,&delv,&nrm0);
	delu.x=pl1[1].x-pl1[0].x;
	delu.y=pl1[1].y-pl1[0].y;
	delu.z=pl1[1].z-pl1[0].z;
	delv.x=pl1[2].x-pl1[0].x;
	delv.y=pl1[2].y-pl1[0].y;
	delv.z=pl1[2].z-pl1[0].z;
	cross3d(&delu,&delv,&nrm1);
	cross3d(&nrm0,&nrm1,&crs);
	del.x=res[1].x-res[0].x;
	del.y=res[1].y-res[0].y;
	del.z=res[1].z-res[0].z;
	if (dot3d(&crs,&del)<0) {
		t=res[0];
		res[0]=res[1];
		res[1]=t;
	}
	return 1;
}

// given 1 sets of 3 points (2 parellelgram planes) and point normal, return 2 points that define line intersection
int plane2infplane(VEC *pl0,VEC *pl1,VEC *norm1,VEC *res)
{
	int i,npnts=0;
	int bmin=-1,bmax=-1;
	int doxyz; // 0 x 1 y 2 z
	VEC maxpnts,minpnts;
	float d,pd;
	float maxd,mind;
	VEC intpnts[8];
	VEC pl03;
	VEC delu,delv,nrm0,crs,t,del;
	pl03.x=pl0[1].x+pl0[2].x-pl0[0].x;
	pl03.y=pl0[1].y+pl0[2].y-pl0[0].y;
	pl03.z=pl0[1].z+pl0[2].z-pl0[0].z;
	maxd=mind=dot3d(&pl03,norm1);
	for (i=0;i<3;i++) {
		d=dot3d(&pl0[i],norm1);
		if (d>maxd)
			maxd=d;
		if (d<mind)
			mind=d;
	}
	pd=dot3d(pl1,norm1);
	if (maxd<pd && mind<pd)
		return 0;
	if (maxd>=pd && mind>=pd)
		return 0;
	if (line2plane(pl1,norm1,&pl0[0],&pl0[1],&intpnts[npnts]))
		npnts++;
	if (line2plane(pl1,norm1,&pl0[1],&pl03,&intpnts[npnts]))
		npnts++;
	if (line2plane(pl1,norm1,&pl03,&pl0[2],&intpnts[npnts]))
		npnts++;
	if (line2plane(pl1,norm1,&pl0[2],&pl0[0],&intpnts[npnts]))
		npnts++;
	if (npnts<2)
		return 0;
	if (npnts==2) {
		res[0]=intpnts[0];
		res[1]=intpnts[1];
	} else {
		minpnts=maxpnts=intpnts[0];
		for (i=1;i<npnts;i++) {
			if (intpnts[i].x<minpnts.x)
				minpnts.x=intpnts[i].x;
			if (intpnts[i].y<minpnts.y)
				minpnts.y=intpnts[i].y;
			if (intpnts[i].z<minpnts.z)
				minpnts.z=intpnts[i].z;
			if (intpnts[i].x>maxpnts.x)
				maxpnts.x=intpnts[i].x;
			if (intpnts[i].y>maxpnts.y)
				maxpnts.y=intpnts[i].y;
			if (intpnts[i].z>maxpnts.z)
				maxpnts.z=intpnts[i].z;
		}
		if (maxpnts.x-minpnts.x>maxpnts.y-minpnts.y) { // x>y
			if (maxpnts.y-minpnts.y>maxpnts.z-minpnts.z) { // x>y && y>z --> x is largest
				doxyz=0;
			} else // x>y && z>y
				if (maxpnts.x-minpnts.x>maxpnts.z-minpnts.z) // x>y && z>y && x>z --> x is largest
					doxyz=0;
				else // x>y && z>y && z>x --> z is largest 
					doxyz=2;
		} else { // x<y
			if (maxpnts.y-minpnts.y>maxpnts.z-minpnts.z) { // x<y && y>z --> y is largest
				doxyz=1;
			} else // x<y && z>y --> z is largest
				doxyz=2;
		}
		if (doxyz==0) {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].x==minpnts.x) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].x==maxpnts.x) {
					bmax=i;
					break;
				}
			}
		} else if (doxyz==1) {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].y==minpnts.y) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].y==maxpnts.y) {
					bmax=i;
					break;
				}
			}
		} else {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].z==minpnts.z) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].z==maxpnts.z) {
					bmax=i;
					break;
				}
			}
		}
		if (bmin<0 || bmax<0)
			errorexit("this error just shouldn't happen");
		res[0]=intpnts[bmin];
		res[1]=intpnts[bmax];
	}
	delu.x=pl0[1].x-pl0[0].x;
	delu.y=pl0[1].y-pl0[0].y;
	delu.z=pl0[1].z-pl0[0].z;
	delv.x=pl0[2].x-pl0[0].x;
	delv.y=pl0[2].y-pl0[0].y;
	delv.z=pl0[2].z-pl0[0].z;
	cross3d(&delu,&delv,&nrm0);
	cross3d(&nrm0,norm1,&crs);
	del.x=res[1].x-res[0].x;
	del.y=res[1].y-res[0].y;
	del.z=res[1].z-res[0].z;
	if (dot3d(&crs,&del)<0) {
		t=res[0];
		res[0]=res[1];
		res[1]=t;
	}
	return 1;
}

// given 2 sets of 3 points (2 triangles), return 2 points that define line intersection
int tri2tri(VEC *tr0,VEC *tr1,VEC *res)
{
	int i,npnts=0;
	int bmin=-1,bmax=-1;
	int doxyz; // 0 x 1 y 2 z
	VEC maxpnts,minpnts;
	VEC maxtr0,mintr0,maxtr1,mintr1;
	VEC intpnts[6];
	VEC nrm0,nrm1,crs,del,t,delu,delv;
	maxtr0=mintr0=tr0[0];
	for (i=1;i<3;i++) {
		if (tr0[i].x>maxtr0.x)
			maxtr0.x=tr0[i].x;
		if (tr0[i].y>maxtr0.y)
			maxtr0.y=tr0[i].y;
		if (tr0[i].z>maxtr0.z)
			maxtr0.z=tr0[i].z;
		if (tr0[i].x<mintr0.x)
			mintr0.x=tr0[i].x;
		if (tr0[i].y<mintr0.y)
			mintr0.y=tr0[i].y;
		if (tr0[i].z<mintr0.z)
			mintr0.z=tr0[i].z;
	}
	maxtr1=mintr1=tr1[0];
	for (i=1;i<3;i++) {
		if (tr1[i].x>maxtr1.x)
			maxtr1.x=tr1[i].x;
		if (tr1[i].y>maxtr1.y)
			maxtr1.y=tr1[i].y;
		if (tr1[i].z>maxtr1.z)
			maxtr1.z=tr1[i].z;
		if (tr1[i].x<mintr1.x)
			mintr1.x=tr1[i].x;
		if (tr1[i].y<mintr1.y)
			mintr1.y=tr1[i].y;
		if (tr1[i].z<mintr1.z)
			mintr1.z=tr1[i].z;
	}
	if (maxtr0.x<mintr1.x || maxtr0.y<mintr1.y || maxtr0.z<mintr1.z)
		return 0;
	if (maxtr1.x<mintr0.x || maxtr1.y<mintr0.y || maxtr1.z<mintr0.z)
		return 0;
	if (line2btri(&tr0[0],&tr0[1],&tr0[2],&tr1[0],&tr1[1],&intpnts[npnts]))
		npnts++;
	if (line2btri(&tr0[0],&tr0[1],&tr0[2],&tr1[1],&tr1[2],&intpnts[npnts]))
		npnts++;
	if (line2btri(&tr0[0],&tr0[1],&tr0[2],&tr1[2],&tr1[0],&intpnts[npnts]))
		npnts++;
	if (line2btri(&tr1[0],&tr1[1],&tr1[2],&tr0[0],&tr0[1],&intpnts[npnts]))
		npnts++;
	if (line2btri(&tr1[0],&tr1[1],&tr1[2],&tr0[1],&tr0[2],&intpnts[npnts]))
		npnts++;
	if (line2btri(&tr1[0],&tr1[1],&tr1[2],&tr0[2],&tr0[0],&intpnts[npnts]))
		npnts++;
	if (npnts<2)
		return 0;
	if (npnts==2) {
		res[0]=intpnts[0];
		res[1]=intpnts[1];
	} else {
		minpnts=maxpnts=intpnts[0];
		for (i=1;i<npnts;i++) {
			if (intpnts[i].x<minpnts.x)
				minpnts.x=intpnts[i].x;
			if (intpnts[i].y<minpnts.y)
				minpnts.y=intpnts[i].y;
			if (intpnts[i].z<minpnts.z)
				minpnts.z=intpnts[i].z;
			if (intpnts[i].x>maxpnts.x)
				maxpnts.x=intpnts[i].x;
			if (intpnts[i].y>maxpnts.y)
				maxpnts.y=intpnts[i].y;
			if (intpnts[i].z>maxpnts.z)
				maxpnts.z=intpnts[i].z;
		}
		if (maxpnts.x-minpnts.x>maxpnts.y-minpnts.y) { // x>y
			if (maxpnts.y-minpnts.y>maxpnts.z-minpnts.z) { // x>y && y>z --> x is largest
				doxyz=0;
			} else // x>y && z>y
				if (maxpnts.x-minpnts.x>maxpnts.z-minpnts.z) // x>y && z>y && x>z --> x is largest
					doxyz=0;
				else // x>y && z>y && z>x --> z is largest 
					doxyz=2;
		} else { // x<y
			if (maxpnts.y-minpnts.y>maxpnts.z-minpnts.z) { // x<y && y>z --> y is largest
				doxyz=1;
			} else // x<y && z>y --> z is largest
				doxyz=2;
		}
		if (doxyz==0) {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].x==minpnts.x) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].x==maxpnts.x) {
					bmax=i;
					break;
				}
			}
		} else if (doxyz==1) {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].y==minpnts.y) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].y==maxpnts.y) {
					bmax=i;
					break;
				}
			}
		} else {
			for (i=0;i<npnts;i++) {
				if (intpnts[i].z==minpnts.z) {
					bmin=i;
					break;
				}
			}
			for (i=0;i<npnts;i++) {
				if (intpnts[i].z==maxpnts.z) {
					bmax=i;
					break;
				}
			}
		}
		if (bmin<0 || bmax<0)
			errorexit("this error just shouldn't happen");
		res[0]=intpnts[bmin];
		res[1]=intpnts[bmax];
	}
	delu.x=tr0[1].x-tr0[0].x;
	delu.y=tr0[1].y-tr0[0].y;
	delu.z=tr0[1].z-tr0[0].z;
	delv.x=tr0[2].x-tr0[0].x;
	delv.y=tr0[2].y-tr0[0].y;
	delv.z=tr0[2].z-tr0[0].z;
	cross3d(&delu,&delv,&nrm0);
	delu.x=tr1[1].x-tr1[0].x;
	delu.y=tr1[1].y-tr1[0].y;
	delu.z=tr1[1].z-tr1[0].z;
	delv.x=tr1[2].x-tr1[0].x;
	delv.y=tr1[2].y-tr1[0].y;
	delv.z=tr1[2].z-tr1[0].z;
	cross3d(&delu,&delv,&nrm1);
	cross3d(&nrm0,&nrm1,&crs);
	del.x=res[1].x-res[0].x;
	del.y=res[1].y-res[0].y;
	del.z=res[1].z-res[0].z;
	if (dot3d(&crs,&del)<0) {
		t=res[0];
		res[0]=res[1];
		res[1]=t;
	}
	return 1;
}

// returns pairs of points (line segs)
static int pntplnidx[6][3]={
	{0,1,2},
	{4,6,5},
	{0,4,1},
	{2,3,6},
	{0,2,4},
	{1,5,3},
};

int box2box(VEC *b0,VEC *b1,VEC *res)
{
	int i,j,k;
	int npnts=0;
//	VEC b0max,b1max,b0min,b1min;
	VEC pln0[3],pln1[3];
/*	b0max=b0min=b0[0];
	for (i=1;i<NCORNERS;i++) {
		if (b0[i].x>b0max.x)
			b0max.x=b0[i].x;
		if (b0[i].y>b0max.y)
			b0max.y=b0[i].y;
		if (b0[i].z>b0max.z)
			b0max.z=b0[i].z;
		if (b0[i].x<b0min.x)
			b0min.x=b0[i].x;
		if (b0[i].y<b0min.y)
			b0min.y=b0[i].y;
		if (b0[i].z<b0min.z)
			b0min.z=b0[i].z;
	}
	b1max=b1min=b1[0];
	for (i=1;i<NCORNERS;i++) {
		if (b1[i].x>b1max.x)
			b1max.x=b1[i].x;
		if (b1[i].y>b1max.y)
			b1max.y=b1[i].y;
		if (b1[i].z>b1max.z)
			b1max.z=b1[i].z;
		if (b1[i].x<b1min.x)
			b1min.x=b1[i].x;
		if (b1[i].y<b1min.y)
			b1min.y=b1[i].y;
		if (b1[i].z<b1min.z)
			b1min.z=b1[i].z;
	}
	if (b0min.x>b1max.x || b0min.y>b1max.y || b0min.z>b1max.z)
		return 0;
	if (b1min.x>b0max.x || b1min.y>b0max.y || b1min.z>b0max.z)
		return 0;
*/	for (j=0;j<6;j++) {
		for (k=0;k<3;k++)
			pln1[k]=b1[pntplnidx[j][k]];
		for (i=0;i<6;i++) {
			for (k=0;k<3;k++)
				pln0[k]=b0[pntplnidx[i][k]];
			if (plane2plane(pln0,pln1,&res[npnts]))
				npnts+=2;
		}
	}
	if (npnts<4 && npnts>0) {
//		logger("box2box with <4 points %d\n",npnts);
		return 0;
	}
	return npnts;
}

int box2plane(VEC *b,VEC *planep,VEC *planenorm,VEC *res)
{
	int i,k;
	int npnts=0;
	VEC pln0[3];
	for (i=0;i<6;i++) {
		for (k=0;k<3;k++)
			pln0[k]=b[pntplnidx[i][k]];
		if (plane2infplane(pln0,planep,planenorm,&res[npnts]))
			npnts+=2;
		}
	if (npnts<6 && npnts>0) {
//		logger("box2plane with <6 points %d\n",npnts);
		return 0;
	}
	return npnts;
}
////////// end handy utils

} // end namespace physics3d
