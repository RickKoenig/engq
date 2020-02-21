//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
//#define aUV1		v2.xy
#define aColor		v3
//#define aNormal		v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
#pragma bind_symbol(Color , v3)
//#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#define oUV0	    o1
#define oUV1	    o2
#define oColor      o3

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)
#pragma output_map(texture1, o2)
#pragma output_map(color, o3)

//// Matrices starting at c0, maximum 8 for now

#define WorldViewProjection c0
//#define WorldView c4
//#define WorldInverseTranspose c8
//#define WorldViewInverseTranspose c12
//#define View c16
#define ProjectionMatrix c20

#pragma bind_symbol(WorldViewProjection,c0,c3)
//#pragma bind_symbol(WorldView,c4,c7)
//#pragma bind_symbol(WorldInverseTranspose,c8,c11)
//#pragma bind_symbol(WorldViewInverseTranspose,c12,c15)
//#pragma bind_symbol(View,c16,c19)
#pragma bind_symbol(ProjectionMatrix,c20,c23)

//// Uniform Parameters starting at c32, maximum 16 for now

//// Constants starting at c48, maximum 32 for now

#define ZEROV c48
#define ONEV c49

def c48, 0,0,0,0
def c49, 1,1,1,1

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

main:
    m4x4	oUV1, aPosition, ProjectionMatrix // transpose ?
    m4x4    oPosition, aPosition, WorldViewProjection
    mov		oColor,aColor
    mov		oUV0,aUV0
    end	// output to all output registers before calling 'end'
endmain:
/*
Frag shader 
	col = oColor RGB components and multiplies each by tex1,tex2,tex3(oUV0) in order
	output color = col + col * caustics(oUV1)
*/

/*
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
	"    vec4 col = (g_Color.r * texture2D(tex1, g_UV)) + (g_Color.g * texture2D(tex2, g_UV)) + (g_Color.b * texture2D(tex3, g_UV));\n"
	//"    gl_FragColor = g_Color;\n"
	"    gl_FragColor = col + col * texture2D(caustics, vec2(g_CUV.x, 1.0 - g_CUV.y));\n"
	// Fog
	"    float z = (gl_FragCoord.z / gl_FragCoord.w) * cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * z;\n"

	"}\n"
;
*/
