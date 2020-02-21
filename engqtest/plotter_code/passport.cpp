//#define PASSPORT
#ifdef PASSPORT
void scaledown(bitmap32 *b)
{
	const int up = 5;
	const int down = 8;
	bitmap32 *nb = bitmap32alloc(b->size.x * up/down,b->size.y * up/down,C32BLACK);
	clipscaleblit32(b,nb);
	clipclear32(b,C32WHITE);
	clipblit32(nb,b,0,0,(b->size.x-nb->size.x)/2,(b->size.y-nb->size.y)/2,nb->size.x,nb->size.y);
	bitmap32free(nb);
}
void dospassprot()
	{
	bitmap32 *b = gfxread32("c:/download/pics/xiaoxia-3.jpg");
	scaledown(b);
	bitmap32 *bigb = bitmap32alloc(b->size.x << 1, b->size.y << 1, C32WHITE);
	clipblit32(b,bigb,0,0,0,0,b->size.x,b->size.y);
	clipblit32(b,bigb,0,0,0,b->size.y,b->size.x,b->size.y);
	clipblit32(b,bigb,0,0,b->size.x,0,b->size.x,b->size.y);
	clipblit32(b,bigb,0,0,b->size.x,b->size.y,b->size.x,b->size.y);
	gfxwrite32("c:/download/pics/xiaoxia-3_2by2.png",bigb);
	bitmap32free(b);
	bitmap32free(bigb);
}
#endif

