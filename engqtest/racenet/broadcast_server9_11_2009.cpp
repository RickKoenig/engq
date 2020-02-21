#define RES3D
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "broadcast_server.h"
#include "packet.h"
#include "gameinfo.h"
#include "../engine7test/n_usefulcpp.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"

//#define MAXCL 256 // max bclients
#define INACTIVETIME 720 // seconds of not hearing anything from logged on client (12 minutes)
#define LBUFFSIZE 512
#define MAXGAMES 1000

namespace broadcast_server {

int frm;

shape* rl,*focus;
pbut* QUIT;
listbox* CONSOLE;

struct socker *sv; // master socket
int nips; // got my ip yet?

//C8 pl_packet[]="1 0 2009_testcheck\\newcheck2.bws cubemap_mountains.jpg 1.000000 0.000000 0.000000 7"; // test pl packet
//S32 pl_packetlen=sizeof(pl_packet)-1;

//C8 go_packet[]="65536#jim#DuneItUp#255#0#0#255#255#255#0#0#255#paint11#decal12#8#9#10#1#0#25#17#3#27#18#16#50#55#60#65#70#75"; // test go packet
//S32 go_packetlen=sizeof(go_packet)-1;

enum playerstate {PS_WAITHI,PS_GOTHI};
enum trackstate {TS_CONNECT,TS_SENTGO,TS_INGAME};

struct carslot {
//	S32 cid; // slot number
	C8 name[100];
	C8 body[100];
	hsv hsvs[NUMHSVS];
	C8 paint[100];
	C8 decal[100];
	S32 rims;
	S32 nweap;
	S32 weaps[MAXWEAPONSCARRY];
	S32 energies[MAXENERGIES];
	U32 playerid;
// first player of unique gameid sends this data up to a trackslot
	U32 gameid; // pass up to trackslot
	C8 gametype; // NYI pass up to trackslot
	C8 fulltrackname[200];
	C8 skyboxname[200];
	S32 numcars;
};

struct client {
	struct socker *cl;
	int inactivetime;
	bool kick;
	int totread,totwrite;
	carslot cs;
	playerstate ps;
};

struct trackslot {
// net
	client bclients[OL_MAXPLAYERS];
	trackstate ts;
// filling up
	S32 numcars;
	S32 curnumcars; // number of players connected and sent a hi packet
	S32 numcl; // number of players connected
// track
	U32 tgameid;
	C8 fulltrackname[200];
	C8 skyboxname[200];
	float scaleup;
	float trackstart;
	float trackend;
	S32 rules;
};
trackslot trackslots[MAXGAMES];
S32 numgames;

U32 port; // listen port of server

///////// post //////////
U32 postip;
U32 postport;
string postcmd,postdata;
string httpstr2;
S32 httpstr2len;
S32 wtpos,rdpos;
C8 userbuff[8000];

//S32 gameid;
//S32 playerid;

socker* postcl;
struct postq
{
	string pc,pd;
};
list<postq> postqq;
enum pstst {POST_READY,POST_SEND,POST_GET,POST_REPLY};
pstst poststate;

void postinit()
{
	poststate=POST_READY;
	postcl=0;
}

void postqueue(string pca,string pda)
{
	postq p;
	p.pc=pca;
	p.pd=pda;
	postqq.push_back(p);
}

void postproc()
{
	list<postq>::iterator it;
	switch(poststate) {
// fetch post from queue and post it
	case POST_READY:
		it=postqq.begin();
		if (it!=postqq.end()) { // any in queue
			postcl=tcpgetconnect(postip,postport);
			wtpos=0;
			if (postcl) {
				httpstr2 = "POST ";
				httpstr2 += (*it).pc;
				httpstr2 += " HTTP/1.0\r\n";
				S32 conlen=((*it).pd.size());
				C8 clstr[100];
				sprintf(clstr,"Content-Length: %d\r\n\r\n",conlen);
				httpstr2 += clstr;
				httpstr2 += (*it).pd;
				httpstr2 += "\r\n\r\n";
				httpstr2len = httpstr2.length();
				logger("http post send '%s'\n",httpstr2.c_str());
				poststate=POST_SEND;
				CONSOLE->printf("POSTED %d bytes",httpstr2len);
				postqq.pop_front(); // remove from queue
			}
		}
		break;
// wait/process HTTP post
	case POST_SEND:
		if ((postcl->flags & SF_CONNECTED) && wtpos!=httpstr2len) {
			S32 len;
			len=tcpputbuffdata(postcl,httpstr2.c_str(),httpstr2len,httpstr2len);
//			len=writedata(postcl,httpstr+wtpos,httpstrlen-wtpos);
			logger("write data %d/%d\n",wtpos,httpstr2len);
			if (len || (postcl->flags & SF_CLOSED)) {
				wtpos=httpstr2len;
				rdpos=0;
				CONSOLE->printf("POST data send %d bytes",wtpos);
				poststate=POST_REPLY;
			}
		}
// wait/process HTML reply
	case POST_REPLY:
	// wait for reply
		if (tcpsendwritebuff(postcl)) { // see if all sent
			if (wtpos==httpstr2len) { // all sent
				S32 len;
				tcpfillreadbuff(postcl);
				len=tcpgetbuffdata(postcl,userbuff+rdpos,1,8000-rdpos-1);
				logger("\nread data %d, len %d\n",rdpos,len);
//				con32_printf(con,"read data %d, len %d\n",rdpos,len);
				if (len) {
/*					C8* str = new C8[len+1];
					memcpy(str,userbuff,len);
					str[len]='\0';
//					con32_printf(con,"%s\n",str);
					logger("%s",str);
					delete[] str;
//					fwrite(userbuff,len,1,fw); */
					rdpos+=len;
				}
				if (postcl && !len && (postcl->flags&SF_CLOSED)) {
// all done
//					busy=BUSY_NONE;
//					textstatus->settname("ready");
					if (postcl)
						freesocker(postcl);
					postcl=0;
					logger("\ntotal data read %d\n",rdpos);
					CONSOLE->printf("POST data read %d",rdpos);
					userbuff[rdpos]='\0';
					logger("reply '%s'\n",userbuff);
//					con32_printf(con,"\ntotal data read %d\n",rdpos);
					poststate=POST_READY;
				}
			}
		}
		break;
	default:
		break;
	}
}

void postexit()
{
	postcmd.clear();
	postdata.clear();
	postqq.clear();
	httpstr2.clear();
	if (postcl) {
		freesocker(postcl);
		postcl=0;
	}
}
///////// end post //////////

void buildatrackslot(trackslot* ts)
{
// net
	memset(ts->bclients,0,sizeof(ts->bclients));
	ts->ts=TS_CONNECT;
// filling up
	ts->numcars=2;
	ts->curnumcars=0;
	ts->numcl=0;
// track
	ts->tgameid=4000000000U; // 4 billion
	strcpy(ts->fulltrackname,"2009_testcheck\\newcheck2.bws");
	strcpy(ts->skyboxname,"cubemap_mountains.jpg");
//	strcpy(ts->fulltrackname,"prehistoric\\prehistoric.bws");
//	strcpy(ts->skyboxname,"prehistoric.jpg");
	ts->scaleup=1;
	ts->trackstart=0;
	ts->trackend=1;
	ts->rules=7;
}

void parseacarslot(carslot* cs,C8* cd) // cd (cardata) from client
//void parseacarslot(trackslot* tsp,carslot* cs,C8* cd) // cd (cardata) from client
{
	char *tok;
	int i,j,k=0;
	logger("parseacarslot playername '%s'\n",cd);
	tok=strtok(cd,"#"); // ignore id from client
/*	if (tsp->curnumcars==0) {
		tok=strtok(NULL,"#"); // numplayers
		tsp->numcars=atoi(tok);
		tok=strtok(NULL,"#"); // trackname
		mystrncpy(tsp->fulltrackname,tok,200);
		tok=strtok(NULL,"#"); // skyboxname
		mystrncpy(tsp->skyboxname,tok,200);
	} else {
		tok=strtok(NULL,"#"); // ignore numplayers
		tok=strtok(NULL,"#"); // ignore trackname
		tok=strtok(NULL,"#"); // ignore skyboxname
	} */
	tok=strtok(NULL,"#"); // numplayers
	cs->numcars=atoi(tok);
	tok=strtok(NULL,"#"); // trackname
	mystrncpy(cs->fulltrackname,tok,200);
	tok=strtok(NULL,"#"); // skyboxname
	mystrncpy(cs->skyboxname,tok,200);

	tok=strtok(NULL,"#"); // gameid
	cs->gameid=atoi(tok);
	tok=strtok(NULL,"#"); // gametype
	cs->gametype=tok[0];
	tok=strtok(NULL,"#"); // playerid
	cs->playerid=atoi(tok);
	CONSOLE->printf("new game '%s' np %d",cs->fulltrackname,cs->numcars);
	CONSOLE->printf("gid %d, gt %c, pid %d",cs->gameid,cs->gametype,cs->playerid);
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(cs->name,tok);
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(cs->body,tok);
	for (i=0;i<NUMHSVS;++i) {
		tok=strtok(NULL,"#");
		if (tok)
			cs->hsvs[i].h=atoi(tok);
		tok=strtok(NULL,"#");
		if (tok)
			cs->hsvs[i].s=atoi(tok);
		tok=strtok(NULL,"#");
		if (tok)
			cs->hsvs[i].v=atoi(tok);
	}
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(cs->paint,tok);
	tok=strtok(NULL,"#");
	if (tok)
		strcpy(cs->decal,tok);
	tok=strtok(NULL,"#");
	if (tok)
		cs->rims=atoi(tok);
	tok=strtok(NULL,"#");
	if (tok) {
		cs->nweap=atoi(tok);
		for (j=0;j<cs->nweap;j++) {
			tok=strtok(NULL,"#");
			if (tok) {
				k=atoi(tok);
				cs->weaps[j]=k;
			}
		} 
		for (i=0;i<MAXENERGIES;i++) {
			tok=strtok(NULL,"#");
			if (tok)
				cs->energies[i]=atoi(tok);
		}
	} else
		cs->nweap=0;
}

C8* make_pl_packet(trackslot* ts,S32 slotnum)
{
	static C8 pl[500];
	sprintf(pl,"%d %d %s %s %f %f %f %d",
		ts->numcars,slotnum,ts->fulltrackname,ts->skyboxname,ts->scaleup,ts->trackstart,ts->trackend,ts->rules);
	return pl;
}

C8* make_go_packet(trackslot* ts,carslot* cs,S32 from,S32 to)
{
	static C8 go[500];
#if 1
	int i,j,k,pn;
	S32 pnnew=(ts->numcars<<16)+(to<<8)+from;
	char weapstr[400],shortstr[10];
	pn=pnnew&0xff;
	sprintf(weapstr,"#%d",cs->nweap);
	for (i=0;i<cs->nweap;i++) {
		k=cs->weaps[i];
		sprintf(shortstr,"#%u",k);
		strcat(weapstr,shortstr);
	}
	for (j=0;j<MAXENERGIES;j++) {
		sprintf(shortstr,"#%u",cs->energies[j]);
		strcat(weapstr,shortstr);
	}
	sprintf(go,"%d#%s#%s#%d#%d#%d#%d#%d#%d#%d#%d#%d#%s#%s#%d%s",pnnew,
		cs->name,cs->body,
		cs->hsvs[0].h,cs->hsvs[0].s,cs->hsvs[0].v,
		cs->hsvs[1].h,cs->hsvs[1].s,cs->hsvs[1].v,
		cs->hsvs[2].h,cs->hsvs[2].s,cs->hsvs[2].v,
		cs->paint,cs->decal,cs->rims,weapstr);
	logger("buildcarinfopacket '%s'\n",go);
#endif
//	OL_MAXPLAYERS;
	return go;
}

} // end namespace broadcast_server
using namespace broadcast_server;

