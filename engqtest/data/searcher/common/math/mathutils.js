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

/*
var insides = [new Point2(),new Point2()];
var is = new Array();
is[0] = new Point2();
is[1] = new Point2();
var la0 = new Point2();
var la1 = new Point2();
var lb0 = new Point2();
var lb1 = new Point2();
// return penetration somehow
var bestcp = new Point2();
var bestpendir = new Point2();
function util_plank2plank(a,b) {
	var i,j;
	var k = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		la0.x = a.pr[i].x;
		la0.y = a.pr[i].y;
		la1.x = a.pr[(i + 1)%NRECTPOINTS].x;
		la1.y = a.pr[(i + 1)%NRECTPOINTS].y;
		for (j=0;j<NRECTPOINTS;++j) {
			lb0.x = b.pr[j].x;
			lb0.y = b.pr[j].y;
			lb1.x = b.pr[(j + 1)%NRECTPOINTS].x;
			lb1.y = b.pr[(j + 1)%NRECTPOINTS].y;
			if (getintersection2d(la0,la1,lb0,lb1,is[k])) {
				++k;
				if (k > 2) {
					return false;
				}
			}
		}
	}
	if (k != 2)
		return false;
	bestcp.x = (is[0].x + is[1].x)/2;
	bestcp.y = (is[0].y + is[1].y)/2;
	bestpendir.x = is[1].y - is[0].y;
	bestpendir.y = is[0].x - is[1].x;
	if (!normalize2d(bestpendir))
		return false;
	// reuse la0 
	la0.x = bestcp.x - a.pos.x;
	la0.y = bestcp.y - a.pos.y;
	if (sdot2vv(la0,bestpendir) > 0) {
		bestpendir.x = -bestpendir.x;
		bestpendir.y = -bestpendir.y;
	}
	// find verts inside plank (1 or 2)
	var ninside = 0;
	var nainside = 0;
	var nbinside = 0;
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(a.pr[i],b)) {
			if (ninside >= 2)
				return false;
			insides[ninside++] = a.pr[i];
			++nainside;
		}
	}
	for (i=0;i<NRECTPOINTS;++i) {
		if (util_point2plank(b.pr[i],a)) {
			if (ninside >= 2)
				return false;
			insides[ninside++] = b.pr[i];
			++nbinside;
		}
	}
	if (!ninside)
		return false;
	if (ninside == 1) {
		var d = util_point2line(insides[0],is[0],is[1]);
		penm = d;
		//bestcp = insides[0];
		if (nbinside) {
			bestpendir.x = -bestpendir.x;
			bestpendir.y = -bestpendir.y;
		}
		bestcp.x = insides[0].x + penm*bestpendir.x;
		bestcp.y = insides[0].y + penm*bestpendir.y;
		if (nbinside) {
			bestpendir.x = -bestpendir.x;
			bestpendir.y = -bestpendir.y;
		}
		collinfo.penm = penm;
		collinfo.cn = new Point2(bestpendir.x,bestpendir.y);
		collinfo.cp = new Point2(bestcp.x,bestcp.y);
		return true;
	} else {
		return false;
	}
}
*/
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

function normang(a) {
	if (a >= 1000*2*Math.PI)
		undefined();
	if (a <= -1000*2*Math.PI)
		undefined();
	while (a >= 2*Math.PI)
		a -= 2*Math.PI;
	while (a < 0)
		a += 2*Math.PI;
	return a;
}

// extend glmatrix library
vec3.inv = function(out, a) {
    out[0] = 1.0/a[0];
    out[1] = 1.0/a[1];
    out[2] = 1.0/a[2];
    return out;
};

vec3.equals = function(a,b) {
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
};

// TODO, optimize
mat4.rotateEuler = function(out,a,ypr) {
    mat4.rotateY(out,a,ypr[1]);
    mat4.rotateX(out,out,ypr[0]);
    mat4.rotateZ(out,out,ypr[2]);
    return out;
};

