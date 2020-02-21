/*
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <engine7cpp.h>
#include <misclib7cpp.h>

#include "mainmenucpp.h"
#include "usefulcpp.h"
#include "carenaconnectcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "carenagamecpp.h"
#include "packetcpp.h"
#include "trackhashcpp.h"
#include "tracklistcpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_carclass.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"

void carenastatusinit(),carenastatusproc(),carenastatusexit();
//static struct state carenagamestate={carenagameinit,carenagameproc,carenagameexit};
//static struct state carenastatusstate={carenastatusinit,carenastatusproc,carenastatusexit};
struct gamecfgn gcfg; // export for debvars
struct gamepick gp;
static con32 *con;
static int framestart;
#define SERVERWAITHI 4
#define CLIENTWAITHIACK 5
#define STATUSTIME 2
static struct trackinfo welcome;
#define MAXTOKBUFF 100000
static char *tokbuff;

static void randomavatar(struct avatar *a)
{
//	static int botidx;
//	char **sc;
//	int nsc;
//	char **scbotname=NULL;
//	int nscbotname=0;
	script scbotname;
//	char **sccarnames=NULL;
//	int nsccarnames=0;
	script* sccarnames;
	char nobotweap[MAX_WEAPKIND];
	int decallo=0,decalhi=0,paintlo=0,painthi=0;
	int i,j,k;
	memset(a,0,sizeof(*a));
	memset(nobotweap,0,sizeof(nobotweap));
// get all the avatars types
pushandsetdirdown("carenalobby");
//	sc=loadscript("gamedomain.txt",&nsc);
	script* sc=new script("gamedomain.txt"); 
popdir();
	S32 nsc=sc->num();
	if (nsc&1)
		errorexit("randomavatar: bad gamedomain.txt");
	for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"track")) { // skip
		} else if (!strcmp(sc->idx(i).c_str(),"serverport")) { // skip
		} else if (!strcmp(sc->idx(i).c_str(),"paintlo"))
			paintlo=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"painthi"))
			painthi=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"decallo"))
			decallo=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"decalhi"))
			decalhi=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"botnoweap")) {
			for (j=0;j<MAX_WEAPKIND;j++)
				if (!strcmp(sc->idx(i+1).c_str(),weapinfos[j].name))
					break;
			if (j==MAX_WEAPKIND)
				errorexit("no such weapon '%s'",sc->idx(i+1).c_str());
			nobotweap[j]=1;
		} else if (!strcmp(sc->idx(i).c_str(),"botname"))
//			scbotname=addscript(scbotname,strclone(sc.idx(i+1).c_str()),&nscbotname);
			scbotname.addscript(sc->idx(i+1).c_str());
		else 
			errorexit("randomavatar: bad token in gamedomain.txt '%s'",sc->idx(i).c_str());
	}
//	freescript(sc,nsc);
	delete sc;
	sc=0;
// get all possible avatar cars
// pick a car at random
/*
pushandsetdirdown("cardata");
//	sc=doadir(&nsc,0);
	sc=new scriptdir(false);
	nsc=sc->num();
	sc->sort();
popdir();
	for (i=0;i<nsc;i++) {
		char name[50];
//		if (fileext(sc.idx(i).c_str(),".jrm")) {
		if (isfileext(sc->idx(i).c_str(),"jrm")) {
//			getname(name,sc.idx(i).c_str());
			mgetname(sc->idx(i).c_str(),name);
//			sccarnames=addscript(sccarnames,strclone(name),&nsccarnames);
			sccarnames.addscript(name);
		}
	}
	S32 nsccarnames=sccarnames.num();
//	freescript(sc,nsc);
	delete sc;
*/
	sccarnames=n_carclass::getcarlist();
	S32 nsccarnames=sccarnames->num();
//	freescript(sc,nsc);
//	delete sc;
	k=mt_random(scbotname.num());
	mystrncpy(a->playername,scbotname.idx(k).c_str(),NAMESIZEI);
//	strcpy(a->ci.carname,sccarnames[mt_random(nsccarnames)]);
	strcpy(a->ci.carname,sccarnames->idx(mt_random(nsccarnames)).c_str());
	a->ci.paint=mt_random(painthi-paintlo+1)+paintlo;
	a->ci.decal=mt_random(decalhi-decallo+1)+decallo;
// pick weapons at random, all different and not in nobotweap list
	for (j=0;j<MAX_WEAPCARRY;j++) {
		while(1) {
			int m;
			k=mt_random(MAX_WEAPKIND);
			if (nobotweap[k])
				continue;
			for (m=0;m<j;m++)
				if (a->ci.weapons[m]==k)
					break;
			if (m==j) {
				a->ci.weapons[j]=k;
				break;
			}
		}
	}
	a->ci.nweapons=j;
