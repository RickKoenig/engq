#include <graph32\graph32.h>
#include <math.h>

#define NUMOBJS 130
// #define DEBUG
#define ATTRACT -10
#define REPEL	600
// #define FRICTION .05
#define FRICTION .002

struct obj
	{
	double x,y;
	double vx,vy;
	double ax,ay;
	} objs[NUMOBJS];

struct bitmap b;

void showobjs()
{
struct obj *p;
cliprect(&b,0,0,HXSIZE-1,HYSIZE-1,black);
for (p=objs;p<objs+sizeof(objs)/sizeof(struct obj);p++)
	clipputpixel(&b,p->x,p->y,white);	
}

void accelobjs()
{
double r,r2,r3,r4;
double x,y;
double ax,ay;
struct obj *p1,*p2;
cliprect(&b,0,0,HXSIZE-1,HYSIZE-1,black);
for (p1=objs;p1<objs+sizeof(objs)/sizeof(struct obj);p1++)
	{
	p1->ax=0;
	p1->ay=0;
	}
for (p1=objs;p1<objs+sizeof(objs)/sizeof(struct obj)-1;p1++)
	for (p2=p1+1;p2<objs+sizeof(objs)/sizeof(struct obj);p2++)
		{
		x=p2->x-p1->x;
		y=p2->y-p1->y;
		r2=x*x+y*y;
		r=sqrt(r2);
		r3=r*r2;
		r4=r2*r2;
		ax=ATTRACT*x/r3+REPEL*x/r4;
		ay=ATTRACT*y/r3+REPEL*y/r4;
		p1->ax-=ax;
		p1->ay-=ay;
		p2->ax+=ax;
		p2->ay+=ay;
		}
for (p1=objs;p1<objs+sizeof(objs)/sizeof(struct obj);p1++)
	{
	p1->vx-=p1->vx*FRICTION;
	p1->vy-=p1->vy*FRICTION;
	}
}

void moveobjs()
{
struct obj *p;
for (p=objs;p<objs+sizeof(objs)/sizeof(struct obj);p++)
	{
	p->vx+=p->ax;
	p->x+=p->vx;
	if (p->x>=HXSIZE)
		{
		p->x=HXSIZE*2-p->x;
		p->vx=-p->vx;
		}
	else if (p->x<0)
		{
		p->x=-p->x;
		p->vx=-p->vx;
		}
	p->vy+=p->ay;
	p->y+=p->vy;
	if (p->y>=HYSIZE)
		{
		p->y=HYSIZE*2-p->y;
		p->vy=-p->vy;
		}
	else if (p->y<0)
		{
		p->y=-p->y;
		p->vy=-p->vy;
		}
	}
}

void initobjs()
{
struct obj *p1,*p2;
int x,y;
for (p1=objs;p1<objs+sizeof(objs)/sizeof(struct obj);)
	{
	p1->x=random(HXSIZE);
	p1->y=random(HYSIZE);
	p1->vx=random(5)-2;
	p1->vy=random(5)-2;
	for (p2=objs;p2<p1;p2++)
		{
		x=p1->x-p2->x;
		y=p1->y-p2->y;
		if (x*x+y*y<400)
			break;
		}
	if (p1==p2)
		p1++;
	}
}

void main()
{
mem_init();
#ifndef DEBUG
hvga_init();
#endif
alloc_bitmap(&b,HXSIZE,HYSIZE,-1);
initobjs();
while(!getkey())
	{
	accelobjs();
	moveobjs();
#ifndef DEBUG
	showobjs();
	hvga_blit_to(&b,0,0,0,0,HXSIZE,HYSIZE);
#endif
	}
#ifndef DEBUG
closegraph();
#endif
free_bitmap(&b);
}


