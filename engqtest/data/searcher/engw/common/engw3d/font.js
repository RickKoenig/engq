////// Model 3d class, one material per model
// not using reference counter
function ModelFont(aname,fontname,shadname,cw,ch,maxcols,maxrows,wwrap) {
	this.name = aname;
	this.mat = {}; // uniform materials, all user defined
	this.isafont = true;
	this.text = "";
	this.shader = shaderPrograms[shadname];
	this.texturename = fontname;
	this.cw = cw;
	this.ch = ch;
	this.maxrows = maxrows;
	this.maxcols = maxcols;
	this.wwrap = wwrap;
/*	this.averts = new Float32Array([
		-1.0,  1.0,  0.0,
         1.0,  1.0,  0.0,
        -1.0, -1.0,  0.0,
         1.0, -1.0,  0.0
	]);
	this.auvs = new Float32Array([
		0.0,  0.0,
		1.0,  0.0,
		0.0,  1.0,
		1.0,  1.0,
	]);
	this.afaces = new Uint16Array([
    	0,1,2,
    	3,2,1
	]); */
	this.nc = 0; // number of chars printing including \n and space
	this.ng = 0; // number of glyphs printing, just printable chars
	this.ngcap = 0; // number of glyphs alloced, same as max of glyphs printed
	this.reftexture = Texture.createtexture(this.texturename); 
}

// copy model to gl, commit
ModelFont.prototype.print = function(text) { // commit
	if (this.text == text)
		return; // nothing to change
	this.text = text.slice(0);
	var i;
	if (!this.shader)
		alert("missing shader on model '" + this.name + "'");
	//if (!this.averts)
	//	alert("missing verts on model '" + this.name + "'");
	this.nc = text.length;
	var j = 0;
	var x = 0;
	var y = 0;
	
    // build tri verts and uvs
	this.averts = new Float32Array(this.nc*12); // 4 vec3's
	this.auvs = new Float32Array(this.nc*8);	// 4 vec2's
	for (i=0;i<this.nc;++i) {
		var cc = text.charCodeAt(i);
		if (cc >= 128)
			continue;
/*		if (cc == 32) { // space
			++x;
			continue;
	} */
		if (cc == 10) { // \n
			x = 0;
			++y;
			continue;
		}
		if (cc < 32)
			var boo = cc;
		if (x >= this.maxcols) {
			if (this.wwrap) {
				x = 0;
				++y;
			} else {
				continue;
			}
		}
		if (y >= this.maxrows)
			break;
		var r = cc>>3;
		var c = cc&7;
		//r = 6;
		//c = 6;
		//var fx = .025; // fudge in, helps prevent wrap around effects
		//var fy = .025;
		var fx = 0;
		var fy = 0;
		var u0 = c/8.0 + fx/8.0;
		var v0 = r/16.0 + fy/16.0;
		var u1 = (c+1)/8.0-fx/8.0;
		var v1 = (r+1)/16.0-fy/16.0;
		this.averts[12*j   ] = this.cw*x;
		this.averts[12*j+ 1] = -this.ch*y;
		this.averts[12*j+ 2] = 0;
		this.averts[12*j+ 3] = this.cw*(x + 1);
		this.averts[12*j+ 4] = -this.ch*y;
		this.averts[12*j+ 5] = 0;
		this.averts[12*j+ 6] = this.cw*x;
		this.averts[12*j+ 7] = -this.ch*(y + 1);
		this.averts[12*j+ 8] = 0;
		this.averts[12*j+ 9] = this.cw*(x + 1);
		this.averts[12*j+10] = -this.ch*(y + 1);
		this.averts[12*j+11] = 0;
		this.auvs[8*j  ] = u0;
		this.auvs[8*j+1] = v0;
		this.auvs[8*j+2] = u1;
		this.auvs[8*j+3] = v0;
		this.auvs[8*j+4] = u0;
		this.auvs[8*j+5] = v1;
		this.auvs[8*j+6] = u1;
		this.auvs[8*j+7] = v1;
		++x;
		++j;
	}
	this.ng = j;
	if (this.glverts) {
		gl.deleteBuffer(this.glverts);
		decnglbuffers();
	}
    this.glverts = gl.createBuffer();
    ++nglbuffers;
    gl.bindBuffer(gl.ARRAY_BUFFER,this.glverts);
    gl.bufferData(gl.ARRAY_BUFFER, this.averts,gl.STATIC_DRAW);
	
	if (this.gluvs) {
		gl.deleteBuffer(this.gluvs);
		decnglbuffers();
	}
	this.gluvs = gl.createBuffer();
    ++nglbuffers;
	gl.bindBuffer(gl.ARRAY_BUFFER,this.gluvs);
	gl.bufferData(gl.ARRAY_BUFFER,this.auvs,gl.STATIC_DRAW);

	 // grow tri faces if necessary
	if (this.ngcap < this.ng) {
		this.afaces = new Uint16Array(this.ng*6); // 2 face3's
		for (i=0;i<this.ng;++i) {
			this.afaces[6*i  ] = i*4;
			this.afaces[6*i+1] = i*4 + 1;
			this.afaces[6*i+2] = i*4 + 2;
			this.afaces[6*i+3] = i*4 + 3;
			this.afaces[6*i+4] = i*4 + 2;
			this.afaces[6*i+5] = i*4 + 1;
		}
		if (this.glfaces) {
			gl.deleteBuffer(this.glfaces);
			decnglbuffers();
		}
		this.glfaces = gl.createBuffer();
	    ++nglbuffers;
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,this.glfaces);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,this.afaces,gl.STATIC_DRAW);
		this.ngcap = this.ng;
	}
	
	// build sampler and texture
	if (this.shader.uSampler !== undefined && !this.reftexture) {
		alert("missing texture on font '" + this.name + "'  shader '" + this.shader.name + "'");
	}	
};

