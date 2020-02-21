// read any chunker file and display everything about it

function unchunker(ab) {
	this.chunksize = 0;
	this.lastnumele = 0;
	this.fr = fopen(ab);
}

unchunker.prototype.chunkname = [
// user section
	'UID_NONE', // for I'm in 'no chunk'
// chunks
	'UID_OBJECT',
	'UID_MATERIAL',
// misc
	'UID_VERSION',
	'UID_COMMENT',
	'UID_NAME',
// materials
	'UID_DTEX',
	'UID_ATEX',
// objects
	'UID_ID',
	'UID_PID',
	'UID_POS',
	'UID_ROTo',
	'UID_SCALE',
	'UID_FL',
	'UID_VL',
	'UID_VN',  // vertex normals
	'UID_TV', // texture uvs obsolete switch to tv0 to tv15
	'UID_TFo', // will be obsolete
	'UID_ROT_ROTAXIS', // change to quat later
	'UID_ROT_QUAT',
	'UID_ROT_EULER',
	'UID_TV0', // use these soon, texture layers, 16 should be enough
	'UID_TV1', // I want them to be contiguous
	'UID_TV2',
	'UID_TV3',
	'UID_TV4',
	'UID_TV5',
	'UID_TV6',
	'UID_TV7',
	'UID_TV8',
	'UID_TV9',
	'UID_TV10',
	'UID_TV11',
	'UID_TV12',
	'UID_TV13',
	'UID_TV14',
	'UID_TV15',
	'UID_FN',	// will be obsolete 3 normals per face.. go for VN later
	'UID_FS', // for groups
	'UID_FO',
	'UID_VS',
	'UID_VO',
	'UID_USERPROP',
	'UID_MATRIX',
	'UID_KIND', // GEOM,HELPER,BONE etc. look in objects.h
	'UID_TARGET',
// camera
	'UID_CAMERA_FOV',
// light
	'UID_LIGHT_COLOR',
	'UID_LIGHT_INTENSITY',
	'UID_LIGHT_HOTSIZE',
	'UID_LIGHT_FALLSIZE',
	'UID_LIGHT_USE_NEAR_ATTEN', 
	'UID_LIGHT_NEAR_ATTEN_START', 
	'UID_LIGHT_NEAR_ATTEN_END',
	'UID_LIGHT_USE_ATTEN',
	'UID_LIGHT_ATTEN_START',
	'UID_LIGHT_ATTEN_END',
	'UID_LIGHT_SHADOW',
	'UID_LIGHT_OVERSHOOT',
// keyframes
	'UID_KEYFRAMEo', // obsolete
	'UID_TRACKFLAGS',
	'UID_POS_BEZ', // now uses KID_SCL_BEZv2 5-7-05, pick this one
	'UID_POS_TCB',
	'UID_POS_LIN',
	'UID_ROT_BEZ',
	'UID_ROT_TCB',
	'UID_ROT_LIN',
	'UID_SCL_BEZ', // now uses KID_SCL_BEZv2 5-7-05, pick this one
	'UID_SCL_TCB',
	'UID_SCL_LIN',
// new 5-7-05
	'UID_ROT_EULER_X', // these three use KID_FLOAT_BEZv2, pick this one, go bez all the way..
	'UID_ROT_EULER_Y',
	'UID_ROT_EULER_Z',
// new 12-2-05
	'UID_DIFFUSE',
// new 12-5-05
	'UID_POS_SAMP', // uses KID of KID_ROT/POS/SCL_LIN
	'UID_ROT_SAMP',
	'UID_SCL_SAMP', 
// new 12-7-05
	'UID_WEIGHTS1', // non blended verts
// new 12-19-05
	'UID_WEIGHTS2', // blended verts, bone index
	'UID_WEIGHTS2F', // blended verts, weight amount
// new 12-23-05
	'UID_AMBIENT',
	'UID_OPACITY', // a float
	'UID_SPECULAR',
	'UID_SHINE',
	'UID_EMIT',
// new 1-3-6
	'UID_TILING', // int with flags, 1 uwrap, 2 vwrap, etc... 
// new 12-29-8
	'UID_VC',  // vertex colors
// new 5-20-09
	'UID_VIS_SAMP',  // visibility track
// new 6-16-09
	'UID_REFL_AMT',	// reflection amount
	'UID_RTEX'		// reflection texture (NYI)
];

