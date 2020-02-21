#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <graph32\graph32.h>

struct bitmap data[13+20];
void readback(char *s)
{
struct bitmap d;
int i;
char t[30];
fileopen("numbers.lbm",READ);
iffread(&d,0,0,0,0,0,0,0);
fileclose();
alloc_bitmap(data,10,10,black);
clipblit(&d,data,100,100,0,0,10,10);
for (i=1;i<13;i++)
	{
	alloc_bitmap(data+i,3,5,black);
	clipblit(&d,data+i,1+4*i,1,0,0,3,5);
	}
free_bitmap(&d);
fileopen("objects.lbm",READ);
iffread(&d,0,0,0,0,0,0,0);
fileclose();
for (i=13;i<13+20;i++)
	{
	alloc_bitmap(data+i,9,5,black);
	clipblit(&d,data+i,1,1+6*(i-13),0,0,9,5);
	}
free_bitmap(&d);
}

struct bitmap b;

void copypict(int x,int y,int p)
{
clipxpar(data+p,&b,0,0,x,y,XSIZE,YSIZE,0);
}

int forecolor,backcolor;
void setfore(int c)
{
forecolor=c;
}

void setback(int c)
{
backcolor=c;
}

int mx,my,but;
extern int numpicts;

#define MAXOBJ 30
#define TENOBJ 10
#define f_lab	0
#define f_train 1

#define d_none 0
#define d_left	1
#define d_right 2
#define d_both	 3

#define o_none	  0
#define o_blowgun	1
#define o_arrow	2
#define o_bullet	3
#define o_lightbeam	4
#define o_detector  5

#define t_gall	  0
#define t_lorent 1
#define t_euclid 2

int xpos;
int rtime;
int play;
int train;
int frame;
double ftrain;

double fxpos;
double frtime;

double factor;

int curtrans;

void doobjs();
void setobj(int,int),setdir(int,int),setframe(int,int);

void setcobj(int,int),quit(),settrans(),gframe(int,int);

double inttofloat(int);
double tgall(),xgall();
double xtrain(double,double),ttrain(double,double);
double xlab(double,double),tlab(double,double);

void outchar2(struct bitmap *b,int x,int y,char c,int fc)
{
cliprect(b,x,y,x+7,y+7,backcolor);
outchar(b,x,y,c,fc);
}

void outtextxy2(struct bitmap *b,int x,int y,char *str,int fc)
{
cliprect(b,x,y,x+strlen(str)*8-1,y+7,backcolor);
outtextxy(b,x,y,str,fc);
}

void compile();

struct object
	{
	int posit,time,rep;
	double fposit;
	int frame,direct,object;
	int vel;
	};

struct object objects[MAXOBJ];
int curobj;		/* 0 - 9 */
char str[20];

void hilit(int v)
{
if (v)
	{
	setfore(0);
	setback(15);
	}
else
	{
	setfore(15);
	setback(0);
	}
}


int toscreen(x)
double x;
{
return((x-inttofloat(xpos))*50+320);
}

int slider(x0,y0,x1,y1,val)
int x0,y0,x1,y1;
int *val;
{
clipline(&b,x0,y0,x0,y1,forecolor);
clipline(&b,x1,y0,x1,y1,forecolor);
clipline(&b,x0,y0,x1,y0,forecolor);
clipline(&b,x0,y1,x1,y1,forecolor);
clipline(&b,x0+2,50-*val+y0,x1-2,50-*val+y0,forecolor);
clipline(&b,x0+2,y0+4,x0+6,y0+4,forecolor);
clipline(&b,x0+4,y0+2,x0+4,y0+6,forecolor);
clipline(&b,x0+2,y1-4,x0+6,y1-4,forecolor);
clipputpixel(&b,x0+1,y0+50,forecolor);
clipputpixel(&b,x1-1,y0+50,forecolor);
/* sprintf(str,"%3d %3d %3d",mx-x0-50,my-y0-50,*val);
outtextxy2(&b,0*8,2*8,str); */
if (!but)
	return(0);
if ((mx<x0)||(my<y0+2)||(mx>x1)||(my>y1-2))
	return(0);
*val=y0+50-my;
return(1);
}

