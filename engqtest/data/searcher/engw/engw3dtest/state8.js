var state8 = {};

// test webgl
state8.roottree = null;

state8.text = "WebGL: This state stress tests the matrix functions by displaying 2000 independent model objects using the engine.";

state8.title = "Many objects";

state8.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/panel.jpg");
};


state8.init = function() {
	logger("entering webgl state8\n");
	
	state8.roottree = new Tree2("root");

	state8.tree0 = buildprism("aprism",[.5,.5,.5],"maptestnck.png","texc"); // helper, builds 1 prism returns a Tree2
	state8.tree0.mod.flags |= modelflagenums.HASALPHA;
	//state8.tree0.scale = vec3.fromValues(.5*4/9,.5*9/9,.5/9);
	var tree1 = buildprism("aprism2",[.25,.25,.25],"maptestnck.png","tex"); // helper, builds 1 prism returns a Tree2
	tree1.trans = [2,0,0];
	tree1.rotvel = [0,1,0];
	state8.tree0.linkchild(tree1);

	var i,j,k;
	for (k=0;k<10;++k) {
		for (j=0;j<10;++j) {
			for (i=0;i<10;++i) {
				var cld = state8.tree0.newdup();
				cld.name = "dim" + k + j + i;
				cld.mat.color = [Math.random(),Math.random(),Math.random(),.5]; // tree override for model color for flat
				cld.trans = [2.0*i,2.0*j,2.0*k];
				cld.rotvel = [Math.random()*.5,Math.random()*.5,0];
				if (Math.random() >= .5)
					cld.settexture("panel.jpg"); // override model texture with tree texture
				state8.roottree.linkchild(cld);
			}
		}
	}
	//state8.tree0.glfree();
	state8.tree0.qrot = [0,.7071,0,.7071];
//	state8.tree0.qrot = [0,0,0,1];
/*	state8.tree0.o2pmat4 = [
//		0,.866,.5,0,
//		0,-.5,.866,0,
//		1,0,0,0,
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		-4,0,0,1,
	];
	tree1.o2pmat4 = [
//		.5,0,.866,0,
//		0,1,0,0,
//		-.866,0,.5,0,
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
		-4,0,0,1,
	];
*/
	tree1.qrot = [.866,0,0,.5];
	state8.tree0.trans = [-4,0,0];
	
	tree1.trans = [-4,0,0];

	state8.tree0.mat.color=[1,0,0,1];
	state8.roottree.linkchild(state8.tree0);
	
	//state8.roottree.rotvel = [0,1,0];

	mainvp.trans = [0,0,-5]; // flycam
	mainvp.rot = [0,0,0]; // flycam
//	mainvp.camattach = state8.tree0;
//	mainvp.camattach = tree1;
//	mainvp.camattach = state8.roottree.children[3];
	mainvp.camattach = state8.roottree.children[3].children[0];
	mainvp.incamattach = true;
	mainvp.lookat = state8.roottree.children[4].children[0];
	debprint.addlist( "state8",[
		"state8.tree0.trans",
		"state8.tree0.mat",
	] );
};

state8.proc = function() {
	//if (!gl)
	//	return;
	

	state8.roottree.proc();
	doflycam(mainvp); // modify the trs of vp
	
	beginscene(mainvp);
    //gl.clearColor(0,.5,1,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	state8.roottree.draw();
};

state8.exit = function() {
	debprint.removelist("state8");
//	gl_mode(false);
//	if (!gl)
//		return;
	state8.roottree.log();
	logrc();
	logger("after roottree glfree\n");
	state8.roottree.glfree();
	logrc();
	state8.roottree = null;
	logger("exiting webgl state8\n");
	mainvp.camattach = null;
	mainvp.incamattach = false;
	mainvp.lookat = null;
	mainvp.inlookat = false;
};
