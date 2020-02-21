#define RES3D
#define D2_3D
#include <time.h>
#include <m_eng.h>
#include <l_misclibm.h>

#include "broadcast_server.h"
#include "packet.h"
#include "gameinfo.h"
#include "../engine7test/n_usefulcpp.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "../timelogger.h"
#include "../u_states.h"

// 5 buckets system of 2 kinds
// single sockets
//	connect 1 // player just connected
//  logon   2 // player sucessfully logged in
// group sockets
//  fill    3 // game forming
//  sentgo  4 // waiting for all data to be sent
//  ingame  5 // playing game, broadcast all packets

#define INACTIVETIMENOHI 10 // seconds of not hearing anything from connected client (10 seconds) no hi packet
#define INACTIVETIME 720 // seconds of not hearing anything from logged on client overall (12 minutes)
#define WAITOPPTIME 120 // seconds of load time (2 minutes) before hi packet
#define WAITOPPTIME2 1200 // seconds of load time (20 minutes) before hi packet
#define GAMETIME 10 // seconds of not hearing anything from logged on client (15 seconds) after hi packet
#define LBUFFSIZE 512

#define MAXCONNECTS 1000
#define MAXLOGONS 1000
#define MAXFILLS 1000
#define MAXSENTGOS 1000
#define MAXGAMES 1000

namespace broadcast_server {

// ui
shape* rl,*focus;
pbut* QUIT;
listbox* CONSOLE;
// font for numerating buckets
fontq* broadfont;

// start log statistics
#define LOGUPDATETIME (10*1000) // millisec between i/o samples
U32 bytesread,byteswritten;
U32 curtick;
timelogger* tl;

int tcpreadtoken8log(struct socker *s,int *id,char *tok,int len)
{
	int ret=tcpreadtoken8(s,id,tok,len);
	if (ret>0)
		bytesread+=2+ret;
	return ret;
}

int tcpwritetoken8log(struct socker *s,int id,char *tok,int len)
{
	int ret=tcpwritetoken8(s,id,tok,len);
	if (ret>0)
		byteswritten+=2+ret;
	return ret;
}

void updateiolog() // LOGUPDATETIME has now passed
{
	U32 sp=LOGUPDATETIME/1000; // seconds passed, 1 - whatever
	const C8* fs="BR %7d, BW %7d, S %3d, BR/S %7d, BW/S %7d\n";
	tl->printf(fs,bytesread,byteswritten,sp,bytesread/sp,byteswritten/sp);
	CONSOLE->printf(fs,bytesread,byteswritten,sp,bytesread/sp,byteswritten/sp);
	U32 ch=tl->getchange();
	const time_t ti=time(0);
	tm* newtime=gmtime(&ti);
	if (ch&MINUTENEW)
		CONSOLE->printf("minutelog %s",asctime(newtime));
	if (ch&HOURNEW)
		CONSOLE->printf("hourlog %s",asctime(newtime));
	if (ch&DAYNEW)
		CONSOLE->printf("daylog %s",asctime(newtime));
	bytesread=byteswritten=0;
}

// end log statistics


// try bucket method, 5 different buckets
// alt try state method
//enum playerstate {PS_WAITHI,PS_GOTHI};
//enum trackstate {TS_CONNECT,TS_SENTGO,TS_INGAME};
int frm;


struct socker *sv; // master socket
int nips; // got my ip yet?
int ninfoips; // other ips (from names)
//C8 pl_packet[]="1 0 2009_testcheck\\newcheck2.bws cubemap_mountains.jpg 1.000000 0.000000 0.000000 7"; // test pl packet
//S32 pl_packetlen=sizeof(pl_packet)-1;

//C8 go_packet[]="65536#jim#DuneItUp#255#0#0#255#255#255#0#0#255#paint11#decal12#8#9#10#1#0#25#17#3#27#18#16#50#55#60#65#70#75"; // test go packet
//S32 go_packetlen=sizeof(go_packet)-1;


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
	C8 gametype;
	C8 ntrackname[200];
//	C8 skyboxname[200];
	S32 numcars;
//	float scaleup;
//	float trackstart;
//	float trackend;
	S32 rules;
	S32 numbots;
// numhumans is numcars-numbots
};

struct client {
	struct socker *cl;
	int inactivetime;
	bool kick;
	int totread,totwrite;
	carslot cs;
	bool sentkey; // cut timeout down for those who sent key packets
//	playerstate ps;
};

client connectslots[MAXCONNECTS];
S32 numconnects;

