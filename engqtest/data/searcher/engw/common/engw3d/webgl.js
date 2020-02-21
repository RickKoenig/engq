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
	//alphacutoff:.0625
	//alphacutoff:.125
	//alphacutoff:.25
	//alphacutoff:.5
	//alphacutoff:.75
	specpow:500,
};

function gl_init() {
	logger_str += " gl_init\n";	
	gl = null;
	gledrawarea = document.getElementById('mycanvas2');
	var glattr = {
		alpha:false,
		depth:true,
		stencil:false,
		antialias:true,
		premultipliedAlpha:true,
		preserveDrawingBuffer:false
	};
	try {
		// Try to grab the standard context. If it fails, fallback to experimental.
		gl = gledrawarea.getContext("webgl",glattr);
		if (!gl) {
			gl = gledrawarea.getContext("experimental-webgl",glattr);
		}
		//gl = edrawarea.getContext("experimental-webgl");
	}
	catch(e) {
		logger_str += "err gl context\n";
	}
	// If we don't have a GL context, give up now
	if (!gl) {
		logger_str += "no webgl\n";
		var ctx=gledrawarea.getContext("2d");
        gledrawarea.width = gledrawarea.clientWidth;
        gledrawarea.height = gledrawarea.clientHeight;
        gledrawarea.style.width = gledrawarea.clientWidth;
        gledrawarea.style.height = gledrawarea.clientHeight;
		ctx.font="20px Arial";
		ctx.fillText("Your browser doesn't support WebGL.",20,400);
		var instele = document.getElementById('instructions');
		instele.innerHTML = 'Get webgl ' +
			'<a href="http://get.webgl.org"> here</a> .';
		//ctx=edrawarea.getContext("2d");
		//ctx.font="30px Arial";
		//ctx.fillText("2d Your browser doesn't support WebGL.",10,50);
		//alert("Unable to initialize WebGL. Your browser may not support it.");
		//gledrawarea.style.opacity = 1;	// hide the 2d canvas by showing the 3d canvas
	}
	if (gl) {
/*		if (!myform) {
			//var sw = screen.availWidth;
			//var sh = screen.availHeight;
			var sw = 1024;
			var sh = 768;
	        maparea.style.width = sw + "px";
	        maparea.style.height = sh + "px";
	        gledrawarea.style.width = sw + "px";
	        gledrawarea.style.height = sh + "px";
	        gl.viewportWidth = sw;
	        gl.viewportHeight = sh;
	} else { */
	        gl.viewportWidth = gledrawarea.clientWidth;
	        gl.viewportHeight = gledrawarea.clientHeight;
	 //	}
        gledrawarea.width = gl.viewportWidth;
        gledrawarea.height = gl.viewportHeight;
        gl.asp = gl.viewportWidth/gl.viewportHeight;
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
	    
	    mainvp = {
	    	// where to draw
			target:null,
			// clear
			clearflags:gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT,
//			clearcolor:[0,0,0,1],
//			clearcolor:[1,1,1,1],
			clearcolor:[0,.75,1,1],
//			clearcolor:[.1,.2,.9,1],
		//	mat4.create();
			// orientation
			"trans":[0,0,0],
			"rot":[0,0,0],
		//	"scale":[1,1,1],
			// frustum
			near:.002,
			far:10000.0,
			zoom:1,
			asp:gl.asp,
			isortho:false,
			//ortho_size:10,
			// optional target (overrides rot)
			inlookat:false,
			//lookat:null,
			incamattach:false
			//camattach:null
	    };
/*	    initBuffers();
	    inittextures(); */
	}
}

function gl_exit() {
	exitShaders();
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
var LvMatrix = mat4.create();
var LpMatrix = mat4.create();
// mv = v * w
// ends up being p * mv = p * v * w

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
		gl.uniformMatrix4fv(shaderProgram.v2wMatrix, false, v2wMatrix); // for env map and shadowmapping
}

function setSamplerUniforms(shaderProgram) {
	if (shaderProgram.uSampler !== undefined)
		gl.uniform1i(shaderProgram.uSampler, 0);
	if (shaderProgram.uSampler2 !== undefined)
		gl.uniform1i(shaderProgram.uSampler2, 1);
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
  
