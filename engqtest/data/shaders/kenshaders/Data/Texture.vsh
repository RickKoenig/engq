//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
// this one has 1 uv and 1 color

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
//#define aUV1		v2.xy
#define aColor		v3
//#define aNormal	v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
#pragma bind_symbol(Color , v3)
//#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#define oUV0	    o1
#define oColor      o2

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)
#pragma output_map(color, o2)

//// Matrices starting at c0, maximum 8 for now

#define WorldViewProjection c0

#pragma bind_symbol(WorldViewProjection,c0,c3)

//// Uniform Parameters starting at c32, maximum 16 for now

//// Constants starting at c48, maximum 32 for now

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

main:
    m4x4    oPosition, aPosition, WorldViewProjection
	mov     oUV0, aUV0
	mov     oColor, aColor
    end	// output to all output registers before calling 'end'
endmain:

/*const char TextureVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"varying mediump float g_Fog;\n"
	"uniform lowp float cameraDistance;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldView;\n"

	"void main() {\n"

	"    gl_Position = Position * WorldViewProjection;\n"

	"    g_UV = vec2(UV0.x, 1.0 - UV0.y);\n"
	"    g_Color = Color;\n"

	"    g_Fog = -(WorldView * Position).z;\n"
	"    g_Fog = g_Fog * cameraDistance;\n"
	"}\n"
;

const char TextureFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec2 g_UV;\n"

	"uniform sampler2D tex1;\n"

	"varying mediump float g_Fog;\n"
	"uniform vec4 fogColor;\n"

	"void main() {\n"
	//"    gl_FragColor = vec(0.5, 0.5, 0.5, 1.0);\n"
	"    vec4 col = texture2D(tex1, g_UV);\n"
	//"    col.a = 1.0;\n"
	"    if (col.a < 0.2) discard;\n"
	"    gl_FragColor = col;\n"

	//"    float z = (gl_FragCoord.z / gl_FragCoord.w) * cameraDistance;\n"
	"    gl_FragColor = vec4((gl_FragColor + (fogColor - gl_FragColor) * g_Fog).xyz, col.a);\n"
	"}\n"
;
*/