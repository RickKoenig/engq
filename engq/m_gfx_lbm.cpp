#include <m_eng.h>

//extern int animframes;
//int iffnoshift=0;

//#include <stdio.h>
//#include <dos.h>

//int animframes;
//int dpan;

static unsigned char FORM[4]={'F','O','R','M'};
static unsigned char  PBM[4]={'P','B','M',' '};
static unsigned char CMAP[4]={'C','M','A','P'};
static unsigned char BMHD[4]={'B','M','H','D'};
static unsigned char BODY2[4]={'B','O','D','Y'};
static unsigned char DPAN[4]={'D','P','A','N'};

static int ilbm; // 'ILBM' or 'PBM '

static int length,comp;
static int bmhd_w,bmhd_h;
static int x,y;
static unsigned char nplanes;
static unsigned char masking;
static int bmhd_xpar;
static unsigned char xasp,yasp;
static int pagew,pageh;

// lbm run length compression
static int lbmcompress;
static int lbmgetval,lbmstate,lbmcount;
#if 1
static FILE* fp;

static void fileopen(const char* fname,const char* fmode)
{
	fp = fopen2(fname,fmode);
}

static void fileclose()
{
	fclose(fp);
	fp = 0;
}

static unsigned int fileread(void *buffer,unsigned int count)
{
	return fread(buffer,1,count,fp);
}

static unsigned int filewrite(void *buffer,unsigned int count)
{
	unsigned int retval;
	retval=fwrite(buffer,1,count,fp);
	if (retval!=count)
		errorexit("error writing file\n","","");
	return retval;
}

static unsigned int fileskip(int count,int method)
{
	int offset=fseek(fp,count,method);
	if (offset!=0)
		errorexit("error in fseek");
	offset=ftell(fp);
	if (offset==-1)
		errorexit("error in ftell");
	return offset;
}

static unsigned int filereadbyte()
{
	unsigned char i;
	if (fread(&i,1,1,fp)==0) {
	//	printf("EOF ");
		return(EOF);
	}
	return(i);
}

static unsigned int filereadword()
{
	unsigned int i;
	i=filereadbyte();
	i=(i<<8)+filereadbyte();
	return(i);
}

/////// reads using little endian 8086 //////
static unsigned int filereadword2()
{
	unsigned int retval;
	retval=filereadbyte();
	return (filereadbyte()<<8) + retval;
}

static unsigned int filereadlong()
{
	unsigned int i;
	i=filereadword();
	i=(i<<16)+filereadword();
	return(i);
}

static unsigned int filereadlong2()
{
	unsigned int retval;
	retval=filereadword2();
	return (filereadword2()<<16) + retval;
}
#endif
static struct pointi2 file_dim;

static void setfiledim(int x,int y)
{
	file_dim.x=x;
	file_dim.y=y;
}

// assumes file opened and file points at beginning of chunk
// and file_dim is set;


static void readblit(struct bitmap8 *d,int sx,int sy,int dx,int dy,int tx,int ty)
{
	unsigned char *ptr;
	unsigned long fileptr=fileskip(0L,SEEK_CUR);
	fileskip(file_dim.x*sy + sx, SEEK_CUR);
	ptr = d->data + dy * d->size.x + dx;
	while(ty--) {
		fileread(ptr,tx);
		ptr += d->size.x;
		fileskip(file_dim.x - tx,SEEK_CUR);
	}
	fileskip(fileptr,SEEK_SET);
}

static unsigned int filereadbytecomp()	/* read compressed */
{
	while(lbmstate==0) {
		lbmgetval=filereadbyte();
		if (lbmgetval>128) {
			lbmstate=2;
			lbmcount=257-lbmgetval;
			lbmgetval=filereadbyte();
		} else if (lbmgetval<128) {
			lbmstate=1;
			lbmcount=lbmgetval+1;
		}
	}
	if (lbmstate==1) {
		lbmgetval=filereadbyte();
		lbmcount--;
		if (lbmcount==0)
			lbmstate=0;
		return(lbmgetval);
	} else {
		lbmcount--;
		if (lbmcount==0)
			lbmstate=0;
		return(lbmgetval);
	}
}

