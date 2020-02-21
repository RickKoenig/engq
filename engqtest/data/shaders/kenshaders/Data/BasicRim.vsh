//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
// brighten normals close to perpendicular to eye (bright outlines)

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
//#define aUV0		v1.xy
//#define aUV1		v2.xy
//#define aColor	v3
#define aNormal		v4.xyz

#pragma bind_symbol(Position, v0)
//#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
//#pragma bind_symbol(Color , v3)
#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers xyzw before 'end'

#define oPosition   o0
//#define oUV0	    o1
#define oColor      o2

#pragma output_map(position, o0)
//#pragma output_map(texture0, o1)
#pragma output_map(color, o2)

//// Matrices starting at c0, maximum 8 for now

#define WorldViewProjection c0
#define WorldView c4
//#define WorldInverseTranspose c8
#define WorldViewInverseTranspose c12
//#define View c16
//#define ProjectionMatrix c20

#pragma bind_symbol(WorldViewProjection,c0,c3)
#pragma bind_symbol(WorldView,c4,c7)
//#pragma bind_symbol(WorldInverseTranspose,c8,c11)
#pragma bind_symbol(WorldViewInverseTranspose,c12,c15)
//#pragma bind_symbol(View,c16,c19)
//#pragma bind_symbol(ProjectionMatrix,c20,c23)

//// Uniform Parameters starting at c32, maximum 16 for now

#define ObjectColor c32.xyz
#define RimColor c33.xyz
#define RimMaximum c34.x // 0 < RM <= 1

#pragma bind_symbol(ObjectColor.xyz, c32)
#pragma bind_symbol(RimColor.xyz, c33)
#pragma bind_symbol(RimMaximum.x, c34)

//// Constants starting at c48, maximum 32 for now

#define ZEROV c48
#define ONEV c49

def c48, 0,0,0,0
def c49, 1,1,1,1

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

#define ObjPos r0
#define g_Normal r1
#define viewVec r2
#define viewDot r3.x
#define finalRimFactor r3.y
#define RCP r4.x
#define SUM r4.y
#define TMP r5

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
//	mov     oUV0, aUV0
//	mov     oColor, aColor
// 2
	m3x3	g_Normal.xyz, aNormal, WorldViewInverseTranspose // transpose/order ?
// 3
    m4x3	ObjPos.xyz, aPosition, WorldView
// 4 start of frag shader that doesn't exist
	nrm		viewVec.xyz,-ObjPos
// 5 no need to normalize since were still in the vertex shader (no interpolation of normal)
	dp3		viewDot,viewVec,g_Normal
	sub		viewDot,ONEV.x,viewDot
// 6 smoothstep is cubic, for now use linear
// formula is finalRimFactor = (viewDot + RimMaximum - 1)/RimMaximum   // clamped between 0 and 1
	rcp		RCP,RimMaximum
	add		SUM,viewDot,RimMaximum
	sub		SUM,SUM,ONEV.y
	mul		finalRimFactor, SUM, RCP
	min		finalRimFactor, finalRimFactor, ZEROV.x
	max		finalRimFactor, finalRimFactor, ONEV.x
// 7
//	mad		oColor.xyz,finalRimFactor,RimColor,ObjectColor // multiple const registers
	mul		TMP.xyz,finalRimFactor,RimColor
	add		oColor.xyz,TMP,ObjectColor
	mov		oColor.w,ONEV.w
    end	// output to all output registers before calling 'end'
endmain:

/*
const char BasicRimVertex[] = 
	"#version 110\n"

    "#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"

	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec3 g_ObjPos;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
// 2
	"    g_Normal = WorldViewInverseTranspose * Normal;\n" // normalize ?
// 3
	"    g_ObjPos = (WorldView * Position).xyz;\n"
	"}\n"
;

const char BasicRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec3 g_ObjPos;\n"

	"uniform vec3 ObjectColor;\n"
	"uniform vec3 RimColor;\n"
	"uniform float RimMaximum;\n"

	"void main() {\n"
// 4
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
// 5
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
// 6
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot);\n"
// 7
	"    gl_FragColor = vec4((RimColor * finalRimFactor) + ObjectColor, 1.0);\n"
	"}\n"
;
*/
