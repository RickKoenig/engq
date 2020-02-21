#include <m_eng.h>

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
#include "carenagame.h"

// calculate t for xt = x0 + (x1-x0)*t, xt usually 0
// but only if x0 >= xt > x1
static bool findt(float x0,float x1,float xt,float* t) // 0 to 1 if intsect, 0 closest
{
	if (x0>=xt && x1<xt) { // went from good to bad
		*t = (xt-x0)/(x1-x0);
		return true;
	} else {
		return false;
	}
}

// input old,new
// output intsect,norm
// do it by hand, all multiple of 90 degrees
static pointf3 world2piece(pointf3 w,S32 rot)
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
	return p;
}

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

// return true if penetrates into solid
bool trk::line2road(const pointf3* top,const pointf3* bot,pointf3* bestintsect,pointf3* bestnorm)
{
	const float ips = 1.0f/PIECESIZE;
// clamping
	float tx = max(0.0f,top->x); // old
	float tz = max(0.0f,top->z);
	float bx = max(0.0f,bot->x); // new
	float bz = max(0.0f,bot->z);
// for now, new/bot to piece, upgrade could be multiple pieces, (line spanning pieces)
	S32 px=int(bx*ips);
	S32 pz=int(bz*ips);
	pointf3x offwt;
	pointf3x offwb;
// clamping and offset
	if (px>=TRACKX) {
		px=TRACKX-1;
		offwt.x=PIECESIZE;
		offwb.x=PIECESIZE;
	} else {
		offwt.x=tx-px*PIECESIZE;
		offwb.x=bx-px*PIECESIZE;
	}
	if (pz>=TRACKZ) {
		pz=TRACKZ-1;
		offwt.z=PIECESIZE;
		offwb.z=PIECESIZE;
	} else {
		offwt.z=tz-pz*PIECESIZE;
		offwb.z=bz-pz*PIECESIZE;
	}
// get data from track for this piece
	const piece& p=pieces[pz][px];
	pointf3 offpt = world2piece(offwt,p.rot); // piece relative points
	pointf3 offpb = world2piece(offwb,p.rot);
//	con32_printf(gg->con,"piece (%d,%d):type %d,r %d, o %6.2f %6.2f\n",px,pz,p.pt,p.rot,offpb.x,offpb.z);
	struct normd {
		pointf3 norm;
		float dist;
	};
	enum {D_LEFT,D_RIGHT,D_BOT,D_TOP,D_INTURN,D_OUTTURN};
	static normd norms[]={
		{{ 1,0, 0,0},PIECESIZE*.25f},  // left
		{{-1,0, 0,0},-PIECESIZE*.75f}, // right
		{{ 0,0, 1,0},PIECESIZE*.25f},  // bot
		{{ 0,0,-1,0},-PIECESIZE*.75f}, // top
	};
	const U32 nnorms = sizeof(norms)/sizeof(norms[0]);
	const U32 numturns=2;
	float outst[nnorms+numturns];
	float outsb[nnorms+numturns];
	U32 i;
	float outside = 0;
// calc out out val from straight lines, out = -penetration
	for (i=0;i<nnorms;++i) {
		outst[i] = dot3d(&norms[i].norm,&offpt)-norms[i].dist;
		outsb[i] = dot3d(&norms[i].norm,&offpb)-norms[i].dist;
	}
	S32 bestout = -1;
	float besttime = 2;
	switch(p.pt) {
	case PCE_STRAIGHT:
	case PCE_STARTFINISH:
		{
			float t;
			if (findt(outst[D_TOP],outsb[D_TOP],0,&t)) {
				con32_printf(gg->con,"t top = %f\n",t);
				if (t<besttime) {
					bestout = D_TOP;
					besttime = t;
				}
			}
			if (findt(outst[D_BOT],outsb[D_BOT],0,&t)) {
				con32_printf(gg->con,"t bot = %f\n",t);
				if (t<besttime) {
					bestout = D_BOT;
					besttime = t;
				}
			}
//			return outsb[D_BOT]<0 || outsb[D_TOP]<0;
			if (bestout == -1)
				return false;
			if (bestnorm) {
				*bestnorm = norms[bestout].norm;
				con32_printf(gg->con,"norm %f,%f,%f\n",bestnorm->x,bestnorm->y,bestnorm->z);
			}
			return true;
		}
	case PCE_INTERSECTION:
		{
#if 1
			float t;
			if (findt(outst[D_TOP],outsb[D_TOP],0,&t)) {
				con32_printf(gg->con,"t top = %f\n",t);
				if (t<besttime) {
					bestout = D_TOP;
					besttime = t;
				}
			}
			if (findt(outst[D_BOT],outsb[D_BOT],0,&t)) {
				con32_printf(gg->con,"t bot = %f\n",t);
				if (t<besttime) {
					bestout = D_BOT;
					besttime = t;
				}
			}
//			return outsb[D_BOT]<0 || outsb[D_TOP]<0;
			if (bestout == -1)
				return false;
			if (bestnorm) {
				*bestnorm = norms[bestout].norm;
				con32_printf(gg->con,"norm %f,%f,%f\n",bestnorm->x,bestnorm->y,bestnorm->z);
			}
			return true;
#else
			return (outsb[D_BOT]<0 || outsb[D_TOP]<0) && (outsb[D_LEFT]<0 || outsb[D_RIGHT]<0); 
#endif
		}
	case PCE_TURN:
		{
			float sqr = offpb.x*offpb.x+offpb.z*offpb.z;
			outsb[D_INTURN] = sqr - PIECESIZE*.25f*PIECESIZE*.25f;
			outsb[D_OUTTURN] = PIECESIZE*.75f*PIECESIZE*.75f - sqr;
			return outsb[D_INTURN]<0 || outsb[D_OUTTURN]<0;
		}
	case PCE_RIGHTFORK:
		{
			float sqr = offpb.x*offpb.x+offpb.z*offpb.z;
			outsb[D_INTURN] = sqr - PIECESIZE*.25f*PIECESIZE*.25f;
			outsb[D_OUTTURN] = PIECESIZE*.75f*PIECESIZE*.75f - sqr;
			return (outsb[D_BOT]<0 || outsb[D_TOP]<0) && (outsb[D_INTURN]<0 || outsb[D_OUTTURN]<0); 
		}
	case PCE_LEFTFORK:
		{
			float moz = PIECESIZE - offpb.z;
			float sqr = offpb.x*offpb.x+moz*moz;
			outsb[D_INTURN] = sqr - PIECESIZE*.25f*PIECESIZE*.25f;
			outsb[D_OUTTURN] = PIECESIZE*.75f*PIECESIZE*.75f - sqr;
			return (outsb[D_BOT]<0 || outsb[D_TOP]<0) && (outsb[D_INTURN]<0 || outsb[D_OUTTURN]<0); 
		}
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
		return false; // free roam
#endif
	}

/*	float ip=0.0f;
	bool didpen =      interp(top->x,bot->x,0.0f,&ip); // check for penetration neg numbers
	didpen = didpen || interp(top->z,bot->z,0.0f,&ip); // check for penetration neg numbers
	didpen = didpen || interp(PIECESIZE-top->x,PIECESIZE-bot->x,0.0f,&ip); // check for penetration neg numbers
	didpen = didpen || interp(PIECESIZE-top->z,PIECESIZE-bot->z,0.0f,&ip); // check for penetration neg numbers
	return didpen; */
}
