#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "timewarp.h"
#include "carclass.h"
#include "soundemit.h"
#include "avaj.h"
#include "physics.h"
#include "netdescj.h"
#include "constructor.h"
#include "envj.h"
#include "gamerunj.h"
#include "gamedescj.h"
#include "netobj.h"
#include "carenagame.h"
#include "scrline.h"
#include "rematch.h"
#include "ai.h"
#include "system/u_states.h"
#include "line2road.h"
#include "gamestatecheck.h"
#include "utils/modelutil.h"
#include "d2_font.h"
#include "rabbits.h"
#include "camera.h"
#include "helper.h"
#include "soundfx.h"

#include "match_logon.h"

using namespace n_physics;

// game vars
gamerunj* gg; // the game
netobj* no; // the collective...
tree2* gamecam;
camatt* ca;
helper* helpers;
wavehandle* backsndwavegame;
soundfx* sfx;
soundemitman* sem;
S32 testsnd;

// 3d viewports
static struct viewport2 game_viewport,scrline_viewport;
static tree2* roottree;
//static tree2* testsphere1,*testsphere2; // don't free.
static textureb* gamewhitetex;
// location of console
#define CONSIZEX 80*8
#define CONSIZEY 12*8
#define CONLOCX 0
#define CONLOCY 8
static bool gamenews=false;
static S32 ping; // units: 1/60 sec
static S32 uloop=0;
static S32 fpssave;
static const S32 GAMESPTRESX = 800;
static const S32 GAMESPTRESY = 600;
// game camera
static struct menuvar gamevars[]={
	{"@lightred@---- game cam -------------------",0,D_VOID,0},
	{"game uloop",&uloop,D_INT|D_RDONLY},
	{"game cam trans x",&game_viewport.camtrans.x,D_FLOAT,FLOATUP/8},
	{"game cam trans y",&game_viewport.camtrans.y,D_FLOAT,FLOATUP/8},
	{"game cam trans z",&game_viewport.camtrans.z,D_FLOAT,FLOATUP/8},
	{"game cam rot x",&game_viewport.camrot.x,D_FLOAT,FLOATUP/8},
	{"game cam rot y",&game_viewport.camrot.y,D_FLOAT,FLOATUP/8},
	{"game cam rot z",&game_viewport.camrot.z,D_FLOAT,FLOATUP/8},
	{"game cam attachcam",&game_viewport.useattachcam,D_INT,1},
	{"game cam zoom",&game_viewport.camzoom,D_FLOAT,FLOATUP/8},
#ifdef FULLPHYSICS
	{"@lightgreen@---- game attach camera -------------------",0,D_VOID,0},
	{"game camatt trans x",&acv2.camatt.x,D_FLOAT,FLOATUP/8},
	{"game camatt trans y",&acv2.camatt.y,D_FLOAT,FLOATUP/8},
	{"game camatt trans z",&acv2.camatt.z,D_FLOAT,FLOATUP/8},
	{"game camatt pitch",&acv2.camattpitch,D_FLOAT,FLOATUP/8},
	{"game camatt dist",&acv2.camattdist,D_FLOAT,FLOATUP/8},
	{"@yellow@---- game physics -------------------",0,D_VOID,0},
	{"extrashocklen[0]",&acv2.extrashocklen[0],D_FLOAT,FLOATUP/8},
	{"extrashocklen[1]",&acv2.extrashocklen[1],D_FLOAT,FLOATUP/8},
	{"extrashocklen[2]",&acv2.extrashocklen[2],D_FLOAT,FLOATUP/8},
	{"extrashocklen[3]",&acv2.extrashocklen[3],D_FLOAT,FLOATUP/8},
	{"normalshocklen[0]",&acv2.normalshocklen[0],D_FLOAT,FLOATUP/8},
	{"normalshocklen[1]",&acv2.normalshocklen[1],D_FLOAT,FLOATUP/8},
	{"normalshocklen[2]",&acv2.normalshocklen[2],D_FLOAT,FLOATUP/8},
	{"normalshocklen[3]",&acv2.normalshocklen[3],D_FLOAT,FLOATUP/8},
	{"shockextra",&acv2.shockextra,D_FLOAT,FLOATUP/8},
	{"p[0].x",&acv2.p[0].x,D_FLOAT,FLOATUP/8},
	{"p[0].y",&acv2.p[0].y,D_FLOAT,FLOATUP/8},
	{"p[0].z",&acv2.p[0].z,D_FLOAT,FLOATUP/8},
	{"p[1].x",&acv2.p[1].x,D_FLOAT,FLOATUP/8},
	{"p[1].y",&acv2.p[1].y,D_FLOAT,FLOATUP/8},
	{"p[1].z",&acv2.p[1].z,D_FLOAT,FLOATUP/8},
	{"p[2].x",&acv2.p[2].x,D_FLOAT,FLOATUP/8},
	{"p[2].y",&acv2.p[2].y,D_FLOAT,FLOATUP/8},
	{"p[2].z",&acv2.p[2].z,D_FLOAT,FLOATUP/8},
	{"p[3].x",&acv2.p[3].x,D_FLOAT,FLOATUP/8},
	{"p[3].y",&acv2.p[3].y,D_FLOAT,FLOATUP/8},
	{"p[3].z",&acv2.p[3].z,D_FLOAT,FLOATUP/8},
	{"p[4].x",&acv2.p[4].x,D_FLOAT,FLOATUP/8},
	{"p[4].y",&acv2.p[4].y,D_FLOAT,FLOATUP/8},
	{"p[4].z",&acv2.p[4].z,D_FLOAT,FLOATUP/8},
	{"p[5].x",&acv2.p[5].x,D_FLOAT,FLOATUP/8},
	{"p[5].y",&acv2.p[5].y,D_FLOAT,FLOATUP/8},
	{"p[5].z",&acv2.p[5].z,D_FLOAT,FLOATUP/8},
	{"p[6].x",&acv2.p[6].x,D_FLOAT,FLOATUP/8},
	{"p[6].y",&acv2.p[6].y,D_FLOAT,FLOATUP/8},
	{"p[6].z",&acv2.p[6].z,D_FLOAT,FLOATUP/8},
	{"p[7].x",&acv2.p[7].x,D_FLOAT,FLOATUP/8},
	{"p[7].y",&acv2.p[7].y,D_FLOAT,FLOATUP/8},
	{"p[7].z",&acv2.p[7].z,D_FLOAT,FLOATUP/8},
	{"p[8].x",&acv2.p[8].x,D_FLOAT,FLOATUP/8},
	{"p[8].y",&acv2.p[8].y,D_FLOAT,FLOATUP/8},
	{"p[8].z",&acv2.p[8].z,D_FLOAT,FLOATUP/8},
	{"p[9].x",&acv2.p[9].x,D_FLOAT,FLOATUP/8},
	{"p[9].y",&acv2.p[9].y,D_FLOAT,FLOATUP/8},
	{"p[9].z",&acv2.p[9].z,D_FLOAT,FLOATUP/8},
	{"p[10].x",&acv2.p[10].x,D_FLOAT,FLOATUP/8},
	{"p[10].y",&acv2.p[10].y,D_FLOAT,FLOATUP/8},
	{"p[10].z",&acv2.p[10].z,D_FLOAT,FLOATUP/8},
	{"p[11].x",&acv2.p[11].x,D_FLOAT,FLOATUP/8},
	{"p[11].y",&acv2.p[11].y,D_FLOAT,FLOATUP/8},
	{"p[11].z",&acv2.p[11].z,D_FLOAT,FLOATUP/8},
	{"p[12].x",&acv2.p[12].x,D_FLOAT,FLOATUP/8},
	{"p[12].y",&acv2.p[12].y,D_FLOAT,FLOATUP/8},
	{"p[12].z",&acv2.p[12].z,D_FLOAT,FLOATUP/8},
	{"p[13].x",&acv2.p[13].x,D_FLOAT,FLOATUP/8},
	{"p[13].y",&acv2.p[13].y,D_FLOAT,FLOATUP/8},
	{"p[13].z",&acv2.p[13].z,D_FLOAT,FLOATUP/8},
	{"p[14].x",&acv2.p[14].x,D_FLOAT,FLOATUP/8},
	{"p[14].y",&acv2.p[14].y,D_FLOAT,FLOATUP/8},
	{"p[14].z",&acv2.p[14].z,D_FLOAT,FLOATUP/8},
	{"p[15].x",&acv2.p[15].x,D_FLOAT,FLOATUP/8},
	{"p[15].y",&acv2.p[15].y,D_FLOAT,FLOATUP/8},
	{"p[15].z",&acv2.p[15].z,D_FLOAT,FLOATUP/8},
	{"p[16].x",&acv2.p[16].x,D_FLOAT,FLOATUP/8},
	{"p[16].y",&acv2.p[16].y,D_FLOAT,FLOATUP/8},
	{"p[16].z",&acv2.p[16].z,D_FLOAT,FLOATUP/8},
	{"p[17].x",&acv2.p[17].x,D_FLOAT,FLOATUP/8},
	{"p[17].y",&acv2.p[17].y,D_FLOAT,FLOATUP/8},
	{"p[17].z",&acv2.p[17].z,D_FLOAT,FLOATUP/8},
/*	{"pr[0].x",&acv2.pr[0].x,D_FLOAT,FLOATUP/8},
	{"pr[0].y",&acv2.pr[0].y,D_FLOAT,FLOATUP/8},
	{"pr[0].z",&acv2.pr[0].z,D_FLOAT,FLOATUP/8},
	{"pr[1].x",&acv2.pr[1].x,D_FLOAT,FLOATUP/8},
	{"pr[1].y",&acv2.pr[1].y,D_FLOAT,FLOATUP/8},
	{"pr[1].z",&acv2.pr[1].z,D_FLOAT,FLOATUP/8},
	{"pr[2].x",&acv2.pr[2].x,D_FLOAT,FLOATUP/8},
	{"pr[2].y",&acv2.pr[2].y,D_FLOAT,FLOATUP/8},
	{"pr[2].z",&acv2.pr[2].z,D_FLOAT,FLOATUP/8},
	{"pr[3].x",&acv2.pr[3].x,D_FLOAT,FLOATUP/8},
	{"pr[3].y",&acv2.pr[3].y,D_FLOAT,FLOATUP/8},
	{"pr[3].z",&acv2.pr[3].z,D_FLOAT,FLOATUP/8},
	{"pr[4].x",&acv2.pr[4].x,D_FLOAT,FLOATUP/8},
	{"pr[4].y",&acv2.pr[4].y,D_FLOAT,FLOATUP/8},
	{"pr[4].z",&acv2.pr[4].z,D_FLOAT,FLOATUP/8},
	{"pr[5].x",&acv2.pr[5].x,D_FLOAT,FLOATUP/8},
	{"pr[5].y",&acv2.pr[5].y,D_FLOAT,FLOATUP/8},
	{"pr[5].z",&acv2.pr[5].z,D_FLOAT,FLOATUP/8},
	{"pr[6].x",&acv2.pr[6].x,D_FLOAT,FLOATUP/8},
	{"pr[6].y",&acv2.pr[6].y,D_FLOAT,FLOATUP/8},
	{"pr[6].z",&acv2.pr[6].z,D_FLOAT,FLOATUP/8},
	{"pr[7].x",&acv2.pr[7].x,D_FLOAT,FLOATUP/8},
	{"pr[7].y",&acv2.pr[7].y,D_FLOAT,FLOATUP/8},
	{"pr[7].z",&acv2.pr[7].z,D_FLOAT,FLOATUP/8},
	{"pr[8].x",&acv2.pr[8].x,D_FLOAT,FLOATUP/8},
	{"pr[8].y",&acv2.pr[8].y,D_FLOAT,FLOATUP/8},
	{"pr[8].z",&acv2.pr[8].z,D_FLOAT,FLOATUP/8},
	{"pr[9].x",&acv2.pr[9].x,D_FLOAT,FLOATUP/8},
	{"pr[9].y",&acv2.pr[9].y,D_FLOAT,FLOATUP/8},
	{"pr[9].z",&acv2.pr[9].z,D_FLOAT,FLOATUP/8},
	{"pr[10].x",&acv2.pr[10].x,D_FLOAT,FLOATUP/8},
	{"pr[10].y",&acv2.pr[10].y,D_FLOAT,FLOATUP/8},
	{"pr[10].z",&acv2.pr[10].z,D_FLOAT,FLOATUP/8},
	{"pr[11].x",&acv2.pr[11].x,D_FLOAT,FLOATUP/8},
	{"pr[11].y",&acv2.pr[11].y,D_FLOAT,FLOATUP/8},
	{"pr[11].z",&acv2.pr[11].z,D_FLOAT,FLOATUP/8},
	{"pr[12].x",&acv2.pr[12].x,D_FLOAT,FLOATUP/8},
	{"pr[12].y",&acv2.pr[12].y,D_FLOAT,FLOATUP/8},
	{"pr[12].z",&acv2.pr[12].z,D_FLOAT,FLOATUP/8},
	{"pr[13].x",&acv2.pr[13].x,D_FLOAT,FLOATUP/8},
	{"pr[13].y",&acv2.pr[13].y,D_FLOAT,FLOATUP/8},
	{"pr[13].z",&acv2.pr[13].z,D_FLOAT,FLOATUP/8},
	{"pr[14].x",&acv2.pr[14].x,D_FLOAT,FLOATUP/8},
	{"pr[14].y",&acv2.pr[14].y,D_FLOAT,FLOATUP/8},
	{"pr[14].z",&acv2.pr[14].z,D_FLOAT,FLOATUP/8},
	{"pr[15].x",&acv2.pr[15].x,D_FLOAT,FLOATUP/8},
	{"pr[15].y",&acv2.pr[15].y,D_FLOAT,FLOATUP/8},
	{"pr[15].z",&acv2.pr[15].z,D_FLOAT,FLOATUP/8},
	{"pr[16].x",&acv2.pr[16].x,D_FLOAT,FLOATUP/8},
	{"pr[16].y",&acv2.pr[16].y,D_FLOAT,FLOATUP/8},
	{"pr[16].z",&acv2.pr[16].z,D_FLOAT,FLOATUP/8},
	{"pr[17].x",&acv2.pr[17].x,D_FLOAT,FLOATUP/8},
	{"pr[17].y",&acv2.pr[17].y,D_FLOAT,FLOATUP/8},
	{"pr[17].z",&acv2.pr[17].z,D_FLOAT,FLOATUP/8}, */
	{"shockspacingx",&acv2.shockspacingx,D_FLOAT,FLOATUP/8},
	{"shockspacingz",&acv2.shockspacingz,D_FLOAT,FLOATUP/8},
	{"shockoffsety",&acv2.shockoffsety,D_FLOAT,FLOATUP/8},
	{"carbboxmin.x",&acv2.carbboxmin.x,D_FLOAT,FLOATUP/8},
	{"carbboxmin.y",&acv2.carbboxmin.y,D_FLOAT,FLOATUP/8},
	{"carbboxmin.z",&acv2.carbboxmin.z,D_FLOAT,FLOATUP/8},
	{"carbboxmax.x",&acv2.carbboxmax.x,D_FLOAT,FLOATUP/8},
	{"carbboxmax.y",&acv2.carbboxmax.y,D_FLOAT,FLOATUP/8},
	{"carbboxmax.z",&acv2.carbboxmax.z,D_FLOAT,FLOATUP/8},
/*	{"c2cpnts[0].x",&acv2.c2cpnts[0].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[0].y",&acv2.c2cpnts[0].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[0].z",&acv2.c2cpnts[0].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[1].x",&acv2.c2cpnts[1].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[1].y",&acv2.c2cpnts[1].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[1].z",&acv2.c2cpnts[1].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[2].x",&acv2.c2cpnts[2].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[2].y",&acv2.c2cpnts[2].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[2].z",&acv2.c2cpnts[2].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[3].x",&acv2.c2cpnts[3].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[3].y",&acv2.c2cpnts[3].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[3].z",&acv2.c2cpnts[3].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[4].x",&acv2.c2cpnts[4].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[4].y",&acv2.c2cpnts[4].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[4].z",&acv2.c2cpnts[4].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[5].x",&acv2.c2cpnts[5].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[5].y",&acv2.c2cpnts[5].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[5].z",&acv2.c2cpnts[5].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[6].x",&acv2.c2cpnts[6].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[6].y",&acv2.c2cpnts[6].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[6].z",&acv2.c2cpnts[6].z,D_FLOAT,FLOATUP/8},
	{"c2cpnts[7].x",&acv2.c2cpnts[7].x,D_FLOAT,FLOATUP/8},
	{"c2cpnts[7].y",&acv2.c2cpnts[7].y,D_FLOAT,FLOATUP/8},
	{"c2cpnts[7].z",&acv2.c2cpnts[7].z,D_FLOAT,FLOATUP/8}, */
/*	{"shocklen[0]",&acv2.shocklen[0],D_FLOAT,FLOATUP/8},
	{"shocklen[1]",&acv2.shocklen[1],D_FLOAT,FLOATUP/8},
	{"shocklen[2]",&acv2.shocklen[2],D_FLOAT,FLOATUP/8},
	{"shocklen[3]",&acv2.shocklen[3],D_FLOAT,FLOATUP/8}, */
/*	{"wheelvel[0]",&acv2.wheelvel[0],D_FLOAT,FLOATUP/8},
	{"wheelvel[1]",&acv2.wheelvel[1],D_FLOAT,FLOATUP/8},
	{"wheelvel[2]",&acv2.wheelvel[2],D_FLOAT,FLOATUP/8},
	{"wheelvel[3]",&acv2.wheelvel[3],D_FLOAT,FLOATUP/8}, */
	{"shockstr",&acv2.shockstr,D_FLOAT,FLOATUP/8},
	{"shockdamp",&acv2.shockdamp,D_FLOAT,FLOATUP/8},
	{"moi",&acv2.moi,D_FLOAT,FLOATUP/8},
	{"mass",&acv2.mass,D_FLOAT,FLOATUP/8},
	{"elast",&acv2.elast,D_FLOAT,FLOATUP/8},
	{"airfric",&acv2.airfric,D_FLOAT,FLOATUP/8},
	{"littleg",&acv2.littleg,D_FLOAT,FLOATUP/8},
	{"wheelrad",&acv2.wheelrad,D_FLOAT,FLOATUP/8},
	{"wheelaccel",&acv2.wheelaccel,D_FLOAT,FLOATUP/8},
//	{"accelspin",&acv2.accelspin,D_FLOAT,FLOATUP/8},
	{"fricttireslide",&acv2.fricttireslide,D_FLOAT,FLOATUP/8},
	{"fricttireroll",&acv2.fricttireroll,D_FLOAT,FLOATUP/8},
	{"frictcarbody",&acv2.frictcarbody,D_FLOAT,FLOATUP/8},
	{"drivemode",&acv2.drivemode,D_FLOAT,FLOATUP/8},
	{"startaccelspin",&acv2.startaccelspin,D_FLOAT,FLOATUP/8},
	{"c2celast",&acv2.c2celast,D_FLOAT,FLOATUP/8},
//	{"carid",&acv2.carid,D_FLOAT,FLOATUP/8},
	{"defuextraheading",&acv2.defuextraheading,D_FLOAT,FLOATUP/8},
	{"maxuextraheading",&acv2.maxuextraheading,D_FLOAT,FLOATUP/8},
	{"maxturnspeed",&acv2.maxturnspeed,D_FLOAT,FLOATUP/8},
	{"testimpval.x",&acv2.testimpval.x,D_FLOAT,FLOATUP/8},
	{"testimpval.y",&acv2.testimpval.y,D_FLOAT,FLOATUP/8},
	{"testimpval.z",&acv2.testimpval.z,D_FLOAT,FLOATUP/8},
	{"testimppnt.x",&acv2.testimppnt.x,D_FLOAT,FLOATUP/8},
	{"testimppnt.y",&acv2.testimppnt.y,D_FLOAT,FLOATUP/8},
	{"testimppnt.z",&acv2.testimppnt.z,D_FLOAT,FLOATUP/8},
	{"maxnoshocktime",&acv2.maxnoshocktime,D_INT,1},
	{"maxnocheckpointtime",&acv2.maxnocheckpointtime,D_INT,1},
	{"maxnoresettime",&acv2.maxnoresettime,D_INT,1},
	{"carflipheight",&acv2.carflipheight,D_FLOAT,FLOATUP/8},
	{"carstartheight",&acv2.carstartheight,D_FLOAT,FLOATUP/8},
// new ones
	{"car2cardist",&acv2.car2cardist,D_FLOAT,FLOATUP/8},
	{"candrivetime",&acv2.candrivetime,D_INT,1},
	{"rematchtime",&acv2.rematchtime,D_INT,1},
	{"norematchtime",&acv2.norematchtime,D_INT,1},
	{"sndc2cmag",&acv2.sndc2cmag,D_FLOAT,FLOATUP/8},
	{"speedofsound",&acv2.speedofsound,D_FLOAT,FLOATUP/8},
#endif
};
static const S32 ngamevars=sizeof(gamevars)/sizeof(gamevars[0]);
//////////////////////////////////////// load and play game ///////////////////////////////////////////////////////

static void loadtrack()
{
// track now already loaded, just get tree
	gg->e.buildtrackhashtree();
	wave_unload(backsndwavegame);
	backsndwavegame = gg->e.oldtrackj->buildbacksnd();
	tree2* thetrack=gg->e.oldtrackj->root;
	roottree->linkchild(thetrack);
	gg->e.oldtrackj->studyforks();
}

static void setupviewports()
{
// setup main viewport
	game_viewport.backcolor=C32BLUE;
	game_viewport.camtrans=gcfg.campos;
	game_viewport.camrot=gcfg.camrot;
	game_viewport.camrot.w=1;
	game_viewport.camzoom=gcfg.camzoom; //3.2f;
	game_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	game_viewport.xstart=0;
	game_viewport.ystart=0;
	game_viewport.xres=WX;
	game_viewport.yres=WY;
	game_viewport.zfront=.25f;
	game_viewport.zback=2000;
	game_viewport.xsrc=4;game_viewport.ysrc=3;
	game_viewport.useattachcam=true;
	gamecam = new tree2("gamecam");
	roottree->linkchild(gamecam);
	game_viewport.camattach = gamecam;
	ca = new camatt(gamecam);
	helpers = new helper(roottree);
//	game_viewport.lookat=0;
// setup scrline/hud viewport
	scrline_viewport.camrot.w=1;
	scrline_viewport.camzoom=1;
	scrline_viewport.flags=0;
	scrline_viewport.xstart=0;
	scrline_viewport.ystart=0;
	scrline_viewport.xres=WX;
	scrline_viewport.yres=WY;
	scrline_viewport.zfront=.1f;
	scrline_viewport.zback=5000;
	scrline_viewport.xsrc=4;scrline_viewport.ysrc=3;
	scrline_viewport.useattachcam=false;
//	scrline_viewport.lookat=0;
}

// initialize players near start/finish pieces
static void setstartpos()
{
#define NEWSTART
#ifdef NEWSTART
	const float across = 5.0f;
	const float down = 6.0f;
	const S32 numacross = 4;
//	const S32 numperpce = 20; // 20 per piece
// piece_rot to car_rot
	const pointf3 rottab[4]={ // for car rotation, 4 quats
		{0,.7071f,0,.7071f},	//  90 right, might be dicey using float literals like this, look at the literature (consistancy)
		{0,1,0,0},				// 180 back
		{0,-.7071f,0,.7071f},	// 270 left , maybe sqrtf(2.0f)*.5f might be better...
		{0,0,0,1},				//   0 front
	};
	const trkt* ot = gg->e.oldtrackj;
	if (!ot)
		errorexit("no old track to set start pos with");
// find s/f pieces, build pos rot list
	S32 i,j;
	vector<pointf3x> sts;
	vector<pointi2> stpcelocs;
	vector<pointf3x> rotcars;
	pointf3x st(PIECESIZE*.5f,0,PIECESIZE*.5f);
	pointf3x rotcar;
	for (j=0;j<TRACKZ;++j) {
		for (i=0;i<TRACKX;++i) {
			if (ot->pieces[j][i].pt==PCE_STARTFINISH) {
				st.x = PIECESIZE*(i*1.0f+.5f); // middle of piece
				st.z = PIECESIZE*(j*1.0f+.5f);
				pointi2 stpce;
				stpce.x = i;
				stpce.y = j;
				stpcelocs.push_back(stpce);
				rotcar = rottab[ot->pieces[j][i].rot];
				sts.push_back(st);
				rotcars.push_back(rotcar);
			}
		}
	}
// place players
	if (sts.empty()) {
		sts.push_back(st);
		rotcars.push_back(rotcar);
		stpcelocs.push_back(pointi2x(0,0));
	}
	S32 k=0;
	S32 totnump = gg->numplayers;
	S32 totnumsf = sts.size();
// ADD whole + num/den   and    wholestep + numstep/den
	S32 whole = 0;
	S32 num = 0;
	S32 den = totnumsf;
	S32 wholestep = totnump/totnumsf;
	S32 numstep = totnump%totnumsf;
	S32 pi;
	for (pi=0;pi<totnumsf;++pi) {
// calc next whole piece
		num += numstep;
		S32 carry = num/den;
		num %=den;
		S32 newwhole = whole + wholestep + carry;
// calc numcars for this one piece
		S32 np = newwhole - whole;
// init positions of cars on one startfinish piece
		j=0;
		while(np>0) {
			S32 npa=min(np,numacross);
			for (i=0;i<npa;++i) {
//				float floorheight = 0;
				pointf3x off(i*across-(npa-1)*across*.5f,0,-((j+.5f)*down));
				quatrot(&rotcars[pi],&off,&off);
				gg->caros[k].cs.pos.x=sts[pi].x+off.x;
				gg->caros[k].cs.pos.y=sts[pi].y+off.y;
				gg->caros[k].cs.pos.z=sts[pi].z+off.z;
//				gg->caros[k].cs.lastpos = gg->caros[k].cs.pos;
#ifdef FULLPHYSICS
				ot->line2roadvert(&gg->caros[k].cs.pos);
				gg->caros[k].cs.pos.y+=acv2.carstartheight;
#endif
				gg->caros[k].cs.rot=rotcars[pi];
				gg->caros[k].stpceloc = stpcelocs[pi];
				++k;
			}
			np-=npa;
			++j;
		}
		whole = newwhole;
	}
#else
	S32 k;
	for (k=0;(S32)k<gg->numplayers;++k) {
		gg->caros[k].cs.pos.x=(float(k%8))*5; // 5 meters apart across
		gg->caros[k].cs.pos.z=(float(k/8))*8; // 8 meters apart back/front
	}
#endif
}

socker* master,*client;
struct contdata {
	U8 buts[2];
	S16 tilt;
};
contdata ctdata;

void carenagameinit()
{
	initsocker();
	master = 0;
	client = 0;
	memset(&ctdata,0,sizeof(ctdata));

#ifdef CHECKGAMESTATE
	gs = new gamestate;
#endif
	fpssave = wininfo.fpswanted;
	ping = 0;
//	setjrmcar();
pushandsetdir("newcarenadata");
// physics
#ifdef FULLPHYSICS
	physics_init();
#endif
	U32 i,j;
	char str[1000];
	logger("---------------------- carenamaingameinit -------------------------------\n");
	lightinfo.uselights=1;
	lightinfo.dodefaultlights=1;
// roottree
	roottree=new tree2("roottree");
	setupviewports();
// load track
	loadtrack();
// sfx
	sfx = new soundfx();
	sem = new soundemitman();
	testsnd = 0;
// load scrline
	init_res3d((float)GAMESPTRESX,(float)GAMESPTRESY);
	initscrline();
	loadrematch();
	gg->con->setvis(gamenews);
// load and place cars
	for (i=0;(S32)i<gg->numplayers;++i) {
		gg->caros[i].cc=new n_carclass(gg->caros[i].carbodyname.c_str(),gg->caros[i].paint,gg->caros[i].decal);
		gg->caros[i].carnull=gg->caros[i].cc->getcartree();
		gg->caros[i].carnull->buildo2p=O2P_FROMTRANSQUATSCALE;
		roottree->linkchild(gg->caros[i].carnull);//linkchildtoparent(acarnull,);
pushandsetdir("gfxtest");
		gg->caros[i].carbodyprism = buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
		gg->caros[i].carbodyprism->treecolor = pointf3x(1,1,1,.5f);
		gg->caros[i].carnull->linkchild(gg->caros[i].carbodyprism);
//		gg->caros[i].carbodyprism->flags|=TF_DONTDRAWC;
		for (j=0;j<NWORKINGSHOCKS;++j) {
// shocks
			gg->caros[i].carshocks[j] = buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
			gg->caros[i].carshocks[j]->treecolor = pointf3x(1,1,1,.5f);
			gg->caros[i].carnull->linkchild(gg->caros[i].carshocks[j]);
// wheels
			pointf3x sz(1,1,1);
			gg->caros[i].carwheels[j] = buildcylinder_xz(sz,"maptestnck.tga","tex");
//			gg->caros[i].cc->getwheeltree(j);
			gg->caros[i].carwheels[j]->treecolor=pointf3x(.5f,.5f,.5f,1);
			if (j&1) // outside of wheels are pointing down
				gg->caros[i].carwheels[j]->rot = pointf3x(0,0,PI/2);
			else
				gg->caros[i].carwheels[j]->rot = pointf3x(0,0,-PI/2);
//			gg->caros[i].carwheels[j]->trans.x+=.1f;
//			gg->caros[i].carwheels[j]->rotvel = pointf3x(TWOPI/30.0f,0,0);
			gg->caros[i].carnull->linkchild(gg->caros[i].carwheels[j]);
//			gg->caros[i].carwheels[j]->flags|=TF_DONTDRAWC;
		}
popdir();
// motor sounds
		gg->caros[i].emit = new soundemit(*sem,motor(i%sem->getnumwaves()));
//		gg->caros[i].emit = new soundemit(*sem,ENG1);
//		gg->caros[i].emit = new soundemit(*sem,ENG2);
	}
// setup connections
//	gg->con=con32_alloc(CONSIZEX,CONSIZEY,C32BLACK,C32WHITE);
	switch(gg->n.gamenet) {
	case GAMENET_SERVER:
		gg->con->printf("start server game");
//		con32_printf(gg->con,"start server game\n");
		break;
	case GAMENET_CLIENT:
		gg->con->printf("start client game");
//		con32_printf(gg->con,"start client game\n");
		break;
	case GAMENET_BOTGAME:
		gg->con->printf("start bot game");
//		con32_printf(gg->con,"start bot game\n");
		break;
	}
	setstartpos(); // initialize positions to start/finish pieces
	initrabbits();
// log initial state
	logger(">>> global objs <<<\n");
	logger("yourid %d, viewslot %d\n",gg->yourid,gg->viewslot);
	logger(">>> game objs (cars) <<<\n");
	for (i=0;(S32)i<gg->numplayers;++i) {
		logger("slot %2d, id %2d, name %-16s, car %-16s, paint %2d, decal %2d, "
			"pos (%6.3f, %6.3f, %6.3f)\n",
			i,gg->caros[i].ownerid,
			gg->caros[i].playername.c_str(),gg->caros[i].carbodyname.c_str(),gg->caros[i].paint,gg->caros[i].decal,
			gg->caros[i].cs.pos.x,gg->caros[i].cs.pos.y,gg->caros[i].cs.pos.z);
		strcpy(str,"    \0");
	}
// setup timewarp
	twsave();
// setup debprint extra
	extradebvars(gamevars,ngamevars);
	avgreset(); // reset running averages after a long load
// test sphere
//	testsphere1=buildsphere(3,"maptestnck.tga","tex");
//	testsphere2=testsphere1->newdup();
//	roottree->linkchild(testsphere1);
//	roottree->linkchild(testsphere2);
	gamewhitetex = texture_create("white.pcx"); // speed up sprites, slight cleanup needed
	if (texture_getrc(gamewhitetex)==1) {
		pushandsetdir("common");
		gamewhitetex->load();
		popdir();
	}
//	playasound(backsndwavegame);
}

// get all key input given keyques, time, ownerid
vector<keys> getkeys()
{
	int i;
//	logger("------ clock %5d ------\n",gg->gs.clock);
	vector<keys> ret;
	for (i=0;i<gg->numplayers;++i) {
// get input
		keys k;
		if (no->canreadkey(i,gg->gs.clock)) { // from collective
			k=no->readkey(i,gg->gs.clock); // even if ownerid == -1
			if (k.kwentbot) {
				static S32 cnt = 0;
				if (gg->caros[i].ownerid != -1)
					gg->con->printf("%2d:player %2d, went bot at %d",cnt++,i,gg->gs.clock-gg->gs.clockoffset);
//					con32_printf(gg->con,"%2d:player %2d, went bot at %d\n",cnt++,i,gg->gs.clock-gg->gs.clockoffset);
				gg->caros[i].ownerid = -1; // does this work outside of timewarp???, yes it does...
				k=getbotkey(i);
			}
		} else if (gg->caros[i].ownerid==-1) {
			k = getbotkey(i);
		} else {
//			errorexit("predicted!");
//			k = getbotkey(i);
			k = no->readlastkey(i);
//			k = keys();
		}
		ret.push_back(k);
	}
	return ret;
}

// advance game 1 tick, includes resetgame, this happens inside timewarp
void gametick(const vector<keys>& ks) //,bool predicted,bool intimewarp)
{
	int i;
// reset game if necessary
//	logger("gametick at %5d: pred %d, intimewarp %d\n",gg->gs.clock,gg->predicted,gg->intimewarp);
	bool rg = false;
	for (i=0;i<gg->numplayers;++i) {
		if (ks[i].kresetgame) { // special 'g' reset game key
			rg = true;
		}
		twcaro* co=&gg->caros[i].cs;
		if (ks[i].krematchyes) { // all players hit yes, bots pick yes automatically, including players who left the game
			co->rematchyes = true;
		} else if (gg->caros[i].cs.curlap<=gg->e.nlaps) {
			co->rematchyes = false;
		}
/*		if (gg->caros[i].cs.curlap<=gg->e.nlaps) { // not done racing, make sure no rematchyes get through early
			if (co->rematchyes) {
				co->rematchyes=false;
			}
		} */
	}
	S32 nrematchyes = 0;
	for (i=0;i<gg->numplayers;++i) {
		twcaro* co=&gg->caros[i].cs;
		if (co->rematchyes) {
			++nrematchyes;
		}
		if (nrematchyes == gg->numplayers) { // all players hit yes
			rg = true;
		}
	}
	if (rg) { // restart game
		twrestore0();
		if (!gg->predicted)
			gg->plc->reset();
		ca->bigchange(); // camera moved alot, reset doppler vel
	}
// move/drive car, all physics
	physics_do(ks);
// run checkpoints/rabbits(ai) // dog race, move rabbit(s) forward, includes lap counters
	for (i=0;i<gg->numplayers;++i) {
		dorabbits(i);
	}
// end game scenario
	checkendgamerabbits();
	if (!gg->intimewarp) {
		S32 td = gg->gs.clock - gg->gs.clockoffset;
		if (td <= acv2.candrivetime) {
			if (td == 1) {
				sfx->playsnd(0,START1);
//				sfx->playsnd(0,ENGSTART);
			} else if (td == acv2.candrivetime/2) {
				sfx->playsnd(0,START1); // 2
			} else if (td == acv2.candrivetime) {
				sfx->playsnd(0,START2); // 3
			}
		}
	}
	++gg->gs.clock; // lastly, move the game clock forward 1 tick
// end 1 tick
}

