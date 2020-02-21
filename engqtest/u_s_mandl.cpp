#include <m_eng.h>
#include "u_states.h"
#include "u_mandl.h"

//#define FOUR

namespace mandlb
{
	const S32 WIX = 1024;
	const S32 WIY = 768;

	const S32 SIZ = 768;
	S32 gdepth = 4096;
	S32 palette = 5;
//	S32 paletteoffset = 0;

#ifdef FOUR
	bitmap32* pic1;
	bitmap32* pic2;
	bitmap32* pic3;
	bitmap32* pic4;
#else
	bitmap32* pic;
#endif
	S32 xloi,yloi,xhii,yhii,zm;
	mandl* md;
}

using namespace mandlb;


void mandlinit()
{
	md = new mandl();
	pushandsetdir("mandl");
	xloi = yloi = 0;
	xhii = yhii = SIZ - 1;
	zm = 0;
/*	S32 siz = 768;
	mandl_task(siz,"_mandl_0_0_0.png");

	mandl_task(siz,"_mandl_1_0_0.png");
	mandl_task(siz,"_mandl_1_1_0.png");
	mandl_task(siz,"_mandl_1_0_1.png");
	mandl_task(siz,"_mandl_1_1_1.png");

	mandl_task(siz,"_mandl_2_0_0.png");
	mandl_task(siz,"_mandl_2_1_0.png");
	mandl_task(siz,"_mandl_2_2_0.png");
	mandl_task(siz,"_mandl_2_3_0.png");

	mandl_task(siz,"_mandl_2_0_1.png");
	mandl_task(siz,"_mandl_2_1_1.png");
	mandl_task(siz,"_mandl_2_2_1.png");
	mandl_task(siz,"_mandl_2_3_1.png");

	mandl_task(siz,"_mandl_2_0_2.png");
	mandl_task(siz,"_mandl_2_1_2.png");
	mandl_task(siz,"_mandl_2_2_2.png");
	mandl_task(siz,"_mandl_2_3_2.png");

	mandl_task(siz,"_mandl_2_0_3.png");
	mandl_task(siz,"_mandl_2_1_3.png");
	mandl_task(siz,"_mandl_2_2_3.png");
	mandl_task(siz,"_mandl_2_3_3.png");
*/
	video_setupwindow(WIX,WIY);
	md->setdepth(gdepth);
	md->setpalette(palette);
#ifdef FOUR
	md.setpicisize(SIZ);
//	md.setzoom(1);
//	pic1 = md.getpic( 0, 0,  PX-1,  PY-1);
//	pic2 = md.getpic(PX, 0,2*PX-1,  PY-1);
//	pic3 = md.getpic( 0,PY,  PX-1,2*PY-1);
//	pic4 = md.getpic(PX,PY,2*PX-1,2*PY-1);
	pic1 = md.getpici(1,0,0);
	pic2 = md.getpici(1,1,0);
	pic3 = md.getpici(1,0,1);
	pic4 = md.getpici(1,1,1);
#else
	md->setpicisize(SIZ);
//	md.setzoom(0);
//	pic = md.getpic(0,0,PX-1,PY-1);
	md->setzoom(zm);
	pic = md->getpic(xloi,yloi,xhii,yhii);
#endif
}

