// yes returns [-PI to PI)
// no  returns 0<=ang<2*PI eventually
function normalangrad(rad) {
	if ((rad > 1000000) || (rad < -1000000))
		alert("normalangrad getting too big! " + rad);
	var watch = 0;
	while (rad < -Math.PI) {
		rad += 2*Math.PI;
		++watch;
		if (watch > 1000) {
			alert("normalangrad too many while loops 1");
			return rad;
		}
	}
	while (rad >= Math.PI) {
		rad -= 2*Math.PI;
		++watch;
		if (watch > 1000) {
			alert("normalangrad too many while loops 2");
			return rad;
		}
	}
	return rad;
}

/*function normang(a) {
	if (a >= 1000*2*Math.PI)
		undefined();
	if (a <= -1000*2*Math.PI)
		undefined();
	while (a >= 2*Math.PI)
		a -= 2*Math.PI;
	while (a < 0)
		a += 2*Math.PI;
	return a;
} */


// handy functions

function divint(a,b) {
	var q = a/b;
	return Math.floor(q);
}

function modint(a,b) {
	var r = a%b;
	if (r<0) {
		r+=b;	
	}
	return Math.floor(r);
}

function irand(n) {
	return Math.floor(Math.random()*n);
}

function Point2(x,y) {
	this.x = x;
	this.y = y;
}

// dot product of 2 vectors
function sdot2vv(a,b) {
	return a.x*b.x + a.y*b.y;
}

// product of scalar with vector
function vmul2sv(s,v) {
	return new Point2(s*v.x,s*v.y);
}

function vadd2vv(a,b) {
	return new Point2(a.x+b.x,a.y+b.y);
}

function vsub2vv(a,b) {
	return new Point2(a.x-b.x,a.y-b.y);
}

function dist2(a,b) {
	var dx = a.x - b.x;
	var dy = a.y - b.y;
	return dx*dx + dy*dy;
}

function dist(a,b) {
	return Math.sqrt(dist2(a,b));
}

// rotate an array of points
function rotpoints2d(p,pr,ang,np) {
	var i;
	var fs=Math.sin(ang);
	var fc=Math.cos(ang);
	for (i=0;i<np;++i) {
		pr[i].x = fc*p[i].x - fs*p[i].y;
		pr[i].y = fc*p[i].y + fs*p[i].x;
	}
}

// intersection of 2 lines
function getintersection2d(la,lb,lc,ld,i0) {
	var e = lb.x - la.x;
	var f = lc.x - ld.x;
	var g = lc.x - la.x;
	var h = lb.y - la.y;
	var j = lc.y - ld.y;
	var k = lc.y - la.y;
	var det = e*j - f*h;
	if (det == 0)
		return false;
	det = 1/det;
	var t0 = (g*j - f*k)*det;
	var t1 = -(g*h - e*k)*det;
	if (t0>=0 && t0<=1 && t1>=0 && t1<=1) {
		if (i0) {
			i0.x = la.x + (lb.x - la.x)*t0;
			i0.y = la.y + (lb.y - la.y)*t0;
		}
		return true;
	}
	return false;
}

// intersection of 2 lines
function getintersection2d(la,lb,lc,ld,i0) {
	var e = lb.x - la.x;
	var f = lc.x - ld.x;
	var g = lc.x - la.x;
	var h = lb.y - la.y;
	var j = lc.y - ld.y;
	var k = lc.y - la.y;
	var det = e*j - f*h;
	if (det == 0)
		return false;
	det = 1/det;
	var t0 = (g*j - f*k)*det;
	var t1 = -(g*h - e*k)*det;
	if (t0>=0 && t0<=1 && t1>=0 && t1<=1) {
		if (i0) {
			i0.x = la.x + (lb.x - la.x)*t0;
			i0.y = la.y + (lb.y - la.y)*t0;
		}
		return true;
	}
	return false;
}

// intersection of 2 lines
function getintersection2dplane(p0,p1,n,d,i) {
	var p0dn = sdot2vv(p0,n);
	var p1dn = sdot2vv(p1,n);
	if ((d > p0dn && d < p1dn) || (d > p1dn && d < p0dn)) {
		var k = (d - p0dn)/(p1dn - p0dn);
		var del = vsub2vv(p1,p0);
		del = vmul2sv(k,del);
		var is = vadd2vv(p0,del);
		if (i) {
			i.x = is.x;
			i.y = is.y;
		}
		return true;
	}
	return false;
}

function normalize2d(v) {
	var d2 = v.x*v.x + v.y*v.y;
	if (d2 == 0) {
		v.x = 1; // point in some direction if a zero vector
		v.y = 0;
		return false;
	}
	var id = 1/Math.sqrt(d2);
	v.x *= id;
	v.y *= id;
	return true;
}

