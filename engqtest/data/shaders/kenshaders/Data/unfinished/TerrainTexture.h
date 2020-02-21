const char TerrainTextureVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec2 g_CUV;\n"

	"varying mediump float g_Fog;\n"
	"uniform lowp float cameraDistance;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldView;\n"
	"uniform mat4 ProjectionMatrix;\n"

	"void main() {\n"

	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_CUV = (ProjectionMatrix * Position).xy;\n"
	
	"    g_UV = vec2(UV0.x, 1.0 - UV0.y);\n"
	//"    g_UV = UV0;\n"
	"    g_Color = Color;\n"

	"    g_Fog = -(WorldView * Position).z;\n"
	"    g_Fog = g_Fog * cameraDistance;\n"
	"}\n"
;

const char TerrainTextureFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec2 g_CUV;\n"

	"uniform sampler2D tex1;\n"
	"uniform lowp sampler2D caustics;\n"

	"varying mediump float g_Fog;\n"
	"uniform vec4 fogColor;\n"

	"void main() {\n"

	"    vec4 col = texture2D(tex1, g_UV);\n"

	"    gl_FragColor = col;\n"
	"    gl_FragColor = col + col * texture2D(caustics, g_CUV);\n"

	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * g_Fog;\n"
	"}\n"
;
