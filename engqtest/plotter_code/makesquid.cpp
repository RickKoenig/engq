//#define MAKESQUID
#ifdef MAKESQUID
void makesquid()
{
	S32 ix = 128;
	S32 iy = 128;
	pushandsetdir("c:/download");
	bitmap32* o = gfxread32("squiddude.png");
	bitmap32* so = bitmap32alloc(ix,iy,C32BLACK);
	clipscaleblit32(o,so);
	bitmap32free(o);
	C32 ref = clipgetpixel32(so,0,0);
	int i,j;
	for (j=0;j<iy;++j) {
		for (i=0;i<ix;++i) {
			if (i>=74 && i<106 && j>=78 && j<110) { // keep the squid's eyes white color
			} else {
				C32 v = clipgetpixel32(so,i,j);
				int t = 190;
				if (v.r > t && v.g > t && v.b > t) { // make white transparent
					v.a = 0;
				}
				clipputpixel32(so,i,j,v);
			}

		}
	}
	gfxwrite32("newsquiddude.png",so);
	bitmap32free(so);
	popdir(); // back to data/plotter
}
#endif
