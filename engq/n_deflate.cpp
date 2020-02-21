// moved from engqtest/engine7test to engq because it's been very useful

//#include <string.h>
//#include <engine7cpp.h>
//#include "deflatecpp.h"

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "n_deflatecpp.h"
#define MAXBITS 15
static unsigned int staticdistlens[32]={
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
};

static unsigned int staticlitlens[288]={
	7,7,7,7, 7,7,7,7, 7,7,7,7, 7,7,7,7,
	7,7,7,7, 7,7,7,7,

	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,

	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,

	8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,

	8,8,8,8, 8,8,8,8,

	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,

	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
	9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
};

struct hufftab {
	unsigned int val;
	unsigned int nbits;
};

static struct hufftab codehuff[128];
static struct hufftab litlenhuff[4096*8],disthuff[4096*8];
static unsigned int lithist[256];


//static struct hufftab litlen[512];
//static int huffgenptr;

struct litcode {
	unsigned int extrabits;
	unsigned int len;
};

// offset by 257 (257 to 285)
static struct litcode litcodetable[31]={
	{0,  3},  // 257
	{0,  4},  // 258
	{0,  5},  // 259
	{0,  6},  // 260
	{0,  7},  // 261
	{0,  8},  // 262
	{0,  9},  // 263
	{0, 10},  // 264

	{1, 11},  // 265
	{1, 13},  // 266
	{1, 15},  // 267
	{1, 17},  // 268

	{2, 19},  // 269
	{2, 23},  // 270
	{2, 27},  // 271
	{2, 31},  // 272

	{3, 35},  // 273
	{3, 43},  // 274
	{3, 51},  // 275
	{3, 59},  // 276

	{4, 67},  // 277
	{4, 83},  // 278
	{4, 99},  // 279
	{4,115},  // 280

	{5,131},  // 281
	{5,163},  // 282
	{5,195},  // 283
	{5,227},  // 284

	{0,258},  // 285
	{0,  0},  // 286 error
	{0,  0},  // 287 error
};

struct distcode {
	unsigned int extrabits;
	unsigned int dist;
};

static struct distcode distcodetable[32]={
	{ 0,     1},    //  0
	{ 0,     2},    //  1
	{ 0,     3},    //  2
	{ 0,     4},    //  3
	{ 1,     5},    //  4
	{ 1,     7},    //  5
	{ 2,     9},    //  6
	{ 2,    13},    //  7
	{ 3,    17},    //  8
	{ 3,    25},    //  9
	{ 4,    33},    // 10
	{ 4,    49},    // 11
	{ 5,    65},    // 12
	{ 5,    97},    // 13
	{ 6,   129},    // 14
	{ 6,   193},    // 15
	{ 7,   257},    // 16
	{ 7,   385},    // 17
	{ 8,   513},    // 18
	{ 8,   769},    // 19
	{ 9,  1025},    // 20
	{ 9,  1537},    // 21
	{10,  2049},    // 22
	{10,  3073},    // 23
	{11,  4097},    // 24
	{11,  6145},    // 25
	{12,  8193},    // 26
	{12, 12289},    // 27
	{13, 16385},    // 28
	{13, 24577},    // 29
	{ 0,     0},    // 30
	{ 0,     0},    // 31
};

static unsigned char codexlate[19]={16,17,18,0,8,  7,9,6,10,5,  11,4,12,3,13,  2,14,1,15};

/*static void genhuff(int valstart,int valend,int nbits)
{
	int i,val;
	int t=1<<(9-nbits);
	for (val=valstart;val<=valend;val++) {
		for (i=0;i<t;i++) {
			litlen[huffgenptr].nbits=nbits;
			litlen[huffgenptr].val=val;
			logger("idx %3d: val %d, nbits %d\n",huffgenptr,litlen[huffgenptr].val,litlen[huffgenptr].nbits);
			huffgenptr++;
		}
	}
}
*/

/*
static char *printbits(unsigned int bits,int nbits)
{
	static char sbits[64];
	char *p=sbits;
	while(nbits--)
		*p++='0'+(1&(bits>>nbits));
	*p='\0';
	return sbits;
}
*/
static unsigned int getbits(unsigned char **pp,unsigned int *bitpos,unsigned int nbits)
{
	unsigned int ret=0;
	unsigned int i;
	for (i=0;i<nbits;i++) {
//		if (*bitpos==0)
//			logger("<<<< get more bits '%s' %02x >>>>\n",printbits(**pp,8),**pp);
		ret+=(1&(**pp>>*bitpos))<<i;
		(*bitpos)++;
		if (*bitpos==8) {
			*bitpos=0;
			(*pp)++;
		}
	}
	return ret;
}

