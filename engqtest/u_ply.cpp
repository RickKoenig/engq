
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_s_ifield.h"
#include "u_z2p.h"
#include "u_states.h"

#include "ring2.h"
#include "augmatz.h"

#include "u_ply.h"

S32 ply::mod = 2; // initial value of ply::mod, must be prime <= maxmod
bool ply::packprint = true;
bool ply::berlfact = true;

bool isprime(int n)
{
	if (n<=1)
		return false;
	if (n>maxmod)
		return false; // handle up to maxmod
	int d;
	for (d=2;d<n;++d) {
		int q = n/d;
		if (q<d)
			return true;
		int r = n%d;
		if (!r)
			return false;
	}
	return true;
}

S32 cleanmod(S32 n,S32 mod)
{
	if (n == 0)
		return 0;
	if (n<0) {
		n = -n;
		n %= mod;
		if (n==0)
			return 0;
		return mod - n;
	}
	return n%mod;
}


#if 0
S32 invp(S32 n,S32 mod) // additive inverse
{
	if (n == 0)
		return 0;
	if (n<0) {
		n = -n;
		return n % mod;
	}
	n %= mod;
	if (n==0)
		return 0;
	return mod - n;
}
#endif
S32 invm(S32 n,S32 mod) // multiplicative inverse
{
	static bool isinited[maxmod+1];
	static S32 invs[maxmod+1][maxmod+1];
	if (!isprime(mod)) // bad mod, mod must be prime
		return 0;
	if (mod > maxmod)
		return 0;
	n = cleanmod(n,mod);
	if (n == 0)
		return 0;
	if (!isinited[mod]) {
		S32 i,j;
		for (i=0;i<mod;++i) {
			for (j=0;j<mod;++j) {
				if (i*j%mod == 1) {
					invs[mod][i] = j;
				}
			}
		}
	}
	return invs[mod][n];
}

bool ply::isprim(S32 dega) const // assumes poly is irreducible
{
	S32 i;
	S32 nd;
	if (dega == 0)
		nd = ipow(getmod(),getdeg()) - 1;
	else
		nd = dega;
	getdeg();
	for (i=1;i<nd;++i) {
		ply b;
		b.monomial(1,i);
		b.inc();
		ply r = b % *this;
		if (r.iszero()) {
			//logger("%d != %d ",i,nd);
			break;
		}
	}
	return i == nd;
}

bool ply::operator==(const ply& rhs) const
{
	if (deg != rhs.deg)
		return false;
	S32 i;
	for (i=0;i<=deg;++i)
		if (coefs[i] != rhs.coefs[i])
			return false;
	return true;
}

bool ply::operator!=(const ply& rhs) const
{
	if (deg != rhs.deg)
		return true;
	S32 i;
	for (i=0;i<=deg;++i)
		if (coefs[i] != rhs.coefs[i])
			return true;
	return false;
}

void ply::setmod(S32 newmod)
{
	if (isprime(newmod))
		mod = newmod;
	else
		mod = 2;
}

void ply::inc()
{
	if (deg == -1) {
		this->monomial(1,0); // 0 to 1
		return;
	}
	S32 i;
	for (i=0;i<=deg;++i) {
		++coefs[i];
		if (coefs[i] != mod)
			return;
		coefs[i] = 0;
	}
	if (i == deg + 1) {
		ply p;
		p.monomial(1,deg+1);
		*this = *this + p;
	}
}

