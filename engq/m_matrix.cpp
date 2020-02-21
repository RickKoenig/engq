//#include <cmath>
#include <m_eng.h>
// do some matrix math
//#include "vidd3d.h"
#include "m_perf.h"

float mouse3doffx,mouse3doffy,mouse3doffz;

/////// matrix stuff
void logmat2(const struct mat2* m,const C8* name)
{
	S32 i,j;
	logger("--------- mat2 - %s -----------\n",name);
	for (j=0;j<2;++j) {
		for (i=0;i<2;++i)
			logger("%10.3g ",m->e[j][i]);
	logger("\n");
	}
}

float detmat2(const struct mat2 *m)
{
	return m->e[0][0]*m->e[1][1]-m->e[0][1]*m->e[1][0];
}

void mulmat2(const struct mat2 *a,const struct mat2 *b,struct mat2 *p) {
	struct mat2 t;
	t.e[0][0]=a->e[0][0]*b->e[0][0]+a->e[0][1]*b->e[1][0];
	t.e[0][1]=a->e[0][0]*b->e[0][1]+a->e[0][1]*b->e[1][1];

	t.e[1][0]=a->e[1][0]*b->e[0][0]+a->e[1][1]*b->e[1][0];
	t.e[1][1]=a->e[1][0]*b->e[0][1]+a->e[1][1]*b->e[1][1];
	*p=t;
}

U32 inversemat2(const struct mat2 *m,struct mat2 *inv)
{
	S32 i,j;
	struct mat2 cf,cft;
	float det,invdet;
	// first get the cofactor matrix
	cf.e[0][0]= m->e[1][1];
	cf.e[0][1]=-m->e[1][0];
	cf.e[1][0]=-m->e[0][1];
	cf.e[1][1]= m->e[0][0];
	// get the det
	det=cf.e[0][0]*m->e[0][0]+cf.e[0][1]*m->e[0][1];
	// transpose
	for (j=0;j<2;j++)
		for (i=0;i<2;i++)
			cft.e[j][i]=cf.e[i][j];
	if (det==0)
		return 0;
	invdet=1.0f/det;
//	logger("det = %f, invdet = %f\n",det,invdet);
// scale
//	for (i=0;i<4;i++)
//		inv->e[0][i]=cft.e[0][i]*invdet;
	for (j=0;j<2;j++)
		for (i=0;i<2;i++)
			inv->e[j][i]=cft.e[j][i]*invdet;
//	logger("sizeof mat2 is %d\n",sizeof(cft));
//	logger("cft = %f,%f,%f,%f\n",cft.e[0][0],cft.e[0][1],cft.e[1][0],cft.e[1][1]);
//	logger("invmat = %f,%f,%f,%f\n",inv->e[0][0],inv->e[0][1],inv->e[1][0],inv->e[1][1]);
	return 1;
}

void transposemat2(const struct mat2 *m,struct mat2 *mt)
{
	mat2 t;
	t.e[0][0]=m->e[0][0];	t.e[1][0]=m->e[0][1];
	t.e[0][1]=m->e[1][0];	t.e[1][1]=m->e[1][1];
	*mt=t;
}

void identmat2(struct mat2 *m)
{
	memset((U8* )m,0,sizeof(*m));
	m->e[0][0]=m->e[1][1]=1.0;
}

void randmat2(struct mat2 *m)
{
	S32 i;
	for (i=0;i<4;i++)
		m->e[0][i]=mt_frand()*4-2;
}

// vi * m = vo
void mulmatvec2(const struct mat2 *m,const struct pointf2 *vi,struct pointf2 *vo)
{
	struct pointf2 t;
	t.x=vi->x*m->e[0][0]+vi->y*m->e[1][0];
	t.y=vi->x*m->e[0][1]+vi->y*m->e[1][1];
	*vo=t;
}

/// mat3 //////
void comatrix3to2(const struct mat3 *m,struct mat2 *cm,S32 r,S32 c)
{
	S32 id,jd,is,js;
	for (js=jd=0;jd<2;jd++,js++) {
		if (js==r)
			js++;
		for (is=id=0;id<2;id++,is++) {
			if (is==c)
				is++;
			cm->e[jd][id]=m->e[js][is];
		}
	}

}

