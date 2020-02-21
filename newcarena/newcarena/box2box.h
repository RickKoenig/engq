// box 2 box
int box2box(pointf3* b0,pointf3* b1,pointf3* res);
int getcolpoint(pointf3* pnts,int npnts,pointf3* cm,pointf3* loc,pointf3* norm);

#define MAXBOX2BOX 72
#define NCORNERS 8

// line to box
int line2box(pointf3* objpos,pointf3* objrot,pointf3* oabbmin,pointf3* oabbmax,pointf3* p0,pointf3* p1,pointf3* intsect);
int box2box_v2(pointf3* objpos0,pointf3* objrot0,pointf3* oabbmin0,pointf3* oabbmax0,
			   pointf3* objpos1,pointf3* objrot1,pointf3* oabbmin1,pointf3* oabbmax1,
			   pointf3* loc,pointf3* norm);

