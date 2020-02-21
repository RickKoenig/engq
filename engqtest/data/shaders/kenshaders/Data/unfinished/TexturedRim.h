const char TexturedRimVertex[] = 
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec2 UV0;\n"
	"attribute vec4 Normal;\n"
	"attribute vec4 Color;\n"

	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Color;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	"void main() {\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
	"    g_UV = UV0;\n"
	"    g_ObjPos = (WorldView * Position).xyz;\n"
	"    g_Color = Color;\n"
	"}\n"
;

const char TexturedRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Color;\n"

	"uniform sampler2D tex1;\n"

	"uniform vec3 RimColor;\n"
	"uniform float RimMaximum;\n"

	"void main() {\n"
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot);\n"
	"    gl_FragColor = (vec4((RimColor * finalRimFactor), 0.0) + texture2D(tex1, g_UV) ) * g_Color;\n"
	"}\n"
;
