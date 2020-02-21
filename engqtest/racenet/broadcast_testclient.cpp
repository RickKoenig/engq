#define RES3D
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

//#include "match_logon.h"
//#include "../u_states.h"
//#include "gameinfo.h"

#include "broadcast_testserver.h"

#define LBUFFSIZE 1000 // size of the little buffer

namespace broadcast_testclient {

// ui
shape* rl,*focus;
pbut* BQUIT,*BLOGON,*BSEND,*BNOSEND,*BLOGOFF;
listbox* CONSOLE;
hscroll* SLIDERATE,*SLIDEPACKSIZE;
text* TEXTRATE,*TEXTPACKSIZE;

// network
int nips; // got my ip yet?
struct socker *cl;
enum clstate {WAITIP,LOGON,READY,SENDING};
clstate cls; // test client 'state'
C8 lbuff[LBUFFSIZE];
S32 totalsent,totalgot;
S32 frm,frmdelay;
S32 num2write;

} // end namespace broadcast_testclient
using namespace broadcast_testclient;

void broadcast_testclient_init()
{
	logger("testclient init\n");
// graphics
	video_setupwindow(GX,GY);
	init_res3d(800,600);
// ui
pushandsetdir("racenetdata/broadcast");
	rl=res_loadfile("broadcast_testclientres.txt");
	BLOGON=rl->find<pbut>("BLOGON");
	BSEND=rl->find<pbut>("BSEND");
	BNOSEND=rl->find<pbut>("BNOSEND");
	BLOGOFF=rl->find<pbut>("BLOGOFF");
	BQUIT=rl->find<pbut>("BQUIT");
	CONSOLE=rl->find<listbox>("CONSOLE");
	CONSOLE->setmax(400);
	SLIDERATE=rl->find<hscroll>("SLIDERATE");
	SLIDERATE->setminmaxval(1,100);
	SLIDEPACKSIZE=rl->find<hscroll>("SLIDEPACKSIZE");
	SLIDEPACKSIZE->setminmaxval(1,LBUFFSIZE);
	TEXTRATE=rl->find<text>("TEXTRATE");
	TEXTPACKSIZE=rl->find<text>("TEXTPACKSIZE");
	focus=CONSOLE;
// network
	cl=0;
	cls=WAITIP;
	nips=0;
	totalsent=totalgot=0;
	initsocker();
	frm=0;
	frmdelay=60;
	num2write=7;
	SLIDERATE->setidx(frmdelay);
	SLIDEPACKSIZE->setidx(num2write);
	CONSOLE->printf("broadcast_testclient init");
}

void broadcast_testclient_proc()
{
// ui
	C8 str[50];
	S32 ridx=SLIDERATE->getidx();
	S32 nb=SLIDEPACKSIZE->getidx();
	sprintf(str,"rate %5.2f p/s",(float)wininfo.fpswanted/ridx);
	TEXTRATE->settname(str);
	if (nb==1)
		sprintf(str,"packsize %d byte",nb);
	else
		sprintf(str,"packsize %d bytes",nb);
	TEXTPACKSIZE->settname(str);
	frmdelay=ridx;
	num2write=nb;
	
	switch(cls) {
	case WAITIP:
		BLOGON->setvis(false);
		BSEND->setvis(false);
		BNOSEND->setvis(false);
		BLOGOFF->setvis(false);
		break;
	case LOGON:
		BLOGON->setvis(true);
		BSEND->setvis(false);
		BNOSEND->setvis(false);
		BLOGOFF->setvis(false);
		break;
	case READY:
		BLOGON->setvis(false);
		BSEND->setvis(true);
		BNOSEND->setvis(false);
		BLOGOFF->setvis(true);
		break;
	case SENDING:
		BLOGON->setvis(false);
		BSEND->setvis(false);
		BNOSEND->setvis(true);
		BLOGOFF->setvis(true);
		break;
	};
// escape
	if (KEY==K_ESCAPE)
		popstate();
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
//enum clstate {WAITIP,LOGON,READY,SENDING};
	if (focus) {
		S32 ret=focus->procfocus(); // focusproc returns -1 if nothing 0 or more when a hit occurs
		shape* focusa = ret>=0 ? focus : 0; // see if something happened
		if (focusa==BQUIT) {
			popstate();
		} else if (focusa==BLOGON) {
			cl=tcpgetconnect(str2ip("127.0.0.1"),BROADCAST_TESTPORT);
			if (cl) {
				cls=READY;
				CONSOLE->printf("connected");
//				tcpputbuffdata(cl,"1234",4,4);
				totalsent=totalgot=0;
			} else {
				CONSOLE->printf("can't connect");
			}
		} else if (focusa==BSEND) {
			cls=SENDING;
			CONSOLE->printf("send");
		} else if (focusa==BNOSEND) {
			cls=READY;
			CONSOLE->printf("nosend");
		} else if (focusa==BLOGOFF) {
			freesocker(cl);
			cl=0;
			cls=LOGON;
			CONSOLE->printf("logoff");
		}
	}
// end ui
// network
// get ip(s)
	if (cls==WAITIP) {
		nips=getnmyip();	// returns 0 until we get one
		if (nips>0) { // show ip's somewhere
			// getmyip(0);
			CONSOLE->printf("got myip");
			cls=LOGON;
		}
	}
	if (cl && cl->flags&SF_CLOSED) {
		CONSOLE->printf("closed");
		freesocker(cl);
		cl=0;
		cls=LOGON;
	}
	if (cls==READY || cls==SENDING) { // logged on
// net --> readbuff
		tcpfillreadbuff(cl);
// get some data
		S32 rd=tcpgetbuffdata(cl,lbuff,1,LBUFFSIZE);
		if (rd>0) {
			S32 i;
			for (i=0;i<rd;++i) {
				if ((U8)lbuff[i]!=U8(totalgot+5)) {
					CONSOLE->printf("mismatch C%d,S%d",(U8)totalgot+5,U8(lbuff[i]));
					freesocker(cl);
					cl=0;
					cls=LOGON;
				}
				++totalgot;
			}
			CONSOLE->printf("read %d/%d bytes",rd,totalgot);
		}
// write some data
		if (cls==SENDING && !frm) {
			S32 i;
			for (i=0;i<num2write;++i)
				lbuff[i]=totalsent++;
			S32 numwrote=tcpputbuffdata(cl,lbuff,num2write,num2write);
			if (!numwrote)
				error("testclient: can't tcpputbuffdata, data lost");
			CONSOLE->printf("wrote %d/%d bytes",numwrote,totalsent);
		}
// writebuff --> net
		tcpsendwritebuff(cl);
	}
	++frm;
	if (frm>=frmdelay)
		frm=0;
}

void broadcast_testclient_draw3d()
{
	video_sprite_begin(
	800,600,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	0,
	0);
	rl->draw3d();
	video_sprite_end(); 
}

void broadcast_testclient_exit()
{
popdir();
	delete rl;
	exit_res3d();
	if (cl)
		freesocker(cl);
	uninitsocker();
}