static unsigned int peekhuffbits(unsigned char **pp,unsigned int bitpos,unsigned int nbits)
{
	unsigned int bp=bitpos;
	unsigned char *p=*pp;
	unsigned int ret=0;
	while(nbits) {
		ret=(ret<<1)+(1&(*p>>bp));
		bp++;
		if (bp==8) {
			bp=0;
			p++;
		}
		nbits--;
	}
	return ret;
}

static void skipbits(unsigned char **pp,unsigned int *bitpos,unsigned int nbits)
{
	while(nbits) {
//		if (*bitpos==0)
//			logger("<<<< get more bits '%s' %02x >>>>\n",printbits(**pp,8),**pp);
		(*bitpos)++;
		if (*bitpos==8) {
			*bitpos=0;
			(*pp)++;
		}
		nbits--;
	}
}

static void nearestbyte(unsigned char **pp,unsigned int *bitpos)
{
	U32 bm = *bitpos & 7; // just making sure..
	if(bm)
		skipbits(pp,bitpos,8-bm);
}

/*
static void alignbits(unsigned char **pp,unsigned int *bitpos)
{
	if (*bitpos==0) // already aligned
		return;
	*bitpos=0;
	(*pp)++;
}
*/
/*static unsigned int getlitlen(unsigned char **pp,unsigned int *bitpos)
{
	unsigned int bits;
	bits=peekbits(pp,bitpos,9);
	printbits(bits,9);
	logger("lit value = %d, nbits %d\n",litlen[bits].val,litlen[bits].nbits);
	skipbits(pp,bitpos,litlen[bits].nbits);
	return litlen[bits].val;
}
*/
static void genhuff(struct hufftab *ht,unsigned int maxbits,unsigned int *codelens,unsigned int ncodes)
{
	unsigned int i,j,p=0;
	unsigned int nc;
	for (i=0;i<ncodes;i++)
		if (codelens[i]>maxbits)
			errorexit("maxhuffbits exceeded: maxbits %d, found %d",maxbits,codelens[i]);
	for (i=1;i<=maxbits;i++) {
		for (j=0;j<ncodes;j++) {
			if (codelens[j]==i) {
				nc=1<<(maxbits-i);
				while(nc--) {
					if (p==(1U<<maxbits))
						errorexit("huff table overflow");
					ht[p].nbits=i;
					ht[p].val=j;
					p++;
				}
			}
		}
	}
	if (p!=(1U<<maxbits))
		errorexit("p %d != maxbits %d",p,1<<maxbits);
//	else
//	logger("p %d == maxbits %d\n",p,1<<maxbits);
//	logger("huff table generated..\n");
//	for (i=0;i<(1<<maxbits);i++)
//		logger("bits %s: code %3d bitlen %2d\n",printbits(i,maxbits),ht[i].val,ht[i].nbits);
}

/*static void genhuff2(struct hufftab *ht,int maxbits,int *codelens,int ncodes)
{
	int i,j,p=0;
	int nc;
	for (i=0;i<ncodes;i++)
		if (codelens[i]>maxbits)
			errorexit("maxhuffbits exceeded: maxbits %d, found %d",maxbits,codelens[i]);
//	for (i=1;i<=maxbits;i++) {
		for (j=0;j<ncodes;j++) {
//			if (codelens[j]==i) {
				nc=1<<(maxbits-i);
				while(nc--) {
					if (p==(1<<maxbits))
						errorexit("huff table overflow");
					ht[p].nbits=codelens[j];
					ht[p].val=j;
					p++;
				}
//			}
		}
//	}
}
*/
static unsigned int gethuff(unsigned char **pp,unsigned int *bitpos,struct hufftab *ht,unsigned int maxbits)
{
	unsigned int bits;
	bits=peekhuffbits(pp,*bitpos,maxbits);
//	printbits(bits,9);
//	logger("lit value = %d, nbits %d\n",litlen[bits].val,litlen[bits].nbits);
	skipbits(pp,bitpos,ht[bits].nbits);
	return ht[bits].val;
}

