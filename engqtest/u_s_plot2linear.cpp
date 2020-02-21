// plotter2 runs in FPU control high precision, so you can use doubles correctly if necessary
// interactively test linear regression formulas

#include <m_eng.h>
#include "u_plotter2.h"

using namespace u_plotter2;

namespace linear_regression {
	const S32 NUM_POINTS = 4;
	S32 npoints = NUM_POINTS;
	pointf2 points[NUM_POINTS] = { 
		{3,4},
		{5,6},
		{11,14},
		{13,12}
	};
	float M = .5f;
	float B = 3;
	float ERR;
	float M1 = 1.5f;
	float B1 = 4;
	float ERR1;
	float M2 = 2.5f;
	float B2 = 5;
	float ERR2;
	float M4 = 3.5f;
	float B4 = 6;
	float ERR4;
	float descentStep = .0015f;
	float moveStep = .4f; // multiplied by dx

	// for debvars
	struct menuvar plot2linear[] = {
		{"@yellow@--- LINEAR REGRESSION VARS ---",NULL,D_VOID,0},
		{"npoints",&npoints,D_INT},
		{"M",&M,D_FLOAT | D_RDONLY},
		{"B",&B,D_FLOAT | D_RDONLY},
		{"Err",&ERR,D_FLOAT | D_RDONLY},
		{"M1",&M1,D_FLOAT},
		{"B1",&B1,D_FLOAT},
		{"Err1",&ERR1,D_FLOAT | D_RDONLY},
		{"M2",&M2,D_FLOAT},
		{"B2",&B2,D_FLOAT},
		{"Err2",&ERR2,D_FLOAT | D_RDONLY},
		{"M4",&M4,D_FLOAT},
		{"B4",&B4,D_FLOAT},
		{"Err4",&ERR4,D_FLOAT | D_RDONLY},
		{"descentStep",&descentStep,D_FLOAT,FLOATUP / 32},
		{"moveStepStep",&moveStep,D_FLOAT,FLOATUP / 256},
		{"points[0].x",&points[0].x,D_FLOAT,FLOATUP / 8},
		{"points[0].y",&points[0].y,D_FLOAT,FLOATUP / 8},
		{"points[1].x",&points[1].x,D_FLOAT,FLOATUP / 8},
		{"points[1].y",&points[1].y,D_FLOAT,FLOATUP / 8},
		{"points[2].x",&points[2].x,D_FLOAT},
		{"points[2].y",&points[2].y,D_FLOAT,FLOATUP / 8},
		{"points[3].x",&points[3].x,D_FLOAT,FLOATUP / 8},
		{"points[3].y",&points[3].y,D_FLOAT,FLOATUP / 8},
	};
	const int nplot2linear = NUMELEMENTS(plot2linear);

	float calcError(float M, float B, float power)
	{
		float err = 0;
		for (S32 i = 0; i < npoints; ++i) {
			const pointf2& p = points[i];
			float term = M * p.x + B - p.y;
			float termP = pow(abs(term), power);
			err += termP;
		}
		return err;
	}

	float descend(float& X, float& Y, float power)
	{
		const U32 iter = 100;
		float ex0y0;
		for (U32 i = 0; i < iter; ++i) {
			ex0y0 = calcError(X, Y, power);
			float ex1y0 = calcError(X + descentStep, Y, power);
			float ex0y1 = calcError(X, Y + descentStep, power);
			float dx = ex1y0 - ex0y0;
			float dy = ex0y1 - ex0y0;
			dx = range(-1.0f, dx, 1.0f);
			dy = range(-1.0f, dy, 1.0f);
			X -= dx * moveStep;
			Y -= dy * moveStep;
		}
		return ex0y0;
	}

	// linear regression formula
	void calcLine1() 
	{
		// calc power = 2 using formula
		float sx = 0, sy = 0, sxy = 0, sxx = 0;
		for (S32 i = 0; i < npoints; ++i) {
			const pointf2& p = points[i];
			sx += p.x;
			sy += p.y;
			sxy += p.x * p.y;
			sxx += p.x * p.x;
		}
		float n = float(npoints);
		solveLinear2(sx, n, sxx, sx, sy, sxy, M, B);
	}
} // end namespace linear_regression

using namespace linear_regression;

void plot2linearinit()
{
	// initialize graph paper
	plotter2init();
	adddebvars("linear_regression", plot2linear, nplot2linear);
}

void plot2linearproc()
{
	// interact with graph paper
	plotter2proc();
	// calc linear progression
	npoints = range(2, npoints, NUM_POINTS);
	calcLine1();
	ERR = calcError(M, B, 2);
	ERR1 = descend(M1, B1, 1);
	ERR2 = descend(M2, B2, 2);
	ERR4 = descend(M4, B4, 4);

}

void drawALine(float M, float B, C32 col)
{
	pointf2 minxy = getminvisxy();
	pointf2 maxxy = getmaxvisxy();
	//drawfpoint(minxy, C32BROWN,12);
	//drawfpoint(maxxy, C32BROWN,12);
	pointf2 startLine = pointf2x(minxy.x, minxy.x*M + B);
	pointf2 endLine = pointf2x(maxxy.x, maxxy.x*M + B);
	drawfpoint(startLine, col, 12);
	drawfpoint(endLine, col, 12);
	drawfline(startLine, endLine, col);
}

void plot2lineardraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// now do linear regression
	for (S32 i = 0; i < npoints; ++i) {
		drawfpoint(points[i], C32RED);
	}
	drawALine(M, B, C32GREEN);
	drawALine(M1, B1, C32RED);
	drawALine(M2, B2, C32BLUE);
	drawALine(M4, B4, C32YELLOW);
}

void plot2linearexit()
{
	// free graph paper, free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("linear_regression");
}
