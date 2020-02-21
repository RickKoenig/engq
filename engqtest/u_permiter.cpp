// permutation class with an iterator
#include <m_eng.h>
#include "u_permiter.h"

#ifdef DONAMESPACE_PERMITER
namespace permiter {
#endif

// study foreach later to simplify code
 // start perm as 0123..
void permReset(vector<S32>& perm, S32 order) {
	perm.clear();
	for (S32 i = 0; i < order; ++i) {
		perm.push_back(i);
	}
}



bool perm::checkIsIndex()
{
	vector<S32> hist(mOrder);
	for (auto p : mPerm) {
		if (p < 0 || p >= mOrder) {
			return false;
		}
		++hist[p];
	}
	for (auto h : hist) {
		if (h != 1)
			return false;
	}
	return true;
}

bool perm::operator==(const perm& rhs) const
{
	return mPerm == rhs.mPerm;
}

perm perm::operator*(const perm& rhs) const
{
	// is everything OK?
	if (!isIndex) {
		logger("operator*: perm lhs not an Index!!\n");
		return *this;
	}
	if (!rhs.isIndex) {
		logger("operator*: perm rhs not an Index!!\n");
		return *this;
	}
	if (mPerm.size() != rhs.mPerm.size()) {
		logger("operator*: sizes not the same %d %d!!\n",mPerm.size(),rhs.mPerm.size());
		return *this;
	}

	// everything is good, calculate *
	vector<S32> result(mOrder);// = rhs.mPerm;
	for (S32 i = 0; i < mOrder; ++i) {
		result[mPerm[i]] = rhs.mPerm[i];
	}
	perm ret = perm(result);
	return ret;
}

void perm::printPerm()
{
	// zeroth element printed on the left
	string s = " ";
	for (auto i : mPerm) {
		s += to_string2(i) + " ";
	}
	logger("printPerm = '%s'\n", s.c_str());
}



void permc::printNumCycles()
{
	if (!isIndex) {
		logger("printNumCycle: perm not an Index!!\n");
		return;
	}
	logger("printNumCycles = %d\n", getNumCycles());
}

void permc::calc()
{
	if (!isIndex)
		return;
	mCycles.clear();
	// find all cycles, start at beginning
	vector<bool> visited(mOrder);
	for (S32 i = 0; i < mOrder; ++i) {
		vector<S32> aCycle;
		S32 j = i;
		while (!visited[j]) {
			visited[j] = true;
			aCycle.push_back(j);
			j = mPerm[j];
		}
		if (aCycle.size() >= 2) { // only something that is a cycle
			mCycles.push_back(aCycle);
		}
	}
	// also calc numCycles
	S32 cycleCount = 1;
	perm cyc = perm(mOrder);
	perm wrap = cyc;
	while (true) {
		cyc = *this * cyc; // calc next position
		if (cyc == wrap) {
			break;
		}
		++cycleCount;
	}
	mNumCycles = cycleCount;
}

S32 permc::getNumCycles()
{
	return mNumCycles;
}

S32 permc::getLCMCycles()
{
	if (mCycles.empty())
		return 1;
	S32 p = 1;
	for (U32 i = 0; i < mCycles.size(); ++i) {
		p = lcm(p, mCycles[i].size());
	}
	return p;
}

void permc::printCycles()
{
	if (!isIndex) {
		logger("printCycles: perm not an Index!!\n");
		return;
	}
	string printStr = "";
	for (U32 i = 0; i < mCycles.size(); ++i) {
		printStr += "( ";
		const vector<S32>& aCycle = mCycles[i];
		for (U32 j = 0; j < aCycle.size(); ++j) {
			printStr += to_string2(aCycle[j]) + " ";
		}
		printStr += ")";
	}
	if (!mCycles.size())
		printStr = "E";

	logger("printCycles = %s\n", printStr.c_str());
}

void permc::printAll()
{
	printPerm();
	printNumCycles();
	printCycles();
}



vector<S32> permIter::getPerm() const
{
	return mPerm;
}


// get the next permutation of the vector
void permIter::next()
{
	vector<S32> ret = mPerm;
	if (mOrder == 0)
		return;
	// look for last increasing number
	S32 lastIncrease = -1;
	for (S32 i = 0; i < mOrder - 1; ++i) {
		if (mPerm[i] < mPerm[i + 1]) {
			lastIncrease = i;
		}
	}
	// put rest of list back to beginning of permutation
	reverse(mPerm.begin() + (lastIncrease + 1), mPerm.end());
	if (lastIncrease == -1) { // all descending, done
		noNext = true; // but it will wrap around to beginning when hasNext is called
		return;
	}
	S32 leftInt = mPerm[lastIncrease]; // this is the value to insert into the new permutation
	// now search for the lowest S32 greater than leftInt S32 in the rest of the array, rightInt
	S32 bignum = INT_MAX;
	S32 rightInt = bignum; // to start with
	S32 bestRightIndex = -1;
	// find best rightInt
	for (S32 i = lastIncrease + 1; i < mOrder; ++i) {
		S32 val = mPerm[i];
		if (val > leftInt && val < rightInt) {
			rightInt = val;
			bestRightIndex = i;
		}
	}
	// swap these 2 values, then ready to continue permuting!
	swap(mPerm[lastIncrease], mPerm[bestRightIndex]);
	return;
}

bool permIter::hasNext()
{
	if (mOrder == 0)
		return false;
	mWatchdog--;
	if (mWatchdog < 0) {
		logger("watchdog hit !!!\n");
		return false;
	}
	if (noNext) { // next time we are starting over
		noNext = false;
		return false;
	}
	return true;
}

#ifdef DONAMESPACE_PERMITER
} // end namespace permiter
using namespace permiter;
#endif



