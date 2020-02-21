// this one has just position transformed
//// Input registers map, read from at least one input register
//
// this one has position transformed (default)

#define aPosition	v0
#pragma bind_symbol(Position, v0)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#pragma output_map(position, o0)

//// Matrices starting at c0, maximum 8 for now

#define WorldViewProjection c0
#pragma bind_symbol(WorldViewProjection,c0,c3)

//// Uniform Parameters starting at c32, maximum 16 for now

//// Constants starting at c48, maximum 32 for now

//// Temporaries, maximum 16 by hardware

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
    end	// output to all output registers before calling 'end'
endmain:

/*const char DebugVertex[] =
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
*/