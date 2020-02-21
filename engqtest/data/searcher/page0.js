// javascript for page0.htm

// call something
window.onload = init;

// globals
// engine sprites
var NSPRITES = 1000;
var sprites = new Array(NSPRITES);
var curnsprites = 0;
var count = 0;
var headertext,out1,out2,out3;
var intervaltime = 125; // 8 fps for now
var badimageframe = 300; // about 40 seconds

var click=0,down=0,up=0,over=0,out=0,move=0;
var click1=0,down1=0,up1=0,over1=0,out1=0,move1=0;
var scrollx = 0;
var scrolly = 0;
var zoom = 0;
var scrollvelx = 0;
var scrollvely = 0;
var zoomvel = 0;
var lastbut;
var repdelay = 0;

var drag = false;
var oldclientx = 0;
var oldclienty = 0;

var viewx = 600;
var viewy = 500;
var starttilex = 256;
var starttiley = 256;
var tilex = starttilex;
var tiley = starttiley;
var tilexrat = 1;
var tileyrat = 1;

var zoomi = 1;
var maxidx = 1<<zoomi;
var zoomstep = 8;
var maxzoomi = 21;
var frame = 1;

var dirs = {
	"UL":{"x":-1,"y":-1,"z":0},
	"U":{"x":0,"y":-1,"z":0},
	"UR":{"x":1,"y":-1,"z":0},
	"L":{"x":-1,"y":0,"z":0},
	"C":{"x":0,"y":0,"z":0},
	"R":{"x":1,"y":0,"z":0},
	"DL":{"x":-1,"y":1,"z":0},
	"D":{"x":0,"y":1,"z":0},
	"DR":{"x":1,"y":1,"z":0},
	"ZOOM IN":{"x":0,"y":0,"z":1},
	"ZOOM OUT":{"x":0,"y":0,"z":-1}
};

/*var themap = [
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
var mapy = 8; */

var sclmclx;
var sclmcly;

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

function removeChildrenFromNode(node) {
	if(node == undefined || node == null) {
		return;
	}
	while (node.hasChildNodes()) {
		node.removeChild(node.firstChild);
	}
}
	
var imagecacher;

function isincache(name)
{
	return imagecacher[name];
}

function addcache(name,obj)
{
	imagecacher[name] = obj;
}

function removecache(name)
{
	delete imagecacher[name];
//	imagecacher[name] = undefined;
}

// build engine sprites
function sprites_init() {
	imagecacher = new Object();
	var i;
//	for (var i in sprites) { // doesn't work maybe because elements are undefined ??
	for (i=0;i<NSPRITES;++i) {
		var spt = new Object();
		spt.x = 0
		spt.y = 0;
		spt.ix = 0;
		spt.iy = 0;
		spt.iz = 0;
		spt.width = 0;
		spt.height = 0;
//		spt.picname = "";
		spt.doclip = false;
		spt.clipleft = 0;
		spt.cliptop = 0;
		spt.clipright = 0;
		spt.clipbottom = 0;
//		spt.image = document.createElement("img");
//		spt.inlist = false;
		sprites[i] = spt;
	}
}

