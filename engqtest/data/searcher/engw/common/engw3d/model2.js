////// Model2 3d class, multi material
// reference counted

function Model2(aname) {
	this.refcount = 1;
	this.name = aname;
	this.mat = {}; // material for whole Model2, user defined
	this.mats = []; // materials, all user defined, just for each group
	this.grps = []; // verts, offsets, name, textures, info, etc.
	this.flags = 0;
	this.texflags = globaltexflags;
	refcountmodellist[aname] = this;
}

Model2.createmodel = function(aname) {
	var amod = refcountmodellist[aname]; // use model's list
	if (amod) {
		++amod.refcount;
	} else {
		amod = new Model2(aname);
	}
	return amod;
};

// set model2 verts (3 floats each)
Model2.prototype.setverts = function(verts) {
	//if (verts.length%3)
	//	alert("verts array not a multiple of 3 on model2 '" + this.name + "' length " + verts.length);
	this.verts = mesharray2vert(verts,3);
	if (!this.verts)
		alert("verts array not good on model2 '" + this.name + "' length " + verts.length);
	//this.nvert = verts.length;
};

// set model norms (3 floats each)
Model2.prototype.setnorms = function(norms) {
	//if (norms.length%3)
	//	alert("norms array not a multiple of 3 on model '" + this.name + "' length " + norms.length);
	this.norms = mesharray2vert(norms,3);
	if (!this.norms)
		alert("norms array not good on model2 '" + this.name + "' length " + norms.length);
	if (this.norms.length != this.verts.length)
		alert("vert norm mismatch on model2 '" + this.name + "'");
};

// set model2 uvs (2 floats each)
Model2.prototype.setuvs = function(uvs) {
	this.uvs = mesharray2vert(uvs,2);
	if (!this.uvs)
		alert("uvs array not good on model2 '" + this.name + "' length " + uvs.length);
	if (this.uvs.length != this.verts.length)
		alert("vert uv mismatch on model2 '" + this.name + "'");
};

Model2.prototype.setuvs2 = function(uvs) {
	this.uvs2 = mesharray2vert(uvs,2);
	if (!this.uvs2)
		alert("uvs2 array not good on model2 '" + this.name + "' length " + uvs2.length);
	if (this.uvs2.length != this.verts.length)
		alert("vert uv2 mismatch on model2 '" + this.name + "'");
};

// set model2 cverts (4 floats each)
Model2.prototype.setcverts = function(cverts) {
	this.cverts = mesharray2vert(cverts,4);
	if (!this.cverts)
		alert("cverts array not good on model2 '" + this.name + "' length " + cverts.length);
	if (this.cverts.length != this.verts.length)
		alert("vert cvert mismatch on model2 '" + this.name + "'");
};

// set model2 faces (1 int each)
Model2.prototype.setfaces = function(faces) {
	//if (faces.length%3)
	//	alert("faces array not a multiple of 3 on model2 '" + this.name + "' length " + faces.length);
	this.faces = mesharray2face(faces,3);
	if (!this.faces)
		alert("faces array not good on model2 '" + this.name + "' length " + faces.length);
	//this.nface = faces.length/3;
};

