#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "carclass.h"
#include "timewarp.h"

#include "netdescj.h"
#include "envj.h"
#include "avaj.h"
#include "gamerunj.h"
#include "carenagame.h" // for predicted
#include "rematch.h"

#include "physics.h"

// backed up into trk_line2road copy

// do it by hand, all multiple of 90 degrees
static pointf3 world2piece(const pointf3& w,S32 rot)
{
	pointf3x p;
	switch(rot) {
	case 0:
		p.x=w.x;
		p.z=w.z;
		break;
	case 1:
		p.x=PIECESIZE-w.z;
		p.z=w.x;
		break;
	case 2:
		p.x=PIECESIZE-w.x;
		p.z=PIECESIZE-w.z;
		break;
	case 3:
		p.x=w.z;
		p.z=PIECESIZE-w.x;
		break;
	}
	p.y = w.y;
	return p;
}

static pointf3 normworld2piece(const pointf3& w,S32 rot)
{
	pointf3x p;
	switch(rot) {
	case 0:
		p.x=w.x;
		p.z=w.z;
		break;
	case 1:
		p.x=-w.z;
		p.z=w.x;
		break;
	case 2:
		p.x=-w.x;
		p.z=-w.z;
		break;
	case 3:
		p.x=w.z;
		p.z=-w.x;
		break;
	}
	p.y = w.y;
	return p;
}

#if 0
static pointf3 piece2world(const pointf3& p,S32 rot)
{
	pointf3x w;
	switch(rot) {
	case 0:
		w.x=p.x;
		w.z=p.z;
		break;
	case 1:
		w.x=p.z;
		w.z=PIECESIZE-p.x;
		break;
	case 2:
		w.x=PIECESIZE-p.x;
		w.z=PIECESIZE-p.z;
		break;
	case 3:
		w.x=PIECESIZE-p.z;
		w.z=p.x;
		break;
	}
	return w;
}
#endif
static pointf3 normpiece2world(const pointf3& p,S32 rot)
{
	pointf3x w;
	switch(rot) {
	case 0:
		w.x=p.x;
		w.z=p.z;
		break;
	case 1:
		w.x=p.z;
		w.z=-p.x;
		break;
	case 2:
		w.x=-p.x;
		w.z=-p.z;
		break;
	case 3:
		w.x=-p.z;
		w.z=p.x;
		break;
	}
	w.y = p.y;
	return w;
}

static float widseg2wid(S32 s)
{
	return PIECESIZE*.25f+(PIECESIZE/6.0f)*s;
}
#ifndef FULLPHYSICS
// return true if penetrates into solid walls, mostly ignoring oldpnt (except for bestrot), setting new point y to current height of road, (2d ish physics)
// bool trk::line2road(const pointf3* oldpnt,const pointf3* newpnt,const pointf3* oldrot,pointf3* bestintsect,pointf3* bestnorm,pointf3* bestrot) const
bool trk::line2roadxz(pointf3* pnt,pointf3* rot,pointf3* normjj) const // reads and writes both pnt and rot, writes normjj
{
	static const float sqrt2o2 = sqrtf(2.0f)*.5f; // static const gets set first time function line2road called fpu CR is set right
	static const float sqrt5o5 = sqrtf(5.0f)/5.0f; // static gets set first time function line2road called fpu CR is set right
	static const float ips = 1.0f/PIECESIZE;
	static const float pieceheightper = sqrtf(6.0f)/6.0f*PIECESIZE/6.0f;

	static const pointf3x norm2(-sqrtf(10.0f)/5.0f,sqrtf(15.0f)/5.0f,0);
	static const pointf3x norm1(-sqrtf(7.0f)/7.0f,sqrtf(42.0f)/7.0f,0);
//	static const pointf3x norm0(0,1,0);
	static const pointf3x normm1(sqrtf(7.0f)/7.0f,sqrtf(42.0f)/7.0f,0);
	static const pointf3x normm2(sqrtf(10.0f)/5.0f,sqrtf(15.0f)/5.0f,0);
// clamping
	float x = max(0.0f,pnt->x); // new
	float z = max(0.0f,pnt->z);
// for now, new/bot to piece, upgrade could be multiple pieces, (line spanning pieces)
	S32 px=int(x*ips);
	S32 pz=int(z*ips);
	pointf3x offwb;
// clamping and offset
	if (px>=TRACKX) {
		px=TRACKX-1;
		offwb.x=PIECESIZE;
	} else {
		offwb.x=x-px*PIECESIZE;
	}
	if (pz>=TRACKZ) {
		pz=TRACKZ-1;
		offwb.z=PIECESIZE;
	} else {
		offwb.z=z-pz*PIECESIZE;
	}
// get data from track for this piece
	const piece& p=pieces[pz][px];
	pointf3 offpb = world2piece(offwb,p.rot);
	if (p.pt == PCE_LEFTFORK)
		offpb.z = PIECESIZE - offpb.z;
//	con32_printf(gg->con,"piece (%d,%d):type %d,r %d, o %6.2f %6.2f\n",px,pz,p.pt,p.rot,offpb.x,offpb.z);
	struct normd {
		pointf3 norm;
		float dist;
	};
	enum {D_LEFT,D_RIGHT,D_BOT,D_TOP};//,D_INTURN,D_OUTTURN};
	static normd norms[]={
		{{ 1,0, 0,0},PIECESIZE*.25f},  // left
		{{-1,0, 0,0},-PIECESIZE*.75f}, // right
		{{ 0,0, 1,0},PIECESIZE*.25f},  // bot
		{{ 0,0,-1,0},-PIECESIZE*.75f}, // top
	};
//	const U32 nnorms = sizeof(norms)/sizeof(norms[0]);
//	float outsb[nnorms];
//	U32 i;
// calc out out val from straight lines, out = -penetration
/*	for (i=0;i<nnorms;++i) {
		outsb[i] = dot3d(&norms[i].norm,&offpb)-norms[i].dist;
	} */
	float bestout = 0;
	pointf3x norm; // initialized norm, for normalize3d
	pointf3x normj(0,1,0); // for hills
// do heights first
	S32 idx = 0;
	float tween = 0.0f;
	float frac = 0.0f;
	switch(p.pt) {
	case PCE_STRAIGHT:
		{
//			ooffpb;
			idx = int(offpb.x*6/PIECESIZE);
			idx = range(0,idx,5);
			frac = offpb.x - float(idx)*PIECESIZE/6.0f;
			tween = frac * 6.0f / PIECESIZE; 
			pnt->y = ((1-tween)* p.heightsegs[idx] + tween * p.heightsegs[idx+1]) * pieceheightper;
			S32 deltaj = p.heightsegs[idx+1]-p.heightsegs[idx];
			if (deltaj == 0) {
				;//normj = norms[D_TOP].norm;
			} else if (deltaj == 1) {
				normj = norm1;
			} else if (deltaj == 2) {
				normj = norm2;
			} else if (deltaj == -1) {
				normj = normm1;
			} else { // deltaj == -2
				normj = normm2;
			}
			normj = normpiece2world(normj,p.rot);
//			if (!gg->predicted) {
//				gg->con->printf("nj %f %f %f",normj.x,normj.y,normj.z);
//			} 
		}
		break;
	case PCE_STARTFINISH:
	case PCE_INTERSECTION:
	case PCE_TURN:
	case PCE_RIGHTFORK:
	case PCE_LEFTFORK:
	case PCE_BLANK:
		pnt->y = pieceheightper * p.heightsegs[3];
		break;
	}
	if (normjj)
		*normjj=normj;
	switch(p.pt) {
	case PCE_STRAIGHT:
		{
#if 1
// do it by hand, might need more refinement
			float bot0 = widseg2wid(p.rightwidsegs[idx]);
			float top0 = widseg2wid(p.leftwidsegs[idx]);
//			float bot1 = widseg2wid(p.rightwidsegs[idx+1]);
//			float top1 = widseg2wid(p.leftwidsegs[idx+1]);
//			float bot = (1-tween)*bot0 + tween*bot1;
//			float top = (1-tween)*top0 + tween*top1;
//			float middle = (bot + top) *.5f;
			S32 topdelta = p.leftwidsegs[idx+1]-p.leftwidsegs[idx];
			S32 botdelta = p.rightwidsegs[idx+1]-p.rightwidsegs[idx];
			pointf3x normt,normb;
			if (topdelta == 0) {
				normt = norms[D_TOP].norm;
			} else if (topdelta == 1) {
				normt = pointf3x(sqrt2o2,0,-sqrt2o2);
			} else { // topdelta == -1
				normt = pointf3x(-sqrt2o2,0,-sqrt2o2);
			}
			if (botdelta == 0) {
				normb = norms[D_BOT].norm;
			} else if (botdelta == 1) {
				normb = pointf3x(-sqrt2o2,0,sqrt2o2);
			} else { // botdelta == -1
				normb = pointf3x(sqrt2o2,0,sqrt2o2);
			}
			pointf3x pnttop0(0.0f,0.0f,top0);
			pointf3x pntbot0(0.0f,0.0f,bot0);
			float dt=dot3d(&pnttop0,&normt);
			float db=dot3d(&pntbot0,&normb);
			pointf3x q(frac,0.0f,offpb.z);
			float bestoutt = dot3d(&normt,&q) - dt;
			float bestoutb = dot3d(&normb,&q) - db;
			if (bestoutt < bestoutb) {
				bestout = bestoutt;
				norm = normt;
			} else {
				bestout = bestoutb;
				norm = normb;
			}
/*			if (!gg->predicted) {
				gg->con->printf("nt %f %f %f",normt.x,normt.y,normt.z);
				gg->con->printf("nb %f %f %f",normb.x,normb.y,normb.z);
				gg->con->printf("bt %f",bestoutt);
				gg->con->printf("bb %f",bestoutb);
			} */
			if (bestout>=0) {
				return false; // is out; (is on the road)
			}
#else
// do it by hand, might need more refinement
			float bot0 = widseg2wid(p.rightwidsegs[idx]);
			float top0 = widseg2wid(p.leftwidsegs[idx]);
			float bot1 = widseg2wid(p.rightwidsegs[idx+1]);
			float top1 = widseg2wid(p.leftwidsegs[idx+1]);
			float bot = (1-tween)*bot0 + tween*bot1;
			float top = (1-tween)*top0 + tween*top1;
			float middle = (bot + top) *.5f;
			if (offpb.z >= middle) {
				norm = norms[D_TOP].norm;
				bestout = top - offpb.z;
			} else {
				norm = norms[D_BOT].norm;
				bestout = offpb.z - bot;
			}
			if (bestout>=0) {
				return; // is out;
			}
#endif
		}
		break;
	case PCE_STARTFINISH:
		{
// do it by hand
			float bot = widseg2wid(p.rightwidsegs[0]);
			float top = widseg2wid(p.leftwidsegs[0]);
			float middle = (bot + top) *.5f;
			if (offpb.z >= middle) {
				norm = norms[D_TOP].norm;
				bestout = top - offpb.z;
			} else {
				norm = norms[D_BOT].norm;
				bestout = offpb.z - bot;
			}
			if (bestout>=0) {
				return false; // is out;
			}
		}
		break;
	case PCE_TURN:
// do it by hand
		{
			float rad = sqrtf(offpb.x*offpb.x+offpb.z*offpb.z);
			float inner = widseg2wid(p.rightwidsegs[0]);
			float outer = widseg2wid(p.leftwidsegs[0]);
			float middle = (inner + outer) *.5f;
// 2 sections
			norm.x=-offpb.x;
			norm.z=-offpb.z;
			normalize3d(&norm,&norm);
			if (rad>=middle) { // close to outer circle
				bestout = outer-rad;
			} else { // close to inner circle
				norm.x=offpb.x;
				norm.z=offpb.z;
				normalize3d(&norm,&norm);
				bestout = rad-inner;
			}
#ifdef DOCON
			con32_printf(gg->con,"turn : ");
#endif
			if (bestout>=0) {
#ifdef DOCON
				con32_printf(gg->con,"\n");
#endif
				return false;
			}
		}
		break;
	case PCE_INTERSECTION:
		{
// do it by hand
// 8 sections, like an 8 slice pizza
			S32 oct = 0;
			float bot = widseg2wid(p.rightwidsegs[0]);
			float top = widseg2wid(p.leftwidsegs[0]);
			float middle = (bot + top) *.5f;
//			static const S32 octtab[8]={D_TOP,D_RIGHT,D_LEFT,D_TOP,D_BOT,D_LEFT,D_RIGHT,D_BOT};
			if (offpb.x >= middle) { // right
				if (offpb.z >= middle) { // right up
					if (offpb.x>=offpb.z) { // right up right
						oct = D_TOP;
					} else { // right up up
						oct = D_RIGHT;
					}
				} else { // right down
					if (offpb.x >= 2*middle - offpb.z) { // right down right
						oct = D_BOT;
					} else { // right down down
						oct = D_RIGHT;
					}
				}
			} else { // left
				if (offpb.z >= middle) { // left up
					if (offpb.x >= 2*middle - offpb.z) { // left up up
						oct = D_LEFT;
					} else { // left up left
						oct = D_TOP;
					}
				} else { // left down
					if (offpb.x>=offpb.z) { // left down down
						oct = D_LEFT;
					} else { // left down left
						oct = D_BOT;
					}
				}
			}
			norm = norms[oct].norm;
			switch(oct) {
			case D_LEFT:
				bestout = offpb.x - bot;
				break;
			case D_RIGHT:
				bestout = top - offpb.x;
				break;
			case D_BOT:
				bestout = offpb.z - bot;
				break;
			case D_TOP:
				bestout = top - offpb.z;
				break;
			}
//			S32 bestoutidx = octtab[oct];
//			bestout = outsb[oct];
//			norm=normpiece2world(norm,p.rot);
#ifdef DOCON
			con32_printf(gg->con,"intsect : ");
#endif
			if (bestout>=0) {
#ifdef DOCON
				con32_printf(gg->con,"\n");
#endif
				return false;
			}
		}
		break;
	case PCE_LEFTFORK:
	case PCE_RIGHTFORK:
		{
// do it by hand
// 4 sections
// 2 straignt sections
			float bot,top;
			if (p.pt == PCE_LEFTFORK) {
				top = widseg2wid(3-p.rightwidsegs[0]);
				bot = widseg2wid(3-p.leftwidsegs[0]);
			} else {
				bot = widseg2wid(p.rightwidsegs[0]);
				top = widseg2wid(p.leftwidsegs[0]);
			}
			float middle = (bot + top) *.5f;
			float inner,outer;
//			if (p.pt == PCE_LEFTFORK) {
//				inner = top;
//				outer = bot;
//			} else {
				inner = bot;
				outer = top;
//			}
			if (offpb.z >= bot) { // straight part
//				S32 two = 0;
//				S32 bestoutidx;
				if (offpb.z >= middle) { // top
//					two = 0;
//					bestoutidx = D_TOP;
					bestout = top - offpb.z;
					norm = norms[D_TOP].norm;
#ifdef DOCON
					con32_printf(gg->con,"T ");
#endif
				} else { // bot
//					two = 1;
//					bestoutidx = D_BOT;
					bestout = offpb.z - bot;
					norm = norms[D_BOT].norm;
#ifdef DOCON
					con32_printf(gg->con,"B ");
#endif
				}
//				static const S32 twotab[2]={D_TOP,D_BOT};
//				S32 bestoutidx = twotab[two];
//				bestout = outsb[bestoutidx];
//				norm = norms[bestoutidx].norm;
			} else { // bot bot part
				float rad = sqrtf(offpb.x*offpb.x+offpb.z*offpb.z);
// 2 turn sections
				if (rad>=middle) { // close to outer circle, check against bottom of straight
					float bestoutstraight = offpb.z - bot;
					float bestoutturn = outer - rad;
					if (bestoutstraight >= bestoutturn) { // go with straight
						bestout = bestoutstraight;
						norm = norms[D_BOT].norm;
#ifdef DOCON
						con32_printf(gg->con,"B2 ");
#endif
					} else { // turn
						bestout = bestoutturn;
						norm.x = -offpb.x;
						norm.z = -offpb.z;
						normalize3d(&norm,&norm);
#ifdef DOCON
						con32_printf(gg->con,"O ");
#endif
					}
				} else { // close to inner circle, go with circle
					norm.x = offpb.x;
					norm.z = offpb.z;
					normalize3d(&norm,&norm);
					bestout = rad - inner;
#ifdef DOCON
					con32_printf(gg->con,"I ");
#endif
				}
			}
			if (p.pt == PCE_LEFTFORK) {
				norm.z = -norm.z;
				offpb.z = PIECESIZE - offpb.z;
			}
#ifdef DOCON
			con32_printf(gg->con,"rightfork : ");
#endif
			if (bestout>=0) {
#ifdef DOCON
				con32_printf(gg->con,"\n");
#endif
				return false;
			}
		}
		break;
	case PCE_BLANK:
	default:
//#define SOLIDBLANK
#ifdef SOLIDBLANK
		if (bestintsect)
			*bestintsect=*top;
		if (bestnorm)
			*bestnorm=pointf3x(0,0,1,0);
		return true; // don't be on blank
#else
#ifdef DOCON
		con32_printf(gg->con,"blank\n");
#endif
		return false; // free roam
#endif
	}
// penatration, needs adjustments (pos,rot)
//	offpb=piece2world(offpb,p.rot);
	norm=normpiece2world(norm,p.rot);
#ifdef DOCON
	con32_printf(gg->con,"off (%5.3f,%5.3f), out %5.3f, norm (%5.3f,%5.3f)\n",
		offpb.x,offpb.z,bestout,norm.x,norm.z);
#endif
#if 1
		nanerr("bestout",bestout);
		nanerr("normx",norm.x);
		nanerr("normy",norm.y);
		nanerr("normz",norm.z);
		nanerr("pntx0",pnt->x);
		nanerr("pnty0",pnt->y);
		pnt->x -= norm.x*bestout;
		pnt->z -= norm.z*bestout;
		nanerr("pntx1",pnt->x);
		nanerr("pnty1",pnt->y);
#else
	if (bestintsect) {
		*bestintsect = *pnt;
		bestintsect->x -= norm.x*bestout;
		bestintsect->z -= norm.z*bestout;
//		bestintsect->x -= 2*norm.x*bestout;
//		bestintsect->z -= 2*norm.z*bestout;
	}
	if (bestnorm) {
		*bestnorm = norm;
	}
#endif
#if 1
	if (rot) {
		pointf3xh up(0,0,1,0);
		pointf3xh tangent(norm.z,0,-norm.x); // 90 deg to norm
//		pointf3xh tangent(norm.x,0,norm.z); // 0 deg to norm
		pointf3xh head(0,0,1,0);
		quatrot(rot,&head,&head);
//		pointf3xh head(newpnt->x-oldpnt->x,0,newpnt->z-oldpnt->z);
//		pointf3x head(0,0,1,0);
//		quatrot(oldrot,&head,&head);
		float glance = dot3d(&head,&tangent);
		if (glance<0) {
			tangent.x = -tangent.x;
			tangent.z = -tangent.z;
		}
//		if (!gg->predicted)
//			logger(" up '%s', tan '%s'\n",up.log().c_str(),tangent.log().c_str());
		normal2quaty(&up,&tangent,rot);
//		if (!gg->predicted)
//			logger(" bestrot '%s'\n",((pointf3xh*)bestrot)->log().c_str());
//		*bestrot = pointf3x();
	}
#else
	if (bestrot) {
	}
#endif
	return true;
}


