#define INCLUDE_WINDOWS
#include <m_eng.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// standard
static LPDIRECTINPUT8 di8;
static LPDIRECTINPUTDEVICE8 key8,mouse8,joy8[MAXJOYSTICKS]; // joystick will still be handled by conventional calls (for now)
#define BUFFSIZE 200
// keyboard
#define MAXKEYBUFFER 32
static U8 keybuffer[MAXKEYBUFFER];
static S32 firstkey,lastkey,nkeys;
// mouse
static U32 mhold;
//S32 lastmousemode=MM_NORMAL;

static U8 vk2k[256]={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x09,0x00,0x00,0xa5,0x0d,0x00,0x00,
	0x01,0x03,0x05,0x12,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x1b,0x00,0x00,0x00,0x00,
	0x20,0x17,0x18,0x16,0x15,0x1c,0x1e,0x1d,0x1f,0x00,0x00,0x00,0x00,0x13,0x14,0x00,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0xab,0xac,0xad,0x00,0x00,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0x2a,0x2b,0x00,0x83,0x84,0x82,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x81,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3b,0x3d,0x2c,0x2d,0x2e,0x2f,
	0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5b,0x5c,0x5d,0x27,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static U8 vk2k2[256]={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x85,0x00,0x00,
	0x02,0x04,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xa9,0xa3,0xa1,0xa7,0xa4,0xa8,0xa6,0xa2,0x00,0x00,0x00,0x00,0xa0,0x84,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static U8 dik2k[256]={
	0x00,0x1b,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x2d,0x3d,0x08,0x09,
	0x71,0x77,0x65,0x72,0x74,0x79,0x75,0x69,0x6f,0x70,0x5b,0x5d,0x0d,0x03,0x61,0x73,
	0x64,0x66,0x67,0x68,0x6a,0x6b,0x6c,0x3b,0x27,0x60,0x01,0x5c,0x7a,0x78,0x63,0x76,
	0x62,0x6e,0x6d,0x2c,0x2e,0x2f,0x02,0x2a,0x05,0x20,0x07,0x90,0x91,0x92,0x93,0x94,
	0x95,0x96,0x97,0x98,0x99,0x81,0x11,0xa7,0xa8,0xa9,0x83,0xa4,0xa5,0xa6,0x2b,0xa1,
	0xa2,0xa3,0xa0,0x84,0x00,0x00,0x00,0x9a,0x9b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x85,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x82,0x00,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x15,0x1e,0x17,0x00,0x1c,0x00,0x1d,0x00,0x16,
	0x1f,0x18,0x13,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xab,0xac,0xad,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static U8 shifttab[256]={
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f, //00-0f
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f, //10-1f
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x22,0x28,0x29,0x2a,0x2b,0x3c,0x5f,0x3e,0x3f, //20-2f
	0x29,0x21,0x40,0x23,0x24,0x25,0x5e,0x26,0x2a,0x28,0x3a,0x3a,0x3c,0x2b,0x3e,0x3f, //30-3f
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f, //40-4f
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x5e,0x5f, //50-5f
	0x7e,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f, //60-6f
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f, //70-7f
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f, //80-8f
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f, //90-9f
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf, //a0-af
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf, //b0-bf
	0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf, //c0-cf
	0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf, //d0-df
	0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef, //e0-ef
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff, //f0-ff
};

U32 Joy_Init();
void Joy_Update();

void resetmhold() // sometimes the middle mouse button doesn't get a release message..
{
	mhold=0;
}
// resetinput
void resetinput()
{
	POINT p;
	S32 i;
	wininfo.mleftclicks=wininfo.mrightclicks=wininfo.mmiddleclicks=0;
	wininfo.mbut=mhold;
	if (!wininfo.usedirectinput || wininfo.dinput_nomouse) {
		if (wininfo.mousemode==MM_NORMAL && wininfo.lastmousemode==MM_INFINITE) { // switch to norm mode
			p.x=MX;
			p.y=MY;
			ClientToScreen((HWND)wininfo.MainWindow,&p);
			SetCursorPos(p.x,p.y);
//			logger("cp1\n");
			wininfo.lastmousemode=MM_NORMAL; // a cursor bug fix added 5-1-03

		}
	}
	if (!wininfo.usedirectinput) {
		for (i=0;i<256;i++)
			if (wininfo.keystate[vk2k[i]] || wininfo.keystate[vk2k2[i]])
				if (!(GetAsyncKeyState(i)&0x8000)) {
					wininfo.keystate[vk2k[i]]&=~1;
					wininfo.keystate[vk2k2[i]]&=~2;
				}
	}
}

// keyboard
static U32 getkey()
{
	U32 vk;
	if (!nkeys)
		return 0;
	vk=keybuffer[firstkey++];
	if (firstkey==MAXKEYBUFFER)
		firstkey=0;
	nkeys--;
	return vk;
}

void flushkeys()
{
	while(getkey())
		;
}

static void putkey(U32 vk)
{
//	logger("in putkey %d\n",vk);
	if (nkeys==MAXKEYBUFFER)
		return;
	keybuffer[lastkey++]=vk;
	if (lastkey==MAXKEYBUFFER)
		lastkey=0;
	nkeys++;
}

void getsomeinput()
{
	//static S32 repcount;
	static S32 repkey;
	if (wininfo.usedirectinput) {
		getdikdata(wininfo.keystate);
		if (!wininfo.dinput_nomouse)
			getdimdata();
	}
	Joy_Update();
// set global key once per frame
	KEY=getkey();
// do repeat key
	if (KEY) {
		if (repkey!=KEY) {
			repkey=KEY;
			wininfo.repcount=0;
		}
	} else
		;//repcount = 0;
	if (wininfo.keystate[repkey]) {
		wininfo.repcount++;
		//wininfo.keystate[repkey] = 0;
	} else
		wininfo.repcount=0;
	if (wininfo.repcount>=wininfo.repdelay && wininfo.repperiod>0 && wininfo.repdelay>=0) {
//		logger("putkey rep %d\n",repkey);
		putkey(repkey);
		wininfo.repcount-=wininfo.repperiod;
	}
	wininfo.dmx=wininfo.mx-wininfo.lmx;
	wininfo.dmy=wininfo.my-wininfo.lmy;
	wininfo.dmz=wininfo.mz-wininfo.lmz;
}


//U32 testvk;
void messonkeydown(HWND hwnd, UINT vk, BOOL fDown, S32 cRepeat, UINT flags)
{
	U8 k,k2;
	if (wininfo.usedirectinput)
		return;
	if (!(flags&KF_REPEAT)) {
//		if (vk)
//			testvk=vk;
		k=vk2k[vk];
		k2=vk2k2[vk];
		if (k) {
			if (wininfo.keystate[K_LEFTSHIFT] || wininfo.keystate[K_RIGHTSHIFT])
				putkey(shifttab[k]);
			else {
//				logger("messkeydown putkey %d\n",k);
				putkey(k);
			}
			wininfo.keystate[k]|=1;
		}
		if (k2)
			wininfo.keystate[k2]|=2;
	}
}

// mouse
void messonleftmousedown(HWND hwnd, BOOL fDoubleClick, S32 x, S32 y, UINT keyFlags)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
	mhold|=MK_LBUTTON;
	MBUT|=MK_LBUTTON;
	wininfo.mleftclicks++;
}

void messonrightmousedown(HWND hwnd, BOOL fDoubleClick, S32 x, S32 y, UINT keyFlags)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
	mhold|=MK_RBUTTON;
	MBUT|=MK_RBUTTON;
	wininfo.mrightclicks++;
}

void messonmiddlemousedown(HWND hwnd, BOOL fDoubleClick, S32 x, S32 y, UINT keyFlags)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
	mhold|=MK_MBUTTON;
	MBUT|=MK_MBUTTON;
	wininfo.mmiddleclicks++;
}

