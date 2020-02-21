#define RES3D // alternate shape class
#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "carclass.h"
#include "utils/json.h"
#include "hash.h"
#include "tracklist.h"
#include "netdescj.h"
#include "constructor.h"
#include "timewarp.h"
#include "envj.h"
#include "avaj.h"
#include "gamedescj.h"
#include "system/u_states.h"

#include "match_logon.h"

// 3d
struct viewport2 lobby_viewport,track_viewport; // for debvars
static tree2* lobby_viewport_roottree,*track_viewport_roottree;  // for 3d drawing

// pass to constructors
string lobby_track; // for constructors, make global

// ui
static shape *rl; // root
static shape* focus; // current
static listbox* ltrack;
static text* ttrack;
static listbox* lweaphave;
static listbox* lweapavail;
static listbox* lrules;
static text* trules;
static listbox* lcarbody;
static listbox* lpaint;
static listbox* ldecal;
static edit* ename;
static text* tname;
static text* thname;
static text* thost;
static edit* eip;
static text* tenergyused[MAX_ENERGIES];
static hscroll* henergy[MAX_ENERGIES];
static text* tenergy[MAX_ENERGIES];
static text* tnump;
static hscroll* hnump;
static text* tnumb;
static hscroll* hnumb;
static pbut* bnewconstructor;
static pbut* boldconstructor;
static pbut* bbot;
static pbut* bclient;
static pbut* bserver;
static pbut* bmainmenu;
static pbut* bcancel;
static listbox* lyourip;
static text* tip;
static listbox* lip;
static pbut* bipdel;
static hscroll* hnlaps;
static text* tnlaps;

static const S32 LOBBYSPTRESX = 800;
static const S32 LOBBYSPTRESY = 600;

static const char *tenergyusedresname[]={
	"TEXTUFUSION",
	"TEXTUELECTRICAL",
	"TEXTUCYBER",
	"TEXTUGEOTHERMAL",
	"TEXTUPARTICLE",
	"TEXTUCHEMICAL",
};
static const char *henergyresname[]={
	"SLIDEFUSION",
	"SLIDEELECTRICAL",
	"SLIDECYBER",
	"SLIDEGEOTHERMAL",
	"SLIDEPARTICLE",
	"SLIDECHEMICAL",
};
static const char *tenergyresname[]={
	"TEXTFUSION",
	"TEXTELECTRICAL",
	"TEXTCYBER",
	"TEXTGEOTHERMAL",
	"TEXTPARTICLE",
	"TEXTCHEMICAL",
};

// end json stuff
static struct menuvar lobbyvars[]={
	{"@lightred@---- lobby car cam -------------------",0,D_VOID,0},
	{"lobby cam trans x",&lobby_viewport.camtrans.x,D_FLOAT,FLOATUP/8},
	{"lobby cam trans y",&lobby_viewport.camtrans.y,D_FLOAT,FLOATUP/8},
	{"lobby cam trans z",&lobby_viewport.camtrans.z,D_FLOAT,FLOATUP/8},
	{"lobby cam rot x",&lobby_viewport.camrot.x,D_FLOAT,FLOATUP/8},
	{"lobby cam rot y",&lobby_viewport.camrot.y,D_FLOAT,FLOATUP/8},
	{"lobby cam rot z",&lobby_viewport.camrot.z,D_FLOAT,FLOATUP/8},
	{"lobby cam zoom",&lobby_viewport.camzoom,D_FLOAT,FLOATUP/8},
	{"@cyan@---- lobby track cam -------------------",0,D_VOID,0},
	{"track cam trans x",&track_viewport.camtrans.x,D_FLOAT,FLOATUP/8},
	{"track cam trans y",&track_viewport.camtrans.y,D_FLOAT,FLOATUP/8},
	{"track cam trans z",&track_viewport.camtrans.z,D_FLOAT,FLOATUP/8},
	{"track cam rot x",&track_viewport.camrot.x,D_FLOAT,FLOATUP/8},
	{"track cam rot y",&track_viewport.camrot.y,D_FLOAT,FLOATUP/8},
	{"track cam rot z",&track_viewport.camrot.z,D_FLOAT,FLOATUP/8},
	{"track cam zoom",&track_viewport.camzoom,D_FLOAT,FLOATUP/8},
};
static const S32 nlobbyvars=sizeof(lobbyvars)/sizeof(lobbyvars[0]);

