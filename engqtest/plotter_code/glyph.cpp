//#define GLYPH
#ifdef GLYPH
// convert from 16 by 8 to 8 by 16
C8* cglyphnames[] = {
	"8x16font.png",
	"applefont-wide.png",
	"atarifont.png",
	"atarifont-retouched.png"
};
#define NCGLYPHNAMES sizeof(cglyphnames)/sizeof(cglyphnames[0])

C8* glyphnames[] = {
	"fontbiggreen.png",
	"smallfont.png"
	"8x16font_convert.png",
	"applefont-wide_convert.png",
	"atarifont_convert.png",
	"atarifont-retouched_convert.png"
};
#define NGLYPHNAMES sizeof(glyphnames)/sizeof(glyphnames[0])

void showglyph()
{
	S32 i;
	C8 e[50];
	C8 n[500];
// convert 16 by 8 to 8 by 16
	for (i=0;i<NCGLYPHNAMES;++i) {
		const C8* name = cglyphnames[i];
		mgetname(name,n);
		mgetext(name,e,50);
		C8 newname[500];
		sprintf(newname,"%s_convert.%s",n,e);
		logger("newname = '%s'\n",newname);
		bitmap32* b= gfxread32(name);
		logger("glyph %s = (%d,%d)\n",n,b->size.x,b->size.y);
		// convert 16 by 8 to 8 by 16
		bitmap32* nb = bitmap32alloc(b->size.x/2,b->size.y*2,C32LIGHTMAGENTA);
		S32 x,y;
		S32 gx,gy,fx,fy,mx,my,nx,ny;
		// size of tile, usually 16 by 32
		gx = b->size.x/16;
		gy = b->size.y/8;
		for (y=0;y<b->size.y;++y) {
			for (x=0;x<b->size.x;++x) {
				C32 v = clipgetpixel32(b,x,y);
				fx = x/gx; // which tile 0-15
				fy = y/gy; // 0-7
				mx = x%gx; // where on tile 0-15
				my = y%gy; // 0-31
				S32 nfx = fx%8;
				S32 nfy = fx/8 + 2*fy;
				nx = nfx*gx + mx;
				ny = nfy*gy + my;
				clipputpixel32(nb,nx,ny,v);
			}
		}
		//clipblit32(b,nb,0,0,0,0,b->size.x,b->size.y);
		gfxwrite32(newname,nb);
		bitmap32free(nb);
		bitmap32free(b);
	}
// visually separate the tiles
	for (i=0;i<NGLYPHNAMES;++i) {
		const C8* name = glyphnames[i];
		mgetname(name,n);
		mgetext(name,e,50);
		C8 newname[500];
		sprintf(newname,"%s_glyph.%s",n,e);
		logger("newname = '%s'\n",newname);
		bitmap32* b= gfxread32(name);
		logger("glyph %s = (%d,%d)\n",n,b->size.x,b->size.y);
		// break into tiles with small separation
		bitmap32* nb = bitmap32alloc(b->size.x+8+1,b->size.y+16+1,C32LIGHTMAGENTA);
		S32 x,y;
		S32 gx,gy,fx,fy,mx,my,nx,ny;
		gx = b->size.x/8;
		gy = b->size.y/16;
		for (y=0;y<b->size.y;++y) {
			for (x=0;x<b->size.x;++x) {
				C32 v = clipgetpixel32(b,x,y);
				fx = x/gx;
				fy = y/gy;
				mx = x%gx;
				my = y%gy;
				nx = 1 + fx*(gx+1) + mx;
				ny = 1 + fy*(gy+1) + my;
				clipputpixel32(nb,nx,ny,v);
			}
		}
		//clipblit32(b,nb,0,0,0,0,b->size.x,b->size.y);
		gfxwrite32(newname,nb);
		bitmap32free(nb);
		bitmap32free(b);
	}
}
#endif

//#define GLYPH2
#ifdef GLYPH2
// copy some chars from one font to some others
#define NSRC 5
S32 srcx[NSRC] = {
	0,
	3,
	5,
	6,
	7,
};
S32 srcy[NSRC] = {
	12,
	15,
	15,
	15,
	15,
};

C8* srcfont = "font1.png";

#define NDST 2
C8* dstfont[NDST] = {"font2.png","font3.png"};
C8* wrtfont[NDST] = {"font2s.png","font3s.png"};

void showglyph2()
{
	bitmap32* s = gfxread32(srcfont);
	S32 i,j;
	C32 cc = C32BLACK;
	for (j=0;j<NDST;++j) {
		bitmap32* a = gfxread32(dstfont[j]);
		cliprect32(a,0,0,128,128,cc);
		for (i=0;i<NSRC;++i) {
			S32 xo = srcx[i]*16;
			S32 yo = srcy[i]*32;
			cliprect32(a,xo,yo,16,32,cc);
			clipblit32(s,a,xo,yo,xo,yo,16,32);
		}
		gfxwrite32(wrtfont[j],a);
		bitmap32free(a);
	}
	bitmap32free(s);
}
#endif

