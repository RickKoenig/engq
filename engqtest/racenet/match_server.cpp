#define RES3D // alternate shape class
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "../u_states.h"

#define MAXCL 256 
#define MAXCLBUFF 1000

#define LOGTIME 5 // seconds to get a logon password from client
#define INACTIVETIME 720 // seconds of not hearing anything from logged on client (12 minutes)

//#include "res.h"
//#include "socker.h"

//#include "st_logon.h"
//#include "data/lobbysystem/serverres.h"

//static struct reslist *rl;
static shape* rl;
static pbut* PBUT1; // start server
static pbut* PBUT2; // stop server
static pbut* PBUT3; // delete port
static pbut* PBUT4; // quit
static pbut* PBUT5; // kick user
static pbut* PBUTERASE; // erase user
static listbox* LISTBOX1; // list of server ip's
static listbox* LISTBOX2; // list of ports
static listbox* LISTBOX3; // send
static listbox* LISTBOX4; // receive
static listboxn* LISTBOXU; // list of client names
static text* TEXT4; // server ip
static edit* EDIT1; // new port number
static edit* EDIT2; // server name
static shape* focus;

static fontq* afont;

static int nips;
static int ipidx;
static struct socker *sv; // master socket

struct gameinfo {
	int tcpgameip,tcpgameport;//,udpgameip,udpgameport;
	char gamename[32];
	C8 gamestatus[32];
};

struct client {
	struct socker *cl;
	char *name;
	int kick;
	int away;
	int loggedon;
	int logtime;
	int inactivetime;
	struct gameinfo gi;
};

struct userprofile {
	string username;
	string password;
};

vector <userprofile> permusers;

enum KICKTYPES {KICK_CLOSED,KICK_BADLOGON,KICK_BUFFOVERFLOW,KICK_BADCMD,KICK_KICK};
/*static char *kickstr[]={
	"closed",
	"badlogin",
	"overflow",
	"bad cmd",
	"kicked",
};*/

static char *noname="noname"; // revamp, use string class !

static struct client clients[MAXCL];
static int numcl;
static char parsebuff[MAXCLBUFF+1]; // for null at end
static char sendbuff[MAXCLBUFF+1];
static int frm;

// replace with string class
static C8* strclone(const C8* orig)
{
	C8* r=new C8[strlen(orig)+1];
	strcpy(r,orig);
	return r;
}

static bool sortusers(const userprofile& a,const userprofile& b)
{
	return a.username < b.username;
}

