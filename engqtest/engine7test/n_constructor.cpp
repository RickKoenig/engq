/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <engine7cpp.h>
#include <videoicpp.h>
#include <misclib7cpp.h>

#include "constructorres.h"
#include "newnameres.h"
#include "perfcpp.h"
#include "constructorcpp.h"
#include "usefulcpp.h"
*/

#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "n_deflatecpp.h"
#include "n_usefulcpp.h" 
#include "n_constructorcpp.h"
#include "n_facevertscpp.h"
#include "n_polylistcpp.h"
#include "n_meshtestcpp.h"
#include "n_carenalobbycpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"

viewport2 constructor_viewport,constructor_viewport2;//,constructor_viewport3;
tree2* constructor_viewport_roottree,*constructor_viewport2_roottree;
//static struct viewport constructor_viewportm;

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
static vscroll* vbasetex;
static listbox* lpiecetypes;
static pbut* protateleft;
static pbut* protateright;

static edit* ename;
static pbut* enameok;
static pbut* enamecan;

#if 0

x	text TEXTTRACK
x	pbut PBUTBACKSAVE
x	pbut PBUTBACKNOSAVE
x	pbut PBUTBACKDELETE
x	pbut PBUTCOPY
x	pbut PBUTCLEAR
x	text TEXTBASETEX
x	vscroll VSLIDERBASETEX
x	listbox LISTPIECETYPES
x	pbut PBUTROTATELEFT
x	pbut PBUTROTATERIGHT
#endif
#if 0
x	edit EDITNAME 2
#endif


static tree2* trackcursor,*trackscene,*trackpiece;

struct strackcursor {
	int x,z,valid;
};
static struct strackcursor trkcur;

//static struct reslist *rl,*rlnn;
static shape* rl,*rlnn;
static shape* focus;
enum mode {MODE_NORMAL,MODE_NEWNAME};
static mode consmode;

#define PIECESIZE 40 // 40 meters by 40 meters
// 8 by 8 grid (TRACKX and TRACKY) make this total 320 by 320 square meters

