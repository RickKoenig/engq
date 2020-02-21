#define INCLUDE_WINDOWS
//#define D2_3D
//#define RES3D // alternate shape class
#include <m_eng.h>
#include <l_misclibm.h>
//#include "u_testasmmacros.h"
#include <process.h>
#include "u_searcher_mandle_thread.h"
#include "u_mandl.h"

namespace u_s_searcher {
HANDLE hThread;
U32 threadID;
HANDLE gettowork;
bool threadquit;
U32 milli0;
S32 runinbackgroundsave;

//S32 taskin,taskout; // inbox outbox
U32 Counter;
U32 totalcount;
list<string> tasks;
CRITICAL_SECTION cs;
const S32 MAXDIFFTASKS=200;
set<string> tasks_pending;
set<string> tasks_done;
//bool tasks_pending[MAXDIFFTASKS];
//bool tasks_done[MAXDIFFTASKS];

void _tasklock()
{
	EnterCriticalSection(&cs);
}

void _taskunlock()
{
	LeaveCriticalSection(&cs);
}

unsigned __stdcall workerthread( void* pArguments )
{
	logger( "WT: start second thread...\n" );
_tasklock();
	S32 tasksleft = tasks.size();
_taskunlock();
	while( !threadquit || tasksleft>0) {
//		logger("WT: before waitforsingleobject %d\n",getmillisec()-milli0);
		WaitForSingleObject(gettowork,INFINITE);
//		logger("WT: after waitforsingleobject %d\n",getmillisec()-milli0);
		wininfo.runinbackground = 1;
_tasklock();
		tasksleft = tasks.size();
_taskunlock();
		while (tasksleft>0 /* && !threadquit */) {
			string t;
_tasklock();
			t=*tasks.begin();
//			logger("WT: starting task '%s' size %d at %d\n",t.c_str(),tasksleft,getmillisec()-milli0);
			tasksleft = tasks.size();
_taskunlock();
			// task start
//#define TESTDELAY
//#define TEXTNAME
#define MANDL
#ifdef TESTDELAY
			while ( Counter < 200 ) { // 2000 millisecond task
				Sleep(10);
				++Counter;
			}
			Counter = 0;
#endif
#ifdef TEXTNAME
			bitmap32* bm = bitmap32alloc(256,128,C32BLUE);
			cliprecto32(bm,0,0,256,128,C32BLACK);
			cliprecto32(bm,1,1,254,126,C32WHITE);
			outtextxyc32(bm,128,64,C32WHITE,t.c_str());
			gfxwrite32("temp.png",bm);
			bitmap32free(bm);
			rename("temp.png",t.c_str());
#endif
#ifdef MANDL
			mandl_task(256,t.c_str());
#endif
			// task done
_tasklock();
			tasks_done.insert(t);
			tasksleft = tasks.size();
			if (tasksleft>0) {
				tasks.pop_front();
				tasksleft = tasks.size();
			}
_taskunlock();
//			logger("WT: finishing task '%s' size now %d at %d\n",t.c_str(),tasksleft,getmillisec()-milli0);
		}
		wininfo.runinbackground = runinbackgroundsave;
	}
	logger( "WT: end second thread...\n" );
	_endthreadex(0); // is this necessary?
    return 0;
} 

void addtask(string tsk)
{
	logger("addtask: task '%s' at %d\n",tsk.c_str(),getmillisec()-milli0);
	if (tasks_pending.find(tsk)!=tasks_pending.end())
		return;
	tasks_pending.insert(tsk);
_tasklock();
	tasks.push_back(tsk);
_taskunlock();
	SetEvent(gettowork);
}

bool waittask(string tsk)
{
_tasklock();
	bool ret = tasks_done.find(tsk)!=tasks_done.end();
_taskunlock();
	return ret;
}

// problem with file
void resettask(string tsk)
{
_tasklock();
	tasks_pending.erase(tsk);
	tasks_done.erase(tsk);
	logger("reseting task '%s'\n",tsk.c_str());
_taskunlock();
}

void printstatus()
{
_tasklock();
	logger("printstatus: tasksize %d, count %d, time %d\n",tasks.size(),Counter,getmillisec()-milli0);
	set<string>::iterator it = tasks_done.begin();
	while(it!=tasks_done.end()) {
		logger("task '%s' done\n",(*it).c_str());
		++it;
	}
_taskunlock();
}

void setthreadquit()
{
	threadquit = true;
	SetEvent(gettowork); // one last job, kill yourself
}

void waitthread()
{
	logger("waitthread: start wait for thread to die\n");
	printstatus();
	setthreadquit();
    WaitForSingleObject(hThread,5000);
	logger("waitthread: end wait for thread to die\n");
	printstatus();
}

void threadinit()
{
//	fill(tasks_done,tasks_done+MAXDIFFTASKS,false);
//	fill(tasks_pending,tasks_pending+MAXDIFFTASKS,false);
	tasks.clear();
	tasks_done.clear();
	tasks_pending.clear();
	InitializeCriticalSection(&cs);
	hThread = (HANDLE)_beginthreadex( NULL, 0, &workerthread, NULL, 0 , &threadID );
	gettowork = CreateEvent(0,0,0,0);
	threadquit = false;
	runinbackgroundsave = wininfo.runinbackground;
	milli0 = getmillisec();
	totalcount = 0;
	Counter = 0;
}

void threadexit()
{
	waitthread();
	printstatus();
	CloseHandle(gettowork);
	CloseHandle(hThread);
	DeleteCriticalSection(&cs);
}

void threadtest()
{
	logger("---------------- start thread test --------------------\n");
	threadinit();
	string t = "the_default";
	// init threadtest
	// WaitForSingleObject(hThread,1000);
	logger("start of thread test\n");
	printstatus();

	//// test
	Sleep(500);
	logger("sleep 500\n");
	printstatus();

	addtask(t);
	addtask(t);
	addtask("3");
	addtask("5");
	Sleep(3000);
	logger("sleep 3000\n");
	printstatus();

	addtask(t);
	addtask(t);
	addtask("7");
	addtask("9");
	addtask("7");
	addtask("9");
	Sleep(3000);
	logger("sleep 3000\n");
	printstatus();

	Sleep(2000);
	logger("sleep 2000\n");
	printstatus();


	//// exit
	logger("end of thread test\n");
	threadexit();
	logger("---------------- end thread test --------------------\n");
}
 
} // end namespace u_s_searcher
