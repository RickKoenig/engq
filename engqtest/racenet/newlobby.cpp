#define INCLUDE_WINDOWS
//#define SELTRACKWID 7
#define RES3D
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include <d3d9.h>

#include "../engine7test/n_usefulcpp.h"
#include "gameinfo.h"
#include "stubhelper.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "../u_states.h"
#include "match_logon.h"
#include "../u_modelutil.h"
#include "../u_s_toon.h"
#include "m_vid_dx9.h"
#include "online_seltrack.h"
#include "carclass.h"
//#include "../particles.h"

extern struct weapinfo weapinfos[];

namespace newlobby {
#define LISTWEAPS
//#define ALLWEAPS

shape* rl;
shape* focus;
int whichgame;
vector<U32> trackrules;
vector<string> tracklist;
vector<string> trackpath;

listbox* LISTSELTRACKNAME,*RULES,*LISTIP;
listbox* LISTWEAP,*LISTWEAPHAVE;//,*LISTALTWEAP;,,*LISTALTWEAPHAVE;
hscroll* SLIDENUMP;
hscroll* SLIDE2009CARFIRST;
hscroll* SLIDENUMBOTS;
edit* EDITIP,*EDITNAME;
text* TEXTNUMP,*TEXTIP;
text* TEXTWEAPS;//,*TEXTALTWEAPS;
pbut* PIPDEL;
pbut* PBUTCANCEL;
pbut* PLAYBOT,*PLAYSERVER,*PLAYBCLIENT,*PLAYCLIENT;
text* TEXT2009CARFIRST;
text* TEXTNUMBOTS;

hscroll* HUES[NUMHSVS];
hscroll* SATS[NUMHSVS];
hscroll* VALS[NUMHSVS];
text* HUETEXT[NUMHSVS];
text* SATTEXT[NUMHSVS];
text* VALTEXT[NUMHSVS];

hscroll* SLIDERIMS;
text* TEXTRIMS;

hscroll* SLIDEGAMEID;
text* TEXTGAMEID;
hscroll* SLIDEGAMETYPE;
text* TEXTGAMETYPE;
hscroll* SLIDEPLAYERID;
text* TEXTPLAYERID;

#define SPTRESX 640
#define SPTRESY 480

script* thecarstxt; // list of valid cars
textureb* trackpic;

#ifdef LISTWEAPS
script* weaplist;
#endif

tree2* roottree;

const C8* cfgtxtname;
const C8* weaptxtname;

hsv otherhsvs[MAXOTHERS][NUMHSVS];

carclass* thecar;
U32 port;

trackinfo* tinf;

// particle system
particle* testpart;

// move the ui into hsv array
void fetchhsvs(hsv* hsvs)
{
	S32 i;
	for (i=0;i<NUMHSVS;++i) {
		hsvs[i].h=HUES[i]->getidx();
		hsvs[i].s=SATS[i]->getidx();
		hsvs[i].v=VALS[i]->getidx();
	}
}

void setweapvis()
{
	S32 r=RULES->getidx()+1;
	bool v = r==RULE_COMBAT || r==RULE_COMBATBOXES;
	LISTWEAP->setvis(v);
	LISTWEAPHAVE->setvis(v);
//	LISTALTWEAP->setvis(v);
//	LISTALTWEAPHAVE->setvis(v);
	TEXTWEAPS->setvis(v);
//	TEXTALTWEAPS->setvis(v);
}

void settrackrules()
{
	S32 trkidx=LISTSELTRACKNAME->getidx();
	if (trkidx>=0) {
		RULES->setidxc(trackrules[trkidx]-1);
		setweapvis();
	}
}

void loadtrackpic()
{
	string picname;
	string pat;
	int i;
	i=LISTSELTRACKNAME->getidx();
	if (i<0)
		i=0;
	pat=trackpath[i];
	picname=tinf->gettrackpicname(i);
pushandsetdir(pat.c_str());
	if (trackpic)
		trackpic->rc.deleterc(trackpic);
	if (fileexist(picname.c_str())) {
		trackpic=texture_create(picname.c_str());
		trackpic->load();
		trackpic->name="trackpic"; // don't conflict with skybox of same name 'data/skybox/prehistoric.jpg'
	} else
		trackpic=texture_create("maptestnck.tga");
popdir();
}


/*
//	p=LISTSELTRACKNAME->getidxname(i);
//	logger("found track pic '%s'\n",p);
	p=tracklist[i].c_str();
	strcpy(str,p);
	strcat(str,".jpg");
*/

void huevis(bool vis)
{
	S32 i;
	for (i=0;i<NUMHSVS;++i) {
		HUES[i]->setvis(vis);
		SATS[i]->setvis(vis);
		VALS[i]->setvis(vis);
		HUETEXT[i]->setvis(vis);
		SATTEXT[i]->setvis(vis);
		VALTEXT[i]->setvis(vis);
	}
}

void findhsvs()
{
	S32 i;
	for (i=0;i<NUMHSVS;++i) {
		char str[50];
		sprintf(str,"HUE%d",i+1);
		HUES[i]=rl->find<hscroll>(str);
		HUES[i]->setminmaxval(0,359);
		HUES[i]->setidx(0);
		sprintf(str,"SAT%d",i+1);
		SATS[i]=rl->find<hscroll>(str);
		SATS[i]->setminmaxval(0,319);
		SATS[i]->setidx(128);
		sprintf(str,"VAL%d",i+1);
		VALS[i]=rl->find<hscroll>(str);
		VALS[i]->setminmaxval(0,255);
		VALS[i]->setidx(128);
		sprintf(str,"HUETEXT%d",i+1);
		HUETEXT[i]=rl->find<text>(str);
		sprintf(str,"SATTEXT%d",i+1);
		SATTEXT[i]=rl->find<text>(str);
		sprintf(str,"VALTEXT%d",i+1);
		VALTEXT[i]=rl->find<text>(str);
	}
}

} // end namespace newlobby
using namespace newlobby;


