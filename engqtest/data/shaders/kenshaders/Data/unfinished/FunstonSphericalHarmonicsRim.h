const char FunstonSphericalHarmonicsRimVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec4 Color;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Normal;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

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
	"const vec C1	= vec3(0.16, 0.13, 0.09);\n"
	"const vec C2	= vec3(-0.02, -0.02, -0.00);\n"
	"const vec C3   = vec3(0.60, 0.61, 0.62);\n"
	"const vec C4   = vec3(-0.14, -0.14, -0.13);\n"
	"const vec C5   = vec3(0.14, 0.12, 0.09);\n"
	"const vec C6   = vec3(-0.07, -0.08, -0.09);\n"
	"const vec C7   = vec3(-0.46, -0.44, -0.35);\n"
	"const vec C8   = vec3(0.33, 0.38, 0.45);\n"
	"const vec C9	= vec3(-0.17, -0.17, -0.17);\n"


	"uniform float ScaleFactor;\r\n"
	
	"void main() {\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_UV = UV0;\n"
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
	"    vec3 tnorm = normalize((WorldInverseTranspose * Normal).xyz);\n"
	/*"    g_Color = vec4(C1 * L22 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) +\n"
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
	"                   C2 * tnorm.z * tnorm.z + \n"
	"                   C3 +\n"
	//"                   C4 * tnorm.x * tnorm.y +\n"
	//"                   C5 * tnorm.x * tnorm.z +\n"

	//"                   C7 * tnorm.x +\n"
	//"                   tnorm.x * (C4 * tnorm.y + C5 * tnorm.z) + \n"

	"                   tnorm.x * (C7 + (C4 * tnorm.y + C5 * tnorm.z) + \n"

	//"                   C6 * tnorm.y * tnorm.z +\n"
	//"                   C8 * tnorm.y +\n"

	"                   tnorm.y * (C6 * tnorm.z + C8) + \n"

	"                   C9 * tnorm.z, 1.0);\n"

	"    g_Color *= ScaleFactor;\n"
	"    g_ObjPos = (WorldView * Position).xyz;\n"
	"    g_Color *= Color;\n"
	"}\n"
;

const char FunstonSphericalHarmonicsRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Normal;\n"

	"uniform float RimMaximum;\n"
	"uniform vec3 RimColor;\n"

	"uniform float cameraDistance;\n"
	"uniform vec4 fogColor;\n"

	"uniform sampler2D tex1;\n"

	"void main() {\n"
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot) ;\n"
	"    gl_FragColor = vec4((finalRimFactor * RimColor), 0.0) + g_Color * texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y));\n"
	// Fog
	"    float z = (gl_FragCoord.z / gl_FragCoord.w) * cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * z;\n"
	"}\n"
;
