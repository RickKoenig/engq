void pushdir();
U32 setdir(const C8* s); // returns true if setdir succeeded
U32 pushandsetdir(const C8* s);
U32 popdir();
const C8* getcurdir();

U32 setdirdown(const C8* s);
U32 setdirup();
U32 pushandsetdirdown(const C8* s);
U32 pushandsetdirup();

void dirinit();
void direxit();

//extern C8 exepath[];
void getexepath();

string reldir2absdir(string rd);
string absdir2reldir(string ad);
string absdir2reldirdisplay(string ad); // just for display