unchunker.prototype.chunktype = [
	 {"bytesize":1,"typename":"KID_I8"},
	 {"bytesize":2,"typename":"KID_U16o"},
	 {"bytesize":4,"typename":"KID_I32"},
	 {"bytesize":1,"typename":"KID_S8o"},
	 {"bytesize":2,"typename":"KID_S16o"},
	 {"bytesize":4,"typename":"KID_S32o"},
	 {"bytesize":8,"typename":"KID_VEC2"},
	{"bytesize":12,"typename":"KID_VEC3"},
	{"bytesize":16,"typename":"KID_VEC4"},
	 {"bytesize":0,"typename":"KID_CHUNK"},
	 {"bytesize":0,"typename":"KID_ENDCHUNK"},
	 {"bytesize":0,"typename":"KID_ARR"},
	 {"bytesize":8,"typename":"KID_IDX2"},
	{"bytesize":12,"typename":"KID_IDX3"},
	{"bytesize":16,"typename":"KID_IDX3M"},
	 {"bytesize":4,"typename":"KID_FLOAT"},
	{"bytesize":16,"typename":"KID_VEC3M"}, // 3 floats and 1 int (for mat idx)
// keyframe types
	{"bytesize":44,"typename":"KID_POS_BEZ"},
	{"bytesize":36,"typename":"KID_POS_TCB"},
	{"bytesize":16,"typename":"KID_POS_LIN"},
//	{"bytesize":28,"typename":"KID_ROT_EULER_X"},
//	{"bytesize":28,"typename":"KID_ROT_EULER_Y"},
//	{"bytesize":28,"typename":"KID_ROT_EULER_Z"},
	{"bytesize":20,"typename":"KID_ROT_BEZ"},
	{"bytesize":40,"typename":"KID_ROT_TCB"},
	{"bytesize":20,"typename":"KID_ROT_LIN"},
	{"bytesize":44,"typename":"KID_SCL_BEZ"},
	{"bytesize":36,"typename":"KID_SCL_TCB"},
	{"bytesize":16,"typename":"KID_SCL_LIN"},

	{"bytesize":28,"typename":"KID_FLOAT_BEZv2"},
	{"bytesize":68,"typename":"KID_POS_BEZv2"},
	 {"bytesize":0,"typename":"KID_ROT_BEZv2o"}, // don't use, use rot bez instead
	{"bytesize":68,"typename":"KID_SCL_BEZv2"},
// new 5-20-09
	{"bytesize":28,"typename":"KID_FLOAT_TCB"},
	 {"bytesize":8,"typename":"KID_FLOAT_LINEAR"}
];

function enumit(arr,key) {
	var ret = {};
    for (var i in arr) {
        var v = parseInt(i);
        if (key)
        	ret[arr[i][key]] = v;
       	else
        	ret[arr[i]] = v;
        	
    }
    return ret;
}

unchunker.prototype.chunknameenum = enumit(unchunker.prototype.chunkname);
unchunker.prototype.chunktypeenum = enumit(unchunker.prototype.chunktype,"typename");


unchunker.prototype.getchunkname_strs = function(cnidx) {
	var unk = "?";
	if (cnidx<=0 || cnidx>=this.chunkname.length)
		return unk;
	return this.chunkname[cnidx];
};

unchunker.prototype.getchunktype_strs = function(ctidx) {
	if (ctidx<0 || ctidx>=this.chunktype.length)
		alert("bad chunktype " + ctidx);
	return this.chunktype[ctidx].typename;
};

unchunker.prototype.getchunktype_bytesize = function(ctidx) {
	if (ctidx<0 || ctidx>=this.chunktype.length)
		alert("bad chunktype " + ctidx);
		//abc.def = ghi.klm;
//	logger("ctidx = %d\n",ctidx);
	return this.chunktype[ctidx].bytesize;
};

