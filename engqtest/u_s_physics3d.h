void physics3dinit(),physics3dproc(),physics3ddraw3d(),physics3dexit();

#define NCORNERS 8

#define FIRSTMESHOBJ 1

// phyobjects

struct objstate {
	VEC pos,rot,momentum,angmomentum; // angmom cm
	VEC vel,rotvel;	// always derived from above
};
/*
struct contact {
//	float sep;
//	VEC *p0,*p1,*p2; // pointers to world coords
//	VEC *ae0,*ae1,*be0,*be1; // edges
//	int planeonb;
//	VEC norm;
//	VEC cloc;	// location on plane
	int dum;
};
*/
struct nb {
	int nnb;
	int visited;
	int *nbs;
	int nnballoced;
};

struct nbf {
	int nnbf;
//	int visited;
	int *nbfs;
	int nnbfalloced;
};

struct phyobject {
// object
	int kind; // box1, cyl1, sph1, look in objects.txt, right now box1 is special
	VEC scale;
// gen object
	// 8 box points
	VEC pnts[NCORNERS]; // local bbox 8 points
	VEC rpnts[NCORNERS]; // world bbox 8 points
	// mesh points if kind>=3
	int haswf; // object has current world verts
	int nwpnts; 
	int nwfaces;
	VEC *wpnts; // malloced world verts
	struct nb *nbs; // neighboring verts
	struct nbf *nbfs; // neighboring faces
	FACE *lfaces; // handy ptr 
	VEC *lpnts; // handy ptr
	//struct contact *contacts; // indexed by the other object
	// pos
	struct objstate s0,st; // motion: s0 -> st, collisions st->st
	float transenergy;
	float potenergy;
	float rotenergy;
// parameters
	float mass;
	float elast;
	float frict;
// generated parameters
	VEC moivec;	// try out principal axis
	int norot,notrans;
// reference to root node of object
	TREE *t;
};
