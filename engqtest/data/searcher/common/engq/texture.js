var refcounttexturelist = {};
var ngltextures = 0;

function ilog2(t) {
	var r = 0;
	if (t == 0)
		return -1; // error, can YOU take the log of 0?
	// there's a 1 out there somewhere
	while (t) {
		t>>=1;
		++r;
	}
	return r-1;
}

// round down to a power of 2
function makepow2(x) {
	return 1<<ilog2(x);
}
	
// build gl texture from image

// optional 
// twidth,theight : width and height of resultant gltexture, powers of 2
// soffx,soffy : start of image to copy
// swidth,sheight : width and height of image to copy
// iscub, 0-5 face of cubemap, undefined if not used
var lasttexw;
var lasttexh;

var globaltexflags = 0;
var cubtexture;
var cubface;
var glcmfaceenums;

function image2gltexture(image, twidth,theight,soffx,soffy,swidth,sheight) {
	var texture;
	if (!image) // fallback to a debug texture
		image = defaultimage;
	var bind;
	if (cubface !== undefined) {
		texture = cubtexture;
		if (!glcmfaceenums) {
			glcmfaceenums = [ // make match cubeenums
				gl.TEXTURE_CUBE_MAP_POSITIVE_Z,
				gl.TEXTURE_CUBE_MAP_NEGATIVE_Z,
				gl.TEXTURE_CUBE_MAP_POSITIVE_X,
				gl.TEXTURE_CUBE_MAP_NEGATIVE_X,
				gl.TEXTURE_CUBE_MAP_POSITIVE_Y,
				gl.TEXTURE_CUBE_MAP_NEGATIVE_Y
			];
		}
		bind = glcmfaceenums[cubface];
	} else {
		texture = gl.createTexture();
		++ngltextures;
		bind = gl.TEXTURE_2D;
		gl.bindTexture(bind, texture);
	}
	if (twidth !== undefined) { // read a sub part of image into gltexture, manual
		var spriteCanvas = document.createElement('canvas');
		twidth = makepow2(twidth);
		theight = makepow2(theight);
		spriteCanvas.width = twidth;
		spriteCanvas.height = theight;
		var spriteContext = spriteCanvas.getContext('2d');
		spriteContext.drawImage(image,soffx,soffy,swidth,sheight,0,0,twidth,theight);
	
		gl.texImage2D(bind, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, spriteCanvas);
		lasttexw = twidth;
		lasttexh = theight;
	} else { // read whole texture in, automatic
		var p2w = makepow2(image.naturalWidth);
		var p2h = makepow2(image.naturalHeight);
		if (p2w != image.naturalWidth || p2h != image.naturalHeight) { // round down to a power of 2
			var spriteCanvas = document.createElement('canvas');
			spriteCanvas.width = p2w;
			spriteCanvas.height = p2h;
			var spriteContext = spriteCanvas.getContext('2d');
			spriteContext.drawImage(image,0,0,image.naturalWidth,image.naturalHeight,0,0,p2w,p2h);
		
			gl.texImage2D(bind, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, spriteCanvas);
		} else {
			gl.texImage2D(bind, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
		}
		lasttexw = p2w;
		lasttexh = p2h;
		
	}
	if (cubface === undefined) {
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		if (globaltexflags & textureflagenums.CLAMPU)
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
		if (globaltexflags & textureflagenums.CLAMPV)
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
		gl.generateMipmap(gl.TEXTURE_2D);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}
	return texture;
}

///// texture class
// reference counted

// for skybox, locations on a 4 by 3 texture
var cubeenums = {
	"POSZ":[1,1],
	"NEGZ":[3,1],
	"POSX":[2,1],
	"NEGX":[0,1],
	"POSY":[1,0],
	"NEGY":[1,2]
};

var textureflagenums = {
// globaltexture
	"CLAMPU":0x100,
	"CLAMPV":0x200,
};

/*var cubelocs = [
	[3,1],
	[1,1],
	[2,1],
	[0,1],
	[1,0],
	[1,2],
];
*/
function Texture(aname) {
	this.refcount = 1;
	this.name = aname;
	var sname = aname;
	var cmidx = aname.indexOf("CUB_");
	var animage;
// build up all of the cubmap
	if (cmidx == 0) { // cubemap prefix 'CUB_', build a cubemap texture
		this.gltexture = cubtexture = gl.createTexture();
		this.iscubemap = true;
		++ngltextures;
		gl.bindTexture(gl.TEXTURE_CUBE_MAP,cubtexture);
		sname = aname.substr(4); // remove 'CUB_'
		var keys = Object.keys(cubeenums);
		for (i=0;i<keys.length;++i) {
			var cf = keys[i] + "_" + sname; // name of face to look for
			animage = preloadedimages[sname];
			cubface = i;
			if (animage) { // cross, names like 'cube2.jpg'
				var key = keys[i];
				var cl = cubeenums[key];
				var w = animage.naturalWidth;
				var h = animage.naturalHeight;
				image2gltexture(animage,
					w/4,h/3,
					w*cl[0]/4,h*cl[1]/3,
					w/4,h/3);
			} else { // 6 pics that came from a folder, names like 'POSX_aname.jpg'
				animage = preloadedimages[cf];
				image2gltexture(animage);
			}
			cubface = undefined;
			cubtexture = undefined;
		}
		gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		//if (globaltexflags & textureflagenums.CLAMPU)
			gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
		//if (globaltexflags & textureflagenums.CLAMPV)
			gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
		gl.generateMipmap(gl.TEXTURE_CUBE_MAP);
		gl.bindTexture(gl.TEXTURE_CUBE_MAP,null);
		refcounttexturelist[aname] = this;
		this.hasalpha = animage.hasalpha;
		this.width = lasttexw;
		this.height = lasttexh;
		return;
	}
	
	this.iscubemap = false;
// build normal or part of skybox
	animage = preloadedimages[sname];
	if (!animage) {
		var keys = Object.keys(cubeenums); // check for skybox prefixes like 'POSX_'
		var i;	
		var cubidx;
		for (i=0;i<keys.length;++i) {
			var key = keys[i];
			//var val = cubeenums[key];
			var idx = aname.indexOf(key + "_");
			if (idx == 0) {
				cubidx = key;
				sname = aname.substr(5);
				break;
			}
			//logger(" key = '" + key + "', val = '" + val + "'\n");
		}
	}

	animage = preloadedimages[sname];
	var w = animage.naturalWidth;
	var h = animage.naturalHeight;
	if (cubidx !== undefined) { // skybox, load a piece from the 'cross'
		var cl = cubeenums[key];
		this.gltexture = image2gltexture(animage,
			w/4,h/3,
			w*cl[0]/4,h*cl[1]/3,
			w/4,h/3);
	} else { // normal
		this.gltexture = image2gltexture(animage);
	}
	this.width = lasttexw;
	this.height = lasttexh;
/*	this.gltexture = image2gltexture(animage,64,64,
		0,animage.naturalHeight/2,
		animage.naturalWidth/2,animage.naturalHeight/2); */
	refcounttexturelist[aname] = this;
	this.hasalpha = animage.hasalpha;
}

Texture.createtexture = function(aname) {
	var atex = refcounttexturelist[aname];
	if (atex) {
		++atex.refcount;
	} else {
		atex = new Texture(aname);
	}
	return atex;
};

/*
	rttTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, rttTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, rttFramebuffer.width, rttFramebuffer.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
*/
var totalpixels = 0;
function texturerc() {
	logger("Texturelist =====\n");
	totalpixels = 0;
	var totaltextures = 0;
	var largest = 0;
	var largestname = "---";
    for (var texname in refcounttexturelist) {
        if (refcounttexturelist.hasOwnProperty(texname)) {
        	var texref = refcounttexturelist[texname];
        	var texlog = "   tex '" + texref.name + "'";
        	texlog += " refcount " + texref.refcount;
        	var cubemult = 1;
        	if (texref.iscubemap) {
        		texlog += " CM 6";
        		cubemult = 6;
        	}
        	if (texref.framebuffer) {
        		texlog += " FB";
        	}
        	texlog += " w " + texref.width;
        	texlog += " h " + texref.height;
        	var prod = texref.width*texref.height*cubemult;
        	texlog += " p " + prod;
        	totalpixels += prod;
        	//logger("   tex = '" + texref.name + "' refcount = " + texref.refcount + "\n");
	        logger(texlog + "\n");
	        ++totaltextures;
	        if (prod > largest) {
	        	largest = prod;
	        	largestname = texref.name;
	        }
        }
    }
    logger("totaltextures " + totaltextures + " totalpixels " + totalpixels + " largest '" + largestname + "'\n");
}

function decngltextures() {
	--ngltextures;
	if (ngltextures == 0)
		logger("ngltextures = 0\n");
	if (ngltextures < 0)
		alert("ngltextures < 0\n");
}

Texture.prototype.glfree = function() {
	--this.refcount;
	if (this.refcount > 0) {
		return;
	}
	if (this.refcount < 0) {
		alert("Texture refcount < 0 in '" + this.name + "'");
	}
	delete refcounttexturelist[this.name];
	gl.deleteTexture(this.gltexture);
	decngltextures();
	this.gltexture = null;
};

// user generated texture
function DataTexture(aname,wid,hit,data) {
	var gltex = gl.createTexture();
	++ngltextures;
    gl.bindTexture(gl.TEXTURE_2D, gltex);
//    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
//    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, wid, hit, 0, gl.RGBA, gl.UNSIGNED_BYTE, data);
    //gl.generateMipmap(gl.TEXTURE_2D);
	this.name = aname;
	this.width = wid;
	this.height = hit;
	this.hasalpha = false;
	this.iscubemap = false;
	this.gltexture = gltex;
	this.refcount = 1;
	refcounttexturelist[aname] = this;
}

DataTexture.createtexture = function(aname,wid,hit,data) {
	var atex = refcounttexturelist[aname];
	if (atex) {
		++atex.refcount;
	} else {
		atex = new DataTexture(aname,wid,hit,data);
	}
	return atex;
};

DataTexture.prototype.glfree = function() {
	--this.refcount;
	if (this.refcount > 0) {
		return;
	}
	if (this.refcount < 0) {
		alert("DataTexture refcount < 0 in '" + this.name + "'");
	}
	delete refcounttexturelist[this.name];
	gl.deleteTexture(this.gltexture);
	decngltextures();
	this.gltexture = null;
};

function FrameBufferTexture(aname,wid,hit) {
	var gltex = gl.createTexture();
	++ngltextures;
    gl.bindTexture(gl.TEXTURE_2D, gltex);
    //gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);
    //gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    //gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR_MIPMAP_NEAREST);
	var extensions = gl.getSupportedExtensions();
	var ext1 = gl.getExtension("OES_texture_float"); 
	if (!ext1)
		alert("no float textures");
	var ext2 = gl.getExtension("OES_texture_float_linear"); 
	if (!ext2)
		;//alert("no float textures with linear filtering");
    if (ext1)
    	gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,wid,hit,0,gl.RGBA,gl.FLOAT,null);
    else
    	gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,wid,hit,0,gl.RGBA,gl.UNSIGNED_BYTE,null);
    //gl.generateMipmap(gl.TEXTURE_2D);
	this.name = aname;
	this.width = wid;
	this.height = hit;
	this.hasalpha = false;
	this.iscubemap = false;
	this.gltexture = gltex;
	this.refcount = 1;
	
	// create frame buffer
	this.framebuffer = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER,this.framebuffer);
	// create zbuffer
	this.renderbuffer = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER,this.renderbuffer);
    gl.renderbufferStorage(gl.RENDERBUFFER,gl.DEPTH_COMPONENT16,wid,hit); // zbuffer
	// attach texture and zbuffer
	gl.framebufferTexture2D(gl.FRAMEBUFFER,gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,this.gltexture,0);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER,gl.DEPTH_ATTACHMENT,gl.RENDERBUFFER,this.renderbuffer);
	// unbind
	gl.bindTexture(gl.TEXTURE_2D,null);
    gl.bindRenderbuffer(gl.RENDERBUFFER,null);
    gl.bindFramebuffer(gl.FRAMEBUFFER,null);
	// list
	refcounttexturelist[aname] = this;
}