enum piecetype {
	PCE_BLANK,
	PCE_STRAIGHT,
	PCE_TURN,
	PCE_STARTFINISH,
	PCE_INTERSECTION,
	PCE_RIGHTFORK,
	PCE_LEFTFORK,
};
static char *tracktypenames[]={
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
static track thetrack;
//static unsigned int trackhash[4];
static int uniquei;

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

static char *basetexnames[]={
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
//	int (*fces)[3];
	face* fces;
	static pointf3 verts[10]={
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
		uvs[i].u=verts[i].x*(1.0f/PIECESIZE);
		uvs[i].v=1-verts[i].z*(1.0f/PIECESIZE);
	}
//	buildmodelstart("trackcursor");
	fces=buildfaces(verts,10,&nfce);
	if (!fces)
		errorexit("can't build trackcursor");
	modelb* m=model_create("trackcursor");
//	setnextmaterial("rim","sanddbl.tga");
	m->copyverts(verts,10);
	m->copyuvs0(uvs,10);
	m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_HASSHADE|SMAT_CALCLIGHTS,
			"sanddbl.tga",0,30,nfce,10);
//	buildmodeladduvs(verts,uvs,10);
//			for (i=0;i<nfce;i++)
//				m->addface(fces[i][0],fces[i][1],fces[i][2]);
//				m->addface(fces[i][0],fces[i][1],fces[i][2]);
//			memfree(fces);
	m->addfaces(fces,nfce,0);
	delete[] fces;
//	trackcursor=alloctreebuildmodelend(0);
//	linkchildtoparent(trackcursor,constructor_viewport.roottree);
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

/*static void flipinz(pointf3 *v,int n)
{
	int i,nh;
	nh=n>>1;
	pointf3 t;
	for (i=0;i<nh;i++) {
		t=v[i];
		v[i]=v[n-1-i];
		v[n-1-i]=t;
	}
	for (i=0;i<n;i++)
		v[i].z=PIECESIZE-v[i].z;
}
*/
static void rotpiece(pointf3 *v,int n,int rotcc90)
{
	int i;
	float t;
	switch(rotcc90) {
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
#define PRES 8
	pointf3 ls={0,0,PIECESIZE/4.0f};
	pointf3 rs={PIECESIZE,0,PIECESIZE/4.0f};
	pointf3 ls2={0,0,PIECESIZE*3/4.0f};
	pointf3 rs2={PIECESIZE,0,PIECESIZE*3/4.0f};
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
//	buildmodelstart(str);
	modelb* m=model_create(str);
//	if (video_maindriver==VIDEO_D3D)
//		buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_CALCLIGHTS,
//			basetexnames[bt],0,"basemat",0);
//	else
//		buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV,
//			basetexnames[bt],0,"basemat",0);
	S32 f1=0;S32 f2=0;S32 f3=0;S32 f4=0;
	S32 v1=0;S32 v2=0;S32 v3=0;S32 v4=0;
	switch(p->pt) {
	case PCE_BLANK:
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			basetexnames[bt],0,30,2,4);
		for (i=0;i<4;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
//		buildmodeladduvs(baseverts,baseuvs,4);
		m->copyverts(baseverts,4);
		m->copyuvs0(baseuvs,4);
//		m->addface(0,2,1);
		m->addface(0,2,1);
//		m->addface(1,2,3);
		m->addface(1,2,3);
		break;

	case PCE_TURN:
//		memcpy(wverts,baseverts,sizeof(pointf3)*4);
// 4 corners
		copy(baseverts,baseverts+4,wverts);
// start of turn on bottom
		wverts[4].x=PIECESIZE/4.0f;
		wverts[4].y=0;
		wverts[4].z=0;
		wverts[4+PRES].x=PIECESIZE*3/4.0f;
		wverts[4+PRES].y=0;
		wverts[4+PRES].z=0;
// middle of turn up and to the left
		for (i=1;i<PRES-1;i++) {
			wverts[4+i].x=(PIECESIZE/4.0f)*cosf(PI*.5f*float(i)/(PRES-1));
			wverts[4+i].y=0;
			wverts[4+i].z=(PIECESIZE/4.0f)*sinf(PI*.5f*float(i)/(PRES-1));
			wverts[4+PRES+i].x=(PIECESIZE*3/4.0f)*cosf(PI*.5f*float(i)/(PRES-1));
			wverts[4+PRES+i].y=0;
			wverts[4+PRES+i].z=(PIECESIZE*3/4.0f)*sinf(PI*.5f*float(i)/(PRES-1));
		}
// end of turn at the left
		wverts[4+PRES-1].x=0;
		wverts[4+PRES-1].y=0;
		wverts[4+PRES-1].z=PIECESIZE/4.0f;
		wverts[4+2*PRES-1].x=0;
		wverts[4+2*PRES-1].y=0;
		wverts[4+2*PRES-1].z=PIECESIZE*3/4.0f;
		rotpiece(wverts,PRES*2+4,p->rot);
		for (i=0;i<PRES*2+4;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
//		memcpy(&wverts[4+2*PRES],wverts,sizeof(pointf3)*(4+2*PRES));
		copy(wverts,wverts+2*PRES+4,wverts+2*PRES+4);
		copy(wuvs,wuvs+2*PRES+4,wuvs+2*PRES+4);
// register verts and uvs with the model
		m->copyverts(wverts,2*(2*PRES+4));
		m->copyuvs0(wuvs,2*(2*PRES+4));
// register faces for outside/backgnd
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
// register mat1 faces for inside/track		
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,PRES-1+PRES-1,2*PRES+4);
//		buildmodeladduvs(wverts,wuvs,PRES*2+4);
// build track
/*		if (video_maindriver==VIDEO_D3D)
			buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_CALCLIGHTS,
				"4pl_tile01.jpg",0,"trackmat",0);
		else
			buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV,
				"4pl_tile01.jpg",0,"trackmat",0); 
		buildmodeladduvs(wverts,wuvs,PRES*2+4); */
		for (i=0;i<PRES-1;i++) {
			m->addface(4+i+4+2*PRES,4+PRES+1+i+4+2*PRES,4+PRES+i+4+2*PRES);
			m->addface(4+i+4+2*PRES,4+1+i+4+2*PRES,4+PRES+1+i+4+2*PRES);
		}
		break;
	case PCE_STRAIGHT:
		copy(baseverts,baseverts+8,wverts);
		copy(wverts,wverts+8,wverts+8);
//		memcpy(wverts,baseverts,sizeof(pointf3)*8);
/*		for (i=0;i<4;i++) {
			wverts[i+4].x=wverts[i].x;
			wverts[i+4].y=0;
			wverts[i+4].z=.5f*(wverts[i].z-16)+16;
		} */
		rotpiece(wverts,16,p->rot&1);
		for (i=0;i<16;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
//		buildmodeladduvs(wverts,wuvs,16);
		m->copyverts(wverts,16);
		m->copyuvs0(wuvs,16);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			basetexnames[bt],0,30,4,8);

		m->addface(0,5,1);
		m->addface(0,4,5);
		m->addface(6,3,7);
		m->addface(6,2,3);
//		if (video_maindriver==VIDEO_D3D)
//			buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_CALCLIGHTS,
//				"4pl_tile01.jpg",0,"trackmat",0);
//		else
//			buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV,
//				"4pl_tile01.jpg",0,"trackmat",0);
//		buildmodeladduvs(wverts,wuvs,8);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,2,8);
		m->addface(4+8,7+8,5+8);
		m->addface(4+8,6+8,7+8);
		break;
	case PCE_INTERSECTION:
		for (j=0;j<4;j++)
			for (i=0;i<4;i++) {
				wverts[i+4*j].x=ivals[i];
				wverts[i+4*j].y=0;
				wverts[i+4*j].z=ivals[j];
			}
		for (i=0;i<16;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
		copy(wverts,wverts+16,wverts+16);
		copy(wuvs,wuvs+16,wuvs+16);
		m->copyverts(wverts,32);
		m->copyuvs0(wuvs,32);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			basetexnames[bt],0,30,8,16);
		for (j=0;j<3;j++)
			for (i=0;i<3;i++)
				if (i!=1 && j!=1) {
					m->addface(i+j*4,i+j*4+4+1,i+j*4+1);
					m->addface(i+j*4,i+j*4+4,i+j*4+4+1);
				}
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,10,16);
		for (j=0;j<3;j++)
			for (i=0;i<3;i++)
				if (i==1 || j==1) {
					m->addface(16+i+j*4,16+i+j*4+4+1,16+i+j*4+1);
					m->addface(16+i+j*4,16+i+j*4+4,i+16+j*4+4+1);
				}
		break;
	case PCE_STARTFINISH:
		for (j=0;j<4;j++)
			for (i=0;i<4;i++) {
				wverts[i+4*j].x=sfvals[i];
				wverts[i+4*j].y=0;
				wverts[i+4*j].z=ivals[j];
			}
		rotpiece(wverts,16,p->rot&1);
		for (i=0;i<16;i++) {
			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
		}
		copy(wverts,wverts+16,wverts+16);
		copy(wuvs,wuvs+16,wuvs+16);
		copy(wverts+16,wverts+32,wverts+32);
		copy(wuvs+16,wuvs+32,wuvs+32);
		m->copyverts(wverts,48);
		m->copyuvs0(wuvs,48);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			basetexnames[bt],0,30,12,16);
		for (j=0;j<3;j++)
			for (i=0;i<3;i++)
				if (j==0 || j==2) {
					m->addface(i+j*4,i+j*4+4+1,i+j*4+1);
					m->addface(i+j*4,i+j*4+4,i+j*4+4+1);
				}
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,4,16);
		for (j=0;j<3;j++)
			for (i=0;i<3;i++)
				if (j==1 && i!=1) {
					m->addface(16+i+j*4,16+i+j*4+4+1,16+i+j*4+1);
					m->addface(16+i+j*4,16+i+j*4+4,16+i+j*4+4+1);
				}
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"sanddbl.tga",0,30,2,16);
		i=1;j=1;
