#include <ctime>        // std::time
//#include <type_traits> // c++11 and up

/*bool lessthanS32(S32 a,S32 b)
{
	//return false;
	return a < b;
}

bool lessthanfloat(float a,float b)
{
	//return false;
	return a < b;
}*/

template <typename T>
bool lessthangeneric(T a,T b)
{
	return a < b;
}

template <typename T>
class median {
	vector<T> data;
	mutable vector<T> sorteddata; // only used when unit testing
	static const U32 divsize = 5;
	typedef bool (*lessthantype)(const T& a,const T& b);
	lessthantype lessthan;

/*	static void usort(typename vector<T>::iterator b,typename vector<T>::iterator e,lessthantype pred)
	{
		if (pred)
			sort(b,e,pred);
		else
			sort(b,e);
	} */

	T medianofmedian() const
	{
		vector<vector<T> > sublists;
		const U32 sublistsize = divsize;
		//U32 i; // sub lists
		U32 k; // whole list
		vector<T> cursublist;
		for (k=0;k<data.size();++k) {
			cursublist.push_back(data[k]);
			if (cursublist.size() == sublistsize) {
				sublists.push_back(cursublist);
				cursublist.clear();
			}
		}
		if (!cursublist.empty()) {
			sublists.push_back(cursublist);
		}

		//2) Sort each sublist and determine the median.
		//Sorting very small lists takes linear time since these sublists have five elements, and this takes  time.
		//In the algorithm described on this page, if the list has an even number of elements,
		// take the floor of the length of the list divided by 2 to find the index of the median.
		vector<T> medians;
		for (k=0;k<sublists.size();++k) {
			vector<T>& c = sublists[k];
			sort(c.begin(),c.end(),lessthan);
			U32 medidx = c.size()/2;
			T medval = c[medidx];
			medians.push_back(medval);
		}
		if (medians.size() <= divsize) { // just one sublist of medians, return that median, speedup by handling up to 5 medians
		//if (medians.size() <= 1) { // slowdown
			sort(medians.begin(),medians.end(),lessthan);
			return medians[medians.size()/2];
		}
		//3)Use the median-of-median algorithm to recursively determine the median of the set of all the medians.
		median<T> sm(medians,lessthan);
		T ret = sm.medianofmedian();
		return ret;
	}

	void splitval(vector<T>& left,vector<T>& middle,vector<T>& right,T pivot)
	{
		// split into left middle right
		U32 i,n = data.size();
		left.clear();
		middle.clear();
		right.clear();
		for (i=0;i<n;++i) {
			T v = data[i];
			if (lessthan(v,pivot)) {
				left.push_back(v);
			} else if (lessthan(pivot,v)) {
				right.push_back(v);
			} else {
				middle.push_back(v);
			}
		}
	}

public:
	median(const T* adata,U32 numele,lessthantype less = lessthangeneric) : lessthan(less)
	{
		data.assign(adata,adata + numele);
	}

	median(const vector<T>& adata,lessthantype less = lessthangeneric) : lessthan(less)
	{
		data = adata;
	}

	U32 size() const
	{
		return data.size();
	}


// median of medians algorithm finds median in an unordered set in O(n) time
// 1/2
	T getmedian() const
	{
		return getvalue(data.size()/2);
	}
// now works with some elements the same
	T getvalue(U32 idx) const // returns value at index if array if it was sorted, but it is not really sorted, O(n)
	{
		// trivial case
		if (data.empty())
			return T(); // not even valid for size == 0
		//else
		//	return data[idx];
		if (data.size() <= divsize) { // speedup for 5 or less
		//if (data.size() <= 1) { // slowdown
			vector<T> sv = data;
			sort(sv.begin(),sv.end(),lessthan);
			return sv[idx];
		}

		// from brilliant.org
		//1) Divide the list into sublists each of length five (if there are fewer than five elements available for the last list, that is fine).
		//4) Use this median as the pivot element x. The pivot is an approximate median of the whole list and then each recursive step hones in on the true median.
		T pivot = medianofmedian();
		// partition
		//5) Reorder  A such that all elements less than  A are to the left of x, and all elements of  A that are greater than  x are to the right.
		//This is called partitioning. The elements are in no particular order once they are placed on either side of x.
		//For example, if x = 5, the list to the right of x maybe look like 7,8,12,15 (i.e. not in sorted order).
		//This takes linear time since  O(n) comparisons occur each element in A is compared against x only.
		// TODO: move this into split function
		vector<T> left;
		vector<T> right;
		U32 i,n = data.size();
		U32 middlecount = 0;
		// split into left middle right
		for (i=0;i<n;++i) {
			T v = data[i];
			if (lessthan(v,pivot)) {
				left.push_back(v);
			} else if (lessthan(pivot,v)) {
				right.push_back(v);
			} else {
				++middlecount;
			}
		}
		// done split left middle right
		U32 middleidx = left.size();
		U32 rightidx = middleidx + middlecount;
		//6)Let k be the rank of x meaning, for a set of numbers S, x is the kth smallest number in S.
		//7a) If idx == middleidx, then return pivot
		if (idx >= middleidx && idx < rightidx) { // in the middle
			return pivot;
		//7b) If idx < middleidx , then recurse using median-of-medians on A[0 to middleidx-1], idx)
		} else if(idx < middleidx) { // to the left
			median<T> lm(left,lessthan);
			return lm.getvalue(idx);
		//7c) If idx > middleidx, recurse using the median-of-medians algorithm on A[middleidx + 1 to end], idx - middleidx
		} else { //if(idx > middleidx) { // over to the right past all the middles
			median<T> rm(right,lessthan);
			return rm.getvalue(idx - rightidx);
		}



	/*
		// placeholder
		if (data.empty())
			return 0;
		else
			return data[idx]; */
	}

