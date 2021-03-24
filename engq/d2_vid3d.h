#define SHADOWMAP
#define SHADOWMAP_SIZE 2048

#define TEX32NCK 0
#define TEX32CK 1
#define NUMTEXFORMATS 2

struct vi3d {
	bitmap32* wbuffer3d;
//	float clearzbuffval; // 0 to 1
	S32 perscorrect; // now only for software
	S32 zenable; //	0 no, 1 yes, 2 from model
	S32 favorshading; // software only
	S32 favorlightmap; // for software too maybe
//	S32 texavail[NUMTEXFORMATS];
	S32 maxtexlogu,maxtexlogv; // currently for software only
	enum clipresult bboxclipstatus;
	float clipdot;
	float zbuffmin,zbuffmax;
	struct mat4 v2c;
	struct mat4 w2v;
	struct mat4 w2c;
	struct mat4 w2c_sky;
// shadowmap
	struct mat4 w2lc;
	bool inshadowdraw;
	bitmap32* sysfont,*sysfont2;
	softfont* smallfont,*mediumfont,*largefont;
	bool canshadow;
	bool cantoon;
	bool is3dhardware;
};
extern vi3d video3dinfo;
#define SMALLFONT video3dinfo.smallfont
#define MEDIUMFONT video3dinfo.mediumfont
#define LARGEFONT video3dinfo.largefont

// called by user
void logrc(); // shows all refcounted resource currenly alloced

// called by engine
void d2_3d_init();
void d2_3d_uninit();

void calcviewportmats(viewport2* vp,bool dohalfpixel); // for consistent (networked?) 

//void video_beginscene(viewport2* vp,bool first);
//void video_drawscene(class tree2* tree);
//void video_endscene(bool last);

void video_beginscene(); // no need to call anymore
void video_setviewport(viewport2* vp);
void video_drawscene(class tree2* tree);
void video_endscene(bool dotextdump); // no need to call anymore

bool video_rendertargetshadowmap(); // returns true if supported, switch to alt render target
void video_rendertargetmain(); // switch back to main render target

modelb* model_create(const C8* namea);
S32 model_getrc(modelb* m); // return reference count, if 1 then build model
// destroy with 	modelb::rc.deleterc(modelb*);

textureb* texture_create(const C8* namea);
S32 texture_getrc(textureb* tex); // return reference count, if 1 then build texture
// destroy with 	textureb::rc.deleterc(textureb*);

void make_envv2tex(const C8* texname);
textureb* get_envv2tex();
void free_envv2tex();
