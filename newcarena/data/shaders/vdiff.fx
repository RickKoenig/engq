// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared texture g_MeshTexture;				// Color texture for mesh
shared float4 g_material_color;				// material color

shared float4 g_amblightcol;				// amblight

shared float3 g_dirlightdir;				// dirlight object space
shared float4 g_dirlightcol;


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
    float4 Position   : POSITION;   // vertex position 
    float4 vertcolor  : COLOR; // vertex color
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
    
// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
// do the per vertex lighting calculation
	float d = saturate(dot(-norm,g_dirlightdir));
	Output.vertcolor.xyz = d;
	Output.vertcolor = g_material_color * (g_amblightcol + g_dirlightcol * Output.vertcolor);
	Output.vertcolor.w = 1;
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
    Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV);
    Output.RGBColor *= In.vertcolor;
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
