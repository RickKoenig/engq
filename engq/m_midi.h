// only works if audio_maindriver == AUDIO_MAINDRIVER_DIRECTSOUND
// otherwise does nothing
void midi_load(const C8* name);
void midi_play(float vol,S32 times); // 0 infinity
void midi_unload(); // be sure to unload before changing audio_maindriver

// some globals
extern S32 midi_disabled;
