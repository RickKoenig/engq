var text9 = "WebGL: Test render targets.";

var roottree;
var roottree1;
var roottree2; // the one with the off screen render target
var datatex1;
var datatex2;
var datatexd;

var texvp = {
//	mat4.create();
	"trans":[0,0,-5],
	"rot":[0,0,0],
	"scale":[1,1,1],
};


var texdataarr;
if (window.Uint8Array) {
	texdataarr = new Uint8Array([
		255,0,0,255, 255,0,0,255, 0,255,0,255, 0,255,0,255,
		255,0,0,255, 255,0,0,255, 0,255,0,255, 0,255,0,255,
		0,0,255,255, 0,0,255,255, 255,255,255,255, 255,255,255,255,
		0,0,255,255, 0,0,255,255, 255,255,255,255, 255,255,255,255
	]);
}

function load9() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/panel.jpg");
}

function init9() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state9\n");

//// build render target
	//datatex1 = DataTexture.createtexture("redtex",4,4,null);
	datatexd = DataTexture.createtexture("datatex",4,4,texdataarr);
	//createframebuffer("rendertex1",256,256);
	datatex1 = FrameBufferTexture.createtexture("rendertex1",512,512);
	datatex2 = FrameBufferTexture.createtexture("rendertex2",512,512);
	
//// build the off screen scene
	roottree1 = new Tree2("root1");
	roottree1.rotvel = [0,.1,0];
	// a modelpart
	var atree2 = buildsphere("atree2sp",1,"panel.jpg","diffusespecp");
	atree2.trans = [3,0,0];
	atree2.rotvel = [.1,0,0];
	roottree1.linkchild(atree2);
	
	atree2 = buildprism("atree2sq1",[1,1,1],"maptestnck.png","tex");
	atree2.trans = [-3,0,0];
	atree2.rotvel = [.1,0,0];
	roottree1.linkchild(atree2);
	
//// build the off screen scene
	roottree2 = new Tree2("root2");
	roottree2.rotvel = [0,.1,0];
	// a modelpart
	var atree2 = buildsphere("atree2sp",1,"panel.jpg","diffusespecp");
	atree2.trans = [3,0,0];
	atree2.rotvel = [.1,0,0];
	roottree2.linkchild(atree2);
	
	atree2 = buildprism("atree2sq2",[1,1,1],"rendertex1","tex");
	atree2.trans = [-3,0,0];
	atree2.rotvel = [.1,0,0];
	roottree2.linkchild(atree2);
	
//// build the main screen scene
	roottree = new Tree2("root");
	//roottree.rotvel = [0,.1,0];
	
	// a modelpart
	var atree = buildprism("atree2prtd",[1,1,1],"datatex","tex");
	atree.trans = [0,3,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(atree);
	
	atree = buildprism("atree2prt",[1,1,1],"rendertex1","tex");
	atree.trans = [0,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(atree);
	
	atree = buildprism("atree2prt2",[1,1,1],"rendertex2","tex");
	atree.trans = [-3,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(atree);
	
	atree = buildprism("atree2prm",[1,1,1],"maptestnck.png","tex");
	atree.trans = [3,0,0];
	//pendpce0.rotvel = [.1,.5,0];
	//pendpce0.flags |= treeflagenums.ALWAYSFACING;
	roottree.linkchild(atree);
	
//// set the lights
	lights.wlightdir = vec3.fromValues(0,0,1);
	
//// set the camera
	//mainvp.trans = [0,0,-15]; // flycam
	mainvp.trans = [0,0,-5]; // flycam
	mainvp.rot = [0,0,0]; // flycam
}

function proc9() {
	if (!gl)
		return;
	
	roottree2.proc();
	roottree1.proc();
	roottree.proc();
	
	// draw to texture1
//	useframebuffer();
	FrameBufferTexture.useframebuffer(datatex1);
    gl.clearColor(.45,.45,0,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.clear(gl.DEPTH_BUFFER_BIT);
	setview(texvp);
	dolights(); // get some lights to eye space using this view
	roottree1.draw();
	
	// draw to texture2
//	useframebuffer();
	FrameBufferTexture.useframebuffer(datatex2);
    gl.clearColor(.55,.35,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    //gl.clear(gl.DEPTH_BUFFER_BIT);
	setview(texvp);
	dolights(); // get some lights to eye space using this view
	roottree2.draw();
	
	// draw to screen
//	usedefaultbuffer();
	FrameBufferTexture.useframebuffer(null);
	// draw to screen
    gl.clearColor(.25,.25,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    //gl.clear(gl.DEPTH_BUFFER_BIT);
	doflycam(mainvp); // modify the mvMatrix
	//dolights(); // don't need lights right now // get some lights to eye space using this view
	roottree.draw();
}

function exit9() {
	gl_mode(false);
	if (!gl)
		return;
		
	logger("roottree log\n");
	roottree.log();
	logger("roottree1 log\n");
	roottree1.log();
	logger("roottree2 log\n");
	roottree2.log();
	logrc();
	
	logger("after roottree and rootree2 glfree\n");
	roottree.glfree();
	roottree1.glfree();
	roottree2.glfree();
	//datatex1.glfree();
	//freeframebuffer();
	datatexd.glfree();
	datatexd = null;
	datatex1.glfree();
	datatex1 = null;
	datatex2.glfree();
	datatex2 = null;
	logrc();
	roottree = null;
	roottree1 = null;
	roottree2 = null;
	
	logger("exiting webgl state9\n");
}
