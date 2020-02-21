// matrix tests

#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

#include "ring2.h"
#include "augmatz.h"
#include "augmatz_ui.h"

#include "lightuppatterndata.h"
#include "lightuppatterns.h"
#include "lightupboard.h"

using namespace lightuppatterns;

namespace u_s_lightup {

S32 nequs = 1; // rows // 5 max
S32 nvars = 1; // cols // 5 max

S32 statemod = 2;
S32 wrap = 0;
S32 pattern = 0;
//const S32 npattern = 7; // see maxpatterns
S32 width = 3;
S32 height = 3;
S32 overxsave = 0;
S32 overysave = 0;
bool dooversave = false;

typedef augmatz_ui augmatnf;
augmatnf* amat;
typedef zee2 ring;

shape* rl;
shape* focus,*oldfocus;

pbut* pbutresetboard,*pbutwidth,*pbutheight,*pbutpattern,*pbutncolors,*pbutwrap;
pbut* pbuttomatrix;
pbut* pbutresetcalc,*pbutstepcalc,*pbutcalc;
pbut* pbutquit;

/*struct preload {
	S32* data;
	S32 w,h;
};

preload* pl;
*/
patternboard *pb;
gameboard* gb;

#if 0
S32 lightup4_4[] = {
	1,1,0,0, 1,0,0,0, 0,0,0,0, 0,0,0,0, 1,
	1,1,1,0, 0,1,0,0, 0,0,0,0, 0,0,0,0, 1,
	0,1,1,1, 0,0,1,0, 0,0,0,0, 0,0,0,0, 1,
	0,0,1,1, 0,0,0,1, 0,0,0,0, 0,0,0,0, 1,

	1,0,0,0, 1,1,0,0, 1,0,0,0, 0,0,0,0, 1,
	0,1,0,0, 1,1,1,0, 0,1,0,0, 0,0,0,0, 1,
	0,0,1,0, 0,1,1,1, 0,0,1,0, 0,0,0,0, 1,
	0,0,0,1, 0,0,1,1, 0,0,0,1, 0,0,0,0, 1,

	0,0,0,0, 1,0,0,0, 1,1,0,0, 1,0,0,0, 1,
	0,0,0,0, 0,1,0,0, 1,1,1,0, 0,1,0,0, 1,
	0,0,0,0, 0,0,1,0, 0,1,1,1, 0,0,1,0, 1,
	0,0,0,0, 0,0,0,1, 0,0,1,1, 0,0,0,1, 1,

	0,0,0,0, 0,0,0,0, 1,0,0,0, 1,1,0,0, 1,
	0,0,0,0, 0,0,0,0, 0,1,0,0, 1,1,1,0, 1,
	0,0,0,0, 0,0,0,0, 0,0,1,0, 0,1,1,1, 1,
	0,0,0,0, 0,0,0,0, 0,0,0,1, 0,0,1,1, 1,
};
preload pl_4x4 = {
	lightup4_4,
	16,16
};

S32 lightup3_3[] = {
	1,1,0, 1,0,0, 0,0,0, 1,
	1,1,1, 0,1,0, 0,0,0, 1,
	0,1,1, 0,0,1, 0,0,0, 1,

	1,0,0, 1,1,0, 1,0,0, 1,
	0,1,0, 1,1,1, 0,1,0, 1,
	0,0,1, 0,1,1, 0,0,1, 1,

	0,0,0, 1,0,0, 1,1,0, 1,
	0,0,0, 0,1,0, 1,1,1, 1,
	0,0,0, 0,0,1, 0,1,1, 1,
};
preload pl_3x3 = {
	lightup3_3,
	9,9
};
#endif

} // end namespace u_s_lightup
using namespace u_s_lightup;

void rememberoversavestate()
{
	pb->getoverraw(overxsave,overysave);
	dooversave = true;
}