client logonslots[MAXLOGONS];
S32 numlogons;

struct trackslot {
// net
	client bclients[OL_MAXPLAYERS];
//	trackstate ts;
// filling up
	S32 numcars;
//	S32 curnumcars; // number of players connected and sent a hi packet
	S32 numcl; // number of players connected
// track
	U32 tgameid;
	C8 tgametype;
	C8 ntrackname[200];
//	C8 skyboxname[200];
//	float scaleup;
//	float trackstart;
//	float trackend;
	S32 rules;
	S32 numbots;
	S32 session;
	bool gothere; // ratchet for session
};
trackslot fillslots[MAXFILLS];
S32 numfills;

trackslot sentgoslots[MAXSENTGOS];
S32 numsentgos;

trackslot gameslots[MAXGAMES];
S32 numgames;


U32 port; // listen port of server

///////// post //////////
U32 postip;
U32 postport;
string posthost,postcmd,postdata;
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
				C8 clstr[300];
				sprintf(clstr,"Host: %s\r\n",posthost.c_str());
				httpstr2 += clstr;
				httpstr2 += "Content-Type: application/x-www-form-urlencoded\r\n";
				httpstr2 += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:0.9.2) Gecko/20010726 Netscape6/6.1\r\n";
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

/*
void buildatrackslot(trackslot* ts)
{
// net
	memset(ts->bclients,0,sizeof(ts->bclients));
//	ts->ts=TS_CONNECT;
// filling up
	ts->numcars=2;
//	ts->curnumcars=0;
	ts->numcl=0;
// track
	ts->tgameid=4000000000U; // 4 billion
	strcpy(ts->fulltrackname,"");
	strcpy(ts->skyboxname,"");
//	strcpy(ts->fulltrackname,"prehistoric\\prehistoric.bws");
//	strcpy(ts->skyboxname,"prehistoric.jpg");
	ts->scaleup=1;
	ts->trackstart=0;
	ts->trackend=1;
	ts->rules=7;
}
*/
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
	mystrncpy(cs->ntrackname,tok,200);

	tok=strtok(NULL,"#"); // gameid
	cs->gameid=atoi(tok);
	tok=strtok(NULL,"#"); // gametype
	cs->gametype=tok[0];
	tok=strtok(NULL,"#"); // playerid
	cs->playerid=atoi(tok);

	tok=strtok(NULL,"#"); // rules
	cs->rules=atoi(tok);
	tok=strtok(NULL,"#"); // nbots
	cs->numbots=atoi(tok);

	CONSOLE->printf("new game '%s' nc %d, nb %d, nh %d",cs->ntrackname,cs->numcars,cs->numbots,cs->numcars-cs->numbots);
	CONSOLE->printf("gid %d, gt %c, pid %d",cs->gameid,cs->gametype,cs->playerid);
	tok=strtok(NULL,"#"); // username
	if (tok)
		strcpy(cs->name,tok);
	tok=strtok(NULL,"#"); // carmake
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
	sprintf(pl,"%d %d %s %d %c %d",
		ts->numcars,slotnum,ts->ntrackname,ts->rules,ts->tgametype,ts->numbots);
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
	video_setupwindow(800,600);
	init_res3d(800,600);
// ui
pushandsetdir("racenetdata/broadcast");
	script sc("broadcast_servercfg.txt");
	if (sc.num()>=5) {
		port=atoi(sc.idx(0).c_str());
//		postip=str2ip(sc.idx(1).c_str());
		postip=~0;
		posthost=sc.idx(1);
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
	ninfoips=0;
	sv=NULL;
	initsocker();
	CONSOLE->printf("broadcast_server init");
	memset(connectslots,0,sizeof(connectslots));
	numconnects=0;
	memset(logonslots,0,sizeof(logonslots));
	numlogons=0;
	memset(fillslots,0,sizeof(fillslots));
	numfills=0;
	memset(sentgoslots,0,sizeof(sentgoslots));
	numsentgos=0;
	memset(gameslots,0,sizeof(gameslots));
	numgames=0;
	logger("sizeof connectslots is %d\n",sizeof(connectslots));
	logger("sizeof logonslots is %d\n",sizeof(logonslots));
	logger("sizeof fillslots is %d\n",sizeof(fillslots));
	logger("sizeof sentgoslots is %d\n",sizeof(sentgoslots));
	logger("sizeof gameslots is %d\n",sizeof(gameslots));
	postinit();
	pushandsetdir("fonts");
	broadfont=new fontq("med");
	popdir();
// log statistics
	bytesread=byteswritten=0;
	curtick=getmillisec();
	tl=new timelogger("racenetdata/broadcast","brd_");
}

