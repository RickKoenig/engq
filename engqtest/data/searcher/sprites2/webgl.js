var gl; // the main interface
var gledrawarea; // canvas to make webgl
var d2edrawarea; // canvas to make webgl

// list of all shaders
var shaderlist;
/* = [
	"red",
	"basic",
	"flat",
	"cvert",
	"tex"
]; */

var glenums = {
	"GL_FLOAT":5126,
	"GL_FLOAT_VEC2":35664,
	"GL_FLOAT_VEC3":35665,
	"GL_FLOAT_VEC4":35666,
	"GL_FLOAT_MAT4":35676
	//"GL_SAMPLER_2D":35678
};

var globalmat = {
	hi:3,
	"ho":5,
	alphacutoff:.03125,
	specpow:500,
	//alphacutoff:.0625
	//alphacutoff:.125
	//alphacutoff:.25
	//alphacutoff:.5
	//alphacutoff:.75
};

function gl_init() {
	logger_str += " gl_init\n";	
	gl = null;
	gledrawarea = document.getElementById('mycanvas2');
	try {
		// Try to grab the standard context. If it fails, fallback to experimental.
		gl = gledrawarea.getContext("webgl");
		if (!gl)
			gl = gledrawarea.getContext("experimental-webgl");
		//gl = edrawarea.getContext("experimental-webgl");
	}
	catch(e) {
		logger_str += "err gl context\n";
	}
	// If we don't have a GL context, give up now
	if (!gl) {
		logger_str += "no webgl\n";
		var ctx=gledrawarea.getContext("2d");
		ctx.font="30px Arial";
		ctx.fillText("Your browser doesn't support WebGL.",10,50);
		//ctx=edrawarea.getContext("2d");
		//ctx.font="30px Arial";
		//ctx.fillText("2d Your browser doesn't support WebGL.",10,50);
		//alert("Unable to initialize WebGL. Your browser may not support it.");
		//gledrawarea.style.opacity = 1;	// hide the 2d canvas by showing the 3d canvas
	}
	if (gl) {
        gl.viewportWidth = gledrawarea.width;
        gl.viewportHeight = gledrawarea.height;
	    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
		gl.enable(gl.BLEND);
		gl.blendFunc (gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA);
	    gl.clearColor(.7,.7,0,1);                      // Set clear color to yellow, fully opaque
	    gl.enable(gl.DEPTH_TEST);                               // Enable depth testing
	    gl.depthFunc(gl.LEQUAL);                                // Near things obscure far things
	    gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);      // Clear the color as well as the depth buffer.
//		mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, 0.5, 2.0);
		mat4.perspectivelhc(pMatrix, Math.PI/180 * 90, gl.viewportWidth / gl.viewportHeight, 0.002,10000.0);
		// convert to d3d like left handed coord system by negating z, z now increases away in front of the camera, towards the horizon
		//pMatrix[8] = -pMatrix[8]; pMatrix[9] = -pMatrix[9]; pMatrix[10] = -pMatrix[10]; pMatrix[11] = -pMatrix[11];
	    initShaders();
	    gl.frontFace(gl.CW);
	    gl.cullFace(gl.BACK);
	    gl.enable(gl.CULL_FACE);
/*	    initBuffers();
	    inittextures(); */
	}
}

function gl_reset() {
	//logger_str+= " gl_reset\n";
}

function gl_exit() {
	exitShaders();
}

var isloading = false;
var isgl = false;

function load_mode(e) {
	isloading = e;
	gl_out_screen();
}
	
function gl_mode(e) {
	isgl = e;
	gl_out_screen();
}

function gl_out_screen() {
	if (!isloading && isgl)
		gledrawarea.style.opacity = 1;	// hide the 2d canvas by showing the 3d canvas, or show no webgl on 3d canvas set to 2d
	else
		gledrawarea.style.opacity = 0;	// see the 2d canvas
}

