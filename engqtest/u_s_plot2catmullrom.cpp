#include <float.h> // for _isnan

// plotter2 runs in FPU control high precision
// display/interact with Catmull Rom splines
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_plotter2.h"

using namespace u_plotter2;

namespace catmullrom {
	float samplePoints[] { 
		3, 
		5, 
		9, 
		7, 
		4, 
		2,
		5,
		9, 
		7, 
		4, 
		6
	};
	const S32 MAX_POINTS = NUMELEMENTS(samplePoints);
	S32 numPoints = 4;


	// for debvars
	struct menuvar catmullromdv[] = {
		{"@lightred@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
		{"numPoints",&numPoints,D_INT},
		{"P0",&samplePoints[0],D_FLOAT,FLOATUP / 4},
		{"P1",&samplePoints[1],D_FLOAT,FLOATUP / 4},
		{"P2",&samplePoints[2],D_FLOAT,FLOATUP / 4},
		{"P3",&samplePoints[3],D_FLOAT,FLOATUP / 4},
		{"P4",&samplePoints[4],D_FLOAT,FLOATUP / 4},
		{"P5",&samplePoints[5],D_FLOAT,FLOATUP / 4},
		{"P6",&samplePoints[6],D_FLOAT,FLOATUP / 4},
		{"P7",&samplePoints[7],D_FLOAT,FLOATUP / 4},
		{"P8",&samplePoints[8],D_FLOAT,FLOATUP / 4},
		{"P9",&samplePoints[9],D_FLOAT,FLOATUP / 4},
		{"P10",&samplePoints[10],D_FLOAT,FLOATUP / 4},
	};
	const S32 ncatmullromdv = NUMELEMENTS(catmullromdv);

	float catmullromcalcMat4(float p[4], float t)
	{
		pointf3x ps(p[0], p[1], p[2], p[3]);
		float t2 = t * t;
		float t3 = t2 * t;
		pointf3x ts(1.0f, t, t2, t3);
		const mat4 cmrMat = { {
			// rows p0,p1,p2,p3
			// columns 1,t,t2,t3
#define ORDER 3
#if ORDER == 0
			{0,0,0,0},
			{1,0,0,0},
			{0,0,0,0},
			{0,0,0,0},
#endif
#if ORDER == 1
			{0, 0,0,0},
			{1,-1,0,0},
			{0, 1,0,0},
			{0, 0,0,0},
#endif
#if ORDER == 3
			{0, -.5f,     1,  -.5f},
			{1,    0, -2.5f,  1.5f},
			{0,  .5f,     2, -1.5f},
			{0,    0,  -.5f,   .5f},
#endif
		} };
		pointf3 psm;
		mulmatvec4(&cmrMat, &ps, &psm);
		//float ret = psm.x*ts.x + psm.y*ts.y + psm.z*ts.z + psm.w*ts.w; // TODO: add dot4d
		float ret = dot4d(&psm, &ts);
		return ret;
		/*float s0 = p[1];
		float s1 = -.5f * p[0] + .5f * p[2];
		float s2 = p[0] - 2.5f * p[1] + 2 * p[2] - .5f * p[3];
		float s3 = -.5f * p[0] + 1.5f * p[1] - 1.5f * p[2] + .5f * p[3];
		return s0 + s1 * t + s2 * t2 + s3 * t3;*/
	}

	float catmullromcalc(float p[4], float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;
		float s0 = p[1];
		float s1 = -.5f * p[0] + .5f * p[2];
		float s2 = p[0] - 2.5f * p[1] + 2 * p[2] - .5f * p[3];
		float s3 = -.5f * p[0] + 1.5f * p[1] - 1.5f * p[2] + .5f * p[3];
		return s0 + s1 * t + s2 * t2 + s3 * t3;
	}

	float catmullromFunct(float x)
	{
		if (numPoints <= 0)
			return 0.0f;
		if (numPoints == 1)
			return samplePoints[0];
		// return first point if out of range negative
		if (x < 0)
			return samplePoints[0];
		// return last point if out of range positive
		if (x >= numPoints - 1)
			return samplePoints[numPoints - 1];
		float index;
		float t = modf(x, &index);
		S32 i = S32(index);
		float p[4];
		p[0] = i < 1
			? 2 * samplePoints[0] - samplePoints[1] // extend to same slope
			: samplePoints[i - 1];
		p[1] = samplePoints[i];
		p[2] = samplePoints[i + 1];
		p[3] = i >= numPoints - 2
			? 2 * samplePoints[numPoints - 1] - samplePoints[numPoints - 2] // extend to same slope
			: samplePoints[i + 2];
		return catmullromcalcMat4(p, t);
	}

} // end namespace catmullrom

using namespace catmullrom;

void plot2catmullrominit()
{
	adddebvars("CMR", catmullromdv, ncatmullromdv);
	plotter2init();
}

void plot2catmullromproc()
{
	numPoints = range(0, numPoints, MAX_POINTS);
	// interact with graph paper
	plotter2proc();
}

void plot2catmullromdraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw the points
	// first draw the points
	for (S32 i = 0; i < numPoints; ++i) {
		const auto& pnt = pointf2x(float(i), samplePoints[i]);
		drawfpoint(pnt, C32RED);
	}
	// now draw the spline
	drawfunctionrange(catmullromFunct, 0, numPoints - 1.0f, C32GREEN);
}

void plot2catmullromexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("CMR");
}