// energies at random
	for (j=0;j<MAX_ENERGIES;j++)
		a->ci.energies[j]=mt_random(10)+11;
//	freescript(sccarnames,nsccarnames);
//	freescript(scbotname,nscbotname);
// no owner yet
	a->ownerid=-1;
	delete sccarnames;
}

static void setupbotgame()
{
	int i;
	gg.yourid=0; // your computer id
	gg.viewslot=0;
	gp.gt.gi.botplayers+=gp.gt.gi.humplayers-1;
	gp.gt.gi.humplayers=1;
	gos[0].av=gp.gpav;
	for (i=gp.gt.gi.humplayers;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
		randomavatar(&gos[i].av);
	}
}

// load gamepicked.txt into gp
static void loadgp()
{
//	char **sc;
//	int nsc;
	int i,j;
pushandsetdirdown("carenalobby");
//	slecstdisable();
//	sc=loadscript("gamepicked.txt",&nsc);
	script sc("gamepicked.txt");
popdir();
	S32 nsc=sc.num();
	if (nsc&1)
		errorexit("bad gamepicked.txt");
	memset(&gp,0,sizeof(gp));
	for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc.idx(i).c_str(),"playername"))
			mystrncpy(gp.gpav.playername,sc.idx(i+1).c_str(),NAMESIZEI);
		else if (!strcmp(sc.idx(i).c_str(),"rules"))
			gp.gt.gi.rules=atoi(sc.idx(i+1).c_str());
		else if (!strcmp(sc.idx(i).c_str(),"gamenet"))
			gp.gamenet=atoi(sc.idx(i+1).c_str());
		else if (!strcmp(sc.idx(i).c_str(),"serverip"))
			gp.serverip=str2ip(sc.idx(i+1).c_str());
		else if (!strcmp(sc.idx(i).c_str(),"serverport"))
			gp.serverport=atoi(sc.idx(i+1).c_str());
		else if (!strcmp(sc.idx(i).c_str(),"track"))
			mystrncpy(gp.gt.ti.trackname,sc.idx(i+1).c_str(),NAMESIZEI);
		else if (!strcmp(sc.idx(i).c_str(),"weap")) {
			for (j=0;j<MAX_WEAPKIND;j++)
				if (!strcmp(weapinfos[j].name,sc.idx(i+1).c_str()))
					break;
			if (j!=MAX_WEAPKIND && gp.gpav.ci.nweapons<MAX_WEAPCARRY)
				gp.gpav.ci.weapons[(U8)gp.gpav.ci.nweapons++]=j;
		} else if (!strcmp(sc.idx(i).c_str(),"body"))
			mystrncpy(gp.gpav.ci.carname,sc.idx(i+1).c_str(),NAMESIZEI);
		else if (!strcmp(sc.idx(i).c_str(),"paint")) {
			gp.gpav.ci.paint=atoi(sc.idx(i+1).c_str()+5);
		} else if (!strcmp(sc.idx(i).c_str(),"decal")) {
			gp.gpav.ci.decal=atoi(sc.idx(i+1).c_str()+5);
		} else if (!strcmp(sc.idx(i).c_str(),"humanplayers"))
			gp.gt.gi.humplayers=atoi(sc.idx(i+1).c_str());
		else if (!strcmp(sc.idx(i).c_str(),"botplayers"))
			gp.gt.gi.botplayers=atoi(sc.idx(i+1).c_str());
		else {
			for (j=0;j<MAX_ENERGIES;j++) // skip past energies
				if (!strcmp(sc.idx(i).c_str(),energy_names[j]))
					break;
			if (j==MAX_ENERGIES)
				errorexit("unknown gamepicked.txt token '%s'",sc.idx(i).c_str());
			gp.gpav.ci.energies[j]=atoi(sc.idx(i+1).c_str());
		}
	}
//	freescript(sc,nsc);
	gp.gpav.ownerid=0;
//	gp.slotnum=0;
//	gp.gpav.used=1;
//	gp.gpav.cntl=CNTL_HUMAN;
pushandsetdirdown("constructed");
//	char str[100];
//	sprintf(str,"%s.trk",gp.gt.trackname);
	memset(&welcome,0,sizeof(welcome));
	if (loadtrackscript(gp.gt.ti.trackname,&gp.gt.gametrack.trkdata)) {
//		loadtrackscript(
//		gp.gt.ti.trackname[0]='\0';
		gettrackhash((unsigned char *)&gp.gt.gametrack.trkdata,sizeof(gp.gt.gametrack.trkdata),
			gp.gt.ti.trackhash);
		gp.gt.ti.validtrackhash=TRACKTYPE_OLD;
	} else if (gp.gt.newgametrack=loadnewtrackscript(gp.gt.ti.trackname)) {
		int len;
		unsigned char *data;
//		loadtrackscript(
//		gp.gt.ti.trackname[0]='\0';
		data=writenewtrack(gp.gt.newgametrack,(U32*)&len);
		gettrackhash(data,len,gp.gt.ti.trackhash);
//		memfree(data);
		delete[] data;
		gp.gt.ti.validtrackhash=TRACKTYPE_NEW;
	}
