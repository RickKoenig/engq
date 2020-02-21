// Main fx

// All the Global variables from every fx
shared texture g_MeshTexture;				// Color texture for mesh
shared texture g_MeshTexture2;				// Shadow texture
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color

shared float4x4 g_o2lcmat;					// object to light projection matrix
shared float4 g_specpow;					// specular power value, texblend, shadowepsilon
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

sampler MeshTextureSampler2 = 
sampler_state
{
    Texture = <g_MeshTexture2>;
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
    float4 Position		: POSITION;   // vertex position 
    float2 TextureUV	: TEXCOORD0;  // vertex texture coords 
	float4 lcpos		: TEXCOORD1;  // light clip position
	float3 norms		: TEXCOORD2;
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float2 vTexCoord0 : TEXCOORD0,
                         float3 norms : NORMAL )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
// Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
// copy object position in light clip space to lcpos
    Output.lcpos = mul(vPos, g_o2lcmat);
// copy normals over
    Output.norms = norms; 
    return Output;    
}

float2 cm = {.5,-.5};
float2 ca = {.5,.5};

// Pixel Shader
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
// try something better later... also add mat color
    float rw = 1/In.lcpos.w;	// 1/w
//	float thez = In.lcpos.z;		// z
	float3 truepos = rw * In.lcpos; // normalized
	float2 smi = cm * truepos + ca; // smi 0,0 1,1 from -1,1 1,-1  y flip
	float shadowz = tex2D(MeshTextureSampler2,smi);
	float r2x = shadowz;
	float r0z = truepos.z; // normalized z
	r2x += g_specpow.z	; // add z epsilon, (offset)
	r2x = r0z - r2x; // compare z's
	float3 nrm = normalize(In.norms);
	float lv = saturate(dot(-g_dirlightdir,nrm));
	float3 lcol = lv * g_dirlightcol;
	lcol = saturate(lcol + g_amblightcol);
	if (r2x>0)
		lcol = g_amblightcol;
// modulate texture with shadow
    Output.RGBColor = tex2D(MeshTextureSampler,In.TextureUV);
    Output.RGBColor.xyz *= lcol;
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
