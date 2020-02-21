struct bitmap32* gfxread32(const C8* fname);
void gfxwrite32(const C8* fname,const struct bitmap32* bm);

struct bitmap8* gfxread8(const C8* fname,C32* dacs);
void gfxwrite8(const C8* fname,const struct bitmap8* bm,const C32* dacs);

struct bitmap32* jpegread(FILE* fh); // for jrm cars
