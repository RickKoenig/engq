#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
//#ifdef __GNUC__
//#include <vfwms.h>
//#else
#include <vfw.h>
//#endif

static HWND hWndC;
static S32 ncams;
static S32 cidx=0; // which camera to use
static U8* vbuff[2];
static struct bitmap32* cbm32;
U32 fourccidx;
static CAPTUREPARMS cp;
static CAPSTATUS cs;
static CAPDRIVERCAPS cc;
static volatile S32 cpframe,bitsperpix,twof,newpic,somepic;

#define T_VR ((S32)(1.4075*256))
#define T_UG ((S32)(-.3455*256))
#define T_VG ((S32)(-.7169*256))
#define T_UB ((S32)(1.7790*256))

static S32 CALLBACK frameproc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	cpframe++;
	if (!vbuff[0] || !lpVHdr || !cbm32)
		return 1;
	if ((S32)lpVHdr->dwBytesUsed*8!=cbm32->size.x*cbm32->size.y*bitsperpix)
		return 1;
	memcpy(vbuff[twof],lpVHdr->lpData,lpVHdr->dwBytesUsed);
	twof^=1;
	newpic=1;
	somepic=1;
	return 1;
}

static void convertrgb(U8* src,struct bitmap32* b32)
{
	S32 i,j;
	C32* d;
	C32 v;
	if (bitsperpix==24) {
		S32 b3=b32->size.x*3;
		d=b32->data;
		for (j=0;j<b32->size.y;j++) {
			U8* sc=src+(b32->size.y-1-j)*b3; // bottom up!
			for (i=0;i<b32->size.x;i++) {
				v.b=*sc++;
				v.g=*sc++;
				v.r=*sc++;
				v.a=0xff;
				*d++=v;
			}
		}
	} else if (bitsperpix==16) { // 555
		d=b32->data;
		for (j=0;j<b32->size.y;j++) {
			U16* sc=(U16*)src+(b32->size.y-1-j)*b32->size.x; // bottom up!
			for (i=0;i<b32->size.x;i++) {
				U16 vs=*sc++;
				v.b=(vs&0x1f)<<3;
				v.g=((vs>>5)&0x1f)<<3;
				v.r=((vs>>10)&0x1f)<<3;
				v.a=0xff;
				*d++=v;
			}
		}
	}
}

static C32 yuv2_C32(S32 y,S32 u,S32 v)
{
	C32 ret;
	S32 r,g,b;
	u-=128;
	v-=128;
	r=y+((v*T_VR)>>8);
	g=y+((u*T_UG+v*T_VG)>>8);
	b=y+((u*T_UB)>>8);
	r=range(0,r,255);
	g=range(0,g,255);
	b=range(0,b,255);
	ret.r=r;
	ret.g=g;
	ret.b=b;
	ret.a=0xff;
	return ret;
}

// 16 bit packed
// y0 u0 y1 v0 2 by 1
static void convertyuy2(U8* sc,struct bitmap32* b32)
{
	S32 prod=b32->size.x*b32->size.y;
	C32* d=b32->data;
	while(prod) {
		*d++=yuv2_C32(sc[0],sc[1],sc[3]);
		*d++=yuv2_C32(sc[2],sc[1],sc[3]);
		sc+=4;
		prod-=2;
	}
}

// 9 bit planar
// y's v's u's ( v's and u's are sampled once very 4 by 4)
static void convertyvu9(U8* sc,struct bitmap32* b32)
{
	S32 i,j;
	S32 prod=b32->size.x*b32->size.y;
	C32* d=b32->data;
	U8* svs=sc+prod;
	U8* sus=svs+(prod>>4);
	U8* su,*sv;
	for (j=0;j<b32->size.y;j++) {
		su=sus;
		sv=svs;
		for (i=0;i<b32->size.x;i++) {
			*d++=yuv2_C32(sc[0],su[0],sv[0]);
			if ((i&3)==3) {
				su++;
				sv++;
			}
			sc++;
			prod--;
		}
		if ((j&3)==3) {
			sus+=b32->size.x>>2;
			svs+=b32->size.x>>2;
		}
	}
}

// 12 bit planar
// y's u's v's ( u's and v's are sampled once very 2 by 2)
//static void convertiyuv(U8* sc,struct bitmap32 *b32) // same as i420
static void converti420(U8* sc,struct bitmap32* b32)
{
	S32 i,j;
	S32 prod=b32->size.x*b32->size.y;
	C32* d=b32->data;
	U8* sus=sc+prod;
	U8 *svs=sus+(prod>>2);
	U8 *su,*sv;
	for (j=0;j<b32->size.y;j++) {
		su=sus;
		sv=svs;
		for (i=0;i<b32->size.x;i++) {
			*d++=yuv2_C32(sc[0],su[0],sv[0]);
			if ((i&1)==1) {
				su++;
				sv++;
			}
			sc++;
			prod--;
		}
		if ((j&1)==1) {
			sus+=b32->size.x>>1;
			svs+=b32->size.x>>1;
		}
	}
}

