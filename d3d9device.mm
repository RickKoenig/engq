<map version="1.0.1">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1222489174076" ID="Freemind_Link_342653302" MODIFIED="1437892084927" TEXT="dx9 code for engq">
<node CREATED="1222490965675" FOLDED="true" ID="Freemind_Link_1096169223" MODIFIED="1437892088365" POSITION="right" TEXT="m_vid_dx9">
<node CREATED="1222489947584" HGAP="29" ID="Freemind_Link_1403308686" MODIFIED="1437891901065" TEXT="misc" VSHIFT="71">
<node CREATED="1222490814538" MODIFIED="1222490814538" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(49):        videoinfodx9.d3d9device-&gt;Present( NULL, NULL, NULL, NULL );"/>
<node CREATED="1222490814539" ID="Freemind_Link_439205654" MODIFIED="1222490944668" TEXT="&#x9;&#x9;&#x9;result= (videoinfodx9.d3d9)-&gt;CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)wininfo.MainWindow,&#xa;&#x9;&#x9;&#x9;&#x9;D3DCREATE_HARDWARE_VERTEXPROCESSING,&amp;videoinfodx9.pp, (LPDIRECT3DDEVICE9*)(&amp;videoinfodx9.d3d9device)) ;&#xa;"/>
<node CREATED="1222490814540" ID="Freemind_Link_46978466" MODIFIED="1222490913400" TEXT="&#x9;&#x9;&#x9;result= (videoinfodx9.d3d9)-&gt;CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)wininfo.MainWindow,&#xa;&#x9;&#x9;&#x9;&#x9;D3DCREATE_SOFTWARE_VERTEXPROCESSING,&amp;videoinfodx9.pp, (LPDIRECT3DDEVICE9*)(&amp;videoinfodx9.d3d9device)) ;&#xa;"/>
<node CREATED="1222490814537" MODIFIED="1222490814537" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(36):        videoinfodx9.d3d9device-&gt;Release();"/>
<node CREATED="1222490814547" ID="Freemind_Link_872123776" MODIFIED="1222490814547" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(189):        HRESULT hr=videoinfodx9.d3d9device-&gt;GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&amp;bb);"/>
</node>
<node CREATED="1222489925418" ID="Freemind_Link_1570233966" MODIFIED="1437891888061" TEXT="setrenderstate" VSHIFT="-5">
<node CREATED="1222490814542" ID="Freemind_Link_1391979759" MODIFIED="1222490814542" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(145):        videoinfodx9.d3d9device-&gt;SetRenderState( D3DRS_LIGHTING, FALSE );"/>
<node CREATED="1222490814542" ID="ID_1079937018" MODIFIED="1222490814542" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(147):        videoinfodx9.d3d9device-&gt;SetRenderState( D3DRS_ZENABLE ,videoinfodx9.zbuffertype);"/>
</node>
<node CREATED="1222490008660" FOLDED="true" ID="Freemind_Link_1862093936" MODIFIED="1437891870092" TEXT="setsamplerstate">
<node CREATED="1222490814543" ID="Freemind_Link_973435231" MODIFIED="1222490814543" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(148):        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222490814544" MODIFIED="1222490814544" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(149):        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222490814544" MODIFIED="1222490814544" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(150):        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222490814545" MODIFIED="1222490814545" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(151):        videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222490814546" MODIFIED="1222490814546" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(152):        videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222490814546" MODIFIED="1222490814546" TEXT="C:\srcw32\engq\engq\m_vid_dx9.cpp(153):        videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );"/>
</node>
</node>
<node CREATED="1222491013362" ID="Freemind_Link_763201330" MODIFIED="1508309047965" POSITION="right" TEXT="d2_dx9">
<node CREATED="1222489925418" ID="Freemind_Link_161654115" MODIFIED="1437891908456" TEXT="setrenderstate" VSHIFT="-5">
<node CREATED="1222492160072" MODIFIED="1222492160072" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(937):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHABLENDENABLE ,true);"/>
<node CREATED="1222492160072" MODIFIED="1222492160072" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(938):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHATESTENABLE, true);"/>
<node CREATED="1222492160072" MODIFIED="1222492160072" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(940):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHABLENDENABLE ,false);"/>
<node CREATED="1222492160073" MODIFIED="1222492160073" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(941):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHATESTENABLE, false);"/>
<node CREATED="1222492160060" ID="Freemind_Link_704398851" MODIFIED="1222492160060" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(489):        videoinfodx9.d3d9device-&gt;SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );"/>
<node CREATED="1222492160060" MODIFIED="1222492160060" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(490):        videoinfodx9.d3d9device-&gt;SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );"/>
<node CREATED="1222492160060" MODIFIED="1222492160060" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(492):        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHAREF, ac);"/>
<node CREATED="1222492160061" MODIFIED="1222492160061" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(493):        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);"/>
<node CREATED="1222492160061" MODIFIED="1222492160061" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(494):        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);"/>
<node CREATED="1222492160061" MODIFIED="1222492160061" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(495):        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_CULLMODE,videoinfodx9.cullmode);"/>
<node CREATED="1222492160056" MODIFIED="1222492160056" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(449):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&amp;videoinfodx9.fog_density));"/>
<node CREATED="1222492160056" MODIFIED="1222492160056" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(453):                        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGENABLE, FALSE);"/>
<node CREATED="1222492160056" MODIFIED="1222492160056" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(454):                        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);"/>
<node CREATED="1222492160057" ID="Freemind_Link_1763956937" MODIFIED="1222492160057" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(468):                    videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGCOLOR, Color);"/>
<node CREATED="1222492160057" MODIFIED="1222492160057" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(473):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGVERTEXMODE, Mode);"/>
<node CREATED="1222492160058" MODIFIED="1222492160058" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(474):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&amp;Start));"/>
<node CREATED="1222492160058" MODIFIED="1222492160058" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(475):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&amp;End));"/>
<node CREATED="1222492160058" MODIFIED="1222492160058" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(477):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGVERTEXMODE, Mode);"/>
<node CREATED="1222492160059" MODIFIED="1222492160059" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(478):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&amp;Density));"/>
<node CREATED="1222492160074" ID="Freemind_Link_1138731762" MODIFIED="1222492160074" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(966):                                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZENABLE,TRUE);"/>
<node CREATED="1222492160075" MODIFIED="1222492160075" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(967):                                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZWRITEENABLE,TRUE);"/>
<node CREATED="1222492160075" MODIFIED="1222492160075" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(969):                                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZENABLE,FALSE);"/>
<node CREATED="1222492160075" MODIFIED="1222492160075" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(970):                                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZWRITEENABLE,FALSE);"/>
<node CREATED="1222492160052" ID="Freemind_Link_1952175734" MODIFIED="1222492160052" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(425):                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZENABLE,TRUE);"/>
<node CREATED="1222492160052" MODIFIED="1222492160052" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(426):                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZWRITEENABLE,TRUE);"/>
<node CREATED="1222492160052" MODIFIED="1222492160052" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(428):                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZENABLE,FALSE);"/>
<node CREATED="1222492160053" MODIFIED="1222492160053" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(429):                hr=videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_ZWRITEENABLE,FALSE);"/>
<node CREATED="1222492160054" ID="Freemind_Link_297486786" MODIFIED="1222492160054" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(440):                    videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGENABLE, TRUE);"/>
<node CREATED="1222492160054" MODIFIED="1222492160054" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(442):                    videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGCOLOR, Color);"/>
<node CREATED="1222492160055" MODIFIED="1222492160055" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(444):                        videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGTABLEMODE, videoinfodx9.fog_mode);"/>
<node CREATED="1222492160055" MODIFIED="1222492160055" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(446):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&amp;videoinfodx9.fog_start));"/>
<node CREATED="1222492160055" MODIFIED="1222492160055" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(447):                                videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&amp;videoinfodx9.fog_end));"/>
<node CREATED="1222492160079" ID="Freemind_Link_878796057" MODIFIED="1222492160079" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1049):                    videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGENABLE, TRUE);"/>
<node CREATED="1222492160080" MODIFIED="1222492160080" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1051):                    videoinfodx9.d3d9device-&gt;SetRenderState(D3DRS_FOGENABLE, FALSE);"/>
</node>
<node CREATED="1222489899860" FOLDED="true" ID="Freemind_Link_86879770" MODIFIED="1437891848389" TEXT="setvertexshaderconstant">
<node CREATED="1222492160065" MODIFIED="1222492160065" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(844):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(14,(float*)&amp;d2_dx9::worldEyePos,1);"/>
<node CREATED="1222492160066" ID="Freemind_Link_1157087536" MODIFIED="1222492160066" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(848):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(11,(float*)&amp;localEyePos,1);"/>
<node CREATED="1222492160066" ID="Freemind_Link_316265305" MODIFIED="1222492160066" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(852):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(12,(float*)&amp;lightinfo.ambcolor,1);"/>
<node CREATED="1222492160067" MODIFIED="1222492160067" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(856):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(4,(float*)&amp;locallightdirs[0], 1);"/>
<node CREATED="1222492160067" MODIFIED="1222492160067" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(860):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(5,(float*)&amp;lightinfo.lightcolors[0], 1);"/>
<node CREATED="1222492160069" ID="Freemind_Link_1355418605" MODIFIED="1222492160069" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(892):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(0,(float*)&amp;o2ct, 4);"/>
<node CREATED="1222492160069" MODIFIED="1222492160069" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(895):        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(7,(float*)&amp;o2wt, 4);"/>
<node CREATED="1222492160069" MODIFIED="1222492160069" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(902):                hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(16,(float*)&amp;o2lct, 4);"/>
<node CREATED="1222492160076" MODIFIED="1222492160076" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(982):                        hr=videoinfodx9.d3d9device-&gt;SetVertexShaderConstantF(13,sp,1);"/>
</node>
<node CREATED="1222490351500" FOLDED="true" HGAP="21" ID="Freemind_Link_270744599" MODIFIED="1437891852326" TEXT="setpixelshaderconstant" VSHIFT="-3">
<node CREATED="1222492160067" ID="Freemind_Link_1502494386" MODIFIED="1222492160067" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(865):        hr=videoinfodx9.d3d9device-&gt;SetPixelShaderConstantF(5,(float*)&amp;lightinfo.ambcolor,1);"/>
<node CREATED="1222492160068" MODIFIED="1222492160068" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(869):        hr=videoinfodx9.d3d9device-&gt;SetPixelShaderConstantF(1,(float*)&amp;locallightdirs[0],1);"/>
<node CREATED="1222492160068" MODIFIED="1222492160068" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(872):        hr=videoinfodx9.d3d9device-&gt;SetPixelShaderConstantF(2,(float*)&amp;lightinfo.lightcolors[0],1);"/>
<node CREATED="1222492160077" ID="Freemind_Link_137003378" MODIFIED="1222492160077" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(986):                        hr=videoinfodx9.d3d9device-&gt;SetPixelShaderConstantF(8,sp,1);"/>
<node CREATED="1222492160077" ID="Freemind_Link_214426682" MODIFIED="1222492160077" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(997):                hr=videoinfodx9.d3d9device-&gt;SetPixelShaderConstantF(6,(float*)&amp;mc,1);"/>
<node CREATED="1222492160077" MODIFIED="1222492160077" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1005):                hr=videoinfodx9.d3d9device-&gt;SetPixelShaderConstantF(9,(float*)&amp;mc2,1);"/>
</node>
<node CREATED="1222490041524" HGAP="26" ID="Freemind_Link_1028894752" MODIFIED="1437891912753" TEXT="settexture" VSHIFT="1">
<node CREATED="1222492160078" ID="Freemind_Link_542462953" MODIFIED="1222492160078" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1015):                                hr=videoinfodx9.d3d9device-&gt;SetTexture(0,t-&gt;texdata);"/>
<node CREATED="1222492160078" MODIFIED="1222492160078" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1019):                        hr=videoinfodx9.d3d9device-&gt;SetTexture(1,g_pShadowMap);"/>
<node CREATED="1222492160078" MODIFIED="1222492160078" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1026):                                hr=videoinfodx9.d3d9device-&gt;SetTexture(0,g_pShadowMap);"/>
<node CREATED="1222492160079" MODIFIED="1222492160079" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1031):                                        hr=videoinfodx9.d3d9device-&gt;SetTexture(0,t-&gt;texdata);"/>
<node CREATED="1222492160079" MODIFIED="1222492160079" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1038):                                hr=videoinfodx9.d3d9device-&gt;SetTexture(1,t-&gt;texdata);"/>
<node CREATED="1222492160079" MODIFIED="1222492160079" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1042):                                hr=videoinfodx9.d3d9device-&gt;SetTexture(1,0);"/>
</node>
<node CREATED="1222490008660" ID="Freemind_Link_449261469" MODIFIED="1437892071739" TEXT="setsamplerstate">
<node CREATED="1222492160070" ID="Freemind_Link_628514959" MODIFIED="1222492160070" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(903):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_POINT );"/>
<node CREATED="1222492160070" MODIFIED="1222492160070" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(904):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_POINT );"/>
<node CREATED="1222492160070" MODIFIED="1222492160070" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(905):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_POINT );"/>
<node CREATED="1222492160070" MODIFIED="1222492160070" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(906):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_ADDRESSU ,D3DTADDRESS_BORDER);"/>
<node CREATED="1222492160071" MODIFIED="1222492160071" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(907):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_ADDRESSV ,D3DTADDRESS_BORDER);"/>
<node CREATED="1222492160080" MODIFIED="1222492160080" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1059):                        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_WRAP);"/>
<node CREATED="1222492160081" MODIFIED="1222492160081" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1060):                        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_WRAP);"/>
<node CREATED="1222492160081" MODIFIED="1222492160081" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1066):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222492160081" MODIFIED="1222492160081" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1067):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222492160082" ID="ID_1268724785" MODIFIED="1222492160082" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1068):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );"/>
<node CREATED="1222492160082" MODIFIED="1222492160082" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1069):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_ADDRESSU ,D3DTADDRESS_WRAP);"/>
<node CREATED="1222492160082" MODIFIED="1222492160082" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1070):                videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_ADDRESSV ,D3DTADDRESS_WRAP);"/>
<node CREATED="1222492160053" ID="Freemind_Link_972999965" MODIFIED="1222492160053" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(431):        videoinfodx9.d3d9device-&gt;SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS ,*(DWORD *)(&amp;videoinfodx9.miplodbias));"/>
<node CREATED="1222492160054" MODIFIED="1222492160054" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(432):        videoinfodx9.d3d9device-&gt;SetSamplerState(1,D3DSAMP_MIPMAPLODBIAS ,*(DWORD *)(&amp;videoinfodx9.miplodbias));"/>
<node CREATED="1222492160048" ID="Freemind_Link_1008170233" MODIFIED="1222492160048" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(351):        videoinfodx9.d3d9device-&gt;SetSamplerState( 1, D3DSAMP_BORDERCOLOR ,C32WHITE.c32);"/>
<node CREATED="1222492160076" ID="Freemind_Link_1511368116" MODIFIED="1222492160076" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(974):                        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_ADDRESSU ,D3DTADDRESS_CLAMP);"/>
<node CREATED="1222492160076" ID="Freemind_Link_148249674" MODIFIED="1222492160076" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(975):                        videoinfodx9.d3d9device-&gt;SetSamplerState( 0, D3DSAMP_ADDRESSV ,D3DTADDRESS_CLAMP);"/>
</node>
<node CREATED="1222493545713" FOLDED="true" ID="_" MODIFIED="1437891849717" TEXT="texture">
<node CREATED="1222492160083" ID="Freemind_Link_1755042031" MODIFIED="1222492160083" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1277):                        result=videoinfodx9.d3d9device-&gt;CreateTexture( br-&gt;size.x,br-&gt;size.y,1,"/>
<node CREATED="1222492160083" ID="Freemind_Link_541280512" MODIFIED="1222492160083" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1257):                        result=videoinfodx9.d3d9device-&gt;CreateCubeTexture(cs,1,"/>
<node CREATED="1222492160047" ID="Freemind_Link_1898125197" MODIFIED="1222492160047" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(313):        HRESULT hr=videoinfodx9.d3d9device-&gt;CreateTexture( SHADOWMAP_SIZE, SHADOWMAP_SIZE,"/>
</node>
<node CREATED="1222493594041" ID="Freemind_Link_444721035" MODIFIED="1437891940957" TEXT="shaders">
<node CREATED="1222492160072" ID="Freemind_Link_1128734459" MODIFIED="1222492160072" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(919):                hr=videoinfodx9.d3d9device-&gt;SetPixelShader(ss.ps);"/>
<node CREATED="1222492160071" ID="Freemind_Link_1667275623" MODIFIED="1222492160071" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(916):                hr=videoinfodx9.d3d9device-&gt;SetVertexShader(ss.vs);"/>
<node CREATED="1222492160046" MODIFIED="1222492160046" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(119):                hr=videoinfodx9.d3d9device-&gt;CreateVertexDeclaration(vdecls[i].ve, &amp;vdecls[i].vd);"/>
<node CREATED="1222492160046" MODIFIED="1222492160046" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(154):                hr=videoinfodx9.d3d9device-&gt;CreateVertexShader((DWORD*)pShaderBuffer-&gt;GetBufferPointer(),&amp;ss.vs);"/>
<node CREATED="1222492160074" MODIFIED="1222492160074" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(960):                        hr=videoinfodx9.d3d9device-&gt;SetStreamSource(0,vb,0,vds.sizeofvertex);"/>
<node CREATED="1222492160074" MODIFIED="1222492160074" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(957):                        hr=videoinfodx9.d3d9device-&gt;SetVertexDeclaration(vds.vd);"/>
<node CREATED="1222492160073" MODIFIED="1222492160073" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(952):                                hr=videoinfodx9.d3d9device-&gt;SetPixelShader(ss.ps);"/>
<node CREATED="1222492160073" ID="Freemind_Link_1771803304" MODIFIED="1222492160073" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(949):                                hr=videoinfodx9.d3d9device-&gt;SetVertexShader(ss.vs);"/>
</node>
<node CREATED="1222493613128" ID="Freemind_Link_668641519" MODIFIED="1437891936582" TEXT="buffers">
<node CREATED="1222492160063" ID="Freemind_Link_501125817" MODIFIED="1222492160063" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(558):    hr=videoinfodx9.d3d9device-&gt;SetRenderTarget( 0, pOldRT );"/>
<node CREATED="1222492160062" ID="Freemind_Link_904801580" MODIFIED="1222492160062" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(522):        hr=videoinfodx9.d3d9device-&gt;GetRenderTarget( 0, &amp;pOldRT );"/>
<node CREATED="1222492160047" ID="Freemind_Link_447977389" MODIFIED="1222492160047" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(179):                hr=videoinfodx9.d3d9device-&gt;CreatePixelShader((DWORD*)pShaderBuffer-&gt;GetBufferPointer(),&amp;ss.ps);"/>
<node CREATED="1222492160048" MODIFIED="1222492160048" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(348):                videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, C32WHITE.c32, 1, 0 );"/>
<node CREATED="1222492160050" MODIFIED="1222492160050" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(398):                        videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_TARGET, c.c32, 1, 0 );"/>
<node CREATED="1222492160047" ID="Freemind_Link_1106425004" MODIFIED="1222492160047" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(327):     hr=videoinfodx9.d3d9device-&gt;CreateDepthStencilSurface( SHADOWMAP_SIZE,"/>
<node CREATED="1222492160050" MODIFIED="1222492160050" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(396):                        videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_ZBUFFER, 0, 1, 0 );"/>
<node CREATED="1222492160050" MODIFIED="1222492160050" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(394):                        videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, c.c32, 1, 0 );"/>
<node CREATED="1222492160051" MODIFIED="1222492160051" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(418):                videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, c.c32, 1, 0 );"/>
<node CREATED="1222492160051" MODIFIED="1222492160051" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(420):                videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_ZBUFFER, 0, 1, 0 );"/>
<node CREATED="1222492160051" MODIFIED="1222492160051" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(422):                videoinfodx9.d3d9device-&gt;Clear( 0,0, D3DCLEAR_TARGET, c.c32, 1, 0 );"/>
<node CREATED="1222492160062" MODIFIED="1222492160062" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(531):        hr=videoinfodx9.d3d9device-&gt;SetRenderTarget( 0, pShadowSurf );"/>
<node CREATED="1222492160063" ID="Freemind_Link_1233864493" MODIFIED="1222492160063" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(537):        hr=videoinfodx9.d3d9device-&gt;GetDepthStencilSurface( &amp;pOldDS );"/>
<node CREATED="1222492160063" ID="Freemind_Link_1601192577" MODIFIED="1222492160063" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(541):        hr=videoinfodx9.d3d9device-&gt;SetDepthStencilSurface( g_pDSShadow );"/>
<node CREATED="1222492160063" ID="Freemind_Link_261759796" MODIFIED="1222492160063" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(553):        hr=videoinfodx9.d3d9device-&gt;SetDepthStencilSurface( pOldDS );"/>
</node>
<node CREATED="1222493581214" ID="Freemind_Link_621581585" MODIFIED="1437891933129" TEXT="mesh">
<node CREATED="1222492160080" ID="Freemind_Link_1690002222" MODIFIED="1222492160080" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(1053):                hr=videoinfodx9.d3d9device-&gt;DrawIndexedPrimitive(D3DPT_TRIANGLELIST,"/>
<node CREATED="1222492160071" ID="Freemind_Link_202374281" MODIFIED="1222492160071" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(909):        hr=videoinfodx9.d3d9device-&gt;SetIndices(ib);"/>
<node CREATED="1222492160065" MODIFIED="1222492160065" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(756):        HRESULT hr=videoinfodx9.d3d9device-&gt;CreateVertexBuffer(nbytes,"/>
<node CREATED="1222492160064" MODIFIED="1222492160064" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(702):        HRESULT hr=videoinfodx9.d3d9device-&gt;CreateVertexBuffer(nbytes,"/>
<node CREATED="1222492160064" MODIFIED="1222492160064" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(624):        HRESULT hr=videoinfodx9.d3d9device-&gt;CreateVertexBuffer(nbytes,"/>
<node CREATED="1222492160064" MODIFIED="1222492160064" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(660):        HRESULT hr=videoinfodx9.d3d9device-&gt;CreateVertexBuffer(nbytes,"/>
<node CREATED="1222492160065" MODIFIED="1222492160065" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(787):                hr=videoinfodx9.d3d9device-&gt;CreateIndexBuffer(4*3*nf,"/>
<node CREATED="1222492160065" ID="Freemind_Link_1611459603" MODIFIED="1222492160065" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(808):                hr=videoinfodx9.d3d9device-&gt;CreateIndexBuffer(2*3*nf,"/>
</node>
<node CREATED="1222493567116" ID="Freemind_Link_1808043843" MODIFIED="1437891923144" TEXT="viewport">
<node CREATED="1222492160048" MODIFIED="1222492160048" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(347):                videoinfodx9.d3d9device-&gt;SetViewport(&amp;vp9);"/>
<node CREATED="1222492160051" ID="Freemind_Link_1684748633" MODIFIED="1222492160051" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(416):        videoinfodx9.d3d9device-&gt;SetViewport(&amp;vp9);"/>
</node>
<node CREATED="1222493961364" ID="Freemind_Link_92596640" MODIFIED="1437891925863" TEXT="scene">
<node CREATED="1222492160049" ID="Freemind_Link_1115727279" MODIFIED="1222492160049" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(382):                videoinfodx9.d3d9device-&gt;BeginScene();"/>
<node CREATED="1222492160050" ID="Freemind_Link_1512524635" MODIFIED="1222492160050" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(400):                        videoinfodx9.d3d9device-&gt;BeginScene();"/>
<node CREATED="1222492160062" ID="Freemind_Link_1508255255" MODIFIED="1222492160062" TEXT="C:\srcw32\engq\engq\d2_dx9.cpp(506):        videoinfodx9.d3d9device-&gt;EndScene();"/>
</node>
</node>
</node>
</map>
