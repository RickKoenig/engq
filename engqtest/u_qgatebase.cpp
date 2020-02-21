#include <m_eng.h>
//#include <l_misclibm.h>

//#include "u_states.h"

#include "u_qstate.h"
#include "u_qmat.h"

#include "u_qgatebase.h"
#include "u_qgatederived.h"
#include "u_qcolumn.h" // for width of gate
//#include "u_qfieldui.h"

const float qgatebase::boxsmaller = 6;
// array of class create methods (for factory)
const qgatebase::createQGateFun qgatebase::qgateFactory[qgatebase::QTYPEENUM] = {
	// 1 qubit gates
	qgatebase::create,
	qgatehadamard::create,
	qgatenot::create,
	qgatepx::create,
	qgatepy::create,
	qgatepz::create,

	qgates::create,
	qgatest::create,
	qgatet::create,
	qgatett::create,
	qgatesnotto::create,
	qgatesnotwp::create,
	qgateg1over3::create,
	qgategm1over3::create,

	qgatesplitter::create,
	qgatemirror::create,
	qgatep45::create,
	qgatepm45::create,
	qgatep30::create,
	qgatepm30::create,

	qgatem0::create,
	qgatem1::create,

	// 2 qubit gates
	swaptop::create,
	swapbot::create,
	cnottop::create,
	cnotbot::create,
	cztop::create,
	czbot::create,
	cytop::create,
	cybot::create,
	chtop::create,
	chbot::create,
	cstop::create,
	csbot::create,
	csttop::create,
	cstbot::create,
	cttop::create,
	ctbot::create,
	ctttop::create,
	cttbot::create,
	cp45top::create,
	cp45bot::create,
	cpm45top::create,
	cpm45bot::create,
	cp30top::create,
	cp30bot::create,
	cpm30top::create,
	cpm30bot::create,
	dectop::create,
	decbot::create,

	// 3 qubit gates
	fredkintop::create,
	fredkinmid::create,
	fredkinbot::create,
	toffolitop::create,
	toffolimid::create,
	toffolibot::create,
	// 4 qubit gates
	toffoli4c0::create,
	toffoli4c1::create,
	toffoli4c2::create,
	toffoli4t::create,
	// 5 qubit gates
	toffoli5c0::create,
	toffoli5c1::create,
	toffoli5c2::create,
	toffoli5c3::create,
	toffoli5t::create,
	// 6 qubit gates
	toffoli6c0::create,
	toffoli6c1::create,
	toffoli6c2::create,
	toffoli6c3::create,
	toffoli6c4::create,
	toffoli6t::create,
};

const C8* qgatebase::qgnames[qgatebase::QTYPEENUM] = {
	// 1 qubit gates
	"NONE",
	"HADAMARD",
	"NOT (PX)",
	"PX (NOT)",
	"PY",
	"PZ",
	"S",
	"St",
	"T",
	"Tt",
	"SNOT TWOAK",
	"SNOT WIKIPEDIA",
	"G1Over3",
	"GM1Over3",
	"SPLITTER",
	"MIRROR",
	"P 45",
	"P -45",
	"P 30",
	"P -30",

	"M0", // measurement gates
	"M1",

	// 2 qubit gates
	"SWAP TOP",
	"SWAP BOTTOM",
	"CNOT TOP",
	"CNOT BOTTOM",
	"CZ TOP",
	"CZ BOTTOM",
	"CY TOP",
	"CY BOTTOM",
	"CH TOP",
	"CH BOTTOM",
	"CS TOP",
	"CS BOTTOM",
	"CSt TOP",
	"CSt BOTTOM",
	"CT TOP",
	"CT BOTTOM",
	"CTt TOP",
	"CTt BOTTOM",
	"CP 45 TOP",
	"CP 45 BOTTOM",
	"CP -45 TOP",
	"CP -45 BOTTOM",
	"CP 30 TOP",
	"CP 30 BOTTOM",
	"CP -30 TOP",
	"CP -30 BOTTOM",
	"DEC TOP",
	"DEC BOTTOM",
	// 3 qubit gates
	"FREDKIN TOP",
	"FREDKIN MIDDLE",
	"FREDKIN BOTTOM",
	"TOFFOLI TOP",
	"TOFFOLI MIDDLE",
	"TOFFOLI BOTTOM",
	// 4 qubit gates
	"TOFFOLI 4C0",
	"TOFFOLI 4C1",
	"TOFFOLI 4C2",
	"TOFFOLI 4T",
	// 5 qubit gates
	"TOFFOLI 5C0",
	"TOFFOLI 5C1",
	"TOFFOLI 5C2",
	"TOFFOLI 5C3",
	"TOFFOLI 5T",
	// 6 qubit gates
	"TOFFOLI 6C0",
	"TOFFOLI 6C1",
	"TOFFOLI 6C2",
	"TOFFOLI 6C3",
	"TOFFOLI 6C4",
	"TOFFOLI 6T",
};