static const C8* medals2str(int* meds)
{
	static C8 ms[100];
	S32 i;
	C8 first=true;
	for (i=0;i<MED_MAXMEDALS;++i)
		if (meds[i])
			break;
	if (i==MED_MAXMEDALS)
		return ""; // no medals
	strcpy(ms,"&player1_awards");
	for (i=0;i<MED_MAXMEDALS;++i) {
		if (meds[i]) {
			C8 ss[20];
			if (first) {
				sprintf(ss,"=%d",i+1);
				first=false;
			} else {
				sprintf(ss,",%d",i+1);
			}
			strcat(ms,ss);
		}
	}
	return ms;
}

void broadcast_server_proc()
{
	int i,j,k;
	U32 newtick=getmillisec();
	if (newtick-curtick>LOGUPDATETIME) {
		curtick=newtick;
		updateiolog();
	}
// ui
// escape
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0; // see if something happened
		if (focusa==QUIT)
			poporchangestate(STATE_MAINMENU);
	}
// end ui
// get ip(s)
	if (!nips) {
		nips=getnmyip();	// returns 0 until we get one
		if (nips>0) { // show ip's somewhere
			CONSOLE->printf("got myip '%s'",ip2str(getmyip(0))); // remember: just 1 ip2str per printf
			// getmyip(0);
			U32 good=getinfofromname(posthost.c_str()); // initiate getinfo, returns 1 if can do, 0 if try again
			if (!good)
				popstate();
		}
	}
// get ip of host
	if (!ninfoips) {
		ninfoips=getninfoip();
		if (ninfoips>0) { // other ip found (host ip)
			postip=getinfoip(0);
			CONSOLE->printf("got host '%s' ip '%s'",posthost.c_str(),ip2str(postip)); // remember: just 1 ip2str per printf
		}
	}
