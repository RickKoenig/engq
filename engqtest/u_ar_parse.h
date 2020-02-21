struct arparseinfo {
	pointf3 arrot;
	pointf3 artrans;
	float matcherror;
	S32 matchstable;
	S32 bigerror;
	float matchstep;
	string currentstr;
//	S32 pospitch; // bool
//	S32 negpitch; // bool, fix debprint
};

struct deginfo {
	C8* name;
	float* ptr;
};

// pass in 5 points, fov, asp, res, get back trans,rot
class doar{
public:
	static const S32 npc = 5;
	static const float zoomfactor;
private:
	static const float matchstepmin;
	static const float saddle; // tighten search
	static const float grad; // loosen search
	static const S32 degrees = 6;
	static const float pitchthreshold;
	static const S32 maxmatchstable;
	static const S32 maxbigerror;
	static const float znear,zfar;
	static const float errorthresh;
	static const pointf3 refpoints[npc];

	const pointf2 inres;
	const pointf2 outres;
	const float asp;

	pointf3 itrans,irot;

	pointf2 scan_cent; // for sorting 5 scanned points

	S32 curdegree; // 0  to  npc-1
	float matchstep; // how fast we move
	S32 matchstable; // very low error and step, bigger the better
	S32 bigerror; // not converging
	float matcherror;
//	bool pospitch; // should keep pitch between 0 and PI/2
//	bool negpitch; // should keep pitch between -PI/2 and 0
	
	pointf2 rp[npc]; // rotated reference points
	pointf2 sp[npc]; // scanned points in plotter system

	deginfo di[degrees];
	bool havescanpoints;

public:
	doar(const pointf2& inresa,const pointf2& outresa);
	void setscanpoints(const pointf2* pnts); // assume 5 points
	void doresetrottrans();
// move closer to the solution
	void calcmatchstep();
	bool isdone();
	void getarparseinfo(arparseinfo&);
	const pointf2* getrp();
	const pointf2* getsp();
private:
	bool convertto2d(const pointf3& p3,pointf2& p2); // convert to 2d using 'division', also clip
	void buildrottrans3d_cref(const pointf3* rpy,const pointf3* transp,mat4* rotm); // unoptimized
	float checkmatch();
// run 3d points all the way to plotter space
// returns true when all points within clip volume
	bool pipeline();
	float pipelinecheckmatch();
public:
	bool operator () ( const pointf2& a, const pointf2& b ); // comparision function for the 5 points sorting function
};

string readarcode(const string& name,const bitmap32* pic,vector<circleScan>& data,bitmap32** scanpic);
void blendbitmaps(bitmap32* a,const bitmap32* b,float f);
