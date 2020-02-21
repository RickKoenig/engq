/*
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <engine7cpp.h>
#include <misclib7cpp.h>

#include "mainmenucpp.h"
#include "videoicpp.h"
#include "usefulcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "carenagamecpp.h"
#include "jrmcarscpp.h"
#include "packetcpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carclass.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"
#include "n_aicpp.h"
#include "n_scrlinecpp.h"
#include "n_physicscpp.h"
#include "n_rematchcpp.h"
#include "n_carenaconnectcpp.h"
#include "n_line2roadcpp.h"
#include "n_twobjcpp.h"
//namespace n_physics {}

struct gameglobal gg;
struct caro gos[MAX_GAMEOBJ];
static struct viewport2 game_viewport,scrline_viewport;
#define CONSIZEX 256
#define CONSIZEY 128
#define CONLOCX 0
#define CONLOCWYMY 0
bool gamenews=false;

static struct menuvar gamevars[]={
	{"@lightred@---- game cam -------------------",0,D_VOID,0},
	{"game cam trans x",&game_viewport.camtrans.x,D_FLOAT,FLOATUP/8},
	{"game cam trans y",&game_viewport.camtrans.y,D_FLOAT,FLOATUP/8},
	{"game cam trans z",&game_viewport.camtrans.z,D_FLOAT,FLOATUP/8},
	{"game cam rot x",&game_viewport.camrot.x,D_FLOAT,FLOATUP/8},
	{"game cam rot y",&game_viewport.camrot.y,D_FLOAT,FLOATUP/8},
	{"game cam rot z",&game_viewport.camrot.z,D_FLOAT,FLOATUP/8},
	{"game cam zoom",&game_viewport.camzoom,D_FLOAT,FLOATUP/8},
};
static const S32 ngamevars=sizeof(gamevars)/sizeof(gamevars[0]);
//////////////////////////////////////// load and play game ///////////////////////////////////////////////////////
/*
static void loadingcallback()
{
//	char *t;
//	int tx,ty;
	int x0,y0,xs0,ys,xs;
	gg.loadingcount++;
	if ((gg.loadingcount&0x3f)!=1)
		return;
	video_lock();
	clipblit16(gg.loadingbackground,B16,0,0,0,0,WX,WY);

	x0=50+20*8;
	y0=WY-13;
	xs0=100;
	ys=8;
	outtextxyf16(B16,50,y0,hiwhite,"Loading Assets");
	cliprecto16(B16,x0-1,y0-1,x0+xs0+1,y0+ys,hiwhite);
	xs=xs0*gg.loadingcount/gcfg.loadinggoal;
	if (xs>xs0)
		xs=xs0;
	logger("loadingcount %d/%d\n",xs,xs0);
	cliprect16(B16,x0,y0,x0+xs-1,y0+ys-1,hiwhite);
	video_unlock();
	video_paintwindow(0);
}
*/
static void loadtrack()
{
	bitmap32* bm;
	char str[50];
	gg.loadingbackground=bitmap32alloc(WX,WY,C32LIGHTGRAY);
//	setloadcallback(loadingcallback);
	if (gp.gt.ti.validtrackhash==TRACKTYPE_OLD) { // tile based track
pushandsetdirdown("constructed");
		gg.thetrack=buildconsscene(&gp.gt.gametrack);
		if (!gg.thetrack)
//			gg.thetrack=alloctree(0,0);
			gg.thetrack=new tree2("oldtrack");
	} else if (gp.gt.ti.validtrackhash==TRACKTYPE_NEW) {
pushandsetdirdown("constructed"); // freeform track
		gg.thetrack=buildnewconsscene(gp.gt.newgametrack);
		if (!gg.thetrack)
//			gg.thetrack=alloctree(0,0);
			gg.thetrack=new tree2("newtrack");
		st2_loadcollist("collist.txt",gg.thetrack);
	} else { // 
		sprintf(str,"tracks/%s",gp.gt.ti.trackname);
pushandsetdirdown(str);
		sprintf(str,"%s.jpg",gp.gt.ti.trackname);
		if (fileexist(str)) {
/*			fileopen(str,READ);
			bm=gfxread16(BPP);
			clipscaleblit32(bm,gg.loadingbackground,0,0,bm->x-1,bm->y-1,
				0,0,gg.loadingbackground->size.x-1,gg.loadingbackground->size.y-1);
			bitmap32free(bm);
			fileclose(); */
			bm=gfxread32(str);
			clipscaleblit32(bm,gg.loadingbackground);
			bitmap32free(bm);
		}
		sprintf(str,"%s.mxs",gp.gt.ti.trackname);
//		if (fileexist(str))
//			gg.thetrack=loadscene(str);
//		else
//			gg.thetrack=alloctree(0,0);
		gg.thetrack=new tree2(str);
	}
//	linkchildtoparent(gg.thetrack,gg.roottree);
	gg.roottree->linkchild(gg.thetrack);
popdir();
}

