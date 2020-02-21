// Stoid Command game
#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"
#include "u_stoid_data.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace stoid {
#endif

bitmap32* B32S; // where stuff is drawn
bitmap32* B32M; // blown up x2, x2
// B32 // blown up some more x2, x2    in engine
struct bitmap32* puzzcirc; // puzzle circles are drawn in this bitmap, left and right
struct bitmap32* mycirc; // magenta circle, my circle, up and down

const S32 SWIDTH = 320;
const S32 SHEIGHT = 200;

// state
int level;
int ypos;
int scount;
int xpuzz;
int delayCount;
char str[80];
enum StoidModeE{
	PLAYING,
	CRASHED,
	REACHED_TOP,
	DONE,
};
StoidModeE stoidMode;

bool fastscan32alpha1(struct bitmap32* s, struct bitmap32* d, S32 sx, S32 sy, S32 dx, S32 dy, S32 tx, S32 ty)
{
	S32 i, j;
	U32 sinc, dinc;
	register C32 *sp, *dp;
	if (tx <= 0 || ty <= 0)
		return false;
	bool col = false;
	sp = s->data + s->size.x*sy + sx;
	dp = d->data + d->size.x*dy + dx;
	sinc = s->size.x;
	dinc = d->size.x;
	for (j = 0; j < ty; j++) {
		for (i = 0; i < tx; i++) {
			register C32 v;
			v = sp[i];
			if (v.a >= 0x80) {
				if (dp[i].c32 & 0xffffff) {
					col = true;
					v = 0xffffffff;
				}
				dp[i] = v;
			}
		}
		sp += sinc;
		dp += dinc;
	}
	return col;
}

bool clipscan32alpha1(struct bitmap32* s, struct bitmap32* d, S32 sx, S32 sy, S32 dx, S32 dy, S32 tx, S32 ty)
{
	if (bclip32(s, d, &sx, &sy, &dx, &dy, &tx, &ty)) {
		return fastscan32alpha1(s, d, sx, sy, dx, dy, tx, ty);
	}
	return false;
}

void doUPDwon()
{
	if (scount & 1)
		return;
	S32 del = 0;
	if (MBUT & 1)
		del--;
	if (MBUT & 2)
		del++;

	if (wininfo.keystate[K_UP] || wininfo.keystate[K_NUMUP] || wininfo.keystate['w'])
		del--;
	if (wininfo.keystate[K_DOWN] || wininfo.keystate[K_NUMDOWN] || wininfo.keystate['s'])
		del++;

	del = range(-1, del, 1);
	//del *= 10; // test speedup up down
	ypos += del;

	if (ypos > 165)
		ypos = 165;

	if (ypos < 40)
	{
		stoidMode = REACHED_TOP;
		delayCount = 60; //60
		clipblit32alpha1(mycirc, B32S, 0, 0, 160 - 16, ypos - 16, 32, 32);
	}
}

void stepCircles()
{

	scount++;
	if (scount == 26)
	{
		int val = levels[level][xpuzz];
		if (val & 1)
			clipcircle32(puzzcirc, 7, 48 + 6, 3, C32GREEN);
		if (val & 4)
			clipcircle32(puzzcirc, 7, 48 + 6 + 26, 3, C32BLUE);
		if (val & 0x10)
			clipcircle32(puzzcirc, 7, 48 + 6 + 2 * 26, 3, C32RED);
		if (val & 0x40)
			clipcircle32(puzzcirc, 7, 48 + 6 + 3 * 26, 3, C32LIGHTBLUE);
		if (val & 2)
			clipcircle32(puzzcirc, 319 - 6, 61 + 6, 3, C32DARKGRAY);
		if (val & 8)
			clipcircle32(puzzcirc, 319 - 6, 61 + 6 + 26, 3, C32LIGHTBLUE);
		if (val & 0x20)
			clipcircle32(puzzcirc, 319 - 6, 61 + 6 + 2 * 26, 3, C32LIGHTRED);
		if (val & 0x80)
			clipcircle32(puzzcirc, 319 - 6, 61 + 6 + 3 * 26, 3, C32BROWN);
		scount = 0;
		xpuzz++;
		if (xpuzz == levelsizes[level])
			xpuzz = 0;
	}
	// shift circles left and right
	int i;
	for (i = 0; i < 4; i++)
		clipblit32(puzzcirc, B32S, 0, i * 26 + 48, 1, i * 26 + 48, SWIDTH - 1, 13);
	for (i = 0; i < 4; i++)
		clipblit32(puzzcirc, B32S, 1, i * 26 + 61, 0, i * 26 + 61, SWIDTH - 1, 13);
	clipblit32(B32S, puzzcirc, 0, 54 - 5, 0, 54 - 5, SWIDTH, 92 + 10);
}

