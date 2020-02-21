#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "match_server.h"
#include "match_lobby.h"
#include "match_logon.h"
#include "match_game1.h"

//struct socker *matchcl;
//char username[256];

static shape* rl;
static shape* focus;
// JOIN (client)
static listbox* LISTBOX1; // clients host list of gameips
static listbox* LISTBOX2; // host gameport
static edit* ECLIP; // edit ip
static edit* ECLPORT; // edit port
static pbut* PDELCLIP; // del ip
static pbut* PDELCLPORT; // del port
static pbut* PJOIN; // join a hosted game, goto 'game1'
// HOST (server)
static listbox* LISTBOX3; // server ips that servers tell their clients
static listbox* LISTBOX4; // server ports
static text* TSVIP; // show current ip
static edit* ESVPORT; // edit port
static pbut* PDELSVPORT; // del port
static pbut* PCREATE; // host a game, goto 'pick game' then options then 'game1'
// quit
static pbut* PQUIT;

enum {LOGENTRY,LOGSEND,LOGWAIT,LOGYES};
//static int logstatus;
//static int logtimer;
static int keepsock;
static int nips,ipidx;


void match_quickgameinit()
{
	int i;
	logger("quickgame init\n");
	video_setupwindow(800,600);
	pushandsetdir("lobbysystem");
//	rl=loadres("quickgameres.txt");
	factory2<shape> fact;
	script* sc=new script("quickgameres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc;
	LISTBOX1=rl->find<listbox>("LISTBOX1");
	LISTBOX2=rl->find<listbox>("LISTBOX2");
	LISTBOX3=rl->find<listbox>("LISTBOX3");
	LISTBOX4=rl->find<listbox>("LISTBOX4");
	PJOIN=rl->find<pbut>("PJOIN");
	PCREATE=rl->find<pbut>("PCREATE");
	TSVIP=rl->find<text>("TSVIP");
	ESVPORT=rl->find<edit>("ESVPORT");
	ECLIP=rl->find<edit>("ECLIP");
	ECLPORT=rl->find<edit>("ECLPORT");
	PQUIT=rl->find<pbut>("PQUIT");
	PDELCLIP=rl->find<pbut>("PDELCLIP");
	PDELCLPORT=rl->find<pbut>("PDELCLPORT");
	PDELSVPORT=rl->find<pbut>("PDELSVPORT");

//	nips=0;
	ipidx=-1;
//	nsc=0;
//	sc=NULL;
	keepsock=0;
//	if (fileexist("quickgamecfg.txt"))
//		sc=loadscript("quickgamecfg.txt",&nsc);
	sc=new script("quickgamecfg.txt");
	S32 nsc=sc->num();
	for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"clip"))
//			addlistboxname(rl,LISTBOX1,-1,sc->idx(i+1).c_str(),-1);
			LISTBOX1->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"clport"))
//			addlistboxname(rl,LISTBOX2,-1,sc->idx(i+1).c_str(),-1);
			LISTBOX2->addstring(sc->idx(i+1).c_str());
//		else if (!strcmp(sc->idx(i).c_str(),"svip"))
//			addlistboxname(rl,LISTBOX3,-1,sc->idx(i+1).c_str(),-1);
		else if (!strcmp(sc->idx(i).c_str(),"svport"))
//			addlistboxname(rl,LISTBOX4,-1,sc->idx(i+1).c_str(),-1);
			LISTBOX4->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"clipidx"))
//			setcurlistbox(rl,LISTBOX1,atoi(sc->idx(i+1).c_str()),-1);
			LISTBOX1->setidxc(atoi(sc->idx(i+1).c_str()));
		else if (!strcmp(sc->idx(i).c_str(),"clportidx"))
//			setcurlistbox(rl,LISTBOX2,atoi(sc->idx(i+1).c_str()),-1);
			LISTBOX2->setidxc(atoi(sc->idx(i+1).c_str()));
		else if (!strcmp(sc->idx(i).c_str(),"svipidx"))
//			setcurlistbox(rl,LISTBOX3,ipidx=atoi(sc->idx(i+1).c_str()),-1);
//			LISTBOX3->setidxc(atoi(sc->idx(i+1).c_str()));
			ipidx=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"svportidx"))
//			setcurlistbox(rl,LISTBOX4,atoi(sc->idx(i+1).c_str()),-1);
			LISTBOX4->setidxc(atoi(sc->idx(i+1).c_str()));
		else
			errorexit("unknown quickgame cfg command '%s'",sc->idx(i).c_str());
	}
