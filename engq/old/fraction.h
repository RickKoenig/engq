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
	bool hasminv() const;
	fraction operator+=(const fraction& rhs);
	fraction operator-=(const fraction& rhs);
	fraction operator*=(const fraction& rhs);
	fraction operator/=(const fraction& rhs);
	fraction operator-() const;
	fraction abs() const;
	string tostring() const;
	S32 getnum() const {return n;}
	S32 getden() const {return d;}
	bool iszero() const {return n==0;}
	bool isnonzero() const {return n!=0;}
	bool operator==(const fraction& rhs) const {return n==rhs.n && d==rhs.d;}
	bool operator!=(const fraction& rhs) const {return n!=rhs.n || d!=rhs.d;}
	bool operator>(const fraction& rhs) const {return n*rhs.d>rhs.n*d;}
	bool operator>=(const fraction& rhs) const {return n*rhs.d>=rhs.n*d;}
	bool operator<(const fraction& rhs) const {return n*rhs.d<rhs.n*d;}
	bool operator<=(const fraction& rhs) const {return n*rhs.d<=rhs.n*d;}
	friend ostream& operator<<(ostream& stream,fraction ob);
};
