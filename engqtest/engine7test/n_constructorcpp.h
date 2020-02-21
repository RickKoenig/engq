void constructorinit(),constructorproc(),constructordraw3d(),constructordraw2d(),constructorexit();
extern viewport2 constructor_viewport,constructor_viewport2;//,constructor_viewport3; // for debvars
//tree2 *loadconstrack(const char *trackname);
extern tree2* constructor_viewport_roottree,*constructor_viewport2_roottree;

#define TRACKX 8
#define TRACKZ 8
struct piece {
	char pt;
	char rot;
//	tree2 *t;
};
struct trk {
	struct piece pieces[TRACKZ][TRACKX];
	int basetex;
//	float terr[TRACKZ+1][TRACKX+1];
};
struct track {
	struct trk trkdata;
	tree2 *treedata[TRACKZ][TRACKX];
};

int loadtrackscript(const char *trackname,trk *tk);
tree2 *buildconsscene(track *t);

const char *saveconstrack(const char *name,trk *td,int uniq);