#else

static bool checkline(const pointf2* p0,const pointf2* p1,const pointf2* pt,pointf2* pn)
{
	pointf2x tmp0(pt->x-p0->x,pt->y-p0->y);
	pointf2x p1mp0(p1->x-p0->x,p1->y-p0->y);
	float k=dot2d(&tmp0,&p1mp0)/dot2d(&p1mp0,&p1mp0);
	if (k>=0 && k<=1.0f) {
		float omk = 1.0f - k;
		pn->x = omk*p0->x + k*p1->x;
		pn->y = omk*p0->y + k*p1->y;
		return true;
	}
	return false;

}

static float xwid;
static float yhit;
static bool pnt2line(const S32* ipnts,S32 npnts,const pointf2* opnt,pointf2* npnt)
{
	pointf2x bestpnt(0,ipnts[0]*yhit);
	float bestdistsq = dist2dsq(&bestpnt,opnt);
	S32 i;
	for (i=1;i<npnts;++i) {
		pointf2x p0(i*xwid,ipnts[i]*yhit);
//		pointf2x p1((i+1)*xwid,ipnts[i+1]*yhit);
		float distsq = dist2dsq(&p0,opnt);
		if (distsq < bestdistsq) {
			bestdistsq = distsq;
			bestpnt = p0;
		}
	}
	for (i=0;i<npnts-1;++i) {
		pointf2 np;
		pointf2x p0(i*xwid,ipnts[i]*yhit);
		pointf2x p1((i+1)*xwid,ipnts[i+1]*yhit);
		if (checkline(&p0,&p1,opnt,&np)) {
			float distsq = dist2dsq(&np,opnt);
			if (distsq < bestdistsq) {
				bestdistsq = distsq;
				bestpnt = np;
			}
		}
	}
	if (opnt->y>bestpnt.y) { // if above road, no loops for now
		*npnt = *opnt;
		return false;
	}
	*npnt = bestpnt;
	return true;
}

