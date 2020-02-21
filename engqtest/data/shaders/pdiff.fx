// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh

shared float3 g_dirlightdir;				// normalized dirlight object space
shared float4 g_dirlightcol;

shared float4 g_amblightcol;				// amblight

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
    float4 Position		: POSITION;		// vertex position 
    float2 TextureUV	: TEXCOORD0;	// vertex texture coords 
    float3 normal		: TEXCOORD1;	// normal
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
    
// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
// Just copy the normal through
	Output.normal = -norm;
    
// do the per vertex lighting calculation
    return Output;    
}

// Pixel Shader
struct PS_OUTPUT
{
    float4 RGBColor : COLOR;  // Pixel color    
};
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
	Output.RGBColor.xyz = saturate(dot(normalize(In.normal),g_dirlightdir)); // is normalize necessary?
	Output.RGBColor.xyz = g_material_color * (g_amblightcol + g_dirlightcol * Output.RGBColor); 
    Output.RGBColor.w = g_material_color.w;
    Output.RGBColor.xyz = saturate(Output.RGBColor);
    Output.RGBColor *= tex2D(MeshTextureSampler, In.TextureUV);
    
//     Output.RGBColor=float4(1,1,1,1);
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
