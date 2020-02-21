// physics demo, simpler, less accurate
var printshapes,printnshapes,printcount; // element handles
var ps = ""; // printnshapes
var cntr = 0; // frame counter
var mul = 0; // more if held down for awhile

// global time
var time = 0;
var timestep = 1;
var timemul = 3;

// global physics parameters

//var littleg = new Point2(0,0);
var littleg = new Point2(0,-.125);
//var littleg = new Point2(0,-.0125);
//var littleg = new Point2(-1,-2.25);
var vdamp = .995;//.95; // velocity damp
var rvdamp = .995; // rot velocity damp

var elast = .65;//1;//.7;//1;//.5; //1;//.95; //1;
var ustatic = .3;
var udynamic = .25;
//var elastfric = true; // conservative friction
var elastfric = false;

//var norot = true; // make moi infinite
var norot = false;

var littlegm2 = sdot2vv(littleg,littleg);

// total energies
var penergy = 0;
var kenergy = 0;
var renergy = 0;
var tenergy = 0;

var types = {"Wall":0,"Plank":1,"Ball":2,"num":3};

var shapes;// = new Array();
var maxshapes = 100;
var minshapes;
var NRECTPOINTS = 4;

// test
//var maxdiff = 0;

////// helper objects ///////////
function drawpoint(pos,rad) {
	var tbr = 2*rad;
	sprite_setsize(tbr,tbr);
	sprite_sethand(.5,.5);
	sprite_setangle(null);
	sprite_draw(pos.x,viewy - pos.y,"ball1.png");
}

////// Wall (immovable)
function Wall(x,y,
		nx,ny) {
	if (shapes.length == maxshapes)
		return;
	if (!x)
		x = 0;
	if (!y)
		y = 0;
	var pos = new Point2(x,y);
	var norm = new Point2(nx,ny);
	normalize2d(norm);
	this.vel = new Point2(0,0);
	this.invmass = 0;
	this.invmoi = 0;
	this.norm = norm;
	this.d = sdot2vv(pos,norm);
	this.stype = "Wall";
	//this.show = wallshow;
	//this.draw = walldraw;
	shapes.push(this); // keep track of this shape
}

Wall.prototype.show = function() {
	ps += "(W " + 
	this.norm.x.toFixed(2) + " " + 
	this.norm.y.toFixed(2) + " " + 
	this.d.toFixed(2) + ")\n";
};

Wall.prototype.draw = function() {
};

////// Ball
function Ball(m,
		x,y,r,
		vx,vy,vr,
		rad) {
	if (shapes.length == maxshapes)
		return;
	var im;
	if (m != 0)
		im = 1/m;
	else
		im = m;
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
	this.invmass = im;
	if (im) {
		this.invmoi = 2*im/(rad*rad);
	}
	if (norot)
		this.invmoi = 0;
	this.pos = new Point2(x,y);
	this.rot = r;
	this.vel = new Point2(vx,vy);
	this.rotvel = vr;
	this.rad = rad;
	this.stype = "Ball";
	//this.show = ballshow;
	//this.draw = balldraw;
	shapes.push(this); // keep track of this shape
}

Ball.prototype.show = function() {
	ps += "(B " + 
	this.invmass.toFixed(2) + " " + 
	this.pos.x.toFixed(2) + " " + 
	this.pos.y.toFixed(2) + " " + 
	this.rot.toFixed(2) + " " + 
	this.vel.x.toFixed(2) + " " + 
	this.vel.y.toFixed(2) + " " + 
	this.rotvel.toFixed(2) + " " + 
	this.rad.toFixed(2) + ")\n";
};

Ball.prototype.draw = function() {
	var tbr = 2*this.rad;
	sprite_setsize(tbr,tbr);
	sprite_sethand(.5,.5);
	sprite_setangle(-this.rot);
	sprite_draw(this.pos.x,viewy - this.pos.y,"ball5.png");
};

function calcpr(b)
{
	b.p[0].x = -.5*b.w;
	b.p[0].y =  .5*b.h;
	b.p[1].x =  .5*b.w;
	b.p[1].y =  .5*b.h;
	b.p[2].x =  .5*b.w;
	b.p[2].y = -.5*b.h;
	b.p[3].x = -.5*b.w;
	b.p[3].y = -.5*b.h;
	rotpoints2d(b.p,b.pr,b.rot,NRECTPOINTS);
	var i;
	for (i=0;i<NRECTPOINTS;++i) {
		b.pr[i] = vadd2vv(b.pr[i],b.pos);
	}
}

