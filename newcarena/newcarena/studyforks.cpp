#include <m_eng.h>

#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "rabbits.h"

static pointi2 stloc;
static S32 stdir; // dir is direction user is going, rot is rotation of piece

//enum frk {NONE,STRAIGHT,TURN,BOTH};

struct pcex {
	S32 dist[DIR_NUM];
//	S32 dist2[DIR_NUM];
//	frk forkstate;
};

//struct av {
//	S32 x,y,dir;
//};

pcex trackx[TRACKZ][TRACKX];

void resetvisitors(trk* t)
{
	S32 i,j,k;
	for (j=0;j<TRACKZ;++j) {
		for (i=0;i<TRACKX;++i) {
			for (k=0;k<DIR_NUM;++k) {
				trackx[j][i].dist[k] = BADDIR; // from s/f
				t->pieces[j][i].dist2[k] = BADDIR; // to s/f
				t->pieces[j][i].forkstate = FNONE;
			}
		}
	}
}

static bool getstloc(trk* t) // find first sf piece
{
	S32 i,j;
	for (j=0;j<TRACKZ;++j) {
		for (i=0;i<TRACKX;++i) {
			if (t->pieces[j][i].pt == PCE_STARTFINISH) {
				stloc.x = i;
				stloc.y = j;
				stdir = t->pieces[j][i].rot;
				return true;
			}
		}
	}
	return false;
}

int mm1(int d)
{
	if (d == BADDIR)
		d = -1;
	return d;
}

static void showtrack(trk* t) // 3 by 3 with pt,up,rot  lf,-,rt  fs,dn,-
{
	S32 i,j;
	S32 n;
	logger("showtrack\n");
	for (j=TRACKZ-1;j>=0;--j) { // printing is opposite data in 'z'
		for (n=0;n<3;++n) {
			for (i=0;i<TRACKX;++i) {
				switch(n) {
				case 0:
					logger("%2d ",t->pieces[j][i].pt);
					logger("%2d ",mm1(trackx[j][i].dist[DIR_UP]));
					logger("%2d  ",t->pieces[j][i].rot);
					break;
				case 1:
					logger("%2d ",mm1(trackx[j][i].dist[DIR_LEFT]));
					logger("-- ");
					logger("%2d  ",mm1(trackx[j][i].dist[DIR_RIGHT]));
					break;
				case 2:
					logger("%2d ",t->pieces[j][i].forkstate);
					logger("%2d ",mm1(trackx[j][i].dist[DIR_DOWN]));
					logger("--  ");
					break;
				}
			}
			logger("\n");
		}
		logger("\n");
	}
	logger("\n");
}

static void showtrack2(trk* t) // 3 by 3 with pt,up,rot  lf,-,rt  fs,dn,-
{
	S32 i,j;
	S32 n;
	logger("showtrack2\n");
	for (j=TRACKZ-1;j>=0;--j) { // printing is opposite data in 'z'
		for (n=0;n<3;++n) {
			for (i=0;i<TRACKX;++i) {
				switch(n) {
				case 0:
					logger("%2d ",t->pieces[j][i].pt);
					logger("%2d ",mm1(t->pieces[j][i].dist2[DIR_UP]));
					logger("%2d  ",t->pieces[j][i].rot);
					break;
				case 1:
					logger("%2d ",mm1(t->pieces[j][i].dist2[DIR_LEFT]));
					logger("-- ");
					logger("%2d  ",mm1(t->pieces[j][i].dist2[DIR_RIGHT]));
					break;
				case 2:
					logger("%2d ",t->pieces[j][i].forkstate);
					logger("%2d ",mm1(t->pieces[j][i].dist2[DIR_DOWN]));
					logger("--  ");
					break;
				}
			}
			logger("\n");
		}
		logger("\n");
	}
	logger("\n");
}

static bool advance(S32 ptype,S32 prot,S32& i,S32& j,S32& wdir,bool forkturn)
{
// no error checking
// go with straight part of forks
//	if (ptype==0)
//		logger("ptype=0\n");
//	bool forkturn = true; // if true use the turn part of the fork instead of the straight part
	S32 pdir = rotmod(-prot,wdir);
	S32 newpdir = nextlocsrot[ptype][pdir][0];
	if (newpdir == BADDIR) {
		logger("bad newpdir\n");
		return false;
	}
	if (forkturn) {
		S32 turndir = nextlocsrot[ptype][pdir][1];
		if (turndir != BADDIR) {
			newpdir = turndir;
		}
	}
	wdir = rotmod(prot,newpdir);
	pointi2 move = rot2pointi2(wdir);
	i += move.x;
	j += move.y;
	return true;
}