// put engine sprites into the DOM
function sprites_draw() {
	var drawarea = document.getElementById('drawarea');
	removeChildrenFromNode(drawarea);
	++frame;
	var i;
	for (i=0;i<curnsprites;++i) {
		var spt = sprites[i]; // sprite to draw
		while(true) {
			var picname = "_mandl_" + spt.iz + "_" + spt.ix + "_" + spt.iy + ".png";
			var img = isincache(picname);
			if (!img) { // get new image from server
				img = document.createElement("img");
				img.src = picname;
				img.alt = picname;
				img.className = "absol";
				img.frame = frame - 1;
				addcache(picname,img);
			}
//			if (spt.iz < 1) { // test: force lowes res image
			if ((img.complete && img.naturalWidth>=40) || spt.iz < 1) {
				break; // valid image
			} else { // try low res
				// check for stale hires image that never got loaded
				if (img.frame + badimageframe <= frame)
					removecache(picname); // will cause a reload of image
				// go lower res
				--spt.iz;
				if (spt.ix % 2 == 0) {
					spt.ix /= 2;				
				} else {
					spt.ix = (spt.ix-1)/2;
					spt.x -= spt.width;
				}
				if (spt.iy % 2 == 0) {
					spt.iy /= 2;				
				} else {
					spt.iy = (spt.iy-1)/2;
					spt.y -= spt.height;
				}
				spt.width *= 2;
				spt.height *= 2;
			}
		}
		if (img.frm == frame)
			continue; // already drawing this lower res image
		img.frm = frame;
		if (!img.complete || img.naturalWidth<40)
			continue;	// not good, nothing to draw
			
		// got a valid image, let's draw it	
		img.width = spt.width;
		img.height = spt.height;
		var stylestr = "left:" + spt.x + "px;top:" + spt.y + "px;" + "z-index:" + spt.iz + ";";
		var dc = spt.doclip;
		var cl = spt.clipleft;
		var cr = spt.clipright;
		var ct = spt.cliptop;
		var cb = spt.clipbottom;
		if (!dc) {
			cl = ct = 0;
			cr = spt.width;
			cb = spt.height;
		}
		if (spt.x<0) {
			cl = Math.max(-spt.x,cl); //Math.max();
			dc = true;
		}
		if (spt.x+spt.width > viewx) {
			cr = Math.min(viewx - spt.x,cr);
			dc = true;
		}
		if (spt.y<0) {
			ct = Math.max(-spt.y,ct);
			dc = true;
		}
		if (spt.y+spt.height > viewy) {
			cb = Math.min(viewy - spt.y,cb);
			dc = true;
		}
		if (dc) {
			stylestr += "clip:rect(" + ct + "px," + cr +
			  "px," + cb + "px," + cl + "px);";	
		}
		img.setAttribute("style",stylestr);
		drawarea.appendChild(img);
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

function initscl() {
	scrollx = -(viewx/2- maxidx*starttilex/2);
	scrolly = -(viewy/2- maxidx*starttiley/2);
	
}

// init/build drawarea after a page load
function init() {
	headertext = document.getElementById('headertext');
	var drawarea = document.getElementById('drawarea');
	out1 = document.getElementById('out1');
	out2 = document.getElementById('out2');
	out3 = document.getElementById('out3');
//	var para = document.createElement("p");
//	var brk = document.createElement("br");
	sprites_init();
	initscl();
	proc(); // do 1 proc right away
	window.setInterval(proc,intervaltime);
/*	document.write("test<br />");
	var j,q,r,d=64;
	for (j=-128;j<=128;j+=1.0) {
		q = j/d;
		q = Math.floor(q);
		r = j%d;
		if (r<0) {
//			--q;
			r+=d;
		}
		document.write("" + j + " / " + d + " = " + q + " R " + r + "<br />");
	} */
	doscroller();
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
	if (dir==null || dir== undefined)
		return;
	if (but.value == "C") {
		zoom = 0;
		ezoom = 1;
		zoomi = 1;
		maxidx = 1<<zoomi;	
		initscl();
	}
	scrollvelx = dir.x;
	scrollvely = dir.y;	
	zoomvel = dir.z;
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
	drawheader();
}

function buthandleup(but) {
	scrollvelx = 0;
	scrollvely = 0;	
	zoomvel = 0;
/*	if (but.value == "U") {
		count = 0;
		++up;	
	} else if (but.value == "D") {
		count = 100;
		++up1;	
	} */
	lastbut = null;
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
	zoomvel = 0;
/*	if (but.value == "U") {
		count = 0;
		++out;	
	} else if (but.value == "D") {
		count = 100;
		++out1;	
	} */
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
//	out1.firstChild.nodeValue = "x=" + e.clientX + ",y=" + e.clientY;
// cancel out any text selections
/*	document.body.focus(); // prevent text selection in IE
	document.onselectstart = function () { return false; };
	// prevent IE from trying to drag an image
	target.ondragstart = function() { return false; }; */
	// prevent text selection (except IE)
	drag = true;
	oldclientx = e.clientX/tilexrat + scrollx;
	oldclienty = e.clientY/tileyrat + scrolly;
	scrollx -= (viewx/2-(e.clientX-e.currentTarget.offsetLeft))/tilexrat;
	scrolly -= (viewy/2-(e.clientY-e.currentTarget.offsetTop))/tileyrat;
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
//	out2.firstChild.nodeValue = "x=" + e.clientX + ",y=" + e.clientY;
// cancel out any text selections
/*	document.body.focus(); // prevent text selection in IE
	document.onselectstart = function () { return false; };
	// prevent IE from trying to drag an image
	target.ondragstart = function() { return false; }; */
	// prevent text selection (except IE)
//	scrollx = oldclientx - e.clientX/tilexrat;
//	scrolly = oldclienty - e.clientY/tileyrat;
	return false;
	
//	scrollx = scrolly = 0;
}

function maphandleup(e) {
	if (e == null)
		e = window.event; 
// IE uses srcElement, others use target
	var target = e.target != null ? e.target : e.srcElement;
// cancel out any text selections
/*	document.body.focus(); // prevent text selection in IE
	document.onselectstart = function () { return false; };
	// prevent IE from trying to drag an image
	target.ondragstart = function() { return false; }; */
	// prevent text selection (except IE)
//	out3.firstChild.nodeValue = "x =" + e.clientX + ",y=" + e.clientY;
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

function doscroller() {
/*	curnsprites = 1;
	var spt = sprites[0];
	spt.x = scrollx;
	spt.y = scrolly;
	spt.width = 80;
	spt.height = 60;
	spt.picname = "take0004.jpg"; */
//	var ezoom = Math.pow(2.0,zoom*.5);
// jump to zoomed in map
	if (zoom>0 && zoomi<maxzoomi) {
		zoom -= zoomstep;
		zoomi += 1;
//		ezoom = Math.pow(2.0,zoom/zoomstep);
		scrollx += viewx/2;	
		scrollx *= 2;	
		scrollx -= viewx/2;
		scrolly += viewy/2;
		scrolly *= 2;	
		scrolly -= viewy/2;
// jump out of zoomed in map
	} else if (zoom < -zoomstep+1 && zoomi>0) {
		zoom += zoomstep;
		zoomi -= 1;
		scrollx += viewx/2;	
		scrollx /= 2;	
		scrollx -= viewx/2;	
		scrolly += viewy/2;	
		scrolly /= 2;	
		scrolly -= viewy/2;	
	}
	var ezoom = Math.pow(2.0,zoom/zoomstep);		
	maxidx = 1<<zoomi;	
	
	tilex = starttilex*ezoom;
	tiley = starttiley*ezoom;
	tilex = Math.floor(tilex);
	tiley = Math.floor(tiley);
	tilexrat = tilex/starttilex;
	tileyrat = tiley/starttiley;
	
	var scrollxt = (scrollx+viewx/2) *tilex/starttilex - viewx/2;
	var scrollyt = (scrolly+viewy/2) *tiley/starttiley - viewy/2;
	scrollxt = Math.floor(scrollxt);
	scrollyt = Math.floor(scrollyt);

	out1.firstChild.nodeValue = "x = " + scrollx + ", y = " + scrolly;
	out2.firstChild.nodeValue = "zoom = " + zoom + ", zoomi = " + zoomi;
	out3.firstChild.nodeValue = "ezoom = " + ezoom + ", sclx = " + scrollx + ", scly = " + scrolly;
	
	var mapi = divint(scrollxt,tilex); // start tile index
	var mapj = divint(scrollyt,tiley);
	var startx = -divmod(scrollxt,tilex); // start pos of first tile
	var starty = -divmod(scrollyt,tiley);
	var i,j; // pos
	var ii,jj; // index
	curnsprites = 0;
	for (j=starty,jj=mapj;j<viewy;j+=tiley,++jj) {
		for (i=startx,ii=mapi;i<viewx;i+=tilex,++ii) {
			if (curnsprites>=NSPRITES)
				break;
/*			if (ii<0 || jj<0 || ii>=mapx || jj>=mapy)
				continue; */
			if (ii<0 || jj<0)
				continue;
			if (ii>=maxidx || jj>=maxidx)
				continue;
			var spt = sprites[curnsprites];
			spt.x = i;
			spt.y = j;
			spt.width = tilex;
			spt.height = tiley;
//			spt.picname = "xpar.png";
//			spt.picname = "take0005.jpg";
/*			if (themap[jj][ii] == 1) {
				spt.picname = "take0005.jpg";
			} else {
				spt.picname = "xpar.png";
			} */
//			var strx = leftpad(ii,4);
//			var stry = leftpad(jj,4);
//			var strx = "" + ii;
//			var stry = "" + jj;
//			spt.picname = "_mandl_" + zoomi + "_" + ii + "_" + jj + ".png";
			spt.iz = zoomi;
			spt.ix = ii;
			spt.iy = jj;
			
/*			spt.doclip = true;
			spt.clipleft = spt.width/32;
			spt.cliptop = spt.height/32;
			spt.clipright = spt.width*31/32;
			spt.clipbottom = spt.height*31/32; */
			
/*			if (i<0) {
				spt.clipleft = -i;
				spt.doclip = true;
			}
			if (i+tilex > viewx) {
				spt.clipright = viewx - i;
				spt.doclip = true;
			}
			if (j<0) {
				spt.cliptop = -j;
				spt.doclip = true;
			}
			if (j+tiley > viewy) {
				spt.clipbottom = viewy - j;
				spt.doclip = true;
			} */
			++curnsprites;	
		}
	}
}

function proc()
{
// proc
/*	curnsprites = 20;
	for (i=0;i<curnsprites;++i) {
		var spt = sprites[i];
		spt.x = 20*i;
		spt.y = 100 + Math.sin(count*.01 + i*.1)*100;
		spt.width = 80;
		spt.height = 60;
		spt.picname = "take" + leftpad(i+1,4) + ".jpg";
} */
	++count;
	if (repdelay>5 || repdelay==0) {
		scrollx += 10*scrollvelx/tilexrat;
		scrolly += 10*scrollvely/tileyrat;
		zoom += zoomvel;
		if (zoom > 11)
			zoom = 11;
		else if (zoom<-zoomstep)
			zoom = -zoomstep;
	}
	++repdelay;
// draw
	// update header text
//	headertext.firstChild.nodeValue = "Map viewer " + count;
	// update map
	drawheader();
//	if (curnsprites==0) {
		doscroller();
		sprites_draw();
//	}
}

function drawheader() {
/*	headertext.firstChild.nodeValue = 
		"C " + click + "," + click1 +
		" D " + down + "," + down1 + 
		" U " + up + "," + up1 +
		" Ov " + over + "," + over1 +
		" Out " + out + "," + out1 +
		" Move " + move + "," + move1; */
//	headertext.firstChild.nodeValue = 
//		" sclvel = " + scrollvelx + "," + scrollvely + " scl = " + scrollx + "," + scrolly +
//		" zoom = " + zoom + " zoomvel " + zoomvel;
//	out1.firstChild.nodeValue = "test1";
//	out2.firstChild.nodeValue = "test2";
}

