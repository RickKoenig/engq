// test webgl
var roottree;
var tree0,treef0,treef1,treef2,treef3;
var amod0,amodf0,amodf1,amodf2,amodf3;
var tree7;
var amod1,amod7;

// instructions and info
var text10 = "WebGL: This has 3D fonts and some texture blending shaders.\n" +
			"The 'flycam' interface works in all WebGL states except state 4.\n" +
			"Moving the mouse up and down on the screen effects the blending.";

function load10() {
	if (!gl)
		return;
	preloadimg("pics/smallfont.png");
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/wonMedal.png");
	preloadimg("pics/coin_logo.png");
	preloadimg("pics/panel.jpg");
}

function init10() {
	gl_mode(true);
	if (!gl)
		return;
	logger("entering webgl state10\n");
	
// first build models
// build model 0, test model, uvs and texture, 'tex' shader, test amp phase freq
    // amod0 = new Model("mod0");
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
	    	]
	    };
	    amod0.setmesh(amod0mesh);
	    amod0.settexture("maptestnck.png");
	    amod0.commit();
	}
    
    amod1 = Model.createmodel("mod0");
    amod1.glfree();

// build model 7, multi mat
	var amod7mesh = {
		"verts":[
			-1.0, 1.0,0.0,
          	 1.0, 1.0,0.0,
         	-1.0,-1.0,0.0,
          	 1.0,-1.0,0.0,
          	 
			 2.0, 1.0,0.0,
          	 4.0, 1.0,0.0,
         	 2.0,-1.0,0.0,
          	 4.0,-1.0,0.0,
          	 
			 5.0, 1.0,0.0,
          	 7.0, 1.0,0.0,
         	 5.0,-1.0,0.0,
          	 7.0,-1.0,0.0,
          	 
			 8.0, 1.0,0.0,
          	 10.0, 1.0,0.0,
         	 8.0,-1.0,0.0,
          	 10.0,-1.0,0.0
 		],
   		"uvs":[
	   		0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			1.0,  1.0,
			
	   		0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			1.0,  1.0,
			
	   		0.0,  0.0,
			3.0,  0.0,
			0.0,  3.0,
			3.0,  3.0,
			
	   		0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			1.0,  1.0
	   	],
   		"uvs2":[
	   		0.0,  0.0,
			2.0,  0.0,
			0.0,  2.0,
			2.0,  2.0,
			
	   		0.0,  0.0,
			2.0,  0.0,
			0.0,  2.0,
			2.0,  2.0,
			
	   		0.0,  0.0,
			2.0,  0.0,
			0.0,  2.0,
			2.0,  2.0,
			
	   		0.0,  0.0,
			2.0,  0.0,
			0.0,  2.0,
			2.0,  2.0
	   	],
		"faces":[
	    	0,1,2,
	    	3,2,1,
	    	
	    	4,5,6,
	    	7,6,5,
	    	
	    	8,9,10,
	    	11,10,9,
	    	
	    	12,13,14,
	    	15,14,13
		]
	};
    //amod7 = new Model2("mod7");
    amod7 = Model2.createmodel("mod7");
    if (amod7.refcount == 1) {
	   	amod7.setmesh(amod7mesh);
		amod7.addmat("tex","coin_logo.png",2,4);
		amod7.addmat("basic","wonMedal.png",2,4);
		amod7.addmat2t("blend2","panel.jpg","coin_logo.png",2,4);
		amod7.addmat2t("blend","panel.jpg","coin_logo.png",2,4);
		amod7.commit();
	}
    
// build model f0, test font model0
    amodf0 = new ModelFont("amodf0","smallfont.png",1,1,10,10);
	amodf0.print("Hello");

// build model f1, test font model1
    amodf1 = new ModelFont("amodf1","smallfont.png",1,1,10,10);
	amodf1.print("Hi");

// build model f2, test font model2
    amodf2 = new ModelFont("amodf2","smallfont.png",1,1,10,10);
	amodf2.print("Fee\nFi\nFo\nFum");

// build model f3, test font char wrap model2
    amodf3 = new ModelFont("amodf3","smallfont.png",1,1,4,4,true);
	amodf3.print("FeeFi\nFoFum");

	//modellist = [amod0,amod1,amodf0,amodf1,amodf2,amodf3];

// now build trees	
	roottree = new Tree2("root");

	tree0 = new Tree2("amod0");
	tree0.setmodel(amod0);
	roottree.linkchild(tree0);
	
	treef0 = new Tree2("amodf0");
	treef0.trans = [0,5,0];
	treef0.setmodel(amodf0);
	roottree.linkchild(treef0);
	
	treef1 = new Tree2("amodf1");
	treef1.trans = [-4,4,0];
	treef1.setmodel(amodf1);
	roottree.linkchild(treef1);
	
	treef2 = new Tree2("amodf2");
	treef2.trans = [2,-1,0];
	treef2.setmodel(amodf2);
	roottree.linkchild(treef2);
	
	treef3 = new Tree2("amodf3");
	treef3.trans = [-4,-2,0];
	treef3.setmodel(amodf3);
	roottree.linkchild(treef3);
	
	tree7 = new Tree2("amod7");
	tree7.trans = [-2.0,2.1,0];
	tree7.setmodel(amod7);
	roottree.linkchild(tree7);
	
	mainvp.trans = [0,0,-8]; // flycam
	mainvp.rot = [0,0,0]; // flycam
	//logger_str = "";
}

function proc10() {
	if (!gl)
		return;
	
    amod7.mats[2].blend = input.my/gl.viewportHeight;
    amod7.mats[3].blend = 1 - input.my/gl.viewportHeight;
    gl.clearColor(.25,.5,1,1);                      // Set clear color to blue, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	doflycam(mainvp); // modify the mvMatrix
	roottree.proc();
	amodf1.print("cnt = " + frame);
	roottree.draw();
}

function exit10() {
	gl_mode(false);
	if (!gl)
		return;
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state10\n");
}
