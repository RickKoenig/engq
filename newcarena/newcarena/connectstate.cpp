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
#include "netobj.h"
#include "system/u_states.h"

#include "match_logon.h"
#include "netobjserver.h"

static gamedescj* gp;
gamecfgn gcfg;
static con32 *con;
static bool goingtogame;
static int timeout;

void carenaconnectinit()
{
	logger("---------------------- carenagameinit -------------------------------\n");
	video_setupwindow(GX,GY);
	mt_setseed(getmicrosec());
	goingtogame=false;
pushandsetdir("newcarenadata");
// build local gp
pushandsetdirdown("carenalobby");
json js(fileload_string(wininfo.isalreadyrunning ? "gamedescb.txt" : "gamedesc.txt"));
popdir();
	gp = new gamedescj(js);
	gp->e.buildtrackhash();
	no = netobj::create(gp); // class factory bot,server,client,broadclient etc..
	gg = 0;
// show local gp
	con=con32_alloc(WX,WY,C32BLACK,C32WHITE);
	con32_printf(con,"CARENACONNECTINIT:\n");
	con32_printf(con,"LOADGP: humplayers %d, gametype '%s'\ntrackname '%s', rules '%s'\ncarbody '%s'\n",
		gp->nhumanplayers,gamenet_names[gp->n.gamenet],gp->e.trackname.c_str(),rule_names[gp->e.rules],gp->a.carbodyname.c_str());
//	con32_printf(con,"LOADGP: humplayers %d, gametype '%s'\ntrackname '%s', rules '%s'\nplayername '%s', carbody '%s'\n",
//		gp->nhumanplayers,gamenet_names[gp->n.gamenet],gp->e.trackname.c_str(),rule_names[gp->e.rules],gp->a.playername.c_str(),gp->a.carbodyname.c_str());
	string hstr = gp->e.trackhash.dump();
	con32_printf(con,"%s\n",hstr.c_str());
}

// send gameinfo back to matching server
static void gameinfoupdate()
{
	if (matchcl && cgi.ishost) { // if here from matchlobby and a host, then send a gamepicked token to matching server
// gamepicked hostname gamename curplayers ip port
		const C8* ipstr=ip2str(cgi.ip);
		C8 token[300];
		const C8* gn=getgamename(cgi.gameidx);
		netserverobj* nos = dynamic_cast<netserverobj*>(no);
		if (!nos) {
			return;
//			errorexit("can't dynamic_cast to netserverobj");
		}
		if (nos->getmaster() && nos->getcurclientschanged()) {
			sprintf(token,"gamepicked %s %d/%d %s %d",gn,nos->getcurclientsloggedon(),nos->getnumhumanplayers(),ipstr,nos->getmaster()->port);
			U32 len=strlen(token);
			tcpwritetoken32(matchcl,0,token,len);
			tcpsendwritebuff(matchcl);
		}
	}
}

static void gameinfoclear()
{
	if (matchcl && cgi.ishost) {
// gamepicked
		char* gp="gamepicked";
		tcpwritetoken32(matchcl,0,gp,strlen(gp));
		tcpsendwritebuff(matchcl);
	}
}

// go from gp to gg
void carenaconnectproc()
{
	if (KEY==K_ESCAPE) {
		poporchangestate(STATE_NEWCARENALOBBY);
		return;
	}
	string ret=no->proc();
	con32_printf(con,"%s",ret.c_str());
	netserverobj* nos = dynamic_cast<netserverobj*>(no);
	if (nos) {
		gameinfoupdate();
	}
	if (no->isready()) {
		gg=no->getgamerunj(); // transition from connection to ingame
		goingtogame=true;
		changestate(STATE_NEWCARENASTATUS);
	}
}

void carenaconnectdraw2d()
{
	clipblit32(con32_getbitmap32(con),B32,0,0,0,0,WX,WY);
}

void carenaconnectexit()
{
	logger("---------------------- carenagameexit -------------------------------\n");
	gameinfoclear(); // no more 'joins'
	delete gp;
	gp=0;
	if (!goingtogame) { // free up sockets on a bail
		delete gg;
		gg=0;
		delete no;
		no=0;
		if (con) {
			con32_free(con);
			con=0;
		}
	}
popdir();
}

void carenastatusinit()
{
	static S32 statustimetable[]={0}; // added to statustime for player arrival test
//	static S32 statustimetable[]={1,3,5,7,2,8}; // added to statustime for player arrival test
//	static S32 statustimetable[]={0,40,10,50,20,60}; // added to statustime for player arrival test
	const S32 statustimetablesize = sizeof(statustimetable)/sizeof(statustimetable[0]);
	int i;
	goingtogame=false;
	con32_printf(con,"\n------- STATUS -------------------\n");
	con32_printf(con,"yourid %d, yourview %d\n",gg->yourid,gg->viewslot);
	for (i=0;i<gg->numplayers;++i)
		con32_printf(con,"slot %2d: owner %2d, name '%-12s', car '%-12s'\n",
		i,gg->caros[i].ownerid,gg->caros[i].playername.c_str(),gg->caros[i].carbodyname.c_str());
	int framestart=getmillisec();
	S32 tt=0;
	if (gg->yourid<statustimetablesize)
		tt=statustimetable[gg->yourid];
	timeout=framestart+1000*(gcfg.statustime+tt); // test
}

void carenastatusproc()
{
	if (KEY==K_ESCAPE) {
		poporchangestate(STATE_NEWCARENALOBBY);
		return;
	}
	int frame=getmillisec();
	if (frame>=timeout) {
		changestate(STATE_NEWCARENAGAME);
		goingtogame=true;
	}
}

void carenastatusdraw2d()
{
	clipblit32(con32_getbitmap32(con),B32,0,0,0,0,WX,WY);
}

void carenastatusexit()
{
	con32_free(con);
	if (!goingtogame) { // free up sockets on a bail
		delete gg;
		gg=0;
		delete no;
		no=0;
	}
}
