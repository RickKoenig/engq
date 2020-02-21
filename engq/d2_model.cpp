#define D2_3D
#include <m_eng.h>
#include "m_perf.h"
referencecount<modelb> modelb::rc; // and add this

modelb::modelb(string namea) :
	name(namea),
//	nverts(0),
//	nfaces(0),
//	drawpri(0),
	refidx(-1),
	updatefaces(false)
{
	boxmin.x=boxmin.y=boxmin.z=1e20f;
	boxmax.x=boxmax.y=boxmax.z=-1e20f;
	boxrad=0;
}

modelb::~modelb()
{
	S32 i,j,n=mats.size();
	for (i=0;i<n;++i) {
		for (j=0;j<MAXTEX;++j) {
			if (mats[i].thetexarr[j]) {
				textureb::rc.deleterc(mats[i].thetexarr[j]);
			}
		}
	}
}

void modelb::addmat(const C8* matname,S32 flags,textureb* tex,const pointf3* colora,float sp,S32 nface,S32 nvert,float refla,float ss)
{
	mater2 m;
	group2 g;
	m.refl=refla;
	m.msflags=flags;
//	m.mtrans=trans;
	m.thetexarr[0]=tex;
	S32 j;
	for (j=1;j<MAXTEX;++j)
		m.thetexarr[j]=0;
	m.name=matname;
	if (colora)
		m.color=*colora;
	else
		m.color=F32WHITE;
	m.colorb=F32WHITE;
//	logger("color = %f %f %f\n",m.color.x,m.color.y,m.color.z);
	m.specpow=sp;
	m.specstrength=ss;
	mats.push_back(m);
	S32 n=groups.size();
	g.nvert=nvert;
	g.nface=nface;
	if (n==0) {
		g.vertidx=0;
		g.faceidx=0;
	} else {
		g.vertidx=groups[n-1].vertidx+groups[n-1].nvert;
		g.faceidx=groups[n-1].faceidx+groups[n-1].nface;
	}
	groups.push_back(g);
}

void modelb::addmatn(const C8* matname,S32 flags,const C8* texname,const pointf3* colora,float sp,S32 nface,S32 nvert)
{
	textureb* tex=texture_create(texname);
	if (textureb::rc.getrc(tex)==1)
		tex->load();
	addmat(matname,flags,tex,colora,sp,nface,nvert);
}

void modelb::addmat2t(const C8*matname,S32 flags,textureb* tex,textureb* tex2,const pointf3* colora,float sp,S32 nface,S32 nvert,float refla)
{
	mater2 m;
	group2 g;
	m.refl=refla;
	m.msflags=flags;
//	m.mtrans=trans;
	m.thetexarr[0]=tex;
	m.thetexarr[1]=tex2;
	S32 j;
	for (j=2;j<MAXTEX;++j)
		m.thetexarr[j]=0;
	m.name=matname;
	if (colora)
		m.color=*colora;
	else
		m.color=F32WHITE;
	m.colorb=F32WHITE;
//	logger("color = %f %f %f\n",m.color.x,m.color.y,m.color.z);
	m.specpow=sp;
	m.specstrength=1;
	mats.push_back(m);
	S32 n=groups.size();
	g.nvert=nvert;
	g.nface=nface;
	if (n==0) {
		g.vertidx=0;
		g.faceidx=0;
	} else {
		g.vertidx=groups[n-1].vertidx+groups[n-1].nvert;
		g.faceidx=groups[n-1].faceidx+groups[n-1].nface;
	}
	groups.push_back(g);
}

void modelb::addmat2c(const C8*matname,S32 flags,textureb* tex,const pointf3* colora,const pointf3* colorba,float sp,S32 nface,S32 nvert)
{
	mater2 m;
	group2 g;
	m.msflags=flags;
//	m.mtrans=trans;
	m.thetexarr[0]=tex;
	m.refl=1;
	S32 j;
	for (j=1;j<MAXTEX;++j)
		m.thetexarr[j]=0;
	m.name=matname;
	if (colora)
		m.color=*colora;
	else
		m.color=F32WHITE;
	if (colorba)
		m.colorb=*colorba;
	else
		m.colorb=F32WHITE;
//	logger("color = %f %f %f\n",m.color.x,m.color.y,m.color.z);
	m.specpow=sp;
	m.specstrength=1;
	mats.push_back(m);
	S32 n=groups.size();
	g.nvert=nvert;
	g.nface=nface;
	if (n==0) {
		g.vertidx=0;
		g.faceidx=0;
	} else {
		g.vertidx=groups[n-1].vertidx+groups[n-1].nvert;
		g.faceidx=groups[n-1].faceidx+groups[n-1].nface;
	}
	groups.push_back(g);
}

/*
void modelb::setbbox(const pointf3* v,S32 nv)
{
	S32 i;
	boxmin.x=boxmin.y=boxmin.z=1e20f;
	boxmax.x=boxmax.y=boxmax.z=-1e20f;
	for (i=0;i<nv;++i) {
		if (v[i].x<boxmin.x)
			boxmin.x=v[i].x;
		if (v[i].x>boxmax.x)
			boxmax.x=v[i].x;
		if (v[i].y<boxmin.y)
			boxmin.y=v[i].y;
		if (v[i].y>boxmax.y)
			boxmax.y=v[i].y;
		if (v[i].z<boxmin.z)
			boxmin.z=v[i].z;
		if (v[i].z>boxmax.z)
			boxmax.z=v[i].z;
	}
	float bx=max(fabsf(boxmin.x),fabsf(boxmax.x));
	float by=max(fabsf(boxmin.y),fabsf(boxmax.y));
	float bz=max(fabsf(boxmin.z),fabsf(boxmax.z));
	boxrad=sqrtf(bx*bx+by*by+bz*bz);
//	logger("bounding box for model '%s' is (%f,%f,%f) (%f,%f,%f), rad %f\n",
//	  name.c_str(),boxmin.x,boxmin.y,boxmin.z,boxmax.x,boxmax.y,boxmax.z,boxrad);
}
*/
void modelb::setbbox()
{
	S32 i;
	boxmin.x=boxmin.y=boxmin.z=1e20f;
	boxmax.x=boxmax.y=boxmax.z=-1e20f;
	S32 nv=verts.size();
	for (i=0;i<nv;++i) {
		if (verts[i].x<boxmin.x)
			boxmin.x=verts[i].x;
		if (verts[i].x>boxmax.x)
			boxmax.x=verts[i].x;
		if (verts[i].y<boxmin.y)
			boxmin.y=verts[i].y;
		if (verts[i].y>boxmax.y)
			boxmax.y=verts[i].y;
		if (verts[i].z<boxmin.z)
			boxmin.z=verts[i].z;
		if (verts[i].z>boxmax.z)
			boxmax.z=verts[i].z;
	}
	float bx=max(fabsf(boxmin.x),fabsf(boxmax.x));
	float by=max(fabsf(boxmin.y),fabsf(boxmax.y));
	float bz=max(fabsf(boxmin.z),fabsf(boxmax.z));
	boxrad=sqrtf(bx*bx+by*by+bz*bz);
//	logger("bounding box for model '%s' is (%f,%f,%f) (%f,%f,%f), rad %f\n",
//	  name.c_str(),boxmin.x,boxmin.y,boxmin.z,boxmax.x,boxmax.y,boxmax.z,boxrad);
}

