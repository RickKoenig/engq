// conversions
// Is ao an Array ?
function meshisarray(ao) {
	if (ao === undefined)
		return false;
	return ao.constructor === Array;
}

// Is vo an array of verts x,y,z ?
function meshisvertarray(vo) {
	if (vo === undefined)
		return false;
	if (vo.constructor !== Array)
		return false;
	var ele = vo[0];
	if (! ele instanceof Object)
		return false;
	if (ele === undefined)
		return true;
	if (ele.x === undefined)
		return false;
	return true;
}

// Is vo an array of faces idx[3] ?
function meshisfacearray(fo) {
	if (fo === undefined)
		return false;
	if (fo.constructor !== Array)
		return false;
	var ele = fo[0];
	if (! ele instanceof Object)
		return false;
	if (ele === undefined)
		return false;
	if (ele.idx === undefined)
		return false;
	if (ele.idx.constructor !== Array && ele.idx.constructor !== Int32Array)
		return false;
	return true; // it's an array of face objects
}

// convert array of verts to array of floats
function meshvert2array(o,dim) {
	if (!meshisvertarray(o)) {
		if (meshisarray(o)) {
			if (o.length % dim != 0)
				return null;
			return o; // already an array of floats of correct multiple fo dim
		}
	}
	var a = [];
	var i;
	for(i=0;i<o.length;++i) {
		var e = o[i];
		if (dim >= 1)
			a.push(e.x);
		if (dim >= 2)
			a.push(e.y);
		if (dim >= 3)
			a.push(e.z);
		if (dim >= 4)
			a.push(e.w);
	}
	return a;
}

// convert array of floats to array of verts
function mesharray2vert(a,dim) {
	if (meshisvertarray(a))
		return a;
	if (a.length % dim != 0)
		return null;
	var o = [];
	var i;
	for (i=0;i<a.length;i+=dim) {
		var e = {};
		if (dim >= 1)
			e.x = a[i];
		if (dim >= 2)
			e.y = a[i+1];
		if (dim >= 3)
			e.z = a[i+2];
		if (dim >= 4)
			e.w = a[i+3];
		o.push(e);
	}	
	return o;
}

// convert array of faces to array of shorts
function meshface2array(f,dim) {
	if (dim != 3 && dim != 4)
		return null;
	if (!meshisfacearray(f)) {
		if (meshisarray(f)) {
			if (f.length % dim != 0)
				return null;
			return f;
		}
	}
	var a = [];
	var i;
	for(i=0;i<f.length;++i) {
		var e = f[i];
		var idxs = e.idx;
		a.push(idxs[0]);
		a.push(idxs[1]);
		a.push(idxs[2]);
		if (dim >= 4)
			a.push(e.matidx);
	}
	return a;
}

// convert array of shorts to array of faces
function mesharray2face(a,dim) {
	if (dim != 3 && dim != 4)
		return null;
	if (meshisfacearray(a))
		return a;
	if (a.length % dim != 0)
		return null;
	var f = [];
	var i;
	for (i=0;i<a.length;i+=dim) {
		var e = {};
		e.idx = [
			a[i],
			a[i + 1],
			a[i + 2]
		];
		if (dim >= 4)
			e.matidx = a[i + 3];
		f.push(e);
	}
	return f;
}

// surface patch functions
function cylinderxz_mid_surf(rad,hit) {
	var functor = function(p,q) {
		var v = [],n = [];
		var sa,ca;
		p = 2*Math.PI*p;
		q = hit*(1-q);
		sa = Math.sin(p);
		ca = Math.cos(p);
		n[0] = ca;
		n[1] = 0;
		n[2] = sa;
		v[0] = rad*n[0];
		v[1] = q;
		v[2] = rad*n[2];
		return {"v":v,"n":n};
	};
	return functor;
}

function cylinderxz_top_surf(rad,hit) {
	var functor = function(p,q) {
		var v = [],n = [];
		var sa,ca;
		p = 2*Math.PI*p;
		q = rad*q;
		sa = Math.sin(p);
		ca = Math.cos(p);
		n[0] = 0;
		n[1] = 1;
		n[2] = 0;
		v[0] = q*ca;
		v[1] = hit;
		v[2] = q*sa;
		return {"v":v,"n":n};
	};
	return functor;
}