// return true if penetrates into solid walls or floor, mostly ignoring oldpnt (except for bestrot)
// bool trk::line2road(const pointf3* oldpnt,const pointf3* newpnt,const pointf3* oldrot,pointf3* bestintsect,pointf3* bestnorm,pointf3* bestrot) const
// not so good on inter piece calcs
bool trk::line2road(pointf3* pnt,pointf3* normjj) const // reads and writes both pnt and rot, writes normjj
{
	static const float sqrt2o2 = sqrtf(2.0f)*.5f; // static const gets set first time function line2road called fpu CR is set right
	static const float sqrt5o5 = sqrtf(5.0f)/5.0f; // static gets set first time function line2road called fpu CR is set right
	static const float ips = 1.0f/PIECESIZE;
	static const float pieceheightper = sqrtf(6.0f)/6.0f*PIECESIZE/6.0f; // 0 to 4

	static const pointf3x norm2(-sqrtf(10.0f)/5.0f,sqrtf(15.0f)/5.0f,0); // steep slope up
	static const pointf3x norm1(-sqrtf(7.0f)/7.0f,sqrtf(42.0f)/7.0f,0); // shallow slope up
//	static const pointf3x norm0(0,1,0); // no slope
	static const pointf3x normm1(sqrtf(7.0f)/7.0f,sqrtf(42.0f)/7.0f,0); // shallow slope down
	static const pointf3x normm2(sqrtf(10.0f)/5.0f,sqrtf(15.0f)/5.0f,0); //steep slope down
// clamping
	float x = max(0.0f,pnt->x); // new
	float z = max(0.0f,pnt->z);
// for now, new/bot to piece, upgrade could be multiple pieces, (line spanning pieces)
	S32 px=int(x*ips);
	S32 pz=int(z*ips);
	pointf3x offwb;
// clamping and offset
	if (px>=TRACKX) {
		px=TRACKX-1;
		offwb.x=PIECESIZE;
	} else {
		offwb.x=x-px*PIECESIZE;
	}
	if (pz>=TRACKZ) {
		pz=TRACKZ-1;
		offwb.z=PIECESIZE;
	} else {
		offwb.z=z-pz*PIECESIZE;
	}
	offwb.y = pnt->y;
// get data from track for this piece
	const piece& p=pieces[pz][px];
	pointf3 offpb = world2piece(offwb,p.rot);
	if (p.pt == PCE_LEFTFORK)
		offpb.z = PIECESIZE - offpb.z;
//	con32_printf(gg->con,"piece (%d,%d):type %d,r %d, o %6.2f %6.2f\n",px,pz,p.pt,p.rot,offpb.x,offpb.z);
	struct normd {
		pointf3 norm;
		float dist;
	};
	enum {D_LEFT,D_RIGHT,D_BOT,D_TOP};//,D_INTURN,D_OUTTURN};
	static normd norms[]={
		{{ 1,0, 0,0},PIECESIZE*.25f},  // left
		{{-1,0, 0,0},-PIECESIZE*.75f}, // right
		{{ 0,0, 1,0},PIECESIZE*.25f},  // bot
		{{ 0,0,-1,0},-PIECESIZE*.75f}, // top
	};
//	const U32 nnorms = sizeof(norms)/sizeof(norms[0]);
//	float outsb[nnorms];
//	U32 i;
// calc out out val from straight lines, out = -penetration
/*	for (i=0;i<nnorms;++i) {
		outsb[i] = dot3d(&norms[i].norm,&offpb)-norms[i].dist;
	} */
	float bestoutw = 0;
	pointf3x normw; // initialized norm, for normalize3d
// do walls first
	S32 idx = 0;
	float tween = 0.0f;
	float frac = 0.0f;
	bool inwall = false;
	switch(p.pt) {
	case PCE_STRAIGHT:
		{
#if 1
// do it by hand, might need more refinement
			float bot0 = widseg2wid(p.rightwidsegs[idx]);
			float top0 = widseg2wid(p.leftwidsegs[idx]);
//			float bot1 = widseg2wid(p.rightwidsegs[idx+1]);
//			float top1 = widseg2wid(p.leftwidsegs[idx+1]);
//			float bot = (1-tween)*bot0 + tween*bot1;
//			float top = (1-tween)*top0 + tween*top1;
//			float middle = (bot + top) *.5f;
			S32 topdelta = p.leftwidsegs[idx+1]-p.leftwidsegs[idx];
			S32 botdelta = p.rightwidsegs[idx+1]-p.rightwidsegs[idx];
			pointf3x normt,normb;
			if (topdelta == 0) {
				normt = norms[D_TOP].norm;
			} else if (topdelta == 1) {
				normt = pointf3x(sqrt2o2,0,-sqrt2o2);
			} else { // topdelta == -1
				normt = pointf3x(-sqrt2o2,0,-sqrt2o2);
			}
			if (botdelta == 0) {
				normb = norms[D_BOT].norm;
			} else if (botdelta == 1) {
				normb = pointf3x(-sqrt2o2,0,sqrt2o2);
			} else { // botdelta == -1
				normb = pointf3x(sqrt2o2,0,sqrt2o2);
			}
			pointf3x pnttop0(0.0f,0.0f,top0);
			pointf3x pntbot0(0.0f,0.0f,bot0);
			float dt=dot3d(&pnttop0,&normt);
			float db=dot3d(&pntbot0,&normb);
			pointf3x q(frac,0.0f,offpb.z);
			float bestoutt = dot3d(&normt,&q) - dt;
			float bestoutb = dot3d(&normb,&q) - db;
			if (bestoutt < bestoutb) {
				bestoutw = bestoutt;
				normw = normt;
			} else {
				bestoutw = bestoutb;
				normw = normb;
			}
/*			if (!gg->predicted) {
				gg->con->printf("nt %f %f %f",normt.x,normt.y,normt.z);
				gg->con->printf("nb %f %f %f",normb.x,normb.y,normb.z);
				gg->con->printf("bt %f",bestoutt);
				gg->con->printf("bb %f",bestoutb);
			} */
			inwall = bestoutw<0;
#else
// do it by hand, might need more refinement
			float bot0 = widseg2wid(p.rightwidsegs[idx]);
			float top0 = widseg2wid(p.leftwidsegs[idx]);
			float bot1 = widseg2wid(p.rightwidsegs[idx+1]);
			float top1 = widseg2wid(p.leftwidsegs[idx+1]);
			float bot = (1-tween)*bot0 + tween*bot1;
			float top = (1-tween)*top0 + tween*top1;
			float middle = (bot + top) *.5f;
			if (offpb.z >= middle) {
				norm = norms[D_TOP].norm;
				bestout = top - offpb.z;
			} else {
				norm = norms[D_BOT].norm;
				bestout = offpb.z - bot;
			}
			inwall = bestout<0;
#endif
		}
		break;
	case PCE_STARTFINISH:
		{
// do it by hand
			float bot = widseg2wid(p.rightwidsegs[0]);
			float top = widseg2wid(p.leftwidsegs[0]);
			float middle = (bot + top) *.5f;
			if (offpb.z >= middle) {
				normw = norms[D_TOP].norm;
				bestoutw = top - offpb.z;
			} else {
				normw = norms[D_BOT].norm;
				bestoutw = offpb.z - bot;
			}
			inwall = bestoutw<0;
		}
		break;
	case PCE_TURN:
// do it by hand
		{
			float rad = sqrtf(offpb.x*offpb.x+offpb.z*offpb.z);
			float inner = widseg2wid(p.rightwidsegs[0]);
			float outer = widseg2wid(p.leftwidsegs[0]);
			float middle = (inner + outer) *.5f;
// 2 sections
			normw.x=-offpb.x;
			normw.z=-offpb.z;
			normalize3d(&normw,&normw);
			if (rad>=middle) { // close to outer circle
				bestoutw = outer-rad;
			} else { // close to inner circle
				normw.x=offpb.x;
				normw.z=offpb.z;
				normalize3d(&normw,&normw);
				bestoutw = rad-inner;
			}
#ifdef DOCON
			con32_printf(gg->con,"turn : ");
#endif
			inwall = bestoutw<0;
		}
		break;
	case PCE_INTERSECTION:
		{
// do it by hand
// 8 sections, like an 8 slice pizza
			S32 oct = 0;
			float bot = widseg2wid(p.rightwidsegs[0]);
			float top = widseg2wid(p.leftwidsegs[0]);
			float middle = (bot + top) *.5f;
//			static const S32 octtab[8]={D_TOP,D_RIGHT,D_LEFT,D_TOP,D_BOT,D_LEFT,D_RIGHT,D_BOT};
			if (offpb.x >= middle) { // right
				if (offpb.z >= middle) { // right up
					if (offpb.x>=offpb.z) { // right up right
						oct = D_TOP;
					} else { // right up up
						oct = D_RIGHT;
					}
				} else { // right down
					if (offpb.x >= 2*middle - offpb.z) { // right down right
						oct = D_BOT;
					} else { // right down down
						oct = D_RIGHT;
					}
				}
			} else { // left
				if (offpb.z >= middle) { // left up
					if (offpb.x >= 2*middle - offpb.z) { // left up up
						oct = D_LEFT;
					} else { // left up left
						oct = D_TOP;
					}
				} else { // left down
					if (offpb.x>=offpb.z) { // left down down
						oct = D_LEFT;
					} else { // left down left
						oct = D_BOT;
					}
				}
			}
			normw = norms[oct].norm;
			switch(oct) {
			case D_LEFT:
				bestoutw = offpb.x - bot;
				break;
			case D_RIGHT:
				bestoutw = top - offpb.x;
				break;
			case D_BOT:
				bestoutw = offpb.z - bot;
				break;
			case D_TOP:
				bestoutw = top - offpb.z;
				break;
			}
//			S32 bestoutidx = octtab[oct];
//			bestout = outsb[oct];
//			norm=normpiece2world(norm,p.rot);
#ifdef DOCON
			con32_printf(gg->con,"intsect : ");
#endif
			inwall = bestoutw<0;
		}
		break;
	case PCE_LEFTFORK:
	case PCE_RIGHTFORK:
		{
// do it by hand
// 4 sections
// 2 straignt sections
			float bot,top;
			if (p.pt == PCE_LEFTFORK) {
				top = widseg2wid(3-p.rightwidsegs[0]);
				bot = widseg2wid(3-p.leftwidsegs[0]);
			} else {
				bot = widseg2wid(p.rightwidsegs[0]);
				top = widseg2wid(p.leftwidsegs[0]);
			}
			float middle = (bot + top) *.5f;
			float inner,outer;
//			if (p.pt == PCE_LEFTFORK) {
//				inner = top;
//				outer = bot;
//			} else {
				inner = bot;
				outer = top;
//			}
			if (offpb.z >= bot) { // straight part
//				S32 two = 0;
//				S32 bestoutidx;
				if (offpb.z >= middle) { // top
//					two = 0;
//					bestoutidx = D_TOP;
					bestoutw = top - offpb.z;
					normw = norms[D_TOP].norm;
#ifdef DOCON
					con32_printf(gg->con,"T ");
#endif
				} else { // bot
//					two = 1;
//					bestoutidx = D_BOT;
					bestoutw = offpb.z - bot;
					normw = norms[D_BOT].norm;
#ifdef DOCON
					con32_printf(gg->con,"B ");
#endif
				}
//				static const S32 twotab[2]={D_TOP,D_BOT};
//				S32 bestoutidx = twotab[two];
//				bestout = outsb[bestoutidx];
//				norm = norms[bestoutidx].norm;
			} else { // bot bot part
				float rad = sqrtf(offpb.x*offpb.x+offpb.z*offpb.z);
// 2 turn sections
				if (rad>=middle) { // close to outer circle, check against bottom of straight
					float bestoutstraight = offpb.z - bot;
					float bestoutturn = outer - rad;
					if (bestoutstraight >= bestoutturn) { // go with straight
						bestoutw = bestoutstraight;
						normw = norms[D_BOT].norm;
#ifdef DOCON
						con32_printf(gg->con,"B2 ");
#endif
					} else { // turn
						bestoutw = bestoutturn;
						normw.x = -offpb.x;
						normw.z = -offpb.z;
						normalize3d(&normw,&normw);
#ifdef DOCON
						con32_printf(gg->con,"O ");
#endif
					}
				} else { // close to inner circle, go with circle
					normw.x = offpb.x;
					normw.z = offpb.z;
					normalize3d(&normw,&normw);
					bestoutw = rad - inner;
#ifdef DOCON
					con32_printf(gg->con,"I ");
#endif
				}
			}
			if (p.pt == PCE_LEFTFORK) {
				normw.z = -normw.z;
				offpb.z = PIECESIZE - offpb.z;
			}
#ifdef DOCON
			con32_printf(gg->con,"rightfork : ");
#endif
			inwall = bestoutw<0;
		}
		break;
	case PCE_BLANK:
	default:
//#define SOLIDBLANK
#ifdef SOLIDBLANK
		if (bestintsect)
			*bestintsect=*top;
		if (bestnorm)
			*bestnorm=pointf3x(0,0,1,0);
		return true; // don't be on blank
#else
#ifdef DOCON
		con32_printf(gg->con,"blank\n");
#endif
		inwall = false;
#endif
	}
// penatration, needs adjustments (pos,rot)
//	offpb=piece2world(offpb,p.rot);
	normw=normpiece2world(normw,p.rot); // wall normal now in world space
#ifdef DOCON
	con32_printf(gg->con,"off (%5.3f,%5.3f), out %5.3f, norm (%5.3f,%5.3f)\n",
		offpb.x,offpb.z,bestout,norm.x,norm.z);
#endif
#if 1
#else
	if (bestintsect) {
		*bestintsect = *pnt;
		bestintsect->x -= norm.x*bestout;
		bestintsect->z -= norm.z*bestout;
//		bestintsect->x -= 2*norm.x*bestout;
//		bestintsect->z -= 2*norm.z*bestout;
	}
	if (bestnorm) {
		*bestnorm = norm;
	}
#endif
// now do floors
	bool infloor = false;
	float bestoutf = 0;
	pointf3x normf(0,1,0); // for hills
	switch(p.pt) {
	case PCE_STRAIGHT:
#if 1
		{
			xwid = PIECESIZE/6.0f;
			yhit = pieceheightper;
			pointf2x opnt(offpb.x,offpb.y);
			pointf2x npnt;
			infloor = pnt2line(p.heightsegs,piece::nseg+1,&opnt,&npnt);
			if (infloor) {
				normf.x = npnt.x - opnt.x;
				normf.y = npnt.y - opnt.y;
				normf.z = 0;
				bestoutf = -normalize3d(&normf,&normf);
				normf = normpiece2world(normf,p.rot);
			}
#if 0
			idx = int(offpb.x*6/PIECESIZE);
			idx = range(0,idx,5);
			frac = offpb.x - float(idx)*PIECESIZE/6.0f;
			tween = frac * 6.0f / PIECESIZE; 
			S32 deltaj = p.heightsegs[idx+1]-p.heightsegs[idx];
			if (deltaj == 0) {
				;//normj = norms[D_TOP].norm;
			} else if (deltaj == 1) {
				normf = norm1;
			} else if (deltaj == 2) {
				normf = norm2;
			} else if (deltaj == -1) {
				normf = normm1;
			} else { // deltaj == -2
				normf = normm2;
			}
//			pnt->y = ((1-tween)* p.heightsegs[idx] + tween * p.heightsegs[idx+1]) * pieceheightper;
			bestoutf = 0;
#endif
//			if (!gg->predicted) {
//				gg->con->printf("nj %f %f %f",normj.x,normj.y,normj.z);
//			} 
		}
		break;
#endif
	case PCE_STARTFINISH:
	case PCE_INTERSECTION:
	case PCE_TURN:
	case PCE_RIGHTFORK:
	case PCE_LEFTFORK:
	case PCE_BLANK:
		float floorheight = pieceheightper * p.heightsegs[3];
		if (pnt->y < floorheight) {
//			pnt->y = floorheight;
			bestoutf = pnt->y - floorheight;
			infloor = true;
		}
		break;
	}
	if (inwall) {
		nanerr("bestoutw",bestoutw);
		nanerr("normwx",normw.x);
		nanerr("normwz",normw.z);
		nanerr("pntx0",pnt->x);
		nanerr("pntz0",pnt->z);
		pnt->x -= normw.x*bestoutw;
		pnt->z -= normw.z*bestoutw;
		nanerr("pntx1",pnt->x);
		nanerr("pntz1",pnt->z);
	}
	if (infloor) {
		nanerr("bestoutf",bestoutf);
		nanerr("normfx",normf.x);
		nanerr("normfy",normf.y);
		nanerr("normfz",normf.z);
		nanerr("pntx0",pnt->x);
		nanerr("pnty0",pnt->y);
		nanerr("pntz0",pnt->z);
		pnt->x -= normf.x*bestoutf;
		pnt->y -= normf.y*bestoutf;
		pnt->z -= normf.z*bestoutf;
		nanerr("pntx1",pnt->x);
		nanerr("pnty1",pnt->y);
		nanerr("pntz1",pnt->z);
	}
	if (inwall) {
		if (infloor) { // inwall and infloor
			normjj->x = -bestoutw*normw.x + -bestoutf*normf.x;
			normjj->y = -bestoutw*normw.y + -bestoutf*normf.y;
			normjj->z = -bestoutw*normw.z + -bestoutf*normf.z;
			normalize3d(normjj,normjj);
			return true;
		} else { // just inwall
			if (normjj)
				*normjj = normw;
			return true;
		}
	} else {
		if (infloor) { // just infloor
			if (normjj)
				*normjj = normf;
			return true;
		} else { // nothing
			return false;
		}
	}

//	if (normjj)
//		*normjj=normj;
//	return infloor;
#if 0
	if (rot) {
		pointf3xh up(0,0,1,0);
		pointf3xh tangent(norm.z,0,-norm.x); // 90 deg to norm
//		pointf3xh tangent(norm.x,0,norm.z); // 0 deg to norm
		pointf3xh head(0,0,1,0);
		quatrot(rot,&head,&head);
//		pointf3xh head(newpnt->x-oldpnt->x,0,newpnt->z-oldpnt->z);
//		pointf3x head(0,0,1,0);
//		quatrot(oldrot,&head,&head);
		float glance = dot3d(&head,&tangent);
		if (glance<0) {
			tangent.x = -tangent.x;
			tangent.z = -tangent.z;
		}
//		if (!gg->predicted)
//			logger(" up '%s', tan '%s'\n",up.log().c_str(),tangent.log().c_str());
		normal2quaty(&up,&tangent,rot);
//		if (!gg->predicted)
//			logger(" bestrot '%s'\n",((pointf3xh*)bestrot)->log().c_str());
//		*bestrot = pointf3x();
	}
#else
//	if (bestrot) {
//	}
#endif
//	return true;
}

