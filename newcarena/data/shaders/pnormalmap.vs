//The vertex shader, diffuse/specular light done in pixel shader, light calcs done in object space
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_normal		v1		// normals in object space
dcl_tangent		v2		// the tangent
dcl_binormal	v3		// the binormal
dcl_texcoord0	v4		// Stage 1 - diffuse texture coordinates

// constant inputs
// c0-c3	object to clip matrix
// c4		dirlight0 direction in object space
// c11		eye positon in object space

// code
// Transform vertex position from object to view space -> A must for all vertex shaders
#define TRANSPOSE
#ifdef TRANSPOSE
	m4x4 oPos,v0,c0
#else
	mul r0,c0,v0.xxxx
	mad r0,c1,v0.yyyy,r0
	mad r0,c2,v0.zzzz,r0
	mad r0,c3,v0.wwww,r0
	mov oPos,r0
#endif
// copy texture uvs over
	mov oT0.xy,v4
#if 1
// copy normals over
//	mov oT1.xyz,v1
//	mov	r0,v2
//	mov	r0,v3
// get eye vector from eye pos and object vertex, object space into r2
	sub r1,v0,c11
	nrm r2,r1
// add light vector to get the half vector into r5
	add r2,r2,c4
	nrm r5,r2
// output light vector to oT1
//	mul r0,v2,c4.xxxx
//	mad r0,-v3,c4.yyyy,r0
//	mad r0,v1,c4.zzzz,r0
	dp3 r0.x,c4,v2
	dp3 r0.y,c4,v3
	dp3 r0.z,c4,v1
	mov oT1.xyz,r0
// output half vector to oT2
//	mul	r0,v2,r5.xxxx
//	mad r0,-v3,r5.yyyy,r0
//	mad r0,v1,r5.zzzz,r0
	dp3 r0.x,r5,v2
	dp3 r0.y,r5,v3
	dp3 r0.z,r5,v1
	mov oT2.xyz,r0
#endif
// new outputs
// oPos		post projection clip space
// oT0		uvs
// oT1		light in tan space
// oT2		half vectors in tan space

// old outputs
// oPos		post projection clip space
// oT0		uvs
// oT1		norms
// oT2		half vectors







// c0 - c3 -> has the view * projection matrix
// c14 -> light position in world space
//def c4,0,0,-1,0
// dcl_texcoord1 v3  // Stage 2 - normal map texture coordinates
// dcl_tangent v4    // Tangent vector
// dcl_binormal v5   // Binormal vector
// dcl_normal v6     // Normal vector


// Calculate the light vector
// mov r1, c14
// sub r3.xyz, r1, v0

// Convert the light vector to tangent space
// m3x3 r1.xyz, r3, v4

// Normalize the light vector
// nrm r3, r1
// mov r3,v1

// Move data to the output registers
// mov oT1.xyz, r3

//mov oT1.xyz,c4

/*
vs_1_1
dcl_position v0
dcl_normal  v1
dcl_texcoord  v2
def c92,0,0,-1,0	// negative directional light vector
m4x4 oPos, v0, c0
mov oT0,v2
dp3 oD0,v1,c92
//mov oD0,c92
//mov oD0,c92
*/