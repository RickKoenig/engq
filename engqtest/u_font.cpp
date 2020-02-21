



#if 0
{
// test joystick code
// test nuqsort
#include <engine1.h>

#include "user1.h"

#define NUMELE 30
static float farray[NUMELE],farray2[NUMELE];

//static struct bitmap8 *b;
static struct font8 *afont,*afont2;

static char *teststr="A quick brown fox jumps over the lazy dog.\n"
	"A stitch in time saves nine.";

static int fcomp(void *array,int i1,int i2)
{
	float *farr=(float *)array;
	return farr[i1] < farr[i2];
}

void user2init()
{
	int i;
	for (i=0;i<NUMELE;i++)
		farray[i]=farray2[i]=frand()*100.0f;
	heapsort(farray2,NUMELE,fcomp);
//	nuqsort(farray2,NUMELE,fcomp);
//	b=getwindowbitmap8();
	setdir("user2");
	afont=loadfont8("arial11.lbm");
	afont2=loadfont8("impact17.lbm");
	video_setupwindow(640,480,8);
	wininfo.joyenable=1;
//	makefixedfont(afont,9);


}

void user2proc()
{
	int i;
//	int k[8];
//	int key;
//	float jx,jy;
//	int mx,my;
//	int jbut;
//	key=getglobalkey();
//	jbut=getjoy(&jx,&jy);
//	getmousexy(&mx,&my);
//	numberkeychangestate();
	video_lock();
	cliprect(B8,0,0,WX-1,WY-1,blue);
	cliprect(B8,250,30,MX,MY,red);
	fontwindow3(afont,B8,250,30,MX,MY,
		white,lightblue,green,teststr);
//	outfontxy3(afont,B8,20,5,white,-1,-1,"jx %6.2f, jy %6.2f, jbut %d",jx,jy,jbut);
	for (i=0;i<NUMELE;i++)
		outfontxy3(afont,B8,40,20+i*13,green,-1,-1,"%2d",i);
	for (i=0;i<NUMELE;i++)
		outfontxy3(afont,B8,70,20+i*13,white,-1,-1,"%f",farray[i]);
	for (i=0;i<NUMELE;i++)
		outfontxy3(afont,B8,160,20+i*13,yellow,-1,-1,"%f",farray2[i]);

/*	jx=2.0f;
	k[0]=(int)jx;
	jx=1.9999f;
	k[1]=(int)jx;
	jx=.7f;
	k[2]=(int)jx;
	jx=.3f;
	k[3]=(int)jx;
	jx=-.3f;
	k[4]=(int)jx;
	jx=-.7f;
	k[5]=(int)jx;
	jx=-1.3f;
	k[6]=(int)jx;
	jx=-1.7f;
	k[7]=(int)jx;
	outtextxyf(b,160,300,white,"%d %d %d %d %d %d %d %d",
		k[0],k[1],k[2],k[3],k[4],k[5],k[6],k[7]); */
	outfontrectxy(afont,B8,160,430,black,teststr);
	outfontxy3(afont,B8,160,430,white,blue,red,teststr);
	outfontrectxy(afont2,B8,160,450,black,teststr);
	outfontxy3(afont2,B8,160,450,white,blue,red,teststr);
	video_unlock();
}

void user2exit()
{
	freefont8(afont);
	freefont8(afont2);
	wininfo.joyenable=0;
}

struct font8 *loadfont8(char *filename);
void freefont8(struct font8 *);

void makefixedfont(struct font8 *f,int xsize);
//void changefontspace(struct font8 *f,int space);

int getfontlenx(struct font8 *f,char *str,...);
int getfontleny(struct font8 *f);

//void outfontxy(struct font8 *f,struct bitmap8 *b,int x,int y,int col,char *str,...);
void outfontxy3(struct font8 *f,struct bitmap8 *b,int x,int y,int col1,int col2,int col3,char *str,...);
void outfontrectxy(struct font8 *f,struct bitmap8 *b,int x,int y,int col,char *str,...);

//void fontwindow(struct font8 *f,struct bitmap8 *b,int sx,int sy,int ex,int ey,int col,char *str,...);
void fontwindow3(struct font8 *f,struct bitmap8 *b,int sx,int sy,int ex,int ey,int col1,int col2,int col3,char *str,...);



#include <stdio.h>
#include <stdarg.h>
#include "engine1.h"

#define NUMCHARS 96
struct font8 {
	struct bitmap8 *b8;
	struct recti2 r[NUMCHARS];
	int logstartx[NUMCHARS];
	int logwidth[NUMCHARS];
	int height;
};

static char strbuff[2000];

// 255 box
// 15 color1
// 14 color2
// 13 color3
// 0 xpar
// 1 logical width
struct font8 *loadfont8(char *filename)
{
	struct font8 *f;
	struct bitmap8 *p;
	int count=0;
	int i,j,a,b;
	int width;
	int height;
	f=(struct font8 *)memalloc(sizeof(struct font8));
	fileopen(filename,READ);
	p=gfxread8(NULL);
	fileclose();
	f->b8=p;
	width=p->x;
	height=p->y;
	for (j=0;j<height;j++)
		for (i=0;i<width;i++)
			if (clipgetpixel(p,i,j)==255)
				if (clipgetpixel(p,i+1,j)==255)
					if (clipgetpixel(p,i,j+1)==255)
						if (clipgetpixel(p,i,j-1)!=255) {
							f->r[count].topleft.x=i+1;
							f->r[count].topleft.y=j+1;
							for (a=i,b=j;clipgetpixel(p,a+1,b)==255;a++)
								;
							for (;clipgetpixel(p,a,b+1)==255;b++)
								;
							f->r[count].size.x=a-i-1;
							f->r[count].size.y=b-j-1;
							if (count==0)
								f->height=b-j-1;
							else if (f->height!=b-j-1)
								errorexit("trouble loading font, inconsistent heights '%c'",count+32);
							count++;
							if (count>NUMCHARS)
								errorexit("font: too many chars");
						}
	if (count!=NUMCHARS)
		errorexit("font: too few chars");

	for (i=0;i<count;i++) {
		int markleft=f->r[i].topleft.x;
		int markright=f->r[i].topleft.x+f->r[i].size.x-1;
		for (a=f->r[i].topleft.x;a<=f->r[i].topleft.x+f->r[i].size.x+1;a++)
			for (b=f->r[i].topleft.y-2;b>f->r[i].topleft.y-20;b--) {
			if (clipgetpixel(p,a,b)==1) {
				markleft=a;
				while(clipgetpixel(p,markleft,b)==1)
					markleft--;
				markleft++;

				markright=a;
				while(clipgetpixel(p,markright,b)==1)
					markright++;
				markright--;

				goto donerefs;
			}
		}
		donerefs:
		f->logstartx[i]=markleft-f->r[i].topleft.x;
		f->logwidth[i]=markright-markleft+1;
	}
	return f;
}

void freefont8(struct font8 *f)
{
	bitmap8free(f->b8);
	memfree(f);
}

void makefixedfont(struct font8 *f,int width)
{
	int i;
	for (i=0;i<NUMCHARS;i++) {
		f->logwidth[i]=width;
		f->logstartx[i]=0;
	}
}



int getfontlenx(struct font8 *f,char *str,...)
{
	int len=0;
	int val;
	va_list arglist;
	va_start(arglist,str);
	vsprintf(strbuff,str,arglist);
	va_end(arglist);
	str=strbuff;
	if (*str>=32 && *str<128)
		len+=f->logstartx[*str-32];
	else
		return 0;
	while(val=*str++) {
		if (val<32 || val>=128)
			break;
		val-=32;
		len+=f->logwidth[val];
	}
	str-=2;
	val=*str;
	val-=32;
	len-=f->logwidth[val];
	len+=f->r[val].size.x-f->logstartx[val];
	return len;
}

int getfontleny(struct font8 *f)
{
	return f->height;
}

void outfontxy3(struct font8 *f,struct bitmap8 *b,int x,int y,int col1,int col2,int col3,char *str,...)
{
	int val;
	unsigned char andormask[512];
	va_list arglist;
	va_start(arglist,str);
	vsprintf(strbuff,str,arglist);
	va_end(arglist);
	str=strbuff;
	clear32(andormask    ,256,0xff);
	clear32(andormask+256,256,0);
	if (col1>=0) {
		andormask[white]=0;
		andormask[white+256]=col1;
	}
	if (col2>=0) {
		andormask[14]=0;
		andormask[14+256]=col2;
	}
	if (col3>=0) {
		andormask[13]=0;
		andormask[13+256]=col3;
	}
	if (*str>=32 && *str<128)
		x+=f->logstartx[*str-32];
	while(val=*str++) {
		if (val<32 || val>=128)
			break;
		val-=32;
		clipmask(f->b8,b,
			f->r[val].topleft.x,f->r[val].topleft.y,
			x-f->logstartx[val],y,
			f->r[val].size.x,f->r[val].size.y,
			andormask);
			x+=f->logwidth[val];
	}
}

void outfontrectxy(struct font8 *f,struct bitmap8 *b,int x,int y,int col,char *str,...)
{
	va_list arglist;
	va_start(arglist,str);
	vsprintf(strbuff,str,arglist);
	va_end(arglist);
	str=strbuff;
	cliprect(b,x,y,x+getfontlenx(f,str)-1,y+getfontleny(f)-1,col);
}


void fontwindow3(struct font8 *f,struct bitmap8 *b,int sx,int sy,int ex,int ey,int col1,int col2,int col3,char *str,...)
{
	struct recti2 saverect;
	unsigned char *ptr,*ptr2,*remem;
	int good;
	saverect=b->cliprect;
	b->cliprect.topleft.x=sx;
	b->cliprect.topleft.y=sy;
	b->cliprect.size.x=ex-sx+1;
	b->cliprect.size.y=ey-sy+1;
	if (b->cliprect.topleft.x<saverect.topleft.x) {
		b->cliprect.size.x+=b->cliprect.topleft.x-saverect.topleft.x;
		b->cliprect.topleft.x=saverect.topleft.x;
	}
	if (b->cliprect.topleft.y<saverect.topleft.y) {
		b->cliprect.size.y+=b->cliprect.topleft.y-saverect.topleft.y;
		b->cliprect.topleft.y=saverect.topleft.y;
	}
	if (b->cliprect.size.x>saverect.size.x) {
		b->cliprect.size.x=saverect.size.x;
	}
	if (b->cliprect.size.y>saverect.size.y) {
		b->cliprect.size.y=saverect.size.y;
	}
	ptr=str;
	while(*ptr) {
		if (getfontlenx(f,ptr)>ex-sx+1) {
			ptr2=ptr;
			while(1) {
				if (*ptr2==' ') {
					good=1;
					break;
				} else if (*ptr2=='\n' || *ptr2=='\0') {
					good=0;
					break;
				}
				ptr2++;
			}
			if (good) {
				*ptr2=(char)0xff;
				if (getfontlenx(f,ptr)>ex-sx+1)
					;
				else {
					while(1) {
						remem=ptr2;
						if (*ptr2==0xff)
							*ptr2=' ';
						ptr2++;
						while(1) {
							if (*ptr2=='\n' || *ptr2 =='\0' || *ptr2 == ' ')
								break;
							ptr2++;
						}
						if (*ptr2==' ')
							*ptr2=(char)0xff;
						if (getfontlenx(f,ptr)>ex-sx+1) {
							if (*ptr2==0xff)
								*ptr2=' ';
							*remem=(char)0xff;
							break;
						}
					}
				}
			}
		}
		outfontxy3(f,b,sx,sy,col1,col2,col3,ptr);
		while(*ptr!='\n' && *ptr!='\0' && *ptr !=0xff)
			ptr++;
		if (*ptr!='\0') {
			sy+=f->height+1;
			ptr++;
		}
	}
	ptr=str;
	while(*ptr) {
		if (*ptr==0xff)
			*ptr=' ';
		ptr++;
	}
	b->cliprect=saverect;
}

}
#endif
