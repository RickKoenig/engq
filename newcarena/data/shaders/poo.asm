
//listing of all techniques and passes with embedded asm listings 

technique RenderScene
{
    pass P0
    {
        vertexshader = 
            asm {
            //
            // Generated by Microsoft (R) D3DX9 Shader Compiler 
            //
            // Parameters:
            //
            //   float4x4 g_mWorldViewProjection;
            //   float4x4 g_o2lcmat;
            //
            //
            // Registers:
            //
            //   Name                   Reg   Size
            //   ---------------------- ----- ----
            //   g_mWorldViewProjection c0       4
            //   g_o2lcmat              c4       4
            //
            
                vs_2_0
                dcl_position v0
                dcl_texcoord v1
                dcl_normal v2
                dp4 oPos.x, v0, c0
                dp4 oPos.y, v0, c1
                dp4 oPos.z, v0, c2
                dp4 oPos.w, v0, c3
                dp4 oT1.x, v0, c4
                dp4 oT1.y, v0, c5
                dp4 oT1.z, v0, c6
                dp4 oT1.w, v0, c7
                mov oT0.xy, v1
                mov oT2.xyz, v2
            
            // approximately 10 instruction slots used
            };

        pixelshader = 
            asm {
            //
            // Generated by Microsoft (R) D3DX9 Shader Compiler 
            //
            // Parameters:
            //
            //   sampler2D MeshTextureSampler;
            //   sampler2D MeshTextureSampler2;
            //   float2 ca;
            //   float2 cm;
            //   float4 g_amblightcol;
            //   float4 g_dirlightcol;
            //   float3 g_dirlightdir;
            //   float4 g_specpow;
            //
            //
            // Registers:
            //
            //   Name                Reg   Size
            //   ------------------- ----- ----
            //   g_specpow           c0       1
            //   g_dirlightdir       c1       1
            //   g_dirlightcol       c2       1
            //   g_amblightcol       c3       1
            //   cm                  c4       1
            //   ca                  c5       1
            //   MeshTextureSampler  s0       1
            //   MeshTextureSampler2 s1       1
            //
            
                ps_2_0
                dcl t0.xy
                dcl t1
                dcl t2.xyz
                dcl_2d s0
                dcl_2d s1
                rcp r0.w, t1.w
                mul r0.xy, r0.w, t1
                mov r1.xy, c4
                mad r0.xy, r1, r0, c5
                texld r1, r0, s1
                texld r2, t0, s0
                add r0.x, r1.x, c0.z
                mad r0.x, r0.w, t1.z, -r0.x
                nrm r1.xyz, t2
                dp3_sat r0.y, -c1, r1
                mov r1.xyz, c3
                mad_sat r1.xyz, r0.y, c2, r1
                cmp r0.xyz, -r0.x, r1, c3
                mul r2.xyz, r2, r0
                mov oC0, r2
            
            // approximately 17 instruction slots used (2 texture, 15 arithmetic)
            };
    }
}

