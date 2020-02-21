#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "carclass.h"
#include "timewarp.h"

#include "netdescj.h"
#include "envj.h"
#include "avaj.h"
#include "gamerunj.h"
#include "carenagame.h" // for predicted
#include "rematch.h"
#include "rabbits.h"
#include "scrline.h"
#include "soundfx.h"

// car over s/f piece
void initrabbits()
{
	S32 i,n=gg->numplayers;
	const trkt* ot = gg->e.oldtrackj;
	for (i=0;i<n;++i) {
		twcaro* tw=&gg->caros[i].cs;
		pointi2& stloc=gg->caros[i].stpceloc;
		S32 pt;
		S32 rot;
		ot->curpiece(&stloc,&pt,&rot);
		tw->lastloc=stloc;
		if (pt!=PCE_STARTFINISH)
			continue;
		tw->nnextloc=1;
		switch(rot) {
		case 0:
			tw->nextloc[0].x=stloc.x+1;
			tw->nextloc[0].y=stloc.y;
			break;
		case 1:
			tw->nextloc[0].x=stloc.x;
			tw->nextloc[0].y=stloc.y-1;
			break;
		case 2:
			tw->nextloc[0].x=stloc.x-1;
			tw->nextloc[0].y=stloc.y;
			break;
		case 3:
			tw->nextloc[0].x=stloc.x;
			tw->nextloc[0].y=stloc.y+1;
			break;
		default:
			break;
		}
//		if (gg->viewslot==i)
//			gg->con->printf("nextloc %d,%d",tw->nextloc[0].x,tw->nextloc[0].y);
	}
}

// move checkpoint system along... , used for checkpoints and bots
// 0 right, 1 down, 2 left, 3 up, like in constructor
// table driven
S32 nextlocsrot[PCE_NPIECETYPE][4][2]={
	{{BADDIR,BADDIR},{BADDIR,BADDIR},{BADDIR,BADDIR},{BADDIR,BADDIR}}, // blank
	{{     0,BADDIR},{BADDIR,BADDIR},{     2,BADDIR},{BADDIR,BADDIR}}, // straight
	{{     1,BADDIR},{BADDIR,BADDIR},{BADDIR,BADDIR},{     2,BADDIR}}, // turn
	{{     0,BADDIR},{BADDIR,BADDIR},{     2,BADDIR},{BADDIR,BADDIR}}, // startfinish
	{{     0,BADDIR},{     1,BADDIR},{     2,BADDIR},{     3,BADDIR}}, // intersection
	{{     0,     1},{BADDIR,BADDIR},{     2,BADDIR},{     2,BADDIR}}, // rightfork
	{{     0,     3},{     2,BADDIR},{     2,BADDIR},{BADDIR,BADDIR}}, // leftfork
};

pointi2 rot2pnt[4]={
	{ 1, 0}, // rot 0, right
	{ 0,-1}, // rot 1, down
	{-1, 0}, // rot 2, left
	{ 0, 1}, // rot 3, up
};

S32 pointi22rot(const pointi2& p)
{
	if (p.y==0) {
		if (p.x==1) {
			return 0;
		} else if (p.x==-1) {
			return 2;
		}
	} else if (p.y==1) {
		if (p.x==0) {
			return 3;
		}
	} else if (p.y==-1) {
		if (p.x==0) {
			return 1;
		}
	}
	errorexit("bad pointi22rot with %d %d",p.x,p.y);
	return BADDIR;
}

pointi2 rot2pointi2(S32 rot)
{
	if (rot<0 || rot>3)
		errorexit("bad rot2point2i with %d",rot);
	pointi2 r=rot2pnt[rot];
	return r;
}

S32 rotmod(S32 a,S32 b) // mod 4
{
	return (a+b)&3;
}