// draw with gl
ModelFont.prototype.draw = function() {
	if (!this.nc)
		return; // nothing to draw
	if (this.flags & modelflagenums.NOZBUFFER)
	    gl.disable(gl.DEPTH_TEST);                               // turn off zbuffer
	var shaderProgram = this.shader;
	gl.useProgram(shaderProgram);
	setMatrixModelViewUniforms(shaderProgram);
	setAttributes(shaderProgram);
	
	setUserModelUniforms(shaderProgram,this.mat);
	if (curtree)
		setUserModelUniforms(shaderProgram,curtree.mat);
	setUserModelUniforms(shaderProgram,globalmat);

	gl.bindBuffer(gl.ARRAY_BUFFER, this.glverts);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,3,gl.FLOAT, false, 0, 0);

    if (this.gluvs && shaderProgram.textureCoordAttribute !== undefined) {
    	gl.bindBuffer(gl.ARRAY_BUFFER, this.gluvs);
    	gl.vertexAttribPointer(shaderProgram.textureCoordAttribute,2,gl.FLOAT, false, 0, 0);
	}
	
	if (this.reftexture.gltexture && shaderProgram.uSampler !== undefined) {
		gl.activeTexture(gl.TEXTURE0);
		gl.bindTexture(gl.TEXTURE_2D, this.reftexture.gltexture);
	}
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,this.glfaces);
   	gl.drawElements(gl.TRIANGLES,this.ng*6,gl.UNSIGNED_SHORT,0);
 	if (this.flags & modelflagenums.NOZBUFFER)
	    gl.enable(gl.DEPTH_TEST);                               // turn it back on
};

// free gl resources
ModelFont.prototype.glfree = function() {
	gl.deleteBuffer(this.glverts);
	decnglbuffers();
	this.glverts = null;
	if (this.gluvs) {
		gl.deleteBuffer(this.gluvs);
		decnglbuffers();
		this.gluvs = null;
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
};
