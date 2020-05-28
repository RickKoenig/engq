namespace st2 {
	typedef mater2 MAT;
	typedef modelb model;
	typedef mat4 JXFORM;
	TREE* loadlws(const char* name);
	TREE* findtreename(TREE* parent, const char* name);
	TREE* alloctree(int maxchildren, const char *modelname);
}

#define addnull(p,rotq)
#define resetnulls()
#define initnulls(root,nullsize) 

#define addvec(start,disp,color)
#define addline(start,end,color)
#define resetvecs()
#define initvecs(root) 

#define initplanes(root,nullsize)
#define resetplanes()
#define addplane(p,rotq)

//#define inittris(root)
//#define resettris()
//#define addtri(v)
//#define unhooktree(t) t->unlink()

