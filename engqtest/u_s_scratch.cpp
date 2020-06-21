// simple tests and one time utils
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"
#include <tinyxml.h>
#include "math_abstract.h"

#include "m_perf.h"

//#define RENAMESHADERS // never call this code again !!!
//#define PERSORTHO // perspective test with y,z to ys,zp and w
//#define HAIL1 // basic updown and counts
//#define HAIL2 // find loops
//#define HAIL3 // solve 2^M - 1 = 3^N
//#define GHZ
//#define WINE_ENTANGLE
//#define DEBUGTEST
//#define CUBIC_R6_TEST
//#define TESTBIRDS
//#define TESTAUTOMORPH
//#define SUMCUBES
//#define UITEST // square ui test 0 - 1024 to 0 - 1024
//#define LISTTEST
//#define HEAPTEST
//#define MEDIANTEST
//#define GROVERTEST
//#define VECTORDESTROYTEST
//#define QUATTESTQ
//#define FUNCTOR // compare strings and sort by column
//#define FUNCTOR2 // float to float
//#define TEMPLATES
//#define TESTSTATIC
//#define TESTGHZ
//#define TESTROUND
//#define TESTTOBLOCHANGLEAXIS
//#define SMALLTEST
//#define MARKOV
//#define ETEST // 2.718 etc.
//#define SPINNERS // 3 non transitive spinners with numbers 1 to 6 each with 1 or 2 numbers
#ifdef SPINNERS
#include "u_spinners.h"
#endif

// C++11 only
#define JOBTEST

//#include "modernCppTests/modernCppTests.cpp"

#ifdef JOBTEST
//#define CIRCULARTEST
//#define STRTOKTEST
//#define LEIATEST1 // task cooldown
//#define LEIATEST2 // median class
//#define LEIATEST3 // Strobogrammatic number
//#define ZIGZAG
//#define REVERSELINKLIST
//#define CYCLICCOPY
//#define CODERPAD
//#define PERMUTATION_ITERATOR
//#define ODD_ONE_OUT
#define INTERVALS

#ifdef LISTTEST
#include "u_listtest.h"
#endif

#ifdef PERMUTATION_ITERATOR
#include "u_permiter.h"
#endif

#ifdef ODD_ONE_OUT
#include "u_oddOneOut.h"
#endif

#endif

#ifdef CODERPAD
void testCoderpad()
{
	logger("in coderpad\n");
	// from coderpad.io
	const C8* words[] = { "Hello, ", "World!", ", Again!!" };
	for (const C8* word : words) {
		cout << word;
		logger(word);
	}
	cout << endl;
	logger("\n");
	logger("num elements = %d\n", NUMELEMENTS(words));
}
#endif

// PERMITERATOR was here, now in it's own module

#ifdef LEIATEST1
/*Given a char array representing tasks CPU need to do.

It contains capital letters A to Z where different letters represent different tasks.
Tasks could be done without original order.
Each task could be done in one interval.
For each interval, CPU could finish one task or just be Idle.

However, there is a non - negative cooling interval n that means between two same tasks, there must
be at least n intervals that CPU are doing different tasks or just be Idle.

You need to return the least number of intervals the CPU will take to finish all the given tasks.

Example:

	***
	[A A A B B B] n = 2
	[A B I A B I A B] Output : 8

	* **
	[A A A] n = 0
	[A A A] Output : 3

	***
	[A A A] n = 1
    [A I A I A] Output : 5

	***
	[A A A] n = 2
	[A I I A I I A] Output : 7

	***
	[A A A] n = 3
	[A I I I A I I I A] Output : 9

	***
	[A A A A A A B B B B B C C C C] n = 2    A:6 B:5 C:4
	[A B C A B C A B C A B C A B I A] Output : 16

	***
	[A B B C C C D] n = 2    A:1 B:2 C:3 D:1
	[C B A C B D C] Output : 7
*/

// BONUS: return a string with the tasks re-ordered, number of tasks is the length of the string

//--------- some code ---------
struct MapEle {
	U32 count; // how many of this type of task left
	C8 letter; // keep track of letter A,B etc.
};

// compare for sort
bool compMapEle(const MapEle& a, const MapEle& b)
{
	S32 diffCount = a.count - b.count;
	if (diffCount == 0) {
		return a.letter < b.letter; // let lower letters go first if tie on the counts
	}
	return diffCount > 0; // high count to low count
}

string leastInterval(const C8* tasks, U32 interval)
{
	//U32 time = 0; // now returning string of ordered tasks
	string ret;
	// build up a histogram of tasks
	vector<MapEle> hist(26);
	for (const C8* cp = tasks; *cp; ++cp) {
		C8 c = *cp;
		U32 cm = c - 'A';
		MapEle& me = hist[cm];
		me.count++;
		me.letter = c;
	}
	// put most common tasks up front
	sort(hist.begin(), hist.end(), compMapEle);
	while (hist[0].count > 0) { // anything left ?
		U32 i = 0;
		while (i <= interval) { // iterate 1 more than interval, that's enough to keep same tasks apart
			if (hist[0].count == 0)
				break; // anything left ?
			if (i < 26 && hist[i].count > 0) { // add a different task
				hist[i].count--;
				ret += hist[i].letter;
			} else { // no more different tasks, add an idle
				ret += ".";
			}
			//time++;
			i++;
		}
		// update put most common tasks up front
		sort(hist.begin(), hist.end(), compMapEle);
	}
	return ret;
}

// unit test for leastInterval
void testLeastInterval()
{
	logger("\ntestLeastInterval\n");
	struct testEle {
		const C8* str; // string of tasks to do
		U32 coolTime; // how long to wait to do same task again
	};
	const testEle problist[] = {
		// string, cooldown time
		{"AAABBB",2},
		{"AAA",0},
		{"AAA",1},
		{"AAA",2},
		{"AAA",3},
		{"AAAAAABBBBBCCCC",0},	// A : 6 B : 5 C : 4
		{"AAAAAABBBBBCCCC",1},	// A : 6 B : 5 C : 4
		{"AAAAAABBBBBCCCC",2},	// A : 6 B : 5 C : 4
		{"AAAAAABBBBBCCCC",3},	// A : 6 B : 5 C : 4
		{"ABBCCCD",2},			// A : 1 B : 2 C : 3 D : 1
		{"AAABCDE",2},
	};
	const U32 numproblist = NUMELEMENTS(problist);
	for (U32 i = 0; i < numproblist; ++i) {
		const testEle& prob = problist[i];
		const C8* tasks = prob.str;
		const U32 n = prob.coolTime;
		const string ret = leastInterval(tasks, n);
		logger("running tasks '%s' with cooldown time of %d, sequence '%s', num cycles %d\n", tasks, n, ret.c_str(),ret.size());
	}
}

/*
	// some more code
	int hist[26];
	for int val in array
		val = val - 'A'
		hist[val]++;

	int n = 2;
	int curletter = 0;
	int ntasks = n;
	while (curletter < 26) {
		if (!hist[curletter) {
			curletter++;
			continue;
		}
		output curletter + 'A';
		hist[curletter]--;
		++curletter;
		--ntasks;
		if (ntasks <= 0)
			output I;

		// TODO handle I

	}
*/
#endif
#ifdef LEIATEST2
// median class

// heap class first
class myHeap {
	bool isMin; // default is maxheap
	vector<float> heapData;
	// if isMin == true then normal, isMin false then reverse (max)
// TODO: use function pointers
	bool gt(float a, float b)
	{
		if (isMin)
			return a > b;
		else
			return a < b;
	}
	bool lt(float a, float b)
	{
		if (isMin)
			return a < b;
		else
			return a > b;
	}
	bool gte(float a, float b)
	{
		if (isMin)
			return a >= b;
		else
			return a <= b;
	}
	bool lte(float a, float b)
	{
		if (isMin)
			return a <= b;
		else
			return a >= b;
	}
public:
	myHeap(bool im = false) : isMin{ im } {}
	float peek() { return heapData.front(); }
	void push(float val);
	float pop();
	bool empty() { return heapData.size() == 0; }
	U32 size() { return heapData.size(); }
	void show();
};

void myHeap::push(float val)
{
	heapData.push_back(val);
	U32 idx = heapData.size() - 1;
	while (idx > 0) {
		U32 newIdx = (idx - 1) / 2;
#if 0
		if (isMin) {
			if (val >= heapData[newIdx]) {
				break;
			}
		}
		else {
			if (val <= heapData[newIdx]) {
				break;
			}
		}
#else
		if (gte(val,heapData[newIdx]))
			break;
#endif
		heapData[idx] = heapData[newIdx];
		idx = newIdx;
	}
	heapData[idx] = val;
	return;
}

// assume something to pop
float myHeap::pop()
{
	if (heapData.size() == 1) { // simple case
		float ret = heapData.front();
		heapData.clear();
		return ret;
	}
	// move last element to front after getting the front value for return
	float ret = heapData.front();
	float val = heapData.back();
	heapData.front() = val;
	heapData.pop_back();
	// now fix any heap violations
	U32 maxIdx = heapData.size() - 1;
	U32 idx = 0;
	while (idx <= maxIdx) {
		//break;
		U32 newLeftIdx = 2 * idx + 1;
		// 0 leaves
		if (newLeftIdx > maxIdx) {
			break; // done
		}
		float steerVal;
		U32 newSteerIdx;
		float leftVal = heapData[newLeftIdx];
		U32 newRightIdx = newLeftIdx + 1;
		if (newRightIdx > maxIdx) {
			// 1 left leaf
			steerVal = leftVal;
			newSteerIdx = newLeftIdx;
			//break;
		} else {
			// 2 leaves
			float rightVal = heapData[newRightIdx];
			// steer left or right
			if (lte(rightVal, leftVal)) {
				//if (rightVal >= leftVal) {
				steerVal = rightVal; // favor right vals in a tie (shorter)
				newSteerIdx = newRightIdx;
			}
			else {
				steerVal = leftVal;
				newSteerIdx = newLeftIdx;
			}
		}
		// deeper ?
		if (lte(val,steerVal)) {
		//if (val >= steerVal) {
			break; // done
		}
		heapData[idx] = heapData[newSteerIdx];
		idx = newSteerIdx;
	}
	heapData[idx] = val;
	return ret;
}

void myHeap::show()
{
	cout << "show myHeap : ";
	for (auto v : heapData)
		cout << " " << v;
	cout << "\n";
}

void myHeapSort(vector<float> &arr,bool isMin = false)
{
	myHeap sh(isMin); // sort heap;
	for (auto v : arr)
		sh.push(v);
	arr.clear();
	while (!sh.empty())
		arr.push_back(sh.pop());
}



bool cmpMin(float a, float b)
{
	return a > b;
}

class myMedian {
	vector<float> maxHeap;
	vector<float> minHeap;
public:
	float addAndGet(float v);
};

float myMedian::addAndGet(float v)
{
	perf_start(MEDIAN_FAST);
	if (maxHeap.empty()) {
		maxHeap.push_back(v);
		perf_end(MEDIAN_FAST);
		return v;
	}
	if (v >= maxHeap.front()) {
		minHeap.push_back(v); push_heap(begin(minHeap), end(minHeap), cmpMin);
	}
	else {
		maxHeap.push_back(v); push_heap(begin(maxHeap), end(maxHeap));
	}
	U32 maxHeapSize = maxHeap.size();
	U32 minHeapSize = minHeap.size();
	S32 del = maxHeapSize - minHeapSize;
	if (del == 2) {
		auto v = maxHeap.front();
		pop_heap(begin(maxHeap), end(maxHeap)); maxHeap.pop_back();
		minHeap.push_back(v); push_heap(begin(minHeap), end(minHeap), cmpMin);
		del = 0;
	}
	else if (del == -2) {
		auto v = minHeap.front();
		pop_heap(begin(minHeap), end(minHeap), cmpMin); minHeap.pop_back();
		maxHeap.push_back(v); push_heap(begin(maxHeap), end(maxHeap));
		del = 0;
	}
	if (del == 0) {
		perf_end(MEDIAN_FAST);
		return (maxHeap.front() + minHeap.front()) / 2;
	}
	else if (del == 1) {
		perf_end(MEDIAN_FAST);
		return maxHeap.front();
	}
	else { // del == -1
		perf_end(MEDIAN_FAST);
		return minHeap.front();
	}
}

class myMedianMyHeap {
	//vector<float> maxHeap;
	//vector<float> minHeap;
	myHeap maxheap;
	myHeap minheap{ 1 };

public:
	float addAndGet(float val);
};

float myMedianMyHeap::addAndGet(float val)
{
	perf_start(MEDIAN_MYHEAP);
	if (maxheap.empty()) {
		maxheap.push(val);
		perf_end(MEDIAN_MYHEAP);
		return val;
	}
	if (val >= maxheap.peek()) {
		minheap.push(val);
	}
	else {
		maxheap.push(val);
	}
	U32 maxHeapSize = maxheap.size();
	U32 minHeapSize = minheap.size();
	S32 del = maxHeapSize - minHeapSize;
	if (del == 2) {
		auto val = maxheap.pop();
		minheap.push(val);
		del = 0;
	}
	else if (del == -2) {
		auto val = minheap.pop();
		maxheap.push(val);
		del = 0;
	}
	if (del == 0) {
		perf_end(MEDIAN_MYHEAP);
		return (maxheap.peek() + minheap.peek()) / 2;
	}
	else if (del == 1) {
		perf_end(MEDIAN_MYHEAP);
		return maxheap.peek();
	}
	else { // del == -1
		perf_end(MEDIAN_MYHEAP);
		return minheap.peek();
	}
}

