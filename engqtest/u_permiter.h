#define DONAMESPACE_PERMITER
#ifdef DONAMESPACE_PERMITER
namespace permiter {
#endif

void permReset(vector<S32>& perm, S32 order);


class perm {
protected:
	S32 mOrder; // size() of perm
	vector<S32> mPerm; // perm data
	bool isIndex; // 1 to 1 mapping available
public:
	perm(int order) : mOrder(order), isIndex(true) { permReset(mPerm, order); }
	bool checkIsIndex();
	perm(const vector<S32> alist) : mOrder(alist.size()), mPerm(alist), isIndex(checkIsIndex()) {}
	S32 getOrder() const { return mOrder; }
	bool operator==(const perm& rhs) const;
	perm operator*(const perm& rhs) const;
	void printPerm();
};


// perm with numCycles and Cycles
class permc : public perm {
protected:
	S32 mNumCycles{ 1 };
	vector<vector<S32>> mCycles;
public:
	permc(int order) : perm(order) { calc(); }
	permc(const vector<S32> alist) : perm(alist) { calc(); }
	permc(perm aperm) : perm(aperm) { calc(); }

	void calc(); // if is index, then calc cycle stuff
	void printNumCycles();
	S32 getNumCycles();
	void printCycles();
	void printAll();
	S32 getLCMCycles();
};


class permIter {
private:
	vector<S32> mPerm;
	S32 mOrder;
	bool noNext = false; // onshot hasNext
	S32 mWatchdog = 10000; // good to about 7! for now
public:
	permIter(S32 order) : mOrder(order) { permReset(mPerm, order); }
	permIter(const vector<S32> perm) : mPerm(perm), mOrder(mPerm.size()) {}
	void next();
	bool hasNext();
	vector<S32> getPerm() const;
};


#ifdef DONAMESPACE_PERMITER
} // end namespace permiter
using namespace permiter;
#endif

void testPermutionIterator();