#define TRACKPICX 345
#define TRACKPICY 490
#define TRACKPICW 140
#define TRACKPICH 94
#define CARVIEWX 150
#define CARVIEWY 40
#define CARVIEWW 400
#define CARVIEWH 300

static bitmap32 *trackpic;

static n_carclass* acar;
static float carspinang,wheelspinang,wheelsteerang;
static int wheelsteerdir=1;
static S32 paintlo=1,painthi=22;
static S32 decallo=0,decalhi=35;

// the main data structure
static class gamedescj* gd;

static void buildui()
{
// resource
	init_res3d((float)LOBBYSPTRESX,(float)LOBBYSPTRESY);
	S32 i;
pushandsetdirdown("carenalobby");
	rl = res_loadfile("carenalobbyres.txt");
	ltrack=rl->find<listbox>("LISTTRACK");
	ttrack=rl->find<text>("TEXTTRACK");
	lweaphave=rl->find<listbox>("LISTWEAPHAVE");
	lweapavail=rl->find<listbox>("LISTWEAPAVAIL");
	lrules=rl->find<listbox>("LISTRULES");
	trules=rl->find<text>("TEXTRULES");
	lcarbody=rl->find<listbox>("LISTCARBODY");
	lpaint=rl->find<listbox>("LISTPAINT");
	ldecal=rl->find<listbox>("LISTDECAL");
	ename=rl->find<edit>("EDITNAME");
	tname=rl->find<text>("TEXTNAME");
	thname=rl->find<text>("HOSTNAME");
	thost=rl->find<text>("TEXTHOST");
	eip=rl->find<edit>("EDITIP");
	boldconstructor=rl->find<pbut>("PBUTOLDCONSTRUCTOR");
	boldconstructor->setvis(1);
	bnewconstructor=rl->find<pbut>("PBUTNEWCONSTRUCTOR");
	bnewconstructor->setvis(0);
	hnump=rl->find<hscroll>("SLIDENUMP");
	tnump=rl->find<text>("TEXTNUMP");
	hnumb=rl->find<hscroll>("SLIDENUMPB");
	tnumb=rl->find<text>("TEXTNUMPB");
	bbot=rl->find<pbut>("PBUTBOT");
	bclient=rl->find<pbut>("PBUTCLIENT");
	bserver=rl->find<pbut>("PBUTSERVER");
	bmainmenu=rl->find<pbut>("PBUTMAINMENU");
	bcancel=rl->find<pbut>("PBUTCANCEL");
	lyourip=rl->find<listbox>("LISTYOURIP");
	lip=rl->find<listbox>("LISTIP");
	tip=rl->find<text>("TEXTIP");
	bipdel=rl->find<pbut>("PBUTIPDEL");
	for (i=0;i<MAX_ENERGIES;++i) {
		tenergyused[i]=rl->find<text>(tenergyusedresname[i]);
		henergy[i]=rl->find<hscroll>(henergyresname[i]);
		tenergy[i]=rl->find<text>(tenergyresname[i]);
	}
	tnlaps=rl->find<text>("TEXTNLAPS");
	hnlaps=rl->find<hscroll>("SLIDENLAPS");
// init vars
	hnlaps->setminmaxval(0,10);
	hnump->setminmaxval(1,MAX_PLAYERS);
	hnumb->setnumidx(MAX_PLAYERS+1); // 0,MAX_PLAYERS
	trackpic=bitmap32alloc(TRACKPICW,TRACKPICH,C32GREEN);
// load ips
	script* sc;
	if (fileexist("ips.txt")) {
		sc=new script("ips.txt");
		sc->sort();
		for (i=0;i<sc->num();i++)
			lip->addstring(sc->idx(i).c_str());
		delete sc;
	}
popdir();
	tracklist* tl=new tracklist();
	for (i=0;i<(S32)tl->size();++i)
		ltrack->addstring(tl->track_names[i].c_str());
	delete tl;
	ltrack->sort();
	sc=n_carclass::getcarlist();
	for (i=0;i<sc->num();i++)
		lcarbody->addstring(sc->idx(i).c_str());
	delete sc;
	for (i=0;i<MAX_WEAPKIND;i++)
		lweapavail->addstring(weapinfos[i]);
	for (i=0;i<MAX_RULES;i++)
		lrules->addstring(rule_names[i]);
	for (i=paintlo;i<=painthi;i++) {
		char str[50];
		sprintf(str,"Paint %02d",i);
		lpaint->addstring(str);
	}
	for (i=decallo;i<=decalhi;i++) {
		char str[50];
		sprintf(str,"Decal %02d",i);
		ldecal->addstring(str);
	}
	ltrack->setidxc(0);
	lrules->setidxc(0);
	lcarbody->setidxc(0);
	lpaint->setidxc(0);
	ldecal->setidxc(0);
//	if (true) {
	if (matchcl && cgi.isvalid) {
		lip->setvis(0);
		bipdel->setvis(0);
		ename->setvis(0);
		eip->setvis(0);
		tip->setvis(0);
		bbot->setvis(0);
//		if (true) {
		if (cgi.ishost) {
			bclient->setvis(0);
		} else {
			bserver->setvis(0);
			hnump->setvis(0);
			tnump->setvis(0);
			hnumb->setvis(0);
			tnumb->setvis(0);
			boldconstructor->setvis(0);
			lrules->setvis(0);
			trules->setvis(0);
			tnlaps->setvis(0);
			hnlaps->setvis(0);
			ltrack->setvis(0);
			ttrack->setvis(0);
		}
		tname->settname(username);
		thname->settname(hostname);
	} else {
		tname->setvis(0);
		thname->setvis(0);
		thost->setvis(0);
	}
}