void mandlproc()
{
	bool dopic = false;
	bool dopicfromwork = false;
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case '0':
		xloi = yloi = 0;
		xhii = yhii = SIZ - 1;
		zm = 0;
		dopic = true;
		break;
	case '1':
		if (zm<21) {
			xloi <<= 1;
			xhii = xloi + SIZ;
			yloi <<= 1;
			yhii = yloi + SIZ;
			++zm;
			dopic = true;
		}
		break;
	case '2':
		if (zm<21) {
			xloi <<= 1;
			xloi += SIZ;
			xhii = xloi + SIZ;
			yloi <<= 1;
			yhii = yloi + SIZ;
			++zm;
			dopic = true;
		}
		break;
	case '3':
		if (zm<21) {
			xloi <<= 1;
			xhii = xloi + SIZ;
			yloi <<= 1;
			yloi += SIZ;
			yhii = yloi + SIZ;
			++zm;
			dopic = true;
		}
		break;
	case '4':
		if (zm<21) {
			xloi <<= 1;
			xloi += SIZ;
			xhii = xloi + SIZ;
			yloi <<= 1;
			yloi += SIZ;
			yhii = yloi + SIZ;
			++zm;
			dopic = true;
		}
		break;
	case '5':
		if (zm<21) {
			xloi <<= 1;
			xloi += SIZ/2;
			xhii = xloi + SIZ;
			yloi <<= 1;
			yloi += SIZ/2;
			yhii = yloi + SIZ;
			++zm;
			dopic = true;
		}
		break;
	case '6':
		if (zm>0) {
			xloi >>= 1;
			xloi -= SIZ/4;
			xhii = xloi + SIZ;
			yloi >>= 1;
			yloi -= SIZ/4;
			yhii = yloi + SIZ;
			--zm;
			dopic = true;
		}
		break;
	case 'h':
		gdepth = md->setdepth(gdepth+16);
		dopic = true;
		break;
	case 'l':
		gdepth = md->setdepth(gdepth-16);
		dopic = true;
		break;
	case 'y':
		gdepth = md->setdepth(gdepth+256);
		dopic = true;
		break;
	case 'o':
		gdepth = md->setdepth(gdepth-256);
		dopic = true;
		break;
	case 'i':
		gdepth = md->setdepth(gdepth+128);
		dopic = true;
		break;
	case 'm':
		gdepth = md->setdepth(gdepth-128);
		dopic = true;
		break;
	case 'p':
		++palette;
		if (palette >= MAXPAL)
			palette = 0;
		md->setpalette(palette);
		dopicfromwork = true;
		break;
	case ',':
		md->rotatedac(-1);
		dopicfromwork = true;
		break;
	case '.':
		md->rotatedac(1);
		dopicfromwork = true;
		break;
	case '-':
		md->rotatedac(-16);
		dopicfromwork = true;
		break;
	case '=':
		md->rotatedac(16);
		dopicfromwork = true;
		break;
	}
	if (wininfo.mleftclicks) {
		if (zm<21) {
			xloi += MX - WX/2;
			yloi += MY - WY/2;
			xhii += MX - WX/2;
			yhii += MY - WY/2;
			xloi <<= 1;
			xloi += SIZ/2;
			xhii = xloi + SIZ;
			yloi <<= 1;
			yloi += SIZ/2;
			yhii = yloi + SIZ;
			++zm;
			dopic = true;
		}
	}
	if (wininfo.mrightclicks) {
		if (zm>0) {
			xloi >>= 1;
			xloi -= SIZ/4;
			xhii = xloi + SIZ;
			yloi >>= 1;
			yloi -= SIZ/4;
			yhii = yloi + SIZ;
			--zm;
			dopic = true;
		}
	}
	if (wininfo.mmiddleclicks) {
		xloi = yloi = 0;
		xhii = yhii = SIZ - 1;
		zm = 0;
		dopic = true;
	}
	if (dopic) {
		bitmap32free(pic);
//		pic = md->getpici(zm,xi,yi);
		md->setzoom(zm);
		pic = md->getpic(xloi,yloi,xhii,yhii);
	}
	if (dopicfromwork) {
		bitmap32free(pic);
		pic = md->getpicfromworkpic();
	}
}

void mandldraw2d()
{
	clipclear32(B32,C32GREEN);
#ifdef FOUR
	clipblit32(pic1,B32,0,0,WIX/2-SIZ,WIY/2-SIZ,SIZ,SIZ);
	clipblit32(pic2,B32,0,0, 10+WIX/2,WIY/2-SIZ,SIZ,SIZ);
	clipblit32(pic3,B32,0,0,WIX/2-SIZ, 10+WIY/2,SIZ,SIZ);
	clipblit32(pic4,B32,0,0, 10+WIX/2, 10+WIY/2,SIZ,SIZ);
#else
	clipblit32(pic,B32,0,0,(WIX-SIZ)/2,(WIY-SIZ)/2,SIZ,SIZ);
	outtextxybf32(B32,0,0,C32WHITE,C32BLACK,"xloi %d, yloi %d, xhii %d, yhii %d, zm %d, gdepth %d, pal %d",
		xloi,yloi,xhii,yhii,zm,gdepth,palette);
#endif
}

void mandlexit()
{
#ifdef FOUR
	bitmap32free(pic1);
	bitmap32free(pic2);
	bitmap32free(pic3);
	bitmap32free(pic4);
#else
	bitmap32free(pic);
#endif
	popdir();
	delete md;
}
