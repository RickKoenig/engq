var state10 = {};

// test webgl
var roottree;
var tree0,treef0,treef1,treef2,treef3;
var tree7;
var treetim;

var amod0,amodf0,amodf1a,amodf1b,amodf1c,amodf1d,amodf1big,amodf2,amodf3;
var amod1,amod7;
var amodtim;
var keyteststr;
var keyteststrmaxlength = 32;
var keytest;

var manyfonttest = true; // if true then test the 'many' character render of a font

// instructions and info
state10.text = "WebGL: This has 3D fonts and some texture blending shaders.\n" +
			"The 'flycam' interface works in all WebGL states except state 4.\n" +
			"Moving the mouse up and down on the screen effects the blending.";

state10.title = "3D Fonts";

state10.load = function() {
//	if (!gl)
//		return;
	preloadimg("../common/sptpics/font0.png");
	preloadimg("../common/sptpics/font1.png");
	preloadimg("../common/sptpics/font2.png");
	preloadimg("../common/sptpics/font3.png");
	preloadimg("../common/sptpics/maptestnck.png");
	preloadimg("../common/sptpics/wonMedal.png");
	preloadimg("../common/sptpics/coin_logo.png");
	preloadimg("../common/sptpics/panel.jpg");
	preloadtime(750,false); // test loading screen
};

state10.testvar = {
	s1:31,
	s2:47,
};

state10.init = function() {
	keytest = true;
	keyteststr = "keyteststr";
//	gl_mode(true);
//	if (!gl)
//		return;
	logger("entering webgl state10\n");
	
	frame = 0;
	
// now build trees	
	roottree = new Tree2("root");

if (!manyfonttest) {
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
	tree0 = new Tree2("amod0");
	tree0.setmodel(amod0);
	roottree.linkchild(tree0);
	

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
	tree7 = new Tree2("amod7");
	tree7.trans = [-2.0,2.0,0];
	tree7.setmodel(amod7);
	roottree.linkchild(tree7);
	
    
// build model f0, test font model0
    amodf0 = new ModelFont("amodf0","font1.png","tex",1,1,10,10);
	amodf0.print("Hello");
	treef0 = new Tree2("amodf0");
	treef0.trans = [4,5,0];
	treef0.setmodel(amodf0);
	roottree.linkchild(treef0);
	

// build model f1, test font model1
    amodf1a = new ModelFont("amodf1a","font0.png","tex",1,1,10,10);
	amodf1a.print("Hia");
	treef1 = new Tree2("amodf1a");
	treef1.trans = [-9,8,0];
	treef1.setmodel(amodf1a);
	roottree.linkchild(treef1);
	
    amodf1b = new ModelFont("amodf1b","font1.png","tex",1,1,10,10);
	amodf1b.print("Hib");
	treef1 = new Tree2("amodf1b");
	treef1.trans = [-9,6.9,0];
	treef1.setmodel(amodf1b);
	roottree.linkchild(treef1);
	
    amodf1c = new ModelFont("amodf1c","font2.png","tex",1,1,10,10);
	amodf1c.print("Hic");
	treef1 = new Tree2("amodf1c");
	treef1.trans = [-9,5.8,0];
	treef1.setmodel(amodf1c);
	roottree.linkchild(treef1);
	
    amodf1d = new ModelFont("amodf1d","font3.png","tex",1,1,10,10);
	amodf1d.print("Hid");
	treef1 = new Tree2("amodf1d");
	treef1.trans = [-9,4.7,0];
	treef1.setmodel(amodf1d);
	roottree.linkchild(treef1);
}	
if (manyfonttest) { // test lots of chars
    amodf1big = new ModelFont("amodf1big","font3.png","tex",16,32,100,100,true);
	treef1 = new Tree2("amodf1big");
	var depth = gl.viewportHeight/2;
	treef1.trans = [-depth,-32*4,depth];
	treef1.setmodel(amodf1big);
	roottree.linkchild(treef1);
	
    amodtim = new ModelFont("amodtim","font3.png","tex",.25,.25,100,100,true);
    amodtim.print("hey ho!");
	treef1 = new Tree2("amodtim");
	treef1.trans = [-4,8,0];
	treef1.setmodel(amodtim);
	roottree.linkchild(treef1);
}
if (!manyfonttest) {
// build model f2, test font model2
    amodf2 = new ModelFont("amodf2","font2.png","font2c",1,1,10,10);
    amodf2.mat.fcolor = [1,1,0,1];
    amodf2.mat.bcolor = [0,1,0,.5];
	amodf2.print("Fee\nFi\nFo\nFum");
	treef2 = new Tree2("amodf2");
	treef2.trans = [6,-1,0];
	treef2.setmodel(amodf2);
	roottree.linkchild(treef2);

// build model f3, test font char wrap model2
    amodf3 = new ModelFont("amodf3","font3.png","font2c",1,1,4,4,true);
    amodf3.mat.fcolor = [0,0,.5,1];
    amodf3.mat.bcolor = [1,0,0,.5];
	amodf3.print("FeeFi\nFoFum");
	treef3 = new Tree2("amodf3");
	treef3.trans = [2,-2,0];
	treef3.setmodel(amodf3);
	roottree.linkchild(treef3);
}
	mainvp.trans = [0,0,0]; // flycam
	mainvp.rot = [0,0,0]; // flycam
	//logger_str = "";
	
	var fml = [
//		{name:"state10",key:"testvar",obj:state10},
		"state10.testvar",
	];
	debprint.addlist("testvar from state10",fml);
};

