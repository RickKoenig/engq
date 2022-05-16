// first test of plotter2
#include <float.h>

#include <m_eng.h>

#include "u_plotter2.h"

using namespace u_plotter2;

#define DONAMESPACE
#ifdef DONAMESPACE
namespace basel {
#endif

	S32 depth;
	S32 lhIndex;
	const float baseRad = 1.0f / PI;
	const S32 maxDepth = 5;
	float energy;
	vector<vector<pointf2>> lhArr; // light houses, level 0 has 1 lighthouse, level 1 has 2, etc. double

	// for debvars
	struct menuvar baseldv[] = {
		{"@yellow@--- basel ---",NULL,D_VOID,0},
		{"depth",&depth,D_INT,1},
		{"lhIndex",&lhIndex,D_INT,1},
		{"energy",&energy,D_FLOAT | D_RDONLY},
	};
	const int nbaseldv = NUMELEMENTS(baseldv);

	void drawPerp(const pointf2& v, const pointf2& a, const pointf2& b)
	{
		const float sqLen = .1f;
		pointf2 av{ a.x - v.x, a.y - v.y };
		float avLenInv = sqLen / sqrt(av.x * av.x + av.y * av.y);
		pointf2 bv{ b.x - v.x, b.y - v.y };
		float bvLenInv = sqLen / sqrt(bv.x * bv.x + bv.y * bv.y);
		pointf2 avNormLen{ av.x * avLenInv, av.y * avLenInv };
		pointf2 bvNormLen{ bv.x * bvLenInv, bv.y * bvLenInv };
		float pdot = abs(avNormLen.x * bvNormLen.x + avNormLen.y * bvNormLen.y);
		if (pdot < .0001f) {
			pointf2 c{ v.x + avNormLen.x, v.y + avNormLen.y };
			pointf2 d{ v.x + bvNormLen.x, v.y + bvNormLen.y };
			pointf2 e{ v.x + avNormLen.x + bvNormLen.x, v.y + avNormLen.y + bvNormLen.y };
			drawfline(c, e, C32BLACK);
			drawfline(d, e, C32BLACK);
		}
	}

	void calcBasel()
	{
		// lighthouse positions at all levels
		for (S32 j = 0; j <= maxDepth; ++j) {
			vector<pointf2> lhj;
			S32 numLight = 1 << j;
			float rad = static_cast<float>(numLight) * baseRad;
			for (S32 i = 0; i < numLight; ++i) {
				float ang = ((2.0f * i + 1.0f) / numLight - .5f) * PI;
				pointf2 lh = { rad * cos(ang), rad * sin(ang) + rad};
				lhj.push_back(lh);
				energy += 1.0f / len2dsq(&lh);
			}
			lhArr.push_back(lhj);
		}
	}

	void rangeCheck()
	{
		depth = range(0, depth, maxDepth);
		S32 maxIdx = -1;
		if (depth > 0) {
			maxIdx = (1 << (depth - 1)) - 1;
		}
		lhIndex = range(-1, lhIndex, maxIdx);
	}

	void calcEnergy()
	{
		// lighthouse energy at a given depth, inv square law, should be same between all levels
		energy = 0;
		for (const auto& lh : lhArr[depth]) {
			energy += 1.0f / len2dsq(&lh);
		}
	}

	void drawbasel()
	{
		float rad = baseRad;
		// lighthouses
		const float lhRad = .04f;
		if (depth > 0) {
			S32 prevDepth = depth - 1;
			const auto& lhiP = lhArr[prevDepth];
			if (lhIndex >= 0) { // one
				drawfcirclef(lhiP[lhIndex], C32RED, lhRad);
			} else { // -1, all
				for (const auto& lh : lhiP) {
					drawfcirclef(lh, C32RED, lhRad);
				}
			}
		}
		// current
		const auto& lhiC = lhArr[depth];
		if (lhIndex >= 0) { // one
			drawfcirclef(lhiC[lhIndex], C32LIGHTRED, lhRad);
			drawfcirclef(lhiC[lhIndex + (1 << (depth - 1))], C32LIGHTRED, lhRad);
		} else { // -1, all
			for (const auto& lh : lhiC) {
				drawfcirclef(lh, C32LIGHTRED, lhRad);
			}
		}
		// all the lines
		if (depth > 0) {
			S32 prevDepth = depth - 1;
			pointf2 zero2d{};
			const auto& lhPrev = lhArr[prevDepth];
			const U32 lhPrevSize = lhPrev.size();
			const auto& lhCur = lhArr[depth];
			if (lhIndex >= 0) { // one
				drawfline(lhPrev[lhIndex], lhCur[lhIndex], C32LIGHTGREEN);
				drawfline(lhPrev[lhIndex], lhCur[lhIndex + lhPrevSize], C32LIGHTGREEN);
				// current to origin
				drawfline(lhCur[lhIndex], zero2d, C32CYAN);
				drawfline(lhCur[lhIndex + lhPrevSize], zero2d, C32CYAN);
				// previous to origin
				drawfline(lhPrev[lhIndex], zero2d, C32RED);
				// perpendicular at previous lighthouse with one of the current lighthouses and origin
				drawPerp(lhPrev[lhIndex], lhCur[lhIndex], zero2d);
				// perpendicular at origin between 2 current lighthouses
				drawPerp(zero2d, lhCur[lhIndex], lhCur[lhIndex + lhPrevSize]);
			} else { // -1, all
				// between lighthouses
				for (U32 i = 0; i < lhPrevSize; ++i) {
					drawfline(lhPrev[i], lhCur[i], C32LIGHTGREEN);
					drawfline(lhPrev[i], lhCur[i + lhPrevSize], C32LIGHTGREEN);
				}
				// current to origin
				for (const auto &p : lhCur) {
					drawfline(p, zero2d, C32CYAN);
				}
				// previous to origin
				for (const auto &p : lhPrev) {
					drawfline(p, zero2d, C32RED);
				}
				// perpendicular at previous lighthouse with one of the current lighthouses and origin
				for (U32 i = 0; i < lhPrevSize; ++i) {
					drawPerp(lhPrev[i], lhCur[i], zero2d);
				}
				// perpendicular at origin between 2 current lighthouses
				for (U32 i = 0; i < lhPrevSize; ++i) {
					drawPerp(zero2d, lhCur[i], lhCur[i + lhPrevSize]);
				}
			}
		}
		// circles
		for (S32 i = 0; i <= depth; ++i) {
			drawfcirclef(pointf2x(0, rad), C32BLACK, rad);
			rad *= 2;
		}
	}

#ifdef DONAMESPACE
} // end namespace basel
using namespace basel;
#endif

// public

void plot2baselinit()
{
	adddebvars("basel", baseldv, nbaseldv);
	plotter2init();

	calcBasel();
	depth = 2;
	lhIndex = 0;

	lzoom = -.625f;
	center = pointf2x(-1, 1);
}

void plot2baselproc()
{
	// interact with graph paper
	plotter2proc();
	// calc the image
	rangeCheck();
	calcEnergy();
}

void plot2baseldraw2d()
{
	// draw graph paper
	plotter2draw2d();
	drawbasel();
}

void plot2baselexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("basel");
}
