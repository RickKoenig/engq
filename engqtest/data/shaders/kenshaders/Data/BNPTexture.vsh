//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
// this one has 1 uv

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
//#define aUV1		v2.xy
//#define aColor		v3
//#define aNormal	v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
//#pragma bind_symbol(Color , v3)
//#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#define oUV0	    o1
//#define oUV1	    o1
//#define oColor      o3

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)
//#pragma output_map(texture1, o2)
//#pragma output_map(color, o3)

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
//	mov     oUV1, aUV1
//	mov     oColor, aColor
    end	// output to all output registers before calling 'end'
endmain:
