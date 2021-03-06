#define INCLUDE_WINDOWS
#define D2_3D
#include <m_eng.h>
#include <d3d9.h>
#include "m_vid_dx9.h"

// user includes
#include "newcarena/jrmcars.h" // for newcarena
#include "newcarena/enums.h" // for newcarena
#include "newcarena/carclass.h" // for newcarena
#include "newcarena/carenalobby.h" // for newcarena
//#include "newcarena/n_loadweapcpp.h" // for newcarena
#include "utils/json.h" // for newcarena
#include "newcarena/hash.h" // for newcarena
#include "newcarena/constructor.h" // for newcarena
#include "newcarena/carenagame.h" // for newcarena
#include "newcarena/timewarp.h"
#include "newcarena/avaj.h"
#include "newcarena/envj.h"
#include "newcarena/physics.h"

struct menuvar wininfovars[]={

	{"@lightred@---- wininfo timer --",0,D_VOID,0},
	{"fpswanted",&wininfo.fpswanted,D_INT,1},
	{"hiprioritywanted",&wininfo.hiprioritywanted,D_INT|D_RDONLY,1},
	{"fpscurrent",&wininfo.fpscurrent,D_FLOAT|D_RDONLY,FLOATUP},
	{"fpsavg",&wininfo.fpsavg,D_FLOAT|D_RDONLY},
	{"microseccurrent",&wininfo.microseccurrent,D_INT|D_RDONLY},
	{"framestep",&wininfo.framestep,D_FLOAT|D_RDONLY},
	{"microsecavg",&wininfo.microsecavg,D_INT|D_RDONLY},
	{"fpscurrent2",&wininfo.fpscurrent2,D_FLOAT|D_RDONLY,FLOATUP},
	{"fpsavg2",&wininfo.fpsavg2,D_FLOAT|D_RDONLY,FLOATUP},
	{"microseccurrent2",&wininfo.microseccurrent2,D_INT|D_RDONLY},
	{"microsecavg2",&wininfo.microsecavg2,D_INT|D_RDONLY},
	{"sleepernum",&wininfo.sleepernum,D_INT|D_RDONLY},
	{"compkhz",&wininfo.compkhz,D_INT|D_RDONLY},
	{"slacktime",&wininfo.slacktime,D_INT|D_RDONLY},
	{"lasttime ",&wininfo.lasttime,D_INT|D_RDONLY},
	{"lasttime2",&wininfo.lasttime2,D_INT|D_RDONLY},

