#ifdef __GNUC__


///////////////////////////// READCW
#define READCW(a) \
__asm__ __volatile__ ( \
	"fstcw %0\n" \
	"andl $0xffff,%0" \
	: "=m" (a) \
	:  );
///////////////////////////// WRITECW
//enum {RND_NEAR,RND_DOWN,RND_UP,RND_0};
//enum {PREC_32,PREC_RES,PREC_64,PREC_80}; // clean up
#define WRITECW(a) \
__asm__ __volatile__ ( \
	"fldcw %0\n" \
	:  \
	: "m" (a) );
///////////////////////////// F2INT
#define F2INT(a,b) \
__asm__ __volatile__ ( \
	"fld %1\n" \
	"fistpl %0" \
	: "=m" (b) \
	: "m" (a) \
	: "st" );
///////////////////////////// FLOG2
#define FLOG2(a,b) \
__asm__ __volatile__ ( \
	"fld1\n" \
	"fld %1\n" \
	"fyl2x\n" \
	"fstp %0" \
	: "=m" (b) \
	: "m" (a) \
	: "st" ,"st(1)");

///////////////////////////// LONGMULDIV
#define LONGMULDIV(a,b,c,d) \
__asm__ __volatile__ ( \
	"imul %%ebx\n" \
	"idiv %%ecx" \
	: "=a" (d) \
	: "a" (a), "b" (b), "c" (c) \
	: "edx","cc" );
///////////////////////////// LONGMULDIVU
#define LONGMULDIVU(a,b,c,d) \
__asm__ __volatile__ ( \
	"mul %%ebx\n" \
	"div %%ecx" \
	: "=a" (d) \
	: "a" (a), "b" (b), "c" (c) \
	: "edx","cc" );
///////////////////////////// LONGMULDIVR
#define LONGMULDIVR(a,b,c,q,r) \
__asm__ __volatile__ ( \
	"imul %%ebx\n" \
	"idiv %%ecx" \
	: "=a" (q),"=d" (r) \
	: "a" (a), "b" (b), "c" (c) \
	: "cc" );
///////////////////////////// LONGMULDIVUR
#define LONGMULDIVUR(a,b,c,q,r) \
__asm__ __volatile__ ( \
	"mul %%ebx\n" \
	"div %%ecx" \
	: "=a" (q),"=d" (r) \
	: "a" (a), "b" (b), "c" (c) \
	: "cc" );

///////////////////////////// RDTSC
#define RDTSC(llptr) \
__asm__ __volatile__ ( \
	".byte 0x0f; .byte 0x31" \
	: "=A" (llptr) );
///////////////////////////// RDTSC2
#define RDTSC2(argh,argl) \
__asm__ __volatile__ ( \
	".byte 0x0f; .byte 0x31" \
	: "=a" (argl),"=d" (argh) );
///////////////////////////// RDTSC256
#define RDTSC256(arg) \
__asm__ __volatile__ ( \
	".byte 0x0f; .byte 0x31\n" \
	"shll $24,%%edx\n" \
	"shrl $8,%%eax\n" \
	"orl %%edx,%%eax" \
	: "=a" (arg) \
	: \
	: "edx" );

///////////////////////////// TIMES5
#define TIMES5(arg1, arg2) \
__asm__ __volatile__ ( \
	"leal (%0,%0,4),%0" \
	: "=r" (arg2) \
	: "0" (arg1) );
///////////////////////////// STOSD
#define STOSD(destptr,dvalue,dcount) \
__asm__ __volatile__ ( \
	"cld\n" \
	"rep\n" \
	"stosl" \
	:  \
	: "c" (dcount), "a" (dvalue), "D" (destptr) \
	: "memory","cc" );
	
	
#else
 

///////////////////////////// READCW
#define READCW(a) \
	__asm { \
		__asm fstcw a \
	} \
	a&=0xffff;
///////////////////////////// WRITECW
//enum {RND_NEAR,RND_DOWN,RND_UP,RND_0};
//enum {PREC_32,PREC_RES,PREC_64,PREC_80}; // clean up
#define WRITECW(a) \
	__asm { \
		__asm fldcw a \
	} 
///////////////////////////// F2INT
#define F2INT(a,b) \
	{ \
		float t1=a; \
		S32 t2; \
		__asm { \
			__asm fld t1 \
			__asm fistp t2 \
		} \
		b=t2; \
	}
///////////////////////////// FLOG2
#define FLOG2(a,b) \
	__asm { \
		__asm fld1 \
		__asm fld a \
		__asm fyl2x \
		__asm fstp b \
	}

///////////////////////////// LONGMULDIV
#define LONGMULDIV(a,b,c,d) \
	{ \
		S32 t1=a; \
		S32 t2=b; \
		S32 t3=c; \
		__asm { \
			__asm mov eax,t1 \
			__asm mov ebx,t2 \
			__asm mov ecx,t3 \
			__asm imul ebx \
			__asm idiv ecx \
			__asm mov d,eax \
		} \
	}
///////////////////////////// LONGMULDIVU
#define LONGMULDIVU(a,b,c,d) \
	{ \
		S32 t1=a; \
		S32 t2=b; \
		S32 t3=c; \
		__asm { \
			__asm mov eax,t1 \
			__asm mov ebx,t2 \
			__asm mov ecx,t3 \
			__asm mul ebx \
			__asm div ecx \
			__asm mov d,eax \
		} \
	}
///////////////////////////// LONGMULDIVR
#define LONGMULDIVR(a,b,c,q,r) \
	{ \
		S32 t1=a; \
		S32 t2=b; \
		S32 t3=c; \
		__asm { \
			__asm mov eax,t1 \
			__asm mov ebx,t2 \
			__asm mov ecx,t3 \
			__asm imul ebx \
			__asm idiv ecx \
			__asm mov q,eax \
			__asm mov r,edx \
		} \
	}
///////////////////////////// LONGMULDIVUR
#define LONGMULDIVUR(a,b,c,q,r) \
	{ \
		S32 t1=a; \
		S32 t2=b; \
		S32 t3=c; \
		__asm { \
			__asm mov eax,t1 \
			__asm mov ebx,t2 \
			__asm mov ecx,t3 \
			__asm mul ebx \
			__asm div ecx \
			__asm mov q,eax \
			__asm mov r,edx \
		} \
	}

///////////////////////////// RDTSC2
#define RDTSC2(argh,argl) \
	__asm  { \
		__asm rdtsc \
		__asm mov argh,edx \
		__asm mov argl,eax \
	}
///////////////////////////// RDTSC256
#define RDTSC256(arg) \
	__asm { \
		__asm rdtsc \
		__asm shl edx,24 \
		__asm shr eax,8 \
		__asm or eax,edx \
		__asm mov arg,eax \
	}

///////////////////////////// TIMES5
#define TIMES5(a,b) \
	__asm { \
		__asm mov eax,a \
		__asm lea eax,[eax+4*eax] \
		__asm mov j,eax \
	}
///////////////////////////// STOSD
#define STOSD(destptr,dvalue,dcount) \
	{ \
		void* t1=destptr; \
		S32 t2=dvalue; \
		S32 t3=dcount; \
		__asm { \
			__asm mov edi,t1 \
			__asm mov eax,t2 \
			__asm mov ecx,t3 \
			__asm cld \
			__asm rep stosd \
		} \
	}
#endif
