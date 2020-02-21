// The vertex shader, texture only

vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_texcoord0	v1		// Stage 0 - texture coordinates

// constant inputs
// c0-c3				object to clip matrix

// code
// Transform vertex position from object to view space -> A must for all vertex shaders to oPos
	m4x4 r1, v0, c0
	mov oPos, r1
// copy texture uvs over to oT0
	mov oT0.xy, v1

// outputs
// oPos		post projection clip space postion
// oT0		texture uvs

/*
vs_1_1
// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_texcoord0	v1		// Stage 0 - texture coordinates

// constant inputs
// c0-c3				object to clip matrix

// code
// Transform vertex position from object to view space -> A must for all vertex shaders to oPos
	m4x4 r1, v0, c0
	mov oPos, r1
// copy texture uvs over to oT0
	mov oT0.xy, v1

// outputs
// oPos		post projection clip space postion
// oT0		texture uvs
*/