#include <m_eng.h>
///////// quaternion section
void rotaxis2quat(struct pointf3 *ra,struct pointf3 *q)
{
	float a;
	float sina,cosa;
// convert to unit quat
	if (ra->w>10000 ||ra->w<-10000)
		errorexit("big angle in rotaxis2quat %f",ra->w);
	while(ra->w<=PI)
		ra->w+=TWOPI;
	while (ra->w>PI)
		ra->w-=TWOPI;
	a=.5f*ra->w;
	sina=(float)sin(a);
	cosa=(float)cos(a);
	q->x=ra->x*sina;
	q->y=ra->y*sina;
	q->z=ra->z*sina;
	q->w=cosa;
}

void quat2rotaxis(const struct pointf3 *q,struct pointf3 *ra) // angle will be 0 to PI/2
{
	float sina;
	float w2;
	w2=q->w*q->w;
	if (w2>1)
		w2=1;	// fixed 6_6_00 (intel compiler)
	sina=(float)sqrt(1-w2);
	nanerr("sina",sina);
	if (sina>EPSILON) {
		ra->w=acosf(q->w)*2.0f; // check
		nanerr("raw",ra->w);
		if (ra->w<0)
			sina=-sina;
		sina=1/sina;
		ra->x=q->x*sina;
		ra->y=q->y*sina;
		ra->z=q->z*sina;
		nanerr("rax",ra->x);
		nanerr("ray",ra->y);
//		nanerr("raz",ra->z);
		if (ra->w>PI)
			ra->w-=2.0f*PI;
		if (ra->w<0) {
			ra->w=-ra->w;
			ra->x=-ra->x;
			ra->y=-ra->y;
			ra->z=-ra->z;
		}
	} else {
		ra->x=0;
		ra->y=1;
		ra->z=0;
		ra->w=0;
	}
}

void quat2xformnotrans(const struct pointf3 *q,struct mat4 *m) // rotaxis
{
//	struct pointf3 q;
	float xy,xz,xw,yz,yw,zw,x2,y2,z2,w2;
// convert to unit quat
//	rotaxis2quat(ra,&q);
// build matrix
	x2=2*q->x*q->x;
	xy=2*q->x*q->y;
	xz=2*q->x*q->z;
	xw=2*q->x*q->w;
	y2=2*q->y*q->y;
	yz=2*q->y*q->z;
	yw=2*q->y*q->w;
	z2=2*q->z*q->z;
	zw=2*q->z*q->w;
	w2=2*q->w*q->w;
	m->e[0][0]=1-y2-z2;
	m->e[1][0]=xy-zw;
	m->e[2][0]=xz+yw;
	m->e[0][1]=xy+zw;
	m->e[1][1]=1-x2-z2;
	m->e[2][1]=yz-xw;
	m->e[0][2]=xz-yw;
	m->e[1][2]=yz+xw;
	m->e[2][2]=1-x2-y2;
	m->e[0][3]=0;
	m->e[1][3]=0;
	m->e[2][3]=0;
	m->e[3][3]=1;
}

void quat2xform(const struct pointf3 *q,struct mat4 *m) // rotaxis
{
	quat2xformnotrans(q,m);
	m->e[3][0]=0;
	m->e[3][1]=0;
	m->e[3][2]=0;
}

void quattrans2xform(const struct pointf3 *q,const struct pointf3 *t,struct mat4 *m) // rotaxis
{
	quat2xformnotrans(q,m);
	m->e[3][0]=t->x;
	m->e[3][1]=t->y;
	m->e[3][2]=t->z;
}

//void quatscalerottrans3d(struct tree *t,struct mat4 *m)
void quatscalerottrans3d(const struct pointf3 *s,const struct pointf3 *q,const struct pointf3 *t,struct mat4 *m)
{
//	struct keyframe **kp;
	if (1) { // uncomment and collisions work right in state 34
//	if (!t->seqs) { // assume if no keys whatsoever, then it's manualy built.. like shadownull
		quat2xformnotrans(q,m);
		m->e[3][0]=t->x;
		m->e[3][1]=t->y;
		m->e[3][2]=t->z;
		premulscale3d(s,m,m);
		return;
	}
//	kp=t->seqs[t->curseq].keys;
//	if (kp[ROTX])
		quat2xformnotrans(t,m);
//	if (kp[TRANSX]) {
		m->e[3][0]=t->x;
		m->e[3][1]=t->y;
		m->e[3][2]=t->z;
//	}
// pre mul scale
//	if (kp[SCLX])
		premulscale3d(s,m,m);
}

