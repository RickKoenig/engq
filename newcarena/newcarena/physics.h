const C8* getphysicsdir();

namespace n_physics {

extern float movespeed; // try to break sync! (by changing it in debprint menu)
//void docarphysics(U32 slot,keys k);
//void collidecars();
void physics_do(const vector<keys>& ks);

#define FULLPHYSICS // shocks and box2box line2road the full monte!
#ifdef FULLPHYSICS
void physics_init(); // manage 'calc' acv, load from physics/physics.txt
//extern calcj acv2;
void physics_exit();

void resetcar2car();
void calccollisioninfo(twcaro* tw,calco* tv); // bbox , points, moment of interia
void checkcar2car(caroj* aj,caroj* bj);
//void checkcar2car(int a,int b);
void doroadcollisions(caroj *coj,calco* tv,const keys& k); // obj to road, will zero airtime if car is in contact with ground
void updatetrans(twcaro* tw);
void updaterot(twcaro* tw,const keys& k); // but i'll do the stunt!

struct collinfo {
	const caroj* cn;//,b;
//	VEC val;
	pointf3 impval;
	pointf3 imppnt;
//	VEC norm;
//	VEC loc;
};
#define TIMEINC (1/60.0f)
#define MAXCOLLINFO 100
//#define MAXCOLLINFO (2*OL_MAXCARSLOTS*(OL_MAXCARSLOTS-1) + 2 + 4*OL_MAXCARSLOTS)
extern struct collinfo collinfos[];
extern int ncollinfo;
extern pointf3 testimpval,testimppnt;
//void calcimpulseo2o(struct phyobject *p0,struct phyobject *p1,VEC *loc,VEC *norm);

/*struct objstate {
	VEC pos,rot,momentum,angmomentum; // angmom cm
	VEC vel,rotvel;	// always derived from above
};

struct phyobject {
//	VEC pnts[NCORNERS]; // local bbox 8 points
//	VEC rpnts[NCORNERS]; // world bbox 8 points
//	struct objstate st;
//	int notrans,norot;
//	float mass;
//	float elast;
	int boo;
//	VEC moivec;	// try out principal axis
};
*/

enum {CARCOAST,CARBRAKE,CARACCEL,CARREVERSEACCEL};

#endif
} // namespace n_physics
