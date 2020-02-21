#include <iomanip>

#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "carclass.h"
#include "hash.h"
#include "timewarp.h"
#include "avaj.h"
#include "constructor.h"
#include "envj.h"
#include "netdescj.h"
#include "gamerunj.h"
#include "carenagame.h"
#include "gamedescj.h"
#include "netobj.h"
#include "gamestatecheck.h"
//#define LOGGAMESTATE

void pointf3xh::hashadd(const hashi& h) const
{
#if 0
	h.add((U8*)&x,sizeof(x));
	h.add((U8*)&y,sizeof(y));
	h.add((U8*)&z,sizeof(z));
	h.add((U8*)&w,sizeof(w));
#else
	h.add((U8*)this,sizeof(*this));
#endif
}

string pointf3xh::log() const
{
	stringstream ss;
//	ss << "< " << float2strhex(x) << " , " << float2strhex(y) << " , " << float2strhex(z) << " , " << float2strhex(w) << " >";
	ss << "< " << x << " , " << y << " , " << z << " , " << w << " >";
	return ss.str();
}

string pointf3xh::logh() const
{
	stringstream ss;
//	ss << setw(8);
//	ss << setfill('0');
	ss << hex;
	U32* ix=(U32*)&x;
	U32* iy=(U32*)&y;
	U32* iz=(U32*)&z;
	U32* iw=(U32*)&w;


//	ss << "< " << float2strhex(x) << " , " << float2strhex(y) << " , " << float2strhex(z) << " , " << float2strhex(w) << " >";
	ss << "< " << *ix << " , " << *iy << " , " << *iz << " , " << *iw << " >";
	return ss.str();
}

// hashcheck(md5) and log complete gamestate
void twgg::hashadd(const hashi& h) const
{
	h.add((U8*)&clock,sizeof(clock));
	h.add((U8*)&clockoffset,sizeof(clockoffset));
	h.add((U8*)&gamenum,sizeof(gamenum));
	h.add((U8*)&nextplace,sizeof(nextplace));
}

void twcaro::hashadd(const hashi& h) const
{
	h.add((U8*)&lastloc,sizeof(lastloc));
	if (nnextloc>0)
		h.add((U8*)&nextloc[0],sizeof(nextloc[0]));
	if (nnextloc>1)
		h.add((U8*)&nextloc[1],sizeof(nextloc[1]));
	h.add((U8*)&nnextloc,sizeof(nnextloc));
	h.add((U8*)&curlap,sizeof(curlap));
	h.add((U8*)&lapratchet,sizeof(lapratchet));
	h.add((U8*)&finplace,sizeof(finplace));
	h.add((U8*)&fintime,sizeof(fintime));
	h.add((U8*)&rematchyes,sizeof(rematchyes));
	pos.hashadd(h);
	rot.hashadd(h);
	vel.hashadd(h);
	rotvel.hashadd(h);
//	normj.hashadd(h);
//	lastpos.hashadd(h);
	h.add((U8*)&shocklen[0],sizeof(shocklen[0]));
	h.add((U8*)&shocklen[1],sizeof(shocklen[1]));
	h.add((U8*)&shocklen[2],sizeof(shocklen[2]));
	h.add((U8*)&shocklen[3],sizeof(shocklen[3]));
	h.add((U8*)&accelspin,sizeof(accelspin));
	h.add((U8*)&steertime,sizeof(steertime));
	h.add((U8*)&noshocktime,sizeof(noshocktime));
	h.add((U8*)&nocheckpointtime,sizeof(nocheckpointtime));
	h.add((U8*)&noresettime,sizeof(noresettime));
}