popdir();
}

void cleansocks()
{
	int i;
	for (i=0;i<gp.gt.gi.humplayers;i++)
		if (gg.sock[i]) {
			freesocker(gg.sock[i]);
			gg.sock[i]=0;
		}
/*	if (gg.sockerinited) { // shut down socker if no sockets
		gg.sockerinited=0;
		uninitsocker();
	} */
}

static void showpacket(char *comment,unsigned char *data,int datasize)
{
	logger("'%s': size %d, ",comment,datasize);
	while(datasize--)
		logger("%02x ",*data++);
	logger("\n");
}

///////////////////////////////////////////////////////////////////////// establish connections, setup multiplayer array
void carenaconnectinit()
{
pushandsetdir("engine7testdata");
	int i;
	logger("---------------------- carenagameinit -------------------------------\n");
//	randomize();
//	mt_randomize();
	mt_setseed(getmicrosec());
//	video_setupwindow(globalxres,globalyres,DESIREDBPP);
	video_setupwindow(GX,GY);
//	con=con16_alloc(WX,WY,hiblack,hiwhite);
	con=con32_alloc(WX,WY,C32BLACK,C32WHITE);
	con32_printf(con,"CARENACONNECTINIT:\n");
	loadenergymatrix();
	loadgp(); // load last settings into gp
	if (gp.gt.gi.humplayers==1 && gp.gamenet==GAMENET_SERVER) // 1 player server game turned into a 1 player bot game
		gp.gamenet=GAMENET_BOTGAME;
	if (wininfo.isalreadyrunning) { // if game already running on same computer then demote to a client with random avatar
//		randomavatar(&gp.gpav);
//		if (gp.gamenet==GAMENET_SERVER)
//			gp.gamenet=GAMENET_CLIENT;
	}
	con32_printf(con,"LOADGP: humplayers %d, gametype '%s'\ntrackname '%s', rules '%s'\nplayername '%s', carbody '%s'\n",
		gp.gt.gi.humplayers,which_game_names[gp.gamenet],gp.gt.ti.trackname,rule_names[gp.gt.gi.rules],gp.gpav.playername,gp.gpav.ci.carname);
	con32_printf(con,"validtrackhash %d, hash ",gp.gt.ti.validtrackhash);
	for (i=0;i<NUMHASHBYTES;i++)
		con32_printf(con,"%02X:",gp.gt.ti.trackhash[i]);
	con32_printf(con,"\n");
	memset(&gg,0,sizeof(gg));
	gg.yourcntl=CNTL_HUMAN;
	if (gp.gamenet==GAMENET_SERVER || gp.gamenet==GAMENET_CLIENT) { // setup sockets if a net game
		initsocker();
		con32_printf(con,"SOCKERINIT:\n");
//		gg.sockerinited=1;
	}
//	framestart=getmillisec();
	framestart=getmillisec();
	gg.timeout=gcfg.beforeloadtimeoutclient*1000+framestart;
	con32_printf(con,"TIMEOUT %d:\n",gg.timeout-framestart);
	memset(gos,0,sizeof(gos)); // and no memset on gcfg
//	tokbuff=(char *)memalloc(MAXTOKBUFF);
	tokbuff=new C8[MAXTOKBUFF];
	inittracklist();
}

// 7 different senarios
// 1) x botgame											play a bot game
// 2) x client normal									play a client game
// 3) x client timeout									play a bot game
// 4) x server normal									play a server game
// 5) x server timeout, some players connected			play a server game with some extra bots
// 6) x server timeout, no players connected			play a bot game
// 7) x escape or GAME_NONE								clean up and return to prev state

