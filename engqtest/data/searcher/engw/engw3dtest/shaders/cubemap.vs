attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

varying highp vec3 vNormalCoord;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	vNormalCoord = normalAttribute;
}