unchunker.prototype.getchunkheader = function() {
	this.lastnumele = this.lastdatasize = 0;
	var ret = {};
	ret.cn = 0;
	ret.ct = 0;
	ret.datasize = 0;
	ret.numele = 0;
	ret.elesize = 0;
	//logger("{getchunkheader}");
	ret.cn = this.readI32();
	if (ret.cn === null)
		return null;
	ret.ct = this.readI32();
	if (ret.ct === null)
		return null;
	ret.datasize = this.getchunktype_bytesize(ret.ct);
	if (!ret.datasize) {
		switch(ret.ct) {
		case this.chunktypeenum.KID_CHUNK:
			ret.datasize = this.readI32();
			break;
		case this.chunktypeenum.KID_ENDCHUNK:
			//logger("in endchunk");
			break;
		case this.chunktypeenum.KID_ARR:
			ret.numele = this.readI32();
			ret.ct = this.readI32();
			ret.elesize = this.getchunktype_bytesize(ret.ct);
			ret.datasize = ret.elesize*ret.numele;
			var pad = ret.datasize & 3;
			if (pad)
				ret.datasize += ( 4 - pad);
			break;
		default:
			ret.elesize = ret.datasize;
			alert("unknown chunkkeyword " + ret.ct);
			break;
		};
	}
	this.lastnumele = ret.numele;
	this.lastdatasize = ret.datasize;
	return ret;
};



unchunker.prototype.readI32 = function() {
	return freadI32(this.fr);
};

unchunker.prototype.readF32 = function() {
	return freadF32(this.fr);
};

unchunker.prototype.readVC2 = function() {
	var ret = {};
	ret.x = freadF32(this.fr);
	ret.y = freadF32(this.fr);
	return ret;
};

unchunker.prototype.readVC3 = function() {
	var ret = {};
	ret.x = freadF32(this.fr);
	ret.y = freadF32(this.fr);
	ret.z = freadF32(this.fr);
	return ret;
};

// read bws matrix into mat4
unchunker.prototype.readmat4 = function() {
	ret = mat4.create();
	var i,j;
	for (j=0;j<4;++j)
		for (i=0;i<3;++i)
			ret[j*4+i]=freadF32(this.fr);
	return ret;
};

unchunker.prototype.readVC4 = function() {
	var ret = {};
	ret.x = freadF32(this.fr);
	ret.y = freadF32(this.fr);
	ret.z = freadF32(this.fr);
	ret.w = freadF32(this.fr);
	return ret;
};

unchunker.prototype.readPOS_LIN = function() {
	var ret = {};
	ret.time = freadF32(this.fr);
	ret.x = freadF32(this.fr);
	ret.y = freadF32(this.fr);
	ret.z = freadF32(this.fr);
	return ret;
};

unchunker.prototype.readROT_LIN = function() {
	var ret = {};
	ret.time = freadF32(this.fr);
	ret.x = freadF32(this.fr);
	ret.y = freadF32(this.fr);
	ret.z = freadF32(this.fr);
	ret.w = freadF32(this.fr);
	return ret;
};

// returns a string that was padded to a multiple of 4 bytes
unchunker.prototype.readI8v = function() {
	if (!this.lastnumele)
		alert("not an array");
	var s = "";
	var i;
	var ca = freadI8v(this.fr,this.lastnumele);
	for (i=0;i<this.lastnumele;++i) {
		if (!ca[i])
			;//alert("null in string");
		else
			s += String.fromCharCode(ca[i]);
	}
	this.lastnumele&=3;
	if (this.lastnumele) {
		this.lastnumele = 4 - this.lastnumele;
		fskip(this.fr,this.lastnumele);
	}
	this.lastnumele=0;
	return s;
};

unchunker.prototype.readI32v = function() {
	var ret = freadI32v(this.fr,this.lastnumele);
	this.lastnumele = 0;
	return ret;
};

