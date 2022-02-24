#include <m_eng.h>
#include "u_random.h"

//#define C_RAND
//#define TEST_RAND
#define CPP_RAND

#ifdef C_RAND
// wrapper for random number generators
 // [0 - 1)
double randomF()
{
	return rand() / (RAND_MAX + 1.0);
}

// [lo - hi]
// TODO: make more precise
S32 randomRange(S32 lo, S32 hi)
{
	S32 rnd = rand(); // [0 - RAND_MAX]
	S32 nVals = hi - lo + 1;
	return lo + nVals * rnd / (RAND_MAX + 1);
}

void randomSetSeed(S32 seed)
{
	srand(seed);
}
#endif

#ifdef CPP_RAND
#include <random>
default_random_engine aRandomEngine;
// wrapper for random number generators
 // [0 - 1)
double randomF()
{
	uniform_real_distribution<> disReal(0.0, 1.0);
	return disReal(aRandomEngine);
}

// [lo - hi]
S32 randomRange(S32 lo, S32 hi)
{
	//logger("randomrange with lo %d and hi %d\n", lo, hi);
	uniform_int_distribution<> disInt(lo, hi);
	return disInt(aRandomEngine);
}

void randomSetSeed(S32 seed)
{
	aRandomEngine.seed(seed);
	//testSeed = seed;
}
#endif

#ifdef TEST_RAND
// wrapper for random number generators
 // [0 - 1)
S32 testSeed;
const S32 randMax = 0x7fff;

S32 randTest()
{
	return (testSeed++) & randMax;
}

double randomF()
{
	return randTest() / (randMax + 1.0);
}

// [lo - hi]
// TODO: make more precise
S32 randomRange(S32 lo, S32 hi)
{
	S32 rnd = randTest(); // [0 - RAND_MAX]
	S32 nVals = hi - lo + 1;
	return lo + nVals * rnd / (randMax + 1);
}

void randomSetSeed(S32 seed)
{
	testSeed = seed;
}
#endif

void testRandom()
{
	logger("BEGIN TEST RANDOM\n");
	randomSetSeed(655);
	logger("frands 1\n");
	for (auto i = 0; i < 10; ++i) {
		logger("frands = %f\n", randomF());
	}
	randomSetSeed(655);
	logger("frands 2\n");
	for (auto i = 0; i < 10; ++i) {
		logger("frands = %f\n", randomF());
	}
	randomSetSeed(0);
	vector<S32> counts(60000);
	for (auto i = 0; i < 6000000; ++i) {
		++counts[randomRange(0, 59999)];
	}
	logger("counts\n");
	for (auto i = 0U; i < counts.size(); ++i) {
		logger("idx = %d, count = %d\n",i, counts[i]);
	}
	logger("END TEST RANDOM\n");
}

void shortShuffle(vector<U32>& arr, U32 limit) {
	if (limit == 0 || limit >= arr.size()) {
		limit = arr.size() - 1;
	}
	for (U32 i = 0; i < limit; ++i) {
		U32 rng = randomRange(i, arr.size() - 1);
		//logger("limit = %d, rng = %d\n", limit, rng);
		swap(arr[i], arr[rng]);
	}
}

void testShuffle()
{
	logger("BEGIN TEST SHUFFLE\n");
	randomSetSeed(10);
	for (auto k = 0U; k < 1U; ++k) {
		vector<U32> arr(60000);
		for (auto i = 0U; i < arr.size(); ++i) {
			arr[i] = i;
		}
		shortShuffle(arr, 100);
		logger("show shuffle = \n");
		for (auto v : arr) {
			logger("%d\n", v);
		}
		logger("\n");
	}
	logger("END TEST SHUFFLE\n");
}
