///////// audio api (currently supports waveplayer and directsound /////////////
#include <m_eng.h>
#define DIRECTSOUND_VERSION 0x0700
#include <dsound.h>
#include "m_perf.h"

HWAVEIN tmic_wh;
WAVEHDR tmic_whdrs[2];

ai audioinfo;

struct {
	S32 nsamples;
	S32 freq;
	S32 sixteenbit;
	S32 stereo;
} buildinfo;
 
static void (*mfifocallback)(C8*,S32);

static S32 nstreamsec=5; // how many seconds is the streaming soundbuffer

static S32 nfifosecn=5;
static S32 nfifosecd=25;

static S32 nmicsecn=4;
static S32 nmicsecd=15;

void setstreamsec(S32 sec)
{
	nstreamsec=sec;
}

void setfifosec(S32 secn,S32 secd)
{
	nfifosecn=secn;
	nfifosecd=secd;
}

void setmicsec(S32 secn,S32 secd)
{
	if (tmic_wh)
		return;
	nmicsecn=secn;
	nmicsecd=secd;
}

struct soundhandle soundhandles[MAXSOUNDS];
static S32 maxdvsounds; // # of wave to play at the same time
//S32 disablewpsetvol;	// disable volume setting for waveplayer if you want
static S32 nsubdrivers;//,nmidisubdrivers;	
//static S32 audio_subdriver;//,midi_subdriver;
//S32 audio_maindriver=-1;
//static S32 audio_micsubdriver;
//static FILE* fpstream;
//static S32 streamdataoffset;
//static S32 streamcurrent;
//static S32 streamend;
//static struct soundhandle* streamplaying;
//static S32 streambufferhalf;
//static S32 streamstartwindow[2];
//static S32 streamendwindow[2];
//static S32 streamcurwindow;
//static S32 streamsilence;
//static S32 streamstopped;
//S32 streamaudiopos;
// load .wav file and fill in a wavehandle structure and return pointer
// strm can be 0, normal
// strm can be 1, stream
// strm can be 2, U8* fn is actually an open FILE*  (good for .tag's)
static struct wavehandle* buildwavehandle(
	const C8* data,S32 samplelen,S32 freq,S32 sixteenbit,S32 stereo)
{
	S32 bits,chan;
	struct wavehandle* r;
	if (sixteenbit)
		bits=16;
	else
		bits=8;
	chan=stereo+1;
//	r=(struct wavehandle*)memzalloc(sizeof(struct wavehandle));
	r=new wavehandle;
	memset(r,0,sizeof(wavehandle));
	r->format_info.wFormatTag=WAVE_FORMAT_PCM;
	r->format_info.nChannels=chan;
	r->format_info.nSamplesPerSec=freq;
	r->format_info.nBlockAlign=(chan*bits)>>3;
	r->format_info.nAvgBytesPerSec=freq*r->format_info.nBlockAlign;
	r->format_info.wBitsPerSample=bits;
	r->format_info.cbSize=0;//sizeof(struct mywaveformatex);

 	r->whdrw.dwBufferLength=samplelen*r->format_info.nBlockAlign;
//	r->whdrw.lpData=(U8*)memalloc(r->whdrw.dwBufferLength);
	r->whdrw.lpData=new U8[r->whdrw.dwBufferLength];
	memcpy(r->whdrw.lpData,data,r->whdrw.dwBufferLength);

//	r->name=strclone("built");
//	r->name="built";
	return r;
}

#if 1
#include <vorbis/codec.h>            // from the vorbis sdk
#include <vorbis/vorbisfile.h>       // also :)
#define BUFSIZE 1048576*60         // buffer length
static struct wavehandle* loadogghandle(const C8* fn)
{
	HRESULT hr;
    FILE    *f;
//	char nameext[256];
	OggVorbis_File vf;
	vorbis_info *vi;
	struct wavehandle* ret;
//    DSBUFFERDESC desc;
//	LPDIRECTSOUNDBUFFER  pTemp;           // the temp buffer
	DWORD   size = BUFSIZE;
    DWORD   pos = 0;
    int     sec = 0;
    int     ret2 = 1;
    char    *buf;
//    char    *buf2;


    if (!fileexist(fn))
		return 0;
	f = fopen2(fn, "rb");
    if (!f) return NULL;

    hr = ov_open(f, &vf, NULL, 0);

    // ok now the tricky part

    vi = ov_info(&vf,-1);

	ret = new wavehandle;
	memset(ret,0,sizeof(*ret));

    // set the wave format
    
    memset(&ret->format_info, 0, sizeof(ret->format_info));

    ret->format_info.cbSize          = sizeof(ret->format_info);
    ret->format_info.nChannels       = vi->channels;
    ret->format_info.wBitsPerSample  = 16;                    // ogg vorbis is always 16 bit
    ret->format_info.nSamplesPerSec  = vi->rate;
    ret->format_info.nAvgBytesPerSec = ret->format_info.nSamplesPerSec*ret->format_info.nChannels*2;
    ret->format_info.nBlockAlign     = 2*ret->format_info.nChannels;
    ret->format_info.wFormatTag      = 1;

    // set up the buffer
	buf = new C8[BUFSIZE];

    // fill the buffer

//    hr=IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)pTemp, 0, size, (LPVOID*)&buf, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    
    // now read in the bits
    while(ret2 && pos<size)
    {
        ret2 = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
        pos += ret2;
    }
	ret->whdrw.lpData=new U8[pos];
	memcpy(ret->whdrw.lpData,buf,pos);
	delete[] buf;
	ret->whdrw.dwBufferLength=pos;

//	IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)pTemp, buf, size, 0, 0 );

	//CREATE BUFFER TO MATCH ACTUAL SIZE OF FILE
//    desc.dwBufferBytes  = pos;

/*	hr=IDirectSound_CreateSoundBuffer(lpDirectSound, &desc,(LPDIRECTSOUNDBUFFER*)&ret->whandle,0);

	hr=IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ret->whandle, 0, pos, (LPVOID*)&buf2, &pos, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    CopyMemory(buf2,buf,pos);
	IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ret->whandle, buf2, size, 0, 0 );

	IDirectSoundBuffer_Release(pTemp);
*/
//	getnameext(nameext,fn);
//	ret->name=strclone(nameext);

	
	ov_clear(&vf);
	fclose(f);

//	buf=NULL;

    return ret;
}
#endif

#if 0
static struct wavehandle* loadogghandleo(const C8* fn)
{
	FILE* fp;
//	C8 nameext[256];
	static C8 any_id[5];
//	static S32 ids;
	U32 any_len;
	U32 any_skip,data_skip=0;	// for moving around in file..
	struct wavehandle* ret;
	struct wavehandle rl;
	if (buildinfo.freq)
		return buildwavehandle(
			fn,buildinfo.nsamples,buildinfo.freq,buildinfo.sixteenbit,buildinfo.stereo);
	memset(&rl,0,sizeof(rl));
//	rl.id=ids++;
//	C8 ext[50];
	fp=fopen2(fn,"rb");
	if (!fp)
// .wav doesn't exist
		return 0;
	fread(&any_id,1,4,fp);
	if (strcmp(any_id,"RIFF")) {
		fclose(fp);
// .wav file not a 'RIFF'
		return 0;
	}
	
	fread(&any_len,1,4,fp);
	fread(any_id,1,4,fp);

	if (strcmp(any_id,"WAVE")) {
		fclose(fp);
// 'RIFF' not a 'WAVE
		return 0;
	}
	while(fread(any_id,1,4,fp)==4) {
		if (any_id[0]<' ' || any_id[0]>'z' ||		// bail if non ascii
			any_id[1]<' ' || any_id[1]>'z' ||
			any_id[2]<' ' || any_id[2]>'z' ||
			any_id[3]<' ' || any_id[3]>'z') {
			break;
		}
		fread(&any_len,1,4,fp);
//		fseek(fp,0,FILE_CURRENT);
		any_skip=ftell(fp);
		if (!strcmp(any_id,"fmt ")) {
			fread(&rl.format_info,1,sizeof(PCMWAVEFORMAT),fp);
		} else if (!strcmp(any_id,"data")) {
			rl.whdrw.dwBufferLength=any_len;
//			fseek(fp,0,FILE_CURRENT);
			data_skip=ftell(fp);
		} else if (!strcmp(any_id,"LIST")) {
			break;
		}
		fseek(fp,any_skip+((any_len+1)&~1),SEEK_SET);	// align by 2
	}
	if (rl.whdrw.dwBufferLength) {
		fseek(fp,data_skip,SEEK_SET);
		rl.whdrw.lpData=new U8[rl.whdrw.dwBufferLength];
		fread(rl.whdrw.lpData,1,rl.whdrw.dwBufferLength,fp);
	}
	fclose(fp);
	if (!rl.whdrw.dwBufferLength) {
// wave has no data
		return 0;
	}
	if (rl.format_info.wFormatTag!=1) {
		delete[] rl.whdrw.lpData;
// wave not a pcm
		return 0;
	}
    if (rl.format_info.nBlockAlign!=
		rl.format_info.wBitsPerSample*rl.format_info.nChannels/8) {
		delete[] rl.whdrw.lpData;
// check formula 1
		return 0;
	}
	if (rl.format_info.nSamplesPerSec*rl.format_info.nBlockAlign!=
		rl.format_info.nAvgBytesPerSec) {
		delete[] rl.whdrw.lpData;
// check formula 2
		return 0;
	}
//	ret=(struct wavehandle*)memalloc(sizeof (struct wavehandle));
	ret=new wavehandle;
	memcpy(ret,&rl,sizeof(struct wavehandle));
//	if (strm&WOPT_FILEPTR) {
//		ret->name=strclone("----");
//		ret->name="----";
//	} else {
//		mgetnameext(fn,nameext);
//		ret->name=strclone(nameext);
//		ret->name=nameext;
//	}
	return ret;
}
#endif

static struct wavehandle* loadwavehandle(const C8* fn,S32 strm)
{
	FILE* fp;
//	C8 nameext[256];
	static C8 any_id[5];
//	static S32 ids;
	U32 any_len;
	U32 any_skip,data_skip=0;	// for moving around in file..
	struct wavehandle* ret;
	struct wavehandle rl;
	if (buildinfo.freq)
		return buildwavehandle(
			fn,buildinfo.nsamples,buildinfo.freq,buildinfo.sixteenbit,buildinfo.stereo);
	memset(&rl,0,sizeof(rl));
//	rl.id=ids++;
//	C8 ext[50];
	if (isfileext(fn,"ogv") || isfileext(fn,"ogg")) {
		if (strm)
			return 0;
		return loadogghandle(fn);
	}
	if (strm&WOPT_FILEPTR)
		fp=(FILE*)fn;
	else {
		if (!fileexist(fn))
			return 0;
	}
	fp=fopen2(fn,"rb");
	if (!fp)
// .wav doesn't exist
		return 0;
	fread(&any_id,1,4,fp);
	if (strcmp(any_id,"RIFF")) {
//		wav_info.riff_len=0;
		if (!(strm&WOPT_FILEPTR))
			fclose(fp);
// .wav file not a 'RIFF'
		return 0;
	}
	
	fread(&any_len,1,4,fp);
	fread(any_id,1,4,fp);

