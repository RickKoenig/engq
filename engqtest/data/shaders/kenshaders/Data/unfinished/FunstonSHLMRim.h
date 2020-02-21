const char FunstonSHLMRimVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec2 UV1;\n"
	"attribute vec4 Color;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec2 g_UV1;\n"
	"varying mediump vec4 g_Normal;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	
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

	"uniform float ScaleFactor;\r\n"
	

	"void main() {\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_UV = UV0;\n"
	"    g_UV1 = UV1;\n"
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
	"    vec3 tnorm = normalize((WorldInverseTranspose * Normal).xyz);\n"
	"    g_Color = vec4(C1 * L22 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) +\n"
	"              C3 * L20 *  tnorm.z * tnorm.z +\n"
	"              C4 * L00 -\n"
	"              C5 * L20 +\n"
	"              2.0 * C1 * L2m2 * tnorm.x * tnorm.y +\n"
	"              2.0 * C1 * L21  * tnorm.x * tnorm.z +\n"
	"              2.0 * C1 * L2m1 * tnorm.y * tnorm.z +\n"
	"              2.0 * C2 * L11  * tnorm.x +\n"
	"              2.0 * C2 * L1m1 * tnorm.y +\n"
	"              2.0 * C2 * L10  * tnorm.z, 1.0);\n"
	"    g_Color *= ScaleFactor;\n"
	"	 g_Color *= Color;\n"
	"    g_ObjPos = (WorldView * Position).xyz;\n"
	"}\n"
;

const char FunstonSHLMRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec2 g_UV1;\n"
	"varying mediump vec4 g_Normal;\n"

	"uniform float RimMaximum;\n"
	"uniform vec3 RimColor;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"

	"void main() {\n"
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"

	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot) ;\n"

	"    gl_FragColor = vec4((finalRimFactor * RimColor), 0.0) + g_Color * texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y)) * texture2D(tex2, vec2(g_UV1.x, 1.0 - g_UV1.y));\n"
	"}\n"
;
