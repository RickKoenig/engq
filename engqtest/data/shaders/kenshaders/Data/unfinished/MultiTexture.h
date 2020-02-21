const char MultiTextureVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision lowp float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"
	"attribute vec2 UV1;\n"

	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"
	"varying lowp vec2 g_UV1;\n"

	"varying mediump float g_Fog;\n"
	"uniform lowp float cameraDistance;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldView;\n"
	
	"void main() {\n"
	"    g_UV = UV0;\n"
	"    g_UV1 = UV1;\n"
	"    g_Color = Color;\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_Fog = -(WorldView * Position).z;\n"
	"    g_Fog = g_Fog * cameraDistance;\n"
	"}\n"
;

const char MultiTextureFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision lowp float;\n#endif\n"


	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"
	"varying lowp vec2 g_UV1;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"

	"varying mediump float g_Fog;\n"
	"uniform vec4 fogColor;\n"

	"void main() {\n"
	//"    gl_FragColor = g_Color * texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y)) * texture2D(tex2, vec2(g_UV1.x, 1.0 - g_UV1.y));\n"
	"    vec4 col = texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y)) * texture2D(tex2, vec2(g_UV1.x, 1.0 - g_UV1.y));\n"
	"    gl_FragColor = col;\n"
	"    gl_FragColor = vec4((gl_FragColor + (fogColor - gl_FragColor) * g_Fog).xyz, col.a);\n"
	"}\n"
;