////// Plank
function Plank(m,
		x,y,r,
		vx,vy,vr,
		w,h) {
	if (shapes.length == maxshapes)
		return;
	var im;
	if (m != 0)
		im = 1/m;
	else
		im = m;
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
	this.invmass = im;
	if (im) {
		this.invmoi = 12*im/(w*w+h*h);
	}
	if (norot)
		this.invmoi = 0;
	this.pos = new Point2(x,y);
	this.rot = r;
	this.vel = new Point2(vx,vy);
	this.rotvel = vr;
	this.w = w;
	this.h = h;
	this.stype = "Plank";
	//this.show = plankshow;
	//this.draw = plankdraw;
	this.p = [new Point2(),new Point2(),new Point2(),new Point2()];
	this.pr = [new Point2(),new Point2(),new Point2(),new Point2()];
	shapes.push(this);
}

Plank.prototype.show = function() {
	ps += "(P " + 
	this.invmass.toFixed(2) + " " + 
	this.pos.x.toFixed(2) + " " + 
	this.pos.y.toFixed(2) + " " + 
	this.rot.toFixed(2) + " " + 
	this.vel.x.toFixed(2) + " " + 
	this.vel.y.toFixed(2) + " " + 
	this.rotvel.toFixed(2) + " " + 
	this.w.toFixed(2) + " " + 
	this.h.toFixed(2) + ")\n";
};

Plank.prototype.draw = function () {
	var tbw = this.w;
	var tbh = this.h;
	if (tbw < tbh) {
		sprite_setangle(-this.rot + Math.PI*.5);
		tbw = this.h; // try again
		tbh = this.w;
	} else {
		sprite_setangle(-this.rot);
	}
	var rat = Math.round(tbw/tbh);
	if (rat < 1)
		rat = 1;
	else if (rat > 6)
		rat = 6;
	sprite_setsize(tbw,tbh);
	sprite_sethand(.5,.5);
	sprite_draw(this.pos.x,viewy - this.pos.y,"plank" + rat + ".png");
};

/*function plankdrawo() {
	var tbw = this.w;
	var tbh = this.h;
	sprite_setangle(-this.rot);
	var rat = 1;
	sprite_setsize(tbw,tbh);
	sprite_sethand(.5,.5);
	sprite_draw(this.pos.x,viewy - this.pos.y,"plank" + rat + ".png");
}
*/
// contact info
// cn: normal, points from b to a
// penm: penetration magnitude
// cp: coll point, at b, other coll point is at cp + penm*cn
var collinfo = {};

// specific collisions
// sets collinfo object
function wall2wall(a,b) {
	//logger_str += "(wall2wall )";
	return false;
}

/*function ball2ball(sa,sb) {
	//logger_str += "(ball2ball )";
	var agrad = sa.rad;
	var bgrad = sb.rad;
	var delpos = vsub2vv(sa.pos,sb.pos);
	var d2 = sdot2vv(delpos,delpos); // distance squared
	var rs = agrad + bgrad;
	var rs2 = rs*rs;
	if (rs2 <= d2)
		return false;
	var d = Math.sqrt(d2);
	var penm = rs - d; // penetration distance
	// COLLIDING
	collinfo.cn = vmul2sv(1/d,delpos); // normal 
	collinfo.penm = penm;
	return true;
}
*/
function fixupcp(cp,nrm,penm) {
	cp.x += nrm.x*penm;
	cp.y += nrm.y*penm;
}

function ball2ball(a,b) {
	//logger_str += "(ball2ball )";
	var del = vsub2vv(a.pos,b.pos);
	var dist2 = del.x*del.x + del.y*del.y;
	var rsum = a.rad + b.rad;
	if (dist2 >= rsum*rsum)
		return false;
	var d = Math.sqrt(dist2);
	//var nrm = del;
	//normalize2d(nrm);
	var nrm = vmul2sv(1/d,del);
	cp = vmul2sv(-a.rad,nrm);
	cp = vadd2vv(cp,a.pos);
	penm = rsum - Math.sqrt(dist2);
	collinfo.penm = penm;
	collinfo.cn = nrm;
	collinfo.cp = cp;
	fixupcp(collinfo.cp,nrm,penm);
	return true;
}

//var bestcp = new Point2();
//var bestpendir = new Point2();

function plank2planko(a,b) {
	//logger_str += "(plank2plank )";
	var res = util_plank2plank(a,b);
//	res = false;
//	if (res) {
		//collinfo.cn = 
		//collinfo.penm = 
		//collinfo.cp = 
//	}
	return res;
}

