const char FunstonSphericalHarmonicsVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec4 Color;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"
	
	"varying mediump float g_Fog;\n"
	"uniform float cameraDistance;\n"

	"uniform vec3 C1;\n"
	"uniform vec3 C2;\n"
	"uniform vec3 C3;\n"
	"uniform vec3 C4;\n"
	"uniform vec3 C5;\n"
	"uniform vec3 C6;\n"
	"uniform vec3 C7;\n"
	"uniform vec3 C8;\n"
	"uniform vec3 C9;\n"

	"uniform float ScaleFactor;\r\n"
	

	"void main() {\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_UV = UV0;\n"

	"    vec3 tnorm = normalize((WorldInverseTranspose * Normal).xyz);\n"
	"    g_Color = vec4(C1 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) + \n"
	"                   C3 +\n"
	"                   tnorm.x * (C7 + (C4 * tnorm.y + C5 * tnorm.z)) + \n"
	"                   tnorm.y * (C6 * tnorm.z + C8) + \n"
	"                   tnorm.z * C9, 1.0);\n"
	"    g_Color *= ScaleFactor;\n"
	//"    g_Color *= Color;\n"
	
	"    vec3 objPos = (WorldView * Position).xyz;\n"
	//"    g_Color *= Color;\n"

	"    vec3 viewVec = normalize(-objPos);\n"
	"    g_Fog = -objPos.z * cameraDistance;\n"

	"}\n"
;

const char FunstonSphericalHarmonicsFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"uniform lowp vec4 fogColor;\n"
	"varying lowp float g_Fog;\n"

	"uniform sampler2D tex1;\n"

	"void main() {\n"
	"    gl_FragColor = g_Color * texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y));\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * g_Fog;\n"
	"}\n"
;
