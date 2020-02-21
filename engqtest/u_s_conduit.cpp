#define INCLUDE_WINDOWS
// simulate a shader
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace conduit {
#endif
	
// manifest of cubemaps
const C8* cubemapfilelist[] = {
	"cube02",
	"cubemap_mountains.jpg",
	"cube2.jpg",
	"cubicmap.jpg",
	"cube.jpg",
	"footballfield",
	"fishpond",
	"skansen",
	"skansen4",
};
const S32 ncubemaplist = NUMELEMENTS(cubemapfilelist);
S32 curcube = 0; // start here

bitmap32* smallbm;

// time
bool pausetime;
U32 curframe;

#define STUDY // always make frames, but do not read or write them
//#define NORMAL // read frame and play what we have, but make more frames if necessary
//#define PLAY // just play what we have, do not make more frames

#ifdef STUDY
const bool DOREAD = false;
const bool DOBUILD = true;
const bool DOWRITE = false;
#endif

#ifdef NORMAL
const bool DOREAD = true;
const bool DOBUILD = true;
const bool DOWRITE = true;
#endif

#ifdef PLAY
const bool DOREAD = true;
const bool DOBUILD = false;
const bool DOWRITE = false;
#endif

#define LORES
//#define MEDRES
//#define HIRES

#ifdef LORES
const S32 divi = 8;
const S32 resx = 80;
const S32 resy = 60;
const C8* framename = "low";
#endif

#ifdef MEDRES
const S32 divi = 2;
const S32 resx = 320;
const S32 resy = 240; // multiple of divi
const C8* framename = "med";
#endif

#ifdef HIRES
const S32 divi = 1;
const S32 resx = 800;
const S32 resy = 452;
//const C8* framename = "test";
const C8* framename = "frame";
#endif

// an instance of cubemap class
cubemap* cm;

// load a pre-existing .png into smallbm
bool readaframe()
{
	C8 fname[MAX_PATH];
	sprintf(fname,"%s%d.png",framename,curframe);
	logger("fname = '%s'\n",fname);
	pushandsetdirdown("conduit");
	if (fileexist(fname)) {
		bitmap32free(smallbm);
		smallbm = gfxread32(fname);
		popdir();
		//if (!pausetime)
		//	++curframe;
		return true;
	}
	popdir();
	return false;
}

U32 float2intcol(float v)
{
	if (v < 0.0f)
		v = 0.0f;
	else if (v >= 1.0f)
		v = 0.9999f;
	return (S32) (v * 256.0f);
}

pointf3 makeAnS(float tm)
{
//#define S sin(p.z*.2)*vec4(1,2,0,0)
	pointf3 ret;
	float sintm = sinf(tm*.2f);
	ret.x = 1.0f * sintm;
	ret.y = 2.0f * sintm;
	ret.z = 0.0f;
	ret.w = 0.0f;
	return ret;
}

// assume b > 0
float shadmod(float a,float b)
{
	if (a >= 0.0f) {
		return fmodf(a,b);
	} else {
		return fmodf(a,b) + b;
	}
}

// build a new frame at curframe into smallbm
void buildaframe()
{
	float time = curframe * 1.0f/wininfo.fpswanted;
	logger("time = %f\n",time);
	S32 sresx = smallbm->size.x;
	S32 sresy = smallbm->size.y;
	pointf2 res = {(float)sresx,(float)sresy};
	C32* outptr = smallbm->data;
	// calc shader varying
	S32 x,y;
	pointf2 frag;

	for (y=0;y<sresy;++y) {
		frag.y = res.y - y;
		pointf3 p;
		for (x=0;x<sresx;++x) {
			frag.x = (float)x;
	// ---------- start the shader ------------
    // vec4 p = vec4(fragCoord,0.,1.)/iResolution.xyxy-.5;
			p.x = frag.x/res.x - .5f;
			p.y = frag.y/res.y - .5f;
			p.z =              - .5f;
			p.w =   1.0f/res.y - .5f;
    // vec4 d = p;
			pointf3 d = p;
    // p.z += iGlobalTime*2.5;
			p.z += time * 2.5f;
			pointf3 S;
#if 1
	// move camera around
			S = makeAnS(p.z);
    //p -= S;
			p.x -= S.x;
			p.y -= S.y;
#endif
	// float is;
			float is = 0.0f;
    // vec4 t;
			pointf3 t;
    // for(float i=2. ; i>0. ; i-=.01) {
			float i;
			for (i=2.0f;i>0.0f;i-=.01f) {
    //    t = abs(mod(p + S,8.) - 4.);
				const float modc = 8.0f;
				const float modo = 4.0f;
#if 1
				S = makeAnS(p.z);
#else
				S = pointf3x(0.0f,0.0f,0.0f,0.0f);
#endif
				t.x = fabsf(shadmod(p.x + S.x,modc) - modo);
				t.y = fabsf(shadmod(p.y + S.y,modc) - modo);
				t.z = fabsf(shadmod(p.z + S.z,modc) - modo);
				//t.w = fabsf(shadmod(p.w + S.w,modc) - modo);
				pointf3 tcv;
				tcv.x = t.z - 3.0f;
				tcv.y = t.x - 3.0f;
				tcv.z = t.y - 3.0f;
    //    float isect = min(t.x, t.y);// - textureCube(iChannel0,t.zxy-3.).x; 
				float isect = min(t.x,t.y);
#define DOCUBE
#ifdef DOCUBE
				pointf3 tcf = cm->textureCubeFloat(tcv);
				isect -= tcf.x;
#endif
    //    if (isect < .01) {
				if (isect < .01f) {
    //        is = i;
					is = i;
    //        break;
					break;
    //    }
				}
    //    p -= d*isect*.2;
				float sr = .2f;
				p.x -= d.x*isect*sr;
				p.y -= d.y*isect*sr;
				p.z -= d.z*isect*sr;
				p.w -= d.w*isect*sr;

			}
     // fragColor.xyz = p.wwx*.02 + is/p.w;
#if 1
			float div = is/p.w;
			float px = p.x*.02f + div;
			float pw = p.w*.02f + div;
			pointf3 fragcol;
			fragcol.x = pw;
			fragcol.y = pw;
			fragcol.z = px;
#else
			float div = is/p.w;
			float px = p.x*.02f;
			float pw = p.w*.02f;
			pointf3 fragcol;
			fragcol.x = div;
			fragcol.y = px;
			fragcol.z = pw;
#endif
			// write color out
			C32 outcol;
			outcol.r = float2intcol(fragcol.x);
			outcol.g = float2intcol(fragcol.y);
			outcol.b = float2intcol(fragcol.z);
			outcol.a = 0xff;
	// ---------- finish the shader ------------
			*outptr = outcol;
			++outptr;
		}
	}
	// timestamp
	outtextxyf32(smallbm,0,0,C32WHITE,"%f",time);
}

void writeaframe()
{
	pushandsetdirdown("conduit");
	C8 fname[MAX_PATH];
	sprintf(fname,"%s%d.png",framename,curframe);
	gfxwrite32(fname,smallbm);
	popdir();
}

#ifdef DONAMESPACE
}
using namespace conduit;
#endif

