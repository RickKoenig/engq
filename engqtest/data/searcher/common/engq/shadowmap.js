var shadowmap = {};

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
shadowmap.create = function(shadx,shady) {
	shadowmap.shadowtexture = FrameBufferTexture.createtexture("shadowmap",shadx,shady);
	shadowmap.lmvMatrix = mat4.create();
};

shadowmap.free = function() {
	shadowmap.shadowtexture.glfree();
};

// assemble a shadowmap render pass
shadowmap.beginpass = function() {
	//doflycam(mainvp); // modify the mvMatrix
	mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, 1.0, .1,1000); // setup square light perspective
	//mat4.ortholhc(pMatrix,-30,30,-30,60,.1,1000);
	//pMatrix[8] = -pMatrix[8]; pMatrix[9] = -pMatrix[9]; pMatrix[10] = -pMatrix[10]; pMatrix[11] = -pMatrix[11];
	globalmat.lightpMatrix = mat4.clone(pMatrix); // copy this over for 2nd render pass
	//globalmat.lightmvMatrix = mat4.clone(mvMatrix);
	shadowmap.lmvMatrix = mat4.clone(mvMatrix); // copy this over for 2nd render pass
	//crn("proc20 3",roottree.rot);
	//crn("proc20 4",roottree.rot);
	//atree3.flags |= treeflagenums.DONTDRAW; // don't draw the shadowmap
	//atree1.flags |= treeflagenums.DONTDRAW; // don't draw the background
	//atree1b.flags |= treeflagenums.DONTDRAW; // don't draw the background
	//atree4.flags |= treeflagenums.DONTDRAW; // don't draw the light
	//crn("proc20 5",roottree.rot); 
	shadowmap.inshadowmapbuild = true; // models know to use shadowmapbuild shader
	FrameBufferTexture.useframebuffer(shadowmap.shadowtexture); // render target
    gl.viewport(0, 0, shadowmap.shadowtexture.width,shadowmap.shadowtexture.height); // a square render target
};

// disassemble a shadowmap render pass back to normal render target and prepare all matrices for shadowmapuse shader
shadowmap.endpass = function () {
	mat4.mul(globalmat.lightpMatrix,globalmat.lightpMatrix,shadowmap.lmvMatrix);
	mat4.mul(globalmat.lightpMatrix,globalmat.lightpMatrix,v2wMatrix);
	mat4.mul(globalmat.lightpMatrix,shadowmap.lightbias,globalmat.lightpMatrix);
	//crn("proc20 10",roottree.rot);
	FrameBufferTexture.useframebuffer(null);
	//crn("proc20 11",roottree.rot);
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
	shadowmap.inshadowmapbuild = false;
	mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, .1,1000); // restore main perspective
	//mat4.ortholhc(pMatrix,-30,30,-30,30,.1,1000);
};
