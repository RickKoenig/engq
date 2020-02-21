var treeflagenums = {
// tree
	"ALWAYSFACING":0x20,
	"DONTDRAW":0x40,
	"DONTDRAWC":0x80,
	"DONTCASTSHADOW":0x100
};

var buildshadowmap = false;

// longer term scratch storage, no allocations
var treeglobals = {
	// fast scratch vars
	"negtrans":vec3.create(),
	"invscale":vec3.create()
};

var qrotmat = mat4.create();

function buildtransrotscale(out,srt) {
	//mat4.identity(out);
	if (srt.trans)
		mat4.translate(out,out,srt.trans);
	if (srt.rot)
		mat4.rotateEuler(out,out,srt.rot);
    if (srt.scale)
    	mat4.scale(out,out,srt.scale);
}

function buildtransqrotscale(out,srt) {
	//mat4.identity(out);
	if (srt.trans)
		mat4.translate(out,out,srt.trans);
	if (srt.qrot) {
		mat4.fromQuat(qrotmat,srt.qrot);
		mat4.mul(out,out,qrotmat);
	}
    if (srt.scale)
    	mat4.scale(out,out,srt.scale);
}

function buildtransrotscaleinv(out,srt) {
    if (srt.scale) {
    	vec3.inv(treeglobals.invscale,srt.scale);
   		mat4.scale(out,out,treeglobals.invscale);
   	}
   	if (srt.rot) {
   		mat4.rotateEulerinv(out,out,srt.rot);
   	}
   	if (srt.trans) {
   		vec3.negate(treeglobals.negtrans,srt.trans);
		mat4.translate(out,out,treeglobals.negtrans);
	}
}

function Tree2(name) {
	this.name = name;
	this.flags = 0;
// model
	//mod(),
// hierarchy
	//parent(0),
	this.children = [];
// orientation
	//this.trans = vec3.create();
	//this.rot = vec3.create();
	//this.scale = vec3.fromValues(1,1,1);
	//this.transvel = vec3.create();
	//this.rotvel = vec3.create();
	//this.scalevel = vec3.fromValues(1,1,1);
	this.mvm = mat4.create(); // this tree's model view matrix
	// tree specific user uniforms
	this.mat = {}; // tree specific user uniforms
	// populate based on file data
	var s = spliturl(name);
	var lext = s.ext.toLowerCase();
	if (lext == "bwo") {
		this.mod = loadbwomodel(name);
	} else if (lext == "bws") {
		loadbws(this);
	}
}

Tree2.prototype.setmodel = function(m) {
	this.mod = m;
};

// can be called more than once, used by 'Model' only for now.
// override model's texture
Tree2.prototype.settexture = function(tname) {
	if (this.reftexture) {
		this.reftexture.glfree();
		//this.reftexture = null;
	}
	this.texturename = tname;
	this.reftexture = Texture.createtexture(this.texturename);
	//if (this.reftexture && this.reftexture.hasalpha)
	//	this.hasalpha = true;
};

Tree2.prototype.newdup = function() {
	var ret = new Tree2(this.name);
	ret.flags = this.flags;
	ret.userproc = this.userproc;
//	ret.name = this.name;
// model
	ret.mod = this.mod;
	if (ret.mod)
		++ret.mod.refcount;
// tree material override
	ret.mat = cloneObject(this.mat);
// tree texture
	ret.texturename = this.texturename;
	ret.reftexture = this.reftexture;
	if (ret.reftexture)
		++ret.reftexture.refcount;
// orientation
	if (this.trans)
		ret.trans = vec3.clone(this.trans);
	if (this.rot)
		ret.rot = vec3.clone(this.rot);
	if (this.scale)
		ret.scale = vec3.clone(this.scale);
	if (this.transvel)
		ret.transvel = vec3.clone(this.transvel);
	if (this.rotvel)
		ret.rotvel = vec3.clone(this.rotvel);
	if (this.scalevel)
		ret.scalevel = vec3.clone(this.scalevel);
// hiearchy
	ret.children = [];
	var i,n = this.children.length;
	for (i=0;i<n;++i) {
		var dupchild = this.children[i].newdup();
		dupchild.parent = ret;
		ret.children.push(dupchild);
	}
	return ret;
};

