//struct sagger *play_sagger(char *sagname);
//void free_sagger(struct sagger *sag);

//unsigned char *vagload(char *name,S32 *sbufflen,S32 *freq,S32 *sixteenbit,S32 *stereo);
struct wavehandle *vag_load(const C8* wavename);
