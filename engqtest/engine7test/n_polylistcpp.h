//#define MAXPOLYS 180
//#define MAXVERTICES 120
#define ALLOCSIZE 16
enum ptype {PT_POLY,PT_HOLE};
struct poly {
	int ptype; // polytype
//	int nverts;
	int matid;
//	pointf3 *verts;
//	uv *uvs;
//	VEC verts[MAXVERTICES];
//	uv uvs[MAXVERTICES];
	vector<pointf3> verts;
	vector<uv> uvs;
//	poly::poly(){}
//	poly::poly(const poly& p){}
};

struct polylist {
	bool hasuvs;
//	int npolys;
//	poly *polys;
//	poly polys[MAXPOLYS];
	vector<poly> polys;
	polylist() { hasuvs=false; }
};

void setm1tobackgnd(polylist *c);
void addlinearuvspolylist(polylist *pl,float offx,float offz,float x2u,float z2v);
void zeroypolylist(polylist *c);
void rotpolylist(polylist *c,float rot);
void offsetpolylist(polylist *c,pointf3 *off);
void backypolylist(polylist *c);
polylist *copypolylist(polylist *src);

//void boolabintsectm1(polylist *pla,VEC *ev,edge *e,int ne,polylist *plc);
polylist *convertfaceverts2polylist(facevert *fv);
polylist *convertfaceverts2polylisto(facevert *tfv);
//void convertfaceverts2polylisto(VEC *v,uv *uvs,int nv,face *f,int nf,polylist *c);
tree2* buildapolylist(polylist *p,char *modelname,char *texname,char *texname2=0);
//void aminusb(polylist *al,polylist *bl,polylist *cl);
//void intersectionab(polylist *al,polylist *bl,polylist *cl);
polylist *boolabintsectm1(const polylist* plt,const vector<pointf3>& verts,const vector<edge>& edges);//,int nedge); // special
enum boolop {OP_INTERSECTION,OP_MINUS};
polylist *boolab(polylist *plb,polylist *plto,boolop op);
//void boolab(polylist *al,polylist *bl,polylist *cl,boolop op);

int inside(poly *pal,pointf3 *p);
//void freepolylist(polylist *plb);