// just up and down, no walls, no physics, used to place car above road when reset pass in pnt->x and pnt->z, sets/returns pnt->y
void trk::line2roadvert(pointf3* pnt) const
{
	static const float ips = 1.0f/PIECESIZE;
	static const float pieceheightper = sqrtf(6.0f)/6.0f*PIECESIZE/6.0f; // 0 to 4
// clamping
	float x = max(0.0f,pnt->x); // new
	float z = max(0.0f,pnt->z);
	S32 px=int(x*ips);
	S32 pz=int(z*ips);
	pointf3x offwb;
// clamping and offset
	if (px>=TRACKX) {
		px=TRACKX-1;
		offwb.x=PIECESIZE;
	} else {
		offwb.x=x-px*PIECESIZE;
	}
	if (pz>=TRACKZ) {
		pz=TRACKZ-1;
		offwb.z=PIECESIZE;
	} else {
		offwb.z=z-pz*PIECESIZE;
	}
// get data from track for this piece
	const piece& p=pieces[pz][px];
	pointf3 offpb = world2piece(offwb,p.rot);
	switch(p.pt) {
	case PCE_STRAIGHT:
		{
			S32 idx = int(offpb.x*6/PIECESIZE);
			idx = range(0,idx,5);
			float frac = offpb.x - float(idx)*PIECESIZE/6.0f;
			float tween = frac * 6.0f / PIECESIZE; 
			pnt->y = ((1-tween) * p.heightsegs[idx] + tween * p.heightsegs[idx+1]) * pieceheightper;
		}
		break;
	case PCE_STARTFINISH:
	case PCE_INTERSECTION:
	case PCE_TURN:
	case PCE_RIGHTFORK:
	case PCE_LEFTFORK:
	case PCE_BLANK:
		pnt->y = p.heightsegs[3] * pieceheightper;
		break;
	}
}