var shaderPrograms = {};
var shadershadowmapbuild = null;
function getShader2v(gl,id) {
    var str = preloadedtext[id];
	if (!str)
		return null;
    var shader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(shader,str);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert("VERTEX SHADER ERROR '" + id + "' " + gl.getShaderInfoLog(shader));
        return null;
    }
    return shader;
}

function getShader2p(gl,id) {
    var str = preloadedtext[id];
	if (!str)
		return null;
    var shader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(shader,str);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert("PIXEL SHADER ERROR '" + id + "' " + gl.getShaderInfoLog(shader));
        return null;
    }
    return shader;
}

function preloadShaders() {
	var i,n = shaderlist.length;
	for (i=0;i<n;++i) {
		preloadtext("shaders/" + shaderlist[i] + ".vs");
		preloadtext("shaders/" + shaderlist[i] + ".ps");
	}
}

function initShaders() {
	var i,j,n = shaderlist.length;
	for (j=0;j<n;++j) {
	    //var vertexShader = getShader(gl, "shader-vs");
	    //var fragmentShader = getShader(gl, "shader-fs");
	    var vertexShader = getShader2v(gl, shaderlist[j] + ".vs");
	    var fragmentShader = getShader2p(gl, shaderlist[j] + ".ps");
	
	    var shaderProgram = gl.createProgram();
	    shaderProgram.name = shaderlist[j];
	    gl.attachShader(shaderProgram, vertexShader);
	    gl.attachShader(shaderProgram, fragmentShader);
	    gl.linkProgram(shaderProgram);
	
	    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
	        alert("Could not initialise shader LINK '" + shaderlist[j] + "'");
	        alert(gl.getProgramInfoLog(shaderProgram));
	    }
	
		var nunif = gl.getProgramParameter(shaderProgram,gl.ACTIVE_UNIFORMS);
		shaderProgram.actunifs = {};
		for (i=0;i<nunif;++i) {
			var au = gl.getActiveUniform(shaderProgram,i);
			shaderProgram[au.name] = gl.getUniformLocation(shaderProgram,au.name);
			//shaderProgram.unifs.push(au);
			shaderProgram.actunifs[au.name] = au;
		}
		shaderProgram.nattrib = gl.getProgramParameter(shaderProgram,gl.ACTIVE_ATTRIBUTES);
		//shaderProgram.attribs = [];
		for (i=0;i<shaderProgram.nattrib;++i) {
			var aa = gl.getActiveAttrib(shaderProgram,i);
			shaderProgram[aa.name] = gl.getAttribLocation(shaderProgram,aa.name);
			//shaderProgram.attribs.push(aa);
		}
/*		shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
		shaderProgram.textureCoordAttribute = gl.getAttribLocation(shaderProgram, "aTextureCoord");
		shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
		shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
		shaderProgram.bright = gl.getUniformLocation(shaderProgram, "bright");
		shaderProgram.phase = gl.getUniformLocation(shaderProgram, "phase");
		shaderProgram.sampler = gl.getUniformLocation(shaderProgram, "uSampler");
		shaderPrograms.basic = shaderProgram; */
	    gl.useProgram(shaderProgram);
		//setAttributes(shaderProgram);
		setSamplerUniforms(shaderProgram);
	    //setMatrixPersUniforms(shaderProgram);
		shaderPrograms[shaderlist[j]] = shaderProgram;
		if (shaderProgram.name == "shadowmapbuild")
			shadershadowmapbuild = shaderProgram;
	}
}


//var wMatrix = mat4.create(); // world matrix, o2w
//var vMatrix = mat4.create(); // view matrix, camera, w2v

var mvMatrix = mat4.create(); // model view matrix, o2v, used in shaders, mvMatrix = vMatrix * wMatrix
var pMatrix = mat4.create(); // perspective matrix, v2c, used in shaders
var v2wMatrix = mat4.create(); // eye to world, used by envmap


// mv = v * w
// ends up being p * mv = p * v * w


var bright = .75;
var phase = 0;

