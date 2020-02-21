#define INCLUDE_WINDOWS
#include <m_eng.h>

#include "u_states.h"

void joytestdiinit()
{
	video_setupwindow(640,480);
}

void joytestdiproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
}

void joytestdidraw2d()
{
	clipclear32(B32,C32(0,0,255));	
	outtextxyf32(B32,200,10,C32WHITE,"Dinput Joytest");
	outtextxyf32(B32,20,20,C32WHITE,"Joy 0 but '%08x'",jdata[0].newsjoy);
	outtextxyf32(B32,20,60,C32WHITE,"Joy 0 axis 0 '%d'",jdata[0].newsjoyaxis[0]);
	outtextxyf32(B32,20,80,C32WHITE,"Joy 0 axis 1 '%d'",jdata[0].newsjoyaxis[1]);
	outtextxyf32(B32,20,100,C32WHITE,"Joy 0 axis 2 '%d'",jdata[0].newsjoyaxis[2]);
	outtextxyf32(B32,20,120,C32WHITE,"Joy 0 axis 3 '%d'",jdata[0].newsjoyaxis[3]);
	outtextxyf32(B32,320,20,C32WHITE,"Joy 1 but '%08x'",jdata[1].newsjoy);
	outtextxyf32(B32,320,60,C32WHITE,"Joy 1 axis 0 '%d'",jdata[1].newsjoyaxis[0]);
	outtextxyf32(B32,320,80,C32WHITE,"Joy 1 axis 1 '%d'",jdata[1].newsjoyaxis[1]);
	outtextxyf32(B32,320,100,C32WHITE,"Joy 1 axis 2 '%d'",jdata[1].newsjoyaxis[2]);
	outtextxyf32(B32,320,120,C32WHITE,"Joy 1 axis 3 '%d'",jdata[1].newsjoyaxis[3]);
}

void joytestdiexit()
{
}

#if 1
// for now i'll handle all joystick input with this one function..
// i don't see how this could be slow or unresponsive since windows doesn't use the joystick
// for anything
static U32 getjoyo(float *jx,float *jy,U32* jp) // floats from -1 to +1
{
	MMRESULT mmr;
	JOYCAPS jc;
//	static int gotcaps;
	JOYINFOEX jie;
//	if (!gotcaps) {
		joyGetDevCaps(JOYSTICKID1, &jc,sizeof(jc)); // in mingw this messes up logfile at termination
//		gotcaps=1;
//	}
	jie.dwSize=sizeof(jie);
	jie.dwFlags=JOY_RETURNALL;
	mmr=joyGetPosEx(JOYSTICKID1,&jie); // in mingw this messes up logfile at termination
	if (mmr==JOYERR_NOERROR) {
		*jx=(2.0f*jie.dwXpos-jc.wXmax-jc.wXmin)/(jc.wXmax-jc.wXmin);
		*jy=(2.0f*jie.dwYpos-jc.wYmax-jc.wYmin)/(jc.wYmax-jc.wYmin);
		*jp=1;
		return jie.dwButtons;
	} else {
		*jx=0;
		*jy=0;
		*jp=0;
		return 0;
	}
}
#endif

static float jmmx,jmmy;
static U32 jmmp,jmmb;

void joytestmminit()
{
	video_setupwindow(640,480);
}

void joytestmmproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	jmmb=getjoyo(&jmmx,&jmmy,&jmmp); // floats from -1 to +1
}

void joytestmmdraw2d()
{
	clipclear32(B32,C32(0,0,255));	
	outtextxyf32(B32,200,10,C32WHITE,"Mmedia Joytest");
	outtextxyf32(B32,20,20,C32WHITE,"Joy 0 but '%08x'",jmmb);
	outtextxyf32(B32,20,60,C32WHITE,"Joy 0 axis 0 '%6.3f'",jmmx);
	outtextxyf32(B32,20,80,C32WHITE,"Joy 0 axis 1 '%6.3f'",jmmy);
}

void joytestmmexit()
{
}
