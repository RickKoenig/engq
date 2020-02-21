// test webgl
var roottree;

var text8 = "WebGL: This state stress tests the matrix functions by displaying 1000 independent model objects using the engine.";

function load8() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
}

function init8() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state8\n");
	
	roottree = new Tree2("root");

	//var tree0 = buildprism("aprism",[1,1,1],null,"flat"); // helper, builds 1 prism returns a Tree2
	//var tree0 = buildprism("aprism",[1,1,1],null,"flat"); // helper, builds 1 prism returns a Tree2
	//tree0.mod.mat.color = [0,1,0,1]; // model color for flat
	var tree0 = buildprism("aprism",[1,1,1],"maptestnck.png","texc"); // helper, builds 1 prism returns a Tree2
	tree0.mod.flags |= modelflagenums.HASALPHA;
	tree0.scale = vec3.fromValues(.5*4/9,.5*9/9,.5/9);

	var i,j,k;
	for (k=0;k<10;++k) {
		for (j=0;j<10;++j) {
			for (i=0;i<10;++i) {
				var cld = tree0.newdup();
				cld.name = "dim" + k + j + i;
				cld.mat.color = [Math.random(),Math.random(),Math.random(),.5]; // tree override for model color for flat
				cld.trans = [2.0*i,2.0*j,2.0*k];
				cld.rotvel = [Math.random()*.5,Math.random()*.5,0];
				if (Math.random() >= .5)
					cld.settexture("panel.jpg"); // override model texture with tree texture
				roottree.linkchild(cld);
			}
		}
	}
	tree0.glfree();

	mainvp.trans = [0,0,-5]; // flycam
	mainvp.rot = [0,0,0]; // flycam
}

function proc8() {
	if (!gl)
		return;
	
    gl.clearColor(0,.5,1,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	doflycam(mainvp); // modify the mvMatrix
	roottree.proc();
	roottree.draw();
}

function exit8() {
	gl_mode(false);
	if (!gl)
		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state8\n");
}
