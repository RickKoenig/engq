// sprite demo with cookies and drag
var state1 = {};

function rectshapeclass(xa,ya,wa,ha,pica) {
	this.x = xa;
	this.y = ya;
	this.w = wa;
	this.h = ha;
	this.pic = pica;
}

function state1class() {
	var i;
	this.nshapes = 20;
	this.shapes = new Array();
	for (i=0;i<this.nshapes;++i) {
		this.shapes[i] = new rectshapeclass(i*20,i*20,100,100,"take" + leftpad(i,4) + ".jpg");
	}
/*	this.x = 100;
	this.y = 100;
	this.w = 100;
	this.h = 100; */
	this.refx = 0;
	this.refy = 0;
	this.indrag = -1;
	this.oldmbut = 0;
}

function readcookie() {
	var myCookie = document.cookie.split("; ");
	var i;
	for (i=0;i<myCookie.length;++i) {
		var cookieData = myCookie[i].split("=");
		var posstr;
		posstr = cookieData[0].indexOf("posx");
		if (posstr >= 0) {
			var idx = parseInt(cookieData[0].substr(posstr+4));
			state1.shapes[idx].x = parseInt(cookieData[1]);
		}
		posstr = cookieData[0].indexOf("posy");
		if (posstr >= 0) {
			var idx = parseInt(cookieData[0].substr(posstr+4));
			state1.shapes[idx].y = parseInt(cookieData[1]);
		}
	}
}

function writecookie() {
	var when = new Date();
// 1 hour from now
	//when.setTime(when.getTime() +  1 * 60 * 60 * 1000);
// 24 hours from now
	//when.setTime(when.getTime() + 24 * 60 * 60 * 1000);
// 1 year from now
	//when.setFullYear(when.getFullYear() + 1);
// 10 years from now
	when.setFullYear(when.getFullYear() + 10);
	var i;
	for (i=0;i<state1.nshapes;++i) {
		var o = state1.shapes[i];
		document.cookie = escape("posx" + i) + "=" + escape(o.x) + ";expires=" + when.toGMTString();
		document.cookie = escape("posy" + i) + "=" + escape(o.y) + ";expires=" + when.toGMTString();
	}
}

function readcookie2() {
	if (typeof(Storage) === "undefined")
		return;
	if (!localStorage.shapes)
		return;
	var o = JSON.parse(localStorage.shapes);
	state1.shapes = o;
}

function writecookie2() {
	if (typeof(Storage) === "undefined")
		return;
	localStorage.pi="3.14";
	var jsonstr = JSON.stringify(state1.shapes);
	localStorage.shapes = jsonstr;
}

// state 1
state1.text = "2D: Drag the tiles around with the mouse.\n" + 
			"Uses 'localstorage' to load and save the tiles to a JSON format.";
state1.title = "drag tiles localstorage";
			
state1.init = function() {
	state1 = new state1class();
// getCookie
	//readcookie();
	readcookie2();
};

function insiderect(x,y,w,h,px,py) {
	return px >= x && px < x + w && py >= y && py < y + h;
}

state1.proc = function() {
// input
	var i;
	if (!state1.oldmbut && input.mbut[0]) { // going down
		for (i=state1.nshapes-1;i>=0;--i) {
			var o = state1.shapes[i];
			if (insiderect(o.x,o.y,o.w,o.h,input.mx,input.my)) {
				state1.refx = o.x - input.mx;
				state1.refy = o.y - input.my;
				state1.indrag = i;
				break;
			}
		}
	}
	if (!input.mbut[0]) // is up
		state1.indrag = -1;
	state1.oldmbut = input.mbut[0];
	if (state1.indrag>=0) { // is dragging
		state1.shapes[state1.indrag].x = input.mx + state1.refx;
		state1.shapes[state1.indrag].y = input.my + state1.refy;
	}
// draw
	sprite_setsize(viewx,viewy);
	sprite_draw(0,0,"take0006.jpg");
	for (i=0;i<state1.nshapes;++i) {
		var o = state1.shapes[i];
		sprite_setsize(o.w,o.h);
		sprite_draw(o.x,o.y,o.pic);
	}
};

state1.exit = function() {
// makeCookie
	//writecookie();
	writecookie2();
	state1 = null;
};