//		for (j=0;j<3;j++) 
//			for (i=0;i<3;i++)
//				if (i==1 && j==1) {
					m->addface(32+i+j*4,32+i+j*4+4+1,32+i+j*4+1);
					m->addface(32+i+j*4,32+i+j*4+4,32+i+j*4+4+1);
//				}
		break;
	case PCE_LEFTFORK:
		zflipf=true;
	case PCE_RIGHTFORK:
// inside turn
		wverts[0]=baseverts[0];
		k=0;
		for (i=0;i<PRES;i++) {
			if (i==0) {
				wverts[PRES-i].x=PIECESIZE/4.0f;
				wverts[PRES-i].y=0;
				wverts[PRES-i].z=0;
			} else if (i==PRES-1) {
				wverts[PRES-i].x=0;
				wverts[PRES-i].y=0;
				wverts[PRES-i].z=PIECESIZE/4.0f;
			} else {
				wverts[PRES-i].x=(PIECESIZE/4.0f)*cosf(PI*.5f*float(i)/(PRES-1));
				wverts[PRES-i].y=0;
				wverts[PRES-i].z=(PIECESIZE/4.0f)*sinf(PI*.5f*float(i)/(PRES-1));
			}
			wverts2[k++]=wverts[PRES-i];
		}
		wverts2[k++]=ls2;
		wverts2[k++]=rs2;
		wverts2[k++]=rs;
//		buildmodeladduvs(wverts,wuvs,PRES+1);

		int nfce;
		face* fces;
		v1=PRES+1;		// PRES+1 verts copied
		fces=buildfaces(wverts,v1,&nfce);
		if (fces) {
//			for (i=0;i<nfce;i++)
//				m->addface(fces[i].vertidx[0],fces[i][1],fces[i][2]);
			m->addfaces(fces,nfce,0,zflipf);
//			memfree(fces);
			delete[] fces;
			f1=nfce;
		}
		copy(wverts,wverts+v1,wverts3); 