var move = [{"x":0,"y":1},{"x":0,"y":-1},{"x":1,"y":0},{"x":-1,"y":0}];
var arr = [
	[],
	[],
	[],
	[]
]; //pointf2 arr[NRECTPOINTS][NRECTPOINTS];
var warr = []; //pointi2 warr[NRECTPOINTS+NRECTPOINTS];

function plank2plank(a,b) {
	//logger_str += "(plank2plank )";
	var i,j;
	// build 2d array of differences
	// TODO need an early out (AABB)
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
//			var new Point2()
//			pointf2x diff(a.pr[i].x-b.pr[j].x,a.pr[i].y-b.pr[j].y);
			arr[j][i] = vsub2vv(a.pr[i],b.pr[j]);
		}
	}
	var wi = 0; // walk
	var wj = 0;
	var wp = arr[0][0];
	// find lowest y value, then lowest x value (incase of 2 or more lowest y values)
	for (j=0;j<NRECTPOINTS;++j) {
		for (i=0;i<NRECTPOINTS;++i) {
			var lop = arr[j][i];
			if (lop.y < wp.y || (lop.y == wp.y && lop.x < wp.x)) { // there should be no points at the same place
				wi = i;
				wj = j;
				wp = lop;
			}
		}
	}

	var wloc = new Point2(wi,wj); //pointi2x wloc(wi,wj);
	var widx = 0;
	// bool hilits[NRECTPOINTS+NRECTPOINTS]; // used just for drawing
	// ::fill(hilits,hilits+NRECTPOINTS+NRECTPOINTS,false);
	warr[widx++] = wloc;
	var wang = 0;
	// walk thru the points, doing gift wrapping
 
	while(widx < 8) {
		// try the 4 'nearest' points (by connection, not distance)
		var k;
		var bestk=0;
		var nwi,nwj;
		var bestang = 5; // bigger than any angle 0-4
		for (k=0;k<4;++k) { // use the one with the lowest angle
			nwi = (wi + move[k].x + NRECTPOINTS)%NRECTPOINTS;
			nwj = (wj + move[k].y + NRECTPOINTS)%NRECTPOINTS;
			var pdest = arr[nwj][nwi];
			var ang = cheapatan2delta(wp,pdest);
			if (ang <= bestang && ang >= wang) {
				bestk = k;
				bestang = ang;
			}
		}
		nwi = (wi + move[bestk].x + NRECTPOINTS)%NRECTPOINTS;
		nwj = (wj + move[bestk].y + NRECTPOINTS)%NRECTPOINTS;
		warr[widx++] = new Point2(nwi,nwj);
		wi = nwi;
		wj = nwj;
		wp = arr[wj][wi];
		wang = bestang;
	} 
	var bestpen = 1e20;
	var bestidx = 0;
	var coll = false;
	var bestnrm = null;

	// got 8 points, find if inside and if so find closest line with 2 points
	for (i=0;i<NRECTPOINTS+NRECTPOINTS;++i) {
		j = (i+1)%(NRECTPOINTS+NRECTPOINTS);
		var p0 = arr[warr[i].y][warr[i].x];
		var p1 = arr[warr[j].y][warr[j].x];
		var pd = vsub2vv(p1,p0);
		var nrm = new Point2(pd.y,-pd.x);
		normalize2d(nrm);
		var d = sdot2vv(nrm,p0);
		if (d <= 0) { // no collision
			coll = false;
			break;
		}
		var d1 = sdot2vv(p0,pd);
		var d2 = sdot2vv(p1,pd);
		if (d < bestpen && ((d1 >= 0 && d2<= 0) || (d1 <= 0 && d2 >= 0))) { 
			// left of line segment and a line from point intersects line segment at 90 degrees
			bestpen = d;
			bestidx = i;
			bestnrm = nrm;
			coll = true;
		}
	}
	// find line segment and point
	if (coll) {
		i = bestidx;
		j = (i+1)%(NRECTPOINTS+NRECTPOINTS);
		bestnrm.x = -bestnrm.x;
		bestnrm.y = -bestnrm.y;
		collinfo.cn = bestnrm;
		var pen = bestpen;
		var cp;
		var newcp = true;
		if (newcp) {
 			// better for deeper penetrations
			// pick a more central collision point
			var paccum = new Point2(0,0);
			var pcnt = 0;
			cp = new Point2();
			// use all points inside and intersections
			for (i=0;i<NRECTPOINTS;++i) {
				if (util_point2plank(b.pr[i],a)) {
					paccum.x += b.pr[i].x;
					paccum.y += b.pr[i].y;
					++pcnt;
				}
			}
			for (i=0;i<NRECTPOINTS;++i) {
				if (util_point2plank(a.pr[i],b)) {
					paccum.x += a.pr[i].x;
					paccum.y += a.pr[i].y;
					++pcnt;
				}
			}
			var is = new Point2();
			for (i=0;i<NRECTPOINTS;++i) {
				var la0 = a.pr[i];
				var la1 = a.pr[(i + 1)%NRECTPOINTS];
				for (j=0;j<NRECTPOINTS;++j) {
					var lb0 = b.pr[j];
					var lb1 = b.pr[(j + 1)%NRECTPOINTS];
					if (getintersection2d(la0,la1,lb0,lb1,is)) {
						paccum.x += is.x;
						paccum.y += is.y;
						++pcnt;
					}
				}
			}
			if (!pcnt)
				undefined("pcnt == 0");
			pcnt = 1.0 / pcnt;
			cp.x = paccum.x * pcnt;
			cp.y = paccum.y * pcnt;			
			collinfo.cp = cp;
		} else {
			if (warr[i].x == warr[j].x) { // same point in a
				var cp2 = a.pr[warr[i].x];
				cp = new Point2(cp2.x,cp2.y);
			} else if (warr[i].y == warr[j].y) { // same point in b
				var cp2 = b.pr[warr[i].y];
				cp = new Point2(cp2.x,cp2.y);
				cp.x -= pen * bestnrm.x;
				cp.y -= pen * bestnrm.y;
			} else { // what ??
				//cp = pointf2x(3,3);
				error("what");
			}
			collinfo.cp = cp;
			fixupcp(collinfo.cp,bestnrm,pen);
		}
		collinfo.penm = pen;
	}
	return coll;
}

