#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "../engine7test/n_usefulcpp.h"

#include "gameinfo.h"
#include "stubhelper.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "online_seltrack.h"


//static int onlinetracknum;
//static script* thetracks;
static script *thecars;

// return -1 if no track, 1 and up otherwise
/*static S32 findtrackidx(const script& tracklist,const C8* shorttrackname)
{
	S32 ret=-1;
	S32 i;
	for (i=0;i<tracklist.num();i+=SELTRACKWID) {
		const string& stn=tracklist.idx(i+1);
		U32 fst=stn.rfind("/");
		if (fst==string::npos)
			fst=stn.rfind("\\");
		if (fst==string::npos)
			errorexit("can't parse trackname '%s'",stn.c_str());
		++fst;
		U32 lst=stn.rfind(".");
		string trknam=stn.substr(fst,lst-fst);
		if (!my_stricmp(trknam.c_str(),shorttrackname)) {
			ret=i/SELTRACKWID+1;
			break;
		}
	}
	return ret;
}
*/
S32 weapxlate[100]; // new to old

S32 inverseweapxlate(S32 old) // old to new
{
	if (old<0)
		return -1;
	S32 i;
	for (i=0;i<100;++i)
		if (weapxlate[i]==old)
			return i;
	return 0; // unknown
}


// xfer members from 'gameinf' to 'ol_opt'
static void patch_globalgame()
{
	if (!globalgame)
		return;
// net
	ol_opt.serverip=globalgame->ni.ip;
	ol_opt.port=globalgame->ni.port;
	ol_opt.nt=globalgame->ni.nt;
	ol_opt.gt=globalgame->ni.gt;
	ol_opt.gameid=globalgame->ni.gameid;
	switch(globalgame->ni.nt) {
	case NET_BOT:
		ol_opt.nplayers=1;
		ol_opt.ncars=globalgame->ni.nplayers;
		ol_opt.nbots=0;
		break;
	case NET_SERVER:
		ol_opt.nplayers=globalgame->ni.nhumans;
		ol_opt.ncars=globalgame->ni.nhumans;
		ol_opt.nbots=0;
		break;
	case NET_BCLIENT:
		ol_opt.nplayers=-1;
		ol_opt.ncars=globalgame->ni.nhumans+globalgame->ni.nbots;
		ol_opt.nbots=globalgame->ni.nbots;
		break;
	case NET_CLIENT:
		ol_opt.nplayers=-1;
		ol_opt.ncars=globalgame->ni.nhumans;
		ol_opt.nbots=0;
		break;
	};

// track
// get tracknum given trackname
/*	onlinetracknum=1;
	for (i=0;i<thetracks->num();i+=SELTRACKWID) {
		const string& stn=thetracks->idx(i+1);
		U32 fst=stn.rfind("/");
		if (fst==string::npos)
			fst=stn.rfind("\\");
		if (fst==string::npos)
			errorexit("can't parse trackname '%s'",stn.c_str());
		++fst;
		U32 lst=stn.rfind(".");
		string trknam=stn.substr(fst,lst-fst);
		if (!my_stricmp(trknam.c_str(),globalgame->ti.trackname.c_str())) {
			onlinetracknum=i/SELTRACKWID+1;
			break;
		}
	} */
//	onlinetracknum=findtrackidx(*thetracks,globalgame->ti.trackname.c_str());
/*	if (onlinetracknum<=0) { // track not found, use first one and be a bot game
		onlinetracknum=1;
		ol_opt.nplayers=1;
		ol_opt.ncars=globalgame->ni.nplayers;
	}
*/
	strcpy(ol_opt.ntrackname,globalgame->ti.trackname.c_str());
	olracecfg.rules=globalgame->ti.rules;

// player
	strcpy(mainstubinfo.names.cd.c_name,globalgame->pi.username.c_str());
// get carnum given carmake
/*	S32 onlinecarnum=0;
	S32 i;
	for (i=0;i<thecars->num();++i) {
		if (!my_stricmp(globalgame->pi.carmake.c_str(),thecars->idx(i).c_str())) {
			onlinecarnum=i;
			break;
		}
	} */
	ol_opt.playerid=globalgame->pi.pid;
//	ol_opt.car2009first=onlinecarnum;
	strcpy(mainstubinfo.names.cd.c_body,globalgame->pi.carmake.c_str());
	mainstubinfo.names.cd.c_rims=globalgame->pi.rims;
	memcpy(mainstubinfo.names.cd.c_hsvs,globalgame->pi.hsvs,sizeof(mainstubinfo.names.cd.c_hsvs));
// medals have
	memcpy(ol_opt.medalsearned,globalgame->pi.medalsearned,sizeof(globalgame->pi.medalsearned));
}

