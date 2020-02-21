void setfog(S32 fogslot);
////////////// function protos
void onlinerace_init(),onlinerace_proc(),onlinerace_draw3d(),onlinerace_exit();			// 2 ONLINE race track, carena connect
void carextractstate_init(),carextractstate_proc(),carextractstate_draw3d(),carextractstate_exit();	// carena car extract(sleextra=1)
void onlineracemain_init(),onlineracemain_proc(),onlineracemain_draw3d(),onlineracemain_exit(); // carena maingame
void ol_playatagsound(int tagidx,float vol,float pan);
void ol_douburst(int numbursts);
void ol_initfinishbursts();
//void ol_spawndirtparts1(),ol_spawndirtparts2();
//static void ol_slowdown(),ol_speedup(),ol_onice(),ol_quicksand();
void ol_usenewcam();
void ol_dowater();
void ol_dolava();
void ol_getpiece(pointf3* v,int *x,int *z);
void ol_docondom(int numcondoms);
//#define USEVARCHECKERtest
//#define USEVARCHECKERrace
// enums and defines
/////////// rules
//enum {RULE_NULL,RULE_NOWEAP,RULE_COMBAT,RULE_CTF,RULE_CTFW,RULE_STUNT,RULE_NOWEAPBOXES,RULE_COMBATBOXES,
//	NUMRULES};
struct rule {
	char *rulename;
	char *windowname;
	char *scorelinet;
	char *scorelineb;
	int useweap;
	int ctf;
	int rboxes;
};
enum {
	MED_FIRST=0,
	MED_DAMAGE=1,
	MED_COINS=2,
	MED_WRONGWAY2=3,
	MED_FIRSTNOWEAP=4,
	MED_TROPHY=5,
	MED_BEAT=6,
	MED_BEAT2=7,
	MED_MAXMEDALS=8,
};
void addmedal(S32 cs,S32 medkind);

extern struct rule rules[MAX_RULES];

//#define WEAPFRAME_DEPLOY 0
//#define WEAPFRAME_ACTIVATE 46
//#define WEAPFRAME_RESULT 82
//#define WEAPFRAME_RETRACT 167
//#define WEAPFRAME_DONE 214 // one past last frame

#define NMISSILES 4
//extern int missilefireframes[NMISSILES];

/*
enum {
	WEAP_LASER,
	WEAP_KLONDYKE,
	WEAP_MISSILES,
	WEAP_OIL,
	WEAP_EMB,
	WEAP_ECTO,
	WEAP_DIELECTRIC,
	WEAP_TRANSDUCER,
	WEAP_PLASMA,
	WEAP_RAMJET,
	WEAP_INTAKE,
	WEAP_SPIKES,
	WEAP_BAINITE,
	WEAP_AERODYNE,
	WEAP_FERRITE,
	WEAP_WEAPBOOST,
	WEAP_RPD,
	WEAP_NANO,
	WEAP_RANDOMIZER,
	WEAP_ICRMS,
// 20 (21!?) new weapons
	WEAP_BIGTIRES,
	WEAP_BUZZ,
//	WEAP_CHEMBOOST,
//	WEAP_CHEMVAC,
	WEAP_ENLEECH,
//	WEAP_ENUPTAKE,
//	WEAP_FSNBOO,
	WEAP_FSNCAN,
//	WEAP_GEOSHIELD,
	WEAP_GRAPPLE,
//	WEAP_HOLO,
	WEAP_MAGNET,
	WEAP_MINES,
//	WEAP_NITRO,
//	WEAP_NRGCON,
//	WEAP_POOLBOO,
	WEAP_POUNCER,
	WEAP_PRISM,
//	WEAP_SHLDBSTR,
	WEAP_SONIC,
//	WEAP_THERMCAN,

// 10 new weapons
	WEAP_SLEDGE,
	WEAP_ENERGYWALL,
	WEAP_STASIS,
	WEAP_SUPAJUMP,
	WEAP_FORKLIFT,
	WEAP_BUMPERCAR,
	WEAP_GYROSCOPE,
	WEAP_GRAVITYBOMB,
	WEAP_FLASHER,
	WEAP_CHARGER,
// 1 extra weapon
	WEAP_SWITCHAROO,

	MAXWEAPONSKIND
};
*/
#if 0
/*struct weapinfo {
	char *name,*longname,special;
	int shield;
	int persist;*/
	float /*deploystart,*/deployend;//,deployloop;
	float actstart,actend,actloop;
	float /*resstart,*/resend;//,resloop;
};*/
#endif
//#define MAXWEAPONSKIND 20
//extern struct weapinfo weapinfos[MAXWEAPONSKIND];
/*
enum {
	REGPOINT_BACK,
	REGPOINT_TIREBR,
	REGPOINT_BOTTOM,
	REGPOINT_TIREBL,
	REGPOINT_TIREFR,
	REGPOINT_TIREFL,
	REGPOINT_FRONT,
	REGPOINT_HOOD,
	REGPOINT_RIGHT,
	REGPOINT_ROOF,
	REGPOINT_LEFT,
	REGPOINT_TRUNK,
	REGPOINT_NREGPOINTS,
};
*/
struct detachweapinfo {
	char *name;
	void (*initfunc)(struct weapfly *wf);
	void (*procfunc)(struct weapfly *wf);
	tree2* mastertree;
	int ntreepool;
//	tree2* *treepool;
	vector<tree2*> treepool;
	pointf3* facenorms;	// for colltree
};

enum {
	DETACH_LASER,
	DETACH_MISSILES,
	DETACH_OIL,
	DETACH_EMB,
	DETACH_ECTO,
//	DETACH_ECTO2,
	DETACH_MINE,
	DETACH_FLAG,

	DETACH_GHOOK,
	DETACH_FUSIONCAN,
	DETACH_THERMOCAN,
	DETACH_SONIC,

	DETACH_GBOMB,
	DETACH_STASIS,
	DETACH_WALL,
	DETACH_RAMP,
	DETACH_MINIGUN,
	DETACH_PARTICLE,

	MAXDETACHKIND
};


#define USECON
//#define TESTSCALEUP 1.0f	// hey, here's an easy way to scale up the pieces!!
//#define PIECESIZEX (10.0f*TESTSCALEUP) // used for start,finish,crashresetloc
//#define PIECESIZEZ (10.0f*TESTSCALEUP)
#define ARTPIECESIZEX 10.0f
#define ARTPIECESIZEZ 10.0f

#define OL_MAXPLAYERS 16
#define OL_MAXCARSLOTS 16
#define MAXAISTUNTS 2

