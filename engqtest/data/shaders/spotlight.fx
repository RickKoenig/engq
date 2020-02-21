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
shared float4 g_specpow;					// NOW setup is: specpower,not_used(reflect?),shadowepsilon,specbrightness       WAS: specpower,texblend,shadowepsilon
											// so g_specpow.x is the power value and g_specpow.w is the amount of specular to use

shared float4 g_lightpos;					// new position of a light, object space

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
    float4 opos : TEXCOORD3; // object position rel to light pos
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
    Output.opos = vPos - g_lightpos;
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
    Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV); // start with output just the texture
//    Output.RGBColor = lerp(
//		tex2D(MeshTextureSampler, In.TextureUV),
//		tex2D(MeshTextureSampler2, In.TextureUV),
//		g_specpow.yyyy); // the blend factor is in y
    float ndl,ndh,pow;
    ndl = dot(nrms,-g_dirlightdir.xyz);
    ndh = dot(nrms,-halfs);
// litret.y is diff
// litret.z is spec
	float rad2 = dot(In.opos.xyz,In.opos.xyz);
	const float radiusi = 3;
	const float radiuso = 7.55;
	float stp = smoothstep(-radiuso*radiuso,-radiusi*radiusi,-rad2);
	float3 nrmpos = normalize(In.opos.xyz);
	float ldp = dot(g_dirlightdir.xyz,nrmpos);
	stp *= step(.7071,ldp); // if stp is 1 then your inside the light, 0 otherwise

    float4 litret=lit(ndl,ndh,g_specpow.x); // ndl is n dot l, ndh is n dot h
    // ....... what the 'lit' intrinsic function does ........
    // ambient  litret.x = 1; // always 1
    // diffuse  litret.y = max(0,ndl); // value, 0 to 1
    // specular litret.z = 0 if ndl<0 or ndh<0 otherwise it's  ndh^g_specpow.x  same as pow(ndh,g_specpow.x) ; // specular value, 0 to 1
	litret.yz *= stp; // kill diffuse and specular light if outside the light, keep ambient light though
    Output.RGBColor.xyz *= g_material_color.xyz * (g_amblightcol + g_dirlightcol * litret.yyy); // mix ambient and diffuse colors
    Output.RGBColor.w *= g_material_color.w; // alpha from material copied straight over
//    Output.RGBColor.xyz += litret.zzzz*g_specpow.w + stp*.25*g_amblightcol; // white on other side
// finally add in the specular, maybe not add that g_amblightcol though
    Output.RGBColor.x += litret.zzzz*g_specpow.w + stp*.25*g_amblightcol; // red on other side, comment out to stop doing other side
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
