var mainvp = {
//	mat4.create();
	"trans":[0,0,0],
	"rot":[0,0,0],
	"scale":[1,1,1],
	lookattrans:[0,0,0]
};

var treeinfo = {
	inflycam:0,
	inlookat:0,
	"flycamrevy":true,
	"flycamspeed":.125
};

function setview(vp) {
	if (treeinfo.inlookat && vp.lookattrans) {
/**
 * Generates a look-at matrix with the given eye position, focal point, and up axis
 *
 * @param {mat4} out mat4 frustum matrix will be written into
 * @param {vec3} eye Position of the viewer
 * @param {vec3} center Point the viewer is looking at
 * @param {vec3} up vec3 pointing up
 * @returns {mat4} out
 */
//mat4.lookAt = function (out, eye, center, up) {
		var mla = mat4.create();
		//vp.lookattrans = state19.lightloc;
		mat4.lookAtlhc(mla,vp.trans,vp.lookattrans,[0,1,0]);
		//mat4.invert(mla,mla);
		//vec3.negate(treeglobals.negtrans,vp.trans);
		mt = mat4.create();
		mat4.invert(mt,mla);
		mat4.copy(mvMatrix,mla);
		mat4.copy(v2wMatrix,mt);
		//mat4.translate(mt,mt,vp.trans);
		//mat4.translate(mt,mt,treeglobals.negtrans);
		//mat4.translate(mla,mla,treeglobals.negtrans);
		//mat4.copy(mvMatrix,mla);
		//mat4.mul(mvMatrix,mt,mla);
		//mat4.mul(mvMatrix,mla,mt);
		//mat4.invert(mvMatrix,mvMatrix);
		
		//mat4.identity(mvMatrix);
		//buildtransrotscaleinv(mvMatrix,vp);
		//mat4.identity(v2wMatrix);
		//buildtransrotscale(v2wMatrix,vp); // for env map
	} else {
		mat4.identity(mvMatrix);
		buildtransrotscaleinv(mvMatrix,vp);
		mat4.identity(v2wMatrix);
		buildtransrotscale(v2wMatrix,vp); // for env map
	}
}

function doflycam(vp) {
	var leftright=0,foreback=0,updown=0;
	var mxc,mxr,myc,myr,rcx,rsx,rcy,rsy;
	//if (wininfo.indebprint || wininfo.releasemode)
	//	return;
/*	if (input.key) {
		logger("gotakey\n");
} */
	if (input.key == "c".charCodeAt(0)) {
		treeinfo.inflycam ^= 1;
		input.key = 0; // so other viewports don't get a key..
	}
	if (input.key == "l".charCodeAt(0)) {
		treeinfo.inlookat ^= 1;
		input.key = 0; // so other viewports don't get a key..
	}
	if (input.mx>=0 && input.mx<gl.viewportWidth && input.my>=0 && input.my<gl.viewportHeight) {
		if (input.key == "r".charCodeAt(0)) {
			vp.trans[0] = vp.trans[1] = vp.trans[2] = 0;
			vp.rot[0] = vp.rot[1] = vp.rot[2] = 0;
			input.key = 0;
		}
		if (treeinfo.inflycam) {
			if (input.key == "+".charCodeAt(0) || input.key == "=".charCodeAt(0))
				treeinfo.flycamspeed *= 2.0;
			if (input.key == "-".charCodeAt(0))
				treeinfo.flycamspeed *= .5;
			if (input.keystate[keycodes.right])
				leftright += treeinfo.flycamspeed;
			if (input.keystate[keycodes.left])
				leftright -= treeinfo.flycamspeed;
			if (input.keystate[keycodes.up])
				foreback += treeinfo.flycamspeed;
			if (input.keystate[keycodes.down]) {
				foreback -= treeinfo.flycamspeed;
			}
			if (input.mbut[1] || input.mbut[2])
				updown += treeinfo.flycamspeed;
			if (input.mbut[0])
				updown -= treeinfo.flycamspeed;

			mxc = .5*gl.viewportWidth;
			myc = .5*gl.viewportHeight;
			mxr = 1.0*2*2*Math.PI/gl.viewportWidth;
			myr = .5*2*Math.PI/gl.viewportHeight;
			if (treeinfo.flycamrevy)
				myr = -myr;
			vp.rot[1] = normalangrad((input.mx - mxc)*mxr);
			vp.rot[0] = normalangrad((input.my - myc)*myr);

			rcx = Math.cos(vp.rot[0]);
			rsx = Math.sin(vp.rot[0]);
			rcy = Math.cos(vp.rot[1]);
			rsy = Math.sin(vp.rot[1]);
			vp.trans[0] += leftright*rcy;
			vp.trans[2] -= leftright*rsy;
			vp.trans[0] += foreback*rcx*rsy;
			vp.trans[1] += -foreback*rsx;
			vp.trans[2] += foreback*rcx*rcy;
			vp.trans[0] += updown*rsx*rsy;
			vp.trans[1] += updown*rcx;
			vp.trans[2] += updown*rsx*rcy;
		}
	}
	setview(vp);
}