void startgame()
{
	gg.gamestart=1;
	gg.timeout=-1;
}

void carenagameinit()
{
	setjrmcar();
pushandsetdir("engine7testdata");
	int i,j;
	char str[1000],str2[50];
	logger("---------------------- carenamaingameinit -------------------------------\n");
/*	if (video_maindriver==VIDEO_MAINDRIVER_D3D) {
		uselights=1;
		usescnlights=0;
		dodefaultlights=1;
	} else {
		uselights=0;
		usescnlights=0;
	} */
// roottree
//	gg.roottree=alloctree(1000,NULL);
	gg.roottree=new tree2("roottree");
// load track
	loadtrack();
// load scrline
	loadscrline();
	loadrematch();
// load cars
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
/*
//		gos[i].carnull=alloctree(50,NULL);
		gos[i].carnull=new tree2("curnull");
		gos[i].carnull->buildo2p=O2P_FROMTRANSQUATSCALE;
		gos[i].carbody=loadnewjrms2(gos[i].av.ci.carname,gos[i].av.ci.paint,gos[i].av.ci.decal);
//		linkchildtoparent(gos[i].carbody,gos[i].carnull);
		gos[i].carnull->linkchild(gos[i].carbody);
		hideregpoints(gos[i].carnull);
//		linkchildtoparent(gos[i].carnull,gg.roottree);
		gg.roottree->linkchild(gos[i].carnull);
		gos[i].pos.x=(float(i%8))*.75f;
		gos[i].pos.z=(float(i/8))*.75f;
		gos[i].rot=pointf3x(0,0,0,1); 
*/
		gos[i].cc=new n_carclass(gos[i].av.ci.carname);
		gos[i].carnull=gos[i].cc->getcartree();
		gos[i].carnull->buildo2p=O2P_FROMTRANSQUATSCALE;
		gg.roottree->linkchild(gos[i].carnull);//linkchildtoparent(acarnull,);
		gos[i].pos.x=(float(i%8))*5; // 5 meters apart left/right
		gos[i].pos.z=(float(i/8))*8; // 8 meters apart back/front
//		gos[i].rot=pointf3x(0,0,0,1);
	}
// setup viewport s
	game_viewport.backcolor=C32BLUE;
	game_viewport.camtrans=gcfg.campos;
	game_viewport.camrot=gcfg.camrot;
	game_viewport.camrot.w=1;
//	game_viewport.camtrans.z=-5;
	game_viewport.camzoom=gcfg.camzoom; //3.2f;
	game_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	game_viewport.roottree=gg.roottree;
	game_viewport.xstart=0;
	game_viewport.ystart=0;
	game_viewport.xres=WX;
	game_viewport.yres=WY;
	game_viewport.zfront=.1f;
	game_viewport.zback=2000;
//	setviewportsrc(&game_viewport); // user calls this aspect 4/3
	game_viewport.xsrc=4;game_viewport.ysrc=3;
	game_viewport.useattachcam=false;
//	game_viewport.lookat=0;
	scrline_viewport.camrot.w=1;
	scrline_viewport.camzoom=1;
	scrline_viewport.flags=0;
//	scrline_viewport.roottree=gg.scrlinenull;
	scrline_viewport.xstart=0;
	scrline_viewport.ystart=0;
	scrline_viewport.xres=WX;
	scrline_viewport.yres=WY;
	scrline_viewport.zfront=.1f;
	scrline_viewport.zback=5000;
//	setviewportsrc(&scrline_viewport); // user calls this aspect 4/3
	scrline_viewport.xsrc=4;scrline_viewport.ysrc=3;
	scrline_viewport.useattachcam=false;
