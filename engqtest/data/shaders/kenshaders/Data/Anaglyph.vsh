//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
// this one does position non transformed and 1 uv, but move the red from tex2 to tex1 ?

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
//#define aUV1		v2.xy
//#define aColor		v3
//#define aNormal		v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
//#pragma bind_symbol(Color , v3)
//#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#define oUV0	    o1
//#define oColor      o2

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)
//#pragma output_map(color, o2)

//// Matrices starting at c0, maximum 8 for now

//// Uniform Parameters starting at c32, maximum 16 for now

//// Constants starting at c48, maximum 32 for now

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

main:
    mov    oPosition, aPosition
	mov     oUV0, aUV0
    end	// output to all output registers before calling 'end'
endmain:

/*
const char AnaglyphVertex[] =
	"#version 110\n"

	"attribute vec4 Position;\n"
	"attribute vec2 UV0;\n"

	"varying vec2 g_UV;\n"
	
	"void main() {\n"
	"    gl_Position = Position;\n"
	"    g_UV = UV0;\n"

	"}\n"
;

const char AnaglyphFrag[] =
	"#version 110\n"

	"varying vec2 g_UV;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"

	"void main() {\n"
	"    vec4 col1 = texture2D(tex1, g_UV);\n"
	"    vec4 col2 = texture2D(tex2, g_UV);\n"
	"    col1.r = col2.r;\n"
	"    gl_FragColor = col1;\n"
	"}\n"
;
*/