void match_server_init()
{
	logger("server init\n");
	video_setupwindow(800,600);
	pushandsetdir("fonts");
	afont=new fontq("med");
	afont->setspace(20);
	afont->setfixedwidth(20);
	afont->setscale(.5f);
	popdir();
	pushandsetdir("lobbysystem");
// load permusers
	script* sc=new script("serverusers.txt");
	S32 nsc=sc->num();
	vector<string> mips;
	vector<string> mports;
//	S32 mipidx=-1,mportidx=-1;
	S32 i;
	for (i=0;i<nsc;i+=2) {
		userprofile up;
		up.username=sc->idx(i);
		up.password=sc->idx(i+1);
		permusers.push_back(up);
	}
	sort(permusers.begin(),permusers.end(),sortusers);
	delete sc;
//	rl=loadres("serverres.txt");
	init_res3d(800,600);
	rl = res_loadfile("serverres.txt");
/*	factory2<shape> fact;
	sc=new script("serverres.txt");
	rl=fact.newclass_from_handle(*sc);
	delete sc; */
	PBUT1=rl->find<pbut>("PBUT1");
	PBUT2=rl->find<pbut>("PBUT2");
	PBUT3=rl->find<pbut>("PBUT3");
	PBUT4=rl->find<pbut>("PBUT4");
	PBUT5=rl->find<pbut>("PBUT5");
	PBUTERASE=rl->find<pbut>("PBUTERASE");
	LISTBOX1=rl->find<listbox>("LISTBOX1");
	LISTBOX2=rl->find<listbox>("LISTBOX2");
	LISTBOX3=rl->find<listbox>("LISTBOX3");
	LISTBOX4=rl->find<listbox>("LISTBOX4");
	LISTBOXU=rl->find<listboxn>("LISTBOXU");
	TEXT4=rl->find<text>("TEXT4");
	EDIT1=rl->find<edit>("EDIT1");
	EDIT2=rl->find<edit>("EDIT2");

//	S32 nsc=0;
	ipidx=-1;

//	if (fileexist("servercfg.txt")) {
//		sc=loadscript("servercfg.txt",&nsc);
	sc=new script("servercfg.txt");
	nsc=sc->num();
//	}

//		addlistboxname(rl,LISTALTWEAP,-1,weapinfos[i].name,-1);
//		LISTALTWEAP->addstring(weapinfos[i].name);

		for (i=0;i<nsc;i+=2) {
		if (!strcmp(sc->idx(i).c_str(),"ip"))
			;//addlistboxname(rl,LISTBOX1,-1,sc->idx(i+1).c_str(),-1);
		else if (!strcmp(sc->idx(i).c_str(),"port"))
//			addlistboxname(rl,LISTBOX2,-1,sc->idx(i+1).c_str(),-1);
			LISTBOX2->addstring(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"ipidx"))
			ipidx=atoi(sc->idx(i+1).c_str());
		else if (!strcmp(sc->idx(i).c_str(),"portidx"))
//			setcurlistbox(rl,LISTBOX2,atoi(sc->idx(i+1).c_str()),-1);
			LISTBOX2->setidxc(atoi(sc->idx(i+1).c_str()));
		else if (!strcmp(sc->idx(i).c_str(),"servername"))
//			setresname(rl,EDIT2,sc->idx(i+1).c_str());
			EDIT2->settname(sc->idx(i+1).c_str());
		else
			errorexit("unknown server cfg command '%s'",sc->idx(i).c_str());
	}
//	freescript(sc,nsc);
	delete sc;
//	setresvis(rl,PBUT1,0);
//	setresvis(rl,PBUT2,0);
	PBUT1->setvis(0);
	PBUT2->setvis(0);
	nips=0;
	sv=NULL;
	initsocker();
	focus=0;
	if (LISTBOX2->getidx()>=0)
		EDIT1->settname(LISTBOX2->getidxname());
	LISTBOX3->setmax(1000);
	LISTBOX3->addstring("Press 'start' to start server.");
	LISTBOX4->setmax(1000);
// build user list
	for (i=0;i<(S32)permusers.size();++i) {
		vector<string> ele;
		ele.push_back(permusers[i].username);
		ele.push_back(" "); // server init
		LISTBOXU->addstring(ele);
	}
 //LISTBOX5->sort();
// customize users columns
	vector<S32> colx;
	colx.push_back(0);
	colx.push_back(100);
	LISTBOXU->setcolumnsx(colx);
}
	