	if (strcmp(any_id,"WAVE")) {
		if (!(strm&WOPT_FILEPTR))
			fclose(fp);
// 'RIFF' not a 'WAVE
		return 0;
	}
	while(fread(any_id,1,4,fp)==4) {
		if (any_id[0]<' ' || any_id[0]>'z' ||		// bail if non ascii
			any_id[1]<' ' || any_id[1]>'z' ||
			any_id[2]<' ' || any_id[2]>'z' ||
			any_id[3]<' ' || any_id[3]>'z') {
			break;
		}
		fread(&any_len,1,4,fp);
//		fseek(fp,0,FILE_CURRENT);
		any_skip=ftell(fp);
		if (!strcmp(any_id,"fmt ")) {
			fread(&rl.format_info,1,sizeof(PCMWAVEFORMAT),fp);
			logger("sizeof PCMWAVEFORMAT is %d\n",sizeof(PCMWAVEFORMAT));
		} else if (!strcmp(any_id,"data")) {
			rl.whdrw.dwBufferLength=any_len;
//			fseek(fp,0,FILE_CURRENT);
			data_skip=ftell(fp);
		} else if (!strcmp(any_id,"LIST")) {
			break;
		}
		fseek(fp,any_skip+((any_len+1)&~1),SEEK_SET);	// align by 2
	}
	if (rl.whdrw.dwBufferLength) {
		fseek(fp,data_skip,SEEK_SET);
		if (strm==1) {
			rl.streamdataoffset=ftell(fp);
			rl.streamend=rl.whdrw.dwBufferLength;
		} else {
//			rl.whdrw.lpData=(U8*)memalloc(rl.whdrw.dwBufferLength);
			rl.whdrw.lpData=new U8[rl.whdrw.dwBufferLength];
			fread(rl.whdrw.lpData,1,rl.whdrw.dwBufferLength,fp);
		}
	}
	if (strm==0)
		fclose(fp);
	if (!rl.whdrw.dwBufferLength) {
// wave has no data
		if (strm==WOPT_STREAM)
			fclose(fp);
		return 0;
	}
	if (rl.format_info.wFormatTag!=1) {
		if (strm!=WOPT_STREAM)
//			memfree(rl.whdrw.lpData);
			delete[] rl.whdrw.lpData;
// wave not a pcm
		else
			fclose(fp);
		return 0;
	}
    if (rl.format_info.nBlockAlign!=
		rl.format_info.wBitsPerSample*rl.format_info.nChannels/8) {
		if (strm!=WOPT_STREAM)
//			memfree(rl.whdrw.lpData);
			delete[] rl.whdrw.lpData;
// check formula 1
		else
			fclose(fp);
		return 0;
	}
	if (rl.format_info.nSamplesPerSec*rl.format_info.nBlockAlign!=
		rl.format_info.nAvgBytesPerSec) {
		if (strm!=WOPT_STREAM)
//			memfree(rl.whdrw.lpData);
			delete[] rl.whdrw.lpData;
// check formula 2
		else
			fclose(fp);
		return 0;
	}
//	ret=(struct wavehandle*)memalloc(sizeof (struct wavehandle));
	ret=new wavehandle;
	memcpy(ret,&rl,sizeof(struct wavehandle));
//	if (strm&WOPT_FILEPTR) {
//		ret->name=strclone("----");
//		ret->name="----";
//	} else {
//		mgetnameext(fn,nameext);
//		ret->name=strclone(nameext);
//		ret->name=nameext;
//	}
	if (strm&WOPT_STREAM)
		ret->fpstream=fp;
	return ret;
}

/////////////////////////////////////////////////////////////////
////////////////////////// directsound waveform audio... 
/////////////////////////////////////////////////////////////////

static LPDIRECTSOUND lpDirectSound;
static DSBUFFERDESC dsbufferdesc,dsprimarydesc;
static GUID driverguid,*lpdriverguid,lastguid,*lplastguid;
static WAVEFORMATEX wfx;
static LPDIRECTSOUNDBUFFER primary;
static S32 dspaused;
static BOOL CALLBACK directsoundenumcallback(LPGUID lpGuid,LPCSTR lpcstrDescription,PCSTR lpcstrModule,LPVOID lpContext)
{
	if (audioinfo.audio_subdriver==nsubdrivers) {
		if (lpGuid) {
			driverguid=*lpGuid;
			lpdriverguid=&driverguid;
		}
		logger("  * ");
	} else
		logger("    ");
	if (lpGuid) {
		lastguid=*lpGuid;
		lplastguid=&driverguid;
	}
	logger("subdriver %2d '%s'\n",nsubdrivers,lpcstrDescription);
	nsubdrivers++;
	return TRUE;
}

static S32 ds_audio_init()
{
	HRESULT hr;
	DSCAPS dscaps;  
	logger(" directsound init:\n");
	nsubdrivers=0;
	lpdriverguid=lplastguid=0;
	dspaused=0;
	hr=DirectSoundEnumerate(directsoundenumcallback,0);
	if (hr!=DS_OK)
		errorexit("DirectSoundEnumerate %08x",hr);
	if (nsubdrivers<=0)
		return 0;
	if (audioinfo.audio_subdriver>=nsubdrivers) {
		audioinfo.audio_subdriver=nsubdrivers-1;
		driverguid=lastguid;
		lpdriverguid=lplastguid;
		if (!nsubdrivers)
			errorexit("directsound: 0 lpdriverguid");
	}
//		errorexit("directsound: pick a lower audiosubdriver");
	hr = DirectSoundCreate(lpdriverguid, &lpDirectSound, 0);
	if (hr!=DS_OK) return 0;
		//errorexit("DirectSoundCreate %08x",hr);
//	hr = IDirectSound_SetCooperativeLevel(lpDirectSound,wininfo.MainWindow,DSSCL_NORMAL);
	hr = lpDirectSound->SetCooperativeLevel((HWND)wininfo.MainWindow,DSSCL_PRIORITY);
//	hr = IDirectSound_SetCooperativeLevel(lpDirectSound,wininfo.MainWindow,DSSCL_PRIORITY);
//	hr = IDirectSound_SetCooperativeLevel(lpDirectSound,wininfo.MainWindow,DSSCL_EXCLUSIVE);
	if (hr!=DS_OK)
		errorexit("IDirectSound_SetCooperativeLevel %08x",hr);
	dscaps.dwSize = sizeof(DSCAPS); 
	hr=lpDirectSound->GetCaps(&dscaps);
//	hr=IDirectSound_GetCaps(lpDirectSound,&dscaps);
	if (hr!=DS_OK)
		errorexit("IDirectSound_GetCaps %08x",hr);
// set the format of the primary soundbuffer
	memset(&dsprimarydesc,0,sizeof(dsprimarydesc));
    dsprimarydesc.dwSize = sizeof(DSBUFFERDESC);
    dsprimarydesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_STICKYFOCUS;
    hr=lpDirectSound->CreateSoundBuffer(&dsprimarydesc,&primary,0);
//    hr=IDirectSound_CreateSoundBuffer(lpDirectSound,&dsprimarydesc,&primary,0);
	if (hr!=DS_OK) return 0;
//		errorexit("IDirectSound_createprimarybuffer %08x",hr);
	memset(&wfx,0,sizeof(wfx)); 
//    wfx.wFormatTag = 1; 
//    wfx.nChannels = 1; 
//    wfx.nSamplesPerSec = 11025; 
//    wfx.wBitsPerSample = 8; 
    wfx.wFormatTag = 1; 
    wfx.nChannels = 2; 
    wfx.nSamplesPerSec = 44100; 
    wfx.wBitsPerSample = 16; 
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    hr = primary->SetFormat(&wfx); 
//    hr = IDirectSoundBuffer_SetFormat(primary,&wfx); 
	if (hr!=DS_OK) return 0;
//		errorexit("IDirectSound_setformatprimarybuffer %08x",hr);
//	return 0; // test, lack of directsound
	return 1;
}

static struct wavehandle* ds_wave_load(const C8* fn,S32 strm)
{
	HRESULT hr;
	U8* buff_ptr;
	U32 buff_bytes;
	struct wavehandle* ret;
// load wave file into memory, filling out the wavehandle structure
//	if (fpstream)
//		strm=0;
	ret=loadwavehandle(fn,strm);
//	if (strm==2)
//		strm=0;
	if (ret==0)
		return 0;
//	ret->stream=strm;
// get a buffer with format and length set
	dsbufferdesc.dwSize=sizeof(DSBUFFERDESC);
    dsbufferdesc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY|
		DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_STICKYFOCUS; 
    if (ret->fpstream) {
		dsbufferdesc.dwBufferBytes=ret->format_info.nAvgBytesPerSec*nstreamsec; 
		dsbufferdesc.dwBufferBytes+=15;
		dsbufferdesc.dwBufferBytes&=~15; // round up to a multiple of 16 bytes..
		ret->streambufferhalf=dsbufferdesc.dwBufferBytes/2;
		ret->streamstartwindow[0]=0;
		ret->streamendwindow[0]=ret->streambufferhalf/2;
		ret->streamstartwindow[1]=ret->streambufferhalf;
		ret->streamendwindow[1]=ret->streambufferhalf*3/2;
	} else
		dsbufferdesc.dwBufferBytes=ret->whdrw.dwBufferLength; 
	dsbufferdesc.lpwfxFormat=(WAVEFORMATEX*)&ret->format_info;
	hr=lpDirectSound->CreateSoundBuffer(
		&dsbufferdesc,(LPDIRECTSOUNDBUFFER*)&ret->whandle,0);
//	hr=IDirectSound_CreateSoundBuffer(lpDirectSound,
//		&dsbufferdesc,(LPDIRECTSOUNDBUFFER*)&ret->whandle,0);
	if (hr!=DS_OK)
		errorexit("IDirectSound_CreateSoundBuffer %08x",hr);
// get a pointer to the memory
	if (!ret->fpstream) {
		hr=((LPDIRECTSOUNDBUFFER)ret->whandle)->Lock(
		(DWORD)0,ret->whdrw.dwBufferLength,(void**)&buff_ptr,(DWORD*)&buff_bytes,0,(DWORD*)0,DSBLOCK_ENTIREBUFFER);
//		hr=IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ret->whandle,
//		(DWORD)0,ret->whdrw.dwBufferLength,(void**)&buff_ptr,(DWORD*)&buff_bytes,0,(DWORD*)0,DSBLOCK_ENTIREBUFFER);
		if (hr!=DS_OK)
			errorexit("IDirectSoundBuffer_Lock %08x",hr);
// make sure ret # of bytes from lock same as in WAVEHDR
		if (buff_bytes!=ret->whdrw.dwBufferLength)
			errorexit("buff_bytes!=ret->whdr.dwBufferLength");
// copy memory in the secondary sound buffer and free it from wavememory
		memcpy(buff_ptr,ret->whdrw.lpData,buff_bytes);
//		memfree(ret->whdrw.lpData);
		delete[] ret->whdrw.lpData;
		ret->whdrw.lpData=0;
// remember mempointer, DON'T FREE IT!!, 'release' it, probably not important..
//	ret->whdr.lpData=buff_ptr;
// remove lock so wave can play
		((LPDIRECTSOUNDBUFFER)ret->whandle)->Unlock(buff_ptr,buff_bytes,0,0);
//		IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ret->whandle,buff_ptr,buff_bytes,0,0);
		if (hr!=DS_OK)
			errorexit("IDirectSoundBuffer_UnLock %08x",hr);
	}
	return ret;
}

static struct soundhandle* ds_getfreesoundslot()
{
	S32 i;
	for (i=0;i<maxdvsounds;i++)
		if (soundhandles[i].wh==0 && soundhandles[i].fifosize==0)
			return &soundhandles[i];
	return 0;
}

static S32 vol2db(float vol)
{
	S32 ivol;
	float flogvol;
	if (vol==0)
		return -10000;
	FLOG2(vol,flogvol);
	ivol=(S32)(1000.0f*flogvol);
	return range(-10000,ivol,0);
}

static S32 pan2db(float pan)
{
	if (pan>=0)
		return -vol2db(1-pan);
	return vol2db(1+pan);
}

static void ds_setsfxvolume(struct soundhandle* sh,float vol)
{ // 0-1
	HRESULT hr;
	if (sh && (sh->wh || sh->fifosize)) {
		hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->SetVolume(vol2db(vol));
//		hr=IDirectSoundBuffer_SetVolume((LPDIRECTSOUNDBUFFER)sh->shandle,vol2db(vol));
		if (hr!=DS_OK)
			errorexit("IDirectSoundBuffer_SetVolume %08x",hr);
		sh->volume=vol;
	}
}

