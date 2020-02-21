#define INCLUDE_WINDOWS
#define D2_3D // for zbuffer and texavail
#include <m_eng.h>
#include "m_perf.h"

//#define DIB565
#define DIB24 // make a 24 bit dib for a 32 bit engine and convert 32bit to 24bit before bitblt
			  // otherwise make a 32 bit dib for 32 bit (you would think this would be faster??)
//#define DIB32

#ifdef DIB565 // 16 bit 565
static U8 bitmapinfo565[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*3];
static LPBITMAPINFO lpbitmapinfo565=(LPBITMAPINFO)bitmapinfo565;
static HBITMAP hbmp565;
static U16 *dibbits565; 
static struct bitmap16 lock16;
static void convert32to565()
{
	S32 i,j,x;
	C32* b32p=lock32->data;
	x=((2*WX+3)&~3);
	U16* px=lock16.data;
	for (j=0;j<WY;++j,px+=(x>>1)) {
		for (i=0;i<WX;++i,++b32p) {
			C32 v=*b32p;
			px[i]=(v.b>>3)+((v.g>>2)<<5)+((v.r>>3)<<11);
		}
	} 
}
#endif
#ifdef DIB24 // 24 bit
// these seem faster then 32 bit for some reason
static U8 bitmapinfo24[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*3];
static LPBITMAPINFO lpbitmapinfo24=(LPBITMAPINFO)bitmapinfo24;
static HBITMAP hbmp24;
static struct rgb24 *dibbits24; // actually 24bit
static struct bitmap24 lock24;
static void convert32to24()
{
	S32 i,j,x;
	C32* b32p=lock32->data;
	x=(3*WX+3)&~3;
	U8* px=lock24.data;
	for (j=0;j<WY;++j,px+=x) {
		S32 wx3=3*WX;
		for (i=0;i<wx3;i+=3,++b32p) {
			C32 v=*b32p;
			px[i]=v.b;
			px[i+1]=v.g;
			px[i+2]=v.r;
		}
	} 
}
#endif
#ifdef DIB32
// 32 bit
static U8 bitmapinfo32[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*3];
static LPBITMAPINFO lpbitmapinfo32=(LPBITMAPINFO)bitmapinfo32;
static HBITMAP hbmp32;
static U32 *dibbits32; // actually 32bit
#endif

// make a dibsection
static void makebitmaps()
{
	HDC hdc;
	U32 x,y;
#ifdef DIB565
	if (hbmp565)
		return;
	DWORD *bitfields565=(DWORD *)((U8*)lpbitmapinfo565+sizeof(BITMAPINFOHEADER));
#endif
#ifdef DIB24
	if (hbmp24)
		return;
#endif
#ifdef DIB32
	if (hbmp32)
		return;
#endif
	hdc = GetDC(0);
	y=WY;
#ifdef DIB565
	x=(2*WX+3)&~3;
	lpbitmapinfo565->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); 
	lpbitmapinfo565->bmiHeader.biWidth=WX; 
	lpbitmapinfo565->bmiHeader.biHeight=-WY; 
	lpbitmapinfo565->bmiHeader.biPlanes=1; 
	lpbitmapinfo565->bmiHeader.biBitCount=16; 
	lpbitmapinfo565->bmiHeader.biCompression=BI_BITFIELDS; 
	lpbitmapinfo565->bmiHeader.biSizeImage=x*y; 
	lpbitmapinfo565->bmiHeader.biXPelsPerMeter=0; 
	lpbitmapinfo565->bmiHeader.biYPelsPerMeter=0; 
	lpbitmapinfo565->bmiHeader.biClrUsed=0; 
	lpbitmapinfo565->bmiHeader.biClrImportant=0;
	bitfields565[0]=0x0000f800;  // red
	bitfields565[1]=0x000007e0;	// green
	bitfields565[2]=0x0000001f;  // blue
	hbmp565=CreateDIBSection(
		hdc,					// handle of device context
		lpbitmapinfo565,	// address of structure containing bitmap size, format, and color data
		DIB_RGB_COLORS,			// color data type indicator: RGB values or palette indices
		(void**)&dibbits565,	// pointer to variable to receive a pointer to the bitmap's bit values
		0,					// optional handle to a file mapping object
		0						// offset to the bitmap bit values within the file mapping object
		);	
	if (!hbmp565)
		errorexit("can't create 565 bit dibsection");
	memset(dibbits565,0,lpbitmapinfo565->bmiHeader.biSizeImage);
	lock16.size.x=x/2; // dibsections scanlines are a multiple of 4 bytes
	lock16.size.y=y;
	lock16.data=dibbits565;
	lock32=bitmap32alloc(WX,WY,(C32)0U);
	outtextxy32(lock32,WX/2-40,WY/2-4,C32WHITE,"Loading...");