//	scrline_viewport.lookat=0;
// log initial state
	logger(">>> global objs <<<\n");
	logger("yourid %d, viewslot %d\n",gg.yourid,gg.viewslot);
	logger(">>> game objs (cars) <<<\n");
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
		logger("slot %2d, id %2d, name %16s, car %16s, paint %2d, decal %2d, "
			"pos (%6.3f, %6.3f, %6.3f)\n",
			i,gos[i].av.ownerid,
			gos[i].av.playername,gos[i].av.ci.carname,gos[i].av.ci.paint,gos[i].av.ci.decal,
			gos[i].pos.x,gos[i].pos.y,gos[i].pos.z);
		strcpy(str,"    \0");
		for (j=0;j<gos[i].av.ci.nweapons;j++) {
			sprintf(str2,"%s",weapinfos[(U8)(gos[i].av.ci.weapons[j])].name);
			strcat(str,str2);
			if (j!=gos[i].av.ci.nweapons-1)
				strcat(str,", ");
		}
		logger("%s\n",str);
		strcpy(str,"    \0");
		for (j=0;j<MAX_ENERGIES;j++) {
			sprintf(str2,"%s %2d",energy_names[j],gos[i].av.ci.energies[j]);
			strcat(str,str2);
			if (j!=MAX_ENERGIES-1)
				strcat(str,", ");
		}
		logger("%s\n",str);
	}
//	setloadcallback(NULL);
	bitmap32free(gg.loadingbackground);
// setup connections
	gg.con=con32_alloc(CONSIZEX,CONSIZEY,C32BLACK,C32WHITE);
	gg.milli=getmillisec();
	if (gp.gamenet==GAMENET_BOTGAME) {
		con32_printf(gg.con,"start bot game\n");
		startgame();
	} else {
		gg.numcurplayers=1; // count yourself
		for (i=0;i<gp.gt.gi.humplayers;i++)
			if (gos[i].av.ownerid==-1)
				gg.numcurplayers++;
		if (gp.gamenet==GAMENET_SERVER)
			for (i=1;i<gp.gt.gi.humplayers;i++)
				if (gg.sock[i])
					gg.retrytimers[i]=gg.milli+gcfg.afterloadtimeoutserver*1000;
				else
					gg.retrytimers[i]=-1;
		else
			gg.timeout=gg.milli+gcfg.afterloadtimeoutclient*1000;
		packetsendhere();
	}
//	gg.lastframe=getmillisec();
// setup console
	con32_printf(gg.con,"numbots %d\n",gp.gt.gi.botplayers);
	twsave0(); // save clock 0 game state for resets
	twsaven(); // save first valid state
	extradebvars(gamevars,ngamevars);
}

void gametick()
{
	int i,j;
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
// drive car
// get input
//		int u=0,d=0,l=0,r=0;
//		gg.ku=gg.kd=gg.kl=gg.kr=0;
		getbotkey(i); // keep seekmode save happy, for humans driving in botmode
		if (gos[i].av.ownerid!=-1)
			getkeybuff(gos[i].av.ownerid);
		if (gg.pk.wentbot) {
			getbotkey(i);
			if (!gg.predicted)
				gos[i].av.ownerid=-1;
		}
		if (gg.pk.krematchyes) {
			gos[i].rematchyes=1;
			logger("player %d: set rematchres to 1\n",i);
		}
		if (gg.pk.kresetgame) {
			logger("player %d: reset game\n",i);
			twrestore0();
		}
//		if (gos[i].av.cntl==CNTL_AI) {
//			getbotkey(i);
//		} else if (gos[i].av.ownerid!=-1) {
//		} else if (gos[i].av.cntl==CNTL_HUMAN) {
//			getkeybuff(i);
//			u=gg.ku;
//			d=gg.kd;
//			r=gg.kr;
//			l=gg.kl;
//		} 
// move car
		n_physics::docarphysics(&gos[i]);

	}
// collide cars
#if 1
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
		pointf3 *p=&gos[i].pos;
		for (j=i+1;j<gp.gt.gi.humplayers+gp.gt.gi.botplayers;j++) {
			pointf3 *q=&gos[j].pos;
			pointf3 pmq;
			float d;
			pmq.x=p->x-q->x;
//			pmq.y=p->y-q->y;
			pmq.z=p->z-q->z;
			d=pmq.x*pmq.x+pmq.z*pmq.z;
			if (d<gcfg.car2cardist*gcfg.car2cardist) {
				pointf3 a;
				a.x=(p->x+q->x)*.5f;
				a.y=(p->y+q->y)*.5f;
				a.z=(p->z+q->z)*.5f;
				float k;
				k=gcfg.car2cardist/(float)sqrt(d);
				p->x=k*p->x+(1-k)*a.x;
				p->y=k*p->y+(1-k)*a.y;
				p->z=k*p->z+(1-k)*a.z;
				q->x=k*q->x+(1-k)*a.x;
				q->y=k*q->y+(1-k)*a.y;
				q->z=k*q->z+(1-k)*a.z; 
			}
		}
	}
#endif
	gg.clock++;
// end 1 tick
}