void gadget(x0,y0,x1,y1,funct)
int x0,y0,x1,y1;
int (*funct)(int,int);
{
int cx,cy;
if (!but)
	return;
cx=mx>>3;
cy=my>>3;
if ((cx<x0)||(cx>x1)||(cy<y0)||(cy>y1))
	return;
(*funct)(cx-x0,cy-y0);
}

void drawnum(x,y,n)
int x,y;
double n;
{
int p;
char *strptr;
x-=12;
strptr=str;
sprintf(str,"%4.1lf",n);
while(*strptr)
	{
	switch(*strptr)
		{
		case	'-':	p=11;
						break;
		case	'.':	p=12;
						break;
		default:		p=*strptr-'0'+1;
		}
	strptr++;
	x+=4;
	if (p<0)
		continue;
	copypict(x,y,p);
	}
}

void doruler()
{
int i;
int off;
int left,right;
left=toscreen(xlab(-10.0,frtime));
right=toscreen(xlab(10.0,frtime));
setfore(red);
clipline(&b,left,330,right,330,forecolor);
clipline(&b,left,347,right,347,forecolor);
for (i=-10;i<=10;i++)
	{
	off=toscreen(xlab((double)(i),frtime));
	clipline(&b,off,330,off,325,forecolor);
	drawnum(off,335,(double)(i));
	drawnum(off,343,tlab((double)(i),frtime));
	}
setfore(green);
left=toscreen(xtrain(-10.0,frtime));
right=toscreen(xtrain(10.0,frtime));
clipline(&b,left,290,right,290,forecolor);
clipline(&b,left,308,right,308,forecolor);
for (i=-10;i<=10;i++)
	{
	off=toscreen(xtrain((double)(i),frtime));
	clipline(&b,off,308,off,313,forecolor);
	drawnum(off,295,(double)(i));
	drawnum(off,303,ttrain((double)(i),frtime));
	}
setfore(white);
}

