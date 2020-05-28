#include <stdio.h>
#include <string.h>
//#include <windows.h>
#include <math.h>

#include <m_eng.h>
#include "st2_glue.h"


//#include <engine1.h>
//#include <d3dtypes.h>

//#include <commoninc/keys.h>

//#include "tensor.h"
//#include "rick.h"

//#include "macros.h"
//#include "../3dlib/mesh.h"
//#include "data.h"
//#include "misc.h"
//#include "3d.h"
//#include "memalloc.h"

//#include "camera.h"
//#include "motion.h"
//#include "tree.h"
//#include "debprint.h"
//#include "lwsread.h"
#include "st2_roadheight.h"
//#include <winput/winput.h>
//#include <3dlib/engine.h>
//#include "pieces.h"

//#include "states.h"

//#include "soundlst.h"
//#include "miscdef.h"
//#include "tspread.h"
//#include <rvid/rvid.pro>
//#include <ros/ros.h>
//#include "newtex.h"
//#include "perf.h"

//#include "uplay3.h"
#include "st2_uphysics2.h"
#include "st2_uplay3.h"
#include "st2_uplay3_internal.h"

namespace st2 {

	extern int utotalcrashes, uleft, uright;

	//void playasound(int,int);
	//float doroadheight2(int bodyid,VEC *pos,VEC *maxnorm, float lrh);
	//VEC getlocalvec(TREE *t,VEC locv);
	//VEC getworldvec(TREE *t,VEC locv);
	//int st2_line2road(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm);
	//int line2piece(int bodyid,VEC *top,VEC *bot,VEC *intsect,VEC *norm);

	struct object objects[NUMOBJECTS];
	// objects
	struct object iobjects[NUMOBJECTS] = { // init object
	//           x,   y,  z, pad, lenx, leny,  lenz, pad, mass, velx, vely, velz,
		{      -38, .2f,-30,   0,  .7f, .55f, 1.47f,   0,    1,   0,    0,    0},
		//           x,   y,  z, pad, lenx, leny,  lenz, pad, mass, velx, vely, velz,
	};

	float normalshocklen[6];
	float shockspacingx = 4.5f;
	float shockspacingz = 4.5f;
	float shockoffsety;
	float wheelrad = 1.25f, wheelwid, wheellen;
	float caroffsetx, caroffsety, caroffsetz;
	int magnatraction;
	float debmass, debmoi;
	extern int airtime;
	float shockstr, shockdamp;
	float timeinc;
	float st2_timeincconst = 1 / 30.0f;
	int powershocks[6] = { 0,0,1,1,0,0 };
	int steershocks[6] = { 1,1,0,0,0,0 };

	int drivemode;
	float accelspin;
	//float timeinc;
	float elast = .8f;
	//int justshocks;
	float fricttireroll, fricttireslide, frictcarbody, frictcarbodyroof;
	//float shockstr;
	//float shockdamp;

	extern int selectedcar;
	VEC carstuntrpy;

	float littleg, airfric = .001f;
	/////////// new physics model quaternion based ............................. //////////
	///////////// extra forces
	void doairfric(float af)
	{
		int i;
		VEC f;
		float fd;
		struct object *op;
		return;
		for (op = objects, i = 0; i < NUMOBJECTS; i++, op++)
			if (op->mass) {
				fd = ((op->carvel.x*op->carvel.x + op->carvel.y*op->carvel.y + op->carvel.z*op->carvel.z)
					*af*timeinc) / op->mass;
				if (fd > EPSILON) {
					f = op->carvel;
					if (normalize3d(&f, &f)) {
						f.x *= fd;
						f.y *= fd;
						f.z *= fd;
						op->carvel.x -= f.x;
						op->carvel.y -= f.y;
						op->carvel.z -= f.z;
					}
				}
			}
	}

	void dograv(float g)
	{
		int i;
		JXFORM x;
		VEC gdir;
		struct object *op;
		for (op = objects, i = 0; i < NUMOBJECTS; i++, op++) {
			if (op->mass) {
				if (magnatraction) {
					quat2xform(&op->carang, &x);
					gdir.x = 0;
					gdir.y = 1;
					gdir.z = 0;
					xformvec(&x, &gdir, &gdir);
					op->carvel.x -= g * timeinc*gdir.x;
					op->carvel.y -= g * timeinc*gdir.y;
					op->carvel.z -= g * timeinc*gdir.z;
				} else {
					op->carvel.y -= g * timeinc;
				}
			}
		}
	}

