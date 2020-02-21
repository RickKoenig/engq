#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "u_states.h"

S32 timmer;
S32 cs;
S32 ribgsv;
C8* spnlist[]={
	"notepad.exe",
	"calc.exe",
	"c:/srcw32/hardrace/hr_game/hardrace.exe",
};
const S32 numcs=sizeof(spnlist)/sizeof(spnlist[0]);

void spawner_init()
{
	wininfo.runinbackground=1;
	ribgsv=wininfo.runinbackground;
//	video_setupwindow(640,480);
	video_setupwindow(800,600);
	timmer=0;
	cs=0;
}

void spawner_proc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
//	if (cs!=numcs)
		++timmer;
//	else
//		changestate(-1);
//		popstate();
	if (timmer==30*4) {
		if (cs==numcs) {
			changestate(-1);
//			popstate();
			return;
		}
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si,0,sizeof(STARTUPINFO));
		si.cb           =   sizeof  (STARTUPINFO);
//		si.dwFlags      =   STARTF_USESHOWWINDOW;
//		si.wShowWindow  =   SW_SHOWNORMAL;

		CreateProcess(NULL,spnlist[cs],
			NULL,
			NULL,
			0,
			0,
//			NORMAL_PRIORITY_CLASS,
			NULL,
			NULL,
			&si,
			&pi
			);
		timmer=0;
		++cs;
//		if (cs==numcs)
//			cs=0;
	}
}

void spawner_draw2d()
{
	C32 col;
	clipclear32(B32,C32BLUE);
	outtextxyf32(B32,WX/2,WY-16,C32WHITE,"Timer %d",timmer);
}

void spawner_exit()
{
	wininfo.runinbackground=ribgsv;
}
