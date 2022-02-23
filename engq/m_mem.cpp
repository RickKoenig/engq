#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

//#define BATTLEMEM

#ifdef BATTLEMEM
#include <npapi.h>
#endif

// global option
#ifdef WIN32
#define OVERRIDE_NEW_DELETE // global override, used with everything
#else
//#define OVERRIDE_NEW_DELETE // global override, used with everything
#endif

// one of these
//#define CHECKPTRS // includes CHECKMEMLEAKS features, keeps track of mem alloced in a huge hash table, does dup frees
//#define CHECKMEMLEAKS // check for leaks, keeps track of total numchunks and sizes, no checks for dup frees
#define CHECKNONE // pass thru

// sub options
#define CHECKBOUNDS // catches buffer overruns and underruns, used with CHECKPTRS
#define EXTRASIZE 0	// 8000 // (new/delete only) allocs extra memory per chunk, good for catching rare crash bugs, used with CHECKPTRS

// begin export linkage as c not c++ (for lua)
#ifdef __cplusplus
#ifndef C_IS_CPP
extern "C" {
#endif
#endif

// for lua
// void *memalloc(S32 amount); // lua doesn't need
void *memrealloc(void *old,U32 newamount); // lua does memalloc by passing a null for old
void memfree(void *ptr);
void memname(const C8* mn); // call before alloc, set's memalloc/new's name.  It'd be cool to have a stack.., naw, be too confusing

void setmemverboselevel(U32 level); // 0 or 1, default 0

// internal
void meminit();
void memexit();

void checkheaps(); // check internal mem heaps
//void memstats();

// end export linkage as c not c++ (for lua)
#ifdef __cplusplus
#ifndef C_IS_CPP
}
#endif
#endif

//static S32 meminited=0;
static S32 memid;
static S32 memverbose=0;
static S32* dummy;

static CRITICAL_SECTION cs;

static void memlock()
{
	if (dummy)
		EnterCriticalSection(&cs);
}

static void memunlock()
{
	if (dummy)
		LeaveCriticalSection(&cs);
}

void setmemverboselevel(U32 level) // 0 or 1, default 0
{
	memverbose=level;
}

static void checkalign(U8 *ptr)
{
	if ((U32)ptr&3)
		errorexit("mem not aligned to 4 byte boundaries");
}

//_CRTIMP S32 __cdecl _CrtCheckMemory();
void checkheaps()
{
//	if (!_CrtCheckMemory())
//		errorexit("heap bad");
#ifdef WIN32
	HANDLE heaps[50];
	S32 nheaps=50;
	memlock();
	nheaps=GetProcessHeaps(nheaps,heaps);
	S32 i;
	for (i=0;i<nheaps;++i) {
		logger("heap %d: %08x\n",i,heaps[i]);
		S32 r=HeapValidate(heaps[i],0,0);
		if (!r)
			errorexit("bad heap!");
	}
	memunlock();
#endif
}

#ifdef CHECKPTRS

static U32 checkptrlist[8]={0,0,0,0,0,0,0,0}; // doesn't work any more since vista (mem scramble security BS)
static U32 ncheckptrlist=0;
static S32 checksize[8]={608,0,0,0,0,0,0,0}; // so now try just sizes
static U32 nchecksize=0;

#ifdef CHECKBOUNDS
static U8 beforebytes[]={
	0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
	0x8b,0x7a,0x69,0x58,0x47,0x36,0x25,0x14,
};
static U8  afterbytes[]={
	0x14,0x25,0x36,0x47,0x58,0x69,0x7a,0x8b,
	0xf0,0xde,0xbc,0x9a,0x78,0x56,0x34,0x12,
};
#endif

// memory //////////////////////////
//#define MEMNAMESIZE 4   // this value has significant effect on mem usage by 'memtab mt',  131078172 bytes when MEMNAMESIZE = 4
#define MEMNAMESIZE 8   // this value has significant effect on mem usage by 'memtab mt',  163846172 bytes when MEMNAMESIZE = 8
//#define MEMNAMESIZE 12    // this value has significant effect on mem usage by 'memtab mt',  196614172 bytes when MEMNAMESIZE = 12

struct memchunk {
	U32 size;
	void *ptr;
	S32 count; // 1 if mem alloced after a call to meminit
	S32 shown;		// 1 if shown with memstats
	C8 mname[MEMNAMESIZE]; // memchunk's name
};

