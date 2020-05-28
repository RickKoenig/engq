#if 1
///////// audio api (currently supports waveplayer and directsound /////////////
#include <stdio.h>
#include <m_eng.h>
#include "m_sagger.h"


#define FOURCC(a,b,c,d) ((a)+((b)<<8)+((c)<<16)+((d)<<24))

struct sagger {
//	S32 pflag;
//	S32 bflag;
//	S32 lflag;
	FILE *sfph;
	S32 Gindex;
	S32 Gvalprev;
	S32 Gindex2;
	S32 Gvalprev2;
	S32 bptr,eptr,cptr;
	soundhandle *sh;
};

static struct sagger *asag;

static S32 ReadLong(FILE *fp)
{
	S32 v;
	fread(&v,1,4,fp);
	return v;
}

/* Intel ADPCM step variation table */
static S32 indexTable[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static S32 stepsizeTable[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

/************************** MONO ****************************/
static void DecodeAdpcm(struct sagger *s,C8 *indata, S16 *outdata, S32 blen)
{
	S8 *inp;           /* Input buffer pointer */
	S16 *outp;         /* output buffer pointer */
	S32 sign;                   /* Current adpcm sign bit */
	S32 delta;                  /* Current adpcm output value */
	S32 step;                   /* Stepsize */
	S32 valpred;                /* Predicted value */
	S32 vpdiff;                 /* Current change to valpred */
	S32 index;                  /* Current step change index */
	S32 inputbuffer=0;            /* place to keep next 4-bit value */
	S32 bufferstep;             /* toggle between inputbuffer/input */
	S32 wlen;
	outp = outdata;
	inp = (S8 *)indata;
	valpred = s->Gvalprev;
	index = s->Gindex;
	step = stepsizeTable[index];
	bufferstep = 0;
	wlen=blen>>1;
	for ( ; wlen > 0 ; wlen-- ) {
/* Step 1 - get the delta value */
		if ( bufferstep ) {
			delta = inputbuffer & 0xf;
		} else {
			inputbuffer = *inp++;
			delta = (inputbuffer >> 4) & 0xf;
		}
		bufferstep = !bufferstep;
/* Step 2 - Find new index value (for later) */
		index += indexTable[delta];
		if ( index < 0 ) index = 0;
		if ( index > 88 ) index = 88;
/* Step 3 - Separate sign and magnitude */
		sign = delta & 8;
		delta = delta & 7;
/* Step 4 - Compute difference and new predicted value */
/*
** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
** in adpcm_coder.
*/
		vpdiff = step >> 3;
		if ( delta & 4 ) vpdiff += step;
		if ( delta & 2 ) vpdiff += step>>1;
		if ( delta & 1 ) vpdiff += step>>2;
		if ( sign )
			valpred -= vpdiff;
		else
			valpred += vpdiff;
/* Step 5 - clamp output value */
		if ( valpred > 32767 )
			valpred = 32767;
		else if ( valpred < -32768 )
			valpred = -32768;
/* Step 6 - Update step value */
		step = stepsizeTable[index];
/* Step 7 - Output value */
		*outp++ = (S16)valpred;
	}
	s->Gvalprev = valpred;
	s->Gindex = index;
}

/*************************** STEREO ***************************/
static void DecodeAdpcm2(struct sagger *s,C8 *indata, S16 *outdata, S32 blen)
{
	S8 *inp;           /* Input buffer pointer */
	S16 *outp;         /* output buffer pointer */
	S32 sign;                   /* Current adpcm sign bit */
	S32 delta;                  /* Current adpcm output value */
	S32 step,step2;                   /* Stepsize */
	S32 valpred,valpred2;                /* Predicted value */
	S32 vpdiff;                 /* Current change to valpred */
	S32 index,index2;                  /* Current step change index */
	S32 inputbuffer;            /* place to keep next 4-bit value */
//	S32 bufferstep;             /* toggle between inputbuffer/input */
	S32 wlen;
	outp = outdata;
	inp = (S8 *)indata;
	valpred = s->Gvalprev;
	index = s->Gindex;
	valpred2 = s->Gvalprev2;
	index2 = s->Gindex2;
	step = stepsizeTable[index];
	step2 = stepsizeTable[index2];
//	bufferstep = 0;
	wlen=blen>>2;
	for ( ; wlen > 0 ; wlen-- ) {
/* Step 1 - get the delta value */
//		if ( bufferstep ) {
//			delta = inputbuffer & 0xf;
//		} else {
			inputbuffer = *inp++;
			delta = (inputbuffer >> 4) & 0xf;
//		}
//		bufferstep = !bufferstep;
/* Step 2 - Find new index value (for later) */
		index += indexTable[delta];
		if ( index < 0 ) index = 0;
		if ( index > 88 ) index = 88;
/* Step 3 - Separate sign and magnitude */
		sign = delta & 8;
		delta = delta & 7;
/* Step 4 - Compute difference and new predicted value */
/*
** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
** in adpcm_coder.
*/
		vpdiff = step >> 3;
		if ( delta & 4 ) vpdiff += step;
		if ( delta & 2 ) vpdiff += step>>1;
		if ( delta & 1 ) vpdiff += step>>2;
		if ( sign )
			valpred -= vpdiff;
		else
			valpred += vpdiff;
/* Step 5 - clamp output value */
		if ( valpred > 32767 )
			valpred = 32767;
		else if ( valpred < -32768 )
			valpred = -32768;
/* Step 6 - Update step value */
		step = stepsizeTable[index];
/* Step 7 - Output value */
		*outp++ = (S16)valpred;
// doit again
/* Step 1 - get the delta value */
//		if ( bufferstep ) {
			delta = inputbuffer & 0xf;
//		} else {
//			inputbuffer = *inp++;
//			delta = (inputbuffer >> 4) & 0xf;
//		}
//		bufferstep = !bufferstep;
/* Step 2 - Find new index value (for later) */
		index2 += indexTable[delta];
		if ( index2 < 0 ) index2 = 0;
		if ( index2 > 88 ) index2 = 88;
/* Step 3 - Separate sign and magnitude */
		sign = delta & 8;
		delta = delta & 7;
/* Step 4 - Compute difference and new predicted value */
/*
** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
** in adpcm_coder.
*/
		vpdiff = step2 >> 3;
		if ( delta & 4 ) vpdiff += step2;
		if ( delta & 2 ) vpdiff += step2>>1;
		if ( delta & 1 ) vpdiff += step2>>2;
		if ( sign )
			valpred2 -= vpdiff;
		else
			valpred2 += vpdiff;
/* Step 5 - clamp output value */
		if ( valpred2 > 32767 )
			valpred2 = 32767;
		else if ( valpred2 < -32768 )
			valpred2 = -32768;
/* Step 6 - Update step value */
		step2 = stepsizeTable[index2];
/* Step 7 - Output value */
		*outp++ = (S16)valpred2;
	}
	s->Gvalprev = valpred;
	s->Gindex = index;
	s->Gvalprev2 = valpred2;
	s->Gindex2 = index2;
}
#if 1
//Read in ADPCM data decompress and store in buffer that is bsze bytes...
#define COMPSIZE 128
#define UNCOMPSIZE (COMPSIZE*4)
static void saggercallback(U8 *buff,S32 ubytes)
{
//	S32 bptr,eptr,cptr;
	C8 InData[COMPSIZE];
	S32 cbytes;
	S32 lsec,rsec,tsec;
	if (!asag)
		return;
	cbytes=ubytes>>2;
	while(cbytes>0) {
		if (cbytes<COMPSIZE)
			tsec=cbytes;
		else
			tsec=COMPSIZE;
		lsec=asag->eptr-asag->cptr; // how much data remaining in the file..
		if (lsec<=tsec) {
			fread(InData,lsec,1,asag->sfph);
			fseek(asag->sfph,asag->bptr,SEEK_SET);
			asag->cptr=asag->bptr;
			rsec=tsec-lsec;
		} else {
			lsec=0;
			rsec=tsec;
		}
		if (rsec) {
			fread(InData+lsec,rsec,1,asag->sfph);
			asag->cptr+=rsec;
		}
		DecodeAdpcm(asag,InData,(S16*)buff,tsec<<2);
		buff += (tsec<<2);
		cbytes-=tsec;
	}
}

struct sagger *play_sagger(C8 *sagname)
{
	FILE *f;
	struct sagger *s;
	long wavlen,fmtlen,datlen;
	if (asag)
		return NULL;
	f=fopen2(sagname,"rb");
	if (!f)
		return NULL;
	if ((ReadLong(f)!=FOURCC('S','L','A','G')) ||
	((wavlen=ReadLong(f))==-1L) ||
	(ReadLong(f)!=FOURCC('W','A','V','E')) ||
	(ReadLong(f)!=FOURCC('f','m','t',' ')) ||
	((fmtlen=ReadLong(f))==-1L) ||
	fseek(f,fmtlen,SEEK_CUR) ||
	(ReadLong(f)!=FOURCC('d','a','t','a')) ||
	((datlen=ReadLong(f))==-1L))	{
		fclose(f);
		return NULL;
	}
	s=(struct sagger *)memalloc(sizeof(*s));
	memset(s,0,sizeof(struct sagger));
	s->bptr=ftell(f);
	s->eptr=s->bptr+(datlen>>2);
	s->cptr=s->bptr;
	s->sfph=f;
	setfifosec(15,1); // 10 second buffers, defaults to  5..
	asag=s;
	s->sh=fifo_play(22050,0,1,saggercallback);
	return s;
}

void free_sagger(struct sagger *sag)
{
	if (!sag)
		return;
	sound_free(sag->sh);
	fclose(sag->sfph);
	memfree(sag);
	asag=NULL;
}
#endif
/////////// new vag reader ////////////////
//		sbuff=vagload(&sbufflen,&freq,&sixteenbit,&stereo);
/*
U8 *vagload(C8 *name,S32 *sbufflen,S32 *freq,S32 *sixteenbit,S32 *stereo)
{
#define SBUFFSIZE 32768
	S32 i,j;
//	U8 *ret;
	S16 *ret;
	i=5;
//	ret=memalloc(SBUFFSIZE*sizeof(U8));
	ret=memalloc(SBUFFSIZE*sizeof(S16));
	for (j=0;j<SBUFFSIZE;j++)
//		ret[j]=(U8)(128+126.0f*sin(2*PI*i*j*100/SBUFFSIZE));
		ret[j]=(S16)(32766.0f*sin(2*PI*i*j*100/SBUFFSIZE));
	*sbufflen=SBUFFSIZE;
	*freq=11025;
	*sixteenbit=1;
	*stereo=0;
	return (U8 *)ret;
}
*/

U8 *vagload(const C8 *vagname,S32 *sbufflen,S32 *freq,S32 *sixteenbit,S32 *stereo)
{
//	S32 i;
	FILE *f;
//	S32 checksum1=0,checksum2=0;
	S32 chunkid,datalen,samprate;
	S16 twobytes;
	U8 *indata;
	S16 *outdata;
	struct sagger *s;
	if (!fileexist(vagname))
		return 0;
	f=fopen2(vagname,"rb");
	if (!f)
		return NULL;
	chunkid=ReadLong(f);
	datalen=ReadLong(f);
	samprate=ReadLong(f);
	fread(&twobytes,1,sizeof(twobytes),f);
	if (chunkid==FOURCC('K','V','A','G') && datalen>0) {
		if (twobytes==0) {
			s=new sagger;//(struct sagger *)memzalloc(sizeof(*s));
			memset(s,0,sizeof(sagger));
			
			*sbufflen=datalen<<1;
//			indata=(U8 *)memalloc(datalen);
			indata=new U8[datalen];
//			outdata=(S16 *)memalloc(*sbufflen*sizeof(S16));
			outdata=new S16[*sbufflen];
			fread(indata,1,datalen,f);
//			for (i=0;i<datalen;i++)
//				checksum1+=indata[i];
			
			DecodeAdpcm(s,(C8 *)indata,outdata,datalen<<2);
//			for (i=0;i<*sbufflen;i++)
//				checksum2+=outdata[i];
			logger("vagload '%20s' mono datalen %5d, samprate %5d\n", //, checksum1 %d, checksum2 %d\n",
				vagname,datalen,samprate); //,checksum1,checksum2);

			*freq=samprate;
			*sixteenbit=1;
			*stereo=0;
			
//			memfree(indata);
			delete[] indata;
//			memfree(s);
			delete s;
			fclose(f);
			return (U8 *)outdata;
		} else if (twobytes==1) {
//			fclose(f);
//			return NULL;
//			s=(struct sagger *)memzalloc(sizeof(*s));
			s=new sagger;
			memset(s,0,sizeof(sagger));
			
			*sbufflen=datalen;
//			indata=(U8 *)memalloc(datalen);
			indata=new U8[datalen];
//			outdata=(S16 *)memalloc(*sbufflen*2*sizeof(S16));
			outdata=new S16[*sbufflen*2];
			fread(indata,1,datalen,f);
//			for (i=0;i<datalen;i++)
//				checksum1+=indata[i];
			
			DecodeAdpcm2(s,(C8 *)indata,outdata,datalen<<2);
//			for (i=0;i<*sbufflen;i++)
//				checksum2+=outdata[i];
			logger("vagload '%20s' stereo datalen %5d, samprate %5d\n", //, checksum1 %d, checksum2 %d\n",
				vagname,datalen,samprate); //,checksum1,checksum2);

			*freq=samprate;
			*sixteenbit=1;
			*stereo=1;
			
//			memfree(indata);
			delete[] indata;
//			memfree(s);
			delete s;
			fclose(f);
			return (U8 *)outdata;
		}
	}
	fclose(f);
	return NULL;
}

struct wavehandle *vag_load(const C8* vagname)
{
	S32 sbufflen,freq,sixteenbit,stereo;
	U8 *sbuff=vagload(vagname,&sbufflen,&freq,&sixteenbit,&stereo);
	if (!sbuff)
		return 0;
	wavehandle* wh=wave_build((C8*)sbuff,sbufflen,freq,sixteenbit,stereo);
//	memfree(sbuff);
	delete[] sbuff;
	return wh;
}

#if 0
struct tag *vagread(C8 **names,S32 num)
{
	U8 *sbuff;
	S32 sbufflen,freq,sixteenbit,stereo;
	struct tag *t;
	S32 i;//,j;
	t=(struct tag *)memalloc(sizeof(struct tag));
	t->th=(struct tagheader *)memalloc(sizeof(struct tagheader)*num);
	t->ntag=num;
	for (i=0;i<num;i++) {
		mystrncpy(t->th[i].name,names[i],16);
		sbuff=vagload(names[i],&sbufflen,&freq,&sixteenbit,&stereo);
		if (sbuff) {
			t->th[i].wh=wave_build(sbuff,sbufflen,freq,sixteenbit,stereo);
			memfree(sbuff);
		} else
			t->th[i].wh=NULL;
//		logger("vag '%s' size is %d\n",names[i],filelen(names[i]));
	}
	return t;
}
#endif
#endif
