/* assume one point at 160,100 & wavelengh of 20 */

#include <math.h>
#include <graph32\graph32.h>

#define TWOPI 2*3.14159
#define wavelen 10
#define num 3

unsigned char dacs[256][3];
struct bitmap v;

void setpallete()
{
int i;
for (i=1;i<65;i++)
	{
	dacs[i][0]=64-i;
	dacs[i][1]=0;
	dacs[i][2]=0;
	}
for (i=65;i<128;i++)
	{
	dacs[i][0]=0;
	dacs[i][1]=(i-64)<<8;
	dacs[i][2]=0;
	}
dacs[1][0]=0x3f;
dacs[1][1]=0x3f;
dacs[1][2]=0x3f;
dacs[127][0]=0x3f;
dacs[127][1]=0x3f;
dacs[127][2]=0x3f;
setalldac(dacs);
}

void fwtdot(int x,int y,double i)
{
if (i<-num)
	i= -num;
else if (i>num)
	i=num;
clipputpixel(&v,x,y,64.5+63*i/num);
}

void doplot()
{
int x,y;
double d,i;
for (x=0;x<319;x++)
	{
	if (kbhit())
		break;
	for (y=0;y<199;y++)
		{
		d=hypot(x-150.0,y-100.0);
		i=sin(d*TWOPI/wavelen); 
		d=hypot(x-160.0,y-120.0);
		i+=sin(d*TWOPI/wavelen); 
		d=hypot(x-170.0,y-100.0);
		i+=sin(d*TWOPI/wavelen); 
		fwtdot(x,y,i);
		}
	}
}

void main()
{
initgraph();
make_video_bitmap(&v);
setpallete();
doplot();
waitkey();
closegraph();
}

