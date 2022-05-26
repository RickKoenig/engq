U32 wininit();
void winproc();
void winexit();

//const float defaultfps = 30.0f;

/*struct avgstruct {
	U32 numavg;
	U32 sumavg;
	U32* avgtab;
	U32 avgindex;
};

struct avgstruct* runavg_alloc(U32 nsamples);
void runavg_free(struct avgstruct* a);
U32 runavg_run(struct avgstruct* as,U32 v);
*/
void avgfree();
void avginit();
void avgreset(); // after a long load, it's good to start with a fresh fps/waitframe system

class runavg {
	U32 maxnumavg;
	U32 numavg;
	U32 sumavg;
	U32 avgindex;
	U32* avgtab;
public:
	runavg(U32 n);
	~runavg();
	U32 run(U32 in);
	void reset();
};

void showcursor(S32 show);
// internal
void useoscursor(S32 useos);
void argcvexit();

void checkmessages();
void setusermessage(U32 offs,void (*func)(U32 messnum,U32 wParam, S32 lParam));
void waitframe();

void changeWindowTitle(C8* str);

