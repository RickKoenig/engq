// do in eye space
attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;
attribute vec2 textureCoordAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform vec3 elightdir;

varying highp vec2 vTextureCoord;
varying highp float bright;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	highp vec3 enorm = (mvMatrixUniform * vec4(normalAttribute,0.0)).xyz;
	enorm = normalize(enorm);
	bright = clamp(-1.0 * dot(enorm,elightdir),0.0,.75) + .25;
	vTextureCoord = textureCoordAttribute;
}