//var activeattribs = [];
var nactiveattribs = 0;
// enable only the ones we need
function setAttributes(shaderProgram) {
	var i,n = shaderProgram.nattrib;
	// assume attribs count from 0
	if (n > nactiveattribs) {
		for (i=nactiveattribs;i<n;++i)
			gl.enableVertexAttribArray(i);
	} else if (n < nactiveattribs) {
		for (i=n;i<nactiveattribs;++i)
			gl.disableVertexAttribArray(i);	
		
	}
	nactiveattribs = n;
/*	gl.disableVertexAttribArray(0);
	gl.disableVertexAttribArray(1);
	for (i=0;i<n;++i) {
		gl.enableVertexAttribArray(shaderProgram[shaderProgram.attribs[i].name]);
	} */
/*	var newattribs = [];
	for (i=0;i<n;++i) {
		var val = shaderProgram[shaderProgram.attribs[i].name];
		newattribs[val] = true;
	}
	n = Math.max(activeattribs.length,n);
	for (i=0;i<n;++i) {
		if (!activeattribs[i] && newattribs[i]) {
			gl.enableVertexAttribArray(i);
			activeattribs[i] = true;
		} else if (activeattribs[i] && !newattribs[i]) {
			gl.disableVertexAttribArray(i);
			activeattribs[i] = false;
		}
	} */
	
/*    gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);
	if (shaderProgram.textureCoordAttribute !== undefined)
		gl.enableVertexAttribArray(shaderProgram.textureCoordAttribute); */
}

/*function setMatrixPersUniforms(shaderProgram) {
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
}
*/
function setMatrixModelViewUniforms(shaderProgram) {
//	mat4.mul(mvMatrix,vMatrix,wMatrix);
//	gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
	gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix); // model view
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix); // perspective
	if (shaderProgram.v2wMatrix !== undefined) // view to world
		gl.uniformMatrix4fv(shaderProgram.v2wMatrix, false, v2wMatrix); // for env map
}

function setUserUniforms(shaderProgram) {
	if (shaderProgram.bright !== undefined)
		gl.uniform1f(shaderProgram.bright,bright);
	if (shaderProgram.phase !== undefined)
		gl.uniform1f(shaderProgram.phase,phase);
}

function setSamplerUniforms(shaderProgram) {
	if (shaderProgram.uSampler !== undefined)
		gl.uniform1i(shaderProgram.uSampler, 0);
	if (shaderProgram.uSampler2 !== undefined)
		gl.uniform1i(shaderProgram.uSampler2, 1);
}


var triangleVertexPositionBuffer;
var triangleVerticesTextureCoordBuffer;

var squareVertexPositionBuffer;
var squareVerticesTextureCoordBuffer;

                
                
function initBuffers() {
    // build tri vertex buffer
    triangleVertexPositionBuffer = gl.createBuffer();
    ++nglbuffers;
    gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
    var vertices = [
         0.0,  1.0,  0.0,
         1.0, -1.0,  0.0,
        -1.0, -1.0,  0.0
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices),gl.STATIC_DRAW);
    triangleVertexPositionBuffer.itemSize = 3;
    triangleVertexPositionBuffer.numItems = 3;
	// build tri uvs
	triangleVerticesTextureCoordBuffer = gl.createBuffer();
    ++nglbuffers;
	gl.bindBuffer(gl.ARRAY_BUFFER, triangleVerticesTextureCoordBuffer);
	textureCoordinates = [
		.5,  0.0,
		0.0,  1.0,
		1.0,  1.0,
	];
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates),gl.STATIC_DRAW);
    triangleVerticesTextureCoordBuffer.itemSize = 2;
    triangleVerticesTextureCoordBuffer.numItems = 3;
	// build square vertex buffer
    squareVertexPositionBuffer = gl.createBuffer();
    ++nglbuffers;
    gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexPositionBuffer);
    vertices = [
        -1.0,  1.0,  0.0,
         1.0,  1.0,  0.0,
        -1.0, -1.0,  0.0,
         1.0, -1.0,  0.0
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices),gl.STATIC_DRAW);
    squareVertexPositionBuffer.itemSize = 3;
    squareVertexPositionBuffer.numItems = 4;
	// build square uvs
	squareVerticesTextureCoordBuffer = gl.createBuffer();
    ++nglbuffers;
	gl.bindBuffer(gl.ARRAY_BUFFER, squareVerticesTextureCoordBuffer);
	var textureCoordinates = [
		0.0,  0.0,
		1.0,  0.0,
		0.0,  1.0,
		1.0,  1.0,
	];
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates),gl.STATIC_DRAW);
    squareVerticesTextureCoordBuffer.itemSize = 2;
    squareVerticesTextureCoordBuffer.numItems = 4;
}