Tree2.prototype.linkchild = function(child) {
	if (child.parent)
		alert("child '" + child.name + "' already has a parent, '" + child.parent.name + "'");
	this.children.push(child);
	child.parent = this;
};

Tree2.prototype.unlinkchild = function() {
//	S32 i,n;
	if (!this.parent)
		alert("child " + this.name + "' has no parent to unlink",this.name);
	var idx = this.parent.children.indexOf(this);
	if (idx < 0)
		alert("child " + this.name + "' parent has already disowned you!");
	else
		this.parent.children.splice(idx,1);
	this.parent = null;
};

// look for freaky javascript interpreter bug that causes an array[3] 0 element to go from 0 to NaN, quite rare, hasn't happened in a while
function crn(name,r) {
	var i;
	for (i=0;i<3;++i)
		if (!isFinite(r[i]) || isNaN(r[i])) {
			alert("not a number " + name + " " + i + " invfpswanted " + invfpswanted);
			r[i] = 0;
		}
}

Tree2.prototype.proc = function() {
	if (this.rotvel) {
		if (!this.rot)
			this.rot = vec3.create();
		crn("proc1",this.rot);
		crn("prco1vel",this.rotvel);
		this.rot[0] = normalangrad(this.rot[0] + this.rotvel[0]*invfpswanted);
		this.rot[1] = normalangrad(this.rot[1] + this.rotvel[1]*invfpswanted);
		this.rot[2] = normalangrad(this.rot[2] + this.rotvel[2]*invfpswanted);
		crn("proc2",this.rot);
	}

	if (this.transvel) {
		if (!this.trans)
			this.trans = vec3.create();
		this.trans[0] += this.transvel[0]*invfpswanted;
		this.trans[1] += this.transvel[1]*invfpswanted;
		this.trans[2] += this.transvel[2]*invfpswanted;
	}
	
	if (this.scalevel) {
		if (!this.scale)
			this.scale = vec3.fromValues(1,1,1);
		this.scale[0] *= Math.pow(this.scalevel[0],invfpswanted);
		this.scale[1] *= Math.pow(this.scalevel[1],invfpswanted);
		this.scale[2] *= Math.pow(this.scalevel[2],invfpswanted);
	}

// children last
	var childcopy = this.children.slice(0);
	var i,n=childcopy.length;
	for (i=0;i<n;++i) {
		childcopy[i].proc();
		if (childcopy[i].userproc)
			childcopy[i].userproc(childcopy[i]);
	}
	//crn("proc3",this.rot);
};

// this was done before with 
// void video_drawscene(tree2* t)
Tree2level = 0;
Tree2drawlist = [];

function Tree2sortfunct (a,b) {
	if (a.drawpri == 2 && b.drawpri == 2) {
		return b.mvm[14] - a.mvm[14]; // decreasing trans z, draw far away first, (left handed coord system, inc z goes away from camera towards the horizon)
		//return a.mvm[14] - b.mvm[14]; // test wrong order
	}
	return a.drawpri - b.drawpri; // decreasing z
}
	
