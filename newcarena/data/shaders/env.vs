//The vertex shader
vs_2_0

// object vertex inputs
dcl_position	v0		// Vertex
dcl_normal		v1		// normals

// constant inputs
// c0-c3	object to clip matrix
// // c7-c10	object to world matrix
// c14		eye in world space

/*
// fog terms
// terms are
// x=near
// y=1/(far-near)
// z=far*y
// example near=10, far=110
//         x    y   z
def c20,  10,  .01,  1.1,  0
*/
// code
// Transform vertex position from object to view space -> A must for all vertex shaders
	m4x4 oPos, v0, c0
// postion in world space to r1
	m4x4 r1,v0,c7
//	mov r3,r1
// get eye vector from eye pos and object vertex, world space into r1, normalize into r2
	sub r1,r1,c14
	mov r3,r1 // save fog dist vector in r3
	nrm r2,r1
// normal to world space in r1
	m3x3 r0.xyz,v1,c7
	nrm r1,r0
// copy normal and eye over
	mov oT0.xyz, r1
	mov oT1.xyz, r2
	
/*
// try some fog, world space distance is in r3
// get distance from eye to vertex into r3
	dp3 r3,r3,r3
	rsq r3.x,r3.x
	rcp r3.x,r3.x
// use c20's parms
	mad r0.x,-c20.y,r3.x,c20.z
//	add r0.x,c20.z
	mov oFog,r0.x
*/
// outputs
// oPos		post projection clip space
// oT0		world normal vector
// oT1		world eye vector
// // oFog		fog factor, scalar 0 to 1