// set model2 mesh
Model2.prototype.setmesh = function(mesh) {
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

Model2.prototype.addmat = function(matname,texname,nface,nvert) {
	var len = this.grps.length;
	var mat = {};
	var grp = {};
	grp.name = matname;
	grp.shader = shaderPrograms[matname];
	grp.texturename = texname;
//	grp.texture = preloadedimages[texname];
	grp.nvert = nvert;
	grp.nface = nface;
	if (len == 0) {
		grp.vertidx = 0;
		grp.faceidx = 0;
	} else {
		--len;
		grp.vertidx = this.grps[len].vertidx + this.grps[len].nvert;
		grp.faceidx = this.grps[len].faceidx + this.grps[len].nface;
	}
	this.grps.push(grp);
	this.mats.push(mat); // empty user defined uniforms
	return grp;
};

Model2.prototype.addmat2t = function(matname,texname,texname2,nface,nvert) {
	var grp = this.addmat(matname,texname,nface,nvert);
	grp.texturename2 = texname2;
};

// copy model2 to gl
Model2.prototype.commit = function() {
	if (!this.verts)
		alert("missing verts on model2 '" + this.name + "'");
	if (!this.grps.length)
		//alert("missing mats on model2 '" + this.name + "'");
		return;
	if (this.grps[0].glverts)
		alert("can only commit once on model2 '" + this.name + "'");
    var arrverts = new Float32Array(meshvert2array(this.verts,3));
    var arrnorms,arrcverts,arruvs,arruvs2,arrfaces;
	if (this.norms)
		arrnorms = new Float32Array(meshvert2array(this.norms,3));
	if (this.cverts)
		arrcverts = new Float32Array(meshvert2array(this.cverts,4));
	if (this.uvs)
		arruvs = new Float32Array(meshvert2array(this.uvs,2));
	if (this.uvs2)
		arruvs2 = new Float32Array(meshvert2array(this.uvs2,2));
	if (this.faces)
		arrfaces = new Uint16Array(meshface2array(this.faces,3));
	var i,n = this.grps.length;
	//this.hasalpha = false;
	this.flags &= ~modelflagenums.HASALPHA;
	for (i=0;i<n;++i) {
		var grp = this.grps[i];
		if (!grp.shader)
			alert("missing shader on model2 '" + this.name + "' grp " + i);
			
	    // build tri vertex buffer
	    grp.glverts = gl.createBuffer();
	    ++nglbuffers;
	    gl.bindBuffer(gl.ARRAY_BUFFER,grp.glverts);
	    gl.bufferData(gl.ARRAY_BUFFER, arrverts.subarray(3*grp.vertidx,3*(grp.vertidx+grp.nvert)),gl.STATIC_DRAW);
		
		// build tri norms
		if (grp.shader.normalAttribute !== undefined) {
			if (arrnorms) {
				grp.glnorms = gl.createBuffer();
			    ++nglbuffers;
				gl.bindBuffer(gl.ARRAY_BUFFER,grp.glnorms);
				gl.bufferData(gl.ARRAY_BUFFER, arrnorms.subarray(3*grp.vertidx,3*(grp.vertidx+grp.nvert)),gl.STATIC_DRAW);
			} else {
				alert("missing norms on model2 '" + this.name + "'  shader '" + grp.shader.name + "' grp " + i);
			}
		}

		// build tri cverts
		if (grp.shader.colorAttribute !== undefined) {
			if (arrcverts) {
				grp.glcverts = gl.createBuffer();
			    ++nglbuffers;
				gl.bindBuffer(gl.ARRAY_BUFFER,grp.glcverts);
				gl.bufferData(gl.ARRAY_BUFFER, arrcverts.subarray(4*grp.vertidx,4*(grp.vertidx+grp.nvert)),gl.STATIC_DRAW);
			} else {
				alert("missing cverts on model2 '" + this.name + "'  shader '" + grp.shader.name + "' grp " + i);
			}
		}
				
		// build tri uvs
		if (grp.shader.textureCoordAttribute !== undefined) {
			if (arruvs) {
				grp.gluvs = gl.createBuffer();
			    ++nglbuffers;
				gl.bindBuffer(gl.ARRAY_BUFFER,grp.gluvs);
				gl.bufferData(gl.ARRAY_BUFFER, arruvs.subarray(2*grp.vertidx,2*(grp.vertidx+grp.nvert)),gl.STATIC_DRAW);
			} else {
				alert("missing uvs on model2 '" + this.name + "'  shader '" + grp.shader.name + "' grp " + i);
			}
		}
		
		// build tri uvs2
		if (grp.shader.textureCoordAttribute2 !== undefined) {
			if (arruvs2) {
				grp.gluvs2 = gl.createBuffer();
			    ++nglbuffers;
				gl.bindBuffer(gl.ARRAY_BUFFER,grp.gluvs2);
				gl.bufferData(gl.ARRAY_BUFFER, arruvs2.subarray(2*grp.vertidx,2*(grp.vertidx+grp.nvert)),gl.STATIC_DRAW);
			} else {
				alert("missing uvs2 on model2 '" + this.name + "'  shader '" + grp.shader.name + "' grp " + i);
			}
		}

		// build tri faces
		if (this.faces) {
			grp.glfaces = gl.createBuffer();
		    ++nglbuffers;
			gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,grp.glfaces);
			var subarr = arrfaces.subarray(3*grp.faceidx,3*(grp.faceidx+grp.nface));
			var j;
			for (j=0;j<3*grp.nface;++j)
				subarr[j] = subarr[j] - grp.vertidx;
			
			gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,subarr,gl.STATIC_DRAW);
		} else if (this.verts.length%3) {
			alert("no faces and verts not a multiple of 3 on model2 '" + this.name + "' grp " + i);
		}
		
		// build texture
		grp.hasalpha = false;
		if (grp.shader.uSampler !== undefined) {
			if (grp.texturename) {
				grp.reftexture = Texture.createtexture(grp.texturename); 
				if (grp.reftexture && grp.reftexture.hasalpha) {
					grp.hasalpha = true;
					//this.hasalpha = true;
					if (i == 0)
						this.flags |= modelflagenums.HASALPHA;
				}
			} else {
				alert("missing texture on model2 '" + this.name + "'  shader '" + grp.shader.name + "' grp " + i);
			}
		}	

		// build texture2
		if (grp.shader.uSampler2 !== undefined) {
			if (grp.texturename2) {
				grp.reftexture2 = Texture.createtexture(grp.texturename2); 
				if (grp.reftexture2 && grp.reftexture2.hasalpha) {
					grp.hasalpha = true;
					//this.hasalpha = true;
					//this.flags |= modelflagenums.HASALPHA;
				}
			} else {
				alert("missing texture2 on model2 '" + this.name + "'  shader '" + grp.shader.name + "' grp " + i);
			}
		}
	}
};

