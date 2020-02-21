var text20 = "WebGL: Shadow Mapping.";

var shadowtexture;

var atree1,atree1b,atree2,atree3,atree4,atree5,atree6,atree7;
var frm;

var shadowvp = {
//	mat4.create();
	"trans":[0,20,-20],
	"rot":[.78,0,0],
	"scale":[1,1,1],
};

function load20() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
	preloadimg("pics/wonMedal.png");
}

function init20() {
	gl_mode(true);
	if (!gl)
		return;
	frm = 0;
	logger("entering webgl state20\n");
	
 
//// build render target
	//shadowtexture = FrameBufferTexture.createtexture("shadowmap",1024,1024);
	shadowmap.create(1024,1024); // prepare for shadowmapping
	
//// build the main screen scene
	roottree = new Tree2("root");
	//roottree.rotvel = [0,.1,0];
	roottree.flags |= treeflagenums.DONTDRAW; // testing
	
	// a modelpart
	atree1 = buildplanexy2t("planexy1",20,20,"maptestnck.png","shadowmap","shadowmapuse"); // tex
	atree1.trans = [0,0,20];
	//atree1.rot = [.3,.4,0];
	//atree1.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	atree1.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree1);
	
	atree1b = buildplanexz2t("planexz1",20,20,"maptestnck.png","shadowmap","shadowmapuse"); // tex
	atree1b.trans = [0,-20,0];
	//atree1.rot = [.3,.4,0];
	//atree1.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	atree1b.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree1b);
	
	var atree2p = new Tree2("par");
	atree2p.trans = [0,0,-4];
	atree2p.rotvel = [0,0,.1];
	atree2 = buildplanexy("planexy2",.5,.5,"wonMedal.png","tex");
	var i,j;
	for (j=0;j<7;++j) {
		for (i=0;i<7;++i) {
			var nt = atree2.newdup();
			nt.trans = [2*(i-3),2*(j-3),-1];
			atree2p.linkchild(nt);
		}
	}
	atree2.glfree();
	roottree.linkchild(atree2p);
/*	atree2 = buildplanexy("planexy2",5,5,"wonMedal.png","tex");
	atree2.trans = [7,-7,-1];
	atree2.rotvel = [0,0,-.1];
	roottree.linkchild(atree2); */
	
	atree3 = buildplanexy("planexy3",2,2,"shadowmap","shadowmapshow"); // invert framebuffer renders, sigh
	atree3.trans = [7,0,-1];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	atree3.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree3);
	
	atree4 = buildsphere("sph4",.2,null,"flat"); // this is where the light is
	atree4.mod.mat.color = [1,1,0,1];
	atree4.trans = [0,20,-20];
	atree4.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree4);
	
	atree5 = buildsphere("sph5",.2,"wonMedal.png","tex");
	atree5.trans = [0,-.875,-8];
	atree5.scale = [8,8,8];
	roottree.linkchild(atree5);

	atree6 = atree5.newdup();
	atree6.trans = [-.75,0,-8];
	atree6.scale = [8,8,8];
	atree6.rotvel = [.4,.25,0];
	roottree.linkchild(atree6);

	atree7 = atree5.newdup();
	atree7.trans = [-.75,-1.5,-8];
	atree7.scale = [12,12,12];
	roottree.linkchild(atree7);

	var ras = new Tree2("cylr");
	ras.rotvel = [.3,0,0];
	roottree.linkchild(ras);
	
	var as = buildcylinderxz2t("cyl6",.4,2.5,"panel.jpg","shadowmap","shadowmapuse");
	//var as = buildsphere("sph6",.2,"panel.jpg","tex");
	as.scale = [2,2,2];
	as.rotvel = [0,0,.2];
	as.trans = [0,0,5];
	ras.linkchild(as);

	as = buildcylinderxz2t("cyl6",.4,2.5,"panel.jpg","shadowmap","shadowmapuse");
	//var as = buildsphere("sph6",.2,"panel.jpg","tex");
	as.scale = [2,2,2];
	as.rotvel = [0,0,.3];
	as.trans = [0,0,-5];
	ras.linkchild(as);

	as = buildcylinderxz("cyl7",.4,1.5,"panel.jpg","tex");
	as.scale = [5,5,5];
	as.trans = [-5,-20,16];
	roottree.linkchild(as);

	as = buildcylinderxz("cyl7",.4,1.5,"panel.jpg","tex");
	as.scale = [2,2,2];
	as.trans = [-1,13,-14];
	as.rotvel = [.4,.8,0];
	roottree.linkchild(as);


/*
	var atree4 = buildplanexy("planexy4",2,2,"maptestnck.png","tex"); // invert framebuffer renders, sigh
	atree4.trans = [-4,0,-1];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(atree4);
*/
	
//// set the lights
	lights.wlightdir = vec3.fromValues(0,-.7071,.7071);
	
//// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,20,-20]; // flycam
	mainvp.rot = [.78,0,0]; // flycam
	// build the scene
}

