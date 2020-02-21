var state16 = {};

state16.text = "WebGL: This state lets you play with many coupled pendulums.";

state16.title = "Many pendulums";

// some code and globals 'borrowed' from state14

//var pendroot = null;
var pend3pos = null;
var pend3vel = null;
var pend3spring = null;
var pend3tree = null;


var pend3k = -.6;
var pend3g = -.02;
var pend3len = 1;
var lastbutsection = -1;
var delta2 = [];

var npendu = 10;
var penduspace = .9;

function updatedamp3() {
	printareadraw(damparea,"Damping3 : " + damparr[dampval].toFixed(5));
}
	
function lessdamp3() {
	dampval -= dampstep;
	if (dampval < 0)
		dampval = 0;
	//updatedamp2();
}

function moredamp3() {
	dampval += dampstep;
	if (dampval >= damparr.length)
		dampval = damparr.length - 1;
	//updatedamp2();
}

function resetdamp3() {
	dampval = 7;
	//updatedamp2();
}

function buildpend3() {
	var ret = new Tree2("apend");
	var pendpce0 = buildsphere("pend3pce0",.1,"panel.jpg","diffusespecp");
	pendpce0.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	ret.linkchild(pendpce0);
	
	// rod 
	var pendpce1 = buildcylinderxz("pend3pce1",.05,pend3len,"panel.jpg","diffusespecp");
	pendpce1.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	ret.linkchild(pendpce1);

	// bob 
	//var pendpce2 = buildcylinderxz("pend2pce2",.4,.2,"panel.jpg","diffusespecp");
	var pendpce2 = buildsphere3("pend3pce2",[.2,.05,.2],"panel.jpg","diffusespecp");
	pendpce2.trans = [0,pend3len,0];
	//pendpce2.trans = [0,4,-.1];
	pendpce2.rot = [Math.PI/2,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	ret.linkchild(pendpce2);
	return ret;
}

state16.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
};

state16.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state16\n");
	
	var i;
	pend3pos = [];
	pend3vel = [];
	pend3tree = [];
	for (i=0;i<npendu;++i) {
		pend3pos[i] = 0;
		pend3vel[i] = 0;
	}
	pend3pos[0] = -Math.PI/16;
	lastbutsection = -1;
	lastbut = false;
	
	
	// build the scene
	roottree = new Tree2("root");
	
	// ui
	setbutsname('pendu3');
	// less,more,reset for pendu1
	damparea = makeaprintarea('damp: ');
	makeabut("less damp3",null,lessdamp3);
	makeabut("more damp3",null,moredamp3);
	makeabut("reset damp3",null,resetdamp3,null,true);
	resetdamp3();
	
	//pendroot = new Tree2("pendroot");
	//roottree.linkchild(pendroot);
	
	for (i=0;i<npendu;++i) {
		pend3tree[i] = buildpend3();
		pend3tree[i].trans = [(-npendu+1)*penduspace/2.0+penduspace*i,0,0];
		pend3tree[i].rot = [0,0,0];
		roottree.linkchild(pend3tree[i]);
	}

	pend3spring = [];
	for (i=0;i<npendu-1;++i) {
		pend3spring[i] = buildcylinderxz("pend3spring",.05,1,"panel.jpg","diffusespecp"); // spring
		//pend3spring[i].trans = [-2+2*i,0,0];
		roottree.linkchild(pend3spring[i]);
	}

	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-camz]; // flycam
	mainvp.rot = [0,0,0]; // flycam
};

state16.proc = function() {
//	if (!gl)
//		return;
    //gl.clearColor(0,.25,0,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	
	var pendpos = 0;
	if (input.mbut[0]) {
		var b = camz;
		var m = 2*camz/gl.viewportHeight;
		pendpos = m*input.mx - b*gl.viewportWidth/gl.viewportHeight;
		//lastbutsection = -1;
		if (lastbutsection < 0) {
			lastbutsection = Math.floor(pendpos/penduspace+npendu/2.0);
			//if (pendpos < 0)
			//	lastbutsection = 0;
			//else 
			//	lastbutsection = 1;	
			lastbutsection = range(0,lastbutsection,npendu-1);	
		}
	} else {
		lastbutsection = -1;
	}
	
	if (lastbutsection >= 0) {
		//pend2tree0.trans = pendpos;
		pend3pos[lastbutsection] = range(-Math.PI/8,(pendpos - pend3tree[lastbutsection].trans[0])/pend3len,Math.PI/8);
		pend3vel[lastbutsection] = 0;
	}	 
///////// start physics /////////
	
	
	for (i=0;i<npendu;++i) {
		var pend3accel = pend3pos[i]*pend3g;
		//var sf = -(pend2pos[1] - pend2pos[0])*pend2k;
		if (i > 0) {
			pend3accel += (pend3pos[i] - pend3pos[i-1])*pend2k;
		} 
		if (i < npendu - 1) {
			pend3accel += (pend3pos[i] - pend3pos[i+1])*pend2k;
		}
		//pend3accel[0] = pend3pos[0]*pend3g + sf;
		//pend3accel[1] = pend3pos[1]*pend3g - sf;
		
		pend3vel[i] += pend3accel;
/*		//pend3vel[1] += pend3accel[1];
		//pend2vel[0] += pend2pos[0]*pend3g;
		//pend2vel[1] += pend2pos[1]*pend3g;
		*/
		pend3vel[i] = pendfric(pend3vel[i]);
		
		pend3pos[i] += pend3vel[i]; 
		
		pend3tree[i].rot[2] = pend3pos[i] + Math.PI;
	
	}
	for (i=0;i<npendu-1;++i) {
		var springstart = [pend3len*Math.sin(pend3pos[i])+pend3tree[i].trans[0],-pend3len*Math.cos(pend3pos[i]),0];
		var springend = [pend3len*Math.sin(pend3pos[i+1])+pend3tree[i+1].trans[0],-pend3len*Math.cos(pend3pos[i+1]),0];
		pend3spring[i].trans = springstart;
		vec3.sub(delta2,springend,springstart);
		var d = vec3.length(delta2);
		//pend2spring.trans = springend;
		pend3spring[i].scale = [1,d,1];
		var rt = Math.atan2(delta2[1],delta2[0])-Math.PI/2;
	//	pend2spring.rot = [0,0,-Math.PI/2];
		pend3spring[i].rot = [0,0,rt];
	}

///////// end physics /////////


	roottree.proc();
	doflycam(mainvp);  // modify the trs of the vp
	
	//pendpce0.trans = [0,0,0];
	beginscene(mainvp);
	//dolights(); // get some lights to eye space
	updatedamp3();
	roottree.draw();
};

state16.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state16\n");
	clearbuts('pendu3');
};
