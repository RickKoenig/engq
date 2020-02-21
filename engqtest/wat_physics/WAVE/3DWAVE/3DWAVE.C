#include <stdlib.h>
#include <graph\graph.h>

#define SIZE 80
int my0[SIZE][SIZE],my1[SIZE][SIZE];

#define centx 15
#define centy 40
void initwave()
{
int i,j,iup,jup;
for (i=1;i<SIZE-1;i++)
	for (j=1;j<SIZE-1;j++)
		{
		iup=abs(i-(j>>2)-centx);
		jup=abs(j+(i>>2)-centy);
		if (jup>30)
			jup=0;
		else if (jup<15)
			jup=450;
		else
			jup=30*(30-jup);
		/*		iup=max(iup,jup); */
		if (iup<10)
			my1[i][j]=(10-iup)*jup;
		}
/* for (i=1;i<SIZE-1;i++)
	for (j=1;j<SIZE-1;j++)
		my0[i][j]=(my1[i+1][j]+my1[i-1][j]+my1[i][j+1]+my1[i][j-1])/4; */
for (i=1;i<SIZE-1;i++)
	for (j=1;j<SIZE-1;j++)
		my0[i][j]=my1[i][j];
}


void initwave2()
{
int i,j,iup,jup;
my1[20][60]=20000;
my1[19][61]=10000;
my1[20][61]=10000;
my1[19][60]=10000;
my1[20][60]=10000;
for (i=1;i<SIZE-1;i++)
	for (j=1;j<SIZE-1;j++)
		my0[i][j]=my1[i][j];
}

void main()
{
int (*yp0)[SIZE],(*yp1)[SIZE],(*temp)[SIZE];
int total,i,j;
int color;
int dotval;
struct bitmap b,v;
mem_init();
alloc_bitmap(&b,XSIZE,YSIZE,0,0);
make_video_bitmap(&v);
initgraph();
initwave();
yp0=my0;
yp1=my1;
while( !getkey())
	{
	fastrect(&b,0,0,XSIZE-1,YSIZE-1,black);
	for (i=1;i<SIZE-1;i++)
		for (j=1;j<SIZE-1;j++)
			{
			dotval=yp1[i][j]>>6; 
			color=0xf&((dotval>>2)+10);
			fastputpixel(&b,(j<<1)+(i<<1),j-i+100-dotval,color);  
  yp0[i][j]=((yp1[i+1][j]+yp1[i-1][j]+yp1[i][j+1]+yp1[i][j-1])>>1)-yp0[i][j];
			}
	temp=yp0;
	yp0=yp1;
	yp1=temp;
	fastblit(&b,&v,0,0,0,0,XSIZE,YSIZE);
	}
closegraph();
free_bitmap(&b);
}