static void runtrack(trk* t)
{
	S32 nlaps = 1; // laps for the race
	S32 nsteps = 0;
// copy from start
	S32 wlap = 0;
	S32 wdir = stdir;
	S32 i=stloc.x;
	S32 j=stloc.y;
	S32 ptype = t->pieces[j][i].pt;
	S32 prot = t->pieces[j][i].rot;
	logger("step %5d: pos (%d,%d), dir %d, lap %d: type %d, rot %d\n",nsteps,i,j,wdir,wlap,ptype,prot);
// run it
	while(nsteps < 1000) {
		ptype = t->pieces[j][i].pt;
		prot = t->pieces[j][i].rot;
		if (!advance(ptype,prot,i,j,wdir,false)) {
			logger("bad advance\n");
			break;
		}
		S32 newtype = t->pieces[j][i].pt;
		S32 newrot = t->pieces[j][i].rot;
		if (newtype == PCE_STARTFINISH)
			++wlap;
		++nsteps;
		logger("step %5d: pos (%d,%d), dir %d, lap %d: type %d, rot %d\n",nsteps,i,j,wdir,wlap,newtype,newrot);
		if (wlap == nlaps)
			break;
	}
	if (nsteps == 1000)
		logger("max nsteps reached!\n");
}

// paint track with dist from s/f pieces
bool painttrackx(trk* t,S32 pn) // true if new paint applied
{
	if (pn == 0) { // do s/f piece first
		S32 i,j;
		bool hassf = false;
		for (j=0;j<TRACKZ;++j) {
			for (i=0;i<TRACKX;++i) {
				if (t->pieces[j][i].pt == PCE_STARTFINISH) {
					hassf = true;
					S32 st_right = rotmod(t->pieces[j][i].rot,DIR_RIGHT);
					S32 st_left = rotmod(t->pieces[j][i].rot,DIR_LEFT);
					trackx[j][i].dist[st_right] = pn; // 0
					trackx[j][i].dist[st_left] = pn; // 0
				}
			}
		}
		return hassf;
	} else {
		S32 i,j,k;
		S32 olddist = pn - 1;
		bool didpaint = false;
		for (j=0;j<TRACKZ;++j) {
			for (i=0;i<TRACKX;++i) {
				for (k=0;k<DIR_NUM;++k) {
					if (trackx[j][i].dist[k] == olddist) {
						S32 ni1 = i;
						S32 nj1 = j;
						S32 nd1 = k;
						if (advance(t->pieces[j][i].pt,t->pieces[j][i].rot,ni1,nj1,nd1,false)) { // straight in fork
							if (trackx[nj1][ni1].dist[nd1] == BADDIR) {
								trackx[nj1][ni1].dist[nd1] = pn;
								logger("new paint1 (%d,%d,%d) pn %d\n",ni1,nj1,nd1,pn);
								didpaint = true;
							}
						}
						S32 ni2 = i;
						S32 nj2 = j;
						S32 nd2 = k;
						if (advance(t->pieces[j][i].pt,t->pieces[j][i].rot,ni2,nj2,nd2,true)) { // turn in fork
							if (trackx[nj2][ni2].dist[nd2] == BADDIR) {
								trackx[nj2][ni2].dist[nd2] = pn;
								logger("new paint2 (%d,%d,%d) pn %d\n",ni2,nj2,nd2,pn);
								didpaint = true;
							}
						}
					}
				}
			}
		}
		return didpaint;
	}
	return false;
}