static unsigned int filereadbyteq()
{
	//unsigned int i;
	if (lbmcompress)
		return(filereadbytecomp());
	return(filereadbyte());
}

static unsigned int filereadwordq()
{
	unsigned int i;
	i=filereadbyteq();
	return((i<<8)+filereadbyteq());
}

static void setcompress(int c)
{
	lbmcompress=c;
}

//////////////////////////// READ ILBM //////////////////////////////////////
static void readbody(struct bitmap8 *d)
{
	int tx,ty;
	int i,x,pow,row,plane,across,val;
	tx=d->size.x;
	ty=d->size.y;
	if (ilbm) {
		for (row=0;row<ty;row++) {
			for (plane=0;plane<nplanes;plane++) {
				pow=1<<plane;
				x=0;
				for (across=0;across<((15+tx)>>4);across++)	{
					val=filereadwordq();
					for (i=0;i<16;i++) {
						if (val&0x8000)
							clipputpixel8(d,x,row,clipgetpixel8(d,x,row)+pow);
						val<<=1;
						x++;
					}
				}
			}
		}
	} else if (comp) {
		for (row=0;row<bmhd_h;row++) {
			for (across=0;across<tx;across++) {
				val=filereadbyteq();
				clipputpixel8(d,across,row,val);
			}
			if (tx&1)
				filereadbyteq();
		}
	} else {
		setfiledim((tx+1)&~1,ty);
		readblit(d,0,0,0,0,tx,ty);
	}
}

static void readcmap(C32* cmap)
{
	while(length>0) {
		cmap->r = filereadbyte();
		cmap->g = filereadbyte();
		cmap->b = filereadbyte();
		cmap->a = 0xff;
		cmap++;
		length-=3;
	}
}

static void readbmhd()
{
	bmhd_w=filereadword();
	bmhd_h=filereadword();
	x=filereadword();
	y=filereadword();
	nplanes=filereadbyte();
	masking=filereadbyte();
	setcompress(comp=filereadbyte());
	filereadbyte();
	bmhd_xpar=filereadword();
	xasp=filereadbyte();
	yasp=filereadbyte();
	pagew=filereadword();
	pageh=filereadword();
	if(masking==1)
		nplanes++;
}

struct bitmap8 *gfxread8_lbm(const C8* fname,struct C32* dacs)
//struct bitmap8 *lbmread(struct rgb24 *dacs)
{
	fileopen(fname,"rb");
	struct bitmap8 *d;
	static char prop[5];
	fileread((unsigned char *)prop,4);
	if (strcmp(prop,"FORM"))
		errorexit("file '%s' not a form",fname);
	fileskip(4,SEEK_CUR);
	fileread((unsigned char *)prop,4);
	if (!strcmp(prop,"ILBM"))
		ilbm=1;
	else if (!strcmp(prop,"PBM "))
		ilbm=0;
	else
		errorexit("file '%s' not an ilbm or a pbm",fname);
	while(fileread((unsigned char *)prop,4)==4) {
		length=filereadlong();
		if (!strcmp(prop,"BMHD")) {
			readbmhd();
		} else if (!strcmp(prop,"CMAP") && dacs) {
			readcmap(dacs);
#define DUMPDACS
#ifdef DUMPDACS
			logger("dumpdacs of '%s'\n",fname);
			for (S32 i=0;i<256;++i) {
				logger("\t dac %3d : (%3d,%3d,%3d)\n",i,dacs[i].r,dacs[i].g,dacs[i].b);
			}
#endif
		} else if (!strcmp(prop,"BODY")) {
			d=bitmap8alloc(bmhd_w,bmhd_h,0);
			readbody(d);
			break;
		} else	fileskip((length+1)&~1,SEEK_CUR);
	}
	fileclose();
	return d;
}