// cylinder bottom
function cylinderxz_bot_surf(rad,hit) {
	var functor = function(p,q) {
		var v = [],n = [];
		var sa,ca;
		p = 2*Math.PI*p;
		q = rad*(1-q);
		sa = Math.sin(p);
		ca = Math.cos(p);
		n[0] = 0;
		n[1] = -1;
		n[2] = 0;
		v[0] = q*ca;
		v[1] = 0;
		v[2] = q*sa;
		return {"v":v,"n":n};
	};
	return functor;
}

function conexz_mid_surf(rad,hit) {
	var functor = function(p,q) {
		var v = [],n = [];
		var ns = [hit,rad,0];
		vec3.normalize(ns,ns);
		var sa,ca;
		p = 2*Math.PI*p;
		var h = hit*(1-q);
		sa = Math.sin(p);
		ca = Math.cos(p);
		var nc = [ca,0,sa];
		var radh = rad*q;
		v[0] = radh*nc[0];
		v[1] = h;
		v[2] = radh*nc[2];
		n[0] = ns[0] * nc[0];
		n[1] = ns[1];
		n[2] = ns[0] * nc[2];
		//n[0] = 0;
		//n[1] = 0;
		//n[2] = -1;
		var m2 = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
		if (m2 < .95 || m2 > 1.05)
			logger("bad m2\n");
		return {"v":v,"n":n};
	};
	return functor;
}

// cylinder bottom
function conexz_bot_surf(rad,hit) {
	var functor = function(p,q) {
		var v = [],n = [];
		var sa,ca;
		p = 2*Math.PI*p;
		q = rad*(1-q);
		sa = Math.sin(p);
		ca = Math.cos(p);
		n[0] = 0;
		n[1] = -1;
		n[2] = 0;
		v[0] = q*ca;
		v[1] = 0;
		v[2] = q*sa;
		return {"v":v,"n":n};
	};
	return functor;
}

function spheref_surf(rad) {
	var functor = function(p,q) {
		var v = [],n = [];
		var sp,cp,sq,cq;
		p = 2*Math.PI*p;
		q = Math.PI/2-Math.PI*q;
		sp = Math.sin(p);
		cp = Math.cos(p);
		sq = Math.sin(q);
		cq = Math.cos(q);
		n[0] =  sp*cq;
		n[1] =  sq;
		n[2] = -cp*cq;
		v[0] = rad*n[0];
		v[1] = rad*n[1];
		v[2] = rad*n[2];
		return {"v":v,"n":n};
	};
	return functor;
}

function spheref_surf3(rad3) {
	var rad3invtranspose = [1.0/rad3[0],1.0/rad3[1],1.0/rad3[2]];
	var functor = function(p,q) {
		var v = [],n = [];
		var sp,cp,sq,cq;
		p = 2*Math.PI*p;
		q = Math.PI/2-Math.PI*q;
		sp = Math.sin(p);
		cp = Math.cos(p);
		sq = Math.sin(q);
		cq = Math.cos(q);
		v[0] = sp*cq;
		v[1] = sq;
		v[2] = -cp*cq;
		vec3.copy(n,v);
		vec3.mul(n,rad3invtranspose,v);
		vec3.mul(v,rad3,v);
		vec3.normalize(n,n);
		return {"v":v,"n":n};
	};
	return functor;
}

function torusxz_surf(rad0,rad1) {
	var functor = function(p,q) {
		var v = [],n = [];
		p *= 2*Math.PI;
		q *= 2*Math.PI;
		var sp = Math.sin(p);
		var cp = Math.cos(p);
		var sq = Math.sin(q);
		var cq = Math.cos(q);
		var radv = rad0 - rad1*cq;
		v[0] = radv*sp;
		v[1] = rad1*sq;
		v[2] = -radv*cp;
		n[0] = -cq*sp;
		n[1] = sq;
		n[2] = cq*cp;
		return {"v":v,n:n};
	};
    return functor;
}

