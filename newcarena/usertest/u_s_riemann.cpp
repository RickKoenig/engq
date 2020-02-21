#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "u_s_riemann.h"

#define X0 185
#define Y0 115
#define X1 660
#define Y1 590
#define XSIZ (X1-X0+1)
#define YSIZ (Y1-Y0+1)

struct cp {
	double r,i;
};

static struct cp z0,zi;
static int iter;
static int xc,xf,yc,yf;

static shape *rl,*focus,*oldfocus;
static hscroll* hslide1,*hslide2;
static vscroll* vslide1,*vslide2,*vslideiter;
static text* textiter,*textzoom,*text1,*text2;
static pbut* pbutzoomin,*pbutzoomout,*pbutquit;

static double zoomf;

static void dozoom(int z)
{
	char str[50];
	if (z==0)
		zoomf=16.0;
	else if (z==1)
		zoomf*=1.41428;
	else if (z==-1)
		zoomf/=1.41428;
	zoomf=range(1.0,zoomf,4096.0);
	sprintf(str,"+/- %f",XSIZ/(zoomf*2.0));
//	setresname(rl,ZOOMTEXT,str);
	textzoom->settname(str);

}

static void cp2scn(struct cp *z,int *x,int *y)
{
	*x=(int)(X0+XSIZ/2+z->r*zoomf);
	*y=(int)(Y0+YSIZ/2-z->i*zoomf);
}

/*double mysin(double a)
{
	int i;
	double d,n;
	double r=0;
	d=1;
	n=a;
	for (i=1;i<30;i+=2) {
		r+=n/d;
		d*=-(i+1)*(i+2);
		n*=a*a;
	}
	return r;
}

double mycos(double a)
{
	int i;
	double d,n;
	double r=1;
	d=-2;
	n=a*a;
	for (i=2;i<30;i+=2) {
		r+=n/d;
		d*=-(i+1)*(i+2);
		n*=a*a;
	}
	return r;
}
*/
void calczi(struct cp *p,int n)
{
	double rp,ang,angs;//,sq;
	ang = angs = -z0.i*log((double)n);
//	rp=sqrt((double)b);
	rp=pow((double)n,-z0.r);
	while(ang<0)
		ang+=TWOPI;
	p->r=rp*cos(ang);
	p->i=rp*sin(ang);
//	logger("n = %3d, mod = %8.4f, arg = %8.4f, a = %8.4f, bi = %8.4f\n",n,rp,angs,p->r,p->i);
//	// now do the reciprocal, now using -z0 for reciprocal
//	sq=p->r*p->r+p->i*p->i;
//	p->r/=sq;
//	p->i/=-sq;
}

static void drawlines()
{
//	int fpucw;
	int i;
	int x0=0,y0=0,x1,y1;
	struct cp zs;
//	fpucw=getfpucontrol();
//	setfpucontrol(fpucw&0xf0ff | (RND_NEAR<<10) | (PREC_80<<8));
	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_32);
//	B16->cliprect.topleft.x=X0;
//	B16->cliprect.topleft.y=Y0;
//	B16->cliprect.size.x=XSIZ;
//	B16->cliprect.size.y=YSIZ;
	cliprect32(B32,X0,Y0,XSIZ,YSIZ,C32WHITE);
	zs.r=zs.i=0;
	cp2scn(&zs,&x0,&y0);
//	clipcircle16(B16,x0,y0,5,higreen);
	clipcircle32(B32,x0,y0,3,C32GREEN);
	for (i=1;i<=iter;i++) {
		calczi(&zi,i);
		zs.r+=zi.r;
		zs.i+=zi.i;
		cp2scn(&zs,&x1,&y1);
//		clipcircle16(B16,x1,y1,5,higreen);
//		clipline16(B16,x0,y0,x1,y1,hiblack);
		clipcircle32(B32,x1,y1,2,C32GREEN);
		clipline32(B32,x0,y0,x1,y1,C32BLACK);
		x0=x1;
		y0=y1;
	}