// draw with gl
Model2.prototype.draw = function() {
	var i,n = this.mats.length;
	if (this.flags & modelflagenums.NOZBUFFER)
	    gl.disable(gl.DEPTH_TEST);                               // turn off zbuffer
	for (i=0;i<n;++i) {
		var grp = this.grps[i];
		var mat = this.mats[i];
		var shaderProgram;
		if (shadowmap.inshadowmapbuild)
			shaderProgram = shadershadowmapbuild;
		else
			shaderProgram = grp.shader;
		gl.useProgram(shaderProgram);
		setMatrixModelViewUniforms(shaderProgram);
		setAttributes(shaderProgram);
		
		setUserModelUniforms(shaderProgram,mat);
		setUserModelUniforms(shaderProgram,this.mat);
		if (curtree)
			setUserModelUniforms(shaderProgram,curtree.mat);
		setUserModelUniforms(shaderProgram,globalmat);
		
		gl.bindBuffer(gl.ARRAY_BUFFER, grp.glverts);
	    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,3,gl.FLOAT, false, 0, 0);
	    
	    if (grp.glnorms) {
	    	gl.bindBuffer(gl.ARRAY_BUFFER, grp.glnorms);
	    	gl.vertexAttribPointer(shaderProgram.normalAttribute,3,gl.FLOAT, false, 0, 0);
		}
		
	    if (grp.gluvs) {
	    	gl.bindBuffer(gl.ARRAY_BUFFER, grp.gluvs);
	    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute,2,gl.FLOAT, false, 0, 0);
		}
		
	    if (grp.gluvs2) {
	    	gl.bindBuffer(gl.ARRAY_BUFFER, grp.gluvs2);
	    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute2,2,gl.FLOAT, false, 0, 0);
		}
		
	    if (grp.glcverts) {
	    	gl.bindBuffer(gl.ARRAY_BUFFER, grp.glcverts);
	    	gl.vertexAttribPointer(shaderProgram.colorAttribute,4,gl.FLOAT, false, 0, 0);
		}
		
		if (grp.reftexture) {
			gl.activeTexture(gl.TEXTURE0);
			if (grp.reftexture.iscubemap)
				gl.bindTexture(gl.TEXTURE_CUBE_MAP, grp.reftexture.gltexture);
			else
				gl.bindTexture(gl.TEXTURE_2D, grp.reftexture.gltexture);
		}
		
		if (grp.reftexture2) {
			gl.activeTexture(gl.TEXTURE1);
			gl.bindTexture(gl.TEXTURE_2D, grp.reftexture2.gltexture);
		}
		
		if (!grp.hasalpha) { // turn it off
			gl.disable(gl.BLEND);
			if (shadowmap.inshadowmapbuild)
				gl.cullFace(gl.FRONT); // back face shadow generate non alpha models
				//gl.frontFace(gl.CCW);
		}
	    if (grp.glfaces) {
			gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,grp.glfaces);
	     	gl.drawElements(gl.TRIANGLES,grp.nface*3,gl.UNSIGNED_SHORT,0);
		} else {
	    	gl.drawArrays(gl.TRIANGLES,0,grp.nvert); // *3 ?
		}
		if (!grp.hasalpha) { // turn it back on
			if (shadowmap.inshadowmapbuild)
				gl.cullFace(gl.BACK);
				//gl.frontFace(gl.CW);
			gl.enable(gl.BLEND);
		}
	}
	if (this.flags & modelflagenums.NOZBUFFER)
	    gl.enable(gl.DEPTH_TEST);                               // turn it back on
};

