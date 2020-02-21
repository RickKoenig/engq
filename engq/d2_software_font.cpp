#define D2_3D
#include <m_eng.h>
#include "d2_font.h"

//#define WRITEFONT
#ifdef WRITEFONT
void writefont()
{
	bitmap32* f32=bitmap32alloc(128,64,0);
	S32 i,j;
	for (j=0;j<8;++j)
		for (i=0;i<16;++i)
			outchar32(f32,i*8,j*8,C32WHITE,i+16*j);
	clipblit32(f32,B32,0,0,0,0,f32->size.x,f32->size.y);
	static bool done;
	if (!done) {
		bitmap8* f8=bitmap8alloc(128,64,-1);
		S32 c=128*64;
		U8* pd=f8->data;
		C32* ps=f32->data;
		for (i=0;i<c;++i)
			pd[i] = ps[i].g >= 0x80  ?  1  :  0;
		C32 dacs[256];
		memset(dacs,0x80,sizeof(dacs));
		dacs[0]=C32BLACK;
		dacs[1]=C32WHITE;
		pushandsetdir("common");
		gfxwrite8("sysfont.pcx",f8,dacs);
		popdir();
		bitmap8free(f8);
		done=true;
	}
	bitmap32free(f32);
}
#endif

void software_drawchars(const fntque& fq)
{
	const C8* p=fq.t.c_str();
	const softfont* sf = fq.font;
	S32 x=fq.x;
	S32 y=fq.y;
	U32 c;
	if (fq.hascolb) {
		C32 cf=pointf3toC32(&fq.colf);
		C32 cb=pointf3toC32(&fq.colb);
		while(c=*p) {
			U32 ch=c%sf->cx;
			U32 cv=c/sf->cx;
			clipforeback32(sf->font,B32,ch*sf->gx,cv*sf->gy,x,y,sf->gx,sf->gy,cf,cb);
			++p;
			x+=sf->gx;
		}
	} else {
		C32 cf=pointf3toC32(&fq.colf);
		while(c=*p) {
			U32 ch=c%sf->cx;
			U32 cv=c/sf->cx;
			clipfore32(sf->font,B32,ch*sf->gx,cv*sf->gy,x,y,sf->gx,sf->gy,cf);
			++p;
			x+=sf->gx;
		}
	}
}

void software_drawtextque_do() // draw que
{
#ifdef WRITEFONT
	writefont();
#endif
	if (!video3dinfo.sysfont)
		return;
//	outtextxybf32(B32,0,0,C32WHITE,C32BLACK,"G");
//	clipblit32(video3dinfo.sysfont,B32,7*8,4*8,0,0,8,8);
	while (fntlist.size()) {
		fntque fq=fntlist.front();
		software_drawchars(fq);
		fntlist.pop_front();
	}
}
