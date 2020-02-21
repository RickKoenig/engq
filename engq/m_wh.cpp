#define INCLUDE_WINDOWS
#define D2_3D
#include <m_eng.h>
#include "m_perf.h"
#include "d2_font.h"
#include "d2_software_font.h"
//#include "resource.h"
#define ICON_RES 101 // check

#define NUMUSERMESSAGES 2500
static void (*usermessfuncs[NUMUSERMESSAGES])(U32 messnum,U32 WParam,S32 Lparam); // user message callbacks

//float wi::defaultfps = 30.0f;

void argcvexit()
{
//	freescript(wininfo.argv,wininfo.argc);
//	wininfo.argc=0;
//	wininfo.argv=0;
	delete wininfo.args;
	wininfo.args=0;
}

//static S32 activewindow=1;
// drag and drop
static BOOL engine1ondropfiles(HWND hwnd,HDROP hDrop)
{
	U32 nfiles,i,buffsize;
	wininfo.justdropped=1;
	argcvexit();
	wininfo.args=new script;
	nfiles=DragQueryFile(hDrop, 0xFFFFFFFF,0,0);
//	nfiles=min(MAXARGS,DragQueryFile(hDrop, 0xFFFFFFFF,0,0));
    for (i=0;i<nfiles;i++) {
		buffsize=1+DragQueryFile(hDrop,i,0,0);
		C8* buff=new C8[buffsize];
//		buff=(C8*)memalloc(buffsize);
		DragQueryFile(hDrop,i,buff,buffsize);
//		wininfo.argv=addscript(wininfo.argv,buff,&wininfo.argc);
		wininfo.args->addscript(buff);
//		logger("drag and drop '%s'\n",buff);
//		memfree(buff);
		delete[] buff;
	}
	SetForegroundWindow(hwnd);
	return 0;	  
}

static BOOL engine1oncreate(HWND hwnd,CREATESTRUCT FAR* lpCreateStruct)
{
//	wininfo.MainWindow=hwnd;
//	wininfo.stdpalette=stdpalette;
//	setwindowpalette(wininfo.stdpalette);
//	MakeBitmaps(BPP);
//	InvalidateRect(hwnd,0,TRUE);
	return 1;
}

// destroy
static void engine1ondestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

static void engine1onclose(HWND hwnd)
{
	wininfo.closerequested=1;
}

static void messonactivateapp(HWND hwnd,U32 activate,DWORD threadid)
{
//	logger("in activate with %d\n",activate);
//	if ((video_maindriver==VIDEO_DDRAW || video_maindriver==VIDEO_D3D) && activate && !activewindow)
//		fixupddrawwindow(); // after windows messes up my window position, i'll set it to where
							// i need it for the mouse
	wininfo.activewindow=activate;
}

static LRESULT CALLBACK winmess(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	S32 o;
// user messages
	if (Message>=WM_USER && Message<WM_USER+NUMUSERMESSAGES) {
		o=Message-WM_USER;
		if (usermessfuncs[o])
			(*usermessfuncs[o])(Message,wParam,lParam);
		return 0;
	} else
		switch(Message) {
// window messages
		HANDLE_MSG(hwnd,WM_CREATE,engine1oncreate);
		HANDLE_MSG(hwnd,WM_DESTROY,engine1ondestroy);
//		HANDLE_MSG(hwnd,WM_PAINT,video_paintwindow);
		case WM_PAINT:
			video_paintwindow((U32)(hwnd));
			return 0;
		HANDLE_MSG(hwnd,WM_CLOSE,engine1onclose);
		HANDLE_MSG(hwnd,WM_DROPFILES,engine1ondropfiles);
		HANDLE_MSG(hwnd,WM_ACTIVATEAPP,messonactivateapp);
// input messages
		HANDLE_MSG(hwnd,WM_KEYDOWN,messonkeydown);
		HANDLE_MSG(hwnd,WM_SYSKEYDOWN,messonkeydown);
		HANDLE_MSG(hwnd,WM_LBUTTONDOWN,messonleftmousedown);
		HANDLE_MSG(hwnd,WM_RBUTTONDOWN,messonrightmousedown);
		HANDLE_MSG(hwnd,WM_MBUTTONDOWN,messonmiddlemousedown);
		HANDLE_MSG(hwnd,WM_LBUTTONUP,messonleftmouseup);
		HANDLE_MSG(hwnd,WM_RBUTTONUP,messonrightmouseup);
		HANDLE_MSG(hwnd,WM_MBUTTONUP,messonmiddlemouseup); 
		HANDLE_MSG(hwnd,WM_MOUSEMOVE,messonmousemove); 
		case WM_MOUSEWHEEL:
			messonmousewheel(((S32)(wParam))>>16);
			return 0;
			break;
		default:
			return DefWindowProc(hwnd,Message,wParam,lParam);
		}
}