void broadcast_server_init()
{
	logger("server init\n");
// graphics
	video_setupwindow(GX,GY);
	init_res3d(800,600);
// ui
pushandsetdir("racenetdata/broadcast");
	script sc("broadcast_servercfg.txt");
	if (sc.num()>=5) {
		port=atoi(sc.idx(0).c_str());
		postip=str2ip(sc.idx(1).c_str());
		postport=atoi(sc.idx(2).c_str());
		postcmd=sc.idx(3);
		postdata=sc.idx(4);
	}
	rl=res_loadfile("broadcast_serverres.txt");
	QUIT=rl->find<pbut>("QUIT");
	CONSOLE=rl->find<listbox>("CONSOLE");
	CONSOLE->setmax(800);
	focus=CONSOLE;
// network
	nips=0;
	sv=NULL;
	initsocker();
	CONSOLE->printf("broadcast_server init");
	memset(trackslots,0,sizeof(trackslots));
	numgames=0;
	logger("sizeof trackslots is %d\n",sizeof(trackslots));
	postinit();
}

void broadcast_server_proc()
{
	int i,j,k;
// ui
// escape
	if (KEY==K_ESCAPE)
		popstate();
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0; // see if something happened
		if (focusa==QUIT)
			popstate();
	}
// end ui
// get ip(s)
	if (!nips) {
		nips=getnmyip();	// returns 0 until we get one
		if (nips>0) { // show ip's somewhere
			// getmyip(0);
		}
	}
