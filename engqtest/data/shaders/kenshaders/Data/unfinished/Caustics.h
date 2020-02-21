const char CausticsVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 ProjectionMatrix;\n"
	
	"void main() {\n"
	"    g_UV = (ProjectionMatrix * Position).xy;\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"}\n"
;

const char CausticsFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"


	"varying mediump vec2 g_UV;\n"

	"uniform sampler2D tex1;\n"

	"void main() {\n"
	"    gl_FragColor = texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y));\n"
	"}\n"
;
