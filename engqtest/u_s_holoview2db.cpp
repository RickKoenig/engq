// test some multi view 2d stuff
#include <m_eng.h>
#include "u_states.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace holoview2db {
#endif

//#define BUILD4VIEWS // make a bunch of test 4 views

bitmap32* threatbm;
class EditPixels* epix;

// resolution for 1 view
const U32 VRESX = 640;
const U32 VRESY = 360;

// resolution for 2x2
const U32 TRESX = VRESX * 2;
const U32 TRESY = VRESY * 2;

bool fullScreen;
bool brushMode = true;

// offsets
pointi2 smallOffset;
pointi2 editOffset;
pointi2 colorSelectOffset;
pointi2 brushSelectOffset;
pointi2 brushEditOffset;
pointi2 statusOffset;
pointi2 instructionsOffset;
S32 textYOffset = 24; // how far up to display text about an area
S32 textYSpace = 10; // vertical gap between text lines


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
class EditPixels {
	// members
	// small pixels
	const pointi2 pixelDataSize{ 32,32 };
	bitmap32* pixelData;

	// edit/big pixels
	const pointi2 editSquaresSize{ 16,16 };
	pointi2 hilitEdit{ 0,0 };

	// color select data
	static constexpr pointi2 colorSelectSquares { 8,2 }; // number of colors in a grid
	C32 selectColors[colorSelectSquares.y][colorSelectSquares.x]; // the grid of colors

	// edit select colors
	const pointi2 colorSelectSquaresSize{ 48,48 }; // size of color grid squares
	static pointi2 butColorSelect[3]; // { 0, 2, 7 }; // L M R, where are the hilits in the grid of colors, clicked 
	pointi2 hilitColorSelect{ 0,0 }; // the one that the mouse is over
//TODO: make middle mouse button transparent color
	C8* butColorSelectNames[3] {"L","M","R" }; // mouse button names

	// brush data
	const pointi2 brushDataSize{ 16,16 };
	static constexpr pointi2 brushSelectSquares{ 8, 2 }; // number of brushes in a grid
	bitmap32* brushData[brushSelectSquares.y][brushSelectSquares.x];

	// select brushes
	const S32 brushSelectZoom = 3;
	const S32 brushDataZoom = 2;
	const pointi2 brushSelectSquaresSep{ brushSelectZoom*brushDataSize.x,brushSelectZoom*brushDataSize.y }; // small brushes
	const pointi2 brushSelectSquaresSize{ brushDataZoom*brushDataSize.x,brushDataZoom*brushDataSize.y }; // small brushes
	static pointi2 butBrushSelect; // { 0, 2, 7 }; // L M R, where are the selects in the grid of colors, clicked 
	pointi2 hilitBrushSelect{ 0,0 }; // the one that the mouse is over

	// edit/big brushes
	const pointi2 editBrushSize{ 16,16 };
	pointi2 hilitBrushEdit{ 0,0 }; // mouse

	// load/save
	const C8* loadName{ "EditPixels.png" };
	const C8* saveName{ "EditPixels.png" };
	const C8* brushLoadName{ "EditBrush_%02dx%02d.png" };
	const C8* brushSaveName{ "EditBrush_%02dx%02d.png" };

	// UI settle down between screen resolution changes
	S32 UI_CHANGE_DELAY = 1; // wait 1 frame for the mouse to settle from changes
	S32 mDelay; // wait a bit for the mouse to settle between change window size actions

public:
	EditPixels();
	void proc();
	void draw() const;
	~EditPixels();
	inline pointi2 getEditHilit() const { return hilitEdit; }

private:
	// private methods
	// small/pixels
	void procSmall();
	void drawSmall() const;

	// edit/big
	void procBig();
	void drawBig() const;

	// color select
	void procSelectColor();
	void drawSelectColor() const;
	static S32 mbutToSelectColor(S32 mb); // 0,1,2 -1 if no mouse button

	// draw brush edit
	void procBrushEdit() const;
	void drawBrushEdit() const;

	// draw brush select
	void procBrushSelect();
	void drawBrushSelect() const;

	// load/save
	void load(const C8* loadName);
	void save(const C8* saveName);
};

