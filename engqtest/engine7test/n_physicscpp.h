namespace n_physics {
void docarphysics(caro *);
#if 1
void ol_calccollisioninfo(); // bbox , points, moment of interia
void ol_doroadcollisions(); // obj to road, will zero airtime if car is in contact with ground
void ol_updatetrans();
void ol_updaterots(); // but i'll do the stunt!

void checkcar2car(struct ol_playerdata *a,struct ol_playerdata *b);
void resetcar2car();
struct collinfo {
	int cn;//,b;
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
#endif
}