void messonleftmouseup(HWND hwnd, S32 x, S32 y, UINT keyFlags)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
	mhold&=~MK_LBUTTON;
}

void messonrightmouseup(HWND hwnd, S32 x, S32 y, UINT keyFlags)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
	mhold&=~MK_RBUTTON;
}

void messonmiddlemouseup(HWND hwnd, S32 x, S32 y, UINT keyFlags)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
	mhold&=~MK_MBUTTON;
}

void messonmousemove(HWND hwnd, S32 x, S32 y, UINT keyFlags)
{
	POINT p;
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
//	logger("mousemove mode %d last mode %d\n",mousemode,lastmousemode);
	if (wininfo.mousemode==MM_INFINITE && wininfo.lastmousemode==MM_NORMAL) { // switch to inf mode
		p.x=WX/2;
		p.y=WY/2;
		ClientToScreen((HWND)wininfo.MainWindow,&p);
//		logger("cp2\n");
		SetCursorPos(p.x,p.y);
	} else if (wininfo.mousemode==MM_NORMAL && wininfo.lastmousemode==MM_INFINITE) { // switch to norm mode
		p.x=MX;
		p.y=MY;
		ClientToScreen((HWND)wininfo.MainWindow,&p);
//		logger("cp3\n");
		SetCursorPos(p.x,p.y);
	} else if (wininfo.mousemode==MM_NORMAL) {
		MX=x;
		MY=y;
		if ((SX<WX || SY<WY)&&videoinfo.video_fullscreen && videoinfo.video_subdriver>0) {
			MX=(x+1)*WX/SX-1; // non primary directx video, uses different screen from primary
			MY=(y+1)*WY/SY-1;
		} else if (MX>=WX || MY>=WY) {
			if (MX>=WX) // standard clipping
				MX=WX-1;
			if (MY>=WY)
				MY=WY-1;
			setmousexy(MX,MY);
		}
	} else {
		if (x!=WX/2 || y!=WY/2) {
			MX+=x-WX/2;
			MY+=y-WY/2;
			p.x=WX/2;
			p.y=WY/2;
			ClientToScreen((HWND)wininfo.MainWindow,&p); // keep cursor in center of window
//			logger("cp4\n");
			SetCursorPos(p.x,p.y);
		}
	}
	wininfo.lastmousemode=wininfo.mousemode;
}

