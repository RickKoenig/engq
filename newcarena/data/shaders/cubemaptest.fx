// test cubemap
#define FRONT
//#define BACK
//#define RIGHT
//#define LEFT
//#define TOP
//#define BOTTOM
// test cubemap orientations

// Global variables
shared float4x4 g_mWorldViewProjection;		// World * View * Projection matrix 'o2c'
//shared float4x4 g_mWorld;					// World matrix 'o2w'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh
//shared float4 g_eyeposworld;				// eye position in world space

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
    float4 Position	: POSITION;			// vertex position 
    float3 norms	: TEXCOORD0;		// norms
    //float3 eye		: TEXCOORD1;		// eye
};
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float2 uv : TEXCOORD0 )
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
// Transform the position to world space
   //Output.eye = mul(vPos, g_mWorld);
	//Output.eye = normalize(Output.eye - g_eyeposworld);
// copy normals over to world space
	float3 inorms;
	inorms.xy = vPos.xy * 2.0;
	inorms.z = 1.0;
// rotate cubemap
#ifdef FRONT
	Output.norms = inorms;
#endif
#ifdef BACK
	Output.norms.x = -inorms.x;
	Output.norms.y = inorms.y;
	Output.norms.z = -inorms.z;
#endif
#ifdef RIGHT
	Output.norms.x = inorms.z;
	Output.norms.y = inorms.y;
	Output.norms.z = -inorms.x;
#endif
#ifdef LEFT
	Output.norms.x = -inorms.z;
	Output.norms.y = inorms.y;
	Output.norms.z = inorms.x;
#endif
#ifdef TOP
	Output.norms.x = inorms.x;
	Output.norms.y = inorms.z;
	Output.norms.z = -inorms.y;
#endif
#ifdef BOTTOM
	Output.norms.x = inorms.x;
	Output.norms.y = -inorms.z;
	Output.norms.z = inorms.y;
#endif
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
// checkerboard pattern
	float3 multi = 4.*In.norms;
	multi = frac(multi);
	multi = step(.5,multi);
	//if (multi > .5)
	//	Output.RGBColor = float4(1.,0.,0.,1.);
    //else
		Output.RGBColor = texCUBE(MeshTextureSampler, In.norms) * float4(multi,1.);
    Output.RGBColor *= g_material_color;
    //Output.RGBColor = g_material_color;
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
