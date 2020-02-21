//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
// this one has position not transformed and 1 uv

#include "Commonasm.h"

// Input registers map
#define aPosition       v0
#define aTexCoord       v1

// Output registers map
#define vPosition       o0
#define vTexCoord       o1

#pragma bind_symbol(Position.xyzw,v0,v0)
#pragma bind_symbol(UV0.xy,v1,v1)

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)

main:
    mov     o0, aPosition
    mov     o1, aTexCoord
    end
endmain:
