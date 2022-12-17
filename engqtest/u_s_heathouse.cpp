// heat in a house
#include <m_eng.h>

#include "u_states.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace u_s_heat_house {
#endif

#define RX 128
#define RY 96

#define XSIZE 1024
#define YSIZE 768

bitmap8 *room,*bigroom; //,*heat,*oldheat;
U32 *heat,*oldheat;
C32 dacs[256];//,dacs2[256];
U8 andor[512];

S32 nthermostats;
S32 nheaters;
S32 nwindows;
//S32 nwalls;
pointi2 *thermostats;
pointi2 *heaters;
pointi2 *windows;
//pointi2 *walls;

S32 heateron;

bitmap8* B8;

void scanroom()
{
	S32 i,j,c;
	nthermostats=nheaters=nwindows=/*nwalls=*/0;
	for (j=0;j<RY;j++)
		for (i=0;i<RX;i++) {
			c=fastgetpixel8(room,i,j);
			if (c) {
				switch (c) {
				//case red:
				case lightred:
				case lightmagenta:
					nheaters++;
					break;
				case lightgreen:
					nthermostats++;
					break;
				case lightblue:
					nwindows++;
					break;
//				case blue:
//				case white:
//					nwalls++;
//					break;
				}
			}
		}
	if (thermostats)
		delete[](thermostats);
	thermostats=new pointi2[nthermostats+1];
	if (heaters)
		delete[](heaters);
	heaters=new pointi2[nheaters+1];
	if (windows)
		delete[](windows);
	windows=new pointi2[nwindows+1];
//	if (walls)
//		memfree(walls);
//	walls=memalloc((nwalls+1)*sizeof(struct pointi2));
	nthermostats=nheaters=nwindows=/*nwalls=*/0;
	for (j=0;j<RY;j++)
		for (i=0;i<RX;i++) {
			c=fastgetpixel8(room,i,j);
			if (c) {
				switch (c) {
				//case red:
				case lightred:
				case lightmagenta:
					heaters[nheaters].x=i;
					heaters[nheaters++].y=j;
					break;
				case lightgreen:
					thermostats[nthermostats].x=i;
					thermostats[nthermostats++].y=j;
					break;
				case lightblue:
					windows[nwindows].x=i;
					windows[nwindows++].y=j;
					break;
//				case white:
//				case blue:
//					walls[nwalls].x=i;
//					walls[nwalls++].y=j;
//					break;
				}
			}
		}
}

void setdacrange(S32 s,S32 e,C32 ds,C32 de)
{
	S32 i;
	for (i=s;i<=e;i++) {
		dacs[i].r=(de.r*(i-s)+ds.r*(e-i))/(e-s);
		dacs[i].g=(de.g*(i-s)+ds.g*(e-i))/(e-s);
		dacs[i].b=(de.b*(i-s)+ds.b*(e-i))/(e-s);
	}
}

void doheat()
{
	U32 *t;
	U32 *p,*p2;
	U8 *w;
	S32 i,j,c,x,y;
	S32 lastheateron=heateron;
	heateron = 0; // should be 0, test with 1
	for (i=0;i<nthermostats;i++) {
		x=thermostats[i].x;
		y=thermostats[i].y;
		c=heat[x+y*RX];
		c>>=18;
		//if (true) {
		if (c<=135) {
			heateron=1;
			break;
		}
	}
	if (heateron)
		for (i=0;i<nheaters;i++)
			fastputpixel8(room,heaters[i].x,heaters[i].y,lightmagenta);
	else
		for (i=0;i<nheaters;i++)
			fastputpixel8(room,heaters[i].x,heaters[i].y,lightred);
	if (heateron)
		for (i=0;i<nheaters;i++)
			heat[heaters[i].x+heaters[i].y*RX]=1023*65536;
	for (i=0;i<nwindows;i++)
		heat[windows[i].x+windows[i].y*RX]=64*65536;
	t=oldheat;
	oldheat=heat;
	heat=t;
	for (j=1;j<RY-1;j++) {
		p=oldheat+RX*j+1;
		p2=heat+RX*j+1;
		w=room->data+RX*j+1;
		for (i=1;i<RX-1;i++,p++,p2++,w++) {
			if (w[1]==white || w[1]==lightblue)
				c=p[0];
			else
				c=p[1];
			if (w[-1]==white || w[-1]==lightblue)
				c+=p[0];
			else
				c+=p[-1];
			if (w[-RX]==white || w[-RX]==lightblue)
				c+=p[0];
			else
				c+=p[-RX];
			if (w[RX]==white || w[RX]==lightblue)
				c+=p[0];
			else
				c+=p[RX];
			c+=2;
			c>>=2;
			p2[0] = c; // 64 << 18;// c;
		}
	}
}

void video_setpalette(const C32* dacs)
{
}

#ifdef USENAMESPACE
} /// end namespace u_s_heat_house
using namespace u_s_heat_house;
#endif


