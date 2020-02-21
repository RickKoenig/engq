script* audio_getnmaindrivers();
script* audio_getnsubdrivers(S32 driver);
//C8** audio_getnmaindrivers(S32* nmaindrivers);
//C8** audio_getnsubdrivers(S32 driver,S32* nsubdrivers);
////////// end public interface ////////////////

struct mywaveformatex {
    U16        wFormatTag;         /* format type */
    U16        nChannels;          /* number of channels (i.e. mono, stereo...) */
    U32         nSamplesPerSec;     /* sample rate */
    U32         nAvgBytesPerSec;    /* for buffer estimation */
    U16        nBlockAlign;        /* block size of data */
    U16        wBitsPerSample;     /* number of bits per sample of mono data */
    U16        cbSize;             /* the count in bytes of the size of */
				    /* extra information (after cbSize) */
};

/* wave data block header */
struct mywaveheader {
    U8*			lpData;                /* pointer to locked data buffer */
    U32	dwBufferLength;         /* length of data buffer */
    U32    dwBytesRecorded;        /* used for input only */
    U32    dwUser;                 /* for client's use */
    U32    dwFlags;                /* assorted flags (see defines) */
    U32    dwLoops;                /* loop control counter */
    U32	lpNext;					/* reserved for driver */
    U32    reserved;               /* reserved for driver */
};

struct wavehandle {
//	U32 whandle;
	struct mywaveheader whdrw;
	struct mywaveformatex format_info;
//	string name;
//	S32 stream;
//	S32 id;		// a unique id for each handle..
	FILE* fpstream; // FILE *
	S32 streamend; // how many data bytes
	S32 streamdataoffset; // start of data in file
	S32 streambufferhalf; // how big half of a streaming buffer is
	S32 streamstartwindow[2]; // 0, 1/2
	S32 streamendwindow[2]; // 1/4, 3/4
	S32 streamplaying; // if wave is being used for a stream now
	S32 streamcurwindow; // which buffer half is being used
	S32 streamsilence,streamcurrent; // 1 if add silence to end of wave, current pos in stream
//	S32 streamstopped;
// DirectSound only
	U32 whandle; // LPDIRECTSOUNDBUFFER *
// streaming
// streaming dsound only
};

//struct midihandle {
//	S32	dum;
//};

#define MAXSOUNDS 64
struct ai {
	S32 audio_maindriver;
	S32 audio_subdriver;
	S32 audio_micsubdriver;
	S32 streamaudiopos;
	S32 disablewpsetvol; // some machines show an annoying green bar when changing volume with waveplayer
};
extern struct ai audioinfo;

//extern S32 maxdvsounds;
struct soundhandle {
	U32 shandle; // LPDIRECTSOUNDBUFFER or HWAVEOUT
	struct wavehandle *wh;
	struct mywaveheader whdrs[2];
	S32 isplayasound; // need babysit to stop and free sound
	float pan;
	float freq;
// DirectSound only
	float volume;	//0 to 1
	S32 basefreq;
//	S32 stream;
//	S32 flags;
//	S32 id;
//	struct struct mywaveformatex format_info;
//	struct mywaveheader whdr;
//	S32 id;		// a unique id for each handle..
	S32 loopcnt;
	S32 fifosize;
	S32 fifohalf;
	void (*fifocallback)(U8*,S32);
};

/////////// public interface ///////////////////
// call before using streaming audio, don't call while using
void setstreamsec(S32 sec);
void setfifosec(S32 secn,S32 secd);
void setmicsec (S32 secn,S32 secd);

#define WOPT_STREAM 1
#define WOPT_FILEPTR 2
//#define WOPT_FIFO 4
struct wavehandle *wave_load(const C8* wavename,S32 stream);
void wave_unload(struct wavehandle *);
struct wavehandle *wave_build(const C8 *data,S32 samplelen,S32 freq,S32 sixteenbit,S32 stereo);

struct soundhandle *wave_play(struct wavehandle *,S32 loopcount);	// 0 loop forever, 1 just once
struct soundhandle *wave_playvol(struct wavehandle *,S32 loopcount,float vol);	// 0 loop forever, 1 just once

struct soundhandle *fifo_play(S32 samplerate,S32 stereo,S32 sixteenbit,void (*callback)(U8*,S32));
struct soundhandle *fifo_playvol(S32 samplerate,S32 stereo,S32 sixteenbit,void (*callback)(U8*,S32),float vol);

void playasound(struct wavehandle *);
void playasoundvol(struct wavehandle *,float vol); // vol is 0 to 1
void sound_free(struct soundhandle *);
S32 sound_status(struct soundhandle *);

// if soundhandle == 0 then effect all waves volume
void setsfxvolume(struct soundhandle *,float val); // 0-1 normal 1
float getsfxvolume(struct soundhandle *);

