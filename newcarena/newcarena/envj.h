// global
class calc { // POD
public:
	calc();
	float extrashocklen[NWORKINGSHOCKS];
	float normalshocklen[NWORKINGSHOCKS];
	float shockextra;
//	pointf3 p[NCARPOINTS],pr[NCARPOINTS];
	pointf3 p[NCARPOINTS];
	float shockspacingx,shockspacingz;
	float shockoffsety;
	pointf3 carbboxmin,carbboxmax;
//	pointf3 c2cpnts[NRECTPOINTS];
//	float shocklen[NWORKINGSHOCKS];
//	float wheelvel[NWORKINGSHOCKS];
	float shockstr,shockdamp;
	float moi;
	float mass;
	float elast;
	float airfric,littleg;
	float wheelrad,wheelaccel;
//	float accelspin;
	float fricttireslide,fricttireroll,frictcarbody;
	float drivemode;
	float startaccelspin;
	float c2celast;
//	float carid;
	float defuextraheading; // amount to add to turn rate
	float maxuextraheading; // fastest turn rate
	float maxturnspeed; // accelspin at which extraheading becomes maxextraheading
	pointf3 testimpval,testimppnt;
	S32 maxnoshocktime,maxnocheckpointtime,maxnoresettime; // flip car over
	float carflipheight; // flip car over
	float carstartheight;
// camera vars
	pointf3 camatt;
	float camattpitch;
	float camattdist;
// new ones
	float car2cardist;
	S32 candrivetime; // countdown to 'GO!', 180
	S32 rematchtime; // seconds after finish, turn on rematch dialog
	S32 norematchtime; // seconds after finish, turn off rematch dialog
	float sndc2cmag;
	float speedofsound;
};

// per object scratch
class calcj : public calc {
public:
	calcj();
	calcj(const calc& ca) : calc(ca) {}
	calcj(const json& j);
	json save() const;
};

extern calcj acv2;

class envj {
	static bool pcnet;
public:
	string trackname;
	trkt* oldtrackj;
	struct newtrack* newtrackj;
	hashj trackhash;
	S32 rules;
	S32 nlaps;
	calc pc;
	envj(const json& j);
	envj() : oldtrackj(0),newtrackj(0),rules(0) {}
	json save() const;
	void buildtrackhash();
	void buildtrackhashtree();
	void cleantrack() {delete oldtrackj; oldtrackj=0;}
	~envj() { cleantrack(); }
	envj& operator=(const envj& rhs);
	static void setpcnet() { pcnet = true; } // embed the physics constants into envj when 'save', for network
};
