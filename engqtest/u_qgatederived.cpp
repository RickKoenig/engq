// all the derived classes from class qgatebase

#include <m_eng.h>

#include "u_qstate.h"
#include "u_qmat.h"

#include "u_qgatebase.h"
#include "u_qgatederived.h"
#include "u_qcolumn.h" // for width of gate
//#include "u_qfieldui.h"

// draw HADAMARD
void qgatehadamard::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big H
	drawtext(bm,xoff,yoff,"H");
	//clipclear32(bm,C32GREEN);
}

qmat qgatehadamard::getqmat() const
{
	qmat qm;
	qm.hadamard();
	return qm;
}

// draw M0
void qgatem0::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big H
	drawtext(bm,xoff+1,yoff,"M0");
	//clipclear32(bm,C32GREEN);
}

qmat qgatem0::getqmat() const
{
	qmat qm;
	qm.m0();
	return qm;
}

// draw M1
void qgatem1::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big H
	drawtext(bm,xoff+4,yoff,"M1");
	//clipclear32(bm,C32GREEN);
}

qmat qgatem1::getqmat() const
{
	qmat qm;
	qm.m1();
	return qm;
}

// draw SPLITTER
void qgatesplitter::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the SN
	drawbox(bm,xoff,yoff);
	// draw a diagonal line
	U32 drawminx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawminy = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	U32 x0 = xoff + drawminx;
	U32 y0 = yoff + drawminy;
	U32 x1 = xoff + qcolumn::cpixwid - 1 - drawminx;
	U32 y1 = yoff + qgatebase::gpixhit - 1 - drawminy;
	// thick line
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	clipline32(bm,x0,y0+1,x1,y1+1,drawcolor);
	clipline32(bm,x0+1,y0+1,x1+1,y1+1,drawcolor);
	outtextxyf32(bm,x0+4,y1-8-4,C32BLACK,"S");
}

qmat qgatesplitter::getqmat() const
{
	qmat qm;
	qm.splitter();
	return qm;
}

// draw MIRROR
void qgatemirror::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the SN
	//drawbox(bm,xoff,yoff);
	// draw a diagonal line
	U32 drawminx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawminy = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	U32 x0 = xoff + drawminx;
	U32 y0 = yoff + drawminy;
	U32 x1 = xoff + qcolumn::cpixwid - 1 - drawminx;
	U32 y1 = yoff + qgatebase::gpixhit - 1 - drawminy;
	// thick line
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	clipline32(bm,x0,y0+1,x1,y1+1,drawcolor);
	clipline32(bm,x0+1,y0+1,x1+1,y1+1,drawcolor);
	outtextxyf32(bm,x0+4,y1-8-4,C32BLACK,"M");
#if 0
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the SN
	drawbox(bm,xoff,yoff);
	// draw a diagonal line
	U32 drawminx = qcolumn::cpixwid/boxsmaller; // make box smaller than whole gate size
	U32 drawminy = gpixhit/boxsmaller; // make box smaller than whole gate size
	U32 x0 = xoff + drawminx;
	U32 y0 = yoff + drawminy;
	U32 x1 = xoff + qcolumn::cpixwid - 1 - drawminx;
	U32 y1 = yoff + qgatebase::gpixhit - 1 - drawminy;
	// thick line
	//clipline32(noB32,x0,y0,x1,y1,drawcolor);
	//clipline32(noB32,x0+1,y0,x1+1,y1,drawcolor);
	//clipline32(noB32,x0,y0+1,x1,y1+1,drawcolor);
	//clipline32(noB32,x0+1,y0+1,x1+1,y1+1,drawcolor);
	U32 xleft = xoff + drawminx;
	U32 ytop = yoff + drawminy;
	outtextxyf32(noB32,xleft+2+8,ytop+3+4,C32BLACK,"MIR");
	outtextxyf32(noB32,xleft+2+8,ytop+3+16+4,C32BLACK,"ROR");
#endif
}

qmat qgatemirror::getqmat() const
{
	qmat qm;
	qm.mirror();
	return qm;
}

