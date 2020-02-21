// javascript for page0.htm

// call something
window.onload = init;

// globals
// engine sprites
var NSPRITES = 1000;
var sprites = new Array();
//var curnsprites = 0;

var aspt,aspt2;

var count = 0;

var headertext,out1,out2,out3;
var drawarea;

var intervaltime = 33; // 10 fps for now

var click=0,down=0,up=0,over=0,out=0,move=0;
var click1=0,down1=0,up1=0,over1=0,out1=0,move1=0;
var lastbut;
var repdelay = 0;
var drag = false;
var oldclientx = 0;
var oldclienty = 0;

var scrollx = 0;
var scrolly = 0;
var scrollvelx = 0;
var scrollvely = 0;
var viewx = 600;
var viewy = 500;
var tilex = 160;
var tiley = 120;
var ticker = null;

var dirs = {
	"UL":{"x":-1,"y":-1},
	"U":{"x":0,"y":-1},
	"UR":{"x":1,"y":-1},
	"L":{"x":-1,"y":0},
	"C":{"x":0,"y":0},
	"R":{"x":1,"y":0},
	"DL":{"x":-1,"y":1},
	"D":{"x":0,"y":1},
	"DR":{"x":1,"y":1}
};

var themap = [
	[0,0,0,0,0,0,0,0],
	[0,1,1,1,1,0,0,0],
	[0,1,0,0,0,0,0,0],
	[0,1,1,0,0,0,0,0],
	[0,1,0,0,0,0,0,0],
	[0,1,0,0,0,0,0,0],
	[0,0,0,0,0,0,1,0],
	[0,0,0,0,0,0,0,0],
];
var mapx = 8;
var mapy = 8;

// utils
// integer log10 of v rounded down
function log10i(v) {
	var ret = -1;
	while(v>0) {
		v /= 10;
		v = Math.floor(v);
		++ret;
	}
	return ret;
}

// make a string of this string this many times
function repstr(s,t) {
	var ret = "";
	var i;
	for (i=0;i<t;++i) {
		ret += s;
	}
	return ret;
}

// put zeros in front to make number have len
function leftpad(number,len) {
	var log = log10i(number);
	if (log<0)
		log = 0;
	var rep = len - log - 1;
	if (rep<0)
		rep = 0;
	var str = repstr("0",rep);
	str += number;
	return str;
}

// build engine sprites
function Sprite(fname,wid,hit) {
	var i;
	this.x = 0
	this.y = 0;
	this.width = wid;
	this.height = hit;
	this.picname = fname;
	this.doclip = false;
	this.clipleft = 0;
	this.cliptop = 0;
	this.clipright = 0;
	this.clipbottom = 0;
	this.image = document.createElement("img");
//	this.inlist = false;
	this.idx = -1;
}

function sprite_new(fname,wid,hit) {
	var spt = new Sprite(fname,wid,hit);
	drawarea.appendChild(spt.image);
	spt.idx = sprites.length;
	sprites.push(spt);
//	++curnsprites;
	return spt;
}

function sprite_delete(spt)
{
	drawarea.removeChild(spt.image);
	sprites[spt.idx] = sprites[sprites.length-1];
	sprites[spt.idx].idx = spt.idx;
	sprites.pop();
//	--curnsprites;
}

// put engine sprites into the DOM
function sprites_update() {
//	removeChildrenFromNode(drawarea);
	var i;
	for (i=0;i<sprites.length;++i) {
//	for (var i in sprites) { // doesn't work maybe because elements are undefined ??
		var spt = sprites[i];
		var img = spt.image; //document.createElement("img");
		img.id = "pic" + i;
		img.src = spt.picname;
		img.width = spt.width;
		img.height = spt.height;
		img.alt = spt.picname;
		img.className = "absol";
//		img.setAttribute("onmousedown","tilehandledown(event);");
//		img.setAttribute("onmousemove","tilehandlemove(event);");
//		img.setAttribute("style",);
		
/*		var st = img.style;
		st.left = "" + spt.x + "px";
		st.top = "" + spt.y + "px";
		if (spt.doclip) {
			var stylestr = "clip:rect(" + spt.cliptop + "px," + spt.clipright +
			  "px," + spt.clipbottom + "px," + spt.clipleft + "px);";	
			img.setAttribute("style",stylestr); 
		}  */
			
		var stylestr = "left:" + spt.x + "px;top:" + spt.y + "px;";
		if (spt.doclip) {
			stylestr += "clip:rect(" + spt.cliptop + "px," + spt.clipright +
			  "px," + spt.clipbottom + "px," + spt.clipleft + "px);";	
		} 
		img.setAttribute("style",stylestr);

//		img.setAttribute("style","left:" + spt.x + "px;top:" + spt.y + "px;" +
//			"clip:rect(10px,70px,50px,10px);");

//		drawarea.appendChild(img);
	}
}

function doscroller() {
	aspt.x = -scrollx;
	aspt.y = -scrolly;
	if (aspt2) {
		aspt2.x = -scrollx + 200;
		aspt2.y = -scrolly;
	}
}

function divint(a,b) {
	var q = a/b;
	return Math.floor(q);
}

function divmod(a,b) {
	var r = a%b;
	if (r<0) {
		r+=b;	
	}
	return r;
}

function buthandleclick(but) {
	if (but.value == "U") {
		count = 0;
		++click;	
	} else if (but.value == "D") {
		count = 100;
		++click1;
	}
	drawheader();
}

function buthandledown(but) {
	var dir = dirs[but.value];
	if (!dir)
		return;
	if (dir.x == 0 && dir.y==0) {
		scrollx = scrolly = 0;
	}
	scrollvelx = dir.x;
	scrollvely = dir.y;	
/*	if (but.value == "U") {
		count = 0;
		++down;	
	} else if (but.value == "D") {
		count = 100;
		++down1;	
	} */
	lastbut = but;
	repdelay = 0;
	proc(); // for now
	intervalgo();
	drawheader();
}