// if have an ip, then enable start server button when we get a good listen socket
	if (!sv && ninfoips) {
		sv=tcpmakemaster(0,port);
		if (sv) {
			CONSOLE->printf("listen: ip '%s', port %d",ip2str(getmyip(0)),sv->port); // remember: just 1 ip2str per printf
//			for (i=0;i<2;++i) // test postqueue
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

// 5 timeouts
// timeout for connectslots
	for (k=0;k<numconnects;++k) {
		if (getmillisec()-connectslots[k].inactivetime>(INACTIVETIMENOHI*1000)) {
			if (!connectslots[k].kick) {
				connectslots[k].kick=true; // inactive
				CONSOLE->printf("connect %d timed out, kick",k);
			}
		} 
	}
// timeout for logonslots
	for (k=0;k<numlogons;++k) {
		if (getmillisec()-logonslots[k].inactivetime>(INACTIVETIME*1000)) {
			if (!logonslots[k].kick) {
				logonslots[k].kick=true; // inactive
				CONSOLE->printf("logon %d timed out, kick",k);
			}
		} 
	}
// timeout for fillslots
	for (k=0;k<numfills;++k) {
		trackslot* tsp=&fillslots[k];
		for (i=0;i<tsp->numcl;i++) {
			if (getmillisec()-tsp->bclients[i].inactivetime>(INACTIVETIME*1000)) {
				if (!tsp->bclients[i].kick) {
					tsp->bclients[i].kick=true; // inactive
					CONSOLE->printf("fillslot %d:%d timed out, kick",k,i);
				}
			} 
		}
	}
// timeout for sentgoslots
	for (k=0;k<numsentgos;++k) {
		trackslot* tsp=&sentgoslots[k];
		for (i=0;i<tsp->numcl;i++) {
			if (getmillisec()-tsp->bclients[i].inactivetime>(INACTIVETIME*1000)) {
				if (!tsp->bclients[i].kick) {
					tsp->bclients[i].kick=true; // inactive
					CONSOLE->printf("sentgoslot %d:%d timed out, kick",k,i);
				}
			} 
		}
	}
// timeout for gameslots
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&gameslots[k];
		for (i=0;i<tsp->numcl;i++) {
			U32 tm;
			if (tsp->bclients[i].sentkey)
				tm=GAMETIME*1000;
			else if (tsp->numcl==1)
				tm=WAITOPPTIME2*1000; // timeout alot longer, during a 1 human game
			else
				tm=WAITOPPTIME*1000;
			if (getmillisec()-tsp->bclients[i].inactivetime>tm) {
				if (!tsp->bclients[i].kick) {
					tsp->bclients[i].kick=true; // inactive
					CONSOLE->printf("gameslot %d:%d timed out, kick",k,i);
				}
			} 
		}
	}

// 5 logouts
// log out old connectslots, someone left before sent hi packet
	for (k=0;k<numconnects;++k) {
		if (connectslots[k].cl) {
			if ((connectslots[k].cl->flags & SF_CLOSED) || connectslots[k].kick) {
				if (connectslots[k].kick)
					CONSOLE->printf("connect %d kicked",k);
				if (connectslots[k].cl->flags & SF_CLOSED)
					CONSOLE->printf("connect %d closed",k);
				freesocker(connectslots[k].cl);
				--numconnects;
				connectslots[k]=connectslots[numconnects]; // move last to here
				--k;
			}
		}
	}
// log out old logonslots, have hi but not xfered to a fillup bucket yet, (maybe should never happen)
	for (k=0;k<numlogons;++k) {
		if (logonslots[k].cl) {
			if ((logonslots[k].cl->flags & SF_CLOSED) || logonslots[k].kick) {
				if (logonslots[k].kick)
					CONSOLE->printf("logon %d kicked",k);
				if (logonslots[k].cl->flags & SF_CLOSED)
					CONSOLE->printf("logon %d closed",k);
				freesocker(logonslots[k].cl);
				--numlogons;
				logonslots[k]=logonslots[numlogons]; // move last to here
				--k;
			}
		}
	}
// log out old fills, someone left before all players arrived
	for (k=0;k<numfills;++k) {
		trackslot* tsp=&fillslots[k];
		for (i=0;i<tsp->numcl;i++) {
			if (tsp->bclients[i].cl) {
				if ((tsp->bclients[i].cl->flags & SF_CLOSED) || tsp->bclients[i].kick) {
					if (tsp->bclients[i].kick)
						CONSOLE->printf("fill %d:%d kicked",k,i);
					if (tsp->bclients[i].cl->flags & SF_CLOSED)
						CONSOLE->printf("fill %d:%d closed",k,i);
					freesocker(tsp->bclients[i].cl);
//					tsp->bclients[i].cl=0;
//					if (tsp->bclients[i].ps==PS_GOTHI)
//						--tsp->curnumcars;
					--tsp->numcl;
					tsp->bclients[i]=tsp->bclients[tsp->numcl]; // move last to here
					--i;
				}
			}
		}
	}
// log out old sentgos // someone left before all GO packets could be sent
	for (k=0;k<numsentgos;++k) {
		trackslot* tsp=&sentgoslots[k];
		for (i=0;i<tsp->numcl;i++) {
			if (tsp->bclients[i].cl) {
				if ((tsp->bclients[i].cl->flags & SF_CLOSED) || tsp->bclients[i].kick) {
					if (tsp->bclients[i].kick)
						CONSOLE->printf("sentgo %d:%d kicked",k,i);
					if (tsp->bclients[i].cl->flags & SF_CLOSED)
						CONSOLE->printf("sentgo %d:%d closed",k,i);
					freesocker(tsp->bclients[i].cl);
					tsp->bclients[i].cl=0;
//					if (tsp->bclients[i].ps==PS_GOTHI)
//						--tsp->curnumcars;
					--tsp->numcl;
					tsp->bclients[i]=tsp->bclients[tsp->numcl]; // move last to here
					--i;
				}
			}
		}
	}
// log out old games // someone left a game
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&gameslots[k];
		for (i=0;i<tsp->numcl;i++) {
			if (tsp->bclients[i].cl) {
				if ((tsp->bclients[i].cl->flags & SF_CLOSED) || tsp->bclients[i].kick) {
					if (tsp->bclients[i].kick)
						CONSOLE->printf("game %d:%d kicked",k,i);
					if (tsp->bclients[i].cl->flags & SF_CLOSED)
						CONSOLE->printf("game %d:%d closed",k,i);
					freesocker(tsp->bclients[i].cl);
					tsp->bclients[i].cl=0;
// post dnf
					carslot* ccs=&tsp->bclients[i].cs;
					C8 hardcmd[50],harddata[1000];
					if (ccs->gametype=='S')
						sprintf(hardcmd,"/race/%c%d",ccs->gametype,ccs->gameid); // for now only 'S', unregistered user
					else
						sprintf(hardcmd,"/race/%d",ccs->gameid); // don't send any gametype if anything else
					sprintf(harddata,"op=save&track=%s&rule1=7&rule2=2&"
						"player1_id=%d&player1_car=%s&player1_finished=0&session=%d",
						tsp->ntrackname,ccs->playerid,ccs->body,tsp->session);
					postqueue(hardcmd,harddata);

					C8 lbuff[1];
					lbuff[0]=i; // player who disconnected
					S32 m;
					for (m=0;m<tsp->numcl;++m) // send this to all other active players
						tcpwritetoken8log(tsp->bclients[m].cl,PKT_DISCONNECT,lbuff,1);
//					--tsp->numcl;
//					tsp->bclients[i]=tsp->bclients[tsp->numcl]; // move last to here
//					--i;
				}
			}
		}
	}