// play with Berlekamp's algorithm, works with squarefree monic polys of degree > 0
// returns '1' if no non trivial factors to be found
ply berl(const ply& f)
{
		S32 i,j;
//		S32 oldmod = ply::getmod();
//		bool oldpp = ply::getpackprint();
//		ply::setpackprint(true);
//		const S32 TESTMOD = 2;
//		const S32 TESTMOD = 7;
//		ply::setmod(TESTMOD);
//		ply f("x31+1"); // mod 2
//		ply f("x5+x4+1"); // mod 2
//		ply f("x5+2x3++x2+x+1"); // mod 7
		S32 d = f.getdeg();
		const S32 TESTMOD = f.getmod();
		augmatz am(TESTMOD,d,d);
		U8* cd = new U8[d*(d+1)];
		ply ret(1);
		::fill(cd,cd+d*(d+1),0); // init to 0
		for (j=0;j<d;++j) { // powers, columns
			//logger("X^%3d = ",j*TESTMOD);
			ply x(1,0); // X
			ply px = x.pow(j*TESTMOD); // X^TESTMOD
			px = px % f;
			//logger("'%s'\n",px.print().c_str());
			for (i=0;i<d;++i) // rows
				//if (j==0 || j==1 || j==2)
					cd[(d+1)*i+j]=px.getcoef(i);
		}
		for (j=0;j<d;++j) { // subract identity, make px^TESTMOD - px mod f
			cd[(d+1)*j+j]=(cd[(d+1)*j+j]+TESTMOD-1)%TESTMOD;
		}

		am.load(cd);
		delete[] cd;
		am.print();
		vector<vector<zee2> > a = am.getvarsc();
		S32 n = a.size();
		S32 m=0;
		if (n)
			m = a[0].size();
		//logger("varsc\n");
		if (n<=2)
			return ret; // no suitable basis vectors
/*		for (j=0;j<n;++j) {
				logger("var %3d: ",j);
			for (i=0;i<m;++i) {
				logger("%d ",a[j][i].v);
			}
			logger("\n");
		}
		logger("end varsc\n"); */
// find a suitable h and then all h+c
		ply h,g,hf;
		for (j=1;j<n;++j) {
			m = a[0].size();
			vector<S32> vc;
			for (i=0;i<m;++i)
				vc.push_back(a[j][i]);
			h = ply(vc);
			//logger("h = '%s',  ",h.print().c_str());
			g = f.gcd(h);
			//logger("g = '%s'\n",g.print().c_str());

//			hs = hs + h;
			if (h.getdeg()>0) { // found a suitable h
				if (hf.iszero())
					hf = h;
				//break;
			}
		}
		//logger("hf = '%s'\n",hf.print().c_str());
		// now find a suitable g using hf + c
		S32 gm = ply::getmod();
		for (i=0;i<gm;++i) {
			hf.setcoef(0,i);
			//logger("hf[%d] = '%s',  ",i,hf.print().c_str());
			g = f.gcd(hf);
			//logger("gf[%d] = '%s'\n",i,g.print().c_str());
			if (ret.getdeg()==0 && g.getdeg()>0) {
				ret = g;
				//logger("found a good gf '%s'\n",g.print().c_str());
				break;
			}
		}
//		ply::setmod(oldmod);
//		ply::setpackprint(oldpp);
		if (ret == f)
			errorexit("found a ret == f, '%s'",ret.print().c_str());
		return ret;
}

bool ply::operator<(const ply& rhs) const // return true if this < rhs
{
	if (getdeg() < rhs.getdeg()) {
		//logger("return true deg '%s' < '%s'\n",print().c_str(),rhs.print().c_str());
		return true;
	}
	else if (getdeg() > rhs.getdeg()) {
		//logger("return false deg '%s' < '%s'\n",print().c_str(),rhs.print().c_str());
		return false;
	}
	S32 i,n=getdeg();
	//logger("deg = %d\n",n);
	for (i=n;i>=0;--i) {
		if (getcoef(i) < rhs.getcoef(i)) {
			//logger("return true coef '%s' < '%s'\n",print().c_str(),rhs.print().c_str());
			//logger("deg = %d, %d rhs = %d\n",i,getcoef(i),rhs.getcoef(i));
			return true;
		} else if (getcoef(i) > rhs.getcoef(i)) {
			//logger("return false coef '%s' > '%s'\n",print().c_str(),rhs.print().c_str());
			//logger("deg = %d, %d rhs = %d\n",i,getcoef(i),rhs.getcoef(i));
			return false;
		}
	}
	//logger("return false left over '%s' < '%s'\n",print().c_str(),rhs.print().c_str());
	return false;
}

