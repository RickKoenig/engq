namespace n_line2road {
struct qmatfunc {
	char *matname;
	void (*matfunc)();
	int useline2road;
	int call;
};

enum {SURF_FIRST,SURF_ALL};
bool st2_line2road(const pointf3* top,const pointf3* bot,pointf3* bestintsect,pointf3* bestnorm);
bool st2_line2roadlo(const pointf3* top,const pointf3* bot,pointf3* bestintsect,pointf3* bestnorm);

void st2_enablesurffunc(); // call to let future line2road's register surffuncs
void st2_callsurffunc(); // call once in the proc, calls registered surffuncs, disables surffunc

void st2_setupmatfuncs(struct qmatfunc *,int nqmatfunc); // call in init before loadcollist
void st2_loadcollist(const char *filename,const tree2* root); // load alot of objects (init)
void st2_freecollgrids(); //(exit)

int gettrackchecksum();

struct colltree {
	const tree2* t;
//	VEC pos,rot,scl;
	struct mat4 cto2w,ctw2o;
	int uid;
	pointf3* facenorms; // points to facenorms in detachweapinfo
	pointf3 aabbmin,aabbmax;
};
//extern struct treecoll treecolls[NTREECOLLS];
//extern int ntreecolls;
void initcolltree();
void addcolltree(int uid,const tree2* t, pointf3*  *facenormplace,const pointf3* p,const pointf3* r,const pointf3* s);
void remcolltree(int uid);
#define MAXCOLLTREES 4 // debug 4, release 40, errorexit's out if too many (FIX)
extern struct colltree l2r_colltrees[MAXCOLLTREES];
extern int l2r_ncolltrees;
}

using namespace n_line2road;
