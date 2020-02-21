struct ti {
	S32 flycam;
	float flycamspeed;
	S32 flycamrevy;
	S32 ndrawtree;
	S32 showpri;
	float defaultdissolvecutoff;
	float dissolvestart;
	S32 treedissolvecutoffenable; // if 0, just use ti.dissolvecutoff .. if 1, use tree2 treedissolvecutoff
};
extern ti treeinfo;

enum o2ptype {O2P_NONE,O2P_FROMTRANSROTSCALE,O2P_FROMTRANSQUATSCALE};

//#define TF_KILLMEC 1 // and children
#define TF_DONTDRAW 2 // don't draw me
#define TF_DONTDRAWC 4	// don't draw me and my children
//#define TF_DONTANIMATE 8
//#define TF_DONTANIMATEC 16
#define TF_ISCAMERA 32
//#define TF_ISBONE 64
#define TF_ISLIGHT 128 // gets added to lightlist
#define TF_ISSCENE 256
#define TF_ISNULL 512
//#define TF_HASBONES 1024
//#define TF_NOINFLUENCE 2048
#define TF_ISAMBLIGHT 4096
#define TF_CASTSHADOW 8192
#define TF_PICKABLE 16384 // can be detected by line to road/bbox/sphere etc.
#define TF_ALWAYSFACING 32768 // NYI
//#define TF_ALWAYSFACING 8192
//#define TF_CALCLIGHTSONCE 16384
//#define TF_DONECALCLIGHTSONCE 32768
//#define TF_ISBIPED 65536
//#define TF_MIXLIGHTS (65536*2)
//#define TF_TREECOLOR (65536*4)
//#define TF_CAMERASPACE (65536*8)

/*//#define TLF_KILLMEC 0 // and children
#define TLF_DONTDRAW 1
#define TLF_DONTDRAWC 2	// and children
#define TLF_DONTANIMATE 3
#define TLF_DONTANIMATEC 4
#define TLF_ISCAMERA 5
#define TLF_ISBONE 6
#define TLF_ISLIGHT 7
#define TLF_ISSCENE 8
#define TLF_ISNULL 9
#define TLF_HASBONES 10
#define TLF_NOINFLUENCE 11
#define TLF_ISAMBLIGHT 12
#define TLF_CALCLIGHTONCE 14
#define TLF_DONECALCLIGHTONCE 15
#define TLF_ISBIPED 16
#define TLF_MIXLIGHTS 17
#define TLF_TREECOLOR 18
//#define TLF_CAMERASPACE 19 */
enum {TRANSX,TRANSY,TRANSZ,ROTX,ROTY,ROTZ,ROTW,SCLX,SCLY,SCLZ,EXTRA,NCHAN};
struct key {
	float time,value,intan,outtan;
	S32 linear;
	float ca,cb,cc,cd;
	float tens,cont,bias;
};

struct keyframe {
//	S32 dum;
	//S32 refcnt;
	S32 nkeys;
	struct key* keys;
	S32 curkey;
//	S32 endstate;
};

struct seq {
	float startframe,endframe;
	float origendframe;
	struct keyframe* keysn[NCHAN];
};

class tree2 {
public:
// name
	string name;
// flags
	script userprop;
	U32 flags;
	float userfloat[4];
	S32 userint[4];
	pointf3 userpointf3[4];
	bool (*userproc)(tree2* t); // if return false and has a parent then caller (tree2::proc) will delete
// related decomposed components
	struct pointf3 trans;
	struct pointf3 rot;
	struct pointf3 scale;
// animation
	struct pointf3 transvel;
	struct pointf3 rotvel;
	struct pointf3 scalevel;
// keyframe
	bool doanim;
	struct seq* seqs; // just 1 sequence for now
	S32 nseq;
	S32 repcount,currepcount; // 0 loop, 1 stop
	float curframe;
	S32 curseq;
	S32 playing;
// matrices
	o2ptype buildo2p;//playo2p;
	mutable mat4 o2p,o2w;//,o2c; //,lm;
// heirarchy
	tree2* parent;
	list<tree2*>::iterator parentit;
	list<tree2*> children;
//	S32 maxchildren;
//	S32 nchildren;
//	struct tree *children[1];
//////////// for lights
	struct pointf3 lightcolor;
	float intensity;
//////////// for cameras
	float zoom;
//////////// for objects
//	S32 visible;
//	float dissolve;
	bool isedge;
	S32 srt_drawpri;
	pointf3 treecolor; // copied to mat[0] if flags set with TF_TREECOLOR, .w is dissolve
// model;
	modelb* mod;
	textureb* alttex; // override main tex in model if non null
// alpha cutoff
//	float zoff;
	float treedissolvecutoff; // 0 to 1
//	lights
//	vectore<pointf3> cverts; // good for calclightsonce
private:
	tree2(); // no default cons
	tree2(tree2&); // no copy cons
	tree2 operator=(tree2&); // no assign
	void copyanim(const tree2* rhs); // copy over ALL animation data to the copy
public:
	tree2(const C8* namea); // build a tree with a name
	tree2* newdup(); // dup a tree
	void linkchild(tree2* child); // hook
	void unlink(); // unhook
	virtual void proc(); // do
	virtual ~tree2(); // free
	void log2(); // show info/text
	void setmodel(modelb* m); // hook, model2's refcount goes up by 1
	void setvis(bool visa)
	{
	    if (visa)
            flags&=~(TF_DONTDRAW|TF_DONTDRAWC);
        else
            flags|=(TF_DONTDRAW|TF_DONTDRAWC);
	}
	void setuserproc(bool (*fun)(tree2*)) {userproc=fun;}
	tree2* find(const C8* name); // find all of the name
	tree2* findsome(const C8* name); // find part of name
	mater2* findmaterial(const C8* name);
// for sorting, no need to init
	mat4 srt_o2c;
	clipresult srt_clipresult;
// texvels
	pointf2 uvoffset;
	pointf2 uvvel;
};
typedef tree2 TREE;

void video_buildtreelist(tree2* t);
void video_sorttreelist();
void video_drawtreelist();


void video_buildworldmatearly(const tree2* t); // call if need o2w early
void obj2world(const tree2* t,const pointf3* in,pointf3* out);
void obj2worldn(const tree2* t,const pointf3* in,pointf3* out,U32 nv);
void video_buildworldmats(tree2* t);
void setcastshadow(tree2* t);

// some global thing for bws, bwo, and the userprop 'lightmap'static string sglm;

// loadbwomodel will try to use the suggested lightmap for 2nd texture
void setsuggestlightmap(string s);
string getsuggestlightmap();

void getlookat(const pointf3* from,const pointf3* to,pointf3* rpy); // look at 'to' from 'from'
