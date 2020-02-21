var input; // object that has all input in it
function Input() {
	this.keybuff = new Array(); // unicode keys pressed
	this.keystate = new Array(); // keycode keys currently down 0 or 1, indexed by keycode
	this.key = 0;
	this.mx = 0;
	this.my = 0;
	this.mbut = [];
	this.mclick = [];
	this.wheelPos = 0;
	this.wheelDelta = 0;
}

function initinput() {
	input = new Input();
	keyinit();
	butinit();
	mapinit();
}

function inputproc() {
	keyproc();
	butproc();
	mapproc();
}
