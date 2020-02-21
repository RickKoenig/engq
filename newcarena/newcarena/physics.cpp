#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "utils/json.h"
#include "utils/jsonx.h"
#include "netdescj.h"
#include "hash.h"
#include "constructor.h"
#include "timewarp.h"
#include "envj.h"
#include "enums.h"
#include "carclass.h"
#include "avaj.h"
#include "physics.h"
#include "gamedescj.h"
#include "carenagame.h"
#include "gamerunj.h"
#include "box2box.h"
#include "soundfx.h"

const C8* getphysicsdir()
{
	if (wininfo.isalreadyrunning)
		return "physics_b";
	else
		return "physics";
}

#ifdef FULLPHYSICS
calcj acv2;
calcj * const cv=&acv2;

#endif
namespace n_physics {
float movespeed = 40.0f*(1.0f/TICKRATE);//.0625f; // meters/second // 40 m/s is about 89 mph (sysdebvars)

#ifndef FULLPHYSICS
const float turnrate = .375f*(TWOPI/TICKRATE);//.035f; // revolutions/second
void docarphysics(U32 slot,const keys& k);
void collidecars();
#endif

void bbox2transscale(const pointf3& bmin,const pointf3& bmax,pointf3& trans,pointf3& scale)
{
	trans.x = (bmin.x + bmax.x) *.5f;
	trans.y = (bmin.y + bmax.y) *.5f;
	trans.z = (bmin.z + bmax.z) *.5f;
	scale.x = bmax.x - bmin.x;
	scale.y = bmax.y - bmin.y;
	scale.z = bmax.z - bmin.z;
}

void physics_do(const vector<keys>& ks)
{
#ifdef FULLPHYSICS
	resetcar2car();
	S32 i,j;
	for (i=0;i<gg->numplayers;++i) {
		twcaro* tw = &gg->caros[i].cs;
		calco* tv = &gg->caros[i].scr;
		calccollisioninfo(tw,tv);
	}
#if 1
	for (j=0;j<gg->numplayers;++j) {
		caroj* twj = &gg->caros[j];
		for (i=j+1;i<gg->numplayers;++i) {
			caroj* twi = &gg->caros[i];
			checkcar2car(twj,twi);
		}
	}
#endif
	for (i=0;i<gg->numplayers;++i) {
		twcaro* tw = &gg->caros[i].cs;
		calco* tv = &gg->caros[i].scr;
		doroadcollisions(&gg->caros[i],tv,ks[i]);
		updatetrans(tw);
		updaterot(tw,ks[i]);
	}
// update graphics
	for (i=0;i<gg->numplayers;++i) {
		caroj* a = &gg->caros[i];
		bbox2transscale(cv->carbboxmin,cv->carbboxmax,a->carbodyprism->trans,a->carbodyprism->scale);
		for (j=0;j<NWORKINGSHOCKS;++j) {
			pointf3x sbmin = pointf3x(cv->p[2*j+1].x-.1f,cv->p[2*j+1].y,cv->p[2*j+1].z-.1f);
			pointf3x sbmax = pointf3x(cv->p[2*j  ].x+.1f,cv->p[2*j  ].y,cv->p[2*j  ].z+.1f);
			bbox2transscale(sbmin,sbmax,a->carshocks[j]->trans,a->carshocks[j]->scale);
			const pointf3x& wmax = cv->p[2*j];
//			const pointf3x& wmin = cv->p[2*j+1];
			a->carwheels[j]->trans=wmax;
			if (j&1)
				a->carwheels[j]->trans.x+=.1f;
			else
				a->carwheels[j]->trans.x-=.1f;
			a->carwheels[j]->trans.y-=a->cs.shocklen[j]-cv->wheelrad;
			a->carwheels[j]->scale=pointf3x(cv->wheelrad,.2f,cv->wheelrad);
//			bbox2transscale(sbmin,sbmax,);
			a->carwheels[j]->rotvel = pointf3x(a->cs.accelspin/cv->wheelrad/30.0f,0,0); // magic 30, make animstep
			if ((j&2)==0) {
				if (ks[i].kl && !ks[i].kr)
					a->carwheels[j]->rot.y=-PI/8; // for now keep simple
				else if (!ks[i].kl && ks[i].kr)
					a->carwheels[j]->rot.y=PI/8; // for now keep simple
				else
					a->carwheels[j]->rot.y=0; // for now keep simple
			}
//			a->carwheels[j]->rotvel = pointf3x(TWOPI/30.0f,0,0);
		} 
	}
#else
// move cars
	S32 i;
	for (i=0;i<gg->numplayers;++i) {
		docarphysics(i,ks[i]);
	}
// collide cars
	collidecars();
#endif
}

#ifndef FULLPHYSICS
static void docarphysics(U32 slot,const keys& pko) // TW
{
	keys pk = pko;
// kill car if too early or if bots done racing
	if (gg->gs.clock-gg->gs.clockoffset<gcfg.candrivetime)
		pk.ku=pk.kd=pk.kl=pk.kr=0;
//	if (gg->caros[slot].cs.fintime && gg->caros[slot].ownerid==-1)
//		pk.ku=pk.kd=pk.kl=pk.kr=0;
// object to move  ->  pos,rot,vel,rotvel
	twcaro* co=&gg->caros[slot].cs;
// speed
	float ms=0;
	if (pk.ku)
		ms+=movespeed;
	if (pk.kd)
		ms-=movespeed;
// ang vel
	pointf3 rotvel=pointf3x(0,1,0,0);
	rotvel.w=0;
	if ((pk.kr && ms>=0.0f) || (pk.kl && ms<0.0f))
		rotvel.w+=turnrate;
	if ((pk.kl && ms>=0.0f) || (pk.kr && ms<0.0f))
		rotvel.w-=turnrate;
	rotaxis2quat(&rotvel,&rotvel);
// update rot
	quattimes(&co->rot,&rotvel,&co->rot);
	quatnormalize(&co->rot,&co->rot);
// vel
	pointf3x vel=pointf3x(0,0,ms);
//	if (/*gg->yourid==1 && */slot==1) // test break timewarp, catch bad hash
//		ms+=movespeed;
//	if (gg->gs.clock==200  && gg->yourid==0 ) // test break timewarp, catch bad hash
//		co->cs.pos.y+=2.0f;
	quatrot(&co->rot,&vel,&vel);
// update pos
//	pointf3 oldpos = co->pos;
	co->pos.x+=vel.x;
	co->pos.z+=vel.z;
// track collision, check new pos
	trk* coll=gg->e.oldtrackj;
//	pointf3 intsect;//,norm;
//	pointf3 bestrot;
#if 1
	coll->line2roadxz(&co->pos,&co->rot,&co->normj);
#else
	bool ret=coll->line2road(&oldpos,&co->pos,&co->rot,&intsect,0,&bestrot);
	if (ret) {
		co->pos = intsect; // move back
//		pointf3 up={0,1,0};
//		pointf3 q;
//		if (pk.kd) { //reverse
//			quattimes(&bestrot,&pointf3x(0,1,0,0),&bestrot);
//		}
		co->rot=bestrot;
	}
#endif
}

static void collidecars() // TW
{
// // track collision
	trk* coll=gg->e.oldtrackj;
//	return;
	S32 i,j;
	for (i=0;i<gg->numplayers;++i) {
		pointf3 *p=&gg->caros[i].cs.pos;
		pointf3 *rp=&gg->caros[i].cs.rot;
		pointf3 *normjp=&gg->caros[i].cs.normj;
		for (j=i+1;j<gg->numplayers;++j) {
			pointf3 *q=&gg->caros[j].cs.pos;
			pointf3 *rq=&gg->caros[j].cs.rot;
			pointf3 *normjq=&gg->caros[j].cs.normj;
			pointf3x pmq;
			float d2;
			pmq.x=p->x-q->x;
			pmq.z=p->z-q->z;
			d2=pmq.x*pmq.x+pmq.z*pmq.z;
			if (d2<gcfg.car2cardist*gcfg.car2cardist) {
				pointf3 a;
				a.x=(p->x+q->x)*.5f;
				a.y=0;
				a.z=(p->z+q->z)*.5f;
				float ik=(float)sqrt(d2)/gcfg.car2cardist;
				if (ik<1.0f/16.0f) { // cars almost in same place, separate in x only
					p->x=a.x-gcfg.car2cardist*.5f;
					p->y=0;
					p->z=a.z;
					q->x=a.x+gcfg.car2cardist*.5f;
					q->y=0;
					q->z=a.z;
				} else {
					float k=1.0f/ik;
					p->x=k*p->x+(1-k)*a.x;
					p->y=0;
					p->z=k*p->z+(1-k)*a.z;
					q->x=k*q->x+(1-k)*a.x;
					q->y=0;
					q->z=k*q->z+(1-k)*a.z;
				}
				coll->line2roadxz(p,rp,normjp);
				coll->line2roadxz(q,rq,normjq);
			}
		}
	}
}
#endif
#ifdef FULLPHYSICS
// car 2 car
pointf3 testimpval;
pointf3 testimppnt;
//static struct phyobject phyobjects[2];
//static float globalelast;
//					car to car,                          test  ,missiles and such
//#define MAXCOLLINFO 100
struct collinfo collinfos[MAXCOLLINFO];
int ncollinfo;

void physics_init()
{
	if (gg->n.gamenet==GAMENET_CLIENT) {
		acv2 = gg->e.pc;
	} else {
pushandsetdirdown(getphysicsdir());
		json js(fileload_string("physics.txt"));
popdir();
		acv2 = calcj(js);
	} 
}

void physics_exit()
{
	if (gg->n.gamenet!=GAMENET_BOTGAME)
		return; // only save physics if in a botgame
	json js=acv2.save();
pushandsetdirdown(getphysicsdir());
	js.save("physics.txt");
popdir();
}

void addcollinfo(int carid,pointf3 *val,pointf3 *pnt);
// car physics
static void doairfric(twcaro *co,float af)
{
//	int i;
	pointf3 f;
	float fd;
//	struct ol_object *op;
//	return;
	if (cv->mass) {
		fd=((co->vel.x*co->vel.x+co->vel.y*co->vel.y+co->vel.z*co->vel.z)
			*af*TIMEINC)/cv->mass;
		if (fd>EPSILON) {
			f=co->vel;
			if (normalize3d(&f,&f)) {
				f.x*=fd;
				f.y*=fd;
				f.z*=fd;
//				if (!gg->predicted)
//					logger("af f.x %f ovx %f ",f.x,co->vel.x);
				co->vel.x-=f.x;
//				if (!gg->predicted)
//					logger("nvx %f\n",co->vel.x);
				co->vel.y-=f.y;
				co->vel.z-=f.z;
			}
		}
	}
}

static void dograv(twcaro *co,float g)
{
//	int i;
//	JXFORM x;
//	pointf3 gdir;
//	struct ol_object *op;
//	for (op=ol_objects,i=0;i<NUMOBJECTS;i++,op++) {
		if (cv->mass) {
//			if (op->vinfusion)
//				g=g*(-.125f);
//			else if (op->vinsonic)
//				g=g*(-2);
/*			if (op->ol_magnatraction || op->extramagnatraction && op->ol_airtime<=2) {
				quat2xform(&op->carang,&x);
				gdir.x=0;
				gdir.y=1;
				gdir.z=0;
				xformvec(&x,&gdir,&gdir);
				op->carvel.x-=g*TIMEINC*gdir.x;
				op->carvel.y-=g*TIMEINC*gdir.y;
				op->carvel.z-=g*TIMEINC*gdir.z;
			} else { */
				co->vel.y-=g*TIMEINC;
//			}
		}
//	}
}

/////////////////////////// collisions
void calccollisioninfo(twcaro *co,calco* tv) // also updates stuff needed for drawing
{
//	JXFORM local2norot;
//	mat4 local2norot;
	int i;
//	struct ol_object *op;
//	for (op=ol_objects,i=0;i<NUMOBJECTS;i++,op++) {
		// shocks
//		op->oldpos=op->pos;
//		op->oldcarang=op->carang;
/*	if (op->extrabigtires) {
		op->shockextra+=od.bigtireuprate;
		if (op->shockextra>od.bigtireheight)
			op->shockextra=od.bigtireheight;
	} else if (op->extrapouncer) {
		float ph;
		ph=od.pouncerheight;
		if (op->tweapbooston)
			ph*=1.5f;
		op->shockextra+=od.pounceruprate;
		if (op->shockextra>ph)
			op->shockextra=ph;
	} else { */
/*		float lowrider=1.0f-op->vunderbigtire*(1/32.0f);
		op->shockextra-=od.bigtiredownrate;
		if (op->shockextra<lowrider)
			op->shockextra=lowrider; */
//	}
	cv->extrashocklen[0]=cv->normalshocklen[0]*cv->shockextra;
	cv->extrashocklen[1]=cv->normalshocklen[1]*cv->shockextra;
	cv->extrashocklen[2]=cv->normalshocklen[2]*cv->shockextra;
	cv->extrashocklen[3]=cv->normalshocklen[3]*cv->shockextra;

	cv->p[0].x=-cv->shockspacingx/2*cv->shockextra;
	cv->p[0].y=cv->shockoffsety;
	cv->p[0].z=cv->shockspacingz/2*cv->shockextra;
	cv->p[1].x=-cv->shockspacingx/2*cv->shockextra;
	cv->p[1].y=cv->shockoffsety-cv->extrashocklen[0];
	cv->p[1].z=cv->shockspacingz/2*cv->shockextra;

	cv->p[2].x=cv->shockspacingx/2*cv->shockextra;
	cv->p[2].y=cv->shockoffsety;
	cv->p[2].z=cv->shockspacingz/2*cv->shockextra;
	cv->p[3].x=cv->shockspacingx/2*cv->shockextra;
	cv->p[3].y=cv->shockoffsety-cv->extrashocklen[1];
	cv->p[3].z=cv->shockspacingz/2*cv->shockextra;

	cv->p[4].x=-cv->shockspacingx/2*cv->shockextra;
	cv->p[4].y=cv->shockoffsety;
	cv->p[4].z=-cv->shockspacingz/2*cv->shockextra;
	cv->p[5].x=-cv->shockspacingx/2*cv->shockextra;
	cv->p[5].y=cv->shockoffsety-cv->extrashocklen[2];
	cv->p[5].z=-cv->shockspacingz/2*cv->shockextra;

	cv->p[6].x=cv->shockspacingx/2*cv->shockextra;
	cv->p[6].y=cv->shockoffsety;
	cv->p[6].z=-cv->shockspacingz/2*cv->shockextra;
	cv->p[7].x=cv->shockspacingx/2*cv->shockextra;
	cv->p[7].y=cv->shockoffsety-cv->extrashocklen[3];
	cv->p[7].z=-cv->shockspacingz/2*cv->shockextra;

	// 8 more points for object to road collisions
	cv->p[8].x=cv->carbboxmin.x;
	cv->p[8].y=cv->carbboxmin.y;
	cv->p[8].z=cv->carbboxmin.z;
	cv->p[9].x=cv->carbboxmin.x;
	cv->p[9].y=cv->carbboxmin.y;
	cv->p[9].z=cv->carbboxmax.z;
	cv->p[10].x=cv->carbboxmin.x;
	cv->p[10].y=cv->carbboxmax.y;
	cv->p[10].z=cv->carbboxmin.z;
	cv->p[11].x=cv->carbboxmin.x;
	cv->p[11].y=cv->carbboxmax.y;
	cv->p[11].z=cv->carbboxmax.z;
	cv->p[12].x=cv->carbboxmax.x;
	cv->p[12].y=cv->carbboxmin.y;
	cv->p[12].z=cv->carbboxmin.z;
	cv->p[13].x=cv->carbboxmax.x;
	cv->p[13].y=cv->carbboxmin.y;
	cv->p[13].z=cv->carbboxmax.z;
	cv->p[14].x=cv->carbboxmax.x;
	cv->p[14].y=cv->carbboxmax.y;
	cv->p[14].z=cv->carbboxmin.z;
	cv->p[15].x=cv->carbboxmax.x;
	cv->p[15].y=cv->carbboxmax.y;
	cv->p[15].z=cv->carbboxmax.z;
	// 2 midpoints, assumes cars length > width > height
	cv->p[16].x=(cv->carbboxmax.x+cv->carbboxmin.x)/2;
	cv->p[16].y=(cv->carbboxmax.y+cv->carbboxmin.y)/2;
	cv->p[16].z=cv->carbboxmin.z+(cv->carbboxmax.x-cv->p[16].x);
	cv->p[17].x=(cv->carbboxmax.x+cv->carbboxmin.x)/2;
	cv->p[17].y=(cv->carbboxmax.y+cv->carbboxmin.y)/2;
	cv->p[17].z=cv->carbboxmax.z-(cv->carbboxmax.x-cv->p[16].x);

//	quat2xform(&co->rot,&local2norot);
//	xformvecs(&local2norot,cv->p,tv->pr,NCARPOINTS);
	quatrots(&co->rot,cv->p,tv->pr,NCARPOINTS);
	for (i=0;i<8;i++) {
		tv->c2cpnts[i].x=tv->pr[i+RECTPOINTOFF].x+co->pos.x;
		tv->c2cpnts[i].y=tv->pr[i+RECTPOINTOFF].y+co->pos.y;
		tv->c2cpnts[i].z=tv->pr[i+RECTPOINTOFF].z+co->pos.z;
	}

//		op->moi=op->mass*(op->len.x*op->len.x/12.0f+op->len.y*op->len.y/3.0f);
/*		if (ol_debmoi)
			op->moi=ol_debmoi;
		else
			ol_debmoi=op->moi;
		if (ol_debmass)
			op->mass=ol_debmass;
		else
			ol_debmass=op->mass; */
//	}
}

#if 0
static bool st2_line2road(const pointf3* top,const pointf3* bot,pointf3* bestintsect,pointf3* bestnorm)
{
	float t;
	float testheight = 0;
	if (top->y>=testheight+EPSILON && bot->y<=testheight-EPSILON) { // line does cross 0 plane
		if (bestnorm) {
			bestnorm->x=0;
			bestnorm->y=1;
			bestnorm->z=0;
		}
		t=(testheight-top->y)/(bot->y-top->y);
		interp3d(top,bot,t,bestintsect);
		return 1;
	}
	return 0;
}
#endif

#if 1
static bool line2road(pointf3* pnt,pointf3* normjj=0) // reads and writes both pnt and rot
{
	trk* coll=gg->e.oldtrackj;
	bool ret = coll->line2road(pnt,normjj);
//	if (ret == true)
//		logger("line2road returns true\n");
	return ret;
}
#else
static bool line2road(pointf3* pnt,pointf3* normjj=0) // reads and writes both pnt and rot
{
	if (pnt->y >= 0)
		return false;
	pnt->y = 0;
	*normjj=pointf3x(0,1,0);
	return true;
}
#endif

static float shockcolliding(twcaro *co,int shn,const pointf3 *topshock,const pointf3 *botshock,
							pointf3 *colpoint,pointf3 *contactforce)
{ // impdircontact is unit force from road, ret value is magnitude
	float contact;//,contact2;
	float oldshocklen,shockchange;
	pointf3 cf;
	float den;
	pointf3x perp; //,ts;
	oldshocklen=co->shocklen[shn];
	// do collision
#if 1
//	line2road
	*colpoint = *botshock;
	if (!line2road(colpoint,&perp)) {
		co->shocklen[shn]=cv->extrashocklen[shn];
		return 0;
	}
#else
	if (!st2_line2road(topshock,botshock,colpoint,&perp)) {
		co->shocklen[shn]=cv->extrashocklen[shn];
		return 0;
	}
#endif
// shock colliding with road
//	if (oi->ol_lastairtime==0)
//		oi->ol_lastairtime=oi->ol_airtime;
//	logger("shock airtime = 0\n");
//	oi->ol_airtime=0;
	// get compression distance
// move pen to intsect, for point2road
	pointf3 tvec;
	tvec.x = topshock->x - botshock->x;
	tvec.y = topshock->y - botshock->y;
	tvec.z = topshock->z - botshock->z;
	pointf3 pvec;
	pvec.x = colpoint->x - botshock->x;
	pvec.y = colpoint->y - botshock->y;
	pvec.z = colpoint->z - botshock->z;
	float kd=dot3d(&tvec,&pvec);
	if (fabsf(kd)<.00001f) {
		contact = 0;
	} else {
		float kn = dot3d(&pvec,&pvec);
		float k = kn/kd;
		k = range(0.0f,k,1.0f);
		colpoint->x = (1.0f-k)*botshock->x + k*topshock->x;
		colpoint->y = (1.0f-k)*botshock->y + k*topshock->y;
		colpoint->z = (1.0f-k)*botshock->z + k*topshock->z;
	}

	contact=dist3d(colpoint,botshock);
	// if compressed to 0 then maxit, maybe try to stop car, bottom out
	if (contact>cv->extrashocklen[shn])
		contact=cv->extrashocklen[shn];
	// get shocklength
	co->shocklen[shn]=cv->extrashocklen[shn]-contact;
	// shock velocity should use timinc
	shockchange=(co->shocklen[shn]-oldshocklen);//*TIMEINC/TIMEINCconst;
	// calc force
	contact=contact*cv->shockstr-cv->shockdamp*shockchange;
	if (contact<0)
		return 0; // road is moving faster away then the wheels, wheels not touching ground
	// calc unit dir of contact force
	cf.x=topshock->x-botshock->x;
	cf.y=topshock->y-botshock->y;
	cf.z=topshock->z-botshock->z;
	if (normalize3d(&cf,&cf)==0)
		return 0;
	// put in the magnitude of contact force
	// project it to normal, should be amplified, not diminished FIX
//	if (proj3d(&cf,&perp,contactforce)==0)
//		return 0;
	den=dot3d(&cf,&perp);
//	den=1/den;
//	if (den>5)
//		den=5;
//	den=1;
	// convert force to mag/dir
	*contactforce=perp;
	return contact*den*TIMEINC;
//	return contact2*contact*timeinc;
}

static void fricbox2road(twcaro *co,float fric,
				 pointf3 *r1,pointf3 *impdir,float contact,pointf3 *impdirfric)
{
	pointf3 va,vb;
	float impfric;
	float dt;
	pointf3 crs;
	pointf3 vtrans;
	pointf3 vrot;
	float t;
	if (fric==0) {
		impdirfric->x=0;
		impdirfric->y=0;
		impdirfric->z=0;
		return;
	}
// get velocity of point on box
	cross3d(&co->rotvel,r1,&vrot); // vrot
	vtrans=co->vel;	// vtrans
// now project velocity of point onto ground
	va.x=vrot.x+vtrans.x;
	va.y=vrot.y+vtrans.y;
	va.z=vrot.z+vtrans.z;
	t=dot3d(&va,impdir);
	va.x-=t*impdir->x;
	va.y-=t*impdir->y;
	va.z-=t*impdir->z;
// get direction of friction force
	impdirfric->x=-va.x;
	impdirfric->y=-va.y;
	impdirfric->z=-va.z;
	if (!normalize3d(impdirfric,impdirfric))
		return;
	cross3d(r1,impdirfric,&crs);
	vb.x=impdirfric->x/cv->mass+crs.x/cv->moi;
	vb.y=impdirfric->y/cv->mass+crs.y/cv->moi;
	vb.z=impdirfric->z/cv->mass+crs.z/cv->moi;
	dt=dot3d(&vb,impdirfric);
	if (dt) {
		if (contact<0)
			error("hey contact less than 0");
		impfric=-dot3d(&va,impdirfric)/dt;
		impdirfric->x*=impfric*fric;
		impdirfric->y*=impfric*fric;
		impdirfric->z*=impfric*fric;
	} else {
		impdirfric->x=0;
		impdirfric->y=0;
		impdirfric->z=0;
	}
}

static float collbox2road(twcaro *co,pointf3 *r1,pointf3 *impdir,pointf3 *impdircontact)
{
	float impmag;
	pointf3 r1cd;
	pointf3 rotvel,veldiff;
// first check to see if point moving into road
	cross3d(&co->rotvel,r1,&rotvel);
// velocity of point1 rel to road
	veldiff.x=co->vel.x+rotvel.x;
	veldiff.y=co->vel.y+rotvel.y;
	veldiff.z=co->vel.z+rotvel.z;
	if (dot3d(impdir,&veldiff)>=0) // return if point moving away from road
		return 0.0f;
// do physics formula
	cross3d(r1,impdir,&r1cd);
	impmag=-2*(dot3d(&co->vel,impdir)+dot3d(&co->rotvel,&r1cd))/
		(1/cv->mass+dot3d(&r1cd,&r1cd)/cv->moi);
	impmag*=cv->elast;
// done physics formula
	if (impmag<0)
		return 0.0f;
//	impmag+=.1f;
	impdircontact->x=impdir->x*impmag;
	impdircontact->y=impdir->y*impmag;
	impdircontact->z=impdir->z*impmag;
//	if (oi->ol_lastairtime==0)
//		oi->ol_lastairtime=oi->ol_airtime;
//	oi->ol_airtime=0;
//	oi->ol_cbairtime=0;
	return impmag;
}

// take vector v, project onto plane with normal n that passes through origin, return in p
static void proj2plane(pointf3 *v,pointf3 *n,pointf3 *p)
{
	float k;
	k=dot3d(v,n)/dot3d(n,n);
	p->x=v->x-k*n->x;
	p->y=v->y-k*n->y;
	p->z=v->z-k*n->z;
}

static string floatlog(float f)
{
	stringstream ss;
	ss << scientific << f;
	return ss.str();
}

static void applyforce(int desc,twcaro *co,const pointf3 *imp,const pointf3 *r1)
{
		pointf3 imptrq;
// add in impulse
#if 0	
		if (!gg->predicted && desc == -1) {
			string impxs = floatlog(imp->x);
			string masss = floatlog(cv->mass);
			string ovxs = floatlog(co->vel.x);
//			logger("af %2d imp.x %s mass %s ovx %s ",desc,impxs.c_str(),masss.c_str(),ovxs.c_str());
//			logger("af imp.x %f mass %f ovx %f ",imp->x,cv->mass,co->vel.x);
		}
#endif
		co->vel.x+=imp->x/cv->mass;
#if 0
		if (!gg->predicted) {
			string nvxs = floatlog(co->vel.x);
			logger("nvx %s\n",nvxs.c_str());
//			logger("nvx %f\n",co->vel.x);
		}
#endif
		co->vel.y+=imp->y/cv->mass;
		co->vel.z+=imp->z/cv->mass;
// add in imptorque
		cross3d(r1,imp,&imptrq);
		co->rotvel.x+=imptrq.x/cv->moi;
		co->rotvel.y+=imptrq.y/cv->moi;
		co->rotvel.z+=imptrq.z/cv->moi;
}

//void checkcar2car(int a,int b);
static void applycar2car(caroj* co);
// per co
void doroadcollisions(caroj* coj,calco* tv,const keys& k1)
{
	twcaro* co = &coj->cs;
	keys k2 = k1;
// kill car if too early or if bots done racing
	if (gg->gs.clock-gg->gs.clockoffset<acv2.candrivetime)
		k2.ku=k2.kd=k2.kl=k2.kr=0;
	cv->drivemode=CARCOAST;
	if (k2.ku /*&& drivemode!=CARBRAKE*/) { // pitch down
		if (co->accelspin>=-1) {
			float sa;
			cv->drivemode=CARACCEL;
			sa=cv->startaccelspin;//*cv->extratopspeed;
//			if (cv->speedup)
//				sa*=od.speedupspeed; //2
			if (co->accelspin>sa)
				cv->drivemode=CARCOAST;
		} else {
			cv->drivemode=CARBRAKE;
		}
	}
	if (k2.kd ) { // pitch up
		if (co->accelspin<1) {
			cv->drivemode=CARREVERSEACCEL;
			if (co->accelspin<-cv->startaccelspin/2)
				cv->drivemode=CARCOAST;
		} else {
			cv->drivemode=CARBRAKE;
		}
	}
	if (!k2.ku && !k2.kd)
		cv->drivemode=CARCOAST;
//	if (op->slowdown && co->accelspin>pfrand1(op->clocktickcount)*op->ol_startaccelspin*op->extratopspeed)
//		op->ol_drivemode=CARBRAKE;
// 3d points
//	extern float ol_startaccelspin;
//	extern int carboost;
// 3d impulse directions
	pointf3 impdirs[13];
// 3d impulses
	pointf3 impulse;//,impfric; //,imptorque;
// motion
//	pointf3 savevel;
//	pointf3 savevelang;
//	pointf3 vel;
//	mat4 jx;
// iteraters
	int j;//i,j,k;
//	int ncols;
// objects
//	struct ol_playerdata *oi=op;
// driving
//	pointf3 norm;
//////////////////////// extra forces
	doairfric(co,cv->airfric);
	dograv(co,cv->littleg);
	applycar2car(coj);
//	logger("------------ road collisions -------------------\n"
//	"   before extratrans, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
//#define DOEXTRATRANS // like jets in the back, good for 'body' scraping tests
#ifdef DOEXTRATRANS
	{
		pointf3x vel;
		if (k2.ku)
			vel.z=+.1f;
		if (k2.kd)
			vel.z-=.1f;
//		quat2xform(&oi->carang,&jx);
//		xformvec(&jx,&vel,&vel);
		quatrot(&co->rot,&vel,&vel);
		co->vel.x+=vel.x;
		co->vel.y+=vel.y;
		co->vel.z+=vel.z;
	}
#endif
	nanerr("afterxtrans posx",co->pos.x);
	nanerr("afterxtrans posy",co->pos.y);
	nanerr("afterxtrans posz",co->pos.z);
	nanerr("afterxtrans velx",co->vel.x);
	nanerr("afterxtrans vely",co->vel.y);
	nanerr("afterxtrans velz",co->vel.z);
//	float validroadnorm=0;
//	logger(" before doshock2road, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
#if 0
	if (oi->carboost>0) {
		pointf3 topvel;//,unitvel;
		float tween,mag;
		mag=oi->carvel.x*oi->carvel.x+oi->carvel.z*oi->carvel.z;
		if (mag>EPSILON) {
			mag=(float)sqrt(mag);
			topvel.x=oi->carvel.x/mag*oi->ol_startaccelspin*oi->ol_wheelrad;
//			topvel.y=unitvel.y*startaccelspin*wheelrad;
			topvel.z=oi->carvel.z/mag*oi->ol_startaccelspin*oi->ol_wheelrad;
/*			switch(oi->carboost) {
			case 1:	tween=.5f;
					break;
			case 2: tween=.75f;
					break;
			case 3:	tween=.8f;
					break;
			case 4:	tween=.9f;
					break;
			default:tween=1;
					break;
			} */
			tween=1;
			oi->carvel.x=oi->carvel.x+(topvel.x-oi->carvel.x)*tween;
//			oi->carvel.y=oi->carvel.y+(topvel.y-oi->carvel.y)*tween;
			oi->carvel.z=oi->carvel.z+(topvel.z-oi->carvel.z)*tween;
		}
		oi->carboost=0;
	}
#endif
#define DOSHOCKS2ROAD
#ifdef DOSHOCKS2ROAD
	float impmags[13];
//	nocrash=0;
	pointf3 carnorm,cardir;
	carnorm.x=tv->pr[0].x-tv->pr[1].x; // car up
	carnorm.y=tv->pr[0].y-tv->pr[1].y;
	carnorm.z=tv->pr[0].z-tv->pr[1].z;
	cardir.x=tv->pr[0].x-tv->pr[4].x; // car forward
	cardir.y=tv->pr[0].y-tv->pr[4].y;
	cardir.z=tv->pr[0].z-tv->pr[4].z;
	pointf3 carvelnorm=co->vel;
//	float carvelmag=normalize3d(&carvelnorm,&carvelnorm);
	normalize3d(&cardir,&cardir);
	normalize3d(&carnorm,&carnorm);
	float shockmag;
	for (j=0;j<NSHOCKPOINTS/2;j++) { // world
		pointf3xh r1;
		pointf3xh impdir; // unit vector
		pointf3 outerpoint,innerpoint,colpoint;
		innerpoint.x=co->pos.x+tv->pr[2*j].x; // top of shock
		innerpoint.y=co->pos.y+tv->pr[2*j].y;
		innerpoint.z=co->pos.z+tv->pr[2*j].z;
		outerpoint.x=co->pos.x+tv->pr[2*j+1].x; // bot of shock
		outerpoint.y=co->pos.y+tv->pr[2*j+1].y;
		outerpoint.z=co->pos.z+tv->pr[2*j+1].z;
		if ((shockmag=shockcolliding(co,j,&innerpoint,&outerpoint,&colpoint,&impdir))>0) { // impdir returned is a unit vector
			r1.x=colpoint.x-co->pos.x;
			r1.y=colpoint.y-co->pos.y;
			r1.z=colpoint.z-co->pos.z;
			impulse.x=shockmag*impdir.x;
			impulse.y=shockmag*impdir.y;
			impulse.z=shockmag*impdir.z;
			applyforce(j,co,&impulse,&r1); // shocks push on road
		}
		impmags[j]=shockmag;
		impdirs[j]=impdir;
#if 0
		if (!gg->predicted) {
			string shockmags = floatlog(shockmag);
			string impdirs = impdir.log();
			string r1s = r1.log();
			logger("%d shockmag %s impdir %s r1 %s\n",j,shockmags.c_str(),impdirs.c_str(),r1s.c_str());
		}
#endif
	}
#endif
// friction, driving, calculate velocity of wheels on ground
	nanerr("aftershock posx",co->pos.x);
	nanerr("aftershock posy",co->pos.y);
	nanerr("aftershock posz",co->pos.z);
	nanerr("aftershock velx",co->vel.x);
	nanerr("aftershock vely",co->vel.y);
	nanerr("aftershock velz",co->vel.z);
//	logger("before shockfriction, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
#define DOSHOCKFRICTION
#ifdef DOSHOCKFRICTION
	{
		float shockmag=0;
		pointf3xh shocknorm=zerov;
		for (j=0;j<NSHOCKPOINTS/2;j++) {
			if (impmags[j]>0) {
				shockmag+=impmags[j];
				shocknorm.x+=impdirs[j].x;
				shocknorm.y+=impdirs[j].y;
				shocknorm.z+=impdirs[j].z;
			}
		}
#if 0
	if (!gg->predicted) {
		string shockmags = floatlog(shockmag);
		string shocknorms = shocknorm.log();
		logger("shockmag %s shocknorm %s\n",shockmags.c_str(),shocknorms.c_str());
	}
#endif
// strange check
		if (shockmag>0) {
			if (normalize3d(&shocknorm,&shocknorm)==0) {
				logger("roadnorm (%f,%f,%f)\n",shocknorm.x,shocknorm.y,shocknorm.z);
				for (j=0;j<NSHOCKPOINTS/2;j++)
					logger(" impdirs[%d] = (%f,%f,%f), impmags[%d] = %f\n",j,
						impdirs[j].x,impdirs[j].y,impdirs[j].z,j,impmags[j]);
//			errorexit("cant norm 1");
				shockmag = 0;
			}
		}
		if (shockmag>0) {
//			validroadnorm=1;
//		oi->ol_lastroadnorm=oi->shocknorm;
//		quat2xform(&co->rot,&jx);
//		xformvec(&jx,&bases,&bases);
// slide
			pointf3 bases={1,0,0};
			quatrot(&co->rot,&bases,&bases);
			proj2plane(&bases,&shocknorm,&bases);
			if (normalize3d(&bases,&bases)==0)
				errorexit("cant norm 2");
			pointf3 ps;
			if (!proj3d(&co->vel,&bases,&ps))
				errorexit("can't proj3d 1");
			ps.x*=cv->mass;
			ps.y*=cv->mass;
			ps.z*=cv->mass;
			float fts;
			fts=cv->fricttireslide;//*oi->extratraction;
			pointf3 is; // impusle slide
			is.x=fts*shockmag*bases.x;
			is.y=fts*shockmag*bases.y;
			is.z=fts*shockmag*bases.z;
			if (dot3d(&ps,&bases)>0) {
				is.x=-is.x;
				is.y=-is.y;
				is.z=-is.z;
			}
			pointf3 ps2;
			ps2.x=ps.x+is.x;
			ps2.y=ps.y+is.y;
			ps2.z=ps.z+is.z;
			if (dot3d(&ps,&ps2)<=0) {
				is.x=-ps.x;
				is.y=-ps.y;
				is.z=-ps.z;
			}
// roll
			pointf3xh baser;//(0,0,1,0);
			cross3d(&bases,&shocknorm,&baser);
			float spd=dot3d(&co->vel,&baser);
			co->accelspin=spd;
			pointf3 pr;
			if (!proj3d(&co->vel,&baser,&pr))
				errorexit("can't proj3d 2");
#if 0
		if (!gg->predicted) {
			string vels = co->vel.logh();
			string basers = baser.logh();
			string prs = pr.logh();
			logger("vel %s baser %s pr %s\n",vels.c_str(),basers.c_str(),prs.c_str());
		}
#endif
			pr.x*=cv->mass;
			pr.y*=cv->mass;
			pr.z*=cv->mass;
// force on roll
			float fr;
			if (cv->drivemode==CARBRAKE)
				fr=fts;
			else if (cv->drivemode!=CARACCEL)
				fr=cv->fricttireroll;
			else
				fr=0;
/*		if (oi->vinoil) {
			fts*=.1f;
			fr*=.1f;
		} */
			pointf3 ir; // impulse roll
			ir.x=fr*shockmag*baser.x;
			ir.y=fr*shockmag*baser.y;
			ir.z=fr*shockmag*baser.z;
			if (dot3d(&pr,&baser)>0) {
				ir.x=-ir.x;
				ir.y=-ir.y;
				ir.z=-ir.z;
			}
			pointf3 pr2;
			pr2.x=pr.x+ir.x;
			pr2.y=pr.y+ir.y;
			pr2.z=pr.z+ir.z;
			if (dot3d(&pr,&pr2)<=0) {
				ir.x=-pr.x;
				ir.y=-pr.y;
				ir.z=-pr.z;
			}
			if (cv->drivemode==CARACCEL /*|| oi->extrapouncer*/) {
				float wa,sas,wae;
//				wa=wheelaccel;
				sas=cv->startaccelspin;//*oi->extratopspeed;
				wae=cv->wheelaccel;//*oi->extraaccel;
				if (co->accelspin<sas*.5f)
					wa=wae;
				else if (co->accelspin<sas*.75f)
					wa=wae*.5f;
				else if (co->accelspin<sas*.9f)
					wa=wae*.1f;
				else
					wa=wae*.01f;
/*			if (oi->vinoil)
				wa*=.1f; */
//			wa=wheelaccel*(1.0f-(float)sqrt(fabs(accelspin/startaccelspin)));
				ir.x+=wa*baser.x*shockmag;
				ir.y+=wa*baser.y*shockmag;
				ir.z+=wa*baser.z*shockmag;
			} else if (cv->drivemode==CARREVERSEACCEL) {
				ir.x-=cv->wheelaccel*baser.x*shockmag;
				ir.y-=cv->wheelaccel*baser.y*shockmag;
				ir.z-=cv->wheelaccel*baser.z*shockmag;
			}
			pointf3 impf;
			impf.x=ir.x+is.x;
			impf.y=ir.y+is.y;
			impf.z=ir.z+is.z;
#if 0
		if (!gg->predicted) {
			string irxs = floatlog(ir.x);
			string isxs = floatlog(is.x);
			logger("irx %s isx %s\n",irxs.c_str(),isxs.c_str());
		}
#endif
			applyforce(-1,co,&impf,&zerov);
			fr=dot3d(&co->vel,&baser);
//		for (j=0;j<NSHOCKPOINTS/2;j++) // calc wheel vels
//			cv->wheelvel[j]=fr;
//	} else {
//		for (j=0;j<NSHOCKPOINTS/2;j++) // calc wheel vels
//			cv->wheelvel[j]=0; // fixed 2011-3-2
			if (!gg->intimewarp && co->noshocktime!=0 /* && &gg->caros[gg->viewslot].cs == co */)
				sfx->playsnd(&co->pos,LAND);
			co->noshocktime=0; // shocks are touching ground
		} else {
			++co->noshocktime;
		}
		++co->noresettime;
//		if (co->noshocktime==cv->maxnoshocktime) {
		if (k2.kresetcar && co->noresettime>cv->maxnoresettime) {
//		if (co->noshocktime==cv->maxnoshocktime || k2.kd) {
			co->noresettime = 0; // flip car back over
			co->noshocktime = 0;
			co->nocheckpointtime = 0;
			gg->e.oldtrackj->line2roadvert(&co->pos);
			co->pos.y += cv->carflipheight;
//			co->lastpos = co->pos;
			co->vel=pointf3x();
			co->rotvel=pointf3x();
//			gg->viewslot = coj - &gg->caros[0];
//			if (gg->viewslot < 0 || gg->viewslot>=gg->numplayers)
//				errorexit("bad viewslot");
			pointf3x nl;
			if (co->nnextloc>1) { // fork choice
				S32 idx = gg->gs.clock >> 5; // reset nextloc dependent on time
				idx &= 1;
				nl.x=PIECESIZE*(co->nextloc[idx].x*1.0f+.5f); // middle of piece
				nl.z=PIECESIZE*(co->nextloc[idx].y*1.0f+.5f);
			} else {
				nl.x=PIECESIZE*(co->nextloc[0].x*1.0f+.5f); // middle of piece
				nl.z=PIECESIZE*(co->nextloc[0].y*1.0f+.5f);
			}
			float x=nl.x-co->pos.x;
			float z=nl.z-co->pos.z;
			float ang = atan2f(x,z);
			co->rot=pointf3x(0,1,0,ang);
			rotaxis2quat(&co->rot,&co->rot);
			if (!gg->intimewarp /* && &gg->caros[gg->viewslot].cs == co */)
				sfx->playsnd(&co->pos,HORN);
		}
	}
#endif
//	logger("      before flipcar, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
//#define FLIPCAR
#ifdef FLIPCAR
	{
		pointf3 r1={0,1,0};
		pointf3 frc={.1f,0,0};
		if (!flymode && validroadnorm && dot3d(&lastroadnorm,&carnorm)<0) {
			applyforce(oi,&frc,&r1);
			r1.y=-1;
			frc.x=-.1f;
			applyforce(oi,&frc,&r1);
		}
	}
#endif
//////////////////////// carbody to road, do this one LAST! //////////
	nanerr("afterfric posx",co->pos.x);
	nanerr("afterfric posy",co->pos.y);
	nanerr("afterfric posz",co->pos.z);
	nanerr("afterfric velx",co->vel.x);
	nanerr("afterfric vely",co->vel.y);
	nanerr("afterfric velz",co->vel.z);
//	logger(" before carbody2road, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
#define DOCARBODY2ROAD
#ifdef DOCARBODY2ROAD
	float fb;
//	int nocrash;
	pointf3 r1s[12]; // relative collision points
	pointf3 dvels[9],dvelangs[9];
/*	if (shockmag<=0) {
		oi->ol_hifrictime++;
		if (oi->ol_hifrictime>20) {
			oi->ol_hifrictime=20;
			fb=oi->ol_frictcarbodyroof; // hi friction when no shocks and upsidedown
		} else
			fb=oi->ol_frictcarbody; // lo friction , oops forgot this...
	} else {
		oi->ol_hifrictime=0;*/
		fb=cv->frictcarbody; // lo friction
//	}
	pointf3 savevel=co->vel;
	pointf3 savevelang=co->rotvel;
// collect upto 8 collision points from the car's bounding box
	int ncols=0;
	for (j=0;j<NRECTPOINTS;j++) {
		pointf3 outerpoint;
		outerpoint.x=co->pos.x+tv->pr[RECTPOINTOFF+j].x;
		outerpoint.y=co->pos.y+tv->pr[RECTPOINTOFF+j].y;
		outerpoint.z=co->pos.z+tv->pr[RECTPOINTOFF+j].z;
		int k=j&1;
		pointf3 innerpoint;
		innerpoint.x=co->pos.x+tv->pr[MIDPOINTOFF+k].x;
		innerpoint.y=co->pos.y+tv->pr[MIDPOINTOFF+k].y;
		innerpoint.z=co->pos.z+tv->pr[MIDPOINTOFF+k].z;
		pointf3xh impdir; // unit vector
		pointf3x r1;

		pointf3 colpoint = outerpoint;
		if (line2road(&colpoint,&impdir)) {
			r1.x=outerpoint.x-co->pos.x;
			r1.y=outerpoint.y-co->pos.y;
			r1.z=outerpoint.z-co->pos.z;
			r1s[ncols]=r1;
			impdirs[ncols]=impdir;
			ncols++;
		}
	}
//#define DETECTFALLTHRUROAD
// can't get it to work
#ifdef DETECTFALLTHRUROAD
	if (st2_line2road(&oi->ol_ulastloc,&oi->pos,&colpoint,&impdir)) {
//			logger("special kludge fix fall thru road\n");
//		oi->pos=ulastloc;
//		oi->carvelang=zerov;
//		oi->carang=zerov;
//		oi->carang.w=1;
//		playasound(54,0); // watermelon sound
		r1s[ncols]=zerov;
		impdirs[ncols]=impdir;
		ncols++;
	}
#endif
// calculate ncol different ways, then avg it
	int k;
	for (k=0;k<ncols;k++) {
		co->vel=savevel;
		co->rotvel=savevelang;
		int i;
		for (i=0;i<ncols;i++) {
			j=k+i;
			if (j>=ncols)
				j-=ncols;
			float carbodymag=collbox2road(co,&r1s[j],&impdirs[j],&impulse);
			if (carbodymag>0) {
// get friction of carbody on road
//				logger("carbodymag = %9.3f\n",carbodymag);
//				logger("impulse = %9.3f,%9.3f,%9.3f\n",impulse.x,impulse.y,impulse.z);
//				if (oi->ol_lastairtime==0)
//					oi->ol_lastairtime=oi->ol_airtime;
//				logger("carbody airtime = 0\n");
//				oi->ol_airtime=0;
//				oi->ol_cbairtime=0;
//				if (impmag>2.5f)
//					airtime=-1;

				pointf3x impfric;
				fricbox2road(co,fb,&r1s[j],&impdirs[j],carbodymag,&impfric);
//				logger("impfric  = %9.3f,%9.3f,%9.3f\n",impfric.x,impfric.y,impfric.z);
				impulse.x+=impfric.x;
				impulse.y+=impfric.y;
				impulse.z+=impfric.z;

// add in impulse
//				logger("i %d, j %d, k %d, ncols %d, applyforce %9.3f %9.3f %9.3f at %9.3f %9.3f %9.3f\n",
//					i,j,k,ncols,impulse.x,impulse.y,impulse.z,r1s[j].x,r1s[j].y,r1s[j].z);
				applyforce(-2,co,&impulse,&r1s[j]); // friction box
			}
		}
		dvels[k]=co->vel;
		dvelangs[k]=co->rotvel;
	}
// average different methods
	if (ncols) {
		co->vel=zerov;
		co->rotvel=zerov;
		for (k=0;k<ncols;k++) {
			co->vel.x+=dvels[k].x;
			co->vel.y+=dvels[k].y;
			co->vel.z+=dvels[k].z;
			co->rotvel.x+=dvelangs[k].x;
			co->rotvel.y+=dvelangs[k].y;
			co->rotvel.z+=dvelangs[k].z;
		}
		co->vel.x/=ncols;
		co->vel.y/=ncols;
		co->vel.z/=ncols;
		co->rotvel.x/=ncols;
		co->rotvel.y/=ncols;
		co->rotvel.z/=ncols;
	}
#endif
//	logger("  after carbody2road, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
// slow down yspin
//	if (nocrash)
//		airtime=0;
//	oi->carvelang.y*=.95f;
//	airtime=0;
// if no shock info get it from carbody
/*	if ( !validroadnorm && ncols>0) {
		oi->ol_lastroadnorm=zerov;
		for (k=0;k<ncols;k++) {
			oi->ol_lastroadnorm.x+=impdirs[k].x;
			oi->ol_lastroadnorm.y+=impdirs[k].y;
			oi->ol_lastroadnorm.z+=impdirs[k].z;
		}
		normalize3d(&oi->ol_lastroadnorm,&oi->ol_lastroadnorm);
		validroadnorm=1;
	} */
	nanerr("aftercarbody2road posx",co->pos.x);
	nanerr("aftercarbody2road posy",co->pos.y);
	nanerr("aftercarbody2road posz",co->pos.z);
	nanerr("aftercarbody2road velx",co->vel.x);
	nanerr("aftercarbody2road vely",co->vel.y);
	nanerr("aftercarbody2road velz",co->vel.z);
#define TESTIMPULSE
#ifdef TESTIMPULSE
	if (!wininfo.releasemode) {
		if (KEY=='i' && gg->n.gamenet==GAMENET_BOTGAME)
			applyforce(-4,co,&cv->testimpval,&cv->testimppnt); // test impulse
	}
/*	if (a->carid==0 && b->carid==1) {
		if (KEY=='i' && ncollinfo+1<MAXCOLLINFO) {
			collinfos[ncollinfo].impval=testimpval;
			collinfos[ncollinfo].imppnt=testimppnt;
			collinfos[ncollinfo].cn=0;
			ncollinfo++; */
#endif
/*	if (oi->ol_ufliptime>140 && !oi->ol_doacrashreset) {
//		playasound(18,0);
//		utotalcrashes++;
		oi->ol_ufliptime=0;
		oi->ol_doacrashreset=1;
//		logger("car reset: flipped over\n");
	}
	if (!validroadnorm || dot3d(&oi->carnorm,&oi->ol_lastroadnorm)>.1f)
		oi->ol_ufliptime=0;
*/
/*	if (validroadnorm && oi->carvelmag>5 && !oi->ol_doacrashreset && oi->ol_flymode && !oi->dofinish) {
//		pointf3 carpdir,carpvelnorm;
//		float dt;
		if (dot3d(&oi->carnorm,&oi->ol_lastroadnorm)<.25f ) {
			if (oi->ol_lastairtime>60) {
				oi->ol_airtime=-1; // signal a crash pitch
			}
		} */
/*		else if (oi->carvel.x*oi->carvel.x+oi->carvel.z*oi->carvel.z>12) { // yaw
			proj2plane(&cardir,&lastroadnorm,&carpdir);
			proj2plane(&carvelnorm,&lastroadnorm,&carpvelnorm);
			normalize3d(&carpdir);
			normalize3d(&carpvelnorm);
			dt=dot3d(&carpdir,&carpvelnorm);
			if (dt<.5f && dt>-.5f)
				airtime=-1;
		}
	}*/
#if 0
	oi->ol_lastairtime=0;
	if (oi->ol_airtime==-1) {
//		int deferx,deferz;
//		logger("random random bone deform for car crash\n");
/*		deferx=random(2);
		deferz=random(3);
		oi->db_curpushin[deferx][deferz]=ol_dembonesc[deferx][deferz].dampushin; */
//		logger("random crash sound\n");
		ol_playatagsound(18+random(3),oi->vold,oi->pan);
		oi->ol_utotalcrashes++;
//		logger("car reset: crashed2\n");
		oi->ol_doacrashreset=1;
	}
#endif
}

/////////////// move
void updaterot(twcaro *co,const keys& k)
{
//	int i;
	pointf3 q;//,q2,q3;
//	JXFORM jx;
	float t;
//	float ramp;
//	struct  *op;
//	for (op=ol_objects,i=0;i<NUMOBJECTS;i++,op++) {
/*		if (op->ol_flymode && op->ol_startstunt) {
			pointf3 carstuntrpyrad;
			carstuntrpyrad.x=op->carstuntrpy.x*PIOVER180;
			carstuntrpyrad.y=op->carstuntrpy.y*PIOVER180;
			carstuntrpyrad.z=op->carstuntrpy.z*PIOVER180;
			rpy2quat(&carstuntrpyrad,&op->carang);
		} else { */
//			float dt,dt2;
//			float delhead=0;
//			pointf3 newvel;
//			int l,r;
//			if (op->vinemb) {
//				l=op->pi.ol_uright;
//				r=op->pi.ol_uleft;
//			} else {
/*				r=op->pi.ol_uright;
				l=op->pi.ol_uleft; */
//			}
//#define DOEXTRAHEADING
#ifdef DOEXTRAHEADING
			pointf3 q2=zerov; // rot add local
			float prodhead = cv->defuextraheading * cv->maxuextraheading;
			float uextraheading = cv -> defuextraheading;
//			if(urollright && !uready && (uleft || uright))
//				uextraheading +=1.5;
//			else
			float ramp = 0;
			if (co->accelspin>prodhead)
				ramp = 1;
			else if (co->accelspin<-prodhead)
				ramp = 1;
			else
				ramp = (float)fabs(co->accelspin/prodhead);
			ramp = 1;
			ramp *= cv->maxuextraheading;
//			if(uextraheading>maxuextraheading)
//				uextraheading = maxuextraheading;
			quatinverse(&co->rot,&q);
//			mat4 jx;
//			quat2xform(&q,&jx);
//			xformvec(&jx,&co->rotvel,&q); // q is car relative rotaxis velocity
			quatrot(&q,&co->rotvel,&q); // q is rotvel local, q2 is rotvel add local
			keys k2 = k;
			if (gg->gs.clock-gg->gs.clockoffset<gcfg.candrivetime)
				k2.ku=k2.kd=k2.kl=k2.kr=0;
//			k2.kl = 1;
			if (k2.kr /* && cv->accelspin>0 || k2.kl && cv->accelspin<=0 */)
				q2.y += uextraheading;
			if (k2.kl /* && cv->accelspin>0 || k2.kr && cv->accelspin<=0 */)
				q2.y -= uextraheading;
			if (!k2.kr && !k2.kl) { // straighten out
				if (q.y>0) {
					q2.y = -uextraheading;
					if (q2.y+q.y<0)
						q2.y = -q.y;
				} else if (q.y<0) {
					q2.y = uextraheading;
					if (q2.y+q.y>0)
						q2.y = -q.y;
				}
			}
			pointf3 q3;
			q3.x=q.x;
			q3.y=q2.y+q.y;
			q3.z=q.z;
			if (q3.y>ramp)
				q2.y=0;
			if (q3.y<-ramp)
				q2.y=0;
//			if (q3.y*q.y<=0)
//				q2.y=0;
//			quat2xform(&co->rot,&jx);
//			xformvec(&jx,&q2,&q2);
			quatrot(&co->rot,&q2,&q2); // back to world
			co->rotvel.x+=q2.x;
			co->rotvel.y+=q2.y;
			co->rotvel.z+=q2.z;
#endif
#define DOEXTRAHEADING2
#ifdef DOEXTRAHEADING2
			q.x=0;
			q.y=1;
			q.z=0;
// gradual steering
			keys k2 = k;
			if (k2.kr || k2.kl)
				co->steertime++;
			else
				co->steertime=2;
			if (co->steertime>8)
				co->steertime=8;
			float uextraheading;
//			float prodhead = cv->defuextraheading * cv->maxuextraheading;
			if (k2.kr || k2.kl)
				uextraheading=cv->defuextraheading*co->steertime/8;
			else
				uextraheading=cv->defuextraheading; // don't gradual when trying to go straight
// end gradual steering
			float ramp;
#ifdef DOEXTRATRANS
			ramp = 1;
#else
			if (co->accelspin>cv->maxturnspeed)
				ramp = 1;
			else if (co->accelspin<-cv->maxturnspeed)
				ramp = 1;
			else
				ramp = (float)fabsf(co->accelspin/cv->maxturnspeed);
#endif
			ramp*=cv->maxuextraheading;
//			quat2xform(&co->rot,&jx);
//			xformvec(&jx,&q,&q); // q is in world, direction of y axis of rotvel car in rotaxis
			quatrot(&co->rot,&q,&q); // q is in world, direction of y axis of rotvel car in rotaxis
			float dt=dot3d(&q,&co->rotvel); // positive is turn clockwise relative to car
			float delhead = 0;
			if (k2.kr && co->accelspin>=0 || k2.kl && co->accelspin<0)
				delhead+=uextraheading;
			if (k2.kl && co->accelspin>=0 || k2.kr && co->accelspin<0)
				delhead-=uextraheading;
			if (!k2.kl && !k2.kr) { // straighten out
				if (dt>0)
					delhead-=uextraheading;
				else if (dt<0)
					delhead+=uextraheading;
			}
			pointf3x newrotvel;
			newrotvel.x=co->rotvel.x+delhead*q.x;
			newrotvel.y=co->rotvel.y+delhead*q.y;
			newrotvel.z=co->rotvel.z+delhead*q.z;
			newrotvel.w=0;
			float dt2=dot3d(&q,&newrotvel); // limits and 0, (what is this?)
			if (dt2>ramp) { // move from >ramp to ramp
				newrotvel.x+=(ramp-dt2)*q.x;
				newrotvel.y+=(ramp-dt2)*q.y;
				newrotvel.z+=(ramp-dt2)*q.z;
			} else if (dt2<-ramp) { // move from <-ramp to -ramp
				newrotvel.x+=(-dt2-ramp)*q.x;
				newrotvel.y+=(-dt2-ramp)*q.y;
				newrotvel.z+=(-dt2-ramp)*q.z;
			} else if (dt*dt2<0 && !k2.kl && !k2.kr) { // move from dot2 to 0
				newrotvel.x-=dt2*q.x;
				newrotvel.y-=dt2*q.y;
				newrotvel.z-=dt2*q.z;
			}
			if (true)
//			if (co == &gg->caros[0].cs) // only player 0
				co->rotvel=newrotvel;
#endif
			q=co->rotvel;
			t=normalize3d(&q,&q);
			if (t) { // finally update rot from rotvel
//				t*=PIUNDER180;
				t*=TIMEINC;
				q.w=t;
/*				if (op->venableicrms) {
					q.w=0;
//					q.w*=op->vicrmsparm;
//					if (op->vicrmsparm==0)
//						co->rotvel=zerov;
				} else if (op->vinoil && co->accelspin>od.oilspinspeed) {
					q.x=0;
					q.y=1;
					q.z=0;
					if (op->clocktickcount&128)
						q.w=od.oilspin;
					else
						q.w=-od.oilspin;

				} else if (op->vinbuzzspin) {
					q.x=0;
					q.y=1;
					q.z=0;
//					if (op->clocktickcount&128)
						q.w=od.buzzspincar;
//					else
//						q.w=-od.oilspin;

				} else if (op->vinsonic) {
					q.x=1;
					q.y=0;
					q.z=0;
//					if (op->clocktickcount&128)
						q.w=od.sonicspincar;
//					else
//						q.w=-od.oilspin;

				} */
				rotaxis2quat(&q,&q);
				quattimes(&q,&co->rot,&co->rot); // world relative rotations
			}
//		}
//	}
}

void updatetrans(twcaro *co)
{
//	int i;
//	pointf3 hipoint,lopoint,dum,intsect;
	pointf3 vel;
//	int spc;
//	struct ol_object *op;
//	extern int ol_doacrashreset;
//	extern pointf3 ol_crashresetloc;
//	extern float ol_crashresetdir;
//	extern pointf3 ol_ulastpos;
//	pointf3 vel;
//	JXFORM jx;
//	for (op=ol_objects,i=0;i<NUMOBJECTS;i++,op++) {
//	op->ol_ulastloc=op->pos;
	vel=co->vel;
//	spc=op->pi.uspace;
// icrms
#if 0
	if (op->venableicrms) {
/*		if (!op->weapbooston && op->enableicrms==od.icrmsduration ||
			op->weapbooston && op->enableicrms==od.icrmsduration*2)
			op->icrmsparm=1;
		else { */
/*			op->vicrmsparm-=od.icrmsslowrate;
			if (op->vicrmsparm<0)
				op->vicrmsparm=0; */
//		}
		op->venableicrms--;
		if (op->venableicrms) {
/*			vel.x*=op->vicrmsparm;
			vel.y*=op->vicrmsparm;
			vel.z*=op->vicrmsparm; */
			spc=0;
//			if (op->vicrmsparm==0)
				op->carvel=zerov;
		}
	}
	if (op->vinecto) {
		vel.x*=od.ectoparm;
		vel.y*=od.ectoparm;
		vel.z*=od.ectoparm;
		op->carvel=vel;
		spc=0;
	}
	if (op->vinbuzzspin)
		op->vinbuzzspin--;
	if (op->vinoil)
		op->vinoil--;
	if (op->vinemb) {
		op->ol_nulightnings=2;
		op->vinemb--;
	}
	if (op->vinecto) {
		op->vinecto--;
//		if (op->vinecto==0)
//			op->carvel=zerov;
	}
	if (op->vinsonic) {
		if (op->vinsonic>5)
			op->ol_nulightnings=2;
		op->vinsonic--;
	}
	if (op->vinthermo) {
		op->ol_nulightnings=2;
		op->vinthermo--;
	}
	if (op->vinfusion) {
		op->ol_nulightnings=2;
		op->vinfusion--;
	}
	if (op->vinleech) {
		op->ol_nulightnings=2;
		op->vinleech--;
	}
	if (op->vinflash) {
		op->ol_nulightnings=2;
		op->vinflash--;
	}
	if (op->vunderbigtire) {
		op->ol_nulightnings=2; // test
		op->vunderbigtire--;
	}
	if (op->inghook) {
//		op->ol_nulightnings=2;
		op->inghook--;
	}
	if (op->inmagnet) {
		op->ol_nulightnings=2;
		op->inmagnet--;
	}
#endif
	// end icrms
	// for doppler
//	co->lastpos = co->pos;
	co->pos.x+=vel.x*TIMEINC;
	co->pos.y+=vel.y*TIMEINC;
	co->pos.z+=vel.z*TIMEINC;
//		if (op->pi.uspace)
//			logger("car reset: uspace\n");
#if 0
	//	if ((co->cs.pos.y<od.lowpoint || spc ) /*&& !op->ol_lastkludge */ ||
//		op->ol_doacrashreset==20 /*&&	!op->dofinish */) {
// kludge car back onto road (if fell thru the floor) , or now reset car after a crash
		op->ol_ttp=0; // new, kill tricks
		op->ol_cantdrive=60;
		if (spc)
			ol_playatagsound(39,op->vold,op->pan); // horn
		else if (!op->dofinish)
			ol_playatagsound(39,op->vold,op->pan); // give me a warp sound! (crows??)
		if (op->ol_doacrashreset)
			op->ol_doacrashreset=0; //crash reset, sound was played 20 frames ago
//		if (!op->dofinish || op->pos.y<od.lowpoint) {
//				logger("car fell thru road, restart\n");
			if (op->cntl==CNTL_HUMAN)
				od.spacebartimer=60;
//				logger("car reset: cary %f, space %d, crashreset %d\n",
//					op->pos.y,op->pi.uspace,op->ol_doacrashreset);
//				if (!op->dofinish)
//				else {
			if (op->dofinish) {
//					logger("special finish kludge fix\n");
				ol_playatagsound(39,op->vold,op->pan);
			}
//			setnewcrashloc(op);
			op->pos=op->ol_crashresetloc;
			op->carvel=zerov;
			co->rotvel=zerov;
//				quat2rpy(&op->carang,&op->carang);
//			op->carang.x=0;
//			op->carang.y=op->ol_crashresetdir;
//			op->carang.z=0;
			op->carang=op->ol_crashresetrot;
			op->seekframe=op->ol_crashresetframe+od.pathbotresetadd; // sync bot paths with crash resets..
//			rpy2quat(&op->carang,&op->carang);
			op->ol_flymode=0;
			op->ol_startstunt=op->ol_uready=0;
//			hipoint=op->pos;
//			lopoint=op->pos;
//			hipoint.y=100;
//			lopoint.y=-100;
//			st2_line2roadlo(&hipoint,&lopoint,&intsect,&dum);
//				intsect.y+=2.95f;
//			intsect.y+=olracecfg.crashresety;
//			op->pos=intsect;
			op->ol_ulastpos=op->pos;
			od.bigmove=20;
//				lastkludge=1;
//		}
//	} else {
	}
#endif
//		lastkludge=0;
//	}
//	}
//	op->ol_lastkludge=op->pi.uspace;
}

