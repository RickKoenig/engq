#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <m_eng.h>

#include "u_s_physics3d.h"
//#include "user52c.h"
//#include "user52.h"
#include "primcoll.h"

//#include "testnulls.h"

static int paidx[MAXF];
static int pbidx[MAXF];
static int pcnts[MAXF];
static int iptrs[MAXV+1];
static int visited;

static int lineline(VEC *a0,VEC *a1,VEC *b0,VEC *b1,VEC *ct,VEC *norm)
{
	float aa,bb,ab,d,t,u;
	VEC ad,bd,abd,r0,r1;
	ad.x=a1->x-a0->x;
	ad.y=a1->y-a0->y;
	ad.z=a1->z-a0->z;
	bd.x=b1->x-b0->x;
	bd.y=b1->y-b0->y;
	bd.z=b1->z-b0->z;
	aa=dot3d(&ad,&ad);
	bb=dot3d(&bd,&bd);
	ab=dot3d(&ad,&bd);
	d=aa*bb-ab*ab;
	if (d<EPSILON && d>-EPSILON)
		return 0;
	d=1.0f/d;
	abd.x=b0->x-a0->x;
	abd.y=b0->y-a0->y;
	abd.z=b0->z-a0->z;
	t=d*(dot3d(&abd,&ad)*bb-dot3d(&abd,&bd)*ab);
	u=d*(dot3d(&abd,&ad)*ab-dot3d(&abd,&bd)*aa);
	r0.x=a0->x+t*ad.x;
	r0.y=a0->y+t*ad.y;
	r0.z=a0->z+t*ad.z;
	r1.x=b0->x+u*bd.x;
	r1.y=b0->y+u*bd.y;
	r1.z=b0->z+u*bd.z;
	ct->x=.5f*(r0.x+r1.x);
	ct->y=.5f*(r0.y+r1.y);
	ct->z=.5f*(r0.z+r1.z);
	cross3d(&ad,&bd,norm);
	if (normalize3d(norm,norm)==0)
		return 0;
	return 1;
}

/*static void addbigtri(VEC *v0,VEC *v1,VEC *v2)
{
	VEC ct;
	VEC vts[3];
	ct.x=(1.0f/3.0f)*(v0->x+v1->x+v2->x);
	ct.y=(1.0f/3.0f)*(v0->y+v1->y+v2->y);
	ct.z=(1.0f/3.0f)*(v0->z+v1->z+v2->z);
	vts[0].x=ct.x+(v0->x-ct.x)*1.5f;
	vts[0].y=ct.y+(v0->y-ct.y)*1.5f;
	vts[0].z=ct.z+(v0->z-ct.z)*1.5f;
	vts[1].x=ct.x+(v1->x-ct.x)*1.5f;
	vts[1].y=ct.y+(v1->y-ct.y)*1.5f;
	vts[1].z=ct.z+(v1->z-ct.z)*1.5f;
	vts[2].x=ct.x+(v2->x-ct.x)*1.5f;
	vts[2].y=ct.y+(v2->y-ct.y)*1.5f;
	vts[2].z=ct.z+(v2->z-ct.z)*1.5f;
	addtri(vts);
}
*/
void checkw(struct phyobject *p)
{
	int j;
	if (!p->haswf) {
		for (j=0;j<p->nwpnts;j++) {
			p->wpnts[j].x=p->scale.x*p->lpnts[j].x;
			p->wpnts[j].y=p->scale.y*p->lpnts[j].y;
			p->wpnts[j].z=p->scale.z*p->lpnts[j].z;
		}
		quatrots(&p->st.rot,p->wpnts,p->wpnts,p->nwpnts);
		for (j=0;j<p->nwpnts;j++) {
			p->wpnts[j].x+=p->st.pos.x;
			p->wpnts[j].y+=p->st.pos.y;
			p->wpnts[j].z+=p->st.pos.z;
		}
	}
}

float meshplane(struct phyobject *p,VEC *ploc,VEC *pnorm,VEC *cloc)
{
	float dot;
	int cnt=0;
	float sep,minsep=1e20f;
	int i;
	int badcnt=0;
	float pdot=dot3d(pnorm,ploc);
	checkw(p);
	*cloc=zerov;
	for (i=0;i<p->nwpnts;i++) {
		dot=dot3d(pnorm,&p->wpnts[i]);
		sep=dot-pdot;
		if (sep<minsep)
			minsep=sep;
		if (sep<=0) {
			cloc->x+=p->wpnts[i].x-sep*pnorm->x;
			cloc->y+=p->wpnts[i].y-sep*pnorm->y;
			cloc->z+=p->wpnts[i].z-sep*pnorm->z;
			badcnt++;
		}
	}
	if (minsep<=0) {
		cloc->x/= badcnt;
		cloc->y/= badcnt;
		cloc->z/= badcnt;
	}
	return minsep;
}

