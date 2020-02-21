#include <m_eng.h>
#include "u_states.h"

static struct bitmap32* pic;
static U32 curpic;
static S32 togvidmode;

static C8* fnames[]={
	"../common/sysfont.pcx",
	"ncrass_head.dds",
	"torso5.dds",
	"torso3.dds",
	"torso1.dds",
	"gamesel.pcx",
	"clinton.pcx",
	"bark.tga",
	"maptest.tga",
	"rengst.jpg",
	"pencil.jpg",
};
#define NFNAMES (sizeof(fnames)/sizeof(fnames[0]))

static void nextpic()
{
	if (pic)
		bitmap32free(pic);
	pic=gfxread32(fnames[curpic]);
	++curpic;
	if (curpic>=NFNAMES)
		curpic=0;
}

void gfxtestinit()
{
//	struct bitmap8* b8;
//	C32 dacs[256];
	video_setupwindow(800,600);
	pushandsetdir("gfxtest");
//	pic=gfxread32("gamesel.pcx");
//	pic=gfxread32("clinton.pcx");
//	pic=gfxread32("barktest.tga");
//	pic=gfxread32("maptest.tga");
//	pic=gfxread32("rengst.jpg");
//	pic=gfxread32("pencil.jpg");
//	gfxwrite32("barktest.tga",pic);
/*
	b8=gfxread8("clinton.pcx",dacs);
	gfxwrite8("cltest.pcx",b8,dacs);
	bitmap8free(b8);
	b8=gfxread8("gamesel.pcx",dacs);
	gfxwrite8("gstest.pcx",b8,dacs);
	bitmap8free(b8);
*/
	nextpic();
}

void gfxtestproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (wininfo.mleftclicks)
		nextpic();
	if (wininfo.mmiddleclicks) {
		togvidmode^=1;
		video_init(togvidmode,0);
		video_setupwindow(800,600);
	}
	if (wininfo.justdropped) {
		if (pic)
			bitmap32free(pic);
		pic=gfxread32(wininfo.args->idx(0).c_str());
	}
	if (KEY=='n')
		nextpic();
}

void gfxtestdraw2d()
{
//	video_lock();
	fastclear32(B32,C32MAGENTA);
	clipblit32(pic,B32,0,0,WX/2-MX,WY/2-MY,pic->size.x,pic->size.y);
	S32 i,j;
	for (j=0;j<16;++j) {
		for (i=0;i<16;++i) {
			C32 c=clipgetpixel32(pic,i+MX-8,j+MY-8);
			cliprect32(B32,8+i*8,8+j*8,7,7,c);
		}
	}
	i=j=8;
	cliprecto32(B32,8+i*8-1,8+j*8-1,9,9,C32WHITE);
	cliprecto32(B32,8+i*8-2,8+j*8-2,11,11,C32BLACK);
	cliprecto32(B32,WX/2-4,WY/2-4,8,8,C32WHITE);
	cliprecto32(B32,WX/2-5,WY/2-5,10,10,C32BLACK);
	outtextxybf32(B32,0,WY-12,C32WHITE,C32BLACK,"%4d %4d",MX,MY);
//	video_unlock();
}

void gfxtestexit()
{
	popdir();
	bitmap32free(pic);
	pic=0;
}
