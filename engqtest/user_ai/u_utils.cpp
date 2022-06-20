#include <m_eng.h>

#include "m_perf.h"

#include "u_s_selfdriving.h"
#ifdef DONAMESPACE
namespace selfdriving {
#endif

S32 intsectCount;
S32 intsectEarlyOut;
	
float lerp(float A, float B, float t)
{
	return A + (B - A) * t;
}

// pass in len = 5, index = [0,4] , P0 = 3, P1 = 4 we get [3, 3.25, 3.5, 3.75, 4]
// pass in len = 3, index = [0,2] , P0 = 3, P1 = 4 we get [3, 3.5, 4]
// pass in len = 1, index = 0 , P0 = 3, P1 = 4 we get [3.5] // no divide by zero
float lerpStep(U32 len, S32 index, float P0, float P1) {
	return lerp(
		P0,
		P1,
		len == 1
		? .5f
		: static_cast<float>(index) / (len - 1)
	);
}

bool getIntersection(const pointf2& A, const pointf2& B, const pointf2& C, const pointf2& D, pointf3* I)
{
	//perf_start(TEST3);
	++intsectCount;
	// early out for y
	float min0y = min(A.y, B.y);
	float max1y = max(C.y, D.y);
	if (min0y > max1y) {
		++intsectEarlyOut;
		//perf_end(TEST3);
		return false;
	}
	float max0y = max(A.y, B.y);
	float min1y = min(C.y, D.y);
	if (min1y > max0y) {
		++intsectEarlyOut;
		//perf_end(TEST3);
		return false;
	}
	// early out for x
	float min0x = min(A.x, B.x);
	float max1x = max(C.x, D.x);
	if (min0x > max1x) {
		++intsectEarlyOut;
		//perf_end(TEST3);
		return false;
	}
	float max0x = max(A.x, B.x);
	float min1x = min(C.x, D.x);
	if (min1x > max0x) {
		++intsectEarlyOut;
		//perf_end(TEST3);
		return false;
	}
	const float tTop = (D.x - C.x) * (A.y - C.y)
		- (D.y - C.y) * (A.x - C.x);
	const float uTop = (C.y - A.y) * (A.x - B.x)
		- (C.x - A.x) * (A.y - B.y);
	const float bottom = (D.y - C.y) * (B.x - A.x)
		- (D.x - C.x) * (B.y - A.y);

	if (bottom != 0) {
		const float t = tTop / bottom;
		const float u = uTop / bottom;
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
			if (I) {
				I->x = lerp(A.x, B.x, t);
				I->y = lerp(A.y, B.y, t);
				I->z = t; // offset
			}
			//perf_end(TEST3);
			return true;
		}
	}
	//perf_end(TEST3);
	return false;
}

bool polysIntersect(const vector<pointf2>& poly1, const vector<pointf2>& poly2)
{
	perf_start(TEST4);
	U32 p1size = poly1.size();
	U32 p2size = poly2.size();
	U32 p1sizeMod = p1size;
	U32 p2sizeMod = p2size;
	// optimize getIntersection calls
	if (p1size <= 1 || p2size <= 1) { // points cannot intersect anything
		perf_end(TEST4);
		return false;
	}
	if (p1size == 2) {
		p1size = 1; // 2 points is a line, not a poly, only one check
	}
	if (p2size == 2) {
		p2size = 1; // 2 points is a line, not a poly, only one check
	}
	for (auto i = 0U; i < p1size; ++i) {
		for (auto j = 0U; j < p2size; ++j) {
			const bool touch = getIntersection(
				poly1[i],
				poly1[(i + 1) % p1sizeMod],
				poly2[j],
				poly2[(j + 1) % p2sizeMod],
				nullptr
			);
			if (touch) {
				perf_end(TEST4);
				return true;
			}
		}
	}
	perf_end(TEST4);
	return false;
}

void clipline32dash(const struct bitmap32* b32, S32 x0, S32 y0, S32 x1, S32 y1, C32 color, S32 repeat)
{
	for (auto i = 0; i <= repeat + 1; ++i) {
		float t0 = static_cast<float>(i) / repeat;
		float t1 = static_cast<float>(i + 1) / repeat;
		// shorter segments
		t1 = (t1 + t0) * .5f;
		const float x0N = lerp(
			static_cast<float>(x0),
			static_cast<float>(x1),
			static_cast<float>(t0)
		);
		const float y0N = lerp(
			static_cast<float>(y0),
			static_cast<float>(y1),
			static_cast<float>(t0)
		);
		const float x1N = lerp(
			static_cast<float>(x0),
			static_cast<float>(x1),
			static_cast<float>(t1)
		);
		const float y1N = lerp(
			static_cast<float>(y0),
			static_cast<float>(y1),
			static_cast<float>(t1)
		);
		clipline32(b32
			, static_cast<S32>(x0N)
			, static_cast<S32>(y0N)
			, static_cast<S32>(x1N)
			, static_cast<S32>(y1N)
			, color);
	}
}

C32 getRGBA(float value) { // return a nice color for values from -1 to +1
	float alpha = abs(value);
	U32 R, G, B;
#if 1
	if (value >= 0) {
		R = 255;
		G = 255;
		B = 0;

	} else {
		R = 64;
		G = 64;
		B = 255;
	}
#endif
	return C32(static_cast<U32>(R * alpha), static_cast<U32>(G * alpha), static_cast<U32>(B * alpha));
}

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif

