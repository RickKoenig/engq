struct bitmap8 {
	struct pointi2 size;
	struct recti2 cliprect;
	U8* data;
};

// palette colors, 8 bit
enum	{black,blue,green,cyan,
		red,magenta,brown,lightgray,
		darkgray,lightblue,lightgreen,lightcyan,
		lightred,lightmagenta,yellow,white};

struct bitmap8* bitmap8alloc(S32 x,S32 y,S32 v);
void bitmap8free(struct bitmap8*);

/////////////////////// pixels ////////////////////////////////

U8 fastgetpixel8(struct bitmap8* p,S32 x0,S32 y0);
//U8 clipgetpixel8(struct bitmap8* p,S32 x0,S32 y0);
void fastputpixel8(struct bitmap8* p,S32 x0,S32 y0,U8 color);
//void clipputpixel8(struct bitmap8* p,S32 x0,S32 y0,U8 color);
void fastline8(struct bitmap8 *,int x0,int y0,int x1,int y1,int color);

extern C32 stdpalette[256];

void clipfloodfill8(struct bitmap8* b8,S32 mx,S32 my,S32 newcol);
void clipline8(struct bitmap8* b8,S32 x0,S32 y0,S32 x1,S32 y1,S32 col);
void fastrect8(struct bitmap8* b8,S32 x0,S32 y0,S32 wid,S32 hit,S32 col);
//void fastmask8(struct bitmap8 *s,struct bitmap8 *d,int sx,int sy,int dx,int dy,int tx,int ty,unsigned char *andor);

void clipstencil8(struct bitmap8 *win,struct bitmap8 *priority_bitmap,struct bitmap8 *scn_bitmap,int sx,int sy,int dx,int dy,int tx,int ty,int id);
void cliprect8(struct bitmap8 *,int x0,int y0,int wid,int hit,int color);
void cliprecto8(struct bitmap8 *,int x0,int y0,int wid,int hit,int color);
void clipputpixel8(struct bitmap8 *,int x0,int y0,int color);
U8 clipgetpixel8(struct bitmap8 *,int x0,int y0);
void cliphline8(struct bitmap8 *,int x0,int y0,int x1,int color);
void clipvline8(struct bitmap8 *,int x0,int y0,int y1,int color);
void clipline8(struct bitmap8 *,int x0,int y0,int x1,int y1,int color);
void fastblit8(const struct bitmap8 *s, const struct bitmap8 *d, int sx, int sy, int dx, int dy, int tx, int ty);
void clipblit8(const struct bitmap8 *s, const struct bitmap8 *d, int sx, int sy, int dx, int dy, int tx, int ty);
void clipmask8(struct bitmap8 *s,struct bitmap8 *d,unsigned char *andor);
void clipxpar8(struct bitmap8 *s,struct bitmap8 *d,int sx,int sy,int dx,int dy,int tx,int ty,int xpar);
void clipbgpri8(struct bitmap8 *s,struct bitmap8 *d,int sx,int sy,int dx,int dy,int tx,int ty,int xpar);
void clipchangeblit8(struct bitmap8 *n,struct bitmap8 *o,struct bitmap8 *d);

bool bclip8(const struct bitmap8 *s,const struct bitmap8 *d,int *sx,int *sy,int *dx,int *dy,int *tx,int *ty);
int clipputpixeltest8(struct bitmap8 *b,int x0,int y0);
int cliplinetest8(struct bitmap8 *b,int *x0,int *y0,int *x1,int *y1);

/*void clipscalexpar8(struct bitmap8 *src,struct bitmap8 *dst,
				   int ssx,int ssy,
				   int sex,int sey,
				   int dsx,int dsy,
				   int dex,int dey,int xpar);
#define clipscaleblit8(src,dst,ssx,ssy,sex,sey,dsx,dsy,dex,dey) \
	clipscalexpar8((src),(dst),(ssx),(ssy),(sex),(sey),(dsx),(dsy),(dex),(dey),-1)
*/
void clipscaleblit8(const struct bitmap8* src,struct bitmap8 *dst);