static float meshplanecount(struct phyobject *p,VEC *ploc,VEC *pnorm,VEC *cloc,int *badcnt)
{
	float dot;
	float sep,minsep=1e20f;
	int i;
	float pdot=dot3d(pnorm,ploc);
	checkw(p);
	*badcnt=0;
	*cloc=zerov;
	for (i=0;i<p->nwpnts;i++) {
		dot=dot3d(pnorm,&p->wpnts[i]);
		sep=dot-pdot;
		if (sep<minsep)
			minsep=sep;
		if (sep<=0) {
			(*badcnt)++;
			cloc->x+=p->wpnts[i].x-minsep*pnorm->x;
			cloc->y+=p->wpnts[i].y-minsep*pnorm->y;
			cloc->z+=p->wpnts[i].z-minsep*pnorm->z;
		}
	}
	if (minsep<=0) {
		cloc->x/= *badcnt;
		cloc->y/= *badcnt;
		cloc->z/= *badcnt;
	}
	return minsep;
}

#if 0
static float checkedge(struct phyobject *a,struct phyobject *b,struct contact *ct,VEC *onorm,VEC *ctr)
{
	VEC chknorm;
	float sepa,sepb;
	VEC cloc;

	if (!lineline(ct->ae0,ct->ae1,ct->be0,ct->be1,ctr,onorm))
		return -1e20f;
	chknorm.x=ctr->x-b->st.pos.x;
	chknorm.y=ctr->y-b->st.pos.y;
	chknorm.z=ctr->z-b->st.pos.z;
	if (dot3d(&chknorm,onorm)<0) {
		onorm->x=-onorm->x;
		onorm->y=-onorm->y;
		onorm->z=-onorm->z;
	}
	sepa=meshplane(a,ctr,onorm,&cloc); // check points on a
	if (sepa>0) {
		onorm->x=-onorm->x;
		onorm->y=-onorm->y;
		onorm->z=-onorm->z;
		sepb=meshplane(b,ctr,onorm,&cloc); // check points on b
		if (sepb>0)
			return sepa+sepb;
		else
			return sepb;
	} else
		return sepa;
}
#endif

