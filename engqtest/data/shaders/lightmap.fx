// Main fx

// All the Global variables from every fx
shared texture g_MeshTexture;				// Color texture for mesh
shared texture g_MeshTexture2;				// Color texture for mesh
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color

// Texture samplers
sampler MeshTextureSampler0 = 
sampler_state
{
    Texture = <g_MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
sampler MeshTextureSampler1 = 
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
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV0  : TEXCOORD0;  // vertex texture coords 
    float2 TextureUV1  : TEXCOORD1;  // vertex texture coords 
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float2 vTexCoord0 : TEXCOORD0, float2 vTexCoord1 : TEXCOORD1 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
// Just copy the texture coordinate through
    Output.TextureUV0 = vTexCoord0; 
    Output.TextureUV1 = vTexCoord1; 
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
//	modulate texture with material color
	float4 tex1 = tex2D(MeshTextureSampler0, In.TextureUV0);
	float4 tex2 = tex2D(MeshTextureSampler1, In.TextureUV1);
	Output.RGBColor =  tex1 * (tex2*3) +.25;
//	Output.RGBColor *= g_material_color;
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