#define MAKE4CC(a,b,c,d) ((a)+((b)<<8)+((c)<<16)+((d)<<24))
struct fourccassoc {
	U32 fcc;
	void (*fun)(U8* src,struct bitmap32* b16);
	char* name;
};
static struct fourccassoc fcca[]={
	{0,convertrgb,"RGB 0"},
	{MAKE4CC('R','G','B',' '),convertrgb,"RGB"},
	{MAKE4CC('Y','U','Y','2'),convertyuy2,"YUY2"},
	{MAKE4CC('I','Y','U','V'),converti420,"IYUV"},
	{MAKE4CC('I','4','2','0'),converti420,"I420"},
	{MAKE4CC('Y','V','U','9'),convertyvu9,"YVU9"},
};
#define NFCC (sizeof(fcca)/sizeof(fcca[0]))

S32 vcaminit()
{
	S32 res;
	C8 name[50],desc[50];
	S32 i;
// enum camera devices
	ncams=0;
	if (hWndC)
		return 0;
	for (i=0;i<=9;i++)
		if (capGetDriverDescription(i,name,50,desc,50)) {
			logger("camera driver [%2d] = '%s' '%s'\n",i,name,desc);
			if (i==0)
				ncams++;
		}
	if (ncams==0)
		return 0;
	hWndC = capCreateCaptureWindow (	"My Own Capture Window",
				WS_CHILD  ,	0, 0, 160, 120, (HWND)wininfo.MainWindow , 0);
	if (hWndC==0) {
		logger("can't create cap window\n");
		return 0;
	}
	res=capDriverConnect (hWndC, cidx);
	if (!res) {
		logger("can't connect to vidcap driver\n");
		DestroyWindow(hWndC);
		hWndC=0;
		return 0;
	}
	res=capDriverGetCaps(hWndC,&cc,sizeof(cc));
	if (res==0) {
		logger("can't get vidcap driver caps\n");
		capDriverDisconnect(hWndC);
		DestroyWindow(hWndC);
		hWndC=0;
		return 0;
	}
	res=capCaptureGetSetup(hWndC,&cp,sizeof(cp));
	if (res==0) {
		logger("can't get vid setup\n");
		capDriverDisconnect(hWndC);
		DestroyWindow(hWndC);
		hWndC=0;
		return 0;
	}
	cp.vKeyAbort=0;                  // Virtual key causing abort
	cp.fAbortLeftMouse=0;            // Abort on left mouse?
    cp.fAbortRightMouse=0;           // Abort on right mouse?
//  cp.fLimitEnabled=0;              // Use wTimeLimit?
	cp.fYield=1;
	cp.fCaptureAudio=0;
//	cp.wStepCaptureAverageFrames=5;
//	cp.dwRequestMicroSecPerFrame=200000;
	res=capCaptureSetSetup(hWndC,&cp,sizeof(cp));
	if (res==0) {
		logger("can't set vid setup\n");
		capDriverDisconnect(hWndC);
		DestroyWindow(hWndC);
		hWndC=0;
		return 0;
	}
//	capCaptureSequenceNoFile(hWndC);
//	capCaptureSequence (hWndC);
	res=capGetStatus(hWndC,&cs,sizeof(cs));
	if (res==0) {
		logger("can't get status\n");
		capDriverDisconnect(hWndC);
		DestroyWindow(hWndC);
		hWndC=0;
		return 0;
	}
	return 1;
}

