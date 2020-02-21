class fraction
{
	S32 n,d; // always d>0, and reduced
	void reduce();
public:
	fraction(S32 w=0,S32 f=1) :n(w),d(f) { reduce(); }
	fraction(const fraction& f) : n(f.n),d(f.d) {}
	fraction operator+(const fraction& rhs) const;
	fraction operator-(const fraction& rhs) const;
	fraction operator++();
	fraction operator--();
	fraction operator++(int);
	fraction operator--(int);
	fraction operator*(const fraction& rhs) const;
	fraction operator/(const fraction& rhs) const;
	//bool hasminv() const;
	fraction operator+=(const fraction& rhs);
	fraction operator-=(const fraction& rhs);
	fraction operator*=(const fraction& rhs);
	fraction operator/=(const fraction& rhs);
	fraction operator-() const;
	fraction abs() const;
	string tostring() const;
	int getnum() const {return n;}
	int getden() const {return d;}
	bool iszero() const {return n==0;}
	bool isnonzero() const {return n!=0;}
//	const fraction& operator=(const fraction& rhs);//  {return *this;}
	bool operator==(const fraction& rhs) const {return n==rhs.n && d==rhs.d;}
	bool operator!=(const fraction& rhs) const {return n!=rhs.n || d!=rhs.d;}
	bool operator>(const fraction& rhs) const {return n*rhs.d>rhs.n*d;}
	bool operator>=(const fraction& rhs) const {return n*rhs.d>=rhs.n*d;}
	bool operator<(const fraction& rhs) const {return n*rhs.d<rhs.n*d;}
	bool operator<=(const fraction& rhs) const {return n*rhs.d<=rhs.n*d;}
	friend ostream& operator<<(ostream& stream,fraction ob);
};

class polynomial
{
	mutable vector<fraction> co; // because of the reverse in the compares
	void reduce();
//	void expand(S32 newdegree);
public:
	polynomial(const polynomial& p) {co=p.co;}
	polynomial(const vector<fraction>& v) {co=v;reduce();}
	polynomial() {} // 0
	polynomial(const fraction& a); // const
	polynomial(const fraction& a,const fraction& b); // linear
	polynomial(const fraction& a,const fraction& b,const fraction& c); // quadratic
	polynomial(const fraction& a,const fraction& b,const fraction& c,const fraction& d); // cubic
	polynomial(const fraction& a,const fraction& b,const fraction& c,const fraction& d,const fraction& e); // quartic
	polynomial(const fraction& a,const fraction& b,const fraction& c,const fraction& d,const fraction& e,const fraction& f); // quintic
	polynomial operator+(const polynomial& rhs) const;
	polynomial operator-(const polynomial& rhs) const;
	polynomial operator*(const polynomial& rhs) const;
	polynomial operator/(const polynomial& rhs) const;
	polynomial operator%(const polynomial& rhs) const;
	polynomial divmod(const polynomial& rhs,polynomial& remainder) const;
	polynomial operator-() const;
	string tostring() const;
	vector<fraction> tovector() const;
	fraction eval(const fraction& x) const;
	bool operator==(const polynomial& rhs) const;
	bool operator!=(const polynomial& rhs) const;
	bool operator>(const polynomial& rhs) const; // size first, then each term if both sizes equal
	bool operator>=(const polynomial& rhs) const;
	bool operator<(const polynomial& rhs) const;
	bool operator<=(const polynomial& rhs) const;
	S32 getdegree() const { return co.size()-1;} // only 0 will be -1 for degree
	fraction highest() const;
};

class complexf {
	float r,i;
public:
	complexf() {}
	complexf(float aa) : r(aa),i(0.0f) {}
	complexf(float aa,float ba) : r(aa),i(ba) {}
	complexf(const complexf& ca): r(ca.r),i(ca.i) {}
	complexf(const pointf2 ca): r(ca.x),i(ca.y) {} // typecast pointf2 to complexf
	complexf operator+(const complexf& rhs) const { return complexf(r+rhs.r,i+rhs.i); }
	complexf operator-(const complexf rhs) const { return complexf(r-rhs.r,i-rhs.i); }
	complexf operator*(const complexf rhs) const { return complexf(r*rhs.r-i*rhs.i,i*rhs.r+r*rhs.i); }
	complexf operator/(const complexf rhs) const;
	complexf operator-() const { return complexf(-r,-i); }
	string tostring() const;
	complexf conjugate() { return complexf(r,-i); }
	operator pointf2(); // typecast complexf to pointf2
};

class mat2c {
	complexf e[2][2];
public:
	mat2c() {}
	mat2c(complexf aa);
	mat2c(complexf a,complexf b,complexf c,complexf d);
	mat2c(const mat2c& ca);
	mat2c operator+(const mat2c& rhs) const;
	mat2c operator-(const mat2c rhs) const;
	mat2c operator*(const mat2c rhs) const;
	mat2c operator-();
	string tostring() const;
	complexf det() const;
	mat2c inv() const;
};

// handle a field of form 2^n
class field2 {
	static U32 prim; // x2+x+1
	static U32 primdeg;
	static U32 gas,gms; // sizeof of add and mult groups
	static vector<S32> lgfs; // log of field element size = 2^n
	static vector<U32> pows; // powers of w, size = 2^n - 1
	static vector<S32> base_lgfs; // log of field element size = 2^n // for cases like 11111
	static vector<U32> base_pows; // powers of w, size = 2^n - 1
	U32 v;
public:
	static void setprim(const C8*); // this call will invalidate any current instances of field2
	static U32 downgrade(U32 po); // downgrade prim power 'po' to base field, for cases like 11111
	field2() { v = 0; }
	field2(U32 i) {	v = i; }
	field2(const C8*);
	bool operator!=(const field2& rhs) const;
	field2 operator/(const field2& rhs) const;
	field2 operator+(const field2& rhs) const;
	field2 operator+=(const field2& rhs);
	field2 operator*(const field2& rhs) const;
	field2 operator*=(const field2& rhs);
	field2 operator-() const; 
	friend ostream& operator<<(ostream& stream,const field2& ob);
	static field2 p2f(S32 p);
	S32 f2p() const;
	bool iszero() const { return v==0; }
	static S32 getprimdeg() {return primdeg;}
};
