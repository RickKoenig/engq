#include <m_eng.h>

S32 midi_disabled;

#define DOMIDI
#ifdef DOMIDI
//#define _DMUSICC_
//#include <dmplugin.h>
//#include <dsound.h>
#include <dmusici.h>

enum {MIDI_START,MIDI_LOADED,MIDI_PLAYING};

struct midihandle {
	IDirectMusicPerformance *pDMusPerformance;
	IDirectMusic *pDirectMusic;
	IDirectMusicLoader *pDMusLoader;
	IDirectMusicSegment *pDMusSegment;
	S32 midistate;
};

static struct midihandle MIDIPerf;
//extern S32 audio_maindriver;
// midi
void midi_load(const C8* name)
{
    S32 hr;
	C8 szDir[MAX_PATH];
    WCHAR wszDir[MAX_PATH],wszMidiFileName[MAX_PATH];
    DMUS_OBJECTDESC ObjDesc; 
	if (audioinfo.audio_maindriver!=AUDIO_MAINDRIVER_DIRECTSOUND || midi_disabled)
		return;
	if (MIDIPerf.midistate!=MIDI_START) {
		midi_unload();
		//return;
	}
	if (!fileexist(name))
		return;
// init COM
	hr=CoInitialize(0);
	if ((hr!=S_OK && hr!=S_FALSE)) {
//	if (S_OK!=CoInitialize(0)) {
//		errorexit("Can't CoInitialize");
		midi_disabled=1;
		return;
	}
// create performance
	if (S_OK!=CoCreateInstance(CLSID_DirectMusicPerformance,0,CLSCTX_INPROC,
			IID_IDirectMusicPerformance, (void**)&MIDIPerf.pDMusPerformance)) {
//		errorexit("Unable to create a performance object.");
		midi_disabled=1;
		return;
	}
	MIDIPerf.pDirectMusic = 0;
// init performance
	if (S_OK!=MIDIPerf.pDMusPerformance->Init(&MIDIPerf.pDirectMusic, 0, 0)) {
//		errorexit("Unable to initialize Performance.");
		midi_disabled=1;
		return;
	}
// add default port
	if (S_OK!=MIDIPerf.pDMusPerformance->AddPort(0)) {
//		errorexit("Can't add port");
		midi_disabled=1;
		return;
	}
// create a loader
	if (S_OK!=CoCreateInstance(CLSID_DirectMusicLoader,0,CLSCTX_INPROC,
				IID_IDirectMusicLoader,(void**)&MIDIPerf.pDMusLoader))
		errorexit("Unable to create a music loader object.");
// set loader search directory to current directory
	if (0==GetCurrentDirectory(MAX_PATH,szDir))
		errorexit("Unable to get current directory");
// convert to unicode
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,szDir,-1,wszDir,MAX_PATH);
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,name,-1,wszMidiFileName,MAX_PATH);
// set search directory
	if (S_OK!=MIDIPerf.pDMusLoader->SetSearchDirectory(GUID_DirectMusicAllTypes,wszDir,FALSE))
		errorexit("Unable to set search directory");
// load object
    ObjDesc.guidClass=CLSID_DirectMusicSegment;
    ObjDesc.dwSize=sizeof(DMUS_OBJECTDESC);
    wcscpy( ObjDesc.wszFileName, wszMidiFileName );
    ObjDesc.dwValidData=DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
// Now load the object and query it for the IDirectMusicSegment interface.
    if (S_OK!=MIDIPerf.pDMusLoader->GetObject(&ObjDesc,
				IID_IDirectMusicSegment2,(void**)&MIDIPerf.pDMusSegment))
		errorexit("can't load midi file");
// standard MIDI file
    if (S_OK!=MIDIPerf.pDMusSegment->SetParam(GUID_StandardMIDIFile,~0,0,0,(void*)MIDIPerf.pDMusPerformance))
		errorexit("can't set standard midi");
// download the instruments.
    if (S_OK!=MIDIPerf.pDMusSegment->SetParam(GUID_Download,~0,0,0,(void*)MIDIPerf.pDMusPerformance))
		errorexit("can't download instruments");
	MIDIPerf.midistate=MIDI_LOADED;
}

void midi_play(float vol,S32 ntimes)
{
 	if (audioinfo.audio_maindriver!=AUDIO_MAINDRIVER_DIRECTSOUND || midi_disabled)
		return;
	if (MIDIPerf.midistate!=MIDI_LOADED)
		return;
  if (ntimes==0)
	  ntimes=DMUS_SEG_REPEAT_INFINITE;
  else
	  ntimes--;
  if (S_OK!=MIDIPerf.pDMusSegment->SetRepeats(ntimes))
		errorexit("can't repeat midi");
  if (S_OK!=MIDIPerf.pDMusPerformance->PlaySegment(MIDIPerf.pDMusSegment,0,0,0))
		errorexit("can't play midi");
	MIDIPerf.midistate=MIDI_PLAYING;
}

void midi_unload()
{
 	if (audioinfo.audio_maindriver!=AUDIO_MAINDRIVER_DIRECTSOUND || midi_disabled)
		return;
	if (MIDIPerf.midistate==MIDI_START)
		return;
// If there is any music playing, stop it.
    MIDIPerf.pDMusPerformance->Stop(0,0,0,0);
// Unload instruments
    MIDIPerf.pDMusSegment->SetParam(GUID_Unload,~0,0,0,(void*)MIDIPerf.pDMusPerformance);
// Release the segment.
    MIDIPerf.pDMusSegment->Release();
// CloseDown and Release the performance object.
    MIDIPerf.pDMusPerformance->CloseDown();
    MIDIPerf.pDMusPerformance->Release();
// Release the loader object.
	MIDIPerf.pDMusLoader->Release();   
// Release COM.
	CoUninitialize();
	MIDIPerf.midistate=MIDI_START;
}

#else

void midi_load(const C8* name) {}
void midi_play(float vol,S32 ntimes) {}
void midi_unload() {}

#endif