static void timewarp(int ul)
{
	int j;
	int clockstart=gg.clock;
	int firstinvalid=firstinvalidkeybuff();
//	if (ul)
//		con16_printf(gg.con,"TW st %d, trg %d, inv %d, sv %d\n",clockstart,clockstart+ul,firstinvalid,twggs.clock);
	if (gg.newdata && twggs.clock<clockstart) { // new data arrived
//	if (twggs.clock<firstinvalid && twggs.clock<clockstart) { // new data arrived
//		con16_printf(gg.con,"rewind %d to %d\n",clockstart,twggs.clock);
		twrestoren(); // rewind back to valid state
		ul+=clockstart-twggs.clock;
	}
	for (j=0;j<ul;j++) { // 'uloop' game ticks done here
// one game tick (1/60) perhaps
		gg.intimewarp=gg.clock<clockstart;
		gg.predicted= (gg.clock>=firstinvalid);
		gametick(); // adds 1 to gg.clock
		if (gg.clock==firstinvalid&& gg.clock!=twggs.clock) {
			twsaven();
//			con16_printf(gg.con,"save %d\n",gg.clock);
			remkeybuff(gg.clock-1);
		}
		if (gg.clock<firstinvalid) {
			twsaven();
//			con16_printf(gg.con,"save2 %d\n",gg.clock);
			remkeybuff(gg.clock-1);
		}
	}
}

static S32 ping;
void carenagameproc()
{
	if (KEY==K_ESCAPE) {
		poporchangestate(STATE_NEWCARENALOBBY);
	}
//	int uloop,ping;
	int uloop;
	int i,j;
	gg.lastmilli=gg.milli;
	gg.milli=getmillisec();
	gg.diffmilli=gg.milli-gg.lastmilli;
// change resolution
/*	if ((KEY=='-' || KEY=='=') && video_maindriver==VIDEO_GDI && !wininfo.releasemode) {
		int dir;
		if (KEY=='-')
			dir=-1;
		else
			dir=1;
		if (changexyres(globalxres,globalyres,dir,&globalxres,&globalyres)) {
			video_setupwindow(globalxres,globalyres,DESIREDBPP);
			game_viewport.xres=WX;
			game_viewport.yres=WY;
			scrline_viewport.xres=WX;
			scrline_viewport.yres=WY;
		}
	} */
// view other players
	if (!wininfo.releasemode) {
		switch(KEY) {
//		if (KEY=='a')
//			useattachcam^=1;
		case ']':
			gg.viewslot++;
			if (gg.viewslot==gp.gt.gi.humplayers+gp.gt.gi.botplayers)
				gg.viewslot=0;
			break;
		case '[':
			gg.viewslot--;
			if (gg.viewslot<0)
				gg.viewslot=gp.gt.gi.humplayers+gp.gt.gi.botplayers-1;
			break;
		case 'g': // reset game
			gg.yk.kresetgame=1;
//		else
//			gg.kresetgame=0;
//			gg.pingclock=0;
//			twrestore0();
			break;

// change driving mode for cars you own
		case 'm':
			gg.yourcntl++;
			if (gg.yourcntl==MAX_CNTL)
				gg.yourcntl=CNTL_NONE;
			break;
// change game speed ( freeze, realtime, faster)
		case '.':
			gcfg.defuloop++;
			break;
		case ',':
			gcfg.defuloop--;
			if (gcfg.defuloop<0)
				gcfg.defuloop=-1;
			break;
		case 'n':
			gamenews=!gamenews;
			break;
		}
	}
// read process packets check timeouts
	ping=gg.clock-gg.pingclock;
	packetreadbuff();
// set uloop depending on realtime or if waiting for other players
	if (gg.gamestart) { // everyone ready to play?
		if (gcfg.defuloop<0) // freeze
			uloop=0;
		else if (gcfg.defuloop==0) { // realtime
			int div=(3000/TICKRATE); // 1/60 of a second is 50 third millisecs
			if (gp.gamenet!=GAMENET_BOTGAME && ping<0) // slight 2 % speedup if seeing neg pings..
				div+=ping/5-1;
			if (div<5)
				div=5;
//				div--;
			gg.thirdmilli+=3*gg.diffmilli;
			uloop=gg.thirdmilli/div; 
			gg.thirdmilli%=div;
		} else
			uloop=gcfg.defuloop; // manual override
	} else {
		uloop=0;
	}
//	logger("uloop = %d\n",uloop); // seems kind of choppy
// get human keyboard input
// check up on rematch/exit
	procrematch(); // sets gg.krematchyes
	if (gg.yourcntl==CNTL_HUMAN) {
		gg.yk.ku=wininfo.keystate[K_UP]||wininfo.keystate[K_NUMUP];
		gg.yk.kd=wininfo.keystate[K_DOWN]||wininfo.keystate[K_NUMDOWN];
		gg.yk.kr=wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT];
		gg.yk.kl=wininfo.keystate[K_LEFT]||wininfo.keystate[K_NUMLEFT];
	} else if (gg.yourcntl==CNTL_AI) { 
		getbotkey(gg.yourid);
		gg.pk.krematchyes=gg.yk.krematchyes;
		gg.pk.kresetgame=gg.yk.kresetgame;
		gg.yk=gg.pk;
	} else 
		gg.yk.ku=gg.yk.kd=gg.yk.kl=gg.yk.kr=0;
