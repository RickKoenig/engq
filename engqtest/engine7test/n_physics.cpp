/*
#include <engine7cpp.h>
#include "usefulcpp.h"
#include "trackhashcpp.h"
#include "constructorcpp.h"
#include "carenagamecpp.h"
#include "line2roadcpp.h"
#include "physicscpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carclass.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"
#include "n_aicpp.h"
#include "n_scrlinecpp.h"
#include "n_physicscpp.h"
#include "n_rematchcpp.h"
#include "n_carenaconnectcpp.h"
#include "n_line2roadcpp.h"
#include "n_twobjcpp.h"
namespace n_physics {
float movespeed = 40.0f*(1.0f/TICKRATE);//.0625f; // meters/second // 40 m/s is about 89 mph
float turnrate = .25f*(TWOPI/TICKRATE);//.035f; // revolutions/second
void docarphysics(caro *co)
{
	pointf3 up={0,1,0};
	pointf3 ra=pointf3x(0,1,0);
//	float drot=0;
//	setVEC(&ra,0,1,0);
	ra.w=0;
	if (gg.pk.kl && !gg.pk.kd || gg.pk.kr && gg.pk.kd)
		ra.w=-turnrate;
//	c->rot.y-=.035f;
	if (gg.pk.kr && !gg.pk.kd || gg.pk.kl && gg.pk.kd)
		ra.w=turnrate;
//	c->rot.y+=.035f;
//	c->rot.y=snormalangrad(c->rot.y);
	rotaxis2quat(&ra,&ra);
	pointf3 zvec=zerov;
	if (gg.pk.ku)
		zvec.z=movespeed;
	if (gg.pk.kd)
		zvec.z=-movespeed;
	quattimes(&co->rot,&ra,&co->rot);
	quatnormalize(&co->rot,&co->rot);
	quatrot(&co->rot,&zvec,&zvec);
	co->pos.x+=zvec.x;
	co->pos.z+=zvec.z;
	pointf3 top,bot;
	top=bot=co->pos;
	top.y+=1000; // max height
	bot.y-=1000; // min height
	pointf3 norm;
	st2_line2road(&top,&bot,&co->pos,&norm);
	pointf3 q;
	normal2quat(&norm,&up,&q);
}
#if 1
//#include <stdio.h>
//#include <string.h>
//#include <windows.h>
//#include <math.h>

//#include <engine1.h>
//#include <misclib.h>

//#include "stubhelper.h"
//#include "line2road.h"
//#include "pieces.h"
//#include "debprint.h" // for loadconfigfile
//#include "box2box.h"
//#include "online_uphysics.h"
//#include "online_uplay.h"
//#include "crasheditor.h"


#define NSHOCKPOINTS 8
#define NRECTPOINTS 8
#define NMIDPOINTS 2

#define SHOCKPOINTOFF 0
#define RECTPOINTOFF (SHOCKPOINTOFF+NSHOCKPOINTS)
#define MIDPOINTOFF (RECTPOINTOFF+NRECTPOINTS)

#define NCARPOINTS (MIDPOINTOFF+NMIDPOINTS)
#define NWORKINGSHOCKS 4
enum {CARCOAST,CARBRAKE,CARACCEL,CARREVERSEACCEL};
struct calc {
	float extrashocklen[NWORKINGSHOCKS];
	float normalshocklen[NWORKINGSHOCKS];
	float shockextra;
	pointf3 p[NCARPOINTS],pr[NCARPOINTS];
	float shockspacingx,shockspacingz;
	float shockoffsety;
	pointf3 carbboxmin,carbboxmax;
	pointf3 c2cpnts[NRECTPOINTS];
	float shocklen[NWORKINGSHOCKS];
	float wheelvel[NWORKINGSHOCKS];
	float shockstr,shockdamp;
	float moi;
	float mass;
	float elast;
	float airfric,littleg;
	float wheelrad,wheelaccel;
	float accelspin;
	float fricttireslide,fricttireroll,frictcarbody;
	float drivemode;
	float startaccelspin;
	float c2celast;
	float carid;
};
calc acv;
calc *cv=&acv;
// car 2 car
pointf3 testimpval;
pointf3 testimppnt;
//static struct phyobject phyobjects[2];
//static float globalelast;
//					car to car,                          test  ,missiles and such
//#define MAXCOLLINFO 100
struct collinfo collinfos[MAXCOLLINFO];
int ncollinfo;
void addcollinfo(int carid,pointf3 *val,pointf3 *pnt);
// car physics
static void doairfric(caro *co,float af)
{
//	int i;
	pointf3 f;
	float fd;
//	struct ol_object *op;
	return;
	if (cv->mass) {
		fd=((co->vel.x*co->vel.x+co->vel.y*co->vel.y+co->vel.z*co->vel.z)
			*af*TIMEINC)/cv->mass;
		if (fd>EPSILON) {
			f=co->vel;
			if (normalize3d(&f,&f)) {
				f.x*=fd;
				f.y*=fd;
				f.z*=fd;
				co->vel.x-=f.x;
				co->vel.y-=f.y;
				co->vel.z-=f.z;
			}
		}
	}
}

static void dograv(caro *co,float g)
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
void calccollisioninfo(caro *co) // also updates stuff needed for drawing
{
//	JXFORM local2norot;
	mat4 local2norot;
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

	quat2xform(&co->rot,&local2norot);
	xformvecs(&local2norot,cv->p,cv->pr,NCARPOINTS);
	for (i=0;i<8;i++) {
		cv->c2cpnts[i].x=cv->pr[i+RECTPOINTOFF].x+co->pos.x;
		cv->c2cpnts[i].y=cv->pr[i+RECTPOINTOFF].y+co->pos.y;
		cv->c2cpnts[i].z=cv->pr[i+RECTPOINTOFF].z+co->pos.z;
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

static float shockcolliding(caro *co,int shn,pointf3 *topshock,pointf3 *botshock,
							pointf3 *colpoint,pointf3 *contactforce)
{ // impdircontact is unit force from road, ret value is magnitude
	float contact;//,contact2;
	float oldshocklen,shockchange;
	pointf3 cf;
	float den;
	pointf3 perp; //,ts;
	oldshocklen=cv->shocklen[shn];
	cv->shocklen[shn]=cv->extrashocklen[shn];
	// do collision
	if (!st2_line2road(topshock,botshock,colpoint,&perp))
		return 0;
	// shock colliding with road
//	if (oi->ol_lastairtime==0)
//		oi->ol_lastairtime=oi->ol_airtime;
//	logger("shock airtime = 0\n");
//	oi->ol_airtime=0;
	// get compression distance
	contact=dist3d(colpoint,botshock);
	// if compressed to 0 then maxit, maybe try to stop car, bottom out
	if (contact>cv->extrashocklen[shn])
		contact=cv->extrashocklen[shn];
	// get shocklength
	cv->shocklen[shn]=cv->extrashocklen[shn]-contact;
	// shock velocity should use timinc
	shockchange=(cv->shocklen[shn]-oldshocklen);//*TIMEINC/TIMEINCconst;
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

static void fricbox2road(caro *co,float fric,
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

static float collbox2road(caro *co,pointf3 *r1,pointf3 *impdir,pointf3 *impdircontact)
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

static void proj2plane(pointf3 *v,pointf3 *n,pointf3 *p)
{
	float k;
	k=dot3d(v,n)/dot3d(n,n);
	p->x=v->x-k*n->x;
	p->y=v->y-k*n->y;
	p->z=v->z-k*n->z;
}

static void applyforce(caro *co,pointf3 *imp,pointf3 *r1)
{
		pointf3 imptrq;
// add in impulse
		co->vel.x+=imp->x/cv->mass;
		co->vel.y+=imp->y/cv->mass;
		co->vel.z+=imp->z/cv->mass;
// add in imptorque
		cross3d(r1,imp,&imptrq);
		co->rotvel.x+=imptrq.x/cv->moi;
		co->rotvel.y+=imptrq.y/cv->moi;
		co->rotvel.z+=imptrq.z/cv->moi;
}

void checkcar2car(struct playerdata *a,struct playerdata *b);
static void applycar2car(caro *co);
void doroadcollisions(caro *co)
{
// 3d points
//	extern float ol_startaccelspin;
//	extern int carboost;
	float fb;
//	int nocrash;
	pointf3 outerpoint,innerpoint,colpoint;
	pointf3 r1;
	pointf3 r1s[12]; // relative collision points
// 3d impulse directions
	pointf3 impdirs[13];
	pointf3 impdir; // unit vector
	float impmags[13];
// 3d impulses
	pointf3 impulse;//,impfric; //,imptorque;
// motion
//	pointf3 savevel;
//	pointf3 savevelang;
//	pointf3 vel;
	pointf3 dvels[9],dvelangs[9];
	mat4 jx;
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
	applycar2car(co);
//	logger("------------ road collisions -------------------\n"
//	"   before extratrans, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
//#define DOEXTRATRANS
#ifdef DOEXTRATRANS
		vel.x=0;
		vel.y=0;
		vel.z=0;
		if (uup)
			vel.z=+.1f;
		if (udown)
			vel.z-=.1f;
		quat2xform(&oi->carang,&jx);
		xformvec(&jx,&vel,&vel);
		oi->carvel.x+=vel.x;
		oi->carvel.y+=vel.y;
		oi->carvel.z+=vel.z;
#endif
	nanerr("afterxtrans posx",co->pos.x);
	nanerr("afterxtrans posy",co->pos.y);
	nanerr("afterxtrans posz",co->pos.z);
	nanerr("afterxtrans velx",co->vel.x);
	nanerr("afterxtrans vely",co->vel.y);
	nanerr("afterxtrans velz",co->vel.z);
	float validroadnorm=0;
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
//	nocrash=0;
	pointf3 carnorm,cardir;
	carnorm.x=cv->pr[0].x-cv->pr[1].x;
	carnorm.y=cv->pr[0].y-cv->pr[1].y;
	carnorm.z=cv->pr[0].z-cv->pr[1].z;
	cardir.x=cv->pr[0].x-cv->pr[4].x;
	cardir.y=cv->pr[0].y-cv->pr[4].y;
	cardir.z=cv->pr[0].z-cv->pr[4].z;
	pointf3 carvelnorm=co->vel;
//	float carvelmag=normalize3d(&carvelnorm,&carvelnorm);
	normalize3d(&cardir,&cardir);
	normalize3d(&carnorm,&carnorm);
	float shockmag;
	for (j=0;j<NSHOCKPOINTS/2;j++) {
		innerpoint.x=co->pos.x+cv->pr[2*j].x; // top of shock
		innerpoint.y=co->pos.y+cv->pr[2*j].y;
		innerpoint.z=co->pos.z+cv->pr[2*j].z;
		outerpoint.x=co->pos.x+cv->pr[2*j+1].x; // bot of shock
		outerpoint.y=co->pos.y+cv->pr[2*j+1].y;
		outerpoint.z=co->pos.z+cv->pr[2*j+1].z;
		if ((shockmag=shockcolliding(co,j,&innerpoint,&outerpoint,&colpoint,&impdir))>0) { // impdir returned is a unit vector
			r1.x=colpoint.x-co->pos.x;
			r1.y=colpoint.y-co->pos.y;
			r1.z=colpoint.z-co->pos.z;
			impulse.x=shockmag*impdir.x;
			impulse.y=shockmag*impdir.y;
			impulse.z=shockmag*impdir.z;
			applyforce(co,&impulse,&r1);
		}
		impmags[j]=shockmag;
		impdirs[j]=impdir;
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
	shockmag=0;
	pointf3 shocknorm=zerov;
	for (j=0;j<NSHOCKPOINTS/2;j++) {
		if (impmags[j]>0) {
			shockmag+=impmags[j];
			shocknorm.x+=impdirs[j].x;
			shocknorm.y+=impdirs[j].y;
			shocknorm.z+=impdirs[j].z;
		}
	}
	if (shockmag>0) {
		float fts;
		float fr;
		pointf3 bases={1,0,0};
		pointf3 baser={0,0,1};
		pointf3 pr,ps,ir,is,impf,pr2,ps2;
		if (normalize3d(&shocknorm,&shocknorm)==0) {
			for (j=0;j<NSHOCKPOINTS/2;j++)
				logger(" impdirs[%d] = (%f,%f,%f), impmags[%d] = %f\n",j,
					impdirs[j].x,impdirs[j].y,impdirs[j].z,j,impmags[j]);
			logger("roadnorm (%f,%f,%f)\n",shocknorm.x,shocknorm.y,shocknorm.z);
			errorexit("cant norm 1");
		}
		validroadnorm=1;
//		oi->ol_lastroadnorm=oi->shocknorm;
		quat2xform(&co->rot,&jx);
		xformvec(&jx,&bases,&bases);
		proj2plane(&bases,&shocknorm,&bases);
		if (normalize3d(&bases,&bases)==0)
			errorexit("cant norm 2");
		cross3d(&bases,&shocknorm,&baser);
		fr=dot3d(&co->vel,&baser)/cv->wheelrad;
		cv->accelspin=fr;
		if (!proj3d(&co->vel,&bases,&ps))
			errorexit("can't proj3d 1");
		if (!proj3d(&co->vel,&baser,&pr))
			errorexit("can't proj3d 2");
		pr.x*=cv->mass;
		pr.y*=cv->mass;
		pr.z*=cv->mass;
		ps.x*=cv->mass;
		ps.y*=cv->mass;
		ps.z*=cv->mass;
		fts=cv->fricttireslide;//*oi->extratraction;
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
		ir.x=fr*shockmag*baser.x;
		ir.y=fr*shockmag*baser.y;
		ir.z=fr*shockmag*baser.z;
		if (dot3d(&pr,&baser)>0) {
			ir.x=-ir.x;
			ir.y=-ir.y;
			ir.z=-ir.z;
		}
		is.x=fts*shockmag*bases.x;
		is.y=fts*shockmag*bases.y;
		is.z=fts*shockmag*bases.z;
		if (dot3d(&ps,&bases)>0) {
			is.x=-is.x;
			is.y=-is.y;
			is.z=-is.z;
		}
		pr2.x=pr.x+ir.x;
		pr2.y=pr.y+ir.y;
		pr2.z=pr.z+ir.z;
		if (dot3d(&pr,&pr2)<=0) {
			ir.x=-pr.x;
			ir.y=-pr.y;
			ir.z=-pr.z;
		}
		ps2.x=ps.x+is.x;
		ps2.y=ps.y+is.y;
		ps2.z=ps.z+is.z;
		if (dot3d(&ps,&ps2)<=0) {
			is.x=-ps.x;
			is.y=-ps.y;
			is.z=-ps.z;
		}
		if (cv->drivemode==CARACCEL /*|| oi->extrapouncer*/) {
			float wa,sas,wae;
//			wa=wheelaccel;
			sas=cv->startaccelspin;//*oi->extratopspeed;
			wae=cv->wheelaccel;//*oi->extraaccel;
			if (cv->accelspin<sas*.5f)
				wa=wae;
			else if (cv->accelspin<sas*.75f)
				wa=wae*.5f;
			else if (cv->accelspin<sas*.9f)
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
		impf.x=ir.x+is.x;
		impf.y=ir.y+is.y;
		impf.z=ir.z+is.z;
		applyforce(co,&impf,&zerov);
		fr=dot3d(&co->vel,&baser)/cv->wheelrad;
		for (j=0;j<NSHOCKPOINTS/2;j++) // calc wheel vels
			cv->wheelvel[j]=fr;
	} else {
		cv->wheelvel[j]=0; // warning, array subscript is above array bounds
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
		outerpoint.x=co->pos.x+cv->pr[RECTPOINTOFF+j].x;
		outerpoint.y=co->pos.y+cv->pr[RECTPOINTOFF+j].y;
		outerpoint.z=co->pos.z+cv->pr[RECTPOINTOFF+j].z;
		int k=j&1;
		innerpoint.x=co->pos.x+cv->pr[MIDPOINTOFF+k].x;
		innerpoint.y=co->pos.y+cv->pr[MIDPOINTOFF+k].y;
		innerpoint.z=co->pos.z+cv->pr[MIDPOINTOFF+k].z;
		if (st2_line2road(&innerpoint,&outerpoint,&colpoint,&impdir)) {
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

				pointf3 impfric;
				fricbox2road(co,fb,&r1s[j],&impdirs[j],carbodymag,&impfric);
//				logger("impfric  = %9.3f,%9.3f,%9.3f\n",impfric.x,impfric.y,impfric.z);
				impulse.x+=impfric.x;
				impulse.y+=impfric.y;
				impulse.z+=impfric.z;

// add in impulse
//				logger("i %d, j %d, k %d, ncols %d, applyforce %9.3f %9.3f %9.3f at %9.3f %9.3f %9.3f\n",
//					i,j,k,ncols,impulse.x,impulse.y,impulse.z,r1s[j].x,r1s[j].y,r1s[j].z);
				applyforce(co,&impulse,&r1s[j]);
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
void updaterots(caro *co)
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
			q2=zerov;
			uextraheading=defuextraheading;
//			if(urollright && !uready && (uleft || uright))
//				uextraheading +=1.5;
//			else
			if (accelspin>30)
				ramp = 1;
			else if (accelspin<-30)
				ramp = 1;
			else
				ramp = (float)fabs(accelspin/30);
//			if(uextraheading>maxuextraheading)
//				uextraheading = maxuextraheading;
			quatinverse(&op->carang,&q);
			quat2xform(&q,&jx);
			ramp*=maxuextraheading;
			xformvec(&jx,&objects[0].carvelang,&q); // q is car relative rotaxis velocity
			if (uright && accelspin>0 || uleft && accelspin<=0)
				q2.y+=uextraheading;
			if (uleft && accelspin>0 || uright && accelspin<=0)
				q2.y-=uextraheading;
			if (!uright && !uleft) { // straighten out
				if (q.y>0)
					q2.y=-uextraheading;
				else if (q.y<0)
					q2.y+=uextraheading;
			}
			q3.x=q.x;
			q3.y=q2.y+q.y;
			q3.z=q.z;
			if (q3.y>ramp)
				q2.y=0;
			if (q3.y<-ramp)
				q2.y=0;
			if (q3.y*q.y<=0)
				q2.y=0;
			quat2xform(&op->carang,&jx);
			xformvec(&jx,&q2,&q2);
			objects[0].carvelang.x+=q2.x;
			objects[0].carvelang.y+=q2.y;
			objects[0].carvelang.z+=q2.z;
#endif
//#define DOEXTRAHEADING2
#ifdef DOEXTRAHEADING2
			q.x=0;
			q.y=1;
			q.z=0;
// gradual steering
			if (op->pi.ol_uright || op->pi.ol_uleft)
				op->ol_steertime++;
			else
				op->ol_steertime=2;
			if (op->ol_steertime>8)
				op->ol_steertime=8;
			if (op->pi.ol_uleft || op->pi.ol_uright)
				op->ol_uextraheading=op->ol_defuextraheading*op->ol_steertime/8;
			else
				op->ol_uextraheading=op->ol_defuextraheading; // don't gradual when trying to go straight
// end gradual steering
			if (op->ol_accelspin>30)
				ramp = 1;
			else if (op->ol_accelspin<-30)
				ramp = 1;
			else
				ramp = (float)fabs(op->ol_accelspin/30);
			ramp*=op->ol_maxuextraheading;
			quat2xform(&op->carang,&jx);
			xformvec(&jx,&q,&q); // q is in world, direction of y axis of rotvel car in rotaxis
			dt=dot3d(&q,&op->carvelang);
			if (r && op->ol_accelspin>=0 || l && op->ol_accelspin<0)
				delhead+=op->ol_uextraheading;
			if (l && op->ol_accelspin>=0 || r && op->ol_accelspin<0)
				delhead-=op->ol_uextraheading;
			if (!op->pi.ol_uright && !op->pi.ol_uleft) { // straighten out
				if (dt>0)
					delhead-=op->ol_uextraheading;
				else if (dt<0)
					delhead+=op->ol_uextraheading;
			}
			newvel.x=op->carvelang.x+delhead*q.x;
			newvel.y=op->carvelang.y+delhead*q.y;
			newvel.z=op->carvelang.z+delhead*q.z;
			newvel.w=0;
			dt2=dot3d(&q,&newvel);
			if (dt2>ramp) { // move from >ramp to ramp
				newvel.x+=(ramp-dt2)*q.x;
				newvel.y+=(ramp-dt2)*q.y;
				newvel.z+=(ramp-dt2)*q.z;
			} else if (dt2<-ramp) { // move from <-ramp to -ramp
				newvel.x+=(-dt2-ramp)*q.x;
				newvel.y+=(-dt2-ramp)*q.y;
				newvel.z+=(-dt2-ramp)*q.z;
			} else if (dt*dt2<0 && !op->pi.ol_uright && !op->pi.ol_uleft) { // move from dot2 to 0
				newvel.x-=dt2*q.x;
				newvel.y-=dt2*q.y;
				newvel.z-=dt2*q.z;
			}
			op->carvelang=newvel;
#endif
			q=co->rotvel;
			t=normalize3d(&q,&q);
			if (t) {
//				t*=PIUNDER180;
				t*=TIMEINC;
				q.w=t;
/*				if (op->venableicrms) {
					q.w=0;
//					q.w*=op->vicrmsparm;
//					if (op->vicrmsparm==0)
//						op->carvelang=zerov;
				} else if (op->vinoil && op->ol_accelspin>od.oilspinspeed) {
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

void updatetrans(caro *co)
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
	co->pos.x+=vel.x*TIMEINC;
	co->pos.y+=vel.y*TIMEINC;
	co->pos.z+=vel.z*TIMEINC;
//		if (op->pi.uspace)
//			logger("car reset: uspace\n");
#if 0
	//	if ((co->pos.y<od.lowpoint || spc ) /*&& !op->ol_lastkludge */ ||
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
			op->carvelang=zerov;
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

#if 0
static int calcimpulseo2o(caro *p1,caro *p2,
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
	*r1=*r2=zerov;
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
#if 0
#define MAXBOX2BOX 72
void checkcar2car(struct ol_playerdata *a,struct ol_playerdata *b)
{
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
	if (cp=box2box(a->c2cpnts,b->c2cpnts,resv))
		if (getcolpoint(resv,cp,&a->pos,&loc,&norm)!=0) {
			if (calcimpulseo2o(a,b,&loc,&norm,&impval,&r1,&r2,&mag) && ncollinfo+1<MAXCOLLINFO) { // generate forces
				if (mag>.5) {
					pointf3 vel;
					ol_playatagsound(18+random(3),a->vold*mag*.5f,a->pan);
					vel.x=(a->carvel.x+b->carvel.x)*.5f*TIMEINC;
					vel.y=(a->carvel.y+b->carvel.y)*.5f*TIMEINC;
					vel.z=(a->carvel.z+b->carvel.z)*.5f*TIMEINC;
					spawnc2ceffect(&loc,&norm,mag,&vel);
				}
//				logger("impulse at %d, %f %f %f, mag %f\n",a->clocktickcount,impval.x,impval.y,impval.z,mag);
				collinfos[ncollinfo].impval=impval;
				collinfos[ncollinfo].imppnt=r1;
				collinfos[ncollinfo].cn=a->carid;
				ncollinfo++;
				collinfos[ncollinfo].impval.x=-impval.x;
				collinfos[ncollinfo].impval.y=-impval.y;
				collinfos[ncollinfo].impval.z=-impval.z;
				collinfos[ncollinfo].imppnt=r2;
				collinfos[ncollinfo].cn=b->carid;
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
static void applycar2car(caro *co)
{
	int i;
	for (i=0;i<ncollinfo;i++) {
		if (collinfos[i].cn==cv->carid) {
//			logger("impulse on %d, loc(%f %f %f), val (%f %f %f)\n",
//				collinfos[i].cn,
//				collinfos[i].imppnt.x,collinfos[i].imppnt.y,collinfos[i].imppnt.z,
//				collinfos[i].impval.x,collinfos[i].impval.y,collinfos[i].impval.z);
			applyforce(co,&collinfos[i].impval,&collinfos[i].imppnt);
		}
	}
}

void resetcar2car()
{
	ncollinfo=0;
}
#endif

}
