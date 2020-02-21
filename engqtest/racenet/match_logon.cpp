#define RES3D // alternate shape class
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "../u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_logon.h"

// some global game stuff
struct socker *matchcl;
char username[256]; // snapshot of LISTBOX1->getidxname()
static char password[256];
static C8 mserverip[256];
static C8 mserverport[256];

carenagameinfo cgi;

struct romgame {
	C8* gamenames;
	S32 gamehoststates;
	S32 gamejoinstates;
	bool gamequicks;	// game can be launched from the quickgame state, 
						// only 'testA' (join doesn't know which game so picks 'testA'
	U32 port;
};

romgame romgames[]={
	{"testA" ,STATE_MATCHOPTIONS  ,STATE_MATCHGAME1     ,true ,5411},
	{"Carena",STATE_OLDCARENALOBBY,STATE_OLDCARENALOBBY,false,5416},
//	{"Carena",STATE_OLDCARENALOBBY,STATE_OLDCARENATOGAME,false,5416},
};
#define NGAMES (sizeof(romgames)/sizeof(romgames[0]))

U32 getngames()
{
	return NGAMES;
}

const C8* getgamename(U32 idx)
{
	return romgames[idx].gamenames;
}

S32 getgamehoststate(U32 idx)
{
	return romgames[idx].gamehoststates;
}

S32 getgamejoinstate(U32 idx)
{
	return romgames[idx].gamejoinstates;
}

S32 getgamemasterport(U32 idx) // get game master port start
{
	return romgames[idx].port;
}

S32 getgameidx(const C8* gamename)
{
	U32 i;
	for (i=0;i<NGAMES;++i)
		if (!strcmp(gamename,romgames[i].gamenames))
			return (S32)i;
	errorexit("unknown gamename '%s'",gamename);
	return -1;
}

bool canbequick(U32 idx)	// can launch from quickgame
{
	return romgames[idx].gamequicks;
}

// build a master or connect socket depending on carenagameinfo cgi
socker* makecgisock(socker** usp)
{
	U32 i;
	socker *ts=0;
	socker *us=0;
	if (usp)
		*usp=0;
	if (!cgi.isvalid || !getnmyip()) {
		return 0;
	}
	if (cgi.ishost) {
		for (i=cgi.port;i<cgi.port+cgi.nports;++i) {
			ts=tcpmakemaster(0,i);
			if (ts) {
				logger("lobby: tcpmakemaster for game at %s:%d\n",
				  ip2str(ts->ip),ts->port);
				break;
			}
		}
		if (!ts)
			return 0;
		if (usp) {
			us=udpmake(ts->ip,ts->port);
			if (us) {
				*usp=us;
			} else {
				freesocker(ts);
				return 0;
			}
		}
	} else {
		ts=tcpgetconnect(cgi.ip,cgi.port);
		if (!ts)
			return 0;
		if (usp) {
			us=udpmake(ts->ip,ts->port);
			if (us) {
				*usp=us;
			} else {
				freesocker(ts);
				return 0;
			}
		}
	}
	return ts;
}

// end some global game stuff

static shape *rl;
static shape* focus;
//static listbox* LISTBOX1,*LISTBOXPW; // username list
static listboxn* LISTBOXNP; // username password list
static edit* EDIT1,*EDITPW; // edit username,password
static pbut* PBUT3; // remove username
static pbut* PBUT1; // logon 
static pbut* PBUTPICKSERVER;
static pbut* PBUT6; // quit

//static listbox* LISTBOXTEST;
//static hscroll* HSLIDETEST;

enum {LOGENTRY,LOGSEND,LOGWAIT,LOGYES};
static int logstatus;
static int logtimer;
static int keepsock;

static fontq* afont;

