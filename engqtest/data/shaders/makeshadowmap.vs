// The vertex shader, shadowmap maker
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_texcoord0	v1

// constant inputs
// c0-c3				object to clip matrix

// code
// Transform vertex position from object to view space -> A must for all vertex shaders to oPos
	m4x4 r1, v0, c0
	mov oPos, r1
	mov oT0,v1
// copy position zw over to oT0.xy
	mov oT1.xy,r1.zw

// outputs
// oPos		post projection clip space postion
// oT0		uv's
// oT1		vertex postion in light clip space, just z and w stored in x and y