	{"@yellow@---- wininfo settings -------------------",0,D_VOID,0},
	{"releasemode",&wininfo.releasemode,D_INT},
	{"enabledebprint",&wininfo.enabledebprint,D_INT,1},
	{"runinbackground",&wininfo.runinbackground,D_INT},
	{"nosleep",&wininfo.nosleep,D_INT},
	{"isalreadyrunning",&wininfo.isalreadyrunning,D_INT|D_RDONLY},
	{"startstate",&wininfo.startstate,D_INT|D_RDONLY},
	{"wantedvideomaindriver",&wininfo.wantedvideomaindriver,D_INT|D_RDONLY},
	{"wantedvideosubdriver",&wininfo.wantedvideosubdriver,D_INT|D_RDONLY},
	{"dopaintmessages",&wininfo.dopaintmessages,D_INT},
	{"wantedaudiomaindriver",&wininfo.wantedaudiomaindriver,D_INT|D_RDONLY},
	{"wantedaudiosubdriver",&wininfo.wantedaudiosubdriver,D_INT|D_RDONLY},
	{"wantedaudiomicsubdriver",&wininfo.wantedaudiomicsubdriver,D_INT|D_RDONLY},
	{"wantedusedirectinput",&wininfo.wantedusedirectinput,D_INT|D_RDONLY},
	{"repdelay",&wininfo.repdelay,D_INT,1},
	{"repperiod",&wininfo.repperiod,D_INT,1},
	{"dinput_nomouse",&wininfo.dinput_nomouse,D_INT|D_RDONLY},
	{"mousemode",&wininfo.mousemode,D_INT,1},
	{"use8way",&wininfo.use8way,D_INT,1},
	{"gamexyidx",&videoinfo.gamexyidx,D_INT|D_RDONLY},
	{"dumpbws",&wininfo.dumpbws,D_INT,1},
	{"dumpbwo",&wininfo.dumpbwo,D_INT,1},
	{"relquat",&wininfo.relquat,D_INT,1},
	{"@lightcyan@---- videoinfo settings -------------------",0,D_VOID,0},
	{"wx",&videoinfo.wx,D_INT|D_RDONLY},
	{"wy",&videoinfo.wy,D_INT|D_RDONLY},
	{"sx",&videoinfo.sx,D_INT|D_RDONLY},
	{"sy",&videoinfo.sy,D_INT|D_RDONLY},
	{"video_maindriver",&videoinfo.video_maindriver,D_INT|D_RDONLY},
	{"video_subdriver",&videoinfo.video_subdriver,D_INT|D_RDONLY},
	{"hiendmachine",&videoinfodx9.hiendmachine,D_INT|D_RDONLY},
	{"texreduce",&videoinfo.texreduce,D_INT,1},
	{"fogenable",&videoinfodx9.fog.enable,D_INT,1},
	{"fogcolorx",&videoinfodx9.fog.color.x,D_FLOAT,FLOATUP/16},
	{"fogcolory",&videoinfodx9.fog.color.y,D_FLOAT,FLOATUP/16},
	{"fogcolorz",&videoinfodx9.fog.color.z,D_FLOAT,FLOATUP/16},
	{"fogcolorw",&videoinfodx9.fog.color.w,D_FLOAT,FLOATUP/16},
	{"fogmode",&videoinfodx9.fog.mode,D_INT,1},
	{"fogstart",&videoinfodx9.fog.start,D_FLOAT,FLOATUP/16},
	{"fogend",&videoinfodx9.fog.end,D_FLOAT,FLOATUP/16},
	{"fogdensity",&videoinfodx9.fog.density,D_FLOAT,FLOATUP/16},
	{"usemip",&videoinfodx9.usemip,D_INT,1},
	{"miplodbias",&videoinfodx9.miplodbias,D_FLOAT,FLOATUP/16},
	{"shadowepsilon",&videoinfodx9.shadowepsilon,D_FLOAT,FLOATUP/16},
	{"cullmode",&videoinfodx9.cullmode,D_INT,1},
	{"fillmode",&videoinfodx9.fillmode,D_INT,1},
	{"toonedgesize",&videoinfodx9.toonedgesize,D_FLOAT,FLOATUP/64},
	{"stanx",&videoinfodx9.stanx,D_FLOAT,FLOATUP/64},
	{"stany",&videoinfodx9.stany,D_FLOAT,FLOATUP/64},
	{"edgesize",&videoinfodx9.edgesize,D_FLOAT,FLOATUP/256},
	{"edgezoff",&videoinfodx9.edgezoff,D_FLOAT,FLOATUP/1024},
	{"edgecosang",&videoinfodx9.edgecosang,D_FLOAT,FLOATUP/64},
	{"normallength",&videoinfodx9.normallength,D_FLOAT,FLOATUP/4},
	{"tooningame",&videoinfodx9.tooningame,D_INT,1},

	{"@lightgreen@---- mouse input -------------",0,D_VOID,0},
	{"mx",&wininfo.mx,D_INT|D_RDONLY},
	{"my",&wininfo.my,D_INT|D_RDONLY},
	{"mz",&wininfo.mz,D_INT|D_RDONLY},
	{"lmx",&wininfo.lmx,D_INT|D_RDONLY},
	{"lmy",&wininfo.lmy,D_INT|D_RDONLY},
	{"lmz",&wininfo.lmz,D_INT|D_RDONLY},
	{"dmx",&wininfo.dmx,D_INT|D_RDONLY},
	{"dmy",&wininfo.dmy,D_INT|D_RDONLY},
	{"dmz",&wininfo.dmz,D_INT|D_RDONLY},
	{"mleftclicks",&wininfo.mleftclicks,D_INT|D_RDONLY},
	{"mrightclicks",&wininfo.mrightclicks,D_INT|D_RDONLY},
	{"nmiddleclicks",&wininfo.mmiddleclicks,D_INT|D_RDONLY},
	{"mbut",&wininfo.mbut,D_INT|D_RDONLY,1},
	{"lmbut",&wininfo.lmbut,D_INT|D_RDONLY,1},

	{"@lightcyan@---- keyboard input -----------",0,D_VOID,0},
	{"key",&wininfo.key,D_INT|D_RDONLY,1},

