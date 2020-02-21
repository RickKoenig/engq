// breadcrumbs and for placer
class fs {
	S32 slot,lap,nsf,cnt,time;
public:
	static bool fcomp(const fs& lhs,const fs& rhs);
	fs(S32 slota,S32 lapa,S32 nsfa,S32 cnta,S32 timea) : slot(slota),lap(lapa),nsf(nsfa),cnt(cnta),time(timea) {}
	friend class placer;
};

typedef bool(*fp)(const fs& lhs, const fs& rhs);

// figure out current place during race
class placer {
	static S32 cnt;
	S32 nplayers;
	set<fs,fp> st;
	vector<fs> plc; // remember what's in st by slot index
	vector<fs> bcs; // breadcrumbs
//	vector<fs> bcs2; // 2nd place breadcrumbs
public:
	placer(S32 nplayersa);
	void update(S32 slot,S32 laps,S32 dist,S32 clk);
	void reset();
	S32 getplace(S32 slot); // return 0 if not ready yet
	S32 gettime(S32 slot); // return 0 if not ready yet
};


class gamerunj {
public:
	placer* plc; // place class
	netdescj n;
	envj e;
// your 'id'
	int numplayers;
	int yourid; // your computer id for this game
	int yourcntl; // none, human, bot
	int viewslot; // current slot you are looking at
	vector<caroj> caros;
// console
	class listbox* con;
//	struct con32* con;
// TW
	twgg gs; // current clock
	twgg gs0; // clock 0
	twgg gsn; // last unpredicted clock
	bool inchat;
	string chatstr;
	S32 bail;
	bool predicted,intimewarp;
	gamerunj() :
		plc(0),numplayers(1),yourid(0),yourcntl(CNTL_AI),viewslot(0),
		/*con(0),*/inchat(false),bail(0),predicted(false),intimewarp(false) {}
	void hashadd(const hashi& h) const;
	void log(const vector<keys>& ks) const;
	~gamerunj()
	{
		delete plc;
	}
};

