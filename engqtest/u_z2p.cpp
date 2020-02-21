#include <m_eng.h>
#include "u_z2p.h"

void z2p::setdeg()  // using 'val', set deg
{
	if (val == 0) {
		deg = -1;
		return;
	}
	deg = 0;
	// O(lg(n))	
	S32 sd = 16;
	while(sd > 0) {
		deg += sd;
		if (val < 1U<<deg) {
			deg -= sd;
		}
		sd >>= 1;
	}
}

z2p::z2p(const C8* s) // 1101 means x3 + x2 + 1
{
	val = 0;
	S32 cnt = 0;
	while(s[cnt] != '\0') {
		if (cnt>maxdeg)
			errorexit("z2p::z2p(const C8* s), overflow with '%s'",s);
		if (s[cnt] == '0') {
			val <<= 1;
		} else if (s[cnt] =='1') {
			val <<= 1;
			++val;
		}
		++cnt;
	}
	setdeg();
}

z2p::z2p()
{
	val = 0;
	deg = -1;
}

// lowest powers first
z2p::z2p(int a)
{
	if (a == 0) {
		val = 0;
		deg = -1;
		return;
	}
	val = a;
	setdeg();
}

z2p::z2p(int a,int b)
{
	val = a + (b<<1);
	setdeg();
}

z2p::z2p(int a,int b,int c)
{
	val = a + (b<<1) + (c<<2);
	setdeg();
}

z2p::z2p(int a,int b,int c,int d)
{
	val = a + (b<<1) + (c<<2) + (d<<3);
	setdeg();
}

z2p::z2p(int a,int b,int c,int d,int e)
{
	val = a + (b<<1) + (c<<2) + (d<<3) + (e<<4);
	setdeg();
}

// lowest powers first
z2p::z2p(const vector<int>& cfs)
{
	S32 i,n=cfs.size();
	if (n>maxdeg+1)
		errorexit("z2p::z2p(const vector<int>& cfs), overflow");
	val = 0;
	for (i=0;i<n;++i) {
		val += cfs[i]*(1<<i);
	}
	setdeg();
}

z2p z2p::operator+(const z2p& rhs) const
{
	z2p ret = val ^ rhs.val;
	ret.setdeg();
	return ret;
}

z2p z2p::operator-(const z2p& rhs) const
{
	return *this + rhs; // same as +
}

z2p z2p::operator*(const z2p& rhs) const
{
	if (deg + rhs.deg > maxdeg)
		errorexit("z2p z2p::operator*(const z2p& rhs) const, overflow");
	S32 i;
	z2p a;
	for (i=0;i<=deg;++i) {
		if (getcoef(i))
			a.val ^= (rhs.val << i);
	}
	a.setdeg();
	return a;
}

z2p z2p::operator*(int c) const
{
	if (c==0)
		return 0;
	else
		return *this;
}

z2p z2p::operator/(const z2p& rhs)
{
	z2p rem;
	z2p q = divmod(rhs,rem);
	return q;
}

z2p z2p::operator%(const z2p& rhs)
{
	z2p rem;
	z2p q = divmod(rhs,rem);
	return rem;
}

z2p z2p::divmod(const z2p& d,z2p& r) // return quotient
{
	if (d.val == 0)
		errorexit("z2p z2p::divmod(const z2p& d,z2p& r), divide by 0");
	if (val == 0) {
		r = 0;
		return 0;
	}
	z2p q;
	r = *this;
	while(r.deg >= d.deg) {
		z2p im;
		im.monomial(1,r.deg-d.deg);
		r = r - im*d;
		q = q + im;
	}
/* NYI
//	int rep = d.getrep();
//	d.monomial(0,-1);
//	d.setrep(rep,mod);
//	d.cleanup();
	if (d.val == 0) { // no can do, not a prime field or divide by 0
		return 0;
	}
	z2p q(mod);
//	q.setdeg(deg - d.deg,mod);
	r = *this;
	while(r.getdeg() >= d.getdeg()) {
		z2p qs(mod);
		S32 monc = (coefs[deg]*invm(d.coefs[d.deg],mod))%mod;
		S32 mond = r.getdeg() - d.getdeg();
		qs.monomial(monc,mond);
		z2p m = qs * d;
		r = r - m;
		q = q + qs;
	}
	return q;
	*/
	return q;
}

z2p z2p::pow(S32 pw) const
{
	if (val == 0 || val==1) //0^x == 0, 1^x == 1
		return *this;
	z2p a = 1;
	while(pw > 0) {
		a = a * *this;
//		logger("pow = '%s'\n",a.print().c_str());
		--pw;
	}
	return a;
}

z2p z2p::compose(const z2p& f) const
{
	z2p a;
	S32 i;
	S32 deg = getdeg();
	for (i=0;i<=deg;++i) {
		if (getcoef(i))
			a = a + f.pow(i);
	}
	return a;
}

string z2p::print2() const
{
	if (val == 0)
		return "0";
	string ret;	
	S32 i;
	for (i=deg;i>=0;--i)
		ret += '0' + ((val>>i)&1);
	return ret;
}

string z2p::print() const
{
	stringstream ss;
	ss << val;
	return ss.str();
}

void z2p::monomial(int c,int d) // cx^d
{
	if (d<0 || d>maxdeg)
		errorexit("void z2p::monomial(int c,int d), invalid args\n");
	val = c<<d;
	setdeg();
}

U32 z2p::getcoef(int pow) const
{
	if (pow<0)
		return 0;
	if (pow>maxdeg)
		return 0;
	return (val>>pow) & 1;
}

vector<z2p> z2p::factors() const
{
	vector<z2p> ret;
	z2p a = *this;
	if (deg == -1 || deg == 0) {
		ret.push_back(*this);
		return ret; // return 0 or 1
	}
	z2p d;
	int currep = getmod(); // start with X
	while(true) {
		d.setrep(currep);
		if (2*d.getdeg() > getdeg())
			break;
		z2p q,r;
		q = a.divmod(d,r);
		if (r.iszero()) {
			ret.push_back(d);
			a = q;
		} else {
			++currep;
		}
	}
	if (a.getdeg()>0)
		ret.push_back(a);
	return ret;
}

S32 z2p::dist0() const
{
	static const S32 nbits[16]={
		0,1,1,2,
		1,2,2,3,
		1,2,2,3, 
		2,3,3,4
	};
	U32 v = val;
	S32 ret = 0;
	S32 i;
	for (i=0;i<8;++i) {
		ret += nbits[v&0xf];
		v >>= 4;
	}
	return ret;
}

S32 z2p::dist(const z2p& rhs) const
{
	return (*this + rhs).dist0();
}
