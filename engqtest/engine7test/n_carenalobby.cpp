/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <engine7cpp.h>
#include <misclib7cpp.h>
#include <videoicpp.h>
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "../u_states.h"
#include "n_carclass.h"
#include "n_marching.h"

struct viewport2 lobby_viewport,track_viewport; // for debvars
tree2* lobby_viewport_roottree,*track_viewport_roottree;
string lobby_track;
static char curweapname[50];
static tree2 *weaptree;

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
// ui
static shape *rl;
static listbox* ltrack;
static listbox* lweaphave;
static listbox* lweapavail;
static listbox* lrules;
static listbox* lcarbody;
static listbox* lpaint;
static listbox* ldecal;
static edit* ename;
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
static listbox* lip;
static pbut* bipdel;
static shape* focus;

char *tenergyusedresname[]={
	"TEXTUFUSION",
	"TEXTUELECTRICAL",
	"TEXTUCYBER",
	"TEXTUGEOTHERMAL",
	"TEXTUPARTICLE",
	"TEXTUCHEMICAL",
};
char *henergyresname[]={
	"SLIDEFUSION",
	"SLIDEELECTRICAL",
	"SLIDECYBER",
	"SLIDEGEOTHERMAL",
	"SLIDEPARTICLE",
	"SLIDECHEMICAL",
};
char *tenergyresname[]={
	"TEXTFUSION",
	"TEXTELECTRICAL",
	"TEXTCYBER",
	"TEXTGEOTHERMAL",
	"TEXTPARTICLE",
	"TEXTCHEMICAL",
};

//static tree2 *acarnull,*acarbody;//,*atrack;
n_carclass* acar;
static float carspinang,wheelspinang,wheelsteerang;
int wheelsteerdir=1;
static int paintlo,painthi;
static int decallo,decalhi;
static int hideregpointsv=1;

// game pick
static int which_game;

// defaults
static int lobby_port;
static int lobby_energies[MAX_ENERGIES];
//static char **sc_botnoweap;
//static int nsc_botnoweap;
//static int nprebuilttracks;

void setjrmcar()
{
	jrmcarscale= .1f; // .014f; was
	jrmcarrot.x= -90;
	jrmcarrot.y= 0;
	jrmcarrot.z= 0;
	jrmcartrans.x= 0;
	jrmcartrans.y= 0;//-.07f;
	jrmcartrans.z= 0;
}

static void loadtrackpic()
{
	struct bitmap32 *pic;
//	struct bitmap24 *jpic,*jpicsmall;
	char str[300];
	const char *p;
//	int i;
//	i=getcurlistbox(rl,LISTTRACK);
	p=ltrack->getidxname();
//	i=lb->getidx();
//	if (i<0)
//		i=0;
//	p=findlistboxname(rl,LISTTRACK,i);
	logger("found track '%s'\n",p);
	sprintf(str,"tracks/%s",p);
	pushandsetdirdown(str);
	sprintf(str,"%s.jpg",p);
	if (fileexist(str)) {
//		fileopen(str,READ);
//		pic=gfxread16(BPP);
//		clipscaleblit16(pic,trackpic,0,0,pic->x-1,pic->y-1,0,0,trackpic->x-1,trackpic->y-1);
//		bitmap16free(pic);
//		fileclose();
		pic=gfxread32(str);
		clipscaleblit32(pic,trackpic);
		bitmap32free(pic);
/*		jpic=gfxread24(); // slower filtered scale
		fileclose();
		jpicsmall=bitmap24alloc(trackpic->x,trackpic->y,rgbblack);
		filt24to24deluxe(jpic,jpicsmall,0,0,jpic->x-1,jpic->y-1,0,0,jpicsmall->x-1,jpicsmall->y-1);
		bitmap24free(jpic);
		pic=conv24to16ck(jpicsmall,BPP);
		bitmap24free(jpicsmall);
		clipblit16(pic,trackpic,0,0,0,0,pic->x,pic->y);
		bitmap16free(pic);
*/	} else
//		cliprect16(trackpic,0,0,trackpic->x-1,trackpic->y-1,hidarkgray);
		cliprect32(trackpic,0,0,trackpic->size.x,trackpic->size.y,C32DARKGRAY);

	popdir();
}

static void changecar()
{
	delete acar;
	const C8* p=lcarbody->getidxname();
	acar=new n_carclass(p);
	lobby_viewport_roottree->linkchild(acar->getcartree());//linkchildtoparent(acarnull,);
#if 0
	int /*i,*/pnt,dec;
	const char *p;
	if (weaptree) {
//		freetree(weaptree);
		delete weaptree;
		weaptree=0;
	}
	freecar();
//	listbox* lb;
//	p=findlistboxname(rl,LISTCARBODY,i);
//	i=getcurlistbox(rl,LISTCARBODY);
//	if (i<0)
//		errorexit("negative car index!");
//	pnt=getcurlistbox(rl,LISTPAINT)+paintlo;
	pnt=lpaint->getidx()+paintlo;
//	dec=getcurlistbox(rl,LISTDECAL)+decallo;
	dec=ldecal->getidx()+decallo;
	pushandsetdir("engine7testdata/cardata");
	C8 hasfile[500];
	sprintf(hasfile,"%s.jrm",p);
	if (fileexist(hasfile)) {
		acarbody=loadnewjrms2(p,pnt,dec);
		if (hideregpointsv)
			hideregpoints(acarbody);
	} else {
		sprintf(hasfile,"%s.bws",p);
		if (fileexist(hasfile)) {
			acarbody=new tree2(hasfile);
		} else {
			acarbody=new tree2("nullcarbody");
		}
	}
	popdir();
//	acarnull=alloctree(50,NULL);
	acarnull=new tree2("acarnull");
//	linkchildtoparent(acarbody,acarnull);
	acarnull->linkchild(acarbody);
//	acarnull->rotvel.y=.4f;
//	linkchildtoparent(acarnull,lobby_viewport.roottree);
	lobby_viewport_roottree->linkchild(acarnull);
	if (curweapname[0]) {
		weaptree=loadweap(acarnull,acarbody,curweapname);
//		linkchildtoparent(weaptree,acarnull);
		acarnull->linkchild(weaptree);
	}
#endif
}