//	freescript(sc,nsc);
	delete sc;
//	setresvis(rl,PJOIN,0);
	PJOIN->setvis(0);
//	setresvis(rl,PCREATE,0);
	PCREATE->setvis(0);
	initsocker();
	focus=0;
}
	
void match_quickgameproc()
{
	if (KEY==K_ESCAPE) {
		keepsock=0;
		popstate();
	}
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	int i,k;
//	struct rmessage rm;
	if (!nips) {
		nips=getnmyip();	// returns 0 until we get one
		if (nips>0) {
//			addlistboxname(rl,LISTBOX3,-1,"0.0.0.0",-1);
			LISTBOX3->addstring("0.0.0.0");
//			addlistboxname(rl,LISTBOX3,-1,"127.0.0.1",-1);
			LISTBOX3->addstring("127.0.0.1");
//			addlistboxname(rl,LISTBOX3,-1,"65.12.12.85",-1);
			LISTBOX3->addstring("65.12.12.85");
			for (i=0;i<nips;i++)
//				addlistboxname(rl,LISTBOX3,-1,ip2str(getmyip(i)),-1);
				LISTBOX3->addstring(ip2str(getmyip(i)));
//			setcurlistbox(rl,LISTBOX3,ipidx,-1);
			LISTBOX3->sort(listbox::SK_IPS);
			LISTBOX3->setidxc(ipidx);
		}
	}
//	if (getresvis(rl,PCREATE)==0) {
	if (PCREATE->getvis()==0) {
//		i=getcurlistbox(rl,LISTBOX3);
		i=LISTBOX3->getidx();
//		k=getcurlistbox(rl,LISTBOX4);
		k=LISTBOX4->getidx();
		if (i>=0 && k>=0 ) {
//			setresname(rl,TSVIP,findlistboxname(rl,LISTBOX3,i));
			TSVIP->settname(LISTBOX3->getidxname(i));
//			setresname(rl,ESVPORT,findlistboxname(rl,LISTBOX4,k));
			ESVPORT->settname(LISTBOX4->getidxname(k));
//			setresvis(rl,PCREATE,1);
			PCREATE->setvis(1);
		}
	}
//	if (getresvis(rl,PJOIN)==0) {
	if (PJOIN->getvis()==0) {
//		i=getcurlistbox(rl,LISTBOX1);
		i=LISTBOX1->getidx();
//		k=getcurlistbox(rl,LISTBOX2);
		k=LISTBOX2->getidx();
		if (i>=0 && k>=0 ) {
//			setresname(rl,ECLIP,findlistboxname(rl,LISTBOX1,i));
			ECLIP->settname(LISTBOX1->getidxname(i));
//			setresname(rl,ECLPORT,findlistboxname(rl,LISTBOX2,k));
			ECLPORT->settname(LISTBOX2->getidxname(k));
//			setresvis(rl,PJOIN,1);
			PJOIN->setvis(1);
		}
	}
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
	if (focus) {
		S32 ret=focus->proc();
		shape* focusb = ret ? focus : 0;
//		shape* focush = ret>=0 ? focus : 0;
		shape* focusl = ret>=0 ? focus : 0;
		shape* focuse = ret>=0 ? focus : 0;
//		switch(rm.id) {
//		case PCREATE:
		if (focusb==PCREATE) {
#if 0
//			gametcpsv=tcpmakemaster(str2ip(getresname(rl,TSVIP)),atoi(getresname(rl,ESVPORT)));
			gametcpsv=tcpmakemaster(str2ip(TSVIP->gettname()),atoi(ESVPORT->gettname()));
			if (gametcpsv)
				gameudpsv=udpmake(gametcpsv->ip,gametcpsv->port);
			if (gametcpsv && gameudpsv) {
				keepsock=1;
//				pushandchangestatenum(72);
				pushchangestate(STATE_MATCHPICKGAME);
// attempt to create
//				i=findlistboxidx(rl,LISTBOX3,getresname(rl,TSVIP));	// -1 if not found
#if 0
				i=LISTBOX3->findstring(TSVIP->gettname());
				if (i>=0) {
//					setcurlistbox(rl,LISTBOX3,i,-1);
					LISTBOX3->setidxc(i);
				} else {
//					addlistboxname(rl,LISTBOX3,0,getresname(rl,TSVIP),-1);
					LISTBOX3->addstring(TSVIP->gettname());
//					setcurlistbox(rl,LISTBOX3,0,-1);
					LISTBOX3->setidxc(0);
				}
#endif
			} else {
				if (gametcpsv) {
					freesocker(gametcpsv);
					gametcpsv=NULL;
				}
				if (gameudpsv) {
					freesocker(gameudpsv);
					gameudpsv=NULL;
				}
			}
#else
			cgi.isvalid=true;
			cgi.gameidx=0;	// for now
			cgi.ishost=true;
			cgi.ip=str2ip(TSVIP->gettname());
			cgi.port=atoi(ESVPORT->gettname());
			cgi.nports=1;
			pushchangestate(STATE_MATCHPICKGAME);
			keepsock=1;
#endif
//			break;
// attempt to join
//		case PJOIN:
		} else if (focusb==PJOIN) {
//			i=findlistboxidx(rl,LISTBOX1,getresname(rl,ECLIP));	// -1 if not found
			i=LISTBOX1->findstring(ECLIP->gettname());
			if (i>=0) {
//				setcurlistbox(rl,LISTBOX1,i,-1);
				LISTBOX1->setidxc(i);
			} else {
//				addlistboxname(rl,LISTBOX1,0,getresname(rl,ECLIP),-1);
				LISTBOX1->addstring(ECLIP->gettname());
//				setcurlistbox(rl,LISTBOX1,0,-1);
				LISTBOX1->setidxc(0);
			}
#if 0
//			gametcpcl=tcpgetconnect(str2ip(getresname(rl,ECLIP)),atoi(getresname(rl,ECLPORT)));	
			gametcpcl=tcpgetconnect(str2ip(ECLIP->gettname()),atoi(ECLPORT->gettname()));
			if (gametcpcl)
				gameudpcl=udpmake(gametcpcl->ip,gametcpcl->port);
			if (gametcpcl && gameudpcl) {
				keepsock=1;
//				pushandchangestatenum(78);
				pushchangestate(STATE_MATCHGAME1);
			} else {
				if (gametcpcl) {
					freesocker(gametcpcl);
					gametcpcl=NULL;
				}
			}
#else
			cgi.isvalid=true;
			cgi.gameidx=0;	// for now
			cgi.ishost=false;
			cgi.ip=str2ip(ECLIP->gettname());
			cgi.port=atoi(ECLPORT->gettname());
			cgi.nports=1;
			pushchangestate(STATE_MATCHGAME1);
			keepsock=1;
#endif
//			break;
// quit
//		case PQUIT:
		} else if (focusb==PQUIT) {
			keepsock=0;
			popstate();
//			break;
//		case PDELCLIP:
		} else if (focusb==PDELCLIP) {
//			dellistboxname(rl,LISTBOX1,getcurlistbox(rl,LISTBOX1),-1);
			LISTBOX1->removeidx();
//			break;
//		case PDELCLPORT:
		} else if (focusb==PDELCLPORT) {
//			dellistboxname(rl,LISTBOX2,getcurlistbox(rl,LISTBOX2),-1);
			LISTBOX2->removeidx();
//			break;
//		case PDELSVPORT:
		} else if (focusb==PDELSVPORT) {
//			dellistboxname(rl,LISTBOX4,getcurlistbox(rl,LISTBOX4),-1);
			LISTBOX4->removeidx();
//			break;
//		case LISTBOX1:
		} else if (focusl==LISTBOX1) {
//			setresname(rl,ECLIP,findlistboxname(rl,LISTBOX1,rm.val));
			ECLIP->settname(LISTBOX1->getidxname());
//			break;
//		case LISTBOX2:
		} else if (focusl==LISTBOX2) {
//			setresname(rl,ECLPORT,findlistboxname(rl,LISTBOX2,rm.val));
			ECLPORT->settname(LISTBOX2->getidxname());
//			break;
//		case LISTBOX3:
		} else if (focusl==LISTBOX3) {
//			setresname(rl,TSVIP,findlistboxname(rl,LISTBOX3,rm.val));
			TSVIP->settname(LISTBOX3->getidxname());
//			break;
//		case LISTBOX4:
		} else if (focusl==LISTBOX4) {
//			setresname(rl,ESVPORT,findlistboxname(rl,LISTBOX4,rm.val));
			ESVPORT->settname(LISTBOX4->getidxname());
//			break;
//		case ECLIP:
		} else if (focuse==ECLIP) {
//			i=findlistboxidx(rl,LISTBOX1,getresname(rl,ECLIP));	// -1 if not found
			i=LISTBOX1->findstring(ECLIP->gettname());
			if (i>=0) {
//				setcurlistbox(rl,LISTBOX1,i,-1);
				LISTBOX1->setidxc(i);
			} else {
//				addlistboxname(rl,LISTBOX1,0,getresname(rl,ECLIP),-1);
				LISTBOX1->addstring(ECLIP->gettname());
//				setcurlistbox(rl,LISTBOX1,0,-1);
//				LISTBOX1->setidxc(0);
				LISTBOX1->sort(listbox::SK_IPS);
			}
//			setresname(rl,ECLIP,"");
			ECLIP->settname("");
//			break;			
//		case ECLPORT:
		} else if (focuse==ECLPORT) {
//			i=findlistboxidx(rl,LISTBOX2,getresname(rl,ECLPORT));	// -1 if not found
			i=LISTBOX2->findstring(ECLPORT->gettname());
			if (i>=0) {
//				setcurlistbox(rl,LISTBOX2,i,-1);
				LISTBOX2->setidxc(i);
			} else {
//				addlistboxname(rl,LISTBOX2,0,getresname(rl,ECLPORT),-1);
				LISTBOX2->addstring(ECLPORT->gettname());
//				setcurlistbox(rl,LISTBOX2,0,-1);
//				LISTBOX2->setidxc(0);
				LISTBOX2->sort();
			}
//			setresname(rl,ECLPORT,"");
			ECLPORT->settname("");
//			break;			
//		case ESVPORT:
		} else if (focuse==ESVPORT) {
//			i=findlistboxidx(rl,LISTBOX4,getresname(rl,ESVPORT));	// -1 if not found
			i=LISTBOX4->findstring(ESVPORT->gettname());
			if (i>=0) {
//				setcurlistbox(rl,LISTBOX4,i,-1);
				LISTBOX4->setidxc(i);
			} else {
//				addlistboxname(rl,LISTBOX4,0,getresname(rl,ESVPORT),-1);
				LISTBOX4->addstring(ESVPORT->gettname());
//				setcurlistbox(rl,LISTBOX4,0,-1);
//				LISTBOX4->setidxc(0);
				LISTBOX4->sort();
			}
//			setresname(rl,ESVPORT,"");
			ESVPORT->settname("");
//			break;			
		}
	}
}

