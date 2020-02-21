// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared texture g_MeshTexture;				// Color texture for mesh
shared float4 g_material_color;				// material color

shared float4 g_amblightcol;				// amblight

shared float3 g_dirlightdir;				// dirlight object space
shared float4 g_dirlightcol;

shared float4 g_specpow;					// specular power value, texblend, shadowepsilon


shared float4 g_eyeposobj;					// eye position in object space

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
    float4 vertcolor  : COLOR; // diff vertex color
    float3 vertcolor2  : TEXCOORD1; // spec vertex color
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
    float ndl,ndh,pow;
    float3 eyedir = normalize(vPos.xyz - g_eyeposobj);
    float3 halfr = normalize(g_dirlightdir.xyz + eyedir.xyz);
    ndl = dot(norm,-g_dirlightdir.xyz);
    ndh = dot(norm,-halfr);
    float4 litret=lit(ndl,ndh,g_specpow.x);
//    litret.y=ndl; // diffuse
//    litret.z=1; // specular
//	float d = saturate(dot(-norm,g_dirlightdir));
	Output.vertcolor.xyz = g_material_color.xyz * (g_amblightcol + g_dirlightcol * litret.yyy); 
	Output.vertcolor.w = g_material_color.w;
	Output.vertcolor2 = litret.zzz;
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
    Output.RGBColor.xyz += In.vertcolor2.xyz;
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