// 3 remove empties
// remove fills with no players in it, everyone left before everyone arrived
	for (k=0;k<numfills;++k) {
		if (!fillslots[k].numcl) { // no players and not connecting, remove
			--numfills;
			CONSOLE->printf("closing empty fill nf:%d",numfills);
			fillslots[k]=fillslots[numfills]; // move last to here
			--k;
		}
	}
// remove sentgos with no players in it, everyone left before allwritten
	for (k=0;k<numsentgos;++k) {
		if (!sentgoslots[k].numcl) { // no players and not connecting, remove
			--numsentgos;
			CONSOLE->printf("closing empty sentgo sg:%d",numsentgos);
			sentgoslots[k]=sentgoslots[numsentgos]; // move last to here
			--k;
		}
	}
// remove games with no players in it
	for (k=0;k<numgames;++k) {
		S32 cnt=0;
		for (j=0;j<gameslots[k].numcl;++j) {
			if (gameslots[k].bclients[j].cl)
				++cnt;
		}
		if (!cnt) { // no players in game, remove
			--numgames;
			CONSOLE->printf("closing empty game ng:%d",numgames);
			gameslots[k]=gameslots[numgames]; // move last to here
			--k;
		}
	}

// logon to 'connects'
	while(numconnects<MAXCONNECTS) {
		socker *cl=tcpgetaccept(sv);
		if (cl) {
			memset(&connectslots[numconnects],0,sizeof(struct client));
			connectslots[numconnects].cl=cl;
			connectslots[numconnects].inactivetime=getmillisec(); // init inactive timeout
//			connectslots[numconnects].ps=PS_WAITHI;
			++numconnects;
			CONSOLE->printf("connect logon nc:%d",numconnects);
		} else
			break;
	}

// take connects and move to logons when HI packet is read
// see what 'connects' are saying, and move connects into logons (advance)
	for (k=0;k<numconnects;++k) {
		client* bcl=&connectslots[k];
		tcpfillreadbuff(bcl->cl); // get from net
		while(1) {
// parse read buffer
			C8 litlinbuff[LBUFFSIZE+1];
			S32 tid=0;
			S32 numread=tcpreadtoken8log(bcl->cl,&tid,litlinbuff,LBUFFSIZE);
			if (numread<=0)
				break; // done get from net, client has no more to say for now
			bcl->inactivetime=getmillisec(); // got something, reset inactive timeout
			litlinbuff[numread]='\0';
// process buffers in --> out
			bcl->totread+=numread;
			CONSOLE->printf("got %d/%d bytes from connect %d, id %d",numread,bcl->totread,k,tid);
			if (tid==PKT_HI && numlogons<MAXLOGONS) {
// got hi packet, move to logon bucket (delete insert)
				parseacarslot(&bcl->cs,litlinbuff); // cd (cardata) from client
				CONSOLE->printf("connect to logon nl: %d",numlogons);
				logonslots[numlogons++]=connectslots[k];
//				bcl->ps=PS_GOTHI;
				--numconnects;
				connectslots[k]=connectslots[numconnects]; // move last to here
				--k;
			} else {
				bcl->kick=true; // expected a HI packet
				if (tid!=PKT_HI)
					CONSOLE->printf("didn't get HI, connect %d, kick",k);
				else
					CONSOLE->printf("logon overflow, connect %d, kick",k);
			}
		}
	}
