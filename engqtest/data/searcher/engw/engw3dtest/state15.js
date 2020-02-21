var state15 = {};
state15.text = "WebGL: This state lets you play with coupled pendulums.";

state15.title = "2 pendulums";

// some code and globals 'borrowed' from state14

//var pendroot = null;
var pend2pos = null;
var pend2vel = null;
var pend2spring = null;
var pend2tree0 = null;
var pend2tree1 = null;


var pend2k = -.006;
var pend2g = -.02;
var pendlen = 4;
var lastbutsection = -1;
var delta2 = [];

function updatedamp2() {
	printareadraw(damparea,"Damping2 : " + damparr[dampval].toFixed(5));
}
	
function lessdamp2() {
	dampval -= dampstep;
	if (dampval < 0)
		dampval = 0;
	//updatedamp2();
}

function moredamp2() {
	dampval += dampstep;
	if (dampval >= damparr.length)
		dampval = damparr.length - 1;
	//updatedamp2();
}

function resetdamp2() {
	dampval = 0;
	//updatedamp2();
}

/*function pendfric(v) {
	return (1-damparr[dampval])*v;
}
*/	
function buildpend2() {
	var ret = new Tree2("apend");
	var pendpce0 = buildsphere("pend2pce0",.2,"panel.jpg","diffusespecp");
	pendpce0.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	ret.linkchild(pendpce0);
	
	// rod 
	var pendpce1 = buildcylinderxz("pend2pce1",.1,pendlen,"panel.jpg","diffusespecp");
	pendpce1.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	ret.linkchild(pendpce1);

	// bob 
	//var pendpce2 = buildcylinderxz("pend2pce2",.4,.2,"panel.jpg","diffusespecp");
	var pendpce2 = buildsphere3("pend2pce2",[.4,.1,.4],"panel.jpg","diffusespecp");
	pendpce2.trans = [0,4,0];
	//pendpce2.trans = [0,4,-.1];
	pendpce2.rot = [Math.PI/2,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	ret.linkchild(pendpce2);
	return ret;
}

state15.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
};

state15.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state15\n");
	
	pend2pos = [-Math.PI/8,0];
	pend2vel = [0,0];
	lastbutsection = -1;
	lastbut = false;
	
	
	// build the scene
	roottree = new Tree2("root");
	
	// ui
	setbutsname('pendu2');
	// less,more,reset for pendu1
	damparea = makeaprintarea('damp: ');
	makeabut("less damp2",null,lessdamp2);
	makeabut("more damp2",null,moredamp2);
	makeabut("reset damp2",null,resetdamp2,null,true);
	resetdamp2();
	
	//pendroot = new Tree2("pendroot");
	//roottree.linkchild(pendroot);
	
	pend2tree0 = buildpend2();
	pend2tree0.trans = [-2,0,0];
	pend2tree0.rot = [0,0,0];
	roottree.linkchild(pend2tree0);

	pend2tree1 = buildpend2();
	pend2tree1.trans = [2,0,0];
	pend2tree1.rot = [0,0,0];
	roottree.linkchild(pend2tree1);

	pend2spring = buildcylinderxz("pend2spring",.1,1,"panel.jpg","diffusespecp"); // spring
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(pend2spring);

	// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
	// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-camz]; // flycam
	mainvp.rot = [0,0,0]; // flycam
};

state15.proc = function() {
//	if (!gl)
//		return;
    //gl.clearColor(0,.25,0,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	
	var pendpos = 0;
	if (input.mbut[0]) {
		var b = camz;
		var m = 2*camz/gl.viewportHeight;
		pendpos = m*input.mx - b*gl.asp;
		if (lastbutsection < 0) {
			if (pendpos < 0)
				lastbutsection = 0;
			else 
				lastbutsection = 1;		
		}
	} else {
		lastbutsection = -1;
	}
	
	if (lastbutsection == 0) {
		//pend2tree0.trans = pendpos;
		pend2pos[0] = range(-Math.PI/8,(pendpos - pend2tree0.trans[0])/pendlen,Math.PI/8);
		pend2vel[0] = 0;
	} else if (lastbutsection == 1) {
		pend2pos[1] = range(-Math.PI/8,(pendpos - pend2tree1.trans[0])/pendlen,Math.PI/8);
		pend2vel[1] = 0;
	}	 
///////// start physics /////////
/*	vec3.sub(pendvel,pendpos,oldpendpos);
	vec3.sub(pendaccel,pendvel,oldpendvel);
	pendaccel[1] += pendgrav;
	vec3.copy(oldpendpos,pendpos);
	vec3.copy(oldpendvel,pendvel);
	
	var torque = pendaccel[0]*Math.sin(pendrot) - pendaccel[1]*Math.cos(pendrot);

	var pendrotaccel = torque/pendradius;
	pendrotvel += pendrotaccel;
	pendrotvel = pendfric(pendrotvel);
	pendrot += pendrotvel; */
	
	var pend2accel = [];
	
	var sf = -(pend2pos[1] - pend2pos[0])*pend2k;
	pend2accel[0] = pend2pos[0]*pend2g + sf;
	pend2accel[1] = pend2pos[1]*pend2g - sf;
	
	pend2vel[0] += pend2accel[0];
	pend2vel[1] += pend2accel[1];
	//pend2vel[0] += pend2pos[0]*pend2g;
	//pend2vel[1] += pend2pos[1]*pend2g;
	
	pend2vel[0] = pendfric(pend2vel[0]);
	pend2vel[1] = pendfric(pend2vel[1]);
	
	pend2pos[0] += pend2vel[0];
	pend2pos[1] += pend2vel[1];
	
	pend2tree0.rot[2] = pend2pos[0] + Math.PI;
	pend2tree1.rot[2] = pend2pos[1] + Math.PI;

	var springstart = [pendlen*Math.sin(pend2pos[0])+pend2tree0.trans[0],-pendlen*Math.cos(pend2pos[0]),0];
	var springend = [pendlen*Math.sin(pend2pos[1])+pend2tree1.trans[0],-pendlen*Math.cos(pend2pos[1]),0];
	pend2spring.trans = springstart;
	vec3.sub(delta2,springend,springstart);
	var d = vec3.length(delta2);
	//pend2spring.trans = springend;
	pend2spring.scale = [1,d,1];
	var rt = Math.atan2(delta2[1],delta2[0])-Math.PI/2;
//	pend2spring.rot = [0,0,-Math.PI/2];
	pend2spring.rot = [0,0,rt];

///////// end physics /////////

	roottree.proc();
	doflycam(mainvp);  // modify the trs of the vp
	
	//pendpce0.trans = [0,0,0];
	beginscene(mainvp);

	//dolights(); // get some lights to eye space
	updatedamp2();
	roottree.draw();
};

state15.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state15\n");
	clearbuts('pendu2');
};
