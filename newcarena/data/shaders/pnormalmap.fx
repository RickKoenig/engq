// test texture blending also
// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh
shared texture g_MeshTexture2;				// Color texture for mesh

shared float4 g_amblightcol;				// amblight

shared float3 g_dirlightdir;				// normalized dirlight object space
shared float4 g_dirlightcol;


shared float4 g_eyeposobj;					// eye position in object space
shared float4 g_specpow;					// specular power value, texblend, shadowepsilon

// Texture samplers
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
sampler MeshTextureSampler2 = 
sampler_state
{
    Texture = <g_MeshTexture2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// Vertex Shader
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position homogeneous clip space
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
    float3 litdir : TEXCOORD1;	// lightdir tan space
    float3 halves : TEXCOORD2;	// half vectors tan space
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
							float3 norm : NORMAL,
							float3 tangent : TANGENT,
							float3 binormal : BINORMAL,
							float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
    
// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 

// get eye vector from eye pos and object vertex, object space into eye, then get half vector obj space
	float3 eye = normalize(vPos.xyz - g_eyeposobj.xyz);
	float3 hlf = normalize(g_dirlightdir + eye);
	
// put light into tan space
//	tangent = float3(0,0,0);
//	binormal = float3(0,0,0);
	Output.litdir.x = dot(tangent,g_dirlightdir);
	Output.litdir.y = dot(binormal,g_dirlightdir);
	Output.litdir.z = dot(norm,g_dirlightdir);
//	Output.litdir.z = -1;
	

// put half into tan space
	Output.halves.x = dot(tangent,hlf);
	Output.halves.y = dot(binormal,hlf);
	Output.halves.z = dot(norm,hlf);

#if 0	
// get eye vector from eye pos and object vertex, object space into r2
	sub r1,v0,c11
	nrm r2,r1
// add light vector to get the half vector into r5
	add r2,r2,c4
	nrm r5,r2
// output light vector to oT1
	dp3 r0.x,c4,v2
	dp3 r0.y,c4,v3
	dp3 r0.z,c4,v1
	mov oT1.xyz,r0
// output half vector to oT2
	dp3 r0.x,r5,v2
	dp3 r0.y,r5,v3
	dp3 r0.z,r5,v1
	mov oT2.xyz,r0
#endif
// get half vectors over
//    float3 eyedir = normalize(vPos.xyz - g_eyeposobj);
//    Output.halves = normalize(g_dirlightdir.xyz + eyedir.xyz);
//    Output.litdir = 0;
    return Output;    
}
#if 0
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
	m4x4 oPos,v0,c0
// copy texture uvs over
	mov oT0.xy,v4
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
	dp3 r0.x,c4,v2
	dp3 r0.y,c4,v3
	dp3 r0.z,c4,v1
	mov oT1.xyz,r0
// output half vector to oT2
	dp3 r0.x,r5,v2
	dp3 r0.y,r5,v3
	dp3 r0.z,r5,v1
	mov oT2.xyz,r0
// new outputs
// oPos		post projection clip space
// oT0		uvs
// oT1		light in tan space
// oT2		half vectors in tan space
#endif

// Pixel Shader
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
    float3 nrms = tex2D(MeshTextureSampler2,In.TextureUV) * 2 - 1;
//  float3 nrms = {0,0,1};
//	float3 nrms = normalize(norms.xyz);
	float3 halfs = normalize(In.halves.xyz);
    Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV); 
    float ndl,ndh,pow;
    float3 ld = normalize(In.litdir);
    ndl = dot(nrms,-ld);
    ndh = dot(nrms,-halfs);
// litret.y is diff
// litret.z is spec
    float4 litret=lit(ndl,ndh,g_specpow.x);
    Output.RGBColor.xyz *= g_material_color.xyz * (g_amblightcol + g_dirlightcol * litret.yyy);
    Output.RGBColor.w *= g_material_color.w;
    Output.RGBColor.xyz += litret.zzzz;
//	Output.RGBColor = float4(1,0,0,1);
    return Output;
}


technique RenderScene
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderSceneVS();
        PixelShader  = compile ps_2_0 RenderScenePS(); 
    }
}
