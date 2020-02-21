#include <m_eng.h>
namespace goldbach {
	
const U32 nump=100;
const U32 rareadd=3;
vector<bool> plist;

void sieve()
{
	plist[0]=false;
	plist[1]=false;
	U32 maxdiv=isqrt(nump);
	logger("max div is %6u\n",maxdiv);
	U32 i,d;
	for (d=2;d<=maxdiv;++d) {
		if (!plist[d])
			continue;
		for (i=d+1;i<nump;++i) {
			if (i%d==0) {
				plist[i]=false;
			}
		}
	}
}

void dorun()
{
	U32 i,x,y;
	for (i=4;i<nump;i+=2) {
		U32 cnt=0;
		for (x=2;true;++x) {
			y=i-x;
			if (x>y)
				break;
			if (plist[x] && plist[y])
				++cnt;
		}
		if (cnt<=rareadd) {
			logger("found just %u: it's ",cnt);
			for (x=2;true;++x) {
				y=i-x;
				if (x>y)
					break;
				if (plist[x] && plist[y])
					logger("(%6u = %6u + %6u) ",i,x,y);
			}
			logger("\n");
		}
	}
}

void rungoldbach()
{
	logger("running goldbach with nump %7u\n",nump);
	plist.assign(nump,true);
	if (plist.size()!=nump)
		errorexit("nump doesn't jive");
	sieve();
/*	U32 i;
	for (i=0;i<nump;++i)
		if (plist[i])
			logger("found prime at %u\n",i);
	for (i=0;i<100;++i)
		logger("isqrt %d = %d\n",i,isqrt(i)); */
	dorun();
	plist.clear();
	logger("done running goldbach\n");
}
}
