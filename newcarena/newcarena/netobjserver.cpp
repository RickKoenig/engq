// a class to handle all networking aspects of any network game, hiding socker layer
#include <m_eng.h>
#include <l_misclibm.h>

#include "utils/json.h"
#include "netdescj.h"
#include "hash.h"
#include "constructor.h"
#include "timewarp.h"
#include "envj.h"
#include "enums.h"
#include "carclass.h"
#include "avaj.h"
#include "gamedescj.h"
#include "gamerunj.h"
#include "netobj.h"
#include "connectpackets.h"
#include "netobjserver.h"
#include "utils/json_packet.h"

#include "match_logon.h"

#define SERVERTIMEOUT 45 // all timeouts are in seconds
#define LOGONTIMEOUT 5
#define AFTERLOADTIMEOUT 120
#define GAMETIMEOUT 30
// 7 different senarios
// 1) x botgame											play a bot game

// 2) x client normal									play a client game
// 3) x client timeout									play a bot game

// 4) x server normal									play a server game
// 5) x server timeout, some players connected			play a server game with some extra bots
// 6) x server timeout, no players connected			play a bot game

// 7) x escape or GAME_NONE								clean up and return to prev state

// netserver
netserverobj::netserverobj(gamedescj* gd) :
	netobj(gd),myip(0),live(false),curclients(1),curclientsloggedon(1),curclientschanged(true),master(0),
	wrotego(false)
{
/*	grj = new gamerunj();
	grj->n=cgd->n;
	grj->n.gamenet=GAMENET_SERVER;
	grj->e=cgd->e;
	grj->yourid=0; // your computer id
	grj->yourcntl=CNTL_HUMAN;
	grj->viewslot=0;
	grj->numplayers=cgd->nhumanplayers+cgd->nbotplayers;
// copy over car 0, make remaining bot cars
	grj->caros.resize(grj->numplayers);
	grj->caros[0]=cgd->a;
	grj->caros[0].ownerid=0;
	for (S32 i=1;i<grj->numplayers;++i) { // beyond the clients
		grj->caros[i].randombot(); // lie the extra bots
	} */
	grj->e.buildtrackhash();
	initsocker();
	S32 nclients=cgd->nhumanplayers;
	clients.resize(nclients,clnt());
	wininfo.runinbackground=1;
	starttime=getmillisec();
	servertimeout=starttime+SERVERTIMEOUT*1000;
}

