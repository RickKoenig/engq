// augmented matrices
// partial rewrite for mod

typedef zee2 T;

class augmatz {
	static const S32 maxwatch = 1000000;
	static bool lasterr;
	static S32 losum; // only keep solutions with low sums, -1 if don't care
public:
	S32 nr,nc; // non aug part
protected:
	S32 mod;
	vector<vector<T> > vals;
	vector<vector<T> > calcvals;
	vector<S32> pivotlistrow;
	vector<S32> pivotlistcol;
	bool noreset;
	S32 lastpivotrow;
public:
	static bool getlasterr() { return lasterr; }
	augmatz(S32 moda,S32 neqa,S32 nvara);
	void load(const U8* data);
	void reset();
	void setnoreset(bool set);
	void calc();
	void calc2();
	void print() const;
	void printcalc() const;
	vector<T> getnonhom() const;
	vector<vector<T> > getvarsc();
	vector<vector<T> > getvars();
//	vector<vector<T> > getvarsc2();
	vector<vector<T> > getvars2(bool losumsa);
	vector<vector<T> > getvarscomb(S32 mul); // do an 'add' operation first, test a mathematical property
	vector<vector<T> > getvarscomb2(S32 mul); // do an 'add' operation first, test a mathematical property
	vector<vector<T> > getvarsbrute();
	vector<T> getmult(vector<T> v);
	bool iter();
	static S32 getsum(vector<T> t);
	static void printvec(vector<T> t);
	static void printvars(vector<vector<T> > t);
	vector<vector<T> > checkvars(vector<vector<T> > invars);
	S32 getmod() const { return mod; }
	static void setlosum(S32 losumsa) { losum = losumsa; }
private:
	void setrc(S32 r,S32 c,T v);
	augmatz modreduce(S32 newmod);
	vector<vector<T> > upvars(vector<vector<T> > invars);
	vector<vector<T> > intersectsortvars(vector<vector<T> > invars0,vector<vector<T> > invars1);
	vector<vector<T> > getvars2sum(); // sum set losum
//	bool hasinv(T v);
	void calcpivot(S32 r,S32 c);
	bool calcpivot2(S32 r,S32 c);
	void swap(S32 r1,S32 r2);
	void scale(S32 r1,T f);
	void add(S32 r1,S32 r2,T f);
	vector<T> axeb(T a,T b);
public:
	static vector<vector<T> > allvars(S32 n); // just a list of all possible values
	vector<vector<T> > anyfront(vector<vector<T> > invars,S32 n); // prepend 'n' 'any's to list of invars
	vector<vector<T> > solve(vector<vector<T> > invars,S32 rpi,S32 cpi); // prepend solution's to list of invars
};
