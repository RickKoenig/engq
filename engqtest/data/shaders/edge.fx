// Global variables
shared float4x4 g_mWorldViewProjection;		// Object * World * View * Projection matrix 'o2c'
shared float4 g_material_color;				// material color
shared texture g_MeshTexture;				// Color texture for mesh

shared float3 g_dirlightdir;				// dirlight in object space
shared float4 g_dirlightcol;

shared float4 g_amblightcol;				// amblight

shared float g_edgesize;					// for edge
shared float g_edgezoff;					// for edge


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
    float4 Position	: POSITION;		// vertex position 
};
#if 0
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float4 vPos2 : TEXCOORD0)
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
	Output.Position /= Output.Position.w; // try a more intelligent approach later...
    float4 pos2 = mul(vPos2, g_mWorldViewProjection);
    pos2 /= pos2.w;
    float2 nrm;
    nrm.xy = pos2.xy-Output.Position.xy;
//    nrm.z = 0;
//    nrm.w = 0;
    if (dot(nrm.xy,nrm.xy)>.000001) // protect from degenerates
	    nrm.xy = normalize(nrm.xy);
    Output.Position.x -= nrm.y*g_edgesize; // 90 degrees
    Output.Position.y += nrm.x*g_edgesize;
    Output.Position.z -= g_edgezoff;
    return Output;    
}
#endif

VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, float4 vPos2 : TEXCOORD0)
{
    VS_OUTPUT Output;
// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
//	Output.Position /= Output.Position.w; // try a more intelligent approach later...
    float4 pos2 = mul(vPos2, g_mWorldViewProjection);
//  pos2 /= pos2.w;
    float2 nrm;
    nrm.xy = pos2.xy*Output.Position.w-Output.Position.xy*pos2.w;
    if (dot(nrm.xy,nrm.xy)>.00001) // protect from degenerates
	    nrm.xy = normalize(nrm.xy);
	else
		nrm.xy = 0;
	float es = g_edgesize*Output.Position.w;
	float zo = g_edgezoff*Output.Position.w;
    Output.Position.x -= nrm.y*es; // 90 degrees
    Output.Position.y += nrm.x*es;
    Output.Position.z -= zo;
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
    Output.RGBColor.rgba = g_material_color;
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