// if have an ip, then enable start server button when we get a good listen socket
	if (!sv && nips) {
		sv=tcpmakemaster(0,port);
		if (sv) {
			CONSOLE->printf("listen: ip '%s', port %d",ip2str(getmyip(0)),sv->port); // remember: just 1 ip2str per printf
//			for (i=0;i<20;++i) // test postqueue
//				postqueue(postcmd,postdata);
//			gameid=1000;
//			playerid=43;
/*			{
				C8 hardcmd[50],harddata[1000];
				sprintf(hardcmd,"/race/%d",gameid);
				sprintf(harddata,"op=save&track=Indy&rule1=7&rule2=2&"
					"player1_id=%d&player1_car=synkro&player1_finished=1&player1_position=1&player1_time=12000&"
					"player1_score=100&player1_awards=1,2,3&player1_xp=23&player1_woc=laser"
					"player1_id=%d&player1_car=synkro&player1_finished=2&player1_position=1&player1_time=12000&"
					"player1_score=100&player1_awards=1,2,3&player1_xp=23&player1_woc=laser"
					,playerid,playerid+1);
				postqueue(hardcmd,harddata);
			} */
		}
	}
	if (sv)
		postproc();
// log out old bclients
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		for (i=0;i<tsp->numcl;i++) {
			if (tsp->bclients[i].cl) {
				if ((tsp->bclients[i].cl->flags & SF_CLOSED) || tsp->bclients[i].kick) {
					if (tsp->bclients[i].kick)
						CONSOLE->printf("client %d kicked",i);
					if (tsp->bclients[i].cl->flags & SF_CLOSED)
						CONSOLE->printf("client %d closed",i);
					freesocker(tsp->bclients[i].cl);
					tsp->bclients[i].cl=0;
					if (tsp->bclients[i].ps==PS_GOTHI)
						--tsp->curnumcars;
					--tsp->numcl;
					tsp->bclients[i]=tsp->bclients[tsp->numcl];
					--i;
				}
			}
		}
	}