void match_logon_init()
{
	int i;
	keepsock=0;
	matchcl=0;
	initsocker();
	logstatus=LOGENTRY;
	logger("logon init\n");
	video_setupwindow(800,600);
	pushandsetdir("fonts");
	afont=new fontq("med");
	afont->setspace(20);
	afont->setfixedwidth(20);
	afont->setscale(.5f);
	popdir();
	pushandsetdir("lobbysystem");
// get match server ip and port from pickservercfg.txt
	mserverip[0]='\0';
	mserverport[0]='\0';
	script* sc=new script("pickservercfg.txt");
	S32 nsc=sc->num();
	vector<string> mips;
	vector<string> mports;
	S32 mipidx=-1,mportidx=-1;
	for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"ip"))
			mips.push_back(sc->idx(i+1));
		else if (!strcmp(sc->idx(i).c_str(),"port"))
			mports.push_back(sc->idx(i+1));
		else if (!strcmp(sc->idx(i).c_str(),"ipidx"))
			mipidx=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"portidx"))
			mportidx=atoi(sc->idx(i+1).c_str());
		else
			errorexit("unknown pickserver cfg command '%s'",sc->idx(i).c_str());
	}
	delete sc;
	if (mipidx>=0 && mipidx<(S32)mips.size())
		strcpy(mserverip,mips[mipidx].c_str());
	if (mportidx>=0 && mportidx<(S32)mports.size())
		strcpy(mserverport,mports[mportidx].c_str());
// setup menu
	init_res3d(800,600);
	rl = res_loadfile("logonres.txt");
//	LISTBOX1=rl->find<listbox>("LISTBOX1");
//	LISTBOXPW=rl->find<listbox>("LISTBOXPW");
	LISTBOXNP=rl->find<listboxn>("LISTBOXNP");
	EDIT1=rl->find<edit>("EDIT1");
	EDITPW=rl->find<edit>("EDITPW");
	PBUT3=rl->find<pbut>("PBUT3");
	PBUT1=rl->find<pbut>("PBUT1");
	PBUTPICKSERVER=rl->find<pbut>("PBUTPICKSERVER");
	PBUT6=rl->find<pbut>("PBUT6");

/*	LISTBOXTEST=rl->find<listbox>("LISTBOXTEST");
	for (i=0;i<100;++i) {
		C8 s[100];
		sprintf(s,"numbering %d",i);
		LISTBOXTEST->addstring(s);
	}
	LISTBOXTEST->setidxc(20);
	HSLIDETEST=rl->find<hscroll>("HSLIDETEST");
	HSLIDETEST->setminmaxval(1,5); */

/*
for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"name"))
			LISTBOX1->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"pass"))
			LISTBOXPW->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"nameidx")) {
			LISTBOX1->setidxc(atoi(sc->idx(i+1).c_str()));
			LISTBOXPW->setidxc(atoi(sc->idx(i+1).c_str()));
		} else
			errorexit("unknown logon cfg command '%s'",sc->idx(i).c_str());
	}
*/
	sc=new script("logoncfg.txt");
	nsc=sc->num();
	S32 npidx=-1;
	for (i=0;i<nsc;) {
		if (i+2<nsc && !strcmp(sc->idx(i).c_str(),"name")) {
			vector<string> ele;
			ele.push_back(sc->idx(i+1));
			ele.push_back(sc->idx(i+2));
			LISTBOXNP->addstring(ele);
			i+=3;
		} else if (i+1<nsc && !strcmp(sc->idx(i).c_str(),"nameidx")) {
			npidx=atoi(sc->idx(i+1).c_str());
			i+=2;
		} else {
			errorexit("unknown logon cfg command '%s'",sc->idx(i).c_str());
		}
	}
	delete sc;
	if (npidx>=LISTBOXNP->getnumidx())
		errorexit("bad logon cfg name idx");
	LISTBOXNP->setidxc(npidx);
	PBUT1->setvis(0);
