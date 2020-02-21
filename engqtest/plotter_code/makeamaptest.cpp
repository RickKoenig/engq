//#define MAKEMAPTEST
#ifdef MAKEMAPTEST
void makemaptest()
{
	S32 ix = 128;
	S32 iy = 128;
	pushandsetdir("c:/download");
	bitmap32* o = gfxread32("maptestnck.tga");
	gfxwrite32("default.png",o);
	bitmap32free(o);
	popdir(); // back to data/plotter
}
#endif
