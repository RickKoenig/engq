// test some multi view 2d stuff
#include <m_eng.h>
#include "u_states.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace holoview2d {
#endif

//#define BUILD4VIEWS // make a bunch of test 4 views

bitmap32* threatbm;
class EditPixels* ep;

// resolution for 1 view
const U32 VRESX = 640;
const U32 VRESY = 360;

// resolution for 2x2
const U32 TRESX = VRESX * 2;
const U32 TRESY = VRESY * 2;

bool fullScreen;

class FourView {
	bitmap32* fvbm;
public:
	FourView();
	void add(bitmap32* b,U32 vn);
	bitmap32* generate2x2();
	~FourView();
};

FourView::FourView()
{
	fvbm = bitmap32alloc(TRESX, TRESY, C32BLACK);
}

void FourView::add(bitmap32* b,U32 vn)
{
	U32 vx = vn & 1;
	U32 vy = vn >> 1;
	clipblit32(b, fvbm, 0, 0, vx * b->size.x, vy * b->size.y, b->size.x, b->size.y);
}

bitmap32* FourView::generate2x2()
{
	return bitmap32copy(fvbm);
}

FourView::~FourView()
{
	bitmap32free(fvbm);
}

// get a good filename for the taskView list
string testSaveName(const bool vt[])
{
	static C8 colorLetter[] = "RGBY";
	string fn = "";
	for (U32 i = 0; i < 4; ++i) {
		fn += vt[i] ? colorLetter[i] : 'o';
	}
	return fn;
}

// get a good FourView for the taskView list
void testMakeFourView(FourView& fv,const bool vt[])
{
	C32 colorValue[]{ C32RED,C32GREEN,C32BLUE,C32YELLOW };
	for (U32 i = 0; i < 4; ++i) {
		bool dt = vt[i];
		// only draw if tasked to do so
		if (!dt)
			continue;
		C32 cv = colorValue[i];
		bitmap32* tbm = bitmap32alloc(VRESX, VRESY, C32BLACK);
		// X
		const S32 rectWidth = 8;// VRESX - 2; // make even
		const S32 xs = rectWidth;
		S32 xo = (VRESX - xs) / 2;
		// Y
		const S32 rectHeightMinus = 10;
		S32 yo = rectHeightMinus;
		const S32 ys = VRESY - 2 * rectHeightMinus;
		cliprect32(tbm, xo, yo, xs, ys, cv);
		//cliprecto32(tbm, xo, yo, xs, ys, C32BLACK); // outline check
		fv.add(tbm, i);
		bitmap32free(tbm);
	}
}

// build a test 4view of 'tn' kind, (16 kinds)
void buildAndSaveAFourView(U32 tn)
{
	logger("buildAFourView %d\n", tn);

	// make a viewTask, what views from 'tn', tasks for each view given task number 'tn'
	// basically converting to binary
	bool viewTask[4];
	for (U32 j = 0; j < 4; ++j)
		viewTask[j] = ((tn >> j) & 1) != 0;

	// make a filename to save to
	string fn = "testView_";
	fn += testSaveName(viewTask);
	fn += "_2x2.png";
	logger("string = '%s'\n\n", fn.c_str());

	// build the four views and write it out
	FourView fv;
	testMakeFourView(fv,viewTask);
	bitmap32* fvOut = fv.generate2x2();
	gfxwrite32(fn.c_str(), fvOut);
	bitmap32free(fvOut);

}

// build some test views, 16 for now
void buildFourViews()
{
	for (U32 tn = 0; tn < 16; ++tn) {
		buildAndSaveAFourView(tn); // which 4view to build
	}
}






//////////////// edit pixels class
U32 pow2Multiple(U32 in)
{
	if (in == 0)
		return 1 << 31;
	U32 ret = 1;
	while ((in & 1) == 0) {
		in >>= 1;
		ret <<= 1;
	}
	return ret;
}

class EditPixels {
	// members
	const pointi2 dataSize{ 32,32 };
	bitmap32* pixelData;
	const pointi2 editSquaresSize{ 16,16 };
	const pointi2 colorSelectSquaresSize{ 48,48 };
	pointi2 hilitEdit{ 0,0 };
	S32 hilitColorSelect{ 0 };
	static S32 butColorSelect[3];// { 0, 2, 7 }; // L M R
	C8* butColorSelectNames[3] {"L","M","R" };
	C32 selectColors[8];
	pointi2 smallOffset{ WX / 4,WY / 2 };
	pointi2 editOffset{ WX/2,WY/4};
	pointi2 colorSelectOffset{ WX * 5 / 8 , WY / 16 };
	const C8* loadName{ "EditPixels.png" };
	const C8* saveName{ "EditPixels.png" };
	S32 UI_CHANGE_DELAY = 1; // wait 1 frame for the mouse to settle from changes
	S32 mDelay; // wait a bit for the mouse to settle between change window size actions
	// methods
	void drawSmall() const;
	void drawBig() const;
	void drawSelectColor() const;
	void procBig();
	void procSelectColor();
	void load(const C8* loadName);
	void save(const C8* saveName);
public:
	EditPixels();
	void proc();
	void draw() const;
	~EditPixels();
	pointi2 getEditHilit() const { return hilitEdit; }
};

