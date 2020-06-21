#include "st2_uplay3.h"

// stunt2 3d realtime constructor demo

// engq API
#include <m_eng.h>
#include <l_misclibm.h>
#include "st2_glue.h"
#include "st2_roadheight.h"
#include "st2_uplay3.h"
#include "st2_uplay3_internal.h"
#include "st2_uphysics2.h"
#include "st2_soundlst.h"
#include "st2_tsp.h"

// list of user states
#include "u_states.h"

// for buildprism, or other 3d primitive objects
#include "u_modelutil.h"

// 3d objects
static tree2* roottree;
static tree2* obj1;

////////////////////////// main
void uplay3_inito()
{
	// setup window size
	video_setupwindow(GX, GY);

	// setup trees
	pushandsetdir("gfxtest"); // default graphics directory
	roottree = new tree2("roottree"); // main tree
	//obj1=buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
	obj1 = buildplane_xy(pointf2x(1, 1), "maptestnck.tga", "tex", "basicplane");
	roottree->trans.z = .5f; // move world forward to see objects
	roottree->linkchild(obj1);
	popdir();

	// setup viewport
	mainvp = viewport2x(); // a good default viewport
	mainvp.backcolor = C32GREEN;
}

void uplay3_proco()
{
	// input
	if (KEY == K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	// proc
	roottree->proc();
	doflycam(&mainvp);
}

void uplay3_draw3do()
{
	// draw
	video_buildworldmats(roottree); // help dolights
	dolights();
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
}

void uplay3_exito()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree; // free the main man
}


#if 0
#include <stdio.h>
#include <string.h>
//#include <windows.h>
#include <math.h>
#include <stdlib.h>
//#include <d3dtypes.h>

//#include <engine1.h>
//#include <misclib.h>
//#include "videoi.h" // access video_maindriver
//#include "globalres.h"
#endif
//#include "editor.h"
#define NUMBODS 49 // as defined in editor.h
//#include <commoninc/keys.h>
#if 0
//#include "tensor.h"
//#include "rick.h"

//#include "macros.h"
//#include "../3dlib/mesh.h"
//#include "data.h"
//#include "misc.h"
//#include "3d.h"
//#include "memalloc.h"

//#include "camera.h"
//#include "motion.h"
//#include "tree.h"
//#include "debprint.h"
//#include "lwsread.h"
#endif
//#include "st2_roadheight.h"
//#include "st2_uphysics2.h"