// remove tracks with no players in it
	for (k=0;k<numgames;++k) {
		if (!trackslots[k].numcl && trackslots[k].ts!=TS_CONNECT) { // no players and not connecting, remove
			--numgames;
			CONSOLE->printf("closing trackslot ng:%d",numgames);
			trackslots[k]=trackslots[numgames];
			--k;
		}
	}
// see if need to open a new track
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		if (tsp->numcl<tsp->numcars && tsp->ts==TS_CONNECT) {
			break;
		}
	}
	if (k==numgames && numgames<MAXGAMES) {
		buildatrackslot(&trackslots[numgames]);
		++numgames;
		CONSOLE->printf("opening trackslot ng:%d",numgames);
	}
// log in new bclients
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		if (tsp->numcl<tsp->numcars && tsp->ts==TS_CONNECT) {
			while(1) {
				socker *cl=tcpgetaccept(sv);
				if (cl) {
					memset(&tsp->bclients[tsp->numcl],0,sizeof(struct client));
					tsp->bclients[tsp->numcl].cl=cl;
					tsp->bclients[tsp->numcl].totread=tsp->bclients[tsp->numcl].totwrite=0;
					tsp->bclients[tsp->numcl].inactivetime=getmillisec(); // init inactive timeout
					tsp->bclients[tsp->numcl].ps=PS_WAITHI;
					CONSOLE->printf("client %d connected",tsp->numcl);
					++tsp->numcl;
				} else
					break;
			}
		}
	}
