// augmented matrices
template <typename T>
class augmatn {
public:
	S32 nr,nc; // non aug part
protected:
	vector<vector<T> > vals;
	vector<vector<T> > calcvals;
	vector<S32> pivotlistrow;
	vector<S32> pivotlistcol;
public:
	augmatn<T>(S32 neqa,S32 nvara);
	void reset();
	void calc();
	void print() const;
	void printcalc() const;
	vector<T> getnonhom() const;
	vector<vector<T> > getvarsc();
	vector<T> getmult(vector<T> v) const;
	bool iter(S32 moda);
	static void printvec(vector<T> t);
	static void printvars(vector<vector<T> > t);
	void setrc(S32 r,S32 c,T v);
	void set(const T* data,S32 datasize);
private:
	bool hasinv(T v);
	void calcpivot(S32 r,S32 c);
	void swap(S32 r1,S32 r2);
	void scale(S32 r1,T f);
	void add(S32 r1,S32 r2,T f);
};

template <typename T>
augmatn<T>::augmatn (S32 neqa,S32 nvara) : nr(neqa),nc(nvara)
{
	S32 j;
	if (nc>255 || nr > 255 || nc<1 || nr<1)
		errorexit("autmatn: bad args for constructor");
	vals.resize(nr);
	calcvals.resize(nr);
	for (j=0;j<nr;++j) { // row
		vals[j].resize(nc+1);
		calcvals[j].resize(nc+1);
	}
	reset();
}

template <typename T>
void augmatn<T>::swap(S32 r1,S32 r2)
{
	gcon_printf("swap rows %d and %d\n",r1+1,r2+1);
	S32 i;
	for (i=0;i<=nc;++i) {
		T t;
		t = calcvals[r1][i];
		calcvals[r1][i] = calcvals[r2][i];
		calcvals[r2][i] = t;
	}
}

template <typename T>
void augmatn<T>::scale(S32 r1,T f)
{
	stringstream ss;
	ss << f;
	gcon_printf("scale row %d by %10s\n",r1+1,ss.str().c_str());
	S32 i;
	for (i=0;i<=nc;++i)
		calcvals[r1][i] *= f;
}

template <typename T>
void augmatn<T>::add(S32 r1,S32 r2,T f)
{
	stringstream ss;
	ss << f;
	gcon_printf("add %10s of row %d to row %d\n",ss.str().c_str(),r1+1,r2+1);
	S32 i;
	for (i=0;i<=nc;++i)
		calcvals[r2][i] += f*calcvals[r1][i];
}

// reset the calculations
template <typename T>
void augmatn<T>::reset()
{
	gcon_printf("reset\n");
	S32 i,j;
	for (j=0;j<nr;++j) { // row
		for (i=0;i<=nc;++i) { // column, include augment column
			calcvals[j][i] = vals[j][i];
		}
	}
	pivotlistrow = vector<S32>(nr,-1);
	pivotlistcol = vector<S32>(nc,-1);
}
template <typename T>
void augmatn<T>::calcpivot(S32 r,S32 c) // process this pivot
{
	gcon_printf("<<<<<< calcpivot at %d,%d >>>>>>\n",r+1,c+1);
//	bool hi = hasinv(calcvals[r][c]);
	scale(r,T(1)/calcvals[r][c]); // the only divide
	S32 j;
	for (j=0;j<nr;++j) {
		if (j!=r) {
			if (calcvals[j][c]!=0U) {
				add(r,j,-calcvals[j][c]);
			}
		}
	}
	pivotlistrow[r] = c;
	pivotlistcol[c] = r;
}


#if 0
template <typename T>
bool augmatn<T>::hasinv(T v)
{
	return v.hasminv();
}
#endif
template <typename T>
void augmatn<T>::calc()
{
	gcon_printf("================ calc ================\n");
	reset();
	// find all pivots for each row
	S32 j;
	S32 c = 0;
	S32 r = 0;
	for (c=0;c<nc;++c) {
		for (j=r;j<nr;++j) { // find pivot in the column c
			if (calcvals[j][c]!=0U) // later on, find largest pivot...
				break;
		}
		if (j==nr) {
			gcon_printf("no pivot in column %d\n",c+1);
		} else if (j==r) {
			stringstream ss;
			ss << calcvals[j][c];
			gcon_printf("pivot %10s in column %d in row %d no swap\n",ss.str().c_str(),c+1,j+1);
			calcpivot(r,c);
			++r;
		} else {
			stringstream ss;
			ss << calcvals[j][c];
			gcon_printf("pivot %10s in column %d swap rows %d and %d\n",ss.str().c_str(),c+1,r+1,j+1);
			swap(j,r);
			calcpivot(r,c);
			++r;
		}
		printcalc();
	}
}