namespace st2 {
#include "st2_game.h"
#include "st2_pieces.h"
	//#include <winput/winput.h>
//#include <3dlib/engine.h>
//#include "pieces.h"

//#include "states.h"

//#include "soundlst.h"
//#include "miscdef.h"
//#include "tspread.h"
//#include <rvid/rvid.pro>
//#include <ros/ros.h>
//#include "newtex.h"
//#include "perf.h"
#if 0

#include "st2_uplay3.h"
#include "st2_uphysics2.h"
#include "pieces.h"
#include "soundlst.h"
#include "debprint.h" // for loadconfigfile
#include "tspread.h"
#include "game.h" // for allcars CARREV0 etc.

#include "user0.h"
#endif
extern int ufliptime;
static int zoomin; // keep at zero for now
extern int audio_maindriver;
// new variables added
//extern struct viewport mainvp;
int st2_loadtrack(char *filename, int override);
int st2_lastpiece, st2_dir;

TREE *root;//,*root0,*root1,*root2,*root3;
#define TREAD 100
#define RADAR 101

VEC newpitch = { 1,0,0,0 };


CARLIST allcars[] = {

 NULL,NULL,"boonyn.lws","boonpwup.lwo","boonshad.lwo","boonyn.tsp","boonyn.lwo","boonynbody","carclp00.lwo","boonflsh.lwo",0,1,CARREV0,
 NULL,NULL,"rollcage.lws","rollpwup.lwo","rollshad.lwo","rollcage.tsp","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1,
 NULL,NULL,"commando.lws","cmndpwup.lwo","cmndshad.lwo","truck.tsp","commando.lwo","commandobody","carclp02.lwo","commflsh.lwo",0,1,CARREV2,
 NULL,NULL,"enforcer.lws","mnstpwup.lwo","mnstshad.lwo","monster.tsp","monster.lwo","monsterbody","carclp03.lwo","mnstrflsh.lwo",0,1,CARREV3,
 NULL,NULL,"flamer.lws","flampwup.lwo","flamshad.lwo","flamer.tsp","flamer.lwo","flamerbody","carclp04.lwo","flameflash.lwo",0,1,CARREV4,
 NULL,NULL,"stingrod.lws","stngpwup.lwo","stngshad.lwo","stingrod.tsp","stingrod.lwo","stingrodbody","carclp05.lwo","stingflsh.lwo",0,1,CARREV5,
 NULL,NULL,"tread.lws","tredpwup.lwo","tredshad.lwo","treadator.tsp","tread.lwo","treadbody","carclp06.lwo","treadflsh.lwo",0,0,CARREV6,
 NULL,NULL,"shocker.lws","shckpwup.lwo","shckshad.lwo","shocker.tsp","shocker.lwo","shockerbody","carclp07.lwo","shockflsh.lwo",0,1,CARREV7,
 NULL,NULL,"tryder.lws","trydpwup.lwo","trydshad.lwo","tryder.tsp","tryder.lwo","tryderbody","carclp08.lwo","trydflsh.lwo",0,1,CARREV8,
 NULL,NULL,"slideout.lws","slidpwup.lwo","slidshad.lwo","slideout.tsp","slideout.lwo","slideoutbody","carclp09.lwo","sldflsh.lwo",0,0,CARREV9,
 NULL,NULL,"surfcrat.lws","srfpwup.lwo","srfshad.lwo","surfcrat.tsp","surfcrat.lwo","surfcratbody","carclp10.lwo","srfflash.lwo",0,1,CARREV10,
 NULL,NULL,"stepper.lws","stppwup.lwo","stpshad.lwo","stepper.tsp","stepper.lwo","stepperbody","carclp11.lwo","stepflsh.lwo",0,1,CARREV11,
 NULL,NULL,"radar.lws","radrpwup.lwo","radrshad.lwo","radar.tsp","radar.lwo","radarbody","carclp12.lwo","radarflsh.lwo",0,1,CARREV12,
};

// put in debprint and config.txt
float dirtscale;
//int dirtq;
float dirtvbx;
int dirtvrandx;
float dirtvby;
int dirtvrandy;
float dirtvbz;
int dirtvrandz;

float sub_dirtscale;
//int sub_dirtq;
float sub_dirtvbx;
int sub_dirtvrandx;
float sub_dirtvby;
int sub_dirtvrandy;
float sub_dirtvbz;
int sub_dirtvrandz;

float darkdirtscale;
float sub_darkdirtscale;
int selectedcar = 1;

static unsigned char backcolorred = 41;
static unsigned char backcolorgreen = 124;
static unsigned char backcolorblue = 42;
static int releasemode = 0;
static int slowpo = 0;
int totalcrashs;
extern int particleeffects;
static int grandtotaltrickpoints, MyGoldCoins;
static int gameplayx = 800;
static int selectedtrack = 7;
static int Esc_out;
float defaultjumpfriction = 4;
// end new variables added

//int uflameonback;
int intunnels;
int uloop = 2;
int ustarttime = 600;
extern unsigned char backcolorred;
extern unsigned char backcolorgreen;
extern unsigned char backcolorblue;
extern int usegdi;
static char uedittrackname[256];
int cantdrive;

float finishy = -.257813f;

//char **load_script(char *,int *);
//VEC world2obj(TREE *t,VEC locv);
//VEC obj2world(TREE *t,VEC locv);
//int st2_line2road(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm);
//int line2piece(int bodyid,VEC *top,VEC *bot,VEC *intsect,VEC *norm);
//int line2piecegrid(COLLGRID *cg,VEC *top,VEC *bot,VEC *intsect,VEC *norm);
extern int gameplayx, gameplayy;

//#define USETESTNULLS
#ifdef USETESTNULLS
static char *nullname = "abuiltnull";
#else
static char *nullname = NULL;
#endif

float extracamang;
int testnewcam;
VEC landspot;
float camtween, camtweenspeed;
VEC testcam, testcar;
float pitchthreshup, rollthreshup, pitchthreshdown, rollthreshdown;//,targreturn;
float littlegground = 3.875f, littlegair = 3.875;

// drive car
//float accelspin;
float startaccelspin;
float wheelaccel;
int airtime;
int stunttime;

// power
enum { FRONTLEFT, FRONTRIGHT, BACKLEFT, BACKRIGHT };

#define MAXSTEER 45
float steervel = 90, steervelback;
float loopcompensater = 2.5f;

float shadyoffset = .02f;
float shadyoffset2 = .16f;

// track data
static TRACK thetrack[NUMBODS];
static PIECE pieces[MAXPIECES];
static int numpieces;
int st2_curpiece; // put back in uplay_proc, it's out for debprint, current piece 0 to 48
static TREE *startpiece, *finishpiece, *nofinishpiece;
static TREE *bigtree;
// sound test
static int numwaves;
int curwave;
// sound
int stuntsound1, stuntsound2, stuntsound3, lightningsound;
int uplayrevfreq, uplayrevvolume;
float revfreqmul;
float revfreqadd;
//int firstsound;
int dofinish;

// input
int uup;
int udown;
int uleft;
int uright;
int urollleft;
int urollright;
int uready;
static int lup;
static int ldown;
static int lleft;
static int lright;
static int lrollright;
static int lrollleft;
int flymode;
int startstunt;
int finishpercent;
int cantstartdrive;

VEC carbboxmin, carbboxmax;
// camera
float camdrift; // how fast the camera settles, 0 to 2 (1 is lock camera)
float camrotdrift;
VEC camstart = { -50,100,25 }; // where camera is at the start of the game
VEC camtrans; // where the camera is going relative to the car
static TREE *camnull; // drive this camera, the actual camera
float thecamzoom = 2.0f;
VEC testcarpos, testcarrpy;
float uplaywheelscale = .007813f, uplaycarscale = .007813f, uplayshadowscale = .007813f;

// score line
VEC scrlinepos, scrlinescl; // defined in game.c;
// tree's used by the car
static TREE *carnull;
static TREE  *carcenternull;
static TREE   *carbody;
static TREE  *wheels[6];

// shadow
static TREE *shadownull;
static TREE *shadowb;
static TREE  *shadowbody;

// debug nulls
#ifdef USETESTNULLS
#define NNULLS2 30
TREE *uplaynulls[NNULLS2];
#endif
VEC roadprobe1, roadprobe2, roadprobei, roadprobenorm;

//VEC camrotsave;
VEC qcamdesired, camdesired;
float shocklimit;
int manualcar;

// testnulls used
/*
0 roadprobe1
1 roadprobe2
2 roadintersect
3 roadnormal
4 landspot
5-12 shocks
13-20 carbox
21-22 carboxmidpoints
23 center of mass
24 crash reset location
*/
// car scenes
extern int selectedcar;
extern int slowpo;
extern CARLIST allcars[];
extern CARRATE carratings[];

//struct animtex carbodyanimtex;//,flameranimtex;
//struct animtex burstanimtex[4];

textureb *numcointex;
struct tsp *clocktsp, *speedotsp, *bursttsp[4], *flametsp, *specialtsp, *condomtsp, *flashtsp;
static int oldtt, oldncoins, oldspeed;
static int burstframe;
int nubursts, nuflashes, nucondoms, nuflames;
int nulightnings;
struct sagger *sag;

TREE *uplayspecial1; // lightning
TREE *uplayspecial2; // condom
TREE *uplayflame1, *uplayflame2;
TREE *uaxles[4];

static TREE *dirtparticle;
struct ci {
	int nwheels;
	char *wheelnames[6];
};

struct matfunc {
	char *matname;
	void(*matfunc)();
};

void spawndirtparts();
void usenewcam();

struct matfunc matfuncs[2] = {
	"dirtsurf",spawndirtparts,
	//	"loopside",usenewcam,
		"loop",usenewcam,
};
#define NMATFUNCS 2

//radar stuff, Radar Dish Null points in -x...
TREE *radartree;
float desiredradarangle; // radar with oscillations;
float targetradarangle;	// radar without oscillations, or -1 if no coins to be found
float radardir = 1;		// direction radar oscillates +/- 1
float radarspeed = 3;
float radarextraang = 35;	// how far radar goes before coming back

float bigwallminx, bigwallminz, bigwallmaxx, bigwallmaxz;
float bigtreeminx, bigtreeminz, bigtreemaxx, bigtreemaxz;

static struct ci carinfo[13] = {
	{4,"boonwhl1.lwo","boonwhl2.lwo","boonwhl3.lwo","boonwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"comwhl1.lwo","comwhl2.lwo","comwhl3.lwo","comwhl4.lwo"},
	{4,"monwhl1.lwo","monwhl2.lwo","monwhl3.lwo","monwhl4.lwo"},
	{4,"flmrwhl1.lwo","flmrwhl2.lwo","flmrwhl3.lwo","flmrwhl4.lwo"},
	{4,"stngwhl1.lwo","stngwhl2.lwo","stngwhl3.lwo","stngwhl4.lwo"},
	{0}, // tread
	{4,"shcwhl1.lwo","shcwhl2.lwo","shcwhl3.lwo","shcwhl4.lwo"},
	{4,"trydwhl1.lwo","trydwhl2.lwo","trydwhl3.lwo","trydwhl4.lwo"},
	{4,"sldwhl1.lwo","sldwhl2.lwo","sldwhl3.lwo","sldwhl4.lwo"},
	{4,"srfwhl1.lwo","srfwhl2.lwo","srfwhl3.lwo","srfwhl4.lwo"},
	{4,"stepwhl1.lwo","stepwhl2.lwo","stepwhl3.lwo","stepwhl4.lwo"},
	{6,"radwhl1.lwo","radwhl2.lwo","radwhl3.lwo","radwhl4.lwo","radwhl5.lwo","radwhl6.lwo",}, // radar
};

CARRATE carratings[] =
{
	//top accel traction yawspeed pitchspeed rollspeed goodtrack
	  100,7,  100,   7,  7,  7,  2,   //boonyn   0
	  105,7,  100,  10, 10, 10,  0,   //rollcage 1
	  110,8,  100,   9,  9,  9, -1,   //commando 2
	   90,6.5, 92,   8,  8,  8, -1,   //enforcer 3
	  120,10,  50,   5,  5,  5,  4,   //flamer   4
   90,4.5, 50,   8,  8,  8, -1,   //stingrod 5
	   90,5.5,100,   5,  0,  0, -1,   //tread    6
  115,6.1f, 85, 7.5,7.5,7.5, -1,   //shocker  7
	   90,5,   95,   7,  7,  7,  5,   //tryder   8
  100,5.5, 40,   8,  8,  8, -1,   //slideout 9
  120,10,  75, 6.5,6.5,6.5,  3,   //surfcrat 10
	  105,10,  35,   6,  6,  6,  1,   //stepper  11
	  120,10, 100,  10, 10, 10, -1,   //radar    12
};
#define NUMCOINS 10
int basecoins;
int piececoins;
struct COIN {
	TREE *t;
	VEC pos;
};

struct COINPRIORITY {
	int priority;
	char name[32];
};
int numcoinprioritys;

#define EXITSTATE 3 // -1
#define FINISHSTATE 3 // 10
void douburst(int numbursts);
void dolightning(int numlightnings);
void doblueflash(int numflashes);
void douflames(int numflames);

struct COIN coinlist[NUMCOINS];
struct COINPRIORITY coinpriority[MAXPIECES];
char coinpris[NUMBODS];
int ncoins, ncoinscaught;
float coincoldist;
int clocktickcount;
static int framecount;
extern int validroadnorm;

/*void fpuexceptions(int on)
{
	int cw;
// Get the default control word.
    cw= _controlfp( 0,0 );
// Set the exception bits ON.
	if (on) {
//	   cw &=~(0);
	   _clearfp();
	   cw &=~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	} else
	   cw |=(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
//	   cw |=(0);
// Set the control word.
   _controlfp( cw, MCW_EM );
}
*/



//#include "tagread.h"

// tags
static struct tag *gardentags;
// motor
static struct soundhandle *motorsh;
//static float motorpan=0,motorfreq=1,motorvol=.5; // in user33.c
static void inittags()
{
	pushandsetdir("st2_game");
	gardentags = tagread("garden.tag");
	popdir();
}

void playatagsound(int tagidx)
{
	playasoundvol(taggetwhbyidx(gardentags, tagidx), 1.0f);
}
// UNC , uncomment
#if 1

/* motor stuff
	if (KEY=='m')
		if (motorsh) {
			sound_free(motorsh);
			motorsh=NULL;
		} else
			motorsh=wave_play(taggetwhbyname(derbytags,"eng05.wav"),0);
	if (motorsh) {
		setsfxpan(motorsh,motorpan);
		setsfxfreq(motorsh,motorfreq);
		setsfxvolume(motorsh,motorvol);
	}
*/

static void exittags()
{
	tagfree(gardentags);
	gardentags = NULL;
	motorsh = NULL;
}


static float normangrad(float ang)
{
	if (ang > TWOPI)
		ang -= TWOPI;
	else if (ang < 0.0f)
		ang += TWOPI;
	return ang;
}

static float normang(float ang)
{
	if (ang > 360)
		ang -= 360;
	else if (ang < 0)
		ang += 360;
	return ang;
}

// get upto NUMCOINS in TREE *coinlist, using a priority scheme
void initcoins()
{
	TREE *c;
	//char **script;
	//int npages;
	int i, j; //,nc;
	char pceidx[NUMBODS], nodel[NUMBODS]; //,baseidx[NUMBODS];
	int ncoinsatpri;
	int totcoins;
	int p;
	int nc;
	ncoins = ncoinscaught = 0;
	//	dorotvels();
	srand(1234);
	pushandsetdir("st2_edpieces");
	//script = loadscript("coinpriority.txt", &npages);
	script sc = script("coinpriority.txt");
	int npages = sc.num();
	if (npages & 1)
		errorexit("bad coinpriority.txt");
	numcoinprioritys = npages / 2;
	for (i = 0; i < numcoinprioritys; i++) {
		strcpy(coinpriority[i].name, sc.idx(2 * i).c_str());
		coinpriority[i].priority = atoi(sc.idx(2 * i + 1).c_str());
	}
	//freescript(script, npages);
	popdir();
	// assign priorities to the pieces 20 max, 0 min, -1 none
	memset(coinpris, -1, sizeof(coinpris));
	totcoins = 0;
	for (i = 0; i < NUMBODS; i++) {
		p = thetrack[i].piece;
		//c = findtreenamerec(thetrack[i].t, "coin.lwo");
		c = thetrack[i].t->find("coin.lwo");
		if (c) { // coin on this piece
			totcoins++;
			coinpris[i] = 2; // at least gets a 2 for priority
			for (j = 0; j < numcoinprioritys; j++)
				if (!my_stricmp(pieces[p].name, coinpriority[j].name))
					coinpris[i] = coinpriority[j].priority;
		}
	}
	for (i = 20; i >= 0; i--) {
		ncoinsatpri = 0;
		for (j = 0; j < NUMBODS; j++)
			if (coinpris[j] == i) {
				nodel[ncoinsatpri] = 0;
				pceidx[ncoinsatpri] = j;
				ncoinsatpri++;
			}
		nc = NUMCOINS - ncoins; // coins left to place
		if (nc > totcoins)
			nc = totcoins;
		if (ncoinsatpri <= nc) { // all coins at priority will be kept
			for (j = 0; j < NUMBODS; j++) {
				if (coinpris[j] == i) {
					//c = findtreenamerec(thetrack[j].t, "coin.lwo");
					c = thetrack[j].t->find("coin.lwo");
					coinlist[ncoins].t = c;
					obj2world(c, &zerov, &coinlist[ncoins++].pos);
				}
			}
		} else if (nc > 0) { // coins must be randomly kept, and some deleted
			while (nc) {
				p = mt_random(ncoinsatpri);
				if (nodel[p] == 0) {
					nodel[p] = 1;
					nc--;
				}
			}
			for (j = 0; j < ncoinsatpri; j++) {
				p = pceidx[j];
				//c = findtreenamerec(thetrack[p].t, "coin.lwo");
				c = thetrack[p].t->find("coin.lwo");
				if (nodel[j]) {
					coinlist[ncoins].t = c;
					obj2world(c, &zerov, &coinlist[ncoins++].pos);
				} else
					freetree(c);
			}
		} else { // low priority coins all get deleted
			for (j = 0; j < NUMBODS; j++) {
				if (coinpris[j] == i) {
					//c = findtreenamerec(thetrack[j].t, "coin.lwo");
					c = thetrack[j].t->find("coin.lwo");
					freetree(c);
				}
			}
		}
		totcoins -= ncoinsatpri;
	}
}
// get upto NUMCOINS in TREE *coinlist
/*
void initcoins()
{
	TREE *c;
	int i,j,nc;
	char hascoins[NUMBODS]; // if 1, is a piececoin, 2 is a basecoin
	char pieceidx[NUMBODS],baseidx[NUMBODS];
	char coinidx[NUMCOINS];
	int p;
	char b[64];
	piececoins=0;
	basecoins=0;
	ncoins=ncoinscaught=0;
	nc=0;
//	randomize();
	srand(1234);
// first find out which pieces have coins on them
	for (i=0;i<NUMBODS;i++) {
		p = thetrack[i].piece;
		getname(b,pieces[p].piece->name);
		c=findtreebodynamerec(thetrack[i].t,"coin.lwo");
		if (c) {
			if (!stricmp(b,"base")) {
				hascoins[i]=2; // coin on a base piece
				baseidx[basecoins]=i;
				basecoins++;
			} else {
				hascoins[i]=1; // coin on a normal piece
				pieceidx[piececoins]=i;
				piececoins++;
			}
		} else {
			hascoins[i]=0; // no coins on this piece
		}
	}
	nc=min(NUMCOINS,piececoins+basecoins); // total # of coins found on track
// move coins from pieces and bases to coinidx
	for (i=0;i<nc;i++) {
		if (piececoins) {
			j=random(piececoins);
			coinidx[ncoins]=pieceidx[j];
			piececoins--;
			pieceidx[j]=pieceidx[piececoins];
			ncoins++;
		} else if (basecoins) { // no coins left on pieces, use base.lwo
			j=random(basecoins);
			coinidx[ncoins]=baseidx[j];
			basecoins--;
			baseidx[j]=baseidx[basecoins];
			ncoins++;
		}
	}
// delete coins that are left in the base and piece coin list
	for (i=0;i<piececoins;i++) {
		p=pieceidx[i];
		p = thetrack[p].piece;
		c=findtreebodynamerec(thetrack[pieceidx[i]].t,"coin.lwo");
		if (c)
			freetree(c);
	}
	for (i=0;i<basecoins;i++) {
		p=baseidx[i];
		p = thetrack[p].piece;
		c=findtreebodynamerec(thetrack[baseidx[i]].t,"coin.lwo");
		if (c)
			freetree(c);
	}
// buildup coinlist array from coinidx
	dorotvels();
	for (i=0;i<ncoins;i++) {
		p=coinidx[i];
		j = thetrack[p].piece;
		c=findtreebodynamerec(thetrack[p].t,"coin.lwo");
		coinlist[i].t=c;
		// precalculate coin world positions for gross collisions (since they don't move)
		coinlist[i].pos=obj2world(c,zerov);
	}
}
*/
void coincollisions()
{
	//extern void docondom();
	int i;
	modelb *b;
	TREE *t;
	float distsq, bestdist;
	int bestcoin = -1;
	if (dofinish)
		return;
	for (i = 0; i < ncoins; i++) {
		if (coinlist[i].t) {
			distsq = dist3dsq(&coinlist[i].pos, &objects[0].pos);
			if (bestcoin == -1 || distsq < bestdist) {
				bestcoin = i;
				bestdist = distsq;
			}
			if (distsq < coincoldist*coincoldist) { // gross collision
				t = coinlist[i].t;
				b = t->mod;
				if (b) {
					//					VEC lowermin;
					//					b=&bodies[bid];
										// finer collision
					//					lowermin.x=carbboxmin.x;
					//					lowermin.y=carbboxmin.y-(carbboxmax.y-carbboxmin.y); // lower belly for coin coll
					//					lowermin.z=carbboxmin.z;
					//					if (colbox2colbox(	carnull,&lowermin,&carbboxmax,
					//										t,&b->boxmin,&b->boxmax)) {
					freetree(coinlist[i].t);
					coinlist[i].t = NULL;
					ncoinscaught++;
					clocktickcount += 2 * 30 * 2;
					playatagsound(ZING2);
					if (clocktickcount > ustarttime * 30 * 2)
						clocktickcount = ustarttime * 30 * 2;
					if (ncoinscaught < ncoins) {
						playatagsound(REDLIGHT); // caught a coin
//							douburst(1);
						docondom(5);

					} else {
						playatagsound(GREENLIGHT); // caught all coins
						douburst(3);
						docondom(15);
					}
					//					}
				}
			}
		}
	}
	if (bestcoin >= 0) {
		i = bestcoin;
		targetradarangle = (float)
			atan2(coinlist[i].pos.x - objects[0].pos.x, coinlist[i].pos.z - objects[0].pos.z);
		//		targetradarangle*=PIUNDER180;
		targetradarangle = normangrad(targetradarangle + (PI / 2.0f));
	} else {
		targetradarangle = -1;
	}
}
// UNC
#endif
struct dembone dembones[COLBONESX][COLBONESZ] = {
	{
		{"colbone6",{1,0,7}},
		{"colbone3",{4,0,0}},
		{"colbone2",{1,0,-9}},
	},
	{
		{"colbone5",{-1,0,10}},
		{"colbone4",{-7,0,0}},
		{"colbone1",{-1,0,-9}},
	}
};
// UNC
#if 1
void loadcustomcarsettings()
{
	char carname[50];
	mgetname(carname, allcars[selectedcar].carname);
	strcat(carname, ".txt");
	pushandsetdir("config");
	if (fileexist(carname)) {
		logger("loading custom car config file '%s'\n", carname);
		pushandsetdir("config");
		loadconfigfile(carname);
		popdir();
	} else {
		logger("custom car config file '%s' not found\n", carname);
	}
	popdir();
}

static char burstnames[][32] =
{
 "burstb.tsp",
 "burstg.tsp",
 "burstr.tsp",
 "burst.tsp"
};

static char burstlwo[][32] =
{
 "burstb.lwo",
 "burstg.lwo",
 "burstr.lwo",
 "burst.lwo"
};

static char burstmats[][32] =
{
 "burstb",
 "burstg",
 "burstr",
 "burst"
};

VEC uflame1pos, uflame1scale = { 1,1,1 }, uflame2pos, uflame2scale = { 1,1,1 };
//VEC uflame3pos,uflame3scale={1,1,1},uflame4pos,uflame4scale={1,1,1};
TREE *ubursta[4], *uburstb[4];
float uthiscarpitch, uthiscaryaw, uthiscarroll;
void st2_buildcar()
{
	//	int tempf; //,hflag;
	//	SPRITE *testsp,*testspa;
	TREE *xxx;
	int i, j;
	// build car
	//	while (selectedcar==TREAD || selectedcar==RADAR)
	//		selectedcar=random(13);
	pushandsetdir("st2_cars");
	carnull = alloctree(20, nullname);
	carnull->buildo2p = O2P_FROMTRANSQUATSCALE;
	carcenternull = alloctree(1, nullname);
	carbody = loadlws(allcars[selectedcar].carname);
	linkchildtoparent(carbody, carcenternull);
	linkchildtoparent(carcenternull, carnull);
	linkchildtoparent(carnull, root);
	xxx = findtreename(root, allcars[selectedcar].mainobject);
	/*if (xxx->mod)
		xxx->mod->drawpri = 1;*/
	uaxles[0] = findtreename(root, "frntl");
	uaxles[1] = findtreename(root, "frntr");
	uaxles[2] = findtreename(root, "backl");
	uaxles[3] = findtreename(root, "backr");
	// find car bones
	for (j = 0; j < COLBONESX*COLBONESZ; j++) {
		dembones[0][j].t = findtreename(carbody, dembones[0][j].name);
		if (!dembones[0][j].t)
			errorexit("can't find bone %s", dembones[0][j].name);
		dembones[0][j].curpushin = zerov;
	}
	// handle special (lightning around car)
	uplayspecial1 = findtreename(root, allcars[selectedcar].spname);
	if (!uplayspecial1)
		error("Error can't find %s", allcars[selectedcar].spname);
	uplayspecial1->flags |= TF_DONTDRAW;

	uplayspecial2 = findtreename(root, allcars[selectedcar].coinsp);
	if (!uplayspecial2)
		error("Error can't find %s", allcars[selectedcar].coinsp);
	uplayspecial2->flags |= TF_DONTDRAW;

	pushandsetdir("st2_game");
	uplayflame1 = alloctree(0, "flame.lwo");
	uplayflame2 = alloctree(0, "flamflip.lwo");
	uplayflame1->treedissolvecutoff = 5 / 256.0f;
	uplayflame2->treedissolvecutoff = 5 / 256.0f;
	uplayflame1->flags |= TF_DONTDRAW;
	uplayflame2->flags |= TF_DONTDRAW;

	if (!slowpo) {
		specialtsp = loadtsp("trkglow.tsp", "trkglowa.tsp", uplayspecial1, allcars[selectedcar].spname, "trkglow2", TSP_USE_1ALPHA, 0);
		/*		uplayspecial1		specialanimtex.nframes = ReadTSP(&testsp,"trkglow.tsp");
		specialanimtex.curframe=0;
		if(specialanimtex.nframes <=0)
			errorexit("Can't find trkglow.tsp");
		tempf = ReadTSP(&testspa,"trkglowa.tsp");
	    if(tempf <=0)
			errorexit("Can't find trkglowa.tsp");
	    if(tempf != specialanimtex.nframes)
			errorexit("trkglow.tsp and trkglowa.tsp have diff # of frames");
	    specialanimtex.mat=findmaterial(uplayspecial1,"trkglow2");
		;//if(!specialanimtex.mat)
		;//	errorexit("mat special is null");
		;//specialanimtex.desttex=gettexptr(specialanimtex.mat,&specialanimtex.w,&specialanimtex.h,NULL);
		logger("building up car lightning effect\n");
		for(i=0;i<specialanimtex.nframes;i++) {
			;superconvert24to16((unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
				(unsigned char *)testspa[i].data,testspa[i].w,testspa[i].h,
				SU_COLORKEY|SU_NEW,
				&specialanimtex.texdata[i],&specialanimtex.w,&specialanimtex.h,
				&specialanimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures
//				NULL,&hflags,1);
		}
	    FreeTSP(testsp,tempf);
		FreeTSP(testspa,tempf);
*/
	}
	// car fireworks
	burstframe = 0;
	for (j = 0; j < 4; j++) {
		ubursta[j] = alloctree(1, burstlwo[j]);
		bursttsp[j] = loadtsp(burstnames[j], "bursta.tsp", ubursta[j], burstlwo[j], burstmats[j], TSP_USE_1ALPHA, 0);
		/*		burstanimtex[j].nframes = ReadTSP(&testsp,burstnames[j]);
		burstanimtex[j].curframe=0;
		if(burstanimtex[j].nframes <=0)
			errorexit("Can't find burst.tsp");
		tempf = ReadTSP(&testspa,"bursta.tsp");
		if(tempf <=0)
			errorexit("Can't find bursta.tsp");
		if(tempf != burstanimtex[j].nframes)
			errorexit("burst.tsp and bursta.tsp have diff # of frames");
		logger("building up car burst effect\n");
		burstanimtex[j].mat = findmaterial(ubursta[j],burstmats[j]);
		;//if(!burstanimtex[j].mat)
		;//	errorexit("Can't find burst surface");
		;//burstanimtex[j].desttex=gettexptr(burstanimtex[j].mat,&burstanimtex[j].w,&burstanimtex[j].h,NULL);
		;//if(!burstanimtex[j].desttex)
		;//	errorexit("ouch *burstdata is null");
		for(i=0;i<burstanimtex[j].nframes;i++) {
			;superconvert24to16((unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
				(unsigned char *)testspa[i].data,testspa[i].w,testspa[i].h,
				SU_COLORKEY|SU_NEW,
				&burstanimtex[j].texdata[i],&burstanimtex[j].w,&burstanimtex[j].h,
				&burstanimtex[j].mat->mhflags,NULL,1); // take advantage of fasteropaquetextures
	//			NULL,&hflags,1);
		}
		FreeTSP(testsp,tempf);
		FreeTSP(testspa,tempf);
*/		ubursta[j]->scale.x=.01f;
		ubursta[j]->scale.y = .01f;
		ubursta[j]->scale.z = .01f;
		//	linkchildtoparent(uburst,carnull);
		//	uburst->trans.y+=.04f;
		//	uburst->rot.x=-90*PIOVER180;
		ubursta[j]->trans.z = .14f;
		uburstb[j] = duptree(ubursta[j]);
		uburstb[j]->rot.y = 180 * PIOVER180;
	}
	//	uburst->rot.x=90*PIOVER180;
	//	linkchildtoparent(uburst,carnull);
	// handle different carbody textures
	//	pushandsetdir("cars");

	condomtsp = loadtsp("wipe.tsp", "wipea.tsp", root, allcars[selectedcar].coinsp, "flash", TSP_USE_1ALPHA, 0);
	/*	goldencondomanimtex.nframes = ReadTSP(&testsp,"wipe.tsp");
		ReadTSP(&testspa,"wipea.tsp");
		xxx=findtreename(&mainvp,allcars[selectedcar].coinsp);
		goldencondomanimtex.mat=findmaterial(xxx,"flash");
		;//if(!goldencondomanimtex.mat)
		;//	errorexit("mat goldencondom is null");
		;//goldencondomanimtex.desttex=gettexptr(goldencondomanimtex.mat,&goldencondomanimtex.w,&goldencondomanimtex.h,NULL);
		logger("building up goldencondom textures (coin car)\n");
		for(i=0;i<goldencondomanimtex.nframes;i++)
			;superconvert24to16(
				(unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
				(unsigned char *)testspa[i].data,testspa[i].w,testspa[i].h,
				SU_COLORKEY|SU_NEW,&goldencondomanimtex.texdata[i],&goldencondomanimtex.w,&goldencondomanimtex.h,
	//			NULL,&hflags,1);
				&goldencondomanimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures
		FreeTSP(testsp,goldencondomanimtex.nframes);
		FreeTSP(testspa,goldencondomanimtex.nframes);
		logger("condom had %d frames\n",goldencondomanimtex.nframes);
	*/
	flametsp = loadtsp("flame.tsp", "flamea.tsp", uplayflame1, "flame.lwo", "flame", TSP_USE_1ALPHA, 0);
	{
/*	CUT	struct mat *mt;
		mt = findmaterial(uplayflame2, "flamflip");
		freetexture(mt->thetex);
		mt->thetex = flametsp->atex;
		mt->thetex->refcount++; */
	}
	/*
		flameranimtex.nframes = ReadTSP(&testsp,"flame.tsp");
		ReadTSP(&testspa,"flamea.tsp");
		flameranimtex.mat=findmaterial(uplayflame1,"flame");
		;//if(!flameranimtex.mat)
		;//	errorexit("mat flamer is null");
		;//flameranimtex.desttex=gettexptr(flameranimtex.mat,&flameranimtex.w,&flameranimtex.h,NULL);
		logger("building up flamer textures (flame car)\n");
		for(i=0;i<flameranimtex.nframes;i++)
			;superconvert24to16(
				(unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
				(unsigned char *)testspa[i].data,testspa[i].w,testspa[i].h,
				SU_COLORKEY|SU_NEW,&flameranimtex.texdata[i],&flameranimtex.w,&flameranimtex.h,
	//			NULL,&hflags,1);
				&flameranimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures
		FreeTSP(testsp,flameranimtex.nframes);
		FreeTSP(testspa,flameranimtex.nframes);
		logger("flamer had %d frames\n",flameranimtex.nframes);
	*/
	popdir();

	flashtsp = loadtsp(allcars[selectedcar].tspname, NULL, root,
		allcars[selectedcar].mainobject, allcars[selectedcar].surfname, TSP_USE_1ALPHA, 0);
	nuflashes = 1;	// force texture
/*	carbodyanimtex.nframes = ReadTSP(&testsp,allcars[selectedcar].tspname);
	carbodyanimtex.curframe=0;
	carbodyanimtex.lastframe=-1;
	if(carbodyanimtex.nframes <=0)
		error("Can't find %s",allcars[selectedcar].tspname);
	xxx=findtreename(&mainvp,allcars[selectedcar].mainobject);
	carbodyanimtex.mat=findmaterial(xxx,allcars[selectedcar].surfname);
	;//if(!carbodyanimtex.mat)
	;//	errorexit("mat carbody is null");
	;//carbodyanimtex.desttex=gettexptr(carbodyanimtex.mat,&carbodyanimtex.w,&carbodyanimtex.h,NULL);
	logger("building up car textures (blue car)\n");
	for(i=0;i<carbodyanimtex.nframes;i++)
		;superconvert24to16(
			(unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
			NULL,0,0,
			SU_COLORKEY|SU_NEW,&carbodyanimtex.texdata[i],&carbodyanimtex.w,&carbodyanimtex.h,
//			NULL,&hflags,1);
			&carbodyanimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures
	FreeTSP(testsp,carbodyanimtex.nframes); */
	//	popdir();

	// build wheels
	if (selectedcar == TREAD)
		for (i = 0; i < 4; i++) {
			wheels[i] = alloctree(1, NULL);
			wheels[i]->buildo2p = O2P_FROMTRANSQUATSCALE;
			linkchildtoparent(wheels[i], carnull);
		}
	else
		for (i = 0; i < carinfo[selectedcar].nwheels; i++) {
			wheels[i] = findtreename(root, carinfo[selectedcar].wheelnames[i]);
			freetree(wheels[i]);
			wheels[i] = alloctree(1, carinfo[selectedcar].wheelnames[i]);
			wheels[i]->buildo2p = O2P_FROMTRANSQUATSCALE;
			linkchildtoparent(wheels[i], carnull);
		}
	linkchildtoparent(uplayflame1, uaxles[2]);
	linkchildtoparent(uplayflame2, uaxles[3]);
	//	uflameonback=1;
	if (selectedcar == RADAR)
		radartree = findtreename(root, "Radar Dish Null");
	else if (selectedcar == TREAD) {
		//		pushandsetdir("cars");
		/*		treadanimtex.nframes = ReadTSP(&testsp,"treadwhl.tsp");
				treadanimtex.curframe=0;
				treadanimtex.lastframe=-1;
				if(treadanimtex.nframes <=0)
					error("Can't find %s","treadwhl.tsp");
				xxx=findtreename(&mainvp,allcars[selectedcar].mainobject);
				treadanimtex.mat=findmaterial(xxx,"treadwhl");
				;//if(!treadanimtex.mat)
				;//	errorexit("mat tread is null");
				;//treadanimtex.desttex=gettexptr(treadanimtex.mat,&treadanimtex.w,&treadanimtex.h,NULL);
				logger("building up car textures (blue car)\n");
				for(i=0;i<treadanimtex.nframes;i++)
					;superconvert24to16(
						(unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
						NULL,0,0,
						SU_COLORKEY|SU_NEW,
						&treadanimtex.texdata[i],&treadanimtex.w,&treadanimtex.h,
		//				NULL,&hflags,1);
						&treadanimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures
				FreeTSP(testsp,treadanimtex.nframes);
		//		popdir();
		*/
	}

	// build shadow
	//	popdir();
	//	pushandsetdir("cars");
	shadownull = alloctree(1, nullname);
	shadownull->buildo2p = O2P_FROMTRANSQUATSCALE;
	shadowb = alloctree(1, nullname);
	shadowb->buildo2p = O2P_FROMTRANSQUATSCALE;
	shadowb->rot.w = 1;
	shadowbody = alloctree(0, allcars[selectedcar].shadowname);
	shadowbody->treedissolvecutoff = 10 / 256.0f;
	//	bodies[shadowbody->bodyid].mat->mhflags|=MHF_HASNOZBUFF;
	shadowbody->buildo2p = O2P_FROMTRANSQUATSCALE;
	// CUT shadowbody->flags |= TF_CALCLIGHTSONCE;
	linkchildtoparent(shadowbody, shadowb);
	linkchildtoparent(shadowb, shadownull);
	linkchildtoparent(shadownull, root);
	popdir();
	uthiscarpitch = carratings[selectedcar].pitchspeed / 3.5f;
	uthiscaryaw = carratings[selectedcar].yawspeed / 3.5f;
	uthiscarroll = carratings[selectedcar].rollspeed / 3.5f;
	loadcustomcarsettings();
}

// helper objects
#ifdef USETESTNULLS
void setanullpos(VEC *p, int i, int rel)
{
	if (i >= NNULLS2)
		return;
	if (rel) {
		uplaynulls[i]->trans.x = p->x + objects[0].pos.x;
		uplaynulls[i]->trans.y = p->y + objects[0].pos.y;
		uplaynulls[i]->trans.z = p->z + objects[0].pos.z;
	} else
		uplaynulls[i]->trans = *p;
}

void setanullrot(VEC *p, int i)
{
	if (i >= NNULLS2)
		return;
	uplaynulls[i]->rot = *p;
}

void initnulls() // only init after an lwsread so we can find 'abuiltnull'
{
	int i;
	VEC nv;
	VEC q = zerov;
	q.w = 1;
	for (i = 0; i < NNULLS2; i++) {
		uplaynulls[i] = alloctree(0, nullname);
		uplaynulls[i]->buildo2p = O2P_FROMTRANSQUATSCALE;
		linkchildtoparent(uplaynulls[i], root);
		nv.x = 0;
		nv.y = i + 10.0f;
		nv.z = 0;
		setanullpos(&nv, i, 0);
		setanullrot(&q, i);
	}
}
#endif

//int loadtracklist2(char *filename);

int st2_loadtracklist2(char *filename)
{
	int i = 0, j;
	char temp[64];
	//int numl;
	//char **script;
	int nump = 0;
	script sc(filename);
	//= loadscript(filename, &numl);
	while (i < sc.num()) {
		j = 0;
		strcpy(pieces[nump].name, sc.idx(i++).c_str()); // grab piece
		pieces[nump].piece = NULL;
		while (1) {
			strcpy(temp, sc.idx(i++).c_str()); // grab changerot OR lwoname
			if (temp[1] == '\0' && temp[0] >= '0' && temp[0] <= '9') {
				pieces[nump].changerot = atoi(temp);
				pieces[nump].nlwos = j;
				break;
			} else {
				strcpy(pieces[nump].lwonames[j], temp);
				logger("extra '%s'\n", temp);
				if (j == NCOLLLWOS)
					errorexit("too many lwonames");
				j++;
			}
		}
		pieces[nump].newor = atoi(sc.idx(i++).c_str());
		nump++;
	}
	//freescript(script, numl);
	return nump;
}

// UNC
#endif

// init
static TREE *scrline;
int utotalcrashes, utimeleft, ucoinscaught, utotalstunts;
int ulastpiece, udirpiece;
struct fs {
	int ang, xoff, zoff;
};
struct fs finishstruct[4] = {
	  0,10, 0,
	 90, 0,-10,
	180,-10, 0,
	270, 0, 10 };
struct fs crashresetstruct[4] = {
	  0, 5, 0,
	 90, 0,-5,
	180,-5, 0,
	270, 0, 5 };

int pieceorder[50];
int curpieceorder;
VEC crashresetloc;
float crashresetdir;
int doacrashreset;
int piecesvisited[50];
int numpiecesvisited;
int numpiecestovisit;

void canfinish(int yo)
{
	//	finishpiece->visible=yo;
	if (yo)
		nofinishpiece->flags |= TF_DONTDRAW;
	else
		nofinishpiece->flags &= ~TF_DONTDRAW;
}
// UNC
#if 1

void initvisit()
{
	int i;
	memset(piecesvisited, 0, sizeof(piecesvisited));
	numpiecesvisited = 0;
	numpiecestovisit = 0;
	for (i = 0; i < NUMBODS; i++)
		if (thetrack[i].piece != PIECE_BASE)
			numpiecestovisit++;
	if (numpiecestovisit)
		canfinish(0);
	else
		canfinish(1);
}
// UNC
#endif
void addvisit(int x, int z)
{
	int cp, pce;
	cp = x * 7 + z;
	pce = thetrack[cp].piece;
	if (pce == PIECE_GENERIC_4 || pce == PIECE_GENERIC_24)
		intunnels = 15;
	if (pce != PIECE_BASE && piecesvisited[cp] == 0) {
		piecesvisited[cp] = 1;
		numpiecesvisited++;
		if (numpiecesvisited * 100 >= finishpercent * numpiecestovisit)
			canfinish(1);
	}
}

void setnextcrashloc(int x, int z)
{
	int	cp = x * 7 + z;
	int orhey;
	addvisit(x, z);
	if (pieceorder[curpieceorder] == cp) {
		orhey = (int)thetrack[cp]. orhey ;
		crashresetloc.x = (float)(x * 10 - 30 - crashresetstruct[(int)orhey/90].xoff);
		crashresetloc.y = .75f;
		crashresetloc.z = (float)(30 - z * 10 - crashresetstruct[(int)orhey/90].zoff);
		crashresetdir = (float)((orhey +90)*PIOVER180);
		//		playatagsound(FIREBURST);
		curpieceorder++;
	}
}
// UNC
#if 1
void scantrackpieces()
{
	int bail = 0;
	int curdir = 0;
	int pl = 0;
	int xoff, zoff, cp, pce;
	// setup finish line
	ulastpiece = st2_lastpiece;
	udirpiece = st2_dir;
	if (ulastpiece == -1 || thetrack[6].piece == PIECE_BASE || thetrack[6].piece == PIECE_GRID) {
		udirpiece = 0;
		ulastpiece = 6;
	}
	startpiece->trans.x = -35;
	startpiece->trans.z = -30;
	startpiece->rot.y = 90 * PIOVER180;

	xoff = ulastpiece / 7;
	zoff = ulastpiece % 7;
	xoff = 10 * xoff - 30;
	zoff = 30 - zoff * 10;
	finishpiece->trans.x = (float)(xoff + finishstruct[udirpiece].xoff);
	finishpiece->trans.z = (float)(zoff + finishstruct[udirpiece].zoff);
	finishpiece->rot.y = (float)((90 + finishstruct[udirpiece].ang)*PIOVER180);
	//	nofinishpiece->trans=finishpiece->trans;
	//	nofinishpiece->rot=finishpiece->rot;
	initvisit();
	// build ordered track piece list
	xoff = 0;
	zoff = 6;
	cp = xoff * 7 + zoff;
	curpieceorder = 0;
	doacrashreset = 0;
	crashresetloc.x = -35;
	crashresetloc.y = 0;
	crashresetloc.z = -30;
	crashresetdir = 90 * PIOVER180;
	if (st2_lastpiece != -1 && thetrack[6].piece != PIECE_BASE && thetrack[6].piece != PIECE_GRID)
		do {
			cp = xoff * 7 + zoff;
			logger("pieceorder %d = %d\n", pl, cp);
			pieceorder[pl++] = cp;
			pce = thetrack[cp].piece;
			if (pieces[pce].changerot) {
				if (pieces[pce].newor == 1)
					curdir--;
				if (pieces[pce].newor == 2)
					curdir++;
			}
			curdir &= 3;
			switch (curdir) {
			case 0:	xoff++;
				break;
			case 1:	zoff++;
				break;
			case 2:	xoff--;
				break;
			case 3:	zoff--;
				break;
			}
			bail++;
			if (bail > 55) { // safety
				logger("bailed\n");
				break;
			}
		} while (cp != ulastpiece);
		logger("pieceorder %d = %d\n", pl, -1);
		pieceorder[pl] = -1;	// sentinel
	/*	{
		int pce;
		char a[64];
		TREE *aaa;
		switch (udirpiece) {
		case 0: pce=ulastpiece+7;
				break;
		case 1: pce=ulastpiece+1;
				break;
		case 2: pce=ulastpiece-7;
				break;
		case 3: pce=ulastpiece-1;
				break;
		}
		sprintf(a,"%02d",pce);
		aaa = findtreebodyname(a);
		aaa = findtreebodynamerec(aaa,"base.lwo");
		aaa->visible=0;
	//	linkchildtoparent(thetrack[i].t,aaa);
		} */
}

void kill_bcams(TREE *t)
{
	//U32 i;
	//for (i = 0; i < t->children.size(); i++)
	//	kill_bcams(t->children[i]);
	for (auto it : t->children)
	if (!strcmp(it->name.c_str(), "lwscamera"))
		freetree(it);
}
// UNC
#endif