#define NUMOBJECTS 1

#define NSHOCKPOINTS 8
#define NRECTPOINTS 8
#define NMIDPOINTS 2

#define SHOCKPOINTOFF 0
#define RECTPOINTOFF (SHOCKPOINTOFF+NSHOCKPOINTS)
#define MIDPOINTOFF (RECTPOINTOFF+NRECTPOINTS)

#define NCARPOINTS (MIDPOINTOFF+NMIDPOINTS) 
#define NWORKINGSHOCKS 4

//enum {CNTL_HUMAN,CNTL_AI,CNTL_NONE};
enum {CARCOAST,CARBRAKE,CARACCEL,CARREVERSEACCEL};

//#define COLBONESX 2
//#define COLBONESZ 3

//#define NUMBODS 49 // as defined in editor.h, number of grid pieces
//#define NUMCOINS 10

#define CARREV0 19
#define CARREV1 20
#define CARREV2 21
#define CARREV3 22
#define CARREV4 23
#define CARREV5 24
#define CARREV6 25
#define CARREV7 26
#define CARREV8 27
#define CARREV9 28
#define CARREV10 29
#define CARREV11 30
#define CARREV12 31

#define NUMTVCAMS 20

#define USCALEDOWN 100.0f

enum {FRONTLEFT,FRONTRIGHT,BACKLEFT,BACKRIGHT,MIDDLELEFT,MIDDLERIGHT};

#define MAXSTEER 45

enum {	PTEXT_DIFFTIME=2,
	PTEXT_TRICKS=3,
	PTEXT_WRONGWAY=4,
//	PTEXT_FINISHTIME,
//	PTEXT_PLACE,
	PTEXT_TEST=10,
	PTEXT_PING=11,
	PTEXT_MAXTEXTSTRINGS=12};

#define MAXTS 200
#define MAXTS2 250

// const struct data
//struct ol_dembonec {
//	char *name;
//	pointf3 dampushin;
//};
//extern struct ol_dembonec ol_dembonesc[COLBONESX][COLBONESZ];

typedef struct {
  tree2* acar;
  tree2* ashad;
  char carname[32]; // car scene to load
  char spname[32]; // blue lightning model name in car scene
  char shadowname[32]; // shadow model name
  char tspname[32]; // name of tsp to load onto texture of carbody model, flash
  char mainobject[32]; // carbody model
  char asurfname[32]; // surface name of carbody model
  char clpname[32]; // not used
  char coinsp[32];	// golden condom
  char atexsets;
  char alocked;
  char arevsound;
} OL_CARLIST;
extern OL_CARLIST ol_allcars[];
//// sub structs
//struct OL_COIN {
//	tree2* t;
//	pointf3 pos;
//};

//struct OL_COINPRIORITY {
//	int priority;
//	char name[32];
//};

// track data
/*typedef struct {
  tree2* t;
  int piece;
  int lpiece;
  int previous;
//  int lor;
  int or;
//  int bodyid;
  float flor;
  int curpiece;
  int add;
} OL_TRACK;

typedef struct {
  char name[64]; // used by uplay3
  tree2* piece;
  int changerot;
  int newor;
  int time;
  int value;
  int ovalue;
} OL_PIECE;
*/
struct timespace { // marks for determining relative time
	int time;
	float space;
	int ncarspassed;
};

#define MAXRECINPUT 8192 //2048 // 32768 // 512, a power of 2
struct keypack {
	char key;
	char weapkey;
};

struct playern {
	struct socker *tcpclients;
//	struct socker *alttcpclients; // reverse server / client for firewall's
//	int sockvalidmask;
//	struct socker *udpclients;
	int udpip;
	int udpport;
	int retrytimers;
// send
//	int sendinputs[MAXSENDINPUT];
//	int tsinputs[MAXSENDINPUT];
//	int nsendinput;
//	int lastinput;
//	int input;
//	int timestamp;
// recieve
//	int ninput;
//	int linput;
	struct keypack inputs[MAXRECINPUT];
//	unsigned short timestamps[MAXRECINPUT];
	int offset;
	int ninput;
	int tsoff;
	int onebotplayer;	// a disconnected player
//	int earlyinput;

};

struct weapfly {
// 000
	pointf3 pos,rot,scale,vel;
	int active,fuel;
	float speed;
	float fuser0;
	int user0;
	float dissolve;
//	tree2* t; // malloc / free not supported in time warp (maybe some day!)
	int kind; // tree type, what sub tree pool to use
	int hitcar;
	int fromcar;
	int flags;
//	int alwaysfacing;
	void (*proc)(struct weapfly *);
};

#define MAXWEAPFLY 200 // 5 - 20 for debug, 200 for normal
//#define MAXJPLACE 4
#define WSOUND_DEPLOY 0
#define WSOUND_ACTIVATE 1
#define WSOUND_RESULT 2

//#define NUMFLAGS 1
#define NUMFLAGS 10

#define NAMESIZEI 32

#define MAXCOINS 10
//// global data
struct ol_data {
//	script* weapxlate;
	U32 medalshave[MED_MAXMEDALS]; // we already have these medals, don't post if got medals again
	particle* parts;
	int numcoins;
	tree2* cointrees[MAXCOINS];
	bool coins_caught[MAXCOINS];
	float coincoldist;
	int numtrophies;
	tree2* trophytrees[MAXCOINS];
	bool trophies_caught[MAXCOINS];
	int numsuperenergies;
	tree2* superenergytrees[MAXCOINS];
	bool superenergies_caught[MAXCOINS];
	bool badtrack;
	wavehandle* backwave;
	soundhandle* backsound;
	float y2009carscale;
	pointf3 y2009carrot;
	pointf3 y2009cartrans1,y2009cartrans2;
	float aiturnratio,aiturndeadzone;
	S32 boxaimaxwatchdog;
	fontq* hugefont;
	int allbotgame,norematch;
	float speedupaccel,speedupspeed;
	int gyroscopeairtime; // 50 # how long in air to activate gyro
	float gyroscopeline2road; // 1 # how close to road before activating gyro
	pointf3 gyroturnrpy; // how fast to turn gyro

