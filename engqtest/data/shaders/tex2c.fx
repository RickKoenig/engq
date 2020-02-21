// Main fx, for debprint fonts, software mode will do a direct blit instead

// All the Global variables from every fx
shared texture g_MeshTexture;				// Color texture for mesh
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// foreground material color
shared float4 g_material_color2;			// background material color

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
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
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
// use texture to pick between 2 different material colors
    float v = tex2D(MeshTextureSampler, In.TextureUV);
    if (v>=.5)
	    Output.RGBColor = g_material_color;
    else
	    Output.RGBColor = g_material_color2;
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
