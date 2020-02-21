// matrix tests
#if 1
#include <m_eng.h>
#include <l_misclibm.h>
#include "system/u_states.h"

static S32 nequs = 1; // rows // 5 max
static S32 nvars = 1; // cols // 5 max
// console
//con32* gcon;
#include <math_abstract.h>
#include "ring.h"
#include "augmatn.h"
#include "augmatn_ui.h"
#include "augmatn_float.h"

namespace u_s_mattest {

S32 statemod = 2;

//typedef float ring;
typedef fraction ring;
//typedef zee<5> ring;

typedef augmatn_ui<ring> augmatnf;
augmatnf* amat;


shape* rl;
shape* focus,*oldfocus;
pbut* pbutquit,*pbutreset,*pbutstep,*pbutcalc,*pbutnvar,*pbutnequ,*pbutmod,*pbut4x4,*pbut3x3;

/*
struct preload {
	S32* data;
	S32 w,h;
};

preload* pl;

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
*/
#if 0
void mattest_check()
{
	S32 ngood = 0, nbad = 0;
#if 1
	static bool checked;
	if (checked)
		return;
	checked = true;
#endif
	const S32 cnequs = 3;
	const S32 cnvars = 3;
#if 1
//	const S32 mod = 7;
//	typedef fraction tst;
	typedef float tst;
//	typedef zee<6> tst;
	augmatn<tst> am(cnequs,cnvars);
#else
	const S32 mod = 2;
//	zee2::setmod(mod);
	augmatz am(mod,cnequs,cnvars);
#if 0
	vector<vector<T> > invars;
	vector<T> avar(2);
	avar[0] = 1;
	avar[1] = 1;
	invars.push_back(avar);
	avar[0] = 1;
	avar[1] = 0;
	invars.push_back(avar);
	gcon_printf("in vars\n");
	am.printvars(invars);
	gcon_printf("test vars\n");
	vector<vector<T> > tv;
	tv = am.anyfront(invars,1);
	am.printvars(tv);
#endif
#if 0
	am.setrc(0,0,2);
	am.setrc(0,1,3);
	am.setrc(0,2,5);
	am.setrc(1,0,3);
	am.setrc(1,1,2);
	am.setrc(1,2,5);
#endif
#if 0
	am.setrc(0,0,4);
	am.setrc(1,0,2);
#endif
#endif
//	typedef ring tst;
	vector<vector<tst> > vars1;
//	vector<vector<tst> > varsbf;
//	vector<vector<tst> > varscomb;
//	vector<vector<tst> > varscomb2;
	vector<vector<tst> > vars2;
	vector<tst> mult;
	vector<tst> nonhom;
	S32 cnt = 0;
	S32 tima,timb,del;
	tima = getmillisec();
	S32 tot1=0,tot2=0;
#if 1
//	logger
	do {
		con32* gconsave = gcon;
		gcon = 0;
		gcon_printf("\n========== am ===========\n");
		am.print();

		S32 time1s = getmillisec();
		vars1 = am.getvars2(); // list of vars, new math
		S32 time1e = getmillisec();
		tot1 += (time1e - time1s);

		S32 time2s = time1e; // getmillisec();
		vars2 = am.getvars();  // list of vars, old math
		S32 time2e = getmillisec();
		tot2 += (time2e - time2s);

//		vars2 = am.getvars(); // list of vars, old math
//		vars2 = am.getvars2(); // list of vars, new math
//		vars2 = am.getvarsbrute();  // list of vars, brute force

//		if (false) { // force bad
//		if (true) { // force good
		if (vars1 == vars2) { // normal, force check
			gcon_printf("========== check good at ===========\n");
			++ngood;
		} else {
			gcon = gconsave;

			gcon_printf("\n========== am ===========\n");
			am.print();

			gcon_printf("========== check bad at ===========\n");
			++nbad;
		}
		gcon_printf("vars1\n");
		amat->printvars(vars1);
		gcon_printf("vars2\n");
		amat->printvars(vars2);
/*		if (vars2 == varsbf) { // normal, force check
//			gcon_printf("========== check good at ===========\n");
		} else {
			gcon_printf("========== check 2 bad at ===========\n");
			am.print();
			gcon_printf("vars2 math\n");
			for (i=0;i<(S32)vars2.size();++i) {
				amat->printvec(vars2[i]);
			}
			gcon_printf("vars brute\n");
			for (i=0;i<(S32)varsbf.size();++i) {
				amat->printvec(varsbf[i]);
			}
		} */
/*		S32 j;
		for (j=0;j<mod;++j) {
			gcon = 0;
			varscomb = am.getvarscomb(j); // list of vars, math
			varscomb2 = am.getvarscomb2(j); // list of vars, math
			gcon = gconsave;
			if (vars == varscomb) { // normal, force check
				// gcon_printf("========== check2 good at ===========\n");
			} else {
				gcon_printf("========== check2 bad at mul %d ===========\n",j);
				am.print();
				gcon_printf("vars math\n");
				for (i=0;i<(S32)vars.size();++i) {
					amat->printvec(vars[i]);
				}
				gcon_printf("vars comb math\n");
				for (i=0;i<(S32)varscomb.size();++i) {
					amat->printvec(varscomb[i]);
				}
			}
			if (vars == varscomb2) { // normal, force check
				// gcon_printf("========== check3 good at ===========\n");
			} else {
				gcon_printf("========== check3 bad at mul %d ===========\n",j);
				am.print();
				gcon_printf("vars math\n");
				for (i=0;i<(S32)vars.size();++i) {
					amat->printvec(vars[i]);
				}
				gcon_printf("vars comb2 math\n");
				for (i=0;i<(S32)varscomb2.size();++i) {
					amat->printvec(varscomb2[i]);
				}
			}
		} */
		gcon = gconsave;
		if ((cnt%1000)==0) {
			timb = getmillisec();
			del = timb - tima;
			tima = timb;
			gcon_printf("cnt = %d, ms = %d\n",cnt,del);
			video_lock();
			clipblit32(con32_getbitmap32(gcon),B32,0,0,0,0,gcon->b32->size.x,gcon->b32->size.y);
			video_unlock();
			checkmessages();
			if (wininfo.closerequested)
				break;
			video_paintwindow(0);
		}
		++cnt;
//	} while(false);
	} while(am.iter());
	gcon_printf("good = %d, bad = %d\n",ngood,nbad);
	gcon_printf("time1 = %d, time2 = %d\n",tot1,tot2);
#else
//	logger
	do {
		am.setrc(0,0,5);
		am.setrc(0,1,5);
		nonhom = am.getnonhom();
		vars = am.getvarsc(); // calc. v0 + C1*v1 + C2*v2 etc. if size == 0 then no solution, size == 1 one solution
		if (vars.size())
			mult = am.getmult(vars[0]);
//		if (true) {
		if (!vars.size()) {
			gcon_printf("========== no solution at ===========\n");
		} else if (nonhom != mult) {
			gcon_printf("========== check bad at ===========\n");
		} else {
			gcon_printf("========== check good at ===========\n");
		}
		am.print();
		gcon_printf("vars\n");
		S32 i;
		for (i=0;i<(S32)vars.size();++i) {
			amat->printvec(vars[i]);
		}
		gcon_printf("-------\n");
		gcon_printf("nonhom ");
		am.printvec(nonhom);
		if (vars.size()) {
			gcon_printf("mult   ");
			am.printvec(mult);
			gcon_printf("\n");
		}
	} while(am.iter());
#endif
}
#endif
} // end namespace u_s_mattest
using namespace u_s_mattest;

