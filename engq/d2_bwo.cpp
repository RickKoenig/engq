#define D2_3D
#include <m_eng.h>

// this is one material of many from a .bwo file
struct model_mat {
	string name;
	S32 fo,vo,fs,vs; // face offset, vert offset, face size, vert size
	string dtex,atex;
	VC4 color; // with alpha
	float shine;
	float specstrength;
	float refl; // refection amount
	S32 flags; // clamp etc.
// and many more obj_mat..
};

#define MAX_UV_CHAN 4
// this is the optimized model from a .bwo file
struct model_info {
	vector<model_mat> mats; // groups
	vector<IDX3M> faces;
// same size
	vector<VC3> verts;
	vector<VC3> norms;
	vector<VC3> cverts;
	vector<VC2> uvs[MAX_UV_CHAN];
};

// process a material chunk from .bwo file, return one material/group
static model_mat get_matgroup(unchunker& uc)
{
	model_mat om;
	chunkname cn;
	chunktype ct;
	VC3 color={1,1,1};
	float opac=1;
	float shine=30;
	float specstrength=1;
	float refl=1;
	S32 flags=3; // wrap flags
	S32 numele,elesize,datasize;
	while(uc.getchunkheader(cn,ct,numele,elesize,datasize)) {
		if (ct==KID_ENDCHUNK) {
			break;
		} else if (numele && ct==KID_I8) {
			switch(cn) {
			case UID_NAME:
				om.name=uc.readI8v();
				break;
			case UID_DTEX:
				om.dtex=uc.readI8v();
				break;
			case UID_ATEX:
				om.atex=uc.readI8v();
				break;
			default:
				uc.skipdata();
				break;
			}
		} else if (!numele && ct==KID_I32) {
			switch(cn) {
			case UID_FO:
				uc.readI32(om.fo);
				break;
			case UID_FS:
				uc.readI32(om.fs);
				break;
			case UID_VO:
				uc.readI32(om.vo);
				break;
			case UID_VS:
				uc.readI32(om.vs);
				break;
			case UID_TILING:
				uc.readI32(flags); // wrap flags
				break;
			default:
				uc.skipdata();
				break;
			}
		} else if (!numele && ct==KID_VEC3) {
			VC3 spcol;
			switch(cn) {
			case UID_SPECULAR:
				uc.readVC3(spcol);
				specstrength=spcol.y; // any will do..
				break;
			case UID_DIFFUSE:
				uc.readVC3(color);
				break;
			default:
				uc.skipdata();
				break;
			}
		} else if (!numele && ct==KID_FLOAT) {
			switch(cn) {
			case UID_OPACITY:
				uc.readFLOAT(opac);
				break;
			case UID_SHINE:
				uc.readFLOAT(shine);
				break;
			case UID_REFL_AMT:
				uc.readFLOAT(refl);
				break;
			default:
				uc.skipdata();
				break;
			}
		} else {
			uc.skipdata();
		}
	}
#if 0
	om.color.x=color.x;
	om.color.y=color.y;
	om.color.z=color.z;
#else
	om.color.x=1;
	om.color.y=1;
	om.color.z=1;
#endif
	om.color.w=opac;
	om.shine=shine*4;	// convert from r*v to h*n (to half vectors) 4?
	om.specstrength=specstrength;
	om.refl=refl;
	om.flags=flags^3; // convert wrap to clamp
	return om;
}

