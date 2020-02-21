attribute vec3 vertexPositionAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
}
