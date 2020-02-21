enum {VIDEO_GDI,VIDEO_DDRAW,VIDEO_D3D_WND,VIDEO_D3D_FS,VIDEO_NSUPPORTED_DEVICES};
#define MAXXYRES 50
struct vi {
	S32 wx,wy;
	S32 sx,sy;
	bitmap32 b32;
	S32 video_subdriver;
	S32 video_maindriver;
	S32 video_fullscreen; // set by driver
	S32 gamexyidx;
	S32 ngamexy;
	pointi2 gamexy[MAXXYRES];
//	bool didcursor;
	S32 oscursor,showcursor;
//	bool didbuildworldmats;
	script* mat2shader;
	S32 texreduce;	// 0 none, 1 1/2 by 1/2, 2 1/4 by 1/4 etc. (non cubemap only)
};
extern vi videoinfo;

extern bitmap32* lock32;
extern bitmap32 dummy32;

#define WX (videoinfo.wx)
#define WY (videoinfo.wy)
#define SX (videoinfo.sx)
#define SY (videoinfo.sy)
#define B32 (&(videoinfo.b32))

class script* video_getnmaindrivers();
class script* video_getnsubdrivers(S32 driver);

void video_init(S32 driver,S32 subdriver);
void video_setupwindow(S32 x,S32 y);//,U32 bpp);
void video_uninit();

void video_paintwindow(U32 paintwindow); // normally 0

void video_lock();
void video_unlock();

void figureoutwindowpos(S32 fs);
void figureoutwindowposxywh(S32 fs,S32* stx,S32* sty,S32* wid,S32* hit);

struct fogparms {
// turn fog off = 0, on = 1
	bool enable;
// distance
	float start,end; // linear
	float density; // exp and exp2
// technique: none = 0, exp = 1, exp2 = 2, linear = 3
	S32 mode;
// color
	pointf3 color;
};

void video_reload_mat2shader(const C8* fname=0);
