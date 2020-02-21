const char NoTransformDebugVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"
	"attribute vec4 Position;\n"

	
	"void main() {\n"
	"    gl_Position = Position;\n"
	"}\n"
;

const char NoTransformDebugFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"uniform mediump vec4 g_Color;\n"

	"void main() {\n"
	"    gl_FragColor = g_Color;\n"
	"}\n"
;