float detmat3(const struct mat3 *m)
{
	return	 m->e[0][0]*(m->e[1][1]*m->e[2][2]-m->e[1][2]*m->e[2][1])
			-m->e[0][1]*(m->e[1][0]*m->e[2][2]-m->e[1][2]*m->e[2][0])
			+m->e[0][2]*(m->e[1][0]*m->e[2][1]-m->e[1][1]*m->e[2][0]);
}

void mulmat3(const struct mat3 *a,const struct mat3 *b,struct mat3 *p) {
	struct mat3 t;
	t.e[0][0]=a->e[0][0]*b->e[0][0]+a->e[0][1]*b->e[1][0]+a->e[0][2]*b->e[2][0];
	t.e[0][1]=a->e[0][0]*b->e[0][1]+a->e[0][1]*b->e[1][1]+a->e[0][2]*b->e[2][1];
	t.e[0][2]=a->e[0][0]*b->e[0][2]+a->e[0][1]*b->e[1][2]+a->e[0][2]*b->e[2][2];

	t.e[1][0]=a->e[1][0]*b->e[0][0]+a->e[1][1]*b->e[1][0]+a->e[1][2]*b->e[2][0];
	t.e[1][1]=a->e[1][0]*b->e[0][1]+a->e[1][1]*b->e[1][1]+a->e[1][2]*b->e[2][1];
	t.e[1][2]=a->e[1][0]*b->e[0][2]+a->e[1][1]*b->e[1][2]+a->e[1][2]*b->e[2][2];

	t.e[2][0]=a->e[2][0]*b->e[0][0]+a->e[2][1]*b->e[1][0]+a->e[2][2]*b->e[2][0];
	t.e[2][1]=a->e[2][0]*b->e[0][1]+a->e[2][1]*b->e[1][1]+a->e[2][2]*b->e[2][1];
	t.e[2][2]=a->e[2][0]*b->e[0][2]+a->e[2][1]*b->e[1][2]+a->e[2][2]*b->e[2][2];
	*p=t;
}

U32 inversemat3(const struct mat3 *m,struct mat3 *inv)
{
	S32 i,j;
	float p=1.0;
	struct mat3 cf,cft;
	struct mat2 detmat;
	float det,invdet;
	// first get the cofactor matrix
	for (j=0;j<3;j++)
		for (i=0;i<3;i++) {
			comatrix3to2(m,&detmat,j,i);
			det=detmat2(&detmat);
			cf.e[j][i]=p*det;
			p*=-1.0;
	}
	// get the det3
	det=cf.e[0][0]*m->e[0][0]+cf.e[0][1]*m->e[0][1]+cf.e[0][2]*m->e[0][2];
	// transpose
	for (j=0;j<3;j++)
		for (i=0;i<3;i++)
			cft.e[j][i]=cf.e[i][j];
	if (det==0)
		return 0;
	invdet=1.0f/det;
	// scale
	for (j=0;j<3;j++)
		for (i=0;i<3;i++)
			inv->e[j][i]=cft.e[j][i]*invdet;
	return 1;
}

void transposemat3(const struct mat3 *m,struct mat3 *mt)
{
	mat3 t;
	t.e[0][0]=m->e[0][0];	t.e[1][0]=m->e[0][1];	t.e[2][0]=m->e[0][2];
	t.e[0][1]=m->e[1][0];	t.e[1][1]=m->e[1][1];	t.e[2][1]=m->e[1][2];
	t.e[0][2]=m->e[2][0];	t.e[1][2]=m->e[2][1];	t.e[2][2]=m->e[2][2];
	*mt=t;
}

void identmat3(struct mat3 *m)
{
	memset((U8* )m,0,sizeof(*m));
	m->e[0][0]=m->e[1][1]=m->e[2][2]=1.0;
}

void randmat3(struct mat3 *m)
{
	S32 i;
	for (i=0;i<9;i++)
		m->e[0][i]=mt_frand()*4-2;
}

