var state12 = {};

// test webgl
var roottree;

state12.text = "WebGL: This state tests surface patches including strands and braids. Drag the mouse left and right to see an effect.";

state12.title = "Surface patches";

var phasestrand = 0;
var phasearea = null;

function updatephase() {
	printareadraw(phasearea,"Phase : " + phasestrand.toFixed(3));
}
	
function resetphase() {
	phasestrand = 0;
	updatephase();
}


var strand1 = null; // tree

/// strand test, sine wave
function sinxy_strand(freq,phase,wid,hit) {
	var functor = function(t) {
		var v = [];
		v[0] = wid*t;
		t *= 2*Math.PI*freq;
		v[1] = hit*Math.sin(t+phase);
		v[2] = 0;
		return v;
	};
    return functor;
}

function simple_strand() {
	var functor = function(t) {
		var v = [0,0,t*10];
		return v;
	};
    return functor;
}

function braid_strand(freq,size,phase) {
	var functor = function(t) {
    	var v = [0,0,0];
    	v[1] = size[1]*t;
    	t *= 2*Math.PI;
    	v[0] = size[0]*Math.sin(freq[0]*t + phase[0]);
    	v[2] = size[2]*Math.sin(freq[2]*t + phase[2]);
    	return v;
	};
	return functor;
}

function buildbraid() {
	var ret = new Tree2("abraidtot");
	var pres = 80;
	var pcapres = 2;
	var qres = 12;
	var brad2 = .05125;
	var nbraid = 3;
	var shadlist = ["diffusep","diffusespecv","diffusespecp"];
	var phase =[0,0,0];
	var freq = [1,0,2];
	var size = [.5,2,.155];
	var i;
	for (i=0;i<nbraid;++i) {
//		if (i!=0)
//			continue;
		phase[0] = 2*Math.PI*i/nbraid;
		phase[2] = 4*Math.PI*i/nbraid;
		sbrdtree = buildstrand("braid_" + i,braid_strand(freq,size,phase),pres,qres,pcapres,pres/4,qres/4,pcapres,brad2,"panel.jpg",shadlist[i]);
		ret.linkchild(sbrdtree);
	}
	return ret;
}

state12.load = function() {
//	if (!gl)
//		return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
};

state12.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state12\n");
	// build the scene
	roottree = new Tree2("root");
	
	// add a sphere
	var sph = buildsphere("pend1sph",3,"panel.jpg","diffusespecp");
	//sph.trans[1] = 12;
	//sph.trans = [0,12,0];
	roottree.linkchild(sph);

	// add a cylinder
	var cyl = buildcylinderxz("pend1cyl",3,4,"panel.jpg","diffusespecp");
	//var cyl = buildsphere("pend1sph",3,"panel.jpg","diffusespecp");
	cyl.trans = [12,8,0];
	//cyl.trans[1] = -2;
	//cyl.rotvel = [.1,.5,0];
	cyl.scale = [3,3,3];
	cyl.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(cyl);

	// add a plane
	var planexz = buildplanexz("pend1planexz",3,3,"panel.jpg","diffusespecp");
	planexz.trans = [0,-8,0];
	//planexz.rotvel = [.1,.5,0];
	planexz.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(planexz);

	// add a plane
	var planexy = buildplanexy("pend1planexy",3,3,"panel.jpg","diffusespecp");
	planexy.trans = [0,-8,0];
	//planexy.rotvel = [.1,.5,0];
	planexy.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(planexy);

	// add a torus
	var torusxz = buildtorusxz("pend1torusxz",3,1.5,"panel.jpg","diffusespecp");
	torusxz.trans = [12,-8,0];
	torusxz.rotvel = [.1,.5,0];
	roottree.linkchild(torusxz);

	// add a strand (braid)
	//var strand1 = buildstrand("pend1strand",sinxy_strand(1,0,3*2*Math.PI,3), 75,25,5 ,8,8,2 ,2.5,"maptestnck.png","diffusespecp");
	// var strand1 = buildstrand("pend1strand",simple_strand(),25,25,25,5,5,2,2.5,"panel.jpg","diffusespecp");
	strand1 = buildstrand("pend1strand",sinxy_strand(1,0,1.5*2*Math.PI,1.5),75,15,5,5,5,2,1.25,"panel.jpg","diffusespecp");
	strand1.trans = [-12,8,0];
	strand1.rotvel = [.1,.5,0];
	roottree.linkchild(strand1);

	var braid1 = buildbraid();
	braid1.trans = [-12,-8,0];
	//braid1.rotvel = [.1,.5,0];
	braid1.scale = [4,4,4];
	roottree.linkchild(braid1);

	
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	
	// test rotaxis
	var rv = vec3.fromValues(0,0,1);
	var ax = vec3.fromValues(.7071,.7071,0);
	var rvo = vec3.create();
	var i;
	var q = quat.create();
	for (i=0;i<=8;++i) {
		var a = 2*Math.PI*i/8;
		quat.setAxisAngle(q,ax,a);
		vec3.transformQuat(rvo,rv,q);
		logger(rvo[0].toFixed(3) + " " + rvo[1].toFixed(3) + " " + rvo[2].toFixed(3) + "\n");
	}
	// done test rotaxis
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [-12,8,-15]; // flycam
	mainvp.rot = [0,0,0]; // flycam

	setbutsname('braid');
	// less,more,reset for pendu1
	phasearea = makeaprintarea('phase');
	resetphase();
};

state12.proc = function() {
//	if (!gl)
//		return;
	
    //gl.clearColor(0,.25,0,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	var lps = phasestrand;
	if (input.mbut[0])
		phasestrand = 2 * Math.PI * input.mx / gl.viewportWidth;
/*	phasestrand += .04;
	if (phasestrand >= 2*Math.PI)
		phasestrand -= 2*Math.PI;
*/	
	if (phasestrand != lps) {
		updatephase();
		changestrandmesh(strand1,sinxy_strand(1,phasestrand,1.5*2*Math.PI,1.5),75,15,5,5,5,2,1.25);
	}
	roottree.proc();
	doflycam(mainvp);  // modify the trs of the vp
	beginscene(mainvp);
	//dolights(); // get some lights to eye space
	roottree.draw();
};

state12.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state12\n");
	clearbuts('braid');
};
