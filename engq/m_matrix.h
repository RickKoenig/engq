/////// matrix stuff
/// mat2 //////
struct mat2 {
	float e[2][2];
};

/// mat3 //////
struct mat3 {
	float e[3][3];
};

/// mat4 //////
struct mat4 {
	float e[4][4];
};

void logmat2(const struct mat2* m,const C8* name);
float detmat2(const struct mat2 *m);
void mulmat2(const struct mat2 *a,const struct mat2 *b,struct mat2 *p);
U32 inversemat2(const struct mat2 *m,struct mat2 *inv);
void transposemat2(const struct mat2 *m,struct mat2 *mt);
void identmat2(struct mat2 *m);
void randmat2(struct mat2 *m);
void mulmatvec2(const struct mat2 *m,const struct pointf2 *vi,struct pointf2 *vo);

void comatrix3to2(const struct mat3 *m,struct mat2 *cm,S32 r,S32 c);
float detmat3(const struct mat3 *m);
void mulmat3(const struct mat3 *a,const struct mat3 *b,struct mat3 *p);
U32 inversemat3(const struct mat3 *m,struct mat3 *inv);
void transposemat3(const struct mat3 *m,struct mat3 *mt);
void identmat3(struct mat3 *m);
void randmat3(struct mat3 *m);
void mulmatvec3(const struct mat3 *m,const struct pointf3 *vi,struct pointf3 *vo);

void logmat4(const struct mat4* m,const C8* name);
void comatrix4to3(const struct mat4 *m,struct mat3 *cm,S32 r,S32 c);
float detmat4(const struct mat4 *m);
void mulmat4(const struct mat4 *a,const struct mat4 *b,struct mat4 *p);
U32 inversemat4(const struct mat4 *m,struct mat4 *inv);
void transposemat4(const struct mat4 *m,struct mat4 *mt);
void identmat4(struct mat4 *m);
void randmat4(struct mat4 *m);
void mulmatvec4(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo);

// 3d stuff
// (3) (4by3) == (3)
void xformvec(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo);
void xformvecs(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo,S32 npoints);
//void xformvec4s(struct mat4 *m,struct pointf3 *vi,struct pointf3 *vo,S32 npoints);
void mulmat3d(const struct mat4 *a,const struct mat4 *b,struct mat4 *p);
S32 inversemat3d(const struct mat4 *m,struct mat4 *inv);
void buildscalerottrans3d(const struct pointf3 *scale,const struct pointf3 *rpy,const struct pointf3 *trans,struct mat4 *m);
void buildscalerot3d(const struct pointf3 *scale,const struct pointf3 *rpy,struct mat4 *m);
void buildrottrans3d(const struct pointf3 *rpy,const struct pointf3 *trans,struct mat4 *m);
void buildrot3d(const struct pointf3 *rpy,struct mat4 *m);
void premulscale3d(const struct pointf3 *scale,const struct mat4 *a,struct mat4 *b);
void postmulscale3d(const struct mat4 *a,const struct pointf3 *scale,struct mat4 *b);

//void registerproj(float offx,float offy,float xres,float yres,float zoff);
//void projvec(struct pointf3 *vi,struct pointf3 *vo);
//void invprojvec(struct pointf3 *vi,struct pointf3 *vo);
//void projvecsd3d(struct pointf3 *vi,struct tlnospec *vo,S32 npoints);
//void projvecd3d(struct pointf3 *vi,struct tlnospec *vo);
//void projvecs(struct pointf3 *vi,struct pointf3 *vo,S32 npoints);
//extern float proj_zoomx,proj_zoomy,proj_offsetx,proj_offsety; // for pers corr tests
//extern float mouse3doffx,mouse3doffy,mouse3doffz;

void xformdir(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo);
void xformdirinv(const struct mat4 *m,const struct pointf3 *vi,struct pointf3 *vo);

/*
/// mat n //// only sqare matrices for now nxn
struct matn {
		S32 n;
		float *e;
};
struct matn *allocmatn(S32 order);
void freematn(struct matn *m);

typedef struct mat4 JXFORM;
*/