void newlobby_init()
{
	cfgtxtname=wininfo.isalreadyrunning ? "altonline_seltrackcfg.txt" : "online_seltrackcfg.txt";
	weaptxtname=wininfo.isalreadyrunning ? "altcaweapons.txt" : "caweapons.txt";
	whichgame=NET_BOT;
	char str[50];
	S32 i,j,k,p=4,pt;
	script* sc;
	i=max(GX,640);
	j=max(GY,480);
	video_setupwindow(i,j);
	testpart=new particle;

#if 0
	// test trackinfo class
	{
		trackinfo tc;
		logger("numtracks %d\n",tc.getnumtracks());
		C8 findstr[]="headon";
		S32 idx=tc.findtrackidx(findstr);
		if (idx<0) idx=0;
		logger("findtrackidx for '%s' is %d\n",findstr,idx);
		logger("trackname '%s'\n",tc.gettrackname(idx).c_str());
		logger("tracknamebws '%s'\n",tc.gettracknamebws(idx).c_str());
		logger("trackdir '%s'\n",tc.gettrackdir(idx).c_str());
		logger("trackscale '%f'\n",tc.gettrackscale(idx));
		logger("trackstart '%f'\n",tc.gettrackstart(idx));
		logger("trackend '%f'\n",tc.gettrackend(idx));
		logger("trackskybox '%s'\n",tc.gettrackskybox(idx).c_str());
		logger("trackdefaultrules '%d'\n",tc.gettrackdefaultrules(idx));
	}
#endif
	logger("newlobby init\n");
	tinf=new trackinfo;
	pushandsetdir("racenetdata");
pushandsetdirdown("onlineopt");
#ifdef LISTWEAPS
	weaplist=new script("weapenable.txt");
#endif
	thecarstxt=new script("thecars.txt");
	init_res3d(SPTRESX,SPTRESY);
	rl = res_loadfile("newlobbyres.txt");
//text
	TEXT2009CARFIRST=rl->find<text>("TEXT2009CARFIRST");
//listbox
	LISTSELTRACKNAME=rl->find<listbox>("LISTSELTRACKNAME");
	RULES=rl->find<listbox>("RULES");
	LISTIP=rl->find<listbox>("LISTIP");
//listbox
	LISTWEAP=rl->find<listbox>("LISTWEAP");
//	LISTALTWEAP=rl->find<listbox>("LISTALTWEAP");
	LISTWEAPHAVE=rl->find<listbox>("LISTWEAPHAVE");
//	LISTALTWEAPHAVE=rl->find<listbox>("LISTALTWEAPHAVE");
//hscroll
	SLIDENUMP=rl->find<hscroll>("SLIDENUMP");
	SLIDENUMBOTS=rl->find<hscroll>("SLIDENUMBOTS");
	SLIDE2009CARFIRST=rl->find<hscroll>("SLIDE2009CARFIRST");
//edit
	EDITIP=rl->find<edit>("EDITIP");
	EDITNAME=rl->find<edit>("EDITNAME");
//text
	TEXTNUMP=rl->find<text>("TEXTNUMP");
	TEXTNUMBOTS=rl->find<text>("TEXTNUMBOTS");
	TEXTWEAPS=rl->find<text>("TEXTWEAPS");
//	TEXTALTWEAPS=rl->find<text>("TEXTALTWEAPS");
	TEXTIP=rl->find<text>("TEXTIP");
//pbut
	PIPDEL=rl->find<pbut>("PIPDEL");
	PBUTCANCEL=rl->find<pbut>("PBUTCANCEL");
	PLAYBOT=rl->find<pbut>("PLAYBOT");
	PLAYSERVER=rl->find<pbut>("PLAYSERVER");
	PLAYBCLIENT=rl->find<pbut>("PLAYBCLIENT");
	PLAYCLIENT=rl->find<pbut>("PLAYCLIENT");
// hsv
	findhsvs();
// rims
	SLIDERIMS=rl->find<hscroll>("SLIDERIMS");
//	SLIDERIMS->setminmaxval(1,12);
pushandsetdir("racenetdata/rims");
	for (i=1;i<100;++i) {
		sprintf(str,"rim%02d.bwo",i);
		if (!fileexist(str)) {
			--i;
			break;
		}
	}
	SLIDERIMS->setminmaxval(1,i);
popdir();
	TEXTRIMS=rl->find<text>("TEXTRIMS");

	SLIDEGAMEID=rl->find<hscroll>("SLIDERGAMEID");
	SLIDEGAMEID->setminmaxval(0,1000);
	TEXTGAMEID=rl->find<text>("TEXTGAMEID");
	SLIDEGAMETYPE=rl->find<hscroll>("SLIDERGAMETYPE");
	SLIDEGAMETYPE->setminmaxval(0,25);
	TEXTGAMETYPE=rl->find<text>("TEXTGAMETYPE");
	SLIDEPLAYERID=rl->find<hscroll>("SLIDERPLAYERID");
	SLIDEPLAYERID->setminmaxval(0,1000);
	TEXTPLAYERID=rl->find<text>("TEXTPLAYERID");
//edit
	SLIDENUMP->setminmaxval(1,16);
//	SLIDENUMBOTS->setminmaxval(0,15);
	LISTSELTRACKNAME->setidxc(0);
	for (i=1;i<MAX_RULES;i++)
		RULES->addstring(rules[i].rulename);
	S32 nsc=0;
	nsc=tinf->getnumtracks();
	for (i=0;i<nsc;++i) {
		trackrules.push_back(tinf->gettrackdefaultrules(i));
		string tname=tinf->gettrackname(i);
		tracklist.push_back(tname);
		trackpath.push_back(tinf->gettrackdir(i));
		LISTSELTRACKNAME->addstring(tname.c_str());
	}
	if (fileexist("newlobby.cfg")) {
		sc=new script("newlobby.cfg");
		S32 nsc=sc->num();
		for (i=0;i<nsc;i++)
			LISTIP->addstring(sc->idx(i).c_str());
		LISTIP->sort(listbox::SK_IPS);
		delete sc;
	}
	S32 pt2=0;
	S32 ptb=0;
	S32 tt0=0;
	mainstubinfo.names.cd.c_rims=1;
	botinfos[0].c_rims=1;
	botinfos[1].c_rims=1;
	botinfos[2].c_rims=1;
	botinfos[3].c_rims=1;
	if (fileexist(cfgtxtname)) {
		sc=new script(cfgtxtname);
		S32 nsc=sc->num();
		for (i=0;i<nsc;i+=2) {
//			if (!strcmp(sc->idx(i).c_str(),"onlinetracknum")) {
//				LISTSELTRACKNAME->setidxc(atoi(sc->idx(i+1).c_str())-1);
			if (!strcmp(sc->idx(i).c_str(),"trackname")) {
				vector<string>::iterator found=find(tracklist.begin(),tracklist.end(),sc->idx(i+1));
				if (found!=tracklist.end()) {
					tt0=distance(tracklist.begin(),found);
					LISTSELTRACKNAME->setidxc(tt0);
				}
			} else if (!strcmp(sc->idx(i).c_str(),"rules")) {
				RULES->setidxc(atoi(sc->idx(i+1).c_str())-1);
			} else if (!strcmp(sc->idx(i).c_str(),"serverip")){
				EDITIP->settname(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"port")){
				port=atoi(sc->idx(i+1).c_str());
//			} else if (!strcmp(sc->idx(i).c_str(),"car2009first")) {
//				pt2=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"numplayers")) {
				pt=atoi(sc->idx(i+1).c_str());
				if (pt>=1)
					p=pt;
			} else if (!strcmp(sc->idx(i).c_str(),"numcars")) {
				pt=atoi(sc->idx(i+1).c_str());
				if (pt>=1)
					p=pt;
			} else if (!strcmp(sc->idx(i).c_str(),"numbots")) {
				ptb=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"nettype")) { // will get set by a button press later
			} else if (!strcmp(sc->idx(i).c_str(),"carbody")) {
				strcpy(mainstubinfo.names.cd.c_body,sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"rims")) {
				mainstubinfo.names.cd.c_rims=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altcarbody")) {
				strcpy(botinfos[0].c_body,sc->idx(i+1).c_str());
				strcpy(botinfos[1].c_body,sc->idx(i+1).c_str());
				strcpy(botinfos[2].c_body,sc->idx(i+1).c_str());
				strcpy(botinfos[3].c_body,sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altrims")) {
				botinfos[0].c_rims=atoi(sc->idx(i+1).c_str());
				botinfos[1].c_rims=atoi(sc->idx(i+1).c_str());
				botinfos[2].c_rims=atoi(sc->idx(i+1).c_str());
				botinfos[3].c_rims=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altcarbody2")) {
				strcpy(botinfos[1].c_body,sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altcarbody3")) {
				strcpy(botinfos[2].c_body,sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altcarbody4")) {
				strcpy(botinfos[3].c_body,sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altrims2")) {
				botinfos[1].c_rims=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altrims3")) {
				botinfos[2].c_rims=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"altrims4")) {
				botinfos[3].c_rims=atoi(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"carname")) {
				EDITNAME->settname(sc->idx(i+1).c_str());
			} else if (!strcmp(sc->idx(i).c_str(),"gameid")) {
				SLIDEGAMEID->setidx(atoi(sc->idx(i+1).c_str()));
			} else if (!strcmp(sc->idx(i).c_str(),"gametype")) {
				string& s=sc->idx(i+1);
				C8 gc=s[0];
				S32 indx=gc-'A';
				SLIDEGAMETYPE->setidx(indx);
			} else if (!strcmp(sc->idx(i).c_str(),"playerid")) {
				SLIDEPLAYERID->setidx(atoi(sc->idx(i+1).c_str()));
			} else {
// get banks of hsvs into otherhsvs
				for (k=0;k<=MAXOTHERS;++k) {
					for (j=0;j<NUMHSVS;++j) {
						C8 str[50];
						sprintf(str,"hue%d_%d",k,j);
						if (!strcmp(sc->idx(i).c_str(),str))
							otherhsvs[k][j].h=atoi(sc->idx(i+1).c_str());
						sprintf(str,"sat%d_%d",k,j);
						if (!strcmp(sc->idx(i).c_str(),str))
							otherhsvs[k][j].s=atoi(sc->idx(i+1).c_str());
						sprintf(str,"val%d_%d",k,j);
						if (!strcmp(sc->idx(i).c_str(),str))
							otherhsvs[k][j].v=atoi(sc->idx(i+1).c_str());
					}
				}
// get bank0 of hsvs into sliders
				for (j=0;j<NUMHSVS;++j) {
					HUES[j]->setidx(otherhsvs[0][j].h);
					SATS[j]->setidx(otherhsvs[0][j].s);
					VALS[j]->setidx(otherhsvs[0][j].v);
				}
			}
		} 
		delete sc;
	}
	pt2=0;
	for (i=0;i<thecarstxt->num();++i) {
		if (!my_stricmp(mainstubinfo.names.cd.c_body,thecarstxt->idx(i).c_str())) {
			pt2=i;
			break;
		}
	}
	SLIDE2009CARFIRST->setminmaxval(0,thecarstxt->num()-1);
	SLIDE2009CARFIRST->setidx(pt2);
	SLIDERIMS->setidx(mainstubinfo.names.cd.c_rims);
	SLIDENUMP->setidx(p);
	SLIDENUMBOTS->setminmaxval(0,p-1);
	if (p==1)
		sprintf(str,"One Car");
	else
		sprintf(str,"%d Cars",p);
	TEXTNUMP->settname(str);
	TEXT2009CARFIRST->settname(thecarstxt->idx(SLIDE2009CARFIRST->getidx()).c_str());
	if (ptb>=p)
		ptb=p-1;
	SLIDENUMBOTS->setidx(ptb);
popdir();

#ifdef ALLWEAPS
	for (i=0;i<MAX_WEAPKIND;i++) {
		LISTWEAP->addstring(weapinfos[i].name);
		LISTALTWEAP->addstring(weapinfos[i].name);
	} 
#endif
#ifdef LISTWEAPS
	for (i=0;i<weaplist->num();i++) {
		LISTWEAP->addstring(weaplist->idx(i).c_str());
//		LISTALTWEAP->addstring(weaplist->idx(i).c_str());
	} 
#endif
	LISTWEAP->sort();
//	LISTALTWEAP->sort();
	sc=new script(weaptxtname);
	nsc=sc->num();
/*	for (i=0;i<nsc;i++)
		for (j=0;j<MAX_WEAPKIND;j++)
			if (!strcmp(weapinfos[j].name,sc->idx(i).c_str())) {
				j=LISTWEAP->findstring(sc->idx(i).c_str());
				if (j>=0) {
					logger("found weapon '%s'\n",sc->idx(i).c_str());
					LISTWEAP->removeidx(j);
					LISTWEAPHAVE->addstring(sc->idx(i).c_str());
				}
				break;
			} */
	for (i=0;i<nsc;++i) {
		j=atoi(sc->idx(i).c_str()); // weap number
		k=LISTWEAP->findstring(weapinfos[j].name); // index into weap list
		if (k>=0) {
			logger("found weapon '%s'\n",weapinfos[j].name);
			LISTWEAP->removeidx(k);
			LISTWEAPHAVE->addstring(weapinfos[j].name);
		}
	}
	delete sc;
/*	sc=new script("altcaweapons.txt");
	nsc=sc->num();
	for (i=0;i<nsc;i++)
		for (j=0;j<MAX_WEAPKIND;j++)
			if (!strcmp(weapinfos[j].name,sc->idx(i).c_str())) {
				j=LISTALTWEAP->findstring(sc->idx(i).c_str());
				if (j>=0) {
					LISTALTWEAP->removeidx(j);
					LISTALTWEAPHAVE->addstring(sc->idx(i).c_str());
				}
				break;
			}
	delete sc; */
	loadtrackpic();
	setweapvis();
	focus=0;
	if (matchcl) {
		EDITNAME->settname(username);
		TEXTIP->setvis(0);
		LISTIP->setvis(0);
		EDITIP->setvis(0);
		PIPDEL->setvis(0);
		if (cgi.ishost) {
			PLAYCLIENT->setvis(0);
		} else {
			PLAYSERVER->setvis(0);
		}
	}
// 3d stuff
// setup viewport
	memset(&mainvp,0,sizeof(mainvp));
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=400;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARWB;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
	mainvp.camtrans=pointf3x(0,1.29f,-1.94f);
	mainvp.camrot=pointf3x(.507f,0,0);
	mainvp.xres=192*WY/SPTRESY;
	mainvp.yres=192*WY/SPTRESY;
	mainvp.xstart=445*WY/SPTRESY;
	mainvp.ystart=220*WY/SPTRESY;
	roottree=new tree2("roottree");
// end stup viewport
// load an explosion fountain particle, spawn every 3 seconds
	tree2* par=particle::curparticle->genpart(PK_NULL,spawnproc);
	par->scale=pointf3x(10,10,10);
//	par->trans.y=.5f;
//	par->rotvel.y=TWOPI/30/9;
	par->treedissolvecutoff=.2f;
	roottree->linkchild(par);
// done load a test part
// skybox
	pushandsetdir("skybox");
	make_envv2tex("CUB_prehistoric.jpg");
	popdir();
// end skybox
// car
	thecar = new carclass(TEXT2009CARFIRST->gettname());
	roottree->linkchild(thecar->getcartree());
	thecar->getcartree()->rotvel.y=TWOPI/30/9;
	hsv hsvs[NUMHSVS];
	fetchhsvs(hsvs);
	thecar->changehsv(hsvs);
	thecar->changerims(mainstubinfo.names.cd.c_rims);
	huevis(thecar->getcanhsv()!=carclass::COLNONE); // ui, turn on hsvs for cars that can hsvs (rgb)
// end car
/*
// test gameinf structure
pushandsetdir("racenetdata/onlineopt");
	const C8* parsethis=fileload("gameinfo.txt");
popdir();
	gameinf gi(parsethis);
	delete parsethis;
	gi.save("gameinfosv.txt"); */
}

void newlobby_proc()
{
	S32 togvidmode;
	int i,j;
	const char *p;
	char str[500];
	switch(thecar->getcanhsv()) {
	case carclass::COLHSV:
		for (i=0;i<NUMHSVS;++i) {
			sprintf(str,"H %d",HUES[i]->getidx());
			HUETEXT[i]->settname(str);
			sprintf(str,"S %d",SATS[i]->getidx());
			SATTEXT[i]->settname(str);
			sprintf(str,"V %d",VALS[i]->getidx());
			VALTEXT[i]->settname(str);
			HUES[i]->setminmaxval(0,359);
			SATS[i]->setminmaxval(0,319);
			VALS[i]->setminmaxval(0,255);
		}
		break;
	case carclass::COLRGB:
		for (i=0;i<NUMHSVS;++i) {
			sprintf(str,"R %d",HUES[i]->getidx());
			HUETEXT[i]->settname(str);
			sprintf(str,"G %d",SATS[i]->getidx());
			SATTEXT[i]->settname(str);
			sprintf(str,"B %d",VALS[i]->getidx());
			VALTEXT[i]->settname(str);
			HUES[i]->setminmaxval(0,255);
			HUES[i]->setidx(HUES[i]->getidx());
			SATS[i]->setminmaxval(0,255);
			SATS[i]->setidx(SATS[i]->getidx());
			VALS[i]->setminmaxval(0,255);
			VALS[i]->setidx(VALS[i]->getidx());
		}
		break;
	default:
		break;
	};
	sprintf(str,"Rim %d",SLIDERIMS->getidx());
	TEXTRIMS->settname(str);
	sprintf(str,"Gameid %d",SLIDEGAMEID->getidx());
	TEXTGAMEID->settname(str);
	sprintf(str,"Gametype :%c:",'A'+SLIDEGAMETYPE->getidx());
	TEXTGAMETYPE->settname(str);
	sprintf(str,"Playerid %d",SLIDEPLAYERID->getidx());
	TEXTPLAYERID->settname(str);

	i=SLIDENUMP->getidx();
	j=SLIDENUMBOTS->getidx();
	if (j>=i)
		j=i-1;
//	if (i+j>16)
//		j=16-i;
	S32 h=i-j;
	if (j==1)
		if (h==1)
			sprintf(str,"One Bot, One Human");
		else
			sprintf(str,"One Bot, %d Humans",h);
	else if (j==0)
		if (h==1)
			sprintf(str,"Zero Bots, One Human");
		else
			sprintf(str,"Zero Bots, %d Humans",h);
	else
		if (h==1)
			sprintf(str,"%d Bots, One Human",j);
		else
			sprintf(str,"%d Bots, %d Humans",j,h);
	TEXTNUMBOTS->settname(str);

	if (KEY==K_ESCAPE) {
		poporchangestate(STATE_MAINMENU);
	}
	if (KEY=='s') {
		togvidmode=videoinfo.video_maindriver;
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_OLDCARENALOBBY);
	}
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus();
		shape* focusa = ret>=0 ? focus : 0;
		if (focusa==PBUTCANCEL) {
			whichgame=NET_BOT;
			poporchangestate(STATE_MAINMENU);
		} else if (focusa==PLAYBOT) {
			whichgame=NET_BOT;
			changestate(STATE_OLDCARENATOGAME);
		} else if (focusa==PLAYSERVER) {
			whichgame=NET_SERVER;
			changestate(STATE_OLDCARENATOGAME);
		} else if (focusa==PLAYBCLIENT) {
			whichgame=NET_BCLIENT;
			changestate(STATE_OLDCARENATOGAME);
		} else if (focusa==PLAYCLIENT) {
			whichgame=NET_CLIENT;
			changestate(STATE_OLDCARENATOGAME);
		} else if (focusa==SLIDENUMP) {
			S32 rmval=SLIDENUMP->getidx();
			if (rmval==1)
				sprintf(str,"One Car");
			else
				sprintf(str,"%d Cars",rmval);
			TEXTNUMP->settname(str);
			SLIDENUMBOTS->setminmaxval(0,rmval-1);
			SLIDENUMBOTS->setidx(SLIDENUMBOTS->getidx());
		} else if (focusa==SLIDE2009CARFIRST) {
			TEXT2009CARFIRST->settname(thecarstxt->idx(SLIDE2009CARFIRST->getidx()).c_str());
			delete thecar;
			thecar=new carclass(TEXT2009CARFIRST->gettname());
			thecar->getcartree()->rotvel.y=TWOPI/30/9;
			roottree->linkchild(thecar->getcartree()); // pass ownership of 'tree' from thecar to rootree
			hsv hsvs[NUMHSVS];
			fetchhsvs(hsvs);
			thecar->changehsv(hsvs);
			thecar->changerims(SLIDERIMS->getidx());
			huevis(thecar->getcanhsv()!=carclass::COLNONE);
		} else if (focusa==SLIDERIMS) {
			thecar->changerims(SLIDERIMS->getidx());
		} else if (focusa==LISTWEAPHAVE) {
			S32 rmval=LISTWEAPHAVE->getidx();
			p=LISTWEAPHAVE->getidxname(rmval);
			LISTWEAP->addstring(p);
			LISTWEAPHAVE->removeidx(rmval);
			LISTWEAP->sort();
			LISTWEAP->setidxc(LISTWEAP->getidx());
		} else if (focusa==LISTWEAP) {
			if (LISTWEAPHAVE->getnumidx()<MAXWEAPONSCARRY) {
				S32 rmval=LISTWEAP->getidx();
				p=LISTWEAP->getidxname(rmval);
				LISTWEAPHAVE->addstring(p);
				LISTWEAP->removeidx(rmval);
				LISTWEAPHAVE->sort();
				LISTWEAPHAVE->setidxc(LISTWEAPHAVE->getidx());
			}
/*		} else if (focusa==LISTALTWEAPHAVE) {
			S32 rmval=LISTALTWEAPHAVE->getidx();
			p=LISTALTWEAPHAVE->getidxname(rmval);
			LISTALTWEAP->addstring(p);
			LISTALTWEAPHAVE->removeidx(rmval);
			LISTALTWEAP->sort();
			LISTALTWEAP->setidxc(LISTALTWEAP->getidx());
		} else if (focusa==LISTALTWEAP) {
			if (LISTALTWEAPHAVE->getnumidx()<MAXWEAPONSCARRY) {
				S32 rmval=LISTALTWEAP->getidx();
				p=LISTALTWEAP->getidxname(rmval);
				LISTALTWEAPHAVE->addstring(p);
				LISTALTWEAP->removeidx(rmval);
				LISTALTWEAPHAVE->sort();
				LISTALTWEAPHAVE->setidxc(LISTALTWEAPHAVE->getidx());
			} */
		} else if (focusa==PIPDEL) {
			i=LISTIP->getidx();
			if (i>=0)
				LISTIP->removeidx(i);
			LISTIP->setidxc(-1);
		} else if (focusa==EDITIP) {
			p=EDITIP->gettname();
			i=LISTIP->findstring(p);
			if (i==-1) {
				LISTIP->addstring(p);
				LISTIP->sort(listbox::SK_IPS);
			}
			LISTIP->setidxc(-1);
		} else if (focusa==LISTIP) {
			S32 rmval=LISTIP->getidx();
			p=LISTIP->getidxname(rmval);
			EDITIP->settname(p);
		} else if (focusa==LISTSELTRACKNAME) {
			loadtrackpic();
			settrackrules();
		} else if (focusa==RULES) {
			setweapvis();
		} else {
			for (i=0;i<NUMHSVS;++i) {
				if (focusa==HUES[i] || focusa==SATS[i] || focusa==VALS[i]) {
					hsv hsvs[NUMHSVS];
					fetchhsvs(hsvs);
					thecar->changehsv(hsvs);
					break;
				}
			}
		}
	}
	doflycam(&mainvp);
	roottree->proc();
}