// see what bclients are saying
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		for (i=0;i<tsp->numcl;i++) {
			tcpfillreadbuff(tsp->bclients[i].cl); // get from net
			while (tsp->ts!=TS_SENTGO) { // if in SENTGO, then don't read until ALLWRITTEN/INGAME
// parse read buffer
				C8 litlinbuff[LBUFFSIZE+1];
//				C8 litloutbuff[LBUFFSIZE];
//				litloutbuff[LBUFFSIZE]='\0';
				S32 tid=0;
				S32 numread=tcpreadtoken8(tsp->bclients[i].cl,&tid,litlinbuff,LBUFFSIZE);
				if (!numread)
					break; // done get from net
				tsp->bclients[i].inactivetime=getmillisec(); // got something, reset inactive timeout
				litlinbuff[numread]='\0';
// process buffers in --> out
				tsp->bclients[i].totread+=numread;
				if (tid!=PKT_KEY) {
					CONSOLE->printf("got %d/%d bytes from client %d, id %d",numread,tsp->bclients[i].totread,i,tid);
//					logger("broadcast server: got '%s' from client %d, id %d\n",litlinbuff,i,tid);
				}
				if (tsp->ts==TS_CONNECT) {
					if (tid==PKT_HI) {
// read hi packet
						parseacarslot(&tsp->bclients[i].cs,litlinbuff); // cd (cardata) from client
						if (tsp->curnumcars==0) { // first car get to decide track and numcars
							tsp->numcars=tsp->bclients[i].cs.numcars;
							mystrncpy(tsp->fulltrackname,tsp->bclients[i].cs.fulltrackname,200);
							mystrncpy(tsp->skyboxname,tsp->bclients[i].cs.skyboxname,200);
						}
//						tsp->bclients[i].cs.cid=tsp->curnumcars;
						++tsp->curnumcars;
						tsp->bclients[i].ps=PS_GOTHI;
// send pl packet
						C8* plpack=make_pl_packet(tsp,i);
						S32 pllen=strlen(plpack);
						S32 numwrote=tcpwritetoken8(tsp->bclients[i].cl,PKT_PL,plpack,pllen);
						if (numwrote!=pllen) {
							tsp->bclients[i].kick=true;
							CONSOLE->printf("write error pl, client %d, kick",i);
						}
						tsp->bclients[i].totwrite+=numwrote;
						CONSOLE->printf("wrote pl %d/%d bytes to client %d",numwrote,tsp->bclients[i].totwrite,i);
					} else {
						if (tsp->bclients[i].ps!=PS_WAITHI) {
							tsp->bclients[i].kick=true;
							CONSOLE->printf("didn't get hi, client %d, kick",i);
						}
					}
// broadcast all other packets
				} else if (tsp->ts==TS_INGAME) {
					S32* ibuff;
					S32 from,to;
					S32 cid,tme,tpos;
					switch(tid) { // connect packets
					case PKT_HI:
					case PKT_PL:
					case PKT_GO:
						break;
					case PKT_RESULT:
						ibuff=(S32*)litlinbuff;
						cid=ibuff[0];
						tme=ibuff[1];
						tpos=ibuff[2];
						CONSOLE->printf("got a result from %d, cid %d, tm %d:%d",i,cid,tme/100,tme%100);
						if (i==0) { // player0 posts all results for now
							if (postcmd=="hardcode" && postdata=="hardcode") {
								C8 tname[100];
								mgetname(tsp->fulltrackname,tname);
								carslot* ccs=&tsp->bclients[cid].cs;
								C8 hardcmd[50],harddata[1000];
								sprintf(hardcmd,"/race/%d",ccs->gameid); // time in centiseconds
								sprintf(harddata,"op=save&track=%s&rule1=7&rule2=2&"
									"player1_id=%d&player1_car=%s&player1_finished=1&player1_position=%d&player1_time=%d&"
									"player1_score=100&player1_awards=1,2,3&player1_xp=23&player1_woc=laser",
									tname,ccs->playerid,ccs->body,tpos,tme);
								postqueue(hardcmd,harddata);
							} else {
								postqueue(postcmd,postdata);
							}
						}
						break;
					default: // broadcast game packets
						to=litlinbuff[0];
						from=i;
						litlinbuff[0]=from;
						for (j=0;j<tsp->numcl;++j) {
							if (to==-1 || j == to) { // to j or to all
								if (j!=from) { // not to yourself
									S32 numwrote=tcpwritetoken8(tsp->bclients[j].cl,tid,litlinbuff,numread);
									if (tid!=PKT_KEY)
										CONSOLE->printf("write token id %d from %d to %d",tid,from,j);
									if (numwrote!=numread) {
										tsp->bclients[i].kick=true;
										CONSOLE->printf("write error game, client %d, kick",i);
									}
								}
							}
						}
						break;
					};
				}
			}
// timeout?
			if (getmillisec()-tsp->bclients[i].inactivetime>(INACTIVETIME*1000)) {
				tsp->bclients[i].kick=true; // inactive
//				--tsp->curnumcars;
				CONSOLE->printf("client %d timed out, kick",i);
			} 
		}
	}
