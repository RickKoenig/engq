#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include "m_vid_gdi.h"
#include "m_vid_ddraw.h"
//#include <d3dx9.h>
#include <d3d9.h>
#include "m_vid_dx9.h"

static C8* supported_devices[VIDEO_NSUPPORTED_DEVICES]={
	"gdi",
	"ddraw fullscn",
	"d3d windowed",
	"d3d fullscn",
};

// ahhhh 3 globals..., should just drop into videoinfo maybe
bitmap32 *lock32=&dummy32;
static U32 islocked;
bitmap32 dummy32={
	{0,0},
	{{0,0},{0,0}},
	0,
};

vi videoinfo;

// internal
struct videoapi {
	script *(*video_getnsubdrivers)();
	void (*video_uninitfunc)();
	void (*video_paintwindowfunc)(U32 h);
	S32 (*video_setupwindowfunc)(S32 x,S32 y);//,U32 bpp);
	void (*video_lockfunc)();
	void (*video_unlockfunc)();
};

static struct videoapi videoapis[VIDEO_NSUPPORTED_DEVICES]={
// standard gdi
	{gdi_getnsubdrivers,gdi_video_uninit,gdi_paintwindow,
	gdi_setupwindow,gdi_lock,gdi_unlock},
// directdraw
	{ddraw_getnsubdrivers,ddraw_video_uninit,ddraw_paintwindow,
	ddraw_setupwindow,ddraw_lock,ddraw_unlock},
// direct3d windowed
	{dx9_getnsubdrivers,dx9_video_uninit,dx9_paintwindow,
	dx9_wnd_setupwindow,dx9_lock,dx9_unlock},
// direct3d fullscreen
	{dx9_getnsubdrivers,dx9_video_uninit,dx9_paintwindow,
	dx9_fs_setupwindow,dx9_lock,dx9_unlock},
};

void figureoutwindowposxywh(S32 fs,S32* wsx,S32* wsy,S32* ww,S32* wh)
{
//	S32 wsx,wsy,ww,wh;
	U32 pal,bp,pl;
	HDC hdc;
	S32 cxframe,cyframe,cycaption,clienttoscreenx,clienttoscreeny;
	SX=GetSystemMetrics(SM_CXSCREEN);
	SY=GetSystemMetrics(SM_CYSCREEN);
	if (!fs) {
		cxframe=GetSystemMetrics(SM_CYFIXEDFRAME);
		cyframe=GetSystemMetrics(SM_CYFIXEDFRAME);
		cycaption=GetSystemMetrics(SM_CYCAPTION);
		hdc = GetDC(0);
		if(!hdc)
			errorexit("can't find screen DC");
		pal=GetDeviceCaps(hdc,RASTERCAPS) & RC_PALETTE;
		bp=GetDeviceCaps(hdc,BITSPIXEL);
		pl=GetDeviceCaps(hdc,PLANES);
		logger("pal %d, bp %d, pl %d\n",pal,bp,pl);
		ReleaseDC(0,hdc);
		if (pal || pl>1 || bp<16)
			errorexit("set video resolution to hicolor or better");
		clienttoscreenx=(SX-WX)/2;
		clienttoscreeny=(SY-WY)/2;
		*wsx=clienttoscreenx-cxframe;
		*wsy=clienttoscreeny-cyframe-cycaption;
		*ww=WX+2*cxframe;
		*wh=WY+2*cyframe+cycaption;
	} else {
		cxframe=GetSystemMetrics(SM_CYFIXEDFRAME);
		cyframe=GetSystemMetrics(SM_CYFIXEDFRAME);
		cycaption=GetSystemMetrics(SM_CYCAPTION);
		clienttoscreenx=0;
		clienttoscreeny=0;
		*wsx=-cxframe;
		*wsy=-cyframe-cycaption;
		if (videoinfo.video_subdriver==0) {
			*ww=WX+2*cxframe;
			*wh=WY+2*cyframe+cycaption;
		} else {
			*ww=SX+2*cxframe;
			*wh=SY+2*cyframe+cycaption;
		}
	}
}
// takes xres,yres and figures out where the window goes on the screen
// by setting see below
void figureoutwindowpos(S32 fs)
//void figureoutwindowpos(S32 *wsx,S32 *wsy,U32 *ww,U32 *wh)
{
	HWND zorder;
	zorder=HWND_NOTOPMOST; // behind taskbar
//	if (WY+140>=SY && !fs)
//		zorder=HWND_TOPMOST; // in front of taskbar
//	else
	S32 wsx,wsy,ww,wh;
	figureoutwindowposxywh(fs,&wsx,&wsy,&ww,&wh);

	//HWND zorder;
	//zorder=HWND_NOTOPMOST; // behind taskbar
//	MoveWindow(wininfo.MainWindow,windowtoscreenx,windowtoscreeny,windowwidth,windowheight,1);
	logger("gdi setwindow pos %d %d %d %d\n",wsx,wsy,ww,wh);
	SetWindowPos(
		(HWND)wininfo.MainWindow,	// handle of window
		zorder,	// placement-order handle
		wsx,	// horizontal position
		wsy,	// vertical position
		ww,	// width
		wh,	// height
		0 	// window-positioning flags
		);	
}