void match_server_proc()
{
	if (KEY==K_ESCAPE) {
//		poporchangestate(STATE_MAINMENU);
		popstate();
	}
	int i,j;//,k;//el;
	char str[256];
//	struct rmessage rm;
	struct socker *cl;
	char *tok;
	int len;
	int stat;
//	char tempname[256];
// get ip(s)
	if (!nips) {
		nips=getnmyip();	// returns 0 until we get one
		if (nips>0) {
			LISTBOX1->addstring("0.0.0.0");
			LISTBOX1->addstring("127.0.0.1");
			for (i=0;i<nips;i++)
				LISTBOX1->addstring(ip2str(getmyip(i)));
			if (LISTBOX1->getidx()>=0)
				TEXT4->settname(LISTBOX1->getidxname());
			LISTBOX1->sort(listbox::SK_IPS);
			LISTBOX1->setidxc(ipidx);
		}
	}
// if have an ip, then enable start server button
	if (!sv && PBUT1->getvis()==0) {
		i=LISTBOX1->getidx();
		if (i>=0) {
			TEXT4->settname(LISTBOX1->getidxname(i));
			i=LISTBOX2->getidx();
			if (i>=0) {
				PBUT1->setvis(1);
				EDIT1->settname(LISTBOX2->getidxname(i));
			}
		}
	}
// start UI
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
//		switch(rm.id) {
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus();
		shape* focusa = ret>=0 ? focus : 0;
//		shape* focusb = ret ? focus : 0;
//		shape* focush = ret>=0 ? focus : 0;
//		shape* focusl = ret>=0 ? focus : 0;
//		shape* focuse = ret>=0 ? focus : 0;
// start server
		if (focusa==PBUT1) {
	//		case PBUT1:
			sv=tcpmakemaster(str2ip(TEXT4->gettname()),atoi(EDIT1->gettname()));
			if (sv) {
				PBUT1->setvis(0);
				PBUT2->setvis(1);
				sprintf(str,"listen on %s:%d",TEXT4->gettname(),atoi(EDIT1->gettname()));
			} else
				sprintf(str,"can't be a server");
//			dofifolist(rl,str,100,LISTBOX3,SCL1);
			LISTBOX3->addstring(str); // output a message
	//		break;
// stop server, killing all client connections
		} else if (focusa==PBUT2) {
	//		case PBUT2:
				PBUT1->setvis(1);
				PBUT2->setvis(0);
			for (i=0;i<numcl;i++) {
				if (clients[i].name!=noname)
					delete[] clients[i].name;
				freesocker(clients[i].cl);
			}
			numcl=0;
			freesocker(sv);
			sv=NULL;
//			clearlistbox(rl,LISTBOX3,SCL1);
//			clearlistbox(rl,LISTBOX4,SCL2);
//			clearlistbox(rl,LISTBOX5,SCL3);
			LISTBOX3->addstring("Server stopped.");
			LISTBOXU->setcol(1," "); // server killed
	//		break;
// remove port
		} else if (focusa==PBUT3) {
	//		case PBUT3:
//			dellistboxname(rl,LISTBOX2,getcurlistbox(rl,LISTBOX2),-1);
			LISTBOX2->removeidx();
	//		break;
// quit
		} else if (focusa==PBUT4) {
	//		case PBUT4:
//			changestatefunc(st_logoninit);
//			changestate(STATE_MATCHLOGON);
//			poporchangestate(STATE_MAINMENU);
			popstate();
	//		break;
// kick selected user
		} else if (focusa==PBUT5) {
	//		case PBUT5:
//			j=getcurlistbox(rl,LISTBOX5);
			j=LISTBOXU->getidx();
			if (j>=0) {
				for (i=0;i<numcl;i++) {
					vector<string> ele=LISTBOXU->getidxname(j);
					if (!strcmp(clients[i].name,ele[0].c_str())) {
						clients[i].kick=KICK_KICK;
						break;
					}
				}
			}
	//		break;
// remove selected user
		} else if (focusa==PBUTERASE) {
	//		case PBUT5:
//			j=getcurlistbox(rl,LISTBOX5);
			j=LISTBOXU->getidx();
			if (j>=0) {
				vector<string> ele=LISTBOXU->getidxname(j);
				for (i=0;i<numcl;i++) {
					if (!strcmp(clients[i].name,ele[0].c_str())) {
						clients[i].kick=KICK_KICK;
						break;
					}
				}
// remove from list
				LISTBOXU->removeidx(j);
				S32 k;
// and remove from permusers
				for (k=0;k<(S32)permusers.size();++k) {
					if (permusers[k].username==ele[0])
						permusers.erase(permusers.begin()+k);
				}
			}
	//		break;
// select ip
		} else if (focusa==LISTBOX1) {
	//		case LISTBOX1:
			TEXT4->settname(LISTBOX1->getidxname());
	//		break;
// select port
		} else if (focusa==LISTBOX2) {
	//		case LISTBOX2:
//			setresname(rl,EDIT1,findlistboxname(rl,LISTBOX2,rm.val));
			EDIT1->settname(LISTBOX2->getidxname());
	//		break;
// add port
		} else if (focusa==EDIT1) {
	//		case EDIT1:
//			i=findlistboxidx(rl,LISTBOX2,getresname(rl,EDIT1));	// -1 if not found
			i=LISTBOX2->findstring(EDIT1->gettname());
			if (i>=0) {
//				setcurlistbox(rl,LISTBOX2,i,-1);
				LISTBOX2->setidxc(i);
			} else {
//				addlistboxname(rl,LISTBOX2,0,getresname(rl,EDIT1),-1);
				LISTBOX2->addstring(EDIT1->gettname());
				LISTBOX2->sort(listbox::SK_INTVAL);
//				setcurlistbox(rl,LISTBOX2,0,-1);
				LISTBOX2->setidxc(-1);
			}
//			setresname(rl,EDIT1,"");
			EDIT1->settname("");
	//		break;	
		} /* else if
			case SCL1:
				setlistboxoffset(rl,LISTBOX3,rm.val);
				break;
			case SCL2:
				setlistboxoffset(rl,LISTBOX4,rm.val);
				break;
			case SCL3:
				setlistboxoffset(rl,LISTBOX5,rm.val);
				break;
			}
		} */
	}
