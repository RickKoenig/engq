#include <m_eng.h>
#include "u_mandl.h"
namespace mandlb {
S32 palidx=0;
//S32 mdepth=0;
double zoom=200.0;
double cmx=0.0;
double cmy=0.0;
S32 x,y;

S32 mousezoom=300;

/*
void rect(S32 x0,S32 y0,S32 x1,S32 y1,S32 col)
{
S32 i,j;
for (i=x0;i<=x1;i++)
	for (j=y0;j<=y1;j++)
		tvga_wtdot(i,j,col);
}
*/

//U8 alldac[256][3];

void tvga_setscreen(S32){}
void tvga_init(){}
void initmouse(){}
void setmousearea(S32 xlo,S32 ylo,S32 xhi,S32 yhi){}
void setmousexym(S32 nmx,S32 nmy){}
void setalldac(U8* alldac){}
void tvga_wtdot(S32 x,S32 y,U32 v){}
U32 tvga_rddot(S32 x,S32 y){return 0;}
void closegraph(){}
S32 getmousexy(S32* gmx,S32* gmy){return 0;}
C8 getkey(){return 0;}
void waitvb(){}

void initdata()
{
	tvga_setscreen(255);
}

void initdac0(C32* p,S32 d)
{
	S32 i;
	for (i=0;i<d;i++) {
		p[i].r=0xff*(i+1)/d;
		p[i].g=0xff*(i+1)/d;
		p[i].b=0xff*(i+1)/d;
		switch(i&3) {
		case 0:
			p[i].r/=2;
	 		break;
		case 1:
			p[i].g/=2;
	 		break;
		case 2:
			p[i].b/=2;
	 		break;
		}
	}
	p[i].r=0;
	p[i].g=0;
	p[i].b=0;
}

S32 getcol(S32 val)
{
	val&=0x3ff;
	switch(val>>8) {
	case 0:
		return(val&0xff);
	case 1:
		return(0xff);
	case 2:
		val = 0xff - val;
		return(val&0xff);
	case 3:
		return(0);
	}
	return 0;
}

void initdac1(C32* p,S32 d)
{
	S32 i;
	for (i=0;i<d;i++) {
		p[i].r=0xff*i/d;
		p[i].g=0xff*i/d;
		p[i].b=0xff*i/d;
	}
	p[i].r=0;
	p[i].g=0;
	p[i].b=0;
}

void initdac2(C32* p,S32 d)
{
	S32 i;
	for (i=0;i<d;i++) {
		p[i].r=getcol(0x400*i/d+0x100);
		p[i].g=getcol(0x400*i/d-0x100);
		p[i].b=getcol(0x400*i/d);
	}
	p[i].r=0;
	p[i].g=0;
	p[i].b=0;
}

void initdac3(C32* p,S32 d)
{
	S32 i;
	for (i=0;i<d;i++) {
		p[i].r=getcol(0x400*i/d);
		p[i].g=getcol(0x400*i/d+0x100);
		p[i].b=getcol(0x400*i/d-0x100);
	}
	p[i].r=0;
	p[i].g=0;
	p[i].b=0;
}

void initdac4(C32*p,S32 d) {
	S32 i;
	for (i=0;i<d;i++) {
		p[i].r=getcol(0x400*i/d);
		p[i].g=getcol(0x400*i/d-0x100);
		p[i].b=getcol(0x400*i/d+0x100);
	}
	p[i].r=0;
	p[i].g=0;
	p[i].b=0;
}

void initdac5(C32*p,S32 d) {
	S32 i;
	U32 r,g,b;
	for (i=0;i<=d;i++) {
		if (i == d) {
			r = 0;
			g = 0;
			b = 0;
		} else if (i < 64) {
			r = 2*i;
			g = 64;
			b = 128;
		} else if (i < 128) {
			r = (((i - 64) * 128) / 126) + 128;
			g = 32;
			b = 128;
		} else if (i < 256) {
			r = (((i - 128) * 62) / 127) + 193;
			g = 0;
			b = 128;
		} else if (i < 512) {
			r = 255;
			g = (((i - 256) * 62) / 255) + 1;
			b = 128;
		} else if (i < 1024) {
			r = 255;
			g = (((i - 512) * 63) / 511) + 64;
			b = 128;
		} else if (i < 2048) {
			r = 255;
			g = (((i - 1024) * 63) / 1023) + 128;
			b = 128;
		} else if (i < 4096) {
			r = 255;
			g = (((i - 2048) * 63) / 2047) + 192;
			b = 128;
		} else {
			r = 255;
			g = 255;
			b = 255;
		}
		p[i].r = r;
		p[i].g = g;
		p[i].b = b;

	}
}

void (*dacfuncts[])(C32* p,S32 d)={initdac0,initdac1,initdac2,initdac3,initdac4,initdac5};

const S32 MAXPAL=sizeof(dacfuncts)/(sizeof ( void (*)()) );

C32* initdac(S32 pal,S32 d)
{
	C32* ret = new C32[d+1];
	fill(ret,ret+d+1,C32WHITE);
	dacfuncts[pal](ret,d);
	return ret;
}

void irotatedac(C32* p,S32 mdepth)
{
	S32 r,g,b;
	S32 i;
	r=p[0].r;
	g=p[0].g;
	b=p[0].b;
	for (i=0;i<mdepth-1;i++) {
		p[i].r=p[i+1].r;
		p[i].g=p[i+1].g;
		p[i].b=p[i+1].b;
	}
	p[mdepth-1].r=r;
	p[mdepth-1].g=g;
	p[mdepth-1].b=b;
}

void irotatedac2(C32* p,S32 mdepth)
{
	S32 r,g,b;
	S32 i;
	r=p[mdepth-1].r;
	g=p[mdepth-1].g;
	b=p[mdepth-1].b;
	for (i=mdepth-2;i>=0;i--) {
		p[i+1].r=p[i].r;
		p[i+1].g=p[i].g;
		p[i+1].b=p[i].b;
	}
	p[0].r=r;
	p[0].g=g;
	p[0].b=b;
}

bool divide(S32 xlo,S32 xhi,S32 ylo,S32 yhi,S32 d);
void maini()
{
	tvga_init();
	initmouse();
	setmousearea(0,0,1023,767);
	setmousexym(512,384);
	initdata();
//	initdac(depth);
//	setalldac(alldac[0]);
	while(1) {
//		if (!odivide(-512,511,-384,383)) {
		if (!divide(-512,511,-384,383,16)) {
			// waitkey();
			;
		}
	}
}

// returns 0 to mdepth inclusive
S32 fastmand(S32 mdepth)
{
	S32 count;
	double ru,iu,rz,iz,rs,is;
	ru=x/zoom-cmx;
	iu=y/zoom-cmy;
	rz=iz=rs=is=0;
	count=-1;
	while((count!=mdepth) && (4.0>(is+rs))) {
		iz=2*rz*iz-iu;
		rz=rs-is-ru;
		rs=rz*rz;
		is=iz*iz;
		count++;
	}
	return count;
}

void tvga_xordot(S32 x,S32 y,S32 sh)
{
	if ((x<0)||(y<0)||(x>=1024)||(y>=768))
		return;
	tvga_wtdot(x,y,tvga_rddot(x,y)+sh);
}

void dovline(S32 x0,S32 y0,S32 y1,S32 sh)
{
	while(y0<=y1) {
		tvga_xordot(x0,y0,sh);
		y0++;
	}
}

void dohline(S32 x0,S32 y0,S32 x1,S32 sh)
{
	while(x0<=x1) {
		tvga_xordot(x0,y0,sh);
		++x0;
	}
}

void dobox(S32 x0,S32 y0,S32 x1,S32 y1,S32 sh)
{
	dovline(x0,y0,y1,sh);
	dovline(x1,y0,y1,sh);
	dohline(x0+1,y0,x1-1,sh);
	dohline(x0+1,y1,x1-1,sh);
}

void cursor(S32 mx,S32 my,S32 sh)
{
	S32 mzy=768*mousezoom/1024;
	dobox(mx-mousezoom,my-mzy,mx+mousezoom,my+mzy,sh);
}
#if 0
S32 checkkey()
{
	S32 but=0,mx=0,my=0;
	S32 sh=0;
	C8 c=getkey();
	if (c!=0) {
		if (c==K_ESCAPE) {
			closegraph();
			exit(0);
		}
		if (c=='r') {
			cmx=0.0;
			cmy=0.0;
			zoom=200;
			initdata();
			return 1;
		}
		if (c=='m') {
			setmousexym(512,384);
			cursor(mx,my,sh);
			while(1) {
				c=getkey();
				waitvb();
				waitvb();
				cursor(mx,my,-sh);
				if (mousezoom<50) {
					if (c=='=')
						mousezoom+=4;
					else if (c=='-')
						mousezoom-=4;
					if (mousezoom<10)
						mousezoom=10;
				} else {
					if (c=='=')
						mousezoom+=20;
					else if (c=='-')
						mousezoom-=20;
					if (mousezoom>400)
						mousezoom=400;
				}
				sh+=85;
				but=getmousexy(&mx,&my);
				cursor(mx,my,sh);
				if (c=='m')
					break;
				if (but) {
					cmx+=(512-mx)/zoom;
					cmy+=(384-my)/zoom;
					zoom*=512.0f/mousezoom;
					initdata();
					return 1;
				}
			}
			cursor(mx,my,-sh);
		}
		if (((c>='1')&&(c<='5'))||(c=='h')||(c=='l')||(c=='p')||(c=='.')||(c==',')) {
			if ((c!='p')&&(c!='.')&&(c!=','))
				initdata();
			switch (c) {
			case '5':
				zoom/=1.8f;
				break;
			case '2':
				cmx=cmx+200.0f/zoom;
				cmy=cmy+200.0f/zoom;
				zoom*=1.8f;
				break;
			case '1':
				cmx=cmx-200.0f/zoom;
				cmy=cmy+200.0f/zoom;
				zoom*=1.8f;
				break;
			case '4':
				cmx=cmx-200.0f/zoom;
				cmy=cmy-200.0f/zoom;
				zoom*=1.8f;
				break;
			case '3':
				cmx=cmx+200.0f/zoom;
				cmy=cmy-200.0f/zoom;
				zoom*=1.8f;
				break;
			case 'h':
				if (depth<240)
					depth+=16;
				initdac(depth);
				setalldac(alldac[0]);
				break;
			case 'l':
				if (depth>16)
					depth-=16;
				initdac(depth);
				setalldac(alldac[0]);
				break;
			case 'p':
				palidx++;
				if (palidx==MAXPAL)
					palidx=0;
				initdac(depth);
				setalldac(alldac[0]);
				return false;
			case '.':
				rotatedac();
				setalldac(alldac[0]);
				return false;
			case ',':
				rotatedac2();
				setalldac(alldac[0]);
				return false;
		  	}
			return true;
		}
	}
	return false;
}
#endif
// return quit
S32 getmand(S32 d);
bool divide(S32 xlo,S32 xhi,S32 ylo,S32 yhi,S32 d)
{
	S32 meanx,meany,val,no;
#if 0
	if (checkkey())
		return true;
#endif
	y=ylo;
	x=xlo;
	no=0;
	val=getmand(d);
	for (x=xlo+1;x<xhi;x++) {
		S32 gm = getmand(d);
		if (gm!=val || gm==0)
			no=1;
	}
	y=yhi;
	for (x=xlo+1;x<xhi;x++) {
		S32 gm = getmand(d);
		if (gm!=val || gm==0)
			no=1;
	}
	x=xlo;
	for (y=ylo;y<=yhi;y++) {
		S32 gm = getmand(d);
		if (gm!=val || gm==0)
			no=1;
	}
	x=xhi;
	for (y=ylo;y<=yhi;y++) {
		S32 gm = getmand(d);
		if (gm!=val || gm==0)
			no=1;
	}
	if (no==0) {
	//	tvga_rect(xlo+512,ylo+384,xhi+512,yhi+384,val);
		for (x=xlo;x<=xhi;x++)
			for (y=ylo;y<=yhi;y++)
				tvga_wtdot(x+512,y+384,val);
		return false;
	}

	if ((xhi-xlo>=2)&&(yhi-ylo>=2)) {
		meanx=(xlo+xhi)>>1;
		meany=(ylo+yhi)>>1;
		if (divide(meanx,xhi,ylo,meany,d))
			return true;
		if (divide(xlo,meanx,ylo,meany,d))
			return true;
		if (divide(xlo,meanx,meany,yhi,d))
			return true;
		if (divide(meanx,xhi,meany,yhi,d))
			return true;
	} else {
		for (x=xlo;x<=xhi;x++)
			for (y=ylo;y<=yhi;y++)
				getmand(d);
	}
	return false;
}

S32 getmand(S32 mdepth)
{
	S32 retval;
	if ((retval=tvga_rddot(x+512,y+384))==255)
		tvga_wtdot(x+512,y+384,retval=fastmand(mdepth));
	return retval;
}

bool odivide(S32 d)
{
	for (x=-512;x<512;x++) {
		for (y=-384;y<384;y++) {
#if 0
			if (checkkey())
				return true;
#endif
			getmand(d);
		}
	}
	return false;
}

mandl::mandl() : iter(0),work(0),zf(0),zoom(1),
  pal(0),paloff(0),palette(0),dirty(false),depth(24),picisize(256)
{
	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_80);
	palette = initdac(pal,depth);
	setzoom(zoom);
}