VEC testsvec;
int testiter=12;
float meshmesh(struct phyobject *pa,struct phyobject *pb,struct contact *ct,VEC *onorm,VEC *cloc)
{
	VEC *p,*q;
	VEC svec,nsvec,delv;
	int i,k;
	int pl,ql;
	int p0=0,q0=0,pn,qn;
	float dot,dote;
	if (!normalize3d(&testsvec,&testsvec)) {
		testsvec.x=0;
		testsvec.y=1;
		testsvec.z=0;
	}
	svec=testsvec;
	checkw(pa);
	checkw(pb);
	p=pa->wpnts;
	q=pb->wpnts;
	//addvec(&p[p0],&svec,rgbwhite);
	nsvec.x=-svec.x;
	nsvec.y=-svec.y;
	nsvec.z=-svec.z;
	//addvec(&q[q0],&nsvec,rgbwhite);
	for (k=0;k<testiter;k++) {

		pl=p0;
		ql=q0;
		visited++;
		dot=dot3d(&p[p0],&svec);
		pa->nbs[p0].visited=visited;
		while(1) {
			for (i=0;i<pa->nbs[p0].nnb;i++) {
				pn=pa->nbs[p0].nbs[i];
				if (pa->nbs[pn].visited!=visited) {
					pa->nbs[pn].visited=visited;
					dote=dot3d(&svec,&p[pn]);
					if (dote>dot)
						break;
				}
			}
			if (i==pa->nbs[p0].nnb)
				break;
			p0=pn;
			dot=dote;
		}

		visited++;
		dot=dot3d(&q[q0],&svec);
		pb->nbs[q0].visited=visited;
		while(1) {
			for (i=0;i<pb->nbs[q0].nnb;i++) {
				qn=pb->nbs[q0].nbs[i];
				if (pb->nbs[qn].visited!=visited) {
					pb->nbs[qn].visited=visited;
					dote=dot3d(&svec,&q[qn]);
					if (dote<dot)
						break;
				}
			}
			if (i==pb->nbs[q0].nnb)
				break;
			q0=qn;
			dot=dote;
		}
		if (q0==ql && p0==pl)
			break;
		delv.x=q[q0].x-p[p0].x;
		delv.y=q[q0].y-p[p0].y;
		delv.z=q[q0].z-p[p0].z;
		cross3d(&delv,&svec,&svec);
		cross3d(&svec,&delv,&svec);
		normalize3d(&svec,&svec);
	}

	nsvec.x=svec.x*2;
	nsvec.y=svec.y*2;
	nsvec.z=svec.z*2;
	//addvec(&p[p0],&nsvec,rgbmagenta);
	nsvec.x=-nsvec.x;
	nsvec.y=-nsvec.y;
	nsvec.z=-nsvec.z;
	//addvec(&q[q0],&nsvec,rgbmagenta);
	//norm?
	//cloc?
	//ct?
//	if (k!=testiter) { // no collide
// for now do all fv, ee vv nearest point checks, prune later
//	}
// collide (maybe)
	{
		float bestsep,cursep;
		VEC bestp0,bestq0,pv,qv;
		VEC fc[3];
// point to point
		bestp0=p[p0];
		bestq0=q[q0];
		bestsep=dist3dsq(&bestp0,&bestq0);
// face p to point q
		qv=pb->wpnts[q0];
		for (i=0;i<pa->nbfs[p0].nnbf;i++) {
			k=pa->nbfs[p0].nbfs[i];
			fc[0]=pa->wpnts[pa->lfaces[k].vertidx[0]];
			fc[1]=pa->wpnts[pa->lfaces[k].vertidx[1]];
			fc[2]=pa->wpnts[pa->lfaces[k].vertidx[2]];
			point2face(fc,&qv,&pv);
			cursep=dist3dsq(&pv,&qv);
			if (cursep<bestsep) {
				bestp0=pv;
				bestq0=qv;
				bestsep=cursep;
			}
		}
// face q to point p
		pv=pa->wpnts[p0];
		for (i=0;i<pb->nbfs[q0].nnbf;i++) {
			k=pb->nbfs[q0].nbfs[i];
			fc[0]=pb->wpnts[pb->lfaces[k].vertidx[0]];
			fc[1]=pb->wpnts[pb->lfaces[k].vertidx[1]];
			fc[2]=pb->wpnts[pb->lfaces[k].vertidx[2]];
			point2face(fc,&pv,&qv);
			cursep=dist3dsq(&pv,&qv);
			if (cursep<bestsep) {
				bestp0=pv;
				bestq0=qv;
				bestsep=cursep;
			}
		}
// edge to edge

		//addline(&bestp0,&bestq0,rgblightgreen);
		cloc->x=.5f*(bestp0.x+bestq0.x);
		cloc->y=.5f*(bestp0.y+bestq0.y);
		cloc->z=.5f*(bestp0.z+bestq0.z);
		onorm->x=bestp0.x-bestq0.x;
		onorm->y=bestp0.y-bestq0.y;
		onorm->z=bestp0.z-bestq0.z;
		normalize3d(onorm,onorm);
//		return 1;	// no collide
	}
//	return bestsep-EPSILON;
	return 1; // temp no collide (really is collide) return -1 or something
}