void patch_weapons()
{
	if (!globalgame)
		return;
	mainstubinfo.names.cd.c_nweap=0;
	U32 i;
	U32 nw=min(globalgame->pi.weapons.size(),(U32)MAXWEAPONSCARRY);
	for (i=0;i<nw;++i) {
		S32 nw=globalgame->pi.weapons[i];
		S32 ow=weapxlate[nw];
		mainstubinfo.names.cd.c_weaps[i]=ow;
	}
	mainstubinfo.names.cd.c_nweap=nw;
}

void patch_energies()
{
	static int energyxlate[MAXENERGIES]={ // new to old
		4,
		5,
		1,
		0,
		2,
		3,
	};
	if (!globalgame)
		return;
	U32 i;
	for (i=0;i<MAXENERGIES;++i)
		mainstubinfo.names.cd.c_energies[energyxlate[i]]=globalgame->pi.energies[i];
}

// load 'ol_op't structure from online_seltrack.txt and online_seltrackcfg.txt
void seltrack_loadconfig()
{
	int i,j,k;
pushandsetdir("racenetdata/onlineopt");
	script* sc=new script("weapxlate.txt");
	memset(weapxlate,0,sizeof(weapxlate));
	for (i=0;i<sc->num();++i)
		weapxlate[i]=atoi(sc->idx(i).c_str());
popdir();
	delete sc;
	memset(&mainstubinfo,0,sizeof(mainstubinfo));
	memset(botinfos,0,sizeof(botinfos));
	i=ol_opt.backtoopt;
	memset(&ol_opt,0,sizeof(ol_opt));
	ol_opt.backtoopt=i;
//	ol_opt.trackscale=1.0f;
	ol_opt.nplayers=1;
	ol_opt.ncars=4;
	pushandsetdir("racenetdata/onlineopt");
//	thetracks=new script("online_seltrack.txt");
//	if (!thetracks ||!thetracks->num() || thetracks->num()%SELTRACKWID)
//		errorexit("bad online_seltrack.txt");
	thecars=new script("thecars.txt");
	if (!thecars ||!thecars->num())
		errorexit("bad thecars.txt");
	const C8* cfgtxtname=wininfo.isalreadyrunning ? "altonline_seltrackcfg.txt" : "online_seltrackcfg.txt";
	if (fileexist(cfgtxtname)) {
		script sc(cfgtxtname);
		S32 nsc=sc.num();
		for (i=0;i<nsc;i+=2) {
/*			if (!strcmp(sc.idx(i).c_str(),"onlinetracknum")) {
				onlinetracknum=atoi(sc.idx(i+1).c_str());
				if (onlinetracknum<=0)
					errorexit("invalid tracknum %d",onlinetracknum);
//				olracecfg.stubtracknum=onlinetracknum; */
			if (!strcmp(sc.idx(i).c_str(),"trackname")) {
				strcpy(ol_opt.ntrackname,sc.idx(i+1).c_str());
//				onlinetracknum=findtrackidx(*thetracks,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"rules")) {
				olracecfg.rules=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"numplayers")) {
				ol_opt.nplayers=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"numcars")) {
				ol_opt.ncars=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"numbots")) {
				ol_opt.nbots=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"serverip")) {
				ol_opt.serverip=str2ip(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"nettype")) {
				ol_opt.nt=(nettype)atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"port")) {
				ol_opt.port=atoi(sc.idx(i+1).c_str());
/*			} else if (!strcmp(sc.idx(i).c_str(),"car2009first")) {
				if (true)
//				if (!wininfo.isalreadyrunning)
					ol_opt.car2009first=atoi(sc.idx(i+1).c_str());
*/
			} else if (!strcmp(sc.idx(i).c_str(),"carname")) {
				if (true)
//				if (!wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_name,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"carbody")) {
				if (true)
//				if (!wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_body,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"cardecal")) {
				if (true)
//				if (!wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_decal,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"rims")) {
				if (true)
//				if (!wininfo.isalreadyrunning)
					mainstubinfo.names.cd.c_rims=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"carpaint")) {
				if (true)
//				if (!wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_paint,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcarname")) {
				if (false)
//				if (wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_name,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcarbody")) {
//				if (false)
//				if (wininfo.isalreadyrunning)
//					strcpy(mainstubinfo.names.c_body,sc.idx(i+1).c_str());
				strcpy(botinfos[0].c_body,sc.idx(i+1).c_str());
				strcpy(botinfos[1].c_body,sc.idx(i+1).c_str());
				strcpy(botinfos[2].c_body,sc.idx(i+1).c_str());
				strcpy(botinfos[3].c_body,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altrims")) {
//				if (false)
//				if (wininfo.isalreadyrunning)
//					strcpy(mainstubinfo.names.c_body,sc.idx(i+1).c_str());
				botinfos[0].c_rims=atoi(sc.idx(i+1).c_str());
				botinfos[1].c_rims=atoi(sc.idx(i+1).c_str());
				botinfos[2].c_rims=atoi(sc.idx(i+1).c_str());
				botinfos[3].c_rims=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"gameid")) {
				ol_opt.gameid=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"gametype")) {
				string& s=sc.idx(i+1);
				C8 gc=s[0];
				ol_opt.gt=gc;
			} else if (!strcmp(sc.idx(i).c_str(),"playerid")) {
				ol_opt.playerid=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcardecal")) {
				if (false)
//				if (wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_decal,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcarpaint")) {
				if (false)
//				if (wininfo.isalreadyrunning)
					strcpy(mainstubinfo.names.cd.c_paint,sc.idx(i+1).c_str());

			} else if (!strcmp(sc.idx(i).c_str(),"altcarname2")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarbody2")) {
				strcpy(botinfos[1].c_body,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altrims2")) {
				botinfos[1].c_rims=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcardecal2")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarpaint2")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarname3")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarbody3")) {
				strcpy(botinfos[2].c_body,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altrims3")) {
				botinfos[2].c_rims=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcardecal3")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarpaint3")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarname4")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarbody4")) {
				strcpy(botinfos[3].c_body,sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altrims4")) {
				botinfos[3].c_rims=atoi(sc.idx(i+1).c_str());
			} else if (!strcmp(sc.idx(i).c_str(),"altcardecal4")) {
			} else if (!strcmp(sc.idx(i).c_str(),"altcarpaint4")) {
			} else {
				for (k=0;k<MAXOTHERS;++k) {
					for (j=0;j<NUMHSVS;++j) {
						C8 str[50];
						sprintf(str,"hue%d_%d",k,j);
						if (!strcmp(sc.idx(i).c_str(),str)) {
//							logger("found hue%d_%d, %d\n",k,j,atoi(sc.idx(i+1).c_str()));
							if (k==0)
								mainstubinfo.names.cd.c_hsvs[j].h=atoi(sc.idx(i+1).c_str());
							else
								botinfos[k-1].c_hsvs[j].h=atoi(sc.idx(i+1).c_str());
							break;
						}
						sprintf(str,"sat%d_%d",k,j);
						if (!strcmp(sc.idx(i).c_str(),str)) {
//							logger("found sat%d_%d, %d\n",k,j,atoi(sc.idx(i+1).c_str()));
							if (k==0)
								mainstubinfo.names.cd.c_hsvs[j].s=atoi(sc.idx(i+1).c_str());
							else
								botinfos[k-1].c_hsvs[j].s=atoi(sc.idx(i+1).c_str());
							break;
						}
						sprintf(str,"val%d_%d",k,j);
						if (!strcmp(sc.idx(i).c_str(),str)) {
//							logger("found val%d_%d, %d\n",k,j,atoi(sc.idx(i+1).c_str()));
							if (k==0)
								mainstubinfo.names.cd.c_hsvs[j].v=atoi(sc.idx(i+1).c_str());
							else
								botinfos[k-1].c_hsvs[j].v=atoi(sc.idx(i+1).c_str());
							break;
						}
					}
					if (j!=NUMHSVS)
						break;
				}
				if (k==MAXOTHERS)
					errorexit("unknown onlinetracksel cfg command '%s'",sc.idx(i).c_str());
			}
		}
	}