void quattimes(const struct pointf3 *a,const struct pointf3 *b,struct pointf3 *c)
{
	struct pointf3 t;
	t.x= a->x*b->w + a->y*b->z - a->z*b->y + a->w*b->x;
	t.y=-a->x*b->z + a->y*b->w + a->z*b->x + a->w*b->y;
	t.z= a->x*b->y - a->y*b->x + a->z*b->w + a->w*b->z;
	t.w=-a->x*b->x - a->y*b->y - a->z*b->z + a->w*b->w;
	*c=t;
}

void quatinverse(const struct pointf3 *a,struct pointf3 *b) // unit quats
{
	b->x=-a->x;
	b->y=-a->y;
	b->z=-a->z;
	b->w=a->w;
}

void quatneg(const struct pointf3 *a,struct pointf3 *b)
{
	b->x=-a->x;
	b->y=-a->y;
	b->z=-a->z;
	b->w=-a->w;
}

void quatnormalize(const struct pointf3 *a,struct pointf3 *b) // make quat a into unit quat b
{
	float r;
	r=a->x*a->x+a->y*a->y+a->z*a->z+a->w*a->w;
	if (r<EPSILON) {
		*b=zerov;
		b->w=1;
	} else {
		r=1/(float)sqrt(r);
		b->x=r*a->x;
		b->y=r*a->y;
		b->z=r*a->z;
		b->w=r*a->w;
	}
}

// pitch -90 to 90, yaw -180 to 180, roll -180 to 180, in radians
// uncomment last few lines for pitch -180 to 180, yaw -180 to 180, roll -90 to 90
void quat2rpy(const struct pointf3 *q,struct pointf3 *rpy)
{
	struct mat4 m;
	float ps;
	quat2xformnotrans(q,&m);
	ps=-m.e[2][1];
	if (ps>.9999f) {
		rpy->z=0;
		rpy->x=PI/2.0f;
		rpy->y=(float)atan2(m.e[1][0],m.e[0][0]);
	} else if (ps<-.9999f) {
		rpy->z=0;
		rpy->x=-PI/2.0f;
		rpy->y=-(float)atan2(m.e[1][0],m.e[0][0]);
	} else {
		rpy->x=asinf(ps);
		rpy->y=atan2f(m.e[2][0],m.e[2][2]);
		rpy->z=atan2f(m.e[0][1],m.e[1][1]);
	}
/*	if (rpy->z>90 || rpy->z<-90) {
		rpy->z+=180;
		rpy->y+=180;
		rpy->x=180-rpy->x;
		if (rpy->x>180)
			rpy->x-=360;
		if (rpy->y>180)
			rpy->y-=360;
		if (rpy->z>180)
			rpy->z-=360;
	}
*/
}

void rpy2quat(const struct pointf3 *rpy,struct pointf3 *q)
{
	float r,p,y;
	struct pointf3 rq,pq,yq,tq;
	r=rpy->z*.5f;
	p=rpy->x*.5f;
	y=rpy->y*.5f;
	rq.x=0;
	rq.y=0;
	rq.z=(float)sin(r);
	rq.w=(float)cos(r);
	pq.x=(float)sin(p);
	pq.y=0;
	pq.z=0;
	pq.w=(float)cos(p);
	yq.x=0;
	yq.y=(float)sin(y);
	yq.z=0;
	yq.w=(float)cos(y);
	quattimes(&yq,&pq,&tq);
	quattimes(&tq,&rq,q);
}

void quatinterp(const struct pointf3 *a,const struct pointf3 *b,float t,struct pointf3 *c)
{
	struct pointf3 iab,ra;
//	if (t<=0) {
//		*c=*a;
//		return;
//	}
//	if (t>=1) {
//		*c=*b;
//		return;
//	}
	quatinverse(a,&iab);
	quattimes(&iab,b,&iab);
	quat2rotaxis(&iab,&ra);
	ra.w*=t;
	rotaxis2quat(&ra,&iab);
	quattimes(a,&iab,c);
	quatnormalize(c,c);
}