static void struct2ui()
{
// load last settings
	S32 i,j;
	ename->settname(gd->a.playername.c_str());
	lrules->setidxc(gd->e.rules);
	eip->settname(ip2str(gd->n.serverip));
	if (lobby_track.empty()) {
		j=ltrack->findstring(gd->e.trackname.c_str());
	} else
		j=ltrack->findstring(lobby_track.c_str());
	if (j>=0)
		ltrack->setidxc(j);
	S32 n=gd->a.weaps.size();
	for (i=0;i<n;++i) {
		j=lweapavail->findstring(gd->a.weaps[i].c_str());
		if (j>=0) {
			lweaphave->addstring(gd->a.weaps[i].c_str());
			lweapavail->removeidx(j);
		}
	}
	j=lcarbody->findstring(gd->a.carbodyname.c_str());
	if (j<0)
		j=0; // if car cannot be found, set to first car in carlist
	lcarbody->setidxc(j);
	lpaint->setidxc(gd->a.paint-paintlo);
	ldecal->setidxc(gd->a.decal-decallo);
// set number of players in slider and text
	C8 str[100];
	S32 nl=gd->e.nlaps;
	if (nl==1)
		sprintf(str,"One Lap");
	else
		sprintf(str,"%d Laps",nl);
	hnlaps->setidx(nl);
	tnlaps->settname(str);

	S32 p=gd->nhumanplayers;
	if (p==1)
		sprintf(str,"One Human Player");
	else
		sprintf(str,"%d Human Players",p);
	tnump->settname(str);
	hnump->setidx(p);

	S32 bp=gd->nbotplayers;
	if (bp==1)
		sprintf(str,"One Bot Player");
	else
		sprintf(str,"%d Bot Players",bp);
	tnumb->settname(str);
	hnumb->setidx(bp);

	gd->a.energies.resize(MAX_ENERGIES,0);
	for (i=0;i<MAX_ENERGIES;i++) {
		henergy[i]->setnumidx(20+1);
		henergy[i]->setidx(gd->a.energies[i]);
		sprintf(str,"%s %d",energy_names[i],gd->a.energies[i]);
		tenergy[i]->settname(str);
	}
}