void newlobby_draw3d()
{
	video_sprite_begin(
	  SPTRESX,SPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  0,
	  0);
	rl->draw3d();
	video_sprite_draw(trackpic,F32WHITE, 25,166,140,105);
	video_sprite_end(); 
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);
}

void newlobby_exit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete testpart;
	if (wininfo.closerequested && matchcl) {
		freesocker(matchcl);
		matchcl=0;
	}
#ifdef LISTWEAPS
	delete weaplist;
	weaplist=0;
#endif
	int i,j,n;
	FILE *fp;
	trackpic->rc.deleterc(trackpic);
	trackpic=0;
pushandsetdirdown("onlineopt");
	n=LISTIP->getnumidx();
	fp=fopen2("newlobby.cfg","w");
	for (i=0;i<n;i++)
		fprintf(fp,"%s\n",LISTIP->getidxname(i));
	fclose(fp);
popdir();
	n=LISTWEAPHAVE->getnumidx();
	fp=fopen2(weaptxtname,"w");
//	fprintf(fp,"[\"Blank\"");
	for (i=0;i<n;i++) {
		const C8* wn=LISTWEAPHAVE->getidxname(i);
//		fprintf(fp,",\"%s\"",LISTWEAPHAVE->getidxname(i));
		for (j=0;j<MAX_WEAPKIND;++j) {
			if (!my_stricmp(wn,weapinfos[j].name)) {
				fprintf(fp,"%2d # %s\n",j,wn);
				break;
			}
		}
	}