// try a hash table to speed this up
#define MAXMEM 16000 // try this and if doesn't work, go with 4000
#define BITNUM 512 // power of 2

struct hashslot {
	U32 nummem;//,numinitedmem;
	U32 maxnummem;
	struct memchunk mc[MAXMEM];
};

struct memtab {
	struct hashslot hs[BITNUM];
	U32 maxhash;
	U32 totalmemsize;
	U32 maxmemsize;
	U32 totalmemchunks;
	U32 maxmemchunks;
//	U32 totalmemsizeinited;
//	U32 totalmemchunksinited;
};
static struct memtab mt; // I'd allocate this structure, but that might create a paradox..
static C8 curmemname[MEMNAMESIZE]="befmain"; // MEMNAMESIZE-1 chars max, and the null makes MEMNAMESIZE

static S32 hashfunc(U32 val)
{
	return ((val>>5)+(val>>9)+(val>>13)+(val>>17))&(BITNUM-1);
}

#ifdef CHECKBOUNDS
static S32 cmem(U8 *ptr1,U8 *ptr2,S32 amount)
{
	U8* ptr1save = ptr1;
	U8* ptr2save = ptr2;
	S32 amountsave = amount;
	while(amount--) {
		if (*ptr1++!=*ptr2++) {
			logger("cmem fail at %p, %p, size %2d\n",ptr1save,ptr2save,amountsave);
			S32 i;
			for (i=0;i<amountsave;++i) {
				logger("mismatch at offset %2d, p1val %02x, p2val %02x\n",i,ptr1save[i],ptr2save[i]);
			}
			return 0;
		}
	}
	return 1;
}
#endif
void memfree(void *ptr)
{
	perf_start(MEMFREE);
	memlock();
	struct memchunk *mp;
	S32 hash;
	U32 i,j;
	S32 numm;
//	checkmem();
	if (ptr==0)
		errorexit("Attempt to free a null pointer");
	for (i=0;i<ncheckptrlist;i++)
		if (checkptrlist[i]==(U32)ptr)
			logger("memfree: memptr in the check list %08x\n",ptr);
	hash=hashfunc((S32)ptr);
	if (mt.hs[hash].nummem<=0)
		errorexit("attempt to free memory not alloced  (empty hash table) %p",ptr);
	mp=mt.hs[hash].mc;
	for (i=0;i<mt.hs[hash].nummem;i++,mp++) {
		if (mp->ptr==ptr) {
			mt.totalmemchunks--;
			mt.hs[hash].nummem--;
/*			if (mp->meminited) {
				mt.hs[hash].numinitedmem--;
				mt.totalmemchunksinited--;
			} */
			if (memverbose /* || !meminited */ || mp->shown)
				logger("memfree:  ptr %p, size %7d, id %7d, name '%s'\n",ptr,mp->size,mp->count,mp->mname);
			mt.totalmemsize-=mp->size;
/*			if (mp->meminited)
				mt.totalmemsizeinited-=mp->size; */
			for (j=0;j<nchecksize;j++)
				if (checksize[j]==mp->size)
					logger("memfree:  memsize in the check list %08x (%d)\n",ptr,mp->size);
#ifdef CHECKBOUNDS
			if (!cmem((U8 *)ptr+mp->size,afterbytes,sizeof(afterbytes)))
				errorexit("corrupt afterbytes %p, memsize %d",ptr,mp->size);
			ptr=(void *)((C8 *)ptr-sizeof(beforebytes));
			if (!cmem((U8 *)ptr,beforebytes,sizeof(beforebytes)))
				errorexit("corrupt beforebytes %p",ptr);
#endif
#ifdef BATTLEMEM
			NPN_MemFree(ptr);
#else
			free(ptr);
#endif
			mp=mt.hs[hash].mc;
			numm=mt.hs[hash].nummem;
			if (i!=numm)
				mp[i]=mp[numm];
			if (mt.totalmemchunks==0) {
				logger("MEMCLEAN\n");
//				logger("memalloc'ed now at %d\n",mt.totalmemchunks);
			}
			memunlock();
			perf_end(MEMFREE);
			return;
		}
	}
	errorexit("attempt to free memory not alloced %p",ptr);
	//logger("attempt to free memory not alloced %p\n",ptr);
	memunlock();
	perf_end(MEMFREE);
}

