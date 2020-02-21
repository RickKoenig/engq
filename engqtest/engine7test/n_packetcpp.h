void packetreadbuff(),packetwritebuff();
void packetsendhere();

enum PACK_KIND {
	PACK_NONE,
// connect
	PACK_WELCOME,	// SV, track hash 
	PACK_HI,		// CL, struct avatar
	PACK_HIACK,		// SV, struct gametype
	PACK_HIACKC,	// SV, struct gametype with constructed track data, includes name
	PACK_HIACKNC,	// SV, struct gametype with new constructed track data, includes name
//	PACK_HIACKH,	// SV, struct gametype use hashed track
	PACK_GO,		// SV, struct avatar
// game
	PACK_HERE,		// struct packhere
	PACK_KEY,		// struct packkey
//	PACK_RESTART,	//
//	PACK_NOP,
//	PACK_TEST,
};

void twsave0();
void twrestore0();
void twsaven();
void twrestoren();

void packetsendkey(int ts,int u,int d,int l,int r,int resetgame,int rematchyes);//,int rematchno);
int firstinvalidkeybuff(); // first predicted time..
void resetkeybuff();
//void addkeybuff(struct packkey *pk);, handled by packetreadbuff and packetsendkey
void getkeybuff(int pn);
void remkeybuff(int timest); // this time stamp and earlier will no longer be needed
extern struct twgg twggs;
void drawkeybuff();