void modelb::showline()
// shows info about this instance when showref is called, and add this
{
	logger("[modelb] name '%-12s', nverts %5d, nfaces %5d, boxmin (%f %f %f), boxmax (%f %f %f)",
		name.c_str(),verts.size(),faces.size(),boxmin.x,boxmin.y,boxmin.z,boxmax.x,boxmax.y,boxmax.z);
	S32 i,j,n=groups.size();
//#define MODEL_VERBOSE
#ifdef MODEL_VERBOSE
	logger("\n");
	const S32 maxsize = 50;
	S32 np = min((S32)faces.size(),maxsize);
	for (i=0;i<np;++i)
		logger("\t\t\t\tface %4d : %4d %4d %4d mat %d\n",i,faces[i].vertidx[0],faces[i].vertidx[1],faces[i].vertidx[2],faces[i].fmatidx);
	np = min(verts.size(),maxsize);
	for (i=0;i<np;++i)
		logger("\t\t\t\tvert %4d : %8.3f %8.3f %8.3f\n",i,verts[i].x,verts[i].y,verts[i].z);
	np = min(cverts.size(),maxsize);
	for (i=0;i<np;++i)
		logger("\t\t\t\tcvert %4d : %8.3f %8.3f %8.3f\n",i,cverts[i].x,cverts[i].y,cverts[i].z);
	np = min(uvs0.size(),maxsize);
	for (i=0;i<np;++i)
		logger("\t\t\t\tuv0 %4d : %8.3f %8.3f\n",i,uvs0[i].u,uvs0[i].v);
	np = min((S32)norms.size(),maxsize);
	for (i=0;i<np;++i)
		logger("\t\t\t\tnorm %4d : %8.3f %8.3f %8.3f\n",i,norms[i].x,norms[i].y,norms[i].z);
#endif

//#define MODEL_XLS_EXPORT
#ifdef MODEL_XLS_EXPORT
// pairs of points to make line segments
	logger("\n------ xls export ----------\n");
	for (i=0;i<(S32)faces.size();++i) {
		S32 vi0 = faces[i].vertidx[0];
		S32 vi1 = faces[i].vertidx[1];
		S32 vi2 = faces[i].vertidx[2];
		logger("%f %f %f\n",verts[vi0].x,verts[vi0].y,verts[vi0].z);
		logger("%f %f %f\n",verts[vi1].x,verts[vi1].y,verts[vi1].z);
		logger("\n");							   
		logger("%f %f %f\n",verts[vi1].x,verts[vi1].y,verts[vi1].z);
		logger("%f %f %f\n",verts[vi2].x,verts[vi2].y,verts[vi2].z);
		logger("\n");							   
		logger("%f %f %f\n",verts[vi2].x,verts[vi2].y,verts[vi2].z);
		logger("%f %f %f\n",verts[vi0].x,verts[vi0].y,verts[vi0].z);
		logger("\n");
	}
	logger("------ end xls export ----------\n\n");
#endif

	if (n)
		logger("\n");
	logger_indent();
	for (i=0;i<n;++i) {
        logger_enableindent();
		logger("group %d: vo %d, vs %d, fo %d, fs %d ",
			i,groups[i].vertidx,groups[i].nvert,groups[i].faceidx,groups[i].nface);
        logger(", matname '%s'",mats[i].name.c_str());
        for (j=0;j<MAXTEX;++j)
			if (mats[i].thetexarr[j])
				logger(", texname%d '%s' ",j,mats[i].thetexarr[j]->name.c_str());
		logger("shine %f, refl %f, specstr %f",mats[i].specpow,mats[i].refl,mats[i].specstrength);
		logger_disableindent();
		logger("\n");
	}
	logger_unindent();
}

