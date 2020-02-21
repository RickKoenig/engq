enum nettype {NET_BOT,NET_SERVER,NET_BCLIENT,NET_CLIENT};
//enum gamekind {GAME_TOURNAMENT,GAME_SKILLMATCH,GAME_CHALLENGE,GAME_NUM};
struct netinf {
	U32 ip,port;
	nettype nt; // 2 client game (see above)
	C8 gt;
	U32 gameid;
	U32 nhumans;
	U32 nbots;	// NYI
	U32 nplayers; // total of nhumans and nbots
};

struct trackinf {
	string trackname;
	U32 rules; // 7 combat boxes, 6 noweaps boxes
};

struct hsv {
	U32 h,s,v;
};

#define MAXWEAPONSCARRY 9
#define MAXENERGIES 6

#define NUMHSVS 3
#define MAXOTHERS 5 // number of 'alts'
#define MAXMEDALS 8
struct playerinf {
	string username;
	U32 pid;
	S32 besttime;	// NYI, your best time on this track // centisecs
	U32 medalsearned[MAXMEDALS]; // check MED_MAXMEDALS too.
	string carmake;
	U32 rims;		// 1 thru 12
	vector<U32> weapons;	// empty upto [MAXWEAPONSCARRY];
	U32 energies[MAXENERGIES];
	U32 exp;		// NYI, experience
	float avgpos;	// NYI, average postion
	hsv hsvs[NUMHSVS];
};

struct challengerinf {
	string username; // NYI
	U32 uid; // NYI
	S32 besttime;	// NYI
};

class gameinf {
public:
	bool isvalid;
	netinf ni;
	trackinf ti;
	playerinf pi;
	challengerinf ci; // NYI
	gameinf(const C8* parsestr);
//	void save(const C8* fname);
};

extern gameinf* globalgame;	// game info gets loaded here.
/*
# Battle Race Starting Plan #1.  
# Disclaimer - This is the first pass, there will be some changes, but the 
# basic system should stay true.

# We'll need to firm up the list of weapons, and naming conventions for # vehicles to be totally functional

# A race is started by sending a message to the exe via the message commmand in osakit.
# The message is basically a long ascii string with space and commas as delimiters.
# if spaces are in the data, then ~ is used as quotes.
# Command chunks are defined by a letter, and a certain # of parameters.

#OVERVIEW OF DATA DEFINITIONS
u  # user name.
r  # race info
v  # vehicle info
w  # weapon list
e  # energy / Batteries/ Fuel cell
x  # player experience and relative skill
i  # ip info for ghost server- important for scaling.
g  # game start

# Here is a detailed list of each data item

u ~Battling Bob~			# user name.

# track #,rules (always 7),rules2(always 2),#Players,#ofbots, unique game id
r Dino,7,2,8,0,T123456789  

#vehicle# ,color HSV0, HSV1, HSV2 
v ~Deoria II~,208,255,255,26,255,255,359,255,255  

w 12,23,14,15,17,0,0,0		# weapons equipped on car.
e 5,5,5,5,5,5 				# energy  - # of batteries.
x 500000,2.2   				#exp, av position last 5 games,
i 192.168.0.167,5421 		#ip - important for scaling, port,
g							#game start


# The practical use would look something like this...
"OSAKit.CallOSAKit ('MESSAGE|u ~Battling Bob~ r dino,7,2,8,0,T123456789  v twinmill,208,255,255,26,255,255,359,255,255 w 12,13,14,15,17,18,19,0 e 5,5,5,5,5 x 500000,2.2 i 192.168.0.167,5421 g)"
*/
