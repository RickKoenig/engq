template <S32 T>
class zee {
	void reduce();
	zee<T> minv() const;
public:
	S32 v; // always 0 to T-1 and reduced
	zee(S32 va=0) :v(va) { reduce(); }
	zee(const zee& f) : v(f.v) {}
	zee operator+(const zee& rhs) const;
	zee<T> operator-(const zee<T>& rhs) const;
	zee operator++();
	zee operator--();
	zee operator++(int);
	zee operator--(int);
	zee<T> operator*(const zee<T>& rhs) const;
	zee<T> operator/(const zee<T>& rhs) const;
	bool hasminv() const;
	zee operator+=(const zee& rhs);
	zee operator-=(const zee& rhs);
	zee operator*=(const zee& rhs);
	zee<T> operator/=(const zee<T>& rhs);
	zee operator-() const;
//	zee abs() const;
	string tostring() const;
	S32 getval() const {return v;}
	bool iszero() const {return v==0;}
	bool isnonzero() const {return v!=0;}
//	const zee& operator=(const zee& rhs);//  {return *this;}
	bool operator==(const zee& rhs) const {return v==rhs.v;}
	bool operator!=(const zee& rhs) const {return v!=rhs.v;}
	bool operator>(const zee& rhs) const {return v>rhs.v;}
	bool operator>=(const zee& rhs) const {return v>=rhs.v;}
	bool operator<(const zee& rhs) const {return v<rhs.v;}
	bool operator<=(const zee& rhs) const {return v<=rhs.v;}
//	template <S32 U>
//	friend ostream& operator<<(ostream& stream,zee<U> ob);
};

template <S32 T>
void zee<T>::reduce()
{
	v %= T;
	if (v<0)
		v += T;
}

template <S32 T>
zee<T> zee<T>::operator+(const zee& rhs) const
{
	zee r(v+rhs.v);
	return r;
}

template <S32 T>
zee<T> zee<T>::operator-(const zee& rhs) const
{
	zee r(v-rhs.v);
	return r;
}

template <S32 T>
zee<T> zee<T>::operator++()
{
	++v;
	reduce();
	return *this;
}

template <S32 T>
zee<T> zee<T>::operator--()
{
	--v;
	reduce();
	return *this;
}

template <S32 T>
zee<T> zee<T>::operator++(int)
{
	zee r = *this;
	++v;
	reduce();
	return r;
}

template <S32 T>
zee<T> zee<T>::operator--(int)
{
	zee r = *this;
	--v;
	reduce();
	return r;
}

template <S32 T>
zee<T> zee<T>::operator*(const zee<T>& rhs) const
{
	zee<T> r(v*rhs.v);
	return r;
}

template <S32 T>
bool zee<T>::hasminv() const
{
	return gcd(T,v)==1;
}

template <S32 T>
zee<T> zee<T>::minv() const
{
	S32 i;
	for (i=1;i<T;++i) {
		zee<T> r = *this * i;
		if (r.v == 1)
			return zee<T>(i);
	}
	gcon_printf("zee<%d>::minv() WARNING: no inverse for %d\n",T,v);
	zee<T> r;
	return r;
}

template <S32 T>
zee<T> zee<T>::operator/(const zee<T>& rhs) const
{
	if (!rhs.hasminv()) {
		gcon_printf("WARNING: zee divide by 0\n");
		zee<T> r;
		return r;
	}
	zee r(rhs.minv()*v);
	gcon_printf("%d/%d = %d\n",v,rhs.v,r.v);
	return r;
}

template <S32 T>
zee<T> zee<T>::operator-() const
{
	zee r(-v);
	return r;
}

template <S32 T>
zee<T> zee<T>::operator+=(const zee& rhs)
{
	*this = *this + rhs;
	return *this;
}

template <S32 T>
zee<T> zee<T>::operator-=(const zee& rhs)
{
	*this = *this - rhs;
	return *this;
}

template <S32 T>
zee<T> zee<T>::operator*=(const zee& rhs)
{
	*this = *this * rhs;
	return *this;
}

template <S32 T>
zee<T> zee<T>::operator/=(const zee& rhs)
{
	*this = *this / rhs;
	return *this;
}


/*template <S32 T>
zee<T> zee<T>::abs() const
{
	S32 va = ::abs(v);
	zee r(va);
	return r;
} */

template <S32 T>
string zee<T>::tostring() const
{
	stringstream ss;
	ss << v;
	return ss.str();
}

template <S32 U>
ostream& operator<<(ostream& stream,zee<U> ob)
{
	stream << ob.v;
	return stream;
}

