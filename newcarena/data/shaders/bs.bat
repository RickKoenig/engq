fxc /T fx_2_0 /Fc tex.asm tex.fx
fxc /T fx_2_0 /Fo tex.fxo tex.fx
fxc /T fx_2_0 /Fc cvert.asm cvert.fx
fxc /T fx_2_0 /Fo cvert.fxo cvert.fx
rem long filenames seem to cause fxc.exe to crash (sigh)
fxc /T fx_2_0 /Fc poo.asm useshadowmap.fx
fxc /T fx_2_0 /Fo poo.fxo useshadowmap.fx
