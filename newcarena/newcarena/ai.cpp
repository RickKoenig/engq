#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "utils/json.h"
#include "netdescj.h"
#include "hash.h"
#include "constructor.h"
#include "timewarp.h"
#include "envj.h"
#include "enums.h"
#include "carclass.h"
#include "avaj.h"
#include "gamedescj.h"
#include "carenagame.h"
#include "gamerunj.h"
#include "physics.h"

//#define GBK_VERBOSE
// read only, don't change cs
// TW and non TW
keys getbotkey(int pn)
{
	const twcaro* cs=&gg->caros[pn].cs; // get car/obj gamestate
	pointf3xh headv(0,0,1,0);
	quatrot(&cs->rot,&headv,&headv);
#ifdef GBK_VERBOSE
	if (gg->viewslot == pn && !gg->predicted)
		con32_printf(gg->con,"headv '%s'\n",headv.log().c_str());
#endif
//tw->fintime=gg->gs.clock-gg->gs.clockoffset-gcfg.candrivetime
	bool botwait = false; // short wait after finish before rematchyes triggered
	keys k;
// finished
	if (cs->fintime) {
		const S32 BOTWAITTIME=6*60;
		botwait = gg->gs.clock-gg->gs.clockoffset-acv2.candrivetime>=cs->fintime+BOTWAITTIME; 
	}
	k.krematchyes = botwait;
#if 1 // true bots always rematch, even early, maybe not (are bots important?)
	if (gg->caros[pn].ownerid==-1) { // don't wait for true bots to finish, they want a rematch quite early, don't wait for bots
		k.krematchyes = true;
	}
#endif
#ifdef FULLPHYSICS
	if (cs->noshocktime >= acv2.maxnoshocktime || cs->nocheckpointtime>= acv2.maxnocheckpointtime) {
		k.kresetcar = true;
	}
#endif
//#define NOAI
#ifdef NOAI
	return k; // bots don't do anything
#endif
	if (cs->nnextloc > 0) {
		S32 idx = 0; // head for next (straight on fork) piece
		if (cs->nnextloc > 1) { // a fork choice
			frk fs = gg->e.oldtrackj->pieces[cs->lastloc.y][cs->lastloc.x].forkstate;
			switch(fs) {
			case FNONE:
			case FBOTH:
				{
					S32 td = (pn<<7) + gg->gs.clock;
					td >>= 8;
					if (td & 1) { // for now: select fork path based on player number/id and clock
						idx = 1;
					}
				}
				break;
			case FSTRAIGHT:
				idx = 0;
				break;
			case FTURN:
				idx = 1;
				break;
			}
		}
//		if (!gg->predicted)
//			gg->con->printf("td %d, td7 %d",td,tds7);
		pointf3xh nl;
		nl.x=PIECESIZE*(cs->nextloc[idx].x*1.0f+.5f); // middle of piece
		nl.z=PIECESIZE*(cs->nextloc[idx].y*1.0f+.5f);
		pointf3xh rabvec(nl.x-cs->pos.x,0,nl.z-cs->pos.z);
		normalize3d(&rabvec,&rabvec);
		float d=dot3d(&rabvec,&headv);
		pointf3xh c;
		cross3d(&rabvec,&headv,&c);
#ifdef GBK_VERBOSE
		if (gg->viewslot == pn && !gg->predicted) {
			con32_printf(gg->con,"rabvec '%s', dot %f\n",rabvec.log().c_str(),d);
			con32_printf(gg->con,"cross '%s'\n",c.log().c_str());
		}
#endif
		const float dthresh = .15f;
//		if (d>=0) {
		if (true) {
			k.ku=1;
		} else {
			k.kd=1;
		}
		if (c.y>dthresh) {
			if (k.ku==1)
				k.kl=1;
			else
				k.kr=1;
		} else if (c.y<-dthresh) {
			if (k.ku==1)
				k.kr=1;
			else
				k.kl=1;
		} else if (d<0) {
			k.kr=1; // turn if 180 backwards
		}
	}
	if (cs->fintime)
		k.ku=k.kd=k.kl=k.kr=0;

//	k.ku = 0; // don't drive
	return k;
#if 0
	// pos left
	pointi2 ppos;
	S32 pt;
	S32 rot;
	const trk* t=gg->e.oldtrackj; // get track
	const twcaro* cs=&gg->caros[pn].cs; // get car/obj gamestate
	t->curpiece(&cs->pos,&ppos,&pt,&rot); 
	if (!gg->predicted) {
		con32_printf(gg->con,"clk %d, s %d,p %d,%d, pt %d, rot %d\n",gg->gs.clock,pn,ppos.x,ppos.y,pt,rot);
	}
#endif
}