void mandl::setpalette(S32 p)
{
	pal = p;
	delete[] palette;
	palette = initdac(pal,depth);
}

void mandl::rotatedac(S32 r)
{
	if (r>0) {
		while(r>0) {
			irotatedac(palette,depth);
			--r;
		}
	} else if (r<0) {
		while(r<0) {
			irotatedac2(palette,depth);
			++r;
		}
	}
}

mandl::~mandl()
{
	delete[] palette;
	if (work)
		bitmap32free(work);
	defaultfpucontrol();
}

void mandl::setzoom(S32 z)
{
	zoom = 1<<z;
	izoom = 4.0 / (zoom * picisize);
}

void mandl::setpicisize(S32 pcs)
{
	picisize = pcs;
	setzoom(zoom);
}

S32 mandl::ffastmand(S32 ix,S32 iy)
{
	S32 count;
	double ru,iu,rz,iz,rs,is;
	ru=ix*izoom-2.0;
	iu=iy*izoom-2.0;
	rz=iz=rs=is=0;
	count=-1;
	while((count!=depth) && (4.0>(is+rs))) {
		iz=2*rz*iz+iu;
		rz=rs-is+ru;
		rs=rz*rz;
		is=iz*iz;
		count++;
	}
	return count;
}

S32 mandl::getmand(S32 x0,S32 y0,S32 ix,S32 iy)
{
	U32 val = fastgetpixel32(work,ix-x0,iy-y0).c32;
	if (val != ~0)
		return val;
	val = ffastmand(ix,iy);
	fastputpixel32(work,ix-x0,iy-y0,val);
	return val;
}