static float ds_getsfxvolume(struct soundhandle* sh)
{
	if (sh && (sh->wh || sh->fifosize))
		return sh->volume;
	return 0;
}

static void ds_streamsound(struct soundhandle* sh)
{
	HRESULT hr;
	U8* buff_ptr;
	U32 buff_bytes;
	U32 rt,lt;
	sh->wh->streamcurwindow^=1;
	if (sh->wh->streamsilence) { // done playing the silence
/*		if (sh->loopcnt!=1) {
			sh->wh->streamcurrent=0;
			fseek(sh->wh->fpstream,sh->wh->streamdataoffset,FILE_START);
			sh->wh->streamsilence=0;
			if (sh->loopcnt!=0)
				sh->loopcnt--;
		} else */
//		{
		hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Stop();
//		hr=IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)sh->shandle);
		sh->wh->streamplaying=0;
		if (hr!=DS_OK)
			errorexit("streamIDirectSoundBuffer_Stop %08x",hr);
		return;
//		}
	}
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Lock(
		sh->wh->streamstartwindow[sh->wh->streamcurwindow],sh->wh->streambufferhalf,
		(void**)&buff_ptr,(DWORD*)&buff_bytes,0,0,0);
//	hr=IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)sh->shandle,
//		sh->wh->streamstartwindow[sh->wh->streamcurwindow],sh->wh->streambufferhalf,
//		(void**)&buff_ptr,(DWORD*)&buff_bytes,0,0,0);
	if (hr!=DS_OK)
		errorexit("streamIDirectSoundBuffer_Lock %08x",hr);
//	if (sh->wh->streamcurrent>=sh->wh->streamend) {
//		if (sh->wh->format_info.wBitsPerSample==8)
//			memset(buff_ptr,128,buff_bytes);
//		else
//			memset(buff_ptr,0,buff_bytes);
//		sh->wh->streamsilence=1;
//	} else 
	if (sh->wh->streamcurrent+sh->wh->streambufferhalf>=sh->wh->streamend) {
		lt=sh->wh->streamend-sh->wh->streamcurrent;
		rt=buff_bytes-lt;
		if (sh->loopcnt==1)
			sh->wh->streamsilence=1;
		else if (sh->loopcnt!=0)
			sh->loopcnt--;
		if (lt)
			fread(buff_ptr,1,lt,sh->wh->fpstream);	
		if (sh->wh->streamsilence) {
			if (rt) {
				if (sh->wh->format_info.wBitsPerSample==8)
					memset(buff_ptr+lt,128,rt);
				else
					memset(buff_ptr+lt,0,rt);
			}
		} else {
			fseek(sh->wh->fpstream,sh->wh->streamdataoffset,SEEK_SET);
			fread(buff_ptr+lt,1,rt,sh->wh->fpstream);	
			sh->wh->streamcurrent=rt;
		}
	} else {
		fread(buff_ptr,1,buff_bytes,sh->wh->fpstream);
		sh->wh->streamcurrent+=buff_bytes;
	}
	((LPDIRECTSOUNDBUFFER)sh->shandle)->Unlock(
		buff_ptr,buff_bytes,0,0);
//	IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)sh->shandle,
//		buff_ptr,buff_bytes,0,0);
	if (hr!=DS_OK)
		errorexit("streamIDirectSoundBuffer_UnLock %08x",hr);
}

static struct soundhandle* ds_wave_playvol(struct wavehandle* wh,S32 loopcount,float vol)
{
	HRESULT hr;
	S32 flags;
	struct soundhandle* sh;
	if (wh==0)
		return 0;
	if (wh->fpstream && wh->streamplaying)
		return 0;
	sh=ds_getfreesoundslot();
	if (sh==0)
		return 0;
// clone wave
	hr=lpDirectSound->DuplicateSoundBuffer(
		(LPDIRECTSOUNDBUFFER)wh->whandle,
		(LPLPDIRECTSOUNDBUFFER)&sh->shandle);
//	hr=IDirectSound_DuplicateSoundBuffer(lpDirectSound,
//		(LPDIRECTSOUNDBUFFER)wh->whandle,
//		(LPLPDIRECTSOUNDBUFFER)&sh->shandle);
	if (hr!=DS_OK)
		errorexit("IDirectSound_DuplicateSoundBuffer %08x",hr);
// play 1 wave
	if (loopcount!=1 || wh->fpstream)
		flags=DSBPLAY_LOOPING;
	else
		flags=0;
	sh->wh=wh;
	sh->pan=0;
	ds_setsfxvolume(sh,vol);
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->GetFrequency((LPDWORD)&sh->basefreq);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_GetFrequency %08x",hr);
	if (wh->fpstream) {
		wh->streamplaying=1;
//		wh->streamstopped=0;
		fseek(wh->fpstream,wh->streamdataoffset,SEEK_SET);
		sh->wh->streamcurrent=0;
//		sh->stream=1;
		sh->wh->streamcurwindow=1;
		sh->wh->streamsilence=0;
		ds_streamsound(sh);
		logger("streamcurrent %d, stream file offset start %d, streamend %d\n",
			sh->wh->streamcurrent,sh->wh->streamdataoffset,sh->wh->streamend);
		sh->loopcnt=loopcount;
//		if (loopcount!=1)
//			sh->stream++;
	}
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Play(0,0,flags);
//	hr=IDirectSoundBuffer_Play((LPDIRECTSOUNDBUFFER)sh->shandle,0,0,flags);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_Play %08x",hr);
	sh->freq=1;
//	sh->flags=flags;
	return sh;
}

static void ds_sound_free(struct soundhandle* sh)
{
	HRESULT hr;
	struct wavehandle* wh;
	if (sh==0)
		return;
	wh=sh->wh;
	if (sh->fifosize==0 && wh==0)
		return;
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Stop();
//	hr=IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)sh->shandle);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_Stop %08x",hr);
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->SetCurrentPosition(0);
//	hr=IDirectSoundBuffer_SetCurrentPosition((LPDIRECTSOUNDBUFFER)sh->shandle,0);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_SetCurrentPosition %08x",hr);
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Release();
//	hr=IDirectSoundBuffer_Release((LPDIRECTSOUNDBUFFER)sh->shandle);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_Release %08x",hr);
	if (sh->wh)
		sh->wh->streamplaying=0;
	sh->fifosize=0;
	sh->isplayasound=0;
	sh->wh=0;
//	if (sh->stream) {
//		sh->stream=0;
//		streamplaying=0;
//		streamstopped=0;
//	}
}

static void ds_wave_unload(struct wavehandle* wh)
{
	HRESULT hr;
	S32 i;
	if (wh==0)
		return;
	for (i=0;i<maxdvsounds;i++)
		if (soundhandles[i].wh==wh)
			ds_sound_free(&soundhandles[i]);
	if (wh->fpstream) {
//		if (fpstream)
			fclose(wh->fpstream);
//		fpstream=0;
	}
	if (wh->whandle) {
		hr=((LPDIRECTSOUNDBUFFER)wh->whandle)->Release();
//		hr=IDirectSoundBuffer_Release((LPDIRECTSOUNDBUFFER)wh->whandle);
		if (hr!=DS_OK)
			errorexit("IDirectSoundBuffer_Release %08x",hr);
	}
//	memfree(wh->name);
//	delete[] wh->name;
//	memfree(wh);
	delete wh;
}

static S32 ds_sound_status(struct soundhandle* sh)
{
	HRESULT hr;
	U32 status;
	struct wavehandle* wh;
	if (sh==0)
		return 0;
	wh=sh->wh;
	if (wh==0 && sh->fifosize==0)
		return 0;
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->GetStatus((DWORD*)&status);
//	hr=IDirectSoundBuffer_GetStatus((LPDIRECTSOUNDBUFFER)sh->shandle,(DWORD*)&status);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_GetStatus %08x",hr);
	if (status&DSBSTATUS_PLAYING)
		return 1;
	return 0;
}

/*static void ds_playasound(struct wavehandle* wh)
{
	struct soundhandle* sh;
	sh=ds_wave_playvol(wh,1,.75f);
	if (sh==0)
		return;
	sh->isplayasound=1;
}
*/
static void ds_playasoundvol(struct wavehandle* wh,float vol)
{
	struct soundhandle* sh;
	sh=ds_wave_playvol(wh,1,vol);
	if (sh==0)
		return;
	sh->isplayasound=1;
}

static LPDIRECTSOUNDCAPTURE miclpDirectSound;
static LPDIRECTSOUNDCAPTUREBUFFER micdsbuff;
//static DSBUFFERDESC dsbufferdesc,dsprimarydesc;
static GUID micdriverguid,*miclpdriverguid,miclastguid,*miclplastguid;
//static S32 micsubdriver;
static S32 micnsubdrivers;
static S32 micdshalf,micdssizehalf;

static void ds_audio_babysit() // just for playasound and streaming
{
	U8* buff_ptr;
	U32 buff_bytes;
	C8* ptr1,*ptr2;
	S32 size1,size2;
	HRESULT hr;
	S32 rdpos=-1;
//	S32 streamaudiopos;
	S32 i;
	struct soundhandle* sh;
	if (!dspaused)
		for (i=0;i<maxdvsounds;i++) {
			sh=&soundhandles[i];
			if (sh->isplayasound) {
				if (!ds_sound_status(sh)) {
					ds_sound_free(sh);
				}
			}
			if (sh->wh && sh->wh->streamplaying) {
				hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->GetCurrentPosition(
					(DWORD*)&audioinfo.streamaudiopos,0);
//				hr=IDirectSoundBuffer_GetCurrentPosition(
//					(LPDIRECTSOUNDBUFFER)sh->shandle,(DWORD*)&audioinfo.streamaudiopos,0);
				if (hr!=DS_OK)
					errorexit("IDirectSoundBuffer_GetCurrentPosition %08x",hr);
				if (audioinfo.streamaudiopos>=sh->wh->streamstartwindow[sh->wh->streamcurwindow] &&
					audioinfo.streamaudiopos<sh->wh->streamendwindow[sh->wh->streamcurwindow])
					ds_streamsound(sh);
			}
			if (sh->fifosize) {
				hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->GetCurrentPosition(
					(DWORD*)&audioinfo.streamaudiopos,0);
//				hr=IDirectSoundBuffer_GetCurrentPosition(
//					(LPDIRECTSOUNDBUFFER)sh->shandle,(DWORD*)&audioinfo.streamaudiopos,0);
				if (hr!=DS_OK)
					errorexit("IDirectSoundBuffer_GetCurrentPosition %08x",hr);
				if (sh->fifohalf)
					audioinfo.streamaudiopos-=(sh->fifosize>>1);
				if (audioinfo.streamaudiopos>=0 && audioinfo.streamaudiopos<(sh->fifosize>>2)) {
					sh->fifohalf^=1;
					hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Lock(
						sh->fifohalf*(sh->fifosize>>1),sh->fifosize>>1,
						(void**)&buff_ptr,(LPDWORD)(void*)&buff_bytes,0,0,0);
//					hr=IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)sh->shandle,
//						sh->fifohalf*(sh->fifosize>>1),sh->fifosize>>1,
//						(void**)&buff_ptr,(LPDWORD)(void*)&buff_bytes,0,0,0);
					if (hr!=DS_OK)
						errorexit("streamIDirectSoundBuffer_Lock %08x",hr);
					(sh->fifocallback)(buff_ptr,buff_bytes);
					((LPDIRECTSOUNDBUFFER)sh->shandle)->Unlock(
						buff_ptr,buff_bytes,0,0);
//					IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)sh->shandle,
//						buff_ptr,buff_bytes,0,0);
					if (hr!=DS_OK)
						errorexit("streamIDirectSoundBuffer_UnLock %08x",hr);
				}
			}
		}
	if (!micdsbuff)
		return;
	hr=micdsbuff->GetCurrentPosition(0,(DWORD*)&rdpos);
//	hr=IDirectSoundCaptureBuffer_GetCurrentPosition(micdsbuff,0,(DWORD*)&rdpos);
	if (hr!=DS_OK || rdpos<0)
		return;
	if (rdpos>=micdshalf && rdpos<micdshalf+(micdssizehalf>>1)) {
		micdshalf=micdssizehalf-micdshalf;
		hr=micdsbuff->Lock(
		  micdshalf,micdssizehalf,(void**)&ptr1,(DWORD*)&size1,(void**)&ptr2,(DWORD*)&size2,0);      
//		hr=IDirectSoundBuffer_Lock(
//		  micdsbuff,micdshalf,micdssizehalf,(void**)&ptr1,(DWORD*)&size1,(void**)&ptr2,(DWORD*)&size2,0);      
		if (size1)
			(mfifocallback)(ptr1,size1);
		if (size2)
			(mfifocallback)(ptr2,size2);
		hr=micdsbuff->Unlock(ptr1,size1,ptr2,size2);      
//		hr=IDirectSoundBuffer_Unlock(micdsbuff,ptr1,size1,ptr2,size2);      
	}
}

