// Fluttering flag vertex shader

// These lines are just for EffectEdit:
string XFile = "flag.x";   // model
int    BCLR = 0xff000000;          // background colour

texture Tex0 < string name = "EuropeFlag.jpg"; >;

// transformations provided by the app as input:
float4x4 matWorldViewProj: WORLDVIEWPROJECTION;
float4x4 matWorld  : WORLD;
float time: TIME;

struct VS_OUTPUT
{
    float4 Pos : POSITION;
    float2 tex : TEXCOORD0;
};

VS_OUTPUT VS(float4 Pos : POSITION,float2 tex : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0; 

    float angle=(time%360)*2;

    Pos.z  = sin( Pos.x+angle);
    Pos.z += sin( Pos.y/2+angle);
    Pos.z *= Pos.x * 0.09f;

    // transform Position
    Out.Pos = Out.Pos + mul(Pos , matWorldViewProj); 

    // Just pass the texture co-ordinate as is
    Out.tex = tex; 

    return Out;
}

// Effect technique to be used
technique TVertexShaderOnly
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS();
        // texture
        Texture[0] = (Tex0);

        // sampler states
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = POINT;

        // set up texture stage states for single texture modulated by diffuse 
        ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = CURRENT;
        AlphaOp[0]   = DISABLE;

        ColorOp[1]   = DISABLE;
        AlphaOp[1]   = DISABLE;
    }  
}

