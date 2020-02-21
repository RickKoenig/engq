var state19 = {};

state19.text = "WebGL: Lorenz Attractor";

state19.title = "Lorenz attractor";

var shadowtexture;

state19.lightdist = 20;
state19.lightloc = [0,state19.lightdist,-state19.lightdist];

var vectormaster;
var vectormasterc;
var vectorlist = [];

var vectorversion = 1;
//var vectorversion = 2;

function makevectormaster() {
	var arrowmaster = new Tree2("arrow");
	var c = [
		Math.random(),
		Math.random(),
		Math.random(),
		1.0
	];
	// a modelpart
	//var atree = buildsphere("atree",.1,"Asphalt.png","diffusespecp");
	atree = buildconexz2t("tail",.125,.5,"maptestnck.png","shadowmap","shadowmapusec");
	atree.trans = [0,0,0];
	//atree.rot = [0,0,-Math.PI/2];
	atree.mat.color = c;
	arrowmaster.linkchild(atree);
	atree = buildcylinderxz2t("mid",.0625,.5,"Asphalt.png","shadowmap","shadowmapusec");
	atree.trans = [0,.25,0];
	//atree.rot = [0,0,-Math.PI/2];
	atree.mat.color = c;
	arrowmaster.linkchild(atree);
	atree = buildconexz2t("head",.125,.5,"maptestnck.png","shadowmap","shadowmapusec");
	atree.trans = [0,.75,0];
	//atree.rot = [0,0,-Math.PI/2];
	atree.mat.color = c;
	arrowmaster.linkchild(atree);
	//atree.trans = [0,0,0];
	//atree.rotvel = [.01,.05,0]; 
	return arrowmaster;
}


function makevectormasterc() {
	switch(vectorversion) {
	case 1:
		var arrowmaster = new Tree2("arrow");
		var c = [
			Math.random(),
			Math.random(),
			Math.random(),
			1.0
		];
		// a modelpart
		//var atree = buildsphere("atree",.1,"Asphalt.png","diffusespecp");
		atree = buildconexz2t("tail",.125,.5,"maptestnck.png","shadowmap","shadowmapusec");
		atree.trans = [0,-.5,0];
		//atree.rot = [0,0,-Math.PI/2];
		atree.mat.color = c;
		arrowmaster.linkchild(atree);
		atree = buildcylinderxz2t("mid",.0625,.5,"Asphalt.png","shadowmap","shadowmapusec");
		atree.trans = [0,-.25,0];
		//atree.rot = [0,0,-Math.PI/2];
		atree.mat.color = c;
		arrowmaster.linkchild(atree);
		atree = buildconexz2t("head",.125,.5,"maptestnck.png","shadowmap","shadowmapusec");
		atree.trans = [0,.25,0];
		//atree.rot = [0,0,-Math.PI/2];
		atree.mat.color = c;
		arrowmaster.linkchild(atree);
		//atree.trans = [0,0,0];
		//atree.rotvel = [.01,.05,0]; 
		break;
	case 2:
		var arrowmaster = buildpaperairplane("paperairplane",null,"cvert");
		arrowmaster.mat.color = [1,0,0,1];
		break;
	}
	return arrowmaster;
}

function changeavector(t,pos,dir,scl) {
	t.trans = vec3.clone(pos);
	//t.rot = [0,0,r];
	if (!dir)
		return t;
	t.qrot = dir2quat(dir);
	//t.rot = dir2rot(dir);
	//t.rot = [0,0,ang];
	var s = scl*vec3.length(dir);	
	t.scale = [s,s,s];
	//t.transvel = [5*Math.cos(a),5*Math.sin(a),0];
	//t.rot = [0,0,a];
	//t.cnt = 5;
	//t.userproc = arrowuserproc;

}

function makeavector(pos,dir,center,c) {
	if (!c)
		c = [1,1,1,1];
	//++arrowcnt;
	var t;
	if (center)
		t = vectormasterc.newdup();
	else
		t = vectormaster.newdup();
	changeavector(t,pos,dir,.1);
	if (t.children.length) {
		var tail = t.children[0];
		tail.children[0].mat.color = c;
		tail.children[1].mat.color = c;
		var mid = t.children[1];
		mid.children[0].mat.color = c;
		mid.children[1].mat.color = c;
		mid.children[2].mat.color = c;
		var head = t.children[2];
		head.children[0].mat.color = c;
		head.children[1].mat.color = c;
	}
	return t;
}

function circlesim(pos) {
	var step = .01;
	var vel = vec3.create();
	vel[0] = -step*pos[1];
	vel[1] = step*pos[0];
	vel[2] = -step*pos[2];
	return vel;
}

function lorenzsim(pos) {
	var sig = 10.0;
	var beta = 8.0/3.0;
	var rho = 28.0;
	var step = .01;
	var vel = vec3.create();
	vel[0] = sig*(pos[1]-pos[0]);
	vel[1] = pos[0]*(rho-(pos[2]+20)) - pos[1];
	vel[2] = pos[0]*pos[1] - beta*(pos[2]+20);
	vec3.scale(vel,vel,step);
	return vel;
}

function procsim1(f) {
	var i,n = vectorlist.length;
	var t;
	for (i=0;i<n;++i) {
		t = vectorlist[i];
		t.transvel = f(t.trans);
		var dir = vec3.create();
		vec3.scale(dir,t.transvel,2);
		changeavector(t,t.trans,dir,3);
	}
}

state19.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("fortpoint/Asphalt.png");
	preloadimg("../common/sptpics/wonMedal.png");
};

state19.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state19\n");
	vectorlist = [];

// build render target
	var shadowmapres = 512;
	shadowtexture = FrameBufferTexture.createtexture("shadowmap",shadowmapres,shadowmapres);

