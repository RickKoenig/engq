// test webgl
// do work at model level, instead of tree level
var amod0;
var amod1;
var amod2;
var amod3;
var amod4;
var amod5;
var amod6;
var amod7;
var amod8;
var amod9;

var modellist;
var ang;
//var toruspos = [0,0,0];

var text6 = "WebGL: Load some binary .bwo files that contain multimaterial mesh data.\n" + 
			"These files automatically load shaders and textures as needed.\n" +
			"Mix that with some procedurally generated shapes and some custom shaders.";

function load6() {
	if (!gl)
		return;
	preloadimg("pics/maptestnck.png");
	preloadimg("pics/wonMedal.png");
	preloadimg("pics/coin_logo.png");
	preloadimg("pics/panel.jpg");
	preloadbwo("../tracks2/woodbridge01.bwo");
	preloadbwo("../tracks2/barn.bwo");
	preloadbwo("../tracks2/redbarn.bwo");
	preloadbwo("../fortpoint/fortpointL3.bwo");
}

function init6() {
	gl_mode(true);
	if (!gl)
		return;
	ang = 0;
	//toruspos.x = 0;toruspos.y = 0;toruspos.z = 0;
	logger("entering webgl state6\n");
	
// build model 0, test model, uvs and texture, 'basic' shader, test amp phase freq
    amod0 = new Model("mod0");
    amod0.setshader("basic");
    //amod0.setshader("tex");
    amod0.setverts([
        -1.0,  1.0,  0.0,
         1.0,  1.0,  0.0,
        -1.0, -1.0,  0.0,
         1.0, -1.0,  0.0
    ]);
    amod0.setuvs([
		0.0,  0.0,
		1.0,  0.0,
		0.0,  1.0,
		1.0,  1.0,
    ]);
    amod0.setfaces([
    	0,1,2,
    	3,2,1
    ]);
    amod0.settexture("coin_logo.png");
    //amod0.settexture("coin_logo.png");
   // amod0.settexture("wonMedal.png");
    amod0.commit();
    
// build model 1, straight texture only, and uvs buildpatch sphere
    amod1 = new Model("mod1");
    amod1.setshader("tex");
   	var amod1mesh = buildpatch(40,40,3,3,spheref_surf(2));
	amod1.setmesh(amod1mesh);
	amod1.settexture("maptestnck.png");
    amod1.commit();
    
// build model 2, just a uniform color 'color' shader
	var amod2mesh = {
		"verts":[
			-1.0, 1.0,0.0,
          	 1.0, 1.0,0.0,
         	-1.0,-1.0,0.0,
          	 1.0,-1.0,0.0
		],
		"faces":[
	    	0,1,2,
	    	3,2,1
		]
	};
	var amod2meshnoface = {
		"verts":[
			-1.0, 1.0,0.0,
          	 1.0, 1.0,0.0,
         	-1.0,-1.0,0.0,

          	 1.0,-1.0,0.0,
         	-1.0,-1.0,0.0,
          	 1.0, 1.0,0.0
		]
	};
    amod2 = new Model("mod2");
    amod2.setshader("flat");
/*    amod2.setverts([
         -1.0, 1.0,0.0,
          1.0, 1.0,0.0,
         -1.0,-1.0,0.0,
          1.0,-1.0,0.0
    ]);
    amod2.setfaces([
    	0,1,2,
    	3,2,1
    ]); */
	var doface = false;
	if (doface)
		amod2.setmesh(amod2mesh);
	else
		amod2.setmesh(amod2meshnoface);
	amod2.commit();
	
// build model 3, 'cverts' shader
    amod3 = new Model("mod3");
    amod3.setshader("cvert");
    amod3.setverts([
         -1.0, 1.0,0.0,
          1.0, 1.0,0.0,
         -1.0,-1.0,0.0,
          1.0,-1.0,0.0
    ]);
    amod3.setcverts([
         1.0,0.0,0.0,1.0,
         0.0,1.0,0.0,1.0,
         0.0,0.0,1.0,1.0,
         1.0,1.0,1.0,1.0
    ]);
    amod3.setfaces([
    	0,1,2,
    	3,2,1
    ]);
	amod3.commit();
	
// build model 4, straight texture only, and uvs torus buildpatch
    amod4 = new Model("mod4");
    amod4.setshader("tex");
   	var amod4mesh = buildpatch(40,40,3,3,torusxz_surf(2,1.5));
	amod4.setmesh(amod4mesh);
	amod4.settexture("maptestnck.png");
    amod4.commit();
	//toruspos = [.25, .75, 1.0];
    
// build model 5, test bwo
/*    amod5 = new Model("mod5");
    amod5.setshader("tex");
	//var oio = loadbwomodel("barn.bwo");
	var oio = loadbwomodel("woodbridge01.bwo");
	//amod5.settexture("pics/panel.jpg");
	amod5.setmesh(oio);
	amod5.settexture(oio.mats[0].dtex);
//	amod5.settexture("maptestnck.png");
    amod5.commit(); */
//	amod5 = loadbwomodel("barn.bwo");
	amod5 = loadbwomodel("woodbridge01.bwo");

    
// build model 6, test bwo
/*    amod6 = new Model("mod6");
    amod6.setshader("red");
   	//var amod5mesh = buildpatch(40,40,10,10,torusxzf_surf(1,.5));
	//var oio = loadbwomodel("barn.bwo");
	var oio = loadbwomodel("woodbridge01.bwo");
	//amod5.settexture("panel.jpg");
	amod6.setmesh(oio);
	amod6.settexture(oio.mats[0].dtex);
//	amod6.settexture("maptestnck.png");
    amod6.commit(); */
// amod6 = loadbwomodel("barn.bwo");
   	amod6 = loadbwomodel("fortpointL3.bwo");
	//amod6 = loadbwomodel("woodbridge01.bwo");
   
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
          	 7.0,-1.0,0.0
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
			1.0,  0.0,
			0.0,  1.0,
			1.0,  1.0
			
	   	],
		"faces":[
	    	0,1,2,
	    	3,2,1,
	    	
	    	4,5,6,
	    	7,6,5,
	    	
	    	8,9,10,
	    	11,10,9
		]
	};
	var amod7meshnoface = {
		"verts":[
			-1.0, 1.0,0.0,
          	 1.0, 1.0,0.0,
         	-1.0,-1.0,0.0,
          	 
          	 1.0,-1.0,0.0,
         	-1.0,-1.0,0.0,
          	 1.0, 1.0,0.0,

			 2.0, 1.0,0.0,
          	 4.0, 1.0,0.0,
         	 2.0,-1.0,0.0,
          	 
          	 4.0,-1.0,0.0,
         	 2.0,-1.0,0.0,
          	 4.0, 1.0,0.0,

			 5.0, 1.0,0.0,
          	 7.0, 1.0,0.0,
         	 5.0,-1.0,0.0,

          	 7.0,-1.0,0.0,
         	 5.0,-1.0,0.0,
          	 7.0, 1.0,0.0
 		],
   		"uvs":[
	   		0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			
			1.0,  1.0,
			0.0,  1.0,
			1.0,  0.0,

	   		0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			
			1.0,  1.0,
			0.0,  1.0,
			1.0,  0.0,

	   		0.0,  0.0,
			1.0,  0.0,
			0.0,  1.0,
			
			1.0,  1.0,
			0.0,  1.0,
			1.0,  0.0,
	   	],
	};
    amod7 = new Model2("mod7");
    var doface = false;
    if (doface) {
	   	amod7.setmesh(amod7mesh);
		amod7.addmat("tex","maptestnck.png",2,4);
		amod7.addmat("basic","wonMedal.png",2,4);
		amod7.addmat("tex","panel.jpg",2,4);
   	} else {
   		amod7.setmesh(amod7meshnoface);
		amod7.addmat("tex","maptestnck.png",0,6);
		amod7.addmat("basic","wonMedal.png",0,6);
		amod7.addmat("tex","panel.jpg",0,6);
	}
	amod7.commit();
	
	amod8= loadbwomodel("barn.bwo");
	amod9 = loadbwomodel("redbarn.bwo");

	modellist = [amod0,amod1,amod2,amod3,amod4,amod5,amod6,amod7,amod8,amod9];
	//logger_str = "";

	mainvp.trans = [0,0,0]; // flycam
	mainvp.rot = [0,0,0]; // flycam
	// if not using flycam, do this
	// mat4.identity(mvMatrix);
}

