#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
//#include <d3dx9.h>
#include <d3d9.h>
#include "m_vid_dx9.h"
#include "d2_font.h"
#include "d2_software_font.h"
#include "d2_dx9_font.h"
#include "d2_dx9.h"

vid3d videoinfodx9;

namespace vidd3d {

#define makecolorrgb(r,g,b) (((r)<<16)+((g)<<8)+(b))
#define makecolorargb(a,r,g,b) (((a)<<24)+((r)<<16)+((g)<<8)+(b))


bool shown;

}
using namespace vidd3d;

void dx9_video_uninit()
{
#ifdef D2_3D
	d2_3d_uninit();
#endif
	logger("video_dx9 uninit\n");
	S32 s=textureb::rc.rlistsize();
	if (s)
		errorexit("dx9_video_uninit: still have %d textures to free\n",s);
	s=modelb::rc.rlistsize();
	if (s)
		errorexit("dx9_video_uninit: still have %d models to free\n",s);
	if (videoinfodx9.d3d9device) {
        s=videoinfodx9.d3d9device->Release();
		logger("d3d9device release %d\n",s);
		videoinfodx9.d3d9device=0;
	}
	if (videoinfodx9.d3d9) {
        s=videoinfodx9.d3d9->Release();
		logger("d3d9 release %d\n",s);
		videoinfodx9.d3d9=0;
	}
}

void dx9_paintwindow(U32 paintwindow)
{
// Present the backbuffer contents to the display
//    perf_start(PRESENT);
	HRESULT hr=videoinfodx9.d3d9device->Present( NULL, NULL, NULL, NULL );
	if (hr!=D3D_OK) {
//		logger("dx9:present error %08x\n",hr);
		hr=videoinfodx9.d3d9device->TestCooperativeLevel();
		if (hr!=D3D_OK)
//			logger("dx9:TestCooperativeLevel %08x\n",hr);
		if (hr==D3DERR_DEVICENOTRESET) {
			release_d3dpool_default();
			hr=videoinfodx9.d3d9device->Reset(&videoinfodx9.pp);
			if (hr!=D3D_OK) {
				logger("dx9:reset error %08x\n",hr);
			}
			reset_d3dpool_default();
			if (videoinfo.video_fullscreen)
				figureoutwindowpos(1);
		}
	}
//    perf_end(PRESENT);
	if (!shown) {
// Show the window
		if (!videoinfo.video_fullscreen) {
			ShowWindow( (HWND)wininfo.MainWindow, SW_SHOWDEFAULT );
			UpdateWindow( (HWND)wininfo.MainWindow );
		}
		shown=true;
	}
}

