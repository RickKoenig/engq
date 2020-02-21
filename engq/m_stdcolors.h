enum CI {CI_BLACK,CI_BLUE,CI_GREEN,CI_CYAN,
	CI_RED,CI_MAGENTA,CI_BROWN,CI_LIGHTGRAY,
	CI_DARKGRAY,CI_LIGHTBLUE,CI_LIGHTGREEN,CI_LIGHTCYAN,
	CI_LIGHTRED,CI_LIGHTMAGENTA,CI_YELLOW,CI_WHITE,
	CI_NUM};
extern C32 C32stdcolors[CI_NUM];

#define C32BLACK C32(0,0,0)
#define C32BLUE C32(0,0,170)
#define C32GREEN C32(0,170,0)
#define C32CYAN C32(0,170,170)
	
#define C32RED C32(170,0,0)
#define C32MAGENTA C32(170,0,170)
#define C32BROWN C32(170,85,0)
#define C32LIGHTGRAY C32(170,170,170)
	
#define C32DARKGRAY	C32(85,85,85)
#define C32LIGHTBLUE C32(85,85,255)
#define C32LIGHTGREEN C32(85,255,85)
#define C32LIGHTCYAN C32(85,255,255)
	
#define C32LIGHTRED C32(255,85,85)
#define C32LIGHTMAGENTA	C32(255,85,255)
#define C32YELLOW C32(255,255,85)
#define C32WHITE C32(255,255,255)

extern C8* stdcolornames[CI_NUM];
S32 findstdcoloridx(const C8* name);

struct bitmap32* convert8to32(struct bitmap8* b8,C32* dacs);
void convert8to32(struct bitmap8* b8,C32* dacs,bitmap32* b32);
extern pointf3 F32stdcolors[CI_NUM];
#define F32BLACK F32stdcolors[CI_BLACK]
#define F32BLUE F32stdcolors[CI_BLUE]
#define F32GREEN F32stdcolors[CI_GREEN]
#define F32CYAN F32stdcolors[CI_CYAN]
#define F32RED F32stdcolors[CI_RED]
#define F32MAGENTA F32stdcolors[CI_MAGENTA]
#define F32BROWN F32stdcolors[CI_BROWN]
#define F32LIGHTGRAY F32stdcolors[CI_LIGHTGRAY]
#define F32DARKGRAY F32stdcolors[CI_DARKGRAY]
#define F32LIGHTBLUE F32stdcolors[CI_LIGHTBLUE]
#define F32LIGHTGREEN F32stdcolors[CI_LIGHTGREEN]
#define F32LIGHTCYAN F32stdcolors[CI_LIGHTCYAN]
#define F32LIGHTRED F32stdcolors[CI_LIGHTRED]
#define F32LIGHTMAGENTA F32stdcolors[CI_LIGHTMAGENTA]
#define F32YELLOW F32stdcolors[CI_YELLOW]
#define F32WHITE F32stdcolors[CI_WHITE]

struct cki {
	S32 usecolorkey; // this applies to load,convert8to32 and 3d stuff
	S32 lasthascolorkey;
};
extern cki colorkeyinfo;

C32 pointf3toC32(const  pointf3* fc);
pointf3 C32topointf3(C32 c,float mul = 1.0f);
