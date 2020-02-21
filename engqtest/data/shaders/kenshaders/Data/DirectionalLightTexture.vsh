//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//

// identical to directionallightpixel.vsh

#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
//#define aUV1		v2.xy
//#define aColor		v3
#define aNormal		v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
//#pragma bind_symbol(Color , v3)
#pragma bind_symbol(Normal.xyz, v4)

//// Output registers map, always write to all output registers before 'end'

#define oPosition   o0
#define oUV0	    o1
#define oColor      o2

#pragma output_map(position, o0)
#pragma output_map(texture0, o1)
#pragma output_map(color, o2)

//// Matrices starting at c0, maximum 8 for now

#define WorldViewProjection c0
#define WorldView c4
#define WorldInverseTranspose c8
#define WorldViewInverseTranspose c12
#define View c16
#define ProjectionMatrix c20

#pragma bind_symbol(WorldViewProjection,c0,c3)
#pragma bind_symbol(WorldView,c4,c7)
#pragma bind_symbol(WorldInverseTranspose,c8,c11)
#pragma bind_symbol(WorldViewInverseTranspose,c12,c15)
#pragma bind_symbol(View,c16,c19)
#pragma bind_symbol(ProjectionMatrix,c20,c23)

//// Uniform Parameters starting at c32, maximum 16 for now

#define LampDirection c32.xyz
#define LampColor c33.xyz
#define Diffuse c34.xyz
#define Ambient c35.xyz
#define Specular c36.xyz
#define Emissive c37.xyz
#define Alpha c38.x
#define Shininess c38.y
#define GlobalAmbient c39.xyz

#pragma bind_symbol(LampDirection.xyz, c32)
#pragma bind_symbol(LampColor.xyz, c33)
#pragma bind_symbol(Diffuse.xyz, c34)
#pragma bind_symbol(Ambient.xyz, c35)
#pragma bind_symbol(Specular.xyz, c36)
#pragma bind_symbol(Emissive.xyz, c37)
#pragma bind_symbol(Alpha.x, c38)
#pragma bind_symbol(Shininess.y, c38)
#pragma bind_symbol(GlobalAmbient.xyz, c39)

//// Constants starting at c48, maximum 32 for now

#define ZEROV c48
#define ONEV c49

def c48, 0,0,0,0
def c49, 1,1,1,1

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

// #define P r0 set but not used
#define NT r1
#define N r2
#define L r3
#define ambient r4
#define diffuse r5
#define V r6
#define H r7
#define specularLight r8.x
#define specular r9
#define LT r10
#define diffuseLight r11.x
#define LPV r12
#define VT r13
#define TColor r14

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
	mov     oUV0, aUV0
// 2 
    m3x3	NT.xyz, aNormal, WorldInverseTranspose
// 3     
    nrm		N.xyz,NT
// 4
    mov		ambient.xyz, Ambient
    mul		ambient.xyz, ambient, GlobalAmbient
// 5    
    mov		LT,-LampDirection
    nrm		L.xyz,LT
    dp3		diffuseLight,N,L
	max		diffuseLight, diffuseLight, ZEROV    
	mul		diffuse.xyz, diffuseLight, LampColor // float diffuseLight gets swizzled .xxxx
	mul		diffuse.xyz, diffuse, Diffuse
// 6
	mov		VT.xyz,View[2]
	nrm		V.xyz,VT
    add		LPV.xyz,L,V
    nrm		H.xyz,LPV
// 7    
	dp3		specularLight,N,H
	max		specularLight,specularLight,ZEROV
	pow		specularLight,specularLight,Shininess
	mul		specular.xyz,Specular,specularLight
	mul		specular.xyz,specular,LampColor
// 8
	add		TColor.xyz,Emissive,diffuse
	add		TColor.xyz,TColor,specular
	add		TColor.xyz,TColor,ambient
	mov		oColor.xyz, TColor
	mov		oColor.w, Alpha
    end	// output to all output registers before calling 'end'
endmain:

/*

	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_UV = UV0;\n"
// 2
//	"    vec3 P = Position.xyz;\n"
	"    vec3 N = (WorldInverseTranspose * Normal).xyz;\n"
// 3
	"    N = normalize(N);\n"
// 4
	"    vec3 ambient = Ambient * GlobalAmbient;\n"
// 5	
	"    vec3 L = normalize(-LampDirection);\n"
	"    float diffuseLight = max(dot(N, L), 0.0);\n"
	"    vec3 diffuse = Diffuse * LampColor * diffuseLight;\n"
// 6
	"    vec3 V = normalize(View[2].xyz);\n" // V is the camera normal
	"    vec3 H = normalize(L + V);\n"
// 7
	"    float specularLight = pow(max(dot(N, H), 0.0), Shininess);\n"
	"    vec3 specular = Specular * LampColor * specularLight;\n"
// 8
	"    g_Color = vec4(Emissive + diffuse + specular + ambient, Alpha);\n"

	"}\n"
;

*/