void heathouseinit()
{
	S32 i;
	S32 p=RX*RY;
	video_setupwindow(XSIZE,YSIZE);
	pushandsetdir("heathouse");
	room = gfxread8("rooms.pcx",dacs);
	logger("rooms size = %d, %d\n",room->size.x,room->size.y);
	popdir();
////////////////////////// main
	bigroom=bitmap8alloc(XSIZE,YSIZE,-1);
	B8=bitmap8alloc(XSIZE,YSIZE,blue);
	heat=new U32[RX*RY];
	oldheat=new U32[RX*RY];
	for (i=0;i<p;i++) {
		heat[i]=64*65536;
		oldheat[i]=64*65536;
	}
	//memcpy(dacs,wininfo.stdpalette,sizeof(struct rgb24)*16);
	copy(stdpalette,stdpalette+16,dacs);
	setdacrange(16,80,C32BLUE,C32LIGHTGREEN);
	setdacrange(80,160,C32LIGHTGREEN,C32YELLOW);
	setdacrange(160,255,C32YELLOW,C32RED);
	dacs[135]=C32MAGENTA;
	for (i=16;i<=255;i++) {
		if (i&1) {
			dacs[i].r^=8;
			dacs[i].g^=8;
			dacs[i].b^=8;
		}
	}
//	setdacrange(16,40,rgbblue,rgblightgreen);
//	setdacrange(40,80,rgblightgreen,rgbyellow);
//	setdacrange(80,135,rgbyellow,rgbred);
//	setdacrange(136,255,rgbdarkgray,rgbwhite);
//	for (i=16;i<=135;i++)
//		dacs2[2*i-16]=dacs[i];
//	for (i=136;i<=255;i++)
//		dacs2[2*i-255]=dacs[i];
	video_setpalette(dacs);
	scanroom();
#if 1
	for (i = 0; i < 256; i++) {
		andor[i] = 0x00;
		andor[i + 256] = i;
	}
#if 1
	andor[blue] = 0xff;
	andor[blue + 256] = 0x00;
	andor[black] = 0xff;
	andor[black + 256] = 0x00;
#endif
#else
	for (i = 0; i < 256; i++) {
		andor[i] = 0;
		andor[i + 256] = i;
	}
	andor[0] = 0xff;
#endif
}

void heathouseproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);

#if 1
// draw background
	S32 p;
	static S32 oldbut;
	S32 mx,my;
	S32 col,newcol;
	//video_lock();
	if (MBUT&!oldbut) {
		mx=MX/8;
		my=MY/8;
		col=clipgetpixel8(room,mx,my);
		if (col!=black && col!=white) {
			switch(col) {
			case red:	
				newcol=lightred;
				break;
			case lightred:
			case lightmagenta:
				newcol=red;
				break;
			case green:	
				newcol=lightgreen;
				break;
			case lightgreen:
				newcol=green;
				break;
			case blue:	
				newcol=lightblue;
				break;
			case lightblue:
				newcol=blue;
				break;
			}
			clipfloodfill8(room,mx,my,newcol);
			scanroom();
		}
	}
	for (p=0;p<250;p++)
		doheat();
	oldbut=MBUT;
#endif
}

void heathousedraw2d()
{
	S32 i,j,wv;
	U32 *ip;
	U8 v;
	//clipclear32(B32,C32(0,0,255));	
	for (j=0;j<RY;j++) {
		ip=heat+RX*j;
		for (i=0;i<RX;i++,ip++) {
			wv=*ip>>18;
//			if (wv>=200)
//				v=200+(wv-200)*55/823;
//			else
//				v=wv;
			v=wv;
			fastrect8(B8,i<<3,j<<3,8,8,v);
		}
	}
#if 1
	clipscaleblit8(room,bigroom);

	clipmask8(bigroom,B8,andor);
	//clipblit8(bigroom, B8, 0, 0, 0, 0, bigroom->size.x, bigroom->size.y);

	for (i = 0; i < 256; i++)
		cliprect8(B8, i * 4, 0, i * 4 + 4, 16, i);
	i=clipgetpixel8(B8,MX,MY);
	clipline8(B8,i*4-1,0,i*4-1,15,black);
	clipline8(B8,i*4-2,0,i*4-2,15,black);
	clipline8(B8,i*4+4,0,i*4+4,15,black);
	clipline8(B8,i*4+5,0,i*4+5,15,black);
#endif
	/*bitmap32* b32 = */
	convert8to32(B8,dacs,B32);
	//bitmap32* b32 = bitmap32alloc(B8->size.x,B8->size.y,C32GREEN);
	//clipblit32(b32,B32,0,0,0,0,b32->size.x,b32->size.y);
	//bitmap32free(b32);
	outtextxyf32(B32,4,WY-8-4,C32WHITE,"Heat House");
	//video_unlock();
}

void heathouseexit()
{
	C32* stddacs = stdpalette;	// get std palette
	video_setpalette(stddacs);	// set it in window
	bitmap8free(room);
	bitmap8free(bigroom);
	delete[] heat;
	delete[] oldheat;
	delete[] thermostats;
	thermostats=0;
	delete[] windows;
	windows = 0;
//	memfree(walls);
//	walls=NULL;
	delete[] heaters;
	heaters = 0;
	bitmap8free(B8);
}

