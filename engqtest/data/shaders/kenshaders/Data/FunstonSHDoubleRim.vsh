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
//#define RimColor c33.xyz
//#define RimMaximum c34.x // 0 < RM <= 1

//#pragma bind_symbol(ObjectColor.xyz, c32)
//#pragma bind_symbol(RimColor.xyz, c33)
//#pragma bind_symbol(RimMaximum.x, c34)

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
#define ScaleFactor c49.x

#pragma bind_symbol(ScaleFactor.x, c49) // not in draw.cpp ??

#define RimColor c50.xyz
#define RimMaximum c51.x // 0 < RM <= 1
#define RimFactor c51.y
#define RimColor2 c52.xyz
#define RimMaximum2 c53.x // 0 < RM <= 1
#define RimFactor2 c53.y

#pragma bind_symbol(RimColor.xyz, c50)
#pragma bind_symbol(RimMaximum.x, c51)
#pragma bind_symbol(RimFactor.y, c51)
#pragma bind_symbol(RimColor2.xyz, c52)
#pragma bind_symbol(RimMaximum2.x, c53)
#pragma bind_symbol(RimFactor2.y, c53)

//// Constants starting at c64, maximum 16 for now

#define ZEROV c64
#define ONEV c65

def c64, 0,0,0,0
def c65, 1,1,1,1

#define C1 c66.x
#define C2 c66.y
#define C3 c66.z
#define C4 c66.w
#define C5 c67.x
#define C1T2 c67.y
#define C2T2 c67.z

def c66, 0.429043,0.511664,0.743125,0.886227	// C1,C2,C3,C4
//def c67, 0.247708,2*0.429043,2*0.511664,0		// C5,C1*2,C2*2,0
def c67, 0.247708,0.858086,1.023328,0			// C5,C1*2,C2*2,0

def c68,  0.68,  0.69,  0.70, 0 // L00 
def c69,  0.32,  0.37,  0.44, 0	// L1m1
def c70, -0.17, -0.17, -0.17, 0	// L10 
def c71, -0.45, -0.43, -0.34, 0	// L11 
def c72, -0.17, -0.17, -0.15, 0	// L2m2
def c73, -0.08, -0.09, -0.10, 0	// L2m1
def c74, -0.03, -0.02, -0.01, 0	// L20 
def c75,  0.16,  0.14,  0.10, 0	// L21 
def c76,  0.37,  0.31,  0.20, 0	// L22 

#define L00  c68.xyz
#define L1m1 c69.xyz
#define L10  c70.xyz
#define L11  c71.xyz
#define L2m2 c72.xyz
#define L2m1 c73.xyz
#define L20  c74.xyz
#define L21  c75.xyz
#define L22  c76.xyz

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
	#define g_Normal r9
	#define viewVec r10
	#define viewDot r11.x
	#define finalRimFactor r11.y
	#define RCP r12.x
	#define SUM r12.y
	#define rim1 r13
	#define viewDot2 r14.x // 1 - viewDot

main:
// 1
    m4x4    oPosition, aPosition, WorldViewProjection
// 2 
	mov     oUV0, aUV0
// 3     
	m3x3	g_Normal.xyz, aNormal, WorldViewInverseTranspose // transpose/order ?
    m3x3	ttnorm, aNormal, WorldInverseTranspose // is this right? or reverse?
    nrm		tnorm,ttnorm
// 4
    mul		tmp1,tnormx,tnormx
    sub		tmp1,tnormy,tnormy
    mul		tcolor.xyz,C1,tmp1
    mul		tcolor.xyz,tcolor.xyz,L22
// 5 
	mul		tmp1,tnormz,tnormz
	mul		tmp2.xyz,tmp1,L20
	mad		tcolor.xyz,tmp2.xyz,C3,tcolor.xyz
// 6
	mov		tmp2.xyz,C4
	mul		tmp2.xyz,tmp2.xyz,L00
	add		tcolor.xyz,tcolor.xyz,tmp2.xyz
// 7
	mov		tmp2.xyz,C5
	mul		tmp2.xyz,tmp2.xyz,L20
	sub		tcolor.xyz,tcolor.xyz,tmp2.xyz
// 8
	mul		tmp1,tnormx,tnormy
	mul		tmp2.xyz,tmp1,L2m2
	mad		tcolor.xyz,tmp2.xyz,C1T2,tcolor.xyz
// 9
	mul		tmp1,tnormx,tnormz
	mul		tmp2.xyz,tmp1,L21
	mad		tcolor.xyz,tmp2.xyz,C1T2,tcolor.xyz
// 10
	mul		tmp1,tnormy,tnormz
	mul		tmp2.xyz,tmp1,L2m1
	mad		tcolor.xyz,tmp2.xyz,C1T2,tcolor.xyz
// 11
	mul		tmp2.xyz,tnormx,L11
	mad		tcolor.xyz,tmp2.xyz,C2T2,tcolor.xyz
// 12
	mul		tmp2.xyz,tnormy,L1m1
	mad		tcolor.xyz,tmp2.xyz,C2T2,tcolor.xyz
// 13
	mul		tmp2.xyz,tnormz,L10
	mad		tcolor.xyz,tmp2.xyz,C2T2,tcolor.xyz
//  mov		tcolor.w,ONEV ? alpha
// 14
	mul		tcolor.xyz,tcolor,ScaleFactor
    mov		tcolor.w,ONEV