// generic version
clipresult modelb::checkbbox(mat4* o2c) // friends with model
{
	S32 i;
	S32 andc=0xff,orc=0;
	S32 code;
	struct pointf3 xbox[8];
	xbox[0].x=boxmin.x; xbox[0].y=boxmin.y; xbox[0].z=boxmin.z;
	xbox[1].x=boxmin.x; xbox[1].y=boxmin.y; xbox[1].z=boxmax.z;
	xbox[2].x=boxmin.x; xbox[2].y=boxmax.y; xbox[2].z=boxmin.z;
	xbox[3].x=boxmin.x; xbox[3].y=boxmax.y; xbox[3].z=boxmax.z;
	xbox[4].x=boxmax.x; xbox[4].y=boxmin.y; xbox[4].z=boxmin.z;
	xbox[5].x=boxmax.x; xbox[5].y=boxmin.y; xbox[5].z=boxmax.z;
	xbox[6].x=boxmax.x; xbox[6].y=boxmax.y; xbox[6].z=boxmin.z;
	xbox[7].x=boxmax.x; xbox[7].y=boxmax.y; xbox[7].z=boxmax.z;
	xformvecs(o2c,xbox,xbox,8);	// object xyx1 to post projection xyzw
//	logger("xbox[0].z %f, hard_zfront %f\n",xbox[0].z,hard_zfront);
	for (i=0;i<8;i++) {
		code=0;
//		if (xbox[i].w>=0) {
			if (xbox[i].z>xbox[i].w /*&& !(mats[0].msflags&SMAT_ISSKY)*/)
	//		if (xbox[i].z>hard_zback)
				code=CODE_FAR;
			if (xbox[i].z<0.0f)
				code|=CODE_NEAR;
			if (xbox[i].x>xbox[i].w)
				code|=CODE_RIGHT;
			if (-xbox[i].x>xbox[i].w)
				code|=CODE_LEFT;
			if (xbox[i].y>xbox[i].w)
				code|=CODE_TOP;
			if (-xbox[i].y>xbox[i].w)
				code|=CODE_BOT;
/*		} else {
			code=CODE_NEAR;
			if (xbox[i].x<xbox[i].w)
				code|=CODE_RIGHT;
			if (-xbox[i].x<xbox[i].w)
				code|=CODE_LEFT;
			if (xbox[i].y<xbox[i].w)
				code|=CODE_TOP;
			if (-xbox[i].y<xbox[i].w)
				code|=CODE_BOT;
		} */
		andc&=code;
		orc|=code;
	}
	if (andc)
		video3dinfo.bboxclipstatus=CLIP_OUT;
	else if (orc) 
		video3dinfo.bboxclipstatus=CLIP_CLIP;
	else
		video3dinfo.bboxclipstatus=CLIP_IN;
	return video3dinfo.bboxclipstatus;
}

///////////////// dx9 model ////////////////////////////////
void modelb::copyverts(const pointf3* vertsa,S32 nvertsa,bool fixgroupverts)
{
//	verts.assign(vertsa,vertsa+nvertsa);
	verts.setsize(nvertsa);
	copy(vertsa,vertsa+nvertsa,&*verts);
	if (fixgroupverts)
		groups[0].nvert = nvertsa;
	setbbox();
}

void modelb::copyuvs0(const uv* uvsa,S32 nuvsa)
{
//	uvs.assign(uvsa,uvsa+nuvsa);
	uvs0.setsize(nuvsa);
	copy(uvsa,uvsa+nuvsa,&*uvs0);
}

void modelb::copyuvs1(const uv* uvsa,S32 nuvsa)
{
//	uvs.assign(uvsa,uvsa+nuvsa);
	uvs1.setsize(nuvsa);
	copy(uvsa,uvsa+nuvsa,&*uvs1);
}

void modelb::copycverts(const pointf3* cvertsa,S32 ncvertsa)
{
//	cverts.assign(cvertsa,cvertsa+ncvertsa);
	cverts.setsize(ncvertsa);
	copy(cvertsa,cvertsa+ncvertsa,&*cverts);
}

void modelb::copyverts2(const pointf3* verts2a,S32 nverts2a)
{
	verts2.assign(verts2a,verts2a+nverts2a);
}

void modelb::copynorms(const pointf3* normsa,S32 nnormsa)
{
//	norms.assign(normsa,normsa+nnormsa);
//	norms.setsize(nnormsa);
//	copy(normsa,normsa+nnormsa,&*norms);
	norms.assign(normsa,normsa+nnormsa);
}

void modelb::copytangents(const pointf3* tangentsa,S32 ntangentsa)
{
	tangents.assign(tangentsa,tangentsa+ntangentsa);
}

void modelb::copybinorms(const pointf3* binormsa,S32 nbinormsa)
{
	binorms.assign(binormsa,binormsa+nbinormsa);
}

