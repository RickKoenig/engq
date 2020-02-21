#include <m_eng.h>

#include "ring2.h"

S32 zee2::defmod = 5; // mod

/*S32 zee2::invarr[maxmod+1][maxmod]={ // [mod][v]
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 0
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 1
	{-1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 2
	{-1, 1, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 3
	{-1, 1,-1, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 4
	{-1, 1, 3, 2, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 5
	{-1, 1,-1,-1,-1, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 6
	{-1, 1, 4, 5, 2, 3, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 7
	{-1, 1,-1, 3,-1, 5,-1, 7,-1,-1,-1,-1,-1,-1,-1,-1,}, // mod 8
	{-1, 1, 5,-1, 7, 2,-1, 4, 8,-1,-1,-1,-1,-1,-1,-1,}, // mod 9
	{-1, 1,-1, 7,-1,-1,-1, 3,-1, 9,-1,-1,-1,-1,-1,-1,}, // mod 10
	{-1, 1, 6, 4, 3, 9, 2, 8, 7, 5,10,-1,-1,-1,-1,-1,}, // mod 11
	{-1, 1,-1,-1,-1, 5,-1, 7,-1,-1,-1,11,-1,-1,-1,-1,}, // mod 12
	{-1, 1, 7, 9,10, 8,11, 2, 5, 3, 4, 6,12,-1,-1,-1,}, // mod 13
	{-1, 1,-1, 5,-1, 3,-1,-1,-1,11,-1, 9,-1,13,-1,-1,}, // mod 14
	{-1, 1, 8,-1, 4,-1,-1,13, 2,-1,-1,11,-1, 7,14,-1,}, // mod 15
	{-1, 1,-1,11,-1,13,-1, 7,-1, 9,-1, 3,-1, 5,-1,15,}, // mod 16
};
*/
S32 zee2::invarr2[maxmod+1][maxmod];

zee2::zee2(S32 va) : v(va),m(defmod)
{
	if (invarr2[0][0] == 0) { // init the invarr
		int i,jm,k;
		for (i=0;i<maxmod;++i) { // init zero mod to -1
			invarr2[0][i] = -1;
		}
		for (jm=1;jm<maxmod+1;++jm) { // current mod
			for (i=0;i<maxmod;++i) { // calc inverse of i given mod jm
				for (k=1;k<maxmod;++k) {
					S32 v = k * i % jm;
					if (v == 1) {
						invarr2[jm][i] = k;
						break;
					}
				}
				if (k == maxmod) {
					invarr2[jm][i] = -1;
				}
			}
		}
	}
	inva2 = invarr2[m];
	reduce();
}

void zee2::setdefmod(S32 ma)
{ 
	if (ma<2 || ma>maxmod)
		errorexit("bad setdefmod %d",ma);
	defmod = ma; 
}

void zee2::reduce()
{
	v %= m;
	if (v<0)
		v += m;
}

zee2 zee2::operator+(const zee2& rhs) const
{
	zee2 r(v+rhs.v);
	return r;
}

zee2 zee2::operator-(const zee2& rhs) const
{
	zee2 r(v-rhs.v);
	return r;
}

zee2 zee2::operator++()
{
	++v;
	reduce();
	return *this;
}

zee2 zee2::operator--()
{
	--v;
	reduce();
	return *this;
}

zee2 zee2::operator++(int)
{
	zee2 r = *this;
	++v;
	reduce();
	return r;
}

zee2 zee2::operator--(int)
{
	zee2 r = *this;
	--v;
	reduce();
	return r;
}

zee2 zee2::operator*(const zee2& rhs) const
{
	zee2 r(v*rhs.v);
	return r;
}

/*bool zee2::hasminv() const
{
	return gcd(m,v)==1;
}
*/

// multiplicative inverse
#if 1
zee2 zee2::minv() const
{
	S32 ret = inva2[v];
	if (ret==-1)
		gcon_printf("zee2(%d)::minv() WARNING: no inverse for %d\n",m,v);
	return ret;
}
bool zee2::hasminv() const
{
	return inva2[v] != -1;
}
#else
zee2 zee2::minv() const
{
	S32 i;
	for (i=1;i<m;++i) {
		zee2 r = (S32)*this * i;
		if (r.v == 1)
			return i;
	}
	gcon_printf("zee2(%d)::minv() WARNING: no inverse for %d\n",m,v);
	return -1;
}
#endif

zee2 zee2::operator/(const zee2& rhs) const
{
	S32 mi = rhs.minv();
	if (mi < 0) {
		gcon_printf("WARNING: zee2 divide by %d\n",rhs.v);
		zee2 r;
		return r;
	}
	zee2 r = mi * v;
	gcon_printf("%d/%d = %d\n",v,rhs.v,r.v);
	return r;
}

zee2 zee2::operator-() const
{
	return -v;
}

zee2 zee2::operator+=(const zee2& rhs)
{
	*this = *this + rhs;
	return *this;
}

zee2 zee2::operator-=(const zee2& rhs)
{
	*this = *this - rhs;
	return *this;
}

zee2 zee2::operator*=(const zee2& rhs)
{
	*this = *this * rhs;
	return *this;
}

zee2 zee2::operator/=(const zee2& rhs)
{
	*this = *this / rhs;
	return *this;
}


/* zee2 zee2::abs() const
{
	S32 va = ::abs(v);
	zee2 r(va);
	return r;
} */

string zee2::tostring() const
{
	stringstream ss;
	ss << v;
	return ss.str();
}

ostream& operator<<(ostream& stream,zee2 ob)
{
//	stream << ob.v << "mod " << ob.m;
	stream << ob.v;
	return stream;
}
