// find 2 elements that don't have a pair
#include <m_eng.h>
#include "u_oddOneOut.h"

#ifdef DONAMESPACE_ODD_ONE_OUT
namespace oddOneOut {
#endif

#ifdef DONAMESPACE_ODD_ONE_OUT
} // end namespace oddOneOut
using namespace oddOneOut;
#endif

void testOddOneOut()
{
	logger("\nin test oddOneOut\n"); 
	// 15, 5 are odd ones out
	S32 array[] = { 33, 34, 35, 15, 17, 17, 5, 66, 69, 69, 66 ,35, 33, 34 };
	S32 twoOut = 0;
	for (auto a : array) {
		twoOut ^= a;
	}
	S32 leastBit = twoOut & ~(twoOut - 1);
	S32 oneOut = 0;
	for (auto a : array) {
		if (!(leastBit & a)) {
			oneOut ^= a;
		}
	}
	logger("%6d, %6d\n", oneOut, oneOut^twoOut);
}