void conduit_init()
{
	video_setupwindow(resx*divi,resy*divi);
	// keep this window on top, will get reset again when video_setupwindow gets called again
	// SetWindowPos((HWND)wininfo.MainWindow,HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	smallbm = bitmap32alloc(resx,resy,C32BLUE);
	outtextxyc32(smallbm,smallbm->size.x/2,smallbm->size.y/2,C32YELLOW,"NOTHING TO DO!");
#if 0
	logger("test float2intcol\n");
	float f;
	for (f=-.1f;f<1.1f;f+=.001f)
		logger("float = %f, int = %d\n",f,float2intcol(f));
#endif
	pushandsetdir("skybox");
	cm = new cubemap(cubemapfilelist[curcube]);
	popdir();
	curframe = 0;
}

void conduit_proc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 't':
		delete cm;
		++curcube;
		if (curcube == ncubemaplist)
			curcube = 0;
		pushandsetdir("skybox");
		cm = new cubemap(cubemapfilelist[curcube]);
		popdir();
		break;
	case 'r': // reset time
		curframe = 0;
		break;
	case 'p': // pause time
		pausetime = !pausetime;
		break;
	}
}

void conduit_draw2d()
{
	bool read = false;
	bool built = false;
	// read a png into B32 if exists and inc the curframe
	read = DOREAD && readaframe();
	// generate a frame
	if (!read && DOBUILD) {
		buildaframe();
		built = true;
	}
	// write the frame
	if (built && DOWRITE)
		writeaframe();
	// output the frame to display and optionally scale it up
	if (read || built) {
		//clipscaleblit32(smallbm,B32);
		if (!pausetime)
			++curframe;
	}
	//if (curframe == 0)
	//	outtextxy32(small
	clipscaleblit32(smallbm,B32);
}

void conduit_exit()
{
	cm->loginfo();
	delete cm;
	bitmap32free(smallbm);
}

// for reference
#if 0
// wave of conduit (how it curves)
//#define S sin(p.z*.2)*vec4(1,2,0,0)

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec4 p = vec4(fragCoord,0.,1.)/iResolution.xyxy-.5;

// change size of conduit
    p *= vec4(1.0,1.0,1.0,1.0);

    vec4 d = p;


    //p.z += iGlobalTime*2.5;

// move camera around
    //p -= S;
	float is;
    vec4 t;
    for(float i=2. ; i>0. ; i-=.01) {
        t = abs(mod(p,8.) - 4.);
     //  t = abs(mod(p + S,8.) - 4.);
        float isect = min(t.x, t.y);// - textureCube(iChannel0,t.zxy-3.).x; 
    //    float isect = t.x - textureCube(iChannel0,t.zxy-3.).x; 
        if (isect < .01) {
            is = i;
            break;
        }
        p -= d*isect*.2;
     }
     fragColor.xyz = p.wwx*.02 + is/p.w;
     //fragColor.xyz = p.xxx*.02;// + is/p.w;
     //fragColor.xyz = p.www*.02;// + is/p.w;
     //fragColor.xyz = vec3(is/p.w);
     //fragColor.w = 0.;
}
#endif