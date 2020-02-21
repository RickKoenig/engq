//var defaultbwoshader = "tex";
var defaultbwoshader = "diffusep";
var defaultbwoshadernonormals = "tex";

function preloadbwo(bwoname,nowait) {
	var t = preloadedbin[bwoname];
	if (t)
		return;
	goAjaxBin(bwoname,resppreloadbwo,nowait);
}

// preload texture images
function resppreloadbwo(bwo,bwoname,nowait) {
	if (!bwo) {
		if (isloaded() && !nowait) {
			if (loaddonefunc) {
				loaddonefunc();
			}
		}
		return;
	}
	var s = spliturl(bwoname);
	var key = geturlfrompathnameext("",s.name,s.ext);
	//logger("in resppreloadbwo read '" + bwoname + "'\n");
	preloadedbin[key] = bwo;
	var uc = new unchunker(bwo);
	var chi;
	while(chi = uc.getchunkheader()) {
		if (chi.ct==uc.chunktypeenum.KID_CHUNK) {	// don't skip subchunk data
			//logger("ignoring data size of chunk " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + ", entering chunk\n");
			continue;
		}
		switch(chi.cn) {
		case uc.chunknameenum.UID_DTEX:
			//logger("processing " + chi.numele + " " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + "\n");
			var dtex=uc.readI8v();
			var dtexsplit = spliturl(dtex);
			//logger("   dtex = '" + dtex + "'\n");
			var relimg = geturlfrompathnameext(s.path,dtexsplit.name,dtexsplit.ext); // use path of bwoname
			preloadimg(relimg,nowait);
			break;
		default:
			//logger("DATA " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + ": SKIPPING\n");
			uc.skipdata();
			break;
		}
	}
	//logger("done resppreloadbwo!\n");
	if (isloaded() && !nowait) {
		if (loaddonefunc) {
			loaddonefunc();
		}
	}
}

// process a material chunk from .bwo file, return one material/group
function get_matgroup(uc) {
	var om = {};
	var cn;
	var ct;
	var chi;
	om.color = {"x":1,"y":1,"z":1,"w":1};
	while(chi = uc.getchunkheader()) {
		if (chi.ct==uc.chunktypeenum.KID_ENDCHUNK) {
			break;
		} else if (chi.numele && chi.ct==uc.chunktypeenum.KID_I8) {
			switch(chi.cn) {
			case uc.chunknameenum.UID_NAME:
				om.name=uc.readI8v();
				break;
			case uc.chunknameenum.UID_DTEX:
				//if (om.dtex) {
				//	uc.skipdata();
				//} else {
					om.dtex=uc.readI8v();
				//}
				break;
			default:
				uc.skipdata();
				break;
			}
		} else if (!chi.numele && chi.ct==uc.chunktypeenum.KID_I32) {
			switch(chi.cn) {
			case uc.chunknameenum.UID_FO:
				om.fo = uc.readI32();
				break;
			case uc.chunknameenum.UID_FS:
				om.fs = uc.readI32();
				break;
			case uc.chunknameenum.UID_VO:
				om.vo = uc.readI32();
				break;
			case uc.chunknameenum.UID_VS:
				om.vs = uc.readI32();
				break;
			default:
				uc.skipdata();
				break;
			}
		} else if (!chi.numele && chi.ct==uc.chunktypeenum.KID_VEC3) {
			switch(chi.cn) {
			case uc.chunknameenum.UID_DIFFUSE:
				om.color = uc.readVC3();
				om.color.w = 1;
				break;
			default:
				uc.skipdata();
				break;
			}
		} else {
			uc.skipdata();
		}
	}
	return om;
}

