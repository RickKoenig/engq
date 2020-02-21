void match_logon_init(),match_logon_proc(),match_logon_draw3d(),match_logon_exit();
extern struct socker *matchcl;
extern char username[];
extern char hostname[];

// set by match lobby or match quickgame
struct carenagameinfo {
	bool isvalid;
	bool ishost;
	U32 ip,port;
	U32 nports; // 1 to 10, scan for open ports
// game specific data
//	U32 tracknum;
	U32 gameidx; // testA,carena
};
extern carenagameinfo cgi;

socker* makecgisock(socker** udpsock=0);

U32 getngames(); // get number if different kinds of games
const C8* getgamename(U32 idx); // get gamename, given game idx
S32 getgamehoststate(U32 idx); // get gamehoststate, given game idx
S32 getgamejoinstate(U32 idx); // get gamejoinstate, given game idx
S32 getgamemasterport(U32 idx); // get game master port start
S32 getgameidx(const C8* gamename); // get gameidx, given game name, or -1 if not found
bool canbequick(U32 idx);	// can launch from quickgame