static keys gethumankey()
{
	keys yk;
	yk.ku=wininfo.keystate[K_UP]||wininfo.keystate[K_NUMUP];
	yk.kd=wininfo.keystate[K_DOWN]||wininfo.keystate[K_NUMDOWN];
	yk.kr=wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT];
	yk.kl=wininfo.keystate[K_LEFT]||wininfo.keystate[K_NUMLEFT];
	if (client) {
		yk.ku = ctdata.buts[1];
		yk.kd = ctdata.buts[0];
		if (ctdata.tilt>1000*PI/8) {
			yk.kr = 1;
			yk.kl = 0;
		} else if (ctdata.tilt<-1000*PI/8) {
			yk.kr = 0;
			yk.kl = 1;
		} else {
			yk.kr = 0;
			yk.kl = 0;
		}
	}
	if (KEY == ' ')
		yk.kresetcar = true;
	if (!wininfo.releasemode) {
		if (KEY == 'g') {
			yk.kresetgame = true;
		}
	}
	if (getrematchbuts()==YES) {
		setrematchbuts(NONE);
		yk.krematchyes = true;
	} else
		yk.krematchyes = false;
	return yk;
}

static keys getnokey()
{
	keys yk;
	if (getrematchbuts()==YES) {
		setrematchbuts(NONE);
		yk.krematchyes = true;
	} else
		yk.krematchyes = false;
	return yk;
}

// with hysteresis
// allow full flexibility for now
static S32 calculoop() {
	if (wininfo.fpswanted == 60) {
		if (wininfo.fpsavg2 < 59.95f)
			wininfo.fpswanted = 30;
	} else if (wininfo.fpswanted == 30) {
		if (wininfo.fpsavg2 < 29.95f)
			wininfo.fpswanted = 20;
		else if (wininfo.fpsavg2 >= 61.0f)
			wininfo.fpswanted = 60;
	} else if (wininfo.fpswanted == 20) {
		if (wininfo.fpsavg2 < 19.95f)
			wininfo.fpswanted = 15;
		else if (wininfo.fpsavg2 >= 31.0f)
			wininfo.fpswanted = 30;
	} else if (wininfo.fpswanted == 15) {
		if (wininfo.fpsavg2 < 14.95f)
			wininfo.fpswanted = 12;
		else if (wininfo.fpsavg2 >= 21.0f)
			wininfo.fpswanted = 20;
	} else if (wininfo.fpswanted == 12) {
		if (wininfo.fpsavg2 < 11.95f)
			wininfo.fpswanted = 10;
		else if (wininfo.fpsavg2 >= 16.0f)
			wininfo.fpswanted = 15;
	} else if (wininfo.fpswanted == 10) {
		if (wininfo.fpsavg2 < 9.95f)
			wininfo.fpswanted = 6;
		else if (wininfo.fpsavg2 >= 13.0f)
			wininfo.fpswanted = 12;
	} else if (wininfo.fpswanted == 6) {
		if (wininfo.fpsavg2 >= 11.0f)
			wininfo.fpswanted = 10;
	} else { // something else
		wininfo.fpswanted = 30;
	}
	S32 ul;
	if (wininfo.fpswanted>0)
		ul = 60/wininfo.fpswanted;
	else
		ul = 2;
	return ul;
}

// used to move games with too early a time ahead (negative ping)
static S32 getping() // in 1/60 secs
{
// our last valid key time
	S32 ylvt=no->lastvalidkeytime(gg->yourid);
// find highest valid key time not our own
	S32 flvt=no->farthestvalidtime(gg->yourid);
	return ylvt - flvt;
}

static void chat()
{
	if (KEY=='t' && !gg->inchat) {
		gg->inchat=true;
		KEY=0;
	}
	if (gg->inchat) {
		if (KEY==K_RETURN) {
			if (!gg->chatstr.empty()) {
				vector<U8> mess(gg->chatstr.begin(),gg->chatstr.end());
				no->writegen(PACK_CHAT,mess); // send out a generic packet
				gg->chatstr.clear();
			}
			gg->inchat=false;
		} else if (KEY==K_BACKSPACE || KEY==K_DELETE) {
			if (!gg->chatstr.empty())
				gg->chatstr=gg->chatstr.substr(0,gg->chatstr.length()-1);
		} else if (isgraph(KEY) || KEY==' ') {
			if (gg->chatstr.length()<40)
				gg->chatstr.append(1,KEY);
		}
		KEY=0;
	}
}