	float underbigtiredown1,underbigtiredown2,underbigtireacross;
	textureb *minetex;
	int minearmed,lastminearmed;
	C32 minearmcolor,minedisarmcolor;
	struct bitmap32 *blueredbm;
	struct bitmap32 *cartexsave[OL_MAXCARSLOTS],*cartexsave2[OL_MAXCARSLOTS];
	struct bitmap32 *cardarktexsave[OL_MAXCARSLOTS],*cardarktexsave2[OL_MAXCARSLOTS];
	textureb *cartex[OL_MAXCARSLOTS],*cartex2[OL_MAXCARSLOTS];
	int cartexstate[OL_MAXCARSLOTS],cartexstate2[OL_MAXCARSLOTS]; // 0 normal
//	int ol_curplayer; // used to process current player
//	int tempdisablekillwait;
	tree2* gothisway;
// debug
	int blitmode;
	float showpathframe;
	int selpath; // 0 for path1, 1 for path2, 
	int showrabbits;
	int path2road;
	int showcrashresetloc;
//
	int uplaycursor;
	int nobotrematch; // if a 1, then bots don't want to rematch
	int numdiscon; // how many players left
//	struct bitmap32 *jplacebm[MAXJPLACE];//,*jplacesave,*weapdescsave;
//	textureb* jplacespt[MAXJPLACE]; // sprite version of jplacebm
	textureb* carrow;	// dir finder arrow
	textureb* cface;	// dir finder clock face
	struct bitmap32 *weapbm;
	int jplacetimer,jplace;
	int wrongway;
	int slowleadmask,slowleadpower,disablespace,wayleadpower;
	int boxaislowleadmask,boxaislowleadpower;//,disablespace,wayleadpower;
	int boxaislowleadmask2,boxaislowleadpower2;//,disablespace,wayleadpower;
	float boxaislowleadrat;
	float wayleadframediff;
	int defuloop;
//	int ol_numplayers;	// number of players, actually number of cars..
	int notspec;		// if not a spectator

	struct {
		tree2* t;
		pointf3 pos;
	} ol_flags[NUMFLAGS];
	int ol_flagidx;

	int ol_ustarttime;
	float ol_camdrift;
	float ol_camrotdrift;
	float ol_extracamang;
	pointf3 ol_camstart;
	pointf3 ol_camtrans;
	float ol_thecamzoom;
	float ol_camtween,ol_camtweenspeed;
	float ol_pitchthreshup;
	float ol_rollthreshup;
	float ol_pitchthreshdown;
	float ol_rollthreshdown;
	pointf3 ol_newpitch;
	float ol_camcoldist;
	int camlandtime;
//	int ol_notimelimit;
	int ol_stunttime;
//	int ol_uplayrevvolume;
//	pointf3 ol_scrlinepos;
//	pointf3 ol_scrlinescl;

	tree2* ol_root;
	int ol_slowpo;
//	tree2* ol_nofinishpiece;
	int ol_numpiecestovisit;
	int ol_finishpercent;
	int ol_pieceorder[50];
	int ol_ulastpiece,ol_st2_lastpiece,ol_udirpiece,ol_st2_dir;
	tree2* ol_startpiece,*ol_finishpiece;
	int ol_particleeffects;
	int ol_uloop;
	int ol_dirtq;
	tree2* udpart[2];
	float ol_timeinc,ol_timeincconst;
	int ol_sub_dirtq;
	tree2* udirtpart[2],*udarkdirtpart[2];
	int ol_sub_dirtvrandx;
	int ol_sub_dirtvrandy;
	int ol_sub_dirtvrandz;
	float ol_dirtscale;
	float ol_sub_dirtscale;
//	tree2* ol_udarkdirt;
	float ol_darkdirtscale;
	float ol_sub_darkdirtscale;
	int ol_timetocomplete;
	int ol_st2_curpiece;
	int ol_trackworth;
//	int ol_releasemode;
//	int ol_usehires;
	int ol_useoldcam;
//	int ol_framecount;
	struct wavehandle *ol_sag;
	float ol_zback;
	int ol_numpieces;
//	char ol_uedittrackname[256];
	char ntrackname[256];
//	char ol_skyboxname[256];
	tree2* ol_camnull; // drive this camera, the actual camera
	pointf3 camnullrot;
	int ol_numwaves;
	struct tag *ol_gardentags,*ol_weapontags[3];
//	tree2* ol_scrline,*tlight,*tlight2;
	tree2* ol_scrlinescene,*ol_scrlinet,*ol_scrlineb;
	int lightstate;
	int ol_oldtt/*,ol_oldncoins*/,ol_oldspeed;
	struct tsp *tlighttsp,*ol_clocktsp;//;//,*ol_speedotsp;
	int texfmt,texfmtnck;
	int fromtex,totex;
	textureb *tlighttex[4];
//	struct texture *ol_numcointex;
	pointf3 ol_qcamdesired,ol_camdesired;
	pointf3 ol_testcam,ol_testcar;
	int ol_testnewcam;
	tree2* ol_fwork[4],*ol_finishpiecenull;
	int ol_tx;
	float ol_shadyoffset;
	float ol_shadyoffset2;
	int ol_manualcar;
	int ol_trackscore;
	float ol_finishy;
	int ol_curwave;
	int ol_selectedtrack;

	struct tsp *ol_condomtsp,*ol_bursttsp[4];
	struct tsp *ol_specialtsp;
	struct tsp *ol_flametsp;

