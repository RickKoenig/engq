#define RES3D // alternate shape class
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_logon.h"
#include "match_register.h"

static char rusername[256];
static char rpassword[256];
static C8 mserverip[256];
static C8 mserverport[256];

static shape *rl;
static shape* focus;
//static listbox* LISTBOX1,*LISTBOXPW; // username list
//static listboxn* LISTBOXNP; // username password list
static edit* EDIT1,*EDITPW; // edit username,password
//static pbut* PBUT3; // remove username
static pbut* PBUT1; // register 
static pbut* PBUTPICKSERVER;
static pbut* PBUT6; // quit

enum {LOGENTRY,LOGSEND,LOGWAIT,LOGYES};
static int logstatus;
static int logtimer;
static int keepsock;

static fontq* afont;

// add recent registration to for logon next time
static void	addlogoncfg(const C8* un,const C8* pw)
{
	vector<vector<string> > ll;
	script* sc=new script("logoncfg.txt");
	S32 nsc=sc->num();
	S32 npidx=-1;
	S32 i;
	for (i=0;i<nsc;) {
		if (i+2<nsc && !strcmp(sc->idx(i).c_str(),"name")) {
			vector<string> ele;
			ele.push_back(sc->idx(i+1));
			ele.push_back(sc->idx(i+2));
			ll.push_back(ele);
			i+=3;
		} else if (i+1<nsc && !strcmp(sc->idx(i).c_str(),"nameidx")) {
			npidx=atoi(sc->idx(i+1).c_str());
			i+=2;
		} else {
			errorexit("unknown logon cfg command '%s' (2)",sc->idx(i).c_str());
		}
	}
	delete sc;
	S32 ls=ll.size();
	if (npidx>=ls)
		errorexit("bad logon cfg name idx");
// see if in the list
	for (i=0;i<ls;++i) {
		if (!strcmp(un,ll[i][0].c_str()))
			break;
	}
	if (i==ls) { // add 
		vector<string> ele;
		ele.push_back(un);
		ele.push_back(pw);
		ll.push_back(ele);
		::sort(ll.begin(),ll.end());
		for (i=0;i<ls;++i) {
			if (!strcmp(un,ll[i][0].c_str())) {
				break;
			}
		}
		npidx=i;
	} else { // replace password
		npidx=i;
		ll[i][1]=pw;
	}
	ls=ll.size();
	FILE *fp;
	fp=fopen2("logoncfg.txt","wb");
	for (i=0;i<ls;i++) {
		fprintf(fp,"name \"%s\" \"%s\"\n",ll[i][0].c_str(),ll[i][1].c_str());
	}
	fprintf(fp,"nameidx %d\n",npidx);
	fclose(fp);
}

void match_registerinit()
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
	rl = res_loadfile("registerres.txt");
//	LISTBOX1=rl->find<listbox>("LISTBOX1");
//	LISTBOXPW=rl->find<listbox>("LISTBOXPW");
//	LISTBOXNP=rl->find<listboxn>("LISTBOXNP");
	EDIT1=rl->find<edit>("EDIT1");
	EDITPW=rl->find<edit>("EDITPW");
//	PBUT3=rl->find<pbut>("PBUT3");
	PBUT1=rl->find<pbut>("PBUT1");
	PBUTPICKSERVER=rl->find<pbut>("PBUTPICKSERVER");
	PBUT6=rl->find<pbut>("PBUT6");
//	sc=new script("logoncfg.txt");
//	nsc=sc->num();
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
/*	vector<string> names;
	vector<string> passs;
	S32 npidx=-1;
	for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"name"))
			names.push_back(sc->idx(i+1));
		else if (!strcmp(sc->idx(i).c_str(),"pass"))
			passs.push_back(sc->idx(i+1));
		else if (!strcmp(sc->idx(i).c_str(),"nameidx")) {
			npidx=atoi(sc->idx(i+1).c_str());
		} else
			errorexit("unknown logon cfg command '%s'",sc->idx(i).c_str());
	}
	delete sc; 
	if (names.size()!=passs.size())
		errorexit("bad logon cfg name/pass size mismatch");
	if (npidx>=(S32)names.size())
		errorexit("bad logon cfg name idx");
	nsc=names.size();
	for (i=0;i<nsc;++i) {
		vector<string> ele;
		ele.push_back(names[i]);
		ele.push_back(passs[i]);
		LISTBOXNP->addstring(ele);
	}
	LISTBOXNP->setidxc(npidx); */
	PBUT1->setvis(0);
