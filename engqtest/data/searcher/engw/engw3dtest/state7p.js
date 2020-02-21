var state7p = {};

// test webgl
//var amod0,amod1,amod2;
//var modellist;
var roottree;

state7p.text = "WebGL: Load a .bws file.  Prehistoric\n" +
			"This may take awhile to load.\n" +
			"Press 'C' then hold down 'up arrow' to begin your adventure!";

state7p.title = "Prehistoric";

state7p.load = function() {
	//if (!gl)
	//	return;
	preloadimg("../common/sptpics/maptestnck.png");
	//preloadimg("../common/sptpics/boohaa.jpg");
	//preloadbws("fortpoint/fp7opt.BWS");
	preloadbws("prehistoric/prehistoric.BWS");
};

state7p.init = function() {
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state7p\n");
	
// build model 0, test model, uvs and texture, 'tex' shader, test amp phase freq
    //var amod0 = new Model("mod0");
    amod0 = Model.createmodel("mod0");
    if (amod0.refcount == 1) {
    	amod0.setshader("tex");
	    var amod0mesh = {
	    	"verts":[
	        -1.0,  1.0,  0.0,
	         1.0,  1.0,  0.0,
	        -1.0, -1.0,  0.0,
	         1.0, -1.0,  0.0
	    ],
	    "uvs":[
			0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			1.0,  1.0,
	    ],
	    "faces":[
	    	0,1,2,
	    	3,2,1
	    ]};
	    amod0.setmesh(amod0mesh);
	    amod0.settexture("maptestnck.png");
	    amod0.commit();
	}
//	logger_str = "";
	//modellist = [amod0];
	
	roottree = new Tree2("root");

	
//	mainvp.trans = [-948.4,459.9,204.1]; // flycam
	mainvp.trans = [0,.3,.1]; // flycam
//	mainvp.rot = [.069,1.69,0]; // flycam
	mainvp.rot = [Math.PI/8,Math.PI,0];
	
	//var bwstree = new Tree2("fp7opt.BWS");
	var bwstree = new Tree2("prehistoric.BWS");
	roottree.linkchild(bwstree);
	var lt = new Tree2("dirlight");
	lt.rot = [Math.PI/4,0,0];
	lt.rotvel = [0,1,0];
	lt.flags |= treeflagenums.DIRLIGHT;
	addlight(lt);
	roottree.linkchild(lt);
	
	var resat = roottree.findtree("Trex_body2.bwo");
	//var resat = roottree.findtree("PTbody.bwo");
	//var resat = roottree.findtree("Trex_torso.bwo");
	//var resat = roottree.findtree("PTjaw.bwo");
	mainvp.camattach = resat;
	mainvp.incamattach = true;
	
	var reslk = roottree.findtree("PTER_loco.bwo");
	//var reslk = roottree.findtree("Trex_body2.bwo");
	mainvp.lookat = reslk;
	//mainvp.inlookat = true;
	
};

//var once4 = true;
state7p.proc = function() {
	//if (!gl)
	//	return;
	doflycam(mainvp); // modify the trs of vp
	
    //gl.clearColor(.5,0,1,1);                      // Set clear color to yellow, fully opaque
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

/*    mat4.copy(mvMatrix,vMatrix);
    var dist = 3;
    mat4.translate(mvMatrix,mvMatrix,[0,0,dist]);
    mat4.scale(mvMatrix,mvMatrix,[dist,dist,dist]);
   amod0.draw();
*/
	roottree.proc();
	beginscene(mainvp);

	roottree.draw();
};

state7p.exit = function() {
	//alert("bye!");
//	gl_mode(false);
//	if (!gl)
//		return;
/*	var i;
	for (i=0;i<modellist.length;++i)
		modellist[i].glfree(); */
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	mainvp.camattach = null;
	mainvp.incamattach = false;
	mainvp.lookat = null;
	mainvp.inlookat = false;
	logger("exiting webgl state7p\n");
};
