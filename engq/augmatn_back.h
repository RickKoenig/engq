// augmented matrices
template <typename T>
class augmatn {
public:
	static const S32 sepx = 120;
	static const S32 sepy = 40;
	static const S32 startx = 10;
	static const S32 starty = 45;
	S32 nr,nc; // non aug part
	S32 phase; // 0 to echelon form, 1 to reduced echelon form, 2 done
	S32 r,c;
private:
	vector<vector<shape*> > pbutshapes;
	vector<vector<T> > vals;
	vector<vector<T> > calcvals;
public:
	augmatn<T>(S32 neqa,S32 nvara,shape* rla);
	bool isfocus(shape* s,S32& ri,S32& ci);
	void dofocus(S32 cr,S32 cc,U32 butval);
	void reset();
	void step();
	void calcpivot(S32 r,S32 c);
	void calc();
	bool isdone();
	void print();
private:
	void swap(S32 r1,S32 r2);
	void scale(S32 r1,T f);
	void add(S32 r1,S32 r2,T f);
};

template <typename T>
augmatn<T>::augmatn (S32 neqa,S32 nvara,shape* rla) : nr(neqa),nc(nvara)
{
	S32 i,j;
	if (nvara>5 || neqa > 5 || nvara<1 || nequ<1)
		errorexit("autmatn: bad args for constructor");
	static const C8* varnames= "XYZWAB";
	pbutshapes.resize(nr);
	vals.resize(nr);
	calcvals.resize(nr);
	for (i=0;i<nc;++i) {
		stringstream ss;
		ss << varnames[i];
		string s = ss.str();
		text* atext=new text("var",startx+sepx*i,starty-sepy,100,20,s.c_str());
		rla->addchild(atext);
	}
text* atext=new text("var",startx+sepx*nc,starty-sepy,100,20,"Q");
	rla->addchild(atext);
	for (j=0;j<nr;++j) { // row
		pbutshapes[j].resize(nc+1);
		vals[j].resize(nc+1);
		calcvals[j].resize(nc+1);
		for (i=0;i<=nc;++i) { // column, include augment column
			stringstream ss;
			ss << (j+1) << " " << (i+1);
			string name = ss.str();
			stringstream ss2;
			ss2 << vals[j][i];
			string vs = ss2.str();
			pbut* abut=new pbut(name.c_str(),startx+sepx*i,starty+sepy*j,100,20,vs.c_str());
			pbutshapes[j][i] = abut;
			rla->addchild(abut);
		}
	}
	reset();
}

template <typename T>
bool augmatn<T>::isfocus(shape* s,S32& ridx,S32& cidx)
{
	S32 i,j;
	for (j=0;j<nr;++j) { // row
		for (i=0;i<=nc;++i) { // column, include augment column
			if (pbutshapes[j][i] == s) {
				ridx = j;
				cidx = i;
				return true;
			}
		}
	}
	return false;
}

template <typename T>
void augmatn<T>::dofocus(S32 cr,S32 cc,U32 butval) // after isfocus returns true
{
	pbut* pb = dynamic_cast<pbut*>(pbutshapes[cr][cc]);
	if (pb) {
		stringstream ss2;
		T& vr = vals[cr][cc];
		if (butval==1) {
			--vr;
		} else if (butval==2) {
			++vr;
		}
		ss2 << vals[cr][cc];
		string vs = ss2.str();
		pb->settname(vs.c_str());
	}
}

template <typename T>
void augmatn<T>::swap(S32 r1,S32 r2)
{
	con32_printf(con,"swap rows %d and %d\n",r1+1,r2+1);
	S32 i;
	for (i=0;i<=nvar;++i) {
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
	con32_printf(con,"scale row %d by %10s\n",r1+1,ss.str().c_str());
	S32 i;
	for (i=0;i<=nvar;++i)
		calcvals[r1][i] *= f;
}

template <typename T>
void augmatn<T>::add(S32 r1,S32 r2,T f)
{
	stringstream ss;
	ss << f;
	con32_printf(con,"add %10s of row %d to row %d\n",ss.str().c_str(),r1+1,r2+1);
	S32 i;
	for (i=0;i<=nvar;++i)
		calcvals[r2][i] += f*calcvals[r1][i];
}

// reset the calculations
template <typename T>
void augmatn<T>::reset()
{
	con32_printf(con,"reset\n");
	S32 i,j;
	for (j=0;j<nr;++j) { // row
		for (i=0;i<=nc;++i) { // column, include augment column
			calcvals[j][i] = vals[j][i];
		}
	}
	phase = 0; // to echelon form
	r = c = 0;
	print();
}

template <typename T>
void augmatn<T>::step() // depreceated, just use calc, much simpler
{
	con32_printf(con,"step NYI\n");
}

template <typename T>
void augmatn<T>::calcpivot(S32 r,S32 c) // process this pivot
{
	con32_printf(con,"<<<<<< calcpivot at %d,%d >>>>>>\n",r+1,c+1);
	scale(r,T(1)/calcvals[r][c]);
	S32 j;
	for (j=0;j<nequ;++j) {
		if (j!=r) {
			if (calcvals[j][c]!=0) {
				add(r,j,-calcvals[j][c]);
			}
		}
	}
}

template <typename T>
void augmatn<T>::calc()
{
	con32_printf(con,"================ calc ================\n");
	reset();
	// find all pivots for each row
	S32 j;
	S32 c = 0;
	S32 r = 0;
	for (c=0;c<nvar;++c) {
		for (j=r;j<nequ;++j) { // find pivot in the column c
			if (calcvals[j][c]!=0)
				break;
		}
		if (j==nequ) {
			con32_printf(con,"no pivot in column %d\n",c+1);
		} else if (j==r) {
			stringstream ss;
			ss << calcvals[j][c];
			con32_printf(con,"pivot %10s in column %d in row %d no swap\n",ss.str().c_str(),c+1,j+1);
			calcpivot(r,c);
			++r;
		} else {
			stringstream ss;
			ss << calcvals[j][c];
			con32_printf(con,"pivot %10s in column %d swap rows %d and %d\n",ss.str().c_str(),c+1,r+1,j+1);
			swap(j,r);
			calcpivot(r,c);
			++r;
		}
		print();
	}
}

template <typename T>
bool augmatn<T>::isdone()
{
	return phase == 2;
}

template <typename T>
void augmatn<T>::print()
{
	S32 i,j;
	for (j=0;j<nequ;++j) {
		for (i=0;i<nvar;++i) {
			stringstream ss;
			ss << calcvals[j][i];
			con32_printf(con,"%10s ",ss.str().c_str()); // X Y Z ...
		}
		stringstream ss2;
		ss2 << calcvals[j][nvar];
		con32_printf(con,"| %10s\n",ss2.str().c_str()); // Q
	}
}

