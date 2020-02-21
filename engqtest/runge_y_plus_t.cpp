// physics (runge kutta) try dy/dt = y + t starting at y=1 and t=1
#include <math.h>
#include <stdlib.h>
#include <engine1.h>

#include "rungekutta.h"

// mode
static int hicolormode=565;
static int wsizex=640,wsizey=480;

static float timestep;

static float startpos=1;
static float starttime=1;

static float pos;
static float time;

static float actpos;
static float acttime;

static float objstate[1];
static int offsets[1]={0};

enum {POS};

static void cf(float *os,float *delos,float t)
{
	delos[POS]=os[POS]+t;
}

static void moveobjs(float ts)
{
// standard
	pos+=ts*(acttime+pos);
// rungekutta
	dorungekutta(objstate,offsets,1,acttime,ts,cf);
// actual
	acttime+=timestep;
	actpos=(float)(3.0/exp(1.0)*exp(acttime)-acttime-1);
}

static void showobjects()
{
	clipcircle16(B16,(int)(acttime*WX/10),(int)(WY-actpos*WY/400),3,color24to16(rgbwhite,BPP));
	clipcircle16(B16,(int)(acttime*WX/10),(int)(WY-pos*WY/400),2,color24to16(rgbred,BPP));
	clipcircle16(B16,(int)(acttime*WX/10),(int)(WY-objstate[POS]*WY/400),2,color24to16(rgbgreen,BPP));
}

void user56init()
{
	acttime=starttime;
	actpos=startpos;
	pos=startpos;
	objstate[POS]=startpos;
	video_setupwindow(wsizex,wsizey,hicolormode);
	video_lock();
	cliprect16(B16,0,0,WX-1,WY-1,color24to16(rgbblue,BPP));
	video_unlock();
}

void user56proc()
{
	if (wininfo.fpswanted>0)
		timestep=.25f;
	else
		timestep=0;
	if (timestep<10)
		moveobjs(timestep);
	video_lock();
	showobjects();
	video_unlock();
}

void user56exit()
{
}
