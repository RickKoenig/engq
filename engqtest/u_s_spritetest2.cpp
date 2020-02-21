// test sprites
#include <m_eng.h>
#include "u_states.h"

namespace {

const U32 numx=100;
const U32 numy=100;
bitmap32* opaq;
S32 togvidmode;

class obj {
	bitmap32* pic;
	float x,y;
protected:
	float xp,yp;
	static float scl;
public:
	obj(float xa,float ya,bitmap32* pica) : pic(pica),xp(xa),yp(ya) {}
	virtual void proc();
	void draw();
};
float obj::scl=1;
void obj::proc()
{
	x=(xp-WX/2)*scl+WX/2;
	y=(yp-WY/2)*scl+WY/2;
//	x=xp;
//	y=yp;
}
void obj::draw()
{
	clipblit32(pic,B32,0,0,(S32)x,(S32)y,pic->size.x,pic->size.y);
}
list<obj*> bag; // a list of base obj ptrs

class playerk : public obj {
public:
	playerk(float xa,float ya,bitmap32* pica) : obj(xa,ya,pica) {}
	void proc();
};
void playerk::proc()
{
// move object via keyboard
	if (wininfo.indebprint)
		return;
	if (wininfo.keystate[K_RIGHT])
		xp+=5;
	if (wininfo.keystate[K_LEFT])
		xp-=5;
	if (wininfo.keystate[K_DOWN])
		yp+=5;
	if (wininfo.keystate[K_UP])
		yp-=5;
	if (wininfo.keystate[K_PAGEUP])
		scl+=.15f;
	if (wininfo.keystate[K_PAGEDOWN])
		scl-=.15f;
	obj::proc();
}

class playerm : public obj {
public:
	playerm(bitmap32* pica) : obj(0,0,pica) {}
	void proc();
};

void playerm::proc()
{
// move object via mouse
	xp=(float)MX;
	yp=(float)MY;
	obj::proc();
}

} // end namespace

void spritetest2init()
{
	video_setupwindow(800,600);
	pushandsetdir("gfxtest");
	bitmap32* opaqorig=gfxread32("maptestnck.tga");
	opaq=bitmap32alloc(16,16,C32BLACK); // scale object down
	clipscaleblit32(opaqorig,opaq); // nice filter
	bitmap32free(opaqorig);
	showcursor(0);
// handle objs
	obj* d3o;
	U32 i,j;
	for (j=0;j<numy;++j) {
		for (i=0;i<numx;++i) {
			d3o=new obj(
				50.0f+i*(WX-100)/(numx-1),
				50.0f+j*(WY-100)/(numy-1),opaq);
			bag.push_back(d3o);
		}
	} 
	d3o=new obj(100,100,opaq);
	bag.push_back(d3o);
	d3o=new playerk(125,125,opaq);
	bag.push_back(d3o);
	d3o=new playerm(opaq);
	bag.push_back(d3o);
}

void spritetest2proc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	};
	if (wininfo.mmiddleclicks) {
		togvidmode^=1; // windowed/fullscreen
		video_init(togvidmode,0);
		video_setupwindow(800,600);
	}

	list<obj*>::iterator it=bag.begin();
	while(it!=bag.end()) {
		(*it)->proc();
		++it;
	}
}

void spritetest2draw2d()
{
	fastclear32(B32,C32MAGENTA);

//	clipblit32(opaq,B32,0,0,MX,MY,opaq->size.x,opaq->size.y);

	list<obj*>::iterator it=bag.begin();
	while(it!=bag.end()) {
		(*it)->draw();
		++it;
	}

	outtextxybf32(B32,12,WY-12,C32WHITE,C32BLACK,"mouse %4d %4d",MX,MY);
}

void spritetest2exit()
{
	popdir();
	bitmap32free(opaq);
	showcursor(1);

	list<obj*>::iterator it=bag.begin();
	while(it!=bag.end()) {
		delete (*it);
		++it;
	}
	bag.clear();
}