template <typename T>
void augmatn<T>::print() const
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<nc;++i) {
			stringstream ss;
			ss << vals[j][i];
			gcon_printf("%10s ",ss.str().c_str()); // X Y Z ...
		}
		stringstream ss2;
		ss2 << vals[j][nc];
		gcon_printf("| %10s\n",ss2.str().c_str()); // Q
	}
}

template <typename T>
void augmatn<T>::printcalc() const
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<nc;++i) {
			stringstream ss;
			ss << calcvals[j][i];
			gcon_printf("%10s ",ss.str().c_str()); // X Y Z ...
		}
		stringstream ss2;
		ss2 << calcvals[j][nc];
		gcon_printf("| %10s\n",ss2.str().c_str()); // Q
	}
}

template <typename T>
void augmatn<T>::printvec(vector<T> t)
{
	S32 i,n=t.size();
	for (i=0;i<n;++i) {
		stringstream ss;
		ss << t[i];
		gcon_printf("%10s ",ss.str().c_str()); // X Y Z ...
	}
	gcon_printf("\n"); // Q
}

template <typename T>
void augmatn<T>::printvars(vector<vector<T> > vars)
{
	S32 i;
	if (vars.size()>10000) {
		gcon_printf("too many vars %d\n",vars.size());
		return;
	}
	for (i=0;i<(S32)vars.size();++i)
		printvec(vars[i]);
}

template <typename T>
vector<T> augmatn<T>::getnonhom() const // = column, return size in rows
{
	vector<T> ret(nr);
	S32 i;
	for (i=0;i<nr;++i)
		ret[i] = vals[i][nc];
	return ret;
}

template <typename T>
vector<vector<T> >augmatn<T>::getvarsc() // calc, return size in columns 'solution', or 0 size if no 'solution'
{
	calc();
	vector<vector<T> >ret;
	S32 i,j;
	for (i=0;i<nr;++i) { // check '0' rows for 'non 0 ' nonhom
		if (pivotlistrow[i]==-1) {
			if (calcvals[i][nc]!=0U) {
				return ret; // something like 0x + 0y + 0z = 5, no solution
			}
		}
	}
	// build nonhom
	vector<T> reti = vector<T>(nc);
	for (i=0;i<nr;++i) {
		j = pivotlistrow[i]; // get col(var)
		if (j>=0)
			reti[j] = calcvals[i][nc];
	}
	ret.push_back(reti);
	// build hom
	for (i=0;i<nc;++i) {
		if (pivotlistcol[i] == -1) { // this column has no pivot
			reti = vector<T>(nc);
			reti[i] = (T)1; // var
			for (j=0;j<nr;++j) {
				S32 pc = pivotlistrow[j];
				if (pc>=0) {
					reti[pc] = -calcvals[j][i];
				}
			}
			ret.push_back(reti);
		}
	}
	return ret;
}

template <typename T>
vector<T> augmatn<T>::getmult(vector<T> vars) const // run some vars to get = column, return size in rows
{
	vector<T> ret(nr);
	S32 i,j;
	for (j=0;j<nr;++j) {
		T v = 0;
		for (i=0;i<nc;++i) {
			v += vals[j][i]*vars[i];
		}
		ret[j] = v;
	}
	return ret;
}

template <typename T>
bool augmatn<T>::iter(S32 moda) // next aug matrix, iterate through all of them
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<=nc;++i) {
			T v = vals[j][i];
			++v;
			if (v == moda) {
				v = 0;
				vals[j][i] = v;
			} else {
				vals[j][i] = v;
				return true;
			}
		}
	}
	return false;
}

template <typename T>
void augmatn<T>::setrc(S32 r,S32 c,T v)
{
	vals[r][c] = v;
}

template <typename T>
void augmatn<T>::set(const T* data,S32 size)
{
	if (size != nr*(nc+1))
		errorexit("can't set augmatn");
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<=nc;++i) {
			setrc(j,i,data[j*(nc+1)+i]);
		}
	}
}