void carenaconnectproc()
{
	int i;
	static int allwritten;
//	int frame=getmillisec();
	S32 frame=getmillisec();
// just go back if no game
	if (gp.gamenet==GAMENET_BAILED) { // senario 7
		popstate();
		return;
	}
// just play if bot game
	if (gp.gamenet==GAMENET_BOTGAME) { // senario 1
		gg.goingtogame=1;
		setupbotgame();
		changestate(STATE_NEWCARENASTATUS);
		return;
	}
// wait for initsock reply
	if (!gg.myip) {
		if (getnmyip()) {
			gg.yourid=-1;
			gg.myip=getmyip(0);	
			con32_printf(con,"myip %s, frm %d\n",ip2str(gg.myip),frame-framestart);
		}
	}
// i have an ip, try some connections
	if (gg.myip) { // got an ip, try to make some connections etc..
		for (i=0;i<gp.gt.gi.humplayers;i++)
			tcpfillreadbuff(gg.sock[i]); // READBUFF
// init server
		if (gp.gamenet==GAMENET_SERVER)
			if (!gg.master && !gg.sock[0]) {
					gg.master=tcpmakemaster(0,gp.serverport); // MAKEMASTER, become a server
					if (gg.master) {
						gg.timeout=gcfg.beforeloadtimeoutserver*1000+frame;
						gg.numcurplayers=1;
						gos[0].av=gp.gpav;
						con32_printf(con,"server created, frm %d\n",frame-framestart);
						for (i=gp.gt.gi.humplayers;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
							con32_printf(con,"server: botplayer %d\n",i);
							randomavatar(&gos[i].av);
//							gos[i].av.cntl=CNTL_AI;
//							gos[i].av.slotnum=i;
							gos[i].av.ownerid=-1; // noone can change
//							gg.numcurplayers++; // got a new player
							con32_printf(con,"slot %2d, owner %d, name '%s', car '%s'\n",
								i,gos[i].av.ownerid,gos[i].av.playername,gos[i].av.ci.carname);
//							gg.retrytimers[i]=-1; // freeze timer
						}
						welcome=gp.gt.ti; // setup welcome packet
					} else {
						randomavatar(&gp.gpav);
						if (gp.gamenet==GAMENET_SERVER)
							gp.gamenet=GAMENET_CLIENT;
					}
			}
// init client
		if (!gg.master && !gg.sock[0]) { // GETCONNECT, become a client
			gg.numcurplayers=0; // wait for nplayers go packets.. 
			gg.sock[0]=tcpgetconnect(gp.serverip,gp.serverport);
			if (gg.sock[0]) { // and set a HI packet
//				tcpwritetoken(gg.sock[0],PACK_HI,(char *)&gp.gpav,sizeof(struct avatar));
//				con32_printf(con,"client: connect to server and sent hi packet at %d\n",frame-framestart);
				gg.retrytimers[0]=frame+1000*CLIENTWAITHIACK;
			}
		}
		if (gg.master) {
//// SERVER SIDE
			struct socker *cl;
			gg.yourid=0;
			cl=tcpgetaccept(gg.master); // GETACCEPT, login
			if (cl) { // look at number of current player connections..
				con32_printf(con,"server: at %d, got a connect\n",frame-framestart);
				for (i=1;i<gp.gt.gi.humplayers;i++)
					if (!gg.sock[i] && gg.retrytimers[i]!=-1) // just connect with anybody (firewall)
						break;
				if (i!=gp.gt.gi.humplayers) {
					gg.sock[i]=cl;
					con32_printf(con,"server: at %d, connect %d\n",frame-framestart,i);
					gg.retrytimers[i]=frame+SERVERWAITHI*1000;
					tcpwritetoken32(gg.sock[i],PACK_WELCOME,(char *)&welcome,sizeof(welcome));
				} else {
					con32_printf(con,"server: at %d, connect too many\n",frame-framestart);
					freesocker(cl); // CLOSE
				}
			}
			for (i=1;i<gp.gt.gi.humplayers;i++) { // close down bad connections
				if (gg.sock[i] && (gg.sock[i]->flags&SF_CLOSED)) {
					freesocker(gg.sock[i]); // CLOSE closed
					gg.sock[i]=NULL;
					gg.retrytimers[i]=0;
					gg.numcurplayers--;
//					logger("server: player %d left, numcurplayers %d\n",i,gg.numcurplayers);
					con32_printf(con,"server: at %d, player %d left\n",frame-framestart,i);
				}
			} 
			for (i=1;i<gp.gt.gi.humplayers;i++) {
				if (gg.sock[i]) {
					int n;
					int id;
					n=tcpreadtoken32(gg.sock[i],&id,tokbuff,MAXTOKBUFF); // READ something
					if (n>0) {
						if (id==PACK_HI) {
							struct histruct *hs;
							hs=(struct histruct *)tokbuff;
							if (hs->gottrackhash) { // client already has that track
//								struct gametype sgt;
//								struct avatar *sav=(struct avatar *)tokbuff;
								con32_printf(con,"server: rec hi gottrackhash 1, at %d, from %d\n",frame-framestart,i);
								logger("server: got a 'hi' packet from %d\n",i);
								gos[i].av=hs->av;
	//							gos[i].av.cntl=CNTL_HUMAN;
	//							gos[i].av.slotnum=i;
								gos[i].av.ownerid=i;
								gg.numcurplayers++; // got a new player
//								sgt=gp.gt;
	//							sgt.yourid=i;
								con32_printf(con,"slot %2d: owner %d, name '%s', car '%s'\n",
									i,gos[i].av.ownerid,gos[i].av.playername,gos[i].av.ci.carname);
								con32_printf(con,"server: sending hiack packet\n");
								tcpwritetoken32(gg.sock[i],PACK_HIACK,(char *)&gp.gt.gi,sizeof(gp.gt.gi)); // send game pick and id
							} else {
								if (welcome.validtrackhash==TRACKTYPE_OLD) {
									struct gameinfoc gic;
									gic.gametrk=gp.gt.gametrack.trkdata;
									gic.gi=gp.gt.gi;
	//								struct gametype sgt;
//									struct avatar *sav=(struct avatar *)tokbuff;
									con32_printf(con,"server: rec hi gottrackhash 0, at %d, from %d\n",frame-framestart,i);
									logger("server: got a 'hi' packet from %d\n",i);
									gos[i].av=hs->av;
		//							gos[i].av.cntl=CNTL_HUMAN;
		//							gos[i].av.slotnum=i;
									gos[i].av.ownerid=i;
									gg.numcurplayers++; // got a new player
	//								sgt=gp.gt;
		//							sgt.yourid=i;
									con32_printf(con,"slot %2d: owner %d, name '%s', car '%s'\n",
										i,gos[i].av.ownerid,gos[i].av.playername,gos[i].av.ci.carname);
									con32_printf(con,"server: sending hiackc packet\n");
									tcpwritetoken32(gg.sock[i],PACK_HIACKC,(char *)&gic,sizeof(gic)); // send game pick and id and track data
								} else { // welcome.validtrackhash==TRACKTYPE_OLD // new track
									int xfersize;
//									struct avatar *sav=(struct avatar *)tokbuff;
									unsigned char *data;
									con32_printf(con,"server: rec hi 'new' gottrackhash 0, at %d, from %d\n",frame-framestart,i);
									logger("server: got a 'hi' packet from %d\n",i);
									gos[i].av=hs->av;
		//							gos[i].av.cntl=CNTL_HUMAN;
		//							gos[i].av.slotnum=i;
									gos[i].av.ownerid=i;
									gg.numcurplayers++; // got a new player
									struct newgameinfoc *ngic=(struct newgameinfoc *)tokbuff;
									data=writenewtrack(gp.gt.newgametrack,&ngic->rawtrklen);
									if (ngic->rawtrklen+sizeof(newgameinfoc)>=MAXTOKBUFF)
										errorexit("buffer too big for new track");
									ngic->gi=gp.gt.gi;
									memcpy(ngic->rawtrk,data,ngic->rawtrklen);
//									memfree(data);
									delete[] data;
	//								struct gametype sgt;
	//								sgt=gp.gt;
		//							sgt.yourid=i;
									con32_printf(con,"'new' slot %2d: owner %d, name '%s', car '%s'\n",
										i,gos[i].av.ownerid,gos[i].av.playername,gos[i].av.ci.carname);
									con32_printf(con,"server: sending hiacknc packet\n");
									logger("sizeof newgameinfoc is %d\n",sizeof(newgameinfoc));
									xfersize=sizeof(newgameinfoc)-sizeof(char *)+ngic->rawtrklen;
									tcpwritetoken32(gg.sock[i],PACK_HIACKNC,tokbuff,xfersize); // send game pick and id and track data
									showpacket("write hiacknc",(unsigned char *)tokbuff,xfersize);
								}
							}
/*							if (gp.gt.ti.trackname[0]) {
							} else {
//								struct gametypec sgtc;
								struct avatar *sav=(struct avatar *)tokbuff;
								con32_printf(con,"server: rec hi at %d, from %d\n",frame-framestart,i);
								logger("server: got a 'hi' packet from %d\n",i);
								gos[i].av=*sav;
	//							gos[i].av.cntl=CNTL_HUMAN;
	//							gos[i].av.slotnum=i;
								gos[i].av.ownerid=i;
								gg.numcurplayers++; // got a new player
//	NYI							gp.gtc.botplayers=gp.gt.gi.botplayers;
//								gp.gtc.humplayers=gp.gt.gi.humplayers;
								gp.gtc.rules=gp.gt.rules;
//								sgtc=gp.gtc;
	//							sgt.yourid=i;
								con32_printf(con,"slot %2d: owner %d, name '%s', car '%s'\n",
									i,gos[i].av.ownerid,gos[i].av.playername,gos[i].av.ci.carname);
								con32_printf(con,"server: sending hiackc packet\n");
								tcpwritetoken(gg.sock[i],PACK_HIACKC,(char *)&gp.gtc,sizeof(gp.gtc)); // send game pick and id
							} */
							gg.retrytimers[i]=-1; // freeze timer
						}
					} else if (gg.retrytimers[i]!=-1 && frame>=gg.retrytimers[i]) { // wait around for client to say 'hi'
						gg.retrytimers[i]=0;
						freesocker(gg.sock[i]); // CLOSE client timeout no 'hi'
						gg.sock[i]=NULL;
						gg.numcurplayers--;
						con32_printf(con,"server: timeout at %d, from %d\n",frame-framestart,i);
					}
				}
			}
// CLIENT SIDE
		} else {
			if (gg.sock[0]) {
				int id;
				int n=tcpreadtoken32(gg.sock[0],&id,tokbuff,MAXTOKBUFF); // READ something, get reply from server
				if (n>0) {
					if (id==PACK_WELCOME) {
						struct histruct hs;
						gp.gt.ti.validtrackhash=TRACKTYPE_NONE;
						struct trackinfo *wcp=(struct trackinfo *)tokbuff;
						welcome=*wcp;
						if (welcome.validtrackhash) {
							const char *trkhashname;
							trkhashname=hastrackhash(welcome.trackhash);
							if (trkhashname) {
								hs.gottrackhash=welcome.validtrackhash;
								gp.gt.ti.validtrackhash=welcome.validtrackhash;
								con32_printf(con,"client: found track '%s'\n",trkhashname);
								strcpy(welcome.trackname,trkhashname);
								if (gp.gt.newgametrack)
									freenewtrackscript(gp.gt.newgametrack);
								gp.gt.newgametrack=0;
								if (welcome.validtrackhash==TRACKTYPE_OLD)
									loadtrackscript(welcome.trackname,&gp.gt.gametrack.trkdata);
								else
									gp.gt.newgametrack=loadnewtrackscript(welcome.trackname);
							} else
								hs.gottrackhash=0;
						} else
							hs.gottrackhash=-1; // assume client has same prebuilt tracks as server (for now)
						hs.av=gp.gpav;
						con32_printf(con,"client: got a welcome at %d, valid %d\n",frame-framestart,welcome.validtrackhash);
//						gp.gpav.gottrackhash=1;
						tcpwritetoken32(gg.sock[0],PACK_HI,(char *)&hs,sizeof(hs));
						con32_printf(con,"client: connect to server and sent hi packet at %d\n",frame-framestart);
						con32_printf(con,"gottrackhash %d\n",hs.gottrackhash);
					} else if (id==PACK_HIACK) { // READ 'polnum' nplayers pid trackname trackscale etc
						struct gameinfo *gi=(struct gameinfo *)tokbuff;
						gp.gt.gi=*gi;
						gp.gt.ti=welcome; // client now commited to server's track
//						gp.gt=*cgt; // overwrite gt with server's
						gg.retrytimers[0]=-1;
//						gg.yourid=cgt->yourid;	// freeze timer, and get id
//						gg.viewslot=cgt->yourid;
//						gg.yourslot=cgt->yourid;
//						gp.gt.gi.botplayers=0; // let server send me go packets with CNTL_AI in them
						con32_printf(con,"client: got a hiack at %d, myid %d\n",frame-framestart,gg.yourid);
						con32_printf(con,"humplayers %d, botplayers %d, rules '%s'\n",
							gp.gt.gi.humplayers,gp.gt.gi.botplayers,rule_names[gp.gt.gi.rules]);
						con32_printf(con,"slot %d, trackname '%s'\n",
							gg.yourid,gp.gt.ti.trackname);
					} else if (id==PACK_HIACKC) { // READ 'polnum' nplayers pid trackname trackscale etc
						const char *u;
						struct gameinfoc *gic=(struct gameinfoc *)tokbuff;
						gp.gt.ti=welcome; // client now commited to server's track
						gp.gt.gi=gic->gi;
//						gp.gt=*cgt; // overwrite gt with server's
//						gp.gt.gi.botplayers=cgtc->botplayers;
//						gp.gt.gi.humplayers=cgtc->humplayers;
//						gp.gt.rules=cgtc->rules;
						gp.gt.gametrack.trkdata=gic->gametrk;
						gg.retrytimers[0]=-1;
//						gg.yourid=cgt->yourid;	// freeze timer, and get id
//						gg.viewslot=cgt->yourid;
//						gg.yourslot=cgt->yourid;
//						gp.gt.gi.botplayers=0; // let server send me go packets with CNTL_AI in them
						u=saveconstrack(welcome.trackname,&gp.gt.gametrack.trkdata,1);
						strcpy(gp.gt.ti.trackname,u);
						con32_printf(con,"client: got a hiackc at %d, myid %d\n",frame-framestart,gg.yourid);
						con32_printf(con,"humplayers %d, botplayers %d, rules '%s'\n",
							gp.gt.gi.humplayers,gp.gt.gi.botplayers,rule_names[gp.gt.gi.rules]);
						con32_printf(con,"slot %d, trackname '%s'\n",
							gg.yourid,gp.gt.ti.trackname);
					} else if (id==PACK_HIACKNC) { // READ 'polnum' nplayers pid trackname trackscale etc
						const char *u;
						struct newgameinfoc *ngic=(struct newgameinfoc *)tokbuff;
						showpacket("read hiacknc",(unsigned char *)tokbuff,n);
						gp.gt.ti=welcome; // client now commited to server's track
						gp.gt.gi=ngic->gi;
//						gp.gt=*cgt; // overwrite gt with server's
//						gp.gt.gi.botplayers=cgtc->botplayers;
//						gp.gt.gi.humplayers=cgtc->humplayers;
//						gp.gt.rules=cgtc->rules;
						if (gp.gt.newgametrack)
							freenewtrackscript(gp.gt.newgametrack);
						gp.gt.newgametrack=0;
						gp.gt.newgametrack=readnewtrack(ngic->rawtrk,ngic->rawtrklen);
						gg.retrytimers[0]=-1;
//						gg.yourid=cgt->yourid;	// freeze timer, and get id
//						gg.viewslot=cgt->yourid;
//						gg.yourslot=cgt->yourid;
//						gp.gt.gi.botplayers=0; // let server send me go packets with CNTL_AI in them
						u=savenewconstrack(welcome.trackname,gp.gt.newgametrack,1);
						strcpy(gp.gt.ti.trackname,u);
						con32_printf(con,"client: got a hiacknc at %d, myid %d\n",frame-framestart,gg.yourid);
						con32_printf(con,"humplayers %d, botplayers %d, rules '%s'\n",
							gp.gt.gi.humplayers,gp.gt.gi.botplayers,rule_names[gp.gt.gi.rules]);
						con32_printf(con,"slot %d, trackname '%s'\n",
							gg.yourid,gp.gt.ti.trackname);
					} else if (id==PACK_GO) { // READ 'go'
//						char *tp=tokbuff;
						gg.yourid=gg.viewslot=*((int *)tokbuff);
						int toklen=sizeof(int);
						for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
							struct avatar *cav=(struct avatar *)&tokbuff[toklen];
							gos[i].av=*cav;
							toklen+=sizeof(avatar);
						}
//						if (cav->ownerid==-1) {
//						if (cav->cntl==CNTL_AI) {
//							cav->ownerid=-1; // noone can change
//							gp.gt.gi.botplayers++;
//						}
//						gos[gg.numcurplayers].av=*cav;
						gg.numcurplayers++;
						con32_printf(con,"client: got a go at %d (%d/%d), myid %d\n",
							frame-framestart,gg.numcurplayers,gp.gt.gi.humplayers+gp.gt.gi.botplayers,gg.yourid);
//						if (gp.gt.gi.humplayers+gp.gt.gi.botplayers==gg.numcurplayers) { // senario 2
						gg.goingtogame=1;
//						justchangestate(&carenastatusstate);
						changestate(STATE_NEWCARENASTATUS);
						return;
//						} 
					}
				} else if (gg.retrytimers[0]!=-1 && frame>=gg.retrytimers[0]) {
					con32_printf(con,"client: timeout no hiack at %d\n",frame-framestart);
					gg.retrytimers[0]=0;
					freesocker(gg.sock[0]); // CLOSE, server never sent 'hiack'
					gg.sock[0]=NULL;
				}
			}
		}
		allwritten=1;
		for (i=0;i<gp.gt.gi.humplayers;i++)
			if (gg.sock[i])
				if (!tcpsendwritebuff(gg.sock[i])) // WRITEBUFF
					allwritten=0;
	}
