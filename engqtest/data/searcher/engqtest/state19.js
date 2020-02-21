var text19 = "WebGL: Lorenz Attractor";

var shadowtexture;

var state19 = {};

var lightdist = 20;
state19.lightloc = [0,lightdist,-lightdist];

state19.shadowvp = {
//	mat4.create();
	"trans":vec3.clone(state19.lightloc),
	"rot":[.78,0,0], // part of lightdir
	"scale":[1,1,1],
};

var vectormaster;
var vectormasterc;
var vectormasterdir = [0,1,0]; // model vector points up
var vectorcross = vec3.create();
var vectornorm = vec3.create();
var vectorlist = [];
var frm;

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
	return arrowmaster;
}

function changeavector(t,pos,dir,scl) {
	t.trans = vec3.clone(pos);
	//t.rot = [0,0,r];
	if (!dir)
		return t;
	t.qrot = quat.create();
	
	vec3.normalize(vectornorm,dir);
	vec3.cross(vectorcross,vectormasterdir,vectornorm);
	vec3.normalize(vectorcross,vectorcross);
	var d = vec3.dot(vectornorm,vectormasterdir);
	var ang = Math.acos(d);
	quat.setAxisAngle(t.qrot,vectorcross,ang);
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

function load19() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("../fortpoint/Asphalt.png");
	preloadimg("pics/wonMedal.png");
}

function init19() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state19\n");
	frm = 0;
	vectorlist = [];
	
//// build render target
	shadowmap.create(1024,1024); // prepare for shadowmapping
	
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
	roottree.linkchild(atree4);

//// set the lights (directional)
	lights.wlightdir = vec3.fromValues(0,-.7071,.7071);  // part of lightdir
	
//// set the camera
	//mainvp.trans = vec3.clone(state19.lightloc);
	//vec3.scale(mainvp.trans,mainvp.trans,.5);
	mainvp.trans = [-21,-3,3];
	treeinfo.inlookat = 1;
//	mainvp.rot = [.78,0,0]; // part of lightdir
	// build the scene
	var testmat4 = mat4.create();
	mat4.lookAtlhc(testmat4,[0,0,10],[1,2,22],[0,1,0]);
	var testmat4inv = mat4.create();
	mat4.invert(testmat4inv,testmat4);
	//vp.trans,vp.lookattrans,[0,1,0]);
}

function proc19() {
	if (!gl)
		return;
	
	
	
	// run the simulation here
	var i;
	for (i=0;i<10;++i) {
		//procsim1(circlesim);
		procsim1(lorenzsim);
		roottree.proc();
	}
	// draw to shadowmap
	setview(state19.shadowvp);
	shadowmap.beginpass();
    gl.clearColor(1,1,1,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	roottree.draw();

	// draw to main target
	mainvp.lookattrans = vectorlist[0].trans;
	doflycam(mainvp); // modify the mvMatrix
//	setview(mainvp); // modify the mvMatrix
	shadowmap.endpass(); // get out of lightmap rendering, setup lightmap matrices, call after setting mvMatrix
	gl.clearColor(.25,.25,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	dolights(); // get some lights from world space to eye space using this view
	roottree.draw();
	frm += .1;
	if (frm >= 2*Math.PI)
		frm -= 2*Math.PI;
}

function exit19() {
	gl_mode(false);
	if (!gl)
		return;
	// restore default perspective
	mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, 0.002,10000.0);
	mainvp.lookattrans = null;	
	// show state
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	
	// free everything
	roottree.glfree();
	shadowmap.free();
	vectormaster.glfree();
	vectormasterc.glfree();
	
	// show freed state
	logrc();
	roottree = null;
	logger("exiting webgl state19\n");
}
