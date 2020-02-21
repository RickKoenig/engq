// borrowed form 'simple web browser' 'u_s_browser.cpp'
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_states.h"

namespace {
shape* rl;
shape* focus,*oldfocus;
pbut* pbutquit;
edit* editphrase;

con32* con;

S32 gotmyip;
socker* cly;
#define USERBUFFSIZE 10240
C8 userbuff[USERBUFFSIZE];

S32 yport=2013;
char* strip = "76.218.105.191";
U32 serverip;
} // end anonymous namespace

void chatter_test_init()
{
	con=con32_alloc(760,360,C32WHITE,C32BLACK);
	initsocker();
	serverip = str2ip(strip);
	video_setupwindow(800,600);
	factory2<shape> fact;
	pushandsetdir("chatter");
	script sc("chatterres.txt");
	popdir();
	rl=fact.newclass_from_handle(sc);
	pbutquit=rl->find<pbut>("PBUTQUIT");
	editphrase=rl->find<edit>("EDITPHRASE");

	focus=oldfocus=0;
	gotmyip=0;
	focus = editphrase;
}

void chatter_test_proc()
{
// read data from socket
	tcpfillreadbuff(cly); // remote to local
	S32 len=tcpgetbuffdata(cly,userbuff,1,USERBUFFSIZE-1);
	if (len > 0) {
		userbuff[len] = '\0';
		logger("\nread data '%s', len %d\n",userbuff,len);
		con32_printf(con,"receive >>> '%s'\n",userbuff);
	}

// bail
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);

// handle buttons and editboxes
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
	if (focus == pbutquit) {
		if (ret==1)
		poporchangestate(STATE_MAINMENU);
	} else if (focus == editphrase) {
		if (ret > 0) {
			const C8* phr = editphrase->gettname();
			con32_printf(con,"send >>> '%s'\n",phr);
			string phrcr = phr;
			phrcr += "\r\n";
			U32 ps = phrcr.size();

			len=tcpputbuffdata(cly,phrcr.c_str(),1,ps);
			logger("write data %d\n",ps);
			editphrase->settname("");
		}
	}
// done handle buttons and editboxes

// wait for my ip, then create socket and connect
	if (!gotmyip) {
		gotmyip=getnmyname();
		S32 i;
		for (i=0;i<gotmyip;i++)
			logger("mynames(s) '%s'\n",getmyname(i));
		gotmyip=getnmyip();
		for (i=0;i<gotmyip;i++)
			logger("myip(s) '%s'\n",ip2str(getmyip(i)));
		if (gotmyip) {
			con32_printf(con,"Ready!\n");
			if (!cly) {
				cly=tcpgetconnect(serverip,yport);
				if (!cly)
					errorexit("can't get connect");
			}
		}
	} 
	tcpsendwritebuff(cly); // local to remote
}

void chatter_test_draw2d()
{
	clipclear32(B32,C32BLUE); // black background
	rl->draw(); // draw UI
	clipblit32(con32_getbitmap32(con),B32,0,0,20,20,WX,WY); // draw console
}

void chatter_test_exit()
{
	delete rl; // free UI
	if (cly) { // free socker
		freesocker(cly);
		cly=0;
	}
	uninitsocker(); // turn off socker system
	con32_free(con); // free console
}
