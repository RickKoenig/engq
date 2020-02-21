void ol_calccollisioninfo(); // bbox , points, moment of interia
void ol_doroadcollisions(); // obj to road, will zero airtime if car is in contact with ground
void ol_updatetrans();
void ol_updaterots(); // but i'll do the stunt!

void checkcar2car(struct ol_playerdata *a,struct ol_playerdata *b);
void resetcar2car();
struct collinfo {
	int cn;//,b;
//	pointf3 val;
	pointf3 impval;
	pointf3 imppnt;
//	pointf3 norm;
//	pointf3 loc;
};
#define MAXCOLLINFO (2*OL_MAXCARSLOTS*(OL_MAXCARSLOTS-1) + 2 + 4*OL_MAXCARSLOTS)
extern struct collinfo collinfos[];
extern int ncollinfo;
extern pointf3 testimpval,testimppnt;
//void calcimpulseo2o(struct phyobject *p0,struct phyobject *p1,pointf3 *loc,pointf3 *norm);

/*struct objstate {
	pointf3 pos,rot,momentum,angmomentum; // angmom cm
	pointf3 vel,rotvel;	// always derived from above
};

struct phyobject {
//	pointf3 pnts[NCORNERS]; // local bbox 8 points
//	pointf3 rpnts[NCORNERS]; // world bbox 8 points
//	struct objstate st;
//	int notrans,norot;
//	float mass;
//	float elast;
	int boo;
//	pointf3 moivec;	// try out principal axis
};
*/
extern int showphysics;