// if got all players, go
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		if (tsp->curnumcars==tsp->numcars && tsp->ts==TS_CONNECT) {
			for (i=0;i<tsp->numcl;++i) { // to
				for (j=0;j<tsp->numcl;++j) { // from
					if (tsp->bclients[i].ps==PS_GOTHI && tsp->bclients[j].ps==PS_GOTHI) {
						C8* gop=make_go_packet(tsp,&tsp->bclients[j].cs,j,i);
						S32 goplen=strlen(gop);
						S32 numwrote=tcpwritetoken8(tsp->bclients[i].cl,PKT_GO,gop,goplen);
						if (numwrote!=goplen) {
							tsp->bclients[i].kick=true;
							CONSOLE->printf("write error go, client %d, kick",i);
						}
						tsp->bclients[i].totwrite+=numwrote;
						CONSOLE->printf("wrote %d/%d bytes from %d to %d",numwrote,tsp->bclients[i].totwrite,j,i);
					}
				}
			}
			CONSOLE->printf("trackslot GO! ng:%d",numgames);
			tsp->ts=TS_SENTGO;
		}
	}
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		bool allwritten=true;
		for (i=0;i<tsp->numcl;++i) // send write buffers to net
			if (!tcpsendwritebuff(tsp->bclients[i].cl))
				allwritten=false;
// see if server done with initial tasks, flush write buffers..
		if (allwritten==true && tsp->ts==TS_SENTGO) {
			CONSOLE->printf("sent go, allwritten ng:%d",numgames);
//			++gameid;
			tsp->ts=TS_INGAME;
		}
	} // next game
	frm++;
}

void broadcast_server_draw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	video_sprite_end(); 
}

void broadcast_server_exit()
{
// free ui
	popdir();
	delete rl;
	rl=0;
	exit_res3d();
// free network
	S32 i,k;
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&trackslots[k];
		for (i=0;i<tsp->numcl;i++) {
			freesocker(tsp->bclients[i].cl);
		}
	}
	numgames=0;
	if (sv)
		freesocker(sv);
	sv=0;
	postexit();
	uninitsocker();
}