static void ui2struct()
{
	gd->a.playername=ename->gettname();
	gd->a.carbodyname=lcarbody->getidxname();
	gd->n.serverip=str2ip(eip->gettname());
	gd->a.paint=lpaint->getidx()+paintlo;
	gd->a.decal=ldecal->getidx()+decallo;
	S32 i,n=lweaphave->getnumidx();
	gd->a.weaps.clear();
	for (i=0;i<n;i++)
		gd->a.weaps.push_back(lweaphave->getidxname(i));
	gd->e.rules=lrules->getidx();
	gd->e.nlaps=hnlaps->getidx();
	gd->nhumanplayers=hnump->getidx();
	gd->nbotplayers=hnumb->getidx();
	gd->e.trackname=ltrack->getidxname();
	lobby_track=gd->e.trackname; // keep track constructors happy
}

static void loadtrackpic()
{
	struct bitmap32 *pic;
	char str[300];
	const char *p;
	p=ltrack->getidxname();
	logger("found track '%s'\n",p);
	sprintf(str,"tracks/%s",p);
	pushandsetdirdown(str);
	sprintf(str,"%s.jpg",p);
	if (fileexist(str)) {
		pic=gfxread32(str);
		clipscaleblit32(pic,trackpic);
		bitmap32free(pic);
	} else
		cliprect32(trackpic,0,0,trackpic->size.x,trackpic->size.y,C32DARKGRAY);
	popdir();
}

static void changecar()
{
	delete acar;
	const C8* p=lcarbody->getidxname();
	acar=new n_carclass(p,lpaint->getidx()+paintlo,ldecal->getidx()+decallo);
	lobby_viewport_roottree->linkchild(acar->getcartree());//linkchildtoparent(acarnull,);
}

tree2* gettrackscene() // ui wants a track change
{
	gd->e.trackname=ltrack->getidxname(); // struct
	gd->e.buildtrackhashtree(); // struct with *trkt or newtrack
	gd->e.oldtrackj->studyforks();
	return gd->e.oldtrackj->root;
}

static void init3d()
{
	lobby_viewport_roottree=new tree2("roottree");
//	load scene
	lightinfo.uselights=1;
	lightinfo.dodefaultlights=1;
// setup viewport
	lobby_viewport.backcolor=C32BLUE;
	lobby_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	lobby_viewport.xstart=CARVIEWX;
	lobby_viewport.ystart=CARVIEWY;
	lobby_viewport.xres=CARVIEWW;
	lobby_viewport.yres=CARVIEWH;
	lobby_viewport.zfront=.1f;
	lobby_viewport.zback=2000;
	lobby_viewport.xsrc=4; lobby_viewport.ysrc=3;
	lobby_viewport.useattachcam=false;
//	lobby_viewport.lookat=0;

	track_viewport.backcolor=C32BLUE;
	track_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	track_viewport.xstart=TRACKPICX;
	track_viewport.ystart=TRACKPICY;
	track_viewport.xres=TRACKPICW;
	track_viewport.yres=TRACKPICH;
	track_viewport.zfront=2.0f;
	track_viewport.zback=2000;
	track_viewport.xsrc=4;track_viewport.ysrc=3;
	track_viewport.useattachcam=false;
//	track_viewport.lookat=0;
}

static void testfloat(float t,bool nodec=false)
{
	string sh = float2strhex(t);
	float t2 = str2floathex(sh.c_str());
	string sh2 = float2strhex(t2);
	if (!nodec) {
		string s = float2str(t2);
		logger("f = hex '%s', hex2 '%s', dec '%s'\n",sh.c_str(),sh2.c_str(),s.c_str());
	} else
		logger("f = hex '%s', hex2 '%s'\n",sh.c_str(),sh2.c_str());
}

/*
static float asinf2(float r)
{
	float x=r;
	float y=sqrtf(1-r*r);
	return atan2(x,y);
}

static float acosf2(float r)
{
	float x=sqrtf(1-r*r);
	float y=r;
	return atan2(x,y);
}
*/