// set switches on forks
void doforks(trk* t)
{
	S32 i,j;
	bool hassf = false;
	for (j=0;j<TRACKZ;++j) {
		for (i=0;i<TRACKX;++i) {
			S32 st,tn;
			S32 pt = t->pieces[j][i].pt;
			S32 rt = t->pieces[j][i].rot;
			if (pt == PCE_RIGHTFORK || pt == PCE_LEFTFORK) {
				if (t->pieces[j][i].pt == PCE_RIGHTFORK) {
					st = rotmod(DIR_LEFT,rt);
					tn = rotmod(DIR_UP,rt);
				} else if (t->pieces[j][i].pt == PCE_LEFTFORK) {
					st = rotmod(DIR_LEFT,rt);
					tn = rotmod(DIR_DOWN,rt);
				}
				S32 stc = trackx[j][i].dist[st];
				S32 tnc = trackx[j][i].dist[tn];
				frk fs = FNONE;
				if (stc>tnc) {
					logger("(%d,%d) : stc = %d, tnc = %d, turn\n",i,j,stc,tnc);
					fs = FTURN;
				} else if (stc<tnc) {
					logger("(%d,%d) : stc = %d, tnc = %d, straight\n",i,j,stc,tnc);
					fs = FSTRAIGHT;
				} else {
					logger("(%d,%d) : stc = %d, tnc = %d, both\n",i,j,stc,tnc);
					fs = FBOTH;
				}
				t->pieces[j][i].forkstate = fs;
			}
		}
	}
}

void nextsfdist(trk* t)
{
	S32 i,j;
	for (j=0;j<TRACKZ;++j) {
		for (i=0;i<TRACKX;++i) {
			S32 pt = t->pieces[j][i].pt;
			S32 rot = t->pieces[j][i].rot;
			switch(pt) {
			case PCE_STARTFINISH:
			case PCE_STRAIGHT:
			case PCE_INTERSECTION:
				t->pieces[j][i].dist2[DIR_RIGHT] = trackx[j][i].dist[DIR_LEFT];
				t->pieces[j][i].dist2[DIR_LEFT] = trackx[j][i].dist[DIR_RIGHT];
				t->pieces[j][i].dist2[DIR_UP] = trackx[j][i].dist[DIR_DOWN];
				t->pieces[j][i].dist2[DIR_DOWN] = trackx[j][i].dist[DIR_UP];
				break;
			case PCE_TURN:
				{
					S32 rt = rotmod(DIR_RIGHT,rot); // piece2world
					S32 up = rotmod(DIR_UP,rot);
					t->pieces[j][i].dist2[rt] = trackx[j][i].dist[up];
					t->pieces[j][i].dist2[up] = trackx[j][i].dist[rt];
				}
				break;
			case PCE_RIGHTFORK:
				{
					S32 up = rotmod(DIR_UP,rot);
					S32 upv = trackx[j][i].dist[up];
					S32 lf = rotmod(DIR_LEFT,rot);
					S32 lfv = trackx[j][i].dist[lf];
					S32 rt = rotmod(DIR_RIGHT,rot); // piece2world
					S32 rtv = trackx[j][i].dist[rt];
					S32 mnv = min(upv,lfv);
					t->pieces[j][i].dist2[rt] = mnv;
					t->pieces[j][i].dist2[up] = rtv;
					t->pieces[j][i].dist2[lf] = rtv;
				}
				break;
			case PCE_LEFTFORK:
				{
					S32 dn = rotmod(DIR_DOWN,rot);
					S32 dnv = trackx[j][i].dist[dn];
					S32 lf = rotmod(DIR_LEFT,rot);
					S32 lfv = trackx[j][i].dist[lf];
					S32 rt = rotmod(DIR_RIGHT,rot); // piece2world
					S32 rtv = trackx[j][i].dist[rt];
					S32 mnv = min(dnv,lfv);
					t->pieces[j][i].dist2[rt] = mnv;
					t->pieces[j][i].dist2[dn] = rtv;
					t->pieces[j][i].dist2[lf] = rtv;
				}
				break;
			}
		}
	}
}

void trk::studyforks()
{
	logger("^^ studyforks ^^\n");
#if 1
	S32 pn = 0;
	resetvisitors(this);
	for (pn = 0; pn < 2*TRACKX*TRACKZ; ++pn) {
		logger("painttrack %d\n",pn);
		if (!painttrackx(this,pn)) {
			break;
		}
	}
	doforks(this);
	showtrack(this);
	nextsfdist(this);
	showtrack2(this);
#else
	if (getstloc(this)) {
		showtrack(this);
		runtrack(this);
	}
#endif
	logger("vv studyforks vv\n");
}
