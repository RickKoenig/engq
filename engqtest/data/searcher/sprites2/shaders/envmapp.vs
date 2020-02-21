// do in world space since envmap is in world space
attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform mat4 v2wMatrix;

varying highp vec3 wvert;
varying highp vec3 wnorm;

void main(void) {
	highp vec3 evert = (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0)).xyz;
	gl_Position = pMatrixUniform * vec4(evert,1.0);
	highp vec3 enorm = (mvMatrixUniform * vec4(normalAttribute,0.0)).xyz;
	wvert = (v2wMatrix * vec4(evert,0.0)).xyz;
	wnorm = (v2wMatrix * vec4(enorm,0.0)).xyz;
}
