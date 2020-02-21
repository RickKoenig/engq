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
//#define WorldView c4
#define WorldInverseTranspose c8
//#define WorldViewInverseTranspose c12
//#define View c16
//#define ProjectionMatrix c20

#pragma bind_symbol(WorldViewProjection,c0,c3)
//#pragma bind_symbol(WorldView,c4,c7)
#pragma bind_symbol(WorldInverseTranspose,c8,c11)
//#pragma bind_symbol(WorldViewInverseTranspose,c12,c15)
//#pragma bind_symbol(View,c16,c19)
//#pragma bind_symbol(ProjectionMatrix,c20,c23)

//// Uniform Parameters starting at c32, maximum 32 for now

/*
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
*/

#define C1 c40.xyz
#define C2 c41.xyz
#define C3 c42.xyz
#define C4 c43.xyz
#define C5 c44.xyz
#define C6 c45.xyz
#define C7 c46.xyz
#define C8 c47.xyz
#define C9 c48.xyz

#pragma bind_symbol(C1.xyz, c40)
#pragma bind_symbol(C2.xyz, c41)
#pragma bind_symbol(C3.xyz, c42)
#pragma bind_symbol(C4.xyz, c43)
#pragma bind_symbol(C5.xyz, c44)
#pragma bind_symbol(C6.xyz, c45)
#pragma bind_symbol(C7.xyz, c46)
#pragma bind_symbol(C8.xyz, c47)
#pragma bind_symbol(C9.xyz, c48)

#define ScaleFactor c49.x

#pragma bind_symbol(ScaleFactor.x, c49) // not in draw.cpp ??

//// Constants starting at c64, maximum 16 for now

#define ZEROV c64
#define ONEV c65

def c64, 0,0,0,0
def c65, 1,1,1,1

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

	#define ttnorm r0.xyz
	#define tnorm r1.xyz
	#define tnormx r1.x
	#define tnormy r1.y
	#define tnormz r1.z
	#define tcolor r2
	#define tmp1 r3.x
	#define tmp2 r4

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
// 2 
	mov     oUV0, aUV0
// 3     
    m3x3	ttnorm, aNormal, WorldInverseTranspose // is this right? or reverse?
    nrm		tnorm,ttnorm
// 4
//  mov		tcolor.w,ONEV
    mul		tmp1,tnormx,tnormx
    sub		tmp1,tnormy,tnormy
    mul		tcolor.xyz,C1,tmp1
// 5
	add		tcolor.xyz,tcolor.xyz,C3
// 6
	mul		tmp2.xyz,tnormy,C4
	mad		tmp2.xyz,tnormz,C5,tmp2.xyz
	add		tmp2.xyz,tmp2.xyz,C7
	mad		tcolor.xyz,tmp2.xyz,tnormx,tcolor.xyz
// 7
	mul		tmp2.xyz,C6,tnormz
	add		tmp2.xyz,tmp2.xyz,C8
	mad		tcolor.xyz,tnormy,C8,tcolor.xyz
// 8
	mad		tcolor.xyz,tnormz,C9,tcolor.xyz
// 9
	mul		oColor.xyz,tcolor,ScaleFactor
	mov		oColor.w,ONEV.w
    end	// output to all output registers before calling 'end'
endmain:

/*
	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
// 2
	"    g_UV = UV0;\n"
// 3
	"    vec3 tnorm = normalize((WorldInverseTranspose * Normal).xyz);\n"
// 4
	"    g_Color = vec4(C1 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) + \n"
// 5
	"                   C3 +\n"
// 6
	"                   tnorm.x * (C7 + (C4 * tnorm.y + C5 * tnorm.z)) + \n"
// 7
	"                   tnorm.y * (C6 * tnorm.z + C8) + \n"
// 8
	"                   tnorm.z * C9, 1.0);\n"
// 9
	"    g_Color *= ScaleFactor;\n"
	"}\n"
*/