S32 vcamstartstream()
{
	U32 fourcc;
	C8 str[200];
	BITMAPINFOHEADER *bmi;
	S32 bmis;
	S32 res;
	if (!hWndC)
		return 0;
	if (cbm32)
		return 1;
	fourccidx=~0U;
//	capGrabFrame(hWndC);
	bmis = capGetVideoFormatSize(hWndC);
	if (bmis==0) {
		logger("can't get vid format size\n");
		return 0;
	}
//	bmi = (BITMAPINFOHEADER*)memzalloc(bmis);
	bmi = new(BITMAPINFOHEADER);
	memset(bmi,0,sizeof(BITMAPINFOHEADER));
	res=capGetVideoFormat(hWndC, bmi,bmis);
	if (res==0) {
		logger("can't get vid format\n");
//		memfree(bmi);
		delete bmi;
		return 0;
	}
	fourcc=bmi->biCompression;
	for (fourccidx=0;fourccidx<NFCC;fourccidx++)
		if (fourcc==fcca[fourccidx].fcc)
			break;
	if (fourccidx==NFCC || (fourccidx<=1 && bmi->biBitCount!=16 && bmi->biBitCount!=24)) {
		sprintf(str,"can't set 4cc");
		fourccidx=~0U;
		logger("%s\n",str);
//		memfree(bmi);
		delete bmi;
		return 0;
	}
	if (bmi->biPlanes!=1 ||
	  8*bmi->biSizeImage !=bmi->biWidth*(U32)abs(bmi->biHeight)*bmi->biBitCount) {
		sprintf(str,"can't set vid format w %d, h %d, comp %08x, bits %d",
			(S32)bmi->biWidth,(S32)bmi->biHeight,(S32)bmi->biCompression,bmi->biBitCount);
		fourccidx=~0U;
		logger("%s\n",str);
//		memfree(bmi);
		delete bmi;
		return 0;
	}
	cbm32=bitmap32alloc(bmi->biWidth,abs(bmi->biHeight),C32GREEN);
	somepic=0;
	bitsperpix=bmi->biBitCount;
//	vbuff[0]=(U8*)memzalloc(cbm32->size.x*cbm32->size.y*bitsperpix/8);
	vbuff[0]=new U8[cbm32->size.x*cbm32->size.y*bitsperpix/8];
	memset(vbuff[0],0,cbm32->size.x*cbm32->size.y*bitsperpix/8);
//	vbuff[1]=(U8*)memzalloc(cbm32->size.x*cbm32->size.y*bitsperpix/8);
	vbuff[1]=new U8[cbm32->size.x*cbm32->size.y*bitsperpix/8];
	memset(vbuff[1],0,cbm32->size.x*cbm32->size.y*bitsperpix/8);
//	memfree(bmi);
	delete bmi;
	twof=cpframe=0;
	res=capSetCallbackOnVideoStream(hWndC,frameproc);
	if (res==0) {
		logger("can't set vid callback 1\n");
		Sleep(1500); // try again later...
		res=capSetCallbackOnVideoStream(hWndC,frameproc);
		if (res==0) {
			logger("can't set vid callback 2\n");
			vcamstopstream();
			return 0;
		}
	}
	res=capCaptureSequenceNoFile(hWndC);
	if (res==0) {
		logger("can't cap sequence no file\n");
		vcamstopstream();
		return 0;
	}
	return 1;
}

struct bitmap32* vcamgrabframe()
{
	S32 cf;
	if (!cbm32)
		return 0;
	if (!somepic)
		return 0;
	if (!newpic)
		return cbm32;
	newpic=0;
	cf=twof^1;
	(*fcca[fourccidx].fun)(vbuff[cf],cbm32);
	return cbm32;
}

const C8* getformatname()
{
	if (fourccidx < 0)
		return "format < 0";
	else if (fourccidx >= NFCC)
		return "format >= NFCC";
	return fcca[fourccidx].name;
}

S32 vcamstopstream()
{
	S32 res;
	if (!hWndC)
		return 0;
	if (!cbm32)
		return 0;
	res=capCaptureStop(hWndC);
	res=capSetCallbackOnVideoStream(hWndC,0); // this might crash, thread issue
	if (cbm32)
		bitmap32free(cbm32);
	cbm32=0;
	if (vbuff[0]) {
//		memfree(vbuff[0]);
		delete[] vbuff[0];
//		memfree(vbuff[1]);
		delete[] vbuff[1];
		vbuff[0]=vbuff[1]=0;
	}
	fourccidx=~0U;
	return 1;
}

S32 vcamexit()
{
	S32 res;
	if (!hWndC)
		return 0;
	res=capDriverDisconnect(hWndC);
	DestroyWindow(hWndC);
	hWndC=0;
	return 1;
}

void vcamformat()
{
	if (hWndC) {
		while(ShowCursor(1)<0)
			;
		capDlgVideoFormat(hWndC);
	}
}

void vcamsource()
{
	if (hWndC) {
		while(ShowCursor(1)<0)
			;
		capDlgVideoSource(hWndC);
	}
}

void vcamcompression()
{
	if (hWndC) {
		while(ShowCursor(1)<0)
			;
		capDlgVideoCompression(hWndC);
	}
}

void vcamdisplay()
{
	if (hWndC) {
		while(ShowCursor(1)<0)
			;
		capDlgVideoDisplay(hWndC);
	}
}

#if 0
static volatile S32 cpframe,newpic,twof;

static S32 bitsperpix;
static U32 fourcc;
static S32 fourccidx=-1;
// some 4 cc codes


