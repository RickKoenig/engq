// Main fx

// All the Global variables from every fx
shared texture g_MeshTexture;				// Color texture for mesh
shared float4x4 g_mWorldViewProjection;		// Object * World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color

// Texture samplers
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;
#if 1
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
#else
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
#endif
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
// modulate texture with material color
    Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV);
    Output.RGBColor *= g_material_color;
  //Output.RGBColor.r = 1;
  //Output.RGBColor.g = 0;
  //Output.RGBColor.b = 1;
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
