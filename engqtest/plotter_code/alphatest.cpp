//#define ALPHATEST
#ifdef ALPHATEST
//#define REDUCERES 128
#define REDUCERES 8192

void alphatest(const C8* pic)
{
	bitmap32* b = gfxread32(pic);
	logger("pic '%s', (%d,%d)\n",pic,b->size.x,b->size.y);
	C32* ptr = b->data;
	U32 p = b->size.x*b->size.y;
	vector<U32> tally(256);
	U32 i;
	for (i=0;i<p;++i)
		++tally[ptr[i].a];
	bitmap32free(b);
	for (i=0;i<256;++i)
		if (tally[i])
			logger("val %4d: cnt %5d\n",i,tally[i]);
}

void doalphatest()
{
	pushandsetdir("c:/xampp/htdocs");
	alphatest("engw/engw3dtest/fortpoint/treesclip2.png");
	alphatest("engw/engw3dtest/fortpoint/wood1.png");
	alphatest("engw/common/sptpics/maptestnck.png");
	popdir();
}

#endif