void modelb::copyfaces(const face* f,S32 nf,bool fixgroupface)
{
	faces.assign(f,f+nf);
	if (fixgroupface)
		groups[0].nface = nf;
	updatefaces = true;
}

void modelb::copyverts(const vector<pointf3>& v,bool fixgroupverts)
{
//	verts=v;
	S32 n=v.size();
	verts.setsize(n);
	copy(v.begin(),v.end(),&*verts);
	if (fixgroupverts)
		groups[0].nvert = n;
	setbbox();
}

void modelb::copyuvs0(const vector<uv>& v)
{
//	uvs=v;
	S32 n=v.size();
	uvs0.setsize(n);
	copy(v.begin(),v.end(),&*uvs0);
}

void modelb::copyuvs1(const vector<uv>& v)
{
//	uvs=v;
	S32 n=v.size();
	uvs1.setsize(n);
	copy(v.begin(),v.end(),&*uvs1);
}

void modelb::copycverts(const vector<pointf3>& v)
{
//	cverts=v;
	S32 n=v.size();
	cverts.setsize(n);
	copy(v.begin(),v.end(),&*cverts);
}

void modelb::copyverts2(const vector<pointf3>& v)
{
	verts2=v;
	U32 i,n=verts2.size();
	for (i=0;i<n;++i) {
		verts2[i].x;
		verts2[i].y;
		verts2[i].z;
	}
}

void modelb::copynorms(const vector<pointf3>& v)
{
//	norms=v;
//	S32 n=v.size();
//	norms.setsize(n);
//	copy(v.begin(),v.end(),&*norms);
	norms=v;
}

void modelb::copytangents(const vector<pointf3>& v)
{
	tangents=v;
}

void modelb::copybinorms(const vector<pointf3>& v)
{
	binorms=v;
}

void modelb::copyfaces(const vector<face>& f,bool fixgroupface)
{
	faces = f;
	if (fixgroupface)
		groups[0].nface = f.size();
	updatefaces = true;
}


// call after verts and faces set
void modelb::calcnorms() 
{
	// all set to zero
	U32 nverts = verts.size();
	norms = vector<pointf3>(verts.size());
	U32 i,nface = faces.size();
	for (i=0;i<nface;++i) {
		const face& f = faces[i];
		S32 vi0 = f.vertidx[0]; S32 vi1 = f.vertidx[1]; S32 vi2 = f.vertidx[2];
		const pointf3& p0 = verts[vi0]; const pointf3& p1 = verts[vi1]; const pointf3& p2 = verts[vi2];
		pointf3x v1m0(p1.x-p0.x,p1.y-p0.y,p1.z-p0.z,0);
		pointf3x v2m0(p2.x-p0.x,p2.y-p0.y,p2.z-p0.z,0);
		pointf3 nrm;
		cross3d(&v1m0,&v2m0,&nrm);
		//normalize3d(&nrm,&nrm); // no normalize, use weighted normals
		pointf3& n0 = norms[vi0]; pointf3& n1 = norms[vi1]; pointf3& n2 = norms[vi2];
		n0.x += nrm.x; n0.y += nrm.y; n0.z += nrm.z;
		n1.x += nrm.x; n1.y += nrm.y; n1.z += nrm.z;
		n2.x += nrm.x; n2.y += nrm.y; n2.z += nrm.z;
	}
	for (i=0;i<nverts;++i) {
		pointf3* v = &norms[i];
		normalize3d(v,v);
	}
}
// to UVS 0,0 to 1,1 (or some multiple maybe)
static void convertXYZtoUV(const pointf3* xyz,uv* auv,const pointf3& offset)
{
	pointf3x xyzo(xyz->x + offset.x,xyz->y + offset.y,xyz->z + offset.z);
	// assume non-degenerates, but do handle the poles
	float xzlen2 = xyzo.x*xyzo.x + xyzo.z*xyzo.z;
	if (xzlen2 <= EPSILON) {
		if (xyzo.y >= 0) { // north pole
			auv->u = 0;
			auv->v = 0;
		} else { // south pole
			auv->u = 0;
			auv->v = 1;
		}
	} else { // not a pole
		auv->u = atan2f(xyzo.x,-xyzo.z);
		float xzlen = sqrtf(xzlen2);
		auv->v = atan2f(xzlen,xyzo.y);
		// make uvs go from 0 to 1
		auv->u *= (.5f/PI);
		if (auv->u < 0)
			auv->u += 1;
		auv->v *= (1.0f/PI);
	}
}