void messonmousewheel(S32 delta)
{
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse)
		return;
//	logger("wheel mouse moved %d\n",delta);
	wininfo.mz+=delta/120;
}

void setmousexy(S32 mx,S32 my)
{
	POINT p;
	if (wininfo.usedirectinput && !wininfo.dinput_nomouse) {
		MX=mx;
		MY=my;
		if (wininfo.mousemode==MM_NORMAL) {
			MX=range(0,MX,WX-1);
			MY=range(0,MY,WY-1);
		}
		return;
	}
	if (wininfo.mousemode==MM_NORMAL) {
		p.x=mx;
		p.y=my;
	} else {
		p.x=WX/2;
		p.y=WY/2;
	}
	ClientToScreen((HWND)wininfo.MainWindow,&p);
//	logger("cp5\n");
	SetCursorPos(p.x,p.y);
	MX=mx;
	MY=my;
	wininfo.lastmousemode=wininfo.mousemode;
}

// i'll run the mouse and keyboard in nonexclusive to continue receiving normal message events
// for mouseclick counts and buffered keyboardinput, and (pause key)
// i should use buffered directinput for this..
void input_init(S32 di)
{
	U32 i;
	DIPROPDWORD prop;
	HRESULT hr;
	logger("input init %d.............................................\n",di);
	if (di) {
		logger("    maindriver  0 'message based'\n");
		logger("  * maindriver  1 'directinput'\n");
    } else {
		logger("  * maindriver  0 'message based'\n");
		logger("    maindriver  1 'directinput'\n");
	}
	for (i=0;i<256;i++)
		wininfo.keystate[i]=0;
	if (di==wininfo.usedirectinput)
		return;
	if (di==0 && wininfo.usedirectinput==1)
		input_uninit();
	if ((di && !wininfo.dinput_nomouse) || videoinfo.video_fullscreen)
		useoscursor(0);
	else
		useoscursor(1);
	if (!di)
		return;
// init directinput
// first create the dinput
/*	hr=DirectInput8Create((HINSTANCE)wininfo.hinst,
		DIRECTINPUT_VERSION,
		&IID_IDirectInput8,
		(void*)(&di8), // should it be (void**)(&di8) ?? (type punned pointer stuff, aye)
		0);
*/
	hr = DirectInput8Create((HINSTANCE)wininfo.hinst, DIRECTINPUT_VERSION,
        IID_IDirectInput8, (void**)&di8, 0);
	if (hr!=DI_OK)
		errorexit("directinputcreateex %08x",hr);
// then the keyboard
	hr = di8->CreateDevice(GUID_SysKeyboard, &key8, 0);
//    hr=IDirectInput8_CreateDevice(di8,&GUID_SysKeyboard,(LPDIRECTINPUTDEVICE8 *)&key8,0);
	if (hr!=DI_OK)
		errorexit("directinputcreatedeviceex keyboard %08x",hr);
// keyboard format
//	hr=IDirectInputDevice8_SetDataFormat(key8,&c_dfDIKeyboard);
	hr=key8->SetDataFormat(&c_dfDIKeyboard);;
	if (hr!=DI_OK)
		errorexit("directinputcreatedeviceex keyboard format %08x",hr);
// set keyboard coop level
//	hr=IDirectInputDevice8_SetCooperativeLevel(key8,(HWND)wininfo.MainWindow,
	hr=key8->SetCooperativeLevel((HWND)wininfo.MainWindow,
		DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
	if (hr!=DI_OK)
		errorexit("directinputcreatedeviceex keyboard cooplevel %08x",hr);
// set keyboard buffer size
	prop.diph.dwSize=sizeof(DIPROPDWORD);
	prop.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	prop.diph.dwObj=0;
	prop.diph.dwHow=DIPH_DEVICE;
	prop.dwData=BUFFSIZE;
//	hr=IDirectInputDevice8_SetProperty(key8,DIPROP_BUFFERSIZE,(LPDIPROPHEADER)&prop);
	hr=key8->SetProperty(DIPROP_BUFFERSIZE,(LPDIPROPHEADER)&prop);
	if (hr!=DI_OK)
		errorexit("directinputcreatedeviceex keyboard setproperty %08x",hr);
// then the mouse
    if (!wininfo.dinput_nomouse) {
		hr = di8->CreateDevice(GUID_SysMouse, &mouse8, 0);
//		hr=IDirectInput8_CreateDevice(di8,&GUID_SysMouse,(LPDIRECTINPUTDEVICE8 *)&mouse8,0);
		if (hr!=DI_OK)
			errorexit("directinputcreatedeviceex mouse %08x",hr);
// mouse format
//		hr=IDirectInputDevice8_SetDataFormat(mouse8,&c_dfDIMouse2);
		hr=mouse8->SetDataFormat(&c_dfDIMouse2);
		if (hr!=DI_OK)
			errorexit("directinputcreatedeviceex mouse format %08x",hr);
// set mouse coop level
//		hr=IDirectInputDevice8_SetCooperativeLevel(mouse8,(HWND)wininfo.MainWindow,
//			DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
//		hr=IDirectInputDevice8_SetCooperativeLevel(mouse8,(HWND)wininfo.MainWindow,
//			DISCL_EXCLUSIVE|DISCL_FOREGROUND);
		hr=mouse8->SetCooperativeLevel((HWND)wininfo.MainWindow,
			DISCL_EXCLUSIVE|DISCL_FOREGROUND);
		if (hr!=DI_OK)
			errorexit("directinputcreatedeviceex mouse cooplevel %08x",hr);
		hr=mouse8->SetProperty(DIPROP_BUFFERSIZE,(LPDIPROPHEADER)&prop);
//		hr=IDirectInputDevice8_SetProperty(mouse8,DIPROP_BUFFERSIZE,(LPDIPROPHEADER)&prop);
		if (hr!=DI_OK)
			errorexit("directinputcreatedeviceex mouse setproperty %08x",hr);
		wininfo.mx=WX/2;
		wininfo.my=WY/2;
		wininfo.mbut=0;
	}
	wininfo.usedirectinput=1;
	Joy_Init();
}

void getdikdata(U8* dikbuff)
{
// aquire the keyboard data
	U32 i;
	DIDEVICEOBJECTDATA devicedata[BUFFSIZE],*dp;
	HRESULT hr;
	DWORD items;
	if (!key8 || !di8) {
//		logger("nodinput\n");
		memset(dikbuff,0,256);
		return;
	}
// keyboard....... try to read the data
//	hr=IDirectInputDevice8_GetDeviceState(key8,256,(LPVOID)dikbuff);
	items=BUFFSIZE;
//	hr=IDirectInputDevice8_GetDeviceData(key8,sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
	hr=key8->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
// noluck
	if (hr!=DI_OK && hr!=DIERR_INPUTLOST && hr!=DIERR_NOTACQUIRED)
		errorexit("directinputcreatedeviceex keyboard getdevicestate %08x",hr);
// try to reaquire the data
	if (hr!=DI_OK) {
		logger("reaquiring %08x\n",hr);
		hr=key8->Acquire();
//		hr=IDirectInputDevice8_Acquire(key8);
// noluck
		if (hr!=DI_OK && hr!=DIERR_OTHERAPPHASPRIO)
			errorexit("directinputcreatedeviceex keyboard aquire %08x",hr);
// we're in background, let's return all zeros
		if (hr!=DI_OK) {
			logger("other app has prio\n");
			memset(dikbuff,0,256);
			return;
		}
// aquire ok, try to read the data again
//		hr=IDirectInputDevice8_GetDeviceState(key7,256,(LPVOID)dikbuff);
//		if (hr!=DI_OK)
//			errorexit("directinputcreatedeviceex keyboard RE-getdevicestate %08x",hr);
		items=BUFFSIZE;
		hr=key8->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
//		hr=IDirectInputDevice8_GetDeviceData(key8,sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
		if (hr!=DI_OK)
			errorexit("directinputcreatedeviceex keyboard RE-getdevicedata %08x",hr);
	}
	for (i=0,dp=devicedata;i<items;i++,dp++) {
		U8 ofs;
		U8 val=(U8)(dp->dwData&0x80);
		ofs=dik2k[dp->dwOfs];
		if (val) {
			if (wininfo.keystate[K_LEFTSHIFT] || wininfo.keystate[K_RIGHTSHIFT])
				putkey(shifttab[ofs]);
			else {
//				logger("dinput putkey %d\n",ofs);
				putkey(ofs);
			}
			dikbuff[ofs]=1;
		} else
			dikbuff[ofs]=0;
	}
}

void getdimdata()
{
// aquire the mouse data
	DIDEVICEOBJECTDATA devicedata[BUFFSIZE],*dp;
	HRESULT hr;
	U32 i;
	DWORD items;
	if (!mouse8 || !di8) {
//		logger("nodinput\n");
		wininfo.mx=0;
		wininfo.my=0;
		wininfo.mz=0;
		wininfo.mbut=0;
		wininfo.mleftclicks=wininfo.mrightclicks=wininfo.mmiddleclicks=0;
		wininfo.mbut=mhold;
		return;
	}
// mouse......... try to read the data
//	hr=IDirectInputDevice8_Poll(mouse7);
//	hr=IDirectInputDevice8_GetDeviceState(mouse7,sizeof(DIMOUSESTATE),(LPVOID)mousestate);
	items=BUFFSIZE;
	hr=mouse8->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
//	hr=IDirectInputDevice8_GetDeviceData(mouse8,sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
// noluck
	if (hr!=DI_OK && hr!=DIERR_INPUTLOST && hr!=DIERR_NOTACQUIRED && hr!=DI_BUFFEROVERFLOW)
		errorexit("directinputcreatedeviceex mouse getdevicestate %08x",hr);
// try to reaquire the data
	if (hr!=DI_OK) {
		logger("reaquiring %08x\n",hr);
//		hr=IDirectInputDevice8_Acquire(mouse8);
		hr=mouse8->Acquire();
// noluck
		if (hr!=DI_OK && hr!=DIERR_OTHERAPPHASPRIO && hr!=DI_BUFFEROVERFLOW)
			errorexit("directinputcreatedeviceex mouse aquire %08x",hr);
// we're in background, let's return all zeros
		if (hr!=DI_OK) {
			logger("other app has prio\n");
//			memset(mousestate,0,sizeof(DIMOUSESTATE));
			return;
		}
// aquire ok, try to read the data again
//		hr=IDirectInputDevice8_GetDeviceState(mouse7,sizeof(DIMOUSESTATE),(LPVOID)mousestate);
	items=BUFFSIZE;
//	hr=IDirectInputDevice8_GetDeviceData(mouse8,sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
	hr=mouse8->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),devicedata,&items,0);
		if (hr!=DI_OK)
			errorexit("directinputcreatedeviceex mouse8 RE-getdevicestate %08x",hr);
	}
	for (i=0,dp=devicedata;i<items;i++,dp++) {
		U8 val=(U8)(dp->dwData&0x80);
//		logger("dp->dwOfs %d\n",dp->dwOfs);
		if (dp->dwOfs == DIMOFS_X) { // c++ doens't like switch/case for these
			wininfo.mx+=dp->dwData;
		} else if (dp->dwOfs == DIMOFS_Y) {
			wininfo.my+=dp->dwData;
		} else if (dp->dwOfs == DIMOFS_BUTTON0) {
            if (val) {
				wininfo.mleftclicks++;
				wininfo.mbut|=MK_LBUTTON;
				mhold|=MK_LBUTTON;
			} else {
				mhold&=~MK_LBUTTON;
			}
		} else if (dp->dwOfs == DIMOFS_BUTTON1) {
           if (val) {
				wininfo.mrightclicks++;
				wininfo.mbut|=MK_RBUTTON;
				mhold|=MK_RBUTTON;
			} else {
				mhold&=~MK_RBUTTON;
			}
		} else if (dp->dwOfs == DIMOFS_BUTTON2) {
           if (val) {
				wininfo.mmiddleclicks++;
				wininfo.mbut|=MK_MBUTTON;
				mhold|=MK_MBUTTON;
			} else {
				mhold&=~MK_MBUTTON;
			}
		} else if (dp->dwOfs == DIMOFS_Z) {
			wininfo.mz+=(S32)(dp->dwData)/120;
		}
/*		switch(dp->dwOfs) {
        case DIMOFS_X:
			wininfo.mx+=dp->dwData;
			break;
        case DIMOFS_Y:
			wininfo.my+=dp->dwData;
			break;
        case DIMOFS_BUTTON0:
            if (val) {
				wininfo.mleftclicks++;
				wininfo.mbut|=1;
				mhold|=1;
			} else
				mhold&=~1;
            break;
        case DIMOFS_BUTTON1:
            if (val) {
				wininfo.mrightclicks++;
				wininfo.mbut|=2;
				mhold|=2;
			} else
				mhold&=~2;
			break;
// this doesn't seem to work, any ideas???
		case DIMOFS_BUTTON2:
            if (val) {
				wininfo.mmiddleclicks++;
				wininfo.mbut|=0x10;
			mhold|=0x10;
			} else
				mhold&=~0x10;
			break;
		case DIMOFS_Z:
			wininfo.mz+=dp->dwData;
			break;
		} */
	}
	if (wininfo.mousemode==MM_NORMAL) {
		wininfo.mx=range(0,wininfo.mx,WX-1);
		wininfo.my=range(0,wininfo.my,WY-1);
	}
}