function ball2wall(sb,sw) {
	//logger_str += "(ball2wall )";
	var penm = sw.d + sb.rad - sdot2vv(sb.pos,sw.norm);
	if (penm <= 0)
		return false;
	// COLLIDING
	collinfo.cn = sw.norm; // normal 
	collinfo.penm = penm;
	collinfo.cp = vmul2sv(-sb.rad,sw.norm);
	collinfo.cp = vadd2vv(collinfo.cp,sb.pos);
	fixupcp(collinfo.cp,sw.norm,penm);
	return true;
}

function plank2wall(sp,sw) {
	//logger_str += "(plank2wall )";
	var newcp = true;
	if (newcp) { // new way, better with deeper penetration
		var i;
		var sum = new Point2(0,0);
		var npnt = 0;
		for (i=0;i<NRECTPOINTS;++i) { // take all the points that are penetrating and find the deepest one
			var vert = sp.pr[i];
			//var vert = vadd2vv(sp.pr[i],sp.pos);
			var penm = sw.d - sdot2vv(vert,sw.norm);
			if (penm > 0) {
				// vert COLLIDING
				sum = vadd2vv(sum,vert);
				++npnt;
			}
		}
		if (npnt) {
			// run through all the intersections
/*			var is = new Point2();
			for (i=0;i<NRECTPOINTS;++i) { // take all the points that are penetrating and find the deepest one
				var la0 = sp.pr[i];
				var la1 = sp.pr[(i + 1)%NRECTPOINTS];
				if (getintersection2dplane(la0,la1,sw.norm,sw.d,is)) {
					sum = vadd2vv(sum,is);
					++npnt;
				}	
			}	*/ // seems better if commented out	
			collinfo.cn = sw.norm;
			npnt = 1.0 / npnt;
			sum.x *= npnt;
			sum.y *= npnt;
			var penm = sw.d - sdot2vv(sum,sw.norm);
			collinfo.penm = penm;
			collinfo.cp = new Point2(sum.x,sum.y);
			return true;
		}
		return false;
	} else { // old way
		var i;
		var sum = new Point2(0,0);
		var bestvert;
		var bestpenm = 0;
		for (i=0;i<NRECTPOINTS;++i) { // take all the points that are penetrating and find the deepest one
			var vert = sp.pr[i];
			//var vert = vadd2vv(sp.pr[i],sp.pos);
			var penm = sw.d - sdot2vv(vert,sw.norm);
			if (penm > 0) {
				// vert COLLIDING
				if (penm > bestpenm) {
					bestpenm = penm;
					bestvert = vert;
				}
			}
		}
		if (bestpenm > 0) {
			collinfo.cn = sw.norm;
			collinfo.penm = bestpenm;
			collinfo.cp = new Point2(bestvert.x,bestvert.y);
			fixupcp(collinfo.cp,sw.norm,bestpenm);
			return true;
		}
		return false;
	}
}

