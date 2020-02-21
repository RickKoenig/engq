struct bitmap32 {
	struct pointi2 size;
	struct recti2 cliprect;
	C32* data;
};

struct bitmap32* bitmap32alloc(S32 x,S32 y,C32 v);
void bitmap32free(struct bitmap32*);

// discreet coords 0,0 to sizex-1,sizey-1, points at center of pixels
C32 fastgetpixel32(struct bitmap32*,S32 x0,S32 y0);
C32 clipgetpixel32(const bitmap32*,S32 x0,S32 y0);
void fastputpixel32(struct bitmap32*,S32 x0,S32 y0,C32 color);
void clipputpixel32(const struct bitmap32*,S32 x0,S32 y0,C32 color);
void fastline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,S32 y1,C32 color);
void clipline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,S32 y1,C32 color);
void fasthline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,C32 color);
void cliphline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,C32 color);
void fastvline32(struct bitmap32* b32,S32 x0,S32 y0,S32 y1,C32 color);
void clipvline32(struct bitmap32* b32,S32 x0,S32 y0,S32 y1,C32 color);
//void fastcircle32(struct bitmap32*,S32 x0,S32 y0,S32 rad,C32 color);
void clipcircle32(const struct bitmap32* b32,S32 x0,S32 y0,S32 rad,C32 color);
//void fastcircleo32(struct bitmap32*,S32 x0,S32 y0,S32 rad,C32 color);
void clipcircleo32(struct bitmap32* b32,S32 x0,S32 y0,S32 rad,C32 color);

// continuous coords 0,0 to sizex,sizey, points on corners of pixels
void fastrect32(const struct bitmap32*,S32 x0,S32 y0,S32 sx,S32 sy,C32 color);
void cliprect32(const struct bitmap32* b32,S32 x0,S32 y0,S32 sx,S32 sy,C32 color);
//void fastrecto32(struct bitmap32*,S32 x0,S32 y0,S32 sx,S32 sy,C32 color);
void cliprecto32(struct bitmap32* b32,S32 x0,S32 y0,S32 sx,S32 sy,C32 color);
void fastclear32(struct bitmap32 *source,C32 color);
void clipclear32(struct bitmap32 *source,C32 color);
bool bclip32(const struct bitmap32* s, const struct bitmap32* d, S32* sx, S32* sy, S32* dx, S32* dy, S32* tx, S32* ty);
void fastblit32(const bitmap32* s, struct bitmap32* d, S32 sx, S32 sy, S32 dx, S32 dy, S32 tx, S32 ty);
void clipblit32(const bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty);
void fastblit32alpha1(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty);
void clipblit32alpha1(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty);
void fastxpar32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 xpar);
void clipxpar32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 xpar);
void fastfore32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore);
void clipfore32(const struct bitmap32* s,const struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore);
void fastforeback32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore,C32 back);
void clipforeback32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore,C32 back);
//void fastscaleblit32(struct bitmap32* s,struct bitmap32* d,struct recti2* src,struct recti2* dest);
//void clipscaleblit32(struct bitmap32* s,struct bitmap32* d,struct recti2* src,struct recti2* dest);
//void fastscalexpar32(struct bitmap32* s,struct bitmap32* d,struct recti2* src,struct recti2* dest,C32 xpar);
//void clipscalexpar32(struct bitmap32* s,struct bitmap32* d,struct recti2* src,struct recti2* dest,C32 xpar);
void clipscaleblit32(const bitmap32* src,struct bitmap32* dst); // nice filter
bitmap32* bitmap32copy(const bitmap32* b); 
bitmap32* bitmap32double(struct bitmap32* b); // double in x and y, no filter
void bitmap32double(struct bitmap32* bi,struct bitmap32* bo);
bitmap32* bitmap32reduce(const bitmap32* b); // half in x and y, filter
void setcliprect32(struct bitmap32* s,S32 x,S32 y,S32 xs,S32 ys); // safe way of adjusting cliprect of a bitmap
void resetcliprect32(struct bitmap32* s);

struct con32 {
	C32 f,b;
	mutable S32 x,y;
	struct bitmap32* b32;
};
	
con32 *con32_alloc(S32 w,S32 h,C32 fc,C32 bc);
void con32_clear(struct con32* c);
struct bitmap32 *con32_getbitmap32(const struct con32* c);
void con32_printf(const struct con32* con,const C8* fmt,...);
void con32_printfvs(const struct con32* con,C8* fmt,va_list arglist);
void con32_free(struct con32* c);
void con32_printfnow(const struct con32* con,const C8* fmt,...);

extern struct con32* gcon;
void gcon_printf(const C8* fnt,...); // if a global con32 print to it, otherwise print to logger

// some kind of checkerboard patterns
void pattern1(bitmap32* b,int pattern);
void pattern2(bitmap32* b,int patternx,int patterny);

// turn opaque black to transparent alpha
void makeblackxpar(bitmap32* b,string name);