// free gl resources, but keep the rest, 'commit' again to get it back into gl
Model2.prototype.glfree = function() {
	--this.refcount;
	if (this.refcount > 0) {
		return;
	}
	if (this.refcount < 0) {
		alert("Model2 refcount < 0 in '" + this.name + "'");
	}
	delete refcountmodellist[this.name];
	var i,n = this.grps.length;
	for (i=0;i<n;++i) {
		var grp = this.grps[i];
		gl.deleteBuffer(grp.glverts);
		decnglbuffers();
		grp.glverts = null;
		
		if (grp.glnorms) {
			gl.deleteBuffer(grp.glnorms);
			decnglbuffers();
			grp.glnorms = null;
		}
		
		if (grp.glcverts) {
			gl.deleteBuffer(grp.glcverts);
			decnglbuffers();
			grp.glcverts = null;
		}
		
		if (grp.gluvs) {
			gl.deleteBuffer(grp.gluvs);
			decnglbuffers();
			grp.gluvs = null;
		}
		
		if (grp.gluvs2) {
			gl.deleteBuffer(grp.gluvs2);
			decnglbuffers();
			grp.gluvs2 = null;
		}
		
		if (grp.glfaces) {
			gl.deleteBuffer(grp.glfaces);
			decnglbuffers();
			grp.glfaces = null;
		}
		
		if (grp.reftexture) {
			grp.reftexture.glfree();
			grp.reftexture = null;
		}
		
		if (grp.reftexture2) {
			grp.reftexture2.glfree();
			grp.reftexture2 = null;
		}
	}
};

Model2.prototype.newdup = function() {
	++this.refcount;
	return this;
};

Model2.prototype.log = function() {


	var modellog = "   Model2 '" + this.name + "'";
	modellog += " refcount " + this.refcount;
	modellog += " verts " + this.verts.length;
	if (this.faces) {
		modellog += " faces " + this.faces.length + "\n";
		totalfaces += this.faces.length;
	}
	totalverts += this.verts.length;

	var i,n = this.grps.length;
	for (i=0;i<n;++i) {
		var grp = this.grps[i];
		var vo = grp.vertidx;
		var vs = grp.nvert;
		var fo = grp.faceidx;
		var fs = grp.nface;
		modellog += "      grp vo " + vo + " vs " + vs + " fo " + fo + " fs " + fs;
		modellog += " shader '" + grp.name + "'";
	//	modellog += " shadername '" + grp.name + "'";
		if (grp.texturename)
			modellog += " texname '" + grp.texturename + "'";
		if (grp.texturename2)
			modellog += " texname2 '" + grp.texturename2 + "'";
	//	modellog += " verts " + this.verts.length;
	//	modellog += " faces " + this.faces.length;
	//		var texnam = "---";
		modellog += "\n";
	}
	logger(modellog + "      ngroups " + n + "\n\n");
};