function buildpatch(np,nq,tileu,tilev,funcsurf) {
	var mesh = {};
/// build the verts, uvs and norms for the model
	var nv = (np+1)*(nq+1);
	var nf = np*nq*2;
	var verts = []; //struct pointf3* verts=new pointf3[nv];
	var norms = []; //struct pointf3* verts=new pointf3[nv];
	var uvs = []; //struct uv* uvs=new uv[nv];
	var faces = [];
	var i,j,k=0;
	var trati = tileu/np;
	var tratj = tilev/nq;
	for (i=0;i<=np;++i) {
	    var s = i/np;
		for (j=0;j<=nq;++j) {
			var t = j/nq;
			coord = funcsurf(s,t);
			verts.push(coord.v[0]);
			verts.push(coord.v[1]);
			verts.push(coord.v[2]);
			if (coord.n) {
				norms.push(coord.n[0]);
				norms.push(coord.n[1]);
				norms.push(coord.n[2]);
			}
 			uvs.push(i*trati);
 			uvs.push(j*tratj);
		}
	}

	for (i=0;i<np;++i) {
		for (j=0;j<nq;++j) {
			var f0=j+(nq+1)*i;
			var f1=f0+(nq+1);
			var f2=f0+1;
			var f3=f1+1;
			faces.push(f0);
			faces.push(f1);
			faces.push(f2);
			faces.push(f3);
			faces.push(f2);
			faces.push(f1);
		}
	}
	mesh.verts = verts;
	if (norms.length)
		mesh.norms = norms;
	mesh.uvs = uvs;
	mesh.faces = faces;
	return mesh;
}

// pointing straight up
var paperairplanemesh = {
	"verts": [
		0,.5,0,
		0,-.5,0,
		-1/3,-.5,0,
		
		0,.5,0,
		-1/3,-.5,0,
		0,-.5,0,
		
		0,.5,0,
		1/3,-.5,0,
		0,-.5,0,

		0,.5,0,
		0,-.5,0,
		1/3,-.5,0,
		
		0,.5,0,
		0,-.5,0,
		0,-.5,1/6,
		
		0,.5,0,
		0,-.5,1/6,
		0,-.5,0
	],
	cverts: [
		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		0,1,0,1,
		0,1,0,1,
		0,1,0,1,

		1,1,0,1,
		1,1,0,1,
		1,1,0,1,

		0,1,1,1,
		0,1,1,1,
		0,1,1,1,
		
		1,0,1,1,
		1,0,1,1,
		1,0,1,1,

		0,0,1,1,
		0,0,1,1,
		0,0,1,1

	]
};

// switched the front and back because we're outside the box (for skybox textures)
var prismmesh = {
	"baseverts": [ // centered
	// front (POSZ) switched
		-1, 1,-1,
		 1, 1,-1,
		-1,-1,-1,
		 1,-1,-1,
	// back (NEGZ) switched
		 1, 1, 1,
		-1, 1, 1,
		 1,-1, 1,
		-1,-1, 1,
	// right (POSX)
		 1, 1,-1,
		 1, 1, 1,
		 1,-1,-1,
		 1,-1, 1,
	// left (NEGX)
		-1, 1, 1,
		-1, 1,-1,
		-1,-1, 1,
		-1,-1,-1,
	// top (POSY)
		-1, 1, 1,
		 1, 1, 1,
		-1, 1,-1,
		 1, 1,-1,
	// bot (NEGY)
		-1,-1,-1,
		 1,-1,-1,
		-1,-1, 1,
		 1,-1, 1
	],
	"baseverts2": [ // 0 to 1 // half size in all dimensions as centered
	// front (POSZ) switched
		 0, 1, 0,
		 1, 1, 0,
		 0, 0, 0,
		 1, 0, 0,
	// back (NEGZ) switched
		 1, 1, 1,
		 0, 1, 1,
		 1, 0, 1,
		 0, 0, 1,
	// right (POSX)
		 1, 1, 0,
		 1, 1, 1,
		 1, 0, 0,
		 1, 0, 1,
	// left (NEGX)
		 0, 1, 1,
		 0, 1, 0,
		 0, 0, 1,
		 0, 0, 0,
	// top (POSY)
		 0, 1, 1,
		 1, 1, 1,
		 0, 1, 0,
		 1, 1, 0,
	// bot (NEGY)
		 0, 0, 0,
		 1, 0, 0,
		 0, 0, 1,
		 1, 0, 1
	],
	"norms": [
	// front
		 0, 0,-1,
		 0, 0,-1,
		 0, 0,-1,
		 0, 0,-1,
	// back
		 0, 0, 1,
		 0, 0, 1,
		 0, 0, 1,
		 0, 0, 1,
	// right
		 1, 0, 0,
		 1, 0, 0,
		 1, 0, 0,
		 1, 0, 0,
	// left
		-1, 0, 0,
		-1, 0, 0,
		-1, 0, 0,
		-1, 0, 0,
	// top
		 0, 1, 0,
		 0, 1, 0,
		 0, 1, 0,
		 0, 1, 0,
	// bot
		 0,-1, 0,
		 0,-1, 0,
		 0,-1, 0,
		 0,-1, 0,
	],
	"uvs": [
	// front
		0,0,
		1,0,
		0,1,
		1,1,
	// back
		0,0,
		1,0,
		0,1,
		1,1,
	// right
		0,0,
		1,0,
		0,1,
		1,1,
	// left
		0,0,
		1,0,
		0,1,
		1,1,
	// top
		0,0,
		1,0,
		0,1,
		1,1,
	// bot
		0,0,
		1,0,
		0,1,
		1,1
	],
	"faces": [	
	// front
		 0, 1, 2,
		 3, 2, 1,
	// back
		 4, 5, 6,
		 7, 6, 5,
	// right
		 8, 9,10,
		11,10, 9,
	// left
		12,13,14,
		15,14,13,
	// top
		16,17,18,
		19,18,17,
	// bot
		20,21,22,
		23,22,21
	]
};