#endif
#ifdef DIB24
	x=(3*WX+3)&~3;
	lpbitmapinfo24->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); 
	lpbitmapinfo24->bmiHeader.biWidth=WX; 
	lpbitmapinfo24->bmiHeader.biHeight=-WY; 
	lpbitmapinfo24->bmiHeader.biPlanes=1; 
	lpbitmapinfo24->bmiHeader.biBitCount=24; 
	lpbitmapinfo24->bmiHeader.biCompression=BI_RGB; 
	lpbitmapinfo24->bmiHeader.biSizeImage=x*y; 
	lpbitmapinfo24->bmiHeader.biXPelsPerMeter=0; 
	lpbitmapinfo24->bmiHeader.biYPelsPerMeter=0; 
	lpbitmapinfo24->bmiHeader.biClrUsed=0; 
	lpbitmapinfo24->bmiHeader.biClrImportant=0;
	hbmp24=CreateDIBSection(
		hdc,					// handle of device context
		lpbitmapinfo24,	// address of structure containing bitmap size, format, and color data
		DIB_RGB_COLORS,			// color data type indicator: RGB values or palette indices
		(void**)&dibbits24,	// pointer to variable to receive a pointer to the bitmap's bit values
		0,					// optional handle to a file mapping object
		0						// offset to the bitmap bit values within the file mapping object
		);	
	if (!hbmp24)
		errorexit("can't create 24 bit dibsection");
	lock24.size.x=WX; // dibsections scanlines are a multiple of 4 bytes
	lock24.size.y=y;
	lock24.data=(U8*)dibbits24;
	lock32=bitmap32alloc(WX,WY,C32BLACK);
	outtextxy32(lock32,WX/2-40,WY/2-4,C32WHITE,"Loading...");
	convert32to24();
#endif
#ifdef DIB32
	x=4*WX;
	lpbitmapinfo32->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); 
	lpbitmapinfo32->bmiHeader.biWidth=WX; 
	lpbitmapinfo32->bmiHeader.biHeight=-WY; 
	lpbitmapinfo32->bmiHeader.biPlanes=1; 
	lpbitmapinfo32->bmiHeader.biBitCount=32; 
	lpbitmapinfo32->bmiHeader.biCompression=BI_RGB;//BI_BITFIELDS; 
	lpbitmapinfo32->bmiHeader.biSizeImage=x*y; 
	lpbitmapinfo32->bmiHeader.biXPelsPerMeter=0; 
	lpbitmapinfo32->bmiHeader.biYPelsPerMeter=0; 
	lpbitmapinfo32->bmiHeader.biClrUsed=0; 
	lpbitmapinfo32->bmiHeader.biClrImportant=0;
//	bitfields32[0]=0x00ff0000;  // red
//	bitfields32[1]=0x0000ff00;	// green
//	bitfields32[2]=0x000000ff;  // blue
	hbmp32=CreateDIBSection(
		hdc,					// handle of device context
		lpbitmapinfo32,	// address of structure containing bitmap size, format, and color data
		DIB_RGB_COLORS,			// color data type indicator: RGB values or palette indices
		(void**)&dibbits32,	// pointer to variable to receive a pointer to the bitmap's bit values
		0,					// optional handle to a file mapping object
		0						// offset to the bitmap bit values within the file mapping object
		);	
	if (!hbmp32)
		errorexit("can't create 32 bit dibsection");
	{
		static struct bitmap32 lock32d;
		lock32d.size.x=x>>2; // dibsections scanlines are a multiple of 4 bytes
		lock32d.size.y=y;
		lock32d.cliprect.topleft.x=0;
		lock32d.cliprect.topleft.y=0;
		lock32d.cliprect.size.x=WX;
		lock32d.cliprect.size.y=WY;
		lock32d.data=(C32*)dibbits32; 
		lock32=&lock32d;
	}
	outtextxy32(lock32,WX/2-40,WY/2-4,C32WHITE,"Loading...");