#if 0
float meshmesh(struct phyobject *pa,struct phyobject *pb,struct contact *ct,VEC *onorm,VEC *cloc)
{
	static int nextvert[3]={1,2,0};
	int i,j,ka,kb,m,cnt,maxcnt;
	VEC pa0,pa1;
	struct contact bct;
	float sep;
	float bestsep=-1e20f;
	int fca,fcb;
	FACE *f;
	VEC *v;
	checkw(pa);
	checkw(pb);
// check cache first
//	ct->p0=NULL; // opt: force plane cache miss
//	ct->ae0=NULL; // opt: force edge cache miss
	if (ct->p0) {
		addbigtri(ct->p0,ct->p1,ct->p2);
		pa0.x=ct->p1->x-ct->p0->x;
		pa0.y=ct->p1->y-ct->p0->y;
		pa0.z=ct->p1->z-ct->p0->z;
		pa1.x=ct->p2->x-ct->p0->x;
		pa1.y=ct->p2->y-ct->p0->y;
		pa1.z=ct->p2->z-ct->p0->z;
		cross3d(&pa0,&pa1,onorm);
		normalize3d(onorm,onorm);
		if (ct->planeonb) { // plane on obj b
			sep=meshplane(pa,ct->p0,onorm,cloc); // check points on a
			if (sep>0)
				return sep;
		} else {
			sep=meshplane(pb,ct->p0,onorm,cloc);
			if (sep>0) {
				onorm->x=-onorm->x;
				onorm->y=-onorm->y;
				onorm->z=-onorm->z;
				return sep;
			}
		}
	} else if (ct->ae0) { // check edge
		sep=checkedge(pa,pb,ct,onorm,cloc);
		addline(ct->ae0,ct->ae1,rgbred);
		addline(ct->be0,ct->be1,rgbred);
		addvec(cloc,onorm,rgbwhite);
		if (sep>0)
			return sep;
	}
// cache miss, try all planes on a
	v=pa->wpnts;
	ct->planeonb=0;
	memset(iptrs,0,sizeof(iptrs[0])*(pb->nwpnts+1));
	maxcnt=0;
	for (i=0;i<pa->nwfaces;i++) {
		f=&pa->lfaces[i];
		ct->p0=&v[f->vertidx[0]];
		ct->p1=&v[f->vertidx[1]];
		ct->p2=&v[f->vertidx[2]];
		pa0.x=ct->p1->x-ct->p0->x;
		pa0.y=ct->p1->y-ct->p0->y;
		pa0.z=ct->p1->z-ct->p0->z;
		pa1.x=ct->p2->x-ct->p0->x;
		pa1.y=ct->p2->y-ct->p0->y;
		pa1.z=ct->p2->z-ct->p0->z;
		cross3d(&pa0,&pa1,&ct->norm);
		normalize3d(&ct->norm,&ct->norm);
//		addbigtri(ct->p0,ct->p1,ct->p2);
		sep=meshplanecount(pb,ct->p0,&ct->norm,&ct->cloc,&cnt); // check points on b
		iptrs[cnt]++;
		pcnts[i]=cnt;
		if (cnt>maxcnt)
			maxcnt=cnt;
		if (sep>bestsep) {
			bct=*ct;
			bestsep=sep;
		}
	}
// insertion sort
	fca=0;
	cnt=0;
	for (i=0;i<=maxcnt;i++) {
		j=iptrs[i];
		iptrs[i]=cnt;
		cnt+=j;
	}
	for (i=0;i<pa->nwfaces;i++) {
		cnt=pcnts[i];
		if (cnt!=pb->nwpnts)
			fca++;
		j=iptrs[cnt];
		paidx[j]=i;
		j++;
		iptrs[cnt]=j;
	}
// try all planes on b
	v=pb->wpnts;
	ct->planeonb=1;
	memset(iptrs,0,sizeof(iptrs[0])*(pa->nwpnts+1));
	maxcnt=0;
	for (i=0;i<pb->nwfaces;i++) {
		f=&pb->lfaces[i];
		ct->p0=&v[f->vertidx[0]];
		ct->p1=&v[f->vertidx[1]];
		ct->p2=&v[f->vertidx[2]];
		pa0.x=ct->p1->x-ct->p0->x;
		pa0.y=ct->p1->y-ct->p0->y;
		pa0.z=ct->p1->z-ct->p0->z;
		pa1.x=ct->p2->x-ct->p0->x;
		pa1.y=ct->p2->y-ct->p0->y;
		pa1.z=ct->p2->z-ct->p0->z;
		cross3d(&pa0,&pa1,&ct->norm);
		normalize3d(&ct->norm,&ct->norm);
//		addbigtri(ct->p0,ct->p1,ct->p2);
		sep=meshplanecount(pa,ct->p0,&ct->norm,&ct->cloc,&cnt); // check points on a
		iptrs[cnt]++;
		pcnts[i]=cnt;
		if (cnt>maxcnt)
			maxcnt=cnt;
		if (sep>bestsep) {
			bct=*ct;
			bestsep=sep;
		}
	}
// insertion sort
	fcb=0;
	cnt=0;
	for (i=0;i<=maxcnt;i++) {
		j=iptrs[i];
		iptrs[i]=cnt;
		cnt+=j;
	}
	for (i=0;i<pb->nwfaces;i++) {
		cnt=pcnts[i];
		if (cnt!=pa->nwpnts)
			fcb++;
		j=iptrs[cnt];
		pbidx[j]=i;
		j++;
		iptrs[cnt]=j;
	}
	if (1) { // opt: best plane (even bad) gets cached, disables edges
//	if (0) { // opt: don't update cache with plane
//	if (bestsep>0) { // opt: only best plane (if any) gets cached
		*ct=bct;
		*cloc=ct->cloc;
		*onorm=ct->norm;
		if (ct->planeonb==0) {
			onorm->x=-onorm->x;
			onorm->y=-onorm->y;
			onorm->z=-onorm->z;
		}
		return bestsep;
	}
	// try edges
	ct->p0=NULL;
//	bestsep=-1e20f; // opt: ignore best planes
	i=-1;
	j=0;
//	logger("a has %d faces, b has %d faces\n",pa->nwfaces,pb->nwfaces);
/*	for (i=0;i<fca;i++) {
		struct face *f;
		VEC *v=pa->wpnts;
		f=&pa->lfaces[paidx[i]];
		addbigtri(&v[f->vertidx[0]],&v[f->vertidx[1]],&v[f->vertidx[2]]);
	}
	for (i=0;i<fcb;i++) {
		struct face *f;
		VEC *v=pb->wpnts;
		f=&pb->lfaces[pbidx[i]];
		addbigtri(&v[f->vertidx[0]],&v[f->vertidx[1]],&v[f->vertidx[2]]);
	}
*/
	while(1) {
		struct face *fa,*fb;
		VEC *va=pa->wpnts,*vb=pb->wpnts;
		if ((i<j || j>=fcb-1) && i<fca-1) {
			i++;
			fa=&pa->lfaces[paidx[i]];
			for (m=0;m<=j;m++) {
				fb=&pb->lfaces[pbidx[m]];
//				if (i==0 && j==0) {
//					addbigtri(&va[fa->vertidx[0]],&va[fa->vertidx[1]],&va[fa->vertidx[2]]);
//					addbigtri(&vb[fb->vertidx[0]],&vb[fb->vertidx[1]],&vb[fb->vertidx[2]]);
//				}
				for (ka=0;ka<3;ka++)
					for (kb=0;kb<3;kb++) {
						ct->ae0=&va[fa->vertidx[ka]];
						ct->ae1=&va[fa->vertidx[nextvert[ka]]];
						ct->be0=&vb[fb->vertidx[kb]];
						ct->be1=&vb[fb->vertidx[nextvert[kb]]];
						sep=checkedge(pa,pb,ct,&ct->norm,&ct->cloc);
						if (sep>bestsep) { // opt: find best edge
							bct=*ct;
							bestsep=sep;
						}
						if (sep>0) // opt: find first edge
							goto here;
					}
			}
		} else if (j<fcb-1) {
			j++;
			fb=&pb->lfaces[pbidx[j]];
			for (m=0;m<=i;m++) {
				fa=&pa->lfaces[paidx[m]];
				for (ka=0;ka<3;ka++)
					for (kb=0;kb<3;kb++) {
						ct->ae0=&va[fa->vertidx[ka]];
						ct->ae1=&va[fa->vertidx[nextvert[ka]]];
						ct->be0=&vb[fb->vertidx[kb]];
						ct->be1=&vb[fb->vertidx[nextvert[kb]]];
						sep=checkedge(pa,pb,ct,&ct->norm,&ct->cloc);
						if (sep>bestsep) { // opt: find best edge
							bct=*ct;
							bestsep=sep;
						}
						if (sep>0) // opt: find first edge
							goto here;
					}
			}
		} else
			break;
	}
here:
	*ct=bct;
	*cloc=ct->cloc;
	*onorm=ct->norm;
	return bestsep;
}
#endif
#if 0
static int v0,v1;
float meshmesh(struct phyobject *pa,struct phyobject *pb,struct contact *ct,VEC *onorm,VEC *cloc)
{
	static VEC desdir,testdir;
	int bestnv0,bestnv1;
	float bn,bd;
	int nv0,nv1;
	int j,k;
	VEC cp;
	float dot;
	extern int testiter;
	v0=0;
	v1=0;
	checkw(pa);
	checkw(pb);
	for (k=0;k<testiter;k++) {
		float aa,bb;
		cp.x=pb->wpnts[v1].x-pa->wpnts[v0].x;
		cp.y=pb->wpnts[v1].y-pa->wpnts[v0].y;
		cp.z=pb->wpnts[v1].z-pa->wpnts[v0].z;
		desdir.x=-cp.x;
		desdir.y=-cp.y;
		desdir.z=-cp.z;
		aa=dot3d(&desdir,&desdir);
		bn=0;
		bd=1;
		bestnv0=-1;
		for (j=0;j<pa->nbs[v0].nnb;j++) {
			nv0=pa->nbs[v0].nbs[j];
			nv1=v1;
			testdir.x=pb->wpnts[nv1].x-pa->wpnts[nv0].x-cp.x;
			testdir.y=pb->wpnts[nv1].y-pa->wpnts[nv0].y-cp.y;
			testdir.z=pb->wpnts[nv1].z-pa->wpnts[nv0].z-cp.z;
			dot=dot3d(&testdir,&desdir);
			if (dot>0) {
				float n,d;
				n=dot*dot;
				bb=dot3d(&testdir,&testdir);
				d=aa*bb;
				if (n*bd>bn*d) {
					bestnv0=nv0;
					bestnv1=nv1;
					bn=n;
					bd=d;
				}
			}
		}
		for (j=0;j<pb->nbs[v1].nnb;j++) {
			nv0=v0;
			nv1=pb->nbs[v1].nbs[j];
			testdir.x=pb->wpnts[nv1].x-pa->wpnts[nv0].x-cp.x;
			testdir.y=pb->wpnts[nv1].y-pa->wpnts[nv0].y-cp.y;
			testdir.z=pb->wpnts[nv1].z-pa->wpnts[nv0].z-cp.z;
			dot=dot3d(&testdir,&desdir);
			if (dot>0) {
				float n,d;
				n=dot*dot;
				bb=dot3d(&testdir,&testdir);
				d=aa*bb;
				if (n*bd>bn*d) {
					bestnv0=nv0;
					bestnv1=nv1;
					bn=n;
					bd=d;
				}
			}
		}
		if (bestnv0==-1)
			break;
		v0=bestnv0;
		v1=bestnv1;
	}
	addnull(&pa->wpnts[v0],NULL);
	addnull(&pb->wpnts[v1],NULL);
	return 1;
}
#endif
///////// closest point problems
void point2face(VEC *face,VEC *pnt,VEC *result)
{
	VEC norm;
	VEC p1mp0,p2mp1,p0mp2,p0mtp,n0,n1,n2,tpmp;
	float k,nsq;
	p1mp0.x=face[1].x-face[0].x;
	p1mp0.y=face[1].y-face[0].y;
	p1mp0.z=face[1].z-face[0].z;
	p2mp1.x=face[2].x-face[1].x;
	p2mp1.y=face[2].y-face[1].y;
	p2mp1.z=face[2].z-face[1].z;
	cross3d(&p1mp0,&p2mp1,&norm);

	cross3d(&p2mp1,&norm,&n0);
//	normalize3d(&n0,&n0);
//	addvec(&face[1],&n0,rgbwhite);
	if (dot3d(pnt,&n0)>dot3d(&face[1],&n0)) {
		if (dot3d(&p2mp1,pnt)>dot3d(&p2mp1,&face[2]))
			*result=face[2];
		else if (dot3d(&p2mp1,pnt)<dot3d(&p2mp1,&face[1]))
			*result=face[1];
		else {
			k=dot3d(&p2mp1,&p2mp1);
			if (k<EPSILON) {
				logger("degen edge 12\n");
				*result=face[2];
				return;
			}
			tpmp.x=pnt->x-face[1].x;
			tpmp.y=pnt->y-face[1].y;
			tpmp.z=pnt->z-face[1].z;
			k=dot3d(&tpmp,&p2mp1)/k;
			result->x=face[1].x+k*p2mp1.x;
			result->y=face[1].y+k*p2mp1.y;
			result->z=face[1].z+k*p2mp1.z;
		}
		return;
	}
	p0mp2.x=face[0].x-face[2].x;
	p0mp2.y=face[0].y-face[2].y;
	p0mp2.z=face[0].z-face[2].z;
	cross3d(&p0mp2,&norm,&n1);
//	normalize3d(&n1,&n1);
//	addvec(&face[2],&n1,rgbwhite);
	if (dot3d(pnt,&n1)>dot3d(&face[2],&n1)) {
		if (dot3d(&p0mp2,pnt)>dot3d(&p0mp2,&face[0]))
			*result=face[0];
		else if (dot3d(&p0mp2,pnt)<dot3d(&p0mp2,&face[2]))
			*result=face[2];
		else {
			k=dot3d(&p0mp2,&p0mp2);
			if (k<EPSILON) {
				logger("degen edge 02\n");
				*result=face[0];
				return;
			}
			tpmp.x=pnt->x-face[2].x;
			tpmp.y=pnt->y-face[2].y;
			tpmp.z=pnt->z-face[2].z;
			k=dot3d(&tpmp,&p0mp2)/k;
			result->x=face[2].x+k*p0mp2.x;
			result->y=face[2].y+k*p0mp2.y;
			result->z=face[2].z+k*p0mp2.z;
		}
		return;
	}

	cross3d(&p1mp0,&norm,&n2);
//	normalize3d(&n2,&n2);
//	addvec(&face[0],&n2,rgbwhite);
	if (dot3d(pnt,&n2)>dot3d(&face[0],&n2)) {
		if (dot3d(&p1mp0,pnt)>dot3d(&p1mp0,&face[1]))
			*result=face[1];
		else if (dot3d(&p1mp0,pnt)<dot3d(&p1mp0,&face[0]))
			*result=face[0];
		else {
			k=dot3d(&p1mp0,&p1mp0);
			if (k<EPSILON) {
				logger("degen edge 01\n");
				*result=face[1];
				return;
			}
			tpmp.x=pnt->x-face[0].x;
			tpmp.y=pnt->y-face[0].y;
			tpmp.z=pnt->z-face[0].z;
			k=dot3d(&tpmp,&p1mp0)/k;
			result->x=face[0].x+k*p1mp0.x;
			result->y=face[0].y+k*p1mp0.y;
			result->z=face[0].z+k*p1mp0.z;
		}
		return;
	}

// on the face
	nsq=dot3d(&norm,&norm);
	if (nsq<EPSILON) {
		logger("degen face\n");
		*result=face[0]; // for now
		return;
	}
	p0mtp.x=face[0].x-pnt->x;
	p0mtp.y=face[0].y-pnt->y;
	p0mtp.z=face[0].z-pnt->z;
	k=dot3d(&p0mtp,&norm)/nsq;
	result->x=norm.x*k+pnt->x;
	result->y=norm.y*k+pnt->y;
	result->z=norm.z*k+pnt->z;
}

