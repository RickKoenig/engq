#ifdef __cplusplus
#ifndef C_IS_CPP
extern "C" {
#endif
#endif

// try using new and delete instead...
void *memalloc(U32 amount);
void* memzalloc(U32 amount);
// for lua
void *memrealloc(void *old,U32 newamount);
void memfree(void *ptr);

void setmemverboselevel(U32 level); // 0 or 1, default 0

// internal
void meminit();
void memexit();
void memname(const C8* mn);
void pushmemname(const C8* mn); // remember previous memname
void popmemname(); // go back to previous memname
void memstats();
U32 membytes();
U32 memchunks();

void checkheaps(); // check internal mem heaps

#ifdef __cplusplus
#ifndef C_IS_CPP
}
#endif
#endif