// fork part
		wverts[0]=baseverts[1];
		for (i=0;i<PRES;i++) {
			if (i==0) {
				wverts[i+1].x=PIECESIZE*3/4.0f;
				wverts[i+1].y=0;
				wverts[i+1].z=0;
			} else if (i==PRES-1) {
				wverts[i+1].x=0;
				wverts[i+1].y=0;
				wverts[i+1].z=PIECESIZE*3/4.0f;
			} else {
				wverts[i+1].x=(PIECESIZE*3/4.0f)*cosf(PI*.5f*float(i)/(PRES-1));
				wverts[i+1].y=0;
				wverts[i+1].z=(PIECESIZE*3/4.0f)*sinf(PI*.5f*float(i)/(PRES-1));
			}
			if (intersectline2d(&ls,&rs,&wverts[i],&wverts[i+1],&intsect,NULL,NULL)>0) {
				wverts[i+1]=intsect;
				wverts[i+2]=rs;
				break;
			}
		}
		j=i+3;
		for (i=j-2;i>=1;i--)
			wverts2[k++]=wverts[i];
//		if (zflipf)
//			flipinz(wverts,j);
//		rotpiece(wverts,j,p->rot);
//		for (i=0;i<j;i++) {
//			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
//			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
//		}
//		buildmodeladduvs(wverts,wuvs,j);
		fces=buildfaces(wverts,j,&nfce);
		if (fces) {
//			for (i=0;i<nfce;i++)
//				m->addface(fces[i].vertidx[0]+v1,fces[i].vertidx[1]+v1,fces[i].vertidx[2]+v1);
			m->addfaces(fces,nfce,v1,zflipf);
//			memfree(fces);
			delete[] fces;
			f2=nfce;
		}
		v2=j;
		copy(wverts,wverts+v2,wverts3+v1);
// straight part
		wverts[0]=baseverts[2];
		wverts[1]=baseverts[3];
		wverts[2]=rs2;
		wverts[3]=ls2;
		j=4;
//		if (zflipf)
//			flipinz(wverts,j);
//		rotpiece(wverts,j,p->rot);
//		for (i=0;i<j;i++) {
//			wuvs[i].u=wverts[i].x*(1.0f/PIECESIZE);
//			wuvs[i].v=1-wverts[i].z*(1.0f/PIECESIZE);
//		}
//		buildmodeladduvs(wverts,wuvs,j);
		fces=buildfaces(wverts,j,&nfce);
		if (fces) {
//			for (i=0;i<nfce;i++)
//				m->addface(fces[i][0],fces[i][1],fces[i][2]);
			m->addfaces(fces,nfce,v1+v2,zflipf);
//			memfree(fces);
			delete[] fces;
			f3=nfce;
		}
		v3=j;
		copy(wverts,wverts+v3,wverts3+v1+v2);
// first material
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			basetexnames[bt],0,30,f1+f2+f3,v1+v2+v3);
// track part, second material
		fces=buildfaces(wverts2,k,&nfce);
		if (fces) {
			m->addfaces(fces,nfce,v1+v2+v3,zflipf);
//			memfree(fces);
			delete[] fces;
			f4=nfce;
		}
		v4=k;
		copy(wverts2,wverts2+v4,wverts3+v1+v2+v3);
		m->addmatn("tex",SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS,
			"4pl_tile01.jpg",0,30,f4,v4);
//		S32 ft=f1+f2+f3+f4;
		S32 vt=v1+v2+v3+v4;
		if (zflipf) {
//			flipinz(wverts3,vt);
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
	}
	m->close();

//	ret=alloctreebuildmodelend(0);
	ret=new tree2(str);
	ret->setmodel(m);
	return ret;
}

static void adjustpiece(tree2 *p,int i,int j)
{
/*	int k;
	for (k=0;k<p->mod->nvert;k++) {
		p->mod->verts[k].x+=i*PIECESIZE;
		p->mod->verts[k].z+=j*PIECESIZE;
	}
	setmodelbbox(p->mod); */
	p->trans.x+=i*PIECESIZE;
	p->trans.z+=j*PIECESIZE;
}

tree2* buildconsscene(track *t)
{
	tree2* ret,*pce;
//	char str[50];
//	perf_start(BUILDCONSSCENE);
pushandsetdirdown("constructed");
//	ret=alloctree(TRACKX*TRACKZ,0);
	ret=new tree2("consscene");
//	pointf3 offverts[4];
	int i,j;//,k;
	for (j=0;j<TRACKZ;j++)
		for (i=0;i<TRACKX;i++) {
//			t->pieces[j][i].pt=PCE_RIGHTFORK;
			pce=buildconspiece(t->trkdata.basetex,&t->trkdata.pieces[j][i]);
			adjustpiece(pce,i,j);
/*			sprintf(str,"tpce_%d_%d.blt",i,j);
			buildmodelstart(str);
			if (video_maindriver==VIDEO_D3D)
				buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_CALCLIGHTS,
					basetexnames[t->basetex],0,"basemat",0);
			else
				buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV,
					basetexnames[t->basetex],0,"basemat",0);
			buildmodeladduvs(offverts,baseuvs,4);
			m->addface(1,0,2);
			m->addface(1,2,3);
			pce=alloctreebuildmodelend(0); */
//			linkchildtoparent(pce,ret);
			ret->linkchild(pce);
			t->treedata[j][i]=pce;
		}
//	calclightonce(ret);
//	perf_end(BUILDCONSSCENE);
popdir();
	return ret;
}