	/////////////////////////// collisions
	void calccollisioninfo() // also updates stuff needed for drawing
	{
		JXFORM local2norot;
		int i;
		struct object *op;
		for (op = objects, i = 0; i < NUMOBJECTS; i++, op++) {
			// shocks
			op->p[0].x = -shockspacingx / 2;
			op->p[0].y = shockoffsety;
			op->p[0].z = shockspacingz / 2;
			op->p[1].x = -shockspacingx / 2;
			op->p[1].y = shockoffsety - normalshocklen[0];
			op->p[1].z = shockspacingz / 2;
			op->p[2].x = shockspacingx / 2;
			op->p[2].y = shockoffsety;
			op->p[2].z = shockspacingz / 2;
			op->p[3].x = shockspacingx / 2;
			op->p[3].y = shockoffsety - normalshocklen[1];
			op->p[3].z = shockspacingz / 2;
			op->p[4].x = -shockspacingx / 2;
			op->p[4].y = shockoffsety;
			op->p[4].z = -shockspacingz / 2;
			op->p[5].x = -shockspacingx / 2;
			op->p[5].y = shockoffsety - normalshocklen[2];
			op->p[5].z = -shockspacingz / 2;
			op->p[6].x = shockspacingx / 2;
			op->p[6].y = shockoffsety;
			op->p[6].z = -shockspacingz / 2;
			op->p[7].x = shockspacingx / 2;
			op->p[7].y = shockoffsety - normalshocklen[3];
			op->p[7].z = -shockspacingz / 2;
			// 8 more points for object to road collisions
			op->p[8].x = carbboxmin.x;
			op->p[8].y = carbboxmin.y;
			op->p[8].z = carbboxmin.z;
			op->p[9].x = carbboxmin.x;
			op->p[9].y = carbboxmin.y;
			op->p[9].z = carbboxmax.z;
			op->p[10].x = carbboxmin.x;
			op->p[10].y = carbboxmax.y;
			op->p[10].z = carbboxmin.z;
			op->p[11].x = carbboxmin.x;
			op->p[11].y = carbboxmax.y;
			op->p[11].z = carbboxmax.z;
			op->p[12].x = carbboxmax.x;
			op->p[12].y = carbboxmin.y;
			op->p[12].z = carbboxmin.z;
			op->p[13].x = carbboxmax.x;
			op->p[13].y = carbboxmin.y;
			op->p[13].z = carbboxmax.z;
			op->p[14].x = carbboxmax.x;
			op->p[14].y = carbboxmax.y;
			op->p[14].z = carbboxmin.z;
			op->p[15].x = carbboxmax.x;
			op->p[15].y = carbboxmax.y;
			op->p[15].z = carbboxmax.z;
			// 2 midpoints, assumes cars length > width > height
			op->p[16].x = (carbboxmax.x + carbboxmin.x) / 2;
			op->p[16].y = (carbboxmax.y + carbboxmin.y) / 2;
			op->p[16].z = carbboxmin.z + (carbboxmax.x - op->p[16].x);
			op->p[17].x = (carbboxmax.x + carbboxmin.x) / 2;
			op->p[17].y = (carbboxmax.y + carbboxmin.y) / 2;
			op->p[17].z = carbboxmax.z - (carbboxmax.x - op->p[16].x);

			quat2xform(&objects[0].carang, &local2norot);
			xformvecs(&local2norot, op->p, op->pr, NCARPOINTS);
			op->moi = op->mass*(op->len.x*op->len.x / 12.0f + op->len.y*op->len.y / 3.0f);
			if (debmoi)
				op->moi = debmoi;
			else
				debmoi = op->moi;
			if (debmass)
				op->mass = debmass;
			else
				debmass = op->mass;
		}
	}

	int lastairtime;
	static float shockcolliding(struct object *oi, int shn, VEC *topshock, VEC *botshock,
		VEC *colpoint, VEC *contactforce)
	{ // impdircontact is unit force from road, ret value is magnitude
		float contact;//,contact2;
		float oldshocklen, shockchange;
		VEC cf;
		float den;
		VEC perp; //,ts;
		oldshocklen = oi->shocklen[shn];
		oi->shocklen[shn] = normalshocklen[shn];
		// do collision
		if (!st2_line2road(topshock, botshock, colpoint, &perp))
			return 0;
		// shock colliding with road
		if (lastairtime == 0)
			lastairtime = airtime;
		//	ricklogger("shock airtime = 0\n");
		airtime = 0;
		// get compression distance
		contact = dist3d(colpoint, botshock);
		// if compressed to 0 then maxit, maybe try to stop car, bottom out
		if (contact > normalshocklen[shn])
			contact = normalshocklen[shn];
		// get shocklength
		oi->shocklen[shn] = normalshocklen[shn] - contact;
		// shock velocity should use timinc
		shockchange = (oi->shocklen[shn] - oldshocklen)*timeinc / timeincconst;
		// calc force
		contact = contact * shockstr - shockdamp * shockchange;
		if (contact < 0)
			return 0; // road is moving faster away then the wheels, wheels not touching ground
		// calc unit dir of contact force
		cf.x = topshock->x - botshock->x;
		cf.y = topshock->y - botshock->y;
		cf.z = topshock->z - botshock->z;
		if (normalize3d(&cf, &cf) == 0)
			return 0;
		// put in the magnitude of contact force
		// project it to normal, should be amplified, not diminished FIX
	//	if (proj3d(&cf,&perp,contactforce)==0)
	//		return 0;
		den = dot3d(&cf, &perp);
		//	den=1/den;
		//	if (den>5)
		//		den=5;
		//	den=1;
			// convert force to mag/dir
		*contactforce = perp;
		return contact * den*timeinc;
		//	return contact2*contact*TINC;
	}