// see if out of time
	if (frame>=gg.timeout) {
		if (gg.master) { // sever timeout
			// senario 5, senario 6   none or some not all players connected, partial/no net play
			con32_printf(con,"server: didn't get all connections at %d ... %d/%d\n",
				frame-framestart,gg.numcurplayers,gp.gt.gi.humplayers);
			gg.timeout=frame+gcfg.beforeloadtimeoutserver*wininfo.fpswanted; // reset timeout
			con32_printf(con,"server: reset timer to %d\n",gg.timeout-framestart);
			for (i=1;i<gp.gt.gi.humplayers;i++) {
				if (gg.retrytimers[i]!=-1) {
					gg.retrytimers[i]=-1;
					if (gg.sock[i]) {
						freesocker(gg.sock[i]);
						gg.sock[i]=NULL;
					}
					randomavatar(&gos[i].av);
//					gos[i].av.cntl=CNTL_AI;
					gos[i].av.ownerid=-1; // noone can change
//					gos[i].av.slotnum=i;
					con32_printf(con,"server: set slot %d to a bot\n",i);
//					gp.gt.gi.botplayers++;
					gg.numcurplayers++;
				}
			}
		} else { // client timeout
			con32_printf(con,"client: timeout at %d, setupbotgame\n",frame-framestart);
			setupbotgame(); // senario 3
			gg.goingtogame=1;
//			justchangestate(&carenastatusstate);
			changestate(STATE_NEWCARENASTATUS);
			if (gg.sock[0]) {
				freesocker(gg.sock[0]);
				gg.sock[0]=NULL;
			}
			return;
		}
	}