// vi * m = vo
void mulmatvec3(const struct mat3 *m,const struct pointf3 *vi,struct pointf3 *vo)
{
	struct pointf3 t;
	t.x=vi->x*m->e[0][0]+vi->y*m->e[1][0]+vi->z*m->e[2][0];
	t.y=vi->x*m->e[0][1]+vi->y*m->e[1][1]+vi->z*m->e[2][1];
	t.z=vi->x*m->e[0][2]+vi->y*m->e[1][2]+vi->z*m->e[2][2];
	*vo=t;
}

/// mat4 //////
void logmat4(const struct mat4* m,const C8* name)
{
	S32 i,j;
	logger("--------- mat4 - %s -----------\n",name);
	for (j=0;j<4;++j) {
		for (i=0;i<4;++i)
			logger("%12.5g ",m->e[j][i]);
	logger("\n");
	}
}

void comatrix4to3(const struct mat4 *m,struct mat3 *cm,S32 r,S32 c)
{
	S32 id,jd,is,js;
	for (js=jd=0;jd<3;jd++,js++) {
		if (js==r)
			js++;
		for (is=id=0;id<3;id++,is++) {
			if (is==c)
				is++;
			cm->e[jd][id]=m->e[js][is];
		}
	}

}

float detmat4(const struct mat4 *m)
{
	return
			m->e[0][0]*(
				  m->e[1][1]*(
					m->e[2][2]*m->e[3][3]-m->e[2][3]*m->e[3][2]
				)-m->e[1][2]*(
					m->e[2][1]*m->e[3][3]-m->e[2][3]*m->e[3][1]
				)+m->e[1][3]*(
					m->e[2][1]*m->e[3][2]-m->e[2][2]*m->e[3][1]
				)
			)-m->e[0][1]*(
				  m->e[1][0]*(
					m->e[2][2]*m->e[3][3]-m->e[2][3]*m->e[3][2]
				)-m->e[1][2]*(
					m->e[2][0]*m->e[3][3]-m->e[2][3]*m->e[3][0]
				)+m->e[1][3]*(
					m->e[2][0]*m->e[3][2]-m->e[2][2]*m->e[3][0]
				)
			)+m->e[0][2]*(
				  m->e[1][0]*(
					m->e[2][1]*m->e[3][3]-m->e[2][3]*m->e[3][1]
				)-m->e[1][1]*(
					m->e[2][0]*m->e[3][3]-m->e[2][3]*m->e[3][0]
				)+m->e[1][3]*(
					m->e[2][0]*m->e[3][1]-m->e[2][1]*m->e[3][0]
				)
			)-m->e[0][3]*(
				  m->e[1][0]*(
					m->e[2][1]*m->e[3][2]-m->e[2][2]*m->e[3][1]
				)-m->e[1][1]*(
					m->e[2][0]*m->e[3][2]-m->e[2][2]*m->e[3][0]
				)+m->e[1][2]*(
					m->e[2][0]*m->e[3][1]-m->e[2][1]*m->e[3][0]
				)
			);
}