static void updatebasetexres()
{
//	setresname(rl,TEXTBASETEX,basetexnames[thetrack.trkdata.basetex]);
	tbasetex->settname(basetexnames[thetrack.trkdata.basetex]);
}

int loadtrackscript(const char *trackname,trk *tk)
{
	char str[100];
	if (tk)
		memset(tk,0,sizeof(*tk));
	logger("sizeof tk is %d\n",sizeof(*tk));
	sprintf(str,"%s.trk",trackname);
pushandsetdirdown("constructed");
	if (fileexist(str)) {
//		char **sc;
//		int nsc;
		script sc(str);
//		sc=loadscript(str,&nsc);

		if (strcmp("oldtrack",sc.idx(0).c_str())) {
//			freescript(sc,nsc);
popdir();
			return 0;
		}
		if (!tk) {
//			freescript(sc,nsc);
popdir();
			return 1;
		}
		int i,j,nsc=sc.num();
		for (i=1;i<nsc;i++) {
			if (!strcmp("basetex",sc.idx(i).c_str())) {
				for (j=0;j<(S32)NBASETEXT;j++)
					if (!strcmp(basetexnames[j],sc.idx(i+1).c_str()))
						break;
				if (j==NBASETEXT)
					j=0;
				tk->basetex=j;
				i++;
			} else if (!strcmp("pieces",sc.idx(i).c_str())) {
				for (j=0;j<TRACKX*TRACKZ;j++) {
					tk->pieces[0][j].pt=piecetype(atoi(sc.idx(i+1+2*j).c_str()));
					tk->pieces[0][j].rot=atoi(sc.idx(i+1+1+2*j).c_str());
				}
				i+=TRACKX*TRACKZ*2;
			}
		}
//		freescript(sc,nsc);
popdir();
		return 1;
	}
popdir();
	return 0;
}

static tree2* loadconstrack(const char *trackname)
{
	trackpiece=0;
	memset(&cpce,0,sizeof(cpce));
	loadtrackscript(trackname,&thetrack.trkdata);
	return buildconsscene(&thetrack);
}

const C8* saveconstrack(const char *name,trk *td,int u)
{
	int i,j;
	FILE *fw;
	char str[70];
	const C8 *uniq=0;
//	mystrncpy(lobby_track,getresname(rl,TEXTTRACK),NAMESIZE);
pushandsetdirdown("constructed");
	if (u) {
		uniq=uniquetrackname(name);
		sprintf(str,"%s.trk",uniq);
	} else
		sprintf(str,"%s.trk",name);
	fw=fopen2(str,"w");
	fprintf(fw,"oldtrack\n");
	fprintf(fw,"basetex \"%s\"\n",basetexnames[td->basetex]);
	fprintf(fw,"pieces\n");
	for (j=0;j<TRACKZ;j++) {
		for (i=0;i<TRACKX;i++)
			fprintf(fw,"%2d %2d    ",td->pieces[j][i].pt,td->pieces[j][i].rot);
		fprintf(fw,"\n");
	}
	fclose(fw);
popdir();
	return uniq;
}

static void rebuildpiece()
{
	if (trackpiece) {
//		unhooktree(trackpiece); // force immediate free
//		freetree(trackpiece);
		delete trackpiece;
	}
pushandsetdirdown("constructed");
	trackpiece=buildconspiece(thetrack.trkdata.basetex,&cpce);	
popdir();
//	linkchildtoparent(trackpiece,constructor_viewport2.roottree);
	constructor_viewport2_roottree->linkchild(trackpiece);
}

static void rebuildtrack()
{
	if (trackscene) {
//		unhooktree(trackscene); // force immediate free
//		freetree(trackscene);
		delete trackscene;
	}
	trackscene=buildconsscene(&thetrack);	
//	linkchildtoparent(trackscene,constructor_viewport.roottree);
	constructor_viewport_roottree->linkchild(trackscene);
}

static void rebuildtrackandpiece()
{
	rebuildtrack();
	rebuildpiece();
}

static void cleartrack()
{
	int i,j;
	for (j=0;j<TRACKZ;j++)
		for (i=0;i<TRACKX;i++) {
			thetrack.trkdata.pieces[j][i].rot=0;
			thetrack.trkdata.pieces[j][i].pt=PCE_BLANK;
		}
	rebuildtrack();
}

