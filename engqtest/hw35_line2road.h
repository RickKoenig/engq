//#define TESTNUMFACES // see how many tris to check for player0 (grid)
typedef pointf3 VEC;
typedef tree2 TREE;
typedef face FACE;
typedef modelb BODY;
namespace hw35_line2road {

struct qmatfunc {
	char matname[50];
	void (*matfunc)();
	int useline2road;
	int call;
};

struct smatfunc {
	char *matname;
	void (*matfunc)();
};

//enum {SURF_FIRST,SURF_ALL};
TREE *st2_line2road(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm);
TREE *st2_line2roadlo(VEC *top,VEC *bot,VEC *bestintsect,VEC *bestnorm);

void st2_enablesurffunc(); // call to let future line2road's register surffuncs
void st2_callsurffunc(); // call once in the proc, calls registered surffuncs, disables surffunc

void st2_setupmatfuncs(struct smatfunc *,int nqmatfunc); // call in init before loadcollist
void st2_loadcollist(char *filename,TREE *root); // load alot of objects (init)
void st2_freecollgrids(); //(exit)

int gettrackchecksum();

struct colltree {
	TREE *t;
//	VEC pos,rot,scl;
	struct mat4 cto2w,ctw2o;
	int uid;
	VEC *facenorms; // points to facenorms in detachweapinfo
	VEC aabbmin,aabbmax;
};
//extern struct treecoll treecolls[NTREECOLLS];
//extern int ntreecolls;
void initcolltree();
void addcolltree(int uid,TREE *t,VEC **facenormplace,VEC *p,VEC *r,VEC *s);
void remcolltree(int uid);
#define MAXCOLLTREES 400
extern struct colltree l2r_colltrees[MAXCOLLTREES];
extern int l2r_ncolltrees;
}

using namespace hw35_line2road;
