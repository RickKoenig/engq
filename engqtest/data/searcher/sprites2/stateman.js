//var startstate = 0; // test sprites
//var startstate = 1; // test object dragging
//var startstate = 2; // test rotsprite
//var startstate = 3; // test physics
//var startstate = 4; // test webgl, model level, webgl level
//var startstate = 5; // Albert's socks simulation
//var startstate = 6; // more webgl Model and Model2, some bwo's and a floor of fortpoint, model level
//var startstate = 7; // more webgl Tree2, the whole fortpoint scene, tree level
//var startstate = 8; // many, tree level
//var startstate = 9; // scratch, test render targets
//var startstate = 10; // multi texture, tree level, font test
//var startstate = 11; // shader test, lighting
//var startstate = 12; // surface patch
//var startstate = 13; // 3d Lissajous curves
//var startstate = 14; // pendu1, swing 1 pendulum around
//var startstate = 15; // pendu2, coupled pendulums
//var startstate = 16; // pendu3, many coupled pendulums
//var startstate = 17; // arrows
//var startstate = 18; // Menger sponge
var startstate = 19; // Lorenz attractor
//var startstate = 20; // shadow mapping

var nstates = 21;

var state = -1;
var newstate = -1;
var dochangestate = false;
var stateinited = false;

function changestate(ns) {
	while (ns >= nstates)
		ns -= nstates;
	while (ns < 0)
		ns += nstates;
	newstate = ns;
	dochangestate = true;
}

function showandgo(s) {
	logger_str += "^^^^^^^^^^^ " + s + " ^^^^^^^^^^^\n";
//	if (s>=0 && s<nstates) {
	var cbf = window[s];
	if (cbf) {
		cbf();
	}
	logger_str += "vvvvvvvvvvvvv " + s + " vvvvvvvvvvvv\n";
//		return true;	
//	} else {
//		return false;
//	} else {
//		return false;
//	}
}

function loadstate() {
//	var instele = document.getElementById('instructions');
//	instele.innerHTML = escapehtml("Hiho hiho\noff to work\nwe go.");
	var s = "text" + state;
	var noesc = "noesc" + state;
	var inst = window[s];
	var noesc = window[noesc];
	if (!inst)
		inst = "Default";
	var instele = document.getElementById('instructions');
	if (noesc)
		instele.innerHTML = inst;
	else
		instele.innerHTML = escapehtml(inst);
	showandgo("load" + state);
//	preloadtime(230);
//	preloadtime(240);
	preloadtime(2500,true);
	preloadtime(5000,true);
	setloaddonefunc(initstate);
//	if (isloading) {
		// load stuff and trigger the initstate using a load complete callback function
//	} else {
//		initstate();
//	}
}

function initstate() {
	maindebugclear();
	maindebugsetbefore(); // but debug after user
	showandgo("init" + state);
	maindebugsetafter(); // but debug after user
	stateinited = true;
}

function procstate() {
	if (dochangestate) {
		exitstate();
		state = newstate;
		loadstate();
		//initstate();
		dochangestate = false;
	}
	sprites_reset();
	gl_reset();
	if (state>=0 && state<nstates) {
//		if (false) {
		if (stateinited) {
//		if (isloaded()) { // assets loaded, proc state
			load_mode(false); // for webgl
			window["proc" + state]();
			drawelements();
		} else { // assets loading, display loading progress screen
			load_mode(true); // for webgl
			loadingproc();
		}
	}
}

function exitstate() {
	var durl = edrawarea.toDataURL();
	if (stateinited) {
		showandgo("exit" + state);
		stateinited = false;
	}
}

function loadingproc() {
	sprite_setsize(viewx,viewy);
	sprite_draw(0,0,"take0007.jpg");
	sprite_setsize(15,15);
	sprite_drawfont(0,0    ,"smallfont.png","LOADING\nframe = " + frame + ", reqcnt = " + reqcnt + ", loadcnt " + loadcnt + 
		"\n nwreqcnt = " + nwreqcnt + ", nwloadcnt = " + nwloadcnt);
//	sprite_drawfont(0,0    ,"fontbiggreen.png","frame = " + frame + ", reqcnt = " + reqcnt + ", loadcnt " + loadcnt);
//	sprite_drawfont(0,0    ,"smallfont.png","Hello World!\nABC\nDEF\nGHI");
}