// take logons and move into fills AUTO (advance) , no net input necessary, send PL packet
	for (k=0;k<numlogons;++k) {
		client* bcl=&logonslots[k];
		U32 lid=bcl->cs.gameid;
		trackslot* fs=&fillslots[0];
		for (i=0;i<numfills;++i) { // find a matching game to fill
			fs=&fillslots[i];
			if (fs->tgameid==lid && fs->numcl<fs->numcars-fs->numbots) {
				break;
			}
		}
		fs=&fillslots[i];
		if (i==numfills && numfills>=MAXFILLS)
			continue; // no free fills
		client* bcl2=0;
		if (i<numfills) { // existing fill
			fs->bclients[fs->numcl++]=*bcl; // pass logon to existing fill
			bcl2=&fs->bclients[fs->numcl-1];
			CONSOLE->printf("fill added player %d:%d/%d",i,fs->numcl,fs->numcars-fs->numbots);
		} else { // new fill, fs=&fillslots[numfills];
			fs->tgameid=lid;
			fs->tgametype=bcl->cs.gametype;
			fs->bclients[0]=*bcl; // pass logon to new fill
			fs->numcars=bcl->cs.numcars;
			fs->numcl=1;
			fs->session=0;
			fs->gothere=false;
			bcl2=&fs->bclients[0]; // this is the one being passed
			strcpy(fs->ntrackname,bcl->cs.ntrackname);
//			strcpy(fs->skyboxname,bcl->cs.skyboxname);
			fs->rules=bcl->cs.rules;
			fs->numbots=bcl->cs.numbots;
//			fs->scaleup=1;
//			fs->trackstart=0;
//			fs->trackend=1;
			++numfills;
			CONSOLE->printf("fill new %d:%d/%d",i,fs->numcl,fs->numcars-fs->numbots);
		}
		--numlogons;
		logonslots[k]=logonslots[numlogons]; // move last to here
		--k;
// send PL packet
		C8* plpack=make_pl_packet(fs,fs->numcl-1);
		S32 pllen=strlen(plpack);
		S32 numwrote=tcpwritetoken8log(bcl2->cl,PKT_PL,plpack,pllen);
		if (numwrote!=pllen) {
			bcl2->kick=true;
			CONSOLE->printf("write error PL, fill %d:%d, kick",numfills,fs->numcl-1);
		}
		bcl2->totwrite+=numwrote;
		tcpsendwritebuff(bcl2->cl);
		CONSOLE->printf("wrote PL %d/%d bytes to fill client %d:%d",numwrote,bcl2->totwrite,numfills,fs->numcl-1);
	}

// take fills and move into sentgos AUTO (advance) , no net input necessary, send GO packet
	for (k=0;k<numfills;++k) {
		trackslot* tsp=&fillslots[k];
		trackslot* tsp2=0;
		if (tsp->numcars-tsp->numbots==tsp->numcl && numsentgos<MAXSENTGOS) {
			CONSOLE->printf("fill to sentgo %d",k);
			sentgoslots[numsentgos++]=fillslots[k];
			tsp2=&sentgoslots[numsentgos-1]; // this is the one being passed
			--numfills;
			fillslots[k]=fillslots[numfills]; // move last to here
			--k;
// send go packets to all players
			for (i=0;i<tsp2->numcl;++i) { // to
				for (j=0;j<tsp2->numcl;++j) { // from
					C8* gop=make_go_packet(tsp2,&tsp2->bclients[j].cs,j,i);
					S32 goplen=strlen(gop);
					S32 numwrote=tcpwritetoken8log(tsp2->bclients[i].cl,PKT_GO,gop,goplen);
					if (numwrote!=goplen) {
						tsp2->bclients[i].kick=true;
						CONSOLE->printf("write error go, client %d:%d, kick",k,i);
					}
					tsp2->bclients[i].totwrite+=numwrote;
					CONSOLE->printf("wrote %d/%d bytes from %d to %d",numwrote,tsp2->bclients[i].totwrite,j,i);
				} // j next from
			} // i next to
		} // move to sentgos
	} // next fill

// take sentgos and move into games (advance) , when packets allwritten 
	for (k=0;k<numsentgos;++k) {
		trackslot* tsp=&sentgoslots[k];
		bool allwritten=true;
		for (i=0;i<tsp->numcl;++i) // send write buffers to net
			if (!tcpsendwritebuff(tsp->bclients[i].cl))
				allwritten=false;
// see if server done with initial tasks, flush write buffers..
		if (allwritten==true && numgames<MAXGAMES) {
// got allwritten, move to games bucket (delete insert)
			CONSOLE->printf("sentgo to game, allwritten ng:%d",numgames);
//			++gameid;
//			tsp->ts=TS_INGAME;
			gameslots[numgames++]=sentgoslots[k];
			--numsentgos;
			sentgoslots[k]=sentgoslots[numsentgos]; // move last to here
			--k;

		}
	} // next sentgo

