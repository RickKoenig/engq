void carenagameinit(),carenagameproc(),carenagamedraw3d(),carenagamedraw2d(),carenagameexit();
void startgame();

#define DESIREDBPP 565

extern struct state carenagamestate;

#define NAMESIZEI 32
struct carinfo {
	char carname[NAMESIZEI];
	char paint;
	char decal;
	char weapons[MAX_WEAPCARRY+1]; // 1 more for the rpd..
	char nweapons;
	char energies[MAX_ENERGIES];
};

// your picked game settings from gamepicked.txt
struct avatar {
// player
	char playername[NAMESIZEI];
// a car
	struct carinfo ci;
// if 0 then not used
//	int used; // C for now 
// 'id' of who owns/controls this obj
//	if negative then thats YOUR ownwerid...
	int ownerid; // sorta like which computer controls this car
//	int gottrackhash;	// used only when client says it has that track, server doesn't have to send track to client
// where is index in array
//	int slotnum;
//	type of player
//	int cntl;
};
//extern struct avatar avatars[MAX_PLAYERS];

/*	PACK_WELCOME,	// SV, track hash 
	PACK_HI,		// CL, struct avatar
	PACK_HIACK,		// SV, struct gametype
	PACK_HIACKC,	// SV, struct gametype with new constructed track data, includes name
	PACK_HIACKH,	// SV, struct gametype use hashed track
	PACK_GO,		// SV, struct avatar
*/
/*enum tracktype {TT_PREBUILT,TT_CUSTOM};
struct trackinfo {
	tracktype tt;
};*/

// PACKET specific structures
// server sends this first, hash if using custom tracks, to see if client can match
enum {TRACKTYPE_NONE,TRACKTYPE_OLD,TRACKTYPE_NEW};
struct trackinfo { // server welcome packet
	char trackname[NAMESIZEI];
	int validtrackhash;
	unsigned char trackhash[NUMHASHBYTES];
};
// then player sends hi
struct histruct {
	struct avatar av; // client specific player data
	int gottrackhash; // 1 if client matches track hash sent in welcome packet
};
// then server sends various hiack structures
struct gameinfo { // doesn't need to send track data
	int humplayers;
	int botplayers;
	int rules;
};
struct gameinfoc { // needs to send track data
	struct gameinfo gi;
	struct trk gametrk;
};
struct newgameinfoc { // needs to send track data
	struct gameinfo gi;
	U32 rawtrklen;
	unsigned char rawtrk[1];
//	struct newtrk newgametrk;
};
// then server sends go packet with clients id and array of avatar structures
// END PACKET structures

struct gametype { // global configurable data
	struct gameinfo gi; // rules types of players
	struct trackinfo ti; // track info minus data
	struct track gametrack;
	struct newtrack *newgametrack;
//	int sid;	// server sets this when sending a HIACK packet, unused otherwise
};

/*struct gametypec { // custom tracks, need to send track data
	int humplayers;
	int botplayers;
	int rules;
	char trackname[NAMESIZE];
//	int sid;	// server sets this when sending a HIACK packet, unused otherwise
};

struct gametypeh { // custom tracks, allready have track (by hash)
	int humplayers;
	int botplayers;
	int rules;
//	int sid;	// server sets this when sending a HIACK packet, unused otherwise
};
*/
/*struct hiack {
	struct gametype gt;
	int yourid;
};
*/
// a complete game description from 1 nodes point of view
struct gamepick {
	struct gametype gt; // possible game parameters
//	struct gametypec gtc; // constructor 
//	struct gametypeh gth; // constructor 
	struct avatar gpav; // your info
	int gamenet; // type of network connection, server, client, botgame
	int serverip; // how to connect to server if client
	int serverport;
};
extern struct gamepick gp;

struct gamecfgn {
// set from config.txt at gameinit, values don't normaly change
//	camera
	pointf3 campos,camrot;
	float camzoom;
	int loadinggoal;
	int beforeloadtimeoutserver;
	int beforeloadtimeoutclient;
	int afterloadtimeoutserver;
	int afterloadtimeoutclient;
	int ingametimeout;
	int rematchtime,norematchtime;//,exittime;
	int statustime;
	int defuloop;
	float scrlinez;
	float car2cardist;
};
extern struct gamecfgn gcfg;

struct keys {
	int ku,kd,kr,kl,kresetgame,krematchyes,wentbot;//,krematchno;
};

struct gameglobal {
	int numobjs;
// your 'id'
	int yourid; // your computer id for this game
	int viewslot; // current slot you are looking at
//	int yourslot; // C the car your supposed to drive
// connect
	struct socker *sock[MAX_PLAYERS];
	int wrotego,timeout,numcurplayers;
	int goingtogame;
//	int sockerinited;
	struct socker *master;
	int myip;
	int retrytimers[MAX_PLAYERS];
// loading
	struct bitmap32* loadingbackground;
	int loadingcount;
// global gameplay
	int thirdmilli;	// third's of a milli seconds! ( 3000 is 1 second), 0 to 49
	int milli,lastmilli,diffmilli,pingclock;
// trees
	tree2* roottree,*thetrack,*scrlinenull;
	struct texture *clocktex;
	struct bitmap32* clocksave;
	int oldtt;	 // old clock time, for scrline update
// gameconnect
	int gamestart;
	struct con32* con;
// keys
	struct keys yk,pk;
	int yourcntl;
	int rematchcount;//,realhumplayers;
// TW manager
	int intimewarp,predicted,newdata;
// TW
	int clock,clockoffset;	// game clock, in 1/60 sec
};
extern struct gameglobal gg;

struct caro {
// car graphic
	tree2* carnull;//,*carbody;
	n_carclass* cc;
// car parameters
	struct avatar av;
// TW
// car ai
	int rematchyes;//,rematchno;
	int seekmode;
//	car physics
	pointf3 pos,rot,vel,rotvel;
};
extern struct caro gos[MAX_GAMEOBJ];

struct twgg {
	int clock,clockoffset;
};

struct twcaro
{
// car ai
	int rematchyes;//,rematchno;
	int seekmode;
//	car physics
	pointf3 pos,rot,vel,rotvel;
};