unchunker.prototype.readVC2v = function() {
	var i;
	var ret = [];
	for (i=0;i<this.lastnumele;++i) 
		ret.push(this.readVC2());
	this.lastnumele = 0;
	return ret;
};

unchunker.prototype.readVC3v = function() {
	var i;
	var ret = [];
	for (i=0;i<this.lastnumele;++i) 
		ret.push(this.readVC3());
	this.lastnumele = 0;
	return ret;
};

unchunker.prototype.readVC3Mv = function() {
	var i;
	var ret = [];
	for (i=0;i<this.lastnumele;++i) { 
		//var ele = {};
		var ele = this.readVC3();
		ele.matidx = freadI32(this.fr);
		ret.push(ele);
	}
	this.lastnumele = 0;
	return ret;
};

unchunker.prototype.readIDX3v = function() {
	var i;
	var ret = [];
	for (i=0;i<this.lastnumele;++i) { 
		var ele = {};
		ele.idx = freadI32v(this.fr,3);
		ret.push(ele);
	}
	this.lastnumele = 0;
	return ret;
};

unchunker.prototype.readIDX3Mv = function() {
	var i;
	var ret = [];
	for (i=0;i<this.lastnumele;++i) { 
		var ele = {};
		ele.idx = freadI32v(this.fr,3);
		ele.matidx = freadI32(this.fr);
		ret.push(ele);
	}
	this.lastnumele = 0;
	return ret;
};

unchunker.prototype.skipdata = function() {
	fskip(this.fr,this.lastdatasize);
	this.lastdatasize = this.lastnumele = 0;
};