class myMedianSlow {
	vector<float> data;
public:
	float addAndGet(float val);
};

float myMedianSlow::addAndGet(float val)
{
	perf_start(MEDIAN_SLOW);
	data.push_back(val);
	sort(begin(data), end(data));
	U32 ds = data.size();
	if (ds & 1) {
		perf_end(MEDIAN_SLOW);
		return data[ds/2];
	} else {
		U32 h = ds / 2;
		perf_end(MEDIAN_SLOW);
		return (data[h-1] + data[h]) / 2;
	}
}

void testMedianClass()
{
	cout << "\nin testMedianClass\n";
#define TESTMYHEAP
#define TESTMEDIAN

#ifdef TESTMYHEAP
// test myHeap first
	cout << "\nfirst test myHeap\n";
	vector <vector<float> > testsHeap = {
		vector<float> {10,9,6},
		vector<float> {6, 10, 9 , 7 ,8 , 2 , 22 , 4, 4, 4, 7, 19, 2},
		vector<float> {-6, -10, -9 , -7 ,-8 , -2 , -22 , -4, -4, -4, -7, -19, -2},
		vector<float> {2,3,4,13,12,11,6,7,8},
		vector<float> {6,6,6,6,7,7,5,5,5,5,5,5,5},
		vector<float> {5, 3, 4, 4, 3, 3, 3, 5, 5, 5, 5},
	};
	for (auto tst : testsHeap) {
		cout << " test a simple myheap run\n";
		myHeap mh;
#define TESTMAX
#define TESTMIN
#ifdef TESTMAX
		for (auto val : tst) {
			mh.push(val);
		}
		cout << " max ";
		while (!mh.empty()) {
			cout << " " << mh.pop();
		}
		cout << "\n";
#endif
#ifdef TESTMIN
		mh = myHeap{ true };
		for (auto val : tst) {
			mh.push(val);
		}
		cout << " min ";
		// special test
		while (!mh.empty()) {
			//if (mh.size() == 3)
			//	cout << "\nsize = 3\n";
			cout << " " << mh.pop();
		}
		cout << "\n";
#endif
	}

#endif //TESTMYHEAP

#ifdef TESTMEDIAN

	// now test median class

	cout << "\nnow test Medians";

	vector <vector<float> > testsMed = {
		vector<float> {2,3,4,13,12,11,5,6,7},
		vector<float> {6,6,6,6,7,7,5,5,5,5,5,5,5},
		vector<float> {6, 10, 9 , 7 ,8 , 2 , 22 , 4, 4, 4, 7, 19, 2},
		vector<float> {5, 3, 4, 4, 3, 3, 3, 5, 5, 5, 5},
	};
	cout << "\nsimple tests\n";
	for (auto tst : testsMed) {
		cout << " test a simple myMedian run\n";
		myMedian med;
		for (auto val : tst) {
			cout << " " << med.addAndGet(val);
		}
		cout << "\n";
		cout << " test a simple myMedian myHeap run\n";
		myMedianMyHeap medmh;
		for (auto val : tst) {
			cout << " " << medmh.addAndGet(val);
		}
		cout << "\n";
	}
#define PROCEDURAL
#ifdef PROCEDURAL
	cout << "more procedural tests\n";
	const U32 RUNS = 4000;
	const U32 MINSIZE = 20;
	const U32 MAXSIZE = 200;
	const S32 MINVAL = 3;
	const S32 MAXVAL = 60;
	bool pass = true;
	for (U32 i = 0; i < RUNS; ++i) {
		const U32 testSize = mt_random(MAXSIZE - MINSIZE + 1) + MINSIZE;
		//cout << " testSize = " << testSize << "\n";
		vector<float> testVec;
		for (U32 j = 0; j < testSize; ++j) {
			U32 val = mt_random(MAXVAL - MINVAL + 1) + MINVAL;
			testVec.push_back(static_cast<float>(val));
		}
		vector<float> hp;
		//cout << " test a procedural run\n";
		// push and pop
		myMedian med; // use std C++ library O(lg(n))
		myMedianMyHeap medMyHeap; // use myHeap O(lg(n)
		myMedianSlow medSlow; // sort then pick median O(n*lg(n)), slow way
		for (auto val : testVec) {
			auto medRet = med.addAndGet(val);
			auto medMyHeapRet = medMyHeap.addAndGet(val);
			auto medSlowRet = medSlow.addAndGet(val);
			if (medRet != medMyHeapRet) {
				cout << " diff found fast heap: " << medRet << " my heap: " << medMyHeapRet << "\n";
				pass = false;
			}
			if (medRet != medSlowRet) {
				cout << " diff found fast heap: " << medRet << " slow: " << medSlowRet << "\n";
				pass = false;
			}
		}
	}
	if (pass)
		cout << "passed!\n";
#endif
#endif //TESTMEDIAN

}
#endif
#ifdef LEIATEST3

vector<int> getDigits(S32 v)
{
	vector<int> ret;
	while (v >= 10) {
		int r = v % 10;
		ret.push_back(r);
		v = v / 10;
	}
	ret.push_back(v);
	return ret;
}

// this is the meat of the problem, the rest is a framework
bool checkFlip180(S32 v)
{
	const int midTable[] = { 0,1,-1,-1,-1,-1,-1,-1,8,-1};
	const int otherTable[] = { 0,1,-1,-1,-1,-1,9,-1,8,6 };
	auto digs = getDigits(v);
	int f = 0;
	int b = digs.size() - 1;
	while (f < b) {
		if (otherTable[digs[f]] != digs[b])
			return false;
		++f;
		--b;
	}
	if (f == b) {
		if (midTable[digs[f]] != digs[b])
			return false;
	}
	return true;
}

void stroboGrammatic()
{
	cout << "\nstroboGrammatic\n";
	const S32 nmin = 1; // lower bound of digits, 1 or more
	const S32 nmax = 4; // upper bound of digits
	for (S32 n = nmin; n <= nmax; ++n) {
		S32 lo = (S32)pow(10,n-1);
		S32 hi = 10 * lo;
		if (lo == 1)
			lo = 0; // include 0 for 1 digit
		cout << "check180 for " << n << " digits:  from " << lo << " to " << hi - 1 << "\n";
		for (S32 v = lo; v < hi; ++v) {
			if (checkFlip180(v)) {
				cout << "v = " << v << "\n";
			}
		}
	}
}
#endif

#ifdef ZIGZAG
class zigzag {
private:
	typedef list<S32>::const_iterator SIT;
	struct intlist{
		SIT it;
		SIT end; // why do we need this when it should be detected from the interator
	};
	list< intlist> lci; // a list of iterators of a each S32 list
public:
	zigzag(const list<list<S32>>& la);
	S32 next();
	bool hasNext();
};

zigzag::zigzag(const list<list<S32>>& la)
{
	list<S32> tl = { 3, 4, 5 };
	for (SIT tli = tl.begin(); tli != tl.end(); ++tli) {
		logger("val = %d\n", *tli);
	}
	for (const list<S32>& lcci : la) {
		if (lcci.size()) {
			SIT pv = lcci.begin();
			SIT ev = lcci.end();
			intlist il = { pv,ev };
			lci.push_back(il);
		}
	}
	logger("iterators loaded\n");
	logger("show iterators\n");
	for (auto li : lci) {
		logger("first elements = %d\n", *li.it);
	}
	logger("done show iterators\n");
}

S32 zigzag::next()
{
	intlist sit = lci.front();
	SIT en = sit.end;
	S32 ret = *sit.it++;
	lci.pop_front();
	if (sit.it != en) {
		lci.push_back(sit);
	}
	return ret;
}

bool zigzag::hasNext()
{
	return lci.size() != 0;
}

void testZigzag()
{
	logger("START testZigzag\n");
	list<S32> allm1{ 47 };
	list<S32> all0{ 1, 3, 2, 4, 7, 6 };
	list<S32> all1{ 11,7,6 };
	list<S32> all2{ 101,3,2,4,7,6,8,10,16 };
	list<S32> all3{  };
	list<S32> all4{ 111,333,222,444,790,322,22431,1234 };
	list<S32> all5{ 1111,3333 };
	list<list<S32>> llall{ allm1,all0,all1,all2,all3,all4,all5 };
	zigzag* zzp = new zigzag(llall);
	while (zzp->hasNext()) {
		S32 ele = zzp->next();
		logger("ele = %d\n", ele);
	}
	delete zzp;
	logger("do again\n");
	zzp = new zigzag(llall);
	while (zzp->hasNext()) {
		S32 ele = zzp->next();
		logger("ele = %d\n", ele);
	}
	delete zzp;
	logger("END testZigzag\n");
}
#endif

#ifdef INTERVALS
//Given a collection of intervals(inclusive), merge all overlapping intervals.
//Example: Input[[8, 10], [1, 4], [3, 6], [15, 18]] Return[[8, 10], [1, 6], [15, 18]]

struct interval {
	int start, end;
};

// TODO: try a lambda, done!
#if 0
bool compareIntervals(const interval& a, const interval& b)
{
	return a.start < b.start;
}
#endif

vector<interval> mergeIntervals(const vector<interval> & inputIntervals)
{
	vector<interval> output;
	if (inputIntervals.empty())
		return output;
	vector<interval> sortedIntervals = inputIntervals;
	sort(sortedIntervals.begin(), sortedIntervals.end(), 
		[] (const interval& a, const interval& b)
	{
		return a.start < b.start;
	});
	interval working = sortedIntervals[0];
	for (auto i = 1U; i < sortedIntervals.size(); ++i) {
		const interval& intv = sortedIntervals[i];
		// check for zero size interval
		if (intv.start == intv.end)
			continue;
		if (intv.start <= working.end) { // next interval is inside first one
			// merge overlapped intervals
			if (intv.end > working.end)
				working.end = intv.end;
		} else {
			// add to list
			output.push_back(working);
			working = sortedIntervals[i];
		}

	}
	if (working.start != working.end)
		output.push_back(working);
	return output;
}

void printIntervals(const string& label, const vector<interval>& intv)
{
	logger("interval '%s'\n", label.c_str());
	for (const auto &v : intv) {
		logger("\t[ %3d, %3d]\n", v.start, v.end);
	}
}

void testIntervals()
{
	logger("START testIntervals\n");
	const vector<vector<interval>> intvs {
		{
			// empty list
		},
		{ // one
			{50,65},
		},
		{ // two
			{5,6}, {3,4}
		},
		{ // another two
			{10,20}, {5,15}
		},
		{ // another two
			{20,30}, {14,34}
		},
		{ // example
			{8,10}, {1,4}, {3,6}, {15,18}
		},
		{ // complex
			{8,14}, {25,27}, {19,21}, {12,17}, {7,10}, {0,1}, {2,6}, {0,1}, {18,22}, {28,31}, {0,1}, {24,26}, {29,30}, {23,25}, {11,13}, {3,5}, {31,32}
		}
	};
	for (auto intv : intvs) {
		const vector<interval> intvOut = mergeIntervals(intv);
		printIntervals("input interval", intv);
		printIntervals("output interval", intvOut);
		logger("---------------------------------------\n");
	}
	logger("END testIntervals\n");
}

#endif

//#ifdef JOBTEST

#ifdef CYCLICCOPY
//////////////// play with cyclic graphs
struct CyclicGraph {
	CyclicGraph(float vala = 0) : val(vala) {}
	float val;
	vector<CyclicGraph*> nodes;
/*	CyclicGraph::~CyclicGraph()
	{
		logger("CyclicGraph destructor\n");
	} */
};

// static stuff

// local allocation of CyclicGraph objects
const S32 MAXCPOOL = 30;
S32 curcpool = 0;
CyclicGraph cpool[MAXCPOOL];

// std::map for copying and printing
#include <unordered_map>
#include <unordered_set>
//unordered_map<const CyclicGraph*, CyclicGraph*> oldNewMap;
//unordered_set<const CyclicGraph*> visited;

// end static stuff

// thread safe
CyclicGraph* newCyclicGraph()
{
	if (curcpool >= MAXCPOOL)
		errorexit("too many CyclicGraph!!");
	CyclicGraph* ret = cpool + curcpool++;
	ret->nodes.clear();
	return ret;
}

// thread safe
void resetCyclicGraphPool()
{
	logger("resetCyclicGraphPool: from %d to 0\n", curcpool);
	curcpool = 0;
}

void printCyclicGraphRec(const CyclicGraph* cg, unordered_set<const CyclicGraph*>& visited)
{
	auto ret = visited.find(cg);
	if (ret == visited.end()) {
		visited.insert(cg);
		logger("\tval = %f\n", cg->val);
		for (auto i : cg->nodes) {
			logger("\t\tdest = %f\n", i->val);
		}
		for (auto i : cg->nodes) {
			printCyclicGraphRec(i,visited);
		}
	}
}

void printCyclicGraph(const CyclicGraph* cg)
{
	unordered_set<const CyclicGraph*> visited;
	logger("START print cyclic graph\n");
	if (!cg) {
		logger_indent();
		logger("null\n");
		logger_unindent();
		logger("END print cyclic graph\n\n");
		return;
	}
	printCyclicGraphRec(cg,visited);
	logger("END print cyclic graph\n\n");
}