//pointi2 EditPixels::hilitEdit;
//S32 EditPixels::hilitColorSelect;
S32 EditPixels::butColorSelect[3] { 0, 2, 7 }; // L M R

EditPixels::EditPixels() : mDelay{ 0 }
{
	// build select colors
	for (U32 i = 0; i < 8; ++i) {
		C32& sc = selectColors[i];
		sc = 0xff000000; // A000, ARGB
		for (U32 j = 0; j < 3; ++j) {
			U32 jp = 1 << j;
			if (jp & i) {
				sc.c32 += (0xff << (8*(2 - j)));
			}
		}
	}

	hilitEdit = { 0,0 };

	// get pixelData ready
	pixelData = bitmap32alloc(dataSize.x, dataSize.y,C32LIGHTCYAN);
	load(loadName);

	// align offset to some multiple
	const S32 leastMultiple = 64; // alignment, maybe a power of 2
	logger("SMALL OFFSET = ( %d by %d ) in PIXELS\n", smallOffset.x, smallOffset.y);
	pointi2 multipleOf{ (S32)pow2Multiple(smallOffset.x),(S32)pow2Multiple(smallOffset.y) };
	logger("SMALL OFFSET MULTIPLE OF = ( %d by %d ) in PIXELS\n", multipleOf.x, multipleOf.y);
	if (multipleOf.x < leastMultiple) {
		smallOffset.x &= ~(leastMultiple - 1);
		smallOffset.x += leastMultiple;
	}
	if (multipleOf.y < leastMultiple) {
		smallOffset.y &= ~(leastMultiple - 1);
		smallOffset.y += leastMultiple;
	}
	logger("new SMALL OFFSET = ( %d by %d ) in PIXELS\n", smallOffset.x, smallOffset.y);
	multipleOf = { (S32)pow2Multiple(smallOffset.x),(S32)pow2Multiple(smallOffset.y) };
	logger("new SMALL OFFSET MULTIPLE OF = ( %d by %d ) in PIXELS\n", multipleOf.x, multipleOf.y);
}

void EditPixels::load(const C8* loadName)
{
	if (!fileexist(loadName))
		return;
	bitmap32* loadData = gfxread32(loadName);
	clipblit32(loadData, pixelData, 0, 0, 0, 0, dataSize.x, dataSize.y);
	bitmap32free(loadData);
}

void EditPixels::save(const C8* saveName)
{
	gfxwrite32(saveName, pixelData);

}

S32 mbutToSelectColor(S32 mb)
{
	S32 ret = -1;
	if (MBUT&M_LBUTTON) {
		ret = 0;
	}
	if (MBUT&M_MBUTTON) {
		ret = 1;
	}
	if (MBUT&M_RBUTTON) {
		ret = 2;
	}
	return ret;
}

void EditPixels::procBig()
{
	bool overBig = false;
	if (MX >= editOffset.x && MY >= editOffset.y) {
		pointi2 off;
		off.x = (MX - editOffset.x) / editSquaresSize.x;
		if (off.x < dataSize.x) {
			off.y = (MY - editOffset.y) / editSquaresSize.y;
			if (off.y < dataSize.y) {
				hilitEdit = off;
				overBig = true;
			}
		}
	}
	C32 col;
	if (MBUT && overBig) {
#if 0
		if (MBUT&M_LBUTTON) {
			col = C32RED;
		}
		if (MBUT&M_RBUTTON) {
			col = C32GREEN;
		}
		if (MBUT&M_MBUTTON) {
			col = C32BLUE;
		}
#else
		S32 mbIdx = mbutToSelectColor(MBUT); // 3 buttons
		if (mbIdx >= 0)
			col = selectColors[butColorSelect[mbIdx]];
#endif
		clipputpixel32(pixelData, hilitEdit.x, hilitEdit.y, col);
	}
}

void EditPixels::procSelectColor()
{
	bool overSelect = false;
	if (MY >= colorSelectOffset.y && MY < colorSelectOffset.y + colorSelectSquaresSize.y) {
		if (MX >= colorSelectOffset.x) {
			U32 offX = (MX - colorSelectOffset.x) / colorSelectSquaresSize.x;
			if (offX < 8) {
				hilitColorSelect = offX;
				overSelect = true;
			}
		}
	}
	if (MBUT && overSelect) {
		S32 mbIdx = mbutToSelectColor(MBUT); // 3 buttons
		if (mbIdx >= 0)
			butColorSelect[mbIdx] = hilitColorSelect;
	}
}

// proc edit grid
void EditPixels::proc()
{
	//cliprecto32(B32, 10, 10, 100, 100, C32GREEN);
	if (mDelay >= UI_CHANGE_DELAY) {
		procBig();
		procSelectColor();

	}
	else {
		++mDelay;
	}

}