void mulmat4(const struct mat4 *a,const struct mat4 *b,struct mat4 *p)
{
	struct mat4 t;
	t.e[0][0]=a->e[0][0]*b->e[0][0]+a->e[0][1]*b->e[1][0]+a->e[0][2]*b->e[2][0]+a->e[0][3]*b->e[3][0];
	t.e[0][1]=a->e[0][0]*b->e[0][1]+a->e[0][1]*b->e[1][1]+a->e[0][2]*b->e[2][1]+a->e[0][3]*b->e[3][1];
	t.e[0][2]=a->e[0][0]*b->e[0][2]+a->e[0][1]*b->e[1][2]+a->e[0][2]*b->e[2][2]+a->e[0][3]*b->e[3][2];
	t.e[0][3]=a->e[0][0]*b->e[0][3]+a->e[0][1]*b->e[1][3]+a->e[0][2]*b->e[2][3]+a->e[0][3]*b->e[3][3];

	t.e[1][0]=a->e[1][0]*b->e[0][0]+a->e[1][1]*b->e[1][0]+a->e[1][2]*b->e[2][0]+a->e[1][3]*b->e[3][0];
	t.e[1][1]=a->e[1][0]*b->e[0][1]+a->e[1][1]*b->e[1][1]+a->e[1][2]*b->e[2][1]+a->e[1][3]*b->e[3][1];
	t.e[1][2]=a->e[1][0]*b->e[0][2]+a->e[1][1]*b->e[1][2]+a->e[1][2]*b->e[2][2]+a->e[1][3]*b->e[3][2];
	t.e[1][3]=a->e[1][0]*b->e[0][3]+a->e[1][1]*b->e[1][3]+a->e[1][2]*b->e[2][3]+a->e[1][3]*b->e[3][3];

	t.e[2][0]=a->e[2][0]*b->e[0][0]+a->e[2][1]*b->e[1][0]+a->e[2][2]*b->e[2][0]+a->e[2][3]*b->e[3][0];
	t.e[2][1]=a->e[2][0]*b->e[0][1]+a->e[2][1]*b->e[1][1]+a->e[2][2]*b->e[2][1]+a->e[2][3]*b->e[3][1];
	t.e[2][2]=a->e[2][0]*b->e[0][2]+a->e[2][1]*b->e[1][2]+a->e[2][2]*b->e[2][2]+a->e[2][3]*b->e[3][2];
	t.e[2][3]=a->e[2][0]*b->e[0][3]+a->e[2][1]*b->e[1][3]+a->e[2][2]*b->e[2][3]+a->e[2][3]*b->e[3][3];

	t.e[3][0]=a->e[3][0]*b->e[0][0]+a->e[3][1]*b->e[1][0]+a->e[3][2]*b->e[2][0]+a->e[3][3]*b->e[3][0];
	t.e[3][1]=a->e[3][0]*b->e[0][1]+a->e[3][1]*b->e[1][1]+a->e[3][2]*b->e[2][1]+a->e[3][3]*b->e[3][1];
	t.e[3][2]=a->e[3][0]*b->e[0][2]+a->e[3][1]*b->e[1][2]+a->e[3][2]*b->e[2][2]+a->e[3][3]*b->e[3][2];
	t.e[3][3]=a->e[3][0]*b->e[0][3]+a->e[3][1]*b->e[1][3]+a->e[3][2]*b->e[2][3]+a->e[3][3]*b->e[3][3];
	*p=t;
}

U32 inversemat4(const struct mat4 *m,struct mat4 *inv)
{
	S32 i,j;
	float p=1.0;
	struct mat4 cf,cft;
	struct mat3 detmat;
	float det,invdet;
	// first get the cofactor matrix
	for (j=0;j<4;j++) {
		for (i=0;i<4;i++) {
			comatrix4to3(m,&detmat,j,i);
			det=detmat3(&detmat);
			cf.e[j][i]=p*det;
			p*=-1.0;
		}
		p*=-1.0;
	}
	// get the det
	det=cf.e[0][0]*m->e[0][0]+cf.e[0][1]*m->e[0][1]+cf.e[0][2]*m->e[0][2]+cf.e[0][3]*m->e[0][3];
	// transpose
	for (j=0;j<4;j++)
		for (i=0;i<4;i++)
			cft.e[j][i]=cf.e[i][j];
	if (det==0)
		return 0;
	invdet=1.0f/det;
	// scale
	for (j=0;j<4;j++)
		for (i=0;i<4;i++)
			inv->e[j][i]=cft.e[j][i]*invdet;
	return 1;
}

void identmat4(struct mat4 *m)
{
	memset((U8* )m,0,sizeof(*m));
	m->e[0][0]=m->e[1][1]=m->e[2][2]=m->e[3][3]=1.0;
}

void randmat4(struct mat4 *m)
{
	S32 i;
	for (i=0;i<16;i++)
		m->e[0][i]=mt_frand()*4-2;
}

