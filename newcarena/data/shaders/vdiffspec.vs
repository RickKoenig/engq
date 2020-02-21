//The vertex shader, diffuse/specular light done in vertex shader, light calcs done in object space
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_normal		v1		// normals in object space
dcl_texcoord0	v2		// Stage 0 - diffuse texture coordinates

// constant inputs
// c0-c3				object to clip matrix
// c4					dirlight0 direction in object space
// c5					dirlight0 color
// c11					eye positon in object space
// c12					amb color
// c13.x				spec power
def c20,1,1,0,1
// code
// Transform vertex position from object to view space -> A must for all vertex shaders to do
	m4x4 r1, v0, c0
	mov oPos, r1
// copy texture uvs over to oT0
	mov oT0.xy, v2
// dot light with normal into r0.x
	dp3 r0.x,-v1,c4
// get eye vector from eye pos and object vertex, object space into r1
	sub r1,v0,c11
	nrm r2,r1
// add light vector to get the half vector into r5
	add r2,r2,c4
	nrm r5,r2
// dot half with normal into r4.x
	dp3 r4.x,-v1,r5
// setup for lit
	mov r2.x,r0.x // n*l
	mov r2.y,r4.x // n*h
	mov r2.w,c13.x // spec power
// get diff val into r3.y, spec pow val into r3.z
	lit r3,r2
// modulate with dirlight0 color with n*l diffuse into r1
	mul r1,c5,r3.yyyy
// add in amb color to oD0
	mov r1.w,c20.x
	add oD0,c12,r1
//	mov oD0.x,c20.z;
//	mov oD0.y,c20.z;
//	mov oD0.z,c20.z;
//	mov oD0.w,c20.x;
// copy spec color over, just white
	mov r3,r3.zzzz
	mov r3.w,c20.z
	mov oD1,r3

// outputs
// oPos		post projection clip space postion
// oT0		texture uvs
// oD0		amb/diffuse color
// oD1		spec color







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