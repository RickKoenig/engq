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

//// Uniform Parameters starting at c32, maximum 32 for now
//#define ObjectColor c32.xyz
#define RimColor c33.xyz
#define RimMaximum c34.x // 0 < RM <= 1

//#pragma bind_symbol(ObjectColor.xyz, c32)
#pragma bind_symbol(RimColor.xyz, c33)
#pragma bind_symbol(RimMaximum.x, c34)

//// Constants starting at c64, maximum 16 for now

#define ZEROV c64
#define ONEV c65

def c64, 0,0,0,0
def c65, 1,1,1,1

// c80 - c81 are defined in Commonasm.h
// c93 - c95 the last 3 constant registers are used by the sincos macro

//// Temporaries, maximum 16 by hardware

	// rim
	#define ObjPos r8
	#define g_Normal r9
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
	m3x3	g_Normal.xyz, aNormal, WorldViewInverseTranspose // transpose/order ?
// 3 
	mov     oUV0, aUV0
// 4
    m4x3	ObjPos.xyz, aPosition, WorldView
// 5
	// nothing	
	
// 6 start of frag shader that doesn't exist
	nrm		viewVec.xyz,-ObjPos
// 7 no need to normalize since were still in the vertex shader (no interpolation of normal)
	dp3		viewDot,viewVec,g_Normal
	sub		viewDot,ONEV.x,viewDot
// 8 smoothstep is cubic, for now use linear
// formula is finalRimFactor = (viewDot + RimMaximum - 1)/RimMaximum   // clamped between 0 and 1
	rcp		RCP,RimMaximum
	add		SUM,viewDot,RimMaximum
	sub		SUM,SUM,ONEV.y
	mul		finalRimFactor, SUM, RCP
	min		finalRimFactor, finalRimFactor, ZEROV.x
	max		finalRimFactor, finalRimFactor, ONEV.x
// 9 hmm, what color to output... ? aColor modulates a texture + (finalRimFactor * RimColor)(rim, no texture)
// need 2 colors output to frag shader, this is wrong
// rim
//	mad		oColor.xyz,finalRimFactor,RimColor,ObjectColor // multiple const registers
	mul		rim1.xyz,finalRimFactor,RimColor // this should be added to frag color
	mov		rim1.w,ZEROV.w // alpha
// funston
// 10
	add		oColor,rim1,aColor
//	mov		oColor.w,ONEV.w
    end	// output to all output registers before calling 'end'
endmain:

/*
const char TexturedRimVertex[] = 
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"attribute vec4 Position;\n"
	"attribute vec2 UV0;\n"
	"attribute vec4 Normal;\n"
	"attribute vec4 Color;\n"

	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Color;\n"

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"

	"void main() {\n"
// 1
	"    gl_Position = Position * WorldViewProjection;\n"
// 2
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
// 3
	"    g_UV = UV0;\n"
// 4
	"    g_ObjPos = (WorldView * Position).xyz;\n"
// 5
	"    g_Color = Color;\n"
	"}\n"
;

const char TexturedRimFrag[] =
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV;\n"
	"varying mediump vec4 g_Color;\n"

	"uniform sampler2D tex1;\n"

	"uniform vec3 RimColor;\n"
	"uniform float RimMaximum;\n"

	"void main() {\n"
// 6
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
// 7
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
// 8
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot);\n"
// 9
	"    gl_FragColor = (vec4((RimColor * finalRimFactor), 0.0) + 
// 10
			texture2D(tex1, g_UV) ) * g_Color;\n"
	"}\n"
;
*/