#endif
#ifdef D2_3D
	d2_3d_init();
#endif
	ReleaseDC(0,hdc);
}

static void freebitmaps()//U32 mode) // winexit
{
#ifdef DIB565
	if (!hbmp565)
		return;
	bitmap32free(lock32);
	lock32=&dummy32;
	DeleteObject(hbmp565);
	hbmp565=0;
#endif
#ifdef DIB24
	if (!hbmp24)
		return;
	bitmap32free(lock32);
	lock32=&dummy32;
	DeleteObject(hbmp24);
	hbmp24=0;
#endif
#ifdef DIB32
	if (!hbmp32)
		return;
	*lock32=dummy32;
	DeleteObject(hbmp32);
	hbmp32=0;
#endif
#ifdef D2_3D
	d2_3d_uninit();
#endif
}

void gdi_video_uninit()
{
	freebitmaps();
}

// messages
void gdi_paintwindow( U32 hwnd )
{
	PAINTSTRUCT ps;
	BITMAP bm;
	HDC hdc;
	HDC hdcMem;	// only valid during paint and mainproc
	HBITMAP hbp=0;
#ifdef DIB565
	hbp=hbmp565;
	perf_start(CONVERT32TO565);
	if (hbp)
		convert32to565();
	perf_end(CONVERT32TO565);
#endif
#ifdef DIB24
	hbp=hbmp24;
	perf_start(CONVERT32TO24);
	if (hbp)
		convert32to24();
	perf_end(CONVERT32TO24);
#endif
#ifdef DIB32
	hbp=hbmp32;
#endif
	if (!hwnd && !hbp)
		return;
	perf_start(GETDC);
	if (hwnd)
		hdc = BeginPaint(HWND(hwnd),&ps);
	else
		hdc = GetDC((HWND)wininfo.MainWindow);
	perf_end(GETDC);
	if (hbp) {
		perf_start(CREATECOMPATIBLEDC);
		hdcMem = CreateCompatibleDC(hdc);
		perf_end(CREATECOMPATIBLEDC);
		perf_start(GETOBJECT);
		GetObject(hbp,sizeof(bm),(LPSTR)&bm);
		perf_end(GETOBJECT);
		perf_start(SELECTOBJECT);
		hbp = (HBITMAP)SelectObject(hdcMem,hbp);
		perf_end(SELECTOBJECT);
		if (wininfo.dopaintmessages || !hwnd) {
			perf_start(BLITDIB);
			BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);
			perf_end(BLITDIB);
		}
		perf_start(SELECTOBJECT2);
		hbp = (HBITMAP)SelectObject(hdcMem,hbp);
		perf_end(SELECTOBJECT2);
		DeleteDC(hdcMem);
	}
	perf_start(RELEASEDC);
	if (hwnd)
		EndPaint(HWND(hwnd),&ps);
	else
		ReleaseDC(HWND(hwnd),hdc);
	perf_end(RELEASEDC);
}

S32 gdi_setupwindow(S32 x,S32 y)//,U32 bpp)
{
	videoinfo.video_fullscreen=0;
	WX=x;
	WY=y;
	figureoutwindowpos(0);
	freebitmaps();
	makebitmaps();
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		useoscursor(0);
	else
		useoscursor(1);
	return 1;
}

script* gdi_getnsubdrivers()
{
	script* sc=new script;
	sc->addscript("window");
	return sc;
}

void gdi_lock()
{
	*B32=*lock32;
}

void gdi_unlock()
{
	*B32=dummy32;
}
