#include <graph32r\graph32r.h>

int cdecl fastmand();

int palette=4;
int depth=240;
double zoom=200.0;
double cmx=0.0;
double cmy=0.0;
int x,y;
char c;

int mousezoom=300;

/*
void rect(int x0,int y0,int x1,int y1,int col)
{
int i,j;
for (i=x0;i<=x1;i++)
	for (j=y0;j<=y1;j++)
		tvga_wtdot(i,j,col);
}
*/

unsigned char alldac[256][3];

void initdata()
{
tvga_setscreen(255);
}

void initdac0(int d)
{
int i;
for (i=0;i<d;i++)
	{
		{
		alldac[i][0]=0x3f*i/d;
		alldac[i][1]=0x3f*i/d;
		alldac[i][2]=0x3f*i/d;
		}
	switch(i&3)
		{
		case 0:	alldac[i][0]/=2;
				 	break;
		case 1:	alldac[i][1]/=2;
				 	break;
		case 2:	alldac[i][2]/=2;
				 	break;
		}
	}
alldac[i][0]=0;
alldac[i][1]=0;
alldac[i][2]=0;
}

int getcol(int val)
{
val&=0xff;
switch(val>>6)
	{
	case	0: return(val&0x3f);
	case	1: return(0x3f);
	case	2: val = 0x3f - val;
				return(val&0x3f);
	case	3: return(0);
	}
return(0);
}

void initdac1(int d)
{
int i;
for (i=0;i<d;i++)
	{
	alldac[i][0]=0x3f*i/d;
	alldac[i][1]=0x3f*i/d;
	alldac[i][2]=0x3f*i/d;
	}
alldac[i][0]=0;
alldac[i][1]=0;
alldac[i][2]=0;
}

void initdac2(int d)
{
int i;
for (i=0;i<d;i++)
	{
	alldac[i][0]=getcol(0x100*i/d+0x40);
	alldac[i][1]=getcol(0x100*i/d-0x40);
	alldac[i][2]=getcol(0x100*i/d);
	}
alldac[i][0]=0;
alldac[i][1]=0;
alldac[i][2]=0;
}

void initdac3(int d)
{
int i;
for (i=0;i<d;i++)
	{
	alldac[i][0]=getcol(0x100*i/d);
	alldac[i][1]=getcol(0x100*i/d+0x40);
	alldac[i][2]=getcol(0x100*i/d-0x40);
	}
alldac[i][0]=0;
alldac[i][1]=0;
alldac[i][2]=0;
}

void initdac4(int d)
{
int i;
for (i=0;i<d;i++)
	{
	alldac[i][0]=getcol(0x100*i/d);
	alldac[i][1]=getcol(0x100*i/d-0x40);
	alldac[i][2]=getcol(0x100*i/d+0x40);
	}
alldac[i][0]=0;
alldac[i][1]=0;
alldac[i][2]=0;
}

void (*dacfuncts[])()={initdac0,initdac1,initdac2,initdac3,initdac4};

#define MAXPAL sizeof(dacfuncts)/(sizeof ( void (*)()) )

void initdac(int d)
{
dacfuncts[palette](d);
}

void rotatedac()
{
int r,g,b;
int i;
r=alldac[0][0];
g=alldac[0][1];
b=alldac[0][2];
for (i=0;i<depth-1;i++)
	{
	alldac[i][0]=alldac[i+1][0];
	alldac[i][1]=alldac[i+1][1];
	alldac[i][2]=alldac[i+1][2];
	}
alldac[depth-1][0]=r;
alldac[depth-1][1]=g;
alldac[depth-1][2]=b;
}

void rotatedac2()
{
int r,g,b;
int i;
r=alldac[depth-1][0];
g=alldac[depth-1][1];
b=alldac[depth-1][2];
for (i=depth-2;i>=0;i--)
	{
	alldac[i+1][0]=alldac[i][0];
	alldac[i+1][1]=alldac[i][1];
	alldac[i+1][2]=alldac[i][2];
	}
alldac[0][0]=r;
alldac[0][1]=g;
alldac[0][2]=b;
}


void main()
{
int i,j;

tvga_init();
initmouse();
setmousearea(0,0,1023,767);
setmousexy(512,384);
initdata();
initdac(depth);
setalldac(alldac);
while(1)
	if (!divide(-512,511,-384,383))
//		waitkey();
;}


/*
int fastmand()
{
int count;
double ru,iu,rz,iz,rs,is;
ru=x/zoom-cmx;
iu=y/zoom-cmy;
rz=iz=rs=is=0;
count=-1;
while((count!=depth)&&(4.0>(is+rs)))
	{
	iz=2*rz*iz-iu;
	rz=rs-is-ru;
	rs=rz*rz;
	is=iz*iz;
	count++;
	}
return(count);
}
*/


void tvga_xordot(int x,int y,int sh)
{
if ((x<0)||(y<0)||(x>=1024)||(y>=768))
	return;
tvga_wtdot(x,y,tvga_rddot(x,y)+sh);
}

void dovline(int x0,int y0,int y1,int sh)
{
while(y0<=y1)
	{
	tvga_xordot(x0,y0,sh);
	y0++;
	}
}

