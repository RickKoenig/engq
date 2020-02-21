#include <m_eng.h>
#include "m_gfx_jpg.h"
#include "m_gfx_pcx.h"
#include "m_gfx_tga.h"
#include "m_gfx_dds.h"
#include "m_gfx_png.h"
#include "m_gfx_lbm.h"

void gfxwrite32(const C8* fname,const struct bitmap32* d32)
{
	C8 ext[256];
	mgetext(fname,ext,256);
	if (!my_stricmp(ext,"tga"))
		gfxwrite32_tga(fname,d32);
	else if (!my_stricmp(ext,"png"))
		gfxwrite32_png(fname,d32);
//	else
//		errorexit("don't know how to write out '%s'",fname);
}

struct bitmap32* gfxread32(const C8* fname)
{
	// see if this breaks everything !?
	if (!fname || !fname[0])
		return 0;
	C8 ext[256];
	struct bitmap32* d32;
	C32 dacs[256];
	mgetext(fname,ext,256);
/*	if (!fileexist(fname)) {
		pushandsetdir("common");
		struct bitmap32* ret=gfxread32_tga(fname);
 		popdir();
		return ret;
	} */
	if (!my_stricmp(ext,"pcx")) {
		struct bitmap8* d8=gfxread8_pcx(fname,dacs);
		d32=convert8to32(d8,dacs);
		bitmap8free(d8);
	} else if (!my_stricmp(ext,"lbm")||!my_stricmp(ext,"bbm")) {
		struct bitmap8* d8=gfxread8_lbm(fname,dacs);
		d32=convert8to32(d8,dacs);
		bitmap8free(d8);
	} else if (!my_stricmp(ext,"tga"))
		d32=gfxread32_tga(fname);
	else if (!my_stricmp(ext,"jpg"))
		d32=gfxread32_jpg(fname);
	else if (!my_stricmp(ext,"dds"))
		d32=gfxread32_dds(fname);
	else if (!my_stricmp(ext,"png"))
		d32=gfxread32_png(fname);
	else
		d32=bitmap32alloc(8,8,C32LIGHTMAGENTA);
	return d32;
}

struct bitmap8* gfxread8(const C8* fname,C32* dacs)
{
	C8 ext[256];
	struct bitmap8* d8;
	mgetext(fname,ext,256);
	if (!my_stricmp(ext,"pcx")) {
		d8=gfxread8_pcx(fname,dacs);
	} else if (!my_stricmp(ext,"lbm")||!my_stricmp(ext,"bbm")) {
		d8=gfxread8_lbm(fname,dacs);
	} else {
		d8=bitmap8alloc(8,8,0);
		memset(dacs,0,256*sizeof(C32));
	}
	return d8;
}

void gfxwrite8(const C8* fname,const struct bitmap8* d8,const C32* dacs)
{
	C8 ext[256];
	mgetext(fname,ext,256);
	if (!my_stricmp(ext,"pcx")) {
		gfxwrite8_pcx(fname,d8,dacs);
	}
}