static void updateenergies()
{
	char str[50];
	int i,j,k;
	for (j=0;j<MAX_WEAPKIND;j++)
		if (!strcmp(weapinfos[j].name,curweapname))
			break;
	for (i=0;i<MAX_ENERGIES;i++) {
		if (j!=MAX_WEAPKIND)
			k=weapinfos[j].energyusage[i];
		else
			k=0;
		if (k)
			sprintf(str,"%s %d",energy_names[i],k);
		else
			str[0]='\0';
//		setresname(rl,TEXTUFUSION+i,str);
//		char rn[50];
//		sprintf(rn,"TEXTFUSION%d",i);
//		text* sh=rl->find<text>(rn);
		tenergyused[i]->settname(str);
	}
}

static void checkconstrack()
{
	struct track atrk;
	struct newtrack *anewtrk;
	int i;//,j;
	if (track_viewport_roottree) {
//		freetree(track_viewport.roottree);
		delete track_viewport_roottree;
		track_viewport_roottree=0;
	}
//	setresvis(rl,PBUTNEWCONSTRUCTOR,0);
//	shape* sh;
//	sh=rl->find("PBUTNEWCONSTRUCTOR");
	bnewconstructor->setvis(0);
//	setresvis(rl,PBUTOLDCONSTRUCTOR,0);
//	sh=rl->find("PBUTOLDCONSTRUCTOR");
	boldconstructor->setvis(0);
//	j=getcurlistbox(rl,LISTTRACK);
//	j=lb->getidx();
//	if (i=loadtrackscript(findlistboxname(rl,LISTTRACK,j),&atrk.trkdata)) {
	if (i=loadtrackscript(ltrack->getidxname(),&atrk.trkdata)) {
		track_viewport_roottree=buildconsscene(&atrk);
//		setresvis(rl,PBUTOLDCONSTRUCTOR,1);
//		sh=rl->find("PBUTOLDCONSTRUCTOR");
		boldconstructor->setvis(1);
//	} else if (anewtrk=loadnewtrackscript(findlistboxname(rl,LISTTRACK,j))) {
	} else if (anewtrk=loadnewtrackscript(ltrack->getidxname())) {
		track_viewport_roottree=buildnewconsscene(anewtrk);
//		setresvis(rl,PBUTNEWCONSTRUCTOR,1);
//		sh=rl->find("PBUTNEWCONSTRUCTOR");
		bnewconstructor->setvis(1);
		freenewtrackscript(anewtrk);
//		anewtrk.trkdata=0;
	}
}

