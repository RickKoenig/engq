var keystatecur;
var keystatehold;

var keycodes = {
	"up":38,
	"down":40,
	"left":37,
	"right":39,
	"pgup":33,
	"pgdown":34,
	"squareleft":219,
	"squareright":221
};

function getkey() {
	var ret = input.keybuff.shift();
	if (!ret)
		return 0;
	return ret;
}

function getkeycode(e) {
	if (e.keyCode)
		return e.keyCode;
	return e.charCode;
}

// event
function bkeyp(e) {
	inputevents += "(KP" + getkeycode(e) + ") ";
	input.keybuff.push(getkeycode(e));
}

// event
function bkeyd(e) {
	inputevents += "(KD" + getkeycode(e) + ") ";
	keystatecur[getkeycode(e)] = 1;
	keystatehold[getkeycode(e)] = 1;
}

// event
function bkeyu(e) {
	inputevents += "(KU" + getkeycode(e) + ") ";
	keystatehold[getkeycode(e)] = 0;
}

function keyinit() {
	keystatecur = new Array(); // keycode keys currently down 0 or 1, indexed by keycode
	keystatehold = new Array(); // keycode keys currently down 0 or 1, indexed by keycode
	var body = document.body;
	body.onkeydown = bkeyd;
	body.onkeypress = bkeyp;
	body.onkeyup = bkeyu;
}

function keyproc()
{
	// buffered keyboard input
	input.key = getkey();
	// keystate, allow for nudges
	input.keystate = keystatecur;
	keystatecur = keystatehold.slice(0);
}