//	B16->cliprect.topleft.x=0;
//	B16->cliprect.topleft.y=0;
//	B16->cliprect.size.x=WX;
//	B16->cliprect.size.y=WY;
//	setfpucontrol(fpucw);
	defaultfpucontrol();
}

void riemann_init()
{
	video_setupwindow(800,600);
	pushandsetdir("riemann");
//	rl=loadres("reimann.txt");
	script sc;
	rl=res_loadfile("riemann.txt");
	popdir();
	hslide1=rl->find<hscroll>("HSLIDE1");
	hslide2=rl->find<hscroll>("HSLIDE2");
	vslide1=rl->find<vscroll>("VSLIDE1");
	vslide2=rl->find<vscroll>("VSLIDE2");
	vslideiter=rl->find<vscroll>("VSLIDEITER");
	textiter=rl->find<text>("TEXTITER");
	textzoom=rl->find<text>("TEXTZOOM");
	text1=rl->find<text>("TEXT1");
	text2=rl->find<text>("TEXT2");
	pbutzoomin=rl->find<pbut>("PBUTZOOMIN");
	pbutzoomout=rl->find<pbut>("PBUTZOOMOUT");
	pbutquit=rl->find<pbut>("PBUTQUIT");
//	setresrange(rl,HSLIDE1,-256,256);
	hslide1->setminmaxval(-1024,1024);
//	setresrange(rl,HSLIDE2,-256,256);
	hslide2->setminmaxval(-1024,1024);
//	setresrange(rl,VSLIDE1,-256,256);
	vslide1->setminmaxval(-1024,1024);
//	setresrange(rl,VSLIDE2,-256,256);
	vslide2->setminmaxval(-1024,1024);
//	setresrange(rl,ITER,1,10000);
	vslideiter->setminmaxval(1,10000);
	xc=xf=yc=yf=0;
//	setresval(rl,HSLIDE1,xc);
	hslide1->setidx(xc);
//	setresval(rl,HSLIDE2,xf);
	hslide2->setidx(xf);
//	setresval(rl,VSLIDE1,yc);
	vslide1->setidx(yc);
//	setresval(rl,VSLIDE2,yf);
	vslide2->setidx(yf);
//	setresval(rl,ITER,10000);
	{
		char str[100];
		iter = 10;
		vslideiter->setidx(10001-iter);
		iter=10001-vslideiter->getidx();
		sprintf(str,"%d",iter);
		textiter->settname(str);
	}
//	setresname(rl,ITERTEXT,str);
	dozoom(0);
	focus = oldfocus = 0;
}

void riemann_proc()
{
	char str[100];
	int setz0=0;
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
// handle buttons and editboxes
	if (wininfo.mleftclicks || wininfo.mrightclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
	if (focus == pbutquit) {
		if (ret==1 || ret==2) {
			poporchangestate(STATE_MAINMENU);
		}
	} else if (focus == pbutzoomin) {
		if (ret==1 || ret==2) {
			dozoom(1);
		}
	} else if (focus == pbutzoomout) {
		if (ret==1 || ret==2) {
			dozoom(-1);
		}
	} else if (focus == hslide1) {
		xc=ret;
		setz0=1;
	} else if (focus == hslide2) {
		xf=ret;
		setz0=1;
	} else if (focus == vslide1) {
		yc=-ret;
		setz0=1;
	} else if (focus == vslide2) {
		yf=-ret;
		setz0=1;
	} else if (focus == vslideiter) {
		iter=10001-ret;
		sprintf(str,"%d",iter);
//		setresname(rl,ITERTEXT,str);
		textiter->settname(str);
	}
	if (setz0) {
		z0.r=xc*(1.0f/4.0f)+xf*(1.0f/2048.0f);
		z0.i=yc*(1.0f/4.0f)+yf*(1.0f/2048.0f);
		sprintf(str,"%f | %fi",z0.r,z0.i);
//		setresname(rl,TEXT1,str);
		text1->settname(str);
	}
}

void riemann_draw2d()
{
	cliprect32(B32,0,0,WX,WY,C32BLUE);
//	rect
	rl->draw();
	drawlines();
//	drawres(rl);
}

void riemann_exit()
{
//	freeres(rl);
	delete rl;
}

