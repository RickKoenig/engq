// polynomials in Z2
class z2p {
	U32 val;
	S32 deg; // upto power of 31 // 0 poly has -1 for degree
	static const S32 maxdeg = 31;
	void setdeg(); // using 'val', set deg
public:
	z2p(const C8* s);
	z2p(); // zero poly
	z2p(int a); // const poly
	z2p(int a,int b); // linear poly, low to high powers
	z2p(int a,int b,int c); // quadratic
	z2p(int a,int b,int c,int d); // cubic
	z2p(int a,int b,int c,int d,int e); // quartic
	z2p(const vector<int>& cfs); // any size, first element is const term
	int getdeg() const {return deg;}
	int getmod() const {return 2;}
	z2p operator+(const z2p& rhs) const; // assume mods match
	z2p operator-(const z2p& rhs) const;
	z2p operator*(const z2p& rhs) const;
	z2p operator*(S32 c) const;
	z2p operator/(const z2p& rhs);
	z2p operator%(const z2p& rhs);
	z2p divmod(const z2p& d,z2p& r); // return quotient
	z2p pow(S32 pw) const;
	z2p compose(const z2p& f) const; // calc this(f()))
	string print() const; // decimal
	string print2() const; // binary
//	void setdeg(int d,int m);
	void monomial(int c,int d); // cx^d
	int getrep() const {return val;} // 1 number to describe the poly
	void setrep(int r) {val = r;setdeg();}
	void zero() {val = 0; deg=-1;}
	bool iszero() const {return val==0;}
	U32 getcoef(int pow) const;
	vector<z2p> factors() const;
	S32 dist0() const;
	S32 dist(const z2p& rhs) const;
};
