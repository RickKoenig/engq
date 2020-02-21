#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "n_deflatecpp.h"
#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "n_faceverts.h"
#include "n_polylist.h"
#include "n_meshtest.h"
#include "carenalobby.h"
#include "tracklist.h"
#include "utils/modelutil.h"
viewport2 constructor_viewport,constructor_viewport2;
tree2* constructor_viewport_roottree,*constructor_viewport2_roottree;
wavehandle* backsndwave;

static tracklist* tl;

static struct menuvar constructorvars[]={
	{"@lightred@---- constructor car cam -------------------",0,D_VOID,0},
	{"constructor cam trans x",&constructor_viewport.camtrans.x,D_FLOAT,FLOATUP/8},
	{"constructor cam trans y",&constructor_viewport.camtrans.y,D_FLOAT,FLOATUP/8},
	{"constructor cam trans z",&constructor_viewport.camtrans.z,D_FLOAT,FLOATUP/8},
	{"constructor cam rot x",&constructor_viewport.camrot.x,D_FLOAT,FLOATUP/8},
	{"constructor cam rot y",&constructor_viewport.camrot.y,D_FLOAT,FLOATUP/8},
	{"constructor cam rot z",&constructor_viewport.camrot.z,D_FLOAT,FLOATUP/8},
	{"constructor cam zoom",&constructor_viewport.camzoom,D_FLOAT,FLOATUP/8},
	{"@cyan@---- constructor track cam -------------------",0,D_VOID,0},
	{"track cam trans x",&constructor_viewport2.camtrans.x,D_FLOAT,FLOATUP/8},
	{"track cam trans y",&constructor_viewport2.camtrans.y,D_FLOAT,FLOATUP/8},
	{"track cam trans z",&constructor_viewport2.camtrans.z,D_FLOAT,FLOATUP/8},
	{"track cam rot x",&constructor_viewport2.camrot.x,D_FLOAT,FLOATUP/8},
	{"track cam rot y",&constructor_viewport2.camrot.y,D_FLOAT,FLOATUP/8},
	{"track cam rot z",&constructor_viewport2.camrot.z,D_FLOAT,FLOATUP/8},
	{"track cam zoom",&constructor_viewport2.camzoom,D_FLOAT,FLOATUP/8},
};
static const S32 nconstructorvars=sizeof(constructorvars)/sizeof(constructorvars[0]);

#define TRACKVIEWX 10
#define TRACKVIEWY 10
#define TRACKVIEWW 512
#define TRACKVIEWH 384
#define TRACKVIEWX2 550
#define TRACKVIEWY2 270
#define TRACKVIEWW2 240
#define TRACKVIEWH2 180
#define CURSORY 2

static text* ttrack;
static pbut* pbacksave;
static pbut* pbacknosave;
static pbut* pbackdelete;
static pbut* pcopy;
static pbut* pclear;
static text* tbasetex;
static text* tskybox;
static text* tbacksnd;
static vscroll* vbasetex;
static vscroll* vskybox;
static vscroll* vbacksnd;
static listbox* lpiecetypes;
static pbut* protateleft;
static pbut* protateright;

static edit* ename;
static pbut* enameok;
static pbut* enamecan;

static vscroll* vleftwidth[piece::nseg+1];
static vscroll* vrightwidth[piece::nseg+1];
static vscroll* vheight[piece::nseg+1];
static vscroll* vleftwidthg;
static vscroll* vrightwidthg;
static vscroll* vheightg;

static tree2* trackcursor,*trackscene,*trackpiece;

struct strackcursor {
	int x,z,valid;
};
static struct strackcursor trkcur;

static shape* rl,*rlnn;
static shape* focus;
enum mode {MODE_NORMAL,MODE_NEWNAME};
static mode consmode;

//#define STARTARROWHEIGHT .25f
#define STARTARROWHEIGHT .0625f

// 8 by 8 grid (TRACKX and TRACKY) make this total 320 by 320 square meters

static const char *tracktypenames[]={
	"blank",
	"straight",
	"turn",
	"start/finish",
	"intersection",
	"right fork",
	"left fork",
};
#define NPIECETYPES (sizeof(tracktypenames)/sizeof(tracktypenames[0]))

static int calcedgrid;
static pointf3 grid; // for mouse to 3d select

static piece cpce;
static trkt* thetrackj;
static int uniquei;
//static S32 leftwidth[piece::nseg+1]={3}; // 3 widest
//static S32 rightwidth[piece::nseg+1]; // 0 widest, rw < lw
//static S32 height[piece::nseg+1]; // 0 widest, rw < lw

static pointf3 baseverts[8]={
	{        0,0,               0},
	{PIECESIZE,0,               0},
	{        0,0,       PIECESIZE},
	{PIECESIZE,0,       PIECESIZE},
	{        0,0,  PIECESIZE/4.0f},
	{PIECESIZE,0,  PIECESIZE/4.0f},
	{        0,0,PIECESIZE*3.0f/4},
	{PIECESIZE,0,PIECESIZE*3.0f/4},
};

static uv baseuvs[4]={
	{0,1},
	{1,1},
	{0,0},
	{1,0},
};

static const char *basetexnames[]={
	"4pl_tile01.jpg",
	"grass.tga",
	"grnd02.jpg",
	"lusrocks.tga",
	"rock.tga",
	"rustiron.jpg",
	"sanddbl.tga",
	"sdirt.tga",
	"maptestnck.tga"
};
#define NBASETEXT (sizeof(basetexnames)/sizeof(basetexnames[0]))

static const char *skyboxnames[]={
	"",
	"cube.jpg",
	"cube2.jpg",
	"cubemap_mountains.jpg",
	"cubicmap.jpg",
	"footballfield",
	"fishpond",
	"skansen",
	"skansen4",
};
#define NSKYBOX (sizeof(skyboxnames)/sizeof(skyboxnames[0]))

static const char *backsndnames[]={
	"",
	"track_1.ogg",
	"track_2.ogg",
	"track_3.ogg",
	"track_4.ogg",
	"track_5.ogg",
	"track_6.ogg",
	"track_7.ogg",
	"track_8.ogg",
	"track_9.ogg",
};
#define NBACKSND (sizeof(backsndnames)/sizeof(backsndnames[0]))


static int calcgrid(struct viewport2 *vp,int mx,int my,pointf3 *g)
{
	pointf3 iv,ov;
	struct mat4 c2w;
	float k;
	if (mx<vp->xstart || my<vp->ystart || mx>=vp->xstart+vp->xres || my>=vp->ystart+vp->yres)
		return 0;
	buildrottrans3d(&vp->camrot,&vp->camtrans,&c2w);
	iv.x=(float(MX)-(vp->xstart+vp->xres*.5f))/(vp->yres*.5f); // is this right? (maybe need xsrc and ysrc)
	iv.y=(-float(MY)+(vp->ystart+vp->yres*.5f))/(vp->yres*.5f);
	iv.z=vp->camzoom;
	xformvec(&c2w,&iv,&ov);
	if (vp->camtrans.y-ov.y<EPSILON)
		return 0;
	k=(g->y-vp->camtrans.y)/(ov.y-vp->camtrans.y);
	g->x=vp->camtrans.x+k*(ov.x-vp->camtrans.x);
	g->z=vp->camtrans.z+k*(ov.z-vp->camtrans.z);
	return 1;
}

static void buildtrackcursor()
{
	int nfce;
	face* fces;
	static pointf3 cursorverts[10]={
		{               0,CURSORY,0},
		{               0,CURSORY,PIECESIZE},
		{       PIECESIZE,CURSORY,PIECESIZE},
		{       PIECESIZE,CURSORY,0},
		{               0,CURSORY,0},
		{  PIECESIZE/8.0f,CURSORY,PIECESIZE/8.0f},
		{PIECESIZE*7.0f/8,CURSORY,PIECESIZE/8.0f},
		{PIECESIZE*7.0f/8,CURSORY,PIECESIZE*7.0f/8},
		{  PIECESIZE/8.0f,CURSORY,PIECESIZE*7.0f/8},
		{  PIECESIZE/8.0f,CURSORY,PIECESIZE/8.0f},
	};
	uv uvs[10];
	int i;
	for (i=0;i<10;i++) {
		uvs[i].u=cursorverts[i].x*(1.0f/PIECESIZE);
		uvs[i].v=1-cursorverts[i].z*(1.0f/PIECESIZE);
	}
	fces=buildfaces(cursorverts,10,&nfce);
	if (!fces)
		errorexit("can't build trackcursor");
	modelb* m=model_create("trackcursor");
	m->copyverts(cursorverts,10);
	m->copyuvs0(uvs,10);
	m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_HASSHADE|SMAT_CALCLIGHTS,
			"sanddbl.tga",0,30,nfce,10);
	m->addfaces(fces,nfce,0);
	delete[] fces;
	trackcursor=new tree2("trackcursor");
	m->close();
	trackcursor->setmodel(m);
	constructor_viewport_roottree->linkchild(trackcursor);
}

static void updatecursor()
{
	calcedgrid=calcgrid(&constructor_viewport,MX,MY,&grid);
	trkcur.x=trkcur.z=0;
	trkcur.x=int(grid.x/PIECESIZE);
	trkcur.z=int(grid.z/PIECESIZE);
	if (calcedgrid && trkcur.x>=0 && trkcur.x<TRACKX && trkcur.z>=0 && trkcur.z<TRACKZ) {
		trkcur.valid=1;
		trackcursor->flags&=~TF_DONTDRAW;
		trackcursor->trans.x=(float)(trkcur.x*PIECESIZE);
		trackcursor->trans.z=(float)(trkcur.z*PIECESIZE);
	} else {
		trkcur.valid=0;
		trackcursor->flags|=TF_DONTDRAW;
	}
}

static void rotpiece(pointf3 *v,int n,int rotc90)
{
	int i;
	float t;
	switch(rotc90) {
	case 1:
		for (i=0;i<n;i++) {
			t=PIECESIZE-v[i].x;
			v[i].x=v[i].z;
			v[i].z=t;
		}
		break;
	case 2:
		for (i=0;i<n;i++) {
			v[i].x=PIECESIZE-v[i].x;
			v[i].z=PIECESIZE-v[i].z;
		}
		break;
	case 3:
		for (i=0;i<n;i++) {
			t=v[i].x;
			v[i].x=PIECESIZE-v[i].z;
			v[i].z=t;
		}
		break;
	}
}

// build models based on piece data
static tree2* buildconspiece(int bt,piece *p)
{
	static const float pieceheight = sqrtf(6.0f)/6.0f*PIECESIZE/6.0f;
pushandsetdirdown("constructed");
#define PRES 8
	pointf3 intsect;
	bool zflipf=false;
	int i,j,k;
	static float ivals[4]={0,PIECESIZE/4.0f,PIECESIZE*3/4.0f,PIECESIZE}; // for intersection
	static float sfvals[4]={0,.49f*PIECESIZE,.51f*PIECESIZE,PIECESIZE}; // for start finish
	pointf3 wverts[200]; // should be enough
	pointf3 wverts2[200]; // should be enough
	pointf3 wverts3[200]; // should be enough
	uv wuvs[200];
	tree2* ret;
	char str[50];
	sprintf(str,"pce%08x",uniquei++);
	modelb* m=model_create(str);
	S32 f1=0;S32 f2=0;S32 f3=0;S32 f4=0;
	S32 v1=0;S32 v2=0;S32 v3=0;S32 v4=0;
//	float leftside=6/8.0f;
//	float rightside=2/8.0f;
	float rightside,leftside;
	if (p->pt==PCE_LEFTFORK) {
//	if (false) {
		rightside  =.25f + (3-p->leftwidsegs[0]) /6.0f;
		leftside =.25f + (3-p->rightwidsegs[0]) /6.0f;
	} else {
		leftside  =.25f + p->leftwidsegs[0] /6.0f;
		rightside =.25f + p->rightwidsegs[0] /6.0f;
	}
	float height = float(p->heightsegs[0]);
//	pointf3 rs0={0,0,PIECESIZE/4.0f};
//	pointf3 rs1={PIECESIZE,0,PIECESIZE/4.0f};
	pointf3 rs0={			0,0,PIECESIZE*rightside};
	pointf3 rs1={	PIECESIZE,0,PIECESIZE*rightside};
	pointf3 ls0={			0,0,PIECESIZE*leftside};
	pointf3 ls1={	PIECESIZE,0,PIECESIZE*leftside};
	switch(p->pt) {
	case PCE_BLANK:
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			basetexnames[bt],0,30,2,4);
		for (i=0;i<4;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
		m->copyverts(baseverts,4);
		m->copyuvs0(baseuvs,4);
		m->addface(0,2,1);
		m->addface(1,2,3);
		break;

	case PCE_STRAIGHT:
#if 1 
		{
			k=0;
			static float upz[6]={0,.25f*PIECESIZE,5.0f/12.0f*PIECESIZE,7.0f/12.0f*PIECESIZE,.75f*PIECESIZE,PIECESIZE};
// backgnd
			for (j=0;j<6;++j) {
				for (i=0;i<7;++i) {
					wverts[k++]=pointf3x(PIECESIZE*i/6.0f,0,upz[j]);
				}
			}
			v1=k;
// track
			for (j=0;j<6;++j) {
				for (i=0;i<7;++i) {
					wverts[k++]=pointf3x(PIECESIZE*i/6.0f,float(p->heightsegs[i])*pieceheight,upz[j]);
				}
			}
			v2=k-v1;
			rotpiece(wverts,k,p->rot);
			for (i=0;i<k;i++) {
				wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
				wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
			}
// backgnd
			S32 tface=0;
#if 1
			for (i=0;i<6;++i) {
				for (j=0;j<5;++j) {
					S32 o = i+j*7;
					if (p->heightsegs[i] || p->heightsegs[i+1]) { // else no track, full backgnd
#if 1
						m->addface(o+0,o+7,o+1,false); // lower left
						m->addface(o+7,o+8,o+1,false); // upper right
//						m->addface(o+0,o+7,o+8,false); // upper left
//						m->addface(o+0,o+8,o+1,false); // lower right
						tface+=2;
#endif
					} else if (j>p->rightwidsegs[i] && j<p->leftwidsegs[i]+1 &&  // full track
					  j>p->rightwidsegs[i+1] && j<p->leftwidsegs[i+1]+1) { // no backgnd 
					} else if (j>p->rightwidsegs[i] && j<p->leftwidsegs[i]+1 && // lower left
						  j>p->rightwidsegs[i+1] /*&& j<p->leftwidsegs[i+1]+1 */) {
#if 1
//						m->addface(o+0,o+7,o+1,false); // lower left
						m->addface(o+7,o+8,o+1,false); // upper right
//						m->addface(o+0,o+7,o+8,false); // upper left
//						m->addface(o+0,o+8,o+1,false); // lower right
						tface+=1;
#endif
					} else if (j>p->rightwidsegs[i] && /*j<p->leftwidsegs[i]+1 && */ // lower right
					  j>p->rightwidsegs[i+1] && j<p->leftwidsegs[i+1]+1) {
#if 1
//						m->addface(o+0,o+7,o+1,false); // lower left
//						m->addface(o+7,o+8,o+1,false); // upper right
						m->addface(o+0,o+7,o+8,false); // upper left
//						m->addface(o+0,o+8,o+1,false); // lower right
						tface+=1;
#endif
					} else if (j>p->rightwidsegs[i] && j<p->leftwidsegs[i]+1 && // upper left
					  /*j>p->rightwidsegs[i+1] && */ j<p->leftwidsegs[i+1]+1) {
#if 1
//						m->addface(o+0,o+7,o+1,false); // lower left
//						m->addface(o+7,o+8,o+1,false); // upper right
//						m->addface(o+0,o+7,o+8,false); // upper left
						m->addface(o+0,o+8,o+1,false); // lower right
						tface+=1;
#endif
					} else if (/*j>p->rightwidsegs[i] && */ j<p->leftwidsegs[i]+1 && // upper right
					  j>p->rightwidsegs[i+1] && j<p->leftwidsegs[i+1]+1) {
#if 1
						m->addface(o+0,o+7,o+1,false); // lower left
//						m->addface(o+7,o+8,o+1,false); // upper right
//						m->addface(o+0,o+7,o+8,false); // upper left
//						m->addface(o+0,o+8,o+1,false); // lower right
						tface+=1;
#endif
					} else { // else no track, full backgnd
#if 1
						m->addface(o+0,o+7,o+1,false); // lower left
						m->addface(o+7,o+8,o+1,false); // upper right
//						m->addface(o+0,o+7,o+8,false); // upper left
//						m->addface(o+0,o+8,o+1,false); // lower right
						tface+=2;
#endif
					}
				}
			}
#endif
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,tface,v1);
// track
			tface=0;
#if 1
			for (i=0;i<6;++i) {
//				if (i!=0 && i!=5) {
					for (j=0;j<5;++j) {
//					for (j=p->rightwidsegs[i]+1;j<p->leftwidsegs[i]+1;++j) {
						S32 o = i+j*7+v1;
						if (j>p->rightwidsegs[i] && j<p->leftwidsegs[i]+1 &&  // full face
						  j>p->rightwidsegs[i+1] && j<p->leftwidsegs[i+1]+1) {
#if 1
							m->addface(o+0,o+7,o+1,true); // lower left
							m->addface(o+7,o+8,o+1,true); // upper right
//							m->addface(o+0,o+7,o+8,true); // upper left
//							m->addface(o+0,o+8,o+1,true); // lower right
							tface+=4;
#endif
						} else if (j>p->rightwidsegs[i] && j<p->leftwidsegs[i]+1 && // lower left
						  j>p->rightwidsegs[i+1] /*&& j<p->leftwidsegs[i+1]+1 */) {
#if 1
							m->addface(o+0,o+7,o+1,true); // lower left
//							m->addface(o+7,o+8,o+1,true); // upper right
//							m->addface(o+0,o+7,o+8,true); // upper left
//							m->addface(o+0,o+8,o+1,true); // lower right
							tface+=2;
#endif
						} else if (j>p->rightwidsegs[i] && /*j<p->leftwidsegs[i]+1 && */ // lower right
						  j>p->rightwidsegs[i+1] && j<p->leftwidsegs[i+1]+1) {
#if 1
//							m->addface(o+0,o+7,o+1,true); // lower left
//							m->addface(o+7,o+8,o+1,true); // upper right
//							m->addface(o+0,o+7,o+8,true); // upper left
							m->addface(o+0,o+8,o+1,true); // lower right
							tface+=2;
#endif
						} else if (j>p->rightwidsegs[i] && j<p->leftwidsegs[i]+1 && // upper left
						  /*j>p->rightwidsegs[i+1] && */ j<p->leftwidsegs[i+1]+1) {
#if 1
//							m->addface(o+0,o+7,o+1,true); // lower left
//							m->addface(o+7,o+8,o+1,true); // upper right
							m->addface(o+0,o+7,o+8,true); // upper left
//							m->addface(o+0,o+8,o+1,true); // lower right
							tface+=2;
#endif
						} else if (/*j>p->rightwidsegs[i] && */ j<p->leftwidsegs[i]+1 && // upper right
						  j>p->rightwidsegs[i+1] && j<p->leftwidsegs[i+1]+1) {
#if 1
//							m->addface(o+0,o+7,o+1,true); // lower left
							m->addface(o+7,o+8,o+1,true); // upper right
//							m->addface(o+0,o+7,o+8,true); // upper left
//							m->addface(o+0,o+8,o+1,true); // lower right
							tface+=2;
#endif
						} // else none
					}
//				}
			}
#endif
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				"4pl_tile01.jpg",0,30,tface,v2);
			m->copyverts(wverts,k);
			m->copyuvs0(wuvs,k);
		}