void constructorinit()
{
pushandsetdir("engine7testdata");
	int i;
//	char **sc;
//	int nsc;
//	perf_start(TEST1);
	consmode=MODE_NORMAL;
//	struct tree *rt;//,*scn,*obj;
	logger("---------------------- constructor init -------------------------------\n");
//	perf_end(TEST1);
// video
//	perf_start(TEST2);
	video_setupwindow(800,600);//,565);
	MX=MY=0; // hack to get the cursor off of the track initialy
//	perf_end(TEST2);
// setup video
//	load scene
/*	if (video_maindriver==VIDEO_MAINDRIVER_D3D) {
		uselights=1;
		usescnlights=1;
		dodefaultlights=1;
	} else {
		uselights=0;
		usescnlights=0;
	} */
//	perf_start(TEST3);
// setup viewport
/*	constructor_viewportm.backcolor=frgbblue;
	constructor_viewportm.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	constructor_viewportm.roottree=0;
	constructor_viewportm.xstart=0;
	constructor_viewportm.ystart=0;
	constructor_viewportm.xres=WX;
	constructor_viewportm.yres=WY;
	constructor_viewportm.zfront=.1f;
	constructor_viewportm.zback=2000;
	constructor_viewportm.camattach=0;//getlastcam(); */
	constructor_viewport.backcolor=C32GREEN;
	constructor_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	rt=alloctree(10000,0);
//	mystrncpy(rt->name,"roottree",NAMESIZE);
//	constructor_viewport.roottree=rt;
	constructor_viewport_roottree=new tree2("roottree");
	constructor_viewport.xstart=TRACKVIEWX;
	constructor_viewport.ystart=TRACKVIEWY;
	constructor_viewport.xres=TRACKVIEWW;
	constructor_viewport.yres=TRACKVIEWH;
	constructor_viewport.zfront=10;
	constructor_viewport.zback=2000;
//	constructor_viewport.camattach=0;//getlastcam();
	constructor_viewport2.backcolor=C32BROWN;//frgbbrown;
	constructor_viewport2.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	rt=alloctree(10000,0);
//	mystrncpy(rt->name,"roottree",NAMESIZE);
//	constructor_viewport2.roottree=rt;
	constructor_viewport2_roottree=new tree2("roottree2");
	constructor_viewport2.xstart=TRACKVIEWX2;
	constructor_viewport2.ystart=TRACKVIEWY2;
	constructor_viewport2.xres=TRACKVIEWW2;
	constructor_viewport2.yres=TRACKVIEWH2;
	constructor_viewport2.zfront=.1f;
	constructor_viewport2.zback=2000;
//	constructor_viewport2.camattach=0;//getlastcam();
//	vp.camattach->camtarget=mt;
//	setviewportsrc(&constructor_viewport); // user calls this
	constructor_viewport.xsrc=4;constructor_viewport.ysrc=3;
//	constructor_viewport.lookat=0;
	constructor_viewport.useattachcam=false;
	constructor_viewport2.xsrc=4;constructor_viewport2.ysrc=3;
	constructor_viewport2.useattachcam=false;
//	constructor_viewport2.lookat=0;
//	setviewportsrc(&constructor_viewport2); // user calls this
// resource
pushandsetdirdown("constructed");
//	rl=loadres("constructorres.txt");
//	rlnn=loadres("newnameres.txt");
	factory2<shape> fact;
	script* msc=new script("constructorres.txt");
	rl=fact.newclass_from_handle(*msc);
	delete msc;
	msc=new script("newnameres.txt");
	rlnn=fact.newclass_from_handle(*msc);
	delete msc;

	ttrack=rl->find<text>("TEXTTRACK");
	pbacksave=rl->find<pbut>("PBUTBACKSAVE");
	pbacknosave=rl->find<pbut>("PBUTBACKNOSAVE");
	pbackdelete=rl->find<pbut>("PBUTBACKDELETE");
	pcopy=rl->find<pbut>("PBUTCOPY");
	pclear=rl->find<pbut>("PBUTCLEAR");
	tbasetex=rl->find<text>("TEXTBASETEX");
	vbasetex=rl->find<vscroll>("VSLIDERBASETEX");
	lpiecetypes=rl->find<listbox>("LISTPIECETYPES");
	protateleft=rl->find<pbut>("PBUTROTATELEFT");
	protateright=rl->find<pbut>("PBUTROTATERIGHT");

	ename=rlnn->find<edit>("EDITNAME");
	enameok=rlnn->find<pbut>("PBUTNAMEOK");
	enamecan=rlnn->find<pbut>("PBUTNAMECAN");

//	if (!lobby_track[0])
	if (!lobby_track.size())
//		strcpy(lobby_track,"default");
		lobby_track="default";
//	setresname(rl,TEXTTRACK,lobby_track); 
	ttrack->settname(lobby_track.c_str());
	inittracklist();
//	perf_end(TEST3);
	trackscene=loadconstrack(lobby_track.c_str());
//	linkchildtoparent(trackscene,constructor_viewport.roottree);
	constructor_viewport_roottree->linkchild(trackscene);
	trackpiece=buildconspiece(thetrack.trkdata.basetex,&cpce);
//	linkchildtoparent(trackpiece,constructor_viewport2.roottree);
	constructor_viewport2_roottree->linkchild(trackpiece);
	buildtrackcursor();
	updatebasetexres();
//	setresrange(rl,VSLIDERBASETEX,0,NBASETEXT-1);
	vbasetex->setnumidx(NBASETEXT);
//	setresval(rl,VSLIDERBASETEX,thetrack.trkdata.basetex);
	vbasetex->setidx(thetrack.trkdata.basetex);
	for (i=0;i<(S32)NPIECETYPES;i++)
//		addlistboxname(rl,LISTPIECETYPES,-1,tracktypenames[i],-1);
		lpiecetypes->addstring(tracktypenames[i]);
//	setresval(rl,LISTPIECETYPES,0);
	lpiecetypes->setidxc(0);
	if (tracklistnum()<=1)
//		setresvis(rl,PBUTBACKDELETE,0);
	pbackdelete->setvis(0);
	focus=0;
popdir();
	extradebvars(constructorvars,nconstructorvars);
}

