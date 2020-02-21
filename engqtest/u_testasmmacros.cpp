#include <m_eng.h>

void testasmmacros()
{
	logger("in testasmmacros\n");
// test STOSD
	S32 i;
	U32 q,r;
	U32 testr[10];
	logger("test STOSD\n");
	for (i=0;i<10;++i)
		testr[i]=0x12345678;
	STOSD(testr,0x87654321,5);
	for (i=0;i<10;++i)
		logger("at %2d,value = %08x\n",i,testr[i]);
// test TIMES5
	logger("test TIMES5\n");
	for (i=0;i<10;++i) {
		U32 j;
		TIMES5(i,j);
		logger("times5 %d and %d\n",i,j);
	}
// test RDTSC2
	logger("test RDTSC2\n");
	for (i=0;i<10;++i) {
		U32 h,l;
		RDTSC2(h,l);
		logger("rdtsc2 %08x:%08x\n",h,l);
	}
// test RDTSC256
	logger("test RDTSC256\n");
	for (i=0;i<10;++i) {
		U32 v;
		RDTSC256(v);
		logger("rdtsc256 %08x\n",v);
	}
// test LONGMULDIV
	logger("test LONGMULDIV\n");
	S32 a=100000000,b=8000000,c=300000000;
	LONGMULDIV(a,b,c,i);
	logger("longmuldiv of %d,%d,%d is %d\n",a,b,c,i);
// test LONGMULDIVUR
	logger("test LONGMULDIVUR\n");
	U32 au=4000000000U,bu=1000000,cu=3579545;
	LONGMULDIVUR(au,bu,cu,q,r);
	logger("longmuldivur of %u,%u,%u is %u R %u\n",au,bu,cu,q,r);
	for (i=0;i<10;++i) {
		au=i;
		LONGMULDIVUR(au,bu,cu,q,r);
		logger("longmuldivur of %u,%u,%u is %u R %u\n",au,bu,cu,q,r);
	}
// test READCW
	i=0xffffffffU;
	logger("test READCW\n");
	READCW(i);
	logger("fpu cw = %08x\n",i);
// test F2INT round up
	float f;
//	U32 cwsave;
	logger("test F2INT round up\n");
//	READCW(cwsave);
//	U32 cw=0x87f;
//	WRITECW(cw);
	setfpuroundprec(FPUCW_RND_UP,FPUCW_PREC_32);
	for (f=-10;f<10;f+=.25f) {
//		float f2=f+.5f;
		F2INT(f,i);
//		i=f;
		logger("f2S32 of %f is %d\n",f,i);
	}
//	WRITECW(cwsave);
	defaultfpucontrol();
	logger("test FLOG2\n");
	float af,bf;
	for (af=1;af<=16;af+=1.0f) {
		FLOG2(af,bf);
		logger("FLOG2(%f) = %f\n",af,bf);
	}
	logger("done testasmmacros\n");
//	logger("before exit from testasmmacros MEMCLEAN");
//	exit(0);
}

