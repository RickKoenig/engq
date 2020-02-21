#define INITFPS 30
#define MAXFPS 30 // 3dsmax
//#define MAXARGS 20000 // is that enough?, for drag and drop
struct wi {
	string globalname;
	U32 MainWindow; // HWND
	U32 hinst; // HINSTANCE
	S32 isalreadyrunning;
	C8 datadir[MAX_PATH];
	S32 releasemode;
	S32 enabledebprint;
	S32 runinbackground;
	S32 nosleep;
	S32 startstate;
	S32 wantedvideomaindriver,wantedvideosubdriver;
	S32 dopaintmessages;
	S32 wantedaudiomaindriver,wantedaudiosubdriver,wantedaudiomicsubdriver;
	S32 wantedusedirectinput;
	S32 usedirectinput;
	S32 repdelay,repperiod,repcount;
	S32 dinput_nomouse;
	S32 mousemode,lastmousemode;
	S32 use8way;	// for joysticks, mapping hat and analog to up,down,left,right, enable diagonals
	
	S32 fpswanted;	// gets set to INITFPS unless overridden in config.txt
	S32 hiprioritywanted;
	S32 mx,my,mz,lmx,lmy,lmz,dmx,dmy,dmz;
	S32 mleftclicks,mrightclicks,mmiddleclicks;
	U32 mbut,lmbut;
	U8	keystate[256];
	S32	key;

	U32 microseccurrent2;
	U32 microseccurrent;
	float framestep;
	//static float defaultfps;
	U32 microsecavg;
	U32 microsecavg2;
	float fpscurrent;
	float fpsavg;
	float fpscurrent2;
	float fpsavg2;
	S32 sleepernum;
	S32 slacktime;
	S32 lasttime;
	S32 lasttime2;

//	S32 argc;
//	C8 **argv;
	class script* args;
	S32 justdropped;	// something was dragged and dropped
	
	S32 closerequested;
	S32 sleeper; // keeps program awake for a bit if recently got a message
	S32 activewindow;
	U32 compkhz;
	U32 indebprint;
	U32 dumpbws,dumpbwo;
	U32 relquat;	// 0 occusim, 1 carena max exporter issues

	U32 messcnt;
	U32 mess;
};
#define MX (wininfo.mx)
#define MY (wininfo.my)
#define MZ (wininfo.mz)
#define LMX (wininfo.lmx)
#define LMY (wininfo.lmy)
#define LMZ (wininfo.lmz)
#define DMX (wininfo.dmx)
#define DMY (wininfo.dmy)
#define DMZ (wininfo.dmz)
#define MBUT (wininfo.mbut)
#define LMBUT (wininfo.lmbut)
#define KEY (wininfo.key)

// engine related global variables
extern struct wi wininfo;

// supplied by  user
extern C8 winmain_datadir[];
extern C8 winmain_version[];
void maininit();
bool mainproc(); // return true to keep in main game loop
void maindraw3d();
void maindraw2d();
bool mainhasdraw3d();
bool mainhasdraw2d();
void mainexit();
