#define D2_3D
#include <m_eng.h>
#include "m_perf.h"

static S32 curstate=NOSTATE;
static S32 nextstate;
static S32 dochangestate;

static S32 nstatestack;
#define MAXSTATESTACK 8
static S32 statestack[MAXSTATESTACK];

void changestate(S32 ns)
{
	nextstate=ns;
	dochangestate=1;
}

void pushchangestate(S32 ns)
{
	if (nstatestack>=MAXSTATESTACK)
		errorexit("pushchangestate stack overflow");
	statestack[nstatestack++]=curstate;
	changestate(ns);
}

void popstate()
{
	S32 ns;
	if (nstatestack) {
		ns=statestack[--nstatestack];
	} else {
		ns=NOSTATE;
	}
	changestate(ns);
}

void poporchangestate(S32 ns)
{
	if (nstatestack)
		popstate();
	else
		changestate(ns);
}

bool stateproc()
{
	if (wininfo.closerequested && !dochangestate) // only close window after states don't change (see goingtogame)
		changestate(NOSTATE);
	if (dochangestate) {
		if (curstate!=(S32)NOSTATE && curstate<numstates) {
			logger("begin exit state %d\n",curstate);
			logger_indent();
			perf_start(STATEEXIT);
			if (states[curstate].exitfunc)
				states[curstate].exitfunc();
			perf_end(STATEEXIT);
			logger_unindent();
			logger("end exit state %d\n",curstate);
		}
		curstate=nextstate;
		dochangestate = 0; // incase init changes state
		if (curstate!=(S32)NOSTATE && curstate<numstates) {
			logger("begin init state %d\n",curstate);
			logger_indent();
			perf_start(STATEINIT);
			if (states[curstate].initfunc)
				states[curstate].initfunc();
			perf_end(STATEINIT);
			logger_unindent();
			logger("end init state %d\n",curstate);
			wininfo.slacktime=0;	// don't need to catchup when switching states.
		}
		//dochangestate=0; // moved up before initfunc
	}
	if (curstate==(S32)NOSTATE || curstate>=numstates)
		return 0;
//	logger_indent();
//	logger("proc state %d\n",curstate);
	if (states[curstate].procfunc) {
		states[curstate].procfunc();
//		logger_unindent();
		textureb::animtex(); // okay, for now, call this here, maybe later put in tree2::proc() or seq_dostep()
		return true;
	}
//	logger_unindent();
	// state has no proc function, gracefully exit
	return false;
}

void statedraw3d()
{
	if (curstate==(S32)NOSTATE || curstate>=numstates)
		return;
	if (!states[curstate].draw3dfunc)
		return;
//	logger_indent();
//	logger("draw3d state %d\n",curstate);
	perf_start(STATEDRAW3D);
	states[curstate].draw3dfunc();
	perf_end(STATEDRAW3D);
//	logger_unindent();
}

void statedraw2d()
{
	if (curstate==(S32)NOSTATE || curstate>=numstates)
		return;
	if (!states[curstate].draw2dfunc)
		return;
//	logger_indent();
//	logger("draw2d state %d\n",curstate);
	perf_start(STATEDRAW2D);
	states[curstate].draw2dfunc();
	perf_end(STATEDRAW2D);
//	logger_unindent();
}

bool statehasdraw3d()
{
	if (curstate==(S32)NOSTATE || curstate>=numstates)
		return false;
	return states[curstate].draw3dfunc!=0;
}

bool statehasdraw2d()
{
	if (curstate==(S32)NOSTATE || curstate>=numstates)
		return false;
	return states[curstate].draw2dfunc!=0;
}