// try to implement Berlekamp's algorithm
S32 factorlevel;
vector<ply> ply::factors() const
{
//	logger("\n\ncalc factors for '%s' mod %d\n",print().c_str(),mod);
	if (factorlevel > 300)
		errorexit("bad factorlevel");
	vector<ply> ret;
	ply a = *this;
	if (deg <= 0) {
		ret.push_back(*this);
		return ret; // return early with 0 or 1
	}
	int hicoef = a.getcoef(deg);
	ply d;
	if (hicoef != 1) { // make 'a' monic (highest degree coef equal to 1)
		d.monomial(hicoef,0);
		ret.push_back(d); // constant
		a = a/d; // remainder is always 0 for a field
	}
	//int currep = getmod(); // start with X
	// first do a squarefree test
	ply der = a.deriv();
	if (der.iszero()) { // perfect square, cube etc. depends on mod, calc square/cube etc root recurse and return early
		ply bld;
		S32 i,dg = a.getdeg();
		for (i=0;i<=dg;++i) {
			if (a.coefs[i]) {
				if (i%mod)
					errorexit("shouldn't happen in factors");
				ply f;
				f.monomial(a.coefs[i],i/mod);
				bld = bld + f;
			}
		}
		++factorlevel;
		vector<ply> root = bld.factors();
		--factorlevel;
//		ret.push_back(ply(1,0,1,0,1,0));
		for (i=0;i<mod;++i) // double the factors for perfect square
			ret.insert(ret.end(),root.begin(),root.end());
		if (factorlevel==0)
			sort(ret.begin(),ret.end());
		return ret;
	} else {
		ply g = a.gcd(der);
		if (g.getdeg()) { // non squarefree, split into f/g and g
			ply fdg = a/g;
			++factorlevel;
			vector<ply> gf = g.factors();
			vector<ply> fdgf = fdg.factors();
			--factorlevel;
//			ret.push_back(ply(1,1,1,1,1,1));
			ret.insert(ret.end(),gf.begin(),gf.end());
//			ret.push_back(ply(1,1,1,1,1,0));
			ret.insert(ret.end(),fdgf.begin(),fdgf.end());
			if (factorlevel==0)
				sort(ret.begin(),ret.end());
			return ret;
		}
	}
	// end squarefree test
	// now have a squarefree poly
	if (berlfact) { // Berlekamp's algorithm
		ply g = berl(a);
		if (g.getdeg() < 1) {
			ret.push_back(a);
			sort(ret.begin(),ret.end());
			return ret;
		} else {
			ply fdg = a/g;
			++factorlevel;
			vector<ply> gf = g.factors();
			vector<ply> fdgf = fdg.factors();
			--factorlevel;
	//		ret.push_back(ply(1,0,0,0,1,1));
			ret.insert(ret.end(),gf.begin(),gf.end());
	//		ret.push_back(ply(1,0,0,0,1,0));
			ret.insert(ret.end(),fdgf.begin(),fdgf.end());
			if (factorlevel==0)
				sort(ret.begin(),ret.end());
			return ret;
		}
	} else { // brute force
		d.monomial(1,1); // start with X
		while(true) { // brute force
			//d.setrep(currep);
			if (2*d.getdeg() > getdeg()) // sqrt of original
				break;
			ply q,r;
			q = a.divmod(d,r);
			if (r.iszero()) {
				ret.push_back(d); // found a factor
				a = q;
			} else {
				//++currep;
				d.inc(); // next divisor
			}
		}
		if (a.getdeg()>0)
			ret.push_back(a);
		if (factorlevel==0)
			sort(ret.begin(),ret.end());
		return ret;
	}
}

ply ply::deriv() const
{
	ply ret;
	if (deg<=0)
		return ret;
	S32 i;
	for (i=1;i<=deg;++i) {
		ply m;
		m.monomial(i*coefs[i],i-1);
		ret = ret + m;
	}
	return ret;
}

ply ply::pow(S32 pw) const
{
	ply a(1);
	while(pw > 0) {
		a = a * *this;
		--pw;
	}
	return a;
}

ply ply::compose(const ply& f) const
{
	ply a;
	S32 i;
	for (i=0;i<=deg;++i) {
		a = a + f.pow(i) * coefs[i];
	}
	return a;
}

bool ply::iszero() const
{
	return deg == -1;
}

int ply::getcoef(int pow) const
{
	if (pow<0)
		return 0;
	if (pow>deg)
		return 0;
	return coefs[pow];
}

void ply::setcoef(int pow,int val)
{
	if (pow<0)
		return;
	if (pow>=deg)
		return; // can't change highest power of poly
	coefs[pow] = val;
}

int ply::getrep() const // 1 number to describe the poly
{
	int i,r=0;
	for (i=deg;i>=0;--i) {
		r *= mod;
		r += coefs[i];
	}
	return r;
}

void ply::setrep(int r)
{
	if (!isprime(mod)) {
		zero();
		return;
	}
	vector<S32> cfs;
	while(true) {
		if (!r)
			break;
		cfs.push_back(r % mod);
		r /= mod;
	}
	*this = ply(cfs);
	cleanup();
}

void ply::monomial(int c,int d)
{
	coefs.assign(d+1,0);
	coefs[d] = c;
	deg = d;
	cleanup();
}

#if 0
void ply::setdeg(int d,int m) // assume mod already set, no cleanup
{
	deg = d;
	mod = m;
	coefs.assign(deg + 1,0);
}
#endif
void ply::zero()
{
	deg = -1;
	coefs.clear();
}