// primitive draw calls
#if 1
void qgatebase::drawoutline(bitmap32* bm,U32 xoff,U32 yoff) const
{
	//return;
	// draw std outline
	cliprecto32(bm,xoff,yoff,qcolumn::cpixwid,gpixhit,bordercolor);
	outtextxyf32(bm,xoff + 2,yoff + 2,C32BLACK,"%d",id);
	outtextxyf32(bm,xoff - 2 + qcolumn::cpixwid - 24,yoff  + 2,C32BLACK,"%3d",nyoff);
}

void qgatebase::drawpassthru(bitmap32* bm,U32 xoff,U32 yoff) const
{
	//return;
	// thick horizontal line (wire, no gate)
	cliphline32(bm,xoff,yoff+gpixhit/2    ,xoff+qcolumn::cpixwid-1,drawcolor);
	cliphline32(bm,xoff,yoff+gpixhit/2 + 1,xoff+qcolumn::cpixwid-1,drawcolor);
}
#endif
void qgatebase::drawbox(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw a box
	U32 drawinx = (U32)(qcolumn::cpixwid/boxsmaller); // make box smaller than whole gate size
	U32 drawiny = (U32)(gpixhit/boxsmaller); // make box smaller than whole gate size
	cliprect32(bm,xoff + drawinx,yoff + drawiny,qcolumn::cpixwid - 2*drawinx,gpixhit - 2*drawiny,C32WHITE);
	cliprecto32(bm,xoff + drawinx,yoff + drawiny,qcolumn::cpixwid - 2*drawinx,gpixhit - 2*drawiny,drawcolor);
	cliprecto32(bm,xoff + drawinx - 1,yoff + drawiny - 1,qcolumn::cpixwid - 2*(drawinx - 1),gpixhit - 2*(drawiny - 1),drawcolor);
}

void qgatebase::drawtext(bitmap32* bm,U32 xoff,U32 yoff,const string& text) const
{
	// centered
	video3dinfo.largefont->outtextxy32(bm,
		xoff + qcolumn::cpixwid/2 - text.size()*video3dinfo.largefont->gx/2,
		yoff + gpixhit/2 - video3dinfo.largefont->gy/2,
		C32BLACK,text.c_str());
}

void qgatebase::drawcircleoutline(bitmap32* bm,U32 xoff,U32 yoff) const
{
	U32 drawmin = (U32)(qcolumn::cpixwid/boxsmaller); // make circle smaller than whole gate size
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + gpixhit/2;
	clipcircleo32(bm,xc,yc,drawmin,drawcolor);
	clipcircleo32(bm,xc,yc,drawmin+1,drawcolor);
}
#if 0
 // draw down to bottom half of gate
void qgatebase::drawconnectdown(U32 xoff,U32 yoff) const
{
	// draw a line down to bottom
	U32 xoffc = xoff + qcolumn::cpixwid/2;
	U32 yoffc = yoff + gpixhit/2;
	clipline32(noB32,xoffc,yoffc,xoffc,yoffc+gpixhit/2,drawcolor);
	clipline32(noB32,xoffc+1,yoffc,xoffc+1,yoffc+gpixhit/2,drawcolor);
}