static U32 getchunksize(void *ptr)
{
	memlock();
	struct memchunk *mp;
	S32 hash;
	S32 numm;
	S32 i;
//	perf_start(GETCHUNKSIZE);
	if (ptr==0)
		errorexit("Attempt to get chunk size on a null pointer");
	hash=hashfunc((S32)ptr);
	numm=mt.hs[hash].nummem;
	if (numm<=0)
		errorexit("attempt to  get chunk size on mem not alloced (empty hash table)");
	mp=mt.hs[hash].mc;
	for (i=0;i<numm;i++,mp++)
		if (mp->ptr==ptr) {
//			perf_end(GETCHUNKSIZE);
			memunlock();
			return mp->size;
		}
	errorexit("attempt to get chunk size on mem not alloced");
	memunlock();
	return 0;
}

void *memalloc(U32 amount)
{
	perf_start(MEMALLOC);
	memlock();
//	if (amount == 4 && !strcmp(curmemname,"qcomp"))
//		logger("malloc special id %d, amount %d, name %s\n",memid,amount,curmemname);
//	if (memid==5055 || memid==5056)
//		logger("malloc special: id %d, amount %d\n",memid,amount);
//	if (amount==528 && memid>=22860)
//		logger("malloc special: id %d, amount %d\n",memid,amount);
//	if (memid==991)
//		logger("malloc special: id %d, amount %d\n",memid,amount);
	struct memchunk *mp;
	S32 hash;
	U32 i;
	S32 numm;
	U8 *retptr;
//	perf_start(ALLOC32);
//	checkmem();
	if (amount==0)
		errorexit("can't alloc 0 bytes");
#ifdef CHECKBOUNDS
#ifdef BATTLEMEM
	retptr=(U8 *)NPN_MemAlloc(amount+sizeof(beforebytes)+sizeof(afterbytes));
#else
	retptr=(U8 *)malloc(amount+sizeof(beforebytes)+sizeof(afterbytes));
#endif
#else
#ifdef BATTLEMEM
	retptr=(U8 *)NPN_MemAalloc(amount);
#else
	retptr=(U8 *)malloc(amount);
#endif
#endif
	checkalign(retptr);
	if (retptr==0)
		errorexit("mem alloc failure, requested %d",amount);
#ifdef CHECKBOUNDS
	memcpy(retptr,beforebytes,sizeof(beforebytes));
	retptr+=sizeof(beforebytes);
	memcpy(retptr+amount,afterbytes,sizeof(afterbytes));
#endif
	if (memverbose)
		logger("memalloc: ptr %p, size %7u, name '%s'\n",retptr,amount,curmemname);
//	perf_end(MALLOC);
	hash=hashfunc((S32)retptr);
	numm=mt.hs[hash].nummem;
	if (numm>=MAXMEM) {
#ifdef CHECKBOUNDS
#ifdef BATTLEMEM
		NPN_MemFree(retptr-sizeof(beforebytes));
#else
		free(retptr-sizeof(beforebytes));
#endif
#else
#ifdef BATTLEMEM
		NPN_MemFree(retptr);
#else
		free(retptr);
#endif
#endif
		errorexit("not enough memory slots (full hash table)");
	}
	mp=mt.hs[hash].mc;
	mp=mp+numm;
	mp->ptr=retptr;
	mp->size=amount;
	strcpy(mp->mname,curmemname); // copy memchunk's name over
//	mp->meminited=meminited;
	mp->count=memid++;
	//if (memid == 3000)
	//	logger("3000\n");
	mp->shown=0;
	mt.totalmemchunks++;
	mt.hs[hash].nummem++;
/*	if (meminited) {
		mt.hs[hash].numinitedmem++;
		mt.totalmemchunksinited++;
	} */
	if (mt.hs[hash].nummem>mt.hs[hash].maxnummem)
		mt.hs[hash].maxnummem=mt.hs[hash].nummem;
	if (mt.hs[hash].nummem>mt.maxhash)
		mt.maxhash=mt.hs[hash].nummem;
	if (mt.totalmemchunks>mt.maxmemchunks)
		mt.maxmemchunks=mt.totalmemchunks;
	mt.totalmemsize+=amount;
/*	if (meminited)
		mt.totalmemsizeinited+=amount; */
	if (mt.totalmemsize>mt.maxmemsize)
		mt.maxmemsize=mt.totalmemsize;
	for (i=0;i<ncheckptrlist;i++)
		if (checkptrlist[i]==(U32)retptr)
			logger("memalloc: memptr in the check list %08x: %d\n",retptr,amount);
	for (i=0;i<nchecksize;i++)
		if (checksize[i]==mp->size)
			logger("memalloc: memsize in the check list %08x (%d)\n",retptr,amount);
	memunlock();
	perf_end(MEMALLOC);
	return retptr;
}