main()
{
int i,c;
mem_init();
alloc_bitmap(&b,HXSIZE,350,-1);
readback("numb");
hvga_init();
initmouse();
setmousearea(0,0,HXSIZE-1,350-1);
setmousexy(301*2,70*2);
while(1)
	{
	but=getmousexy(&mx,&my);
	c=getkey();
	if ((c>='0')&&(c<='9'))
		curobj=c-'0';
	switch(c)
		{
		case K_UP:	setmousexy(mx,--my);
						but=1;
						break;
		case K_DOWN:	setmousexy(mx,++my);
						but=1;
						break;
		case K_PGUP:	my-=5;
						setmousexy(mx,my);
						but=1;
						break;
		case K_PGDN:	my+=5;
						setmousexy(mx,my);
						but=1;
						break;
		case '\r':	but=1;
						break; 
		case 'x':	setmousexy(360,72);
						break;
		case 's':	setmousexy(438,72);
						break;
		case 'd':	setmousexy(470,72);
						break;
		case 'p':	setmousexy(342,236);
						break;
		case 'i':	setmousexy(504,72);
						break;
		case 'a':	frame=f_lab;
						break;
		case 'r':	frame=f_train;
						break;
		case 'g':	curtrans=t_gall;
						break;
		case 'z':	curtrans=t_lorent;
						break;
		case 'q':	quit();

		case 'l':	objects[curobj].frame=f_lab;
						break;
		case 't':	objects[curobj].frame=f_train;
						break;
		case 'n':	objects[curobj].direct=d_none;
						break;
		case 'f':	objects[curobj].direct=d_left;
						break;
		case 'h':	objects[curobj].direct=d_right;
						break;
		case 'b':	objects[curobj].direct=d_both;
						break;

		case 'o':	objects[curobj].object=o_none;
						break;
		case 'u':	objects[curobj].object=o_blowgun;
						break;
		case 'w':	objects[curobj].object=o_arrow;
						break;
		case 'e':	objects[curobj].object=o_bullet;
						break;
		case 'm':	objects[curobj].object=o_lightbeam;
						break;
		case 'c':	objects[curobj].object=o_detector;
						break;
		case 'y':	setmousexy(380,236);
						break;
		case 'k':	setmousexy(412,236);
						break;
		}
//	mx<<=1;
//	my<<=1;
	hilit(0);
	cliprect(&b,0,0,HXSIZE-1,350-1,black);
	gadget(54,24,58,26,setframe); 
	gadget(60,24,64,30,setdir);
	gadget(67,24,75,34,setobj);
	gadget(37,4,39,22,setcobj);
	gadget(73,9,80,11,settrans); 
	gadget(67,3,71,5,gframe); 
	gadget(73,17,76,17,quit);
	slider(348,22,366,122,&xpos);
	if (slider(426,22,444,122,&rtime))
		play=0;
	slider(458,22,476,122,&play);
	slider(492,22,526,122,&train);
/*	slider(542,22,568,122,&rotate); */
	slider(330,186,352,286,&objects[curobj].posit);
	slider(366,186,388,286,&objects[curobj].time);
	slider(404,186,420,286,&objects[curobj].rep);
	if (objects[curobj].rep<0)
		objects[curobj].rep=0;
	rtime+=((play+1)>>2);
	if (xpos>48)
		xpos-=96;
	if (xpos<-48)
		xpos+=96;
	if (rtime>48)
		rtime-=96;
	if (rtime<-48)
		rtime+=96; 
	clipline(&b,0,0,HXSIZE-1,0,forecolor);
	clipline(&b,0,0,0,350-1,forecolor);
	clipline(&b,0,350-1,HXSIZE-1,350-1,forecolor);
	clipline(&b,HXSIZE-1,0,HXSIZE-1,350-1,forecolor);
	clipline(&b,300,0,300,288,forecolor);
	clipline(&b,320,0,320,288,forecolor);
	clipline(&b,0,288,HXSIZE-1,288,forecolor);
	clipline(&b,320,150,HXSIZE-1,150,forecolor);
	outtextxy2(&b,41*8,1*8,"X pos     t Set spD traIn frame",forecolor);
	hilit(frame==f_lab);
	outtextxy2(&b,67*8,3*8,"lAb",forecolor);
	hilit(frame==f_train);
	outtextxy2(&b,67*8,5*8,"tRain",forecolor);
/*	hilit(curtrans==t_euclid);
	outtextxy2(&b,73*8,9*8,"euclid",forecolor); */
	hilit(curtrans==t_gall);
	outtextxy2(&b,73*8,9*8,"Gall",forecolor);
	hilit(curtrans==t_lorent);
	outtextxy2(&b,72*8,11*8,"lorentZ",forecolor);
	hilit(0);
	outtextxy2(&b,73*8,17*8,"Quit",forecolor);
	outtextxy2(&b,53*8,20*8,"OBJECTS",forecolor);
	outtextxy2(&b,41*8,22*8,"Pos tYme pK  frame direct object",forecolor);
	hilit(objects[curobj].frame==f_lab);
	outtextxy2(&b,54*8,24*8,"Lab",forecolor);
	hilit(objects[curobj].frame==f_train);
	outtextxy2(&b,54*8,26*8,"Train",forecolor);
	hilit(objects[curobj].direct==d_none);
	outtextxy2(&b,60*8,24*8,"None",forecolor);
	hilit(objects[curobj].direct==d_left);
	outtextxy2(&b,60*8,26*8,"leFt",forecolor);
	hilit(objects[curobj].direct==d_right);
	outtextxy2(&b,60*8,28*8,"rigHt",forecolor);
	hilit(objects[curobj].direct==d_both);
	outtextxy2(&b,60*8,30*8,"Both",forecolor);
	hilit(objects[curobj].object==o_none);
	outtextxy2(&b,67*8,24*8,"nOne",forecolor);
	hilit(objects[curobj].object==o_blowgun);
	outtextxy2(&b,67*8,26*8,"blowgUn",forecolor);
	hilit(objects[curobj].object==o_arrow);
	outtextxy2(&b,67*8,28*8,"arroW",forecolor);
	hilit(objects[curobj].object==o_bullet);
	outtextxy2(&b,67*8,30*8,"bullEt",forecolor);
	hilit(objects[curobj].object==o_lightbeam);
	outtextxy2(&b,67*8,32*8,"lightbeaM",forecolor);
	hilit(objects[curobj].object==o_detector);
	outtextxy2(&b,67*8,34*8,"deteCtor",forecolor);
	for (i=0;i<TENOBJ;i++)
		{
		hilit(curobj==i);
		outchar2(&b,38*8,(i+2)<<4,'0'+i,forecolor);
		}					
	copypict(mx,my,0);
	sprintf(str,"%3d %3d",mx,my);
	outtextxy2(&b,0,0,str,forecolor);
	fxpos=inttofloat(xpos);
	frtime=inttofloat(rtime);
	ftrain=train/50.0;
	factor=sqrt(1-ftrain*ftrain);
	compile();
	doruler(); 
	doobjs();
	setfore(cyan);
	setback(black);
	sprintf(str,"%4.1lf",fxpos);
	outtextxy2(&b,46*8,17*8,str,forecolor);   
	sprintf(str,"%4.1lf",frtime);
	outtextxy2(&b,54*8,17*8,str,forecolor);   
	sprintf(str,"%5.2lf",ftrain);
	outtextxy2(&b,61*8,17*8,str,forecolor);   
	sprintf(str,"%4.1lf",inttofloat(objects[curobj].posit));
	outtextxy2(&b,40*8,21*8,str,forecolor);   
	sprintf(str,"%4.1lf",inttofloat(objects[curobj].time));
	outtextxy2(&b,45*8,21*8,str,forecolor);   
	sprintf(str,"%4.1lf",inttofloat(	objects[curobj].rep));
	outtextxy2(&b,51*8,21*8,str,forecolor);   
	hvga_blit_to(&b,0,0,0,0,HXSIZE,350);
	}
}