void copyCyclicGraphRec1(const CyclicGraph* cg, unordered_map<const CyclicGraph*, CyclicGraph*>& oldNewMap)
{
	if (oldNewMap.find(cg) != oldNewMap.end())
		return;
	CyclicGraph* ncg = newCyclicGraph();
	ncg->val = cg->val * 11;
	oldNewMap[cg] = ncg;
	for (auto i : cg->nodes) {
		copyCyclicGraphRec1(i,oldNewMap);
	}
}

void copyCyclicGraphPass2(unordered_map<const CyclicGraph*, CyclicGraph*>& oldNewMap)
{
	for (auto  i : oldNewMap) {
		auto f = i.first;
		auto s = i.second;
		for (auto j : f->nodes) {
			auto nj = oldNewMap[j];
			s->nodes.push_back(nj);
		}
	}
}

CyclicGraph* copyCyclicGraph(const CyclicGraph* cg)
{
	if (!cg)
		return nullptr;
	unordered_map<const CyclicGraph*, CyclicGraph*> oldNewMap;
	copyCyclicGraphRec1(cg,oldNewMap);
	copyCyclicGraphPass2(oldNewMap);
	auto ret = oldNewMap[cg];
	return ret;
 }

void testCopyCyclicGraph()
{
	//resetCyclicGraphPool();
	// build A graph
/*	== visual representation ==
		1 -> 2 -> 3 ->
		^    ^    ^   \
		\----\----\-- - 4

		== connections ==
		1 -> 2
		2 -> 3
		3 -> 4
		4 -> 1, 2, 3 */
	CyclicGraph a_1 = CyclicGraph(1);
	CyclicGraph a_2 = CyclicGraph(2);
	CyclicGraph a_3 = CyclicGraph(3);
	CyclicGraph a_4 = CyclicGraph(4);
	a_1.nodes.push_back(&a_2);
	a_2.nodes.push_back(&a_3);
	a_3.nodes.push_back(&a_4);
	a_4.nodes.push_back(&a_1);
	a_4.nodes.push_back(&a_2);
	a_4.nodes.push_back(&a_3);

	// build B graph
	CyclicGraph b_1 = CyclicGraph(1);
	CyclicGraph b_2 = CyclicGraph(2);
	CyclicGraph b_3 = CyclicGraph(3);
	CyclicGraph b_4 = CyclicGraph(4);
	CyclicGraph b_5 = CyclicGraph(5);
	CyclicGraph b_6 = CyclicGraph(6);
	CyclicGraph b_7 = CyclicGraph(7);
	CyclicGraph b_8 = CyclicGraph(8);
	b_1.nodes.push_back(&b_2);
	b_2.nodes.push_back(&b_6);
	b_2.nodes.push_back(&b_3);
	b_3.nodes.push_back(&b_2);
	b_3.nodes.push_back(&b_4);
	b_4.nodes.push_back(&b_3);
	b_5.nodes.push_back(&b_6);
	b_6.nodes.push_back(&b_8);
	b_6.nodes.push_back(&b_7);
	b_7.nodes.push_back(&b_8);
	b_8.nodes.push_back(&b_5);
	b_8.nodes.push_back(&b_7);

	CyclicGraph c_1 = CyclicGraph(1);
	CyclicGraph c_2 = CyclicGraph(2);
	c_1.nodes.push_back(&c_2);
	c_2.nodes.push_back(&c_1);

	CyclicGraph d_1 = CyclicGraph(1);
	CyclicGraph d_2 = CyclicGraph(2);
	d_1.nodes.push_back(&d_2);

	CyclicGraph e_1 = CyclicGraph(1);

	logger("print cyclic graphs\n");

	printCyclicGraph(nullptr);
	printCyclicGraph(&a_1);
#if 1
	printCyclicGraph(&b_1);
	printCyclicGraph(&c_1);
	printCyclicGraph(&d_1);
	printCyclicGraph(&e_1);

	logger("print cyclic graphics copies\n");
	 CyclicGraph* copy_nul = copyCyclicGraph(nullptr);
	 CyclicGraph* copya_1 = copyCyclicGraph(&a_1);
	 CyclicGraph* copyb_1 = copyCyclicGraph(&b_1);
	 CyclicGraph* copyc_1 = copyCyclicGraph(&c_1);
	 CyclicGraph* copyd_1 = copyCyclicGraph(&d_1);
	 CyclicGraph* copye_1 = copyCyclicGraph(&e_1);
	printCyclicGraph(copy_nul);
	printCyclicGraph(copya_1);
	printCyclicGraph(copyb_1);
	printCyclicGraph(copyc_1);
	printCyclicGraph(copyd_1);
	printCyclicGraph(copye_1);
#endif
	resetCyclicGraphPool();
}
#endif // CYCLICCOPY

#ifdef REVERSELINKLIST
/////////////// play with linked lists
struct LinkedList {
	float val;
	LinkedList* next;
};

void printLinkedList(const LinkedList* ll)
{
	logger("START print linked list\n");
	while (ll) {
		logger("\tval = %f\n", ll->val);
		ll = ll->next;
	}
	logger("END print linked list\n\n");
}

LinkedList* reverseLinkedList(LinkedList* ll)
{
	if (!ll || !ll->next)
		return ll; // size 0 or 1, do nothing
	LinkedList* n = ll->next; // n = 'the rest'
	ll->next = nullptr; // ll was first, now last, null next now
	LinkedList* ret = reverseLinkedList(n); // 'the rest' now in front, return this, n is now the last element of 'the rest'
	n->next = ll; // make that point to what was the first element
	return ret;
}

void testLinkList()
{
	LinkedList ex1a = { 3 };

	LinkedList ex2a = { 5 };
	LinkedList ex2b = { 7 };
	ex2a.next = &ex2b;

	LinkedList ex3a = { 11 };
	LinkedList ex3b = { 13 };
	LinkedList ex3c = { 17 };
	ex3a.next = &ex3b;
	ex3b.next = &ex3c;

	LinkedList ex4a = { 19 };
	LinkedList ex4b = { 23 };
	LinkedList ex4c = { 29 };
	LinkedList ex4d = { 31 };
	ex4a.next = &ex4b;
	ex4b.next = &ex4c;
	ex4c.next = &ex4d;

	printLinkedList(nullptr);
	printLinkedList(&ex1a);
	printLinkedList(&ex2a);
	printLinkedList(&ex3a);
	printLinkedList(&ex4a);

	logger("now reverse\n");
	LinkedList* rex0a = reverseLinkedList(nullptr);
	LinkedList* rex1a = reverseLinkedList(&ex1a);
	LinkedList* rex2a = reverseLinkedList(&ex2a);
	LinkedList* rex3a = reverseLinkedList(&ex3a);
	LinkedList* rex4a = reverseLinkedList(&ex4a);
	printLinkedList(rex0a);
	printLinkedList(rex1a);
	printLinkedList(rex2a);
	printLinkedList(rex3a);
	printLinkedList(rex4a);
}
#endif // REVERSELINKLIST
//#endif // JOBTEST

#ifdef CIRCULARTEST
#include "u_circularA.h"
#include "u_circularB.h"
#include "u_s_scratch.h"
void circulartest()
{
	logger("------- circular class test ------\n");
	CircularA* ca = new CircularA();
	CircularB* cb = new CircularB();
	ca->setB(cb);
	cb->setA(ca);
	ca->print();
	cb->print();
	ca->printBoth();
	cb->printBoth();
	delete ca;
	delete cb;
}
#endif

#ifdef STRTOKTEST
C8* mystrtok(C8* m, const C8* d)
{
	//return strtok(m, d);
	// nothing in delim, trival case
	if (!d) {
		if (m && *m) {
			return m; // if not empty
		}
		return nullptr;
	}

	// shortcut for d == ""
	//if (!*d)
	//	return m;

	// let's get to work
	static C8* sp;
	if (m)
		sp = m; // walk the string with several calls
	// find first non-delimiter
	while (true) {
		if (!*sp)
			return nullptr; // done
		// is char a delim or not
		const C8* dp = d;
		C8 cv = *sp;
		C8 dv;
		while (true) {
			dv = *dp++;
			if (!dv)
				break; // couldn't find delimiter, must be main character
			if (cv == dv)
				break; // found delimiter
		}
		if (dv) { // char is a delim char
			++sp;
		} else {
			break;
		}
	}
	C8* ret = sp; // this is what we'll return, first non-delim char
	// find first delimiter
	while (true) {
		if (!*sp)
			return ret; // done
		// is char a non-delimiter or not
		const C8* dp = d;
		C8 cv = *sp;
		C8 dv;
		while (true) {
			dv = *dp++;
			if (!dv)
				break; // couldn't find delimiter, must be main character
			if (dv == cv)
				break; // found delimiter
		}
		if (!dv) { // char is a non-delim char
			++sp;
		} else {
			*sp = '\0'; // c_str
			++sp;
			break; //
		}
	}
	return ret;
}

static C8* strclone(const C8* orig)
{
	C8* r = new C8[strlen(orig) + 1];
	strcpy(r, orig);
	return r;
}


void studystrtok(const C8* m, const C8* d)
{
	logger("in studystrtok with main = '%s' and delim = '%s'\n", m, d);
	C8* cln = strclone(m);
	C8* nxt = cln;
#if 1
	while (true) {
		C8* tok = mystrtok(nxt, d);
		if (!tok && nxt) // null right at the start
			logger("\ttok NULL\n");
		if (!tok)
			break;
		nxt = nullptr;
		logger("\ttok = '%s'\n", tok);
	}
	delete[] cln;
#endif
}

void strtoktest()
{
	logger("------ strtok test ------\n");
/*	C8* mainStr = "Hi,This is,a test";
	C8* mainStr2 = " ,H , , W  ,";
	C8* delimStr = ", ";
	C8* delimStr2 = "i";

	struct test {
		C8* main;
		C8* delim;
	};

	struct test tests[] = {
		{mainStr,delimStr},
		{mainStr2,delimStr},
		{mainStr,delimStr2},
		{mainStr2,delimStr2},
	};

	for (test t : tests) {
		studystrtok(t.main, t.delim);
	} */

	const C8* mainStrs[] = { "hi","ho","Hi,This is,a test"," ,H , , W  ,",""/*,nullptr*/}; // cannot pass nullptr, BAD
	const C8* delimStrs[] = { "del","what","is","abchi",", " ,"i" ,"",nullptr};
	for (auto ms : mainStrs) {
		logger("-------- next main string --------\n");
		for (const auto ds : delimStrs) {
			studystrtok(ms, ds);
		}
	}

#if 0
	C8* mainStr = "Hi,This is,a test";
	C8* mainStr2 = " ,H , , W  ,";
	C8* delimStr = ", ";
	C8* delimStr2 = "i";
#endif
}
#endif

#ifdef ETEST

