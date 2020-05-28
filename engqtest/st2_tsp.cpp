#include <m_eng.h>
#include "st2_glue.h"
namespace st2 {
#if 0
#define NUMTEXFORMATS 6
#define TEX555NCK 0
#define TEX565NCK 1
#define TEX5551A 2
#define TEX4444A 3
	// later
#define TEX555CK 4
#define TEX565CK 5

// clipmap variable
#define CLIPMAP_NONE 0
#define CLIPMAP_COLORKEY 1
#define CLIPMAP_1BITALPHA 2
#define CLIPMAP_1AND4BITALPHA 3

#define MAXMIPMAPS 16
	struct texture {
		struct bitmap16 *texdata;
		//	int tmaskx,tmasky;
		char name[NAMESIZE];
		int logu, logv;
		//	int format;
		int refcount;
		int index;
		int islocked;
		int texformat;
		void *tdsurftex7a[MAXMIPMAPS]; //LPDIRECTDRAWSURFACE7 tdsurftex7;
		int nmipmap;
	};

	//extern int globaltexturemode;
	//void set1texturemode(int mode);
	struct texture *loadfindtexture(char *fname, char *alphafname, int mipmap);
	struct texture *buildtexture(char *texname, int x, int y, int texformat);
	void freetexture(struct texture *t);

#define MAXTEXTURES 1000
	extern struct texture *texlist[];
	extern int ntextures;

	struct bitmap16 *locktexture(struct texture *);
	void unlocktexture(struct texture *);
	struct bitmap16 *locktexturei(struct texture *, int i);
	void unlocktexturei(struct texture *, int i);

	extern int texavail[NUMTEXFORMATS];
	extern int textmannegalpha;

	struct tsp {
		int nframe;
		struct bitmap16 **frames;
		int texformat;
		int x, y;
		int curframe, lastframe; // user controlled, inited to 0,-1
		struct texture *atex;	// usercontrolled, inited to null
	};


	struct tsp *loadtsp(char *name, char *alphaname,
		struct tree *roottree, char *treename, char *matname,
		int useformat, int keepoldsize);
	struct tsp *loadtspo(char *name, char *alphaname, int texformat, int checktexsize);
	struct tsp *loadtspscript(char *name, char *alphaname, int texformat);
	void freetsp(struct tsp *t);
	void tspanimtex(struct tsp *t, int offx, int offy, int xpar);
	void tspsetframe(struct tsp *t, int frame); // gets set when tspanimtex gets called
	struct tsp *duptsp(struct tsp *);
	void initifls(struct model *mod, struct mat *m);

	// some globals
	extern int clipmap;
	extern int video_mipmap, mipmapwanted;


#endif
struct tsp *loadtsp(char *name, char *alphaname,
	TREE *roottree, char *treename, char *matname,
	int useformat, int keepoldsize)
{
	return nullptr;
}

void freetsp(struct tsp *t)
{

}

void tspanimtex(struct tsp *t, int offx, int offy, int xpar)
{

}

void tspsetframe(struct tsp *t, int frame) // gets set when tspanimtex gets called
{

}

}
