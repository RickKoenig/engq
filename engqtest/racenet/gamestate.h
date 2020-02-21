//#define CHECKGSTATE
//#define SHOWGLOBALS
void addgstate();
void initgstate();
void exitgstate();
#ifdef CHECKGSTATE
void checkgstate(int frm,U8 *str);
void loggamestate(char *fmt,...);
#else
#define checkgstate 
#define loggamestate 
#endif
