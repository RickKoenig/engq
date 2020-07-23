#include <m_eng.h>

namespace zero_knowledge {

// set Bezout coefficients and return gcd
S32 extendedEuclidAlgorithm(S32 a, S32 b, S32& BezX, S32& BezY)
{
	S32 r0 = a;
	S32 s0 = 1;
	S32 t0 = 0;
	S32 r1 = b;
	S32 s1 = 0;
	S32 t1 = 1;
	do {
		S32 q1 = r0 / r1;
		S32 r2 = r0 - q1 * r1;
		S32 s2 = s0 - q1 * s1;
		S32 t2 = t0 - q1 * t1;
		r0 = r1;
		r1 = r2;
		s0 = s1;
		s1 = s2;
		t0 = t1;
		t1 = t2;
	} while (r1);
	BezX = s0;
	BezY = t0;
	return r0;
}

S32 totient(S32 v)
{
	// naive, could use a more efficient formula (maybe)
	S32 count{ 0 };
	for (S32 i = 1; i <= v; ++i) {
		if (gcd(i, v) == 1) {
			++count;
		}
	}
	return count;
}

void totientTable(S32 maxTotient)
{
	logger("===== totient table =====\n");
	float lowestRatio{ 1 };
	S32 lowestOrder{ -1 };
	S32 lowestTotient{ -1 };
	for (S32 i = 1; i <= maxTotient; ++i) {
		S32 t = totient(i);
		float ratio = float(t) / i;
		logger("tot(%3d) = %3d, ratio = %f\n", i, t, ratio);
		if (ratio < lowestRatio) {
			lowestRatio = ratio;
			lowestOrder = i;
			lowestTotient = t;
		}
	}
	logger("lowest order = %3d, totient = %3d, ratio = %f\n", lowestOrder, lowestTotient, lowestRatio);
	logger("\n");
}

S32 calcInverseBrute(S32 primeField, S32 val)
{
	S32 i;
	for (i = 1; i < primeField; ++i) {
		S32 prod = i * val % primeField;
		if (prod == 1) {
			break;
		}
	}
	if (i == primeField)
		errorexit("can't find inverse with p = %d and val = %d", primeField, val);
	return i;
}

S32 calcInverseExtendedEuclid(S32 primeField, S32 val)
{
	S32 bezX, bezY, retGcd;
	retGcd = extendedEuclidAlgorithm(val, primeField, bezX, bezY);
	if (retGcd != 1)
		errorexit("inverse euclid, not a prime field");
	if (bezX < 0)
		bezX += primeField;
	return bezX;
}

class mulGroup {
	S32 order;
	vector<vector<S32>> table;
	S32 lowestGenerator;
	vector<bool> isGenerator;
public:
	mulGroup(S32 ord); // please pass in a prime number
	S32 power(S32 base, S32 exp);
	void showTable();
	void showExps();
	void showLowGenExp();
	void showLowGenLog();
	void showInverses();
private:
	void buildTable();
	void buildGenerators();
};

mulGroup::mulGroup(S32 ord) : order(ord), table(ord)
{
	// setup the table with 0's first
	for (auto& row : table) {
		row = vector<S32>(order);
	}
	buildTable();
	buildGenerators();
}

S32 mulGroup::power(S32 base, S32 exp)
{
	if (exp < 0)
		errorexit("bad: negative exponent");
	// naive approach first, TODO: use more efficient squaring method
	S32 ret = 1;
	const vector<S32>& rowBase = table[base];
	while (exp--) {
		ret = rowBase[ret];
	}
	return ret;
}

void mulGroup::showTable()
{
	logger("order %d, mult table\n", order);
	for (const auto& row : table) {
		for (const auto& cell : row) {
			logger("%3d ", cell);
		}
		logger("\n");
	}
	logger("\n");
}

void mulGroup::showExps()
{
	logger("===== order %d, exponent table =====\n", order);
	logger("exponents   ");
	const S32 extra = 2;
	for (auto exp = 0; exp < order + extra - 1; ++exp) {
		if (exp == order - 1) {
			logger("--extra--");
		}
		logger("%3d ", exp);
	}
	logger("\n\n");
	for (auto base = 1; base < order; ++base) {
		logger("base = %3d %c", base, isGenerator[base] ? 'G' : ' ');
		const vector<S32>& rowBase = table[base];
		S32 curValue = 1;
		for (auto exp = 0; exp < order + extra - 1; ++exp) {
			if (exp == order - 1) {
				logger("--extra--");
			}
			logger("%3d ", curValue);
			curValue = rowBase[curValue];
		}
		logger("\n");
	}
	logger("\n");
}

void mulGroup::showLowGenExp()
{
	S32 val = 1;
	const vector<S32>& rowBase = table[lowestGenerator];

	logger("exponent tables for p = %d, g = %d\n", order, lowestGenerator);
	const S32 extra = 3;
	for (auto power = 0; power < order + extra - 1; ++power) {
		if (power == order - 1) {
			logger("-- extra (show wrap) --\n");
		}
		logger("%3d ^ %3d = %3d\n", lowestGenerator, power, val);
		val = rowBase[val];
	}
	logger("\n");
}

void mulGroup::showLowGenLog()
{
	vector<S32> logTable(order + 1);
	const vector<S32>& rowBase = table[lowestGenerator];
	S32 val = 1;
	for (auto power = 0; power < order - 1; ++power) {
		logTable[val] = power;
		val = rowBase[val];
	}

	logger("log table for p = %d, g = %d\n", order, lowestGenerator);
	for (auto val = 1; val < order; ++val) {
		logger("log%d (%3d) = %3d\n", lowestGenerator, val, logTable[val]);
	}
	logger("\n");
}

void mulGroup::showInverses()
{
	logger("inverse table for p = %d\n", order);
	for (auto v = 1; v < order; ++v) {
		S32 invB = calcInverseBrute(order, v);
		S32 invEE = calcInverseExtendedEuclid(order, v);
		if (invB != invEE)
			errorexit("inverses don't match!");
		logger("%3d  * %3d  =  %3d\n", v, invEE, table[v][invEE]); // table should always read as a 1
		//logger("\n");
	}
	logger("\n");
}

void mulGroup::buildTable()
{
	for (auto j = 0; j < order; ++j) {
		vector<S32>& row = table[j];
		for (auto i = 0; i < order; ++i) {
			S32& cell = row[i];
			cell = (i * j) % order;
		}
	}
}

void mulGroup::buildGenerators()
{
	isGenerator = vector<bool>(order);
	lowestGenerator = std::numeric_limits<S32>::max();
	// naive approach first, TODO: use more efficient prime factoring method
	// find generators of the multiplicative group of 'order-1'
	for (S32 j = 1; j < order; ++j) {
		S32 prod = 1;
		vector<S32>& row = table[j]; 
		S32 i;
		for (i = 1; i < order; ++i) {
			prod = row[prod];
			if (prod == 1) {
				//logger("hit 1 at i = %d, j = %d\n", i,j);
				break;
			}
		}
		if (i == order - 1) {
			//logger("hit 1 at i = %d, j = %d\n", i,j);
			isGenerator[j] = true;
			if (j < lowestGenerator) {
				lowestGenerator = j;
			}
		}
	}
}

} // end namespace zero_knowledge
using namespace zero_knowledge;

void do_zero_knowledge()
{
	logger("========\n");
	logger("do zero knowledge\n\n");
	totientTable(10);
	//totientTable(35);
	//totientTable(110);
	S32 somePrimes[] = { 
		2,
		3,
		7,
		11,
		23,
		107,
	};

	for (auto p : somePrimes) {
		logger("\n\n\n##### ORDER = %d #####\n", p);
		mulGroup mg(p);
		mg.showTable();
		mg.showInverses();
		mg.showExps();
		mg.showLowGenExp();
		mg.showLowGenLog();
	}

	logger("========\n");
	popstate();
}
