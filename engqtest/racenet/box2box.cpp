//#include <engine1.h>
//#include "box2box.h"
#define D2_3D
#include <m_eng.h>
#include "box2box.h"

static int pntplnidx[6][3]={
	{0,1,2},
	{4,6,5},
	{0,4,1},
	{2,3,6},
	{0,2,4},
	{1,5,3},
};

// returns pairs of points (line segs)
int getcolpoint(pointf3* pnts,int npnts,pointf3* cm,pointf3* loc,pointf3* norm)
{
	int i;
	float dsum=0,d;
	pointf3 p2o;
	pointf3 v[MAXBOX2BOX/2];
	pointf3 vc[MAXBOX2BOX/2];
	float da[MAXBOX2BOX/2];
	pointf3 crs;
	*loc=zerov;
	for (i=0;i<npnts;i+=2) {
		d=dist3dsq(&pnts[i],&pnts[i+1]);
		da[i>>1]=d;
		loc->x+=pnts[i].x*d;
		loc->y+=pnts[i].y*d;
		loc->z+=pnts[i].z*d;
		loc->x+=pnts[i+1].x*d;
		loc->y+=pnts[i+1].y*d;
		loc->z+=pnts[i+1].z*d;
		dsum+=2*d;
	}
	if (dsum<EPSILON)
		return 0;
	loc->x/=dsum;
	loc->y/=dsum;
	loc->z/=dsum; 
	npnts/=2;
	for (i=0;i<npnts;i++) {
		v[i].x=pnts[i*2+1].x-pnts[i*2].x;
		v[i].y=pnts[i*2+1].y-pnts[i*2].y;
		v[i].z=pnts[i*2+1].z-pnts[i*2].z;
		vc[i].x=pnts[i*2].x-loc->x;
		vc[i].y=pnts[i*2].y-loc->y;
		vc[i].z=pnts[i*2].z-loc->z;
	}
	*norm=zerov;
	for (i=0;i<npnts;i++) {
		cross3d(&vc[i],&v[i],&crs);
		norm->x+=crs.x*da[i];
		norm->y+=crs.y*da[i];
		norm->z+=crs.z*da[i];
	}
	d=normalize3d(norm,norm);
	if (d<EPSILON) {
//		logger("getcolpoint ret 0\n");
		return 0;
	}
	p2o.x=loc->x-cm->x;
	p2o.y=loc->y-cm->y;
	p2o.z=loc->z-cm->z;
	if (dot3d(&p2o,norm)>0) {
		norm->x=-norm->x;
		norm->y=-norm->y;
		norm->z=-norm->z;
	}
	return 1; 
}


