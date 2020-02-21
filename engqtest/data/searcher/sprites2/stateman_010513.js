//var state = 0; // test sprites
//var state = 1; // test object dragging
//var state = 2; // test rotsprite
//var state = 3; // test physics
//var state = 4; // test webgl
var state = 5; // Albert's socks simulation

var nstates = 6;
var newstate = -1;

function showandgo(s) {
	logger += "(" + s + ") ";
	window[s]();	
}

function changestate(ns) {
	if (ns >= nstates)
		ns = 0;
	else if (ns < 0)
		ns = nstates - 1;
	showandgo("exit" + state);
	state = ns;
	initstate();
}

function initstate() {
	maindebugclear();
	maindebugsetbefore(); // but debug after user
	showandgo("init" + state);
	maindebugsetafter(); // but debug after user
	procstate();
}

function procstate() {
	sprites_reset();
	gl_reset();
	window["proc" + state]();
	drawelements();
	// showandgo("proc" + state);
}

function exitstate() {
	showandgo("exit" + state);
}