	void splitmedian(vector<T>& left,vector<T>& middle,vector<T>& right)
	{
		splitidx(left,middle,right,data.size()/2);
	}

	void splitidx(vector<T>& left,vector<T>& middle,vector<T>& right,U32 idx)
	{
		T val = getvalue(idx);
		splitval(left,middle,right,val);

	}

	T getsortedvalue(U32 idx) const // returns value at index of sorted array, O(n*lg(n)), only sort once
	{
		if (data.size() != sorteddata.size()) {
			// request sorted data
			sorteddata = data;
			/*if (lessthan)
				sort(sorteddata.begin(),sorteddata.end(),lessthan);
			else
				sort(sorteddata.begin(),sorteddata.end());*/
			sort(sorteddata.begin(),sorteddata.end(),lessthan);
		}
		return sorteddata[idx];
	}

	T getrawvalue(U32 idx) const // just return index of unsorted array, no work
	{
		return data[idx];
	}


	// unit test
	bool testmedian(const string& title,S32 testnumber = -1) const // does a pretty good check of getvalue against slower getsortedvalue for consistancy
	{
		U32 i,ne=data.size();
		for (i=0;i<ne;++i) {
			if (getvalue(i) != getsortedvalue(i))
				break;
		}
		if (i == ne) { // full match
			//logger("--- %s %d ---, match !!!\n",title.c_str(),testnumber);
			return true;
		}
		logger("--- %s %d ---, mismatch\n",title.c_str(),testnumber);
/*		for (i=0;i<ne;++i) {
			S32 v = getvalue(i);
			S32 sv = getsortedvalue(i);
			S32 rv = getrawvalue(i);
			//if (is_same<T,S32>::value) {
			//}
			logger("\tindex value at %6u = %6d, test sorted value = %6d, raw unsorted value = %6d\n",i,v,sv,rv);
		} */
		return false;
	}

	static void unittest()
	{
		// first try example from brilliant.org
		S32 medtestarr[] = {25,21,98,100,76,76,76,22,43,60,89,87};
		U32 ne = NUMELEMENTS(medtestarr);
		median<S32> mo(medtestarr,ne);
		bool allpass = true;
		allpass = allpass && mo.testmedian("brilliant");

		// now run a lot more super random tests
		const U32 testnum = 2;
		U32 i = 0;
//#define INTTYPE
#define FLOATTYPE
#ifdef INTTYPE
		typedef S32 type;
#define lessthanfunc lessthanS32
#endif
#ifdef FLOATTYPE
		typedef float type;
#define lessthanfunc lessthanfloat
#endif
	#if 1
		// all different
		vector<type> shuflist(100);
		U32 n=shuflist.size();
		for (i=0;i<n;++i)
			shuflist[i] = type(i);
		for (i=0;i<testnum;++i) {
			random_shuffle(shuflist.begin(),shuflist.end(),mt_random);
			median<type> so(shuflist);
			allpass = allpass && so.testmedian("shuflist",i);
		}
	#endif
	#if 1
		// some the same
		vector<type> randlist(1000);
		const U32 maxrandsize = 1000;
		for (U32 k=1;k<=maxrandsize;k+=500) {
			U32 j,n = randlist.size();
			for (j=0;j<testnum;++j) {
				for (i=0;i<n;++i) {
					randlist[i] = type(mt_random(k));
				}
				median<type> so(randlist);
				allpass = allpass && so.testmedian("randlist",i);
			}
		}
	#endif
		if (allpass) {
			logger("everthing passes !!!!\n");
		}
	}
};