// v0 v1 v2 and v0+(v1-v0)+(v2-v0)
static int line2bplane(pointf3* v0i, pointf3* v1i, pointf3* v2i, pointf3* top,pointf3* bot,pointf3* intsect)
{
	pointf3 uvec,vvec;
	pointf3 nrm;
	float topdn,botdn,pdn,pdi;
	float t,b0,b1;
	pointf3 np1,np2;
	pointf3 v0,v1,v2;//,cm;
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

// given 2 sets of 3 points (2 parellelgram planes), return 2 points that define line intersection
static int plane2plane(pointf3* pl0,pointf3* pl1,pointf3* res)
{
	int i,npnts=0;
	int bmin=-1,bmax=-1;
	int doxyz; // 0 x 1 y 2 z
	pointf3 maxpnts,minpnts;
	pointf3 intpnts[8];
	pointf3 pl03,pl13;
	pointf3 maxpl0,minpl0,maxpl1,minpl1;
	pointf3 delu,delv,nrm0,nrm1,crs,t,del;
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

int box2box(pointf3* b0,pointf3* b1,pointf3* res)
{
	int i,j,k;
	int npnts=0;
	pointf3 b0max,b1max,b0min,b1min;
	pointf3 pln0[3],pln1[3];
	b0max=b0min=b0[0];
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
	for (j=0;j<6;j++) {
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

//////// line to box
int line2box(pointf3* objpos,pointf3* objrot,pointf3* oabbmin,pointf3* oabbmax,pointf3* p0,pointf3* p1,pointf3* intsect)
{
	float t,tmin=0,tmax=1;
	pointf3 rp0,rp1,qi;
	if (dist3dsq(objpos,p0)>100.0f)
		return 0;
	quatinverse(objrot,&qi);
	rp0.x=p0->x-objpos->x;
	rp0.y=p0->y-objpos->y;
	rp0.z=p0->z-objpos->z;
	rp1.x=p1->x-objpos->x;
	rp1.y=p1->y-objpos->y;
	rp1.z=p1->z-objpos->z;
	quatrot(&qi,&rp0,&rp0);
	quatrot(&qi,&rp1,&rp1);
// x	
	if (rp0.x<oabbmin->x) {
		if (rp1.x<oabbmin->x)
			return 0;
		t=(oabbmin->x-rp0.x)/(rp1.x-rp0.x);
		if (t>tmin)
			tmin=t;
	} else {
		if (rp1.x<oabbmin->x) {
			t=(oabbmin->x-rp0.x)/(rp1.x-rp0.x);
			if (t<tmax)
				tmax=t;
		}
	}
	if (rp0.x>oabbmax->x) {
		if (rp1.x>oabbmax->x)
			return 0;
		t=(oabbmax->x-rp0.x)/(rp1.x-rp0.x);
		if (t>tmin)
			tmin=t;
	} else {
		if (rp1.x>oabbmax->x) {
			t=(oabbmax->x-rp0.x)/(rp1.x-rp0.x);
			if (t<tmax)
				tmax=t;
		}
	}
	if (tmin>tmax)
		return 0;
// y
	if (rp0.y<oabbmin->y) {
		if (rp1.y<oabbmin->y)
			return 0;
		t=(oabbmin->y-rp0.y)/(rp1.y-rp0.y);
		if (t>tmin)
			tmin=t;
	} else {
		if (rp1.y<oabbmin->y) {
			t=(oabbmin->y-rp0.y)/(rp1.y-rp0.y);
			if (t<tmax)
				tmax=t;
		}
	}
	if (rp0.y>oabbmax->y) {
		if (rp1.y>oabbmax->y)
			return 0;
		t=(oabbmax->y-rp0.y)/(rp1.y-rp0.y);
		if (t>tmin)
			tmin=t;
	} else {
		if (rp1.y>oabbmax->y) {
			t=(oabbmax->y-rp0.y)/(rp1.y-rp0.y);
			if (t<tmax)
				tmax=t;
		}
	}
	if (tmin>tmax)
		return 0;
// z
	if (rp0.z<oabbmin->z) {
		if (rp1.z<oabbmin->z)
			return 0;
		t=(oabbmin->z-rp0.z)/(rp1.z-rp0.z);
		if (t>tmin)
			tmin=t;
	} else {
		if (rp1.z<oabbmin->z) {
			t=(oabbmin->z-rp0.z)/(rp1.z-rp0.z);
			if (t<tmax)
				tmax=t;
		}
	}
	if (tmin>tmax)
		return 0;
	if (rp0.z>oabbmax->z) {
		if (rp1.z>oabbmax->z)
			return 0;
		t=(oabbmax->z-rp0.z)/(rp1.z-rp0.z);
		if (t>tmin)
			tmin=t;
	} else {
		if (rp1.z>oabbmax->z) {
			t=(oabbmax->z-rp0.z)/(rp1.z-rp0.z);
			if (t<tmax)
				tmax=t;
		}
	}
	if (tmin>tmax)
		return 0;
	if (intsect) {
		interp3d(&rp0,&rp1,tmin,intsect);
		quatrot(objrot,intsect,intsect);
		intsect->x+=objpos->x;
		intsect->y+=objpos->y;
		intsect->z+=objpos->z;
	}
	return 1;
}

int box2box_v2(pointf3* objpos0,pointf3* objrot0,pointf3* oabbmin0,pointf3* oabbmax0,
			   pointf3* objpos1,pointf3* objrot1,pointf3* oabbmin1,pointf3* oabbmax1,
			   pointf3* loc,pointf3* norm)
{
	pointf3 pnts0[8];
	pointf3 pnts1[8];
	pointf3 resv[MAXBOX2BOX];
	int cp,i;
	pnts0[0].x=oabbmin0->x;	pnts0[0].y=oabbmin0->y;	pnts0[0].z=oabbmin0->z;
	pnts0[1].x=oabbmin0->x;	pnts0[1].y=oabbmin0->y;	pnts0[1].z=oabbmax0->z;
	pnts0[2].x=oabbmin0->x;	pnts0[2].y=oabbmax0->y;	pnts0[2].z=oabbmin0->z;
	pnts0[3].x=oabbmin0->x;	pnts0[3].y=oabbmax0->y;	pnts0[3].z=oabbmax0->z;
	pnts0[4].x=oabbmax0->x;	pnts0[4].y=oabbmin0->y;	pnts0[4].z=oabbmin0->z;
	pnts0[5].x=oabbmax0->x;	pnts0[5].y=oabbmin0->y;	pnts0[5].z=oabbmax0->z;
	pnts0[6].x=oabbmax0->x;	pnts0[6].y=oabbmax0->y;	pnts0[6].z=oabbmin0->z;
	pnts0[7].x=oabbmax0->x;	pnts0[7].y=oabbmax0->y;	pnts0[7].z=oabbmax0->z;

	pnts1[0].x=oabbmin1->x;	pnts1[0].y=oabbmin1->y;	pnts1[0].z=oabbmin1->z;
	pnts1[1].x=oabbmin1->x;	pnts1[1].y=oabbmin1->y;	pnts1[1].z=oabbmax1->z;
	pnts1[2].x=oabbmin1->x;	pnts1[2].y=oabbmax1->y;	pnts1[2].z=oabbmin1->z;
	pnts1[3].x=oabbmin1->x;	pnts1[3].y=oabbmax1->y;	pnts1[3].z=oabbmax1->z;
	pnts1[4].x=oabbmax1->x;	pnts1[4].y=oabbmin1->y;	pnts1[4].z=oabbmin1->z;
	pnts1[5].x=oabbmax1->x;	pnts1[5].y=oabbmin1->y;	pnts1[5].z=oabbmax1->z;
	pnts1[6].x=oabbmax1->x;	pnts1[6].y=oabbmax1->y;	pnts1[6].z=oabbmin1->z;
	pnts1[7].x=oabbmax1->x;	pnts1[7].y=oabbmax1->y;	pnts1[7].z=oabbmax1->z;
	quatrots(objrot0,pnts0,pnts0,8);
	quatrots(objrot1,pnts1,pnts1,8);
	for (i=0;i<8;i++) {
		pnts0[i].x+=objpos0->x;
		pnts0[i].y+=objpos0->y;
		pnts0[i].z+=objpos0->z;
	}
	for (i=0;i<8;i++) {
		pnts1[i].x+=objpos1->x;
		pnts1[i].y+=objpos1->y;
		pnts1[i].z+=objpos1->z;
	}
	if (cp=box2box(pnts0,pnts1,resv))
		return getcolpoint(resv,cp,objpos0,loc,norm);
	return 0;
}
