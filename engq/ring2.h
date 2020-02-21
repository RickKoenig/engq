class zee2 {
public:
	S8 v; // always 0 to m-1 and reduced
	S8 m; // mod
	static const S32 maxmod = 31;
private:
	static S32 defmod;
	S32* inva2;
	static S32 invarr2[maxmod+1][maxmod]; // [mod][val]
	void reduce();
public:
//	S32* inva;
	static void setdefmod(S32 ma);
	static S32 getdefmod() { return defmod; }
	zee2(S32 va = 0);
//	zee2(S32 va = 0) : v(va),m(defmod),inva(invarr[defmod]) { reduce(); }
//	zee2(const zee2& f) : v(f.v) {}
	zee2 minv() const; // multiplicative inverse
	zee2 operator+(const zee2& rhs) const;
	zee2 operator-(const zee2& rhs) const;
	zee2 operator++();
	zee2 operator--();
	zee2 operator++(int);
	zee2 operator--(int);
	zee2 operator*(const zee2& rhs) const;
	zee2 operator/(const zee2& rhs) const;
	bool hasminv() const;
	zee2 operator+=(const zee2& rhs);
	zee2 operator-=(const zee2& rhs);
	zee2 operator*=(const zee2& rhs);
	zee2 operator/=(const zee2& rhs);
	zee2 operator-() const;
//	zee2 abs() const;
	string tostring() const;
	S32 getval() const {return v;}
	bool iszero() const {return v==0;}
	bool isnonzero() const {return v!=0;}
//	const zee2& operator=(const zee2& rhs);//  {return *this;}
	bool operator==(const zee2& rhs) const {return v==rhs.v;}
	bool operator!=(const zee2& rhs) const {return v!=rhs.v;}
	bool operator>(const zee2& rhs) const {return v>rhs.v;}
	bool operator>=(const zee2& rhs) const {return v>=rhs.v;}
	bool operator<(const zee2& rhs) const {return v<rhs.v;}
	bool operator<=(const zee2& rhs) const {return v<=rhs.v;}
//	template <S32 U>
//	friend ostream& operator<<(ostream& stream,zee2<U> ob);
	// cast to S32
	operator S32() const {return v;}
};

ostream& operator<<(ostream& stream,zee2 ob);
