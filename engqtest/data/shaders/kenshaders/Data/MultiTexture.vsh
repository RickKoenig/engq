//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
// this one has 2 uv's and a color

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
#define aUV1		v2.xy
#define aColor		v3
//#define aNormal	v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
#pragma bind_symbol(UV1.xy, v2)
#pragma bind_symbol(Color , v3)
//#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#define oUV0	    o1
#define oUV1	    o1
#define oColor      o3

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)
#pragma output_map(texture1, o2)
#pragma output_map(color, o3)

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
	mov     oUV1, aUV1
	mov     oColor, aColor
    end	// output to all output registers before calling 'end'
endmain:

/*
const char MultiTextureVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision lowp float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Color;\n"
	"attribute vec2 UV0;\n"
	"attribute vec2 UV1;\n"

	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"
	"varying lowp vec2 g_UV1;\n"

	"varying mediump float g_Fog;\n"
	"uniform lowp float cameraDistance;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldView;\n"
	
	"void main() {\n"
	"    g_UV = UV0;\n"
	"    g_UV1 = UV1;\n"
	"    g_Color = Color;\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_Fog = -(WorldView * Position).z;\n"
	"    g_Fog = g_Fog * cameraDistance;\n"
	"}\n"
;

// basically the frag shader should do this
// gl_FragColor = g_Color * tex1(UV0) * tex2(UV1)

const char MultiTextureFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision lowp float;\n#endif\n"


	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"
	"varying lowp vec2 g_UV1;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"

	"varying mediump float g_Fog;\n"
	"uniform vec4 fogColor;\n"

	"void main() {\n"
	//"    gl_FragColor = g_Color * texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y)) * 
				texture2D(tex2, vec2(g_UV1.x, 1.0 - g_UV1.y));\n"
	"    vec4 col = texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y)) * 
				texture2D(tex2, vec2(g_UV1.x, 1.0 - g_UV1.y));\n"
	"    gl_FragColor = col;\n"
	"    gl_FragColor = vec4((gl_FragColor + (fogColor - gl_FragColor) * g_Fog).xyz, col.a);\n"
	"}\n"
;
*/