void testPermutionIterator()
{
	logger("\nin testPermutionIterator\n");

#if 1
// test just the perm class
	//permc aperm = perm(3);
	permc aperm = permc(vector<S32> {0, 1, 2, 3, 5, 4});
	//permc aperm = permc(vector<S32> {10, 1, 2, 3, 5, 4});
	logger("\ntest printAll\n");
	aperm.printAll();

	logger("\ntest operator*\n");
	permc A = permc(vector<S32> {0, 1, 5, 2, 4, 3});
	logger("A\n");
	A.printAll();

	permc B = permc(vector<S32> {2, 1, 0, 3, 4, 5});
	logger("B\n");
	B.printAll();

	permc C = A * B;
	logger("A*B = C\n");
	C.printAll();

	permc D = B * A;
	logger("B*A = D\n");
	D.printAll();
#endif

// test permutation iterators
	logger("\nnow do a permutation sequence\n");
//#define CUSTOM_SET
#ifdef CUSTOM_SET
	vector<S32> av{ 10,10,11,11 };
	permIter pi(av);
#else
	const S32 PERM_ORDER = 4;
	permIter pi{ PERM_ORDER };
#endif
	logger("start perm\n");
	permc pic = permc(pi.getPerm());
	pic.printAll();
	S32 numCounts = 2;
	for (S32 cc = 0; cc < numCounts; ++cc) {
		S32 permCount = 0;
		logger("\nrun %d/%d\n", cc + 1, numCounts);
		while (pi.hasNext()) {
			pic = permc(pi.getPerm());
			pic.printAll();
			S32 nc = pic.getNumCycles();
			S32 lcmCyc = pic.getLCMCycles();
			if (nc != lcmCyc) logger("failed LCM nc = %d, lcm nc = %d\n",nc,lcmCyc);
			pi.next();
			logger("\n");
			++permCount;
		}
		logger("TOTAL permCount = %d\n", permCount);
	}
	logger("\ndone perm sequence\n\n\n");
}
