var shadowmap = {};

shadowmap.LvMatrix = mat4.create();

shadowmap.lightbias = mat4.clone([
	 .5,  0,  0,  0,
	  0, .5,  0,  0,
	  0,  0, .5,  0,
	 .5, .5, .5,1.0]);

/*
shadowmap.lightbias = mat4.clone([
	  1,  0,  0,  0,
	  0,  1,  0,  0,
	  0,  0,  1,  0,
	  0,  0,  0,  1]);
*/

// assemble a shadowmap render pass 1
shadowmap.beginpass = function() {
	globalmat.LpMatrix = mat4.clone(pMatrix); // copy this over for 2nd render pass
	shadowmap.LvMatrix = mat4.clone(mvMatrix); // copy this over for 2nd render pass
	shadowmap.inshadowmapbuild = true; // models know to use shadowmapbuild shader
};

// disassemble a shadowmap render pass back to normal render target and prepare all matrices for shadowmapuse shader render pass 2
shadowmap.endpass = function () {
	if (shadowmap.inshadowmapbuild) {
		mat4.mul(globalmat.LpMatrix,globalmat.LpMatrix,shadowmap.LvMatrix);
		mat4.mul(globalmat.LpMatrix,globalmat.LpMatrix,v2wMatrix);
		mat4.mul(globalmat.LpMatrix,shadowmap.lightbias,globalmat.LpMatrix);
		shadowmap.inshadowmapbuild = false;
	}
};