// process games, broadcast
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&gameslots[k];
		S32 fgp=-1;	// first good player
		for (i=0;i<tsp->numcl;i++) {
			if (tsp->bclients[i].cl) {
				fgp=i;
				break;
			}
		}
		for (i=0;i<tsp->numcl;i++) {
			tcpfillreadbuff(tsp->bclients[i].cl); // get from net
			while (true) {
// parse read buffer
				C8 litlinbuff[LBUFFSIZE+1];
				S32 tid=0;
				S32 numread=tcpreadtoken8log(tsp->bclients[i].cl,&tid,litlinbuff,LBUFFSIZE);
				if (numread<=0)
					break; // done get from net
				tsp->bclients[i].inactivetime=getmillisec(); // got something, reset inactive timeout
				litlinbuff[numread]='\0';
// process buffers in --> out
				tsp->bclients[i].totread+=numread;
				if (tid!=PKT_KEY) {
					CONSOLE->printf("got %d/%d game bytes from client %d, id %d",numread,tsp->bclients[i].totread,i,tid);
//					logger("broadcast server: got '%s' from client %d, id %d\n",litlinbuff,i,tid);
				}
				S32* ibuff=(S32*)litlinbuff;
				S32 from,to;
				S32 cid,tme,tpos,woc,score,xp;
				S32* medals; // MED_MAXMEDALS
				S32 q;
				switch(tid) { // connect packets
				case PKT_HI:
				case PKT_PL:
				case PKT_GO:
					break;
				case PKT_RESULT:
					if (numread!=4*(6+MED_MAXMEDALS)) // right now 12 * 4 bytes
						break;
					cid=ibuff[0];
					tme=ibuff[1];
					tpos=ibuff[2];
					woc=ibuff[3];
					score=ibuff[4];
					xp=ibuff[5];
					medals=&ibuff[6];
					CONSOLE->printf("got a result from %d, cid %d, tm %d:%d",i,cid,tme/100,tme%100);
					for (q=0;q<MED_MAXMEDALS;++q)
						CONSOLE->printf("medal %d is %d",q,medals[q]);
					if (true) { // all players post results, client only post result for self, less secure, but easier to implement medals earned
//					if (i==fgp) { // lowest active player posts all results, all players post everybodies result
						if (postcmd=="hardcode" && postdata=="hardcode") {
//							C8 tname[100];
//							mgetname(tsp->fulltrackname,tname);
							carslot* ccs=&tsp->bclients[cid].cs;
							C8 hardcmd[50],harddata[1000];
//							sprintf(hardcmd,"/api.test.hwp.workatplay.com/race/%d",ccs->gameid); // time in centiseconds
							if (ccs->gametype=='S')
								sprintf(hardcmd,"/race/%c%d",ccs->gametype,ccs->gameid); // for now only 'S', unregistered user
							else
								sprintf(hardcmd,"/race/%d",ccs->gameid); // don't send any gametype if anything else
							const C8* medalstr=medals2str(medals);
							if (woc<0) {
								sprintf(harddata,"op=save&track=%s&rule1=7&rule2=2&"
									"player1_id=%d&player1_car=%s&player1_finished=1&player1_position=%d&player1_time=%d&"
									"player1_score=%d%s&player1_xp=%d&"
									"session=%d",
									tsp->ntrackname,ccs->playerid,ccs->body,tpos,tme,
									score,medalstr,xp,
									tsp->session);
							} else {
								sprintf(harddata,"op=save&track=%s&rule1=7&rule2=2&"
									"player1_id=%d&player1_car=%s&player1_finished=1&player1_position=%d&player1_time=%d&"
									"player1_score=%d%s&player1_xp=%d&player1_woc=%d&"
									"session=%d",
									tsp->ntrackname,ccs->playerid,ccs->body,tpos,tme,
									score,medalstr,xp,woc,
									tsp->session);
							}
							postqueue(hardcmd,harddata);
						} else {
							postqueue(postcmd,postdata);
						}
					}
					break;
//				case PKT_KEY:
				default: // broadcast game packets
					if (tid==PKT_KEY) {
						tsp->bclients[i].sentkey=true;
					} else if (tid==PKT_HERE) {
						tsp->gothere=true;
					} else if (tid==PKT_REQREMATCH) {
						if (tsp->gothere) {
							tsp->gothere=false;
							++tsp->session;
							CONSOLE->printf("session now %d",tsp->session);
						}
					}
					to=litlinbuff[0];
					from=i;
					litlinbuff[0]=from;
					for (j=0;j<tsp->numcl;++j) {
						if (to==-1 || j == to) { // to j or to all
							if (j!=from) { // not to yourself
								if (tsp->bclients[j].cl) {
									S32 numwrote=tcpwritetoken8log(tsp->bclients[j].cl,tid,litlinbuff,numread);
									if (tid!=PKT_KEY)
										CONSOLE->printf("write token id %d from %d to %d",tid,from,j);
									if (numwrote!=numread && !tsp->bclients[j].kick) {
										tsp->bclients[j].kick=true;
										CONSOLE->printf("write error game, client %d, kick",j);
									}
								}
							} // not self
						} // match broadcast
					} // j num broadcast
					break;
				} // tid switch
			} // while packets to read
		} // i numcl
		for (i=0;i<tsp->numcl;i++) {
			tcpsendwritebuff(tsp->bclients[i].cl); // send to net
		} // i numcl
	} // k numgames
	frm++;
}

