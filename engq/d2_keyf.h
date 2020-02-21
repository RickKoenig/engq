void tcbcalcallcoeffs(struct keyframe *kf);
void bezcalcallcoeffs(struct keyframe *kf);
//float tcbcalckeyvalue(struct keyframe *kf,float time);
//void quatcalckeyvalue(struct keyframe *kfx,struct keyframe *kfy,struct keyframe *kfz,struct keyframe *kfw,
//					   float time,struct pointf3 *rq);
//void tcbkeyframeproc(class tree2 *t);
//void quatkeyframeproc(class tree2 *t);
//extern struct keyframe deftestkeyframe;
//struct key deftestkeys[];

void seq_dostep(class tree2*);
void seq_start(class tree2 *);
void seq_stop(class tree2 *);
void seq_setseq(class tree2 *,int seq);
void seq_setframe(class tree2 *,float frame);
//void seq_mergekeyframes(class tree2 *s,class tree2 *m);
//void seq_setmorph(class tree2 *,float morph);

int seq_getseq(class tree2 *t);
int seq_isplaying(class tree2 *t);
float seq_getframe(class tree2 *t);
void seq_setrepcount(class tree2 *t,int rep);

// some globals, for test override
struct kt {
	int manualframe; // 0
	float framenum;  // 0
	float framestep; // 1
};

extern struct kt keytester;