void setsfxfreq(struct soundhandle *,float val); // .001 to 1000
float getsfxfreq(struct soundhandle *);

void setsfxpan(struct soundhandle *,float val); // -1 to 1, left to right
float getsfxpan(struct soundhandle *);

void sound_pause();
void sound_resume();

// internal
#define AUDIO_MAINDRIVER_NONE -1
#define AUDIO_MAINDRIVER_WAVEPLAYER 0
#define AUDIO_MAINDRIVER_DIRECTSOUND 1
void audio_init(S32 driver,S32 subdriver,S32 micsubdriver);
void audio_uninit();
void audio_babysit();
extern struct soundhandle soundhandles[MAXSOUNDS];

// mic support
script* mic_getnsubdrivers(S32 driver);
//C8** mic_getnsubdrivers(S32 driver,S32 *nsubdrivers);
void mic_record(S32 samprate,S32 ster,S32 sixteen,void (*callback)(U8* ,S32),float vol);
void mic_free();
void mic_setvol(float);
float mic_getvol();

// some globals
//extern S32 streamaudiopos;
//extern S32 disablewpsetvol;

// handy classes

class backsndplayer
{
	const script sc;
	S32 curtrack;
	enum st {st_none,st_midi,st_wave};
	st sst;
	vector<wavehandle*> wha;
	soundhandle* sh;
public:
	backsndplayer(const script sca) : sc(sca),curtrack(-1),sst(st_none),sh(0)
	{
		S32 i;
		logger("backsndplayer created:\n");
		logger_indent();
		pushandsetdir("audio/backsnd");
		for (i=0;i<sc.num();++i) {
			logger("track: '%s'\n",sc.idx(i).c_str());
			C8 ext[200];
			mgetext(sc.idx(i).c_str(),ext,200);
			if (!my_stricmp(ext,"wav")) {
				wavehandle* wh=wave_load(sc.idx(i).c_str(),0);
				wha.push_back(wh);
			} else {
				wha.push_back(0);
			}
		}
		popdir();
		logger_unindent();
	}
	~backsndplayer()
	{
		if (st_midi)
			midi_unload();
		S32 i,n=wha.size();
		for (i=0;i<n;++i)
			if (wha[i])
				wave_unload(wha[i]); // automatically invalidates frees sh
	}
	void settrack(S32 t)
	{
		if (!sc.num())
			return;
		if (t>=0)
			t%=sc.num();
// change track
		if (curtrack==t)
			return;
		switch(sst) {
		case st_none:
			break;
		case st_midi:
			midi_unload();
			break;
		case st_wave:
			if (sh) {
				sound_free(sh);
				sh=0;
			}
			break;
		}
		sst=st_none;
		curtrack=t;
		if (t<0)
			return;
		C8 ext[200];
		mgetext(sc.idx(curtrack).c_str(),ext,200);
		logger("changing to track '%s'\n",sc.idx(curtrack).c_str());
		logger("ext is '%s'\n",ext);
		if (!my_stricmp(ext,"mid")) {
			pushandsetdir("audio/backsnd");
			midi_load(sc.idx(curtrack).c_str());
			midi_play(1,0);
			popdir();
			sst=st_midi;
		} else if (wha[curtrack]) {
			sh=wave_play(wha[curtrack],0);
			sst=st_wave;
		}
	}
	S32 gettrack() { return curtrack; }
	S32 getnumtracks() { return sc.num(); }
};

class waveslotplayer {
	vector<wavehandle*> wha;
public:
	waveslotplayer(const script sca)
	{
		S32 i;
		logger("waveslotplayer created:\n");
		logger_indent();
		for (i=0;i<sca.num();++i) {
			logger("track: '%s'\n",sca.idx(i).c_str());
			C8 ext[200];
			mgetext(sca.idx(i).c_str(),ext,200);
			wavehandle* wh;
			if (!my_stricmp(ext,"wav"))
				wh=wave_load(sca.idx(i).c_str(),0);
			else if (!my_stricmp(ext,"vag"))
				wh=vag_load(sca.idx(i).c_str());
			else
				wh=0;
			wha.push_back(wh);
		}
		logger_unindent();
	}
	~waveslotplayer()
	{
		S32 i,n=wha.size();
		for (i=0;i<n;++i)
			if (wha[i])
				wave_unload(wha[i]); // automatically invalidates frees sh
	}
	void settrack(S32 t)
	{
		if (t>=(S32)wha.size())
			return;
		playasoundvol(wha[t],.125f);
	}
	void settrack(S32 t,float vol)
	{
		if (t>=(S32)wha.size())
			return;
		playasoundvol(wha[t],vol);
	}
	S32 getnumtracks() { return wha.size(); }
	wavehandle* getwh(S32 t)
	{
		if (t>=(S32)wha.size())
			return 0;
		return wha[t];
	}
};

