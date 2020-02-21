// physics demo, more complicated
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

var damp = 1;//.995;//.95;
var elast = 1;//.8; //1;//.95; //1;

// total energies
var penergy = 0;
var kenergy = 0;
var tenergy = 0;

var types = {"ball":1,"plank":2};

// static collisions
var walls;

// Ball
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

function Ball(x,y,r,vx,vy,vr,rad) {
	if (!x)
		x = 0;
	if (!y)
		y = 0;
	if (!r)
		r = 0;
	this.pos = new Point2(x,y);
	this.r = r;
	if (!vx)
		vx = 0;
	if (!vy)
		vy = 0;
	if (!vr)
		vr = 0;
	this.vel = new Point2(vx,vy);
	this.vr = vr;
	if (!rad)
		rad = 10;
	this.rad = rad;
	this.type = types.ball;
	this.show = ballshow;
	this.draw = balldraw;
}

// Plank
function plankshow() {
	ps += "(P " + this.pos.x.toFixed(2) + " " + this.pos.y.toFixed(2) + " " + this.vel.x.toFixed(2) + " " + this.vel.y.toFixed(2) + " " + this.w.toFixed(2) + " " + this.h.toFixed(2) + ")";
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

function Plank(x,y,r,vx,vy,vr,w,h) {
	if (!x)
		x = 0;
	if (!y)
		y = 0;
	if (!r)
		r = 0;
	this.pos = new Point2(x,y);
	this.r = r;
	if (!vx)
		vx = 0;
	if (!vy)
		vy = 0;
	if (!vr)
		vr = 0;
	this.vel = new Point2(vx,vy);
	this.vr = vr;
	if (!w)
		w = 20;
	this.w = w;
	if (!h)
		h = 20;
	this.h = h;
	this.type = types.plank;
	this.show = plankshow;
	this.draw = plankdraw;
}

var shapes;
var maxshapes = 300;

// click events
function addball(x,y,r,vx,vy,vr,rad) {
	if (shapes.length == maxshapes)
		return;
	var aball = new Ball(x,y,r,vx,vy,vr,rad);
	shapes.push(aball);
}

function addplank(x,y,r,vx,vy,vr,w,h) {
	if (shapes.length == maxshapes)
		return;
	var aplank = new Plank(x,y,r,vx,vy,vr,w,h);
	shapes.push(aplank);
}

function resetshapes(b) {
	logger += "(resetshapes " + b.value + ") ";
	shapes.length = 0;
}

function removeshapes(idx,cnt) {
	if (cnt == undefined)
		cnt = 1;
	shapes.splice(idx,cnt);
}

function procshapes(ts) {
	penergy = 0;
	kenergy = 0;
	var i,j;
	// move all objects
	for (i=0;i<shapes.length;++i) {
		// move
		var s = shapes[i];
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
	// collide with all the walls
	// do balls against walls
	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		for (j=0;j<walls.length;++j) {
			var c = walls[j];
			var penm = c.d - sdot2vv(c.n,s.pos);
			// assume balls for now
			penm += s.rad;
			if (penm > 0) {
				var velm = sdot2vv(c.n,s.vel); // vel projected onto normal
				if (velm < 0) { 
					// COLLIDING
					//var pen = vmul2sv(penm,c.n); // move shape by this much and it will be on the surface

					// do the bounce
					// tdm that's our 'k'
					var tdm = penm*(1+elast); // calc teleport distance, reduced because of the elast
					var td = vmul2sv(tdm,c.n);
					s.pos = vadd2vv(s.pos,td);
					// flip velocity (collision),  reduced because of elast
					var tvm = velm*(1+elast);
					var tv = vmul2sv(tvm,c.n);
					s.vel = vsub2vv(s.vel,tv);

					// recalc vel and pos due to gravity
					if (littlegm2 <= 0)
						continue; // no gravity
					velm = sdot2vv(c.n,s.vel); // velocity component in direction of normal, should be positive
					if (velm < 0) {
						logger += "(velm < 0) ";
						//alert("velm < 0");
						velm = 0;
					}
					var oldkenng = .5*velm*velm; // kinetic energy in direction of normal
					var delpotng = -sdot2vv(littleg,td); // potential energy increased by this amount if on bottom
					var newkenng = oldkenng - delpotng; // kinetic energy at new position, so kinetic energy should be less if bounced on bottom, more if bounced on top
					if (newkenng >= 0) {
/*						if (oldkenng == 0) {
							oldkenng = .001;
							logger += "(oldkenng was == 0) ";
							alert("oldkenng was == 0");
						} */
						var velmfac = Math.sqrt(velm*velm - 2*delpotng); // do it without the divide
						var vn = vmul2sv(velmfac-velm,c.n); // adjust ke along the normal line, subtract off old velm, then add velm with factor
						// var fac = Math.sqrt(newkenng/oldkenng);
						// var vn = vmul2sv(velm*(fac-1),c.n); // adjust ke along the normal line, subtract off old velm, then add velm with factor
						s.vel = vadd2vv(s.vel,vn); // the kinetic energy will now be less by delpotng in the normal direction
					} else { // not enough kinetic energy along the normal line to be here
						var vn = vmul2sv(-velm,c.n); // adjust kinetic energy along the normal line, remove all vel in normal line
						s.vel = vadd2vv(s.vel,vn); // all vel is now perpendicular to norm
						
						var dgc = sdot2vv(littleg,c.n);
						var adjm = 0;
						if (dgc == 0) {
							logger += "(dgc == 0) "; // should never get here
							alert("dgc == 0");
						} else {
							adjm = newkenng/dgc; // normally a positive number
						}
						var adj = vmul2sv(adjm,c.n);
						s.pos = vsub2vv(s.pos,adj); // move closer to coll point, along normal line
						
						// recalc pen
						var penm = c.d - sdot2vv(c.n,s.pos);
						penm += s.rad;
						if (penm > 0) { // if past collision point, then set it to touch, lowest energy state
							var pen = vmul2sv(penm,c.n);
							s.pos = vadd2vv(s.pos,pen);
						}
					}
					
				}
			}
		}
	}
	// do ball to ball collisions
	for (i=0;i<shapes.length;++i) {
		var sa = shapes[i];
		for (j=i+1;j<shapes.length;++j) {
			var sb = shapes[j];
			var delpos = vsub2vv(sa.pos,sb.pos);
			var d2 = sdot2vv(delpos,delpos);
			var rs = sa.rad + sb.rad;
			var rs2 = rs*rs;
			if (rs2 > d2) {
				var d = Math.sqrt(d2);
				var penm = rs - d;
				// assume balls for now
				if (penm > 0) {
					var cn = vmul2sv(1/d,delpos);
					var sv = vsub2vv(sa.vel,sb.vel);
					var velm = sdot2vv(cn,sv); // rel vel projected onto normal
					if (velm < 0) { 
						// COLLIDING
						//var pen = vmul2sv(penm,c.n); // move shape by this much and it will be on the surface
	
						// do the bounce, assume elast = 1
						var tdm = penm; // calc teleport distance, reduced because of the elast
						var td = vmul2sv(tdm,cn);
						sa.pos = vadd2vv(sa.pos,td);
						sb.pos = vsub2vv(sb.pos,td);
						// flip velocity (collision),  reduced because of elast
						var tvm = velm;
						var tv = vmul2sv(tvm,cn);
						sa.vel = vsub2vv(sa.vel,tv);
						sb.vel = vadd2vv(sb.vel,tv);
	
						// recalc vel and pos due to gravity
						//if (littlegm2 <= 0)
						if (true)
							continue; // no gravity
/*						// might be 'trued out'
						// balla
						velm = sdot2vv(cn,sa.vel); // velocity component in direction of normal, should be positive
						if (velm < 0) {
							logger += "(velm < 0) ";
							//alert("velm < 0");
							velm = 0;
						}
						var oldkenng = .5*velm*velm; // kinetic energy in direction of normal
						var delpotng = -sdot2vv(littleg,td); // potential energy increased by this amount if on bottom
						var newkenng = oldkenng - delpotng; // kinetic energy at new position, so kinetic energy should be less if bounced on bottom, more if bounced on top
						if (newkenng >= 0) {
							var velmfac = Math.sqrt(velm*velm - 2*delpotng); // do it without the divide
							var vn = vmul2sv(velmfac-velm,cn); // adjust ke along the normal line, subtract off old velm, then add velm with factor
							// var fac = Math.sqrt(newkenng/oldkenng);
							// var vn = vmul2sv(velm*(fac-1),cn); // adjust ke along the normal line, subtract off old velm, then add velm with factor
							sa.vel = vadd2vv(sa.vel,vn); // the kinetic energy will now be less by delpotng in the normal direction
						}
						// ballb
						velm = sdot2vv(cn,sb.vel); // velocity component in direction of normal, should be positive
						if (velm < 0) {
							logger += "(velm < 0) ";
							//alert("velm < 0");
							velm = 0;
						}
						var oldkenng = .5*velm*velm; // kinetic energy in direction of normal
						var delpotng = +sdot2vv(littleg,td); // potential energy increased by this amount if on bottom
						var newkenng = oldkenng - delpotng; // kinetic energy at new position, so kinetic energy should be less if bounced on bottom, more if bounced on top
						if (newkenng >= 0) {
							var velmfac = Math.sqrt(velm*velm - 2*delpotng); // do it without the divide
							var vn = vmul2sv(velmfac-velm,cn); // adjust ke along the normal line, subtract off old velm, then add velm with factor
							// var fac = Math.sqrt(newkenng/oldkenng);
							// var vn = vmul2sv(velm*(fac-1),cn); // adjust ke along the normal line, subtract off old velm, then add velm with factor
							sb.vel = vadd2vv(sb.vel,vn); // the kinetic energy will now be less by delpotng in the normal direction
						}
						// end might be 'trued out' */
					}
				}
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
	//sprite_setrotatemode(true);
	//gang = cntr*2*Math.PI/spt_nfrm;
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
//			if ((shapes.length % 2) == 0) {
				addball(
					200,200,0,
					5,20*Math.random(),0,
					50*Math.random()+25
				);
/*			} else {
				var s = 50*Math.random()+50;
				addplank(200,200,0,
					5,15,0,
					s,s
				);
			} */
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
	shapes = new Array();
/*	addball(
		200,50,0,
		0,0,0,
		50
	); */
	addball(
		200,200,0,
		0,0,0,
		50
	); 
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
	clearbuts('user');
}