// draw up to top half of gate
void qgatebase::drawconnectup(U32 xoff,U32 yoff) const
{
	// draw a line up to top
	U32 xoffc = xoff + qcolumn::cpixwid/2;
	U32 yoffc = yoff + gpixhit/2;
	clipline32(noB32,xoffc,yoffc,xoffc,yoffc-gpixhit/2,drawcolor);
	clipline32(noB32,xoffc+1,yoffc,xoffc+1,yoffc-gpixhit/2,drawcolor);
}
#endif
void qgatebase::drawconnect(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw a line down to bottom
	U32 xoffc = xoff + qcolumn::cpixwid/2;
	U32 yoffc = yoff + gpixhit/2;
	clipline32(bm,xoffc,yoffc,xoffc,yoffc+nyoff*gpixhit,drawcolor);
	clipline32(bm,xoffc+1,yoffc,xoffc+1,yoffc+nyoff*gpixhit,drawcolor);
}

void qgatebase::drawboxm(bitmap32* bm,U32 xoff,U32 yoff) const
{
	//return;
	// draw a box
	U32 drawinx = (U32)(qcolumn::cpixwid/boxsmaller/2); // make box smaller than whole gate size
	U32 drawiny = (U32)(gpixhit/boxsmaller/2); // make box smaller than whole gate size
	U32 x0 = xoff + drawinx;
	U32 y0 = yoff + drawiny;
	//U32 x1 = xoff + qcolumn::cpixwid - 1 - drawinx;
	//U32 y1 = yoff + gpixhit - 1 - drawiny;
	U32 wx = qcolumn::cpixwid - 2*drawinx;
	U32 wy = gpixhit - 2*drawiny;
	S32 ayo = abs(nyoff);
	wy += ayo*gpixhit;
	if (nyoff < 0)
		y0 += nyoff*gpixhit;

	//cliprect32(noB32,x0,y0,
	//	qcolumn::cpixwid - 2*drawinx,
	//	gpixhit - 2*drawiny,C32WHITE);
	cliprect32(bm,x0,y0,wx,wy,C32WHITE);
	cliprecto32(bm,x0,y0,wx,wy,drawcolor);
	cliprecto32(bm,x0+1,y0+1,wx-2,wy-2,drawcolor);
	/*clipline32(noB32,x0,y0,x0,y1,drawcolor);
	clipline32(noB32,x0,y0,x1,y0,drawcolor);
	clipline32(noB32,x1,y0,x1,y1,drawcolor);
	clipline32(noB32,x0+1,y0+1,x0+1,y1+1,drawcolor);
	clipline32(noB32,x0+1,y0+1,x1+1,y0+1,drawcolor);
	clipline32(noB32,x1+1,y0+1,x1+1,y1+1,drawcolor);*/
	//clipline32(noB32,xoff + drawinx,yoff + drawiny,xoff + qcolumn::cpixwid - drawinx,yoff + gpixhit - drawiny,C32BLACK);
	//clipline32(noB32,0,0,0,0,C32BLACK);
	//clipline32(noB32,0,0,0,0,C32BLACK);
	//cliprecto32(noB32,xoff + drawinx,yoff + drawiny,qcolumn::cpixwid - 2*drawinx,gpixhit - 2*drawiny,C32BLACK);
	//cliprecto32(noB32,xoff + drawinx - 1,yoff + drawiny - 1,qcolumn::cpixwid - 2*(drawinx - 1),gpixhit - 2*(drawiny - 1),C32BLACK);
}