// end UI
// log out old clients
	for (i=0;i<numcl;i++)
		if ((clients[i].cl->flags & SF_CLOSED) || clients[i].kick) {
//			con16_printf(constat,"### logoff '%s' : '%s'\n",clients[i].name,kickstr[clients[i].kick]);
			if (clients[i].kick) {
				sprintf(str,"%s:kicked",clients[i].name);
//				dofifolist(rl,str,100,LISTBOX3,SCL1);
				LISTBOX3->addstring(str);
			} else {
				sprintf(str,"%s:closed",clients[i].name);
//				dofifolist(rl,str,100,LISTBOX4,SCL2);
				LISTBOX4->addstring(str);
			}
			freesocker(clients[i].cl);
//			tempname[0]='\0';
//			const C8* tempname=clients[i].name;
			if (clients[i].name!=noname) {
//				strcpy(tempname,clients[i].name);
//				j=findlistboxidx(rl,LISTBOX5,tempname);
				j=LISTBOXU->findstring(clients[i].name);
				if (j>=0) {
//					dellistboxname(rl,LISTBOX5,j,SCL3);
//					LISTBOXU->removeidx(j);
					vector<string> ele;
					ele.push_back(clients[i].name);
					ele.push_back(" "); // logged off
					LISTBOXU->changestring(ele,j);
				}
				strcpy(sendbuff,"loggedoff ");
				strcat(sendbuff,clients[i].name);
				len=strlen(sendbuff);
				for (j=0;j<numcl;j++) {
					if (!clients[j].away) {
						sprintf(str,"%s:%s",clients[j].name,sendbuff);
//						dofifolist(rl,str,100,LISTBOX3,SCL1);
						LISTBOX3->addstring(str);
						tcpwritetoken32(clients[j].cl,0,sendbuff,len);
					}
				}
				delete[] clients[i].name;
			}
//			memfree(clients[i].clbuff);
			numcl--;
			clients[i]=clients[numcl];
			i--;
		}
// log in new clients
	if (numcl<MAXCL && !(frm&7)) {
//		con16_printf(constat,"check accept\n");
		cl=tcpgetaccept(sv);
		if (cl) {
			memset(&clients[numcl],0,sizeof(struct client));
//			clients[i].clbuff=memzalloc(MAXCLBUFF); 
			clients[numcl].cl=cl;
			clients[numcl].name=noname;
			clients[numcl].away=1;
			clients[i].logtime=getmillisec(); // keep clients[i]
			clients[i].inactivetime=getmillisec(); // keep clients[i]
//			con16_printf(constat,"client connect\n");
			sprintf(str,"%s:%d : connect"
				,ip2str(clients[numcl].cl->remoteip),clients[numcl].cl->remoteport);
//			dofifolist(rl,str,100,LISTBOX4,SCL2);
			LISTBOX4->addstring(str);
			numcl++;
		}
	}
