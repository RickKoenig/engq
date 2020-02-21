var state17 = {};

state17.text = "WebGL: Arrow Demo.";

state17.title = "Arrows";

var arrowarea;
var camz = 5;
var arrowcnt;

var arrowmaster;
var expmaster;
var ang = 0;
var dela = 0;

var driftview = .15;
var maxacc = .1;

var roottree,arrowlist;
var camvel = [0,0,0];

function makearrowmaster() {
	var arrowmaster = new Tree2("arrow");
	// a modelpart
	//var atree = buildsphere("atree",.1,"panel.jpg","diffusespecp");
	atree = buildconexz("tail",.375,.5,"maptestnck.png","diffusespecp");
	atree.trans = [-.5,0,0];
	atree.rot = [0,0,-Math.PI/2];
	arrowmaster.linkchild(atree);
	atree = buildcylinderxz("mid",.1875,.5,"maptestnck.png","diffusespecp");
	atree.trans = [-.25,0,0];
	atree.rot = [0,0,-Math.PI/2];
	arrowmaster.linkchild(atree);
	atree = buildconexz("head",.375,.5,"maptestnck.png","diffusespecp");
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
	var atree = buildsphere("exp",.75,"maptestnck.png","texc");
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
	var childcopy = arrowlist.children.slice(0);
	for (i=0;i<childcopy.length;++i) {
		childcopy[i].glfree();
		childcopy[i].unlinkchild();
	}
}

var maxarrowcnt = 200;
/*
function arrowuserproc(t) {
	--t.cnt;
	if (t.cnt < 0) {
		t.unlinkchild();
		t.glfree();
		var r = Math.random();
		if (r < .5 && arrowcnt < maxarrowcnt) {
			r = t.rot[2];
			t = makeanarrow(t.trans,normalangrad(r + .2));
			arrowlist.linkchild(t);
			t = makeanarrow(t.trans,normalangrad(r - .2));
			arrowlist.linkchild(t);
		} else {
			t = makeanexp(t.trans,t.transvel);
			arrowlist.linkchild(t);
		}
		--arrowcnt;
	}
}
*/
function arrowuserproc(t) {
	--t.cnt;
	if (t.cnt < 0) {
		var r = Math.random();
		if (r < .5) {
			t.cnt = 10;
			return;
		}
		t.unlinkchild();
		t.glfree();
		r = Math.random();
		if (r < .5 && arrowcnt < maxarrowcnt) {
			r = t.rot[2];
			t = makeanarrow(t.trans,normalangrad(r + .2));
			arrowlist.linkchild(t);
			t = makeanarrow(t.trans,normalangrad(r - .2));
			arrowlist.linkchild(t);
		} else {
			t = makeanexp(t.trans,t.transvel);
			arrowlist.linkchild(t);
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
		t.mat.color = [1,0,0,t.cnt/15.0];
		var s = 15 - t.cnt;
		s *= .05;
		s += .5;
		t.scale = [s,s,s];
	}
}

function makeanarrow(pos,a) {
	++arrowcnt;
	var t = arrowmaster.newdup();
	t.trans = vec3.clone(pos);
	t.transvel = [2.5*Math.cos(a),2.5*Math.sin(a),0];
	t.rot = [0,0,a];
	t.cnt = 10;
	t.userproc = arrowuserproc;
	return t;
}

function makeanexp(pos,vel) {
	var t = expmaster.newdup();
	t.trans = vec3.clone(pos);
	t.scale = [0,0,0];
	t.transvel = vel;
	//t.transvel = [Math.cos(a),Math.sin(a),0];
	//t.scalevel = [2,2,2];
	t.cnt = 15;
	t.userproc = expuserproc;
	return t;
}

function centerarrowsview() {
	var cld = arrowlist.children;
	var dpos;
	var moveback = 2;
	if (!cld.length) {
		dpos = [0,0,-moveback]; // flycam
		//mainvp.rot = [0,0,0]; // flycam
	} else if (cld.length == 1) {
		dpos = [cld[0].trans[0],cld[0].trans[1],-moveback]; // flycam
	} else {
		var minpnt = vec3.clone(cld[0].trans);
		var maxpnt = vec3.clone(minpnt);
		var i;
		for (i=1;i<cld.length;++i) {
			vec3.min(minpnt,minpnt,cld[i].trans);
			vec3.max(maxpnt,maxpnt,cld[i].trans);
		}
		dpos = [];
		vec3.add(dpos,minpnt,maxpnt);
		vec3.scale(dpos,dpos,.5);
		var spread = [];
		vec3.sub(spread,maxpnt,minpnt);
		dpos[2] = -.5*Math.max(spread[0]/gl.asp,spread[1]) - moveback;
	}
//	vec3.lerp(mainvp.trans,mainvp.trans,dpos,driftview);
	var newpos = [];
	vec3.lerp(newpos,mainvp.trans,dpos,driftview);
	
	// now add inertia to the camera
	var dvel = [];
	vec3.sub(dvel,newpos,mainvp.trans);
	var acc = [];
	vec3.sub(acc,dvel,camvel);
	var sl = vec3.squaredLength(acc);
	if (sl > maxacc*maxacc) {
		var scl = maxacc/Math.sqrt(sl);
		vec3.scale(acc,acc,scl);
	}
	vec3.add(camvel,camvel,acc);
	//vec3.lerp(dvel,camvel,dvel,driftvel);
	vec3.add(mainvp.trans,mainvp.trans,camvel);
	if (mainvp.trans[2] > -moveback+1)
		mainvp.trans[2] = -moveback+1;
}

state17.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
};

state17.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state17\n");
	
	// build the scene
	arrowmaster = makearrowmaster();
	expmaster = makeexpmaster();
	roottree = new Tree2("root");
	var ppsaveu = planepatchu;
	var ppsavev = planepatchv;
	planepatchu = 20;
	planepatchv = 20;
	var pln = buildplanexy("back",200,200,"panel.jpg","tex");
	planepatchu = ppsaveu;
	planepatchv = ppsavev;
	pln.trans = [0,0,5];
	roottree.linkchild(pln);

	arrowlist = new Tree2("arrowlist");
	roottree.linkchild(arrowlist);
	
	// ui
	setbutsname('arrow');
	// less,more,reset for pendu1
	arrowarea = makeaprintarea('arrow: ');
	makeabut("Reset Arrows",resetarrows);
	resetarrows();
	
	// a modelpart
	ang = 0;
	dela = 0;
	//var atree = makeanarrow([0,0,0],ang);
	//arrowlist.linkchild(atree);
	//atree = makeanexp([0,0,0]);
	//roottree.linkchild(atree);
	
	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-25]; // flycam
	mainvp.rot = [0,0,0]; // flycam
	camvel = [0,0,0];
};

state17.proc = function() {
//	if (!gl)
//		return;
    //gl.clearColor(.25,.25,0,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	if (input.mbut[0] || !arrowcnt) {
		++dela;
		if (dela == 6) {
			ang = normalangrad(ang + .13);
			var t = makeanarrow([0,0,0],ang);
			arrowlist.linkchild(t);
			dela = 0;
		}
	}
		
	roottree.proc();
	doflycam(mainvp);  // modify the trs of the vp
	
	//pendpce0.trans = [0,0,0];
	//dolights(); // get some lights to eye space
	updatearrows();
	if (!flycamstate.inflycam) {
		centerarrowsview();
	}
	beginscene(mainvp);
	roottree.draw();
};

state17.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
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
};