// vi * m = vo
void mulmatvec4(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo)
{
	struct pointf3 t;
	t.x=vi->x*m->e[0][0]+vi->y*m->e[1][0]+vi->z*m->e[2][0]+vi->w*m->e[3][0];
	t.y=vi->x*m->e[0][1]+vi->y*m->e[1][1]+vi->z*m->e[2][1]+vi->w*m->e[3][1];
	t.z=vi->x*m->e[0][2]+vi->y*m->e[1][2]+vi->z*m->e[2][2]+vi->w*m->e[3][2];
	t.w=vi->x*m->e[0][3]+vi->y*m->e[1][3]+vi->z*m->e[2][3]+vi->w*m->e[3][3];
	*vo=t;
}

void transposemat4(const struct mat4 *m,struct mat4 *mt)
{
	mat4 t;
	t.e[0][0]=m->e[0][0];	t.e[1][0]=m->e[0][1];	t.e[2][0]=m->e[0][2];	t.e[3][0]=m->e[0][3];
	t.e[0][1]=m->e[1][0];	t.e[1][1]=m->e[1][1];	t.e[2][1]=m->e[1][2];	t.e[3][1]=m->e[1][3];
	t.e[0][2]=m->e[2][0];	t.e[1][2]=m->e[2][1];	t.e[2][2]=m->e[2][2];	t.e[3][2]=m->e[2][3];
	t.e[0][3]=m->e[3][0];	t.e[1][3]=m->e[3][1];	t.e[2][3]=m->e[3][2];	t.e[3][3]=m->e[3][3];
	*mt=t;
}

// 3d stuff
void xformvec(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo)
{
	struct pointf3 t;
	t.x=m->e[0][0]*vi->x+m->e[1][0]*vi->y+m->e[2][0]*vi->z+m->e[3][0];
	t.y=m->e[0][1]*vi->x+m->e[1][1]*vi->y+m->e[2][1]*vi->z+m->e[3][1];
	t.z=m->e[0][2]*vi->x+m->e[1][2]*vi->y+m->e[2][2]*vi->z+m->e[3][2];
	t.w=m->e[0][3]*vi->x+m->e[1][3]*vi->y+m->e[2][3]*vi->z+m->e[3][3];
	*vo=t;
}

void xformdir(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo)
{
	struct pointf3 t;
	t.x=m->e[0][0]*vi->x+m->e[1][0]*vi->y+m->e[2][0]*vi->z;
	t.y=m->e[0][1]*vi->x+m->e[1][1]*vi->y+m->e[2][1]*vi->z;
	t.z=m->e[0][2]*vi->x+m->e[1][2]*vi->y+m->e[2][2]*vi->z;
	vo->x=t.x;
	vo->y=t.y;
	vo->z=t.z;
}

//try transpose instead of inverse
void xformdirinv(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo)
{
	struct pointf3 t;
	t.x=m->e[0][0]*vi->x+m->e[0][1]*vi->y+m->e[0][2]*vi->z;
	t.y=m->e[1][0]*vi->x+m->e[1][1]*vi->y+m->e[1][2]*vi->z;
	t.z=m->e[2][0]*vi->x+m->e[2][1]*vi->y+m->e[2][2]*vi->z;
	vo->x=t.x;
	vo->y=t.y;
	vo->z=t.z;
}