function ball2plank(b,a) {
	//logger_str += "(ball2plank )";
// TODO AABB early out
	var bestidx = 0;
	var coll = false;
	var bestnrm = new Point2();
	var bestpen = 1e20;
	var i,j;
//	see if ball is close to edge
	for (i=0;i<NRECTPOINTS;++i) {
		j = (i+1)%(NRECTPOINTS);
		var p0 = a.pr[i];
		var p1 = a.pr[j];
		var pd = vsub2vv(p1,p0);
		var nrm = new Point2(pd.y,-pd.x);
		normalize2d(nrm);
		var d = sdot2vv(nrm,p0); // line in d,nrm  format
		var pen = sdot2vv(nrm,b.pos) - d + b.rad;
		if (pen <= 0) {
			coll = false;
			break; // too far away, no collision
		}
		// now work 90 degrees from nrm
		var d1 = sdot2vv(p0,pd);
		var d2 = sdot2vv(p1,pd);
		var dp = sdot2vv(b.pos,pd);
//		if (pen < bestpen) { 
		if (pen < bestpen && ((d1 >= dp && d2<= dp) || (d1 <= dp && d2 >= dp))) { 
			// left of line segment and a line from point intersects line segment at 90 degrees
			bestpen = pen;
			bestidx = i;
			bestnrm = nrm;
			coll = true;
		}
	}
	if (coll) 
		if (bestpen > 20)
			var q = 3.1;
	if (!coll && i == NRECTPOINTS) { // check corners
		var bestdist2 = 1e20;
		for (i=0;i<NRECTPOINTS;++i) {
			var del = vsub2vv(a.pr[i],b.pos);
			var dist2 = del.x*del.x + del.y*del.y;
			if (dist2 >= b.rad*b.rad)
				continue;
			if (dist2 < bestdist2) {
				bestdist2 = dist2;
				bestidx = i;
				coll = true;
			}
		}
		if (coll) {
			bestnrm = vsub2vv(a.pr[bestidx],b.pos);
			normalize2d(bestnrm); // this might be wrong, could be 0
			bestpen = b.rad - Math.sqrt(bestdist2);
			if (bestpen > 20)
				var q = 3.1;
		}
	}
	if (coll) {
		collinfo.penm = bestpen;
		collinfo.cp = vmul2sv(b.rad,bestnrm);
		collinfo.cp = vadd2vv(collinfo.cp,b.pos);
		bestnrm.x = -bestnrm.x;
		bestnrm.y = -bestnrm.y;
		collinfo.cn = bestnrm;
		fixupcp(collinfo.cp,bestnrm,bestpen);
	}
	return coll;
}

/*
// general collision
function buildcollmatrix() { // all types of collisions
	logger_str += "(buildcollmatrix) ";
	var ret = [
		[wall2wall],
		[plank2wall,plank2plank],
		[ball2wall,ball2plank,ball2ball]
	];
	return ret;
}

var collmatrix = buildcollmatrix();
*/
var collmatrix = [
		[wall2wall],
		[plank2wall,plank2plank],
		[ball2wall,ball2plank,ball2ball]
	];
	