void setusermessage(U32 offs,void (*func)(U32 messnum,U32 wParam, S32 lParam))
{
	offs-=WM_USER;
	if (offs>=NUMUSERMESSAGES)
		errorexit("setusermessage %d too big max is %d",offs,NUMUSERMESSAGES-1);
	usermessfuncs[offs]=func;
}

void checkmessages()
{
	MSG Msg;
	U32 ret;
	perf_start(CHECKMESS);
	wininfo.lmx=wininfo.mx;
	wininfo.lmy=wininfo.my;
	wininfo.lmz=wininfo.mz;
	wininfo.lmbut=wininfo.mbut;
	resetinput();
	wininfo.justdropped=0;
	while(1) {
		if (wininfo.activewindow || wininfo.runinbackground || wininfo.closerequested || wininfo.sleeper) {
			ret=PeekMessage(&Msg, 0, 0, 0, PM_NOREMOVE);
			if (!ret)
				break;
		}
		if (!GetMessage(&Msg, 0, 0, 0)) {
			break;
		} else {
			// logger("msg: message %08x, wparam %08x, lparam %08x\n",Msg.message,Msg.wParam,Msg.lParam);
			wininfo.mess = Msg.message;
			++wininfo.messcnt;
		}
		DispatchMessage(&Msg);
		wininfo.sleeper=100; // this allows background programs to be alive for a little while. (good for web servers)
	}
	if (wininfo.sleeper)
		wininfo.sleeper--;
	getsomeinput();
	perf_end(CHECKMESS);
}


