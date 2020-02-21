void constructorinit(),constructorproc(),constructordraw3d(),constructordraw2d(),constructorexit();
extern viewport2 constructor_viewport,constructor_viewport2;//,constructor_viewport3; // for debvars
extern tree2* constructor_viewport_roottree,*constructor_viewport2_roottree;

#define TRACKX 8
#define TRACKZ 8
#define PIECESIZE 40 // 40 meters by 40 meters
//#define PIECEHEIGHT (40.0f/6.0f) 
//#define PIECEHEIGHT 2.5f // 0 to 2.5*4 or 10 meters
enum {DIR_RIGHT,DIR_DOWN,DIR_LEFT,DIR_UP,DIR_NUM};

enum piecetype {
	PCE_BLANK,
	PCE_STRAIGHT,
	PCE_TURN,
	PCE_STARTFINISH,
	PCE_INTERSECTION,
	PCE_RIGHTFORK,
	PCE_LEFTFORK,
	PCE_NPIECETYPE
};

enum frk {FNONE,FSTRAIGHT,FTURN,FBOTH};

struct piece { // POD
	static const S32 nseg=6;
	S32 leftwidsegs[nseg+1];
	S32 rightwidsegs[nseg+1];
	S32 heightsegs[nseg+1];
	S32 pt;
	S32 rot;
	frk forkstate;
	S32 dist2[DIR_NUM];
	piece();
	piece(const class json&);
	json save() const;
};

struct trk {
	struct piece pieces[TRACKZ][TRACKX];
	int basetex;
	int skybox;
	int backsnd;
//	const float sqrt2o2;
//	const float pieceheight;
	trk() {} //: sqrt2o2(0),pieceheight(0) {}
	trk(const class json&);
	json save() const;
	hashi gettrackhash() { return hashi((U8*)this,sizeof(*this)); }
//	bool line2road(const pointf3* top,const pointf3* bot,pointf3* bestintsect,pointf3* bestnorm);
//	bool line2road(const pointf3* oldpnt,const pointf3* newpnt,const pointf3* oldrot,pointf3* bestintsect,pointf3* bestnorm,pointf3* bestrot) const; // returns out, if neg then penetration
// check walls
	bool line2roadxz(pointf3* pnt,pointf3* rot,pointf3* normjj=0) const; // reads and writes both pnt and rot
// check floor/road
	bool line2road(pointf3* pnt,pointf3* normjj=0) const; // reads and writes  pnt 
	void line2roadvert(pointf3* pnt) const; // reads and writes  pnt finds surface above or below pnt
	void curpiece(const pointf3* newpnt,pointf3* off,pointi2* ppos,S32* pt,S32* rot) const;
	void curpiece(const pointi2* ppos,S32* pt,S32* rot) const;
	void studyforks();
};

struct trkt : public trk {
	tree2* root; // not? handled/owned by roottree
	tree2 *treedata[TRACKZ][TRACKX]; // not? handled/owned by roottree
	trkt() : root(0) { fill(treedata[0],treedata[0]+TRACKZ*TRACKX,(tree2*)0);}
	trkt(const class json& j);
	void buildtrack(bool newskybox = true); // (re)populate member values in trkj
	wavehandle* buildbacksnd();
	~trkt()
	{ 
		logger("deleting %p\n",root);
		delete root; // this will free treedata too, connected
	}
private:
	trkt operator=(const trkt& rhs);
};

const C8* getconstructeddir(); // checks wininfo.isalreadyrunning, allows test of trackhash between 2 different folders