// break this up some more...
void carenagameproc()
{
	if (!master) {
		int gotmyip=getnmyip();
		if (gotmyip)
			master = tcpmakemaster(0,2013);
	}
	if (!client && master) {
		client = tcpgetaccept(master);
		//if (client)
		//	con32_printf(con,"connected\n");
	}
	if (client) {
		tcpfillreadbuff(client);
		if (client->flags&SF_CLOSED) {
			freesocker(client);
			client = 0;
			//con32_printf(con,"closed\n");
		} else {
			int rd;
			do {
				rd = tcpgetbuffdata(client,(C8*)&ctdata,sizeof(ctdata),sizeof(ctdata));
				//if (rd>0) {
				//	con32_printf(con,"c %4d, b1 %d, b2 %d, tilt %d\n",
				//		cntr,ctdata.buts[0],ctdata.buts[1],ctdata.tilt);
					//con32_printf(con,"c %4d, b %d,%d a %5.2f %5.2f %5.2f\n",
					//	cntr,ctdata.buts[0],ctdata.buts[1],ctdata.accel[0],ctdata.accel[1],ctdata.accel[2]);
				//}
			} while(rd > 0);
		}
	}
// handle general input
// handle chat
	chat();
	procrematch(); // sets rematchbuts
	if (KEY==K_ESCAPE || getrematchbuts()==NO) {
		if (matchcl)
			changestate(STATE_MATCHRESULTS);
		else
			poporchangestate(STATE_NEWCARENALOBBY);
	}
// bail countdown and exit
	int i,j;
// change resolution
	if ((KEY=='-' || KEY=='=') && videoinfo.video_maindriver==VIDEO_GDI && !wininfo.releasemode) {
		int dir;
		if (KEY=='-')
			dir=-1;
		else
			dir=1;
		if (changeglobalxyres(dir)) {
			video_setupwindow(GX,GY);
			game_viewport.xres=WX;
			game_viewport.yres=WY;
			scrline_viewport.xres=WX;
			scrline_viewport.yres=WY;
		}
	}
// view other players
	if (!wininfo.releasemode) {
		switch(KEY) {
		case 'a':
			game_viewport.useattachcam^=1;
			break;
		case 's':
			sfx->playsnd(0,fxe(testsnd));
			++testsnd;
			if (testsnd==NSOUNDFX)
				testsnd=0;
			break;
		case ']':
			gg->viewslot++;
			if (gg->viewslot==gg->numplayers)
				gg->viewslot=0;
			break;
		case '[':
			gg->viewslot--;
			if (gg->viewslot<0)
				gg->viewslot=gg->numplayers-1;
			break;
// change driving mode for cars you own
		case 'm':
			gg->yourcntl++;
			if (gg->yourcntl==MAX_CNTL)
				gg->yourcntl=CNTL_NONE;
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
			gg->con->setvis(gamenews);
			break;
		case 'v':
			ca->changemode();
			break;
		}
	}
// handle game input
// build a 'keys' based on your control and input, used 'uloop' times
	keys yk;
	gg->predicted=false; // just to get some log from getbotkey
//	keys bk=getbotkey(gg->yourid);
	switch(gg->yourcntl) {
	case CNTL_NONE:
		yk = getnokey();
		break;
	case CNTL_HUMAN:
		yk = gethumankey();
		break;
	case CNTL_AI:
//		yk = bk;
		yk = getbotkey(gg->yourid);
		break;
	}
	setrematchbuts(NONE); // done reading rematch buttons
// set uloop depending on realtime or if waiting for other players
// adjust uloop, given (arrivals and queues not empty) and (neg ping)
	S32 lastvalidkeytime = no->lastvalidtime(); // everyone has written a key
	S32 ylkt = no->lastvalidkeytime(gg->yourid); // have you written any keys yet?
	bool arrived = (lastvalidkeytime>=0 || ylkt == -1); // writekey if game started or no keys in your keyqueue
	if (arrived) { // writekey if game started or no keys in your keyqueue
		uloop=calculoop(); // realtime, and sometimes adjusts 'fpswanted'
		ping = getping();
		if (ping < 0) { // slight speedup for negative ping
			S32 np = -ping;
			static S32 cnt; // uloop += np/DEN
			static const S32 DEN = 7;
			S32 q = np/DEN;
			S32 r = np%DEN;
			cnt += r;
			uloop += q;
			if (cnt >= DEN) {
				++uloop;
				cnt -= DEN;
			}
		}
	} else {
		uloop = 0; // wait for others to arrive (load time differences)
	}
// debprint uloop override
	if (gcfg.defuloop<0)
		uloop=0; // freeze
	else if (gcfg.defuloop>0)
		uloop=gcfg.defuloop; // manual override
// write out uloop keys to collective (same keys)
	if (uloop == 0)
		no->writenone(); // keepalive while waiting for all players to arrive, or just all uloop == 0
	for (i=0;i<uloop;++i) {
		if (i>0) {
			yk.krematchyes=false; // only send a oneshot krematchyes
			yk.kresetgame=false;
		}
		no->writekey(yk); // send your input to collective uloop times
	}
// all 'write' mail done by now
	// run net mailman/collective, all network stuff done here...
///////////////////////// mail stuff into mailbox
	string ret = no->proc(); // mailman arrived, move writekey to net, get net to readkey, all netcode is here...
	if (ret.size())
		gg->con->printf("%s",ret.c_str()); // messages from mailman
//	con32_printf(gg->con,"%s",ret.c_str()); // messages from mailman
///////////////////////// sift through mail
// start 'reading' mail now
// handle generic messages/packets
	{
		vector<U8> mess;
		PACK_KIND id;
		S32 i,n=gg->numplayers;
		for (i=0;i<n;++i) {
			while(no->readgen(i,id,mess)) { // read slot i
				switch(id) {
				case PACK_CHAT:
					if (mess.size()) {
						string messstr((C8*)(&mess[0]),mess.size());
						gg->con->printf("%s: %s",gg->caros[i].playername.c_str(),messstr.c_str());
//						con32_printf(gg->con,"%s: %s\n",gg->caros[i].playername.c_str(),messstr.c_str());
					}
					break;
#ifdef CHECKGAMESTATE
				case PACK_GAMESTATE:
					if (mess.size()) {
						gs->readgamestate(i,mess);
					}
					break;
#endif
				default:
					break;
				}
			}
		}
	}
// end handle generic messages/packets
// check up on rematch/exit
	if (gg->bail) {
		gg->bail-=uloop;
		if (gg->bail<=0) {
			if (matchcl)
				changestate(STATE_MATCHRESULTS);
			else
				poporchangestate(STATE_NEWCARENALOBBY);
		}
	}
// run game uloop times TW
	timewarp(uloop);
// game to engine
// update car
	for (j=0;j<gg->numplayers;++j) {
		gg->caros[j].carnull->trans=gg->caros[j].cs.pos;
		gg->caros[j].carnull->rot=gg->caros[j].cs.rot;
		gg->con->printf("spd = %f",gg->caros[j].cs.accelspin/acv2.startaccelspin);
		gg->caros[j].emit->update(gg->caros[j].cs.pos,gg->caros[j].cs.vel,gg->caros[j].cs.accelspin/acv2.startaccelspin*8.0f); // whine (*8) at full speed
//		pointf3x up(0,1,0);
//		pointf3 jquat;
//		normal2quaty(&up,&gg->caros[j].cs.normj,&jquat);
//		quattimes(&jquat,&gg->caros[j].cs.rot,&gg->caros[j].carnull->rot);
	}
// update scrline
	procscrline();
// update camera
#if 0
	game_viewport.camattach=gg->caros[gg->viewslot].carnull;//getlastcam();
	if (gg->caros[gg->viewslot].carnull)
		gg->caros[gg->viewslot].carnull->zoom=game_viewport.camzoom;
#else
#ifdef FULLPHYSICS
//	gamecam->trans = acv2.camatt;
//	getlookat(&acv2.camatt,&gg->caros[gg->viewslot].carnull->trans,&gamecam->rot);
	ca->proc(uloop);
// camera
	pointf3 cp = ca->getpos();
	pointf3 cr = ca->getrot();
	pointf3 cv = ca->getvel(uloop);
	sfx->setears(cp,cr);
//	gg->con->printf("cv %f %f %f",cv.x,cv.y,cv.z);
// car
//	const pointf3& vl = gg->caros[gg->viewslot].cs.vel;
//	gg->con->printf("vl %f %f %f",vl.x,vl.y,vl.z);
#endif
	sem->update(cp,cr,cv,acv2.speedofsound); // proc the sound emitters, motor sounds etc.
//	sem->update(pointf3x(0,0,0),pointf3x(0,0,0,1),pointf3x(0,0,0)); // proc the sound emitters, motor sounds etc.
#endif
// update 3d animation
	doflycam(&game_viewport);
	helpers->resethelper();
	roottree->proc();
	{ // move test sphere around, rabbit
		pointf3xh nl;
		const twcaro& cs=gg->caros[gg->viewslot].cs;
		if (cs.nnextloc>0) {
			nl.x=PIECESIZE*(cs.nextloc[0].x*1.0f+.5f); // middle of piece
			nl.z=PIECESIZE*(cs.nextloc[0].y*1.0f+.5f);
//			testsphere1->trans=nl;
//			testsphere1->flags&=~TF_DONTDRAW;
			helpers->addhelper(nl);
//		} else {
//			testsphere1->flags|=TF_DONTDRAW;
		}
		if (cs.nnextloc>1) {
			nl.x=PIECESIZE*(cs.nextloc[1].x*1.0f+.5f); // middle of piece
			nl.z=PIECESIZE*(cs.nextloc[1].y*1.0f+.5f);
//			testsphere2->trans=nl;
//			testsphere2->flags&=~TF_DONTDRAW;
			helpers->addhelper(nl);
//		} else {
//			testsphere2->flags|=TF_DONTDRAW;
		}
		nl.x=PIECESIZE*(cs.lastloc.x*1.0f+.5f); // middle of piece
		nl.z=PIECESIZE*(cs.lastloc.y*1.0f+.5f);
		helpers->addhelper(nl);
	}
	video_buildworldmats(roottree);
	video_buildworldmats(scrlinenull);
	dolights();
// end game to engine
}

