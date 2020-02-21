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

//shared float4 g_lightpos;					// new position of a light, object space

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
    float3 norms : TEXCOORD1;	// norms
    float3 halves : TEXCOORD2;	// half vectors
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
    
// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 

// copy normals over
	Output.norms = norm;
	
// get half vectors over
    float3 eyedir = normalize(vPos.xyz - g_eyeposobj);
    Output.halves = normalize(g_dirlightdir.xyz + eyedir.xyz);
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
	float3 nrms = normalize(In.norms.xyz);
	float3 halfs = normalize(In.halves.xyz);
    Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV);
//    Output.RGBColor = lerp(
//		tex2D(MeshTextureSampler, In.TextureUV),
//		tex2D(MeshTextureSampler2, In.TextureUV),
//		g_specpow.yyyy); // the blend factor is in y
    float ndl,ndh,pow;
    ndl = dot(nrms,-g_dirlightdir.xyz);
    ndh = dot(nrms,-halfs);
// litret.y is diff
// litret.z is spec
    float4 litret=lit(ndl,ndh,g_specpow.x);
    Output.RGBColor.xyz *= g_material_color.xyz * (g_amblightcol + g_dirlightcol * litret.yyy);
//    Output.RGBColor.xyz *= g_material_color.xyz * (g_amblightcol + g_dirlightcol * g_lightpos * litret.yyy);
    Output.RGBColor.w *= g_material_color.w;
    Output.RGBColor.xyz += litret.zzzz*g_specpow.w;

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
