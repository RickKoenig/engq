// simple tests and one time utils
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"
#if 0

(a+bw+cw2)3+(a+bw2+cw)3 ==
+ 2a^3
+ 2b^3
+ 2c^3

- 3a^2b
- 3ab^2
- 3a^2c
- 3b^2c
- 3ac^2
- 3bc^2

+ 12abc
and is a symmetrical polynomial

((a+bw+cw2)3-(a+bw2+cw)3)2 ==
- 27a^4b^2
- 27a^2b^4
- 27a^4c^2
- 27b^4c^2
- 27a^2c^4
- 27b^2c^4

+ 54a^3b^3
+ 54a^3c^3
+ 54b^3c^3

+ 54a^4bc
+ 54ab^4c
+ 54abc^4

- 54a^3b^2c
- 54a^2b^3c
- 54a^3bc^2
- 54ab^3c^2
- 54a^2bc^3
- 54ab^2c^3

+ 162a^2b^2c^2
and is a symmetrical polynomial

#endif
namespace poly_sym {
// console
con32* con;

// ui
shape* rl;
shape* focus,*oldfocus;

pbut* bquit;
edit* epoly;
pbut* bcalc;
text* tresult;

class poly;

class base { // 3*b^2, 4^3, 2*(a+b)^5, 10*(a+2*b+3*b*c)^4
private:
public:
	// poly or letter to a power
	poly* p; // if 0 then is a letter, base is the owner
	S32 letter; // 0 a, 1 b etc.
	// to the
	S32 power; // 0 or greater

	base(S32 let,S32 pow) : p(0),letter(let),power(pow) {} // let ^ pow 0a, 1b etc.
	base(const poly& pp, S32 pow);
	base(poly* pp, S32 pow); // base gets ownership
	base(const base& br);
	~base();
	base& operator=(const base& rhs);
};

#define NPERMUTES 8 // a thru h
// compare 2 letters, letters are sorted i thru z, then a thru h
bool letterless(S32 a,S32 b)
{
	if (a >= NPERMUTES && b < NPERMUTES)
		return true;
	if (a < NPERMUTES && b >= NPERMUTES)
		return false;
	return a < b;
}

bool ispermletter(S32 a)
{
	return a>=0 && a<NPERMUTES;
}

bool baseless(const base& b1,const base& b2)
{
	if (b1.p)
		return false;
	if (b2.p)
		return true;
	return letterless(b1.letter,b2.letter); //b1.letter < b2.letter; hmm
}

class term { // product of bases
public:
	S32 coef;
	deque<base> bases;