static void ds_audio_uninit()
{
	S32 i;
	HRESULT hr;
	dspaused=0;
	for (i=0;i<maxdvsounds;i++)
		ds_sound_free(&soundhandles[i]);	
	hr=lpDirectSound->Release();
//	hr=IDirectSound_Release((LPDIRECTSOUND)lpDirectSound);
	if (hr!=DS_OK)
		errorexit("IDirectSound_Release %08x",hr);
//	if (fpstream)
//		fclose(fpstream);
//	streamplaying=0;
//	streamstopped=0;
}

static void ds_setsfxpan(struct soundhandle* sh,float pan) // -1 to 1, left to right
{
	HRESULT hr;
	if (sh && (sh->wh || sh->fifosize)) {
		hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->SetPan(pan2db(pan));
//		hr=IDirectSoundBuffer_SetPan((LPDIRECTSOUNDBUFFER)sh->shandle,pan2db(pan));
		if (hr!=DS_OK)
			errorexit("IDirectSoundBuffer_SetPan %08x",hr);
		sh->pan=pan;
	}
}

static float ds_getsfxpan(struct soundhandle* sh)
{
	if (sh && (sh->wh || sh->fifosize))
		return sh->pan;
	return 0;
}

static void ds_setsfxfreq(struct soundhandle* sh,float freq) // .001 to 1000
{
	HRESULT hr;
	S32 ifreq;
	if (sh && (sh->wh || sh->fifosize)) {
		ifreq=(S32)(sh->basefreq*freq);
		ifreq=range(100,ifreq,100000);
		hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->SetFrequency(ifreq);
//		hr=IDirectSoundBuffer_SetFrequency((LPDIRECTSOUNDBUFFER)sh->shandle,ifreq);
//		if (hr!=DS_OK)
//			errorexit("IDirectSoundBuffer_SetFrequency %08x",hr);
		sh->freq=freq;
	}
}

static float ds_getsfxfreq(struct soundhandle* sh)
{
	if (sh && (sh->wh || sh->fifosize))
		return sh->freq;
	return 0;
}

static void ds_soundpause()
{
	S32 i;
	S32 hr;
	struct soundhandle* sh;
	dspaused=1;
	for (sh=soundhandles,i=0;i<maxdvsounds;i++,sh++)
		if (sh->wh || sh->fifosize)
			hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Stop();
//			hr=IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)sh->shandle);
}

static void ds_soundresume()
{
	S32 i;
	S32 looping;
	S32 hr;
	struct soundhandle* sh;
	dspaused=0;
	for (sh=soundhandles,i=0;i<maxdvsounds;i++,sh++)
 		if (sh->wh || sh->fifosize) {
			if (sh->loopcnt==0)
				looping=DSBPLAY_LOOPING;
			else
				looping=0;
			hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Play(0,0,looping);
//			hr=IDirectSoundBuffer_Play((LPDIRECTSOUNDBUFFER)sh->shandle,0,0,looping);
		}
}


static script* scd;

static BOOL CALLBACK dssubdriversenumcallback(GUID FAR* lpGUID,
	LPSTR lpDriverDescription,LPSTR lpDriverName,LPVOID lpContext)          
{
	scd->addscript(lpDriverDescription);
	return TRUE;
}

// return a script of all valid direct sound cards
script* ds_ndrv()
{
	S32 hr;
	scd=new script;
//	sc=addscript(sc,"Primary",&nsc);
//	*ndrivers=nsc;
	hr=DirectSoundEnumerate((LPDSENUMCALLBACK)dssubdriversenumcallback,0);
	if (hr!=DS_OK)
		errorexit("DirectSoundEnumerate %08x",hr);
//	C8** sc=0;
//	S32 nsc=0;
	return scd;
}

//static C8** micsc;
//static S32 nmicsc;
static script* micsc;
static BOOL CALLBACK nummicdirectsoundenumcallback(LPGUID lpGuid,LPCSTR lpcstrDescription,PCSTR lpcstrModule,LPVOID lpContext)
{
	micsc->addscript((C8* )lpcstrDescription);
//	micsc=addscript(micsc,(C8* )lpcstrDescription,&nmicsc);
	return TRUE;
}

script* ds_mic_ndrv()
{
	HRESULT hr;
	micsc=new script;
//	micsc=0;
//	nmicsc=0;
//	DSCAPS dscaps;  
	hr=DirectSoundCaptureEnumerate(nummicdirectsoundenumcallback,0);
	if (hr!=DS_OK)
		errorexit("micDirectSoundEnumerate %08x",hr);
//	*ndrivers=nmicsc;
	return micsc;
}

/*
C8** ds_mic_ndrv(S32* ndrivers)
{
	HRESULT hr;
	micsc=0;
	nmicsc=0;
//	DSCAPS dscaps;  
	hr=DirectSoundCaptureEnumerate(nummicdirectsoundenumcallback,0);
	if (hr!=DS_OK)
		errorexit("micDirectSoundEnumerate %08x",hr);
	*ndrivers=nmicsc;
	return micsc;
}
*/


struct soundhandle* ds_fifo_playvol(S32 samplerate,S32 stereo,S32 sixteenbit,
									void (*callback)(U8* ,S32),float vol)
{
	HRESULT hr;
	U8* buff_ptr;
	U32 buff_bytes;
	struct mywaveformatex fmt;
	struct soundhandle* sh;
	sh=ds_getfreesoundslot();
	if (sh==0)
		return 0;
	fmt.wFormatTag=WAVE_FORMAT_PCM;
	fmt.nChannels=stereo+1;
	fmt.wBitsPerSample=(1+sixteenbit)*8;
	fmt.nBlockAlign=fmt.nChannels*(sixteenbit+1);
	fmt.nSamplesPerSec=samplerate;
	fmt.nAvgBytesPerSec=samplerate*fmt.nBlockAlign;
	fmt.cbSize=sizeof(struct mywaveformatex);
//	ret->stream=strm;
// get a buffer with format and length set
	dsbufferdesc.dwSize=sizeof(DSBUFFERDESC);
    dsbufferdesc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY|
		DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_STICKYFOCUS; 
	dsbufferdesc.dwBufferBytes=fmt.nAvgBytesPerSec*nfifosecn/nfifosecd; 
	dsbufferdesc.dwBufferBytes+=15;
	dsbufferdesc.dwBufferBytes&=~15; // round up to a multiple of 16 bytes..
//	wh->streambufferhalf=dsbufferdesc.dwBufferBytes/2;
//	wh->streamstartwindow[0]=0;
//	wh->streamendwindow[0]=wh->streambufferhalf/2;
//	wh->streamstartwindow[1]=wh->streambufferhalf;
//	wh->streamendwindow[1]=wh->streambufferhalf*3/2;
	dsbufferdesc.lpwfxFormat=(tWAVEFORMATEX*)&fmt;

	hr=lpDirectSound->CreateSoundBuffer(&dsbufferdesc,(LPDIRECTSOUNDBUFFER* )&sh->shandle,0);
//	hr=IDirectSound_CreateSoundBuffer(lpDirectSound,
//		&dsbufferdesc,(LPDIRECTSOUNDBUFFER* )&sh->shandle,0);
	if (hr!=DS_OK)
		errorexit("IDirectSound_CreateSoundBuffer %08x",hr);
	
	sh->pan=0;
	sh->fifocallback=callback;
	ds_setsfxvolume(sh,vol);
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->GetFrequency((DWORD*)&sh->basefreq);
//	hr=IDirectSoundBuffer_GetFrequency((LPDIRECTSOUNDBUFFER)sh->shandle,(DWORD*)&sh->basefreq);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_GetFrequency %08x",hr);

	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Lock(
		0,dsbufferdesc.dwBufferBytes,(void**)&buff_ptr,(DWORD*)&buff_bytes,0,0,DSBLOCK_ENTIREBUFFER);
//	hr=IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)sh->shandle,
//		0,dsbufferdesc.dwBufferBytes,(void**)&buff_ptr,(DWORD*)&buff_bytes,0,0,DSBLOCK_ENTIREBUFFER);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_Lock %08x",hr);
// make sure ret # of bytes from lock same as in WAVEHDR
	if (buff_bytes!=dsbufferdesc.dwBufferBytes)
		errorexit("buff_bytes!=ret->whdr.dwBufferLength");
// copy memory in the secondary sound buffer and free it from wavememory
	(sh->fifocallback)(buff_ptr,buff_bytes);
	((LPDIRECTSOUNDBUFFER)sh->shandle)->Unlock(buff_ptr,buff_bytes,0,0);
//	IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)sh->shandle,buff_ptr,buff_bytes,0,0);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_UnLock %08x",hr);
	
	
	hr=((LPDIRECTSOUNDBUFFER)sh->shandle)->Play(0,0,DSBPLAY_LOOPING);
//	hr=IDirectSoundBuffer_Play((LPDIRECTSOUNDBUFFER)sh->shandle,0,0,DSBPLAY_LOOPING);
	if (hr!=DS_OK)
		errorexit("IDirectSoundBuffer_Play %08x",hr);
	sh->freq=1;
	sh->fifosize=dsbufferdesc.dwBufferBytes;
	sh->fifohalf=1;
	return sh;
}


/////////////////////////////////////////////////////////////////
////////////////////////// win32 waveform audio... 
/////////////////////////////////////////////////////////////////

static struct wavehandle* pausedwh;
static float pausedvol;
static S32 pauseisplayasound;
static S32 volsave;//,midivolsave;
struct mywaveformatex fi={
    1,1,11025,11025,1,8,0
};

static S32 wp_audio_init()
{
	HWAVEOUT wh;
//	HMIDIOUT mh;
	S32 i;
	S32 result;
	WAVEOUTCAPS woc;
//	MIDIOUTCAPS moc;
	pausedwh=0;
	nsubdrivers=waveOutGetNumDevs(); 
	logger(" waveplayer init:\n");
	if (nsubdrivers<=0)
		return 0;
	for (i=0;i<nsubdrivers;i++) {
		result=waveOutGetDevCaps(i,&woc,sizeof(woc));
		logger("result %d,WAVEOUTCAPS dwSupport = %08x\n",result,woc.dwSupport);
		if (audioinfo.audio_subdriver==i)
			logger("  * ");
		else
			logger("    ");
		logger("subdriver %2d '%s'\n",i,woc.szPname);
	}
	if (audioinfo.audio_subdriver>=nsubdrivers)
//		errorexit("waveplayer: pick a lower audiosubdriver");
		audioinfo.audio_subdriver=nsubdrivers-1;
	result=waveOutOpen(&wh,audioinfo.audio_subdriver,(WAVEFORMATEX*)&fi,0,0,0);
	result=waveOutGetVolume(wh, (DWORD*)&volsave);
	result=waveOutClose(wh);
// midi
/*	nmidisubdrivers=midiOutGetNumDevs(); 
	logger(" midiplayer init:\n");
	if (nmidisubdrivers<=0)
		return 0;
	for (i=0;i<nmidisubdrivers;i++) {
		result=midiOutGetDevCaps(i,&moc,sizeof(moc));
		logger("result %d,MIDIOUTCAPS dwSupport = %08x\n",result,moc.dwSupport);
		if (midi_subdriver==i)
			logger("  * ");
		else
			logger("    ");
		logger("midisubdriver %2d '%s'\n",i,moc.szPname);
	}
	if (midi_subdriver>=nmidisubdrivers)
//		errorexit("midiplayer: pick a lower midisubdriver");
		midi_subdriver=nmidisubdrivers-1;
	result=midiOutOpen(&mh,midi_subdriver,0,0,CALLBACK_0);
	result=midiOutGetVolume(mh, &midivolsave);
	result=midiOutClose(mh);
*/
	return 1;
}