	static void fricbox2road(float fric, struct object *oi,
		VEC *r1, VEC *impdir, float contact, VEC *impdirfric)
	{
		VEC va, vb;
		float impfric;
		float dt;
		VEC crs;
		VEC vtrans;
		VEC vrot;
		float t;
		if (fric == 0) {
			impdirfric->x = 0;
			impdirfric->y = 0;
			impdirfric->z = 0;
			return;
		}
		// get velocity of point on box
		cross3d(&oi->carvelang, r1, &vrot); // vrot
		vtrans = oi->carvel;	// vtrans
	// now project velocity of point onto ground
		va.x = vrot.x + vtrans.x;
		va.y = vrot.y + vtrans.y;
		va.z = vrot.z + vtrans.z;
		t = dot3d(&va, impdir);
		va.x -= t * impdir->x;
		va.y -= t * impdir->y;
		va.z -= t * impdir->z;
		// get direction of friction force
		impdirfric->x = -va.x;
		impdirfric->y = -va.y;
		impdirfric->z = -va.z;
		if (!normalize3d(impdirfric, impdirfric))
			return;
		cross3d(r1, impdirfric, &crs);
		vb.x = impdirfric->x / oi->mass + crs.x / oi->moi;
		vb.y = impdirfric->y / oi->mass + crs.y / oi->moi;
		vb.z = impdirfric->z / oi->mass + crs.z / oi->moi;
		dt = dot3d(&vb, impdirfric);
		if (dt) {
			if (contact < 0)
				error("hey contact less than 0");
			impfric = -dot3d(&va, impdirfric) / dt;
			impdirfric->x *= impfric * fric;
			impdirfric->y *= impfric * fric;
			impdirfric->z *= impfric * fric;
		} else {
			impdirfric->x = 0;
			impdirfric->y = 0;
			impdirfric->z = 0;
		}
	}

	int cbairtime;
	static float collbox2road(struct object *oi, VEC *r1, VEC *impdir, VEC *impdircontact)
	{
		float impmag;
		VEC r1cd;
		VEC rotvel, veldiff;
		// first check to see if point moving into road
		cross3d(&oi->carvelang, r1, &rotvel);
		// velocity of point1 rel to road
		veldiff.x = oi->carvel.x + rotvel.x;
		veldiff.y = oi->carvel.y + rotvel.y;
		veldiff.z = oi->carvel.z + rotvel.z;
		if (dot3d(impdir, &veldiff) >= 0) // return if point moving away from road
			return 0.0f;
		// do physics formula
		cross3d(r1, impdir, &r1cd);
		impmag = -2 * (dot3d(&oi->carvel, impdir) + dot3d(&oi->carvelang, &r1cd)) /
			(1 / oi->mass + dot3d(&r1cd, &r1cd) / oi->moi);
		impmag *= elast;
		// done physics formula
		if (impmag < 0)
			return 0.0f;
		//	impmag+=.1f;
		impdircontact->x = impdir->x*impmag;
		impdircontact->y = impdir->y*impmag;
		impdircontact->z = impdir->z*impmag;
		if (lastairtime == 0)
			lastairtime = airtime;
		airtime = 0;
		cbairtime = 0;
		return impmag;
	}

	void proj2plane(VEC *v, VEC *n, VEC *p)
	{
		float k;
		k = dot3d(v, n) / dot3d(n, n);
		p->x = v->x - k * n->x;
		p->y = v->y - k * n->y;
		p->z = v->z - k * n->z;
	}

	void applyforce(struct object *oi, VEC *imp, VEC *r1)
	{
		VEC imptrq;
		// add in impulse
		oi->carvel.x += imp->x / oi->mass;
		oi->carvel.y += imp->y / oi->mass;
		oi->carvel.z += imp->z / oi->mass;
		// add in imptorque
		cross3d(r1, imp, &imptrq);
		oi->carvelang.x += imptrq.x / oi->moi;
		oi->carvelang.y += imptrq.y / oi->moi;
		oi->carvelang.z += imptrq.z / oi->moi;
	}

	// using cycling and avg technique
	static VEC shocknorm;
	static float shockmag;
	static VEC carbodynorm;
	static float carbodymag;
	static int hifrictime;
	VEC carnorm, cardir;
	float carvelmag;
	VEC carvelnorm;
	VEC lastroadnorm;
	int validroadnorm;
	int ufliptime;
	extern int doacrashreset;
	extern int dofinish;
	VEC ulastloc;

