var state13 = {};
// test webgl
var roottree;

state13.text = "WebGL: Display 3D Lissajous curves, old down mouse button to change phase";

state13.title = "3D Lissajous curves";
var phasestrand3 = [0,0,0];
var freqstrand3 = [2,3,5];
var freqmul = .99;//freqstrand3[0]*freqstrand3[1];
var sizestrand3 = [3,3,3];
var lps3 = [0,0,0];
var phasearea3 = null;

var lspres = 175;
var lsqres = 15;
var lscapp = 5;
var lsuvp = 120;
var lsuvq = 6;
var lsuvcapp = 2;
var lsrad = .25;

function updatephase3() {
	printareadraw(phasearea3,"Phase3 : " + phasestrand3[0].toFixed(3) + " " + phasestrand3[1].toFixed(3) + " " + phasestrand3[2].toFixed(3));
}
	
function resetphase3() {
	phasestrand3 = [.2,0,0];
	updatephase3();
}


var strand3 = null;
/// strand test, sine wave
function lissagues_strand(freq,ff,phaser,size) {
	var functor = function(t) {
		t *= 2*Math.PI;
		var v = [];
		//phaser[0] = 0;
		//phaser[1] = 0;
		v[0] = Math.sin(freq[0]*ff*t + phaser[0])*size[0];
		v[1] = Math.sin(freq[1]*ff*t + phaser[1])*size[1];
		v[2] = Math.sin(freq[2]*ff*t + phaser[2])*size[2];
		return v;
	};
    return functor;
}

state13.load = function() {
//	if (!gl)
//		return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
};

state13.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state13\n");
	// build the scene
	roottree = new Tree2("root");
	
	setbutsname('lissa');
	// less,more,reset for pendu1
	phasearea3 = makeaprintarea('phase');
	resetphase3();
	// add a strand (braid)
	//var strand3 = buildstrand("pend1strand",sinxy_strand(1,0,3*2*Math.PI,3), 75,25,5 ,8,8,2 ,2.5,"maptestnck.png","diffusespecp");
	// var strand3 = buildstrand("pend1strand",simple_strand(),25,25,25,5,5,2,2.5,"panel.jpg","diffusespecp");
	var pic = "panel.jpg";
	//var pic = "maptestnck.png";
	strand3 = buildstrand("lissastrand",lissagues_strand(freqstrand3,freqmul,phasestrand3,sizestrand3),lspres,lsqres,lscapp,lsuvp,lsuvq,lsuvcapp,lsrad,pic,"diffusespecp");
//	strand3 = buildstrand("lissastrand",sinxy_strand(1,0,1,1),75,15,5,5,5,2,.25,"panel.jpg","diffusespecp");
	//strand3.trans = [-12,8,0];
	//strand3.rotvel = [.1,.5,0];
	roottree.linkchild(strand3);

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
	mainvp.trans = [0,0,-5]; // flycam
	mainvp.rot = [0,0,0]; // flycam
};

state13.proc = function() {
//	if (!gl)
//		return;
    //gl.clearColor(0,.25,0,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	vec3.copy(lps3,phasestrand3);
	if (input.mbut[0]) {
		phasestrand3[0] = 2 * Math.PI * input.mx / gl.viewportWidth;
		phasestrand3[1] = 2 * Math.PI * input.my / gl.viewportHeight;
	}
	if (!vec3.equals(phasestrand3,lps3)) {
		updatephase3();
		changestrandmesh(strand3,lissagues_strand(freqstrand3,freqmul,phasestrand3,sizestrand3),lspres,lsqres,lscapp,lsuvp,lsuvq,lsuvcapp,lsrad);
	}
	roottree.proc();
	doflycam(mainvp);  // modify the trs of the vp
	beginscene(mainvp);
	//dolights(); // get some lights to eye space
	roottree.draw();
};

state13.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state13\n");
	clearbuts('lissa');
};
