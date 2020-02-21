const char FunstonSphericalHarmonicsRimVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision lowp float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec4 Color;\n"

	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	"uniform float RimMaximum;\n"
	"uniform vec3 RimColor;\n"

	"varying mediump float g_Fog;\n"
	"uniform float cameraDistance;\n"
	/*
	"const float C1 = 0.429043;\n"
	"const float C2 = 0.511664;\n"
	"const float C3 = 0.743125;\n"
	"const float C4 = 0.886227;\n"
	"const float C5 = 0.247708;\n"

	"const vec3 L00   = vec3( 0.68,  0.69,  0.70);\n"
	"const vec3 L1m1  = vec3( 0.32,  0.37,  0.44);\n"
	"const vec3 L10   = vec3(-0.17, -0.17, -0.17);\n"
	"const vec3 L11   = vec3(-0.45, -0.43, -0.34);\n"
	"const vec3 L2m2  = vec3(-0.17, -0.17, -0.15);\n"
	"const vec3 L2m1  = vec3(-0.08, -0.09, -0.10);\n"
	"const vec3 L20   = vec3(-0.03, -0.02, -0.01);\n"
	"const vec3 L21   = vec3( 0.16,  0.14,  0.10);\n"
	"const vec3 L22   = vec3( 0.37,  0.31,  0.20);\n"
*/
/*
	"const vec3 C1 = vec3(0.16, 0.13, 0.09);\n"
	"const vec3 C2 = vec3(-0.02, -0.02, -0.00);\n"
	"const vec3 C3 = vec3(0.60, 0.61, 0.62);\n"
	"const vec3 C4 = vec3(-0.14, -0.14, -0.13);\n"
	"const vec3 C5 = vec3(0.14, 0.12, 0.09);\n"
	"const vec3 C6 = vec3(-0.07, -0.08, -0.09);\n"
	"const vec3 C7 = vec3(-0.46, -0.44, -0.35);\n"
	"const vec3 C8 = vec3(0.33, 0.38, 0.45);\n"
	"const vec3 C9 = vec3(-0.17, -0.17, -0.17);\n"
*/
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
	"    g_UV = vec2(UV0.x, 1.0 - UV0.y);\n"
	"    vec4 vnormal = WorldViewInverseTranspose * Normal;\n"
	"    vec3 tnorm = normalize((WorldInverseTranspose * Normal).xyz);\n"
/*
		"    g_Color = vec4(C1 * L22 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) +\n"
	"              C3 * L20 *  tnorm.z * tnorm.z +\n"
	"              C4 * L00 -\n"
	"              C5 * L20 +\n"
	"              2.0 * C1 * L2m2 * tnorm.x * tnorm.y +\n"
	"              2.0 * C1 * L21  * tnorm.x * tnorm.z +\n"
	"              2.0 * C1 * L2m1 * tnorm.y * tnorm.z +\n"
	"              2.0 * C2 * L11  * tnorm.x +\n"
	"              2.0 * C2 * L1m1 * tnorm.y +\n"
	"              2.0 * C2 * L10  * tnorm.z, 1.0);\n"*/

	"    g_Color = vec4(C1 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) + \n"
	"                   C3 +\n"
	"                   tnorm.x * (C7 + (C4 * tnorm.y + C5 * tnorm.z)) + \n"
	"                   tnorm.y * (C6 * tnorm.z + C8) + \n"
	"                   tnorm.z * C9, 1.0);\n"

	"    g_Color *= ScaleFactor;\n"

	"    vec3 objPos = (WorldView * Position).xyz;\n"
	"    g_Color *= Color;\n"

	"    vec3 viewVec = normalize(-objPos);\n"
	"    float viewDot = 1.0 - dot(viewVec, normalize(vnormal.xyz));\n"
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot) ;\n"

	"    g_Color += vec4((finalRimFactor * RimColor), 0.0);\n"

	"    g_Fog = -objPos.z * cameraDistance;\n"

	"}\n"
;

const char FunstonSphericalHarmonicsRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"

	"uniform lowp vec4 fogColor;\n"
	"varying lowp float g_Fog;\n"

	"uniform sampler2D tex1;\n"

	"void main() {\n"

	"    gl_FragColor = g_Color * texture2D(tex1, g_UV);\n"
	// Fog
	//"    float z = (gl_FragCoord.z / gl_FragCoord.w) / cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * g_Fog;\n"
	"}\n"
;
