var text18 = "WebGL: Menger sponge, upto level 4.\n" +
			"Toggle the 'flycam' using the 'C' key.\n" +
			"Reset the position of the camera with the 'R' key.\n" +
			"Aim the camera with the mouse.\n" + 
			"Move the camera with the arrow keys and left and right mouse buttons.\n" +
			"Speed up and slow down camera movement with '+/=' and '-' keys.\n" +
			"Tab through various states using the 'prev state' and 'next state' buttons.";

var curlevel = null;
var curleveltree = null; // note: don't use curtree, maybe look into namespace issues

function lesslevel() {
	if (curlevel > 0)
		--curlevel;
	updatelevel();
}

function morelevel() {
	if (curlevel < 4)
		++curlevel;
	updatelevel();
}

var pow3 = [1,3,9,27,81,243,729];
var trin;

// return an value of binary has ones, base3 to base2 like
function tobase3(n,ndig) {
	var ret = 0;
	var i;
	var p = 1;
	for (i=0;i<ndig;++i) {
		var m = n%3;
		n = Math.floor(n/3);
		if (m == 1)
			ret += p;
		p *= 2;
	}
	return ret;
}

function getones(lev) {
	trin = [];
	var m = pow3[lev];
	var i;
	for (i=0;i<m;++i) {
		var r = tobase3(i,lev);
		trin.push(r);
	}
}

function issolid(pos) {
	var br = [];
	var i;
	for (i=0;i<3;++i) {
		var t = pos[i];
		var b;
		if (t<0 || t>=trin.length)
			return false;
		b = trin[t];
		br.push(b);
	}
	if (br[0] & br[1])
		return false;
	if (br[0] & br[2])
		return false;
	if (br[1] & br[2])
		return false;
	return true;
}
	
var smeshfaceposx = {
	verts: [
		 0,1,1,
		 0,1,0,
		 0,0,1,
		 0,0,0
	],
	uvs: [
		 0,0,
		 1,0,
		 0,1,
		 1,1
	],
	faces: [
		0,1,2,
		3,2,1
	]
};

var smeshfacenegx = {
	verts: [
		 0,1,0,
		 0,1,1,
		 0,0,0,
		 0,0,1
	],
	uvs: [
		 0,0,
		 1,0,
		 0,1,
		 1,1
	],
	faces: [
		0,1,2,
		3,2,1
	]
};

var smeshfaceposy = {
	verts: [
		 0,0,0,
		 1,0,0,
		 0,0,1,
		 1,0,1
	],
	uvs: [
		 0,0,
		 1,0,
		 0,1,
		 1,1
	],
	faces: [
		0,1,2,
		3,2,1
	]
};

var smeshfacenegy = {
	verts: [
		 0,0,1,
		 1,0,1,
		 0,0,0,
		 1,0,0
	],
	uvs: [
		 0,0,
		 1,0,
		 0,1,
		 1,1
	],
	faces: [
		0,1,2,
		3,2,1
	]
};

var smeshfaceposz = {
	verts: [
		 0,1,0,
		 1,1,0,
		 0,0,0,
		 1,0,0
	],
	uvs: [
		 0,0,
		 1,0,
		 0,1,
		 1,1
	],
	faces: [
		0,1,2,
		3,2,1
	]
};

var smeshfacenegz = {
	verts: [
		 1,1,0,
		 0,1,0,
		 1,0,0,
		 0,0,0
	],
	uvs: [
		 0,0,
		 1,0,
		 0,1,
		 1,1
	],
	faces: [
		0,1,2,
		3,2,1
	]
};

var meshes6 = [
	smeshfaceposz,
	smeshfacenegz,
	smeshfaceposx,
	smeshfacenegx,
	smeshfaceposy,
	smeshfacenegy
];
var off60 = [
	[0,0,0],
	[0,0,1],
	[0,0,0],
	[1,0,0],
	[0,0,0],
	[0,1,0]
];
var off62 = [
	[0,0,-1],
	[0,0,1],
	[-1,0,0],
	[1,0,0],
	[0,-1,0],
	[0,1,0]
];
var colss6 = [
	[1,.125,.125,1],
	[.125,1,.125,1],
	[.125,.125,1,1],
	[1,1,.125,1],
	[1,.125,1,1],
	[.125,1,1,1]
];

var smesh;
var curmeshidx;

function clearsmesh() {
	smesh = {verts:[],faces:[],uvs:[]};
	curmeshidx = 0;
}

