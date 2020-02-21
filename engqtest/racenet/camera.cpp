//#include <stdio.h>
//#include <math.h>

//#include <engine1.h>
//#include <misclib.h>

//#include "stubhelper.h"
//#include "line2road.h"
//#include "online_uplay.h"
//#include "camera.h"

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "../engine7test/n_usefulcpp.h"
#include "gameinfo.h"
//#include "line2road.h"
//#include "../engine7test/n_line2roadcpp.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "crasheditor.h"
#include "newlobby.h"
#include "online_seltrack.h"
#include "../u_states.h"
#include "../engine7test/n_jrmcarscpp.h"
#include "boxai.h"
#include "packet.h"
#include "online_uphysics.h"
#include "box2box.h"
//#include "tspread.h"
#include "soundlst.h"
#include "gamestate.h"
#include "camera.h"
#include "scrline.h"

struct newcams newcamvars;

void new_cam_init()
{
//	newcamvars.zoom=3.2f;
//	newcamvars.dist=2;
//	newcamvars.center=-.15f;
//	newcamvars.pitch=1;
	newcamvars.pitch=newcamvars.normalpitch;
	newcamvars.yaw=0;
}

static void calccamrottrans()
{
	pointf3 targ,targrot,caroff,extracenter;
	float cospitch,chead;
//	if (!newcamvars.usesteeryaw2)
	chead=newcamvars.yaw;
	caroff.y=newcamvars.dist*sinf(newcamvars.pitch);
	cospitch=cosf(newcamvars.pitch);
	caroff.x=-cospitch*newcamvars.dist*sinf(chead);
	caroff.z=-cospitch*newcamvars.dist*cosf(chead);
	newcamvars.pos.x=op->pos.x+caroff.x;
	newcamvars.pos.y=op->pos.y+caroff.y;
	newcamvars.pos.z=op->pos.z+caroff.z;
// set camera rot to target car
	targ.x=op->pos.x-newcamvars.pos.x;
	targ.y=op->pos.y-newcamvars.pos.y;
	targ.z=op->pos.z-newcamvars.pos.z;
	targrot.y=(float)atan2(targ.x,targ.z);
	targrot.x=(float)-atan2(targ.y,sqrt(targ.x*targ.x+targ.z*targ.z));
	targrot.z=0;
	rpy2quat(&targrot,&newcamvars.rot);
// un center the car on the screen (move down)
	extracenter.x=1;
	extracenter.y=0;
	extracenter.z=0;
	extracenter.w=newcamvars.centery;
	rotaxis2quat(&extracenter,&extracenter);
	quattimes(&newcamvars.rot,&extracenter,&newcamvars.rot);
//	if (newcamvars.usesteeryaw2) {
		extracenter.x=0;
		extracenter.y=1;
		extracenter.z=0;
		extracenter.w=newcamvars.centerx;
		rotaxis2quat(&extracenter,&extracenter);
		quattimes(&newcamvars.rot,&extracenter,&newcamvars.rot);
//	}
}

static bool st2_fatline2road(pointf3 *top,pointf3 *bot,
		pointf3 *up,pointf3 *right,float safeup,float saferight,
		pointf3 *intsect,pointf3 *norm)
{
	pointf3 nb;
	bool ret;
	ret=(st2_line2road(top,bot,intsect,norm)!=0);
	if (ret)
		return ret;
	nb.x=bot->x+safeup*up->x;
	nb.y=bot->y+safeup*up->y;
	nb.z=bot->z+safeup*up->z;
	if (ret=(st2_line2road(top,&nb,intsect,norm)!=0)) {
		norm->x=-up->x;
		norm->y=-up->y;
		norm->z=-up->z;
		return ret;
	}
	nb.x=bot->x-safeup*up->x;
	nb.y=bot->y-safeup*up->y;
	nb.z=bot->z-safeup*up->z;
	if (ret=(st2_line2road(top,&nb,intsect,norm)!=0)) {
		*norm=*up;
		return ret;
	}
	nb.x=bot->x+saferight*right->x;
	nb.y=bot->y+saferight*right->y;
	nb.z=bot->z+saferight*right->z;
	if (ret=(st2_line2road(top,&nb,intsect,norm)!=0)) {
		norm->x=-right->x;
		norm->y=-right->y;
		norm->z=-right->z;
		return ret;
	}
	nb.x=bot->x-saferight*right->x;
	nb.y=bot->y-saferight*right->y;
	nb.z=bot->z-saferight*right->z;
	if (ret=(st2_line2road(top,&nb,intsect,norm)!=0)) {
		*norm=*right;
		return ret;
	}
	return false;
}
	