#else
		{
			S32 i=0;
			wverts[i++]=pointf3x(			0,0,		0);
			wverts[i++]=pointf3x(	PIECESIZE,0,		0);
			wverts[i++]=pointf3x(			0,0,PIECESIZE);
			wverts[i++]=pointf3x(	PIECESIZE,0,PIECESIZE);
			wverts[i++]=pointf3x(			0,0,rightside*PIECESIZE);
			wverts[i++]=pointf3x(	PIECESIZE,0,rightside*PIECESIZE);
			wverts[i++]=pointf3x(			0,0,leftside*PIECESIZE);
			wverts[i++]=pointf3x(	PIECESIZE,0,leftside*PIECESIZE);
			wverts[i++]=pointf3x(			0,height*PIECEHEIGHT,		0);
			wverts[i++]=pointf3x(	PIECESIZE,height*PIECEHEIGHT,		0);
			wverts[i++]=pointf3x(			0,height*PIECEHEIGHT,PIECESIZE);
			wverts[i++]=pointf3x(	PIECESIZE,height*PIECEHEIGHT,PIECESIZE);
			wverts[i++]=pointf3x(			0,height*PIECEHEIGHT,rightside*PIECESIZE);
			wverts[i++]=pointf3x(	PIECESIZE,height*PIECEHEIGHT,rightside*PIECESIZE);
			wverts[i++]=pointf3x(			0,height*PIECEHEIGHT,leftside*PIECESIZE);
			wverts[i++]=pointf3x(	PIECESIZE,height*PIECEHEIGHT,leftside*PIECESIZE);
			rotpiece(wverts,16,p->rot);
			for (i=0;i<16;i++) {
				wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
				wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
			}
			m->copyverts(wverts,16);
			m->copyuvs0(wuvs,16);
// backgnd
			if (height==0.0f) {
				m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
					basetexnames[bt],0,30,4,8);
				m->addface(0,5,1);
				m->addface(0,4,5);
				m->addface(6,3,7);
				m->addface(6,2,3);
			} else {
				m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
					basetexnames[bt],0,30,2,8);
				m->addface(0,2,1);
				m->addface(1,2,3);
			}
// road
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				"4pl_tile01.jpg",0,30,4,8);
			m->addface(4+8,7+8,5+8,true);
			m->addface(4+8,6+8,7+8,true);
		}
#endif
		break;

	case PCE_TURN:
// 4 corners
		copy(baseverts,baseverts+4,wverts);
// start of turn on bottom
		wverts[4].x=PIECESIZE*rightside;
		wverts[4].y=0;
		wverts[4].z=0;
		wverts[4+PRES].x=PIECESIZE*leftside;
		wverts[4+PRES].y=0;
		wverts[4+PRES].z=0;
// middle of turn up and to the left
		for (i=1;i<PRES-1;i++) {
			wverts[4+i].x=(PIECESIZE*rightside)*cosf(PI*.5f*float(i)/(PRES-1));
			wverts[4+i].y=0;
			wverts[4+i].z=(PIECESIZE*rightside)*sinf(PI*.5f*float(i)/(PRES-1));
			wverts[4+PRES+i].x=(PIECESIZE*leftside)*cosf(PI*.5f*float(i)/(PRES-1));
			wverts[4+PRES+i].y=0;
			wverts[4+PRES+i].z=(PIECESIZE*leftside)*sinf(PI*.5f*float(i)/(PRES-1));
		}