// call after verts are set, map xyz to longitude and latitude (spherical coords)
void modelb::calcuvs(const pointf3& offset)
{
//#define JUSTCLEAR
	U32 nface = faces.size();
#define DOTILE
#define FIXCUT // try to repair around the branch cut
#define AVGPOLE // try to move the u of a pole to the avg of the other u's in that face
	// all set to zero
	S32 nverts = verts.size();
	uvs0.setsize(nverts);
#ifdef JUSTCLEAR
	fill(&uvs0[0],&uvs0[(S32)nverts],uvx(.75f,.15f));
	return;
#endif
	// first pass, just convert x,y,z to u,v with no branch cut checking (prime meridian)
	for (S32 i=0;i<nverts;++i) {
		convertXYZtoUV(&verts[i],&uvs0[i],offset);
	}
// try to make uvs not do a "rewind" when crossing the prime meridian, only for 'u' subtract u by a full 1
#ifdef FIXCUT
	for (U32 j=0;j<nface;++j) {
		// find minimum 'u' on a face, but only use if not a pole (v == 0 || v == 1)
		float minu = 1.0f;
		for (U32 i=0;i<3;++i) {
			uv curuv = uvs0[faces[j].vertidx[i]];
			if (curuv.v != 0.0f && curuv.v != 1.0f)
				if (curuv.u < minu)
					minu = curuv.u;
		}
		// see if some u's on other verts of the face are .5 or more than the minimum, if so then subtract a whole 1 from the u to make it closer to minimum, but not on poles
		for (U32 i=0;i<3;++i) {
			uv& curuv = uvs0[faces[j].vertidx[i]]; // aah, a reference
			if (curuv.v != 0.0f && curuv.v != 1.0f)
				if (curuv.u >= minu + .5f)
					curuv.u--;  // make more face local
		}
	}
#endif
#ifdef AVGPOLE
	for (U32 j=0;j<nface;++j) {
		// calc avg u on non poles for each face
		S32 nnp = 0; // non pole count
		float sumu = 0;
		for (U32 i=0;i<3;++i) {
			const uv& curuv = uvs0[faces[j].vertidx[i]];
			if (curuv.v != 0.0f && curuv.v != 1.0f) {
				sumu += curuv.u;
				++nnp;
			}
		}
		// now put that average u into the u of the poles
		if (nnp > 0) {
			float avgu = sumu/nnp;
			for (U32 i=0;i<3;++i) {
				uv& curuv = uvs0[faces[j].vertidx[i]];
				if (curuv.v == 0.0f || curuv.v == 1.0f) {
					curuv.u = avgu;
				}
			}
		}
	}
#endif
#ifdef DOTILE
	float tileu = 8.0f;
	float tilev = 6.0f;
	for (S32 i=0;i<nverts;++i) {
		uvs0[i].u *= tileu;
		uvs0[i].v *= tilev;
	}
#endif
}

void modelb::addface(S32 a,S32 b,S32 c,bool ds)
{
	face aface;
	aface.vertidx[0]=a;
	aface.vertidx[1]=b;
	aface.vertidx[2]=c;
	aface.fmatidx = 0;
	faces.push_back(aface);
	if (ds) {
		aface.vertidx[1]=c;
		aface.vertidx[2]=b;
		faces.push_back(aface);
	}
}

