// single chr
void outchar32 (    const struct bitmap32* b,S32 x,S32 y,C32 col ,         U32 chr);
// string
void outtextxy32(  const struct bitmap32* b,S32 x,S32 y,C32 col ,         const C8* str);
// formatted string
void outtextxyf32( struct bitmap32* b,S32 x,S32 y,C32 colf,         const C8* fmt,...);
// string with back color
void outtextxyb32( struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* str);
// formatted string with back color
void outtextxybf32(struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* fmt,...);
// centered chr
void outcharc32(    struct bitmap32* b,S32 x,S32 y,C32 col ,         U32 chr);
// centered string
void outtextxyc32(  struct bitmap32* b,S32 x,S32 y,C32 col ,         const C8* str);
// centered formatted string
void outtextxyfc32( struct bitmap32* b,S32 x,S32 y,C32 colf,         const C8* fmt,...);
// centered string with back color
void outtextxybc32( struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* str);
// centered formatted string with back color
void outtextxybfc32(struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* fmt,...);

class softfont {
public:
	bitmap32* font;
	S32 gx,gy; // size of glyph
	S32 cx,cy; // chars in x and y
	string name;
	softfont(C8* filename,S32 gxa,S32 gya,S32 cxa,S32 cya);
	~softfont();
	// single chr
	void outchar32 (    const struct bitmap32* b,S32 x,S32 y,C32 col ,         U32 chr) const;
	// string
	void outtextxy32(   const struct bitmap32* b,S32 x,S32 y,C32 col ,         const C8* str) const;
	// formatted string
	void outtextxyf32(  struct bitmap32* b,S32 x,S32 y,C32 colf,         const C8* fmt,...) const;
	// string with back color
	void outtextxyb32(  struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* str) const;
	// formatted string with back color
	void outtextxybf32( struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* fmt,...) const;
	// centered chr
	void outcharc32(    struct bitmap32* b,S32 x,S32 y,C32 col ,         U32 chr) const;
	// centered string
	void outtextxyc32(  struct bitmap32* b,S32 x,S32 y,C32 col ,         const C8* str) const;
	// centered formatted string
	void outtextxyfc32( struct bitmap32* b,S32 x,S32 y,C32 colf,         const C8* fmt,...) const;
	// centered string with back color
	void outtextxybc32( struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* str) const;
	// centered formatted string with back color
	void outtextxybfc32(struct bitmap32* b,S32 x,S32 y,C32 colf,C32 colb,const C8* fmt,...) const;
};



