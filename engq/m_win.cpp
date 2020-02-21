#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

LONG WINAPI crashfunc(LPEXCEPTION_POINTERS ep)
{
	logger("hit an exception at %08x\n",ep->ExceptionRecord->ExceptionAddress);
	logger("exception code: %08x\n",ep->ExceptionRecord->ExceptionCode);
	logger("exception flags: %08x\n",ep->ExceptionRecord->ExceptionFlags);
	loggerexit();
	loggerclose();
	return EXCEPTION_EXECUTE_HANDLER;
}

struct wi wininfo;

static S32 lastlogcheck()
{
	script sc(getloggername());
	S32 i;
	if (!sc.num())
		return 1;
	for (i=0;i<sc.num();++i)
		if (!my_stricmp(sc.idx(i).c_str(),"MEMCLEAN"))
			return 1;
	C8* logdata=fileload(getloggername(),100);
	strcat(logdata,"\r\nadded for next run ... MEMCLEAN\r\n");
	FILE* fw;
	fw=fopen2(getloggername(),"wb");
	fwrite(logdata,1,strlen(logdata),fw);
	fclose(fw);
	delete[] logdata;
	return 0;
}

S32 alreadyrunning()
{
	HANDLE mh;
	DWORD err;
	mh=CreateMutex(NULL,0,"enginemtest");
	err=GetLastError();
	return err;
}

void parsecommandline(const script& sc)
{
	S32 i;
	S32 scn = sc.num();
	if (scn & 1) {
		for (i=0;i<sc.num();++i) {
			error("commandline arg %d = '%s'",i,sc.idx(i).c_str());
		}
		errorexit("odd number of command line args");
	}
	for (i=0;i<sc.num();i+=2) {
		const string& s = sc.idx(i);
		if (!s.length() || s[0] != '-')
			errorexit("command option '%s' not valid",s.c_str());
	}
	for (i=0;i<sc.num();++i)
		logger("command script %3d = '%s'\n",i,sc.idx(i).c_str());
	loadconfigscript(sc,wininfovars,nwininfovars);
}

////////// the main main
S32 WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,S32 nCmdShow)
{
	HANDLE hproc;
	U32 ret;
	U32 cw;
	wininfo.hinst=(U32)hInst;
	wininfo.isalreadyrunning=alreadyrunning()!=0;
//	SetUnhandledExceptionFilter(crashfunc);
	meminit();
//	setmemverboselevel(1);
	dirinit();
	setdir(""); // log file in data directory
#if 1
	if (!wininfo.isalreadyrunning)
		if (!lastlogcheck())
			errorexit("check last log file for mem leaks");
#endif
	loggerinit();
#ifdef NDEBUG
	logger("release mode\n");
#else
	logger("debug mode\n");
#endif
	logger("commandline is '%s'\n",lpszCmdParam);
	script cls(lpszCmdParam,true);
	logger("datadir is '%s'\n",wininfo.datadir);
	wininfo.fpswanted=INITFPS;
	defaultfpucontrol();
	pushandsetdir("config");
	script test = script("test.txt");
	logger("Test script input chars = %d, num tokens = %d, num chars = %d\n",test.inchars(),test.num(),test.numchars());
	loadconfigfile("config.txt",wininfovars,nwininfovars);
	parsecommandline(cls);
	logger("global name = '%s'\n",wininfo.globalname.c_str());
	popdir();
	if (wininfo.releasemode==47)
		wininfo.releasemode=0;
	else
		wininfo.releasemode=1;
	if (wininfo.releasemode) {
//		enablenumkeychangestate=0;
		wininfo.enabledebprint=0;
	}
	load_device();
	debp_init();
	if (!wininit())
		errorexit("can't register window");
	set_device();
//	video_init(wininfo.startvideomaindriver,wininfo.startvideosubdriver);
//	input_init(wininfo.startusedirectinput);
//	audio_init(wininfo.startaudiomaindriver,wininfo.startaudiosubdriver,wininfo.startaudiomicsubdriver);
	timerinit(); // now does checkmhz
	avginit();
	logger("^^^^^^^^^    begin main init    ^^^^^^^^^\n");
	maininit();
	logger("VVVVVVVVV    end main init    VVVVVVVVVVV\n");
/*
	checkmhz(); // for perf analyser
	MX=WX/2;
	MY=WY/2;
*/
	if (wininfo.hiprioritywanted) {
		hproc=GetCurrentProcess();
		ret=SetPriorityClass(hproc,HIGH_PRIORITY_CLASS);
		logger("setpriorityclass to high returns a %d\n",ret);
	}
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
	winproc();
	while(1) {
		perf_start(MAINLOOP);
		perf_start(STATEPROC);
		if (!mainproc()) {
			perf_end(STATEPROC);
			perf_end(MAINLOOP);
			break;
		}
		perf_end(STATEPROC);
		winproc();
		perf_end(MAINLOOP);
		perf_frame();
	}
	avgfree();
	logger("^^^^^^^^^    begin main exit    ^^^^^^^^^\n");
	mainexit();
	logger("VVVVVVVVV    end main exit    VVVVVVVVVVV\n");

	save_device();
	audio_uninit();
	input_uninit();
	video_uninit();
	debp_exit();
	direxit();
	READCW(cw);
	logger("final fpu control is at $%08x\n",cw);
	argcvexit();
	loggerexit();
//	error("here i am");
	winexit();
	memexit();
	return 0;
}
