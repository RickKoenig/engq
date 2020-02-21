#define INCLUDE_WINDOWS
#include <m_eng.h>
#define PERFNAMES
#include "m_perf.h"

S32 nopublic;
#ifdef PERF_ON

#define STACKSIZE 250
#define DIV256
//#define BIT64

#ifdef BIT64
static LARGE_INTEGER starttime;
static LARGE_INTEGER difftimer[STACKSIZE];
static U32 counts[STACKSIZE];

static U32 thestack[STACKSIZE];
static U32 thestackp;
#endif
#ifdef DIV256
static S32 starttime;
static S32 difftimer[STACKSIZE];
static U32 counts[STACKSIZE];

static U32 thestack[STACKSIZE];
static U32 thestackp;
#endif

static void perf_initmark()
{
//	struct bigtime rt;
//	starttime=getmicrosec();
//	getrawtime(&rt);
	static bool inited;
	if (!inited) {
		ULONG_PTR pm,sm; // force code to run on 1 cpu for accurate timing
		HANDLE p=GetCurrentProcess();
		U32 hr=GetProcessAffinityMask(p,&pm,&sm);
		logger("hr = %d,sizeof pm is %d\n",hr,sizeof(pm));
		logger("sm %d, pm %d\n",sm,pm);
		hr=SetProcessAffinityMask(p,1);
		logger("hr = %d\n",hr);
		hr=GetProcessAffinityMask(p,&pm,&sm);
		logger("hr = %d after, sizeof pm is %d\n",hr,sizeof(pm));
		logger("sm after is %d, pm %d\n",sm,pm);

		inited=true;
	}
#ifdef DIV256
	RDTSC256(starttime); // fast
#endif
#ifdef BIT64
	QueryPerformanceCounter(&starttime); // slow
#endif
}

static void perf_mark(U32 index)
{
//	struct bigtime rt;
//	endtime=getmicrosec();
//	getrawtime(&rt);
//	endtime=rt.lo;
#ifdef DIV256
	S32 endtime;
	RDTSC256(endtime);
	difftimer[index]+=endtime-starttime;
#endif
#ifdef BIT64
	LARGE_INTEGER endtime;
	QueryPerformanceCounter(&endtime);
	difftimer[index].QuadPart+=endtime.QuadPart-starttime.QuadPart;
#endif
//	logger("perf test %d\n",endtime-starttime);
	starttime=endtime;
}

void perf_start(U32 index)
{
	if (thestackp)
		perf_mark(thestack[thestackp-1]);
	else
		perf_initmark();
	if (thestackp>=STACKSIZE)
		errorexit("perf: STACKSIZE overflow!");
	thestack[thestackp++]=index;
	counts[index]++;
}

void perf_end(U32 index)
{
	if (!thestackp)
		errorexit("perf: STACKSIZE underflow");
	thestackp--;
	if (thestack[thestackp]!=index)
		errorexit("perf: perf_end(%s) expected %s",perfnames[index],perfnames[thestack[thestackp]]);
	perf_mark(index);
}

void perf_frame()
{
#ifdef BIT64
	static bool gotfreq;
	static LARGE_INTEGER freq;
	if (!gotfreq) {
		QueryPerformanceFrequency(&freq);
		gotfreq=true;
	}
#endif
	U32 microsec;
	U32 i;
	U32 totaltime=0;
	static S32 samplerstart,samplerend;
	S32 frames;
	if ((samplerend&(PERF_FRAMES-1))==0) {
		frames=samplerend-samplerstart+1;
		logger("\n");
		logger("-------- PERF ---------  frame %5d - %5d -------------------------------------------------\n",
			samplerstart,samplerend);
		for (i=0;i<PERF_NUM;i++) {
			if (counts[i]) {
#ifdef DIV256
				microsec = (difftimer[i]/wininfo.compkhz)<<8; // use getrawtimelod256
				LONGMULDIV(256000,difftimer[i],wininfo.compkhz,microsec);
//				logger("difftimer %d, adj %d\n",difftimer[i],microsec);
				logger("%3d %-20s:%9d calls/frame,   %9d microsec/frame ,    %9d microsec/call\n",
					i,perfnames[i],counts[i]/frames,microsec/frames,microsec/counts[i]);
				totaltime+=microsec;
				difftimer[i]=0;
				counts[i]=0;
#endif
#ifdef BIT64
				microsec = (U32)(1000000LL*difftimer[i].QuadPart/freq.QuadPart);
				logger("%3d %-20s: %8d calls/frame,   %9d microsec/frame ,    %9d microsec/call\n",
					i,perfnames[i],counts[i]/frames,microsec/frames,microsec/counts[i]);
				totaltime+=microsec;
				difftimer[i].QuadPart=0;
				counts[i]=0;
#endif
			}
		}
#ifdef DIV256
		logger("--- PERF SUMMARY ------ %5d frame(s) sampled,   %9d microsec/frame , %8.3f fps ----------\n",
			frames,totaltime/frames,1000000.0*frames/totaltime);
#endif
#ifdef BIT64
		logger("--- PERF SUMMARY ------ %5d frame(s) sampled,  %9d microsec/frame , %8.3f fps ----------\n",
			frames,totaltime/frames,1000000.0*frames/totaltime);
#endif
		samplerstart=samplerend+1;
	}
	samplerend++;
}

/*void perf_test()
{
perf_start(TEST1);
perf_end(TEST1);
}
*/
/*
//float floatarray[1000];
//S32 intarray[1000];
S32 i,k=10000;
float f=12.3456,g=46.3432;
perf_start(TEST1);
for (i=0;i<1000;i++)
	intarray[i]=i*i;
perf_end(TEST1);
perf_start(TEST2);
for (i=1;i<1000;i++)
	intarray[i]=k/i;
perf_end(TEST2);
perf_start(TEST3);
for (i=0;i<1000;i++)
	floatarray[i]=f*i;
perf_end(TEST3);
perf_start(TEST4);
for (i=0;i<1000;i++)
	floatarray[i]=f/g;
perf_end(TEST4);
perf_start(TEST5);
for (i=0;i<1000;i++)
	floatarray[i]=i+g;
perf_end(TEST5);
perf_start(TEST6);
for (i=0;i<1000;i++)
	intarray[i]=i+k;
perf_end(TEST6);
*/


#endif
