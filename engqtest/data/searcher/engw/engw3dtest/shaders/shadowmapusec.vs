attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;
attribute vec2 textureCoordAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform vec3 elightdir;

//uniform mat4 lightmvMatrix;
uniform mat4 lightpMatrix;

varying mediump vec2 vTextureCoord;
varying highp vec4 vShadowCoord;
varying highp vec3 enorm;
varying highp vec3 halfv;

void main(void) {
	// evert and position
	highp vec3 evert = (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0)).xyz;
	gl_Position = pMatrixUniform * vec4(evert,1.0);
	//vShadowCoord = lightpMatrix * (lightmvMatrix * vec4(vertexPositionAttribute, 1.0));
	vShadowCoord = lightpMatrix * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	vTextureCoord = textureCoordAttribute;
	// enorm
	enorm = (mvMatrixUniform * vec4(-normalAttribute,0.0)).xyz;
// specular
	halfv = normalize(normalize(evert) + elightdir);
	vTextureCoord = textureCoordAttribute;
}
