//#define MATHTEST // test transcendental functions across net
#ifdef MATHTEST
enum {
// 1
	MT_ECHO,
	MT_DOUBLE,
	MT_HALF,
	MT_REC,	 // 1/x
	MT_SQRT,
	MT_SIN,
	MT_COS,
	MT_TAN,
	MT_EXP,
	MT_LOG,
	MT_ACOS,
// 2
	MT_ADD,
	MT_SUB,
	MT_MUL,
	MT_DIV,
	MT_ATAN2,
};
struct mathreq {
	int kind;
	float x,y;
};

#endif
// a class to handle all networking aspects of any network game, hiding socker layer
// has 2 parts, connection and ingame
enum PACK_KIND {
// keepalive
	PACK_NONE,
// connect
	PACK_JSON_WELCOME,
	PACK_JSON_HI,
	PACK_JSON_TRACK,
	PACK_JSON_GO,
	PACK_MATHTEST,
// game, broadcast
// from here on out, 2 ids for each packet one for C2S and one for S2C (S2C includes a U8 'from') make bigger later...
	PACK_KEY,		// struct packkey C2S
	PACK_KEYF,		// struct packkey, with 'from' S2C

	PACK_USER=1000,
	PACK_CHAT=PACK_USER, // C2S
	PACK_CHATF, // S2C, with 'from'
	PACK_GAMESTATE, // C2S
	PACK_GAMESTATEF, // S2C
};

struct message {
	PACK_KIND id;
	vector<U8> data;
};

class netobj {
protected:
	gamedescj* cgd;
	gamerunj* grj;
	bool owngamerun; // true if destructor should delete grj
// ingame
	vector<deque<U8> > keyques;
	S32 queoffset;
	bool letsgo; // false: connect, true: ingame
// 'user' packets
	vector<list<message > > genques;
public:
	static netobj* create(gamedescj* gd); // connection:
	netobj(gamedescj* gd); // connection: get lobby gamedesc setup
//	netobj(const gamedescj* gd) :cgd(gd),grj(0),owngamerun(true),/*yourid(-1),*/queoffset(0),letsgo(false) {} // connection: get lobby gamedesc setup
	bool isready() { return letsgo; } // transition:
	gamerunj* getgamerunj(); // transition: after isready returns true
// ingame key packets, game will keep track of current time
	void buildkeyques(); // setup keyques based on gamerunj
	S32 lastvalidtime(); // time of first invalid key from any player, 0 at start, when 1 or more, then all players arrived
	S32 farthestvalidtime(S32 yid); // time of last invalid key from any player (not own 'yid'), 0 at start, when 1 or more, then all players arrived
	S32 lastvalidkeytime(U32 slot); // ingame: human, time of last valid key, starts at 0
	bool canreadkey(U32 slot,S32 when); // ingame: human, get key from slot at time starts at 0
	keys readkey(U32 slot,S32 when); // ingame: human, get key from slot at time starts at 0
	keys readlastkey(U32 slot); // ingame: human, get last key from slot, or '0' if no keys in queue
	void clearearlykeydata(S32 t); // remove early key data
	void drawkeybuff(); // bitmap32
	void drawkeybuff3d(); // sprite
	virtual string proc()=0; // connection/ingame: returns status messages (abstract)
	virtual void writekey(const keys& k); // ingame: send key to collective, base just adds key to local collective
	virtual void writenone() {} // ingame: send none to collective, (keepalive for server, waiting for all players to arrive)
	virtual ~netobj() { if (owngamerun) delete grj; } // connection/ingame: the destroyer
	virtual S32 getnumsockets()=0;
	virtual S32 getwritebuffsize(S32 slot)=0;
	virtual S32 getwritebuffused(S32 slot)=0;

	virtual void writegen(PACK_KIND id,const vector<U8>& mess);
	bool readgen(U32 slot,PACK_KIND& id,vector<U8>& mess);
	S32 getnumplayers() { return grj->numplayers; }
	S32 getnumhumanplayers() { return cgd->nhumanplayers; }
};

struct clnt {
	socker* s;
	S32 timeout;
	bool cantimeout;
	int state; // no enums yet
	clnt() : s(0),timeout(0),cantimeout(true),state(0) {}
	S32 getwritebuffsize() {return tcpgetwbuffsize(s);}
	S32 getwritebuffused() {if (s) return tcpgetwbuffused(s); else return 0;}
};

struct keyfrom {
	U8 from;
	U8 k;
};