void new_cam_proc(struct ol_playerdata *op)
{
	pointf3 *ca;
	pointf3 intsect,norm;
	pointf3 cd;
	int tries=0;
	float chead,dhead;
	float chr,cpr,cyr,ps,ys,cs;
//	float steeryaw;
	if (!newcamvars.usenewcam)
		return;
// get car heading
	if (op->ol_flymode==1 && op->ol_startstunt) // last ground car rot
		ca=&op->carangsave;
	else
		ca=&op->carang;
	newcamvars.centerx=0;
	cd.x=0;
	cd.y=0;
	cd.z=1;
	quatrot(ca,&cd,&cd);
	chead=(float)atan2(cd.x,cd.z);
	chr=newcamvars.cheadrate;
	cpr=newcamvars.collpitchrate;
	cyr=newcamvars.collyawrate;
	if (od.ol_uloop>0) {
		chr*=od.ol_uloop;
		cpr*=od.ol_uloop;
		cyr*=od.ol_uloop;
//			syr*=od.ol_uloop;
	}
	chr*=.5f;
	cpr*=.5f;
	cyr*=.5f;
	if (op->clocktickcount>INITCANTSTARTDRIVE) {
		newcamvars.centery+=newcamvars.centerratey;
		if (newcamvars.centery>newcamvars.normalcentery)
			newcamvars.centery=newcamvars.normalcentery;
//		syr=newcamvars.steeryawrate;
//		syr*=.5f;
		dhead=snormalangrad(chead-newcamvars.yaw);
		if (dhead>chr*.5f)
			newcamvars.yaw+=chr;
		else if (dhead<chr*-.5f)
			newcamvars.yaw-=chr;
		else
			newcamvars.yaw=chead;
	} else {
//		resetcamera();
		newcamvars.yaw=chead;
		newcamvars.centery=newcamvars.startcentery;
		newcamvars.cyaw=newcamvars.yaw;
		newcamvars.cpitch=newcamvars.pitch;
		newcamvars.ccenterx=newcamvars.centerx;
	}
/*	if (1) {
//		newcamvars.normalyaw=op->wheelyaw*newcamvars.steeryaw;
		int reduce=0;
		if (op->ol_flymode==0) {
			if (op->pi.ol_uleft && !op->pi.ol_uright) {
				newcamvars.currightcount=0;
				if (od.ol_uloop>0)
					newcamvars.curleftcount+=od.ol_uloop;
				else
					newcamvars.curleftcount++;
				if (newcamvars.curleftcount>=newcamvars.leftcount) {
					newcamvars.normalyaw-=syr;
					if (newcamvars.normalyaw<-newcamvars.steeryawlimit)
						newcamvars.normalyaw=-newcamvars.steeryawlimit;
				} else
					reduce=1;
			} else if (op->pi.ol_uright && !op->pi.ol_uleft) {
				if (od.ol_uloop>0)
					newcamvars.currightcount+=od.ol_uloop;
				else
					newcamvars.currightcount++;
				newcamvars.curleftcount=0;
				if (newcamvars.currightcount>=newcamvars.rightcount) {
					newcamvars.normalyaw+=syr;
					if (newcamvars.normalyaw>newcamvars.steeryawlimit)
						newcamvars.normalyaw=newcamvars.steeryawlimit;
				} else
					reduce=1;
			} else {
				reduce=1;
				newcamvars.currightcount=newcamvars.curleftcount=0;
			}
			if (reduce) {
				if (newcamvars.normalyaw>syr*.5f)
					newcamvars.normalyaw-=syr;
				if (newcamvars.normalyaw<syr*-.5f)
					newcamvars.normalyaw+=syr;
			}
		} else
			newcamvars.normalyaw=0;
	} else
		newcamvars.normalyaw=0;
*/
	if (!newcamvars.usecoll) {
		newcamvars.pitch=newcamvars.normalpitch;
	}
//	steeryaw=op->wheelyaw;
//	steeryaw=newcamvars.normalyaw;
	calccamrottrans();
// set pos given yaw and pitch (and car heading/pos)
	if (newcamvars.usecoll) {
		pointf3 campv,camyv;
		while (tries<newcamvars.colltries) {
			float dotp,doty;
//			setVEC(&campv,0,1,0);
//			setVEC(&camyv,1,0,0);
			campv=pointf3x(0,1,0);
			camyv=pointf3x(1,0,0);
			quatrot(&newcamvars.rot,&campv,&campv);
			quatrot(&newcamvars.rot,&camyv,&camyv);
			if (st2_fatline2road(&op->pos,&newcamvars.pos,&campv,&camyv,newcamvars.pitchsafe,newcamvars.yawsafe,&intsect,&norm)) {
// something in the way
				dotp=dot3d(&norm,&campv);
				doty=dot3d(&norm,&camyv);
//				if (0) {
// which way to go
				if (fabs(dotp)>fabs(doty)) {
					if (dotp>0) {
						newcamvars.pitch+=cpr;
						if (newcamvars.pitch>newcamvars.pitchlimit) {
							newcamvars.pitch=newcamvars.pitchlimit;
							break;
						}
					} else {
						newcamvars.pitch-=cpr;
						if (newcamvars.pitch<-newcamvars.pitchlimit) {
							newcamvars.pitch=-newcamvars.pitchlimit;
							break;
						}
					}
				} else {
					if (doty<0) {
						newcamvars.yaw+=cyr;
						if (snormalangrad(newcamvars.yaw-chead)>newcamvars.yawlimit) {
							newcamvars.yaw=chead+newcamvars.yawlimit;
							break;
						}
					} else {
						newcamvars.yaw-=cyr;
						if (snormalangrad(newcamvars.yaw-chead)<-newcamvars.yawlimit) {
							newcamvars.yaw=chead-newcamvars.yawlimit;
							break;
						}
					}
				}
				calccamrottrans();
				tries++;
			} else
				break;
		}
		if (tries==0) {
			int recalc;
			float dp;//,dy;
			float psave,ysave;
			recalc=0;
			dp=newcamvars.pitch-newcamvars.normalpitch;
			psave=newcamvars.pitch;
			ysave=newcamvars.yaw;
			if (dp<cpr*-.5f) {
				newcamvars.pitch+=cpr;
				recalc=1;
			} else if (dp>cpr*.5f) {
				newcamvars.pitch-=cpr;
				recalc=1;
			} else
				newcamvars.pitch=newcamvars.normalpitch;
//			newcamvars.normalyaw2=newcamvars.yaw;
//			newcamvars.normalyaw2=newcamvars.normalyaw;
//			newcamvars.normalyaw2=0;
			dhead=snormalangrad(chead-newcamvars.yaw);
			if (dhead>cyr*.5f)
				newcamvars.yaw+=cyr;
			else if (dhead<cyr*-.5f)
				newcamvars.yaw-=cyr;
			else
				newcamvars.yaw=chead;
			if (recalc) {
				calccamrottrans();
//				setVEC(&campv,0,1,0);
//				setVEC(&camyv,1,0,0);
				campv=pointf3x(0,1,0);
				camyv=pointf3x(1,0,0);
				quatrot(&newcamvars.rot,&campv,&campv);
				quatrot(&newcamvars.rot,&camyv,&camyv);
				if (st2_fatline2road(&op->pos,&newcamvars.pos,&campv,&camyv,newcamvars.pitchsafe,newcamvars.yawsafe,&intsect,&norm)) {
					newcamvars.yaw=ysave;
					newcamvars.pitch=psave;
					calccamrottrans();
				}
			}
		}
	} 
	newcamvars.centerx=newcamvars.steeryaw*snormalangrad(newcamvars.yaw-newcamvars.cyaw);
	if (op->clocktickcount<INITCANTSTARTDRIVE) {
		newcamvars.cpitch=newcamvars.pitch;
		newcamvars.cyaw=newcamvars.yaw;
		newcamvars.ccenterx=newcamvars.centerx;
	} else {
		newcamvars.cpitch+=(newcamvars.pitch-newcamvars.cpitch)*newcamvars.pitchdrift;	
		newcamvars.cyaw+=snormalangrad(newcamvars.yaw-newcamvars.cyaw)*newcamvars.yawdrift;	
		newcamvars.cyaw=snormalangrad(newcamvars.cyaw);
		newcamvars.ccenterx+=(newcamvars.centerx-newcamvars.ccenterx)*newcamvars.centerxdrift;
	}
	ps=newcamvars.pitch;
	ys=newcamvars.yaw;
	cs=newcamvars.centerx;
	newcamvars.pitch=newcamvars.cpitch;
	newcamvars.yaw=newcamvars.cyaw;
	newcamvars.centerx=newcamvars.ccenterx;
	calccamrottrans();
	newcamvars.pitch=ps;
	newcamvars.yaw=ys;
	newcamvars.centerx=cs;
// copy
	od.ol_camnull->trans=newcamvars.pos;
	od.ol_camnull->rot=newcamvars.rot;
	od.ol_camnull->zoom=newcamvars.zoom;
}

void resetcamera()
{
	newcamvars.cyaw=newcamvars.yaw;
	newcamvars.cpitch=newcamvars.pitch;
	newcamvars.ccenterx=newcamvars.centerx;
}