//	fprintf(fp,"]");
	fclose(fp);

/*	n=LISTALTWEAPHAVE->getnumidx();
	fp=fopen2("altcaweapons.txt","w");
	fprintf(fp,"[\"Blank\"");
	for (i=0;i<n;i++)
		fprintf(fp,",\"%s\"",LISTALTWEAPHAVE->getidxname(i));
	fprintf(fp,"]");
	fclose(fp); */
pushandsetdirdown("onlineopt");
	fp=fopen2(cfgtxtname,"w");
/*	i=LISTSELTRACKNAME->getidx()+1;
	if (i<1)
		i=1;
	fprintf(fp,"onlinetracknum %d\n",i); */
	i=LISTSELTRACKNAME->getidx();
	fprintf(fp,"trackname \"%s\"\n",tracklist[i].c_str());
	i=RULES->getidx()+1;
	if (i<1)
		i=1;
	fprintf(fp,"rules %d\n",i);
	S32 ci=SLIDE2009CARFIRST->getidx(); // get car index into thecars.txt
//	fprintf(fp,"car2009first %d\n",i);
	i=SLIDENUMP->getidx(); // numplayers
	j=SLIDENUMBOTS->getidx();
	if (j>=i)
		j=i-1;
//	if (i+j>16)
//		j=16-i;
	if (whichgame==NET_BOT) {
		fprintf(fp,"numplayers 1\n");
		fprintf(fp,"numcars %d\n",i);
		fprintf(fp,"numbots %d\n",j);
	} else if (whichgame==NET_SERVER) {
		fprintf(fp,"numplayers %d\n",i);
		fprintf(fp,"numcars %d\n",i);
		fprintf(fp,"numbots %d\n",j);
	} else if (whichgame==NET_CLIENT || whichgame==NET_BCLIENT) {
		fprintf(fp,"numplayers -1\n");
		fprintf(fp,"numcars %d\n",i);
		fprintf(fp,"numbots %d\n",j);
	} else { // default to bot
		fprintf(fp,"numplayers 1\n");
		fprintf(fp,"numcars %d\n",i);
		fprintf(fp,"numbots %d\n",j);
	}
	fprintf(fp,"nettype %d\n",whichgame);
	if (matchcl && !cgi.ishost) {
		logger("match client, servers ip is '%s'\n",ip2str(cgi.ip));
		fprintf(fp,"serverip %s // from cgi.ip\n",ip2str(cgi.ip));
	} else {
		fprintf(fp,"serverip %s\n",EDITIP->gettname());
	}
	fprintf(fp,"port %d\n",port);
	fprintf(fp,"carname \"%s\"\n",EDITNAME->gettname());