static U32 numjoysticks;

void input_uninit()
{
	U32 i;
	HRESULT hr;
	logger("input uninit\n");
	if (!wininfo.usedirectinput)
		return;
// release keyboard
	hr=key8->Unacquire();
//	hr=IDirectInputDevice8_Unacquire(key8);
	logger("unacquire keyboard %08x\n",hr);
	hr=key8->Release();
//	hr=IDirectInputDevice8_Release(key8);
	key8=0;
	logger("dikeydev8 keyboard release %08x\n",hr);
// release mouse
	if (!wininfo.dinput_nomouse) {
		hr=mouse8->Unacquire();
//		hr=IDirectInputDevice8_Unacquire(mouse8);
		logger("unacquire mouse %08x\n",hr);
		hr=mouse8->Release();
//		hr=IDirectInputDevice8_Release(mouse8);
		mouse8=0;
		logger("dikeydev8 mouse release %08x\n",hr);
	}
// release joy
	for (i=0;i<MAXJOYSTICKS;++i) {
		if (joy8[i]) {
			hr=joy8[i]->Unacquire();
//			hr=IDirectInputDevice8_Unacquire(joy8[i]);
			if(FAILED(hr))
				logger("unacquire joy[%d] fails.\n",i);
			else
				logger("unacquire joy[%d] success!\n",i);

			hr=joy8[i]->Release();
//			hr=IDirectInputDevice8_Release(joy8[i]);
			if(FAILED(hr))
				logger("dikeydev8 joy[%d] release fails.\n",i);
			else {
				joy8[i]=0;
				logger("dikeydev8 joy[%d] release success!\n",i);
			}
		}
	}
	numjoysticks=0;
	memset(jdata,0,sizeof(jdata));
// release dinput
	hr=IDirectInput8_Release(di8);
	di8=0;
	logger("dinput release %08x\n",hr);
	wininfo.usedirectinput=0;
}

