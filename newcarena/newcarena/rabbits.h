void initrabbits();
void dorabbits(U32 slot);
void checkendgamerabbits(); // finished / game over scenarios

// for studyforks

extern S32 nextlocsrot[PCE_NPIECETYPE][4][2];
const int BADDIR = 1000000; // not -1 anymore
S32 pointi22rot(const pointi2& p); // point2dir
pointi2 rot2pointi2(S32 rot); // dir2point
S32 rotmod(S32 a,S32 b); // +b piece2world, -b world2piece