void mattest_init()
{
	video_setupwindow(800,600);
	pushandsetdir("mattest");
	script sc("browserres.txt");
	rl=res_loadfile("mattest.txt");
	pbutquit=rl->find<pbut>("PBUTQUIT");
	pbutreset=rl->find<pbut>("PBUTRESET");
	pbutstep=rl->find<pbut>("PBUTSTEP");
	pbutcalc=rl->find<pbut>("PBUTCALC");
	pbutnvar=rl->find<pbut>("PBUTNVAR");
	pbutnequ=rl->find<pbut>("PBUTNEQU");
	pbutmod=rl->find<pbut>("PBUTMOD");
	pbut4x4=rl->find<pbut>("PBUT4X4");
	pbut3x3=rl->find<pbut>("PBUT3X3");
	C8 tname[100];
	sprintf(tname,"Mod %d",statemod);
	pbutmod->settname(tname);
//	pbutele=rl->find<pbut>("1 1");
//	zee2::setmod(statemod);
/*	if (pl) {
		nvars = pl->w;
		nequs = pl->h;
	} */
	amat = new augmatnf(nequs,nvars,rl);
/*	if (pl) {
		amat->load(pl->data);
		amat->update();
		pl = 0;
	} */
	focus = oldfocus = 0;
	if (nequs>12)
		gcon=con32_alloc(600,270,C32BLACK,C32WHITE);
	else
		gcon=con32_alloc(600,370,C32BLACK,C32WHITE);
//	mattest_check();
	gcon_printf("Mattest numvar = %d, numequ = %d\n",nvars,nequs);
}

