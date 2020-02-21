/*
attribute vec3 vertexPositionAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
}
*/

attribute vec3 vertexPositionAttribute;
//attribute vec2 textureCoordAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;

//varying highp vec2 vTextureCoord;

void main(void) {
	gl_Position = pMatrixUniform * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	//float t = textureCoordAttribute.x;
	//vTextureCoord.x = 3.0;
	//vTextureCoord.y = 5.0;
	//vTextureCoord = textureCoordAttribute;
}
