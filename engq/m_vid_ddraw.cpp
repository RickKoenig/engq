#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>
#include "m_perf.h"

// direct draw
static S32 nsubdrivers,nvidmodes;
static DDSURFACEDESC2 bestvidmodedesc,getsurfdesc;
static S32 bestmodenum;
static S32 bestdist;
static GUID driverguid,*lpdriverguid,lastguid,*lplastguid;
static LPDIRECTDRAW7 ddraw7;
static LPDIRECTDRAWSURFACE7 dsurf7;
static DWORD desiredx,desiredy;//,desiredbpp;

static void logsurfdesc2(LPDDSURFACEDESC2 sd)
{
	logger("surfdesc2\n");
// see if we have a valide width, height
	if ((sd->dwFlags&(DDSD_WIDTH|DDSD_HEIGHT))==(DDSD_WIDTH|DDSD_HEIGHT))
		logger("x %4d, y %4d, ",sd->dwWidth,sd->dwHeight);
// gotta have pitch
	if (sd->dwFlags&DDSD_PITCH)
		logger("p %4d, ",sd->lPitch);
	if (sd->dwFlags&DDSD_BACKBUFFERCOUNT)
		logger("backbuffercount %d, ",sd->dwBackBufferCount);
	if (sd->dwFlags&DDSD_REFRESHRATE)
		logger("r %d, ",sd->dwRefreshRate);
	if (sd->dwFlags&DDSD_ALPHABITDEPTH)
		logger("a %d, ",sd->dwAlphaBitDepth);
// don't really need caps just yet
	if (sd->dwFlags&DDSD_CAPS) {
		logger("ddscaps(");
		if (sd->ddsCaps.dwCaps&DDSCAPS_3DDEVICE)
			logger("3ddevice ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_NONLOCALVIDMEM)
			logger("nonlocalvidmem ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_PRIMARYSURFACE)
			logger("primarysurface ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_STANDARDVGAMODE)
			logger("starndardvgamode ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_SYSTEMMEMORY)
			logger("systemmemory ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_TEXTURE)
			logger("texture ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_VIDEOMEMORY)
			logger("videomemory ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_VIDEOPORT)
			logger("videoport ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_VISIBLE)
			logger("visible ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_WRITEONLY)
			logger("writeonly ");
		if (sd->ddsCaps.dwCaps&DDSCAPS_ZBUFFER)
			logger("zbuffer ");
		logger(")");
	}
// check bpp
	if (sd->dwFlags&DDSD_PIXELFORMAT) {
		logger("bpp %2d, ",sd->ddpfPixelFormat.dwRGBBitCount);
// check color masks and set mode16 to 555 or 565 if 16 bit mode
		logger("rmsk %08x, ",sd->ddpfPixelFormat.dwRBitMask);
		logger("gmsk %08x, ",sd->ddpfPixelFormat.dwGBitMask);
		logger("bmsk %08x, ",sd->ddpfPixelFormat.dwBBitMask);
		logger("amsk %08x, ",sd->ddpfPixelFormat.dwRGBAlphaBitMask);
// make sure it's RGB
		if (sd->ddpfPixelFormat.dwFlags&DDPF_RGB)
			logger("RGB ");
// no alpha
		if (sd->ddpfPixelFormat.dwFlags&DDPF_ALPHA)
			logger("A ");
		if (sd->ddpfPixelFormat.dwFlags&DDPF_ALPHAPIXELS)
			logger("AP ");
		if (sd->ddpfPixelFormat.dwFlags&DDPF_ALPHAPREMULT)
			logger("APM ");
		if (sd->ddpfPixelFormat.dwFlags&DDPF_PALETTEINDEXED8)
			logger("PI8 ");
		if (sd->ddpfPixelFormat.dwFlags&DDPF_PALETTEINDEXEDTO8)
			logger("PITO8 ");
	}
	logger("\n");
}

static BOOL CALLBACK directdrawenumcallback(GUID FAR *lpGUID,
		LPSTR lpDriverDescription,LPSTR lpDriverName,LPVOID lpContext)          
{
	if (videoinfo.video_subdriver==nsubdrivers) {
		if (lpGUID) {
			driverguid=*lpGUID;
			lpdriverguid=&driverguid;
		}
		logger("  * ");
	} else
		logger("    ");
	if (lpGUID) {
		lastguid=*lpGUID;
		lplastguid=&lastguid;
	}
	logger("subdriver %2d '%s' '%s'\n",nsubdrivers,lpDriverName,lpDriverDescription);
	nsubdrivers++;
	return TRUE;
}

static void makeddrawbitmaps()
{
	lock32=bitmap32alloc(WX,WY,C32BLACK);
	outtextxy32(lock32,WX/2-40,WY/2-4,C32WHITE,"Loading...");
#ifdef D2_3D
	d2_3d_init();
//	video3dinfo.wbuffer3d=bitmap32alloc(WX,WY,C32BLACK);
#endif
}

static void freeddrawbitmaps()
{
	bitmap32free(lock32);
	lock32=&dummy32;
#ifdef D2_3D
	d2_3d_uninit();
//	bitmap32free(video3dinfo.wbuffer3d);
#endif
}

void ddraw_video_uninit()
{
	HRESULT hr;
	if (dsurf7) {
		hr=dsurf7->Release();
		if (hr!=DD_OK)
			errorexit("IDirectSurface_Release %08x",hr);
		dsurf7=0;
	}
	freeddrawbitmaps();
	WX=0;
	WY=0;
	hr=ddraw7->Release();
	if (hr!=DD_OK)
		errorexit("IDirectDraw7_Release %08x",hr);
	ddraw7=0;
}

void ddraw_paintwindow(U32 paintwindow)
{
	DDSURFACEDESC2 surf7;
	HRESULT hr;
	S32 curlock;
	static S32 lastlock; // trick to reposition window, should use focus messages
	curlock=1;
// lock down primary surface
	perf_start(DDRAWLOCK);
	surf7.dwSize=sizeof(surf7);
	hr=dsurf7->Lock(0,&surf7,DDLOCK_WAIT,0);                    
	if (hr==DDERR_SURFACELOST) {
		hr=ddraw7->RestoreAllSurfaces();                    
		figureoutwindowpos(1);
		if (hr==DD_OK) {
			hr=dsurf7->Lock(0,&surf7,DDLOCK_WAIT,0);
			logger("surface restore and lock a sucess\n");
		} // else
	}
	perf_end(DDRAWLOCK);
// blit from sysmem to primary surface
	if (hr==DD_OK) {
		perf_start(BLITDIB);
		{
			struct bitmap32 surf32;
			surf32.size.x=getsurfdesc.lPitch>>2;
//			logger("pitch %d\n",getsurfdesc.lPitch);
			surf32.size.y=WY;
			surf32.data=(C32*)surf7.lpSurface;
			surf32.cliprect.topleft.x=0;
			surf32.cliprect.topleft.y=0;
			surf32.cliprect.size.x=WX;
			surf32.cliprect.size.y=WY;
			fastblit32(lock32,&surf32,0,0,0,0,WX,WY);
		}
		perf_end(BLITDIB);
// unlock surface
		perf_start(DDRAWUNLOCK);
		hr=dsurf7->Unlock(0);
		if (hr!=DD_OK)
			errorexit("IDirectDrawSurface7_UnLock %08x",hr);
		perf_end(DDRAWUNLOCK);
	} else
		logger("IDirectDrawSurface7_Lock %08x\n",hr);
//	if (curlock && !lastlock) {
//		fixupddrawwindow(); // keep mouse alive, windows messed up my mouse window(gdi), fixit
//		logger("keep mouse alive\n");
//	}
	lastlock=curlock;
}

static HRESULT WINAPI vidmodescallback(LPDDSURFACEDESC2 sd,LPVOID lpContext)
{
	S32 goodmode=1;
	logger("%2d: ",nvidmodes);
// see if we have a valid width, height
	if ((sd->dwFlags&(DDSD_WIDTH|DDSD_HEIGHT))==(DDSD_WIDTH|DDSD_HEIGHT)) {
		logger("x %4d, y %4d, ",sd->dwWidth,sd->dwHeight);
		if (desiredx>sd->dwWidth || desiredy>sd->dwHeight)
			goodmode=0;
	} else 
		goodmode=0;
// gotta have pitch
	if (sd->dwFlags&DDSD_PITCH)
		logger("p %4d, ",sd->lPitch);
	else
		goodmode=0;
	if (sd->dwFlags&DDSD_BACKBUFFERCOUNT)
		logger("backbuffercount %d, ",sd->dwBackBufferCount);
	if (sd->dwFlags&DDSD_REFRESHRATE)
		logger("r %d, ",sd->dwRefreshRate);
	if (sd->dwFlags&DDSD_ALPHABITDEPTH)
		logger("a %d, ",sd->dwAlphaBitDepth);
// check bpp
	if (sd->dwFlags&DDSD_PIXELFORMAT) {
		logger("bpp %2d, ",sd->ddpfPixelFormat.dwRGBBitCount);
		if (sd->ddpfPixelFormat.dwRGBBitCount!=32)
			goodmode=0;
// check color masks and set mode16 to 555 or 565 if 16 bit mode
		logger("rmsk %08x, ",sd->ddpfPixelFormat.dwRBitMask);
		logger("gmsk %08x, ",sd->ddpfPixelFormat.dwGBitMask);
		logger("bmsk %08x, ",sd->ddpfPixelFormat.dwBBitMask);
		logger("amsk %08x, ",sd->ddpfPixelFormat.dwRGBAlphaBitMask);
		if (sd->ddpfPixelFormat.dwRBitMask!=0xff0000)
			goodmode=0;
		if (sd->ddpfPixelFormat.dwGBitMask!=0xff00)
			goodmode=0;
		if (sd->ddpfPixelFormat.dwBBitMask!=0xff)
			goodmode=0;
// make sure it's RGB
		if (sd->ddpfPixelFormat.dwFlags&DDPF_RGB)
			logger("RGB ");
		else
			goodmode=0;
// no alpha
		if (sd->ddpfPixelFormat.dwFlags&DDPF_ALPHA) {
			logger("A ");
			goodmode=0;
		}
		if (sd->ddpfPixelFormat.dwFlags&DDPF_ALPHAPIXELS) {
			logger("AP ");
			goodmode=0;
		}
		if (sd->ddpfPixelFormat.dwFlags&DDPF_ALPHAPREMULT) {
			logger("APM ");
			goodmode=0;
		}
		if (sd->ddpfPixelFormat.dwFlags&DDPF_PALETTEINDEXED8)
			logger("PI8 ");
		if (sd->ddpfPixelFormat.dwFlags&DDPF_PALETTEINDEXEDTO8)
			logger("PITO8 ");
		if (goodmode)
			logger("GOODMODE");
		logger("\n");
		if (goodmode) {
			S32 dist;
			dist=sd->dwWidth+sd->dwHeight;
			dist<<=1;
			if (dist<bestdist) {
				bestvidmodedesc=*sd;
				bestmodenum=nvidmodes;
				bestdist=dist;
			}
		}
	}
	nvidmodes++;
	return DDENUMRET_OK;
}

S32 ddraw_setupwindow(S32 x,S32 y)
{
	S32 ddrawbail=0;
	static DDSURFACEDESC2 surfd;
	HRESULT hr;
	logger(" directdraw init:\n");
	videoinfo.video_fullscreen=1;
	useoscursor(0);
	nsubdrivers=0;
// enum video cards
	lpdriverguid=lplastguid=0;
	hr=DirectDrawEnumerate(directdrawenumcallback,0);
	if (hr!=DD_OK)
		return 0;
	if (nsubdrivers==0)
		return 0;
	if (videoinfo.video_subdriver>=nsubdrivers) {
		videoinfo.video_subdriver=nsubdrivers-1;
		driverguid=lastguid;
		lpdriverguid=lplastguid;
	}
// pick a video card
	hr=DirectDrawCreateEx(lpdriverguid,(void**)(&ddraw7),IID_IDirectDraw7,0);
	if (hr!=DD_OK)
		return 0;
// setup a default video resolution
// enum display modes
	bestmodenum=-1;
	bestdist=1000000000;
	nvidmodes=0;
	desiredx=x;
	desiredy=y;
	logger("video modes -------------------------------\n");
	hr=ddraw7->EnumDisplayModes(DDEDM_REFRESHRATES|DDEDM_STANDARDVGAMODES,0,0,vidmodescallback);
	if (hr!=DD_OK)
		errorexit("EnumDisplayModes %08x",hr);
	if (bestmodenum==-1)
		ddrawbail=1;
	else {
		logger("best mode is %d\n",bestmodenum);
	} 
	if (ddrawbail) {
		hr=ddraw7->Release();
		if (hr!=DD_OK)
			errorexit("IDirectDraw7_Release %08x",hr);
		ddraw7=0;
		return 0;
	}
// pick display mode
	WX=bestvidmodedesc.dwWidth;
	WY=bestvidmodedesc.dwHeight;
	hr=ddraw7->SetCooperativeLevel((HWND)wininfo.MainWindow,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
	if (hr!=DD_OK)
		errorexit("SetCooperativeLevel %08x",hr);
	hr=ddraw7->SetDisplayMode(WX,WY,bestvidmodedesc.ddpfPixelFormat.dwRGBBitCount,0,0); 
	if (hr!=DD_OK)
		errorexit("SetDisplayMode %08x",hr);
// create primary surface
	surfd.dwSize=sizeof(surfd); 
	surfd.dwFlags=DDSD_CAPS ; 
	surfd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE; 
 	hr=ddraw7->CreateSurface(&surfd,&dsurf7,0); 
	if (hr!=DD_OK)
		errorexit("CreateSurface %08x",hr);
	getsurfdesc.dwSize=sizeof(getsurfdesc);
	hr=dsurf7->GetSurfaceDesc(&getsurfdesc);
	if (hr!=DD_OK)
		errorexit("GetSurfaceDesc %08x",hr);
	logsurfdesc2(&getsurfdesc);
	makeddrawbitmaps();
/*
#ifdef D2_3D
	texavail[TEX32CK]=1;
	texavail[TEX32NCK]=1;
#endif
*/
	figureoutwindowpos(1);
	ddraw_paintwindow(0);
	return 1;
}

static script* scd;
static BOOL CALLBACK ddsubdriversenumcallback(GUID FAR *lpGUID,
	LPSTR lpDriverDescription,LPSTR lpDriverName,LPVOID lpContext)          
{
	scd->addscript((C8*)lpDriverName);
	return TRUE;
}

// return a script of all valid direct draw vid cards
script* ddraw_getnsubdrivers()
{
	S32 hr;
	scd=new script;
	hr=DirectDrawEnumerate(ddsubdriversenumcallback,0);
	return scd;
}

void ddraw_lock()
{
	*B32=*lock32;
}

void ddraw_unlock()
{
	*B32=dummy32;
}