function unchunktest(ab) {
	//logger_str = "";
	logger("in unchunktest\n");
	var chunktestdepth = 10;
	var depth = 0;
	var uc = new unchunker(ab);
	var chi;
	while(chi = uc.getchunkheader()) {
		var indentstr = "";
		var i;
		for (i=0;i<depth;++i)
			indentstr += "    ";
		if (depth < 0)
			indentstr="???";
		if (!chi.numele)
			logger(indentstr + 
				"chunk: name '" + uc.getchunkname_strs(chi.cn) + 
				"', type '" + uc.getchunktype_strs(chi.ct) + 
				"', datasize " + chi.datasize + 
				", ");
		else
			logger(indentstr + 
				"chunk: name '" + uc.getchunkname_strs(chi.cn) + 
				"', array[" + chi.numele +
				"] of type '" + uc.getchunktype_strs(chi.ct) + 
				"', eledatasize " + chi.elesize +
				", totaldatasize " + chi.datasize + 
				", ");
		if (chi.ct==uc.chunktypeenum.KID_CHUNK) {	// don't skip subchunk data
			if (depth<chunktestdepth) {
				logger("ignoring data size of chunk, entering chunk\n");
				++depth; // enter subchunk data
				continue;
			}
		}
// else skip subchunk data
		if (!chi.numele) { // it's not an array
			switch(chi.ct) {
			case uc.chunktypeenum.KID_I32: // object id's (maybe)
				var i32 = uc.readI32();
				logger("DATA: I32 = " + i32 + "\n");
				break;
			case uc.chunktypeenum.KID_FLOAT: // object id's (maybe)
				var f32 = uc.readF32().toFixed(3);
				logger("DATA: F32 = " + f32 + "\n");
				break;
			case uc.chunktypeenum.KID_VEC2: // 2 floats (for uvs, maybe)
				var vc2 = uc.readVC2();
				logger("DATA: VEC2 (" + vc2.x.toFixed(3) + " " + vc2.y.toFixed(3) + ")\n");
				break;
			case uc.chunktypeenum.KID_VEC3: // 3 floats (for unpadded 3d points)
				var vc3 = uc.readVC3();
				logger("DATA: VEC3 (" + vc3.x.toFixed(3) + " " + vc3.y.toFixed(3) + " " + vc3.z.toFixed(3) + ")\n");
				break;
			case uc.chunktypeenum.KID_VEC4: // 4 floats (for quats or rotaxis)
				var vc4 = uc.readVC4();
				logger("DATA: VEC4 (" + vc4.x.toFixed(3) + " " + vc4.y.toFixed(3) + " " + vc4.z.toFixed(3) + " " + vc4.w.toFixed(3) + ")\n");
				break;
			case uc.chunktypeenum.KID_ENDCHUNK:
				logger("ENDCHUNK: DONE\n");
				uc.skipdata();
				--depth;
				break;
			case uc.chunktypeenum.KID_CHUNK:
				logger("CHUNK: SKIPPING\n");
				uc.skipdata();
				break;
			default:
				logger("DATA: SKIPPING\n");
				uc.skipdata();
				break;
			}
		} else { // it's an array
			var i;
			var head = 20; // how much to print
			switch(chi.ct) {
			case uc.chunktypeenum.KID_I8:
				var i8v=uc.readI8v();
				logger("DATA ARRAY: I8 '" + i8v + "'\n");
				break;
			case uc.chunktypeenum.KID_I32:
				var i32v=uc.readI32v();
				logger("\n");
				for (i=0;i<i32v.length;++i) {
					if (i >= head)
						break;
					logger(indentstr + "   DATA ARRAY[" + i + "]: I32 " + i32v[i] + "\n");
				}
				break;
			case uc.chunktypeenum.KID_VEC2: // 2 floats (for uvs, maybe)
				logger("\n");
				var vc2v=uc.readVC2v();
				for (i=0;i<vc2v.length;++i) {
					if (i >= head)
						break;
					logger(indentstr + "   DATA ARRAY[" + i + "]: VEC2 " + vc2v[i].x.toFixed(3) + " " + vc2v[i].y.toFixed(3) + "\n");
				}
				break;
			case uc.chunktypeenum.KID_VEC3: // 3 floats (for unpadded 3d points)
				logger("\n");
				var vc3v=uc.readVC3v();
				for (i=0;i<vc3v.length;++i) {
					if (i >= head)
						break;
					logger(indentstr + "   DATA ARRAY[" + i + "]: VEC3 " + vc3v[i].x.toFixed(3) + " " + vc3v[i].y.toFixed(3) + " " + vc3v[i].z.toFixed(3) + "\n");
				}
				break;
			case uc.chunktypeenum.KID_IDX3: //%s for tfaces
				logger("\n");
				var idx3v=uc.readIDX3v();
				for (i=0;i<idx3v.length;++i) {
					if (i >= head)
						break;
					logger(indentstr + "   DATA ARRAY[" + i + "]: TFACE " + idx3v[i].idx[0] + " " + idx3v[i].idx[1] + " " + idx3v[i].idx[2] + "\n");
				}
				break;
			case uc.chunktypeenum.KID_IDX3M: // for faces
				logger("\n");
				var idx3mv=uc.readIDX3Mv();
				for (i=0;i<idx3mv.length;++i) {
					if (i >= head)
						break;
					logger(indentstr + "   DATA ARRAY[" + i + "]: FACE with matidx " + idx3mv[i].idx[0] + " " + idx3mv[i].idx[1] + " " + idx3mv[i].idx[2] + " " + idx3mv[i].matidx + "\n");
				}
				break;
			case uc.chunktypeenum.KID_VEC3M: // 3 floats and 1 int (for mat idx)
				logger("\n");
				var vc3mv=uc.readVC3Mv();
				for (i=0;i<vc3mv.length;++i) {
					if (i >= head)
						break;
					logger(indentstr + "   DATA ARRAY[" + i + "]: VERT with matidx " + vc3mv[i].x.toFixed(3) + " " + vc3mv[i].y.toFixed(3) + " " + vc3mv[i].z.toFixed(3) + " " + vc3mv[i].matidx + "\n");
				}
				break;
			default:
				logger("DATA ARRAY: SKIPPING\n");
				uc.skipdata();
				break;
			}
		}
	}
	logger("done!");
	htmladd("responsephp","p","(" + logger_str + ")");
}
