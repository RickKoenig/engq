#include <stdio.h>
#include <math.h>
#include "quant.h"

void fastquan()
{
short t,x,n;
long ampr,ampi,xp;
for (t=0;t<TIME;t++)
	{
	for (x=0;x<SPACE;x++)
		{
		ampr=ampi=0;
		for (n=1;n<ENERGIES;n++)
			if (ak[n])
				{
				xp=angsx[x][n];
				ampr+=xp*angsreal[t][n];
				ampi+=xp*angsimag[t][n];
				}
		ampr=(ampr/sumk)>>11;
		ampi=(ampi/sumk)>>11;
		reals[x][t]=ampr>>1;
		imags[x][t]=ampi>>1;
		probs[x][t]=(ampr*ampr+ampi*ampi)>>9;
		}
	if (!(t&0xf))
		printf("t = %d\n",t);
	}
}