static S32 dx9_setupwindow(S32 x,S32 y)
{
	logger("video init\n");
	S32 s=textureb::rc.rlistsize();
	if (s)
		errorexit("dx9_setupwindow: still have %d textures to free",s);
	s=modelb::rc.rlistsize();
	if (s)
		errorexit("dx9_setupwindow: still have %d models to free",s);
	WX=x;
	WY=y;
	videoinfodx9.zbuffertype=D3DZB_TRUE;
// Initialize Direct3D
// Create the D3D object.
    if( NULL == ( videoinfodx9.d3d9 = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        errorexit("can't created3d9");
// Set up the structure used to create the D3DDevice
    ZeroMemory( &videoinfodx9.pp, sizeof(videoinfodx9.pp) );
	if (videoinfo.video_fullscreen) {
		useoscursor(0);
		videoinfodx9.pp.Windowed = FALSE;
		videoinfodx9.pp.BackBufferFormat=D3DFMT_A8R8G8B8;
		videoinfodx9.pp.BackBufferWidth=WX;
		videoinfodx9.pp.BackBufferHeight=WY;
		videoinfodx9.pp.FullScreen_RefreshRateInHz=0;
		videoinfodx9.pp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	} else {
		if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
			useoscursor(0);
		else
			useoscursor(1);
		figureoutwindowpos(0);
		videoinfodx9.pp.Windowed = TRUE;
		videoinfodx9.pp.BackBufferFormat = D3DFMT_UNKNOWN;
		videoinfodx9.pp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}
	videoinfodx9.pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
//	videoinfodx9.pp.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
	videoinfodx9.pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
    videoinfodx9.pp.BackBufferCount = 1;
    videoinfodx9.pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	videoinfodx9.pp.hDeviceWindow=(HWND)wininfo.MainWindow;
	videoinfodx9.pp.EnableAutoDepthStencil = TRUE;
// Create the D3DDevice
	HRESULT result;
	result=(videoinfodx9.d3d9)->GetDeviceCaps(D3DADAPTER_DEFAULT ,D3DDEVTYPE_HAL ,&videoinfodx9.ddcaps9);
	if (result!=D3D_OK)
		errorexit("can't get dev caps");
	logger("vertex shader version %d.%d\n",
		D3DSHADER_VERSION_MAJOR(videoinfodx9.ddcaps9.VertexShaderVersion),
		D3DSHADER_VERSION_MINOR(videoinfodx9.ddcaps9.VertexShaderVersion));
	logger("pixel shader version %d.%d\n",
		D3DSHADER_VERSION_MAJOR(videoinfodx9.ddcaps9.PixelShaderVersion),
		D3DSHADER_VERSION_MINOR(videoinfodx9.ddcaps9.PixelShaderVersion));
	if (D3DSHADER_VERSION_MAJOR(videoinfodx9.ddcaps9.VertexShaderVersion)<2) {
		logger("bad vertex shader version, switching to software vertex processing\n");
		videoinfodx9.hiendmachine=0;
	}
	if (D3DSHADER_VERSION_MAJOR(videoinfodx9.ddcaps9.PixelShaderVersion)<2) {
		logger("bad pixel shader version, no d3d, switching to software\n");
		dx9_video_uninit(); // reduce
		return 0;
	}
	while(1) {
		D3DDEVTYPE devtype=D3DDEVTYPE_HAL;
//		D3DDEVTYPE devtype=D3DDEVTYPE_REF;
		if (videoinfodx9.hiendmachine) {
			videoinfodx9.pp.AutoDepthStencilFormat = D3DFMT_D24S8;
			result= (videoinfodx9.d3d9)->CreateDevice( D3DADAPTER_DEFAULT, devtype, (HWND)wininfo.MainWindow,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,&videoinfodx9.pp, (LPDIRECT3DDEVICE9*)(&videoinfodx9.d3d9device)) ;
//			result=D3D_OK+1;
			if (result!=D3D_OK)
				logger("downgrading 'hiendmachine' from 1 to 0\n");
		} else {
			videoinfodx9.pp.AutoDepthStencilFormat = D3DFMT_D16;
			result= (videoinfodx9.d3d9)->CreateDevice( D3DADAPTER_DEFAULT, devtype, (HWND)wininfo.MainWindow,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,&videoinfodx9.pp, (LPDIRECT3DDEVICE9*)(&videoinfodx9.d3d9device)) ;
//			result=D3D_OK+1;
			if (result!=D3D_OK)
				logger("downgrading 'hiendmachine' from 0 to software\n");
		}
		if (result==D3D_OK)
			break; // fine
		if (!videoinfodx9.hiendmachine)
			break; // no reduction
		videoinfodx9.hiendmachine=0; // reduce
	} 
	if (result!=D3D_OK) {
 //       errorexit("can't created3d9device %08x",result);
		dx9_video_uninit(); // reduce
		return 0;
	}
	logger("d3d backbuffer format %d\n",videoinfodx9.pp.BackBufferFormat);
// determine if in 32bit display mode
	if (videoinfodx9.pp.BackBufferFormat!=D3DFMT_R8G8B8) {
		if (videoinfodx9.pp.BackBufferFormat!=D3DFMT_A8R8G8B8) {
			if (videoinfodx9.pp.BackBufferFormat!=D3DFMT_X8R8G8B8) {
				logger("bad d3d backbuffer format %d\n",videoinfodx9.pp.BackBufferFormat);
				dx9_video_uninit(); // reduce
				return 0;
			}
		}
	}

// Turn off culling, so we see the front and back of the triangle
//   wininfo.d3d9device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
// Turn off D3D lighting, since we are providing our own vertex colors
	videoinfodx9.d3d9device->SetRenderState( D3DRS_LIGHTING, FALSE );
// setup z or w buffer
	videoinfodx9.d3d9device->SetRenderState( D3DRS_ZENABLE ,videoinfodx9.zbuffertype);
	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 0, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MINFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MAGFILTER ,D3DTEXF_LINEAR );
	videoinfodx9.d3d9device->SetSamplerState( 1, D3DSAMP_MIPFILTER ,D3DTEXF_LINEAR );
	shown=false;
	if (videoinfo.video_fullscreen)
		figureoutwindowpos(1);
#ifdef D2_3D
	d2_3d_init();
#endif
	dx9_paintwindow(0);
	return 1;
}

S32 dx9_wnd_setupwindow(S32 x,S32 y)
{
	videoinfo.video_fullscreen=0;
	return dx9_setupwindow(x,y);
}

S32 dx9_fs_setupwindow(S32 x,S32 y)
{
	videoinfo.video_fullscreen=1;
	return dx9_setupwindow(x,y);
}

// return a script of all valid d3d vid cards
static script* scd;
script* dx9_getnsubdrivers()
{
	scd=new script;
	scd->addscript("default");
	return scd;
}

static IDirect3DSurface9* bb;
// TODO: make 2D stuff blit onto a texture instead of doing this lock...
void dx9_lock()
{
	D3DLOCKED_RECT lr;
	HRESULT hr=videoinfodx9.d3d9device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&bb);
	if (hr!=D3D_OK)
		errorexit("can't getbackbuffer %08x",hr);
	D3DSURFACE_DESC desc;
	bb->GetDesc(&desc);
	if (!bb)
		errorexit("no bb");
//	hr=bb->LockRect(&lr,0,0);
	S32 numTries = 20;
	S32 i;
	for (i = 0; i < numTries; ++i) {
		hr = bb->LockRect(&lr, 0, D3DLOCK_DONOTWAIT);
		if (hr != D3DERR_WASSTILLDRAWING) {
			break;
		}
		Sleep(100);
		//hr = bb->Release(); // try to release
		//bb = 0;
	}
	if (i == numTries) {
		logger("BAD: gave up on dx9_lock after %d tries\n", numTries);
	} else {
		;// logger("GOOD: got a lock on dx9_lock after %d tries\n", i);
	}

#if 0
	hr=bb->LockRect(&lr,0,D3DLOCK_DONOTWAIT);
	// it's tough to lock a buffer for 2D drawing when also in 3D mode...
	while (hr==D3DERR_WASSTILLDRAWING) {
		Sleep(100);
		hr=bb->LockRect(&lr,0,D3DLOCK_DONOTWAIT);
		if (hr==D3DERR_WASSTILLDRAWING) {
			hr=bb->Release(); // try to release
			bb=0;
			return; // give up, black window ...???!!!
		}
	} 
#endif
	if (WX != desc.Width || WY != desc.Height)
		errorexit("incompatible sizes for dx9_LockRect");
	B32->cliprect.topleft.x=0;
	B32->cliprect.topleft.y=0;
	B32->cliprect.size.x=WX;
	B32->cliprect.size.y=WY;
	B32->size.x=lr.Pitch>>2;
	B32->size.y=WY;
	B32->data=(C32*)lr.pBits;
	if (!B32->data) {
		//errorexit("dx9_lock, data == null !!"); // assert
		logger("ERROR: dx9_lock, data == null !!\n"); // ignore
	}
}

void dx9_unlock()
{
	*B32=dummy32;
	if (bb==0)
		return;
	HRESULT hr=bb->UnlockRect();
	if (hr != D3D_OK) {
		//errorexit("can't unlock %08x", hr); // assert
		logger("ERROR: can't unlock %08x\n"); // ignore
	}
	hr=bb->Release();
	if (hr!=D3D_OK)
		errorexit("can't release %08x",hr);
	bb=0;
}