// assume verts are independent, multiple of 3, generate the faces for this
void modelb::genindifaces(bool ds)
{
	S32 f;
	S32 nf = verts.size()/3;
	for (f=0;f<nf;++f) {
		S32 f3 = f*3;
		face aface;
		aface.vertidx[0] = f3 + 0;
		aface.vertidx[1] = f3 + 1;
		aface.vertidx[2] = f3 + 2;
		aface.fmatidx = 0;
		faces.push_back(aface);
		if (ds) {
			aface.vertidx[1] = f3 + 2;
			aface.vertidx[2] = f3 + 1;
			faces.push_back(aface);
		}
	}
}

// add a bunch of faces
void modelb::addfaces(const face* f,S32 nfaces,S32 vertoffset,bool flip,bool ds)
{
	S32 i;
	if (ds) {
		for (i=0;i<nfaces;++i) {
			face aface;
			aface.vertidx[0]=f[i].vertidx[0]+vertoffset;
			aface.vertidx[1]=f[i].vertidx[1]+vertoffset;
			aface.vertidx[2]=f[i].vertidx[2]+vertoffset;
			aface.fmatidx=f[i].fmatidx;
			faces.push_back(aface);
			aface.vertidx[1]=f[i].vertidx[2]+vertoffset;
			aface.vertidx[2]=f[i].vertidx[1]+vertoffset;
			faces.push_back(aface);
		}
	} else {
		if (!flip) {
			for (i=0;i<nfaces;++i) {
				face aface;
				aface.vertidx[0]=f[i].vertidx[0]+vertoffset;
				aface.vertidx[1]=f[i].vertidx[1]+vertoffset;
				aface.vertidx[2]=f[i].vertidx[2]+vertoffset;
				aface.fmatidx=f[i].fmatidx;
				faces.push_back(aface);
			}
		} else {
			for (i=0;i<nfaces;++i) {
				face aface;
				aface.vertidx[0]=f[i].vertidx[2]+vertoffset;
				aface.vertidx[1]=f[i].vertidx[1]+vertoffset;
				aface.vertidx[2]=f[i].vertidx[0]+vertoffset;
				aface.fmatidx=f[i].fmatidx;
				faces.push_back(aface);
			}
		}
	}
}

void modelb::addfaces(const vector<face> f,S32 vertoffset,bool flip,bool ds)
{
	S32 i;
	S32 nfaces=f.size();
	if (ds) {
		for (i=0;i<nfaces;++i) {
			face aface;
			aface.vertidx[0]=f[i].vertidx[0]+vertoffset;
			aface.vertidx[1]=f[i].vertidx[1]+vertoffset;
			aface.vertidx[2]=f[i].vertidx[2]+vertoffset;
			aface.fmatidx=f[i].fmatidx;
			faces.push_back(aface);
			aface.vertidx[1]=f[i].vertidx[2]+vertoffset;
			aface.vertidx[2]=f[i].vertidx[1]+vertoffset;
			faces.push_back(aface);
		}
	} else {
		if (!flip) {
			for (i=0;i<nfaces;++i) {
				face aface;
				aface.vertidx[0]=f[i].vertidx[0]+vertoffset;
				aface.vertidx[1]=f[i].vertidx[1]+vertoffset;
				aface.vertidx[2]=f[i].vertidx[2]+vertoffset;
				aface.fmatidx=f[i].fmatidx;
				faces.push_back(aface);
			}
		} else {
			for (i=0;i<nfaces;++i) {
				face aface;
				aface.vertidx[0]=f[i].vertidx[2]+vertoffset;
				aface.vertidx[1]=f[i].vertidx[1]+vertoffset;
				aface.vertidx[2]=f[i].vertidx[0]+vertoffset;
				aface.fmatidx=f[i].fmatidx;
				faces.push_back(aface);
			}
		}
	}
}

void modelb::changetex(textureb* tx,U32 matn,U32 texn)
{
	if (matn>=mats.size())
		return;
	if (texn>=MAXTEX)
		return;
	textureb* & txslot=mats[0].thetexarr[0];
	textureb::rc.deleterc(txslot);
	txslot=textureb::rc.newrc(tx);
}