/////////////// car 2 car ///////////////
 // generate forces st -> st

/*static void doinvmoi(struct ol_playerdata *p,pointf3 *rot,pointf3 *angmomin,pointf3 *angvelout)
{
	pointf3 quat;
	if (p->norot)
		return;
	quatinverse(rot,&quat);
	quatrot(&quat,angmomin,angvelout);
	angvelout->x/=p->moivec.x;
	angvelout->y/=p->moivec.y;
	angvelout->z/=p->moivec.z;
	quatrot(rot,angvelout,angvelout);
}

static void calcimpulseo2o(struct ol_playerdata *p0,struct ol_playerdata *p1,pointf3 *loc,pointf3 *norm,pointf3 *imppnt,pointf3 *imploc)
{
	float k;
	pointf3 vang0,vang1;
	pointf3 vrel,vrel0,vrel1;	// obj 1 rel to obj 0 (obj0 space), norm is toward obj 0 away from obj 1
	pointf3 rc0,rc1;
	pointf3 force10,torque;
	float top,bot=0;
	pointf3 rcn0,rcn1,rcnr0,rcnr1;
	rc0.x=loc->x-p0->st.pos.x;
	rc0.y=loc->y-p0->st.pos.y;
	rc0.z=loc->z-p0->st.pos.z;
	cross3d(&p0->st.rotvel,&rc0,&vang0);
	vrel0.x=p0->st.vel.x+vang0.x;
	vrel0.y=p0->st.vel.y+vang0.y;
	vrel0.z=p0->st.vel.z+vang0.z;
	rc1.x=loc->x-p1->st.pos.x;
	rc1.y=loc->y-p1->st.pos.y;
	rc1.z=loc->z-p1->st.pos.z;
	cross3d(&p1->st.rotvel,&rc1,&vang1);
	vrel1.x=p1->st.vel.x+vang1.x;
	vrel1.y=p1->st.vel.y+vang1.y;
	vrel1.z=p1->st.vel.z+vang1.z;
	vrel.x=vrel1.x-vrel0.x;
	vrel.y=vrel1.y-vrel0.y;
	vrel.z=vrel1.z-vrel0.z;
	top=2.0f*dot3d(norm,&vrel);
	if (top<=0)
		return; // moving away
	if (p0->notrans==0)
		bot+=1.0f/p0->mass;
	if (p1->notrans==0)
		bot+=1.0f/p1->mass;
	if (p0->norot==0) {
		cross3d(&rc0,norm,&rcn0);
		doinvmoi(p0,&p0->st.rot,&rcn0,&rcnr0);
		bot+=dot3d(&rcn0,&rcnr0);
	}
	if (p1->norot==0) {
		cross3d(&rc1,norm,&rcn1);
		doinvmoi(p1,&p1->st.rot,&rcn1,&rcnr1);
		bot+=dot3d(&rcn1,&rcnr1);
	}
	if (bot<EPSILON)
		return;
	k=top/bot;
	k*=.5f+.5f*p0->elast*p1->elast*globalelast;
	force10.x=k*norm->x;
	force10.y=k*norm->y;
	force10.z=k*norm->z;
	if (p0->notrans==0) {
		p0->st.momentum.x+=force10.x;
		p0->st.momentum.y+=force10.y;
		p0->st.momentum.z+=force10.z;
		p0->st.vel.x=p0->st.momentum.x/p0->mass;
		p0->st.vel.y=p0->st.momentum.y/p0->mass;
		p0->st.vel.z=p0->st.momentum.z/p0->mass;
	}
	if (p1->notrans==0) {
		p1->st.momentum.x-=force10.x;
		p1->st.momentum.y-=force10.y;
		p1->st.momentum.z-=force10.z;
		p1->st.vel.x=p1->st.momentum.x/p1->mass;
		p1->st.vel.y=p1->st.momentum.y/p1->mass;
		p1->st.vel.z=p1->st.momentum.z/p1->mass;
	}
	if (p0->norot==0) {
		cross3d(&rc0,&force10,&torque);
		p0->st.angmomentum.x+=torque.x;
		p0->st.angmomentum.y+=torque.y;
		p0->st.angmomentum.z+=torque.z;
		doinvmoi(p0,&p0->st.rot,&p0->st.angmomentum,&p0->st.rotvel);
	}
	if (p1->norot==0) {
		cross3d(&rc1,&force10,&torque);
		p1->st.angmomentum.x-=torque.x;
		p1->st.angmomentum.y-=torque.y;
		p1->st.angmomentum.z-=torque.z;
		doinvmoi(p1,&p1->st.rot,&p1->st.angmomentum,&p1->st.rotvel);
	}
}
*/

