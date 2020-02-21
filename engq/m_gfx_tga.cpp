#include <m_eng.h>

#include <pshpack1.h>
struct tgaheader {
	U8 id;
	U8 colortype;
	U8 imagetype;
	U16 firstcolor;
	U16 colormaplen;
	U8 colormapbits;
	U16 xorg;
	U16 yorg;
	U16 width;
	U16 height;
	U8 bitsperpixel;
	U8 description;
};

static struct tgaheader tgaheaderread;
static struct tgaheader tgaheaderwrite= {
	0,
	0,
	2,
	0,
	0,
	0,
	0,
	0,
	69,
	69,
	32,
	32+8
};
#include <poppack.h>

void gfxwrite32_tga(const C8* fname,const struct bitmap32* d)
{
	FILE* fh;
	S32 i,j;
	S32 tx,ty;
	C32* p=d->data;
	tx=d->size.x;
	ty=d->size.y;
/////// first write header //////////
	tgaheaderwrite.width=tx;
	tgaheaderwrite.height=ty;
//	tgaheaderwrite.bitsperpixel=32;
//	logger("writing tga32 '%s'\n",fname);
	fh=fopen2(fname,"wb");
	fwrite(&tgaheaderwrite,1,sizeof(tgaheaderwrite),fh);
// then write data
	for (j=0;j<ty;j++) {
		for (i=0;i<tx;++i) {
			fwrite(&p->b,1,1,fh);
			fwrite(&p->g,1,1,fh);
			fwrite(&p->r,1,1,fh);
			fwrite(&p->a,1,1,fh);
			++p;
		}
	}
//	tgaheaderwrite.bitsperpixel=24;
	fclose(fh);
}

static void fastxflip32(struct bitmap32* b)
{
	S32 i,j;
	struct bitmap32* t=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
	for (j=0;j<b->size.y;++j)
		for (i=0;i<b->size.x;++i)
			fastputpixel32(t,i,j,fastgetpixel32(b,b->size.x-1-i,j));
	fastblit32(t,b,0,0,0,0,b->size.x,b->size.y);
	bitmap32free(t);
}

static void fastyflip32(struct bitmap32* b)
{
	S32 i,j;
	struct bitmap32* t=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
	for (j=0;j<b->size.y;++j)
		for (i=0;i<b->size.x;++i)
			fastputpixel32(t,i,j,fastgetpixel32(b,i,b->size.y-1-j));
	fastblit32(t,b,0,0,0,0,b->size.x,b->size.y);
	bitmap32free(t);
}

struct bitmap32* gfxread32_tga(const C8* fname)
{
	FILE* fh;
	C32 colorkey=C32BLACK;
	struct bitmap32* d;
	C32* p;
	S32 tx,ty,prod;
	S32 j;
	S32 compressed=0;
	U8 count;
	colorkeyinfo.lasthascolorkey=0;
//	logger("sizeof tga header is %d\n",sizeof(struct tgaheader));
/////// first read file //////////
	fh=fopen2(fname,"rb");
	fread(&tgaheaderread,1,sizeof(tgaheaderread),fh);
	////// now alloc tga //////////
	tx=tgaheaderread.width;
	ty=tgaheaderread.height;
	d=bitmap32alloc(tx,ty,C32BLACK);
	p=d->data;
	if ((tgaheaderread.imagetype&~8)!=2)
		errorexit("can't handle this image format '%d, can only handle '2' or 10, fname '%s'",
			tgaheaderread.imagetype,fname);
	if (tgaheaderread.imagetype&8)
		compressed=1;
	if (tgaheaderread.bitsperpixel!=24 && tgaheaderread.bitsperpixel!=32)
		errorexit("can't handle this bpp %d, only 24 or 32, fname '%s'",
		tgaheaderread.bitsperpixel,fname);
	fseek(fh,tgaheaderread.id,SEEK_CUR);
	prod=tx*ty;
	if (compressed) {
		if (tgaheaderread.bitsperpixel==24) {
			for (j=0;j<prod;) {
				U32 k;
				fread(&count,1,1,fh);
//				count=filereadbyte();
				if (count&0x80) { // run
					C32 v;
					count&=0x7f;
					++count;
					fread(&v.b,1,1,fh);
					fread(&v.g,1,1,fh);
					fread(&v.r,1,1,fh);
					v.a=0xff;
					if (colorkeyinfo.usecolorkey) {
						if (v.c32==colorkey.c32) {
							v.a=0;
							colorkeyinfo.lasthascolorkey=1;
						}
					}
					for (k=0;k<count;++k) {
						*p++=v;
					}
				} else { // lit
					++count;
					for (k=0;k<count;++k) {
						fread(&p->b,1,1,fh);
						fread(&p->g,1,1,fh);
						fread(&p->r,1,1,fh);
						p->a=0xff;
						if (colorkeyinfo.usecolorkey) {
							if (p->c32==colorkey.c32) {
								p->a=0;
								colorkeyinfo.lasthascolorkey=1;
							}
						}
						++p;
					}
				}
				j+=count;
			}
		} else { // 32
			colorkeyinfo.lasthascolorkey=colorkeyinfo.usecolorkey;
			for (j=0;j<prod;) {
				U32 k;
				fread(&count,1,1,fh);
//				count=filereadbyte();
				if (count&0x80) { // run
					C32 v;
					count&=0x7f;
					++count;
					fread(&v.b,1,1,fh);
					fread(&v.g,1,1,fh);
					fread(&v.r,1,1,fh);
					fread(&v.a,1,1,fh);
					for (k=0;k<count;++k) {
						*p++=v;
					}
				} else { // lit
					++count;
					for (k=0;k<count;++k) {
						fread(&p->b,1,1,fh);
						fread(&p->g,1,1,fh);
						fread(&p->r,1,1,fh);
						fread(&p->a,1,1,fh);
						++p;
					}
				}
				j+=count;
			}
		}
		if (j!=prod)
			errorexit("error in tga decompression '%s'",fname);
	} else {
		if (tgaheaderread.bitsperpixel==24) {
			for (j=0;j<prod;++j) {
				fread(&p->b,1,1,fh);
				fread(&p->g,1,1,fh);
				fread(&p->r,1,1,fh);
				p->a=0xff;
				if (colorkeyinfo.usecolorkey) {
					if (p->c32==colorkey.c32) {
						p->a=0;
						colorkeyinfo.lasthascolorkey=1;
					}
				}
				++p;
			}
		} else { // 32
			colorkeyinfo.lasthascolorkey=colorkeyinfo.usecolorkey;
			for (j=0;j<prod;++j) {
				fread(&p->b,1,1,fh);
				fread(&p->g,1,1,fh);
				fread(&p->r,1,1,fh);
				fread(&p->a,1,1,fh);
				++p;
			}
		}
	}
	if (tgaheaderread.description&0x10) {
		fastxflip32(d);
	}
	if (!(tgaheaderread.description&0x20)) {
		fastyflip32(d);
	}
	fclose(fh);
	return d;
}
