#define RES3D // alternate shape class
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
#include "scrline.h"

// gamerunj
void gamerunj::hashadd(const hashi& h) const
{
	gs.hashadd(h);
	S32 i,n=gg->numplayers;
	for (i=0;i<n;++i)
		caros[i].cs.hashadd(h);
}

void gamerunj::log(const vector<keys>& ks) const
{
	logger("global game log\n");
	logger_indent();
	logger("clock %d\n",gg->gs.clock);
	logger("clockoffset %d\n",gg->gs.clockoffset);
	S32 i;
	for (i=0;i<gg->numplayers;++i) {
		logger("player %d, keys '%s'\n",i,ks[i].log().c_str());
		logger_indent();
		gg->caros[i].cs.log();
		logger_unindent();
	}
	logger_unindent();
}

bool fs::fcomp(const fs& lhs,const fs& rhs)
{
	if (lhs.lap != rhs.lap)
		return lhs.lap > rhs.lap; // highest laps first
	if (lhs.nsf != rhs.nsf)
		return lhs.nsf < rhs.nsf; // lowest dist to next s/f piece next
	return lhs.cnt < rhs.cnt; // lowest cnt/id next
}

placer::placer(S32 nplayersa) : nplayers(nplayersa),st(fs::fcomp),plc(nplayersa,fs(-1,0,0,0,0))
{
	cnt = 0;
}

void placer::update(S32 slot,S32 laps,S32 dist,S32 clk)
{
	if (slot>=nplayers)
		errorexit("bad nplayers in placer");
	const fs& f = plc[slot];
	if (f.slot >= 0) {
		st.erase(f);
	}
	fs fsn(slot,laps,dist,++cnt,clk);
	st.insert(fsn);
	set<fs,fp>::iterator fnd = st.find(fsn);
	if (fnd == st.begin()) { // 1st place, leave a breadcrumb
		bcs.push_back(fsn);
//		gg->con->printf("1st %d, lap %d, nsf %d, time %s",
//			slot,laps,dist,getmsh(clk).c_str());
//	} else if (st.size()>=2 && fnd == ++st.begin()) {
//		bcs2.push_back(fsn);
//		gg->con->printf("2nd %d, lap %d, nsf %d, time %s",
//			slot,laps,dist,getmsh(clk).c_str());
	}
	plc[slot] = fsn;
}

void placer::reset()
{
	plc.assign(nplayers,fs(-1,0,0,0,0));
	cnt = 0;
	st.clear();
	bcs.clear();
}

S32 placer::getplace(S32 slot) // return 0 if not ready yet
{
	if (plc[slot].slot == -1)
		return 0;
	S32 i = 1;
	set<fs,fp>::iterator it = st.begin();
	while(it != st.end()) {
		if (it->slot == slot)
			return i;
		++i;
		++it;
	}
	return 0;
}

S32 placer::gettime(S32 slot) // return 0 if not ready yet
{
	const fs& sy = plc[slot]; // your last 'update' call
	if (sy.slot == -1)
		return 0; // nothing in plc, no update calls yet
	if (st.find(sy) == st.begin()) { // in first, look at 2nd
		if (st.size()<2) {
			return 0;
		}
		const fs& s2 = *(++st.begin()); // 2nd place
		return -gettime(s2.slot); // tricky
	}
	vector<fs>::iterator it = bcs.begin();
	while(it != bcs.end()) {
		const fs& sf = *it; // first place, breadcrumbs
		if (sf.lap == sy.lap && sf.nsf == sy.nsf)
			return sy.time - sf.time;
		++it;
	}
	return 0; // breadcrumb not found
}

S32 placer::cnt;
