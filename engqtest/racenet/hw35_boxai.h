#define MAXPATH 5
#ifndef NAMESIZE
#define NAMESIZE 32
#endif
typedef pointf3 VEC;
typedef tree2 TREE;
struct chkbox {
	struct chkbox *next[MAXPATH];
	struct chkbox *prev[MAXPATH];
	U32 nnext;
	U32 nprev;
	unsigned char isreset,pathnum,isloopcam,isfastweap,isslowreset,ishalfspeed,isbotpath,ismerge;
	unsigned char istimer,isportal,isboost,isfog;
	char name[NAMESIZE];
	float num;
//	int inum;
	VEC pos,rot;
	VEC bmin,bmax;
// place
	int ftime,npassed;
	unsigned char caridchk[OL_MAXCARSLOTS];
// ptr to object
	TREE *bt;
// ptr to objects to trigger
	TREE **triggers;
	int *triggerprocidx;
	int *triggercond;
	U32 ntriggers;
// sound triggers
	int soundnum;
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