/////////////////////////// All about joysticks //////////////////////////////////////////////////////
struct joydata jdata[MAXJOYSTICKS];

// joystick
//#define MAXJOYSTICKS 2
static DIJOYSTATE2 joystate[MAXJOYSTICKS];

S32 CALLBACK Joy_enumDevice(const DIDEVICEINSTANCE* instance, VOID* context)
{
	HRESULT hr;
	//InputBase* IB = (InputBase*)context;
	//InputDevice *ID = IB->GetInputDevice();
	//IDirectInputDevice8* td = IB->GetBaseDevice();
	// Obtain an interface to the enumerated joystick.
//	hr = di8->CreateDevice(,
//	hr=IDirectInput8_CreateDevice(di8,&instance->guidInstance,&joy8[numjoysticks], 0);
	hr = di8->CreateDevice(instance->guidInstance, &joy8[numjoysticks], 0);
	//IB->SetBaseDevice(td);
	// If it failed, then we can't use this joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if (FAILED(hr)) {
		//G.Error = G.ERROR_FAIL;
		//G.LogError( "Input object device cretion failed (JoyStick) - User Pulled?" );
//		errorexit("Input object device cretion failed (JoyStick) - User Pulled?");
		return DIENUM_CONTINUE;
	}
	++numjoysticks;
	if (numjoysticks>=MAXJOYSTICKS)
		return DIENUM_STOP;
	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_CONTINUE;
}