void carenalobbyinit()
{
	setjrmcar();
pushandsetdir("engine7testdata");
	int i,j;
	int p=4,bp=0; // default 4 players if no 'players'
//	char **sc;
//	int nsc;
	script* sc;
//	tree2 *rt;//,*scn,*obj;
	logger("---------------------- carenalobbyinit -------------------------------\n");
// video
	video_setupwindow(800,600);
// setup video
//	rt=alloctree(10000,NULL);
//	mystrncpy(rt->name,"roottree",NAMESIZE);
	lobby_viewport_roottree=new tree2("roottree");
//	load scene
//	if (video_maindriver==VIDEO_MAINDRIVER_D3D) {
		lightinfo.uselights=1;
//		usescnlights=1;
		lightinfo.dodefaultlights=1;
/*	} else {
		uselights=0;
		usescnlights=0;
	} */
// setup viewport
	lobby_viewport.backcolor=C32BLUE;
//	lobby_viewport.camtrans.z=-100;
//	lobby_viewport.camzoom=2.0f; //3.2f;
	lobby_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	lobby_viewport.roottree=rt;
	lobby_viewport.xstart=CARVIEWX;
	lobby_viewport.ystart=CARVIEWY;
	lobby_viewport.xres=CARVIEWW;
	lobby_viewport.yres=CARVIEWH;
	lobby_viewport.zfront=.1f;
	lobby_viewport.zback=2000;
//	lobby_viewport.camattach=NULL;//getlastcam();
//	vp.camattach->camtarget=mt;
//	setviewportsrc(&lobby_viewport); // user calls this
	lobby_viewport.xsrc=4; lobby_viewport.ysrc=3;
	lobby_viewport.useattachcam=false;
//	lobby_viewport.lookat=0;
	track_viewport.backcolor=C32BLUE;
//	track_viewport.camtrans.z=-1;
//	track_viewport.camzoom=2.0f; //3.2f;
	track_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	track_viewport.roottree=0;
	track_viewport.xstart=TRACKPICX;
	track_viewport.ystart=TRACKPICY;
	track_viewport.xres=TRACKPICW;
	track_viewport.yres=TRACKPICH;
	track_viewport.zfront=.1f;
	track_viewport.zback=2000;
//	track_viewport.camattach=NULL;//getlastcam();
//	vp.camattach->camtarget=mt;
//	setviewportsrc(&track_viewport); // user calls this
	track_viewport.xsrc=4;track_viewport.ysrc=3;
	track_viewport.useattachcam=false;
//	track_viewport.lookat=0;
// resource
pushandsetdirdown("carenalobby");
//	rl=loadres("carenalobbyres.txt");
	factory2<shape> fact;
	script* msc=new script("carenalobbyres.txt");
	rl=fact.newclass_from_handle(*msc);
	delete msc;
	ltrack=rl->find<listbox>("LISTTRACK");
	lweaphave=rl->find<listbox>("LISTWEAPHAVE");
	lweapavail=rl->find<listbox>("LISTWEAPAVAIL");
	lrules=rl->find<listbox>("LISTRULES");
	lcarbody=rl->find<listbox>("LISTCARBODY");
	lpaint=rl->find<listbox>("LISTPAINT");
	ldecal=rl->find<listbox>("LISTDECAL");
	ename=rl->find<edit>("EDITNAME");
	eip=rl->find<edit>("EDITIP");
	boldconstructor=rl->find<pbut>("PBUTOLDCONSTRUCTOR");
	bnewconstructor=rl->find<pbut>("PBUTNEWCONSTRUCTOR");
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
	bipdel=rl->find<pbut>("PBUTIPDEL");
	for (i=0;i<MAX_ENERGIES;++i) {
		tenergyused[i]=rl->find<text>(tenergyusedresname[i]);
		henergy[i]=rl->find<hscroll>(henergyresname[i]);
		tenergy[i]=rl->find<text>(tenergyresname[i]);
	}
// init vars
	loadenergymatrix();
//	setresrange(rl,SLIDENUMP,1,MAX_PLAYERS);
	hscroll* hs=rl->find<hscroll>("SLIDENUMP");
//	hs->setnumidx(MAX_PLAYERS+1);
	hs->setminmaxval(1,64);
//	setresrange(rl,SLIDENUMPB,0,MAX_PLAYERS);
	hs=rl->find<hscroll>("SLIDENUMPB");
	hs->setnumidx(MAX_PLAYERS+1);
	lobby_port=0;
	which_game=GAMENET_BAILED;
	trackpic=bitmap32alloc(TRACKPICW,TRACKPICH,C32GREEN);
// load ips
	if (fileexist("ips.txt")) {
//		slecstdisable();
//		sc=loadscript("ips.txt",&nsc);
		sc=new script("ips.txt");
		sc->sort();
		for (i=0;i<sc->num();i++)
//			addlistboxname(rl,LISTIP,-1,sc->idx(i).c_str(),-1);
			lip->addstring(sc->idx(i).c_str());
//		sortlistbox(rl,LISTIP,-1,-1,-1,0);
//		freescript(sc,nsc);
		delete sc;
	}
// load defaults
//	sc=loadscript("gamedomain.txt",&nsc);
	sc=new script("gamedomain.txt");
//	if (nsc&1)
	if (sc->num()&1)
		errorexit("bad gamedomain.txt");
popdir();
	paintlo=painthi=decallo=decalhi=0;
	memset(lobby_energies,0,sizeof(lobby_energies));
	for (i=0;i<sc->num();i+=2) {
//		if (!strcmp(sc->idx(i).c_str(),"track"))
//			addlistboxname(rl,LISTTRACK,-1,sc->idx(i+1).c_str(),-1);
//		else if (!strcmp(sc->idx(i).c_str(),"hideregpoints")) {
//			hideregpointsv=atoi(sc->idx(i+1).c_str());
//		} else if (!strcmp(sc->idx(i).c_str(),"rules")) {
//			addlistboxname(rl,LISTRULES,-1,sc->idx(i+1).c_str(),-1);
//		} else if (!strcmp(sc->idx(i).c_str(),"body")) {
//			addlistboxname(rl,LISTCARBODY,-1,sc->idx(i+1).c_str(),SCLCARBODY);
		if (!strcmp(sc->idx(i).c_str(),"paintlo")) {
			paintlo=atoi(sc->idx(i+1).c_str());
		} else if (!strcmp(sc->idx(i).c_str(),"painthi")) {
			painthi=atoi(sc->idx(i+1).c_str());
		} else if (!strcmp(sc->idx(i).c_str(),"decallo")) {
			decallo=atoi(sc->idx(i+1).c_str());
		} else if (!strcmp(sc->idx(i).c_str(),"decalhi")) {
			decalhi=atoi(sc->idx(i+1).c_str());
		} else if (!strcmp(sc->idx(i).c_str(),"botname")) { // skip over botnames
		} else if (!strcmp(sc->idx(i).c_str(),"serverport")) {
			lobby_port=atoi(sc->idx(i+1).c_str());
//		} else if (!strcmp(sc->idx(i).c_str(),"weap")) {
//			addlistboxname(rl,LISTWEAPAVAIL,-1,sc->idx(i+1).c_str(),-1);
		} else if (!strcmp(sc->idx(i).c_str(),"botnoweap")) { // skip over botnoweap
		} else {
//			for (j=0;j<MAX_ENERGIES;j++)
//				if (!strcmp(sc->idx(i).c_str(),energy_names[j]))
//					break;
//			if (j!=MAX_ENERGIES)
//				lobby_energies[j]=atoi(sc->idx(i+1).c_str());
//			else
			errorexit("unknown gamedomain.txt token '%s'",sc->idx(i).c_str());
		}
	}
//	freescript(sc,nsc);
	delete sc;
pushandsetdirdown("tracks");
//	sc=doadir(&nsc,1);
	sc=new scriptdir(1);
//	sc->sort();
popdir();
	for (i=0;i<sc->num();i++)
//		if (sc->idx(i).c_str()[0]=='*') // a dir
//			addlistboxname(rl,LISTTRACK,-1,&sc->idx(i).c_str()[1],SCLTRACK);
		ltrack->addstring(sc->idx(i).c_str());

//	nprebuilttracks=nsc;
//	freescript(sc,nsc);
	delete sc;
pushandsetdirdown("constructed");
//	sc=doadir(&nsc,0);
	sc=new scriptdir(0);
popdir();
	for (i=0;i<sc->num();i++)
		if (isfileext(sc->idx(i).c_str(),"trk")) {
			char str[50];
			mgetname(sc->idx(i).c_str(),str);
//			addlistboxname(rl,LISTTRACK,-1,str,SCLTRACK);
			ltrack->addstring(str);
		}
//	freescript(sc,nsc);
	delete sc;
	ltrack->sort();
//	sortlistbox(rl,LISTTRACK,-1,-1,-1,0);
/*pushandsetdirdown("cardata");
//	sc=doadir(&nsc,0);
	sc=new scriptdir(0);
	sc->sort();
popdir();
	for (i=0;i<sc->num();i++) {
		char name[50];
		if (isfileext(sc->idx(i).c_str(),"jrm")) {
			mgetname(sc->idx(i).c_str(),name);
//			addlistboxname(rl,LISTCARBODY,-1,name,SCLCARBODY);
			lcarbody->addstring(name);
		}
		if (isfileext(sc->idx(i).c_str(),"bws")) {
			mgetname(sc->idx(i).c_str(),name);
//			addlistboxname(rl,LISTCARBODY,-1,name,SCLCARBODY);
			lcarbody->addstring(name);
		}
	}

	lcarbody->sort();
*/
	sc=n_carclass::getcarlist();
	for (i=0;i<sc->num();i++)
		lcarbody->addstring(sc->idx(i).c_str());
//	freescript(sc,sc->num());
	delete sc;
	for (i=0;i<MAX_WEAPKIND;i++)
//		addlistboxname(rl,LISTWEAPAVAIL,-1,weapinfos[i].name,-1);
		lweapavail->addstring(weapinfos[i].name);
	for (i=0;i<MAX_RULES;i++)
//		addlistboxname(rl,LISTRULES,-1,rule_names[i],-1);
		lrules->addstring(rule_names[i]);
	for (i=paintlo;i<=painthi;i++) {
		char str[50];
		sprintf(str,"Paint %02d",i);
//		addlistboxname(rl,LISTPAINT,-1,str,SCLPAINT);
		lpaint->addstring(str);
	}
	for (i=decallo;i<=decalhi;i++) {
		char str[50];
		sprintf(str,"Decal %02d",i);
//		addlistboxname(rl,LISTDECAL,-1,str,SCLDECAL);
		ldecal->addstring(str);
	}
	if (lobby_port==0)
		errorexit("need serverport in gamedomain.txt");
//	setcurlistbox(rl,LISTTRACK,0,SCLTRACK);
//	setcurlistbox(rl,LISTRULES,0,-1);
//	setcurlistbox(rl,LISTCARBODY,0,SCLCARBODY);
//	setcurlistbox(rl,LISTPAINT,0,SCLPAINT);
//	setcurlistbox(rl,LISTDECAL,0,SCLDECAL);
//	sortlistbox(rl,LISTWEAPAVAIL,-1,-1,-1,0);
	ltrack->setidxc(0);
	lrules->setidxc(0);
	lcarbody->setidxc(0);
	lpaint->setidxc(0);
	ldecal->setidxc(0);
//	lweapavail->setidxc(-1);
// load last settings
pushandsetdirdown("carenalobby");
	if (fileexist("gamepicked.txt")) {
//		slecstdisable();
//		sc=loadscript("gamepicked.txt",&nsc);
		sc=new script("gamepicked.txt");
		if (sc->num()&1)
			errorexit("bad gamepicked.txt");
		for (i=0;i<sc->num();i+=2)
			if (!strcmp(sc->idx(i).c_str(),"playername")) {
//				setresname(rl,EDITNAME,sc->idx(i+1).c_str());
				ename->settname(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"rules")) {
				j=atoi(sc->idx(i+1).c_str());
//				j=findlistboxidx(rl,LISTRULES,sc[i+1]);
				if (j>=0)
//					setcurlistbox(rl,LISTRULES,j,-1);
					lrules->setidxc(j);
			} else if (!strcmp(sc->idx(i).c_str(),"gamenet")) { // skip
			} else if (!strcmp(sc->idx(i).c_str(),"serverip")) {
//				setresname(rl,EDITIP,sc->idx(i+1).c_str());
				eip->settname(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"serverport")) { // skip
			} else if (!strcmp(sc->idx(i).c_str(),"track")) {
//				if (!lobby_track[0]) // can be set by constructor
				if (!lobby_track.size())
//					strcpy(lobby_track,sc->idx(i+1).c_str());
					lobby_track=sc->idx(i+1);
			} else if (!strcmp(sc->idx(i).c_str(),"weap")) {
//				if (getnumlistbox(rl,LISTWEAPHAVE)<MAX_WEAPCARRY) {
				if (lweaphave->getnumidx()<MAX_WEAPCARRY) {
//					j=findlistboxidx(rl,LISTWEAPAVAIL,sc->idx(i+1).c_str());
					j=lweapavail->findstring(sc->idx(i+1).c_str());
					if (j>=0) {
//						addlistboxname(rl,LISTWEAPHAVE,-1,sc->idx(i+1).c_str(),-1);
						lweaphave->addstring(sc->idx(i+1).c_str());
//						dellistboxname(rl,LISTWEAPAVAIL,j,-1);
						lweapavail->removeidx(j);
					}
				}
			} else if (!strcmp(sc->idx(i).c_str(),"body")) {
//				j=findlistboxidx(rl,LISTCARBODY,sc->idx(i+1).c_str());
				j=lcarbody->findstring(sc->idx(i+1).c_str());
				if (j>=0)
//					setcurlistbox(rl,LISTCARBODY,j,SCLCARBODY);
					lcarbody->setidxc(j);
			} else if (!strcmp(sc->idx(i).c_str(),"paint")) {
//				j=findlistboxidx(rl,LISTPAINT,sc->idx(i+1).c_str());
				j=lpaint->findstring(sc->idx(i+1).c_str());
				if (j>=0)
//					setcurlistbox(rl,LISTPAINT,j,SCLPAINT);
					lpaint->setidxc(j);
			} else if (!strcmp(sc->idx(i).c_str(),"decal")) {
//				j=findlistboxidx(rl,LISTDECAL,sc->idx(i+1).c_str());
				j=ldecal->findstring(sc->idx(i+1).c_str());
				if (j>=0)
//					setcurlistbox(rl,LISTDECAL,j,SCLDECAL);
					ldecal->setidxc(j);
			} else if (!strcmp(sc->idx(i).c_str(),"humanplayers")) {
				p=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"botplayers")) {
				bp=atoi(sc->idx(i+1).c_str());
			} else {
				for (j=0;j<MAX_ENERGIES;j++)
					if (!strcmp(sc->idx(i).c_str(),energy_names[j]))
						break;
				if (j==MAX_ENERGIES)
					errorexit("unknown gamepicked.txt token '%s'",sc->idx(i).c_str());
				lobby_energies[j]=atoi(sc->idx(i+1).c_str());
			}
//		freescript(sc,nsc);
		delete sc;
	}
