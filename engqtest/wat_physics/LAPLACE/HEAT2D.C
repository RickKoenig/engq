#include <graph32\graph32.h>

struct bitmap v,b;

int array[100][100];
int array2[100][100];

struct point around[4]={0,1,1,0,0,-1,-1,0};

void intrect(int x0,int y0,int x1,int y1,int val)
{
int i,j;
val<<=8;
for (i=x0;i<=x1;i++)
	for (j=y0;j<=y1;j++)
		array[i][j]=val;
}

void main()
{
int i,j,k;
int x,y;
int sum;
int val;
mem_init();
alloc_bitmap(&b,100,100,0);
make_video_bitmap(&v);
initgraph();
while(!getkey())
	{
	for (i=0;i<100;i++)	
		for (j=0;j<100;j++)	
		  	{
		  	sum=0.0;
		  	for (k=0;k<4;k++)
				sum+=array2[i+around[k].x][j+around[k].y];
			array[i][j]=sum>>2;
			}
	intrect(50,50,80,80,10);
	intrect(20,20,40,40,20);
	intrect(90,40,95,44,25);
	intrect( 0, 0,99, 0,0);
	intrect( 0,99,99,99,0);
	intrect( 0, 0, 0,99,0);
	intrect(99, 0,99,99,0);
	for (i=0;i<100;i++)
		for (j=0;j<100;j++)
			array2[i][j]=array[i][j];
	for (i=0;i<100;i++)
		for (j=0;j<100;j++)
			clipputpixel(&b,i,j,array2[i][j]>>8);
	clipblit(&b,&v,0,0,0,0,100,100);
	}
closegraph();
}


