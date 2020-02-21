//#define SELTRACKWID 7

void seltrack_init(),seltrack_proc(),seltrack_exit(); // 1 ONLINE select track
void seltrack_loadconfig();

void patch_weapons();
void patch_energies();

// return -1 if no track, 1 and up otherwise
S32 findtrackidx(const script& tracklist,const C8* shorttrackname);

class trackinfo {
	script* ts;
	S32 ntracks;
	static S32 SELTRACKWID;
public:
	trackinfo();
	S32 getnumtracks();
	S32 findtrackidx(string name);
	string gettrackname(S32 idx);
	string gettracknamebws(S32 idx);
	string gettrackdir(S32 idx);
	string gettrackpicname(S32 idx);
	string gettracktopname(S32 idx);
	string gettrackcfgname(S32 idx);
	float gettrackscale(S32 idx);
	float gettrackstart(S32 idx);
	float gettrackend(S32 idx);
	string gettrackskybox(S32 idx);
	S32 gettrackdefaultrules(S32 idx);
	~trackinfo();
};

extern S32 weapxlate[100];
S32 inverseweapxlate(S32 old);