void EditPixels::drawSmall() const
{
	//cliprecto32(B32, 10, 10, 100, 100, C32GREEN);
	outtextxybf32(B32, smallOffset.x, smallOffset.y - 24, C32WHITE, C32BLACK, "Pixel Area, offset %4u, %4u", smallOffset.x, smallOffset.y);
	clipblit32(pixelData, B32, 0, 0, smallOffset.x, smallOffset.y, dataSize.x, dataSize.y);
}

void EditPixels::drawBig() const
{
	//cliprecto32(B32, 10, 10, 100, 100, C32GREEN);
	outtextxyb32(B32, editOffset.x, editOffset.y - 24, C32WHITE, C32BLACK, "Edit Area");
	for (U32 j = 0; j < (U32)dataSize.y; ++j) {
		for (U32 i = 0; i < (U32)dataSize.x; ++i) {
			cliprect32(B32, editOffset.x + i * editSquaresSize.x, editOffset.y + j * editSquaresSize.y,
				editSquaresSize.x - 2, editSquaresSize.y - 2, clipgetpixel32(pixelData, i, j));
		}
	}
	cliprecto32(B32, editOffset.x - 1 + hilitEdit.x*editSquaresSize.x, editOffset.y - 1 + hilitEdit.y*editSquaresSize.y,
		editSquaresSize.x, editSquaresSize.y, C32WHITE);
}

void EditPixels::drawSelectColor() const
{
	//cliprecto32(B32, 10, 10, 100, 100, C32GREEN);
	// title
	outtextxyb32(B32, colorSelectOffset.x, colorSelectOffset.y - 24, C32WHITE, C32BLACK, "Select Color Area");
	// colored squares
	for (auto i = 0; i < 8; ++i) {
		cliprect32(B32, colorSelectOffset.x + colorSelectSquaresSize.x*i, colorSelectOffset.y, colorSelectSquaresSize.x - 6,
			colorSelectSquaresSize.y - 6, selectColors[i]);
	}
	// mouse select
	for (auto i = 0; i < 3; ++i) {
		S32 j = butColorSelect[i];
		outtextxyb32(B32, colorSelectOffset.x + colorSelectSquaresSize.x*j + 8 + 12*i, colorSelectOffset.y + colorSelectSquaresSize.y+8, C32WHITE, C32BLACK, butColorSelectNames[i]);
#if 0
		cliprect32(B32, colorSelectOffset.x + colorSelectSquaresSize.x*j, colorSelectOffset.y, colorSelectSquaresSize.x - 26,
			colorSelectSquaresSize.y - 26, C32DARKGRAY);
#endif
	}
	// hilit
	cliprecto32(B32, colorSelectOffset.x - 2 + hilitColorSelect * colorSelectSquaresSize.x, colorSelectOffset.y - 2,
		colorSelectSquaresSize.x - 2, colorSelectSquaresSize.y - 2, C32WHITE);
}

// draw edit grid
void EditPixels::draw() const
{
	//cliprecto32(B32, 10, 10, 100, 100, C32GREEN);
	drawSmall();
	drawBig();
	drawSelectColor();
}

EditPixels::~EditPixels()
{
	save(saveName);
	bitmap32free(pixelData);
}


#ifdef DONAMESPACE
} // end namespace holoview2d
using namespace holoview2d;
#endif


void holoview2dinit()
{
	logger("holoview state\n");

	if (fullScreen)
		video_setupwindow(SX, SY);
	else
		video_setupwindow(TRESX, TRESY); // 4 views, 2x2
	pushandsetdir("holoview2d");
	// for show only
	threatbm = gfxread32("dhs_threat1.jpg");
	logger("WINDOW SIZE = ( %d by %d ) in PIXELS\n", WX, WY);
	logger("SCREEN SIZE = ( %d by %d ) in PIXELS\n", SX, SY);
	ep = new EditPixels;

#ifdef BUILD4VIEWS
	// do some real work
	buildFourViews();
#endif
}

void holoview2dproc()
{
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 'f':
		fullScreen = !fullScreen;
		changestate(STATE_HOLOVIEW2D);
		break;
	}
	ep->proc();
}

void holoview2ddraw2d()
{
	// start
	clipclear32(B32,C32BROWN);

	// draw threat level
	//clipblit32(threatbm, B32, 0, 0, WX - threatbm->size.x, WY - threatbm->size.y, threatbm->size.x, threatbm->size.y);

	// draw EditPixels class
	ep->draw();

	// draw cursor
	clipline32(B32, MX + 3, MY, MX + 60, MY, C32WHITE);
	clipline32(B32, MX - 3, MY, MX - 60, MY, C32WHITE);
	clipline32(B32, MX, MY + 3, MX, MY + 60, C32WHITE);
	clipline32(B32, MX, MY - 3, MX, MY - 60, C32WHITE);

	// status
	pointi2 editPoint = ep->getEditHilit();
	outtextxybf32(B32, 0, 0, C32WHITE, C32BLACK, "px %d, py %d", editPoint.x, editPoint.y);
}

void holoview2dexit()
{
	bitmap32free(threatbm);
	delete ep;
	popdir();
}
