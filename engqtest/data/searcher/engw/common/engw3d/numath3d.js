var vectormasterdir = [0,1,0]; // model vector points up
var vectorcross = vec3.create();
var vectornorm = vec3.create();
//var epsilon = .6;
var epsilon = GLMAT_EPSILON;
// return a quat for rotating a vector pointing up i.e. (0,1,0) to a vector pointing in dir (not need to be normalized)
function dir2quat(dir) {
	var ret = quat.create();
	vec3.normalize(vectornorm,dir);
	vec3.cross(vectorcross,vectormasterdir,vectornorm);
	if (vec3.sqrLen(vectorcross) < epsilon*epsilon) {
		if (dir[1] >= 0)
			quat.set(ret,0,0,0,1);
		else
			quat.set(ret,1,0,0,0); // some quat at 180
	} else {
		vec3.normalize(vectorcross,vectorcross);
		var d = vec3.dot(vectormasterdir,vectornorm);
		var ang = Math.acos(d);
		quat.setAxisAngle(ret,vectorcross,ang);
	}
	return ret;
}

// return Euler angles for rotating a vector pointing up i.e. (0,1,0) to a vector pointing in dir (not need to be normalized)
function dir2rot(dir) {
	var ret = vec3.create();
	var len = vec3.length(dir);
	var lenxz = Math.sqrt(dir[0]*dir[0] + dir[2]*dir[2]);
	if (lenxz < epsilon*len) {
		if (dir[1] >= 0)
			vec3.set(ret,0,0,0);
		else
			vec3.set(ret,Math.PI,0,0);
	} else {
		ret[0] = Math.atan2(lenxz,dir[1]);
		ret[1] = Math.atan2(dir[0],dir[2]);
	}
	return ret;
}





//// extend glmatrix library
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

mat4.perspectivezf = function (out, zf, aspect, near, far) {
    var f = zf;
        nf = 1 / (near - far);
    out[0] = f / aspect;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = f;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = (far + near) * nf;
    out[11] = -1;
    out[12] = 0;
    out[13] = 0;
    out[14] = (2 * far * near) * nf;
    out[15] = 0;
    return out;
};

mat4.perspectivelhc = function(out,fovy,aspect,near,far) {
	mat4.perspective(out,fovy,aspect,near,far);
	out[8] = -out[8]; out[9] = -out[9]; out[10] = -out[10]; out[11] = -out[11];
};

mat4.perspectivelhczf = function(out,zf,aspect,near,far) {
	mat4.perspectivezf(out,zf,aspect,near,far);
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

    if (Math.abs(eyex - centerx) < epsilon &&
        Math.abs(eyey - centery) < epsilon &&
        Math.abs(eyez - centerz) < epsilon) {
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

mat4.det = mat4.determinant;

//quat.setAxisAngle = function(out, axis, rad) {
	
// reverse of setAxisAngle, sets axis in out[0],out[1],out[2] and angle in out[3]
quat.getAxisAngle = function(out, q) {
	var w2 = q[3]*q[3];
	if (w2 > 1.0)
		w2 = 1.0;
	var sina = Math.sqrt(1-w2);
	if (sina > epsilon) {
		out[3] = 2.0*Math.acos(q[3]);
		if (out[3] < 0)
			sina = -sina;
		sina = 1.0/sina;
		out[0] = q[0]*sina;
		out[1] = q[1]*sina;
		out[2] = q[2]*sina;
		if (out[3] >= Math.PI)
			out[3] -= 2.0*Math.PI;
		if (out[3] < 0) {
			out[0] = -out[0];
			out[1] = -out[1];
			out[2] = -out[2];
			out[3] = -out[3];
		}
	} else {
		out[0] = 0;
		out[1] = 1;
		out[2] = 0;
		out[3] = 0;
	}
};


quat.slerp2 = function (c, a, b, t) {
	var iab = quat.create();
	quat.invert(iab,a);
	quat.mul(iab,iab,b);
	var ra = vec4.create();
	quat.getAxisAngle(ra,iab);
	ra[3] *= t;
	quat.setAxisAngle(iab,ra,ra[3]);
	quat.mul(c,a,iab);
	quat.normalize(c,c);
};

