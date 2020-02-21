#include <m_eng.h>

#include "lightuppatterns.h"
#include "lightuppatterndata.h"

using namespace lightuppatterns;

void getpattern(S32 midx,S32 midy,S32 dimx,S32 dimy, S32 pat, S32 wrap, U8 data[]) // sizeof data is dimx*dimy
{
	S32 bi,bj,pi,pj;
	const S32 patdimx = maxwidth*2-1;
	const S32 patdimy = maxheight*2-1;
	const U8 ((*patdat)[patdimx]) = patterndata[pat]; // get current pattern
	::fill(data,data+dimx*dimy,0);
	for (pj=0;pj<patdimy;++pj) {
		for (pi=0;pi<patdimx;++pi) {
			U8 p = patdat[pj][pi];
			if (p==0)
				continue;
			bi = midx - (maxwidth - 1) + pi;
			bj = midy - (maxheight - 1) + pj;
			if (wrap) {
				if (bi < 0)
					bi += dimx;
				else if (bi >= dimx)
					bi -= dimx;
				if (bj < 0)
					bj += dimy;
				else if (bj >= dimy)
					bj -= dimy;
			}
			if (bi>=0 && bi<dimx && bj>=0 && bj<dimy) {
				data[bj*dimx+bi] = 1; 
			}
		}
	}
}
