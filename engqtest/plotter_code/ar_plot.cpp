//#include "../u_ar_scan.h"
//#include "../u_ar_parse.h"
//using namespace u_ar_parse;

#define AR_PLOT
#ifdef AR_PLOT

#define USENAMESPACE
#ifdef USENAMESPACE
namespace ar_plot {
#endif

doar* anar;
arparseinfo arpi; // read only structure of state of ar calc

S32 resetrottrans = 0; // if non zero, reset the calc
S32 calcmatch; // how many to calc
S32 fastcalcmatch; // how many to calc in 1 frame

// math coords
float xres = 4.0f;
float yres = 3.0f;
// scan cooards
float scanxres = 640.0f;
float scanyres = 480.0f;

// which scan to analyze
S32 scanidx = 0;
S32 oldscanidx = -1;
const S32 npc = doar::npc; // number of points, right now 5

// scanned points, assume 640 by 480
const pointf2 arcode00017[npc] = {
   {186, 77},
   {477, 267},
   {149, 238},
   {316, 91},
   {441, 105},
};

const pointf2 arcode00018[npc] = {
   {173, 89},
   {280, 91},
   {176, 306},
   {388, 299},
   {383, 93},
};

const pointf2 arcode00019[npc] = {
   {99, 50},
   {300, 261},
   {95, 257},
   {303, 68},
   {205, 59},
};

const pointf2 arcode00020[npc] = {
   {291, 126},
   {100, 418},
   {139, 125},
   {477, 412},
   {440, 132},
};

const pointf2 arcode00021[npc] = {
   {455, 50},
   {402, 419},
   {503, 336},
   {592, 258},
   {250, 211},
};

const pointf2 arcode00022[npc] = {
   {53, 70},
   {20, 314},
   {386, 310},
   {340, 83},
   {201, 78},
};

const pointf2 arcode00023[npc] = {
   {330, 28},
   {194, 68},
   {142, 362},
   {501, 220},
   {37, 110},
};

const pointf2 arcode00024[npc] = {
   {13, 226},
   {359, 450},
   {79, 413},
   {53, 331},
   {383, 273},
};

const pointf2 arcode00026[npc] = {
   {448, 15},
   {17, 160},
   {91, 32},
   {592, 130},
   {501, 62},
};

const pointf2 arcode00027[npc] = {
   {164, 88},
   {309, 221},
   {402, 94},
   {463, 218},
   {150, 218},
};

// list of scanned points
const pointf2* scannedpointsets[] =
{
	arcode00017,
	arcode00018,
	arcode00019,
	arcode00020,
	arcode00021,
	arcode00022,
	arcode00023,
	arcode00024,
	arcode00026,
	arcode00027,
};

const S32 numscans = NUMELEMENTS(scannedpointsets);

#ifdef USENAMESPACE
} // namespace ar_plot
using namespace ar_plot;
#endif
// start ar state
#include <float.h>
void init_arplot()
{
    logger("hiho %e\n",FLT_MAX);
	anar = new doar(pointf2x(scanxres,scanyres),pointf2x(xres,yres));
	oldscanidx = -1;
}

// proc ar state
void draw_arplot()
{
	S32 i;
// UI adjustments
// limit range of index of scanned points
	if (scanidx >= numscans)
		scanidx = 0;
	else if (scanidx < 0)
		scanidx = numscans - 1;
	if (resetrottrans) {
		anar->doresetrottrans();
		resetrottrans = 0;
	}
	if (calcmatch < 0)
		calcmatch = 0;
	fastcalcmatch = range(1,fastcalcmatch,10000);
	if (scanidx != oldscanidx) {
		anar->doresetrottrans();
		anar->setscanpoints(scannedpointsets[scanidx]);
		oldscanidx = scanidx;
	}
// calc rot and trans by stepping and checking for errors
	for (i=0;i<fastcalcmatch;++i) {
		if (calcmatch > 0) {
			anar->calcmatchstep();
			--calcmatch;
		}
	}
	anar->getarparseinfo(arpi);
// draw screen borders
	drawbox2(pointf2x(0,0),pointf2x(xres,yres),C32(180,180,255));
// draw scanned plotter points
	const pointf2* sp = anar->getsp();
	const pointf2* rp = anar->getrp();
	for (i=0;i<npc;++i) {
		pointf2 ps = sp[i];
		drawfpoint(ps,C32RED);
		pointi2 pi=math2screen(ps);
		outtextxybf32(B32,pi.x-8,pi.y-8,C32BLACK,C32WHITE,"%d",i);
	}
 // draw reference plotter points
	for (i=0;i<npc;++i) {
		pointf2 pr = rp[i];
		drawfpoint(pr,C32GREEN);
		pointi2 pi=math2screen(pr);
		outtextxybf32(B32,pi.x-8,pi.y-8,C32BLACK,C32WHITE,"%d",i);
	}
}

void exit_arplot()
{
	delete anar;
	anar = 0;
}

#endif // AR_PLOT
