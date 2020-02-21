// physics demo, simpler, less accurate
var printshapes,printnshapes,printcount; // element handles
var ps = ""; // printnshapes
var cntr = 0; // frame counter
var mul = 0; // more if held down for awhile

// global time
var time = 0;
var timestep = 1;

// global physics parameters
var gang = 0;

//var littleg = new Point2(0,0);
var littleg = new Point2(0,-1);
//var littleg = new Point2(-1,-2.25);

var littlegm2 = sdot2vv(littleg,littleg);

var damp = .9;//.995;//.95;
var elast = .5; //1;//.95; //1;

// total energies
var penergy = 0;
var kenergy = 0;
var tenergy = 0;

var types = {"ball":1,"plank":2,"wall":3};

// static collisions
var walls;


var shapes = new Array();
var maxshapes = 100;

////// Ball
function Ball(x,y,r,vx,vy,vr,rad) {
	if (shapes.length == maxshapes)
		return;
	if (!x)
		x = 0;
	if (!y)
		y = 0;
	if (!r)
		r = 0;
	if (!vx)
		vx = 0;
	if (!vy)
		vy = 0;
	if (!vr)
		vr = 0;
	if (!rad)
		rad = 10;
	this.pos = new Point2(x,y);
	this.rot = r;
	this.vel = new Point2(vx,vy);
	this.vr = vr;
	this.rad = rad;
	this.type = types.ball;
	this.show = ballshow;
	this.draw = balldraw;
	shapes.push(this); // keep track of this shape
}

function ballshow() {
		ps += "(B " + this.pos.x.toFixed(2) + " " + this.pos.y.toFixed(2) + " " + this.vel.x.toFixed(2) + " " + this.vel.y.toFixed(2) + " " + this.rad.toFixed(2) + ")";
}

function balldraw() {
		var tbr = 2*this.rad;
		sprite_setsize(tbr,tbr);
		sprite_sethand(.5,.5);
		sprite_setangle(gang);
		sprite_draw(this.pos.x,viewy - this.pos.y,"ball5.png");
}

////// Plank
function plankshow() {
	ps += "(P " + this.pos.x.toFixed(2) + " " + this.pos.y.toFixed(2) + " " + this.vel.x.toFixed(2) + " " + this.vel.y.toFixed(2) + " " + this.w.toFixed(2) + " " + this.h.toFixed(2) + ")";
}

function Plank(x,y,r,vx,vy,vr,w,h) {
	if (shapes.length == maxshapes)
		return;
	if (!x)
		x = 0;
	if (!y)
		y = 0;
	if (!r)
		r = 0;
	if (!vx)
		vx = 0;
	if (!vy)
		vy = 0;
	if (!vr)
		vr = 0;
	if (!w)
		w = 20;
	if (!h)
		h = 20;
	this.pos = new Point2(x,y);
	this.rot = r;
	this.vel = new Point2(vx,vy);
	this.vr = vr;
	this.w = w;
	this.h = h;
	this.type = types.plank;
	this.show = plankshow;
	this.draw = plankdraw;
	this.p = new Array();
	this.p[0] = new Point2(-.5*w, .5*h);
	this.p[1] = new Point2( .5*w, .5*h);
	this.p[2] = new Point2( .5*w,-.5*h);
	this.p[3] = new Point2(-.5*w,-.5*h);
	this.pr = new Array();
	var i;
	for (i=0;i<NRECTPOINTS;++i) {
		this.pr[i] = new Point2();
	}
	shapes.push(this);
}

function plankdraw() {
	var tbw = this.w;
	var tbh = this.h;
	if (tbw < tbh) {
		sprite_setangle(gang + Math.PI*.5);
		tbw = this.h; // try again
		tbh = this.w;
	} else {
		sprite_setangle(gang);
	}
	var rat = Math.round(tbw/tbh);
	if (rat < 1)
		rat = 1;
	else if (rat > 6)
		rat = 6;
	sprite_setsize(tbw,tbh);
	sprite_sethand(.5,.5);
	sprite_draw(this.pos.x,viewy - this.pos.y,"plank" + rat + ".png");
}

// click events
// remove all shapes
function resetshapes(b) {
	if (!b)
		b = {"value":"exiting"};
	logger += "(resetshapes " + b.value + ") ";
	shapes.length = 0;
}

// remove some shapes
function removeshapes(idx,cnt) {
	if (cnt == undefined)
		cnt = 1;
	shapes.splice(idx,cnt);
}