// end of turn at the left
		wverts[4+PRES-1].x=0;
		wverts[4+PRES-1].y=0;
		wverts[4+PRES-1].z=PIECESIZE*rightside;
		wverts[4+2*PRES-1].x=0;
		wverts[4+2*PRES-1].y=0;
		wverts[4+2*PRES-1].z=PIECESIZE*leftside;
		rotpiece(wverts,PRES*2+4,p->rot);
		for (i=0;i<PRES*2+4;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
		copy(wverts,wverts+2*PRES+4,wverts+2*PRES+4);
		copy(wuvs,wuvs+2*PRES+4,wuvs+2*PRES+4);
		for (i=2*PRES+4;i<2*(2*PRES+4);++i)
			wverts[i].y=height*pieceheight;
// register verts and uvs with the model
		m->copyverts(wverts,2*(2*PRES+4));
		m->copyuvs0(wuvs,2*(2*PRES+4));
// register faces for backgnd
// backgnd
		if (height==0.0f) {
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,PRES-1+PRES-1+2,2*PRES+4);
// build inner fan
			for (i=0;i<PRES-1;i++)
				m->addface(0,i+4+1,i+4);
// build outer fan
			for (i=0;i<PRES-1;i++)
				m->addface(3,PRES+i+4,PRES+i+4+1);
			m->addface(4+PRES,3,1);
			m->addface(4+2*PRES-1,2,3);
		} else {
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,2,2*PRES+4);
			m->addface(0,2,1);
			m->addface(1,2,3);
		}
// track
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,2*(PRES-1+PRES-1),2*PRES+4);
		for (i=0;i<PRES-1;i++) {
			m->addface(4+i+4+2*PRES,4+PRES+1+i+4+2*PRES,4+PRES+i+4+2*PRES,true);
			m->addface(4+i+4+2*PRES,4+1+i+4+2*PRES,4+PRES+1+i+4+2*PRES,true);
		}
		break;

	case PCE_INTERSECTION:
		for (j=0;j<4;j++)
			for (i=0;i<4;i++) {
				if (i==1)
					wverts[i+4*j].x=PIECESIZE*rightside;
				else if (i==2)
					wverts[i+4*j].x=PIECESIZE*leftside;
				else
					wverts[i+4*j].x=ivals[i];
				wverts[i+4*j].y=0;
				if (j==1)
					wverts[i+4*j].z=PIECESIZE*rightside;
				else if (j==2)
					wverts[i+4*j].z=PIECESIZE*leftside;
				else
					wverts[i+4*j].z=ivals[j];
			}
		rotpiece(wverts,PRES*2+4,p->rot);
		for (i=0;i<16;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
		copy(wverts,wverts+16,wverts+16);
		for (i=16;i<32;++i)
			wverts[i].y=height*pieceheight;
		copy(wuvs,wuvs+16,wuvs+16);
		m->copyverts(wverts,32);
		m->copyuvs0(wuvs,32);
// backgnd
		if (height==0.0f) {
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,8,16);
			for (j=0;j<3;j++) {
				for (i=0;i<3;i++) {
					if (i!=1 && j!=1) {
						m->addface(i+j*4,i+j*4+4+1,i+j*4+1);
						m->addface(i+j*4,i+j*4+4,i+j*4+4+1);
					}
				}
			}
		} else {
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,2,16);
			m->addface(0,12,3);
			m->addface(12,15,3);
		}
// track
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,20,16);
		for (j=0;j<3;j++) {
			for (i=0;i<3;i++) {
				if (i==1 || j==1) {
					m->addface(16+i+j*4,16+i+j*4+4+1,16+i+j*4+1,true);
					m->addface(16+i+j*4,16+i+j*4+4,i+16+j*4+4+1,true);
				}
			}
		}
		break;

	case PCE_STARTFINISH:
		for (j=0;j<4;j++) {
			for (i=0;i<4;i++) {
				wverts[i+4*j].x=sfvals[i];
				wverts[i+4*j].y=0;
				wverts[i+4*j].z=ivals[j];
			}
		}
// add arrow, 3 verts 1 face
		wverts[16].x=PIECESIZE*5/8.0f;
		wverts[16].y=STARTARROWHEIGHT;
		wverts[16].z=PIECESIZE*5/8.0f;

		wverts[17].x=PIECESIZE*7/8.0f;
		wverts[17].y=STARTARROWHEIGHT;
		wverts[17].z=PIECESIZE*4/8.0f;

		wverts[18].x=PIECESIZE*5/8.0f;
		wverts[18].y=STARTARROWHEIGHT;
		wverts[18].z=PIECESIZE*3/8.0f;
		rotpiece(wverts,19,p->rot);
		for (i=0;i<19;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
		copy(wverts,wverts+19,wverts+19);
		copy(wuvs,wuvs+19,wuvs+19);
		copy(wverts+19,wverts+38,wverts+38);
		copy(wuvs+19,wuvs+38,wuvs+38);
		for (i=19;i<38;++i) {
			wverts[i].y+=height*pieceheight;
		}
		for (i=38;i<57;++i) {
			wverts[i].y+=height*pieceheight;
		}
		m->copyverts(wverts,57);
		m->copyuvs0(wuvs,57);
// background
		if (height==0.0f) {
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,12,19);
			for (j=0;j<3;j++) {
				for (i=0;i<3;i++) {
					if (j==0 || j==2) {
						m->addface(i+j*4,i+j*4+4+1,i+j*4+1);
						m->addface(i+j*4,i+j*4+4,i+j*4+4+1);
					}
				}
			}
		} else {
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,2,19);
			m->addface(12,3,0);
			m->addface(3,12,15);
		}
// track
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,8,19);
		for (j=0;j<3;j++)
			for (i=0;i<3;i++)
				if (j==1 && i!=1) {
					m->addface(19+i+j*4,19+i+j*4+4+1,19+i+j*4+1,true);
					m->addface(19+i+j*4,19+i+j*4+4,19+i+j*4+4+1,true);
				}
