#include "Commonasm.h"

// maybe move some of this into 'Commonasm.h'
//// Input registers map, read from at least one input register

#define aPosition	v0
#define aUV0		v1.xy
//#define aUV1		v2.xy
#define aColor		v3
#define aNormal		v4.xyz

#pragma bind_symbol(Position, v0)
#pragma bind_symbol(UV0.xy, v1)
//#pragma bind_symbol(UV1.xy, v2)
#pragma bind_symbol(Color , v3)
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
//#define View c16
//#define ProjectionMatrix c20

#pragma bind_symbol(WorldViewProjection,c0,c3)
#pragma bind_symbol(WorldView,c4,c7)
#pragma bind_symbol(WorldInverseTranspose,c8,c11)
#pragma bind_symbol(WorldViewInverseTranspose,c12,c15)
//#pragma bind_symbol(View,c16,c19)
//#pragma bind_symbol(ProjectionMatrix,c20,c23)

//// Uniform Parameters starting at c32, maximum 32 for now
//#define ObjectColor c32.xyz
#define RimColor c33.xyz
#define RimMaximum c34.x // 0 < RM <= 1

//#pragma bind_symbol(ObjectColor.xyz, c32)
#pragma bind_symbol(RimColor.xyz, c33)
#pragma bind_symbol(RimMaximum.x, c34)

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

	// funston
	#define ttnorm r0.xyz
	#define tnorm r1.xyz
	#define tnormx r1.x
	#define tnormy r1.y
	#define tnormz r1.z
	#define tcolor r2 // g_Color, funston
	#define tmp1 r3.x
	#define tmp2 r4

	// rim
	#define ObjPos r8
	#define vnormal r9
	#define viewVec r10
	#define viewDot r11.x
	#define finalRimFactor r11.y
	#define RCP r12.x
	#define SUM r12.y
	#define rim1 r13

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
// 2 
	mov     oUV0, aUV0
// 3     
	m3x3	vnormal.xyz, aNormal, WorldViewInverseTranspose // transpose/order ?
    m3x3	ttnorm, aNormal, WorldInverseTranspose // is this right? or reverse?
    nrm		tnorm,ttnorm
// 4
    mov		tcolor.w,ONEV
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
	mad		tcolor.xyz,tmp2.xyz,tnormy,tcolor.xyz
// 8
	mad		tcolor.xyz,tnormz,C9,tcolor.xyz
// 9
	mul		tcolor.xyz,tcolor.xyz,ScaleFactor // should ScaleFactor effect alpha ?
// 10
    m4x3	ObjPos.xyz, aPosition, WorldView
// 11
	mul		tcolor,tcolor,aColor // include alpha
	
	
// 12 the source code is now in the vertex shader (as opposed to FunstonSphericalHarmonicsRim.h)
	nrm		viewVec.xyz,-ObjPos
// 13 no need to normalize since were still in the vertex shader (no interpolation of normal)
	dp3		viewDot,viewVec,vnormal
	sub		viewDot,ONEV.x,viewDot
// 14 smoothstep is cubic, for now use linear
// formula is finalRimFactor = (viewDot + RimMaximum - 1)/RimMaximum   // clamped between 0 and 1
	rcp		RCP,RimMaximum
	add		SUM,viewDot,RimMaximum
	sub		SUM,SUM,ONEV.y
	mul		finalRimFactor, SUM, RCP
	min		finalRimFactor, finalRimFactor, ZEROV.x
	max		finalRimFactor, finalRimFactor, ONEV.x
// 15 color output ok now
// rim
//	mad		oColor.xyz,finalRimFactor,RimColor,ObjectColor // multiple const registers
	mad		tcolor.xyz,finalRimFactor,RimColor,tcolor.xyz // add funston color to FinalRimColor
// funston
//	add		oColor.xyz,tcolor,rim1
//	mov		oColor.w,ONEV.w
	mov		oColor,tcolor
    end	// output to all output registers before calling 'end'
endmain:


/*
const char FunstonSphericalHarmonicsRimVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision lowp float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec4 Color;\n"

	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	"uniform float RimMaximum;\n"
	"uniform vec3 RimColor;\n"

	"varying mediump float g_Fog;\n"
	"uniform float cameraDistance;\n"
	"uniform vec3 C1;\n"
	"uniform vec3 C2;\n"
	"uniform vec3 C3;\n"
	"uniform vec3 C4;\n"
	"uniform vec3 C5;\n"
	"uniform vec3 C6;\n"
	"uniform vec3 C7;\n"
	"uniform vec3 C8;\n"
	"uniform vec3 C9;\n"

	"uniform float ScaleFactor;\r\n"

	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
// 2
	"    g_UV = vec2(UV0.x, 1.0 - UV0.y);\n"
// 3
	"    vec4 vnormal = WorldViewInverseTranspose * Normal;\n"
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
// 10
	"    vec3 objPos = (WorldView * Position).xyz;\n"
// 11
	"    g_Color *= Color;\n"
	
// 12 this code was in frag shader for FunstonSphericalHarmonicsRim.h, now running 'smoothstep' in vertex shader
	"    vec3 viewVec = normalize(-objPos);\n"
// 13
	"    float viewDot = 1.0 - dot(viewVec, normalize(vnormal.xyz));\n"
// 14
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot) ;\n"
// 15
	"    g_Color += vec4((finalRimFactor * RimColor), 0.0);\n"

	"    g_Fog = -objPos.z * cameraDistance;\n"

	"}\n"
;

// nothing to translate here
const char FunstonSphericalHarmonicsRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying lowp vec4 g_Color;\n"
	"varying lowp vec2 g_UV;\n"

	"uniform lowp vec4 fogColor;\n"
	"varying lowp float g_Fog;\n"

	"uniform sampler2D tex1;\n"

	"void main() {\n"

	"    gl_FragColor = g_Color * texture2D(tex1, g_UV);\n"
	// Fog
	//"    float z = (gl_FragCoord.z / gl_FragCoord.w) / cameraDistance;\n"
	"    gl_FragColor = gl_FragColor + (fogColor - gl_FragColor) * g_Fog;\n"
	"}\n"
;
*/