// see if SERVER done with all tasks
	if (gg.master && gp.gt.gi.humplayers==gg.numcurplayers) {
		if (!gg.wrotego) { // send 'go' packets to all clients
			con32_printf(con,"server: done.. sending go packets at %d\n",frame-framestart);
			gg.timeout=1000*gcfg.beforeloadtimeoutserver+frame; // wait around again just to make sure go packets sent

			char *tp=tokbuff;;
			int tokbufflen=sizeof(int); // make room for id
			for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++) {
				memcpy(&tp[tokbufflen],&gos[i].av,sizeof(avatar));
				tokbufflen+=sizeof(avatar);
			}
			for (i=1;i<gp.gt.gi.humplayers;i++) {
				if (gg.sock[i]) { // && ((od.curplayersmask>>i)&1) ) {
//					logger("go #1\n");
//					gos[i].av.ownerid*=-1;
					memcpy(tokbuff,&i,sizeof(int));
					con32_printf(con,"server: sending GO to player %d\n",i);
					tcpwritetoken32(gg.sock[i],PACK_GO,tokbuff,tokbufflen);	// WRITE 'go'
//					gos[i].av.ownerid*=-1;
				}
			}

			gg.wrotego=1; // done sending everything
			allwritten=0; // wait around for ack
		}
		if (allwritten) { // all done sending and got ack, load and play race
			con32_printf(con,"server: allwritten at %d\n",frame-framestart);
//			justchangestate(&carenastatusstate);
			changestate(STATE_NEWCARENASTATUS);
			gg.goingtogame=1;
			return;
		}
	}
	frame++;
}

