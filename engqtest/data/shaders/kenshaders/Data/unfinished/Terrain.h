const char TerrainVertex[] =

	"#version 110\n"

	// Input

	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"

	// Output

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec2 g_CUV;\n"

	// Matrices

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 ProjectionMatrix;\n"

	// Shader entry

	"void main() {\n"
	"    g_CUV = (ProjectionMatrix * Position).xy;\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_Color = Color;\n"
	"    g_UV = UV0;\n"

	"}\n"
;

const char TerrainFrag[] =
	
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec2 g_CUV;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"
	"uniform sampler2D tex3;\n"
	"uniform sampler2D caustics;\n"

	"uniform float cameraDistance;\n"
	"uniform vec4 fogColor;\n"

	"void main() {\n"
	"    vec4 col = (g_Color.r * texture2D(tex1, g_UV)) + (g_Color.g * texture2D(tex2, g_UV)) + "
	       "(g_Color.b * texture2D(tex3, g_UV));\n"
	//"    gl_FragColor = g_Color;\n"
	"    gl_FragColor = col + col * texture2D(caustics, vec2(g_CUV.x, 1.0 - g_CUV.y));\n"
	// Fog
	"    float z = (gl_FragCoord.z / gl_FragCoord.w) * cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * z;\n"

	"}\n"
;