function collide(sa,sb) {
	// early out
	var tim = sa.invmass + sb.invmass;
	if (tim <= 0)
		return;
	// switch objects if necessary 
	var satype = types[sa.stype];
	var sbtype = types[sb.stype];
	if (satype < sbtype) {
		var t = sa;
		sa = sb;
		sb = t;
		var t = satype;
		satype = sbtype;
		sbtype = t;
	}
	// do the collision
	var res = collmatrix[satype][sbtype](sa,sb);
	if (!res) // no collision
		return;
	var cn = collinfo.cn; // normal of impulse from b to a
	var cp = collinfo.cp; // where the collision took place
	var penm = collinfo.penm; // how deep the collision was
	// display collision info
	var cp2 = vmul2sv(.5*penm,cn);
	cp2 = vadd2vv(cp2,cp);
	drawpoint(cp,6);
	drawpoint(cp2,3);
/*	// position update due to penetration, maybe do later
	var resolvepen = .1; // 0 to 1, 0 never, 1 instant
	if (sa.invmass) {
		var pena = resolvepen*penm*sa.invmass/tim;
		var tda = vmul2sv(pena,cn);
		sa.pos = vadd2vv(sa.pos,tda);
	}
	if (sb.invmass) {
		var penb = resolvepen*penm*sb.invmass/tim;
		var tdb = vmul2sv(penb,cn);
		sb.pos = vsub2vv(sb.pos,tdb);
	} */
	
	// velocity update very long
	// calc rel vel
	var rveltrans = vsub2vv(sa.vel,sb.vel); // rel vel, trans part, a rel to b
	var rvelk = rveltrans;
	//var rvelmo = -sdot2vv(cn,rvelk); // rel vel, trans part projected onto normal
	if (sa.invmoi) {
		var ra = vsub2vv(cp,sa.pos);
		//var racn = scross2vv(ra,cn);
		//rvelmo -= sa.rotvel*racn;
		var rva = vcross2zv(sa.rotvel,ra);
		rvelk = vadd2vv(rvelk,rva);
	}
	if (sb.invmoi) {
		var rb = vsub2vv(cp,sb.pos);
		//var rbcn = scross2vv(rb,cn);
		//rvelmo += sb.rotvel*rbcn;
		var rvb = vcross2zv(sb.rotvel,rb);
		rvelk = vsub2vv(rvelk,rvb);
	}
	
	// calc k, the impulse
	var rvelm = -sdot2vv(rvelk,cn); // vel rel to -normal, should be positive
	//var diff = Math.abs(rvelm-rvelmo);
	//if (diff > maxdiff)
	//	maxdiff = diff;
	if (rvelm <= 0)  // pen velocity
		return; // already moving away
	//  impulse formula
	var timm = tim;
	if (sa.invmoi) {
		var racn = scross2vv(ra,cn);
		timm += racn*racn*sa.invmoi;
	}
	if (sb.invmoi) {
		var rbcn = scross2vv(rb,cn);
		timm += rbcn*rbcn*sb.invmoi;
	}
	var k = (1+elast)*rvelm/timm;

	// apply impulse maybe do later
	//k = 0;
	if (sa.invmass) {
		var dva = k*sa.invmass;
		var tva = vmul2sv(dva,cn);
		sa.vel = vadd2vv(sa.vel,tva);
	}
	if (sa.invmoi) {
		sa.rotvel += k*racn*sa.invmoi;
	}
	if (sb.invmass) {
		var dvb = k*sb.invmass;
		var tvb = vmul2sv(dvb,cn);
		sb.vel = vsub2vv(sb.vel,tvb);
	}
	if (sb.invmoi) {
		sb.rotvel -= k*rbcn*sb.invmoi;
	}
	// new friction
	var f = 0;
	if (ustatic > 0) {
		// calc a new rvel
		var rvelf = rveltrans;
		//var rvelmo = -sdot2vv(tang,rvelf); // rel vel, trans part projected onto normal
		if (sa.invmoi) {
			var ra = vsub2vv(cp,sa.pos);
			//var racn = scross2vv(ra,cn);
			//rvelmo -= sa.rotvel*racn;
			var rva = vcross2zv(sa.rotvel,ra);
			rvelf = vadd2vv(rvelf,rva);
		}
		if (sb.invmoi) {
			var rb = vsub2vv(cp,sb.pos);
			//var rbcn = scross2vv(rb,cn);
			//rvelmo += sb.rotvel*rbcn;
			var rvb = vcross2zv(sb.rotvel,rb);
			rvelf = vsub2vv(rvelf,rvb);
		}
		// try a new direction of force here
		var tang = new Point2(cn.y,-cn.x); // 90 degrees to normal
		var rvelt = -sdot2vv(rvelf,tang);
		if (rvelt < 0) { // make sure force is opposite the rvelf
			rvelt = -rvelt;
			tang.x = -tang.x;
			tang.y = -tang.y;
		}
		if (rvelt > 0) {
			//var diff = Math.abs(rvelm-rvelmo);
			//if (diff > maxdiff)
			//	maxdiff = diff;
			//if (rvelm <= 0)  // pen velocity
			//	return;
			//  impulse formula
			var timt = tim;
			if (sa.invmoi) {
				var racnt = scross2vv(ra,tang);
				timt += racnt*racnt*sa.invmoi;
			}
			if (sb.invmoi) {
				var rbcnt = scross2vv(rb,tang);
				timt += rbcnt*rbcnt*sb.invmoi;
			}
			if (elastfric) {
				f = 2*rvelt/timt; // this f will bounce it back
			} else {
				f = rvelt/timt; // this f will stop objects
				fs = k * ustatic;
				if (f > fs) { // then slip
					fd = k * udynamic;
					f = fd;
				}
			}
		}
	}
/*	
	// apply impulse maybe do sooner
	//k = 0;
	if (sa.invmass) {
		var dva = k*sa.invmass;
		var tva = vmul2sv(dva,cn);
		sa.vel = vadd2vv(sa.vel,tva);
	}
	if (sa.invmoi) {
		sa.rotvel += k*racn*sa.invmoi;
	}
	if (sb.invmass) {
		var dvb = k*sb.invmass;
		var tvb = vmul2sv(dvb,cn);
		sb.vel = vsub2vv(sb.vel,tvb);
	}
	if (sb.invmoi) {
		sb.rotvel -= k*rbcn*sb.invmoi;
	}
*/
	// apply new friction impulse
	if (f != 0) {
		if (sa.invmass) {
			var dvat = f*sa.invmass;
			var tvat = vmul2sv(dvat,tang);
			sa.vel = vadd2vv(sa.vel,tvat);
		}
		if (sa.invmoi) {
			sa.rotvel += f*racnt*sa.invmoi;
		}
		if (sb.invmass) {
			var dvbt = f*sb.invmass;
			var tvbt = vmul2sv(dvbt,tang);
			sb.vel = vsub2vv(sb.vel,tvbt);
		}
		if (sb.invmoi) {
			sb.rotvel -= f*rbcnt*sb.invmoi;
		}
	}
	// position update due to penetration, maybe do sooner
	var resolvepen = .1; // 0 to 1, 0 never, 1 instant
	if (sa.invmass) {
		var pena = resolvepen*penm*sa.invmass/tim;
		var tda = vmul2sv(pena,cn);
		sa.pos = vadd2vv(sa.pos,tda);
	}
	if (sb.invmass) {
		var penb = resolvepen*penm*sb.invmass/tim;
		var tdb = vmul2sv(penb,cn);
		sb.pos = vsub2vv(sb.pos,tdb);
	}
}