// 15
    m4x3	ObjPos.xyz, aPosition, WorldView
	
// nn start of frag shader that doesn't exist
// 16
	nrm		viewVec.xyz,-ObjPos
// nn no need to normalize since were still in the vertex shader (no interpolation of normal)
// 17
	dp3		viewDot2,viewVec,g_Normal
	sub		viewDot,ONEV.x,viewDot2
// nn smoothstep is cubic, for now use linear
// formula is finalRimFactor = (viewDot + RimMaximum - 1)/RimMaximum   // clamped between 0 and 1
// 18 rim 1
	rcp		RCP,RimMaximum
	add		SUM,viewDot,RimMaximum
	sub		SUM,SUM,ONEV.y
	mul		finalRimFactor, SUM, RCP
	min		finalRimFactor, finalRimFactor, ZEROV.x
	max		finalRimFactor, finalRimFactor, ONEV.x
	mul		rim1.xyz,RimFactor,finalRimFactor
// nn hmm, what color to output... ? g_Color modulates a texture(funston) + (finalRimFactor * RimColor)(rim, no texture)
// need 2 colors output to frag shader, this is wrong
// rim
// 19
	mul		rim1.xyz,finalRimFactor,RimColor // this should be added to frag color
// 20 rim 2, maybe do in parallel
	rcp		RCP,RimMaximum2
	add		SUM,viewDot2,RimMaximum2
	sub		SUM,SUM,ONEV.y
	mul		finalRimFactor, SUM, RCP
	min		finalRimFactor, finalRimFactor, ZEROV.x
	max		finalRimFactor, finalRimFactor, ONEV.x
	mul		rim1.xyz,RimFactor2,finalRimFactor
// 21
	mad		rim1.xyz,finalRimFactor,RimColor2,rim1.xyz // this should be added to frag color
// funston
// 22
	add		oColor.xyz,rim1,tcolor
	mov		oColor.w,ONEV.w
    end	// output to all output registers before calling 'end'
endmain:

/*
const char FunstonSHDoubleRimVertex[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Normal;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	"const float C1 = 0.429043;\n"
	"const float C2 = 0.511664;\n"
	"const float C3 = 0.743125;\n"
	"const float C4 = 0.886227;\n"
	"const float C5 = 0.247708;\n"

	"const vec3 L00   = vec3( 0.68,  0.69,  0.70);\n"
	"const vec3 L1m1  = vec3( 0.32,  0.37,  0.44);\n"
	"const vec3 L10   = vec3(-0.17, -0.17, -0.17);\n"
	"const vec3 L11   = vec3(-0.45, -0.43, -0.34);\n"
	"const vec3 L2m2  = vec3(-0.17, -0.17, -0.15);\n"
	"const vec3 L2m1  = vec3(-0.08, -0.09, -0.10);\n"
	"const vec3 L20   = vec3(-0.03, -0.02, -0.01);\n"
	"const vec3 L21   = vec3( 0.16,  0.14,  0.10);\n"
	"const vec3 L22   = vec3( 0.37,  0.31,  0.20);\n"

	"uniform float ScaleFactor;\r\n"
	

	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
// 2
	"    g_UV = UV0;\n"
// 3
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
	"    vec3 tnorm = normalize((WorldInverseTranspose * Normal).xyz);\n"
// 4 alot of this can be precalculated
	"    g_Color = vec4(C1 * L22 * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) +\n"
// 5
	"              C3 * L20 *  tnorm.z * tnorm.z +\n"
// 6
	"              C4 * L00 -\n"
// 7
	"              C5 * L20 +\n"
// 8
	"              2.0 * C1 * L2m2 * tnorm.x * tnorm.y +\n"
// 9
	"              2.0 * C1 * L21  * tnorm.x * tnorm.z +\n"
// 10
	"              2.0 * C1 * L2m1 * tnorm.y * tnorm.z +\n"
// 11
	"              2.0 * C2 * L11  * tnorm.x +\n"
// 12
	"              2.0 * C2 * L1m1 * tnorm.y +\n"
// 13
	"              2.0 * C2 * L10  * tnorm.z, 1.0);\n"
// 14
	"    g_Color *= ScaleFactor;\n"
// 15
	"    g_ObjPos = (WorldView * Position).xyz;\n"
	"}\n"
;

const char FunstonSHDoubleRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Normal;\n"

	"uniform float RimFactor;\n"
	"uniform float RimMaximum;\n"
	"uniform vec4 RimColor;\n"

	"uniform float RimFactor2;\n"
	"uniform float RimMaximum2;\n"
	"uniform vec4 RimColor2;\n"

	"uniform sampler2D tex1;\n"

	"void main() {\n"
// 16
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
// 17
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
// 18
	"    float finalRimFactor1 = RimFactor * smoothstep(1.0 - RimMaximum, 1.0, viewDot) ;\n"
// 19
	"    vec4 rim1 = RimColor * finalRimFactor1;\n"
// 20
	"    float finalRimFactor2 = RimFactor2 * smoothstep(1.0 - RimMaximum2, 1.0, 1.0 - viewDot);\n"
// 21
	"    vec4 rim2 = RimColor2 * finalRimFactor2;\n"
// 22
	"    gl_FragColor = rim1 + rim2 + g_Color * texture2D(tex1, vec2(g_UV.x, 1.0 - g_UV.y));\n"
	"}\n"
;
*/