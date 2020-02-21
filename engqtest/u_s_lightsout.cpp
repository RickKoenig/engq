#include <m_eng.h>
#include "m_perf.h"
#include "u_states.h"

namespace lightsout {
static U32 bx=2,by=2;
static U32 hasinverse;
#define MAXBX 12
#define MAXBY 11

#define LEFTX 5
#define RIGHTX (WX/2+5)
#define TOPY 70
#define SIZE 20

static struct matb* but2lit;
static struct matb* lit2but;

struct boardl {
	struct vecb* lights,*buttons;
};
static struct boardl puzz,solve;

struct butplace {
	U32 earlybail;
	S32 px,py;
	const C8* desc;
	void (*func)();
};

static void quitfunc()
{
	popstate();
}

static void resetpfunc()
{
	vecbzero(puzz.lights);
	vecbzero(puzz.buttons);
}

static void resetsfunc()
{
	vecbzero(solve.lights);
	vecbzero(solve.buttons);
}

static void flipsfunc()
{
	S32 i;
	for (i=0;i<solve.lights->order;++i)
		solve.lights->data[i]^=1;
}

static void pbxfunc()
{
	if (bx<MAXBX)
		++bx;
	changestate(STATE_LIGHTSOUT);
}

static void mbxfunc()
{
	if (bx>1)
		--bx;
	changestate(STATE_LIGHTSOUT);
}

static void pbyfunc()
{
	if (by<MAXBY)
		++by;
	changestate(STATE_LIGHTSOUT);
}

static void mbyfunc()
{
	if (by>1)
		--by;
	changestate(STATE_LIGHTSOUT);
}

static struct butplace bp[]={
	{0,450,360,"Quit",quitfunc},
	{0, 10,340,"Reset",resetpfunc},
	{0,266,340,"Reset",resetsfunc},
	{0,266,315,"Flip",flipsfunc},
	{1,100,360,"-",mbxfunc},
	{1,140,360,"+",pbxfunc},
	{1,356,360,"-",mbyfunc},
	{1,396,360,"+",pbyfunc},
};
#define NBUTPLACE (sizeof(bp)/sizeof(bp[0]))

static void buildmats()
{
	U32 ib,jb,il,jl,bidx,lidx;
// build but to light matrix first
	matbzero(but2lit);
	for (jb=0;jb<by;++jb) {
		for (ib=0;ib<bx;++ib) {
			bidx=ib+jb*bx;
			for (jl=0;jl<by;++jl) {
				for (il=0;il<bx;++il) {
					lidx=il+jl*bx;
					if (lidx==bidx)
						matbsetcolrow(but2lit,bidx,lidx,1);
					if (il==ib-1 && jl==jb)
						matbsetcolrow(but2lit,bidx,lidx,1);
					if (il==ib+1 && jl==jb)
						matbsetcolrow(but2lit,bidx,lidx,1);
					if (il==ib && jl==jb-1)
						matbsetcolrow(but2lit,bidx,lidx,1);
					if (il==ib && jl==jb+1)
						matbsetcolrow(but2lit,bidx,lidx,1);
				}
			}
		}
	}
	hasinverse=matbinverse(but2lit,lit2but);
}

static void draw_butplace()
{
	U32 i;
	for (i=0;i<NBUTPLACE;++i) {
		S32 px,py;
		S32 sx,sy;
		px=bp[i].px-8;
		py=bp[i].py-8;
		sx=16+8*strlen(bp[i].desc);
		sy=24;
		cliprect32(B32,px,py,sx,sy,C32YELLOW);
		cliprect32(B32,px+1,py+1,sx-2,sy-2,C32CYAN);
		outtextxy32(B32,bp[i].px,bp[i].py,C32BLUE,bp[i].desc);
	}
}

static U32 proc_butplace()
{
	U32 i;
	if (!wininfo.mleftclicks)
		return 0;
	for (i=0;i<NBUTPLACE;++i) {
		S32 px,py;
		S32 sx,sy;
		px=bp[i].px-8;
		py=bp[i].py-8;
		sx=16+8*strlen(bp[i].desc);
		sy=24;
		if (MX>=px && MX<px+sx && MY>=py && MY<py+sy) {
			(*(bp[i].func))();
			return bp[i].earlybail;
		}
	}
	return 0;
}

static void draw_board(struct boardl* b,S32 px,S32 py)
{
	U32 i,j;
	C32 col;
	for (j=0;j<by;++j) {
		for (i=0;i<bx;++i) {
			U32 val;
			val=vecbgetele(b->lights,i+j*bx);
			if (val)
				col=C32YELLOW;
			else
				col=C32BLUE;
			cliprect32(B32,i*SIZE+px,j*SIZE+py,SIZE-2,SIZE-2,col);
			val=vecbgetele(b->buttons,i+j*bx);
			if (val)
				outtextxy32(B32,i*SIZE+px+(SIZE-8)/2,j*SIZE+py+(SIZE-8)/2,C32RED,"X");
		}
	}
}

static void proc_board_but(struct boardl* b,S32 spx,S32 spy)
{
	U32 i,j;
	U32 val;
	if (wininfo.mleftclicks) {
		for (j=0;j<by;++j) {
			for (i=0;i<bx;++i) {
				S32 px,py,sx,sy;
				px=i*SIZE+spx;
				py=j*SIZE+spy;
				sx=SIZE-2;
				sy=SIZE-2;
				if (MX>=px && MX<px+sx && MY>=py && MY<py+sy) {
					val=vecbgetele(b->buttons,i+j*bx);
					val^=1;
					vecbsetele(b->buttons,i+j*bx,val);
				}
			}
		}
	}
	vecbmult(but2lit,b->buttons,b->lights);
}

#if 1
static S32 iterb(struct vecb* b)
{
	S32 pos=0;
	while(pos<b->order) {
		if (b->data[pos]<1) {
			++(b->data[pos]);
			return 0;
		}
		b->data[pos]=0;
		++pos;
	}
	return 1;
}

static S32 vecbcount1s(struct vecb* b)
{
	U32 sum=0;
	S32 i;
	for (i=0;i<b->order;++i)
		sum+=b->data[i];
	return sum;
}

#define OLDBRUTE
//#define NEWBRUTE

#ifdef OLDBRUTE
static void brute(const struct matb* bu2li,const struct vecb* li,struct vecb* bu)
{
	perf_start(MAT2BRUTE);
	S32 ndig;
	struct vecb* test=vecbcreate(li->order);
//	for (ndig=li->order;ndig>=0;--ndig) {
	for (ndig=0;ndig<=li->order;++ndig) {
		vecbzero(bu);
		do {
			if (vecbcount1s(bu)==ndig) {
				vecbmult(but2lit,bu,test);
				if (!vecbcmp(test,li)) {
					vecbfree(test);
					perf_end(MAT2BRUTE);
					return;
				}
			}
		} while(!iterb(bu));
	}
	vecbzero(bu);
	vecbfree(test);
	perf_end(MAT2BRUTE);
}
#endif

#ifdef NEWBRUTE
static void brute(const struct matb* bu2li,const struct vecb* li,struct vecb* bu)
{
	perf_start(MAT2BRUTE);
	//S32 ndig;
	struct vecb* test=vecbcreate(li->order);
//	for (ndig=li->order;ndig>=0;--ndig) {
	//for (ndig=0;ndig<=li->order;++ndig) {
		vecbzero(bu);
		do {
			//if (vecbcount1s(bu)==ndig) {
				vecbmult(but2lit,bu,test);
				if (!vecbcmp(test,li)) {
					vecbfree(test);
					perf_end(MAT2BRUTE);
					return;
				}
			//}
		} while(!iterb(bu));
	//}
	vecbzero(bu);
	vecbfree(test);
	perf_end(MAT2BRUTE);
}
#endif

#endif

static void proc_board_light(struct boardl* b,S32 spx,S32 spy)
{
	U32 i,j;
	U32 val;
	if (wininfo.mleftclicks) {
		for (j=0;j<by;++j) {
			for (i=0;i<bx;++i) {
				S32 px,py,sx,sy;
				px=i*SIZE+spx;
				py=j*SIZE+spy;
				sx=SIZE-2;
				sy=SIZE-2;
				if (MX>=px && MX<px+sx && MY>=py && MY<py+sy) {
					val=vecbgetele(b->lights,i+j*bx);
					val^=1;
					vecbsetele(b->lights,i+j*bx,val);
				}
			}
		}
		if (hasinverse)
			vecbmult(lit2but,b->lights,b->buttons); // one solution
		else if (bx*by<25) // brute force it if reasonable
			brute(but2lit,b->lights,b->buttons);
		else
			matbsolve(but2lit,b->lights,b->buttons);

	}
}

static void testmats()
{
	U32 result;
	struct matn* mn=matncreate(3);
	struct vecn* vn1=vecncreate(3);
	struct vecn* vn2=vecncreate(3);
	matnsetcolrow(mn,0,0,2.0f);
	matnsetcolrow(mn,1,0,-5.0f);
	matnsetcolrow(mn,2,0,4.0f);
	matnsetcolrow(mn,0,1,1.0f);
	matnsetcolrow(mn,1,1,-2.0f);
	matnsetcolrow(mn,2,1,1.0f);
	matnsetcolrow(mn,0,2,1.0f);
	matnsetcolrow(mn,1,2,-4.0f);
	matnsetcolrow(mn,2,2,6.0f);
	vecnsetele(vn1,0,-3.0f);
	vecnsetele(vn1,1,5.0f);
	vecnsetele(vn1,2,10.0f);
	result=matnsolve(mn,vn1,vn2);
	matnlogger(mn,"mn");
	vecnlogger(vn1,"vn1");
	logger("result %d\n",result);
	vecnlogger(vn2,"vn2");
	matnfree(mn);
	vecnfree(vn1);
	vecnfree(vn2);
}

} // end namespace lightsout
using namespace lightsout;