//	if (onlinetracknum<=0) {
//		onlinetracknum=1;
//		ol_opt.nplayers=1;	// track not found, use first track and force botmode..
//	}
	if (ol_opt.nt!=NET_BCLIENT)
		ol_opt.nbots=0;

	patch_globalgame();
/*	mystrncpy(ol_opt.trackname,thetracks->idx((onlinetracknum-1)*SELTRACKWID+1).c_str(),NAMESIZEI);
	ol_opt.trackscale=(float)atof(thetracks->idx((onlinetracknum-1)*SELTRACKWID+2).c_str());
	ol_opt.trackstart=(float)atof(thetracks->idx((onlinetracknum-1)*SELTRACKWID+3).c_str());
	ol_opt.trackend=(float)atof(thetracks->idx((onlinetracknum-1)*SELTRACKWID+4).c_str());
	mystrncpy(ol_opt.skyboxname,thetracks->idx((onlinetracknum-1)*SELTRACKWID+5).c_str(),NAMESIZEI);
	if (!ol_opt.skyboxname[0])
		strcpy(ol_opt.skyboxname,"-"); */
popdir();
//	delete thetracks;
	delete thecars;
}

/////////// trackinfo class ///////////// (used for handling 'online_seltrack.txt'
static S32 thetracktextwidth() // this gets called before main
{
	return 7;
}

