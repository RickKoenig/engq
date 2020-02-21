var text18 = "WebGL: Menger sponge.";

//var ot = 1.0/3.0;
//var tt = 2.0/3.0;

// 2d version
/*var smeshtemplate = {
	verts: [
		 0, 0,0,
		 1, 0,0,
		 0, 1,0,
		 1, 1,0,
		ot,ot,0,
		tt,ot,0,
		ot,tt,0,
		tt,tt,0
	],
	faces: [
		0,6,4,
		0,2,6,
		2,7,6,
		2,3,7,
		3,5,7,
		3,1,5,
		1,4,5,
		1,0,4
	]
};
*/

var smeshtemplate = {
	verts: [
		 0,0,0,
		 1,0,0,
		 0,1,0,
		 1,1,0
	],
	faces: [
		0,2,1,
		3,1,2
	]
};

// return an array of 0,1,2 elements base3 representation of n
/*function tobase3(n,ndig) {
	var ret = [];
	var i;
	for (i=0;i<ndig;++i) {
		var m = n%3;
		n = Math.floor(n/3);
		ret.push(m);
	}
	return ret;
}
*/
/*function testtobase3() {
	var result = [];
	for (i=0;i<=40;++i) {
		var rs = tobase3(i,4);
		result.push(rs);
	}
	return result;
}
*/

var smesh;
var cursmesh;

function clearsmesh() {
	smesh = {verts:[],faces:[]};
	cursmesh = 0;
}

var pow3 = [1,3,9,27,81,243,729];

function addsmesh(off) {
	var i,j;
	for (i=0;i<8;++i) {
		for (j=0;j<3;++j) {
			smesh.verts.push(smeshtemplate.verts[3*i+j]+off[j]);
		}
	}
	for (i=0;i<8;++i) {
		for (j=0;j<3;++j) {
			smesh.faces.push(smeshtemplate.faces[3*i+j]+cursmesh);
		}
	}
	cursmesh += 8;
}

// level 1 is the above mesh
function makespongesiderec(level,off) {
	if (level == 0) {
		addsmesh(off);
	} else {
		--level;
		var pm = pow3[level];
		var i,j;
		for (j=0;j<3;++j) {
			for (i=0;i<3;++i) {
				//if (i!=1 || j!=1) {
					makespongesiderec(level,[off[0]+i*pm,off[1]+j*pm,off[2]]);
				//}
			}
		}
//		makespongesiderec(level,off);
//		makespongesiderec(level,[off[0]+   pow3[level-1],off[1],off[2]]);
//		makespongesiderec(level,[off[0]+ 2*pow3[level-1],off[1],off[2]]);
	}
}

function makespongeside(level,off) {
	clearsmesh();
	if (!off)
		off = [0,0,0];
	makespongesiderec(level,off);
	return smesh;
}

function maketreecubesponge(level,off) {
	if (!off)
		off = [0,0,0];
	if (level == 0) {
		var t = buildprism2("scube",[1,1,1],"maptestnck.png","tex");
		t.trans = [off[0],off[1],off[2]];
		return t;
	} else {
		var r = new Tree2("node");
		--level;
		var pm = pow3[level];
		var i,j,k;
		for (k=0;k<3;++k) {
			for (j=0;j<3;++j) {
				for (i=0;i<3;++i) {
					var c = 0;
					if (i==1)
						++c;
					if (j==1)
						++c;
					if (k==1)
						++c;
					//if (c >= 2) {
					if (c <= 1) {
					//if (i!=1 || j!=1) {
					//if (j==0) {
						var t = maketreecubesponge(level,[off[0]+i*pm,off[1]+j*pm,off[2]+k*pm]);
						r.linkchild(t);
					}
				}
			}
		}
		return r;
	}
}

/* 
//	var res = testtobase3();
//	logger("result of base3test = " + JSON.stringify(res) + "\n");
	addsmesh([1,0,0]);
	addsmesh([2,0,0]);
	addsmesh([0,1,0]);
	addsmesh([2,1,0]);
	addsmesh([0,2,0]);
	addsmesh([1,2,0]);
	addsmesh([2,2,0]);
	return smesh;
*/

function load18() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
}

function init18() {
	if (!gl)
		return;
	gl_mode(true);
	logger("entering webgl state18\n");
	
	// build the scene
	roottree = new Tree2("root");
	
	var i;
	for (i=0;i<3;++i) {
		// a modelpart
/*		var amod = Model.createmodel("spongemod" + i);
		if (amod.refcount == 1) {
			//amod.setmesh(smeshtemplate);
			var msh = makespongeside(i);//,[0,-i*1.5,0]);
			amod.setmesh(msh);
			amod.setshader("flat");
			amod.mat.color = [.75,.75,.75,1];
			amod.commit();
		}


		//amod.settexture();
		var atree = new Tree2("spongepart" + i);
		atree.setmodel(amod);
		atree.trans = [0,3-i*1.5,0];
		var scl = 1.0/pow3[i];
		atree.scale = [scl,scl,scl];
		//pendpce0.rotvel = [.1,.5,0];
		//pendpce0.flags |= treeflagenums.ALWAYSFACING;
		roottree.linkchild(atree);
*/
		var atree = maketreecubesponge(i);
		atree.trans = [0,3-i*2.5,0];
		var scl = 1.0/pow3[i];
		atree.scale = [scl,scl,scl];
		//pendpce0.rotvel = [.1,.5,0];
		//pendpce0.flags |= treeflagenums.ALWAYSFACING;
		roottree.linkchild(atree);
	}
	
	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-5]; // flycam
	mainvp.rot = [0,0,0]; // flycam
}

function proc18() {
	if (!gl)
		return;
    gl.clearColor(.25,.25,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	doflycam(mainvp); // modify the mvMatrix
	
	//pendpce0.trans = [0,0,0];
	roottree.proc();
	dolights(); // get some lights to eye space
	roottree.draw();
}

function exit18() {
	gl_mode(false);
	if (!gl)
		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state18\n");
}