bitmap32* mandl::getpic(S32 xlo,S32 ylo,S32 xhi,S32 yhi)
{
	getworkpic2(xlo,ylo,xhi,yhi);
	bitmap32* ret=getpicfromworkpic();
	return ret;
}

void mandl::getworkpic(S32 xlo,S32 ylo,S32 xhi,S32 yhi)
{
	S32 w = xhi - xlo + 1;
	S32 h = yhi - ylo + 1;
//	S32 prod = w*h;
	if (work)
		bitmap32free(work);
	work = bitmap32alloc(w,h,C32(0));
	S32 x,y;
	C32* datai = work->data;
	for (y=ylo;y<=yhi;y++) {
		for (x=xlo;x<=xhi;x++) {
			*datai++ = ffastmand(x,y);
		}
	}
}

void mandl::getworkpic2(S32 xlo,S32 ylo, S32 xhi,S32 yhi)
{
	S32 w = xhi - xlo + 1;
	S32 h = yhi - ylo + 1;
	if (work)
		bitmap32free(work);
	work = bitmap32alloc(w,h,C32(~0));
	getworkpicr(xlo,ylo,xlo,ylo,xhi,yhi);
}

void mandl::getworkpicr(S32 x0,S32 y0,S32 xlo,S32 ylo, S32 xhi,S32 yhi)
{
	S32 w = xhi - xlo + 1;
	S32 h = yhi - ylo + 1;
	S32 x,y;
	if (w<=2 || h<=2) {
		for (y=ylo;y<=yhi;y++) {
			for (x=xlo;x<=xhi;x++) {
				getmand(x0,y0,x,y);
			}
		}
		return;
	}
	bool recr = false;
	U32 val = getmand(x0,y0,xlo,ylo);
	if (val==0)
		recr = true;
	else {
		for (x=xlo+1;x<=xhi;++x) {
			if (val != getmand(x0,y0,x,ylo)) {
				recr = true;
				break;
			}
		}
		if (!recr) {
			for (x=xlo+1;x<=xhi;++x) {
				if (val != getmand(x0,y0,x,yhi)) {
					recr = true;
					break;
				}
			}
		}
		if (!recr) {
			for (y=ylo+1;y<=yhi;++y) {
				if (val != getmand(x0,y0,xlo,y)) {
					recr = true;
					break;
				}
			}
		}
		if (!recr) {
			for (y=ylo+1;y<yhi;++y) {
				if (val != getmand(x0,y0,xhi,y)) {
					recr = true;
					break;
				}
			}
		}
	}
	if (recr) {
		getworkpicr(x0,y0,xlo,ylo,(xlo+xhi)>>1,(ylo+yhi)>>1);
		getworkpicr(x0,y0,(xlo+xhi)>>1,ylo,xhi,(ylo+yhi)>>1);
		getworkpicr(x0,y0,xlo,(ylo+yhi)>>1,(xlo+xhi)>>1,yhi);
		getworkpicr(x0,y0,(xlo+xhi)>>1,(ylo+yhi)>>1,xhi,yhi);
	} else {
		for (y=ylo;y<=yhi;y++) {
			for (x=xlo;x<=xhi;x++) {
				fastputpixel32(work,x-x0,y-y0,val);
			}
		}
	}
/*	if (work)
		bitmap32free(work);
	work = bitmap32alloc(w,h,C32(0));
	S32 x,y;
	C32* datai = work->data;
	for (y=ylo;y<=yhi;y++) {
		for (x=xlo;x<=xhi;x++) {
			*datai++ = ffastmand(x,y);
		}
	} */
}