void ply::cleanup() // remove highest 0's from poly, clean mods
{
	if (!isprime(mod)) {
		zero();
		return;
	}
	int i;
	for (i=0;i<=deg;++i)
		coefs[i] = cleanmod(coefs[i],mod);
	while(deg >= 0) {
		if (coefs[deg] != 0)
			return;
		coefs.pop_back();
		--deg;
	}
}

ply::ply() : deg(-1)
{
}

// lowest powers first
ply::ply(int a) : deg(0)
{
	coefs.push_back(a);
	cleanup();
}


ply::ply(int a,int b) : deg(1)
{
	coefs.push_back(b);
	coefs.push_back(a);
	cleanup();
}

ply::ply(int a,int b,int c) : deg(2)
{
	coefs.push_back(c);
	coefs.push_back(b);
	coefs.push_back(a);
	cleanup();
}

ply::ply(int a,int b,int c,int d) : deg(3)
{
	coefs.push_back(d);
	coefs.push_back(c);
	coefs.push_back(b);
	coefs.push_back(a);
	cleanup();
}

ply::ply(int a,int b,int c,int d,int e) : deg(4)
{
	coefs.push_back(e);
	coefs.push_back(d);
	coefs.push_back(c);
	coefs.push_back(b);
	coefs.push_back(a);
	cleanup();
}

ply::ply(int a,int b,int c,int d,int e,int f) : deg(5)
{
	coefs.push_back(f);
	coefs.push_back(e);
	coefs.push_back(d);
	coefs.push_back(c);
	coefs.push_back(b);
	coefs.push_back(a);
	cleanup();
}

// lowest powers first
ply::ply(const vector<int>& cfs)
{
	coefs = cfs;
	deg = coefs.size() - 1;
	cleanup();
}

ply::ply(const C8* s,S32 newmoda) : deg(-1) // 1,2,3M5 means 1x^2 + 2x + 3 mod 5 // new 123 means x^2+2x+3
{
	if (newmoda)
		setmod(newmoda);
	S32 len = strlen(s);
	if (len<1) {
		return;
	}
	S32 i;
	for (i=0;i<len;++i)
		if (s[i] == 'x' || s[i]=='X' || s[i]=='+')
			break;
	if (i!=len) { // parse with special 'X' mode all in decimal
		C8* news = new C8[len+1];
		C8* ptr = news;
		C8* tok;
		copy(s,s+len+1,news);
		while(true) {
			tok = strtok(ptr,"+");
			if (!tok)
				break;
			ptr = 0;
			//logger("tok = '%s'\n",tok);
			S32 toklen = strlen(tok);
			C8* newtok = new C8[toklen+1];
			copy(tok,tok+toklen+1,newtok);
			for (i=0;i<toklen;++i) {
				if (newtok[i] == 'x' || newtok[i] == 'X') {
					break;
				}
			}
			if (i!=toklen) { // 4 cases with X
				//logger("X found in '%s'\n",newtok); 
				if (i == 0) {
					if (i == toklen-1) {
						//logger("lone X\n");
						ply mon;
						mon.monomial(1,1);
						//logger("mon = '%s'\n",mon.print().c_str());
						*this = *this + mon;
					} else {
						//logger("x..\n");
						ply mon;
						mon.monomial(1,atoi(newtok+1));
						//logger("mon = '%s'\n",mon.print().c_str());
						*this = *this + mon;
					}
				} else {
					if (i == toklen-1) {
						//logger("..x\n");
						newtok[toklen-1] = '\0'; // remove the 'x'
						ply mon;
						mon.monomial(atoi(newtok),1);
						//logger("mon = '%s'\n",mon.print().c_str());
						*this = *this + mon;
					} else {
						//logger("..x..\n");
						C8* newt1 = new C8[i+1];
						copy(newtok,newtok+i,newt1);
						newt1[i] = '\0';
						C8* t2 = newtok + i + 1;
						//logger("t1 = '%s', t2 = '%s'\n",newt1,t2);
						ply mon;
						mon.monomial(atoi(newt1),atoi(t2));
						//logger("mon = '%s'\n",mon.print().c_str());
						*this = *this + mon;
						delete[] newt1;
					}
				}
			} else { // no X, 1 case, just a constant, degree == 0
				//logger("X not found in '%s'\n",newtok);
				ply mon;
				mon.monomial(atoi(newtok),0);
				//logger("mon = '%s'\n",mon.print().c_str());
				*this = *this + mon;
			}
			delete[] newtok;
		}
		delete[] news;
		return;
	}
	deg = len - 1;
	for (i=deg;i>=0;--i) {
		C8 c = s[i];
		c = tolower(c);
		S32 cf = -1;
		if (c >= '0' && c<= '9')
			cf = c - '0';
		else if (c >= 'a' && c<= 'z')
			cf = c - 'a' + 10;
		if (cf == -1 || cf >= mod) {
			zero();
			return;
		}
		coefs.push_back(cf);
	}
	cleanup();
}

