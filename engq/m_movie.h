//////// movie system
//void movieinit(char *fname,struct tex* mt,int* fps); // pass in a 'dynamic' texture
int movieinit(char *fname,S32& wid,S32& hit); // returns frame rate of the movie
bitmap32* moviegetframe(); // returns a reference to current movie frame, do not free
//void moviedrawframe(int x,int y,int xs,int ys);
void movieexit();