#if 1
static int calcimpulseo2o(twcaro *p1,twcaro *p2,
						  pointf3 *loc,pointf3 *norm,
						  pointf3 *impval,pointf3 *r1,pointf3 *r2,float *mag)
{
	pointf3 relvel;
	float k;
//	if (od.c2celast<=0)
//		return 0;
	relvel.x=p1->vel.x-p2->vel.x;
	relvel.y=p1->vel.y-p2->vel.y;
	relvel.z=p1->vel.z-p2->vel.z;
	k=dot3d(norm,&relvel);
	if (k>=0)
		return 0;
	k*=-cv->c2celast;
	impval->x=k*norm->x;
	impval->y=k*norm->y;
	impval->z=k*norm->z;
//	*r1=*r2=zerov;
	r1->x = loc->x - p1->pos.x;
	r1->y = loc->y - p1->pos.y;
	r1->z = loc->z - p1->pos.z;
	r2->x = loc->x - p2->pos.x;
	r2->y = loc->y - p2->pos.y;
	r2->z = loc->z - p2->pos.z;
	*mag=k;
	return 1;
/*	pointf3 relvel;
	pointf3 r1cd,r2cd;
	float n,d,k;
	if (od.c2celast<=0)
		return 0;
	r1->x=loc->x-p1->pos.x;
	r1->y=loc->y-p1->pos.y;
	r1->z=loc->z-p1->pos.z;
	r2->x=loc->x-p2->pos.x;
	r2->y=loc->y-p2->pos.y;
	r2->z=loc->z-p2->pos.z;
	relvel.x=p1->carvel.x-p2->carvel.x;
	relvel.y=p1->carvel.y-p2->carvel.y;
	relvel.z=p1->carvel.z-p2->carvel.z;
	n=dot3d(norm,&relvel);
	cross3d(r1,norm,&r1cd);
	cross3d(r2,norm,&r2cd);
	n+=dot3d(&p1->carvelang,&r1cd);
	n+=dot3d(&p2->carvelang,&r2cd);
	if (n>=0)
		return 0;
	d=1/p1->mass+1/p2->mass;
	d+=dot3d(&r1cd,&r1cd)/p1->moi;
	d+=dot3d(&r2cd,&r2cd)/p2->moi;
	k=n/d;
	k*=-od.c2celast;
	impval->x=k*norm->x;
	impval->y=k*norm->y;
	impval->z=k*norm->z;
	return 1;
*/
}
#endif
#if 1
#define MAXBOX2BOX 72
void checkcar2car(caroj* aj,caroj* bj)
{
	twcaro* a = &aj->cs;
	twcaro* b = &bj->cs;
	calco* atv = &aj->scr;
	calco* btv = &bj->scr;
	pointf3 resv[MAXBOX2BOX],norm,loc,impval;
	int cp;
	pointf3 r1,r2;
	float mag;
	if (cv->c2celast<=0)
		return;
//	int i;//,j,k;
//	struct phyobject *p0=&phyobjects[0],*p1=&phyobjects[1];
/*	if (a->carid==0 && b->carid==1) {
		if (KEY=='i' && ncollinfo+1<MAXCOLLINFO) {
			collinfos[ncollinfo].impval=testimpval;
			collinfos[ncollinfo].imppnt=testimppnt;
			collinfos[ncollinfo].cn=0;
			ncollinfo++;
			collinfos[ncollinfo].impval.x=-testimpval.x;
			collinfos[ncollinfo].impval.y=-testimpval.y;
			collinfos[ncollinfo].impval.z=-testimpval.z;
			collinfos[ncollinfo].imppnt.x=-testimppnt.x;
			collinfos[ncollinfo].imppnt.y=-testimppnt.y;
			collinfos[ncollinfo].imppnt.z=-testimppnt.z;
			collinfos[ncollinfo].cn=1;
			ncollinfo++;
			KEY=0;
		}
		if (KEY=='j' && ncollinfo+1<MAXCOLLINFO) {
			collinfos[ncollinfo].impval=testimpval;
			collinfos[ncollinfo].imppnt=testimppnt;
			collinfos[ncollinfo].cn=1;
			ncollinfo++;
			collinfos[ncollinfo].impval.x=-testimpval.x;
			collinfos[ncollinfo].impval.y=-testimpval.y;
			collinfos[ncollinfo].impval.z=-testimpval.z;
			collinfos[ncollinfo].imppnt.x=-testimppnt.x;
			collinfos[ncollinfo].imppnt.y=-testimppnt.y;
			collinfos[ncollinfo].imppnt.z=-testimppnt.z;
			collinfos[ncollinfo].cn=0;
			ncollinfo++;
			KEY=0;
		}
	} */
//	for (i=ci+1;i<od.ol_numplayers;i++) {

//	for (i=0;i<ncolpairs;i++)
	if (cp=box2box(atv->c2cpnts,btv->c2cpnts,resv))
		if (getcolpoint(resv,cp,&a->pos,&loc,&norm)!=0) {
			if (calcimpulseo2o(a,b,&loc,&norm,&impval,&r1,&r2,&mag) && ncollinfo+1<MAXCOLLINFO) { // generate forces
#define EFFECTS
#ifdef EFFECTS
				if (mag>cv->sndc2cmag && !gg->predicted /* && (a == &gg->caros[gg->viewslot].cs || b == &gg->caros[gg->viewslot].cs) */) {
					if (!gg->intimewarp) {
						sfx->playsnd(&loc,fxe(U32(CRASH1)+mt_random(3)));
/*						pointf3 vel;
						vel.x=(a->vel.x+b->vel.x)*.5f*TIMEINC;
						vel.y=(a->vel.y+b->vel.y)*.5f*TIMEINC;
						vel.z=(a->vel.z+b->vel.z)*.5f*TIMEINC;
						spawnc2ceffect(&loc,&norm,mag,&vel); */
//						gg->con->printf("imp at %d, %f %f %f, mag %f",
//							gg->gs.clock,impval.x,impval.y,impval.z,mag);
					}
				}
#endif
				collinfos[ncollinfo].impval=impval;
				collinfos[ncollinfo].imppnt=r1;
				collinfos[ncollinfo].cn=aj;
				ncollinfo++;
				collinfos[ncollinfo].impval.x=-impval.x;
				collinfos[ncollinfo].impval.y=-impval.y;
				collinfos[ncollinfo].impval.z=-impval.z;
				collinfos[ncollinfo].imppnt=r2;
				collinfos[ncollinfo].cn=bj;
				ncollinfo++;
			}
		}
//	}
//	op->c2chit=0;
/*	for (i=RECTPOINTOFF;i<RECTPOINTOFF+NRECTPOINTS;i++) {
		t.x=op->pos.x+op->pr[i].x;
		t.y=op->pos.y+op->pr[i].y;
		t.z=op->pos.z+op->pr[i].z; */
//		addnull(&t,&op->carang);
//	}
}
#endif
static void applycar2car(caroj *coj)
{
	int i;
	for (i=0;i<ncollinfo;i++) {
		if (collinfos[i].cn==coj) {
//			logger("impulse on %d, loc(%f %f %f), val (%f %f %f)\n",
//				collinfos[i].cn,
//				collinfos[i].imppnt.x,collinfos[i].imppnt.y,collinfos[i].imppnt.z,
//				collinfos[i].impval.x,collinfos[i].impval.y,collinfos[i].impval.z);
			applyforce(-3,&coj->cs,&collinfos[i].impval,&collinfos[i].imppnt);
		}
	}
}

void resetcar2car()
{
	ncollinfo=0;
}
#endif

}