// xform xyz1 to xyzw
void xformvecs(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo,S32 npoints)
{
	perf_start(XFORMVECS);
	while(npoints) {
		xformvec(m,vi,vo);
		vi++;
		vo++;
		npoints--;
	}
	perf_end(XFORMVECS);
}
// xform xyzw to xyzw
/*void xformvec4s(struct mat4 *m,struct pointf3 *vi,struct pointf3 *vo,S32 npoints)
{

perf_start(XFORMVECS);
	while(npoints) {
		mulmatvec4(m,vi,vo);
		vi++;
		vo++;
		npoints--;
	}
	perf_end(XFORMVECS);
}
*/
// just does the 4 by 3 part
void mulmat3d(const struct mat4 *a,const struct mat4 *b,struct mat4 *p)
{
	struct mat4 t;
	perf_start(MULMAT3D);
	t.e[0][0]=a->e[0][0]*b->e[0][0]+a->e[0][1]*b->e[1][0]+a->e[0][2]*b->e[2][0];
	t.e[0][1]=a->e[0][0]*b->e[0][1]+a->e[0][1]*b->e[1][1]+a->e[0][2]*b->e[2][1];
	t.e[0][2]=a->e[0][0]*b->e[0][2]+a->e[0][1]*b->e[1][2]+a->e[0][2]*b->e[2][2];
	t.e[0][3]=0;

	t.e[1][0]=a->e[1][0]*b->e[0][0]+a->e[1][1]*b->e[1][0]+a->e[1][2]*b->e[2][0];
	t.e[1][1]=a->e[1][0]*b->e[0][1]+a->e[1][1]*b->e[1][1]+a->e[1][2]*b->e[2][1];
	t.e[1][2]=a->e[1][0]*b->e[0][2]+a->e[1][1]*b->e[1][2]+a->e[1][2]*b->e[2][2];
	t.e[1][3]=0;

	t.e[2][0]=a->e[2][0]*b->e[0][0]+a->e[2][1]*b->e[1][0]+a->e[2][2]*b->e[2][0];
	t.e[2][1]=a->e[2][0]*b->e[0][1]+a->e[2][1]*b->e[1][1]+a->e[2][2]*b->e[2][1];
	t.e[2][2]=a->e[2][0]*b->e[0][2]+a->e[2][1]*b->e[1][2]+a->e[2][2]*b->e[2][2];
	t.e[2][3]=0;

	t.e[3][0]=a->e[3][0]*b->e[0][0]+a->e[3][1]*b->e[1][0]+a->e[3][2]*b->e[2][0]+b->e[3][0];
	t.e[3][1]=a->e[3][0]*b->e[0][1]+a->e[3][1]*b->e[1][1]+a->e[3][2]*b->e[2][1]+b->e[3][1];
	t.e[3][2]=a->e[3][0]*b->e[0][2]+a->e[3][1]*b->e[1][2]+a->e[3][2]*b->e[2][2]+b->e[3][2];
	t.e[3][3]=1;
	*p=t;
//	logger("mulmat3d\n");
//	logmat4(a,"a");
//	logmat4(b,"b");
//	logmat4(p,"p");
	perf_end(MULMAT3D);
}

S32 inversemat3d(const struct mat4 *m,struct mat4 *inv)
{
	S32 i,j;
	float p=1.0;
	struct mat4 cf,cft;
	struct mat3 detmat;
	float det,invdet;
	// optimize for 4 by 3 later
/*	m->e[0][3]=0;
	m->e[1][3]=0;
	m->e[2][3]=0;
	m->e[3][3]=1; */
	// first get the cofactor matrix
	for (j=0;j<4;j++) {
		for (i=0;i<4;i++) {
			comatrix4to3(m,&detmat,j,i);
			det=detmat3(&detmat);
			cf.e[j][i]=p*det;
			p*=-1.0;
		}
		p*=-1.0;
	}
	// get the det
	det=cf.e[0][0]*m->e[0][0]+cf.e[0][1]*m->e[0][1]+cf.e[0][2]*m->e[0][2]+cf.e[0][3]*m->e[0][3];
	// transpose
	for (j=0;j<4;j++)
		for (i=0;i<4;i++)
			cft.e[j][i]=cf.e[i][j];
	if (det==0)
		return 0;
	invdet=1.0f/det;
	// scale
	for (j=0;j<4;j++)
		for (i=0;i<4;i++)
			inv->e[j][i]=cft.e[j][i]*invdet;
	return 1;
}