	void getgridxz(COLLGRID *gc, VEC *v, int *x, int *z)
	{
		*x = (int)(gc->mulx*v->x + gc->offx);
		*z = (int)(gc->mulz*v->z + gc->offz);
		//#define EXTRAGRIDCHECK
#ifdef EXTRAGRIDCHECK
		if (*x < 0 || *x >= ST2_COLLGRIDX || *z < 0 || *z >= ST2_COLLGRIDZ)
			errorexit("grid outa bounds %d %d\n", *x, *z);
#endif
	}
// UNC
#if 1
void getvecfromgridxz(COLLGRID *gc, VEC *v, int x, int z)
{
	v->x = ((float)x - gc->offx) / gc->mulx;
	v->z = ((float)z - gc->offz) / gc->mulz;
}

int tri2pointxz(VEC *t0, VEC *t1, VEC *t2, VEC *p)
{
	VEC r0, r1, r2;
	float c0, c1, c2;
	r0.x = t0->x - p->x;
	r0.z = t0->z - p->z;
	r1.x = t1->x - p->x;
	r1.z = t1->z - p->z;
	r2.x = t2->x - p->x;
	r2.z = t2->z - p->z;
	c0 = r0.x*r1.z - r0.z*r1.x;
	c1 = r1.x*r2.z - r1.z*r2.x;
	c2 = r2.x*r0.z - r2.z*r0.x;
	if (c0 >= 0 && c1 >= 0 && c2 >= 0)
		return 1;
	if (c0 <= 0 && c1 <= 0 && c2 <= 0)
		return 1;
	return 0;
}

// only check xz
int tri2rectxz(VEC *t0, VEC *t1, VEC *t2, VEC *b0, VEC *b1)
{
	VEC rp;
	float ix, iz, del;
	// see if any tripoints inside rectangle
	if (t0->x >= b0->x && t0->x <= b1->x && t0->z >= b0->z && t0->z <= b1->z)
		return 1;
	if (t1->x >= b0->x && t1->x <= b1->x && t1->z >= b0->z && t1->z <= b1->z)
		return 1;
	if (t2->x >= b0->x && t2->x <= b1->x && t2->z >= b0->z && t2->z <= b1->z)
		return 1;
	// see if any rectpoints inside triangle
	if (tri2pointxz(t0, t1, t2, b0))
		return 1;
	if (tri2pointxz(t0, t1, t2, b1))
		return 1;
	rp.x = b0->x;
	rp.z = b1->z;
	if (tri2pointxz(t0, t1, t2, &rp))
		return 1;
	rp.x = b1->x;
	rp.z = b0->z;
	if (tri2pointxz(t0, t1, t2, &rp))
		return 1;
	// now check all intersections of rect to tri
	//t0-t1 z
	del = t0->z - t1->z;
	if (del<-EPSILON || del>EPSILON) {
		ix = t0->x + (t1->x - t0->x)*(b0->z - t0->z) / (t1->z - t0->z);
		if (ix >= b0->x && ix <= b1->x)
			return 1;
		ix = t0->x + (t1->x - t0->x)*(b1->z - t0->z) / (t1->z - t0->z);
		if (ix >= b0->x && ix <= b1->x)
			return 1;
	}
	//t0-t1 x
	del = t0->x - t1->x;
	if (del<-EPSILON || del>EPSILON) {
		iz = t0->z + (t1->z - t0->z)*(b0->x - t0->x) / (t1->x - t0->x);
		if (iz >= b0->z && iz <= b1->z)
			return 1;
		iz = t0->z + (t1->z - t0->z)*(b1->x - t0->x) / (t1->x - t0->x);
		if (iz >= b0->z && iz <= b1->z)
			return 1;
	}
	//t1-t2 x
	del = t1->z - t2->z;
	if (del<-EPSILON || del>EPSILON) {
		ix = t1->x + (t2->x - t1->x)*(b0->z - t1->z) / (t2->z - t1->z);
		if (ix >= b0->x && ix <= b1->x)
			return 1;
		ix = t1->x + (t2->x - t1->x)*(b1->z - t1->z) / (t2->z - t1->z);
		if (ix >= b0->x && ix <= b1->x)
			return 1;
	}
	//t1-t2 z
	del = t1->x - t2->x;
	if (del<-EPSILON || del>EPSILON) {
		iz = t1->z + (t2->z - t1->z)*(b0->x - t1->x) / (t2->x - t1->x);
		if (iz >= b0->z && iz <= b1->z)
			return 1;
		iz = t1->z + (t2->z - t1->z)*(b1->x - t1->x) / (t2->x - t1->x);
		if (iz >= b0->z && iz <= b1->z)
			return 1;
	}
	//t2-t0 x
	del = t2->z - t0->z;
	if (del<-EPSILON || del>EPSILON) {
		ix = t2->x + (t0->x - t2->x)*(b0->z - t2->z) / (t0->z - t2->z);
		if (ix >= b0->x && ix <= b1->x)
			return 1;
		ix = t2->x + (t0->x - t2->x)*(b1->z - t2->z) / (t0->z - t2->z);
		if (ix >= b0->x && ix <= b1->x)
			return 1;
	}
	//t2-t0 z
	del = t2->x - t0->x;
	if (del<-EPSILON || del>EPSILON) {
		iz = t2->z + (t0->z - t2->z)*(b0->x - t2->x) / (t0->x - t2->x);
		if (iz >= b0->z && iz <= b1->z)
			return 1;
		iz = t2->z + (t0->z - t2->z)*(b1->x - t2->x) / (t0->x - t2->x);
		if (iz >= b0->z && iz <= b1->z)
			return 1;
	}
	return 0;
}

void setupmatfuncs()
{
	int i, j, k, el;
	modelb *b;
	//MAT *m;
	int nmat;
	for (i = 0; i < NUMBODS; i++)
		for (j = 0; j < NCOLLLWOS; j++)
			for (k = 0; k < MAXMATFUNCS; k++)
				thetrack[i].clwos[j].matfuncs[k] = NULL;
	for (i = 0; i < NUMBODS; i++)
		for (j = 0; j < thetrack[i].nlwos; j++) {
			b = thetrack[i].tlwos[j]->mod;
			if (!b)
				errorexit("bodyid of piece < 0");
			//			b=&bodies[k];
			//m = b->mats;
			//nmat = b->nmat;
			nmat = b->mats.size();
			if (nmat > MAXMATFUNCS)
				errorexit("too many mats");
			//			logger("checking '%s'\n",thetrack[i].tlwos[j]->name);
			for (k = 0; k < nmat; k++)
				for (el = 0; el < NMATFUNCS; el++)
					if (!my_stricmp(matfuncs[el].matname, b->mats[k].name.c_str())) {
						//						logger("matfunc found for piece '%s' material '%s'\n",
						//							thetrack[i].tlwos[j]->name,b->surfnames+(k<<4));
						thetrack[i].clwos[j].matfuncs[k] = matfuncs[el].matfunc;
					}
		}
}

/*struct matfunc matfuncs[2]={
	"dirtsurf",spawndirtparts,
	"loopside",usenewcam,
};
#define NMATFUNCS 2
*/
// UNC
#endif
int carboost;
//int alphacutoffsave;
//extern int alphacutoff;
int useoldcam;

/////////////////////// particle stuff here
/*
static void partfunc(TREE *t)
{
	t->user1--;
	if (t->user1==0)
		freetree(t);
}
*/
int particleeffects = 3;
static TREE *udpart, *udirtpart, *udarkdirt, *udarkdirtpart;
int dirtq, sub_dirtq, darkdirtq, sub_darkdirtq;
extern float dirtvbx, dirtvby, dirtvbz;
extern float sub_dirtvbx, sub_dirtvby, sub_dirtvbz;
float darkdirtvbx, darkdirtvby, darkdirtvbz;
float sub_darkdirtvbx, sub_darkdirtvby, sub_darkdirtvbz;
extern int dirtvrandx, dirtvrandy, dirtvrandz;
extern int sub_dirtvrandx, sub_dirtvrandy, sub_dirtvrandz;
int darkdirtvrandx, darkdirtvrandy, darkdirtvrandz;
int sub_darkdirtvrandx, sub_darkdirtvrandy, sub_darkdirtvrandz;
extern float dirtscale, sub_dirtscale, darkdirtscale, sub_darkdirtscale;

#define USCALEDOWN 100.0f
// UNC
#if 1
static bool udirtanim(TREE *a)
{
	a->transvel.y -= 3 / USCALEDOWN;
	a->userint[2]++;
	if (a->userint[2] > a->userint[3])
		freetree(a);
	return true;
}

bool udirtcanim(TREE *a)
{
	a->transvel.y -= 1 / USCALEDOWN;
	a->userint[2]++;
	if (a->userint[2] > a->userint[3])
		freetree(a);
	return true;
}

bool udarkdirtanim(TREE *a)
{
	a->transvel.y -= 2 / USCALEDOWN;
	a->userint[2]++;
	if (a->userint[2] > a->userint[3])
		freetree(a);
	return true;
}
// UNC
#endif
	 //float timeinc;
	//struct object objects[NUMOBJECTS];
void uspawndirt() // no YOU spawndirt!, called when it is time to spawn dirt
{
	JXFORM jx;
	int i;
	int pe;
	int wantp;
	static int ccc = 0;
	TREE *unpart, *axl, *axr;
	if (fabs(accelspin * 4) < startaccelspin)
		return;
	ccc++;
	//	if (usehardware)
	//		pe=particleeffects*uloop;
	//	else
	pe = particleeffects * uloop;
	if (ccc > pe)
		ccc = 0;
	else
		return;
	if (accelspin < 0) {
		axl = wheels[FRONTLEFT];
		axr = wheels[FRONTRIGHT];
		if (dirtvbz >= 0)
			dirtvbz *= -1;
		if (sub_dirtvbz >= 0)
			sub_dirtvbz *= -1;
	} else {
		//		axl = wheels[BACKLEFT];
		//		axr = wheels[BACKRIGHT];
		axl = wheels[FRONTLEFT];
		axr = wheels[FRONTRIGHT];
		dirtvbz = (float)fabs(dirtvbz);
		sub_dirtvbz = (float)fabs(sub_dirtvbz);
	}
	wantp = dirtq;
	quat2xform(&objects[0].carang, &jx);
	for (i = 0; i < wantp; i++) {
		unpart = duptree(udpart);
		unpart->trans = zerov; //objects[0].pos;
		setVEC(&unpart->transvel, -(dirtvbx + mt_random(dirtvrandx)) / USCALEDOWN,
			(dirtvby + mt_random(dirtvrandy)) / USCALEDOWN,
			-(dirtvbz + mt_random(dirtvrandz)) / USCALEDOWN);
		xformvec(&jx, &unpart->transvel, &unpart->transvel);
		unpart->transvel.x += objects[0].carvel.x*timeinc*uloop;
		unpart->transvel.y += objects[0].carvel.y*timeinc*uloop;
		unpart->transvel.z += objects[0].carvel.z*timeinc*uloop;
		obj2world(axl, &unpart->trans, &unpart->trans);
		linkchildtoparent(unpart, root);

		unpart = duptree(udpart);
		unpart->trans = zerov; //objects[0].pos;
		setVEC(&unpart->transvel, (dirtvbx + mt_random(dirtvrandx)) / USCALEDOWN,
			(dirtvby + mt_random(dirtvrandy)) / USCALEDOWN,
			-(dirtvbz + mt_random(dirtvrandz)) / USCALEDOWN);
		xformvec(&jx, &unpart->transvel, &unpart->transvel);
		unpart->transvel.x += objects[0].carvel.x*timeinc*uloop;
		unpart->transvel.y += objects[0].carvel.y*timeinc*uloop;
		unpart->transvel.z += objects[0].carvel.z*timeinc*uloop;
		obj2world(axr, &unpart->trans, &unpart->trans);
		linkchildtoparent(unpart, root);
	}
	wantp = sub_dirtq;
	for (i = 0; i < wantp; i++) {
		if (mt_random(2))
			unpart = duptree(udirtpart);
		else
			unpart = duptree(udarkdirtpart);
		unpart->trans = zerov; //objects[0].pos;
		setVEC(&unpart->transvel, -(sub_dirtvbx + mt_random(sub_dirtvrandx)) / USCALEDOWN,
			(sub_dirtvby + mt_random(sub_dirtvrandy)) / USCALEDOWN,
			-(sub_dirtvbz + mt_random(sub_dirtvrandz)) / USCALEDOWN);
		xformvec(&jx, &unpart->transvel, &unpart->transvel);
		unpart->transvel.x += objects[0].carvel.x*timeinc*uloop;
		unpart->transvel.y += objects[0].carvel.y*timeinc*uloop;
		unpart->transvel.z += objects[0].carvel.z*timeinc*uloop;
		obj2world(wheels[BACKLEFT], &unpart->trans, &unpart->trans);
		linkchildtoparent(unpart, root);

		if (mt_random(2))
			unpart = duptree(udirtpart);
		else
			unpart = duptree(udarkdirtpart);
		unpart->trans = zerov; //objects[0].pos;
		setVEC(&unpart->transvel, (sub_dirtvbx + mt_random(sub_dirtvrandx)) / USCALEDOWN,
			(sub_dirtvby + mt_random(sub_dirtvrandy)) / USCALEDOWN,
			-(sub_dirtvbz + mt_random(sub_dirtvrandz)) / USCALEDOWN);
		xformvec(&jx, &unpart->transvel, &unpart->transvel);
		unpart->transvel.x += objects[0].carvel.x*timeinc*uloop;
		unpart->transvel.y += objects[0].carvel.y*timeinc*uloop;
		unpart->transvel.z += objects[0].carvel.z*timeinc*uloop;
		obj2world(wheels[BACKRIGHT], &unpart->trans, &unpart->trans);
		linkchildtoparent(unpart, root);
		//		unpart = duptree(udirtpart);
		//		linkchildtoparent(unpart,root);
	}
}
// UNC
#if 1
#if 0
void uspawndarkdirt()
{
	int i;
	int wantp;
	static int ccc = 0;
	TREE *unpart;
	ccc++;
	if (ccc > particleeffects*uloop)
		ccc = 0;
	else
		return;
	wantp = darkdirtq;
	for (i = 0; i < wantp; i++) {
		unpart = duptree(udarkdirt);
		unpart->trans = objects[0].pos;
		setVEC(&unpart->transvel, -(darkdirtvbx + random(darkdirtvrandx)) / USCALEDOWN,
			(darkdirtvby + random(darkdirtvrandy)) / USCALEDOWN,
			-(darkdirtvbz + random(darkdirtvrandz)) / USCALEDOWN);
		unpart->transvel.x += objects[0].carvel.x*timeinc*uloop;
		unpart->transvel.y += objects[0].carvel.y*timeinc*uloop;
		unpart->transvel.z += objects[0].carvel.z*timeinc*uloop;
		linkchildtoparent(unpart, root3);
		//		unpart = duptree(udarkdirt);
		//		linkchildtoparent(unpart,root);
	}
	wantp = sub_darkdirtq;
	for (i = 0; i < wantp; i++) {
		unpart = duptree(udarkdirtpart);
		unpart->trans = objects[0].pos;
		setVEC(&unpart->transvel, -(sub_darkdirtvbx + random(sub_darkdirtvrandx)) / USCALEDOWN,
			(sub_darkdirtvby + random(sub_darkdirtvrandy)) / USCALEDOWN,
			-(sub_darkdirtvbz + random(sub_darkdirtvrandz)) / USCALEDOWN);
		unpart->transvel.x += objects[0].carvel.x*timeinc*uloop;
		unpart->transvel.y += objects[0].carvel.y*timeinc*uloop;
		unpart->transvel.z += objects[0].carvel.z*timeinc*uloop;
		linkchildtoparent(unpart, root3);
		//		unpart = duptree(udarkdirtpart);
		//		linkchildtoparent(unpart,root);
	}
}
#endif
// UNC
#endif
void spawndirtparts()
{
	uspawndirt();
	//	uspawndarkdirt();
}
// UNC
#if 1
static void uinitparticles()
{
	pushandsetdir("st2_game");
	udpart = alloctree(1, "dust4.lwo");
	udpart->flags |= TF_ALWAYSFACING;
	udpart->userint[2] = 0;
	udpart->userint[3] = 5;
	udpart->treedissolvecutoff = 10 / 256.0f;
	setVEC(&udpart->scale, dirtscale, dirtscale, dirtscale);
	udpart->userproc = udirtanim;
	if (videoinfo.video_maindriver != VIDEO_D3D_FS || videoinfo.video_maindriver != VIDEO_D3D_WND) {
		MAT *m;
		;//togglezbuffer(udpart,"dust",1);
		//m = findmaterial(udpart, "dust");
		m = udpart->findmaterial("dust");
		m->color.w = .5f;
	}

	udirtpart = alloctree(1, "dirtpart.lwo");
	udirtpart->flags |= TF_ALWAYSFACING;
	udirtpart->userint[2] = 0;
	udirtpart->userint[3] = 5;
	udirtpart->treedissolvecutoff = 10 / 256.0f;
	setVEC(&udirtpart->scale, sub_dirtscale, sub_dirtscale, sub_dirtscale);
	udirtpart->userproc = udirtcanim;
	if (videoinfo.video_maindriver != VIDEO_D3D_FS || videoinfo.video_maindriver != VIDEO_D3D_WND) {
		MAT *m;
		;//togglezbuffer(udirtpart,"dirt",1);
		//m = findmaterial(udirtpart, "dirt");
		m = udirtpart->findmaterial("dirt");
		m->color.w = .5f;
	}

	udarkdirt = alloctree(1, "drkdirt.lwo");
	udarkdirt->flags |= TF_ALWAYSFACING;
	udarkdirt->userint[2] = 0;
	udarkdirt->userint[3] = 5;
	udarkdirt->treedissolvecutoff = 10 / 256.0f;
	setVEC(&udarkdirt->scale, darkdirtscale, darkdirtscale, darkdirtscale);
	udarkdirt->userproc = udarkdirtanim;
	if (videoinfo.video_maindriver != VIDEO_D3D_FS || videoinfo.video_maindriver != VIDEO_D3D_WND) {
		MAT *m;
		;//togglezbuffer(udarkdirt,"dust",1);
		//m = findmaterial(udarkdirt, "dust");
		m = udarkdirt->findmaterial( "dust");
		m->color.w = .5f;
	}

	udarkdirtpart = alloctree(1, "dkdrtprt.lwo");
	udarkdirtpart->flags |= TF_ALWAYSFACING;
	udarkdirtpart->userint[2] = 0;
	udarkdirtpart->userint[3] = 5;
	udarkdirtpart->treedissolvecutoff = 10 / 256.0f;
	setVEC(&udarkdirtpart->scale, sub_darkdirtscale, sub_darkdirtscale, sub_darkdirtscale);
	udarkdirtpart->userproc = udirtcanim;
	/*    if (usehardware==0) {
			MAT *m;
			togglezbuffer(udarkdirtpart,"drkdirt",1);
			m = findmaterial(udarkdirtpart,"drkdirt");
			m->mtrans=.5f;
		} */
	popdir();
	//	setVEC(&udpart->trans,0,1,0);
	//	linkchildtoparent(udpart,root);
	//	udpart->proc=NULL;
	//	setVEC(&udirtpart->trans,0,2,0);
	//	linkchildtoparent(udirtpart,root);
	//	udirtpart->proc=NULL;
	//	setVEC(&udarkdirt->trans,0,3,0);
	//	linkchildtoparent(udarkdirt,root);
	//	udarkdirt->proc=NULL;
	//	setVEC(&udarkdirtpart->trans,0,4,0);
	//	linkchildtoparent(udarkdirtpart,root);
	//	udarkdirtpart->proc=NULL;
}

static void ufreeparticles()
{
	freetree(udpart);
	freetree(udirtpart);
	freetree(udarkdirt);
	freetree(udarkdirtpart);
}

/////////////////////// end particle stuff

int usehires;
void initfinishbursts();

void fixpondalphacutoff(TREE *t)
{
//	int i;
	if (!my_stricmp(t->name.c_str(), "pondw.lwo"))
		t->treedissolvecutoff = 10 / 256.0f;
//	for (i = 0; i < t->nchildren; i++)
//		fixpondalphacutoff(t->children[i]);
	for (auto it : t->children) {
		fixpondalphacutoff(it);
	}
}

//int halfp,halfy,halfr;

int st2_loadtrack(char *filename, int override)
{
	int i, j = 0, numl;
	//char **script;
	pushandsetdir("st2_tracks");
	//script = loadscript(filename, &numl);
	script sc = script(filename);
	numl = sc.num();
	for (i = 0; i < numl; i += 7)
	{
		thetrack[j].piece = atoi(sc.idx(i).c_str());
		if (override && thetrack[j].piece == PIECE_GRID) thetrack[j].piece = PIECE_BASE;
		if (!override && thetrack[j].piece == PIECE_BASE) thetrack[j].piece = PIECE_GRID;
		thetrack[j]. orhey = (float)atof(sc.idx(i + 1).c_str());
		thetrack[j].lpiece = atoi(sc.idx(i + 2).c_str());
		thetrack[j].previous = atoi(sc.idx(i + 3).c_str());
		st2_curpiece = atoi(sc.idx(i + 4).c_str());
		st2_lastpiece = atoi(sc.idx(i + 5).c_str());
		st2_dir = atoi(sc.idx(i + 6).c_str());
		j++;
	}
	//freescript(script, numl);
	popdir();

	return numl;
}
// UNC
#endif
// UNC
#if 1

//////// camera logic
//(rot)
void pickgroundcamrot() // set rotation relative to rotation of car (car relative)
{
	VEC q;
	q.x = 1;
	q.y = 0;
	q.z = 0;
	q.w = extracamang * PIOVER180;
	rotaxis2quat(&q, &q);
	quattimes(&objects[0].carang, &q, &q);
	qcamdesired = q;
}

// set camdesired to where car is centered given cameras rot, (move no rot)
void pickgroundcampos()
{
	JXFORM x;
	VEC wv;
	wv.x = 0;
	wv.y = 0;
	wv.z = camtrans.z;
	if (intunnels)
		wv.z /= 2;
	quat2xform(&camnull->rot, &x);
	xformvec(&x, &wv, &wv);
	camdesired.x = objects[0].pos.x + wv.x;
	camdesired.y = objects[0].pos.y + wv.y;
	camdesired.z = objects[0].pos.z + wv.z;
}

//(rot)
void pickaircamrot()  // target car to landspot (rot no move)
{
	VEC targ;
	targ.x = landspot.x - objects[0].pos.x;
	targ.y = landspot.y - objects[0].pos.y;
	targ.z = landspot.z - objects[0].pos.z;
	qcamdesired.y = (float)atan2(targ.x, targ.z);
	qcamdesired.x = (float)-atan2(targ.y, sqrt(targ.x*targ.x + targ.z*targ.z));
	qcamdesired.z = 0;
	rpy2quat(&qcamdesired, &qcamdesired);
}

void pickaircampos()
{
	VEC wv;
	JXFORM x;
	wv.x = 0;
	wv.y = 0;
	wv.z = camtrans.z;
	if (intunnels)
		wv.z /= 2;
	quat2xform(&camnull->rot, &x);
	xformvec(&x, &wv, &wv);
	camdesired.x = objects[0].pos.x + wv.x;
	camdesired.y = objects[0].pos.y + wv.y;
	camdesired.z = objects[0].pos.z + wv.z;
}

// set rot to look at car no matter what (bypasses qcamdesired) (rot no move)
void targetcamrot()
{
	VEC targ;
	VEC qcar, carrpy;
	JXFORM xcar;
	VEC targrot;
	VEC xftarg;
	VEC qcamr1, qcamr2;
	VEC qpit, qcarrot;
	float pitchthresh, rollthresh;
	targ.x = objects[0].pos.x - camnull->trans.x;
	targ.y = objects[0].pos.y - camnull->trans.y;
	targ.z = objects[0].pos.z - camnull->trans.z;
	quatinverse(&objects[0].carang, &qcar);
	rotaxis2quat(&newpitch, &qpit);
	quattimes(&qpit, &qcar, &qcar);
	quat2xform(&qcar, &xcar);
	xformvec(&xcar, &targ, &xftarg);
	targrot.y = (float)atan2(xftarg.x, xftarg.z);
	targrot.x = (float)-atan2(xftarg.y, sqrt(xftarg.x*xftarg.x + xftarg.z*xftarg.z));
	targrot.z = 0;
	rpy2quat(&targrot, &qcarrot);
	quatinverse(&qcar, &qcar);
	quattimes(&qcar, &qcarrot, &qcamr2);
	targrot.y = (float)atan2(targ.x, targ.z);
	targrot.x = (float)-atan2(targ.y, sqrt(targ.x*targ.x + targ.z*targ.z));
	targrot.z = 0;
	testcam = targrot;
	quat2rpy(&objects[0].carang, &carrpy);
	testcar = carrpy;
	rpy2quat(&targrot, &qcamr1);
	if (flymode)
		; //testnewcam=0;
	else {
		if (testnewcam == 1) {
			pitchthresh = pitchthreshdown;
			rollthresh = rollthreshdown;
		} else {
			pitchthresh = pitchthreshup;
			rollthresh = rollthreshup;
		}
		//		if ((fabs(targrot.x)>pitchthresh || fabs(carrpy.x)>pitchthresh || fabs(carrpy.z)>rollthresh) &&
		//			justshocks>10 )
		//			testnewcam=1;
		//		else
		//			testnewcam=0;
	}
	if (testnewcam) {
		camtween += camtweenspeed;
		if (camtween > 1)
			camtween = 1;
	} else {
		camtween -= camtweenspeed;
		if (camtween < 0)
			camtween = 0;
	}
	quatinterp(&qcamr1, &qcamr2, camtween, &camnull->rot);
}

// set camrot from qcamdesired (smooth rot)
void driftcamrot(float cdr)
{
	quatinterp(&camnull->rot, &qcamdesired, cdr, &camnull->rot);
}

float camcoldist;
//int incam;
void cameracollide()
{
	VEC camdel, camdel2;
	if (intunnels)
		return;
	camdel.x = camnull->trans.x;
	camdel.y = camnull->trans.y - camcoldist;
	camdel.z = camnull->trans.z;
	// for now, the camera stays above the ground
//	incam=1;
	if (st2_line2road(&camnull->trans, &camdel, &camdel2, NULL))
		camnull->trans.y = camdel2.y + camcoldist;
	//	incam=0;
}

////////// finishbursts
#define BSND1 37 // 9/16 (common)
#define BSND2 30 // 3/16
#define BSND3 31 // 3/16
#define BSND4 32 // 1/16 (rare)
int finishburstsounds[16] = {
	BSND2,BSND2,BSND2,BSND3,
	BSND3,BSND3,BSND4,BSND1,
	BSND1,BSND1,BSND1,BSND1,
	BSND1,BSND1,BSND1,BSND1,
};
#endif // UNC
TREE *fwork[4], *finishpiecenull;
static float tx = 1.4f;
#if 1 // UNC

/*// firey finish (not used)
void burstfinishproc3(TREE *t)
{
	int fs;
	t->user1++;	// inc living time
	t->transvel.y-=.001; // gravity
//	t->dissolve -= .03; // fade
	if (t->user1>t->userint[2]) { // die if living time > die time
		fs=finishburstsounds[random(16)];
		playasound(fs,0);
		switch(fs) {
		case BSND1:
			break;
		case BSND2:
			break;
		case BSND3:
			break;
		case BSND4:
			break;
		}
		freetree(t);
	}
}
*/

// spread
bool burstfinishproc2(TREE *t)
{
	//	int fs;
	t->userint[1]++;	// inc living time
	t->transvel.y -= .001f; // gravity
	t->treedissolvecutoff -= .03f; // fade
	if (t->userint[1] > t->userint[2]) { // die if living time > die time
		freetree(t);
	}
	return true;
}

// launch
bool burstfinishproc(TREE *t)
{
	int fs, i;
	TREE *u;
	t->userint[1]++;	// inc living time
	t->transvel.y -= .001f; // gravity
//	t->dissolve -= .03; // fade
	if (t->userint[1] > t->userint[2]) { // die if living time > die time
		fs = finishburstsounds[mt_random(16)];
		playatagsound(fs);
		switch (fs) {
		case BSND1:
			break;
		case BSND2:
			break;
		case BSND3:
			break;
		case BSND4:
			break;
		}
		for (i = 0; i < 100; i++) {
			float lo, la;
			if (fs == BSND4)
				u = duptree(fwork[mt_random(4)]);
			else
				u = duptree(fwork[t->userint[3]]);
			lo = mt_frand() * 2 * PI;
			la = mt_frand()*1.9999f - .99995f;
			u->transvel.y = .05f*la;
			la = (float)sqrt(1 - la * la);
			u->transvel.x = .05f*la*(float)cos(lo);
			u->transvel.z = .05f*la*(float)sin(lo);
			u->transvel.x += t->transvel.x;
			u->transvel.y += t->transvel.y;
			u->transvel.z += t->transvel.z;
			u->trans = t->trans;
			u->treedissolvecutoff = 10 / 256.0f;
			u->userproc = burstfinishproc2;
			u->treedissolvecutoff = .5f;
			u->userint[2] = 10 + mt_random(20); // time to live
			u->flags |= TF_ALWAYSFACING;
			setVEC(&u->scale, .02f, .02f, .02f);
			linkchildtoparent(u, finishpiecenull);
		}
		freetree(t);
	}
	return true;
}

void dofinishbursts()
{
	TREE *t;
	int i, r;
	static int ddd = 48; // time for next burst
	static int ccc;
	int rd;
	ccc++;
	if (ccc < ddd)
		return;
	ddd = 38 + mt_random(30);
	ccc = 0;
	rd = 2 + mt_random(3); // number of launches
//	rd=5+random(20);
	for (i = 0; i < rd; i++) {
		r = mt_random(4);
		t = duptree(fwork[r]);
		t->userint[3] = r; // keep track of color
		t->userproc = burstfinishproc;
		t->transvel.x = mt_frand()*.02f - .01f;
		t->transvel.y = .05f + mt_frand()*.02f;
		t->transvel.z = mt_frand()*.02f - .01f;
		t->trans.x = tx;
		t->treedissolvecutoff = 240;
		tx = -tx;
		t->trans.y = 2.1f;
		t->treedissolvecutoff = 10 / 256.0f;
		t->userint[2] = 30 + mt_random(50); // time to live
		setVEC(&t->scale, .03f, .03f, .03f);
		t->flags |= TF_ALWAYSFACING;
		linkchildtoparent(t, finishpiecenull);
	}
}
#endif // UNC
void initfinishbursts()
{
	pushandsetdir("st2_fireworks");
	fwork[0] = alloctree(0, "ember1.lwo");
	fwork[1] = alloctree(0, "ember2.lwo");
	fwork[2] = alloctree(0, "ember3.lwo");
	fwork[3] = alloctree(0, "ember4.lwo");
	if (videoinfo.video_maindriver != VIDEO_D3D_FS || videoinfo.video_maindriver != VIDEO_D3D_WND) {
		;//togglezbuffer(fwork[0],"ember1",0);
		;//togglezbuffer(fwork[1],"ember2",0);
		;//togglezbuffer(fwork[2],"ember3",0);
		;//togglezbuffer(fwork[3],"ember4",0);
	}
	popdir();
	finishpiecenull = alloctree(4000, NULL);
	linkchildtoparent(finishpiecenull, finishpiece);
}
#if 1 // UNC
void freefinishbursts()
{
	int i;
	for (i = 0; i < 4; i++)
		freetree(fwork[i]);
}
////////// end finishbursts

// set campos from camdesired (smooth move)
void driftcam(float cd)
{
	VEC camdelta;
	camdelta.x = camdesired.x - camnull->trans.x;
	camdelta.y = camdesired.y - camnull->trans.y;
	camdelta.z = camdesired.z - camnull->trans.z;
	//	dofinishbursts();
	if (!dofinish) {
		camnull->trans.x += cd * camdelta.x;
		camnull->trans.y += cd * camdelta.y;
		camnull->trans.z += cd * camdelta.z;
	} else if (clocktickcount > 0) {
		camnull->trans.y += uloop * .010f;
		if (dofinish < 250)
			//		if (dofinish<250 && utotalstunts>=50) // maybe we only want fworks when stunts>=50
			dofinishbursts();
	}
}

static void drawobjects()
{
	int i, j;
	VEC q, q2;
	VEC checkshadroll;
	VEC localroll180;
	VEC tt, tb, ti, n = { 0,1,0 };
	float d;
	////////// copy object to tree
	// do car
	/*	if (fabs(accelspin*4)<startaccelspin)
		nuflames=0;
		else if (accelspin*4<-startaccelspin) {
			if (uflameonback) {
				uflameonback=0;
				unhooktree(uplayflame1);
				unhooktree(uplayflame2);
				linkchildtoparent(uplayflame1,uaxles[1]);
				linkchildtoparent(uplayflame2,uaxles[0]);
			}
		} else if (accelspin*4>startaccelspin) {
			if (!uflameonback) {
				uflameonback=1;
				unhooktree(uplayflame1);
				unhooktree(uplayflame2);
				linkchildtoparent(uplayflame1,uaxles[2]);
				linkchildtoparent(uplayflame2,uaxles[3]);
			}
		} */
		//	if (uflameonback) {
	uplayflame1->trans = uflame1pos;
	uplayflame2->trans = uflame2pos;
	uplayflame1->scale = uflame1scale;
	uplayflame2->scale = uflame2scale;
	//	} else {
	//		uplayflame1->trans=uflame4pos;
	//		uplayflame2->trans=uflame3pos;
	//		uplayflame1->scale=uflame4scale;
	//		uplayflame2->scale=uflame3scale;
	//	}
	carbody->trans.x = caroffsetx;
	carbody->trans.y = caroffsety;
	carbody->trans.z = caroffsetz;
	carnull->rot = objects[0].carang;
	carnull->trans = objects[0].pos;
	carcenternull->scale.x = uplaycarscale;
	carcenternull->scale.y = uplaycarscale;
	carcenternull->scale.z = uplaycarscale;
	quat2rpy(&carnull->rot, &checkshadroll); //it's in radians
// do car bones
	for (i = 0; i < COLBONESX; i++)
		for (j = 0; j < COLBONESZ; j++)
			dembones[i][j].t->transvel = dembones[i][j].curpushin;
	// do shadow
	shadowbody->rot = objects[0].carang;
	if (fabs(checkshadroll.z) > 90 * PIOVER180) {
		localroll180.x = 0;
		localroll180.y = 0;
		localroll180.z = 1;
		localroll180.w = 0;
		quattimes(&shadowbody->rot, &localroll180, &shadowbody->rot);
	}
	shadownull->trans = objects[0].pos;
	shadowb->scale.x = uplayshadowscale;
	shadowb->scale.y = uplayshadowscale * .001f;
	shadowb->scale.z = uplayshadowscale;
	tb = objects[0].pos;
	tt = objects[0].pos;
	tt.y += .15f;
	tb.y -= 100;
	st2_line2road(&tt, &tb, &ti, &n);
	if (objects[0].pos.y - ti.y > 1.4f)
		shadownull->trans.y = ti.y + shadyoffset2;
	else
		shadownull->trans.y = ti.y + shadyoffset;
	ti.x = 0;
	ti.y = 1;
	ti.z = 0;
	d = dot3d(&ti, &n);
	cross3d(&ti, &n, &ti);
	ti.w = racos(d);
	rotaxis2quat(&ti, &shadownull->rot);
	// update motor sound
	uplayrevfreq = (int)(fabs(objects[0].wheelvel[BACKLEFT])*revfreqmul + revfreqadd);
	uplayrevfreq = range(100, uplayrevfreq, 100000);
	/////// update wheel trees
	if (selectedcar == RADAR) {
		objects[0].shocklen[4] = (objects[0].shocklen[0] + objects[0].shocklen[2]) / 2;
		objects[0].shocklen[5] = (objects[0].shocklen[1] + objects[0].shocklen[3]) / 2;
		objects[0].wheelvel[4] = (objects[0].wheelvel[0] + objects[0].wheelvel[2]) / 2;
		objects[0].wheelvel[5] = (objects[0].wheelvel[1] + objects[0].wheelvel[3]) / 2;
	}
	for (i = 0; i < max(4, carinfo[selectedcar].nwheels); i++) {
		wheels[i]->trans.y = -(objects[0].shocklen[i] - wheelrad - shockoffsety);
		if (wheels[i]->trans.y > shocklimit)
			wheels[i]->trans.y = shocklimit;
		if (steershocks[i]) {
			q.x = 0;
			q.y = 1;
			q.z = 0;
			q.w = objects[0].wheelyaw*PIOVER180;
			rotaxis2quat(&q, &q);
		} else {
			q.x = 0;
			q.y = 0;
			q.z = 0;
			q.w = 1;
		}
		//		if (i<4)
		objects[0].wheelang[i] = normang(objects[0].wheelang[i] + objects[0].wheelvel[i] * timeincconst);
		//		else
		//			objects[0].wheelang[i]=normangrad(objects[0].wheelang[i]+objects[0].wheelvel[i-4]*timeincconst);
		q2.x = 1;
		q2.y = 0;
		q2.z = 0;
		q2.w = objects[0].wheelang[i];//*PIOVER180;
//		if (selectedcar==TREAD) {
//			treadanimtex.curframe=(int)(q2.w*(8.0f/PI));
//			treadanimtex.curframe&=1;
//		}
		rotaxis2quat(&q2, &q2);
		quattimes(&q, &q2, &q);
		wheels[i]->rot = q;
	}
	wheels[FRONTLEFT]->trans.x = -shockspacingx / 2 + wheelwid / 2;
	wheels[FRONTLEFT]->trans.z = shockspacingz / 2 - wheellen / 2;
	wheels[FRONTRIGHT]->trans.x = shockspacingx / 2 - wheelwid / 2;
	wheels[FRONTRIGHT]->trans.z = shockspacingz / 2 - wheellen / 2;
	wheels[BACKLEFT]->trans.x = -shockspacingx / 2 + wheelwid / 2;
	wheels[BACKLEFT]->trans.z = -shockspacingz / 2 + wheellen / 2;
	wheels[BACKRIGHT]->trans.x = shockspacingx / 2 - wheelwid / 2;
	wheels[BACKRIGHT]->trans.z = -shockspacingz / 2 + wheellen / 2;
	if (selectedcar == RADAR) {
		wheels[4]->trans.x = -shockspacingx / 2 + wheelwid / 2;
		wheels[4]->trans.z = 0;
		wheels[5]->trans.x = shockspacingx / 2 - wheelwid / 2;
		wheels[5]->trans.z = 0;
	}
	for (i = 0; i < max(4, carinfo[selectedcar].nwheels); i++) {
		wheels[i]->scale.x = uplaywheelscale;
		wheels[i]->scale.y = uplaywheelscale;
		wheels[i]->scale.z = uplaywheelscale;
	}
	// handle special effects (lightning around car)
	if (nulightnings && !slowpo) {
		uplayspecial1->flags &= ~TF_DONTDRAW;
		;//memset((unsigned char *)specialanimtex.desttex,0,specialanimtex.w*specialanimtex.h*2);
		;/*SpriteBlit(specialanimtex.texdata[specialanimtex.curframe],specialanimtex.desttex,0,0,
			specialanimtex.w,specialanimtex.h,specialanimtex.w,specialanimtex.h,specialanimtex.w,0);*/
		;//DumpExistingTex(specialanimtex.mat);
//		specialanimtex.curframe++;
		tspanimtex(specialtsp, 0, 0, 0);
		if (specialtsp->curframe == 0) {
			nulightnings -= uloop;
			if (nulightnings < 0)
				nulightnings = 0;
			if (nulightnings == 0) {
				uplayspecial1->flags |= TF_DONTDRAW;
				;//Sound_StopSound(lightningsound);
			}
		}
	} else
		uplayspecial1->flags |= TF_DONTDRAW;
	// car flashes (blue car)
	if (nuflashes) {
		tspanimtex(flashtsp, 0, 0, 0);
		//		carbodyanimtex.curframe++;
		if (flashtsp->curframe == 0) {
			//			carbodyanimtex.curframe=0;
			nuflashes -= uloop;
			if (nuflashes < 0)
				nuflashes = 0;
		}
		;//memset((unsigned char *)carbodyanimtex.desttex,0,carbodyanimtex.w*carbodyanimtex.h*2);
		;/*SpriteBlit(carbodyanimtex.texdata[carbodyanimtex.curframe],carbodyanimtex.desttex,0,0,
			carbodyanimtex.w,carbodyanimtex.h,carbodyanimtex.w,carbodyanimtex.h,carbodyanimtex.w,0);*/
		;//DumpExistingTex(carbodyanimtex.mat);
	}
	// golden condom
	if (nucondoms) {
		uplayspecial2->flags &= ~TF_DONTDRAW;
		tspanimtex(condomtsp, 0, 0, 0);
		//		goldencondomanimtex.curframe++;
		if (condomtsp->curframe == 0) {
			nucondoms -= uloop;
			if (nucondoms < 0)
				nucondoms = 0;
			if (nucondoms == 0)
				uplayspecial2->flags |= TF_DONTDRAW;
		}
		;//memset((unsigned char *)goldencondomanimtex.desttex,0,goldencondomanimtex.w*goldencondomanimtex.h*2);
		;/*SpriteBlit(goldencondomanimtex.texdata[goldencondomanimtex.curframe],goldencondomanimtex.desttex,0,0,
			goldencondomanimtex.w,goldencondomanimtex.h,goldencondomanimtex.w,goldencondomanimtex.h,goldencondomanimtex.w,0); */
		;//DumpExistingTex(goldencondomanimtex.mat);
	}
	// flames
	if (nuflames) {
		uplayflame1->flags &= ~TF_DONTDRAW;
		uplayflame2->flags &= ~TF_DONTDRAW;
		tspanimtex(flametsp, 0, 0, 0);
		//		flameranimtex.curframe++;
		if (flametsp->curframe == 0) {
			nuflames -= uloop;
			if (nuflames < 0)
				nuflames = 0;
		}
		;//memset((unsigned char *)flameranimtex.desttex,0,flameranimtex.w*flameranimtex.h*2);
		;/*SpriteBlit(flameranimtex.texdata[flameranimtex.curframe],flameranimtex.desttex,0,0,
			flameranimtex.w,flameranimtex.h,flameranimtex.w,flameranimtex.h,flameranimtex.w,0); */
		;//DumpExistingTex(flameranimtex.mat);
		if (flymode || nuflames == 0) {
			uplayflame1->flags |= TF_DONTDRAW;
			uplayflame2->flags |= TF_DONTDRAW;
		}
	}
	// scoreline
	scrline->scale.x = scrlinescl.x*.001f; // scale down the enormously huge scoreline
	scrline->scale.y = scrlinescl.y*.001f;
	scrline->scale.z = scrlinescl.z*.001f;
	scrline->trans.x = scrlinepos.x*.001f;
	scrline->trans.y = scrlinepos.y*.001f;
	scrline->trans.z = scrlinepos.z*.001f;
	////////////// lastly, work the camera .................
	// try yeta nother camera
	for (i = 0; i < uloop; i++) {
		if (flymode && startstunt) {
			pickaircamrot();
			driftcamrot(camrotdrift);
			pickaircampos();
			driftcam(camdrift);
		} else {
			pickgroundcamrot();
			driftcamrot(camrotdrift);
			pickgroundcampos();
			driftcam(camdrift);
		}
	}
	if (manualcar)
		camnull->trans = landspot;
	cameracollide();
	targetcamrot();
}

static int ttp; // total trick points
VEC ucarrotvel;
int udidroll, udidyaw, udidpitch;
static int rollpie[4], pitchpie[4], yawpie[4], rollpiesum, pitchpiesum, yawpiesum;
static int cantaddroll, cantaddpitch, cantaddyaw;
static void clearrollpie()
{
	int i;
	for (i = 0; i < 4; i++)
		rollpie[i] = 0;
	rollpiesum = 0;
}

static void clearpitchpie()
{
	int i;
	for (i = 0; i < 4; i++)
		pitchpie[i] = 0;
	pitchpiesum = 0;
}

static void clearyawpie()
{
	int i;
	for (i = 0; i < 4; i++)
		yawpie[i] = 0;
	yawpiesum = 0;
}

static void clearpies()
{
	clearyawpie();
	clearpitchpie();
	clearrollpie();
	cantaddroll = cantaddpitch = cantaddyaw = -1;
	udidroll = udidyaw = udidpitch = 0;
}

int addrollpie(float ang)
{
	int i = (int)ang / 90;
	i = range(0, i, 3);
	i &= 3;
	if (!rollpie[i] && cantaddroll != i) {
		rollpie[i] = 1;
		rollpiesum++;
		cantaddroll = -1;
		if (rollpiesum == 4) {
			clearrollpie();
			cantaddroll = i;
			return 1;
		}
	}
	return 0;
}

int addpitchpie(float ang)
{
	int i = (int)ang / 90;
	i = range(0, i, 3);
	i &= 3;
	if (!pitchpie[i] && cantaddpitch != i) {
		pitchpie[i] = 1;
		pitchpiesum++;
		cantaddpitch = -1;
		if (pitchpiesum == 4) {
			clearpitchpie();
			cantaddpitch = i;
			return 1;
		}
	}
	return 0;
}

int addyawpie(float ang)
{
	int i = (int)ang / 90;
	i = range(0, i, 3);
	i &= 3;
	if (!yawpie[i] && cantaddyaw != i) {
		yawpie[i] = 1;
		yawpiesum++;
		cantaddyaw = -1;
		if (yawpiesum == 4) {
			clearyawpie();
			cantaddyaw = i;
			return 1;
		}
	}
	return 0;
}

void drivecar()
{
	struct soundhandle *sh;
	extern int releasemode;
	extern float defaultjumpfriction;
	//;//extern int dmx,dmy;
	int doatricksound1 = 0, doatricksound2 = 0, doatricksound3 = 0;
	// get controls into variables
	// remove joystick and mouse code for now....
#if 0
	if (releasemode) { // use the mouse only in release mode
		uup = (wininfo.keystate[K_UP] || wininfo.jy < -.5f || (wininfo.jbut & 1) || (MBUT & 1) || wininfo.dmy <= -8);
		udown = (wininfo.keystate[K_DOWN] || wininfo.jy > .5f || (wininfo.jbut & 2) || (MBUT & 2) || wininfo.dmy >= 8);
		uleft = (wininfo.keystate[K_LEFT] || wininfo.jx < -.5f || wininfo.dmx <= -8);
		uright = (wininfo.keystate[K_RIGHT] || wininfo.jx > .5f || wininfo.dmx >= 8);
	} else {
		uup = (wininfo.keystate[K_UP] || wininfo.jy < -.5f || (wininfo.jbut & 1));
		udown = (wininfo.keystate[K_DOWN] || wininfo.jy > .5f || (wininfo.jbut & 2));
		uleft = (wininfo.keystate[K_LEFT] || wininfo.jx < -.5f);
		uright = (wininfo.keystate[K_RIGHT] || wininfo.jx > .5f);
	}
#else
	uup = wininfo.keystate[K_UP];
	udown = wininfo.keystate[K_DOWN];
	uleft = wininfo.keystate[K_LEFT];
	uright = wininfo.keystate[K_RIGHT];
#endif
	urollright = wininfo.keystate[K_LEFTALT] | wininfo.keystate[K_RIGHTALT] |
		wininfo.keystate[K_LEFTSHIFT] | wininfo.keystate[K_RIGHTSHIFT];
	urollleft = wininfo.keystate[K_RIGHTCTRL] | wininfo.keystate[K_LEFTCTRL];
	//	urollright=wininfo.keystate[K_LEFTALT]|wininfo.keystate[K_LEFTSHIFT]|wininfo.keystate[K_RIGHTCTRL];
	//	urollleft=wininfo.keystate[K_RIGHTALT]|wininfo.keystate[K_RIGHTSHIFT]|wininfo.keystate[K_LEFTCTRL];
	if (urollright)
		logger("urollright\n");
	if (urollleft)
		logger("urollleft\n");
	if (doacrashreset || cantdrive || cantstartdrive) {
		uup = udown = uleft = uright = urollright = urollleft = 0; // can't drive when crashing
	}
	if (startstunt) {
		if (uup && !lup || udown && !ldown)
			uready = 1;
	}
	//	if (globalkey==DIK_M) // disabled now, but it is in debprint.c
	//		magnatraction^=1;
	if (!flymode) {
		startstunt = uready = 0;
		if (uup /*&& drivemode!=CARBRAKE*/) { // pitch down
			if (accelspin >= -1) {
				drivemode = CARACCEL;
				if (accelspin > startaccelspin)
					drivemode = CARCOAST;
			} else {
				drivemode = CARBRAKE;
			}
		}
		if (udown) { // pitch up
			if (accelspin < 1) {
				drivemode = CARREVERSEACCEL;
				if (accelspin < -startaccelspin / 2)
					drivemode = CARCOAST;
			} else {
				drivemode = CARBRAKE;
			}
		}
		if (!uup && !udown) {
			drivemode = CARCOAST;
		}
		if (dofinish)
			drivemode = CARBRAKE;
		if (uleft) { // yaw left
			if (objects[0].wheelyaw > 0)
				objects[0].wheelyaw -= steervelback * timeincconst;
			else
				objects[0].wheelyaw -= steervel * timeincconst;
			if (objects[0].wheelyaw < -MAXSTEER)
				objects[0].wheelyaw = -MAXSTEER;
		}
		if (uright) { // yaw right
			if (objects[0].wheelyaw > 0)
				objects[0].wheelyaw += steervel * timeincconst;
			else
				objects[0].wheelyaw += steervelback * timeincconst;
			if (objects[0].wheelyaw > MAXSTEER)
				objects[0].wheelyaw = MAXSTEER;
		}
		if (!uleft && !uright) {
			if (objects[0].wheelyaw > steervelback*timeincconst)
				objects[0].wheelyaw -= steervelback * timeincconst;
			else if (objects[0].wheelyaw < -steervelback * timeincconst)
				objects[0].wheelyaw += steervelback * timeincconst;
			else
				objects[0].wheelyaw = 0;
		}
	} else { // fly
		if (!startstunt) {
			// just left ground
			float t;
			clearpies();
			quat2rpy(&objects[0].carang, &carstuntrpy); // convert from quats to rpy
			carstuntrpy.x *= PIUNDER180;
			carstuntrpy.y *= PIUNDER180;
			carstuntrpy.z *= PIUNDER180;
			startstunt = 1; // start a stunt
			objects[0].carvelang = zerov;
			// calculate a landing spot
			littleg = littlegair;
			t = (objects[0].carvel.y +
				(float)sqrt(objects[0].carvel.y*objects[0].carvel.y + 2 * littleg*objects[0].pos.y)) / littleg;
			t *= 1.1f;
			landspot.x = objects[0].pos.x + objects[0].carvel.x*t;
			landspot.y = -3;
			landspot.z = objects[0].pos.z + objects[0].carvel.z*t;
		}
		if (uup && uready) {
			ucarrotvel.x += uthiscarpitch;
		}
		if (udown && uready) {
			ucarrotvel.x -= uthiscarpitch;
		}
		if (uright /*&& uready*/) {
			ucarrotvel.y += uthiscaryaw;
		}
		if (uleft /*&& uready*/) {
			ucarrotvel.y -= uthiscaryaw;
		}
		if (urollleft /*&& uready*/) {
			ucarrotvel.z += uthiscarroll;
			//			carstuntrpy.z+=24;
		}
		if (urollright /*&& uready*/) {
			ucarrotvel.z -= uthiscarroll;
			//			carstuntrpy.z-=24;
		}
		carstuntrpy.x += ucarrotvel.x / 2;
		carstuntrpy.y += ucarrotvel.y / 2;
		carstuntrpy.z += ucarrotvel.z / 2;

		ucarrotvel.x -= ucarrotvel.x / defaultjumpfriction / 2;
		ucarrotvel.y -= ucarrotvel.y / defaultjumpfriction / 2;
		ucarrotvel.z -= ucarrotvel.z / defaultjumpfriction / 2;

		carstuntrpy.x = normang(carstuntrpy.x);
		carstuntrpy.y = normang(carstuntrpy.y);
		carstuntrpy.z = normang(carstuntrpy.z);
		doatricksound1 = addpitchpie(carstuntrpy.x);
		doatricksound2 = addyawpie(carstuntrpy.y);
		doatricksound3 = addrollpie(carstuntrpy.z);
		if (doatricksound1) {
			float tf;
			//			if (halfp&1) {
			sh = wave_play(taggetwhbyidx(gardentags, MENUREV9), 1);
			tf = getsfxfreq(sh);
			if (sh)
				setsfxfreq(sh, tf + ttp * 150.0f / sh->basefreq);
			sh->isplayasound = 1;
			//			}
			//			halfp++;
			udidpitch = 1;
			ttp++;
		}
		if (doatricksound2) {
			float tf;
			//			if (halfy&1) {
			sh = wave_play(taggetwhbyidx(gardentags, MENUREV10), 1);
			tf = getsfxfreq(sh);
			if (sh)
				setsfxfreq(sh, tf + ttp * 150.0f / sh->basefreq);
			sh->isplayasound = 1;
			//			}
			//			halfy++;
			udidyaw = 1;
			ttp++;
		}
		if (doatricksound3) {
			float tf;
			//			if (halfr&1) {
			sh = wave_play(taggetwhbyidx(gardentags, MENUREV11), 1);
			tf = getsfxfreq(sh);
			if (sh)
				setsfxfreq(sh, tf + ttp * 150.0f / sh->basefreq);
			sh->isplayasound = 1;
			//			}
			//			halfr++;
			udidroll = 1;
			ttp++;
		}
		if (doatricksound1)
			ttp += udidroll + udidyaw;
		if (doatricksound2)
			ttp += udidroll + udidpitch;
		if (doatricksound3)
			ttp += udidpitch + udidyaw;
		if (!urollright && !urollleft) {
			//		if (uready && !manualcar) {
			if (carstuntrpy.z > 3 && carstuntrpy.z < 180)
				carstuntrpy.z -= 2;
			else if (carstuntrpy.z < 357 && carstuntrpy.z>180)
				carstuntrpy.z += 2;
			else
				carstuntrpy.z = 0;
		}
	}
}
// UNC
#endif
void getpiece(VEC *v, int *x, int *z)
{
	*x = (int)((35 + v->x + 100) / 10) - 10;
	*z = (int)((35 - v->z + 100) / 10) - 10;
}

//////////////// new roadheight code /////////

int bestpiece, bestlwo, bestmat;

void st2_callsurffunc() // call only if line2road returns true
{
	void(*fun)();
	if (bestpiece >= 0) {
		fun = thetrack[bestpiece].clwos[bestlwo].matfuncs[bestmat];
		if (fun)
			(*fun)();
	}
}

int st2_line2road(VEC *top, VEC *bot, VEC *bestintsect, VEC *bestnorm)
{
	// get piece
	int mat;
	int i, j, k;
	int foundint = 0;
	int minpx, minpz, maxpx, maxpz, x, z;
	VEC minv, maxv;
	float bestdist, dist;
	int piece;
	VEC ltop, lbot, intsect, norm;
	TREE *tr;
	float t;
	bestpiece = -1;
	getpiece(&objects[0].pos, &x, &z);
	if (x >= 0 && x <= 6 && z >= 0 && z <= 6)
		setnextcrashloc(x, z);
	// first do 0 plane xz, outside track pieces
	if (top->y >= EPSILON && bot->y <= -EPSILON) { // line does cross 0 plane
		if (bestnorm) {
			bestnorm->x = 0;
			bestnorm->y = 1;
			bestnorm->z = 0;
		}
		t = -top->y / (bot->y - top->y);
		interp3d(top, bot, t, bestintsect);
		getpiece(bestintsect, &x, &z); // see if intsect point is outside the pieces
//		if (1) {
		if (x < 0 || x>6 || z < 0 || z>6) {
			bestdist = top->y*top->y;
			foundint = 1;
			//			return 1;
			//		} else {
			//			setnextcrashloc(x,z);
		}
	}
	//	return 0;
	//float bigwallminx,bigwallminz,bigwallmaxx,bigwallmaxz;
	//float bigtreeminx,bigtreeminz,bigtreemaxx,bigtreemaxz;
	// do collision with bigwall and bigtree
	// 4 walls
	if (bot->x < bigwallminx - EPSILON && top->x >= bigwallminx + EPSILON) {
		t = (bigwallminx - top->x) / (bot->x - top->x);
		interp3d(top, bot, t, &intsect);
		dist = (top->x - bigwallminx)*(top->x - bigwallminx);
		if (dist < bestdist || !foundint) {
			foundint = 1;
			bestdist = dist;
			*bestintsect = intsect;
			if (bestnorm) {
				bestnorm->x = 1;
				bestnorm->y = 0;
				bestnorm->z = 0;
			}
		}
	}
	if (bot->z < bigwallminz - EPSILON && top->z >= bigwallminz + EPSILON) {
		t = (bigwallminz - top->z) / (bot->z - top->z);
		interp3d(top, bot, t, &intsect);
		dist = (top->z - bigwallminz)*(top->z - bigwallminz);
		if (dist < bestdist || !foundint) {
			foundint = 1;
			bestdist = dist;
			*bestintsect = intsect;
			if (bestnorm) {
				bestnorm->x = 0;
				bestnorm->y = 0;
				bestnorm->z = 1;
			}
		}
	}
	if (bot->x > bigwallmaxx + EPSILON && top->x <= bigwallmaxx - EPSILON) {
		t = (bigwallmaxx - top->x) / (bot->x - top->x);
		interp3d(top, bot, t, &intsect);
		dist = (top->x - bigwallmaxx)*(top->x - bigwallmaxx);
		if (dist < bestdist || !foundint) {
			foundint = 1;
			bestdist = dist;
			*bestintsect = intsect;
			if (bestnorm) {
				bestnorm->x = -1;
				bestnorm->y = 0;
				bestnorm->z = 0;
			}
		}
	}
	if (bot->z > bigwallmaxz + EPSILON && top->z <= bigwallmaxz - EPSILON) {
		t = (bigwallmaxz - top->z) / (bot->z - top->z);
		interp3d(top, bot, t, &intsect);
		dist = (top->z - bigwallmaxz)*(top->z - bigwallmaxz);
		if (dist < bestdist || !foundint) {
			foundint = 1;
			bestdist = dist;
			*bestintsect = intsect;
			if (bestnorm) {
				bestnorm->x = 0;
				bestnorm->y = 0;
				bestnorm->z = -1;
			}
		}
	}
	// tree
	if (top->x <bigtreemaxx && top->x>bigtreeminx && top->z > bigtreeminz && top->z < bigtreemaxz) {
		world2obj(bigtree, top, &ltop); // do it with the .lwo !
		world2obj(bigtree, bot, &lbot); // do it with the .lwo !
	// get roadheight given local x z
		if (line2piece(bigtree->mod, &ltop, &lbot, &intsect, &norm)) {
			dist = dist3dsq(&intsect, &ltop);
			if (!foundint || dist < bestdist) {
				foundint = 1;
				obj2world(bigtree, &intsect, &intsect);
				obj2world(bigtree, &norm, &ltop);
				obj2world(bigtree, &zerov, &lbot);
				norm.x = ltop.x - lbot.x;
				norm.y = ltop.y - lbot.y;
				norm.z = ltop.z - lbot.z;
				bestdist = dist;
				if (bestnorm)
					*bestnorm = norm;
				*bestintsect = intsect;
			}
		}
	}
	// grid pieces
	minv.x = min(top->x, bot->x) - loopcompensater; // 2.5 is for the loop that extends over other pieces
	maxv.z = min(top->z, bot->z) - loopcompensater;
	maxv.x = max(top->x, bot->x) + loopcompensater;
	minv.z = max(top->z, bot->z) + loopcompensater;
	getpiece(&minv, &minpx, &minpz);
	getpiece(&maxv, &maxpx, &maxpz);
	if (minpx < 0)
		minpx = 0;
	if (maxpx > 6)
		maxpx = 6;
	if (minpz < 0)
		minpz = 0;
	if (maxpz > 6)
		maxpz = 6;
	for (i = minpx; i <= maxpx; i++)
		for (j = minpz; j <= maxpz; j++) {
			//			if (i<0 || i>6 || j<0 || j>6)
			//				errorexit("outobounds");
			piece = 7 * i + j;
			for (k = 0; k < thetrack[piece].nlwos; k++) {
				COLLGRID *cg;
				tr = thetrack[piece].tlwos[k];
				cg = &thetrack[piece].clwos[k];
				//				tr=thetrack[piece].t->children[0];
				// world to local
				world2obj(tr, top, &ltop); // do it with the .lwo !
				world2obj(tr, bot, &lbot); // do it with the .lwo !
// get roadheight given local x z
				/* new */		if ((mat = line2piecegrid(cg, &ltop, &lbot, &intsect, &norm))) { // grid system
				// old			if (line2piece(tr->bodyid,&ltop,&lbot,&intsect,&norm)) {
				// very old  	if (line2piece(thetrack[piece].t->children[0]->bodyid,&ltop,&lbot,&intsect,&norm)) {
					dist = dist3dsq(&intsect, &ltop);
					if (!foundint || dist < bestdist) {
						foundint = 1;
						obj2world(tr, &intsect, &intsect);
						obj2world(tr, &norm, &ltop);
						obj2world(tr, &zerov, &lbot);
						norm.x = ltop.x - lbot.x;
						norm.y = ltop.y - lbot.y;
						norm.z = ltop.z - lbot.z;
						bestdist = dist;
						bestmat = mat - 1;
						bestlwo = k;
						bestpiece = piece;
						if (bestnorm)
							*bestnorm = norm;
						*bestintsect = intsect;
					}
				}
			}
		}

	if (bestnorm)
		normalize3d(bestnorm, bestnorm);
	return foundint;
}
// UNC
#if 1
//////////////// end new roadheight code /////
float ufireworkscale;
void douburst(int numbursts)
{
	int i;
	if (nubursts)
		return;
	ufireworkscale = (float)(numbursts / 5);
	ufireworkscale = range(1.5f, ufireworkscale, 3.0f);
	for (i = 0; i < 4; i++) {
		linkchildtoparent(ubursta[i], carnull);
		linkchildtoparent(uburstb[i], carnull);
	}
	nubursts = numbursts;
}

void dolightning(int numlightnings)
{
	if (nulightnings)
		return;
	if (slowpo)
		return;
	;//lightningsound = Sound_FindSlot(MENUREV12); // one car sound
	;//Sound_Play(lightningsound, 0,0,0, 1); // loop until done
	nulightnings = numlightnings;
}

void docondom(int numcondoms)
{
	if (nucondoms)
		return;
	//	lightningsound = Sound_FindSlot(MENUREV12); // one car sound
	//	Sound_Play(lightningsound, 0,0,0, 1); // loop until done
	nucondoms = numcondoms;
}

void doblueflash(int numflashes)
{
	if (nuflashes)
		return;
	nuflashes = numflashes;
}

void douflames(int numflames)
{
	if (nuflames)
		return;
	nuflames = numflames;
}

// update animtex for clock,coin,speedo,burst
int notimelimit;
void uplayupdateclock()
{
	int j, k;
	int m = 1, t = 4, o = 2;
	//	int sw,sh,dw,dh;
	int tt;
	// time left
	//	sw=clockanimtex.w;
	//	sh=clockanimtex.h;
	//	dw=1<<clockanimtex.mat->thetex->logu;
	//	dh=1<<clockanimtex.mat->thetex->logv;
	if (!dofinish && !notimelimit && !cantstartdrive)
		clocktickcount -= uloop;
	if (clocktickcount < 0) {
		if (!dofinish) {
			playatagsound(39); // horn
			dofinish = 1;
		}
		clocktickcount = 0;
	}
	//(ustarttime*30+29)*2
	// now they want time from start
	tt = ((ustarttime * 30 + 29) * 2 - clocktickcount) / 2;
	if (oldtt != tt) {
		oldtt = tt;
		m = (tt) / (60 * 30);
		t = (tt) / 300 % 6;
		o = (tt) / 30 % 10;
		if (m >= 10) {
			m = 9;
			t = 5;
			o = 9;
		}
		// time left
		//	memset(clockanimtex.desttex,0,clockanimtex.w*clockanimtex.h*2);
		if (clocktickcount < 10 * 60 && (clocktickcount / 2) % 10>5)
			m = t = o = 10;
		//	m=(clocktickcount/2)/(60*30);
		//	t=(clocktickcount/2)/300%6;
		//	o=(clocktickcount/2)/30%10;
		//	if (gameplayx>=800) {
		if (usehires) { // hires
			clocktsp->lastframe = -1;
			tspsetframe(clocktsp, m);
			tspanimtex(clocktsp, 14, -2, 1);
			clocktsp->lastframe = -1;
			tspsetframe(clocktsp, t);
			tspanimtex(clocktsp, 40, -2, 1);
			clocktsp->lastframe = -1;
			tspsetframe(clocktsp, o);
			tspanimtex(clocktsp, 65, -2, 1);
			//SpriteBlit(clockanimtex.texdata[m],clockanimtex.desttex,14,-2,sw,sh,dw,dh,dw,0);
			;//SpriteBlit(clockanimtex.texdata[t],clockanimtex.desttex,40,-2,sw,sh,dw,dh,dw,0);
			;//SpriteBlit(clockanimtex.texdata[o],clockanimtex.desttex,65,-2,sw,sh,dw,dh,dw,0);
		} else { // lores
			;//SpriteBlit(clockanimtex.texdata[m],clockanimtex.desttex, 0,-1,sw,sh,dw,dh,dw,0);
			;//SpriteBlit(clockanimtex.texdata[t],clockanimtex.desttex,15,-1,sw,sh,dw,dh,dw,0);
			;//SpriteBlit(clockanimtex.texdata[o],clockanimtex.desttex,27,-1,sw,sh,dw,dh,dw,0);
		}
	}
	;//DumpExistingTex(clockanimtex.mat);
// now they want coins caught
	if (oldncoins != ncoinscaught) {
		oldncoins = ncoinscaught;
		t = (ncoinscaught) / 10;
		o = (ncoinscaught) % 10;
		// coins left
		//	t=(ncoins-ncoinscaught)/10;
		//	o=(ncoins-ncoinscaught)%10;
		//	dw=1<<coinanimtex.mat->thetex->logu;
		//	dh=1<<coinanimtex.mat->thetex->logv;
		//	if (gameplayx>=800) {
		if (usehires) { // hires
#if 0 // update coins caught in display....
			struct bitmap16 *b16;
			b16 = locktexture(numcointex);
			clipxpar16(clocktsp->frames[t], b16, 0, 0, 42, 32 - 5, clocktsp->x, clocktsp->y);
			clipxpar16(clocktsp->frames[o], b16, 0, 0, 67, 32 - 5, clocktsp->x, clocktsp->y);
			unlocktexture(numcointex);
			;//SpriteBlit(clockanimtex.texdata[t],coinanimtex.desttex,42,32-1,sw,sh,dw,dh,dw,0);
			;//SpriteBlit(clockanimtex.texdata[o],coinanimtex.desttex,67,32-1,sw,sh,dw,dh,dw,0);
#endif
		} else { // lores
			;//SpriteBlit(clockanimtex.texdata[t],coinanimtex.desttex,18,16-3,sw,sh,dw,dh,dw,0);
			;//SpriteBlit(clockanimtex.texdata[o],coinanimtex.desttex,28,16-3,sw,sh,dw,dh,dw,0);
		}
		;//DumpExistingTex(coinanimtex.mat);
	}
	// speedo
	o = (int)fabs(accelspin*speedotsp->nframe / startaccelspin);
	if (o < 1)
		o = 1;
	if (o >= speedotsp->nframe)
		o = speedotsp->nframe - 1;
	if (oldspeed != o) {
		oldspeed = o;
		tspsetframe(speedotsp, o); // gets set when tspanimtex gets called
		tspanimtex(speedotsp, 0, 0, 0);
		//	memset(speedoanimtex.desttex,0,speedoanimtex.w*speedoanimtex.h*2);
		//	sw=speedoanimtex.w;
		//	sh=speedoanimtex.h;
		//	dw=1<<coinanimtex.mat->thetex->logu;
		//	dh=1<<coinanimtex.mat->thetex->logv;
		;//SpriteBlit(speedoanimtex.texdata[o],speedoanimtex.desttex,0,0,sw,sh,sw,sh,sw,0);
		;//DumpExistingTex(speedoanimtex.mat);
	}
	// burst
	if (nubursts > 0) {
		if (nubursts >= 20)
			j = 3;
		else if (nubursts >= 10)
			j = 2;
		else if (nubursts >= 5)
			j = 1;
		else
			j = 0;
		if (burstframe == 0)
			playatagsound(FIREBURST);
		//		sw=burstanimtex[j].w;
		//		sh=burstanimtex[j].h;
		//		dw=1<<burstanimtex[j].mat->thetex->logu;
		//		dh=1<<burstanimtex[j].mat->thetex->logv;
		//		memset(burstanimtex[j].desttex,0,burstanimtex[j].w*burstanimtex[j].h*2);
		;//SpriteBlit(burstanimtex[j].texdata[burstanimtex[0].curframe],burstanimtex[j].desttex,0,0,sw,sh,sw,sh,sw,0);
		;//DumpExistingTex(burstanimtex[j].mat);
		tspsetframe(bursttsp[j], burstframe);
		tspanimtex(bursttsp[j], 0, 0, 0);
		burstframe++;
		for (k = 0; k < 4; k++)
			if (k == j) {
				ufireworkscale = range(1.5f, ufireworkscale, 3.0f);
				ubursta[k]->scale.x = .01f*ufireworkscale;
				ubursta[k]->scale.y = .01f*ufireworkscale;
				ubursta[k]->scale.z = .01f*ufireworkscale;
				uburstb[k]->scale.x = .01f*ufireworkscale;
				uburstb[k]->scale.y = .01f*ufireworkscale;
				uburstb[k]->scale.z = .01f*ufireworkscale;
				ubursta[k]->flags &= ~TF_DONTDRAW;
				uburstb[k]->flags &= ~TF_DONTDRAW;
			} else {
				ubursta[k]->flags |= TF_DONTDRAW;
				uburstb[k]->flags |= TF_DONTDRAW;
			}
		if (burstframe == bursttsp[0]->nframe) {
			ufireworkscale -= .1f;
			burstframe = 0;
			nubursts--;
			if (nubursts == 0) {
				for (k = 0; k < 4; k++) {
					unhooktree(ubursta[k]);
					unhooktree(uburstb[k]);
				}
			}
		}
	}
}
#else
void docondom(int)
{

}

void douburst(int)
{

}

// UNC
#endif
VEC ulastpos;
// UNC
#if 1
int fpucwsw;
extern int cbairtime;

void usenewcam()
{
	useoldcam = 0;
}
// UNC
#if 1
/*void spawndirtparts()
{
	static tm;
	TREE *t;
	tm++;
	if ((tm&15)==0) {
		t=duptree(dirtparticle);
		t->transvel.y=.1f;
		t->transvel.x=frand()*.1f-.05f;
		t->transvel.z=frand()*.1f-.05f;
		t->trans=objects[0].pos;
		linkchildtoparent(t,root);
	}
	//	playasound(REDLIGHT,0); // caught a coin
}
*/
// UNC
#endif
	extern int grandtotaltrickpoints, MyGoldCoins;
	extern int totalcrashs, selectedtrack;
	extern int selected;
	extern int releasemode;

} // namespace
using namespace st2;
void uplay3_init()
{
	// UNC
#if 1
	//extern int clipmap;	// for coins in scoreline
	//int clipmapsave;
	//extern TREE *findtreenamerec(TREE *t, char *n);
	//	SPRITE *testsp;
	TREE *grid, *gridloco, *xxx;
	int i, j;//,hflags;
	extern int drawmouse;
	extern int selected, saveslowpo;
	extern int usegdi;
	mater2 *mt;
	//    int ax,ay,ab,mode;

		// temp
	/*	FILE *fh;
		pushandsetdir("temp1");
		fh=fopen3("temp2.txt","rb");
		fclose3(fh);
		pushandsetdir("temp2");
		fh=fopen3("temp1.txt","rb");
		fclose3(fh);
		popdir();
		popdir(); */

	uloop = 2;
	pushandsetdir("st2_config");
	loadconfigfile("config.txt"); // probably can't go back to tim's stuff without
									// reloading the real config.txt
	newpitch.w *= PIOVER180;
	popdir();
	//	halfp=halfy=halfr=0;
// UNC
#endif
// UNC
#if 1
	video_setupwindow(GX, GY);
	intunnels = 0;
	cantstartdrive = 100;
	;//fasteropaquetextures=1; // if source data is totally opaque, take advantage of this
//	selectedcar=TREAD;
	if (gameplayx >= 800)
		usehires = 1;
	else
		usehires = 0;
	//	usehires=0; // force the issue
	useoldcam = 20;
	cantdrive = 1;
	carboost = 0;
	validroadnorm = 0;
	framecount = 0;
	dofinish = 0;
	backcolorred = 41;
	backcolorgreen = 124;
	backcolorblue = 42;
	//alphacutoffsave = alphacutoff;
	//alphacutoff = 137;
	;//if(!usegdi) fmtorm(saveslowpo);
	utotalcrashes = utotalstunts = 0;
	// clock
	clocktickcount = (ustarttime * 30 + 29) * 2;	// 2 minutes
// system
	;//showcursor(0);
	//drawmouse = 0;
// music
	pushandsetdir("st2_main");
	sag = play_sagger("menu.sag");
	popdir();
	;//loadmenumusic();
// setup main tree
	root = alloctree(5000, NULL);
	//	root0 = alloctree(5000,NULL);
	//	root1 = alloctree(5000,NULL);
	//	root2 = alloctree(5000,NULL);
	//	root3 = alloctree(5000,NULL);
	//	linkchildtoparent(root0,root);
	//	linkchildtoparent(root1,root);
	//	linkchildtoparent(root2,root);
	//	linkchildtoparent(root3,root);
	mainvp.backcolor = C32GREEN;
	mainvp.zfront = .125f;
	mainvp.zback = 25000;
	if (zoomin) {
		mainvp.xres = WX / 2;
		mainvp.yres = WY / 2;
		mainvp.xstart = WX / 2;
		mainvp.ystart = WY / 2;
	} else {
		mainvp.xres = WX;
		mainvp.yres = WY;
		mainvp.xstart = 0;
		mainvp.ystart = 0;
	}
	//mainvp.roottree = root;
	//	mainvp.camattach=camnull;//getlastcam(); // from last scene loaded, set later when we
													// get a cam(null)
	mainvp.camzoom = 3.2f; // it'll getit from tree camattach if you have one
	//setviewportsrc(&mainvp); // from last scene loaded
	mainvp.flags = VP_CLEARWB | VP_CLEARBG;

	// get main scene
	pushandsetdir("st2_uplay");
	//usescnlights = 1;
	grid = loadlws("uplay.lws");
	linkchildtoparent(grid, root);
	bigtree = findtreename(root, "tree1.lwo");
	gridloco = findtreename(root, "gridloco"); // fix zbuffer problem, get grass to draw after horizon
	unhooktree(gridloco);
	linkchildtoparent(gridloco, grid);
	//usescnlights = 0;
	startpiece = alloctree(0, "start.lwo");
	finishpiece = loadlws("finish.lws");
	//	pushandsetdir("edpieces");
	//	xxx=loadlws("straight.lws");
	//	popdir();
	//	linkchildtoparent(xxx,finishpiece);
	//nofinishpiece = findtreenamerec(finishpiece, "redx.lwo");
	nofinishpiece = finishpiece->find("coin.lwo");

	linkchildtoparent(startpiece, root);
	linkchildtoparent(finishpiece, root);
	initfinishbursts();
	//	linkchildtoparent(nofinishpiece,root);
	popdir();
	// setup some test nulls
#ifdef USETESTNULLS
	initnulls();
#endif
	initnulls(root, 1);
	// load and build track
	pushandsetdir("st2_edpieces");
	numpieces = st2_loadtracklist2("uedit.txt");
	popdir();
	strcpy(uedittrackname, "track1.txt");
	st2_loadtrack(uedittrackname, 1);
	if (st2_curpiece == 6) { // add an extra straight for a blank track
		st2_curpiece = 13;
		st2_lastpiece = 6;
		st2_dir = 0;
		thetrack[st2_lastpiece].piece = PIECE_GENERIC_3; // straight
		thetrack[st2_lastpiece].orhey = 0;
	}
	// substitute last blank piece with a straight
//	thetrack[st2_curpiece].piece=PIECE_GENERIC_3;
	thetrack[st2_curpiece].piece = PIECE_GENERIC_25; // specialfinish.lws
	thetrack[st2_curpiece].orhey = (float)(st2_dir * 90 + 180); // ah theirs is 180 off
	for (i = 0; i < NUMBODS; i++) {
		TREE *aaa;
		char a[64];
		//		char b[64];
		int p;
		//		TREE *clone;
		logger("<<<<<<<<<<<<<<<<<< trackpiece %d\n", i);
		p = thetrack[i].piece;
		sprintf(a, "%02d", i);
		if (pieces[p].piece)
			thetrack[i].t = duptree(pieces[p].piece);
		else {
			pushandsetdir("st2_edpieces");
			thetrack[i].t = pieces[p].piece = loadlws(pieces[p].name);
			popdir();
		}
		//		getname(b,pieces[p].piece->name);
		//		strcat(b,".lwo");
		thetrack[i].t->rot.y = thetrack[i].orhey *PIOVER180;
		thetrack[i].t->scale.x = 1.02f;	// overlap pieces some, 1% to 2% now 2%
	//	thetrack[i].t->scale.z=1.02f;
		aaa = findtreename(root, a);
		linkchildtoparent(thetrack[i].t, aaa);
		//		clone = findtreebodyname(b);
		//		thetrack[i].bodyid = clone->bodyid;
		// handle collision lists
		if (pieces[p].nlwos == 0) { // no collision list, use old system of children[0]
			thetrack[i].tlwos[0] = thetrack[i].t->children.front();
			//thetrack[i].tlwos[0] = thetrack[i].t->children[0];
			thetrack[i].nlwos = 1;
		} else {
			thetrack[i].nlwos = pieces[p].nlwos; // find name of lwo in lws
			for (j = 0; j < pieces[p].nlwos; j++) {
				if (!my_stricmp(pieces[p].lwonames[j], "coin.lwo")) // coin might be freed by current system
					errorexit("no coins in 'uedit.txt' please");
				//if ((thetrack[i].tlwos[j] = findtreenamerec(thetrack[i].t, pieces[p].lwonames[j])) == NULL)
				if ((thetrack[i].tlwos[j]->find(pieces[p].lwonames[j])) == NULL)
					errorexit("can't find '%s' in '%s'",
						pieces[p].lwonames[j], thetrack[i].t->name.c_str());
			}
		}
		// build collision grid
		for (j = 0; j < thetrack[i].nlwos; j++) {
			int k;
			int gx, gz, gxmin, gzmin, gxmax, gzmax;
			COLLGRID *cg;
			TREE *t;
			model *b;
			VEC *v;
			vector<FACE> f;
			int nf;
			int nv;
			logger("<<<<< lwo %d\n", j);
			cg = &thetrack[i].clwos[j];
			t = thetrack[i].tlwos[j];
			if (!t->mod)
				errorexit("bad bodyid in grid control");
			b = t->mod;
			v = b->verts;
			f = b->faces;
			nf = b->faces.size();
			nv = b->verts.size();
			logger("   '%s has %d faces'\n", thetrack[i].tlwos[j]->name.c_str(), nf);
			cg->mulx = (ST2_COLLGRIDX - .2f) / (b->boxmax.x - b->boxmin.x); // safety factor .1 on each side
			cg->offx = .1f - cg->mulx*b->boxmin.x;
			cg->mulz = (ST2_COLLGRIDZ - .2f) / (b->boxmax.z - b->boxmin.z); // safety factor .1 on each side
			cg->offz = .1f - cg->mulz*b->boxmin.z;
			cg->verts = v;
			memset(cg->nfaceidx, 0, ST2_COLLGRIDX*ST2_COLLGRIDZ * sizeof(int));
			// pass 1, run thru the tris and count them up
			for (k = 0; k < nf; k++) {
				getgridxz(cg, v + f[k].vertidx[0], &gxmin, &gzmin);
				gxmax = gxmin;
				gzmax = gzmin;
				getgridxz(cg, v + f[k].vertidx[1], &gx, &gz);
				if (gx < gxmin)
					gxmin = gx;
				if (gz < gzmin)
					gzmin = gz;
				if (gx > gxmax)
					gxmax = gx;
				if (gz > gzmax)
					gzmax = gz;
				getgridxz(cg, v + f[k].vertidx[2], &gx, &gz);
				if (gx < gxmin)
					gxmin = gx;
				if (gz < gzmin)
					gzmin = gz;
				if (gx > gxmax)
					gxmax = gx;
				if (gz > gzmax)
					gzmax = gz;
				//				gzmin=0;
				//				gzmax=0;
				//				gxmin=0;
				//				gxmax=0;
				for (gz = gzmin; gz <= gzmax; gz++)
					for (gx = gxmin; gx <= gxmax; gx++) {
						VEC minv, maxv;
						getvecfromgridxz(cg, &minv, gx, gz);
						getvecfromgridxz(cg, &maxv, gx + 1, gz + 1);
						if (tri2rectxz(v + f[k].vertidx[0], v + f[k].vertidx[1], v + f[k].vertidx[2], &minv, &maxv)) {
							cg->nfaceidx[gz][gx]++;
						}
					}
			}
			// alloc memory for grid
			for (gz = 0; gz < ST2_COLLGRIDZ; gz++)
				for (gx = 0; gx < ST2_COLLGRIDX; gx++) {
					cg->faceidx[gz][gx] = (FACE **)memalloc(sizeof(int)*(cg->nfaceidx[gz][gx] + 2));
					cg->nfaceidx[gz][gx] = 0;
				}
			// pass 2, run thru the tris and count them up, collect tri #'s
			for (k = 0; k < nf; k++) {
				getgridxz(cg, v + f[k].vertidx[0], &gxmin, &gzmin);
				gxmax = gxmin;
				gzmax = gzmin;
				getgridxz(cg, v + f[k].vertidx[1], &gx, &gz);
				if (gx < gxmin)
					gxmin = gx;
				if (gz < gzmin)
					gzmin = gz;
				if (gx > gxmax)
					gxmax = gx;
				if (gz > gzmax)
					gzmax = gz;
				getgridxz(cg, v + f[k].vertidx[2], &gx, &gz);
				if (gx < gxmin)
					gxmin = gx;
				if (gz < gzmin)
					gzmin = gz;
				if (gx > gxmax)
					gxmax = gx;
				if (gz > gzmax)
					gzmax = gz;
				//				gzmin=0;
				//				gzmax=0;
				//				gxmin=0;
				//				gxmax=0;
				for (gz = gzmin; gz <= gzmax; gz++)
					for (gx = gxmin; gx <= gxmax; gx++) {
						VEC minv, maxv;
						getvecfromgridxz(cg, &minv, gx, gz);
						getvecfromgridxz(cg, &maxv, gx + 1, gz + 1);
						if (tri2rectxz(v + f[k].vertidx[0], v + f[k].vertidx[1], v + f[k].vertidx[2], &minv, &maxv)) {
							cg->faceidx[gz][gx][cg->nfaceidx[gz][gx]] = &f[0] + k;
							cg->nfaceidx[gz][gx]++;
						}
					}
			}
		}
	}
	scantrackpieces();
	initcoins();
	setupmatfuncs();
	// build up our car
	st2_buildcar();
	// setup objects
	memcpy(objects, iobjects, sizeof(objects));
	//	objects[0].pos.x=100;
	//	roadprobe1.z=100;
	objects[0].carang.x = 0; // yaw car 90 degrees to the right
	objects[0].carang.y = 1;
	objects[0].carang.z = 0;
	objects[0].carang.w = PI / 2.0f;
	rotaxis2quat(&objects[0].carang, &objects[0].carang);
	// build up our camera
	//	pushandsetdir("helperobj");
	/*camnull = allochelper(1, HELPER_CAMERA, 0);
	//	camnull=alloctree(1,"helpercamera.lwo");
	//	popdir();
	//	camnull=alloctree(1,nullname);
	camnull->buildo2p = O2P_FROMTRANSQUATSCALE;
	camnull->rot.w = 1;
	camnull->zoom = thecamzoom;
	camnull->trans = camstart;
	;//addlwscam(camnull); // hi level camera
	;//curlwscam = nlwscams-1; // hi level camera
	mainvp.camattach = camnull;
	linkchildtoparent(camnull, root);*/
	;//cam.useroot=0; // lo level camera
// set misc variables
	startstunt = uready = 0;
	accelspin = 0;
	littleg = littlegground;
	// get some sound
	inittags();
	numwaves = taggetnum(gardentags);
	;//pushandsetdir("game");
	;//if (!Sound_LoadTag("garden.tag", &numwaves)) errorexit("Sound_LoadTag Failed");
	;//popdir();
//	revsound = Sound_FindSlot(MENUREV1+selectedcar); // good idea but wrong sounds
// load up scrline
	if (usehires)
		//	if (slowpo)
		//	if (gameplayx>=800)
		pushandsetdir("st2_hiscrline"); // hires
	else
		pushandsetdir("st2_scrline"); // lores
	//clipmapsave = clipmap;	// force all texture to be loaded to be colorkeys
	//clipmap = CLIPMAP_COLORKEY;
	scrline = loadlws("scrline.lws");
	//clipmap = clipmapsave;
	linkchildtoparent(scrline, camnull);
	findtreename(root, "light.lwo")->flags |= TF_DONTDRAW;
	findtreename(root, "light2.lwo")->flags |= TF_DONTDRAW;
	logger("loading clock textures (digits)..\n");
	oldtt = oldncoins = oldspeed = -1;
	// clock
	clocktsp = loadtsp("clock.tsp", NULL, root, "scrline.lwo", "clock", TSP_USE_1ALPHA, 1);
	;//clockanimtex.mat = findmaterial(xxx,"clock");
	;//if(!clockanimtex.mat)
	;//	error("Can't find clock surface");
	;//clockanimtex.desttex=gettexptr(clockanimtex.mat,&clockanimtex.w,&clockanimtex.h,NULL);
	;//if(!clockanimtex.desttex)
	;//	error("clockdata is null");
// coin
	xxx = findtreename(root, "scrline.lwo");
	if (!xxx)
		errorexit("cant find scrline.lwo");
	mt = xxx->findmaterial("scrline3");
	if (!mt)
		errorexit("Can't find coin surface");
	// coin tex should be same as clock tex

/*	numcointex=buildtexture("ncointex",1<<mt->thetex->logu,1<<mt->thetex->logv,clocktsp->texformat);
	if (!numcointex)
		errorexit("can't build cointex");
	freetexture(mt->thetex);
	mt->thetex=numcointex;
*/
	numcointex = mt->thetexarr[0];

	;//coinanimtex.desttex=gettexptr(coinanimtex.mat,&coinanimtex.w,&coinanimtex.h,NULL);
	;//if(!coinanimtex.desttex)
	;//	error("coindata is null");
// speedo
	speedotsp = loadtsp("speed.tsp", NULL, root, "scrline.lwo", "speed", TSP_USE_1ALPHA, 1);
	/*	mt = findmaterial(xxx,"speed");
		if(!mt)
			errorexit("Can't find speedo surface");
		speedotex=mt->thetex; */
	;//if(!speedoanimtex.mat)
	;//	error("Can't find speedo surface");
	;//speedoanimtex.desttex=gettexptr(speedoanimtex.mat,&speedoanimtex.w,&speedoanimtex.h,NULL);
	;//if(!speedoanimtex.desttex)
	;//	error("speedodata is null");
// get clock tsp
	//loadtsp("clock.tsp");
//	clockanimtex.nframes = ReadTSP(&testsp,"clock.tsp");
//	if(clockanimtex.nframes<=0)
//		error("can't find clock.tsp");
//	for(i=0;i<clockanimtex.nframes;i++) {
//		;/*superconvert24to16(
//			(unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
//			NULL,0,0,
//			SU_COLORKEY|SU_NEW,
//			&clockanimtex.texdata[i],&clockanimtex.w,&clockanimtex.h,
//			NULL,&hflags,1);
//			&clockanimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures */
//	}
//	FreeTSP(testsp,clockanimtex.nframes);
// get speed tsp
	logger("loading speedo textures (digits)..\n");
	//	speedoanimtex.nframes = ReadTSP(&testsp,"speed.tsp");
	//	if(speedoanimtex.nframes<=0)
	//		error("can't find speedo.tsp");
	//	for(i=0;i<speedoanimtex.nframes;i++) {
	//		;/*superconvert24to16(
	//			(unsigned char *)testsp[i].data,testsp[i].w,testsp[i].h,
	//			NULL,0,0,
	//			SU_COLORKEY|SU_NEW,
	//			&speedoanimtex.texdata[i],&speedoanimtex.w,&speedoanimtex.h,
	//			NULL,&hflags,1);
	//			&speedoanimtex.mat->mhflags,NULL,1); // take advantage of fasteropaquetextures */
	//	}
	//	FreeTSP(testsp,speedoanimtex.nframes);
	popdir();
	// turn on animation
	kill_bcams(grid);
	seq_start(grid);
	seq_start(finishpiece);
	//	firstsound=1;
	//calclightonce(grid);
	// force the issue
	;//curlwscam=0;
	;//cam.attach=camnull;
	;//lwscamlist[0]=camnull;
	;//cam.useroot=0;
	pushandsetdir("st2_game");
	uinitparticles();
	//	dirtparticle=alloctree(1,"dust4.lwo");
	//	dirtparticle->proc=partfunc;
	//	dirtparticle->user1=60;
	//	dirtparticle->flags|=TF_ALWAYSFACING;
	//	dirtparticle->treedissolvecutoff=10/256.0f;
	//    if (usehardware==0) {
	//		MAT *m;
	//		togglezbuffer(dirtparticle,"dust",1);
	//		m = findmaterial(dirtparticle,"dust");
	//		m->mtrans=.5f;
	//	}
	popdir();
	// log results
	fixpondalphacutoff(root);

	/*  if(!usegdi && xres != gameplayx && yres != gameplayy)  //in init
	  {
		if(usehardware) flush3d();

		mode = VidFindClosest(gameplayx,gameplayy);
		if(mode<0) errorexit("can't set %d %d",gameplayx,gameplayy);
		VidSet(mode);
		VidGetInfo(mode, &ax,&ay,&ab);
		gameplayx = ax;
		gameplayy = ay;
		bitmode = ab;

		if(usehardware)
		{
			Init3d(0, 0, gameplayx, gameplayy);
			restoreallsurfaces();
			SetHardwareFlags();
		}
	  } */
	  // put the sky to low priority drawing
	  // setup drawpriority
	{
		TREE *skydome;
		skydome = findtreename(root, "dome1.lwo");
		//		unhooktree(skydome);
		//		linkchildtoparent(skydome,root);
		//setdrawprirec(skydome, 0);
		//		skydome->mod->drawpri=0;
		//		settreedraworder(root);
		//useattachcam = 1;
	}
	//logviewport(&mainvp, OPT_SOME);
#endif //UNC
}


void uplay3_proc()
{
	//	extern int reversedraw;
	VEC yv;
	int i, j;
	//	getfpucwsw(&fpucwsw);
	//	fpuexceptions(1);
	//	if (KEY=='r')
	//		reversedraw^=1;
	//if (KEY == 's')
	//	testcapture();
	if (KEY == K_ESCAPE /* || wininfo.keystate[DIK_JOYBUTTON(3)]) && !dofinish */) {
		if (!dofinish)
			Esc_out = 1;
		//		dofinish=1;
		;//AckKey(DIK_ESCAPE);
		;//changestate(EXITSTATE);
		popstate();
	}
	if (KEY == 'z') {
		//zoomin^=1;
		if (zoomin) {
			mainvp.xres = WX / 2;
			mainvp.yres = WY / 2;
			mainvp.xstart = WX / 2;
			mainvp.ystart = WY / 2;
		} else {
			mainvp.xres = WX;
			mainvp.yres = WY;
			mainvp.xstart = 0;
			mainvp.ystart = 0;
		}
	}
// disable change resolution on the fly
#if 0
	if ((KEY == '-' || KEY == '=') && video_maindriver != VIDEO_D3D) {
		int dir;
		if (KEY == '-')
			dir = -1;
		else
			dir = 1;
		if (changexyres(globalxres, globalyres, dir, &globalxres, &globalyres)) {
			video_setupwindow(globalxres, globalyres, 565);
			if (zoomin) {
				mainvp.xres = WX / 2;
				mainvp.yres = WY / 2;
				mainvp.xstart = WX / 2;
				mainvp.ystart = WY / 2;
			} else {
				mainvp.xres = WX;
				mainvp.yres = WY;
				mainvp.xstart = 0;
				mainvp.ystart = 0;
			}
		}
	}
#endif

	//#ifdef USETESTNULLS
	resetnulls();
	addnull(&roadprobe1, 0);
	yv.x = roadprobe2.x + roadprobe1.x;
	yv.y = roadprobe2.y + roadprobe1.y;
	yv.z = roadprobe2.z + roadprobe1.z;
	addnull(&yv, 0);
	if (st2_line2road(&roadprobe1, &yv, &roadprobei, &roadprobenorm)) {
		addnull(&roadprobei, 0);
		roadprobenorm.x += roadprobei.x;
		roadprobenorm.y += roadprobei.y;
		roadprobenorm.z += roadprobei.z;
		addnull(&roadprobenorm, 0);
	} else {
		addnull(&zerov, 0);
		addnull(&zerov, 0);
	}
	addnull(&landspot, 0);
	//#endif
	finishpiece->trans.y = finishy;
	///////////// okay now do the physics/math
	if ((framecount & 0x3f) == 0x3f) {
		if (wininfo.fpsavg2 > 25) {// 30
			uloop = 2;
			wininfo.fpswanted = 30;
		} else if (wininfo.fpsavg2 > 17) {// 20
			uloop = 3;
			wininfo.fpswanted = 20;
		} else { //if (avg1fps>13) // 15
			uloop = 4;
			wininfo.fpswanted = 15;
		}
		//	else //if (fpsavg1>10) // 12 and below
		//		uloop=5;
		//	else
		//		uloop=6;
	}
	timeinc = 1 / 60.0f;
	ulastpos = objects[0].pos;
	for (j = 0; j < uloop; j++) {
		useoldcam++;
		if (useoldcam > 3) {
			useoldcam = 3;
			testnewcam = 0;
		} else
			testnewcam = 1;
		if (intunnels)
			intunnels--;
		if (dofinish)
			dofinish++;
		if (dofinish == 480)
			popstate(); //changestate(FINISHSTATE);
		drivecar();
		// collide objects
		if (airtime >= 0)
			airtime++;
		if (cbairtime >= 0)
			cbairtime++;
		if (ufliptime >= 0)
			ufliptime++;
		if (cantstartdrive > 0)
			cantstartdrive--;
		//	justshocks++;
		if (doacrashreset)
			doacrashreset++;
		calccollisioninfo(); // bbox , points, moment of interia
#ifdef USETESTNULLS
		for (i = SHOCKPOINTOFF; i < SHOCKPOINTOFF + NSHOCKPOINTS; i++)
			setanullpos(&objects[0].pr[i], i + 5, 1);
		for (i = RECTPOINTOFF; i < RECTPOINTOFF + NRECTPOINTS; i++)
			setanullpos(&objects[0].pr[i], i + 5, 1);
		setanullpos(&zerov, 23, 1);
		setanullpos(&crashresetloc, 24, 0);
#endif
		//find intersection, calc impulse, adjust trans and rot velocities
		;//perf_start(DOROADCOLLISIONS);
		doroadcollisions(); // obj to road, will zero airtime if car is in contact with ground
		;//perf_end(DOROADCOLLISIONS);
		// extra forces
		if (airtime > stunttime || manualcar) {
			accelspin = 0;
			flymode = 1;
			cbairtime = airtime;
			if (objects[0].wheelvel[0] > 0)
				for (i = 0; i < 6; i++)
					objects[0].wheelvel[i] -= 1;
			else
				for (i = 0; i < 6; i++)
					objects[0].wheelvel[i] += 1;
		} else { // on ground
			ucarrotvel = zerov;
			littleg = littlegground;
			if (airtime == -1) {
				ttp = 0; // you lose the stunts
//				halfp=halfy=halfr=0;
				doacrashreset = 1;
				airtime = 0;
			}
			if (flymode && !doacrashreset) { // just landed
				cantdrive = 0;
				if (yawpiesum >= 2)
					ttp++;
				if (ttp == 0) {
					playatagsound(14 + mt_random(3)); // land
				} else {
					if (ttp > 1) {
						doblueflash(15);
						dolightning(8);
						douflames(4);
					}
					douburst(ttp);
				}
				/*				if (ttp==1) { // one
									douburst(1);
								} else if (ttp==2) { // two
									douburst(2);
									doblueflash(20);
									dolightning(3);
								} else if (ttp==3) { // three
									douburst(3);
									dolightning(3);
								} else if (ttp==4) { // four
									douburst(2);
									doblueflash(20);
									dolightning(2);
								} else if (ttp==5) { // five
									douburst(3);
									doblueflash(30);
									dolightning(3);
								} else if (ttp>=6) { // six or more stunts
									douburst(4);
									doblueflash(40);
									dolightning(4);
								} */
				utotalstunts += ttp;
				carboost = ttp;
				ttp = 0;
			}
			flymode = 0;
		}
		if (!manualcar) {
			// move objects
			updatetrans();
		} else {
			objects[0].carvel = zerov;
		}
		if (selectedcar == RADAR) {
			// radar
			desiredradarangle = normang(desiredradarangle + radardir * radarspeed);
			if (normang(radartree->rotvel.y - desiredradarangle) >= 180)
				radartree->rotvel.y = normang(radartree->rotvel.y + radarspeed);
			else
				radartree->rotvel.y = normang(radartree->rotvel.y - radarspeed);
			if (targetradarangle >= 0) {
				float na, ta;
				ta = normang(targetradarangle - testcar.y);
				na = normang(radartree->rotvel.y - ta);
				if (na >= radarextraang) {
					if (na < 180) {
						radardir = -1;
					} else if (na < 360 - radarextraang) {
						radardir = 1;
					}
				}
			}
		} else if (selectedcar == TREAD) {
			// tread
		//			if (treadanimtex.curframe!=treadanimtex.lastframe) {
		//				;//SpriteBlit(treadanimtex.texdata[treadanimtex.curframe],treadanimtex.desttex,0,0,
		//				 //	treadanimtex.w,treadanimtex.h,treadanimtex.w,treadanimtex.h,treadanimtex.w,0);
		//				;//DumpExistingTex(treadanimtex.mat);
		//				treadanimtex.lastframe=treadanimtex.curframe;
		//			}

		}
		updaterots(); // but i'll do the stunt!
	}
	drawobjects();
	coincollisions();
	uplayupdateclock();
	// test sound ................................
	if (KEY == 'a') {
		;//AckKey(DIK_A);
		playatagsound(curwave);
		curwave++;
		if (curwave == numwaves)
			curwave = 0;
	}
	if (KEY == 'h') {
		;//AckKey(DIK_H);
		if (!releasemode)
			objects[0].pos.y = 10;
	}
	if (!motorsh)
		motorsh = wave_playvol(taggetwhbyidx(gardentags, selectedcar), 0, .75f);
	if (motorsh)
		setsfxfreq(motorsh, (float)uplayrevfreq / motorsh->basefreq);
	;//Sound_SetVolume(revsound,uplayrevvolume);
/*	if (firstsound) {
		firstsound=0;
		;//revsound = Sound_FindSlot(selectedcar+1); // one car sound
		;//Sound_Play(revsound, 0,0,0, 1);
	} */
	// finish up
	;//dohilevelcamera();
	;//defaultproc();
	lup = uup;
	ldown = udown;
	lleft = uleft;
	lright = uright;
	lrollleft = urollleft;
	lrollright = urollright;
	// see if we crossed the finish line..
	if (!dofinish) {
		if (udirpiece & 1) { // cross in z
			if (objects[0].pos.x > finishpiece->trans.x - 1.5f && objects[0].pos.x < finishpiece->trans.x + 1.5f) {
				if (objects[0].pos.z > finishpiece->trans.z && ulastpos.z <= finishpiece->trans.z)
					dofinish = 1;
				if (objects[0].pos.z < finishpiece->trans.z && ulastpos.z >= finishpiece->trans.z)
					dofinish = 1;
			}
		} else { // cross in x
			if (objects[0].pos.z > finishpiece->trans.z - 1.5f && objects[0].pos.z < finishpiece->trans.z + 1.5f) {
				if (objects[0].pos.x > finishpiece->trans.x && ulastpos.x <= finishpiece->trans.x)
					dofinish = 1;
				if (objects[0].pos.x < finishpiece->trans.x && ulastpos.x >= finishpiece->trans.x)
					dofinish = 1;
			}
		}
		if (dofinish == 1 && !(nofinishpiece->flags&TF_DONTDRAW)) {
			dofinish = 0;
			doacrashreset = 1;
		}
		if (dofinish)
			playatagsound(ZING2); // finishsound
	}
	//doanims(root);
	root->proc();
	// hi level camera
	doflycam(&mainvp);
	// prepare scene
	//buildtreematrices(root); //roottree,camtree);
	video_buildworldmats(root);
	dolights();
	// draw scene
	//video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(root);


}

void uplay3_draw3d()
{
	// draw
	//video_buildworldmats(roottree); // help dolights
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);