// inside the box
var ERR2 = 1.0/200.0;
var skyboxmesh = {
	"baseverts": [
	// back (POSZ)
		-1, 1, 1,
		 1, 1, 1,
		-1,-1, 1,
		 1,-1, 1,
	// front (NEGZ)
		 1, 1,-1,
		-1, 1,-1,
		 1,-1,-1,
		-1,-1,-1,
	// right (POSX)
		 1, 1, 1,
		 1, 1,-1,
		 1,-1, 1,
		 1,-1,-1,
	// left (NEGX)
		-1, 1,-1,
		-1, 1, 1,
		-1,-1,-1,
		-1,-1, 1,
	// top (POSY)
		-1, 1,-1,
		 1, 1,-1,
		-1, 1, 1,
		 1, 1, 1,
	// bot (NEGY)
		-1,-1, 1,
		 1,-1, 1,
		-1,-1,-1,
		 1,-1,-1
	],
	"uvs": [
	// front
		ERR2,ERR2,
		1-ERR2,ERR2,
		ERR2,1-ERR2,
		1-ERR2,1-ERR2,
	// back
		ERR2,ERR2,
		1-ERR2,ERR2,
		ERR2,1-ERR2,
		1-ERR2,1-ERR2,
	// right
		ERR2,ERR2,
		1-ERR2,ERR2,
		ERR2,1-ERR2,
		1-ERR2,1-ERR2,
	// left
		ERR2,ERR2,
		1-ERR2,ERR2,
		ERR2,1-ERR2,
		1-ERR2,1-ERR2,
	// top
		ERR2,ERR2,
		1-ERR2,ERR2,
		ERR2,1-ERR2,
		1-ERR2,1-ERR2,
	// bot
		ERR2,ERR2,
		1-ERR2,ERR2,
		ERR2,1-ERR2,
		1-ERR2,1-ERR2
	],
	"faces": [	
	// front
		 0, 1, 2,
		 3, 2, 1,
	// back
		 4, 5, 6,
		 7, 6, 5,
	// right
		 8, 9,10,
		11,10, 9,
	// left
		12,13,14,
		15,14,13,
	// top
		16,17,18,
		19,18,17,
	// bot
		20,21,22,
		23,22,21
	]
};

