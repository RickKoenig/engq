// first test of plotter2 
#include <float.h>

#include <m_eng.h>
//#include <l_misclibm.h>

#include "u_plotter2.h"

using namespace u_plotter2;

#define DONAMESPACE
#ifdef DONAMESPACE
namespace eigentest {
#endif

// quadratic tests
// coeffs
float coa=2,cob=-10,coc=8;
compf ccoa=1,ccob(-2,-4),ccoc(2,-8);
// roots
float r0,r1;
compf cr0,cr1;
compf ccr0,ccr1;
// results
S32 result1,result2;

// eigen tests
mat2 A = {2,1,1,2},EM0,EM1;
pointf2 inVec={0,0},outVec;
const U32 gridsx = 5;
const U32 gridsy = 5;
pointf2 ingrid[gridsy][gridsx];
pointf2 outgrid[gridsy][gridsx];
float eigenvalue0,eigenvalue1;
pointf2 eigenvector0,eigenvector1;
bool eigenreal;

// for debvars
struct menuvar eigendv[]={
	{"@lightred@--- 2D Matrix tests ---",NULL,D_VOID,0},
	{"A[0][0]",&A.e[0][0],D_FLOAT,FLOATUP/4},
	{"A[0][1]",&A.e[1][0],D_FLOAT,FLOATUP/4},
	{"A[1][0]",&A.e[0][1],D_FLOAT,FLOATUP/4}, // make column major (swap indices)
	{"A[1][1]",&A.e[1][1],D_FLOAT,FLOATUP/4},
	{"EM0[0][0]",&EM0.e[0][0],D_FLOAT|D_RDONLY},
	{"EM0[0][1]",&EM0.e[1][0],D_FLOAT|D_RDONLY},
	{"EM0[1][0]",&EM0.e[0][1],D_FLOAT|D_RDONLY}, // make column major (swap indices)
	{"EM0[1][1]",&EM0.e[1][1],D_FLOAT|D_RDONLY},
	{"EM1[0][0]",&EM1.e[0][0],D_FLOAT|D_RDONLY},
	{"EM1[0][1]",&EM1.e[1][0],D_FLOAT|D_RDONLY},
	{"EM1[1][0]",&EM1.e[0][1],D_FLOAT|D_RDONLY}, // make column major (swap indices)
	{"EM1[1][1]",&EM1.e[1][1],D_FLOAT|D_RDONLY},
	{"eigenreal",&eigenreal,D_INT|D_RDONLY},
	{"eigenvalue0",&eigenvalue0,D_FLOAT|D_RDONLY},
	{"eigenvector0[0]",&eigenvector0.x,D_FLOAT|D_RDONLY},
	{"eigenvector0[1]",&eigenvector0.y,D_FLOAT|D_RDONLY},
	{"eigenvalue1",&eigenvalue1,D_FLOAT|D_RDONLY},
	{"eigenvector1[0]",&eigenvector1.x,D_FLOAT|D_RDONLY},
	{"eigenvector1[1]",&eigenvector1.y,D_FLOAT|D_RDONLY},
	{"inVec[0]",&inVec.x,D_FLOAT,FLOATUP/8},
	{"inVec[1]",&inVec.y,D_FLOAT,FLOATUP/8},
	{"outVec[0]",&outVec.x,D_FLOAT|D_RDONLY},
	{"outVec[1]",&outVec.y,D_FLOAT|D_RDONLY},
	{"@lightgreen@--- QUADRATIC TESTS all complex ---",NULL,D_VOID,0},
	{"A.r",&((pointf2*)(&ccoa))->x,D_FLOAT,FLOATUP/8},
	{"A.i",&((pointf2*)(&ccoa))->y,D_FLOAT,FLOATUP/8},
	{"B.r",&((pointf2*)(&ccob))->x,D_FLOAT,FLOATUP/8},
	{"B.i",&((pointf2*)(&ccob))->y,D_FLOAT,FLOATUP/8},
	{"C.r",&((pointf2*)(&ccoc))->x,D_FLOAT,FLOATUP/8},
	{"C.i",&((pointf2*)(&ccoc))->y,D_FLOAT,FLOATUP/8},
	{"R0.r",&((pointf2*)(&ccr0))->x,D_FLOAT|D_RDONLY},
	{"R0.i",&((pointf2*)(&ccr0))->y,D_FLOAT|D_RDONLY},
	{"R1.r",&((pointf2*)(&ccr1))->x,D_FLOAT|D_RDONLY},
	{"R1.i",&((pointf2*)(&ccr1))->y,D_FLOAT|D_RDONLY},
	{"@green@--- QUADRATIC TESTS coefs real roots complex ---",NULL,D_VOID,0},
	{"A",&coa,D_FLOAT,FLOATUP/8},
	{"B",&cob,D_FLOAT,FLOATUP/8},
	{"C",&coc,D_FLOAT,FLOATUP/8},
	{"result",&result1,D_INT|D_RDONLY},
	{"R0.r",&((pointf2*)(&cr0))->x,D_FLOAT|D_RDONLY},
	{"R0.i",&((pointf2*)(&cr0))->y,D_FLOAT|D_RDONLY},
	{"R1.r",&((pointf2*)(&cr1))->x,D_FLOAT|D_RDONLY},
	{"R1.i",&((pointf2*)(&cr1))->y,D_FLOAT|D_RDONLY},
	{"@lightgreen@--- QUADRATIC TESTS all real---",NULL,D_VOID,0},
	{"A",&coa,D_FLOAT,FLOATUP/8},
	{"B",&cob,D_FLOAT,FLOATUP/8},
	{"C",&coc,D_FLOAT,FLOATUP/8},
	{"result",&result2,D_INT|D_RDONLY},
	{"R0",&r0,D_FLOAT|D_RDONLY},
	{"R1",&r1,D_FLOAT|D_RDONLY},
};
const int neigendv = NUMELEMENTS(eigendv);

void initgrid(pointf2 grid[][gridsx])
{
	// initialize input grid
	U32 i,j;
	for (j=0;j<gridsy;++j) {
		for (i=0;i<gridsx;++i) {
			grid[j][i] = pointf2x(i/(gridsx - 1.0f),j/(gridsy - 1.0f));
		}
	}
}

void xformgrid(const mat2* M,const pointf2 igrid[][gridsx],pointf2 ogrid[][gridsx])
{
	U32 i,p = gridsx*gridsy;
	for (i=0;i<p;++i) {
		//grid[j][i] = pointf2x(i/(gridsx - 1.0f),j/(gridsy - 1.0f));
		mulmatvec2(M,&igrid[0][i],&ogrid[0][i]);
	}
}

void drawgrid(const pointf2 grid[][gridsx])
{
	U32 i,j;
	// draw some horizontal lines
	for (j=0;j<gridsy;++j) {
		for (i=0;i<gridsx-1;++i) {
			drawfline(grid[j][i],grid[j][i+1],C32RED);
		}
	}
	// draw some vertical lines
	for (j=0;j<gridsy-1;++j) {
		for (i=0;i<gridsx;++i) {
			drawfline(grid[j][i],grid[j+1][i],C32GREEN);
		}
	}
	// draw some points
	for (j=0;j<gridsy;++j) {
		for (i=0;i<gridsx;++i) {
			drawfpoint(grid[j][i],C32BLACK,4);
		}
	}
}

// assume det(A) == 0
void calceigenvectorfromvalue(float eval,pointf2& evec,mat2& me)
{
	static float NANI = PI; // place holder for any...
//#define TEST
#ifdef TEST
	eval = 0;
#endif
	me.e[0][0] = A.e[0][0] - eval;
	me.e[0][1] = A.e[0][1];
	me.e[1][0] = A.e[1][0];
	me.e[1][1] = A.e[1][1] - eval;
	float nonzero[2][2];
	nonzero[0][0] = abs(me.e[0][0]);
	nonzero[0][1] = abs(me.e[0][1]);
	nonzero[1][0] = abs(me.e[1][0]);
	nonzero[1][1] = abs(me.e[1][1]);
	// find highest number
	U32 i,j;
	float maxnum = 0;
	U32 bi = 0,bj = 0;
	for (j=0;j<2;++j) {
		for (i=0;i<2;++i) {
			if (nonzero[j][i] > maxnum) {
				maxnum = nonzero[j][i];
				bi = i;
				bj = j;
			}
		}
	}
	// if highest number is zero then all zeros
	if (maxnum <= EPSILON) {
		evec.x = NANI;
		evec.y = NANI;
		return;
	}
	// calc largest row
	float a = me.e[bj][bi]; // larger
	float b = me.e[1-bj][bi]; // smaller, column major, same row
	float rat = b/a;
	if (bj == 0) {
		evec.x = -rat;
		evec.y = 1;
	} else {
		evec.x = 1;
		evec.y = -rat;
	}
   normalize2d(&evec,&evec);
}

void calceigen()
{
	static float NANI = expf(1.0f);
	float a = 1;
	float b = -(A.e[0][0] + A.e[1][1]);
	float c = A.e[0][0]*A.e[1][1] - A.e[1][0]*A.e[0][1];
	eigenreal = quadratic(a,b,c,eigenvalue0,eigenvalue1);
	if (eigenreal) {
		//eigenvector0 = pointf2x(4,5);
		//eigenvector1 = pointf2x(7,8);
		calceigenvectorfromvalue(eigenvalue0,eigenvector0,EM0);
		calceigenvectorfromvalue(eigenvalue1,eigenvector1,EM1);
	} else {
		eigenvalue0 = NANI;
		eigenvalue1 = NANI;
		eigenvector0.x = NANI;
		eigenvector0.y = NANI;
		eigenvector1.x = NANI;
		eigenvector1.y = NANI;
	}
}

#ifdef DONAMESPACE
} // end namespace eigentest
using namespace eigentest;
#endif

