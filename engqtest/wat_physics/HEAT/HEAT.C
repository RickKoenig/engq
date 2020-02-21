#include <dos.h>
#include <stdlib.h>
#include <math.h>

#include <graph32\graph32.h>

struct bitmap v;
struct bitmap b;

#define CONDUCT .2

double deg[XSIZE];
double backup[XSIZE];

void doheat()
{
int i;
int a,b;
if (random(30)==1)
	{
	a=random(XSIZE);
	b=random(XSIZE);
	i=deg[a];
	deg[a]=deg[b];
	deg[b]=i;
	}
deg[XSIZE-1]=0.0;
for (i=0;i<XSIZE;i++)
	backup[i]=deg[i];
backup[0]+=CONDUCT*(deg[1]-deg[0]);
for (i=1;i<XSIZE-1;i++)
	backup[i]+=CONDUCT*(deg[i+1]+deg[i-1]-2*deg[i]);
for (i=0;i<XSIZE;i++)
	{
	deg[i]=backup[i];
//	printf("%d %f\n",i,deg[i]);
	}
}

void initdeg()
{
int i;
for (i=0;i<XSIZE;i++)
	deg[i]=190.0;
}

void drawdeg()
{
int i;
for (i=0;i<XSIZE;i++)
	clipputpixel(&b,i,YSIZE-5-deg[i],white);
}

void main()
{
int but;
int x,y,i;
mem_init();
randomize();
initdeg();
initgraph();
initmouse();
make_video_bitmap(&v);
alloc_bitmap(&b,XSIZE,YSIZE,0);
while(1)
	{
	but=getmousexy(&x,&y);
	if (but!=0)
		break;
	fastrect(&b,0,0,XSIZE-1,YSIZE-1,black);
	drawdeg();
	for (i=0;i<10;i++)
		doheat();
	fastblit(&b,&v,0,0,0,0,XSIZE,YSIZE);
	}
closegraph();
free_bitmap(&b);
}