string netserverobj::proc()
{
	S32 i,n=clients.size();
	stringstream retss;
	S32 curtime=getmillisec();
// all the server prelims ... connections, closures, timeouts
// wait for initsock reply / wait for my ip
	if (!myip) {
		if (getnmyip()) {
			myip=getmyip(0);
			string ipstr;
			const C8* ipst=ip2str(myip);
			if (matchcl && cgi.isvalid) {
				retss << "host " << grj->caros[0].playername << " ";
			}
			retss << "myip " << string(ipst) << ", tick " << curtime - starttime << "\n";
		}
	}
// i have an ip, make master socket
	if (myip) { // got an ip, try to make some connections etc..
		if (!live) {
			if (matchcl && cgi.isvalid) {
				master=makecgisock(0);
			} else {
				master=tcpmakemaster(0,grj->n.serverport); // MAKEMASTER, become a server
			}
			if (master) {
				live = true; // start with connections, never gets set back to false, red (onair) light is on
				retss << "master created " << ", tick " << curtime - starttime << ", port " << master->port << "\n";
			} else {
				retss << "can't create master " << ", tick " << curtime - starttime << "\n";
			}
		}
	}
// handle new connections 'master'
	if (master) {
		socker* cl=tcpgetaccept(master); // GETACCEPT, login
		if (cl) { // look at number of current player connections..
			retss << "server: at " << curtime - starttime << ", got a connect\n";
			S32 n=clients.size();
			for (i=1;i<n;++i) { // find empty slot
				if (!clients[i].s) // just connect with anybody (firewall)
					break;
			}
			if (i!=n) { // room for more
				clients[i].s=cl; // place in slot
				clients[i].timeout=curtime+LOGONTIMEOUT*1000;
				retss << "server: at " << curtime - starttime << " connect " << i << "\n";
				welcomej wj;
				wj.yourid=i;
				wj.numplayers=grj->numplayers;
				wj.e=grj->e;
				envj::setpcnet();
				json js=wj.save();
				writejsontoken(clients[i].s,PACK_JSON_WELCOME,js); // write out a json  to socket
#ifdef MATHTEST
//				float pi = 3.14f;
//				tcpwritetoken32(clients[i].s,PACK_MATHTEST,(const C8*)&pi,sizeof(pi));
#endif
				++curclients;
			} else { // server full
				retss << "server: at " << curtime - starttime << ", connect too many\n";
				freesocker(cl); // CLOSE
			}
		}
	}
// close down bad connections
	for (i=1;i<n;++i) {
		if (clients[i].s) {
			bool gameclientdied = false;
			if (clients[i].s->flags&SF_CLOSED) {
				freesocker(clients[i].s); // CLOSE closed
				if (clients[i].state == 1) { // loggedon loggedoff
					--curclientsloggedon;
					curclientschanged = true;
				}
				clients[i]=clnt();
				if (letsgo)
					gameclientdied = true;
				retss << "server: at " << curtime - starttime << ", player " << i << " left\n";
				--curclients;
#ifndef MATHTEST
			} else if (curtime>=clients[i].timeout) {
				freesocker(clients[i].s); // timeout closed
				if (clients[i].state == 1) { // loggedon loggedoff
					--curclientsloggedon;
					curclientschanged = true;
				}
				clients[i]=clnt();
				if (letsgo)
					gameclientdied = true;
				retss << "server: at " << curtime - starttime << ", player " << i << " timeout\n";
				--curclients;
#endif
			}
			if (gameclientdied) {
				keyfrom kf;
				keys botkey;
				botkey.kwentbot = true;
				kf.from = i; // from
				kf.k = botkey.getpack();
				keyques[kf.from].push_back(kf.k);
				retss << "server: player "<< i << " went bot at clock " << keyques[kf.from].size()+1 << "\n";
				S32 j;
				for (j=1;j<n;++j) { // to
					if (clients[j].s && j!=i) {
						tcpwritetoken32(clients[j].s,PACK_KEYF,(const C8*)&kf,sizeof(keyfrom));
					} 
				}
			}
		}
	}
// see if server out of time
#ifndef MATHTEST // test transcendental functions across net
	if (!letsgo && curtime>=servertimeout && !wrotego) {
// senario 5, senario 6   none or some not all players connected, partial/no net play
		retss << "server: didn't get all connections at " << curtime - starttime << " ... " << curclients << "/" << n << "\n";
		servertimeout = curtime + SERVERTIMEOUT*1000; // reset timeout
		retss << "server: reset endtime to " << servertimeout - starttime << "\n";
		for (i=1;i<n;++i) {
			if (clients[i].state == 0) { // player not logged on
				freesocker(clients[i].s);
				clients[i].s=0;
				grj->caros[i].ownerid=-1;
				retss << "server: set lagging slot " << i << " to a bot\n";
				++curclients;
				++curclientsloggedon; // pretend to log on
				curclientschanged = true;
			}
		}
	}
#endif
// end of all the server prelims...

// we've got sockets to read/write
	if (live) {
// read NET
		for (i=1;i<n;++i) {
			tcpfillreadbuff(clients[i].s); // NET --> READBUFF
		}
// process packets
		for (i=1;i<n;++i) {
			if (clients[i].s) { // from
				S32 id;
				const int MAXTOKBUFF=100000;
				C8 tokbuff[MAXTOKBUFF];
				while (true) {
					S32 nr=tcpreadtoken32(clients[i].s,&id,tokbuff,MAXTOKBUFF); // READ something
					if (nr<=0)
						break;
					if (letsgo) { // ingame packets
						switch(id) {
						case PACK_KEY:
							{
								clients[i].timeout = curtime + GAMETIMEOUT*1000;
								keyfrom kf;
								kf.from = i; // from
								kf.k = (U8)tokbuff[0];
								keyques[kf.from].push_back(kf.k);
								S32 j;
								for (j=1;j<n;++j) { // to
									if (clients[j].s && j!=i) {
										tcpwritetoken32(clients[j].s,PACK_KEYF,(const C8*)&kf,sizeof(keyfrom));
									} 
								}
							}
							break;
						case PACK_NONE: // when 'arriving' send keepalive packets until 'gamestart' (waiting for other players to arrive)
							{ // todo: should pass around to clients too.
								clients[i].timeout = curtime + GAMETIMEOUT*1000;
//								logger("got none\n");
							}
							break;
						}
						if (id>=PACK_USER) {
							clients[i].timeout = curtime + GAMETIMEOUT*1000;
							message m;
							m.id = PACK_KIND(id);
							m.data = vector<U8>(tokbuff,tokbuff+nr);
							genques[i].push_back(m);
							vector<U8> sendout;
							sendout.push_back(i); // from
							sendout.insert(sendout.end(),tokbuff,tokbuff+nr);
							S32 j;
							for (j=1;j<n;++j) { // to
								if (clients[j].s && j!=i) {
									tcpwritetoken32(clients[j].s,id+1,(const C8*)&sendout[0],sendout.size()); // +1, add the 'from'
								} 
							}
						}
					} else { // connect packets
						switch(id) {
						case PACK_JSON_HI:
							{
								json js = readjsontoken(tokbuff,nr);
								js.save("sv_test_read_hi.txt"); // test save a text file of this hi packet
								hij h = hij(js);
								grj->caros[i] = h.a;
								grj->caros[i].ownerid = i;
								clients[i].timeout = curtime + AFTERLOADTIMEOUT*1000;
								clients[i].state = 1; // loggedon
								++curclientsloggedon;
								curclientschanged = true;
								retss << "Got a 'hi' packet from " << i << " at " << curtime - starttime << " "
									<< grj->caros[i].playername << "\n";
//								if (true) {
								if (!h.hastrack) {
									js = grj->e.oldtrackj->save();
									writejsontoken(clients[i].s,PACK_JSON_TRACK,js);
									retss << "need track " << grj->e.trackname << "\n";
								}
							}
							break;
#ifdef MATHTEST
						case PACK_MATHTEST:
							{
								if (nr>=4) {
									S32 k = *(S32*) &tokbuff[0];
									float x = 0;
									float y = 0;
									if (nr>=8) 
										x = *(float*) &tokbuff[4];
									if (nr>=12)
										y = *(float*) &tokbuff[8];
									float r = 3.14f;
									switch(k) {
									case MT_ECHO:
										r = x;
										break;
									case MT_DOUBLE:
										r = 2.0f * x;
										break;
									case MT_HALF:
										r = .5f * x;
										break;
									case MT_REC:
										r = 1.0f / x;
										break;
									case MT_SQRT:
										r = sqrtf(x);
										break;
									case MT_SIN:
										r = sinf(x);
										break;
									case MT_COS:
										r = cosf(x);
										break;
									case MT_TAN:
										r = tanf(x);
										break;
									case MT_EXP:
										r = expf(x);
										break;
									case MT_LOG:
										r = logf(x);
										break;
									case MT_ACOS:
										r = racos(x);
										break;
									case MT_ADD:
										r = x + y;
										break;
									case MT_SUB:
										r = x - y;
										break;
									case MT_MUL:
										r = x * y;
										break;
									case MT_DIV:
										r = x / y;
										break;
									case MT_ATAN2:
										r = atan2f(y,x);
										break;
									}
									tcpwritetoken32(clients[i].s,PACK_MATHTEST,(const C8*)&r,sizeof(r));
								}
							}
							break;
#endif
						}
					}
				}
			}
		}
	}
// see if SERVER done with all tasks, switch from connect/logon mode to broadcast/ingame mode
	if (master && (n == curclientsloggedon)) { // all/enough players are logged in
#ifndef MATHTEST // test transcendental functions across net
		if (!wrotego) { // send 'go' packets to all clients
			retss << "server: done.. sending go packets at " << curtime-starttime << "\n";
			servertimeout = curtime + SERVERTIMEOUT*1000; // wait around again just to make sure go packets sent
// send go packets
			json js=json::create_object();
			js.insert_array_object<caroj,caroj>("carojs","caroj",grj->caros); // add a class/struct to json object
			for (i=1;i<n;++i) {
				writejsontoken(clients[i].s,PACK_JSON_GO,js);
			}
			wrotego = true; // done sending everything, will transition when allwritten is true
		}
#endif
	}
// write NET
	bool allwritten = true;
	for (i=0;i<n;++i) {
		if (!tcpsendwritebuff(clients[i].s)) { // WRITEBUFF --> NET, returns true if all data sent
			allwritten = false;
		}
	}
	if (allwritten && wrotego && !letsgo) { // all done sending and got ack, load and play race
		retss << "server: allwritten at " << curtime-starttime << "\n";
		letsgo = true;
		buildkeyques();
		freesocker(master); // no need for server anymore..
		master = 0; // no longer a server, but we're still 'live'
	}
	return retss.str();
}