	void doroadcollisions()
	{
		// 3d points
		extern float startaccelspin;
		extern int carboost;
		float fb;
		//	int nocrash;
		VEC outerpoint, innerpoint, colpoint;
		VEC r1;
		VEC r1s[12]; // relative collision points
	// 3d impulse directions
		VEC impdirs[13];
		VEC impdir; // unit vector
		float impmags[13];
		// 3d impulses
		VEC impulse, impfric; //,imptorque;
	// motion
		VEC savevel;
		VEC savevelang;
		//	VEC vel;
		VEC dvels[9], dvelangs[9];
		JXFORM jx;
		// iteraters
		int i, j, k;
		int ncols;
		// objects
		struct object *oi = &objects[0];
		// driving
		//	VEC norm;
		//////////////////////// extra forces
		doairfric(airfric);
		dograv(littleg);
		//	ricklogger("------------ road collisions -------------------\n"
		//	"   before extratrans, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
		//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
		//#define DOEXTRATRANS
#ifdef DOEXTRATRANS
		vel.x = 0;
		vel.y = 0;
		vel.z = 0;
		if (uup)
			vel.z = +.1f;
		if (udown)
			vel.z -= .1f;
		quat2xform(&oi->carang, &jx);
		xformvec(&jx, &vel, &vel);
		oi->carvel.x += vel.x;
		oi->carvel.y += vel.y;
		oi->carvel.z += vel.z;
#endif
		//	nanerr("afterxtrans posx",oi->pos.x);
		//	nanerr("afterxtrans posy",oi->pos.y);
		//	nanerr("afterxtrans posz",oi->pos.z);
		//	nanerr("afterxtrans velx",oi->carvel.x);
		//	nanerr("afterxtrans vely",oi->carvel.y);
		//	nanerr("afterxtrans velz",oi->carvel.z);
		validroadnorm = 0;
		//	ricklogger(" before doshock2road, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
		//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
		if (carboost > 0) {
			VEC topvel;//,unitvel;
			float tween, mag;
			mag = oi->carvel.x*oi->carvel.x + oi->carvel.z*oi->carvel.z;
			if (mag > EPSILON) {
				mag = (float)sqrt(mag);
				topvel.x = oi->carvel.x / mag * startaccelspin*wheelrad;
				//			topvel.y=unitvel.y*startaccelspin*wheelrad;
				topvel.z = oi->carvel.z / mag * startaccelspin*wheelrad;
				switch (carboost) {
				case 1:	tween = .5f;
					break;
				case 2: tween = .75f;
					break;
				case 3:	tween = .8f;
					break;
				case 4:	tween = .9f;
					break;
				default:tween = 1;
					break;
				}
				oi->carvel.x = oi->carvel.x + (topvel.x - oi->carvel.x)*tween;
				//			oi->carvel.y=oi->carvel.y+(topvel.y-oi->carvel.y)*tween;
				oi->carvel.z = oi->carvel.z + (topvel.z - oi->carvel.z)*tween;
			}
			carboost = 0;
		}
#define DOSHOCKS2ROAD
#ifdef DOSHOCKS2ROAD
		//	nocrash=0;
		carnorm.x = oi->pr[0].x - oi->pr[1].x;
		carnorm.y = oi->pr[0].y - oi->pr[1].y;
		carnorm.z = oi->pr[0].z - oi->pr[1].z;
		cardir.x = oi->pr[0].x - oi->pr[4].x;
		cardir.y = oi->pr[0].y - oi->pr[4].y;
		cardir.z = oi->pr[0].z - oi->pr[4].z;
		carvelnorm = oi->carvel;
		carvelmag = normalize3d(&carvelnorm, &carvelnorm);
		normalize3d(&cardir, &cardir);
		normalize3d(&carnorm, &carnorm);
		for (j = 0; j < NSHOCKPOINTS / 2; j++) {
			innerpoint.x = oi->pos.x + oi->pr[2 * j].x; // top of shock
			innerpoint.y = oi->pos.y + oi->pr[2 * j].y;
			innerpoint.z = oi->pos.z + oi->pr[2 * j].z;
			outerpoint.x = oi->pos.x + oi->pr[2 * j + 1].x; // bot of shock
			outerpoint.y = oi->pos.y + oi->pr[2 * j + 1].y;
			outerpoint.z = oi->pos.z + oi->pr[2 * j + 1].z;
			if ((shockmag = shockcolliding(oi, j, &innerpoint, &outerpoint, &colpoint, &impdir)) > 0) { // impdir returned is a unit vector
	//			iscrash=islightcrash=0;
	//			nocrash=1;
				r1.x = colpoint.x - oi->pos.x;
				r1.y = colpoint.y - oi->pos.y;
				r1.z = colpoint.z - oi->pos.z;
				impulse.x = shockmag * impdir.x;
				impulse.y = shockmag * impdir.y;
				impulse.z = shockmag * impdir.z;
				applyforce(oi, &impulse, &r1);
			}
			impmags[j] = shockmag;
			impdirs[j] = impdir;
		}
#endif
		// friction, driving, calculate velocity of wheels on ground
		//	nanerr("aftershock posx",oi->pos.x);
		//	nanerr("aftershock posy",oi->pos.y);
		//	nanerr("aftershock posz",oi->pos.z);
		//	nanerr("aftershock velx",oi->carvel.x);
		//	nanerr("aftershock vely",oi->carvel.y);
		//	nanerr("aftershock velz",oi->carvel.z);
		//	ricklogger("before shockfriction, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
		//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
#define DOSHOCKFRICTION
#ifdef DOSHOCKFRICTION
		shockmag = 0;
		shocknorm = zerov;
		for (j = 0; j < NSHOCKPOINTS / 2; j++) {
			if (impmags[j] > 0) {
				shockmag += impmags[j];
				shocknorm.x += impdirs[j].x;
				shocknorm.y += impdirs[j].y;
				shocknorm.z += impdirs[j].z;
			}
		}
		if (shockmag > 0) {
			float fr;
			VEC bases = { 1,0,0 };
			VEC baser = { 0,0,1 };
			VEC pr, ps, ir, is, impf, pr2, ps2;
			if (normalize3d(&shocknorm, &shocknorm) == 0) {
				for (j = 0; j < NSHOCKPOINTS / 2; j++)
					logger(" impdirs[%d] = (%f,%f,%f), impmags[%d] = %f\n", j,
						impdirs[j].x, impdirs[j].y, impdirs[j].z, j, impmags[j]);
				logger("roadnorm (%f,%f,%f)\n", shocknorm.x, shocknorm.y, shocknorm.z);
				errorexit("cant norm 1");
			}
			validroadnorm = 1;
			st2_callsurffunc();
			lastroadnorm = shocknorm;
			quat2xform(&oi->carang, &jx);
			xformvec(&jx, &bases, &bases);
			proj2plane(&bases, &shocknorm, &bases);
			if (normalize3d(&bases, &bases) == 0)
				errorexit("cant norm 2");
			cross3d(&bases, &shocknorm, &baser);
			fr = dot3d(&oi->carvel, &baser) / wheelrad;
			accelspin = fr;
			if (!proj3d(&oi->carvel, &bases, &ps))
				errorexit("can't proj3d 1");
			if (!proj3d(&oi->carvel, &baser, &pr))
				errorexit("can't proj3d 2");
			pr.x *= oi->mass;
			pr.y *= oi->mass;
			pr.z *= oi->mass;
			ps.x *= oi->mass;
			ps.y *= oi->mass;
			ps.z *= oi->mass;
			if (drivemode == CARBRAKE)
				fr = fricttireslide;
			else if (drivemode != CARACCEL)
				//		else
				fr = fricttireroll;
			else
				fr = 0;
			ir.x = fr * shockmag*baser.x;
			ir.y = fr * shockmag*baser.y;
			ir.z = fr * shockmag*baser.z;
			if (dot3d(&pr, &baser) > 0) {
				ir.x = -ir.x;
				ir.y = -ir.y;
				ir.z = -ir.z;
			}
			is.x = fricttireslide * shockmag*bases.x;
			is.y = fricttireslide * shockmag*bases.y;
			is.z = fricttireslide * shockmag*bases.z;
			if (dot3d(&ps, &bases) > 0) {
				is.x = -is.x;
				is.y = -is.y;
				is.z = -is.z;
			}
			pr2.x = pr.x + ir.x;
			pr2.y = pr.y + ir.y;
			pr2.z = pr.z + ir.z;
			if (dot3d(&pr, &pr2) <= 0) {
				ir.x = -pr.x;
				ir.y = -pr.y;
				ir.z = -pr.z;
			}
			ps2.x = ps.x + is.x;
			ps2.y = ps.y + is.y;
			ps2.z = ps.z + is.z;
			if (dot3d(&ps, &ps2) <= 0) {
				is.x = -ps.x;
				is.y = -ps.y;
				is.z = -ps.z;
			}
			if (drivemode == CARACCEL) {
				float wa;
				//			wa=wheelaccel;
				if (accelspin < startaccelspin*.5f)
					wa = wheelaccel;
				else if (accelspin < startaccelspin*.75f)
					wa = wheelaccel / 2;
				else if (accelspin < startaccelspin*.9f)
					wa = wheelaccel / 10;
				else
					wa = wheelaccel / 100;
				//			wa=wheelaccel*(1.0f-(float)sqrt(fabs(accelspin/startaccelspin)));
				ir.x += wa * baser.x*shockmag;
				ir.y += wa * baser.y*shockmag;
				ir.z += wa * baser.z*shockmag;
			} else if (drivemode == CARREVERSEACCEL) {
				ir.x -= wheelaccel * baser.x*shockmag;
				ir.y -= wheelaccel * baser.y*shockmag;
				ir.z -= wheelaccel * baser.z*shockmag;
			}
			impf.x = ir.x + is.x;
			impf.y = ir.y + is.y;
			impf.z = ir.z + is.z;
			applyforce(oi, &impf, &zerov);
			fr = dot3d(&oi->carvel, &baser) / wheelrad;
			for (j = 0; j < NSHOCKPOINTS / 2; j++) // calc wheel vels
				oi->wheelvel[j] = fr;
		}
		else {
			oi->wheelvel[j] = 0;
		}
#endif
		//	ricklogger("      before flipcar, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
		//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
		//#define FLIPCAR
#ifdef FLIPCAR
		{
			VEC r1 = { 0,1,0 };
			VEC frc = { .1f,0,0 };
			if (!flymode && validroadnorm && dot3d(&lastroadnorm, &carnorm) < 0) {
				applyforce(oi, &frc, &r1);
				r1.y = -1;
				frc.x = -.1f;
				applyforce(oi, &frc, &r1);
			}
		}
#endif
		//////////////////////// carbody to road, do this one LAST! //////////
		//	nanerr("afterfric posx",oi->pos.x);
		//	nanerr("afterfric posy",oi->pos.y);
		//	nanerr("afterfric posz",oi->pos.z);
		//	nanerr("afterfric velx",oi->carvel.x);
		//	nanerr("afterfric vely",oi->carvel.y);
		//	nanerr("afterfric velz",oi->carvel.z);
		//	ricklogger(" before carbody2road, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
		//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
#define DOCARBODY2ROAD
#ifdef DOCARBODY2ROAD
		if (shockmag <= 0) {
			hifrictime++;
			if (hifrictime > 20) {
				hifrictime = 20;
				fb = frictcarbodyroof; // hi friction when no shocks and upsidedown
			} else
				fb = frictcarbody; // lo friction , oops forgot this...
		} else {
			hifrictime = 0;
			fb = frictcarbody; // lo friction
		}
		savevel = oi->carvel;
		savevelang = oi->carvelang;
		// collect upto 8 collision points from the car's bounding box
		ncols = 0;
		for (j = 0; j < NRECTPOINTS; j++) {
			outerpoint.x = oi->pos.x + oi->pr[RECTPOINTOFF + j].x;
			outerpoint.y = oi->pos.y + oi->pr[RECTPOINTOFF + j].y;
			outerpoint.z = oi->pos.z + oi->pr[RECTPOINTOFF + j].z;
			k = j & 1;
			innerpoint.x = oi->pos.x + oi->pr[MIDPOINTOFF + k].x;
			innerpoint.y = oi->pos.y + oi->pr[MIDPOINTOFF + k].y;
			innerpoint.z = oi->pos.z + oi->pr[MIDPOINTOFF + k].z;
			if (st2_line2road(&innerpoint, &outerpoint, &colpoint, &impdir)) {
				r1.x = outerpoint.x - oi->pos.x;
				r1.y = outerpoint.y - oi->pos.y;
				r1.z = outerpoint.z - oi->pos.z;
				r1s[ncols] = r1;
				impdirs[ncols] = impdir;
				ncols++;
			}
		}
#define DETECTFALLTHRUROAD
		// can't get it to work
#ifdef DETECTFALLTHRUROAD
//	if (oi->pos.y<0)
//		ricklogger("special note2 kludge fix fall thru road\n");
//	if (!ncols) {
//	if (cbairtime>20) {
//	if (cbairtime>20 && !ncols) {
//		if (oi->pos.y<0 && ulastloc.y>0)
//			ricklogger("special note kludge fix fall thru road\n");
		if (st2_line2road(&ulastloc, &oi->pos, &colpoint, &impdir)) {
			logger("special kludge fix fall thru road\n");
			//			oi->pos=ulastloc;
			//			oi->carvelang=zerov;
			//			oi->carang=zerov;
			//			oi->carang.w=1;
			//			playasound(54,0); // watermelon sound
			r1s[ncols] = zerov;
			impdirs[ncols] = impdir;
			ncols++;
		}
		//	}
#endif
// calculate ncol different ways, then avg it
		for (k = 0; k < ncols; k++) {
			oi->carvel = savevel;
			oi->carvelang = savevelang;
			for (i = 0; i < ncols; i++) {
				j = k + i;
				if (j >= ncols)
					j -= ncols;
				carbodymag = collbox2road(oi, &r1s[j], &impdirs[j], &impulse);
				if (carbodymag > 0) {
					// get friction of carbody on road
					//				ricklogger("carbodymag = %9.3f\n",carbodymag);
					//				ricklogger("impulse = %9.3f,%9.3f,%9.3f\n",impulse.x,impulse.y,impulse.z);
					if (lastairtime == 0)
						lastairtime = airtime;
					//				ricklogger("carbody airtime = 0\n");
					airtime = 0;
					cbairtime = 0;
					//				if (impmag>2.5f)
					//					airtime=-1;

					fricbox2road(fb, oi, &r1s[j], &impdirs[j], carbodymag, &impfric);
					//				ricklogger("impfric  = %9.3f,%9.3f,%9.3f\n",impfric.x,impfric.y,impfric.z);
					impulse.x += impfric.x;
					impulse.y += impfric.y;
					impulse.z += impfric.z;

					// add in impulse
					//				ricklogger("i %d, j %d, k %d, ncols %d, applyforce %9.3f %9.3f %9.3f at %9.3f %9.3f %9.3f\n",
					//					i,j,k,ncols,impulse.x,impulse.y,impulse.z,r1s[j].x,r1s[j].y,r1s[j].z);
					applyforce(oi, &impulse, &r1s[j]);
					//				procgrav=0;
				}
			}
			dvels[k] = oi->carvel;
			dvelangs[k] = oi->carvelang;
		}
		// average different methods
		if (ncols) {
			oi->carvel = zerov;
			oi->carvelang = zerov;
			for (k = 0; k < ncols; k++) {
				oi->carvel.x += dvels[k].x;
				oi->carvel.y += dvels[k].y;
				oi->carvel.z += dvels[k].z;
				oi->carvelang.x += dvelangs[k].x;
				oi->carvelang.y += dvelangs[k].y;
				oi->carvelang.z += dvelangs[k].z;
			}
			oi->carvel.x /= ncols;
			oi->carvel.y /= ncols;
			oi->carvel.z /= ncols;
			oi->carvelang.x /= ncols;
			oi->carvelang.y /= ncols;
			oi->carvelang.z /= ncols;
		}
#endif
		//	ricklogger("  after carbody2road, %9.3f %9.3f %9.3f     %9.3f %9.3f %9.3f\n",
		//		oi->carvel.x,oi->carvel.y,oi->carvel.z,oi->carvelang.x,oi->carvelang.y,oi->carvelang.z);
			// slow down yspin
		//	if (nocrash)
		//		airtime=0;
		//	oi->carvelang.y*=.95f;
		//	airtime=0;
		// if no shock info get it from carbody
		if (!validroadnorm && ncols > 0) {
			st2_callsurffunc();
			lastroadnorm = zerov;
			for (k = 0; k < ncols; k++) {
				lastroadnorm.x += impdirs[k].x;
				lastroadnorm.y += impdirs[k].y;
				lastroadnorm.z += impdirs[k].z;
			}
			normalize3d(&lastroadnorm, &lastroadnorm);
			validroadnorm = 1;
		}
		//	nanerr("aftercarbody2road posx",oi->pos.x);
		//	nanerr("aftercarbody2road posy",oi->pos.y);
		//	nanerr("aftercarbody2road posz",oi->pos.z);
		//	nanerr("aftercarbody2road velx",oi->carvel.x);
		//	nanerr("aftercarbody2road vely",oi->carvel.y);
		//	nanerr("aftercarbody2road velz",oi->carvel.z);
		if (ufliptime > 140 && !doacrashreset) {
			//		playasound(18,0);
			//		utotalcrashes++;
			ufliptime = 0;
			doacrashreset = 1;
		}
		if (!validroadnorm || dot3d(&carnorm, &lastroadnorm) > .1f)
			ufliptime = 0;

		if (validroadnorm && carvelmag > 5 && !doacrashreset && flymode && !dofinish) {
			//		VEC carpdir,carpvelnorm;
			//		float dt;
			if (dot3d(&carnorm, &lastroadnorm) < .25f) {
				if (lastairtime > 60) {
					airtime = -1; // signal a crash pitch
				}
			}
			/*		else if (oi->carvel.x*oi->carvel.x+oi->carvel.z*oi->carvel.z>12) { // yaw
						proj2plane(&cardir,&lastroadnorm,&carpdir);
						proj2plane(&carvelnorm,&lastroadnorm,&carpvelnorm);
						normalize3d(&carpdir);
						normalize3d(&carpvelnorm);
						dt=dot3d(&carpdir,&carpvelnorm);
						if (dt<.5f && dt>-.5f)
							airtime=-1;
					} */
		}
		lastairtime = 0;
		if (airtime == -1) {
			int deferx, deferz;
			deferx = mt_random(2);
			deferz = mt_random(3);
			dembones[deferx][deferz].curpushin = dembones[deferx][deferz].dampushin;
			playatagsound(18 + mt_random(3));
			utotalcrashes++;
			doacrashreset = 1;
		}
	}