// arrow and sf line
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"sanddbl.tga",0,30,5,19);
		i=1;j=1;
		m->addface(38+i+j*4,38+i+j*4+4+1,38+i+j*4+1,true);
		m->addface(38+i+j*4,38+i+j*4+4,38+i+j*4+4+1,true);
		m->addface(38+16,38+17,38+18);
		break;

	case PCE_LEFTFORK:
		zflipf=true;
	case PCE_RIGHTFORK: 
		{ // fork
	// inside turn
			wverts[0]=baseverts[0];
			k=0;
			for (i=0;i<PRES;i++) {
				if (i==0) {
					wverts[PRES-i].x=PIECESIZE*rightside;
					wverts[PRES-i].y=0;
					wverts[PRES-i].z=0;
				} else if (i==PRES-1) {
					wverts[PRES-i].x=0;
					wverts[PRES-i].y=0;
					wverts[PRES-i].z=PIECESIZE*rightside;
				} else {
					wverts[PRES-i].x=(PIECESIZE*rightside)*cosf(PI*.5f*float(i)/(PRES-1));
					wverts[PRES-i].y=0;
					wverts[PRES-i].z=(PIECESIZE*rightside)*sinf(PI*.5f*float(i)/(PRES-1));
				}
				wverts2[k++]=wverts[PRES-i];
			}
			wverts2[k++]=ls0;
			wverts2[k++]=ls1;
			wverts2[k++]=rs1;
			int nfce;
			face* fces;
// backgnd
			if (height==0.0f) {
				v1=PRES+1;		// PRES+1 verts copied
				fces=buildfaces(wverts,v1,&nfce);
				if (fces) {
					m->addfaces(fces,nfce,0,zflipf);
					delete[] fces;
					f1=nfce;
				}
				copy(wverts,wverts+v1,wverts3);
			}
		// fork part
			wverts[0]=baseverts[1];
			for (i=0;i<PRES;i++) {
				if (i==0) {
					wverts[i+1].x=PIECESIZE*leftside;
					wverts[i+1].y=0;
					wverts[i+1].z=0;
				} else if (i==PRES-1) {
					wverts[i+1].x=0;
					wverts[i+1].y=0;
					wverts[i+1].z=PIECESIZE*leftside;
				} else {
					wverts[i+1].x=(PIECESIZE*leftside)*cosf(PI*.5f*float(i)/(PRES-1));
					wverts[i+1].y=0;
					wverts[i+1].z=(PIECESIZE*leftside)*sinf(PI*.5f*float(i)/(PRES-1));
				}
				if (intersectline2d(&rs0,&rs1,&wverts[i],&wverts[i+1],&intsect,NULL,NULL)>0) {
					wverts[i+1]=intsect;
					wverts[i+2]=rs1;
					break;
				}
			}
			j=i+3;
			for (i=j-2;i>=1;i--)
				wverts2[k++]=wverts[i];
			if (height==0.0f) {
				fces=buildfaces(wverts,j,&nfce);
				if (fces) {
					m->addfaces(fces,nfce,v1,zflipf);
					delete[] fces;
					f2=nfce;
				}
				v2=j;
				copy(wverts,wverts+v2,wverts3+v1);
			}
	// straight part
			wverts[0]=baseverts[2];
			wverts[1]=baseverts[3];
			wverts[2]=ls1;
			wverts[3]=ls0;
			if (height==0.0f) {
				v3=4;
				fces=buildfaces(wverts,v3,&nfce);
				if (fces) {
					m->addfaces(fces,nfce,v1+v2,zflipf);
					delete[] fces;
					f3=nfce;
				}
				copy(wverts,wverts+v3,wverts3+v1+v2);
			}	// first material
// backgnd
			if (height!=0.0f) {
				copy(baseverts,baseverts+4,wverts);
				v3=4;
				f3=2;
				if (zflipf) {
					m->addface(0,1,2);
					m->addface(1,3,2);
				} else {
					m->addface(0,2,1);
					m->addface(1,2,3);
				}
				copy(wverts,wverts+v3,wverts3+v1+v2);
			}	// first material 
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				basetexnames[bt],0,30,f1+f2+f3,v1+v2+v3);
// track part, second material
			fces=buildfaces(wverts2,k,&nfce);
			if (fces) {
				m->addfaces(fces,nfce,v1+v2+v3,zflipf,true);
				delete[] fces;
				f4=nfce;
			}
			v4=k;
			for (i=0;i<v4;++i) {
				wverts2[i].y=height*pieceheight;
			}
			copy(wverts2,wverts2+v4,wverts3+v1+v2+v3);
// track
			m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
				"4pl_tile01.jpg",0,30,f4*2,v4);
			S32 vt=v1+v2+v3+v4;
			if (zflipf) {
				for (i=0;i<vt;i++)
					wverts3[i].z=PIECESIZE-wverts3[i].z;
			}
			rotpiece(wverts3,vt,p->rot);
			for (i=0;i<vt;i++) {
				wuvs[i].u=wverts3[i].x*(1.0f/PIECESIZE);
				wuvs[i].v=1-wverts3[i].z*(1.0f/PIECESIZE);
			}
			m->copyverts(wverts3,vt);
			m->copyuvs0(wuvs,vt);
			break;
		} // end fork
	} // end switch
	m->close(); // lock down model, bbox, hardware, etc. 
	ret=new tree2(str);
	ret->setmodel(m);
// pillars
	const float colwid = 2;
	switch(p->pt) {
	case PCE_STRAIGHT:
		{
			S32 hsm = min(min(p->heightsegs[2],p->heightsegs[3]),p->heightsegs[4]);
			if (hsm) {
				float h3 = float(hsm);
				float leftsidec  =.25f + p->leftwidsegs[3] /6.0f;
				float rightsidec =.25f + p->rightwidsegs[3] /6.0f;
				float c = .5f * PIECESIZE * (leftsidec+rightsidec);
				tree2* pil=buildprism(pointf3x(colwid,h3*pieceheight-STARTARROWHEIGHT,colwid),"sanddbl.tga","tex");
				pil->trans=pointf3x(PIECESIZE*.5f,(h3*pieceheight-STARTARROWHEIGHT)*.5f,c);
//				if (p->pt==PCE_LEFTFORK)
//					pil->trans.z = PIECESIZE - pil->trans.z;
				rotpiece(&pil->trans,1,p->rot);
				ret->linkchild(pil);
			}
		}
		break;
	case PCE_TURN:
		if (height) {
			float leftsidec  =.25f + p->leftwidsegs[0] /6.0f;
			float rightsidec =.25f + p->rightwidsegs[0] /6.0f;
			float c = .5f * PIECESIZE * (leftsidec+rightsidec);
			c *= sqrtf(2.0f)*.5f;
			tree2* pil=buildprism(pointf3x(colwid,height*pieceheight-STARTARROWHEIGHT,colwid),"sanddbl.tga","tex");
			pil->trans=pointf3x(c,(height*pieceheight-STARTARROWHEIGHT)*.5f,c);
//			if (p->pt==PCE_LEFTFORK)
//				pil->trans.z = PIECESIZE - pil->trans.z;
			rotpiece(&pil->trans,1,p->rot);
			ret->linkchild(pil);
		}
		break;
	case PCE_STARTFINISH:
		if (height) {
			tree2* pil=buildprism(pointf3x(colwid,height*pieceheight-STARTARROWHEIGHT,colwid),"sanddbl.tga","tex");
			pil->trans=pointf3x(PIECESIZE*.5f,(height*pieceheight-STARTARROWHEIGHT)*.5f,PIECESIZE*.5f);
			ret->linkchild(pil);
		}
		break;
	case PCE_INTERSECTION:
		if (height) {
			float leftsidec  =.25f + p->leftwidsegs[0] /6.0f;
			float rightsidec =.25f + p->rightwidsegs[0] /6.0f;
			float c = .5f * PIECESIZE * (leftsidec+rightsidec);
//			c *= sqrtf(2.0f)*.5f;
			tree2* pil=buildprism(pointf3x(colwid,height*pieceheight-STARTARROWHEIGHT,colwid),"sanddbl.tga","tex");
			pil->trans=pointf3x(c,(height*pieceheight-STARTARROWHEIGHT)*.5f,c);
//			if (p->pt==PCE_LEFTFORK)
//				pil->trans.z = PIECESIZE - pil->trans.z;
			rotpiece(&pil->trans,1,p->rot);
			ret->linkchild(pil);
		}
		break;
	case PCE_RIGHTFORK:
	case PCE_LEFTFORK:
		if (height) {
			float leftsidec  =.25f + p->leftwidsegs[0] /6.0f;
			float rightsidec =.25f + p->rightwidsegs[0] /6.0f;
			float c = .5f * PIECESIZE * (leftsidec+rightsidec);
			tree2* pil=buildprism(pointf3x(colwid,height*pieceheight-STARTARROWHEIGHT,colwid),"sanddbl.tga","tex");
			pil->trans=pointf3x(PIECESIZE*.5f,(height*pieceheight-STARTARROWHEIGHT)*.5f,c);
//			if (p->pt==PCE_LEFTFORK)
//				pil->trans.z = PIECESIZE - pil->trans.z;
			rotpiece(&pil->trans,1,p->rot);
			ret->linkchild(pil);
		}
		break;
	}
popdir();
	return ret;
}

static void adjustpiece(tree2 *p,int i,int j)
{
	p->trans.x+=i*PIECESIZE;
	p->trans.z+=j*PIECESIZE;
}

