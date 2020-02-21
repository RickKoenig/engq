// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
shared float4x4 g_mWorld;					// World matrix 'o2w'
shared float4 g_eyeposworld;				// eye position in world space
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh

shared float3 g_dirlightdir;				// dirlight object space
shared float4 g_dirlightcol;

shared float4 g_amblightcol;				// amblight

shared float g_toonedgesize = .1;			// for toon toonedge

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
    float3 onormal		: TEXCOORD0;	// obj normal 0
    float3 wnormal		: TEXCOORD1;	// world normal 1
    float2 TextureUV0	: TEXCOORD2;	// vertex texture coords 0
//	float2 TextureUV1	: TEXCOORD3;	// vertex texture coords 1
    float3 eye			: TEXCOORD3;	// eye vector
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float3 norm : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0, float2 vTexCoord1 : TEXCOORD1)
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
//    Output.Position.x += Output.Position.z * .9;
//    Output.Position.y += Output.Position.z;

// Transform the position to world space
    Output.eye = mul(vPos, g_mWorld);
	Output.eye = normalize(Output.eye - g_eyeposworld);

// Just copy the texture coordinate through
    Output.TextureUV0 = vTexCoord0; 
//    Output.TextureUV1 = vTexCoord1; 
//    Output.TextureUV = 0;
    
// Just copy the normal through
// copy normals over to world space
	Output.wnormal = mul(norm,g_mWorld);
	Output.onormal = -norm;
    
// do the per vertex lighting calculation
/*	float d = saturate(dot(-norm,g_dirlightdir));
	Output.vertcolor.xyz = d;
	Output.vertcolor.w = 1;
	Output.vertcolor = g_material_color * (g_amblightcol + g_dirlightcol * Output.vertcolor); */
//	Output.Position /= Output.Position.w; // try to remove pers corrrection
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
    float3 onn = normalize(In.onormal);
    float3 wnn = normalize(In.wnormal);
    float d = dot(onn,g_dirlightdir);
	Output.RGBColor.xyz = saturate(d); // is normalize necessary?
#if 1
	Output.RGBColor.xyz *= 4;
	Output.RGBColor.xyz = round(Output.RGBColor.xyz);
	Output.RGBColor.xyz *= .25;
#endif
	Output.RGBColor.xyz = g_material_color * (g_amblightcol + g_dirlightcol * Output.RGBColor); 
    Output.RGBColor.w = g_material_color.w;
    Output.RGBColor.xyz = saturate(Output.RGBColor);
    float4 c = tex2D(MeshTextureSampler, In.TextureUV0);
/*    if (In.TextureUV1.x<g_toonedgesize || 
		In.TextureUV1.y<g_toonedgesize || 
		In.TextureUV1.x+In.TextureUV1.y > 1-g_toonedgesize)
		c = 0;
	else */
//		c = .5*c + .5; 
	float d2 = dot(wnn,In.eye);
//	float d2 = nn.z;
    if (d2>-.25 && d2<.25)
//		c=0;//frac(d2*10);
		Output.RGBColor = 0;
	else 
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