// see what clients are saying
	for (i=0;i<numcl;i++) {
		tcpfillreadbuff(clients[i].cl); // get from net
		while (1) {
			stat=tcpreadtoken32(clients[i].cl,0,parsebuff,MAXCLBUFF);
			if (stat==-1) { // error in token, kick user
				clients[i].kick=KICK_BUFFOVERFLOW;
				break;
			}
			if (stat==0) // token not ready yet
				break;
			clients[i].inactivetime=getmillisec(); // keep clients[i]
			parsebuff[stat]='\0'; // just in case, sentinel
			sprintf(str,"%s:%s",clients[i].name,parsebuff); // so client->server message
			LISTBOX4->addstring(str);
			tok=strtok(parsebuff," "); // bust message up into tokens
			if (!tok) tok=""; // safety
// logon username passwd
			bool ISLOGON = !strcmp(tok,"logon");
			bool ISREGISTER = !strcmp(tok,"register");
			if (ISLOGON || ISREGISTER) {
				if (clients[i].loggedon) {
					clients[i].kick=KICK_BADLOGON; // can only logon once!
					break;
				} else { // not yet logged in
					C8* lsusername=strtok(NULL," ");
					if (!lsusername) { // no logon name
						clients[i].kick=KICK_BADLOGON;
						break;
					} else {
						C8* lspassword=strtok(NULL," "); // password token not sent, assume ""
						if (!lspassword) {
							lspassword="";
						}
// logon username password: see if already logged on
						for (j=0;j<numcl;j++) {
							if (!strcmp(lsusername,clients[j].name)) {
								clients[i].kick=KICK_BADLOGON; // name already logged on
								break;
							}
						}
// now see if matches(logon) username password in permusers file, or not matches(register)
						S32 npu=permusers.size();
						bool CANCONNECT=false;
						if (ISLOGON) {
							S32 k;
							for (k=0;k<npu;++k) {
								if (!strcmp(permusers[k].username.c_str(),lsusername) && 
									!strcmp(permusers[k].password.c_str(),lspassword)) {
									CANCONNECT=true; // user/pass match
									break;
								}
							}
						} else { // ISREGISTER
							S32 k;
							for (k=0;k<npu;++k) {
								if (!strcmp(permusers[k].username.c_str(),lsusername)) {
									break;
								}
							}
							if (k==npu) // can't find this perm user, registration can proceed
								CANCONNECT=true;
						}
						if (j==numcl && CANCONNECT) { // logging in successfully, not yet loggedon and in the permlist
// add/change userlist
							vector<string> ele; // put an 'X' by the username
							ele.push_back(lsusername);
							if (ISLOGON) {
								ele.push_back("X");
								S32 idx=LISTBOXU->findstring(lsusername);
								if (idx<0)
									errorexit("bad lsusername");
								LISTBOXU->changestring(ele,idx); 
							} else { // ISREGISTER
								ele.push_back("R");
								userprofile up; up.username=lsusername; up.password=lspassword;
								permusers.push_back(up);
								LISTBOXU->addstringstay(ele);
							}
							clients[i].loggedon=1;
							clients[i].name=strclone(lsusername); // only logged on players have a name

							strcpy(sendbuff,"logged yes"); // send message back to client, logon successful
							len=strlen(sendbuff);
							tcpwritetoken32(clients[i].cl,0,sendbuff,len);
							sprintf(str,"%s:%s",clients[i].name,sendbuff);
//							dofifolist(rl,str,100,LISTBOX3,SCL1);
							LISTBOX3->addstring(str);

							strcpy(sendbuff,"loggedon "); // tell other users not away, add new user to list
							strcat(sendbuff,lsusername);
							len=strlen(sendbuff);
						// tell everyone else new client logged on
							for (j=0;j<numcl;j++) {
								if (i!=j) {
									if (!clients[j].away) {
										tcpwritetoken32(clients[j].cl,0,sendbuff,len);
										sprintf(str,"%s:%s",clients[j].name,sendbuff);
//											dofifolist(rl,str,100,LISTBOX3,SCL1);
										LISTBOX3->addstring(str);
									}
								}
							}
						}
					}
				}
// say something,
			} else if (!strcmp(tok,"say")) {
				if (!clients[i].loggedon) {
					clients[i].kick=KICK_BADLOGON;
					break;
				} else {
					tok=strtok(NULL,"");
					if (tok) {
//						con16_printf(constat,"'%s' : '%s'\n",clients[i].name,tok);
						strcpy(sendbuff,"said ");
						strcat(sendbuff,clients[i].name);
						strcat(sendbuff," ");
						strcat(sendbuff,tok);
						len=strlen(sendbuff);
						for (j=0;j<numcl;j++) {
							if (clients[j].name!=noname) {
								if (!clients[j].away) {
									tcpwritetoken32(clients[j].cl,0,sendbuff,len);
									sprintf(str,"%s:%s",clients[j].name,sendbuff);
//									dofifolist(rl,str,100,LISTBOX3,SCL1);
									LISTBOX3->addstring(str);
								}
							}
						}
					}
				}
// away
			} else if (!strcmp(tok,"away")) {
				if (!clients[i].loggedon) {
					clients[i].kick=KICK_BADLOGON;
					break;
				} else {
					clients[i].away=1;
					strcpy(sendbuff,"said ");
					strcat(sendbuff,clients[i].name);
					strcat(sendbuff," away");
					len=strlen(sendbuff);
					for (j=0;j<numcl;j++) {
						if (clients[j].name!=noname) {
							if (!clients[j].away) {
								tcpwritetoken32(clients[j].cl,0,sendbuff,len);
								sprintf(str,"%s:%s",clients[j].name,sendbuff);
//								dofifolist(rl,str,100,LISTBOX3,SCL1);
								LISTBOX3->addstring(str);
							}
						}
					}
				}
// gamepicked gamename gamestat ip port
// gamepicked
			} else if (!strcmp(tok,"gamepicked")) {
				if (!clients[i].loggedon) {
					clients[i].kick=KICK_BADLOGON;
					break;
				} else {
					memset(&clients[i].gi,0,sizeof(clients[i].gi));
					tok=strtok(NULL," "); // get game name
					if (tok) { 
						mystrncpy(clients[i].gi.gamename,tok,32);
						tok=strtok(NULL," "); // get gamestatus
						if (tok) {
							mystrncpy(clients[i].gi.gamestatus,tok,32);
							tok=strtok(NULL," "); // get tcp ip
							if (tok) {
								clients[i].gi.tcpgameip=str2ip(tok);
								tok=strtok(NULL," "); // get tcp port
								if (tok) {
									clients[i].gi.tcpgameport=atoi(tok);
/*									tok=strtok(NULL," "); // get udp ip
									if (tok) {
										clients[i].gi.udpgameip=str2ip(tok);
										tok=strtok(NULL," "); // get udp port
										if (tok)
											clients[i].gi.udpgameport=str2ip(tok);
									} */
								}
							}
						}
					}
					strcpy(sendbuff,"gameinfo ");
					strcat(sendbuff,clients[i].name);
					if (clients[i].gi.gamename[0]) {
						sprintf(str," %s %s %s %d",
							clients[i].gi.gamename,
							clients[i].gi.gamestatus,
							ip2str(clients[i].gi.tcpgameip),
							clients[i].gi.tcpgameport);
						strcat(sendbuff,str);
					}
					len=strlen(sendbuff);
					for (j=0;j<numcl;j++) {
						if (clients[j].name!=noname) {
							if (!clients[j].away) {
								tcpwritetoken32(clients[j].cl,0,sendbuff,len);
								sprintf(str,"%s:%s",clients[j].name,sendbuff);
//								dofifolist(rl,str,100,LISTBOX3,SCL1);
								LISTBOX3->addstring(str);
							}
						}
					}
				}
// back
			} else if (!strcmp(tok,"back")) {
				if (!clients[i].loggedon) {
					clients[i].kick=KICK_BADLOGON;
					break;
				} else {
					clients[i].away=0;
				// tell everyone you're back
					strcpy(sendbuff,"said ");
					strcat(sendbuff,clients[i].name);
					strcat(sendbuff," back");
					len=strlen(sendbuff);
					for (j=0;j<numcl;j++) {
						if (clients[j].name!=noname) {
							if (!clients[j].away) {
								tcpwritetoken32(clients[j].cl,0,sendbuff,len);
								sprintf(str,"%s:%s",clients[j].name,sendbuff);
//								dofifolist(rl,str,100,LISTBOX3,SCL1);
								LISTBOX3->addstring(str);
							}
						}
					}
				// send a server welcome
//					sprintf(sendbuff,"said %s welcome",getresname(rl,EDIT2));
					sprintf(sendbuff,"said %s welcome",EDIT2->gettname());
					len=strlen(sendbuff);
					tcpwritetoken32(clients[i].cl,0,sendbuff,len);
					sprintf(str,"%s:%s",clients[i].name,sendbuff);
//					dofifolist(rl,str,100,LISTBOX3,SCL1);
					LISTBOX3->addstring(str);

				// get you the current name list
					for (j=0;j<numcl;j++) {
						if (i!=j && clients[j].name!=noname) {
							strcpy(sendbuff,"loggedon ");
							strcat(sendbuff,clients[j].name);
							len=strlen(sendbuff);
							tcpwritetoken32(clients[i].cl,0,sendbuff,len);
							sprintf(str,"%s:%s",clients[i].name,sendbuff);
//							dofifolist(rl,str,100,LISTBOX3,SCL1);
							LISTBOX3->addstring(str);
							if (clients[j].gi.gamename[0]) {
								sprintf(sendbuff,"gameinfo %s %s %s %s %d",
									clients[j].name,clients[j].gi.gamename,clients[j].gi.gamestatus,
										ip2str(clients[j].gi.tcpgameip),
										clients[j].gi.tcpgameport);
								len=strlen(sendbuff);
								tcpwritetoken32(clients[i].cl,0,sendbuff,len);
								sprintf(str,"%s:%s",clients[i].name,sendbuff);
//								dofifolist(rl,str,100,LISTBOX3,SCL1);
								LISTBOX3->addstring(str);
							}
						}
					}
				}
// unknown
			} else {
				clients[i].kick=KICK_BADCMD;
				break;
			}
		}
		if (!clients[i].loggedon) { // client better send a logon command real soon...
//			clients[i].logtime++;
			if (getmillisec()-clients[i].logtime>(LOGTIME*1000)) {
				clients[i].kick=KICK_BADLOGON;
//				break;
			}
		} else {
//			clients[i].inactivetime++;
			if (getmillisec()-clients[i].inactivetime>(INACTIVETIME*1000)) {
				clients[i].kick=KICK_BADLOGON;
//				break;
			} 
		}
	}
	for (i=0;i<numcl;i++) // send to net
		tcpsendwritebuff(clients[i].cl);
	frm++;
}

