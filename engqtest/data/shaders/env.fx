// test texture blending also
// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4x4 g_mWorld;					// World matrix 'o2w'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh
shared float4 g_eyeposworld;				// eye position in world space

// Texture samplers
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// Vertex Shader
struct VS_OUTPUT
{
    float4 Position	: POSITION;			// vertex position 
    float3 norms	: TEXCOORD0;		// norms
    float3 eye		: TEXCOORD1;		// eye
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
// Transform the position to world space
	//vPos.w = 0.;
    Output.eye = mul(vPos, g_mWorld);
//	Output.eye = normalize(Output.eye - g_eyeposworld); // buggy
	Output.eye = Output.eye - g_eyeposworld; // not so buggy
// copy normals over to world space
	Output.norms = mul(norm,g_mWorld);
// done
    return Output;    
}

// Pixel Shader
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
	float3 nrms = normalize(In.norms);
	float3 eye = normalize(In.eye);
	float3 coord = eye - 2*dot(nrms,eye) * nrms;
    Output.RGBColor = texCUBE(MeshTextureSampler, coord);
    Output.RGBColor *= g_material_color;
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
