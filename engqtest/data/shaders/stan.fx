// like Stan in monkey island
// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh

shared float3 g_dirlightdir;				// dirlight object space
shared float4 g_dirlightcol;

shared float4 g_amblightcol;				// amblight

shared float g_toonedgesize;				// for toon toonedge
shared float g_stanx;
shared float g_stany;						// stan effect, see monkey island

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
    float3 normal		: TEXCOORD0;	// normal
    float4 uvs			: TEXCOORD1;	// tex uvs
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL)
{
    VS_OUTPUT Output;
    
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
    
// Just copy the normal through
	Output.normal = -norm;
	Output.uvs = Output.Position;
	Output.uvs.x *= g_stanx;
	Output.uvs.y *= g_stany;
    
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
    float2 uv=In.uvs/In.uvs.w;
    float4 c = tex2D(MeshTextureSampler, uv);
	c = .5*c + .5; 
    Output.RGBColor *= c;
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