#define FSX 400
#define FSY 12
#if 0
void match_server_draw2d()
{
//	video_lock();
	rl->draw();//drawres(rl);
	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"server");
//	outtextxyf16(B16,10,580,hiwhite,"lobby server, numclients %d",numcl);
//	for (i=0;i<numcl;i++)
//		outtextxyf16(B16,360,8+(i<<3),hiwhite,clients[i].name);
//	clipblit16(constat->b16,B16,0,0,20,20,constat->b16->x,constat->b16->y);
//	video_unlock();
}
#endif

void match_server_draw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	afont->print((float)WX/2-5*4,10,FSX,FSY,F32WHITE,"server");
	video_sprite_end(); 
}

void match_server_exit()
{
	FILE *fp;
	int i,nlb;
	fp=fopen2("servercfg.txt","wb");
	nlb=LISTBOX1->getnumidx();
	for (i=0;i<nlb;i++)
		fprintf(fp,"ip \"%s\"\n",LISTBOX1->getidxname(i));
	nlb=LISTBOX2->getnumidx();
	for (i=0;i<nlb;i++)
		fprintf(fp,"port \"%s\"\n",LISTBOX2->getidxname(i));
	fprintf(fp,"ipidx %d\n",LISTBOX1->getidx());
	fprintf(fp,"portidx %d\n",LISTBOX2->getidx());
	fprintf(fp,"servername \"%s\"\n",EDIT2->gettname());
	fclose(fp);
//	freeres(rl);
	delete rl;
	rl=0;
	for (i=0;i<numcl;i++) {
		if (clients[i].name!=noname)
			delete[] clients[i].name;
		freesocker(clients[i].cl);
	}
	numcl=0;
	if (sv)
		freesocker(sv);
	sv=NULL;
	uninitsocker();
	fp=fopen2("serverusers.txt","wb");
	for (i=0;i<(S32)permusers.size();++i)
		fprintf(fp,"%s %s\n",permusers[i].username.c_str(),permusers[i].password.c_str());
	fclose(fp);
	permusers.clear();
	popdir();
	delete afont;
	afont=0;
	exit_res3d();
}