void memname(const C8* mn)
{
	mystrncpy(curmemname,mn,MEMNAMESIZE);
}

#define MEM_STACK_SIZE 10
C8 memstack[MEM_STACK_SIZE][MEMNAMESIZE]; // make static
static S32 curmemstack;

void pushmemname(const C8* mn)
{
	if (curmemstack >= 10)
		errorexit("popmemname stack full while pushing '%s'\n",mn);
	mystrncpy(memstack[curmemstack++],curmemname,MEMNAMESIZE);
	memname(mn);
}

void popmemname()
{
	if (curmemstack <= 0)
		errorexit("popmemname stack empty");
	mystrncpy(curmemname,memstack[--curmemstack],MEMNAMESIZE);
}

void memstats()
{
	U32 i,j;
//	S32 flag=0;
//	if (mt.totalmemsizeinited || mt.totalmemchunksinited) {
//		error("mem leak: max chunks %d, cur chunks %d\nmax alloced %d, cur alloced %d",
//			mt.maxmemchunks,mt.totalmemchunks,mt.maxmemsize,mt.totalmemsize);
		logger("mem leak: max chunks %u, cur chunks %u\n",mt.maxmemchunks,mt.totalmemchunks);
		logger("max alloced %u, cur alloced %u\n",mt.maxmemsize,mt.totalmemsize);
//	}
	logger("mem stats(MAXHASH ALLOWED=%u): maxmemsize %u, maxmemchunks %u\n",MAXMEM,mt.maxmemsize,mt.maxmemchunks);
	logger("hash table, maxhash %u\n",mt.maxhash);
	for (i=0;i<BITNUM;i++) {
//		if (nummem[i])
//			error("Mem stats","%3d: maxhashchunks %4d, curhashchunks %d",i,maxnummem[i],nummem[i]);
		for (j=0;j<mt.hs[i].nummem;j++)
			if (mt.hs[i].mc[j].size) {
//				if (mt.hs[i].mc[j].meminited)
//					flag=1;
				if (!mt.hs[i].mc[j].shown) {
					logger("chunk not freed ptr %p, amount %7u, id %7u, name '%s'\n",
						mt.hs[i].mc[j].ptr,mt.hs[i].mc[j].size,mt.hs[i].mc[j].count,mt.hs[i].mc[j].mname);
					mt.hs[i].mc[j].shown=1;
				}
			}
	}
//	if (flag)
//		error("mem leak, check logfile.txt for details");
}

void *memrealloc(void *old,U32 newamount)
{
	//memlock();
	void *newptr=memalloc(newamount);
	if (old) {
		memcpy(newptr,old,min(getchunksize(old),newamount));
		memfree(old);
	}
	//memunlock();
	return newptr;
}

U32 memchunks()
{
	return mt.totalmemchunks;
}

U32 membytes()
{
	return mt.totalmemsize;
}

#endif

#ifdef CHECKMEMLEAKS

struct memtab {
	U32 totalmemsize;
	U32 maxmemsize;
	U32 totalmemchunks;
	U32 maxmemchunks;
};
static struct memtab mt;

void memfree(void *ptr)
{
	perf_start(MEMFREE);
	if (ptr==0)
		errorexit("Attempt to free a null pointer");
	U32* ptr32=(U32*)ptr;
	--ptr32;
	U32 size=ptr32[0];
	mt.totalmemchunks--;
	//logger("just lowered totalmemchunks to %d\n",mt.totalmemchunks);
	//if (mt.totalmemchunks == 100)
	//	logger("lowering to 100\n");
	mt.totalmemsize-=size;
	if (memverbose /* || !meminited */)
		logger("memfree: ptr %p, size %u, left to free %u:%u\n",ptr,size,mt.totalmemchunks,mt.totalmemsize);
	free(ptr32);
	if (mt.totalmemchunks==0 && mt.totalmemsize==0) {
		logger("MEMCLEAN\n");
//		logger("memalloc'ed now at %d\n",mt.totalmemchunks);
	}
	perf_end(MEMFREE);
}

