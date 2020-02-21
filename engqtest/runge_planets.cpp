// physics (runge kutta) planets example
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <engine1.h>

#include "rungekutta.h"

// mode
static int hicolormode=565;
static int wsizex=800,wsizey=600;

static float bigg=110000;
static float timestep;

struct object {
	struct pointf2 pos;
	struct pointf2 vel;
	float mass;
};

static struct object romobjs[]={
	{0,   0,-9.8f,0, 100,},
	{0,-130,  240,0,   1,},
	{0,-110,  360,0,   1,},
	{0,-230,  140,0,   1,},
	{0,-210,  240,0,   1,},
	{0, 210, -240,0,   2,},
};

#define NOBJS (sizeof(romobjs)/sizeof(romobjs[0]))

static struct object rkobjs[NOBJS];
static struct object objs[NOBJS];

// 8 byte struct alignment, i thought, well now 4 byte struct alignment i guess
int offsets[]={
	0,1,2,3,
	5,6,7,8,
	10,11,12,13,
	15,16,17,18,
	20,21,22,23,
	25,26,27,28};

static void chfun(float *os,float *newos,float time)
{
	struct object *oos=(struct object *)os;
	struct object *onewos=(struct object *)newos;
	int i,j;
	struct pointf2 d;
	float r2,r;
// standard
	for (i=0;i<NOBJS;i++) {
		onewos[i].pos.x=oos[i].vel.x;
		onewos[i].pos.y=oos[i].vel.y;
		onewos[i].vel.x=0;
		onewos[i].vel.y=0;
		for (j=0;j<NOBJS;j++)
			if (i!=j) {
				d.x=oos[i].pos.x-oos[j].pos.x;
				d.y=oos[i].pos.y-oos[j].pos.y;
				r2=d.x*d.x+d.y*d.y;
				r=(float)sqrt(r2);
				onewos[i].vel.x+=-d.x*bigg*rkobjs[j].mass/(r2*r);
				onewos[i].vel.y+=-d.y*bigg*rkobjs[j].mass/(r2*r);
			}
	}
}

static void moveobjs(float ts)
{
	int i,j;
	struct pointf2 a,d;
	float r2,r;
// standard
	for (i=0;i<NOBJS;i++) {
		a.x=a.y=0;
		for (j=0;j<NOBJS;j++)
			if (i!=j) {
				d.x=objs[i].pos.x-objs[j].pos.x;
				d.y=objs[i].pos.y-objs[j].pos.y;
				r2=d.x*d.x+d.y*d.y;
				r=(float)sqrt(r2);
				a.x+=-d.x*bigg*objs[j].mass/(r2*r);
				a.y+=-d.y*bigg*objs[j].mass/(r2*r);
			}
		objs[i].vel.x+=ts*a.x;
		objs[i].vel.y+=ts*a.y;
	}
	for (i=0;i<NOBJS;i++) {
		objs[i].pos.x+=ts*objs[i].vel.x;
		objs[i].pos.y+=ts*objs[i].vel.y;
	}
// runge kutta
	dorungekutta((float *)rkobjs,offsets,4*NOBJS,0,ts,chfun);
}

static void showobjects()
{
	int i;
	for (i=0;i<NOBJS;i++) {
		clipcircle16(B16,(int)objs[i].pos.x+WX/2,(int)objs[i].pos.y+WY/2,3,color24to16(rgbred,BPP));
		clipcircle16(B16,(int)rkobjs[i].pos.x+WX/2,(int)rkobjs[i].pos.y+WY/2,3,color24to16(rgbgreen,BPP));
	}
}

void user54init()
{
	video_setupwindow(wsizex,wsizey,hicolormode);
	memcpy(rkobjs,romobjs,sizeof(romobjs));
	memcpy(objs,romobjs,sizeof(romobjs));
}

void user54proc()
{
	if (wininfo.fpswanted>0)
		timestep=1.0f/wininfo.fpswanted;
	else
		timestep=0;
	moveobjs(timestep);
	video_lock();
	cliprect16(B16,0,0,WX-1,WY-1,color24to16(rgbwhite,BPP));
	showobjects();
	video_unlock();
}

void user54exit()
{
}