//pointi2 EditPixels::hilitEdit;
//S32 EditPixels::hilitColorSelect;
pointi2 EditPixels::butColorSelect[3]{ {0,0}, {2,0}, {7,0} }; // L M R
pointi2 EditPixels::butBrushSelect{ 0, 0 };

// public methods
EditPixels::EditPixels() : mDelay{ 0 }
{
	// small pixels, allocate data
	pixelData = bitmap32alloc(pixelDataSize.x, pixelDataSize.y, C32LIGHTCYAN);

	// brushes allocate data	
	for (S32 j = 0; j < brushSelectSquares.y; ++j) {
		for (S32 i = 0; i < brushSelectSquares.x; ++i) {
			brushData[j][i] = bitmap32alloc(brushDataSize.x, brushDataSize.y, C32YELLOW);
		}
	}

	// edit/big pixels, set cursor
	hilitEdit = { 0,0 };

	// select colors, build colors
	S32 k = 0;
	for (S32 j = 0; j < colorSelectSquares.y; ++j) {
		for (S32 i = 0; i < colorSelectSquares.x; ++i,++k) {
			C32& sc = selectColors[j][i];
			sc = C32stdcolors[k];
		}
	}

	// load all data
	load(loadName);
}

// proc edit grid
void EditPixels::proc()
{
	// let UI settle between screen resolution changes, don't call proc right away
	if (mDelay >= UI_CHANGE_DELAY) {
		procBig();
		procSelectColor();
		procSmall();
		procBrushSelect();
		procBrushEdit();
	}
	else {
		++mDelay;
	}
}

// draw edit grid
void EditPixels::draw() const
{
	drawSmall();
	drawBig();
	drawSelectColor();
	drawBrushSelect();
	drawBrushEdit();
}

EditPixels::~EditPixels()
{
	// save everything
	save(saveName);

	// pixels free data
	bitmap32free(pixelData);

	// brushes free data	
	for (S32 j = 0; j < brushSelectSquares.y; ++j) {
		for (S32 i = 0; i < brushSelectSquares.x; ++i) {
			bitmap32free(brushData[j][i]);
		}
	}
}

// private methods
void EditPixels::procSmall()
{
	switch (KEY) {
	case K_RIGHT:
		++smallOffset.x;
		break;
	case K_LEFT:
		--smallOffset.x;
		break;
	case K_DOWN:
		++smallOffset.y;
		break;
	case K_UP:
		--smallOffset.y;
		break;
	}
	smallOffset.x = range(0, smallOffset.x, 200);
	smallOffset.y = range(0, smallOffset.y, 200);
}

void EditPixels::drawSmall() const
{
	outtextxybf32(B32, smallOffset.x, smallOffset.y - textYOffset, C32WHITE, C32BLACK, "Pixel Area, offset %4u, %4u", smallOffset.x, smallOffset.y);
	clipblit32(pixelData, B32, 0, 0, smallOffset.x, smallOffset.y, pixelDataSize.x, pixelDataSize.y);
}

void EditPixels::procBig()
{
	bool overBig = false;
	if (MX >= editOffset.x && MY >= editOffset.y) {
		pointi2 off;
		off.x = (MX - editOffset.x) / editSquaresSize.x;
		if (off.x < pixelDataSize.x) {
			off.y = (MY - editOffset.y) / editSquaresSize.y;
			if (off.y < pixelDataSize.y) {
				hilitEdit = off;
				overBig = true;
			}
		}
	}
	C32 col;
	if (MBUT && overBig) {
		S32 mbIdx = mbutToSelectColor(MBUT); // 3 buttons
		if (mbIdx >= 0) {
			//butColorSelect[mbIdx]
			const pointi2& csi = butColorSelect[mbIdx];
			S32 xIdx = csi.x;
			S32 yIdx = csi.y;
			col = selectColors[yIdx][xIdx];
		}
		clipputpixel32(pixelData, hilitEdit.x, hilitEdit.y, col);
	}
}

