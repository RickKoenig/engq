// keyboard
void flushkeys();
// mouse
void setmousexy(S32,S32);
enum {MM_NORMAL,MM_INFINITE};
void resetmhold();
void input_init(S32 usedinput);
void input_uninit();


// INTERNAL include file, winmain uses
void messonkeydown(HWND hwnd, UINT vk, BOOL fDown, S32 cRepeat, UINT flags);
void messonleftmousedown(HWND hwnd, BOOL fDoubleClick, S32 x, S32 y, UINT keyFlags);
void messonrightmousedown(HWND hwnd, BOOL fDoubleClick, S32 x, S32 y, UINT keyFlags);
void messonmiddlemousedown(HWND hwnd, BOOL fDoubleClick, S32 x, S32 y, UINT keyFlags);
void messonleftmouseup(HWND hwnd, S32 x, S32 y, UINT keyFlags);
void messonrightmouseup(HWND hwnd, S32 x, S32 y, UINT keyFlags);
void messonmiddlemouseup(HWND hwnd, S32 x, S32 y, UINT keyFlags);
void messonmousemove(HWND hwnd, S32 x, S32 y, UINT keyFlags);
void messonmousewheel(S32 delta);
void resetinput();
void getsomeinput();
void getdikdata(U8* dikbuff);
void getdimdata();

// joystick
#define MAXJOYSTICKS 2
#define MAXAXIS 4 // keep at 4
#define MAXJOYBITSRAW 16
#define MAXJOYBITS 16 // add 4 more directions as buttons
struct joydata {
	U32 newsjoy,newsjoyl,newsjoyd;
	S32 newsjoyaxis[MAXAXIS],newsjoyaxisl[MAXAXIS],newsjoyaxisd[MAXAXIS];
	U32 newsjoyrep;
	U32 repcounters[MAXJOYBITS];

};
extern struct joydata jdata[MAXJOYSTICKS];

#define NSJ_BUT1  1
#define NSJ_BUT2  2
#define NSJ_BUT3  4
#define NSJ_BUT4  8

#define NSJ_BUT5  0x10
#define NSJ_BUT6  0x20
#define NSJ_BUT7  0x40
#define NSJ_BUT8  0x80

#define NSJ_BUT9  0x100
#define NSJ_BUT10 0x200
#define NSJ_BUT11 0x400
#define NSJ_BUT12 0x800

#define NSJ_BUT13 0x1000
#define NSJ_BUT14 0x2000
#define NSJ_BUT15 0x4000
#define NSJ_BUT16 0x8000

#define NSJ_UP    0x10000
#define NSJ_DOWN  0x20000
#define NSJ_RIGHT 0x40000
#define NSJ_LEFT  0x80000
