#include <float.h> // for _isnan

// plotter2 runs in FPU control high precision
// display/interact with random Rom splines
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_plotter2.h"

using namespace u_plotter2;

//namespace randompoints {
	const S32 numPoints{ 200 };
	float randomPoints[numPoints];
	float randomPoints2[numPoints];

	float ticker = 0;
	float tickerStep = .00625f;
	float tickerLimit = 1;


	// for debvars
	struct menuvar randompointsdv[] = {
		{"@lightred@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
		{"time",&ticker,D_FLOAT,FLOATUP / 4},
		{"timeStep",&tickerStep,D_FLOAT,FLOATUP / 16},
		{"timeLimit",&tickerLimit,D_FLOAT,FLOATUP / 16},
	};
	const S32 nrandompointsdv = NUMELEMENTS(randompointsdv);

	float fract(float f)
	{
		return f - floor(f);
	}

	float randomIndex(S32 i, float mult)
	{
		float ret = fract(mult * sin(float(i)));
		return ret;
	}

//} // end namespace randompoints

//using namespace randompoints;

void plot2randominit()
{
	adddebvars("CMR", randompointsdv, nrandompointsdv);
	plotter2init();
	for (auto i = 0; i < numPoints; ++i) {
		float& f = randomPoints[i];
		float& f2 = randomPoints2[i];
		//f = mt_frand();
		f = randomIndex(i, 167.0f);
		f2 = 5.0f + floor(10.0f * randomIndex(i, 179.0f));
	}
}

void plot2randomproc()
{
	//numPoints = range(0, numPoints, MAX_POINTS);
	tickerLimit = range(.0625f, tickerLimit, 1000.0f);
	tickerStep = range(-tickerLimit, tickerStep, tickerLimit);
	ticker += tickerStep;
	if (ticker < 0)
		ticker += tickerLimit;
	if (ticker >= tickerLimit)
		ticker -= tickerLimit;
	// interact with graph paper
	plotter2proc();
}

void plot2randomdraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw the points
	// first draw the points
#if 0
	for (S32 i = 0; i < numPoints; ++i) {
		const auto pnt = pointf2x(float(i)*.25f, randomPoints[i] * 10.0f);
		const auto pnt2 = pointf2x(float(i)*.25f, randomPoints2[i] * 10.0f);
		drawfpoint(pnt, C32RED, 2);
		drawfpoint(pnt2, C32GREEN, 2);
	}
#endif
#if 1
	for (S32 i = 0; i < numPoints; ++i) {
		pointf2 pnt = pointf2x(10.0f * fract(randomPoints[i] + ticker*randomPoints2[i]), float(i)/20.0f);
		drawfpoint(pnt, C32RED, 2);
	}
#endif

	// now draw the spline
	//drawfunctionrange(randompointsFunct, 0, numPoints - 1.0f, C32GREEN);
}

void plot2randomexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("CMR");
}