U32 wininit()
{
	// register
	S32 wsx,wsy,ww,wh;
	WNDCLASS wc;
	WX=GX;
	WY=GY;
//	BPP=0;
	memset(&wc,0,sizeof(wc));
	wc.style         = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc   = winmess;
	wc.hInstance     = (HINSTANCE)wininfo.hinst;
//	wc.hIcon         = LoadIcon(0,IDI_APPLICATION);
	wc.hIcon         = LoadIcon((HINSTANCE)wc.hInstance,MAKEINTRESOURCE(ICON_RES));
	wc.hCursor       = LoadCursor(0,IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpszMenuName  = 0;
	wc.lpszClassName = winmain_version;
	if (!RegisterClass(&wc))
		return 0;
// create
	wsx=0;wsy=0;ww=WX,wh=WY;
//	figureoutwindowpos(&wsx,&wsy,&ww,&wh);
	figureoutwindowposxywh(videoinfo.video_fullscreen,&wsx,&wsy,&ww,&wh);
//	figureoutwindowpos(videoinfo.video_fullscreen);
	wininfo.activewindow=1;
	wininfo.MainWindow = (U32)CreateWindowEx(
		WS_EX_ACCEPTFILES,	// extended window style
		winmain_version,	// address of registered class name
		winmain_version,	// address of window name
		WS_CAPTION | WS_POPUPWINDOW|WS_MINIMIZEBOX|WS_VISIBLE, //:	// standard
//		WS_CAPTION | WS_POPUPWINDOW|WS_MINIMIZEBOX, //:	// standard, invisible
//		WS_POPUP, //:	// standard
		wsx,	// horizontal position of window
		wsy, // vertical position of window
		ww,	// width
		wh,	// height
		0,	// handle of parent or owner window
		0,	// handle of menu, or child-window identifier
		(HINSTANCE)wininfo.hinst,	// handle of application instance
		0 	// address of window-creation data
	);	
//	if (hwindow)
//		UpdateWindow(hwindow);
	SX=GetSystemMetrics(SM_CXSCREEN);
	SY=GetSystemMetrics(SM_CYSCREEN);
	videoinfo.oscursor=1;
	videoinfo.showcursor=1;
	return 1;
}

//static S32 oscur=1,showcur=1;

void showcursor(S32 show)
{
	videoinfo.showcursor=show;
}

void useoscursor(S32 useos)
{
	videoinfo.oscursor=useos;
}
#if 0
class runavg {
	U32 runavgmaxsize;
	U32 sumavg;
	list<U32> avgtab;
public:
	runavg(U32 n) : runavgmaxsize(n),sumavg(0) {}
	U32 run(U32 in) // can't just get a value without submitting one (avoids divide by zero)
	{
		// return 0; 
		// somehow this almost leaks memory, accumulates but frees all at the end, fine in gcc
		sumavg += in;
		U32 numavg = avgtab.size();
		if (numavg == runavgmaxsize) {
			U32 f=avgtab.front();
			sumavg -= f;
			avgtab.pop_front();
			--numavg;
		}
		avgtab.push_back(in);
		++numavg;
		return (sumavg+(numavg>>1))/numavg;
	}
	void reset()
	{
		avgtab.clear();
		sumavg = 0;
	}
};
#else
runavg::runavg(U32 n) : maxnumavg(n),numavg(0),sumavg(0),avgindex(0)
{ 
	avgtab = new U32[n];
	fill(avgtab,avgtab+n,0);
	//memset(avgtab,0,sizeof(U32)*n);
}
runavg::~runavg()
{
	delete[] avgtab;
}
U32 runavg::run(U32 in)
{
	sumavg-=avgtab[avgindex];
	avgtab[avgindex]=in;
	sumavg+=in;
	++avgindex;
	if (avgindex>=maxnumavg)
		avgindex=0;
	if (numavg < maxnumavg)
		++numavg;
	return (sumavg+(numavg>>1))/numavg;
}
void runavg::reset()
{
	fill(avgtab,avgtab+maxnumavg,0);
	sumavg = 0;
	numavg = 0;
}
#endif	
static runavg* msa,*msa2,*sla; // running avgerages
// call these 2 from winmain at the right time
void avginit()
{
	msa=new runavg(1024);
	msa2=new runavg(256);
	sla=new runavg(64);
}

void avgfree()
{
//	runavg_free(msa);
	delete msa;
	delete msa2;
	delete sla;
}	
void avgreset()
{
	msa->reset();
	msa2->reset();
	sla->reset();
}	
/*
struct avgstruct* runavg_alloc(U32 nsamp)
{
	struct avgstruct* ret;
	ret=(struct avgstruct*)memzalloc(sizeof(struct avgstruct));
	ret->avgtab=(U32*)memzalloc(sizeof(ret->avgtab[0])*nsamp);
	ret->numavg=nsamp;
	return ret;
}

void runavg_free(struct avgstruct* a)
{
	memfree(a->avgtab);
	memfree(a);
}

U32 runavg_run(struct avgstruct* as,U32 v)
{
	as->sumavg-=as->avgtab[as->avgindex];
	as->avgtab[as->avgindex]=v;
	as->sumavg+=v;
	++(as->avgindex);
	if (as->avgindex>=as->numavg)
		as->avgindex=0;
	return (as->sumavg+(as->numavg>>1))/as->numavg;
}
*/
void waitframe()
{
	static bool firsttime=true;
//	static S32 lasttime,lasttime2;
	S32 curtime2; // time after proc
	S32 curtime; // time after proc and wait
	S32 timetowait;
	S32 timetowait2;
	S32 microsecwanted;
	U32 bail=0;
	S32 slp;
	perf_start(WAITFRAME);
	curtime=curtime2=getmicrosec();
	if (firsttime) {
		wininfo.lasttime=wininfo.lasttime2=curtime2-1;
		firsttime=false;
	}
	wininfo.microseccurrent2=curtime2-wininfo.lasttime;
	if (wininfo.microseccurrent2>1000000) {
//		wininfo.microseccurrent2=1000000;
	} else {
		wininfo.fpscurrent2=1000000.0f/wininfo.microseccurrent2;
		wininfo.microsecavg2=msa2->run(wininfo.microseccurrent2);
		wininfo.fpsavg2=1000000.0f/wininfo.microsecavg2;
	}
	if (wininfo.fpswanted<=0)
		microsecwanted=0;
	else
		microsecwanted=1000000/wininfo.fpswanted;
	timetowait2=microsecwanted-wininfo.microseccurrent2;
	timetowait=microsecwanted-(curtime2-wininfo.lasttime2);
//	logger("timetowait = %d\n",timetowait);
	if (timetowait2<0)
		timetowait2=0;
	wininfo.sleepernum=sla->run(timetowait2);
	slp=wininfo.sleepernum/1000-1;
	//slp=wininfo.sleepernum/1000-50;
	if (wininfo.nosleep)
		slp=0;
	slp -= 8; // in milli seconds, less sleep, try for smoothness
	if (slp>0) {
//		logger("sleep %d\n",slp);
		Sleep(slp);
	}
	if (timetowait>0) {
		do {
			++bail;
			if (bail>4000000000U) { // in case timer is flakey.. too big?
				logger("waitframe: bailed!\n"); // emergency..
				break;
			}
			curtime=getmicrosec();
		} while(curtime-wininfo.lasttime<microsecwanted);
	}
//	logger("bal = %d\n",bail);
	wininfo.microseccurrent=curtime-wininfo.lasttime;
	if (wininfo.microseccurrent>1000000) {
//		wininfo.microseccurrent=1000000;
	} else {
		wininfo.framestep=wininfo.microseccurrent*(INITFPS/1000000.0f); // for animation
		wininfo.framestep=range(0.0f,wininfo.framestep,10.0f);
		wininfo.fpscurrent=1000000.0f/wininfo.microseccurrent;
		wininfo.microsecavg=msa->run(wininfo.microseccurrent);
		wininfo.fpsavg=1000000.0f/wininfo.microsecavg;
	}
	wininfo.lasttime=curtime;
	wininfo.lasttime2+=microsecwanted;
	wininfo.slacktime=wininfo.lasttime-wininfo.lasttime2;
	const S32 maxslacktime = 250000;//1000000;
	if (wininfo.slacktime>maxslacktime)
		wininfo.lasttime2=wininfo.lasttime-maxslacktime;
	perf_end(WAITFRAME);
}

static void drawcursor()
{
/*
//	extern S32 lastmousemode;
	perf_start(DRAWCURSOR);
	if ((!videoinfo.oscur || wininfo.mousemode) && videoinfo.showcur)
		if (videoinfo.video_maindriver>=0) {
			video_lock();
			clipline32(B32,MX-8,MY,MX-2,MY,C32WHITE); // assuming palette isn't screwed up
			clipline32(B32,MX+2,MY,MX+8,MY,C32WHITE);
			clipline32(B32,MX,MY-8,MX,MY-2,C32WHITE);
			clipline32(B32,MX,MY+2,MX,MY+8,C32WHITE);
			clipline32(B32,MX-8,MY-1,MX-2,MY-1,C32BLACK); // assuming palette isn't screwed up
			clipline32(B32,MX+2,MY+1,MX+8,MY+1,C32BLACK);
			clipline32(B32,MX+1,MY-8,MX+1,MY-2,C32BLACK);
			clipline32(B32,MX-1,MY+2,MX-1,MY+8,C32BLACK);
			video_unlock();
		}
	if (videoinfo.oscur && videoinfo.showcur && !wininfo.mousemode && !wininfo.lastmousemode)
		while(ShowCursor(1)<0)
			;
	else
		while(ShowCursor(0)>=0)
			;
	perf_end(DRAWCURSOR);
*/
	perf_start(DRAWCURSOR);
	if ((!videoinfo.oscursor || wininfo.mousemode) && videoinfo.showcursor)
		if (videoinfo.video_maindriver>=0) {
			drawtextque_string(SMALLFONT,MX-9+1,MY-5+1,F32BLACK,"><");
			drawtextque_string(SMALLFONT,MX-8+1,MY-4+1,F32WHITE,"><");
		}
	if (videoinfo.oscursor && videoinfo.showcursor && !wininfo.mousemode && !wininfo.lastmousemode)
		while(ShowCursor(1)<0)
			;
	else
		while(ShowCursor(0)>=0)
			;
	perf_end(DRAWCURSOR);
//	videoinfo.didcursor=true;
}


#if 0
void maininit()
{
//	rungametheory();
//	testasmmacros();
	changestate(wininfo.startstate);
}

U32 mainproc()
{
	return stateproc();
}

void mainexit()
{
}
#endif

/*
void winproc()
{
	drawdebprint();
	perf_start(WINPROC);
// incase nothing drew the font (2d/3d), do it in 2d
	if (fntlist.size()) {
		video_lock();
		software_drawtextque_do(); // draw que
		video_unlock(); 
	}
	if (!videoinfo.didcursor)
		drawcursor();
	donelights();
	videoinfo.didbuildworldmats=false;
	videoinfo.didcursor=false;
	audio_babysit();
	video_paintwindow(0);
	waitframe();
	checkmessages();
	perf_end(WINPROC);
}
*/
void winproc()
{
	perf_start(WINPROC);
	drawdebprint(); // queued up into text
	drawcursor(); // queued up into text
	if (mainhasdraw3d()) {
		video_beginscene();
		maindraw3d();
		video_endscene(!mainhasdraw2d()); // hardware (dx9) versions will dump text here using ortho if no 2d
	}
	if (mainhasdraw2d()) {
		video_lock();
		if (B32->data)
			maindraw2d();
		//video_unlock();
	}
// incase nothing drew the text in 3d, dump it using 2d blits
	if (fntlist.size()) {
		video_lock();
		software_drawtextque_do(); // draw que
		//cliprect32(B32,0,0,100,100,C32GREEN);
		//video_unlock(); 
	}
	video_unlock();
	audio_babysit();
	video_paintwindow(0);
	waitframe();
	checkmessages();
	debprocesskey();
	perf_end(WINPROC);
}

// memory manager is not available
void winexit()
{
//	freebitmaps(BPP); // winexit
//	video_uninit();
//	argcvexit();
	DestroyWindow((HWND)wininfo.MainWindow);
}

#if 0
// for reference
// this is still being thought out, it seems to work okay for now...
// I want wininfo.fpsavg to read closer to wininfo.fpswanted
// maybe split up proc into proc and draw
// and have a maximum for the 'slack'
static void waitframe()
{
	U32 bail=0;
	S32 timetowait=0,curtime,curtime2,slacktime;
	U32 microsecwanted;
	static U32 firsttime=1;
	static S32 lasttime;
	perf_start(WAITFRAME);
	if (firsttime) {
		lasttime=getmicrosec();
		firsttime=0;
		slacktime=0;
	}
	if (wininfo.fpswanted<=0)
		microsecwanted=0;
	else
		microsecwanted=1000000/wininfo.fpswanted;
	curtime=curtime2=getmicrosec();
	wininfo.microseccurrent2=curtime2-lasttime;
	if (wininfo.microseccurrent2>1000000)
		wininfo.microseccurrent2=1000000;
	wininfo.fpscurrent2=1000000.0f/wininfo.microseccurrent2;
	wininfo.microsecavg2=runavg_run(msa2,wininfo.microseccurrent2);
	wininfo.fpsavg2=1000000.0f/wininfo.microsecavg2;
	timetowait=microsecwanted-wininfo.microseccurrent2;
	if (timetowait<0)
		timetowait=0;
//	logger("timetowait = %d\n",timetowait);
	wininfo.sleepernum=runavg_run(sla,timetowait);
	if (wininfo.sleepernum>0) { // kill some time
		S32 slp=wininfo.sleepernum/1000-1;
		if (slp>0)
			Sleep(slp);
		do {
			++bail;
			if (bail>4000000000U) { // in case timer is flakey.. too big?
				logger("bailed\n"); // emergency..
				break;
			}
			curtime=getmicrosec();
		} while(curtime-lasttime<microsecwanted);
	} 
	wininfo.microseccurrent=curtime-lasttime;
//	logger("msc %d\n",wininfo.microseccurrent);
	if (wininfo.microseccurrent>1000000)
		wininfo.microseccurrent=1000000;
	wininfo.fpscurrent=1000000.0f/wininfo.microseccurrent;
	wininfo.microsecavg=runavg_run(msa,wininfo.microseccurrent);
	wininfo.fpsavg=1000000.0f/wininfo.microsecavg;
//	lasttime=curtime;
	lasttime=lasttime+microsecwanted;
	perf_end(WAITFRAME);
}
#endif

