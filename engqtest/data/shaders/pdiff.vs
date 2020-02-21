//The vertex shader, diffuse light done in pixel shader, light calcs done in object space
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in clip/world space
dcl_normal		v1		// normals
dcl_texcoord0	v2		// Stage 1 - diffuse texture coordinates

// constant inputs
// c0-c3	object to clip matrix

// code
// Transform vertex position from object to view space -> A must for all vertex shaders
	m4x4 oPos, v0, c0
// copy texture uvs over
	mov oT0.xy, v2
// copy normals over
	mov oT1.xyz, v1

// outputs
// oPos		post projection clip space
// oT0		uvs
// oT1		norms







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