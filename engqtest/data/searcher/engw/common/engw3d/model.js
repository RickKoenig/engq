var modelflagenums = {
// model
	"NOZBUFFER":0x1,
	"HASALPHA":0x2,
	"ISSKYBOX":0x10, // puts object at location eye space 0,0,0
};

var refcountmodellist = {};
var nglbuffers = 0;

//var inshadowmapbuild = false;
var curtree = null;
var totalverts = 0;
var totalfaces = 0;

function logrc() {
	modelrc();
	texturerc();
}

function modelrc() {
	logger("Modellist =====\n");
	totalverts = 0;
	totalfaces = 0;
	var totalmodels = 0;
	var largest = 0;
	var largestname = "---";
    for (var modname in refcountmodellist) {
        if (refcountmodellist.hasOwnProperty(modname)) {
        	var modref = refcountmodellist[modname];
        	modref.log();
        	++totalmodels;
	        var nv = modref.verts.length;
	        if (nv > largest) {
	        	largest = nv;
	        	largestname = modref.name;
	        }
        }
    }
    logger("totalmodels " + totalmodels + " totalverts " + totalverts + " totalfaces " + totalfaces + " largest '" + largestname + "'\n");
}

////// Model 3d class, one material per model
// reference counted

function Model(aname) {
	this.refcount = 1;
	this.name = aname;
	this.mat = {}; // uniform materials, all user defined
	this.flags = 0;
	this.texflags = globaltexflags;
	refcountmodellist[aname] = this;
}

Model.createmodel = function(aname) {
	var amod = refcountmodellist[aname];
	if (amod) {
		++amod.refcount;
	} else {
		amod = new Model(aname);
	}
	return amod;
};

// set model verts (3 floats each)
Model.prototype.setverts = function(verts) {
	//if (verts.length%3)
	//	alert("verts array not a multiple of 3 on model '" + this.name + "' length " + verts.length);
	this.verts = mesharray2vert(verts,3);
	if (!this.verts)
		alert("verts array not good on model '" + this.name + "' length " + verts.length);
	//this.nvert = verts.length;
};

// set model norms (3 floats each)
Model.prototype.setnorms = function(norms) {
	//if (norms.length%3)
	//	alert("norms array not a multiple of 3 on model '" + this.name + "' length " + norms.length);
	this.norms = mesharray2vert(norms,3);
	if (!this.norms)
		alert("norms array not good on model '" + this.name + "' length " + norms.length);
	if (this.norms.length != this.verts.length)
		alert("vert norm mismatch on model '" + this.name + "'");
};

// set model uvs (2 floats each)
Model.prototype.setuvs = function(uvs) {
	this.uvs = mesharray2vert(uvs,2);
	if (!this.uvs)
		alert("uvs array not good on model '" + this.name + "' length " + uvs.length);
	if (this.uvs.length != this.verts.length)
		alert("vert uv mismatch on model '" + this.name + "'");
};

Model.prototype.setuvs2 = function(uvs) {
	this.uvs2 = mesharray2vert(uvs,2);
	if (!this.uvs2)
		alert("uvs2 array not good on model '" + this.name + "' length " + uvs2.length);
	if (this.uvs2.length != this.verts.length)
		alert("vert uv2 mismatch on model '" + this.name + "'");
};

// set model cverts (4 floats each)
Model.prototype.setcverts = function(cverts) {
	this.cverts = mesharray2vert(cverts,4);
	if (!this.cverts)
		alert("cverts array not good on model '" + this.name + "' length " + cverts.length);
	if (this.cverts.length != this.verts.length)
		alert("vert cvert mismatch on model '" + this.name + "'");
};

// set model faces (1 int each)
Model.prototype.setfaces = function(faces) {
	//if (faces.length%3)
	//	alert("faces array not a multiple of 3 on model '" + this.name + "' length " + faces.length);
	this.faces = mesharray2face(faces,3);
	if (!this.faces)
		alert("faces array not good on model '" + this.name + "' length " + faces.length);
	//this.nface = faces.length/3;
};

// set model mesh
Model.prototype.setmesh = function(mesh) {
	if (mesh.verts)
		this.setverts(mesh.verts);
	if (mesh.norms)
		this.setnorms(mesh.norms);
	if (mesh.uvs)
		this.setuvs(mesh.uvs);
	if (mesh.uvs2)
		this.setuvs2(mesh.uvs2);
	if (mesh.cverts)
		this.setcverts(mesh.cverts);
	if (mesh.faces)
		this.setfaces(mesh.faces);
};