	// disable zoomin
#if 0
	if (zoomin) {
		video_lock();
		zoombitmap16(B16);
		video_unlock();
	}
#endif
	framecount++;
}

void uplay3_exit()
{
	//	extern TOURNEY_STRUCT tourney[];
	//;//int ax,ay,ab;
	//;//int mode;
	int i, j, k;
	;//fasteropaquetextures=0;	// set back to 0 for the newbies
	utimeleft = clocktickcount / 60; // convert to seconds
	if (selectedtrack < 7 || selectedtrack>10)
		selectedtrack = 7;
	;//tourney[selectedtrack].tms = ustarttime-utimeleft; // what track do i select ????
	;//if (tourney[selectedtrack].tms>=9*60+59)
	;//	tourney[selectedtrack].tms=9*60+59;
	totalcrashs = utotalcrashes;
	MyGoldCoins = ucoinscaught = ncoinscaught;
	grandtotaltrickpoints = utotalstunts;

	freefinishbursts();
	for (j = 0; j < 4; j++) {
		freetree(ubursta[j]);
		freetree(uburstb[j]);
	}
	//	freetree(dirtparticle);
	ufreeparticles();
	for (i = 0; i < NUMBODS; i++) {
		for (j = 0; j < thetrack[i].nlwos; j++) {
			FACE ***f = &thetrack[i].clwos[j].faceidx[0][0];
			for (k = 0; k < ST2_COLLGRIDZ*ST2_COLLGRIDX; k++) {
				if (*f) {
					memfree(*f);
					*f = NULL;
				}
				f++;
			}
		}
	}
	;//defaultexit();
// after defaultexit, no trees are alloced
	//if (nulightnings)
	//	;//Sound_StopSound(lightningsound);
	nulightnings = nubursts = nucondoms = nuflames = 0;
	nuflashes = 1;	// force texture
	;//Sound_StopSound(revsound);
//	Sound_StopSound(stuntsound);
	;//loadmenutag();
	if (!slowpo) {
		freetsp(specialtsp);
		/*		for(i=0;i<specialanimtex.nframes;i++) {
					if(specialanimtex.texdata[i]) {
						memfree(specialanimtex.texdata[i]);
						specialanimtex.texdata[i]=NULL;
					}
				}
				specialanimtex.nframes=0; */
	}
	/*	for(i=0;i<carbodyanimtex.nframes;i++) {
			if(carbodyanimtex.texdata[i]) {
				memfree(carbodyanimtex.texdata[i]);
				carbodyanimtex.texdata[i]=NULL;
			}
		} */
	freetsp(flashtsp);
	//	for(i=0;i<goldencondomanimtex.nframes;i++) {
	//		if(goldencondomanimtex.texdata[i]) {
	//			memfree(goldencondomanimtex.texdata[i]);
	//			goldencondomanimtex.texdata[i]=NULL;
	//		}
	//	}
	freetsp(condomtsp);
	//	for(i=0;i<flameranimtex.nframes;i++) {
	//		if(flameranimtex.texdata[i]) {
	//			memfree(flameranimtex.texdata[i]);
	//			flameranimtex.texdata[i]=NULL;
	//		}
	//	}
	freetsp(flametsp);
	//	for (i=0;i<clockanimtex.nframes;i++) {
	//		if(clockanimtex.texdata[i]) {
	//			memfree(clockanimtex.texdata[i]);
	//			clockanimtex.texdata[i]=NULL;
	//		}
	//	}
	//	for (i=0;i<speedoanimtex.nframes;i++) {
	//		if(speedoanimtex.texdata[i]) {
	//			memfree(speedoanimtex.texdata[i]);
	//			speedoanimtex.texdata[i]=NULL;
	//		}
	//	}
	freetsp(speedotsp);
	for (j = 0; j < 4; j++)
		freetsp(bursttsp[j]);
	/*		for (i=0;i<burstanimtex[j].nframes;i++) {
				if(burstanimtex[j].texdata[i]) {
					memfree(burstanimtex[j].texdata[i]);
					burstanimtex[j].texdata[i]=NULL;
				}
			} */
			//	for (i=0;i<treadanimtex.nframes;i++) {
			//		if(treadanimtex.texdata[i]) {
			//			memfree(treadanimtex.texdata[i]);
			//			treadanimtex.texdata[i]=NULL;
			//		}
			//	}
			//	treadanimtex.nframes=0;
	freetsp(clocktsp);

	/*  if(!usegdi) fmtorm(0);
	  if(!usegdi && xres != gameplayx && yres != gameplayy)
	  {
		if(usehardware) flush3d();
		mode = VidFindExact(xres,yres);
		if(mode<0) errorexit("can't set %d %d",gameplayx,gameplayy);
		VidSet(mode);
		VidGetInfo(mode, &ax,&ay,&ab);
		bitmode = ab;

		if(usehardware)
		{
			Init3d(0, 0, xres, yres);
			restoreallsurfaces();
			SetHardwareFlags();
		}
	  }
	  alphacutoff=alphacutoffsave; */
	wininfo.fpswanted = 30;
	freetree(root);
	exittags();
	showcursor(1);
	free_sagger(sag);
	//usescnlights = 1;
}
// UNC
#endif
//}
