#define INCLUDE_WINDOWS
#include <m_eng.h>
// the goal here is to make a good microsecond time measurment
// this code tends to be highly nonportable
// on the pc you can use rdtsc, querypref, or good 'ol gettickcount
// some motherboards have bugs with the rdtsc (something with the carry)
// querypref seems to use a separate frequency tied to an old colorburst crystal 3,579,545 Hertz
// attempt to not use 64bit.
// LARGE_INTEGER assumed to be LowPart U32,HighPart S32
// getmicrosec should be called frequently to work properly. (We'll see)
// assume HighPart of QueryPerformanceFrequency is 0. (Bad assumption?) [If they push past 4GHZ]
// maybe time to port up to 64 bit math here..
static LARGE_INTEGER tm_lasttime;
static LARGE_INTEGER freq;
static U32 lastusec,lastusecfrac;

// c=a-b
void largeminus(LARGE_INTEGER* a,LARGE_INTEGER* b,LARGE_INTEGER* c)
{
	c->HighPart=a->HighPart-b->HighPart;
	c->LowPart=a->LowPart-b->LowPart;
	if (a->LowPart<b->LowPart) {
		--c->HighPart; // borrow
//		logger("borrow\n");
	}
}

// clamp down from 64bit to 32bit
void largeclamp(LARGE_INTEGER* li)
{
	if (li->HighPart) {
		li->HighPart=0;
		li->LowPart=0xffffffff;
	}
}

U32 getmicrosec()
{
//	static U32 t;
	LARGE_INTEGER deltatime;
	LARGE_INTEGER newtime;
	U32 newusec,newusecfrac;
	U32 dmq,dmr;
	QueryPerformanceCounter(&newtime); // slow, takes about 1 microsecond
	largeminus(&newtime,&tm_lasttime,&deltatime);
	largeclamp(&deltatime); // just in case..
	LONGMULDIVUR(1000000U,deltatime.LowPart,freq.LowPart,dmq,dmr); // 64 bit assembly
	newusecfrac=lastusecfrac+dmr;
	newusec=lastusec+dmq;
	if (newusecfrac>=freq.LowPart) {
		++newusec;
		newusecfrac-=freq.LowPart;
	}
	if (newusecfrac>=freq.LowPart)
		logger("WHAT'S THIS!\n");
	tm_lasttime=newtime;
	lastusec=newusec;
	lastusecfrac=newusecfrac;
//	++t;
//	return t;
	return newusec;
}

U32 getmillisec()
{
	return GetTickCount();
}
// init timer
// do some sampling 
void timerinit()
{
	LARGE_INTEGER samplast,sampnew,sampdiff;
	U32 freq10;
// init timer
	QueryPerformanceFrequency(&freq);
	largeclamp(&freq);
	QueryPerformanceCounter(&tm_lasttime);
	logger("freq %08x:%08x, counter %08x:%08x\n",
		freq.HighPart,freq.LowPart,tm_lasttime.HighPart,tm_lasttime.LowPart);
	lastusec=lastusecfrac=0;
// sample and compute CPU MHZ (KHZ)
	RDTSC2(samplast.HighPart,samplast.LowPart); // fast, less reliable
	freq10=freq.LowPart/10U; // 1/10 second delay
	while(1) {
		LARGE_INTEGER nt;
		QueryPerformanceCounter(&nt);
		if (nt.LowPart-tm_lasttime.LowPart>freq10)
			break;
	}
	RDTSC2(sampnew.HighPart,sampnew.LowPart);
	largeminus(&sampnew,&samplast,&sampdiff);
	largeclamp(&sampdiff);
	wininfo.compkhz=sampdiff.LowPart/100U;
	logger("cpu in KHZ is %u\n",wininfo.compkhz);
// maybe use this info for an RDTSC based getmicrosec() timer.
// for now use the query functions instead for that.
}

