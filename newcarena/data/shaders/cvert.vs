//The vertex shader, colored verts
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_color		v1		// Vertex colors
dcl_texcoord0	v2		// Stage 0 - texture coordinates

// constant inputs
// c0-c3				object to clip matrix

// code
	m4x4 oPos, v0, c0
	mov oT0, v2
// Transform vertex position from object to view space -> A must for all vertex shaders to oPos
	m4x4 r1, v0, c0
	mov oPos, r1
// copy texture uvs over to oT0
	mov oT0.xy, v2
// copy colored vert over to oD0
	mov oD0,v1

// outputs
// oPos		post projection clip space postion
// oT0		texture uvs
// oD0		colored vert color