	term() : coef(1) {}
	void sortandmerge();
	bool haspoly() const;
//	void clean();
	term mul(const term& rhs) const;
	term stripperms() const;
};

term term::stripperms() const
{
	term t;
	t.coef = coef;
	deque<base>::const_iterator bi = bases.begin();
	while(bi!=bases.end()) {
		if (!ispermletter(bi->letter))
			t.bases.push_back(*bi);
		++bi;
	}
	return t;
}

S32 special(S32 lett,S32 expo)
{
	if (lett == 'i' - 'a')
		return expo%4;
	if (lett == 'w' - 'a')
		return expo%3;
	return expo;
}

bool isquared(S32 lett,S32 expo)
{
	return lett == 'i' - 'a' && expo == 2;
}

bool icubed(S32 lett,S32 expo)
{
	return lett == 'i' - 'a' && expo == 3;
}

bool wsquared(S32 lett,S32 expo)
{
	return lett == 'w' - 'a' && expo == 2;
}

poly* getm1mw();

term term::mul(const term& rhs) const // assumes sorted bases
{
	term ret;
	ret.coef = coef * rhs.coef;
	deque<base>::const_iterator i1 = bases.begin();
	deque<base>::const_iterator i2 = rhs.bases.begin();
	while(i1 != bases.end() && i2!= rhs.bases.end()) {
		if (i2->p) // rhs has nothing left but polys, std copy
			break;
		if (i1->p) { // lhs has nothing left but polys, copy rhs letters
			ret.bases.push_back(*i2++);
			continue;
		}
		// just letters
		// no empties and no (poly), just 1 or more letters in each
		if (letterless(i1->letter,i2->letter)) {
//		if (i1->letter < i2->letter) { hmm
			ret.bases.push_back(*i1++);
		} else if (letterless(i2->letter,i1->letter)) {
//		} else if (i2->letter < i1->letter) { hmm
			ret.bases.push_back(*i2++);
		} else {
			S32 ex = i1->power+i2->power;
			ex = special(i1->letter,ex);
			if (ex>0) {
				if (isquared(i1->letter,ex)) { // i^2 == -1
					ret.coef *= -1;
				} else if (icubed(i1->letter,ex)) { // i^3 == -i
					base b(i1->letter,1);
					ret.coef *= -1;
					ret.bases.push_back(b);
				} else if (wsquared(i1->letter,ex)) { // w^2 == -1 - w
					poly* pp = getm1mw();
					base b(pp,1);
					ret.bases.push_back(b);
				} else { //
					base b(i1->letter,ex);
					ret.bases.push_back(b);
				}
			}
			++i1;
			++i2;
		}
	}
	while(i1 != bases.end()) { // copy lhs
		ret.bases.push_back(*i1++);
	}
	while(i2 != rhs.bases.end()) { // copy rhs
		ret.bases.push_back(*i2++);
	}
	return ret;
}

/*
void term::clean()
{
//remove bases with power of 0
	deque<base>::iterator it = bases.begin();
	while(it != bases.end()) {
		if (it->power == 0)
			it = bases.erase(it);
		else
			++it;
	}
// merge exponents
}
*/

// merge aabbb to a^2b^3 etc, just letters for now... (not polys)
void term::sortandmerge()
{
	sort(bases.begin(),bases.end(),baseless);
	deque<base> newbases;
	deque<base>::iterator it = bases.begin();
	S32 curletter = -1; // start with nothing
	S32 curexp = 0;
	while(it != bases.end()) {
		if (curletter != it->letter || it->p) { // different letter or start poly, write out old base (flush)
			if (curletter >= 0) { // only if something to flush
				curexp = special(curletter,curexp);
				if (curexp>0) {
					if (isquared(curletter,curexp)) { // i^2 == -1
						//ex = 0;
						coef *= -1;
					} else if (icubed(curletter,curexp)) { // i^3 == -i
						base b(curletter,1);
						coef *= -1;
						newbases.push_back(b);
					} else if (wsquared(curletter,curexp)) { // w^2 == -1 - w
						poly* pp = getm1mw();
						base b(pp,1);
						newbases.push_back(b);
					} else {
						base b(curletter,curexp);
						newbases.push_back(b);
					}
				}
			}
			if (it->p)
				curletter = -1;
			else
				curletter = it->letter;
			curexp = it->power;
		} else {
			curexp += it->power; // same base, add exponents
		}

		if (it->p) { // copy poly base immediately
			newbases.push_back(*it);
		}
		++it;
	}
	if (curletter >= 0) { // if no polys then flush last letter
		curexp = special(curletter,curexp);
		if (curexp>0) {
			if (isquared(curletter,curexp)) { // i^2 == -1
				//ex = 0;
				coef *= -1;
			} else if (wsquared(curletter,curexp)) { // w^2 == -1 - w
				poly* pp = getm1mw();
				base b(pp,1);
				newbases.push_back(b);
			} else if (icubed(curletter,curexp)) { // i^3 == -i
				base b(curletter,1);
				coef *= -1;
				newbases.push_back(b);
			} else {
				base b(curletter,curexp);
				newbases.push_back(b);
			}
		}
	}
	bases = newbases;
}

class poly {
	deque<term> pterms; // sum of terms

