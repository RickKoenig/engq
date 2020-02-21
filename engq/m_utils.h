template <typename T>
void exch(T& a,T& b)
{
	T t;
	t=a;
	a=b;
	b=t;
}

template <typename T>
T range(const T& a,const T& b,const T& c)
{
	if (b<a)
		return a;
	if (b>c)
		return c;
	return b;
}

#define NUMELEMENTS(a) (sizeof(a)/sizeof(*a))

// 'to_string' doesn't work in codeblocks for some reason
// so we introduce 'to_string2' which does the same thing
string to_string2(S32 val);
