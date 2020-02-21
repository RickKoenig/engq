//#define SCRATCHR
#define LODER
#define LODEW

#include <m_eng.h>
//#include <png.h>
#ifdef LODEW
#define LODE
#endif
#ifdef LODER
#define LODE
#endif
#ifdef LODE
#include "LodePNG/lodepng.h"
#endif
// relies on engine7test/n_deflate.cpp
// #include "n_deflatecpp.h"

#ifdef SCRATCHR
#define PNGHEADSIZE 8
static U8 pngheader[PNGHEADSIZE]={0x89,'P','N','G',0x0d,0x0a,0x1a,0x0a};
static U32 bpp;

#pragma pack(1)
struct ihdr {
	S32 wid,hit;
	U8 bitdepth;
	U8 coltype;
	U8 comp;
	U8 filt;
	U8 interlace;
};
#pragma pack()
#endif
// bad load, return something anyways
static bitmap32* bad(const C8* badstr="bad")
{
	colorkeyinfo.lasthascolorkey=0;
	U32 len=strlen(badstr);
	bitmap32* r=bitmap32alloc(len*8*2,32,C32LIGHTGREEN);
	outtextxyc32(r,r->size.x/2,r->size.y/2,C32BLACK,badstr);
	return r;
}

#ifdef SCRATCHR
static U32 swapbyteorder(U32 i)
{
	return ((i&0xff)<<24)+((i&0xff00)<<8)+((i&0xff0000)>>8)+((i&0xff000000)>>24);
}

// a = left, b = above, c = upper left
static U32 PaethPredictor(U32 a,U32 b,U32 c)
{
	S32 p = (S32)a + (S32)b - (S32)c;	// initial estimate
	S32 pa = abs(p - (S32)a);			// distances to a, b, c
	S32 pb = abs(p - (S32)b);
	S32 pc = abs(p - (S32)c);
// return nearest of a,b,c,
// breaking ties in order a,b,c.
	if (pa <= pb &&  pa <= pc)
		return a;
	else if (pb <= pc)
		return b;
	return c;
}

static C32 pal[256];
static U32 npal;

static void showpal()
{
//	U32 i;
//	for (i=0;i<npal;++i)
//		logger("pal at %3d: is (%3d,%3d,%3d,%3d)\n",i,pal[i].r,pal[i].g,pal[i].b,pal[i].a);
}

/*extern "C" {
	int uncompress(U8* dest, U32* destLen, U8* source, U32 sourceLen);
}
*/
#endif

void fixupRGBA(std::vector<unsigned char>& bs)
{
	U32 i=0,len=bs.size();
	while(i<len) {
		U8 t = bs[i];
		bs[i] = bs[i+2];
		bs[i+2] = t;
		i+=4;
	}
}