	S32 trackidx;
	float ntrackscale,trackstart,trackend;
	float piecesizex,piecesizez;
	float lowpoint,hipoint;	// too low crashreset, cars are reset at this relative height
//////// paths and ai
	tree2* path1,*path2;
//	float coinscale;
	int laps;
	int place,fplace;	// 0 to numplayers-1
// font stuff
//	int fontopt;
	struct tsp *ol_tscorefont1; // for 2d blits, nice fixed size letters
	struct tsp *ol_tscorefont2; // copy of variable width letters, but for blitting to startline
	struct tsp *ol_tscorefont3; // for 2d blits, variable width letters, for names
	struct tsp *ol_tscorefont4; // copy of variable width letters, but for blitting to scoreline//;weapicon
//	struct tsp *ol_tscorefont5; // copy of variable width letters, but for blitting to speedo
//	char finishnames[OL_MAXCARSLOTS][NAMESIZE];
	struct bitmap32 *oldstarttex;
	textureb *starttex;
	float startcamrotx;
	char scorestrings[PTEXT_MAXTEXTSTRINGS][NAMESIZEI]; // max 9 chars in cscorestring
	float cscoret[PTEXT_MAXTEXTSTRINGS];
	int tscoret[PTEXT_MAXTEXTSTRINGS];
	struct timespace ts[MAXTS2];	// 10 laps max..
	struct timespace curts;
	float nextspace;
	float stepspace;
	int numts;
	int signalfirst;	// signal first place car with the time..
	int bigmove;	// camera or car took a big move, 0 means valid vel
	pointf3 lastcamposdp;	// where camera is at in world (including viewport settings)
	pointf3 camposdp;	// where camera is at in world (including viewport settings)
	pointf3 camveldp;	// how fast cam is moving
	float speedosound;
	float pathframestep,pathclosedist,pathmeddist,pathfardist,pathvfardist,turnthresh;
	float pathfardistplace;
	int maxwatchdog;
	float pathbotresetadd;
	float voldist;
	float volcutoff;
	pointf3 tvcams[NUMTVCAMS];
	pointf3 tvcamsq[NUMTVCAMS]; // rot
	int tvcamnum;
	int tvcammode; // on or off
	int tvcampan; // target or off
	int pingmode,difftimemode;
// network stuff
	int keepsockets,goingtogame;
//	int showcon;
//	struct con16 *ucon;
	int playerid;//,carid;	// who you are
	struct socker *master;//,*altmaster;
	int myip;
	int numplayers,numcurplayers;	// how many connections (normally 2 for 2 player game) more for spectators
	int numcars;
	int numbots; // num extra bots nbots+nplayers=ncars
//	int playersmask;
//	int curplayersmask,altcurplayersmask;
	struct playern playernet[OL_MAXPLAYERS];
	struct socker *udp;
//	struct keypackbuffer kpb; // queued keystrokes to send on packetwrite
//	int nretries;
	int lastinput;
//	int framerate;
	int intimewarp,predicted;
	int speedup;
	float c2celast;
	int markfordisconnect;//,kickaplayer;
	int lastmillisec;
	int quittimer,quittime;	// when all players finished..
//	float newcaroffy,newcarscale;
	float ol_uplaycarscale;
//	int dorematch;
	int gamenum;		// for rematches
	int sentrematch;
	int resvis;
	int rematchplayers; // server only..
	int spacebartimer; // no cheating!
//	int opponenttime;
//	char opponentname[64];
	int loadingcount; // for file loads
//	struct bitmap32 /**loadingcar,*/*loadingbackground,*awaitconnect,*cursorpic;
	textureb* awaitconnect;
	textureb* loadingbackground;
//	,*weapicons[MAXWEAPONSKIND];
	int quitting;
// missile art
	tree2* mtrail,*chemtrail,*stasisfield;
	float mtrailscale,chemtrailscale,chemtrailback,chemtraildown;
// missiles
	int missilestartfuel;
	float missilevel;
//	float missileseekangle;
	float missileturnangle;
	float missilestartupangle;
	float missileimpactforce,missileupforce;
// emb
	int embstartfuel;
	float embvel;
//	float embseekangle;
	float embturnangle;
	float embstartupangle;
	float embimpactforce;
	float embrad;
// laser
	pointf3 laseroffset;
	int laserstartfuel;
	float laservel;
//	float laserseekangle;
//	float laserturnangle;
	float laserstartupangle;
	float laserimpactforce,laserupforce,laserextrabounce;
// fusion
	pointf3 fusionoffset;
	int fusionstartfuel;
	float fusionstartupangle;
	float fusionvel;
	float fusionimpactforce,fusionupforce;
	pointf3 fusionrotvel;
	float fusionscalevel;
	float fusionrad;
	float fusionminradscan,fusionmaxradscan,fusiongrabrad,fusionmaxscale;
	float fusionmorphmax,fusionmorphmin;
	int fusionmorphframe;
	float fusiongrav;
// thermo
	pointf3 thermooffset;
	int thermostartfuel;
	float thermostartupangle;
	float thermovel;
	float thermoimpactforce,thermoupforce;
	pointf3 thermorotvel;
	float thermoscalevel;
	float thermorad;
// sonic
	pointf3 sonicoffset;
	int sonicstartfuel;
	float sonicstartupangle;
	float sonicvel;
	float sonicimpactforce,sonicupforce;
	pointf3 sonicrotvel;
	float sonicscalevel;
	float sonicrad1,sonicrad2;
	float sonicspincar,sonicheight;
// grapple
#define GRAPPLETIME 7
	pointf3 grappleoffset;
	int grapplestartfuel;
	float grapplestartupangle;
	float grapplevel;
	float grappleimpactforce;
	float grapplerad;
// magnet
	float magnetrad;
// flasher
	float testflasher;
	float flashermindistance,flashermaxdistance,flasherminstrength,flashermaxstrength;
	tree2* flasherobj;
	pointf3 flashercolor;
// bumper
	float bumperkickframe;
	int bumpershowbox;
//	pointf3 buzzboxmin;
//	pointf3 buzzboxmax;
//	float buzzkickframe;
	float bumperkickamount;
	pointf3 bumper0boxmin,bumper0boxmax;
	float /*buzzleft,*/bumpersizex,bumpersizez,bumpersizey,bumperup;
//	float buzzspincar;
// extra handling settings
	float extrabainitetraction; // less slides
	float extraramjettopspeed,extrachargertopspeed; // faster top end
	float chargerfastframe;
	float extrachemboosttopspeed;
	float extraaerodynetopspeed; // faster top end
	float extranitrotopspeed;
	float extraintakeaccel; // get to top speed quicker
	float extrafsnbooaccel;
//	float extrafly; // try less gravity in air
// reg point rot
	pointf3 regrot;
	int regnum;	// for finding out if numerical order of regpoints in .jrm is consistent...
	int defaultcntl;
// icrms
//	int enableicrms;
//	float icrmsparm;
	int icrmsduration;
	float icrmsslowrate;
	float icrmsdistance,icrmsfreezeframe;
// klondyke
	int klondykeshowbox;
//	pointf3 klondykeboxmin;
//	pointf3 klondykeboxmax;
	float klondykekickframe;
	float klondykekickamount;
	pointf3 klondyke0boxmin,klondyke0boxmax;
	pointf3 klondyke1boxmin,klondyke1boxmax;
// oil
	float oildist,oilback,oildown,oilspin,oilspinspeed,oilvel,oilymin,oilymax;
	int oilstartfuel;
	float oilstopframe;
// ecto
	float ectoparm,ectorad,ectograv,ectoscale;
	pointf3 ectofvel,ectocvel,ectorvel;
	int ectostartfuel;
	float ectoup,ectoframe;
// ferrite
	float ferritefric; // .995	# slow down
	float ferritexzaccel; //1	# movement
	float ferriteyvelthresh; // .05 # how fast must fall before ferrite kicks in
	float ferriteypos; // .8
	float ferriteyposboost; // 3 # with weapboost on
	float ferriteyaccel; // .4 #
	float ferritemaxspeed;
//	plasma
	float plasmatime;
// rpd
	float rpdstealframe,rpddistance;
// new weapons (21)
// mine
	float minedist,minevel,mineymin,mineymax;
	float mineupforce,mineback,minedown;
	int minearmtime;
// chemvac
	float chemvacdistance;
// enleech
	float enleechdistance;
// hologram
	float holofront;
// prism
	float prismup;
// bigtires
	float bigtirescale;
	float bigtireheight;
	float bigtireuprate;
	float bigtiredownrate;
// pouncer
	float pouncerheight;
	float pounceruprate;
//	float pouncerdownrate;
	float pouncerframe;
// buzz
	float buzzkickframe;
	int buzzshowbox;
//	pointf3 buzzboxmin;
//	pointf3 buzzboxmax;
//	float buzzkickframe;
	float buzzkickamount;
	pointf3 buzz0boxmin,buzz0boxmax;
	float /*buzzleft,*/buzzsizex,buzzsizez,buzzsizey,buzzup;
	float buzzspincar;
// forklift
	int forkshowbox;
	float forkkickamount;
	float forkkickframe;
	pointf3 fork0boxmin,fork0boxmax;
// sledge
	float sledgehitframe,sledgerad;
// gravitybomb
	int gbombstartfuel;
	float gbombup,gbombframe;
	pointf3 gbombfvel,gbombrvel;
	float gbombscale,gbombrad,gbombgrav;
	float gbombstrength,gbombminrad,gbombdamp;
// stasis
	int stasisstartfuel;
	float stasisup,stasisframe;
	pointf3 stasisfvel,stasisrvel;
	float stasisscale,stasisrad,stasisgrav;
// wall
	int wallstartfuel;
	float wallup,wallframe;
	pointf3 wallfvel,wallrvel;
	float wallscale,wallrad,wallgrav;
// ramp
	int rampstartfuel;
	float rampup,rampfront,rampframe;
	pointf3 rampfvel,ramprvel;
	float rampscale,ramprad,rampgrav;
// switcharoo
	float switcharooframe,switcharoorad;

// generic flying weapons (missiles,ecto,emb,oil,laser)
	struct weapfly weapflys[MAXWEAPFLY]; // time warp
//	tree2* weapflytrees[MAXWEAPFLY]; // no time warp
//	int weapflylastkind[MAXWEAPFLY]; // no time warp
// new concept, a tree pool of popular objects for time warp to handle
//	tree2* *treepool[MAXDETACHKIND];
//	int ntreepool[MAXDETACHKIND];
//	char **freeweaplist;
//	int nfreeweaplist;
//	char **detachweaplist;
//	int ndetachweaplist;
	script* freeweaplist;
	script* detachweaplist;
	int fountainon;
// energy values
//	int lastenergies[MAXENERGIES]; // sync up textures that changed
//	struct texture *energytex[MAXENERGIES];
// weapon icons
	int curweapicon,prevweapicon,nextweapicon;
	int lastcurweapicon,lastprevweapicon,lastnextweapicon;
	char curweapstring[50],lastweapstring[50];
	struct tsp *weapicontsp;
	struct texture *curweaptex,*energiestex,*speedotex,*clocktex,*flagstex,*radartex;//,*prevweaptex,*nextweaptex;
	struct bitmap16 *weapactivesave,*energiessave,*speedobar,*clocksave,*flagssave,*radarsave;
// energy matrix..
//	int energymatrix[MAXWEAPONSKIND][MAXENERGIES];
//	int energymatrixused[MAXWEAPONSKIND];
// 'place' texture
//	struct texture *splacetex,*weapdesctex;
// test ints
	int testint1,testint2;
// weapscale
	float weapscale,weapscaleregpoint;
// prism
	tree2* prism;
};
//enum {ENERGY_FUSION,ENERGY_ELECTRICAL,ENERGY_CYBER,ENERGY_GEOTHERMAL,ENERGY_PARTICLE,ENERGY_CHEMICAL};
enum {WEAPSTATE_DEPLOY,WEAPSTATE_ACTIVE,WEAPSTATE_RESULT}; //,WEAPSTATE_STOLEN};