//#define TESTPLACER
#ifdef TESTPLACER
void testplacer()
{
	placer p(8);
	p.update(0,0,54);
	p.update(1,0,54);
	p.update(0,0,53);
	p.update(0,0,52);
	p.update(1,0,53);
	p.update(1,0,52);
	p.update(1,0,51);
	p.update(2,0,54);
	p.update(2,0,53);
	p.update(2,0,52);
	p.update(2,0,51);
	p.update(1,0,50);
	p.update(1,0,49);
	p.update(0,0,51);
	p.update(1,0,49);
	p.update(2,0,49);
	p.update(2,0,48);
	S32 s0p = p.getplace(0);
	S32 s1p = p.getplace(1);
	S32 s2p = p.getplace(2);
	p.reset();
	/*
	st.insert(fs(0,0,50));
	st.insert(fs(0,0,34));
	st.insert(fs(0,0,57));
	st.insert(fs(0,0,99));
	st.insert(fs(0,0,19));
	st.insert(fs(0,0,12));
	st.insert(fs(0,0,55));
//	st.erase(fs(0,0,12));
	set<fs,fp>::iterator it = st.begin();
	logger("set :\n");
	while(it!=st.end()) {
		const fs& f = *it;
		logger("%d %d %d\n",f.slot,f.lap,f.nsf);
		++it;
	}
	logger("\n");  */
}
#endif

void carenalobbyinit()
{
	logger("---------------------- carenalobbyinit -------------------------------\n");
#define FLOATTEST3
#ifdef FLOATTEST3
	float z = 9;
	logger("z = %a\n",z);
//	float q = 0x1.8p+5f; // darn, no hexfloat literals
//	logger("q = %f\n",q);
#endif
//#define FLOATTEST2
#ifdef FLOATTEST2
	pointf3xh vel;
	pointf3xh bases;
	pointf3xh pr;
	U32 ivx = 0x00010000;
	U32 ivy = 0x3d8b4633;
	U32 ivz = 0x00000000;
	U32 ibx = 0x3f7ffd92;
	U32 iby = 0x80000000;
	U32 ibz = 0x3c0d1df5;
	vel.x = *(float*)(&ivx);
	vel.y = *(float*)(&ivy);
	vel.z = *(float*)(&ivz);
	bases.x = *(float*)(&ibx);
	bases.y = *(float*)(&iby);
	bases.z = *(float*)(&ibz);
	U32 cw;
	READCW(cw);
	logger("fpu control is at %08x\n",cw);
	proj3d(&vel,&bases,&pr);
	U32 ipx = *(U32*)&pr.x;
	U32 ipy = *(U32*)&pr.y;
	U32 ipz = *(U32*)&pr.z;
	logger("proj3d floattest gives %08x,%08x,%08x\n",ipx,ipy,ipz);


#endif
// #define FLOATTEST
#ifdef FLOATTEST
	{
		const float st = -.9999f;
		const float end = .9999f;
		const float step = .0003f;
		float r;
		for (r=st;r<end;r+=step) {
			float a=rasin(r);
			float a2=rasin(r);
			string rs = float2strhex(r);
			string as = float2strhex(a);
			string as2 = float2strhex(a2);
			logger("asinf('%s')='%s' '%s'\n",rs.c_str(),as2.c_str(),as2.c_str());
		}
		for (r=st;r<end;r+=step) {
			float a=racos(r);
			float a2=racos(r);
			string rs = float2strhex(r);
			string as = float2strhex(a);
			string as2 = float2strhex(a2);
			logger("acosf('%s')='%s' '%s'\n",rs.c_str(),as2.c_str(),as2.c_str());
		}
		for (r=st;r<end;r+=step) {
			float a=atanf(r);
			string rs = float2strhex(r);
			string as = float2strhex(a);
			logger("atanf('%s')='%s'\n",rs.c_str(),as.c_str());
		}
		string upx  = "+0.000000h+00";
		string upy  = "+0.000000h+00";
		string upz  = "+1.000000h+00";
		string upw  = "+0.000000h+00";
		string tanx = "+1.1c387eh-01";
		string tany = "+0.000000h+00";
		string tanz = "+1.a9ddech-01";
		string tanw = "+1.000000h+00";
		string brotx,broty,brotz,brotw;
		pointf3x up(str2floathex(upx.c_str()),str2floathex(upy.c_str()),str2floathex(upz.c_str()),str2floathex(upw.c_str()));
		pointf3x tann(str2floathex(tanx.c_str()),str2floathex(tany.c_str()),str2floathex(tanz.c_str()),str2floathex(tanw.c_str()));
		pointf3 brot;
		normal2quaty(&up,&tann,&brot);
		brotx = float2strhex(brot.x);
		broty = float2strhex(brot.y);
		brotz = float2strhex(brot.z);
		brotw = float2strhex(brot.w);
		logger("up = '%s','%s','%s','%s'\n",upx.c_str(),upy.c_str(),upz.c_str(),upw.c_str());
		logger("tan = '%s','%s','%s','%s'\n",tanx.c_str(),tany.c_str(),tanz.c_str(),tanw.c_str());
		logger("brot = '%s','%s','%s','%s'\n",brotx.c_str(),broty.c_str(),brotz.c_str(),brotw.c_str());
		float f = 0;
		testfloat(f);
		f = PI;
		testfloat(f);
		f = TWOPI;
		testfloat(f);
		f = step;
		testfloat(f);
		f = EPSILON;
		testfloat(f,true);
		f = .125;
		float m = .125;
		while(f<20) {
			testfloat(f);
			f = f+m;
		}
	}
#endif
#ifdef TESTPLACER
	testplacer();
#endif
//	setjrmcar(); // setup defaults for jrm cars
// set data subfolder
pushandsetdir("newcarenadata");
// fetch json file
pushandsetdirdown("carenalobby");
	json js=json(fileload_string(wininfo.isalreadyrunning ? "gamedescb.txt" : "gamedesc.txt"));
popdir();
// convert to struct
	gd = new gamedescj(js);
// video
	video_setupwindow(GX,GY);
// setup video
	init3d();
	buildui();
	struct2ui();
	loadtrackpic();
	track_viewport_roottree=gettrackscene();
	changecar();
	carspinang=wheelspinang=0;
	initsocker();
	focus=0;
	lweaphave->sort();
	lweapavail->sort();
	extradebvars(lobbyvars,nlobbyvars);
}