void point2edge(VEC *edge,VEC *pnt,VEC *result)
{
	VEC p1mp0,tpmp0;
	float k,nsq;
	p1mp0.x=edge[1].x-edge[0].x;
	p1mp0.y=edge[1].y-edge[0].y;
	p1mp0.z=edge[1].z-edge[0].z;
	if (dot3d(pnt,&p1mp0)>dot3d(&edge[1],&p1mp0)) {
		*result=edge[1];
		return;
	}
	if (dot3d(pnt,&p1mp0)<dot3d(&edge[0],&p1mp0)) {
		*result=edge[0];
		return;
	}

	nsq=dot3d(&p1mp0,&p1mp0);
	if (nsq<EPSILON) {
		logger("degen point2edge\n");
		*result=edge[0]; // for now
		return;
	}
	tpmp0.x=pnt->x-edge[0].x;
	tpmp0.y=pnt->y-edge[0].y;
	tpmp0.z=pnt->z-edge[0].z;
	k=dot3d(&tpmp0,&p1mp0)/nsq;
	result->x=p1mp0.x*k+edge[0].x;
	result->y=p1mp0.y*k+edge[0].y;
	result->z=p1mp0.z*k+edge[0].z;
}

void edge2edge(VEC *edgep,VEC *edgeq,VEC *resultp,VEC *resultq)
{
	VEC p1mp0,q1mq0,p0mq0;
	float d,a,b;
	float ps,qs,pq,pqp,pqq;
	p1mp0.x=edgep[1].x-edgep[0].x;
	p1mp0.y=edgep[1].y-edgep[0].y;
	p1mp0.z=edgep[1].z-edgep[0].z;
	q1mq0.x=edgeq[1].x-edgeq[0].x;
	q1mq0.y=edgeq[1].y-edgeq[0].y;
	q1mq0.z=edgeq[1].z-edgeq[0].z;
	p0mq0.x=edgep[0].x-edgeq[0].x;
	p0mq0.y=edgep[0].y-edgeq[0].y;
	p0mq0.z=edgep[0].z-edgeq[0].z;
	ps=dot3d(&p1mp0,&p1mp0);
	qs=dot3d(&q1mq0,&q1mq0);
	pq=dot3d(&p1mp0,&q1mq0);
	d=pq*pq-ps*qs;
	if (d>-EPSILON && d<EPSILON) { // check for parallel lines
		VEC cp;
		float dp0,dp1,dq0,dq1,t;
		VEC *qsm,*qlg;
//		logger("degen edge2edge (parallel)\n");
		dp0=dot3d(&p1mp0,&edgep[0]);
		dp1=dot3d(&p1mp0,&edgep[1]);
		dq0=dot3d(&p1mp0,&edgeq[0]);
		dq1=dot3d(&p1mp0,&edgeq[1]);
		if (dq0>dq1) {
			t=dq0;
			dq0=dq1;
			dq1=t;
			qsm=&edgeq[1];
			qlg=&edgeq[0];
		} else {
			qlg=&edgeq[1];
			qsm=&edgeq[0];
		}
		if (dq1<dp0) {
			*resultp=edgep[0];
			*resultq=*qlg;
		} else if (dq1<dp1) {
			if (dq0<dp0) {
				cp.x=.5f*(edgep[0].x+qlg->x);
				cp.y=.5f*(edgep[0].y+qlg->y);
				cp.z=.5f*(edgep[0].z+qlg->z);
				point2edge(edgep,&cp,resultp);
				point2edge(edgeq,&cp,resultq);
			} else {
				cp.x=.5f*(qsm->x+qlg->x);
				cp.y=.5f*(qsm->y+qlg->y);
				cp.z=.5f*(qsm->z+qlg->z);
				point2edge(edgep,&cp,resultp);
				point2edge(edgeq,&cp,resultq);
			}
		} else {
			if (dq0<dp0) {
				cp.x=.5f*(edgep[0].x+edgep[1].x);
				cp.y=.5f*(edgep[0].y+edgep[1].y);
				cp.z=.5f*(edgep[0].z+edgep[1].z);
				point2edge(edgep,&cp,resultp);
				point2edge(edgeq,&cp,resultq);
			} else if (dq0<dp1) {
				cp.x=.5f*(edgep[1].x+qsm->x);
				cp.y=.5f*(edgep[1].y+qsm->y);
				cp.z=.5f*(edgep[1].z+qsm->z);
				point2edge(edgep,&cp,resultp);
				point2edge(edgeq,&cp,resultq);
			} else {
				*resultp=edgep[1];
				*resultq=*qsm;
			}
		}
		return;
	}
	d=1.0f/d;
	pqp=dot3d(&p0mq0,&p1mp0);
	pqq=dot3d(&p0mq0,&q1mq0);
	a=d*(pqp*qs-pq*pqq);
	b=d*(-ps*pqq+pq*pqp);
	if (a<=0) {
		if (b<=0) {
			point2edge(edgep,&edgeq[0],resultp);
			point2edge(edgeq,resultp,resultq);
		} else if (b>=1) {
			point2edge(edgep,&edgeq[1],resultp);
			point2edge(edgeq,resultp,resultq);
		} else {
			*resultp=edgep[0];
			point2edge(edgeq,resultp,resultq);
		}
	} else if (a>=1) {
		if (b<=0) {
			point2edge(edgep,&edgeq[0],resultp);
			point2edge(edgeq,resultp,resultq);
		} else if (b>=1) {
			point2edge(edgep,&edgeq[1],resultp);
			point2edge(edgeq,resultp,resultq);
		} else {
			*resultp=edgep[1];
			point2edge(edgeq,resultp,resultq);
		}
	} else {
		if (b<=0) {
			*resultq=edgeq[0];
			point2edge(edgep,resultq,resultp);
		} else if (b>=1) {
			*resultq=edgeq[1];
			point2edge(edgep,resultq,resultp);
		} else {
			resultp->x=edgep[0].x+a*p1mp0.x;
			resultp->y=edgep[0].y+a*p1mp0.y;
			resultp->z=edgep[0].z+a*p1mp0.z;
			resultq->x=edgeq[0].x+b*q1mq0.x;
			resultq->y=edgeq[0].y+b*q1mq0.y;
			resultq->z=edgeq[0].z+b*q1mq0.z;
		}
	}
}

