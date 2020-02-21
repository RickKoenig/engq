const char NoTransformTextureVert[] =
	//"#version 110\n"
	//"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec2 UV0;\n"

	"varying vec2 g_UV;\n"
	
	"void main() {\n"
	"    gl_Position = Position;\n"
	"    g_UV = UV0;\n"

	"}\n"
;

const char NoTransformTextureFrag[] =
	//"#version 110\n"

	//"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"
	"varying vec2 g_UV;\n"

	"uniform medium sampler2D tex1;\n"

	"void main() {\n"
	"    gl_FragColor = texture2D(tex1, g_UV);\n"
	//"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n"
;