static void showwin()
{
	static U32 wv=0;
	if (wv==0) {
//		ShowWindow((HWND)wininfo.MainWindow,SW_SHOW); // we'll show the window AFTER it's been painted upon..
		wv=1;
	}
}

void video_reload_mat2shader(const C8* fname)
{
	if (videoinfo.mat2shader)
		delete videoinfo.mat2shader;
	const C8* tweek=0;
	if (videoinfodx9.tooningame==2)
		tweek="toon2_";
	else if (videoinfodx9.tooningame)
		tweek="toon_";
	if (fname && fileexist(fname)) {
		videoinfo.mat2shader=new scriptinc(fname,tweek);
	} else {
		pushandsetdir("shaders");
		videoinfo.mat2shader=new scriptinc("mat2shader.txt",tweek);
		popdir();
	}
	S32 na=videoinfo.mat2shader->num();
	if (na%2)
		errorexit("bad mat2shader.txt");
}

void video_setupwindow(S32 x,S32 y)//,U32 bpp)
{
	S32 i;
	S32 setret=0;
	if (islocked)
		errorexit("video_setupwindow locked\n");
	if (x==0)
		x=WX;
	if (y==0)
		y=WY;

	video_reload_mat2shader();
	if (wininfo.wantedvideomaindriver==videoinfo.video_maindriver && wininfo.wantedvideosubdriver==videoinfo.video_subdriver) {
		if (x==WX && y==WY)
			return; // already set
	}
	resetmhold();
	if (videoinfo.video_maindriver>=0) {
//		bitmap32free(videoinfo.wbuffer3d);
		videoapis[videoinfo.video_maindriver].video_uninitfunc();
	}
	videoinfo.video_maindriver=-1;
	if (wininfo.wantedvideomaindriver<0)
		return;
	script* sc=video_getnmaindrivers();
	logger("video init %d.............................................\n",wininfo.wantedvideomaindriver);
	for (i=0;i<sc->num();++i) {
		if (i==wininfo.wantedvideomaindriver)
			logger("  * ");
		else
			logger("    ");
		logger("maindriver %2d \"%s\"\n",i,(*sc).printidx(i).c_str());
	}
	delete sc;
	logger("video_setupwindow x=%d, y=%d, bpp=%d\n",x,y,32);
	videoinfo.video_maindriver=wininfo.wantedvideomaindriver;
	videoinfo.video_subdriver=wininfo.wantedvideosubdriver;
	while(!setret) {
//		videoinfo.video_fullscreen=supported_devices[videoinfo.video_maindriver].fullscreen;
		setret=videoapis[videoinfo.video_maindriver].video_setupwindowfunc(x,y);//,bpp);
		if (!setret) {
			videoinfo.video_maindriver=0;
			videoinfo.video_subdriver=0;
			wininfo.wantedvideomaindriver=0;
			wininfo.wantedvideosubdriver=0;
		}
	}
//	figureoutwindowpos(supported_devices[i].fullscreen);
//	videoinfo.wbuffer3d=bitmap32alloc(WX,WY,C32BLACK); // done in d2_vid3d.cpp
	video_paintwindow(0);
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		setmousexy(WX/2,WY/2);
}