// ingame: send key to collective
void netserverobj::writekey(const keys& keyp) 
{
	netobj::writekey(keyp);
	S32 i,n=clients.size();
	keyfrom kf;
	kf.from = 0; // SERVER
	kf.k = keyp.getpack();
	for (i=1;i<n;++i) {
		if (clients[i].s) {
			tcpwritetoken32(clients[i].s,PACK_KEYF,(const C8*)&kf,sizeof(keyfrom));
		}
	}
}

netserverobj::~netserverobj()
{
	freesocker(master);
	U32 i;
	for (i=0;i<clients.size();++i) {
		freesocker(clients[i].s);
	}
	wininfo.runinbackground=0;
}


S32 netserverobj::getnumsockets()
{
	return clients.size();
}

S32 netserverobj::getwritebuffsize(S32 slot)
{
	return clients[slot].getwritebuffsize();
}

S32 netserverobj::getwritebuffused(S32 slot)
{
	return clients[slot].getwritebuffused();
}

void netserverobj::writegen(PACK_KIND id,const vector<U8>& mess)
{
	netobj::writegen(id,mess);
	S32 i,n=clients.size();
	vector<U8> sendout;
	sendout.push_back(0); // from
	sendout.insert(sendout.end(),mess.begin(),mess.end());
	for (i=1;i<n;++i) {
		if (clients[i].s) {
			tcpwritetoken32(clients[i].s,id+1,(const C8*)&sendout[0],sendout.size());
		}
	}
}
