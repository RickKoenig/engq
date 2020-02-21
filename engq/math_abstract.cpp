#include <m_eng.h>
#include "math_abstract.h"

/*S32 gcd(S32 a,S32 b)
{
	a=abs(a);
	b=abs(b);
	while(true) {
		if (a==0)
			return b;
		if (b==0)
			return a;
		if (a>b) {
			a%=b;
		} else {
			b%=a;
		}
	}
}

S32 lcm(S32 a,S32 b)
{
	if (a==0 && b==0)
		return 0;
	return (abs(a)/gcd(a,b)*abs(b)); // divides cleanly
}
*/
void fraction::reduce()
{
	if (d==0) {
//		errorexit("fraction:: 0 denominator");
		n=0;
		d=1;
		logger("fraction:: WARNING 0 denominator, setting to 0/1\n");
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

/*bool fraction::hasminv() const
{
	return n!=0;
}
*/
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

polynomial::polynomial(const fraction& a) // const
{
	co.push_back(a);
	reduce();
}

polynomial::polynomial(const fraction& a,const fraction& b) // linear
{
	co.push_back(b);
	co.push_back(a);
	reduce();
}

polynomial::polynomial(const fraction& a,const fraction& b,const fraction& c) // quadratic
{
	co.push_back(c);
	co.push_back(b);
	co.push_back(a);
	reduce();
}

polynomial::polynomial(const fraction& a,const fraction& b,const fraction& c,const fraction& d) // cubic
{
	co.push_back(d);
	co.push_back(c);
	co.push_back(b);
	co.push_back(a);
	reduce();
}

polynomial::polynomial(const fraction& a,const fraction& b,const fraction& c,const fraction& d,const fraction& e) // quartic
{
	co.push_back(e);
	co.push_back(d);
	co.push_back(c);
	co.push_back(b);
	co.push_back(a);
	reduce();
}

polynomial::polynomial(const fraction& a,const fraction& b,const fraction& c,const fraction& d,const fraction& e,const fraction& f) // quintic
{
	co.push_back(f);
	co.push_back(e);
	co.push_back(d);
	co.push_back(c);
	co.push_back(b);
	co.push_back(a);
	reduce();
}

void polynomial::reduce()
{
	S32 n=co.size();
	S32 i;
	for (i=n-1;i>=0;--i) {
		if (co[i].isnonzero())
			break;
	}
	if (i==n-1)
		return;
	co.resize(i+1,0);
}

/*void polynomial::expand(S32 newdegree)
{
	co.resize(newdegree+1,0);
}
*/

polynomial polynomial::operator+(const polynomial& rhs) const
{
	const polynomial *a,*b;
	if (co.size()>=rhs.co.size()) {
		a=this;
		b=&rhs;
	} else {
		a=&rhs;
		b=this;
	}
	polynomial r=*a;
	S32 i,n=b->co.size();
	for (i=0;i<n;++i)
		r.co[i]=co[i]+rhs.co[i];
	r.reduce();
	return r;
}

polynomial polynomial::operator-(const polynomial& rhs) const
{
	if (co.empty())
		return -rhs;
	const polynomial *a,*b;
	bool neg=false;
	if (co.size()>=rhs.co.size()) {
		a=this;
		b=&rhs;
	} else {
		a=&rhs;
		b=this;
		neg=true;
	}
	polynomial r=*a;
	S32 i,n=b->co.size();
	if (neg) {
		for (i=0;i<n;++i)
			r.co[i]=rhs.co[i]-co[i];
		r=-r;
	} else {
		for (i=0;i<n;++i)
			r.co[i]=co[i]-rhs.co[i];
	}
	r.reduce();
	return r;
}

polynomial polynomial::operator*(const polynomial& rhs) const
{
	S32 a=co.size();
	S32 b=rhs.co.size();
	polynomial r;
	if (a==0 || b==0)
		return r;
	r.co.resize(a+b-1,0);
	S32 i,j,ni=co.size(),nj=rhs.co.size();
	for (j=0;j<nj;++j)
		if (rhs.co[j].isnonzero())
			for (i=0;i<ni;++i)
				r.co[i+j]=r.co[i+j]+co[i]*rhs.co[j];
	r.reduce();
	return r;
}

polynomial polynomial::operator/(const polynomial& rhs) const
{
	polynomial rem;
	return divmod(rhs,rem);
}

polynomial polynomial::operator%(const polynomial& rhs) const
{
	polynomial rem;
	divmod(rhs,rem);
	return rem;
}

polynomial polynomial::divmod(const polynomial& rhs,polynomial& rem) const
{
	if (rhs.co.empty()) { // a/0 becomes 0r0 with warning/error
//		errorexit("polynomial:: 0 divisor");
		rem = polynomial();
		logger("polynomial:: WARNING 0 divisor, setting to q=0,r=0\n");
		return polynomial();
	}
	if (co.empty()) { // 0/a = 0r0
		rem = polynomial();
		return polynomial();
	}
	rem=*this;
	polynomial quot,quotp; // quote and monomial quote parts
	S32 d=max(0,S32(co.size()-rhs.co.size()+1));
	quot.co.resize(d,0); // accumulate
	quotp.co.resize(d,0); // just 1 nonzero term
	S32 wd=100; // watchdog
	while(true) {
		if (wd<=0) {
			rem = fraction(1);
			return polynomial();
		}
		S32 i=rem.co.size()-rhs.co.size();
		if (i<0)
			break;
		fraction q=rem.highest()/rhs.highest();
		quot.co[i]=q;
		quotp.co[i]=q;
		polynomial p=rhs*quotp;
		rem=rem-p;
		quotp.co[i]=0;
		--wd;
	}
	quot.reduce();
	return quot;
}

fraction polynomial::highest() const
{
	if (co.empty())
		return 0;
	return co[co.size()-1];
}


polynomial polynomial::operator-() const
{
	polynomial ret=*this;
	vector<fraction>::iterator it;
	for (it=ret.co.begin();it!=ret.co.end();++it)
		*it=-*it;
	return ret;
}

string polynomial::tostring() const
{
	S32 nt=co.size();
	if (!nt)
		return "ZERO";
	stringstream ss;
	S32 i;
	bool dodelim=false;
	for (i=0;i<nt;++i) {
		S32 jp=nt-1-i;
		const fraction& v=co[jp];
		S32 num=v.getnum();
		if (num) {
			fraction va=v.abs();
			if (dodelim) {
				if (num>0)
					ss << " + ";
				else
					ss << " - ";
			} else {
				if (num<0)
					ss << '-';
				dodelim=true;
			}
			if (va!=1 || jp==0)
				ss << va.tostring();
			if (jp>=2) {
				ss << "X^" << jp;
			} else if (jp==1) {
				ss << 'X';
			}
		}
	}
	return ss.str();
}

vector<fraction> polynomial::tovector() const
{
	return co;
}

fraction polynomial::eval(const fraction& x) const
{
	fraction r=0;
	S32 i,n=co.size();
	for (i=0;i<n;++i) {
		r=r*x+co[n-1-i];
	}
	return r;
}

bool polynomial::operator==(const polynomial& rhs) const
{
	return co==rhs.co;
}

bool polynomial::operator!=(const polynomial& rhs) const
{
	return co!=rhs.co;
}

// for now polys with higher degree are greater, 0 polys are the lowest by this convention
// reverse order for vector comparison
bool polynomial::operator>(const polynomial& rhs) const{
	if (co.size()!=rhs.co.size())
		return co.size()>rhs.co.size();
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	bool ret = co>rhs.co;
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	return ret;
}

bool polynomial::operator>=(const polynomial& rhs) const
{
	if (co.size()!=rhs.co.size())
		return co.size()>rhs.co.size();
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	bool ret = co>=rhs.co;
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	return ret;
}

bool polynomial::operator<(const polynomial& rhs) const
{
	if (co.size()!=rhs.co.size())
		return co.size()<rhs.co.size();
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	bool ret = co<rhs.co;
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	return ret;
}

bool polynomial::operator<=(const polynomial& rhs) const
{
	if (co.size()!=rhs.co.size())
		return co.size()<rhs.co.size();
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	bool ret = co<=rhs.co;
	reverse(co.begin(),co.end());
	reverse(rhs.co.begin(),rhs.co.end());
	return ret;
}

complexf complexf::operator/(complexf rhs) const {
	float rn=1.0f/(rhs.r*rhs.r+rhs.i*rhs.i);
	return complexf(rn*(r*rhs.r+i*rhs.i),rn*(i*rhs.r-r*rhs.i));
}

string complexf::tostring() const
{
	stringstream ss;
	ss << '(' << r << ',' << i << ')';
	return ss.str();
}

complexf::operator pointf2()
{
	pointf2 r;
	r.x = this->r;
	r.y = this->i;
	return r;
}

mat2c::mat2c(complexf aa)
{
	e[0][0]=aa;
	e[0][1]=0.0f;
	e[1][0]=0.0f;
	e[1][1]=aa;
}
mat2c::mat2c(complexf a,complexf b,complexf c,complexf d)
{
	e[0][0]=a;
	e[0][1]=b;
	e[1][0]=c;
	e[1][1]=d;
}
mat2c::mat2c(const mat2c& ca)
{
	e[0][0]=ca.e[0][0];
	e[0][1]=ca.e[0][1];
	e[1][0]=ca.e[1][0];
	e[1][1]=ca.e[1][1];
}
mat2c mat2c::operator+(const mat2c& rhs) const
{
	return mat2c(e[0][0]+rhs.e[0][0],e[0][1]+rhs.e[0][1],e[1][0]+rhs.e[1][0],e[1][1]+rhs.e[1][1]);
}
mat2c mat2c::operator-(const mat2c rhs) const
{
	return mat2c(e[0][0]-rhs.e[0][0],e[0][1]-rhs.e[0][1],e[1][0]-rhs.e[1][0],e[1][1]-rhs.e[1][1]);
}
mat2c mat2c::operator*(const mat2c rhs) const
{
	return mat2c(
		e[0][0]*rhs.e[0][0]+e[0][1]*rhs.e[1][0],
		e[0][0]*rhs.e[0][1]+e[0][1]*rhs.e[1][1],
		e[1][0]*rhs.e[0][0]+e[1][1]*rhs.e[1][0],
		e[1][0]*rhs.e[0][1]+e[1][1]*rhs.e[1][1]
	);
}
mat2c mat2c::operator-()
{
	return mat2c(-e[0][0],-e[0][1],-e[1][0],-e[1][1]);
}
string mat2c::tostring() const
{
	stringstream ss;
	ss << "[("
		<< e[0][0].tostring()
		<< ','
		<< e[0][1].tostring()
		<< ")("
		<< e[1][0].tostring()
		<< ','
		<< e[1][1].tostring()
		<< ")]";
	return ss.str();
}

complexf mat2c::det() const
{
	return e[0][0]*e[1][1]-e[0][1]*e[1][0];
}

mat2c mat2c::inv() const
{
	complexf idet=complexf(1.0f)/det();
	return mat2c(
		idet*e[1][1],
		-idet*e[0][1],
		-idet*e[1][0],
		idet*e[0][0]
	);
}

// field2 class

field2::field2(const C8* s) : v(0)
{

	while(*s) {
		if (*s == '0') {
			v <<= 1;
		} else if (*s == '1') {
			v <<= 1;
			++v;
		}
		++s;
	}
}

static string bin2str(U32 v)
{
	string s;
	if (v == 0) {
		s = "0";
	} else {
		while(v) {
			if (v&1)
				s += "1";
			else
				s += "0";
			v >>= 1;
		}
	}
	s = string(s.rbegin(),s.rend());
	return s;
}

static U32 field2mul(U32 a,U32 b) // binary multiply with no carry, max 32 bits
{
	U32 r=0,s=0;
	while(a) {
		if (a&1) {
			r ^= (b<<s);
		}
		++s;
		a>>=1;
	}
	return r;
}

static U32 field2div(U32 a,U32 b,U32* rem)
{
	U32 q = 0;
	if (b==0)
		return 0; // error
	if (a==0)
		return 0; // fine
	S32 la = ilog2(a);
	S32 lb = ilog2(b);
	while(la >= lb) {
			U32 sp = 1<<la;
		if (sp&a) {
			U32 s = b << (la-lb);
			a ^= s;
			q ^= 1 << (la-lb);
		}
		--la;
	}
	if (rem)
		*rem = a;
	return q;
}

void field2::setprim(const C8* s) // static
{
	prim = 0;
	primdeg = -1;
	while(*s) {
		if (*s == '0') {
			prim <<= 1;
			if (prim)
				++primdeg;
		} else if (*s == '1') {
			prim <<= 1;
			++prim;
			++primdeg;
		}
		++s;
	}
	// now build log and power tables
	gas = 1 << primdeg; // add
	gms = gas - 1; // mul
	U32 i;
	U32 ap = 2; // '10' or X
	bool good = false;
	bool first = true;
	base_lgfs.assign(gas,-1);
	base_pows.assign(gms,0);
	while(!good) {
		logger("trying prim w(1) = '%s'\n",bin2str(ap).c_str());
		lgfs.assign(gas,-1);
		pows.assign(gms,0);
		U32 a = 1;
		good = true;
		for (i=0;i<gms;++i) {
			if (lgfs[a] != -1) {
				logger("prim w(1) = '%s' no good\n",bin2str(ap).c_str());
				good = false;
				++ap;
				break;
			}
			pows[i] = a;
			lgfs[a] = i;
			a = field2mul(a,ap);
			U32 r = 0;
			/*U32 q = */field2div(a,prim,&r);
			a = r;
		}
//		if (true) {
		if (first || good) {
			logger("pow table for field2\n");
			for (i=0;i<gms;++i) {
				logger("w^%d = '%s'\n",i,bin2str(pows[i]).c_str());
			}
			logger("log table for field2\n");
			for (i=0;i<gas;++i) {
				logger("lf('%s') = '%d\n",bin2str(i).c_str(),lgfs[i]);
			}
			first = false;
		}
		if (!base_pows[0]) {
			base_lgfs = lgfs;
			base_pows = pows;
		}
	}
}

U32 field2::downgrade(U32 po) // downgrade prim power 'po' to base field, for cases like 11111
{
	if (po >= pows.size())
		errorexit("bad po");
	U32 rw = pows[po];
	if (rw >= base_lgfs.size())
		errorexit("bad rw");
	S32 bpo = base_lgfs[rw];
#if 0
	if (bpo < 0)
		logger("bad bpo");
#endif
	//logger("downgrade w from %u to %u\n",po,bpo);
	return bpo;
}

ostream& operator<<(ostream& stream,const field2& ob)
{
	string s = bin2str(ob.v);
	stream << "'" << s << "'" << " w(" << ob.f2p() << ")";
	return stream;
}

bool field2::operator!=(const field2& rhs) const
{
	return v != rhs.v;
}

field2 field2::operator-() const
{
	return *this; // negative of element of field 2^p is itself!
}

field2 field2::operator+(const field2& rhs) const
{
	return field2(v ^ rhs.v);
}

field2 field2::operator*(const field2& rhs) const
{
	if (!v || !rhs.v)
		return field2(0U);
	S32 e1 = lgfs[v];
	S32 e2 = lgfs[rhs.v];
	S32 es = e1 + e2;
	es %= gms;
	U32 nv = pows[es];
	return nv;
}

field2 field2::operator/(const field2& rhs) const
{
	if (!v || !rhs.v)
		return field2(0U);
	S32 e1 = lgfs[v];
	S32 e2 = lgfs[rhs.v];
	S32 es = e1 - e2;
	while(es<0)
		es += gms;
	es %= gms;
	U32 nv = pows[es];
	return nv;
}

field2 field2::operator+=(const field2& rhs)
{
	*this = *this + rhs;
	return *this;
}

field2 field2::operator*=(const field2& rhs)
{
	*this = *this * rhs;
	return *this;
}

field2 field2::p2f(S32 p)
{
	while(p<0)
		p += gms;
	p %= gms;
	U32 wp = pows[p];
	return wp;
}

S32 field2::f2p() const
{
	return lgfs[v];
}

U32 field2::prim;
U32 field2::primdeg;
U32 field2::gas;
U32 field2::gms;

vector<S32> field2::lgfs; // log of field element size = 2^n
vector<U32> field2::pows; // powers of w, size = 2^n - 1
vector<S32> field2::base_lgfs; // reduced, special, for things like X5+1 or 11111 non primitives
vector<U32> field2::base_pows; // reduced
