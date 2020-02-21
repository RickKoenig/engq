namespace physics3d {
#define MAXBOX2BOX 72
#define MAXBOX2PLANE 12
//#define NCORNERS 8

////////// handy utils
float newellnormal(VEC *pnts,int npnts,VEC *norm);
// given a point and norm (1 unbounded plane), and a pnt and dir (line) return t and i
int line2plane(VEC *pln0,VEC *plnnorm,VEC *lin0,VEC *lin1,VEC *intsect);
// v0 v1 v2 and v0+(v1-v0)+(v2-v0)
int line2bplane(VEC *v0i, VEC *v1i, VEC *v2i, VEC *top,VEC *bot,VEC *intsect);
// v0 v1 v2 and v0+(v1-v0)+(v2-v0)
int line2btri(VEC *v0, VEC *v1, VEC *v2, VEC *top,VEC *bot,VEC *intsect);
int line2btricull(VEC *v0, VEC *v1, VEC *v2, VEC *top,VEC *bot,VEC *intsect);
// given 2 sets of 3 points (2 parellelgram planes), return 2 points that define line intersection
int plane2plane(VEC *pl0,VEC *pl1,VEC *res);
// given 1 sets of 3 points (2 parellelgram planes) and point normal, return 2 points that define line intersection
int plane2infplane(VEC *pl0,VEC *pl1,VEC *norm1,VEC *res);
// given 2 sets of 3 points (2 triangles), return 2 points that define line intersection
int tri2tri(VEC *tr0,VEC *tr1,VEC *res);
int box2box(VEC *b0,VEC *b1,VEC *res);
int box2plane(VEC *b,VEC *planep,VEC *planenorm,VEC *res);

}