void match_quickgamedraw2d()
{
	cliprect32(B32,0,0,WX-1,WY-1,C32BLACK);
	rl->draw();
	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"quickgame");
}

void match_quickgameexit()
{
	nips=0;
	FILE *fp;
	int i,nlb;
	fp=fopen2("quickgamecfg.txt","wb");
	nlb=LISTBOX1->getnumidx();//getnumlistbox(rl,LISTBOX1);
	for (i=0;i<nlb;i++)
		fprintf(fp,"clip \"%s\"\n",LISTBOX1->getidxname(i));//findlistboxname(rl,LISTBOX1,i));
	nlb=LISTBOX2->getnumidx();//getnumlistbox(rl,LISTBOX2);
	for (i=0;i<nlb;i++)
		fprintf(fp,"clport \"%s\"\n",LISTBOX2->getidxname(i));//findlistboxname(rl,LISTBOX2,i));
//	nlb=getnumlistbox(rl,LISTBOX3);
//	for (i=0;i<nlb;i++)
//		fprintf(fp,"svip \"%s\"\n",findlistboxname(rl,LISTBOX3,i));
	nlb=LISTBOX4->getnumidx();//getnumlistbox(rl,LISTBOX4);
	for (i=0;i<nlb;i++)
		fprintf(fp,"svport \"%s\"\n",LISTBOX4->getidxname(i)); //findlistboxname(rl,LISTBOX4,i));
	fprintf(fp,"clipidx %d\n",LISTBOX1->getidx());
	fprintf(fp,"clportidx %d\n",LISTBOX2->getidx());
	fprintf(fp,"svipidx %d\n",LISTBOX3->getidx());
	fprintf(fp,"svportidx %d\n",LISTBOX4->getidx());
	fclose(fp);
	popdir();
//	freeres(rl);
	delete rl;
	if (!keepsock) {
#if 0
		if (gametcpsv) {
			freesocker(gametcpsv);
			gametcpsv=NULL;
		}
#endif
		if (!matchcl)
			uninitsocker();
	}
}