function buthandleup(but) {
	scrollvelx = 0;
	scrollvely = 0;	
/*	if (but.value == "U") {
		count = 0;
		++up;	
	} else if (but.value == "D") {
		count = 100;
		++up1;	
	} */
	lastbut = null;
	intervalstop();
	drawheader();
}

function buthandleover(but) {
	if (but.value == "U") {
		count = 0;
		++over;	
	} else if (but.value == "D") {
		count = 100;
		++over1;	
	}
	if (but == lastbut) {
		buthandledown(but);
	}
	drawheader();
}

function buthandleout(but) {
	scrollvelx = 0;
	scrollvely = 0;	
/*	if (but.value == "U") {
		count = 0;
		++out;	
	} else if (but.value == "D") {
		count = 100;
		++out1;	
	} */
	intervalstop();
	drawheader();
}

function buthandlemove(but) {
	if (but.value == "U") {
		count = 0;
		++move;	
	} else if (but.value == "D") {
		count = 100;
		++move1;	
	}
	drawheader();
}

function maphandledown(e) {
	if (e == null)
		e = window.event; 
// IE uses srcElement, others use target
	var target = e.target != null ? e.target : e.srcElement;
	//	scrollx = scrolly = 0;
	if (out1)
		out1.firstChild.nodeValue = "x = " + e.clientX + ", y = " + e.clientY;
// cancel out any text selections
	document.body.focus(); // prevent text selection in IE
	document.onselectstart = function () { return false; };
	// prevent IE from trying to drag an image
	target.ondragstart = function() { return false; };
	// prevent text selection (except IE)
	drag = true;
	oldclientx = e.clientX + scrollx;
	oldclienty = e.clientY + scrolly;
	return false;
}

/*function tilehandledown(e) {
if (e == null)
	e = window.event; 
//	scrollx = scrolly = 10;
}*/

function maphandlemove(e) {
	if (!drag)
		return false;
	if (e == null)
		e = window.event; 
// IE uses srcElement, others use target
	var target = e.target != null ? e.target : e.srcElement;
	out2.firstChild.nodeValue = "x = " + e.clientX + ", y = " + e.clientY;
// cancel out any text selections
	document.body.focus(); // prevent text selection in IE
	document.onselectstart = function () { return false; };
	// prevent IE from trying to drag an image
	target.ondragstart = function() { return false; }; 
	// prevent text selection (except IE)
	scrollx = oldclientx - e.clientX;
	scrolly = oldclienty - e.clientY;
	proc();
	return false;
	
//	scrollx = scrolly = 0;
}

function maphandleup(e) {
	if (e == null)
		e = window.event; 
// IE uses srcElement, others use target
	var target = e.target != null ? e.target : e.srcElement;
// cancel out any text selections
	document.body.focus(); // prevent text selection in IE
	document.onselectstart = function () { return false; };
	// prevent IE from trying to drag an image
	target.ondragstart = function() { return false; }; 
	// prevent text selection (except IE)
//	out3.firstChild.nodeValue = "x = " + e.clientX + ", y = " + e.clientY;
	drag = false;
	return false;
	
//	scrollx = scrolly = 0;
}

/*function tilehandlemove(e) {
//if (e == null)
//	e = window.event; 
//	scrollx = scrolly = 10;
}*/

function goAjax() {
	
}

function drawheader() {
/*	headertext.firstChild.nodeValue = 
		"C " + click + "," + click1 +
		" D " + down + "," + down1 + 
		" U " + up + "," + up1 +
		" Ov " + over + "," + over1 +
		" Out " + out + "," + out1 +
		" Move " + move + "," + move1; */
	headertext.firstChild.nodeValue = 
		" sclvel = " + scrollvelx + "," + scrollvely + " scl = " + scrollx + "," + scrolly;
//	out1.firstChild.nodeValue = "test1";
//	out2.firstChild.nodeValue = "test2";
}

function intervalgo()
{
	if (ticker == null)
		ticker = window.setInterval(proc,intervaltime);
}

function intervalstop()
{
	if (ticker != null) {
		window.clearInterval(ticker);
		ticker = null;
	}
}

var image2;
function image2loaded()
{
	out3.firstChild.nodeValue = "image 2 loaded";
	aspt2 = sprite_new("take0002.jpg",200,150);
	proc();
}

// init/build drawarea after a page load
function init() {
	headertext = document.getElementById('headertext');
	drawarea = document.getElementById('drawarea');
	out1 = document.getElementById('out1');
	out2 = document.getElementById('out2');
	out3 = document.getElementById('out3');
	aspt = sprite_new("take0001.jpg",200,150);
	aspt.doclip = true;
	aspt.clipleft = 20;
	aspt.cliptop = 20;
	aspt.clipright = 120;
	aspt.clipbottom = 120;
	proc(); // do 1 proc right away
	// test image
	image2 = new Image();
	image2.onload = image2loaded;
	image2.src = "take0002.jpg";
	out3.firstChild.nodeValue = "image 2 loading";
}

function proc()
{
// proc
	++count;
	if (repdelay>5 || repdelay==0) {
		scrollx += 10*scrollvelx;
		scrolly += 10*scrollvely;
	}
	++repdelay;
// draw
	// update header text
//	headertext.firstChild.nodeValue = "Map viewer " + count;
	// update map
	drawheader();
	doscroller(); 
	sprites_update();
//	if (curnsprites==0) {
//		doscroller();
//		sprites_draw();
//	}
}

