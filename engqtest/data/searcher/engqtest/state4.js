// test webgl
var ang;
var amod0;
var amod1;
var amod2;
var amod3;
var modellist;
//var preloadedbwos = {};

var text4 = "WebGL: First attempt at WebGL.\n" + 
			"Uses shaders, blending, zbuffer, inline images\n" +
			"and matrices.";
	
function load4() {
	if (!gl)
		return;
//	preloadtext("shaders/basic.ps");
//	preloadtext("shaders/basic.vs");
	preloadimg("pics/panel.jpg");
	preloadimg("pics/wonMedal.png");
	preloadimg("pics/coin_logo.png");
	preloadimg("checkmark.png",false,imgdata64);
	preloadbwo("../tracks2/woodbridge01.bwo");
}

var imgdata64 = "data:image/png;base64,iVBORw0KGgoAA" +
"AANSUhEUgAAABAAAAAQAQMAAAAlPW0iAAAABlBMVEUAAAD///+l2Z/dAAAAM0l" +
"EQVR4nGP4/5/h/1+G/58ZDrAz3D/McH8yw83NDDeNGe4Ug9C9zwz3gVLMDA/A6" +
"P9/AFGGFyjOXZtQAAAAAElFTkSuQmCC";
 
function init4() {
	gl_mode(true);
	if (!gl)
		return;
	var plwb = preloadedbin["woodbridge01.bwo"];
	var bwbsize = 0;
	if (plwb)
		bwbsize = plwb.byteLength;
	logger(" (bytesize1 = " + bwbsize + " )\n");
	var bbbsize = 0;
	var pbbb = preloadedbin["testbinbig.bin"];
	if (pbbb)
		bbbsize = pbbb.byteLength;
	logger(" (bytesize2 = " + bbbsize + " )\n");
	logger("entering webgl state4\n");
	ang = 0;
    // initShaders();
    initBuffers();
    inittextures();
    //showbsize = true;
// build model 0
    amod0 = new Model("mod0");
    amod0.setshader("basic");
    amod0.setverts([
         0.0,  1.5,  0.0,
         1.0, -1.0,  0.0,
        -1.0, -1.0,  0.0
    ]);
    amod0.setuvs([
		1.5,  .25,
		.25,  1.75,
		1.75,  1.75,
    ]); 
    amod0.setfaces([
    	0,1,2
    ]);
    amod0.settexture("wonMedal.png");
    amod0.commit();
// build model 1
    amod1 = new Model("mod1");
    amod1.setshader("basic");
    amod1.setverts([
         -1.0, 1.0,0.0,
          1.0, 1.0,0.0,
         -1.0,-1.0,0.0,
          1.0,-1.0,0.0
    ]);
    amod1.setuvs([
		.375,.375,
		1.625,.375,
		.375,1.625,
		1.625,1.625,
    ]); 
    amod1.setfaces([
    	0,1,2,
    	3,2,1
    	//1,3,2
    ]);
    amod1.settexture("coin_logo.png");
    amod1.commit();
// build model 2
    amod2 = new Model("mod2");
    amod2.setshader("basic");
    amod2.setverts([
         -1.0, 1.0,0.0,
          1.0, 1.0,0.0,
         -1.0,-1.0,0.0,
          1.0,-1.0,0.0
    ]);
    amod2.setuvs([
		.375,.375,
		1.625,.375,
		.375,1.625,
		1.625,1.625,
    ]); 
    amod2.setfaces([
    	0,1,2,
    	3,2,1
    	//1,3,2
    ]);
    amod2.settexture("checkmark.png");
    amod2.commit(); 
// build model 2
    amod3 = new Model("mod3");
    amod3.setshader("red");
    amod3.setverts([
         -1.0, 1.0,0.0,
          1.0, 1.0,0.0,
         -1.0,-1.0,0.0,
          1.0,-1.0,0.0
    ]);
    amod3.setfaces([
    	0,1,2,
    	3,2,1
     ]);
    amod3.commit(); 
    modellist = [amod0,amod1,amod2,amod3];
	//logger_str = "";
}

//var once4 = true;
function proc4() {
	if (!gl)
		return;
    gl.clearColor(.7,.7,0,1);                      // Set clear color to yellow, fully opaque
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	drawScene(ang);
	
	//var shaderProgram;
	//shaderProgram = shaderPrograms["red"];
    //gl.useProgram(shaderProgram);
	//shaderProgram = shaderPrograms["basic"];
    //gl.useProgram(shaderProgram);
 /*   if (once4) { */
		//setAttributes(shaderProgram);
		//setSamplerUniforms(shaderProgram);
	    //setMatrixPersUniforms(shaderProgram);
	    //once4 = false;
/*	} */
	//setUserUniforms(shaderProgram);
    
    amod0.mat.bright = 1;
    amod0.mat.phase = 0;
    mat4.identity(mvMatrix);
    mat4.translate(mvMatrix,mvMatrix, [0.0, 0.0, 4.0]);
    mat4.rotateY(mvMatrix,mvMatrix,4*ang);
//    setMatrixModelViewUniforms(amod0.shader);
    amod0.draw();
    
    //amod1.mat.bright = bright;
    //amod1.mat.phase = phase/3;
    mat4.identity(mvMatrix);
    mat4.translate(mvMatrix,mvMatrix, [.75, 0.0, 4.0]);
    mat4.rotateY(mvMatrix,mvMatrix,5*ang);
 //   setMatrixModelViewUniforms(amod1.shader);
    amod1.draw();
    
    amod2.mat.bright = 0;
    amod2.mat.phase = phase/3;
    amod2.mat.hum = phase/3;
    
    var i,n=10;
    var nmod = modellist.length;
    for (i=0;i<n;++i) {
	    mat4.identity(mvMatrix);
	    mat4.translate(mvMatrix,mvMatrix, [-7.75 + i*1.8, -4.0, 9.0]);
	    mat4.rotateY(mvMatrix,mvMatrix,ang);
	//    setMatrixModelViewUniforms(amod2.shader);
	    var midx = i % nmod;
	    var mod = modellist[midx];
	    mod.draw(); 
    }

    for (i=0;i<n;++i) {
	    mat4.identity(mvMatrix);
	    mat4.translate(mvMatrix,mvMatrix, [-7.75 + i*1.8, -7.0, 9.0]);
	    mat4.rotateY(mvMatrix,mvMatrix,ang);
	//    setMatrixModelViewUniforms(amod2.shader);
	    var midx = i % nmod;
	    var mod = modellist[midx];
	    mod.draw(); 
    }

	ang += 2*Math.PI / 640;
	if (ang >= 2*Math.PI)
		ang -= 2*Math.PI;
}

function exit4() {
	//alert("bye!");
	gl_mode(false);
	if (!gl)
		return;
	var i;
	logrc();
	logger("after modellist free\n");
	for (i=0;i<modellist.length;++i)
		modellist[i].glfree();
	logrc();
    // exitShaders();
    exitBuffers();
    exittextures();
	logger_str += "exiting webgl state4\n";
}
