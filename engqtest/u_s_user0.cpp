// draw the mainmenu's shape class without doing anything else
#include <m_eng.h>
#include <l_misclibm.h>

static shape* rl;
static shape* focus;

void user0init()
{
//	test some winsock calls
/*	U32 version = MAKEWORD(1,1);
	WSADATA wsaData;
	S32 appState = WSAStartup( version, &wsaData);
	logger("appState = %d\n",appState);
	WSACleanup();
*/
	initsocker();
	uninitsocker();
	video_setupwindow(1366,768);
	factory2<shape> fact;
//	pushandsetdir("mainmenu");
//	script sc("mainres.txt");
//	pushandsetdir("gametheory");
//	script sc("gametheoryres.txt");
//	pushandsetdir("qcomp");
//	script sc("qcompres.txt");
	pushandsetdir("racenetdata/onlineopt");
	script sc("newlobbyres.txt");
	popdir();
	rl=fact.newclass_from_handle(sc);
	focus=0;
}

void user0proc()
{
	if (KEY==K_ESCAPE)
		popstate();
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	if (focus) {
		S32 ret=focus->proc();
	}
}

void user0draw2d()
{
//	video_lock();
	clipclear32(B32,C32BLUE);
	rl->draw();
//	video_unlock();
}

void user0exit()
{
	delete rl;
}
