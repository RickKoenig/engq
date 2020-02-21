void checkw(struct phyobject *p);

float meshplane(struct phyobject *p,VEC *ploc,VEC *pnorm,VEC *cloc);
float meshmesh(struct phyobject *pa,struct phyobject *pb,struct contact *,VEC *onorm,VEC *cloc);

#define MAXF 1000
#define MAXV 1000

void point2face(VEC *face,VEC *pnt,VEC *result);
void edge2edge(VEC *edge0,VEC *edge1,VEC *result0,VEC *result1);
void point2edge(VEC *edge,VEC *pnt,VEC *result);

extern VEC testsvec;
extern int testiter;
