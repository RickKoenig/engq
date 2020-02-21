// do in eye space
attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;
attribute vec2 textureCoordAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform vec3 elightdir;

varying highp vec2 vTextureCoord;
varying highp vec3 enorm;
varying highp vec3 halfv;

void main(void) {
	// evert and position
	highp vec3 evert = (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0)).xyz;
	gl_Position = pMatrixUniform * vec4(evert,1.0);
	
	// enorm
	enorm = (mvMatrixUniform * vec4(-normalAttribute,0.0)).xyz;
	
// specular
	halfv = normalize(normalize(evert) + elightdir);
	vTextureCoord = textureCoordAttribute;
}
