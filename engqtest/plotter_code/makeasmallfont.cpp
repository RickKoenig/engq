#define MAKESMALLFONT
#ifdef MAKESMALLFONT
void makesmallfont()
{
	pushandsetdir("c:/download");
	bitmap32* fo = bitmap32alloc(64,128,C32BLACK);
	int i,j;
	for (j=0;j<16;++j)
		for (i=0;i<8;++i)
			outchar32(fo,i*8,j*8,C32WHITE,i+j*8);
	for (j=0;j<128;++j) {
		for (i=0;i<64;++i) {
			C32 v = clipgetpixel32(fo,i,j);
			if (v == C32BLACK) {
				//v = 0;
				clipputpixel32(fo,i,j,v);
			} else {
				clipputpixel32(fo,i,j,C32WHITE);
			}
		}
	}
	gfxwrite32("smallfont.png",fo);
	bitmap32free(fo);
	popdir(); // back to data/plotter
}
#endif

