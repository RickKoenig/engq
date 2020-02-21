attribute vec3 vertexPositionAttribute;
attribute vec2 textureCoordAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

varying mediump vec2 vTextureCoord;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	vTextureCoord = textureCoordAttribute;
}
