// non associative spinners
#include <m_eng.h>
#include "u_permiter.h"

#define USE_SPINNER_NAMESPACE
#ifdef USE_SPINNER_NAMESPACE
namespace u_spinner {
#endif

int foo; // keep editor from indenting ?!?!

float a, b, apb, atb;
struct menuvar spinnersdv[] = {
	{"@green@--- SPINNERS ---",NULL,D_VOID,0},
	{"A", &a, D_FLOAT, FLOATUP / 4},
	{"B", &b, D_FLOAT, FLOATUP / 2},
	{"A+B", &apb, D_FLOAT | D_RDONLY},
	{"A*B", &atb, D_FLOAT | D_RDONLY},
};
const int nspinnersdv = NUMELEMENTS(spinnersdv);

// make into a class when neccesary
struct spinner {
	S32 vals[2];
	float probVal0; // probability of getting vals[0]
};

float probWin(const spinner& A, const spinner& B)
{
	float ret = 0;
	if (A.vals[0] > B.vals[0])
		ret += A.probVal0 * B.probVal0;
	if (A.vals[0] > B.vals[1])
		ret += A.probVal0 * (1 - B.probVal0);
	if (A.vals[1] > B.vals[0])
		ret += (1 - A.probVal0) * B.probVal0;
	if (A.vals[1] > B.vals[1])
		ret += (1 - A.probVal0) * (1 - B.probVal0);
	return ret;
}

// if > 0 then has non associative
float nonAssociativeScore(float AB, float BC, float CA)
{
	if (AB > .5 && BC > .5 && CA > .5)
		return min(AB, min(BC, CA));
	return 0;
}

// see if we can prune cases that don't work
bool checkSpinnerPermutation(const vector<S32>& pv)
{
	//return true;
	U32 vpSize = pv.size();
	if (vpSize != 6) {
		if (pv.size() > 0) {
			return pv[0] == 0;
		}
		return true;
	}
	// special for order 6, (3 spinners with 2 elements each)
	// 1st value must be 0, symmetry
	if (pv[0] != 1)
		return false;
	// 2nd value must not be 1, always loses, no chance for non associative
	if (pv[1] == 2)
		return false;
	// other spinners can't be 4 and 5
	if (pv[2] == 5 && pv[3] == 6)
		return false;
	if (pv[2] == 6 && pv[3] == 5)
		return false;
	if (pv[4] == 5 && pv[5] == 6)
		return false;
	if (pv[4] == 6 && pv[5] == 5)
		return false;
	// throw out spinners that aren't properly ordered
	if (pv[2] > pv[3])
		return false;
	if (pv[4] > pv[5])
		return false;
	return true;
}


#ifdef USE_SPINNER_NAMESPACE
} // namespace u_spinner
using namespace u_spinner;
#endif

void spinnersInit()
{
	adddebvars("spinners", spinnersdv, nspinnersdv);
	//const S32 PERM_ORDER = 6;
	//permIter pi = vector<S32> ( 1,2,3,4,5,6 );
	//vector<S32> vi{ 3,3,5 };
	permIter pi({ 1,2,3,4,5,6 });
	logger("START PERM\n");
	permc pic = permc(pi.getPerm());
	S32 permCount = 0;
	S32 prunePermCount = 0;
	while (pi.hasNext()) {
		const vector<S32>& pv = pi.getPerm();
		if (checkSpinnerPermutation(pv)) {
			pic = permc(pv);
			//pic.printPerm();
			logger("A(%d,%d), B(%d,%d), C(%d,%d)\n", pv[0], pv[1], pv[2], pv[3], pv[4], pv[5]);
			const int numSteps = 16;
			const float numInc = 1.0f / numSteps;
			spinner A = { pv[0],pv[1]};
			spinner B = { pv[2],pv[3]};
			spinner C = { pv[4],pv[5]};
			float bestScore = 0;
			for (float pA = 0; pA <= 1; pA += numInc) {
				A.probVal0 = pA;
				for (float pB = 0; pB <= 1; pB += numInc) {
					B.probVal0 = pB;
					for (float pC = 0; pC <= 1; pC += numInc) {
						C.probVal0 = pC;
						float AbeatsB = probWin(A, B);
						float BbeatsC = probWin(B, C);
						float CbeatsA = probWin(C, A);
						float score = nonAssociativeScore(AbeatsB, BbeatsC, CbeatsA);
						if (score > 0) {
							if (score > bestScore)
								bestScore = score;
							logger("A(%5.3f)>B(%5.3f) = %6.3f, B(%5.3f)>C(%5.3f) = %6.3f, C(%5.3f)>A(%5.3f) = %6.3f, score = %6.3f\n",
								A.probVal0, B.probVal0, AbeatsB,
								B.probVal0, C.probVal0, BbeatsC,
								C.probVal0, A.probVal0, CbeatsA,
								score);
						}
					}
				}
			}
			logger("best score = %f\n",bestScore);
			++prunePermCount;
		}
		pi.next();
		++permCount;
	}
	logger("END PERM, permCount = %d, prunePermCount = %d\n", permCount, prunePermCount);
}

void spinnersProc()
{
	apb = a + b;
	atb = a * b;
}

void spinnersExit()
{
	removedebvars("spinners");
}
