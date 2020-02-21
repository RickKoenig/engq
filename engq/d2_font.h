// draw queued text


#define SRESX 640
#define SRESY 480

struct fntque
{
	S32 x,y;
	string t;
	pointf3 colf,colb;
	bool hascolb;
	softfont* font;
	fntque(softfont* fonta,S32 xa,S32 ya,const pointf3& cola,U32 chra) : x(xa),y(ya),colf(cola),hascolb(false),font(fonta)
	{
		t=(C8)chra;
	}
	fntque(softfont* fonta,S32 xa,S32 ya,const pointf3& cola,const C8* s) : x(xa),y(ya),colf(cola),hascolb(false),font(fonta)
	{
		t=s;
	}
	fntque(softfont* fonta,S32 xa,S32 ya,const pointf3& colfa,const pointf3& colba,const C8* s) : x(xa),y(ya),colf(colfa),colb(colba),hascolb(true),font(fonta)
	{
		t=s;
	}
};

extern list<fntque> fntlist;


// void drawtextque_do(); // draw que, device independent, see d2_software_font.h and d2_dx9_font.h
// tree2* drawtextque_build(); // draw que, device independent, see d2_software_font.h and d2_dx9_font.h

// single chr
void drawtextque_char (				softfont* font,S32 x,S32 y,const pointf3& col ,					U32 chr);
// centered chr
void drawtextque_char_center(		softfont* font,S32 x,S32 y,const pointf3& col ,					U32 chr);

// string
void drawtextque_string(			softfont* font,S32 x,S32 y,const pointf3& col ,					const C8* str);
// centered string
void drawtextque_string_center(		softfont* font,S32 x,S32 y,const pointf3& col ,					const C8* str);
// string with back color
void drawtextque_string_foreback(	softfont* font,S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* str);
// string with back color and centered
//void drawtextque_string_foreback_center(S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* str);

// formatted string
void drawtextque_format(			softfont* font,S32 x,S32 y,const pointf3& colf,					const C8* fmt,...);
// centered formatted string
void drawtextque_format_center(		softfont* font,S32 x,S32 y,const pointf3& col ,					const C8* fmt,...);
// formatted string with back color
void drawtextque_format_foreback(	softfont* font,S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* fmt,...);
// formatted string with back color and centered
//void drawtextque_format_foreback_center(S32 x,S32 y,const pointf3& colf,const pointf3& colb,const C8* fmt,...);

