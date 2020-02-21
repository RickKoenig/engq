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
#define aUV1		v2.xy
//#define aColor		v3
#define aNormal		v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
#pragma bind_symbol(UV1.xy, v2)
//#pragma bind_symbol(Color , v3)
#pragma bind_symbol(Normal.xyz, v4)

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
#define WorldView c4
#define WorldInverseTranspose c8
#define WorldViewInverseTranspose c12
#define View c16
//#define ProjectionMatrix c20

#pragma bind_symbol(WorldViewProjection,c0,c3)
#pragma bind_symbol(WorldView,c4,c7)
#pragma bind_symbol(WorldInverseTranspose,c8,c11)
#pragma bind_symbol(WorldViewInverseTranspose,c12,c15)
#pragma bind_symbol(View,c16,c19)
//#pragma bind_symbol(ProjectionMatrix,c20,c23)

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

//#define ObjectColor c40.xyz
#define RimColor c41.xyz
#define RimMaximum c42.x // 0 < RM <= 1

//#pragma bind_symbol(ObjectColor.xyz, c40)
#pragma bind_symbol(RimColor.xyz, c41)
#pragma bind_symbol(RimMaximum.x, c42)

//// Constants starting at c48, maximum 32 for now

#define ZEROV c48
#define ONEV c49

def c48, 0,0,0,0
def c49, 1,1,1,1

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

// #define P r0 set but not used
#define NT r1 // temp
#define N r2
#define L r3
#define ambient r4
#define diffuse r5
#define V r6
#define H r7 // temp
#define specularLight r8.x
#define specular r9
#define LT r10 // temp
#define diffuseLight r11.x
#define LPV r12 // temp
#define VT r13 // temp
#define TColor r14
// some overlap
#define ObjPos r1
#define g_Normal r7
#define viewVec r10
#define viewDot r12.x
#define finalRimFactor r12.y
#define RCP r13.x
#define SUM r13.y
#define rim1 r15

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
// 2 
	mov     oUV0, aUV0
	mov     oUV1, aUV1
// 3     
    m3x3	NT.xyz, aNormal, WorldInverseTranspose
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
	mov		TColor.w, Alpha
// 9
	m3x3	g_Normal.xyz, aNormal, WorldViewInverseTranspose // transpose/order ?
    m4x3	ObjPos.xyz, aPosition, WorldView


// 10 frag shader
	nrm		viewVec.xyz,-ObjPos
// 11 no need to normalize since were still in the vertex shader (no interpolation of normal)
	dp3		viewDot,viewVec,g_Normal
	sub		viewDot,ONEV.x,viewDot
// 12 smoothstep is cubic, for now use linear
// formula is finalRimFactor = (viewDot + RimMaximum - 1)/RimMaximum   // clamped between 0 and 1
	rcp		RCP,RimMaximum
	add		SUM,viewDot,RimMaximum
	sub		SUM,SUM,ONEV.y
	mul		finalRimFactor, SUM, RCP
	min		finalRimFactor, finalRimFactor, ZEROV.x
	max		finalRimFactor, finalRimFactor, ONEV.x
// 13 hmm, what color to output... ? aColor modulates a texture + (finalRimFactor * RimColor)(rim, no texture)
// need 2 colors output to frag shader, this is wrong
// rim
//	mad		oColor.xyz,finalRimFactor,RimColor,ObjectColor // multiple const registers
	mul		rim1.xyz,finalRimFactor,RimColor // this should be added to frag color
	mov		rim1.w,ZEROV.w // alpha
// add light
// 14
	add		oColor,rim1,TColor
//	mov		oColor.w,ONEV.w
	
    end	// output to all output registers before calling 'end'
endmain:

/*
const char DirectionalLightMultiTextureRimVertex[] =

	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	// Input

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec2 UV1;\n"

	// Output



	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"

	// Matrices

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"
	"uniform mat4 View;\n"

	"uniform vec3 LampDirection;\n"
	"uniform vec3 LampColor;\n"
	"uniform vec3 Diffuse;\n"
	"uniform vec3 Ambient;\n"
	"uniform vec3 Specular;\n"
	"uniform vec3 Emissive;\n"
	"uniform float Alpha;\n"
	"uniform float Shininess;\n"
	"uniform vec3 GlobalAmbient;\n"
	

	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
// 2
	"    g_UV0 = UV0;\n"
	"    g_UV1 = UV1;\n"
// 3
	"    vec3 P = Position.xyz;\n"
	"    vec3 N = (WorldInverseTranspose * Normal).xyz;\n"
	"    N = normalize(N);\n"
// 4
	"    vec3 ambient = Ambient * GlobalAmbient;\n"
// 5	
	"    vec3 L = normalize(-LampDirection);\n"
	"    float diffuseLight = max(dot(N, L), 0.0);\n"
	"    vec3 diffuse = Diffuse * LampColor * diffuseLight;\n"
// 6
	"    vec3 V = normalize(View[2].xyz);\n"
	"    vec3 H = normalize(L + V);\n"
// 7
	"    float specularLight = pow(max(dot(N, H), 0.0), Shininess);\n"
	"    vec3 specular = Specular * LampColor * specularLight;\n"
// 8
	"    g_Color = vec4(Emissive + diffuse + specular + ambient, Alpha);\n"
// 9
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
	"    g_ObjPos = (WorldView * Position).xyz;\n"

	"}\n"
;


const char DirectionalLightMultiTextureRimFrag[] =
	
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec4 g_Normal;\n"
    "varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"
	"uniform vec3 RimColor;\n"
	"uniform float RimMaximum;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"
	
	"void main() {\n"
// mul 2 textures with light color
"    vec4 col = g_Color * texture2D(tex1, g_UV0) * texture2D(tex2, g_UV1);\n"
	
// 10
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
// 11
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
// 12
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot);\n"
// 13
	"    gl_FragColor = vec4((RimColor * finalRimFactor), 0.0) +
// 14
			 col;\n"
	
	"}\n"
;
*/