//	EDIT1->settname(LISTBOX1->getidxname());
//	EDITPW->settname(LISTBOXPW->getidxname());
	if (LISTBOXNP->getnumidx()>0) {
		vector<string> ele=LISTBOXNP->getidxname();
		if (ele.size()!=2)
			errorexit("bad size for columns");
		EDIT1->settname(ele[0].c_str());
		EDITPW->settname(ele[1].c_str());
	}
	focus=EDIT1;
	username[0]='\0';
	password[0]='\0';
}

	
void match_logon_proc()
{
	if (KEY==K_ESCAPE) {
		changestate(STATE_MATCHTITLE);
	}
	int i;
//	if (PBUT1->getvis()==0) {
//		i=LISTBOX1->getidx();
		i=LISTBOXNP->getidx();
		bool v = logstatus==LOGENTRY && i>=0 && strlen(mserverip) && strlen(mserverport);
//			EDIT1->settname(LISTBOX1->getidxname());
		PBUT1->setvis(v);
//		}
//	}
// ui
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		if (KEY==K_TAB) {
			if (focus==EDIT1)
				focus=EDITPW;
			else if (focus==EDITPW)
				focus=EDIT1;
		}
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0;
//		shape* focush = ret>=0 ? focus : 0;
//		shape* focusa = ret>=0 ? focus : 0;
//		shape* focusa = ret>=0 ? focus : 0;
		if (focusa==PBUT1) {
//		case PBUT1:
// attempt to logon
/*			i=LISTBOX1->findstring(EDIT1->gettname());	// -1 if not found
			if (i>=0) {
				LISTBOX1->setidxc(i);
			} else {
				LISTBOX1->addstring(EDIT1->gettname());
				LISTBOX1->setidxc(LISTBOX1->getnumidx()-1);
			} */
			if (logstatus==LOGENTRY) {
				matchcl=tcpgetconnect(str2ip(mserverip),atoi(mserverport));
				if (matchcl) {
					logtimer=150;
					logstatus=LOGSEND;
					PBUT1->setvis(0);
				}
				vector<string> ele=LISTBOXNP->getidxname();
				strcpy(username,ele[0].c_str());
				strcpy(password,ele[1].c_str());
			}
		} else if (focusa==PBUT6) {
			keepsock=0;
			changestate(STATE_MATCHTITLE);
		} else if (focusa==PBUTPICKSERVER) {
			keepsock=0;
			pushchangestate(STATE_MATCHPICKSERVER);
		} else if (focusa==PBUT3) {
//			LISTBOX1->removeidx();
//			LISTBOXPW->removeidx();
			LISTBOXNP->removeidx();
		} else if (focusa==LISTBOXNP) {
//			LISTBOX1->setidxc(ret);
//			LISTBOXPW->setidxc(ret);
			vector<string> ele=LISTBOXNP->getidxname();
			EDIT1->settname(ele[0].c_str());
			EDITPW->settname(ele[1].c_str());
/*		} else if (focusa==LISTBOX1) {
			LISTBOX1->setidxc(ret);
			LISTBOXPW->setidxc(ret);
			EDIT1->settname(LISTBOX1->getidxname());
			EDITPW->settname(LISTBOXPW->getidxname());
		} else if (focusa==LISTBOXPW) {
			LISTBOX1->setidxc(ret);
			LISTBOXPW->setidxc(ret);
			EDIT1->settname(LISTBOX1->getidxname());
			EDITPW->settname(LISTBOXPW->getidxname()); */
		} else if (focusa==EDIT1 || focusa==EDITPW) {
			i=LISTBOXNP->findstring(EDIT1->gettname());	// -1 if not found
			vector<string> ele;
			ele.push_back(EDIT1->gettname());
			ele.push_back(EDITPW->gettname());
			if (i>=0) { // change password
				LISTBOXNP->setidxc(i);
//				LISTBOXPW->setidxc(i);
//				LISTBOXPW->changestring(EDITPW->gettname(),i); // change password
				LISTBOXNP->changestring(ele,i);

			} else { // add new name
/*				LISTBOX1->addstring(EDIT1->gettname());
				S32 wh=LISTBOX1->sort();
				LISTBOXPW->addstring(EDITPW->gettname(),wh);
				LISTBOX1->setidxc();
				LISTBOXPW->setidxc();
				*/
				LISTBOXNP->addstring(ele);
				LISTBOXNP->sort();
				LISTBOXNP->setidxc();
			}
			EDIT1->settname("");
			EDITPW->settname("");
		}
	}