//	fprintf(fp,"carbody \"%s\"\n",mainstubinfo.names.c_body);
	fprintf(fp,"carbody \"%s\"\n",thecarstxt->idx(ci).c_str());
	fprintf(fp,"rims %d\n",SLIDERIMS->getidx());
	fprintf(fp,"gameid %d\n",SLIDEGAMEID->getidx());
	fprintf(fp,"gametype %c\n",SLIDEGAMETYPE->getidx()+'A');
	fprintf(fp,"playerid %d\n",SLIDEPLAYERID->getidx());
	fprintf(fp,"carpaint \"paint11\"\n");
	fprintf(fp,"cardecal \"decal12\"\n");
	fprintf(fp,"altcarname \"BILL\"\n");
	fprintf(fp,"altcarbody \"%s\"\n",botinfos[0].c_body);
	fprintf(fp,"altrims %d\n",botinfos[0].c_rims);
	fprintf(fp,"altcarpaint \"paint03\"\n");
	fprintf(fp,"altcardecal \"decal04\"\n");
	fprintf(fp,"altcarname2 \"TIM\"\n");
	fprintf(fp,"altcarbody2 \"%s\"\n",botinfos[1].c_body);
	fprintf(fp,"altrims2 %d\n",botinfos[1].c_rims);
	fprintf(fp,"altcarpaint2 \"paint05\"\n");
	fprintf(fp,"altcardecal2 \"decal06\"\n");
	fprintf(fp,"altcarname3 \"SHIFTER\"\n");
	fprintf(fp,"altcarbody3 \"%s\"\n",botinfos[2].c_body);
	fprintf(fp,"altrims3 %d\n",botinfos[2].c_rims);
	fprintf(fp,"altcarpaint3 \"paint07\"\n");
	fprintf(fp,"altcardecal3 \"decal08\"\n");
	fprintf(fp,"altcarname4 \"JR\"\n");
	fprintf(fp,"altcarbody4 \"%s\"\n",botinfos[3].c_body);
	fprintf(fp,"altrims4 %d\n",botinfos[3].c_rims);
	fprintf(fp,"altcarpaint4 \"paint09\"\n");
	fprintf(fp,"altcardecal4 \"decal10\"\n");
	fetchhsvs(otherhsvs[0]);
	for (j=0;j<MAXOTHERS;++j) {
		for (i=0;i<NUMHSVS;++i) {
			fprintf(fp,"hue%d_%d %d\n",j,i,otherhsvs[j][i].h);
			fprintf(fp,"sat%d_%d %d\n",j,i,otherhsvs[j][i].s);
			fprintf(fp,"val%d_%d %d\n",j,i,otherhsvs[j][i].v);
		}
	}
	fclose(fp);