#if 0
void qgatebase::drawboxdown(U32 xoff,U32 yoff) const
{
	//return;
	// draw a box
	U32 drawinx = qcolumn::cpixwid/boxsmaller/2; // make box smaller than whole gate size
	U32 drawiny = gpixhit/boxsmaller/2; // make box smaller than whole gate size
	U32 x0 = xoff + drawinx;
	U32 y0 = yoff + drawiny;
	U32 x1 = xoff + qcolumn::cpixwid - drawinx;
	U32 y1 = yoff + gpixhit - 1;// - drawiny;
	cliprect32(noB32,x0,y0,
		qcolumn::cpixwid - 2*drawinx,
		gpixhit - drawiny,C32WHITE);
	clipline32(noB32,x0,y0,x0,y1,drawcolor);
	clipline32(noB32,x0,y0,x1,y0,drawcolor);
	clipline32(noB32,x1,y0,x1,y1,drawcolor);
	clipline32(noB32,x0+1,y0+1,x0+1,y1+1,drawcolor);
	clipline32(noB32,x0+1,y0+1,x1+1,y0+1,drawcolor);
	clipline32(noB32,x1+1,y0+1,x1+1,y1+1,drawcolor);
	//clipline32(noB32,xoff + drawinx,yoff + drawiny,xoff + qcolumn::cpixwid - drawinx,yoff + gpixhit - drawiny,C32BLACK);
	//clipline32(noB32,0,0,0,0,C32BLACK);
	//clipline32(noB32,0,0,0,0,C32BLACK);
	//cliprecto32(noB32,xoff + drawinx,yoff + drawiny,qcolumn::cpixwid - 2*drawinx,gpixhit - 2*drawiny,C32BLACK);
	//cliprecto32(noB32,xoff + drawinx - 1,yoff + drawiny - 1,qcolumn::cpixwid - 2*(drawinx - 1),gpixhit - 2*(drawiny - 1),C32BLACK);
}

void qgatebase::drawboxup(U32 xoff,U32 yoff) const
{
	//return;
	// draw a box
	U32 drawinx = qcolumn::cpixwid/boxsmaller/2; // make box smaller than whole gate size
	U32 drawiny = gpixhit/boxsmaller/2; // make box smaller than whole gate size
	U32 x0 = xoff + drawinx;
	U32 y0 = yoff;// + drawiny;
	U32 x1 = xoff + qcolumn::cpixwid - drawinx;
	U32 y1 = yoff + gpixhit - 1 - drawiny;// - drawiny;
	cliprect32(noB32,x0,y0,
		qcolumn::cpixwid - 2*drawinx,
		gpixhit - drawiny,C32WHITE);
	clipline32(noB32,x0,y0,x0,y1,drawcolor);
	clipline32(noB32,x0,y1,x1,y1,drawcolor);
	clipline32(noB32,x1,y0,x1,y1,drawcolor);

	clipline32(noB32,x0+1,y0+1,x0+1,y1+1,drawcolor);
	clipline32(noB32,x0+1,y1+1,x1+1,y1+1,drawcolor);
	clipline32(noB32,x1+1,y0+1,x1+1,y1+1,drawcolor);

#if 0


	clipline32(noB32,x0+1,y0,x0+1,y1,drawcolor);
	clipline32(noB32,x0+1,y1,x1+1,y1,drawcolor);
	clipline32(noB32,x1+1,y0,x1+1,y1,drawcolor);

	clipline32(noB32,x0,y0+1,x0,y1+1,drawcolor);
	clipline32(noB32,x0,y1+1,x1,y1+1,drawcolor);
	clipline32(noB32,x1,y0+1,x1,y1+1,drawcolor);
#endif
	//clipline32(noB32,xoff + drawinx,yoff + drawiny,xoff + qcolumn::cpixwid - drawinx,yoff + gpixhit - drawiny,C32BLACK);
	//clipline32(noB32,0,0,0,0,C32BLACK);
	//clipline32(noB32,0,0,0,0,C32BLACK);
	//cliprecto32(noB32,xoff + drawinx,yoff + drawiny,qcolumn::cpixwid - 2*drawinx,gpixhit - 2*drawiny,C32BLACK);
	//cliprecto32(noB32,xoff + drawinx - 1,yoff + drawiny - 1,qcolumn::cpixwid - 2*(drawinx - 1),gpixhit - 2*(drawiny - 1),C32BLACK);
}
#endif
// draw PASSTHRU qgatebase base class
void qgatebase::draw(bitmap32* bm,U32 xoff,U32 yoff) const
{
	// draw std outline
	//drawoutline(xoff,yoff);
	// do a double draw of a horizontal line across the center
	//drawpassthru(xoff,yoff);
}


qmat qgatebase::getqmat() const
{
	qmat qm;
	qm.ident1();
	return qm;
}

const U32 bc = 250; // almost white
C32 qgatebase::bordercolor(bc,bc,bc);