function loadbwomodel(bwoname) {
	//var ret = new Model2(bwoname);
	var ret = Model2.createmodel(bwoname);
	if (ret.refcount > 1)
		return ret;
	var bwo = preloadedbin[bwoname];
	if (!bwo)
		return null;
	//logger("in bwo read\n");
	var uc = new unchunker(bwo);
	var chi;
	var oio = {};
	//oio.diffuse = [];
	oio.mats = [];
	
	var head = 10;
	while(chi = uc.getchunkheader()) {
		if (chi.ct==uc.chunktypeenum.KID_ENDCHUNK) {	// don't skip subchunk data
			//logger("end chunk " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + ", entering chunk\n");
			break;
		}
		switch(chi.cn) {
		case uc.chunknameenum.UID_FL:
			//logger("processing " + chi.numele + " " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + "\n");
			oio.faces=uc.readIDX3Mv();
/*			for (i=0;i<oio.faces.length;++i) {
				if (i >= head)
					break;
				//logger("   DATA ARRAY[" + i + "]: FACE with matidx " + oio.faces[i].idx[0] + " " + oio.faces[i].idx[1] + " " + oio.faces[i].idx[2] + " " + oio.faces[i].matidx + "\n");
			} */
			break;
		case uc.chunknameenum.UID_VL:
			//logger("processing " + chi.numele + " " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + "\n");
			oio.verts=uc.readVC3v();
/*			for (i=0;i<oio.verts.length;++i) {
				if (i >= head)
					break;
				//logger("   DATA ARRAY[" + i + "]: VERT " + oio.verts[i].x.toFixed(2) + " " + oio.verts[i].y.toFixed(2) + " " + oio.verts[i].z.toFixed(2) + "\n");
			} */
			break;
		case uc.chunknameenum.UID_VN:
			//logger("processing " + chi.numele + " " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + "\n");
			oio.norms=uc.readVC3v();
/*			for (i=0;i<oio.norms.length;++i) {
				if (i >= head)
					break;
				//logger("   DATA ARRAY[" + i + "]: NORM " + oio.norms[i].x.toFixed(2) + " " + oio.norms[i].y.toFixed(2) + " " + oio.norms[i].z.toFixed(2) + "\n");
			} */
			break;
		// cverts ?
		case uc.chunknameenum.UID_TV:
			//logger("processing " + chi.numele + " " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + "\n");
			if (!oio.uvs) {
				oio.uvs=uc.readVC2v();
			} else if (!oio.uvs2) {
				oio.uvs2=uc.readVC2v();
			} else {
				uc.skipdata();
			}
/*			if (oio.uvs) {
				//logger("   DATA ARRAY skipping, already have UID_TV\n");
				uc.skipdata();
			} else {
				oio.uvs=uc.readVC2v();
				for (i=0;i<oio.uvs.length;++i) {
					if (i >= head)
						break;
					//logger("   DATA ARRAY[" + i + "]: UV " + oio.uvs[i].x.toFixed(2) + " " + oio.uvs[i].y.toFixed(2) + "\n");
				} 
			} */
			break;
		case uc.chunknameenum.UID_MATERIAL:
			if (chi.ct==uc.chunktypeenum.KID_CHUNK) {
				oio.mats.push(get_matgroup(uc));
			}
/*		case uc.chunknameenum.UID_DIFFUSE:
			//logger("processing " + chi.numele + " " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + "\n");
			var diff = uc.readVC3();
			//logger("   DIFFUSE " + diff.x.toFixed(2) + " " + diff.y.toFixed(2) + " " + diff.z.toFixed(2) + "\n");
			oio.diffuse.push(diff);
			break; */
		default:
			//logger("DATA " + uc.getchunkname_strs(chi.cn) + " " + uc.getchunktype_strs(chi.ct) + ": SKIPPING\n");
			uc.skipdata();
			break;
		}
	}
	//logger("done loadbwo!\n");
//	return oio;
	//logger("build bwo Model2\n");
	// modify the mesh depending on dolightmap
	var candolightmap = oio.uvs2 && globallmname;
	switch(dolightmap) {
	case 0: // no lightmap
		uvs2 = null;
		break;
	case 1: // mix it all together
		break;
	case 2: // just show the lightmap
		if (candolightmap) { // switch over to alt uvs
			oio.uvs = oio.uvs2;
			oio.uvs2 = null;
		} else {
			oio.uvs2 = null;
		}
		break;
	}
	ret.setmesh(oio);
	var i;
	var ds = oio.norms ? defaultbwoshader : defaultbwoshadernonormals;
	for (i=0;i<oio.mats.length;++i) {
		var mat = oio.mats[i];
		// 1 or 2 textures depending on dolightmap
		switch(dolightmap) {
		case 0:
			ret.addmat(ds,mat.dtex,mat.fs,mat.vs);
			break;
		case 1:
			if (candolightmap) {
				ret.addmat2t("lightmap",mat.dtex,globallmname,mat.fs,mat.vs); // mix it all together
			} else {
				ret.addmat(ds,mat.dtex,mat.fs,mat.vs);
			}
			break;
		case 2:
			if (candolightmap) {
				ret.addmat(ds,globallmname,mat.fs,mat.vs); // switch over to alt texture
			} else {
				ret.addmat(ds,mat.dtex,mat.fs,mat.vs);
			}
			break;
		}
	}
	// check groups
	for (i=0;i<oio.mats.length;++i) {
		var mat = oio.mats[i];
		var glmat = ret.grps[i];
		if (mat.fo != glmat.faceidx || mat.fs != glmat.nface || mat.vo != glmat.vertidx || mat.vs != glmat.nvert)
			alert("group mismatch '" + bwoname + "'");
	}
	ret.commit();
	return ret;
}

/*
// read in a .bwo file, return engine model ptr, used by tree2
modelb* loadbwomodel(const C8 *fname)
{
	S32 i,n;
//	logger("loadbwo ------------------ '%s' -------\n",fname);
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

 */