// read in a .bwo file, return engine model ptr, used by tree2
modelb* loadbwomodel(const C8 *fname)
{
	S32 i,n;
	//logger("loadbwo ------------------ '%s' -------\n",fname);
	modelb* mod=model_create(fname);
	if (model_getrc(mod)>1)
		return mod;
	if (wininfo.dumpbwo)
		unchunktest(fname,10);
// first read .bwo file into model_info_optimized
	model_info oio;
	chunkname cn;
	chunktype ct;
	S32 numele,elesize,datasize;
	unchunker bwoch(fname);
	while(bwoch.getchunkheader(cn,ct,numele,elesize,datasize)) {
		if (ct==KID_ENDCHUNK)
			break;
		switch(cn) {
		case UID_FL:
			oio.faces=bwoch.readIDX3Mv();
			break;
		case UID_VL:
			oio.verts=bwoch.readVC3v();
			break;
		case UID_VN:
			oio.norms=bwoch.readVC3v();
			break;
		case UID_VC:
			oio.cverts=bwoch.readVC3v();
			break;
		case UID_TV:
//			if (1) {
			if (video3dinfo.favorlightmap) {
				oio.uvs[0]=bwoch.readVC2v(); // catch lightmap list set of uvs as uvs[0]
			} else {
				if (oio.uvs[0].empty()) {
					oio.uvs[0]=bwoch.readVC2v();
				} else if (oio.uvs[1].empty()) {
					oio.uvs[1]=bwoch.readVC2v();
				} else {
					bwoch.skipdata();
				}
			}
			break;
		case UID_MATERIAL:
			if (ct==KID_CHUNK) {
				oio.mats.push_back(get_matgroup(bwoch));
			}
		default:
			bwoch.skipdata();
			break;
		}
	}
	if (!oio.faces.size())
		return mod;
// now build an engine model from modelinfo_optimized
// using buildamodel
	float ms=1;
//	float ms=getmasterscale();
// copy verts
	n=oio.verts.size();
	pointf3* vp=new pointf3[n];
	for (i=0;i<n;++i) {
		vp[i].x=oio.verts[i].x*ms;
		vp[i].y=oio.verts[i].y*ms;
		vp[i].z=oio.verts[i].z*ms;
	}
	mod->copyverts(vp,n);
	delete[] vp;
// copy uvs0
	n=oio.uvs[0].size();
	if (n) {
		uv* uvp=new uv[n] ;
		for (i=0;i<n;++i) {
			uvp[i].u=oio.uvs[0][i].x;
			uvp[i].v=oio.uvs[0][i].y;
		}
		mod->copyuvs0(uvp,n);
		delete[] uvp;
	}
// copy uvs1
	n=oio.uvs[1].size();
	if (n) {
		uv* uvp=new uv[n] ;
		for (i=0;i<n;++i) {
			uvp[i].u=oio.uvs[1][i].x;
			uvp[i].v=oio.uvs[1][i].y;
		}
		mod->copyuvs1(uvp,n);
		delete[] uvp;
	}
// copy norms
	n=oio.norms.size();
	pointf3* normp=new pointf3[n] ;
	for (i=0;i<n;++i) {
		normp[i].x=oio.norms[i].x;
		normp[i].y=oio.norms[i].y;
		normp[i].z=oio.norms[i].z;
	}
	mod->copynorms(normp,n);
	delete[] normp;
// copy cverts
	n=oio.cverts.size();
	pointf3* cvertp=new pointf3[n] ;
	for (i=0;i<n;++i) {
		cvertp[i].x=oio.cverts[i].x;
		cvertp[i].y=oio.cverts[i].y;
		cvertp[i].z=oio.cverts[i].z;
		cvertp[i].w=1;
	}
	mod->copycverts(cvertp,n);
	delete[] cvertp;
// copy faces
	n=oio.faces.size();
	for (i=0;i<n;++i) {
		face f;
		f.vertidx[0]=oio.faces[i].idx[0];
		f.vertidx[1]=oio.faces[i].idx[1];
		f.vertidx[2]=oio.faces[i].idx[2];
//		f.fmatidx=0;
		f.fmatidx=oio.faces[i].matidx;
		mod->addfaces(&f,1,0);
//		logger("face %3d: %3d %3d %3d\n",i,f.vertidx[0],f.vertidx[1],f.vertidx[2]);
	}
//	S32 flags = SMAT_HASTEX|SMAT_HASWBUFF;
	n=oio.mats.size();
	for (i=0;i<n;++i) {
		const model_mat& mr=oio.mats[i];
// build materials and groups
		S32 flags = lightinfo.uselights ?
			SMAT_HASTEX|SMAT_HASWBUFF|SMAT_HASSHADE|SMAT_CALCLIGHTS : SMAT_HASTEX|SMAT_HASWBUFF;
		if (mr.flags&1) // u clamp
			flags|=SMAT_CLAMPU;
		if (mr.flags&2) // v clamp
			flags|=SMAT_CLAMPV;
		const pointf3x col(mr.color.x,mr.color.y,mr.color.z,mr.color.w);
//		if (mr.color.w<.95f)
//			logger("mat opacity for '%s' is %f\n",mr.name.c_str(),mr.color.w);
// get texture
		if (video3dinfo.favorlightmap) { // use lightmap in main path
// get 2nd texture
			if (!getsuggestlightmap().empty()) { // see if global lightmap, use as 2nd texture
				textureb* tex2=texture_create(getsuggestlightmap().c_str());
				if (texture_getrc(tex2)==1) {
					tex2->load();
				}
//				mod->addmat2t(mr.name.c_str(),flags,tex,tex2,&col,mr.shine,mr.fs,mr.vs);
				mod->addmat(mr.name.c_str(),flags,tex2,&col,mr.shine,mr.fs,mr.vs,mr.refl,mr.specstrength);
			} else if (!(mr.atex.empty())) { // see if alpha tex, use as 2nd texture, obsolete, but were trying
				textureb* tex2=texture_create(mr.atex.c_str());
				if (texture_getrc(tex2)==1) {
					tex2->load();
				}
//				mod->addmat2t(mr.name.c_str(),flags,tex,tex2,&col,mr.shine,mr.fs,mr.vs);
				mod->addmat(mr.name.c_str(),flags,tex2,&col,mr.shine,mr.fs,mr.vs,mr.refl,mr.specstrength);
// nope just one texture
			} else {
				textureb* tex=texture_create(mr.dtex.c_str());
				if (texture_getrc(tex)==1) {
					tex->load();
				}
				mod->addmat(mr.name.c_str(),flags,tex,&col,mr.shine,mr.fs,mr.vs,mr.refl,mr.specstrength);
			}
		} else { // no favorlightmap (normal)
// get 1st texture
			textureb* tex=texture_create(mr.dtex.c_str());
			if (texture_getrc(tex)==1) {
				tex->load();
			}
// get 2nd texture if necc.
//			if (true) {
			if (video3dinfo.is3dhardware) {
				if (!getsuggestlightmap().empty()) { // see if global lightmap, use as 2nd texture
					textureb* tex2=texture_create(getsuggestlightmap().c_str());
					if (texture_getrc(tex2)==1) {
						tex2->load();
					}
					mod->addmat2t("lightmap",flags,tex,tex2,&col,mr.shine,mr.fs,mr.vs,mr.refl);
//					mod->addmat(mr.name.c_str(),flags,tex2,&col,mr.shine,mr.fs,mr.vs);
				} else if (!(mr.atex.empty())) { // see if alpha tex, use as 2nd texture, obsolete, but were trying
					textureb* tex2=texture_create(mr.atex.c_str());
					if (texture_getrc(tex2)==1) {
						tex2->load();
					}
					mod->addmat2t(mr.name.c_str(),flags,tex,tex2,&col,mr.shine,mr.fs,mr.vs,mr.refl);
//					mod->addmat(mr.name.c_str(),flags,tex2,&col,mr.shine,mr.fs,mr.vs);
// nope just one (normal 1 texture load)
				} else {
					mod->addmat(mr.name.c_str(),flags,tex,&col,mr.shine,mr.fs,mr.vs,mr.refl,mr.specstrength);
				}
			} else {
				mod->addmat(mr.name.c_str(),flags,tex,&col,mr.shine,mr.fs,mr.vs,mr.refl,mr.specstrength);
			}
		}
	}
	mod->close();
	if (!mod->uvs0.size()) { // if no uv's don't use texture, prevent crash
		int n=mod->mats.size();
		for (i=0;i<n;++i) {
			mod->mats[i].msflags&=~SMAT_HASTEX;
		}
	}
	return mod;
}
