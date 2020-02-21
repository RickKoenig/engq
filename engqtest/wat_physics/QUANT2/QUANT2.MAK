.asm.obj:
        wasm -d1 $*.asm 

.c.obj:	.AUTODEPEND
      wcc386 -zp4 -5s -otexan -7 -d2 -mf -oi -fp5 $*.c

quant2.exe: quant2.obj fastquan.obj &
	quant2.mak \watcom\lib386\dos\graph32.lib
	wlink @quant2.lnk