bitmap32* mandl::getpicfromworkpic()
{
	C32* datai = work->data;
	S32 i;
	bitmap32* ret = bitmap32alloc(work->size.x,work->size.y,C32BLACK);
	S32 prod = work->size.x*work->size.y;
	C32* datao = ret->data;
	for (i=0;i<prod;++i) {
		datao[i] = palette[datai[i].c32];
	}
//	bitmap32free(work);
//	clipcircle32(ret,ret->size.x/2,ret->size.y/2,5,C32MAGENTA);
//	cliprecto32(ret,0,0,ret->size.x,ret->size.y,C32RED);
	return ret;
}

bitmap32* mandl::getpici(S32 zm,S32 ix,S32 iy)
{
	setzoom(zm);
	S32 xlo,ylo,xhi,yhi;
	xlo = ix*picisize;
	ylo = iy*picisize;
	xhi = xlo + picisize - 1;
	yhi = ylo + picisize - 1;
	bitmap32* ret = getpic(xlo,ylo,xhi,yhi);
//	outtextxyfc32(ret,ret->size.x/2,ret->size.y/2,C32WHITE,"%d,%d,%d",zm,ix,iy);
	return ret;
}

S32 mandl::setdepth(S32 d)
{
	depth = range(16,d,(S32)MAXDEPTH2);
	delete[] palette;
	palette = initdac(pal,depth);
	return depth;
}
//const S32 mand::MAXDEPTH2;

} // end namespace mandl
using namespace mandlb;

void mandl_task(S32 picsiz,const C8* fname)
{
// replace with string class
	C8* r=new C8[strlen(fname)+1];
	C8* rc = r;
	strcpy(r,fname);
	strtok(rc,"_");
	rc = 0;
	C8* zs = strtok(rc,"_");
	C8* xis = strtok(rc,"_");
	C8* yis = strtok(rc,".");
//	logger("parse picsize %d: '%s' is zoom='%s', ix='%s', iy='%s'\n",picsiz,fname,zs,xis,yis);

	mandl md;
	md.setdepth(4096);
	md.setpalette(5);
	md.setpicisize(picsiz);
	bitmap32* pic = md.getpici(atoi(zs),atoi(xis),atoi(yis));

	delete[] r;

	gfxwrite32("temp.png",pic);
	remove(fname);
	rename("temp.png",fname);
	bitmap32free(pic);

}
