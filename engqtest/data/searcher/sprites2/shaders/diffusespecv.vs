// do in eye space
attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;
attribute vec2 textureCoordAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform vec3 elightdir;
uniform mediump float specpow;

varying highp vec2 vTextureCoord;
varying highp float bright;
varying highp float specbright;

void main(void) {
	//highp float specpow = 32.0;
	// evert and position
	highp vec3 evert = (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0)).xyz;
	gl_Position = pMatrixUniform * vec4(evert,1.0);
	
	// enorm
	highp vec3 enorm = (mvMatrixUniform * vec4(normalAttribute,0.0)).xyz;
	enorm = normalize(-enorm);
	
// dot for diffuse
	highp float diffdot = dot(enorm,elightdir);
	bright = clamp(1.0 * diffdot,0.0,.75) + .25;
	//bright = 0.0;

// specular
	highp vec3 halfv = normalize(normalize(evert) + elightdir);
	highp float specdot = dot(enorm,halfv); // doesn't work if -dot(enorm,halfv) , hmm.. 'a shader compiler bug'
	if (diffdot > 0.0 && specdot > 0.0) {
		specbright = pow(specdot,specpow);
		/*highp float s2 = specdot*specdot;
		highp float s4 = s2*s2;
		highp float s8 = s4*s4;
		highp float s16 = s8*s8;
		specbright = s16*s16; // fixed spec pow of 32, pow doesn't work past exp of 8.0 !?
		*/
	} else {
		specbright = 0.0;
	}
	vTextureCoord = textureCoordAttribute;
}