function addsmesh(off,msh) {
	var i,j;
	// 4 verts
	for (i=0;i<4;++i) {
		for (j=0;j<3;++j) {
			smesh.verts.push(msh.verts[3*i+j]+off[j]);
		}
	}
	// 4 uvs
	for (i=0;i<4;++i) {
		for (j=0;j<2;++j) {
			smesh.uvs.push(msh.uvs[2*i+j]);
		}
	}
	// 2 faces
	for (i=0;i<2;++i) {
		for (j=0;j<3;++j) {
			smesh.faces.push(msh.faces[3*i+j]+curmeshidx);
		}
	}
	curmeshidx += 4;
}

function makesponge(level,f) {
	var i,j,k,f;
	var m = pow3[level];
	getones(level);
	clearsmesh();
	for (k=0;k<=m;++k) {
		for (j=0;j<=m;++j) {
			for (i=0;i<=m;++i) {
//				for (f=0;f<6;++f) {
					var off0 = [i+off60[f][0], j+off60[f][1], k+off60[f][2]];
					var off1 = [i,j,k];
					var off2 = [i+off62[f][0], j+off62[f][1], k+off62[f][2]];
					
					// pz
					//off1 = [i,j,k];
					//off2 = [i,j,k-1];
					if (issolid(off1) && !issolid(off2))
						addsmesh(off0,meshes6[f]);
	/*				
					// nz
					offp1 = [i,j,k+1];
					if (issolid(off1) && !issolid(off2))
						addsmesh(off11,smeshfacenegz);
						
					// px
					offm1 = [i-1,j,k];
					if (issolid(off1) && !issolid(off2))
						addsmesh(off1,smeshfaceposx);
						
					// nx
					offp1 = [i+1,j,k];
					if (issolid(off1) && !issolid(off2))
						addsmesh(off1,smeshfacenegx);
						
					// py
					offm1 = [i,j-1,k];
					if (issolid(off1) && !issolid(off2))
						addsmesh(off1,smeshfaceposy);
						
					// ny
					offp1 = [i,j+1,k];
					if (issolid(off1) && !issolid(off2))
						addsmesh(off1,smeshfacenegy); */
//				}
			}
		}
	}
	return smesh;
}

function updatelevel() {
	printareadraw(levelarea,"Level : " + curlevel);
	var i,f;
	var childcopy = roottree.children.slice(0);
	for (i=0;i<childcopy.length;++i) {
		childcopy[i].glfree();
		childcopy[i].unlinkchild();
	}
	i = curlevel;
	//for (i=cur;i<=3;++i) {
		for (g=0;g<6;++g) {
			// a modelpart
			var amod = Model2.createmodel("spongemod m" + i + "s" + g);
			if (amod.refcount == 1) {
				//amod.setmesh(smeshtemplate);
				var msh = makesponge(i,g);//,[0,-i*1.5,0]);
				amod.setmesh(msh);
				//amod.settexture("maptestnck.png");
				//amod.setshader("tex");
				var fs = 6000;
				for (f=0;f<amod.faces.length;f += fs) {
					var f2 = f + fs;
					if (f2 >= amod.faces.length)
						f2 = amod.faces.length;
					var fp = f2 - f;
					//amod.addmat("tex",null,fp,2*fp);
					//amod.addmat("texc","BridgeCon1.png",fp,2*fp);
					amod.addmat("texc","maptestnck.png",fp,2*fp);
				}
				amod.mat.color = colss6[g];
				//amod.mat.color = [.75,.75,.75,1];
				amod.commit();
				//amod.settexture();
				var atree = new Tree2("spongepart" + i);
				atree.setmodel(amod);
				//atree.trans = [0,(4-i)*1.5,0];
				atree.trans = [-.45,-.45,0];
				var scl = 1.0/pow3[i];
				atree.scale = [scl,scl,scl];
				//pendpce0.rotvel = [.1,.5,0];
				//pendpce0.flags |= treeflagenums.ALWAYSFACING;
				roottree.linkchild(atree);
			}
		}
	//}
}

function load18() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
	preloadimg("../fortpoint/BridgeCon1.png");
}

function init18() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state18\n");
	
	// build the scene
	roottree = new Tree2("root");
	
	// ui
	setbutsname('menger');
	// less,more,reset for pendu1
	levelarea = makeaprintarea('level: ');
	makeabut("lower level",null,lesslevel);
	makeabut("higher level",null,morelevel);
	curleveltree = 0;
	curlevel = 1;
	updatelevel();
	
	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-1]; // flycam
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
	clearbuts('menger');
}
