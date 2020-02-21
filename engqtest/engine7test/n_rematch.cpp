/*
#include <engine7cpp.h>
#include <misclib7cpp.h>
#include "usefulcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "carenagamecpp.h"
#include "rematchrescpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_carclass.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"

//static struct reslist *r;
static shape* r;
static int hiderematch;
static int rematchyes;
static shape* rmfocus;
static pbut* PBUTYES,*PBUTNO;

void loadrematch()
{
pushandsetdirdown("rematchres");
//	r=loadres("online_rematchres.txt");
	factory2<shape> fact;
	script* msc=new script("online_rematchres.txt");
	r=fact.newclass_from_handle(*msc);
	delete msc;
popdir();
	PBUTYES=r->find<pbut>("PBUTYES");
	PBUTNO=r->find<pbut>("PBUTNO");
}

void freerematch()
{
//	freeres(r);
	delete r;
	rmfocus=0;
}

void drawrematch()
{
	int clk=gg.clock-gg.clockoffset;
	if (clk<TICKRATE*gcfg.rematchtime)
		hiderematch=rematchyes=0;
	if (clk>=TICKRATE*gcfg.rematchtime && clk<TICKRATE*gcfg.norematchtime && !hiderematch)
//		drawres(r);
		r->draw();
//	if (clk>=TICKRATE*gcfg.exittime && !hiderematch)
//		popstate();
	if (clk>=TICKRATE*gcfg.norematchtime && !rematchyes)
		popstate();
}

void procrematch()
{
//	gg.krematchyes=0;//=gg.krematchno=0;
	int clk=gg.clock-gg.clockoffset;
	if (clk>=TICKRATE*gcfg.rematchtime && clk<TICKRATE*gcfg.norematchtime && !hiderematch) {
//		struct rmessage rm;
// set focus
		if (wininfo.mleftclicks)
			rmfocus=r->getfocus();
// if something selected...
		if (rmfocus) {
			S32 ret=rmfocus->proc();
			shape* focusb = ret ? rmfocus : 0;
//		checkres(r);
			if (focusb==PBUTYES) { // start state button, can't do a switch on pointers
//		while(getresmess(r,&rm)) {
//			switch(rm.id) {
//			case PBUTYES:
				gg.yk.krematchyes=1;
				rematchyes=1;
				hiderematch=1;
//			break;
//			case PBUTNO:
			} else if (focusb==PBUTNO) {
				hiderematch=1;
				popstate();
//			break;
			}
		}
		if (gg.yourcntl==CNTL_AI) {
			gg.yk.krematchyes=1;
			rematchyes=1;
			hiderematch=1;
			return;
		}
		if (KEY=='y') {
			gg.yk.krematchyes=1;
			rematchyes=1;
			hiderematch=1;
			return;
		}
		if (KEY=='n') {
			hiderematch=1;
			popstate();
			return;
		}
	}
}