script* video_getnmaindrivers()
{
	S32 i;
	script* sc=new script;
	for (i=0;i<VIDEO_NSUPPORTED_DEVICES;++i)
		sc->addscript(supported_devices[i]);
	return sc;
}

script* video_getnsubdrivers(S32 driver)
{
	S32 nd;
	script* mdsc=video_getnmaindrivers();
	nd=mdsc->num();
	delete mdsc;
	if (driver<0 || driver>=nd)
		return new script;
	return videoapis[driver].video_getnsubdrivers();
}

void video_init(S32 maindriver,S32 subdriver)
{
#if 1
	if (!video3dinfo.sysfont) {
		pushandsetdir("common");
		video3dinfo.sysfont=gfxread32("sysfont.pcx");
		video3dinfo.sysfont2=gfxread32("font0.png");
		makeblackxpar(video3dinfo.sysfont,"sysfont"); // not necessary
		makeblackxpar(video3dinfo.sysfont2,"sysfont2");
		video3dinfo.smallfont = new softfont("sysfont.pcx",8,8,16,8);
		video3dinfo.mediumfont = new softfont("sysfont.pcx", 8, 8, 16, 8, 2, 2);
		video3dinfo.largefont = new softfont("font0.png", 16, 32, 8, 16);
		popdir();
	} else {
		logger("sysfont already loaded...\n");
	}
#endif
	static S32 firsttime=1;
	if (firsttime) {
		videoinfo.video_maindriver=-1;
		firsttime=0;
	}
	if (islocked)
		errorexit("video_init locked\n");
	*B32=dummy32;
	script* sc=video_getnmaindrivers();
	wininfo.wantedvideomaindriver=min(maindriver,sc->num()-1);
	wininfo.wantedvideosubdriver=subdriver;
	delete sc;
}

void video_uninit()
{
	if (islocked)
		errorexit("video_uninit locked\n");
	if (videoinfo.video_maindriver>=0) {
//		bitmap32free(videoinfo.wbuffer3d);
		videoapis[videoinfo.video_maindriver].video_uninitfunc();
	}
	WX=0;
	WY=0;
	videoinfo.video_maindriver=-1;
	if (videoinfo.mat2shader) {
		delete videoinfo.mat2shader;
		videoinfo.mat2shader=0;	
	}
#if 1
	if (video3dinfo.sysfont) {
		bitmap32free(video3dinfo.sysfont);
		bitmap32free(video3dinfo.sysfont2);
		delete video3dinfo.smallfont;
		delete video3dinfo.mediumfont;
		delete video3dinfo.largefont;
		video3dinfo.sysfont=0;
		video3dinfo.sysfont2=0;
		video3dinfo.smallfont = 0;
		video3dinfo.mediumfont = 0;
		video3dinfo.largefont = 0;
	}
#endif
	logger("video_uninit\n");

}

void video_paintwindow(U32 hwn) // can also be used to process a paint message
{
	if (islocked && !hwn)
		errorexit("video_paintwindow locked\n");
	showwin();
	if (hwn) { // redirect a real paint message to gdi
		gdi_paintwindow(hwn);
		return;
	}
	if (videoinfo.video_maindriver>=0)
		videoapis[videoinfo.video_maindriver].video_paintwindowfunc(hwn);
}

void video_lock()
{
	if (islocked)
		return; //errorexit("already locked\n");
	islocked++;
	perf_start(VIDLOCK);
	if (videoinfo.video_maindriver>=0) {
		videoapis[videoinfo.video_maindriver].video_lockfunc();
		//if (B32->data == 0)
		//	errorexit("video_lock data == null !!");
	}
	perf_end(VIDLOCK);
}

void video_unlock()
{
	if (!islocked)
		return; //errorexit("already unlocked\n");
	islocked--;
	perf_start(VIDUNLOCK);
	if (videoinfo.video_maindriver>=0)
		videoapis[videoinfo.video_maindriver].video_unlockfunc();
	perf_end(VIDUNLOCK);
}