popdir();
//	j=findlistboxidx(rl,LISTTRACK,lobby_track);
	j=ltrack->findstring(lobby_track.c_str());
	if (j>=0)
//		setcurlistbox(rl,LISTTRACK,j,SCLTRACK);
		ltrack->setidxc(j);
	char str[50];
	for (i=0;i<MAX_ENERGIES;i++) {
//		setresrange(rl,SLIDEFUSION+i*2,0,20);
//		setresval(rl,SLIDEFUSION+i*2,lobby_energies[i]);
		henergy[i]->setnumidx(20+1);
		henergy[i]->setidx(lobby_energies[i]);
		sprintf(str,"%s %d",energy_names[i],lobby_energies[i]);
//		setresname(rl,TEXTFUSION+i*2,str);
		tenergy[i]->settname(str);
	}
// set number of players in slider and text
//	setresval(rl,SLIDENUMP,p);
	hnump->setidx(p);
	if (p==1)
		sprintf(str,"One Human Player");
	else
		sprintf(str,"%d Human Players",p);
//	setresname(rl,TEXTNUMP,str);
	tnump->settname(str);

	sprintf(str,"%d Bot Players",bp);
//	setresname(rl,TEXTNUMPB,str);
	tnumb->settname(str);

//	setresval(rl,SLIDENUMPB,bp);
	hnumb->setidx(bp);
	curweapname[0]='\0';
	loadtrackpic();
	changecar();
