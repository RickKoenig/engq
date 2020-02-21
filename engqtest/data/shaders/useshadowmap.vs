// The vertex shader, uses shadow mapper
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_normal		v1		// normals
dcl_texcoord0	v2		// Stage 0 - texture coordinates

// constant inputs
// c0-c3				// object to clip matrix
// // c7-c10				// object to world matrix
// // c14					// world eye
// c16-c19				// object to lightclip matrix

// code
// Transform vertex position from object to view space -> A must for all vertex shaders to oPos
	m4x4 r0, v0, c0
	mov oPos, r0
// copy texture uvs over to oT0
	mov oT0.xy, v2
// copy object position in light clip space to oT1
	m4x4 r1,v0,c16
	mov oT1,r1
// copy normals over
	mov oT2.xyz, v1
/*
// try some fog, world space distance is in r3
// get world distance from eye to vertex into r3
	m4x4 r3,v0,c7 // world
	sub r3,r3,c14
	dp3 r3,r3,r3
	rsq r3.x,r3.x
	rcp r3.x,r3.x
// use c20's parms
	mad r0.x,-c20.y,r3.x,c20.z
//	add r0.x,c20.z
	mov oFog,r0.x
*/
// outputs
// oPos		post projection clip space postion
// oT0		texture uvs
// oT1		position in light clip space
// oT2		normals
// // oFog		fog factor, scalar 0 to 1