// draw SNOT twisted oak
// one thin pass
void qgatesnotto::drawthinsnot(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw a sqrt with a little not symbol, sideways L
	U32 drawminx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawminy = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	U32 xleft = xoff + drawminx;
	U32 ytop = yoff + drawminy;
	U32 xright = xoff + qcolumn::cpixwid - 1 - drawminx;
	U32 ybot = yoff + qgatebase::gpixhit - 1 - drawminy;
	// lines to build a thin SNOT gate
	static pointf2 lines[] = {
		// sqrt symbol
		{ .25f,  .5f},
		{.375f, .75f},

		{.375f, .75f},
		{  .5f, .35f},

		{.5f,  .35f},
		{.875f,.35f},

		// 'L' symbol

		{.625f,  .5f},
		{.875f,  .5f},

		{.875f,  .5f},
		{.875f,.625f},

	};
	U32 i;
	for (i=0;i<NUMELEMENTS(lines);i+=2) {
		const pointf2& l0 = lines[i];
		const pointf2& l1 = lines[i+1];
		U32 x0 = U32(xleft + l0.x*(xright - xleft));
		U32 y0 = U32(ytop  + l0.y*(ybot   - ytop ));
		U32 x1 = U32(xleft + l1.x*(xright - xleft));
		U32 y1 = U32(ytop  + l1.y*(ybot   - ytop ));
		clipline32(bm,x0,y0,x1,y1,C32BLACK);
	}
}

qmat qgatesnotto::getqmat() const
{
	qmat qm;
	qm.snotto();
	return qm;
}

void qgatesnotto::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a thick
	drawbox(bm,xoff,yoff);
	drawthinsnot(bm,xoff,yoff);
	drawthinsnot(bm,xoff+1,yoff);
	drawthinsnot(bm,xoff,yoff+1);
	drawthinsnot(bm,xoff+1,yoff+1);
	U32 drawminx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawminy = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	U32 xleft = xoff + drawminx;
	U32 ytop = yoff + drawminy;
	outtextxyf32(bm,xleft+2,ytop+3,C32BLACK,"T.O.");
}

// draw SNOT wikipedia
// one thin pass
void qgatesnotwp::drawthinsnot(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw a sqrt with a little not symbol, sideways L
	U32 drawminx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawminy = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	U32 xleft = xoff + drawminx;
	U32 ytop = yoff + drawminy;
	U32 xright = xoff + qcolumn::cpixwid - 1 - drawminx;
	U32 ybot = yoff + qgatebase::gpixhit - 1 - drawminy;
	// lines to build a thin SNOT gate
	static pointf2 lines[] = {
		// sqrt symbol
		{ .25f,  .5f},
		{.375f, .75f},

		{.375f, .75f},
		{  .5f, .35f},

		{.5f,  .35f},
		{.875f,.35f},

		// 'L' symbol

		{.625f,  .5f},
		{.875f,  .5f},

		{.875f,  .5f},
		{.875f,.625f},

	};
	U32 i;
	for (i=0;i<NUMELEMENTS(lines);i+=2) {
		const pointf2& l0 = lines[i];
		const pointf2& l1 = lines[i+1];
		U32 x0 = U32(xleft + l0.x*(xright - xleft));
		U32 y0 = U32(ytop  + l0.y*(ybot   - ytop ));
		U32 x1 = U32(xleft + l1.x*(xright - xleft));
		U32 y1 = U32(ytop  + l1.y*(ybot   - ytop ));
		clipline32(bm,x0,y0,x1,y1,C32BLACK);
	}
}

void qgatesnotwp::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a thick
	drawbox(bm,xoff,yoff);
	drawthinsnot(bm,xoff,yoff);
	drawthinsnot(bm,xoff+1,yoff);
	drawthinsnot(bm,xoff,yoff+1);
	drawthinsnot(bm,xoff+1,yoff+1);
	U32 drawminx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawminy = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	U32 xleft = xoff + drawminx;
	U32 ytop = yoff + drawminy;
	outtextxyf32(bm,xleft+2,ytop+3,C32BLACK,"W.P.");
}

qmat qgatesnotwp::getqmat() const
{
	qmat qm;
	qm.snotwp();
	return qm;
}

void qgatenot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	drawcircleoutline(bm,xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
}

qmat qgatenot::getqmat() const
{
	qmat qm;
	qm.not1();
	return qm;
}

void qgatepx::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PX
	drawtext(bm,xoff,yoff,"PX");
}

qmat qgatepx::getqmat() const
{
	qmat qm;
	qm.pauliX();
	return qm;
}

void qgatepy::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PY
	drawtext(bm,xoff,yoff,"PY");
}

qmat qgatepy::getqmat() const
{
	qmat qm;
	qm.pauliY();
	return qm;
}

void qgatepz::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"PZ");
}