void carenagamedraw3d()
{
// update video
// draw main scene
	const trkt* ot = gg->e.oldtrackj;
	if (!ot)
		errorexit("no old track to set viewport with");
	if (ot->skybox)
		game_viewport.flags=VP_CLEARWB;
	else
		game_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	video_setviewport(&game_viewport);
	video_drawscene(roottree);
// draw hud/scrline
	video_setviewport(&scrline_viewport);
	video_drawscene(scrlinenull);
	S32 j;
// draw console
	if (gamenews) {
#if 0
		clipblit32(con32_getbitmap32(gg->con),B32,0,0,CONLOCX,CONLOCY,CONSIZEX,CONSIZEY);
#endif
		S32 n=no->getnumsockets();
		for (j=0;j<n;++j) {
			S32 u=no->getwritebuffused(j);
			S32 s=no->getwritebuffsize(j);
			drawtextque_format_foreback(SMALLFONT,WX-88,8*j,F32BLACK,F32WHITE,"%5d/%5d",u,s);
		}
	}
	video_sprite_begin(
//	  GAMESPTRESX,GAMESPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  WX,WY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  0,
	  0);
//	rl->draw3d();
// draw clock
	j=gg->gs.clock%120;
	video_sprite_draw(gamewhitetex,F32WHITE,(float)4*j,0,8,8);
// draw keybuff
	no->drawkeybuff3d();
	drawtextque_format_foreback(SMALLFONT,WX-96,WY-8,F32WHITE,F32BLACK,"uloop %d",uloop);
	drawtextque_format_foreback(SMALLFONT,WX-96,WY-16,F32WHITE,F32BLACK,"clock %d",gg->gs.clock-gg->gs.clockoffset);
	if (/* gg->n.gamenet!=GAMENET_BOTGAME && */ !wininfo.releasemode) {
		drawtextque_format_foreback(SMALLFONT,WX-196,WY-24,F32WHITE,F32BLACK,"gn %d,ping %d",gg->gs.gamenum,ping);
	}
	drawtextque_format_foreback(SMALLFONT,WX-196,WY-8,F32WHITE,F32BLACK,"'%s'",gg->caros[gg->viewslot].playername.c_str());
	drawtextque_format_foreback(SMALLFONT,WX-196,WY-32,F32WHITE,F32BLACK,"lap %2d/%2d, lapratchet %d",gg->caros[gg->viewslot].cs.curlap,gg->e.nlaps,gg->caros[gg->viewslot].cs.lapratchet);
	const twcaro& cs=gg->caros[gg->viewslot].cs;
	drawtextque_format_foreback(SMALLFONT,WX-196,WY-40,F32WHITE,F32BLACK,"speed %f",cs.accelspin);
	drawtextque_format_foreback(SMALLFONT,WX-196,WY-48,F32WHITE,F32BLACK,"noshocktime %d,%d",cs.noshocktime,cs.noresettime);
	drawtextque_format_foreback(SMALLFONT,WX-196,WY-56,F32WHITE,F32BLACK,"nochecktime %d",cs.nocheckpointtime);
	drawtextque_format_foreback(SMALLFONT,WX-196,WY-64,F32WHITE,F32BLACK,"viewslot %d",gg->viewslot);
	if (gg->inchat) {
		video_sprite_draw(gamewhitetex,F32WHITE,0,(float)WY-24,320,8);
		drawtextque_format(SMALLFONT,0,WY-24,F32BLACK,"%s",gg->chatstr.c_str());
	}
	video_sprite_end();
// draw rematch
	video_sprite_begin(
	  GAMESPTRESX,GAMESPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
//	  WX,WY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  0,
	  0);
	drawsptscrline(GAMESPTRESX,GAMESPTRESY);
	drawrematch();
	video_sprite_end();
}
#if 0
void carenagamedraw2d()
{
	S32 j;
// draw console
	if (gamenews) {
//		clipblit32(con32_getbitmap32(gg->con),B32,0,0,CONLOCX,CONLOCY,CONSIZEX,CONSIZEY);
		S32 n=no->getnumsockets();
		for (j=0;j<n;++j) {
			S32 u=no->getwritebuffused(j);
			S32 s=no->getwritebuffsize(j);
			outtextxybf32(B32,WX-88,8*j,C32BLACK,C32WHITE,"%5d/%5d",u,s);
		}
	}
// draw clock
	j=gg->gs.clock%120;
	cliprect32(B32,4*j,0,8,8,C32WHITE);
// draw keybuff
	no->drawkeybuff();
// draw rematch
	drawrematch();
// draw ping time
#if 0
	if (/* gg->n.gamenet!=GAMENET_BOTGAME && */ !wininfo.releasemode) {
		outtextxybf32(B32,WX-96,WY-20-8,C32WHITE,C32BLACK,"ping %d",ping);
	}
	outtextxybf32(B32,WX-96,WY-40-8,C32WHITE,C32BLACK,"clock %d",gg->gs.clock-gg->gs.clockoffset);
	outtextxybf32(B32,WX-96,WY-40-16,C32WHITE,C32BLACK,"uloop %d",uloop);
#endif
	if (gg->inchat) {
		cliprect32(B32,0,WY-24,320,8,C32WHITE);
#if 0
		outtextxyf32(B32,0,WY-24,C32BLACK,"%s",gg->chatstr.c_str());
#endif
	}
}
#endif
void carenagameexit()
{
	logger("---------------------- carenamaingameexit -------------------------------\n");
// sounds
	wave_unload(backsndwavegame);
	backsndwavegame = 0;
	delete sfx;
	sfx = 0;
	delete sem;
	sem = 0;
// attach cam
	delete ca;
	ca = 0;
// helpers
	delete helpers;
	helpers = 0;
// physics
#ifdef FULLPHYSICS
	physics_exit();
#endif
// log viewports
	logger("game viewport\n");
	roottree->log2();
	logger("scrline viewport\n");
	scrlinenull->log2();
	logger("logging reference lists\n");
	logrc();
// free everthing
// remove debvars
	extradebvars(0,0);
// remove carclass
	int i;
	for (i=0;i<gg->numplayers;++i) {
		delete gg->caros[i].cc;
		gg->caros[i].cc=0;
	}
	gamewhitetex->rc.deleterc(gamewhitetex);
// network
	delete no;
	no=0;
// console
//	con32_free(gg->con);
// rematch
	freerematch();
// linetoroad
	st2_freecollgrids();
	delete gg;
	gg=0;
// game root tree
	delete roottree;
// hud/scrline
	exitscrline();
#ifdef CHECKGAMESTATE
	delete gs;
	gs=0;
#endif
	exit_res3d();
popdir();
	wininfo.fpswanted=fpssave;
	cgi.isvalid = false;
	freesocker(client);
	freesocker(master);
	uninitsocker();
}