void constructorproc()
{
//	int i;
	char str[70];
//	struct rmessage rm;
	switch(consmode) {
	case MODE_NORMAL:
		if (trkcur.valid) {
			if (wininfo.mleftclicks) { // drop
				tree2* *pt;
				struct piece *p;
				p=&thetrack.trkdata.pieces[trkcur.z][trkcur.x];
				pt=&thetrack.treedata[trkcur.z][trkcur.x];
				p->pt=cpce.pt;
				p->rot=cpce.rot;
//				unhooktree(*pt);
//				freetree(*pt);
				delete *pt;
				*pt=buildconspiece(thetrack.trkdata.basetex,p);
				adjustpiece(*pt,trkcur.x,trkcur.z);
//				linkchildtoparent(*pt,trackscene);
				trackscene->linkchild(*pt);
			}
			if (wininfo.mrightclicks) { // pick up
				struct piece *p;
				p=&thetrack.trkdata.pieces[trkcur.z][trkcur.x];
				cpce.pt=p->pt;
				cpce.rot=p->rot;
				rebuildpiece();
			}
		}
		if (wininfo.mleftclicks)
			focus=rl->getfocus();
		if (focus) {
			S32 ret=focus->proc();
			shape* focusb = ret ? focus : 0;
			shape* focush = ret>=0 ? focus : 0;
			shape* focusl = ret>=0 ? focus : 0;
//			shape* focuse = ret>=0 ? focus : 0;
//		checkres(rl);
//		while(getresmess(rl,&rm)) {
//			switch(rm.id) {
// quit
			if (focusb==pbacksave) {
//			case PBUTBACKSAVE:
//				mystrncpy(lobby_track,getresname(rl,TEXTTRACK),NAMESIZE);
				lobby_track=ttrack->gettname();
				saveconstrack(lobby_track.c_str(),&thetrack.trkdata,0);
				popstate();
//				break;
//			case PBUTBACKNOSAVE:
			} else if (focusb==pbacknosave) {
				popstate();
//				break;
//			case PBUTBACKDELETE:
			} else if (focusb==pbackdelete) {
				sprintf(str,"%s.trk",lobby_track.c_str());
pushandsetdirdown("constructed");
				remove(str);
popdir();
				popstate();
//				break;
// copy
//			case PBUTCOPY:
			} else if (focusb==pcopy) {
				consmode=MODE_NEWNAME;
				focus=ename;
//				settextfocus(rlnn,EDITNAME);
//				setresname(rlnn,EDITNAME,uniquetrackname(getresname(rl,TEXTTRACK)));
				const C8* un;
				un=uniquetrackname(ttrack->gettname());
				ename->settname(un);
//				setresname(rlnn,EDITNAME,"newname");
//				break;
// clear
//			case PBUTCLEAR:
			} else if (focusb==pclear) {
				cleartrack();
				rebuildtrackandpiece();
//				break;
// change basetex
//			case VSLIDERBASETEX:
			} else if (focush==vbasetex) {
//				thetrack.trkdata.basetex=rm.val;
				thetrack.trkdata.basetex=vbasetex->getidx();
				rebuildtrackandpiece();
				updatebasetexres();
//				break;
//			case LISTPIECETYPES:
			} else if (focusl==lpiecetypes) {
//				cpce.pt=piecetype(rm.val);
				cpce.pt=piecetype(lpiecetypes->getidx());
				rebuildpiece();
//				break;
//			case PBUTROTATELEFT:
			} else if (focusb==protateleft) {
				cpce.rot--;
				cpce.rot&=3;
				rebuildpiece();
//				break;
//			case PBUTROTATERIGHT:
			} else if (focusb==protateright) {
				cpce.rot++;
				cpce.rot&=3;
				rebuildpiece();
//				break;
			}
		}
		break;
	case MODE_NEWNAME:
//		checkres(rlnn);
//		while(getresmess(rlnn,&rm)) {
//			switch(rm.id) {
		if (wininfo.mleftclicks)
			focus=rlnn->getfocus();
		if (focus) {
			S32 ret=focus->proc();
			shape* focusb = ret ? focus : 0;
//			shape* focush = ret>=0 ? focus : 0;
//			shape* focusl = ret>=0 ? focus : 0;
			shape* focuse = ret>=0 ? focus : 0;
	// pick a new name
//				case EDITNAME:
			if (focuse==ename || focusb==enameok) {
//				setresname(rlnn,EDITNAME,uniquetrackname(getresname(rlnn,EDITNAME)));
				const C8* un=uniquetrackname(ename->gettname());
				ename->settname(un);
/*				for (i=0;i<ntrack_sc;i++)
					if (!my_stricmp(track_sc[i],getresname(rlnn,EDITNAME)))
						break;
				if (i==ntrack_sc) { // got a different name */
				consmode=MODE_NORMAL;
				focus=0;
//					setresname(rl,TEXTTRACK,getresname(rlnn,EDITNAME));
				ttrack->settname(un);
/*				} else {
					setresname(rlnn,EDITNAME,"name taken");
				} */
//				break;
			} else if (focusb==enamecan) {
				consmode=MODE_NORMAL;
				focus=0;
			}
//			break;
		}
	}
//	calcgrid(&constructor_viewport2,MX,MY,&grid);
// update display
	updatecursor();
	doflycam(&constructor_viewport);
	doflycam(&constructor_viewport2);
//	doanims(constructor_viewport.roottree);
	constructor_viewport_roottree->proc();
//	buildtreematrices(constructor_viewport.roottree);
	video_buildworldmats(constructor_viewport_roottree);
//	doanims(constructor_viewport2.roottree);
	constructor_viewport2_roottree->proc();
//	buildtreematrices(constructor_viewport2.roottree);
	video_buildworldmats(constructor_viewport2_roottree);
}

