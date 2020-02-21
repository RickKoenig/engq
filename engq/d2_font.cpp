#include <m_eng.h>
#include "d2_font.h"

static C8 str[1024]; // not secure, watch out, beware, should use new secure vsprintf !?

// can be made faster
// single chr
//static recti2 fontclip= {
//	{0,0},
//	{SRESX,SRESY},
//};

#define FC_LEFT 0
#define FC_TOP 0
#define FC_SIZEX WX
#define FC_SIZEY WY
#define OFFX 0 // 1
list<fntque> fntlist;
//static bool doclip=true;
/*void drawtextque_setclip(bool v)
{
	doclip=v;
}*/

void drawtextque_char(softfont* font,S32 x,S32 y,const pointf3& col,U32 chr)
{
//	U32 w,h,i=0;
//	U8 *curc;
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
		if (x+font->gx<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
		if (chr<32 || chr>0x7f)
			return; //errorexit("invalid chr %02x",chr);
//	}
/*	chr-=32;
	curc = &fontdata[chr<<6];
	for(h=0;h<8U;h++)
		for(w=0;w<8U;w++)
			if (curc[i++])
				clipputpixel32(b,x+w,y+h,col); */
	fntque fq(font,x,y,col,chr);
	fntlist.push_back(fq);
}

// string
void drawtextque_string(softfont* font,S32 x,S32 y,const pointf3& col,const C8* str)
{
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
		S32 len=strlen(str);
		if (x+font->gx*(len+OFFX)<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
//	}
	fntque fq(font,x,y,col,str);
	fntlist.push_back(fq);
}

// formatted string
void drawtextque_format(softfont* font,S32 x,S32 y,const pointf3& colf,const C8* fmt,...)
{
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
//	}
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	va_end(arglist);
	S32 len=strlen(str);
//	if (doclip) {
		if (x+font->gx*(len+OFFX)<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
//	}
	fntque fq(font,x,y,colf,str);
	fntlist.push_back(fq);
}

// string with back color
void drawtextque_string_foreback(softfont* font,S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* str)
{
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
//	}
//	S32 len;
//	len=strlen(str);
//	cliprect32(b,x,y,(len<<3),8,colb);
//	drawtextque_string(x,y,colf,str);
	S32 len=strlen(str);
//	if (doclip) {
		if (x+font->gx*(len+OFFX)<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
//	}
	fntque fq(font,x,y,colf,colb,str);
	fntlist.push_back(fq);
}
#if 0
// string with back color
void drawtextque_string_foreback_center(S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* str)
{
	if (doclip) {
		if (y+8<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
	}
//	S32 len;
//	len=strlen(str);
//	cliprect32(b,x,y,(len<<3),8,colb);
//	drawtextque_string(x,y,colf,str);
	S32 len=strlen(str);
	if (doclip) {
		if (x+8*(len+OFFX)<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
	}
	fntque fq(x,y,colf,colb,str);
	fntlist.push_back(fq);
}
#endif
// formatted string with back color
void drawtextque_format_foreback(softfont* font,S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* fmt,...)
{
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
//	}
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	va_end(arglist);
//	drawtextque_string_foreback(x,y,colf,colb,str);
	S32 len=strlen(str);
//	if (doclip) {
		if (x+font->gx*(len+OFFX)<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
//	}
	fntque fq(font,x,y,colf,colb,str);
	fntlist.push_back(fq);
}
#if 0
// formatted string with back color and center
void drawtextque_format_foreback_center(S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* fmt,...)
{
	if (doclip) {
		if (y+8<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
	}
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	va_end(arglist);
//	drawtextque_string_foreback(x,y,colf,colb,str);
	S32 len=strlen(str);
	if (doclip) {
		if (x+8*(len+OFFX)<=FC_LEFT)
			return;
		if (x>FC_LEFT+FC_SIZEX)
			return;
	}
	fntque fq(x,y,colf,colb,str);
	fntlist.push_back(fq);
}
#endif
// centered chr
void drawtextque_char_center(softfont* font,S32 x,S32 y,const pointf3& col,U32 chr)
{
	drawtextque_char(font,x-(font->gx>>1),y-(font->gy>>1),col,chr);
}

// centered string
void drawtextque_string_center(softfont* font,S32 x,S32 y,const pointf3& col,const C8* str)
{
	/*
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
//	}
*/
	drawtextque_string(font,x-(strlen(str)*(font->gx>>1)),y-(font->gy>>1),col,str);
}

// centered formatted string
void drawtextque_format_center(softfont* font,S32 x,S32 y,const pointf3& col,const C8* fmt,...)
{
	/*
//	if (doclip) {
		if (y+font->gy<=FC_TOP)
			return;
		if (y>FC_TOP+FC_SIZEY)
			return;
//	}
*/
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	va_end(arglist);
	drawtextque_string(font,x-(strlen(str)*(font->gx>>1)),y-(font->gy>>1),col,str);
}