// standard slerp, i like mine better
void quatinterp2(const struct pointf3 *qa,const struct pointf3 *qb,float t,struct pointf3 *qc)
{
	float a,wa,wb;
	struct pointf3 tqb;
	a=qa->x*qb->x+qa->y*qb->y+qa->z*qb->z+qa->w*qb->w;
	if (a<0)
		quatneg(qb,&tqb);
	else
		tqb=*qb;
	if (a>.999f || a<-.999f) {
		wa=1-t;
		wb=t;
	} else {
		a=acosf(a);
		wa=sinf((1-t)*a);
		wb=sinf(t*a);
		a=sinf(a);
		wa/=a;
		wb/=a;
	}
	qc->x=wa*qa->x+wb*tqb.x;
	qc->y=wa*qa->y+wb*tqb.y;
	qc->z=wa*qa->z+wb*tqb.z;
	qc->w=wa*qa->w+wb*tqb.w;
	quatnormalize(qc,qc);
}

void quatrot(const struct pointf3 *q,const struct pointf3 *vi,struct pointf3 *vo)
{
	struct pointf3 qi;
	pointf3 vi2 = *vi;
	vi2.w=0;
	quatinverse(q,&qi);
	quattimes(q,&vi2,vo);
	quattimes(vo,&qi,vo);
}

void quatrots(const struct pointf3 *q,const struct pointf3 *vi,struct pointf3 *vo,U32 npnts)
{
	U32 i;
	for (i=0;i<npnts;i++)
		quatrot(q,&vi[i],&vo[i]);
}

////////////// end quaternion section

// one more, good for cheesy shadows, quatrot(quat,up,norm) calcs quat given up and norm
void normal2quat(const pointf3* up,const pointf3* norm,pointf3* quat) // if 180 upside down, flip in x
{
	float d;
	d=dot3d(up,norm);
	pointf3 ra;
	cross3d(up,norm,&ra);
	if (normalize3d(&ra,&ra)<.001f) {
//		if (norm->y<0) { // 180 on x
		if (d<0) { // 180 on x
			quat->w=quat->y=quat->z=0;
			quat->x=1;
		} else {
			quat->x=quat->y=quat->z=0;
			quat->w=1;
		}
	} else {
		ra.w=racos(d);
		rotaxis2quat(&ra,quat);
	}
}

// one more good for cheesy edge of road deflections
//#define VERBOSEN2QY
void normal2quaty(const pointf3* up,const pointf3* norm,pointf3* quat)  // if 180 upside down, flip in y
{
	float d;
	d=dot3d(up,norm);
#ifdef VERBOSEN2QY
	string ds = float2strhex(d);
	logger("d '%s'\n",ds.c_str());
#endif
	cross3d(up,norm,quat);
#ifdef VERBOSEN2QY
	string qx = float2strhex(quat->x);
	string qy = float2strhex(quat->y);
	string qz = float2strhex(quat->z);
	logger("q '%s', '%s', '%s'\n",qx.c_str(),qy.c_str(),qz.c_str());
#endif
	if (normalize3d(quat,quat)<.001f) {
//		if (norm->y<0) { // 180 on y
		if (d<0) { // 180 on y
			quat->w=quat->x=quat->z=0;
			quat->y=1;
		} else {
			quat->x=quat->y=quat->z=0;
			quat->w=1;
		}
	} else {
#ifdef VERBOSEN2QY
		qx = float2strhex(quat->x);
		qy = float2strhex(quat->y);
		qz = float2strhex(quat->z);
#endif
		quat->w=racos(d);
#ifdef VERBOSEN2QY
		string qw = float2strhex(quat->w);
		logger("qn '%s', '%s', '%s' '%s'\n",qx.c_str(),qy.c_str(),qz.c_str(),qw.c_str());
#endif
		rotaxis2quat(quat,quat);
#ifdef VERBOSEN2QY
		qx = float2strhex(quat->x);
		qy = float2strhex(quat->y);
		qz = float2strhex(quat->z);
		qw = float2strhex(quat->w);
		logger("quat '%s', '%s', '%s' '%s'\n",qx.c_str(),qy.c_str(),qz.c_str(),qw.c_str());
#endif
	}
}
