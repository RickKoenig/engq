#define RES3D
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "broadcast_testserver.h"
#include "broadcast_server.h"

#define MAXCL 256 // max clients
#define MAXCLBUFF 1000 // parse buffers
#define INACTIVETIME 720 // seconds of not hearing anything from logged on client (12 minutes)
#define LBUFFSIZE 10

namespace broadcast_testserver {

struct socker *sv; // master socket
int nips; // got my ip yet?

struct client {
	struct socker *cl;
	int inactivetime;
	int kick;
	int totread,totwrite;
};

client clients[MAXCL];
int numcl;

//char parsebuff[MAXCLBUFF+1]; // for null at end
//char sendbuff[MAXCLBUFF+1];
int frm;

shape* rl,*focus;
pbut* QUIT;
listbox* CONSOLE;
} // end namespace broadcast_server
using namespace broadcast_testserver;

void broadcast_testserver_init()
{
	logger("server init\n");
// graphics
	video_setupwindow(GX,GY);
	init_res3d(800,600);
// ui
pushandsetdir("racenetdata/broadcast");
	rl=res_loadfile("broadcast_testserverres.txt");
	QUIT=rl->find<pbut>("QUIT");
	CONSOLE=rl->find<listbox>("CONSOLE");
	CONSOLE->setmax(400);
	focus=CONSOLE;
// network
	nips=0;
	sv=NULL;
	initsocker();
	CONSOLE->printf("broadcast_testserver init");
}

void broadcast_testserver_proc()
{
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
		sv=tcpmakemaster(0,BROADCAST_TESTPORT);
		if (sv) {
			CONSOLE->printf("listen: ip '%s', port %d",ip2str(getmyip(0)),sv->port); // remember: just 1 ip2str per printf
		}
	}
// log out old clients
	int i;
	for (i=0;i<numcl;i++) {
		if ((clients[i].cl->flags & SF_CLOSED) || clients[i].kick) {
			if (clients[i].kick)
				CONSOLE->printf("client %d kicked",i);
			if (clients[i].cl->flags & SF_CLOSED)
				CONSOLE->printf("client %d closed",i);
			freesocker(clients[i].cl);
			numcl--;
			clients[i]=clients[numcl];
			i--;
		}
	}
// log in new clients
	if (numcl<MAXCL) {
		while(1) {
			socker *cl=tcpgetaccept(sv);
			if (cl) {
				memset(&clients[numcl],0,sizeof(struct client));
				clients[numcl].cl=cl;
				clients[numcl].totread=clients[numcl].totwrite=0;
				clients[i].inactivetime=getmillisec(); // init inactive timeout
				numcl++;
				CONSOLE->printf("client %d connected",i);
			} else
				break;
		}
	}
// see what clients are saying
	for (i=0;i<numcl;i++) {
		tcpfillreadbuff(clients[i].cl); // get from net
		while (1) {
// parse read buffer
			C8 litlinbuff[LBUFFSIZE];
			C8 litloutbuff[LBUFFSIZE];
			S32 numread=tcpgetbuffdata(clients[i].cl,litlinbuff,1,LBUFFSIZE);
			if (!numread)
				break; // done get from net
// process buffers in --> out
			clients[i].totread+=numread;
			CONSOLE->printf("got %d/%d bytes from client %d",numread,clients[i].totread,i);
			S32 j;
			for (j=0;j<numread;++j)
				litloutbuff[j]=litlinbuff[j]+5; // do something to the data
// output write buffer
			S32 numwrote=tcpputbuffdata(clients[i].cl,litloutbuff,numread,numread);
			if (numwrote!=numread) {
				clients[i].kick=true;
				CONSOLE->printf("write error, client %d, kick",i);
			}
			clients[i].totwrite+=numwrote;
			CONSOLE->printf("wrote %d/%d bytes to client %d",numwrote,clients[i].totwrite,i);
		}
// timeout?
		if (getmillisec()-clients[i].inactivetime>(INACTIVETIME*1000)) {
			clients[i].kick=true; // inactive
			CONSOLE->printf("client %d timed out, kick",i);
		} 
	}
	for (i=0;i<numcl;i++) // send write buffers to net
		tcpsendwritebuff(clients[i].cl);
//	if (!(frm&1)) {
//		CONSOLE->printf("frame %d",frm);
//	}
	frm++;
}

void broadcast_testserver_draw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	video_sprite_end(); 
}

void broadcast_testserver_exit()
{
// free ui
	popdir();
	delete rl;
	rl=0;
	exit_res3d();
// free network
	S32 i;
	for (i=0;i<numcl;i++) {
		freesocker(clients[i].cl);
	}
	numcl=0;
	if (sv)
		freesocker(sv);
	sv=0;
	uninitsocker();
}
