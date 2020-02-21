attribute vec3 vertexPositionAttribute;
attribute vec2 textureCoordAttribute;
attribute vec2 textureCoordAttribute2;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

varying highp vec2 vTextureCoord;
varying highp vec2 vTextureCoord2;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	vTextureCoord = textureCoordAttribute;
	vTextureCoord2 = textureCoordAttribute2;
}
