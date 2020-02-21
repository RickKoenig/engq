var mclick = [0,0,0];
var maparea = null;
var rawwheeldelta = 0;

function getxcode(e) {
	return e.clientX - e.currentTarget.offsetLeft;
}

function getycode(e) {
	return e.clientY - e.currentTarget.offsetTop;
}

// event
function bmoused(e) {
	mbutcur[e.button] = 1;
	mbuthold[e.button] = 1;
	inputevents += "(Mdown[" + e.button + "] " + getxcode(e) + " " + getycode(e) + ") ";
	e.preventDefault();
}

// event
function bmouseu(e) {
	mbuthold[e.button] = 0;
	inputevents += "(Mup[" + e.button + "] " + getxcode(e) + " " + getycode(e) + ") ";
	++mclick[e.button];
	e.preventDefault();
}

// event
function bmouseov(e) {
	//inputevents += "(Mover " + getxcode(e) + " " + getycode(e) + ") ";
}

// event
function bmouseou(e) {
	mbuthold[e.button] = 0;
	inputevents += "(Mout " + getxcode(e) + " " + getycode(e) + ") ";
}

// event
function bmousem(e) {
	input.mx = getxcode(e);
	input.my = getycode(e);
}

// event, doesn't seem to work if you click on an image on the map, and you click on it, implement with bmoused and bmouseu
function bmousec(e) {
	inputevents += "(Mclick[" + e.button + "] " + getxcode(e) + " " + getycode(e) + ") ";
	// ++mclick;
}

// event
function bmousewheel(e) {
	//rawwheeldelta = 0;
	if (e.wheelDelta) {
		rawwheeldelta += e.wheelDelta/120;
	} else if (e.detail) { /** Mozilla case. */
                /** In Mozilla, sign of delta is different than in IE.
                 * Also, delta is multiple of 3.
                 */
 		rawwheeldelta += -e.detail/3;
	}
	if (e.preventDefault)
        e.preventDefault();
}

function mapinit() {
	maparea = document.getElementById('drawarea');
	if (!maparea)
		maparea = document.getElementById('mycanvas2');
	maparea.onclick = bmousec;
	maparea.onmousedown = bmoused;
	maparea.onmouseup = bmouseu;
	maparea.onmousemove = bmousem;
	maparea.onmouseover = bmouseov;
	maparea.onmouseout = bmouseou;
	maparea.onmousewheel = bmousewheel;
	maparea.addEventListener('DOMMouseScroll', bmousewheel, false);
	//window.addEventListener('DOMMouseScroll', bmousewheel, false);
	//document.onmousewheel = wheel;
	//document.onmousewheel = wheel;
}

function mapproc()
{
	// mouse clicks in current frame
	
	input.mclick[0] = mclick[0];
	input.mclick[1] = mclick[1];
	input.mclick[2] = mclick[2];
	mclick[0] = mclick[1] = mclick[2] = 0;
	// mouse button: 1 down, 0 up
	input.mbut[0] = mbutcur[0]; // allow for nudges
	input.mbut[1] = mbutcur[1]; // allow for nudges
	input.mbut[2] = mbutcur[2]; // allow for nudges
	mbutcur[0] = mbuthold[0];
	mbutcur[1] = mbuthold[1];
	mbutcur[2] = mbuthold[2];
	input.wheelDelta = rawwheeldelta;
	if (rawwheeldelta)
		input.wheelPos += rawwheeldelta;
	rawwheeldelta = 0;
}
