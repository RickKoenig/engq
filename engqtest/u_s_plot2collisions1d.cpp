// plotter2 runs in FPU control high precision
// do simple 1d physics collisions of 2 1d objects
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_plotter2.h"
#include "u_states.h"

using namespace u_plotter2;

namespace collisions1dp2 {

	const float INVSQRTPI = 1.0f / sqrt(PI);
	// just the object itself
	// start properties of object
	// at t = 0
	struct basePhyobj {
		float mass;
		float pos; // start position
		float vel;
		void limit();
	};

	// including derived components
	// current properties of object
	// t >= 0
	struct phyobj {
		// base parameters
		basePhyobj current;
		// modified by time
		// derived parameters
		float p; // momentum
		float ke; // kinetic energy
		float im; // inverse mass
		float rad; // radius
		void copy(const basePhyobj& bo);
		void move();
		void update();
		void addImpulse(float imp);
		void draw() const;
	};

	void phyobj::draw() const
	{
		drawfpoint(pointf2x(current.pos, 0), C32RED);
		if (im != 0) {
			drawfcirclef(pointf2x(current.pos, 0), C32(85, 0, 0), rad);
		} else {
			drawfline(pointf2x(current.pos, -10), pointf2x(current.pos, 10), C32(85, 0, 0));
		}
	}

	void phyobj::update()
	{
		if (current.mass != 0) {
			im = 1.0f/current.mass;
			p = current.mass * current.vel;
			ke = p * current.vel * .5f;
			rad = sqrt(current.mass)*INVSQRTPI; // do 2d for now
		} else {
			im = 0.0f; // infinite mass, like a wall
			p = 0; // throw infinities under the rug
			ke = 0;
			rad = 0;
			current.vel = 0;
		}
	}

	void phyobj::addImpulse(float imp)
	{
		if (current.mass) {
			current.vel += imp * im;
			update();
		}
	}

	void phyobj::copy(const basePhyobj& bo)
	{ 
		current = bo;
		update();
	}

	void phyobj::move() 
	{ 
		current.pos += current.vel / wininfo.fpswanted;
	}

	void basePhyobj::limit()
	{
		mass = range(0.0f, mass, 100.0f);
		pos = range(-20.0f, pos, 20.0f);
		vel = range(-20.0f, vel, 20.0f);
	}

	basePhyobj bobj1{
		1, -10,  5
	};
	basePhyobj bobj2{
		100,  10, -3
	};

	phyobj obj1;
	phyobj obj2;

	float curTime;
	float maxTime = 10.0f; // in seconds
	float elast = 1.0f;

	float totalP;
	float totalKE;

	// for debvars
	struct menuvar simpledv[]={
		{"@lightred@--- PLOTTER2 USER VARS FOR PHYSICS 1D ---",NULL,D_VOID,0},
		{"--- starting values ---",NULL,D_VOID,0},
		{"Start P1M",&bobj1.mass,D_FLOAT, FLOATUP / 8},
		{"Start P1X",&bobj1.pos,D_FLOAT, FLOATUP / 8},
		{"Start P1V",&bobj1.vel,D_FLOAT, FLOATUP / 8},
		{"Start P2M",&bobj2.mass,D_FLOAT, FLOATUP / 8},
		{"Start P2X",&bobj2.pos,D_FLOAT, FLOATUP / 8},
		{"Start P2V",&bobj2.vel,D_FLOAT, FLOATUP / 8},
		{"@red@--- current values ---",NULL,D_VOID,0},
		{"Current P1M",&bobj1.mass,D_FLOAT | D_RDONLY},
		{"Current P1X",&obj1.current.pos,D_FLOAT | D_RDONLY},
		{"Current P1V",&obj1.current.vel,D_FLOAT | D_RDONLY},
		{"current P1P",&obj1.p,D_FLOAT | D_RDONLY},
		{"current P1KE",&obj1.ke,D_FLOAT | D_RDONLY},
		{"Current P1M",&bobj1.mass,D_FLOAT | D_RDONLY},
		{"Current P2X",&obj2.current.pos,D_FLOAT | D_RDONLY},
		{"Current P2V",&obj2.current.vel,D_FLOAT | D_RDONLY},
		{"current P2P",&obj2.p,D_FLOAT | D_RDONLY},
		{"current P2KE",&obj2.ke,D_FLOAT | D_RDONLY},
		{"@lightred@--- global values ---",NULL,D_VOID,0},
		{"time",&curTime,D_FLOAT | D_RDONLY},
		{"maxtime",&maxTime,D_FLOAT, FLOATUP / 8},
		{"elast",&elast,D_FLOAT, FLOATUP / 8},
		{"totalP",&totalP,D_FLOAT | D_RDONLY},
		{"totalKE",&totalKE,D_FLOAT | D_RDONLY},
	};
	const int nsimpledv = NUMELEMENTS(simpledv);

	float collideObjs(phyobj& o1, phyobj& o2) // returns non zero impulse if collided
	{
		float delPos = o2.current.pos - o1.current.pos;
		float delVel = o2.current.vel - o1.current.vel;
		//if (delPos < 0 /*&& delVel > 0*/)
		//	return 1.0f;
		//if (delPos > 0 && delVel < 0)
		//	return 1.0f;
		if (delPos >= 0 && delVel >= 0)
			return 0; // moving apart
		if (delPos <= 0 && delVel <= 0)
			return 0; // moving apart
		// coming together
		if (delPos > o1.rad + o2.rad)
			return 0;
		// calc impulse
		float imp = 2.0f*elast*(o1.current.vel - o2.current.vel)/(o1.im + o2.im);
		return imp;
	}

	void resetObjs()
	{
		obj1.copy(bobj1);
		obj2.copy(bobj2);
		curTime = 0;
	}

	void procObjs()
	{
		bobj1.limit();
		bobj2.limit();
		elast = range(0.0f, elast, 1.0f);
		maxTime = range(0.0f, maxTime, 30.0f);
		obj1.move();
		obj2.move();
		float I = collideObjs(obj1, obj2);
		if (I != 0.0f) {
			obj1.addImpulse(-I);
			obj2.addImpulse(I);
		}
		totalP = obj1.p + obj2.p;
		totalKE = obj1.ke + obj2.ke;
	}

} // end namespace collisions1dp2

using namespace collisions1dp2;

void plot2collisions1dinit()
{
	curTime = 0;
	resetObjs();
	adddebvars("collisions1d",simpledv,nsimpledv);
	plotter2init();
}

void plot2collisions1dproc()
{
	// interact with graph paper
	plotter2proc();
	procObjs();
	curTime += 1.0f / wininfo.fpswanted;
	if (curTime > maxTime) {
		resetObjs();
	}
	if (KEY == 'r') {
		changestate(STATE_PLOT2COLLISION1D);
	}
}

void plot2collisions1ddraw2d()
{
	// draw graph paper
	plotter2draw2d();
	obj1.draw();
	obj2.draw();
}

void plot2collisions1dexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("collisions1d");
}