// run the physics etc
function procshapes(ts) {
	penergy = 0;
	kenergy = 0;
	renergy = 0;
	var i,j;
	// move all objects
	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		// preprocess planks
		if (s.stype == "Plank") {
			calcpr(s);
		}
		// move
		if (s.invmass <= 0)
			continue;	// can't move walls etc.
		// air friction
		s.vel = vmul2sv(vdamp,s.vel); // beware, should be damp^ts, we'll see
		s.rotvel *= rvdamp;
		// integrator
		// p1 = p0 + v0t + 1/2at^2
		var vt = vmul2sv(ts,s.vel);
		var at2 = vmul2sv(.5*ts*ts,littleg);
		s.pos = vadd2vv(s.pos,vt);
		s.pos = vadd2vv(s.pos,at2);
		//s.pos.x += .1;
		// v1 = v0 + at
		var at = vmul2sv(ts,littleg);
		s.vel = vadd2vv(s.vel,at);
		// rotate
		s.rot += ts*s.rotvel;
		s.rot = normang(s.rot);
	}
	// do shape to shape collisions
	for (i=0;i<shapes.length;++i) {
		var sa = shapes[i];
		for (j=i+1;j<shapes.length;++j) {
			var sb = shapes[j];
			collide(sa,sb);
		}
	}
	// update stats
	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		if (s.invmass) {
			var m = 1/s.invmass;
			penergy -= m*sdot2vv(s.pos,littleg); // littleg is <0
			kenergy += m*.5*sdot2vv(s.vel,s.vel);
		}
		if (s.invmoi) {
			renergy += .5*s.rotvel*s.rotvel/s.invmoi;
		}
	}
	tenergy = penergy + kenergy + renergy;
}

function drawshapes() {
	var i;
	ps = "shapes: ";
	for (i=0;i<shapes.length;++i) {
		var s = shapes[i];
		s.show(); // text
		s.draw(); // graphic
	}
	//drawpoint(new Point2(100,200),8);
}

// click events
// remove all shapes
function resetshapes(b) {
	if (!b) { // exit state
		b = {"value":"exiting"};
		logger("(resetshapese " + b.value + ")\n");
		shapes.length = 0;
	} else { // reset button
		logger("(resetshapesr " + b.value + ")\n");
		shapes.length = 4;
	}
}

// remove some shapes
function removeshapes(idx,cnt) {
	if (cnt == undefined)
		cnt = 1;
	shapes.splice(idx,cnt);
}

