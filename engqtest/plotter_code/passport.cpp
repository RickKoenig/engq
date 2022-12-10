//#define PASSPORT
//#define PALETTE_DPAINT2
#define DOPCXHEATHOUSE
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
#ifdef PALETTE_DPAINT2
// read in a snipped palette, sample, scale down to 16 by 16, 1 pixel per palette entry
void doPalette()
{
	bitmap32 *b = gfxread32("dpaint2PaletteLarge.png");
	bitmap32 *bs = bitmap32alloc(16, 16);
	const S32 offx = 14;
	const S32 offy = 16;
	const S32 stepx = 16;
	const S32 stepy = 10;
	for (S32 j = 0; j < 16; ++j) {
		const S32 posy = offy + stepy * j;
		for (S32 i = 0; i < 16; ++i) {
			const S32 posx = offx + stepx * i;
			const C32 val = clipgetpixel32(b, posx, posy);
			clipputpixel32(bs, j, i, val);
		}
	}
	gfxwrite32("dpaint2Palette.png", bs);
	bitmap32free(b);
	bitmap32free(bs);
}
#endif
#ifdef DOPCXHEATHOUSE
// 0 to 255 on significant red channel, tracers on green and blue channels for 'show index'
C32 indexToColor32(U32 idx)
{
	U8 r = idx;
	U32 r16 = idx * 16;
	U8 g = r16 & 0xff;
	U8 b = (r16 >> 4) & 0xff;
	C32 ret = C32(r, g, b);
	return ret;
}

void dopcxheathouse()
{
	logger("DO HEAT HOUSE!\n");
	pushandsetdir("heathouse");
	C32 dacs[256];
	bitmap8* roomsBm8 = gfxread8("rooms.pcx", dacs);
	U8* roomsBm8Data = roomsBm8->data;

	bitmap32* palBm32 = bitmap32alloc(16, 16);
	C32* palBm32Data = palBm32->data;
	for (S32 k = 0; k < 256; ++k) {
		palBm32Data[k] = dacs[k];
	}
	gfxwrite32("roomsPal.png", palBm32);
	bitmap32free(palBm32);

	U32 sx = roomsBm8->size.x;
	U32 sy = roomsBm8->size.y;
	S32 prod = sx * sy;
	bitmap32* idxBm32 = bitmap32alloc(sx, sy);
	C32* idxBm32Data = idxBm32->data;
	for (S32 k = 0; k < prod; ++k) {
		idxBm32Data[k] = indexToColor32(roomsBm8Data[k]);
	}
	gfxwrite32("roomsIdx.png", idxBm32);
	bitmap32free(idxBm32);
	bitmap8free(roomsBm8);
	popdir();
}
#endif


