#include <stdio.h>
#include <engine1.h>
#include <debprint.h>

#include "stubhelper.h"

#include "globalres.h"
#include "line2road.h"
#include "online_uplay.h"
#include "camera.h"
#include "onlinecars.h"

extern int aliasmode;

// enums
static char *noyes[]={"no","yes",NULL};
static char *drivemodestr[]={"coast","brake","accel","revaccel",NULL};
static char *aliasmodes[]={"ANTI-ALIAS OFF","ANTI-ALIAS ON EDGES","ANTI-ALIAS FULLSCREEN I","ANTI-ALIAS FULLSCREEN D",NULL};

extern int testtotal,interleavealways,dopaintmessages;
extern int startstate;

extern int trikind;
extern float flycamspeed;
extern int myid;

//extern float motorpan,motorfreq,motorvol;
extern float revfreqmul,revfreqadd;
extern float framenum;
extern int manualframe,useattachcam,usetargetcam;

static char *clipret[]={"clipout","clipin","clipclip",NULL};
static char *noyesfrommodel[]={"no","yes","frommodel",NULL};
extern int bboxret;
extern float helperscale;
extern int showbones,showlights,showcameras,showobjs,shownulls;
extern int usescnlights,usescnbones,usescncamera;
extern int uselights;
extern int dodefaultlights;
extern int restbones;
extern int smoverride;
extern float steamd;
extern float lavad;
static char *smoothstr[]={"facet","smooth","smoothang",NULL};
extern int clipmap;
extern int usealwaysfacing;
extern int perscorrect;
extern int startaudiomaindriver,startaudiosubdriver;
extern int startvideomaindriver,startvideosubdriver;
extern int disablewpsetvol;
extern int runinbackground;
//extern int camflyframes;
extern VEC lightheight;
extern VEC gp;

extern int updaterenderstates;
extern int zenable,softwarecolorkeyonly;
extern int hardwareclipping;
extern int texwrap;
extern int filter;
extern int vsync;
extern float clearzbuffval;
extern int favorshading;
extern int usecvertsfromuv;
extern int runexclusive;
extern int mousemode;
extern int streamaudiopos;
extern int startusedirectinput;
static char *clipmapenumstr[]={"none","colorkey","1bitalpha","1and4bitalpha",NULL};
static char *mousemodestr[]={"normal","infinite",NULL};
extern int enabledebprint;
extern int enablenumkeychangestate;
extern int repdelay,repperiod;
extern int fastnorms;
extern int disabledissolve,forcealphamodulate;

extern VEC camstartpos;
extern float colcamdist;
extern float colcampushoff;
extern float camposdrift;
extern float camoffsety;
extern float camposdrift;
//extern float cratersize,craterdepth;
extern int flycamrevy;
extern int gto;
extern int footprints;
extern int speedupdeformobjects;
extern int texavail[];
extern int fogmethod;
extern int fogtype;
extern VEC fogcolor;
extern float fogstart;
extern float fogend;
extern float fogdensity;
static char *fogenum[]={"Vertex Fog","Pixel Fog","No Fog",NULL};
static char *fogform[]={"None","Exp","Exp2","Linear",NULL};
extern int fogenable;

// st2_
//extern float bigtreeminx,bigtreeminz,bigtreemaxx,bigtreemaxz;
extern int defaultalphacutoff;
extern int dofilelogger;
extern int showpri;
extern float testheight;
static float dummy;
extern VEC testimpval;
extern VEC testimppnt;
extern int slecs;	 // primitive checksum for txt files
extern int extractsleep;
extern int midi_disabled;
//extern char teststr[];
extern int d3denhancedwanted,mipmapwanted;
extern float shinycarscale;
extern VEC shinycarrot,shinycartrans;
extern float jrmcarscale;
extern VEC jrmcarrot,jrmcartrans;
extern float envmapoffsetv;
extern float video_mipmaplodbias;
extern int timalpha;
extern VEC roadprobe1,roadprobe2,roadprobei,roadprobenorm;