//#define GLYPH3
#ifdef GLYPH3
// change last char to a checkerboard

C8* srcfont = "font3s.png";
C8* dstfont = "font3sw.png";

void showglyph3()
{
	bitmap32* a = gfxread32(srcfont);
	S32 i,j;
	// 127
	for (j=15*32;j<16*32;++j) {
		for (i=7*16;i<8*16;++i) {
			bool t = ((i&1)^(j&1)) == 0;
			C32 cc = t ? C32WHITE : C32BLACK;
			clipputpixel32(a,i,j,cc);
		}
	}
	// 24,25
	// a diagonal over 2 chars
	for (j=3*32;j<4*32;++j) {
		for (i=0*32;i<1*32;++i) {
			bool t = (i&31)==(j&31);
			C32 cc = t ? C32WHITE : C32BLACK;
			clipputpixel32(a,i,j,cc);
		}
	}
	// some binary patterns
	// 26,27
	for (j=0;j<32;++j) {
		for (i=0;i<16;++i) {
			bool t = ((i>>j)&1) != 0;
			C32 cc = t ? C32WHITE : C32BLACK;
			clipputpixel32(a,i+2*16,j+3*32,cc);
		}
	}
	for (j=0;j<32;++j) {
		for (i=0;i<16;++i) {
			bool t = ((j>>i)&1) != 0;
			C32 cc = t ? C32WHITE : C32BLACK;
			clipputpixel32(a,i+3*16,j+3*32,cc);
		}
	}
	gfxwrite32(dstfont,a);
	bitmap32free(a);
}
#endif

//#define GLYPH4
#ifdef GLYPH4
// change last char to a checkerboard

C8* srcfont = "font0.png";
C8* dstfont = "font0sw.png";

void showglyph4()
{
	bitmap32* a = gfxread32(srcfont);
	S32 i,j;
	// 127
	for (j=15*32;j<16*32;++j) {
		for (i=7*16;i<8*16;++i) {
			bool t = ((i&1)^(j&1)) == 0;
			C32 cc = t ? C32WHITE : C32BLACK;
			clipputpixel32(a,i,j,cc);
		}
	}
	gfxwrite32(dstfont,a);
	bitmap32free(a);
}
#endif

bitmap8* bitmap32to8(bitmap32* b32,C32* dacs)
{
	bitmap8* b8 = bitmap8alloc(b32->size.x,b32->size.y,0);
	//vector<C32> uc; // unique colors
	int ds = 0; // dac size
	S32 prod = b32->size.x*b32->size.y;
	S32 i;
	for (i=0;i<prod;++i) {
		C32 v32 = b32->data[i];
		U8& v8 = b8->data[i];
		//int idx;
		//vector<C32>::iterator it = find(uc.begin(),uc.end(),v32);
		int idx = find(&dacs[0],&dacs[ds],v32) - dacs;
		//idx = it - uc.begin();
		logger("idx = %d, size = %d\n",idx,ds);
		if (idx == ds) { // not found
			if (ds >= 256)
				errorexit("can't convert bitmap32to8, too many colors");
			dacs[ds++] = v32;
		}
		v8 = idx;
	}
	//dacs[0] = C32GREEN;
	return b8;
}

bitmap32* bitmap8to32(bitmap8* b8,const C32* dacs)
{
	bitmap32* b32 = bitmap32alloc(b8->size.x,b8->size.y,0);
	S32 prod = b8->size.x*b8->size.y;
	S32 i;
	for (i=0;i<prod;++i) {
		U8 v8 = b8->data[i];
		C32& v32 = b32->data[i];
		v32 = dacs[v8];
	}
	return b32;
}


// convert font3 from png to pcx
//#define GLYPH5
#ifdef GLYPH5
void showglyph5()
{
	C8* srcfont = "C:/nodejs/engw/common/sptpics/font3.png";
	C8* dstfont = "C:/rickerspc/dosprog/dpaint2/font3_new.pcx";
	bitmap32* a32 = gfxread32(srcfont);
	C32 dacs[256];
	bitmap8* a8 = bitmap32to8(a32,dacs);
	gfxwrite8(dstfont,a8,dacs);
	bitmap8free(a8);
	bitmap32free(a32);
}
#endif

// convert font3 from pcx to png
#define GLYPH6
#ifdef GLYPH6
void showglyph6()
{
	C8* srcfont = "C:/rickerspc/dosprog/dpaint2/font3.pcx";
	C8* dstfont = "C:/nodejs/engw/common/sptpics/font3_new.png";
	C32 dacs[256];
	bitmap8* a8 = gfxread8(srcfont,dacs);
	bitmap32* a32 = bitmap8to32(a8,dacs);
	gfxwrite32(dstfont,a32);
	bitmap8free(a8);
	bitmap32free(a32);
}
#endif