void EditPixels::drawBig() const
{
	//cliprecto32(B32, 10, 10, 100, 100, C32GREEN);
	outtextxyb32(B32, editOffset.x, editOffset.y - textYOffset, C32WHITE, C32BLACK, "Pixel Edit Area");
	for (U32 j = 0; j < (U32)pixelDataSize.y; ++j) {
		for (U32 i = 0; i < (U32)pixelDataSize.x; ++i) {
			cliprect32(B32, editOffset.x + i * editSquaresSize.x, editOffset.y + j * editSquaresSize.y,
				editSquaresSize.x - 2, editSquaresSize.y - 2, clipgetpixel32(pixelData, i, j));
		}
	}
	cliprecto32(B32, editOffset.x - 1 + hilitEdit.x*editSquaresSize.x, editOffset.y - 1 + hilitEdit.y*editSquaresSize.y,
		editSquaresSize.x, editSquaresSize.y, C32WHITE);
}

void EditPixels::procSelectColor()
{
	bool overSelect = false;
	if (MY >= colorSelectOffset.y && MY < colorSelectOffset.y + colorSelectSquaresSize.y*colorSelectSquaresSize.y) {
		if (MX >= colorSelectOffset.x && MX < colorSelectOffset.x + colorSelectSquaresSize.x*colorSelectSquaresSize.x) {
			S32 offX = (MX - colorSelectOffset.x) / colorSelectSquaresSize.x;
			S32 offY = (MY - colorSelectOffset.y) / colorSelectSquaresSize.y;
			if (offX < colorSelectSquares.x && offY < colorSelectSquares.y) {
				hilitColorSelect = pointi2{ offX, offY };
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

void EditPixels::drawSelectColor() const
{
	// title
	outtextxyb32(B32, colorSelectOffset.x, colorSelectOffset.y - textYOffset, C32WHITE, C32BLACK, "Select Color Area");

	// colored squares
	for (auto j = 0; j < colorSelectSquares.y; ++j) {
		for (auto i = 0; i < colorSelectSquares.x; ++i) {
			cliprect32(B32, colorSelectOffset.x + colorSelectSquaresSize.x*i, colorSelectOffset.y + colorSelectSquaresSize.y*j, colorSelectSquaresSize.x - 6,
				colorSelectSquaresSize.y - 6, selectColors[j][i]);
		}
	}

	// mouse select
	for (auto k = 0; k < 3; ++k) {
		const auto& cs = butColorSelect[k];
		outtextxyb32(B32, 
			colorSelectOffset.x + colorSelectSquaresSize.x*cs.x + 4 + 12*k,
			colorSelectOffset.y + colorSelectSquaresSize.y*cs.y + colorSelectSquaresSize.y/2 - 4,
			C32WHITE, C32BLACK,
			butColorSelectNames[k]);
	}

	// hilit
	cliprecto32(B32, 
		colorSelectOffset.x - 2 + hilitColorSelect.x * colorSelectSquaresSize.x,
		colorSelectOffset.y - 2 + hilitColorSelect.y * colorSelectSquaresSize.y,
		colorSelectSquaresSize.x - 2,
		colorSelectSquaresSize.y - 2,
		C32WHITE);
}

// draw brush edit
void EditPixels::procBrushEdit() const
{

}

void EditPixels::drawBrushEdit() const
{
	// title
	outtextxyb32(B32, brushEditOffset.x, brushEditOffset.y - textYOffset, C32WHITE, C32BLACK, "Brush Edit");
}

// draw brush select
void EditPixels::procBrushSelect()
{
	bool overSelect = false;
	if (MY >= brushSelectOffset.y && MY < brushSelectOffset.y + colorSelectSquaresSize.y*colorSelectSquaresSize.y) {
		if (MX >= brushSelectOffset.x && MX < brushSelectOffset.x + colorSelectSquaresSize.x*colorSelectSquaresSize.x) {
			S32 offX = (MX - brushSelectOffset.x) / brushSelectSquaresSep.x;
			S32 offY = (MY - brushSelectOffset.y) / brushSelectSquaresSep.y;
			if (offX < brushSelectSquares.x && offY < brushSelectSquares.y) {
				hilitBrushSelect = pointi2{ offX, offY };
				overSelect = true;
			}
		}
	}
	// select brush
	if (wininfo.mleftclicks && overSelect) {
		butBrushSelect = hilitBrushSelect;
	}


}

void EditPixels::drawBrushSelect() const
{
	// title
	outtextxyb32(B32, brushSelectOffset.x, brushSelectOffset.y - textYOffset, C32WHITE, C32BLACK, "Brush Select");

	// brush data
	for (auto j = 0; j < brushSelectSquares.y; ++j) {
		for (auto i = 0; i < brushSelectSquares.x; ++i) {
			bitmap32* zoomBrush = brushData[j][i];
			bool own = false;
			switch (brushDataZoom) {
			case 2:
				zoomBrush = bitmap32double(zoomBrush);
				own = true;
				break;
			}
			clipblit32(zoomBrush,B32,
				0,
				0,
				brushSelectOffset.x + brushSelectSquaresSep.x*i,
				brushSelectOffset.y + brushSelectSquaresSep.y*j,
				brushSelectSquaresSep.x - 6,
				brushSelectSquaresSep.y - 6);
			if (own)
				bitmap32free(zoomBrush);
		}
	}

	// mouse select
	//const auto& cs = butColorSelect[k];
	outtextxyb32(B32,
		brushSelectOffset.x + brushSelectSquaresSep.x*butBrushSelect.x + 4,
		brushSelectOffset.y + brushSelectSquaresSep.y*butBrushSelect.y - 12,
		C32WHITE, C32BLACK,
		"VVV");
	outtextxyb32(B32,
		brushSelectOffset.x + brushSelectSquaresSep.x*butBrushSelect.x + 4,
		brushSelectOffset.y + brushSelectSquaresSep.y*butBrushSelect.y - 12 + brushSelectSquaresSep.y,
		C32WHITE, C32BLACK,
		"^^^");

	// hilit mouse over
	cliprecto32(B32,
		brushSelectOffset.x - 2 + hilitBrushSelect.x * brushSelectSquaresSep.x,
		brushSelectOffset.y - 2 + hilitBrushSelect.y * brushSelectSquaresSep.y,
		brushSelectSquaresSize.x + 4,
		brushSelectSquaresSize.y + 4,
		C32WHITE);


}

S32 EditPixels::mbutToSelectColor(S32 mb)
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

void EditPixels::load(const C8* loadName)
{
	// load pixels
	if (!fileexist(loadName))
		return;
	bitmap32* loadData = gfxread32(loadName);
	clipblit32(loadData, pixelData, 0, 0, 0, 0, pixelDataSize.x, pixelDataSize.y);
	bitmap32free(loadData);

	// load brushes
	for (S32 j = 0; j < brushSelectSquares.y; ++j) {
		for (S32 i = 0; i < brushSelectSquares.x; ++i) {

			C8 brushName[1000];
			sprintf(brushName, brushLoadName, i, j);
			if (!fileexist(brushName))
				continue;
			bitmap32* loadData = gfxread32(brushName);
			bitmap32* bd = brushData[j][i];
			clipblit32(loadData, bd, 0, 0, 0, 0, brushDataSize.x, brushDataSize.y);
			bitmap32free(loadData);
		}
	}

}

void EditPixels::save(const C8* saveName)
{
	// save pixels
	gfxwrite32(saveName, pixelData);

	// save brushes
	for (S32 j = 0; j < brushSelectSquares.y; ++j) {
		for (S32 i = 0; i < brushSelectSquares.x; ++i) {

			C8 brushName[1000];
			sprintf(brushName, brushSaveName, i, j);
			bitmap32* bd = brushData[j][i];
			gfxwrite32(brushName, bd);
		}
	}
}


struct menuvar holoview2dbSettings[] = {

	{"@lightred@---- holoview2db settings -------------------",0,D_VOID,0},
	{"smallOffset.x",&smallOffset.x,D_INT|D_RDONLY},
	{"smallOffset.y",&smallOffset.y,D_INT|D_RDONLY },
};
	S32 nholoview2dbSettings = NUMELEMENTS(holoview2dbSettings);

void loadSmallOffset()
{
	loadconfigfile("settings.txt", holoview2dbSettings, nholoview2dbSettings);
}

void saveSmallOffset()
{
	FILE* fw;
	fw = fopen2("settings.txt", "w");
	if (fw) {
		fprintf(fw, "# generated by holoview2d brush...\n");
		fprintf(fw, "smallOffset.x %d\n", smallOffset.x);
		fprintf(fw, "smallOffset.y %d\n", smallOffset.y);
		fclose(fw);
	}
}


#ifdef DONAMESPACE
} // end namespace holoview2d
using namespace holoview2db;
#endif


void holoview2dbinit()
{
	logger("holoview2d brush state\n");

	if (fullScreen)
		video_setupwindow(SX, SY);
	else
		video_setupwindow(1820, 980);
		//video_setupwindow(TRESX, TRESY); // 4 views, 2x2
	pushandsetdir("holoview2dbrush");
	// for show only
	threatbm = gfxread32("dhs_threat1.jpg");
	logger("WINDOW SIZE = ( %d by %d ) in PIXELS\n", WX, WY);
	logger("SCREEN SIZE = ( %d by %d ) in PIXELS\n", SX, SY);

	// setup every UI offset
	smallOffset = { 64,50 };
	loadSmallOffset(); // if this file exists, then update smallOffset
	editOffset = { S32(WX * .3f),S32(WY * .45f) };
	colorSelectOffset = { S32(WX * .75f) , 50 };
	brushSelectOffset = { S32(WX * .75f) , 250 };
	brushEditOffset = { S32(WX * .75f) , editOffset.y };
	statusOffset = { S32(WX*.22f),50 };
	instructionsOffset = { S32(WX*.45f),50 };

	// runs bigpixels, smallpixels, and color select grid areas
	epix = new EditPixels;

#ifdef BUILD4VIEWS
	// do some real work
	buildFourViews();
#endif
	//adddebvars("holo2d", holoview2dbSettings, nholoview2dbSettings);
}

void holoview2dbproc()
{
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 'f':
		fullScreen = !fullScreen;
		changestate(STATE_HOLOVIEW2DB);
		break;
	case 'm':
		brushMode = !brushMode;
		//changestate(STATE_HOLOVIEW2DB);
		break;
	}
	epix->proc();
}

void holoview2dbdraw2d()
{
	// show brush mode by background color
	clipclear32(B32,brushMode ? C32BROWN : C32LIGHTGRAY);

	// show instructions
	outtextxybf32(B32, instructionsOffset.x, instructionsOffset.y, C32WHITE, C32BLACK, "KEY press 'm' to change modes");
	outtextxybf32(B32, instructionsOffset.x, instructionsOffset.y + textYSpace, C32WHITE, C32BLACK, "KEY press 'f' to from fullscreen");
	outtextxybf32(B32, instructionsOffset.x, instructionsOffset.y + 2*textYSpace, C32WHITE, C32BLACK, "3 mouse buttons to select color and draw");
	outtextxybf32(B32, instructionsOffset.x, instructionsOffset.y + 3*textYSpace, C32WHITE, C32BLACK, "Arrow keys to adjust smallOffset");

	// draw threat level
	//clipblit32(threatbm, B32, 0, 0, WX - threatbm->size.x, WY - threatbm->size.y, threatbm->size.x, threatbm->size.y);

	// draw EditPixels class
	epix->draw();

	// draw cursor
	clipline32(B32, MX + 3, MY, MX + 60, MY, C32WHITE);
	clipline32(B32, MX - 3, MY, MX - 60, MY, C32WHITE);
	clipline32(B32, MX, MY + 3, MX, MY + 60, C32WHITE);
	clipline32(B32, MX, MY - 3, MX, MY - 60, C32WHITE);

	// draw status
	pointi2 editPoint = epix->getEditHilit();
	outtextxybf32(B32, statusOffset.x, statusOffset.y, C32WHITE, C32BLACK, "STATUS");
	outtextxybf32(B32, statusOffset.x, statusOffset.y + textYSpace, C32WHITE, C32BLACK, "Edit Pixel: px %d, py %d", editPoint.x, editPoint.y);
	outtextxybf32(B32, statusOffset.x, statusOffset.y + 2*textYSpace, C32WHITE, C32BLACK, "Brushmode: %s",brushMode ? "TOBRUSH" : "TOSCREEN");
}

void holoview2dbexit()
{
	saveSmallOffset();
	bitmap32free(threatbm);
	delete epix;
	popdir();
	//removedebvars("holo2d");
}