	{"@lightblue@---- colorkey -----------",0,D_VOID,0},
	{"usecolorkey",&colorkeyinfo.usecolorkey,D_INT,1},
	{"@lightgreen@---- keyframe tester -----------",0,D_VOID,0},
	{"override",&keytester.manualframe,D_INT,1},
	{"framenum",&keytester.framenum,D_FLOAT,FLOATUP/4},
	{"framestep",&keytester.framestep,D_FLOAT,FLOATUP/16},
#ifdef D2_3D
	{"@white@---- main viewport --------",0,D_VOID,0},
	{"mainvp.zfront",&mainvp.zfront,D_FLOAT,FLOATUP},
	{"mainvp.zback",&mainvp.zback,D_FLOAT,FLOATUP},
	{"mainvp.camtrans.x",&mainvp.camtrans.x,D_FLOAT,FLOATUP},
	{"mainvp.camtrans.y",&mainvp.camtrans.y,D_FLOAT,FLOATUP},
	{"mainvp.camtrans.z",&mainvp.camtrans.z,D_FLOAT,FLOATUP},
	{"mainvp.camrot.x",&mainvp.camrot.x,D_FLOAT,FLOATUP},
	{"mainvp.camrot.y",&mainvp.camrot.y,D_FLOAT,FLOATUP},
	{"mainvp.camrot.z",&mainvp.camrot.z,D_FLOAT,FLOATUP},
	{"mainvp.camrot.w",&mainvp.camrot.w,D_FLOAT,FLOATUP},
	{"mainvp.camzoom",&mainvp.camzoom,D_FLOAT,FLOATUP/64},
	{"mainvp.flags",&mainvp.flags,D_HEX,1},
	{"mainvp.isortho",&mainvp.isortho,D_INT,1},
	{"mainvp.ortho_size",&mainvp.ortho_size,D_FLOAT,FLOATUP},
	{"mainvp.useattachcam",&mainvp.useattachcam,D_INT,1},
	{"lightvp.zfront",&lightvp.zfront,D_FLOAT,FLOATUP},
	{"lightvp.zback",&lightvp.zback,D_FLOAT,FLOATUP},
	{"lightvp.camtrans.x",&lightvp.camtrans.x,D_FLOAT,FLOATUP},
	{"lightvp.camtrans.y",&lightvp.camtrans.y,D_FLOAT,FLOATUP},
	{"lightvp.camtrans.z",&lightvp.camtrans.z,D_FLOAT,FLOATUP},
	{"lightvp.camrot.x",&lightvp.camrot.x,D_FLOAT,FLOATUP},
	{"lightvp.camrot.y",&lightvp.camrot.y,D_FLOAT,FLOATUP},
	{"lightvp.camrot.z",&lightvp.camrot.z,D_FLOAT,FLOATUP},
	{"lightvp.camrot.w",&lightvp.camrot.w,D_FLOAT,FLOATUP},
	{"lightvp.camzoom",&lightvp.camzoom,D_FLOAT,FLOATUP/64},
	{"lightvp.flags",&lightvp.flags,D_HEX,1},
	{"lightvp.isortho",&lightvp.isortho,D_INT,1},
	{"lightvp.ortho_size",&lightvp.ortho_size,D_FLOAT,FLOATUP},
	{"@white@---- lights --------",0,D_VOID,0},
	{"dodefaultlights",&lightinfo.dodefaultlights,D_INT,1},
	{"uselights",&lightinfo.uselights,D_INT,1},
	{"def amb colr",&lightinfo.ambcolor.x,D_FLOAT,FLOATUP/4},
	{"def amb colg",&lightinfo.ambcolor.y,D_FLOAT,FLOATUP/4},
	{"def amb colb",&lightinfo.ambcolor.z,D_FLOAT,FLOATUP/4},
	{"def light0 rotx",&lightinfo.deflightrot[0].x,D_FLOAT,FLOATUP/4},
	{"def light0 roty",&lightinfo.deflightrot[0].y,D_FLOAT,FLOATUP/4},
	{"def light0 rotz",&lightinfo.deflightrot[0].z,D_FLOAT,FLOATUP/4},
	{"def light0 posx",&lightinfo.deflightpos[0].x,D_FLOAT,FLOATUP/4},
	{"def light0 posy",&lightinfo.deflightpos[0].y,D_FLOAT,FLOATUP/4},
	{"def light0 posz",&lightinfo.deflightpos[0].z,D_FLOAT,FLOATUP/4},
	{"def light0 colr",&lightinfo.lightcolors[0].x,D_FLOAT,FLOATUP/4},
	{"def light0 colg",&lightinfo.lightcolors[0].y,D_FLOAT,FLOATUP/4},
	{"def light0 colb",&lightinfo.lightcolors[0].z,D_FLOAT,FLOATUP/4},
	{"@yellow@---- tree --------",0,D_VOID,0},
	{"flycamspeed",&treeinfo.flycamspeed,D_FLOAT,FLOATUP/16},
	{"flycam",&treeinfo.flycam,D_INT,1},
	{"flycamrevy",&treeinfo.flycamrevy,D_INT,1},
	{"dissolvestart",&treeinfo.dissolvestart,D_FLOAT,FLOATUP/64},
	{"defaultdissolvecutoff",&treeinfo.defaultdissolvecutoff,D_FLOAT,FLOATUP/64},
	{"treedissolvecutoffenable",&treeinfo.treedissolvecutoffenable,D_INT,1},
	{"zenable",&video3dinfo.zenable,D_INT},
	{"perscorrect",&video3dinfo.perscorrect,D_INT,1},
	{"favorshading",&video3dinfo.favorshading,D_INT,1},
	{"favorlightmap",&video3dinfo.favorlightmap,D_INT,1},
	{"bbox clip status",&video3dinfo.bboxclipstatus,D_INT|D_RDONLY},
	{"clip dot",&video3dinfo.clipdot,D_FLOAT|D_RDONLY},
	{"zbuffmin",&video3dinfo.zbuffmin,D_FLOAT|D_RDONLY},
	{"zbuffmax",&video3dinfo.zbuffmax,D_FLOAT|D_RDONLY},
	{"cantoon",&video3dinfo.cantoon,D_INT|D_RDONLY},
	{"canshadow",&video3dinfo.canshadow,D_INT|D_RDONLY},
	{"is3dhardware",&video3dinfo.is3dhardware,D_INT|D_RDONLY},
#endif
// joystick input all readonly
	{"@lightmagenta@---- joystick input --------",0,D_VOID,0},
	{"joy[0].newsjoy",&jdata[0].newsjoy,D_HEX|D_RDONLY},
	{"joy[0].newsjoyl",&jdata[0].newsjoyl,D_HEX|D_RDONLY},
	{"joy[0].newsjoyd",&jdata[0].newsjoyd,D_HEX|D_RDONLY},
	{"joy[0].newsjoyrep",&jdata[0].newsjoyrep,D_HEX|D_RDONLY},
	{"joy[0].newsjoyaxis[0]",&jdata[0].newsjoyaxis[0],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisl[0]",&jdata[0].newsjoyaxisl[0],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisd[0]",&jdata[0].newsjoyaxisd[0],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxis[1]",&jdata[0].newsjoyaxis[1],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisl[1]",&jdata[0].newsjoyaxisl[1],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisd[1]",&jdata[0].newsjoyaxisd[1],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxis[2]",&jdata[0].newsjoyaxis[2],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisl[2]",&jdata[0].newsjoyaxisl[2],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisd[2]",&jdata[0].newsjoyaxisd[2],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxis[3]",&jdata[0].newsjoyaxis[3],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisl[3]",&jdata[0].newsjoyaxisl[3],D_INT|D_RDONLY},
	{"joy[0].newsjoyaxisd[3]",&jdata[0].newsjoyaxisd[3],D_INT|D_RDONLY},
	{"joy[1].newsjoy",&jdata[1].newsjoy,D_HEX|D_RDONLY},
	{"joy[1].newsjoyl",&jdata[1].newsjoyl,D_HEX|D_RDONLY},
	{"joy[1].newsjoyd",&jdata[1].newsjoyd,D_HEX|D_RDONLY},
	{"joy[1].newsjoyrep",&jdata[1].newsjoyrep,D_HEX|D_RDONLY},
	{"joy[1].newsjoyaxis[0]",&jdata[1].newsjoyaxis[0],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisl[0]",&jdata[1].newsjoyaxisl[0],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisd[0]",&jdata[1].newsjoyaxisd[0],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxis[1]",&jdata[1].newsjoyaxis[1],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisl[1]",&jdata[1].newsjoyaxisl[1],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisd[1]",&jdata[1].newsjoyaxisd[1],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxis[2]",&jdata[1].newsjoyaxis[2],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisl[2]",&jdata[1].newsjoyaxisl[2],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisd[2]",&jdata[1].newsjoyaxisd[2],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxis[3]",&jdata[1].newsjoyaxis[3],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisl[3]",&jdata[1].newsjoyaxisl[3],D_INT|D_RDONLY},
	{"joy[1].newsjoyaxisd[3]",&jdata[1].newsjoyaxisd[3],D_INT|D_RDONLY},
// numath
	{"@white@---- user --------",0,D_VOID,0},
	{"userinta",&userinta,D_INT,1},
	{"userintb",&userintb,D_INT,1},
	{"userintc",&userintc,D_INT,1},
	{"userintd",&userintd,D_INT,1},
	{"userfloata",&userfloata,D_FLOAT,FLOATUP/4},
	{"userfloatb",&userfloatb,D_FLOAT,FLOATUP/4},
	{"userfloatc",&userfloatc,D_FLOAT,FLOATUP/4},
	{"userfloatd",&userfloatd,D_FLOAT,FLOATUP/4},
	{"userpointf3a.x",&userpointf3a.x,D_FLOAT,FLOATUP/16},
	{"userpointf3a.y",&userpointf3a.y,D_FLOAT,FLOATUP/16},
	{"userpointf3a.z",&userpointf3a.z,D_FLOAT,FLOATUP/16},
	{"userpointf3a.w",&userpointf3a.w,D_FLOAT,FLOATUP/16},
	{"userpointf3b.x",&userpointf3b.x,D_FLOAT,FLOATUP/16},
	{"userpointf3b.y",&userpointf3b.y,D_FLOAT,FLOATUP/16},
	{"userpointf3b.z",&userpointf3b.z,D_FLOAT,FLOATUP/16},
	{"userpointf3b.w",&userpointf3b.w,D_FLOAT,FLOATUP/16},
	{"userpointf3c.x",&userpointf3c.x,D_FLOAT,FLOATUP/16},
	{"userpointf3c.y",&userpointf3c.y,D_FLOAT,FLOATUP/16},
	{"userpointf3c.z",&userpointf3c.z,D_FLOAT,FLOATUP/16},
	{"userpointf3c.w",&userpointf3c.w,D_FLOAT,FLOATUP/16},
	{"userpointf3d.x",&userpointf3d.x,D_FLOAT,FLOATUP/16},
	{"userpointf3d.y",&userpointf3d.y,D_FLOAT,FLOATUP/16},
	{"userpointf3d.z",&userpointf3d.z,D_FLOAT,FLOATUP/16},
	{"userpointf3d.w",&userpointf3d.w,D_FLOAT,FLOATUP/16},

////// user

// lobby_viewport
	{"---- lobby camera (local) -----------------",NULL,D_VOID,0},
	{"lobbycamtrans.x",&lobby_viewport.camtrans.x,D_FLOAT,FLOATUP/4},
	{"lobbycamtrans.y",&lobby_viewport.camtrans.y,D_FLOAT,FLOATUP/4},
	{"lobbycamtrans.z",&lobby_viewport.camtrans.z,D_FLOAT,FLOATUP/4},
	{"lobbycamrot.x",&lobby_viewport.camrot.x,D_FLOAT,FLOATUP/4},
	{"lobbycamrot.y",&lobby_viewport.camrot.y,D_FLOAT,FLOATUP/4},
	{"lobbycamrot.z",&lobby_viewport.camrot.z,D_FLOAT,FLOATUP/4},
	{"lobbycamzoom",&lobby_viewport.camzoom,D_FLOAT,FLOATUP/4},
// track_viewport
	{"trackcamtrans.x",&track_viewport.camtrans.x,D_FLOAT,FLOATUP/4},
	{"trackcamtrans.y",&track_viewport.camtrans.y,D_FLOAT,FLOATUP/4},
	{"trackcamtrans.z",&track_viewport.camtrans.z,D_FLOAT,FLOATUP/4},
	{"trackcamrot.x",&track_viewport.camrot.x,D_FLOAT,FLOATUP/4},
	{"trackcamrot.y",&track_viewport.camrot.y,D_FLOAT,FLOATUP/4},
	{"trackcamrot.z",&track_viewport.camrot.z,D_FLOAT,FLOATUP/4},
	{"trackcamzoom",&track_viewport.camzoom,D_FLOAT,FLOATUP/4},
// constructor_viewport
	{"---- constructor camera (local) -----------------",NULL,D_VOID,0},
	{"constructorcamtrans.x",&constructor_viewport.camtrans.x,D_FLOAT,FLOATUP/4},
	{"constructorcamtrans.y",&constructor_viewport.camtrans.y,D_FLOAT,FLOATUP/4},
	{"constructorcamtrans.z",&constructor_viewport.camtrans.z,D_FLOAT,FLOATUP/4},
	{"constructorcamrot.x",&constructor_viewport.camrot.x,D_FLOAT,FLOATUP/4},
	{"constructorcamrot.y",&constructor_viewport.camrot.y,D_FLOAT,FLOATUP/4},
	{"constructorcamrot.z",&constructor_viewport.camrot.z,D_FLOAT,FLOATUP/4},
	{"constructorcamzoom",&constructor_viewport.camzoom,D_FLOAT,FLOATUP/4},
// constructor_viewport2
	{"---- constructor camera 2 (local) -----------------",NULL,D_VOID,0},
	{"constructorcamtrans2.x",&constructor_viewport2.camtrans.x,D_FLOAT,FLOATUP/4},
	{"constructorcamtrans2.y",&constructor_viewport2.camtrans.y,D_FLOAT,FLOATUP/4},
	{"constructorcamtrans2.z",&constructor_viewport2.camtrans.z,D_FLOAT,FLOATUP/4},
	{"constructorcamrot2.x",&constructor_viewport2.camrot.x,D_FLOAT,FLOATUP/4},
	{"constructorcamrot2.y",&constructor_viewport2.camrot.y,D_FLOAT,FLOATUP/4},
	{"constructorcamrot2.z",&constructor_viewport2.camrot.z,D_FLOAT,FLOATUP/4},
	{"constructorcamzoom2",&constructor_viewport2.camzoom,D_FLOAT,FLOATUP/4},
// gcfg
	{"---- game camera (local) -----------------",NULL,D_VOID,0},
	{"gamecamtrans.x",&gcfg.campos.x,D_FLOAT,FLOATUP/4},
	{"gamecamtrans.y",&gcfg.campos.y,D_FLOAT,FLOATUP/4},
	{"gamecamtrans.z",&gcfg.campos.z,D_FLOAT,FLOATUP/4},
	{"gamecamrot.x",&gcfg.camrot.x,D_FLOAT,FLOATUP/4},
	{"gamecamrot.y",&gcfg.camrot.y,D_FLOAT,FLOATUP/4},
	{"gamecamrot.z",&gcfg.camrot.z,D_FLOAT,FLOATUP/4},
	{"gamecamzoom",&gcfg.camzoom,D_FLOAT,FLOATUP/4},
	{"scrlinez",&gcfg.scrlinez,D_FLOAT,FLOATUP},
// times
// net
#if 0
	{"---- game loading goal -----------------",NULL,D_VOID,0},
	{"beforeloadtimeoutserver",&gcfg.beforeloadtimeoutserver,D_INT,1},
	{"beforeloadtimeoutclient",&gcfg.beforeloadtimeoutclient,D_INT,1},
	{"afterloadtimeoutserver",&gcfg.afterloadtimeoutserver,D_INT,1},
	{"afterloadtimeoutclient",&gcfg.afterloadtimeoutclient,D_INT,1},
	{"ingametimeout",&gcfg.ingametimeout,D_INT,1},
#endif
	// nonet
	{"statustime",&gcfg.statustime,D_INT,1},
//	{"rematchtime",&gcfg.rematchtime,D_INT,1},
//	{"norematchtime",&gcfg.norematchtime,D_INT,1},
// distances
//	{"car2cardist",&gcfg.car2cardist,D_FLOAT,FLOATUP},
// GO!, time
//	{"candrivetime",&gcfg.candrivetime,D_INT,1},
// other settings
//	{"loadinggoal",&gcfg.loadinggoal,D_INT,1},
	{"defuloop",&gcfg.defuloop,D_INT,1},
	{"movespeed",&n_physics::movespeed,D_FLOAT,FLOATUP/4},
};

const S32 nwininfovars=sizeof(wininfovars)/sizeof(wininfovars[0]);