struct menuvar debvars[]={
	{"---------- ROAD PROBE ------------------",NULL,D_VOID,0},
	{"roadprobe1x",&roadprobe1.x,D_FLOAT,FLOATUP},
	{"roadprobe1y",&roadprobe1.y,D_FLOAT,FLOATUP},
	{"roadprobe1z",&roadprobe1.z,D_FLOAT,FLOATUP},
	{"roadprobe2x",&roadprobe2.x,D_FLOAT,FLOATUP},
	{"roadprobe2y",&roadprobe2.y,D_FLOAT,FLOATUP},
	{"roadprobe2z",&roadprobe2.z,D_FLOAT,FLOATUP},
	{"roadprobeix",&roadprobei.x,D_FLOAT,FLOATUP},
	{"roadprobeiy",&roadprobei.y,D_FLOAT,FLOATUP},
	{"roadprobeiz",&roadprobei.z,D_FLOAT,FLOATUP},
	{"roadprobenormx",&roadprobenorm.x,D_FLOAT,FLOATUP},
	{"roadprobenormy",&roadprobenorm.y,D_FLOAT,FLOATUP},
	{"roadprobenormz",&roadprobenorm.z,D_FLOAT,FLOATUP},
	{"excludelist0",excludelist[0],D_STR,0},
	{"excludelist1",excludelist[1],D_STR,0},
	{"excludelist2",excludelist[2],D_STR,0},
	{"excludelist3",excludelist[3],D_STR,0},
	{"excludelist4",excludelist[4],D_STR,0},
	{"excludelist5",excludelist[5],D_STR,0},
	{"#---- wininfo -----------------",NULL,D_VOID,0},
	{"SX",&SX,D_INT|D_RDONLY},
	{"SY",&SY,D_INT|D_RDONLY},
	{"WX",&WX,D_INT|D_RDONLY},
	{"WY",&WY,D_INT|D_RDONLY},
	{"BPP",&BPP,D_INT|D_RDONLY},
	{"sleepernum",&wininfo.sleepernum,D_INT,1},
	{"fpswanted",&wininfo.fpswanted,D_INT,1},
	{"fpsavg",&wininfo.fpsavg,D_FLOAT|D_RDONLY},
	{"fpscurrent",&wininfo.fpscurrent,D_FLOAT|D_RDONLY},
	{"microseccurrent",&wininfo.microseccurrent,D_INT|D_RDONLY},
	{"microsecavg",&wininfo.microsecavg,D_INT|D_RDONLY},
	{"fpsavg2",&wininfo.fpsavg2,D_FLOAT|D_RDONLY},
	{"fpscurrent2",&wininfo.fpscurrent2,D_FLOAT|D_RDONLY},
	{"microseccurrent2",&wininfo.microseccurrent2,D_INT|D_RDONLY},
	{"microsecavg2",&wininfo.microsecavg2,D_INT|D_RDONLY},
	{"texavail[TEX555NCK]",&texavail[0],D_ENUM|D_RDONLY,1,noyes},
	{"texavail[TEX565NCK]",&texavail[1],D_ENUM|D_RDONLY,1,noyes},
	{"texavail[TEX5551A]",&texavail[2],D_ENUM|D_RDONLY,1,noyes},
	{"texavail[TEX4444A]",&texavail[3],D_ENUM|D_RDONLY,1,noyes},
	{"texavail[TEX555CK]",&texavail[4],D_ENUM|D_RDONLY,1,noyes},
	{"texavail[TEX565CK]",&texavail[5],D_ENUM|D_RDONLY,1,noyes},
	{"disabledissolve",&disabledissolve,D_ENUM,1,noyes},
	{"forcealphamodulate",&forcealphamodulate,D_ENUM,1,noyes},
	{"argc",&wininfo.argc,D_INT|D_RDONLY},
	{"ndrops",&wininfo.ndrops,D_INT|D_RDONLY},
	{"MX",&MX,D_INT|D_RDONLY},
	{"MY",&MY,D_INT|D_RDONLY},
	{"lmx",&wininfo.lmx,D_INT|D_RDONLY},
	{"lmy",&wininfo.lmy,D_INT|D_RDONLY},
	{"dmx",&wininfo.dmx,D_INT|D_RDONLY},
	{"dmy",&wininfo.dmy,D_INT|D_RDONLY},
	{"MBUT",&MBUT,D_INT|D_RDONLY},
	{"MLEFTC",&wininfo.mleftclicks,D_INT|D_RDONLY},
	{"MRIGHTC",&wininfo.mrightclicks,D_INT|D_RDONLY},
	{"KEY",&KEY,D_INT|D_RDONLY},
	{"JX",&JX,D_FLOAT|D_RDONLY},
	{"JY",&JY,D_FLOAT|D_RDONLY},
	{"JBUT",&JBUT,D_INT|D_RDONLY},
	{"showpri",&showpri,D_INT,1},
	{"globalxres",&globalxres,D_INT},
	{"globalyres",&globalyres,D_INT},
	{"softwarexres",&softwarexres,D_INT},
	{"softwareyres",&softwareyres,D_INT},
	{"zoomin",&zoomin,D_ENUM,1,noyes},
	{"dofilelogger",&dofilelogger,D_ENUM,1,noyes},
	{"joyenable",&wininfo.joyenable,D_ENUM,1,noyes},
//	{"aliasmode",&aliasmode,D_ENUM,1,aliasmodes},
	{"defaultalphacutoff",&defaultalphacutoff,D_INT,1},
	{"hasmmx",&wininfo.hasmmx,D_ENUM,1,noyes},
	{"releasemode",&wininfo.releasemode,D_ENUM,1,noyes},
	{"maxtexlogu",&wininfo.maxtexlogu,D_INT,1,},
	{"maxtexlogv",&wininfo.maxtexlogv,D_INT,1,},
	{"startaudiomaindriver",&startaudiomaindriver,D_INT,1},
	{"startaudiosubdriver",&startaudiosubdriver,D_INT,1},
	{"mididisabled",&midi_disabled,D_INT,1},
	{"startvideomaindriver",&startvideomaindriver,D_INT,1},
	{"startvideosubdriver",&startvideosubdriver,D_INT,1},
	{"d3denhancedwanted",&d3denhancedwanted,D_INT,1},
	{"mipmapwanted",&mipmapwanted,D_INT,1},
	{"mipmaplodbias",&video_mipmaplodbias,D_FLOAT,FLOATUP/64},
	{"envmapoffsetv",&envmapoffsetv,D_FLOAT,FLOATUP/4},
	{"startusedirectinput",&startusedirectinput,D_ENUM,1,noyes},
	{"enabledebprint",&enabledebprint,D_ENUM,1,noyes},
	{"enablenumkeychangestate",&enablenumkeychangestate,D_ENUM,1,noyes},
	{"framestep",&framestep,D_FLOAT,FLOATUP/4},
	{"speedupdeformobjects",&speedupdeformobjects,D_ENUM,1,noyes},
//	{"trikind",&trikind,D_INT|D_RDONLY,1},
	{"startstate",&startstate,D_INT,0},
	{"compmhz",&compmhz,D_INT,1},
//	{"testtotal",&testtotal,D_INT,1},
//	{"interleavealways",&interleavealways,D_ENUM,1,noyes},
	{"dopaintmessages",&dopaintmessages,D_ENUM,1,noyes},
	{"timalpha",&timalpha,D_INT,1},
	{"",NULL,D_VOID,0},