	static const C8* pp; // parse pointer

	void skip();
	S32 parseplusminus();
	S32 parseposnum(S32 def); // def=1 for coef and exponents
	term parseterm(S32 defsgn);
	S32 parseletter(); // def=1 for exponents // -1 not found, 0a, 1b etc.
//	poly parseparen();
	static poly expandterm(const term& t);
	void expandterms();
	void sortcleanterms();
public:
	poly(const C8* inittext);
//	poly(const poly& pr) {}
	poly() {}
	poly(const term& t) {pterms.push_back(t);}
	void calcsym() const;
	string pretty() const;
//	string result() const;
	poly mul(const poly& rhs) const;
	poly add(const poly& rhs) const;
	static poly* m1mw(); // returns -1-w same as w^2
	poly permpoly(const vector<S32>& pfunc) const;
	static bool polyequal(const poly& p1,const poly& p2);
	void calcele(const vector<S32> lets) const; // figure out elem sym poly from sym poly
	static poly getelem(const vector<S32> lets,S32 order,S32 coef); // get an elem sym poly
	poly getoneelemterm() const; // return best ele poly term
};

poly poly::expandterm(const term& t) // converts term to a poly with no paren
{
	poly ret;
	if (!t.haspoly()) { // no polys, can't expand term, just return poly with 1 term
		ret.pterms.push_back(t);
		return ret;
	}
	term t1;
	ret.pterms.push_back(t1);
	deque<base>::const_iterator it = t.bases.begin();
	while(it != t.bases.end()) {
		if (it->p) { // poly
			S32 pow = it->power;
			while(pow>0) {
				ret = ret.mul(*it->p);
				--pow;
			}
		} else { // letter
			term t;
			t.bases.push_back(*it);
			poly p;
			p.pterms.push_back(t);
			ret = ret.mul(p);
		}
		++it;
	}
	deque<term>::iterator itp = ret.pterms.begin();
	while(itp != ret.pterms.end()) {
		itp->coef *= t.coef;
		++itp;
	}
	ret.sortcleanterms();
	return ret;
}

void poly::skip()
{
	while(true) {
		if (*pp>= 'a' && *pp <= 'z')
			break;
		if (*pp>= '0' && *pp <= '9')
			break;
		if (*pp == '+' || *pp == '-')
			break;
		if (*pp == '(' || *pp == ')')
			break;
		if (!*pp)
			break;
		++pp;
	}
}

S32 poly::parseplusminus()
{
	skip();
	if (*pp == '-') {
		++pp;
		return -1;
	} else if (*pp == '+') {
		++pp;
		return 1;
	}
	return 0;
}

S32 poly::parseposnum(S32 def)
{
	skip();
	S32 num=0;
	bool dodef = true;
	while(*pp >= '0' && *pp <='9') {
		num = 10*num + (*pp - '0');
		++pp;
		skip();
		dodef = false;
	}
	if (dodef)
		num = def;
	return num;
}

S32 poly::parseletter()
{
	skip();
	S32 let = -1;
	if(*pp >= 'a' && *pp <='z') {
		let = *pp - 'a';
		++pp;
		skip();
	}
	return let;
}

term poly::parseterm(S32 defaultsign)
{
	skip();
	term t;
	S32 sgn = parseplusminus(); // + or -
	if (!sgn)
		sgn = defaultsign;
	S32 cf = parseposnum(1); // num
	t.coef = sgn * cf;
	while(*pp) {
		if (*pp == '-' || *pp == '+')
			break;
		if (*pp >= '0' && *pp <='9') {
			error("safety in parseterm");
			break;
		}
		if (*pp == ')') {
			break;
		}
		S32 let = parseletter(); // a
		if (let>=0) {
			S32 exp = parseposnum(1); // ^n
			base b(let,exp);
			if (exp)
				t.bases.push_back(b);
		} else if (*pp == '(') { // parse paren poly
			++pp;
			poly p = poly(0); // parse inside paren ()
			S32 exp = parseposnum(1); // ^n
			base b(p,exp);
			if (p.pterms.empty())
				t.coef = 0; // invalidate this term
			if (exp>0) // no need to clean 0 exp terms later
				t.bases.push_back(b);
		}
		skip();
	}
	t.sortandmerge(); // build up powers
	return t;
}

poly* getm1mw()
{
	return poly::m1mw();
}

poly* poly::m1mw()
{
	poly* ret = new poly;
	term t;
	t.coef = -1;
	ret->pterms.push_back(t);
	base b('w'-'a',1);
	t.bases.push_back(b);
	ret->pterms.push_back(t);
	return ret;
}

/*
poly poly::parseparen()
{
	skip();
	term t;
	S32 sgn = parseplusminus();
	if (!sgn)
		sgn = defaultsign;
	S32 cf = parseposnum(1);
	t.coef = sgn * cf;
	while(*pp) {
		if (*pp == '-' || *pp == '+')
			break;
		if (*pp >= '0' && *pp <='9') {
			error("safety in parseterm");
			break;
		}
		S32 let = parseletter();
		if (let>=0) {
			S32 exp = parseposnum(1);
			base b(let,exp);
			if (exp)
				t.bases.push_back(b);
		}
		skip();
	}
	return t;
}
*/

// assume bases already sorted, ignore coef
bool termless(const term& t1,const term& t2)
{
	deque<base>::const_iterator i1 = t1.bases.end();
	deque<base>::const_iterator i2 = t2.bases.end();
	while(true) {
		if (i2 == t2.bases.begin())
			return false; // can never be less than t1
		if (i1 == t1.bases.begin())
			return true; // is always t1 < t2
		// no empties at this point
		--i1;
		--i2;
		if (i1->p)
			return false;
		if (i2->p)
			return true;
		// just letters
		// no empties and no (poly), just 1 or more letters in each
		if (letterless(i1->letter , i2->letter)) //m
//		if (i1->letter < i2->letter) // hmm
			return true;
		else if (letterless(i2->letter , i1->letter))
//		else if (i1->letter > i2->letter) // hmm
			return false;
		// letters are equal, compare powers
		if (i1->power < i2->power)
			return true;
		if (i1->power > i2->power)
			return false;
	}
	return false; // never executed
}

// assume bases already sorted, ignore coef
bool termequal(const term& t1,const term& t2)
{
	deque<base>::const_iterator i1 = t1.bases.end();
	deque<base>::const_iterator i2 = t2.bases.end();
	while(true) {
		if (i1 == t1.bases.begin() && i2 == t2.bases.begin()) { // both empty
			return true;
		}
		if (i1 == t1.bases.begin() || i2 == t2.bases.begin()) // one empty
			return false;
		// no empties at this point
		--i1;
		--i2;
		if (i1->p || i2->p) // don't compare (poly)
			return false;
		// no empties and no (poly), just 1 or more letters in each
		if (i1->letter != i2->letter)
			return false;
		if (i1->power != i2->power)
			return false;
	}
	return false; // never executed
}

// assume bases already sorted, include coef
bool termequalcoef(const term& t1,const term& t2)
{
	if (t1.coef != t2.coef)
		return false;
	return termequal(t1,t2);
}

bool poly::polyequal(const poly& p1,const poly& p2)
{
	S32 n1 = p1.pterms.size();
	S32 n2 = p2.pterms.size();
	if (n1!=n2)
		return false;
	deque<term>::const_iterator i1 = p1.pterms.begin();
	deque<term>::const_iterator i2 = p2.pterms.begin();
	S32 i;
	for (i=0;i<n1;++i) {
		if (!termequalcoef(*i1,*i2))
			return false;
		++i1;
		++i2;
	}
	return true;
}

bool checkforone(const term& t)
{
	if (!t.bases.empty())
		return false;
	bool ret = t.coef == 1;
//	if (ret)
//		error("check for one");
	return ret;
}

// 2 dimensional distributive law
poly poly::mul(const poly& rhs) const // assumes sorted bases
{
	poly ret;
	deque<term>::const_iterator i1 = pterms.begin();
	deque<term>::const_iterator i2 = rhs.pterms.begin();
	while(i1!=pterms.end()) {
		i2 = rhs.pterms.begin();
		while(i2!=rhs.pterms.end()) {
			term t = i1->mul(*i2);
			ret.pterms.push_back(t);
			++i2;
		}
		++i1;
	}
	ret.sortcleanterms();
	return ret;
}

poly poly::add(const poly& rhs) const // assumes sorted bases and terms
{
	poly ret = *this; // lhs
	ret.pterms.insert(ret.pterms.end(),rhs.pterms.begin(),rhs.pterms.end());
	ret.sortcleanterms();
	return ret;
}

void poly::expandterms() // coverts poly to that of no parens
{
	deque<term> newpterms;
	deque<term>::iterator it = pterms.begin();
	while(it != pterms.end()) {
		poly ts = expandterm(*it);
		newpterms.insert(newpterms.end(),ts.pterms.begin(),ts.pterms.end());
		++it;
	}
	pterms = newpterms;
}

void poly::sortcleanterms() // highest letter , then highest exponent, combine like terms
{
	expandterms();
	deque<term>::iterator it;
	for(it = pterms.begin();it != pterms.end();++it) {
		it->sortandmerge(); // clean term first, build up powers, before sort
	}
	sort(pterms.begin(),pterms.end(),termless);
	deque<term> newpterms;
	S32 coef = 0; // start with nothing
	term oldt;
	for(it = pterms.begin();it != pterms.end();++it) {
		// it->sortandmerge(); // clean term first, build up powers
		if (it->haspoly() || !coef || !termequal(oldt,*it)) { // different term or start poly, write out old term (flush)
			if (coef) { // only if something to flush
				term t = oldt;
				t.coef = coef;
				checkforone(t);
				newpterms.push_back(t);
			}
			if (it->haspoly())
				coef = 0;
			else {
				oldt = *it;
				coef = it->coef;
			}
		} else {
			coef += it->coef; // same base, add coef
		}

		if (it->haspoly()) { // copy poly base immediately
			//checkforone(*it);
			newpterms.push_back(*it);
		}
	}
	if (coef) { // if no polys then flush last term
		term t = oldt;
		t.coef = coef;
		checkforone(t);
		newpterms.push_back(t);
	}
	pterms = newpterms;
}

poly::poly(const C8* inittext)
{
	// parse text into poly
	if (inittext)
		pp = inittext;
	else if (!pp)
		errorexit("no text to parse!");
	S32 defsgn = 1;  // first term doesn't need a plus or minus
	while(*pp) {
		if (*pp == ')') {
			++pp;
			break;
		}
		term t = parseterm(defsgn);
		defsgn = 0;
		if (t.coef)
			pterms.push_back(t);
		// like 2a2bc or -6a2b2c4 or a or -3
	}
	sortcleanterms(); // clean up the obvious
	expandterms(); // blow it up
	sortcleanterms(); // remove the waste
}

poly poly::permpoly(const vector<S32>& pfunc) const
{
	poly ret = *this;
	deque<term>::iterator jt;
	// work thru letters
	S32 n = pfunc.size();
	for (jt=ret.pterms.begin();jt!=ret.pterms.end();++jt) {
		deque<base>::iterator it;
		term& t = *jt;
		for (it=t.bases.begin();it!=t.bases.end();++it) {
			if (it->letter >=0 && it->letter<n) {
				it->letter = pfunc[it->letter];
			}
		}
	}
	return ret;
}
// poly is sorted and clean
void poly::calcsym() const
{
	vector<bool> hletters(NPERMUTES,false);
	deque<term>::const_iterator jt;
	S32 maxletter = -1;
	// scan for letters
	for (jt=pterms.begin();jt!=pterms.end();++jt) {
		deque<base>::const_iterator it;
		const term& t = *jt;
		for (it=t.bases.begin();it!=t.bases.end();++it) {
			if (it->letter >=0 && it->letter<NPERMUTES) {
				hletters[it->letter] = true;
				if (it->letter > maxletter)
					maxletter = it->letter;
			}
		}
	}
	vector<S32> lets;
	S32 i;
	// build a perm
	for (i=0;i<=maxletter;++i)
		if (hletters[i])
			lets.push_back(i);
	S32 n = lets.size();
	vector<S32> lets0 = lets; // sorted perm
	// run perm
	con32_printf(con,"Running perms ----------------\n");
	bool issym = true;
	poly sp;
	do { // there's at least 1 perm
		con32_printf(con,"Perm ");
		for (i=0;i<n;++i)
			con32_printf(con," %d,",lets[i]);
		con32_printf(con,"\n");
		// build perm func to perm the poly with
		vector<S32> pfunc;
		// identity
		for (i=0;i<=maxletter;++i)
			pfunc.push_back(i);
		// now add perm to function/array
		for (i=0;i<n;++i) {
			pfunc[lets0[i]] = lets[i];
		}
		con32_printf(con,"Perm func");
		for (i=0;i<=maxletter;++i)
			con32_printf(con," %d,",pfunc[i]);
		con32_printf(con,"\n");
		poly pp = permpoly(pfunc);
		pp.sortcleanterms();
		con32_printf(con,"poly = '%s'\n",pp.pretty().c_str());
		if (polyequal(*this,pp)) {
			con32_printf(con,"eq true\n\n");
		} else {
			con32_printf(con,"eq false\n\n");
			issym = false;
		}
		sp = sp.add(pp);
	} while (next_permutation(lets.begin(),lets.begin()+n));
	if (issym) {
		sp = *this; // don't use add, use original
		con32_printf(con,"sym ------ true ----------\n");
	} else { // sym is sum
		con32_printf(con,"sym ------ false, reverting to add mode ---------\n");
		con32_printf(con,"sym poly = %s\n",sp.pretty().c_str());
	}
//	if (issym) // always sym now...
		sp.calcele(lets0);
}

poly poly::getelem(const vector<S32> lets,S32 order,S32 coef) // get an elem sym poly
{
	poly ret;
	S32 n = lets.size();
	if (order > n)
		errorexit("bad getelem");
	if (order==0) { // 1
		term t;
		t.coef = coef;
		ret.pterms.push_back(t);
		return ret;
	} else if (order == 1) { // a+b+c
		S32 i;
		for (i=0;i<n;++i) {
			term t;
			base b(lets[i],1);
			t.bases.push_back(b);
			t.coef = coef;
			ret.pterms.push_back(t);
		}
	} else if (order == n) { // abc
		S32 i;
		term t;
		for (i=0;i<n;++i) {
			base b(lets[i],1);
			t.bases.push_back(b);
		}
		t.coef = coef;
		ret.pterms.push_back(t);
	} else { // ord >= 2 and ord < n
		S32 i;
		vector<S32> c;
		for (i=0;i<order;++i)
			c.push_back(i); // 0 1 2 3 for order 4
		do {
			// generate proper elem sym polys
			S32 j;
			S32 lv = -1;
			bool okord = true;
			for (j=0;j<order;++j) { // only terms in order ab ac bc etc.. ,  not ba bb
				if (lv >= c[j]) {
					okord = false;
					break;
				}
				lv = c[j];
			}
			if (okord) {
				term t;
				for (j=0;j<order;++j) {
					base b(lets[c[j]],1);
					t.coef = coef;
					t.bases.push_back(b);
					// con32_printf(con,"%d ",c[j]);
				}
				ret.pterms.push_back(t);
				// con32_printf(con,"\n");
			}
			// inc
			i=order-1;
			while (i>=0) {
				++c[i];
				if (c[i]<n)
					break;
				c[i] = 0;
				--i;
			}
		} while(i>=0);
	}
	ret.sortcleanterms();
	return ret;
}

// break sym poly into ele sym polys, assume already sorted and cleaned
void poly::calcele(const vector<S32> lets) const
{
	poly acc = *this;

	S32 w;
	con32_printf(con,"break apart sym poly\n");
	for (w=0;w<100;++w) { // watchdog
		//con32_printf(con,"acc = %s\n",acc.pretty().c_str());
		if (acc.pterms.empty()) {
			con32_printf(con,"empty sym poly, 0  done\n");
			return;
		}
		deque<term>::const_iterator it = acc.pterms.end();
		--it; // last term
		if (it->bases.empty()) {
			con32_printf(con,"empty highest base sym poly, (%s)*e[0]   done\n",acc.pretty().c_str());
			return;
		}
		deque<base>::const_iterator jt = it->bases.end();
		--jt; // highest valued term
		if (jt->p) {
			con32_printf(con,"error: poly in term of sym poly, 0  not good,   done\n");
			return;
		}
		//con32_printf(con,"highest letter is '%c^%d'\n",jt->letter+'a',jt->power);
		if (!ispermletter(jt->letter)) {
			con32_printf(con,"no perms (%s)*e[0]   done\n",acc.pretty().c_str());
			return;
		}
		// for each term in poly, scan thru exp from end to start
		// S32 n=lets.size();
		S32 m=acc.pterms.size();
		const term* t=&acc.pterms[m-1];
		S32 q = t->bases.size();
		vector<S32> ex;
		S32 k;
		for (k=q-1;k>=0;--k) {
			S32 po = t->bases[k].power;
			if (ispermletter(t->bases[k].letter))
				ex.push_back(po);
		}
		ex.push_back(0);
		q = ex.size();
		con32_printf(con,"ele sym poly ");
/*		if (q==1) { // nothing worth parsing e[0] == 1, checked above no perms ... done
			term ts = t->stripperms();
			con32_printf(con,"%s*e[0]",((poly)ts).pretty().c_str());
			ts.coef *= -1; // subtract mode
			poly ps;
			ps.pterms.push_back(ts);
			acc = acc.add(ps);
		} else { */
			term ts = t->stripperms();
			con32_printf(con,"%s",((poly)ts).pretty().c_str());
			ts.coef *= -1; // subtract mode
			poly ps;
			ps.pterms.push_back(ts);
			S32 ce = ex[0];
			for (k=1;k<q;++k) {
				S32 nce = ex[k];
				S32 del = ce - nce;
				if (del>0) {
					if (del>1)
						con32_printf(con,"*e[%d]^%d",k,del);
					else
						con32_printf(con,"*e[%d]",k);
					poly esp = getelem(lets,k,1);
					S32 z;
					for (z=0;z<del;++z)
						ps = ps.mul(esp);
				}
				ce = nce;
			}
			acc = acc.add(ps);
//		}
		con32_printf(con,"\n");
	}
	con32_printf(con,"error: watchdog hit in calcele, not good..\n");
}

string poly::pretty() const
{
	if (pterms.empty())
		return "0";
	stringstream ss;
// print pretty poly out of poly
	S32 i,m=pterms.size();
	for (i=0;i<m;++i) {
		const term* t = &pterms[i];
		if (i>0 && t->coef>=0) // + -
			ss << " + ";
		else if (t->coef<0)
			ss << " - ";
		S32 abscoef = abs(t->coef); // nn
		bool useabscoef = false;
		if (abscoef != 1)
			useabscoef = true;
		S32 j,n=t->bases.size();
		if (!useabscoef) {
			useabscoef = true;
			for (j=0;j<n;++j) {
				const base* b = &t->bases[j];
				if (b->power>0) {
					useabscoef = false;
					break;
				}
			}
		}
		if (useabscoef) // draw coeff if not 1 or nothing in bases
			ss << abscoef;
		for (j=0;j<n;++j) {
			const base* b = &t->bases[j];
			if (b->p) {
				ss << '(';
				if (b->power>1) { // a^2 or better
					ss << b->p->pretty() << ")^" << b->power;
				} else if (b->power==1) { // a
					ss << b->p->pretty() << ')';
				} else { // 0 // a^0 or 1, should have been removed by parser
					ss << b->p->pretty() << ")z" << b->power;
				}
			} else {
				if (b->power>1) { // a^2 or better
					ss << char('a' + b->letter) << "^" << b->power;
				} else if (b->power==1) { // a
					ss << char('a' + b->letter);
				} else { // 0 // a^0 or 1, should have been removed by parser
					ss << char('a' + b->letter) << "z" << b->power;
				}
			}
		}
	}
	return ss.str().c_str();
}

/*string poly::result() const
{
	return string("hi");
}*/

const C8* poly::pp;

base::base(const base& br) {
	if (br.p) {
		p = new poly(*br.p); // base now owns this
	} else
		p = 0;
	letter = br.letter;
	power = br.power;
}

base::base(const poly& pp, S32 pow): letter(0),power(pow)
{
	// make a copy of pp
	p = new poly(pp);
}

base::base(poly* pp, S32 pow): letter(0),power(pow)
{
	// base gets ownership of pp
	p = pp;
}


base::~base() {
	delete p;
};

/*envj& envj::operator=(const envj& rhs)
{
	if (this!=&rhs) {
		trackname=rhs.trackname;
		trackhash=rhs.trackhash;
		rules=rhs.rules;
		nlaps=rhs.nlaps;
		delete oldtrackj;
		oldtrackj = 0;
		newtrackj = 0;
		pc = rhs.pc;
	}
	return *this;
}
*/
base& base::operator=(const base& rhs)
{
	if (this == &rhs)
		return *this;
	delete p;
	if (rhs.p)
		p = new poly(*rhs.p);
	else
		p = 0;
	letter = rhs.letter;
	power = rhs.power;
	return *this;
}

// assume sorted
bool term::haspoly() const
{
	if (bases.empty())
		return false;
	deque<base>::const_iterator it = bases.end() - 1;
	return it->p != 0;
}

void calcpoly()
{
// read
	const C8* txt = epoly->gettname();
	poly p(txt); // poly is already clean and sorted
// pretty
//	epoly->settname(p.pretty().c_str());
	tresult->settname(p.pretty().c_str());
	con32_printf(con,"'%s'\n",p.pretty().c_str());
// calc
	p.calcsym();
}

}
using namespace poly_sym;

void polysyminit()
{
	video_setupwindow(1024,768);
	pushandsetdir("scratch");
	rl=res_loadfile("polysymres.txt");
	popdir();
	bquit=rl->find<pbut>("BQUIT");
	epoly=rl->find<edit>("EPOLY");
	bcalc=rl->find<pbut>("BCALC");
	tresult=rl->find<text>("TRESULT");
	con=con32_alloc(800,300,C32BLACK,C32WHITE);
	con32_printf(con,">>>\n");
	focus=oldfocus=0;
}

void polysymproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (focus == epoly && KEY == '\r')
		calcpoly();
// ui
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
// quit
	if (ret==1) {
		if (focus == bquit) {
			poporchangestate(STATE_MAINMENU);
		} else if (focus == bcalc) {
			calcpoly();
		}
	}
}

void polysymdraw2d()
{
	rl->draw();
	bitmap32* cb=con32_getbitmap32(con);
	clipblit32(cb,B32,0,0,112,400,cb->size.x,cb->size.y);
}

void polysymexit()
{
	delete rl;
	con32_free(con);
}
