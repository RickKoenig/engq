var state20 = {};

state20.text = "WebGL: Shadow Mapping.";

state20.title = "Shadow mapping";

var shadowtexture;

var atree1,atree1b,atree2,atree3,atree4,atree5,atree6,atree7;
var frm;

state20.lightdist = 20;
state20.lightloc = [0,state20.lightdist,-state20.lightdist];

state20.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
	preloadimg("../common/sptpics/wonMedal.png");
};

state20.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
		
	frm = 0;
	
	logger("entering webgl state20\n");
	
// build render target
	var shadowmapres = 512;
	shadowtexture = FrameBufferTexture.createtexture("shadowmap",shadowmapres,shadowmapres);

// shadow viewport
	state20.shadowvp = {
		target:shadowtexture,
	   	clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
		clearcolor:[1,1,1,1],                    // Set clear color to yellow, fully opaque
	//	mat4.create();
		"trans":vec3.clone(state20.lightloc),
		"rot":[Math.PI/4,0,0], // part of lightdir
		//"scale":[1,1,1],
	   	near:.1,
	   	far:10000.0,
	   	zoom:1,
		asp:1,
		isshadowmap:true
	};

// main viewport for state 20
	state20.mvp = {
	   	clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
		clearcolor:[.15,.25,.75,1],                    // Set clear color to yellow, fully opaque
	//	trans:[-21,-3,3],
		"trans":vec3.clone(state20.lightloc),
		"rot":[Math.PI/4,0,0], // part of lightdir
		//"scale":[1,1,1],
	   	near:.1,
	   	far:10000.0,
	   	zoom:1,
		asp:gl.asp,
	};

// build the  scene
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
	
	// lots of medals
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
	
	// shadow map viewer
	atree3 = buildplanexy("planexy3",2,2,"shadowmap","shadowmapshow"); // invert framebuffer renders, sigh
	atree3.trans = [7,0,-1];
	atree3.flags |= treeflagenums.DONTCASTSHADOW;
	roottree.linkchild(atree3);
	
	// some spheres
	
	// the light sphere
	atree4 = buildsphere("sph4",.2,null,"flat"); // this is where the light is
	atree4.mod.mat.color = [1,1,0,1];
	atree4.trans = [0,20,-20];
	atree4.flags |= treeflagenums.DONTCASTSHADOW;
	atree4.flags |= treeflagenums.DIRLIGHT;
	atree4.rot = [Math.PI/4,0,0];
	addlight(atree4);
	roottree.linkchild(atree4);
	
	// three moving spheres
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

	// four rotating cylinders
	var ras = new Tree2("cylr"); // 2 cyl linked together
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

	// two more linked to root
	as = buildcylinderxz("cyl7",.4,1.5,"panel.jpg","tex");
	as.scale = [5,5,5];
	as.trans = [-5,-20,16];
	roottree.linkchild(as);

	as = buildcylinderxz("cyl7",.4,1.5,"panel.jpg","tex");
	as.scale = [2,2,2];
	as.trans = [-1,13,-14];
	as.rotvel = [.4,.8,0];
	roottree.linkchild(as);

//// set the lights, make this match the shadowvp
	//lights.wlightdir = vec3.fromValues(0,-.7071,.7071);
};

state20.proc = function() {
//	if (!gl)
//		return;
	// move some spheres around
	atree7.trans[2] = 10 + 15*Math.sin(2*frm);
	atree6.trans[2] = 10 + 15*Math.sin(3*frm);
	atree5.trans[2] = 10 + 15*Math.sin(5*frm);
	roottree.proc();
	doflycam(state20.mvp); // modify the trs of the vp
	
	// draw to shadowmap
	beginscene(state20.shadowvp);
	roottree.draw();

	// draw main scene
	beginscene(state20.mvp);	
	roottree.draw();

	// update frame counter
	frm += .002;
	if (frm >= 2*Math.PI)
		frm -= 2*Math.PI;
};

state20.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	shadowtexture.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state20\n");
};