void mattest_proc()
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
	if (focus == pbutquit) {
		if (ret==1 || ret==2) {
//			poporchangestate(STATE_MAINMENU);
			popstate();
			gcon_printf("quit\n");
		}
	} else if (focus == pbutreset) {
		if (ret==1 || ret==2) {
			amat->reset();
		}
	} else if (focus == pbutstep) {
		if (ret==1 || ret==2) {
//			amat->step();
		}
	} else if (focus == pbutcalc) {
		if (ret==1 || ret==2) {
//			amat->calc();
/*			vector<vector<ring> > vars = amat->getvarsc();
			gcon_printf("varsc\n");
			for (i=0;i<vars.size();++i) {
				amat->printvec(vars[i]);
			} */
			con32* gconsave = gcon;
//			gcon = 0;
			S32 timematha = getmillisec();
			vector<vector<ring> > vars1 = amat->getvarsc();
			S32 timemathb = getmillisec();
			gcon_printf("varssc\n");
			amat->printvars(vars1);

/*			S32 timebrutea = getmillisec();
			vector<vector<ring> > vars2 = amat->getvars();
			S32 timebruteb = getmillisec();
			gcon_printf("vars2\n");
			amat->printvars(vars2); */

			gcon = gconsave;

			gcon_printf("millisec: math = %d\n",timemathb-timematha);
/*			if (vars1 == vars2)
				gcon_printf("match! good\n");
			else
				gcon_printf("no match! bad\n"); */
		}
/*	} else if (focus == pbut4x4) {
		if (ret==1 || ret==2) {
			gcon_printf("4x4\n");
			pl = &pl_4x4;
			changestate(STATE_MATTEST);
		}
	} else if (focus == pbut3x3) {
		if (ret==1 || ret==2) {
			gcon_printf("3x3\n");
			pl = &pl_3x3;
			changestate(STATE_MATTEST);
		} */
	} else if (focus == pbutnvar) {
		if (ret==1) {
			gcon_printf("nvars -\n");
			--nvars;
			if (nvars < 1)
				nvars = 5;
			changestate(STATE_MATTEST);
		} else if (ret==2) {
			gcon_printf("nvars +\n");
			++nvars;
			if (nvars > 5)
				nvars = 1;
			changestate(STATE_MATTEST);
		}
	} else if (focus == pbutnequ) {
		if (ret==1) {
			gcon_printf("nequs -\n");
			--nequs;
			if (nequs < 1)
				nequs = 5;
			changestate(STATE_MATTEST);
		} else if (ret==2) {
			gcon_printf("nequs +\n");
			++nequs;
			if (nequs > 5)
				nequs = 1;
			changestate(STATE_MATTEST);
		}
/*	} else if (focus == pbutmod) {
		if (ret==1) {
			--statemod;
			if (statemod < 2)
				statemod = zee2::maxmod;
			changestate(STATE_MATTEST);
		} else if (ret==2) {
			++statemod;
			if (statemod > zee2::maxmod)
				statemod = 2;
			changestate(STATE_MATTEST);
		} */
	} else {
		if (ret==1 || ret==2) {
			S32 ri,ci;
			if (amat->isfocus(focus,ri,ci)) {
				amat->dofocus(ri,ci,ret);
			}
		}
	}
}

void mattest_draw2d()
{
	clipclear32(B32,C32BLUE);
	rl->draw();
	clipline32(B32,
		amat->startx + amat->sepx*amat->nc - (amat->sepx-amat->sizex)/2,
		amat->starty - amat->sepy - (amat->sepy-amat->sizey)/2,
		amat->startx + amat->sepx*amat->nc - (amat->sepx-amat->sizex)/2,
		amat->starty + amat->sepy*amat->nr - (amat->sepy-amat->sizey)/2,
		C32WHITE);
	bitmap32* cb=con32_getbitmap32(gcon);
	clipblit32(cb,B32,0,0,0,WY-cb->size.y,cb->size.x,cb->size.y);
}

void mattest_exit()
{
	con32_free(gcon);
	gcon = 0;
	delete rl;
	popdir();
	delete amat;
}
#else
void mattest_init()
{
}
void mattest_proc()
{
}

void mattest_draw2d()
{
}

void mattest_exit()
{
}
#endif