void *memalloc(U32 amount)
{
	perf_start(MEMALLOC);
	memlock();
	U32 *retptr;
	if (amount==0)
		errorexit("can't alloc 0 bytes");
	retptr=(U32*)malloc(amount+4);
	if (!retptr)
		errorexit("err, cant alloc: %u bytes",amount);
	checkalign((U8*)retptr);
	retptr[0]=amount;
	if (memverbose)
		logger("memalloc: ptr %p, size %u\n",&retptr[1],amount);
	++mt.totalmemchunks;
	//logger("just raised totalmemchunks to %d\n",mt.totalmemchunks);
	if (mt.totalmemchunks>mt.maxmemchunks)
		mt.maxmemchunks=mt.totalmemchunks;
	mt.totalmemsize+=amount;
	if (mt.totalmemsize>mt.maxmemsize)
		mt.maxmemsize=mt.totalmemsize;
	memunlock();
	perf_end(MEMALLOC);
	return &retptr[1];
}

void memname(const C8* mn)
{
}

void pushmemname(const C8* mn)
{
}

void popmemname()
{
}

void memstats()
{
	if (mt.totalmemsize || mt.totalmemchunks) {
		logger("mem leak: max chunks %u, cur chunks %u\n",mt.maxmemchunks,mt.totalmemchunks);
		logger("max alloced %u, cur alloced %u\n",mt.maxmemsize,mt.totalmemsize);
	}
	logger("mem stats: maxmemsize %u, maxmemchunks %u\n",mt.maxmemsize,mt.maxmemchunks);
}

void *memrealloc(void *old,U32 newamount)
{
	if (!old)
		return memalloc(newamount);
	if (!newamount) {
		memfree(old);
		return 0;
	}
	U32* old32=(U32*)old;
	--old32;
	mt.totalmemsize-=old32[0];
	mt.totalmemsize+=newamount;
	if (mt.totalmemsize>mt.maxmemsize)
		mt.maxmemsize=mt.totalmemsize;
	U32* ret=(U32*)realloc(old32,newamount+4);
	ret[0]=newamount;
	return &ret[1];
}

U32 memchunks()
{
	return mt.totalmemchunks;
}

U32 membytes()
{
	return mt.totalmemsize;
}

#endif // CHECKMEMLEAKS

#ifdef CHECKNONE

void memfree(void *ptr)
{
	perf_start(MEMFREE);
	free(ptr);
	perf_end(MEMFREE);
}

void *memalloc(U32 amount)
{
	void *retptr;
	perf_start(MEMALLOC);
	memlock();
	retptr=malloc(amount);
	checkalign((U8*)retptr);
	if (!retptr)
		errorexit("err, cant alloc: %u bytes",amount);
	memunlock();
	perf_end(MEMALLOC);
	return retptr;
}

void memname(const C8* mn)
{
}

void pushmemname(const C8* mn)
{
}

void popmemname()
{
}

void memstats()
{
}

void *memrealloc(void *old,U32 newamount)
{
	return realloc(old,newamount);
}

U32 membytes()
{
	return 0;
}

U32 memchunks()
{
	return 0;
}

#endif // CHECKNONE

void meminit()
{
//	meminited=1;
	memname("default");
	dummy=new S32;
	InitializeCriticalSection(&cs);
}

void memexit()
{
	delete dummy;
	dummy = 0;
	DeleteCriticalSection(&cs);
	memname("exited");
//	meminited=0;
	memstats();
#ifdef CHECKPTRS
	logger("sizeof CHECKPTRS structures %u\n",sizeof(mt));
#endif
#ifdef CHECKNONE
	logger("no check mem leaks MEMCLEAN\n");
#endif
}

#ifdef OVERRIDE_NEW_DELETE
// new of size 0 allocates 1 byte (we could have it return a pointer to "global static addr" and check for that addr on delete)
void *operator new(size_t t)
{
	if (t==0)
		t=1;
#ifdef BATTLEMEM
	void *ret=NPN_MemAlloc(t+EXTRASIZE);
#else
	void *ret=memalloc(t+EXTRASIZE);
#endif

	return ret;
}
// can delete null ptrs, they just do nothing
void operator delete(void *p)
{
	if (!p)
		return;
	memfree(p);
}
#endif

void* memzalloc(U32 amount)
{
	void* ret = memalloc(amount);
	memset(ret,0,amount);
	return ret;
}