#endif // FULLPHYSICS

// given world space position, return piece space offset,piece index,part and part rot
void trk::curpiece(const pointf3* newpnt,pointf3* offpb,pointi2* ppos,S32* pt,S32* rot) const
{
	const float ips = 1.0f/PIECESIZE;
// clamping and offset
	float x = max(0.0f,newpnt->x); // new
	float z = max(0.0f,newpnt->z);
// for now, new/bot to piece, upgrade could be multiple pieces, (line spanning pieces)
	S32 px=int(x*ips);
	S32 pz=int(z*ips);
//	pointf3x offwb;
// clamping and offset
	pointf3 offwb;
	if (px>=TRACKX) {
		px=TRACKX-1;
		offwb.x=PIECESIZE;
	} else {
		offwb.x=x-px*PIECESIZE;
	}
	if (pz>=TRACKZ) {
		pz=TRACKZ-1;
		offwb.z=PIECESIZE;
	} else {
		offwb.z=z-pz*PIECESIZE;
	}
	offwb.y=offwb.w=0;
// get data from track for this piece
	if (ppos) {
		ppos->x=px;
		ppos->y=pz;
	}
	const piece& p=pieces[pz][px];
	pointf3 offpbs = world2piece(offwb,p.rot);
	if (p.pt == PCE_LEFTFORK)
		offpbs.z = PIECESIZE - offpbs.z;
	if (offpb)
		*offpb = offpbs;
	if (pt)
		*pt=p.pt;
	if (rot)
		*rot=p.rot;
}

void trk::curpiece(const pointi2* ppos,S32* pt,S32* rot) const
{
// clamping and offset
	S32 px=range(0,ppos->x,TRACKX-1);
	S32 pz=range(0,ppos->y,TRACKZ-1);
	if (px>=TRACKX)
		px=TRACKX-1;
	if (pz>=TRACKZ)
		pz=TRACKZ-1;
// get data from track for this piece
	const piece& p=pieces[pz][px];
	if (pt)
		*pt=p.pt;
	if (rot)
		*rot=p.rot;
}