mat4.rotateEulerinv = function(out,a,ypr) {
    mat4.rotateZ(out,a,-ypr[2]);
    mat4.rotateX(out,out,-ypr[0]);
    mat4.rotateY(out,out,-ypr[1]);
    return out;
};

mat4.matrixstack = [];
mat4.nmatrixstack = 0;

mat4.push = function(a) {
	var ms = mat4.matrixstack;
	var nms = mat4.nmatrixstack;
	if (ms.length > nms)
		mat4.copy(ms[nms++],a);
	else
		ms[nms++] = mat4.clone(a);
	mat4.nmatrixstack = nms;
	return a;
};

mat4.pop = function(out) {
	var ms = mat4.matrixstack;
	var nms = mat4.nmatrixstack;
	if (nms <= 0)
		alert("mat4.pop: matrix stack empty!");
	mat4.copy(out,ms[--nms]);
	mat4.nmatrixstack = nms;
	return out;
};

mat4.perspectivelhc = function(out,fovy,aspect,near,far) {
	mat4.perspective(out,fovy,aspect,near,far);
	out[8] = -out[8]; out[9] = -out[9]; out[10] = -out[10]; out[11] = -out[11];
};

mat4.ortholhc = function(out,l,r,b,t,n,f) {
	mat4.ortho(out,l,r,b,t,n,f);
	out[8] = -out[8]; out[9] = -out[9]; out[10] = -out[10]; out[11] = -out[11];
};

/**
 * Generates a look-at matrix with the given eye position, focal point, and up axis
 *
 * @param {mat4} out mat4 frustum matrix will be written into
 * @param {vec3} eye Position of the viewer
 * @param {vec3} center Point the viewer is looking at
 * @param {vec3} up vec3 pointing up
 * @returns {mat4} out
 */
mat4.lookAtlhc = function (out, eye, center, up) {
    var x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
        eyex = eye[0],
        eyey = eye[1],
        eyez = eye[2],
        upx = up[0],
        upy = up[1],
        upz = up[2],
        centerx = center[0],
        centery = center[1],
        centerz = center[2];

    if (Math.abs(eyex - centerx) < GLMAT_EPSILON &&
        Math.abs(eyey - centery) < GLMAT_EPSILON &&
        Math.abs(eyez - centerz) < GLMAT_EPSILON) {
        return mat4.identity(out);
    }

    z0 = -eyex + centerx;
    z1 = -eyey + centery;
    z2 = -eyez + centerz;

    len = 1 / Math.sqrt(z0 * z0 + z1 * z1 + z2 * z2);
    z0 *= len;
    z1 *= len;
    z2 *= len;

    x0 = upy * z2 - upz * z1;
    x1 = upz * z0 - upx * z2;
    x2 = upx * z1 - upy * z0;
    len = Math.sqrt(x0 * x0 + x1 * x1 + x2 * x2);
    if (!len) {
        x0 = 0;
        x1 = 0;
        x2 = 0;
    } else {
        len = 1 / len;
        x0 *= len;
        x1 *= len;
        x2 *= len;
    }

    y0 = z1 * x2 - z2 * x1;
    y1 = z2 * x0 - z0 * x2;
    y2 = z0 * x1 - z1 * x0;

    len = Math.sqrt(y0 * y0 + y1 * y1 + y2 * y2);
    if (!len) {
        y0 = 0;
        y1 = 0;
        y2 = 0;
    } else {
        len = 1 / len;
        y0 *= len;
        y1 *= len;
        y2 *= len;
    }

    out[0] = x0;
    out[1] = y0;
    out[2] = z0;
    out[3] = 0;
    out[4] = x1;
    out[5] = y1;
    out[6] = z1;
    out[7] = 0;
    out[8] = x2;
    out[9] = y2;
    out[10] = z2;
    out[11] = 0;
    out[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
    out[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
    out[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
    out[15] = 1;

    return out;
};