// set model shader
Model.prototype.setshader = function(shadername) {
	this.shadername = shadername;
	this.shader = shaderPrograms[shadername];
};

// set model texture (1 int each)
Model.prototype.settexture = function(texturename) {
	this.texturename = texturename;
};

// set model texture (1 int each)
Model.prototype.settexture2 = function(texturename) {
	this.texturename2 = texturename;
};

// copy model to gl
Model.prototype.commit = function() {
	if (!this.shader)
		alert("missing shader on model '" + this.name + "'");
	if (!this.verts)
		alert("missing verts on model '" + this.name + "'");
	if (this.glverts)
		alert("can only commit once on model '" + this.name + "'");
		
    // build tri vertex buffer
   	this.glverts = gl.createBuffer();
    ++nglbuffers;
    gl.bindBuffer(gl.ARRAY_BUFFER,this.glverts);
    var arrverts = meshvert2array(this.verts,3);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arrverts),gl.STATIC_DRAW);
    
	// build tri norms
	if (this.shader.normalAttribute !== undefined) {
		if (this.norms) {
			this.glnorms = gl.createBuffer();
		    ++nglbuffers;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.glnorms);
			var arrnorms = meshvert2array(this.norms,3);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arrnorms),gl.STATIC_DRAW);
		} else {
			alert("missing norms on model '" + this.name + "'  shader '" + this.shader.name + "'");
		}
	}	
	
	// build tri cverts
	if (this.shader.colorAttribute !== undefined) {
		if (this.cverts) {
			this.glcverts = gl.createBuffer();
		    ++nglbuffers;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.glcverts);
			var arrcverts = meshvert2array(this.cverts,4);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arrcverts),gl.STATIC_DRAW);
		} else {
			alert("missing cverts on model '" + this.name + "'  shader '" + this.shader.name + "'");
		}
	}
	
	// build tri uvs
	if (this.shader.textureCoordAttribute !== undefined) {
		if (this.uvs) {
			this.gluvs = gl.createBuffer();
		    ++nglbuffers;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.gluvs);
			var arruvs = meshvert2array(this.uvs,2);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arruvs),gl.STATIC_DRAW);
		} else {
			alert("missing uvs on model '" + this.name + "'  shader '" + this.shader.name + "'");
		}	
	}
	
	// build tri uvs2
	if (this.shader.textureCoordAttribute2 !== undefined) {
		if (this.uvs2) {
			this.gluvs2 = gl.createBuffer();
		    ++nglbuffers2;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.gluvs2);
			var arruvs2 = meshvert2array(this.uvs2,2);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arruvs2),gl.STATIC_DRAW);
		} else {
			alert("missing uvs2 on model '" + this.name + "'  shader '" + this.shader.name + "'");
		}	
	}

	// build tri faces
	if (this.faces) {
		if (!this.glfaces) {
			this.glfaces = gl.createBuffer();
		    ++nglbuffers;
		}
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,this.glfaces);
		var arrfaces = meshface2array(this.faces,3);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(arrfaces),gl.STATIC_DRAW);
//		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(this.faces),gl.STATIC_DRAW);
	} else if (this.verts.length%3) {
		alert("no faces and verts not a multiple of 3 on model '" + this.name + "'");
	}
	
	// build texture
	//this.hasalpha = false;
	if (this.shader.uSampler !== undefined) {
		if (this.texturename) {
			this.reftexture = Texture.createtexture(this.texturename); 
			if (this.reftexture && this.reftexture.hasalpha)
				//this.hasalpha = true;
				this.flags |= modelflagenums.HASALPHA;
		} else {
			alert("missing texture on model '" + this.name + "'  shader '" + this.shader.name + "'");
		}
	}
	
	// build texture2
	if (this.shader.uSampler2 !== undefined) {
		if (this.texturename2) {
			this.reftexture2 = Texture.createtexture(this.texturename2); 
			if (this.reftexture2 && this.reftexture2.hasalpha)
				//this.hasalpha = true;
				this.flags |= modelflagenums.HASALPHA;
		} else {
			alert("missing texture2 on model '" + this.name + "'  shader '" + this.shader.name + "'");
		}
	}
};