void carenaconnectdraw2d()
{
//	video_lock();
//	cliprect16(B16,0,0,WX-1,WY-1,hiblue);
//	clipblit16(con16_getbitmap16(con),B16,0,0,0,0,WX,WY);
	clipblit32(con32_getbitmap32(con),B32,0,0,0,0,WX,WY);
//	video_unlock(); 
}

void carenaconnectexit()
{
	int nsocks=0,i;
	logger("---------------------- carenagameexit -------------------------------\n");
	if (gg.master) { // we don't need it anymore
		freesocker(gg.master);
		gg.master=0;
	}
	if (!gg.goingtogame) { // free up sockets on a bail
		if (con) {
			con32_free(con);
			con=0;
		}
		for (i=0;i<gp.gt.gi.humplayers;i++)
			if (gg.sock[i]) {
				freesocker(gg.sock[i]);
				gg.sock[i]=0;
			}
		if (gp.gt.newgametrack)
			freenewtrackscript(gp.gt.newgametrack);
		gp.gt.newgametrack=0;
	}
	for (i=0;i<gp.gt.gi.humplayers;i++) // count up sockets
		if (gg.sock[i])
			nsocks++;
	if (!nsocks) {
		cleansocks();
		gp.gamenet=GAMENET_BOTGAME;
	}
//	memfree(tokbuff);
	delete[] tokbuff;
	freetracklist();
popdir();
}

void carenastatusinit()
{
	int i;
	gg.goingtogame=0;
	con32_printf(con,"------- STATUS -------------------\n");
	con32_printf(con,"yourid %d, yourview %d\n",gg.yourid,gg.viewslot);
	for (i=0;i<gp.gt.gi.humplayers+gp.gt.gi.botplayers;i++)
		con32_printf(con,"slot %2d: owner %2d, name '%s', car '%s'\n",
			i,gos[i].av.ownerid,gos[i].av.playername,gos[i].av.ci.carname);
	framestart=getmillisec();
	gg.timeout=framestart+1000*gcfg.statustime;
}

void carenastatusproc()
{
	int frame=getmillisec();
	if (frame>=gg.timeout) {
//		justchangestate(&carenagamestate);
#if 1
		changestate(STATE_NEWCARENAGAME);
		gg.goingtogame=1;
#else
		popstate();
		gg.goingtogame=0;
#endif
	}
}

void carenastatusdraw2d()
{
	clipblit32(con32_getbitmap32(con),B32,0,0,0,0,WX,WY);
}

void carenastatusexit()
{
	if (!gg.goingtogame) {
		if (gp.gt.newgametrack)
			freenewtrackscript(gp.gt.newgametrack);
		gp.gt.newgametrack=0;
		cleansocks();
	}
	if (con) {
		con32_free(con);
		con=0;
	}
}

/*
struct gameglobal gg;
struct caro gos[MAX_GAMEOBJ];

void carenagameinit()
{
}
void carenagameproc()
{
	popstate();
}
void carenagameexit()
{
}
void carenagamedraw3d()
{
}
void carenagamedraw2d()
{
}
*/