qmat qgatepz::getqmat() const
{
	qmat qm;
	qm.pauliZ();
	return qm;
}

void qgates::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"S");
}

qmat qgates::getqmat() const
{
	qmat qm;
	qm.S();
	return qm;
}

void qgatest::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"St");
}

qmat qgatest::getqmat() const
{
	qmat qm;
	qm.St();
	return qm;
}

void qgatet::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"T");
}

qmat qgatet::getqmat() const
{
	qmat qm;
	qm.T();
	return qm;
}

void qgatett::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the H
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"Tt");
}

qmat qgatett::getqmat() const
{
	qmat qm;
	qm.Tt();
	return qm;
}

void qgateg1over3::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawbox(bm,xoff,yoff);

	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8/2 - 6,
		C32BLACK,"G");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 + 6,
		C32BLACK,"1/3");
}

qmat qgateg1over3::getqmat() const
{
	qmat qm;
	qm.g1over3();
	return qm;
}

void qgategm1over3::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawbox(bm,xoff,yoff);

	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8 - 6,
		C32BLACK,"G");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8 + 6,
		C32BLACK,"1/3");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8 + 20,
		C32BLACK,"t");
}

qmat qgategm1over3::getqmat() const
{
	qmat qm;
	qm.gm1over3();
	return qm;
}

void qgatep45::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawbox(bm,xoff,yoff);

	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 + 6,
		C32BLACK,"45");
}

qmat qgatep45::getqmat() const
{
	qmat qm;
	qm.p45();
	return qm;
}

void qgatepm45::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawbox(bm,xoff,yoff);

	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 + 6,
		C32BLACK,"-45");
}

qmat qgatepm45::getqmat() const
{
	qmat qm;
	qm.pm45();
	return qm;
}

void qgatep30::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawbox(bm,xoff,yoff);

	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 + 6,
		C32BLACK,"30");
}

qmat qgatep30::getqmat() const
{
	qmat qm;
	qm.p30();
	return qm;
}

void qgatepm30::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawbox(bm,xoff,yoff);

	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 + 6,
		C32BLACK,"-30");
}

qmat qgatepm30::getqmat() const
{
	qmat qm;
	qm.pm30();
	return qm;
}

////////// 2 qubit gates

void swaptop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw an X
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2 - halfline;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = xoff + qcolumn::cpixwid/2 + halfline;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	clipline32(bm,x0,y1,x1,y0,drawcolor);
	clipline32(bm,x0+1,y1,x1+1,y0,drawcolor);
#if 0
	// draw a line down to bottom
	U32 xoffc = xoff + qcolumn::cpixwid/2;
	U32 yoffc = yoff + gpixhit/2;
	clipline32(bm,xoffc,yoffc,xoffc,yoffc+gpixhit/2,C32BLACK);
	clipline32(bm,xoffc+1,yoffc,xoffc+1,yoffc+gpixhit/2,C32BLACK);
#else
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
#endif
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
}

qmat swaptop::getqmat() const
{
	qmat qm;
	qm.swap();
	return qm;
}

void swapbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw an X
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2 - halfline;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = xoff + qcolumn::cpixwid/2 + halfline;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	clipline32(bm,x0,y1,x1,y0,drawcolor);
	clipline32(bm,x0+1,y1,x1+1,y0,drawcolor);
#if 0
	// draw a line up to top
	U32 xoffc = xoff + qcolumn::cpixwid/2;
	U32 yoffc = yoff + gpixhit/2;
	clipline32(bm,xoffc,yoffc,xoffc,yoffc-gpixhit/2,C32BLACK);
	clipline32(bm,xoffc+1,yoffc,xoffc+1,yoffc-gpixhit/2,C32BLACK);
#else
	//drawconnectup(xoff,yoff);
#endif
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
}

void cnottop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
}

qmat cnottop::getqmat() const
{
	qmat qm;
	qm.cnot();
	//qm.toffoli(2);
	return qm;
}

void cnotbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	drawcircleoutline(bm,xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
}

void cztop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CZ");
}

qmat cztop::getqmat() const
{
	qmat qm;
	qm.cz();
	//qm.toffoli(2);
	return qm;
}

void czbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CZ");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"CZ");
}

void cytop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CY");
}

qmat cytop::getqmat() const
{
	qmat qm;
	qm.cy();
	//qm.toffoli(2);
	return qm;
}

void cybot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CY");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"CY");
}

void chtop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CH");
}