void setcobj(x,y)
int x,y;
{
if ((y&1)==0)
	curobj=y>>1;
}

void settrans(x,y)
int x,y;
{
if ((y&1)==0)
	curtrans=y>>1;
}

void quit()
{
int i;
closegraph();
free_bitmap(&b);
for (i=0;i<13+20;i++)
	free_bitmap(data+i);
exit(0);
}

void setframe(x,y)
int x,y;
{
objects[curobj].frame=y>>1;
};

void gframe(x,y)
int x,y;
{
frame=y>>1;
};

void setobj(x,y)
int x,y;
{
objects[curobj].object=y>>1;
};

void setdir(x,y)
int x,y;
{
objects[curobj].direct=y>>1;
};


void doobjs()
{
int i,x,y,p;
double xoff,toff,t;
struct object *objptr=objects;
for (i=0;i<MAXOBJ;i++)
	{
	if (objptr->object)
		{
		p=objptr->object*4+11;
		toff=inttofloat(objptr->time);
		if (i<10)
			xoff=inttofloat(objptr->posit);
		else
			xoff=objptr->fposit;
		if (objptr->frame==f_lab)
			{
			y=325;
			x=toscreen(xlab(xoff,frtime));
			t=tlab(xoff,frtime);
			}
		else
			{
			y=315;
			x=toscreen(xtrain(xoff,frtime));
			t=ttrain(xoff,frtime);
			}
		if (((toff+.5>=t)&&(toff-.5<=t))||(i>=10))
			p-=2;
		if (objptr->direct==d_left)
			{
			p++;
			x-=4;
			}
		else
			{
			x-=4;
			}
		copypict(x,y,p);
		}
	objptr++;
	}
}