// from Lode
#ifdef LODER
struct bitmap32* gfxread32_png(const C8* name)
{
	//load and decode
	std::vector<unsigned char> image;
	U32 width, height;
	U32 error = LodePNG::decode(image, width, height, name);
	//if there's an error, display it
	if(error != 0)
		return bad(LodePNG_error_text(error));
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
	bitmap32* ret = bitmap32alloc(width,height,0);
	fixupRGBA(image);
	copy(image.begin(),image.end(),(U8*)ret->data);
	colorkeyinfo.lasthascolorkey=0;
	S32 prod = width*height;
	S32 i;
	for (i=0;i<prod;++i)
		if (ret->data[i].a!=0xff)
			colorkeyinfo.lasthascolorkey=1;
	return ret;
}
#endif
#ifdef SCRATCHR
// from scratch
struct bitmap32* gfxread32_png(const C8* name)
{
	U32 tcla=0;
//	logger("\n-----------------------------------\nreading '%s'\n",name);
	colorkeyinfo.lasthascolorkey=0;
	bpp=0;
	FILE* fh=fopen2(name,"rb");
// read sig
	U8 header[8];
	fread(&header,1,sizeof(header),fh);
	S32 r=memcmp(pngheader,header,PNGHEADSIZE);
//	logger("result of png header test %d\n",r);
	if (r) {
		fclose(fh);
		return bad("bad png header");
	}
// read chunks
	ihdr hdr;
	U8* ud=0;
	U32 tcl=0;
	U8* cd=0;
	while(1) {
		U32 cl=filereadU32BE(fh); // get len
		if (cl==~0U) // eof
			break;
		static C8 cn[5];
		fread(cn,1,4,fh); // get type (id)
//		logger("chunk id '%s' len %d\n",cn,cl);
		if (!strcmp("IHDR",cn)) {
//			logger("found IHDR\n");
			if (cl!=sizeof(hdr))
				errorexit("bad IHDR in '%s'",name);
			fread(&hdr,1,sizeof(hdr),fh); // read data
			hdr.wid=swapbyteorder(hdr.wid);
			hdr.hit=swapbyteorder(hdr.hit);
//			logger("wid %d,hit %d,bitdep %d,coltype %d,comp %d,filt %d,interlace %d\n",
//				hdr.wid,hdr.hit,hdr.bitdepth,hdr.coltype,hdr.comp,hdr.filt,hdr.interlace);
			if (hdr.bitdepth!=8 || hdr.comp!=0 || hdr.filt!=0 || hdr.interlace!=0) {
				fclose(fh);
				// this
//				errorexit("unk png type '%s'",name);
				// or this
				logger("unk png type '%s'\n",name);
				return bad("unk png type (a)");
			}
			if (hdr.coltype==6) { // RGBA
//				logger("rgba\n");
				bpp=32;
			} else if (hdr.coltype==2) { // RGB
//				logger("rgba\n");
				bpp=24;
			} else if (hdr.coltype==3) { // PALETTE
//				logger("palmode = true\n");
				bpp=8;
				memset(pal,0,sizeof(pal));	// careful, (must be p.o.d.)
				npal=0;
//				return bad();
			} else { // anything else
				fclose(fh);
				// this
//				errorexit("unk png type '%s'",name);
				// or this
				logger("unk png type '%s'\n",name);
				return bad("unk png type (b)");
			}
		} else if(!strcmp("IDAT",cn)) {
//			logger("found IDAT\n");
			if (!cd) { // no compress buffer, make one
				cd=new U8[cl];
				tcla=cl;
			}
			if (tcl+cl>tcla) { // not enough room in comp buffer, make room
//				logger("changing tcla from %d ",tcla);
				tcla = (tcl+cl)<<1;
//				logger("to %d\n",tcla);
				U8* t=new U8[tcla];
				memcpy(t,cd,tcl);
				delete[] cd;
				cd=t;
			}
			fread(cd+tcl,1,cl,fh);
			tcl+=cl;
		} else if(!strcmp("IEND",cn)) {
//			logger("found IEND\n"); // done
			break;
		} else if(!strcmp("PLTE",cn)) {
//			logger("found PLTE\n"); // color palette (needs work)
			U32 i,j;
			for (i=0,j=0;i<cl;i+=3,++j) {
				pal[j].r=filereadU8(fh);
				pal[j].g=filereadU8(fh);
				pal[j].b=filereadU8(fh);
				pal[j].a=0xff;
			}
			npal=j;
		} else if(!strcmp("tRNS",cn)) { // process optional chunk
//			logger("found tRNS\n"); // alpha for palette
			if (hdr.coltype==3)
				colorkeyinfo.lasthascolorkey=1;
			if (bpp==8) {
				U32 j;
				for (j=0;j<cl;++j)
					pal[j].a=filereadU8(fh);
			} else
				fseek(fh,cl,SEEK_CUR); // skip data
		} else {
//			logger("found something else\n");
			fseek(fh,cl,SEEK_CUR); // skip data
		}
		fseek(fh,4,SEEK_CUR); // skip CRC
	}
	fclose(fh);

