const char TerrainLightMapVertex[] =

	"#version 110\n"
	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"
	
	// Input
	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"
	"attribute vec2 UV1;\n"

	// Output
	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"
	"varying mediump vec2 g_CUV;\n"

	"varying mediump float g_Fog;\n"

	"uniform lowp float cameraDistance;\n"

	// Matrices

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 ProjectionMatrix;\n"
	"uniform mat4 WorldView;\n"

	// Shader entry

	"void main() {\n"

	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_CUV = (ProjectionMatrix * Position).xy;\n"

	"    g_Color = Color;\n"

	"    g_UV0 = vec2(UV0.x, 1.0 - UV0.y);\n"
	"    g_UV1 = vec2(UV1.x, 1.0 - UV1.y);\n"

	//"    g_Fog = (gl_Position.z / gl_Position.w) / cameraDistance;\n"
	"    g_Fog = -(WorldView * Position).z;\n"
	"    g_Fog = g_Fog * cameraDistance;\n"

	"}\n"
;

const char TerrainLightMapFrag[] =
	
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"
	"varying mediump vec2 g_CUV;\n"

	"varying mediump float g_Fog;\n"

	"uniform lowp sampler2D tex1;\n"
	"uniform lowp sampler2D tex2;\n"
	"uniform lowp sampler2D tex3;\n"
	"uniform lowp sampler2D tex4;\n"
	"uniform lowp sampler2D caustics;\n"

	// Fog

	"uniform vec4 fogColor;\n"

//	"uniform vec4 fogTopColor;\n"

	"void main() {\n"
	"    vec4 col = (g_Color.r * texture2D(tex1, g_UV0) + g_Color.g * texture2D(tex2, g_UV0) + g_Color.b * texture2D(tex3, g_UV0)) * texture2D(tex4, g_UV1);\n"
	//"    vec4 col = (g_Color.r * texture2D(tex1, g_UV0) + g_Color.g * texture2D(tex2, g_UV0) + g_Color.b * texture2D(tex3, g_UV0));\n"
	"    gl_FragColor = col;\n"
	//"    gl_FragColor = min(col + col * texture2D(caustics, g_CUV), vec4(1.0, 1.0, 1.0, 1.0));\n"

	"    col = col + col * texture2D(caustics, g_CUV);\n"
	"    gl_FragColor = clamp(col, col, vec4(1.0, 1.0, 1.0, 1.0));\n"


	// Fog
	//"    float z = (gl_FragCoord.z / gl_FragCoord.w) / cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * g_Fog;\n"

	//"    gl_FragColor = vec4(g_Fog, g_Fog, g_Fog, 1.0);\n"

	"}\n"
;
