// a class to handle all networking aspects of any network game, hiding socker layer
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
#include "gamerunj.h"
#include "netobj.h"
#include "connectpackets.h"
#include "netobjserver.h"
#include "netobjclient.h"
#include "netobjbot.h"

#include "match_logon.h"

//base constructor
netobj::netobj(gamedescj* gd) :cgd(gd),grj(0),owngamerun(true),/*yourid(-1),*/queoffset(0),letsgo(false) // connection: get lobby gamedesc setup
{
	grj = new gamerunj();
	
	grj->n=cgd->n;
//	grj->n.gamenet=gd->n.gamenet;//GAMENET_BOTGAME;
	grj->e=cgd->e;
	grj->yourid=0; // your computer id
	grj->yourcntl=CNTL_HUMAN;
	grj->viewslot=0;
	grj->numplayers=cgd->nhumanplayers+cgd->nbotplayers;
// copy over car 0, make remaining bot cars
	grj->caros.resize(grj->numplayers);
	grj->caros[0]=cgd->a;
	if (matchcl && cgi.isvalid) {
		grj->caros[0].playername=username;
		cgd->a.playername=username;
	}
	grj->caros[0].ownerid=0;
	for (S32 i=1;i<grj->numplayers;++i) { // beyond the clients
		grj->caros[i].randombot(); // lie the extra bots
	}
}

// factory
netobj* netobj::create(gamedescj* gd)
{
	if (gd->n.gamenet==GAMENET_BOTGAME)
		return new netbotgameobj(gd);
	else if (gd->n.gamenet==GAMENET_SERVER)
		return new netserverobj(gd);
	else if (gd->n.gamenet==GAMENET_CLIENT)
		return new netclientobj(gd);
	else {
		errorexit("can't create netobj %d",gd->n.gamenet);
		return 0;
	}
}
// connect: pass ownership of gamerunj to caller
gamerunj* netobj::getgamerunj()
{
	gamerunj* grjt=grj;
//	grj=0; // let netobj keep a reference to grj
	owngamerun = false; // will not delete, owner deletes
	grjt->plc = new placer(grjt->numplayers);
	return grjt;
}
void netobj::buildkeyques()
{
	keyques.resize(grj->numplayers);
	genques.resize(grj->numplayers);
}
// ingame: send key to collective
void netobj::writekey(const keys& key)
{
	keyques[grj->yourid].push_back(key.getpack());
}

// ingame: human, get key from slot at time starts at 0
bool netobj::canreadkey(U32 slot,S32 when)
{
	if (slot >= keyques.size())
		return false;
	S32 ss = keyques[slot].size();
	S32 ki = when - queoffset;
	if (ki >= ss)
		return false;
	if (ki < 0)
		return false;
	return true;//keyques[slot][when-queoffset];
}
// ingame: human, get key from slot at time starts at 0
keys netobj::readkey(U32 slot,S32 when)
{
	return keyques[slot][when-queoffset];
}
keys netobj::readlastkey(U32 slot)
{
	S32 lst = keyques[slot].size()-1;
	if (lst<0)
		return keys();
	return keyques[slot][lst];
}
// ingame: human, time of last valid key, starts at -1
S32 netobj::lastvalidkeytime(U32 slot)
{
	return keyques[slot].size()+queoffset-1;
}
// ingame: time of last valid key from any player, starts at '-1', when '0' then all players arrived
S32 netobj::lastvalidtime()
{
	S32 i,n=keyques.size();
	bool hasmintime = false;
	S32 mintime = -1;
	for (i=0;i<n;++i) { // number of queues
		if (grj->caros[i].ownerid == -1) 
			continue;
		S32 fi=lastvalidkeytime(i);
		if (!hasmintime || fi<mintime) {
			mintime = fi;
			hasmintime = true;
		}
	}
	return mintime;
}
// ingame: time of most futuristic valid key from any player, but not you
S32 netobj::farthestvalidtime(S32 yid)
{
	S32 i,n=keyques.size();
	bool hasmaxtime = false;
	S32 maxtime = lastvalidkeytime(yid);
	for (i=0;i<n;++i) { // number of queues
		if (grj->caros[i].ownerid == -1 || grj->caros[i].ownerid == yid)
			continue;
		S32 fi=lastvalidkeytime(i);
		if (!hasmaxtime || fi>maxtime) {
			maxtime = fi;
			hasmaxtime = true;
		}
	}
	return maxtime;
}
// ingame: remove early key data, earlier than this time are no longer needed (langoliers)
void netobj::clearearlykeydata(S32 t)
{
	t=t-1; // for read last key...
	S32 i,j,n=keyques.size();
	S32 dt=t-queoffset;
	for (i=0;i<n;++i) { // number of queues
		for (j=0;j<dt;++j) { // number of time elements to delete
			if (!keyques[i].empty()) {
				keyques[i].pop_front();
//			} else {
//				errorexit("already keyqueue empty");
			}
		}
	}
	queoffset=t;
}

void netobj::drawkeybuff()
{
#define MUL 1
#define DIV 1
	int i,n=keyques.size();
	int lf,rt;
#define OFFX 0
#define SIZEX 240
	cliprect32(B32,OFFX,WY-16,256+SIZEX,WY-1,C32DARKGRAY);
	for (i=0;i<n;++i) {
		if (grj->caros[i].ownerid==-1)
			continue;
		const deque<U8>& k=keyques[i];
		lf=0;
		rt=k.size();
		cliprect32(B32,OFFX+lf*MUL/DIV,WY-16+2*i+1,rt*MUL/DIV-lf*MUL/DIV+1,1,C32WHITE);
	}
}

void netobj::drawkeybuff3d()
{
#define MUL 1
#define DIV 1
	int i,n=keyques.size();
	int lf,rt;
#define OFFX 0
#define SIZEX 240
//	cliprect32(B32,OFFX,WY-16,256+SIZEX,WY-1,C32DARKGRAY);
	video_sprite_draw(0,F32DARKGRAY,OFFX,(float)WY-24,256+(float)SIZEX,(float)WY-1);
	for (i=0;i<n;++i) {
		if (grj->caros[i].ownerid==-1)
			continue;
		const deque<U8>& k=keyques[i];
		lf=0;
		rt=k.size();
//		cliprect32(B32,OFFX+lf*MUL/DIV,WY-16+2*i+1,rt*MUL/DIV-lf*MUL/DIV+1,1,C32WHITE);
		video_sprite_draw(0,F32WHITE,(float)OFFX+lf*MUL/DIV,(float)WY-24+2*i+1,(float)rt*MUL/DIV-lf*MUL/DIV+1,1);
	}
}

void netobj::writegen(PACK_KIND id,const vector<U8>& mess)
{
	message m;
	m.id=id;
	m.data=mess;
	genques[grj->yourid].push_back(m);
}

bool netobj::readgen(U32 slot,PACK_KIND& id,vector<U8>& mess)
{
	if (genques[slot].empty())
		return false;
	const message& m=genques[slot].front();
	id=m.id;
	mess=m.data;
	genques[slot].pop_front();
	return true;
}