static void buildrot3dnotrans(const struct pointf3 *rpy,struct mat4 *m)
{
	float rs,rc,ps,pc,ys,yc;
	float pcyc,pcys,rcpc,rspc,rsyc,rcyc,rsys,rcys;
	float rspsys,rcpsys,rspsyc,rcpsyc;
	rs=sinf(rpy->z);
	rc=cosf(rpy->z);
	ps=sinf(rpy->x);
	pc=cosf(rpy->x);
	ys=sinf(rpy->y);
	yc=cosf(rpy->y);
	pcyc=pc*yc;
	pcys=pc*ys;
	rcpc=rc*pc;
	rspc=rs*pc;
	rsyc=rs*yc;
	rcyc=rc*yc;
	rsys=rs*ys;
	rcys=rc*ys;
	rspsys=rsys*ps;
	rcpsys=rcys*ps;
	rspsyc=rsyc*ps;
	rcpsyc=rcyc*ps;
	m->e[0][0]=rcyc+rspsys;
	m->e[0][1]=rspc;
	m->e[0][2]=-rcys+rspsyc;
	m->e[1][0]=-rsyc+rcpsys;
	m->e[1][1]=rcpc;
	m->e[1][2]=rsys+rcpsyc;
	m->e[2][0]=pcys;
	m->e[2][1]=-ps;
	m->e[2][2]=pcyc;
	m->e[0][3]=0;
	m->e[1][3]=0;
	m->e[2][3]=0;
	m->e[3][3]=1;
}

void premulscale3d(const struct pointf3 *scale,const struct mat4 *a,struct mat4 *b)
{
	b->e[0][0]=a->e[0][0]*scale->x;
	b->e[0][1]=a->e[0][1]*scale->x;
	b->e[0][2]=a->e[0][2]*scale->x;
	b->e[1][0]=a->e[1][0]*scale->y;
	b->e[1][1]=a->e[1][1]*scale->y;
	b->e[1][2]=a->e[1][2]*scale->y;
	b->e[2][0]=a->e[2][0]*scale->z;
	b->e[2][1]=a->e[2][1]*scale->z;
	b->e[2][2]=a->e[2][2]*scale->z;
	b->e[0][3]=0;
	b->e[1][3]=0;
	b->e[2][3]=0;
	b->e[3][3]=1;
}

void postmulscale3d(const struct mat4 *a,const struct pointf3 *scale,struct mat4 *b)
{
	b->e[0][0]=a->e[0][0]*scale->x;
	b->e[0][1]=a->e[0][1]*scale->y;
	b->e[0][2]=a->e[0][2]*scale->z;
	b->e[1][0]=a->e[1][0]*scale->x;
	b->e[1][1]=a->e[1][1]*scale->y;
	b->e[1][2]=a->e[1][2]*scale->z;
	b->e[2][0]=a->e[2][0]*scale->x;
	b->e[2][1]=a->e[2][1]*scale->y;
	b->e[2][2]=a->e[2][2]*scale->z;
	b->e[3][0]=a->e[3][0]*scale->x;
	b->e[3][1]=a->e[3][1]*scale->y;
	b->e[3][2]=a->e[3][2]*scale->z;
	b->e[0][3]=0;
	b->e[1][3]=0;
	b->e[2][3]=0;
	b->e[3][3]=1;
}

void buildscalerot3d(const struct pointf3 *scale,const struct pointf3 *rpy,struct mat4 *m)
{
	buildrot3dnotrans(rpy,m);
	m->e[3][0]=0;
	m->e[3][1]=0;
	m->e[3][2]=0;
	// pre mul scale
	premulscale3d(scale,m,m);
}

void buildscalerottrans3d(const struct pointf3 *scale,const struct pointf3 *rpy,const struct pointf3 *trans,struct mat4 *m)
{
	buildrot3dnotrans(rpy,m);
	m->e[3][0]=trans->x;
	m->e[3][1]=trans->y;
	m->e[3][2]=trans->z;
	// pre mul scale
	premulscale3d(scale,m,m);
}

void buildrot3d(const struct pointf3 *rpy,struct mat4 *m)
{
	buildrot3dnotrans(rpy,m);
	m->e[3][0]=0;
	m->e[3][1]=0;
	m->e[3][2]=0;
}

void buildrottrans3d(const struct pointf3 *rpy,const struct pointf3 *trans,struct mat4 *m)
{
	buildrot3dnotrans(rpy,m);
	m->e[3][0]=trans->x;
	m->e[3][1]=trans->y;
	m->e[3][2]=trans->z;
}