void twcaro::log() const
{
	logger("lastloc %d,%d\n",lastloc.x,lastloc.y);
	if (nnextloc>0)
		logger("nextloc %d,%d\n",nextloc[0].x,nextloc[0].y);
	if (nnextloc>1)
		logger("nextloc %d,%d\n",nextloc[1].x,nextloc[1].y);
//	logger("stpceloc %d,%d\n",stpceloc.x,stpceloc.y);
	logger("curlap %d\n",curlap);
	logger("lapratchet %d\n",lapratchet);
	logger("pos %s\n",pos.logh().c_str());
	logger("rot %s\n",rot.logh().c_str());
	logger("vel %s\n",vel.logh().c_str());
	logger("rotvel %s\n",rotvel.logh().c_str());
//	logger("normj %s\n",normj.logh().c_str());
	logger("shocklen[0] %f\n",shocklen[0]);
	logger("shocklen[1] %f\n",shocklen[1]);
	logger("shocklen[2] %f\n",shocklen[2]);
	logger("shocklen[3] %f\n",shocklen[3]);
	logger("accelspin %f\n",accelspin);
	logger("steertime %f\n",steertime);
	logger("noshocktime %d\n",noshocktime);
	logger("nocheckpointtime %d\n",nocheckpointtime);
	logger("noresettime %d\n",noresettime);
//	logger("rematchyes %d\n",rematchyes);
//	logger("seekmode %d\n",seekmode);
}

// save and restore gamestate
static void twsave0()
{
	gg->gs0=gg->gs;
	S32 i,n=gg->numplayers;
	for (i=0;i<n;++i)
		gg->caros[i].cs0=gg->caros[i].cs;
}

void twrestore0() // rematch, reset game
{
// special
	S32 c = gg->gs.clock;
	S32 co = gg->gs.clockoffset;
	S32 gn = gg->gs.gamenum;
// general
	gg->gs = gg->gs0;
// special
	gg->gs.clock = c; // keep
	gg->gs.clockoffset = c;
	gg->gs.gamenum = gn + 1;
// players
	S32 i,n=gg->numplayers;
	for (i=0;i<n;++i)
		gg->caros[i].cs = gg->caros[i].cs0;
}

static void twsaven()
{
	gg->gsn=gg->gs;
	S32 i,n=gg->numplayers;
	for (i=0;i<n;++i)
		gg->caros[i].csn=gg->caros[i].cs;
}

static void twrestoren()
{
	gg->gs=gg->gsn;
	S32 i,n=gg->numplayers;
	for (i=0;i<n;++i)
		gg->caros[i].cs=gg->caros[i].csn;
}

void twsave()
{
	twsave0();
	twsaven();
}

// move gamestate forward uloop times
void timewarp(S32 uloop)
{
// TIME WARP
	S32 oldclock = gg->gs.clock;
	S32 newclock = oldclock + uloop; // move realtime game this far
// backup time if new data arrived since last nonpredicted time (current time)
// 1st half: move the non predicted part forward
	gg->predicted = false;
// gsn contains the last non predicted time
	S32 lastvalidkeytime = no->lastvalidtime(); // test, get updated 'lastvalidkeytime'
	S32 nonpredcount = lastvalidkeytime - gg->gsn.clock + 1; // can move forward non predicted part by this amount, all key data available
	if (nonpredcount > 0) {
		twrestoren(); // restore non predicted state
		S32 i;
		for (i=0;i<nonpredcount;++i) {
			vector<keys> ks = getkeys();
			gg->intimewarp = gg->gs.clock < oldclock;
#ifdef CHECKGAMESTATE
			gs->writegamestate(ks); // only write out the nonpredicted game states
#endif
#ifdef LOGGAMESTATE
			gg->log(ks); // always log
#else
			if (gg->bail) // log only when bailing
				gg->log(ks);
#endif
			gametick(ks);//,false,intimewarp);
		}
		twsaven(); // save last non predicted state
		no->clearearlykeydata(gg->gs.clock); // don't need keystrokes earlier than this time (langoliers)
	}
// 2nd half: move the predicted part forward, get to current time
	gg->predicted = true;
	while(gg->gs.clock<newclock) {
		vector<keys> ks = getkeys();
		gg->intimewarp = gg->gs.clock < oldclock;
		gametick(ks);//,true,intimewarp);
	}
// end TIME WARP
}