qmat chtop::getqmat() const
{
	qmat qm;
	qm.ch();
	//qm.toffoli(2);
	return qm;
}

void chbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CH");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"CH");
}






void cstop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CS");
}

qmat cstop::getqmat() const
{
	qmat qm;
	qm.cs();
	//qm.toffoli(2);
	return qm;
}

void csbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CS");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"CS");
}

void csttop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CSt");
}

qmat csttop::getqmat() const
{
	qmat qm;
	qm.cst();
	//qm.toffoli(2);
	return qm;
}

void cstbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CSt");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff - 4,"CS");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 + 14,
		C32BLACK,"t");
}

void cttop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CT");
}

qmat cttop::getqmat() const
{
	qmat qm;
	qm.ct();
	//qm.toffoli(2);
	return qm;
}

void ctbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CT");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff,"CT");
}

void ctttop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CTt");
}

qmat ctttop::getqmat() const
{
	qmat qm;
	qm.ctt();
	//qm.toffoli(2);
	return qm;
}

void cttbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CTt");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	drawtext(bm,xoff,yoff - 4,"CT");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 + 14,
		C32BLACK,"t");
}











void cp45top::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP45");
}

qmat cp45top::getqmat() const
{
	qmat qm;
	qm.cp45();
	//qm.toffoli(2);
	return qm;
}

void cp45bot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP45");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	//drawtext(bm,xoff,yoff,"CP45");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8/2 - 18 + 4,
		C32BLACK,"C");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 + 6 + 4,
		C32BLACK,"45");

}

void cpm45top::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP-45");
}

qmat cpm45top::getqmat() const
{
	qmat qm;
	qm.cpm45();
	//qm.toffoli(2);
	return qm;
}

void cpm45bot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP-45");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	//drawtext(bm,xoff,yoff,"CP45");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8/2 - 18 + 4,
		C32BLACK,"C");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 + 6 + 4,
		C32BLACK,"-45");

}

void cp30top::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP30");
}

qmat cp30top::getqmat() const
{
	qmat qm;
	qm.cp30();
	//qm.toffoli(2);
	return qm;
}

void cp30bot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP30");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	//drawtext(bm,xoff,yoff,"CP45");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8/2 - 18 + 4,
		C32BLACK,"C");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 + 6 + 4,
		C32BLACK,"30");
}

void cpm30top::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw solid small circle
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	//drawconnectdown(xoff,yoff);
	//drawconnect(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP-30");
}

qmat cpm30top::getqmat() const
{
	qmat qm;
	qm.cpm30();
	//qm.toffoli(2);
	return qm;
}

void cpm30bot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw thick circle
	//drawcircleoutline(xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	//drawconnectup(xoff,yoff);
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"CP-30");
	drawbox(bm,xoff,yoff);
	// draw a big PZ
	//drawtext(bm,xoff,yoff,"CP45");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 1*8/2,
		yoff + gpixhit/2 - 8/2 - 18 + 4,
		C32BLACK,"C");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4,
		C32BLACK,"Ph");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 + 6 + 4,
		C32BLACK,"-30");
}

void dectop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxm(xoff,yoff);
	// draw 'DEC' on the top side
	drawbox(bm,xoff,yoff);
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4 - 10,
		C32BLACK,"DEC");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4 + 10,
		C32BLACK,"HI");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
}

qmat dectop::getqmat() const
{
	qmat qm;
	qm.dec();
	return qm;
}

void decbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, upwards
	//drawboxup(xoff,yoff);
	// draw bottom text, nothing
	//drawtext(bm,xoff,yoff,".");
	drawbox(bm,xoff,yoff);
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 3*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4 - 10,
		C32BLACK,"DEC");
	outtextxyf32(bm,
		xoff + qcolumn::cpixwid/2 - 2*8/2,
		yoff + gpixhit/2 - 8/2 - 6 + 4 + 10,
		C32BLACK,"LO");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
}

void fredkintop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxdown(xoff,yoff);
	//drawboxm(xoff,yoff);
	// draw 'DEC' on the top side
	//drawtext(bm,xoff,yoff,"FRED");
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"FRED");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	//drawconnect(xoff,yoff);
}

qmat fredkintop::getqmat() const
{
	qmat qm;
	qm.fredkin3();
	return qm;
}