var lightbias = mat4.clone([
	 .5,  0,  0,  0,
	  0, .5,  0,  0,
	  0,  0, .5,  0,
	 .5, .5, .5,1.0]);

/*var lightbias = mat4.clone([
	  1,  0,  0,  0,
	  0,  1,  0,  0,
	  0,  0,  1,  0,
	  0,  0,  0,  1]);
*/
function proc20() {
	if (!gl)
		return;
	//crn("proc20 1",roottree.rot);
	atree7.trans[2] = 10 + 15*Math.sin(2*frm);
	atree6.trans[2] = 10 + 15*Math.sin(3*frm);
	atree5.trans[2] = 10 + 15*Math.sin(5*frm);
	//crn("proc20 2",roottree.rot);
	
		roottree.proc();
	// draw to shadowmap
	setview(shadowvp);
	shadowmap.beginpass();
/*	//doflycam(mainvp); // modify the mvMatrix
	mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, 1.0, .1,1000);
	//pMatrix[8] = -pMatrix[8]; pMatrix[9] = -pMatrix[9]; pMatrix[10] = -pMatrix[10]; pMatrix[11] = -pMatrix[11];
	globalmat.lightpMatrix = mat4.clone(pMatrix);
	//globalmat.lightmvMatrix = mat4.clone(mvMatrix);
	var lmvMatrix = mat4.clone(mvMatrix);
	//crn("proc20 3",roottree.rot);
	//crn("proc20 4",roottree.rot);
	atree3.flags |= treeflagenums.DONTDRAW; // don't draw the shadowmap
	atree1.flags |= treeflagenums.DONTDRAW; // don't draw the background
	atree1b.flags |= treeflagenums.DONTDRAW; // don't draw the background
	atree4.flags |= treeflagenums.DONTDRAW; // don't draw the light
	//crn("proc20 5",roottree.rot); 
	inshadowmapbuild = true;
	FrameBufferTexture.useframebuffer(shadowmap.shadowtexture);
    gl.viewport(0, 0, shadowmap.shadowtexture.width,shadowmap.shadowtexture.height); */
    //gl.clearColor(.45,.45,0,1);                      // Set clear color to yellow, fully opaque
    gl.clearColor(1,1,1,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	//globalmat.viewportxy = [shadowtexture.width,shadowtexture.height];
	//var i;
	//for (i=0;i<10;++i)
    //gl.cullFace(gl.FRONT);
	//crn("proc20 6",roottree.rot);
	roottree.draw();
	//crn("proc20 7",roottree.rot);
    //gl.cullFace(gl.BACK);

	doflycam(mainvp); // modify the mvMatrix
//	setview(mainvp); // modify the mvMatrix
	shadowmap.endpass(); // get out of lightmap rendering, setup lightmap matrices, call after setting mvMatrix
    // draw main screen
	//mat4.mul(globalmat.lightpMatrix,globalmat.lightpMatrix,lightbias);
	//crn("proc20 8",roottree.rot);
	//crn("proc20 9",roottree.rot);
/*	mat4.mul(globalmat.lightpMatrix,globalmat.lightpMatrix,shadowmap.lmvMatrix);
	mat4.mul(globalmat.lightpMatrix,globalmat.lightpMatrix,v2wMatrix);
	mat4.mul(globalmat.lightpMatrix,lightbias,globalmat.lightpMatrix);
	//crn("proc20 10",roottree.rot);
	FrameBufferTexture.useframebuffer(null);
	//crn("proc20 11",roottree.rot);
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight); */
    gl.clearColor(.25,.25,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	//mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, .1,1000);
	//pMatrix[8] = -pMatrix[8]; pMatrix[9] = -pMatrix[9]; pMatrix[10] = -pMatrix[10]; pMatrix[11] = -pMatrix[11];
	//crn("proc20 12",roottree.rot);
	//shadowmap.inshadowmapbuild = false;
	//dolights(); // get some lights to eye space
//	atree3.flags &= ~treeflagenums.DONTDRAW; // draw the shadowmap
//	atree1.flags &= ~treeflagenums.DONTDRAW; // draw the background
//	atree1b.flags &= ~treeflagenums.DONTDRAW; // draw the background
//	atree4.flags &= ~treeflagenums.DONTDRAW; // draw the light
	//crn("proc20 13",roottree.rot);
	
	dolights(); // get some lights from world space to eye space using this view
	roottree.draw();
	//crn("proc20 14",roottree.rot);

	frm += .002;
	if (frm >= 2*Math.PI)
		frm -= 2*Math.PI;
	//crn("proc20 15",roottree.rot);
}

function exit20() {
	gl_mode(false);
	if (!gl)
		return;
	// restore default perspective
	mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, 0.002,10000.0);
//	mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, .001,4000.0);
	//pMatrix[8] = -pMatrix[8]; pMatrix[9] = -pMatrix[9]; pMatrix[10] = -pMatrix[10]; pMatrix[11] = -pMatrix[11];
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	shadowmap.free();
	//shadowtexture.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state20\n");
}
