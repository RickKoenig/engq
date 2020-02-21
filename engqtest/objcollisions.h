// bbox code
void init3dbboxes(int nboxes);
void collide3dboxes();

#define MAX3DBOX 100

struct bbox3d {
	VEC b,e;
};

struct colpair3d {
	int a,b;	// a < b
};

extern struct bbox3d bboxs3d[];
extern struct colpair3d colpairs3d[];
extern int ncolpairs;