// shadow viewport
	state19.shadowvp = {
		target:shadowtexture,
	   	clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
		clearcolor:[1,1,1,1],                    // Set clear color to yellow, fully opaque
	//	mat4.create();
		"trans":vec3.clone(state19.lightloc),
		"rot":[Math.PI/4,0,0], // part of lightdir
		//"scale":[1,1,1],
	   	near:.1,
	   	far:10000.0,
	   	zoom:1,
		asp:1,
		inlookat:false,
		isshadowmap:true
	};

// main viewport for state 19
	state19.mvp = {
	   	clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
		clearcolor:[.15,.25,.75,1],                    // Set clear color to yellow, fully opaque
		trans:[-21,-3,3],
	//	"trans":vec3.clone(state19.lightloc),
		"rot":[Math.PI/4,0,0], // part of lightdir
		//"scale":[1,1,1],
	   	near:.1,
	   	far:10000.0,
	   	zoom:1,
		asp:gl.asp,
		inlookat:true
	};

// build a master vector
	vectormaster = makevectormaster();
	vectormasterc = makevectormasterc();
	
//// build the main screen scene
	roottree = new Tree2("root");
	
	// back plane
	atree1 = buildplanexy2t("planexy1",20,20,"maptestnck.png","shadowmap","shadowmapuse"); // tex
	atree1.trans = [0,0,20];
	atree1.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree1);
	
	// ground plane
	atree1b = buildplanexz2t("planexz1",20,20,"maptestnck.png","shadowmap","shadowmapuse"); // tex
	atree1b.trans = [0,-20,0];
	atree1b.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree1b);
	
/*	// test object
	var as = buildcylinderxz2t("cyl6",.4,2.5,"Asphalt.png","shadowmap","shadowmapuse");
	as.scale = [2,2,2];
	//as.rotvel = [0,0,.3];
	as.rot = [0,0,Math.PI/2-.1];
	as.trans = [0,-10,10];
	roottree.linkchild(as);
*/	
	// a bunch of vectors
/*	var i,j,k;
	for (k=-1;k<=1;++k) {
		for (j=-1;j<=1;++j) {
			for (i=-1;i<=1;++i) {
				var sp = 3;
				as = makeavector([sp*i,sp*j,sp*k]);
				as.rotvel = [Math.random(),Math.random(),Math.random()];
				as.scale = [4,4,4];
				roottree.linkchild(as);
				vectorlist.push(as);
			}
		}
	}
*/
/*	// some more vectors
	var i,j,k;//k = 0;
	for (i=-1;i<=1;++i) {
		for (j=-1;j<=1;++j) {
			for (k=-1;k<=1;++k) {
				var sp = 3;
				as = makeavector([i,j,k],[i,j,k],true);
				//as.rotvel = [Math.random(),Math.random(),Math.random()];
				//as.scale = [4,4,4];
				roottree.linkchild(as);
				vectorlist.push(as);
			}
		}
	}
*/
	var i;
	for (i=0;i<50;++i) {
		var sz = 10;
		var x = 2*sz*Math.random()-sz;
		var y = 2*sz*Math.random()-sz;
		var z = 2*sz*Math.random()-sz;
		as = makeavector([x,y,z],[1,0,0],true,[Math.random()*.5+.5,Math.random()*.5+.5,Math.random()*.5+.5,1]);
		//as.rotvel = [Math.random(),Math.random(),Math.random()];
		//as.scale = [4,4,4];
		roottree.linkchild(as);
		vectorlist.push(as);
	}
	// the light
	var atree4 = buildsphere("sph4",.2,null,"flat"); // this is where the light is for (point) shadowcasting
	atree4.mod.mat.color = [1,1,.5,1];
	atree4.trans = vec3.clone(state19.lightloc);
	atree4.flags |= treeflagenums.DONTCASTSHADOW;
	atree4.flags |= treeflagenums.DIRLIGHT;
	atree4.rot = [Math.PI/4,0,0];
	addlight(atree4);
	roottree.linkchild(atree4);

//// set the lights (directional)
	//lights.wlightdir = vec3.fromValues(0,-.7071,.7071);  // part of lightdir
	
//// set the camera
	//mainvp.trans = vec3.clone(state19.lightloc);
	//vec3.scale(mainvp.trans,mainvp.trans,.5);
	//mainvp.trans = [-21,-3,3];
	//mainvp.inlookat = 1;
//	mainvp.rot = [Math.PI/4,0,0]; // part of lightdir
	// build the scene
/*	var testmat4 = mat4.create();
	mat4.lookAtlhc(testmat4,[0,0,10],[1,2,22],[0,1,0]);
	var testmat4inv = mat4.create();
	mat4.invert(testmat4inv,testmat4);
	//vp.trans,vp.lookattrans,[0,1,0]); */
	state19.mvp.lookat = vectorlist[0];
	state19.mvp.inlookat = true;
};

state19.proc = function() {
//	if (!gl)
//		return;
	
	// run the simulation here
	var i;
	for (i=0;i<10;++i) {
		//procsim1(circlesim);
		procsim1(lorenzsim);
		roottree.proc();
	}
	
	// lookat the 0th arrow/vector
	doflycam(state19.mvp); // modify the trs of the vp
	
	// draw to shadowmap
	beginscene(state19.shadowvp);
	roottree.draw();

	// draw main scene
	beginscene(state19.mvp);	
	roottree.draw();
};

state19.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	// show state
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	
	// free everything
	roottree.glfree();
	shadowtexture.glfree();
	vectormaster.glfree();
	vectormasterc.glfree();
	
	// show freed state
	logrc();
	roottree = null;
	state19.mvp.lookat = null;
	state19.mvp.inlookat = false;
	logger("exiting webgl state19\n");
};
