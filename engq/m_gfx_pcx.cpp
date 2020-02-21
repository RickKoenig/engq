#include <m_eng.h>

static FILE* fh;
struct pcxheader {
	U8 manufacturer;
	U8 version;
	U8 encoding;
	U8 bits_per_pixel;
	U16 xmin,ymin;
	U16 xmax,ymax;
	U16 hres;
	U16 vres;
	U8 palette[48];
	U8 reserved;
	U8 color_planes;
	U16 bytes_per_line;
	U16 palette_type;
	U16 hscreensize,vscreensize;
	U8 filler[54];
};
static struct pcxheader pcxheader;
static struct pcxheader pcxheaderwrite={
	0xa,		// U8 manufacturer;
	0x5,		//	U8 version;
	0x1,	 	// U8 encoding;
	0x8,		// U8 bits_per_pixel;
	0x0,		// U16 xmin,ymin;
	0x0,
	0x69,		// U16 xmax,ymax;
	0x69,
	0x320,	//	U16 hres;
	0x200,	//	U16 vres;
	{0x0,0x0,0x0,		// U8 palette[48];
	0x0,0x0,0xaaU,
	0x0,0xaaU,0x0,
	0x0,0xaaU,0xaaU,
	0xaaU,0x0,0x0,
	0xaaU,0x0,0xaaU,
	0xaaU,0x55U,0x0,
	0xaaU,0xaaU,0xaaU,
	0x55U,0x55U,0x55U,
	0x55U,0x55U,0xffU,
	0x55U,0xffU,0x55U,
	0x55U,0xffU,0xffU,
	0xffU,0x55U,0x55U,
	0xffU,0x55U,0xffU,
	0xffU,0xffU,0x55U,
	0xffU,0xffU,0xffU},
	0x0,		// U8 reserved;
	0x1,		//	U8 color_planes;
	0x69U,		// U16 bytes_per_line;
	0,			// palette type
	0,0,		// hors vert screen size
	{0x0}, 		// filler
};

void gfxwrite8_pcx(const C8* fname,const struct bitmap8* d,const C32* dacs)
{
	U8 c12=12;
	U8 c193=193U;
	S32 i,j,pv;
	U8 v;
	S32 tx=d->size.x,ty=d->size.y;
	///// dx,dy,bx,by ignored //////
	pcxheaderwrite.xmax=tx-1;
	pcxheaderwrite.ymax=ty-1;
	if (tx&1)
		++tx; // make even
	pcxheaderwrite.bytes_per_line=tx;
	fh=fopen2(fname,"wb");
	fwrite(&pcxheaderwrite,1,128,fh);
//	filewrite(&pcxheaderwrite,sizeof(pcxheaderwrite));
	for (j=0;j<ty;j++)
		for (i=0;i<tx;i++) {
			if (i>=d->size.x)
				v=0;
			else
				v=d->data[j*d->size.x+i];
			if (v>=192)
				fwrite(&c193,1,1,fh);
//				filewritebyte(193);
			fwrite(&v,1,1,fh);
//			filewritebyte(v);
		}
	fwrite(&c12,1,1,fh);
//	filewritebyte(0xc);
	pv=256*3;
	while(pv>0) {
//		filewritebyte(dacs->r);
//		filewritebyte(dacs->g);
//		filewritebyte(dacs->b);
		fwrite(&dacs->r,1,1,fh);
		fwrite(&dacs->g,1,1,fh);
		fwrite(&dacs->b,1,1,fh);
		dacs++;
		pv-=3;
	}
	fclose(fh);
}

static void readpcxline(U8* dest,S32 tx)
{
	U8 pcxrepval;
	U8 pcxcount;
	while(tx>0)	{
		fread(&pcxcount,1,1,fh);
//		pcxcount=filereadbyte();
		if (pcxcount>192) {
			pcxcount-=192;
			fread(&pcxrepval,1,1,fh);
//			pcxrepval=filereadbyte();
			if (pcxcount>tx)
				errorexit("pcxcount>tx %d %d",pcxcount,tx);
			memset(dest,pcxrepval,pcxcount);
//			clear32(dest,pcxcount,pcxrepval);
			tx-=pcxcount;
			dest+=pcxcount;
		} else {
			*dest++ = pcxcount;
			--tx;
		}
	}
}

struct bitmap8* gfxread8_pcx(const C8* fname,C32* dacs)
{
	struct bitmap8 *d;
	struct bitmap8 *oneline;
	S32 j;
	S32 val;
	U8 paltype;
	S32 tx,ty;
//	logger("sizeof pcxheader is %d\n",sizeof(pcxheader));
	fh=fopen2(fname,"rb");
	fread(&pcxheader,1,128,fh);
	if (pcxheader.manufacturer!=10)
		errorexit("%s: bad manufacturer, should be 10",fname);
	if (pcxheader.encoding!=1 )
		errorexit("%s: bad encoding, should be 1");
	if (pcxheader.bits_per_pixel!=8)
		errorexit("%s: bad bits per pixel, should be 8",fname);
	if (pcxheader.color_planes!=1)
		errorexit("%s: bad color planes, should be 1",fname);
	/* if (pcxheader.xmax-pcxheader.xmin+1!=pcxheader.bytes_per_line)
		{
		sprintf(aMsgStr, "mismatch between xmax,xmin and bytes per line\n");
		ErrorExit(aMsgStr, "", "");
		}  */
	tx=pcxheader.xmax-pcxheader.xmin+1;
	ty=pcxheader.ymax-pcxheader.ymin+1;
	d=bitmap8alloc(tx,ty,-1);

	oneline=bitmap8alloc(pcxheader.bytes_per_line,1,-1);
	for (j=0;j<pcxheader.ymax-pcxheader.ymin+1;j++)	{
		readpcxline(oneline->data,pcxheader.bytes_per_line);
//		clipblit(oneline,d,0,0,0,j,tx,1);
		memcpy(&d->data[j*d->size.x],oneline->data,tx);
	}
	bitmap8free(oneline);
	if (dacs) {
		fread(&paltype,1,1,fh);
		if (paltype!=12) {
			fseek(fh,-769,SEEK_END);
//			paltype=filereadbyte();
			fread(&paltype,1,1,fh);
			if (paltype!=12)
				errorexit("%s: unknown palette format, should be 12",fname);
		}
		val=256*3;
		while(val>0) {
			fread(&dacs->r,1,1,fh);
			fread(&dacs->g,1,1,fh);
			fread(&dacs->b,1,1,fh);
			dacs->a = 0xff;
			dacs++;
			val-=3;
		} 
	}
	fclose(fh);
	return d;
}