// rep events
// add or remove shapes depending on the button value
function morelessshapes(b) {
	logger("(morelessshapes " + b.value + ")\n");
	var v = 0;
	if (b.value == 'less') {
		v = -1;
	} else if (b.value == 'more') {
		v = 1;
	}
	var ds = v*Math.floor(Math.exp(mul/16));
	if (ds > 0) { // add new shapes
		var i;
		for (i=0;i<ds;++i) {
			if ((shapes.length % 2) == 1) {
				new Ball(1,
					200,200,0,
					5,20*Math.random(),0,
					50*Math.random()+25
				);
			} else { 
				var w = 125*Math.random()+20;
				var h = 125*Math.random()+20;
				new Plank(1,
					200,200,0,
//					5,15,0,
					5,20*Math.random(),0,
					w,h
				);
			} 
		}
	} else if (ds < 0) { // remove shapes
		if (minshapes > shapes.length + ds) {
			ds = minshapes - shapes.length;
		}
		removeshapes(shapes.length+ds,-ds); // remove from end
	}
	++mul;
}

// rep reset events
function resetmul(b) {
	logger("(resetmul " + b.value + ")\n");
	mul = 0;
}

var text3 = "2D: Application of sprite package.\n" +
			"2D physics research area, has walls circles and boxes, good collisions, friction, mass, MOI.\n" + 
			"uses 'localstorage' to load/save 'state' to/from a serialized JSON format.";
			
function init3() {
	//testserial();
	//testproto();
	shapes = loadshapes();
	if (shapes.length <= 4) {
		shapes.length = 0;
		new Wall(0,0,
			1,0
		);
		new Wall(viewx,0,
			-1,0
		);
		new Wall(0,0,
			0,1
		);
		new Wall(0,viewy,
			0,-1
		);
		minshapes = shapes.length;
	/*	new Ball(1,
			225.001,200,Math.PI/2,
			0,-1,0,
			50
		);
		new Plank(1,
			200,100,0,
			0,0,0,
			50,50
		); */
	/*	new Ball(1,
			200,200,Math.PI*(1/16),
			0,0,0,
			50
	);
	*/	
	/*	new Plank(1,
			400,200,Math.PI*(1/2+1/7),
			0,0,0,
			300,100
		); */
		new Plank(1,
			200,380,Math.PI*(1/16),
			5,-.5,0,
			150,50
		); 
	/*	new Ball(1,
			200,80,Math.PI*(1/16),
			2,-.5,0,
			50
		); */
		new Plank(0,
			400,200,Math.PI*(1/2),
			0,0,0,
			150,50
		);
	
	/*	new Ball(1,
			200,200,Math.PI*(1/16),
			3,0,2*Math.PI/fpswanted,
			50
	);
		new Ball(1,
			400,200,0,
			0,0,0,
			100
		); 
	*/
	/*	new Ball(1,
			310,200,0,
			0,0,0,
			50
		); */
		//collide(shapes[0],shapes[1]);
	/*	new Ball(1,
			200,0,0,
			0,5,0,
			50
		); */
	}
	var i;
	setbutsname('user');
	// less,more,reset
	makeaprintarea('Change number of shapes');
	makeabut("less",null,morelessshapes,resetmul); // rep
	makeabut("more",null,morelessshapes,resetmul); // rep
	//makeabr();
	makeabut("reset",resetshapes,null,null,true); // click
	
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
/*
	if (shapes[4]) {
		shapes[4].pos.x = 2*scrollx;
		shapes[4].pos.y = -2*scrolly;
}
*/
	var i;
	var ts = timestep / timemul;
	for (i=0;i<timemul;++i)
		procshapes(ts);
	// draw
	drawshapes();
	printareadraw(printnshapes,"nshapes = " + shapes.length + ", mul " + mul + ", time = " + time);
	printareadraw(printcount,"count " + cntr + 	", penergy " + penergy.toFixed(3) + 
												", kenergy " + kenergy.toFixed(3) + 
												", renergy " + renergy.toFixed(3) + 
												", tenergy " + tenergy.toFixed(3));
	printareadraw(printshapes,ps);
	// next frame
	++cntr;
	time += timestep;
}

function exit3() {
	saveshapes(shapes);
	resetshapes();
	clearbuts('user');
}

function loadshapes() {
	//return [];
	if (typeof(Storage) === "undefined")
		return [];
	if (!localStorage.physics_shapes)
		return [];
	try {
		var s = JSON.parse(localStorage.physics_shapes);
	}
	catch(err) {
		var s = [];
	}
	methodify(s);
	return s;
}

function saveshapes(s) {
	if (typeof(Storage) === "undefined")
		return;
	var jsonstr = JSON.stringify(shapes);
	localStorage.physics_shapes = jsonstr;
}