// end ui
// collect server messages
	if (matchcl) {
		int len;
		char token[200];
//		vector<char> token(200+1);
		char *tok;
		tcpfillreadbuff(matchcl);
		if (logstatus==LOGWAIT) {
			len=tcpreadtoken32(matchcl,0,token,200);
// logged yes, if found, successful logon
			if (len>0) {
				token[len]='\0';
//				con16_printf(constat,"recieving '%s'\n",token);
				tok=strtok(token," ");
				if (tok && !strcmp(tok,"logged")) {
					tok=strtok(NULL,"");
					if (tok && !strcmp(tok,"yes"))
						logstatus=LOGYES;
					else
						logstatus=LOGENTRY;
				} else
					logstatus=LOGENTRY;
			} else logtimer--;
				if (logtimer==0)
					logstatus=LOGENTRY;
		}
//		con16_printf(constat,"trying to logon\n");
		else if (logstatus==LOGSEND) {
// logon username passwd, send to server
//			username[0]='\0';
			C8 sendstr[512];
			sprintf(sendstr,"logon %s %s",username,password);
			len=strlen(sendstr);
			len=tcpwritetoken32(matchcl,0,sendstr,len);
			if (len>0) {
//				con16_printf(constat,"'%s' sent\n",token);
				logstatus=LOGWAIT;
			} else {
				logtimer--;
				if (logtimer==0)
					logstatus=LOGENTRY;
			}
		} 
		if (logstatus==LOGENTRY) { // nope
			freesocker(matchcl);
			matchcl=NULL;
			keepsock=0;
		} else if (logstatus==LOGYES) { // we logged on!
			keepsock=1;
			changestate(STATE_MATCHLOBBY);
		} 
		tcpsendwritebuff(matchcl);
	}
}

#define FSX 400
#define FSY 12
#if 0
void match_logon_draw2d()
{
//	cliprect32(B32,0,0,WX,WY,C32BLACK);
	rl->draw();
/*	outtextxyf32(B32,WX/2-25*4,10,C32WHITE,"logon to 'TRN' (match server)");
	if (LISTBOXNP->getidx()>=0) {
		vector<string> ele=LISTBOXNP->getidxname();
		outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"name: '%s', password '%s'",ele[0].c_str(),ele[1].c_str());
	} else
		outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"pick a username");
	if (strlen(mserverip))
		outtextxyf32(B32,WX/2-25*4,40,C32WHITE,"ip: '%s'",mserverip);
	else
		outtextxyf32(B32,WX/2-25*4,40,C32WHITE,"pick a server ip");
	if (strlen(mserverport))
		outtextxyf32(B32,WX/2-25*4,50,C32WHITE,"port '%s'",mserverport);
	else
		outtextxyf32(B32,WX/2-25*4,50,C32WHITE,"pick a server port");
	outtextxyf32(B32,0,0,C32WHITE,"focus = %p",focus); */
}
#endif

void match_logon_draw3d()
{
//	logger("logon draw3d\n");
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();

//	afont->print(50,50,50,20,F32YELLOW,"LAP");
	afont->print(800/2-25*4,10,FSX,FSY,F32WHITE,"logon to 'TRN' (match server)");
	if (LISTBOXNP->getidx()>=0) {
		vector<string> ele=LISTBOXNP->getidxname();
		afont->print(800/2-25*4,30,FSX,FSY,F32WHITE,"name: '%s', password '%s'",ele[0].c_str(),ele[1].c_str());
	} else
		afont->print(800/2-25*4,30,FSX,FSY,F32WHITE,"pick a username");
	if (strlen(mserverip))
		afont->print(800/2-25*4,40,FSX,FSY,F32WHITE,"ip: '%s'",mserverip);
	else
		afont->print(800/2-25*4,40,FSX,FSY,F32WHITE,"pick a server ip");
	if (strlen(mserverport))
		afont->print(800/2-25*4,50,FSX,FSY,F32WHITE,"port '%s'",mserverport);
	else
		afont->print(800/2-25*4,50,FSX,FSY,F32WHITE,"pick a server port");
	afont->print(0,0,FSX,FSY,F32WHITE,"focus = %p",focus);

	video_sprite_end(); 
}

void match_logon_exit()
{
	FILE *fp;
	int i,nlb;
	fp=fopen2("logoncfg.txt","wb");
	nlb=LISTBOXNP->getnumidx(); // number of users
	for (i=0;i<nlb;i++) {
		vector<string> ele=LISTBOXNP->getidxname(i);
		fprintf(fp,"name \"%s\" \"%s\"\n",ele[0].c_str(),ele[1].c_str());
	}
	fprintf(fp,"nameidx %d\n",LISTBOXNP->getidx());
	fclose(fp);
	popdir();
	delete rl;
	if (!keepsock) {
		if (matchcl)
			freesocker(matchcl);
		matchcl=NULL;
		uninitsocker();
	}
	delete afont;
	afont=0;
	exit_res3d();
}
