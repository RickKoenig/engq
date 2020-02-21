const char DebugVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"

	"uniform mat4 WorldViewProjection;\n"
	
	"void main() {\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"}\n"
;

const char DebugFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"uniform mediump vec4 g_Color;\n"

	"void main() {\n"
	"    gl_FragColor = g_Color;\n"
	"}\n"
;