void broadcast_server_draw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	broadfont->print(640,10,150,20,F32WHITE,"CONNECTS %d",numconnects);
	broadfont->print(640,40,150,20,F32WHITE,"LOGONS %d",numlogons);
	broadfont->print(640,70,150,20,F32WHITE,"FILLS %d",numfills);
	broadfont->print(640,100,150,20,F32WHITE,"SENTGOS %d",numsentgos);
	broadfont->print(640,130,150,20,F32WHITE,"INGAMES %d",numgames);
	video_sprite_end(); 
}

void broadcast_server_exit()
{
// free ui
	popdir();
	delete rl;
	rl=0;
	exit_res3d();
	delete broadfont;
// free network
	S32 i,k;
// connects
	for (k=0;k<numconnects;++k)
		freesocker(connectslots[k].cl);
	numconnects=0;
// logons
	for (k=0;k<numlogons;++k)
		freesocker(logonslots[k].cl);
	numlogons=0;
// fills
	for (k=0;k<numfills;++k) {
		trackslot* tsp=&fillslots[k];
		for (i=0;i<tsp->numcl;i++) {
			freesocker(tsp->bclients[i].cl);
		}
	}
	numfills=0;
// sentgos
	for (k=0;k<numsentgos;++k) {
		trackslot* tsp=&sentgoslots[k];
		for (i=0;i<tsp->numcl;i++) {
			freesocker(tsp->bclients[i].cl);
		}
	}
	numsentgos=0;
// games
	for (k=0;k<numgames;++k) {
		trackslot* tsp=&gameslots[k];
		for (i=0;i<tsp->numcl;i++) {
			freesocker(tsp->bclients[i].cl);
		}
	}
	numgames=0;
// master
	if (sv)
		freesocker(sv);
	sv=0;
	postexit();
	uninitsocker();
// log statistics
	delete tl;
	tl=0;
}

#if 0
// match gameid if possible
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
/*
// using older trackslots method
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
*/

// see what trackslots bclients are saying
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
				S32 numread=tcpreadtoken8log(tsp->bclients[i].cl,&tid,litlinbuff,LBUFFSIZE);
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
/*						if (tsp->curnumcars==0) { // first car get to decide track and numcars
							tsp->numcars=tsp->bclients[i].cs.numcars;
							mystrncpy(tsp->fulltrackname,tsp->bclients[i].cs.fulltrackname,200);
							mystrncpy(tsp->skyboxname,tsp->bclients[i].cs.skyboxname,200);
						} */
//						tsp->bclients[i].cs.cid=tsp->curnumcars;
//						++tsp->curnumcars;
						tsp->bclients[i].ps=PS_GOTHI;
// send pl packet
						C8* plpack=make_pl_packet(tsp,i);
						S32 pllen=strlen(plpack);
						S32 numwrote=tcpwritetoken8log(tsp->bclients[i].cl,PKT_PL,plpack,pllen);
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
									S32 numwrote=tcpwritetoken8log(tsp->bclients[j].cl,tid,litlinbuff,numread);
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
// timeout for trackslots
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
		if (tsp->numcl==tsp->numcars && tsp->ts==TS_CONNECT) {
			for (i=0;i<tsp->numcl;++i) { // to
				for (j=0;j<tsp->numcl;++j) { // from
					if (tsp->bclients[i].ps==PS_GOTHI && tsp->bclients[j].ps==PS_GOTHI) {
						C8* gop=make_go_packet(tsp,&tsp->bclients[j].cs,j,i);
						S32 goplen=strlen(gop);
						S32 numwrote=tcpwritetoken8log(tsp->bclients[i].cl,PKT_GO,gop,goplen);
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
#endif
