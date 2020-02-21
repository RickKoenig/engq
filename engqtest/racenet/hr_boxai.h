#define MAXPATH 5
#define MAXPATH 5
#ifndef NAMESIZE
#define NAMESIZE 32
#endif
struct chkbox {
	struct chkbox *next[MAXPATH];
	struct chkbox *prev[MAXPATH];
	int nnext;
	int nprev;
	unsigned char isreset,pathnum,isloopcam,isfastweap,isslowreset,ishalfspeed,isbotpath,ismerge;
	unsigned char istimer,isportal,isboost,isfog;
	char name[NAMESIZE];
	float num;
//	int inum;
	pointf3 pos,rot;
	pointf3 bmin,bmax;
// place
	int ftime,npassed;
	unsigned char caridchk[OL_MAXCARSLOTS];
// ptr to object
	tree2 *boxtree;
// ptr to objects to trigger
	tree2 **triggers;
	int *triggerprocidx;
	int *triggercond;
	int ntriggers;
// sound triggers
	int soundnum;
	int cbsoundplayed;
	int cb_specialtracknum;
};
enum {TCOND_P1,TCOND_PA};

struct cbsound {
	char name[50];
	struct wavehandle *wh;
};

void boxai_init();
void boxai_getstartpos(); // VEC *sp,VEC *rot,int car);
void boxai_ai(); // one car at a time, drive ai cars
void boxai_check(); // checkpoint_reset system
void boxai_exit();

void boxai_reinit(); // called on rematch
//void boxai_getplace(); // called to get place and difftime od.place
int boxai_slowlead();

int getlapnum(struct ol_playerdata* of);
