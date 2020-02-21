#include <math.h>
#include <stdio.h>

double v;
double m=10;
double c=186000;
double ke,ke2;

void main()
{
for (v=1000;v<186000;v+=10)
	{
	ke=m*v*v/2;
	ke2=m*c*c/sqrt(1-v*v/c/c)-m*c*c;
	printf("v = %10.3f, ke = %16.3f, ke2 = %17.3f, rat = %8.5f\n",v,ke,ke2,ke2/ke);
	}
}