static struct wavehandle* wp_wave_load(const C8* fn,S32 stream)
{
	struct wavehandle* ret;
	//	if (stream!=2) // 2 means fn is actually a FILE* 
//		stream=0;
//	if (fpstream)
//		stream=0;
	ret=loadwavehandle(fn,stream);
	if (stream&WOPT_STREAM) {
		ret->streambufferhalf=dsbufferdesc.dwBufferBytes/2;
//		ret->streamstartwindow[0]=0;
//		ret->streamendwindow[0]=ret->streambufferhalf/2;
//		ret->streamstartwindow[1]=ret->streambufferhalf;
//		ret->streamendwindow[1]=ret->streambufferhalf*3/2;
	}
//	if (stream==2)
//		stream=0;
//	if (ret==0)
//		return 0;
//	ret->stream=stream;
	return ret;
}

static struct soundhandle* wp_getfreesoundslot()
{
	S32 i;
	for (i=0;i<maxdvsounds;i++)
		if (soundhandles[i].wh==0 && soundhandles[i].fifosize==0)
			return &soundhandles[i];
	return 0;
}

static void wp_sound_free(struct soundhandle* sh)
{
	S32 result;
	struct wavehandle* wh;
	if (sh==0)
		return;
	wh=sh->wh;
	if (wh==0 && sh->fifosize==0)
		return;
	result=waveOutReset((HWAVEOUT)sh->shandle); 
	if ((sh->wh && sh->wh->fpstream) || sh->fifosize) {
		result=waveOutUnprepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR));
		result=waveOutUnprepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[1],sizeof(WAVEHDR));
//		memfree(sh->whdrs[0].lpData);
		delete[] sh->whdrs[0].lpData;
		sh->whdrs[0].lpData=0;
//		memfree(sh->whdrs[1].lpData);
		delete[] sh->whdrs[1].lpData;
		sh->whdrs[1].lpData=0;
	} else {
		result=waveOutUnprepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR));
		sh->whdrs[0].lpData=0;
	}
//	
	result=waveOutClose((HWAVEOUT)sh->shandle);
	sh->wh=0;
	sh->fifosize=0;
	sh->isplayasound=0;
//	gwh=0;
}

static void wp_streamsound(struct soundhandle* sh)
{
	S32 result;
	U8* buff_ptr;
	S32 buff_bytes;
	U32 rt,lt;
	sh->wh->streamcurwindow^=1;
	if (sh->wh->streamsilence) { // done playing the silence
/*		if (sh->loopcnt!=1) {
			sh->wh->streamcurrent=0;
			fseek(sh->wh->fpstream,sh->wh->streamdataoffset,FILE_START);
			sh->wh->streamsilence=0;
			if (sh->loopcnt!=0)
				sh->loopcnt--;
		} else */
//		{
		result=waveOutReset((HWAVEOUT)sh->shandle); 
		sh->wh->streamplaying=0;
		return;
//		}
	}
	result=waveOutUnprepareHeader(
		(HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[sh->wh->streamcurwindow],sizeof(WAVEHDR));
	buff_ptr=sh->whdrs[sh->wh->streamcurwindow].lpData;
	buff_bytes=sh->whdrs[0].dwBufferLength;
	if (sh->wh->streamcurrent+buff_bytes>=sh->wh->streamend) {
		lt=sh->wh->streamend-sh->wh->streamcurrent;
		rt=buff_bytes-lt;
		if (sh->loopcnt==1)
			sh->wh->streamsilence=1;
		else if (sh->loopcnt!=0)
			sh->loopcnt--;
		if (lt)
			fread(buff_ptr,1,lt,sh->wh->fpstream);	
		if (sh->wh->streamsilence) {
			if (rt) {
				if (sh->wh->format_info.wBitsPerSample==8)
					memset(buff_ptr+lt,128,rt);
				else
					memset(buff_ptr+lt,0,rt);
			}
		} else {
			fseek(sh->wh->fpstream,sh->wh->streamdataoffset,SEEK_SET);
			fread(buff_ptr+lt,1,rt,sh->wh->fpstream);	
			sh->wh->streamcurrent=rt;
		}
	} else {
		fread(buff_ptr,1,buff_bytes,sh->wh->fpstream);
		sh->wh->streamcurrent+=buff_bytes;
	}
	result=waveOutPrepareHeader(
		(HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[sh->wh->streamcurwindow],sizeof(WAVEHDR));
	result=waveOutWrite(
		(HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[sh->wh->streamcurwindow],sizeof(WAVEHDR)); 
}

static struct soundhandle* wp_wave_playvol(struct wavehandle* wh,S32 loopcount,float vol)
{
	S32 result;
	S32 val;
	struct soundhandle* sh;
	pausedwh=0;
	if (wh==0)
		return 0;
//	if (wh->whdr.lpData==0)
//		return 0;
	sh=wp_getfreesoundslot();
	if (sh==0) {
		wp_sound_free(&soundhandles[0]); // kill old wave
		sh=&soundhandles[0]; // now youv'e got a free slot
//		return 0; // keep old wave playing
	}
	sh->pan=0; // we really don't know what the pan is set to.
	sh->freq=1;
	sh->wh=wh;
// open and prepare the wave
//	result=waveOutOpen((HWAVEOUT* )&sh->shandle,audio_subdriver,(WAVEFORMATEX* )&wh->format_info,0,0,0);
	result=waveOutOpen((HWAVEOUT* )&sh->shandle,audioinfo.audio_subdriver,(WAVEFORMATEX* )&wh->format_info,0,0,0);
//WAVE_MAPPER
	val=(S32)(vol*65536.0f);
	val=range(0,val,65535);
	memset(&sh->whdrs[0],0,sizeof(sh->whdrs[0]));
	memset(&sh->whdrs[1],0,sizeof(sh->whdrs[1]));
	if (wh->fpstream) {

		wh->streamplaying=1;
		fseek(wh->fpstream,wh->streamdataoffset,SEEK_SET);
		wh->streamcurrent=0;
		wh->streamcurwindow=1;
		wh->streamsilence=0;
	
		sh->whdrs[0].dwBufferLength=wh->format_info.nAvgBytesPerSec*nstreamsec; 
		sh->whdrs[0].dwBufferLength+=15;
		sh->whdrs[0].dwBufferLength&=~15; // round up to a multiple of 16 bytes..
		sh->whdrs[0].dwBufferLength/=2;
//		sh->whdrs[0].lpData=(U8*)memalloc(sh->whdrs[0].dwBufferLength);
		sh->whdrs[0].lpData=new U8[sh->whdrs[0].dwBufferLength];
		sh->whdrs[1].dwBufferLength=sh->whdrs[0].dwBufferLength;
//		sh->whdrs[1].lpData=(U8*)memalloc(sh->whdrs[0].dwBufferLength);
		sh->whdrs[1].lpData=new U8[sh->whdrs[0].dwBufferLength];
		result=waveOutPrepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR));
		result=waveOutPrepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[1],sizeof(WAVEHDR));
//	waveOutReset((HWAVEOUT)sh->shandle); 
		waveOutSetVolume((HWAVEOUT)sh->shandle, val + (val<<16)); 
// these 2 lines enable looping, dwloops only works with 0, loop forever
//		if (loopcount==0) {
//		sh->whdrs[0].dwFlags=0; //WHDR_BEGINLOOP; // enable looping
		sh->whdrs[0].dwLoops=0; //loopcount; 
//		sh->whdrs[1].dwFlags=0; //WHDR_ENDLOOP; // enable looping
		sh->whdrs[1].dwLoops=0; //loopcount; 
//		} else if (loopcount==1) {
//			sh->whdrs[0].dwFlags&=~(WHDR_BEGINLOOP|WHDR_ENDLOOP); // disable looping
//			sh->whdrs[0].dwLoops=0; 
//		}
		wp_streamsound(sh);
		wp_streamsound(sh);

			
//		result=waveOutWrite((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR)); 
//		result=waveOutWrite((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[1],sizeof(WAVEHDR)); 
		sh->loopcnt=loopcount;
	} else {
		sh->whdrs[0]=wh->whdrw;
		result=waveOutPrepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR));
//	waveOutReset((HWAVEOUT)sh->shandle); 
		waveOutSetVolume((HWAVEOUT)sh->shandle, val + (val<<16)); 
// these 2 lines enable looping, dwloops only works with 0, loop forever
		if (loopcount==0) {
			sh->whdrs[0].dwFlags|=WHDR_BEGINLOOP|WHDR_ENDLOOP; // enable looping
			sh->whdrs[0].dwLoops=1000000; //loopcount; 
		} else if (loopcount==1) {
			sh->whdrs[0].dwFlags&=~(WHDR_BEGINLOOP|WHDR_ENDLOOP); // disable looping
			sh->whdrs[0].dwLoops=0; 
		}
		result=waveOutWrite((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR)); 
	}
//	sh->basefreq=16000;
	sh->basefreq=fi.nSamplesPerSec;
	return sh;
}

static void wp_wave_unload(struct wavehandle* wh)
{
	S32 i;
	pausedwh=0;
	if (wh==0)
		return;
	for (i=0;i<maxdvsounds;i++)
		if (soundhandles[i].wh==wh)
			wp_sound_free(&soundhandles[i]);
	if (wh->fpstream) {
//		if (fpstream)
			fclose(wh->fpstream);
//		fpstream=0;
	}
	if (wh->whdrw.lpData)
//		memfree(wh->whdrw.lpData);
		delete[] wh->whdrw.lpData;
//	memfree(wh->name);
//	delete[] wh->name;
//	memfree(wh);
	delete wh;
}

static S32 wp_sound_status(struct soundhandle* sh)
{
	struct wavehandle* wh;
	if (sh==0)
		return 0;
	if (sh->fifosize)
		return 1;
	wh=sh->wh;
	if (wh) {
		if (wh->fpstream) {
			if ((sh->whdrs[0].dwFlags & WHDR_DONE) && (sh->whdrs[1].dwFlags & WHDR_DONE))
				return 0;
			return 1;
		}
		if (sh->whdrs[0].dwFlags & WHDR_DONE)
			return 0;
		return 1;
	}
	return 0;
}

/*static void wp_playasound(struct wavehandle* wh)
{
	struct soundhandle* sh;
	sh=wp_wave_playvol(wh,1,.75f);
	if (sh==0)
		return;
	sh->isplayasound=1;
}
*/
static void wp_setsfxvolume(struct soundhandle* sh,float vol)
{ 
// low order is left, hi order is right
	S32 lval,rval;
	float fval;
	if (audioinfo.disablewpsetvol)
		return;
	if (sh && sh->wh) {
		fval=65536.0f*vol;
		if (sh->pan>=0) { // right
			lval=(S32)(fval*(1-sh->pan));
			rval=(S32)fval;
		} else { // left
			lval=(S32)fval;
			rval=(S32)(fval*(1+sh->pan));
		}
		lval=range(0,lval,65535);
		rval=range(0,rval,65535);
		waveOutSetVolume((HWAVEOUT)sh->shandle, lval + (rval<<16)); 
	}
}