void carenalobbyproc()
{
	char str[500];
	const char *ptr;
	int i,n;
// update yourip list
	if (!lyourip->getnumidx()) {
		n=getnmyip();
		for (i=0;i<n;i++) {
			lyourip->addstring(ip2str(getmyip(i)));
		}
	}
#if 1
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	S32 ret = 0;
	shape* focusa = 0;
	if (focus) {
		ret=focus->procfocus();
		focusa = ret>=0 ? focus : 0;
	}
#else
// set focus
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
// if something selected...
#endif
	if (focusa) {
//		S32 ret=focus->proc();
		shape* focusb = focusa; // released (hit)
		shape* focush = focusa; // always?
		shape* focusl = focusa; // changed selection
		shape* focuse = focusa; // hit enter
// quit
		if (focusb==bcancel) { // start state button, can't do a switch on pointers
			gd->n.gamenet=GAMENET_BAILED;
			cgi.isvalid = false;
			popstate();
// main menu options
		} else if (focusb==bmainmenu) {
			gd->n.gamenet=GAMENET_BAILED;
			pushchangestate(STATE_MAINMENU);
// play game
		} else if (focusb==bbot) {
			gd->n.gamenet=GAMENET_BOTGAME;
			if (matchcl)
				changestate(STATE_NEWCARENACONNECT);
			else
				pushchangestate(STATE_NEWCARENACONNECT);
		} else if (focusb==bclient) {
			gd->n.gamenet=GAMENET_CLIENT;
			if (matchcl)
				changestate(STATE_NEWCARENACONNECT);
			else
				pushchangestate(STATE_NEWCARENACONNECT);
		} else if (focusb==bserver) {
			gd->n.gamenet=GAMENET_SERVER;
			if (matchcl)
				changestate(STATE_NEWCARENACONNECT);
			else
				pushchangestate(STATE_NEWCARENACONNECT);
		} else if (focusb==boldconstructor) {
			gd->n.gamenet=GAMENET_BAILED;
			pushchangestate(STATE_NEWCARENAOLDCONS);
		} else if (focusb==bnewconstructor) {
			gd->n.gamenet=GAMENET_BAILED;
			pushchangestate(STATE_NEWCARENANEWCONS);
		} else if (focusb==bipdel) {
			lip->removeidx(lip->getidx());
// num players
		} else if (focush==hnlaps) {
			S32 nl=hnlaps->getidx();
			if (nl==1)
				sprintf(str,"One Lap");
			else
				sprintf(str,"%d Laps",nl);
			tnlaps->settname(str);
		} else if (focush==hnump) {
			S32 np=hnump->getidx();
			if (np==1)
				sprintf(str,"One Human Player");
			else
				sprintf(str,"%d Human Players",np);
			tnump->settname(str);
		} else if (focush==hnumb) {
			S32 np=hnumb->getidx();
			if (np==1)
				sprintf(str,"One Bot Player");
			else
				sprintf(str,"%d Bot Players",np);
			tnumb->settname(str);
// choose/change car
		} else if (focush==lcarbody) {
			changecar();
		} else if (focush==lpaint) {
			changecar();
		} else if (focush==ldecal) {
			changecar();
// fiddle with weapons
		} else if (focusl==lweaphave) {
			ptr=lweaphave->getidxname();
			lweapavail->addstring(ptr);
			lweaphave->removeidx();
			lweapavail->sort();
			lweaphave->setidxc(-1);
		} else if (focusl==lweapavail) {
			if (lweaphave->getnumidx()<MAX_WEAPCARRY) {
				ptr=lweapavail->getidxname();
				lweaphave->addstring(ptr);
				lweapavail->removeidx();
			}
			lweaphave->sort();
			lweapavail->setidxc(-1);
// mess with ips
		} else if (focusl==lip) {
			ptr=lip->getidxname();
			eip->settname(ptr);
		} else if (focusl==lyourip) {
			ptr=lyourip->getidxname();
			eip->settname(ptr);
// select track
		} else if (focusl==ltrack) {
			loadtrackpic();
			track_viewport_roottree=gettrackscene();
		} else if (focusl==lyourip) {
		} else if (focuse==eip) {
			ptr=eip->gettname();
			i=lip->findstring(ptr);
			if (i==-1) {
				lip->addstring(ptr);
				lip->sort();
				i=lip->findstring(ptr);
				lip->setidxc(i);
			}
		}
// fiddle with energies
		for (i=0;i<MAX_ENERGIES;++i) {
			if (focush==henergy[i]) {
				gd->a.energies[i]=henergy[i]->getidx();
				sprintf(str,"%s %d",energy_names[i],gd->a.energies[i]);
				tenergy[i]->settname(str);
				break;
			}
		}
	}
// done ui
	carspinang+=.01f;
	carspinang=normalangrad(carspinang);
	wheelsteerang+=wheelsteerdir*.01f;
	if (wheelsteerang>=.25*PI)
		wheelsteerdir=-1;
	else if (wheelsteerang<=-.25*PI)
		wheelsteerdir=1;
// animate car
	tree2* acarnull=acar->getcartree();
	acarnull->rot.y=carspinang;
	{
		tree2 *wheels[MAX_WHEELS];
		wheelspinang+=.05f;
		wheelspinang=normalangrad(wheelspinang);
		findcarwheels(acarnull,wheels);
		for (i=0;i<MAX_WHEELS;i++)
			if (wheels[i]) {
				wheels[i]->buildo2p=O2P_FROMTRANSROTSCALE;
				wheels[i]->rot.x=wheelspinang;
				if (i==WHEEL_FL || i==WHEEL_FR)
					wheels[i]->rot.y=wheelsteerang;
			}
	}
// update display
	doflycam(&lobby_viewport);
	doflycam(&track_viewport);
	lobby_viewport_roottree->proc();
	video_buildworldmats(lobby_viewport_roottree);
	dolights();
}