U8 *deflate_decompress(unsigned char *cd,int compsize,int uncompsize)
{
//	bool noluck=false;
	int unccnt=0; // number of pixels copyied..
	unsigned int i;
	int cnt;
	unsigned int bitpos=0;	// 0 to 7
	unsigned int header1,header2;
	unsigned int final,comptype;
	unsigned int nlit,ndist,ncode;
	unsigned int codecodelens[19];
	unsigned int newcodecodelens[19];
	unsigned int litdistlens[288+32];
//	unsigned int litlens[286];
//	C32 *p32,c32tab[256];
//	U8* p8;
	U8* cdsave=new U8[compsize+32]; // safety margin
	memcpy(cdsave,cd,compsize);
	memset(cdsave+compsize,0,32);
	U8* cp=cdsave;
	U8* cpend=cp+compsize;
//	unsigned int testbits;
//	struct bitmap32 *r,*rf;
//	struct bitmap8* r;
	header1=*cp++;
	header2=*cp++;
	if ((header1*256+header2)%31)
		errorexit("tex header1 and header2 fails modulo 31 check");
	if (header2&0x20)
		errorexit("tex header2 has a preset dictionary");
	if ((header1&0xf)!=0x8) {
		// this
//		errorexit("tex header1 not right was $%02X supposed to end in 8",header1);
		// or this
		logger("tex header1 not right was $%02X supposed to end in 8\n",header1);
		delete[] cdsave;
		return 0;
	}
//	if (header2!=0xDA)
//		errorexit("tex header2 not right was $%02X supposed to be $DA",header2);
//	logger("------------------- deflate --------------------\n");
	U8* p8=new U8[uncompsize];
	memset(p8,0,uncompsize);	// debug..
	while(1) {
		cnt=5000;
		unsigned int litptr=0;
		unsigned int litcode=16; // unused code
		final=getbits(&cp,&bitpos,1);
//		if (final!=1)
//			;//errorexit("not the final block");
//		if (final)
//			logger("final block\n");
//		else
//			logger("not final block\n");
		comptype=getbits(&cp,&bitpos,2);
		if (comptype==2) { // dynamic huffman
//			logger("comptype == 2\n");
			nlit=257+getbits(&cp,&bitpos,5);
			ndist=1+getbits(&cp,&bitpos,5);
			ncode=4+getbits(&cp,&bitpos,4);
			memset(codecodelens,0,sizeof(codecodelens));
			memset(newcodecodelens,0,sizeof(newcodecodelens));
			memset(litdistlens,0,sizeof(litdistlens));
//			logger("ncode = %d, nlit = %d, ndist = %d\n",ncode,nlit,ndist);
	// build code len code
			for (i=0;i<ncode;i++)
				codecodelens[i]=getbits(&cp,&bitpos,3);
//			for (i=0;i<ncode;i++)
//				logger("code lens for %2d (%2d): %d\n",i,codexlate[i],codecodelens[i]);
			for (i=0;i<19;i++)
				newcodecodelens[codexlate[i]]=codecodelens[i];
			genhuff(codehuff,7,newcodecodelens,19);
//		for (i=0;i<128;i++)
//			codehuff[i].val=codexlate[codehuff[i].val];
//		for (i=0;i<128;i++)
//			logger("%s: val %2d, nbits %d\n",printbits(i,7),codehuff[i].val,codehuff[i].nbits);
//		for (i=0;i<128;i++)
//			logger("bits %3d: val %2d, nbits %d\n",i,codehuff[i].val,codehuff[i].nbits);
			while(litptr<nlit+ndist && cnt--) {
				unsigned int lc,litlen=~0;
				lc=gethuff(&cp,&bitpos,codehuff,7);
//				logger("lc is %d\n",lc);
				if (lc<=15) {
					litcode=lc;
					litlen=1;
				} else if (lc==16) {
					if (litcode==16)
						errorexit("prev used when no prev");
					litlen=3+getbits(&cp,&bitpos,2);
				} else if (lc==17) {
					litcode=0;
					litlen=3+getbits(&cp,&bitpos,3);
				} else if (lc==18) {
					litcode=0;
					litlen=11+getbits(&cp,&bitpos,7);
				} else
					errorexit("bad lit len code %d\n",lc);
//				logger("%3d: lit codelen %2d and rep %3d\n",litptr,litcode,litlen);
				for (i=0;i<litlen;i++) {
/*					if (litptr==0)
						logger("litlens\n");
					else if (litptr==nlit)
						logger("distlens\n");
					if (litcode!=0)
						if (litptr<nlit)
							logger("litlen for %3d: is %d\n",litptr,litcode);
						else
							logger("distlen for %3d: is %d\n",litptr-nlit,litcode); */
					litdistlens[litptr]=litcode;
					litptr++;
				}
			}
/*			logger("litcodes...\n");
			for (i=0;i<nlit;++i)
				logger("%3d: len %d\n",i,litdistlens[i]);
			logger("distcodes...\n");
			for (i=nlit;i<nlit+ndist;++i)
				logger("%3d: len %d\n",i-nlit,litdistlens[i]); */
			genhuff(litlenhuff,MAXBITS,litdistlens,nlit);
			genhuff(disthuff,MAXBITS,litdistlens+nlit,ndist);
		} else if (comptype==1) { // static huffman
//			logger("comptype == 1\n");
//			noluck=true;
			genhuff(litlenhuff,MAXBITS,staticlitlens,288);
			genhuff(disthuff,MAXBITS,staticdistlens,32);
			for (i=0;i<(1<<MAXBITS);i++)
				if (litlenhuff[i].val<24)
					litlenhuff[i].val+=256;
				else if (litlenhuff[i].val<168)
					litlenhuff[i].val-=24;
				else if (litlenhuff[i].val<176)
					litlenhuff[i].val+=112;
				else
					litlenhuff[i].val-=32;
	//		exit(0);
		} else if (comptype==0) { // no compression
//			logger("comptype == 0\n");
			nearestbyte(&cp,&bitpos);
			U32 ulen = getbits(&cp,&bitpos,16);
			U32 ulenn = getbits(&cp,&bitpos,16);
//			logger("ulen = %08x, ulenn %08x\n",ulen,ulenn);
			memcpy(p8+unccnt,cp,ulen);
			unccnt+=ulen;
			cp+=ulen;
		} else {
//			errorexit("comptype not zero, one or two, (dynamic) was %d",comptype);
			// or
			logger("comp not 0,1 or 2 (is %d)bailing\n",comptype);
			delete[] p8;
			p8=0;
			delete[] cdsave;
			return p8;
		}
		if (comptype==1 || comptype==2) {
			while(1) {
				if (cp>=cpend+4) {
//					errorexit("inflate: eof (Where's the break code?)");
					logger("inflate: eof (Where's the break code?) aborting.. returning what we got..\n");
//					final=1;
//					break;
					delete[] p8;
					p8=0;
					delete[] cdsave;
					return p8;
				}
				unsigned int litlencode,distcode;
				unsigned int ll,ld;
				litlencode=gethuff(&cp,&bitpos,litlenhuff,MAXBITS);
		//		logger("pixcnt = %d\n",unccnt);
				if (litlencode==256) {
					if (final) {
						if (unccnt != uncompsize)
							errorexit("break code and pixcnt = %d, should be %d",unccnt,uncompsize);
//						else
//							logger("success: break code and pixcnt = %d !!\n",unccnt);
					}
					break;
				} else if (litlencode<256) {
//					logger("lit code %d\n",litlencode);
					lithist[litlencode]++;
		//			p16[unccnt]=c16tab[litlencode];
					p8[unccnt]=litlencode;
					unccnt++;
				} else if (litlencode>285) {
					errorexit("litcode(%d) > 285",litlencode);
				} else { // code 257 to 285
//					logger("lit code %d, ",litlencode);
					ll=litcodetable[litlencode-257].len;
					ll+=getbits(&cp,&bitpos,litcodetable[litlencode-257].extrabits);
//					logger("lit len %d, ",ll);
					distcode=gethuff(&cp,&bitpos,disthuff,MAXBITS);
					if (distcode>=30)
						errorexit("distcode >=30");
					ld=distcodetable[distcode].dist;
					ld+=getbits(&cp,&bitpos,distcodetable[distcode].extrabits);
//					logger("dist %d\n",ld);
					for (i=0;i<ll;i++)
		//				p16[unccnt+i]=p16[unccnt+i-ld];
						p8[unccnt+i]=p8[unccnt+i-ld];
					unccnt+=ll;
				}
				if (unccnt>uncompsize)
					errorexit("eof: %d/%d",unccnt,uncompsize);
	//			else
	//				logger("uncompressed bytes is %d !\n",unccnt);
			}
		}
		if (final)
			break;
	}
//	logger("------------------- done deflate --------------------\n");
// flip in y
//	rf=bitmap16alloc(r->x,r->y,-1);
/*	if (noluck) { // dynamic huffman
		logger("noluck, bailing\n");
		delete[] p8;
		p8=0;
	} */
	delete[] cdsave;
	return p8;
/*	bitmap8* rf=bitmap8alloc(x,y,-1);
	for (i=0;i<(unsigned)y;i++)
		memcpy(&rf->data[(y-i-1)*x],&r->data[i*x],sizeof(U8)*x);
	bitmap8free(r);
	return rf; */
}

struct bitmap8 *deflate_decompress_mask(unsigned char *compdata,int compsize,int x,int y)
{
	U8* p8=deflate_decompress(compdata,compsize,x*y);
	if (!p8)
		errorexit("couldn't decomp mask");
	bitmap8* rf=bitmap8alloc(x,y,-1);
	U32 i;
	for (i=0;i<(unsigned)y;i++)
		memcpy(&rf->data[(y-i-1)*x],&p8[i*x],sizeof(U8)*x);
	delete[] p8;
	return rf;
}

void deflate_stats()
{
//	int i;
//	logger("deflate_stats:  pixel values.............\n");
//	for (i=0;i<256;i++)
//		if (lithist[i])
//			logger("val %3d: freq %d\n",i,lithist[i]);
}