//var once4 = true;
function proc6() {
	if (!gl)
		return;
	doflycam(mainvp); // modify the mvMatrix
	var vMatrix = mat4.clone(mvMatrix);
	
    gl.clearColor(0,.5,1,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	var bright = .5 - .5*Math.sin(ang);

    // 'basic' shader, test amp freq phase
    amod0.mat.bright = 1;
    amod0.mat.amp = input.my/gl.viewportHeight;
    amod0.mat.freq = 5*input.mx/gl.viewportWidth;
    amod0.mat.phase = 0;
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [-.5, .75, 1.0]);
    //mat4.rotateZ(mvMatrix,mvMatrix,ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod0.draw();
    
    // sphere
    amod1.mat.bright = 1;
    amod1.mat.phase = 0;
    amod1.mat.freq = 0;
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [-.5, .75, 1.0]);
    //mat4.rotateX(mvMatrix,mvMatrix,ang);
    //mat4.rotateZ(mvMatrix,mvMatrix,ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod1.draw();
    
    // color shader
    amod2.mat.color = [1,ang/(2*Math.PI),0,1];
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [-.25, .75, 1.0]);
    //mat4.rotateZ(mvMatrix,mvMatrix*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod2.draw();
    
    // cvert shader
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [0, .75, 1.0]);
    //mat4.rotateZ(mvMatrix,mvMatrix,ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod3.draw();
    
    // torus
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [.25, .75, 1.0]);
    //mat4.rotateX(mvMatrix,mvMatrix,ang);
   // mat4.rotateZ(mvMatrix,mvMatrix,4*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod4.draw();
    
    // bwo
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [-.5, 0, 1.0]);
    //mat4.rotateX(mvMatrix,mvMatrix,ang);
    //mat4.rotateZ(mvMatrix,mvMatrix,4*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod5.draw();
    
    // sphere again
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [-.5, -.5, 1.0]);
    //mat4.rotateX(mvMatrix,mvMatrix,5*ang);
    mat4.rotateX(mvMatrix,mvMatrix,ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod1.draw();
    
   // another bwo
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [.5, 0, 1.0]);
    //mat4.rotateY(mvMatrix,mvMatrix,ang);
    //mat4.rotateX(mvMatrix,mvMatrix,5*ang);
    //mat4.rotateZ(mvMatrix,mvMatrix,20*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod6.draw();
    
   // start multi material
    //amod7.mats[0].color = [ang/(2*Math.PI),1,0,1];
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [.5, -1.5, 1.0]);
    //mat4.rotateY(mvMatrix,mvMatrix,ang);
    //mat4.rotateX(mvMatrix,mvMatrix,5*ang);
    //mat4.rotateZ(mvMatrix,mvMatrix,20*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod7.draw();
    
   // another bwo
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [1.5, 0, 1.0]);
    //mat4.rotateY(mvMatrix,mvMatrix,ang);
    //mat4.rotateX(mvMatrix,mvMatrix,5*ang);
    //mat4.rotateZ(mvMatrix,mvMatrix,20*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod8.draw();
    
   // another bwo
    mat4.copy(mvMatrix,vMatrix);
	//mat4.translate(mvMatrix,mvMatrix,toruspos);
    mat4.translate(mvMatrix,mvMatrix, [2.5, 0, 1.0]);
    //mat4.rotateY(mvMatrix,mvMatrix,ang);
    //mat4.rotateX(mvMatrix,mvMatrix,5*ang);
    //mat4.rotateZ(mvMatrix,mvMatrix,20*ang);
    mat4.scale(mvMatrix,mvMatrix,[.1,.1,.1]);
    amod9.draw();
    
	ang += 2*Math.PI / (fpswanted*10);
	if (ang >= 2*Math.PI)
		ang -= 2*Math.PI;
}

function exit6() {
	//alert("bye!");
	gl_mode(false);
	if (!gl)
		return;
	var i;
	logrc();
	logger("after modellist free\n");
	for (i=0;i<modellist.length;++i) {
//		modellist[i].log();
		modellist[i].glfree();
	}
	logrc();
	logger("exiting webgl state6\n");
}
