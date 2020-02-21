// it's back.
// now it waits for gameinfo message for osakit.

#define RES3D
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "match_logon.h"
#include "../u_states.h"
#include "gameinfo.h"

namespace updater {
shape* rl;

shape* focus;
pbut* NO;//,*YES;

bool doupdate;
S32 testtimer;
}
using namespace updater;

void updater_init()
{
	testtimer=0;
pushandsetdir("racenetdata/updater");
	script sc("updatestatus.txt"); // need to read osakit message?
	if (sc.num() && sc.idx(0)=="0")
		doupdate=false;
	else
		doupdate=true;
// bail if no update
	if (!doupdate)
		return;
	focus=0;
	logger("updater init\n");
	video_setupwindow(GX,GY);
	init_res3d(800,600);
	rl=res_loadfile("updaterres.txt");
	NO=rl->find<pbut>("NO");
//	YES=rl->find<pbut>("YES");
//	YES->setvis(false);
}

void updater_proc()
{
// bail if no update
	if (!doupdate) {
		changestate(STATE_OLDCARENACONNECT);
		return;
	}
// see if any osakit messages around, mockup
	const C8* bm1=0;
	const C8* bm2="bogus message";
	const C8* bm3="p 987654 u ~gameinfo.txt~,20057 r ~newcheck2~,7,0,3,0,C1357902468 v ~megaduty~,250,1,2,0,251,2,0,1,252,5 w 10,1,22,-1,-1,-1,-1,-1,-1 e 2,3,5,7,11,13 x 1428571428,2.25 i 192.168.1.100,65432 c ~racer q~,97531,24000 g";
	const C8* bm=0;
	if (testtimer==30)
		bm=bm1;
	else if (testtimer==60)
		bm=bm2;
	else if (testtimer==90)
		bm=bm3;
	gameinf* gi=new gameinf(bm);
	if (!gi->isvalid) {
		delete gi;
		gi=0;
	} else { // got a valid message
		delete globalgame;
		globalgame=gi;
		gi=0;
		changestate(STATE_OLDCARENACONNECT);
	}
// escape
	if (KEY==K_ESCAPE) {
		if (matchcl) { 
			changestate(STATE_MATCHRESULTS);
		} else {
			changestate(STATE_OLDCARENALOBBY);
		}
	}
// ui
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0; // see if something happened
/*		if (focusa==YES) {
			FILE* fw=fopen2("updatestatus.txt","w");
			fprintf(fw,"0\n"); // updated
			fclose(fw);
			changestate(STATE_OLDCARENACONNECT);
		} else */ if (focusa==NO) {
			changestate(STATE_OLDCARENACONNECT);
		}
	}
// end ui
	++testtimer;
}

void updater_draw3d()
{
// bail if no update
	if (!doupdate) // dont' draw if no update
		return;
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	video_sprite_end(); 
}

void updater_exit()
{
popdir();
// bail if no update
	if (!doupdate)
		return;
	delete rl;
	exit_res3d();
	if (wininfo.closerequested && matchcl) { // if we have a match server socket and bailing then free it
		freesocker(matchcl);
		matchcl=0;
	}
}
