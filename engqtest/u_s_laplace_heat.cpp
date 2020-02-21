// simple tests and one time utils
#include <m_eng.h>
#include "u_states.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace laplaceheat {
#endif

const U32 xdim = 100;
const U32 ydim = 100;
const U32 rad = 2;
const U32 space = 7;
const U32 xoff = 50;
const U32 yoff = 50;
float larr[ydim][xdim];
float larrlast[ydim][xdim];

void boundaryarray()
{
	larr[5][33] = 1;
	larr[55][33] = 1;
	larr[30][8] = 1;
	larr[30][58] = 1;
	larr[5][8] = 0;
	larr[5][58] = 0;
	larr[55][8] = 0;
	larr[55][58] = 0;
}

void initarray()
{
	U32 i,j;
	for (j=0;j<ydim;++j) {
		for (i=0;i<xdim;++i) {
			larr[j][i] = .5f;
		}
	}
	boundaryarray();
}

C32 convertcolor(float in)
{
	in = range(0.0f,in,1.0f);
	U8 v = U8(255*in);
	if ((v&4) == 0)
		return C32(v,0,0);
	return C32(v,v,v);
}

void drawarray()
{
	U32 i,j;
	for (j=0;j<ydim;++j) {
		for (i=0;i<xdim;++i) {
			float v = larrlast[j][i];
			C32 cv = convertcolor(v);
			clipcircle32(B32,xoff + space*i,yoff + space*j,rad,cv);
		}
	}
}

void procarray()
{
	//copy(&larr[0][0],&larr[ydim][xdim],&larrlast[0][0]); // crashes in VS 2005 release
/*	U32 i,j;
	for (j=0;j<ydim;++j) {
		for (i=0;i<xdim;++i) {
			larrlast[j][i] = larr[j][i];
		}
	} */
	memcpy(larrlast,larr,ydim*xdim*sizeof(larr[0][0]));
	// do a laplace step
	U32 i,j;
	for (j=0;j<ydim;++j) {
		for (i=0;i<xdim;++i) {
			float sum = 0;
			U32 dsum = 0;
			// top
			if (j > 0) {
				sum = larrlast[j-1][i];
				++dsum;
			}
			// bottom
			if (j + 1 < ydim) {
				sum += larrlast[j+1][i];
				++dsum;
			}
			// left
			if (i > 0) {
				sum += larrlast[j][i-1];
				++dsum;
			}
			// right
			if (i + 1 < ydim) {
				sum += larrlast[j][i+1];
				++dsum;
			}
			larr[j][i] = sum/dsum;
		}
	}
	boundaryarray();
}

#ifdef DONAMESPACE
} // end namespace qcomp

using namespace laplaceheat;
#endif

void laplaceinit()
{
	video_setupwindow(1024,768);
	initarray();
}

void laplaceproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='r')
		changestate(STATE_LAPLACE);
	const U32 speedup = 100;
	for (U32 i = 0;i<speedup;++i)
		procarray();
}

void laplacedraw2d()
{
	clipclear32(B32,C32(0,0,255)); // blue
	drawarray();
}

void laplaceexit()
{
	// no resources to free
}