void fredkinmid::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxdown(xoff,yoff);
	// draw 'DEC' on the top side
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2 - halfline;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = xoff + qcolumn::cpixwid/2 + halfline;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	clipline32(bm,x0,y1,x1,y0,drawcolor);
	clipline32(bm,x0+1,y1,x1+1,y0,drawcolor);
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"FRED");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"M");
	//drawconnect(xoff,yoff);
}

void fredkinbot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxdown(xoff,yoff);
	// draw 'DEC' on the top side
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2 - halfline;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = xoff + qcolumn::cpixwid/2 + halfline;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	clipline32(bm,x0,y1,x1,y0,drawcolor);
	clipline32(bm,x0+1,y1,x1+1,y0,drawcolor);
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"FRED");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
}

void toffolitop::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxdown(xoff,yoff);
	//drawboxm(xoff,yoff);
	// draw 'DEC' on the top side
	//drawtext(bm,xoff,yoff,"TOFF");
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"T");
	//drawconnect(xoff,yoff);
}

qmat toffolitop::getqmat() const
{
	qmat qm;
	//qm.toffoli3();
	qm.toffoli(3);
	return qm;
}

void toffolimid::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxdown(xoff,yoff);
	// draw 'DEC' on the top side
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"M");
	//drawconnect(xoff,yoff);
}

void toffolibot::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
	// draw a box for the DEC, but going down for the bottom half
	//drawboxdown(xoff,yoff);
	// draw 'DEC' on the top side
	drawcircleoutline(bm,xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF");
	outtextxyf32(bm,xoff + 16,yoff + gpixhit - 24 + 2,C32GREEN,"B");
}

// TOFFOLI4
void toffoli4c0::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF4");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C0");
	//drawconnect(xoff,yoff);
}

qmat toffoli4c0::getqmat() const
{
	//qmat qm(1<<4);
	//qm.toffoli();
	qmat qm;
	//qm.toffoli4();
	qm.toffoli(4);
	return qm;
}

void toffoli4c1::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF4");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C1");
	//drawconnect(xoff,yoff);
}

void toffoli4c2::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF4");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C2");
	//drawconnect(xoff,yoff);
}

void toffoli4t::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawcircleoutline(bm,xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF4");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"TARG");
}

// TOFFOLI5
void toffoli5c0::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF4");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C0");
	//drawconnect(xoff,yoff);
}

qmat toffoli5c0::getqmat() const
{
	//qmat qm(1<<4);
	//qm.toffoli();
	qmat qm;
	//qm.toffoli5();
	qm.toffoli(5);
	return qm;
}

void toffoli5c1::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF5");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C1");
	//drawconnect(xoff,yoff);
}

void toffoli5c2::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF5");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C2");
	//drawconnect(xoff,yoff);
}

void toffoli5c3::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF5");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C3");
	//drawconnect(xoff,yoff);
}

void toffoli5t::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawcircleoutline(bm,xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF5");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"TARG");
}

// TOFFOLI6
void toffoli6c0::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF6");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C0");
	//drawconnect(xoff,yoff);
}

qmat toffoli6c0::getqmat() const
{
	//qmat qm(1<<4);
	//qm.toffoli();
	qmat qm;
	//qm.toffoli6();
	qm.toffoli(6);
	return qm;
}

void toffoli6c1::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF6");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C1");
	//drawconnect(xoff,yoff);
}

void toffoli6c2::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF6");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C2");
	//drawconnect(xoff,yoff);
}

void toffoli6c3::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF6");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C3");
	//drawconnect(xoff,yoff);
}

void toffoli6c4::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircle32(bm,xc,yc,qcolumn::cpixwid/10,drawcolor); // smaller solid control circle
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF6");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"C4");
	//drawconnect(xoff,yoff);
}

void toffoli6t::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	drawcircleoutline(bm,xoff,yoff);
	U32 halfline = (U32)(qcolumn::cpixwid/boxsmaller);
	U32 x0 = xoff + qcolumn::cpixwid/2;
	U32 y0 = yoff + gpixhit/2 - halfline;
	U32 x1 = x0;
	U32 y1 = yoff + gpixhit/2 + halfline;
	clipline32(bm,x0,y0,x1,y1,drawcolor);
	clipline32(bm,x0+1,y0,x1+1,y1,drawcolor);
	outtextxyf32(bm,xoff +2,yoff + 2 + 16,C32BLACK,"TOFF6");
	outtextxyf32(bm,xoff + 16 - 8,yoff + gpixhit - 24 + 2,C32GREEN,"TARG");
}

