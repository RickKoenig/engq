#include <iomanip>
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
#include "netobjclient.h"
#include "utils/json_packet.h"
#include "tracklist.h"

#include "match_logon.h"

#define WELCOMETIMEOUT 50
#define GOTIMEOUT 150 // time between 'welcome' and 'go'
#define CLIENTTIMEOUT 150
#define GAMETIMEOUT 200 
#define AFTERLOADTIMEOUT 120

// 7 different senarios
// 1) x botgame											play a bot game
// 2) x client normal									play a client game
// 3) x client timeout									play a bot game
// 4) x server normal									play a server game
// 5) x server timeout, some players connected			play a server game with some extra bots
// 6) x server timeout, no players connected			play a bot game
// 7) x escape or GAME_NONE								clean up and return to prev state

#ifdef MATHTEST
class mathtask
{
	static const S32 maxque = 1000;
	static const S32 maxoffenders = 50;
	socker* s;
	float start,step;
	bool stepmul;
	S32 totcnt;
	S32 sendcnt,reccnt;
	S32 score;
	S32 kind;
	float cur;
	list<float> xque;
	list<float> rque;
	list<float> xbad;
	list<float> sbad;
	list<float> cbad;
public:
	mathtask(socker* sa,float starta,float stepa,bool stepmula,S32 totcnta,S32 kinda) : 
	  s(sa),start(starta),step(stepa),stepmul(stepmula),
	  totcnt(totcnta),sendcnt(0),reccnt(0),score(0),kind(kinda),cur(starta)
	{
	}
	S32 getscore() { return score; }
	S32 getrec() { return reccnt; }
	S32 gettot() { return totcnt; }
	bool popoffenders(float& argo,float& cbado,float& sbado)
	{
		if (xbad.empty())
			return false;
		argo = xbad.front();
		xbad.pop_front();
		cbado = cbad.front();
		cbad.pop_front();
		sbado = sbad.front();
		sbad.pop_front();
		return true;
	}
	void addresult(float sres)
	{
		if (rque.empty())
			errorexit("mathtest: que empty");
		float arg = xque.front();
		xque.pop_front();
		float cres = rque.front();
		rque.pop_front();
		++reccnt;
		bool correct = sres == cres;
		if (correct) {
			++score;
		} else {
			if (cbad.size()<maxoffenders) {
				xbad.push_back(arg);
				cbad.push_back(cres);
				sbad.push_back(sres);
			}
		}
	}
	bool proc()
	{
		while(true) {
			if (reccnt>=totcnt)
				return false; // done
			if (sendcnt>=totcnt)
				return true; // wait for server all done
			if (rque.size()>=(S32)maxque)
				return true; // server busy
// work to do
			mathreq mr;
			mr.kind = kind;
			float x = cur;
			float y = 0.0f;
			mr.x = cur;
			mr.y = 0.0f;
			tcpwritetoken32(s,PACK_MATHTEST,(const C8*)&mr,sizeof(mr));
			float r = 0.0f;
			switch(kind) {
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
			xque.push_back(x); // this is the arg
			rque.push_back(r); // this is what server should send back
			if (stepmul)
				cur *= step;
			else
				cur += step;
			++sendcnt;
		}
	}
};
static mathtask *mt;
#endif

// netclient
netclientobj::netclientobj(gamedescj* gd) :
	netobj(gd),clienttimeout(0),myip(0),myaj(gd->a),wc(0)
{
/*	grj = new gamerunj();
	grj->n=cgd->n;
	grj->n.gamenet=GAMENET_CLIENT;
	grj->e=cgd->e;
	grj->yourid=0; // your computer id
	grj->yourcntl=CNTL_HUMAN;
	grj->viewslot=0;
	grj->numplayers=cgd->nhumanplayers+cgd->nbotplayers;
// copy over car 0, make remaining bot cars, incase can't get server gamedescj
	grj->caros.resize(grj->numplayers);
	grj->caros[0]=cgd->a;
	grj->caros[0].ownerid=0;
	for (S32 i=1;i<grj->numplayers;++i) { // beyond the clients
		grj->caros[i].randombot();
	} */
	initsocker();
	wininfo.runinbackground=1;
	starttime=getmillisec();
	clienttimeout=starttime+1000*CLIENTTIMEOUT;
}

string netclientobj::proc()
{
	stringstream retss;
	S32 curtime=getmillisec();
// all the client prelims ... connections, closures, timeouts
// wait for initsock reply / wait for my ip
	if (!myip) {
		if (getnmyip()) {
			myip=getmyip(0);
			string ipstr;
			const C8* ipst=ip2str(myip);
			retss << "myip " << string(ipst) << ", tick " << curtime - starttime << "\n";
		}
	}
// i have an ip, make connection socket
	if (myip && !letsgo) { // got an ip, try to make some connections etc..
		if (!client.s) {
			socker* cl;
			if (matchcl && cgi.isvalid)
				cl=makecgisock();
			else
				cl=tcpgetconnect(cgd->n.serverip,cgd->n.serverport);
			if (cl) {
				retss << "client created " << ", tick " << curtime - starttime << "\n";
				client.s=cl;
				client.timeout = curtime + WELCOMETIMEOUT*1000;
			}
		}
	}
// close down bad connections
	if (client.s) {
		bool gameserverdied = false;
		if (client.s->flags&SF_CLOSED) {
			freesocker(client.s); // CLOSE closed
			client=clnt();
			if (letsgo)
				gameserverdied = true;
			retss << "client: at " << curtime - starttime << ", closed\n";
#ifndef MATHTEST
		} else if (curtime>=client.timeout) {
			freesocker(client.s); // timeout closed
			client=clnt();
			if (letsgo)
				gameserverdied = true;
			retss << "client: at " << curtime - starttime << ", timeout\n";
#endif
		}
		if (gameserverdied) {
			retss << "gameserverdied: at " << curtime - starttime << "\n";
			keys keybot;
			keybot.kwentbot = true;
			for (S32 i=0;i<grj->numplayers;++i) {
				if (i!=grj->yourid)
					keyques[i].push_back(keybot.getpack());
			}
		}
	}
// see if client out of time
#ifndef MATHTEST
	if (!letsgo) {
		if (curtime>=clienttimeout) {
// senario 5, senario 6   none or some not all players connected, partial/no net play
			retss << "client: didn't get all connections at " << curtime - starttime << "\n";
			retss << "client: reset endtime to " << client.timeout - starttime << "\n";
			freesocker(client.s);
			client=clnt();
			buildkeyques();
			delete wc;
			wc = 0;
			letsgo = true;
		}
	}
#endif
// end of all the client prelims...

// we've got sockets to read/write
	if (client.s) {
// read NET
		tcpfillreadbuff(client.s); // NET --> READBUFF
// process packets
		if (client.s) {
			int id;
			const int MAXTOKBUFF=100000;
			C8 tokbuff[MAXTOKBUFF];
			while (true) {
				S32 nr=tcpreadtoken32(client.s,&id,tokbuff,MAXTOKBUFF); // READ something
#ifdef MATHTEST
				if (mt) {
					if (!mt->proc()) {
						retss << "offenders    arg              client          server\n";
						while(true) {
							float argo,cbado,sbado;
							if (!mt->popoffenders(argo,cbado,sbado))
								break;
							retss << "         " << 
							  setw(15) << scientific << argo << " " << 
							  setw(15) << scientific << cbado << " " << 
							  setw(15) << scientific << sbado << "\n";
						}
						delete mt;
						mt = 0;
						retss << "delete mathtask\n";
					}
				}
#endif
				if (nr<=0)
					break;
				if (letsgo) { // ingame
					switch(id) {
					case PACK_KEYF: // got a 'key' packet from server
						{
							client.timeout = curtime + GAMETIMEOUT*1000;
							if (nr!=2)
								errorexit("bad key packet in client");
							U8 from = tokbuff[0];
							U8 key = tokbuff[1];
							keyques[from].push_back(key);
						}
						break;
					}
					if (id>=PACK_USER) { // user packet
						client.timeout = curtime + GAMETIMEOUT*1000;
						U8 from = tokbuff[0]; // for now, U8
						message mess;
						mess.id=PACK_KIND(id-1); // take off the 'from' ...
						mess.data=vector<U8>((U8*)(tokbuff+1),(U8*)(tokbuff+nr));
						genques[from].push_back(mess);
					}
				} else { // connect/logon
					switch(id) {
					case PACK_JSON_WELCOME:
						{
							json js = readjsontoken(tokbuff,nr);
							js.save("cl_test_read_welcome.txt"); // test save a text file of this welcome packet
// configure game based on welcome packet
							delete wc;
							wc = 0;
							envj::setpcnet();
							wc = new welcomej(js);
							retss << "client: read json welcome packet at " << curtime - starttime << "\n";
							retss << "  myid " << wc->yourid << ", numplayers " << wc->numplayers << "\n";
							retss << "  track " << wc->e.trackname << "\n";
							tracklist tl;
							string hths = tl.hastrackhash(wc->e.trackhash); // do we have that welcom track hash?
							bool hth = hths.size()>0;
							if (hth) {
								retss << "  has track " << hths << "\n";
								wc->e.trackname = hths; // use this name, same hash
							}
// reply back to server with avatar and hastrack
							const hij& hi = hij(myaj,hth); // make a hi packet from my avatar
							js = hi.save();
							writejsontoken(client.s,PACK_JSON_HI,js); // send to server
							client.state = 1;
							client.timeout = curtime + GOTIMEOUT*1000;
#ifdef MATHTEST
//							mt = new mathtask(client.s,-2,.00001f,false,400001,MT_ACOS);
//							mt = new mathtask(client.s,-2,.00001f,false,400001,MT_SIN);
//							mt = new mathtask(client.s,1,1.001f,true,200000,MT_SIN);
//							mt = new mathtask(client.s,0,1000,false,200000,MT_SIN);
//							mt = new mathtask(client.s,0,1,false,200000,MT_LOG);
//							mt = new mathtask(client.s,1,.99f,true,200000,MT_LOG);
//							mt = new mathtask(client.s,1,1.001f,true,200000,MT_COS);
//							mt = new mathtask(client.s,0,-1,false,2000,MT_EXP);
//							mt = new mathtask(client.s,0,-1,false,200000,MT_TAN);
//							mt = new mathtask(client.s,-2,.00001f,false,400001,MT_ACOS);
//							mt = new mathtask(client.s,-2,.00001f,false,400001,MT_ACOS);
//							mt = new mathtask(client.s,1,.999f,true,200000,MT_ACOS);
							mt = new mathtask(client.s,1,.999f,true,200000,MT_SQRT);
							retss << "mathtask         correct received total\n";
/*							retss << "new mathtask\n";
							mathreq mr = {MT_DIV,3.0f,5.0f};
							tcpwritetoken32(client.s,PACK_MATHTEST,(const C8*)&mr,sizeof(mr));
							mathreq mr2 = {MT_SUB,3.0f,5.0f};
							tcpwritetoken32(client.s,PACK_MATHTEST,(const C8*)&mr2,sizeof(mr2)); */
#endif
						}
						break;
					case PACK_JSON_TRACK: // if not hastrack, server will send track data
						{
							json js = readjsontoken(tokbuff,nr); // get track into js
							tracklist tl;
							string newtrackname = tl.uniquetrackname(wc->e.trackname); // get unique name for it
							string newtracknameext = newtrackname + ".trkoc"; // get unique name for it
pushandsetdir("newcarenadata");
pushandsetdirdown(getconstructeddir());
							js.save(newtracknameext.c_str()); // save the track
							wc->e.trackname = newtrackname; // use this name, same hash, maybe different name
popdir();
popdir();
							retss << "Got track " << newtrackname << " from server\n"; 
						}
						break;
					case PACK_JSON_GO:
						{
							json js = readjsontoken(tokbuff,nr);
							js.save("cl_test_read_go.txt"); // test save a text file of this welcome packet
// setup gamerunj
							js.get_object_array_object<caroj,caroj>("carojs","caroj",grj->caros);
							S32 yid=wc->yourid; 
							grj->e=wc->e; // env
							grj->yourid=yid;
							grj->viewslot=yid;
							grj->numplayers=wc->numplayers;
							grj->caros.resize(grj->numplayers);
							grj->caros[yid]=cgd->a;
							grj->caros[yid].ownerid=yid;
// setup collective
							buildkeyques();
							client.timeout=starttime+1000*AFTERLOADTIMEOUT;
							delete wc;
							wc = 0;
							letsgo = true;
						}
						break;
#ifdef MATHTEST
					case PACK_MATHTEST:
						{
							client.timeout = curtime + GAMETIMEOUT*1000;
							if (nr!=sizeof(float))
								errorexit("bad mathtest packet");
							float result = *(float*)&tokbuff[0];
							if (!mt)
								errorexit("bad mathresult");
							mt->addresult(result);
							S32 rec = mt->getrec();
							S32 tot = mt->gettot();
							if ((rec%10000)==0 || rec==tot) {
								S32 score = mt->getscore();
	//							retss << "mathresult " << result << "\n"; 
								retss << "mathresult stats " << 
									setw(6) << score <<
									" " << setw(6) << rec << 
									" " << setw(6) << tot << "\n"; 
	//							retss << "mathresult " << result << " stats " << score << " " << rec << " " << tot << "\n"; 
							}
						}
						break;
#endif
					}
				}
			}
		}
// write NET
		tcpsendwritebuff(client.s); // WRITEBUFF --> NET, returns true if all data sent
	}
	return retss.str();
}

// ingame: send key to collective
void netclientobj::writekey(const keys& key) 
{
	netobj::writekey(key);
	if (client.s) {
		U8 k = key.getpack();
		tcpwritetoken32(client.s,PACK_KEY,(const C8*)&k,sizeof(U8));
	}
}

void netclientobj::writenone() 
{
	if (client.s) {
		C8 dum = 0;
		tcpwritetoken32(client.s,PACK_NONE,&dum,sizeof(dum));
	}
}

netclientobj::~netclientobj()
{
	freesocker(client.s);
	wininfo.runinbackground=0;
	delete wc;
#ifdef MATHTEST
	delete mt;
	mt = 0;
#endif
}

S32 netclientobj::getnumsockets()
{
	if (client.s)
		return 1;
	else
		return 0;
}

S32 netclientobj::getwritebuffsize(S32 slot)
{
	return client.getwritebuffsize();
}

S32 netclientobj::getwritebuffused(S32 slot)
{
	return client.getwritebuffused();
}

void netclientobj::writegen(PACK_KIND id,const vector<U8>& mess)
{
	netobj::writegen(id,mess);
	if (client.s) {
		tcpwritetoken32(client.s,id,(const C8*)&mess[0],mess.size());
	}
}