struct pinput {
	int ol_uup;
	int ol_udown;
	int ol_uright,ol_uleft;
	int ol_urollleft;
	int ol_urollright;
	int uspace;
	int weapkey;	// 1 thru 12 for f1-f12
};

// old method of projectiles, keep until new method (see weapfly) works
/*struct missi {
	pointf3 pos,vel,rot;
	int active,fuel;
	tree2* t;//,*oldt;
//	int flags;
	int hitcar;
	int fromcar;
}; */

extern listbox* GAMENEWS;

// car slot data
struct ol_playerdata {
	int medals[MED_MAXMEDALS];
	int woc;
// boxai 2009
	struct chkbox* ol_crashresetbox;
// type
// cosmetic
//	int color;
// position of cm
	pointf3 pos;
//	pointf3 oldpos; // for missiles seeking lower numbered cars
// shape of object
//	pointf3 len; // lenx is also radius
// total mass
	float mass; // mass of 0 is infinite
	pointf3 carvel; // units per sec
// rotation
	pointf3 carang; //...... convert to quat, isa quat
	pointf3 carangsave;	// when in air for the camera
//	pointf3 oldcarang; // see oldpos
// time related
	pointf3 carvelang;	//rads per frame, about cm isa rotaxis
// friction
//	float cof; // contact friction coeff
/////////// derived points
	float moi;	// moment of inertia about cm, keep simple for now (i.e. not a tensor)
	float shocklen[6];
	int wrongway2;	// for the wrongway medal
	int ncoins_caught;
	int ntrophies_caught;
	int nsuperenergies_caught;
//	int startroad,endroad;	// for smartroad
// drawing and collision points
	pointf3 p[NCARPOINTS];
	pointf3 pr[NCARPOINTS];
// wheel spin
	float wheelang[6];
	float wheelvel[6];
	float wheelyaw;
	float steering;
// bounding box
	pointf3 bb0,bb1;
//	int nograv;
	int ol_magnatraction;
	float ol_shockspacingx;
	float ol_shockspacingz;
	float ol_shockoffsety;
	float ol_normalshocklen[4];
	float extrashocklen[4];
	float shockextra; // for big tires, multiplies into normalshocklen
	int extrabigtires,extrapouncer;
	int speedup,slowdown,onice,quicksand;
	pointf3 ol_carbboxmin,ol_carbboxmax;
	int ol_lastairtime;
	int ol_airtime,ol_cbairtime;
	float ol_shockstr,ol_shockdamp;
	float ol_elast;
	float ol_airfric,ol_littleg;
	int ol_validroadnorm;
	float ol_wheelrad;
	pointf3 ol_carnorm;
	pointf3 ol_cardir;
	pointf3 ol_carvelnorm;
	float ol_carvelmag;
	float ol_shockmag;
	pointf3 ol_shocknorm;
	pointf3 ol_lastroadnorm;
	float ol_accelspin;
	int ol_drivemode;
	float ol_fricttireslide;
	float ol_fricttireroll;
	float ol_startaccelspin,ol_wheelaccel;
	int ol_hifrictime;
	float ol_frictcarbody,ol_frictcarbodyroof;
	pointf3 ol_ulastloc;
	float ol_carbodymag;
	int ol_ufliptime,ol_doacrashreset,ol_flymode;

//	tree2* db_t[COLBONESX][COLBONESZ]; // dembones
//	pointf3 db_curpushin[COLBONESX][COLBONESZ];

