// cube maps in software
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace cubemaptest2 {
#endif

 // do a real cubemap simulation
bool docubemap = true;

// manifest of cubemaps
const C8* cubemapfilelist[] = {
	"cube02",
	"cubemap_mountains.jpg",
	"cube2.jpg",
	"cubicmap.jpg",
	"cube.jpg",
	"footballfield",
	"fishpond",
	"skansen",
	"skansen4",
};
const S32 ncubemaplist = NUMELEMENTS(cubemapfilelist);
S32 curcube = 5;

// cubemap ui pitch,yaw,roll
float ctroll,ctpitch,ctyaw;
struct menuvar cubedv[]={
	{"@lightred@---- cubemaptest2 -------------------",0,D_VOID,0},
	{"cubemap2 roll",&ctroll,D_FLOAT,FLOATUP/8},
	{"cubemap2 pitch",&ctpitch,D_FLOAT,FLOATUP/8},
	{"cubemap2 yaw",&ctyaw,D_FLOAT,FLOATUP/8},
};
const S32 ncubedv = NUMELEMENTS(cubedv);


// an instance of cubemap class
cubemap* cm;

pointf3 screen2normal(pointi2 s)
{
	pointf3 n;
	n.x = (s.x - WX*.5f)/(WY*.5f);
	n.y = -(s.y - WY*.5f)/(WY*.5f);
	n.z = 1.0f;
	return n;
}

void screen2normaltest()
{
	logger("screen2normal test once!!!\n");
	static pointi2 ps[] = {
		{0,0},
		{WX/2,WY/2},
		{WX-1,WY-1},
	};
	S32 nps = NUMELEMENTS(ps);
	S32 i;
	for (i=0;i<nps;++i) {
		pointf3 n = screen2normal(ps[i]);
		logger("point %4d %4d, normal %f %f %f\n",ps[i].x,ps[i].y,n.x,n.y,n.z);
	}
	logger("float to screen test\n");
	float f;
	const S32 res = 5;
	for (f=-1.0f;f<=1.0f;f+=(1.0f/32.0f)) {
		S32 x = cubemap::float2bitmap(f,res);
		logger("f = %f, x = %d\n",f,(S32)x);
	}
}

U32 float2intcol(float v)
{
	if (v < 0.0f)
		v = 0.0f;
	else if (v >= 1.0f)
		v = 0.9999f;
	return (S32) (v * 256.0f);
}

#ifdef DONAMESPACE
}
using namespace cubemaptest2;
#endif

void cubemaptest2init()
{
	logger("in cubemaptest2init\n");
	screen2normaltest();
	video_setupwindow(640,480);
	pushandsetdir("skybox");
	cm = new cubemap(cubemapfilelist[curcube]);
	extradebvars(cubedv,ncubedv);
}

void cubemaptest2proc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 't':
		delete cm;
		++curcube;
		if (curcube == ncubemaplist)
			curcube = 0;
		cm = new cubemap(cubemapfilelist[curcube]);
		break;
	case 'm':
		docubemap = !docubemap;
		break;
	}
}

void cubemaptest2draw2d()
{
	//if (!B32->data)
	//	return;
	if (docubemap) {
		pointf3x rpy(ctpitch,ctyaw,ctroll);
		mat4 m;
		buildrot3d(&rpy,&m);
		S32 i,j;
		C32 *data = B32->data;
		for (j=0;j<WY;++j) {
			for (i=0;i<WX;++i) {
				pointf3 n = screen2normal(pointi2x(i,j));
				xformvec(&m,&n,&n);
#if 1
				C32 cv = cm->textureCube(n);
#else
				pointf3 cvf = cm->textureCubeFloat(n);
				//cvf.y = 0.0f;
				//cvf.z = 0.0f;
				C32 cv;
				cv.r = float2intcol(cvf.x);
				cv.g = float2intcol(cvf.y);
				cv.b = float2intcol(cvf.z);
#endif
				*data++ = cv;
			}
		}
	} else {
		clipclear32(B32,C32(0,200,255));
		const bitmap32* bm = cm->getbitmap();
		//if (B32->data == 0)
		//	errorexit("no data for B32 !!");
		//clipblit32(bm,B32,0,0,0,0,bm->size.x,bm->size.y);
		//if (B32->data)
			clipscaleblit32(bm,B32);
	}
}

void cubemaptest2exit()
{
	extradebvars(0,0);
	cm->loginfo();
	delete cm;
	popdir();
}