static void wp_playasoundvol(struct wavehandle* wh,float vol)
{
	struct soundhandle* sh;
	sh=wp_wave_playvol(wh,1,vol);
	if (sh==0)
		return;
	sh->isplayasound=1;
}

static void wp_audio_babysit() // just for playasound
{
//	extern struct con16* testcn;
	S32 i,result;
	struct soundhandle* sh;
	for (i=0;i<maxdvsounds;i++) {
		sh=&soundhandles[i];
		if (sh->isplayasound) {
			if (!wp_sound_status(sh)) {
				wp_sound_free(sh);
			}
		}
		if (sh->wh && sh->wh->streamplaying) {
			if ((sh->whdrs[0].dwFlags&WHDR_DONE) || (sh->whdrs[1].dwFlags&WHDR_DONE))
				wp_streamsound(sh);
		}
	
		if (sh->fifosize) {
			if ((sh->whdrs[0].dwFlags&WHDR_DONE) || (sh->whdrs[1].dwFlags&WHDR_DONE)) {
				sh->fifohalf^=1;
				result=waveOutUnprepareHeader(
					(HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[sh->fifohalf],sizeof(WAVEHDR));
				(sh->fifocallback)(sh->whdrs[sh->fifohalf].lpData,sh->whdrs[0].dwBufferLength);
				result=waveOutPrepareHeader(
					(HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[sh->fifohalf],sizeof(WAVEHDR));
				result=waveOutWrite(
					(HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[sh->fifohalf],sizeof(WAVEHDR));
			}
		}
	}
	if (tmic_wh) {
		i=-1;
		if (tmic_whdrs[0].dwFlags&WHDR_DONE)
			i=0;
		else if (tmic_whdrs[1].dwFlags&WHDR_DONE)
			i=1;
		if (i>=0) {
//			con16_printf(testcn,"br0 %d, br1 %d\n",tmic_whdrs[0].dwBytesRecorded,tmic_whdrs[1].dwBytesRecorded);
			result=waveInUnprepareHeader(
				(HWAVEIN)tmic_wh,(WAVEHDR* )&tmic_whdrs[i],sizeof(WAVEHDR));
			(mfifocallback)(tmic_whdrs[i].lpData,tmic_whdrs[i].dwBufferLength);
			result=waveInPrepareHeader(
				(HWAVEIN)tmic_wh,(WAVEHDR* )&tmic_whdrs[i],sizeof(WAVEHDR));
			result=waveInAddBuffer(
				(HWAVEIN)tmic_wh,(WAVEHDR* )&tmic_whdrs[i],sizeof(WAVEHDR));
		}
	}

}

static void wp_audio_uninit()
{
	HWAVEOUT wh;
//	HMIDIOUT mh;
	S32 i,result;
	pausedwh=0;
	for (i=0;i<maxdvsounds;i++)
		wp_sound_free(&soundhandles[i]);
	result=waveOutOpen(&wh,audioinfo.audio_subdriver,(WAVEFORMATEX*)&fi,0,0,0);
	result=waveOutSetVolume(wh,volsave);
	result=waveOutClose(wh);

//	result=midiOutOpen(&mh,midi_subdriver,0,0,CALLBACK_0);
//	result=midiOutSetVolume(mh, midivolsave);
//	result=midiOutClose(mh);
}

/*// test function
S32 wp_getrawvolume(struct soundhandle* sh)
{
	U32 v;
	if (sh && sh->wh) {
		waveOutGetVolume((HWAVEOUT)sh->shandle, &v);
		return v;
	}
	return 0;
}
*/

static float wp_getsfxvolume(struct soundhandle* sh)
{
	U32 v,vl,vr;
	if (sh && sh->wh) {
		waveOutGetVolume((HWAVEOUT)sh->shandle, (DWORD*)&v);
		vl=v&0xffff;
		vr=v>>16;
		v=max(vl,vr);
//		v=vl+vr; // l+r
		return (1.0f/65536.0f)*v;
	}
	return 0;
}

static void wp_setsfxpan(struct soundhandle* sh,float pan) // -1 to 1, left to right
{
//	sfxvolume=val;
// low order is left, hi order is right
	S32 lval,rval;
	float vol,fval;
	if (audioinfo.disablewpsetvol)
		return;
	if (sh && sh->wh) {
		vol=wp_getsfxvolume(sh);
		fval=65536.0f*vol;
		if (pan>=0) { // right
			lval=(S32)(fval*(1-pan));
			rval=(S32)fval;
		} else { // left
			lval=(S32)fval;
			rval=(S32)(fval*(1+pan));
		}
		lval=range(0,lval,65535);
		rval=range(0,rval,65535);
		waveOutSetVolume((HWAVEOUT)sh->shandle, lval + (rval<<16)); 
		sh->pan=pan;
	}
}

static float wp_getsfxpan(struct soundhandle* sh)
{
	if (sh && sh->wh)
		return sh->pan;
	return 0;
}

// my riptide doesn't support waveOutSetPlaybackRate nor waveOutSetPitch,
//               see WAVEOUTCAPS struct for more details audio_init();
static void wp_setsfxfreq(struct soundhandle* sh,float freq) // .001 to 1000
{
	if (sh && sh->wh) {
//		waveOutSetPlaybackRate((HWAVEOUT)sh->shandle,(S32)(freq*65536.0f));
		sh->freq=freq;
	}
}

static float wp_getsfxfreq(struct soundhandle* sh)
{
	if (sh && sh->wh)
		return sh->freq;
	return 0;
}

static void wp_soundresume()
{
// this code did not reliably work!
//	S32 i;
//	S32 ret;
//	for (i=0;i<maxdvsounds;i++)
//		if (soundhandles[i].wh)
//			waveOutRestart((HWAVEOUT)soundhandles[i].shandle);
//	logger("sound resume %08x\n",ret);
// play sound that was paused
	struct soundhandle* sh;
	if (pausedwh) {
		sh=wp_wave_playvol(pausedwh,-1,pausedvol);
		sh->isplayasound=pauseisplayasound;
	}
}

static void wp_soundpause()
{
// this code did not reliably work!
//	S32 i;
//	S32 ret;
//	wp_soundresume();
//	for (i=0;i<maxdvsounds;i++)
//		if (soundhandles[i].wh)
//			ret=waveOutPause((HWAVEOUT)soundhandles[i].shandle);
// i'll just kill any sounds going on here
	S32 i;
	pausedwh=0;
	for (i=0;i<maxdvsounds;i++)
		if (soundhandles[i].wh) {
			pausedwh=soundhandles[i].wh;
			pauseisplayasound=soundhandles[i].isplayasound;
			pausedvol=wp_getsfxfreq(&soundhandles[i]);
			wp_sound_free(&soundhandles[i]);
		}
}

script* wp_ndrv()
{
	script* ret=new script;
	ret->addscript("Primary");
	return ret;
}
/*C8** wp_ndrv(S32* ndrivers)
{
	C8** sc=0;
	*ndrivers=0;
	sc=addscript(sc,"Primary",ndrivers);
	return sc;
}
*/
script* wp_mic_ndrv()
{
	script* ret=new script;
	S32 i,n;
	WAVEINCAPS wic;
//	C8** sc=0;
//	*ndrivers=0;
	n=waveInGetNumDevs();
	for (i=0;i<n;i++) {
		waveInGetDevCaps(i,&wic,sizeof(wic));
//		sc=addscript(sc,wic.szPname,ndrivers);
		ret->addscript(wic.szPname);
	}
	return ret;
}

struct soundhandle* wp_fifo_playvol(S32 samplerate,S32 stereo,S32 sixteenbit,
									void (*callback)(U8* ,S32),float vol)
{
	S32 result;
	S32 val;
	struct mywaveformatex fmt;
	struct soundhandle* sh;
	pausedwh=0;
	sh=wp_getfreesoundslot();
	if (sh==0) {
		wp_sound_free(&soundhandles[0]); // kill old wave
		sh=&soundhandles[0]; // now youv'e got a free slot
//		return 0; // keep old wave playing
	}
	sh->pan=0; // we really don't know what the pan is set to.
	sh->freq=1;
	sh->wh=0;
	memset(&fmt,0,sizeof(fmt));
	fmt.wFormatTag=WAVE_FORMAT_PCM;
	fmt.nChannels=1+stereo;
	fmt.nSamplesPerSec=samplerate;
	fmt.nBlockAlign=(1+sixteenbit)*fmt.nChannels;
	fmt.nAvgBytesPerSec=fmt.nBlockAlign*fmt.nSamplesPerSec;
	fmt.wBitsPerSample=(1+sixteenbit)*8;
// open and prepare the wave
//	result=waveOutOpen((HWAVEOUT* )&sh->shandle,audio_subdriver,(WAVEFORMATEX* )&wh->format_info,0,0,0);
	result=waveOutOpen((HWAVEOUT* )&sh->shandle,audioinfo.audio_subdriver,(WAVEFORMATEX*)&fmt,0,0,0);
//WAVE_MAPPER
	val=(S32)(vol*65536.0f);
	val=range(0,val,65535);
	memset(&sh->whdrs[0],0,sizeof(sh->whdrs[0]));
	memset(&sh->whdrs[1],0,sizeof(sh->whdrs[1]));

	sh->whdrs[0].dwBufferLength=fmt.nAvgBytesPerSec*nfifosecn/nfifosecd;
	sh->whdrs[0].dwBufferLength+=15;
	sh->whdrs[0].dwBufferLength&=~15; // round up to a multiple of 16 bytes..
	sh->whdrs[0].dwBufferLength/=2;
	sh->whdrs[0].lpData=new U8[sh->whdrs[0].dwBufferLength];
//	sh->whdrs[0].lpData=(U8*)memalloc(sh->whdrs[0].dwBufferLength);
	sh->whdrs[1].dwBufferLength=sh->whdrs[0].dwBufferLength;
	sh->whdrs[1].lpData=new U8[sh->whdrs[0].dwBufferLength];
//	sh->whdrs[1].lpData=(U8*)memalloc(sh->whdrs[0].dwBufferLength);
	sh->fifocallback=callback;

	(sh->fifocallback)(sh->whdrs[0].lpData,sh->whdrs[0].dwBufferLength);
	(sh->fifocallback)(sh->whdrs[1].lpData,sh->whdrs[1].dwBufferLength);
	result=waveOutPrepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR));
	result=waveOutPrepareHeader((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[1],sizeof(WAVEHDR));
//	waveOutReset((HWAVEOUT)sh->shandle); 
	waveOutSetVolume((HWAVEOUT)sh->shandle, val + (val<<16)); 
// these 2 lines enable looping, dwloops only works with 0, loop forever
//		if (loopcount==0) {
//	sh->whdrs[0].dwFlags=0; //WHDR_BEGINLOOP; // enable looping
	sh->whdrs[0].dwLoops=0; //loopcount; 
//	sh->whdrs[1].dwFlags=0; //WHDR_ENDLOOP; // enable looping
	sh->whdrs[1].dwLoops=0; //loopcount; 
//		} else if (loopcount==1) {
//			sh->whdrs[0].dwFlags&=~(WHDR_BEGINLOOP|WHDR_ENDLOOP); // disable looping
//			sh->whdrs[0].dwLoops=0; 
//		}
	sh->pan=0;
//	wp_streamsound(sh);
//	wp_streamsound(sh);

	result=waveOutWrite((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[0],sizeof(WAVEHDR)); 
	result=waveOutWrite((HWAVEOUT)sh->shandle,(WAVEHDR* )&sh->whdrs[1],sizeof(WAVEHDR)); 
	sh->fifosize=2*sh->whdrs[0].dwBufferLength;
	sh->fifohalf=1;
	return sh;
}

// internal

struct audioapi {
	S32 maxsounds;
	script* (*audio_getnsubdrivers)();
//	C8** (*audio_getnsubdrivers)(S32* nsub);
	S32 (*audio_initfunc)();
	void (*audio_uninitfunc)();
	void (*audio_babysitfunc)();
	struct wavehandle* (*wave_loadfunc)(const C8* ,S32);
	void (*wave_unloadfunc)(struct wavehandle* );
	struct soundhandle* (*wave_playvolfunc)(struct wavehandle* ,S32 loopcount,float vol);	// 0 loop forever, 1 just once
	void (*playasoundvolfunc)(struct wavehandle* ,float vol);
	void (*sound_freefunc)(struct soundhandle* );
	S32 (*sound_statusfunc)(struct soundhandle* );
	void (*setsfxvolumefunc)(struct soundhandle* ,float val); // 0-1
	float (*getsfxvolumefunc)(struct soundhandle* );
	void (*setsfxpanfunc)(struct soundhandle* ,float val); // 0-1
	float (*getsfxpanfunc)(struct soundhandle* );
	void (*setsfxfreqfunc)(struct soundhandle* ,float val); // 0-1
	float (*getsfxfreqfunc)(struct soundhandle* );
	void (*soundpausefunc)();
	void (*soundresumefunc)();
	struct soundhandle* (*audio_fifo_playvol)(S32 samplerate,S32 stereo,S32 sixteenbit,
		void (*callback)(U8* ,S32),float vol);
};

static struct audioapi audioapis[]={
// standard
	{1,wp_ndrv,wp_audio_init,wp_audio_uninit,wp_audio_babysit,wp_wave_load,wp_wave_unload,
		wp_wave_playvol,wp_playasoundvol,wp_sound_free,wp_sound_status,
		wp_setsfxvolume,wp_getsfxvolume,wp_setsfxpan,wp_getsfxpan,wp_setsfxfreq,wp_getsfxfreq,
		wp_soundpause,wp_soundresume,
		wp_fifo_playvol}, 
// directsound
	{MAXSOUNDS,ds_ndrv,ds_audio_init,ds_audio_uninit,ds_audio_babysit,ds_wave_load,ds_wave_unload,
		ds_wave_playvol,ds_playasoundvol,ds_sound_free,ds_sound_status,
		ds_setsfxvolume,ds_getsfxvolume,ds_setsfxpan,ds_getsfxpan,ds_setsfxfreq,ds_getsfxfreq,
		ds_soundpause,ds_soundresume,
		ds_fifo_playvol},
};

/////////////// PUBLIC API //////////////////////////////
void audio_babysit()
{
	perf_start(AUDIOBABYSIT);
	if (audioinfo.audio_maindriver>=0)
		(*audioapis[audioinfo.audio_maindriver].audio_babysitfunc)();
	perf_end(AUDIOBABYSIT);
}

static C8* audiomainstrs[]={"waveplayer","directsound"};
static S32 maxaudiomaindrivers=2;
void audio_init(S32 maindriver,S32 subdriver,S32 micsubdriver)
{
	S32 i;
	static S32 firsttime=1;
	if (firsttime) {
		audioinfo.audio_maindriver=-1;
		firsttime=0;
	}
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].audio_uninitfunc();
	audioinfo.audio_maindriver=maindriver;
	if (audioinfo.audio_maindriver>=maxaudiomaindrivers)
		errorexit("audio_init: can only support waveplayer and directsound");
	audioinfo.audio_subdriver=subdriver;
	audioinfo.audio_micsubdriver=micsubdriver;
	if (audioinfo.audio_maindriver>=0) {
		logger("audio init %d %d.............................................\n",maindriver,subdriver);
		for (i=0;i<maxaudiomaindrivers;i++) {
			if (i==maindriver)
				logger("  * ");
			else
				logger("    ");
			logger("maindriver %2d '%s'\n",i,audiomainstrs[i]);
		}
		maxdvsounds=audioapis[audioinfo.audio_maindriver].maxsounds;

		if (!(audioapis[audioinfo.audio_maindriver].audio_initfunc)()) {
			logger("directsound create fails, no sounds!\n");
			audioinfo.audio_maindriver=-1;
			//audio_init(0,0,0);
		}

	}
}

void audio_uninit()
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].audio_uninitfunc();
	audioinfo.audio_maindriver=-1;
	logger("audio_uninit\n");
}