FrameBufferTexture.createtexture = function(aname,wid,hit) {
	var aftex = refcounttexturelist[aname];
	if (aftex) {
		++aftex.refcount;
	} else {
		aftex = new FrameBufferTexture(aname,wid,hit);
	}
	return aftex;
};

FrameBufferTexture.prototype.glfree = function() {
	--this.refcount;
	if (this.refcount > 0) {
		return;
	}
	if (this.refcount < 0) {
		alert("FrameBufferTexture refcount < 0 in '" + this.name + "'");
	}
	delete refcounttexturelist[this.name];
	gl.deleteTexture(this.gltexture);
	decngltextures();
	this.gltexture = null;
	gl.deleteRenderbuffer(this.renderbuffer);
	this.renderbuffer = null;
	gl.deleteFramebuffer(this.framebuffer);
	this.framebuffer = null;
};

FrameBufferTexture.useframebuffer = function(texfb) {
	if (texfb)
		gl.bindFramebuffer(gl.FRAMEBUFFER,texfb.framebuffer);
	else
		gl.bindFramebuffer(gl.FRAMEBUFFER,null);
};

//var framebuffer = null;
//var renderbuffer = null;
//var rendertex = null;
/*
function createframebuffer(name,wid,hit) {
	framebuffer = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
    framebuffer.width = wid;
    framebuffer.height = hit;
    
    rendertex = DataTexture.createtexture(name,wid,hit,null);
	renderbuffer = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16,wid,hit);

	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, rendertex.gltexture, 0);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, renderbuffer);

	gl.bindTexture(gl.TEXTURE_2D, null);
    gl.bindRenderbuffer(gl.RENDERBUFFER, null);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
}

function freeframebuffer() {
	gl.deleteRenderbuffer(renderbuffer);
	gl.deleteFramebuffer(framebuffer);
	rendertex.glfree();
	framebuffer = null;
	renderbuffer = null;
	rendertex = null;
}

function useframebuffer() {
	gl.bindFramebuffer(gl.FRAMEBUFFER,framebuffer);
}

function usedefaultbuffer() {
	gl.bindFramebuffer(gl.FRAMEBUFFER,null);
}
*/