// set model mesh
Model.prototype.changemesh = function(newmesh) {
	if (newmesh.verts) {
		this.setverts(newmesh.verts);
		//gl.deleteBuffer(this.glverts);
		//decnglbuffers();
	   	//this.glverts = gl.createBuffer();
	    //++nglbuffers;
	    gl.bindBuffer(gl.ARRAY_BUFFER,this.glverts);
	    var arrverts = meshvert2array(this.verts,3);
	    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arrverts),gl.STATIC_DRAW);
	}
	if (newmesh.norms) {
		this.setnorms(newmesh.norms);
		if (this.shader.normalAttribute !== undefined && this.glnorms) {
			//gl.deleteBuffer(this.glnorms);
			//decnglbuffers();
			//this.glnorms = gl.createBuffer();
		    //++nglbuffers;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.glnorms);
			var arrnorms = meshvert2array(this.norms,3);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arrnorms),gl.STATIC_DRAW);
		}	
	}
	if (newmesh.uvs) {
		this.setuvs(newmesh.uvs);
		if (this.shader.textureCoordAttribute !== undefined && this.gluvs) {
			//gl.deleteBuffer(this.gluvs);
			//decnglbuffers();
			//this.gluvs = gl.createBuffer();
		    //++nglbuffers;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.gluvs);
			var arruvs = meshvert2array(this.uvs,2);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arruvs),gl.STATIC_DRAW);
		}
	}
	if (newmesh.cverts) {
		this.setcverts(newmesh.cverts);
		if (this.shader.colorAttribute !== undefined && this.glcverts) {
			//gl.deleteBuffer(this.glcverts);
			//decnglbuffers();
			//this.glcverts = gl.createBuffer();
		    //++nglbuffers;
			gl.bindBuffer(gl.ARRAY_BUFFER,this.glcverts);
			var arrcverts = meshvert2array(this.cverts,4);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(arrcverts),gl.STATIC_DRAW);
		}
	}
};

// change model texture
Model.prototype.changetexture = function(newname) {
	this.texturename = newname;
	//this.hasalpha = false;
	this.flags &= ~modelflagenums.HASALPHA;
	if (this.shader.uSampler !== undefined && this.reftexture) {
		this.reftexture.glfree();
		this.reftexture = Texture.createtexture(this.texturename); 
		if (this.reftexture && this.reftexture.hasalpha) {
			//this.hasalpha = true;
			this.flags |= modelflagenums.HASALPHA;
		}
	}
};

function setUserModelUniforms(shader,mat) {
	var keys = Object.keys(mat);
	for (var i=0;i<keys.length;++i) {
		var key = keys[i];
		var au = shader.actunifs[key];
		if (au) {
			var unf = shader[key];
			var val = mat[key];
			var type = au.type;
			switch(type) {
			case glenums.GL_FLOAT:
				gl.uniform1f(unf,val);
				break;
			case glenums.GL_FLOAT_VEC3:
				gl.uniform3fv(unf,val);
				break;
			case glenums.GL_FLOAT_VEC4:
				gl.uniform4fv(unf,val);
				break;
			case glenums.GL_FLOAT_MAT4:
				gl.uniformMatrix4fv(unf,false,val);
				break;
			}
		}
	}
}

