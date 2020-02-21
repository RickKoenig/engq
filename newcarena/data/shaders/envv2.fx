// merge of pdiffspec and env (envv2), a mix of obj and world
// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4x4 g_mWorld;					// World matrix 'o2w'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh
shared float4 g_eyeposworld;				// eye position in world space
shared float4 g_amblightcol;				// amblight

shared float3 g_dirlightdir;				// normalized dirlight object space
shared float4 g_dirlightcol;


shared float4 g_eyeposobj;					// eye position in object space
shared float4 g_specpow;					// specular power value, texblend, shadowepsilon

shared texture g_envv2tex;				// Color texture for mesh

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
    Texture = <g_envv2tex>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// Vertex Shader
struct VS_OUTPUT
{
    float4 Position	: POSITION;			// vertex position 
    float2 TextureUV: TEXCOORD0;		// vertex texture coords 
    float3 normso	: TEXCOORD1;		// obj norms for pdiffspec
    float3 normsw	: TEXCOORD2;		// world norms for env
    float3 eyew		: TEXCOORD3;		// eye
    float3 halves	: TEXCOORD4;		// halves
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);

// Transform the position to world space
    Output.eyew = mul(vPos, g_mWorld);
	Output.eyew = Output.eyew - g_eyeposworld;

// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 

// copy normals over to world space
	Output.normsw = mul(norm,g_mWorld);

// copy normals over to object space
	Output.normso = norm;

// get half vectors over
    float3 eyedir = normalize(vPos.xyz - g_eyeposobj);
    Output.halves = normalize(g_dirlightdir.xyz + eyedir.xyz);

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
    float4 colds,cole;
	float3 nrmsw = normalize(In.normsw);
	float3 eye = normalize(In.eyew);
	float3 coord = eye - 2*dot(nrmsw,eye) * nrmsw;
    cole = texCUBE(MeshTextureSampler2, coord);
    cole *= g_material_color;

	float3 nrmso = normalize(In.normso.xyz);
	float3 halfs = normalize(In.halves.xyz);
    colds = tex2D(MeshTextureSampler, In.TextureUV);
    float ndl,ndh;
    ndl = dot(nrmso,-g_dirlightdir.xyz);
    ndh = dot(nrmso,-halfs);
// litret.y is diff
// litret.z is spec
    float4 litret=lit(ndl,ndh,g_specpow.x);
    colds.xyz *= g_material_color.xyz * (g_amblightcol + g_dirlightcol * litret.yyy);
    colds.w *= g_material_color.w;
    colds.xyz += litret.zzzz*g_specpow.w; // spec, comment out for no spec
//  Output.RGBColor.xyzw = float4(0,0,0,0);
    
	Output.RGBColor.xyz = lerp(colds.xyz,cole.xyz,g_specpow.yyy);
//	Output.RGBColor.xyz = colds.xyz;
//	Output.RGBColor.xyz = cole.xyz;
    Output.RGBColor.w = colds.w;

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