	if (!cd)
		errorexit("no idat");
	U32 prod=0;
	switch(bpp) {
	case 8:
		prod=(hdr.wid+1)*hdr.hit;
		break;
	case 24:
		prod=(hdr.wid*3+1)*hdr.hit;
		break;
	case 32:
		prod=(hdr.wid*4+1)*hdr.hit;
		break;
	default:
		errorexit("no bpp");
	}

/* ===========================================================================
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.
*/
#if 0
	U8* dest = new U8[prod];
	U32 destLen;
	destLen = prod;
	U8* source = cd;
	U32 sourceLen = cl;
	int ret = uncompress(dest, &destLen, source, sourceLen);
	errorexit("ret = %d",ret);
#endif
	ud=deflate_decompress(cd,tcl,prod);
	delete[] cd;
//			fseek(fh,cl,SEEK_CUR); // skip data

	if (ud) {
		if (bpp==8) { // paletted mode
			showpal();
			S32 i,j;
			bitmap8* r=bitmap8alloc(hdr.wid,hdr.hit,0); // raw values (or is it the other way around)
			bitmap8* f=bitmap8alloc(hdr.wid+1,hdr.hit+1,0); // one larger in x and y, filtered values
			bitmap8* g=bitmap8alloc(hdr.wid,hdr.hit,0); // final return, true x and y, filtered values
			U8* filt=new U8[hdr.hit];
			U8* udp=ud;
			U8* rp=r->data;
	// copy over raw data
			for (j=0;j<hdr.hit;++j) {
				filt[j]=*udp++; // get filter for scanline
				for (i=0;i<hdr.wid;++i) {
					*rp=*udp++;
					++rp;
				}
			}
	// filter data
			U8* fp=f->data+(hdr.wid+1)+1;
			rp=r->data;
			for (j=0;j<hdr.hit;++j) {
				switch(filt[j]) {
				case 0:	// none
					for (i=0;i<hdr.wid;++i) {
						*fp++ = *rp++;
					}
					break;
				case 1: // sub
					for (i=0;i<hdr.wid;++i) {
						U8* fpl=fp-1;
						*fp = *fpl + *rp;
						++fp;
						++rp;
					}
					break;
				case 2: // up
					for (i=0;i<hdr.wid;++i) {
						U8* fpu=fp-(hdr.wid+1);
						*fp = *fpu + *rp;
						++fp;
						++rp;
					}
					break;
				case 3: // average
					for (i=0;i<hdr.wid;++i) {
						U8* fpl=fp-1;
						U8* fpu=fp-(hdr.wid+1);
						U32 rv = *fpl + *fpu;	rv >>= 1;	*fp = rv + *rp;
						++fp;
						++rp;
					}
					break;
				case 4: // Paeth
					for (i=0;i<hdr.wid;++i) {
						U8* fpl=fp-1;
						U8* fpu=fp-(hdr.wid+1);
						U8* fpul=fp-(hdr.wid+1)-1;
						U32 rv = PaethPredictor(*fpl,*fpu,*fpul);	*fp = rv + *rp;
						++fp;
						++rp;
					}
					break;
				default:
					errorexit("unk filter %d in '%s'",filt[j],name);
					break;
				}
				++fp;
			}
//			for (j=0;j<hdr.hit;++j)
//				logger("pal filt %4d: is %d\n",j,filt[j]);
			delete[] ud;
			delete[] filt;
//			colorkeyinfo.lasthascolorkey=0;
			bitmap8free(r);
	// reduce filtered (remove extra row and column)
			for (j=0;j<f->size.y;++j)
				for (i=0;i<f->size.x;++i)
					clipputpixel8(g,i,j,clipgetpixel8(f,i+1,j+1));
			bitmap8free(f);
			bitmap32* g32=bitmap32alloc(g->size.x,g->size.y,C32BLACK);
			for (j=0;j<g->size.y;++j) {
				for (i=0;i<g->size.x;++i) {
					U8 gs=clipgetpixel8(g,i,j);
//					logger("%d",gs);
//					C32 c=C32(gs,gs,gs,255);
					clipputpixel32(g32,i,j,pal[gs]); // palettize
				}
//				logger("\n");
			}
			bitmap8free(g);
			return g32;
		} else if (bpp==24) { // true color mode rgb
			colorkeyinfo.lasthascolorkey=0;
			bitmap32* r=bitmap32alloc(hdr.wid,hdr.hit,C32BLACK); // raw values (or is it the other way around)
			bitmap32* f=bitmap32alloc(hdr.wid+1,hdr.hit+1,C32(0,0,0,0)); // one larger in x and y, filtered values
			bitmap32* g=bitmap32alloc(hdr.wid,hdr.hit,C32BLACK); // final return, true x and y, filtered values
			S32 i,j;
			U8* filt=new U8[hdr.hit];
			U8* udp=ud;
			C32* rp=r->data;
	// copy over raw data
			for (j=0;j<hdr.hit;++j) {
				filt[j]=*udp++; // get filter for scanline
				for (i=0;i<hdr.wid;++i) {
					rp->r=*udp++;
					rp->g=*udp++;
					rp->b=*udp++;
					rp->a=0xff;
					++rp;
				}
			}
	// filter data
			C32* fp=f->data+(hdr.wid+1)+1;
			rp=r->data;
			for (j=0;j<hdr.hit;++j) {
				switch(filt[j]) {
				case 0:	// none
					for (i=0;i<hdr.wid;++i) {
						*fp++ = *rp++;
					}
					break;
				case 1: // sub
					for (i=0;i<hdr.wid;++i) {
						C32* fpl=fp-1;
						fp->r = fpl->r + rp->r;
						fp->g = fpl->g + rp->g;
						fp->b = fpl->b + rp->b;
						fp->a = 0xff;
						++fp;
						++rp;
					}
					break;
				case 2: // up
					for (i=0;i<hdr.wid;++i) {
						C32* fpu=fp-(hdr.wid+1);
						fp->r = fpu->r + rp->r;
						fp->g = fpu->g + rp->g;
						fp->b = fpu->b + rp->b;
						fp->a = 0xff;
						++fp;
						++rp;
					}
					break;
				case 3: // average
					for (i=0;i<hdr.wid;++i) {
						C32* fpl=fp-1;
						C32* fpu=fp-(hdr.wid+1);
						U32 rv = fpl->r + fpu->r;	rv >>= 1;	fp->r = rv + rp->r;
						U32 gv = fpl->g + fpu->g;	gv >>= 1;	fp->g = gv + rp->g;
						U32 bv = fpl->b + fpu->b;	bv >>= 1;	fp->b = bv + rp->b;
																fp->a = 0xff;
						++fp;
						++rp;
					}
					break;
				case 4: // Paeth
					for (i=0;i<hdr.wid;++i) {
						C32* fpl=fp-1;
						C32* fpu=fp-(hdr.wid+1);
						C32* fpul=fp-(hdr.wid+1)-1;
						U32 rv = PaethPredictor(fpl->r,fpu->r,fpul->r);	fp->r = rv + rp->r;
						U32 gv = PaethPredictor(fpl->g,fpu->g,fpul->g);	fp->g = gv + rp->g;
						U32 bv = PaethPredictor(fpl->b,fpu->b,fpul->b);	fp->b = bv + rp->b;
																		fp->a = 0xff;
						++fp;
						++rp;
					}
					break;
				default:
					errorexit("unk filter %d in '%s'",filt[j],name);
					break;
				}
				++fp;
			}
//			for (j=0;j<hdr.hit;++j)
//				logger("filt %4d: is %d\n",j,filt[j]);
			delete[] ud;
			delete[] filt;
			bitmap32free(r);
	// reduce filtered (remove extra row and column)
			for (j=0;j<f->size.y;++j)
				for (i=0;i<f->size.x;++i)
					clipputpixel32(g,i,j,clipgetpixel32(f,i+1,j+1));
			bitmap32free(f);
			return g;
		} else { // true color mode rgba
			colorkeyinfo.lasthascolorkey=1;
			bitmap32* r=bitmap32alloc(hdr.wid,hdr.hit,C32BLACK); // raw values (or is it the other way around)
			bitmap32* f=bitmap32alloc(hdr.wid+1,hdr.hit+1,C32(0,0,0,0)); // one larger in x and y, filtered values
			bitmap32* g=bitmap32alloc(hdr.wid,hdr.hit,C32BLACK); // final return, true x and y, filtered values
			S32 i,j;
			U8* filt=new U8[hdr.hit];
			U8* udp=ud;
			C32* rp=r->data;
	// copy over raw data
			for (j=0;j<hdr.hit;++j) {
				filt[j]=*udp++; // get filter for scanline
				for (i=0;i<hdr.wid;++i) {
					rp->r=*udp++;
					rp->g=*udp++;
					rp->b=*udp++;
					rp->a=*udp++;
					++rp;
				}
			}
	// filter data
			C32* fp=f->data+(hdr.wid+1)+1;
			rp=r->data;
			for (j=0;j<hdr.hit;++j) {
				switch(filt[j]) {
				case 0:	// none
					for (i=0;i<hdr.wid;++i) {
						*fp++ = *rp++;
					}
					break;
				case 1: // sub
					for (i=0;i<hdr.wid;++i) {
						C32* fpl=fp-1;
						fp->r = fpl->r + rp->r;
						fp->g = fpl->g + rp->g;
						fp->b = fpl->b + rp->b;
						fp->a = fpl->a + rp->a;
						++fp;
						++rp;
					}
					break;
				case 2: // up
					for (i=0;i<hdr.wid;++i) {
						C32* fpu=fp-(hdr.wid+1);
						fp->r = fpu->r + rp->r;
						fp->g = fpu->g + rp->g;
						fp->b = fpu->b + rp->b;
						fp->a = fpu->a + rp->a;
						++fp;
						++rp;
					}
					break;
				case 3: // average
					for (i=0;i<hdr.wid;++i) {
						C32* fpl=fp-1;
						C32* fpu=fp-(hdr.wid+1);
						U32 rv = fpl->r + fpu->r;	rv >>= 1;	fp->r = rv + rp->r;
						U32 gv = fpl->g + fpu->g;	gv >>= 1;	fp->g = gv + rp->g;
						U32 bv = fpl->b + fpu->b;	bv >>= 1;	fp->b = bv + rp->b;
						U32 av = fpl->a + fpu->a;	av >>= 1;	fp->a = av + rp->a;
						++fp;
						++rp;
					}
					break;
				case 4: // Paeth
					for (i=0;i<hdr.wid;++i) {
						C32* fpl=fp-1;
						C32* fpu=fp-(hdr.wid+1);
						C32* fpul=fp-(hdr.wid+1)-1;
						U32 rv = PaethPredictor(fpl->r,fpu->r,fpul->r);	fp->r = rv + rp->r;
						U32 gv = PaethPredictor(fpl->g,fpu->g,fpul->g);	fp->g = gv + rp->g;
						U32 bv = PaethPredictor(fpl->b,fpu->b,fpul->b);	fp->b = bv + rp->b;
						U32 av = PaethPredictor(fpl->a,fpu->a,fpul->a);	fp->a = av + rp->a;
						++fp;
						++rp;
					}
					break;
				default:
					errorexit("unk filter %d in '%s'",filt[j],name);
					break;
				}
				++fp;
			}
//			for (j=0;j<hdr.hit;++j)
//				logger("filt %4d: is %d\n",j,filt[j]);
			delete[] ud;
			delete[] filt;
			bitmap32free(r);
	// reduce filtered (remove extra row and column)
			for (j=0;j<f->size.y;++j)
				for (i=0;i<f->size.x;++i)
					clipputpixel32(g,i,j,clipgetpixel32(f,i+1,j+1));
			bitmap32free(f);
			return g;
		}
	} else
		return bad("bad decomp");
}
#endif
// use Lode
void gfxwrite32_png(const C8* fname,const struct bitmap32* d32)
{
#ifdef LODEW
	//	png_structp spng = 0;
	//	png_set_invert_mono(spng);
	//check if user gave a filename
	U32 prod = d32->size.x * d32->size.y;
	std::vector<unsigned char> image;
	image.resize(prod * sizeof(U32));
	C32* imagedata = (C32*)&image[0];
	copy(d32->data,d32->data+prod,imagedata);
	fixupRGBA(image);
	LodePNG::encode(fname, image, d32->size.x, d32->size.y);
#endif
}