// draw with gl
Model.prototype.draw = function() {
	if (this.flags & modelflagenums.NOZBUFFER)
	    gl.disable(gl.DEPTH_TEST);                               // turn off zbuffer
	var shaderProgram;
	if (shadowmap.inshadowmapbuild)
		shaderProgram = shadershadowmapbuild;
	else
		shaderProgram = this.shader;
	gl.useProgram(shaderProgram);
	setMatrixModelViewUniforms(shaderProgram);
	setAttributes(shaderProgram);
	
	setUserModelUniforms(shaderProgram,this.mat);
	if (curtree)
		setUserModelUniforms(shaderProgram,curtree.mat);
	setUserModelUniforms(shaderProgram,globalmat);
	
	
	gl.bindBuffer(gl.ARRAY_BUFFER, this.glverts);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,3,gl.FLOAT, false, 0, 0);
    
    if (this.glnorms) {
    	gl.bindBuffer(gl.ARRAY_BUFFER, this.glnorms);
    	gl.vertexAttribPointer(shaderProgram.normalAttribute,3,gl.FLOAT, false, 0, 0);
	}
	
    if (this.gluvs) {
    	gl.bindBuffer(gl.ARRAY_BUFFER, this.gluvs);
    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute,2,gl.FLOAT, false, 0, 0);
	}
	
    if (this.gluvs2) {
    	gl.bindBuffer(gl.ARRAY_BUFFER, this.gluvs2);
    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute2,2,gl.FLOAT, false, 0, 0);
	}
	
    if (this.glcverts) {
    	gl.bindBuffer(gl.ARRAY_BUFFER, this.glcverts);
    	gl.vertexAttribPointer(shaderProgram.colorAttribute,4,gl.FLOAT, false, 0, 0);
	}
	
	if (curtree && curtree.reftexture) {
		if (curtree.reftexture) {
			gl.activeTexture(gl.TEXTURE0);
			if (curtree.reftexture.iscubemap)
				gl.bindTexture(gl.TEXTURE_CUBE_MAP, curtree.reftexture.gltexture);
			else
				gl.bindTexture(gl.TEXTURE_2D, curtree.reftexture.gltexture);
		}
	} else {
		if (this.reftexture) {
			gl.activeTexture(gl.TEXTURE0);
			if (this.reftexture.iscubemap)
				gl.bindTexture(gl.TEXTURE_CUBE_MAP, this.reftexture.gltexture);
			else
				gl.bindTexture(gl.TEXTURE_2D, this.reftexture.gltexture);
		}
	}
	
	if (this.reftexture2) {
		gl.activeTexture(gl.TEXTURE1);
		gl.bindTexture(gl.TEXTURE_2D, this.reftexture2.gltexture);
	}
	
	if (!(this.flags & modelflagenums.HASALPHA)) { // turn it off
		if (shadowmap.inshadowmapbuild)
			gl.cullFace(gl.FRONT); // back face shadow generate non alpha models
		gl.disable(gl.BLEND);
	}
    if (this.glfaces) {
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,this.glfaces);
     	gl.drawElements(gl.TRIANGLES,this.faces.length*3,gl.UNSIGNED_SHORT,0);
    } else {
    	gl.drawArrays(gl.TRIANGLES,0,this.verts.length); // *3 ?
    }
    
	if (!(this.flags & modelflagenums.HASALPHA)) { // turn it back on
		if (shadowmap.inshadowmapbuild)
			gl.cullFace(gl.BACK); // back face shadow generate non alpha models
		gl.enable(gl.BLEND);
	}
	if (this.flags & modelflagenums.NOZBUFFER)
	    gl.enable(gl.DEPTH_TEST);                               // turn zbuffer back on
};

function decnglbuffers() {
	--nglbuffers;
	if (nglbuffers == 0)
		logger("nglbuffers = 0\n");
	if (ngltextures < 0)
		alert("nglbuffers < 0\n");
}

// free gl resources, but keep the rest, 'commit' again to get it back into gl
Model.prototype.glfree = function() {
	--this.refcount;
	if (this.refcount > 0) {
		return;
	}
	if (this.refcount < 0) {
		alert("Model refcount < 0 in '" + this.name + "'");
	}
	delete refcountmodellist[this.name];

	gl.deleteBuffer(this.glverts);
	decnglbuffers();
	this.glverts = null;

	if (this.glnorms) {
		gl.deleteBuffer(this.glnorms);
		decnglbuffers();
		this.glnorms = null;
	}
		
	if (this.glcverts) {
		gl.deleteBuffer(this.glcverts);
		decnglbuffers();
		this.glcverts = null;
	}

	if (this.gluvs) {
		gl.deleteBuffer(this.gluvs);
		decnglbuffers();
		this.gluvs = null;
	}

	if (this.gluvs2) {
		gl.deleteBuffer(this.gluvs2);
		decnglbuffers();
		this.gluvs2 = null;
	}

	if (this.glfaces) {
		gl.deleteBuffer(this.glfaces);
		decnglbuffers();
		this.glfaces = null;
	}

	if (this.reftexture) {
		this.reftexture.glfree();
		this.reftexture = null;
	}
	
	if (this.reftexture2) {
		this.reftexture2.glfree();
		this.reftexture2 = null;
	}
};

Model.prototype.newdup = function() {
	++this.refcount;
	return this;
};

Model.prototype.log = function() {
	var modellog = "   Model '" + this.name + "'";
	modellog += " refcount " + this.refcount;
	modellog += " verts " + this.verts.length;
	if (this.faces) {
		modellog += " faces " + this.faces.length;
		totalfaces += this.faces.length;
	}
	totalverts += this.verts.length;
	modellog += " shadername '" + this.shadername + "'";
	if (this.texturename)
		modellog += " texname '" + this.texturename + "'";
	if (this.texturename2)
		modellog += " texname2 '" + this.texturename2 + "'";
	logger(modellog + "\n\n");
};