double inttofloat(x)
{
return(x/5.0);
}

double xgall(x,t,v)
double x,t,v;
{
return(v*t+x);
}

double tgall(x,t,v)
double x,t,v;
{
return (t);
}

double xlorent(x,t,v)
double x,t,v;
{
return(factor*x+v*t);
}

double tlorent(x,t,v)
double x,t,v;
{
return (factor*t-v*x);
}

double xtrain(x,t)
double x,t;
{
if (frame==f_train)
	return(x);
if (curtrans==t_gall)
	return(xgall(x,t,ftrain));
return(xlorent(x,t,ftrain));
}

double ttrain(x,t)
double x,t;
{
if (frame==f_train)
	return(t);
if (curtrans==t_gall)
	return(tgall(x,t,ftrain));
return(tlorent(x,t,ftrain));
}

double xlab(x,t)
double x,t;
{
if (frame==f_lab)
	return(x);
if (curtrans==t_gall)
	return(xgall(x,t,-ftrain));
return(xlorent(x,t,-ftrain));
}

double tlab(x,t)
double x,t;
{
if (frame==f_lab)
	return(t);
if (curtrans==t_gall)
	return(tgall(x,t,-ftrain));
return(tlorent(x,t,-ftrain));
}

void compile()
{
int i;
for (i=0;i<TENOBJ;i++)
	{
	objects[i+TENOBJ].object=objects[i+2*TENOBJ].object=0;
	if ((objects[i].object)&&(objects[i].direct)&&(objects[i].object!=o_detector))
		{
		if ((curtrans==t_gall)||(objects[i].frame==frame))
			{
			if (rtime<objects[i].time)
				continue;
			}
		else
			if (frame==f_lab)
				{
				if (frtime*factor<inttofloat(objects[i].time)+ftrain*inttofloat(objects[i].posit))
					continue;
				}
			else
				{
				if (frtime*factor<inttofloat(objects[i].time)-ftrain*inttofloat(objects[i].posit))
					continue;
				}
		if (objects[i].direct&d_left)
			objects[i+TENOBJ].object=objects[i].object;
		if (objects[i].direct&d_right)
			objects[i+2*TENOBJ].object=objects[i].object;
		objects[i+2*TENOBJ].direct=d_right;
		objects[i+  TENOBJ].direct=d_left;
		objects[i+2*TENOBJ].frame=objects[i+TENOBJ].frame=objects[i].frame;
		if ((curtrans==t_gall)||(objects[i].frame==frame))
			{
			objects[i+2*TENOBJ].fposit=inttofloat(objects[i].posit)+objects[i].object*(frtime-inttofloat(objects[i].time))/4;
			objects[i+  TENOBJ].fposit=inttofloat(objects[i].posit)-objects[i].object*(frtime-inttofloat(objects[i].time))/4;
			}
		else
			{
			if (frame==f_lab)
				{
				objects[i+2*TENOBJ].fposit=(inttofloat(objects[i].posit)+objects[i].object*(factor*frtime-inttofloat(objects[i].time))/4)/(1+ftrain*objects[i].object/4);
				objects[i+  TENOBJ].fposit=(inttofloat(objects[i].posit)-objects[i].object*(factor*frtime-inttofloat(objects[i].time))/4)/(1-ftrain*objects[i].object/4);
				}
			else
				{
				objects[i+2*TENOBJ].fposit=(inttofloat(objects[i].posit)+objects[i].object*(factor*frtime-inttofloat(objects[i].time))/4)/(1-ftrain*objects[i].object/4);
				objects[i+  TENOBJ].fposit=(inttofloat(objects[i].posit)-objects[i].object*(factor*frtime-inttofloat(objects[i].time))/4)/(1+ftrain*objects[i].object/4);
				}
			}
		}
	}
}

