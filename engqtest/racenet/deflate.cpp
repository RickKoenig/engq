#include <string.h>

#include <engine1.h>

#include "deflate.h"

int staticdistlens[32]={
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
};

int staticlitlens[288]={
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
static struct hufftab litlenhuff[4096],disthuff[4096];
static int lithist[256];


//static struct hufftab litlen[512];
static int huffgenptr;

struct litcode {
	unsigned int extrabits;
	unsigned int len;
};

// offset by 257 (257 to 285)
static struct litcode litcodetable[31]={
	0,  3,  // 257      
	0,  4,  // 258        
	0,  5,  // 259        
	0,  6,  // 260        
	0,  7,  // 261        
	0,  8,  // 262        
	0,  9,  // 263        
	0, 10,  // 264      

	1, 11,  // 265     
	1, 13,  // 266     
	1, 15,  // 267   
	1, 17,  // 268   

	2, 19,  // 269   
	2, 23,  // 270   
	2, 27,  // 271   
	2, 31,  // 272   

	3, 35,  // 273   
	3, 43,  // 274   
	3, 51,  // 275   
	3, 59,  // 276 

	4, 67,  // 277 
	4, 83,  // 278 
	4, 99,  // 279 
	4,115,  // 280 

	5,131,  // 281 
	5,163,  // 282 
	5,195,  // 283 
	5,227,  // 284 

	0,258,  // 285 
	0,  0,  // 286 error
	0,  0,  // 287 error
};


struct distcode {
	int extrabits;
	int dist;
};

static struct distcode distcodetable[32]={
	 0,     1,    //  0
	 0,     2,    //  1
	 0,     3,    //  2
	 0,     4,    //  3
	 1,     5,    //  4
	 1,     7,    //  5
	 2,     9,    //  6
	 2,    13,    //  7
	 3,    17,    //  8
	 3,    25,    //  9
	 4,    33,    // 10
	 4,    49,    // 11
	 5,    65,    // 12
	 5,    97,    // 13
	 6,   129,    // 14
	 6,   193,    // 15
	 7,   257,    // 16
	 7,   385,    // 17
	 8,   513,    // 18
	 8,   769,    // 19
	 9,  1025,    // 20
	 9,  1537,    // 21
	10,  2049,    // 22
	10,  3073,    // 23
	11,  4097,    // 24
	11,  6145,    // 25
	12,  8193,    // 26
	12, 12289,    // 27
	13, 16385,    // 28
	13, 24577,    // 29
	 0,     0,    // 30
	 0,     0,    // 31
};

unsigned char codexlate[19]={16,17,18,0,8,  7,9,6,10,5,  11,4,12,3,13,  2,14,1,15};

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

static char *printbits(unsigned int bits,int nbits)
{
	static char sbits[64];
	char *p=sbits;
	while(nbits--)
		*p++='0'+(1&(bits>>nbits));
	*p='\0';
	return sbits;
}

static unsigned int getbits(unsigned char **pp,unsigned int *bitpos,int nbits)
{
	unsigned int ret=0;
	int i;
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

static unsigned int peekhuffbits(unsigned char **pp,unsigned int *bitpos,int nbits)
{
	unsigned int bp=*bitpos;
	char *p=*pp;
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

static void skipbits(unsigned char **pp,unsigned int *bitpos,int nbits)
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

static void alignbits(unsigned char **pp,unsigned int *bitpos)
{
	if (*bitpos==0) // already aligned
		return;
	*bitpos=0;
	(*pp)++;
}

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
static void genhuff(struct hufftab *ht,int maxbits,int *codelens,int ncodes)
{
	int i,j,p=0;
	int nc;
	for (i=0;i<ncodes;i++)
		if (codelens[i]>maxbits)
			errorexit("maxhuffbits exceeded: maxbits %d, found %d",maxbits,codelens[i]);
	for (i=1;i<=maxbits;i++) {
		for (j=0;j<ncodes;j++) {
			if (codelens[j]==i) {
				nc=1<<(maxbits-i);
				while(nc--) {
					if (p==(1<<maxbits))
						errorexit("huff table overflow");
					ht[p].nbits=i;
					ht[p].val=j;
					p++;
				}
			}
		}
	}
	if (p!=(1<<maxbits))
		errorexit("p != maxbits");
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
unsigned int gethuff(unsigned char **pp,unsigned int *bitpos,struct hufftab *ht,int maxbits)
{
	unsigned int bits;
	bits=peekhuffbits(pp,bitpos,maxbits);
//	printbits(bits,9);
//	logger("lit value = %d, nbits %d\n",litlen[bits].val,litlen[bits].nbits);
	skipbits(pp,bitpos,ht[bits].nbits);
	return ht[bits].val;
}

struct bitmap16 *deflate_decompress(unsigned char *compdata,int compsize,int x,int y)
{
	int prod=x*y,pc=0;
	unsigned int i;
	int cnt=500;
	unsigned int bitpos=0;
	unsigned int header1,header2;
	unsigned int final,comptype;
	unsigned int nlit,ndist,ncode;
	unsigned int codecodelens[19];
	unsigned int newcodecodelens[19];
	unsigned int litdistlens[286+32];
//	unsigned int litlens[286];
	unsigned int litptr=0;
	unsigned int litcode=16; // unused code
	unsigned short *p16,c16tab[256];

//	unsigned int testbits;
	struct bitmap16 *r,*rf;
	header1=*compdata++;
	header2=*compdata++;
	if (header1!=0x78)
		errorexit("tex header1 not right was %$02X supposed to be $78",header1);
	if (header2!=0xDA)
		errorexit("tex header2 not right was %$02X supposed to be $DA",header2);
//	logger("------------------- deflate --------------------\n");
	final=getbits(&compdata,&bitpos,1);
	if (final!=1)
		errorexit("not the final block");
	comptype=getbits(&compdata,&bitpos,2);
	if (comptype==2) { // dynamic huffman
		nlit=257+getbits(&compdata,&bitpos,5);
		ndist=1+getbits(&compdata,&bitpos,5);
		ncode=4+getbits(&compdata,&bitpos,4);
		memset(codecodelens,0,sizeof(codecodelens));
		memset(litdistlens,0,sizeof(litdistlens));
//		logger("ncode = %d, nlit = %d, ndist = %d\n",ncode,nlit,ndist);
// build code len code
		for (i=0;i<ncode;i++)
			codecodelens[i]=getbits(&compdata,&bitpos,3);
//		for (i=0;i<ncode;i++)
//			logger("code lens for %2d (%2d): %d\n",i,codexlate[i],codecodelens[i]);
		for (i=0;i<19;i++)
			newcodecodelens[codexlate[i]]=codecodelens[i];
		genhuff(codehuff,7,newcodecodelens,19);
//	for (i=0;i<128;i++)
//		codehuff[i].val=codexlate[codehuff[i].val];
//	for (i=0;i<128;i++)
//		logger("%s: val %2d, nbits %d\n",printbits(i,7),codehuff[i].val,codehuff[i].nbits);
		while(litptr<nlit+ndist && cnt--) {
			unsigned int lc,litlen;
			lc=gethuff(&compdata,&bitpos,codehuff,7);
//			logger("lc is %d\n",lc);
			if (lc<=15) {
				litlen=1;
				litcode=lc;
			} else if (lc==16) {
				if (litcode==16)
					errorexit("prev used when no prev");
				litlen=3+getbits(&compdata,&bitpos,2);
			} else if (lc==17) {
				litcode=0;
				litlen=3+getbits(&compdata,&bitpos,3);
			} else if (lc==18) {
				litcode=0;
				litlen=11+getbits(&compdata,&bitpos,7);
			} else 
				errorexit("bad lit len code %d\n",lc);
//		logger("%3d: lit codelen %2d and rep %3d\n",litptr,litcode,litlen);
			for (i=0;i<litlen;i++) {
//				if (litptr==0)
//					logger("litlens\n");
//				else if (litptr==nlit)
//					logger("distlens\n");
//				if (litcode!=0)
//					if (litptr<nlit)
//						logger("litlen for %3d: is %d\n",litptr,litcode);
//					else
//						logger("distlen for %3d: is %d\n",litptr-nlit,litcode);
				litdistlens[litptr]=litcode;
				litptr++;
			}
		}
		genhuff(litlenhuff,12,litdistlens,nlit);
		genhuff(disthuff,12,litdistlens+nlit,ndist);
	} else if (comptype==1) { // static huffman
		genhuff(litlenhuff,12,staticlitlens,288);
		genhuff(disthuff,12,staticdistlens,32);
		for (i=0;i<4096;i++)
			if (litlenhuff[i].val<24)
				litlenhuff[i].val+=256;
			else if (litlenhuff[i].val<168)
				litlenhuff[i].val-=24;
			else if (litlenhuff[i].val<176)
				litlenhuff[i].val+=112;
			else
				litlenhuff[i].val-=32;
//		exit(0);
	} else
		errorexit("comptype not two, (dynamic) was %d",comptype);
/*	while(cnt--) {
		litval=getlitlen(&compdata,&bitpos);
		if (litval==256)
			break;
		if (litval>=286)
			errorexit("litval >=286");
		if (litval<256)
			logger("lit val %d\n",litval);
		else {
			int len,dist;
			int nextrabits,extrabits;
			int distbits;
			len=litcodetable[litval-257].len;
			nextrabits=litcodetable[litval-257].extrabits;
			logger("litcode table for idx %d is len %d, nextrabits %d\n",litval,len,nextrabits);
			extrabits=getbits(&compdata,&bitpos,nextrabits);
			len+=extrabits;
			logger("total len is %d\n",len);
			distbits=getbits(&compdata,&bitpos,5);
			dist=distcodetable[distbits].dist;
			nextrabits=distcodetable[distbits].extrabits;
			logger("distcode table for idx %d is dist %d, nextrabits %d\n",distbits,dist,nextrabits);
			extrabits=getbits(&compdata,&bitpos,nextrabits);
			dist+=extrabits;
			logger("total len is %d\n",dist);
		}
	}
*/	
//	logger("decoding %d pixels....\n",prod);
	r=bitmap16alloc(x,y,hiwhite);
	p16=r->plane;
	if (BPP==565 || BPP==555)
		for (i=0;i<256;i++) {
			struct rgb24 c;
			c.r=c.g=c.b=i;
			c16tab[i]=color24to16(c,BPP);
		}
	else
		errorexit("don't know which bit mode to pick 555 or 565");
	while(1) {
		unsigned int litlencode,distcode;
		unsigned int ll,ld;
		litlencode=gethuff(&compdata,&bitpos,litlenhuff,12);
//		logger("pixcnt = %d\n",pc);
		if (litlencode==256) {
			if (pc != prod)
				errorexit("break code and pixcnt = %d",pc);
//			else
//				logger("success: break code and pixcnt = %d\n",pc);
			break;
		} else if (litlencode<256) {
//			logger("lit code %d\n",litcode);
			lithist[litlencode]++;
			p16[pc]=c16tab[litlencode];
			pc++;
		} else if (litlencode>285) {
			errorexit("litcode > 285");
		} else { // code 257 to 285
			ll=litcodetable[litlencode-257].len;
			ll+=getbits(&compdata,&bitpos,litcodetable[litlencode-257].extrabits);
//			logger("lit len %d, ",ll);
			distcode=gethuff(&compdata,&bitpos,disthuff,12);
			ld=distcodetable[distcode].dist;
			ld+=getbits(&compdata,&bitpos,distcodetable[distcode].extrabits);
//			logger("dist %d\n",ld);
			for (i=0;i<ll;i++)
				p16[pc+i]=p16[pc+i-ld];
			pc+=ll;
		}
		if (pc>prod) {
			errorexit("eof: %d/%d",pc,prod);
		}
	}
//	logger("------------------- done deflate --------------------\n");
// flip in y
	rf=bitmap16alloc(r->x,r->y,-1);
	for (i=0;i<(unsigned)r->y;i++)
		memcpy(&rf->plane[(r->y-i-1)*r->x],&r->plane[i*r->x],sizeof(short)*r->x);
	bitmap16free(r);
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
