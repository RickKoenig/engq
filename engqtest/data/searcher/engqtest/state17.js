var text17 = "WebGL: Arrow Demo.";

var arrowarea;
var camz = 5;
var arrowcnt;

var arrowmaster;
var expmaster;
var ang = 0;
var dela = 0;

function makearrowmaster() {
	var arrowmaster = new Tree2("arrow");
	// a modelpart
	//var atree = buildsphere("atree",.1,"panel.jpg","diffusespecp");
	atree = buildconexz("tail",.125,.5,"maptestnck.png","diffusespecp");
	atree.trans = [-.5,0,0];
	atree.rot = [0,0,-Math.PI/2];
	arrowmaster.linkchild(atree);
	atree = buildcylinderxz("mid",.0625,.5,"panel.jpg","diffusespecp");
	atree.trans = [-.25,0,0];
	atree.rot = [0,0,-Math.PI/2];
	arrowmaster.linkchild(atree);
	atree = buildconexz("head",.125,.5,"maptestnck.png","diffusespecp");
	atree.trans = [.25,0,0];
	atree.rot = [0,0,-Math.PI/2];
	arrowmaster.linkchild(atree);
	//atree.trans = [0,0,0];
	//atree.rotvel = [.01,.05,0];
	return arrowmaster;
}

function makeexpmaster() {
	//expmaster = new Tree2("explosion");
	// a modelpart
	var atree = buildsphere("exp",.25,"maptestnck.png","texc");
	atree.mat.color = [1,0,0,1];
	atree.mod.flags |= modelflagenums.HASALPHA;
	var expmaster = atree;
	return expmaster;
}

	
function updatearrows() {
	printareadraw(arrowarea,"Arrow count : " + arrowcnt);
}
	
function resetarrows() {
	arrowcnt = 0;
	var i;
	var childcopy = roottree.children.slice(0);
	for (i=0;i<childcopy.length;++i) {
		childcopy[i].glfree();
		childcopy[i].unlinkchild();
	}
}

function arrowuserproc(t) {
	--t.cnt;
	if (t.cnt < 0) {
		t.unlinkchild();
		t.glfree();
		var r = Math.random();
		if (r < .5 && arrowcnt < 1000) {
			r = t.rot[2];
			t = makeanarrow(t.trans,normang(r + .2));
			roottree.linkchild(t);
			t = makeanarrow(t.trans,normang(r - .2));
			roottree.linkchild(t);
		} else {
			t = makeanexp(t.trans);
			roottree.linkchild(t);
		}
		--arrowcnt;
	}
}

function expuserproc(t) {
	--t.cnt;
	if (t.cnt < 0) {
		t.unlinkchild();
		t.glfree();
		//t.cnt = 100;
		//t.transvel[0] = -t.transvel[0];
	} else {
		t.mat.color = [1,0,0,t.cnt/10.0];
	}
}

function makeanarrow(pos,a) {
	++arrowcnt;
	var t = arrowmaster.newdup();
	t.trans = vec3.clone(pos);
	t.transvel = [5*Math.cos(a),5*Math.sin(a),0];
	t.rot = [0,0,a];
	t.cnt = 5;
	t.userproc = arrowuserproc;
	return t;
}

function makeanexp(pos) {
	var t = expmaster.newdup();
	t.trans = vec3.clone(pos);
	//t.transvel = [Math.cos(a),Math.sin(a),0];
	t.scalevel = [2,2,2];
	t.cnt = 10;
	t.userproc = expuserproc;
	return t;
}

function load17() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
}

function init17() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state17\n");
	
	// build the scene
	arrowmaster = makearrowmaster();
	expmaster = makeexpmaster();
	roottree = new Tree2("root");
	
	// ui
	setbutsname('arrow');
	// less,more,reset for pendu1
	arrowarea = makeaprintarea('arrow: ');
	makeabut("Reset Arrows",resetarrows);
	resetarrows();
	
	// a modelpart
	ang = 0;
	dela = 0;
	var atree = makeanarrow([0,0,0],ang);
	roottree.linkchild(atree);
	//atree = makeanexp([0,0,0]);
	//roottree.linkchild(atree);
	
	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-25]; // flycam
	mainvp.rot = [0,0,0]; // flycam
}

function proc17() {
	if (!gl)
		return;
    gl.clearColor(.25,.25,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	if (input.mbut[0]) {
		++dela;
		if (dela == 6) {
			ang = normang(ang + .13);
			var t = makeanarrow([0,0,0],ang);
			roottree.linkchild(t);
			dela = 0;
		}
	}
		
	doflycam(mainvp); // modify the mvMatrix
	
	//pendpce0.trans = [0,0,0];
	roottree.proc();
	dolights(); // get some lights to eye space
	updatearrows();
	roottree.draw();
}

function exit17() {
	gl_mode(false);
	if (!gl)
		return;
	arrowmaster.glfree();
	arrowmaster = null;
	expmaster.glfree();
	expmaster = null;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state17\n");
	clearbuts('arrow');
}