//	EDIT1->settname(LISTBOX1->getidxname());
//	EDITPW->settname(LISTBOXPW->getidxname());
/*	if (LISTBOXNP->getnumidx()>0) {
		vector<string> ele=LISTBOXNP->getidxname();
		if (ele.size()!=2)
			errorexit("bad size for columns");
		EDIT1->settname(ele[0].c_str());
		EDITPW->settname(ele[1].c_str());
	} */
	focus=EDIT1;
	rusername[0]='\0';
	rpassword[0]='\0';
}

	
void match_registerproc()
{
	if (KEY==K_ESCAPE) {
		changestate(STATE_MATCHTITLE);
	}
//	int i;
//	if (PBUT1->getvis()==0) {
//		i=LISTBOX1->getidx();
//		i=LISTBOXNP->getidx();
	bool v = logstatus==LOGENTRY && 
		strlen(EDIT1->gettname()) && 
		strlen(EDITPW->gettname()) && 
		strlen(mserverip) && 
		strlen(mserverport);
//			EDIT1->settname(LISTBOX1->getidxname());
	PBUT1->setvis(v);
//		}
//	}
	if (logstatus==LOGENTRY) {
		strcpy(rusername,EDIT1->gettname());
		strcpy(rpassword,EDITPW->gettname());
	}
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
//				vector<string> ele=LISTBOXNP->getidxname();
//				strcpy(rusername,ele[0].c_str());
//				strcpy(password,ele[1].c_str());
			}
		} else if (focusa==PBUT6) {
			keepsock=0;
			changestate(STATE_MATCHTITLE);
		} else if (focusa==PBUTPICKSERVER) {
			keepsock=0;
			pushchangestate(STATE_MATCHPICKSERVER);
//		} else if (focusa==PBUT3) {
//			LISTBOX1->removeidx();
//			LISTBOXPW->removeidx();
//			LISTBOXNP->removeidx();
//		} else if (focusa==LISTBOXNP) {
//			LISTBOX1->setidxc(ret);
//			LISTBOXPW->setidxc(ret);
//			vector<string> ele=LISTBOXNP->getidxname();
//			EDIT1->settname(ele[0].c_str());
//			EDITPW->settname(ele[1].c_str());
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
#if 0
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
#endif
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
// register username passwd, send to server
//			rusername[0]='\0';
			C8 sendstr[512];
			sprintf(sendstr,"register %s %s",rusername,rpassword);
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
			strcpy(username,rusername);
			addlogoncfg(rusername,rpassword);
		} 
		tcpsendwritebuff(matchcl);
	}
}

#define FSX 400
#define FSY 12
#if 0
void match_registerdraw2d()
{
	cliprect32(B32,0,0,WX,WY,C32BLACK);
	rl->draw();
	outtextxyf32(B32,WX/2-25*4,10,C32WHITE,"register to 'TRN' (match server)");
/*	if (LISTBOXNP->getidx()>=0) {
		vector<string> ele=LISTBOXNP->getidxname();
		outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"name: '%s', password '%s'",ele[0].c_str(),ele[1].c_str());
	} else
		outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"pick a username");
*/
	outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"name: '%s', password '%s'",rusername,rpassword);
	if (strlen(mserverip))
		outtextxyf32(B32,WX/2-25*4,40,C32WHITE,"ip: '%s'",mserverip);
	else
		outtextxyf32(B32,WX/2-25*4,40,C32WHITE,"pick a server ip");
	if (strlen(mserverport))
		outtextxyf32(B32,WX/2-25*4,50,C32WHITE,"port '%s'",mserverport);
	else
		outtextxyf32(B32,WX/2-25*4,50,C32WHITE,"pick a server port");
	outtextxyf32(B32,0,0,C32WHITE,"focus = %p",focus);
}
#endif

void match_registerdraw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
//	afont->print(800/2-25*4,30,FSX,FSY,F32WHITE,"name: '%s', password '%s'",ele[0].c_str(),ele[1].c_str());
	afont->print((float)WX/2-25*4,10,FSX,FSY,F32WHITE,"register to 'TRN' (match server)");
/*	if (LISTBOXNP->getidx()>=0) {
		vector<string> ele=LISTBOXNP->getidxname();
		outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"name: '%s', password '%s'",ele[0].c_str(),ele[1].c_str());
	} else
		outtextxyf32(B32,WX/2-25*4,30,C32WHITE,"pick a username");
*/
	afont->print((float)WX/2-25*4,30,FSX,FSY,F32WHITE,"name: '%s', password '%s'",rusername,rpassword);
	if (strlen(mserverip))
		afont->print((float)WX/2-25*4,40,FSX,FSY,F32WHITE,"ip: '%s'",mserverip);
	else
		afont->print((float)WX/2-25*4,40,FSX,FSY,F32WHITE,"pick a server ip");
	if (strlen(mserverport))
		afont->print((float)WX/2-25*4,50,FSX,FSY,F32WHITE,"port '%s'",mserverport);
	else
		afont->print((float)WX/2-25*4,50,FSX,FSY,F32WHITE,"pick a server port");
	afont->print(0,0,FSX,FSY,F32WHITE,"focus = %p",focus);
	video_sprite_end(); 
}

void match_registerexit()
{
//	FILE *fp;
//	int i,nlb;
/*	fp=fopen2("logoncfg.txt","wb");
	nlb=LISTBOXNP->getnumidx(); // number of users
	for (i=0;i<nlb;i++) {
		vector<string> ele=LISTBOXNP->getidxname(i);
		fprintf(fp,"name \"%s\"\n",ele[0].c_str());
	}
//	nlb=LISTBOXPW->getnumidx();
	for (i=0;i<nlb;i++) {
		vector<string> ele=LISTBOXNP->getidxname(i);
		fprintf(fp,"pass \"%s\"\n",ele[1].c_str());
	}
	fprintf(fp,"nameidx %d\n",LISTBOXNP->getidx());
	fclose(fp); */
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