function buildprismmodel(name,size,texname,shadername) {
    var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
		mod.setshader(shadername);
	    var i,j;
	    prismmesh.verts = [];
		for (j=0;j<24;++j) {
			for (i=0;i<3;++i) {
				prismmesh.verts.push(size[i]*prismmesh.baseverts[3*j+i]);
			}
		}
	    mod.setmesh(prismmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}
	
function buildprism(name,size,texname,shadername) {
	var mod = buildprismmodel(name,size,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildprismmodel2(name,size,texname,shadername) {
    var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
		mod.setshader(shadername);
	    var i,j;
	    prismmesh.verts = [];
		for (j=0;j<24;++j) {
			for (i=0;i<3;++i) {
				prismmesh.verts.push(size[i]*prismmesh.baseverts2[3*j+i]);
			}
		}
	    mod.setmesh(prismmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}
	
function buildprism2(name,size,texname,shadername) {
	var mod = buildprismmodel2(name,size,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildprismmodel6(name,size,basetexname,shadername) {
    var mod = Model2.createmodel(name);
	if (mod.refcount == 1) {
		var i,j,k;
	    prismmesh.verts = [];
		for (j=0;j<24;++j) {
			for (i=0;i<3;++i) {
				prismmesh.verts.push(size[i]*prismmesh.baseverts[3*j+i]);
			}
		}
	    mod.setmesh(prismmesh);
		var keys = Object.keys(cubeenums);
		for (var k=0;k<keys.length;++k) {
			var key = keys[k];
			mod.addmat(shadername,key+"_"+basetexname,2,4);
			//mod.addmat("tex","maptestnck.png",2,4);
		}
	    mod.commit();
	}
    return mod;
}

function buildprism6(name,size,basetexname,shadername) {
	var mod = buildprismmodel6(name,size,basetexname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildskyboxmodel(name,size,basetexname,shadername) {
	globaltexflags = textureflagenums.CLAMPU | textureflagenums.CLAMPV;
    var mod = Model2.createmodel(name);
    mod.flags = modelflagenums.NOZBUFFER | modelflagenums.ISSKYBOX;
	if (mod.refcount == 1) {
		var i,j,k;
	    skyboxmesh.verts = [];
		for (j=0;j<24;++j) {
			for (i=0;i<3;++i) {
				skyboxmesh.verts.push(size[i]*skyboxmesh.baseverts[3*j+i]);
			}
		}
	    mod.setmesh(skyboxmesh);
		var keys = Object.keys(cubeenums);
		for (var k=0;k<keys.length;++k) {
			var key = keys[k];
			mod.addmat(shadername,key+"_"+basetexname,2,4);
			//mod.addmat("tex","maptestnck.png",2,4);
		}
	    mod.commit();
	}
	globaltexflags = 0;
    return mod;
}

function buildskybox(name,size,basetexname,shadername) {
	var mod = buildskyboxmodel(name,size,basetexname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

// sphere
// fix 'wrap gap' in sphere mesh, after random tweeking of verts
// 2d array runs down in y
function spherefixpatch(mesh) {
	var p2left = 0;
	var p3left = 0;
	var p2right = spherepatchi*(spherepatchj+1);
	var p3right = p2right;
	p2right *= 2;
	p3right *= 3;
	// fix prime meridian
	var i,j;
	for (j=0;j<=spherepatchj;++j) {
		mesh.verts[p3right  ] = mesh.verts[p3left  ];
		mesh.verts[p3right+1] = mesh.verts[p3left+1];
		mesh.verts[p3right+2] = mesh.verts[p3left+2];
		if (mesh.norms) {
			mesh.norms[p3right  ] = mesh.norms[p3left  ];
			mesh.norms[p3right+1] = mesh.norms[p3left+1];
			mesh.norms[p3right+2] = mesh.norms[p3left+2];
		}
		mesh.uvs[p2right  ] = mesh.uvs[p2left  ] + spherepatchu;
		mesh.uvs[p2right+1] = mesh.uvs[p2left+1];
		p2left += 2;
		p3left += 3;
		p2right += 2;
		p3right += 3;
	}
	// fix top verts
	var step3 = 3*(spherepatchj+1);
	var p3top = step3;
	for (i=1;i<=spherepatchi;++i) {
		mesh.verts[p3top  ] = mesh.verts[0];
		mesh.verts[p3top+1] = mesh.verts[1];
		mesh.verts[p3top+2] = mesh.verts[2];
		p3top += step3;
	}
	// fix bot verts
	var p3bot0 = 3*spherepatchj;
	var p3bot = p3bot0 + step3;
	for (i=1;i<=spherepatchi;++i) {
		mesh.verts[p3bot  ] = mesh.verts[p3bot0  ];
		mesh.verts[p3bot+1] = mesh.verts[p3bot0+1];
		mesh.verts[p3bot+2] = mesh.verts[p3bot0+2];
		p3bot += step3;
	}
}

var spherepatchi = 40;
var spherepatchj = 40;
var spherepatchu = 3;
var spherepatchv = 3;

function buildspheremesh(rad) {
	return buildpatch(spherepatchi,spherepatchj,spherepatchu,spherepatchv,spheref_surf(rad));
}

function buildspheremodel(name,rad,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var spheremesh = buildspheremesh(rad);
	    mod.setmesh(spheremesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildsphere(name,rad,texname,shadername) {
	var mod = buildspheremodel(name,rad,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildspheremodel2t(name,rad,texname1,texname2,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var spheremesh = buildspheremesh(rad);
	    mod.setmesh(spheremesh);
	    mod.settexture(texname1);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

function buildsphere2t(name,rad,texname1,texname2,shadername) {
	var mod = buildspheremodel2t(name,rad,texname1,texname2,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildspheremesh3(rad3) {
	return buildpatch(spherepatchi,spherepatchj,spherepatchu,spherepatchv,spheref_surf3(rad3));
}

function buildspheremodel3(name,rad3,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var spheremesh3 = buildspheremesh3(rad3);
	    mod.setmesh(spheremesh3);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildsphere3(name,rad3,texname,shadername) {
	var mod = buildspheremodel3(name,rad3,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

var cylpatchi = 40;
var cylpatchj = 5;
var cylpatchu = 3;
var cylpatchv = 3;

// cylinder middle
function buildcylinderxzmeshmid(rad,hit) {
	return buildpatch(cylpatchi,cylpatchj,cylpatchu,cylpatchv,cylinderxz_mid_surf(rad,hit));
}

function buildcylinderxzmidmodel(name,rad,hit,texname,shadername) {
	var mod = Model.createmodel(name + "_mid");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var cylmesh = buildcylinderxzmeshmid(rad,hit);
	    mod.setmesh(cylmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

// cylinder top
function buildcylinderxzmeshtop(rad,hit) {
	return buildpatch(cylpatchi,cylpatchj,cylpatchu,cylpatchv,cylinderxz_top_surf(rad,hit));
}

function buildcylinderxztopmodel(name,rad,hit,texname,shadername) {
	var mod = Model.createmodel(name + "_top");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var cylmesh = buildcylinderxzmeshtop(rad,hit);
	    mod.setmesh(cylmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

// cylinder bottom
function buildcylinderxzmeshbot(rad,hit) {
	return buildpatch(cylpatchi,cylpatchj,cylpatchu,cylpatchv,cylinderxz_bot_surf(rad,hit));
}

function buildcylinderxzmidmodel2t(name,rad,hit,texname,texname2,shadername) {
	var mod = Model.createmodel(name + "_mid");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var cylmesh = buildcylinderxzmeshmid(rad,hit);
	    mod.setmesh(cylmesh);
	    mod.settexture(texname);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

function buildcylinderxztopmodel2t(name,rad,hit,texname,texname2,shadername) {
	var mod = Model.createmodel(name + "_top");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var cylmesh = buildcylinderxzmeshtop(rad,hit);
	    mod.setmesh(cylmesh);
	    mod.settexture(texname);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

function buildcylinderxzbotmodel2t(name,rad,hit,texname,texname2,shadername) {
	var mod = Model.createmodel(name + "_bot");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var cylmesh = buildcylinderxzmeshbot(rad,hit);
	    mod.setmesh(cylmesh);
	    mod.settexture(texname);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

// cylinder tree
function buildcylinderxz(name,rad,hit,texname,shadername) {
	// the whole cylinder
	var grp = new Tree2(name);
	// mid section
	var mid = new Tree2(name);
	var modmid = buildcylinderxzmidmodel(name,rad,hit,texname,shadername);
	mid.setmodel(modmid);
	grp.linkchild(mid);
	// top section
	var top = new Tree2(name);
	var modtop = buildcylinderxztopmodel(name,rad,hit,texname,shadername);
	top.setmodel(modtop);
	grp.linkchild(top);
	// bot section
	var bot = new Tree2(name);
	var modbot = buildcylinderxzbotmodel(name,rad,hit,texname,shadername);
	bot.setmodel(modbot);
	grp.linkchild(bot);
	// everything
	return grp;
}

function buildcylinderxzbotmodel(name,rad,hit,texname,shadername) {
	var mod = Model.createmodel(name + "_bot");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var cylmesh = buildcylinderxzmeshbot(rad,hit);
	    mod.setmesh(cylmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

// cylinder tree
function buildcylinderxz2t(name,rad,hit,texname,texname2t,shadername) {
	// the whole cylinder
	var grp = new Tree2(name);
	// mid section
	var mid = new Tree2(name);
	var modmid = buildcylinderxzmidmodel2t(name,rad,hit,texname,texname2t,shadername);
	mid.setmodel(modmid);
	grp.linkchild(mid);
	// top section
	var top = new Tree2(name);
	var modtop = buildcylinderxztopmodel2t(name,rad,hit,texname,texname2t,shadername);
	top.setmodel(modtop);
	grp.linkchild(top);
	// bot section
	var bot = new Tree2(name);
	var modbot = buildcylinderxzbotmodel2t(name,rad,hit,texname,texname2t,shadername);
	bot.setmodel(modbot);
	grp.linkchild(bot);
	// everything
	return grp;
}

var conepatchi = 40;
var conepatchj = 5;
var conepatchu = 3;
var conepatchv = 3;

// cone middle
function buildconexzmeshmid(rad,hit) {
	return buildpatch(conepatchi,conepatchj,conepatchu,conepatchv,conexz_mid_surf(rad,hit));
}

function buildconexzmidmodel(name,rad,hit,texname,shadername) {
	var mod = Model.createmodel(name + "_mid");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var conemesh = buildconexzmeshmid(rad,hit);
	    mod.setmesh(conemesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildconexzmidmodel2t(name,rad,hit,texname,texname2,shadername) {
	var mod = Model.createmodel(name + "_mid");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var conemesh = buildconexzmeshmid(rad,hit);
	    mod.setmesh(conemesh);
	    mod.settexture(texname);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

// cone bottom
function buildconexzmeshbot(rad,hit) {
	return buildpatch(conepatchi,conepatchj,conepatchu,conepatchv,conexz_bot_surf(rad,hit));
}

function buildconexzbotmodel(name,rad,hit,texname,shadername) {
	var mod = Model.createmodel(name + "_bot");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var conemesh = buildconexzmeshbot(rad,hit);
	    mod.setmesh(conemesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildconexzbotmodel2t(name,rad,hit,texname,texname2,shadername) {
	var mod = Model.createmodel(name + "_bot");
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var conemesh = buildconexzmeshbot(rad,hit);
	    mod.setmesh(conemesh);
	    mod.settexture(texname);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

// cone tree
function buildconexz(name,rad,hit,texname,shadername) {
	// the whole cone
	var grp = new Tree2(name);
	// mid section
	var mid = new Tree2(name);
	var modmid = buildconexzmidmodel(name,rad,hit,texname,shadername);
	mid.setmodel(modmid);
	grp.linkchild(mid);
	// bot section
	var bot = new Tree2(name);
	var modbot = buildconexzbotmodel(name,rad,hit,texname,shadername);
	bot.setmodel(modbot);
	grp.linkchild(bot);
	// everything
	return grp;
}

// cone tree
function buildconexz2t(name,rad,hit,texname,texname2,shadername) {
	// the whole cone
	var grp = new Tree2(name);
	// mid section
	var mid = new Tree2(name);
	var modmid = buildconexzmidmodel2t(name,rad,hit,texname,texname2,shadername);
	mid.setmodel(modmid);
	grp.linkchild(mid);
	// bot section
	var bot = new Tree2(name);
	var modbot = buildconexzbotmodel2t(name,rad,hit,texname,texname2,shadername);
	bot.setmodel(modbot);
	grp.linkchild(bot);
	// everything
	return grp;
}

// planexy,planexz
var planepatchi = 40;
var planepatchj = 40;
//var planepatchu = 3;
//var planepatchv = 3;
var planepatchu = 1;
var planepatchv = 1;

// planexz
function planexz_surf(wid,hit) {
	var startx = -wid;
	var stepx = 2*wid;
	var startz = hit;
	var stepz = -2*hit;
	var functor = function(p,q) {
		var v = [],n = [];
		n[0] = 0;
		n[1] = 1;
		n[2] = 0;
		v[0] = startx + p*stepx;
		v[1] = 0;
		v[2] = startz + q*stepz;
		return {"v":v,"n":n};
	};
	return functor;
}

function buildplanexzmesh(wid,hit) {
	return buildpatch(planepatchi,planepatchj,planepatchu,planepatchv,planexz_surf(wid,hit));
}

function buildplanexzmodel(name,wid,hit,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		//var spheremesh = buildpatch(8,8,3,3,spheref_surf(rad));
		var planexzmesh = buildplanexzmesh(wid,hit);
	    mod.setmesh(planexzmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildplanexz(name,wid,hit,texname,shadername) {
	var mod = buildplanexzmodel(name,wid,hit,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildplanexzmodel2t(name,wid,hit,texname1,texname2,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var planexzmesh = buildplanexzmesh(wid,hit);
	    mod.setmesh(planexzmesh);
	    mod.settexture(texname1);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

function buildplanexz2t(name,wid,hit,texname1,texname2,shadername) {
	var mod = buildplanexzmodel2t(name,wid,hit,texname1,texname2,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

// planexy
function planexy_surf(wid,hit) {
	var startx = -wid;
	var stepx = 2*wid;
	var starty = hit;
	var stepy = -2*hit;
	var functor = function(p,q) {
		var v = [],n = [];
		n[0] = 0;
		n[1] = 0;
		n[2] = -1;
		v[0] = startx + p*stepx;
		v[1] = starty + q*stepy;
		v[2] = 0;
		return {"v":v,"n":n};
	};
	return functor;
}

function buildplanexymesh(wid,hit) {
	return buildpatch(planepatchi,planepatchj,planepatchu,planepatchv,planexy_surf(wid,hit));
}

function buildplanexymodel(name,wid,hit,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var planexymesh = buildplanexymesh(wid,hit);
	    mod.setmesh(planexymesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildplanexy(name,wid,hit,texname,shadername) {
	var mod = buildplanexymodel(name,wid,hit,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildplanexymodel2t(name,wid,hit,texname1,texname2,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var planexymesh = buildplanexymesh(wid,hit);
	    mod.setmesh(planexymesh);
	    mod.settexture(texname1);
	    mod.settexture2(texname2);
	    mod.commit();
	}
    return mod;
}

function buildplanexy2t(name,wid,hit,texname1,texname2,shadername) {
	var mod = buildplanexymodel2t(name,wid,hit,texname1,texname2,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

// torusxz
var toruspatchi = 40;
var toruspatchj = 40;
var toruspatchu = 3;
var toruspatchv = 3;

function buildtorusxzmesh(rad0,rad1) {
	return buildpatch(toruspatchi,toruspatchj,toruspatchu,toruspatchv,torusxz_surf(rad0,rad1));
}

function buildtorusxzmodel(name,rad0,rad1,texname,shadername) {
	var mod = Model.createmodel(name);
	if (mod.refcount == 1) {
	    mod.setshader(shadername);
		var torusxzmesh = buildtorusxzmesh(rad0,rad1);
	    mod.setmesh(torusxzmesh);
	    mod.settexture(texname);
	    mod.commit();
	}
    return mod;
}

function buildtorusxz(name,rad0,rad1,texname,shadername) {
	var mod = buildtorusxzmodel(name,rad0,rad1,texname,shadername);
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}

function buildpaperairplane(name,texname,shadername) {
//	var mod = buildtorusxzmodel(name,1,.5,texname,shadername);
	var mod = Model.createmodel(name);
	mod.setmesh(paperairplanemesh);
	mod.settexture(texname);
	mod.setshader(shadername);
	mod.commit();
	var ret = new Tree2(name);
	ret.setmodel(mod);
	return ret;
}