popdir();
	ol_opt.backtoopt=1;
	delete rl;
	exit_res3d();
	trackrules.clear();
	tracklist.clear();
	trackpath.clear();
popdir();
	delete thecar; // call before 'delete roottree'
	delete roottree;
	roottree=0;
	free_envv2tex();
	delete thecarstxt;
	thecarstxt=0;
	delete tinf;
	tinf=0;
}

// temp
struct cardata botinfos[MAXSTUBCARS];
struct stbinfo mainstubinfo;
//struct rule rules[MAX_RULES];
//RULE
struct rule rules[MAX_RULES]={
//	rulename			windowname				scorelinet			scorelineb		useweap		ctf useboxes 
	{NULL},
	{"no weapons",		"NO WEAPONS 6.19",		"scrlnetop2.mxs",	NULL,				0,		0,   0},	
	{"combat",			"CARENA VERSION 6.19","scrlnetop2.mxs","scrlnebtm3.mxs",		1,		0,   0},	
	{"CTF no weapons",	"CTF NO WEAPONS 13.29",	"scrlnetop3.mxs",	NULL,				0,		1,   0},	
	{"CTF weapons",		"CTF WEAPONS 13.29",	"scrlnetop3.mxs",	"scrlnebtm3.mxs",	1,		1,   0},	
	{"stunt",			"STUNT 13.29",			"scrlnetop2.mxs",	NULL,				0,		0,   0},	
	{"no weapons boxes","NO WEAPONS 13.29",		"scrlnetop2.mxs",	NULL,				0,		0,   1},	
	{"combat boxes",	"HIGHWAY 35 VERSION 14.17","scrlnetop2.mxs","scrlnebtm3.mxs",	1,		0,   1},	
}; 
struct onlineopt ol_opt;