static void updatebasetexres()
{
	tbasetex->settname(basetexnames[thetrackj->basetex]);
}

static void updateskyboxres()
{
	tskybox->settname(skyboxnames[thetrackj->skybox]);
}

static void updatebacksndres()
{
	tbacksnd->settname(backsndnames[thetrackj->backsnd]);
}

piece::piece() : pt(0),rot(0)
{
	fill(leftwidsegs,leftwidsegs+nseg+1,3);
	fill(rightwidsegs,rightwidsegs+nseg+1,0);
	fill(heightsegs,heightsegs+nseg+1,0);
}

piece::piece(const json& js) : pt(0),rot(0)
{
	fill(leftwidsegs,leftwidsegs+nseg+1,3);
	fill(rightwidsegs,rightwidsegs+nseg+1,0);
	fill(heightsegs,heightsegs+nseg+1,0);
	js.get_object_int("pt",pt);
	js.get_object_int("rot",rot);
	js.get_object_array_int("leftwidsegs",leftwidsegs,nseg+1);
	js.get_object_array_int("rightwidsegs",rightwidsegs,nseg+1);
	js.get_object_array_int("heightsegs",heightsegs,nseg+1);
}

json piece::save() const
{
	json ret=json::create_object();
	ret.insert_int("pt",pt);
	ret.insert_int("rot",rot);
	ret.insert_array_int("leftwidsegs",leftwidsegs,nseg+1);
	ret.insert_array_int("rightwidsegs",rightwidsegs,nseg+1);
	ret.insert_array_int("heightsegs",heightsegs,nseg+1);
	return ret;
}

trk::trk(const json& js) /* : 
	sqrt2o2(sqrtf(2.0f)*.5f),
	pieceheight(PIECESIZE/6.0f*sqrtf(6.0f)/6.0f) */
{
	//memset(pieces,0,sizeof(pieces));
	fill(pieces[0],pieces[0]+TRACKZ*TRACKX,piece());
	js.get_object_array_object<piece,piece>("pieces","piece",pieces[0],TRACKZ*TRACKX);
	basetex=0;
	js.get_object_int("basetex",basetex);
	skybox=0;
	js.get_object_int("skybox",skybox);
	backsnd=0;
	js.get_object_int("backsnd",backsnd);
//	studyforks();
}

json trk::save() const
{
	json ret=json::create_object();
	ret.insert_array_object<piece,piece>("pieces","piece",pieces[0],TRACKZ*TRACKX);
	ret.insert_int("basetex",basetex);
	ret.insert_int("skybox",skybox);
	ret.insert_int("backsnd",backsnd);
	return ret;
}

// load and parse json, convert to trk, build trk to tree2
trkt::trkt(const json& js) : trk(js),root(0)
{
	fill(treedata[0],treedata[0]+TRACKZ*TRACKX,(tree2*)0);
}

void trkt::buildtrack(bool newskybox)
{
// tricky part1
	tree2* sb = 0;
	if (root && !newskybox)
		sb = root->find("skybox");
	if (sb)
		sb->unlink();
	delete root;
	root=new tree2("consscene");
	int i,j;
	for (j=0;j<TRACKZ;j++) {
		for (i=0;i<TRACKX;i++) {
			tree2* pce=buildconspiece(basetex,&pieces[j][i]);
			adjustpiece(pce,i,j);
			root->linkchild(pce);
			treedata[j][i]=pce;
		}
	}
// build a skybox
// tricky part2
	if (sb) {
		root->linkchild(sb);
	} else {
		if (skybox) {
pushandsetdir("skybox");
			tree2* tt=buildskybox(pointf3x(200,200,200),skyboxnames[skybox],"tex");
			root->linkchild(tt);
popdir();
		}
	}
}

wavehandle* trkt::buildbacksnd()
{
pushandsetdirdown("soundbackgnd");
	wavehandle* ret = wave_load(backsndnames[backsnd],0);
popdir();
	wave_play(ret,0); // 0 forever, 1 once, wave_unload will free soundhandle
//	playasound(ret);
	return ret;
}

static void rebuildpiece()
{
	if (trackpiece) {
		delete trackpiece;
	}
pushandsetdirdown("constructed");
	trackpiece=buildconspiece(thetrackj->basetex,&cpce);
popdir();
	constructor_viewport2_roottree->linkchild(trackpiece);
}

static void rebuildtrack(bool newskybox)
{
	thetrackj->buildtrack(newskybox);
	wave_unload(backsndwave);
	backsndwave = thetrackj->buildbacksnd();
	trackscene=thetrackj->root;
	constructor_viewport_roottree->linkchild(trackscene);
}

static void rebuildtrackandpiece(bool newskybox)
{
	rebuildtrack(newskybox);
	rebuildpiece();
}

static void cleartrack()
{
	int i,j;
	for (j=0;j<TRACKZ;j++) {
		for (i=0;i<TRACKX;i++) {
			thetrackj->pieces[j][i] = piece();
		}
	}
	rebuildtrack(false);
}
#if 0
static void ui2piece(piece& p)
{
	S32 i;
	for (i=0;i<piece::nseg+1;++i) {
		p.leftwidsegs[i] = 3 - vleftwidth[i]->getidx();
		p.rightwidsegs[i] = 2 - vrightwidth[i]->getidx();
		p.heightsegs[i] = 4 - vheight[i]->getidx();
	}
}
#endif
static void setuivis(S32 pt)
{
	S32 i;
	switch(pt) {
		case PCE_STARTFINISH:
			for (i=0;i<piece::nseg+1;++i) {
				vleftwidth[i]->setvis(0);
				vrightwidth[i]->setvis(0);
				vheight[i]->setvis(0);
			}
			vleftwidthg->setvis(0);
			vrightwidthg->setvis(0);
			vheightg->setvis(1);
			break;
		case PCE_STRAIGHT:
			for (i=0;i<piece::nseg+1;++i) {
				vleftwidth[i]->setvis(1);
				vrightwidth[i]->setvis(1);
				vheight[i]->setvis(1);
			}
			vleftwidthg->setvis(1);
			vrightwidthg->setvis(1);
			vheightg->setvis(1);
			break;
		case PCE_BLANK:
			for (i=0;i<piece::nseg+1;++i) {
				vleftwidth[i]->setvis(0);
				vrightwidth[i]->setvis(0);
				vheight[i]->setvis(0);
			}
			vleftwidthg->setvis(0);
			vrightwidthg->setvis(0);
			vheightg->setvis(0);
			break;
		case PCE_LEFTFORK:
		case PCE_RIGHTFORK:
		case PCE_INTERSECTION:
		case PCE_TURN:
			for (i=0;i<piece::nseg+1;++i) {
				vleftwidth[i]->setvis(0);
				vrightwidth[i]->setvis(0);
				vheight[i]->setvis(0);
			}
			vleftwidthg->setvis(1);
			vrightwidthg->setvis(1);
			vheightg->setvis(1);
			break;
	}
}

static void piece2ui(const piece& p)
{
	S32 i;
	for (i=0;i<piece::nseg+1;++i) {
		vleftwidth[i]->setnumidx(3);
		vleftwidth[i]->setidx(3-p.leftwidsegs[i]);

		vrightwidth[i]->setnumidx(3);
		vrightwidth[i]->setidx(2-p.rightwidsegs[i]);

		vheight[i]->setnumidx(5);
		vheight[i]->setidx(4-p.heightsegs[i]);
	}
	vleftwidthg->setidx(3-p.leftwidsegs[3]);
	vrightwidthg->setidx(2-p.rightwidsegs[3]);
	vheightg->setidx(4-p.heightsegs[3]);
	setuivis(p.pt);
}