struct wavehandle* wave_load(const C8* fn,S32 stream)
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].wave_loadfunc(fn,stream);
	return 0;
}

void wave_unload(struct wavehandle* wh)
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].wave_unloadfunc(wh);
}

struct soundhandle* wave_playvol(struct wavehandle* wh,S32 loopcount,float vol) 	// 0 loop forever, 1 just once
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].wave_playvolfunc(wh,loopcount,vol);
	return 0;
}

struct soundhandle* wave_play(struct wavehandle* wh,S32 loopcount) 	// 0 loop forever, 1 just once
{
	return wave_playvol(wh,loopcount,1);
}

void playasoundvol(struct wavehandle* wh,float vol)
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].playasoundvolfunc(wh,vol);
}

void playasound(struct wavehandle* wh)
{
	//playasoundvol(wh,.75f);
	playasoundvol(wh,1);
}

void sound_free(struct soundhandle* sh)
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].sound_freefunc(sh);
}

S32 sound_status(struct soundhandle* sh)
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].sound_statusfunc(sh);
	return 0;
}

void setsfxvolume(struct soundhandle* sh,float val) // 0-1
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].setsfxvolumefunc(sh,val);
}

float getsfxvolume(struct soundhandle* sh)
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].getsfxvolumefunc(sh);
	return 0;
}

void setsfxfreq(struct soundhandle* sh,float val) // .001 to 1000
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].setsfxfreqfunc(sh,val);
}

float getsfxfreq(struct soundhandle* sh)
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].getsfxfreqfunc(sh);
	return 0;
}

void setsfxpan(struct soundhandle* sh,float val) // -1 to 1, left to right
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].setsfxpanfunc(sh,val);
}

float getsfxpan(struct soundhandle* sh)
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].getsfxpanfunc(sh);
	return 0;
}

void sound_pause()
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].soundpausefunc();
}

void sound_resume()
{
	if (audioinfo.audio_maindriver>=0)
		audioapis[audioinfo.audio_maindriver].soundresumefunc();
}

script* audio_getnsubdrivers(S32 driver)
{
	S32 nd;
	script* sc=audio_getnmaindrivers();
	nd=sc->num();
	delete sc;
	if (driver>=0 && driver<nd)
		return audioapis[driver].audio_getnsubdrivers();
	return new script;
}

script* audio_getnmaindrivers()
{
	script* sc=new script;
	sc->addscript("wave player");
	sc->addscript("direct sound");
	return sc;
}
/*
C8** audio_getnsubdrivers(S32 driver,S32* nsubdrivers)
{
	if (driver>=0)
		return audioapis[driver].audio_getnsubdrivers(nsubdrivers);
	*nsubdrivers=0;
	return 0;
}

C8** audio_getnmaindrivers(S32* ndrivers)
{
	C8 **sc=0;
	*ndrivers=0;
	sc=addscript(sc,"wave player",ndrivers);
	sc=addscript(sc,"direct sound",ndrivers);
	return sc;
}
*/
struct soundhandle* fifo_playvol(S32 samplerate,S32 stereo,S32 sixteenbit,void (*callback)(U8*,S32),float vol)
{
	if (audioinfo.audio_maindriver>=0)
		return audioapis[audioinfo.audio_maindriver].audio_fifo_playvol(samplerate,stereo,sixteenbit,callback,vol);
	return 0;
}

struct soundhandle* fifo_play(S32 samplerate,S32 stereo,S32 sixteenbit,void (*callback)(U8*,S32))
{
	return fifo_playvol(samplerate,stereo,sixteenbit,callback,1.0f);
}

struct wavehandle* wave_build(const C8* data,S32 samplelen,S32 freq,S32 sixteenbit,S32 stereo)
{
	struct wavehandle* r;
	buildinfo.nsamples=samplelen;
	buildinfo.freq=freq;
	buildinfo.sixteenbit=sixteenbit;
	buildinfo.stereo=stereo;
	r=wave_load(data,0);
	buildinfo.freq=0;
	return r;
}

// mic support


//static WAVEFORMATEX wfx;
//static LPDIRECTSOUNDBUFFER primary;
static BOOL CALLBACK micdirectsoundenumcallback(LPGUID lpGuid,LPCSTR lpcstrDescription,PCSTR lpcstrModule,LPVOID lpContext)
{
	if (audioinfo.audio_micsubdriver==micnsubdrivers) {
		if (lpGuid) {
			micdriverguid=*lpGuid;
			miclpdriverguid=&micdriverguid;
		}
		logger("mic  * ");
	} else
		logger("mic    ");
	if (lpGuid) {
		miclastguid=*lpGuid;
		miclplastguid=&micdriverguid;
	}
	logger("subdriver %2d '%s'\n",micnsubdrivers,lpcstrDescription);
	micnsubdrivers++;
	return 1;
}

/*typedef struct {
    U32           dwSize;
    U32           dwFlags;
    U32           dwBufferBytes;
    U32           dwReserved;
    LPWAVEFORMATEX  lpwfxFormat;
} MYDSCBUFFERDESC,* MYLPDSCBUFFERDESC;*/

static void ds_mic_record(S32 samprate,S32 ster,S32 sixteen,void (*callback)(C8* ,S32),float vol)
{
	DSCCAPS micdscaps;  
	DSCBUFFERDESC micdsdesc;
//	MYDSCBUFFERDESC micdsdesc;
	WAVEFORMATEX wfxFormat,gfmt;
	HRESULT hr;
//	DSCAPS dscaps;  
	if (miclpDirectSound)
		return;
	logger(" directsound init:\n");
//	micsubdriver=subdrvr;
	micnsubdrivers=0;
	miclpdriverguid=miclplastguid=0;
	hr=DirectSoundCaptureEnumerate(micdirectsoundenumcallback,0);
	if (hr!=DS_OK)
		errorexit("micDirectSoundEnumerate %08x",hr);
	if (micnsubdrivers<=0)
		return;
	if (audioinfo.audio_micsubdriver>=micnsubdrivers) {
		audioinfo.audio_micsubdriver=micnsubdrivers-1;
		micdriverguid=miclastguid;
		miclpdriverguid=miclplastguid;
		if (!miclpdriverguid)
			errorexit("micdirectsound: 0 lpdriverguid");
	}
//		errorexit("directsound: pick a lower audiosubdriver");
	hr = DirectSoundCaptureCreate(miclpdriverguid, &miclpDirectSound, 0);
	if (hr!=DS_OK)
		errorexit("micDirectSoundCaptureCreate %08x",hr);
/*
//	hr = IDirectSound_SetCooperativeLevel(lpDirectSound,wininfo.MainWindow,DSSCL_NORMAL);
	hr = IDirectSound_SetCooperativeLevel(lpDirectSound,wininfo.MainWindow,DSSCL_PRIORITY);
//	hr = IDirectSound_SetCooperativeLevel(lpDirectSound,wininfo.MainWindow,DSSCL_EXCLUSIVE);
	if (hr!=DS_OK)
		errorexit("IDirectSound_SetCooperativeLevel %08x",hr);
	dscaps.dwSize = sizeof(DSCAPS); 
*/
	micdscaps.dwSize = sizeof(DSCCAPS); 
	hr=miclpDirectSound->GetCaps(&micdscaps);
//	hr=IDirectSoundCapture_GetCaps(miclpDirectSound,&micdscaps);
	if (hr!=DS_OK)
		errorexit("micIDirectSound_GetCaps %08x",hr);
	logger("mic caps: dwFlags %08x, dwFormats %08x, dwChannels %d\n",
		micdscaps.dwFlags,micdscaps.dwFormats,micdscaps.dwChannels);

	wfxFormat.wFormatTag=WAVE_FORMAT_PCM;
	wfxFormat.nChannels=ster+1;
	wfxFormat.wBitsPerSample=(1+sixteen)*8;
	wfxFormat.nBlockAlign=wfxFormat.nChannels*(sixteen+1);
	wfxFormat.nSamplesPerSec=samprate;
	wfxFormat.nAvgBytesPerSec=samprate*wfxFormat.nBlockAlign;
	wfxFormat.cbSize=0;
	micdsdesc.dwSize=sizeof(micdsdesc);
	micdsdesc.dwFlags=DSCBCAPS_WAVEMAPPED;
	micdsdesc.dwBufferBytes=wfxFormat.nAvgBytesPerSec*nmicsecn/nmicsecd;
	micdsdesc.dwBufferBytes+=15;
	micdsdesc.dwBufferBytes&=~15;
	micdssizehalf=micdsdesc.dwBufferBytes/2;
	micdsdesc.dwReserved=0;
    micdsdesc.lpwfxFormat=&wfxFormat;

	hr=miclpDirectSound->CreateCaptureBuffer(&micdsdesc,&micdsbuff,0);
//	hr=IDirectSoundCapture_CreateCaptureBuffer(miclpDirectSound,&micdsdesc,&micdsbuff,0);
	if (hr!=DS_OK)
		errorexit("mic create dsound capture buffer %08x",hr);

	hr=micdsbuff->GetFormat(&gfmt,sizeof(gfmt),0);
//	hr=IDirectSoundCaptureBuffer_GetFormat(micdsbuff,&gfmt,sizeof(gfmt),0);
	if (hr!=DS_OK)
		error("mic start get format capture buffer %08x",hr);
	logger("mic get format is nch %d, bitpersamp %d, blkalign %d, samppersec %d, avgbytespersec %d\n",
		gfmt.nChannels,gfmt.wBitsPerSample,gfmt.nBlockAlign,gfmt.nSamplesPerSec,gfmt.nAvgBytesPerSec);

	hr=micdsbuff->Start(DSCBSTART_LOOPING);
//	hr=IDirectSoundCaptureBuffer_Start(micdsbuff,DSCBSTART_LOOPING);
	if (hr!=DS_OK)
		error("mic start dsound capture buffer %08x",hr);
	mfifocallback=callback;
	micdshalf=micdssizehalf;
}