void userlightsoutinit()
{
	testmats();
	video_setupwindow(512,384);
	puzz.lights=vecbcreate(bx*by);
	puzz.buttons=vecbcreate(bx*by);
	solve.lights=vecbcreate(bx*by);
	solve.buttons=vecbcreate(bx*by);
	but2lit=matbcreate(bx*by);
	lit2but=matbcreate(bx*by);
	buildmats();
}

void userlightsoutproc()
{
	if (KEY==K_ESCAPE)
		popstate();
//	if (KEY==K_RETURN)
//		popstate();
	if (proc_butplace())
		return;
	proc_board_but(&puzz,LEFTX,TOPY);
	proc_board_light(&solve,RIGHTX,TOPY);
}

void userlightsoutdraw2d()
{
//	video_lock();
	fastclear32(B32,C32CYAN);
	draw_board(&puzz,LEFTX,TOPY);
	draw_board(&solve,RIGHTX,TOPY);
	outtextxy32(B32,WX/4,10,C32WHITE,"L I G H T S   O U T");
	if (!hasinverse)
		outtextxy32(B32,WX/4,20,C32LIGHTRED,"NO INVERSE");
	outtextxy32(B32,10,50,C32WHITE,"Puzzle");
	outtextxy32(B32,WX/2+10,50,C32WHITE,"Solver");
	outtextxyf32(B32,WX/8-50,360,C32WHITE,"Width %d",bx);
	outtextxyf32(B32,WX*5/8-50,360,C32WHITE,"Height %d",by);
	draw_butplace();
//	video_unlock(); 
}

void userlightsoutexit()
{
	vecbfree(puzz.lights);
	vecbfree(puzz.buttons);
	vecbfree(solve.lights);
	vecbfree(solve.buttons);
	matbfree(but2lit);
	matbfree(lit2but);
}
