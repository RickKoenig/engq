//#define NEEDLE
#ifdef NEEDLE
void alterneedle(bitmap32 *b)
{
	S32 i,prod=b->size.x*b->size.y;
	C32* p = b->data;
	U32 bc = 0xc8;
	for (i=0;i<prod;++i) {
		C32 v = p[i];
		if (v.r >= bc && v.g >= bc && v.b >= bc) {
			v = C32(0,0,0,0);
		}
		p[i] = v;
	}
}
void doneedle()
	{
	pushandsetdir("c:/download");
	bitmap32 *b = gfxread32("needle.jpg");
	alterneedle(b);
	gfxwrite32("needle.png",b);
	bitmap32free(b);
	popdir();
}
#endif

