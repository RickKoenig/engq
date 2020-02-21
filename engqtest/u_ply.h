const S32 maxmod = 31;
bool isprime(int n);

class ply {
	S32 deg; // -1 if 0 polynomial
	static S32 mod; // 2 or more
	static bool packprint;
	static bool berlfact;
	vector<S32> coefs; // size is one more than deg

	void cleanup();
public:
	ply(const C8* s,S32 newdefmoda = 0);
	ply(); // zero poly
	ply(int a); // const poly
	ply(int a,int b); // linear poly
	ply(int a,int b,int c); // quadratic
	ply(int a,int b,int c,int d); // cubic
	ply(int a,int b,int c,int d,int e); // quartic
	ply(int a,int b,int c,int d,int e,int f); // quintic
	ply(const vector<int>& cfs); // any size first element is const term
	int getdeg() const {return deg;}
//	void setdeg(int d,int m);
	static int getmod() {return mod;}
	static void setmod(S32 newmod);
	static bool getpackprint() {return packprint;}
	static void setpackprint(bool packprinta) {packprint = packprinta;}
	static bool getberlfact() {return berlfact;}
	static void setberlfact(bool berlfacta) {berlfact = berlfacta;}
	void zero();
	bool iszero() const;
	int getcoef(int pow) const;
	void setcoef(int pow,int val);

	ply operator+(const ply& rhs) const; // assume mods match
	ply operator-(const ply& rhs) const;
	ply operator*(const ply& rhs) const;
	ply operator*(S32 c) const;
	ply operator/(const ply& rhs) const; // does cleanup
	ply operator%(const ply& rhs) const;
	bool operator==(const ply& rhs) const;
	bool operator<(const ply& rhs) const;
	bool operator!=(const ply& rhs) const;
	ply gcd(const ply& rhs) const;
	ply divmod(const ply& d,ply& r) const; // return quotient
	ply pow(S32 pw) const;
	ply compose(const ply& f) const; // calc this(f()))
	void monomial(int c,int d); // cx^d

	string print() const;

	int getrep() const; // 1 number to describe the poly
	void setrep(int r);
	vector<ply> factors() const;
	ply deriv() const;
	void inc();
	bool isprim(S32 dega = 0) const;
	S32 dist0() const; // return number of non zero coef
	S32 dist(const ply& rhs) const; // return number of different coef
};