static struct tree *roottree,*testscene;
//extern struct viewport mainvp;
static struct tree *thebox;
static struct mat *boxmat;
static struct texture *tx,*tx2;

void vidcap_init()
{
// setup state
	roottree=alloctree(100,0);
	mystrncpy(roottree->name,"testroottree",NAMESIZE);
	video_setupwindow(800,600,565);
	pushandsetdir("vc");
	rl=loadres("vcres.txt");
	testscene=loadscene("vid.mxs");
	linkchild(testscene,roottree);
//	calclightonce(testscene);
	popdir();

	settreedraworder(roottree);
	mainvp.backcolor=frgblightblue;
	mainvp.zfront=.5f;
	mainvp.zback=25000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.roottree=roottree;
	mainvp.camattach=getlastcam(); // from last scene loaded
	mainvp.camzoom=3.2f; // it'll getit from tree camattach if you have one
	setviewportsrc(&mainvp); // from last scene loaded
	mainvp.flags=VP_CLEARWB;
//	logviewport(&mainvp,OPT_ALL);

	thebox=findtreenamerec(roottree,"Box01.mxo");
	if (!thebox)
		errorexit("can't find the box");
	boxmat=findmaterial(thebox,"Material #1");
	if (!boxmat)
		errorexit("can't find the box material");
	tx2=boxmat->thetex;
	if (tx2)
		freetexture(tx2);
	if (texavail[TEX565CK])
		tx=buildtexture("testtex",128,128,TEX565CK);
	else if (texavail[TEX555CK])
		tx=buildtexture("testtex",128,128,TEX555CK);
	else
		errorexit("can't set texture on box");
	boxmat->thetex=tx;
	logviewport(&mainvp,OPT_ALL);
}

void vidcap_proc()
{
	struct bitmap16 *t16;
	S32 i;
	struct rmessage rmess;
	if (!ncams)
		popstate();
	checkres(rl);
	while(getresmess(rl,&rmess)) {
		switch(rmess.id) {
		case QUIT:
			popstate();
			break;
		case VCSTART:
			vcaminit()
			break;
		case GRABF:
			if (hWndC)
				vcgrab();
			break;
		case VCSTOP:
			vcamstop();
			setresname(rl,TSTATUS,"done capture");
			break;
		case VCQUIT:
			vcamstopstream();
			vcamstop();
			setresname(rl,TSTATUS,"done with driver");
			break;
		case PVIDDISPLAY:
			vcamdisplay();
			break;
		case PVIDCOMP:
			vcamcompression();
			break;
		case PVIDFORMAT:
			vcamformat();
			break;
		case PVIDSOURCE:
			vcamsource();
			break;
		case P3D:
			in3d^=1;
			break;
		}
	}
	if (cbm16) {
		if (in3d) {
			S32 minx,miny;
			if (tx->texformat==TEX555CK)
				convertbitmap(555);
			else
				convertbitmap(565);
			t16=locktexture(tx);
			minx=mymin(t16->x,cbm16->x);
			miny=mymin(t16->y,cbm16->y);
			clipblit16(cbm16,t16,
				(cbm16->x-minx)>>1,(cbm16->y-miny)>>1,(t16->x-minx)>>1,(t16->y-miny)>>1,minx,miny);
//			clipline16(t16,0,0,127,0,hiwhite);
//			clipline16(t16,127,0,127,127,higreen);
//			clipline16(t16,0,0,0,127,hired);
//			clipline16(t16,0,127,127,127,hiyellow);
			unlocktexture(tx);
		} else
			convertbitmap(BPP);
	}
	video_lock();
	cliprect16(B16,0,0,WX-1,WY-1,hiblue);
	if (cbm16 && !in3d)
		clipblit16(cbm16,B16,0,0,(WX-cbm16->x)/2,(WY-cbm16->y)/2,cbm16->x,cbm16->y);
	drawres(rl);
	outtextxyf16(B16,10,10,hiwhite,"vidcap frame %6d, capfrm %d, in3d %d",frame,cpframe,in3d);
	for (i=1;i<NFCC;i++) {
		U8 smstr[5];
		smstr[4]='\0';
		memcpy(smstr,&fcca[i].fcc,4);
		outtextxyf16(B16,750,50+i*10,hiwhite,"%s",smstr);
	}
	if (fourccidx>=0)
		outtextxyf16(B16,742,50+fourccidx*10,hiwhite,"*");
	video_unlock();
	if (in3d) {
		doanims(roottree);
// hi level camera
		doflycam(&mainvp);
// prepare scene
		buildtreematrices(roottree); //roottree,camtree);
// draw scene
		video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
		video_drawscene(&mainvp);
		video_endscene(&mainvp); // nothing right now
	}
	frame++;
}

#endif