#ifdef DONAMESPACE
} // end namespace stoid

using namespace stoid;
#endif

void stoidinit()
{
	video_setupwindow(SWIDTH*4, SHEIGHT*4); // maybe 1280 by 800

	// alloc bitmaps
	B32M = bitmap32alloc(SWIDTH*2, SHEIGHT*2, C32BLACK);
	B32S = bitmap32alloc(SWIDTH, SHEIGHT, C32BLACK);
	puzzcirc = bitmap32alloc(SWIDTH, SHEIGHT, C32BLACK);
	mycirc = bitmap32alloc(32, 32, 0);
	clipcircle32(mycirc, 16, 16, 3, C32MAGENTA);

	// init state
	level = 0;
	xpuzz = 0;
	scount = 0;
	delayCount = 0;
	ypos = 165;
	stoidMode = PLAYING;
}

void stoidproc()
{
	//	changestate(NOSTATE);
	if (KEY == K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	switch (stoidMode) {
	case PLAYING:
		cliprect32(B32S, 0, 0, SWIDTH, SHEIGHT, C32BLACK);
		doUPDwon();
		stepCircles();
		break;
	case CRASHED:
		--delayCount;
		if (!delayCount) {
			stoidMode = PLAYING;
			ypos = 165;
			cliprect32(puzzcirc, 0, 0, SWIDTH - 1, SHEIGHT - 1, C32BLACK);
		}
		break;
	case REACHED_TOP:
		--delayCount;
		if (!delayCount) {
			stoidMode = PLAYING;
			level++;
			S32 ll = LAST_LEVEL;
			if (level == ll) {
				level = ll - 1;
				stoidMode = DONE;
				delayCount = 150;
			} else {
				cliprect32(puzzcirc, 0, 0, SWIDTH - 1, SHEIGHT - 1, C32BLACK);
				ypos = 165;
				xpuzz = 0;
				scount = 0;
			}
		}
		break;
	case DONE:
		--delayCount;
		if (!delayCount)
			popstate();
		if (delayCount & 16) { // cheap animate win state
			outtextxybf32(B32S, 160 - 4 * strlen("Koodoos!!"), 96, C32BLUE, C32BLACK, "Koodoos!!");
			outtextxybf32(B32S, 160 - 4 * strlen("You Won!!"), 104, C32RED, C32BLACK, "You Won!!");
		} else {
			outtextxybf32(B32S, 160 - 4 * strlen("Koodoos!!"), 96, C32RED, C32BLACK, "Koodoos!!");
			outtextxybf32(B32S, 160 - 4 * strlen("You Won!!"), 104, C32BLUE, C32BLACK, "You Won!!");
		}
		break;
	}
	if (stoidMode == PLAYING) {
		bool hit = clipscan32alpha1(mycirc, B32S, 0, 0, 160 - 16, ypos - 16, 32, 32);
		if (hit && !delayCount) {
			stoidMode = CRASHED;
			delayCount = 45;
			cliprect32(puzzcirc, 0, 0, SWIDTH - 1, SHEIGHT - 1, C32BLACK);
			scount = 0;
			xpuzz = 0;
		}
	}
	sprintf(str, "Level %d, %s", level + 1, levelstrs[level]);
	outtextxy32(B32S, 160 - 4 * strlen(str), 192, C32WHITE, str);
}

void stoiddraw2d()
{
	bitmap32double(B32S, B32M);
	bitmap32double(B32M, B32);
}

void stoidexit()
{
	bitmap32free(B32M);
	bitmap32free(B32S);
	bitmap32free(mycirc);
	bitmap32free(puzzcirc);
}
