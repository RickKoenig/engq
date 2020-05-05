namespace st2 {
	float crossf2(VEC *a, VEC *b);
	int insidetri(VEC *vbase, FACE *t, VEC *p);
	float getroadinfo(VEC *vbase, FACE *tri, VEC *xz, VEC *norm);
	float doroadheight(TREE **t, int nt, VEC *pos, VEC *maxnorm, float lrh, float *close);
	float doroadheighto(TREE **t, int nt, VEC *pos, VEC *maxnorm);
	float doroadheight2(model *b, VEC *pos, VEC *maxnorm, float lrh);
	void converttoworld(TREE *t);
	void zerokeyframes(TREE *t);
	float getpitchfromnorm(VEC *n);
	float getyawfromnorm(VEC *n);



	// from data.h


	//#define VEND (1.0e-20f)	// ends a raw prototype vertex list
	//#define MAXLOD 3
	//#define MAXPROTOS 200

	//#define NAMESIZE 32

	enum { NONE, FLAT, TEXT, XPAR, LINE, DOT };	// triangle kinds, used by POLY and FACE

	typedef struct
	{
		char startjump;
		char endjump;
		char levelend;
		char uselwsride;
		char pid;
		char pid1;
		char pid2;
		char ending;
		char supersteer;
		char hassplit;
		char wheelsready;
		char nullswitch;
		char endrum;
		char tween;
		char crash1;
		char crash2;
		char crash3;
		char crash4;
		char crash5;
		char lbank;
		char rbank;
		char startrum;
		int nowjump;
		int leftgoto;
		int rightgoto;
		int turnval;
		int jumpback;
		int specialflag;
		float roadheight;
		float leftcone;
		float rightcone;
		char caroff;
		char b;
		char c;
		char d;
		float jumpfriction;
		float temp3;
		float temp4;
		float temp5;
	} TRACKDATA;

	typedef struct
	{
		char soundtweak;
		char soundloop;
		char hassound;
		char soundid;
		char landsound;
		int newval;
		int temp1;
		int temp2;
		int temp3;
		int temp4;
		int temp5;
	} SOUNDDATA;

	//TRACKDATA *td;
	//SOUNDDATA *sd;

	typedef struct
	{
		char trackname[32];   //name of the track
		char dimname[32];     //dimmer object
		char carnull[32];
		char scoresurf[32];
		int tms;
		int gold;             //win time for gold
		int silver;           //win time for silver
		int bronze;           //win time for bronze
		char islocked;	    //is this track locked, probably won't need
		char winlevel;
		char winningcar;      //this is car that you used to beat the level with
		int totaltrickpoints; //total # of stunt points
		char numcrashes;      //total # of crashes
		char numdents;		//total # of dents to car
		char numsecrets;      //total # of secrets found
		char locked;
	} TOURNEY_STRUCT;


	/*
	///// basic keyframe
	struct keyframe {
		int framenum;
		VEC pos;
		VEC rot;
		VEC scale;
	};

	///// basic envelope
	struct envkeyframe {
		int framenum;
		float val;
	};

	//// a sequence
	struct keyinfo {
		struct keyframe *keys;
		int nkeys;
		int lastkeyframe;

		struct envkeyframe *envkeys;
		int envnkeys;
		int envendbehavior;
		int envlastkeyframe;

	};
	*/


	/////// cubic /////
	struct keyf {
		int framenum;
		float val;
		int linear;
		float tens;
		float cont;
		float bias;
		float a, b, c, d; // at3+bt2+ct+d
	};

	//// a sequence
	struct keyframes {
		int nkeys;
		int lastkeyframe;	// keys[nkeys-1].framenum
		struct keyf *keys;
		int startframe, endframe;
		int endbehavior;
	};
	//// end cubic //////

	/*
	//// an envelope sequence
	struct envkeyinfo {
		int endbehavior;
		int startframe,endframe;
		int lastkeyframe;
	};
	*/

#define MAXSEQUENCES 100
	////// main structure
	enum { KPOSX, KPOSY, KPOSZ, KROTX, KROTY, KROTZ, KSCLX, KSCLY, KSCLZ, KENV };
	typedef struct {
		TREE *obj;
		int curframe, lastframe;
		int nframes;
		int w, h;
		MAT *mat;
		unsigned short *texdata[128];
		unsigned short *desttex;
	} SPRITEANIMTEX;

	typedef struct
	{
		TREE *t;
		int startframe;
		int endframe;
		int res1;
		int res2;
		int res3;
		int res4;
		int res5;
	} CRASHES_STRUCT;

	typedef struct
	{
		int topspeed;
		float accel;
		float traction;
		float yawspeed;
		float pitchspeed;
		float rollspeed;
		int goodtrack;
	} CARRATE;

	typedef struct
	{
		TREE *car;
		TREE *shad;
		char carname[32];
		char spname[32];
		char shadowname[32];
		char tspname[32];
		char mainobject[32];
		char surfname[32];
		char clpname[32];
		char coinsp[32];
		char texsets;
		char locked;
		char revsound;
	} CARLIST;

	typedef struct
	{
		TREE *t;
		char ridename[32];
		char hitrackname[32];
		char soundtrackdata[32];
		char trackdata[32];
		char tagfile[32];
		char sagfile[32];
		char trackwindow[32];
		float movelr;
		float rotlr;
		float cmovelr;
		float crotlr;
		float jumpvel;
		int nframes;
		int x;
		int y;
		int usenullswitch;
		int timetobeat;
		int locked;
		float coldist;
	} TRACKS;

	// use just 8 in x and 8 in z for now
#define ST2_COLLGRIDX 16
#define ST2_COLLGRIDZ 16
#define NCOLLLWOS 4
#define MAXMATFUNCS 32
	typedef struct
	{
		// FACE *faces;
		VEC *verts;
		float offx, offz;
		float mulx, mulz;
		int nfaceidx[ST2_COLLGRIDZ][ST2_COLLGRIDX];
		FACE **faceidx[ST2_COLLGRIDZ][ST2_COLLGRIDX];
		void(*matfuncs[MAXMATFUNCS])(); // used by uplay3, interact with different surfaces
	} COLLGRID;

	typedef struct
	{
		TREE *t;
		int piece;
		int lpiece;
		int previous;
		//  int lor;
		float orhey;
		//  int bodyid;
		int curpiece;
		int add;
		TREE *tlwos[NCOLLLWOS];	// upto 4 collision pieces to collide with, only used in uplay? and uphysics?
		COLLGRID clwos[NCOLLLWOS];
		int nlwos;
	} TRACK;

	typedef struct
	{
		char name[64]; // used by uplay3
		char lwonames[NCOLLLWOS][64]; // only used by uplay? and uphysics?
		int nlwos;	// only used by uplay? and uphysics?
		TREE *piece;
		int changerot;
		int newor;
	} PIECE;

	typedef struct
	{
		int n;
		int e;
		int s;
		int w;
	} PIECE_TAB;

	typedef struct
	{
		TREE *bone;
		char bonename[32];
		int whichaxis;
		float amount;
	} BONERS;


	//TRACK thetrack[NUMBODS];


	//extern struct proto protos[MAXPROTOS];
	//int nprotos;

	////// global functions
	TREE *alloctree(int nsubs, char *numberc);
	TREE *alloctreeid(int nchildren, int id);
	TREE *duptree(TREE *t);
	void freetree(TREE *t);
	void rickloggertree(TREE *t);

	void unhooktree(TREE *t);
	void setorientation(TREE *t, VEC *trans, VEC *rot, VEC *transvel, VEC *rotvel);
	void setscale(TREE *child, VEC *scale);
	void linkchildtoparent(TREE *child, TREE *parent);
	//void setlod(TREE *t,int lod);	// call to override lod, or -1 to cancel override

	// proto stuff
	//void dupproto(char *oldproto,char *newproto);
	//void freeprotos();
	void lockbodies();
	void unlockbodies();
	int readbodydeluxe(char *nibname);

	void startbuildbody(int nverts, int nfaces, int logw, int logh, unsigned short *tex565data, char *name16, int hflags);
	//void addtri(int f1,int f2,int f3);
	void addpoint(float x, float y, float z, float u, float v);
	void makebox(float x0, float y0, float z0, float x1, float y1, float z1);
	int endbuildbody();

	int addlwscam(TREE *t);
	void deformbones(TREE *t);

	////// internal functions
	//void updatelod(TREE *t,int lod);	// build3d will call this to load protos if necc.
	//void clearprotoupdate(TREE *t);		// draw3d will call this


	unsigned short *gettexptr(MAT *surfid, int *w, int *h, int *texid);
	void updatetex(unsigned short *tex, int w, int h, int texid);
	MAT *getmatptr(TREE *t, char *surfname);

	void killdeaddudes();

	int line2piece(model *b, VEC *top, VEC *bot, VEC *bestintsect, VEC *bestnorm);
	int line2piecegrid(COLLGRID *cg, VEC *top, VEC *bot, VEC *bestintsect, VEC *bestnorm);
}
