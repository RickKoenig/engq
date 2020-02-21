const char BasicColorVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"varying mediump float g_Fog;\n"
	"uniform lowp float cameraDistance;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldView;\n"

	"void main() {\n"

	"    gl_Position = Position * WorldViewProjection;\n"

	"    g_UV = vec2(UV0.x, 1.0 - UV0.y);\n"
	"    g_Color = Color;\n"

	"    g_Fog = -(WorldView * Position).z;\n"
	"    g_Fog = g_Fog * cameraDistance;\n"
	"}\n"
;

const char BasicColorFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"varying mediump float g_Fog;\n"
	"uniform vec4 fogColor;\n"

	"void main() {\n"

	//"    gl_FragColor = vec4((gl_FragColor + (fogColor - gl_FragColor) * g_Fog).xyz, col.a);\n"
	"    gl_FragColor = g_Color\n"
	"}\n"
;