	int ol_utotalcrashes,ol_utotalstunts;//,ol_ucoinscaught;

	int ol_startstunt;
	pointf3 ol_carstuntrpy;
	int ol_steertime;
	float ol_uextraheading,ol_defuextraheading;
	float ol_maxuextraheading;
	int ol_cantdrive;//,ol_lastkludge,;
	pointf3 ol_crashresetloc;
	pointf3 ol_crashresetrot;
	float ol_crashresetframe;
//	float ol_crashresetdir;
	int ol_uready;

//	int ol_ncoins,ol_ncoinscaught; // for now keep in global
	struct soundhandle *ol_motorsh;
//	int ol_selectedcar;
	float ol_defaultjumpfriction;
	int ol_dirtvrandx;
	int ol_dirtvrandy;
	int ol_dirtvrandz;
	float ol_sub_dirtvbx;
	float ol_sub_dirtvby;
	float ol_sub_dirtvbz;
	int ol_darkdirtq;
	float ol_darkdirtvbx;
	int ol_darkdirtvrandx;
	float ol_darkdirtvby;
	int ol_darkdirtvrandy;
	float ol_darkdirtvbz;
	int ol_darkdirtvrandz;
	int ol_sub_darkdirtq;
	float ol_sub_darkdirtvbx;
	int ol_sub_darkdirtvrandx;
	float ol_sub_darkdirtvby;
	int ol_sub_darkdirtvrandy;
	float ol_sub_darkdirtvbz;
	int ol_sub_darkdirtvrandz;
	float ol_debmass;
	float ol_debmoi;
	float ol_littlegground;
	float ol_littlegair;
	float ol_steervel;
	float ol_steervelback;
	float ol_wheelwid;
	float ol_wheellen;
	float ol_shocklimit;
	float ol_caroffsetx;
	float ol_caroffsety;
	float ol_caroffsetz;
	float ol_uplaywheelscale;
	float ol_uplayshadowscale;
	int ol_nubursts;
	int ol_nuflashes;
	int ol_nucondoms;
	int ol_nuflames;
	int ol_nulightnings;
	float ol_revfreqmul;
	float ol_revfreqadd;
	tree2* ol_carnull,*ol_carcenternull,*ol_carbody;
//	tree2* ol_uaxles[4];
//	tree2* ol_uplayspecial1; // lightning
//	tree2* ol_uplayspecial2; // condom
	tree2* ol_uplayflame1,*ol_uplayflame2;
//	tree2* ol_ubursta,*ol_uburstb;
	int ol_burstframe,ol_flameframe,ol_flashframe,ol_lightningframe,ol_condomframe;
	tree2* ol_wheels[6],*ol_wheels2[6];
	tree2* ol_shadownull,*ol_shadowb,*ol_shadowbody;
	float ol_uthiscarpitch,ol_uthiscarroll,ol_uthiscaryaw;
	int ol_piecesvisited[50];
	int ol_numpiecesvisited;
	int ol_intunnels;
	int ol_curpieceorder;
	int ccc,ccc2,ddd2;
	float ol_dirtvbx;
	float ol_dirtvby;
	float ol_dirtvbz;

//	int ol_grandtotaltrickpoints;
	int cantstartdrive;
	pointf3 ol_ulastpos;
	int ol_carboost;
	pointf3 ol_landspot;
	int ol_uplayrevfreq;
	int ol_rollpie[4],ol_pitchpie[4],ol_yawpie[4],ol_rollpiesum,ol_pitchpiesum,ol_yawpiesum;
	int ol_cantaddroll,ol_cantaddpitch,ol_cantaddyaw;
	int ol_udidroll,ol_udidyaw,ol_udidpitch;
	struct pinput pi;
	struct chkbox *boxchk;
	float aiturnval; // used by boxai
	int espace;	// extra space
//	int ol_lup,ol_ldown;
	pointf3 ol_ucarrotvel;
	int ol_ttp; // total trick points
	float ol_ufireworkscale;
	int dofinish;
//	int ol_totalcrashs;//,ol_MyGoldCoins;

//	textureb *bursttex,*flametex,*flashtex,*condomtex,*lightningtex;
//	struct tsp *ol_flashtsp;
	int cntl; // human, ai, network
	int playerid,carid,cartype; // what player drives this car..
//////// paths and ai
//	tree2* path1;
	float /*crashframe,*/seekframe;//,botbigscan;
	float seekframe2;
	int laps2;
	int chkpnt;
	int chklap;
//////// stunt ai
	pointf3 stunttimes[MAXAISTUNTS]; // time needed to do pitch,yaw,roll stunts
	pointf3 stuntgoals; // what angles stiving for
	pointf3 curstuntangs; // current angle (no normalized) of stunt
	int doaistunt[3];
//	int lapready;	// close to doing a lap, now close to finish line
//	int lapsdone;
	int finplace; // 1 2 3 etc.
	int cartspassed;	// which marker to look for
	pointf3 carveldp;		// vector of car vel, if od.bigmove then not valid
	float dpf;			// doppler factor
	float vold;			// volume from distance
	float pan;
	int stuckwatchdog;
	int clocktickcount;
//	int isnewcar;
//	pointf3 c2cimpval,c2cimppnt; // for car2car
//	int c2chit; // boolean, are we hitting any cars?
	pointf3 c2cpnts[8];
	float dh;	// direction to path object
	float ch;	// car heading
	tree2* onlinecar,*y2009car;//,*shinycar;
	tree2* regpoints,*theregpoints;
	pointf3 regpointsoffsetjrm[REGPOINT_NREGPOINTS];
//	pointf3 regpointsoffsetshiny[REGPOINT_NREGPOINTS];
	pointf3* regpointsoffset;//[REGPOINT_NREGPOINTS];
// weapons
//	int xcurweapkind,lastactiveweap,curweapvar;//lastcurweap;
//	float xweapframe;
	tree2* weaptrees[MAXWEAPONSCARRY];
	tree2* bigtiretrees[4];
//	bool (*bigtireproc)(tree2 *); // animation proc save for tires..

// thief side
//	unsigned char ttweapstolenfrom;//[MAXWEAPONSCARRY];
//	unsigned char ttweapstolenslot;
// victim side
//	unsigned char vvweapstolen[MAXWEAPONSCARRY];	// normaly a 0
// missiles, gonna be gone when new global generic time warp system integrated
//	struct missi missileinfos[NMISSILES];
// handling
	float extratraction; // less slides
	float extratopspeed; // faster top end
	float extraaccel; // get to top speed quicker
//	float extrafly; // try less gravity in air
	int extramagnatraction; // try magnatraction when in contact with ground
// defense
//	int extrashieldsup;
// icrms
//	int enableicrms;
//	float icrmsparm;
//	int inoil,inecto;
//	int randomizeron,weapbooston;
// klondye
	tree2* klondykebox0,*klondykebox1;
	tree2* buzzbox0,*bumperbox0,*forkbox0;
// weapon interface
//	int curselweap; // currently selected weapon, not activated..
// energies
//	int energies[MAXENERGIES];