Tree2.prototype.draw = function() {
	if (Tree2level == 0) {
		Tree2drawlist = [];
	}
	if (this.flags & treeflagenums.DONTDRAWC) {
		return;
	}
	mat4.push(mvMatrix);
	if (this.o2pmat4)
		mat4.mul(mvMatrix,mvMatrix,this.o2pmat4);
	else if (this.qrot) {
		buildtransqrotscale(mvMatrix,this); // using trs members (trans rot scale)
	} else {
		buildtransrotscale(mvMatrix,this); // using trs members (trans rot scale)
		if (this.mod && (this.mod.flags & modelflagenums.ISSKYBOX)) { // skybox is relative to view
			mvMatrix[12] = 0;
			mvMatrix[13] = 0;
			mvMatrix[14] = 0;
		} else if (this.flags & treeflagenums.ALWAYSFACING) { // rotate to view and keep scale
			var scl = Math.abs(mat4.determinant(mvMatrix));
			var scl = Math.pow(scl,1.0/3.0);
			mvMatrix[0] = scl;
			mvMatrix[1] = 0;
			mvMatrix[2] = 0;
			mvMatrix[4] = 0;
			mvMatrix[5] = scl;
			mvMatrix[6] = 0;
			mvMatrix[8] = 0;
			mvMatrix[9] = 0;
			mvMatrix[10] = scl;
		}
	}
	if (this.mod) {
		if (!(this.flags & treeflagenums.DONTDRAW)) {
			if (!shadowmap.inshadowmapbuild  || !(this.flags & treeflagenums.DONTCASTSHADOW)) {
			//curtree = this;
				mat4.copy(this.mvm,mvMatrix);
				Tree2drawlist.push(this);
				//this.mod.draw();
				//curtree = null;
			}
		}
	}
// children first
	var i,n=this.children.length;
	for (i=0;i<n;++i) {
		++Tree2level;
		this.children[i].draw();
		--Tree2level;
	}
	mat4.pop(mvMatrix);
	if (Tree2level == 0) {
		var ndrawlist = Tree2drawlist.length;
		var i;
		var alphacnt = 0;
		var skyboxcnt = 0;
		var opaquecnt = 0;
		for (i=0;i<ndrawlist;++i) { // set drawpri (0 skybox, 1 opaque, 2 alpha)
			curtree = Tree2drawlist[i];
			var ctm = curtree.mod;
			if (ctm.flags & modelflagenums.ISSKYBOX) {
				++skyboxcnt;
				curtree.drawpri = 0;
			} else if (ctm.flags & modelflagenums.HASALPHA) {
				++alphacnt;
				curtree.drawpri = 2;
			} else {
				++opaquecnt;
				curtree.drawpri = 1;
			}
			//curtree.mod.draw();
		}
		// sort drawlist
		Tree2drawlist.sort(Tree2sortfunct);
		for (i=0;i<ndrawlist;++i) {
			curtree = Tree2drawlist[i];
			mat4.copy(mvMatrix,curtree.mvm);
			curtree.mod.draw();
		}
		curtree = null; // keep lower level (engine bypass) stuff happy
	}
};

tree2level = 0;

function tree2indent(n) {
	var ret = "";
	var i;
	for (i=0;i<n;++i) {
		ret += "   ";
	}
	return ret;
}

var ntreenodes = 0;
Tree2.prototype.log = function() {
	if (tree2level == 0) {
		logger("Tree Log\n");
		ntreenodes = 0;
	}
	++ntreenodes;
	var modname = "---";
	if (this.mod)
		modname = this.mod.name;
	logger("" + tree2indent(tree2level+1) + this.name + " mod " + modname);
	if (this.mod && this.mod.isafont)
		logger(" ModelFont");
	//logger(tree2level);
	if (this.texturename)
		logger(" treetexture " + this.texturename);
	logger("\n");
	var i,n=this.children.length;
	for (i=0;i<n;++i) {
		++tree2level;
		this.children[i].log();
		--tree2level;
	}
	if (tree2level == 0) {
		logger("num modes = " + ntreenodes + "\n");
	}
};
	
Tree2.prototype.glfree = function() {
//	if (this.parent)
//		this.unlinkchild();
	if (this.mod) {
		this.mod.glfree();
		this.mod = null;
	}
	if (this.reftexture) {
		this.reftexture.glfree();
		this.reftexture = null;
	}
	var i,n=this.children.length;
	for (i=0;i<n;++i) {
		this.children[i].glfree();
	}
};