// run the physics etc
function procshapes(ts) {
	penergy = 0;
	kenergy = 0;
	var i,j;
	// move all objects
	for (i=0;i<shapes.length;++i) {
		// move
		var s = shapes[i];
		if (s.type == types.plank) {
			rotpoints2d(s.p,s.pr,s.rot,NRECTPOINTS);
		}
		// air friction
		s.vel = vmul2sv(damp,s.vel); // beware, should be damp^ts, we'll see
		var vt = vmul2sv(ts,s.vel);
		// p1 = p0 + v0t + 1/2at^2
		var at2 = vmul2sv(.5*ts*ts,littleg);
		s.pos = vadd2vv(s.pos,vt);
		s.pos = vadd2vv(s.pos,at2);
		// v1 = v0 + at
		var at = vmul2sv(ts,littleg);
		s.vel = vadd2vv(s.vel,at);
	}
	// do ball to ball collisions
	for (i=0;i<shapes.length;++i) {
		var sa = shapes[i];
		var agrad;
		if (sa.type == types.ball)
			agrad = sa.rad;
		else
			agrad = sa.w * .5;
		for (j=i+1;j<shapes.length;++j) {
			var sb = shapes[j];
			var bgrad;
			if (sb.type == types.ball)
				bgrad = sb.rad;
			else
				bgrad = sb.w * .5;
			var delpos = vsub2vv(sa.pos,sb.pos);
			var d2 = sdot2vv(delpos,delpos); // distance squared
			var rs = agrad + bgrad;
			var rs2 = rs*rs;
			if (rs2 > d2) {
				var d = Math.sqrt(d2);
				var penm = rs - d; // penetration distance
				// assume balls for now
				if (penm > 0) {
					// COLLIDING
					// do the bounce
					// distance
					var tdm = .5*penm; // calc teleport distance
					var cn = vmul2sv(1/d,delpos); // normal 
					var td = vmul2sv(tdm,cn);
					sa.pos = vadd2vv(sa.pos,td);
					sb.pos = vsub2vv(sb.pos,td);
					// velocity
					var sv = vsub2vv(sa.vel,sb.vel); // rel vel
					var velm = -sdot2vv(cn,sv); // rel vel projected onto normal
					var tvm;
					if (velm > 0) { // pen velocity
						// flip velocity (collision)
						// tvm that's our 'k'
						tvm = .5*velm*(1+elast);
						var tv = vmul2sv(tvm,cn);
						sa.vel = vadd2vv(sa.vel,tv);
						sb.vel = vsub2vv(sb.vel,tv);
					//} else {  // is object penetrating but moving apart?
					//	tvm = .5*velm; // stick vel
					}
				}
			}
		}
	}
	// collide with all the walls
	// do balls against walls
	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		for (j=0;j<walls.length;++j) {
			var c = walls[j];
			var penm = c.d - sdot2vv(c.n,s.pos);
			// assume balls for now
			if (s.type == types.ball)
				penm += s.rad; // penetration distance
			else
				penm += s.w*.5;
			var to = typeof(s);
			if (penm > 0) {
				// COLLIDING
				// do the bounce
				var tdm = penm; // calc teleport distance
				var td = vmul2sv(tdm,c.n);
				s.pos = vadd2vv(s.pos,td);
				var velm = -sdot2vv(c.n,s.vel); // vel projected onto normal, penetration velocity
				var tvm;
				if (velm > 0) {
					// flip velocity (collision),  reduced because of elast
					// tvm that's our 'k'
					tvm = velm*(1+elast);
				} else { // is object penetrating but moving apart?
					tvm = velm; // stick vel
				}
				var tv = vmul2sv(tvm,c.n);
				s.vel = vadd2vv(s.vel,tv);
			}
		}
	}

	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		penergy -=  sdot2vv(s.pos,littleg); // littleg is <0
		kenergy += .5*sdot2vv(s.vel,s.vel);
	}
	tenergy = penergy + kenergy;
}

function drawshapes() {
	var i;
	gang = cntr*2*Math.PI/spt_nfrm;
	ps = "shapes: ";
	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		s.show(); // text
		s.draw(); // graphic
	}
}

// rep events
// add or remove shapes depending on the button value
function morelessshapes(b) {
	logger += "(morelessshapes " + b.value + ") ";
	var v = 0;
	if (b.value == 'less') {
		v = -1;
	} else if (b.value == 'more') {
		v = 1;
	}
	var ds = v*Math.floor(Math.exp(mul/16));
	if (ds > 0) {
		var i;
		for (i=0;i<ds;++i) {
			if ((shapes.length % 2) == 0) {
				new Ball(
					200,200,0,
					5,20*Math.random(),0,
					50*Math.random()+25
				);
			} else {
				var s = 50*Math.random()+50;
				new Plank(200,200,0,
					5,15,0,
					s,s
				);
			} 
		}
	} else if (ds < 0) {
		removeshapes(shapes.length+ds,-ds); // remove from end
	}

	++mul;
}

// rep reset events
function resetmul(b) {
	logger += "(resetmul " + b.value + ") ";
	mul = 0;
}

function init3() {
	walls = [
		{"n" : {"x" :  1,"y" :  0,}, "d" : 0},
		{"n" : {"x" : -1,"y" :  0,}, "d" : -viewx},
		{"n" : {"x" :  0,"y" :  1,}, "d" : 0},
		{"n" : {"x" :  0,"y" : -1,}, "d" : -viewy}
	];
/*	new Ball(
		200,50,0,
		0,0,0,
		50
	); */
	new Ball(
		200,200,0,
		0,0,0,
		50
	); 
	new Ball(
		400,200,0,
		0,0,0,
		50
	); 
/*	new Ball(
		200,0,0,
		0,5,0,
		50
	); */
	var i;
	setbutsname('user');
	// less,more,reset
	makeaprintarea('Change number of shapes');
	makeabut("less",null,morelessshapes,resetmul); // rep
	makeabut("more",null,morelessshapes,resetmul); // rep
	makeabr();
	makeabut("reset",resetshapes); // click
	
	printnshapes = makeaprintarea();
	printcount = makeaprintarea();
	printshapes = makeaprintarea();
	
	cntr = 0;
	time = 0;
}

function proc3() {
	// background
	sprite_setsize(viewx,viewy);
	sprite_draw(0,0,"take0005.jpg");
	// proc
	procshapes(timestep);
	// draw
	drawshapes();
	printareadraw(printnshapes,"nshapes = " + shapes.length + ", mul " + mul + ", time = " + time);
	printareadraw(printcount,"count " + cntr + 	", penergy " + penergy.toFixed(3) + 
												", kenergy " + kenergy.toFixed(3) + 
												", tenergy " + tenergy.toFixed(3));
	printareadraw(printshapes,ps);
	// next frame
	++cntr;
	time += timestep;
}

function exit3() {
	resetshapes();
	clearbuts('user');
}