//	tree2* mt=march_test(1.0f,6); // draw a test march object
//	mt->trans.x=3.5f;
//	lobby_viewport_roottree->linkchild(mt);
	updateenergies();
	carspinang=wheelspinang=0;
	checkconstrack();
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
//	int p,bp;
//	if (!getnumlistbox(rl,LISTYOURIP)) {
	if (!lyourip->getnumidx()) {
		n=getnmyip();
		for (i=0;i<n;i++) {
//			addlistboxname(rl,LISTYOURIP,-1,ip2str(getmyip(i)),-1);
			lyourip->addstring(ip2str(getmyip(i)));
		}
	}
//	struct rmessage rm;
//	checkres(rl);
// set focus
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
// if something selected...
//	while(getresmess(rl,&rm)) {
	if (focus) {
		S32 ret=focus->proc();
		shape* focusb = ret ? focus : 0;
		shape* focush = ret>=0 ? focus : 0;
		shape* focusl = ret>=0 ? focus : 0;
		shape* focuse = ret>=0 ? focus : 0;
//		switch(rm.id) {
// quit
//		case PBUTCANCEL:
		if (focusb==bcancel) { // start state button, can't do a switch on pointers
			which_game=GAMENET_BAILED;
			popstate();
//			break;
// main menu options
//		case PBUTMAINMENU:
		} else if (focusb==bmainmenu) {
			which_game=GAMENET_BAILED;
			pushchangestate(STATE_MAINMENU);
//			break;
// play game
//		case PBUTBOT:
		} else if (focusb==bbot) {
			which_game=GAMENET_BOTGAME;
			pushchangestate(STATE_NEWCARENACONNECT);
//			break;
//		case PBUTCLIENT:
		} else if (focusb==bclient) {
			which_game=GAMENET_CLIENT;
			pushchangestate(STATE_NEWCARENACONNECT);
//			break;
//		case PBUTSERVER:
		} else if (focusb==bserver) {
			which_game=GAMENET_SERVER;
			pushchangestate(STATE_NEWCARENACONNECT);
//			break;
//		case PBUTOLDCONSTRUCTOR:
		} else if (focusb==boldconstructor) {
			which_game=GAMENET_BAILED;
			pushchangestate(STATE_NEWCARENAOLDCONS);
//			break;
//		case PBUTNEWCONSTRUCTOR:
		} else if (focusb==bnewconstructor) {
			which_game=GAMENET_BAILED;
			pushchangestate(STATE_NEWCARENANEWCONS);
//			break;
//		case PBUTIPDEL:
		} else if (focusb==bipdel) {
//			i=getcurlistbox(rl,LISTIP);
//			if (i>=0)
//				dellistboxname(rl,LISTIP,i,-1);
//			setcurlistbox(rl,LISTIP,-1,-1);
			lip->removeidx(lip->getidx());
//			break;
// num players
//		case SLIDENUMP:
		} else if (focusl==hnump) {
//			p=rm.val;
			if (ret==1)
				sprintf(str,"One Human Player");
			else
				sprintf(str,"%d Human Players",ret);
//			setresname(rl,TEXTNUMP,str);
			tnump->settname(str);
//			break;
//		case SLIDENUMPB:
		} else if (focusl==hnumb) {
//			bp=rm.val;
			sprintf(str,"%d Bot Players",ret);
//			setresname(rl,TEXTNUMPB,str);
			tnumb->settname(str);
//			break;
// choose/change car
//		case LISTCARBODY:
		} else if (focusl==lcarbody) {
			changecar();
//			setcurlistbox(rl,LISTCARBODY,getcurlistbox(rl,LISTCARBODY),SCLCARBODY);
//			break;
//		case LISTPAINT:
		} else if (focusl==lpaint) {
			changecar();
//			setcurlistbox(rl,LISTPAINT,getcurlistbox(rl,LISTPAINT),SCLPAINT);
//			break;
//		case LISTDECAL:
		} else if (focusl==ldecal) {
			changecar();
//			setcurlistbox(rl,LISTDECAL,getcurlistbox(rl,LISTDECAL),SCLDECAL);
//			break;
//		case SCLCARBODY:
//		} else if (focus==bmainmenu) {
//			setlistboxoffset(rl,LISTCARBODY,rm.val);
//			break;
//		case SCLPAINT:
//		} else if (focus==bmainmenu) {
//			setlistboxoffset(rl,LISTPAINT,rm.val);
//			break;
//		case SCLDECAL:
//		} else if (focus==bmainmenu) {
//			setlistboxoffset(rl,LISTDECAL,rm.val);
//			break;
// fiddle with weapons
//		case LISTWEAPHAVE:
		} else if (focusl==lweaphave) {
//			ptr=findlistboxname(rl,LISTWEAPHAVE,rm.val);
			ptr=lweaphave->getidxname();
//			addlistboxname(rl,LISTWEAPAVAIL,-1,ptr,-1);
			lweapavail->addstring(ptr);
//			dellistboxname(rl,LISTWEAPHAVE,rm.val,-1);
			lweaphave->removeidx();
//			setcurlistbox(rl,LISTWEAPHAVE,-1,-1);
//			sortlistbox(rl,LISTWEAPAVAIL,-1,-1,-1,0);
			lweapavail->sort();
			lweaphave->setidxc(-1);
			if (weaptree) {
//				freetree(weaptree);
				delete weaptree;
				weaptree=0;
			}
			curweapname[0]='\0';
			updateenergies();
//			break;
//		case LISTWEAPAVAIL:
		} else if (focusl==lweapavail) {
//			if (getnumlistbox(rl,LISTWEAPHAVE)<MAX_WEAPCARRY) {
			if (lweaphave->getnumidx()<MAX_WEAPCARRY) {
//				ptr=findlistboxname(rl,LISTWEAPAVAIL,rm.val);
				ptr=lweapavail->getidxname();
				if (weaptree)
//					freetree(weaptree);
					delete weaptree;
/*				weaptree=loadweap(acarnull,acarbody,ptr);
//				linkchildtoparent(weaptree,acarnull);
				acarnull->linkchild(weaptree);
				strcpy(curweapname,ptr); */
				updateenergies();
//				addlistboxname(rl,LISTWEAPHAVE,-1,ptr,-1);
				lweaphave->addstring(ptr);
//				dellistboxname(rl,LISTWEAPAVAIL,rm.val,-1);
				lweapavail->removeidx();
			}
			lweaphave->sort();
			lweapavail->setidxc(-1);
//			setcurlistbox(rl,LISTWEAPAVAIL,-1,-1);
//			break;
// mess with ips
//		case LISTIP:
		} else if (focusl==lip) {
//			ptr=findlistboxname(rl,LISTIP,rm.val);
			ptr=lip->getidxname();
//			setresname(rl,EDITIP,ptr);
			eip->settname(ptr);
//			break;
		} else if (focusl==lyourip) {
//			ptr=findlistboxname(rl,LISTIP,rm.val);
			ptr=lyourip->getidxname();
//			setresname(rl,EDITIP,ptr);
			eip->settname(ptr);
//			break;
// select track
//		case LISTTRACK:
		} else if (focusl==ltrack) {
			loadtrackpic();
			checkconstrack();
//			setcurlistbox(rl,LISTTRACK,getcurlistbox(rl,LISTTRACK),SCLTRACK);
//			break;
//		case SCLTRACK:
//		} else if (focus==bmainmenu) {
//			setlistboxoffset(rl,LISTTRACK,rm.val);
//			break;
//		case LISTYOURIP:
		} else if (focusl==lyourip) {
//			setcurlistbox(rl,LISTYOURIP,-1,-1);
//			break;
//		case EDITIP:
		} else if (focuse==eip) {
//			ptr=getresname(rl,EDITIP);
			ptr=eip->gettname();
//			i=findlistboxidx(rl,LISTIP,ptr);
			i=lip->findstring(ptr);
			if (i==-1) {
//				addlistboxname(rl,LISTIP,-1,ptr,-1);
				lip->addstring(ptr);
//				sortlistbox(rl,LISTIP,-1,-1,-1,0);
				lip->sort();
				i=lip->findstring(ptr);
				lip->setidxc(i);
			}
//			setcurlistbox(rl,LISTIP,-1,-1);
//			break;
		}

// fiddle with energies
/*		if (rm.id>=SLIDEFUSION && rm.id<=SLIDECHEMICAL) {
			i=(rm.id-SLIDEFUSION)/2;
			lobby_energies[i]=rm.val;
			sprintf(str,"%s %d",energy_names[i],lobby_energies[i]);
			setresname(rl,rm.id-1,str);
		} */
		for (i=0;i<MAX_ENERGIES;++i) {
			if (focush==henergy[i]) {
				lobby_energies[i]=ret;
				sprintf(str,"%s %d",energy_names[i],lobby_energies[i]);
				tenergy[i]->settname(str);
				break;
			}
		}
	}
	carspinang+=.01f;
	carspinang=normalangrad(carspinang);
	wheelsteerang+=wheelsteerdir*.01f;
	if (wheelsteerang>=.25*PI)
		wheelsteerdir=-1;
	else if (wheelsteerang<=-.25*PI)
		wheelsteerdir=1;