state10.proc = function() {
	
if (!manyfonttest) {
	amod7.mats[2].blend = input.my/gl.viewportHeight;
    amod7.mats[3].blend = 1 - input.my/gl.viewportHeight;

	amodf1a.print("cnt = " + frame);
	amodf1b.print("cnt = " + frame);
	amodf1c.print("cnt = " + frame);
	amodf1d.print("cnt = " + frame);
}
if (manyfonttest) {
/*	var i,j;
	var str = "";
	for (j=0;j<36;++j) {
		for(i=0;i<10;++i) {
			str += frame;
		}
		str += "\n";
	}
	amodf1big.print(str);
*/
    var docircles = false;
    var str = "Test string ready to use.";
    var i,j;
    if (docircles) {
	    for (j=0;j<96;++j) {
		    for (i=0;i<96;++i) {
		    	var x = i - 48;
		    	var y = j - 48;
		    	var r = Math.floor(Math.sqrt(x*x + y*y));
	    		str += String.fromCharCode((r + 96*1000000 - frame)%96 + 32);
	    	}
	    	str += "\n";
	    }
    }
    //var val = "\n".charCodeAt(0); // this
    var val = keycodes.RETURN; // or this
    if (input.key == val)
    	keytest = !keytest;
    if (keytest) {
    	if (input.key == keycodes.BACKSPACE && keyteststr.length > 0)
    		keyteststr = keyteststr.substr(0,keyteststr.length -1 );
    	else if (input.key >= 0x20 && input.key < 0x7f && keyteststr.length < keyteststrmaxlength)
    		keyteststr += String.fromCharCode(input.key);
    	if (!keyteststr.length)
    		keyteststr = "";
    	str = keyteststr;
    	input.key = 0;
    }
    str += "\nstate10.testvar " + JSON.stringify(state10.testvar);
	amodf1big.print(str);
	
	//var str = "	frametimeactual = " + frametimeactual.toFixed(8) + ", proctimeactual = " + proctimeactual.toFixed(8);
	//var str = "" + performance.now();
	//amodtim.print(str);
}
	//roottree.proc();
	doflycam(mainvp);  // modify the trs of the vp
	beginscene(mainvp);
	roottree.draw();
};

state10.exit = function() {
//	gl_mode(false);
//	if (!gl)
//		return;
	debprint.removelist("testvar from state10");
	roottree.log();
	logrc();
	logger("after roottree glfree\n");
	roottree.glfree();
	logrc();
	roottree = null;
	logger("exiting webgl state10\n");
};
