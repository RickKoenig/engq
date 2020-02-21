attribute vec3 vertexPositionAttribute;
attribute vec3 normalAttribute;
attribute vec4 colorAttribute;

uniform mat4 mvMatrixUniform;
uniform mat4 pMatrixUniform;
uniform vec3 elightdir;

//uniform mat4 lightmvMatrix;
uniform mat4 lightpMatrix;

varying highp vec4 vColor;
varying highp vec4 vShadowCoord;
varying highp vec3 enorm;
varying highp vec3 halfv;

void main(void) {
	// evert and position
	highp vec3 evert = (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0)).xyz;
	gl_Position = pMatrixUniform * vec4(evert,1.0);
	//vShadowCoord = lightpMatrix * (lightmvMatrix * vec4(vertexPositionAttribute, 1.0));
	vShadowCoord = lightpMatrix * (mvMatrixUniform * vec4(vertexPositionAttribute, 1.0));
	//vTextureCoord = textureCoordAttribute;
	// enorm
	enorm = (mvMatrixUniform * vec4(-normalAttribute,0.0)).xyz;
// specular
	halfv = normalize(normalize(evert) + elightdir);
	vColor = colorAttribute;
}



/*
uniform mediump vec2 viewportxy;
// built in
struct gl_DepthRangeParameters {
	highp float near; // n
	highp float far; // f
	highp float diff; // f - n
};
uniform gl_DepthRangeParameters gl_DepthRange;

1
2
3
4
5
6
7
8
// z in non linear range [0,1]
float zpixel = inputTexture.r;

// conversion into NDC [-1,1]
float zndc = zpixel * 2.0 - 1.0;

// conversion into eye space
float zeye = 2*f*n / (zndc*(f-n)-(f+n));
X and Y reconstruction
First of all we need to know X and Y coordinates in NDC space:

1
2
3
// Converting from pixel coordinates to NDC
float xndc = gl_FragCoord.x/textureWidth * 2.0 - 1.0;
float yndc = gl_FragCoord.y/textureHeight * 2.0 - 1.0;
Once we have all this stuff, we can compute X and Y in eye space by using the following formulae (we are simply unprojecting X and Y from NDC to eye space):

1
2
float xeye = -zeye*(xndc*(r-l)+(r+l))/(2.0*n);
float yeye = -zeye*(yndc*(t-b)+(t+b))/(2.0*n);
 
 */