var testttexture0;

function inittextures() {
	testttexture0 = image2gltexture(preloadedimages["coin_logo.png"]);
}

//var oncegl = true;

function drawScene(ang) {
	var fang = 30*ang;
	bright = .5 - .5*Math.sin(fang);
	phase = .5*fang;
	
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	//return;
	
	//gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, testttexture0);
	
	//var shaderProgram = shaderPrograms["red"];
	var shaderProgram = shaderPrograms["basic"];
	gl.useProgram(shaderProgram);
/*	if (oncegl) {
	    //oncegl = false;
} */
/*		setAttributes(shaderProgram);
		//setSamplerUniforms(shaderProgram);
	    //setMatrixPersUniforms(shaderProgram);*/
	setUserUniforms(shaderProgram); 
	setAttributes(shaderProgram);
	

    mat4.identity(mvMatrix);
    mat4.translate(mvMatrix,mvMatrix, [-1.5, 0.0, 4.0]);
    mat4.rotateY(mvMatrix,mvMatrix,ang);
    setMatrixModelViewUniforms(shaderProgram);

    gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, triangleVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
    if (shaderProgram.textureCoordAttribute !== undefined) {
	    gl.bindBuffer(gl.ARRAY_BUFFER, triangleVerticesTextureCoordBuffer);
    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute, triangleVerticesTextureCoordBuffer.itemSize, gl.FLOAT, false, 0, 0);
    }
    gl.drawArrays(gl.TRIANGLES, 0, triangleVertexPositionBuffer.numItems);


    mat4.identity(mvMatrix);
    mat4.translate(mvMatrix,mvMatrix, [1.5, 0.0, 4.0]);
    mat4.rotateY(mvMatrix,mvMatrix,ang);
    setMatrixModelViewUniforms(shaderProgram);

    gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, squareVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
    if (shaderProgram.textureCoordAttribute !== undefined) {
   		gl.bindBuffer(gl.ARRAY_BUFFER, squareVerticesTextureCoordBuffer);
    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute, squareVerticesTextureCoordBuffer.itemSize, gl.FLOAT, false, 0, 0);
    }
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, squareVertexPositionBuffer.numItems);
}

//var shaderProgram = null;

function exitShaders() {
	return;
	var i,n = shaderlist.length;
	for (i=0;i<n;++i) {
		shaderProgram = shaderPrograms[i];
		var sharr = gl.getAttachedShaders(shaderProgram);
		var m = sharr.length;
		var j;
		for (j=0;j<m;++j)
			gl.deleteShader(sharr[j]);
	 	gl.deleteProgram(shaderProgram);
	 	shaderProgram = null;
	 }
	 shaderPrograms = {};
}
    
function exitBuffers() {
	gl.deleteBuffer(triangleVertexPositionBuffer);
	decnglbuffers();
	triangleVertexPositionBuffer = null;
	gl.deleteBuffer(triangleVerticesTextureCoordBuffer);
	decnglbuffers();
	triangleVerticesTextureCoordBuffer = null;
	gl.deleteBuffer(squareVertexPositionBuffer);
	decnglbuffers();
	squareVertexPositionBuffer = null;
	gl.deleteBuffer(squareVerticesTextureCoordBuffer);
	decnglbuffers();
	squareVerticesTextureCoordBuffer = null;
}

function exittextures() {
	gl.deleteTexture(testttexture0);
	decngltextures();
	testttexture0 = null;
}