double factorial(double n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

double floatCombo(double n, double k)
{
	return factorial(n) / (factorial(k)*factorial(n - k));
}

void doEtest()
{
	logger("doing E test\n");
	setfpuroundprec(FPUCW_RND_NEAR, FPUCW_PREC_80); // non default super high precision
	// test binomial vs factorial terms to the limit
	const S32 MAXTERMS{ 10 };
	const S32 MAXEXP{ 100 };

	// first show factorials
	double sum = 0;
	logger("showing factorial terms\n");
	for (auto i = 0; i < MAXTERMS; ++i) {
		double term = 1.0f / factorial(double(i));
		logger("%9.5g ", term);
		sum += term;
	}
	logger("\nsum = %f\n",sum);

	// now show binomial expansion of (1+1/u)^u for increasing u
	for (auto j = 0; j <= MAXEXP; ++j) {
		logger("\nshowing binomial power %d\n", j);
		sum = 0;
		for (auto i = 0; i < min(MAXTERMS,j+1); ++i) {
			double p = pow(double(j),double(i));
			//logger("%f ", p);
			double c = floatCombo(double(j), double(i));
			//logger("%g ", c);
			//logger("%g ", invTerm);
			//sum += invTerm;
			double term = c / p;
			logger("%9.5g ", term);
			sum += term;
		}
		logger("\nsum = %f\n", sum);
	}
	defaultfpucontrol(); // default low precision
}
#endif

#ifdef RENAMESHADERS
// rename webgl shaders to be more like android shaders
void renameShaders()
{
	logger("in rename shaders...\n");
	pushandsetdir("c:/nodejs/engw/engw3dtest/shaders");
	script sc = scriptdir(0); // just files
	string from, to;
	bool isPS = false;
	S32 i, n = sc.num();
	bool found = false;
	for (i = 0; i < n; ++i) {
		found = false;
		from = sc.idx(i);
		S32 idx = from.rfind(".vs");
		if (idx != string::npos) {
			isPS = false;
			//logger("VS\n");
			to = from.substr(0, idx);
			to += ".vert.glsl";
			found = true;
		} else {
			idx = from.rfind(".ps");
			if (idx != string::npos) {
				isPS = true;
				//logger("PS\n");
				to = from.substr(0, idx);
				to += ".frag.glsl";
				found = true;
			}
		}
		if (found) {
			logger(from.c_str());
			logger("\n");
			logger(to.c_str());
			logger("\n");
			C8* shaderData = fileload(from.c_str());
			filesave(to.c_str(), shaderData);
			delete[] shaderData;
		}
	}

	popdir();
}
#endif

#ifdef PERSORTHO
namespace pers {

	// for debvars

	// inputs
	S32 D3D = 0; // OGL 0, D3D 1
	S32 ORTHO = 1; // PERS 0, ORTHO 1
	float Y = 2;
	float Z = 6;
	float N = 5;
	float F = 7;
	float D = 4; // focal length for pers, zoom for both pers and ortho
	// outputs
	float YS;
	float ZS;
	float WS;
	float YSOW;
	float ZSOW;

	struct menuvar persdv[] = {
		{"@yellow@--- PERS ORTHO USER VARS ---",NULL,D_VOID,0},
		// inputs
		{"D3D", &D3D, D_INT},
		{"ORTHO", &ORTHO, D_INT},
		{"Y", &Y, D_FLOAT, FLOATUP},
		{"Z", &Z, D_FLOAT, FLOATUP},
		{"N", &N, D_FLOAT, FLOATUP},
		{"F", &F, D_FLOAT, FLOATUP},
		{"D", &D, D_FLOAT, FLOATUP},
		// outputs
		{"YS",&YS,D_FLOAT | D_RDONLY},
		{"ZS",&ZS,D_FLOAT | D_RDONLY},
		{"WS",&WS,D_FLOAT | D_RDONLY},
		{"YSOW",&YSOW,D_FLOAT | D_RDONLY},
		{"ZSOW",&ZSOW,D_FLOAT | D_RDONLY},
	};
	const int npersdv = NUMELEMENTS(persdv);

	void doPersOrtho()
	{
		D3D &= 1;
		YS = D * Y;
		if (ORTHO) {
			// no friendlies
			float b, c;
			if (D3D) {
				b = 1 / (F - N);
				c = -N / (F - N);
			}
			else { // OGL
				b = 2 / (F - N);
				//c = -2*N / (F - N) - 1;
				c = -(F + N) / (F - N);
			}
			ZS = b * Z + c;
			WS = 1; // no pers and NOT w friendly
		}
		else { // PERS, w friendly
			if (D3D)
				ZS = F / (F - N)*Z - F * N / (F - N);
			else // OGL
				ZS = (F + N) / (F - N)*Z - 2 * F*N / (F - N);
			WS = Z; // w friendly
		}
		YSOW = YS / WS;
		ZSOW = ZS / WS;
	}
}
#endif

#ifdef WINE_ENTANGLE
namespace wine {

	// for debvars

#define CLASSICAL
#define QUANTUM

#ifdef CLASSICAL
	// 4 boolean choices
	S32 cAliceGlass;
	S32 cAliceBottle;
	S32 cBobGlass;
	S32 cBobBottle;
	float cwin; // probability of picking different wines unless 2 bottles then the same
	int cGG;
	int cGB;
	int cBG;
	int cBB;
#endif
#ifdef QUANTUM
	// game
	// 4 boolean choices
	float qAliceGlass = 0;
	float qAliceBottle = 90;
	float qBobGlass = 45;
	float qBobBottle = 315;
	float qwin; // probability of picking different wines unless 2 bottles then the same
	float qGG;
	float qGB;
	float qBG;
	float qBB;
	// test
	// inputs, angle between measurement of 1/2 spin anti-correlated entangled pair
	float qang = 135;
	// outputs
	float qintensity;
	float qprob;
#endif
	struct menuvar winedv[] = {
#ifdef CLASSICAL
		{"@green@--- WINE ENTANGLE USER VARS, CLASSICAL ---",NULL,D_VOID,0},
		// inputs
		{"AliceGlass", &cAliceGlass, D_INT},
		{"AliceBottle", &cAliceBottle, D_INT},
		{"BobGlass", &cBobGlass, D_INT},
		{"BobBottle", &cBobBottle, D_INT},
		// output
		{"win",&cwin, D_FLOAT | D_RDONLY},
		{"GG",&cGG, D_INT | D_RDONLY},
		{"GB",&cGB, D_INT | D_RDONLY},
		{"BG",&cBG, D_INT | D_RDONLY},
		{"BB",&cBB, D_INT | D_RDONLY},
#endif
#ifdef QUANTUM
		{"@lightgreen@--- WINE ENTANGLE USER VARS, QUANTUM ---",NULL,D_VOID,0},
		// inputs
		{"AliceGlass", &qAliceGlass, D_FLOAT},
		{"AliceBottle", &qAliceBottle, D_FLOAT},
		{"BobGlass", &qBobGlass, D_FLOAT},
		{"BobBottle", &qBobBottle, D_FLOAT},
		// output
		{"win",&qwin, D_FLOAT | D_RDONLY},
		{"GG",&qGG, D_FLOAT | D_RDONLY},
		{"GB",&qGB, D_FLOAT | D_RDONLY},
		{"BG",&qBG, D_FLOAT | D_RDONLY},
		{"BB",&qBB, D_FLOAT | D_RDONLY},
		{"@lightgreen@--- TEST ANGLES ---",NULL,D_VOID,0},
		// inputs
		{"angle", &qang, D_FLOAT,FLOATUP*1},
		// outputs
		{"Intensity", &qintensity, D_FLOAT | D_RDONLY},
		{"Probability", &qprob, D_FLOAT | D_RDONLY},
#endif
	};
	const int nwinedv = NUMELEMENTS(winedv);

#ifdef QUANTUM
// assume spin 1/2 anti-correlated
float getIntensity(float diffAng) {
	float intensity = sin((diffAng*PIOVER180) / 2.0f);
	return intensity;
}

float getProb(float diffAng) {
	float intensity = getIntensity(diffAng);
	return intensity*intensity;
}
#endif

void doWine()
{
#ifdef CLASSICAL
		// these are all booleans, restrict to 0 or 1
		cAliceGlass &= 1;
		cAliceBottle &= 1;
		cBobGlass &= 1;
		cBobBottle &= 1;
		// 4 different cases for the waiters, 16 in all given Alice and Bob's choices
		// GG
		cGG = cAliceGlass != cBobGlass;
		// GB
		cGB = cAliceGlass != cBobBottle;
		// BG
		cBG = cAliceBottle != cBobGlass;
		// BB
		cBB = cAliceBottle == cBobBottle;
		S32 cwini = cGG + cGB + cBG + cBB;
		cwin = cwini / 4.0f; // average

#endif
#ifdef QUANTUM
		// game
		qAliceGlass = normalangdeg(qAliceGlass);
		qAliceBottle = normalangdeg(qAliceBottle);
		qBobGlass = normalangdeg(qBobGlass);
		qBobBottle = normalangdeg(qBobBottle);
		// 4 different cases for the waiters, 16 in all given Alice and Bob's choices
		// GG
		qGG = 1 - getProb(qAliceGlass - qBobGlass);
		// GB
		qGB = 1 - getProb(qAliceGlass - qBobBottle);
		// BG
		qBG = 1 - getProb(qAliceBottle - qBobGlass);
		// BB
		qBB = getProb(qAliceBottle - qBobBottle);

		qwin = qGG + qGB + qBG + qBB;
		qwin /= 4.0f; // average


		// test
		qintensity = getIntensity(qang);
		qprob = getProb(qang); // qintensity * qintensity;
#endif
	}

} // end namespace wine
#endif

#ifdef HAIL1
namespace hail1 {

	bool passEnd;
	S32 curPass; // for passEnd
	S32 largest;
	S32 maxCount;

	S32 hailStep(S32 n)
	{
		S32 ret;
		if (n & 1) {
			ret = (3 * n + 1) / 2;
		}
		else {
			ret = n / 2;
		}
		if (ret > largest)
			largest = ret;
		return ret;
	}

	bool doNext(S32 n)
	{
		const S32 PASSEND_AMOUNT = 20;
		if (passEnd) {
			return curPass < PASSEND_AMOUNT;
		}
		else {
			return n > 1;
		}
	}

	vector<S32> hailVector(S32 n) {
		vector<S32> ret;
		curPass = 0;
		while (true) {
			if (!doNext(n))
				break;
			ret.push_back(n);
			n = hailStep(n);
			++curPass;
		}
		ret.push_back(n);
		return ret;
	}

	vector<C8> hailUpDown(S32 n) {
		vector<C8> ret;
		curPass = 0;
		while (true) {
			if (!doNext(n))
				break;
			S32 next = hailStep(n);
			if (next == n) {
				ret.push_back('d');
			}
			else {
				ret.push_back(next > n ? 'U' : 'D');
			}
			n = next;
			++curPass;
		}
		return ret;
	}

	void dohail1(bool pe)
	{
		passEnd = pe;
		logger("############## start do hail 1 ##############\n");
		logger_disableindent();
		const S32 MINNUM = 0;
		const S32 MAXNUM = 128;
		largest = 0;
		maxCount = 0;
#define DOLIST
#define DOUPDOWN
#ifdef DOLIST
		logger("\nlist of numbers\n");
		for (S32 i = MINNUM; i <= MAXNUM; ++i) {
			const vector<S32> hailList = hailVector(i);
			S32 cnt = hailList.size();
			logger("n = %4d, count = %3d [", i, cnt);
			for (S32 v : hailList) {
				logger("%4d ", v);
			}
			if (hailList.size() > U32(maxCount))
				maxCount = hailList.size();
			logger("]\n");
		}
#endif
#ifdef DOUPDOWN
		logger("\nup down sequence\n");
		for (S32 i = MINNUM; i <= MAXNUM; ++i) {
			const vector<C8> upDown = hailUpDown(i);
			S32 cnt = upDown.size();
			logger("n = %4d, count = %3d [ ", i, cnt);
			for (auto v : upDown) {
				logger("%c ", v);
			}
			if (upDown.size() > U32(maxCount))
				maxCount = upDown.size();
			logger("]\n");
		}
#endif
		logger("\n");
		logger_enableindent();
		logger("############## end do hail 1 largest %d, maxcount %d ##############\n\n", largest, maxCount);
	}

} // end namespace hail1
#endif

#ifdef HAIL2
namespace hail2 {
	// look for loops
	const S32 minDigits = 1;
	const S32 maxDigits = 6;
	void dohail2()
	{
		logger("############## start do hail 2 ##############\n");
		logger_disableindent();
		logger("\n");

		logger("find loops\n");
		for (auto numDigits = minDigits; numDigits <= maxDigits; ++numDigits) {
			logger("%d digits\n", numDigits);
			//S32 N = 1 << numDigits;
			auto maxNum = (1 << numDigits);
			for (auto packed = 0; packed < maxNum; ++packed) {
				// MX + B = NX
				logger("\tj = %3d ", packed);
				for (auto k = numDigits; k < maxDigits; ++k) {
					logger(" ");
				}
				S32 M = 1;
				S32 B = 0;
				S32 N = 1;
				for (auto k = 0; k < numDigits; ++k) {
					bool up = (packed & (1 << (numDigits - k - 1))) != 0;
					if (up) {
						logger("U");
						M = 3 * M;
						B = 3 * B + N;
					}
					else {
						logger("D");
					}
					N = 2 * N;
				}
				logger(", ");
				if (M != 1) {
					logger("%3dx", M);
				}
				else {
					logger("   x");
				}
				if (B > 0) {
					logger(" + %3d = ", B);
				}
				else {
					logger("     =   ");
				}
				fraction frcSolution = fraction(B, N - M);
				S32 numerator = frcSolution.getnum();
				S32 denominator = frcSolution.getden();
				//float solution = B/float(N - M);
				if (denominator == 1) {
					logger("%3dx,    x = %d\n", N, numerator);
				}
				else {
					logger("%3dx,    x = %d/%d\n", N, numerator, denominator);
				}
			}
		}

		logger("\n");
		logger_enableindent();
		logger("############## end do hail 2 ##############\n\n");
		//poporchangestate(STATE_MAINMENU);
		popstate();
	}

} // end namespace hail2
#endif

#ifdef HAIL3
namespace hail3 {
	// solve 2^M - 1 = 3^N, M >= 0 && N >= 0
	void dohail3()
	{
		logger("############## start do hail 3 ##############\n");
		logger_disableindent();
		logger("\n");

		logger("solve: 2^M - 3^N = 1\n");
		S64 M = 0;
		S64 N = 0;
		S64 MP = 1;
		S64 NP = 1;
		S64 MAXN = numeric_limits<S64>::max() / 3;
		while (MP < MAXN && NP < MAXN) {
			S64 D = MP - NP;
			logger("M^%2lld = %19lld, N^%2lld = %19lld, difference = %20lld\n", M, MP, N, NP, D);
			if (D <= 1) {
				MP *= 2;
				++M;
			}
			else {
				NP *= 3;
				++N;
			}
		}

		logger("\n");
		logger_enableindent();
		logger("############## end do hail 3 ##############\n\n");
		popstate();
	}

} // end namespace hail3
#endif

#ifdef GHZ
namespace ghz {
	// solve GHZ classically
	const S32 NUM_DIGS{ 6 };
	bool incChoices(S32 ch[NUM_DIGS])
	{
		S32 dig = 0;
		while (dig < NUM_DIGS) {
			S32& d = ch[dig];
			d ^= 1;
			if (d) {
				break;
			}
			++dig;
		}
		return dig == NUM_DIGS;
	}

	S32 calcWin(S32 ch[NUM_DIGS])
	{
		S32 sum;
		S32 win = 0;
		// XXX
		sum = ch[0] + ch[2] + ch[4];
		if (sum & 1)
			++win;
		// XYY
		sum = ch[0] + ch[3] + ch[5];
		if (!(sum & 1))
			++win;
		// YXY
		sum = ch[1] + ch[2] + ch[5];
		if (!(sum & 1))
			++win;
		// YYX
		sum = ch[1] + ch[3] + ch[4];
		if (!(sum & 1))
			++win;
		return win;
	}

	void showChoices(S32 ch[NUM_DIGS])
	{
		S32 wincount = 3;
		for (S32 i = 0; i < NUM_DIGS;++i) {
			S32 v = ch[i];
			v = 1 - 2 * v;
			logger("%2d ", v);
		}
		logger("  Win %d/4\n",calcWin(ch));
	}

	void doGHZ()
	{
		logger("############## start do GHZ classical ##############\n");
		logger_disableindent();
		logger("\n");

		logger("tables\n");
		S32 choices[NUM_DIGS] {};
		/* indices
		AX 0
		AY 1
		BX 0
		BY 1
		CX 0
		CY 1
		*/
		/* values
		0 -> 1
		1 -> -1
		*/
		S32 watch = 0;
		logger("AX AY BX BY CX CY\n");
		while (watch<100) {
			if (watch % 4 == 0)
				logger("\n");
			if (watch % 16 == 0)
				logger("\n");
			++watch;
			showChoices(choices);
			bool wrap = incChoices(choices);
			if (wrap) {
				break;
			}
		}
		logger("watch = %d\n", watch);

		logger("\n");
		logger_enableindent();
		logger("############## end do GHZ classical ##############\n\n");
		popstate();
	}

} // end namespace ghz
#endif



#ifdef MARKOV
void markov()
{
	logger("######################### in markov #########################\n");
	logger_indent();
	const S32 startstate = 0;
	S32 state;
	const S32 metaruns = 10;
	const S32 numruns = 10000;
	const float S0toS0 = .8f; // probability
	const float S1toS0 = .4f;
	S32 count0,count1;
	S32 i,j;
	mt_setseed(getmillisec());
	for (j=0;j<metaruns;++j) {
		state = startstate;
		count0 = 0;
		count1 = 0;
// run the markov simulation
		for (i=0;i<numruns;++i) {
			float r = mt_frand();
			if (state == 0) {
				if (r >= S0toS0) {
					state = 1;
				}
			} else { // in state 1
				if (r < S1toS0) {
					state = 0;
				}
			}
			if (state == 0)
				++count0;
			else
				++count1;
		}
// done run the markov simulation
		logger("run %3d: count0 = %6d, count1 = %6d\n",j,count0,count1);
	}
	logger_unindent();
}
#endif

#ifdef SMALLTEST
void reverse(S32* arr,S32 first,S32 last)
{
	if (last - first <= 1)
		return; // no reverse for 1 element
}

void smalltest()
{
	logger("######################### in small test #########################\n");
	S32 arr[] = {2,3,5,7,11};//,13,17,19,23,29};
	S32 narr = NUMELEMENTS(arr);
	for (S32 i=0;i<narr;++i)
		logger("arr[%d] = %d\n",i,arr[i]);

	reverse(arr,0,narr);
}
#endif

#ifdef TESTROUND
void testround()
{
	logger("=========== test rounding ===========\n");
	S32* a = new int;
	delete a; // check mem leaks works
	float f;

	logger("standard type cast float to int\n");
	for (f = -2.0f ; f <= 2.0f ; f+=.125f) {
		//S32 i = S32(f);
		S32 i;
		F2INT(f,i);
		logger("\tfloat = %8.3f, int = %3d\n",f,i);
	}

	setfpuroundprec(FPUCW_RND_NEAR,FPUCW_PREC_32);
	logger("near type cast float to int\n");
	for (f = -2.0f ; f <= 2.0f ; f+=.125f) {
		//S32 i = S32(f);
		S32 i;
		F2INT(f,i);
		logger("\tfloat = %8.3f, int = %3d\n",f,i);
	}

	setfpuroundprec(FPUCW_RND_DOWN,FPUCW_PREC_32);
	logger("down type cast float to int\n");
	for (f = -2.0f ; f <= 2.0f ; f+=.125f) {
		//S32 i = S32(f);
		S32 i;
		F2INT(f,i);
		logger("\tfloat = %8.3f, int = %3d\n",f,i);
	}

	setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
	logger("up type cast float to int\n");
	for (f = -2.0f ; f <= 2.0f ; f+=.125f) {
		//S32 i = S32(f);
		S32 i;
		F2INT(f,i);
		logger("\tfloat = %8.3f, int = %3d\n",f,i);
	}

	setfpuroundprec(FPUCW_RND_0,FPUCW_PREC_32);
	logger("0 type cast float to int\n");
	for (f = -2.0f ; f <= 2.0f ; f+=.125f) {
		//S32 i = S32(f);
		S32 i;
		F2INT(f,i);
		logger("\tfloat = %8.3f, int = %3d\n",f,i);
	}

	defaultfpucontrol();

}

#endif

#ifdef TESTTOBLOCHANGLEAXIS

/// mat2 //////
struct mat2q {
	compf e[2][2];
};

void logmat2c(const mat2q* m,const C8* name)
{
	S32 i,j;
	logger("--------- mat2c - %s -----------\n",name);
	for (j=0;j<2;++j) {
		for (i=0;i<2;++i) {
			const compf& v = m->e[j][i];
			logger("(%10.3g,%10.3g) ",v.real(),v.imag());
		}
		logger("\n");
	}
}

/*
credit Craig Gidney, thanks
Breaksdown a matrix U into axis, angle, and phase_angle components satisfying
U = exp(i phase_angle) (I cos(angle/2) - axis sigma i sin(angle/2))

:param matrix: The 2x2 unitary matrix U
:return: The breakdown (axis(x, y, z), angle, phase_angle)
*/
void toBlochAngleAxis(mat2q m,pointf3* axis,float* angleRet,float* phaseRet)
{
	// read in the matrix
	compf a = m.e[0][0];
	compf b = m.e[0][1];
	compf c = m.e[1][0];
	compf d = m.e[1][1];

    // --- Part 1: convert to a quaternion ---
    // Phased components of quaternion.
	compf pfi = compf(0,.5f);
    compf wp = (a + d) * .5f;
    compf xp = -(b + c) * pfi;
    compf yp = (b - c) * .5f;
    compf zp = -(a - d) * pfi;

    // Arbitrarily use largest value to determine the global phase factor.
	// W
    // phase = max([wp, xp, yp, zp], key=abs)
	float bestnrm = norm(wp);
	compf phase = wp;
	// X
	float nrm = norm(xp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = xp;
	}
	// Y
	nrm = norm(yp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = yp;
	}
	// Z
	nrm = norm(zp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = zp;
	}
    phase /= abs(phase);
	compf phaseconj = conj(phase);


    // Cancel global phase factor, recovering quaternion components.
    float w = (wp * phaseconj).real();
    float x = (xp * phaseconj).real();
    float y = (yp * phaseconj).real();
    float z = (zp * phaseconj).real();

    // --- Part 2: convert from quaternion to angle-axis ---

    // Floating point error may have pushed w outside of [-1, +1]. Fix that.
	// w = min(max(w, -1.0f), 1.0f);
    w = range(-1.0f,w,1.0f);

    // Recover angle.
    *angleRet = -2*acosf(w);

    // Normalize axis.
    float n = sqrtf(x*x + y*y + z*z);
	if (n < 0.000001f) {
        // There's an axis singularity near angleRet=0.
        // Just default to no rotation around the Z axis in this case.
        *angleRet = 0;
        x = 0;
        y = 0;
        z = 1;
        n = 1;
	}
	float in = 1.0f/n;
    x *= in;
    y *= in;
    z *= in;

	// --- Part 3: (optional) canonicalize ---
    // Prefer angle in [-pi, pi]
	if (*angleRet <= -PI) {
        *angleRet += TWOPI;
        phase *= -1;
	}
    // Prefer axes that point positive-ward.
	if (x + y + z < 0) {
        x *= -1;
        y *= -1;
        z *= -1;
        *angleRet *= -1;
	}
	axis->x = x;
	axis->y = y;
	axis->z = z;
    //phase_angle = cmath.polar(phase)[1]
	*phaseRet = arg(phase);
    //return axis(x, y, z), angle, phase_angle
}

static float SR2O2 = sqrtf(2.0f)*.5f;
void testtoBlochAngleAxis()
{
	logger("=========== test toBlochAngleAxis ===========\n");
	mat2q mats[] =
	{
		{{ // Identity
			{1,0},
			{0,1},
		}},
		{{ // Pauli X, 180 deg around X
			{0,1},
			{1,0},
		}},
		{{ // Pauli Y, 180 deg around Y
			{0,compf(0,-1)},
			{compf(0,1),0},
		}},
		{{ // Pauli Z, 180 deg around Z
			{1,0},
			{0,-1},
		}},
		{{ // Hadamard, 180 deg around X+Z
			{SR2O2,SR2O2},
			{SR2O2,-SR2O2},
		}},
		{{ // -90 deg X axis, no phase
			{SR2O2,compf(0,SR2O2)},
			{compf(0,SR2O2),SR2O2},
		}},
		{{ // T, 45 deg around Z
			{1,0},
			{0,compf(SR2O2,SR2O2)},
		}},
	};

#if 0
print(toBlochAngleAxis([[1, 0], [0, 1]])) # Identity
# ([0, 0, 1], 0, 0.0)

print(toBlochAngleAxis([[0, 1], [1, 0]])) # Pauli X, 180 deg around X
# ([1.0, -0.0, -0.0], 3.141592653589793, 1.5707963267948966)

print(toBlochAngleAxis([[0, -1j], [1j, 0]])) # Pauli Y, 180 deg around Y
# ([-0.0, 1.0, -0.0], 3.141592653589793, 1.5707963267948966)

print(toBlochAngleAxis([[1, 0], [0, -1]])) # Pauli Z, 180 deg around Z
# ([-0.0, -0.0, 1.0], 3.141592653589793, 1.5707963267948966)

s = math.sqrt(0.5)
print(toBlochAngleAxis([[s, s], [s, -s]])) # Hadamard, 180 deg around X+Z
# ([0.7071067811865476, -0.0, 0.7071067811865476], 3.141592653589793, 1.5707963267948966)

print(toBlochAngleAxis([[s, s*1j], [s*1j, s]])) # -90 deg X axis, no phase
# ((1.0, 0.0, 0.0), -1.5707963267948966, 0.0)
#endif

//	};
	const S32 nummats = NUMELEMENTS(mats);
	S32 i;
	for (i=0;i<nummats;++i) {
		pointf3 axis;
		float angle,phase;
		logger("\n-------- %2d ------------\n",i);
		//return axis(x, y, z), angle, phase_angle
		toBlochAngleAxis(mats[i],&axis,&angle,&phase);
		logmat2c(&mats[i],"matrix");
		logger("axis %f %f %f,angle %f,phase %f\n",axis.x,axis.y,axis.z,angle,phase);
	}
}

#if 0
// 6 tests

print(toBlochAngleAxis([[1, 0], [0, 1]])) # Identity
# ([0, 0, 1], 0, 0.0)

print(toBlochAngleAxis([[0, 1], [1, 0]])) # Pauli X, 180 deg around X
# ([1.0, -0.0, -0.0], 3.141592653589793, 1.5707963267948966)

print(toBlochAngleAxis([[0, -1j], [1j, 0]])) # Pauli Y, 180 deg around Y
# ([-0.0, 1.0, -0.0], 3.141592653589793, 1.5707963267948966)

print(toBlochAngleAxis([[1, 0], [0, -1]])) # Pauli Z, 180 deg around Z
# ([-0.0, -0.0, 1.0], 3.141592653589793, 1.5707963267948966)

s = math.sqrt(0.5)
print(toBlochAngleAxis([[s, s], [s, -s]])) # Hadamard, 180 deg around X+Z
# ([0.7071067811865476, -0.0, 0.7071067811865476], 3.141592653589793, 1.5707963267948966)

print(toBlochAngleAxis([[s, s*1j], [s*1j, s]])) # -90 deg X axis, no phase
# ((1.0, 0.0, 0.0), -1.5707963267948966, 0.0)
#endif

#endif // TESTTOBLOCHANGLEAXIS

#ifdef TESTGHZ

char* names[4] = {"XXX","XYY","YXY","YYX"};
	char* binnames[8] = {" 000","-001","-010"," 011","-100"," 101"," 110","-111",};
S32 yvalues[4] = {0,3,5,6};
float signv[8] = {1,-1,-1,1,-1,1,1,-1}; // for expected value

float samplerun[4][8] = {
	{.046f,.226f,.125f,.007f,.250f,.025f,.094f,.228f}, // XXX
	{.227f,.039f,.013f,.128f,.033f,.247f,.229f,.084f}, // XYY
	{.222f,.042f,.012f,.111f,.034f,.254f,.251f,.074f}, // YXY
	{.201f,.038f,.016f,.114f,.030f,.258f,.258f,.086f}, // YXX
};

void getsamplerun(float sample[4][8])
{
	copy(&samplerun[0][0],&samplerun[0][0]+4*8,&sample[0][0]);
}

// scheme bits are y2 y1 y0 x2 x1 x0, 64 possible
// 1 of 4 measurements
// returns 1 of 8 possible qstates
S32 getstrat(S32 meas,S32 scm)
{
	S32 ymask = yvalues[meas]; // example: YYX = 110
	S32 xmask = ~ymask;
	S32 xs = scm&7; // strat for x
	S32 ys = scm>>3; // strat for y
	S32 xsc = xmask&xs;
	S32 ysc = ymask&ys;
	S32 res = xsc | ysc;
	return res;
}

void getclassicalrun(float sample[4][8],S32 schm)
{
	// clear all
	fill(&sample[0][0],&sample[0][0]+4*8,0.0f);
	// all 0 strategy for now
	S32 j;
	for (j=0;j<4;++j) {
		S32 pick = getstrat(j,schm);
		sample[j][pick] = 1; // only one pick, the rest are 0 probability
	}
}

void printsamplerun(float sample[4][8],S32 scheme)
{
	S32 i,j;
	logger("\n\n************** scheme = %3d\n        ",scheme);
	for (i=0;i<8;++i) {
		logger("%s   ",binnames[i]);
	}
	logger("\n\n");//------------------------------------------------------------------------------\n");
	float m = 1;
	for (j=0;j<4;++j) {
		float prob = 0;
		float expect = 0;
		logger("%s ",names[j]);
		for (i=0;i<8;++i) {
			float v = sample[j][i];
			prob += v;
			expect += v*signv[i];
			logger("%6.3f ",v);
		}
		m *= expect;
		logger("'%s' prob %6.3f,  expect %6.3f\n",names[j],prob,expect);
	}
	logger("M = %f\n",m);
}

// 3 qubit classical hidden test, all 64 combinations
void testghz()
{
	logger("=========== test GHZ ===========\n");
	S32 scheme; // 0 to 63
	// bits are y2 y1 y0 x2 x1 x0
	S32 amp[8];
	fill(amp,amp+8,0);
	float sr[4][8];
	getsamplerun(sr);
	printsamplerun(sr,-1);
	for (scheme=0;scheme<64;++scheme) {
		getclassicalrun(sr,scheme);
		printsamplerun(sr,scheme);
	}

}
#endif

// test static members
#ifdef TESTSTATIC

//#define STACK_OVERFLOW
struct p2s {
	S32 a;
	static S32 b;
};
S32 p2s::b;

class pnt2cnt {
	S32 x,y;
	static S32 cntr;
#ifdef STACK_OVERFLOW
	static S32 watch;
#endif

public:
	// 2 arg constructor
	pnt2cnt(S32 xa,S32 ya) : x(xa),y(ya)
	{
		++cntr;
	}
	// copy constructor
	pnt2cnt(const pnt2cnt& rhs) : x(rhs.x),y(rhs.y)
	{
		++cntr;
	}
	// assignment operator, not really needed
	void operator=(const pnt2cnt& rhs)
	{
		if (this == &rhs)
			return; // same object
#ifdef STACK_OVERFLOW
		++watch;
		const S32 watchlimit = 100;
		if (watch >= watchlimit) {
			if (watch == watchlimit)
				error("watch hit!!"); // refuse to copy
			return;
		}
		*this = rhs; // bad idea, stack overflow
#else
		x = rhs.x; // we must do this ourselves to avoid recursion
		y = rhs.y;
#endif
	}
	static S32 getcount()
	{
		return cntr;
	}
	S32 getx()
	{
		return x;
	}
	S32 gety()
	{
		return y;
	}
	// one less of this class
	~pnt2cnt()
	{
		--cntr;
	}
};
S32 pnt2cnt::cntr;
#ifdef STACK_OVERFLOW
S32 pnt2cnt::watch;
#endif

struct foo {
	float f;
};

struct fooall {
	foo one;
	foo two;
};

fooall fai;
vector<fooall> fav;

class base {
public:
	int x;
	base() : x(3) {}
	virtual void speak() const
	{
		logger("base\n");
	}
};

class derived : public base {
public:
	int y;
	derived() : y(4) {}
	void speak() const
	{
		logger("derived\n");
	}
};

void teststatic()
{
    logger("^^^^^^^^^^^^^^^ test vector ^^^^^^^^^\n");
	fai.one.f = 1;
	fai.two.f = 2;
	fav.push_back(fai);
	fooall far = fav.back();
	fav.pop_back();
	logger("size of fai size = %d\n",fav.size());
	//fooall far = fav[0];
	logger("fooall pushed = %f and %f\n",far.one.f,far.two.f);
	logger("vvvvvvvvvvvvvvv end test vector vvvvvvvvv\n");

	fai.one.f = 1;
	fav.push_back(fai);
	logger("size of fai size = %d\n",fav.size());

	logger("^^^^^^^^^^^^^^^ test static ^^^^^^^^^\n");
	p2s ia,ib;
	ia.a = 3;
	ia.b = 4;
	ib.a = 5;
	ib.b = 6;
	logger("1st ia = (%d,%d), ib = (%d,%d)\n",ia.Aia.b,ib.Aib.b);
	p2s::b = 99;
	ia.a = 11;
	ib.a = 13;
	logger("2nd ia = (%d,%d), ib = (%d,%d)\n",ia.Aia.b,ib.Aib.b);
	logger("sizeof ia = %d\n",sizeof(ia));
	logger("sizeof ib = %d\n",sizeof(ib));
	logger("sizeof p2s = %d\n",sizeof(p2s));

	{
		//pnt2cnt err;
		pnt2cnt* pp2a = new pnt2cnt(11,22);
		pnt2cnt p2a(3,4);
		pnt2cnt p2b = p2a;
		logger("class count = %d\n",pnt2cnt::getcount());
		p2a = p2b;
		logger("class count = %d\n",p2a.getcount()); // works fine but bad style
		delete pp2a;
	}
	logger("class count = %d\n",pnt2cnt::getcount());

	{
#if 0
		vector<base> bs;
		base b1;
		derived d1;
		base b2 = d1;
#else
		vector<base*> bs;
		base* b1 = new base();
		derived* d1 = new derived();
		base* b2 = d1;
#endif
		bs.push_back(b1);
		bs.push_back(d1);
		bs.push_back(b2);
#if 0
		for (vector<base>::const_iterator it = bs.begin();it!=bs.end();++it)
			(*it).speak();
#else
		for (vector<base*>::const_iterator it = bs.begin();it!=bs.end();++it)
			(*it)->speak();
		delete b1;
		delete d1;
		// don't delete b2
#endif
	}


	logger("vvvvvvvvvvvvvvv end test static vvvvvvvvv\n");
}
#endif

/////////////// test functors
#ifdef FUNCTOR

template<typename T>
void testfunc(T f)
{
	bool res = f("hi","ho");
//	bool (*fp1)(const C8*,const C8*) = f;
	T fp2 = f;
}

bool compnamesrev(const C8* Aconst C8* b)
{
	return strcmp(Ab) > 0; // normal is less than
}

// sort on 3rd column
bool compnames2(const C8* Aconst C8* b)
{
	const U32 offset = 2;
	a += min(strlen(a),offset); // don't go past the null
	b += min(strlen(b),offset);
	return strcmp(Ab) < 0;
}

class columnselect {
	U32 column;
public:
	columnselect(U32 col) : column(col) {}
	bool operator() (const C8* Aconst C8* b)
	{
		a += min(strlen(a),column); // don't go past the null
		b += min(strlen(b),column);
		return strcmp(Ab) < 0;
	}

	// I tried to pass this into sort, won't work, I guess why operator() is there.
	bool testcomp(const C8* Aconst C8* b)
	{
		a += min(strlen(a),column); // don't go past the null
		b += min(strlen(b),column);
		return strcmp(Ab) < 0;
	}
};

// list the names list
void shownames(const C8* title,const C8* names[],U32 numnames)
{
	U32 i;
	logger("%s\n",title);
	for (i=0;i<numnames;++i)
		logger("%2d %p name = '%s'\n",i,names[i],names[i]);
}

typedef bool (*FUNC)(const C8* Aconst C8* b);

// demonstrate what a functor is
void dofunctor()
{
	// sort compare function
	const C8* names[] = {
		"hi",
		"abe",
		"jill",
		"zack",
		"",
		"bo",
		"frank",
	};
	const U32 numnames = NUMELEMENTS(names);
	logger("------------ in functor test -------------\n");

	//this will not sort properly, no overload for <, array of strings WILL work though, sorts the pointer addresses
	sort(names,names+numnames);
	shownames("std sort, default compare function, it sorts the pointers",names,numnames);

	sort(names,names+numnames,compnamesrev);
	shownames("sort on 1st column reverse",names,numnames);

	sort(names,names+numnames,compnames2);
	shownames("sort on 3rd column",names,numnames);

	columnselect second(3);
	sort(names,names+numnames,second); // works
	//sort(names,names+numnames,&second.testcomp); // doesn't work, can't pass member function as a main function
	shownames("sort with a functor on 4rd column",names,numnames);

	vector<FUNC> vf;
	vf.push_back(compnames2);
	//vf.push_back(second);

	testfunc(compnames2);
	testfunc(second);

//#define WITH_FUNCTOR

#ifdef WITH_FUNCTOR
	T funcp = second; // doesn't work
#else
	bool (*funcp)(const C8* Aconst C8* b) = compnames2; // works
#endif

	sort(names,names+numnames,funcp); // sort with the function pointer

#ifdef WITH_FUNCTOR
	shownames("sort with a functor pointer on 4rd column",names,numnames);
#else
	shownames("sort with pointer to 3rd colum (wanted with a functor pointer on 4rd column)",names,numnames);
#endif

}

#endif

/////////////// test functors2
#ifdef FUNCTOR2

typedef float (*FUNCFF)(float in);

class funaddn {
	U32 n;
public:
	funaddn(U32 na) : n(na) {}
	float operator() (float in)
	{
		return in + n;
	}
};

funaddn ftor17(17);
funaddn ftor19(19);

#if 0
template<typename T>
void testfunc(T f,float in)
{
	float res = f(in);
	return res;
}
#endif
float funadd3(float in)
{
	return in + 3;
}

float funadd7(float in)
{
	return in + 7;
}

float funadd17(float in)
{
	return ftor17(in);
}

float funadd19(float in)
{
	return ftor19(in);
}

FUNCFF funcfflist[] = {
	funadd7,
	funadd3,
	funadd17, // try to use functors for this one
	funadd19, // try to use functors for this one
};

void dofunctor2()
{
	logger("------------ in functor2 test -------------\n");
	funaddn funadd17 = funaddn(17);
	float addval = 10000;
	float res3 = funadd3(addval);
	float res7 = funadd7(addval);
	float res17 = funadd17(addval);
	logger("res3 = %f\n",res3);
	logger("res7 = %f\n",res7);
	logger("res17 = %f\n",res17);
	U32 i,j;
	logger("run through function list\n");
	for (i=0;i<NUMELEMENTS(funcfflist);++i) {
		for (j=50;j<=150;j+=50) {
			float resn = funcfflist[i](addval + j);
			logger("funcff idx %2d, in %f, out %f\n",i,addval + j,resn);
		}
	}
}

#endif

#ifdef TEMPLATES

// demonstrate what a template can do
template<typename T>
class doubler {
	T val;
public:
	doubler(T t) : val(t) {}
	T dodouble()
	{
		return val + val;
	}
};

void dotemplates()
{
	logger("------------ in templates test -------------\n");
	// now show template class in action
	doubler<string> ds("hello");
	doubler<float> df(13.0f);
	string dsresult = ds.dodouble();
	float dfresult = df.dodouble();
	logger("result of doubles are: string = '%s', float = %f\n",dsresult.c_str(),dfresult);
}

#endif

#ifdef VECTORDESTROYTEST
class element {
    S32 val;
    const vector<element>* par;
public:
    element(S32 vala,const vector<element>* para) : val(vala),par(para)
    {
    }
    ~element()
    {
        logger("in element destructor: val = %d, size of parent = %d\n",val,par->size());
    }
};

void vectordestroytest()
{
	logger("in vectordestroytest ===================\n");
	vector<element> eles0 = vector<element>();
	vector<element> eles1 = vector<element>();
	{
		element e = element(34,&eles0);
		eles0.push_back(e);
		e = element(47,&eles0);
		eles0.push_back(e);
		e = element(69,&eles0);
		eles0.push_back(e);
	}
	eles0.clear();
	logger("done\n");
}
#endif

#if defined(HEAPTEST) || defined(MEDIANTEST) // see if we need a heap

// type of data for heap and median tests
//#define U32TYPE
#define FLOATTYPE

#ifdef U32TYPE
	typedef U32 datatype;
	const C8* formattype = "%2u ";
#define randomtype(maxsize) mt_random(maxsize)
#endif

#ifdef FLOATTYPE
	typedef float datatype;
	const C8* formattype = "%4.1f ";
#define randomtype(maxval) mt_frand()
#endif

template<typename T>
class heap {
//         0
//    1          2
//  3   4     5     6
// 7 8 9 10 11 12 13 14

private:
	bool more; // if true, larger values at top of heap
	vector<T> data;

	bool comp(T AT b)
	{
		bool ret = a < b;
		if (more)
			ret = !ret;
		return ret;
	}
public:
	heap(bool morea) : more(morea)
	{
	}

	void push(T v) {
		data.push_back(v);
		// fixup heap violation, push
		U32 from = data.size() - 1;
		while(from) {
			U32 to = (from - 1) >> 1; // move towards top of heap
			T& fromd = data[from];
			T& tod = data[to];
			if (comp(fromd,tod))
				swap(tod,fromd);
			else
				break;
			from = to;
		}
	}

	bool empty()
	{
		return data.empty();
	}

	T peek()
	{
		if (empty())
			errorexit("peek: heap is empty");
		return data[0];
	}

	T pop()
	{
		if (empty())
			errorexit("pop: heap is empty");
		T ret = data[0];
		T last = data.back();
		data.pop_back();
		if (!empty()) {
			data[0] = last;
			// fixup heap violation, pop, more complicated
			U32 from = 0;
			U32 numheap = data.size();
			while(true) {
				U32 to0 = (from<<1) + 1; // move down the heap
				if (to0 >= numheap) // no next nodes
					break;
				U32 to1 = to0 + 1;
				T& fromd = data[from];
				T& tod0 = data[to0];
				if (to1 >= numheap) { // just 1 left node
					if (comp(tod0,fromd)) {
						swap(tod0,fromd);
					}
					break; // can't continue to move up since 'from' is only a left node ( 'to0' is last node)
				}

				// 2 nodes to check
				T& tod1 = data[to1];
				if (comp(fromd,tod0) && comp(fromd,tod1))
					break; // heap okay
				// put lowest number on top
				if (comp(tod0,tod1)) { // tod0 < tod1
					swap(tod0,fromd);
					from = to0; // or to0
				} else { // tod1 < tod0
					swap(tod1,fromd);
					from = to1; // or to0
				}
			}
		}
		return ret;
	}

	void clear()
	{
		data.clear();
	}

	U32 size()
	{
		return data.size();
	}
};

template <typename T>
void runheap(const string& title,heap<T>& h,const T* arr,U32 arrsize)
{
	U32 i;
	for (i=0;i<arrsize;++i) {
		h.push(arr[i]);
	}
	logger("HEAP %s ",title.c_str());
	for (i=0;i<arrsize;++i) {
		logger(formattype,h.pop());
	}
	logger("\n");
}

template <typename T>
bool compu(T AT b)
{
	return a > b;
}

void heaptest()
{
	logger("****** in heap test\n");

//#define TESTPOP // std heap
#ifdef TESTPOP
	int myints[] = {10,20,30,5,15};
	std::vector<int> v(myints,myints+5);

	std::make_heap (v.begin(),v.end());
	logger("initial max heap   : %d\n",v.front());

	std::pop_heap (v.begin(),v.end());
	v.pop_back();
	logger("max heap after pop : %d\n",v.front());

	v.push_back(99);
	std::push_heap (v.begin(),v.end());
	logger("max heap after push: %d\n",v.front());

	std::sort_heap (v.begin(),v.end());

	logger("final sorted range :\n");
	for (unsigned i=0; i<v.size(); i++)
		logger("%2d ",v[i]);

	logger("\n");
#endif

	heap<datatype> lessheap(false);
	heap<datatype> moreheap(true);
	const datatype test2[] = {3,5,7,2,4,6,10,20,30,15,25,35};
	U32 test2size = NUMELEMENTS(test2);
	for (U32 i=test2size;i<=test2size;++i)
		runheap("test2 less",lessheap,test2,i);
	for (U32 i=test2size;i<=test2size;++i)
		runheap("test2 more",moreheap,test2,i);
	// bigger tests
	mt_setseed(459);
//#define TSMALL
//#define TMEDIUM
#define TLARGE
#ifdef TSMALL
	const U32 testsize = 1000;
	const U32 iterations = 30;
	const U32 maxsize = 30000;
#endif
#ifdef TMEDIUM
	const U32 testsize = 1000;
	const U32 iterations = 30;
	const U32 maxsize = 300000;
#endif
#ifdef TLARGE
	const U32 testsize = 10000;
	const U32 iterations = 30;
	const U32 maxval = 300000;
#endif
	vector<datatype> inputa; // random input
	vector<datatype> houtputal; // output of heap sort
	vector<datatype> houtputam; // output of heap sort
	U32 i,j;
	heap<datatype> sheapl(false);
	heap<datatype> sheapm(true);
	for (j=0;j<iterations;++j) {
		for (i=0;i<testsize;++i) {
			datatype r = randomtype(maxval);
			sheapl.push(r);
			sheapm.push(r);
			inputa.push_back(r);
		}
		for (i=0;i<testsize;++i) {
			datatype v = sheapl.pop();
			houtputal.push_back(v);
			v = sheapm.pop();
			houtputam.push_back(v);
		}
		// less
		sort(inputa.begin(),inputa.end()); // ascending
		// compare
		for (i=0;i<testsize;++i) {
			if (inputa[i] != houtputal[i])
				errorexit("heaptest less iter %d, size %d, fail!\n",j,i);
		}
		// more
		sort(inputa.begin(),inputa.end(),compu<datatype>); // descending
		// compare
		for (i=0;i<testsize;++i) {
			if (inputa[i] != houtputam[i])
				errorexit("heaptest more iter %d, size %d, fail!\n",j,i);
		}
		// cleanup
		inputa.clear();
		houtputal.clear();
		houtputam.clear();
	}
}

#endif

#ifdef MEDIANTEST

template<typename T>
class slowmedian {
	vector<T> data;
public:
	slowmedian()
	{
	}

	T runmedian(T in)
	{
		data.push_back(in);
		std::sort(data.begin(),data.end());
		U32 dsize = data.size();
		bool isodd = (bool)(dsize&1);
		U32 half = dsize>>1;
		if (isodd)
			return data[half];
		return (data[half-1] + data[half]) / 2;
	}

	void clear()
	{
		data.clear();
	}
};

template<typename T>
class heapmedian {
	heap<T> lefti; // lower numbers, pop highest
	heap<T> righti; // higher numbers, pop lowest
	//T middle;
	//bool hasmiddle;
public:
	heapmedian() : lefti(true),righti(false)/*,hasmiddle(false)*/
	{
	}

	T runmedian(T in)
	{
		// must keep peekleft <= peekright
		// first entry goes to the left
		if (lefti.empty()) { // left == right, start total == 0
			lefti.push(in); // 'left' heap is 0 or 1 larger in size than 'right' heap
			return in; // end total == 1
		}
		// second entry goes to the right, but after a comparison
		if (righti.empty()) { // start total == 1
			T peekleft = lefti.peek();
			if (peekleft <= in) { // just put 'in' to the right
				righti.push(in);
			} else { // move peekleft to right heap, and put 'in' to the left
				lefti.pop(); // got the value already in peekleft
				lefti.push(in);
				righti.push(peekleft);
			}
			// end total == 2
		} else { // three or more entries
			if (lefti.size() == righti.size()) {
				// start total == even, add to the left
				T peekright = righti.peek();
				if (peekright >= in) { // just push to the left
					lefti.push(in);
				} else { // move peekright to the left heap, and put 'in' to the right
					righti.pop();
					righti.push(in);
					lefti.push(peekright);
				}
				// end total == odd
			} else {
				// start total == odd, add to the right
				T peekleft = lefti.peek();
				if (peekleft <= in) { // just push to the right
					righti.push(in);
				} else { // move peekleft to the right heap, and put 'in' to the left
					lefti.pop();
					lefti.push(in);
					righti.push(peekleft);
				}
				// end total == even
			}

		}
		if (lefti.size() == righti.size()) {
			// even size, return avg of heaps
			return (lefti.peek() + righti.peek()) / 2;
		}
		// odd size, return peek of left heap
		return lefti.peek();
	}

	void clear()
	{
		lefti.clear();
		righti.clear();
	}

	U32 size()
	{
		return lefti.size() + righti.size();
	}
};

void mediantest()
{
	logger("****** in median test\n");
	const datatype test2[] = {3,5,7,2,4,6,10,20,30,15,25,35};
	U32 test2size = NUMELEMENTS(test2);
	slowmedian<datatype> sm;
	heapmedian<datatype> hm;
	logger("test data             : ");
	for (U32 i=0;i<test2size;++i) {
		datatype ini = test2[i];
		logger(formattype,ini);
	}
	logger("\n");
	logger("slow small median test: ");
	for (U32 i=0;i<test2size;++i) {
		datatype ini = test2[i];
		datatype outis = sm.runmedian(ini);
		logger(formattype,outis);
	}
	logger("\n");
	logger("heap small median test: ");
	for (U32 i=0;i<test2size;++i) {
		datatype ini = test2[i];
		datatype outih = hm.runmedian(ini);
		logger(formattype,outih);
	}
	logger("\n");
	// larger tests
#define MTSMALL
//#define MTMEDIUM
//#define MTLARGE
#ifdef MTSMALL
	const U32 testsize = 1000;
	const U32 iterations = 30;
	const U32 maxval = 30000;
#endif
#ifdef MTMEDIUM
	const U32 testsize = 1000;
	const U32 iterations = 30;
	const U32 maxval = 300000;
#endif
#ifdef MTLARGE
	const U32 testsize = 10000;
	const U32 iterations = 30;
	const U32 maxval = 300000;
#endif
	mt_setseed(2143);
	vector<datatype> inputa; // random input
	U32 i,j;
	for (j=0;j<iterations;++j) {
		slowmedian<datatype> smedian;
		heapmedian<datatype> hmedian;
		for (i=0;i<testsize;++i) {
			datatype r = randomtype(maxval);
			datatype sv = smedian.runmedian(r);
			datatype hv = hmedian.runmedian(r);
			if (sv != hv)
				errorexit("mediantest iter %d, size %d, fail!\n",j,i);
		}
	}
}

#endif

#ifdef GROVERTEST
void grovertest()
{
	logger("****** in grover test\n");
	U32 i,j;
	for (j=2;j<=6;++j) {
		logger("qubits = %d\n",j);
		logger("    prob = ");
		U32 N = 1<<j;
		float p = 1.0f/N;
		float as = sqrtf(p);
		float bs = as;
		for (i=0;i<=6;++i) {
			p = as*as;
			logger("%8.5f ",p);
			float cs = (-as + bs*(N - 1))/N;
			// new bs and as
			bs = cs - (bs - cs);
			as = cs - (-as - cs);
		}
		logger("\n");
	}
}
#endif

#ifdef UITEST
void uitest()
{
	logger("in uitest\n");
	int i;
	for (i=0;i<=1024;++i) {
		int j = i*i*i/1024/1024;
		logger("i = %d, j = %d\n",i,j);
	}
}

#endif

#ifdef CUBIC_R6_TEST
void testr6()
{
	complex sampleroots[] = {
		complex(3.0f,4.0f),
		complex(7.0f,11.0f),
		complex(13.0f,5.0f),
	};
	int r0idx[6] = {0,1,2,0,2,1};
	int r1idx[6] = {1,2,0,2,1,0};
	int r2idx[6] = {2,0,1,1,0,2};
	char* perms[6] = {"","012","021","12","02","01"};
	logger("Test cubic r^6 invariants\n");
	complex w0 = complex(-.5f,sqrtf(3.0f)/2.0f);
	complex w1 = complex(-.5f,-sqrtf(3.0f)/2.0f);
	int i;
	for (i=0;i<6;++i) {
		complex r0 = sampleroots[r0idx[i]];
		complex r1 = sampleroots[r1idx[i]];
		complex r2 = sampleroots[r2idx[i]];
		complex r = r0 + w0*r1 + w0*w0*r2; // E
		complex r3 = r*r*r; // A3
		complex r6 = r3*r3; // A3
		string rs = r.tostring();
		string r3s = r3.tostring();
		string r6s = r6.tostring();
		complex rp = r0 + w0*w0*r1 + w0*r2; // E
		complex r3p = rp*rp*rp; // A3
		complex r6p = r3p*r3p; // A3
		string rsp = rp.tostring();
		string r3sp = r3p.tostring();
		string r6sp = r6p.tostring();
		logger("perm (%-3s)\n",
			perms[i]);
		logger("\tr = %s\n\tr^3 = %s\n\tr^6 = %s\n",
			rs.c_str(),r3s.c_str(),r6s.c_str());
		logger("\trp = %s\n\trp^3 = %s\n\trp^6 = %s\n",
			rsp.c_str(),r3sp.c_str(),r6sp.c_str());
		complex sym = r3 + r3p; // S3, go from A3 to S3
		string syms = sym.tostring();
		logger("\tr^3+rp^3 = %s\n",
			syms.c_str());
		complex sym2 = r * rp; // S3, go from A3 to S3
		string syms2 = sym2.tostring();
		logger("\tr*rp = %s\n",
			syms2.c_str());
	}
	// test inverse of r1,r2,r3 to AB,C
	{
		complex r1 = 1.0f;
		complex r2 = 2.0f;
		complex r3 = 5.0f;
		complex A = r1 + r2 + r3;
		complex B = r1 + w0*r2 + w1*r3;
		complex C = r1 + w1*r2 + w0*r3;
		complex r1r = (A + B + C)/3.0f;
		logger("hi\n");
		complex lhs = (A*(w0*2.0f + 1.0f) + B*(-w0 - 2.0f) + C*(-w0 + 1.0f))*(1.0f/3.0f);
		complex rhs = w0*(-w0 + 1.0f)*r3;
		logger("check1 %s %s\n",lhs.tostring().c_str(),rhs.tostring().c_str());
		lhs = (A*(w1 + 2.0f) + B*(-w1*2.0 -1.0f) + C*(w1 - 1.0f))*(1.0f/3.0f);
		rhs = (-w0 + 1.0f)*r3;
		logger("check2 %s %s\n",lhs.tostring().c_str(),rhs.tostring().c_str());
		lhs = (A + B*w0 + C*w1)*(1.0f/3.0f);
		rhs = r3;
		logger("check3 %s %s\n",lhs.tostring().c_str(),rhs.tostring().c_str());
	}
}
#endif

//namespace {

#ifdef TESTBIRDS
static void testbirds()
{
	logger(" ^v^v^v^v^v^v do birds\n");
	mt_setseed(getmillisec());
	const S32 nbirds=2000; // 2 or more
	const S32 nruns=100000; // 1 or more
	float birds[nbirds];
	S32 i,j;
	float sumpaint=0;
	for (i=0;i<nruns;++i) {
//		logger("run %d\n",i);
		logger_indent();for (j=0;j<nbirds;++j) {
			birds[j]=mt_frand();
		}
		sort(birds,birds+nbirds);
//		for (j=0;j<nbirds;++j)
//			logger("bird %4d : %f\n",j,birds[j]);
		float paint=0;
// add 0 and n-1 segments
// n = nbirds - 1
		paint+=birds[1]-birds[0]; // 0
		if (nbirds>2)
			paint+=birds[nbirds-1]-birds[nbirds-2]; // n-1
		for (j=1;j<nbirds-2;++j) {
			if ((birds[j+1]-birds[j]<birds[j]-birds[j-1]) ||
			  (birds[j+1]-birds[j]<birds[j+2]-birds[j+1]))
				paint+=birds[j+1]-birds[j];
		}
//		logger("paint %f\n",paint);
		logger_unindent();
		sumpaint+=paint;
	}
	logger("avg paint = %f\n",sumpaint/nruns);
	logger("^v^v^v^v^v^v end do birds\n");
}
#endif

#ifdef TESTAUTOMORPH
// Cayley table
class grp {
	string name;
	S32 order;
	vector< vector <S32> > eles;
public:
	grp(string namea,const S32* elems,S32 norder) : name(namea),order(norder) // sizeof array is norder*norder
	{
		eles.resize(norder);
		S32 i,n=norder;
		for (i=0;i<n;++i) {
			eles[i].assign(elems+norder*i,elems+norder*(i+1));
		}
	}
	void print()
	{
		S32 i,j;
		logger("group '%s' of order %d\n",name.c_str(),order);
		for (j=0;j<order;++j) {
			for (i=0;i<order;++i) {
				logger("%2d ",eles[j][i]);
			}
			logger("\n");
		}
	}
};

void testautomorph()
{
	logger("------ start automorphism finder --------\n");
	static const S32 oc22=4;
	static const S32 c22[oc22*oc22]={0,1,2,3,1,0,3,2,2,3,0,1,3,2,1,0};
	grp g("c22",c22,oc22);
	g.print();
	logger("------ end automorphism finder ----------\n");
}
#endif

#ifdef SUMCUBES
void test1()
{
	logger("------ start sum cubes 1 --------\n");
	float bestabsdiff = 0.0f;
	float Ab,c;
	for (c = 0.0f ; c <= 1.0f ; c += .25f) {
		for (b = 0.0f ; b <= 1.0f ; b += .25f) {
			for (a = 0.0f ; a <= 1.0f ; a += .25f) {
				float e = a + b + c;
				float f = a*b + a*c + b*c;
				float g = a*b*c;
				float sc1 = a*a*a + b*b*b + c*c*c;
				float sc2 = e*e*e - 3.0f*e*f + 3.0f*g;
				float absdiff = abs(sc1 - sc2);
				if (absdiff > bestabsdiff)
					bestabsdiff = absdiff;
				logger("a = %f, b = %f, c = %f, sc1 = %f, sc2 = %f, diff = %f\n",
					Ab,c,sc1,sc2,sc1 - sc2);
			}
		}
	}
	logger("------ end sum cubes 1 best abs diff = %f --------\n",bestabsdiff);
}

void test2()
{
	logger("------ start sum cubes 2 --------\n");
	float bestabsdiff = 0.0f;
	float r,s,t;
	for (t = 0.0f ; t <= 1.0f ; t += .25f) {
		for (s = 0.0f ; s <= 1.0f ; s += .25f) {
			for (r = 0.0f ; r <= 1.0f ; r += .25f) {
				float b = -(r + s + t);
				float c = r*s + r*t + s*t;
				float d = -r*s*t;
				float sc1 = r*r*r + s*s*s + t*t*t;
				//float sc2 = -b*b*b - 3.0f*b*c - 9.0f*d; // doesn't seem to work
				//float sc2 = -b*b*b - 3.0f*b*c + 3.0f*d; // guess at right solution

				//float sc2 =  b*b*b + 3.0f*b*c + 3.0f*d; // figure it out
				//float sc2 =  b*b*b + 3.0f*b*c - 3.0f*d; // figure it out
				//float sc2 =  b*b*b - 3.0f*b*c + 3.0f*d; // figure it out
				//float sc2 =  b*b*b - 3.0f*b*c - 3.0f*d; // figure it out
				//float sc2 = -b*b*b + 3.0f*b*c + 3.0f*d; // figure it out
				float sc2 = -b*b*b + 3.0f*b*c - 3.0f*d; // figure it out
				//float sc2 = -b*b*b - 3.0f*b*c + 3.0f*d; // figure it out
				//float sc2 = -b*b*b - 3.0f*b*c - 3.0f*d; // figure it out

				float absdiff = abs(sc1 - sc2);
				if (absdiff > bestabsdiff)
					bestabsdiff = absdiff;
				logger("r = %f, s = %f, t = %f, sc1 = %f, sc2 = %f, diff = %f\n",
					r,s,t,sc1,sc2,sc1 - sc2);
			}
		}
	}
	logger("------ end sum cubes best abs diff = %f --------\n",bestabsdiff);
}

void sumcubes()
{
	test1(); // my way
	test2(); // mathisbeautyblog
}
#endif

#ifdef QUATTESTQ
void quattestq()
{
	static const float sr2o2 = sqrtf(2.0f)*.5f;
	logger("************ in quant test quant\n");
	pointf3 h = pointf3x(sr2o2,0,sr2o2,0); // H, quat
	pointf3 z = pointf3x(0,0,1,0); // Z, quat
	pointf3 x = pointf3x(1,0,0,0); // X, quat
	pointf3 calcx;
	// CALCX = H*Z*H, quat
	quattimes(&h,&z,&calcx);
	quattimes(&calcx,&h,&calcx);
	logger("PX = %f,%f,%f,%f\n",x.x,x.y,x.z,x.w);
	logger("CALCPX = %f,%f,%f,%f\n",calcx.x,calcx.y,calcx.z,calcx.w);
}
#endif

static void testtinyxml()
{
// test tinyxml
	TiXmlDocument doc;
	doc.LoadFile("test.xml");
	FILE* out=fopen2("textxmlout.txt","w");
	doc.Print(out);
	fclose(out);
// done test tinyxml
}

bitmap32* threatbm;

//} // end anonymous namespace

void scratchinit()
{
#ifdef PERSORTHO
	using namespace pers;
	adddebvars("pers", persdv, npersdv);
#endif
#ifdef WINE_ENTANGLE
	using namespace wine;
	adddebvars("wine", winedv, nwinedv);
#endif
#ifdef HAIL1
	using namespace hail1;
	dohail1(true); // always run a fixed number of steps
	dohail1(false); // run until <= 1
#endif
#ifdef HAIL2
	using namespace hail2;
	dohail2();
#endif
#ifdef HAIL3
	using namespace hail3;
	dohail3();
#endif
#ifdef GHZ
	using namespace ghz;
	doGHZ();
#endif
#ifdef SPINNERS
#include "u_spinners.h"
	spinnersInit();
#endif
#ifdef RENAMESHADERS
	renameShaders(); // convert .vs and .ps to .vert.glsl and .frag.glsl
#endif
#if 0
	float f = 3.0f;
	f = f / 0;
	bool isn = my_isnan(f);
	logger("isnan = %d\n", isn);
#endif
	video_setupwindow(1024,768);
	pushandsetdir("scratch");
	testtinyxml();
//	testpolynomial();
#ifdef LEIATEST1
	testLeastInterval();
#endif
#ifdef LEIATEST2
	testMedianClass();
#endif
#ifdef LEIATEST3
	stroboGrammatic();
#endif
#ifdef ZIGZAG
	testZigzag();
#endif
#ifdef INTERVALS
	testIntervals();
#endif
#ifdef CODERPAD
	testCoderpad();
#endif
#ifdef PERMUTATION_ITERATOR
	testPermutionIterator();
#endif
#ifdef ODD_ONE_OUT
	testOddOneOut();
#endif
#ifdef MODERNCPPTEST
	modernCPPTest();
#endif
#ifdef REVERSELINKLIST
	testLinkList();
#endif
#ifdef CYCLICCOPY
	testCopyCyclicGraph();
#endif
#ifdef CIRCULARTEST
	circulartest();
#endif
#ifdef STRTOKTEST
	strtoktest();
#endif
#ifdef ETEST
	doEtest();
#endif
#ifdef MARKOV
	markov();
#endif
#ifdef SMALLTEST
	smalltest();
#endif
#ifdef TESTGHZ
	testghz();
#endif
#ifdef TESTROUND
	testround();
#endif
#ifdef TESTTOBLOCHANGLEAXIS
	testtoBlochAngleAxis();
#endif
#ifdef TESTSTATIC
	teststatic();
#endif
#ifdef TESTBIRDS
	testbirds();
#endif
#ifdef SUMCUBES
	sumcubes();
#endif
#ifdef TESTAUTOMORPH
	testautomorph();
#endif
#ifdef CUBIC_R6_TEST
	testr6();
#endif
#ifdef UITEST
	uitest();
#endif
#ifdef LISTTEST
	testLists();
#endif
#ifdef HEAPTEST
	heaptest();
#endif
#ifdef MEDIANTEST
	mediantest();
#endif
#ifdef GROVERTEST
	grovertest();
#endif
	threatbm = gfxread32("dhs_threat1.jpg");
#ifdef DEBUGTEST
	vector<int> someints;
	int someints2[] = {2,3,5,7,11};
	someints.push_back(39);
	someints.push_back(64);
	U32 i;
	for (i=0;i<someints.size();++i) {
        logger("test int debug codeblocks for vector %d = %d\n",i,someints[i]);
	}
	for (i=0;i<NUMELEMENTS(someints2);++i) {
        logger("test int debug codeblocks for array %d = %d\n",i,someints2[i]);
	}
#endif
#ifdef VECTORDESTROYTEST
	vectordestroytest();
#endif
#ifdef QUATTESTQ
	quattestq();
#endif
#ifdef FUNCTOR
	dofunctor();
#endif
#ifdef FUNCTOR2
	dofunctor2();
#endif
#ifdef TEMPLATES
	dotemplates();
#endif
}

void scratchproc()
{
//	changestate(NOSTATE);
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
#ifdef PERSORTHO
	using namespace pers;
	doPersOrtho();
#endif
#ifdef WINE_ENTANGLE
	using namespace wine;
	doWine();
#endif
#ifdef SPINNERS
	spinnersProc();
#endif
}

void scratchdraw2d()
{
#if 1
	C32 col;
//	video_lock();
	clipclear32(B32,C32(0,0,255));
	clipblit32(threatbm,B32,0,0,20,20,threatbm->size.x,threatbm->size.y);
	clipcircle32(B32,100,100,5,C32RED);
	clipcircle32(B32,120,100,5,C32GREEN);
	clipcircle32(B32,140,100,5,C32BLUE);
	cliprect32 (B32,MX   ,MY   ,16,10,C32WHITE);
	clipcircle32(B32,300,100,25,C32BLACK);
	clipcircle32(B32,400,300,50,C32WHITE);
	clipline32(B32,0,0,639,479,C32WHITE);
	clipline32(B32,0,479,639,0,C32WHITE);
	outtextxyf32(B32,WX/2,WY-16,C32LIGHTRED,"H I   T H E R E %d !!!",3+4);
	cliprecto32(B32,MX+40,MY   ,16,10,C32WHITE);
	cliprecto32(B32,MX+40,MY+40,16,10,C32WHITE);
	cliprecto32(B32,MX+40,MY-40,16,10,C32WHITE);
	cliprecto32(B32,MX   ,MY+40,16,10,C32WHITE);
	cliprecto32(B32,MX   ,MY-40,16,10,C32WHITE);
	cliprecto32(B32,MX-40,MY   ,16,10,C32WHITE);
	cliprecto32(B32,MX-40,MY+40,16,10,C32WHITE);
	cliprecto32(B32,MX-40,MY-40,16,10,C32WHITE);
	col=clipgetpixel32(B32,MX,MY);
	clipputpixel32(B32,20,20,col);
	clipputpixel32(B32,20,21,col);
	clipputpixel32(B32,20,22,col);
	clipputpixel32(B32,21,20,col);
	clipputpixel32(B32,21,22,col);
	clipputpixel32(B32,22,20,col);
	clipputpixel32(B32,22,21,col);
	clipputpixel32(B32,22,22,col);
//	video_unlock();
#endif
//	rl->draw();
}

void scratchexit()
{
	bitmap32free(threatbm);
	popdir();
//	delete rl;
#ifdef PERSORTHO
	removedebvars("pers");
#endif
#ifdef WINE_ENTANGLE
	removedebvars("wine");
#endif
#ifdef SPINNERS
	spinnersExit();
#endif
}
