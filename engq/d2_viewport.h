// vp flags
#define VP_CLEARBG 1
#define VP_CLEARWB 2
#define VP_CHECKER 4

//S32 video_drawmodel(tree2* m,viewport2* v,mat4* o2c);//,float dissolve,struct mat4 *envmat,float zoff);

// captures a 16 bit screen image to a .tga file in capture data folder (assumes video is unlocked)
//void testcapture(); 
//void testcapturefrm(S32 bn);

struct viewport2 {
// bitmaps
//	struct bitmap16 *backdrop; // where to draw
//	S32 backdroptype;			// format of drawing surface
//	struct bitmap16 *wbuffer; // wbuffer for draw
//	mat4 w2c;
	C32 backcolor;
// clipping
	float zfront,zback;
	S32 xstart,ystart; // used in projection
	S32 xres,yres; // used in projection
	S32 xsrc,ysrc;
// matrix
//	struct mat4 w2c; // derived from camattach,roottree,trans,rot,scale,wbscale,zoom,xsrc,ysrc,xres,yres
//	struct tree* roottree;
	pointf3 camtrans,camrot;
	mat4 v2w;
	bool usev2w;
//	float wbscale;
	float camzoom;
// flags
	S32 flags;
// for ortho
	bool isortho;
	float ortho_size;
	tree2* camattach;
	bool useattachcam;
//	tree2* lookat;
};
extern viewport2 mainvp,lightvp;//,*curvp;

//void video_getgdi();
void doflycam(viewport2* vp);
