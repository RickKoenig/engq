echo off
ctr_VertexShaderAssembler32 %1.vsh -nowarning
ctr_VertexShaderLinker32 %1.obj  -Obin\%1.shbin
del *.obj
