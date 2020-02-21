#define D2_3D
#define RES3D // alternate shape class
#include <m_eng.h>
#include <l_misclibm.h>

#include "carenalobby.h"
#include "jrmcars.h"
#include "n_loadweapcpp.h"
#include "enums.h"
#include "carclass.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "newconstructor.h"
#include "carenagame.h"
#include "tracklist.h"
#include "system/u_states.h"
#include "netdescj.h"
#include "timewarp.h"
#include "envj.h"
#include "avaj.h"
#include "gamerunj.h"
#include "rematch.h"

static shape* rm;
static int hiderematch;
static int rematchyes;
static shape* rmfocus;
static text* TEXTREMATCH;
static pbut* PBUTYES,*PBUTNO;
static rematch rematchbuts;

rematch getrematchbuts()
{
	return rematchbuts;
}

void setrematchbuts(rematch rm)
{
	rematchbuts=rm;
}

void loadrematch()
{
pushandsetdirdown("rematchres");
	rm = res_loadfile("online_rematchres.txt");
popdir();
	TEXTREMATCH=rm->find<text>("TEXTREMATCH");
	PBUTYES=rm->find<pbut>("PBUTYES");
	PBUTNO=rm->find<pbut>("PBUTNO");
	gg->con=rm->find<listbox>("LISTGAMENEWS");
	gg->con->setmax(600);
	rematchoff();
	rematchbuts=NONE;
}

void rematchon()
{
	TEXTREMATCH->setvis(1);
	PBUTYES->setvis(1);
	PBUTNO->setvis(1);
}

void rematchoff()
{
	TEXTREMATCH->setvis(0);
	PBUTYES->setvis(0);
	PBUTNO->setvis(0);
}

bool isrematchon()
{
	return PBUTYES->getvis()!=0;
}

void freerematch()
{
	delete rm;
	rmfocus=0;
}

void drawrematch()
{
	rm->draw3d();
}

void procrematch()
{
	shape* over=rm->getover();
	if (wininfo.mleftclicks)
		rmfocus=over;
// call over or not over proc
	rm->over_no_over_proc(rmfocus,over);
	S32 ret = 0;
	shape* focusa = 0;
	if (rmfocus) {
		ret=rmfocus->procfocus();
		focusa = ret>=0 ? rmfocus : 0;
		if (focusa == PBUTYES) {
			rematchbuts=YES;
		} else if (focusa == PBUTNO) {
			rematchbuts=NO;
		}
	}
	if (isrematchon()) { // keyboard shortcuts
		if (KEY=='y') {
			rematchbuts=YES;
			KEY=0;
		} else if (KEY=='n') {
			rematchbuts=NO;
			KEY=0;
		}
	}
}