	/////////////// move
	float uextraheading;
	float defuextraheading;
	float maxuextraheading;
	void updaterots()
	{
		int i;
		VEC q;//,q2,q3;
		JXFORM jx;
		float t;
		float ramp;
		struct object *op;
		for (op = objects, i = 0; i < NUMOBJECTS; i++, op++) {
			if (flymode && startstunt) {
				VEC carstuntrpyrad;
				carstuntrpyrad.x = carstuntrpy.x*PIOVER180;
				carstuntrpyrad.y = carstuntrpy.y*PIOVER180;
				carstuntrpyrad.z = carstuntrpy.z*PIOVER180;
				rpy2quat(&carstuntrpyrad, &op->carang);
			} else {
				//#define DOEXTRAHEADING
#ifdef DOEXTRAHEADING
				q2 = zerov;
				uextraheading = defuextraheading;
				//			if(urollright && !uready && (uleft || uright))
				//				uextraheading +=1.5;
				//			else
				if (accelspin > 30)
					ramp = 1;
				else if (accelspin < -30)
					ramp = 1;
				else
					ramp = (float)fabs(accelspin / 30);
				//			if(uextraheading>maxuextraheading)
				//				uextraheading = maxuextraheading;
				quatinverse(&op->carang, &q);
				quat2xform(&q, &jx);
				ramp *= maxuextraheading;
				xformvec(&jx, &objects[0].carvelang, &q); // q is car relative rotaxis velocity
				if (uright && accelspin > 0 || uleft && accelspin <= 0)
					q2.y += uextraheading;
				if (uleft && accelspin > 0 || uright && accelspin <= 0)
					q2.y -= uextraheading;
				if (!uright && !uleft) { // straighten out
					if (q.y > 0)
						q2.y = -uextraheading;
					else if (q.y < 0)
						q2.y += uextraheading;
				}
				q3.x = q.x;
				q3.y = q2.y + q.y;
				q3.z = q.z;
				if (q3.y > ramp)
					q2.y = 0;
				if (q3.y < -ramp)
					q2.y = 0;
				if (q3.y*q.y <= 0)
					q2.y = 0;
				quat2xform(&op->carang, &jx);
				xformvec(&jx, &q2, &q2);
				objects[0].carvelang.x += q2.x;
				objects[0].carvelang.y += q2.y;
				objects[0].carvelang.z += q2.z;
#endif
#define DOEXTRAHEADING2
#ifdef DOEXTRAHEADING2
				float dt, dt2;
				float delhead = 0;
				static int steertime;
				VEC newvel;
				q.x = 0;
				q.y = 1;
				q.z = 0;
				// gradual steering
				if (uright || uleft)
					steertime++;
				else
					steertime = 2;
				if (steertime > 8)
					steertime = 8;
				if (uleft || uright)
					uextraheading = defuextraheading * steertime / 8;
				else
					uextraheading = defuextraheading; // don't gradual when trying to go straight
	// end gradual steering
				if (accelspin > 30)
					ramp = 1;
				else if (accelspin < -30)
					ramp = 1;
				else
					ramp = (float)fabs(accelspin / 30);
				ramp *= maxuextraheading;
				quat2xform(&op->carang, &jx);
				xformvec(&jx, &q, &q); // q is in world, direction of y axis of rotvel car in rotaxis
				dt = dot3d(&q, &objects[0].carvelang);
				if (uright && accelspin >= 0 || uleft && accelspin < 0)
					delhead += uextraheading;
				if (uleft && accelspin >= 0 || uright && accelspin < 0)
					delhead -= uextraheading;
				if (!uright && !uleft) { // straighten out
					if (dt > 0)
						delhead -= uextraheading;
					else if (dt < 0)
						delhead += uextraheading;
				}
				newvel.x = objects[0].carvelang.x + delhead * q.x;
				newvel.y = objects[0].carvelang.y + delhead * q.y;
				newvel.z = objects[0].carvelang.z + delhead * q.z;
				dt2 = dot3d(&q, &newvel);
				if (dt2 > ramp) { // move from >ramp to ramp
					newvel.x += (ramp - dt2)*q.x;
					newvel.y += (ramp - dt2)*q.y;
					newvel.z += (ramp - dt2)*q.z;
				} else if (dt2 < -ramp) { // move from <-ramp to -ramp
					newvel.x += (-dt2 - ramp)*q.x;
					newvel.y += (-dt2 - ramp)*q.y;
					newvel.z += (-dt2 - ramp)*q.z;
				} else if (dt*dt2 < 0 && !uright && !uleft) { // move from dot2 to 0
					newvel.x -= dt2 * q.x;
					newvel.y -= dt2 * q.y;
					newvel.z -= dt2 * q.z;
				}
				objects[0].carvelang = newvel;
#endif
				q = objects[0].carvelang;
				t = normalize3d(&q, &q);
				if (t) {
					//				t*=PIUNDER180;
					t *= timeinc;
					q.w = t;
					rotaxis2quat(&q, &q);
					quattimes(&q, &op->carang, &op->carang); // world relative rotations
				}
			}
		}
	}