#if 1
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
#endif
// update display
	doflycam(&lobby_viewport);
	doflycam(&track_viewport);
//	doanims(lobby_viewport_roottree);
//	buildtreematrices(lobby_viewport_roottree);
	lobby_viewport_roottree->proc();
	video_buildworldmats(lobby_viewport_roottree);
	dolights();
}

void carenalobbydraw2d()
{
//	video_lock();
//	drawres(rl);
	rl->draw();
// draw fancy track pic
//	cliprecto16(B16,TRACKPICX-2,TRACKPICY-2,TRACKPICX+TRACKPICW+1,TRACKPICY+TRACKPICH+1,hiwhite);
	cliprecto32(B32,TRACKPICX-2,TRACKPICY-2,TRACKPICW+4,TRACKPICH+4,C32WHITE);
//	cliprecto16(B16,TRACKPICX-1,TRACKPICY-1,TRACKPICX+TRACKPICW,TRACKPICY+TRACKPICH,hiblack);
	cliprecto32(B32,TRACKPICX-1,TRACKPICY-1,TRACKPICW+2,TRACKPICH+2,C32BLACK);
	if (!track_viewport_roottree)
//		clipblit16(trackpic,B16,0,0,TRACKPICX,TRACKPICY,TRACKPICW,TRACKPICH);
		clipblit32(trackpic,B32,0,0,TRACKPICX,TRACKPICY,TRACKPICW,TRACKPICH);
//	video_unlock();
}