	int xcurweapkind;
	int xcurweapslot;
	int xcurweapstate;
	int lastactiveweapslot,lastspecialweapslot;
	int curweapvar;
	float xweapframe,xweapboostframe;
	int extrashieldsup;
//	float vicrmsparm;
	int venableicrms;
	int tweapbooston,tshieldbooston;
	int vinoil,vinbuzzspin;
	int vinecto,vinemb,vinleech,vinflash,vunderbigtire;
	int vinsonic,vinthermo,vinfusion,inmagnet,inghook;
	int thooknum,vhooknum,magnum;
	unsigned char vvweapstolen[MAXWEAPONSCARRY];
	unsigned char ttweapstolenslot;
	char ttweapstolenfrom;
	unsigned char tenergies[MAXENERGIES];
//	unsigned char lenergies[MAXENERGIES];
	int charges[MAXENERGIES];
	tree2* hologram,*prism;

	int ol_numflags;
	int carvis;
};

extern struct ol_data od; // global data
extern struct ol_playerdata opa[OL_MAXPLAYERS]; // player data
extern struct ol_playerdata *op;	// pointer to current player data

// onlinerace_init reads from this structure to setup the race options
//struct playeropt {
//	char playername[NAMESIZE];
//	int carid;
//	int cntl;
//	int pid;
//};
//struct conninfo {
//	int ip;
//	int port;
//	int ready;
//};
struct onlineopt {
	U32 medalsearned[MAXMEDALS];
	char ntrackname[NAMESIZEI]; // if blank use constructed track
//	char skyboxname[NAMESIZEI];
//	int car2009first;	// selected car for player0 (2009_cars) (temp)
	int ncars;		// number of car slots
	int nplayers;   // number of connections
	int nbots; // num extra bots nbots+nplayers=ncars
	nettype nt;
	U32 playerid;
	U32 gameid;
	C8 gt;			// game type, TSN who knows
//	int curslot;	// camera follows this car..
//	int carid;
//	struct playeropt popt[OL_MAXCARSLOTS]; // car slots
//	struct conninfo conninfos[OL_MAXPLAYERS];	// player slots
//	float trackscale,trackstart,trackend;
//	char useskybox[NAMESIZEI];
	int backtoopt; // set to a 1 when main game should return to online opts
//	int numprebuilttracks;
	U32 serverip;
	int myip;
	int port;
};

extern struct onlineopt ol_opt;

struct globalstate {
//	int dum;
	struct weapfly weapflys[MAXWEAPFLY];
	int	flagidx;
	struct colltree colltrees[MAXCOLLTREES];
	int ncolltrees;
	bool coins_caught[MAXCOINS];
	bool trophies_caught[MAXCOINS];
	bool superenergies_caught[MAXCOINS];
};

extern struct globalstate globalstatepacket,globalstatepack0;