void plot2eigeninit()
{
	adddebvars("eigen",eigendv,neigendv);
	plotter2init();
	//lzoom = -2.5f;
	initgrid(ingrid);
}

void plot2eigenproc()
{
	// interact with graph paper
	plotter2proc();
	// calc the image
	mulmatvec2(&A,&inVec,&outVec);
	xformgrid(&A,ingrid,outgrid);
	calceigen();
	result1 = quadratic(coa,cob,coc,r0,r1);
	result2 = quadratic(coa,cob,coc,cr0,cr1);
	quadratic(ccoa,ccob,ccoc,ccr0,ccr1);
}

void plot2eigendraw2d()
{
	// draw graph paper
	plotter2draw2d();
	// draw the transformed grid
	drawgrid(outgrid);
	// draw eigen
	if (eigenreal) {
		drawfline(pointf2x(),pointf2x(eigenvalue0*eigenvector0.x,eigenvalue0*eigenvector0.y),C32LIGHTRED);
		drawfline(pointf2x(),pointf2x(eigenvalue1*eigenvector1.x,eigenvalue1*eigenvector1.y),C32LIGHTGREEN);
		drawfcircle(pointf2x(0,0),C32LIGHTRED,S32(math2screen(abs(eigenvalue0))));
		drawfcircle(pointf2x(0,0),C32LIGHTGREEN,S32(math2screen(abs(eigenvalue1))));
	}
	drawfline(pointf2x(),inVec,C32BLUE);
	drawfline(pointf2x(),outVec,C32LIGHTBLUE);
}

void plot2eigenexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("eigen");
}