S32 trackinfo::SELTRACKWID=thetracktextwidth();
trackinfo::trackinfo()
{
	pushandsetdir("racenetdata/onlineopt");
	ts=new script("online_seltrack.txt");
	if (ts->num()==0 || ts->num()%SELTRACKWID)
		errorexit("bad online_seltrack.txt");
	popdir();
	ntracks=ts->num()/SELTRACKWID;
}

S32 trackinfo::getnumtracks()
{
	return ntracks;
}

// -1 if err not found
S32 trackinfo::findtrackidx(string name)
{
	S32 i;
	for (i=0;i<ntracks;++i)
		if (!my_stricmp(gettrackname(i).c_str(),name.c_str()))
			return i;
	return -1;
}

string trackinfo::gettrackname(S32 idx)
{
	C8 name[500];
	mgetname(ts->idx(SELTRACKWID*idx+1).c_str(),name);
	return name;
}

string trackinfo::gettracknamebws(S32 idx)
{
	C8 nameext[500];
	mgetnameext(ts->idx(SELTRACKWID*idx+1).c_str(),nameext);
	return nameext;
}

string trackinfo::gettrackdir(S32 idx)
{
	C8 dirname[500];
	mgetpath(ts->idx(SELTRACKWID*idx+1).c_str(),dirname);
	return string("racenetdata/") + dirname;
}

string trackinfo::gettrackpicname(S32 idx)
{
	C8 name[500];
	mgetname(ts->idx(SELTRACKWID*idx+1).c_str(),name);
	return string(name)+".jpg";
}

string trackinfo::gettracktopname(S32 idx)
{
	C8 name[500];
	mgetname(ts->idx(SELTRACKWID*idx+1).c_str(),name);
	return string(name)+"_top.png";
}

string trackinfo::gettrackcfgname(S32 idx)
{
	C8 name[500];
	mgetname(ts->idx(SELTRACKWID*idx+1).c_str(),name);
	return string(name)+".cfg";
}

float trackinfo::gettrackscale(S32 idx)
{
	return (float)atof(ts->idx(SELTRACKWID*idx+2).c_str());
}

float trackinfo::gettrackstart(S32 idx)
{
	return (float)atof(ts->idx(SELTRACKWID*idx+3).c_str());
}

float trackinfo::gettrackend(S32 idx)
{
	return (float)atof(ts->idx(SELTRACKWID*idx+4).c_str());
}

string trackinfo::gettrackskybox(S32 idx)
{
	return ts->idx(SELTRACKWID*idx+5);
}

S32 trackinfo::gettrackdefaultrules(S32 idx)
{
	return atoi(ts->idx(SELTRACKWID*idx+6).c_str());
}

trackinfo::~trackinfo()
{
	delete ts;
}