function scross2vv(a,b)
{
	return a.x*b.y - a.y*b.x;
}

function vcross2zv(a,b)
{
	return new Point2(-a*b.y,a*b.x);
}

function util_point2line(p,la,lb,nrma)
{
	var nrm = vsub2vv(la,lb);
	nrm = new Point2(nrm.y,-nrm.x);
	normalize2d(nrm);
	var d = sdot2vv(nrm,la) - sdot2vv(nrm,p);
	nrma.x = nrm.x;
	nrma.y = nrm.y;
	return Math.abs(d);
}


// assume not same point, return 0 to almost 4
function cheapatan2delta(from,to) {
	var dx = to.x - from.x;
	var dy = to.y - from.y;
	var ax = Math.abs(dx);
	var ay = Math.abs(dy);
	var ang = dy/(ax+ay);
	if (dx<0)
		ang = 2 - ang;
	else if (dy<0)
		ang = 4 + ang;
	return ang;
}



var NRECTPOINTS = 4;

var vs = [];
function util_point2plank(p,b)
{
	var i;
	var sgn = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		vs[i] = vsub2vv(b.pr[i],p);
	}
	for (i=0;i<NRECTPOINTS;++i) {
		var c = scross2vv(vs[i],vs[(i + 1)%NRECTPOINTS]);
		if (sgn == 0) {
			if (c >= 0) {
				sgn = 1;
			} else {
				sgn = -1;
			}
		} else {
			if (sgn == 1 && c < 0)
				return false;
			if (sgn == -1 && c >= 0)
				return false;
		}
	}
	return true;
}

var isidx = [new Point2(),new Point2()];
var insides = [new Point2(),new Point2()];
var bp = new Point2();
function util_plank2plank(a,b) {
// find intersections
	var i,j;
	var k = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		la0 = a.pr[i];
		la1 = a.pr[(i + 1)%NRECTPOINTS];
		for (j=0;j<NRECTPOINTS;++j) {
			lb0 = b.pr[j];
			lb1 = b.pr[(j + 1)%NRECTPOINTS];
			if (getintersection2d(la0,la1,lb0,lb1,0)) {
				if (k >= 2) {
					return false;
				}
				isidx[k].x = i;
				isidx[k].y = j;
				++k;
			}
		}
	}
	if (k != 2)
		return false;
// find out which verts are inside other box
	var aidx,bidx;
	var ninside = 0;
	var nainside = 0;
	var nbinside = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(a.pr[i],b)) {
			if (ninside >= 1)
				return false;
			insides[ninside++] = a.pr[i];
			aidx = i;
			++nainside;
		}
	}
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(b.pr[i],a)) {
			if (ninside >= 1)
				return false;
			insides[ninside++] = b.pr[i];
			bidx = i;
			++nbinside;
		}
	}
	if (ninside != 1)
		return false;
// 1 vert inside box, find out closest line to it
	var lns;
	var pt = insides[0]; // the point inside a box
	if (nainside)
		lns = b.pr; // a point from 'a' inside 'b'
	else
		lns = a.pr; // a point from 'b' inside 'a'
	// do some checks
	if (nainside) {
		if (isidx[0].y != isidx[1].y) // is intersection on same line?
			return false;
		i = isidx[0].y; // this is the line
		// check the point for usage in intersections
		var di = isidx[0].x - isidx[1].x;
		var pi;
		if (di < 0)
			di += NRECTPOINTS;
		if (di == 1) {
			pi = isidx[0].x;
		} else if (di == 3) {
			pi = isidx[1].x;
		} else
			return false; // not right line
		if (pi != aidx)
			return false; // not right point
	} else { // nbinside
		if (isidx[0].x != isidx[1].x) // is intersection on same line?
			return false;
		i = isidx[0].x; // this is the line
		// check the point for usage in intersections
		var di = isidx[0].y - isidx[1].y;
		var pi;
		if (di < 0)
			di += NRECTPOINTS;
		if (di == 1) {
			pi = isidx[0].y;
		} else if (di == 3) {
			pi = isidx[1].y;
		} else
			return false; // not right line
		if (pi != bidx)
			return false; // not right point
	}
	j = (i + 1)%NRECTPOINTS;
	collinfo.cn = bp;
	collinfo.penm = util_point2line(pt,lns[i],lns[j],collinfo.cn);
	collinfo.cp = pt;
	if (nbinside) {
		collinfo.cp.x = collinfo.penm*collinfo.cn.x + collinfo.cp.x;
		collinfo.cp.y = collinfo.penm*collinfo.cn.y + collinfo.cp.y;
		collinfo.cn.x = -collinfo.cn.x;
		collinfo.cn.y = -collinfo.cn.y;
	}
	return true;
}
