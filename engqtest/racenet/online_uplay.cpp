#if 0
#include <windows.h>
#include <d3d.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <engine1.h>
#include <misclib.h>
#include "videoi.h" // access video_maindriver
#include "vidd3d.h"

#include "stubhelper.h"

#include "globalres.h"
#include "line2road.h"
#include "crasheditor.h"
#include "boxai.h"

//#include "pieces.h"
#include "debprint.h" // for loadconfigfile
#include "online_uplay.h"
#include "box2box.h"
#include "online_uphysics.h"
#include "soundlst.h"
#include "tspread.h"
#include "mainmenu.h" // for changeresolution
#include "online_seltrack.h" // for reading online_seltrackdfg.txt
//#include "online_varchecker.h"
#include "packet.h"
#include "onlinecars.h"
//#include <stubhelper.h>
#include "perf.h"
#include "online_rematchres.h"
#include "scrline.h"
#include "camera.h"

#include "gamestate.h"

#include "bluered.h"
// temp debug
//int inlogmode;
// end temp debug
#endif
#define RES3D // alternate shape class
#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "../engine7test/n_usefulcpp.h"
#include "gameinfo.h"
#include "stubhelper.h"
//#include "line2road.h"
//#include "../engine7test/n_line2roadcpp.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "crasheditor.h"
#include "newlobby.h"
#include "online_seltrack.h"
#include "../u_states.h"
#include "../engine7test/n_jrmcarscpp.h"
#include "hw35_boxai.h"
#include "packet.h"
#include "online_uphysics.h"
#include "box2box.h"
//#include "tspread.h"
#include "soundlst.h"
#include "gamestate.h"
#include "camera.h"
#include "scrline.h"
#include "tsp.h"

#include "../u_modelutil.h"

#include "match_logon.h"

// for fog
#include <d3d9.h>
#include <m_vid_dx9.h>

#include "../u_s_toon.h"
#include "../helperobj.h"
#include "carclass.h"
#include "broadcast_server.h"
#include "coins.h"
//#include "../particles.h"

#define linkchildtoparent(c,p) p->linkchild(c)
#define duptree(t) t->newdup()
#define findtreenamerec(t,n) t->find(n)
#define alloctree(nc,name) new tree2(name)
#define freetree(t) delete t
#define freetexture(tx) textureb::rc.deleterc(tx)
#define locktexture(t) t->locktexture()
#define unlocktexture(t) t->unlocktexture()
#define findmaterial(t,matname) t->findmaterial(matname)
#define unhooktree(t) t->unlink()
static tree2* mainvproottree;//,*mainvp2roottree;
static viewport2 mainvp2;
static S32 runinbackgroundsave;
static tree2* nullobj;
vector<tree2*> nulllist;

helperobj* ho;

// do fog
fogparms fog1,fog2;
//particle* parts;

void setfog(S32 fogslot) // 0 (0->1), 1 (1->0), 2 (2->0)
{
	switch(fogslot) {
	case 0: // init
	case 1: // slot 1
		videoinfodx9.fog=fog1;
		break;
	case 2: // slot 2
		videoinfodx9.fog=fog2;
		break;
	}
}
// done fog

void addnullinit()
{
	pushandsetdir("gfxtest");
	nullobj=buildprism(pointf3x(.1f,100,.1f),"maptestnck.tga","tex");
//	nullobj=new tree2("anullobj");
	popdir();
}

void addnull(const pointf3* p,const pointf3* r)
{
	tree2* d=nullobj->newdup();
	d->trans=*p;
	d->rot=zerov;//*r;
	mainvproottree->linkchild(d);
	nulllist.push_back(d);
}

void addnullcleanup()
{
	vector<tree2*>::const_iterator i;
	for (i=nulllist.begin();i!=nulllist.end();++i)
		delete(*i);
	nulllist.clear();
}

void addnullexit()
{
	delete nullobj;
	nullobj=0;
}

static textureb* buildtexture(const C8* texname,S32 x,S32 y,S32 format)
{
	textureb* ret=texture_create(texname);
	bitmap32* dum=bitmap32alloc(x,y,C32BLACK);
	ret->addbitmap(dum,false);
	bitmap32free(dum);
	return ret;
}

//struct tsp *loadtsp(char *name,char *alphaname,
//					struct tree *roottree,char *treename,char *matname,
//					int useformat,int keepoldsize);
//struct tsp *loadtspo(char *name,char *alphaname,int texformat,int checktexsize);
//struct tsp *loadtspscript(char *name,char *alphaname,int texformat);
//void freetsp(struct tsp *t);
//void tspanimtex(struct tsp *t,int offx,int offy,int xpar);
//void tspsetframe(struct tsp *t,int frame); // gets set when tspanimtex gets called
//struct tsp *duptsp(struct tsp *);
void initifls(struct model *mod,struct mat *m);
//tag* tagread(const C8*);
//void tagfree(tag* t);
//tag* vagread(const C8* const *,S32 nvags);
//wavehandle* taggetwhbyidx(tag* tagi,S32 idx);
//sagger* play_sagger(const C8* name);
//void free_sagger(sagger* saga);
//S32 taggetnum(tag*);
static void ol_dolightning(int);
struct customstub carstubinfos[MAXSTUBCARS];


#define DESIREDBPP 565

#define FONTWIDTH 16
#define FONTHEIGHT 20

//JAY
#define RADAR_POS_X (WX>>3)
#define RADAR_POS_Y ((WY>>4)+(WY>>6))
//#define RADAR_POS_X (WX / 2 - WX / 125)
//#define RADAR_POS_Y (WY - (WY>>4))
#define RADAR_RANGE (15.0f)
#define RADAR_SCALE_X (WX / 16)
#define RADAR_SCALE_Y (WY / 18)

//#define BEHINDFIREWALL

//#define USEROADPROBE
#ifdef USEROADPROBE
pointf3 roadprobe1,roadprobe2,roadprobei,roadprobenorm;
#endif

void onlineracemain_init(),onlineracemain_proc(),onlineracemain_exit();
void carextractstate_init(),carextractstate_proc(),carextractstate_exit();
//static struct state mainracestate={onlineracemain_init,onlineracemain_proc,onlineracemain_exit};
//static struct state carextractstate={carextractstate_init,carextractstate_proc,carextractstate_exit};
struct gamecfg olracecfg;
// 1 debug string
//static struct reslist *game_rl;
static shape* game_rl;
static text* PTEXTQ,*PTEXTQ2;
static pbut* PBUTYES,*PBUTNO;
static back* BACK;
listbox* GAMENEWS;
static shape* focus;
//static char teststr0[300];//="HELLO";
//static char teststr1[300];//="HELLO";
//static char teststr2[300];//="HELLO";
struct ol_data od; // data
struct ol_playerdata opa[OL_MAXCARSLOTS]; // player data
static struct playerstatepacket opasave[OL_MAXCARSLOTS]; // player data from some time ago...
static struct playerstatepacket opasave0[OL_MAXCARSLOTS]; // player data from clocktickcount=0
struct globalstate globalstatepacket,globalstatepacket0; // external stuff like missiles, ecto
 //static struct  ol_playerdata opasave2[OL_MAXCARSLOTS]; // player data from some time ago...
static struct ol_playerdata ocp;//,*op=&opa[0];	// player data template, copy to other players
//struct ol_playerdata *op=&opa[0];	// pointer to current player data
struct ol_playerdata *op;	// pointer to current player data, called on for each player
struct ol_playerdata *of;	// pointer to player with the camera focus..

//struct onlineopt ol_opt;

/*
//RULE
struct rule rules[MAX_RULES]={
//																										NYI    NYI
//	rulename			windowname				scorelinet			scorelineb		useweap		ctf dolaps stunts
	{NULL},
	{"no weapons",		"NO WEAPONS 6.19",		"scrlnetop2.mxs",	NULL,				0,		0,   0},
	{"combat",			"CARENA VERSION 6.19","scrlnetop2.mxs","scrlnebtm3.mxs",	1,		0,   0},
	{"CTF no weapons",	"CTF NO WEAPONS 13.29",	"scrlnetop3.mxs",	NULL,				0,		1,   0},
	{"CTF weapons",		"CTF WEAPONS 13.29",	"scrlnetop3.mxs",	"scrlnebtm3.mxs",	1,		1,   0},
	{"stunt",			"STUNT 13.29",			"scrlnetop2.mxs",	NULL,				0,		0,   0},
	{"no weapons boxes","NO WEAPONS 13.29",		"scrlnetop2.mxs",	NULL,				0,		0,   1},
	{"combat boxes",	"HIGHWAY 35 VERSION 14.17","scrlnetop2.mxs","scrlnebtm3.mxs",	1,		0,   1},
};
*/
struct rule *currule;

//////// const data
/*struct ol_dembonec ol_dembonesc[COLBONESX][COLBONESZ]={
	{{"colbone6",1,0,7},
	{"colbone3",4,0,0,},
	{"colbone2",1,0,-9}},
	{{"colbone5",-1,0,10},
	{"colbone4",-7,0,0},
	{"colbone1",-1,0,-9}},
};*/

OL_CARLIST ol_allcars[] = {
 {NULL,NULL,"boonyn.lws","boonpwup.lwo","boonshad.lwo","boonyn.txt","boonyn.lwo","boonynbody","carclp00.lwo","boonflsh.lwo",0,1,CARREV0},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","rollshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"commando.lws","cmndpwup.lwo","cmndshad.lwo","truck.tsp","commando.lwo","commandobody","carclp02.lwo","commflsh.lwo",0,1,CARREV2},
 {NULL,NULL,"enforcer.lws","mnstpwup.lwo","mnstshad.lwo","monster.tsp","monster.lwo","monsterbody","carclp03.lwo","mnstrflsh.lwo",0,1,CARREV3},
 {NULL,NULL,"flamer.lws","flampwup.lwo","flamshad.lwo","flamer.tsp","flamer.lwo","flamerbody","carclp04.lwo","flameflash.lwo",0,1,CARREV4},
 {NULL,NULL,"stingrod.lws","stngpwup.lwo","stngshad.lwo","stingrod.tsp","stingrod.lwo","stingrodbody","carclp05.lwo","stingflsh.lwo",0,1,CARREV5},

// {NULL,NULL,"enforcer.lws","mnstpwup.lwo","mnstshad.lwo","monster.tsp","monster.lwo","monsterbody","carclp03.lwo","mnstrflsh.lwo",0,1,CARREV3},
// {NULL,NULL,"commando.lws","cmndpwup.lwo","cmndshad.lwo","truck.tsp","commando.lwo","commandobody","carclp02.lwo","commflsh.lwo",0,1,CARREV2},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
 {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage2.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
// {NULL,NULL,"boonyn.mxs","boonpwup.bwo","cmndshad.lwo","boonyn.txt","boonyn.bwo","boonynbody","boonpwup.lwo","boonflsh.bwo",0,1,CARREV0},
// {NULL,NULL,"boonyn.lws","boonpwup.lwo","cmndshad.lwo","boonyn.txt","boonyn.lwo","boonynbody","carclp00.lwo","boonflsh.lwo",0,1,CARREV0},
// {NULL,NULL,"rollcage.lws","rollpwup.lwo","cmndshad.lwo","rollcage.txt","rollcage.lwo","rollcagebody","carclp01.lwo","rollflsh.lwo",0,0,CARREV1},
/*// {NULL,NULL,"tread.lws","tredpwup.lwo","tredshad.lwo","treadator.tsp","tread.lwo","treadbody","carclp06.lwo","treadflsh.lwo",0,0,CARREV6},
 {NULL,NULL,"shocker.lws","shckpwup.lwo","shckshad.lwo","shocker.tsp","shocker.lwo","shockerbody","carclp07.lwo","shockflsh.lwo",0,1,CARREV7},
 {NULL,NULL,"tryder.lws","trydpwup.lwo","trydshad.lwo","tryder.tsp","tryder.lwo","tryderbody","carclp08.lwo","trydflsh.lwo",0,1,CARREV8},
 {NULL,NULL,"slideout.lws","slidpwup.lwo","slidshad.lwo","slideout.tsp","slideout.lwo","slideoutbody","carclp09.lwo","sldflsh.lwo",0,0,CARREV9},
 {NULL,NULL,"surfcrat.lws","srfpwup.lwo","srfshad.lwo","surfcrat.tsp","surfcrat.lwo","surfcratbody","carclp10.lwo","srfflash.lwo",0,1,CARREV10},
 {NULL,NULL,"stepper.lws","stppwup.lwo","stpshad.lwo","stepper.tsp","stepper.lwo","stepperbody","carclp11.lwo","stepflsh.lwo",0,1,CARREV11},
// {NULL,NULL,"radar.lws","radrpwup.lwo","radrshad.lwo","radar.tsp","radar.lwo","radarbody","carclp12.lwo","radarflsh.lwo",0,1,CARREV12},
*/
};

struct ol_ci {
	int nwheels;
	char *wheelnames[6];
};
static struct ol_ci ol_carinfo[]={
	{4,"boonwhl1.lwo","boonwhl2.lwo","boonwhl3.lwo","boonwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"comwhl1.lwo","comwhl2.lwo","comwhl3.lwo","comwhl4.lwo"},
	{4,"monwhl1.lwo","monwhl2.lwo","monwhl3.lwo","monwhl4.lwo"},
	{4,"flmrwhl1.lwo","flmrwhl2.lwo","flmrwhl3.lwo","flmrwhl4.lwo"},
	{4,"stngwhl1.lwo","stngwhl2.lwo","stngwhl3.lwo","stngwhl4.lwo"},

	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
//	{4,"rollwhl1.lwo","rollwhl2.lwo","rollwhl3.lwo","rollwhl4.lwo"},
//	{4,"boontirebl.bwo","boontirebr.bwo","boontirefl.bwo","boonwhlfr.bwo"},
//	{4,"boonwhl1.lwo","boonwhl2.lwo","boonwhl3.lwo","boonwhl4.lwo"},
//	{4,"monwhl1.lwo","monwhl2.lwo","monwhl3.lwo","monwhl4.lwo"},
//	{4,"comwhl1.lwo","comwhl2.lwo","comwhl3.lwo","comwhl4.lwo"},
/*
//	{0}, // tread
	{4,"shcwhl1.lwo","shcwhl2.lwo","shcwhl3.lwo","shcwhl4.lwo"},
	{4,"trydwhl1.lwo","trydwhl2.lwo","trydwhl3.lwo","trydwhl4.lwo"},
	{4,"sldwhl1.lwo","sldwhl2.lwo","sldwhl3.lwo","sldwhl4.lwo"},
	{4,"srfwhl1.lwo","srfwhl2.lwo","srfwhl3.lwo","srfwhl4.lwo"},
	{4,"stepwhl1.lwo","stepwhl2.lwo","stepwhl3.lwo","stepwhl4.lwo"},
//	{6,"radwhl1.lwo","radwhl2.lwo","radwhl3.lwo","radwhl4.lwo","radwhl5.lwo","radwhl6.lwo",}, // radar
*/
};

static char *tirenames[4]={
	"fl","fr","bl","br"
};

char *shinywheelnames[]={
	"wheel_frontleft",
	"wheel_frontright",
	"wheel_backleft",
	"wheel_backright",
//	"tirefl",
//	"tirefr",
//	"tirebl",
//	"tirebr",
};

static char *y2009tirenames[6]={
	"frontleft",
	"frontright",
	"backleft",
	"backright",
	"middleleft",
	"middleright",
};

static script* newcarnames;

typedef struct
{
 int atopspeed;
 float aaccel;
 float atraction;
 float yawspeed;
 float pitchspeed;
 float rollspeed;
 int agoodtrack;
} OL_CARRATE;
static OL_CARRATE ol_carratings =
{
//top accel traction yawspeed pitchspeed rollspeed goodtrack
/*  100,7,  100,   7,  7,  7,  2,   //boonyn   0
  105,7,  100,  10, 10, 10,  0,   //rollcage 1
  110,8,  100,   9,  9,  9, -1,   //commando 2
   90,6.5, 92,   8,  8,  8, -1,   //enforcer 3
  120,10,  50,   5,  5,  5,  4,   //flamer   4
   90,4.5, 50,   8,  8,  8, -1,   //stingrod 5

//   90,6.5, 92,   8,  8,  8, -1,   //enforcer 3 */
  110,8,  100,   4,  9,  9, -1,   //commando 2
//  110,8,  100,   9,  9,  9, -1,   //commando 2
/*  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
  110,8,  100,   9,  9,  9, -1,   //commando 2
								  */
/*
//  90,5.5,100,   5,  0,  0, -1,   //tread    6
  115,6.1f, 85, 7.5,7.5,7.5, -1,   //shocker  7
   90,5,   95,   7,  7,  7,  5,   //tryder   8
  100,5.5, 40,   8,  8,  8, -1,   //slideout 9
  120,10,  75, 6.5,6.5,6.5,  3,   //surfcrat 10
  105,10,  35,   6,  6,  6,  1,   //stepper  11
  120,10, 100,  10, 10, 10, -1,   //radar    12
*/
};

#define NSCL (MAXENERGIES+1)
char *energymatnames[NSCL]={"enrg_fusion","enrg_elec","enrg_cyber","enrg_geothrm","enrg_particle","enrg_chem","speed"};
//static char *scrlinestr[]={"enrg_fusion","enrg_elec","enrg_cyber",
//	"enrg_geothrm","enrg_particle","enrg_chem","speed"}; // put these last
char *energynames[]={"fusion","electrical","cyber","geothermal","eparticle","chemical"};

//static char *ol_noyes[]={"no","yes",NULL};
//static char *ol_drivemodestr[]={"coast","brake","accel","revaccel",NULL};

static float missilefireframes[NMISSILES];
static float embfireframes[NMISSILES];
static float laserfireframes[NMISSILES];
static float sonicfireframes[NMISSILES];
static int bannery[3][4];
static int showgamenews;

static struct menuvar gamecfg_menuvar[]={
	{"loadinggoal1",&olracecfg.loadinggoal1,D_INT,1},
//	{"scorelinekind",&olracecfg.scorelinekind,D_INT,1},
	{"tweakolwheels",&olracecfg.tweakolwheels,D_FLOAT,FLOATUP},
	{"tweakolwheelsu",&olracecfg.tweakolwheelsu,D_FLOAT,FLOATUP},
	{"tweakolwheelsv",&olracecfg.tweakolwheelsv,D_FLOAT,FLOATUP},
	{"beforeloadtimeoutclient",&olracecfg.beforeloadtimeoutclient,D_INT,1},
	{"beforeloadtimeoutserver",&olracecfg.beforeloadtimeoutserver,D_INT,1},
	{"carextracttimeout",&olracecfg.carextracttimeout,D_INT,1},
	{"afterloadtimeout",&olracecfg.afterloadtimeout,D_INT,1},
	{"gameplaytimeout",&olracecfg.gameplaytimeout,D_INT,1},
//	{"useudp",&olracecfg.useudp,D_INT,1},
	{"udpserversize",&olracecfg.udpserversize,D_INT,1},
	{"udpclientsize",&olracecfg.udpclientsize,D_INT,1},
	{"disabletcp",&olracecfg.tcpdisable,D_INT,1},
	{"enableudp",&olracecfg.udpenable,D_INT,1},
	{"crashresety",&olracecfg.crashresety,D_FLOAT,FLOATUP},
	{"forcebotmode",&olracecfg.forcebotmode,D_INT,1},
//	{"testkicknum",&olracecfg.testkicknum,D_INT,1},
//	{"stubtracknum",&olracecfg.stubtracknum,D_INT,1},
	{"rules",&olracecfg.rules,D_INT,1},
//	{"testint1",&od.testint1,D_INT,1},
//	{"testint2",&od.testint2,D_INT,1},
	{"extractsleep",&olracecfg.extractsleep,D_INT,256},
	{"",NULL,D_VOID,0},

/*	{"-------TESTIMPVAL--",NULL,D_VOID,0},
	{"testimpval.x",&testimpval.x,D_FLOAT,FLOATUP/64},
	{"testimpval.y",&testimpval.y,D_FLOAT,FLOATUP/64},
	{"testimpval.z",&testimpval.z,D_FLOAT,FLOATUP/64},
	{"testimppnt.x",&testimppnt.x,D_FLOAT,FLOATUP/64},
	{"testimppnt.y",&testimppnt.y,D_FLOAT,FLOATUP/64},
	{"testimppnt.z",&testimppnt.z,D_FLOAT,FLOATUP/64},
	{"",NULL,D_VOID,0},
*/
	{"-------DIRECTION FINDER--",NULL,D_VOID,0},
	{"gothiswayscalex",&olracecfg.gothiswayscale.x,D_FLOAT,FLOATUP/64},
	{"gothiswayscaley",&olracecfg.gothiswayscale.y,D_FLOAT,FLOATUP/64},
	{"gothiswayscalez",&olracecfg.gothiswayscale.z,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.x",&olracecfg.gothiswaypos.x,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.y",&olracecfg.gothiswaypos.y,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.z",&olracecfg.gothiswaypos.z,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.x",&olracecfg.gothiswayrot.x,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.y",&olracecfg.gothiswayrot.y,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.z",&olracecfg.gothiswayrot.z,D_FLOAT,FLOATUP/64},
//	{"noweapsum",&olracecfg.noweapsum,D_INT,1},
//	{"bluered3d",&olracecfg.bluered3d,D_FLOAT,FLOATUP/64},
	{"",NULL,D_VOID,0},
};
#define GAMECFG_NMENUVAR (sizeof(gamecfg_menuvar)/sizeof(gamecfg_menuvar[0]))


static struct menuvar ol_debvars[]={
// boxai 2009
/*	{"emeter0x",&scrline::energymeterlocs[0].x,D_FLOAT,FLOATUP},
	{"emeter0y",&scrline::energymeterlocs[0].y,D_FLOAT,FLOATUP},
	{"emeter1x",&scrline::energymeterlocs[1].x,D_FLOAT,FLOATUP},
	{"emeter1y",&scrline::energymeterlocs[1].y,D_FLOAT,FLOATUP},
	{"emeter2x",&scrline::energymeterlocs[2].x,D_FLOAT,FLOATUP},
	{"emeter2y",&scrline::energymeterlocs[2].y,D_FLOAT,FLOATUP},
	{"emeter3x",&scrline::energymeterlocs[3].x,D_FLOAT,FLOATUP},
	{"emeter3y",&scrline::energymeterlocs[3].y,D_FLOAT,FLOATUP},
	{"emeter4x",&scrline::energymeterlocs[4].x,D_FLOAT,FLOATUP},
	{"emeter4y",&scrline::energymeterlocs[4].y,D_FLOAT,FLOATUP},
	{"emeter5x",&scrline::energymeterlocs[5].x,D_FLOAT,FLOATUP},
	{"emeter5y",&scrline::energymeterlocs[5].y,D_FLOAT,FLOATUP}, */
//	{"emetertest",&emetertest,D_FLOAT,FLOATUP/64},
	{"track_topx",&track_topx,D_FLOAT,FLOATUP/64},
	{"track_topy",&track_topy,D_FLOAT,FLOATUP/64},
	{"track_topsclx",&track_topsclx,D_FLOAT,FLOATUP/64},
	{"track_topscly",&track_topscly,D_FLOAT,FLOATUP/64},
	{"track_topsoffx",&track_topoffx,D_FLOAT,FLOATUP/64},
	{"track_topoffy",&track_topoffy,D_FLOAT,FLOATUP/64},
	{"track_toprot",&track_toprot,D_FLOAT,FLOATUP/64},
	{"speedtest",&speedtest,D_FLOAT,FLOATUP/64},
	{"speedcx",&speedcx,D_FLOAT,FLOATUP/64},
	{"speedcy",&speedcy,D_FLOAT,FLOATUP/64},
	{"speedpicx",&speedpicx,D_FLOAT,FLOATUP/64},
	{"speedpicy",&speedpicy,D_FLOAT,FLOATUP/64},
	{"speedstartang",&speedstartang,D_FLOAT,FLOATUP/64},
	{"speedendang",&speedendang,D_FLOAT,FLOATUP/64},
	{"etest",&etest,D_FLOAT,FLOATUP/64},
	{"pcx",&pcx,D_FLOAT,FLOATUP*4},
	{"pcy",&pcy,D_FLOAT,FLOATUP*4},
	{"picx0",&picx[0],D_FLOAT,FLOATUP*4},
	{"picy0",&picy[0],D_FLOAT,FLOATUP*4},
	{"picx1",&picx[1],D_FLOAT,FLOATUP*4},
	{"picy1",&picy[1],D_FLOAT,FLOATUP*4},
	{"picx2",&picx[2],D_FLOAT,FLOATUP*4},
	{"picy2",&picy[2],D_FLOAT,FLOATUP*4},
	{"picx3",&picx[3],D_FLOAT,FLOATUP*4},
	{"picy3",&picy[3],D_FLOAT,FLOATUP*4},
	{"picx4",&picx[4],D_FLOAT,FLOATUP*4},
	{"picy4",&picy[4],D_FLOAT,FLOATUP*4},
	{"picx5",&picx[5],D_FLOAT,FLOATUP*4},
	{"picy5",&picy[5],D_FLOAT,FLOATUP*4},
	{"sizemul",&sizemul,D_FLOAT,FLOATUP},
	{"p0x",&trip[0].x,D_FLOAT,FLOATUP*4},
	{"p0y",&trip[0].y,D_FLOAT,FLOATUP*4},
	{"p1x",&trip[1].x,D_FLOAT,FLOATUP*4},
	{"p1y",&trip[1].y,D_FLOAT,FLOATUP*4},
	{"p2x",&trip[2].x,D_FLOAT,FLOATUP*4},
	{"p2y",&trip[2].y,D_FLOAT,FLOATUP*4},
	{"uv0u",&triuv[0].u,D_FLOAT,FLOATUP/64},
	{"uv0v",&triuv[0].v,D_FLOAT,FLOATUP/64},
	{"uv1u",&triuv[1].u,D_FLOAT,FLOATUP/64},
	{"uv1v",&triuv[1].v,D_FLOAT,FLOATUP/64},
	{"uv2u",&triuv[2].u,D_FLOAT,FLOATUP/64},
	{"uv2v",&triuv[2].v,D_FLOAT,FLOATUP/64},
	{"hudloc5x",&hudlocs[5].x,D_FLOAT,FLOATUP*4},
	{"hudloc5y",&hudlocs[5].y,D_FLOAT,FLOATUP*4},
	{"hudloc6x",&hudlocs[6].x,D_FLOAT,FLOATUP*4},
	{"hudloc6y",&hudlocs[6].y,D_FLOAT,FLOATUP*4},
	{"hudloc7x",&hudlocs[7].x,D_FLOAT,FLOATUP*4},
	{"hudloc7y",&hudlocs[7].y,D_FLOAT,FLOATUP*4},
	{"hudloc8x",&hudlocs[8].x,D_FLOAT,FLOATUP*4},
	{"hudloc8y",&hudlocs[8].y,D_FLOAT,FLOATUP*4},
	{"2009_carscale",&od.y2009carscale,D_FLOAT,FLOATUP/64},
	{"showgamenews",&showgamenews,D_INT,1},
	{"coincoldist",&od.coincoldist,D_FLOAT,FLOATUP/64},
	{"dissolvestart",&treeinfo.dissolvestart,D_FLOAT,FLOATUP/64},
	{"defaultdissolvecutoff",&treeinfo.defaultdissolvecutoff,D_FLOAT,FLOATUP/64},
	{"2009_cartrans1.x",&od.y2009cartrans1.x,D_FLOAT,FLOATUP/64},
	{"2009_cartrans1.y",&od.y2009cartrans1.y,D_FLOAT,FLOATUP/64},
	{"2009_cartrans1.z",&od.y2009cartrans1.z,D_FLOAT,FLOATUP/64},
	{"2009_cartrans2.x",&od.y2009cartrans2.x,D_FLOAT,FLOATUP/64},
	{"2009_cartrans2.y",&od.y2009cartrans2.y,D_FLOAT,FLOATUP/64},
	{"2009_cartrans2.z",&od.y2009cartrans2.z,D_FLOAT,FLOATUP/64},
	{"2009_carrot.x",&od.y2009carrot.x,D_FLOAT,FLOATUP/64},
	{"2009_carrot.y",&od.y2009carrot.y,D_FLOAT,FLOATUP/64},
	{"2009_carrot.z",&od.y2009carrot.z,D_FLOAT,FLOATUP/64},
	{"od.aiturnratio",&od.aiturnratio,D_FLOAT,FLOATUP/64},
	{"od.aiturndeadzone",&od.aiturndeadzone,D_FLOAT,FLOATUP/64},
	{"od.boxaimaxwatchdog",&od.boxaimaxwatchdog,D_INT,1},
// custom fog
	{"----- FOG --------------",NULL,D_VOID,0},
	{"fog_start1",&fog1.start,D_FLOAT,FLOATUP/4},
	{"fog_end1",&fog1.end,D_FLOAT,FLOATUP/4},
	{"fog_density1",&fog1.density,D_FLOAT,FLOATUP/4},
	{"fog_r1",&fog1.color.x,D_FLOAT,1},
	{"fog_g1",&fog1.color.y,D_FLOAT,1},
	{"fog_b1",&fog1.color.z,D_FLOAT,1},
	{"fog_a1",&fog1.color.w,D_FLOAT,1},
	{"fog_method1",&fog1.mode,D_INT,1},
	{"fog_enable1",&fog1.enable,D_INT,1},
	{"----- FOG2 --------------",NULL,D_VOID,0},
	{"fog_start2",&fog2.start,D_FLOAT,FLOATUP/4},
	{"fog_end2",&fog2.end,D_FLOAT,FLOATUP/4},
	{"fog_density2",&fog2.density,D_FLOAT,FLOATUP/4},
	{"fog_r2",&fog2.color.x,D_FLOAT,1},
	{"fog_g2",&fog2.color.y,D_FLOAT,1},
	{"fog_b2",&fog2.color.z,D_FLOAT,1},
	{"fog_a2",&fog2.color.w,D_FLOAT,1},
	{"fog_method2",&fog2.mode,D_INT,1},
	{"fog_enable2",&fog2.enable,D_INT,1},

/*	{"-------DIRECTION FINDER--",NULL,D_VOID,0},
	{"gothiswayscalex",&olracecfg.gothiswayscale.x,D_FLOAT,FLOATUP/64},
	{"gothiswayscaley",&olracecfg.gothiswayscale.y,D_FLOAT,FLOATUP/64},
	{"gothiswayscalez",&olracecfg.gothiswayscale.z,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.x",&olracecfg.gothiswaypos.x,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.y",&olracecfg.gothiswaypos.y,D_FLOAT,FLOATUP/64},
	{"gothiswaypos.z",&olracecfg.gothiswaypos.z,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.x",&olracecfg.gothiswayrot.x,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.y",&olracecfg.gothiswayrot.y,D_FLOAT,FLOATUP/64},
	{"gothiswayrot.z",&olracecfg.gothiswayrot.z,D_FLOAT,FLOATUP/64},
	{"-------CARENA-------",NULL,D_VOID,0},
//	{"testheight",&testheight,D_FLOAT,FLOATUP/16},
	{"loadinggoal1",&olracecfg.loadinggoal1,D_INT,1},
//	{"scorelinekind",&olracecfg.scorelinekind,D_INT,1},
	{"tweakolwheels",&olracecfg.tweakolwheels,D_FLOAT,FLOATUP},
	{"tweakolwheelsu",&olracecfg.tweakolwheelsu,D_FLOAT,FLOATUP},
	{"tweakolwheelsv",&olracecfg.tweakolwheelsv,D_FLOAT,FLOATUP},
	{"beforeloadtimeoutclient",&olracecfg.beforeloadtimeoutclient,D_INT,1},
	{"beforeloadtimeoutserver",&olracecfg.beforeloadtimeoutserver,D_INT,1},
	{"carextracttimeout",&olracecfg.carextracttimeout,D_INT,1},
	{"afterloadtimeout",&olracecfg.afterloadtimeout,D_INT,1},
	{"gameplaytimeout",&olracecfg.gameplaytimeout,D_INT,1},
//	{"useudp",&olracecfg.useudp,D_INT,1},
	{"udpserversize",&olracecfg.udpserversize,D_INT,1},
	{"udpclientsize",&olracecfg.udpclientsize,D_INT,1},
	{"disabletcp",&olracecfg.tcpdisable,D_INT,1},
	{"enableudp",&olracecfg.udpenable,D_INT,1},
	{"crashresety",&olracecfg.crashresety,D_FLOAT,FLOATUP},
	{"forcebotmode",&olracecfg.forcebotmode,D_INT,1},
//	{"testkicknum",&olracecfg.testkicknum,D_INT,1},
	{"stubtracknum",&olracecfg.stubtracknum,D_INT,1},
	{"rules",&olracecfg.rules,D_INT,1},
	{"testint1",&od.testint1,D_INT,1},
	{"testint2",&od.testint2,D_INT,1},
	{"extractsleep",&olracecfg.extractsleep,D_INT,256},
	{"",NULL,D_VOID,0},
*/
// carena camera work
// enable
	{"newcamvars.usenewcam",&newcamvars.usenewcam,D_INT,1},
//	{"newcamvars.usesteeryaw",&newcamvars.usesteeryaw,D_ENUM,1,noyes},
//	{"newcamvars.usesteeryaw2",&newcamvars.usesteeryaw2,D_ENUM,1,noyes},
	{"newcamvars.usecoll",&newcamvars.usecoll,D_INT,1},
	{"newcamvars.colltries",&newcamvars.colltries,D_INT,1},
// settings
	{"newcamvars.steeryaw",&newcamvars.steeryaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.cpitch",&newcamvars.cpitch,D_FLOAT,FLOATUP/16},
	{"newcamvars.cyaw",&newcamvars.cyaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.pitchdrift",&newcamvars.pitchdrift,D_FLOAT,FLOATUP/16},
	{"newcamvars.yawdrift",&newcamvars.yawdrift,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryaw",&newcamvars.steeryaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.normalpitch",&newcamvars.normalpitch,D_FLOAT,FLOATUP/16},
//	{"newcamvars.normalyaw",&newcamvars.normalyaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.collyawrate",&newcamvars.collyawrate,D_FLOAT,FLOATUP/16},
	{"newcamvars.collpitchrate",&newcamvars.collpitchrate,D_FLOAT,FLOATUP/16},
	{"newcamvars.yawsafe",&newcamvars.yawsafe,D_FLOAT,FLOATUP/16},
	{"newcamvars.pitchsafe",&newcamvars.pitchsafe,D_FLOAT,FLOATUP/16},
	{"newcamvars.yawlimit",&newcamvars.yawlimit,D_FLOAT,FLOATUP/16},
	{"newcamvars.pitchlimit",&newcamvars.pitchlimit,D_FLOAT,FLOATUP/16},
	{"newcamvars.normalcentery",&newcamvars.normalcentery,D_FLOAT,FLOATUP/16},
	{"newcamvars.startcentery",&newcamvars.startcentery,D_FLOAT,FLOATUP/16},
	{"newcamvars.centerratey",&newcamvars.centerratey,D_FLOAT,FLOATUP/16},
	{"newcamvars.cheadrate",&newcamvars.cheadrate,D_FLOAT,FLOATUP/16},
	{"newcamvars.centery",&newcamvars.centery,D_FLOAT,FLOATUP/16},
	{"newcamvars.centerx",&newcamvars.centerx,D_FLOAT,FLOATUP/16},
	{"newcamvars.ccenterx",&newcamvars.ccenterx,D_FLOAT,FLOATUP/16},
	{"newcamvars.centerxdrift",&newcamvars.centerxdrift,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryawlimit",&newcamvars.steeryawlimit,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryawrate",&newcamvars.steeryawrate,D_FLOAT,FLOATUP/16},
//	{"newcamvars.steeryawrate2",&newcamvars.steeryawrate2,D_FLOAT,FLOATUP/16},
//	{"newcamvars.leftcount",&newcamvars.leftcount,D_INT,1},
//	{"newcamvars.rightcount",&newcamvars.rightcount,D_INT,1},
// less raw
	{"newcamvars.pitch",&newcamvars.pitch,D_FLOAT,FLOATUP/16},
	{"newcamvars.yaw",&newcamvars.yaw,D_FLOAT,FLOATUP/16},
	{"newcamvars.dist",&newcamvars.dist,D_FLOAT,FLOATUP/16},
//	{"newcamvars.centery",&newcamvars.centery,D_FLOAT,FLOATUP/16},
// raw
	{"newcamvars.zoom",&newcamvars.zoom,D_FLOAT,FLOATUP/16},
	{"newcamvars.pos.x",&newcamvars.pos.x,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.pos.y",&newcamvars.pos.y,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.pos.z",&newcamvars.pos.z,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.x",&newcamvars.rot.x,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.y",&newcamvars.rot.y,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.z",&newcamvars.rot.z,D_FLOAT|D_RDONLY,FLOATUP/16},
	{"newcamvars.rot.w",&newcamvars.rot.w,D_FLOAT|D_RDONLY,FLOATUP/16},
// jrm cars and weap scale
	{"---- jrm car (local) -----------------",NULL,D_VOID,0},
	{"jrmcarrot.x",&jrmcarrot.x,D_FLOAT,FLOATUP/4},
	{"jrmcarrot.y",&jrmcarrot.y,D_FLOAT,FLOATUP/4},
	{"jrmcarrot.z",&jrmcarrot.z,D_FLOAT,FLOATUP/4},
	{"jrmcartrans.x",&jrmcartrans.x,D_FLOAT,FLOATUP/4},
	{"jrmcartrans.y",&jrmcartrans.y,D_FLOAT,FLOATUP/4},
	{"jrmcartrans.z",&jrmcartrans.z,D_FLOAT,FLOATUP/4},
	{"jrmcarscale",&jrmcarscale,D_FLOAT,FLOATUP/4},
	{"weapscale",&od.weapscale,D_FLOAT,FLOATUP/4},

	{"--- extra deb vars from online_uplay.c ---",NULL,D_VOID,0},
	{"testflasher",&od.testflasher,D_FLOAT,FLOATUP/64},
	{"flashercolor.r",&od.flashercolor.x,D_FLOAT,FLOATUP/64},
	{"flashercolor.g",&od.flashercolor.y,D_FLOAT,FLOATUP/64},
	{"flashercolor.b",&od.flashercolor.z,D_FLOAT,FLOATUP/64},
	{"bannery_2_1",&bannery[0][0],D_INT,1},
	{"bannery_2_2",&bannery[0][1],D_INT,1},
	{"bannery_3_1",&bannery[1][0],D_INT,1},
	{"bannery_3_2",&bannery[1][1],D_INT,1},
	{"bannery_3_3",&bannery[1][2],D_INT,1},
	{"bannery_4_1",&bannery[2][0],D_INT,1},
	{"bannery_4_2",&bannery[2][1],D_INT,1},
	{"bannery_4_3",&bannery[2][2],D_INT,1},
	{"bannery_4_4",&bannery[2][3],D_INT,1},
	{"-------- SHOW PATH ----------",NULL,D_VOID,0},
	{"selpath",&od.selpath,D_INT,1},
	{"showpathframe",&od.showpathframe,D_FLOAT,FLOATUP},
	{"showrabbits",&od.showrabbits,D_INT,1},
	{"path2road",&od.path2road,D_INT,1},
	{"showcrashresetloc",&od.showcrashresetloc,D_INT,1},
	{"-------- CRASH RESET EDITOR ----------",NULL,D_VOID,0},
	{"editenable",&crashinfos.editenable,D_INT,1},
	{"num",&crashinfos.num,D_INT|D_RDONLY,1},
	{"cur",&crashinfos.cur,D_INT,1},
	{"checkidx",&crashinfos.curedit.checkidx,D_INT,1},
	{"locx",&crashinfos.curedit.loc.x,D_FLOAT,FLOATUP/16},
	{"locy",&crashinfos.curedit.loc.y,D_FLOAT,FLOATUP/16},
	{"locz",&crashinfos.curedit.loc.z,D_FLOAT,FLOATUP/16},
	{"rotx",&crashinfos.curedit.rot.x,D_FLOAT,FLOATUP/16},
	{"roty",&crashinfos.curedit.rot.y,D_FLOAT,FLOATUP/16},
	{"rotz",&crashinfos.curedit.rot.z,D_FLOAT,FLOATUP/16},
	{"rotw",&crashinfos.curedit.rot.w,D_FLOAT,FLOATUP/16},
	{"doinsert",&crashinfos.doinsert,D_INT,1},
	{"dodelete",&crashinfos.dodelete,D_INT,1},
	{"doload",&crashinfos.doload,D_INT,1},
	{"dosave",&crashinfos.dosave,D_INT,1},
	{"-------- CHECKPOINT EDITOR ----------",NULL,D_VOID,0},
	{"editenable",&checkinfos.editenable,D_INT,1},
	{"num",&checkinfos.num,D_INT|D_RDONLY,1},
	{"cur",&checkinfos.cur,D_INT,1},
	{"locx",&checkinfos.curedit.loc.x,D_FLOAT,FLOATUP/16},
	{"locy",&checkinfos.curedit.loc.y,D_FLOAT,FLOATUP/16},
	{"locz",&checkinfos.curedit.loc.z,D_FLOAT,FLOATUP/16},
	{"rotx",&checkinfos.curedit.rot.x,D_FLOAT,FLOATUP/16},
	{"roty",&checkinfos.curedit.rot.y,D_FLOAT,FLOATUP/16},
	{"rotz",&checkinfos.curedit.rot.z,D_FLOAT,FLOATUP/16},
	{"rotw",&checkinfos.curedit.rot.w,D_FLOAT,FLOATUP/16},
	{"doinsert",&checkinfos.doinsert,D_INT,1},
	{"dodelete",&checkinfos.dodelete,D_INT,1},
	{"doload",&checkinfos.doload,D_INT,1},
	{"dosave",&checkinfos.dosave,D_INT,1},
/*	{"-------- BOT AI EDITOR ----------",NULL,D_VOID,0},
	{"editenable",&botaiinfos.editenable,D_INT,1},
	{"num",&botaiinfos.num,D_INT|D_RDONLY,1},
	{"cur",&botaiinfos.cur,D_INT,1},
	{"pathid",&botaiinfos.pathid,D_INT,1},
	{"locx",&botaiinfos.curedit.loc.x,D_FLOAT,FLOATUP/16},
	{"locy",&botaiinfos.curedit.loc.y,D_FLOAT,FLOATUP/16},
	{"locz",&botaiinfos.curedit.loc.z,D_FLOAT,FLOATUP/16},
	{"doinsert",&botaiinfos.doinsert,D_INT,1},
	{"dodelete",&botaiinfos.dodelete,D_INT,1},
	{"doload",&botaiinfos.doload,D_INT,1},
	{"dosave",&botaiinfos.dosave,D_INT,1}, */
	{"od.bigmove",&od.bigmove,D_INT,1},
//	{"od.newcarscale",&od.newcarscale,D_FLOAT,FLOATUP/64},
//	{"od.newcaroffy",&od.newcaroffy,D_FLOAT,FLOATUP/64},
	{"uplaycarscale",&od.ol_uplaycarscale,D_FLOAT,FLOATUP},
	{"od.quittime",&od.quittime,D_INT,1},
	{"od.defuloop",&od.defuloop,D_INT,1},
	{"od.slowleadmask",&od.slowleadmask,D_INT,1},
	{"od.boxaislowleadmask",&od.boxaislowleadmask,D_INT,1},
	{"od.boxaislowleadpower",&od.boxaislowleadpower,D_INT,1},
	{"od.boxaislowleadmask2",&od.boxaislowleadmask2,D_INT,1},
	{"od.boxaislowleadpower2",&od.boxaislowleadpower2,D_INT,1},
	{"od.boxaislowleadrat",&od.boxaislowleadrat,D_FLOAT,FLOATUP},
	{"od.slowleadpower",&od.slowleadpower,D_INT,1},
	{"od.wayleadpower",&od.wayleadpower,D_INT,1},
	{"od.wayleadframediff",&od.wayleadframediff,D_FLOAT,FLOATUP/64},
	{"od.disablespace",&od.disablespace,D_INT,1},
//	{"od.pingmode",&od.pingmode,D_INT,1},
	{"od.difftimemode",&od.difftimemode,D_INT,1},
	{"od.ol_loop",&od.ol_uloop,D_INT,1},
	{"od.c2celast",&od.c2celast,D_FLOAT,FLOATUP/64},
	{"particleeffects",&od.ol_particleeffects,D_INT,1},
	{"defaultjumpfriction",&ocp.ol_defaultjumpfriction,D_FLOAT,FLOATUP/64},
	{"dirtscale",&od.ol_dirtscale,D_FLOAT,FLOATUP/64},
	{"dirtq",&od.ol_dirtq,D_INT,1},
	{"dirtvbx",&ocp.ol_dirtvbx,D_FLOAT,FLOATUP/64},
	{"dirtvrandx",&ocp.ol_dirtvrandx,D_INT,1},
	{"dirtvby",&ocp.ol_dirtvby,D_FLOAT,FLOATUP/64},
	{"dirtvrandy",&ocp.ol_dirtvrandy,D_INT,1},
	{"dirtvbz",&ocp.ol_dirtvbz,D_FLOAT,FLOATUP/64},
	{"dirtvrandz",&ocp.ol_dirtvrandz,D_INT,1},
	{"sub_dirtscale",&od.ol_sub_dirtscale,D_FLOAT,FLOATUP/64},
	{"sub_dirtq",&od.ol_sub_dirtq,D_INT,1},
	{"sub_dirtvbx",&ocp.ol_sub_dirtvbx,D_FLOAT,FLOATUP/64},
	{"sub_dirtvrandx",&od.ol_sub_dirtvrandx,D_INT,1},
	{"sub_dirtvby",&ocp.ol_sub_dirtvby,D_FLOAT,FLOATUP/64},
	{"sub_dirtvrandy",&od.ol_sub_dirtvrandy,D_INT,1},
	{"sub_dirtvbz",&ocp.ol_sub_dirtvbz,D_FLOAT,FLOATUP/64},
	{"sub_dirtvrandz",&od.ol_sub_dirtvrandz,D_INT,1},

	{"darkdirtscale",&od.ol_darkdirtscale,D_FLOAT,FLOATUP/64},
	{"darkdirtq",&ocp.ol_darkdirtq,D_INT,1},
	{"darkdirtvbx",&ocp.ol_darkdirtvbx,D_FLOAT,FLOATUP/64},
	{"darkdirtvrandx",&ocp.ol_darkdirtvrandx,D_INT,1},
	{"darkdirtvby",&ocp.ol_darkdirtvby,D_FLOAT,FLOATUP/64},
	{"darkdirtvrandy",&ocp.ol_darkdirtvrandy,D_INT,1},
	{"darkdirtvbz",&ocp.ol_darkdirtvbz,D_FLOAT,FLOATUP/64},
	{"darkdirtvrandz",&ocp.ol_darkdirtvrandz,D_INT,1},
	{"sub_darkdirtscale",&od.ol_sub_darkdirtscale,D_FLOAT,FLOATUP/64},
	{"sub_darkdirtq",&ocp.ol_sub_darkdirtq,D_INT,1},
	{"sub_darkdirtvbx",&ocp.ol_sub_darkdirtvbx,D_FLOAT,FLOATUP/64},
	{"sub_darkdirtvrandx",&ocp.ol_sub_darkdirtvrandx,D_INT,1},
	{"sub_darkdirtvby",&ocp.ol_sub_darkdirtvby,D_FLOAT,FLOATUP/64},
	{"sub_darkdirtvrandy",&ocp.ol_sub_darkdirtvrandy,D_INT,1},
	{"sub_darkdirtvbz",&ocp.ol_sub_darkdirtvbz,D_FLOAT,FLOATUP/64},
	{"sub_darkdirtvrandz",&ocp.ol_sub_darkdirtvrandz,D_INT,1},

	{"camdrift",&od.ol_camdrift,D_FLOAT,FLOATUP/4},
	{"camrotdrift",&od.ol_camrotdrift,D_FLOAT,FLOATUP/4},
	{"extracamang",&od.ol_extracamang,D_FLOAT,FLOATUP/16},
	{"camstartx",&od.ol_camstart.x,D_FLOAT,FLOATUP/4},
	{"camstarty",&od.ol_camstart.y,D_FLOAT,FLOATUP/4},
	{"camstartz",&od.ol_camstart.z,D_FLOAT,FLOATUP/4},
	{"camtransz",&od.ol_camtrans.z,D_FLOAT,FLOATUP/16},
	{"thecamzoom",&od.ol_thecamzoom,D_FLOAT,FLOATUP/16},
	{"camtweenspeed",&od.ol_camtweenspeed,D_FLOAT,FLOATUP/64},
	{"pitchthreshup",&od.ol_pitchthreshup,D_FLOAT,FLOATUP*1},
	{"rollthreshup",&od.ol_rollthreshup,D_FLOAT,FLOATUP*1},
	{"pitchthreshdown",&od.ol_pitchthreshdown,D_FLOAT,FLOATUP*1},
	{"rollthreshdown",&od.ol_rollthreshdown,D_FLOAT,FLOATUP*1},
	{"newpitch",&od.ol_newpitch.w,D_FLOAT,FLOATUP},
	{"camcoldist",&od.ol_camcoldist,D_FLOAT,FLOATUP/64},
	{"camlandtime",&od.camlandtime,D_INT,1},
	{"ustarttime",&od.ol_ustarttime,D_INT,1},
	{"clocktickcount",&ocp.clocktickcount,D_INT,1},
	{"finishpercent",&od.ol_finishpercent,D_INT,1},
//	{"notimelimit",&od.ol_notimelimit,D_INT,1},
	{"mass",&ocp.mass,D_FLOAT,FLOATUP/16},
	{"moi",&ocp.moi,D_FLOAT,FLOATUP/16},
	{"littlegground",&ocp.ol_littlegground,D_FLOAT,FLOATUP/16},
	{"littlegair",&ocp.ol_littlegair,D_FLOAT,FLOATUP/16},
	{"airfric",&ocp.ol_airfric,D_FLOAT,FLOATUP/16},
	{"defuextraheading",&ocp.ol_defuextraheading,D_FLOAT,FLOATUP},
	{"maxuextraheading",&ocp.ol_maxuextraheading,D_FLOAT,FLOATUP},
	{"steervel",&ocp.ol_steervel,D_FLOAT,FLOATUP/16},
	{"steervelback",&ocp.ol_steervelback,D_FLOAT,FLOATUP/16},
	{"wheelaccel",&ocp.ol_wheelaccel,D_FLOAT,FLOATUP/16},
	{"startaccelspin",&ocp.ol_startaccelspin,D_FLOAT,FLOATUP/16},
	{"stunttime",&od.ol_stunttime,D_INT,16384},
//	{"uplayrevvolume",&od.ol_uplayrevvolume,D_INT,1},
//	{"loopcompensater",&loopcompensater,D_FLOAT,FLOATUP},
//	{"coincoldist",&od.ol_coincoldist,D_FLOAT,FLOATUP/16},
	{"shockstr",&ocp.ol_shockstr,D_FLOAT,FLOATUP/16},
	{"shockdamp",&ocp.ol_shockdamp,D_FLOAT,FLOATUP/16},
	{"fricttireroll",&ocp.ol_fricttireroll,D_FLOAT,FLOATUP/16},
	{"fricttireslide",&ocp.ol_fricttireslide,D_FLOAT,FLOATUP/16},
	{"shockspacingx",&ocp.ol_shockspacingx,D_FLOAT,FLOATUP/16},
	{"shockspacingz",&ocp.ol_shockspacingz,D_FLOAT,FLOATUP/16},
	{"shockoffsety",&ocp.ol_shockoffsety,D_FLOAT,FLOATUP/16},
	{"wheelrad",&ocp.ol_wheelrad,D_FLOAT,FLOATUP/16},
	{"wheelwid",&ocp.ol_wheelwid,D_FLOAT,FLOATUP/16},
	{"wheellen",&ocp.ol_wheellen,D_FLOAT,FLOATUP/16},
	{"normalshocklen0",&ocp.ol_normalshocklen[0],D_FLOAT,FLOATUP/64},
	{"normalshocklen1",&ocp.ol_normalshocklen[1],D_FLOAT,FLOATUP/64},
	{"normalshocklen2",&ocp.ol_normalshocklen[2],D_FLOAT,FLOATUP/64},
	{"normalshocklen3",&ocp.ol_normalshocklen[3],D_FLOAT,FLOATUP/64},
	{"normalshocklen4",&ocp.ol_normalshocklen[4],D_FLOAT,FLOATUP/64},
	{"normalshocklen5",&ocp.ol_normalshocklen[5],D_FLOAT,FLOATUP/64},
	{"shocklimit",&ocp.ol_shocklimit,D_FLOAT,FLOATUP/64},
	{"caroffsetx",&ocp.ol_caroffsetx,D_FLOAT,FLOATUP/16},
	{"caroffsety",&ocp.ol_caroffsety,D_FLOAT,FLOATUP/16},
	{"caroffsetz",&ocp.ol_caroffsetz,D_FLOAT,FLOATUP/16},
	{"uplaywheelscale",&ocp.ol_uplaywheelscale,D_FLOAT,FLOATUP},
	{"uplayshadowscale",&ocp.ol_uplayshadowscale,D_FLOAT,FLOATUP},
	{"carleft",&ocp.ol_carbboxmin.x,D_FLOAT,FLOATUP/64},
	{"carbot",&ocp.ol_carbboxmin.y,D_FLOAT,FLOATUP/64},
	{"carback",&ocp.ol_carbboxmin.z,D_FLOAT,FLOATUP/64},
	{"carright",&ocp.ol_carbboxmax.x,D_FLOAT,FLOATUP/64},
	{"cartop",&ocp.ol_carbboxmax.y,D_FLOAT,FLOATUP/64},
	{"carfront",&ocp.ol_carbboxmax.z,D_FLOAT,FLOATUP/64},
	{"frictcarbody",&ocp.ol_frictcarbody,D_FLOAT,FLOATUP/16},
	{"frictcarbodyroof",&ocp.ol_frictcarbodyroof,D_FLOAT,FLOATUP/16},
	{"elast",&ocp.ol_elast,D_FLOAT,FLOATUP/16},
	{"nubursts",&ocp.ol_nubursts,D_INT,1},
	{"nuflashes",&ocp.ol_nuflashes,D_INT,1},
	{"nucondoms",&ocp.ol_nucondoms,D_INT,1},
	{"nuflames",&ocp.ol_nuflames,D_INT,1},
	{"nulightnings",&ocp.ol_nulightnings,D_INT,1},
	{"revfreqmul",&ocp.ol_revfreqmul,D_FLOAT,FLOATUP/4},
	{"revfreqadd",&ocp.ol_revfreqadd,D_FLOAT,FLOATUP/4},
//	{"scrlineposx",&od.ol_scrlinepos.x,D_FLOAT,FLOATUP/64},
//	{"scrlineposy",&od.ol_scrlinepos.y,D_FLOAT,FLOATUP/64},
//	{"scrlineposz",&od.ol_scrlinepos.z,D_FLOAT,FLOATUP/64},
//	{"scrlinesclx",&od.ol_scrlinescl.x,D_FLOAT,FLOATUP/64},
//	{"scrlinescly",&od.ol_scrlinescl.y,D_FLOAT,FLOATUP/64},
//	{"scrlinesclz",&od.ol_scrlinescl.z,D_FLOAT,FLOATUP/64},

// opa[0] tweek physics
	{"@lightcyan@---- opa[0] test tweek physics -------------------",0,D_VOID,0},
	{"opa_defaultjumpfriction",&opa[0].ol_defaultjumpfriction,D_FLOAT,FLOATUP/64},
	{"opa_mass",&opa[0].mass,D_FLOAT,FLOATUP/16},
	{"opa_moi",&opa[0].moi,D_FLOAT,FLOATUP/16},
	{"opa_littlegground",&opa[0].ol_littlegground,D_FLOAT,FLOATUP/16},
	{"opa_littlegair",&opa[0].ol_littlegair,D_FLOAT,FLOATUP/16},
	{"opa_airfric",&opa[0].ol_airfric,D_FLOAT,FLOATUP/16},
	{"opa_defuextraheading",&opa[0].ol_defuextraheading,D_FLOAT,FLOATUP},
	{"opa_maxuextraheading",&opa[0].ol_maxuextraheading,D_FLOAT,FLOATUP},
	{"opa_steervel",&opa[0].ol_steervel,D_FLOAT,FLOATUP/16},
	{"opa_steervelback",&opa[0].ol_steervelback,D_FLOAT,FLOATUP/16},
	{"opa_wheelaccel",&opa[0].ol_wheelaccel,D_FLOAT,FLOATUP/16},
	{"opa_startaccelspin",&opa[0].ol_startaccelspin,D_FLOAT,FLOATUP/16},
	{"opa_shockstr",&opa[0].ol_shockstr,D_FLOAT,FLOATUP/16},
	{"opa_shockdamp",&opa[0].ol_shockdamp,D_FLOAT,FLOATUP/16},
	{"opa_fricttireroll",&opa[0].ol_fricttireroll,D_FLOAT,FLOATUP/16},
	{"opa_fricttireslide",&opa[0].ol_fricttireslide,D_FLOAT,FLOATUP/16},
	{"opa_shockspacingx",&opa[0].ol_shockspacingx,D_FLOAT,FLOATUP/16},
	{"opa_shockspacingz",&opa[0].ol_shockspacingz,D_FLOAT,FLOATUP/16},
	{"opa_shockoffsety",&opa[0].ol_shockoffsety,D_FLOAT,FLOATUP/16},
	{"opa_wheelrad",&opa[0].ol_wheelrad,D_FLOAT,FLOATUP/16},
	{"opa_wheelwid",&opa[0].ol_wheelwid,D_FLOAT,FLOATUP/16},
	{"opa_wheellen",&opa[0].ol_wheellen,D_FLOAT,FLOATUP/16},
	{"opa_normalshocklen0",&opa[0].ol_normalshocklen[0],D_FLOAT,FLOATUP/64},
	{"opa_normalshocklen1",&opa[0].ol_normalshocklen[1],D_FLOAT,FLOATUP/64},
	{"opa_normalshocklen2",&opa[0].ol_normalshocklen[2],D_FLOAT,FLOATUP/64},
	{"opa_normalshocklen3",&opa[0].ol_normalshocklen[3],D_FLOAT,FLOATUP/64},
	{"opa_normalshocklen4",&opa[0].ol_normalshocklen[4],D_FLOAT,FLOATUP/64},
	{"opa_normalshocklen5",&opa[0].ol_normalshocklen[5],D_FLOAT,FLOATUP/64},
	{"opa_shocklimit",&opa[0].ol_shocklimit,D_FLOAT,FLOATUP/64},
	{"opa_caroffsetx",&opa[0].ol_caroffsetx,D_FLOAT,FLOATUP/16},
	{"opa_caroffsety",&opa[0].ol_caroffsety,D_FLOAT,FLOATUP/16},
	{"opa_caroffsetz",&opa[0].ol_caroffsetz,D_FLOAT,FLOATUP/16},
	{"opa_uplaywheelscale",&opa[0].ol_uplaywheelscale,D_FLOAT,FLOATUP},
	{"opa_uplayshadowscale",&opa[0].ol_uplayshadowscale,D_FLOAT,FLOATUP},
	{"opa_carleft",&opa[0].ol_carbboxmin.x,D_FLOAT,FLOATUP/64},
	{"opa_carbot",&opa[0].ol_carbboxmin.y,D_FLOAT,FLOATUP/64},
	{"opa_carback",&opa[0].ol_carbboxmin.z,D_FLOAT,FLOATUP/64},
	{"opa_carright",&opa[0].ol_carbboxmax.x,D_FLOAT,FLOATUP/64},
	{"opa_cartop",&opa[0].ol_carbboxmax.y,D_FLOAT,FLOATUP/64},
	{"opa_carfront",&opa[0].ol_carbboxmax.z,D_FLOAT,FLOATUP/64},
	{"opa_frictcarbody",&opa[0].ol_frictcarbody,D_FLOAT,FLOATUP/16},
	{"opa_frictcarbodyroof",&opa[0].ol_frictcarbodyroof,D_FLOAT,FLOATUP/16},
	{"opa_elast",&opa[0].ol_elast,D_FLOAT,FLOATUP/16},
	{"opa_nubursts",&opa[0].ol_nubursts,D_INT,1},
	{"opa_nuflashes",&opa[0].ol_nuflashes,D_INT,1},
	{"opa_nucondoms",&opa[0].ol_nucondoms,D_INT,1},
	{"opa_nuflames",&opa[0].ol_nuflames,D_INT,1},
	{"opa_nulightnings",&opa[0].ol_nulightnings,D_INT,1},
	{"opa_revfreqmul",&opa[0].ol_revfreqmul,D_FLOAT,FLOATUP/4},
	{"opa_revfreqadd",&opa[0].ol_revfreqadd,D_FLOAT,FLOATUP/4},
// end opa[0] tweek physics

	{"@white@hipoint",&od.hipoint,D_FLOAT,FLOATUP},
	{"lowpoint",&od.lowpoint,D_FLOAT,FLOATUP},
//	{"coinscale",&od.coinscale,D_FLOAT,FLOATUP},
//	{"extratrackscale",&od.extratrackscale,D_FLOAT,FLOATUP},
	{"laps",&od.laps,D_INT,1},
	{"speedosound",&od.speedosound,D_FLOAT,1},
	{"maxwatchdog",&od.maxwatchdog,D_INT,1},
//	{"pathhornback",&od.pathhornback,D_FLOAT,1},
//	{"pathaihornback",&od.pathaihornback,D_FLOAT,1},
	{"pathbotresetadd",&od.pathbotresetadd,D_FLOAT,1},
	{"pathframestep",&od.pathframestep,D_FLOAT,1},
	{"pathclosedist",&od.pathclosedist,D_FLOAT,1},
	{"pathmeddist",&od.pathmeddist,D_FLOAT,1},
	{"pathfardist",&od.pathfardist,D_FLOAT,1},
	{"pathfardistplace",&od.pathfardistplace,D_FLOAT,1},
	{"pathvfardist",&od.pathvfardist,D_FLOAT,1},
	{"turnthresh",&od.turnthresh,D_FLOAT,1},

	{"voldist",&od.voldist,D_FLOAT,1},
	{"volcutoff",&od.volcutoff,D_FLOAT,1},

	{"fromtex",&od.fromtex,D_INT,1},
	{"totex",&od.totex,D_INT,1},
	{"curwave",&od.ol_curwave,D_INT,1},
	{"ol_zback",&od.ol_zback,D_FLOAT,FLOATUP},
	{"od.speedup",&od.speedup,D_INT,1},
	{"fountainon",&od.fountainon,D_INT,1},
	//{"od.framerate",&od.framerate,D_INT,1},
	{"-------MISSILES-------",NULL,D_VOID,0},
	{"missilestartfuel",&od.missilestartfuel,D_INT,16},
	{"missilevel",&od.missilevel,D_FLOAT,FLOATUP/64},
	{"missilestartupangle",&od.missilestartupangle,D_FLOAT,FLOATUP/64},
//	{"missileseekangle",&od.missileseekangle,D_FLOAT,FLOATUP/64},
	{"missileturnangle",&od.missileturnangle,D_FLOAT,FLOATUP/64},
	{"missileimpactforce",&od.missileimpactforce,D_FLOAT,FLOATUP/64},
	{"missileupforce",&od.missileupforce,D_FLOAT,FLOATUP/64},
	{"missilefireframes0",&missilefireframes[0],D_FLOAT,FLOATUP/64},
	{"missilefireframes1",&missilefireframes[1],D_FLOAT,FLOATUP/64},
	{"missilefireframes2",&missilefireframes[2],D_FLOAT,FLOATUP/64},
	{"missilefireframes3",&missilefireframes[3],D_FLOAT,FLOATUP/64},
	{"mtrailscale",&od.mtrailscale,D_FLOAT,FLOATUP/64},
	{"-------EMB-------",NULL,D_VOID,0},
	{"embstartfuel",&od.embstartfuel,D_INT,16},
	{"embvel",&od.embvel,D_FLOAT,FLOATUP/64},
	{"embstartupangle",&od.embstartupangle,D_FLOAT,FLOATUP/64},
//	{"embseekangle",&od.embseekangle,D_FLOAT,FLOATUP/64},
	{"embturnangle",&od.embturnangle,D_FLOAT,FLOATUP/64},
	{"embimpactforce",&od.embimpactforce,D_FLOAT,FLOATUP/64},
	{"embfireframes0",&embfireframes[0],D_FLOAT,FLOATUP/64},
	{"embfireframes1",&embfireframes[1],D_FLOAT,FLOATUP/64},
	{"embfireframes2",&embfireframes[2],D_FLOAT,FLOATUP/64},
	{"embfireframes3",&embfireframes[3],D_FLOAT,FLOATUP/64},
	{"embrad",&od.embrad,D_FLOAT,FLOATUP/64},
	{"-------PLASMA-------",NULL,D_VOID,0},
	{"plasmatime",&od.plasmatime,D_FLOAT,FLOATUP/64},
	{"-------RPD-------",NULL,D_VOID,0},
	{"rpdstealframe",&od.rpdstealframe,D_FLOAT,FLOATUP/64},
	{"rpddistance",&od.rpddistance,D_FLOAT,FLOATUP},
	{"-------LASERS-------",NULL,D_VOID,0},
	{"laserstartfuel",&od.laserstartfuel,D_INT,16},
	{"laservel",&od.laservel,D_FLOAT,FLOATUP/64},
	{"laserstartupangle",&od.laserstartupangle,D_FLOAT,FLOATUP/64},
//	{"laserseekangle",&od.laserseekangle,D_FLOAT,FLOATUP/64},
//	{"laserturnangle",&od.laserturnangle,D_FLOAT,FLOATUP/64},
	{"laserimpactforce",&od.laserimpactforce,D_FLOAT,FLOATUP/64},
	{"laserupforce",&od.laserupforce,D_FLOAT,FLOATUP/64},
	{"laserextrabounce",&od.laserextrabounce,D_FLOAT,FLOATUP/64},
	{"laseroffsetx",&od.laseroffset.x,D_FLOAT,FLOATUP/64},
	{"laseroffsety",&od.laseroffset.y,D_FLOAT,FLOATUP/64},
	{"laseroffsetz",&od.laseroffset.z,D_FLOAT,FLOATUP/64},
	{"laserfireframes0",&laserfireframes[0],D_FLOAT,FLOATUP/64},
	{"laserfireframes1",&laserfireframes[1],D_FLOAT,FLOATUP/64},
	{"laserfireframes2",&laserfireframes[2],D_FLOAT,FLOATUP/64},
	{"laserfireframes3",&laserfireframes[3],D_FLOAT,FLOATUP/64},

/* 400
laservel 2 # 10 # units / sec
#laserstartupangle 10 # in degrees..
#missileseekangle 30
#missileturnangle 3 # degrees/sec
laserimpactforce .1 # and this also gets multiplied by missilevel
laserx -.1 # left of car
lasery 0 # up
laserz 0 # forward */

	{"-------HANDLING-------",NULL,D_VOID,0},
	{"extrabainitetraction",&od.extrabainitetraction,D_FLOAT,FLOATUP/16}, // less slides
	{"extraaerodynetopspeed",&od.extraaerodynetopspeed,D_FLOAT,FLOATUP/16}, // faster top end
	{"extraramjettopspeed",&od.extraramjettopspeed,D_FLOAT,FLOATUP/16}, // faster top end
	{"extrachargertopspeed",&od.extrachargertopspeed,D_FLOAT,FLOATUP/16}, // faster top end
	{"chargerfastframe",&od.chargerfastframe,D_FLOAT,1}, // wait for faster top end
	{"extranitrotopspeed",&od.extranitrotopspeed,D_FLOAT,FLOATUP/16}, // faster top end
	{"extrachemboosttopspeed",&od.extrachemboosttopspeed,D_FLOAT,FLOATUP/16}, // faster top end
	{"extraintakeaccel",&od.extraintakeaccel,D_FLOAT,FLOATUP/16}, // get to top speed quicker
	{"extrafsnbooaccel",&od.extrafsnbooaccel,D_FLOAT,FLOATUP/16}, // get to top speed quicker
//	{"extrafly",&od.extrafly,D_FLOAT,FLOATUP/16}, // try less gravity in air
	{"speedupspeed",&od.speedupspeed,D_FLOAT,FLOATUP/16},
	{"speedupaccel",&od.speedupaccel,D_FLOAT,FLOATUP/16},
	{"-------REGROT-------",NULL,D_VOID,0},
	{"regrotx",&od.regrot.x,D_FLOAT,FLOATUP*4},
	{"regroty",&od.regrot.y,D_FLOAT,FLOATUP*4},
	{"regrotz",&od.regrot.z,D_FLOAT,FLOATUP*4},
	{"regnum",&od.regnum,D_INT,1},
	{"defaultcntl",&od.defaultcntl,D_INT,1},
	{"-------ICRMS--------",NULL,D_VOID,0},
//	{"enableicrms",&od.enableicrms,D_INT,1},
//	{"icrmsparm",&od.icrmsparm,D_FLOAT,FLOATUP/16},
	{"icrmsduration",&od.icrmsduration,D_INT},
	{"icrmsslowrate",&od.icrmsslowrate,D_FLOAT,FLOATUP/64},
	{"icrmsdistance",&od.icrmsdistance,D_FLOAT,FLOATUP},
	{"icrmsfreezeframe",&od.icrmsfreezeframe,D_FLOAT,FLOATUP},
	{"-------KLONDYKE--------",NULL,D_VOID,0},
	{"klondykeshowbox",&od.klondykeshowbox,D_INT,1},
	{"klondyke0boxmin.x",&od.klondyke0boxmin.x,D_FLOAT,FLOATUP/64},
	{"klondyke0boxmin.y",&od.klondyke0boxmin.y,D_FLOAT,FLOATUP/64},
	{"klondyke0boxmin.z",&od.klondyke0boxmin.z,D_FLOAT,FLOATUP/64},
	{"klondyke0boxmax.x",&od.klondyke0boxmax.x,D_FLOAT,FLOATUP/64},
	{"klondyke0boxmax.y",&od.klondyke0boxmax.y,D_FLOAT,FLOATUP/64},
	{"klondyke0boxmax.z",&od.klondyke0boxmax.z,D_FLOAT,FLOATUP/64},
	{"klondykekickframe",&od.klondykekickframe,D_FLOAT,FLOATUP/4},
	{"klondykekickamount",&od.klondykekickamount,D_FLOAT,FLOATUP/4},
	{"-------OIL--------",NULL,D_VOID,0},
	{"oildist",&od.oildist,D_FLOAT,FLOATUP/16},
	{"oilback",&od.oilback,D_FLOAT,FLOATUP/16},
	{"oildown",&od.oildown,D_FLOAT,FLOATUP/16},
	{"oilspin",&od.oilspin,D_FLOAT,FLOATUP/16},
	{"oilspinspeed",&od.oilspinspeed,D_FLOAT,FLOATUP/16},
	{"oilstartfuel",&od.oilstartfuel,D_INT,16},
	{"oilvel",&od.oilvel,D_FLOAT,FLOATUP/64},
	{"oilymin",&od.oilymin,D_FLOAT,FLOATUP/64},
	{"oilymax",&od.oilymax,D_FLOAT,FLOATUP/64},
	{"oilstopframe",&od.oilstopframe,D_FLOAT,FLOATUP/16},
	{"-------ECTO--------",NULL,D_VOID,0},
	{"ectograv",&od.ectograv,D_FLOAT,FLOATUP/16},
	{"ectorad",&od.ectorad,D_FLOAT,FLOATUP/16},
	{"ectoscale",&od.ectoscale,D_FLOAT,FLOATUP/16},
	{"ectoparm",&od.ectoparm,D_FLOAT,FLOATUP/16},
	{"ectostartfuel",&od.ectostartfuel,D_INT,16},
	{"ectofvelx",&od.ectofvel.x,D_FLOAT,FLOATUP/16},
	{"ectofvely",&od.ectofvel.y,D_FLOAT,FLOATUP/16},
	{"ectofvelz",&od.ectofvel.z,D_FLOAT,FLOATUP/16},
	{"ectocvelx",&od.ectocvel.x,D_FLOAT,FLOATUP/16},
	{"ectocvely",&od.ectocvel.y,D_FLOAT,FLOATUP/16},
	{"ectocvelz",&od.ectocvel.z,D_FLOAT,FLOATUP/16},
	{"ectorvelx",&od.ectorvel.x,D_FLOAT,FLOATUP/16},
	{"ectorvely",&od.ectorvel.y,D_FLOAT,FLOATUP/16},
	{"ectorvelz",&od.ectorvel.z,D_FLOAT,FLOATUP/16},
	{"ectoup",&od.ectoup,D_FLOAT,FLOATUP/16},
	{"ectoframe",&od.ectoframe,D_FLOAT,FLOATUP/16},
	{"-------FERRITE--------",NULL,D_VOID,0},
	{"ferritefric",&od.ferritefric,D_FLOAT,FLOATUP/16},
	{"ferritexzaccel",&od.ferritexzaccel,D_FLOAT,FLOATUP/16},
	{"ferriteyvelthresh",&od.ferriteyvelthresh,D_FLOAT,FLOATUP/16},
	{"ferriteypos",&od.ferriteypos,D_FLOAT,FLOATUP/16},
	{"ferriteyposboost",&od.ferriteyposboost,D_FLOAT,FLOATUP/16},
	{"ferriteyaccel",&od.ferriteyaccel,D_FLOAT,FLOATUP/16},
	{"ferritemaxspeed",&od.ferritemaxspeed,D_FLOAT,FLOATUP/16},

	{"-------MINE--------",NULL,D_VOID,0},
	{"minedist",&od.minedist,D_FLOAT,FLOATUP/16},
	{"minevel",&od.minevel,D_FLOAT,FLOATUP/64},
	{"mineymin",&od.mineymin,D_FLOAT,FLOATUP/64},
	{"mineymax",&od.mineymax,D_FLOAT,FLOATUP/64},
	{"mineupforce",&od.mineupforce,D_FLOAT,FLOATUP/64},
	{"mineback",&od.mineback,D_FLOAT,FLOATUP/16},
	{"minedown",&od.minedown,D_FLOAT,FLOATUP/16},
	{"minearmtime",&od.minearmtime,D_INT,1},
	{"-------CHEMBOOST--------",NULL,D_VOID,0},
	{"chemtrailscale",&od.chemtrailscale,D_FLOAT,FLOATUP/64},
	{"chemtrailback",&od.chemtrailback,D_FLOAT,FLOATUP/64},
	{"chemtraildown",&od.chemtraildown,D_FLOAT,FLOATUP/64},
	{"-------CHEMVAC--------",NULL,D_VOID,0},
	{"chemvacdistance",&od.chemvacdistance,D_FLOAT,FLOATUP},
	{"-------ENLEECH--------",NULL,D_VOID,0},
	{"enleechdistance",&od.enleechdistance,D_FLOAT,FLOATUP},
	{"-------HOLOGRAM--------",NULL,D_VOID,0},
	{"holofront",&od.holofront,D_FLOAT,FLOATUP},
	{"-------PRISM--------",NULL,D_VOID,0},
	{"prismup",&od.prismup,D_FLOAT,FLOATUP},
	{"-------BIGTIRES---------",NULL,D_VOID,0},
	{"bigtirescale",&od.bigtirescale,D_FLOAT,FLOATUP},
	{"bigtireheight",&od.bigtireheight,D_FLOAT,FLOATUP},
	{"bigtireuprate",&od.bigtireuprate,D_FLOAT,FLOATUP},
	{"bigtiredownrate",&od.bigtiredownrate,D_FLOAT,FLOATUP},
	{"underbigtiredown1",&od.underbigtiredown1,D_FLOAT,FLOATUP},
	{"underbigtiredown2",&od.underbigtiredown2,D_FLOAT,FLOATUP},
	{"underbigtireacross",&od.underbigtireacross,D_FLOAT,FLOATUP},
	{"-------POUNCER---------",NULL,D_VOID,0},
	{"pouncerheight",&od.pouncerheight,D_FLOAT,FLOATUP},
	{"pounceruprate",&od.pounceruprate,D_FLOAT,FLOATUP},
//	{"pouncerdownrate",&od.pouncerdownrate,D_FLOAT,FLOATUP},
	{"pouncerframe",&od.pouncerframe,D_FLOAT,FLOATUP},
	{"-------BUZZ--------",NULL,D_VOID,0},
	{"buzzkickframe",&od.buzzkickframe,D_FLOAT,FLOATUP/4},
	{"buzzshowbox",&od.buzzshowbox,D_INT,1},
/*	{"buzz0boxmin.x",&od.buzz0boxmin.x,D_FLOAT,FLOATUP/64},
	{"buzz0boxmin.y",&od.buzz0boxmin.y,D_FLOAT,FLOATUP/64},
	{"buzz0boxmin.z",&od.buzz0boxmin.z,D_FLOAT,FLOATUP/64},
	{"buzz0boxmax.x",&od.buzz0boxmax.x,D_FLOAT,FLOATUP/64},
	{"buzz0boxmax.y",&od.buzz0boxmax.y,D_FLOAT,FLOATUP/64},
	{"buzz0boxmax.z",&od.buzz0boxmax.z,D_FLOAT,FLOATUP/64}, */
//	{"buzzleft",&od.buzzleft,D_FLOAT,FLOATUP/64},
	{"buzzup",&od.buzzup,D_FLOAT,FLOATUP/64},
	{"buzzsizex",&od.buzzsizex,D_FLOAT,FLOATUP/64},
	{"buzzsizez",&od.buzzsizez,D_FLOAT,FLOATUP/64},
	{"buzzsizey",&od.buzzsizey,D_FLOAT,FLOATUP/64},

//	{"buzzkickframe",&od.buzzkickframe,D_FLOAT,FLOATUP/4},
	{"buzzkickamount",&od.buzzkickamount,D_FLOAT,FLOATUP/4},
	{"buzzspincar",&od.buzzspincar,D_FLOAT,FLOATUP/64},
	{"-------FUSION---------",NULL,D_VOID,0},
	{"fusionstartfuel",&od.fusionstartfuel,D_INT,16},
	{"fusionoffsetx",&od.fusionoffset.x,D_FLOAT,FLOATUP/64},
	{"fusionoffsety",&od.fusionoffset.y,D_FLOAT,FLOATUP/64},
	{"fusionoffsetz",&od.fusionoffset.z,D_FLOAT,FLOATUP/64},
	{"fusionvel",&od.fusionvel,D_FLOAT,FLOATUP/64},
	{"fusionscalevel",&od.fusionscalevel,D_FLOAT,FLOATUP/64},
	{"fusionrotvelx",&od.fusionrotvel.x,D_FLOAT,FLOATUP/64},
	{"fusionrotvely",&od.fusionrotvel.y,D_FLOAT,FLOATUP/64},
	{"fusionrotvelz",&od.fusionrotvel.z,D_FLOAT,FLOATUP/64},
	{"fusionstartupangle",&od.fusionstartupangle,D_FLOAT,FLOATUP/64},
	{"fusionimpactforce",&od.fusionimpactforce,D_FLOAT,FLOATUP/64},
	{"fusionupforce",&od.fusionupforce,D_FLOAT,FLOATUP/64},
	{"fusionrad",&od.fusionrad,D_FLOAT,FLOATUP/64},

	{"fusionminradscan",&od.fusionminradscan,D_FLOAT,FLOATUP/64},
	{"fusionmaxradscan",&od.fusionmaxradscan,D_FLOAT,FLOATUP/64},
	{"fusiongrabrad",&od.fusiongrabrad,D_FLOAT,FLOATUP/64},
	{"fusionmaxscale",&od.fusionmaxscale,D_FLOAT,FLOATUP/64},
	{"fusionmorphmax",&od.fusionmorphmax,D_FLOAT,FLOATUP/64},
	{"fusionmorphmin",&od.fusionmorphmin,D_FLOAT,FLOATUP/64},
	{"fusionmorphframe",&od.fusionmorphframe,D_INT,16},
	{"fusiongrav",&od.fusiongrav,D_FLOAT,FLOATUP/64},

	{"-------SONIC---------",NULL,D_VOID,0},
	{"sonicstartfuel",&od.sonicstartfuel,D_INT,16},
	{"sonicoffsetx",&od.sonicoffset.x,D_FLOAT,FLOATUP/64},
	{"sonicoffsety",&od.sonicoffset.y,D_FLOAT,FLOATUP/64},
	{"sonicoffsetz",&od.sonicoffset.z,D_FLOAT,FLOATUP/64},
	{"sonicvel",&od.sonicvel,D_FLOAT,FLOATUP/64},
	{"sonicscalevel",&od.sonicscalevel,D_FLOAT,FLOATUP/64},
	{"sonicrotvelx",&od.sonicrotvel.x,D_FLOAT,FLOATUP/64},
	{"sonicrotvely",&od.sonicrotvel.y,D_FLOAT,FLOATUP/64},
	{"sonicrotvelz",&od.sonicrotvel.z,D_FLOAT,FLOATUP/64},
	{"sonicstartupangle",&od.sonicstartupangle,D_FLOAT,FLOATUP/64},
	{"sonicimpactforce",&od.sonicimpactforce,D_FLOAT,FLOATUP/64},
	{"sonicupforce",&od.sonicupforce,D_FLOAT,FLOATUP/64},
	{"sonicfireframes0",&sonicfireframes[0],D_FLOAT,FLOATUP/64},
	{"sonicfireframes1",&sonicfireframes[1],D_FLOAT,FLOATUP/64},
	{"sonicfireframes2",&sonicfireframes[2],D_FLOAT,FLOATUP/64},
	{"sonicfireframes3",&sonicfireframes[3],D_FLOAT,FLOATUP/64},
	{"sonicrad1",&od.sonicrad1,D_FLOAT,FLOATUP/64},
	{"sonicrad2",&od.sonicrad2,D_FLOAT,FLOATUP/64},
	{"sonicspincar",&od.sonicspincar,D_FLOAT,FLOATUP/64},
	{"sonicheight",&od.sonicheight,D_FLOAT,FLOATUP/64},
	{"-------THERMO---------",NULL,D_VOID,0},
	{"thermostartfuel",&od.thermostartfuel,D_INT,16},
	{"thermooffsetx",&od.thermooffset.x,D_FLOAT,FLOATUP/64},
	{"thermooffsety",&od.thermooffset.y,D_FLOAT,FLOATUP/64},
	{"thermooffsetz",&od.thermooffset.z,D_FLOAT,FLOATUP/64},
	{"thermovel",&od.thermovel,D_FLOAT,FLOATUP/64},
	{"thermoscalevel",&od.thermoscalevel,D_FLOAT,FLOATUP/64},
	{"thermorotvelx",&od.thermorotvel.x,D_FLOAT,FLOATUP/64},
	{"thermorotvely",&od.thermorotvel.y,D_FLOAT,FLOATUP/64},
	{"thermorotvelz",&od.thermorotvel.z,D_FLOAT,FLOATUP/64},
	{"thermostartupangle",&od.thermostartupangle,D_FLOAT,FLOATUP/64},
	{"thermoimpactforce",&od.thermoimpactforce,D_FLOAT,FLOATUP/64},
	{"thermoupforce",&od.thermoupforce,D_FLOAT,FLOATUP/64},
	{"thermorad",&od.thermorad,D_FLOAT,FLOATUP/64},
	{"-------HGOOK---------",NULL,D_VOID,0},
	{"grapplestartfuel",&od.grapplestartfuel,D_INT,16},
	{"grappleoffsetx",&od.grappleoffset.x,D_FLOAT,FLOATUP/64},
	{"grappleoffsety",&od.grappleoffset.y,D_FLOAT,FLOATUP/64},
	{"grappleoffsetz",&od.grappleoffset.z,D_FLOAT,FLOATUP/64},
	{"grapplevel",&od.grapplevel,D_FLOAT,FLOATUP/64},
	{"grapplestartupangle",&od.grapplestartupangle,D_FLOAT,FLOATUP/64},
	{"grappleimpactforce",&od.grappleimpactforce,D_FLOAT,FLOATUP/64},
	{"grapplerad",&od.grapplerad,D_FLOAT,FLOATUP/64},
	{"-------MAGNET---------",NULL,D_VOID,0},
//	{"fusionstartfuel",&od.fusionstartfuel,D_INT,16},
	{"magnetrad",&od.magnetrad,D_FLOAT,FLOATUP/64},
	{"-------FLASHER--------",NULL,D_VOID,0},
	{"flashermindistance",&od.flashermindistance,D_FLOAT,FLOATUP},
	{"flashermaxdistance",&od.flashermaxdistance,D_FLOAT,FLOATUP},
	{"flasherminstrength",&od.flasherminstrength,D_FLOAT,FLOATUP},
	{"flashermaxstrength",&od.flashermaxstrength,D_FLOAT,FLOATUP},
	{"-------GYROSCOPE--------",NULL,D_VOID,0},
	{"gyroscopeairtime",&od.gyroscopeairtime,D_INT,1},
	{"gyroscopeline2road",&od.gyroscopeline2road,D_FLOAT,FLOATUP/16},
	{"gyroturnrpy.x",&od.gyroturnrpy.x,D_FLOAT,FLOATUP/4},
	{"gyroturnrpy.z",&od.gyroturnrpy.z,D_FLOAT,FLOATUP/4},
	{"-------BUMPER--------",NULL,D_VOID,0},
	{"bumperkickframe",&od.bumperkickframe,D_FLOAT,FLOATUP/4},
	{"bumpershowbox",&od.bumpershowbox,D_INT,1},
	{"bumperup",&od.bumperup,D_FLOAT,FLOATUP/64},
	{"bumpersizex",&od.bumpersizex,D_FLOAT,FLOATUP/64},
	{"bumpersizez",&od.bumpersizez,D_FLOAT,FLOATUP/64},
	{"bumpersizey",&od.bumpersizey,D_FLOAT,FLOATUP/64},
	{"bumperkickamount",&od.bumperkickamount,D_FLOAT,FLOATUP/4},
	{"-------FORKLIFT--------",NULL,D_VOID,0},
	{"forkkickframe",&od.forkkickframe,D_FLOAT,FLOATUP/4},
	{"forkkickamount",&od.forkkickamount,D_FLOAT,FLOATUP/4},
	{"forkshowbox",&od.forkshowbox,D_INT,1},
	{"fork0boxmin.x",&od.fork0boxmin.x,D_FLOAT,FLOATUP/64},
	{"fork0boxmin.y",&od.fork0boxmin.y,D_FLOAT,FLOATUP/64},
	{"fork0boxmin.z",&od.fork0boxmin.z,D_FLOAT,FLOATUP/64},
	{"fork0boxmax.x",&od.fork0boxmax.x,D_FLOAT,FLOATUP/64},
	{"fork0boxmax.y",&od.fork0boxmax.y,D_FLOAT,FLOATUP/64},
	{"fork0boxmax.z",&od.fork0boxmax.z,D_FLOAT,FLOATUP/64},
	{"-------SLEDGE--------",NULL,D_VOID,0},
	{"sledgehitframe",&od.sledgehitframe,D_FLOAT,FLOATUP/64},
	{"sledgerad",&od.sledgerad,D_FLOAT,FLOATUP/64},
	{"-------GRAVITYBOMB--------",NULL,D_VOID,0},
	{"gbombstartfuel",&od.gbombstartfuel,D_INT,16},
	{"gbombup",&od.gbombup,D_FLOAT,FLOATUP/16},
	{"gbombframe",&od.gbombframe,D_FLOAT,FLOATUP/16},
	{"gbombscale",&od.gbombscale,D_FLOAT,FLOATUP/16},
//	{"gbombparm",&od.gbombparm,D_FLOAT,FLOATUP/16},
	{"gbombstartfuel",&od.gbombstartfuel,D_INT,16},
	{"gbombfvelx",&od.gbombfvel.x,D_FLOAT,FLOATUP/16},
	{"gbombfvely",&od.gbombfvel.y,D_FLOAT,FLOATUP/16},
	{"gbombfvelz",&od.gbombfvel.z,D_FLOAT,FLOATUP/16},
//	{"gbombcvelx",&od.gbombcvel.x,D_FLOAT,FLOATUP/16},
//	{"gbombcvely",&od.gbombcvel.y,D_FLOAT,FLOATUP/16},
//	{"gbombcvelz",&od.gbombcvel.z,D_FLOAT,FLOATUP/16},
	{"gbombrvelx",&od.gbombrvel.x,D_FLOAT,FLOATUP/16},
	{"gbombrvely",&od.gbombrvel.y,D_FLOAT,FLOATUP/16},
	{"gbombrvelz",&od.gbombrvel.z,D_FLOAT,FLOATUP/16},
	{"gbombgrav",&od.gbombgrav,D_FLOAT,FLOATUP/16},
	{"gbombrad",&od.gbombrad,D_FLOAT,FLOATUP/16},
	{"gbombminrad",&od.gbombminrad,D_FLOAT,FLOATUP/16},
	{"gbombstrength",&od.gbombstrength,D_FLOAT,FLOATUP/16},
	{"gbombdamp",&od.gbombdamp,D_FLOAT,FLOATUP/16},
	{"-------STASIS--------",NULL,D_VOID,0},
	{"stasisstartfuel",&od.stasisstartfuel,D_INT,16},
	{"stasisup",&od.stasisup,D_FLOAT,FLOATUP/16},
	{"stasisframe",&od.stasisframe,D_FLOAT,FLOATUP/16},
	{"stasisscale",&od.stasisscale,D_FLOAT,FLOATUP/16},
//	{"stasisparm",&od.stasisparm,D_FLOAT,FLOATUP/16},
	{"stasisstartfuel",&od.stasisstartfuel,D_INT,16},
	{"stasisfvelx",&od.stasisfvel.x,D_FLOAT,FLOATUP/16},
	{"stasisfvely",&od.stasisfvel.y,D_FLOAT,FLOATUP/16},
	{"stasisfvelz",&od.stasisfvel.z,D_FLOAT,FLOATUP/16},
//	{"stasiscvelx",&od.stasiscvel.x,D_FLOAT,FLOATUP/16},
//	{"stasiscvely",&od.stasiscvel.y,D_FLOAT,FLOATUP/16},
//	{"stasiscvelz",&od.stasiscvel.z,D_FLOAT,FLOATUP/16},
	{"stasisrvelx",&od.stasisrvel.x,D_FLOAT,FLOATUP/16},
	{"stasisrvely",&od.stasisrvel.y,D_FLOAT,FLOATUP/16},
	{"stasisrvelz",&od.stasisrvel.z,D_FLOAT,FLOATUP/16},
	{"stasisgrav",&od.stasisgrav,D_FLOAT,FLOATUP/16},
	{"stasisrad",&od.stasisrad,D_FLOAT,FLOATUP/16},
	{"-------RAMP--------",NULL,D_VOID,0},
	{"rampfront",&od.rampfront,D_FLOAT,FLOATUP/16},
	{"rampstartfuel",&od.rampstartfuel,D_INT,16},
	{"rampup",&od.rampup,D_FLOAT,FLOATUP/16},
	{"rampframe",&od.rampframe,D_FLOAT,FLOATUP/16},
	{"rampscale",&od.rampscale,D_FLOAT,FLOATUP/16},
//	{"rampparm",&od.rampparm,D_FLOAT,FLOATUP/16},
	{"rampstartfuel",&od.rampstartfuel,D_INT,16},
	{"rampfvelx",&od.rampfvel.x,D_FLOAT,FLOATUP/16},
	{"rampfvely",&od.rampfvel.y,D_FLOAT,FLOATUP/16},
	{"rampfvelz",&od.rampfvel.z,D_FLOAT,FLOATUP/16},
//	{"rampcvelx",&od.rampcvel.x,D_FLOAT,FLOATUP/16},
//	{"rampcvely",&od.rampcvel.y,D_FLOAT,FLOATUP/16},
//	{"rampcvelz",&od.rampcvel.z,D_FLOAT,FLOATUP/16},
	{"ramprvelx",&od.ramprvel.x,D_FLOAT,FLOATUP/16},
	{"ramprvely",&od.ramprvel.y,D_FLOAT,FLOATUP/16},
	{"ramprvelz",&od.ramprvel.z,D_FLOAT,FLOATUP/16},
	{"rampgrav",&od.rampgrav,D_FLOAT,FLOATUP/16},
	{"ramprad",&od.ramprad,D_FLOAT,FLOATUP/16},
	{"-------WALL--------",NULL,D_VOID,0},
	{"wallstartfuel",&od.wallstartfuel,D_INT,16},
	{"wallup",&od.wallup,D_FLOAT,FLOATUP/16},
	{"wallframe",&od.wallframe,D_FLOAT,FLOATUP/16},
	{"wallscale",&od.wallscale,D_FLOAT,FLOATUP/16},
//	{"wallparm",&od.wallparm,D_FLOAT,FLOATUP/16},
	{"wallstartfuel",&od.wallstartfuel,D_INT,16},
	{"wallfvelx",&od.wallfvel.x,D_FLOAT,FLOATUP/16},
	{"wallfvely",&od.wallfvel.y,D_FLOAT,FLOATUP/16},
	{"wallfvelz",&od.wallfvel.z,D_FLOAT,FLOATUP/16},
//	{"wallcvelx",&od.wallcvel.x,D_FLOAT,FLOATUP/16},
//	{"wallcvely",&od.wallcvel.y,D_FLOAT,FLOATUP/16},
//	{"wallcvelz",&od.wallcvel.z,D_FLOAT,FLOATUP/16},
	{"wallrvelx",&od.wallrvel.x,D_FLOAT,FLOATUP/16},
	{"wallrvely",&od.wallrvel.y,D_FLOAT,FLOATUP/16},
	{"wallrvelz",&od.wallrvel.z,D_FLOAT,FLOATUP/16},
	{"wallgrav",&od.wallgrav,D_FLOAT,FLOATUP/16},
	{"wallrad",&od.wallrad,D_FLOAT,FLOATUP/16},

	{"-------SWITCHAROO--------",NULL,D_VOID,0},
	{"switcharoorad",&od.switcharoorad,D_FLOAT,FLOATUP/16},
	{"switcharooframe",&od.switcharooframe,D_FLOAT,FLOATUP/16},




	{"-------energies--------",NULL,D_VOID,0},
//	{"enrg_fusion",&od.energies[ENRG_FUSION],D_INT,1},
//	{"enrg_elec",&od.energies[ENRG_ELEC],D_INT,1},
//	{"enrg_cyber",&od.energies[ENRG_CYBER],D_INT,1},
//	{"enrg_geothrm",&od.energies[ENRG_GEOTHRM],D_INT,1},
//	{"enrg_particle",&od.energies[ENRG_PARTICLE],D_INT,1},
//	{"enrg_chem",&od.energies[ENRG_CHEM],D_INT,1},
	{"------- WEAPSCALE -------",NULL,D_VOID,0},
	{"weapscale",&od.weapscale,D_FLOAT,FLOATUP},
	{"weapscaleregpoint",&od.weapscaleregpoint,D_FLOAT,FLOATUP},
	{"-------weapons--------",NULL,D_VOID,0},
	{"curweapicon",&od.curweapicon,D_INT,1},
	{"prevweapicon",&od.prevweapicon,D_INT,1},
	{"nextweapicon",&od.nextweapicon,D_INT,1},
	{"-------weaponstates-------",NULL,D_VOID,0},
	{"------- LASER -------",NULL,D_VOID,0},
	{"laserpersist",&weapinfos[WEAP_LASER].persist,D_INT,1},
//	{"laserdeploystart",&weapinfos[WEAP_LASER].deploystart,D_FLOAT,FLOATUP},
	{"laserdeployend",&weapinfos[WEAP_LASER].deployend,D_FLOAT,FLOATUP},
//	{"laserdeployloop",&weapinfos[WEAP_LASER].deployloop,D_FLOAT,FLOATUP},
	{"laseractstart",&weapinfos[WEAP_LASER].actstart,D_FLOAT,FLOATUP},
	{"laseractend",&weapinfos[WEAP_LASER].actend,D_FLOAT,FLOATUP},
	{"laseractloop",&weapinfos[WEAP_LASER].actloop,D_FLOAT,FLOATUP},
//	{"laserresstart",&weapinfos[WEAP_LASER].resstart,D_FLOAT,FLOATUP},
	{"laserresend",&weapinfos[WEAP_LASER].resend,D_FLOAT,FLOATUP},
//	{"laserresloop",&weapinfos[WEAP_LASER].resloop,D_FLOAT,FLOATUP},
	{"------- KLONDYKE -------",NULL,D_VOID,0},
	{"klondykepersist",&weapinfos[WEAP_KLONDYKE].persist,D_INT,1},
//	{"klondykedeploystart",&weapinfos[WEAP_KLONDYKE].deploystart,D_FLOAT,FLOATUP},
	{"klondykedeployend",&weapinfos[WEAP_KLONDYKE].deployend,D_FLOAT,FLOATUP},
//	{"klondykedeployloop",&weapinfos[WEAP_KLONDYKE].deployloop,D_FLOAT,FLOATUP},
	{"klondykeactstart",&weapinfos[WEAP_KLONDYKE].actstart,D_FLOAT,FLOATUP},
	{"klondykeactend",&weapinfos[WEAP_KLONDYKE].actend,D_FLOAT,FLOATUP},
	{"klondykeactloop",&weapinfos[WEAP_KLONDYKE].actloop,D_FLOAT,FLOATUP},
//	{"klondykeresstart",&weapinfos[WEAP_KLONDYKE].resstart,D_FLOAT,FLOATUP},
	{"klondykeresend",&weapinfos[WEAP_KLONDYKE].resend,D_FLOAT,FLOATUP},
//	{"klondykeresloop",&weapinfos[WEAP_KLONDYKE].resloop,D_FLOAT,FLOATUP},
	{"------- MISSILES -------",NULL,D_VOID,0},
	{"missilespersist",&weapinfos[WEAP_MISSILES].persist,D_INT,1},
//	{"missilesdeploystart",&weapinfos[WEAP_MISSILES].deploystart,D_FLOAT,FLOATUP},
	{"missilesdeployend",&weapinfos[WEAP_MISSILES].deployend,D_FLOAT,FLOATUP},
//	{"missilesdeployloop",&weapinfos[WEAP_MISSILES].deployloop,D_FLOAT,FLOATUP},
	{"missilesactstart",&weapinfos[WEAP_MISSILES].actstart,D_FLOAT,FLOATUP},
	{"missilesactend",&weapinfos[WEAP_MISSILES].actend,D_FLOAT,FLOATUP},
	{"missilesactloop",&weapinfos[WEAP_MISSILES].actloop,D_FLOAT,FLOATUP},
//	{"missilesresstart",&weapinfos[WEAP_MISSILES].resstart,D_FLOAT,FLOATUP},
	{"missilesresend",&weapinfos[WEAP_MISSILES].resend,D_FLOAT,FLOATUP},
//	{"missilesresloop",&weapinfos[WEAP_MISSILES].resloop,D_FLOAT,FLOATUP},
	{"------- OIL -------",NULL,D_VOID,0},
	{"oilpersist",&weapinfos[WEAP_OIL].persist,D_INT,1},
//	{"oildeploystart",&weapinfos[WEAP_OIL].deploystart,D_FLOAT,FLOATUP},
	{"oildeployend",&weapinfos[WEAP_OIL].deployend,D_FLOAT,FLOATUP},
//	{"oildeployloop",&weapinfos[WEAP_OIL].deployloop,D_FLOAT,FLOATUP},
	{"oilactstart",&weapinfos[WEAP_OIL].actstart,D_FLOAT,FLOATUP},
	{"oilactend",&weapinfos[WEAP_OIL].actend,D_FLOAT,FLOATUP},
	{"oilactloop",&weapinfos[WEAP_OIL].actloop,D_FLOAT,FLOATUP},
//	{"oilresstart",&weapinfos[WEAP_OIL].resstart,D_FLOAT,FLOATUP},
	{"oilresend",&weapinfos[WEAP_OIL].resend,D_FLOAT,FLOATUP},
//	{"oilresloop",&weapinfos[WEAP_OIL].resloop,D_FLOAT,FLOATUP},
	{"------- EMB -------",NULL,D_VOID,0},
	{"embpersist",&weapinfos[WEAP_EMB].persist,D_INT,1},
//	{"embdeploystart",&weapinfos[WEAP_EMB].deploystart,D_FLOAT,FLOATUP},
	{"embdeployend",&weapinfos[WEAP_EMB].deployend,D_FLOAT,FLOATUP},
//	{"embdeployloop",&weapinfos[WEAP_EMB].deployloop,D_FLOAT,FLOATUP},
	{"embactstart",&weapinfos[WEAP_EMB].actstart,D_FLOAT,FLOATUP},
	{"embactend",&weapinfos[WEAP_EMB].actend,D_FLOAT,FLOATUP},
	{"embactloop",&weapinfos[WEAP_EMB].actloop,D_FLOAT,FLOATUP},
//	{"embresstart",&weapinfos[WEAP_EMB].resstart,D_FLOAT,FLOATUP},
	{"embresend",&weapinfos[WEAP_EMB].resend,D_FLOAT,FLOATUP},
//	{"embresloop",&weapinfos[WEAP_EMB].resloop,D_FLOAT,FLOATUP},
	{"------- ECTO -------",NULL,D_VOID,0},
	{"ectopersist",&weapinfos[WEAP_ECTO].persist,D_INT,1},
//	{"ectodeploystart",&weapinfos[WEAP_ECTO].deploystart,D_FLOAT,FLOATUP},
	{"ectodeployend",&weapinfos[WEAP_ECTO].deployend,D_FLOAT,FLOATUP},
//	{"ectodeployloop",&weapinfos[WEAP_ECTO].deployloop,D_FLOAT,FLOATUP},
	{"ectoactstart",&weapinfos[WEAP_ECTO].actstart,D_FLOAT,FLOATUP},
	{"ectoactend",&weapinfos[WEAP_ECTO].actend,D_FLOAT,FLOATUP},
	{"ectoactloop",&weapinfos[WEAP_ECTO].actloop,D_FLOAT,FLOATUP},
//	{"ectoresstart",&weapinfos[WEAP_ECTO].resstart,D_FLOAT,FLOATUP},
	{"ectoresend",&weapinfos[WEAP_ECTO].resend,D_FLOAT,FLOATUP},
//	{"ectoresloop",&weapinfos[WEAP_ECTO].resloop,D_FLOAT,FLOATUP},
	{"------- DIELECTRIC -------",NULL,D_VOID,0},
	{"dielectricpersist",&weapinfos[WEAP_DIELECTRIC].persist,D_INT,1},
//	{"dielectricdeploystart",&weapinfos[WEAP_DIELECTRIC].deploystart,D_FLOAT,FLOATUP},
	{"dielectricdeployend",&weapinfos[WEAP_DIELECTRIC].deployend,D_FLOAT,FLOATUP},
//	{"dielectricdeployloop",&weapinfos[WEAP_DIELECTRIC].deployloop,D_FLOAT,FLOATUP},
	{"dielectricactstart",&weapinfos[WEAP_DIELECTRIC].actstart,D_FLOAT,FLOATUP},
	{"dielectricactend",&weapinfos[WEAP_DIELECTRIC].actend,D_FLOAT,FLOATUP},
	{"dielectricactloop",&weapinfos[WEAP_DIELECTRIC].actloop,D_FLOAT,FLOATUP},
//	{"dielectricresstart",&weapinfos[WEAP_DIELECTRIC].resstart,D_FLOAT,FLOATUP},
	{"dielectricresend",&weapinfos[WEAP_DIELECTRIC].resend,D_FLOAT,FLOATUP},
//	{"dielectricresloop",&weapinfos[WEAP_DIELECTRIC].resloop,D_FLOAT,FLOATUP},
	{"------- TRANSDUCER -------",NULL,D_VOID,0},
	{"transducerpersist",&weapinfos[WEAP_TRANSDUCER].persist,D_INT,1},
//	{"transducerdeploystart",&weapinfos[WEAP_TRANSDUCER].deploystart,D_FLOAT,FLOATUP},
	{"transducerdeployend",&weapinfos[WEAP_TRANSDUCER].deployend,D_FLOAT,FLOATUP},
//	{"transducerdeployloop",&weapinfos[WEAP_TRANSDUCER].deployloop,D_FLOAT,FLOATUP},
	{"transduceractstart",&weapinfos[WEAP_TRANSDUCER].actstart,D_FLOAT,FLOATUP},
	{"transduceractend",&weapinfos[WEAP_TRANSDUCER].actend,D_FLOAT,FLOATUP},
	{"transduceractloop",&weapinfos[WEAP_TRANSDUCER].actloop,D_FLOAT,FLOATUP},
//	{"transducerresstart",&weapinfos[WEAP_TRANSDUCER].resstart,D_FLOAT,FLOATUP},
	{"transducerresend",&weapinfos[WEAP_TRANSDUCER].resend,D_FLOAT,FLOATUP},
//	{"transducerresloop",&weapinfos[WEAP_TRANSDUCER].resloop,D_FLOAT,FLOATUP},
	{"------- PLASMA -------",NULL,D_VOID,0},
	{"plasmapersist",&weapinfos[WEAP_PLASMA].persist,D_INT,1},
//	{"plasmadeploystart",&weapinfos[WEAP_PLASMA].deploystart,D_FLOAT,FLOATUP},
	{"plasmadeployend",&weapinfos[WEAP_PLASMA].deployend,D_FLOAT,FLOATUP},
//	{"plasmadeployloop",&weapinfos[WEAP_PLASMA].deployloop,D_FLOAT,FLOATUP},
	{"plasmaactstart",&weapinfos[WEAP_PLASMA].actstart,D_FLOAT,FLOATUP},
	{"plasmaactend",&weapinfos[WEAP_PLASMA].actend,D_FLOAT,FLOATUP},
	{"plasmaactloop",&weapinfos[WEAP_PLASMA].actloop,D_FLOAT,FLOATUP},
//	{"plasmaresstart",&weapinfos[WEAP_PLASMA].resstart,D_FLOAT,FLOATUP},
	{"plasmaresend",&weapinfos[WEAP_PLASMA].resend,D_FLOAT,FLOATUP},
//	{"plasmaresloop",&weapinfos[WEAP_PLASMA].resloop,D_FLOAT,FLOATUP},
	{"------- RAMJET -------",NULL,D_VOID,0},
	{"ramjetpersist",&weapinfos[WEAP_RAMJET].persist,D_INT,1},
//	{"ramjetdeploystart",&weapinfos[WEAP_RAMJET].deploystart,D_FLOAT,FLOATUP},
	{"ramjetdeployend",&weapinfos[WEAP_RAMJET].deployend,D_FLOAT,FLOATUP},
//	{"ramjetdeployloop",&weapinfos[WEAP_RAMJET].deployloop,D_FLOAT,FLOATUP},
	{"ramjetactstart",&weapinfos[WEAP_RAMJET].actstart,D_FLOAT,FLOATUP},
	{"ramjetactend",&weapinfos[WEAP_RAMJET].actend,D_FLOAT,FLOATUP},
	{"ramjetactloop",&weapinfos[WEAP_RAMJET].actloop,D_FLOAT,FLOATUP},
//	{"ramjetresstart",&weapinfos[WEAP_RAMJET].resstart,D_FLOAT,FLOATUP},
	{"ramjetresend",&weapinfos[WEAP_RAMJET].resend,D_FLOAT,FLOATUP},
//	{"ramjetresloop",&weapinfos[WEAP_RAMJET].resloop,D_FLOAT,FLOATUP},
	{"------- INTAKE -------",NULL,D_VOID,0},
	{"intakepersist",&weapinfos[WEAP_INTAKE].persist,D_INT,1},
//	{"intakedeploystart",&weapinfos[WEAP_INTAKE].deploystart,D_FLOAT,FLOATUP},
	{"intakedeployend",&weapinfos[WEAP_INTAKE].deployend,D_FLOAT,FLOATUP},
//	{"intakedeployloop",&weapinfos[WEAP_INTAKE].deployloop,D_FLOAT,FLOATUP},
	{"intakeactstart",&weapinfos[WEAP_INTAKE].actstart,D_FLOAT,FLOATUP},
	{"intakeactend",&weapinfos[WEAP_INTAKE].actend,D_FLOAT,FLOATUP},
	{"intakeactloop",&weapinfos[WEAP_INTAKE].actloop,D_FLOAT,FLOATUP},
//	{"intakeresstart",&weapinfos[WEAP_INTAKE].resstart,D_FLOAT,FLOATUP},
	{"intakeresend",&weapinfos[WEAP_INTAKE].resend,D_FLOAT,FLOATUP},
//	{"intakeresloop",&weapinfos[WEAP_INTAKE].resloop,D_FLOAT,FLOATUP},
	{"------- SPIKES -------",NULL,D_VOID,0},
	{"spikespersist",&weapinfos[WEAP_SPIKES].persist,D_INT,1},
//	{"spikesdeploystart",&weapinfos[WEAP_SPIKES].deploystart,D_FLOAT,FLOATUP},
	{"spikesdeployend",&weapinfos[WEAP_SPIKES].deployend,D_FLOAT,FLOATUP},
//	{"spikesdeployloop",&weapinfos[WEAP_SPIKES].deployloop,D_FLOAT,FLOATUP},
	{"spikesactstart",&weapinfos[WEAP_SPIKES].actstart,D_FLOAT,FLOATUP},
	{"spikesactend",&weapinfos[WEAP_SPIKES].actend,D_FLOAT,FLOATUP},
	{"spikesactloop",&weapinfos[WEAP_SPIKES].actloop,D_FLOAT,FLOATUP},
//	{"spikesresstart",&weapinfos[WEAP_SPIKES].resstart,D_FLOAT,FLOATUP},
	{"spikesresend",&weapinfos[WEAP_SPIKES].resend,D_FLOAT,FLOATUP},
//	{"spikesresloop",&weapinfos[WEAP_SPIKES].resloop,D_FLOAT,FLOATUP},
	{"------- BAINITE -------",NULL,D_VOID,0},
	{"bainitepersist",&weapinfos[WEAP_BAINITE].persist,D_INT,1},
//	{"bainitedeploystart",&weapinfos[WEAP_BAINITE].deploystart,D_FLOAT,FLOATUP},
	{"bainitedeployend",&weapinfos[WEAP_BAINITE].deployend,D_FLOAT,FLOATUP},
//	{"bainitedeployloop",&weapinfos[WEAP_BAINITE].deployloop,D_FLOAT,FLOATUP},
	{"bainiteactstart",&weapinfos[WEAP_BAINITE].actstart,D_FLOAT,FLOATUP},
	{"bainiteactend",&weapinfos[WEAP_BAINITE].actend,D_FLOAT,FLOATUP},
	{"bainiteactloop",&weapinfos[WEAP_BAINITE].actloop,D_FLOAT,FLOATUP},
//	{"bainiteresstart",&weapinfos[WEAP_BAINITE].resstart,D_FLOAT,FLOATUP},
	{"bainiteresend",&weapinfos[WEAP_BAINITE].resend,D_FLOAT,FLOATUP},
//	{"bainiteresloop",&weapinfos[WEAP_BAINITE].resloop,D_FLOAT,FLOATUP},
	{"------- AERODYNE -------",NULL,D_VOID,0},
	{"aerodynepersist",&weapinfos[WEAP_AERODYNE].persist,D_INT,1},
//	{"aerodynedeploystart",&weapinfos[WEAP_AERODYNE].deploystart,D_FLOAT,FLOATUP},
	{"aerodynedeployend",&weapinfos[WEAP_AERODYNE].deployend,D_FLOAT,FLOATUP},
//	{"aerodynedeployloop",&weapinfos[WEAP_AERODYNE].deployloop,D_FLOAT,FLOATUP},
	{"aerodyneactstart",&weapinfos[WEAP_AERODYNE].actstart,D_FLOAT,FLOATUP},
	{"aerodyneactend",&weapinfos[WEAP_AERODYNE].actend,D_FLOAT,FLOATUP},
	{"aerodyneactloop",&weapinfos[WEAP_AERODYNE].actloop,D_FLOAT,FLOATUP},
//	{"aerodyneresstart",&weapinfos[WEAP_AERODYNE].resstart,D_FLOAT,FLOATUP},
	{"aerodyneresend",&weapinfos[WEAP_AERODYNE].resend,D_FLOAT,FLOATUP},
//	{"aerodyneresloop",&weapinfos[WEAP_AERODYNE].resloop,D_FLOAT,FLOATUP},
	{"------- FERRITE -------",NULL,D_VOID,0},
	{"ferritepersist",&weapinfos[WEAP_FERRITE].persist,D_INT,1},
//	{"ferritedeploystart",&weapinfos[WEAP_FERRITE].deploystart,D_FLOAT,FLOATUP},
	{"ferritedeployend",&weapinfos[WEAP_FERRITE].deployend,D_FLOAT,FLOATUP},
//	{"ferritedeployloop",&weapinfos[WEAP_FERRITE].deployloop,D_FLOAT,FLOATUP},
	{"ferriteactstart",&weapinfos[WEAP_FERRITE].actstart,D_FLOAT,FLOATUP},
	{"ferriteactend",&weapinfos[WEAP_FERRITE].actend,D_FLOAT,FLOATUP},
	{"ferriteactloop",&weapinfos[WEAP_FERRITE].actloop,D_FLOAT,FLOATUP},
//	{"ferriteresstart",&weapinfos[WEAP_FERRITE].resstart,D_FLOAT,FLOATUP},
	{"ferriteresend",&weapinfos[WEAP_FERRITE].resend,D_FLOAT,FLOATUP},
//	{"ferriteresloop",&weapinfos[WEAP_FERRITE].resloop,D_FLOAT,FLOATUP},
	{"------- WEAPBOOST -------",NULL,D_VOID,0},
	{"weapboostpersist",&weapinfos[WEAP_WEAPBOOST].persist,D_INT,1},
//	{"weapboostdeploystart",&weapinfos[WEAP_WEAPBOOST].deploystart,D_FLOAT,FLOATUP},
	{"weapboostdeployend",&weapinfos[WEAP_WEAPBOOST].deployend,D_FLOAT,FLOATUP},
//	{"weapboostdeployloop",&weapinfos[WEAP_WEAPBOOST].deployloop,D_FLOAT,FLOATUP},
	{"weapboostactstart",&weapinfos[WEAP_WEAPBOOST].actstart,D_FLOAT,FLOATUP},
	{"weapboostactend",&weapinfos[WEAP_WEAPBOOST].actend,D_FLOAT,FLOATUP},
	{"weapboostactloop",&weapinfos[WEAP_WEAPBOOST].actloop,D_FLOAT,FLOATUP},
//	{"weapboostresstart",&weapinfos[WEAP_WEAPBOOST].resstart,D_FLOAT,FLOATUP},
	{"weapboostresend",&weapinfos[WEAP_WEAPBOOST].resend,D_FLOAT,FLOATUP},
//	{"weapboostresloop",&weapinfos[WEAP_WEAPBOOST].resloop,D_FLOAT,FLOATUP},
	{"------- RPD -------",NULL,D_VOID,0},
	{"rpdpersist",&weapinfos[WEAP_RPD].persist,D_INT,1},
//	{"rpddeploystart",&weapinfos[WEAP_RPD].deploystart,D_FLOAT,FLOATUP},
	{"rpddeployend",&weapinfos[WEAP_RPD].deployend,D_FLOAT,FLOATUP},
//	{"rpddeployloop",&weapinfos[WEAP_RPD].deployloop,D_FLOAT,FLOATUP},
	{"rpdactstart",&weapinfos[WEAP_RPD].actstart,D_FLOAT,FLOATUP},
	{"rpdactend",&weapinfos[WEAP_RPD].actend,D_FLOAT,FLOATUP},
	{"rpdactloop",&weapinfos[WEAP_RPD].actloop,D_FLOAT,FLOATUP},
//	{"rpdresstart",&weapinfos[WEAP_RPD].resstart,D_FLOAT,FLOATUP},
	{"rpdresend",&weapinfos[WEAP_RPD].resend,D_FLOAT,FLOATUP},
//	{"rpdresloop",&weapinfos[WEAP_RPD].resloop,D_FLOAT,FLOATUP},
	{"------- NANO -------",NULL,D_VOID,0},
	{"nanopersist",&weapinfos[WEAP_NANO].persist,D_INT,1},
//	{"nanodeploystart",&weapinfos[WEAP_NANO].deploystart,D_FLOAT,FLOATUP},
	{"nanodeployend",&weapinfos[WEAP_NANO].deployend,D_FLOAT,FLOATUP},
//	{"nanodeployloop",&weapinfos[WEAP_NANO].deployloop,D_FLOAT,FLOATUP},
	{"nanoactstart",&weapinfos[WEAP_NANO].actstart,D_FLOAT,FLOATUP},
	{"nanoactend",&weapinfos[WEAP_NANO].actend,D_FLOAT,FLOATUP},
	{"nanoactloop",&weapinfos[WEAP_NANO].actloop,D_FLOAT,FLOATUP},
//	{"nanoresstart",&weapinfos[WEAP_NANO].resstart,D_FLOAT,FLOATUP},
	{"nanoresend",&weapinfos[WEAP_NANO].resend,D_FLOAT,FLOATUP},
//	{"nanoresloop",&weapinfos[WEAP_NANO].resloop,D_FLOAT,FLOATUP},
	{"------- RANDOMIZER -------",NULL,D_VOID,0},
	{"randomizerpersist",&weapinfos[WEAP_RANDOMIZER].persist,D_INT,1},
//	{"randomizerdeploystart",&weapinfos[WEAP_RANDOMIZER].deploystart,D_FLOAT,FLOATUP},
	{"randomizerdeployend",&weapinfos[WEAP_RANDOMIZER].deployend,D_FLOAT,FLOATUP},
//	{"randomizerdeployloop",&weapinfos[WEAP_RANDOMIZER].deployloop,D_FLOAT,FLOATUP},
	{"randomizeractstart",&weapinfos[WEAP_RANDOMIZER].actstart,D_FLOAT,FLOATUP},
	{"randomizeractend",&weapinfos[WEAP_RANDOMIZER].actend,D_FLOAT,FLOATUP},
	{"randomizeractloop",&weapinfos[WEAP_RANDOMIZER].actloop,D_FLOAT,FLOATUP},
//	{"randomizerresstart",&weapinfos[WEAP_RANDOMIZER].resstart,D_FLOAT,FLOATUP},
	{"randomizerresend",&weapinfos[WEAP_RANDOMIZER].resend,D_FLOAT,FLOATUP},
//	{"randomizerresloop",&weapinfos[WEAP_RANDOMIZER].resloop,D_FLOAT,FLOATUP},
	{"------- ICRMS -------",NULL,D_VOID,0},
	{"icrmspersist",&weapinfos[WEAP_ICRMS].persist,D_INT,1},
//	{"icrmsdeploystart",&weapinfos[WEAP_ICRMS].deploystart,D_FLOAT,FLOATUP},
	{"icrmsdeployend",&weapinfos[WEAP_ICRMS].deployend,D_FLOAT,FLOATUP},
//	{"icrmsdeployloop",&weapinfos[WEAP_ICRMS].deployloop,D_FLOAT,FLOATUP},
	{"icrmsactstart",&weapinfos[WEAP_ICRMS].actstart,D_FLOAT,FLOATUP},
	{"icrmsactend",&weapinfos[WEAP_ICRMS].actend,D_FLOAT,FLOATUP},
	{"icrmsactloop",&weapinfos[WEAP_ICRMS].actloop,D_FLOAT,FLOATUP},
//	{"icrmsresstart",&weapinfos[WEAP_ICRMS].resstart,D_FLOAT,FLOATUP},
	{"icrmsresend",&weapinfos[WEAP_ICRMS].resend,D_FLOAT,FLOATUP},
//	{"icrmsresloop",&weapinfos[WEAP_ICRMS].resloop,D_FLOAT,FLOATUP},
	{"------- BIGTIRES -------",NULL,D_VOID,0},
	{"bigtirespersist",&weapinfos[WEAP_BIGTIRES].persist,D_INT,1},
//	{"bigtiresdeploystart",&weapinfos[WEAP_BIGTIRES].deploystart,D_FLOAT,FLOATUP},
	{"bigtiresdeployend",&weapinfos[WEAP_BIGTIRES].deployend,D_FLOAT,FLOATUP},
//	{"bigtiresdeployloop",&weapinfos[WEAP_BIGTIRES].deployloop,D_FLOAT,FLOATUP},
	{"bigtiresactstart",&weapinfos[WEAP_BIGTIRES].actstart,D_FLOAT,FLOATUP},
	{"bigtiresactend",&weapinfos[WEAP_BIGTIRES].actend,D_FLOAT,FLOATUP},
	{"bigtiresactloop",&weapinfos[WEAP_BIGTIRES].actloop,D_FLOAT,FLOATUP},
//	{"bigtiresresstart",&weapinfos[WEAP_BIGTIRES].resstart,D_FLOAT,FLOATUP},
	{"bigtiresresend",&weapinfos[WEAP_BIGTIRES].resend,D_FLOAT,FLOATUP},
//	{"bigtiresresloop",&weapinfos[WEAP_BIGTIRES].resloop,D_FLOAT,FLOATUP},
	{"------- BUZZ -------",NULL,D_VOID,0},
	{"buzzpersist",&weapinfos[WEAP_BUZZ].persist,D_INT,1},
//	{"buzzdeploystart",&weapinfos[WEAP_BUZZ].deploystart,D_FLOAT,FLOATUP},
	{"buzzdeployend",&weapinfos[WEAP_BUZZ].deployend,D_FLOAT,FLOATUP},
//	{"buzzdeployloop",&weapinfos[WEAP_BUZZ].deployloop,D_FLOAT,FLOATUP},
	{"buzzactstart",&weapinfos[WEAP_BUZZ].actstart,D_FLOAT,FLOATUP},
	{"buzzactend",&weapinfos[WEAP_BUZZ].actend,D_FLOAT,FLOATUP},
	{"buzzactloop",&weapinfos[WEAP_BUZZ].actloop,D_FLOAT,FLOATUP},
//	{"buzzresstart",&weapinfos[WEAP_BUZZ].resstart,D_FLOAT,FLOATUP},
	{"buzzresend",&weapinfos[WEAP_BUZZ].resend,D_FLOAT,FLOATUP},
//	{"buzzresloop",&weapinfos[WEAP_BUZZ].resloop,D_FLOAT,FLOATUP},
/*	{"------- CHEMBOOST -------",NULL,D_VOID,0},
	{"chemboostpersist",&weapinfos[WEAP_CHEMBOOST].persist,D_INT,1},
	{"chemboostdeploystart",&weapinfos[WEAP_CHEMBOOST].deploystart,D_FLOAT,FLOATUP},
	{"chemboostdeployend",&weapinfos[WEAP_CHEMBOOST].deployend,D_FLOAT,FLOATUP},
	{"chemboostdeployloop",&weapinfos[WEAP_CHEMBOOST].deployloop,D_FLOAT,FLOATUP},
	{"chemboostactstart",&weapinfos[WEAP_CHEMBOOST].actstart,D_FLOAT,FLOATUP},
	{"chemboostactend",&weapinfos[WEAP_CHEMBOOST].actend,D_FLOAT,FLOATUP},
	{"chemboostactloop",&weapinfos[WEAP_CHEMBOOST].actloop,D_FLOAT,FLOATUP},
	{"chemboostresstart",&weapinfos[WEAP_CHEMBOOST].resstart,D_FLOAT,FLOATUP},
	{"chemboostresend",&weapinfos[WEAP_CHEMBOOST].resend,D_FLOAT,FLOATUP},
	{"chemboostresloop",&weapinfos[WEAP_CHEMBOOST].resloop,D_FLOAT,FLOATUP},
	{"------- CHEMVAC -------",NULL,D_VOID,0},
	{"chemvacpersist",&weapinfos[WEAP_CHEMVAC].persist,D_INT,1},
	{"chemvacdeploystart",&weapinfos[WEAP_CHEMVAC].deploystart,D_FLOAT,FLOATUP},
	{"chemvacdeployend",&weapinfos[WEAP_CHEMVAC].deployend,D_FLOAT,FLOATUP},
	{"chemvacdeployloop",&weapinfos[WEAP_CHEMVAC].deployloop,D_FLOAT,FLOATUP},
	{"chemvacactstart",&weapinfos[WEAP_CHEMVAC].actstart,D_FLOAT,FLOATUP},
	{"chemvacactend",&weapinfos[WEAP_CHEMVAC].actend,D_FLOAT,FLOATUP},
	{"chemvacactloop",&weapinfos[WEAP_CHEMVAC].actloop,D_FLOAT,FLOATUP},
	{"chemvacresstart",&weapinfos[WEAP_CHEMVAC].resstart,D_FLOAT,FLOATUP},
	{"chemvacresend",&weapinfos[WEAP_CHEMVAC].resend,D_FLOAT,FLOATUP},
	{"chemvacresloop",&weapinfos[WEAP_CHEMVAC].resloop,D_FLOAT,FLOATUP}, */
	{"------- ENLEECH -------",NULL,D_VOID,0},
	{"enleechpersist",&weapinfos[WEAP_ENLEECH].persist,D_INT,1},
//	{"enleechdeploystart",&weapinfos[WEAP_ENLEECH].deploystart,D_FLOAT,FLOATUP},
	{"enleechdeployend",&weapinfos[WEAP_ENLEECH].deployend,D_FLOAT,FLOATUP},
//	{"enleechdeployloop",&weapinfos[WEAP_ENLEECH].deployloop,D_FLOAT,FLOATUP},
	{"enleechactstart",&weapinfos[WEAP_ENLEECH].actstart,D_FLOAT,FLOATUP},
	{"enleechactend",&weapinfos[WEAP_ENLEECH].actend,D_FLOAT,FLOATUP},
	{"enleechactloop",&weapinfos[WEAP_ENLEECH].actloop,D_FLOAT,FLOATUP},
//	{"enleechresstart",&weapinfos[WEAP_ENLEECH].resstart,D_FLOAT,FLOATUP},
	{"enleechresend",&weapinfos[WEAP_ENLEECH].resend,D_FLOAT,FLOATUP},
//	{"enleechresloop",&weapinfos[WEAP_ENLEECH].resloop,D_FLOAT,FLOATUP},
/*	{"------- ENUPTAKE -------",NULL,D_VOID,0},
	{"enuptakepersist",&weapinfos[WEAP_ENUPTAKE].persist,D_INT,1},
	{"enuptakedeploystart",&weapinfos[WEAP_ENUPTAKE].deploystart,D_FLOAT,FLOATUP},
	{"enuptakedeployend",&weapinfos[WEAP_ENUPTAKE].deployend,D_FLOAT,FLOATUP},
	{"enuptakedeployloop",&weapinfos[WEAP_ENUPTAKE].deployloop,D_FLOAT,FLOATUP},
	{"enuptakeactstart",&weapinfos[WEAP_ENUPTAKE].actstart,D_FLOAT,FLOATUP},
	{"enuptakeactend",&weapinfos[WEAP_ENUPTAKE].actend,D_FLOAT,FLOATUP},
	{"enuptakeactloop",&weapinfos[WEAP_ENUPTAKE].actloop,D_FLOAT,FLOATUP},
	{"enuptakeresstart",&weapinfos[WEAP_ENUPTAKE].resstart,D_FLOAT,FLOATUP},
	{"enuptakeresend",&weapinfos[WEAP_ENUPTAKE].resend,D_FLOAT,FLOATUP},
	{"enuptakeresloop",&weapinfos[WEAP_ENUPTAKE].resloop,D_FLOAT,FLOATUP},
	{"------- FSNBOO -------",NULL,D_VOID,0},
	{"fsnboopersist",&weapinfos[WEAP_FSNBOO].persist,D_INT,1},
	{"fsnboodeploystart",&weapinfos[WEAP_FSNBOO].deploystart,D_FLOAT,FLOATUP},
	{"fsnboodeployend",&weapinfos[WEAP_FSNBOO].deployend,D_FLOAT,FLOATUP},
	{"fsnboodeployloop",&weapinfos[WEAP_FSNBOO].deployloop,D_FLOAT,FLOATUP},
	{"fsnbooactstart",&weapinfos[WEAP_FSNBOO].actstart,D_FLOAT,FLOATUP},
	{"fsnbooactend",&weapinfos[WEAP_FSNBOO].actend,D_FLOAT,FLOATUP},
	{"fsnbooactloop",&weapinfos[WEAP_FSNBOO].actloop,D_FLOAT,FLOATUP},
	{"fsnbooresstart",&weapinfos[WEAP_FSNBOO].resstart,D_FLOAT,FLOATUP},
	{"fsnbooresend",&weapinfos[WEAP_FSNBOO].resend,D_FLOAT,FLOATUP},
	{"fsnbooresloop",&weapinfos[WEAP_FSNBOO].resloop,D_FLOAT,FLOATUP}, */
	{"------- FSNCAN -------",NULL,D_VOID,0},
	{"fsncanpersist",&weapinfos[WEAP_FSNCAN].persist,D_INT,1},
//	{"fsncandeploystart",&weapinfos[WEAP_FSNCAN].deploystart,D_FLOAT,FLOATUP},
	{"fsncandeployend",&weapinfos[WEAP_FSNCAN].deployend,D_FLOAT,FLOATUP},
//	{"fsncandeployloop",&weapinfos[WEAP_FSNCAN].deployloop,D_FLOAT,FLOATUP},
	{"fsncanactstart",&weapinfos[WEAP_FSNCAN].actstart,D_FLOAT,FLOATUP},
	{"fsncanactend",&weapinfos[WEAP_FSNCAN].actend,D_FLOAT,FLOATUP},
	{"fsncanactloop",&weapinfos[WEAP_FSNCAN].actloop,D_FLOAT,FLOATUP},
//	{"fsncanresstart",&weapinfos[WEAP_FSNCAN].resstart,D_FLOAT,FLOATUP},
	{"fsncanresend",&weapinfos[WEAP_FSNCAN].resend,D_FLOAT,FLOATUP},
//	{"fsncanresloop",&weapinfos[WEAP_FSNCAN].resloop,D_FLOAT,FLOATUP},
/*	{"------- GEOSHIELD -------",NULL,D_VOID,0},
	{"geoshieldpersist",&weapinfos[WEAP_GEOSHIELD].persist,D_INT,1},
	{"geoshielddeploystart",&weapinfos[WEAP_GEOSHIELD].deploystart,D_FLOAT,FLOATUP},
	{"geoshielddeployend",&weapinfos[WEAP_GEOSHIELD].deployend,D_FLOAT,FLOATUP},
	{"geoshielddeployloop",&weapinfos[WEAP_GEOSHIELD].deployloop,D_FLOAT,FLOATUP},
	{"geoshieldactstart",&weapinfos[WEAP_GEOSHIELD].actstart,D_FLOAT,FLOATUP},
	{"geoshieldactend",&weapinfos[WEAP_GEOSHIELD].actend,D_FLOAT,FLOATUP},
	{"geoshieldactloop",&weapinfos[WEAP_GEOSHIELD].actloop,D_FLOAT,FLOATUP},
	{"geoshieldresstart",&weapinfos[WEAP_GEOSHIELD].resstart,D_FLOAT,FLOATUP},
	{"geoshieldresend",&weapinfos[WEAP_GEOSHIELD].resend,D_FLOAT,FLOATUP},
	{"geoshieldresloop",&weapinfos[WEAP_GEOSHIELD].resloop,D_FLOAT,FLOATUP}, */
	{"------- GRAPPLE -------",NULL,D_VOID,0},
	{"grapplepersist",&weapinfos[WEAP_GRAPPLE].persist,D_INT,1},
//	{"grappledeploystart",&weapinfos[WEAP_GRAPPLE].deploystart,D_FLOAT,FLOATUP},
	{"grappledeployend",&weapinfos[WEAP_GRAPPLE].deployend,D_FLOAT,FLOATUP},
//	{"grappledeployloop",&weapinfos[WEAP_GRAPPLE].deployloop,D_FLOAT,FLOATUP},
	{"grappleactstart",&weapinfos[WEAP_GRAPPLE].actstart,D_FLOAT,FLOATUP},
	{"grappleactend",&weapinfos[WEAP_GRAPPLE].actend,D_FLOAT,FLOATUP},
	{"grappleactloop",&weapinfos[WEAP_GRAPPLE].actloop,D_FLOAT,FLOATUP},
//	{"grappleresstart",&weapinfos[WEAP_GRAPPLE].resstart,D_FLOAT,FLOATUP},
	{"grappleresend",&weapinfos[WEAP_GRAPPLE].resend,D_FLOAT,FLOATUP},
//	{"grappleresloop",&weapinfos[WEAP_GRAPPLE].resloop,D_FLOAT,FLOATUP},
/*	{"------- HOLO -------",NULL,D_VOID,0},
	{"holopersist",&weapinfos[WEAP_HOLO].persist,D_INT,1},
	{"holodeploystart",&weapinfos[WEAP_HOLO].deploystart,D_FLOAT,FLOATUP},
	{"holodeployend",&weapinfos[WEAP_HOLO].deployend,D_FLOAT,FLOATUP},
	{"holodeployloop",&weapinfos[WEAP_HOLO].deployloop,D_FLOAT,FLOATUP},
	{"holoactstart",&weapinfos[WEAP_HOLO].actstart,D_FLOAT,FLOATUP},
	{"holoactend",&weapinfos[WEAP_HOLO].actend,D_FLOAT,FLOATUP},
	{"holoactloop",&weapinfos[WEAP_HOLO].actloop,D_FLOAT,FLOATUP},
	{"holoresstart",&weapinfos[WEAP_HOLO].resstart,D_FLOAT,FLOATUP},
	{"holoresend",&weapinfos[WEAP_HOLO].resend,D_FLOAT,FLOATUP},
	{"holoresloop",&weapinfos[WEAP_HOLO].resloop,D_FLOAT,FLOATUP}, */
	{"------- MAGNET -------",NULL,D_VOID,0},
	{"magnetpersist",&weapinfos[WEAP_MAGNET].persist,D_INT,1},
//	{"magnetdeploystart",&weapinfos[WEAP_MAGNET].deploystart,D_FLOAT,FLOATUP},
	{"magnetdeployend",&weapinfos[WEAP_MAGNET].deployend,D_FLOAT,FLOATUP},
//	{"magnetdeployloop",&weapinfos[WEAP_MAGNET].deployloop,D_FLOAT,FLOATUP},
	{"magnetactstart",&weapinfos[WEAP_MAGNET].actstart,D_FLOAT,FLOATUP},
	{"magnetactend",&weapinfos[WEAP_MAGNET].actend,D_FLOAT,FLOATUP},
	{"magnetactloop",&weapinfos[WEAP_MAGNET].actloop,D_FLOAT,FLOATUP},
//	{"magnetresstart",&weapinfos[WEAP_MAGNET].resstart,D_FLOAT,FLOATUP},
	{"magnetresend",&weapinfos[WEAP_MAGNET].resend,D_FLOAT,FLOATUP},
//	{"magnetresloop",&weapinfos[WEAP_MAGNET].resloop,D_FLOAT,FLOATUP},
	{"------- MINES -------",NULL,D_VOID,0},
	{"minespersist",&weapinfos[WEAP_MINES].persist,D_INT,1},
//	{"minesdeploystart",&weapinfos[WEAP_MINES].deploystart,D_FLOAT,FLOATUP},
	{"minesdeployend",&weapinfos[WEAP_MINES].deployend,D_FLOAT,FLOATUP},
//	{"minesdeployloop",&weapinfos[WEAP_MINES].deployloop,D_FLOAT,FLOATUP},
	{"minesactstart",&weapinfos[WEAP_MINES].actstart,D_FLOAT,FLOATUP},
	{"minesactend",&weapinfos[WEAP_MINES].actend,D_FLOAT,FLOATUP},
	{"minesactloop",&weapinfos[WEAP_MINES].actloop,D_FLOAT,FLOATUP},
//	{"minesresstart",&weapinfos[WEAP_MINES].resstart,D_FLOAT,FLOATUP},
	{"minesresend",&weapinfos[WEAP_MINES].resend,D_FLOAT,FLOATUP},
//	{"minesresloop",&weapinfos[WEAP_MINES].resloop,D_FLOAT,FLOATUP},
/*	{"------- NITRO -------",NULL,D_VOID,0},
	{"nitropersist",&weapinfos[WEAP_NITRO].persist,D_INT,1},
	{"nitrodeploystart",&weapinfos[WEAP_NITRO].deploystart,D_FLOAT,FLOATUP},
	{"nitrodeployend",&weapinfos[WEAP_NITRO].deployend,D_FLOAT,FLOATUP},
	{"nitrodeployloop",&weapinfos[WEAP_NITRO].deployloop,D_FLOAT,FLOATUP},
	{"nitroactstart",&weapinfos[WEAP_NITRO].actstart,D_FLOAT,FLOATUP},
	{"nitroactend",&weapinfos[WEAP_NITRO].actend,D_FLOAT,FLOATUP},
	{"nitroactloop",&weapinfos[WEAP_NITRO].actloop,D_FLOAT,FLOATUP},
	{"nitroresstart",&weapinfos[WEAP_NITRO].resstart,D_FLOAT,FLOATUP},
	{"nitroresend",&weapinfos[WEAP_NITRO].resend,D_FLOAT,FLOATUP},
	{"nitroresloop",&weapinfos[WEAP_NITRO].resloop,D_FLOAT,FLOATUP},
	{"------- NRGCON -------",NULL,D_VOID,0},
	{"nrgconpersist",&weapinfos[WEAP_NRGCON].persist,D_INT,1},
	{"nrgcondeploystart",&weapinfos[WEAP_NRGCON].deploystart,D_FLOAT,FLOATUP},
	{"nrgcondeployend",&weapinfos[WEAP_NRGCON].deployend,D_FLOAT,FLOATUP},
	{"nrgcondeployloop",&weapinfos[WEAP_NRGCON].deployloop,D_FLOAT,FLOATUP},
	{"nrgconactstart",&weapinfos[WEAP_NRGCON].actstart,D_FLOAT,FLOATUP},
	{"nrgconactend",&weapinfos[WEAP_NRGCON].actend,D_FLOAT,FLOATUP},
	{"nrgconactloop",&weapinfos[WEAP_NRGCON].actloop,D_FLOAT,FLOATUP},
	{"nrgconresstart",&weapinfos[WEAP_NRGCON].resstart,D_FLOAT,FLOATUP},
	{"nrgconresend",&weapinfos[WEAP_NRGCON].resend,D_FLOAT,FLOATUP},
	{"nrgconresloop",&weapinfos[WEAP_NRGCON].resloop,D_FLOAT,FLOATUP},
	{"------- POOLBOO -------",NULL,D_VOID,0},
	{"poolboopersist",&weapinfos[WEAP_POOLBOO].persist,D_INT,1},
	{"poolboodeploystart",&weapinfos[WEAP_POOLBOO].deploystart,D_FLOAT,FLOATUP},
	{"poolboodeployend",&weapinfos[WEAP_POOLBOO].deployend,D_FLOAT,FLOATUP},
	{"poolboodeployloop",&weapinfos[WEAP_POOLBOO].deployloop,D_FLOAT,FLOATUP},
	{"poolbooactstart",&weapinfos[WEAP_POOLBOO].actstart,D_FLOAT,FLOATUP},
	{"poolbooactend",&weapinfos[WEAP_POOLBOO].actend,D_FLOAT,FLOATUP},
	{"poolbooactloop",&weapinfos[WEAP_POOLBOO].actloop,D_FLOAT,FLOATUP},
	{"poolbooresstart",&weapinfos[WEAP_POOLBOO].resstart,D_FLOAT,FLOATUP},
	{"poolbooresend",&weapinfos[WEAP_POOLBOO].resend,D_FLOAT,FLOATUP},
	{"poolbooresloop",&weapinfos[WEAP_POOLBOO].resloop,D_FLOAT,FLOATUP}, */
	{"------- POUNCER -------",NULL,D_VOID,0},
	{"pouncerpersist",&weapinfos[WEAP_POUNCER].persist,D_INT,1},
//	{"pouncerdeploystart",&weapinfos[WEAP_POUNCER].deploystart,D_FLOAT,FLOATUP},
	{"pouncerdeployend",&weapinfos[WEAP_POUNCER].deployend,D_FLOAT,FLOATUP},
//	{"pouncerdeployloop",&weapinfos[WEAP_POUNCER].deployloop,D_FLOAT,FLOATUP},
	{"pounceractstart",&weapinfos[WEAP_POUNCER].actstart,D_FLOAT,FLOATUP},
	{"pounceractend",&weapinfos[WEAP_POUNCER].actend,D_FLOAT,FLOATUP},
	{"pounceractloop",&weapinfos[WEAP_POUNCER].actloop,D_FLOAT,FLOATUP},
//	{"pouncerresstart",&weapinfos[WEAP_POUNCER].resstart,D_FLOAT,FLOATUP},
	{"pouncerresend",&weapinfos[WEAP_POUNCER].resend,D_FLOAT,FLOATUP},
//	{"pouncerresloop",&weapinfos[WEAP_POUNCER].resloop,D_FLOAT,FLOATUP},
	{"------- PRISM -------",NULL,D_VOID,0},
	{"prismpersist",&weapinfos[WEAP_PRISM].persist,D_INT,1},
//	{"prismdeploystart",&weapinfos[WEAP_PRISM].deploystart,D_FLOAT,FLOATUP},
	{"prismdeployend",&weapinfos[WEAP_PRISM].deployend,D_FLOAT,FLOATUP},
//	{"prismdeployloop",&weapinfos[WEAP_PRISM].deployloop,D_FLOAT,FLOATUP},
	{"prismactstart",&weapinfos[WEAP_PRISM].actstart,D_FLOAT,FLOATUP},
	{"prismactend",&weapinfos[WEAP_PRISM].actend,D_FLOAT,FLOATUP},
	{"prismactloop",&weapinfos[WEAP_PRISM].actloop,D_FLOAT,FLOATUP},
//	{"prismresstart",&weapinfos[WEAP_PRISM].resstart,D_FLOAT,FLOATUP},
	{"prismresend",&weapinfos[WEAP_PRISM].resend,D_FLOAT,FLOATUP},
//	{"prismresloop",&weapinfos[WEAP_PRISM].resloop,D_FLOAT,FLOATUP},
/*	{"------- SHLDBSTR -------",NULL,D_VOID,0},
	{"shldbstrpersist",&weapinfos[WEAP_SHLDBSTR].persist,D_INT,1},
	{"shldbstrdeploystart",&weapinfos[WEAP_SHLDBSTR].deploystart,D_FLOAT,FLOATUP},
	{"shldbstrdeployend",&weapinfos[WEAP_SHLDBSTR].deployend,D_FLOAT,FLOATUP},
	{"shldbstrdeployloop",&weapinfos[WEAP_SHLDBSTR].deployloop,D_FLOAT,FLOATUP},
	{"shldbstractstart",&weapinfos[WEAP_SHLDBSTR].actstart,D_FLOAT,FLOATUP},
	{"shldbstractend",&weapinfos[WEAP_SHLDBSTR].actend,D_FLOAT,FLOATUP},
	{"shldbstractloop",&weapinfos[WEAP_SHLDBSTR].actloop,D_FLOAT,FLOATUP},
	{"shldbstrresstart",&weapinfos[WEAP_SHLDBSTR].resstart,D_FLOAT,FLOATUP},
	{"shldbstrresend",&weapinfos[WEAP_SHLDBSTR].resend,D_FLOAT,FLOATUP},
	{"shldbstrresloop",&weapinfos[WEAP_SHLDBSTR].resloop,D_FLOAT,FLOATUP}, */
	{"------- SONIC -------",NULL,D_VOID,0},
	{"sonicpersist",&weapinfos[WEAP_SONIC].persist,D_INT,1},
//	{"sonicdeploystart",&weapinfos[WEAP_SONIC].deploystart,D_FLOAT,FLOATUP},
	{"sonicdeployend",&weapinfos[WEAP_SONIC].deployend,D_FLOAT,FLOATUP},
//	{"sonicdeployloop",&weapinfos[WEAP_SONIC].deployloop,D_FLOAT,FLOATUP},
	{"sonicactstart",&weapinfos[WEAP_SONIC].actstart,D_FLOAT,FLOATUP},
	{"sonicactend",&weapinfos[WEAP_SONIC].actend,D_FLOAT,FLOATUP},
	{"sonicactloop",&weapinfos[WEAP_SONIC].actloop,D_FLOAT,FLOATUP},
//	{"sonicresstart",&weapinfos[WEAP_SONIC].resstart,D_FLOAT,FLOATUP},
	{"sonicresend",&weapinfos[WEAP_SONIC].resend,D_FLOAT,FLOATUP},
//	{"sonicresloop",&weapinfos[WEAP_SONIC].resloop,D_FLOAT,FLOATUP},
/*	{"------- THERMCAN -------",NULL,D_VOID,0},
	{"thermcanpersist",&weapinfos[WEAP_THERMCAN].persist,D_INT,1},
	{"thermcandeploystart",&weapinfos[WEAP_THERMCAN].deploystart,D_FLOAT,FLOATUP},
	{"thermcandeployend",&weapinfos[WEAP_THERMCAN].deployend,D_FLOAT,FLOATUP},
	{"thermcandeployloop",&weapinfos[WEAP_THERMCAN].deployloop,D_FLOAT,FLOATUP},
	{"thermcanactstart",&weapinfos[WEAP_THERMCAN].actstart,D_FLOAT,FLOATUP},
	{"thermcanactend",&weapinfos[WEAP_THERMCAN].actend,D_FLOAT,FLOATUP},
	{"thermcanactloop",&weapinfos[WEAP_THERMCAN].actloop,D_FLOAT,FLOATUP},
	{"thermcanresstart",&weapinfos[WEAP_THERMCAN].resstart,D_FLOAT,FLOATUP},
	{"thermcanresend",&weapinfos[WEAP_THERMCAN].resend,D_FLOAT,FLOATUP},
	{"thermcanresloop",&weapinfos[WEAP_THERMCAN].resloop,D_FLOAT,FLOATUP}, */

// 10 new weapons
	{"------- SLEDGE -------",NULL,D_VOID,0},
	{"sledgepersist",&weapinfos[WEAP_SLEDGE].persist,D_INT,1},
//	{"sledgedeploystart",&weapinfos[WEAP_SLEDGE].deploystart,D_FLOAT,FLOATUP},
	{"sledgedeployend",&weapinfos[WEAP_SLEDGE].deployend,D_FLOAT,FLOATUP},
//	{"sledgedeployloop",&weapinfos[WEAP_SLEDGE].deployloop,D_FLOAT,FLOATUP},
	{"sledgeactstart",&weapinfos[WEAP_SLEDGE].actstart,D_FLOAT,FLOATUP},
	{"sledgeactend",&weapinfos[WEAP_SLEDGE].actend,D_FLOAT,FLOATUP},
	{"sledgeactloop",&weapinfos[WEAP_SLEDGE].actloop,D_FLOAT,FLOATUP},
//	{"sledgeresstart",&weapinfos[WEAP_SLEDGE].resstart,D_FLOAT,FLOATUP},
	{"sledgeresend",&weapinfos[WEAP_SLEDGE].resend,D_FLOAT,FLOATUP},
//	{"sledgeresloop",&weapinfos[WEAP_SLEDGE].resloop,D_FLOAT,FLOATUP},
	{"------- ENERGYWALL -------",NULL,D_VOID,0},
	{"energywallpersist",&weapinfos[WEAP_ENERGYWALL].persist,D_INT,1},
//	{"energywalldeploystart",&weapinfos[WEAP_ENERGYWALL].deploystart,D_FLOAT,FLOATUP},
	{"energywalldeployend",&weapinfos[WEAP_ENERGYWALL].deployend,D_FLOAT,FLOATUP},
//	{"energywalldeployloop",&weapinfos[WEAP_ENERGYWALL].deployloop,D_FLOAT,FLOATUP},
	{"energywallactstart",&weapinfos[WEAP_ENERGYWALL].actstart,D_FLOAT,FLOATUP},
	{"energywallactend",&weapinfos[WEAP_ENERGYWALL].actend,D_FLOAT,FLOATUP},
	{"energywallactloop",&weapinfos[WEAP_ENERGYWALL].actloop,D_FLOAT,FLOATUP},
//	{"energywallresstart",&weapinfos[WEAP_ENERGYWALL].resstart,D_FLOAT,FLOATUP},
	{"energywallresend",&weapinfos[WEAP_ENERGYWALL].resend,D_FLOAT,FLOATUP},
//	{"energywallresloop",&weapinfos[WEAP_ENERGYWALL].resloop,D_FLOAT,FLOATUP},
	{"------- STASIS -------",NULL,D_VOID,0},
	{"stasispersist",&weapinfos[WEAP_STASIS].persist,D_INT,1},
//	{"stasisdeploystart",&weapinfos[WEAP_STASIS].deploystart,D_FLOAT,FLOATUP},
	{"stasisdeployend",&weapinfos[WEAP_STASIS].deployend,D_FLOAT,FLOATUP},
//	{"stasisdeployloop",&weapinfos[WEAP_STASIS].deployloop,D_FLOAT,FLOATUP},
	{"stasisactstart",&weapinfos[WEAP_STASIS].actstart,D_FLOAT,FLOATUP},
	{"stasisactend",&weapinfos[WEAP_STASIS].actend,D_FLOAT,FLOATUP},
	{"stasisactloop",&weapinfos[WEAP_STASIS].actloop,D_FLOAT,FLOATUP},
//	{"stasisresstart",&weapinfos[WEAP_STASIS].resstart,D_FLOAT,FLOATUP},
	{"stasisresend",&weapinfos[WEAP_STASIS].resend,D_FLOAT,FLOATUP},
//	{"stasisresloop",&weapinfos[WEAP_STASIS].resloop,D_FLOAT,FLOATUP},
	{"------- SUPAJUMP -------",NULL,D_VOID,0},
	{"supajumppersist",&weapinfos[WEAP_SUPAJUMP].persist,D_INT,1},
//	{"supajumpdeploystart",&weapinfos[WEAP_SUPAJUMP].deploystart,D_FLOAT,FLOATUP},
	{"supajumpdeployend",&weapinfos[WEAP_SUPAJUMP].deployend,D_FLOAT,FLOATUP},
//	{"supajumpdeployloop",&weapinfos[WEAP_SUPAJUMP].deployloop,D_FLOAT,FLOATUP},
	{"supajumpactstart",&weapinfos[WEAP_SUPAJUMP].actstart,D_FLOAT,FLOATUP},
	{"supajumpactend",&weapinfos[WEAP_SUPAJUMP].actend,D_FLOAT,FLOATUP},
	{"supajumpactloop",&weapinfos[WEAP_SUPAJUMP].actloop,D_FLOAT,FLOATUP},
//	{"supajumpresstart",&weapinfos[WEAP_SUPAJUMP].resstart,D_FLOAT,FLOATUP},
	{"supajumpresend",&weapinfos[WEAP_SUPAJUMP].resend,D_FLOAT,FLOATUP},
//	{"supajumpresloop",&weapinfos[WEAP_SUPAJUMP].resloop,D_FLOAT,FLOATUP},
	{"------- FORKLIFT -------",NULL,D_VOID,0},
	{"forkliftpersist",&weapinfos[WEAP_FORKLIFT].persist,D_INT,1},
//	{"forkliftdeploystart",&weapinfos[WEAP_FORKLIFT].deploystart,D_FLOAT,FLOATUP},
	{"forkliftdeployend",&weapinfos[WEAP_FORKLIFT].deployend,D_FLOAT,FLOATUP},
//	{"forkliftdeployloop",&weapinfos[WEAP_FORKLIFT].deployloop,D_FLOAT,FLOATUP},
	{"forkliftactstart",&weapinfos[WEAP_FORKLIFT].actstart,D_FLOAT,FLOATUP},
	{"forkliftactend",&weapinfos[WEAP_FORKLIFT].actend,D_FLOAT,FLOATUP},
	{"forkliftactloop",&weapinfos[WEAP_FORKLIFT].actloop,D_FLOAT,FLOATUP},
//	{"forkliftresstart",&weapinfos[WEAP_FORKLIFT].resstart,D_FLOAT,FLOATUP},
	{"forkliftresend",&weapinfos[WEAP_FORKLIFT].resend,D_FLOAT,FLOATUP},
//	{"forkliftresloop",&weapinfos[WEAP_FORKLIFT].resloop,D_FLOAT,FLOATUP},
	{"------- BUMPERCAR -------",NULL,D_VOID,0},
	{"bumpercarpersist",&weapinfos[WEAP_BUMPERCAR].persist,D_INT,1},
//	{"bumpercardeploystart",&weapinfos[WEAP_BUMPERCAR].deploystart,D_FLOAT,FLOATUP},
	{"bumpercardeployend",&weapinfos[WEAP_BUMPERCAR].deployend,D_FLOAT,FLOATUP},
//	{"bumpercardeployloop",&weapinfos[WEAP_BUMPERCAR].deployloop,D_FLOAT,FLOATUP},
	{"bumpercaractstart",&weapinfos[WEAP_BUMPERCAR].actstart,D_FLOAT,FLOATUP},
	{"bumpercaractend",&weapinfos[WEAP_BUMPERCAR].actend,D_FLOAT,FLOATUP},
	{"bumpercaractloop",&weapinfos[WEAP_BUMPERCAR].actloop,D_FLOAT,FLOATUP},
//	{"bumpercarresstart",&weapinfos[WEAP_BUMPERCAR].resstart,D_FLOAT,FLOATUP},
	{"bumpercarresend",&weapinfos[WEAP_BUMPERCAR].resend,D_FLOAT,FLOATUP},
//	{"bumpercarresloop",&weapinfos[WEAP_BUMPERCAR].resloop,D_FLOAT,FLOATUP},
	{"------- GYROSCOPE -------",NULL,D_VOID,0},
	{"gyroscopepersist",&weapinfos[WEAP_GYROSCOPE].persist,D_INT,1},
//	{"gyroscopedeploystart",&weapinfos[WEAP_GYROSCOPE].deploystart,D_FLOAT,FLOATUP},
	{"gyroscopedeployend",&weapinfos[WEAP_GYROSCOPE].deployend,D_FLOAT,FLOATUP},
//	{"gyroscopedeployloop",&weapinfos[WEAP_GYROSCOPE].deployloop,D_FLOAT,FLOATUP},
	{"gyroscopeactstart",&weapinfos[WEAP_GYROSCOPE].actstart,D_FLOAT,FLOATUP},
	{"gyroscopeactend",&weapinfos[WEAP_GYROSCOPE].actend,D_FLOAT,FLOATUP},
	{"gyroscopeactloop",&weapinfos[WEAP_GYROSCOPE].actloop,D_FLOAT,FLOATUP},
//	{"gyroscoperesstart",&weapinfos[WEAP_GYROSCOPE].resstart,D_FLOAT,FLOATUP},
	{"gyroscoperesend",&weapinfos[WEAP_GYROSCOPE].resend,D_FLOAT,FLOATUP},
//	{"gyroscoperesloop",&weapinfos[WEAP_GYROSCOPE].resloop,D_FLOAT,FLOATUP},
	{"------- GRAVITYBOMB -------",NULL,D_VOID,0},
	{"gravitybombpersist",&weapinfos[WEAP_GRAVITYBOMB].persist,D_INT,1},
//	{"gravitybombdeploystart",&weapinfos[WEAP_GRAVITYBOMB].deploystart,D_FLOAT,FLOATUP},
	{"gravitybombdeployend",&weapinfos[WEAP_GRAVITYBOMB].deployend,D_FLOAT,FLOATUP},
//	{"gravitybombdeployloop",&weapinfos[WEAP_GRAVITYBOMB].deployloop,D_FLOAT,FLOATUP},
	{"gravitybombactstart",&weapinfos[WEAP_GRAVITYBOMB].actstart,D_FLOAT,FLOATUP},
	{"gravitybombactend",&weapinfos[WEAP_GRAVITYBOMB].actend,D_FLOAT,FLOATUP},
	{"gravitybombactloop",&weapinfos[WEAP_GRAVITYBOMB].actloop,D_FLOAT,FLOATUP},
//	{"gravitybombresstart",&weapinfos[WEAP_GRAVITYBOMB].resstart,D_FLOAT,FLOATUP},
	{"gravitybombresend",&weapinfos[WEAP_GRAVITYBOMB].resend,D_FLOAT,FLOATUP},
//	{"gravitybombresloop",&weapinfos[WEAP_GRAVITYBOMB].resloop,D_FLOAT,FLOATUP},
	{"------- FLASHER -------",NULL,D_VOID,0},
	{"flasherpersist",&weapinfos[WEAP_FLASHER].persist,D_INT,1},
//	{"flasherdeploystart",&weapinfos[WEAP_FLASHER].deploystart,D_FLOAT,FLOATUP},
	{"flasherdeployend",&weapinfos[WEAP_FLASHER].deployend,D_FLOAT,FLOATUP},
//	{"flasherdeployloop",&weapinfos[WEAP_FLASHER].deployloop,D_FLOAT,FLOATUP},
	{"flasheractstart",&weapinfos[WEAP_FLASHER].actstart,D_FLOAT,FLOATUP},
	{"flasheractend",&weapinfos[WEAP_FLASHER].actend,D_FLOAT,FLOATUP},
	{"flasheractloop",&weapinfos[WEAP_FLASHER].actloop,D_FLOAT,FLOATUP},
//	{"flasherresstart",&weapinfos[WEAP_FLASHER].resstart,D_FLOAT,FLOATUP},
	{"flasherresend",&weapinfos[WEAP_FLASHER].resend,D_FLOAT,FLOATUP},
//	{"flasherresloop",&weapinfos[WEAP_FLASHER].resloop,D_FLOAT,FLOATUP},
	{"------- CHARGER -------",NULL,D_VOID,0},
	{"chargerpersist",&weapinfos[WEAP_CHARGER].persist,D_INT,1},
//	{"chargerdeploystart",&weapinfos[WEAP_CHARGER].deploystart,D_FLOAT,FLOATUP},
	{"chargerdeployend",&weapinfos[WEAP_CHARGER].deployend,D_FLOAT,FLOATUP},
//	{"chargerdeployloop",&weapinfos[WEAP_CHARGER].deployloop,D_FLOAT,FLOATUP},
	{"chargeractstart",&weapinfos[WEAP_CHARGER].actstart,D_FLOAT,FLOATUP},
	{"chargeractend",&weapinfos[WEAP_CHARGER].actend,D_FLOAT,FLOATUP},
	{"chargeractloop",&weapinfos[WEAP_CHARGER].actloop,D_FLOAT,FLOATUP},
//	{"chargerresstart",&weapinfos[WEAP_CHARGER].resstart,D_FLOAT,FLOATUP},
	{"chargerresend",&weapinfos[WEAP_CHARGER].resend,D_FLOAT,FLOATUP},
//	{"chargerresloop",&weapinfos[WEAP_CHARGER].resloop,D_FLOAT,FLOATUP},

	{"------- SWITCHAROO -------",NULL,D_VOID,0},
	{"switcharoopersist",&weapinfos[WEAP_SWITCHAROO].persist,D_INT,1},
//	{"switcharoodeploystart",&weapinfos[WEAP_SWITCHAROO].deploystart,D_FLOAT,FLOATUP},
	{"switcharoodeployend",&weapinfos[WEAP_SWITCHAROO].deployend,D_FLOAT,FLOATUP},
//	{"switcharoodeployloop",&weapinfos[WEAP_SWITCHAROO].deployloop,D_FLOAT,FLOATUP},
	{"switcharooactstart",&weapinfos[WEAP_SWITCHAROO].actstart,D_FLOAT,FLOATUP},
	{"switcharooactend",&weapinfos[WEAP_SWITCHAROO].actend,D_FLOAT,FLOATUP},
	{"switcharooactloop",&weapinfos[WEAP_SWITCHAROO].actloop,D_FLOAT,FLOATUP},
//	{"switcharooresstart",&weapinfos[WEAP_SWITCHAROO].resstart,D_FLOAT,FLOATUP},
	{"switcharooresend",&weapinfos[WEAP_SWITCHAROO].resend,D_FLOAT,FLOATUP},
//	{"switcharooresloop",&weapinfos[WEAP_SWITCHAROO].resloop,D_FLOAT,FLOATUP},
	{"------- MINIGUN -------",NULL,D_VOID,0},
	{"minigunpersist",&weapinfos[WEAP_MINIGUN].persist,D_INT,1},
//	{"minigundeploystart",&weapinfos[WEAP_MINIGUN].deploystart,D_FLOAT,FLOATUP},
	{"minigundeployend",&weapinfos[WEAP_MINIGUN].deployend,D_FLOAT,FLOATUP},
//	{"minigundeployloop",&weapinfos[WEAP_MINIGUN].deployloop,D_FLOAT,FLOATUP},
	{"minigunactstart",&weapinfos[WEAP_MINIGUN].actstart,D_FLOAT,FLOATUP},
	{"minigunactend",&weapinfos[WEAP_MINIGUN].actend,D_FLOAT,FLOATUP},
	{"minigunactloop",&weapinfos[WEAP_MINIGUN].actloop,D_FLOAT,FLOATUP},
//	{"minigunresstart",&weapinfos[WEAP_MINIGUN].resstart,D_FLOAT,FLOATUP},
	{"minigunresend",&weapinfos[WEAP_MINIGUN].resend,D_FLOAT,FLOATUP},
//	{"minigunresloop",&weapinfos[WEAP_MINIGUN].resloop,D_FLOAT,FLOATUP},
	{"------- PARTICLE -------",NULL,D_VOID,0},
	{"particlepersist",&weapinfos[WEAP_PARTICLE].persist,D_INT,1},
//	{"particledeploystart",&weapinfos[WEAP_PARTICLE].deploystart,D_FLOAT,FLOATUP},
	{"particledeployend",&weapinfos[WEAP_PARTICLE].deployend,D_FLOAT,FLOATUP},
//	{"particledeployloop",&weapinfos[WEAP_PARTICLE].deployloop,D_FLOAT,FLOATUP},
	{"particleactstart",&weapinfos[WEAP_PARTICLE].actstart,D_FLOAT,FLOATUP},
	{"particleactend",&weapinfos[WEAP_PARTICLE].actend,D_FLOAT,FLOATUP},
	{"particleactloop",&weapinfos[WEAP_PARTICLE].actloop,D_FLOAT,FLOATUP},
//	{"particleresstart",&weapinfos[WEAP_PARTICLE].resstart,D_FLOAT,FLOATUP},
	{"particleresend",&weapinfos[WEAP_PARTICLE].resend,D_FLOAT,FLOATUP},
//	{"particleresloop",&weapinfos[WEAP_PARTICLE].resloop,D_FLOAT,FLOATUP},

};
#define OL_NDEBVARS (sizeof(ol_debvars)/sizeof(ol_debvars[0]))

#ifdef USETESTNULLS
static char *ol_nullname="abuiltnull";
#else
static char *ol_nullname=NULL;
#endif

static char ol_burstnames[][32] =
{
 "burstb.txt",
 "burstg.txt",
 "burstr.txt",
 "bursty.txt"
};

static char ol_burstlwo[][32] =
{
 "burstb.lwo",
 "burstg.lwo",
 "burstr.lwo",
 "burst.lwo"
};

static char ol_burstmats[][32] =
{
 "burstb",
 "burstg",
 "burstr",
 "burst"
};


static char *placestrs[]={"1ST","2ND","3RD","4TH","5TH","6TH","7TH","8TH","9TH","10TH","11TH","12TH","13TH","14TH","15TH","16TH"};
struct ol_fs {
	int ang;
	float xoff,zoff;
};
static struct ol_fs ol_finishstruct[4]={
	{  0,1, 0},
	{ 90, 0,-1},
	{180,-1, 0},
	{270, 0, 1}
};
static struct ol_fs ol_crashresetstruct[4]={
	{  0, .5f, 0},
	{ 90, 0,-.5f},
	{180,-.5f, 0},
	{270, 0, .5f}
};
/*
static struct qmatfunc ol_matfuncs[]={
//	"dirtsurf",ol_spawndirtparts,1,0,
//	"Material #25",ol_spawndirtparts,1,0,
//	"loopside",usenewcam,
//	"loop",ol_usenewcam,0,0,
	"lava",ol_spawndirtparts2,1,0,
	"hw2pLava",ol_spawndirtparts2,1,0,
//	"hw2pLavaRockTrans",ol_spawndirtparts2,1,0,
	"WATER",ol_spawndirtparts1,0,0,
//	"lava",ol_dolava,0,0,
//	"WATER",ol_dowater,0,0,
};
#define OL_NMATFUNCS (sizeof(ol_matfuncs)/sizeof(ol_matfuncs[0]))
*/
static void ol_slowdown(),ol_speedup(),ol_spawndirtparts1(),ol_spawndirtparts2();
static struct smatfunc ol_matfuncs[]={
//	{"hitlava",ol_hitground},
//	{"hitwall",ol_hitground},
//	{"hitground",ol_hitground},
//	{"splash",ol_splash},
	{"slowdown",ol_slowdown},
	{"speedup",ol_speedup},
	{"water",ol_spawndirtparts1},
	{"lava",ol_spawndirtparts2},
};
#define OL_NMATFUNCS (sizeof(ol_matfuncs)/sizeof(ol_matfuncs[0]))

////////// finishbursts
#define BSND1 37 // 9/16 (common)
#define BSND2 30 // 3/16
#define BSND3 31 // 3/16
#define BSND4 32 // 1/16 (rare)
static int ol_finishburstsounds[16]={
	BSND2,BSND2,BSND2,BSND3,
	BSND3,BSND3,BSND4,BSND1,
	BSND1,BSND1,BSND1,BSND1,
	BSND1,BSND1,BSND1,BSND1,
};

static int ol_steershocks[6]={1,1,0,0,0,0};

//extern char prefsdir[MAX_PATH];
//extern char dswmediadir[MAX_PATH];



/*
struct weapinfo weapinfos[MAX_WEAPKIND]={
	{"laser",NULL,0},
	{"klondyke",NULL,0},
	{"missiles",NULL,0},
	{"oil",NULL,0},
	{"emb",NULL,0},
	{"ecto",NULL,0},
	{"dielectric",NULL,0,2},
	{"transducer",NULL,0,1},
	{"plasma",NULL,0,1000000},
	{"ramjet",NULL,0},
	{"intake",NULL,0},
	{"spikes",NULL,0},
	{"bainite",NULL,0},
	{"aerodyne",NULL,0},
	{"ferrite",NULL,0},
	{"weapboost",NULL,1},
	{"rpd",NULL,1},
	{"nano",NULL,1},
	{"randomizer",NULL,1},
	{"icrms",NULL,0},

	{"bigtires",NULL,0},
	{"buzz",NULL,0},
//	{"chemboost",0},
//	{"chemvac",0},
	{"enleech",NULL,0},
//	{"enuptake",0},
//	{"fsnboo",0},
	{"fsncan",NULL,0},
//	{"geoshield",0,1},
	{"grapple",NULL,0},
//	{"holo",0,1},
	{"magnet",NULL,0},
	{"mines",NULL,0},
//	{"nitro",0},
//	{"nrgcon",0},
//	{"poolboo",0},
	{"pouncer",NULL,0},
	{"prism",NULL,0,4},
//	{"shldbstr",1},
	{"sonic",NULL,0},
//	{"thermcan",0},

	{"sledge","bashing mallet",0},
	{"energywall","static-ion barrier wall",0},
	{"stasis",NULL,0},
	{"supajump",NULL,0},
	{"forklift",NULL,1},
	{"bumpercar","repulsion barrier",1},
	{"gyroscope","precise landing device",1},
	{"gravitybomb",NULL,0},
	{"flasher","photonic retinal attack",0},
	{"charger",NULL,0},
	{"switcharoo",NULL,0},
};
*/
char *weapvags[MAX_WEAPKIND]={
	"laser.vag",
	"klondyke.vag",
	"missiles.vag",
	"oil.vag",
	"emb.vag",
	"ecto.vag",
	"dielectric.vag",
	"transducer.vag",
	"plasma.vag",
	"ramjet.vag",
	"intake.vag",
	"spikes.vag",
	"bainite.vag",
	"aerodyne.vag",
	"ferrite.vag",
	"weapboost.vag",
	"rpd.vag",
	"nano.vag",
	"randomizer.vag",
	"icrms.vag",
	"bigtires.vag",
	"buzz.vag",
	"enleech.vag",
	"fsncan.vag",
	"grapple.vag",
	"magnet.vag",
	"mines.vag",
	"pouncer.vag",
	"prism.vag",
	"sonic.vag",
	"sledge.vag",
	"energywall.vag",
	"stasis.vag",
	"supajump.vag",
	"forklift.vag",
	"bumpercar.vag",
	"gyroscope.vag",
	"gravitybomb.vag",
	"flasher.vag",
	"charger.vag",
	"switcharoo.vag",
	"minigun.vag",
	"particle.vag",
};

// check enums in h file, DETACH_
static struct detachweapinfo detachweapinfos[MAXDETACHKIND]={
	{"laser"	,initlaser,proclaser},
	{"missiles"	,initmissiles,procmissiles},
	{"oil"		,initoil,procoil},
	{"emb"		,initemb,procemb},
	{"ecto"		,initecto,procecto}, // ecto2 removed..
	{"mine"		,initmine,procmine},
//JAY
	{"flag"		,initflag,procflag},
//NEW 20-40
	{"ghook"	,initghook,procghook},
	{"fusioncan",initfusioncan,procfusioncan},
	{"thermocan",initthermocan,procthermocan},
	{"sonic"	,initsonic,procsonic},

	{"gbomb"	,initgbomb,procgbomb},
	{"stasis"	,initstasis,procstasis},
	{"wall"		,initwall,procwall},
	{"ramp"		,initramp,procramp},
	{"minigun"	,initlaser,proclaser},
	{"particle"	,initlaser,proclaser},
};

/*static void (*wkproclist[MAXDETACHKIND])(struct weapfly *wf)={
	proclaser,
	procmissiles,
	procoil,
	procemb,
	procecto,
//	NULL,	// never spawn ecto2
	procmine,
//JAY
	procflag,
//new 20-40
	procghook,
	procfusioncan,
	procthermocan,
	procsonic,
};

static void (*wkinitlist[MAXDETACHKIND])(struct weapfly *wf)={
	initlaser,
	initmissiles,
	initoil,
	initemb,
	initecto,
//	NULL,
	initmine,
//JAY	procflag,
	initflag,
// new 20-40
	initghook,
	initfusioncan,
	initthermocan,
	initsonic,
};
*/
/*char *regpointnames[]={
	"back",
	"tirebr",
	"bottom",
	"tirebl",
	"tirefr",
	"tirefl",
	"front",
	"hood",
	"right",
	"roof",
	"left",
	"trunk",
};
*/
static char *shinyregpointnames[]={
	"_bk.bwo",
	"_br.bwo",
	"_bm.bwo",
	"_bl.bwo",
	"_fr.bwo",
	"_fl.bwo",
	"_ft.bwo",
	"_hd.bwo",
	"_rt.bwo",
	"_rf.bwo",
	"_lt.bwo",
	"_tk.bwo",
};

//static tree2 *detachweaps[MAXDETACHKIND];
/*int weapalwaysfacing[MAX_WEAPKIND]={
	0, // laser
	0, // klondyke
	0, // missiles
	0, // oil
	0, // emb
	0, // ecto
	0, // dielectric
	0, // transducer
	0, // plasma
	0, // ramjet
	0, // intake
	0, // spikes
	0, // bainite
	0, // aerodyne
	0, // ferrite
	0, // weapboost
	0, // rpd
	0, // nano
	0, // randomizer
	0, // icrms
};
*/

//static char winmain_versionvs[]="2 PLAYER STUNT 9.28"; // main version defined in states.c

int prandom1(int clock,int poss)
{
	int ret;
	ret=clock;
	ret%=poss;
	return ret;
}

int prandom2(int clock,int poss)
{
	int ret;
	ret=clock>>4;
	ret%=poss;
	return ret;
}

int prandom3(int clock,int poss)
{
	int ret;
	ret=clock>>8;
	ret%=poss;
	return ret;
}

/*float pfrand1(int clock)
{
	float fret;

	int ret,ret2;
	ret=1234;
	ret+=clock;
	ret2=ret;
	ret=((ret<<15)+(ret<<7))^ret;
	ret*=ret2;
	ret&=0xfffff;

	fret=ret*(1.0f/1048576.0f);
	return fret;
}
*/
float pfrand1(int clock)
{
	float fret;

	int ret,ret2;
	ret=1234;
	ret+=clock;
	ret2=ret;
	ret=((ret<<15)+(ret<<7))^ret;
	ret*=ret2;
	ret&=0xfffff;

	fret=ret*(1.0f/1048576.0f);
	return fret;
}

float pfrand2(int clock)
{
	float fret;

	int ret,ret2;
	ret=5679;
	ret+=clock;
	ret2=ret;
	ret=((ret<<15)+(ret<<7))^ret;
	ret*=ret2;
	ret&=0xfffff;

	fret=ret*(1.0f/1048576.0f);
	return fret;
}

float pfrand3(int clock)
{
	float fret;

	int ret,ret2;
	ret=8087;
	ret+=clock;
	ret2=ret;
	ret=((ret<<15)+(ret<<7))^ret;
	ret*=ret2;
	ret&=0xfffff;

	fret=ret*(1.0f/1048576.0f);
	return fret;
}

/////////////////// connection state , establish all connections before racing.. //////
// this function might disappear soon
static void copyglobalinfo(const struct onlineopt *oo,struct ol_data *gd)
{
//	gd->ntrackscale=oo->trackscale;
//	gd->trackstart=oo->trackstart;
//	gd->trackend=oo->trackend;
	gd->numplayers=oo->nplayers;
	gd->numcars=oo->ncars;
	gd->numbots=oo->nbots;
//	strcpy(gd->ol_uedittrackname,oo->trackname);
	strcpy(gd->ntrackname,oo->ntrackname);
//	strcpy(gd->ol_skyboxname,oo->skyboxname);
//	return oo->curslot; // sets 'of' later
	memcpy(gd->medalshave,oo->medalsearned,sizeof(oo->medalsearned));
}

/*
struct playeropt {
//	char playername[NAMESIZEI];
	int carnum;
	int cntl;
	int pnum;
};
struct conninfo {
	int ip;
	int port;
	int ready;
};
struct onlineopt {
	char trackname[NAMESIZEI]; // if blank use constructed track
	int numplayers; // number of car slots
	int curslot;	// camera follows this car..
	int playerid;
	struct playeropt popt[OL_MAXCARSLOTS]; // car slots
	struct conninfo conninfos[OL_MAXPLAYERS];	// player slots
	float trackscale;
	int backtoopt; // set to a 1 when main game should return to online opts
*/
static char *botnames[16]={
	"0:SHIFTER",
	"1:LANDSLIDE",
	"2:JR",
	"3:POWER",
	"4:DARCI",
	"5:BILL",
	"6:QUANTUM",
	"7:TIM",
	"8:SHIFTER2",
	"9:LANDSLIDE2",
	"10:JR2",
	"11:POWER2",
	"12:DARCI2",
	"13:BILL2",
	"14:QUANTUM2",
	"15:TIM2",
};

#if 0
void seltrack_loadstubinfo()
{
//	int i;
	FILE *fw;
//	int serverip;
//	int c0p,c1p;
//stubinfo.pname
//stubinfo.p1name
//stubinfo.p1ip
//stubinfo.p2ip
//stubinfo.port
//	p0ip=stubinfo.pips[0];
//	p1ip=stubinfo.pips[1];
	if (wininfo.isalreadyrunning)
		return;
	pushandsetdirdown("downonlineopt");
	fw=fopen2("online_seltrackcfg.txt","w");
	popdir();
	if (!fw)
		return;
	if (mainstubinfo.tracknum>0) {
		logger("stub track num found and is a %d\n",mainstubinfo.tracknum);
		olracecfg.stubtracknum=mainstubinfo.tracknum;
	}
//RULE
	if (mainstubinfo.rules>0) {
		logger("stub rules found and is a %d\n",mainstubinfo.rules);
		olracecfg.rules=mainstubinfo.rules;
	}
	fprintf(fw,"onlinetracknum %d\n",olracecfg.stubtracknum);
//RULE
	fprintf(fw,"rules %d\n",olracecfg.rules);
	fprintf(fw,"numplayers %d\n",mainstubinfo.nplayers);
	fprintf(fw,"numcars %d\n",mainstubinfo.ncars);
//	fprintf(fw,"curslot 0\n");
//	fprintf(fw,"pnum 0\n");
	if (mainstubinfo.nplayers>1) { // server
		fprintf(fw,"serverip %s\n",ip2str(mainstubinfo.serverip));
		fprintf(fw,"port %d\n",mainstubinfo.port);
	} else if (mainstubinfo.nplayers==1) { // bots
	} else { // -1 client
		fprintf(fw,"serverip %s\n",ip2str(mainstubinfo.serverip));
		fprintf(fw,"port %d\n",mainstubinfo.port);
	}
/*		for (i=0;i<stubinfo.nplayers;i++)
			fprintf(fw,"slot 0 car %d cntl 0 player %d\n",6+i,i);
//		if (stubinfo.playerid==0) { // server
			for (i=0;i<stubinfo.ncomputers;i++)
				fprintf(fw,"ips %d tcp ip %s port %d\n",i,ip2str(stubinfo.pips[0]),stubinfo.port);
//		} else { // client
//		}
//		fprintf(fw,"ips 1 tcp ip %s port %d\n",ip2str(p1ip),stubinfo.port);

//		if (!strcmp(stubinfo.pname,stubinfo.p1name)) // player 0 only
//			strcpy(stubinfo.p2name,"------");
//		else if (!strcmp(stubinfo.pname,stubinfo.p2name)) // player 1 only
//			strcpy(stubinfo.p1name,"------");
	} else {
		fprintf(fw,"slot 0 car 6 cntl 0 player 0\n");
		for (i=1;i<stubinfo.nplayers;i++)
			fprintf(fw,"slot %d car %d cntl 1 player 0\n",i,6+i);
	} */
	fprintf(fw,"carname \"%s\"\n",mainstubinfo.names.name);
	fprintf(fw,"carbody \"%s\"\n",mainstubinfo.names.body);
	fprintf(fw,"carpaint \"%s\"\n",mainstubinfo.names.paint);
	fprintf(fw,"cardecal \"%s\"\n",mainstubinfo.names.decal);
	fprintf(fw,"altcarname \"%s\"\n",botnames[mt_random(8)]);
	fprintf(fw,"altcarbody \"%s\"\n",botinfo.botbodys[0]);
	fprintf(fw,"altcarpaint \"%s\"\n",botinfo.botpaints[0]);
	fprintf(fw,"altcardecal \"%s\"\n",botinfo.botdecals[0]);
	fclose(fw);
}
#endif
static int findcurslot(int pid)
{
	int i;
	for (i=0;i<od.numplayers;i++)
		if (opa[i].playerid==pid && opa[i].cntl==CNTL_HUMAN)
			return i;
	for (i=0;i<od.numplayers;i++)
		if (opa[i].playerid==pid)
			return i;
	return 0;
}

// send gameinfo back to matching server
static void gameinfoupdate()
{
	if (matchcl && od.master) { // if here from matchlobby and a host, then send a gamepicked token to matching server
// gamepicked hostname gamename curplayers ip port
		const C8* ipstr=ip2str(cgi.ip);
		C8 token[300];
		const C8* gn=getgamename(cgi.gameidx);
		sprintf(token,"gamepicked %s %d/%d %s %d",gn,od.numcurplayers,ol_opt.nplayers,ipstr,od.master->port);
		U32 len=strlen(token);
		tcpwritetoken32(matchcl,0,token,len);
		tcpsendwritebuff(matchcl);
	}
}

static void gameinfoclear()
{
	if (matchcl && od.master) {
// gamepicked
		char* gp="gamepicked";
		tcpwritetoken32(matchcl,0,gp,strlen(gp));
		tcpsendwritebuff(matchcl);
	}
}

static void disconnectandai() // needs alittle work
{
	if (ol_opt.nt==NET_BCLIENT) {
		ol_opt.nt=NET_BOT;
		S32 i;
		od.numcurplayers=1;
		for (i=0;i<od.numcars-od.numbots;++i) {
			if (opa[i].playerid!=od.playerid) {
				od.playernet[i].onebotplayer=1;
				++od.numcurplayers;
			}
		}
		return;
	}
	int j;
	logger("in disconnectandai\n");
	if (od.numplayers==1)
		return;
	if (od.numplayers!=1) {
		for (j=0;j<OL_MAXPLAYERS;j++) {
			if (od.playernet[j].tcpclients) {
				freesocker(od.playernet[j].tcpclients);
				od.playernet[j].tcpclients=NULL;
			}
			od.playernet[j].onebotplayer=0;
//			if (od.playernet[j].alttcpclients) {
//				freesocker(od.playernet[j].alttcpclients);
//				od.playernet[j].alttcpclients=NULL;
//			}
		}
		gameinfoclear(); // tell match server
		if (od.master) {
			freesocker(od.master);
			od.master=NULL;
		}
//		if (od.altmaster) {
//			freesocker(od.altmaster);
//			od.altmaster=NULL;
//		}
		if (!matchcl)
			uninitsocker();
	}
//	if (od.playerid!=0)
//		od.playerid=1;
//	opa[0].cntl=CNTL_HUMAN;
	for (j=0;j<od.numcars;j++) {
		if (od.playerid!=opa[j].playerid) {
//			ol_opt.popt[j].cntl=0;
//			opa[j].cntl=0;
//		} else {
//			ol_opt.popt[j].cntl=CNTL_AI;
//			if (opa[j].cntl!=CNTL_AI) {
/*				if (botinfo.botbodys[0][0] && carstubinfos[j].c_body[0]) {
					strcpy(carstubinfos[j].c_body,botinfo.botbodys[j%3]);
					strcpy(carstubinfos[j].c_decal,botinfo.botdecals[j%3]);
					strcpy(carstubinfos[j].c_paint,botinfo.botpaints[j%3]);
				} */
/*				while(1) {
					if (j>=8) {
						sprintf(carstubinfos[i].name,"PLAYER%d",j);
						break;
					}
					strcpy(carstubinfos[j].name,botnames[mt_random(8)]);
					for (i=0;i<j;i++)
						if (i!=j && !strcmp(carstubinfos[j].name,carstubinfos[i].name))
							break;
					if (i==j)
						break;
				} */
//			}
			opa[j].cntl=od.defaultcntl; //CNTL_AI;
		}
//		ol_opt.popt[j].pnum=0;
		opa[j].playerid=0;
	}
	od.numplayers=1;
	od.numdiscon=0; // bug fix 9-6-02, (setuprematch was called erroneously
	od.numbots=0;
	od.numcurplayers=1;
	od.playerid=0;
//	od.carid=;
//	of=&opa[od.carid];
//	of=&opa[findcurslot(0)]; // find first car that has p0
//	od.nobotrematch=1;
}

// check for consistency between the different sources
#if 0
static void mergeenergymatrix(int *oneweapenergies,int weapidx,int frmbits)
{
	int i;
	int oldbits=olracecfg.energymatrixused[weapidx];
	olracecfg.energymatrixused[weapidx]|=frmbits;
	if (!oldbits) {
		memcpy(olracecfg.energymatrix[weapidx],oneweapenergies,sizeof(int)*MAXENERGIES);
	} else
		for (i=0;i<MAXENERGIES;i++)
			if (olracecfg.energymatrix[weapidx][i]!=oneweapenergies[i])
				errorexit("problem merging weapon energy matrix, weapon '%s' energy '%s'",
				weapinfos[weapidx].name,energynames[i]);
}

static void checkweapsum(char *name,char *str)
{
	char *csstr;
	int cs=0,cs2;
	if (mainstubinfo.random1==0)
		return;
	if (olracecfg.noweapsum)
		return;
	csstr=fileload(name);
	if (!csstr)
		errorexit("bad weapsum 1");
	cs2=atoi(csstr);
//	memfree(csstr);
	delete[] csstr;
	while(*str)
		cs+=*str++;
	cs/=mainstubinfo.random1;
	cs/=mainstubinfo.random2;
	if (cs!=cs2)
		errorexit("bad weapsum 2");
}
#endif
static void loadweaps()
{
//	char *str;//,**toks;
//	int oneweapenergies[MAXENERGIES];
	int ntok,i,j,k;
	script* toks;
//	pushandsetdir("everything");
//	popdir();
	mainstubinfo.names.cd.c_nweap=0;
	botinfos[0].c_nweap=0;
//	if (olracecfg.scorelinekind==0)
//		return;
//pushandsetdirdown(prefsdir);
// load your weapons
//	str=NULL;
//	str=fileload("caweapons.txt");
//	checkweapsum("weaponschecksum.txt",str);
//	toks=tokscript(str,&ntok,"\"");
//	memfree(str);
	if (wininfo.isalreadyrunning && fileexist("altcaweapons.txt"))
		toks = new script("altcaweapons.txt");
	else
		toks = new script("caweapons.txt");
	ntok=toks->num();
	for (i=0;i<ntok;i++) {
/*		for (j=0;j<MAX_WEAPKIND;j++)
			if (!strcmp(weapinfos[j].name,toks->idx(i).c_str()))
				if (mainstubinfo.names.c_nweap<MAXWEAPONSCARRY) {
					for (k=0;k<mainstubinfo.names.c_nweap;k++)
						if (mainstubinfo.names.c_weaps[k]==j)
							break;
					if (k==mainstubinfo.names.c_nweap)
						mainstubinfo.names.c_weaps[mainstubinfo.names.c_nweap++]=j;
				} */
		j=atoi(toks->idx(i).c_str());
		mainstubinfo.names.cd.c_weaps[mainstubinfo.names.cd.c_nweap++]=j;
	}
//	freescript(toks,ntok);
	delete toks;
	patch_weapons();	// use globalgame if non null
	logger("main weapons\n");
	for (i=0;i<mainstubinfo.names.cd.c_nweap;i++) {
		logger("     %2d %3d: ",i,mainstubinfo.names.cd.c_weaps[i]);
		logger(weapinfos[mainstubinfo.names.cd.c_weaps[i]].name);
		logger("\n");
	}
// load bot weapons
//	str=NULL;
	if (fileexist("altcaweapons.txt"))
//		str=fileload("altcaweapons.txt");
		toks=new script("altcaweapons.txt");
	else
//		str=fileload("caweapons.txt");
		toks=new script("caweapons.txt");
	ntok=toks->num();
//	if (str) {
//		toks=tokscript(str,&ntok,"\"");
//		memfree(str);
	for (i=0;i<ntok;i++) {
		j=atoi(toks->idx(i).c_str());
		botinfos[0].c_weaps[botinfos[0].c_nweap++]=j;//.botweaps[0][botinfo.nbotweap[0]++]=j;
	}
/*			for (j=0;j<MAX_WEAPKIND;j++)
				if (!strcmp(weapinfos[j].name,toks->idx(i).c_str()))
					if (botinfo.nbotweap[0]<MAXWEAPONSCARRY) {
						for (k=0;k<botinfo.nbotweap[0];k++)
							if (botinfo.botweaps[0][k]==j)
								break;
						if (k==botinfo.nbotweap[0])
							botinfo.botweaps[0][botinfo.nbotweap[0]++]=j;
					}
//		freescript(toks,ntok); */
	delete toks;
//	}
	logger("alt weapons\n");
	for (i=0;i<botinfos[0].c_nweap;++i) {
		logger("     %2d %3d: ",i,botinfos[0].c_weaps[i]);
		logger(weapinfos[botinfos[0].c_weaps[i]].name);
		logger("\n");
	}
// load your energies
//[#electrical: 90, #chemical: 80, #fusion: 70, #cyber: 60, #geothermal: 50, #eparticle: 40]
//	str=NULL;
//	str=fileload("caenergy.txt");
//	checkweapsum("energychecksum.txt",str);
//	toks=tokscript(str,&ntok,"[#: \t,]");
//	memfree(str);
	if (wininfo.isalreadyrunning && fileexist("altcaenergy.txt"))
		toks = new script("altcaenergy.txt");
	else
		toks = new script("caenergy.txt");
	ntok=toks->num();
/*	for (i=0;i<ntok;i+=2)
		for (j=0;j<MAXENERGIES;j++)
			if (!strcmp(energynames[j],toks->idx(i).c_str()))
				mainstubinfo.names.c_energies[j]=range(0,atoi(toks->idx(i+1).c_str()),100); */
	for (i=0;i<ntok;++i)
		mainstubinfo.names.cd.c_energies[i]=range(0,atoi(toks->idx(i).c_str()),200);
//	freescript(toks,ntok);
	delete toks;
	patch_energies();
	logger("main energies\n");
	for (i=0;i<MAXENERGIES;i++)
		logger("     %2d: %-12s %3d\n",i,energynames[i],mainstubinfo.names.cd.c_energies[i]);
// load bot energies
//	str=NULL;
	if (fileexist("altcaenergy.txt"))
//		str=fileload("altcaenergy.txt");
		toks=new script("altcaenergy.txt");
	else
//		str=fileload("caenergy.txt");
		toks=new script("caenergy.txt");
	ntok=toks->num();
//	if (str) {
//		toks=tokscript(str,&ntok,"[#: \t,]");
//		memfree(str);
/*		for (i=0;i<ntok;i+=2)
			for (j=0;j<MAXENERGIES;j++)
				if (!strcmp(energynames[j],toks->idx(i).c_str()))
					botinfo.botenergies[0][j]=range(0,atoi(toks->idx(i+1).c_str()),100); */
	for (i=0;i<ntok;++i)
		botinfos[0].c_energies[i]=range(0,atoi(toks->idx(i).c_str()),200);

//		freescript(toks,ntok);
		delete toks;
//	}
	logger("bot energies\n");
	for (i=0;i<MAXENERGIES;i++)
		logger("     %2d: %-12s %3d\n",i,energynames[i],botinfos[0].c_energies[i]);
// load main energy matrix
//["[#ignore: 0, #WeaponName: "nano", #Energies: "[#electrical: "0", #chemical: "0", #fusion: "0", #cyber: "1", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "dielectric", #Energies: "[#electrical: "2", #chemical: "0", #fusion: "1", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "icrms", #Energies: "[#electrical: "0", #chemical: "0", #fusion: "0", #cyber: "3", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "randomizer", #Energies: "[#electrical: "0", #chemical: "0", #fusion: "0", #cyber: "1", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "transducer", #Energies: "[#electrical: "1", #chemical: "0", #fusion: "0", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "ramjet", #Energies: "[#electrical: "1", #chemical: "0", #fusion: "0", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "aerodyne", #Energies: "[#electrical: "1", #chemical: "2", #fusion: "0", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "ferrite", #Energies: "[#electrical: "1", #chemical: "0", #fusion: "0", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "spikes", #Energies: "[#electrical: "2", #chemical: "0", #fusion: "0", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "klondyke", #Energies: "[#electrical: "1", #chemical: "0", #fusion: "1", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "emb", #Energies: "[#electrical: "1", #chemical: "0", #fusion: "0", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]",
//	"[#ignore: 0, #WeaponName: "missiles", #Energies: "[#electrical: "1", #chemical: "0", #fusion: "1", #cyber: "0", #geothermal: "0", #eparticle: "0"]"]"]
//	str=NULL;
//	memset(oneweapenergies,0,sizeof(oneweapenergies));
//	str=fileload("caweaponsattrib.txt");
//	checkweapsum("weaponsattribchecksum.txt",str);
#if 0
	toks = new script("caweaponsattrib.txt");
//	if (str) {
//		int k=-1;
		k=-1;
		int idx=0;
//		toks=tokscript(str,&ntok,"[\"#: \t,]");
//		memfree(str);
		for (i=0;i<ntok;i++)
			if (k==-1) {
				for (j=0;j<MAX_WEAPKIND;j++)
					if (!strcmp(weapinfos[j].name,toks->idx(i).c_str())) {
						idx=j;
						k=0;
						break;
					}
			} else {
				for (j=0;j<MAXENERGIES;j++)
					if (!strcmp(energynames[j],toks->idx(i).c_str())) {
						i++;
						oneweapenergies[j]=atoi(toks->idx(i).c_str());
//						od.energymatrix[idx][j]=atoi(toks->idx(i).c_str());
						k++;
						if (k==MAXENERGIES) {
							mergeenergymatrix(oneweapenergies,idx,1);
//							od.energymatrixused[idx]|=1;
							k=-1;
						}
						break;
					}
			}
//		freescript(toks,ntok);
		delete toks;
//	}
// load bot energy matrix
//	str=NULL;
//	if (fileexist("altcaweaponsattrib.txt"))
//		str=fileload("altcaweaponsattrib.txt");
	toks=new script("altcaweaponsattrib.txt");
	ntok=toks->num();
//	if (str) {
//		int k=-1;
		k=-1;
//		int idx;
//		toks=tokscript(str,&ntok,"[\"#: \t,]");
//		memfree(str);
		for (i=0;i<ntok;i++)
			if (k==-1) {
				for (j=0;j<MAX_WEAPKIND;j++)
					if (!strcmp(weapinfos[j].name,toks->idx(i).c_str())) {
						idx=j;
						k=0;
						break;
					}
			} else {
				for (j=0;j<MAXENERGIES;j++)
					if (!strcmp(energynames[j],toks->idx(i).c_str())) {
						i++;
						oneweapenergies[j]=atoi(toks->idx(i).c_str());
//						od.energymatrix[idx][j]=atoi(toks->idx(i).c_str());
						k++;
						if (k==MAXENERGIES) {
							mergeenergymatrix(oneweapenergies,idx,2);
//							od.energymatrixused[idx]|=2;
							k=-1;
						}
						break;
					}
			}
//		freescript(toks,ntok);
		delete toks;
//	}
#endif
pushandsetdirdown("onlineopt");
	script sc("energymatrix.txt");
	if (sc.num()!=MAX_WEAPKIND*MAXENERGIES)
		errorexit("bad energymatrix.txt");
	for (j=0,k=0;j<MAX_WEAPKIND;++j)
		for (i=0;i<MAXENERGIES;++i,++k)
			olracecfg.energymatrix[j][i]=atoi(sc.idx(k).c_str());
popdir();
logger("energy matrix\n");
	logger("                    ");	// 20 spaces
	for (i=0;i<MAXENERGIES;i++)
		logger("%12s",energynames[i]);
	logger("\n");
	for (j=0;j<MAX_WEAPKIND;j++) {
//		if (olracecfg.energymatrixused[j]) {
			logger("%3d  %-12s",j,weapinfos[j].name);
			for (i=0;i<MAXENERGIES;i++)
				logger("%12d",olracecfg.energymatrix[j][i]);
			logger("\n");
//		}
	}
//popdir();
}

static void freeweaps()
{
/*	if (olracecfg.allweaps) {
		freescript(olracecfg.allweaps,olracecfg.nallweap);
		olracecfg.allweaps=NULL;
		olracecfg.nallweap=0;
	} */
	int i;
	for (i=0;i<MAXDETACHKIND;i++) {
		if (detachweapinfos[i].mastertree) {
			logger("freeing weapon '%s'\n",detachweapinfos[i].mastertree->name.c_str());
//			freetree(detachweapinfos[i].mastertree);
			delete detachweapinfos[i].mastertree;
		}
		if (detachweapinfos[i].facenorms) {
			logger("\tfreeing detachweapinfo facenorms\n");
//			memfree(detachweapinfos[i].facenorms);
			delete[] detachweapinfos[i].facenorms;
		}
		detachweapinfos[i].facenorms=NULL;
		detachweapinfos[i].mastertree=NULL;
	}
}

/*
static void updateweapdisplay()
{
	int i;
	int hicol;
	struct customstub *ci=&carstubinfos[of->carid];

	cliprect32(od.weapbm,0,0,od.weapbm->x,od.weapbm->y,hiblue);
	if (ci->nweap) {
		for (i=0;i<ci->nweap;i++) {
			if (of->vweapstolen[i])
				outtextxyf32(od.weapbm,0,i*8,hiyellow,"stolen");
			else if (of->curweap==i)
				outtextxyf32(od.weapbm,0,i*8,hiyellow,"%3.0f:%s",of->weapframe,weapinfos[ci->weaps[i]].name);
			else {
				if (ci->weaps[i]==WEAP_WEAPBOOST && of->weapbooston ||
					ci->weaps[i]==WEAP_RANDOMIZER && of->randomizeron)
					hicol=hired;
				else
					hicol=C32WHITE;
				outtextxyf32(od.weapbm,0,i*8,hicol,"F%.2d:%s",i+1,weapinfos[ci->weaps[i]].name);
			}
		}
		if (of->curweap==MAXWEAPONSCARRY)
			outtextxyf32(od.weapbm,0,i*8,higreen,"%3.0f:%s",of->weapframe,weapinfos[ci->weaps[MAXWEAPONSCARRY]].name);
	} else
		outtextxyf32(od.weapbm,0,0,C32WHITE,"no weapons");
}
*/
static int checkenergy(unsigned char eng[],int wk)
{
	int i;
	int *mx=olracecfg.energymatrix[wk];
	for (i=0;i<MAXENERGIES;i++)
		if (eng[i]<mx[i])
			return 0;
	return 1;
}

static void useenergy(unsigned char eng[]/*,unsigned char leng[]*/,int wk)
{
	int i;
	int *mx=olracecfg.energymatrix[wk];
	for (i=0;i<MAXENERGIES;i++) {
		eng[i]-=mx[i];
//		if (wk!=WEAP_ENUPTAKE)
//			leng[i]=mx[i];
	}
}

static void updateweapdisplay()
{
	int i;
	char str[100],wb[8];
	struct customstub *ci=&carstubinfos[of->carid];
	wb[0]='\0';
	if (of->tweapbooston)
		strcat(wb,"WB:");
	if (of->tshieldbooston)
		strcat(wb,"SB:");
	if (ci->cd.c_nweap) {
		char *wn;
		if (weapinfos[of->xcurweapkind].longname)
			wn=weapinfos[of->xcurweapkind].longname;
		else
			wn=weapinfos[of->xcurweapkind].name;
		od.curweapstring[0]='\0';
		if (of->xcurweapslot!=MAXWEAPONSCARRY && of->vvweapstolen[of->xcurweapslot])
			strcpy(od.curweapstring,"STOLEN");
		else if (of->xcurweapslot==MAXWEAPONSCARRY ) {
			sprintf(od.curweapstring,"%s%3.0f RPD %s",wb,of->xweapframe,wn);
			my_strupr(od.curweapstring);
		} else if (of->xcurweapstate==WEAPSTATE_ACTIVE) {
			sprintf(str,"%sACT %3.0f",wb,of->xweapframe);
			strcpy(od.curweapstring,str);
		} else if (of->xcurweapstate==WEAPSTATE_RESULT) {
			sprintf(str,"%sRES %3.0f",wb,of->xweapframe);
			strcpy(od.curweapstring,str);
//		} else if (of->xcurweapkind==WEAP_WEAPBOOST) {
//			if (of->tweapbooston)
//				strcpy(od.curweapstring,"ENABLED");
//			else
//				strcpy(od.curweapstring,"DISABLED");
/*		} else if (of->xcurweapkind==WEAP_RANDOMIZER) {
			if (of->randomizeron)
				strcpy(od.curweapstring,"ENABLED");
//			else
//				strcpy(od.curweapstring,"DISABLED"); */
		}
/*		if ((of->xcurweapkind==WEAP_RPD) && (of->curweapvar==1)) {
			sprintf(str,"RPD: %3.0f %s",of->xweapframe,weapinfos[ci->weaps[MAXWEAPONSCARRY]]);
			my_strupr(str);
			strcpy(od.curweapstring,str);
		} */
		if (!od.curweapstring[0]) {
			if (of->xcurweapslot==MAXWEAPONSCARRY || checkenergy(of->tenergies,of->xcurweapkind))
				sprintf(str,"%s%3.0f F%d %s",wb,of->xweapframe,of->xcurweapslot+1,wn);
			else
				sprintf(str,"EMPTY");
			my_strupr(str);
			strcpy(od.curweapstring,str);
		}
		od.curweapicon=of->xcurweapkind;
		if (ci->cd.c_nweap==1)
			od.prevweapicon=od.nextweapicon=-1;
		else {
			i=of->xcurweapslot-1;
			if (i<0)
				i=ci->cd.c_nweap-1;
			od.prevweapicon=ci->cd.c_weaps[i];
			i=of->xcurweapslot+1;
			if (i>=ci->cd.c_nweap)
				i=0;
			od.nextweapicon=ci->cd.c_weaps[i];
		}
	} else {
		strcpy(od.curweapstring,"NONE");
		od.curweapicon=od.prevweapicon=od.nextweapicon=-1;
	}
}

/*void drawweapdisplay()
{
	struct customstub *ci=&carstubinfos[of->carid];
	int i;//,k;
	if (ci->nweap>0)
		i=ci->nweap*8;
	else
		i=8;
	if (of->curweap==MAXWEAPONSCARRY)
		i+=8;
	clipblit32(od.weapbm,B32,0,0,0,0,od.weapbm->x,i);
//	if (of->curweap>=0) {
//		k=carstubinfos[of->carid].weaps[of->curweap];
//		clipxpar16(od.weapicons[k],B32,0,0,od.weapbm->x+od.jplacebm[0]->x,0,32,32);
//	}
} */

/*#define MAXTRACKS 16
static int trackca[MAXTRACKS]={
	0, // constructor track, not used anymore
	1, // combat arena, weapons enabled
	0, // old 2p stunt track
	0, // new 2p stunt track
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
}; */

fontq* onlineracefont;

void onlinerace_init()
{
pushandsetdir("racenetdata");
pushandsetdirdown("onlineopt");
	loadconfigfile("game.cfg",gamecfg_menuvar,GAMECFG_NMENUVAR);
popdir();
	int i;
	char *s;
//	struct bitmap32 *b1,*b2;
//	extern int wantedmaindriver;
//	extern char winmain_version[];
//	char **sc=NULL;
//	int nsc=0;
//	randomize();
	mt_setseed(GetTickCount());
	olracecfg.timeout=olracecfg.beforeloadtimeoutclient*30;
//	if (mainstubinfo.usestub)
//		seltrack_loadstubinfo(); // get info using stub system
	seltrack_loadconfig(); // get info from seltrack.c
	olracecfg.rules=range(1,olracecfg.rules,MAX_RULES-1);


//	olracecfg.scorelinekind=olracecfg.stubtracknum&1; // odd numbered tracks get weapons
//RULE
	currule=&rules[olracecfg.rules];

//	if (olracecfg.scorelinekind==0)
//		setwindowtext(winmain_versionvs);
//	else
//		setwindowtext(winmain_version);
//	setwindowtext(currule->windowname);
	loadweaps(); // just loads the names, not the art..
	s=strchr(mainstubinfo.names.cd.c_name,'#');
	if (s)
		strcpy(mainstubinfo.names.cd.c_name,"Bad Name");
	if (strlen(mainstubinfo.names.cd.c_name)==0)
		strcpy(mainstubinfo.names.cd.c_name,"No Name");
	if (!strcmp("iamabot",mainstubinfo.names.cd.c_name))
		olracecfg.forcebotmode=1;
/*	if (wantedmaindriver==VIDEO_D3D)
		sc=video_getnsubdrivers(VIDEO_D3D,&nsc);
	freescript(sc,nsc);
	if (olracecfg.forcebotmode) {
		video_init(0,0);
		logger("botvideo\n");
		video_setupwindow(160,120,DESIREDBPP);
		zoomin=1;
	} else if (nsc>0) {
		logger("d3dvideo\n");
		video_setupwindow(globalxres,globalyres,DESIREDBPP);
		uselights=1;
	} else {
		logger("nod3dvideo\n");
		video_init(0,0);
		video_setupwindow(softwarexres,softwareyres,DESIREDBPP);
		uselights=0;
	}
	usescnlights=0; */
	video_setupwindow(GX,GY);
	pushandsetdir("fonts");
	onlineracefont=new fontq("med");
	popdir();
	memset(&od,0,sizeof(od));
	memset(&ocp,0,sizeof(ocp));
	ocp.cntl=CNTL_HUMAN;
	memset(&opa[0],0,sizeof(opa));
	memset(&opasave[0],0,sizeof(opasave));
	memset(&globalstatepacket,0,sizeof(globalstatepacket));
	memset(&globalstatepacket0,0,sizeof(globalstatepacket0));
	copyglobalinfo(&ol_opt,&od);
/*	if (olracecfg.bluered3d) {
		od.blueredbm=bitmap16alloc(WX,WY,-1);
		initblueredmask();
	} */
	if (ol_opt.nt==NET_BCLIENT)
		od.numcurplayers=0;
	else
		od.numcurplayers=1; // count yourself
//	for (i=0;i<MAXWEAPFLY;i++)
//		od.weapflys[i].kind=od.weapflys[i].lastkind=-1;
	if (od.numplayers>1) { // server
		carstubinfos[0].cd=mainstubinfo.names.cd;
// instead of ...
/*		strcpy(carstubinfos[0].c_name,mainstubinfo.names.c_name);
		strcpy(carstubinfos[0].c_paint,mainstubinfo.names.c_paint);
		strcpy(carstubinfos[0].c_decal,mainstubinfo.names.c_decal);
		strcpy(carstubinfos[0].c_body,mainstubinfo.names.c_body);
		memcpy(carstubinfos[0].c_hsvs,mainstubinfo.names.c_hsvs,sizeof(carstubinfos[0].c_hsvs));
		memcpy(carstubinfos[0].c_weaps,mainstubinfo.names.c_weaps,mainstubinfo.names.c_nweap*sizeof(mainstubinfo.names.c_weaps[0]));
		carstubinfos[0].c_nweap=mainstubinfo.names.c_nweap;
		memcpy(carstubinfos[0].c_energies,mainstubinfo.names.c_energies,sizeof(carstubinfos[0].c_energies));
*/
		for (i=0;i<od.numcars;i++) {
			opa[i].playerid=i;
		}
		od.playerid=0;
	}
	if (od.numplayers!=1 || ol_opt.nt==NET_BCLIENT) {
		if (!matchcl)
			initsocker();
	}
//	if (currule->useweap)
pushandsetdirdown("loading");
//	elsex

// pushandsetdirdown("loadingvs");
//	fileopen("awaitconnect.jpg",READ);
//	b1=gfxread24();
//	fileclose();
	od.awaitconnect=texture_create("hw_game_wait.jpg");
	od.awaitconnect->load();
/*	bitmap32* b1=gfxread32("awaitconnect.jpg");
//	if (WX==b1->x && WY==b1->y) {
//		if (0) {
//		od.awaitconnect=conv24to16ck(b1,BPP);
//		bitmap24free(b1);
//	} else {
//		od.awaitconnect=bitmap32alloc(WX,WY,C32GREEN);
//		filt24to24deluxe(b1,b2,0,0,b1->x-1,b1->y-1,0,0,WX-1,WY-1);
//		bitmap24free(b1);
//		od.awaitconnect=conv24to16ck(b2,BPP);
		clipscaleblit32(b1,od.awaitconnect);
//		bitmap24free(b2);
		bitmap32free(b1);
//	} */
popdir();
	U32 cw;
	READCW(cw);
	logger("end of onlineraceinit fpu control is at %08x\n",cw);
	if (videoinfo.video_fullscreen || wininfo.mousemode==MM_INFINITE || wininfo.usedirectinput)
//	if (video_maindriver!=VIDEO_GDI || mousemode==MM_INFINITE || usedirectinput)
		showcursor(0);
}

static void copycarinfop(int id,char *s)
{
//	int oneweapenergies[MAXENERGIES];
	char *tok;
	int i,j,k=0;
	logger("copycarinfop playername '%s'\n",s);
	tok=strtok(s,"#");
	if (tok)
		if (id<0) { // client
			id=atoi(tok);
			od.numplayers=od.numcars=id>>16;
			od.playerid=(id>>8)&0xff;
			of=&opa[od.playerid]; // find first car that has p j
			id=id&0xff;
			logger("copycarinfop curid %d, myid %d, numplayers %d\n",id,od.playerid,od.numplayers);
		} else { // server
			tok=strtok(NULL,"#"); // skip numplayers
			tok=strtok(NULL,"#"); // skip trackname
			tok=strtok(NULL,"#"); // skip gameid
			tok=strtok(NULL,"#"); // skip gametype
			tok=strtok(NULL,"#"); // skip playerid
			tok=strtok(NULL,"#"); // skip rules
			tok=strtok(NULL,"#"); // skip numbots
		}
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(carstubinfos[id].cd.c_name,tok);
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(carstubinfos[id].cd.c_body,tok);
	for (i=0;i<NUMHSVS;++i) {
		tok=strtok(NULL,"#");
		if (tok)
			carstubinfos[id].cd.c_hsvs[i].h=atoi(tok);
		tok=strtok(NULL,"#");
		if (tok)
			carstubinfos[id].cd.c_hsvs[i].s=atoi(tok);
		tok=strtok(NULL,"#");
		if (tok)
			carstubinfos[id].cd.c_hsvs[i].v=atoi(tok);
	}
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(carstubinfos[id].cd.c_paint,tok);
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(carstubinfos[id].cd.c_decal,tok);
	tok=strtok(NULL,"#");
	if (tok)
		carstubinfos[id].cd.c_rims=atoi(tok);
	tok=strtok(NULL,"#");
	if (tok) {
		carstubinfos[id].cd.c_nweap=atoi(tok);
		for (j=0;j<carstubinfos[id].cd.c_nweap;j++) {
			tok=strtok(NULL,"#");
			if (tok) {
				k=atoi(tok);
				carstubinfos[id].cd.c_weaps[j]=k;
			}
/*			for (i=0;i<MAXENERGIES;i++) {
				tok=strtok(NULL,"#");
				if (tok)
					oneweapenergies[i]=atoi(tok);
				else
					break;
			}
			if (i==MAXENERGIES)
				mergeenergymatrix(oneweapenergies,k,1<<(id+16)); */
		}
		for (i=0;i<MAXENERGIES;i++) {
			tok=strtok(NULL,"#");
			if (tok)
				carstubinfos[id].cd.c_energies[i]=atoi(tok);
		}
	} else
		carstubinfos[id].cd.c_nweap=0;
}

static void buildcarinfopacket(int pnnew,char *s)
{
	int i,j,k,pn;
	char weapstr[400],shortstr[10];
	if (pnnew>=0) { // server sends GO packet
		pn=pnnew&0xff;
		sprintf(weapstr,"#%d",carstubinfos[pn].cd.c_nweap);
		for (i=0;i<carstubinfos[pn].cd.c_nweap;i++) {
			k=carstubinfos[pn].cd.c_weaps[i];
			sprintf(shortstr,"#%u",k);
			strcat(weapstr,shortstr);
/*			for (j=0;j<MAXENERGIES;j++) {
				sprintf(shortstr,"#%u",olracecfg.energymatrix[k][j]);
				strcat(weapstr,shortstr);
			} */
		}
		for (j=0;j<MAXENERGIES;j++) {
			sprintf(shortstr,"#%u",carstubinfos[pn].cd.c_energies[j]);
			strcat(weapstr,shortstr);
		}
		sprintf(s,"%d#"
			"%s#%s#"
			"%d#%d#%d#"
			"%d#%d#%d#"
			"%d#%d#%d#"
			"%s#%s#%d%s",
			pnnew,
//			5,"trackname","skyboxname", // numplayers
			carstubinfos[pn].cd.c_name,carstubinfos[pn].cd.c_body,
			carstubinfos[pn].cd.c_hsvs[0].h,carstubinfos[pn].cd.c_hsvs[0].s,carstubinfos[pn].cd.c_hsvs[0].v,
			carstubinfos[pn].cd.c_hsvs[1].h,carstubinfos[pn].cd.c_hsvs[1].s,carstubinfos[pn].cd.c_hsvs[1].v,
			carstubinfos[pn].cd.c_hsvs[2].h,carstubinfos[pn].cd.c_hsvs[2].s,carstubinfos[pn].cd.c_hsvs[2].v,
			carstubinfos[pn].cd.c_paint,carstubinfos[pn].cd.c_decal,carstubinfos[pn].cd.c_rims,weapstr);
	} else { // client sends HI packet
		if (0) {
//		if (wininfo.isalreadyrunning) {
			sprintf(weapstr,"#%d",botinfos[0].c_nweap);
			for (i=0;i<botinfos[0].c_nweap;++i) {
				k=botinfos[0].c_weaps[i];
				sprintf(shortstr,"#%u",k);
				strcat(weapstr,shortstr);
/*				for (j=0;j<MAXENERGIES;j++) {
					sprintf(shortstr,"#%u",olracecfg.energymatrix[k][j]);
					strcat(weapstr,shortstr);
				} */
			}
			for (j=0;j<MAXENERGIES;j++) {
				sprintf(shortstr,"#%u",botinfos[0].c_energies[j]);
				strcat(weapstr,shortstr);
			}
		} else {
			sprintf(weapstr,"#%d",mainstubinfo.names.cd.c_nweap);
			for (i=0;i<mainstubinfo.names.cd.c_nweap;i++) {
				k=mainstubinfo.names.cd.c_weaps[i];
				sprintf(shortstr,"#%u",k);
				strcat(weapstr,shortstr);
/*				for (j=0;j<MAXENERGIES;j++) {
					sprintf(shortstr,"#%u",olracecfg.energymatrix[k][j]);
					strcat(weapstr,shortstr);
				} */
			}
			for (j=0;j<MAXENERGIES;j++) {
				sprintf(shortstr,"#%u",mainstubinfo.names.cd.c_energies[j]);
				strcat(weapstr,shortstr);
			}
		}
		sprintf(s,"%d#"
			"%d#%s#"
			"%d#%c#%d#"
			"%d#%d#"
			"%s#%s#"
			"%d#%d#%d#"
			"%d#%d#%d#"
			"%d#%d#%d#"
			"%s#%s#%d%s",
			pnnew,
			od.numcars,od.ntrackname, // numplayers,trackname,skyboxname send to server
			ol_opt.gameid,ol_opt.gt,ol_opt.playerid,
			olracecfg.rules,od.numbots,
			mainstubinfo.names.cd.c_name,mainstubinfo.names.cd.c_body,
			mainstubinfo.names.cd.c_hsvs[0].h,mainstubinfo.names.cd.c_hsvs[0].s,mainstubinfo.names.cd.c_hsvs[0].v,
			mainstubinfo.names.cd.c_hsvs[1].h,mainstubinfo.names.cd.c_hsvs[1].s,mainstubinfo.names.cd.c_hsvs[1].v,
			mainstubinfo.names.cd.c_hsvs[2].h,mainstubinfo.names.cd.c_hsvs[2].s,mainstubinfo.names.cd.c_hsvs[2].v,
			mainstubinfo.names.cd.c_paint,mainstubinfo.names.cd.c_decal,mainstubinfo.names.cd.c_rims,weapstr);
	}
	logger("buildcarinfopacket '%s'\n",s);
}

static void randombot(S32 slot)
{
#define NBOTWEAPS 2
	pushandsetdir("racenetdata/onlineopt");
	script sc("thecars.txt");
	script weapenablelist("weapenable.txt");
	S32 ncars=sc.num();
	popdir();
	mt_setseed(slot+ol_opt.gameid); // all players in same race have same gameid
	S32 i,j,k;
	opa[slot].cntl=CNTL_AI;
	memset(&carstubinfos[slot],0,sizeof(carstubinfos[0]));
	cardata& c=carstubinfos[slot].cd; // save some typing
// name
	strcpy(c.c_name,botnames[slot]);
// body
	strcpy(c.c_body,sc.idx(mt_random(ncars)).c_str());
// colors
	for (j=0;j<NUMHSVS;++j) {
		c.c_hsvs[j].h=mt_random(256);
		c.c_hsvs[j].s=mt_random(256);
		c.c_hsvs[j].v=mt_random(256);
	}
// energies
	for (j=0;j<MAXENERGIES;++j)
		c.c_energies[j]=2;
// weapons
	S32 nena=weapenablelist.num();
	U32 cwe=mt_random(nena);
	for (k=0;k<NBOTWEAPS;++k) {
		for (i=0;i<MAX_WEAPKIND;++i) {
			if (!my_stricmp(weapinfos[i].name,weapenablelist.idx((cwe+k)%nena).c_str())) {
				c.c_weaps[k]=i;
				++c.c_nweap;
			}
		}
	}
// rims
	c.c_rims=mt_random(5)+1; // first 5 rims, starting at 1, 10-28-09 bug fix, forgot to add 1

/*	for (i=od.numcurplayers;i<od.numplayers;++i) {
		carstubinfos[i].cd=botinfos[i%3];
		while(1) {
			if (i>=8) {
				sprintf(carstubinfos[i].cd.c_name,"PLAYER%d",i);
				break;
			}
			strcpy(carstubinfos[i].cd.c_name,botnames[mt_random(8)]);
			S32 j;
			for (j=0;j<i;j++)
				if (i!=j && !strcmp(carstubinfos[i].cd.c_name,carstubinfos[j].cd.c_name))
					break;
			if (j==i)
				break;
		}
		memcpy(carstubinfos[i].cd.c_weaps,botinfos[0].c_weaps,sizeof(carstubinfos[0].cd.c_weaps));
		carstubinfos[i].cd.c_nweap=botinfos[0].c_nweap;
		memcpy(carstubinfos[i].cd.c_energies,botinfos[0].c_energies,sizeof(carstubinfos[0].cd.c_energies));
	} */
}

void onlinerace_proc()
{
	static int allwritten=0,wrotego=0;
	char str[500];//,scalestr[50],startstr[50],endstr[50],
	char *tok;
	int i,j,n;
// initial timeout connect
	if (olracecfg.timeout>0) {
		olracecfg.timeout--;
		if (olracecfg.timeout==0) {
			if (od.master) {
				logger("server: didn't get all connections... %d/%d\n",
				  od.numcurplayers,od.numplayers);
			}
			if (od.numcurplayers==1 || !od.master) { // client timeout
//				justchangestate(&carextractstate);
				changestate(STATE_OLDCARENAEXTRACT);
//				gameinfoclear(); // tell match server
				od.goingtogame=1;
				mainstubinfo.bots=1;
				mainstubinfo.connections=0;
				of=&opa[0];
				disconnectandai(); // out of time on connect screen

				od.playerid=0;
				opa[0].playerid=0;
				memset(&carstubinfos[0],0,sizeof(carstubinfos[0]));
				carstubinfos[0].cd=mainstubinfo.names.cd; // copy this
/*				rather than this...
				strcpy(carstubinfos[0].c_name,mainstubinfo.names.c_name);
				strcpy(carstubinfos[0].c_paint,mainstubinfo.names.c_paint);
				strcpy(carstubinfos[0].c_body,mainstubinfo.names.c_body);
				memcpy(carstubinfos[0].c_hsvs,mainstubinfo.names.c_hsvs,sizeof(carstubinfos[0].c_hsvs));
				strcpy(carstubinfos[0].c_decal,mainstubinfo.names.c_decal);
				memcpy(carstubinfos[0].c_weaps,mainstubinfo.names.c_weaps,mainstubinfo.names.c_nweap*sizeof(mainstubinfo.names.c_weaps[0]));
				carstubinfos[0].c_nweap=mainstubinfo.names.c_nweap;
				memcpy(carstubinfos[0].c_energies,mainstubinfo.names.c_energies,sizeof(carstubinfos[0].c_energies)); */
				for (i=1;i<od.numcars;i++) {
					opa[i].playerid=0;
					opa[i].cntl=od.defaultcntl;
					memset(&carstubinfos[i],0,sizeof(carstubinfos[0]));
					carstubinfos[i].cd=botinfos[i%3];
					while(1) {
						if (i>=8) {
							sprintf(carstubinfos[i].cd.c_name,"PLAYER%d",i);
							break;
						}
						strcpy(carstubinfos[i].cd.c_name,botnames[mt_random(8)]);
						for (j=0;j<i;j++)
							if (i!=j && !strcmp(carstubinfos[i].cd.c_name,carstubinfos[j].cd.c_name))
								break;
						if (j==i)
							break;
					}
					memcpy(carstubinfos[i].cd.c_weaps,botinfos[0].c_weaps,sizeof(carstubinfos[0].cd.c_weaps));
					carstubinfos[i].cd.c_nweap=botinfos[0].c_nweap;
					memcpy(carstubinfos[i].cd.c_energies,botinfos[0].c_energies,sizeof(carstubinfos[0].cd.c_energies));

/*					if (botinfo.botbodys[i-1][0]) {
						strcpy(carstubinfos[i].c_body,botinfo.botbodys[i-1]);
						memcpy(carstubinfos[i].c_hsvs,botinfo.hsvs[i-1],sizeof(carstubinfos[0].c_hsvs));
					} else {
						strcpy(carstubinfos[i].c_body,botinfo.botbodys[i%3]);
						memcpy(carstubinfos[i].c_hsvs,botinfo.hsvs[i%3],sizeof(carstubinfos[0].c_hsvs));
					}
					strcpy(carstubinfos[i].c_paint,botinfo.botpaints[i%3]);
					strcpy(carstubinfos[i].c_decal,botinfo.botdecals[i%3]);
					memcpy(carstubinfos[i].c_weaps,botinfo.botweaps[0],sizeof(carstubinfos[0].c_weaps));
					carstubinfos[i].c_nweap=botinfo.nbotweap[0];
					memcpy(carstubinfos[i].c_energies,botinfo.botenergies[0],sizeof(carstubinfos[0].c_energies)); */
				}
				of=&opa[0]; // find first car that has p0
				return;
			} else { // server okay, not all clients connected, try to reduce the number of players
				olracecfg.timeout=olracecfg.beforeloadtimeoutserver*15;
				of=&opa[0];
				j=1;
				for (i=1;i<OL_MAXPLAYERS;i++) {
					if (od.playernet[i].retrytimers!=-1 && od.playernet[i].tcpclients) {
						od.playernet[i].retrytimers=0;
						freesocker(od.playernet[i].tcpclients);
						od.playernet[i].tcpclients=NULL;
					}
				}
				for (i=1;i<od.numplayers;i++) {
					if (od.playernet[i].tcpclients) {
						logger("copying %d to %d\n",i,j);
						if (i!=j) {
							carstubinfos[j].cd=carstubinfos[i].cd;
/*							strcpy(carstubinfos[j].c_name,carstubinfos[i].c_name);
							strcpy(carstubinfos[j].c_paint,carstubinfos[i].c_paint);
							strcpy(carstubinfos[j].c_body,carstubinfos[i].c_body);
							memcpy(carstubinfos[j].c_hsvs,carstubinfos[i].c_hsvs,sizeof(carstubinfos[0].c_hsvs));
							strcpy(carstubinfos[j].c_decal,carstubinfos[i].c_decal);
							memcpy(carstubinfos[j].c_weaps,carstubinfos[i].c_weaps,carstubinfos[0].c_nweap*sizeof(carstubinfos[0].c_weaps[0]));
							carstubinfos[j].c_nweap=carstubinfos[i].c_nweap;
							memcpy(carstubinfos[j].c_energies,carstubinfos[i].c_energies,sizeof(carstubinfos[0].c_energies)); */
							od.playernet[j].tcpclients=od.playernet[i].tcpclients;
								od.playernet[i].tcpclients=NULL;
						}
						j++;
					}
				}
				of=&opa[0]; // find first car that has p0
				od.numcars=od.numplayers=od.numcurplayers=j;
				od.playerid=0;
				opa[0].playerid=0;
				logger("server: didn't get all connections part II, reduced to %d players\n",od.numplayers);
			}
		}
	}
// just 1 player (no net) go load and race now
	if (od.numplayers==1 && ol_opt.nt!=NET_BCLIENT) {
//		od.numcars=4;
		od.playerid=0;
		opa[0].playerid=0;
		opa[0].cntl=CNTL_HUMAN;
		carstubinfos[0].cd=mainstubinfo.names.cd;
/*		memset(&carstubinfos[0],0,sizeof(carstubinfos[0]));
		strcpy(carstubinfos[0].c_name,mainstubinfo.names.c_name);
		strcpy(carstubinfos[0].c_paint,mainstubinfo.names.c_paint);
		strcpy(carstubinfos[0].c_body,mainstubinfo.names.c_body);
		memcpy(carstubinfos[0].c_hsvs,mainstubinfo.names.c_hsvs,sizeof(carstubinfos[0].c_hsvs));
		strcpy(carstubinfos[0].c_decal,mainstubinfo.names.c_decal);
		memcpy(carstubinfos[0].c_weaps,mainstubinfo.names.c_weaps,mainstubinfo.names.c_nweap*sizeof(mainstubinfo.names.c_weaps[0]));
		carstubinfos[0].c_nweap=mainstubinfo.names.c_nweap;
		memcpy(carstubinfos[0].c_energies,mainstubinfo.names.c_energies,sizeof(carstubinfos[0].c_energies)); */
		for (i=1;i<od.numcars;i++) {
			opa[i].playerid=0;
			opa[i].cntl=od.defaultcntl; // CNTL_AI;
			memset(&carstubinfos[i],0,sizeof(carstubinfos[0]));
			carstubinfos[i].cd=botinfos[i%3];
			while(1) {
				if (i>=8) {
					sprintf(carstubinfos[i].cd.c_name,"PLAYER%d",i);
					break;
				}
				strcpy(carstubinfos[i].cd.c_name,botnames[mt_random(8)]);
				for (j=0;j<i;j++)
					if (i!=j && !strcmp(carstubinfos[i].cd.c_name,carstubinfos[j].cd.c_name))
						break;
				if (j==i)
					break;
			}
			carstubinfos[i].cd.c_nweap=botinfos[0].c_nweap;
			memcpy(carstubinfos[i].cd.c_weaps,botinfos[0].c_weaps,sizeof(carstubinfos[0].cd.c_weaps));
			memcpy(carstubinfos[i].cd.c_energies,botinfos[0].c_energies,sizeof(carstubinfos[0].cd.c_energies));
/*			strcpy(carstubinfos[i].c_paint,botinfo.botpaints[(i-1)%3]);
			if (botinfo.botbodys[i-1][0]) {
				strcpy(carstubinfos[i].c_body,botinfo.botbodys[i-1]);
				memcpy(carstubinfos[i].c_hsvs,botinfo.hsvs[i-1],sizeof(carstubinfos[0].c_hsvs));
			} else {
				strcpy(carstubinfos[i].c_body,botinfo.botbodys[(i-1)%3]);
				memcpy(carstubinfos[i].c_hsvs,botinfo.hsvs[(i-1)%3],sizeof(carstubinfos[0].c_hsvs));
			}
			strcpy(carstubinfos[i].c_decal,botinfo.botdecals[(i-1)%3]); */
		}
//		od.carid=0;
		of=&opa[0]; // find first car that has p0
//		justchangestate(&carextractstate);
		changestate(STATE_OLDCARENAEXTRACT);
		od.keepsockets=1; // sockets were never inited, so don't uninit
		od.goingtogame=1;
		mainstubinfo.bots=1;
		mainstubinfo.connections=0;
		return;
	}
	if (KEY==K_ESCAPE) {
		if (ol_opt.backtoopt || matchcl) {
			if (matchcl) {
				changestate(STATE_MATCHRESULTS);
			} else {
				changestate(STATE_OLDCARENALOBBY);
			}
//			gameinfoclear(); // tell match server
		} else {
			changestate(-1);
		}
	}
//		popstate(); // bail
//		changestate(-1); // bail
// see if SERVER done with all tasks
	if (od.master && od.numplayers-od.numbots==od.numcurplayers) {
		if (!wrotego) { // send 'go' packets to all clients
			for (i=0;i<od.numplayers-od.numbots;i++) {
				if (od.playernet[i].tcpclients) { // && ((od.curplayersmask>>i)&1) ) {
//					sprintf(str,"go%s",stubinfo.pname);
					logger("go #1\n");
					for (j=0;j<od.numplayers-od.numbots;j++) {
						buildcarinfopacket(j+(i<<8)+(od.numplayers<<16),str);
						tcpwritetoken8(od.playernet[i].tcpclients,PKT_GO,str,strlen(str));	// WRITE 'go'
					}
				}
			}
			wrotego=1; // done sending everything
			allwritten=0; // wait around for ack
		}
		if (allwritten) { // all done sending and got ack, load and play race
			of=&opa[findcurslot(0)]; // find first car that has p0
//			justchangestate(&carextractstate);
			changestate(STATE_OLDCARENAEXTRACT);
//			gameinfoclear(); // tell match server
			od.keepsockets=1;
			od.goingtogame=1;
			mainstubinfo.bots=0;
			mainstubinfo.connections=od.numplayers-1;
			return;
		}
	}
// wait for myip(s)
	if (!od.myip) {
		od.playerid=-1;
		wrotego=0;
		if (n=getnmyip()) {
			for (j=0;j<OL_MAXPLAYERS;j++) {
				for (i=0;i<n;i++)
					if (ol_opt.serverip==getmyip(i)) { // is it in the list?
						od.myip=ol_opt.serverip;
						break;
					}
				if (i!=n)
					break;
			}
///*#ifdef BEHINDFIREWALL
			if (!od.myip)
				od.myip=getmyip(0);	// if ip not in the list, (firewall)
//#endif */
		}
	}
// i have an ip, try some connections
	if (od.myip) { // got an ip, try to make some connections etc..
		for (i=0;i<OL_MAXPLAYERS;i++) {
			if (od.playernet[i].tcpclients)
				tcpfillreadbuff(od.playernet[i].tcpclients); // READBUFF
//			if (od.playernet[i].alttcpclients)
//				tcpfillreadbuff(od.playernet[i].alttcpclients); // READBUFF
		}
//	logger("makemasters fpu control is at %08x\n",getfpucontrol());
		if (cgi.isvalid && !cgi.ishost)
			ol_opt.nplayers=-1;
		if (ol_opt.nplayers>1 /*&& !wininfo.isrunning */) { // try and make a server
//			if ((mainstubinfo.usestub && mainstubinfo.playerid==0) ||
//			(!mainstubinfo.usestub && od.myip==ol_opt.conninfos[0].ip))
			if (!od.master && !od.playernet[0].tcpclients) { /*&& !od.playernet[0].alttcpclients*/
				if (cgi.isvalid) {
					od.master=makecgisock(); // get master socket from match lobby info
					gameinfoupdate();
				} else {
					od.master=tcpmakemaster(0,ol_opt.port); // MAKEMASTER, become a server
				}
				if (od.master)
					olracecfg.timeout=olracecfg.beforeloadtimeoutserver*30;
			}
		}

//	logger("makemastere fpu control is at %08x\n",getfpucontrol());
//	logger("makeconnects fpu control is at %08x\n",getfpucontrol());
		if (!od.master && !od.playernet[0].tcpclients) { // GETCONNECT, become a client
			od.numcurplayers=0; // wait for nplayers go packets..
			if (ol_opt.nt==NET_BCLIENT) { // connect to broadcast server
//				od.playernet[0].tcpclients=tcpgetconnect(ol_opt.serverip,BROADCAST_PORT);
				od.playernet[0].tcpclients=tcpgetconnect(ol_opt.serverip,ol_opt.port);
			} else {
				if (cgi.isvalid) {
					od.playernet[0].tcpclients=makecgisock(); // get connect socket from match lobby info
				} else {
					od.playernet[0].tcpclients=tcpgetconnect(ol_opt.serverip,ol_opt.port/*+1*/);
				}
			}
//	logger("makeconnecte fpu control is at %08x\n",getfpucontrol());
//	logger("tcpw2s fpu control is at %08x\n",getfpucontrol());
			logger("hi #1\n");
			buildcarinfopacket(-1,str);
			tcpwritetoken8(od.playernet[0].tcpclients,PKT_HI,str,strlen(str)); // WRITE 'hi'
//	logger("tcpw2e fpu control is at %08x\n",getfpucontrol());
			allwritten=0;
		}
		if (od.master) {
//// SERVER SIDE
			struct socker *cl;
			od.playerid=0;
			opa[0].cntl=CNTL_HUMAN;
//	logger("stgetaccept fpu control is at %08x\n",getfpucontrol());
			cl=tcpgetaccept(od.master); // GETACCEPT, login
//	logger("endgetaccept fpu control is at %08x\n",getfpucontrol());
			if (cl) { // look at number of current player connections..
				for (i=1;i<OL_MAXPLAYERS;i++)
//#ifdef BEHINDFIREWALL
					if (!od.playernet[i].tcpclients) // just connect with anybody (firewall)
//#else
//					if (cl->remoteip==ol_opt.conninfos[i].ip && !od.playernet[i].tcpclients)
//#endif
						break;
				if (i!=OL_MAXPLAYERS)
					od.playernet[i].tcpclients=cl;
				else
					freesocker(cl); // CLOSE
			}
			for (i=0;i<OL_MAXPLAYERS;i++) {
				if (od.playernet[i].tcpclients && (od.playernet[i].tcpclients->flags&SF_CLOSED)) {
					freesocker(od.playernet[i].tcpclients); // CLOSE closed
					od.playernet[i].tcpclients=NULL;
					od.playernet[i].retrytimers=0;
//					od.curplayersmask&=~(1<<i);
					od.numcurplayers--;
					gameinfoupdate();
					logger("server: player %d left, numcurplayers %d\n",i,od.numcurplayers);
				}
			}
			for (i=0;i<OL_MAXPLAYERS;i++) {
				if (od.playernet[i].tcpclients) {
					S32 id;
					n=tcpreadtoken8(od.playernet[i].tcpclients,&id,str,499); // READ something
					if (n>0) {
						str[n]='\0';
//						if (str[0]=='H' && str[1]=='I') {
						if (id==PKT_HI) {
							logger("server: got a 'hi' packet from %d, namestr #1 %s\n",i,str);
							copycarinfop(i,str);
							od.numcurplayers++;
							gameinfoupdate();
//							sprintf(scalestr,"%f",od.ntrackscale);
//							sprintf(startstr,"%f",od.trackstart);
//							sprintf(endstr,"%f",od.trackend);
							sprintf(str,"%d %d %s %d %c %d",
								od.numcars,i,od.ntrackname,olracecfg.rules,ol_opt.gt,od.numbots);
//							od.ntrackscale=(float)atof(scalestr);
//							od.trackstart=(float)atof(startstr);
//							od.trackend=(float)atof(endstr);
							logger("sending PL packet '%s'\n",str);
							tcpwritetoken8(od.playernet[i].tcpclients,PKT_PL,str,strlen(str));
							od.playernet[i].retrytimers=-1;
							allwritten=0;
						}
					} else if (od.playernet[i].retrytimers>=0) { // wait around for client to say 'hi'
						od.playernet[i].retrytimers++;
						if (od.playernet[i].retrytimers==60) {
							od.playernet[i].retrytimers=0;
							freesocker(od.playernet[i].tcpclients); // CLOSE client no 'hi'
							od.playernet[i].tcpclients=NULL;
//							od.curplayersmask&=~(1<<i);
							od.numcurplayers--;
							gameinfoupdate();
						}
					}
				}
			}
// CLIENT SIDE
		} else {
			if (od.playernet[0].tcpclients) {
				S32 id;
				n=tcpreadtoken8(od.playernet[0].tcpclients,&id,str,499); // READ something, get reply from server
				if (n>0) {
					str[n]='\0';
//					if (str[0]=='P' && str[1]=='L') { // READ 'polnum' nplayers pid trackname trackscale
					if (id==PKT_PL) {
						tok=strtok(str," ");
						if (tok) {
							j=atoi(tok);
							od.numcars=od.numplayers=j;
							tok=strtok(NULL," ");
							if (tok) {
								j=atoi(tok);
								od.playerid=j;
								of=&opa[j]; // find first car that has p j
								of->cntl=CNTL_HUMAN;
								tok=strtok(NULL," ");
								for (j=0;j<od.numcars;j++)
									opa[j].playerid=j;
								if (tok) {
									mystrncpy(od.ntrackname,tok,NAMESIZEI);
//									od.ol_uedittrackname[0]='q'; // force client to see a bad track
									tok=strtok(NULL," ");
									if (tok) {
										olracecfg.rules=atoi(tok);
										currule=&rules[olracecfg.rules];
										tok=strtok(NULL," ");
										if (tok) {
											ol_opt.gt=tok[0];
											tok=strtok(NULL," ");
											if (tok) {
												od.numbots=atoi(tok);
											}
										}
									}
								}
							}
						}
//					} else if (str[0]=='G' && str[1]=='O') { // READ 'go'
					} else if (id==PKT_GO) { // READ 'go'
						logger("namestr #3 %s\n",str);
						copycarinfop(-1,str);
						od.numcurplayers++;
						if (od.numplayers-od.numbots==od.numcurplayers) {
//							justchangestate(&carextractstate);
							changestate(STATE_OLDCARENAEXTRACT);
							gameinfoclear(); // tell match server
							od.keepsockets|=1;
							od.goingtogame=1;
							mainstubinfo.bots=0;
							mainstubinfo.connections=1;
						}
					}
				} else {
					if (od.playerid==-1)
						od.playernet[0].retrytimers++;
					if (od.playernet[0].retrytimers==180) {
						od.playernet[0].retrytimers=0;
						freesocker(od.playernet[0].tcpclients); // CLOSE, server never sent 'polnum'
						od.playernet[0].tcpclients=NULL;
					}
				}
			}
		}
		allwritten=1;
		for (i=0;i<OL_MAXPLAYERS;i++)
			if (od.playernet[i].tcpclients)
				if (!tcpsendwritebuff(od.playernet[i].tcpclients)) // WRITEBUFF
					allwritten=0;
	}
// print stuff
}

/*
void onlinerace_draw2d()
{
	if (od.awaitconnect) {
		int t;
//		video_lock();
		clipblit32(od.awaitconnect,B32,0,0,0,0,WX,WY);
		if (od.master)
			t=olracecfg.timeout*4-olracecfg.beforeloadtimeoutserver*120+WX*4/8;
		else
			t=olracecfg.timeout*2-olracecfg.beforeloadtimeoutclient*60+WX*4/8;
		if (t<0)
			t=0;
		outtextxyf32(B32,WX/2-44-t,WY/2-16-t,C32BLACK,"WAITING FOR");
		outtextxyf32(B32,WX/2-48-t,WY/2-t,C32BLACK,"OTHER PLAYERS");
		outtextxyf32(B32,WX/2-40-t,WY/2+16-t,C32BLACK,"TO CONNECT");
		outtextxyf32(B32,WX/2-44+1+t,WY/2-16+1+t,C32WHITE,"WAITING FOR");
		outtextxyf32(B32,WX/2-48+1+t,WY/2+1+t,C32WHITE,"OTHER PLAYERS");
		outtextxyf32(B32,WX/2-40+1+t,WY/2+16+1+t,C32WHITE,"TO CONNECT");
//		video_unlock();
	}
}
*/
void onlinerace_draw3d()
{
	if (od.awaitconnect) {
		video_sprite_begin(
		800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
		VP_CLEARBG,
		0);
		video_sprite_draw(od.awaitconnect,F32WHITE,0,0,800,600);
		int t;
		if (od.master)
			t=olracecfg.timeout*4-olracecfg.beforeloadtimeoutserver*120+WX*4/8;
		else
			t=olracecfg.timeout*2-olracecfg.beforeloadtimeoutclient*60+WX*4/8;
		if (t<0)
			t=0;
/*		outtextxyf32(B32,WX/2-44-t,WY/2-16-t,C32BLACK,"WAITING FOR");
		outtextxyf32(B32,WX/2-48-t,WY/2-t,C32BLACK,"OTHER PLAYERS");
		outtextxyf32(B32,WX/2-40-t,WY/2+16-t,C32BLACK,"TO CONNECT");
		outtextxyf32(B32,WX/2-44+1+t,WY/2-16+1+t,C32WHITE,"WAITING FOR");
		outtextxyf32(B32,WX/2-48+1+t,WY/2+1+t,C32WHITE,"OTHER PLAYERS");
		outtextxyf32(B32,WX/2-40+1+t,WY/2+16+1+t,C32WHITE,"TO CONNECT"); */
		pointf3 c=pointf3x(0,0,0);
		c.w=1-t/255.0f;
		c.w=range(0.0f,c.w,.9999f);
		onlineracefont->print(182,110,400,20,c,"Waiting for other players to connect");
		video_sprite_end();
	}
}

#if 0
void loadingcallback()
{
//	char *t;
//	int tx,ty;
	int x0,y0,xs0,ys,xs;
	od.loadingcount++;
	if ((od.loadingcount&0xf)!=1)
		return;
	video_lock();
	clipblit32(od.loadingbackground,B32,0,0,0,0,WX,WY);

	x0=50+20*8;
	y0=WY-13;
	xs0=100;
	ys=8;
	outtextxyf32(B32,50,y0,C32WHITE,"Loading Assets");
	cliprecto32(B32,x0-1,y0-1,x0+xs0+1,y0+ys,C32WHITE);
	xs=xs0*od.loadingcount/olracecfg.loadinggoal1;
	if (xs>xs0)
		xs=xs0;
	cliprect32(B32,x0,y0,x0+xs-1,y0+ys-1,C32WHITE);


//	tx=WX*od.loadingcount/olracecfg.loadinggoal1-od.loadingcar->x/2;
//	ty=WY*od.loadingcount/olracecfg.loadinggoal1-od.loadingcar->y/2;
//	clipxpar16(od.loadingcar,B32,0,0,tx,ty,od.loadingcar->x,od.loadingcar->y);
/*	if (video_maindriver!=VIDEO_GDI) {
		cliprect32(B32,0,0,WX-1,7,hiblue);
		t=getwindowtext();
		outtextxyf32(B32,WX/2-4*strlen(t),0,0xffff,"%s",t);
//		outtextxybf16(B32,WX/2-28,16,0,0xffff,"Loading");
	} */
// else
//		outtextxybf16(B32,WX/2-28,8,0,0xffff,"Loading");
//	outtextxyf32(B32,WX/2-100,WY/2-4,0xffff,"Loading game... %d/%d",od.loadingcount,olracecfg.loadinggoal1);
//	outtextxyf32(B32,WX/2-100,WY/2-4+10,0xffff,"Track '%s'",od.ol_uedittrackname);
//	outtextxyf32(B32,WX/2-100,WY/2-4+20,0xffff,"Scale %f",od.ntrackscale);
	video_unlock();
	video_paintwindow(0);
}
#endif

void onlinerace_exit()
{
	if (wininfo.closerequested && matchcl) {
		freesocker(matchcl);
		matchcl=0;
	}
	int i,j;
	U32 cw;
	READCW(cw);
	logger("start of onlineraceexit fpu control is at %08x\n",cw);
	if (!od.goingtogame) {
		freeweaps();
		popdir();
		cgi.isvalid=false;
		delete onlineracefont;
		onlineracefont=0;
	}
	if (od.awaitconnect)
		od.awaitconnect->rc.deleterc(od.awaitconnect);
	od.awaitconnect=0;
//	if (od.awaitconnect)
//		bitmap32free(od.awaitconnect);
//	od.awaitconnect=NULL;
	od.numcurplayers=1;
//	od.numcurolplayers=0;
	if (!od.keepsockets) {
		for (j=0;j<OL_MAXPLAYERS;j++) {
			if (od.playernet[j].tcpclients) {
				freesocker(od.playernet[j].tcpclients);
				od.playernet[j].tcpclients=NULL;
			}
//			if (od.playernet[j].alttcpclients) {
//				freesocker(od.playernet[j].alttcpclients);
//				od.playernet[j].alttcpclients=NULL;
//			}
		}
		if (!matchcl)
			uninitsocker();
	}
	for (i=0;i<OL_MAXPLAYERS;i++)
		od.playernet[i].retrytimers=-1;
	if (od.numplayers==1)//&& (olracecfg.timeout || !olracecfg.beforeloadtimeout))
		return;
	gameinfoclear(); // tell match server
	if (od.master)
		freesocker(od.master);
//	if (od.altmaster)
//		freesocker(od.altmaster);
	od.master=/*od.altmaster=*/NULL;
	if (!od.goingtogame)
		showcursor(1);
}

///////////// car extract state //////////
static int extracttimer,nostartsleep;
static int gointorace2;
//static char carmodelnames[2][50];
void writetalkfile(char *str);
// returns 1 if doneextracting
int readtalkfile();

//text2ext.txt
//["tCREvilTwinTextureBank.w3d","EvilTwin","paint06","decal04","Player1_texture.tga","tCREvilTwinTextureBank.w3d","EvilTwin","paint04","decal12","Player2_texture.tga"]
//cars2ext.txt
//["mdlCREvilTwin.w3d","EvilTwin","Player1","mdlCREvilTwin.w3d","EvilTwin","Player2"]
/*static int w3dexists(char *mname)
{
	char str[100];
//	pushandsetdir(dswmediadir);
	sprintf(str,"mdlcr%s.w3d",mname);
	if (fileexist(str)) {
		popdir();
		return 1;
	}
//	popdir();
	return 0;
}
*/
void carextractstate_init()
{
	int i;//,ii;
//	struct bitmap24 *b1,*b2;
//	FILE *fw;
//	mainstubinfo.useslextra=0; // disable slextra
	pushandsetdir("racenetdata/loading");
	od.loadingbackground=texture_create("hw_game_load.jpg");
	od.loadingbackground->load();
	popdir();
	gointorace2=0;
//	if (!olracecfg.carextracttimeout)
		olracecfg.carextracttimeout=2;
	for (i=0,op=&opa[0];i<OL_MAXCARSLOTS;i++,op++)
		op->carid=i;
	logger("mycar is %d\n",of->carid);
	for (i=0;i<OL_MAXCARSLOTS;i++)
		logger("car %d: car body '%s', car paint '%s', car decal '%s' rims %d, hsv(%d,%d,%d)(%d,%d,%d)(%d,%d,%d)\n",
		i,carstubinfos[i].cd.c_body,carstubinfos[i].cd.c_paint,carstubinfos[i].cd.c_decal,carstubinfos[i].cd.c_rims,
			carstubinfos[i].cd.c_hsvs[0].h,carstubinfos[i].cd.c_hsvs[0].s,carstubinfos[i].cd.c_hsvs[0].v,
			carstubinfos[i].cd.c_hsvs[1].h,carstubinfos[i].cd.c_hsvs[1].s,carstubinfos[i].cd.c_hsvs[1].v,
			carstubinfos[i].cd.c_hsvs[2].h,carstubinfos[i].cd.c_hsvs[2].s,carstubinfos[i].cd.c_hsvs[2].v);
	extracttimer=2;//olracecfg.carextracttimeout*30;
	nostartsleep=5;
	logger("in carextractstate init timer at %d\n",extracttimer);
/*	if (mainstubinfo.useslextra) {
		if (currule->useweap)
			pushandsetdir("loading");
		else
			pushandsetdir("loadingvs");
		fileopen("awaitconnect.jpg",READ);
		b1=gfxread24();
		fileclose();
		if (WX==b1->x && WY==b1->y) {
	//		if (0) {
			od.awaitconnect=conv24to16ck(b1,BPP);
			bitmap24free(b1);
		} else {
			b2=bitmap24alloc(WX,WY,rgbgreen);
			filt24to24deluxe(b1,b2,0,0,b1->x-1,b1->y-1,0,0,WX-1,WY-1);
			bitmap24free(b1);
			od.awaitconnect=conv24to16ck(b2,BPP);
			bitmap24free(b2);
		}
		popdir();
		pushandsetdir(prefsdir);
		fw=fopen2("cars2ext.txt","w");
		popdir();
		if (fw) {

			for (i=0;i<od.numcars;i++) {
//			for (i=0;i<mainstubinfo.ncars;i++) {
				if (w3dexists(carstubinfos[i].body)) {
					ii=i;
					logger("'%s' w3d file exists, good..\n",carstubinfos[i].body);
				} else {
					ii=of->carid;
					logger("'%s' w3d file not found, using '%s'\n",carstubinfos[i].body,carstubinfos[ii].body);
				}
				if (i==0)
					fprintf(fw,"[\"dummy.w3d\",\"%s\",\"player%d\",",carstubinfos[ii].body,i+1);
				else if (i==od.numcars-1)
					fprintf(fw,"\"dummy.w3d\",\"%s\",\"player%d\"]",carstubinfos[ii].body,i+1);
				else
					fprintf(fw,"\"dummy.w3d\",\"%s\",\"player%d\",",carstubinfos[ii].body,i+1);
			}
//			fprintf(fw,"\"dummy.w3d\",\"%s\",\"player2\"]",stubinfo.p2body);
//			fprintf(fw,"[\"dummy.w3d\",\"%s\",\"%s\",",stubinfo.p1body,stubinfo.p1body);
//			fprintf(fw,"\"dummy.w3d\",\"%s\",\"%s\"]",stubinfo.p2body,stubinfo.p2body);
			fclose(fw);
		}
		pushandsetdir(prefsdir);
		fw=fopen2("tex2ext.txt","w");
		popdir();
		if (fw) {
			for (i=0;i<od.numcars;i++) {
//			for (i=0;i<mainstubinfo.ncars;i++) {
				if (w3dexists(carstubinfos[i].body))
					ii=i;
				else
					ii=of->carid;
				if (i==0)
					fprintf(fw,"[\"dummybank.w3d\",\"%s\",\"%s\",\"%s\",\"Player%d_texture.tga\",",
						carstubinfos[ii].body,carstubinfos[i].paint,carstubinfos[i].decal,i+1);
				else if (i==od.numcars-1)
					fprintf(fw,"\"dummybank.w3d\",\"%s\",\"%s\",\"%s\",\"Player%d_texture.tga\"]",
						carstubinfos[ii].body,carstubinfos[i].paint,carstubinfos[i].decal,i+1);
				else
					fprintf(fw,"\"dummybank.w3d\",\"%s\",\"%s\",\"%s\",\"Player%d_texture.tga\",",
						carstubinfos[ii].body,carstubinfos[i].paint,carstubinfos[i].decal,i+1);
			}
			fclose(fw);
		}
//		strcpy(carmodelnames[0],stubinfo.p1body);
//		strcpy(carmodelnames[1],stubinfo.p2body);
		writetalkfile("ExtractData");
	} */
}

void carextractstate_proc()
{
//	int x0,y0,xs0,xs,ys;
//	int done;
/*	if (olracecfg.extractsleep && mainstubinfo.useslextra && extracttimer>0 && !nostartsleep) {
		extracttimer-=olracecfg.extractsleep*30/1000;
		logger("going to sleep for %d millisecs\n",olracecfg.extractsleep);
		Sleep(olracecfg.extractsleep);
	}
	else */
		extracttimer--;
		if (extracttimer<=0) {
//	if (extracttimer<=0 || !mainstubinfo.useslextra) {
		logger("out of time or no extra\n");
//		mainstubinfo.useslextra=0;
		gointorace2=1;
//		justchangestate(&mainracestate);
		changestate(STATE_OLDCARENAMAINGAME);
		return;
	}
//	if (extracttimer==135)
//		writetalkfile("DoneExtracting");
#if 0
	if (olracecfg.extractsleep || !(extracttimer%30)) {
		done=readtalkfile();
		if (done) {
			logger("extra extracted with time left %d\n",extracttimer);
			mainstubinfo.useslextra=1;
			gointorace2=1;
			justchangestate(&mainracestate);
			return;
		}
	}
//	if (KEY==K_ESCAPE) {
//		logger("bailed out of car extraction extra\n");
//		changestate(-1);
//		return;
//	}
	video_lock();
	clipblit32(od.awaitconnect,B32,0,0,0,0,WX,WY);
	x0=50+20*8;
	y0=WY-13;
	xs0=100;
	ys=8;
	outtextxyf32(B32,50,y0,C32WHITE,"Extracting Vehicles");
	cliprecto16(B32,x0-1,y0-1,x0+xs0+1,y0+ys,C32WHITE);
	xs=(olracecfg.carextracttimeout*30-extracttimer)*xs0/(olracecfg.carextracttimeout*30)+1;
	cliprect32(B32,x0,y0,x0+xs-1,y0+ys-1,C32WHITE);
	video_unlock();
	if (nostartsleep)
		nostartsleep--;
#endif
}

void carextractstate_draw3d()
{
	if (od.loadingbackground) {
		video_sprite_begin(
		800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
		VP_CLEARBG,
		0);
		video_sprite_draw(od.loadingbackground,F32WHITE,0,0,800,600);
		pointf3 c=pointf3x(1,1,1,1);
		onlineracefont->print(182,110,400,20,c,"Loading");
		video_sprite_end();
	}
}

void carextractstate_exit()
{
	int j;
	logger("in carextractstate exit\n");
	delete onlineracefont;
	onlineracefont=0;
	if (od.awaitconnect)
		od.awaitconnect->rc.deleterc(od.awaitconnect);
	od.awaitconnect=0;
	if (od.loadingbackground)
		od.loadingbackground->rc.deleterc(od.loadingbackground);
	od.loadingbackground=0;
//	if (od.awaitconnect)
//		bitmap32free(od.awaitconnect);
//	od.awaitconnect=NULL;
	if (!gointorace2) {
		cgi.isvalid=false;
//		bitmap32free(od.loadingbackground);
//		bitmap32free(od.cursorpic);
//		bitmap16free(od.loadingcar);
		if (od.numplayers!=1) {
			for (j=0;j<OL_MAXPLAYERS;j++) {
				if (od.playernet[j].tcpclients) {
					freesocker(od.playernet[j].tcpclients);
					od.playernet[j].tcpclients=NULL;
				}
//				if (od.playernet[j].alttcpclients) {
//					freesocker(od.playernet[j].alttcpclients);
//					od.playernet[j].alttcpclients=NULL;
//				}
			}
			gameinfoclear(); // tell match server
			if (od.master)
				freesocker(od.master);
//			if (od.altmaster)
//				freesocker(od.altmaster);
			od.master=/*od.altmaster=*/NULL;
			if (!matchcl)
				uninitsocker();
		}
	} else {
//		setloadcallback(loadingcallback);
		od.loadingcount=0;
	}
}

////////////// code main race state ////////////////////////////////////////////////////////
static void cleartext()
{
	int i;
	for (i=0;i<PTEXT_MAXTEXTSTRINGS;i++)
		od.tscoret[i]=0;
}

// time in 1/60 of sec
static void getsctime(int tt,char *str) // 9 chars
{
	int fs,s,m,n=0;
	if (tt<0) {
		n=1;
		tt=-tt;
	}
	if (tt>99*60*60-1)
		tt=99*60*60-1;
	fs=tt%60;
	fs=fs*100/60;
	s=tt/60;
	m=s/60;
	s%=60;
	if (m)
		if (n)
			sprintf(str,"-%2d.%02d.%02d",m,s,fs);
		else
			sprintf(str," %2d.%02d.%02d",m,s,fs);
	else
		if (n)
			sprintf(str,"   -%2d.%02d",s,fs);
		else
			sprintf(str,"    %2d.%02d",s,fs);
}

static void getsctimes(int tt,char *str) // skip hundredths of second 5 chars
{
	int m,s,n;
	tt=range(0,tt,99*60*60-1);
	n=tt%60;
	s=tt/60;
	m=s/60;
	s%=60;
	sprintf(str,"%2d:%02d.%02d",m,s,n*100/60);
}
//JAY
/*static void getsctimesctf(int tt,char *str) // skip hundredths of second 5 chars
{
	sprintf(str,"%2d",tt);
}
*/
static void olcalcscore(int self)
{
	int i,p;
//	char name[100];
//	char str[50];
	int cid=op->carid;
	if (op->finplace==1) {
		carstubinfos[cid].stunts=range(0,op->ol_utotalstunts,50);
		carstubinfos[cid].score=200;
	} else if (op->finplace==2 && od.numcars>=3) {
		carstubinfos[cid].stunts=range(0,op->ol_utotalstunts,50);
		carstubinfos[cid].score=100;
	} else {
		carstubinfos[cid].stunts=range(0,op->ol_utotalstunts,49);
		carstubinfos[cid].score=0;
	}
	carstubinfos[cid].score+=carstubinfos[cid].stunts;
	carstubinfos[cid].score*=5;
	carstubinfos[cid].credits=carstubinfos[cid].score;
//	onegameinfo.credits=onegameinfo.score/10+1;
//	if (onegameinfo.credits>10)
//		onegameinfo.credits=10;
	carstubinfos[cid].experience=5*carstubinfos[cid].credits;
	mainstubinfo.highestscore=max(mainstubinfo.highestscore,carstubinfos[cid].score);


	if (self) { // you finished the race
		cleartext();
		for (i=0;i<PTEXT_MAXTEXTSTRINGS;i++) {
			od.cscoret[i]=20;
			od.tscoret[i]=10000;
			od.scorestrings[i][0]='\0';
		}
		mainstubinfo.names.credits+=carstubinfos[cid].credits;
		mainstubinfo.names.experience+=carstubinfos[cid].experience;
		mainstubinfo.names.score+=carstubinfos[cid].score;
		mainstubinfo.names.stunts+=carstubinfos[cid].stunts;

		p=range(1,op->finplace,16);
//		getsctime(op->dofinish-INITCANTSTARTDRIVE,str);
//		sprintf(od.scorestrings[1] ,"YOU CAME IN %s",placestrs[p-1]);
//		sprintf(od.scorestrings[2] ,"YOUR TIME");
//		sprintf(od.scorestrings[3] ,"%s",str);
//		sprintf(od.scorestrings[6] ,"STUNTS     %3d",onegameinfo.stunts);
//		sprintf(od.scorestrings[7] ,"YOUR SCORE %3d",onegameinfo.score);
//		sprintf(od.scorestrings[8] ,"HISCORE    %3d",stubinfo.highestscore);
//		sprintf(od.scorestrings[9] ,"EXPERIENCE %3d",stubinfo.experience);
//		sprintf(od.scorestrings[10],"CREDITS    %3d",stubinfo.credits);
//		getsctimes(op->dofinish-INITCANTSTARTDRIVE,str);
		if (op->finplace==1) {
			mainstubinfo.gameswon++;
			sprintf(od.scorestrings[2] ,"    YOU WIN");
		} else
			sprintf(od.scorestrings[2] ,"    YOU LOST");
		mainstubinfo.gamesplayed++;
		logger("gamesplayed = %d, gameswon = %d\n",mainstubinfo.gamesplayed,mainstubinfo.gameswon);
//		if (op->carid==0)
//			strcpy(name,stubinfo.p1name);
//		else if (op->carid==1)
//			strcpy(name,stubinfo.p2name);
//		else
//			sprintf(name,"...%2d...",op->carid);
//		strcpy(name,carstubinfos[cid].name);
//		name[14]='\0';
//		my_strupr(name);
//		mystrncpy(od.name1,name,NAMESIZEI);
//		sprintf(od.scorestrings[4] ,"%14s %s"," ",str);
		sprintf(od.scorestrings[8] ,"    STUNTS %3d",carstubinfos[cid].stunts*5);
		if (op->finplace==1)
			sprintf(od.scorestrings[9] ,"    BONUS 1000");//,carstubinfos[cid].stunts);
		else if (op->finplace==2 && od.numcars>=3)
			sprintf(od.scorestrings[9] ,"    BONUS  500");//,carstubinfos[cid].stunts);
		sprintf(od.scorestrings[10] ,    "    SCORE %4d",carstubinfos[cid].score);
	} //else { // opponent finished the race
//		if (op->carid==0)
//			strcpy(od.opponentname,stubinfo.p1name);
//		else if (op->carid==1)
//			strcpy(od.opponentname,stubinfo.p2name);
//		else
//			sprintf(od.opponentname,"...%2d...",op->carid);
//		strcpy(od.opponentname,carstubinfos[cid].name);
//		od.opponenttime=op->dofinish;
//	}
/*	if (of->dofinish && od.opponenttime) {
//		getsctime(od.opponenttime-INITCANTSTARTDRIVE,str);
//		sprintf(od.scorestrings[4] ,"OPPONENTS");
//		sprintf(od.scorestrings[5] ,"%s",str);
		getsctimes(od.opponenttime-INITCANTSTARTDRIVE,str);
		strcpy(name,od.opponentname);
		name[14]='\0';
		my_strupr(name);
		sprintf(od.scorestrings[5] ,"%14s %s"," ",str);
		mystrncpy(od.name2,name,NAMESIZEI);
//		sprintf(od.scorestrings[5] ,"%14s %s",name,str); */
//	}
//	getsctimes(op->dofinish-INITCANTSTARTDRIVE,str);
//	strcpy(name,carstubinfos[cid].name);
//		name[14]='\0';
//	my_strupr(name);
//	mystrncpy(od.name1,name,NAMESIZEI);
//	sprintf(od.scorestrings[4] ,"%14s %s"," ",str);
}

#define KNOWVAGS
#ifdef KNOWVAGS
static void ol_inittags()
{
	pushandsetdirdown("st2_game");
	od.ol_gardentags=newtagread("gardentag.txt");
//	od.ol_gardentags=tagread("garden.tag");
	popdir();
	pushandsetdirdown("weapsounds_deploy");
	if (fileexist("weapsounds_deploy.txt")) {
		od.ol_weapontags[0]=newtagread("weapsounds_deploy.txt");
	} else {
		od.ol_weapontags[0]=vagread(weapvags,MAX_WEAPKIND);
	}
	popdir();
	pushandsetdirdown("weapsounds_activate");
	if (fileexist("weapsounds_activate.txt")) {
		od.ol_weapontags[1]=newtagread("weapsounds_activate.txt");
	} else {
		od.ol_weapontags[1]=vagread(weapvags,MAX_WEAPKIND);
	}
	popdir();
//	pushandsetdir("weapsounds_result");
//	od.ol_weapontags[2]=vagread(weapvags,MAX_WEAPKIND);
//	popdir();
}

void ol_playatagsound(int tagidx,float vol,float pan)
{
	struct soundhandle *sh;
	if (od.intimewarp && od.numcurplayers!=1)
		return;
//	vol=1; // test
	sh = wave_playvol(taggetwhbyidx(od.ol_gardentags,tagidx),1,vol);
	if (sh) {
		setsfxpan(sh,pan);
		sh->isplayasound=1;
//		logger("playatagsound %d\n",tagidx);
	} else
		logger("can't playatagsound %d\n",tagidx);
}

void ol_playatagsound2(int tagidx,int whichtag,float vol,float pan)
{
	struct soundhandle *sh;
	logger("in playavagsound with '%s'\n",weapvags[tagidx]);
	if (!od.ol_weapontags[whichtag])
		return;
	if (od.intimewarp && od.numcurplayers!=1)
		return;
//	vol=1;
	sh = wave_playvol(taggetwhbyidx(od.ol_weapontags[whichtag],tagidx),1,vol);
	if (sh) {
		setsfxpan(sh,pan);
		sh->isplayasound=1;
//		logger("playatagsound %d\n",tagidx);
	} else
		logger("can't playavagsound %d, [%d]\n",tagidx,whichtag);
}

static void ol_exittags()
{
	tagfree(od.ol_gardentags);
	od.ol_gardentags=NULL;
	tagfree(od.ol_weapontags[0]);
	od.ol_weapontags[0]=NULL;
	tagfree(od.ol_weapontags[1]);
	od.ol_weapontags[1]=NULL;
//	tagfree(od.ol_weapontags[2]);
//	od.ol_weapontags[2]=NULL;
//	op->ol_motorsh=NULL;
}

#endif

static float ol_normang(float ang)
{
	if (ang>360)
		ang-=360;
	else if (ang<0)
		ang+=360;
	return ang;
}

//JAY

#define FLAGCOLLECTDIST 2.0f
#if KNOWCTF
static void ol_initflags()
{
	int i;

	od.ol_flagidx = 0;
	for (i=NUMFLAGS; i>0; i--)
	{
		char flagname[32];
		sprintf(flagname,"FLG_flag%.2d.bwo",i);
//		od.ol_flags[od.ol_flagidx].t = findtreenamerec(od.ol_root,flagname);
		od.ol_flags[od.ol_flagidx].t = od.ol_root->findflagname);
		if ( od.ol_flags[od.ol_flagidx].t )
		{
			obj2world(od.ol_flags[od.ol_flagidx].t,&zerov,&od.ol_flags[od.ol_flagidx].pos);
			//logger("found flag %d at %.2f %.2f %.2f\n", i, od.ol_flags[od.ol_flagidx].pos.x, od.ol_flags[od.ol_flagidx].pos.y, od.ol_flags[od.ol_flagidx].pos.z);
			makeaflyweap(DETACH_FLAG,-1,&od.ol_flags[od.ol_flagidx].pos,&zerov,&zerov,0,od.ol_flagidx);
			od.ol_flags[od.ol_flagidx].t->flags |= TF_DONTDRAW;
			//unhooktree(od.ol_flags[od.ol_flagidx].t);
			od.ol_flagidx++;
		}
	}
	od.ol_flagidx--;

	logger("found %d flags total\n", od.ol_flagidx + 1);
}
#define FLAGCOLLECTDIST 2.0f
/*
static void ol_flagcollision()
{
	float distsq;

	if (op->dofinish) return;
	if (od.ol_flagidx < 0) return;

	if (od.ol_flags[od.ol_flagidx].t) {
		distsq=dist3dsq(&od.ol_flags[od.ol_flagidx].pos,&op->pos);
		if (distsq<FLAGCOLLECTDIST*FLAGCOLLECTDIST) {
			if (od.ol_flagidx >= 0) od.ol_flags[od.ol_flagidx].t->flags |= TLF_DONTDRAW;
			od.ol_flagidx--;
			if (od.ol_flagidx >= 0) if (od.ol_flagidx >= 0) od.ol_flags[od.ol_flagidx].t->flags &= ~TLF_DONTDRAW;
		}
	}
}
*/
#endif
static textureb *uniquetex(tree2 *t,int x,int y,int fmt)
{
	mater2 *mt;
	textureb *tx;
	char str[100],name[100];
	if (!t->mod)
		errorexit("can't find model for unique tex\n");
	sprintf(str,"U_%s",t->mod->name.c_str());
//	spgmystrncpy(t->mod->name,str,NAMESIZEI);
	t->mod->name=str;
	mt=&t->mod->mats[0];
	tx=mt->thetexarr[0];
	if (!tx)
		errorexit("can't find tex for unique tex\n");
	if (!x) {
		sprintf(str,"U_%s",tx->name.c_str());
//		mystrncpy(tx->name,str,NAMESIZEI);
		tx->name=str;
	} else {
		strcpy(name,tx->name.c_str());
//		freetexture(tx);
		textureb::rc.deleterc(tx);
//		mt->thetexarr[0]=tx=buildtexture(name,x,y,fmt);
		mt->thetexarr[0]=texture_create(name);
		sprintf(str,"U_%s",tx->name.c_str());
//		mystrncpy(tx->name,str,NAMESIZEI);
		tx->name=str;
	}
	return tx;
}

/*static void copyplayerinfo(struct playeropt *po,struct ol_playerdata *pd)
{
	pd->cartype=po->carid;
	pd->cntl=po->cntl;
	pd->playerid=po->playerid;
	if (od.playerid==pd->playerid && olracecfg.forcebotmode)
		pd->cntl=CNTL_AI;
}
*/
static void setupaistunts()
{
// precalculate stunt times for various stunts
	int i,j[3];
	j[0]=j[1]=j[2]=0;
	for (i=0;i<MAXAISTUNTS;i++) {
		op->stunttimes[i].x=99999.0f;
		op->stunttimes[i].y=99999.0f;
		op->stunttimes[i].z=99999.0f;
	}
	for (i=0;i<300;i++) {
		op->ol_ucarrotvel.x += op->ol_uthiscarpitch;
		op->ol_ucarrotvel.y += op->ol_uthiscaryaw;
		op->ol_ucarrotvel.z += op->ol_uthiscarroll;
		op->ol_carstuntrpy.x+=op->ol_ucarrotvel.x/2;
		op->ol_carstuntrpy.y+=op->ol_ucarrotvel.y/2;
		op->ol_carstuntrpy.z+=op->ol_ucarrotvel.z/2;
		op->ol_ucarrotvel.x-=op->ol_ucarrotvel.x/op->ol_defaultjumpfriction/2;
		op->ol_ucarrotvel.y-=op->ol_ucarrotvel.y/op->ol_defaultjumpfriction/2;
		op->ol_ucarrotvel.z-=op->ol_ucarrotvel.z/op->ol_defaultjumpfriction/2;
//		logger("aistunt yaw, s %7.3f, v %7.3f, a %7.3f, f %7.3f\n",
//			op->ol_carstuntrpy.y,op->ol_ucarrotvel.y,op->ol_uthiscarroll,op->ol_defaultjumpfriction);
		if (op->ol_carstuntrpy.z>3 && op->ol_carstuntrpy.z<180)
			op->ol_carstuntrpy.z-=2;
		else if (op->ol_carstuntrpy.z<357 && op->ol_carstuntrpy.z>180)
			op->ol_carstuntrpy.z+=2;
		else if (op->ol_carstuntrpy.z>180) // 358 to 359
			op->ol_carstuntrpy.z=360;
		else // 0 to 2
			op->ol_carstuntrpy.z=0;
		if (j[0]<MAXAISTUNTS && op->ol_carstuntrpy.x>=360) {
//			logger("pitch %d at %d\n",j[0],i);
			op->ol_carstuntrpy.x-=360;
			op->stunttimes[j[0]++].x=(float)i;
		}
		if (j[1]<MAXAISTUNTS && op->ol_carstuntrpy.y>=360) {
//			logger("yaw %d at %d\n",j[1],i);
			op->ol_carstuntrpy.y-=360;
			op->stunttimes[j[1]++].y=(float)i;
		}
		if (j[2]<MAXAISTUNTS && op->ol_carstuntrpy.z>=360) {
//			logger("roll %d at %d\n",j[2],i);
			op->ol_carstuntrpy.z-=360;
			op->stunttimes[j[2]++].z=(float)i;
		}
	}
	for (i=0;i<MAXAISTUNTS;i++) {
		op->stunttimes[i].x*=od.ol_timeincconst;
		op->stunttimes[i].y*=od.ol_timeincconst;
		op->stunttimes[i].z*=od.ol_timeincconst;
	}
}

static int suffixstr(const char *name,char *suff)
{
	int len,sufflen;
	len=strlen(name);
	sufflen=strlen(suff);
	if (len>=sufflen)
		if (!my_stricmp(suff,&name[len-sufflen]))
			return 1;
	return 0;
}

static tree2 *findsuffix(tree2 *t,char *tirename)
{
	tree2 *r;
//	int i;
//	int len,len2;
//	len2=strlen(tirename);
//	len=strlen(t->name);
//	if (len>=len2)
//		if (!my_stricmp(tirename,&t->name[len-len2]))
//			return t;
//	if (t->flags&TF_KILLMEC)
//		return NULL;
//	if (suffixstr(t->name.c_str(),tirename))
//		return t;
	U32 p=t->name.find(tirename);
	if (p!=string::npos)
		return t;
//	S32 tnchildren=t->children.size();
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it) {
//	for (i=0;i<tnchildren;i++) {
//		r=findsuffix(t->children[i],tirename);
		r=findsuffix(*it,tirename);
		if (r)
			return r;
	}
	return NULL;
}

static tree2 *findshinymodel(tree2 *t)
{
	tree2 *r;
//	int i;
//	int len,len2;
//	len2=strlen(tirename);
//	len=strlen(t->name);
//	if (len>=len2)
//		if (!my_stricmp(tirename,&t->name[len-len2]))
//			return t;
//	if (t->flags&TF_KILLMEC)
//		return NULL;
	if (suffixstr(t->name.c_str(),".bwo") && !my_strnicmp(t->name.c_str(),"mdl",3))
		return t;
//	for (i=0;i<t->nchildren;i++) {
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it) {
//		r=findshinymodel(t->children[i]);
		r=findshinymodel(*it);
		if (r)
			return r;
	}
	return NULL;
}

static tree2 *findshinywheel(tree2 *sc,int wn)
{
	char str[80];
	tree2 *r;
//	int i;
//	int len,len2;
//	len2=strlen(tirename);
//	len=strlen(t->name);
//	if (len>=len2)
//		if (!my_stricmp(tirename,&t->name[len-len2]))
//			return t;
//	if (sc->flags&TF_KILLMEC)
//		return NULL;
	strcpy(str,sc->name.c_str());
	my_strlwr(str);
	if (strstr(str,shinywheelnames[wn]))
		return sc;
//	for (i=0;i<sc->nchildren;i++) {
	list<tree2*>::iterator it;
	for (it=sc->children.begin();it!=sc->children.end();++it) {
//		r=findshinywheel(sc->children[i],wn);
		r=findshinywheel(*it,wn);
		if (r)
			return r;
	}
	return NULL;
}

// return keep, selects regpoint or -1 if none
static int getweapuserinfo(tree2 *t,int *regidx)
{
	int i;
	char tokstr[100];
	char *tok;
	*regidx=-1;
//	if (!strncmp(t->name,"mdl",3)) // throw out mdl's
//		return 0;
//	if (!strncmp(t->name,"dum",3)) // throw out dum's
//		return 0;
//	if (!strncmp(t->name,"tp",2)) // throw out tp's
//		return 0;
// keep grp's
	S32 tnuserprop=t->userprop.num();
	for (i=0;i<tnuserprop;i++) // look for partlinkinf
		if (!strncmp(t->userprop.idx(i).c_str(),"partLinkInf",11))
			break;
	if (i==tnuserprop)
		return 1;
	strcpy(tokstr,&t->userprop.idx(i).c_str()[12]);
	tok=strtok(tokstr,",\n\r ");
	for (i=0;i<REGPOINT_NREGPOINTS;i++)
		if (!my_stricmp(tok,regpointnames[i]))
			break;
	if (i!=REGPOINT_NREGPOINTS) {
		*regidx=i;
		logger("weaponlinkinfo '%s' linked to '%s'\n",t->name.c_str(),regpointnames[i]);
	}
	return 1;
}

// calc left klondyke given right
static void getklondyke1()
{
	od.klondyke1boxmin.x=-od.klondyke0boxmax.x;
	od.klondyke1boxmin.y=od.klondyke0boxmin.y;
	od.klondyke1boxmin.z=od.klondyke0boxmin.z;
	od.klondyke1boxmax.x=-od.klondyke0boxmin.x;
	od.klondyke1boxmax.y=od.klondyke0boxmax.y;
	od.klondyke1boxmax.z=od.klondyke0boxmax.z;
}

void fixuppart(tree2 *prt) //,int af)
{
//	int i;
	seq_stop(prt);
//	prt->proc=NULL;
	seq_setframe(prt,0);
//	prt->dissolve=1;
	prt->treecolor.w=1;
//	if (af)
//		prt->flags|=TF_ALWAYSFACING;
//	for (i=0;i<prt->nchildren;i++)
	list<tree2*>::iterator it;
	for (it=prt->children.begin();it!=prt->children.end();++it)
//		fixuppart(prt->children[i]); //,af);
		fixuppart(*it);
}

#if 0
static char *getassoc(char **script,int nscript,char *key)
{
	int i;
	if (nscript%1)
		errorexit("can't assoc with an odd # of tokens");
	for (i=0;i<nscript>>1;i++)
		if (!strcmp(script[(i<<1)],key))
			return script[(i<<1)+1];
//	errorexit("can't find '%s' in assoc file\n",key);
	return NULL;
}
#endif
static const char *getassoc(const script& sc,const char *key)
{
	int i;
	S32 nscript=sc.num();
	if (nscript%1)
		errorexit("can't assoc with an odd # of tokens");
	for (i=0;i<nscript>>1;i++)
		if (sc.idx(i<<1)==key)
			return sc.idx((i<<1)+1).c_str();
//	errorexit("can't find '%s' in assoc file\n",key);
	return NULL;
}

void loaddetach(int wk)
{
	char str[100];
	const char* s;
	tree2 *scn,*prt;
	if (detachweapinfos[wk].mastertree)
		return;
//	s=getassoc(od.detachweaplist,od.ndetachweaplist,detachweapinfos[wk].name);
	s=getassoc(*od.detachweaplist,detachweapinfos[wk].name);
	if (s) {
		sprintf(str,"%s.bws",detachweapinfos[wk].name);
pushandsetdirdown(detachweapinfos[wk].name);
//		scn=new tree2(str);
		scn=new tree2(str);
popdir();
//		prt=findtreenamerec(scn,s);
		prt=scn->find(s);
		if (prt) {
//			detachweapinfos[wk].mastertree=duptree(prt);
			detachweapinfos[wk].mastertree=prt->newdup();
//			freetree(scn);
			delete scn;
			fixuppart(detachweapinfos[wk].mastertree); //,weapalwaysfacing[wk]); // get rid of animation for now...
		} else
			detachweapinfos[wk].mastertree=0;//errorexit("can't find part '%s'",s);
	}
}

// load up some better art
void loadnewdetach(int wk)
{
	tree2 *scn,*prt;
	const char *s;
//	char **sc;
//	int nsc;
	char str[100];
	if (detachweapinfos[wk].mastertree) {
//		freetree(detachweapinfos[wk].mastertree);
		delete detachweapinfos[wk].mastertree;
		detachweapinfos[wk].mastertree=NULL;
	}
pushandsetdirdown("onlineopt");
//	sc=loadscript("newdetachweaps.txt",&nsc);
	script sc("newdetachweaps.txt");
popdir();

//	s=getassoc(sc,nsc,detachweapinfos[wk].name);
	s=getassoc(sc,detachweapinfos[wk].name);
	if (s) {
pushandsetdirdown("newdetach");
		sprintf(str,"%s.bws",s);
		scn=new tree2(str);
popdir();
		sprintf(str,"%s.bwo",s);
//		prt=findtreenamerec(scn,str);
		prt=scn->find(str);
		if (prt) {
			detachweapinfos[wk].mastertree=prt->newdup();//duptree(prt);
//			freetree(scn);
			fixuppart(detachweapinfos[wk].mastertree); //,weapalwaysfacing[wk]); // get rid of animation for now...
//		} else if (scn->nchildren>0) {
//			prt=scn->children[0];
//			detachweaps[wk]=duptree(prt);
//			freetree(scn);
//			fixuppart(detachweaps[wk]); //,weapalwaysfacing[wk]); // get rid of animation for now...
		} else
			errorexit("can't find part '%s'",str);//detachweapinfos[wk].mastertree=0;
		delete scn;
	} else {
		errorexit("can't find detach '%s'",detachweapinfos[wk].name);//detachweapinfos[wk].mastertree=0;
	}
//	freescript(sc,nsc);
}

void zerotrans(tree2 *t)
{
//	int i;
	t->trans=zerov;
//	t->proc=NULL;
//	for (i=0;i<t->nchildren;i++)
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it)
//		zerotrans(t->children[i]);
		zerotrans(*it);
}

textureb *getcartex(tree2 *t)
{
//	int i;
	textureb *tx;
	if (t->mod) {
		tx=t->mod->mats[0].thetexarr[0];
		if (tx)
			return tx;
	}
//	for (i=0;i<t->nchildren;i++) {
//		tx=getcartex(t->children[i]);
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it) {
		tx=getcartex(*it);
		if (tx)
			return tx;
	}
	return NULL;
}

static struct bitmap32 *darkentex32(struct bitmap32 *b)
{
	C32 *s,*d,v;
	int prod=b->size.x*b->size.y;
	struct bitmap32 *r;
	r=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
	s=b->data;
	d=r->data;
	while(prod--) {
		v=*s++;
		v.r>>=1;
		v.g>>=1;
		v.b>>=1;
		*d++=v;
	}
	return r;
}

static struct bitmap32 *changecolortex32(struct bitmap32 *b,U32 i,U32 n)
{
	C32 *s,*d,cv;
	int prod=b->size.x*b->size.y;
	struct bitmap32 *r;
	r=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
	s=b->data;
	d=r->data;
	while(prod--) {
		cv=*s++;
		S32 r,g,b,h,s,v;
		r=cv.r;
		g=cv.g;
		b=cv.b;
		rgb2hsv(r,g,b,&h,&s,&v);
		h += i*360/n;
		h %= 360;
		hsv2rgb(h,s,v,&r,&g,&b);
		cv.r=r;
		cv.g=g;
		cv.b=b;
		*d++=cv;
	}
	return r;
}
/*
static struct bitmap16 *darkentex555(struct bitmap16 *b)
{
	unsigned short *s,*d,v;
	int prod=b->x*b->y;
	struct bitmap16 *r;
	r=bitmap16alloc(b->x,b->y,-1);
	s=b->plane;
	d=r->plane;
	while(prod--) {
		v=*s++;
		v&=~0xc63;
		v>>=2;
		if (v==0)
			v=1;
		*d++=v;
	}
	return r;
}
*/
static void slotify(tree2* t,S32 slt) // make models unique for cars
{
//	logger("slotify treename '%s', slot %d\n",t->name.c_str(),slt);
	list<tree2*>::iterator it;
	if (t->mod && !t->isedge)
		t->mod->name=unique();
	for(it=t->children.begin();it!=t->children.end();++it)
		slotify(*it,slt);
}

static tree2* makeashadow()
{
	modelb* mod2=model_create("ashadow");
	if (model_getrc(mod2)==1) {
pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,4,2,1,1,planexz_surf(65,95),"carshadow.tga","tex");
		if (mod2->mats.size()) {
			mod2->mats[0].color=pointf3x(1,1,1,1);
		}
popdir();
	}
	tree2* spheretree2=new tree2("ashadow");
	spheretree2->setmodel(mod2);
	mod2->mats[0].msflags|=SMAT_SHADOW;
	return spheretree2;
}

//#define HUMMER
#define NEWCAR
static void ol_st2_buildcar(int slot)
//static void ol_st2_buildcar(struct playeropt *po)
{
	char str[80];
//	struct ol_playerdata def;	// default settings
//	tree2 *mainobj;//*onlinecar,
//	struct mater2 *mt,*mt2;
	int i,j;
	list<tree2*>::iterator it;
// find best texture for animated textures
//	def=ocp;
//	copyplayerinfo(po,op);
#ifdef HUMMER
pushandsetdirdown("2009_hummer");
	op->y2009car=new tree2("td_hummeroffrd.bws");
	tree2* cb=op->y2009car->find("td_HummerOffRd.bwo");
	if (!cb)
		errorexit("can't find car body");
	textureb* origtex=texture_create("HUMOFFRD.TGA"); // should already be loaded
	cb->alttex=texture_create(unique()); // each car gets a unique texture
	od.cartex2[op->carid]=cb->alttex; // remember
	bitmap32* bm=locktexture(origtex); // copy orig to uniq
//	od.cartexsave2[op->carid]=bitmap32alloc(bm->size.x,bm->size.y,0);
//	clipblit32(bm,od.cartexsave2[op->carid],0,0,0,0,bm->size.x,bm->size.y);
	od.cartexsave2[op->carid]=changecolortex32(bm,op->carid,od.numcars);
	unlocktexture(origtex);
	cb->alttex->addbitmap(od.cartexsave2[op->carid],0); // can't add a locked bitmap!, hence 'bm2'
//	bitmap32free(bm2);
	textureb::rc.deleterc(origtex); // cleanup
popdir();
#endif
#ifdef NEWCAR
#define NO2009 // move over to c_body
#define USECARCLASS
#ifdef NO2009 // read car from c_body
	S32 idx=0;
	for (i=0;i<newcarnames->num();++i) {
		if (!my_stricmp(carstubinfos[op->carid].cd.c_body,newcarnames->idx(i).c_str())) {
			idx=i;
			break;
		}
	}
	const C8* cn=newcarnames->idx(idx).c_str();
#ifdef USECARCLASS
//	op->y2009car=new tree2("ho");
	carclass *cc=new carclass(cn);
//	hsv hsvs[NUMHSVS];
//	if (slot==0)
//		memset(hsvs,0,sizeof(hsvs));
//	else
//		memset(hsvs,0xff,sizeof(hsvs));
//	cc->changehsv(hsvs);
	cc->changehsv(carstubinfos[op->carid].cd.c_hsvs);
	cc->changerims(carstubinfos[op->carid].cd.c_rims);
	op->y2009car=cc->getcartree_passownership();
	slotify(op->y2009car,slot); // make all car models unique
	delete cc;
#else
	C8 newcardir[200];
	C8 newcarbws[200];
	sprintf(newcardir,"2009_cars/%s",cn);
	sprintf(newcarbws,"%s.bws",cn);
pushandsetdirdown(newcardir);
	op->y2009car=new tree2(newcarbws);
popdir();
#endif
#else
	S32 nnewcar=newcarnames->num();
	if (nnewcar==0)
		errorexit("bad thecars.txt");
	S32 idx=(slot+ol_opt.car2009first) % nnewcar;
	const C8* cn=newcarnames->idx(idx).c_str();
	C8 newcardir[200];
	C8 newcarbws[200];
	sprintf(newcardir,"2009_cars/%s",cn);
	sprintf(newcarbws,"%s.bws",cn);
pushandsetdirdown(newcardir);
	op->y2009car=new tree2(newcarbws);
popdir();
#endif

// find regpoints
	tree2* rp=0;
/*	for (it=op->y2009car->children.begin();it!=op->y2009car->children.end();++it) {
		if ((*it)->name.find("regpoints.bwo")!=string::npos) {
			rp=*it;
			break;
		}
	} */
	rp=op->y2009car->findsome("regpoints.bwo");
	if (!rp)
		errorexit("can't find regpoints for '%s'",op->y2009car->name.c_str());
	pointf3 y2009regpoints[REGPOINT_NREGPOINTS];
	bool y2009regpointshave[REGPOINT_NREGPOINTS];
	fill(y2009regpointshave,y2009regpointshave+REGPOINT_NREGPOINTS,false);
//	tree2* tregpoints[REGPOINT_NREGPOINTS];
//	fill(tregpoints,tregpoints+REGPOINT_NREGPOINTS,(tree2*)0);
//	shinyregpointnames
	for (it=rp->children.begin();it!=rp->children.end();++it) {
		tree2* rpt=*it;
		for (j=0;j<REGPOINT_NREGPOINTS;++j) {
			if (rpt->name.find(shinyregpointnames[j])!=string::npos) {
				y2009regpoints[j]=rpt->trans;
				y2009regpointshave[j]=true;
				break;
			}
		}
	}
	j=0;
	for (i=0;i<REGPOINT_NREGPOINTS;++i) {
		if (y2009regpointshave[i]) {
			logger("regpoint '%s' is %f,%f,%f\n",shinyregpointnames[i],y2009regpoints[i].x,y2009regpoints[i].y,y2009regpoints[i].z);
			++j;
		} else {
			logger("missing regpoint '%s'\n",shinyregpointnames[i]);
		}
	}
	if (j!=REGPOINT_NREGPOINTS)
		errorexit("missing some regpoints in '%s', see logfile.txt for details",op->y2009car->name.c_str());
// done find regpoints

	if (videoinfodx9.tooningame) {
/*		tree2* modl2=op->y2009car->newdup();
		edgeify(modl2);
		modl2->trans=pointf3x();
		modl2->rot=pointf3x();
		modl2->scale=pointf3x(1,1,1,1);
		modl2->rotvel=pointf3x();
		op->y2009car->linkchild(modl2); */
#ifndef USECARCLASS
		edgeify(op->y2009car); // carclass has already edgeified the car
#endif
	}

/*	tree2* cb=op->y2009car->find("td_HummerOffRd.bwo");
	if (!cb)
		errorexit("can't find car body");
	textureb* origtex=texture_create("HUMOFFRD.TGA"); // should already be loaded
	cb->alttex=texture_create(unique()); // each car gets a unique texture
	od.cartex2[op->carid]=cb->alttex; // remember
	bitmap32* bm=locktexture(origtex); // copy orig to uniq
//	od.cartexsave2[op->carid]=bitmap32alloc(bm->size.x,bm->size.y,0);
//	clipblit32(bm,od.cartexsave2[op->carid],0,0,0,0,bm->size.x,bm->size.y);
	od.cartexsave2[op->carid]=changecolortex32(bm,op->carid,od.numcars);
	unlocktexture(origtex);
	cb->alttex->addbitmap(od.cartexsave2[op->carid],0); // can't add a locked bitmap!, hence 'bm2'
//	bitmap32free(bm2);
	textureb::rc.deleterc(origtex); // cleanup */
//popdir();
#endif
	op->y2009car->trans=od.y2009cartrans1;
	op->y2009car->rot=od.y2009carrot;
	op->y2009car->scale=pointf3x(od.y2009carscale,od.y2009carscale,od.y2009carscale);
	op->ol_carnull=new tree2("car_main_null");//alloctree(40,ol_nullname);
	op->ol_carnull->linkchild(op->y2009car);

	obj2worldn(rp,y2009regpoints,y2009regpoints,REGPOINT_NREGPOINTS);
	delete rp;
/*	ho->setlinger(true);
	for (i=0;i<REGPOINT_NREGPOINTS;++i)
		ho->addsphere(op->ol_carnull,y2009regpoints[i],.02f,F32WHITE);
	ho->setlinger(false); */

/*	for (i=0;i<REGPOINT_NREGPOINTS;++i) {
		C8 str[50];
		sprintf(str,"tregpoint_%d",i);
		tregpoints[i]=new tree2(str);
		tregpoints[i]->trans=y2009regpoints[i];
		op->ol_carnull->linkchild(tregpoints[i]);
	}
*/
//	strcpy(op->ol_carnull->name,"car_main_null");
	op->ol_carnull->buildo2p=O2P_FROMTRANSQUATSCALE;
	op->ol_carcenternull=new tree2(ol_nullname);//alloctree(1,ol_nullname);
	if (currule->useweap==0) {
		carstubinfos[op->carid].cd.c_nweap=0;
	}
	if (od.klondykeshowbox) {
pushandsetdir("helperobj");
		op->klondykebox0=new tree2("cornerbox1.mxs");
		op->klondykebox1=duptree(op->klondykebox0);
		linkchildtoparent(op->klondykebox0,op->ol_carnull);
		linkchildtoparent(op->klondykebox1,op->ol_carnull);
popdir();
	}
	if (od.buzzshowbox) {
pushandsetdir("helperobj");
		op->buzzbox0=new tree2("cornerbox1.mxs");
//		op->klondykebox1=duptree(op->klondykebox0);
		linkchildtoparent(op->buzzbox0,op->ol_carnull);
//		linkchildtoparent(op->klondykebox1,op->ol_carnull);
popdir();
	}
	if (od.bumpershowbox) {
pushandsetdir("helperobj");
		op->bumperbox0=new tree2("cornerbox1.mxs");
//		op->klondykebox1=duptree(op->klondykebox0);
		linkchildtoparent(op->bumperbox0,op->ol_carnull);
//		linkchildtoparent(op->klondykebox1,op->ol_carnull);
popdir();
	}
	if (od.forkshowbox) {
pushandsetdir("helperobj");
		op->forkbox0=new tree2("cornerbox1.mxs");
//		op->klondykebox1=duptree(op->klondykebox0);
		linkchildtoparent(op->forkbox0,op->ol_carnull);
//		linkchildtoparent(op->klondykebox1,op->ol_carnull);
popdir();
	}
	getklondyke1();
//	usescnbones=0;
	op->cartype=op->carid+6;
	op->ol_carbody=new tree2(ol_allcars[op->cartype].carname);
//	usescnbones=1;
//	xxx=findtreenamerec(op->ol_carbody,ol_allcars[op->cartype].mainobject);
//	linkchildtoparent(op->ol_carbody,op->ol_carcenternull);
	op->ol_carcenternull->linkchild(op->ol_carbody);
//	linkchildtoparent(op->ol_carcenternull,op->ol_carnull);
	op->ol_carnull->linkchild(op->ol_carcenternull);
//	linkchildtoparent(op->ol_carnull,od.ol_root);
	od.ol_root->linkchild(op->ol_carnull);
#if 0
/*	if (op->cartype==7) { // for the new boonyn
		op->ol_uaxles[0]=findtreenamerec(op->ol_carbody,"boonyntirebl.bwo");
		op->ol_uaxles[1]=findtreenamerec(op->ol_carbody,"boonyntirebr.bwo");
		op->ol_uaxles[2]=findtreenamerec(op->ol_carbody,"boonyntirefl.bwo");
		op->ol_uaxles[3]=findtreenamerec(op->ol_carbody,"boonyntirefr.bwo");
//		setVEC(&op->ol_carnull->scale,56,56,56);
		op->ol_carnull->proc=NULL;
	} else { */
		op->ol_uaxles[0]=findtreenamerec(op->ol_carbody,"frntl");
		op->ol_uaxles[1]=findtreenamerec(op->ol_carbody,"frntr");
		op->ol_uaxles[2]=findtreenamerec(op->ol_carbody,"backl");
		op->ol_uaxles[3]=findtreenamerec(op->ol_carbody,"backr");
//	}
#endif
// find car bones
#if 0
	for(j=0;j<COLBONESX*COLBONESZ;j++) {
		op->db_t[0][j] = findtreenamerec(op->ol_carbody,ol_dembonesc[0][j].name);
		if(!op->db_t[0][j]) errorexit("can't find bone %s",ol_dembonesc[0][j].name);
			op->db_curpushin[0][j]=zerov;
	}
#endif
// handle special (lightning around car), turn of visibility
#if 0
	op->ol_uplayspecial1=findtreenamerec(op->ol_carbody,ol_allcars[op->cartype].spname);
	if(!op->ol_uplayspecial1)
		errorexit("Error can't find %s",ol_allcars[op->cartype].spname);
	op->ol_uplayspecial1->flags|=TF_DONTDRAW;

	op->ol_uplayspecial2=findtreenamerec(op->ol_carbody,ol_allcars[op->cartype].coinsp);
	if(!op->ol_uplayspecial2)
		errorexit("Error can't find %s",ol_allcars[op->cartype].coinsp);
	op->ol_uplayspecial2->flags|=TF_DONTDRAW;
#endif

pushandsetdirdown("st2_game");
	op->ol_uplayflame1=alloctree(0,"flame.lwo");
	op->ol_uplayflame2=alloctree(0,"flamflip.lwo");
//	op->ol_uplayflame1->treealphacutoff=5;
//	op->ol_uplayflame2->treealphacutoff=5;
	op->ol_uplayflame1->flags|=TF_DONTDRAW;
	op->ol_uplayflame2->flags|=TF_DONTDRAW;

	if (!od.ol_specialtsp) {
// blue lightning
		if (!od.ol_slowpo)
//			od.ol_specialtsp=loadtspo("trkglow.txt","trkglowa.txt",od.texfmt,1);
			od.ol_specialtsp=loadtspo("trkglow.txt",NULL,od.texfmt,1);
//				op->ol_uplayspecial1,ol_allcars[op->cartype].spname,"trkglow2",TSP_USE_4ALPHA,0);
	}
#if 0
	// car fireworks
	op->ol_burstframe=0;
	op->ol_ubursta = alloctree(1,ol_burstlwo[0]);
	for (j=0;j<4;j++) {
		if (!od.ol_bursttsp[j])
			od.ol_bursttsp[j]=loadtspo(ol_burstnames[j],NULL,od.texfmt,1);
//			od.ol_bursttsp[j]=loadtspo(ol_burstnames[j],"bursta.txt",od.texfmt,1);
//			op->ol_ubursta[j],ol_burstlwo[j],ol_burstmats[j],TSP_USE_4ALPHA,0);
	}
	op->ol_ubursta->scale.x=.01f;
	op->ol_ubursta->scale.y=.01f;
	op->ol_ubursta->scale.z=.01f;
	op->ol_ubursta->trans.z=.14f;
	op->bursttex=uniquetex(op->ol_ubursta,od.ol_bursttsp[0]->x,od.ol_bursttsp[0]->y,od.texfmt);
	op->ol_uburstb=duptree(op->ol_ubursta);
	op->ol_uburstb->rot.y=180*PIOVER180;
	linkchildtoparent(op->ol_ubursta,op->ol_carnull);
	linkchildtoparent(op->ol_uburstb,op->ol_carnull);

// golden condom
	if (!od.ol_condomtsp)
		od.ol_condomtsp=loadtspo("wipe.txt",NULL,od.texfmt,1);
//		od.ol_condomtsp=loadtspo("wipe.txt","wipea.txt",od.texfmt,1);
//			op->ol_carbody,ol_allcars[op->cartype].coinsp,"flash",TSP_USE_4ALPHA,0);

// flame from wheels
	if (!od.ol_flametsp)
		od.ol_flametsp=loadtspo("flame.txt",NULL,od.texfmt,1);
//		od.ol_flametsp=loadtspo("flame.txt","flamea.txt",od.texfmt,1);
//			op->ol_uplayflame1,"flame.lwo","flame",TSP_USE_4ALPHA,0);
	op->flametex=uniquetex(op->ol_uplayflame1,od.ol_flametsp->x,od.ol_flametsp->y,od.texfmt);
	uniquetex(op->ol_uplayflame2,od.ol_flametsp->x,od.ol_flametsp->y,od.texfmt);
	mt=findmaterial(op->ol_uplayflame1,"flame");
	mt2=findmaterial(op->ol_uplayflame2,"flamflip");
	freetexture(mt2->thetexarr[0]);
//	mt2->thetexarr[0]=mt->thetexarr[0];
//	mt->thetexarr[0]->refcount++;
	mt2->thetexarr[0]=textureb::rc.newrc(mt->thetexarr[0]);
	popdir();

// blue flash
pushandsetdirdown("st2_cars");
//	if (!od.ol_flashtsp)
		op->ol_flashtsp=loadtspo(ol_allcars[op->cartype].tspname,NULL,od.texfmt,1);
//		op->ol_carbody,ol_allcars[op->cartype].mainobject,ol_allcars[op->cartype].surfname,TSP_USE_4ALPHA,0);
	op->ol_nuflashes=1;	// force texture
	op->lightningtex=uniquetex(op->ol_uplayspecial1,od.ol_specialtsp->x,od.ol_specialtsp->y,od.texfmt);
	op->condomtex=uniquetex(op->ol_uplayspecial2,od.ol_condomtsp->x,od.ol_condomtsp->y,od.texfmt);

#endif
#if 1
// build wheels
	for (i=0;i<ol_carinfo[op->cartype].nwheels;i++) {
		tree2 *t;
		op->ol_wheels[i]=findtreenamerec(op->ol_carbody,ol_carinfo[op->cartype].wheelnames[i]);
		t=op->ol_wheels[i];
		op->ol_wheels[i]=alloctree(1,ol_carinfo[op->cartype].wheelnames[i]);
		freetree(t);
		op->ol_wheels[i]->buildo2p=O2P_FROMTRANSQUATSCALE;
		linkchildtoparent(op->ol_wheels[i],op->ol_carnull);
	}

//	linkchildtoparent(op->ol_uplayflame1,op->ol_uaxles[2]);
//	linkchildtoparent(op->ol_uplayflame2,op->ol_uaxles[3]);
	delete(op->ol_uplayflame1);
	delete(op->ol_uplayflame2);
#endif
// build shadow
	op->ol_shadownull=alloctree(1,ol_nullname);
	op->ol_shadownull->buildo2p=O2P_FROMTRANSQUATSCALE;
	op->ol_shadowb=alloctree(1,ol_nullname);
	op->ol_shadowb->buildo2p=O2P_FROMTRANSQUATSCALE;
	op->ol_shadowb->rot.w=1;
	op->ol_shadowbody=makeashadow();
//	op->ol_shadowbody=alloctree(0,ol_allcars[op->cartype].shadowname);
//	op->ol_shadowbody->treealphacutoff=10;
	op->ol_shadowbody->buildo2p=O2P_FROMTRANSQUATSCALE;
//	op->ol_shadowbody->flags|=TF_CALCLIGHTSONCE;
	linkchildtoparent(op->ol_shadowbody,op->ol_shadowb);
	linkchildtoparent(op->ol_shadowb,op->ol_shadownull);
	linkchildtoparent(op->ol_shadownull,od.ol_root);

popdir();

	op->ol_uthiscarpitch = ol_carratings.pitchspeed/3.5f;
	op->ol_uthiscaryaw = ol_carratings.yawspeed/3.5f;
	op->ol_uthiscarroll = ol_carratings.rollspeed/3.5f;
//	ol_loadcustomcarsettings();
// make car unique, by changing names slightly...
#if 0
	mainobj=findtreenamerec(op->ol_carbody,ol_allcars[op->cartype].mainobject);
//	if (mainobj->mod)
//		mainobj->mod->drawpri=1;
	op->flashtex=uniquetex(mainobj,op->ol_flashtsp->x,op->ol_flashtsp->y,od.texfmt);
#endif
	setupaistunts();
// now done with 'uniquetexture' in the loadtsp call
//	if (0) {
	if (op->cartype>=6) {
//	if (op->carid==0)
//		extern int usenewprs;
		int pn,dn;
		pn=atoi(carstubinfos[op->carid].cd.c_paint+5);
		dn=atoi(carstubinfos[op->carid].cd.c_decal+5);
		sprintf(str,"player%d",op->cartype-5);
//		onlinecar=loadonlinecarenatest("player1","player1");
//		onlinecar=loadonlinecarbin(stubinfo.p1body,"player1");
//		onlinecar=loadonlinecarbin("eviltwin","player1");
//		if (!onlinecar)
//			onlinecar=loadonlinecar("player1",1);
//	} else if (op->cartype==7) {
//	else if (op->carid==1)
//		usenewprs=1;

//		op->onlinecar=loadnewjrms2(carstubinfos[op->carid].body,pn,dn);
//		if (op->carid!=od.playerid && !op->onlinecar) // attempt to get your car instead
//			op->onlinecar=loadnewjrms2(,pn,dn);
		op->onlinecar=loadnewjrms2("muscletone",pn,dn);

//		op->onlinecar=loadnewjrms(str,str);

//		op->onlinecar=loadnewjrms("eviltwin","eviltwin");
//		op->shinycar=loadshinycar("mdlcrvulture23r.mxs");
//		sprintf(str,"mdlcr%s.mxs",carstubinfos[op->carid].body);
/*		op->shinycar=loadshinycar(carstubinfos[op->carid].body);
		if (!op->shinycar)
			errorexit("couldn't load '%s'",carstubinfos[op->carid].body);
		if (op->shinycar && video_d3denhanced) {
			op->onlinecar->flags|=TF_DONTDRAWC;
			op->carvis=2;
			op->regpointsoffset=op->regpointsoffsetshiny;
		} else {
			if (op->shinycar)
				op->shinycar->flags|=TF_DONTDRAWC; */
			op->carvis=1;
			op->regpointsoffset=op->regpointsoffsetjrm;
/*		} */
//		usenewprs=0;
//		onlinecar=loadonlinecarenatest("player2","player2");
//		onlinecar=loadonlinecarbin(stubinfo.p2body,"player2");
//		onlinecar=loadonlinecarbin("eviltwin","player2");
//		if (!onlinecar)
//			onlinecar=loadonlinecar("player2",1);
		if (op->onlinecar)
			op->onlinecar->flags|=TF_DONTDRAWC;
	} else {
		op->onlinecar=NULL;
	}
/*	if (op->shinycar)
		linkchildtoparent(op->shinycar,op->ol_carnull); */
	if (op->onlinecar) {
//		pointf3 r;
		struct mat4 regmat;
		tree2 *xxx;
		od.cartex[op->carid]=getcartex(op->onlinecar);
		if (od.cartex[op->carid]) {
			struct bitmap32 *b;
//			logger("cartex %d found, name '%s' lg size %d %d\n",
//				op->carid,od.cartex[op->carid]->name,od.cartex[op->carid]->logu,od.cartex[op->carid]->logv);
			b=locktexture(od.cartex[op->carid]);
			od.cartexsave[op->carid]=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
			clipblit32(b,od.cartexsave[op->carid],0,0,0,0,b->size.x,b->size.y);
			unlocktexture(od.cartex[op->carid]);
//			if (od.cartex[op->carid]->texformat==TEX565NCK)
//				od.cardarktexsave[op->carid]=darkentex565(od.cartexsave[op->carid]);
//			if (od.cartex[op->carid]->texformat==TEX555NCK)
//				od.cardarktexsave[op->carid]=darkentex555(od.cartexsave[op->carid]);
			od.cardarktexsave[op->carid]=darkentex32(od.cartexsave[op->carid]);
		}
		linkchildtoparent(op->onlinecar,op->ol_carnull);

		xxx=findsuffix(op->onlinecar,"regpoints");
		if (!xxx)
			errorexit("can't find onlinecar 'regpoints'");
//		strcpy(xxx->name,"theregpoints");
		xxx->name="theregpoints";
		op->theregpoints=xxx;
//		op->regpoints=alloctree(REGPOINT_NREGPOINTS,NULL);
//		linkchildtoparent(op->regpoints,xxx->parent);
		if (xxx->children.size()!=REGPOINT_NREGPOINTS)
			errorexit("number of regpoints != %d",REGPOINT_NREGPOINTS);
//		r.x=od.regrot.x*PIOVER180;
//		r.y=od.regrot.y*PIOVER180;
//		r.z=od.regrot.z*PIOVER180;
//		buildrottrans3d(&r,&zerov,&regmat);
		for (i=0,it=xxx->children.begin();i<REGPOINT_NREGPOINTS&&it!=xxx->children.end();i++,++it) {
//			extern float helperscale;
//			tree2 *t;
			int nv;
			modelb *m;
			pointf3 center;
//			m=xxx->children[i]->mod;
			m=(*it)->mod;
			if (!m)
				errorexit("no regpoint");
			nv=m->verts.size();//m->nvert;
			if (nv!=4 && nv!=8)
				errorexit("bad regpoint, nvert = %d, name = '%s'",nv,m->name.c_str());
			center=zerov;
			for (j=0;j<nv;j++) {
				center.x+=m->verts[j].x;
				center.y+=m->verts[j].y;
				center.z+=m->verts[j].z;
			}
			center.x/=nv;
			center.y/=nv;
			center.z/=nv;
			buildscalerottrans3d(&op->theregpoints->scale,&op->theregpoints->rot,&op->theregpoints->trans,&regmat);
			xformvec(&regmat,&center,&center);
			buildscalerottrans3d(&op->onlinecar->scale,&op->onlinecar->rot,&op->onlinecar->trans,&regmat);
			xformvec(&regmat,&center,&op->regpointsoffsetjrm[i]);
//			op->regpointsoffset[i]=center;
			logger("regpoint %2d: %f %f %f\n",i,op->regpointsoffsetjrm[i].x,op->regpointsoffsetjrm[i].y,op->regpointsoffsetjrm[i].z);
//			op->regpointsoffset[i].x/=op->ol_uplaycarscale;
//			op->regpointsoffset[i].y/=op->ol_uplaycarscale;
//			op->regpointsoffset[i].z/=op->ol_uplaycarscale;
//			helperscale=.005f;
//			t=allochelper(0,HELPER_NULL,0);
//			helperscale=0;
//			t->trans=op->regpointsoffset[i];
//			linkchildtoparent(t,op->ol_carnull);//>onlinecar);
		}
//		memcpy(op->regpointsoffsetjrm,op->regpointsoffset,sizeof(op->regpointsoffset));
		xxx->flags|=TF_DONTDRAWC;
		freetree(xxx);
	}
/*	if (op->shinycar) {
		struct mat4 regmat;
		tree2 *xxx;
//		op->onlinecar->rot.x=-PI/2;
//		op->isnewcar=1;
//		onlinecar->children[0]->dissolve=.25f;
//		op->onlinecar->trans.y=od.newcaroffy;
//		setVEC(&op->onlinecar->scale,od.newcarscale,od.newcarscale,od.newcarscale);
//		linkchildtoparent(op->onlinecar,op->ol_carnull);





		xxx=findsuffix(op->shinycar,"_regpoints.bwo");
		if (!xxx)
			errorexit("can't find shinycar '_regpoints.bwo'");
		strcpy(xxx->name,"theregpoints");
		op->theregpoints=xxx;
//		op->regpoints=alloctree(REGPOINT_NREGPOINTS,NULL);
//		linkchildtoparent(op->regpoints,xxx->parent);
		if (xxx->nchildren!=REGPOINT_NREGPOINTS) {
			logviewport(&mainvp,OPT_SOME);
			errorexit("number of regpoints != %d",REGPOINT_NREGPOINTS);
		}
//		r.x=od.regrot.x*PIOVER180;
//		r.y=od.regrot.y*PIOVER180;
//		r.z=od.regrot.z*PIOVER180;
//		buildrottrans3d(&r,&zerov,&regmat);
		for (i=0;i<REGPOINT_NREGPOINTS;i++) {
			extern float helperscale;
//			tree2 *t;
			pointf3 center;
			xxx=findsuffix(op->shinycar,shinyregpointnames[i]);
			if (!xxx)
				errorexit("can't find '%s'",shinyregpointnames[i]);
			center=xxx->trans;
			buildscalerottrans3d(&op->theregpoints->scale,&op->theregpoints->rot,&op->theregpoints->trans,&regmat);
			xformvec(&regmat,&center,&center);
			buildscalerottrans3d(&op->shinycar->scale,&op->shinycar->rot,&op->shinycar->trans,&regmat);
			xformvec(&regmat,&center,&op->regpointsoffsetshiny[i]);
//			op->regpointsoffset[i]=center;
			logger("regpointshiny %2d: %f %f %f\n",
				i,op->regpointsoffsetshiny[i].x,op->regpointsoffsetshiny[i].y,op->regpointsoffsetshiny[i].z);
//			op->regpointsoffset[i].x/=op->ol_uplaycarscale;
//			op->regpointsoffset[i].y/=op->ol_uplaycarscale;
//			op->regpointsoffset[i].z/=op->ol_uplaycarscale;
//			helperscale=.005f;
//			t=allochelper(0,HELPER_NULL,0);
//			helperscale=0;
//			t->trans=op->regpointsoffsetshiny[i];
//			linkchildtoparent(t,op->ol_carnull);//>onlinecar);
		}
//		memcpy(op->regpointsoffsetshiny,op->regpointsoffset,sizeof(op->regpointsoffset));
		xxx->flags|=TF_DONTDRAWC;
		freetree(op->theregpoints);
		xxx=findshinymodel(op->shinycar);
		if (xxx && xxx->mod) {
			struct mat *m;
			strcat(xxx->mod->name,"_U");
			m=findmaterial(xxx,"body");
			if (m)
				od.cartex2[op->carid]=m->thetexarr[0];
		}
		if (od.cartex2[op->carid]) {
			struct bitmap16 *b;
			logger("cartex2 %d found, name '%s' lg size %d %d\n",
				op->carid,od.cartex2[op->carid]->name,od.cartex2[op->carid]->logu,od.cartex2[op->carid]->logv);
			b=locktexture(od.cartex2[op->carid]);
			od.cartexsave2[op->carid]=bitmap16alloc(b->x,b->y,-1);
			clipblit32(b,od.cartexsave2[op->carid],0,0,0,0,b->x,b->y);
			unlocktexture(od.cartex2[op->carid]);
			if (od.cartex2[op->carid]->texformat==TEX565NCK)
				od.cardarktexsave2[op->carid]=darkentex565(od.cartexsave2[op->carid]);
			if (od.cartex2[op->carid]->texformat==TEX555NCK)
				od.cardarktexsave2[op->carid]=darkentex555(od.cartexsave2[op->carid]);
			strcat(od.cartex2[op->carid]->name,"_U");
		}
		for (i=0;i<4;i++) {
			op->ol_wheels2[i]=findshinywheel(op->shinycar,i);
			if (!op->ol_wheels2[i])
				errorexit("can't find shiny wheel");
			else
				op->ol_wheels2[i]->proc=NULL;
		}
	} */
	if (op->onlinecar) {
		tree2 *weaptree;
		tree2 *xxx;
		logger("---------- loading car weapons for car %d -------------\n",op->carid);
		for (i=0;i<carstubinfos[op->carid].cd.c_nweap;i++) {
//			char *mn;
//			pointf3 top,bot,intsect;
			int k=carstubinfos[op->carid].cd.c_weaps[i];
			sprintf(str,"2009_weapons/%s",weapinfos[k].name);
//			sprintf(str,"everything/%s",weapinfos[k].name);
pushandsetdirdown(str);
			sprintf(str,"%s.bws",weapinfos[k].name);
			weaptree=NULL;
			if (fileexist(str))
				weaptree=new tree2(str);
			else {
popdir();
pushandsetdirdown("2009_weapons/icrms");
				weaptree=new tree2("icrms.bws");
			}
			if (weaptree==NULL)
				errorexit("can't find '%s'",str);
			linkchildtoparent(weaptree,op->ol_carnull);
			seq_stop(weaptree);
			seq_setframe(weaptree,0);
			if (k==WEAP_BIGTIRES) {
				weaptree->scale=pointf3x(od.bigtirescale,od.bigtirescale,od.bigtirescale);
				char str[100];
				for (j=0;j<4;j++) {
					sprintf(str,"mdPTbigtires_tire0%d.bwo",j+1);
					op->bigtiretrees[j]=findtreenamerec(weaptree,str);
					if (!op->bigtiretrees[j])
						errorexit("can't find '%s'",str);
//					op->bigtireproc=op->bigtiretrees[j]->userproc;
				}
			}
			weaptree->flags|=TF_DONTDRAWC;
//			} else
//				weaptree=NULL;
//			weaptree->flags|=TF_DONTDRAWC;
			op->weaptrees[i]=weaptree;
popdir();
/*			if (!my_stricmp(weapinfos[k].name,"missiles")) {
//				float v;
				tree2 *t;
				t=findtreenamerec(weaptree,"grpPTmissiles.bwo");
				t->proc=NULL;
				t->trans=op->regpointsoffset[REGPOINT_ROOF];
				t->trans.x*=op->ol_uplaycarscale;
				t->trans.y*=op->ol_uplaycarscale;
				t->trans.z*=op->ol_uplaycarscale;
//				unhooktree(t);
//				linkchildtoparent(t,op->regpoints->children[REGPOINT_ROOF]);
//				v=1.0f/od.newcarscale;
//				setVEC(&t->scale,v,v,v);
			} */
// move weapons over to regpoints
			logger("weapon '%s'\n",weaptree->name.c_str());
			list<tree2*>::iterator it,ik;
//			for (j=0;j<weaptree->nchildren;j++) {
			for (it=weaptree->children.begin();it!=weaptree->children.end();) {
				tree2 *t;
				int keep,regidx;
//				t=weaptree->children[j];
				t=*it;
				t->userint[1]=-1;
				logger("weapgroup name '%s'\n",t->name.c_str());
				keep=getweapuserinfo(t,&regidx);
//				if (t->name=="grpPTmagnet_dep.bwo") // magnetic field going out
//					keep=0;
//				if (t->name=="grpPTmagnet_act.bwo") // magnetic field going out (redundant)
//					keep=0;
//				if (t->name=="grpPTmagnet.bwo") // magnets
//					keep=0;
				if (keep) {
					if (regidx>=0) {

						++it;

						C8 str[50];
						sprintf(str,"tregpoint_%d",regidx);
						tree2* nt=new tree2(str);
						ho->setlinger(true);
						ho->addsphere(nt,pointf3x(),.04f,F32WHITE);
						ho->setlinger(false);
						nt->trans=y2009regpoints[regidx];
						nt->scale=pointf3x(od.weapscale,od.weapscale,od.weapscale);
						t->unlink(); // insert a regpoint
						nt->linkchild(t);
						weaptree->linkchild(nt);
//						tregpoints[regidx]->linkchild(t);
//						t->proc=NULL;
//						t->userproc=0;
//						t->trans=zerov;
//						zerotrans(t);
//						t->trans=pointf3x(0,20,0);//op->regpointsoffset[regidx];
//						t->rot=pointf3x();
//						t->scale=pointf3x(1,1,1);
//						t->buildo2p=O2P_NONE;//O2P_FROMTRANSQUATSCALE;
//						t->doanim=false;

						t->userint[1]=regidx;
//						t->trans.x*=od.ol_uplaycarscale;
//						t->trans.y*=od.ol_uplaycarscale;
//						t->trans.z*=od.ol_uplaycarscale;
					} else {
						++it;
					}
				} else {
					ik=it;
					++it;
					delete *ik;
				}
			}
//			}
// just 5 weapons so far have extra detached objects
//				op->missileinfos[1].oldt=xxx=findtreenamerec(op->onlinecar,"mdlPTmissiles_B.bwo");
// setup for tree pool, master detach objects
/*			mn=getassoc(od.detachweaplist,od.ndetachweaplist,detachweapinfos[k].name);
			if (mn) {
				xxx=findtreenamerec(weaptree,mn);
//				if (!xxx)
//					errorexit("can't find '%s' for detach weap",mn);
				if (xxx) {
					if (!detachweaps[k]) {
//					detachweaps[WEAP_MISSILES]=alloctree(0,"mdlPTmissiles_A.bwo"); //	duptree(xxx);
						detachweaps[k]=duptree(xxx);
						fixuppart(detachweaps[k]); //,weapalwaysfacing[k]);
//						setVEC(&detachweaps[k]->scale,10,10,10); // big missiles
					}
					freetree(xxx);
				}
			} */
//				xxx->flags|=TF_DONTDRAWC;
/*				bot=top=op->pos;
				top.y+=2;
				bot.y-=2;
				st2_line2road(&top,&bot,&intsect,NULL);
				intsect.y+=2;
				for (j=0;j<4;j++) {
					op->missileinfos[j].t=duptree(xxx);
					op->missileinfos[j].t->trans=intsect;
					op->missileinfos[j].t->proc=NULL;
					setVEC(&op->missileinfos[j].t->scale,10,10,10); // big missiles
					op->missileinfos[j].active=0;
					op->missileinfos[j].fromcar=op->carid;
					op->missileinfos[j].t->trans.x+=(float)j;
					linkchildtoparent(op->missileinfos[j].t,od.ol_root);
//					op->missileinfos[j].oldt->flags|=TF_DONTDRAWC;
				} */
//				break;
//				xxx->flags|=TF_DONTDRAWC;
//				op->missileinfos[2].oldt=xxx=findtreenamerec(op->onlinecar,"mdlPTmissiles_C.bwo");
/*				xxx=findtreenamerec(weaptree,"mdlPTmissiles_C.bwo");
				if (!xxx)
					errorexit("can't find missile!");
				freetree(xxx);
//				xxx->flags|=TF_DONTDRAWC;
//				op->missileinfos[3].oldt=xxx=findtreenamerec(op->onlinecar,"mdlPTmissiles_D.bwo");
				xxx=findtreenamerec(weaptree,"mdlPTmissiles_D.bwo");
				if (!xxx)
					errorexit("can't find missile!");
				freetree(xxx);
//				xxx->flags|=TF_DONTDRAWC;
//				op->missileinfos[0].oldt=xxx=findtreenamerec(op->onlinecar,"mdlPTmissiles_A.bwo");
*/

		}

// free up extra weapon heirarchy
		for (j=0;j<od.freeweaplist->num();j++) {
			xxx=findtreenamerec(op->ol_carnull,od.freeweaplist->idx(j).c_str());
			if (xxx)
//				errorexit("can't find '%s' to free",od.freeweaplist[j]);
				freetree(xxx);
		}
//		mainobj->flags|=TF_DONTDRAW;
//		if (xxx)
//			freetree(xxx);
		for (i=0;i<4;i++) {
			op->ol_wheels[i]->flags|=TF_DONTDRAW;
//			sprintf(str,"%s",carmodelnames[op->cartype-6],tirenames[i]);
//			sprintf(str,"olwheel%d",i);
//			sprintf(str,"olwheel%d",(i+3)&3);
			op->ol_wheels[i]=findsuffix(op->onlinecar,tirenames[i]);
			if (!op->ol_wheels[i])
				errorexit("can't find wheel '%s'",tirenames[i]);
			op->ol_wheels[i]->buildo2p=O2P_FROMTRANSQUATSCALE;
		}
		for (i=0;i<6;i++) {
			op->ol_wheels2[i]=findsuffix(op->y2009car,y2009tirenames[i]);
//			if (!op->ol_wheels2[i])
//				errorexit("can't find wheel2 '%s'",y2009tirenames[i]);
			if (op->ol_wheels2[i]) {
				op->ol_wheels2[i]->buildo2p=O2P_FROMTRANSQUATSCALE;
				if (op->ol_wheels2[i]->mod)
					op->ol_wheels2[i]->mod->name=unique();
			}
/*			if (video3dinfo.cantoon) {
				tree2* n=op->ol_wheels2[i]->newdup();
				normalify(n);
				op->ol_wheels2[i]->parent->linkchild(n);
			} */
		}
/*		xxx=findtreenamerec(mainobj,"comnsde.lwo");
		if (xxx)
			xxx->flags|=TF_DONTDRAW;
		xxx=findtreenamerec(mainobj,"comref.lwo");
		if (xxx)
			xxx->flags|=TF_DONTDRAW;
		xxx=findtreenamerec(mainobj,"boonref.lwo");
		if (xxx)
			xxx->flags|=TF_DONTDRAW; */
	}
//	ocp=def;	// restore default player settings..
}

/*static int ol_st2_loadtracklist2(char *filename)
{
	int i=0;
	char temp[64];
	int numl;
	char **script;
	int nump=0;
	script=loadscript(filename,&numl);
	while(i<numl) {
		strcpy(od.ol_pieces[nump].name,script[i++]); // grab piece
		od.ol_pieces[nump].piece = NULL;
		while(1) {
			strcpy(temp,script[i++]); // grab changerot OR lwoname
			if (temp[1]=='\0' && temp[0]>='0' && temp[0]<='9') {
				od.ol_pieces[nump].changerot = atoi(temp);
				break;
			} else {
			}
		}
		od.ol_pieces[nump].newor = atoi(script[i++]);
		od.ol_pieces[nump].time = atoi(script[i++]);
		od.ol_pieces[nump].value = atoi(script[i++]);
		od.ol_pieces[nump].ovalue = od.ol_pieces[nump].value;
		//error("piece %d value %d",nump,pieces[nump].value);
		nump++;
	}
	freescript(script,numl);
	return nump;
}
*/
// init
/*static void ol_canfinish(int yo)
{
	if (yo)
		od.ol_nofinishpiece->flags|=TF_DONTDRAW;
	else
		od.ol_nofinishpiece->flags&=~TF_DONTDRAW;
}
*/
/*static void ol_initvisit()
{
	int i;
	memset(ocp.ol_piecesvisited,0,sizeof(ocp.ol_piecesvisited));
	ocp.ol_numpiecesvisited=0;
	od.ol_numpiecestovisit=0;
	for (i=0;i<NUMBODS;i++)
		if (od.ol_thetrack[i].piece!=PIECE_BASE)
			od.ol_numpiecestovisit++;
#if 0
	if (od.ol_numpiecestovisit)
		ol_canfinish(0);
	else
		ol_canfinish(1);
#endif
}

static void ol_addvisit(int x,int z)
{
	int cp,pce;
	if (od.ol_uedittrackname[0]!='\0')
		return;
	cp=x*7+z;
	pce=od.ol_thetrack[cp].piece;
	if (pce==PIECE_GENERIC_4)
		op->ol_intunnels=15;

	if (pce!=PIECE_BASE && op->ol_piecesvisited[cp]==0) {
		op->ol_piecesvisited[cp]=1;
		od.ol_trackscore += od.ol_pieces[pce].value;
		od.ol_pieces[pce].value --;
		if(od.ol_pieces[pce].value < 0) od.ol_pieces[pce].value = 0;
		op->ol_numpiecesvisited++;
//		if (op->ol_numpiecesvisited*100>=od.ol_finishpercent*od.ol_numpiecestovisit)
//			ol_canfinish(1);
	}
}
*/
#if 0
static void ol_setnextcrashloc()
{
//	int	cp=x*7+z;
//	int or;
	pointf3 top,bot,intsect;
/*	if (od.ol_uedittrackname[0]=='\0' || !od.path1) {
		ol_addvisit(x,z);
		if (od.ol_pieceorder[op->ol_curpieceorder]==cp) {
			or=(int)od.ol_thetrack[cp].or;
			op->ol_crashresetloc.x=(float)(x*od.piecesizex-3*od.piecesizex-od.piecesizex*ol_crashresetstruct[(int)or/90].xoff);
			op->ol_crashresetloc.y=od.hipoint;
			op->ol_crashresetloc.z=(float)(3*od.piecesizez-z*od.piecesizez-od.piecesizez*ol_crashresetstruct[(int)or/90].zoff);
			op->ol_crashresetdir=(float)((or+90)*PIOVER180);
			op->ol_curpieceorder++;
		}
	}
	top=bot=op->ol_crashresetloc;
	top.y=2000;
	bot.y=-2000;
	if (st2_line2road(&top,&bot,&intsect,NULL)) {
		op->ol_crashresetloc.y=intsect.y+od.hipoint;
	}
}
*/
#endif
/*static void ol_scantrackpieces()
{
	int bail=0;
	int curdir=0;
	int pl=0;
	int xoff,zoff,cp,pce;
// setup finish line
	od.ol_ulastpiece=od.ol_st2_lastpiece;
	od.ol_udirpiece=od.ol_st2_dir;
	if (od.ol_ulastpiece==-1 || od.ol_thetrack[6].piece==PIECE_BASE || od.ol_thetrack[6].piece==PIECE_GRID) {
		od.ol_udirpiece=0;
		od.ol_ulastpiece=6;
	}
	od.ol_startpiece->trans.x=-3.5f*od.piecesizex;
	od.ol_startpiece->trans.z=-3.0f*od.piecesizez;
	od.ol_startpiece->rot.y=90*PIOVER180;

	xoff=od.ol_ulastpiece/7;
	zoff=od.ol_ulastpiece%7;
	xoff=(int)(od.piecesizex*xoff-3.0f*od.piecesizex);
	zoff=(int)(3.0f*od.piecesizez-zoff*od.piecesizez);
	od.ol_finishpiece->trans.x=(float)(xoff+od.piecesizex*ol_finishstruct[od.ol_udirpiece].xoff);
	od.ol_finishpiece->trans.z=(float)(zoff+od.piecesizez*ol_finishstruct[od.ol_udirpiece].zoff);
	od.ol_finishpiece->rot.y=(float)((90+ol_finishstruct[od.ol_udirpiece].ang)*PIOVER180);
	ol_initvisit();
// build ordered track piece list
	xoff=0;
	zoff=6;
	cp=xoff*7+zoff;
	ocp.ol_curpieceorder=0;
	ocp.ol_doacrashreset=0;
	ocp.ol_crashresetloc.x=-3.5f*od.piecesizex;
	ocp.ol_crashresetloc.y=0;
	ocp.ol_crashresetloc.z=-3.0f*od.piecesizez;
	ocp.ol_crashresetdir=90*PIOVER180;
	if (od.ol_st2_lastpiece!=-1 && od.ol_thetrack[6].piece!=PIECE_BASE && od.ol_thetrack[6].piece!=PIECE_GRID)
		do {
			cp=xoff*7+zoff;
//			logger("pieceorder %d = %d\n",pl,cp);
			od.ol_pieceorder[pl++]=cp;
			pce=od.ol_thetrack[cp].piece;
			if (od.ol_pieces[pce].changerot) {
				if (od.ol_pieces[pce].newor==1)
					curdir--;
				if (od.ol_pieces[pce].newor==2)
					curdir++;
			}
			curdir&=3;
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
			if (bail>55) { // safety
				logger("bailed\n");
				break;
			}
		} while (cp!=od.ol_ulastpiece);
//	logger("pieceorder %d = %d\n",pl,-1);
	od.ol_pieceorder[pl]=-1;	// sentinel
}
*/
static void ol_kill_bcams(tree2 *t)
{
//	int i;
//	for (i=0;i<t->nchildren;i++)
//		ol_kill_bcams(t->children[i]);
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it)
		ol_kill_bcams(*it);
	if (t->name=="lwscamera")
		freetree(t);
}

static void mtrailproc(tree2 *u)
{
	float s;
	u->trans.x+=u->transvel.x;
	u->trans.y+=u->transvel.y;
	u->trans.z+=u->transvel.z;
	u->userint[2]+=od.ol_uloop;
	s=(float)u->userint[2]/u->userint[3];
//	u->treealphacutoff=255;
//	u->dissolve=1-s;
	u->treecolor.w=1-s;
//	u->dissolve*=.85f;
	u->treecolor.w*=.85f;
	s+=.25f;
	s*=od.mtrailscale;
//	setVEC(&u->scale,s,s,s);
	u->scale=pointf3x(s,s,s);
	u->treecolor.x*=.98f;
	u->treecolor.y*=.98f;
	u->treecolor.z*=.98f;
	if (u->userint[2] > u->userint[3])
		freetree(u);
}

static void stasisfieldproc(tree2 *u)
{
	float s;
	u->userint[2]+=od.ol_uloop;
	s=(float)u->userint[2]/u->userint[3];
//	u->treealphacutoff=255;
//	u->dissolve=1-s;
	u->treecolor.w=1-s;
//	u->dissolve*=.85f;
	if (u->userint[2] > u->userint[3])
		freetree(u);
}

static void chemtrailproc(tree2 *u)
{
	float s;
//	int i;
/*	if (u->mod->cverts) {
		for (i=0;i<u->mod->nvert;i++) {
			u->cverts[i].x-=.04f;
			if (u->cverts[i].x<0)
				u->cverts[i].x=0;
			u->cverts[i].y-=.04f;
			if (u->cverts[i].y<0)
				u->cverts[i].y=0;
			u->cverts[i].z-=.04f;
			if (u->cverts[i].z<0)
				u->cverts[i].z=0;
		}
//		if (video_maindriver==VIDEO_D3D)
//			for (i=0;i<u->mod->nvert;i++)
//				u->d3dverts[i].c=D3DRGB(u->cverts[i].x,u->cverts[i].y,u->cverts[i].z);
	} */
	u->trans.x+=u->transvel.x;
	u->trans.y+=u->transvel.y;
	u->trans.z+=u->transvel.z;
	u->userint[2]+=od.ol_uloop;
	s=1-(float)u->userint[2]/u->userint[3];
//	u->treealphacutoff=92;
	s*=od.chemtrailscale;
//	setVEC(&u->scale,s,s,s);
	u->scale=pointf3x(s,s,s);
	if (u->userint[2] > u->userint[3])
		freetree(u);
}

static bool ol_udirtanim(tree2 *a)
{
  a->transvel.y -= 3/USCALEDOWN;
  a->userint[2]+=od.ol_uloop;
  if(a->userint[2]>a->userint[3])
	  return false;
  return true;
}

static bool ol_udirtcanim(tree2 *a)
{
  a->transvel.y -= 1/USCALEDOWN;
  a->userint[2]+=od.ol_uloop;
  if(a->userint[2]>a->userint[3])
	  return false;
  return true;
}

static void ol_udarkdirtanim(tree2 *a)
{
  a->transvel.y -= 2/USCALEDOWN;
  a->userint[2]+=od.ol_uloop;
  if(a->userint[2]>a->userint[3])
	  freetree(a);
}

#if 0
static void ol_uspawndirt(int fx) // no YOU spawndirt!, called when it is time to spawn dirt
{
	fx=0;
	mat4 jx;
	int i;
	int pe;
	int wantp;
	tree2 *unpart,*axl,*axr;
	if (fabs(op->ol_accelspin*4)<op->ol_startaccelspin)
		return;
	op->ccc++;
	pe=od.ol_particleeffects*2;//od.ol_uloop;
	if(op->ccc > pe)
		op->ccc = 0;
	else
		return;
	if(op->ol_accelspin<0) {
		axl = op->ol_wheels[FRONTLEFT];
		axr = op->ol_wheels[FRONTRIGHT];
		if(op->ol_dirtvbz>=0)
			op->ol_dirtvbz *= -1;
		if(op->ol_sub_dirtvbz>=0)
			op->ol_sub_dirtvbz *= -1;
	} else {
		axl = op->ol_wheels[FRONTLEFT];
		axr = op->ol_wheels[FRONTRIGHT];
		op->ol_dirtvbz = (float)fabs(op->ol_dirtvbz);
		op->ol_sub_dirtvbz = (float)fabs(op->ol_sub_dirtvbz);
	}
	wantp = od.ol_dirtq;
	quat2xform(&op->carang,&jx);
	for(i=0;i<wantp;i++) {
		unpart = duptree(od.udpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three mt_random for unpart\n");
//		unpart->transvel=pointf3x(-(op->ol_dirtvbx+mt_random(op->ol_dirtvrandx))/USCALEDOWN,
//									(op->ol_dirtvby+mt_random(op->ol_dirtvrandy))/USCALEDOWN,
//									-(op->ol_dirtvbz+mt_random(op->ol_dirtvrandz))/USCALEDOWN);
//		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel=pointf3x(0,(op->ol_dirtvby+mt_random(op->ol_dirtvrandy))/USCALEDOWN,0);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(axl,&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);

		unpart = duptree(od.udpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three more mt_random for unpart\n");
		unpart->transvel=pointf3x((op->ol_dirtvbx+mt_random(op->ol_dirtvrandx))/USCALEDOWN,
									(op->ol_dirtvby+mt_random(op->ol_dirtvrandy))/USCALEDOWN,
									-(op->ol_dirtvbz+mt_random(op->ol_dirtvrandz))/USCALEDOWN);
		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(axr,&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);
	}
	wantp = od.ol_sub_dirtq;
	for(i=0;i<wantp;i++) {
//		logger("a mt_random 2 for sub_dirtq\n");
		if (mt_random(2))
			unpart = duptree(od.udirtpart[fx]);
		else
			unpart = duptree(od.udarkdirtpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three mt_random for sub_dirt\n");
//		unpart->transvel=pointf3x(-(op->ol_sub_dirtvbx+mt_random(od.ol_sub_dirtvrandx))/USCALEDOWN,
//									(op->ol_sub_dirtvby+mt_random(od.ol_sub_dirtvrandy))/USCALEDOWN,
//									-(op->ol_sub_dirtvbz+mt_random(od.ol_sub_dirtvrandz))/USCALEDOWN);
//		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(op->ol_wheels[BACKLEFT],&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);

//		logger("a mt_random 2 for udirtpart\n");
		if (mt_random(2))
			unpart = duptree(od.udirtpart[fx]);
		else
			unpart = duptree(od.udarkdirtpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three mt_random for subdirtv\n");
		unpart->transvel=pointf3x((op->ol_sub_dirtvbx+mt_random(od.ol_sub_dirtvrandx))/USCALEDOWN,
									(op->ol_sub_dirtvby+mt_random(od.ol_sub_dirtvrandy))/USCALEDOWN,
									-(op->ol_sub_dirtvbz+mt_random(od.ol_sub_dirtvrandz))/USCALEDOWN);
		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(op->ol_wheels[BACKRIGHT],&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);
	}
}
#endif

static void ol_uspawndirt(int fx) // no YOU spawndirt!, called when it is time to spawn dirt
{
	fx=0;
	mat4 jx;
	int i;
	int pe;
	int wantp;
	tree2 *unpart,*axl,*axr;
	if (fabs(op->ol_accelspin*4)<op->ol_startaccelspin)
		return;
	op->ccc++;
	pe=od.ol_particleeffects*2;//od.ol_uloop;
	if(op->ccc > pe)
		op->ccc = 0;
	else
		return;
	if(op->ol_accelspin<0) {
		axl = op->ol_wheels[FRONTLEFT];
		axr = op->ol_wheels[FRONTRIGHT];
		if(op->ol_dirtvbz>=0)
			op->ol_dirtvbz *= -1;
		if(op->ol_sub_dirtvbz>=0)
			op->ol_sub_dirtvbz *= -1;
	} else {
		axl = op->ol_wheels[FRONTLEFT];
		axr = op->ol_wheels[FRONTRIGHT];
		op->ol_dirtvbz = (float)fabs(op->ol_dirtvbz);
		op->ol_sub_dirtvbz = (float)fabs(op->ol_sub_dirtvbz);
	}
	wantp = od.ol_dirtq;
	quat2xform(&op->carang,&jx);
	for(i=0;i<wantp;i++) {
		unpart = duptree(od.udpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three mt_random for unpart\n");
//		unpart->transvel=pointf3x(-(op->ol_dirtvbx+mt_random(op->ol_dirtvrandx))/USCALEDOWN,
//									(op->ol_dirtvby+mt_random(op->ol_dirtvrandy))/USCALEDOWN,
//									-(op->ol_dirtvbz+mt_random(op->ol_dirtvrandz))/USCALEDOWN);
//		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel=pointf3x(0,(op->ol_dirtvby+mt_random(op->ol_dirtvrandy))/USCALEDOWN,0);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop*.5f;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop*.5f;
		obj2world(axl,&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);

#if 0
		unpart = duptree(od.udpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three more mt_random for unpart\n");
		unpart->transvel=pointf3x((op->ol_dirtvbx+mt_random(op->ol_dirtvrandx))/USCALEDOWN,
									(op->ol_dirtvby+mt_random(op->ol_dirtvrandy))/USCALEDOWN,
									-(op->ol_dirtvbz+mt_random(op->ol_dirtvrandz))/USCALEDOWN);
		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(axr,&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);
	}
	wantp = od.ol_sub_dirtq;
	for(i=0;i<wantp;i++) {
//		logger("a mt_random 2 for sub_dirtq\n");
		if (mt_random(2))
			unpart = duptree(od.udirtpart[fx]);
		else
			unpart = duptree(od.udarkdirtpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three mt_random for sub_dirt\n");
//		unpart->transvel=pointf3x(-(op->ol_sub_dirtvbx+mt_random(od.ol_sub_dirtvrandx))/USCALEDOWN,
//									(op->ol_sub_dirtvby+mt_random(od.ol_sub_dirtvrandy))/USCALEDOWN,
//									-(op->ol_sub_dirtvbz+mt_random(od.ol_sub_dirtvrandz))/USCALEDOWN);
//		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(op->ol_wheels[BACKLEFT],&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);

//		logger("a mt_random 2 for udirtpart\n");
		if (mt_random(2))
			unpart = duptree(od.udirtpart[fx]);
		else
			unpart = duptree(od.udarkdirtpart[fx]);
		unpart->trans=zerov; //objects[0].pos;
//		logger("three mt_random for subdirtv\n");
		unpart->transvel=pointf3x((op->ol_sub_dirtvbx+mt_random(od.ol_sub_dirtvrandx))/USCALEDOWN,
									(op->ol_sub_dirtvby+mt_random(od.ol_sub_dirtvrandy))/USCALEDOWN,
									-(op->ol_sub_dirtvbz+mt_random(od.ol_sub_dirtvrandz))/USCALEDOWN);
		xformvec(&jx,&unpart->transvel,&unpart->transvel);
		unpart->transvel.x+=op->carvel.x*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.y+=op->carvel.y*od.ol_timeinc*od.ol_uloop;
		unpart->transvel.z+=op->carvel.z*od.ol_timeinc*od.ol_uloop;
		obj2world(op->ol_wheels[BACKRIGHT],&unpart->trans,&unpart->trans);
		if (!unpart->userproc)
			errorexit("no proc");
		linkchildtoparent(unpart,od.ol_root);
//		freetree(unpart);
#endif
	}
}

void ol_spawndirtparts1()
{
	if (!od.intimewarp) { // don't spawn dirt while time correction is taking place..
		ol_uspawndirt(0);
		op->ol_intunnels=50;
	}
}

void ol_spawndirtparts2()
{
	if (op->ol_airtime==0 || op->ol_cbairtime==0) {
		op->vinthermo=90;
		op->ol_doacrashreset=20;
		if (!od.intimewarp) // don't spawn dirt while time correction is taking place..
			ol_uspawndirt(1);
	}
}

static void ol_slowdown()
{
	op->slowdown=3;
}

static void ol_speedup()
{
	op->speedup=3;
}


/*
/// build the spheretree model if this is the first one
static tree2* buildasphere(const C8* modelname,const C8* matname)
{
	modelb* mod2=model_create("sphere3");
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,4,2,1,1,spheref_surf(1.25f),0,"tex");
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree3");
	spheretree2->setmodel(mod2);
	return spheretree2;
}
*/
/// build the spheretree model if this is the first one
static tree2* buildasphere(const C8* modelname,const C8* matname)
{
	pushandsetdir("particles");
	tree2* ret=new tree2("bainite_particles.bwo");
	popdir();
	return  ret;
}

#define SPARKSCALE 4
static void ol_uinitparticles()
{
//	return;
//	mater2 *m;
	pushandsetdirdown("st2_game");
//	od.udpart[0] = alloctree(1,"dust4a.lwo");
	od.udpart[0]=buildasphere("sparky","tex");
	od.udpart[0]->buildo2p=O2P_FROMTRANSROTSCALE;
	od.udpart[0]->rotvel=pointf3x(.25f,.35f,.45f);
	od.udpart[0]->flags|=TF_ALWAYSFACING; // shall we support? NYI
	od.udpart[0]->userint[2] = 0;
	od.udpart[0]->userint[3] = 10;
//	od.udpart[0]->treealphacutoff=10;
//	od.udpart[0]->scale=pointf3x(od.ol_dirtscale,od.ol_dirtscale,od.ol_dirtscale);
	od.udpart[0]->scale=pointf3x(SPARKSCALE,SPARKSCALE,SPARKSCALE);
	od.udpart[0]->userproc = ol_udirtanim;
	od.udpart[0]->treecolor=F32BLUE;
	od.udpart[0]->treecolor.w=.5f;
//    if (video_maindriver!=VIDEO_D3D) {
//		MAT *m;
//		;//togglezbuffer(udpart,"dust",1);
//		m = findmaterial(od.udpart[0],"dust");
//		m->mtrans=.5f;
//		if (m) {
//			m->color.w=.5f;
//			m->msflags|=SMAT_HASWBUFF;
//		}
//	}

	od.udirtpart[0] = alloctree(1,"dirtparta.lwo");
	od.udirtpart[0]->flags|=TF_ALWAYSFACING;
	od.udirtpart[0]->userint[2] = 0;
	od.udirtpart[0]->userint[3] = 10;
//	od.udirtpart[0]->treealphacutoff=10;
//	setVEC(&od.udirtpart[0]->scale,od.ol_sub_dirtscale,od.ol_sub_dirtscale,od.ol_sub_dirtscale);
	od.udirtpart[0]->scale=pointf3x(od.ol_sub_dirtscale,od.ol_sub_dirtscale,od.ol_sub_dirtscale);
	od.udirtpart[0]->userproc = ol_udirtcanim;
 //   if (video_maindriver!=VIDEO_D3D) {
//		MAT *m;
//		;//togglezbuffer(udirtpart,"dirt",1);
//		m = findmaterial(od.udirtpart[0],"dirt");
//		if (m) {
//			m->color.w=.5f;
//			m->msflags|=SMAT_HASWBUFF;
//		}
//	}

/*	od.ol_udarkdirt = alloctree(1,"drkdirt.lwo");
	od.ol_udarkdirt->flags|=TF_ALWAYSFACING;
	od.ol_udarkdirt->userint[2] = 0;
	od.ol_udarkdirt->userint[3] = 5;
	od.ol_udarkdirt->treealphacutoff=10;
	setVEC(&od.ol_udarkdirt->scale,od.ol_darkdirtscale,od.ol_darkdirtscale,od.ol_darkdirtscale);
	od.ol_udarkdirt->proc = ol_udarkdirtanim;
 //   if (video_maindriver!=VIDEO_D3D) {
		;//togglezbuffer(udarkdirt,"dust",1);
		m = findmaterial(od.ol_udarkdirt,"dust");
		m->mtrans=.5f;
		m->msflags|=SMAT_HASWBUFF;
//	}
*/
	od.udarkdirtpart[0] = alloctree(1,"dkdrtprta.lwo");
	od.udarkdirtpart[0]->flags|=TF_ALWAYSFACING;
	od.udarkdirtpart[0]->userint[2] = 0;
	od.udarkdirtpart[0]->userint[3] = 10;
//	od.udarkdirtpart[0]->treealphacutoff=10;
//	setVEC(&od.udarkdirtpart[0]->scale,od.ol_sub_darkdirtscale,od.ol_sub_darkdirtscale,od.ol_sub_darkdirtscale);
	od.udarkdirtpart[0]->scale=pointf3x(od.ol_sub_darkdirtscale,od.ol_sub_darkdirtscale,od.ol_sub_darkdirtscale);
	od.udarkdirtpart[0]->userproc = ol_udirtcanim;

	od.udpart[1] = alloctree(1,"dust4b.lwo");
	od.udpart[1]->flags|=TF_ALWAYSFACING;
	od.udpart[1]->userint[2] = 0;
	od.udpart[1]->userint[3] = 10;
//	od.udpart[1]->treealphacutoff=10;
	od.udpart[1]->scale=pointf3x(od.ol_dirtscale,od.ol_dirtscale,od.ol_dirtscale);
	od.udpart[1]->userproc = ol_udirtanim;
//    if (video_maindriver!=VIDEO_D3D) {
//		MAT *m;
//		;//togglezbuffer(udpart,"dust",1);
//		m = findmaterial(od.udpart[1],"dust");
//		if (m) {
//			m->color.w=.5f;
//			m->msflags|=SMAT_HASWBUFF;
//		}
//	}

	od.udirtpart[1] = alloctree(1,"dirtpartb.lwo");
	od.udirtpart[1]->flags|=TF_ALWAYSFACING;
	od.udirtpart[1]->userint[2] = 0;
	od.udirtpart[1]->userint[3] = 10;
//	od.udirtpart[1]->treealphacutoff=10;
	od.udirtpart[1]->scale=pointf3x(od.ol_sub_dirtscale,od.ol_sub_dirtscale,od.ol_sub_dirtscale);
	od.udirtpart[1]->userproc = ol_udirtcanim;
 //   if (video_maindriver!=VIDEO_D3D) {
//		MAT *m;
//		;//togglezbuffer(udirtpart,"dirt",1);
//		m = findmaterial(od.udirtpart[1],"dirt");
//		if (m) {
//			m->color.w=.5f;
//			m->msflags|=SMAT_HASWBUFF;
//		}
//	}

/*	od.ol_udarkdirt = alloctree(1,"drkdirt.lwo");
	od.ol_udarkdirt->flags|=TF_ALWAYSFACING;
	od.ol_udarkdirt->userint[2] = 0;
	od.ol_udarkdirt->userint[3] = 5;
	od.ol_udarkdirt->treealphacutoff=10;
	setVEC(&od.ol_udarkdirt->scale,od.ol_darkdirtscale,od.ol_darkdirtscale,od.ol_darkdirtscale);
	od.ol_udarkdirt->proc = ol_udarkdirtanim;
 //   if (video_maindriver!=VIDEO_D3D) {
		;//togglezbuffer(udarkdirt,"dust",1);
		m = findmaterial(od.ol_udarkdirt,"dust");
		m->mtrans=.5f;
		m->msflags|=SMAT_HASWBUFF;
//	}
*/
	od.udarkdirtpart[1] = alloctree(1,"dkdrtprtb.lwo");
	od.udarkdirtpart[1]->flags|=TF_ALWAYSFACING;
	od.udarkdirtpart[1]->userint[2] = 0;
	od.udarkdirtpart[1]->userint[3] = 10;
//	od.udarkdirtpart[1]->treealphacutoff=10;
	od.udarkdirtpart[1]->scale=pointf3x(od.ol_sub_darkdirtscale,od.ol_sub_darkdirtscale,od.ol_sub_darkdirtscale);
	od.udarkdirtpart[1]->userproc = ol_udirtcanim;

	popdir();
}

static void ol_ufreeparticles()
{
	freetree(od.udpart[0]);
	freetree(od.udirtpart[0]);
//	freetree(od.ol_udarkdirt);
	freetree(od.udarkdirtpart[0]);
	freetree(od.udpart[1]);
	freetree(od.udirtpart[1]);
//	freetree(od.ol_udarkdirt);
	freetree(od.udarkdirtpart[1]);
}

void ol_dowater()
{
	logger("%2d: in dowater ct %d\n",op->carid,op->clocktickcount);
}

void ol_dolava()
{
	logger("%2d: in dolava ct %d\n",op->carid,op->clocktickcount);
}

/*void ol_slowdown()
{
	op->slowdown=3;
}

void ol_speedup()
{
	op->speedup=3;
}
*/
/////////////////////// end particle stuff

static void ol_fixpondalphacutoff(tree2 *t)
{
/*	int i;
	if (!my_stricmp(t->name.c_str(),"pondw.lwo"))
		;//t->treealphacutoff=10;
	for (i=0;i<t->nchildren;i++)
		ol_fixpondalphacutoff(t->children[i]); */
}

/*static int ol_st2_loadtrack(char *filename, int override)
{
	int i,j=0,numl;
	char **script;
    memset(od.ol_thetrack,0,sizeof(od.ol_thetrack));
    pushandsetdir("st2_tracks");
		script=loadscript(filename,&numl);

		for(i=0;i<numl;i+=7)
		{
			od.ol_thetrack[j].piece = atoi(script[i]);

			if(override && od.ol_thetrack[j].piece == PIECE_GRID) od.ol_thetrack[j].piece = PIECE_BASE;
			if(!override && od.ol_thetrack[j].piece == PIECE_BASE) od.ol_thetrack[j].piece = PIECE_GRID;

			od.ol_timetocomplete += od.ol_pieces[od.ol_thetrack[j].piece].time;
			//error("%d %d",timetocomplete,pieces[thetrack[j].piece].time);

			od.ol_thetrack[j].or = atoi(script[i+1]);
			od.ol_thetrack[j].lpiece = atoi(script[i+2]);
			od.ol_thetrack[j].previous = atoi(script[i+3]);
			od.ol_st2_curpiece = atoi(script[i+4]);
			od.ol_st2_lastpiece = atoi(script[i+5]);
			od.ol_st2_dir = atoi(script[i+6]);
			od.ol_trackworth += od.ol_pieces[od.ol_thetrack[j].piece].value;
			od.ol_pieces[od.ol_thetrack[j].piece].value --;
			if(od.ol_pieces[od.ol_thetrack[j].piece].value < 0) od.ol_pieces[od.ol_thetrack[j].piece].value = 0;

			j++;
		}
		freescript(script,numl);
	popdir();

	for(i=0;i<j;i++) od.ol_pieces[od.ol_thetrack[i].piece].value = od.ol_pieces[od.ol_thetrack[i].piece].ovalue;

	//error("%d",trackworth);

	return numl;
}
*/
static void splitbigmodels(tree2 *t)
{
#ifdef KNOWSPLITMODELS
	struct tree *c,*c2;
	int i,n;//j;
//	return;
	n=t->nchildren;
	for (i=0;i<n;i++) {
		c=t->children[i];
		if (c->mod) {
			if (c->mod->nface>4000) {
				c2=duptreeandsplitmodel(c,16,1,16);
				freetree(c);
				linkchildtoparent(c2,t);
//				for (j=0;j<c2->nchildren;j++) {
//					c2->children[j]->o2p.e[3][1]=.001f*j;
//				}
			} //else
		}
		splitbigmodels(c);
	}
#endif
}

static void targetquat(pointf3* src,pointf3* dst,pointf3* q)
{
	pointf3 targ;
	targ.x=dst->x-src->x;
	targ.y=dst->y-src->y;
	targ.z=dst->z-src->z;
	q->y=(float)atan2(targ.x,targ.z);
	q->x=(float)-atan2(targ.y,sqrt(targ.x*targ.x+targ.z*targ.z));
	q->z=0;
	rpy2quat(q,q);
}

//////////// do ai and paths /////////////////////
static void setupai(tree2 *t)
{
#define STARTFRAME 0
	int i;
	pointf3 top,bot,tv,intsect;
//	od.path1=findtreenamerec(t,"path1.bwo");
//	od.path2=findtreenamerec(t,"path2.bwo");
	if (!od.path2)
		od.path2=findtreenamerec(t,"path1.bwo");
	if (od.path1) {
		seq_stop(od.path1);
		seq_stop(od.path2);
		for (i=0;i<NUMTVCAMS;i++) {
			tv.z=-1;
			tv.y=0;
			if (i&1)
				tv.x=-2/od.ntrackscale;
			else
				tv.x=2/od.ntrackscale;
			od.path1->curframe=(float)i*(od.trackend-od.trackstart)/(NUMTVCAMS-1)+od.trackstart;
			(od.path1->proc)();
			obj2world(od.path1,&tv,&top);
			bot=top;
			top.y+=20;
			bot.y-=20;
			if (st2_line2roadlo(&top,&bot,&od.tvcams[i],NULL))
				od.tvcams[i].y+=.95f;
			tv=zerov;
			obj2world(od.path1,&tv,&top);
			top.y+=20;
			bot.y-=20;
			if (st2_line2road(&top,&bot,&intsect,NULL))
				targetquat(&od.tvcams[i],&intsect,&od.tvcamsq[i]);

		}
		od.path1->curframe=od.trackstart;
		(od.path1->proc)();
		od.path2->curframe=od.trackstart;
		(od.path2->proc)();
//		for (i=0,op=&opa[0];i<od.ol_numplayers;i++,op++) {
//			op->seekframe=od.trackstart;
//		}
		op=NULL;
	od.stepspace=od.trackend/MAXTS*1.8f;//od.path1->seqs[0].endframe/10/8;
	od.nextspace=od.trackend/20;
	}
}

void updatedifftime(int clk)
{
	char str[150];
	if (od.difftimemode==1) {
		getsctime(clk,str);
		od.tscoret[PTEXT_DIFFTIME]=320;
		od.cscoret[PTEXT_DIFFTIME]=20;
		sprintf(od.scorestrings[PTEXT_DIFFTIME],"           %s",str);
	}
}

static void doai()
{
//#define VFDIST 8.06f
//#define FDIST 4.47f
//#define MDIST 2.83f
//#define CDIST 2.0f
//#define TURNTHRESH (TWOPI/64.0f)
//#define TURNTHRESH (TWOPI/32.0f)
	int cnt;
	tree2 *cp;
//	pointf3 top,bot;
	pointf3 pointz={0,0,1,0};
	float closedist2=od.pathclosedist*od.pathclosedist;
	float middist2=od.pathmeddist*od.pathmeddist;
	float fardist2=od.pathfardist*od.pathfardist;
	float fardist2place=od.pathfardistplace*od.pathfardistplace;
	float vfardist2=od.pathvfardist*od.pathvfardist;
	float fs=od.pathframestep; // frame step
//	float dh;
	pointf3 pdir;	// direction z of path is pointing in world
	float dist;
	pointf3 delta;
	float chead,phead,dhead; // carheading and pathheading and heading from car to path
	float s;
	if (!od.path1)
		return;
//	if (!od.path1->proc)
//		return;
//	if (op->ol_airtime>2)
//		return;
//	if (op->cntl==CNTL_AI)
//	if (od.selpath==1)
//		cp=od.path1;
//	else if (od.selpath==2)
//		cp=od.path2;
//	else
//JAY
	if (currule->ctf) {
		if ( od.ol_flagidx >= 0 )
			cp = od.ol_flags[od.ol_flagidx].t;
		else
			return;
	} else {
		if (op->carid&1)
			cp=od.path2;
		else
			cp=od.path1;
	}
		//if (op->carid&1)
		//	cp=od.path2;
		//else
		//	cp=od.path1;


//	else
//		if (op->clocktickcount&1)
//			cp=od.path2;
//		else
//			cp=od.path1;
	op->pi.ol_uup=1;
//	od.path1->curframe=op->seekframe;
//	(od.path1->proc)(od.path1);
//	od.path2->curframe=op->seekframe;
//	(od.path2->proc)(od.path2);
// do it again, just for what place your in
	cnt=10;
	cp->curframe=op->seekframe2;
	(cp->proc)();
	delta.x=cp->trans.x*od.ntrackscale;
	delta.z=cp->trans.z*od.ntrackscale;
	delta.x-=op->pos.x;
	delta.z-=op->pos.z;
	dist=delta.x*delta.x+delta.z*delta.z;
//	logger("moving seekframe2 start %f\n",op->seekframe2);
	while (dist<fardist2place) { // move path1 if close enough
		if (cnt==0)
			break;
		op->seekframe2+=fs;
		if (op->laps2==od.laps)
			op->seekframe2=od.trackend;
		else {
			if (od.laps==1) {
				if (op->seekframe2>=od.trackend)
					op->seekframe2=od.trackend;
			} else {
				if (op->seekframe2>=od.trackend) {
					op->seekframe2-=od.trackend;
					op->laps2++;
				}
			}
		}
		cp->curframe=op->seekframe2;
 		(cp->proc)();
		delta.x=cp->trans.x*od.ntrackscale;
		delta.z=cp->trans.z*od.ntrackscale;
//		logger("op2 %f,%f rab %f,%f, frm %f, cnt %d\n",op->pos.x,op->pos.z,delta.x,delta.z,op->seekframe2,cnt);
		delta.x-=op->pos.x;
		delta.z-=op->pos.z;
		dist=delta.x*delta.x+delta.z*delta.z;
		cnt--;
	}
//	logger("moving seekframe2 end\n");
	cp->curframe=op->seekframe;
	(cp->proc)();
	delta.x=(cp->trans.x*od.ntrackscale)-op->pos.x;
	delta.z=(cp->trans.z*od.ntrackscale)-op->pos.z;
	dist=delta.x*delta.x+delta.z*delta.z;
	if (dist>vfardist2 && !od.disablespace) {
		logger("car reset: space generated for your car that's too far away %f > %f (more..)\n",
			dist,vfardist2);
		logger("   seekframe %f, carid %d, oppos %f %f, cp %f %f, ntrackscale %f\n",
			op->seekframe,op->carid,op->pos.x,op->pos.z,cp->trans.x,cp->trans.z,od.ntrackscale);
		op->espace=1;
	}
	if (op->clocktickcount>=INITCANTSTARTDRIVE && !op->venableicrms)
		op->stuckwatchdog++;
	if (op->stuckwatchdog>od.maxwatchdog) {
		if (op->cntl==CNTL_AI || od.playernet[op->carid].onebotplayer) {// && !od.disablespace)
//			logger("car reset: space generated for ai car that's stuck\n");
			op->pi.uspace=1;
		}
		op->stuckwatchdog=0;
	}
	cnt=10;
//	logger("moving seekframe start %f\n",of->seekframe);
	while (dist<fardist2) { // move path1 if close enough
		if (cnt==0)
			break;
		op->seekframe+=fs;
		op->stuckwatchdog=0;
		if (op->chklap==od.laps) {
//			if (op->seekframe>=od.trackend)
				op->seekframe=od.trackend;
		} else {
			if (od.laps==1) {
				if (op->seekframe>=od.trackend)
					op->seekframe=od.trackend;
			} else {
				if (op->seekframe>=od.trackend)
					op->seekframe-=od.trackend;
			}
		}
//			op->seekframe-=od.trackstart;
//		if (op->seekframe>=od.path1->seqs[0].endframe) {
//			op->seekframe-=od.path1->seqs[0].endframe;
//			op->lapready++;
#if 0
		if (op->cntl==CNTL_AI)
			cp->curframe=op->seekframe-od.pathaihornback;//*od.path1->seqs[0].endframe;
		else
			cp->curframe=op->seekframe-od.pathhornback;//*od.path1->seqs[0].endframe;
		if (cp->curframe<0)
			cp->curframe=0;
//			od.path1->curframe+=od.path1->seqs[0].endframe;
#endif
		cp->curframe=op->seekframe;
 		(cp->proc)();
//		delta.x=cp->trans.x*od.ntrackscale;
//		delta.z=cp->trans.z*od.ntrackscale;
/*
		op->ol_crashresetloc.x=delta.x;
		op->ol_crashresetloc.y=0;
		op->ol_crashresetloc.z=delta.z;
		top=bot=op->ol_crashresetloc;
		top.y+=2000;
		bot.y-=2000;
		if (st2_line2road(&top,&bot,&op->ol_crashresetloc,NULL)) {
			op->ol_crashresetloc.y+=od.hipoint;
			quat2rpy(&cp->rot,&top);
			op->ol_crashresetdir=top.y;
		}

*/
/*		od.path1->curframe=op->seekframe;
		(od.path1->proc)(od.path1);
		od.path2->curframe=op->seekframe;
		(od.path2->proc)(od.path2); */
		delta.x=cp->trans.x*od.ntrackscale;
		delta.z=cp->trans.z*od.ntrackscale;
//		logger("op %f,%f rab %f,%f, frm %f, cnt %d\n",op->pos.x,op->pos.z,delta.x,delta.z,op->seekframe2,cnt);
		delta.x-=op->pos.x;
		delta.z-=op->pos.z;
		dist=delta.x*delta.x+delta.z*delta.z;

		cnt--;
	}
//	logger("moving seekframe end\n");
#if 1
	if (od.showrabbits) {
		if (!od.intimewarp) {
			pointf3 del;
			if (op==of) {
				pointf3 top,bot;
				od.tscoret[PTEXT_TEST-1]=100;
				od.cscoret[PTEXT_TEST-1]=20;
				sprintf(od.scorestrings[PTEXT_TEST-1],"SF2 %4.1f LP2 %d",op->seekframe2,op->laps2);
//				sprintf(od.scorestrings[PTEXT_TEST-1],"SF %4.1f RF %4.1f",op->seekframe,op->ol_crashresetframe);

				od.tscoret[PTEXT_TEST]=100;
				od.cscoret[PTEXT_TEST]=20;
				sprintf(od.scorestrings[PTEXT_TEST],"D2 %6.3f WD %d",dist,op->stuckwatchdog);
				cp->curframe=op->seekframe;
				(cp->proc)();
				del.x=cp->trans.x*od.ntrackscale;
				del.y=cp->trans.y*od.ntrackscale;
				del.z=cp->trans.z*od.ntrackscale;
				if (od.path2road) {
					top=bot=del;
					top.y+=100;
					bot.y-=200;
					if (st2_line2roadlo(&top,&bot,&del,NULL))
						addnull(&del,&cp->rot);
				} else
					addnull(&del,&cp->rot);
			}
/*			del.x=od.path2->trans.x*od.ntrackscale;
			del.y=od.path2->trans.y*od.ntrackscale;
			del.z=od.path2->trans.z*od.ntrackscale;
			top=bot=del;
			top.y+=100;
			bot.y-=200;
			if (st2_line2roadlo(&top,&bot,&del,NULL))
				addnull(&del,&od.path2->rot); */
		}
	}
	if (od.selpath) {
		if (!od.intimewarp && op==of) {
			pointf3 top,bot;
			tree2 *sp;
			pointf3 del;
			if (od.selpath==1 || od.selpath==3) {
				sp=od.path1;
				sp->curframe=od.showpathframe;
				(sp->proc)();
				del.x=sp->trans.x*od.ntrackscale;
				del.y=sp->trans.y*od.ntrackscale;
				del.z=sp->trans.z*od.ntrackscale;
				if (od.path2road) {
					top=bot=del;
					top.y+=100;
					bot.y-=200;
					if (st2_line2roadlo(&top,&bot,&del,NULL))
						addnull(&del,&sp->rot);
				} else
					addnull(&del,&sp->rot);
			}
			if (od.selpath==2 || od.selpath==3) {
				sp=od.path2;
				sp->curframe=od.showpathframe;
				(sp->proc)();
				del.x=sp->trans.x*od.ntrackscale;
				del.y=sp->trans.y*od.ntrackscale;
				del.z=sp->trans.z*od.ntrackscale;
				if (od.path2road) {
					top=bot=del;
					top.y+=100;
					bot.y-=200;
					if (st2_line2roadlo(&top,&bot,&del,NULL))
						addnull(&del,&sp->rot);
				} else
					addnull(&del,&sp->rot);
			}
		}
	}
#endif
// calc space on track for rel time calc
	if (!op->dofinish && !od.predicted) {
//		s=/*od.path1->seqs[0].endframe*op->lapready+*/op->seekframe;
		s=op->seekframe2+op->laps2*(od.trackend+1);
		if (s>od.nextspace && od.numts<MAXTS2) {
			od.ts[od.numts].time=op->clocktickcount;
			od.ts[od.numts].space=s;
			od.ts[od.numts].ncarspassed=1;
			od.nextspace+=od.stepspace;
			od.numts++;
			op->cartspassed=od.numts;
		} else {
			if (op->cartspassed<od.numts) {	// see if any markers to look for
				if (s>od.ts[op->cartspassed].space) {
					int dt;
					char str[50];
					od.ts[op->cartspassed].ncarspassed++;
					if (od.ts[op->cartspassed].ncarspassed==2) // 2nd car to pass this point, signal first car
						od.signalfirst=od.ts[op->cartspassed].time-op->clocktickcount;
					if (op==of && !op->finplace/*!od.fplace*/) {
						if (od.difftimemode==1) {
							dt=op->clocktickcount-od.ts[op->cartspassed].time;
							getsctime(dt,str);
							od.tscoret[PTEXT_DIFFTIME]=120;
							od.cscoret[PTEXT_DIFFTIME]=20;
							sprintf(od.scorestrings[PTEXT_DIFFTIME],"           %s",str);
						}
					}
					op->cartspassed++;
				}
			}
		}
	}
	if (dist>=closedist2) { // real close, just drive straight, no atans
// calc headings
		chead=(float)atan2(op->ol_cardir.x,op->ol_cardir.z);
		op->ch=chead;
		quatrot(&cp->rot,&pointz,&pdir);
		if (op==of && !od.intimewarp) {
			if (pdir.x*op->ol_cardir.x+pdir.z*op->ol_cardir.z<-.7071f)
				od.wrongway++;
			else
				od.wrongway=0;
		}
		dhead=(float)atan2(delta.x,delta.z);
		if (dist<middist2) { // kinda close, drive in same dir as path obj
			phead=(float)atan2(pdir.x,pdir.z);
//			dhead=-999*PIOVER180;
			op->dh=snormalangrad(chead-phead);
		} else { // far, drive towards path
			phead=-999*PIOVER180;
			op->dh=snormalangrad(chead-dhead);
		}
		if (op->dh>od.turnthresh)
			op->pi.ol_uleft=1;
		else if (op->dh<-od.turnthresh)
			op->pi.ol_uright=1;
	}
	if (op->ol_airtime>2)
		op->pi.ol_uup=0;
	if (op->ol_startstunt) {
		if (op->doaistunt[0] && op->ol_uready) {
			op->pi.ol_uup=op->pi.ol_udown=0;
			if (op->curstuntangs.x<op->stuntgoals.x-40) {
				op->pi.ol_uup=1;
			} else if (op->curstuntangs.x>op->stuntgoals.x+40) {
				op->pi.ol_udown=1;
			} else {
				op->doaistunt[0]=0;
			}
		}
		if (op->doaistunt[1]) {
			op->pi.ol_uleft=op->pi.ol_uright=0;
			if (op->curstuntangs.y<op->stuntgoals.y-40) {
				op->pi.ol_uright=1;
			} else if (op->curstuntangs.y>op->stuntgoals.y+40) {
				op->pi.ol_uleft=1;
			} else {
				op->doaistunt[1]=0;
			}
		}
		if (op->doaistunt[2]) {
			op->pi.ol_urollleft=op->pi.ol_urollright=0;
			if (op->curstuntangs.z<op->stuntgoals.z-40) {
				op->pi.ol_urollleft=1;
			} else if (op->curstuntangs.z>op->stuntgoals.z+40) {
				op->pi.ol_urollright=1;
			} else {
				op->doaistunt[2]=0;
			}
		}
	}
//	sprintf(teststr,"phead %f, chead %f, dhead %f, dh %f",
//		phead*PIUNDER180,chead*PIUNDER180,dhead*PIUNDER180,dh*PIUNDER180);
}

static void setupaistunttime(float t)
{
// yaw
#define RANDSTUNTX .09f
#define RANDSTUNTY .09f
#define RANDSTUNTZ .09f
	int i;
	t*=2.1f; // compensate for lower landing spot, fudge
	op->doaistunt[0]=op->doaistunt[1]=op->doaistunt[2]=0;
	op->curstuntangs=op->stuntgoals=op->ol_carstuntrpy;
//	op->doaistunt[0]=op->doaistunt[1]=op->doaistunt[2]=1;
//	return;
//	logger("setupaistunttime x mt_frand\n");
	if (pfrand1(op->clocktickcount)<RANDSTUNTX) {
		for (i=MAXAISTUNTS-1;i>=0;i--)
			if (op->stunttimes[i].x<t)
				break;
		if (i>=0) {
			op->doaistunt[0]=1;
			op->stuntgoals.x-=360*(float)(i+1);
//			logger("setupaistunttime x mt_random 2\n");
			if (prandom1(op->clocktickcount,2))
				op->stuntgoals.x=-op->stuntgoals.x;
		}
	}
//	logger("setupaistunttime y mt_frand\n");
	if (pfrand2(op->clocktickcount)<RANDSTUNTY) {
		for (i=MAXAISTUNTS-1;i>=0;i--)
			if (op->stunttimes[i].y<t)
				break;
		if (i>=0) {
			op->doaistunt[1]=1;
			op->stuntgoals.y-=360*(float)(i+1);
//			logger("setupaistunttime y mt_random 2\n");
			if (prandom2(op->clocktickcount,2))
				op->stuntgoals.y=-op->stuntgoals.y;
		}
	}
//	logger("setupaistunttime z mt_frand\n");
	if (pfrand3(op->clocktickcount)<RANDSTUNTZ) {
		for (i=MAXAISTUNTS-1;i>=0;i--)
			if (op->stunttimes[i].z<t)
				break;
		if (i>=0) {
			op->doaistunt[2]=1;
			op->stuntgoals.z-=360*(float)(i+1);
//			logger("setupaistunttime z mt_random 2\n");
			if (prandom3(op->clocktickcount,2))
				op->stuntgoals.z=-op->stuntgoals.z;
		}
	}
}

//////////////// end do ai and paths
#if 1
static void changelightstate(int s)
{
	struct bitmap32 *b16;
	if (s==od.lightstate)
		return;
	switch(s) {
	case 0:	// off, load tex
	case 6: // off
/*		od.tlight->flags|=TF_DONTDRAW;
		od.tlight2->flags|=TF_DONTDRAW;
		*/
		if (od.tlighttex[0]) {
			b16=locktexture(od.tlighttex[0]);
			clipblit32(od.tlighttsp->frames[0],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[0]);
			b16=locktexture(od.tlighttex[1]);
			clipblit32(od.tlighttsp->frames[2],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[1]);
			b16=locktexture(od.tlighttex[2]);
			clipblit32(od.tlighttsp->frames[4],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[2]);
			b16=locktexture(od.tlighttex[3]);
			clipblit32(od.tlighttsp->frames[6],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[3]);
		}
		break;
	case 1: // on
//		od.tlight->flags&=~TF_DONTDRAW;
//		od.tlight2->flags&=~TF_DONTDRAW;
		ol_playatagsound(40,1.0f,0); // start engines
		break;
	case 2: // red
		if (od.tlighttex[0]) {
			b16=locktexture(od.tlighttex[0]);
			clipblit32(od.tlighttsp->frames[1],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[0]);
		}
		ol_playatagsound(28,1.0f,0);
		break;
	case 3: // red2
		if (od.tlighttex[0]) {
			b16=locktexture(od.tlighttex[1]);
			clipblit32(od.tlighttsp->frames[3],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[1]);
		}
		ol_playatagsound(28,1.0f,0);
		break;
	case 4: // yellow
		if (od.tlighttex[0]) {
			b16=locktexture(od.tlighttex[2]);
			clipblit32(od.tlighttsp->frames[5],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[2]);
		}
		ol_playatagsound(28,1.0f,0);
		break;
	case 5: // green
		if (od.tlighttex[0]) {
			b16=locktexture(od.tlighttex[3]);
			clipblit32(od.tlighttsp->frames[7],b16,0,0,0,0,od.tlighttsp->x,od.tlighttsp->y);
			unlocktexture(od.tlighttex[3]);
		}
		ol_playatagsound(30,1.0f,0);
		break;
//		od.tlight->flags|=TF_DONTDRAW;
//		od.tlight2->flags|=TF_DONTDRAW;
//		break;
	}
	od.lightstate=s;
}
#endif
static void globalchangematerial(tree2 *t)
{
	int i;
	modelb *m;
/*	if (t->buildo2p==O2P_NONE) {
		t->buildo2p=O2P_FROMTRANSROTSCALE;
//		t->trans.x=mt_frand()-.5f;
//		t->trans.y=mt_frand()-.5f;
		t->trans.z=mt_frand()-.5f;
	} */
	m=t->mod;
//	if (strcmp("scrline.lwo",t->name))
//		t->flags|=TF_DONTDRAW;
	if (m) {
		if (!my_stricmp(t->name.c_str(),"outerrelm.bwo") || !my_stricmp(t->name.c_str(),"horizon.bwo"))
			m->mats[0].msflags|=SMAT_ISSKY;
		for (i=0;i!=(S32)m->mats.size();i++) {
//			m->mats[i].msflags&=~(SMAT_HASWBUFF);
//			m->mats[i].msflags&=~(SMAT_HASWBUFF|SMAT_HASTEX/*|SMAT_HASSHADE*/);
//			m->mats[i].msflags&=~(SMAT_HASTEX/*|SMAT_HASSHADE*/);
		}
	}
//	for (i=0;i<t->nchildren;i++)
//		globalchangematerial(t->children[i]);
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it)
		globalchangematerial(*it);
}

#if 0
void convertframe(struct bitmap16 *b,int oldformat,int newformat)
{
	int k,p;//,good;
	unsigned short *d;
	unsigned int v;
	p=b->x*b->y;
	d=b->plane;
	if (oldformat==TEX555CK) {
//		logger("convertfont 555 to 565\n");
		for (k=0;k<p;k++,d++) {
			v=*d;
			v=color555to565(v);
			*d=v;
		}
	} else if (oldformat!=TEX565CK)
		errorexit("convertfont: unknown source format %d",oldformat);
	d=b->plane;
	switch(newformat) {
	case TEX555CK:
	case TEX555NCK:
//		logger("convertfont 565 to 555\n");
		for (k=0;k<p;k++,d++) {
			v=*d;
			if (v!=0) {
				v=color565to555(v);
				if (v==0)
					v++;
			}
			*d=v;
		}
		break;
	case TEX5551A:
//		logger("convertfont 565 to 5551\n");
		for (k=0;k<p;k++,d++) {
			v=*d;
			if (v!=0) {
				v=color565to555(v);
				v|=0x8000;
			}
			*d=v;
		}
		break;
	case TEX4444A:
//		logger("convertfont 565 to 4444\n");
		for (k=0;k<p;k++,d++) {
			v=*d;
			if (v!=0) {
				v=((v>>1)&0xf)+((v>>3)&0xf0)+((v>>4)&0xf00);
				v|=0xf000;
			}
			*d=v;
		}
		break;
	case TEX565NCK:
	case TEX565CK:
		break;
	default:
		errorexit("convertfont: unknown dest format %d",newformat);
	}
//	for (k=0;k<p;k++,d++) {
//		v=*d;
//		*d=v;
//	}
}

void convertfont(struct tsp *t,int oldformat,int newformat)
{
//	logger("converting font from %d to %d\n",oldformat,newformat);
//	if (oldformat==newformat)
//		return;
	int i;
	for (i=0;i<t->nframe;i++) {
//		good=0;
/*		if (i>='A' && i<='Z')
			good=1;
		if (i>='0' && i<='9')
			good=1;
		if (i=='-' || i=='.')
			good=1;
		if (!good)
			continue; */
		convertframe(t->frames[i],oldformat,newformat);
	}
}
#endif

static void ol_drawstring(struct tsp *f,struct bitmap32 *dest,char *s, int x, int y)
{
	outtextxy32(B32,x,y,C32WHITE,s);
	if (!f)
		return;
	unsigned int c,fx;
	int d;
//	if (zoomin) {
//		x+=(x+WX)>>1;
//		y+=(y+WY)>>1;
//	}
//	perf_start(DRAWSTRING);
	while(c=*s++) {
		if (c!=' ' && c<128) {
			fx=f->frames[c]->size.x;
//			logger("'%c' wid fontopt %d %d\n",c,fx,od.fontopt);
			d=FONTWIDTH-fx;
//			if (d<0)
//				d=0;
			clipxpar32(f->frames[c],dest,
			0,0,x+(d>>1),y,fx,f->frames[c]->size.y,C32BLACK);
		}
		x+=FONTWIDTH;
	}
//	perf_end(DRAWSTRING);
}

void ol_drawstring2(struct tsp *f,struct bitmap32 *dest,char *s, int x, int y,int maxwidth)
{
	outtextxy32(B32,x,y,C32WHITE,s);
	if (!f)
		return;
	int xs;
	unsigned int c;
//	if (zoomin) {
//		x+=(x+WX)>>1;
//		y+=(y+WY)>>1;
//	}
//	perf_start(DRAWSTRING);
//	maxwidth+=x;
	xs=x;
	while(c=*s++) { // variable width
		if (c!=' ' && c<128 && x-xs+f->frames[c]->size.x>maxwidth)
			break;
		if (c==' ')
			x+=3;
		else if (c<128) {
			clipxpar32(f->frames[c],dest,
				0,0,x,y,f->frames[c]->size.x,f->frames[c]->size.y,C32BLACK);
			x+=f->frames[c]->size.x+1;
		}
	}
//	perf_end(DRAWSTRING);
}

static int ol_lendrawstring2(struct tsp *f,char *s,int maxwidth)
{
	if (!f)
		return 0;
	int x=0;
	unsigned int c;
	while(c=*s++ ) { // variable width
		if (c!=' ' && c<128 && x+f->frames[c]->size.x>maxwidth)
			break;
		if (c==' ')
			x+=3;
		else if (c<128)
			x+=f->frames[c]->size.x+1;
	}
	return x;
}

/*static void setfontopt(int v)
{
	od.fontopt=v;
}
*/
//static FILE *testwrite;

static void deactivateweap(struct ol_playerdata *p)
{
	tree2 *t;
	int ii,jj;
	if (p->xcurweapstate!=WEAPSTATE_DEPLOY) {
		if (p->xcurweapkind!=WEAP_WEAPBOOST)
			p->tweapbooston=0;
//		if (1)
//		if (p->xcurweapkind!=WEAP_SHLDBSTR)
			p->tshieldbooston=0;
	}
	ii=p->ttweapstolenfrom;
	jj=p->ttweapstolenslot;
	if (ii>=0) {
		opa[ii].vvweapstolen[jj]=0;
		p->ttweapstolenfrom=-1;

		t=opa[ii].weaptrees[jj];
		if (t) {
			unhooktree(t);
			linkchildtoparent(t,opa[ii].ol_carnull);
			t->flags|=TF_DONTDRAWC;
			seq_setframe(t,0);
		}
	}
	p->xcurweapstate=WEAPSTATE_DEPLOY;
	p->curweapvar=0;
	if (p->lastspecialweapslot>=0) {
		p->xcurweapslot=p->lastspecialweapslot;
		p->lastspecialweapslot=-1;
		p->xweapframe=0;
	}
}

static unsigned char wscratch[MAXWEAPONSCARRY];
static int pickrandomweap(struct ol_playerdata *p)
{
	int nw;
	unsigned char *wp;
	int i,poss;
	wp=carstubinfos[p->carid].cd.c_weaps;
	nw=carstubinfos[p->carid].cd.c_nweap;
	poss=0;
	for (i=0;i<nw;i++)
		if (!weapinfos[wp[i]].special)
			if (!p->vvweapstolen[i])
				if (p->xcurweapslot!=i || p->xcurweapstate==WEAPSTATE_DEPLOY)
					wscratch[poss++]=i;
	if (poss==0)
		return -1;
	return wscratch[prandom2(p->clocktickcount,poss)];
}

static int pickrandomweaprpd(struct ol_playerdata *victim,struct ol_playerdata *self)
{
	int nw,nwme;
	unsigned char *wp,*wpme;
	int i,k,poss;
	wp=carstubinfos[victim->carid].cd.c_weaps;
	nw=carstubinfos[victim->carid].cd.c_nweap;
	wpme=carstubinfos[self->carid].cd.c_weaps;
	nwme=carstubinfos[self->carid].cd.c_nweap;
	poss=0;
	for (i=0;i<nw;i++)
		if (!weapinfos[wp[i]].special)
			if (!victim->vvweapstolen[i])
				if (victim->xcurweapslot!=i || victim->xcurweapstate==WEAPSTATE_DEPLOY) {
					for (k=0;k<nwme;k++)
						if (wp[i]==wpme[k])
							break;
					if (k==nwme)
						wscratch[poss++]=i;
				}
	if (poss==0)
		return -1;
	return wscratch[prandom2(victim->clocktickcount,poss)];
}

static trackinfo* tinf;
static void customcfgload()
{
	string name,path;
// load default track .cfg first
pushandsetdirdown("config");
	if (fileexist("tdefault.cfg")) {
		loadconfigfile("tdefault.cfg",ol_debvars,OL_NDEBVARS); // probably can't go back to tim's stuff without
		logger("loading default track config file\n");
	}
popdir();
	name=tinf->gettrackcfgname(od.trackidx);
	path=tinf->gettrackdir(od.trackidx);
//	mgetpath(od.ol_uedittrackname,path);
//	mgetname(od.ol_uedittrackname,name);
pushandsetdir(path.c_str());
//	strcat(name.,".cfg");
	if (fileexist(name.c_str()))
		loadconfigfile(name.c_str(),ol_debvars,OL_NDEBVARS); // probably can't go back to tim's stuff without
popdir();
}

static pointf3 fusballverts[60];
static pointf3 fusballcollverts[12];

static void studyfusioncanball()
{
	return;
	int ncf=0;
	int i,k,vi;
	float d;
	struct pointf3 *v;
	int nf,nv;
	struct face *f;
	modelb *m;
	m=detachweapinfos[DETACH_FUSIONCAN].mastertree->mod;
	nf=m->faces.size();
	f=&m->faces[0];
	v=&m->verts[0];
	nv=m->verts.size();
	if (nf!=60 || nv!=180)
		errorexit("bad fusionball");
	for (i=0;i<nf;i++,f++) {
		for (k=0;k<3;k++) {
			vi=f->vertidx[k];
			d=dist3dsq(&v[vi],&zerov);
			if (d>.009f && d<.011f)
				break;
		}
		if (k==3)
			errorexit("can't find fusion rad");
		if (k==1) {
			vi=f->vertidx[1];
			f->vertidx[1]=f->vertidx[2];
			f->vertidx[2]=f->vertidx[0];
			f->vertidx[0]=vi;
		} else if (k==2) {
			vi=f->vertidx[2];
			f->vertidx[2]=f->vertidx[1];
			f->vertidx[1]=f->vertidx[0];
			f->vertidx[0]=vi;
		}
		normalize3d(&v[vi],&fusballverts[i]);
	}
	for (i=0;i<nv;i++) {
		d=dist3dsq(&v[i],&zerov);
		if (d>.019f && d<.021f) {
			for (k=0;k<ncf;k++)
				if (dist3dsq(&v[i],&fusballcollverts[k])<.00001f)
					break;
			if (k==ncf) {
				if (ncf>=12)
					errorexit("too many fusballcollverts");
				fusballcollverts[ncf++]=v[i];
			}
		}
	}
	if (ncf!=12)
		errorexit("%d coll verts for fusball, need 12",ncf);
}

static pointf3 fusioncollpoints[12]; // or 20 depending...

/* NYI
static void alterfusionball()
{
	float dist;
	float sum,diff;
	int i,vi;
	struct pointf3 *v,*n,da,db;
	int nf;
	struct face *f;
	modelb *m;
	m=detachweapinfos[DETACH_FUSIONCAN].mastertree->mod;
	nf=m->faces.size();
	f=&m->faces[0];
	v=&m->verts[0];
	n=&m->norms[0];
	sum=(od.fusionmorphmin+od.fusionmorphmax)*.5f;
	diff=(od.fusionmorphmax-od.fusionmorphmin)*.5f;
	dist=sum-diff*rcos((TWOPI/od.fusionmorphframe)*
		(opa[0].clocktickcount&(od.fusionmorphframe-1)));
	for (i=0;i<nf;i++,f++) {
		vi=f->vertidx[0];
		v[vi].x=fusballverts[i].x*dist;
		v[vi].y=fusballverts[i].y*dist;
		v[vi].z=fusballverts[i].z*dist;
		da.x=v[vi].x-v[f->vertidx[1]].x;
		da.y=v[vi].y-v[f->vertidx[1]].y;
		da.z=v[vi].z-v[f->vertidx[1]].z;
		db.x=v[vi].x-v[f->vertidx[2]].x;
		db.y=v[vi].y-v[f->vertidx[2]].y;
		db.z=v[vi].z-v[f->vertidx[2]].z;
		cross3d(&da,&db,&n[vi]);
		normalize3d(&n[vi],&n[vi]);
		n[f->vertidx[1]]=n[vi];
		n[f->vertidx[2]]=n[vi];
	}
	m->setbbox();
}
*/



void setrematchvis(int v)
{
//	v=1;
	PTEXTQ->setvis(v);
	PTEXTQ2->setvis(v);
	PBUTYES->setvis(v);
	PBUTNO->setvis(v);
	BACK->setvis(v);
}

void onlineracemain_init()
{
	string name,namebws,path,skyboxname;
	tinf=new trackinfo;
	od.trackidx=tinf->findtrackidx(od.ntrackname);
	if (od.trackidx<0) {
		od.trackidx=0;
		od.badtrack=true;
	}
	name=tinf->gettrackname(od.trackidx);
	namebws=tinf->gettracknamebws(od.trackidx);
	path=tinf->gettrackdir(od.trackidx);
	skyboxname=tinf->gettrackskybox(od.trackidx);
	od.ntrackscale=tinf->gettrackscale(od.trackidx);
	od.trackstart=tinf->gettrackstart(od.trackidx);
	od.trackend=tinf->gettrackend(od.trackidx);
	logger("sizeof playerpacket is %d\n",sizeof(playerstatepacket));
	logger("sizeof globalpacket is %d\n",sizeof(globalstatepacket));
	logger("sizeof weapfly is %d\n",sizeof(weapfly));
	logger("sizeof colltree is %d\n",sizeof(colltree));
	runinbackgroundsave=wininfo.runinbackground;
	wininfo.runinbackground=1;

//		char name[300],path[300];
// see if have the track...
/*pushandsetdir("racenetdata/onlineopt");
	script *sc=new script("online_seltrack.txt");
	S32 i,j;
	for (i=0;i<sc->num();i+=SELTRACKWID)
		if (!my_stricmp(sc->idx(i+1).c_str(),od.ol_uedittrackname))
			break;
	if (i==sc->num()) { // can't find track, switching to default track
//		disconnectandai();
//		freesocker(od.playernet[0].tcpclients);
//		od.playernet[0].tcpclients=0;
		ol_opt.badtrack=true;
		mystrncpy(od.ol_uedittrackname,sc->idx(1).c_str(),NAMESIZEI);
		ol_opt.trackscale=(float)atof(sc->idx(2).c_str());
		ol_opt.trackstart=(float)atof(sc->idx(3).c_str());
		ol_opt.trackend=(float)atof(sc->idx(4).c_str());
		mystrncpy(ol_opt.skyboxname,sc->idx(5).c_str(),NAMESIZEI);
		if (!ol_opt.skyboxname[0])
			strcpy(ol_opt.skyboxname,"-");
	}
	delete sc;
	sc=0;
popdir();
*/

	if (od.goingtogame) {
//		struct bitmap32 *b1=0;//,*b2;
//		mgetpath(od.ol_uedittrackname,path);
//		mgetname(od.ol_uedittrackname,name);
pushandsetdir(path.c_str());
		logger("loading '%s' mat2shader\n",path.c_str());
		video_reload_mat2shader("mat2shader.txt");
/*		strcat(name,".jpg");
		if (fileexist(name)) {
//			fileopen(name,READ);
			b1=gfxread32(name);
		} else {
//			if (currule->useweap) {
pushandsetdirdown("loading");
//			fileopen("loadingbackground.jpg",READ);
			b1=gfxread32("loadingbackground.jpg");
//			} else {
//pushandsetdirdown("loadingvs");
//			fileopen("loadingbackground.jpg",READ);
//			b1=gfxread32("loadingbackground.jpg");

//			}
popdir();
		}
*/
popdir();
//		b1=gfxread24();
//		fileclose();
//		if (currule->useweap)
/*
pushandsetdirdown("loading");
//		else
//pushandsetdirdown("loadingvs");
//		if (WX==b1->x && WY==b1->y) {
//		if (0) {
//			od.loadingbackground=conv24to16ck(b1,BPP);
//			bitmap24free(b1);
			od.loadingbackground=b1;
//			fileopen("loadingcar.pcx",READ);
//			od.loadingcar=gfxread16(BPP);
//			fileclose();
//		} else {
			b2=bitmap32alloc(WX,WY,C32GREEN);
//			filt24to24deluxe(b1,b2,0,0,b1->x-1,b1->y-1,0,0,WX-1,WY-1);
			clipscaleblit32(b1,b2);
			bitmap32free(b1);
//			od.loadingbackground=conv24to16ck(b2,BPP);
			od.loadingbackground=b2;
//			bitmap24free(b2);
//			fileopen("loadingcar.pcx",READ);
//			b1=gfxread24();
//			fileclose();
//			b2=bitmap24alloc(
//				WX*b1->x/od.loadingbackground->x,WY*b1->y/od.loadingbackground->y,rgbblue);
//			clipscaleblit24(b1,b2,0,0,b1->x-1,b1->y-1,0,0,b2->x-1,b2->y-1);
//			bitmap24free(b1);
//			od.loadingcar=conv24to16ck(b2,BPP);
//			bitmap24free(b2);
//		}
//		fileopen("cursorpic.pcx",READ);
//		od.cursorpic=gfxread16(BPP);
//		fileclose();
		od.cursorpic=gfxread32("cursorpic.pcx");
popdir();
*/
	}

//	extern int extracttsp;
//	extern char winmain_version[];
//	int logtexsaveu,logtexsavev;
//	extern int dodefaultlights;
/*	pushandsetdir("fonts");
	od.hugefont=new fontq("med");
	od.hugefont->setspace(20);
	od.hugefont->setfixedwidth(20);
	popdir(); */
	logger("sizeof tree2 = %d\n",sizeof(tree2));
	struct bitmap32 *tbm16=0;
	pointf3 top,bot,intsect;
//	struct model *mod;
//	int blitmode;
//	extern int clipmap;	// for coins in scoreline
	tree2 *grid;//,/**xxx,*xxx2,*/ //*xxx3; // temp models for starting lights
	mater2 /**mt,*mt2,*/*mt3; // temp material(s) for starting lights
//	od.tscoret[PTEXT_TEST]=200;
//	od.cscoret[PTEXT_TEST]=0;
//	sprintf(od.scorestrings[PTEXT_TEST],"01234567890123456789");
//i=getfpucontrol();  // trap zero divide and overflow, temp
//i&=~6;
//setfpucontrol(i);
//	sethelperscale(12.0f); // debug ai
//	memset(&carstubinfos,0,sizeof(onegameinfos));
	mainstubinfo.finished=0;
	mainstubinfo.names.credits=0;
	mainstubinfo.names.experience=0;
	mainstubinfo.names.score=0;
	mainstubinfo.names.stunts=0;
	olracecfg.timeout=olracecfg.afterloadtimeout*30;
//	olracecfg.timeoutg=olracecfg.gameplaytimeout*60;
//	olracecfg.scorelinekind=olracecfg.stubtracknum&1; // odd numbered tracks get weapons
//	if (olracecfg.scorelinekind==0)
//		setwindowtext(winmain_versionvs);
//	else
//		setwindowtext(winmain_version);
	currule=&rules[olracecfg.rules];

//	if (olracecfg.scorelinekind==0)
//		setwindowtext(winmain_versionvs);
//	else
//		setwindowtext(winmain_version);
//	setwindowtext(currule->windowname);
pushandsetdirdown("packetlog");
//	testwrite=fopen("logfile.txt","w");
popdir();
//	logger("srand 1234\n");
//	setseed(1234);
#ifdef USEVARCHECKERrace
	pushandsetdir("..");	// move up to source
	addhfile("../engine1/numath.h");
	addhfile("online_varchecker.h");
	addhfile("online_uplay.h");
	addvar("struct ol_data","od",&od,1);
	addvar("struct ol_playerdata","opa",opa,OL_MAXPLAYERS);
	writevargen("vargen.c");
	popdir();
#endif
	od.piecesizex=ARTPIECESIZEX*od.ntrackscale;
	od.piecesizez=ARTPIECESIZEZ*od.ntrackscale;
	od.bigmove=20;
	od.hipoint=.5f;
//	od.showcon=0;
#ifdef USECON
//	od.ucon=con16_alloc(200,128,C32WHITE,hiblack);
#endif
//	of=&ocp;
	od.pingmode=1-wininfo.releasemode;
//	od.tempdisablekillwait=5*60;
	od.ol_timeincconst=1/30.0f;
//	ocp.curweap=-1;
//	ocp.lastcurweap=-1;
//	ocp.chkpnt=-1;
	ocp.woc=-1;
	ocp.xcurweapslot=-1;
	ocp.ttweapstolenfrom=-1;
	ocp.xcurweapstate=WEAPSTATE_DEPLOY;
	ocp.xweapframe=0;
	ocp.curweapvar=0;
	ocp.lastactiveweapslot=ocp.lastspecialweapslot=-1;

	ocp.pos.y=.2f;
	ocp.ol_nuflashes=1;	// force texture
	ocp.dpf=1.0f;
//	ocp.weapstolen=-1;
// setup video
/*	if (od.ol_releasemode) {
		wininfo.joyenable=1;
		mousemode=MM_INFINITE; // do we still want mouse support???
	} */
//	if (video_maindriver!=VIDEO_GDI || mousemode==MM_INFINITE || usedirectinput)
	if (videoinfo.video_fullscreen || wininfo.mousemode==MM_INFINITE || wininfo.usedirectinput)
		showcursor(0);
	od.uplaycursor=0;
	ho=new helperobj();

//	inithelper();
//	ocp.ol_grandtotaltrickpoints = 0;
//	ocp.ol_trackbonus = od.ol_timetocomplete = 0;
	od.ol_trackworth = od.ol_trackscore = 0;
//	usescncamera=0;

	od.ol_uloop=od.defuloop;
pushandsetdirdown("onlineopt");
	newcarnames = new script("thecars.txt");
	init_res3d(1024,768);
	game_rl = res_loadfile("online_rematchres.txt");
/*
//	game_rl=loadres("online_rematchres.txt");
	factory2<shape> fact;
	script* msc=new script("online_rematchres.txt");
	game_rl=fact.newclass_from_handle(*msc);
	delete msc;
*/
	focus=0;
	PTEXTQ=game_rl->find<text>("PTEXTQ");
	PTEXTQ2=game_rl->find<text>("PTEXTQ2");
	PBUTYES=game_rl->find<pbut>("PBUTYES");
	PBUTNO=game_rl->find<pbut>("PBUTNO");
	BACK=game_rl->find<back>("BACK");
	GAMENEWS=game_rl->find<listbox>("GAMENEWS");
	GAMENEWS->setmax(200);
	GAMENEWS->printf("Game started: np %d,nc %d,nb %d",od.numplayers,od.numcars,od.numbots);

//	setresvis(game_rl,PBACK1,0);
//	setresvis(game_rl,PTEXTQ,0);
//	setresvis(game_rl,PBUTYES,0);
//	setresvis(game_rl,PTEXTQ2,0);
//	setresvis(game_rl,PBUTNO,0);
/*	PTEXTQ->setvis(0);
	PTEXTQ2->setvis(0);
	PBUTYES->setvis(0);
	PBUTNO->setvis(0);
*/
	setrematchvis(0);
	extradebvars(ol_debvars,OL_NDEBVARS);
	loadconfigfile("config.txt",ol_debvars,OL_NDEBVARS); // probably can't go back to tim's stuff without
//	loadprivateconfigfile("config.txt",ol_debvars,OL_NDEBVARS); // probably can't go back to tim's stuff without
									// reloading the real config.txt
	od.quittime*=60;
//	logger("quittime set to %d\n",od.quittime);
	od.ol_newpitch.x=1;
	od.ol_newpitch.w*=PIOVER180;
popdir();
//	video_setupwindow(globalxres,globalyres,DESIREDBPP);
/*	if (BPP==565)
		od.blitmode=TEX565CK;
	else
		od.blitmode=TEX555CK;
	if (texavail[TEX4444A])
		od.texfmt=TEX4444A;
	else if (texavail[TEX5551A])
		od.texfmt=TEX5551A;
	else if (texavail[TEX565CK])
		od.texfmt=TEX565CK;
	else if (texavail[TEX555CK])
		od.texfmt=TEX555CK;
	else
		errorexit("can't find a tex format for anim tex");
	if (texavail[TEX565NCK])
		od.texfmtnck=TEX565NCK;
	else if (texavail[TEX555NCK])
		od.texfmtnck=TEX555NCK;
	else
		errorexit("can't find a tex format for anim tex nck"); */
	ocp.ol_intunnels=0;
//	ocp.ol_cantstartdrive=INITCANTSTARTDRIVE;
//	od.ol_usehires=1;
	od.ol_useoldcam=20;
	ocp.ol_cantdrive=60;
	ocp.ol_carboost=0;
	ocp.ol_validroadnorm=0;
//	od.ol_framecount=0;
	ocp.dofinish=0;
//	ol_alphacutoffsave=ol_alphacutoff;
//	ol_alphacutoff=137;
	ocp.ol_utotalcrashes=ocp.ol_utotalstunts=0;
// clock
//	od.ol_clocktickcount=(od.ol_ustarttime*30+29)*2;	// 2 minutes
	ocp.clocktickcount=0; //(od.ol_ustarttime*30+29)*2;	// 2 minutes
// music
	pushandsetdirdown("st2_main");
//	od.ol_sag=play_sagger("menu.sag");
	od.ol_sag=vag_load("menu.sag");
	wave_play(od.ol_sag,0);
	popdir();
// setup main tree
	od.ol_root = alloctree(30000,"game_main_root");
//	C32 bk=mainvp.backcolor;
	memset(&mainvp,0,sizeof(mainvp));
//	mainvp.backcolor=bk;
	mainvp.backcolor=C32WHITE;
//	mainvp.backcolor=frgbgreen;
//	mainvp.backcolor=frgbbrown;
	mainvp.zfront=.125f; //.35f; // .125f;
/*	if (zoomin) {
		mainvp.xres=WX/2;
		mainvp.yres=WY/2;
		mainvp.xstart=WX/2;
		mainvp.ystart=WY/2;
*/
// } else {
		mainvp.xres=WX;
		mainvp.yres=WY;
		mainvp.xstart=0;
		mainvp.ystart=0;
//	}
	mainvproottree=od.ol_root;
												// get a cam(null)
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
//	setviewportsrc(&mainvp); // from last scene loaded
	mainvp.xsrc=4;mainvp.ysrc=3;
	mainvp.flags=VP_CLEARWB;//|VP_CLEARBG;

// get main scene
//	fonttools();
	pushandsetdirdown("fontdump");
//	extracttsp=1;
	od.ol_tscorefont3=loadtspo("font2.tsp",NULL,od.blitmode,0);
//	extracttsp=1;
	od.ol_tscorefont1=loadtspo("font1.tsp",NULL,od.blitmode,0);
//	od.ol_tscorefont=loadtspo("font.tsp",NULL,blitmode,0);
popdir();
pushandsetdirdown("st2_uplay");
	od.ol_tscorefont2=duptsp(od.ol_tscorefont3);
/*	fileopen("1stplace.pcx",READ);
	od.jplacebm[0]=gfxread16(BPP);
	fileclose();
	fileopen("2ndplace.pcx",READ);
	od.jplacebm[1]=gfxread16(BPP);
	fileclose();
	fileopen("3rdplace.pcx",READ);
	od.jplacebm[2]=gfxread16(BPP);
	fileclose();
	fileopen("4thplace.pcx",READ);
	od.jplacebm[3]=gfxread16(BPP);
	fileclose(); */
/*	od.jplacebm[0]=gfxread32("1stplace.pcx");
	od.jplacebm[1]=gfxread32("2ndplace.pcx");
	od.jplacebm[2]=gfxread32("3rdplace.pcx");
	od.jplacebm[3]=gfxread32("4thplace.pcx"); */
/*	colorkeyinfo.lasthascolorkey=1;
	od.jplacespt[0]=texture_create("1stplace.pcx");
	od.jplacespt[0]->load();
	od.jplacespt[1]=texture_create("2ndplace.pcx");
	od.jplacespt[1]->load();
	od.jplacespt[2]=texture_create("3rdplace.pcx");
	od.jplacespt[2]->load();
	od.jplacespt[3]=texture_create("4thplace.pcx");
	od.jplacespt[3]->load(); */
	pushandsetdirup(); // push 'st2_uplay', up to racenetdata
/*	setdirdown("2009_dirfinder");  // down to 2009_dirfinder
	od.cface=texture_create("cface.pcx");
	od.cface->load();
	od.carrow=texture_create("carrow.pcx");
	od.carrow->load(); */
	popdir();	// back to st2_uplay
/*	pushandsetdir("weapicons");
	for (i=0;i<MAX_WEAPKIND;i++) {
		sprintf(name,"%s.pcx",weapinfos[i]);
		fileopen(name,READ);
		od.weapicons[i]=gfxread16(BPP);
		fileclose();
	}
	popdir(); */
	od.weapbm=bitmap32alloc(76,13*8,C32BLACK);
//	for (i=0;i<128;i++)
//		logger("font %2d: w %d, h %d\n",i,od.ol_tscorefont1->frames[i]->x,od.ol_tscorefont1->frames[i]->y);
//	mgetpath(od.ol_uedittrackname,path);
popdir(); //  up to racenetdata
// load weapon xlat table
//pushandsetdir("racenetdata/onlineopt");
//	od.weapxlate=new script("weapxlate.txt");
//popdir();
	od.parts=new particle();
pushandsetdir(path.c_str());  // down to track
/*	if (od.ol_uedittrackname[0]=='\0') {
//		usescnlights = 1;
		grid = new tree2("uplay.mxs");
		grid->scale=pointf3x(od.ntrackscale,od.ntrackscale,od.ntrackscale);
		linkchildtoparent(grid,od.ol_root);
//		usescnlights = 0;
	} else { */
		char str2[200];
//		extern int dodefaultlights;
//		mgetnameext(od.ol_uedittrackname,name);
//		dodefaultlights=0;
//		usescnlights = 1;
		grid = new tree2(namebws.c_str()); // this will load track art
//		grid->flags|=TF_DONTDRAWC;
		linkchildtoparent(grid,od.ol_root);
		st2_setupmatfuncs(ol_matfuncs,OL_NMATFUNCS); // turn on
//		mgetname(od.ol_uedittrackname,str);
		sprintf(str2,"%s.txt",name.c_str());
/*		if (video_maindriver!=VIDEO_D3D) {
			tree2 *t;
			t=findtreenamerec(grid,"4p_glass.bwo");
			if (t) {
				t->dissolve=.5f;
				if (t->mod)
					t->mod->drawpri=3;
			}
		} */
		splitbigmodels(grid);
		grid->scale=pointf3x(od.ntrackscale,od.ntrackscale,od.ntrackscale);
		st2_loadcollist(str2,od.ol_root); // load alot of objects (init) (collist)
		initcolltree();
		sprintf(str2,"%s.ogg",name.c_str());
		od.backwave=wave_load(str2,0);
popdir(); // back to st2_uplay
// load a skybox
pushandsetdir("skybox");
		if (skyboxname.size()) {
//		if (od.ol_skyboxname[0] && od.ol_skyboxname[0]!='-') {
			tree2* askybox=buildskybox(pointf3x(10,10,10),skyboxname.c_str(),"tex");
//			tree2* askybox=buildskybox(pointf3x(100,100,100),"cubemap_mountains.jpg","tex");
			od.ol_root->linkchild(askybox);
		}
//		pushandsetdir("gfxtest");
//		make_envv2tex("maptestnck.tga");
//		popdir();
//		make_envv2tex("CUB_cubicmap.jpg");
		C8 cubstr[100];
		sprintf(cubstr,"CUB_%s",skyboxname.c_str());
		make_envv2tex(cubstr);
popdir();
// find the old rabbit paths if they exist
		od.path1=findtreenamerec(od.ol_root,"path1.bwo");
		od.path2=findtreenamerec(od.ol_root,"path2.bwo");
		customcfgload();
		GAMENEWS->setvis(showgamenews);
		od.y2009carrot.x*=PIOVER180;
		od.y2009carrot.y*=PIOVER180;
		od.y2009carrot.z*=PIOVER180;
		if (currule->rboxes) {
pushandsetdir(path.c_str());  // down to track
			boxai_init();
popdir();
		} else {
			crasheditload();
			checkeditload();
//		boteditload();
			buildcheckresets();
		}
		coinsinit(grid);
//		calccrashresets();
//		usescnlights = 0;
//		popdir();
//	}
	mainvp.zback=od.ol_zback;
	od.startcamrotx=-.3514f;
//	logtexsaveu=wininfo.maxtexlogu;
//	logtexsavev=wininfo.maxtexlogv;
//	wininfo.maxtexlogu=wininfo.maxtexlogv=8;
	od.ol_startpiece=alloctree(0,"start.lwo");
	od.ol_startpiece->scale.x=2;
//	wininfo.maxtexlogu=logtexsaveu;
//	wininfo.maxtexlogv=logtexsavev;
	mt3=findmaterial(od.ol_startpiece,"chkrdflg");
	if (mt3)
		od.starttex=mt3->thetexarr[0];
//	convertfont(od.ol_tscorefont2,od.blitmode,od.starttex->texformat);

	if (od.starttex) {
		tbm16=locktexture(od.starttex);
		od.oldstarttex=bitmap32alloc(tbm16->size.x,tbm16->size.y,0);
		clipblit32(tbm16,od.oldstarttex,0,0,0,0,tbm16->size.x,tbm16->size.y);
		unlocktexture(od.starttex);
	}
	S32 i,j;
	for (i=0;i<od.numcars;i++) {
		C8 cname[200];
		strcpy(cname,carstubinfos[i].cd.c_name);
//	name[16]='\0';
		my_strupr(cname);
		j=ol_lendrawstring2(od.ol_tscorefont2,cname,256);
		if (od.numcars>=2 && od.numcars<=4)
			ol_drawstring2(od.ol_tscorefont2,tbm16,cname,(256-j)>>1,bannery[od.numcars-2][i],256);
		else
			ol_drawstring2(od.ol_tscorefont2,tbm16,cname,(256-j)>>1,20*i,256);
//		ol_drawstring(od.ol_tscorefont2,tbm16,name,128-strlen(name)*8,20*i);
//	ol_drawstring(od.ol_tscorefont2,tbm16,"VS.",104,54);
	}
//	strcpy(name,stubinfo.p2name);
//	name[16]='\0';
//	my_strupr(name);
//	ol_drawstring(od.ol_tscorefont2,tbm16,name,128-strlen(name)*8,91);

//	od.ol_finishpiece=loadlws("finish.lws");
	od.ol_finishpiece=alloctree(1,"finish.lwo");
//	od.ol_nofinishpiece=findtreenamerec(od.ol_finishpiece,"redx.lwo");
	od.ol_finishpiece->scale.x=2;
	linkchildtoparent(od.ol_startpiece,od.ol_root);
	linkchildtoparent(od.ol_finishpiece,od.ol_root);
	ol_initfinishbursts();
//setdirup();
//	popdir();

// setup some test nulls
//	initnulls(od.ol_root,.45f);
//	initnulls(od.ol_root,4.5f);
//	initvecs(od.ol_root);
//	initplanes(od.ol_root,3);
// load and build track
/*	if (od.ol_uedittrackname[0]=='\0') {
		pushandsetdir("st2_edpieces");
		od.ol_numpieces = ol_st2_loadtracklist2("uedit.txt");
		popdir();
		ol_st2_loadtrack("track1.txt",1);
		if (od.ol_st2_curpiece==6) { // add an extra straight for a blank track
			od.ol_st2_curpiece=13;
			od.ol_st2_lastpiece=6;
			od.ol_st2_dir=0;
			od.ol_thetrack[od.ol_st2_lastpiece].piece=PIECE_GENERIC_3; // straight
			od.ol_thetrack[od.ol_st2_lastpiece].or=0;
			od.ol_thetrack[od.ol_st2_lastpiece].flor=0;
		}
		// substitute last blank piece with a straight
		od.ol_thetrack[od.ol_st2_curpiece].piece=PIECE_GENERIC_25; // specialfinish.lws
		//thetrack[st2_curpiece].or=(st2_dir*90+180); // ah theirs is 180 off
		for(i=0;i<NUMBODS;i++) {
			tree2 *aaa;
			char a[64];
			int p;
	//		logger("<<<<<<<<<<<<<<<<<< trackpiece %d\n",i);
			p = od.ol_thetrack[i].piece;
			sprintf(a,"%02d.bwo",i);
			if (od.ol_pieces[p].piece)
				od.ol_thetrack[i].t = duptree(od.ol_pieces[p].piece);
			else {
				pushandsetdir("st2_edpieces");
				od.ol_thetrack[i].t = od.ol_pieces[p].piece = new tree2(od.ol_pieces[p].name);
				popdir();
			}
			if(i==od.ol_st2_curpiece)
				od.ol_thetrack[i].t->rot.y = PIOVER180 * (od.ol_st2_dir*90+180);
			else
				od.ol_thetrack[i].t->rot.y = PIOVER180 * od.ol_thetrack[i].or;
			aaa = findtreename(&mainvp,a);
			linkchildtoparent(od.ol_thetrack[i].t,aaa);
		}
		ol_scantrackpieces();
		ol_initcoins();
		st2_setupmatfuncs(ol_matfuncs,OL_NMATFUNCS);
		pushandsetdir("st2_edpieces");
		st2_loadcollist("qcollist.txt",od.ol_root); // load alot of objects (init) (collist)
		popdir();
	} else */{
//		char str[200],str2[200];
//		od.lowpoint=od.ntrackscale*-2000.0f;
//		st2_setupmatfuncs(ol_matfuncs,OL_NMATFUNCS);
//		pushandsetdir(path);
//		getname(str,od.ol_uedittrackname);
//		sprintf(str2,"%s.txt",str);
//		st2_loadcollist(str2,od.ol_root); // load alot of objects (init) (collist)
//		popdir();
	}
// handle collisions
	setupai(od.ol_root);
// build up our car, at this point we use op structure instead of of(template)
pushandsetdirdown("onlineopt"); // racenetdata/onlineopt
//	od.detachweaplist=loadscript("detachweaps.txt",&od.ndetachweaplist);
//	od.freeweaplist=loadscript("freeweaps.txt",&od.nfreeweaplist);
	od.detachweaplist=new script("detachweaps.txt");
	od.freeweaplist=new script("freeweaps.txt");
popdir();
	for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
		j=op->playerid; // save playerid
		*op=ocp;
		op->playerid=j; // restore playerid
		if (ol_opt.nt==NET_BCLIENT) {
			if (i>=od.numcars-od.numbots) { // only bclient has nonzero numbots {
				randombot(i);
			}
		} else { // bot game most likely
			if (i>=od.numcurplayers) {
				randombot(i); // load name,body,weapons colors etc. // just the structure(s), no loading
			}
		}
		op->seekframe=od.trackstart; // rabbits are timewarp broken with numbots>0, who cares?
//		ol_st2_buildcar(0);
//		ol_st2_buildcar(1);
		op->carid=i; // slotnum
//		op->playerid=i;
//		ol_st2_buildcar(&ol_opt.popt[i]);
//		ol_st2_buildcar(i%4+2);
// setup objects
//	memcpy(ol_objects,ol_iobjects,sizeof(ol_objects));
		if (currule->rboxes) {
			boxai_getstartpos();
			op->ol_ulastpos=op->pos;
//			&op->pos,&op->carang,i);
		} else {
			if (od.ntrackname[0]=='\0' || !od.path1) {
				op->pos.x=-3.7f*od.piecesizex-.75f*(i>>2);
				op->pos.z=-3.0f*od.piecesizez-.5f*(i&3)+.25f*(4-1);
			} else {
				float x,z;
				pointf3 pdirx,pdirz;
				pointf3 pointx={1,0,0,0};
				pointf3 pointz={0,0,1,0};
				od.path1->curframe=od.trackstart;
				od.path1->proc();
				quatrot(&od.path1->rot,&pointx,&pdirx);
				quatrot(&od.path1->rot,&pointz,&pdirz);
				pdirx.y=pdirz.y=0;
				normalize3d(&pdirx,&pdirx);
				normalize3d(&pdirz,&pdirz);
				z=-.75f*((i+10)/5);
				x=.4f*((i+1)%5)-.25f*(4-1);
				op->pos.x=od.path1->trans.x*od.ntrackscale+x*pdirx.x+z*pdirz.x;
				op->pos.z=od.path1->trans.z*od.ntrackscale+x*pdirx.z+z*pdirz.z;
	/*			if (i==0) {
					extern FILE *fwgs;
					initfwgs();
					fprintf(fwgs,"curframe %08x, pdirx %08x, pdiry %08x, pdirz %08x\n",*(int *)&od.trackstart,*(int *)&pdirx.x,*(int *)&pdirx.y,*(int *)&pdirx.z);
					fprintf(fwgs,"posx %08x, posy %08x, posz %08x\n",*(int *)&op->pos.x,*(int *)&op->pos.y,*(int *)&op->pos.z);
				} */
			}
			top=bot=op->pos;
			bot.y=-2.0f;
			top.y=2.0f;
			if (st2_line2roadlo(&top,&bot,&intsect,NULL)) {
				op->pos=intsect;
				op->pos.y+=od.hipoint;
			} else {
				bot.y=-2000.0f;
				top.y=2000.0f;
				if (st2_line2roadlo(&top,&bot,&intsect,NULL)) {
					op->pos=intsect;
					op->pos.y+=od.hipoint;
				}
			}
		}
/*		if (od.ol_uedittrackname[0] && od.path1) {
			op->ol_crashresetloc=op->pos;
			quat2rpy(&od.path1->rot,&top);
			op->ol_crashresetdir=top.y;
		} */
		op->pos.w=0;
		op->ol_crashresetloc=op->pos;
		ol_st2_buildcar(i);
//	objects[0].pos.x=100;
//	roadprobe1.z=100;
		if (!currule->rboxes) {
			if (od.ntrackname[0]=='\0' || !od.path1) {
				op->carang.x=0; // yaw car 90 degrees to the right
				op->carang.y=1;
				op->carang.z=0;
				op->carang.w=PI/2.0f;
				rotaxis2quat(&op->carang,&op->carang);
			} else {
				op->carang=od.path1->rot;
			}
		}
		op->ol_crashresetrot=op->carang;
// set misc variables
		op->ol_startstunt=op->ol_uready=0;
		op->ol_accelspin=0;
		op->ol_littleg=op->ol_littlegground;
		for (j=0;j<MAXENERGIES;j++) {
			op->tenergies[j]=2; // try start out with 2 energies each.
			op->charges[j]=carstubinfos[i].cd.c_energies[j]; // charge rate, a constant, no time warp
		}
	}
// load up graphics for fountains
	loadnewdetach(DETACH_MISSILES);
//	loaddetach(DETACH_ECTO);
	loadnewdetach(DETACH_LASER);
	loadnewdetach(DETACH_MINIGUN);
	loadnewdetach(DETACH_PARTICLE);
//	loaddetach(DETACH_EMB);
	loadnewdetach(DETACH_OIL);
	loadnewdetach(DETACH_MINE);


// get mine texture
	if (detachweapinfos[DETACH_MINE].mastertree)
		od.minetex=detachweapinfos[DETACH_MINE].mastertree->mod->mats[0].thetexarr[0];
	else
		od.minetex=0;
	if (od.minetex) {
/*		struct rgb24 ca={0,0,255};
		struct rgb24 cd={1,0,0};
		if (od.minetex->texformat==TEX555NCK) {
			od.minearmcolor=color24to555(ca);
			od.minedisarmcolor=color24to555(cd);
		} else if (od.minetex->texformat==TEX565NCK) {
			od.minearmcolor=color24to565(ca);
			od.minedisarmcolor=color24to565(cd);
		} else
			od.minetex=NULL; */
		od.minearmcolor=C32LIGHTRED;
		od.minedisarmcolor=C32(0,0,1);
	}

//	i=clipmap;
//	if (video_maindriver!=VIDEO_D3D)
//		clipmap=CLIPMAP_NONE;
	loadnewdetach(DETACH_ECTO);
//	loadnewdetach(DETACH_ECTO2);
//	clipmap=i;
	loadnewdetach(DETACH_EMB);

	loadnewdetach(DETACH_GHOOK);
	loadnewdetach(DETACH_SONIC);
	loadnewdetach(DETACH_FUSIONCAN);
// get fuscan verts
	studyfusioncanball();

	loadnewdetach(DETACH_THERMOCAN);

	loadnewdetach(DETACH_GBOMB);
	loadnewdetach(DETACH_STASIS);
	loadnewdetach(DETACH_WALL);
	loadnewdetach(DETACH_RAMP);

//	freescript(od.detachweaplist,od.ndetachweaplist);
//	freescript(od.freeweaplist,od.nfreeweaplist);
	delete od.detachweaplist;
	od.detachweaplist=0;
	delete od.freeweaplist;
	od.freeweaplist=0;

	if (od.numplayers==1) {
//		od.numcars=4;
		opa[0].cntl=CNTL_HUMAN;
		od.playerid=0;
		opa[0].playerid=0;
		for (i=1;i<od.numcars;i++) {
			opa[i].playerid=0;
			opa[i].cntl=od.defaultcntl; // CNTL_AI
		}
	}
	if (olracecfg.forcebotmode)
		for (i=0;i<od.numcars;i++)
			if (od.playerid==opa[i].playerid)
				opa[i].cntl=CNTL_AI;


	op=NULL;
// setup finish onto path
	if (od.ntrackname[0]!='\0' && od.path1) {
		od.path1->curframe=od.trackstart;
		(od.path1->proc)();
		od.ol_startpiece->buildo2p=O2P_FROMTRANSQUATSCALE;
		od.ol_startpiece->rot=od.path1->rot;
		od.ol_startpiece->trans=od.path1->trans;
		od.ol_startpiece->trans.x*=od.ntrackscale;
		od.ol_startpiece->trans.y*=od.ntrackscale;
		od.ol_startpiece->trans.z*=od.ntrackscale;
		top=bot=od.ol_startpiece->trans;
		bot.y-=.125f;
		top.y+=.125f;
		if (st2_line2roadlo(&top,&bot,&intsect,NULL)) {
			od.ol_startpiece->trans=intsect;
//			od.ol_startpiece->trans.y=-.257813f;
		} else {
			bot.y-=20.0f;
			top.y+=20.0f;
			if (st2_line2roadlo(&top,&bot,&intsect,NULL)) {
				od.ol_startpiece->trans=intsect;
//			od.ol_startpiece->trans.y=-.257813f;
			} else {
				bot.y-=200.0f;
				top.y+=200.0f;
				if (st2_line2roadlo(&top,&bot,&intsect,NULL))
					od.ol_startpiece->trans=intsect;
			}
		}
		od.path1->curframe=od.trackend;
		(od.path1->proc)();
		od.ol_finishpiece->buildo2p=O2P_FROMTRANSQUATSCALE;
		od.ol_finishpiece->rot=od.path1->rot;
		od.ol_finishpiece->trans=od.path1->trans;
		od.ol_finishpiece->trans.x*=od.ntrackscale;
		od.ol_finishpiece->trans.y*=od.ntrackscale;
		od.ol_finishpiece->trans.z*=od.ntrackscale;
		top=bot=od.ol_finishpiece->trans;
		bot.y-=.125f;
		top.y+=.125f;
		if (st2_line2roadlo(&top,&bot,&intsect,NULL)) {
			od.ol_finishpiece->trans=intsect;
//			od.ol_finishpiece->trans.y=-.257813f;
		} else {
			bot.y-=20.0f;
			top.y+=20.0f;
			if (st2_line2roadlo(&top,&bot,&intsect,NULL)) {
				od.ol_finishpiece->trans=intsect;
	//			od.ol_finishpiece->trans.y=-.257813f;
			} else {
				bot.y-=200.0f;
				top.y+=200.0f;
				if (st2_line2roadlo(&top,&bot,&intsect,NULL))
					od.ol_finishpiece->trans=intsect;
		//			od.ol_finishpiece->trans.y=-.257813f;
			}
		}
//		ol_canfinish(1);
	}
//	of=&opa[curplayer_of];
// build up our camera
	od.ol_camnull=new tree2("camnull");//allochelper(0,HELPER_CAMERA,0);
	od.ol_camnull->buildo2p=O2P_FROMTRANSQUATSCALE;
	od.ol_camnull->rot.w=1;
	od.camnullrot=od.ol_camnull->rot;
	od.ol_camnull->trans=od.ol_camstart;
	mainvp.camattach=od.ol_camnull;
	mainvp.useattachcam=true;
	linkchildtoparent(od.ol_camnull,od.ol_root);
// get some sound
	ol_inittags();
	od.ol_numwaves=taggetnum(od.ol_gardentags);
//	revsound = Sound_FindSlot(MENUREV1+selectedcar); // good idea but wrong sounds
// load up scrline
	loadscoreline();

//	od.tlight=findtreenamerec(od.ol_scrline,"light.lwo");
//	od.tlight->flags|=TF_DONTDRAW;
//	od.tlight2=findtreenamerec(od.ol_scrline,"light2.lwo");
//	od.tlight2->flags|=TF_DONTDRAW;
//	logger("loading clock textures (digits)..\n");
// traffic light
	pushandsetdir("maxscrline");
	od.tlighttsp=loadtspo("ryg.tsp",NULL,od.texfmtnck,1);
	popdir();
/*	for (i=0;i<od.tlighttsp->nframe;i++) {
		int j,p;
		p=od.tlighttsp->x*od.tlighttsp->y;
		for (j=0;j<p;j++)
			od.tlighttsp->frames[i]->plane[j]=0xffff;
	} */
#if 0
	xxx3=od.ol_startpiece;
/*	xxx=od.tlight;
	xxx2=od.tlight2;

	mt=findmaterial(xxx,"red");
	mt2=findmaterial(xxx2,"red"); */
	mt3=findmaterial(xxx3,"red");
//	freetexture(mt->thetexarr[0]);
//	freetexture(mt2->thetexarr[0]);
	if (mt3)
		freetexture(mt3->thetexarr[0]);
	od.tlighttex[0]=/*mt->thetexarr[0]=mt2->thetexarr[0]=*/mt3->thetexarr[0]=
		buildtexture("rednu",od.tlighttsp->x,od.tlighttsp->y,od.texfmtnck);
//	mt->thetexarr[0]->refcount=3;

//	mt=findmaterial(xxx,"red2");
//	mt2=findmaterial(xxx2,"red2");
	mt3=findmaterial(xxx3,"red2");
//	freetexture(mt->thetexarr[0]);
//	freetexture(mt2->thetexarr[0]);
	if (mt3)
		freetexture(mt3->thetexarr[0]);
	od.tlighttex[1]=/*mt->thetexarr[0]=mt2->thetexarr[0]=*/mt3->thetexarr[0]=
		buildtexture("red2nu",od.tlighttsp->x,od.tlighttsp->y,od.texfmtnck);
//	mt->thetexarr[0]->refcount=3;

//	mt=findmaterial(xxx,"yellow");
//	mt2=findmaterial(xxx2,"yellow");
	mt3=findmaterial(xxx3,"yellow");
//	freetexture(mt->thetexarr[0]);
//	freetexture(mt2->thetexarr[0]);
	freetexture(mt3->thetexarr[0]);
	od.tlighttex[2]=/*mt->thetexarr[0]=mt2->thetexarr[0]=*/mt3->thetexarr[0]=
		buildtexture("yellownu",od.tlighttsp->x,od.tlighttsp->y,od.texfmtnck);
//	mt->thetexarr[0]->refcount=3;

//	mt=findmaterial(xxx,"green");
//	mt2=findmaterial(xxx2,"green");
	mt3=findmaterial(xxx3,"green");
//	freetexture(mt->thetexarr[0]);
//	freetexture(mt2->thetexarr[0]);
	freetexture(mt3->thetexarr[0]);
	od.tlighttex[3]=/*mt->thetexarr[0]=mt2->thetexarr[0]=*/mt3->thetexarr[0]=
		buildtexture("greennu",od.tlighttsp->x,od.tlighttsp->y,od.texfmtnck);
//	mt->thetexarr[0]->refcount=3;

	for (i=0;i<od.tlighttsp->nframe;i++) {
//		char tname[NAMESIZEI];
		logger("tlight %d: %d by %d\n",i,od.tlighttsp->frames[i]->size.x,od.tlighttsp->frames[i]->size.y);
	}
#endif
	od.lightstate=-1;
	changelightstate(0);
// clock
//	logtexsaveu=wininfo.maxtexlogu;
//	logtexsavev=wininfo.maxtexlogv;
//	wininfo.maxtexlogu=wininfo.maxtexlogv=8;
//	if (!currule->ctf)
//		od.ol_clocktsp=loadtsp("clock.tsp",NULL,mainvp2.roottree,"scrline.lwo","clock",TSP_USE_1ALPHA,1);
//JAY
	//od.ol_clocktsp=loadtsp("clock.tsp",NULL,mainvp.roottree,"scrline.lwo","clock",TSP_USE_1ALPHA,1);

// coin
//	xxx=findtreename(&mainvp2,"scrline.lwo");
//	if (!xxx)
//		errorexit("cant find scrline.lwo");
/*	mt = findmaterial(xxx,"scrline3");
	if(!mt)
		errorexit("Can't find coin surface");
// coin tex should be same as clock tex
	od.ol_numcointex=mt->thetexarr[0]; */
// speedo
//	od.ol_speedotsp=loadtsp("speed.tsp",NULL,mainvp2.roottree,"scrline.lwo","speed",TSP_USE_4ALPHA,1);
// energies
	if (currule->useweap) {
//	if (olracecfg.scorelinekind==1) {
/*		for (i=0;i<MAXENERGIES;i++) {
			struct texture *tx;
			od.lastenergies[i]=-1;
			mt=findmaterial(xxx,energymatnames[i]);
			if (!mt)
				errorexit("Can't find energy material '%s' in scoreline",energymatnames[i]);
			tx=buildtexture(energymatnames[i],od.ol_speedotsp->x,od.ol_speedotsp->y,
				od.ol_speedotsp->texformat);
			freetexture(mt->thetexarr[0]);
			od.energytex[i]=mt->thetexarr[0]=tx;
		} */
	}
// sort the groups in scoreline to put speed and energies last, (better draw order)
/*	mod=xxx->mod;
	j=mod->nmat-1;
	for (k=0;k<NSCL;k++) {
		for (i=0;i<mod->nmat;i++) {
			if (!strcmp(mod->mats[i].name,energymatnames[k])) {
				struct mat m;
				struct group g;
				m=mod->mats[j];
				g=mod->groups[j];
				mod->mats[j]=mod->mats[i];
				mod->groups[j]=mod->groups[i];
				mod->mats[i]=m;
				mod->groups[i]=g;
				j--;
				break;
			}
		}
	} */
// setup last,cur,next weap icons
	if (currule->useweap) {
//	if (olracecfg.scorelinekind==1) {
/*		mt=findmaterial(xxx,"weap_active");
		if (!mt)
			errorexit("can't find weap_active");
		od.curweaptex=mt->thetexarr[0];
		tbm16=locktexture(od.curweaptex);
		cliprect32(tbm16,0,0,tbm16->x-1,tbm16->y-1,0);
		unlocktexture(od.curweaptex);

		mt=findmaterial(xxx,"weap_prev");
		if (!mt)
			errorexit("can't find weap_prev");
		od.prevweaptex=mt->thetexarr[0];
		tbm16=locktexture(od.prevweaptex);
		cliprect32(tbm16,0,0,tbm16->x-1,tbm16->y-1,0);
		unlocktexture(od.prevweaptex);
		mt=findmaterial(xxx,"weap_next");
		if (!mt)
			errorexit("can't find weap_next");
		od.nextweaptex=mt->thetexarr[0];
		tbm16=locktexture(od.nextweaptex);
		cliprect32(tbm16,0,0,tbm16->x-1,tbm16->y-1,0);
		unlocktexture(od.nextweaptex);

		mt=findmaterial(xxx,"scrlne.TOP1");
		if (!mt)
			errorexit("can't find weapdesctex");
		od.weapdesctex=mt->thetexarr[0];
		mt=findmaterial(xxx,"scrlne.TOP2");
		if (!mt)
			errorexit("can't find splacetex");
		od.splacetex=mt->thetexarr[0];
		for (i=0;i<MAXJPLACE;i++)
			convertframe(od.jplacebm[i],blitmode,od.splacetex->texformat);
		od.ol_tscorefont4=duptsp(od.ol_tscorefont3);
		od.ol_tscorefont5=duptsp(od.ol_tscorefont3);
		convertfont(od.ol_tscorefont4,blitmode,od.weapdesctex->texformat);
		convertfont(od.ol_tscorefont5,blitmode,od.ol_speedotsp->texformat);
		od.lastprevweapicon=od.lastnextweapicon=od.lastcurweapicon=-2;
		od.curweapicon=od.prevweapicon=od.nextweapicon=-1;
		od.weapicontsp=loadtspo("weapicons.txt",NULL,od.curweaptex->texformat,1); */
	}
// get clock tsp
// get speed tsp
//	logger("loading speedo textures (digits)..\n");
//	wininfo.maxtexlogu=logtexsaveu;
//	wininfo.maxtexlogv=logtexsavev;
//	popdir();
// turn on animation
//	if (grid) {
		ol_kill_bcams(grid);
		seq_start(grid);
		seq_start(od.ol_finishpiece);
		if (od.path1)
			seq_stop(od.path1);
		if (od.path2)
			seq_stop(od.path2);
//		calclightonce(grid);
//	}
	pushandsetdir("st2_game");
	ol_uinitparticles();
	popdir();
// test objects
	pushandsetdir("helperobj");
//	grid=new tree2("testpole.mxs");
//	grid=new tree2("testbillboard.mxs");
//	i=uselights;
//	uselights=0;
	grid=new tree2("mtrail.mxs");
	od.mtrail=new tree2("mtrail");//duptree(findtreenamerec(grid,"mtrail.bwo"));
	od.prism=new tree2("prism");
	od.stasisfield=new tree2("stasisfield");
//	linkchildtoparent(grid,od.ol_root);
//	od.mtrail=duptree(grid->children[0]);
//	mt=&od.mtrail->mod->mats[0];
//	od.mtrail->mod->hastexvels=1;
//	mt->msflags|=SMAT_TEXVELS;
//	mt->texvel.u+=.05f;
//	mt->texvel.v+=.05f;
	freetree(grid);
#if 0
	od.mtrail->flags|=TF_ALWAYSFACING;//|TF_TREECOLOR;
//	od.mtrail->proc=mtrailproc; // NYI
	od.mtrail->scale=pointf3x(od.mtrailscale,od.mtrailscale,od.mtrailscale);

	grid=new tree2("stasisfield.mxs");
	od.stasisfield=duptree(findtreenamerec(grid,"stasisfield.bwo"));
//	linkchildtoparent(grid,od.ol_root);
//	od.mtrail=duptree(grid->children[0]);
//	mt=&od.mtrail->mod->mats[0];
//	od.mtrail->mod->hastexvels=1;
//	mt->msflags|=SMAT_TEXVELS;
//	mt->texvel.u+=.05f;
//	mt->texvel.v+=.05f;
	freetree(grid);
//	od.stasisfield->flags|=TF_ALWAYSFACING|TF_TREECOLOR;
//	od.stasisfield->proc=stasisfieldproc; // NYI
//	setVEC(&od.mtrail->scale,od.mtrailscale,od.mtrailscale,od.mtrailscale);

	grid=new tree2("chemtrail.mxs");
	od.chemtrail=duptree(findtreenamerec(grid,"chemtrail.bwo"));
	freetree(grid);
	od.chemtrail->flags|=TF_ALWAYSFACING;
// 	od.chemtrail->proc=chemtrailproc; // NYI
	od.chemtrail->scale=pointf3x(od.chemtrailscale,od.chemtrailscale,od.chemtrailscale);
//	uselights=1;
	grid=new tree2("prism.mxs");
	od.prism=duptree(findtreenamerec(grid,"prism.bwo"));
	freetree(grid);
	od.prism->buildo2p=O2P_FROMTRANSROTSCALE;
#endif
	popdir();
//	uselights=i;
/*	grid=findtreenamerec(od.ol_root,"mdlPTmissiles_A.bwo");
	if (grid) {
		grid=duptree(grid);
		top=intsect;
		top.y+=2;
		grid->trans=top;
		grid->proc=NULL;
		setVEC(&grid->scale,10,10,10);
		linkchildtoparent(grid,od.ol_root);
	} */
// log results
	od.lowpoint*=od.ntrackscale;
	ol_fixpondalphacutoff(od.ol_root);
// put the sky to low priority drawing
//	if (od.ol_uedittrackname[0])
//		ol_initcoins_mp();
	od.notspec=0;
	for (i=0;i<od.numcars;i++) {
		if (opa[i].playerid==od.playerid) {
			od.notspec=1;
			break;
		}
	}
	od.ol_uloop=0;
/*	pushandsetdir("helperobj");
	j=uselights;
	uselights=0;
	od.gothisway=new tree2("vectorc.mxs");
	uselights=j;
	dodefaultlights=1;
	linkchildtoparent(od.gothisway,xxx);
	popdir(); */
	globalchangematerial(mainvproottree);
//	logviewport(&mainvp,OPT_SOME);
//	setloadcallback(NULL); // NYI
//	bitmap32free(od.loadingbackground);
//	bitmap16free(od.loadingcar);
	logger("loadingcount done at %d/%d\n",od.loadingcount,olracecfg.loadinggoal1);
#ifdef USEVARCHECKERrace
	marknochange();
#endif
	updateweapdisplay();
//JAY

/*	if (currule->ctf)
		ol_initflags(); */

	logger("c0 pid %d, c1 pid %d, my pid %d\n",opa[0].playerid,opa[1].playerid,od.playerid);
//	uselights=1; // try this out, looks like the art has no normals, just for software prism
//	pushandsetdirdown("midifiles");
//	midi_load("carena1.mid");
//	popdir();
//	midi_play(1,0);
	od.backsound=wave_play(od.backwave,0);
	new_cam_init();
	initgstate();
	if (od.numplayers==1)
		od.allbotgame=1;
	focus=0;
	addnullinit();
	setfog(0);
}

//////// camera logic
//(rot)
static void ol_pickgroundcamrot() // set rotation relative to rotation of car (car relative)
{
	pointf3 q;
	q.x=1;
	q.y=0;
	q.z=0;
	q.w=od.ol_extracamang*PIOVER180;
	rotaxis2quat(&q,&q);
	if (of->ol_flymode==1 && of->ol_startstunt) // startstunt 1 means, not a good landspot calc
		quattimes(&of->carangsave,&q,&q);
	else
		quattimes(&of->carang,&q,&q);
	od.ol_qcamdesired=q;
}

// set camdesired to where car is centered given cameras rot, (move no rot)
static void ol_pickgroundcampos()
{
	mat4 x;
	pointf3 wv;
	wv.x=0;
	wv.y=0;
	wv.z=od.ol_camtrans.z;
	if (of->ol_intunnels)
		wv.z*=.75f;
	quat2xform(&od.camnullrot,&x);
	xformvec(&x,&wv,&wv);
	od.ol_camdesired.x=of->pos.x+wv.x;
	od.ol_camdesired.y=of->pos.y+wv.y;
	od.ol_camdesired.z=of->pos.z+wv.z;
//	logger("desired %f %f %f %f\n",od.ol_camdesired.x,od.ol_camdesired.y,od.ol_camdesired.z,od.ol_camdesired.w);
}

//(rot)
static void ol_pickaircamrot()  // target car to landspot (rot no move)
{
	pointf3 targ;
	targ.x=of->ol_landspot.x-of->pos.x;
	targ.y=of->ol_landspot.y-of->pos.y;
	targ.z=of->ol_landspot.z-of->pos.z;
	od.ol_qcamdesired.y=(float)atan2(targ.x,targ.z);
	od.ol_qcamdesired.x=(float)-atan2(targ.y,sqrt(targ.x*targ.x+targ.z*targ.z));
	od.ol_qcamdesired.z=0;
	rpy2quat(&od.ol_qcamdesired,&od.ol_qcamdesired);
}

static void ol_pickaircampos()
{
	pointf3 wv;
	mat4 x;
	wv.x=0;
	wv.y=0;
	wv.z=od.ol_camtrans.z;
	if (of->ol_intunnels)
		wv.z/=2;
	quat2xform(&od.camnullrot,&x);
	xformvec(&x,&wv,&wv);
	od.ol_camdesired.x=of->pos.x+wv.x;
	od.ol_camdesired.y=of->pos.y+wv.y;
	od.ol_camdesired.z=of->pos.z+wv.z;
}

// set rot to look at car no matter what (bypasses qcamdesired) (rot no move)
static void ol_targetcamrot()
{
	pointf3 targ;
	pointf3 qcar,carrpy;
	mat4 xcar;
	pointf3 targrot;
	pointf3 xftarg;
	pointf3 qcamr1,qcamr2;
	pointf3 qpit,qcarrot;
	float pitchthresh,rollthresh;
	targ.x=of->pos.x-od.ol_camnull->trans.x;
	targ.y=of->pos.y-od.ol_camnull->trans.y;
	targ.z=of->pos.z-od.ol_camnull->trans.z;
//	logger("pos %f %f %f %f\n",of->pos.x,of->pos.y,of->pos.z,of->pos.w);
//	logger("cam %f %f %f %f\n",od.ol_camnull->trans.x,
//		od.ol_camnull->trans.y,od.ol_camnull->trans.z,od.ol_camnull->trans.w);
	quatinverse(&of->carang,&qcar);
	rotaxis2quat(&od.ol_newpitch,&qpit);
	quattimes(&qpit,&qcar,&qcar);
	quat2xform(&qcar,&xcar);
	xformvec(&xcar,&targ,&xftarg);
	targrot.y=(float)atan2(xftarg.x,xftarg.z);
	targrot.x=(float)-atan2(xftarg.y,sqrt(xftarg.x*xftarg.x+xftarg.z*xftarg.z));
	targrot.z=0;
	rpy2quat(&targrot,&qcarrot);
	quatinverse(&qcar,&qcar);
	quattimes(&qcar,&qcarrot,&qcamr2);
//	logger("targ %f %f %f %f\n",targ.x,targ.y,targ.z,targ.w);
	targrot.y=(float)atan2(targ.x,targ.z);
	targrot.x=(float)-atan2(targ.y,sqrt(targ.x*targ.x+targ.z*targ.z));
	targrot.z=0;
	od.ol_testcam=targrot;
//	logger("testcam %f %f %f %f\n",targrot.x,targrot.y,targrot.z,targrot.w);
	quat2rpy(&of->carang,&carrpy);
	od.ol_testcar=carrpy;
	rpy2quat(&targrot,&qcamr1);
	if (!of->ol_flymode) {
		if (od.ol_testnewcam==1) {
			pitchthresh=od.ol_pitchthreshdown;
			rollthresh=od.ol_rollthreshdown;
		} else {
			pitchthresh=od.ol_pitchthreshup;
			rollthresh=od.ol_rollthreshup;
		}
	}
	if (od.ol_testnewcam) {
		od.ol_camtween+=od.ol_camtweenspeed;
		if (od.ol_camtween>1)
			od.ol_camtween=1;
	} else {
		od.ol_camtween-=od.ol_camtweenspeed;
		if (od.ol_camtween<0)
			od.ol_camtween=0;
	}
//	od.ol_camtween=1;
	quatinterp(&qcamr1,&qcamr2,od.ol_camtween,&od.camnullrot);
//	logger("tween %f\n",od.ol_camtween);
	if (od.startcamrotx) {
		qcamr1.x=1;
		qcamr1.y=0;
		qcamr1.z=0;
		qcamr1.w=od.startcamrotx;
		rotaxis2quat(&qcamr1,&qcamr1);
//		logger("camrot %f\n",od.startcamrotx);
		quattimes(&od.camnullrot,&qcamr1,&od.ol_camnull->rot);
//		logger("cam quat %f %f %f %f\n",
//			od.ol_camnull->rot.x,
//			od.ol_camnull->rot.y,
//			od.ol_camnull->rot.z,
//			od.ol_camnull->rot.w
//			);
/*		if (of->clocktickcount>=INITCANTSTARTDRIVE) {
			od.startcamrotx+=.01f;
			if (od.startcamrotx>0)
				od.startcamrotx=0;
		} */
	} else
		od.ol_camnull->rot=od.camnullrot;
	od.ol_camnull->zoom=od.ol_thecamzoom;
}

// set camrot from qcamdesired (smooth rot)
static void ol_driftcamrot(float cdr)
{
	quatinterp(&od.camnullrot,&od.ol_qcamdesired,cdr,&od.camnullrot);
}

static void ol_cameracollide()
{
	pointf3 camdel,camdel2;
	if (of->ol_intunnels)
		return;
	camdel.x=od.ol_camnull->trans.x;
	camdel.y=od.ol_camnull->trans.y-od.ol_camcoldist;
	camdel.z=od.ol_camnull->trans.z;
// for now, the camera stays above the ground
	if (st2_line2road(&od.ol_camnull->trans,&camdel,&camdel2,NULL))
		od.ol_camnull->trans.y=camdel2.y+od.ol_camcoldist;
}

// spread
static bool ol_burstfinishproc2(tree2 *t)
{
//	int fs;
	t->userint[1]+=od.ol_uloop;	// inc living time
	t->transvel.y-=.001f; // gravity
	t->treecolor.w -= .003f; // fade
	if (t->userint[1]>t->userint[2]) { // die if living time > die time
		return false; // freetree(t); // false means delete me
	}
	return true;
}

// launch
static bool ol_burstfinishproc(tree2 *t)
{
	int fs,i;
	tree2 *u;
	t->userint[1]+=od.ol_uloop;	// inc living time
	t->transvel.y-=.001f; // gravity
//	t->dissolve -= .03; // fade
	if (t->userint[1]>t->userint[2]) { // die if living time > die time
//		logger("finish bursts 16\n");
		fs=ol_finishburstsounds[mt_random(16)];
		od.intimewarp=0;
		if (od.fplace)
			ol_playatagsound(fs,1.0f,0);
		od.intimewarp=1;
		for (i=0;i<100;i++) {
			float lo,la;
			if (fs==BSND4) {
//				logger("mt_random 4 colors for finish fireworks\n");
				u=duptree(od.ol_fwork[mt_random(4)]);
			} else
				u=duptree(od.ol_fwork[t->userint[3]]);
//			logger("lo mt_frand and la mt_frand for sphere\n");
			lo=mt_frand()*2*PI;
			la=mt_frand()*1.9999f-.99995f;
			u->transvel.y=.05f*la;
			la=(float)sqrt(1-la*la);
			u->transvel.x=.05f*la*(float)cos(lo);
			u->transvel.z=.05f*la*(float)sin(lo);
			u->transvel.x+=t->transvel.x;
			u->transvel.y+=t->transvel.y;
			u->transvel.z+=t->transvel.z;
			u->trans=t->trans;
//			u->treealphacutoff=10;
//			u->proc=ol_burstfinishproc2; // NYI
			u->treecolor.w=.5f;
//			logger("time2live mt_random 20\n");
			u->userint[2]=20+mt_random(40); // time to live
			u->flags|=TF_ALWAYSFACING;
			u->scale=pointf3x(.02f,.02f,.02f);
//			linkchildtoparent(u,od.ol_finishpiecenull);
			freetree(u);
		}
		freetree(t);
	}
}

static void ol_dofinishbursts()
{
	tree2 *t;
	int i,r;
	int rd;
	op->ccc2++;
	if (op->ccc2<op->ddd2)
		return;
//	logger("dofinishbursts mt_random 30\n");
	op->ddd2=38+mt_random(30);
	op->ccc2=0;
//	logger("mt_random 3 number of launches\n");
	rd=2+mt_random(3); // number of launches
	for (i=0;i<rd;i++) {
//		logger("more number of launches mt_random 4\n");
		r=mt_random(4);
		t=duptree(od.ol_fwork[r]);
		t->userint[3]=r; // keep track of color
//		t->proc=ol_burstfinishproc; // NYI
//		logger("three mt_frand for sphere again\n");
		t->transvel.x=mt_frand()*.02f-.01f;
		t->transvel.y=.005f+mt_frand()*.02f;
		t->transvel.z=mt_frand()*.02f-.01f;
		t->treecolor.w=240;
		t->trans.x=1.4f;
		if (od.ol_tx)
			t->trans.x=-1.4f;
		od.ol_tx^=1;
		t->trans.y=2.1f;
//		t->treealphacutoff=10;
//		logger("more time 2 live mt_random 50\n");
		t->userint[2]=20+mt_random(32); // time to live
		t->scale=pointf3x(.03f,.03f,.03f);
		t->flags|=TF_ALWAYSFACING;
//		linkchildtoparent(t,od.ol_finishpiecenull);
		freetree(t);
	}
}


void ol_initfinishbursts()
{
	pushandsetdirdown("st2_fireworks");
	od.ol_fwork[0]=alloctree(0,"ember1.lwo");
	od.ol_fwork[1]=alloctree(0,"ember2.lwo");
	od.ol_fwork[2]=alloctree(0,"ember3.lwo");
//	od.ol_fwork[3]=alloctree(0,"ember4.lwo
	od.ol_fwork[3]=buildasphere("sparky","tex");
	od.ol_fwork[3]->buildo2p=O2P_FROMTRANSROTSCALE;
	od.ol_fwork[3]->rotvel=pointf3x(.25f,.35f,.45f);
	popdir();
	od.ol_finishpiecenull=alloctree(4000,NULL);
	linkchildtoparent(od.ol_finishpiecenull,od.ol_finishpiece);
}

static void ol_freefinishbursts()
{
	int i;
	for (i=0;i<4;i++)
		freetree(od.ol_fwork[i]);
}
////////// end finishbursts

// set campos from camdesired (smooth move)
static void ol_driftcam(float cd)
{
	pointf3 camdelta;
	camdelta.x=od.ol_camdesired.x-od.ol_camnull->trans.x;
	camdelta.y=od.ol_camdesired.y-od.ol_camnull->trans.y;
	camdelta.z=od.ol_camdesired.z-od.ol_camnull->trans.z;
//	dofinishbursts();
//	if (!op->dofinish) {
		od.ol_camnull->trans.x+=cd*camdelta.x;
		od.ol_camnull->trans.y+=cd*camdelta.y;
		od.ol_camnull->trans.z+=cd*camdelta.z;
//	} else {
//	} else if (op->ol_clocktickcount>0) {
//		od.ol_camnull->trans.y+=.0020f;
//		od.ol_camnull->trans.y+=od.ol_uloop*.010f;
//		if (op->ol_dofinish<250)
//		if (dofinish<250 && utotalstunts>=50) // maybe we only want fworks when stunts>=50
//	}
}

static void normal2quat(pointf3* up,pointf3* norm,pointf3* quat)
{
	float d;
	d=dot3d(up,norm);
	cross3d(up,norm,quat);
	if (normalize3d(quat,quat)<.001f) {
		quat->x=quat->y=quat->z=0;
		quat->w=1;
	} else {
		quat->w=racos(d);
		rotaxis2quat(quat,quat);
	}
}

static void ol_drawobjects()
{
	int x,z;
	int i;
	pointf3 q,q2,q3;
	pointf3 checkshadroll;
	pointf3 localroll180;
	pointf3 tt,tb,ti,n={0,1,0};
//	float d;
// fry cars, (turn them black if necc..)
	if (op->vinthermo && !od.cartexstate[op->carid] && od.cartex[op->carid]) {
		struct bitmap32 *b;
		b=locktexture(od.cartex[op->carid]);
		if (od.cardarktexsave[op->carid])
			clipblit32(od.cardarktexsave[op->carid],b,0,0,0,0,b->size.x,b->size.y);
		else
			cliprect32(b,0,0,b->size.x-1,b->size.y-1,1);
		unlocktexture(od.cartex[op->carid]);
		od.cartexstate[op->carid]=1;
	}
	if (!op->vinthermo && od.cartexstate[op->carid] && od.cartex[op->carid]) {
		struct bitmap32 *b;
		b=locktexture(od.cartex[op->carid]);
		clipblit32(od.cartexsave[op->carid],b,0,0,0,0,b->size.x,b->size.y);
		unlocktexture(od.cartex[op->carid]);
		od.cartexstate[op->carid]=0;
	}
	if (op->vinthermo && !od.cartexstate2[op->carid] && od.cartex2[op->carid]) {
		struct bitmap32 *b;
		b=locktexture(od.cartex2[op->carid]);
		if (od.cardarktexsave2[op->carid])
			clipblit32(od.cardarktexsave2[op->carid],b,0,0,0,0,b->size.x,b->size.y);
		else
			cliprect32(b,0,0,b->size.x-1,b->size.y-1,1);
		unlocktexture(od.cartex2[op->carid]);
		od.cartexstate2[op->carid]=1;
	}
	if (!op->vinthermo && od.cartexstate2[op->carid] && od.cartex2[op->carid]) {
		struct bitmap32 *b;
		b=locktexture(od.cartex2[op->carid]);
		clipblit32(od.cartexsave2[op->carid],b,0,0,0,0,b->size.x,b->size.y);
		unlocktexture(od.cartex2[op->carid]);
		od.cartexstate2[op->carid]=0;
	}

////////// copy object to tree
// do car
//		op->ol_uplayflame1->trans=od.ol_uflame1pos;
//		op->ol_uplayflame2->trans=od.ol_uflame2pos;
//		op->ol_uplayflame1->scale=od.ol_uflame1scale;
//		op->ol_uplayflame2->scale=od.ol_uflame2scale;
	if (od.klondykeshowbox) {
		getklondyke1();
		op->klondykebox0->trans=od.klondyke0boxmin;
		op->klondykebox0->scale.x=od.klondyke0boxmax.x-od.klondyke0boxmin.x;
		op->klondykebox0->scale.y=od.klondyke0boxmax.y-od.klondyke0boxmin.y;
		op->klondykebox0->scale.z=od.klondyke0boxmax.z-od.klondyke0boxmin.z;
		op->klondykebox1->trans=od.klondyke1boxmin;
		op->klondykebox1->scale.x=od.klondyke1boxmax.x-od.klondyke1boxmin.x;
		op->klondykebox1->scale.y=od.klondyke1boxmax.y-od.klondyke1boxmin.y;
		op->klondykebox1->scale.z=od.klondyke1boxmax.z-od.klondyke1boxmin.z;
	}
	if (od.forkshowbox) {
		op->forkbox0->trans=od.fork0boxmin;
		op->forkbox0->scale.x=od.fork0boxmax.x-od.fork0boxmin.x;
		op->forkbox0->scale.y=od.fork0boxmax.y-od.fork0boxmin.y;
		op->forkbox0->scale.z=od.fork0boxmax.z-od.fork0boxmin.z;
	}
//	if (od.bumpershowbox) {
//		getklondyke1();
//		op->bumperbox0->trans=od.bumper0boxmin;
//		op->bumperbox0->scale.x=od.bumper0boxmax.x-od.bumper0boxmin.x;
//		op->bumperbox0->scale.y=od.bumper0boxmax.y-od.bumper0boxmin.y;
//		op->bumperbox0->scale.z=od.bumper0boxmax.z-od.bumper0boxmin.z;
//		op->bumperbox1->trans=od.bumper1boxmin;
//		op->bumperbox1->scale.x=od.bumper1boxmax.x-od.bumper1boxmin.x;
//		op->bumperbox1->scale.y=od.bumper1boxmax.y-od.bumper1boxmin.y;
//		op->bumperbox1->scale.z=od.bumper1boxmax.z-od.bumper1boxmin.z;
//	}
//	if (od.buzzshowbox) {
//		getklondyke1();

//		op->buzzbox0->trans=od.buzz0boxmin;
//		op->buzzbox0->scale.x=od.buzz0boxmax.x-od.buzz0boxmin.x;
//		op->buzzbox0->scale.y=od.buzz0boxmax.y-od.buzz0boxmin.y;
//		op->buzzbox0->scale.z=od.buzz0boxmax.z-od.buzz0boxmin.z;
//	}
	op->ol_carbody->trans.x=op->ol_caroffsetx;
	op->ol_carbody->trans.y=op->ol_caroffsety;
	op->ol_carbody->trans.z=op->ol_caroffsetz;
	op->ol_carnull->rot=op->carang;
	op->ol_carnull->trans=op->pos;
	op->ol_carcenternull->scale.x=od.ol_uplaycarscale;
	op->ol_carcenternull->scale.y=od.ol_uplaycarscale;
	op->ol_carcenternull->scale.z=od.ol_uplaycarscale;
	quat2rpy(&op->ol_carnull->rot,&checkshadroll); //it's in radians
// do car bones
/*	for (i=0;i<COLBONESX;i++)
		for (j=0;j<COLBONESZ;j++)
			op->db_t[i][j]->transvel=op->db_curpushin[i][j]; */
// do shadow
	op->ol_shadowbody->rot=op->carang;
	if (fabs(checkshadroll.z)>90*PIOVER180) {
		localroll180.x=0;
		localroll180.y=0;
		localroll180.z=1;
		localroll180.w=0;
		quattimes(&op->ol_shadowbody->rot,&localroll180,&op->ol_shadowbody->rot);
	}
	op->ol_shadownull->trans=op->pos;
	float bigtireshadowupscale;
	if (op->xcurweapstate==WEAPSTATE_ACTIVE && op->xcurweapkind==WEAP_BIGTIRES)
		bigtireshadowupscale=1.43f;
	else
		bigtireshadowupscale=1;
	op->ol_shadowb->scale.x=op->ol_uplayshadowscale*bigtireshadowupscale;
	op->ol_shadowb->scale.y=op->ol_uplayshadowscale*.001f*bigtireshadowupscale;
	op->ol_shadowb->scale.z=op->ol_uplayshadowscale*bigtireshadowupscale;
	tb=op->pos;
	tt=op->pos;
	tt.y+=.15f;
	tb.y-=100;
	st2_line2road(&tt,&tb,&ti,&n);
	if (op->pos.y-ti.y>1.4f)
		op->ol_shadownull->trans.y=ti.y+(od.ol_shadyoffset2=.16f);
	else
		op->ol_shadownull->trans.y=ti.y+(od.ol_shadyoffset=.02f);
	ti.x=0;
	ti.y=1;
	ti.z=0;
	normal2quat(&ti,&n,&op->ol_shadownull->rot);
//	d=dot3d(&ti,&n);
//	cross3d(&ti,&n,&ti);
//	ti.w=racos(d);
//	rotaxis2quat(&ti,&op->ol_shadownull->rot);
//
	ol_getpiece(&op->pos,&x,&z);
//	if (x>=0 && x<=6 && z>=0 && z<=6)
//		ol_setnextcrashloc();
// update motor sound
	op->ol_uplayrevfreq=(int)(fabs(op->wheelvel[BACKLEFT])*op->ol_revfreqmul+op->ol_revfreqadd);
	op->ol_uplayrevfreq=range(100,op->ol_uplayrevfreq,100000);
/////// update wheel trees
	for (i=0;i<6;i++) {
		if (!op->ol_wheels2[i])
			continue;
		pointf3 qfix=pointf3x(0,0,0,1); // fix the left wheels for newer 'null' wheels
		if (!op->ol_wheels2[i]->mod) { // if it's a newer 'null' wheel complete with rims..
			if (i==FRONTLEFT || i==BACKLEFT || i==MIDDLELEFT) { // these need to be yawed 180 for the newer 'null' wheels
				qfix=pointf3x(0,1,0,0); // 180 yaw
			}
		}
//	for (i=0;i<max(4,ol_carinfo[op->cartype].nwheels);i++) {
		if (!op->onlinecar) { // wheels bounce, (maybe turn on)
			op->ol_wheels[i]->trans.y=-(op->shocklen[i]-op->ol_wheelrad-op->ol_shockoffsety);
			if (op->ol_wheels[i]->trans.y>op->ol_shocklimit)
				op->ol_wheels[i]->trans.y=op->ol_shocklimit;
		}
		if (ol_steershocks[i]) { // front
			q.x=0;
			q.y=1;
			q.z=0;
			q.w=op->wheelyaw*PIOVER180;
/*			if (!op->ol_wheels2[i]->mod) { // if it's a newer 'null' wheel complete with rims..
				if (i==FRONTLEFT || i==BACKLEFT) {
					q.w+=PI; // then left wheels need to be yawed 180
				}
			} */
			rotaxis2quat(&q,&q);
		} else { // back
			q.x=0; // ident
			q.y=0;
			q.z=0;
			q.w=1;
/*			if (!op->ol_wheels2[i]->mod) { // if it's a newer 'null' wheel complete with rims..
				if (i==FRONTLEFT || i==BACKLEFT) {
//					q.x=0; // yaw 180 from ident
					q.y=1;
//					q.z=0;
					q.w=0;
				}
			} */
		}
//		if (op->isnewcar) // && (i==1 || i==3))
		S32 j=i;
		if (i>=4)
			j=3;
			op->wheelang[i] += op->wheelvel[j]*od.ol_timeincconst*.85f; // fudge, maybe bad practice
			if (op->wheelang[i]>4*PI)
				op->wheelang[i]-=4*PI;
			if (op->wheelang[i]<0)
				op->wheelang[i]+=4*PI;
//		else
//			op->wheelang[i]=normalangrad(op->wheelang[i]-op->wheelvel[i]*od.ol_timeincconst);
		q2.x=1;
		q2.y=0;
		q2.z=0;
		q2.w=op->wheelang[i];//*PIOVER180;
		rotaxis2quat(&q2,&q2);
		quatinverse(&q2,&q3);
// older wheels
		quattimes(&q,&q2,&q2);
		quatnormalize(&q2,&q2);
// newer wheels
		quattimes(&q,&q3,&q3);
		quatnormalize(&q3,&q3);
		if (op->ol_wheels[i]) {
			op->ol_wheels[i]->rot=q2;
			if (op->extrabigtires)
				op->ol_wheels[i]->flags|=TF_DONTDRAW;
			else
				op->ol_wheels[i]->flags&=~TF_DONTDRAW;
		}
		if (op->ol_wheels2[i]) {
			quattimes(&q3,&qfix,&q3);
			op->ol_wheels2[i]->rot=q3;
			if (op->extrabigtires)
				op->ol_wheels2[i]->flags|=TF_DONTDRAW;
			else
				op->ol_wheels2[i]->flags&=~TF_DONTDRAW;
		}
	}
	if (!op->onlinecar) {
		op->ol_wheels[FRONTLEFT]->trans.x=-op->ol_shockspacingx/2+op->ol_wheelwid/2;
		op->ol_wheels[FRONTLEFT]->trans.z=op->ol_shockspacingz/2-op->ol_wheellen/2;
		op->ol_wheels[FRONTRIGHT]->trans.x=op->ol_shockspacingx/2-op->ol_wheelwid/2;
		op->ol_wheels[FRONTRIGHT]->trans.z=op->ol_shockspacingz/2-op->ol_wheellen/2;
		op->ol_wheels[BACKLEFT]->trans.x=-op->ol_shockspacingx/2+op->ol_wheelwid/2;
		op->ol_wheels[BACKLEFT]->trans.z=-op->ol_shockspacingz/2+op->ol_wheellen/2;
		op->ol_wheels[BACKRIGHT]->trans.x=op->ol_shockspacingx/2-op->ol_wheelwid/2;
		op->ol_wheels[BACKRIGHT]->trans.z=-op->ol_shockspacingz/2+op->ol_wheellen/2;
		for (i=0;i<4;i++) {
	//	for (i=0;i<max(4,ol_carinfo[op->cartype].nwheels);i++) {
			op->ol_wheels[i]->scale.x=op->ol_uplaywheelscale;
			op->ol_wheels[i]->scale.y=op->ol_uplaywheelscale;
			op->ol_wheels[i]->scale.z=op->ol_uplaywheelscale;
		}
	}
// handle special effects (lightning around car)
#if 0
	if (op->ol_nulightnings && !od.ol_slowpo) {
		struct bitmap32 *b16;
		op->ol_uplayspecial1->flags&=~TF_DONTDRAW;
		b16=locktexture(op->lightningtex);
		clipblit32(od.ol_specialtsp->frames[op->ol_lightningframe],b16,0,0,0,0,od.ol_specialtsp->x,od.ol_specialtsp->y);
		unlocktexture(op->lightningtex);
		op->ol_lightningframe++;
		if (op->ol_lightningframe>=od.ol_specialtsp->nframe) {
			op->ol_lightningframe=0;
			op->ol_nulightnings-=od.ol_uloop;
			if (op->ol_nulightnings<=0)
				op->ol_nulightnings=0;
		}
/*		tspanimtex(op->ol_specialtsp,0,0,0);
		if(op->ol_specialtsp->curframe==0) {
			op->ol_nulightnings-=od.ol_uloop;
			if (op->ol_nulightnings<0)
				op->ol_nulightnings=0;
			if (op->ol_nulightnings==0) {
				op->ol_uplayspecial1->flags|=TF_DONTDRAW;
			}
		} */
	} else
		op->ol_uplayspecial1->flags|=TF_DONTDRAW;
// car flashes (blue car)
	if (op->ol_nuflashes) {
		struct bitmap32 *b16;
		b16=locktexture(op->flashtex);
		clipblit32(op->ol_flashtsp->frames[op->ol_flashtsp->nframe-op->ol_flashframe-1],b16,0,0,0,0,op->ol_flashtsp->x,op->ol_flashtsp->y);
		unlocktexture(op->flashtex);
		op->ol_flashframe++;
		if (op->ol_flashframe>=op->ol_flashtsp->nframe) {
			int ul;
			ul=max(1,od.ol_uloop);
			op->ol_flashframe=0;
			op->ol_nuflashes-=ul;
			if (op->ol_nuflashes<=0)
				op->ol_nuflashes=0;
		}
	}

/*		tspanimtex(op->ol_flashtsp,0,0,0);
		if (op->ol_flashtsp->curframe==0) {
	} */
// golden condom
	if (op->ol_nucondoms) {
		struct bitmap32 *b16;
		op->ol_uplayspecial2->flags&=~TF_DONTDRAW;
		b16=locktexture(op->condomtex);
		clipblit32(od.ol_condomtsp->frames[op->ol_condomframe],b16,0,0,0,0,od.ol_condomtsp->x,od.ol_condomtsp->y);
		unlocktexture(op->condomtex);
		op->ol_condomframe++;
		if (op->ol_condomframe>=od.ol_condomtsp->nframe) {
			op->ol_condomframe=0;
			op->ol_nucondoms-=od.ol_uloop;
			if (op->ol_nucondoms<=0)
				op->ol_nucondoms=0;
//				op->ol_uplayspecial2->flags|=TF_DONTDRAW;
		}
	} else
		op->ol_uplayspecial2->flags|=TF_DONTDRAW;
/*		tspanimtex(op->ol_condomtsp,0,0,0);
		if (op->ol_condomtsp->curframe==0) {
			op->ol_nucondoms-=od.ol_uloop;
			if (op->ol_nucondoms<0)
				op->ol_nucondoms=0;
			if (op->ol_nucondoms==0)
		} */
// flames
	if (op->ol_nuflames) {
		struct bitmap32 *b16;
		op->ol_uplayflame1->flags&=~TF_DONTDRAW;
		op->ol_uplayflame2->flags&=~TF_DONTDRAW;
		b16=locktexture(op->flametex);
		clipblit32(od.ol_flametsp->frames[op->ol_flameframe],b16,0,0,0,0,od.ol_flametsp->x,od.ol_flametsp->y);
		unlocktexture(op->flametex);
//		tspanimtex(od.ol_flametsp,0,0,0);
		op->ol_flameframe++;
		if (op->ol_flameframe==od.ol_flametsp->nframe) {
			op->ol_flameframe=0;
			op->ol_nuflames-=2;
			if (op->ol_nuflames<=0)
				op->ol_nuflames=0;
		}
	}
	if (op->ol_flymode || op->ol_nuflames==0) {
		op->ol_uplayflame1->flags|=TF_DONTDRAW;
		op->ol_uplayflame2->flags|=TF_DONTDRAW;
	}
#endif
// scoreline
//	od.ol_scrline->scale.x=od.ol_scrlinescl.x*.001f; // scale down the enormously huge scoreline
//	od.ol_scrline->scale.y=od.ol_scrlinescl.y*.001f;
//	od.ol_scrline->scale.z=od.ol_scrlinescl.z*.001f;
//	od.ol_scrline->trans.x=od.ol_scrlinepos.x*.001f;
//	od.ol_scrline->trans.y=od.ol_scrlinepos.y*.001f;
//	od.ol_scrline->trans.z=od.ol_scrlinepos.z*.001f;
////////////// lastly, work the camera .................
// try yeta nother camera
	if (of==op) {
		int ul=max(1,od.ol_uloop);
		for (i=0;i<ul;i++) {
//			if (0) {
			if (of->ol_flymode==1 && of->ol_startstunt==2 && of->ol_airtime>od.camlandtime ) {
				ol_pickaircamrot();
				ol_driftcamrot(od.ol_camrotdrift);
				ol_pickaircampos();
				ol_driftcam(od.ol_camdrift);
			} else {
				ol_pickgroundcamrot();
				ol_driftcamrot(od.ol_camrotdrift);
				ol_pickgroundcampos();
				ol_driftcam(od.ol_camdrift);
			}
		}
		if (op->dofinish && op==of)
			ol_dofinishbursts();
		if (od.ol_manualcar)
			od.ol_camnull->trans=of->ol_landspot;
		ol_cameracollide();
		ol_targetcamrot();
		new_cam_proc(op);
	}


//	addnull(&op->ol_carnull->trans,NULL);
//	obj2world(op->ol_carbody,&zerov,&q);
//	addnull(&q,NULL);
/*	if (op->regpoints) {
//		op->regpoints->rot=od.regrot;
//		op->regpoints->rot.x*=PIOVER180;
//		op->regpoints->rot.y*=PIOVER180;
//		op->regpoints->rot.z*=PIOVER180;
		if (op->regpoints->nchildren==NREGPOINTS) {
			if (od.regnum<0 || od.regnum>=NREGPOINTS)
				for (i=0;i<NREGPOINTS;i++)
					op->regpoints->children[i]->flags&=~TF_DONTDRAW;
			else
				for (i=0;i<NREGPOINTS;i++)
					if (od.regnum==i)
						op->regpoints->children[i]->flags&=~TF_DONTDRAW;
					else
						op->regpoints->children[i]->flags|=TF_DONTDRAW;
		}
	} */
/*	if (op->onlinecar) {
		for (i=0;i<REGPOINT_NREGPOINTS;i++)
			if (od.regnum==-1 || od.regnum==i) {
				pointf3 v;
				obj2world(op->ol_carnull,&op->regpointsoffset[i],&v);
//				obj2world(op->theregpoints,&op->regpointsoffset[i],&v);
//				obj2world(op->regpoints,&zerov,&v);
//				obj2world(op->ol_carnull,,&v);
//				addnull(&v,NULL);
			}
	} */
/*	if (0) {
//	if (op->xcurweapkind==WEAP_HOLO && op->xcurweapstate==WEAPSTATE_ACTIVE) {
		if (!op->hologram) {
			op->hologram=duptree(op->ol_carnull);
			linkchildtoparent(op->hologram,op->ol_carnull);
			setVEC(&op->hologram->trans,0,0,od.holofront);
			op->hologram->rot=zerov;
			op->hologram->rot.w=1;
		}
	} else {
		if (op->hologram) {
			freetree(op->hologram);
			op->hologram=NULL;
		}
	} */
	if (op->xcurweapkind==WEAP_PRISM && op->xcurweapstate==WEAPSTATE_ACTIVE) {
		if (!op->prism) {
			op->prism=duptree(od.prism);
			linkchildtoparent(op->prism,op->ol_carnull);
			op->prism->trans=pointf3x(0,od.prismup,0);
			op->prism->rot=zerov;
			op->prism->rot.w=1;
			op->prism->rotvel.x=2*PI/32;
		}
	} else {
		if (op->prism) {
			freetree(op->prism);
			op->prism=NULL;
		}
	}
	if (op->prism) { // play with prism
		op->prism->userint[0]++;
		if (op->prism->userint[0]==65) {
			float f;
			i=mt_random(3);
			op->prism->userint[0]=0;
			f=(mt_frand()-.5f)*.5f;
			switch(i) {
			case 0:
				op->prism->rotvel.x=f;
				break;
			case 1:
				op->prism->rotvel.y=f;
				break;
			case 2:
				op->prism->rotvel.z=f;
				break;
			}
		}
	}
//	if (od.showcrashresetloc && op==of) {
//		addnull(&op->ol_crashresetloc,&op->ol_crashresetrot);
//	}
//	setVEC(&q,1,0,0);
//	addvec(&op->pos,&q,rgbred);
//	setVEC(&q,0,1,0);
//	addvec(&op->pos,&q,rgbgreen);
//	setVEC(&q,0,0,1);
//	addvec(&op->pos,&q,rgbblue);
//	addvec(&op->pos,&op->carvel,rgbblue);
}

static void ol_clearrollpie()
{
	int i;
	for (i=0;i<4;i++)
		op->ol_rollpie[i]=0;
	op->ol_rollpiesum=0;
}

static void ol_clearpitchpie()
{
	int i;
	for (i=0;i<4;i++)
		op->ol_pitchpie[i]=0;
	op->ol_pitchpiesum=0;
}

static void ol_clearyawpie()
{
	int i;
	for (i=0;i<4;i++)
		op->ol_yawpie[i]=0;
	op->ol_yawpiesum=0;
}

static void ol_clearpies()
{
	ol_clearyawpie();
	ol_clearpitchpie();
	ol_clearrollpie();
	op->ol_cantaddroll=op->ol_cantaddpitch=op->ol_cantaddyaw=-1;
	op->ol_udidroll=op->ol_udidyaw=op->ol_udidpitch=0;
}

static int ol_addrollpie(float ang)
{
	int i=(int)ang/90;
	i=range(0,i,3);
	i&=3;
	if (!op->ol_rollpie[i] && op->ol_cantaddroll!=i) {
		op->ol_rollpie[i]=1;
		op->ol_rollpiesum++;
		op->ol_cantaddroll=-1;
		if (op->ol_rollpiesum==4) {
			ol_clearrollpie();
			op->ol_cantaddroll=i;
			return 1;
		}
	}
	return 0;
}

static int ol_addpitchpie(float ang)
{
	int i=(int)ang/90;
	i=range(0,i,3);
	i&=3;
	if (!op->ol_pitchpie[i] && op->ol_cantaddpitch!=i) {
		op->ol_pitchpie[i]=1;
		op->ol_pitchpiesum++;
		op->ol_cantaddpitch=-1;
		if (op->ol_pitchpiesum==4) {
			ol_clearpitchpie();
			op->ol_cantaddpitch=i;
			return 1;
		}
	}
	return 0;
}

static int ol_addyawpie(float ang)
{
	int i=(int)ang/90;
	i=range(0,i,3);
	i&=3;
	if (!op->ol_yawpie[i] && op->ol_cantaddyaw!=i) {
		op->ol_yawpie[i]=1;
		op->ol_yawpiesum++;
		op->ol_cantaddyaw=-1;
		if (op->ol_yawpiesum==4) {
			ol_clearyawpie();
			op->ol_cantaddyaw=i;
			return 1;
		}
	}
	return 0;
}

struct weapfly *allocprojectile()
{
	int i;
	for (i=0;i<MAXWEAPFLY;i++)
		if (!od.weapflys[i].active) {
			memset(&od.weapflys[i],0,sizeof(od.weapflys[0]));
			od.weapflys[i].active=1;
			od.weapflys[i].dissolve=1;
			return &od.weapflys[i];
		}
	return NULL;
}

void freeprojectile(struct weapfly *t)
{
	t->active=0;
}

void freetreepool()
{
	int i,j;
	logger("in freetreepool -------\n");
	for (i=0;i<MAXDETACHKIND;i++) {
		logger("%3d pooled trees for '%s'\n",detachweapinfos[i].ntreepool,detachweapinfos[i].name);
		for (j=0;j<detachweapinfos[i].ntreepool;j++) {
			if (detachweapinfos[i].treepool[j]->parent)
				unhooktree(detachweapinfos[i].treepool[j]);
			freetree(detachweapinfos[i].treepool[j]);
		}
		detachweapinfos[i].ntreepool=0;
//		if (detachweapinfos[i].treepool)
//			delete[] detachweapinfos[i].treepool;
		detachweapinfos[i].treepool.clear();
//		detachweapinfos[i].treepool=NULL;
	}
}

static void shieldhit(int weapkind,int car,float soundvol)
{
	struct ol_playerdata *p;
	int c;
	ol_playatagsound(33,soundvol,0);
	p=&opa[car];
	if (p->xcurweapstate!=WEAPSTATE_ACTIVE)
		return;	// shields are already going down
	c=weapinfos[p->xcurweapkind].shield;
	if (p->tweapbooston)
//	if (p->tshieldbooston)
		c<<=1;
	p->curweapvar++;
	if (p->xcurweapkind==WEAP_PRISM) {
		p->tenergies[ENERGY_ELECTRICAL]++;
		p->tenergies[ENERGY_FUSION]++;
	}
	logger("car %d, clk %d: shield '%s' counters '%s' (%d/%d)\n",
			car,opa[0].clocktickcount,
			weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name,
			p->curweapvar,c);
	if (p->curweapvar>=c)
		deactivateweap(p);

//	switch(p->xcurweapkind) {
/*
	case WEAP_DIELECTRIC:
		logger("car %d, clk %d: shield '%s' counters '%s'\n",car,opa[0].clocktickcount,weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name);
		p->curweapvar++;
		if (p->curweapvar>=2)
			deactivateweap(p);

		break;
	case WEAP_GEOSHIELD:
		logger("car %d, clk %d: shield '%s' counters '%s'\n",car,opa[0].clocktickcount,weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name);
//		if (!p->curweapvar)
//			p->curweapvar=1;
//		else {
//			deactivateweap(p);
		}
		break;
	case WEAP_PLASMA:
		logger("car %d, clk %d: shield '%s' counters '%s'\n",car,opa[0].clocktickcount,weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name);
		break;
	case WEAP_TRANSDUCER:
		logger("car %d, clk %d: shield '%s' counters '%s'\n",car,opa[0].clocktickcount,weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name);
		deactivateweap(p);
		break;
	case WEAP_HOLO:
		logger("car %d, clk %d: shield '%s' counters '%s'\n",car,opa[0].clocktickcount,weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name);
		deactivateweap(p);
		break;
	case WEAP_PRISM:
		logger("car %d, clk %d: shield '%s' counters '%s'\n",car,opa[0].clocktickcount,weapinfos[p->xcurweapkind].name,weapinfos[weapkind].name);
		deactivateweap(p);
		break;
	}
*/
}

static void fakepos(int id,pointf3* pos)
{
	if (0) {
//	if (opa[id].xcurweapkind==WEAP_HOLO && opa[id].xcurweapstate==WEAPSTATE_ACTIVE) {
		pos->x=pos->y=0;
		pos->z=od.holofront;
		quatrot(&opa[id].carang,pos,pos);
		pos->x+=opa[id].pos.x;
		pos->y+=opa[id].pos.y;
		pos->z+=opa[id].pos.z;
	} else if (opa[id].xcurweapkind==WEAP_PRISM && opa[id].xcurweapstate==WEAPSTATE_ACTIVE) {
		pos->x=pos->z=0;
		pos->y=od.prismup;
		quatrot(&opa[id].carang,pos,pos);
		pos->x+=opa[id].pos.x;
		pos->y+=opa[id].pos.y;
		pos->z+=opa[id].pos.z;
	} else
		*pos=opa[id].pos;
}

// inlead is not timewarp safe, ok for humans, not for newbots
static void ol_drivecar(int inlead)
{
//	inlead=0; // disable rubber banding
//	int lastinput;
	int sf; // start fuel
	int ct;
	float pitch,yaw,roll;	// for icrms
	pointf3 top,bot;
//	float f;
	tree2 *wt=0;
	int wk; // randomizer
	int i=0,j=0,nw;//,weapkind,nw;
	int lup,ldown,sp,lweapkey;
	struct soundhandle *sh;
	int doatricksound1=0,doatricksound2=0,doatricksound3=0;
// get controls into variables
//	logger("in drivecar with clock %d\n",op->ol_clocktickcount);

	lup=op->pi.ol_uup;
	ldown=op->pi.ol_udown;
	lweapkey=op->pi.weapkey;
	memset(&op->pi,0,sizeof(op->pi));
	op->espace=0;
// get ai input settings
	nw=carstubinfos[op->carid].cd.c_nweap;
	if (currule->rboxes) {
		boxai_ai();
		boxai_check();
	} else {
		doai();
		docheckpoints(op);
//		docheckpoints();
	}
	if (op->xcurweapslot==-1 && nw)
		op->xcurweapslot=0;
	ct=op->cntl;
	if (od.playernet[op->carid].onebotplayer)
		ct=od.defaultcntl;
	switch(ct) {
//	switch(op->cntl) {
// human settings, allowing uspace from both ai and human
	case CNTL_HUMAN:
		if (wininfo.releasemode) { // use the mouse only in release mode
//			op->pi.ol_uup   =(wininfo.keystate[K_UP]   || wininfo.keystate[K_NUMUP]   || wininfo.jy<-.5f || (wininfo.jbut&1) /*|| (MBUT&1) || wininfo.dmy <= -8 */);
//			op->pi.ol_udown =(wininfo.keystate[K_DOWN] || wininfo.keystate[K_NUMDOWN] || wininfo.jy> .5f || (wininfo.jbut&2) /*|| (MBUT&2) || wininfo.dmy >=  8 */);
//			op->pi.ol_uleft =(wininfo.keystate[K_LEFT] || wininfo.keystate[K_NUMLEFT] || wininfo.jx<-.5f /* || wininfo.dmx <= -8 */);
//			op->pi.ol_uright=(wininfo.keystate[K_RIGHT]|| wininfo.keystate[K_NUMRIGHT]|| wininfo.jx> .5f /* || wininfo.dmx >=  8 */);
			op->pi.ol_uup   =(wininfo.keystate[K_UP]   || wininfo.keystate[K_NUMUP]    /*|| (MBUT&1) || wininfo.dmy <= -8 */);
			op->pi.ol_udown =(wininfo.keystate[K_DOWN] || wininfo.keystate[K_NUMDOWN]  /*|| (MBUT&2) || wininfo.dmy >=  8 */);
			op->pi.ol_uleft =(wininfo.keystate[K_LEFT] || wininfo.keystate[K_NUMLEFT]  /* || wininfo.dmx <= -8 */);
			op->pi.ol_uright=(wininfo.keystate[K_RIGHT]|| wininfo.keystate[K_NUMRIGHT] /* || wininfo.dmx >=  8 */);
			op->pi.ol_urollleft=
				wininfo.keystate[K_LEFTSHIFT]||
				wininfo.keystate[K_RIGHTSHIFT];
//				wininfo.keystate[K_RIGHTALT]||
			op->pi.ol_urollright=
				wininfo.keystate[K_RIGHTALT]||
				wininfo.keystate[K_LEFTALT];//||
//				wininfo.keystate[K_RIGHTCTRL]||
//				wininfo.keystate[K_LEFTCTRL];
		} else {
//			op->pi.ol_uup   =(wininfo.keystate[K_UP]   ||wininfo.keystate[K_NUMUP]   || wininfo.jy<-.5f || (wininfo.jbut&1));
//			op->pi.ol_udown =(wininfo.keystate[K_DOWN] ||wininfo.keystate[K_NUMDOWN] || wininfo.jy> .5f || (wininfo.jbut&2));
//			op->pi.ol_uleft =(wininfo.keystate[K_LEFT] ||wininfo.keystate[K_NUMLEFT] || wininfo.jx<-.5f);
//			op->pi.ol_uright=(wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT]|| wininfo.jx> .5f);
			op->pi.ol_uup   =(wininfo.keystate[K_UP]   ||wininfo.keystate[K_NUMUP]);
			op->pi.ol_udown =(wininfo.keystate[K_DOWN] ||wininfo.keystate[K_NUMDOWN]);
			op->pi.ol_uleft =(wininfo.keystate[K_LEFT] ||wininfo.keystate[K_NUMLEFT]);
			op->pi.ol_uright=(wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT]);
			op->pi.ol_urollleft=
				wininfo.keystate[K_LEFTSHIFT]||
				wininfo.keystate[K_RIGHTSHIFT];
//				wininfo.keystate[K_RIGHTALT]||
			op->pi.ol_urollright=
				wininfo.keystate[K_RIGHTALT]||
				wininfo.keystate[K_LEFTALT];//||
//				wininfo.keystate[K_RIGHTCTRL]||
//				wininfo.keystate[K_LEFTCTRL];
		}
		if (nw && /*!od.intimewarp && */op->playerid==od.playerid) {
			i=-1;
			if (wininfo.keystate[(U8)'x'] && nw>1) {
//				KEY=0;
				if (op->xcurweapslot==MAXWEAPONSCARRY && op->lastspecialweapslot>=0) {
					i=op->lastspecialweapslot;
				} else {
					i=op->xcurweapslot+1;
//				op->curselweap++;
					if (i>=nw)
						i=0;
				}
//				updateweapdisplay();
			}
			if (wininfo.keystate[(U8)'z'] && nw>1) {
//				KEY=0;
				if (op->xcurweapslot==MAXWEAPONSCARRY && op->lastspecialweapslot>=0) {
					i=op->lastspecialweapslot;
				} else {
					i=op->xcurweapslot-1;
//				op->curselweap--;
					if (i<0)
						i=nw-1;
//				updateweapdisplay();
				}
			}
//			i=-1;
//			if (wininfo.keystate[K_RIGHTCTRL]|wininfo.keystate[K_LEFTCTRL])
			if (wininfo.keystate[(U8)K_RIGHTCTRL]||wininfo.keystate[(U8)K_LEFTCTRL]||wininfo.keystate[(U8)'f']) {
//				KEY=0;
				i=op->xcurweapslot;
			}
			if (i==-1) {
				int nw2; // just 12 function keys
//				if (KEY>=K_F1 && KEY<K_F1+nw) {
//					i=KEY-K_F1;
//					KEY=0;
//				}
				if (nw>12)
					nw2=12;
				else
					nw2=nw;
				for (i=0;i<nw2;i++)
					if (wininfo.keystate[K_F1+i])
						break;
				if (i==nw2)
					i=-1;
			}

			if (i>=0) {
				op->pi.weapkey=i+1;
//				updateweapdisplay();
			} else
				op->pi.weapkey=0;
		}
		sp=wininfo.keystate[(U8)' '];// || (wininfo.jbut&4);
		if (!od.spacebartimer && sp ) {
// 			logger("car reset: space generated from key %d, joy %d\n",wininfo.keystate[' '],wininfo.jbut&4);
 			op->pi.uspace|=sp;
//			od.spacebartimer=120;
		}
		break;
	case CNTL_AI:
		if (op->clocktickcount>200+40*op->carid && nw>0)
			if ((op->clocktickcount&512)==0)
				op->pi.weapkey=1+(((op->clocktickcount+40*op->carid)>>12)+op->carid*3)%nw;
			else
				op->pi.weapkey=0;
			//		else if
//		op->pi.weapkey=1;
		break;
	case CNTL_NONE:
		memset(&op->pi,0,sizeof(op->pi));
		break;
	}
/*	if (op==of && inlogmode) {
		logger("in drivecar1 set %d\n", op->pi.ol_uup+
				(op->pi.ol_udown<<1)+
				(op->pi.ol_uright<<2)+
				(op->pi.ol_uleft<<3)+
				(op->pi.ol_urollleft<<4)+
				(op->pi.ol_urollright<<5)+
				(op->pi.uspace<<6));
	} */
	if (!op->ol_flymode && od.slowleadmask && !currule->rboxes) { // rabbits
//		if (op->cntl==CNTL_HUMAN && inlead==2)
		if (inlead==2)
			inlead=1;
//		if (op->cntl==CNTL_HUMAN || (op->clocktickcount&0x3ff)>0xff) {
		if (true) {
			if (inlead==1) {
				if ((op->clocktickcount&od.slowleadmask) >= od.slowleadpower)
					op->pi.ol_uup=0;
			} else if (inlead==2) {
				if ((op->clocktickcount&od.slowleadmask) >= od.wayleadpower)
					op->pi.ol_uup=0;
			}
		}
		if (op->cntl==CNTL_AI && op->finplace) { // bots don't drive after finishing
			op->pi.ol_uup=0;
			op->stuckwatchdog=0;
		}
	}
	if (!op->ol_flymode && od.boxaislowleadmask && currule->rboxes) {
// try a governor
		if ((op->clocktickcount&od.boxaislowleadmask2) >= od.boxaislowleadpower2) {
			int sn=op->carid; // slotnum
			float r=1.0f-sn*od.boxaislowleadrat/od.numcars;
			if (r<0)
				r=0;
			if (op->ol_accelspin>=op->ol_startaccelspin*r) {
				op->pi.ol_uup=0;
			}
		}
// reduce speed of leader
		if ((op->clocktickcount&od.boxaislowleadmask) >= od.boxaislowleadpower) {
			if (boxai_slowlead())
				op->pi.ol_uup=0;
		}
// of->ol_accelspin/of->ol_startaccelspin
	}
//	if (op->pi.ol_urollright)
//		logger("urollright\n");
//	if (op->pi.ol_urollleft)
//		logger("urollleft\n");
// from the net
/*	if (op==of && inlogmode) {
		logger("in drivecar2 set %d\n", op->pi.ol_uup+
				(op->pi.ol_udown<<1)+
				(op->pi.ol_uright<<2)+
				(op->pi.ol_uleft<<3)+
				(op->pi.ol_urollleft<<4)+
				(op->pi.ol_urollright<<5)+
				(op->pi.uspace<<6));
	} */
//	if (op->carid==od.playerid) {
	if (op->playerid==od.playerid /* || od.playernet[op->carid].onebotplayer */) {
		int si,inp;
		si=someinput(op,0);
/*		if (op==of && inlogmode) {
			logger("in someinput %d\n",si);
		} */
		if (si<op->clocktickcount) {
			inp=
				 op->pi.ol_uup+
				(op->pi.ol_udown<<1)+
				(op->pi.ol_uright<<2)+
				(op->pi.ol_uleft<<3)+
				(op->pi.ol_urollleft<<4)+
				(op->pi.ol_urollright<<5)+
				(op->pi.uspace<<6);
//			packetaddinput(op->carid,inp,op->pi.weapkey); //,op->clocktickcount);
			packetaddinput(op->playerid,inp,op->pi.weapkey); //,op->clocktickcount);
//			packetaddinput(od.playerid,inp,op->pi.weapkey); //,op->clocktickcount);
			sendinput(op);	// from pi variables (player input)
		}
//	} else {
	}
	getinput(op);	// to pi variables
	if (op->espace)
		op->pi.uspace=1;
/*	if (!od.predicted)
		logger("   car %d, clock %d, up %d, dn %d, lf %d, rt %d, game_rl %d, rr %d sp %d, esp %d, weapkey %d\n",
			op->carid,
			op->clocktickcount,
			op->pi.ol_uup,
			op->pi.ol_udown,
			op->pi.ol_uleft,
			op->pi.ol_uright,
			op->pi.ol_urollleft,
			op->pi.ol_urollright,
			op->pi.uspace,
			op->espace,
			op->pi.weapkey); */
/*	if (op==of && inlogmode) {
		logger("cantdrivestuff cr %d, cd %d clock %d\n",op->ol_doacrashreset,op->ol_cantdrive,op->clocktickcount);
	} */
	if (op->ol_doacrashreset || op->ol_cantdrive || op->clocktickcount<INITCANTSTARTDRIVE) {
		op->pi.uspace=op->pi.ol_uup=op->pi.ol_udown=op->pi.ol_uleft=op->pi.ol_uright=op->pi.ol_urollright=op->pi.ol_urollleft=0; // can't drive when crashing
	}
// handle weapons
//	weapkind=-1;
	wk=op->pi.weapkey-1;
//	if (op->clocktickcount<INITCANTSTARTDRIVE)
//		wk=-1;
/*	if (op->randomizeron) {
		i=prandom1(op->clocktickcount,100);
		if (i==0) {
			int k;
			k=prandom2(op->clocktickcount,nw);
			k+=op->lastcurweap;
			k%=nw;
			logger("prandom %d\n",k);
			j=carstubinfos[op->carid].weaps[k];
			switch(j) {
			case WEAP_NANO:
			case WEAP_RANDOMIZER:
			case WEAP_WEAPBOOST:
			case WEAP_RPD:
				break;
			default:
				wk=k+1;
				logger("setting weapon to %d\n",wk);
				break;
			}
		}
	}
*/
	op->extraaccel=1;
//	op->extrafly=1;
	op->extratopspeed=1;
	op->extramagnatraction=0;
	op->extratraction=1;
	op->extrabigtires=0;
	op->extrapouncer=0;
	op->extrashieldsup--;
	if (op->extrashieldsup<0)
		op->extrashieldsup=0;
	op->xcurweapkind=carstubinfos[op->carid].cd.c_weaps[op->xcurweapslot];
	if (wk>=0 && wk!=op->xcurweapslot && !lweapkey) { // deploy
		if (op->xcurweapstate!=WEAPSTATE_DEPLOY) {
			if (op->xcurweapkind!=WEAP_WEAPBOOST)
				op->tweapbooston=0;
			if (1)
//			if (op->xcurweapkind!=WEAP_SHLDBSTR)
				op->tshieldbooston=0;
		}
		deactivateweap(op);
		op->xcurweapslot=wk;
//		op->xcurweapstate=WEAPSTATE_DEPLOY;
//		op->curweapvar=0;
		op->xweapframe=0;
		op->xcurweapkind=carstubinfos[op->carid].cd.c_weaps[op->xcurweapslot];
		if (of==op)
			ol_playatagsound2(op->xcurweapkind,WSOUND_DEPLOY,1,0);
//			ol_playatagsound(28,1,0);
	} else if (op->xcurweapstate==WEAPSTATE_DEPLOY && !lweapkey &&
		4*op->xweapframe>=weapinfos[op->xcurweapkind].deployend &&
		wk>=0 && wk==op->xcurweapslot && !op->vvweapstolen[wk] &&
		op->clocktickcount>=INITCANTSTARTDRIVE) { // activate

//	}
//	if (wk && op->curweap==-1 && !op->vvweapstolen[wk-1]) {
//		op->curweap=wk-1;
//		weapkind=carstubinfos[op->carid].weaps[op->curweap];
		if (checkenergy(op->tenergies,op->xcurweapkind)) {
//			op->curweap=-1;
//		} else
			logger("curweap activated %d\n",wk);
			if (op->woc<0)
				op->woc=op->xcurweapkind;
			op->xweapframe=weapinfos[op->xcurweapkind].actstart;
//			op->xweapframe=WEAPFRAME_ACTIVATE;
			op->xcurweapstate=WEAPSTATE_ACTIVE;
			useenergy(op->tenergies/*,op->lenergies*/,op->xcurweapkind);
			if (!weapinfos[op->xcurweapkind].special) /*&& op->curweap>=0 */
				op->lastactiveweapslot=op->xcurweapslot;
			if (of==op)
				ol_playatagsound2(op->xcurweapkind,WSOUND_ACTIVATE,1,0);
//			updateweapdisplay();
		}
//		logger("weapkind %d\n",weapkind);
/*		if (weapkind==WEAP_MISSILES) {
			for (i=0;i<NMISSILES;i++)
				if (op->missileinfos[i].active)
					break;
			if (i!=NMISSILES) {
				op->curweap=-1;	// can't fire missiles again until all 4 of the old ones die
				weapkind=-1;
			}
		} */
	}
//#define WEAPFRAME_DEPLOY 0
//#define WEAPFRAME_ACTIVATE 46
//#define WEAPFRAME_RESULT 82
//#define WEAPFRAME_RECTRACT 167
//#define WEAPFRAME_DONE 214 // one past last frame
// do frames and such
//	if (op->buzzbox0) {
//		op->buzzbox0->flags|=TF_DONTDRAWC;
//	}
	if (op->xcurweapstate==WEAPSTATE_DEPLOY) {        // deploy
		op->xweapframe+=.5f; // careful
		if (op->xweapframe>=weapinfos[op->xcurweapkind].deployend) {
			op->xweapframe=weapinfos[op->xcurweapkind].deployend;
		}
//	} else if (op->xcurweapstate==WEAPSTATE_STOLEN) {     // stolen ?
//		op->xweapframe=0;
	} else {													// active OR result
		op->xweapframe+=.5f; // careful

//		weapkind=carstubinfos[op->carid].weaps[op->curweap];
		S32 f2i;
		float ff;
		switch(op->xcurweapkind) {
// car-handling weapons
		case WEAP_FERRITE:
//			if (1) {
//			if (op->weapframe==WEAPFRAME_ACTIVATE) {
				if (op->carvel.y<od.ferriteyvelthresh) {
					top=op->pos;
					bot=op->pos;
					bot.y-=od.ferriteypos;
					if (op->tweapbooston)
						bot.y-=od.ferriteyposboost; // hover higher
					if (st2_line2road(&top,&bot,&top,NULL))
						op->carvel.y+=od.ferriteyaccel;
				}
//			}
			if (op->curweapvar>=200) {
				if (op->ol_cbairtime<1 || op->ol_airtime<1) {
//					op->xcurweapstate=WEAPSTATE_DEPLOY;
//					op->curweapvar=0;
					deactivateweap(op);
				}
			} else
				op->curweapvar++;
			break;
		case WEAP_RAMJET:
			op->extratopspeed=od.extraramjettopspeed;
			if (op->tweapbooston)
				op->extratopspeed*=1.5f;	// even faster
			break;
		case WEAP_INTAKE:
			op->extraaccel=od.extraintakeaccel;
			if (op->tweapbooston)
				op->extraaccel*=1.5f; // even more..
			break;
		case WEAP_SPIKES:
			op->extramagnatraction=1;
			break;
		case WEAP_BAINITE:
			op->extratraction=od.extrabainitetraction;
			if (op->tweapbooston)
				op->extratraction*=1.5f;
			break;
		case WEAP_AERODYNE:
			op->extratopspeed=od.extraaerodynetopspeed;
			if (op->tweapbooston)
				op->extratopspeed*=1.1f;	// even faster
//			op->extrafly=od.extrafly;
//			if (op->tweapbooston)
//				op->extrafly*=.5f;
			break;

// shield weapons
		case WEAP_DIELECTRIC:
			op->extrashieldsup=3;
//			if (op->curweapvar)
//				op->curweapvar=2;
			break;
		case WEAP_TRANSDUCER:
			op->extrashieldsup=3;
//			op->curweapvar=1;
			break;
		case WEAP_PLASMA:
			op->extrashieldsup=3;
			op->curweapvar++;
			if (op->tshieldbooston) {
				if (op->curweapvar>=od.plasmatime*2)
					op->xcurweapstate=WEAPSTATE_RESULT;
			} else {
				if (op->curweapvar>=od.plasmatime)
					op->xcurweapstate=WEAPSTATE_RESULT;
			}
			break;

// offensive weapons
		case WEAP_KLONDYKE:
			if (op->xweapframe>=weapinfos[op->xcurweapkind].actend)
				op->curweapvar=1; // ready to attack
			if (op->curweapvar==1) {
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						int k0,k1;
						k0=box2box_v2(&op->pos,&op->carang,&od.klondyke0boxmin,&od.klondyke0boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						k1=box2box_v2(&op->pos,&op->carang,&od.klondyke1boxmin,&od.klondyke1boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						if (k0+k1==1) {
							op->xcurweapstate=WEAPSTATE_RESULT;
							op->curweapvar=2;
						}
					}
				}
			}
			if (op->xweapframe==od.klondykekickframe) {
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						float ka;
						int k0,k1;
						k0=box2box_v2(&op->pos,&op->carang,&od.klondyke0boxmin,&od.klondyke0boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						k1=box2box_v2(&op->pos,&op->carang,&od.klondyke1boxmin,&od.klondyke1boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						if (k0+k1==1 && ncollinfo<MAXCOLLINFO) {
							if (opa[j].extrashieldsup) {
								float f;
								f=dist3d(&od.camposdp,&opa[j].pos)*(1/4.0f);
								if (f<od.voldist)
									f=1;
								f=od.voldist/f;
								shieldhit(WEAP_KLONDYKE,j,f);
							} else {
								ka=od.klondykekickamount;
								if (op->tweapbooston)
									ka*=1.5f; // twice the kick
								if (k0) // right klondyke
									norm=pointf3x(ka,0,0);
								else
									norm=pointf3x(-ka,0,0);
								quatrot(&op->carang,&norm,&collinfos[ncollinfo].impval);
								collinfos[ncollinfo].imppnt=zerov;
								collinfos[ncollinfo].cn=j;
								ncollinfo++;
								ol_playatagsound(18+mt_random(3),op->vold,op->pan);
							}
						}
					}
				}
			}
			break;
		case WEAP_MISSILES:
			for (i=0;i<NMISSILES;i++) {
				if (op->xweapframe==missilefireframes[i]) {
					j=0;
					break;
				}
				if (op->tweapbooston && op->xweapframe==missilefireframes[i]+1) {
					j=1;
					break; // twice the missiles
				}
			}
			if (i!=NMISSILES) { // generate a missile if it's time to
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,.125f,0);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=op->carang;
				top.x=top.z=0;
				top.y=1;
				top.w=((float)i-1.5f)*.2f;
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				top.y=top.z=0;
				top.x=1;
				top.w=-(float)od.missilestartupangle*(PI/180.0f);
				if (j)
					top.w*=3;
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				if (od.numcars==1)
					j=0; // self
				else {
					j=(op->carid+i)%(od.numcars-1);
					if (j==op->carid)
						j=od.numcars-1;
				}
				makeaflyweap(DETACH_MISSILES,j,&pos,&rot,&zerov,od.missilestartfuel,op->carid);
			}
			break;
		case WEAP_LASER:
		case WEAP_MINIGUN:
		case WEAP_PARTICLE:
			for (i=0;i<NMISSILES;i++) {
				if (op->xweapframe==laserfireframes[i]) {
					j=0;
					break;
				}
				if (op->tweapbooston && op->xweapframe==laserfireframes[i]+1) {
					j=1;
					break; // twice the missiles
				}
			}
			if (i!=NMISSILES) { // generate a missile if it's time to
				pointf3 pos,rot;
				pos=op->pos;
				top=od.laseroffset;
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=op->carang;
				top.x=top.z=0;
				top.y=1;
				top.w=((float)(3-i)+(float)j*.5f-1.5f)*.01f;
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				top.y=top.z=0;
				top.x=1;
				top.w=-(float)od.laserstartupangle*(PI/180.0f);
				if (j)
					top.w*=3;
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
/*				if (od.numcars==1)
					j=0; // self
				else {
					j=(op->carid+i)%(od.numcars-1);
					if (j==op->carid)
						j=od.numcars-1;
				} */
				j=op->carid;
				S32 awf=0;
				if (op->xcurweapkind==WEAP_LASER)
					awf=DETACH_LASER;
				else if (op->xcurweapkind==WEAP_MINIGUN)
					awf=DETACH_MINIGUN;
				else
					awf=DETACH_PARTICLE;
				makeaflyweap(awf,j,&pos,&rot,&zerov,od.laserstartfuel,op->carid);
			}
			break;
		case WEAP_OIL:
			if (op->tweapbooston)
				j=3;
			else
				j=7;
			ff=op->xweapframe*2;
			F2INT(ff,f2i);
			if ((f2i&j)==0 && op->xweapframe<od.oilstopframe) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(pfrand1(op->clocktickcount)-.5f,od.oildown,-od.oilback);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=zerov;
				rot.w=1;
				makeaflyweap(DETACH_OIL,-1,&pos,&rot,&zerov,od.oilstartfuel,op->carid);
			}
//			if (op->xweapframe>50)
//				op->xcurweapstate=WEAPSTATE_DEPLOY;
			break;
		case WEAP_EMB:
			if (op->xweapframe>=weapinfos[op->xcurweapkind].actend)
				op->curweapvar=1; // ready to attack
			if (op->curweapvar==1) {
				int besti=-1;
				float bestdist=0,dist;
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						dist=dist3dsq(&op->pos,&opa[j].pos);
						if (besti==-1 || dist<bestdist) {
							bestdist=dist;
							besti=j;
						}
						if (besti>=0 && bestdist<od.embrad*od.embrad) {
							op->xcurweapstate=WEAPSTATE_RESULT;
							op->curweapvar=2+besti;
						}
					}
				}
			}
			for (i=0;i<NMISSILES;i++) {
				if (op->xweapframe==embfireframes[i]) {
					j=0;
					break;
				}
				if (op->tweapbooston && op->xweapframe==embfireframes[i]+1) {
					j=1;
					break; // twice the missiles
				}
			}
			if (i!=NMISSILES) { // generate a missile if it's time to
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,.125f,0);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=op->carang;
				top.x=top.z=0;
				top.y=1;
				top.w=((float)i-1.5f)*.2f;
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				top.y=top.z=0;
				top.x=1;
				top.w=-(float)od.embstartupangle*(PI/180.0f);
				if (j)
					top.w*=3;
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				makeaflyweap(DETACH_EMB,op->curweapvar-2,&pos,&rot,&zerov,od.embstartfuel,op->carid);
			}
			break;
		case WEAP_ECTO:
//			if (op->tweapbooston)
//				j=15;
//			else
//				j=31;
			if (op->xweapframe==od.ectoframe ||
				(op->tweapbooston && (op->xweapframe==od.ectoframe+4 || op->xweapframe==od.ectoframe+8))) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,od.ectoup,0);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=zerov;
				rot.w=1;
				makeaflyweap(DETACH_ECTO,op->carid,&pos,&rot,&zerov,od.ectostartfuel,op->carid);
			}
			break;

// special weapons
		case WEAP_WEAPBOOST:
//			if (op->xweapframe==100)
			op->tweapbooston=1+op->xcurweapslot;
			break;
		case WEAP_RPD:
			if (op->xweapframe==od.rpdstealframe) {
				if (od.numcars>1) {
					int ii=0,jj=0,el;
					int off=prandom3(op->clocktickcount,od.numcars);
					for (i=off;i<od.numcars+off;i++) {
						ii=i%od.numcars;
						if (carstubinfos[ii].cd.c_nweap==0)
							continue; // next car
						if (ii!=op->carid && dist3dsq(&op->pos,&opa[ii].pos)<od.rpddistance*od.rpddistance) {
//							int off2=prandom2(op->clocktickcount,carstubinfos[ii].nweap);
							jj=pickrandomweaprpd(&opa[ii],op);
//							logger("nweap mt_random = %d\n",off2);
							if (jj!=-1)
								break;
						}
					}
					if (i!=od.numcars+off) {
						el=carstubinfos[ii].cd.c_weaps[jj];
						logger("weapon stolen from car %d, weap # %d, weap slot %d, name '%s'\n",
							ii,el,jj,weapinfos[el]);
						carstubinfos[op->carid].cd.c_weaps[MAXWEAPONSCARRY]=el;
						opa[ii].vvweapstolen[jj]=1;
						op->ttweapstolenfrom=ii;
						op->ttweapstolenslot=jj;
						op->lastspecialweapslot=op->xcurweapslot;
						op->xcurweapslot=MAXWEAPONSCARRY;
						op->xcurweapkind=carstubinfos[op->carid].cd.c_weaps[op->xcurweapslot];
						if (of==op)
							ol_playatagsound2(op->xcurweapkind,WSOUND_ACTIVATE,1,0);
						op->xweapframe=0;//.0f;
					} else {
//						op->xcurweapstate=WEAPSTATE_DEPLOY;
//						op->curweapvar=0;
						deactivateweap(op);
					}
				}
			}
			break;
		case WEAP_NANO:
			if (op->xweapframe==weapinfos[WEAP_NANO].actend) {
				if (op->lastactiveweapslot>=0) {
					op->lastspecialweapslot=op->xcurweapslot;
					op->xcurweapslot=op->lastactiveweapslot;
					op->xweapframe=0;//.0f;
					op->xcurweapkind=carstubinfos[op->carid].cd.c_weaps[op->xcurweapslot];
					if (of==op)
						ol_playatagsound2(op->xcurweapkind,WSOUND_ACTIVATE,1,0);
				} else {
					op->xcurweapstate=WEAPSTATE_DEPLOY;
					op->curweapvar=0;
				}
			}
			break;
		case WEAP_RANDOMIZER:
			if (op->xweapframe==weapinfos[WEAP_NANO].actend) {
				op->lastactiveweapslot=pickrandomweap(op);
				if (op->lastactiveweapslot>=0) {
					op->lastspecialweapslot=op->xcurweapslot;
					op->xcurweapslot=op->lastactiveweapslot;
					op->xcurweapkind=carstubinfos[op->carid].cd.c_weaps[op->xcurweapslot];
					if (of==op)
						ol_playatagsound2(op->xcurweapkind,WSOUND_ACTIVATE,1,0);
					op->xweapframe=0; //WEAPFRAME_ACTIVATE;
				} else {
					op->xcurweapstate=WEAPSTATE_DEPLOY;
					op->curweapvar=0;
				}
			}
			break;
		case WEAP_ICRMS:
			if (op->xweapframe==od.icrmsfreezeframe) {
//				if (od.numcars>1) {
//				if (!op->curweapvar) {
					for (i=0;i<od.numcars;i++)
						if (/*i!=op->carid && */dist3dsq(&op->pos,&opa[i].pos)<od.icrmsdistance*od.icrmsdistance) {
							struct ol_playerdata *ops;
//							static void ol_dolightning(int);
							if (opa[i].extrashieldsup) {
								float f;
								f=dist3d(&od.camposdp,&opa[i].pos)*(1/4.0f);
								if (f<od.voldist)
									f=1;
								f=od.voldist/f;
								shieldhit(WEAP_ICRMS,i,f);
							} else {
								opa[i].venableicrms=od.icrmsduration;
								if (op->tweapbooston)
									opa[i].venableicrms*=2; // frozen longer
								logger("icrms duration set to %d\n",op->venableicrms);
								ops=op;
								op=&opa[i];
								ol_dolightning(2);
								op=ops;
							}
						}
				}
//				op->curweapvar=1;
/*				} else {
					op->venableicrms=od.icrmsduration; // test: 1 car freezes itself
					if (op->tweapbooston)
						op->venableicrms*=2;
					op->vicrmsparm=1; */
//				}
//			}
			break;
//////////// new weapons (21)
		case WEAP_BIGTIRES:
			op->extrabigtires=1;
			i=op->carid;
			for (j=0;j<od.numcars;j++) {
				if (i!=j) {
					if (opa[j].pos.x+od.underbigtireacross>opa[i].pos.x &&
						opa[j].pos.x-od.underbigtireacross<opa[i].pos.x &&
						opa[j].pos.z+od.underbigtireacross>opa[i].pos.z &&
						opa[j].pos.z-od.underbigtireacross<opa[i].pos.z &&
						opa[j].pos.y+od.underbigtiredown1<opa[i].pos.y &&
						opa[j].pos.y+od.underbigtiredown2>opa[i].pos.y)
							opa[j].vunderbigtire=20;
				}
			}
			break;
		case WEAP_BUZZ:
			if (op->xweapframe>=weapinfos[op->xcurweapkind].actend && op->curweapvar==0)
				op->curweapvar=1; // ready to attack
			if (op->curweapvar==1) {
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						int k0,bx,bz;
//						pointf3 buzz0boxmin,buzz0boxmax;
						bx=bz=0;
						od.buzz0boxmin.x=bx-od.buzzsizex;
						od.buzz0boxmin.y=od.buzzup-od.buzzsizey;
						od.buzz0boxmin.z=bz-od.buzzsizez;
						od.buzz0boxmax.x=bx+od.buzzsizex;
						od.buzz0boxmax.y=od.buzzup+od.buzzsizey;
						od.buzz0boxmax.z=bz+od.buzzsizez;
						k0=box2box_v2(&op->pos,&op->carang,&od.buzz0boxmin,&od.buzz0boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						if (k0) {
							op->xcurweapstate=WEAPSTATE_RESULT;
							op->xweapframe=weapinfos[op->xcurweapkind].actend;
							op->curweapvar=2;
						}
					}
				}
			}
			if (op->xweapframe==od.buzzkickframe) {
//			if (op->xweapframe>=weapinfos[WEAP_BUZZ].actstart) {
//				int bf;
				float bx,bz;//,ang;
//				pointf3 buzz0boxmin,buzz0boxmax;
				i=op->carid;
//				bf=f2int(op->xweapframe*2.0f);
//				if (!od.predicted)
//					logger("buzzsaw active frame %d, clock %d\n",bf,opa[0].clocktickcount);
//				if ((bf&15)==0)
//				ang=(op->xweapframe-weapinfos[WEAP_BUZZ].actstart)/
//					(weapinfos[WEAP_BUZZ].actend-weapinfos[WEAP_BUZZ].actstart)*TWOPI;
//				bx=od.buzzleft*rcos(ang);
//				bz=-od.buzzleft*rsin(ang);
				bx=bz=0;
				od.buzz0boxmin.x=bx-od.buzzsizex;
				od.buzz0boxmin.y=od.buzzup-od.buzzsizey;
				od.buzz0boxmin.z=bz-od.buzzsizez;
				od.buzz0boxmax.x=bx+od.buzzsizex;
				od.buzz0boxmax.y=od.buzzup+od.buzzsizey;
				od.buzz0boxmax.z=bz+od.buzzsizez;
				if (op->buzzbox0) {
					op->buzzbox0->flags&=~TF_DONTDRAWC;
					op->buzzbox0->trans=od.buzz0boxmin;
					op->buzzbox0->scale.x=od.buzz0boxmax.x-od.buzz0boxmin.x;
					op->buzzbox0->scale.y=od.buzz0boxmax.y-od.buzz0boxmin.y;
					op->buzzbox0->scale.z=od.buzz0boxmax.z-od.buzz0boxmin.z;
				}
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						float ka;
						int k0;
						if (!opa[j].vinbuzzspin) {
//							od.buzz0boxmax;
//							od.buzz0boxmin;
							k0=box2box_v2(&op->pos,&op->carang,&od.buzz0boxmin,&od.buzz0boxmax,
								&opa[j].pos,&opa[j].carang,
								&opa[j].ol_carbboxmin,
								&opa[j].ol_carbboxmax,
								&loc,&norm);
							if (k0 && ncollinfo<MAXCOLLINFO) {
								if (opa[j].extrashieldsup) {
									float f;
									f=dist3d(&od.camposdp,&opa[j].pos)*(1/4.0f);
									if (f<od.voldist)
										f=1;
									f=od.voldist/f;
									shieldhit(WEAP_BUZZ,j,f);
								} else {
									ka=od.buzzkickamount;
									if (op->tweapbooston)
										ka*=1.5f; // twice the kick
									norm.x=opa[j].pos.x-op->pos.x;
									norm.y=opa[j].pos.y-op->pos.y;
									norm.z=opa[j].pos.z-op->pos.z;
									if (normalize3d(&norm,&norm)>0) {
//										norm.x=-ka*rcos(ang);
//										norm.y=0;
//										norm.z=ka*rsin(ang);
										norm.x*=ka;
										norm.z*=ka;
//										setVEC(&norm,-ka,0,0);
										quatrot(&op->carang,&norm,&collinfos[ncollinfo].impval);
										collinfos[ncollinfo].imppnt=zerov;
										collinfos[ncollinfo].cn=j;
										if (!od.predicted)
											logger("buzzsaw hit clk %d, ncol %d, v %f %f %f, car %d\n",
											opa[0].clocktickcount,ncollinfo,
											collinfos[ncollinfo].impval.x,
											collinfos[ncollinfo].impval.y,
											collinfos[ncollinfo].impval.z,j);
										ncollinfo++;
										opa[j].vinbuzzspin=45;
										ol_playatagsound(18+mt_random(3),op->vold,op->pan);
									}
								}
							}
						}
					}
				}
			}
			break;
/*		case WEAP_CHEMBOOST:
			op->extratopspeed=od.extrachemboosttopspeed;
			if (op->tweapbooston)
				op->extratopspeed*=1.5f;	// even faster
			if (op->clocktickcount&1) {
				pointf3 pos;
				pos=op->pos;
				setVEC(&top,(pfrand1(op->clocktickcount)-.5f)*.0625f,od.chemtraildown,-od.chemtrailback);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				spawnchemtrail(&pos);
			}
			break;
		case WEAP_CHEMVAC:
			if (op->xweapframe==WEAPFRAME_ACTIVATE+.5f) {
				if (od.numcars>1) {
					int ii;
					int off=prandom3(op->clocktickcount,od.numcars);
					for (i=off;i<od.numcars+off;i++) {
						ii=i%od.numcars;
						if (opa[ii].tenergies[ENERGY_CHEMICAL]<3)
							continue; // next car
						if (ii!=op->carid && dist3dsq(&op->pos,&opa[ii].pos)<od.chemvacdistance*od.chemvacdistance) {
							opa[ii].tenergies[ENERGY_CHEMICAL]-=3;
							op->tenergies[ENERGY_CHEMICAL]+=3;
						}
					}
				}
			}
			break;
*/		case WEAP_ENLEECH:
			if (op->xweapframe==weapinfos[WEAP_ENLEECH].actstart+.5f) {
				if (od.numcars>1) {
					int k,t=2;	// how many to take
					for (i=0;i<od.numcars;i++) { // find close cars
						if (i!=op->carid) {
							float dist=dist3dsq(&op->pos,&opa[i].pos);
							if (dist<od.enleechdistance*od.enleechdistance) {
								int me=opa[i].tenergies[ENERGY_ELECTRICAL];
								if (me<t)
									k=me;
								else
									k=t;
//								if (me>=k) {
									opa[i].tenergies[ENERGY_ELECTRICAL]-=k;
									op->tenergies[ENERGY_ELECTRICAL]+=k;
									opa[i].vinleech=10;
//								}
							}
						}
					}
				}
			}
			break;
/*		case WEAP_ENUPTAKE:
			if (op->xweapframe==WEAPFRAME_ACTIVATE+.5f)
				for (i=0;i<MAXENERGIES;i++) {
					op->tenergies[i]+=op->lenergies[i];
					op->lenergies[i]=0;
				}
			break;
		case WEAP_FSNBOO:
			op->extraaccel=od.extrafsnbooaccel;
			if (op->tweapbooston)
				op->extraaccel*=1.5f; // even more..
			break;
*/		case WEAP_FSNCAN:
			if (op->xweapframe==weapinfos[WEAP_FSNCAN].actstart+.5f) {
				pointf3 pos,rot;
				pos=op->pos;
				top=od.fusionoffset;
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=op->carang;
				top.y=top.z=0;
				top.x=1;
				top.w=-(float)od.fusionstartupangle*(PI/180.0f);
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				j=op->carid;
				sf=2*od.fusionstartfuel;
				if (op->tweapbooston)
					sf*=2;
				makeaflyweap(DETACH_FUSIONCAN,j,&pos,&rot,&zerov,sf,op->carid);
			}
			break;
/*		case WEAP_GEOSHIELD:
			op->extrashieldsup=3;
			break;
*/		case WEAP_GRAPPLE:
			if (op->curweapvar==0 && op->xcurweapstate==WEAPSTATE_ACTIVE) {
				// first look for car close by
				int bestj=-1;
				float bestdist=0,dist,gr;
				if (op->tweapbooston)
					gr=od.grapplerad*1.5f;
				else
					gr=od.grapplerad;
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						dist=dist3dsq(&op->pos,&opa[j].pos);
						if ((bestj==-1 || dist<bestdist) && dist<gr*gr) {
							pointf3 sep,ca;
							float dot;
							sep.x=opa[j].pos.x-opa[i].pos.x;
							sep.y=opa[j].pos.y-opa[i].pos.y;
							sep.z=opa[j].pos.z-opa[i].pos.z;
							ca.x=ca.y=0;
							ca.z=1;
							quatrot(&opa[i].carang,&ca,&ca);
							dot=sep.x*ca.x+sep.z*ca.z;
							if (dot>0) {
								bestdist=dist;
								bestj=j;
							}
						}
					}
				}
				if (bestj>=0) { // found a car, spawn the grapple
					pointf3 pos,rot;
					pos=op->pos;
					top=od.grappleoffset;
					quatrot(&op->carang,&top,&top);
					pos.x+=top.x;
					pos.y+=top.y;
					pos.z+=top.z; // start them above the car
					rot=op->carang;
					top.y=top.z=0;
					top.x=1;
					top.w=-(float)od.fusionstartupangle*(PI/180.0f);
					rotaxis2quat(&top,&top);
					quattimes(&rot,&top,&rot);
					j=op->carid;
					sf=2*od.grapplestartfuel;
					if (op->tweapbooston)
						sf*=2;
					makeaflyweap(DETACH_GHOOK,bestj,&pos,&rot,&zerov,sf,op->carid);
					op->curweapvar++;
					op->thooknum=j;
					op->vhooknum=bestj;
				}
			}
			if (op->curweapvar>120)
				deactivateweap(op);
			else if (op->curweapvar>0) { // used for frame
				int dp,cp,sp;
				int mu,md,ml,mr;
				float dot,crs;
				pointf3 sep;
				pointf3 cb;
				int bestj=op->vhooknum;
				i=op->carid;
				if (op->curweapvar>GRAPPLETIME) {
					sep.x=opa[bestj].pos.x-opa[i].pos.x;
					sep.y=opa[bestj].pos.y-opa[i].pos.y;
					sep.z=opa[bestj].pos.z-opa[i].pos.z;
// victim
					cb.x=cb.y=0;
					cb.z=1;
					quatrot(&opa[bestj].carang,&cb,&cb);
					dot=sep.x*cb.x+sep.z*cb.z;
					if (dot>0)
						dp=1;
					else
						dp=0;
					crs=cb.x*sep.z-cb.z*sep.x;
					if (crs>0)
						cp=1;
					else
						cp=0;
					if (opa[bestj].ol_accelspin>0)
						sp=1;
					else
						sp=0;
					mu=!dp;
					ml=dp^cp^sp;
					md=1-mu;
					mr=1-ml;
					opa[bestj].magnum=mu+(md<<1)+(ml<<2)+(mr<<3);
					opa[bestj].inghook=3;
// originator
/*					ca.x=ca.y=0;
					ca.z=1;
					quatrot(&opa[i].carang,&ca,&ca);
					dot=sep.x*ca.x+sep.z*ca.z;
					if (dot>0)
						dp=1;
					else
						dp=0;
					crs=ca.x*sep.z-ca.z*sep.x;
					if (crs>0)
						cp=1;
					else
						cp=0;
					if (opa[i].ol_accelspin>0)
						sp=1;
					else
						sp=0;
					mu=dp;
					ml=dp^cp^sp;
					md=1-mu;
					mr=1-ml;
					opa[i].magnum=mu+(md<<1)+(ml<<2)+(mr<<3);
					opa[i].inghook=3; */
				}
				op->curweapvar++;
			}
			break;
/*		case WEAP_HOLO:
			op->extrashieldsup=3;
			break;
*/		case WEAP_MAGNET:
			if (1) {
				int bestj=-1;
				float bestdist,dist,mr;
				if (op->tweapbooston)
					mr=od.magnetrad*1.5f;
				else
					mr=od.magnetrad;
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						dist=dist3dsq(&op->pos,&opa[j].pos);
						if (/*(bestj==-1 || dist<bestdist) && */dist<mr*mr) {
							bestdist=dist;
							bestj=j;
							{
								int dp,cp,sp;
								int mu,md,ml,mr;
								float dot,crs;
								pointf3 sep;
								pointf3 cb;//ca;
								sep.x=opa[j].pos.x-opa[i].pos.x;
								sep.y=opa[j].pos.y-opa[i].pos.y;
								sep.z=opa[j].pos.z-opa[i].pos.z;
			// victim
								cb.x=cb.y=0;
								cb.z=1;
								quatrot(&opa[j].carang,&cb,&cb);
								dot=sep.x*cb.x+sep.z*cb.z;
								if (dot>0)
									dp=1;
								else
									dp=0;
								crs=cb.x*sep.z-cb.z*sep.x;
								if (crs>0)
									cp=1;
								else
									cp=0;
								if (opa[j].ol_accelspin>0)
									sp=1;
								else
									sp=0;
								mu=dp;
								ml=dp^cp^sp;
								md=1-mu;
								mr=1-ml;
								opa[j].magnum=mu+(md<<1)+(ml<<2)+(mr<<3);
								opa[j].inmagnet=3;
			// originator
/*								ca.x=ca.y=0;
								ca.z=1;
								quatrot(&opa[i].carang,&ca,&ca);
								dot=sep.x*ca.x+sep.z*ca.z;
								if (dot>0)
									dp=1;
								else
									dp=0;
								crs=ca.x*sep.z-ca.z*sep.x;
								if (crs>0)
									cp=1;
								else
									cp=0;
								if (opa[i].ol_accelspin>0)
									sp=1;
								else
									sp=0;
								mu=!dp;
								ml=dp^cp^sp;
								md=1-mu;
								mr=1-ml;
								opa[i].magnum=mu+(md<<1)+(ml<<2)+(mr<<3);
								opa[i].inmagnet=3; */
							}
						}
					}
				}
				if (bestj<0 || op->curweapvar>240)
					op->xcurweapstate=WEAPSTATE_RESULT;
				else
					op->curweapvar++;
			}
			break;
		case WEAP_MINES:
			if (op->xweapframe==weapinfos[WEAP_MINES].actstart+.5f) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,od.minedown,-od.mineback);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=zerov;
				rot.w=1;
				makeaflyweap(DETACH_MINE,-1,&pos,&rot,&zerov,op->tweapbooston,op->carid);
				od.minearmed=0;
			}
//			if (op->xweapframe>50)
//				op->xcurweapstate=WEAPSTATE_DEPLOY;
			break;
/*		case WEAP_NITRO:
			op->extratopspeed=od.extranitrotopspeed;
			if (op->tweapbooston)
				op->extratopspeed*=1.5f;	// even faster
			break;
		case WEAP_NRGCON:
			if (op->xweapframe==WEAPFRAME_ACTIVATE+.5f)
					op->tenergies[ENERGY_FUSION]+=3;
			break;
		case WEAP_POOLBOO:
			if (op->xweapframe==WEAPFRAME_ACTIVATE+.5f)
					op->tenergies[ENERGY_ELECTRICAL]+=3;
			break;
*/		case WEAP_POUNCER:
			if (op->xweapframe>=od.pouncerframe)
				op->extrapouncer=1;
			break;
		case WEAP_PRISM:
			op->extrashieldsup=3;
			break;
/*		case WEAP_SHLDBSTR:
//			if (op->xweapframe==100)
			op->tshieldbooston=1+op->xcurweapslot;
			break;
*/		case WEAP_SONIC:
			for (i=0;i<NMISSILES;i++) {
				if (op->xweapframe==sonicfireframes[i])
					break;
//				if (op->tweapbooston && op->xweapframe==sonicfireframes[i]+1)
//					break; // twice the missiles
			}
			if (i!=NMISSILES) { // generate a missile if it's time to
				int sf;
				pointf3 pos,rot;
				pos=op->pos;
				top=od.sonicoffset;
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
//				rot=op->carang;
				rot=zerov;
				rot.w=1;
				top.y=top.z=0;
				top.x=1;
				top.w=-(float)od.sonicstartupangle*(PI/180.0f);
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				j=op->carid;
				sf=od.sonicstartfuel;
				if (op->tweapbooston)
					sf<<=1;
				makeaflyweap(DETACH_SONIC,j,&pos,&rot,&zerov,sf,op->carid);
			}
			break;
/*		case WEAP_THERMCAN:
			if (op->xweapframe==weapinfos[WEAP_THERMCAN].actstart+.5f) {
				pointf3 pos,rot;
				j=0;
				pos=op->pos;
				top=od.thermooffset;
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=op->carang;
				top.y=top.z=0;
				top.x=1;
				top.w=-(float)od.thermostartupangle*(PI/180.0f);
				rotaxis2quat(&top,&top);
				quattimes(&rot,&top,&rot);
				j=op->carid;
				sf=2*od.thermostartfuel;
				if (op->tweapbooston)
					sf*=2;
				makeaflyweap(DETACH_THERMOCAN,j,&pos,&rot,&zerov,sf,op->carid);
			}
			break;
*/
		case WEAP_SLEDGE:
			if (op->xweapframe==od.sledgehitframe && op->ol_airtime<2) {
				for (i=0;i<od.numcars;i++) {
					if (opa[i].ol_airtime<2) {
						if (i!=op->carid) {
							float dsq;
							dsq=dist3dsq(&op->pos,&opa[i].pos);
							if (dsq<od.sledgerad*od.sledgerad) {
								float f;
								f=dist3d(&od.camposdp,&opa[i].pos)*(1/4.0f);
								if (f<od.voldist)
									f=1;
								f=od.voldist/f;
								if (opa[i].extrashieldsup) {
									shieldhit(WEAP_SLEDGE,i,f);
								} else {
									opa[i].vinsonic=20;
									bot=pointf3x(1,0,0);
									spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
									bot=pointf3x(0,1,0);
									spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
									bot=pointf3x(0,0,1);
									spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
									ol_playatagsound(18,f,0);
								}
							}
						} else
							opa[i].vinsonic=3;
					}
				}
			}
			break;
		case WEAP_ENERGYWALL:
//			if (op->tweapbooston)
//				j=15;
//			else
//				j=31;
			j=od.wallstartfuel;
			if (op->tweapbooston) {
				j*=3;
				j>>=1;
			}
			if (op->xweapframe==od.wallframe) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,od.wallup,0);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
//				rot=zerov;
//				rot.w=1;
				rot=op->carang;
				makeaflyweap(DETACH_WALL,op->carid,&pos,&rot,&zerov,j,op->carid);
			}
			break;
		case WEAP_STASIS:
//			if (op->tweapbooston)
//				j=15;
//			else
//				j=31;
			j=od.stasisstartfuel;
			if (op->tweapbooston) {
				j*=3;
				j>>=1;
			}
			if (op->xweapframe==od.stasisframe) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,od.stasisup,0);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=zerov;
				rot.w=1;
				makeaflyweap(DETACH_STASIS,op->carid,&pos,&rot,&zerov,j,op->carid);
			}
			break;
		case WEAP_SUPAJUMP:
//			if (op->tweapbooston)
//				j=15;
//			else
//				j=31;
			j=od.rampstartfuel;
			if (op->tweapbooston) {
				j*=3;
				j>>=1;
			}
			if (op->xweapframe==od.rampframe) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,od.rampup,od.rampfront);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
//				rot=zerov;
//				rot.w=1;
				rot=op->carang;
				makeaflyweap(DETACH_RAMP,op->carid,&pos,&rot,&zerov,j,op->carid);
			}
			break;
		case WEAP_FORKLIFT:
			if (op->xweapframe>=weapinfos[op->xcurweapkind].actend && op->curweapvar==0)
				op->curweapvar=1; // ready to attack
			if (op->curweapvar==1) {
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						int k0,bx,bz;
						bx=bz=0;
						k0=box2box_v2(&op->pos,&op->carang,&od.fork0boxmin,&od.fork0boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						if (k0) {
							op->xcurweapstate=WEAPSTATE_RESULT;
							op->xweapframe=weapinfos[op->xcurweapkind].actend;
							op->curweapvar=2;
						}
					}
				}
			}
			if (op->xweapframe==od.forkkickframe) {
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						float ka;
						int k0;
						k0=box2box_v2(&op->pos,&op->carang,&od.fork0boxmin,&od.fork0boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						if (k0 && ncollinfo<MAXCOLLINFO) {
							if (opa[j].extrashieldsup) {
								float f;
								f=dist3d(&od.camposdp,&opa[j].pos)*(1/4.0f);
								if (f<od.voldist)
									f=1;
								f=od.voldist/f;
								shieldhit(WEAP_FORKLIFT,j,f);
							} else {
								ka=od.forkkickamount;
								if (op->tweapbooston)
									ka*=1.5f; // twice the kick
								norm=pointf3x(0,ka,0);
/*								collinfos[ncollinfo].impval=norm;
								collinfos[ncollinfo].imppnt=zerov;
								collinfos[ncollinfo].cn=j;
								if (!od.predicted)
									logger("forklift hit clk %d, ncol %d, v %f %f %f, car %d\n",
									opa[0].clocktickcount,ncollinfo,
									collinfos[ncollinfo].impval.x,
									collinfos[ncollinfo].impval.y,
									collinfos[ncollinfo].impval.z,j);
								ncollinfo++; */
								opa[j].carvel.y+=ka;
								ol_playatagsound(18+mt_random(3),op->vold,op->pan);
							}
						}
					}
				}
			}
			break;
		case WEAP_BUMPERCAR:
			if (op->xweapframe>=weapinfos[op->xcurweapkind].actend && op->curweapvar==0)
				op->curweapvar=1; // ready to attack
			if (op->curweapvar==1) {
				i=op->carid;
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						int k0,bx,bz;
						bx=bz=0;
						od.bumper0boxmin.x=bx-od.bumpersizex;
						od.bumper0boxmin.y=od.bumperup-od.bumpersizey;
						od.bumper0boxmin.z=bz-od.bumpersizez;
						od.bumper0boxmax.x=bx+od.bumpersizex;
						od.bumper0boxmax.y=od.bumperup+od.bumpersizey;
						od.bumper0boxmax.z=bz+od.bumpersizez;
						k0=box2box_v2(&op->pos,&op->carang,&od.bumper0boxmin,&od.bumper0boxmax,
							&opa[j].pos,&opa[j].carang,
							&opa[j].ol_carbboxmin,
							&opa[j].ol_carbboxmax,
							&loc,&norm);
						if (k0) {
							op->xcurweapstate=WEAPSTATE_RESULT;
							op->xweapframe=weapinfos[op->xcurweapkind].actend;
							op->curweapvar=2;
						}
					}
				}
			}
			if (op->xweapframe==od.bumperkickframe) {
//				int bf;
				float bx,bz;//,ang;
//				pointf3 buzz0boxmin,buzz0boxmax;
				i=op->carid;
//				bf=f2int(op->xweapframe*2.0f);
//				if (!od.predicted)
//					logger("buzzsaw active frame %d, clock %d\n",bf,opa[0].clocktickcount);
//				if ((bf&15)==0)
//				ang=(op->xweapframe-weapinfos[WEAP_BUZZ].actstart)/
//					(weapinfos[WEAP_BUZZ].actend-weapinfos[WEAP_BUZZ].actstart)*TWOPI;
//				bx=od.buzzleft*rcos(ang);
//				bz=-od.buzzleft*rsin(ang);
				bx=bz=0;
				od.bumper0boxmin.x=bx-od.bumpersizex;
				od.bumper0boxmin.y=od.bumperup-od.bumpersizey;
				od.bumper0boxmin.z=bz-od.bumpersizez;
				od.bumper0boxmax.x=bx+od.bumpersizex;
				od.bumper0boxmax.y=od.bumperup+od.bumpersizey;
				od.bumper0boxmax.z=bz+od.bumpersizez;
				if (op->bumperbox0) {
					op->bumperbox0->flags&=~TF_DONTDRAWC;
					op->bumperbox0->trans=od.bumper0boxmin;
					op->bumperbox0->scale.x=od.bumper0boxmax.x-od.bumper0boxmin.x;
					op->bumperbox0->scale.y=od.bumper0boxmax.y-od.bumper0boxmin.y;
					op->bumperbox0->scale.z=od.bumper0boxmax.z-od.bumper0boxmin.z;
				}
				for (j=0;j<od.numcars;j++) { // look for other cars
					if (j!=i || od.numcars==1) {
						pointf3 loc,norm;
						float ka;
						int k0;
//						if (!opa[j].vinbumperspin) {
//							od.bumper0boxmax;
//							od.bumper0boxmin;
							k0=box2box_v2(&op->pos,&op->carang,&od.bumper0boxmin,&od.bumper0boxmax,
								&opa[j].pos,&opa[j].carang,
								&opa[j].ol_carbboxmin,
								&opa[j].ol_carbboxmax,
								&loc,&norm);
							if (k0 && ncollinfo<MAXCOLLINFO) {
								if (opa[j].extrashieldsup) {
									float f;
									f=dist3d(&od.camposdp,&opa[j].pos)*(1/4.0f);
									if (f<od.voldist)
										f=1;
									f=od.voldist/f;
									shieldhit(WEAP_BUMPERCAR,j,f);
								} else {
									ka=od.bumperkickamount;
									if (op->tweapbooston)
										ka*=1.5f; // twice the kick
									norm.x=opa[j].pos.x-op->pos.x;
									norm.y=opa[j].pos.y-op->pos.y;
									norm.z=opa[j].pos.z-op->pos.z;
									if (normalize3d(&norm,&norm)>0) {
//										norm.x=-ka*rcos(ang);
//										norm.y=0;
//										norm.z=ka*rsin(ang);
										norm.x*=ka;
										norm.z*=ka;
//										setVEC(&norm,-ka,0,0);
//										quatrot(&op->carang,&norm,&collinfos[ncollinfo].impval);
										collinfos[ncollinfo].impval=norm;
										collinfos[ncollinfo].imppnt=zerov;
										collinfos[ncollinfo].cn=j;
										if (!od.predicted)
											logger("bumpercar hit clk %d, ncol %d, v %f %f %f, car %d\n",
											opa[0].clocktickcount,ncollinfo,
											collinfos[ncollinfo].impval.x,
											collinfos[ncollinfo].impval.y,
											collinfos[ncollinfo].impval.z,j);
										ncollinfo++;
//										opa[j].vinbumperspin=45;
										ol_playatagsound(18+mt_random(3),op->vold,op->pan);
									}
								}
							}
//						}
					}
				}
			}
			break;
		case WEAP_GYROSCOPE:
			break;
		case WEAP_GRAVITYBOMB:
//			if (op->tweapbooston)
//				j=15;
//			else
//				j=31;
			j=od.gbombstartfuel;
			if (op->tweapbooston) {
				j*=3;
				j>>=1;
			}
			if (op->xweapframe==od.gbombframe) {
				pointf3 pos,rot;
				pos=op->pos;
				top=pointf3x(0,od.gbombup,0);
				quatrot(&op->carang,&top,&top);
				pos.x+=top.x;
				pos.y+=top.y;
				pos.z+=top.z; // start them above the car
				rot=zerov;
				rot.w=1;
				makeaflyweap(DETACH_GBOMB,op->carid,&pos,&rot,&zerov,j,op->carid);
			}
			break;
		case WEAP_FLASHER:
			if (op->xweapframe==weapinfos[WEAP_FLASHER].actstart+.5f) {
				if (od.numcars>1) {
					for (i=0;i<od.numcars;i++) { // find close cars
						if (i!=op->carid) {
							float f=0,dist=dist3d(&op->pos,&opa[i].pos);
							if (dist<od.flashermindistance)
								f=od.flashermaxstrength;
							else if (dist<od.flashermaxdistance)
								f=od.flashermaxstrength*(od.flashermaxdistance-dist)/
									(od.flashermaxdistance-od.flashermindistance);
							if (op->tweapbooston)
								f*=1.5f;
							opa[i].vinflash=opa[i].vinflash+(int)f;
						}
					}
				} else
					opa[0].vinflash=100;
			}
			break;
		case WEAP_CHARGER:
			if (op->xweapframe>=weapinfos[WEAP_CHARGER].actstart+.5f) {
				if (op->xweapframe<od.chargerfastframe) {
					op->extratopspeed=.5f;
				} else {
					op->extratopspeed=od.extrachargertopspeed;
					if (op->tweapbooston)
						op->extratopspeed*=1.5f;	// even faster
				}
			}
			break;
		case WEAP_SWITCHAROO:
			if (op->xweapframe==od.switcharooframe) {
				pointf3 temp;
				float r,minr=1e20f;
				j=-1;
				r=od.switcharoorad*od.switcharoorad;
				if (op->tweapbooston)
					r*=1.5f*1.5f;
				for (i=0;i<od.numcars;i++) {
					if (i!=op->carid) {
						float f;
						f=dist3dsq(&op->pos,&opa[i].pos);
						if (f<r && f<minr) {
							j=i;
							minr=f;
						}
					}

				}
				if (j>=0) {
					temp=opa[j].pos;
					opa[j].pos=op->pos;
					op->pos=temp;
					temp=opa[j].carvel;
					opa[j].carvel=op->carvel;
					op->carvel=temp;
					temp=opa[j].carang;
					opa[j].carang=op->carang;
					op->carang=temp;
					temp=opa[j].carvelang;
					opa[j].carvelang=op->carvelang;
					op->carvelang=temp;
				}
			}
			break;
		}
// turn off weapon when done
		if (op->xcurweapstate==WEAPSTATE_ACTIVE) {
			if (op->xweapframe>=weapinfos[op->xcurweapkind].actend) {
				if (weapinfos[op->xcurweapkind].persist) {
					if (weapinfos[op->xcurweapkind].actloop)
						op->xweapframe=weapinfos[op->xcurweapkind].actstart;
					else
						op->xweapframe=weapinfos[op->xcurweapkind].actend;
				} else {
//					op->xcurweapstate=WEAPSTATE_DEPLOY;
//					op->curweapvar=0;
//					op->xweapframe=0;
					deactivateweap(op);
				}
			}
		} else if (op->xcurweapstate==WEAPSTATE_RESULT) {
			if (op->xweapframe>=weapinfos[op->xcurweapkind].resend)
//				op->xcurweapstate=WEAPSTATE_DEPLOY;
//				op->curweapvar=0;
				deactivateweap(op);
		}

/* 	if (op->xweapframe>=WEAPFRAME_DONE) {
//			op->xweapframe=0;
			if (op->xcurweapslot==MAXWEAPONSCARRY) { // give stolen weapon back
				tree2 *t;
				int oc;
				oc=op->ttweapstolenfrom;
				t=opa[oc].weaptrees[op->ttweapstolenslot];
				unhooktree(t);
				linkchildtoparent(t,opa[oc].ol_carnull);
				t->flags|=TF_DONTDRAWC;
				seq_setframe(t,0);
				opa[oc].vvweapstolen[op->ttweapstolenslot]=0;
			}
			op->xcurweapstate=WEAPSTATE_DEPLOY;
		} */
//		updateweapdisplay();
	}
// sync weapon visibility to weapframe and curframe
	op->xcurweapkind=carstubinfos[op->carid].cd.c_weaps[op->xcurweapslot];
	updateweapdisplay();
	for (i=0;i<nw;i++) {
		wt=op->weaptrees[i];
		if (wt && !op->vvweapstolen[i]) {
			if (op->xcurweapslot==i || op->tweapbooston-1==i || op->tshieldbooston-1==i) {
				wt->flags&=~TF_DONTDRAWC;
				if (op->tweapbooston-1==i && op->xcurweapslot!=i) {
					if (op->xweapboostframe<weapinfos[WEAP_WEAPBOOST].actstart)
						op->xweapboostframe=weapinfos[WEAP_WEAPBOOST].actstart;
					seq_setframe(wt,op->xweapboostframe);
					op->xweapboostframe+=.5f;
					if (op->xweapboostframe>weapinfos[WEAP_WEAPBOOST].actend)
						op->xweapboostframe=weapinfos[WEAP_WEAPBOOST].actstart;
/*				} else if (op->tshieldbooston-1==i && op->xcurweapslot!=i) {
					if (op->xweapboostframe<weapinfos[WEAP_SHLDBSTR].actstart)
						op->xweapboostframe=weapinfos[WEAP_SHLDBSTR].actstart;
					seq_setframe(wt,op->xweapboostframe);
					op->xweapboostframe+=.5f;
					if (op->xweapboostframe>weapinfos[WEAP_SHLDBSTR].actend)
						op->xweapboostframe=weapinfos[WEAP_SHLDBSTR].actstart;
				*/
				} else
					seq_setframe(wt,op->xweapframe);
#if 0
				list<tree2*>::iterator it;
				tree2* tp;
//				for (j=0;j<wt->nchildren;j++) {
				for (it=wt->children.begin();it!=wt->children.end();++it) {
					tp=*it;
					int k;
					if (!tp->userint[0]) {
						tp->userpointf3[0]=tp->scale;
						tp->userint[0]=1;
					}
					k=tp->userint[1];
					if (k>=0)
						tp->trans=op->regpointsoffset[k];
					tp->scale.x=tp->userpointf3[0].x*od.weapscale;
					tp->scale.y=tp->userpointf3[0].y*od.weapscale;
					tp->scale.z=tp->userpointf3[0].z*od.weapscale;
				}
#endif
			} else
				wt->flags|=TF_DONTDRAWC;
			if (op->xcurweapstate==WEAPSTATE_ACTIVE && i==op->xcurweapslot && op->xcurweapkind==WEAP_BIGTIRES) {
				if (op->xweapframe>=weapinfos[WEAP_BIGTIRES].actstart) {
//					op->bigtiretrees[0]->userproc=op->bigtiretrees[1]->userproc=
//								op->bigtiretrees[2]->userproc=op->bigtiretrees[3]->userproc=NULL;
					op->bigtiretrees[0]->doanim=op->bigtiretrees[1]->doanim=
					op->bigtiretrees[2]->doanim=op->bigtiretrees[3]->doanim=false;
					for (j=0;j<4;j++) {
						pointf3 q,q2;
						if (j==1 || j==2) { // steer
							q.x=0;
							q.y=1;
							q.z=0;
							q.w=op->wheelyaw*PIOVER180;
							rotaxis2quat(&q,&q);
						} else {
							q.x=0;
							q.y=0;
							q.z=0;
							q.w=1;
						}
						q2.x=1;
						q2.y=0;
						q2.z=0;
						if ((j&2)==2)
							q2.w=op->wheelang[j]*-.5f;//*PIOVER180;
						else
							q2.w=op->wheelang[j]*.5f;//*PIOVER180;
						rotaxis2quat(&q2,&q2);
						quattimes(&q,&q2,&q);
						quatnormalize(&q,&q);
						op->bigtiretrees[j]->rot=q;
						op->bigtiretrees[j]->userproc=NULL;
					}
				} else {
					op->bigtiretrees[0]->doanim=op->bigtiretrees[1]->doanim=
					op->bigtiretrees[2]->doanim=op->bigtiretrees[3]->doanim=true;
				}
			}
		}
	}
	if (op->xcurweapslot==MAXWEAPONSCARRY && op->ttweapstolenfrom>=0) { // a stolen weapon
		tree2 *t;
		int oc;
		oc=op->ttweapstolenfrom;
		t=opa[oc].weaptrees[op->ttweapstolenslot];
		if (t) {
			unhooktree(t);
			linkchildtoparent(t,op->ol_carnull);
			t->flags&=~TF_DONTDRAWC;
			seq_setframe(t,op->xweapframe);

			list<tree2*>::iterator it;
			tree2* tp;
//			for (j=0;j<t->nchildren;j++) {
			for (it=wt->children.begin();it!=wt->children.end();++it) {
				tp=*it;
				int k;
				if (!tp->userint[0]) {
					tp->userpointf3[0]=tp->scale;
					tp->userint[0]=1;
				}
				k=tp->userint[1];
				if (k>=0)
					tp->trans=op->regpointsoffset[k];
				tp->scale.x=tp->userpointf3[0].x*od.weapscale;
				tp->scale.y=tp->userpointf3[0].y*od.weapscale;
				tp->scale.z=tp->userpointf3[0].z*od.weapscale;
			}
// handle an active stolen bigtire
			if (op->xcurweapstate==WEAPSTATE_ACTIVE && op->xcurweapkind==WEAP_BIGTIRES) {
				if (op->xweapframe>=weapinfos[WEAP_BIGTIRES].actstart) {
					for (j=0;j<4;j++) {
						pointf3 q,q2;
						if (j==1 || j==2) { // steer
							q.x=0;
							q.y=1;
							q.z=0;
							q.w=op->wheelyaw*PIOVER180;
							rotaxis2quat(&q,&q);
						} else {
							q.x=0;
							q.y=0;
							q.z=0;
							q.w=1;
						}
						q2.x=1;
						q2.y=0;
						q2.z=0;
						if ((j&2)==2)
							q2.w=op->wheelang[j]*-.5f;//*PIOVER180;
						else
							q2.w=op->wheelang[j]*.5f;//*PIOVER180;
						rotaxis2quat(&q2,&q2);
						quattimes(&q,&q2,&q);
						quatnormalize(&q,&q);
						opa[oc].bigtiretrees[j]->rot=q;
//						opa[oc].bigtiretrees[j]->proc=NULL; // NYI
					}
				} else {
//					opa[oc].bigtiretrees[0]->proc=opa[oc].bigtiretrees[1]->proc=
//						opa[oc].bigtiretrees[2]->proc=opa[oc].bigtiretrees[3]->proc=NULL; // NYI
				}
			}
		}
	}
// do car stunts etc.
	if (op->ol_startstunt) {
		if (op->pi.ol_uup && !lup || op->pi.ol_udown && !ldown)
			op->ol_uready=1;
	}
	if (!op->ol_flymode) {
		int l,r,u,d,t,ml,mr,mu,md;//,tu,td,tl,tr;
		op->ol_startstunt=op->ol_uready=0;
		u=op->pi.ol_uup;
		d=op->pi.ol_udown;
		l=op->pi.ol_uleft;
		r=op->pi.ol_uright;
		if (op->inmagnet || op->inghook) { // move cars apart
			mu=op->magnum&1;
			md=(op->magnum&2)>>1;
			ml=(op->magnum&4)>>2;
			mr=(op->magnum&8)>>3;
//			tu=u&~md|mu&~d;
//			td=d&~mu|md&~u;
//			tr=r&~ml|mr&~l;
//			tl=l&~mr|ml&~r;
//			u=tu;d=td;l=tl,r=tr;
			u=mu;d=md,l=ml,r=mr;
		}
		if (op->vinemb) { // reverse left and right
			t=r;
			r=l;
			l=t;
		}
		if (op->vunderbigtire)
			u=d=0;
		if (op->vinthermo) {
			if (op->ol_accelspin>0)
				d=1;
			else
				d=0;
			u=0;
		}
		if (op->vinfusion) {
			d=op->clocktickcount&1;
			u=0;
		}
		if (op->venableicrms) {
			u=d=0;
		}
		op->pi.ol_uup=u;
		op->pi.ol_udown=d;
		op->pi.ol_uleft=l;
		op->pi.ol_uright=r;
		if (u /*&& drivemode!=CARBRAKE*/) { // pitch down
			if (op->ol_accelspin>=-1) {
				float sa;
				op->ol_drivemode=CARACCEL;
				sa=op->ol_startaccelspin*op->extratopspeed;
				if (op->speedup)
					sa*=od.speedupspeed; //2
				if (op->ol_accelspin>sa)
					op->ol_drivemode=CARCOAST;
			} else {
				op->ol_drivemode=CARBRAKE;
			}
		}
		if (d ) { // pitch up
			if (op->ol_accelspin<1) {
				op->ol_drivemode=CARREVERSEACCEL;
				if (op->ol_accelspin<-op->ol_startaccelspin/2)
					op->ol_drivemode=CARCOAST;
			} else {
				op->ol_drivemode=CARBRAKE;
			}
		}
		if (!u && !d)
			op->ol_drivemode=CARCOAST;
		if (op->slowdown && op->ol_accelspin>pfrand1(op->clocktickcount)*op->ol_startaccelspin*op->extratopspeed)
			op->ol_drivemode=CARBRAKE;
//		if (op->dofinish)
//			op->ol_drivemode=CARBRAKE;
//		op->vinemb=10;
		if (l) { // yaw left
			if (op->wheelyaw>0)
				op->wheelyaw-=op->ol_steervelback*od.ol_timeincconst;
			else
				op->wheelyaw-=op->ol_steervel*od.ol_timeincconst;
			if (op->wheelyaw<-MAXSTEER)
				op->wheelyaw=-MAXSTEER;
		}
		if (r) { // yaw right
/*			if (op==of && inlogmode) {
				logger("in yaw right %f %f\n",op->wheelyaw,op->ol_steervel);
			} */
			if (op->wheelyaw>0)
				op->wheelyaw+=op->ol_steervel*od.ol_timeincconst;
			else
				op->wheelyaw+=op->ol_steervelback*od.ol_timeincconst;
			if (op->wheelyaw>MAXSTEER)
				op->wheelyaw=MAXSTEER;
		}
		if (!l && !r) {
			if (op->wheelyaw>op->ol_steervelback*od.ol_timeincconst)
				op->wheelyaw-=op->ol_steervelback*od.ol_timeincconst;
			else if (op->wheelyaw<-op->ol_steervelback*od.ol_timeincconst)
				op->wheelyaw+=op->ol_steervelback*od.ol_timeincconst;
			else
				op->wheelyaw=0;
		}
	} else { // fly
		int dogyro=0;
		if (!op->ol_startstunt) {
			// just left ground
			float t;
			ol_clearpies();
			quat2rpy(&op->carang,&op->ol_carstuntrpy); // convert from quats to rpy
			op->carangsave=op->carang;
			op->ol_carstuntrpy.x*=PIUNDER180;
			op->ol_carstuntrpy.y*=PIUNDER180;
			op->ol_carstuntrpy.z*=PIUNDER180;
			op->ol_startstunt = 1; // start a stunt
			op->carvelang=zerov;
// calculate a landing spot
			op->ol_littleg=op->ol_littlegair;
//			op->ol_littleg=op->ol_littlegair*op->extrafly;
			t=2.0f*op->carvel.y/op->ol_littleg;
//			t=(op->carvel.y+
//				(float)sqrt(op->carvel.y*op->carvel.y+2*op->ol_littleg*op->pos.y))/op->ol_littleg;
			t*=1.4f;
//			logger("aistunt est. air time %f\n",t);
//			if (t>od.camlandtime)
				op->ol_startstunt=2; // a long upward jump
//			if (op->cntl==CNTL_AI)
				setupaistunttime(t);
			op->ol_landspot.x=op->pos.x+op->carvel.x*t;
//			op->ol_landspot.y=op->pos.y-.5f; // put it alittle under ground
			op->ol_landspot.y=-3; // put it alittle under ground
			op->ol_landspot.z=op->pos.z+op->carvel.z*t;
		}
		pitch=op->ol_uthiscarpitch;
		yaw=op->ol_uthiscaryaw;
		roll=op->ol_uthiscarroll;
		if (op->venableicrms) {
			pitch=0;
			yaw=0;
			roll=0;
//			pitch*=op->vicrmsparm;
//			yaw*=op->vicrmsparm;
//			roll*=op->vicrmsparm;
//			if (op->vicrmsparm==0)
				op->ol_ucarrotvel=zerov;
		}
		if (op->vinfusion) {
			op->ol_uready=1;
			op->pi.ol_udown=1;
			op->pi.ol_uup=0;
		}
		// see if in need of gyro
		if (op->xcurweapkind==WEAP_GYROSCOPE && op->xcurweapstate==WEAPSTATE_ACTIVE && op->ol_airtime>od.gyroscopeairtime) {
			pointf3 bot;
			bot.x=op->pos.x+op->carvel.x*od.gyroscopeline2road;
			bot.y=op->pos.y+op->carvel.y*od.gyroscopeline2road;
			bot.z=op->pos.z+op->carvel.z*od.gyroscopeline2road;
			if (st2_line2road(&op->pos,&bot,&bot,NULL))
				dogyro=1;
		}
		if (dogyro) { // gyro pitch and roll
			op->ol_ucarrotvel.x=0;
			if (op->ol_carstuntrpy.x<=180) {
				if (op->ol_carstuntrpy.x>=od.gyroturnrpy.x)
					op->ol_carstuntrpy.x-=od.gyroturnrpy.x;
				else
					op->ol_carstuntrpy.x=0;
			} else {
				if (op->ol_carstuntrpy.x-360<=od.gyroturnrpy.x)
					op->ol_carstuntrpy.x+=od.gyroturnrpy.x;
				else
					op->ol_carstuntrpy.x=0;
			}
			op->ol_ucarrotvel.z=0;
			if (op->ol_carstuntrpy.z<=180) {
				if (op->ol_carstuntrpy.z>=od.gyroturnrpy.z)
					op->ol_carstuntrpy.z-=od.gyroturnrpy.z;
				else
					op->ol_carstuntrpy.z=0;
			} else {
				if (op->ol_carstuntrpy.z-360<=od.gyroturnrpy.z)
					op->ol_carstuntrpy.z+=od.gyroturnrpy.z;
				else
					op->ol_carstuntrpy.z=0;
			}
		} else { // normal pitch and roll
			if (op->pi.ol_uup && (op->ol_uready||(op->xcurweapkind==WEAP_FERRITE&&op->xcurweapstate==WEAPSTATE_ACTIVE)))
				op->ol_ucarrotvel.x += pitch;
			if (op->pi.ol_udown && (op->ol_uready||(op->xcurweapkind==WEAP_FERRITE&&op->xcurweapstate==WEAPSTATE_ACTIVE)))
				op->ol_ucarrotvel.x -= pitch;
			if (op->pi.ol_urollleft /*&& uready*/)
				op->ol_ucarrotvel.z += roll;
			if (op->pi.ol_urollright /*&& uready*/)
				op->ol_ucarrotvel.z -= roll;
		}
		if (op->pi.ol_uright /*&& uready*/)
			op->ol_ucarrotvel.y += yaw;
		if (op->pi.ol_uleft /*&& uready*/)
			op->ol_ucarrotvel.y -= yaw;
		if (op->xcurweapkind!=WEAP_FERRITE || op->xcurweapstate!=WEAPSTATE_ACTIVE) { // normal
			op->ol_carstuntrpy.x+=op->ol_ucarrotvel.x/2;
			op->ol_carstuntrpy.y+=op->ol_ucarrotvel.y/2;
			op->ol_carstuntrpy.z+=op->ol_ucarrotvel.z/2;
			op->ol_carstuntrpy.x=ol_normang(op->ol_carstuntrpy.x);
			op->ol_carstuntrpy.y=ol_normang(op->ol_carstuntrpy.y);
			op->ol_carstuntrpy.z=ol_normang(op->ol_carstuntrpy.z);
			op->curstuntangs.x+=op->ol_ucarrotvel.x/2; // unnormalized version
			op->curstuntangs.y+=op->ol_ucarrotvel.y/2;
			op->curstuntangs.z+=op->ol_ucarrotvel.z/2;
		} else { // ferrite flying
			float spd;
			struct mat4 anmat;
			pointf3 anrpy,anvec;
			if (op->ol_flymode)
				op->ol_flymode=2;
			op->ol_uready=0;
			op->ol_ucarrotvel.z=0;
			op->ol_carstuntrpy.x+=op->ol_ucarrotvel.x/4;
			op->ol_carstuntrpy.y+=op->ol_ucarrotvel.y/4;
			op->ol_carstuntrpy.z-=op->ol_ucarrotvel.y/4;
			op->ol_carstuntrpy.x=ol_normang(op->ol_carstuntrpy.x);
			op->ol_carstuntrpy.y=ol_normang(op->ol_carstuntrpy.y);
			op->ol_carstuntrpy.z=ol_normang(op->ol_carstuntrpy.z);
			if (op->ol_carstuntrpy.x>=180) {
				if (op->ol_carstuntrpy.x<315) {
					op->ol_carstuntrpy.x+=3;
					op->ol_ucarrotvel.x=0;
				} else
					op->ol_carstuntrpy.x+=.5f;
			} else {
				if (op->ol_carstuntrpy.x>45) {
					op->ol_carstuntrpy.x-=3;
					op->ol_ucarrotvel.x=0;
				} else
					op->ol_carstuntrpy.x-=.5f;
			}
			if (op->ol_carstuntrpy.z>=180) {
				if (op->ol_carstuntrpy.z<315) {
					op->ol_carstuntrpy.z+=3;
				}
			} else {
				if (op->ol_carstuntrpy.z>45) {
					op->ol_carstuntrpy.z-=3;
				}
			}
// now add in vels..
			anrpy.x=op->ol_carstuntrpy.x*PIOVER180;
			anrpy.y=op->ol_carstuntrpy.y*PIOVER180;
			anrpy.z=op->ol_carstuntrpy.z*PIOVER180;
			buildrot3d(&anrpy,&anmat);
			anvec.x=anvec.z=0;
			anvec.y=od.ferritexzaccel;
			xformvec(&anmat,&anvec,&anvec);
			spd=op->carvel.x*op->carvel.x+op->carvel.z*op->carvel.z;
			if (spd<od.ferritemaxspeed*od.ferritemaxspeed) {
				op->carvel.x+=anvec.x;
				op->carvel.z+=anvec.z;
			}
			op->carvel.x*=od.ferritefric;
			op->carvel.z*=od.ferritefric;
		}

		op->ol_ucarrotvel.x-=op->ol_ucarrotvel.x/op->ol_defaultjumpfriction/2;
		op->ol_ucarrotvel.y-=op->ol_ucarrotvel.y/op->ol_defaultjumpfriction/2;
		op->ol_ucarrotvel.z-=op->ol_ucarrotvel.z/op->ol_defaultjumpfriction/2;

//		logger("aistunt yaw, s %7.3f, v %7.3f, a %7.3f, f %7.3f\n",
//			op->ol_carstuntrpy.y,op->ol_ucarrotvel.y,op->ol_uthiscarroll,op->ol_defaultjumpfriction);
		doatricksound1=ol_addpitchpie(op->ol_carstuntrpy.x);
		if (op->xcurweapkind!=WEAP_FERRITE|| op->xcurweapstate!=WEAPSTATE_ACTIVE)
			doatricksound2=ol_addyawpie(op->ol_carstuntrpy.y);
		doatricksound3=ol_addrollpie(op->ol_carstuntrpy.z);
		if (doatricksound1) {
			float tf;
			sh = wave_playvol(taggetwhbyidx(od.ol_gardentags,AIRZ0190),1,op->vold);
			if (sh) {
				tf=getsfxfreq(sh);
				setsfxfreq(sh,tf+op->ol_ttp*150.0f/sh->basefreq);
				sh->isplayasound=1;
			}
			op->ol_udidpitch=1;
			op->ol_ttp++;
		}
		if (doatricksound2) {
			float tf;
			sh = wave_playvol(taggetwhbyidx(od.ol_gardentags,AIRZ0191),1,op->vold);
			if (sh) {
				tf=getsfxfreq(sh);
				setsfxfreq(sh,tf+op->ol_ttp*150.0f/sh->basefreq);
				sh->isplayasound=1;
			}
			op->ol_udidyaw=1;
			op->ol_ttp++;
		}
		if (doatricksound3) {
			float tf;
			sh = wave_playvol(taggetwhbyidx(od.ol_gardentags,AIRZ0192),1,op->vold);
			if (sh) {
				tf=getsfxfreq(sh);
				setsfxfreq(sh,tf+op->ol_ttp*150.0f/sh->basefreq);
				sh->isplayasound=1;
			}
			op->ol_udidroll=1;
			op->ol_ttp++;
		}
		if (doatricksound1)
			op->ol_ttp+=op->ol_udidroll+op->ol_udidyaw;
		if (doatricksound2)
			op->ol_ttp+=op->ol_udidroll+op->ol_udidpitch;
		if (doatricksound3)
			op->ol_ttp+=op->ol_udidpitch+op->ol_udidyaw;
		if (!op->pi.ol_urollright && !op->pi.ol_urollleft) {
			if (op->ol_carstuntrpy.z>3 && op->ol_carstuntrpy.z<180) {
				op->ol_carstuntrpy.z-=2;
				op->curstuntangs.z-=2;
			} else if (op->ol_carstuntrpy.z<357 && op->ol_carstuntrpy.z>180) {
				op->ol_carstuntrpy.z+=2;
				op->curstuntangs.z+=2;
			} else if (op->ol_carstuntrpy.z>180) { // 358, 359
				op->curstuntangs.z-=(op->ol_carstuntrpy.z-360);
				op->ol_carstuntrpy.z=0;
			} else { // 0,1,2
				op->curstuntangs.z-=op->ol_carstuntrpy.z;
				op->ol_carstuntrpy.z=0;
			}
		}
	}
}

void ol_getpiece(pointf3* v,int *x,int *z)
{
	if (od.ntrackname[0]!='\0') {
		*x=*z=0;
	} else {
		*x = (int)((3.5f*od.piecesizex+v->x+40*od.piecesizex)/od.piecesizex)-40;
		*z = (int)((3.5f*od.piecesizez-v->z+40*od.piecesizez)/od.piecesizez)-40;
	}
}

//////////////// end new roadheight code /////
void ol_douburst(int numbursts)
{
//	int i;
	op->ol_nubursts+=numbursts;
//	if (op->ol_nubursts)
//		return;
	op->ol_ufireworkscale=(float)(op->ol_nubursts/5);
	op->ol_ufireworkscale=range(1.5f,op->ol_ufireworkscale,3.0f);
/*	for (i=0;i<4;i++) {
		linkchildtoparent(op->ol_ubursta[i],op->ol_carnull);
		linkchildtoparent(op->ol_uburstb[i],op->ol_carnull);
	}
*/
}

static void ol_dolightning(int numlightnings)
{
	if (op->ol_nulightnings)
		return;
	if (od.ol_slowpo)
		return;
	op->ol_nulightnings=numlightnings;
}

void ol_docondom(int numcondoms)
{
	if (op->ol_nucondoms)
		return;
	op->ol_nucondoms=numcondoms;
}

static void ol_doblueflash(int numflashes)
{
	if (op->ol_nuflashes)
		return;
	op->ol_nuflashes=numflashes;
}

static void ol_douflames(int numflames)
{
	if (op->ol_nuflames)
		return;
	op->ol_nuflames=numflames;
}

// update animtex for clock,coin,speedo,burst
//static void ol_uplayupdateclock()
//{
//	struct bitmap16 *b16;
//	int i;
//	int m=1,t=4,o=2;
//	int tt,tt2;
// time left
//(ustarttime*30+29)*2
// now they want time from start
/*	if (od.numplayers!=od.numcurplayers) {
		tt=60*od.numplayers;
		tt2=od.numcurplayers;
	} else if (of->clocktickcount<INITCANTSTARTDRIVE) {
//		tt=INITCANTSTARTDRIVE-of->clocktickcount+60-1;
//		tt2=1;
		tt=tt2=0;
	} else {
		if (of->dofinish) {
			tt=of->dofinish-INITCANTSTARTDRIVE;
//			tt2=(tt%60)*100/60;
			tt2=range(1,od.place,od.numcars);
		} else {
			tt=of->clocktickcount-INITCANTSTARTDRIVE;
			tt2=range(1,od.place,od.numcars);
		}
//JAY
		if (currule->ctf) {
			tt = of->ol_numflags * 60;
			tt2 = range(1,od.place,od.numcars);
		} else {
			if (of->dofinish) {
				tt=of->dofinish-INITCANTSTARTDRIVE;
//				tt2=(tt%60)*100/60;
			} else {
				tt=of->clocktickcount-INITCANTSTARTDRIVE;
			}
			tt2=range(1,od.place,od.numcars);
		} */
/*	tt=range(0,tt,10*60*60-1);
	if (od.ol_oldtt!=tt) {
		od.ol_oldtt=tt;
		m=tt/(60*60);
		tt-=m*60*60;
		t=(tt)/600;
		tt-=t*600;
		o=tt/60; */
// time left
/*		if (of->ol_clocktickcount<10*60 && (of->ol_clocktickcount/2)%10>5)
			m=t=o=10;
*///		if (od.ol_usehires) { // hires
//		if (!currule->ctf) {
/*			od.ol_clocktsp->lastframe=-1;
			tspsetframe(od.ol_clocktsp,m);
			tspanimtex(od.ol_clocktsp,14,-2,1);
			od.ol_clocktsp->lastframe=-1;
			tspsetframe(od.ol_clocktsp,t);
			tspanimtex(od.ol_clocktsp,40,-2,1);
			od.ol_clocktsp->lastframe=-1;
			tspsetframe(od.ol_clocktsp,o);
			tspanimtex(od.ol_clocktsp,65,-2,1); */
//		}
//JAY
		//od.ol_clocktsp->lastframe=-1;
		//tspsetframe(od.ol_clocktsp,m);
		//tspanimtex(od.ol_clocktsp,14,-2,1);
		//od.ol_clocktsp->lastframe=-1;
		//tspsetframe(od.ol_clocktsp,t);
		//tspanimtex(od.ol_clocktsp,40,-2,1);
		//od.ol_clocktsp->lastframe=-1;
		//tspsetframe(od.ol_clocktsp,o);
		//tspanimtex(od.ol_clocktsp,65,-2,1);
//		}*/
//	}
// now they want coins caught
/*	tt2=range(0,tt2,99);
	if (od.ol_oldncoins!=tt2) {
		od.ol_oldncoins=tt2;
		t=(tt2)/10;
		o=(tt2)%10;
		if (od.ol_usehires) { // hires
			b16=locktexture(od.ol_numcointex);
			cliprect32(b16,0,0,b16->x-1,b16->y-1,0xffff);
//			clipxpar16(od.ol_clocktsp->frames[t],b16,0,0,42,32-5,od.ol_clocktsp->x,od.ol_clocktsp->y);
//			clipxpar16(od.ol_clocktsp->frames[o],b16,0,0,67,32-5,od.ol_clocktsp->x,od.ol_clocktsp->y);
			unlocktexture(od.ol_numcointex);
		}
	} */
// speedo
/*	o=(int)fabs(of->ol_accelspin*od.ol_speedotsp->nframe/of->ol_startaccelspin);
	if (o<1)
		o=1;
	if (o>=od.ol_speedotsp->nframe)
		o=od.ol_speedotsp->nframe-1;
	if (od.ol_oldspeed!=o) {
		od.ol_oldspeed=o;
		tspsetframe(od.ol_speedotsp,o); // gets set when tspanimtex gets called
		tspanimtex(od.ol_speedotsp,0,0,0);
	} */
// energies
//	if (currule->useweap) {
/*		for (i=0;i<MAXENERGIES;i++) {
//			char str[20];
			int e;//,x;
			e=of->tenergies[i];
			if (e!=od.lastenergies[i] || od.curweapicon!=od.lastcurweapicon) {
				od.lastenergies[i]=e;
//			e=(e*(od.ol_speedotsp->nframe-2)+87)/99;
				e=range(0,e,od.ol_speedotsp->nframe-1);
				b16=locktexture(od.energytex[i]);
				clipblit32(od.ol_speedotsp->frames[e],b16,
					0,0,0,0,od.ol_speedotsp->x,od.ol_speedotsp->y);
				if (od.curweapicon>=0)
					sprintf(str,"%d",olracecfg.energymatrix[od.curweapicon][i]);
//				sprintf(str,"%d . %d",olracecfg.energymatrix[od.curweapicon][i],of->tenergies[i]);
				else
					str[0]='\0';
//				sprintf(str,"%d",of->tenergies[i]);
//			x=ol_lendrawstring2(od.ol_tscorefont4,str,256);
//			x=(256-x)>>1;
				x=20;
				ol_drawstring2(od.ol_tscorefont5,b16,str,x,0,256);
				unlocktexture(od.energytex[i]);
			}
		} */
// weapons
//		if (opa[0].clocktickcount>0) {
//			updatescoreline(of);
/*			i=range(-1,od.curweapicon,od.weapicontsp->nframe-1);
			if (i!=od.lastcurweapicon || strcmp(od.curweapstring,od.lastweapstring)) {
				int dx=43,dy=0;
//		dx=od.testint1;
//		dy=od.testint2;
				b16=locktexture(od.curweaptex);
				if (i<0)
					cliprect32(b16,
						0,0,od.weapicontsp->x-1,od.weapicontsp->y-1,0);
				else
					clipblit32(od.weapicontsp->frames[i],b16,
						0,0,0,0,od.weapicontsp->x,od.weapicontsp->y);
//		ol_drawstring2(od.ol_tscorefont4,b16,od.curweapstring,0,44,64);
				unlocktexture(od.curweaptex);

				b16=locktexture(od.weapdesctex);
				if (!od.weapdescsave) {
					od.weapdescsave=
						bitmap16alloc(od.weapdesctex->texdata->x,od.weapdesctex->texdata->y,-1);
						clipblit32(b16,od.weapdescsave,0,0,0,0,od.weapdescsave->x,od.weapdescsave->y);
				} else {
//			cliprect32(b16,0,0,b16->x-1,b16->y-1,0);
					clipblit32(od.weapdescsave,b16,0,0,0,0,b16->x,b16->y);
				}
				ol_drawstring2(od.ol_tscorefont4,b16,od.curweapstring,dx,dy,128);
				unlocktexture(od.weapdesctex);
				od.lastcurweapicon=i;
				strcpy(od.lastweapstring,od.curweapstring);
			}

			i=range(-1,od.prevweapicon,od.weapicontsp->nframe-1);
			if (i!=od.lastprevweapicon) {
				b16=locktexture(od.prevweaptex);
				if (i<0)
					cliprect32(b16,
						0,0,od.weapicontsp->x-1,od.weapicontsp->y-1,0);
				else
					clipblit32(od.weapicontsp->frames[i],b16,
						0,0,0,0,od.weapicontsp->x,od.weapicontsp->y);
				od.lastprevweapicon=i;
				unlocktexture(od.prevweaptex);
			}

			i=range(-1,od.nextweapicon,od.weapicontsp->nframe-1);
			if (i!=od.lastnextweapicon) {
				b16=locktexture(od.nextweaptex);
				if (i<0)
					cliprect32(b16,
						0,0,od.weapicontsp->x-1,od.weapicontsp->y-1,0);
				else
					clipblit32(od.weapicontsp->frames[i],b16,
						0,0,0,0,od.weapicontsp->x,od.weapicontsp->y);
				od.lastnextweapicon=i;
				unlocktexture(od.nextweaptex);
			}*/
//		}
//	}
// gothisway
//	od.gothisway->scale=olracecfg.gothiswayscale;
//	setVEC(&od.gothisway->scale,olracecfg.gothiswayscale,olracecfg.gothiswayscale,olracecfg.gothiswayscale);
//	od.gothisway->trans=olracecfg.gothiswaypos;
//	od.gothisway->rot=olracecfg.gothiswayrot;
//	od.gothisway->rot.z=of->dh;
//}
// burst
static void ol_uplayupdateclock2() {
#if 1
	int j;//,k;
	pointf3 tc;
	if (op->ol_nubursts>0) {
//		struct bitmap32 *b16;
		if (op->ol_nubursts>=20) {
			tc=F32MAGENTA;
			j=3;
		} else if (op->ol_nubursts>=10) {
			tc=F32BLUE;
			j=2;
		} else if (op->ol_nubursts>=5) {
			tc=F32RED;
			j=1;
		} else {
			tc=F32YELLOW;
			j=0;
		}
		if (op->ol_burstframe==0) {
			od.intimewarp=0; // force playatagsound to play, sleezy
			ol_playatagsound(FIREWORK,op->vold,op->pan);
			tree2* u=duptree(od.ol_fwork[3]);
			u->transvel.y+=.025f;
			u->transvel.x=.015f;
			u->userproc=ol_burstfinishproc2;
			u->treecolor=tc;
			u->treecolor.w=.5f;
			u->userint[2]=50; // time to live
			u->flags|=TF_ALWAYSFACING; // NYI
			u->scale=pointf3x(SPARKSCALE,SPARKSCALE,SPARKSCALE);
			linkchildtoparent(u,op->ol_carnull);
			u=duptree(u);
			u->transvel.x=-.015f;
			linkchildtoparent(u,op->ol_carnull);
			od.intimewarp=1;
		}
/*		tspsetframe(op->ol_bursttsp[j],op->ol_burstframe);
		tspanimtex(op->ol_bursttsp[j],0,0,0);
*/
#endif
#if 0
		b16=locktexture(op->bursttex);
//		cliprect32(b16,0,0,b16->x-1,b16->y-1,himagenta);
		clipblit32(od.ol_bursttsp[j]->frames[op->ol_burstframe],b16,0,0,0,0,od.ol_bursttsp[j]->x,od.ol_bursttsp[j]->y);
//		clipblit32(t->frames[t->curframe],b16,0,0,offx,offy,t->frames[0]->x,t->frames[0]->y);
		//	b16->x,b16->y);
//		if (texformat==TEX555CK)
//			clipcircleo16(b16,mt_random(256),mt_random(256),mt_random(30),rand());
//		else
//			clipcircleo16(b16,mt_random(256),mt_random(256),mt_random(30),mt_random(65536));
		unlocktexture(op->bursttex);

//		for (k=0;k<4;k++)
//			if (k==j) {
#endif
				op->ol_burstframe++;
#if 0
				op->ol_ufireworkscale=range(1.5f,op->ol_ufireworkscale,3.0f);
				op->ol_ubursta->scale.x=.01f*op->ol_ufireworkscale;
				op->ol_ubursta->scale.y=.01f*op->ol_ufireworkscale;
				op->ol_ubursta->scale.z=.01f*op->ol_ufireworkscale;
				op->ol_uburstb->scale.x=.01f*op->ol_ufireworkscale;
				op->ol_uburstb->scale.y=.01f*op->ol_ufireworkscale;
				op->ol_uburstb->scale.z=.01f*op->ol_ufireworkscale;
				op->ol_ubursta->flags&=~TF_DONTDRAW;
				op->ol_uburstb->flags&=~TF_DONTDRAW;
#endif
#if 1
//			} else {
//			}
//		if (op->ol_burstframe==od.ol_bursttsp[0]->nframe) {
		if (op->ol_burstframe==12) {
			op->ol_ufireworkscale-=.1f;
			op->ol_burstframe=0;
			op->ol_nubursts--;
/*			if (op->ol_nubursts==0) {
				for (k=0;k<4;k++) {
					unhooktree(op->ol_ubursta[k]);
					unhooktree(op->ol_uburstb[k]);
				}
			} */
		}
#endif
	} else {
//		op->ol_ubursta->flags|=TF_DONTDRAW;
//		op->ol_uburstb->flags|=TF_DONTDRAW;
	}
}

/*static int ol_calcstring(char *string)
{
	unsigned int i;
	int sizex=0;

	for(i=0;i<strlen(string);i++) sizex+=od.ol_tscorefont1->frames[i]->x;

	return sizex;
}
*/

/*static void ol_calcscore()
{
	int temp2,temp,tb;
	char msg[256];
	int offy;
	int coinbonus,timebonus,trickbonus,totalscore,trackbonus;
	tb = 10;

	setfontopt(0);
	coinbonus = od.ol_ncoinscaught * 10;
    if(od.ol_ncoinscaught == 10) coinbonus *= 2;

	temp = 1000 * (((od.ol_ustarttime*30+29)*2-of->clocktickcount)/2);
	temp = (int)(temp/30.0f);

	temp2 = od.ol_timetocomplete-temp;
	if(temp2<0) tb = 3;

	timebonus = temp2/1000;
	trackbonus = od.ol_trackscore * 3;
	trickbonus = of->ol_utotalstunts * 2;
	totalscore = trickbonus + trackbonus + (timebonus*tb) + coinbonus;

	video_lock();
		offy = (globalyres/2) - (6*(od.ol_tscorefont->frames[0]->y+5));

		sprintf(msg,"COINS BONUS %4d",coinbonus);
		ol_drawstring(msg,(globalxres/2)-(ol_calcstring(msg)/2),offy);
		offy+=od.ol_tscorefont->y+5;

		sprintf(msg,"TIME BONUS %4d",timebonus*tb);

		ol_drawstring(msg,(globalxres/2)-(ol_calcstring(msg)/2),offy);
		offy+=od.ol_tscorefont->y+5;

		sprintf(msg,"TRACK BONUS %4d",trackbonus);
		ol_drawstring(msg,(globalxres/2)-(ol_calcstring(msg)/2),offy);
		offy+=od.ol_tscorefont->y+5;

		sprintf(msg,"TRICK BONUS %4d",trickbonus);
		ol_drawstring(msg,(globalxres/2)-(ol_calcstring(msg)/2),offy);
		offy+=od.ol_tscorefont->y+5;

		strcpy(msg,"             ----");
		ol_drawstring(msg,(globalxres/2)-((strlen(msg)*od.ol_tscorefont->frames[0]->x)/2),offy);
		offy+=od.ol_tscorefont->y+5;

		sprintf(msg,"              %4d",totalscore);
		ol_drawstring(msg,(globalxres/2)-((strlen(msg)*od.ol_tscorefont->frames[0]->x)/2),offy);
		offy+=od.ol_tscorefont->y+5;
	video_unlock();

}
*/

// leader and biglead not timewarp safe
void procaplayer(int i,int leader,int biglead)
{
	int j;
	//	varchecker();
	//			logger("player %d, clock %d, (%f %f %f)\n",i,op->ol_clocktickcount,
	//				op->pos.x,op->pos.y,op->pos.z);
//	perf_start(PROCAPLAYER);
	op=&opa[i];
//	logger("in procaplayer with clock %6d, itw %d pred %d\n",op->clocktickcount,od.intimewarp,od.predicted);
/*	if (op==of && inlogmode) {
		logger("in procaplayer %d %d %d\n",i,leader,biglead);
	} */
/*	if (!od.predicted) {
		logger("----- procaplayer %d, tick %d\n",i,op->clocktickcount);
		logger("   pos %f %f %f\n",op->pos.x,op->pos.y,op->pos.z);
		logger("   curweap %d, weapframe %f\n",op->curweap,op->weapframe);

		for (j=0;j<NMISSILES;j++) {
			if (op->missileinfos[j].active) {
				logger("      miss %d, pos %f %f %f, vel %f %f %f, rot %f %f %f %f, fuel %d\n",
					j,
					op->missileinfos[j].pos.x,
					op->missileinfos[j].pos.y,
					op->missileinfos[j].pos.z,
					op->missileinfos[j].vel.x,
					op->missileinfos[j].vel.y,
					op->missileinfos[j].vel.z,
					op->missileinfos[j].rot.x,
					op->missileinfos[j].rot.y,
					op->missileinfos[j].rot.z,
					op->missileinfos[j].fuel);
			}
		}
	} */
// charge up energy, in timewarp
	for (j=0;j<MAXENERGIES;++j) {
		if (op->tenergies[j]<8) {
			int per=12-op->charges[j];
			if (per<2)
				per=2;
			if (op->clocktickcount>INITCANTSTARTDRIVE) {
				int co=op->clocktickcount-INITCANTSTARTDRIVE; // >0
				if ((co%(per*60)) == 0)
					++op->tenergies[j];
			}
		}
	}
	op->ol_ulastpos=op->pos;
	if (of==op) {
		od.ol_useoldcam++;
		if (od.ol_useoldcam>3) {
			od.ol_useoldcam=3;
			od.ol_testnewcam=0;
		} else
			od.ol_testnewcam=1;
	}
	if (op->ol_intunnels)
		op->ol_intunnels--;

//	if (op->ol_dofinish)
//		op->ol_dofinish++;

/*	if (of->ol_dofinish>=1000)
		if (ol_opt.backtoopt)
			changestate(1);
		else
			changestate(-1); */
		//popstate(); //changestate(FINISHSTATE);

//	ol_calccollisioninfo(); // bbox , points, moment of interia, new loc
	for (j=i+1;j<od.numcars;j++)
		checkcar2car(op,&opa[j]);
	ol_drivecar(i==leader ? biglead : 0);
	// collide objects
	if (op->ol_airtime>=0)
		op->ol_airtime++;
	if (op->ol_cbairtime>=0)
		op->ol_cbairtime++;
	if (op->ol_ufliptime>=0)
		op->ol_ufliptime++;
//	if (op->ol_cantstartdrive>0)
//		op->ol_cantstartdrive--;
	if (op==of) {
		if (of->clocktickcount==0) {
//			od.showcon=0;
			changelightstate(1);
		}
		if (of->clocktickcount==INITCANTSTARTDRIVE*1/4)
			changelightstate(2);
		if (of->clocktickcount==INITCANTSTARTDRIVE*2/4)
			changelightstate(3);
		if (of->clocktickcount==INITCANTSTARTDRIVE*3/4)
			changelightstate(4);
		if (of->clocktickcount==INITCANTSTARTDRIVE)
			changelightstate(5);
		if (op->clocktickcount==INITCANTSTARTDRIVE*5/4) {
			od.rematchplayers=0; // safety
			changelightstate(6);
		}
	}
	if (op->ol_doacrashreset)
		op->ol_doacrashreset++;
//	ol_calccollisioninfo(); // bbox , points, moment of interia, was here
	//find intersection, calc impulse, adjust trans and rot velocities
	//perf_start(DOROADCOLLISIONS);
}

const C8* medalnames[MED_MAXMEDALS]={
	"1st place medal earned!",
	"Max damage medal earned!",
	"Caught all coins medal earned!",
	"Wrong way 2nd place medal earned!",
	"1st place medal noweaps earned!",
	"Caught all trophies medal earned!",
	"Beat that medal earned!",
	"Beat that medal 2 earned!",
};
void addmedal(S32 cs,S32 medkind)
{
	if (od.playerid==opa[cs].carid) {
		if (!od.medalshave[medkind]) {
			GAMENEWS->printf(medalnames[medkind]);
			opa[cs].medals[medkind]=1;
			od.medalshave[medkind]=1;
		}
	}
}

void procaplayer2(int leader,int biglead)
{
	pointf3 top,bot,intsect;
	int k;
//	if (!od.intimewarp) {
		st2_enablesurffunc();
		top=bot=op->pos;
		top.y+=2;
//		bot.y=0; // this was stupid
		st2_line2road(&top,&bot,&intsect,NULL);
//	}
	ol_doroadcollisions(); // obj to road, will zero airtime if car is in contact with ground
//	if (!od.intimewarp)
		st2_callsurffunc();
	//perf_end(DOROADCOLLISIONS);
	// extra forces
	if (op->ol_cantdrive>0)
		op->ol_cantdrive--;
	if (op->ol_airtime>od.ol_stunttime || od.ol_manualcar) {
		op->ol_accelspin=0;
		if (!op->ol_flymode)
			op->ol_flymode=1;
		op->ol_cbairtime=op->ol_airtime;
		if (op->wheelvel[0]>0)
			for (k=0;k<4;k++)
				op->wheelvel[k]-=1;
		else
			for (k=0;k<6;k++)
				op->wheelvel[k]+=1;
	} else { // on ground
		op->ol_ucarrotvel=zerov;
		op->ol_littleg=op->ol_littlegground;
		if (op->ol_airtime==-1) {
			op->ol_ttp=0; // you lose the stunts
			op->ol_doacrashreset=1;
			op->ol_airtime=0;
//			logger("car reset: crashed1\n");
		}
		if (op->ol_flymode && !op->ol_doacrashreset) { // just landed
//			resetcamera();
			op->ol_cantdrive=0;
			if(op->ol_yawpiesum>=2)
				op->ol_ttp++;
			if (op->ol_ttp==0) {
				ol_playatagsound(14,op->vold,op->pan); // land
			} else {
				if (op->ol_ttp>1) {
					ol_doblueflash(15);
					ol_dolightning(8);
					ol_douflames(4);
				}
				ol_douburst(op->ol_ttp);
			}
			if (!op->dofinish) {
				op->ol_utotalstunts+=op->ol_ttp;
				op->ol_carboost=op->ol_ttp;
			}
			if (op->ol_ttp && op==of && !op->dofinish) { // && !od.pingmode) {
				od.tscoret[PTEXT_TRICKS]=100;
				od.cscoret[PTEXT_TRICKS]=0;
				if (op->ol_ttp==1)
					sprintf(od.scorestrings[PTEXT_TRICKS],"           1 TRICK");
				else
					sprintf(od.scorestrings[PTEXT_TRICKS],"           %2d TRICKS",op->ol_ttp);
			}
			op->ol_ttp=0;
		}
		op->ol_flymode=0;
	}
	if (!od.ol_manualcar) {
		// move objects
		ol_updatetrans();

	} else {
		op->carvel=zerov;
	}
	ol_updaterots(); // but i'll do the stunt!
//	if (!op->dofinish && !od.ol_notimelimit && !op->ol_cantstartdrive)
//nothing changed
	op->clocktickcount++;//=od.ol_uloop;
/*	if (op->ol_clocktickcount<0) {
		if (!op->dofinish) {
			ol_playatagsound(39,of->vold,op->pan); // horn
			op->dofinish=1;
		}
		op->ol_clocktickcount=0;
	}*/
//	ol_coincollisions();

//nothing changed
	//ol_flagcollision();
// see if we crossed the finish line..
//#if 0
//JAY MESS1
	if (!od.predicted) { // with absolute certainty
//		if (op==of)
//			GAMENEWS->printf("clk %d, wrongway2 %d",op->clocktickcount,op->wrongway2);
	}
	if (currule->ctf) {
		if (!op->dofinish /*&& !od.predicted */) {
			if (!op->finplace) { // close to finishline
				pointf3 del,normz={0,0,1,0};
	//			quatrot(&od.ol_startpiece->rot,&normz,&normz);
	//			del.x=op->pos.x-od.ol_startpiece->trans.x;
	//			del.y=op->pos.y-od.ol_startpiece->trans.y;
	//			del.z=op->pos.z-od.ol_startpiece->trans.z;
				quatrot(&od.ol_finishpiece->rot,&normz,&normz);
				del.x=op->pos.x-od.ol_finishpiece->trans.x;
				del.y=op->pos.y-od.ol_finishpiece->trans.y;
				del.z=op->pos.z-od.ol_finishpiece->trans.z;
				//if (op->chklap>=od.laps) {
				if (od.ol_flagidx < 0) {
	//			if (dot3d(&normz,&del)>0) {
	//				op->lapsdone++;
	//				op->lapready=0;
	//				if (op->lapsdone==od.laps) {
					if (!od.predicted) {
						od.fplace++;
						op->finplace=od.fplace;
						if (op->finplace>=OL_MAXPLAYERS)
							op->finplace=OL_MAXPLAYERS;
						if (od.fplace==1) {
							if (op==of)
								ol_playatagsound(34,1,0);
							ol_douburst(30);
							ol_douflames(30);
							ol_doblueflash(30);
							ol_dolightning(30);
							ol_docondom(30);
						} else if (od.fplace==2) {
							if (op==of)
								ol_playatagsound(33,1,0);
							ol_douburst(10);
							ol_douflames(10);
							ol_doblueflash(10);
							ol_dolightning(10);
							ol_docondom(10);
						} else {
							ol_douburst(2);
							ol_douflames(2);
							ol_doblueflash(2);
							ol_dolightning(2);
							ol_docondom(2);
						}
					}
					op->dofinish=op->ol_numflags + 1;//op->clocktickcount;
	//				}
	//					char str[50];
	//					int tt=of->clocktickcount-INITCANTSTARTDRIVE; // time from start to finish
					if (op->dofinish && !od.predicted) {
						if (op==of) {
							mainstubinfo.finished=1; // no more stub fail
							if (!mainstubinfo.place)
								mainstubinfo.place=op->finplace;
						}
						logger("ALL FLAGS CAUGHT! : car %d, clocktickcount %d !\n",op->carid,op->clocktickcount);
						olcalcscore(op==of);
					}
	//					od.tscoret[PTEXT_FINISHTIME]=10000;
	//					else
	//						od.tscoret[0]=200;
	//					od.cscoret[PTEXT_FINISHTIME]=0;
	//					getsctime(tt,str);
	//					sprintf(od.scorestrings[PTEXT_FINISHTIME],"       %s",str);
	//					od.tscoret[2]=200;
	//					od.cscoret[2]=0;
	//					if (op->lapsdone+1==od.laps)
	//						sprintf(od.scorestrings[2]," LAST LAP");
	//					else if (op->lapsdone+1<od.laps)
	//						sprintf(od.scorestrings[2],"    LAP %d",op->lapsdone+1);
	//					else
	//						od.tscoret[2]=0;
				}
			}
			if (op->dofinish)// && !od.predicted)
				ol_playatagsound(ZING0150,op->vold,op->pan); // finishsound
		}
	} else {	//#endif
		if (!op->dofinish /*&& !od.predicted */) {
			 if (!op->finplace) { // close to finishline
				pointf3 del,normz={0,0,1,0};
	//			quatrot(&od.ol_startpiece->rot,&normz,&normz);
	//			del.x=op->pos.x-od.ol_startpiece->trans.x;
	//			del.y=op->pos.y-od.ol_startpiece->trans.y;
	//			del.z=op->pos.z-od.ol_startpiece->trans.z;
				quatrot(&od.ol_finishpiece->rot,&normz,&normz);
				del.x=op->pos.x-od.ol_finishpiece->trans.x;
				del.y=op->pos.y-od.ol_finishpiece->trans.y;
				del.z=op->pos.z-od.ol_finishpiece->trans.z;
//				if (0) { // never finish
//				if (1) { // force a finish immediatly
//				if (op->clocktickcount-INITCANTSTARTDRIVE>=(op->carid+1)*120) { // force a finish real soon
				if (op->chklap>=od.laps) { // finish when done the laps, normal
	//			if (dot3d(&normz,&del)>0) {
	//				op->lapsdone++;
	//				op->lapready=0;
	//				if (op->lapsdone==od.laps) {
					if (!od.predicted) {
						od.fplace++; // it's official
//						if (od
						op->finplace=od.fplace;
						if (op->finplace>=OL_MAXPLAYERS)
							op->finplace=OL_MAXPLAYERS;
						if (od.fplace==1) {
							if (op==of)
								ol_playatagsound(34,1,0);
							ol_douburst(30);
							ol_douflames(30);
							ol_doblueflash(30);
							ol_dolightning(30);
							ol_docondom(30);
						} else if (od.fplace==2) {
							if (op==of)
								ol_playatagsound(33,1,0);
							ol_douburst(10);
							ol_douflames(10);
							ol_doblueflash(10);
							ol_dolightning(10);
							ol_docondom(10);
						} else {
							ol_douburst(2);
							ol_douflames(2);
							ol_doblueflash(2);
							ol_dolightning(2);
							ol_docondom(2);
						}
					}
					op->dofinish=op->clocktickcount;
	//				}
	//					char str[50];
	//					int tt=of->clocktickcount-INITCANTSTARTDRIVE; // time from start to finish
					if (op->dofinish && !od.predicted) { // this is the place to post official race results, out of time warp
						if (op==of) { // remember, '[' and ']' work in debug mode only..
							mainstubinfo.finished=1; // no more stub fail
							if (!mainstubinfo.place)
								mainstubinfo.place=op->finplace;
						}
						logger("FINISHED! : car %d, clocktickcount %d !\n",op->carid,op->clocktickcount);
						if (op->finplace==2 && op->wrongway2>5) {
//							GAMENEWS->printf("wrong way, 2nd place medal earned!");
//							op->medals[MED_WRONGWAY2]=1;
							addmedal(op->carid,MED_WRONGWAY2);
						}
						if (op->finplace==1) {
//							GAMENEWS->printf("1st place medal earned!");
//							op->medals[MED_FIRST]=1;
							addmedal(op->carid,MED_FIRST);
						}
						if (op->finplace==1 && op->woc==-1) {
//							GAMENEWS->printf("1st place medal, noweaps earned!");
//							op->medals[MED_FIRSTNOWEAP]=1;
							addmedal(op->carid,MED_FIRSTNOWEAP);
						}
						olcalcscore(op==of);
// only od.carid==op->carid can post, less secure, but easier to implement medals earned
						if (ol_opt.nt==NET_BCLIENT && od.playerid==op->carid && !od.playernet[op->carid].onebotplayer) {
// anybody can post
//						if (ol_opt.nt==NET_BCLIENT && op->carid<od.numcars-od.numbots && !od.playernet[op->carid].onebotplayer) {
							S32 toksend[6+MED_MAXMEDALS];
							toksend[0]=op->carid; // slot number
							toksend[1]=(op->clocktickcount-INITCANTSTARTDRIVE)*5/3; // race time, convert to centisec
							toksend[2]=op->finplace; // 1,2,3 etc.
							toksend[3]=inverseweapxlate(op->woc); // weapon of choice
							toksend[4]=carstubinfos[op->carid].score; // score
							toksend[5]=carstubinfos[op->carid].experience; // exp
							S32 q;
							for (q=0;q<MED_MAXMEDALS;++q) // send some medals too
								toksend[6+q]=op->medals[q];
							tcpwritetoken8(od.playernet[0].tcpclients,PKT_RESULT,(C8*)toksend,sizeof(toksend));
						}
					}
	//					od.tscoret[PTEXT_FINISHTIME]=10000;
	//					else
	//						od.tscoret[0]=200;
	//					od.cscoret[PTEXT_FINISHTIME]=0;
	//					getsctime(tt,str);
	//					sprintf(od.scorestrings[PTEXT_FINISHTIME],"       %s",str);
	//					od.tscoret[2]=200;
	//					od.cscoret[2]=0;
	//					if (op->lapsdone+1==od.laps)
	//						sprintf(od.scorestrings[2]," LAST LAP");
	//					else if (op->lapsdone+1<od.laps)
	//						sprintf(od.scorestrings[2],"    LAP %d",op->lapsdone+1);
	//					else
	//						od.tscoret[2]=0;
				}
			}
			if (op->dofinish)// && !od.predicted)
				ol_playatagsound(ZING0150,op->vold,op->pan); // finishsound
		}
	}
//	perf_end(PROCAPLAYER);
//JAY END MESS1
//#endif
}

// global projectile procs
void initmissiles(struct weapfly *wf)
{
	wf->speed=od.missilevel;
	wf->scale=pointf3x(3,3,3);
}

void procmissiles(struct weapfly *wf)
{
	int j;
	pointf3 mvel,top,bot,fpos;
	float f;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
		}
	j=wf->hitcar;
	if (j>=0) {
		fakepos(j,&fpos);
		top.x=fpos.x-wf->pos.x;
		top.y=fpos.y-wf->pos.y;
		top.z=fpos.z-wf->pos.z;
		f=normalize3d(&top,&top);
		if (f) {
			bot=pointf3x(0,0,1);
			quatrot(&wf->rot,&bot,&bot);
			cross3d(&bot,&top,&top);
			top.w=od.missileturnangle*(PI/180.0f);
			rotaxis2quat(&top,&top);
			quattimes(&top,&wf->rot,&wf->rot);
			quatnormalize(&wf->rot,&wf->rot);
		}
	}
	f=wf->speed*od.ol_timeinc;
	top=pointf3x(0,0,f);
	quatrot(&wf->rot,&top,&mvel);
	bot=wf->pos;
	wf->pos.x+=mvel.x;
	wf->pos.y+=mvel.y;
	wf->pos.z+=mvel.z;
	if (line2box(&fpos,&opa[j].carang,&opa[j].ol_carbboxmin,&opa[j].ol_carbboxmax,&bot,&wf->pos,&top)) {
		f=dist3d(&od.camposdp,&top)*(1/4.0f);
		if (f<od.voldist)
			f=1;
		f=od.voldist/f;
		if (opa[j].extrashieldsup) {
			shieldhit(WEAP_MISSILES,j,f);
		} else if (ncollinfo<MAXCOLLINFO) {
			bot=pointf3x(1,0,0);
			spawnc2ceffect(&top,&bot,3,&zerov);
			bot=pointf3x(0,1,0);
			spawnc2ceffect(&top,&bot,3,&zerov);
			bot=pointf3x(0,0,1);
			spawnc2ceffect(&top,&bot,3,&zerov);
			ol_playatagsound(18,f,0);
			bot.x=mvel.x*od.missileimpactforce;
			bot.y=mvel.y*od.missileimpactforce;
			bot.z=mvel.z*od.missileimpactforce;
			bot.y+=od.missileupforce;
			collinfos[ncollinfo].impval=bot;
			top.x-=opa[j].pos.x;
			top.y-=opa[j].pos.y;
			top.z-=opa[j].pos.z;
			collinfos[ncollinfo].imppnt=top;
			collinfos[ncollinfo].cn=j;
			ncollinfo++;
		}
		freeprojectile(wf);
		return;
	}
	if (st2_line2road(&bot,&wf->pos,&top,NULL)) {
		bot=pointf3x(1,0,0);
		spawnc2ceffect(&top,&bot,3,&zerov);
		bot=pointf3x(0,1,0);
		spawnc2ceffect(&top,&bot,3,&zerov);
		bot=pointf3x(0,0,1);
		spawnc2ceffect(&top,&bot,3,&zerov);
		f=dist3d(&od.camposdp,&top)*(1/4.0f);
		if (f<od.voldist)
			f=1;
		f=od.voldist/f;
		ol_playatagsound(ZING0150,f,0);
		freeprojectile(wf);
		return;
	}
	if (!od.intimewarp) {
//	if ((wf->fuel&1)==0 && !od.intimewarp) {
		spawnmtrail(&wf->pos);
//		spawnmtrail(&wf->pos);
	}
}

enum {ECTO_FALL,ECTO_LAND,ECTO_DISABLED,ECTO_HIT0};
void initecto(struct weapfly *wf)
{
	wf->speed=1;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(od.ectoscale,od.ectoscale,od.ectoscale);
//	setVEC(&wf->scale,.45f,.45f,.45f);
	wf->fuel=od.ectostartfuel;
/*	if (wf->fromcar>=0) {
		wf->vel=od.ectocvel;
		wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
		wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
		wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
	} else */
		wf->vel=od.ectofvel;
	wf->vel.x+=od.ectorvel.x*(pfrand1(opa[0].clocktickcount)-.5f);
	wf->vel.y+=od.ectorvel.y*(pfrand2(opa[0].clocktickcount)-.5f);
	wf->vel.z+=od.ectorvel.z*(pfrand3(opa[0].clocktickcount)-.5f);
}

void procecto(struct weapfly *wf)
{
	int i;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
		}
	if (!wf->flags) { // fall to road
		pointf3 pos,intsect;
		pos=wf->pos;
		wf->pos.x+=wf->vel.x;
		wf->pos.y+=wf->vel.y;
		wf->pos.z+=wf->vel.z;
		wf->vel.y+=od.ectograv;
		if (st2_line2road(&pos,&wf->pos,&intsect,NULL)) {
			wf->flags=ECTO_LAND;
			wf->vel=zerov;
			wf->pos=intsect;
		}
	}
	if (wf->flags==ECTO_LAND)
		for (i=0;i<od.numcars;i++)
			if (/*wf->hitcar!=i && */dist3dsq(&opa[i].pos,&wf->pos)<od.ectorad*od.ectorad) {
				wf->flags=ECTO_HIT0+i;	// attach to a car
//				wf->kind=DETACH_ECTO2;
				wf->fuel=10;
			}
	if (wf->flags>=ECTO_HIT0) {
		int k=wf->flags-ECTO_HIT0;
		if (opa[k].extrashieldsup) {
			float f;
			f=dist3d(&od.camposdp,&opa[k].pos)*(1/4.0f);
			if (f<od.voldist)
				f=1;
			f=od.voldist/f;
			shieldhit(WEAP_ECTO,k,f);
			wf->flags=ECTO_DISABLED;
		} else
			opa[k].vinecto=3;
	}
	if (wf->flags>=ECTO_DISABLED) {
//		wf->pos=opa[k].pos;
		wf->scale.x+=.05f;
		wf->scale.y+=.05f;
		wf->scale.z+=.05f;
//		setVEC(&wf->scale,.3f,.3f,.3f);
	}
	if ((wf->fuel&8)!=0) {
//		logger("ecto scale flatten %f %f %f clocktick %d\n",wf->scale.x,wf->scale.y,wf->scale.z,op->clocktickcount);
		wf->scale.x+=.005f;
		wf->scale.y-=.005f;
		wf->scale.z+=.005f;
	} else {
//		logger("ecto scale tallen %f %f %f clocktick %d\n",wf->scale.x,wf->scale.y,wf->scale.z,op->clocktickcount);
		wf->scale.x-=.005f;
		wf->scale.y+=.005f;
		wf->scale.z-=.005f;
	}
}

void initlaser(struct weapfly *wf)
{
	wf->speed=od.laservel;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(.3f,.3f,.3f);
}

static void laserreflect1(pointf3* i,pointf3* n,pointf3* o)
{
	o->x=-i->x;
	o->y=-i->y;
	o->z=-i->z;
}

static void laserreflect2(pointf3* i,pointf3* n,pointf3* o)
{
	float d=2*dot3d(i,n);
	o->x=i->x-n->x*d;
	o->y=i->y-n->y*d;
	o->z=i->z-n->z*d;
}

void proclaser(struct weapfly *wf)
{
	int i,j;
	pointf3 top,bot,norm,invel,outvel,ti;
	float f;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
	}
	i=wf->hitcar;
/*	if (j>=0) {
		top.x=opa[j].pos.x-wf->pos.x;
		top.y=opa[j].pos.y-wf->pos.y;
		top.z=opa[j].pos.z-wf->pos.z;
		f=normalize3d(&top,&top);
		if (f) {
			setVEC(&bot,0,0,1);
			quatrot(&wf->rot,&bot,&bot);
			cross3d(&bot,&top,&top);
			top.w=od.laserturnangle*(PI/180.0f);
			rotaxis2quat(&top,&top);
			quattimes(&top,&wf->rot,&wf->rot);
			quatnormalize(&wf->rot,&wf->rot);
		}
	} */
	f=wf->speed*od.ol_timeinc;
	top=pointf3x(0,0,f);
//	wf->rot.x=0;
//	wf->rot.y=1;
//	wf->rot.z=0;
//	wf->rot.w=0;
	quatrot(&wf->rot,&top,&invel);
	bot=wf->pos;
	wf->pos.x+=invel.x;
	wf->pos.y+=invel.y;
	wf->pos.z+=invel.z;
	for (j=0;j<od.numcars;j++) {
		if (i!=j && line2box(&opa[j].pos,&opa[j].carang,&opa[j].ol_carbboxmin,&opa[j].ol_carbboxmax,&bot,&wf->pos,&top)) {
			f=dist3d(&od.camposdp,&top)*(1/4.0f);
			if (f<od.voldist)
				f=1;
			f=od.voldist/f;
			if (opa[j].extrashieldsup) {
				shieldhit(WEAP_LASER,j,f);
			} else if (ncollinfo<MAXCOLLINFO) {
				bot=pointf3x(1,0,0);
				spawnc2ceffect(&top,&bot,3,&zerov);
				bot=pointf3x(0,1,0);
				spawnc2ceffect(&top,&bot,3,&zerov);
				bot=pointf3x(0,0,1);
				spawnc2ceffect(&top,&bot,3,&zerov);
				ol_playatagsound(18,f,0);
				bot.x=invel.x*od.laserimpactforce;
				bot.y=invel.y*od.laserimpactforce;
				bot.z=invel.z*od.laserimpactforce;
				bot.y+=od.laserupforce;
				collinfos[ncollinfo].impval=bot;
				top.x-=opa[j].pos.x;
				top.y-=opa[j].pos.y;
				top.z-=opa[j].pos.z;
				collinfos[ncollinfo].imppnt=top;
				collinfos[ncollinfo].cn=j;
				ncollinfo++;
			}
			freeprojectile(wf);
			return;
		}
	}
	if (st2_line2road(&bot,&wf->pos,&top,&norm)) {
//		setVEC(&bot,1,0,0);
		spawnc2ceffect(&top,&norm,3,&zerov);
//		setVEC(&bot,0,1,0);
//		spawnc2ceffect(&top,&bot,3,&zerov);
//		setVEC(&bot,0,0,1);
//		spawnc2ceffect(&top,&bot,3,&zerov);
		top.w=0;
		wf->pos=top;
		if (prandom3(opa[0].clocktickcount,2))
			laserreflect1(&invel,&norm,&outvel);
		else
			laserreflect2(&invel,&norm,&outvel);
		normalize3d(&outvel,&outvel);
		wf->pos.x+=outvel.x*od.laserextrabounce;
		wf->pos.y+=outvel.y*od.laserextrabounce;
		wf->pos.z+=outvel.z*od.laserextrabounce;
//		logger("laser out vel %f %f %f\n",outvel.x,outvel.y,outvel.z);
		ti.x=0;
		ti.y=0;
		ti.z=1;
//		logger("laser quat in %f %f %f %f\n",wf->rot.x,wf->rot.y,wf->rot.z,wf->rot.w);
		cross3d(&ti,&outvel,&wf->rot);
		normalize3d(&wf->rot,&wf->rot);
		wf->rot.w=racos(dot3d(&ti,&outvel));
//		logger("laser rotaxis %f %f %f %f\n",wf->rot.x,wf->rot.y,wf->rot.z,wf->rot.w);
		rotaxis2quat(&wf->rot,&wf->rot);
//		logger("laser quat out %f %f %f %f\n",wf->rot.x,wf->rot.y,wf->rot.z,wf->rot.w);
		f=dist3d(&od.camposdp,&top)*(1/4.0f);
		if (f<od.voldist)
			f=1;
		f=od.voldist/f;
		ol_playatagsound(ZING0150,f,0);
//		freeprojectile(wf);
		return;
	}
}

void initemb(struct weapfly *wf)
{
	wf->speed=od.embvel;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(.3f,.3f,.3f);
}

void procemb(struct weapfly *wf)
{
	int j;
	pointf3 mvel,top,bot,fpos;
	float f;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
	}
	j=wf->hitcar;
	if (j>=0) {
		fakepos(j,&fpos);
		top.x=fpos.x-wf->pos.x;
		top.y=fpos.y-wf->pos.y;
		top.z=fpos.z-wf->pos.z;
		f=normalize3d(&top,&top);
		if (f) {
			bot=pointf3x(0,0,1);
			quatrot(&wf->rot,&bot,&bot);
			cross3d(&bot,&top,&top);
			top.w=od.embturnangle*(PI/180.0f);
			rotaxis2quat(&top,&top);
			quattimes(&top,&wf->rot,&wf->rot);
			quatnormalize(&wf->rot,&wf->rot);
		}
	}
	f=wf->speed*od.ol_timeinc;
	top=pointf3x(0,0,f);
	quatrot(&wf->rot,&top,&mvel);
	bot=wf->pos;
	wf->pos.x+=mvel.x;
	wf->pos.y+=mvel.y;
	wf->pos.z+=mvel.z;
	if (line2box(&fpos,&opa[j].carang,&opa[j].ol_carbboxmin,&opa[j].ol_carbboxmax,&bot,&wf->pos,&top)) {
		f=dist3d(&od.camposdp,&top)*(1/4.0f);
		if (f<od.voldist)
			f=1;
		f=od.voldist/f;
		if (opa[j].extrashieldsup) {
			shieldhit(WEAP_EMB,j,f);
		} else if (ncollinfo<MAXCOLLINFO) {
			struct ol_playerdata *ops;
			bot=pointf3x(1,0,0);
			spawnc2ceffect(&top,&bot,3,&zerov);
			bot=pointf3x(0,1,0);
			spawnc2ceffect(&top,&bot,3,&zerov);
			bot=pointf3x(0,0,1);
			spawnc2ceffect(&top,&bot,3,&zerov);
			ol_playatagsound(18,f,0);
			bot.x=mvel.x*od.embimpactforce;
			bot.y=mvel.y*od.embimpactforce;
			bot.z=mvel.z*od.embimpactforce;
			collinfos[ncollinfo].impval=bot;
			top.x-=opa[j].pos.x;
			top.y-=opa[j].pos.y;
			top.z-=opa[j].pos.z;
			collinfos[ncollinfo].imppnt=top;
			collinfos[ncollinfo].cn=j;
			ncollinfo++;
			ops=op;
			op=&opa[j];
//			ol_dolightning(30);
			op->vinemb+=45;
			op=ops;
		}
		freeprojectile(wf);
		return;
	}
	if (st2_line2road(&bot,&wf->pos,&top,NULL)) {
		bot=pointf3x(1,0,0);
		spawnc2ceffect(&top,&bot,3,&zerov);
		bot=pointf3x(0,1,0);
		spawnc2ceffect(&top,&bot,3,&zerov);
		bot=pointf3x(0,0,1);
		spawnc2ceffect(&top,&bot,3,&zerov);
		f=dist3d(&od.camposdp,&top)*(1/4.0f);
		if (f<od.voldist)
			f=1;
		f=od.voldist/f;
		ol_playatagsound(ZING0150,f,0);
		freeprojectile(wf);
		return;
	}
}

void initoil(struct weapfly *wf)
{
	wf->speed=od.oilvel;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(1,.1f,1);
}

void procoil(struct weapfly *wf)
{
	int i;
	pointf3 ti;
	pointf3 top,bot,intsect,norm;
	wf->fuel--;
	wf->pos.y+=wf->speed;
	top=wf->pos;
	bot=wf->pos;
	top.y+=od.oilymax;
	bot.y+=od.oilymin;
	if (wf->speed && st2_line2roadlo(&top,&bot,&intsect,&norm)) {
		wf->speed=0;
		ti=pointf3x(0,1,0);
		normal2quat(&ti,&norm,&wf->rot);
	}
	if (wf->fuel==0 || wf->pos.y<od.lowpoint) {
		freeprojectile(wf);
		return;
	}
	for (i=0;i<od.numcars;i++)
		if (dist3dsq(&opa[i].pos,&wf->pos)<od.oildist*od.oildist)
			if (opa[i].xcurweapstate!=WEAPSTATE_ACTIVE ||
				(opa[i].xcurweapkind!=WEAP_SPIKES &&
				opa[i].xcurweapkind!=WEAP_BAINITE &&
				opa[i].xcurweapkind!=WEAP_BIGTIRES))
				opa[i].vinoil=30;
			else
				logger("car %d clk %d: oil countered by '%s'\n",i,opa[0].clocktickcount,weapinfos[opa[i].xcurweapkind].name);
}

void initmine(struct weapfly *wf)
{
	wf->speed=od.minevel;
//	wf->alwaysfacing=1;
//	setVEC(&wf->scale,1,.1f,1);
}

void procmine(struct weapfly *wf)
{
	int i;
	pointf3 top,bot,intsect;
//	wf->fuel--; // using fuel for weapboost
	wf->pos.y+=wf->speed;
	top=wf->pos;
	bot=wf->pos;
	top.y+=od.mineymax;
	bot.y+=od.mineymin;
	if (wf->speed && st2_line2roadlo(&top,&bot,&intsect,NULL))
		wf->speed=0;
	if (/*wf->fuel==0 || */wf->pos.y<od.lowpoint) {
		freeprojectile(wf);
		return;
	}
	if (wf->flags>=od.minearmtime) {
		if (wf->flags==od.minearmtime)
			od.minearmed=1;
		for (i=0;i<od.numcars;i++)
			if (dist3dsq(&opa[i].pos,&wf->pos)<od.minedist*od.minedist) {
				float f;
				f=dist3d(&od.camposdp,&top)*(1/4.0f);
				if (f<od.voldist)
					f=1;
				f=od.voldist/f;
				if (opa[i].extrashieldsup) {
					shieldhit(WEAP_MINES,i,f);
				} else if (ncollinfo<MAXCOLLINFO) {
					bot=pointf3x(1,0,0);
					spawnc2ceffect(&top,&bot,3,&zerov);
					bot=pointf3x(0,1,0);
					spawnc2ceffect(&top,&bot,3,&zerov);
					bot=pointf3x(0,0,1);
					spawnc2ceffect(&top,&bot,3,&zerov);
					ol_playatagsound(18,f,0);
					bot.x=bot.z=0;
					bot.y=od.mineupforce;
					if (wf->fuel) // weapboost
						bot.y+=od.mineupforce;
					collinfos[ncollinfo].impval=bot;
					top.x-=opa[i].pos.x;
					top.y-=opa[i].pos.y;
					top.z-=opa[i].pos.z;
					collinfos[ncollinfo].imppnt=top;
					collinfos[ncollinfo].cn=i;
					ncollinfo++;
				}
				freeprojectile(wf);
				return;
			}

	/*			if (opa[i].xcurweapstate!=WEAPSTATE_ACTIVE ||
					(opa[i].xcurweapkind!=WEAP_SPIKES && opa[i].xcurweapkind!=WEAP_BAINITE))
					opa[i].vinoil=30;
				else
					logger("car %d clk %d: oil countered by '%s'\n",i,opa[0].clocktickcount,weapinfos[opa[i].xcurweapkind].name);
	*/
//	} else {
	}
	wf->flags++;
}

//JAY
void initflag(struct weapfly *wf)
{
//	wf->speed=od.minevel;
//	wf->alwaysfacing=1;
//	setVEC(&wf->scale,1,.1f,1);
}

#if 1
void procflag(struct weapfly *wf)
{
	int i;
//	od.ol_flags[wf->fromcar].t->flags |= TF_DONTDRAW;
	if (wf->fromcar != od.ol_flagidx) //{
//		if ( od.ol_flags[wf->fromcar].t->parent ) //unhooktree(od.ol_flags[wf->fromcar].t);
//		od.ol_flags[wf->fromcar].t->flags |= TF_DONTDRAW;
		return;
//	if ( !od.ol_flags[wf->fromcar].t->parent ) //linkchildtoparent(od.ol_flags[wf->fromcar].t, od.ol_root);
	od.ol_flags[wf->fromcar].t->flags &= ~TF_DONTDRAW;
	if (wf->flags>=od.minearmtime) {
		for (i=0;i<od.numcars;i++) {
			if (dist3dsq(&opa[i].pos,&wf->pos)<FLAGCOLLECTDIST*FLAGCOLLECTDIST) {
				float f;
				f=dist3d(&od.camposdp,&wf->pos)*(1/4.0f);
				if (f<od.voldist)
					f=1;
				f=od.voldist/f;
				if (ncollinfo<MAXCOLLINFO) {
					pointf3 norm;
					norm=pointf3x(1,0,0);
					spawnc2ceffect(&wf->pos,&norm,3,&zerov);
					norm=pointf3x(0,1,0);
					spawnc2ceffect(&wf->pos,&norm,3,&zerov);
					norm=pointf3x(0,0,1);
					spawnc2ceffect(&wf->pos,&norm,3,&zerov);
					ol_playatagsound(18,f,0);
					collinfos[ncollinfo].impval=zerov;
					collinfos[ncollinfo].imppnt=wf->pos;
					collinfos[ncollinfo].cn=i;
					ncollinfo++;
				}
				opa[i].ol_numflags++;
//				unhooktree(od.ol_flags[wf->fromcar].t);
				od.ol_flagidx--;
				// fromcar is actually a flag index
				/*
				od.ol_flags[wf->fromcar].t->flags |= TLF_DONTDRAW;
				if ( wf->fromcar > 0 ) {
					wf->fromcar--;
					wf->pos = od.ol_flags[wf->fromcar].pos;
//				} else*/ //{
				od.ol_flags[wf->fromcar].t->flags |= TF_DONTDRAW;
				freeprojectile(wf);
//				}
//				return;
			}
		}
	} else
		wf->flags++;
}
#endif
void initghook(struct weapfly *wf)
{
//	wf->speed=od.grapplevel; //zerov;
//	wf->alwaysfacing=1;
//	setVEC(&wf->scale,.3f,.3f,.3f);
}

void procghook(struct weapfly *wf)
{
	pointf3 displace,dir,crs;
	pointf3 yup={0,1,0};
	float t,scale,dt;
	int to,from;
// move the object
	from=wf->fromcar;
	to=wf->hitcar;
	if (opa[from].xcurweapkind==WEAP_GRAPPLE && opa[from].xcurweapstate==WEAPSTATE_ACTIVE) {
		if (opa[from].curweapvar>GRAPPLETIME)
			t=1;
		else
			t=opa[from].curweapvar*(1/30.0f);
		interp3d(&opa[from].pos,&opa[to].pos,t,&displace);
		if (st2_line2road(&opa[from].pos,&displace,&crs,NULL)) { // crs as a dummy
			freeprojectile(wf);
			opa[from].curweapvar=1000; // turn off the weapon
			return;
		}
		displace.x-=opa[from].pos.x;
		displace.y-=opa[from].pos.y;
		displace.z-=opa[from].pos.z;
		wf->pos=opa[from].pos;
		scale=normalize3d(&displace,&dir);
		if (scale==0) {
			wf->scale=zerov;
			wf->rot=zerov;
			wf->rot.w=1;
		} else {
			cross3d(&yup,&dir,&crs);
			dt=dot3d(&yup,&dir);
			if (!normalize3d(&crs,&crs))
				crs=pointf3x(1,0,0);
			crs.w=racos(dt);
			rotaxis2quat(&crs,&wf->rot);
			wf->scale.y=scale;
		}
	} else {
		freeprojectile(wf);
	}
}

void initfusioncan(struct weapfly *wf)
{
	float f;
	pointf3 top;
	wf->speed=od.fusionvel; //zerov;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(0,0,0);
	f=wf->speed*od.ol_timeinc;
	top=pointf3x(0,0,f);
	quatrot(&wf->rot,&top,&wf->vel);

	wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
	wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
	wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
}

void procfusioncan(struct weapfly *wf)
{
	pointf3 top,bot;
	int i,bi=-1;
	float f;
	float bdist=1e20f;
	pointf3 bestintsect,norm,bestnorm;
// scale the object
	wf->scale.x+=od.fusionscalevel;
	if (wf->scale.x>od.fusionmaxscale)
		wf->scale.x=od.fusionmaxscale;
	wf->scale.y=wf->scale.z=wf->scale.x;
	wf->dissolve=(float)wf->fuel/od.fusionstartfuel+.25f;
// rotate the object
	f=normalize3d(&od.fusionrotvel,&top);
	if (f!=0) {
		top.w=f;
		rotaxis2quat(&top,&top);
		quattimes(&wf->rot,&top,&wf->rot);
	}
// move the object
	wf->vel.y-=od.fusiongrav;
	wf->pos.x+=wf->vel.x;
	wf->pos.y+=wf->vel.y;
	wf->pos.z+=wf->vel.z;
// check collisions
	for (i=0;i<12;i++) {
		bot.x=wf->pos.x+fusballcollverts[i].x*wf->scale.x;
		bot.y=wf->pos.y+fusballcollverts[i].y*wf->scale.x;
		bot.z=wf->pos.z+fusballcollverts[i].z*wf->scale.x;
		if (st2_line2road(&wf->pos,&bot,&bestintsect,&norm)) {
			if (dot3d(&norm,&wf->vel)<0) {
				f=dist3dsq(&bestintsect,&wf->pos);
				if (f<bdist) {
					bi=i;
					bdist=f;
					bestnorm=norm;
				}
			}
		}
	}
	if (bi>=0) {
		f=dot3d(&wf->vel,&norm);
		f*=1.9f; // elasticity 2.0 max
		wf->vel.x-=f*norm.x;
		wf->vel.y-=f*norm.y;
		wf->vel.z-=f*norm.z;
	}
// wf->pos is the new pos, bot is the old pos...
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
	}
// look around for other cars
	for (i=0;i<od.numcars;i++)
		if (i!=wf->fromcar && !opa[i].vinfusion &&
					dist3dsq(&opa[i].pos,&wf->pos)<
					wf->scale.x*wf->scale.x*od.fusionrad*od.fusionrad*
					od.fusiongrabrad*od.fusiongrabrad) {
			float f;
			f=dist3d(&od.camposdp,&opa[i].pos)*(1/4.0f);
			if (f<od.voldist)
				f=1;
			f=od.voldist/f;
			if (opa[i].extrashieldsup) {
				shieldhit(WEAP_FSNCAN,i,f);
			} else {
				opa[i].vinfusion=60;
				bot=pointf3x(1,0,0);
				spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
				bot=pointf3x(0,1,0);
				spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
				bot=pointf3x(0,0,1);
				spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
				ol_playatagsound(18,f,0);
			}
		}
}

void initthermocan(struct weapfly *wf)
{
	float f;
	pointf3 top;
	wf->speed=od.thermovel;//zerov;
//	wf->alwaysfacing=1;
//	setVEC(&wf->scale,.3f,.3f,.3f);
	wf->scale=pointf3x(0,0,0);
	f=wf->speed*od.ol_timeinc;
	top=pointf3x(0,0,f);
	quatrot(&wf->rot,&top,&wf->vel);
}

void procthermocan(struct weapfly *wf)
{
/*	pointf3 top,bot;
	int i;
	float f;
	wf->scale.x+=od.thermoscalevel;
	wf->scale.y=wf->scale.z=wf->scale.x;
	wf->dissolve=(float)wf->fuel/od.thermostartfuel+.25f;
// rotate the object
	f=normalize3d(&od.thermorotvel,&top);
	if (f!=0) {
		top.w=f;
		rotaxis2quat(&top,&top);
		quattimes(&wf->rot,&top,&wf->rot);
	}
// move the object
	wf->pos.x+=wf->vel.x;
	wf->pos.y+=wf->vel.y;
	wf->pos.z+=wf->vel.z;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
	}
// look around for other cars
	for (i=0;i<od.numcars;i++)
		if (i!=wf->fromcar && !opa[i].vinthermo && dist3dsq(&opa[i].pos,&wf->pos)<wf->scale.x*wf->scale.x*od.thermorad*od.thermorad) {
			float f;
			f=dist3d(&od.camposdp,&opa[i].pos)*(1/4.0f);
			if (f<od.voldist)
				f=1;
			f=od.voldist/f;
			if (opa[i].extrashieldsup) {
				shieldhit(WEAP_THERMCAN,i,f); // uncomment if you want to bring it back
			} else {
				opa[i].vinthermo=60;
				setVEC(&bot,1,0,0);
				spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
				setVEC(&bot,0,1,0);
				spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
				setVEC(&bot,0,0,1);
				spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
				ol_playatagsound(18,f,0);
			}
		}
*/
}

void initsonic(struct weapfly *wf)
{
	float f;
	pointf3 top;
	wf->speed=od.sonicvel;//zerov;
//	wf->alwaysfacing=1;
//	setVEC(&wf->scale,.3f,.3f,.3f);
	wf->scale=pointf3x(0,0,0);
	f=wf->speed*od.ol_timeinc;
	top=pointf3x(0,0,f);
	quatrot(&wf->rot,&top,&wf->vel);
	wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
	wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
	wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
}

void procsonic(struct weapfly *wf)
{
	pointf3 bot;
	int i;
	float r1sq,r2sq,dsq;
	pointf3 top;
	float f;
	wf->scale.x+=od.sonicscalevel;
	wf->scale.y=wf->scale.z=wf->scale.x;
	wf->dissolve=(float)wf->fuel/od.sonicstartfuel+.25f;
// rotate the object
	f=normalize3d(&od.sonicrotvel,&top);
	if (f!=0) {
		top.w=f;
		rotaxis2quat(&top,&top);
		quattimes(&wf->rot,&top,&wf->rot);
	}
// move the object
	wf->pos.x+=wf->vel.x;
	wf->pos.y+=wf->vel.y;
	wf->pos.z+=wf->vel.z;
// wf->pos is the new pos, bot is the old pos...
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
	}
// look around for other cars
	r1sq=wf->scale.x*wf->scale.x;
	r2sq=r1sq;
	r1sq*=od.sonicrad1*od.sonicrad1;
	r2sq*=od.sonicrad2*od.sonicrad2;
	for (i=0;i<od.numcars;i++) {
		if (i!=wf->fromcar && !opa[i].vinsonic) {
			float dx,dz;
			dx=opa[i].pos.x-wf->pos.x;
			dz=opa[i].pos.z-wf->pos.z;
			dsq=dx*dx+dz*dz;
			if (dsq>=r1sq && dsq<=r2sq && wf->pos.y+od.sonicheight> opa[i].pos.y && wf->pos.y-od.sonicheight<opa[i].pos.y) {
				float f;
				f=dist3d(&od.camposdp,&opa[i].pos)*(1/4.0f);
				if (f<od.voldist)
					f=1;
				f=od.voldist/f;
				if (opa[i].extrashieldsup) {
					shieldhit(WEAP_SONIC,i,f);
				} else {
					opa[i].vinsonic=30;
					bot=pointf3x(1,0,0);
					spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
					bot=pointf3x(0,1,0);
					spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
					bot=pointf3x(0,0,1);
					spawnc2ceffect(&opa[i].pos,&bot,9,&zerov);
					ol_playatagsound(18,f,0);
				}
			}
		}
	}
}

enum {GBOMB_FALL,GBOMB_LAND,GBOMB_DISABLED,GBOMB_HIT0};
void initgbomb(struct weapfly *wf)
{
	wf->speed=1;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(od.gbombscale,od.gbombscale,od.gbombscale);
//	setVEC(&wf->scale,.45f,.45f,.45f);
	wf->fuel=od.gbombstartfuel;
/*	if (wf->fromcar>=0) {
		wf->vel=od.ectocvel;
		wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
		wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
		wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
	} else */
		wf->vel=od.gbombfvel;
	wf->vel.x+=od.gbombrvel.x*(pfrand1(opa[0].clocktickcount)-.5f);
	wf->vel.y+=od.gbombrvel.y*(pfrand2(opa[0].clocktickcount)-.5f);
	wf->vel.z+=od.gbombrvel.z*(pfrand3(opa[0].clocktickcount)-.5f);
}

void procgbomb(struct weapfly *wf)
{
	int i;
	float r2,mr2;
	float d2,f;
	pointf3 fv,n,del;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
		}
	if (!wf->flags) { // fall to road
		pointf3 pos,intsect;
		pos=wf->pos;
		wf->pos.x+=wf->vel.x;
		wf->pos.y+=wf->vel.y;
		wf->pos.z+=wf->vel.z;
		wf->vel.y+=od.gbombgrav;
		if (st2_line2road(&pos,&wf->pos,&intsect,NULL)) {
			wf->flags=GBOMB_LAND;
			wf->vel=zerov;
			wf->pos=intsect;
		}
	}
	if (wf->flags==GBOMB_LAND) {
		r2=od.gbombrad*od.gbombrad;
		mr2=od.gbombminrad*od.gbombminrad;
		for (i=0;i<od.numcars;i++) {
//			if (wf->hitcar!=i) {
				del.x=wf->pos.x-opa[i].pos.x;
				del.y=wf->pos.y-opa[i].pos.y;
				del.z=wf->pos.z-opa[i].pos.z;
				d2=del.x*del.x+del.y*del.y+del.z*del.z;
				dist3dsq(&opa[i].pos,&wf->pos);
				if (d2<r2 && d2>mr2) {
					if (d2>=mr2) {
						f=od.gbombstrength*r2/d2;
						normalize3d(&del,&n);
						fv.x=f*n.x;
						fv.y=f*n.y;
						fv.z=f*n.z;
						opa[i].carvel.x+=fv.x;
						opa[i].carvel.y+=fv.y;
						opa[i].carvel.z+=fv.z;
					}
					opa[i].carvel.x*=od.gbombdamp;
					opa[i].carvel.y*=od.gbombdamp;
					opa[i].carvel.z*=od.gbombdamp;
				}
//			}
		}
	}
}

enum {STASIS_FALL,STASIS_ON,STASIS_SHRINK};
void initstasis(struct weapfly *wf)
{
	wf->speed=1;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(od.stasisscale,od.stasisscale,od.stasisscale);
//	setVEC(&wf->scale,.45f,.45f,.45f);
	wf->fuel=od.stasisstartfuel;
/*	if (wf->fromcar>=0) {
		wf->vel=od.ectocvel;
		wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
		wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
		wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
	} else */
		wf->vel=od.stasisfvel;
	wf->vel.x+=od.stasisrvel.x*(pfrand1(opa[0].clocktickcount)-.5f);
	wf->vel.y+=od.stasisrvel.y*(pfrand2(opa[0].clocktickcount)-.5f);
	wf->vel.z+=od.stasisrvel.z*(pfrand3(opa[0].clocktickcount)-.5f);
}

void procstasis(struct weapfly *wf)
{
	pointf3 v;
	float a;
	int i;
	wf->fuel--;
	if (wf->fuel==0) {
		freeprojectile(wf);
		return;
		}
	if (!wf->flags) { // fall to road
//		pointf3 pos;//,intsect;
//		pos=wf->pos;
		wf->pos.x+=wf->vel.x;
		wf->pos.y+=wf->vel.y;
		wf->pos.z+=wf->vel.z;
		wf->vel.y+=od.stasisgrav;
		if (wf->vel.y<=0) {
			wf->vel=zerov;
			wf->flags=STASIS_ON;
		}
	}
	if (wf->flags==STASIS_ON) {
		if (!od.intimewarp) {
//			a=TWOPI*mt_frand();
			a=wf->fuel*TWOPI*4/od.stasisstartfuel;
			v.x=wf->pos.x+od.stasisrad*(float)sin(a);
			v.y=wf->pos.y;
			v.z=wf->pos.z+od.stasisrad*(float)cos(a);
			spawnstasisfield(&v);
		}
		for (i=0;i<od.numcars;i++)
			if (opa[i].pos.y<wf->pos.y &&
				(opa[i].pos.x-wf->pos.x)*(opa[i].pos.x-wf->pos.x)+
				(opa[i].pos.z-wf->pos.z)*(opa[i].pos.z-wf->pos.z)<od.stasisrad*od.stasisrad) //{
			opa[i].venableicrms=10;
	}
/*	if (wf->flags>=STASIS_DISABLED) {
//		wf->pos=opa[k].pos;
		wf->scale.x+=.05f;
		wf->scale.y+=.05f;
		wf->scale.z+=.05f;
//		setVEC(&wf->scale,.3f,.3f,.3f);
	} */
}

enum {WALL_FALL,WALL_LAND};
void initwall(struct weapfly *wf)
{
	wf->speed=1;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(od.wallscale,od.wallscale,od.wallscale);
//	setVEC(&wf->scale,.45f,.45f,.45f);
	wf->fuel=od.wallstartfuel;
/*	if (wf->fromcar>=0) {
		wf->vel=od.ectocvel;
		wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
		wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
		wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
	} else */
		wf->vel=od.wallfvel;
	wf->vel.x+=od.wallrvel.x*(pfrand1(opa[0].clocktickcount)-.5f);
	wf->vel.y+=od.wallrvel.y*(pfrand2(opa[0].clocktickcount)-.5f);
	wf->vel.z+=od.wallrvel.z*(pfrand3(opa[0].clocktickcount)-.5f);
}

void procwall(struct weapfly *wf)
{
	wf->fuel--;
	if (wf->fuel==0) {
		if (wf->flags>=WALL_LAND)
			remcolltree((int)wf);
		freeprojectile(wf);
		return;
		}
	if (!wf->flags) { // fall to road
		pointf3 pos,intsect;
		pos=wf->pos;
		wf->pos.x+=wf->vel.x;
		wf->pos.y+=wf->vel.y;
		wf->pos.z+=wf->vel.z;
		wf->vel.y+=od.wallgrav;
		if (st2_line2road(&pos,&wf->pos,&intsect,NULL)) {
			wf->flags=WALL_LAND;
			wf->vel=zerov;
			wf->pos=intsect;
			addcolltree((int)wf,detachweapinfos[wf->kind].mastertree,&detachweapinfos[wf->kind].facenorms,&wf->pos,&wf->rot,&wf->scale);
		}
	}
}

enum {RAMP_FALL,RAMP_LAND};
void initramp(struct weapfly *wf)
{
	wf->speed=1;
//	wf->alwaysfacing=1;
	wf->scale=pointf3x(od.rampscale,od.rampscale,od.rampscale);
//	setVEC(&wf->scale,.45f,.45f,.45f);
	wf->fuel=od.rampstartfuel;
/*	if (wf->fromcar>=0) {
		wf->vel=od.ectocvel;
		wf->vel.x+=opa[wf->fromcar].carvel.x*od.ol_timeinc;
		wf->vel.y+=opa[wf->fromcar].carvel.y*od.ol_timeinc;
		wf->vel.z+=opa[wf->fromcar].carvel.z*od.ol_timeinc;
	} else */
		wf->vel=od.rampfvel;
	wf->vel.x+=od.ramprvel.x*(pfrand1(opa[0].clocktickcount)-.5f);
	wf->vel.y+=od.ramprvel.y*(pfrand2(opa[0].clocktickcount)-.5f);
	wf->vel.z+=od.ramprvel.z*(pfrand3(opa[0].clocktickcount)-.5f);
}

void procramp(struct weapfly *wf)
{
	wf->fuel--;
	if (wf->fuel==0) {
		if (wf->flags>=RAMP_LAND)
			remcolltree((int)wf);
		freeprojectile(wf);
		return;
		}
	if (!wf->flags) { // fall to road
		pointf3 pos,intsect;
		pos=wf->pos;
		wf->pos.x+=wf->vel.x;
		wf->pos.y+=wf->vel.y;
		wf->pos.z+=wf->vel.z;
		wf->vel.y+=od.rampgrav;
		if (st2_line2road(&pos,&wf->pos,&intsect,NULL)) {
			wf->flags=RAMP_LAND;
			wf->vel=zerov;
			wf->pos=intsect;
			addcolltree((int)wf,detachweapinfos[wf->kind].mastertree,&detachweapinfos[wf->kind].facenorms,&wf->pos,&wf->rot,&wf->scale);
		}
	}
}

// does it all
void makeaflyweap(int wk,int hitcar,pointf3* pos,pointf3* rot,pointf3* vel,int fuel,int from)
{
	struct weapfly *wf;
	if (wf=allocprojectile()) {
		wf->hitcar=hitcar;
		wf->pos=*pos;
		wf->rot=*rot;
		wf->vel=*vel;
		wf->fuel=fuel;
		wf->fromcar=from;
		wf->scale=pointf3x(1,1,1);
		wf->kind=wk;
		wf->proc=detachweapinfos[wk].procfunc;
//		wf->alwaysfacing=0;
		if (detachweapinfos[wk].initfunc)
			(*detachweapinfos[wk].initfunc)(wf);
//		if (wk->alwaysfacing)
//			t->flags|=TF_ALWAYSFACING;
	}
}

void procglobal()
{
#define NWKLIST 4
	static int wklist[NWKLIST]={DETACH_ECTO,DETACH_MISSILES,DETACH_EMB,DETACH_LASER};
	pointf3 vel={1,0,0};
	pointf3 pos={16,-1,2};
	int i;
// test a weapon fountain
	if (od.fountainon) {
		if ((opa[0].clocktickcount&63)==0) {
			i=((opa[0].clocktickcount)>>6)%NWKLIST;
			makeaflyweap(wklist[i],(opa[0].clocktickcount>>8)%od.numcars,&pos,&zerov,&vel,1000,-1);
		}
	}
// move global objects
	for (i=0;i<MAXWEAPFLY;i++)
		if (od.weapflys[i].proc && od.weapflys[i].active)
			(od.weapflys[i].proc)(&od.weapflys[i]);
// handle coins
	coinsproc();
}

void drawglobals()
{
	int i,k,j;
	int fa=0;
	tree2 *t;
	int curobj[MAXDETACHKIND];
	memset(curobj,0,sizeof(curobj));
	for (i=0;i<MAXDETACHKIND;i++)
		for (j=0;j<detachweapinfos[i].ntreepool;j++)
			if (detachweapinfos[i].treepool[j]->parent)
				unhooktree(detachweapinfos[i].treepool[j]);
	for (k=0;k<MAXWEAPFLY;k++)
		if (od.weapflys[k].active) {
			j=od.weapflys[k].kind;
			if (j>=0) {
				if (detachweapinfos[j].mastertree) {
					if (detachweapinfos[j].ntreepool<=curobj[j]) {
						detachweapinfos[j].ntreepool++;
//						detachweapinfos[j].treepool=memrealloc(
//								detachweapinfos[j].treepool,
//								detachweapinfos[j].ntreepool*sizeof(void *));
//						detachweapinfos[j].treepool[curobj[j]]=duptree(detachweapinfos[j].mastertree);
						tree2* dt=duptree(detachweapinfos[j].mastertree);
						detachweapinfos[j].treepool.push_back(dt);
					}
					t=detachweapinfos[j].treepool[curobj[j]];
					t->trans=od.weapflys[k].pos;
					t->rot=od.weapflys[k].rot;
					t->scale=od.weapflys[k].scale;
					t->treecolor.w=od.weapflys[k].dissolve;
					t->buildo2p=O2P_FROMTRANSQUATSCALE;
					linkchildtoparent(t,od.ol_root);
					curobj[j]++;
					if (j==DETACH_FUSIONCAN)
						fa=1;
				}
			}
		}
//	if (fa)
//		alterfusionball();
}

void pk_restorepstate0(int p)
{
	restoreplayerstate(&opasave0[p],&opa[p]);
}

void setuprematch()
{
	int i;
	for (i=0;i<od.numcars;i++) {
		int ii,jj;
		tree2 *t;
		ii=opa[i].ttweapstolenfrom;
		jj=opa[i].ttweapstolenslot;
		if (ii>=0) {
			t=opa[ii].weaptrees[jj];
			if (t) {
				logger("in rematch weapon %d given back to %d\n",jj,ii);
				unhooktree(t);
				linkchildtoparent(t,opa[ii].ol_carnull);
				t->flags|=TF_DONTDRAWC;
				seq_setframe(t,0);
			}
		}
		if (opa[i].hologram) {
			freetree(opa[i].hologram);
			opa[i].hologram=NULL;
		}
	}
	for (i=0;i<od.numcars;i++) {
		restoreplayerstate(&opasave0[i],&opa[i]);
//		restorepstate0(i);
		resetinputp(i); // erase ALL input that has been recorded
		opa[i].ol_utotalstunts=opa[i].ol_utotalcrashes=0;
	}
	restoreglobalstate(&globalstatepacket0);
	olracecfg.timeout=olracecfg.gameplaytimeout*30; // reset timer for waiting for 'here's
	od.numcurplayers=1; // wait 'till others restart also, that is, recieve all 'here's
	for (i=0;i<od.numcars-od.numbots;++i)
		if (od.playernet[i].onebotplayer)
			++od.numcurplayers;
	od.playernet[0].retrytimers=-1; // force resend 'here' packet
	od.gamenum++;
//	od.lastmillisec=getmillisec();
	od.quittimer=0;
	od.lastmillisec=0;
	od.speedup=0;
	od.ol_camnull->trans=od.ol_camstart;
	cleartext();
	od.signalfirst=0; // clear it
	od.numts=0;
	od.stepspace=od.trackend/MAXTS*1.5f;//od.path1->seqs[0].endframe/10/8;
	od.nextspace=od.trackend/20;
//	od.stepspace=od.nextspace=od.path1->seqs[0].endframe/10/8;
//	memset(&onegameinfo,0,sizeof(onegameinfo));
//	od.opponenttime=0;
//	od.opponentname[0]='\0';
	od.fplace=0;
	od.startcamrotx=-.3514f;
	od.jplace=0;
//	updateweapdisplay();
//	od.finishnames[0][0]='\0';
//	od.finishnames[1][0]='\0';
//	od.finishnames[2][0]='\0';
//	od.finishnames[3][0]='\0';
//	od.tempdisablekillwait=5*60;
//	midi_unload();
//	pushandsetdirdown("midifiles");
//	midi_load("carena1.mid");
//	popdir();
//	midi_play(1,0);
	new_cam_init();
	exitgstate();
	initgstate();
	boxai_reinit();
	coinsreinit();
//	initcolltree();
	setfog(1);
}

void disconnectoneplayer(int i) // server only
{
	freesocker(od.playernet[i].tcpclients);
	od.playernet[i].tcpclients=NULL;
	if (!od.playernet[i].onebotplayer) {
		++od.numdiscon;
		od.playernet[i].onebotplayer=1; // make this one player a bot..
	}
	logger("disconnectoneplayer %d\n",i);
}

//JAY
#ifdef KNOWCTF
void drawradarblip(pointf3* pos, pointf3* refpos, struct bitmap16 *b32, unsigned int color)
{
	pointf3 camrpy;
	float dx, dy, dir, dist;

	quat2rpy(&od.ol_camnull->rot,&camrpy);
	dx = (pos->x - refpos->x);
	dy = (pos->z - refpos->z);
	dir= ((float)atan2(dx, dy) - camrpy.y) - PI/2;
	dist = (float)sqrt(dx * dx + dy * dy) / RADAR_RANGE;
	if ( dist > 1.0f ) dist = 1.0f;
//	clipputpixel16(b16, (int)(RADAR_POS_X + RADAR_SCALE_X * dist * cos(dir)), (int)(RADAR_POS_Y + RADAR_SCALE_Y * dist * sin(dir)), color);
	clipcircle16(b16, (int)(RADAR_POS_X + RADAR_SCALE_X * dist * cos(dir)), (int)(RADAR_POS_Y + RADAR_SCALE_Y * dist * sin(dir)), 2,color);
}
#endif
int DoCapture = 0;
int CurrentFrame = 0;
void onlineracemain_proc()
{
	ho->reset();
//	ho->addjack(od.ol_root,pointf3(),10,F32WHITE);
//	logger("race main proc\n");
//	int w;
	int millisec;
	int leader=0,bigleader=0;
	float bestseekframe,worstseekframe;
//	struct bitmap16 *conb16=NULL;
	struct mat4 pm,pm2;
//	extern float testheight;
//	static int testfont=1;
	int fuloop;
	int clocksave;
	int dt;
//	pointf3 yv;
//	extern int ol_releasemode;
//	int si,i,j;
	int si,i,j,esi,fut;
//	extern int ol_ufliptime;
//	popstate();
//	return;
//	op=&opa[0];
//	logger("race proc\n");
//	struct rmessage rm;
	static int bailer;
	if (bailer) {
//		if (bailer==1)
//			sendapacket(od.playerid,-1,PKT_DISCONNECT,0,NULL);
		bailer++;
		if (bailer>60 || packetwrite()) {
			if (ol_opt.backtoopt || matchcl) {
				if (matchcl) {
					changestate(STATE_MATCHRESULTS);
				} else {
					changestate(STATE_OLDCARENALOBBY);
				}
			} else {
				changestate(-1);
			}
			bailer=0;
		}
		return;
	}
//	checkres(game_rl);


	shape* over=game_rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
	game_rl->over_no_over_proc(focus,over);
//	if (wininfo.mleftclicks)
//		focus=game_rl->getfocus();
// if something selected...
//	while(getresmess(game_rl,&rm)) {
	shape* focusb=0;
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		focusb = ret>=0 ? focus : 0;
//		switch(rm.id) {
// quit
//		case PBUTCANCEL:
	}
	if (od.resvis && !focusb) {
		if (KEY=='y') {
			focusb=PBUTYES;
		} else if (KEY=='n') {
			focusb=PBUTNO;
		}
	}
	if (focusb==PBUTYES) { // start state button, can't do a switch on pointers
//		switch(rm.id) {
//		case PBUTYES:
		od.sentrematch=1;
//			setresvis(game_rl,PBACK1,0);
//			setresvis(game_rl,PTEXTQ,0);
//			setresvis(game_rl,PTEXTQ2,0);
//			setresvis(game_rl,PBUTYES,0);
//			setresvis(game_rl,PBUTNO,0);
/*		PTEXTQ->setvis(0);
		PTEXTQ2->setvis(0);
		PBUTYES->setvis(0);
		PBUTNO->setvis(0);
*/
		setrematchvis(0);
		if (od.resvis)
			if (od.norematch) {
				bailer=1;
				od.quitting=1;
			} else {
//				if (od.playerid==0) // do both
					++od.rematchplayers;
//				else
					requestrematch();
			}
		od.resvis=0;
//		break;
	} else if (focusb==PBUTNO) {
//		case PBUTNO:
//			setresvis(game_rl,PBACK1,0);
//			setresvis(game_rl,PTEXTQ,0);
//			setresvis(game_rl,PTEXTQ2,0);
//			setresvis(game_rl,PBUTYES,0);
//			setresvis(game_rl,PBUTNO,0);
/*		PTEXTQ->setvis(0);
		PTEXTQ2->setvis(0);
		PBUTYES->setvis(0);
		PBUTNO->setvis(0); */
		setrematchvis(0);
		od.resvis=0;
		bailer=1;
		od.quitting=1;
//		break;
	}
#ifdef USEVARCHECKERrace
//	varchange();
#endif
	if (ol_opt.nt==NET_BCLIENT && !od.playernet[0].tcpclients)
		disconnectandai();
	else if (od.numplayers>1 && olracecfg.timeout>0 && od.numcurplayers<od.numplayers-od.numbots) {
		olracecfg.timeout--;
		if (olracecfg.timeout==0 || od.badtrack)
			disconnectandai(); // player didn't arrive
	}
	packetread();
	if(KEY==K_ESCAPE || (od.quittime && od.quittimer>od.quittime)) { //wininfo.keystate[DIK_JOYBUTTON(3)]) && !dofinish */  ) {
//		if (!of->ol_dofinish)
			bailer=1;

//		popstate();
	}
/*	if (KEY=='f' && !wininfo.releasemode) {
		extern int perscorrect;
		perscorrect++;
		if (perscorrect==3)
			perscorrect=0;
//		extern int favorshading;
//		favorshading^=1;
	} */
// test sound ................................
//	if (od.defuloop) {
/*		if (KEY=='s')
			for (i=0;i<od.ol_numplayers;i++) {
//				saveplayerstate(&opa[i],&opasave[i]);
//				saveplayerstate2(&opa[i],&opasave2[i]);
				}
		if (KEY=='r')
			for (i=0;i<od.ol_numplayers;i++) {
//				struct ol_playerdata td;
//				td=opa[i];
//				restoreplayerstate2(&opasave2[i],&opa[i]);
#ifdef USEVARCHECKERrace
				marknochange();
#endif
//				opa[i]=td;
//				restoreplayerstate(&opasave[i],&opa[i]);
#ifdef USEVARCHECKERrace
				varchange();
#endif
			}
//	} */
//	if (od.defuloop) {
//	S32 onebots=0;
//	S32 firstnonbot=0;
//	for (i=0;i<od.numplayers-od.numbots;++i) {
//		if (od.playernet[i].onebotplayer)
//			++onebots;
//		else if (firstnonbot==0)
//			firstnonbot=i;
//	}
	if (/*od.playerid==firstnonbot && */od.rematchplayers==od.numplayers-od.numbots-od.numdiscon) {
		if (od.norematch) {
			od.quitting=1;
			bailer=1;
		} else {
			od.rematchplayers=0;
			setuprematch();
		}
	}
//	if (KEY=='k' && !wininfo.releasemode && od.playerid==0 &&
//		olracecfg.testkicknum>0 && olracecfg.testkicknum<od.numcars) {
//		disconnectoneplayer(olracecfg.testkicknum);
//	}
	if (KEY=='g'  && !wininfo.releasemode /*&& of->dofinish*/) {
		setuprematch();
	}
	if (KEY=='.' && !wininfo.releasemode)
		++od.defuloop;
	if (KEY==',' && !wininfo.releasemode && od.defuloop>=0)
		--od.defuloop;
	if (KEY=='s' && !wininfo.releasemode)
	{
       DoCapture++;
	   if (DoCapture > 1) DoCapture = 0;
	}
	if (KEY=='a') // toggle attachcam
		mainvp.useattachcam^=1;
/*	if (KEY=='a' && !wininfo.releasemode) { // play a bunch of sounds
		;//AckKey(DIK_A);
		od.intimewarp=0;
		ol_playatagsound(od.ol_curwave,1.0f,0);
		od.intimewarp=1;
		od.ol_curwave++;
		if (od.ol_curwave==od.ol_numwaves)
			od.ol_curwave=0;
	} */
	if (KEY=='b' && !wininfo.releasemode) {
		op=of;
		ol_douburst(30);
		ol_douflames(30);
		ol_doblueflash(30);
		ol_dolightning(30);
		ol_docondom(30);
		op->vinthermo=60;
	}
	if (KEY=='v' && !wininfo.releasemode) {
		of->carvis++;
		if (of->onlinecar) {
			if (of->carvis&1) {
				of->onlinecar->flags&=~TF_DONTDRAWC;
				of->regpointsoffset=of->regpointsoffsetjrm;
			} else
				of->onlinecar->flags|=TF_DONTDRAWC;
		}
		if (of->y2009car) {
			if (of->carvis&2) {
				of->y2009car->flags&=~TF_DONTDRAWC;
			} else
				of->y2009car->flags|=TF_DONTDRAWC;
		}
/*		if (of->shinycar) {
			if (of->carvis&2) {
				of->shinycar->flags&=~TF_DONTDRAWC;
				of->regpointsoffset=of->regpointsoffsetshiny;
			} else
				of->shinycar->flags|=TF_DONTDRAWC;
		} */
//		of->onlinecar->children[0]->flags^=TF_DONTDRAW;
	}
	if (KEY=='m' && !wininfo.releasemode) {
		if (of->playerid==od.playerid) {
			of->cntl++;
			if (of->cntl==3)
				of->cntl=0;
		}
	}
	if (KEY=='t' && !wininfo.releasemode) {
		od.tvcammode^=1;
		if (od.tvcammode==0) {
			od.ol_camnull->rot=od.ol_qcamdesired;
			od.ol_camnull->trans=od.ol_camdesired;
		}
		od.bigmove=3;
	}
//	if (KEY=='l' && !wininfo.releasemode && !od.resvis)
//		od.showcon^=1;
	if (KEY=='p' && !wininfo.releasemode)
		if (od.tvcammode)
			od.tvcampan^=1;
		else
			od.pingmode^=1;
#ifdef USEVARCHECKERrace
/*	if (KEY=='l') {
		inlogmode=1;
	}
	if (inlogmode) {
		logger("current player for varchange %d\n",of-opa);
		varchange();
	} */
#endif
//	if (KEY=='d' && !wininfo.releasemode)
	if (wininfo.releasemode) {
		if (KEY=='d')
			od.difftimemode^=1;
	} else {
		if (KEY=='d') {
			od.difftimemode++;
			if (od.difftimemode==4)
				od.difftimemode=0;
		}
	}
#if 0
	if (KEY=='l' && !wininfo.releasemode) {
		zoomin^=1;
		if (zoomin) {
			mainvp.xres=WX/2;
			mainvp.yres=WY/2;
			mainvp.xstart=WX/2;
			mainvp.ystart=WY/2;
			mainvp2.xres=WX/2;
			mainvp2.yres=WY/2;
			mainvp2.xstart=WX/2;
			mainvp2.ystart=WY/2;
		} else {
			mainvp.xres=WX;
			mainvp.yres=WY;
			mainvp.xstart=0;
			mainvp.ystart=0;
			mainvp2.xres=WX;
			mainvp2.yres=WY;
			mainvp2.xstart=0;
			mainvp2.ystart=0;
		}
	}
#endif
	if (KEY==']' && !wininfo.releasemode) {
		of++;
		if (of==&opa[od.numcars])
			of=&opa[0];
		od.bigmove=20;
//		updateweapdisplay();
	}
	if (KEY=='[' && !wininfo.releasemode) {
		if (of==&opa[0])
			of=&opa[od.numcars];
		of--;
		od.bigmove=20;
//		updateweapdisplay();
	}
	if (KEY=='n' && !wininfo.releasemode) {
		GAMENEWS->setvis(1-GAMENEWS->getvis());
	}
	if (od.path1 && od.tvcammode) {
		od.tvcamnum=(int)(.5f+(NUMTVCAMS-1)*(of->seekframe2-od.trackstart)/(od.trackend-od.trackstart));
		if (od.tvcamnum>=NUMTVCAMS)
			od.tvcamnum=NUMTVCAMS-1;
//			od.tvcamnum-=NUMTVCAMS;
	}
#if 0
	if ((KEY=='-' || KEY=='=') && video_maindriver!=VIDEO_D3D && !wininfo.releasemode) {
		int dir;
		if (KEY=='-')
			dir=-1;
		else
			dir=1;
		if (changexyres(globalxres,globalyres,dir,&globalxres,&globalyres)) {
			video_setupwindow(globalxres,globalyres,DESIREDBPP);
			if (olracecfg.bluered3d) {
				bitmap32free(od.blueredbm);
				od.blueredbm=bitmap32alloc(WX,WY,C32BLACK);
			}
			if (zoomin) {
				mainvp.xres=WX/2;
				mainvp.yres=WY/2;
				mainvp.xstart=WX/2;
				mainvp.ystart=WY/2;
				mainvp2.xres=WX/2;
				mainvp2.yres=WY/2;
				mainvp2.xstart=WX/2;
				mainvp2.ystart=WY/2;
			} else {
				mainvp.xres=WX;
				mainvp.yres=WY;
				mainvp.xstart=0;
				mainvp.ystart=0;
				mainvp2.xres=WX;
				mainvp2.yres=WY;
				mainvp2.xstart=0;
				mainvp2.ystart=0;
			}
		}
	}
#endif
//	resetnulls();
//	resetplanes();
//	resetvecs();
#ifdef USEROADPROBE
	addnull(&roadprobe1,0);
	yv.x=roadprobe2.x+roadprobe1.x;
	yv.y=roadprobe2.y+roadprobe1.y;
	yv.z=roadprobe2.z+roadprobe1.z;
	addnull(&yv,0);
	if (st2_line2road(&roadprobe1,&yv,&roadprobei,&roadprobenorm)) {
		addnull(&roadprobei,0);
		roadprobenorm.x+=roadprobei.x;
		roadprobenorm.y+=roadprobei.y;
		roadprobenorm.z+=roadprobei.z;
		addnull(&roadprobenorm,0);
		roadprobenorm.x-=roadprobei.x;
		roadprobenorm.y-=roadprobei.y;
		roadprobenorm.z-=roadprobei.z;
	} //else {
//		addnull(&zerov,0);
//		addnull(&zerov,0);
//	}
//	addnull(&landspot,0);
#endif
	if (od.ntrackname[0]=='\0')
		od.ol_finishpiece->trans.y=(od.ol_finishy=-.257813f);
///////////// okay now do the physics/math
	if (wininfo.fpsavg2>59) {// 60
		wininfo.fpswanted=60;
	} else if (wininfo.fpsavg2>29) {// 30
		wininfo.fpswanted=30;
	} else if (wininfo.fpsavg2>19) {// 20
		wininfo.fpswanted=20;
	} else if (wininfo.fpsavg2>14) { // 15
		wininfo.fpswanted=15;
	} else if (wininfo.fpsavg2>11) { // 12 and below
		wininfo.fpswanted=12;
	} else if (wininfo.fpsavg2>9) {
		wininfo.fpswanted=10;
	} else
		wininfo.fpswanted=6;
	if ( crashinfos.editenable || checkinfos.editenable /* || botaiinfos.editenable */ ||
		(!od.lastmillisec && od.numplayers-od.numbots==od.numcurplayers))
//		od.lastmillisec=getmillisec();
//	millisec=getmillisec();
		od.lastmillisec=GetTickCount();
	millisec=GetTickCount();
	if (!currule->rboxes) {
		crasheditor(millisec,&opa[0]);
		checkeditor(millisec,&opa[0]);
	}
//	boteditor(millisec,&opa[0]);
	fuloop=(millisec+od.speedup-od.lastmillisec)*6/100-opa[0].clocktickcount;
//	logger("millisec %d, fuloop %d\n",millisec,fuloop);
	od.ol_timeinc=1/60.0f;
// figure out leader for slow down leader
	if (od.numcars==1)
		leader=-1;
	else {
		bestseekframe=-100000.0f;
		worstseekframe=100000.0f;
		for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
			float sf=op->seekframe2+op->laps2*(od.trackend+1);
			if (sf>bestseekframe) {
				leader=i;
				bestseekframe=sf;
			}
			if (sf<worstseekframe)
				worstseekframe=sf;
		}
		if (bestseekframe-worstseekframe>=od.wayleadframediff)
			bigleader=2;
		else
			bigleader=1;
	}
// wait for all players to arrive
	if (od.numplayers-od.numbots==od.numcurplayers && (od.playernet[0].retrytimers!=-1 || od.numplayers==1)) {
		if (!od.defuloop)
			od.ol_uloop=fuloop; // pro: network play, prevents neg ping speedup issues, con: seems jerkier, bad motorsound
//			od.ol_uloop=max(1,fuloop); // force at least 1 gametick 1/60 sec, pro: smoother, good motorsound, con: network neg ping speedups
		// resolve by fixing bad motorsound...
		else
			od.ol_uloop=od.defuloop;
		od.ol_uloop=max(0,od.ol_uloop);
		if (opa[0].clocktickcount<10) {
			strcpy(od.scorestrings[1],"");
			strcpy(od.scorestrings[2],"");
		}
	} else {
		od.ol_uloop=0;	// don't start until all players arrive
		od.cscoret[1]=20;
		od.tscoret[1]=10000;
		strcpy(od.scorestrings[1],"    WAITING FOR");
		od.cscoret[2]=20;
		od.tscoret[2]=10000;
		strcpy(od.scorestrings[2],"     OPPONENTS");
//		od.cscoret[3]=20;
//		od.tscoret[3]=10000;
//		strcpy(od.scorestrings[3],"       TO ARRIVE.");
	}
	for (i=0,op=&opa[0];i<od.numcars-od.numbots;i++,op++) {
		if (!op->dofinish)
			break;
	}
	if (i==od.numcars-od.numbots || od.quittimer) { // race is all done, wait around awhile
		od.quittimer+=fuloop;
//		logger("od.quittimer is now %d\n",od.quittimer);
	}
///////////////////// start of player state changes //////////////////////////
	for (j=0;j<od.ol_uloop;j++) {
		resetcar2car();
//		esi=-2;
		esi=2000000000;
		fut=0;
		for (i=0,op=&opa[0];i<od.numcars-od.numbots;i++,op++) {
			si=someinput(op,1);
//			logger("si %d from car %d\n",si,i);
			if (si<esi)
//			if (si<esi || esi<0)
				esi=si;
			if (si>fut)
				fut=si;
		}
//		GAMENEWS->printf("si %d %d %d",someinput(&opa[0],1),someinput(&opa[1],1),someinput(&opa[2],1));
		if (fut>opa[0].clocktickcount) {	// if packets from the future, speed up
			od.speedup+=10;
//			logger("od.speedup = %d\n",od.speedup);
		}
		if (!opa[0].clocktickcount) {
			for (i=0;i<od.numcars;i++) {
				saveplayerstate(&opa[i],&opasave0[i]);
				saveplayerstate(&opa[i],&opasave[i]); // save first instance
			}
			saveglobalstate(&globalstatepacket0);
			saveglobalstate(&globalstatepacket);
		}
		od.intimewarp=1;
// print ping
		dt=opa[0].clocktickcount-fut;
		if (dt>=0)
			dt=opa[0].clocktickcount-esi; // halfping time..
		if (od.pingmode && dt /*&& !od.fplace*/) { // && op->pnum!=od.playerid) {
			od.tscoret[PTEXT_PING]=100;
			od.cscoret[PTEXT_PING]=0;
			sprintf(od.scorestrings[PTEXT_PING],"       %6d",dt*1000/60);
		}
		if (!od.pingmode && of->chkpnt==0 && opa[0].clocktickcount>=INITCANTSTARTDRIVE) {
			od.tscoret[PTEXT_PING]=100;
			od.cscoret[PTEXT_PING]=0;
			if (od.laps>1)
				if (of->chklap==od.laps-1)
					sprintf(od.scorestrings[PTEXT_PING],"      LAST LAP");
				else if (of->chklap<od.laps)
					sprintf(od.scorestrings[PTEXT_PING],"     LAP %d OF %d",of->chklap+1,od.laps);
		}
// time correct if save state is older than valid input
//		logger("esi %d, save %d\n",esi,opasave[0].clocktickcount);
		if (opasave[0].clocktickcount<esi) {
			clocksave=opa[0].clocktickcount;
			resetcar2car();
			for (i=0,op=&opa[0];i<od.numcars;i++,op++)
				restoreplayerstate(&opasave[i],&opa[i]);
			restoreglobalstate(&globalstatepacket);
// wind back to either clocksave or last valid state
			S32 w=0;
			od.predicted=0;
			while(opa[0].clocktickcount<esi && opa[0].clocktickcount<clocksave) {
//				showgamestate();
				resetcar2car();
				for (i=0,op=&opa[0];i<od.numcars;i++,op++)
					ol_calccollisioninfo();
				for (i=0,op=&opa[0];i<od.numcars;i++,op++)
					procaplayer(i,leader,bigleader);
				procglobal();
				for (i=0,op=&opa[0];i<od.numcars;i++,op++)
					procaplayer2(leader,bigleader);
				w++;
				addgstate();
				if (w==10000)
					errorexit("big watchdog in loop1 clk %d, esi %d, cs %d",opa[0].clocktickcount,esi,clocksave);
			}
// save last valid state OR save last valid state at this clocksave
			for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
				saveplayerstate(&opa[i],&opasave[i]);
				removeinput(i,esi-1);
			}
			saveglobalstate(&globalstatepacket);
// wind back to clocksave
			w=0;
			od.predicted=1;
			while(opa[0].clocktickcount<clocksave) {
				resetcar2car();
				for (i=0,op=&opa[0];i<od.numcars;i++,op++)
					ol_calccollisioninfo(); // bbox , points, moment of interia, new loc
				for (i=0,op=&opa[0];i<od.numcars;i++,op++)
					procaplayer(i,leader,bigleader);
				procglobal();
				for (i=0,op=&opa[0];i<od.numcars;i++,op++)
					procaplayer2(leader,bigleader);
				w++;
				if (w==10000)
					errorexit("big watchdog in loop2 clk %d, esi %d, cs %d",opa[0].clocktickcount,esi,clocksave);
			}
		}
//			if (j==od.ol_uloop-1)
		od.intimewarp=0;
//		if (od.numplayers>1)
			od.predicted=1;
//		else {
//			od.predicted=0;
//			showgamestate();
//		}
		resetcar2car();
		for (i=0,op=&opa[0];i<od.numcars;i++,op++)
			ol_calccollisioninfo(); // bbox , points, moment of interia, new loc
		for (i=0,op=&opa[0];i<od.numcars;i++,op++)
			procaplayer(i,leader,bigleader);
		procglobal();
		for (i=0,op=&opa[0];i<od.numcars;i++,op++)
			procaplayer2(leader,bigleader);
		od.intimewarp=1;
		if (od.markfordisconnect) {
			od.markfordisconnect=0;
			disconnectandai(); // player lagged too far behind
		}
	}
///////////////////// end of player state changes //////////////////////////
//	logger("done uloop one\n");
	for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
		ol_drawobjects();
		ol_uplayupdateclock2(); // really burst and such, per object
	}
	drawglobals();
	if (od.minearmed!=od.lastminearmed) {
		od.lastminearmed=od.minearmed;
		if (od.minetex) {
			struct bitmap32 *b;
			b=locktexture(od.minetex);
			if (od.minearmed)
				cliprect32(b,15,15,17,17,od.minearmcolor);
			else
				cliprect32(b,15,15,17,17,od.minedisarmcolor);
			unlocktexture(od.minetex);
		}
	}
	op=NULL;
//	ol_uplayupdateclock(); // scoreline, per game
//	if (opa[0].clocktickcount>0)
//		updatescoreline(of);

	if (od.tvcammode) {
		od.ol_camnull->trans=od.tvcams[od.tvcamnum];
		if (od.tvcampan)
			targetquat(&od.tvcams[od.tvcamnum],&of->pos,&od.ol_camnull->rot);
		else
			od.ol_camnull->rot=od.tvcamsq[od.tvcamnum];
//		sprintf(teststr,"%d %f %f %f\n",od.tvcamnum,od.tvcams[od.tvcamnum].x,od.tvcams[od.tvcamnum].y,od.tvcams[od.tvcamnum].z);
//		od.ol_camnull->rot=zerov;
//		od.ol_camnull->rot.w=1;
	}
// doppler, first get camera vel
	od.lastcamposdp=od.camposdp;

	od.camposdp.x=0;
	od.camposdp.y=20;
	od.camposdp.z=0;
//	addnull(&od.camposdp,NULL);

	if (mainvp.useattachcam)
		obj2world(od.ol_camnull,&mainvp.camtrans,&od.camposdp);
	else
		od.camposdp=mainvp.camtrans;
	if (1) { // some panning
//		pointf3 tr;
//		tr.x=0;
//		tr.y=20;
//		tr.z=0;
		buildrottrans3d(&mainvp.camrot,&mainvp.camtrans,&pm);
		if (mainvp.useattachcam) {
			quattrans2xform(&od.ol_camnull->rot,&od.ol_camnull->trans,&pm2);
			mulmat3d(&pm,&pm2,&pm);
		}
		inversemat3d(&pm,&pm);
//		xformvec(&m,&tr,&tr);
//		sprintf(teststr,"%f %f %f\n",tr.x,tr.y,tr.z);
	}
// camera velocity for car doppler
	if (od.ol_uloop>0) {
		od.camveldp.x=(od.camposdp.x-od.lastcamposdp.x)/od.ol_uloop;
		od.camveldp.y=(od.camposdp.y-od.lastcamposdp.y)/od.ol_uloop;
		od.camveldp.z=(od.camposdp.z-od.lastcamposdp.z)/od.ol_uloop;
	} else {
		od.camveldp.x=(od.camposdp.x-od.lastcamposdp.x);
		od.camveldp.y=(od.camposdp.y-od.lastcamposdp.y);
		od.camveldp.z=(od.camposdp.z-od.lastcamposdp.z);
	}
// car sounds: volume and cutoff
	for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
//		logger("clock %d for loop 2 player %d\n",op->ol_clocktickcount,i);
		op->vold=dist3d(&od.camposdp,&op->pos);
//		od.lightstate=2;
		if (od.lightstate>1)
			if (op->vold<od.voldist)
				op->vold=1;
			else
				op->vold=od.voldist/op->vold;
		else
			op->vold=0;
//		op->vold=1;
		if (op->vold<od.volcutoff && op->ol_motorsh) {
			sound_free(op->ol_motorsh);
			op->ol_motorsh=NULL;
		}
		if (op->vold>=od.volcutoff && !op->ol_motorsh) {
			op->ol_motorsh=wave_playvol(taggetwhbyidx(od.ol_gardentags,1),0,.75f*op->vold);
//			op->ol_motorsh=wave_playvol(taggetwhbyidx(od.ol_gardentags,op->cartype),0,.75f);
//			if (op->ol_motorsh)
//				;//setsfxfreq(op->ol_motorsh,(float)op->dpf*op->ol_uplayrevfreq/op->ol_motorsh->basefreq);
//			else
//				logger("can't get motor sound\n");
		}
// car sounds: doppler, only if
		if (op->ol_motorsh && od.ol_uloop) {
			if (od.bigmove==0) {
				pointf3 tr; // from car to cam
				float dt,dr,f; // dot car, dot cam
				op->carveldp.x=(op->pos.x-op->ol_ulastpos.x);
				op->carveldp.y=(op->pos.y-op->ol_ulastpos.y);
				op->carveldp.z=(op->pos.z-op->ol_ulastpos.z);
//				op->carveldp.x=(op->pos.x-op->ol_ulastpos.x)/od.ol_uloop;
//				op->carveldp.y=(op->pos.y-op->ol_ulastpos.y)/od.ol_uloop;
//				op->carveldp.z=(op->pos.z-op->ol_ulastpos.z)/od.ol_uloop;
				tr.x=od.camposdp.x-op->pos.x;
				tr.y=od.camposdp.y-op->pos.y;
				tr.z=od.camposdp.z-op->pos.z;
				if (!normalize3d(&tr,&tr))
					tr=zerov;
				dr=dot3d(&tr,&od.camveldp);
				dt=dot3d(&tr,&op->carveldp);
				if (dt<od.speedosound) {
					f=(od.speedosound-dr)/(od.speedosound-dt); // doppler formula
					if (f>.2f && f<5.0f)
						op->dpf=f;
				}
				xformvec(&pm,&op->pos,&tr);
//				tr.y=0;
				if (normalize3d(&tr,&tr))
					op->pan=tr.x;
				setsfxfreq(op->ol_motorsh,(float)op->dpf*op->ol_uplayrevfreq/op->ol_motorsh->basefreq);
//				logger("setsfxfreq dpf %f, revfreq %d,base %d\n",op->dpf,op->ol_uplayrevfreq,op->ol_motorsh->basefreq);
				setsfxvolume(op->ol_motorsh,.75f*op->vold);
				setsfxpan(op->ol_motorsh,op->pan);
			}
		}
// finish up
//		op->ol_lleft=op->ol_uleft;
//		op->ol_lright=op->ol_uright;
//		op->ol_lrollleft=op->ol_urollleft;
//		op->ol_lrollright=op->ol_urollright;
	}
//	logger("done loop 2\n");
	packetwrite();
	if (ol_opt.nt==NET_BCLIENT) { // broadcast server went down or you were kicked from it
		if (od.playernet[0].tcpclients && od.playernet[0].tcpclients->flags & (SF_CLOSED|SF_ERR)) {
			freesocker(od.playernet[0].tcpclients);
			od.playernet[0].tcpclients=0;
			for (i=0;i<od.numcars-od.numbots;++i) {
				if (od.playerid!=opa[i].playerid) {
					if (!od.playernet[i].onebotplayer) {
						od.playernet[i].onebotplayer=1;
						++od.numdiscon;
					}
				}
			}
		}
	} else {
		if (od.numplayers!=1) {
//			int m,mi,to;
			for (i=0;i<OL_MAXPLAYERS;i++) {
				if (od.playernet[i].tcpclients) {
					if (od.playernet[i].tcpclients->flags & (SF_CLOSED|SF_ERR)) { // broadcast server shut down
//						logger("socket %d is closed now\n",i);
						if (i>0) {
							if (!od.playernet[i].onebotplayer)
								disconnectoneplayer(i);
							if (od.numdiscon+1==od.numplayers)
								i=0;
						}
						if (i==0) {
							od.cscoret[1]=0;
							od.tscoret[1]=0;
							od.cscoret[2]=0;
							od.tscoret[2]=0;
							od.cscoret[3]=0;
							od.tscoret[3]=0;
							disconnectandai(); // player left
							if (!od.allbotgame)
								od.norematch=1;
						}
//					} else {
//						logger("socket %d flags %02x\n",i,od.playernet[i].tcpclients->flags);
					}
				}
				}
		}
	}

	if (od.difftimemode==0) { // car loc
		od.tscoret[8]=120;
		od.cscoret[8]=20;
		sprintf(od.scorestrings[8],"           %9.3f",of->pos.x);
		od.tscoret[9]=120;
		od.cscoret[9]=20;
		sprintf(od.scorestrings[9],"           %9.3f",of->pos.y);
		od.tscoret[10]=120;
		od.cscoret[10]=20;
		sprintf(od.scorestrings[10],"           %9.3f",of->pos.z);
	} else if (od.difftimemode==2) { // debug info, show when crossed checkpoints
		od.tscoret[8]=120;
		od.cscoret[8]=20;
		sprintf(od.scorestrings[8],"           LAP %d",of->chklap);
		od.tscoret[9]=120;
		od.cscoret[9]=20;
		if (of->boxchk)
			sprintf(od.scorestrings[9],"    %s",of->boxchk->name);
		od.tscoret[10]=120;
		od.cscoret[10]=20;
//		sprintf(od.scorestrings[10],"");
		od.scorestrings[10][0]='\0';
	} else if (od.difftimemode==3 && od.playerid==0) { // debug info, input buffers
		od.tscoret[8]=120;
		od.cscoret[8]=20;
		sprintf(od.scorestrings[8],"           P1 %d",od.playernet[1].ninput);
		od.tscoret[9]=120;
		od.cscoret[9]=20;
		sprintf(od.scorestrings[9],"           P2 %d",od.playernet[2].ninput);
		od.tscoret[10]=120;
		od.cscoret[10]=20;
		sprintf(od.scorestrings[10],"           P3 %d",od.playernet[3].ninput);
	}
	op=NULL;
	if (od.path1)
		od.path1->curframe=0;
	if (od.path2)
		od.path2->curframe=0;
//	doanims(od.ol_root);

// helpers
	if (showphysics) {
//	ho->addjack(od.ol_root,of->pos,.25,F32WHITE);
	ho->addjack(of->ol_carnull,pointf3x(),.35f,F32WHITE);
	ho->addbox(of->ol_carnull,of->ol_carbboxmin,of->ol_carbboxmax,pointf3x(1,1,0,.4f));
	pointf3 sh0,sh1;
	float esl[4];
	esl[0]=of->ol_normalshocklen[0]*of->shockextra;
	esl[1]=of->ol_normalshocklen[1]*of->shockextra;
	esl[2]=of->ol_normalshocklen[2]*of->shockextra;
	esl[3]=of->ol_normalshocklen[3]*of->shockextra;
	sh0.x=-of->ol_shockspacingx/2*of->shockextra;
	sh0.y=of->ol_shockoffsety;
	sh0.z=of->ol_shockspacingz/2*of->shockextra;
	sh1.x=-of->ol_shockspacingx/2*of->shockextra;
	sh1.y=of->ol_shockoffsety-esl[0];
	sh1.z=of->ol_shockspacingz/2*of->shockextra;
	ho->addline(of->ol_carnull,sh0,sh1,F32WHITE);
	sh0.x=of->ol_shockspacingx/2*of->shockextra;
	sh0.y=of->ol_shockoffsety;
	sh0.z=of->ol_shockspacingz/2*of->shockextra;
	sh1.x=of->ol_shockspacingx/2*of->shockextra;
	sh1.y=of->ol_shockoffsety-esl[1];
	sh1.z=of->ol_shockspacingz/2*of->shockextra;
	ho->addline(of->ol_carnull,sh0,sh1,F32WHITE);
	sh0.x=-of->ol_shockspacingx/2*of->shockextra;
	sh0.y=of->ol_shockoffsety;
	sh0.z=-of->ol_shockspacingz/2*of->shockextra;
	sh1.x=-of->ol_shockspacingx/2*of->shockextra;
	sh1.y=of->ol_shockoffsety-esl[2];
	sh1.z=-of->ol_shockspacingz/2*of->shockextra;
	ho->addline(of->ol_carnull,sh0,sh1,F32WHITE);
	sh0.x=of->ol_shockspacingx/2*of->shockextra;
	sh0.y=of->ol_shockoffsety;
	sh0.z=-of->ol_shockspacingz/2*of->shockextra;
	sh1.x=of->ol_shockspacingx/2*of->shockextra;
	sh1.y=of->ol_shockoffsety-esl[3];
	sh1.z=-of->ol_shockspacingz/2*of->shockextra;
	ho->addline(of->ol_carnull,sh0,sh1,F32WHITE);
// end helpers
	}
	od.ol_root->proc();
// hi level camera
	doflycam(&mainvp);
// prepare scene
//	buildtreematrices(mainvproottree); //roottree,camtree);
//	buildtreematrices(mainvp2roottree); //roottree,camtree);
	video_buildworldmats(mainvproottree);
//	video_buildworldmats(mainvp2roottree);
// draw scene
#ifdef KNOWBLUERED
	if (olracecfg.bluered3d) { // works only in software
		float camxsave;
		camxsave=mainvp.camtrans.x;
		mainvp.camtrans.x+=olracecfg.bluered3d;
		video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
		video_drawscene(&mainvp);
		makeblue(B32,od.blueredbm);
		video_endscene(&mainvp);
//		mainvp.flags&=~VP_CLEARBG;
		mainvp.camtrans.x=camxsave-olracecfg.bluered3d;
		video_beginscene(&mainvp);
//		mainvp.flags|=VP_CLEARBG;
		video_drawscene(&mainvp);
		combinebluered(od.blueredbm,B32);
		mainvp.camtrans.x=camxsave;
	} else {
		video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
		video_drawscene(&mainvp);
	}
#else
//		video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
//		video_drawscene(&mainvp);
//		video_setviewport(&mainvp);
//		video_drawscene(mainvproottree);
#endif
//	d3d_setfog(0,0,0,&zerov,2,0);
//	video_drawscene(&mainvp2);
//	video_setviewport(&mainvp2);
//	video_drawscene(mainvp2roottree);
//	video_endscene(&mainvp); // nothing right now
//	if (zoomin) {
//		video_lock();
//		zoombitmap16(B32);
//		video_unlock();
//	}
// calc score for constructor
//	if(of->dofinish && od.ol_uedittrackname[0]=='\0')
//		ol_calcscore();
// calc score for prebuilt, paths
//JAY MESS2
	if (currule->ctf) {
#ifdef KNOWCTF
		if (od.numcars>1) {
		//if (od.numcars>1 && od.path1) {
		//if (od.ol_numplayers>1 && od.path1 && od.ol_framecount>300) {
			//float a;//,b;
			int p=0;

			if (of->finplace)
				p=of->finplace-1;
			else {
				//a=of->seekframe;//+of->lapready*2*od.path1->seqs[0].endframe;
				for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
					if (op!=of) {
						//b=op->seekframe;//+op->lapready*2*od.path1->seqs[0].endframe;
						if (op->ol_numflags > of->ol_numflags)
							p++;
					}
				}
				op=NULL;
			}
			p++; // okay this is what place your in 1 2 3 etc.

			if (od.signalfirst) { // 2nd place guy sends you your -neg first place diff time
				if (p==1) {
					if (od.difftimemode==1 && !od.fplace) {
						char str[50];
						od.tscoret[PTEXT_DIFFTIME]=120;
						od.cscoret[PTEXT_DIFFTIME]=20;
						getsctime(od.signalfirst,str);
						sprintf(od.scorestrings[PTEXT_DIFFTIME],"           %s",str);
					}
					od.signalfirst=0; // clear it
				}
			}
			if (p!=od.place || of->finplace) {
				od.place=p; // this is the final place 1 2 3 etc.
/*			if (of->finplace)
					od.tscoret[PTEXT_PLACE]=10000;
				else
					od.tscoret[PTEXT_PLACE]=100;
				od.cscoret[PTEXT_PLACE]=0;
				if (p==od.ol_numplayers)
					sprintf(od.scorestrings[PTEXT_PLACE],"   LAST");
				else {
					if (p>=OL_MAXPLAYERS)
						p=OL_MAXPLAYERS;
					sprintf(od.scorestrings[PTEXT_PLACE],"   %s",placestrs[p-1]);
				}*/
			}
		}
#endif
	} else {
// calc score for prebuilt, paths
		if (!of->dofinish) {
			if (of->ol_airtime==0 && od.wrongway>300) {
/*				if (od.tscoret[PTEXT_WRONGWAY]==0) {
					od.tscoret[PTEXT_WRONGWAY]=20;
					od.cscoret[PTEXT_WRONGWAY]=0;
					od.wrongway=300;
				}
				strcpy(od.scorestrings[PTEXT_WRONGWAY],"     WRONG WAY"); */
			} else if (of->clocktickcount>=INITCANTSTARTDRIVE) { // only when not showing "waiting for OTHER PLAYER to arrive"
				od.cscoret[PTEXT_WRONGWAY]=0;
				od.tscoret[PTEXT_WRONGWAY]=0;
			}
		}
		if (currule->rboxes) {
			int i;
			float mk,k;
			float mn,n;
			VEC delb,delp;
			if (od.numcars==1)
				od.place=1;
			else if (of->finplace)
				od.place=of->finplace;
			else {
				if (!of->boxchk)
					mn=10000;
				else if (!of->boxchk->prev[0])
					mn=-10000;
				else {
					int p=1;
					mn=of->boxchk->num;
					delb.x=of->boxchk->pos.x-of->boxchk->prev[0]->pos.x;
					delb.y=of->boxchk->pos.y-of->boxchk->prev[0]->pos.y;
					delb.z=of->boxchk->pos.z-of->boxchk->prev[0]->pos.z;
					delp.x=of->pos.x-of->boxchk->prev[0]->pos.x;
					delp.y=of->pos.y-of->boxchk->prev[0]->pos.y;
					delp.z=of->pos.z-of->boxchk->prev[0]->pos.z;
					mk=dot3d(&delp,&delb)/dot3d(&delb,&delb);
					for (i=0;i<od.numcars;i++) {
						if (i!=of->carid) {
							if (!opa[i].boxchk || opa[i].finplace) {
//								n=10000;
								p++;
							} else if (!opa[i].boxchk->prev[0]) {
//								n=-10000;
							} else {
								n=opa[i].boxchk->num;
								if (n==mn) {
									delb.x=opa[i].boxchk->pos.x-opa[i].boxchk->prev[0]->pos.x;
									delb.y=opa[i].boxchk->pos.y-opa[i].boxchk->prev[0]->pos.y;
									delb.z=opa[i].boxchk->pos.z-opa[i].boxchk->prev[0]->pos.z;
									delp.x=opa[i].pos.x-opa[i].boxchk->prev[0]->pos.x;
									delp.y=opa[i].pos.y-opa[i].boxchk->prev[0]->pos.y;
									delp.z=opa[i].pos.z-opa[i].boxchk->prev[0]->pos.z;
									k=dot3d(&delp,&delb)/dot3d(&delb,&delb);
									if (k>mk)
										p++;
								} else {
									if (n>mn)
										p++;
								}
							}
						}
					}
					od.place=p;
				}
			}
		}
		if (od.numcars>1 && od.path1) {
//	if (od.ol_numplayers>1 && od.path1 && od.ol_framecount>300) {
			int p=0;
			float a,b;
			if (of->finplace)
				p=of->finplace-1;
			else {
				a=of->seekframe2+of->laps2*(od.trackend+1);//+of->lapready*2*od.path1->seqs[0].endframe;
//				a=of->seekframe;//+of->lapready*2*od.path1->seqs[0].endframe;
				for (i=0,op=&opa[0];i<od.numcars;i++,op++) {
					if (op!=of) {
						b=op->seekframe2+op->laps2*(od.trackend+1);
//						b=op->seekframe;//+op->lapready*2*od.path1->seqs[0].endframe;
						if (b>a)
							p++;
					}
				}
				op=NULL;
			}
			p++; // okay this is what place your in 1 2 3 etc.
			od.place=p;
		}
		if (!of->finplace && od.place<=od.fplace) // if you didn't finish and opponent finished and place is wrong, fix
			od.place=od.fplace+1;
#if 0
		if (p!=od.place || of->finplace) {
			od.place=p; // this is the final place 1 2 3 etc.
/*			if (of->finplace)
				od.tscoret[PTEXT_PLACE]=10000;
			else
				od.tscoret[PTEXT_PLACE]=100;
			od.cscoret[PTEXT_PLACE]=0;
			if (p==od.ol_numplayers)
				sprintf(od.scorestrings[PTEXT_PLACE],"   LAST");
			else {
				if (p>=OL_MAXPLAYERS)
					p=OL_MAXPLAYERS;
				sprintf(od.scorestrings[PTEXT_PLACE],"   %s",placestrs[p-1]);
			}*/
		}
#endif
		if (od.signalfirst) { // 2nd place guy sends you your -neg first place diff time
			if (od.place==1) {
				if (od.difftimemode==1 && !od.fplace) {
					char str[50];
					od.tscoret[PTEXT_DIFFTIME]=120;
					od.cscoret[PTEXT_DIFFTIME]=20;
					getsctime(od.signalfirst,str);
					sprintf(od.scorestrings[PTEXT_DIFFTIME],"           %s",str);
				}
				od.signalfirst=0; // clear it
			}
		}
	}
// print fancy text
//	if (testfont && od.ol_uedittrackname[0]!='\0') {
//		for (i=0;i<PTEXT_MAXTEXTSTRINGS;i++)
//			if (od.tscoret[i])
//				break;
//		if (i!=PTEXT_MAXTEXTSTRINGS) {
//		if (1) {
// to disable doppler
	if (od.bigmove>0)
		od.bigmove--;
	od.spacebartimer-=od.ol_uloop;
//	logger("sbt %d, uloop %d\n",od.spacebartimer,od.ol_uloop);
	if (od.spacebartimer<0)
		od.spacebartimer=0;
// steady display of place 1st,2nd etc.
	if (of->clocktickcount>=INITCANTSTARTDRIVE) {
//		od.jplacetimer=0;
		if (od.jplacetimer==0 || of->dofinish) {
			int tt2;
//			int dx=48,dy=16;
//			dx=od.testint1;
//			dy=od.testint2;
			tt2=range(1,od.place,od.numcars);
			if (od.jplace!=tt2) {
				od.jplacetimer=20;
				od.jplace=tt2;
//				if (od.jplace>=1) {
//					if (currule->useweap) {
/*						b16=locktexture(od.splacetex);
						if (!od.jplacesave) {
							od.jplacesave=bitmap16alloc(od.jplacebm[0]->x,od.jplacebm[0]->y,-1);
							clipblit32(b16,od.jplacesave,dx,dy,0,0,od.jplacebm[0]->x,od.jplacebm[0]->y);
						} else
//						cliprect32(b16,0,0,b16->x-1,b16->y-1,0);
							clipblit32(od.jplacesave,b16,0,0,dx,dy,b16->x,b16->y);
						if (od.jplace<=MAXJPLACE)
							clipxpar16(od.jplacebm[od.jplace-1],b16,0,0,dx,dy,b16->x,b16->y);
//						cliprect32(b16,0,0,b16->x-1,b16->y-1,0xffff);
						unlocktexture(od.splacetex); */
//					}
//				}
			}
		} else
			od.jplacetimer--;
	}
}

void onlineracemain_draw3d()
{
	dolights();
	video_setviewport(&mainvp);
	video_drawscene(mainvproottree);
//	video_setviewport(&mainvp2);
//	video_drawscene(mainvp2roottree);
	addnullcleanup();
/*
}

void onlineracemain_draw2d()
{
*/
//	video_lock();
// now draw stuff directly to the screen (sorta)
// show animated text array

	video_sprite_begin(
	1024,768,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0 // vpflags see d2_viewport.h, use VP_CLEARBG if no 3d called (no setviewports)
	//C32 backcolor=C32BLACK); // if flags has VP_CLEARBG
	);
	if (opa[0].clocktickcount>0)
		updatescoreline(of);
#if 0
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0 // vpflags see d2_viewport.h, use VP_CLEARBG if no 3d called (no setviewports)
	//C32 backcolor=C32BLACK); // if flags has VP_CLEARBG
	);

	S32 i,j;
	for (i=0;i<PTEXT_MAXTEXTSTRINGS;i++) {
		int sx;
		sx=0;
		bool ddr=true;
		if (od.tscoret[i]) {
			od.cscoret[i]+=.5f;
			if (od.cscoret[i]<20) {
				if ((int)(od.cscoret[i])&4) {
					sx=20*FONTWIDTH;
					ddr=false;
				}
			} else if (od.cscoret[i]>od.tscoret[i]) {
				sx=FONTWIDTH*20;
				od.tscoret[i]=0;od.cscoret[i]=0;
				ddr=false;
			} else if (od.cscoret[i]+20>od.tscoret[i])
				sx=int((od.cscoret[i]+20-od.tscoret[i])*FONTWIDTH);
//			else
//			ol_drawstring(od.ol_tscorefont1,B32,od.scorestrings[i],WX-20*FONTWIDTH+sx,FONTHEIGHT*i);
//			od.hugefont->print(float(WX-20*FONTWIDTH+sx),float(FONTHEIGHT*i),200,8,F32WHITE,od.scorestrings[i]);
			if (ddr)
				od.hugefont->print(float(320-20*FONTWIDTH+sx),float(FONTHEIGHT*i),200,8,F32LIGHTMAGENTA,od.scorestrings[i]);
//			if (i==4)
//				ol_drawstring2(od.ol_tscorefont3,B32,od.name1,WX-20*16,4*FONTHEIGHT,224);
//			else if (i==5)
//
//				ol_drawstring2(od.ol_tscorefont3,B32,od.name2,WX-20*16,5*FONTHEIGHT,224);
		}
	}
// show finishers and their times
	if (of->finplace) {
		char name[100];
		for (i=0;i<od.numcars;i++) {
			j=opa[i].finplace;
			if (j) {
				j--;
				strcpy(name,carstubinfos[i].name);
				name[14]='\0';
				my_strupr(name);
				if (of!=&opa[i] || (of->clocktickcount&32))
//					ol_drawstring2(od.ol_tscorefont3,B32,name,WX-20*16,(j+4)*FONTHEIGHT,224);
					od.hugefont->print(float(50),float(FONTHEIGHT*(j+4)),100,8,F32GREEN,name);
				if (currule->ctf)
					sprintf(name,"%2d",opa[i].ol_numflags);
//					getsctimesctf(opa[i].dofinish-INITCANTSTARTDRIVE,name);
				else
					getsctimes(opa[i].dofinish-INITCANTSTARTDRIVE,name);
//				ol_drawstring(od.ol_tscorefont1,B32,name,WX-5*FONTWIDTH,FONTHEIGHT*(j+4));
				od.hugefont->print(float(320-100),float(FONTHEIGHT*(j+4)),100,8,F32LIGHTGREEN,name);
			}
		}
	}
//JAY
	// current rank table stuff, used to be used only after finish
	//if (of->finplace) {
	if (currule->ctf) {
		char name[128];
		for (i=0;i<od.numcars;i++) {
			int xloc,yloc;
			int p = 0;

			for (j=0;j<od.numcars;j++) {
				// update rank table slot
				p += (opa[j].ol_numflags > opa[i].ol_numflags) || ((opa[j].ol_numflags == opa[i].ol_numflags) && (i > j));
			}
			strcpy(name,carstubinfos[i].name);
			name[14]='\0';
			my_strupr(name);
			//if (of!=&opa[i] || (of->clocktickcount&32))
			xloc=WX-126;// - p*WX/80;
//			xloc=WX-WX/5;// - p*WX/80;
			yloc=WY/32 + p*(2*FONTHEIGHT/3);
//			xloc=WX-WX/5+WX/128 - p*WX/80;
//			yloc=WY - WY/3 - WY/48 + (p+4)*FONTHEIGHT;
			ol_drawstring2(od.ol_tscorefont3,B32,name,xloc,yloc,224);
			sprintf(name,"%2d",opa[i].ol_numflags);
			ol_drawstring(od.ol_tscorefont3,B32,name,xloc + FONTWIDTH*5,yloc);

			// draw radar blip while we're at it
//			drawradarblip(&opa[i].pos, &of->pos, B32, (&opa[i] == of) ? C32WHITE : C32YELLOW); // also works for 555
//			drawradarblip(&opa[i].pos, &of->pos, B32, (&opa[i] == of) ? 0xffff : 0xf800);
		}
		// flag blip
//		if ( od.ol_flagidx >= 0 ) drawradarblip(&od.ol_flags[od.ol_flagidx].pos, &of->pos, B32, C32LIGHTRED);
//		if ( od.ol_flagidx >= 0 ) drawradarblip(&od.ol_flags[od.ol_flagidx].pos, &of->pos, B32, 0x07e0);
	}
//	i=od.jplace-1;
//	if (i>=0 && i<MAXJPLACE)
//		clipxpar16(od.jplacebm[i],B32,0,0,od.weapbm->x,0,od.jplacebm[i]->x,od.jplacebm[i]->y);
//				clipblit32(od.jplacebm[i],B32,0,0,WX-od.jplacebm[i]->x,0,od.jplacebm[i]->x,od.jplacebm[i]->y);
//			updateweapdisplay();

//	if (!od.name1[0] && !od.name2[0])
//	drawweapdisplay();
//	if (currule->useweap==0) {


// show place
	i=od.jplace-1;
	if (i>=0 && i<MAXJPLACE) {
#if 0
//		struct bitmap32 *b32;
//		b32=od.jplacebm[i];
		if (currule->ctf)
			clipxpar32(b32,B32,0,0,(WX-b32->size.x)>>1,WY-b32->size.y,b32->size.x,b32->size.y,C32BLACK);
		else
			clipxpar32(b32,B32,0,0,(WX-b32->size.x)>>1,0,b32->size.x,b32->size.y,C32BLACK);
#else
		video_sprite_draw(od.jplacespt[i],F32WHITE,float((320-30)>>1),5,30,30);
	}
// direction finder
	video_sprite_draw(od.cface ,pointf3x(1,1,1,.5f), 320-24, 240-24, 0, 0, 24.0f/49.0f, 24.0f/49.0f,of->ch);
	video_sprite_draw(od.carrow,F32WHITE, 320-24, 240-24, 0, 0,  3.0f/ 7.0f, 15.0f/22.0f,of->dh);
#endif
// speed
	static S32 ospeedf;
	if (of->ol_airtime==0) {
		float spdf=fabs(of->ol_accelspin/of->ol_startaccelspin);
		ospeedf=S32(150*spdf);
	}
	od.hugefont->print(320-48,240-60,48,10,F32WHITE,"%3d",ospeedf);
// clock
	S32 tt,m,s;
	if (of->clocktickcount<INITCANTSTARTDRIVE)
		tt=0;
	else {
		if (of->dofinish)
			tt=of->dofinish-INITCANTSTARTDRIVE;
		else
			tt=of->clocktickcount-INITCANTSTARTDRIVE;
		tt/=60;
		tt=range(0,tt,99*60-1);
	}
	m=tt/60;
	s=tt%60;
	od.hugefont->print(320/2-38/2,240-10,38,10,F32WHITE,"%2d:%02d",m,s);
//	}
//	video_unlock();
//		}
//	}
// debug console
#ifdef USECON
/*	if (od.showcon)
		conb16=con16_getbitmap16(od.ucon);
	if (teststr0[0] || teststr1[0] || teststr2[0] || conb16) { // generic debug string
		video_lock();
		if (conb16)
			clipblit32(conb16,B32,0,0,8,8,WX,WY);
		ol_drawstring(od.ol_tscorefont1,B32,teststr0,10,10);
		ol_drawstring(od.ol_tscorefont1,B32,teststr1,10,25);
		ol_drawstring(od.ol_tscorefont1,B32,teststr2,10,40);
		video_unlock();
	} */
#endif
#endif
#if 1
// rematch logic
	if (od.nobotrematch && od.quittimer>0 && od.quittimer*10<od.quittime) {
		od.quittimer=29*od.quittime/30+10;
	}
	if (od.quittimer<5*60) {
//	if (100*od.quittimer<od.quittime) {
//		setresvis(game_rl,PBACK1,0);
//		setresvis(game_rl,PTEXTQ,0);
//		setresvis(game_rl,PBUTYES,0);
//		setresvis(game_rl,PTEXTQ2,0);
//		setresvis(game_rl,PBUTNO,0);
/*		PTEXTQ->setvis(0);
		PTEXTQ2->setvis(0);
		PBUTYES->setvis(0);
		PBUTNO->setvis(0); */
		setrematchvis(0);
		od.sentrematch=0;
		od.resvis=0;
//		if (video_maindriver!=VIDEO_GDI || mousemode==MM_INFINITE || usedirectinput)
		if (videoinfo.video_fullscreen || wininfo.mousemode==MM_INFINITE || wininfo.usedirectinput)
			showcursor(0);
		od.uplaycursor=0;
	} else if (30*od.quittimer<29*od.quittime && !od.sentrematch) {
/*		if (game_rl) {
			int d,i;
			d=(WX-5)-(game_rl->resbox[0].x+game_rl->resbox[0].xs);
			for (i=0;i<game_rl->nresbox;i++) {
				game_rl->resbox[i].x+=d;
			}
			if (!od.resvis && !od.quitting) {
				setmousexy(game_rl->resbox[1].x+game_rl->resbox[1].xs/2,
					game_rl->resbox[1].y+game_rl->resbox[1].ys/2);
			}
		} */
//		if (video_maindriver!=VIDEO_GDI || mousemode==MM_INFINITE || usedirectinput)
			od.uplaycursor=1;
		showcursor(1);
//		setresvis(game_rl,PBACK1,1);
//		setresvis(game_rl,PTEXTQ,1);
//		setresvis(game_rl,PBUTYES,1);
//		setresvis(game_rl,PTEXTQ2,1);
//		setresvis(game_rl,PBUTNO,1);
/*		PTEXTQ->setvis(1);
		PTEXTQ2->setvis(1);
		PBUTYES->setvis(1);
		PBUTNO->setvis(1); */
		setrematchvis(1);
		od.resvis=1;

		if (of->cntl==CNTL_AI && od.nobotrematch==0) {
			od.sentrematch=1;
//			if (video_maindriver!=VIDEO_GDI || mousemode==MM_INFINITE || usedirectinput)
			if (videoinfo.video_fullscreen || wininfo.mousemode==MM_INFINITE || wininfo.usedirectinput)
				showcursor(0);
			od.uplaycursor=0;
//			setresvis(game_rl,PBACK1,0);
//			setresvis(game_rl,PTEXTQ,0);
//			setresvis(game_rl,PBUTYES,0);
//			setresvis(game_rl,PTEXTQ2,0);
//			setresvis(game_rl,PBUTNO,0);
/*			PTEXTQ->setvis(0);
			PTEXTQ2->setvis(0);
			PBUTYES->setvis(0);
			PBUTNO->setvis(0);
*/
			setrematchvis(0);
			od.resvis=0;
//			if (od.playerid==0) // do both
				++od.rematchplayers;
//			else
				requestrematch();
		}

	} else {
		od.uplaycursor=0;
	if (videoinfo.video_fullscreen || wininfo.mousemode==MM_INFINITE || wininfo.usedirectinput)
//	if (video_maindriver!=VIDEO_GDI || mousemode==MM_INFINITE || usedirectinput)
			showcursor(0);
//		setresvis(game_rl,PBACK1,0);
//		setresvis(game_rl,PTEXTQ,0);
//		setresvis(game_rl,PBUTYES,0);
//		setresvis(game_rl,PTEXTQ2,0);
//		setresvis(game_rl,PBUTNO,0);
/*		PTEXTQ->setvis(0);
		PTEXTQ2->setvis(0);
		PBUTYES->setvis(0);
		PBUTNO->setvis(0); */
		od.resvis=0;
		setrematchvis(0);
	}
	if (/*od.resvis &&*/ game_rl) {
//		video_lock();
//		drawres(game_rl);
		game_rl->draw3d(); // not right now // NYI
//		video_unlock();
	}
	if (od.uplaycursor) {
//		video_lock();
//		clipcircle16(B32,MX,MY,3,0);
//		clipxpar32(od.cursorpic,B32,0,0,MX,MY,od.cursorpic->size.x,od.cursorpic->size.y,C32BLACK);
//		video_unlock();
	}
// junk
//	od.ol_framecount+=od.ol_uloop; // 1/60 sec
//	testheight+=.01f;
//	if (testheight>20.0f)
//		testheight=0.0f;

/*    if(DoCapture == 1)
	{
      testcapturefrm(CurrentFrame);
	  CurrentFrame++;
	} */
#endif
#if 1
	video_sprite_end();

#endif
}

void onlineracemain_exit()
{
	delete od.parts;
//	od.weapxlate=0;
	delete tinf;
	tinf=0;
	exit_res3d();
	free_envv2tex();
	delete newcarnames;
	newcarnames=0;
	if (wininfo.closerequested) {
		if (matchcl) {
			freesocker(matchcl);
			matchcl=0;
		}
	}
	cgi.isvalid=false;
	wininfo.runinbackground=runinbackgroundsave;
//	delete od.hugefont;
//	od.hugefont=0;
	//	extern int ol_grandtotaltrickpoints,ol_MyGoldCoins;
	//	extern int ol_totalcrashs,ol_selectedtrack;
	//	extern int ol_selected;
//	pointf3 t;
	int i,j;
	addnullexit();
//	FILE *fw;
//	pushandsetdir("packetlog");
//	fw=fopen("logfile.txt","w");
//	popdir();
//	if (fw)
//		for (i=0;i<od.ol_numplayers;i++)
//			for (j=0;j<od.playernet[i].ninput;j++)
//				fprintf(fw,"packetlog p %d k %d ts %d\n",i,od.playernet[i].inputs[j],od.playernet[i].timestamps[j]);
//
//	fclose(testwrite);
	if (crashinfos.editenable)
		crasheditsave();
	if (checkinfos.editenable)
		checkeditsave();
//	if (botaiinfos.editenable)
//		boteditsave();
	if (currule->rboxes)
		boxai_exit();
	else
		freecheckresets();
	coinsexit();
//	logviewport(&mainvp,OPT_SOME);
	logger("logging roottree\n");
	mainvproottree->log2();
	logger("logging reference lists\n");
	logrc();
//	logviewport(&mainvp2,OPT_SOME);
//	freeres(game_rl);
	delete game_rl;
	game_rl=NULL;
#ifdef USEVARCHECKERrace
	freevarchecker();
#endif
	extradebvars(NULL,0);
//	of->ol_utimeleft=of->ol_clocktickcount/60; // convert to seconds
	od.ol_selectedtrack=7;
	if (od.ol_selectedtrack<7 || od.ol_selectedtrack>10)
		od.ol_selectedtrack=7;
//	of->ol_totalcrashs=of->ol_utotalcrashes;
//	of->ol_MyGoldCoins=of->ol_ucoinscaught=od.ol_ncoinscaught;
//	of->ol_grandtotaltrickpoints=of->ol_utotalstunts;

	ol_freefinishbursts();
//	for (i=0,op=&opa[0];i<od.ol_numplayers;i++,op++) {
//		for (j=0;j<4;j++) {
//			freetree(op->ol_ubursta[j]);
//			freetree(op->ol_uburstb[j]);
//		}
//	}
	freetree(od.mtrail);
	freetree(od.chemtrail);
	freetree(od.stasisfield);
#if 0
	for (i=0,op=&opa[0];i<od.numcars;i++,op++)
		freetsp(op->ol_flashtsp);
#endif
	op=NULL;
	if (od.ol_condomtsp)
		freetsp(od.ol_condomtsp);
	if (!od.ol_slowpo && od.ol_specialtsp)
		freetsp(od.ol_specialtsp);
	for (j=0;j<4;j++)
		if (od.ol_bursttsp[j])
			freetsp(od.ol_bursttsp[j]);
	ol_ufreeparticles();
	st2_freecollgrids();
	// after defaultexit, no trees are alloced
//	if (op->ol_nulightnings)
//		op->ol_nulightnings=op->ol_nubursts=op->ol_nucondoms=op->ol_nuflames=0;
	if (od.ol_flametsp)
		freetsp(od.ol_flametsp);
//	if (od.ol_speedotsp)
//		freetsp(od.ol_speedotsp);
	if (od.weapicontsp)
		freetsp(od.weapicontsp);
//JAY
//	if (!currule->ctf) {
//		if (od.ol_clocktsp)
//			freetsp(od.ol_clocktsp);
//	}
	if (od.tlighttsp)
		freetsp(od.tlighttsp);
	wininfo.fpswanted=30;
	freetreepool();
	freetree(mainvproottree);
//	freetree(mainvp2roottree);
//	freetree(od.ol_scrline);
	ol_exittags();
	showcursor(1);
//	free_sagger(od.ol_sag);
	wave_unload(od.ol_sag);
//	usescncamera=0;

	//  FreeTSP(scorefont,scoreframes);
	freetsp(od.ol_tscorefont1);
	freetsp(od.ol_tscorefont2);
	freetsp(od.ol_tscorefont3);
	freescoreline();
/*	bitmap32free(od.jplacebm[0]);
	bitmap32free(od.jplacebm[1]);
	bitmap32free(od.jplacebm[2]);
	bitmap32free(od.jplacebm[3]); */
/*	textureb::rc.deleterc(od.jplacespt[0]);
	textureb::rc.deleterc(od.jplacespt[1]);
	textureb::rc.deleterc(od.jplacespt[2]);
	textureb::rc.deleterc(od.jplacespt[3]); */
//	textureb::rc.deleterc(od.cface);
//	textureb::rc.deleterc(od.carrow);
//	if (od.jplacesave)
//		bitmap16free(od.jplacesave);
//	if (od.weapdescsave)
//		bitmap16free(od.weapdescsave);
//	od.jplacesave=NULL;
//	od.weapdescsave=NULL;
	bitmap32free(od.weapbm);
	if (od.oldstarttex)
		bitmap32free(od.oldstarttex);
//	bitmap32free(od.cursorpic);
//	for (j=0;j<MAX_WEAPKIND;j++)
//		bitmap16free(od.weapicons[j]);
//	wininfo.joyenable=0;
//	mousemode=MM_NORMAL;
	C32 ct=mainvp.backcolor;
//	memset(&mainvp,0,sizeof(mainvp));
	mainvp.backcolor=ct;
	if (od.udp)
		freesocker(od.udp);
	for (j=0;j<OL_MAXPLAYERS;j++) {
		if (od.playernet[j].tcpclients) {
			freesocker(od.playernet[j].tcpclients);
			od.playernet[j].tcpclients=NULL;
		}
	if (od.numplayers!=1) {
//			if (od.playernet[j].alttcpclients) {
//				freesocker(od.playernet[j].alttcpclients);
//				od.playernet[j].alttcpclients=NULL;
//			}
		}
		gameinfoclear(); // tell match server
		if (od.master)
			freesocker(od.master);
//		if (od.altmaster)
//			freesocker(od.altmaster);
		od.master=/*od.altmaster=*/NULL;
		if (!matchcl)
			uninitsocker();
	}
	for (j=0;j<OL_MAXPLAYERS;j++) {
		if (od.cartexsave[j])
			bitmap32free(od.cartexsave[j]);
		od.cartexsave[j]=NULL;
		if (od.cardarktexsave[j])
			bitmap32free(od.cardarktexsave[j]);
		od.cardarktexsave[j]=NULL;
		od.cartex[j]=NULL;
		if (od.cartexsave2[j])
			bitmap32free(od.cartexsave2[j]);
		od.cartexsave2[j]=NULL;
		if (od.cardarktexsave2[j])
			bitmap32free(od.cardarktexsave2[j]);
		od.cardarktexsave2[j]=NULL;
		od.cartex2[j]=NULL;
	}

#ifdef USECON
//	con16_free(od.ucon);
#endif
	freeweaps();
	for (i=0;i<od.numcars;i++) {
		opa[i].hologram=NULL;
		opa[i].prism=NULL;
	}
	freetree(od.prism);
//	exithelper();
#if 0
	if (olracecfg.bluered3d) {
		bitmap32free(od.blueredbm);
		freeblueredmask();
	}
#endif
	logger("try to catch some mem leaks\n");
//	logviewport(NULL,OPT_TEXTURES|OPT_MODELS|OPT_MATERIALS);
//	mainvproottree->log2();
	od.minetex=NULL;
//	midi_unload();
	exitgstate();
	sound_free(od.backsound);
	od.backsound=0;
	wave_unload(od.backwave);
	od.backwave=0;
	videoinfodx9.fog.enable=false;
	popdir();
	delete ho;
}

void ol_usenewcam()
{
	if (op==of)
		od.ol_useoldcam=0;
}

void spawnmtrail(pointf3* loc)
{
	tree2 *u;
	pointf3 t;
	u=duptree(od.mtrail);
	u->trans=*loc;
//	u->treealphacutoff=10;
	u->userint[3]=320; // time to live
	u->treecolor.x=.8f+.2f*mt_frand();
	u->treecolor.y=.8f+.2f*mt_frand();
	u->treecolor.z=mt_frand();
//	u->userint[3]=40+mt_random(80); // time to live
	u->transvel.x=mt_frand()*.0005f-.00025f;
	u->transvel.y=mt_frand()*.0005f-.00025f;
	u->transvel.z=mt_frand()*.0005f-.00025f;
	t.x=t.y=0;
	t.z=1;
	t.w=mt_frand()*PI*2;
	rotaxis2quat(&t,&t);
	u->rot=t;
	linkchildtoparent(u,od.ol_root);
}

void spawnstasisfield(pointf3* loc)
{
	tree2 *u;
	u=duptree(od.stasisfield);
	u->trans=*loc;
//	u->treealphacutoff=10;
	u->userint[3]=15; // time to live
//	u->userint[3]=40+mt_random(80); // time to live
	linkchildtoparent(u,od.ol_root);
}

void spawnchemtrail(pointf3* loc)
{
	tree2 *u;
	pointf3 t;
	u=duptree(od.chemtrail);
	u->trans=*loc;
//	u->treealphacutoff=10;
	u->userint[3]=320; // time to live
//	u->userint[3]=40+mt_random(80); // time to live
	u->transvel.x=mt_frand()*.002f-.001f;
	u->transvel.y=mt_frand()*.002f-.001f;
	u->transvel.z=mt_frand()*.002f-.001f;
	t.x=t.y=0;
	t.z=1;
	t.w=mt_frand()*PI*2;
	rotaxis2quat(&t,&t);
	u->rot=t;
	linkchildtoparent(u,od.ol_root);
}

void spawnc2ceffect(pointf3* loc,pointf3* norm,float mag,pointf3* vel)
{
	int i;
	tree2 *u;
	pointf3 yup={0,1,0,0},q2;
	float ang,mag2;//,dt;
	pointf3 crs,out;
	if (od.intimewarp)
		return;
//	addnull(loc,NULL);
//	logger("in spawnc2ceffect with norm %f %f %f mag %f\n",norm->x,norm->y,norm->z,mag);
	cross3d(&yup,norm,&crs);
//	dt=dot3d(&yup,norm);
	if (!normalize3d(&crs,&crs))
		crs=pointf3x(1,0,0);
//	crs.w=racos(dt);
//	logger("cross at %f %f %f\n",crs.x,crs.y,crs.z);
//	rotaxis2quat(&crs,&q);
//	logger("q at %f %f %f %f\n",q.x,q.y,q.z,q.w);
	for (i=0;i<20;i++) {
		ang=2*i*PI/20;
		q2=*norm;
		q2.w=ang;
		rotaxis2quat(&q2,&q2);
//		logger("q2 at %f %f %f %f\n",q2.x,q2.y,q2.z,q2.w);
//		quattimes(&q2,&q,&q2);
//		logger("q2t at %f %f %f %f\n",q2.x,q2.y,q2.z,q2.w);
		quatrot(&q2,&crs,&out);
//		logger("out at %f %f %f\n",out.x,out.y,out.z);
		u=duptree(od.ol_fwork[3]);
//		u=duptree(od.ol_fwork[mt_random(4)]);
		u->trans=*loc;
//		u->trans.x+=(i-10)*.125f;
		mag2=mag*(mt_frand()*.2f+.9f);
		u->transvel.x=mag2*out.x*.01f+vel->x*od.ol_uloop;
		u->transvel.y=mag2*out.y*.01f+vel->y*od.ol_uloop;
		u->transvel.z=mag2*out.z*.01f+vel->z*od.ol_uloop;
//		u->treealphacutoff=10;
		u->userproc=ol_burstfinishproc2;
		u->treecolor.w=.5f;
		u->userint[2]=20+mt_random(40); // time to live
		u->flags|=TF_ALWAYSFACING;
		u->scale=pointf3x(SPARKSCALE,SPARKSCALE,SPARKSCALE);
		linkchildtoparent(u,od.ol_root);
//		freetree(u);
	}
}

void openc2ceffect()
{
}

void closec2ceffect()
{
}
