#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <graph32/graph32.h>

#define NUMFRAMES 256
#define RADIUS 82	// measured from graphic
#define GRAV .1 //
#define PI 3.14159

double ftab[8]={.00001,.00005,.0001,.001,.002,.005,.01,0};
int fricvar=0;

double fric(double v)
{
return (1-ftab[fricvar])*v;
}

int ftointrnd(double f)
{
if (f>0)
	return f+.5;
return f-.5;
}

void main()
{
char str[80];
int key;
int mx,my,but;
int x,y,oldx=XSIZE/2,oldy=YSIZE/2;
int vx,vy,oldvx=0,oldvy=0;

double ax,ay;
double angv=0,angd=PI,anga;

double torque;

struct folder *f;
struct sprite s;
struct bitmap b,v;

mem_init();

alloc_bitmap(&b,XSIZE,YSIZE,-1);
make_video_bitmap(&v);
if (isolder("pend0001.lbm","pendu.spt")>0)
	f=foldersptread("pendu",0);
else
	{
	f=folderiffread("pend.lbm",0,0,255,-1,-1);
	foldersptwrite(f,"pendu");
	}
sptlink(&s,f,0); // hook up fld
sptmove(&s,160,100);
initgraph();
initmouse();	// mouseset at 160 100

while(1)
	{
	key=getkey();
	but=getmousexy(&mx,&my);
	if (but || key=='q' || key==K_ESC)
		break;
	if (key=='f')
		{
		fricvar++;
		fricvar&=7;
		}
	cliprect(&b,0,0,XSIZE-1,YSIZE-1,black);
///////// start physics /////////
	x=mx;
	y=my;
	vx=x-oldx;
	vy=y-oldy;
	ax=vx-oldvx;
	ay=vy-oldvy-GRAV;
	oldx=x;
	oldy=y;
	oldvx=vx;
	oldvy=vy;
	
	torque=ax*cos(angd)+ay*sin(angd);

	anga=torque/RADIUS;
	angv+=anga;
	angv=fric(angv);
	angd+=angv;
	if (angd<0)
		angd+=2*PI;
	if (angd>2*PI)
		angd-=2*PI;
///////// end physics /////////
	sptmove(&s,x,y);
	sptframe(&s,NUMFRAMES);
	sptshow(&s,&b);

/*	sptframe(&s,NUMFRAMES+1);
	sptmove(&s,10,10);
	sptshow(&s,&b); */

	sptmove(&s,x,y);
	sptframe(&s,ftointrnd(angd*NUMFRAMES/(2*PI))&(NUMFRAMES-1));
	sptshow(&s,&b);

	sprintf(str,"ax=%6.3f,ay=%6.3f,vx=%3d,vy=%3d",ax,ay,vx,vy);
	outtextxy(&b,0,0,str,white);
	sprintf(str,"angd=%6.3f,angv=%6.3f,anga=%8.5f",angd,angv,anga);
	outtextxy(&b,0,8,str,white);
	sprintf(str,"fric=%8.5f",ftab[fricvar]);
	outtextxy(&b,0,16,str,white);
	
	
	waitvb();
	clipblit(&b,&v,0,0,0,0,XSIZE,YSIZE);
	}
closegraph();
folderfree(f);				// free folder 
free_bitmap(&b);
}