// loadup an augmat from lightupboard
void tomatrix(augmatz* am,lightupboard* g)
{
	S32 lsize = am->nr*(am->nc+1);
	U8 *loaddata = new U8[lsize]; // matches am
	::fill(loaddata,loaddata+lsize,0);
	// non hom
	S32 i,j,k;
	for (k=0,j=0;j<g->getdimy();++j) {
		for (i=0;i<g->getdimx();++i,++k) {
			loaddata[am->nc+k*(am->nc+1)] = am->getmod()-1-g->getcell(i,j);
		}
	}
	// hom
	U8* homdata = new U8[g->getdimx() * g->getdimy()];
	for (k=0,j=0;j<g->getdimy();++j) {
		for (i=0;i<g->getdimx();++i,++k) {
			getpattern(i,j,g->getdimx(),g->getdimy(),g->getpat(),g->getwrap(),homdata); // sizeof data is dimx*dimy
			S32 m;
			for (m=0;m<am->nr;++m) {
				loaddata[k+m*(am->nc+1)] = homdata[m];
			}
		}
	}
	delete[] homdata;
	am->load(loaddata);
	delete[] loaddata;
}

void lightup_check()
{
	static bool checked = true;
	if (checked)
		return;
	checked = true;
	gcon_printf("lightup_check\n");
// check allvars with sum restriction
/*	zee2::setdefmod(3);
//	augmatz::setlosum(4);
	vector<vector<zee2> > allvars = augmatz::allvars(38);
	gcon_printf("allvars test\n");
	augmatz::printvars(allvars); */

	S32 ngood = 0,nbad = 0;
	S32 cnt = 0;
	S32 bdx,bdy,pt,wp,ncl;
#if 1
#if 1
	for (ncl=2;ncl<=2;++ncl)
	for (wp=0;wp<1;++wp)
	for (pt=0;pt<7;++pt)
	for (bdy=2;bdy<=8;++bdy)
	for (bdx=2;bdx<=8;++bdx) {
#elif 0
	for (ncl=2;ncl<=6;++ncl)
	for (wp=1;wp<2;++wp)
	for (pt=6;pt<7;++pt)
	for (bdy=7;bdy<=8;++bdy)
	for (bdx=7;bdx<=8;++bdx) {
#else
	for (ncl=3;ncl<=3;++ncl)
	for (wp=1;wp<2;++wp)
	for (pt=5;pt<6;++pt)
	for (bdy=2;bdy<=2;++bdy)
	for (bdx=2;bdx<=2;++bdx) {
#endif
		gcon_printf("---------------------------------\n");
		con32* gconsave = gcon;
		S32 prod = bdx*bdy;
		augmatz* a = new augmatz(ncl,prod,prod);
		lightupboard* b = new lightupboard("check",0,0,bdx,bdy,pt,wp);
		tomatrix(a,b);
		delete b;
		S32 timemath2a = getmillisec();
		gcon = 0;
		vector<vector<ring> > vars2 = a->getvars2(false); // new math
		S32 timemath2b = getmillisec();
		gcon = gconsave;
		gcon_printf("board: dimx %d, dimy %d, pat %d, wrap %d, ncols %d\n",
		  bdx,bdy,pt,wp,ncl);
		gcon_printf("new math, var size %d\n",vars2.size());
		S32 deltime = timemath2b-timemath2a;
		gcon_printf("millisec: check new math = %d\n",deltime);
		if (deltime >= 100000)
			gcon_printf("extreme time\n");
		else if (deltime >= 10000)
			gcon_printf("huge time\n");
		else if (deltime >= 1000)
			gcon_printf("big time\n");
		S32 err = a->getlasterr();
		gcon_printf("new math error is %d\n",err);
//		a->printvars(vars2);
		delete a;
		if (err) {
			gcon_printf("error at: dimx %d, dimy %d, pat %d, wrap %d, ncols %d\n",
			  bdx,bdy,pt,wp,ncl);
			++nbad;
		} else {
			++ngood;
		}
		if ((cnt%1)==0) {
			gcon_printf("cnt = %d\n",cnt);
			video_lock();
			clipblit32(con32_getbitmap32(gcon),B32,0,0,0,0,gcon->b32->size.x,gcon->b32->size.y);
			video_unlock();
			checkmessages();
			if (wininfo.closerequested)
				return;
			if (KEY == K_ESCAPE) {
				KEY = 0;
				return;
			}
			video_paintwindow(0);
		}
		++cnt;
	}
#endif
	gcon_printf("---------------------------------\ngood = %d, bad = %d\n",ngood,nbad);
}

void lightup_init()
{
	video_setupwindow(1024,768);
	pushandsetdir("lightup");
	// ui
	script sc("browserres.txt");
	rl=res_loadfile("lightup.txt");
	pbutresetboard=rl->find<pbut>("PBUTRESETBOARD");
	pbutwidth=rl->find<pbut>("PBUTWIDTH");
	pbutheight=rl->find<pbut>("PBUTHEIGHT");
	pbutpattern=rl->find<pbut>("PBUTPATTERN");
	pbutncolors=rl->find<pbut>("PBUTNCOLORS");
	pbutwrap=rl->find<pbut>("PBUTWRAP");
	pbutresetcalc=rl->find<pbut>("PBUTRESETCALC");
	pbuttomatrix=rl->find<pbut>("PBUTTOMATRIX");
	pbutstepcalc=rl->find<pbut>("PBUTSTEPCALC");
	pbutcalc=rl->find<pbut>("PBUTCALC");
	pbutquit=rl->find<pbut>("PBUTQUIT");
	C8 tname[100];
	sprintf(tname,"Num Colors %d",statemod);
	pbutncolors->settname(tname);
	sprintf(tname,"Width %d",width);
	pbutwidth->settname(tname);
	sprintf(tname,"Height %d",height);
	pbutheight->settname(tname);
	sprintf(tname,"Pattern %d",pattern);
	pbutpattern->settname(tname);
	sprintf(tname,"Wrap %d",wrap);
	pbutwrap->settname(tname);
	focus = oldfocus = 0;
	// setup state
	nequs = nvars = width * height;
/*	if (pl) {
		nvars = pl->w;
		nequs = pl->h;
	} */
	amat = new augmatnf(statemod,nequs,nvars,rl); // this will set the default mod for zee2 class
//	amat->setlosums(true);
/*	if (pl) {
		amat->load(pl->data);
		amat->update();
		pl = 0;
	} */
	// boards
	pb = new patternboard("Pattern",810,15,width,height,pattern,wrap);
	if (dooversave) {
		pb->setoverraw(overxsave,overysave);
		dooversave = false;
	}
	gb = new gameboard("Game",810,235,width,height,pattern,wrap,statemod);
	// console
	if (nequs<=12)
		gcon=con32_alloc(600,370,C32BLACK,C32WHITE);
	else if (nequs<=16)
		gcon=con32_alloc(600,270,C32BLACK,C32WHITE);
	else
		gcon=con32_alloc(600,60,C32BLACK,C32WHITE);
	lightup_check();
	gcon_printf("Lightup numvar = %d, numequ = %d\n",nvars,nequs);
}

void lightup_proc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
// handle buttons and editboxes
	if (wininfo.mleftclicks || wininfo.mrightclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
	if (focus == pbutresetboard) {
		if (ret==1 || ret==2) {
			gb->reset();
		}
	} else if (focus == pbutwidth) {
		if (ret==1) {
			gcon_printf("width -\n");
			--width;
			if (width < 1)
				width = 8;
			changestate(STATE_LIGHTUP);
		} else if (ret==2) {
			gcon_printf("width +\n");
			++width;
			if (width > 8)
				width = 1;
			changestate(STATE_LIGHTUP);
		}
	} else if (focus == pbutheight) {
		if (ret==1) {
			gcon_printf("height -\n");
			--height;
			if (height < 1)
				height = 8;
			changestate(STATE_LIGHTUP);
		} else if (ret==2) {
			gcon_printf("height +\n");
			++height;
			if (height > 8)
				height = 1;
			changestate(STATE_LIGHTUP);
		}
	} else if (focus == pbutpattern) {
		if (ret==1) {
			gcon_printf("pattern -\n");
			--pattern;
			if (pattern < 0)
				pattern = maxpatterns-1;
			rememberoversavestate();
			changestate(STATE_LIGHTUP);
		} else if (ret==2) {
			gcon_printf("pattern +\n");
			++pattern;
			if (pattern>=maxpatterns)
				pattern = 0;
			rememberoversavestate();
			changestate(STATE_LIGHTUP);
		}
//			pl = &pl_3x3;
//		}
	} else if (focus == pbutncolors) {
		if (ret==1) {
			gcon_printf("numcolors -\n");
			--statemod;
			if (statemod < 2)
				statemod = zee2::maxmod;
			rememberoversavestate();
			changestate(STATE_LIGHTUP);
		} else if (ret==2) {
			gcon_printf("numcolors +\n");
			++statemod;
			if (statemod > zee2::maxmod)
				statemod = 2;
			rememberoversavestate();
			changestate(STATE_LIGHTUP);
		}
	} else if (focus == pbutwrap) {
		if (ret==1 || ret==2) {
			gcon_printf("wrap toggle\n");
			wrap = 1 - wrap;
//			pl = &pl_4x4;
			rememberoversavestate();
			changestate(STATE_LIGHTUP);
		}
	} else if (focus == pbuttomatrix) {
		if (ret==1 || ret==2) {
			tomatrix(amat,gb);
			amat->update();
		}
	} else if (focus == pbutresetcalc) {
		if (ret==1 || ret==2) {
			amat->reset();
		}
	} else if (focus == pbutstepcalc) {
		if (ret==1 || ret==2) {
//			amat->step();
		}
	} else if (focus == pbutcalc) {
		if (ret==1 || ret==2) {
			con32* gconsave = gcon;
//			gcon = 0;
			S32 timemath2a = getmillisec();
			vector<vector<ring> > vars2 = amat->getvars2(true); // new math
			S32 timemath2b = getmillisec();
			gcon_printf("new math\n");
			gcon_printf("new math error is %d\n",amat->getlasterr());
			amat->printvars(vars2);

/*			S32 timematha = getmillisec();
			vector<vector<ring> > vars = amat->getvars(); // old math
			S32 timemathb = getmillisec();
			gcon_printf("old math\n");
			gcon_printf("old math error is %d\n",amat->getlasterr());
			amat->printvars(vars); */

			gcon = gconsave;
			
			gcon_printf("millisec: new math = %d, new math = %d\n",timemath2b-timemath2a,timemath2b-timemath2a);
//			if (vars == vars2)
//				gcon_printf("match! good\n");
//			else
//				gcon_printf("no match! bad\n");
		}
	} else if (focus == pbutquit) {
		if (ret==1 || ret==2) {
//			poporchangestate(STATE_MAINMENU);
			popstate();
			gcon_printf("quit\n");
		}
	} else {
		if (ret==1 || ret==2) {
			S32 ri,ci;
			if (amat->isfocus(focus,ri,ci)) {
				amat->dofocus(ri,ci,ret);
			}
		}
	}
	if (wininfo.mleftclicks) {
		gb->click(MX,MY,-1);
	}
	if (wininfo.mrightclicks) {
		gb->click(MX,MY,1);
	}
	pb->setovermouse(MX,MY,MBUT);
}

void lightup_draw2d()
{
	clipclear32(B32,C32(0,90,0));
	rl->draw();
	clipline32(B32,
		amat->startx + amat->sepx*amat->nc - (amat->sepx-amat->sizex)/2,
		amat->starty - amat->sepy - (amat->sepy-amat->sizey)/2,
		amat->startx + amat->sepx*amat->nc - (amat->sepx-amat->sizex)/2,
		amat->starty + amat->sepy*amat->nr - (amat->sepy-amat->sizey)/2,
		C32WHITE);
	bitmap32* cb=con32_getbitmap32(gcon);
	clipblit32(cb,B32,0,0,0,WY-cb->size.y,cb->size.x,cb->size.y);
	pb->draw();
	gb->draw();
}

void lightup_exit()
{
	con32_free(gcon);
	gcon = 0;
	delete rl;
	popdir();
	delete amat;
	delete pb;
	delete gb;
}
