//var state = 0; // test sprites
//var state = 1; // test object dragging
//var state = 2; // test rotsprite
//var state = 3; // test physics
//var state = 4; // test webgl
var state = 5; // abert's socks

var nstates = 6;

function nextstate() {
	changestate(state+1);
}

function prevstate() {
	changestate(state-1);
}

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
	maindebugclear();
	maindebugsetbefore(); // but debug after user
	showandgo("init" + state);
	sprites_reset();
	gl_reset();
	window["proc" + state](); // do 1 proc right away
	maindebugsetafter(); // but debug after user
	drawelements();
}

function initstate() {
	maindebugclear();
	maindebugsetbefore(); // but debug after user
	showandgo("init" + state);
	sprites_reset();
	gl_reset();
	window["proc" + state](); // do 1 proc right away
	maindebugsetafter(); // but debug after user
	drawelements();
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