// handles laps and checkpoints, called by gametick, TW
void dorabbits(U32 slot)
{
	const trkt* ot = gg->e.oldtrackj;
	twcaro* tw=&gg->caros[slot].cs;
	pointi2 cpos;
	S32 pt;
	S32 rot;
	pointf3 off;
	ot->curpiece(&tw->pos,&off,&cpos,&pt,&rot);
	S32 i;
	bool newcheckpoint = false;
	for (i=0;i<2;++i) {
		if (cpos.x==tw->lastloc.x && cpos.y==tw->lastloc.y) { // on last valid piece before checkpoint
			if (pt==PCE_STARTFINISH) { // handle lap
//				if (gg->viewslot==slot)
//					gg->con->printf("SF (%f,%f,%f)\n",off.x,off.y,off.z);
				bool isgreater = off.x >= PIECESIZE*.5f;
				bool addlap = false;
				switch(tw->lapratchet) {
				case twcaro::READY:
					if (isgreater)
						tw->lapratchet = twcaro::GREATER;
					else
						tw->lapratchet = twcaro::LESS;
					break;
				case twcaro::DONE:
					break;
				case twcaro::LESS:
					if (isgreater) {
						addlap = true;;
						tw->lapratchet = twcaro::DONE;
					} else {
					}
					break;
				case twcaro::GREATER:
					if (isgreater) {
					} else {
						addlap = true;
						tw->lapratchet = twcaro::DONE;
					}
					break;
				}
				if (addlap) {
					++tw->curlap;
				}
			} else {
				tw->lapratchet=twcaro::READY;
			}
		}
		if ((i < tw->nnextloc) && (cpos.x == tw->nextloc[i].x) && (cpos.y == tw->nextloc[i].y)) { // reached checkpoint
			pointi2 in;
			in.x = cpos.x - tw->lastloc.x;
			in.y = cpos.y - tw->lastloc.y;
			S32 inrotw = pointi22rot(in);
			S32 inrotp = rotmod(inrotw,-rot); // to piece space
			S32 outrot = nextlocsrot[pt][inrotp][0];
			tw->nnextloc = 0;
			if (outrot != BADDIR) {
				outrot = rotmod(outrot,rot); // back to world space
				pointi2 out = rot2pointi2(outrot);
				tw->nextloc[0].x = cpos.x + out.x; // new checkpoint 0
				tw->nextloc[0].y = cpos.y + out.y;
				tw->nnextloc=1;
			}
			S32 outrot2 = nextlocsrot[pt][inrotp][1];
			if (outrot2 != BADDIR) {
				outrot2 = rotmod(outrot2,rot); // back to world space
				pointi2 out2 = rot2pointi2(outrot2);
				tw->nextloc[1].x = cpos.x + out2.x; // new checkpoint 1 (for forks)
				tw->nextloc[1].y = cpos.y + out2.y;
				tw->nnextloc=2;
			}
			tw->lastloc.x=cpos.x;
			tw->lastloc.y=cpos.y;
			newcheckpoint = true;
			if (true)
//			if (gg->viewslot==slot)
				if (!gg->predicted)
//					gg->con->printf("player %d, lap %d, nsf %d",
//					  slot,tw->curlap,ot->pieces[cpos.y][cpos.x].dist2[inrotw]);
					gg->plc->update(
					  slot,tw->curlap,
					  ot->pieces[cpos.y][cpos.x].dist2[inrotw],
					  gg->gs.clock-gg->gs.clockoffset);
//					gg->con->printf("ll %d,%d nl %d,%d inrotw %d nsf %d",
//					tw->lastloc.x,tw->lastloc.y,tw->nextloc[0].x,tw->nextloc[0].y,inrotw,ot->pieces[cpos.y][cpos.x].dist2[inrotw]);
		}
	}
// watchdog new checkpoint
	if (newcheckpoint)
		tw->nocheckpointtime = 0;
	else
		++tw->nocheckpointtime;
// end race senarios
	if (!tw->fintime) { // finish race if not finished
//		if (gg->gs.clock-gg->gs.clockoffset>5) {	// almost right away
//		if (false) {								// never
 		if (tw->curlap>gg->e.nlaps) {				// done the laps
			tw->fintime=gg->gs.clock-gg->gs.clockoffset-acv2.candrivetime; // record finish race
			tw->finplace=gg->gs.nextplace;
			if (!gg->predicted && tw == &gg->caros[gg->viewslot].cs) {
				switch(tw->finplace) {
				case 1:
					sfx->playsnd(0,FINISH1);
					break;
				case 2:
					sfx->playsnd(0,FINISH2);
					break;
				case 3:
					sfx->playsnd(0,FINISH3);
					break;
				default: // 4th and beyond
					sfx->playsnd(0,FINISH4);
					break;
				}
			}
			++gg->gs.nextplace;
			if (!gg->predicted) {
				gg->con->printf("player %2d, place %2d, time '%s'",slot,tw->finplace,getmsh(tw->fintime).c_str());
			}
		}
	}
}

// endgame scenarios (rules)
void checkendgamerabbits()
{
	static S32 rematchcounter;
	if (!gg->predicted) { // only when sure, turn on and off rematch UI, don't change TW variables here
		S32 i;
		S32 donecount = 0;
		for (i=0;i<gg->numplayers;++i) {
//			if (gg->caros[i].cs.curlap>gg->e.nlaps) { // done racing, all players
//			if (gg->caros[i].cs.fintime) { // done racing, all players
			if (gg->caros[i].cs.fintime || gg->caros[i].ownerid==-1) { // done racing, just human players
				++donecount;
			}
//			gg->con->printf("%2d: rematchyes %d",i,gg->caros[i].cs.rematchyes);
		}
// all players done, trigger rematch logic
		if (donecount == gg->numplayers) { // everyone important done racing
			if (!gg->caros[gg->yourid].cs.rematchyes) { // if no selection yet   'yes' 'no' 
				if (rematchcounter==acv2.rematchtime*TICKRATE) // turn on
					rematchon();
				if (rematchcounter==acv2.norematchtime*TICKRATE) // turn off, exit game 'no'
					setrematchbuts(NO);
				++rematchcounter;
			} else {
				rematchcounter=0; // 'yes'
				rematchoff();
			}
		} else {
			rematchcounter=0; // no rematch yet
			rematchoff();
		}
	}
}