U32 Joy_Init()
{
//	hasjoy = 0;

	DIDEVCAPS mCaps;
    U32 i;
	HRESULT hr = S_OK;
	hr = IDirectInput8_EnumDevices(di8,DI8DEVCLASS_GAMECTRL, Joy_enumDevice, 0, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
		return 0;
/*
	if(!joy8) {
        logger("Couldn't find a JoyStick\n" );
		return 0;
	}
*/
	for (i=0;i<numjoysticks;++i) {
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		hr = joy8[i]->SetDataFormat(&c_dfDIJoystick2);
//		hr = IDirectInputDevice8_SetDataFormat(joy8[i],&c_dfDIJoystick2);
		if (FAILED(hr)) {
			return hr!=0;
		}
	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
		hr = joy8[i]->SetCooperativeLevel((HWND)wininfo.MainWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
//		hr = IDirectInputDevice8_SetCooperativeLevel(joy8[i],(HWND)wininfo.MainWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if (FAILED(hr)) {
			return hr!=0;
		}
		mCaps.dwSize = sizeof(DIDEVCAPS);
		hr = joy8[i]->GetCapabilities(&mCaps );
//		hr = IDirectInputDevice8_GetCapabilities(joy8[i], &mCaps );
		if( FAILED( hr )) {
			logger( "Filed to get caps (JoyStick)\n" );
			return 0;
		}
        if( !mCaps.dwFlags & DIDC_ATTACHED ) {
			logger( "Not attached (JoyStick)\n" );
			//G.Error = G.ERROR_NOTFOUND;
			return 0;
		}
	// Enumerate the axes of the joyctick and set the range of each axis. Note:
	// we could just use the defaults, but we're just trying to show an example
	// of enumerating device objects (axes, buttons, etc.).
/*	if (FAILED(hr = joy7->EnumObjects(Joy_enumAxesCallback, wininfo.MainWindow, DIDFT_ALL))) {
		return hr;
	}
*/
	//U8 msg[256];
	//hasjoy = 1;
	//sprintf(msg,"Slider Count %d, POVCount %d\n",GetSliderCount(),GetPOVCount());
	//G.LogError( msg );
	   joy8[i]->Acquire();
//	   IDirectInputDevice8_Acquire(joy8[i]);
	}
    //this->Clear();
    //this->StoreInVector();
    return 1;
}

#define NSJ_REPDELAY 8
#define NSJ_REPPERIOD 3

#define MAPHAT
#define MAPANALOG

void Joy_GetState(U32 joynum,DIJOYSTATE2* js,struct joydata* jd)
{
	S32 i;
	U32 shf;
	U32 state=0;
#ifdef MAPANALOG
	S32 jx,jy,distsq;
#endif
// do last
	U32 newsjoyhat;
	jd->newsjoyl=jd->newsjoy;
	for (i=0;i<MAXAXIS;++i)
		jd->newsjoyaxisl[i]=jd->newsjoyaxis[i];
	newsjoyhat = js->rgdwPOV[0] & 0xffff;
// copy over the axis info too
	jd->newsjoyaxis[0]=js->lX;
	jd->newsjoyaxis[1]=js->lY;
	jd->newsjoyaxis[2]=js->lZ;
	jd->newsjoyaxis[3]=js->lRz;
	for (i=0;i<MAXJOYBITSRAW;++i) {	// okay 12 buttons, pack them into 'state'
		state += js->rgbButtons[i]>>7<<i;
	}
#ifdef MAPHAT
// map the pov to the buttons too
	if (newsjoyhat != 0xffff) {
		if (wininfo.use8way) {
			if (newsjoyhat<6000 || newsjoyhat>=30000)
				state |= NSJ_UP;
			if (newsjoyhat>=3000 && newsjoyhat<15000)
				state |= NSJ_RIGHT;
			if (newsjoyhat>=12000 && newsjoyhat<24000)
				state |= NSJ_DOWN;
			if (newsjoyhat>=21000 && newsjoyhat<33000)
				state |= NSJ_LEFT;
		} else {
			if (newsjoyhat<3000 || newsjoyhat>=33000)
				state |= NSJ_UP;
			if (newsjoyhat>=6000 && newsjoyhat<12000)
				state |= NSJ_RIGHT;
			if (newsjoyhat>=15000 && newsjoyhat<21000)
				state |= NSJ_DOWN;
			if (newsjoyhat>=24000 && newsjoyhat<30000)
				state |= NSJ_LEFT;
		}
	}
#endif // MAPHAT
#ifdef MAPANALOG
	jx=(jd->newsjoyaxis[0]-32768)>>1;
	jy=(jd->newsjoyaxis[1]-32768)>>1;
	distsq=jx*jx+jy*jy;
#define THRESH (32768*3/4/2)
	if (distsq>=THRESH*THRESH) {
		S32 x,y;
		if (wininfo.use8way) {
			x=abs(jx);
			y=abs(jy);
			y<<=8;
			if (x*618>y) {
				if (jx>=0)
					state |= NSJ_RIGHT;
				else
					state |= NSJ_LEFT;
			}
			if (x*106<y) {
				if (jy>0)
					state |= NSJ_DOWN;
				else
					state |= NSJ_UP;
			}
		} else {
			x=abs(jx);
			y=abs(jy);
			if (x>y)
				if (jx>=0)
					state |= NSJ_RIGHT;
				else
					state |= NSJ_LEFT;
			else
				if (jy>0)
					state |= NSJ_DOWN;
				else
					state |= NSJ_UP;
		}
	}
#endif // MAPANALOG
	jd->newsjoy=state;
// do delta and rep
	jd->newsjoyd = jd->newsjoy & ~jd->newsjoyl;
	for (i=0;i<MAXAXIS;++i)
		jd->newsjoyaxisd[i]=jd->newsjoyaxis[i]-jd->newsjoyaxisl[i];
// and rep
	shf=jd->newsjoy;
	for (i=0;i<MAXJOYBITS;++i) {
		if (shf&1) {
			++(jd->repcounters[i]);
		} else
			jd->repcounters[i]=0;
		shf>>=1;
	}
	jd->newsjoyrep=0;
	for (i=MAXJOYBITS-1;i>=0;--i) {
		S32 b=0;
		if (jd->repcounters[i]==1)
			b=1;
		if (jd->repcounters[i]==NSJ_REPDELAY) {
			jd->repcounters[i]-=NSJ_REPPERIOD;
			b=1;
		}
		jd->newsjoyrep<<=1;
		jd->newsjoyrep+=b;
	}
}

void Joy_Update()
{
	U32 i;
	HRESULT hr;
	for (i=0;i<numjoysticks;++i) {
		if(!joy8[i])
			continue;
// Poll the device to read the current state
	   hr = joy8[i]->Poll();
//	   hr = IDirectInputDevice8_Poll(joy8[i]);
		if( FAILED(hr) )  {
			// DInput is telling us that the input stream has been
			// interrupted. We aren't tracking any state between polls, so
			// we don't have any special reset that needs to be done. We
			// just re-acquire and try again.
			hr = joy8[i]->Acquire();
//			hr = IDirectInputDevice8_Acquire(joy8[i]);
			while( hr == DIERR_INPUTLOST )
//				hr = IDirectInputDevice8_Acquire(joy8[i]);
				hr = joy8[i]->Acquire();
			// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
			// may occur when the app is minimized or in the process of
			// switching, so just try again later
			//return S_OK;
		}
	// Get the input's device state
		hr = joy8[i]->GetDeviceState(sizeof(DIJOYSTATE2), &joystate[i] );
//		hr = IDirectInputDevice8_GetDeviceState(joy8[i], sizeof(DIJOYSTATE2), &joystate[i] );
		Joy_GetState(i,&joystate[i],&jdata[i]);
	}
}