/*S32 testmicrdpos,testmiccappos;

void testcheckmicpos()
{
	HRESULT hr;
	if (!micdsbuff)
		return;
	hr=IDirectSoundCaptureBuffer_GetCurrentPosition(micdsbuff,&testmiccappos,&testmicrdpos);
//	if (hr==DSERR_UNSUPPORTED)
//		return;
//	if (hr!=DS_OK)
//		errorexit("mic read pos dsound capture buffer %08x",hr);
//
}
*/
static void ds_mic_free()
{
	HRESULT hr;
	if (!miclpDirectSound)
		return;
 	hr=micdsbuff->Stop();
// 	hr=IDirectSoundCaptureBuffer_Stop(micdsbuff);
//	if (hr!=DS_OK)
//		error("mic stop dsound capture buffer %08x",hr);

	if (micdsbuff) 
        hr=micdsbuff->Release();
//      hr=IDirectSoundCaptureBuffer_Release(micdsbuff);
	micdsbuff=0;
	if (hr!=DS_OK)
		errorexit("micIDirectSound buffer Release %08x",hr);
	hr=miclpDirectSound->Release();
//	hr=IDirectSoundCapture_Release(miclpDirectSound);
	miclpDirectSound=0;
	if (hr!=DS_OK)
		errorexit("micIDirectSound_Release %08x",hr);
}

static void ds_mic_setvol(float v)
{
}

static float ds_mic_getvol()
{
	return 0;
}

static void wp_mic_record(S32 samprate,S32 ster,S32 sixteen,void (*callback)(C8* ,S32),float vol)
{
	struct mywaveformatex fmt;
	S32 i,n,result;
	WAVEINCAPS wic;
	if (tmic_wh)
		return;
	n=waveInGetNumDevs();
	for (i=0;i<n;i++) {
		waveInGetDevCaps(i,&wic,sizeof(wic));
		logger("mic dev %d, formats %08x, nchan %d '%s'\n",i,wic.dwFormats,wic.wChannels,wic.szPname);
	}
	if (n<=0)
		return;
	if (audioinfo.audio_micsubdriver>=n)
		audioinfo.audio_micsubdriver=n-1;
	memset(&tmic_whdrs[0],0,sizeof(tmic_whdrs[0]));
	memset(&tmic_whdrs[1],0,sizeof(tmic_whdrs[1]));
	fmt.wFormatTag=WAVE_FORMAT_PCM;
	fmt.nChannels=ster+1;
	fmt.wBitsPerSample=(1+sixteen)*8;
	fmt.nBlockAlign=fmt.nChannels*(sixteen+1);
	fmt.nSamplesPerSec=samprate;
	fmt.nAvgBytesPerSec=samprate*fmt.nBlockAlign;
	fmt.cbSize=sizeof(fmt);
//	result=waveInOpen(&tmic_wh,WAVE_MAPPER,(LPWAVEFORMATEX)&fmt,0,0,0); 
	result=waveInOpen(&tmic_wh,audioinfo.audio_micsubdriver,(WAVEFORMATEX*)&fmt,0,0,0);
	if (result!=MMSYSERR_NOERROR)
		return;
//	result=waveInOpen(&tmic_wh,1,(LPWAVEFORMATEX)&fmt,0,0,0); 
	tmic_whdrs[0].dwBufferLength=fmt.nAvgBytesPerSec*nmicsecn/nmicsecd;
	tmic_whdrs[0].dwBufferLength+=15;
	tmic_whdrs[0].dwBufferLength&=~15; // round up to a multiple of 16 bytes..
	tmic_whdrs[0].dwBufferLength/=2;
//	tmic_whdrs[0].lpData=(C8*)memalloc(tmic_whdrs[0].dwBufferLength);
	tmic_whdrs[0].lpData=new C8[tmic_whdrs[0].dwBufferLength];
	tmic_whdrs[1].dwBufferLength=tmic_whdrs[0].dwBufferLength;
//	tmic_whdrs[1].lpData=(C8*)memalloc(tmic_whdrs[0].dwBufferLength);
	tmic_whdrs[1].lpData=new C8[tmic_whdrs[0].dwBufferLength];
	mfifocallback=callback;
	result=waveInPrepareHeader(tmic_wh,&tmic_whdrs[0],sizeof(tmic_whdrs[0]));
	result=waveInPrepareHeader(tmic_wh,&tmic_whdrs[1],sizeof(tmic_whdrs[1]));
	result=waveInAddBuffer(tmic_wh,&tmic_whdrs[0],sizeof(tmic_whdrs[0])); 
	result=waveInAddBuffer(tmic_wh,&tmic_whdrs[1],sizeof(tmic_whdrs[1])); 
	result=waveInStart(tmic_wh);
}

static void wp_mic_free()
{
//	extern struct con16* testcn;
//	struct bigtime bt;
	S32 result;
	if (!tmic_wh)
		return;
	
//	getrawtime(&bt);
//	con16_printf(testcn,"begin waveinstop %08x:%08x\n",bt.hi,bt.lo);
	result=waveInStop(tmic_wh);
	
	if ((tmic_whdrs[0].dwFlags&WHDR_DONE) && tmic_whdrs[0].dwBytesRecorded>0) {
		result=waveInUnprepareHeader(
			(HWAVEIN)tmic_wh,(WAVEHDR* )&tmic_whdrs[0],sizeof(WAVEHDR));
		(mfifocallback)(tmic_whdrs[0].lpData,tmic_whdrs[0].dwBytesRecorded);
	}
	if ((tmic_whdrs[1].dwFlags&WHDR_DONE) && tmic_whdrs[1].dwBytesRecorded>0) {
		result=waveInUnprepareHeader(
			(HWAVEIN)tmic_wh,(WAVEHDR* )&tmic_whdrs[1],sizeof(WAVEHDR));
		(mfifocallback)(tmic_whdrs[1].lpData,tmic_whdrs[1].dwBytesRecorded);
	}
//	getrawtime(&bt);
//	con16_printf(testcn,"begin waveinrest %08x:%08x\n",bt.hi,bt.lo);
	result=waveInReset(tmic_wh);
	
//	getrawtime(&bt);
//	con16_printf(testcn,"begin waveinup 0 %08x:%08x\n",bt.hi,bt.lo);
	result=waveInUnprepareHeader(tmic_wh,&tmic_whdrs[0],sizeof(tmic_whdrs[0]));
	
//	getrawtime(&bt);
//	con16_printf(testcn,"begin waveinup 1 %08x:%08x\n",bt.hi,bt.lo);
	result=waveInUnprepareHeader(tmic_wh,&tmic_whdrs[1],sizeof(tmic_whdrs[1]));

//	getrawtime(&bt);
//	con16_printf(testcn,"begin waveinfree %08x:%08x\n",bt.hi,bt.lo);
//	memfree(tmic_whdrs[0].lpData);
	delete[] tmic_whdrs[0].lpData;
//	memfree(tmic_whdrs[1].lpData);
	delete[] tmic_whdrs[1].lpData;

//	getrawtime(&bt);
//	con16_printf(testcn,"begin waveinclse %08x:%08x\n",bt.hi,bt.lo);
	result=waveInClose(tmic_wh);

//	getrawtime(&bt);
//	con16_printf(testcn,"end waveinclose  %08x:%08x\n",bt.hi,bt.lo);
	tmic_wh=0;
}

static void wp_mic_setvol(float v)
{
	if (!tmic_wh)
		return;
}

static float wp_mic_getvol()
{
	if (!tmic_wh)
		return 0;
	return 0;
}

struct micapi {
	script* (*mic_getnsubdriversfunc)();
	void (*mic_recordfunc)(S32 samplerate,S32 stereo,S32 sixteenbit,
		void (*callback)(C8* ,S32),float vol);
	void (*mic_setvolfunc)(float val); // 0-1
	float (*mic_getvolfunc)();
	void (*mic_freefunc)();
};

static struct micapi micapis[]={
	{wp_mic_ndrv,wp_mic_record,wp_mic_setvol,wp_mic_getvol,wp_mic_free},
	{ds_mic_ndrv,ds_mic_record,ds_mic_setvol,ds_mic_getvol,ds_mic_free},
};

script* mic_getnsubdrivers(S32 driver)
{
	S32 ndrivers;
	script* sc=audio_getnmaindrivers();
	ndrivers=sc->num();
	delete sc;
	if (driver>=0 && driver<ndrivers)
		return micapis[driver].mic_getnsubdriversfunc();
	return new script;
//	*nsubdrivers=0;
//	return 0;
}

/*C8** mic_getnsubdrivers(S32 driver,S32* nsubdrivers)
{
	if (driver>=0)
		return micapis[driver].mic_getnsubdriversfunc(nsubdrivers);
	*nsubdrivers=0;
	return 0;
}
*/
void mic_record(S32 samprate,S32 ster,S32 sixteen,void (*callback)(C8*,S32),float vol)
{
	if (audioinfo.audio_maindriver>=0)
		micapis[audioinfo.audio_maindriver].mic_recordfunc(samprate,ster,sixteen,callback,vol);
}

void mic_free()
{
	if (audioinfo.audio_maindriver>=0)
		micapis[audioinfo.audio_maindriver].mic_freefunc();
}

void mic_setvol(float v)
{
	if (audioinfo.audio_maindriver>=0)
		micapis[audioinfo.audio_maindriver].mic_setvolfunc(v);
}

float mic_getvol()
{
	if (audioinfo.audio_maindriver>=0)
		return micapis[audioinfo.audio_maindriver].mic_getvolfunc();
	return 0;
}