void constructorinit()
{
pushandsetdir("newcarenadata");
	tl = new tracklist;
	int i;
	consmode=MODE_NORMAL;
	logger("---------------------- constructor init -------------------------------\n");
	video_setupwindow(800,600);//,565);
//	MX=MY=0; // hack to get the cursor off of the track initialy
	constructor_viewport.backcolor=C32GREEN;
	constructor_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	constructor_viewport_roottree=new tree2("roottree");
	constructor_viewport.xstart=TRACKVIEWX;
	constructor_viewport.ystart=TRACKVIEWY;
	constructor_viewport.xres=TRACKVIEWW;
	constructor_viewport.yres=TRACKVIEWH;
	constructor_viewport.zfront=50;
	constructor_viewport.zback=2000;
	constructor_viewport2.backcolor=C32BROWN;//frgbbrown;
	constructor_viewport2.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	constructor_viewport2_roottree=new tree2("roottree2");
	constructor_viewport2.xstart=TRACKVIEWX2;
	constructor_viewport2.ystart=TRACKVIEWY2;
	constructor_viewport2.xres=TRACKVIEWW2;
	constructor_viewport2.yres=TRACKVIEWH2;
	constructor_viewport2.zfront=1.0f;
	constructor_viewport2.zback=2000;
	constructor_viewport.xsrc=4;constructor_viewport.ysrc=3;
//	constructor_viewport.lookat=0;
	constructor_viewport.useattachcam=false;
	constructor_viewport2.xsrc=4;constructor_viewport2.ysrc=3;
	constructor_viewport2.useattachcam=false;
//	constructor_viewport2.lookat=0;
pushandsetdirdown("constructed");
	rl=res_loadfile("constructorres.txt");
	rlnn=res_loadfile("newnameres.txt");
popdir();
pushandsetdirdown(getconstructeddir());
	ttrack=rl->find<text>("TEXTTRACK");
	pbacksave=rl->find<pbut>("PBUTBACKSAVE");
	pbacknosave=rl->find<pbut>("PBUTBACKNOSAVE");
	pbackdelete=rl->find<pbut>("PBUTBACKDELETE");
	pcopy=rl->find<pbut>("PBUTCOPY");
	pclear=rl->find<pbut>("PBUTCLEAR");
	tbasetex=rl->find<text>("TEXTBASETEX");
	vbasetex=rl->find<vscroll>("VSLIDERBASETEX");
	tskybox=rl->find<text>("TEXTSKYBOX");
	vskybox=rl->find<vscroll>("VSLIDERSKYBOX");
	tbacksnd=rl->find<text>("TEXTBACKSND");
	vbacksnd=rl->find<vscroll>("VSLIDERBACKSND");
	lpiecetypes=rl->find<listbox>("LISTPIECETYPES");
	protateleft=rl->find<pbut>("PBUTROTATELEFT");
	protateright=rl->find<pbut>("PBUTROTATERIGHT");

	ename=rlnn->find<edit>("EDITNAME");
	enameok=rlnn->find<pbut>("PBUTNAMEOK");
	enamecan=rlnn->find<pbut>("PBUTNAMECAN");
	vleftwidthg=rl->find<vscroll>("VSLIDERTRACKLEFTWIDTHG");
	vrightwidthg=rl->find<vscroll>("VSLIDERTRACKRIGHTWIDTHG");
	vheightg=rl->find<vscroll>("VSLIDERTRACKHEIGHTG");
	vleftwidthg->setnumidx(3);
	vrightwidthg->setnumidx(3);
	vheightg->setnumidx(5);
#if 1
	{
		C8 str[100];
		S32 i;
		for (i=0;i<piece::nseg+1;++i) {
			sprintf(str,"VSLIDERTRACKLEFTWIDTH%d",i);
			vleftwidth[i]=rl->find<vscroll>(str);
//			vleftwidth[i]->setnumidx(3);
//			vleftwidth[i]->setidx(0);
//			leftwidth[i]=3;

			sprintf(str,"VSLIDERTRACKRIGHTWIDTH%d",i);
			vrightwidth[i]=rl->find<vscroll>(str);
//			vrightwidth[i]->setnumidx(3);
//			vrightwidth[i]->setidx(2);
//			rightwidth[i]=0;

			sprintf(str,"VSLIDERTRACKHEIGHT%d",i);
			vheight[i]=rl->find<vscroll>(str);
//			vheight[i]->setnumidx(5);
//			vheight[i]->setidx(4);
//			height[i]=0;
		}
	}
#endif
	if (!lobby_track.size())
		lobby_track="default";
	ttrack->settname(lobby_track.c_str());
	json js=json(fileload_string((lobby_track+".trkoc").c_str()));
popdir();
pushandsetdirdown("constructed");
	thetrackj = new trkt(js);
	thetrackj->buildtrack(true);
	wave_unload(backsndwave);
popdir();
	backsndwave = thetrackj->buildbacksnd();
	trackscene=thetrackj->root;
	constructor_viewport_roottree->linkchild(trackscene);
	cpce = piece();
	piece2ui(cpce);
	//memset(&cpce,0,sizeof(cpce));
	trackpiece=buildconspiece(thetrackj->basetex,&cpce);
	constructor_viewport2_roottree->linkchild(trackpiece);
pushandsetdirdown("constructed");
	buildtrackcursor();
popdir();

	updatebasetexres();
	vbasetex->setnumidx(NBASETEXT);
	vbasetex->setidx(thetrackj->basetex);

	updateskyboxres();
	vskybox->setnumidx(NSKYBOX);
	vskybox->setidx(thetrackj->skybox);

	updatebacksndres();
	vbacksnd->setnumidx(NBACKSND);
	vbacksnd->setidx(thetrackj->backsnd);

	for (i=0;i<(S32)NPIECETYPES;i++)
		lpiecetypes->addstring(tracktypenames[i]);
	lpiecetypes->setidxc(0);
	if (tl->size()<=1)
		pbackdelete->setvis(0);
	focus=0;
	extradebvars(constructorvars,nconstructorvars);
}

static void fixsegs(S32* s,S32 i,S32 ms) // segment, index, maxslope
{
	if (i>0) { // check one less
		if (s[i-1]>s[i]+ms) {
			s[i-1]=s[i]+ms;
		} if (s[i-1]<s[i]-ms) {
			s[i-1]=s[i]-ms;
		}
	}
	if (i<piece::nseg) { // check one more
		if (s[i+1]>s[i]+ms) {
			s[i+1]=s[i]+ms;
		} if (s[i+1]<s[i]-ms) {
			s[i+1]=s[i]-ms;
		}
	}
}

