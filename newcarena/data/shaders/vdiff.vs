//The vertex shader, diffuse light done in vertex shader, light calcs done in object space
vs_2_0

// vertex inputs
dcl_position	v0		// Vertex position in object space
dcl_normal		v1		// normals in object space
dcl_texcoord0	v2		// Stage 0 - diffuse texture coordinates

// constant inputs
// c0-c3				object to clip matrix
// c4					dirlight0 direction in object space
// c5					dirlight0 color
// c12					amb color
def c20,0,0,0,1			// clamp dot

// code
// Transform vertex position from object to view space -> A must for all vertex shaders to oPos
	m4x4 r1, v0, c0
	mov oPos, r1
// copy texture uvs over to oT0
	mov oT0.xy, v2
// dot light with normal into r0.x
	dp3 r0.x,-v1,c4
	max r0,c20,r0
// modulate with dirlight0 color to r1
	mul r1,c5,r0.xxxx
	mov r1.w,c20.w
// add in amb color to oD0
	add oD0,c12,r1

// outputs
// oPos		post projection clip space postion
// oT0		texture uvs
// oD0		amb/diffuse color