void carenalobbydraw2d()
{
//	rl->draw();
// draw fancy track pic
	cliprecto32(B32,TRACKPICX-2,TRACKPICY-2,TRACKPICW+4,TRACKPICH+4,C32WHITE);
	cliprecto32(B32,TRACKPICX-1,TRACKPICY-1,TRACKPICW+2,TRACKPICH+2,C32BLACK);
	if (!track_viewport_roottree)
		clipblit32(trackpic,B32,0,0,TRACKPICX,TRACKPICY,TRACKPICW,TRACKPICH);
}

void carenalobbydraw3d()
{
//	video_sprite_draw(trackpic,F32WHITE, 25,166,140,105);
//	video_sprite_end(); 
	lobby_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	lobby_viewport.xres=WX;
	lobby_viewport.yres=WY;
	lobby_viewport.xstart=0;
	lobby_viewport.ystart=0;
	video_setviewport(&lobby_viewport); // clear everything whole screen
	lobby_viewport.flags=0;
	lobby_viewport.xres=CARVIEWW*WX/LOBBYSPTRESX;
	lobby_viewport.yres=CARVIEWH*WY/LOBBYSPTRESY;
	lobby_viewport.xstart=CARVIEWX*WX/LOBBYSPTRESX;
	lobby_viewport.ystart=CARVIEWY*WY/LOBBYSPTRESY;
	video_setviewport(&lobby_viewport); // set for car
	video_drawscene(lobby_viewport_roottree);
	if (track_viewport_roottree && ltrack->getvis()) { // draw track if available
		track_viewport_roottree->proc();
		video_buildworldmats(track_viewport_roottree);

//	const trkt* ot = gd->e.oldtrackj;
//	if (!ot)
//		errorexit("no old track to set viewport with");
//	if (ot->skybox)
		track_viewport.flags=0;//VP_CLEARWB; // all handled by lobby_viewport
//	else
//		track_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
		track_viewport.xstart=TRACKPICX*WX/LOBBYSPTRESX;
		track_viewport.ystart=TRACKPICY*WY/LOBBYSPTRESY;
		track_viewport.xres=TRACKPICW*WX/LOBBYSPTRESX;
		track_viewport.yres=TRACKPICH*WY/LOBBYSPTRESY;
		video_setviewport(&track_viewport); // set for track
		video_drawscene(track_viewport_roottree);
	}

#if 1
	video_sprite_begin(
	  LOBBYSPTRESX,LOBBYSPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  0,
	  0);
	rl->draw3d();
	video_sprite_end();
#endif
}

void carenalobbyexit()
{
	int i,n;
	FILE* fp;
	logger("---------------------- carenalobbyexit -------------------------------\n");
// log and free
	logger("logging lobby roottree\n");
	lobby_viewport_roottree->log2();
	if (track_viewport_roottree) {
		logger("logging track roottree\n");
		track_viewport_roottree->log2();
	}
	logger("logging reference lists\n");
	logrc();
// write out ips.txt
pushandsetdirdown("carenalobby");
	ui2struct();
	json js=gd->save();
	js.save(wininfo.isalreadyrunning ? "gamedescb.txt" : "gamedesc.txt");
	n=lip->getnumidx();
	fp=fopen2("ips.txt","w");
	for (i=0;i<n;i++)
		fprintf(fp,"%s\n",lip->getidxname(i));
	fclose(fp);
// write out gamepicked.txt
popdir();
// free everything
	delete gd;
	extradebvars(0,0);
	delete rl;
	delete acar;
	acar=0;
	if (trackpic) {
		bitmap32free(trackpic);
		trackpic=0;
	}
	delete lobby_viewport_roottree;
	if (!matchcl)
		uninitsocker();
	exit_res3d();
popdir(); // newcarenadata
}