void constructorproc()
{
	char str[70];
	switch(consmode) {
	case MODE_NORMAL:
		if (trkcur.valid) {
			if (wininfo.mleftclicks) { // drop, replace old piece
				tree2* *pt;
				struct piece *p;
				p = &thetrackj->pieces[trkcur.z][trkcur.x]; // old piece data
				pt = &thetrackj->treedata[trkcur.z][trkcur.x]; // old piece tree
//				p->pt=cpce.pt;
//				p->rot=cpce.rot;
				*p = cpce; // copy cur piece to track array
				delete *pt; // rebuild tree
				*pt=buildconspiece(thetrackj->basetex,p);
				adjustpiece(*pt,trkcur.x,trkcur.z);
				trackscene->linkchild(*pt);
			}
			if (wininfo.mrightclicks) { // pick up
				struct piece *p;
				p=&thetrackj->pieces[trkcur.z][trkcur.x];
//				cpce.pt=p->pt;
//				cpce.rot=p->rot;
				cpce = *p;
				rebuildpiece();
				focus=0;
				piece2ui(cpce);
			}
		}
		if (wininfo.mleftclicks)
			focus=rl->getfocus();
		if (focus) {
			S32 ret=focus->proc();
			shape* focusb = ret ? focus : 0;
			shape* focush = ret>=0 ? focus : 0;
			shape* focusl = ret>=0 ? focus : 0;
			if (focusb==pbacksave) {
				lobby_track=ttrack->gettname();
pushandsetdirdown(getconstructeddir());
				json js=thetrackj->save();
				string ss = lobby_track + ".trkoc";
				js.save(ss.c_str());
popdir();
				popstate();
			} else if (focusb==pbacknosave) {
				popstate();
			} else if (focusb==pbackdelete) {
				sprintf(str,"%s.trkoc",lobby_track.c_str());
pushandsetdirdown(getconstructeddir());
				lobby_track.clear();
				remove(str);
popdir();
				popstate();
			} else if (focusb==pcopy) {
				consmode=MODE_NEWNAME;
				focus=ename;
				string un=tl->uniquetrackname(ttrack->gettname());
				ename->settname(un.c_str());
			} else if (focusb==pclear) {
				cleartrack();
				rebuildtrackandpiece(false);
			} else if (focush==vbasetex) {
				if (thetrackj->basetex!=vbasetex->getidx()) {
					thetrackj->basetex=vbasetex->getidx();
					rebuildtrackandpiece(false);
					updatebasetexres();
				}
			} else if (focush==vskybox) {
				if (thetrackj->skybox!=vskybox->getidx()) {
					thetrackj->skybox=vskybox->getidx();
					rebuildtrackandpiece(true);
					updateskyboxres();
				}
			} else if (focush==vbacksnd) {
				if (thetrackj->backsnd!=vbacksnd->getidx()) {
					thetrackj->backsnd=vbacksnd->getidx();
//					rebuildtrackandpiece();
					wave_unload(backsndwave);
					backsndwave=thetrackj->buildbacksnd();
					updatebacksndres();
				}
			} else if (focusl==lpiecetypes) {
				cpce = piece();
				cpce.pt=piecetype(lpiecetypes->getidx());
				rebuildpiece();
				piece2ui(cpce);
			} else if (focusb==protateleft) {
				cpce.rot--;
				cpce.rot&=3;
				rebuildpiece();
			} else if (focusb==protateright) {
				cpce.rot++;
				cpce.rot&=3;
				rebuildpiece();
			} else if (focush==vleftwidthg) {
				S32 i;
				for (i=0;i<=piece::nseg;++i) {
					cpce.leftwidsegs[i]=3-vleftwidthg->getidx();
					if (cpce.leftwidsegs[i]<=cpce.rightwidsegs[i]) {
						cpce.rightwidsegs[i]=cpce.leftwidsegs[i]-1; // min width 1
						fixsegs(cpce.rightwidsegs,i,1);
					}
				}
				rebuildtrackandpiece(false);
				piece2ui(cpce);
			} else if (focush==vrightwidthg) {
				S32 i;
				for (i=0;i<=piece::nseg;++i) {
					cpce.rightwidsegs[i]=2-vrightwidthg->getidx();
					if (cpce.leftwidsegs[i]<=cpce.rightwidsegs[i]) {
						cpce.leftwidsegs[i]=cpce.rightwidsegs[i]+1;
						fixsegs(cpce.leftwidsegs,i,1);
					}
				}
				rebuildtrackandpiece(false);
				piece2ui(cpce);
			} else if (focush==vheightg) {
				S32 i;
				for (i=0;i<=piece::nseg;++i) {
					cpce.heightsegs[i]=4-vheightg->getidx();
				}
				rebuildtrackandpiece(false);
				piece2ui(cpce);
			} else { // modifier sliders
				S32 i;
				bool bui = false;
				for (i=0;i<=piece::nseg;++i) {
					if (focush==vleftwidth[i]) {
						cpce.leftwidsegs[i]=3-vleftwidth[i]->getidx();
						fixsegs(cpce.leftwidsegs,i,1);
						if (cpce.leftwidsegs[i]<=cpce.rightwidsegs[i]) {
							cpce.rightwidsegs[i]=cpce.leftwidsegs[i]-1; // min width 1
							fixsegs(cpce.rightwidsegs,i,1);
						}
						break;
					} else if (focush==vrightwidth[i]) {
						cpce.rightwidsegs[i]=2-vrightwidth[i]->getidx();
						fixsegs(cpce.rightwidsegs,i,1);
						if (cpce.leftwidsegs[i]<=cpce.rightwidsegs[i]) {
							cpce.leftwidsegs[i]=cpce.rightwidsegs[i]+1;
							fixsegs(cpce.leftwidsegs,i,1);
						}
						break;
					} else if (focush==vheight[i]) {
						cpce.heightsegs[i]=4-vheight[i]->getidx();
						fixsegs(cpce.heightsegs,i,2);
						break;
					}
				} // end for
				if (i!=piece::nseg+1) { // break;
					rebuildtrackandpiece(false);
					piece2ui(cpce);
				}
			} // end modifier sliders
		}
		break;
	case MODE_NEWNAME:
		if (wininfo.mleftclicks)
			focus=rlnn->getfocus();
		if (focus) {
			S32 ret=focus->proc();
			shape* focusb = ret ? focus : 0;
			shape* focuse = ret>=0 ? focus : 0;
			if (focuse==ename || focusb==enameok) {
				string un=tl->uniquetrackname(ename->gettname());
				ename->settname(un.c_str());
				consmode=MODE_NORMAL;
				focus=0;
				ttrack->settname(un.c_str());
			} else if (focusb==enamecan) {
				consmode=MODE_NORMAL;
				focus=0;
			}
		}
	}
// update display
	updatecursor();
	doflycam(&constructor_viewport);
	doflycam(&constructor_viewport2);
	constructor_viewport_roottree->proc();
	video_buildworldmats(constructor_viewport_roottree);
	constructor_viewport2_roottree->proc();
	video_buildworldmats(constructor_viewport2_roottree);
}

void constructordraw2d()
{
	switch(consmode) {
	case MODE_NORMAL:
		rl->draw();
		break;
	case MODE_NEWNAME:
		rlnn->draw();
		break;
	}
	if (calcedgrid)
		outtextxyf32(B32,150,380,C32WHITE,"%f, %f, %f",grid.x,grid.y,grid.z);
}

void constructordraw3d()
{
	constructor_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	constructor_viewport.xres=WX;
	constructor_viewport.yres=WY;
	constructor_viewport.xstart=0;
	constructor_viewport.ystart=0;
	constructor_viewport.backcolor=C32(0,85,0);
	video_setviewport(&constructor_viewport); // clear zbuf etc.

	constructor_viewport.flags=0;//VP_CHECKER|VP_CLEARBG;
	constructor_viewport.xres=TRACKVIEWW;
	constructor_viewport.yres=TRACKVIEWH;
	constructor_viewport.xstart=TRACKVIEWX;
	constructor_viewport.ystart=TRACKVIEWY;
	constructor_viewport.backcolor=C32GREEN;
	video_setviewport(&constructor_viewport); // clear zbuf etc.
	video_drawscene(constructor_viewport_roottree);

	video_setviewport(&constructor_viewport2);
	video_drawscene(constructor_viewport2_roottree);
}

void constructorexit()
{
	logger("---------------------- constructor exit -------------------------------\n");
	constructor_viewport_roottree->log2();
	constructor_viewport2_roottree->log2();
	logrc();
// free everything
	wave_unload(backsndwave);
	backsndwave = 0;
	extradebvars(0,0);
	delete rl;
	delete rlnn;
	delete thetrackj;
	delete constructor_viewport_roottree;
	constructor_viewport_roottree=0;
	delete constructor_viewport2_roottree;
	constructor_viewport2_roottree=0;
popdir();
	delete tl;
//	leftwidth[0]=3; // rs0orary
//	rightwidth[0]=0;
}

const C8* getconstructeddir()
{
	if (wininfo.isalreadyrunning)
		return "constructed_b";
	else
		return "constructed";
}
