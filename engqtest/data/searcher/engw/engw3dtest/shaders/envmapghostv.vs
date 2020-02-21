// do in world space since envmap is in world space
// draw what the skybox draws
attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform mat4 v2wMatrix;

varying highp vec3 woutvec;

void main(void) {
	highp vec3 evert = (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0)).xyz;
	gl_Position = pMatrixUniform * vec4(evert,1.0);
	// calc in eye space
	highp vec3 enorm = (mvMatrixUniform * vec4(normalAttribute,0.0)).xyz;
	enorm = normalize(enorm);
	highp vec3 eoutvec = evert;// - 2.0*dot(evert,enorm) * enorm; // no reflection , just draw skybox
	// convert to world space
	woutvec = (v2wMatrix * vec4(eoutvec,0.0)).xyz;
}
