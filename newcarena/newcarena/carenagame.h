void carenagameinit(),carenagameproc(),carenagamedraw3d(),carenagamedraw2d(),carenagameexit();

void gametick(const vector<keys>& ks);//,bool predicted,bool intimewarp);
vector<keys> getkeys();

extern class gamerunj* gg;
extern class netobj* no;
extern class soundfx* sfx;

struct gamecfgn { // POD
// set from config.txt at gameinit, values don't normally change
// display gaminfo before game loads/starts
	int statustime;
//	game camera
	pointf3 campos,camrot;
	float camzoom;
	S32 defuloop;
	float scrlinez;
// in game
//	float car2cardist;
//	S32 candrivetime;
//	S32 rematchtime,norematchtime;
};
extern struct gamecfgn gcfg;

