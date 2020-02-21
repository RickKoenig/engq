// don't change!
struct crashloc {
	pointf3 loc;
	pointf3 rot;
	int checkidx;
};

#define MAXCRASHRESETS 400
struct crashinfo {
	int editenable;
	int lasteditenable;
	int doload;
	int dosave;
	int doinsert;
	int dodelete;
	int num,cur,lastcur;
	struct crashloc curedit;
	struct crashloc crashlocs[MAXCRASHRESETS];
};

extern struct crashinfo crashinfos;

void crasheditor(int millisec,struct ol_playerdata *p);
void crasheditload(),crasheditsave();
//void calccrashresets();
void docheckpoints(struct ol_playerdata *p);

// don't change!
struct checkloc {
	pointf3 loc;
	pointf3 rot;
};

struct checkplane {
	pointf3 n;
	float d;
};

#define MAXCHECKPLANES 400
#define MAXLAPS 10

struct checkinfo {
	int editenable;
	int lasteditenable;
	int doload;
	int dosave;
	int doinsert;
	int dodelete;
	int num,cur,lastcur;
	struct checkloc curedit;
	struct checkloc checklocs[MAXCHECKPLANES];
	struct checkplane checkplanes[MAXCHECKPLANES];
	int ncrashresets[MAXCHECKPLANES];
	int *crashresets[MAXCHECKPLANES]; // index into crashinfos.crashlocs
	float botseekframeresets[MAXCRASHRESETS*MAXLAPS]; // 
	int nbotseekframeresets;
};

extern struct checkinfo checkinfos;

void checkeditor(int millisec,struct ol_playerdata *p);
void checkeditload(),checkeditsave();
void buildcheckresets(),freecheckresets();
void setnewcrashloc(struct ol_playerdata *p);

/*// don't change!
struct botloc {
	pointf3 loc;
};

// don't change
#define MAXBOTPATHS 8

#define MAXBOTLOC 400
struct botaiinfo {
	int editenable;
	int lasteditenable;
	int doload;
	int dosave;
	int doinsert;
	int dodelete;
	int num,cur,lastcur;
	int pathid,lastpathid;
	struct botloc curedit;
	struct botloc botlocs[MAXCRASHRESETS][MAXBOTPATHS];
};

extern struct botaiinfo botaiinfos;

void boteditor(int millisec,struct ol_playerdata *p);
void boteditload(),boteditsave();
*/