void constructordraw2d()
{
//	video_lock();
	switch(consmode) {
	case MODE_NORMAL:
		rl->draw();
//		drawres(rl);
		break;
	case MODE_NEWNAME:
//		drawres(rlnn);
		rlnn->draw();
		break;
	}
// draw fancy track pic
//	cliprecto16(B16,TRACKPICX-2,TRACKPICY-2,TRACKPICX+TRACKPICW+1,TRACKPICY+TRACKPICH+1,hiwhite);
//	cliprecto16(B16,TRACKPICX-1,TRACKPICY-1,TRACKPICX+TRACKPICW,TRACKPICY+TRACKPICH,hiblack);
//	clipblit16(trackpic,B16,0,0,TRACKPICX,TRACKPICY,TRACKPICW,TRACKPICH);
	if (calcedgrid)
		outtextxyf32(B32,150,400,C32WHITE,"%f, %f, %f",grid.x,grid.y,grid.z);
//	video_unlock();
}

void constructordraw3d()
{
//	video_beginscene(&constructor_viewport);
//	video_drawscene(&constructor_viewport);
//	video_endscene(&constructor_viewport);
//	video_beginscene(&constructor_viewport2);
//	video_drawscene(&constructor_viewport2);
//	video_endscene(&constructor_viewport2);


	constructor_viewport.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	constructor_viewport.xres=WX;
	constructor_viewport.yres=WY;
	constructor_viewport.xstart=0;
	constructor_viewport.ystart=0;
	constructor_viewport.backcolor=C32(0,85,0);
	video_setviewport(&constructor_viewport); // clear zbuf etc.

	constructor_viewport.flags=VP_CHECKER|VP_CLEARBG;
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
//	logviewport(&constructor_viewport,OPT_SOME);
//	logviewport(&constructor_viewport2,OPT_SOME);
	constructor_viewport_roottree->log2();
	constructor_viewport2_roottree->log2();
	logrc();
// free everything
	extradebvars(0,0);
//	freeres(rl);
//	freeres(rlnn);
	delete rl;
	delete rlnn;
//	freetree(constructor_viewport.roottree);
	delete constructor_viewport_roottree;
	constructor_viewport_roottree=0;
//	freetree(constructor_viewport2.roottree);
	delete constructor_viewport2_roottree;
	constructor_viewport2_roottree=0;
//	freescript(track_sc,ntrack_sc);
	freetracklist();
popdir();
}
