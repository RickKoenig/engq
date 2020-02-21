// physics (runge kutta) try SHO
#include <math.h>
#include <stdlib.h>
#include <engine1.h>

#include "rungekutta.h"

// mode
static int hicolormode=565;
static int wsizex=640,wsizey=480;
static struct bitmap16 *b;

static float k=10;
static float timestep;

static float pos=200;
static float vel;
static float accel;

static float startpos=200;
static float actpos;
static float acttime;

//static float kpos=200;
//static float kvel;

static float objstate[2]={200,0}; // pos and vel
enum {POS,VEL};

static void cf(float *os,float *delos,float t)
{
	delos[POS]=os[VEL];
	delos[VEL]=-k*os[POS];
}

static void moveobjs(float ts)
{
// standard
	static int offsets[2]={0,1};
	accel=-k*pos;
	vel+=ts*accel;
	pos+=ts*vel;
// rungekutta
	dorungekutta(objstate,offsets,2,acttime,ts,cf);
// actual
	acttime+=timestep;
	actpos=startpos*(float)cos(sqrt((float)k)*acttime);
}

static void showobjects()
{
	clipcircle16(b,WX-4,(int)actpos+WY/2,3,color24to16(rgbwhite,BPP));
	clipcircle16(b,WX-4,(int)pos+WY/2,2,color24to16(rgbred,BPP));
	clipcircle16(b,WX-4,(int)objstate[POS]+WY/2,2,color24to16(rgbgreen,BPP));
}

void user55init()
{
	video_setupwindow(wsizex,wsizey,hicolormode);
	b=bitmap16alloc(WX,WY,color24to16(rgbmagenta,BPP));
}

void user55proc()
{
	if (wininfo.fpswanted>0)
		timestep=1.0f/wininfo.fpswanted;
	else
		timestep=0;
	moveobjs(timestep);
	acttime=normalangdeg(acttime);
	clipblit16(b,b,8,0,0,0,WX,WY);
	cliprect16(b,WX-8,0,WX-1,WY-1,color24to16(rgbblue,BPP));
	video_lock();
	clipblit16(b,B16,0,0,0,0,WX,WY);
	showobjects();
	video_unlock();
}

void user55exit()
{
	bitmap16free(b);
}
