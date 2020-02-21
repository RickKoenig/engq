#include <m_eng.h>
#include <math_abstract.h>

static const S32 winscore = 15;
static bool hasscorearr[winscore][winscore];
static fraction scorearr[winscore][winscore];

static fraction winprob(S32 h,S32 v)
{
	if (h == v)
		return fraction(1,2);
	if (h>=winscore || v>=winscore) {
		if (h >= v+2)
			return 1;
		if (h+2 <= v)
			return 0;
		if (h == v+1)
			return fraction(3,4);
		if (h+1 == v)
			return fraction(1,4);
	}
	if (hasscorearr[h][v])
		return scorearr[h][v];
	hasscorearr[h][v] = true;
	fraction half(1,2);
	fraction ret = half*winprob(h+1,v)+half*winprob(h,v+1);
	scorearr[h][v] = ret;
	return ret;
}

void scoretest()
{
	S32 i,j;
	for (j=0;j<winscore;++j)
		for (i=0;i<winscore;++i)
			hasscorearr[j][i]=false;
	for (j=0;j<=18;++j) {
		for (i=0;i<=18;++i) {
			fraction f = winprob(i,j);
			stringstream s;
			s << f;
			logger("at h %2d, v %2d, fraction is %9.5f %s\n",i,j,(float)f.getnum()/f.getden(),s.str().c_str());
		}
	}
}