	{"-------- FOG ----------",NULL,D_VOID,0},
	{"fogenable",&fogenable,D_ENUM,1,noyes},
	{"fogtype",&fogtype,D_ENUM,1,fogform},
	{"fogmethod",&fogmethod,D_ENUM,1,fogenum},
	{"fogcolorr",&fogcolor.x,D_FLOAT,FLOATUP/64},
	{"fogcolorg",&fogcolor.y,D_FLOAT,FLOATUP/64},
	{"fogcolorb",&fogcolor.z,D_FLOAT,FLOATUP/64},
	{"fogdensity",&fogdensity,D_FLOAT,FLOATUP/64},
	{"fogstart",&fogstart,D_FLOAT,FLOATUP/16384},
	{"fogend",&fogend,D_FLOAT,FLOATUP/16384},
	{"updaterenderstates",&updaterenderstates,D_ENUM,1,noyes},

	{"-------- NEW CAMERA ----------",NULL,D_VOID,0},
// enable
	{"newcamvars.usenewcam",&newcamvars.usenewcam,D_ENUM,1,noyes},
//	{"newcamvars.usesteeryaw",&newcamvars.usesteeryaw,D_ENUM,1,noyes},
//	{"newcamvars.usesteeryaw2",&newcamvars.usesteeryaw2,D_ENUM,1,noyes},
	{"newcamvars.usecoll",&newcamvars.usecoll,D_ENUM,1,noyes},
	{"newcamvars.colltries",&newcamvars.colltries,D_INT,1},
// settings
	{"newcamvars.steeryaw",&newcamvars.steeryaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.cpitch",&newcamvars.cpitch,D_FLOAT,FLOATUP/16},
	{"newcamvars.cyaw",&newcamvars.cyaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.pitchdrift",&newcamvars.pitchdrift,D_FLOAT,FLOATUP/16},
	{"newcamvars.yawdrift",&newcamvars.yawdrift,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryaw",&newcamvars.steeryaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.normalpitch",&newcamvars.normalpitch,D_FLOAT,FLOATUP/16},
//	{"newcamvars.normalyaw",&newcamvars.normalyaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.collyawrate",&newcamvars.collyawrate,D_FLOAT,FLOATUP/16},
	{"newcamvars.collpitchrate",&newcamvars.collpitchrate,D_FLOAT,FLOATUP/16},
	{"newcamvars.yawsafe",&newcamvars.yawsafe,D_FLOAT,FLOATUP/16},
	{"newcamvars.pitchsafe",&newcamvars.pitchsafe,D_FLOAT,FLOATUP/16},
	{"newcamvars.yawlimit",&newcamvars.yawlimit,D_FLOAT,FLOATUP/16},
	{"newcamvars.pitchlimit",&newcamvars.pitchlimit,D_FLOAT,FLOATUP/16},
	{"newcamvars.normalcentery",&newcamvars.normalcentery,D_FLOAT,FLOATUP/16},
	{"newcamvars.startcentery",&newcamvars.startcentery,D_FLOAT,FLOATUP/16},
	{"newcamvars.centerratey",&newcamvars.centerratey,D_FLOAT,FLOATUP/16},
	{"newcamvars.cheadrate",&newcamvars.cheadrate,D_FLOAT,FLOATUP/16},
	{"newcamvars.centery",&newcamvars.centery,D_FLOAT,FLOATUP/16},
	{"newcamvars.centerx",&newcamvars.centerx,D_FLOAT,FLOATUP/16},
	{"newcamvars.ccenterx",&newcamvars.ccenterx,D_FLOAT,FLOATUP/16},
	{"newcamvars.centerxdrift",&newcamvars.centerxdrift,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryawlimit",&newcamvars.steeryawlimit,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryawrate",&newcamvars.steeryawrate,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryawrate2",&newcamvars.steeryawrate2,D_FLOAT,FLOATUP/16},
//	{"newcamvars.leftcount",&newcamvars.leftcount,D_INT,1},
//	{"newcamvars.rightcount",&newcamvars.rightcount,D_INT,1},
// less raw
	{"newcamvars.pitch",&newcamvars.pitch,D_FLOAT,FLOATUP/16},
	{"newcamvars.yaw",&newcamvars.yaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.dist",&newcamvars.dist,D_FLOAT,FLOATUP/16},
//	{"newcamvars.centery",&newcamvars.centery,D_FLOAT,FLOATUP/16},
// raw
	{"newcamvars.zoom",&newcamvars.zoom,D_FLOAT,FLOATUP/16},
	{"newcamvars.pos.x",&newcamvars.pos.x,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.pos.y",&newcamvars.pos.y,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.pos.z",&newcamvars.pos.z,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.x",&newcamvars.rot.x,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.y",&newcamvars.rot.y,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.z",&newcamvars.rot.z,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.w",&newcamvars.rot.w,D_FLOAT|D_RDONLY,FLOATUP/16},

	{"-------CARENA-------",NULL,D_VOID,0},
	{"testheight",&testheight,D_FLOAT,FLOATUP/16},
	{"loadinggoal1",&olracecfg.loadinggoal1,D_INT,1},
//	{"scorelinekind",&olracecfg.scorelinekind,D_INT,1},
	{"tweakolwheels",&olracecfg.tweakolwheels,D_FLOAT,FLOATUP},
	{"tweakolwheelsu",&olracecfg.tweakolwheelsu,D_FLOAT,FLOATUP},
	{"tweakolwheelsv",&olracecfg.tweakolwheelsv,D_FLOAT,FLOATUP},
	{"beforeloadtimeoutclient",&olracecfg.beforeloadtimeoutclient,D_INT,1},
	{"beforeloadtimeoutserver",&olracecfg.beforeloadtimeoutserver,D_INT,1},
	{"carextracttimeout",&olracecfg.carextracttimeout,D_INT,1},
	{"afterloadtimeout",&olracecfg.afterloadtimeout,D_INT,1},
	{"gameplaytimeout",&olracecfg.gameplaytimeout,D_INT,1},
//	{"useudp",&olracecfg.useudp,D_INT,1},
	{"udpserversize",&olracecfg.udpserversize,D_INT,1},
	{"udpclientsize",&olracecfg.udpclientsize,D_INT,1},
	{"disabletcp",&olracecfg.tcpdisable,D_INT,1},
	{"enableudp",&olracecfg.udpenable,D_INT,1},
	{"crashresety",&olracecfg.crashresety,D_FLOAT,FLOATUP},
	{"forcebotmode",&olracecfg.forcebotmode,D_INT,1},
//	{"testkicknum",&olracecfg.testkicknum,D_INT,1},
	{"stubtracknum",&olracecfg.stubtracknum,D_INT,1},
	{"rules",&olracecfg.rules,D_INT,1},
	{"testint1",&od.testint1,D_INT,1},
	{"testint2",&od.testint2,D_INT,1},
	{"extractsleep",&olracecfg.extractsleep,D_INT,256},
	{"",NULL,D_VOID,0},

	{"-------TESTIMPVAL--",NULL,D_VOID,0},
	{"testimpval.x",&testimpval.x,D_FLOAT,FLOATUP/64},
	{"testimpval.y",&testimpval.y,D_FLOAT,FLOATUP/64},
	{"testimpval.z",&testimpval.z,D_FLOAT,FLOATUP/64},
	{"testimppnt.x",&testimppnt.x,D_FLOAT,FLOATUP/64},
	{"testimppnt.y",&testimppnt.y,D_FLOAT,FLOATUP/64},
	{"testimppnt.z",&testimppnt.z,D_FLOAT,FLOATUP/64},
	{"",NULL,D_VOID,0},
	{"-------DIRECTION FINDER--",NULL,D_VOID,0},
	{"gothiswayscalex",&olracecfg.gothiswayscale.x,D_FLOAT,FLOATUP/64},
	{"gothiswayscaley",&olracecfg.gothiswayscale.y,D_FLOAT,FLOATUP/64},
	{"gothiswayscalez",&olracecfg.gothiswayscale.z,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.x",&olracecfg.gothiswaypos.x,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.y",&olracecfg.gothiswaypos.y,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.z",&olracecfg.gothiswaypos.z,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.x",&olracecfg.gothiswayrot.x,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.y",&olracecfg.gothiswayrot.y,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.z",&olracecfg.gothiswayrot.z,D_FLOAT,FLOATUP/64}, 
	{"noweapsum",&olracecfg.noweapsum,D_INT,1},
	{"bluered3d",&olracecfg.bluered3d,D_FLOAT,FLOATUP/64},
	{"",NULL,D_VOID,0},

	{"-------- misc ------------------",NULL,D_VOID,0},
	{"slecs",&slecs,D_INT,1},
	{"repdelay",&repdelay,D_INT,1},
	{"repperiod",&repperiod,D_INT,1},
	{"streamaudiopos",&streamaudiopos,D_INT|D_RDONLY,1},
	{"runexclusive",&runexclusive,D_INT,1},
	{"mousemode",&mousemode,D_ENUM,1,mousemodestr},
	{"disablewpsetvol",&disablewpsetvol,D_ENUM,1,noyes},
	{"runinbackground",&runinbackground,D_ENUM,1,noyes},
	{"isrunning",&wininfo.isrunning,D_INT|D_RDONLY,1},
	{"updaterenderstates",&updaterenderstates,D_ENUM,1,noyes},
	{"perscorrect",&perscorrect,D_ENUM,1,noyesfrommodel},
	{"favorshading",&favorshading,D_ENUM,1,noyes},
//	{"usecvertsfromuv",&usecvertsfromuv,D_ENUM,1,noyes},
	{"zenable",&zenable,D_ENUM,1,noyesfrommodel},
//	{"softwarecolorkeyonly",&softwarecolorkeyonly,D_ENUM,1,noyes},
	{"hardwareclipping",&hardwareclipping,D_ENUM,1,noyes},
	{"texwrap",&texwrap,D_ENUM,1,noyesfrommodel},
	{"filter",&filter,D_ENUM,1,noyes},
	{"vsync",&vsync,D_ENUM,1,noyes},
	{"clearzbuffval",&clearzbuffval,D_FLOAT,FLOATUP/64},
	{"usealwaysfacing",&usealwaysfacing,D_ENUM,1,noyes},
	{"clipmap",&clipmap,D_ENUM,1,clipmapenumstr},
	{"smoverride",&smoverride,D_ENUM,1,smoothstr},
	{"showobjs",&showobjs,D_ENUM,1,noyes},
	{"shownulls",&shownulls,D_ENUM,1,noyes},
	{"showcameras",&showcameras,D_ENUM,1,noyes},
	{"showlights",&showlights,D_ENUM,1,noyes},
	{"showbones",&showbones,D_ENUM,1,noyes},
	{"restbones",&restbones,D_ENUM,1,noyes},
	{"usescnlights",&usescnlights,D_ENUM,1,noyes},
	{"usescnbones",&usescnbones,D_ENUM,1,noyes},
	{"usescncamera",&usescncamera,D_ENUM,1,noyes},
	{"uselights",&uselights,D_ENUM,1,noyes},
	{"fastnorms",&fastnorms,D_ENUM,1,noyes},
	{"dodefaultlights",&dodefaultlights,D_ENUM,1,noyes},
	{"helperscale",&helperscale,D_FLOAT,FLOATUP*4},
	{"manualframe",&manualframe,D_ENUM,1,noyes},
	{"framenum",&framenum,D_FLOAT,FLOATUP},
	{"useattachcam",&useattachcam,D_ENUM,1,noyes},
	{"usetargetcam",&usetargetcam,D_ENUM,1,noyes},
	{"flycamrevy",&flycamrevy,D_ENUM,1,noyes},

	{"---- 3d engine -----------------",NULL,D_VOID,0},
	{"flycamspeed",&flycamspeed,D_FLOAT,FLOATUP/4},
	{"vpcamtrans.x",&mainvp.camtrans.x,D_FLOAT,FLOATUP/4},
	{"vpcamtrans.y",&mainvp.camtrans.y,D_FLOAT,FLOATUP/4},
	{"vpcamtrans.z",&mainvp.camtrans.z,D_FLOAT,FLOATUP/4},
	{"backcolor.x",&mainvp.backcolor.x,D_FLOAT,FLOATUP/64},
	{"backcolor.y",&mainvp.backcolor.y,D_FLOAT,FLOATUP/64},
	{"backcolor.z",&mainvp.backcolor.z,D_FLOAT,FLOATUP/64},
	{"vpcamrot.x",&mainvp.camrot.x,D_FLOAT,FLOATUP/4},
	{"vpcamrot.y",&mainvp.camrot.y,D_FLOAT,FLOATUP/4},
	{"vpcamrot.z",&mainvp.camrot.z,D_FLOAT,FLOATUP/4},
	{"vpcamzoom",&mainvp.camzoom,D_FLOAT,FLOATUP/16},
	{"vpfront",&mainvp.zfront,D_FLOAT,FLOATUP/16},
	{"vpback",&mainvp.zback,D_FLOAT,FLOATUP/16},
/*	{"vpcamtrans2.x",&mainvp2.camtrans.x,D_FLOAT,FLOATUP/4},
	{"vpcamtrans2.y",&mainvp2.camtrans.y,D_FLOAT,FLOATUP/4},
	{"vpcamtrans2.z",&mainvp2.camtrans.z,D_FLOAT,FLOATUP/4},
	{"backcolor2.x",&mainvp2.backcolor.x,D_FLOAT,FLOATUP/64},
	{"backcolor2.y",&mainvp2.backcolor.y,D_FLOAT,FLOATUP/64},
	{"backcolor2.z",&mainvp2.backcolor.z,D_FLOAT,FLOATUP/64},
	{"vpcamrot2.x",&mainvp2.camrot.x,D_FLOAT,FLOATUP/4},
	{"vpcamrot2.y",&mainvp2.camrot.y,D_FLOAT,FLOATUP/4},
	{"vpcamrot2.z",&mainvp2.camrot.z,D_FLOAT,FLOATUP/4},
	{"vpcamzoom2",&mainvp2.camzoom,D_FLOAT,FLOATUP/16},
	{"vpfront2",&mainvp2.zfront,D_FLOAT,FLOATUP/16},
	{"vpback2",&mainvp2.zback,D_FLOAT,FLOATUP/16}, */
	{"bboxret",&bboxret,D_ENUM|D_RDONLY,3,clipret},
	{"---- shiny car -----------------",NULL,D_VOID,0},
	{"shinycarrot.x",&shinycarrot.x,D_FLOAT,FLOATUP/4},
	{"shinycarrot.y",&shinycarrot.y,D_FLOAT,FLOATUP/4},
	{"shinycarrot.z",&shinycarrot.z,D_FLOAT,FLOATUP/4},
	{"shinycartrans.x",&shinycartrans.x,D_FLOAT,FLOATUP/4},
	{"shinycartrans.y",&shinycartrans.y,D_FLOAT,FLOATUP/4},
	{"shinycartrans.z",&shinycartrans.z,D_FLOAT,FLOATUP/4},
	{"shinycarscale",&shinycarscale,D_FLOAT,FLOATUP/4},
	{"---- jrm car -----------------",NULL,D_VOID,0},
	{"jrmcarrot.x",&jrmcarrot.x,D_FLOAT,FLOATUP/4},
	{"jrmcarrot.y",&jrmcarrot.y,D_FLOAT,FLOATUP/4},
	{"jrmcarrot.z",&jrmcarrot.z,D_FLOAT,FLOATUP/4},
	{"jrmcartrans.x",&jrmcartrans.x,D_FLOAT,FLOATUP/4},
	{"jrmcartrans.y",&jrmcartrans.y,D_FLOAT,FLOATUP/4},
	{"jrmcartrans.z",&jrmcartrans.z,D_FLOAT,FLOATUP/4},
	{"jrmcarscale",&jrmcarscale,D_FLOAT,FLOATUP/4},
};

int ndebvars=sizeof(debvars)/sizeof(debvars[0]);