	extern int cantdrive;
	void updatetrans()
	{
		int i;
		static int lastkludge;
		VEC hipoint, lopoint, dum, intsect;
		struct object *op;
		extern int doacrashreset;
		extern VEC crashresetloc;
		extern float crashresetdir;
		extern VEC ulastpos;
		//	VEC vel;
		//	JXFORM jx;
		U32 jbut = 0; // joystick
		for (op = objects, i = 0; i < NUMOBJECTS; i++, op++) {
			ulastloc = op->pos;
			op->pos.x += op->carvel.x*timeinc;
			op->pos.y += op->carvel.y*timeinc;
			op->pos.z += op->carvel.z*timeinc;
			if ((op->pos.y < -2 || wininfo.keystate[' '] || (jbut & 4)) && !lastkludge || doacrashreset == 20 && !dofinish) {
				// kludge car back onto road (if fell thru the floor) , or now reset car after a crash
				cantdrive = 1;
				if (wininfo.keystate[' '] || (jbut & 4))
					playatagsound(39); // horn
				else if (!dofinish)
					playatagsound(52); // give me a warp sound! (crows??)
				if (doacrashreset)
					doacrashreset = 0; //crash reset, sound was played 20 frames ago
	//			else {
	//				playasound(18,0); //kludge
	//				utotalcrashes++;
	//			}
	//			if (!dofinish) {
				if (!dofinish || op->pos.y < -2) {
					//				ricklogger("car fell thru road, restart\n");
					if (!dofinish)
						op->pos = crashresetloc;
					else {
						logger("special finish kludge fix\n");
						playatagsound(54);
					}
					op->carvel = zerov;
					op->carvelang = zerov;
					//				quat2rpy(&op->carang,&op->carang);
					op->carang.x = 0;
					op->carang.y = crashresetdir;
					op->carang.z = 0;
					rpy2quat(&op->carang, &op->carang);
					flymode = 0;
					startstunt = uready = 0;
					hipoint = op->pos;
					lopoint = op->pos;
					hipoint.y = 100;
					lopoint.y = -100;
					st2_line2road(&hipoint, &lopoint, &intsect, &dum);
					//				intsect.y+=2.95f;
					intsect.y += .95f;
					op->pos = intsect;
					ulastpos = op->pos;
					//				lastkludge=1;
				}
			} else {
				//			lastkludge=0;
			}
		}
		lastkludge = (wininfo.keystate[' '] || (jbut & 4));
	}

}