struct playerstatepacket {
// numbers messed up
// 2009 boxai
// 0
	struct chkbox* ol_crashresetbox;
// 4
	pointf3 pos;
// 14
	pointf3 carvel;
// 24
	pointf3 carang;
// 34
	pointf3 carvelang;
// 44
	float shocklen[4];
// 54
	float wheelang[4];
// 64
	float wheelvel[6];
// 7c
	float wheelyaw;
	int ol_lastairtime;
// 84
	int ol_airtime;
	int ol_cbairtime;
	float ol_littleg;
	int ol_validroadnorm;
// 94
	pointf3 ol_carnorm;
// a4
	pointf3 ol_cardir;
// b4
	pointf3 ol_carvelnorm;
// c4
	float ol_carvelmag;
	float ol_shockmag;
// cc
	pointf3 ol_shocknorm;
// dc
	pointf3 ol_lastroadnorm;
// ec
	float ol_accelspin;
	int ol_drivemode;
// f4
	int ol_hifrictime;
// f8
	pointf3 ol_ulastloc;
// 108
	float ol_carbodymag;
	int ol_ufliptime;
	int ol_doacrashreset;
// 114
	int ol_flymode;
	int ol_utotalcrashes;
	int ol_utotalstunts;
	int ol_startstunt;
// 124
	pointf3 ol_carstuntrpy;
// 134
	int ol_steertime;
	float ol_uextraheading;
//	int ol_lastkludge;
	int ol_cantdrive;
// 140
	pointf3 ol_crashresetloc;
// 150
	pointf3 ol_crashresetrot;
// 160
	float ol_crashresetframe;
// 164
//	float ol_crashresetdir;
	int ol_uready;
//	int cantstartdrive;
// 168
	pointf3 ol_ulastpos;
// numbers now messed up.. 2009 boxai
// 178
	struct chkbox *boxchk;
// 17c
	float aiturnval; // used by boxai
// 180
	int ol_carboost;
// 184
	pointf3 ol_landspot;
// 194
	int ol_rollpie[4];
// 1a4
	int ol_pitchpie[4];
// 1b4
	int ol_yawpie[4];
// 1c4
	int ol_rollpiesum;
	int ol_pitchpiesum;
// 1cc
	int ol_yawpiesum;
	int ol_cantaddroll;
	int ol_cantaddpitch;
	int ol_cantaddyaw;
// 1dc
	int ol_udidroll;
	int ol_udidyaw;
	int ol_udidpitch;
	int piol_uup;
// 1ec
	int piol_udown;
	int weapkey;
//----------- 84

//32  struct pinput pi	// should be 1
//---------------- 1

// 1f4
	pointf3 ol_ucarrotvel;
// 204
	int ol_ttp;
	int dofinish; 
//	int cntl;
// 20c
	float seekframe,botbigscan;
	float seekframe2;
	int laps2;
// 21c
	int chkpnt,chklap;
// 224
	pointf3 stuntgoals;
// 234
	pointf3 curstuntangs;
//---------------- 48 

// 244
	int doaistunt[3];
//	int lapready;
//	int lapsdone;
// 250
	int finplace;
// 254
	int cartspassed;
	int stuckwatchdog;
// 25c
	int clocktickcount;
// weapons
/*	int curweap,lastcurweap;
	float weapframe;
//	struct missi missileinfos[NMISSILES];
	int extrashieldsup;
	int enableicrms;
	float icrmsparm;
	int inoil,inecto;
	int randomizeron,weapbooston;
// thief side
	unsigned char tweapstolenfrom;//[MAXWEAPONSCARRY];
	unsigned char tweapstolenslot;
// victim side
	unsigned char vweapstolen[MAXWEAPONSCARRY];	// normaly a 0
// your energies
	int energies[MAXENERGIES]; */
// 260
	int xcurweapkind;
	int xcurweapslot;
	int xcurweapstate;
// 26c
	int lastactiveweapslot,lastspecialweapslot;
	int curweapvar;
	float xweapframe;
// 27c
	int extrashieldsup;
//	float vicrmsparm;
	int venableicrms;
	int tweapbooston,tshieldbooston;
// 28c
	int vinoil,vinbuzzspin;
// 294
	int vinecto,vinemb,vinleech,vinflash,vunderbigtire;
// 2a8
	int vinsonic,vinthermo,vinfusion,inmagnet,inghook;
// 2bc
	int thooknum,vhooknum,magnum;
// 2c8	
	unsigned char vvweapstolen[MAXWEAPONSCARRY];
// 2de
	unsigned char ttweapstolenslot;
	char ttweapstolenfrom;
// 2e0
	int numflags;
// 2e4
	unsigned char tenergies[MAXENERGIES];
// 2ea
//	unsigned char lenergies[MAXENERGIES];
// 2f0
	float shockextra;
// 2f4
	int extrapouncer;
// 2f8
	int extrabigtires;
// 2fc
	int speedup,slowdown,onice,quicksand;
// 30c
	int wrongway2;
	int ncoins_caught;
	int ntrophies_caught;
	int nsuperenergies_caught;
};

struct gamecfg {
	int forcebotmode;//,testkicknum;	// set from main config.txt 
	int beforeloadtimeoutclient;
	int beforeloadtimeoutserver;
	int carextracttimeout;
	int afterloadtimeout;
	int gameplaytimeout;
	int timeout;
//	int timeoutg;
	int loadinggoal1;
//	int scorelinekind;
	float tweakolwheels;
	float tweakolwheelsu;
	float tweakolwheelsv;
	float crashresety;
//	int stubtracknum;
// go this way
	pointf3 gothiswayscale;
	pointf3 gothiswaypos;
	pointf3 gothiswayrot;
// energy matrix..
	int energymatrix[MAX_WEAPKIND][MAXENERGIES];
//	int energymatrixused[MAX_WEAPKIND];
//	int noweapsum;
	int extractsleep;
// udp
	int udpserversize,udpclientsize;
	int udpenable,tcpdisable;
//
	int rules;
//	float bluered3d;
};

//extern tree2* detachweaps[MAXDETACHKIND];

extern struct gamecfg olracecfg;
void setuprematch();
void pk_restorepstate0(int p);

void spawnc2ceffect(pointf3* loc,pointf3* norm,float mag,pointf3* vel);
void spawnmtrail(pointf3* loc);
void spawnchemtrail(pointf3* loc);
void spawnstasisfield(pointf3* loc);
void openc2ceffect();
void closec2ceffect();

struct weapfly *allocprojectile();
void freeprojectile(struct weapfly *);

void makeaflyweap(int wk,int hitcar,pointf3* pos,pointf3* rot,pointf3* vel,int fuel,int from); // does it all
// some detachable weapon procs
void procmissiles(struct weapfly *wf);
void procecto(struct weapfly *wf);
void procemb(struct weapfly *wf);
void proclaser(struct weapfly *wf);
void procoil(struct weapfly *wf);
void procmine(struct weapfly *wf);
void procflag(struct weapfly *wf);

void procghook(struct weapfly *wf);
void procfusioncan(struct weapfly *wf);
void procthermocan(struct weapfly *wf);
void procsonic(struct weapfly *wf);

void procgbomb(struct weapfly *wf);
void procstasis(struct weapfly *wf);
void procramp(struct weapfly *wf);
void procwall(struct weapfly *wf);

void initmissiles(struct weapfly *wf);
void initecto(struct weapfly *wf);
void initemb(struct weapfly *wf);
void initlaser(struct weapfly *wf);
void initoil(struct weapfly *wf);
void initmine(struct weapfly *wf);
void initflag(struct weapfly *wf);

void initghook(struct weapfly *wf);
void initfusioncan(struct weapfly *wf);
void initthermocan(struct weapfly *wf);
void initsonic(struct weapfly *wf);

void initgbomb(struct weapfly *wf);
void initstasis(struct weapfly *wf);
void initramp(struct weapfly *wf);
void initwall(struct weapfly *wf);

extern struct rule *currule; // rule interface
void convertfont(struct tsp *t,int oldformat,int newformat);
void ol_drawstring2(struct tsp *f,struct bitmap32 *dest,char *s, int x, int y,int maxwidth);
extern char *energynames[];
void convertframe(struct bitmap16 *b,int oldformat,int newformat);
#define INITCANTSTARTDRIVE (3*60)


void updatedifftime(int clk);

extern struct ol_playerdata *of;	// pointer to player with the camera focus..