//	if (gp.gamenet!=GAMENET_BOTGAME) {
	if (gg.yourid==0) {
		for (i=0;i<gp.gt.gi.humplayers;i++)
			if (gos[i].av.ownerid!=-1 && !gos[i].rematchyes)
				break;
		if (i==gp.gt.gi.humplayers)
			gg.yk.kresetgame=1;
	}
	for (j=0;j<uloop;j++) { // 'uloop' game ticks done here
		packetsendkey(gg.clock+j,gg.yk.ku,gg.yk.kd,gg.yk.kl,gg.yk.kr,gg.yk.kresetgame,gg.yk.krematchyes);//,gg.krematchno);
		gg.yk.kresetgame=gg.yk.krematchyes=0;//gg.krematchno=0;
	}
	packetwritebuff();
//	}
// TIME WARP 3 clocks to look at, 
	timewarp(uloop); // run game this many ticks..
// update car
	for (j=0;j<gp.gt.gi.humplayers+gp.gt.gi.botplayers;j++) {
		gos[j].carnull->trans=gos[j].pos;
		gos[j].carnull->rot=gos[j].rot;
	}
// update scrline
	updatescrline();
// update camera
	game_viewport.camattach=gos[gg.viewslot].carnull;//getlastcam();
	game_viewport.useattachcam=true;
	if (gos[gg.viewslot].carnull)
		gos[gg.viewslot].carnull->zoom=game_viewport.camzoom;
// update 3d animation
	doflycam(&game_viewport);
//	doanims(game_viewport.roottree);
	gg.roottree->proc();
//	buildtreematrices(game_viewport.roottree);
	video_buildworldmats(gg.roottree);
//	buildtreematrices(scrline_viewport.roottree);
	video_buildworldmats(gg.scrlinenull);
}

void carenagamedraw3d()
{
// update video
// draw scene
//	video_beginscene(&game_viewport);
	video_setviewport(&game_viewport);
//	video_drawscene(&game_viewport);
	video_drawscene(gg.roottree);
//	video_drawscene(&scrline_viewport);
	video_setviewport(&scrline_viewport);
	video_drawscene(gg.scrlinenull);
//	video_endscene(&game_viewport);
}

void carenagamedraw2d()
{
//	video_lock();
//	outtextxyf16(B16,0,0,hiwhite,"%f %f",a,gos[0].rot.y);
// draw console
	if (gamenews)
		clipblit32(con32_getbitmap32(gg.con),B32,0,0,CONLOCX,WY-CONLOCWYMY-CONSIZEY,CONSIZEX,CONSIZEY);
	S32 j=gg.clock%120;
	cliprect32(B32,4*j,0,8,8,C32WHITE);
// draw keybuff
	drawkeybuff();
// draw rematch
	drawrematch();
// draw ping time
	if (gp.gamenet!=GAMENET_BOTGAME && !wininfo.releasemode)
		outtextxybf32(B32,256,20,C32WHITE,C32BLACK,"ping %d",ping*1000/60);
//	video_unlock();
}

void carenagameexit()
{
	logger("---------------------- carenamaingameexit -------------------------------\n");
	logger("game viewport\n");
//	logviewport(&game_viewport,OPT_SOME);
	gg.roottree->log2();
	logger("scrline viewport\n");
//	logviewport(&scrline_viewport,OPT_SOME);
	gg.scrlinenull->log2();
	extradebvars(0,0);
	int i;
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
		delete gos[i].cc;
		gos[i].cc=0;
	}
	freescrline();
//	freetree(gg.roottree);
	delete gg.roottree;
	cleansocks();
	resetkeybuff();
	con32_free(gg.con);
	twobjtest();
	freerematch();
	if (gp.gt.newgametrack)
		freenewtrackscript(gp.gt.newgametrack);
	gp.gt.newgametrack=0;
	st2_freecollgrids();
popdir();
}
