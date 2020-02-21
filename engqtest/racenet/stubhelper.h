#define MAXSTUBCARS 16
#if 0
#define MAXWEAPONSCARRY 9
#define MAXENERGIES 6
#endif
//#define MAXPLAYERS 8
//enum {NET_BOT,NET_SERVER,NET_CLIENT};
struct cardata {
	char c_name[100];
	char c_body[100];
	hsv c_hsvs[NUMHSVS];
	int c_rims;
	unsigned char c_weaps[MAXWEAPONSCARRY+1]; // 1 more for the rpd weapon stolen...
	int c_nweap;
	int c_energies[MAXENERGIES];
	char c_paint[100];
	char c_decal[100];
};

/*struct btinfo {
	char botbodys[MAXSTUBCARS][100];
	int botrims[MAXSTUBCARS];
	char botpaints[MAXSTUBCARS][100];
	char botdecals[MAXSTUBCARS][100];
	int botenergies[MAXSTUBCARS][MAXENERGIES];
	unsigned char botweaps[MAXSTUBCARS][MAXWEAPONSCARRY];
	int nbotweap[MAXSTUBCARS];
	hsv hsvs[MAXSTUBCARS][NUMHSVS];
};*/

struct customstub {
	cardata cd;
	int score;
	int stunts;
	int credits;
	int experience;
//	int c_tracknum;
//	int c_nhsvs; // 0 or NUMHSVS
};

struct stbinfo {
// all
	int usestub;
//	int useslextra;
	int random1,random2;	// for checksumming weapons...
//	int networkmode;
	int tracknum;
	int nplayers; // max number of connections
	int ncars; // number of objects, is server if >0 and not allready running 
	int serverip; // clients
	int port;
	customstub names;
	int gamesplayed;
	int gameswon;
	int highestscore;
	int finished;
// for carstubinfo
	int playerid; // connection index for car
	int carid; // self
	int drivemode;
//	struct socker *sock;
//	unsigned char weaps[MAXWEAPONSCARRY];
//	int nweap;
	int place;
	int bots;
	int connections;
	int rules;
	int xres;
	int yres;
	int used3d;
};

// pass to server, settings for YOUR car slots
extern struct stbinfo mainstubinfo;
// get back from server, settings for all car slots
extern customstub carstubinfos[MAXSTUBCARS];
//extern struct btinfo botinfo;
extern cardata botinfos[MAXSTUBCARS];

//void stub_earlymaininit();
void stub_maininit(int stubinited);
void stub_latemainexit();

void writetalkfile(char *str); // current settings are ExtractData,done,failed
int readtalkfile();		// returns true if DoneExtracting
