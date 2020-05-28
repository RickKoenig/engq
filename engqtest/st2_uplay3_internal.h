namespace st2 {

	int st2_line2road(VEC *top, VEC *bot, VEC *bestintsect, VEC *bestnorm);
	void st2_callsurffunc();
	struct animtex {
		int curframe, lastframe;
		int nframes;
		int w, h;
		MAT *mat;
		unsigned short *texdata[32];
		unsigned short *desttex;
	};

	#define COLBONESX 2
	#define COLBONESZ 3
	struct dembone {
		char *name;
		VEC dampushin;
		TREE *t;
		VEC curpushin;
	};

	extern struct dembone dembones[COLBONESX][COLBONESZ];

}

void uplay3_init();
void uplay3_proc();
void uplay3_exit();

namespace st2 {
	extern int uup;
	extern int udown;
	extern int uleft;
	extern int uright;
	extern int urollleft;
	extern int urollright;
	extern int uready;

	extern int flymode;
	extern int startstunt;

	extern float wheelaccel;

	void playatagsound(int tagidx);
	//void getgridxz(COLLGRID *gc, VEC *v, int *x, int *z);
	void docondom(int numcondoms);
	void douburst(int numbursts);
}