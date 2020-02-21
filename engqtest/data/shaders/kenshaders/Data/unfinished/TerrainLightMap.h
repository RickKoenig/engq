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

	// Matrices

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 ProjectionMatrix;\n"

	// Shader entry

	"void main() {\n"
	"    g_CUV = (Position * ProjectionMatrix).xy;\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_Color = Color;\n"
	"    g_UV0 = UV0;\n"
	"    g_UV1 = UV1;\n"

	"}\n"
;

const char TerrainLightMapFrag[] =
	
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"
	"varying mediump vec2 g_CUV;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"
	"uniform sampler2D tex3;\n"
	"uniform sampler2D tex4;\n"
	"uniform sampler2D caustics;\n"

	// Fog
	"uniform float cameraDistance;\n"
	"uniform vec4 fogColor;\n"

//	"uniform vec4 fogTopColor;\n"

	"void main() {\n"
	"    vec2 uv0 = vec2(g_UV0.x, 1.0 - g_UV0.y);\n"
	"    vec2 uv1 = vec2(g_UV1.x, 1.0 - g_UV1.y);\n"
	"    vec4 col = (g_Color.r * texture2D(tex1, uv0) + g_Color.g * texture2D(tex2, uv0) + g_Color.b * texture2D(tex3, uv0)) * texture2D(tex4, uv1);\n"
	"    gl_FragColor = min(col + col * texture2D(caustics, g_CUV), vec4(1.0, 1.0, 1.0, 1.0));\n"

	// Fog
	"    float z = (gl_FragCoord.z / gl_FragCoord.w) * cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * z;\n"

	"}\n"
;
