namespace st2 {
#define NUMOBJECTS 1

#define NSHOCKPOINTS 8
#define NRECTPOINTS 8
#define NMIDPOINTS 2

#define SHOCKPOINTOFF 0
#define RECTPOINTOFF (SHOCKPOINTOFF+NSHOCKPOINTS)
#define MIDPOINTOFF (RECTPOINTOFF+NRECTPOINTS)

#define NCARPOINTS (MIDPOINTOFF+NMIDPOINTS) 
#define NWORKINGSHOCKS 4
	//#define POWERSHOCK 0

	struct object {
		////// fundamental
		// type
		// cosmetic
		//	int color;
		// position of cm
		VEC pos;
		// shape of object
		VEC len; // lenx is also radius
	// total mass
		float mass; // mass of 0 is infinite
		VEC carvel; // units per sec
	// rotation
		VEC carang; //...... convert to quat, isa quat
	// time related
		VEC carvelang;	//rads per frame, about cm isa rotaxis
	// friction
	//	float cof; // contact friction coeff
	/////////// derived points
		float moi;	// moment of inertia about cm, keep simple for now (i.e. not a tensor)
		float shocklen[6];
		//	int startroad,endroad;	// for smartroad
		// drawing and collision points
		VEC p[NCARPOINTS];
		VEC pr[NCARPOINTS];
		// wheel spin
		float wheelang[6];
		float wheelvel[6];
		float wheelyaw;
		float steering;
		// bounding box
		VEC bb0, bb1;
		//	int nograv;
	};

	extern struct object objects[NUMOBJECTS], iobjects[NUMOBJECTS];

	void updaterots();
	void doroadcollisions();
	void updatetrans();
	void dograv(float);
	void doairfric(float);
	void calccollisioninfo();

	extern float caroffsetx, caroffsety, caroffsetz, wheelrad, wheelwid, wheellen;
	extern float shockoffsety, shockspacingx, shockspacingz;
	extern int magnatraction;
	extern VEC carbboxmin, carbboxmax;
	extern float timeinc;
	//extern float timeincconst; // always 1/30
	const float timeincconst = 1 / 30.0f;
	extern int steershocks[6];
	extern int drivemode;
	enum { CARCOAST, CARBRAKE, CARACCEL, CARREVERSEACCEL };
	extern float accelspin;
	//extern int justshocks;
	extern int flymode, startstunt;
	extern VEC carstuntrpy;

	extern float littleg, airfric;
}
