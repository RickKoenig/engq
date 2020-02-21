attribute vec3 vertexPositionAttribute;
attribute vec4 colorAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

varying highp vec4 vColor;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	vColor = colorAttribute;
}
