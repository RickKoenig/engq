#include <m_eng.h>

#include "fraction.h"

void fraction::reduce()
{
	if (d==0) {
//		errorexit("fraction:: 0 denominator");
		n=0;
		d=1;
		gcon_printf("fraction:: WARNING 0 denominator, setting to 0/1\n");
		return;
	}
	if (d<0) {
		n=-n;
		d=-d;
	}
	S32 r=gcd(n,d);
	n/=r;
	d/=r;
}

fraction fraction::operator+(const fraction& rhs) const
{
	S32 gcd=lcm(d,rhs.d);
	fraction r(gcd/d*n+gcd/rhs.d*rhs.n,gcd);
	return r;
}

fraction fraction::operator-(const fraction& rhs) const
{
	return *this + -rhs;
}

fraction fraction::operator++()
{
	n += d;
	return *this;
}

fraction fraction::operator--()
{
	n -= d;
	return *this;
}

fraction fraction::operator++(int)
{
	fraction r = *this;
	n += d;
	return r;
}

fraction fraction::operator--(int)
{
	fraction r = *this;
	n -= d;
	return r;
}

fraction fraction::operator*(const fraction& rhs) const
{
	fraction r(n*rhs.n,d*rhs.d);
	r.reduce();
	return r;
}

fraction fraction::operator/(const fraction& rhs) const
{
	fraction r(n*rhs.d,d*rhs.n);
	r.reduce();
	return r;
}

fraction fraction::operator-() const
{
	return fraction(-n,d);
}

fraction fraction::operator+=(const fraction& rhs)
{
	*this = *this + rhs;
	return *this;
}

fraction fraction::operator-=(const fraction& rhs)
{
	*this = *this - rhs;
	return *this;
}

fraction fraction::operator*=(const fraction& rhs)
{
	*this = *this * rhs;
	return *this;
}

fraction fraction::operator/=(const fraction& rhs)
{
	*this = *this / rhs;
	return *this;
}

bool fraction::hasminv() const
{
	return n!=0;
}

fraction fraction::abs() const
{
	fraction r=*this;
	if (r.n<0)
		r.n=-r.n;
	return r;
}

string fraction::tostring() const
{
	stringstream ss;
	if (d==1)
		ss << n;
	else
		ss << n << '/' << d;
	return ss.str();
}

ostream& operator<<(ostream& stream,fraction ob)
{
	if (ob.d == 1)
		stream << ob.n;
	else
		stream << ob.n << " / " << ob.d;
	return stream;
}