void carenalobbydraw3d()
{
//	video_beginscene(&lobby_viewport);
//	video_drawscene(&lobby_viewport);
//	video_endscene(&lobby_viewport);
	lobby_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	lobby_viewport.xres=WX;
	lobby_viewport.yres=WY;
	lobby_viewport.xstart=0;
	lobby_viewport.ystart=0;
	video_setviewport(&lobby_viewport); // clear zbuf etc.
	lobby_viewport.flags=0;
	lobby_viewport.xres=CARVIEWW;
	lobby_viewport.yres=CARVIEWH;
	lobby_viewport.xstart=CARVIEWX;
	lobby_viewport.ystart=CARVIEWY;
	video_setviewport(&lobby_viewport); // clear zbuf etc.
	video_drawscene(lobby_viewport_roottree);
	if (track_viewport_roottree) {
		track_viewport_roottree->proc();
		video_buildworldmats(track_viewport_roottree);
//		doanims(track_viewport.roottree);
//		buildtreematrices(track_viewport.roottree);
//		video_beginscene(&track_viewport);
//		video_drawscene(&track_viewport);
//		video_endscene(&track_viewport);
		video_setviewport(&track_viewport); // clear zbuf etc.
		video_drawscene(track_viewport_roottree);
	}
}

void carenalobbyexit()
{
	int i,j,p,bp,n;
	FILE *fp;
	logger("---------------------- carenalobbyexit -------------------------------\n");
//	logviewport(&lobby_viewport,OPT_SOME);
//	logviewport(&track_viewport,OPT_SOME);
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
//	n=getnumlistbox(rl,LISTIP);
	n=lip->getnumidx();
	fp=fopen2("ips.txt","w");
	for (i=0;i<n;i++)
//		fprintf(fp,"%s\n",findlistboxname(rl,LISTIP,i));
		fprintf(fp,"%s\n",lip->getidxname(i));
	fclose(fp);
// write out gamepicked.txt
//	if (!wininfo.isalreadyrunning) {
		fp=fopen2("gamepicked.txt","w");
//		fp=fopen2("gamepickedw.txt","w"); // test
//		fprintf(fp,"playername \"%s\"\n",getresname(rl,EDITNAME));
		fprintf(fp,"playername \"%s\"\n",ename->gettname());
//		i=getcurlistbox(rl,LISTCARBODY);
//		fprintf(fp,"body \"%s\"\n",findlistboxname(rl,LISTCARBODY,i));
		fprintf(fp,"body \"%s\"\n",lcarbody->getidxname());
//		i=getcurlistbox(rl,LISTPAINT);
//		fprintf(fp,"paint \"%s\"\n",findlistboxname(rl,LISTPAINT,i));
		fprintf(fp,"paint \"%s\"\n",lpaint->getidxname());
//		i=getcurlistbox(rl,LISTDECAL);
//		fprintf(fp,"decal \"%s\"\n",findlistboxname(rl,LISTDECAL,i));
		fprintf(fp,"decal \"%s\"\n",ldecal->getidxname());
//		n=getnumlistbox(rl,LISTWEAPHAVE);
		n=lweaphave->getnumidx();
		for (i=0;i<n;i++)
//			fprintf(fp,"weap \"%s\"\n",findlistboxname(rl,LISTWEAPHAVE,i));
			fprintf(fp,"weap \"%s\"\n",lweaphave->getidxname(i));
		for (i=0;i<MAX_ENERGIES;i++)
			if (strlen(energy_names[i]))
				fprintf(fp,"\"%s\" %d\n",energy_names[i],lobby_energies[i]);
//		i=getcurlistbox(rl,LISTTRACK);
//		strcpy(lobby_track,findlistboxname(rl,LISTTRACK,i));
//		strcpy(lobby_track,ltrack->getidxname());
		lobby_track=ltrack->getidxname();
		fprintf(fp,"track \"%s\"\n",lobby_track.c_str());
//		i=getcurlistbox(rl,LISTRULES);
		i=lrules->getidx();
		fprintf(fp,"rules %d\n",i);
//		p=getresval(rl,SLIDENUMP);
		p=hnump->getidx();
		fprintf(fp,"humanplayers %d\n",p);
//		bp=getresval(rl,SLIDENUMPB);
		bp=hnumb->getidx();
		fprintf(fp,"botplayers %d\n",bp);
//		fprintf(fp,"gamenet %s\n",which_game_names[which_game]);
		fprintf(fp,"gamenet %d\n",which_game);
//		fprintf(fp,"serverip \"%s\"\n",getresname(rl,EDITIP));
		fprintf(fp,"serverip \"%s\"\n",eip->gettname());
		fprintf(fp,"serverport %d\n",lobby_port);
		fclose(fp);
//	}
popdir();
// free everything
	extradebvars(0,0);
//	freeres(rl);
	delete rl;
	if (weaptree) {
//		freetree(weaptree);
		delete weaptree;
		weaptree=0;
	}
	delete acar;
	acar=0;
	if (trackpic) {
		bitmap32free(trackpic);
		trackpic=0;
	}
//	freetree(lobby_viewport_roottree);
	delete lobby_viewport_roottree;
	uninitsocker();
	if (track_viewport_roottree) {
//		freetree(track_viewport_roottree);
		delete track_viewport_roottree;
		track_viewport_roottree=0;
	}
	static char *teststrs[]={
		"",
		"a",
		"abc",
		"message digest",
		"abcdefghijklmnopqrstuvwxyz",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890",
	};
	#define NTESTSTRS (sizeof(teststrs)/sizeof(teststrs[0]))
	static unsigned char hashs[NTESTSTRS][NUMHASHBYTES]={
		{0xd4,0x1d,0x8c,0xd9,0x8f,0x00,0xb2,0x04,0xe9,0x80,0x09,0x98,0xec,0xf8,0x42,0x7e},
		{0x0c,0xc1,0x75,0xb9,0xc0,0xf1,0xb6,0xa8,0x31,0xc3,0x99,0xe2,0x69,0x77,0x26,0x61},
		{0x90,0x01,0x50,0x98,0x3c,0xd2,0x4f,0xb0,0xd6,0x96,0x3f,0x7d,0x28,0xe1,0x7f,0x72},
		{0xf9,0x6b,0x69,0x7d,0x7c,0xb7,0x93,0x8d,0x52,0x5a,0x2f,0x31,0xaa,0xf1,0x61,0xd0},
		{0xc3,0xfc,0xd3,0xd7,0x61,0x92,0xe4,0x00,0x7d,0xfb,0x49,0x6c,0xca,0x67,0xe1,0x3b},
		{0xd1,0x74,0xab,0x98,0xd2,0x77,0xd9,0xf5,0xa5,0x61,0x1c,0x2c,0x9f,0x41,0x9d,0x9f},
		{0x57,0xed,0xf4,0xa2,0x2b,0xe3,0xc9,0x55,0xac,0x49,0xda,0x2e,0x21,0x07,0xb6,0x7a},
	};

/*MD5 test suite:
MD5 ("") =
MD5 ("a") =
MD5 ("abc") =
MD5 ("message digest") =
MD5 ("abcdefghijklmnopqrstuvwxyz") =
MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =

MD5 ("123456789012345678901234567890123456789012345678901234567890123456
78901234567890") =  */

	for (i=0;i<(S32)NTESTSTRS;i++) {
		unsigned char hash[NUMHASHBYTES];
		gettrackhash((unsigned char *)(teststrs[i]),strlen(teststrs[i]),hash);
		logger("---------------------------------------\nmd5 test '%s' is\n",teststrs[i]);
		logger("ret value ");
		for (j=0;j<NUMHASHBYTES;j++)
			logger("%02x:",hash[j]);
		logger("\n");
		logger("should be ");
		for (j=0;j<NUMHASHBYTES;j++)
			logger("%02x:",hashs[i][j]);
		logger("\n");
	}
popdir();
}