void dohline(int x0,int y0,int x1,int sh)
{
while(x0<=x1)
	{
	tvga_xordot(x0,y0,sh);
	x0++;
	}
}

void dobox(int x0,int y0,int x1,int y1,int sh)
{
dovline(x0,y0,y1,sh);
dovline(x1,y0,y1,sh);
dohline(x0+1,y0,x1-1,sh);
dohline(x0+1,y1,x1-1,sh);
}

void cursor(int mx,int my,int sh)
{
int mzy=longmuldiv(768,mousezoom,1024);
dobox(mx-mousezoom,my-mzy,mx+mousezoom,my+mzy,sh);
}

int checkkey()
{
int but,mx,my;
int sh=0;
if (c=getkey())
	{
	if (c==K_ESC)
		{
		closegraph();
		exit();
		}
	if (c=='r')
		{
		cmx=0.0;
		cmy=0.0;
		zoom=200;
		initdata();
		return 1;
		}
	if (c=='m')
		{
		setmousexy(512,384);
		cursor(mx,my,sh);
		while(1)
			{
			c=getkey();
			waitvb();
			waitvb();
			cursor(mx,my,-sh);
			if (mousezoom<50)
				{
				if (c=='=')
					mousezoom+=4;
				else if (c=='-')
					mousezoom-=4;
				if (mousezoom<10)
					mousezoom=10;
				}
			else
				{
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
			if (but)
				{
				cmx+=(512-mx)/zoom;
				cmy+=(384-my)/zoom;
				zoom*=512.0/mousezoom;
				initdata();
				return 1;
				}
			}
		cursor(mx,my,-sh);
		}
	if (((c>='1')&&(c<='5'))||(c=='h')||(c=='l')||(c=='p')||(c=='.')||(c==','))
		{
		if ((c!='p')&&(c!='.')&&(c!=','))
			initdata();
		switch (c)
			{
			case '5':	zoom/=1.8;
							break;
			case '2':	cmx=cmx+200.0/zoom;
							cmy=cmy+200.0/zoom;
							zoom*=1.8;
							break;
			case '1':	cmx=cmx-200.0/zoom;
							cmy=cmy+200.0/zoom;
							zoom*=1.8;
							break;
			case '4':	cmx=cmx-200.0/zoom;
							cmy=cmy-200.0/zoom;
							zoom*=1.8;
							break;
			case '3':	cmx=cmx+200.0/zoom;
							cmy=cmy-200.0/zoom;
							zoom*=1.8;
							break;
			case 'h':	if (depth<240)
								depth+=16;
							initdac(depth);
							setalldac(alldac);
							break;
			case 'l':	if (depth>16)
								depth-=16;
							initdac(depth);
							setalldac(alldac);
							break;
			case 'p':	palette++;
							if (palette==MAXPAL)
								palette=0;
							initdac(depth);
							setalldac(alldac);
							return(0);
			case '.':	rotatedac();
							setalldac(alldac);
							return(0);
			case ',':	rotatedac2();
							setalldac(alldac);
							return(0);
		  	}
		return(1);
		}
	}
return(0);
}

int divide(xlo,xhi,ylo,yhi)
int xlo,xhi,ylo,yhi;
{
int meanx,meany,val,no;
if (checkkey())
	return(1);
y=ylo;
x=xlo;
no=0;
val=getmand();
for (x=xlo+1;x<xhi;x++)
	if (getmand()!=val)
		no=1;
y=yhi;
for (x=xlo+1;x<xhi;x++)
	if (getmand()!=val)
		no=1;
x=xlo;
for (y=ylo;y<=yhi;y++)
	if (getmand()!=val)
		no=1;
x=xhi;
for (y=ylo;y<=yhi;y++)
	if (getmand()!=val)
		no=1;
if (no==0)
	{
	tvga_rect(xlo+512,ylo+384,xhi+512,yhi+384,val);
//	for (x=xlo;x<=xhi;x++)
//		for (y=ylo;y<=yhi;y++)
//			tvga_wtdot(x+512,y+384,val);
	return(0);
	}

if ((xhi-xlo>=2)&&(yhi-ylo>=2))
	{
	meanx=(xlo+xhi)>>1;
	meany=(ylo+yhi)>>1;
	if (divide(meanx,xhi,ylo,meany))
		return(1);
	if (divide(xlo,meanx,ylo,meany))
		return(1);
	if (divide(xlo,meanx,meany,yhi))
		return(1);
	if (divide(meanx,xhi,meany,yhi))
		return(1);
	}
else
	for (x=xlo;x<=xhi;x++)
		for (y=ylo;y<=yhi;y++)
			getmand();
return(0);
}


int getmand()
{
int retval;
if ((retval=tvga_rddot(x+512,y+384))==255)
	tvga_wtdot(x+512,y+384,retval=fastmand());
return(retval);
}




/* odivide()
{
for (x=-512;x<512;x++)
	for (y=-384;y<384;y++)
		{
		if (checkkey())
			return(1);
		getmand();
		}
return(0);
}	*/