ply ply::operator+(const ply& rhs) const
{
	ply ret;
	int i;
	int maxdeg = max(deg,rhs.deg);
	ret.deg = maxdeg;
	for (i=0;i<=maxdeg;++i) {
		int v = 0;
		if (i<=deg)
			v += coefs[i];
		if (i<=rhs.deg)
			v += rhs.coefs[i];
		ret.coefs.push_back(v);
	}
	ret.cleanup();
	return ret;
}

ply ply::operator-(const ply& rhs) const
{
	ply ret;
	int i;
	int maxdeg = max(deg,rhs.deg);
	ret.deg = maxdeg;
	for (i=0;i<=maxdeg;++i) {
		int v = 0;
		if (i<=deg)
			v += coefs[i];
		if (i<=rhs.deg)
			v -= rhs.coefs[i];
		ret.coefs.push_back(v);
	}
	ret.cleanup();
	return ret;
}

ply ply::operator*(const ply& rhs) const
{
	ply ret;
	if (iszero() || rhs.iszero())
		return ret;
	ret.deg = deg + rhs.deg;
	ret.coefs.assign(ret.deg + 1,0);
	int i,j;
	for (j=0;j<=deg;++j) {
		for (i=0;i<=rhs.deg;++i) {
			ret.coefs[i+j] += coefs[j] * rhs.coefs[i];
		}
	}
	ret.cleanup();
	return ret;
}

ply ply::operator*(int c) const
{
	ply ret = *this;
	int i;
	for (i=0;i<=ret.deg;++i)
		ret.coefs[i] *= c;
	ret.cleanup();
	return ret;
}

ply ply::divmod(const ply& d,ply& r) const // return quotient
{
//	int rep = d.getrep();
//	d.monomial(0,-1);
//	d.setrep(rep,mod);
//	d.cleanup();
	if (d.iszero()) { // no can do, divide by 0
		r = ply(); // Q and R are both 0, an error
		return r;
	}
	ply q;
//	q.setdeg(deg - d.deg,mod);
	r = *this;
	while(r.getdeg() >= d.getdeg()) {
		ply qs;
		S32 monc = (coefs[deg]*invm(d.coefs[d.deg],mod))%mod;
		S32 mond = r.getdeg() - d.getdeg();
		qs.monomial(monc,mond);
		ply m = qs * d;
		r = r - m;
		q = q + qs;
	}
	return q;
}

ply ply::operator/(const ply& rhs) const
{
	ply rem;
	ply q = divmod(rhs,rem);
	return q;
}

ply ply::operator%(const ply& rhs) const
{
	ply rem;
	ply q = divmod(rhs,rem);
	return rem;
}

ply ply::gcd(const ply& rhs) const
{
	ply a = *this;
	ply b = rhs;
	while(true) {
		if (a.getdeg()<b.getdeg()) {
			ply t = a;
			a = b;
			b = t;
		}
		if (b.iszero())
			break;
		// a is now >= b
		a = a % b;

	}
	if (a.iszero())
		return a;
	int deg = a.getdeg();
	int hicoef = a.getcoef(deg);
	ply d;
	if (hicoef != 1) { // make 'a' monic (highest degree coef equal to 1)
		d.monomial(hicoef,0);
		a = a/d; // remainder is always 0 for a field
	}
	return a;
}

string ply::print() const
{
	if (deg == -1)
		return "Z";
	stringstream ss;
	S32 i;
	string delim = "";
	for (i=deg;i>=0;--i) {
		S32 v = coefs[i];
		if (packprint) {
			C8 c;
			if (v < 10)
				c = v + '0';
			else
				c = v + 'A' - 0xa;
			ss << c;
		} else {
			if (!v)
				continue;
			ss << delim;
			delim = "+";
			if (v!=1 || i==0)
				ss << v;
			if (i!=0)
				ss << "X";
			if (i!=0 && i!=1)
				ss << i;
		}
	}
	return ss.str();
}

S32 ply::dist0() const
{
	S32 i,n = deg;
	S32 cnt = 0;
	for (i=0;i<=deg;++i)
		if (coefs[i])
			++cnt;
	return cnt;
}

S32 ply::dist(const ply& rhs) const
{
	return (*this - rhs).dist0();
}
