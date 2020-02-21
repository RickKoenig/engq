// draw the mainmenu's shape class without doing anything else
#define D2_3D
#include <m_eng.h>

#include "m_perf.h"
#include <l_misclibm.h>
#include "u_gametheory.h"

#include "u_states.h"
#include "u_groupanalyze.h"
#include "u_cayley.h"

namespace {
/*#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <engine1.h>
#include <misclib.h>*/
//#include "rcube.h"
/*
#define MOVEU 0
#define MOVEUI 1
#define MOVED 2
#define MOVEDI 3
#define MOVEF 4
#define MOVEFI 5
#define MOVEB 6
#define MOVEBI 7
#define MOVER 8
#define MOVERI 9
#define MOVEL 10
#define MOVELI 11
#define QUIT1 12
#define LOAD1 13
#define LOAD2 14
#define LOAD3 15
#define LOAD4 16
#define LOAD5 17
#define LOAD6 18
#define SAVE1 19
#define SAVE2 20
#define SAVE3 21
#define SAVE4 22
#define SAVE5 23
#define SAVE6 24
#define RESET 25
#define SCRAMBLE 26
#define ROTCW 27
#define ROTCCW 28
#define R1 29
#define R2 30
#define R3 31
#define R4 32
#define R5 33
#define R6 34
#define R7 35
#define R8 36
#define R9 37
#define R10 38
#define P1 39
#define P2 40
#define P3 41
#define P4 42
#define P5 43
#define P6 44
#define P7 45
#define P8 46
#define P9 47
#define P10 48
#define RECDONE 49
*/

//#include "globalres.h"

//extern int dodefaultlights; // from engine.h (sigh)
//extern float flycamspeed;
//extern struct viewport2 mainvp;
viewport2 vp1,vp2;

tree2 *roottree;
tree2 *cube,*cube2;
shape* rl;
shape* focus;
pbut* bquit;

S32 analyzed;

struct faceinfo {
	void (*func)();
	C32 col32;	// color of each face		white,green,blue,yellow,red,orange
	pointf3 colf;
};

pointf2 faceverts[4]={
	{-1, 1},
	{ 1, 1},
	{-1,-1},
	{ 1,-1},
};
pointf3 cubeverts[8];

S32 cubelocs[3][27];

enum {F_UP,F_DOWN,F_FRONT,F_BACK,F_RIGHT,F_LEFT};

#define MAXQUEUE 4
char movequeue[MAXQUEUE];
int queuetop;
int queuenum;
int cubemoving;
int curmove;
pointf3 cubedirs[12]={ // indexed by move
	{ 0, 1, 0},
	{ 0,-1, 0},
	{ 0,-1, 0},
	{ 0, 1, 0},
	{ 0, 0,-1},
	{ 0, 0, 1},
	{ 0, 0, 1},
	{ 0, 0,-1},
	{ 1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{ 1, 0, 0},
};

/*static int movelists[6][9]={ // indexed by face
	 6, 7, 8,15,16,17,24,25,26,
	 0, 1, 2, 9,10,11,18,19,20,
	 0, 1, 2, 3, 4, 5, 6, 7, 8,
	18,19,20,21,22,23,24,25,26,
	 2, 5, 8,11,14,17,20,23,26,
	 0, 3, 6, 9,12,15,18,21,24,
};
*/
struct ml {
	int index,val;
};

ml newmovelists[6]={
	{1, 1},
	{1,-1},
	{2,-1},
	{2, 1},
	{0, 1},
	{0,-1},
};

#define MAXMOVE 8
//pointf3 movequats[6][MAXMOVE];

void initmovequats()
{
	curmove=-1;
}

void addmove(int move)
{
	curmove=move;
}

void fixuploc(int i,int xyz,float ccw)
{
	int a,b;
	int fi,ti;
	if (ccw<0) {
		fi=(xyz+1)%3;
		ti=(xyz+2)%3;
	} else {
		fi=(xyz+2)%3;
		ti=(xyz+1)%3;
	}
	a=cubelocs[fi][i];
	b=cubelocs[ti][i];
	cubelocs[fi][i]=b;
	cubelocs[ti][i]=-a;
}

void procmove()
{
	tree2* t;
	pointf3 *d,q;
	int i,cf,idx,val;
	float ccw;
	if (curmove<0)
		return;
	d=&cubedirs[curmove];
	q=*d;
	q.w=PI/2/MAXMOVE;
	rotaxis2quat(&q,&q);
	cf=curmove>>1;
	ccw=cubedirs[curmove].x+cubedirs[curmove].y+cubedirs[curmove].z;
	idx=newmovelists[cf].index; // x,y or z
	val=newmovelists[cf].val;
	list<tree2*>::iterator it=cube->children.begin();
	for (i=0;i<27;i++,++it) {
		if (cubelocs[idx][i]==val) {
//			t=cube->children[i];
			t=*it;
			quattimes(&q,&t->rot,&t->rot);
			fixuploc(i,idx,ccw);
		}
	}
	cubemoving++;
	if (cubemoving==MAXMOVE) {
		for (i=0;i<27;i++)
			if (cubelocs[idx][i]==val)
				fixuploc(i,idx,ccw);
		cubemoving=0;
		curmove=-1;
	}
}

void initqueue()
{
	queuetop=queuenum=0;
	cubemoving=0;
}

void queuepush(int val)
{
	if (queuenum>=MAXQUEUE)
		return;
//		errorexit("queue too big");
	movequeue[(queuetop+queuenum)%MAXQUEUE]=val;
	queuenum++;
}

int queuepop()
{
	int val;
	if (queuenum<=0)
		return -1;
	val=movequeue[queuetop];
	queuenum--;
	queuetop++;
	queuetop%=MAXQUEUE;
	return val;
}

void makeup()
{
	int i;
	for (i=0;i<4;i++) {
		cubeverts[i].z=faceverts[i].y;
		cubeverts[i].x=faceverts[i].x;
		cubeverts[i].y=1;
		cubeverts[i+4].x=.9f*cubeverts[i].x;
		cubeverts[i+4].y=1;
		cubeverts[i+4].z=.9f*cubeverts[i].z;
	}
}

void makedown()
{
	int i;
	for (i=0;i<4;i++) {
		cubeverts[i].z=-faceverts[i].y;
		cubeverts[i].x=faceverts[i].x;
		cubeverts[i].y=-1;
		cubeverts[i+4].x=.9f*cubeverts[i].x;
		cubeverts[i+4].y=-1;
		cubeverts[i+4].z=.9f*cubeverts[i].z;
	}
}

void makefront()
{
	int i;
	for (i=0;i<4;i++) {
		cubeverts[i].z=-1;
		cubeverts[i].x=faceverts[i].x;
		cubeverts[i].y=faceverts[i].y;
		cubeverts[i+4].x=.9f*cubeverts[i].x;
		cubeverts[i+4].y=.9f*cubeverts[i].y;
		cubeverts[i+4].z=-1;
	}
}

void makeback()
{
	int i;
	for (i=0;i<4;i++) {
		cubeverts[i].z=1;
		cubeverts[i].x=-faceverts[i].x;
		cubeverts[i].y=faceverts[i].y;
		cubeverts[i+4].x=.9f*cubeverts[i].x;
		cubeverts[i+4].y=.9f*cubeverts[i].y;
		cubeverts[i+4].z=1;
	}
}

void makeright()
{
	int i;
	for (i=0;i<4;i++) {
		cubeverts[i].z=faceverts[i].x;
		cubeverts[i].x=1;
		cubeverts[i].y=faceverts[i].y;
		cubeverts[i+4].x=1;
		cubeverts[i+4].y=.9f*cubeverts[i].y;
		cubeverts[i+4].z=.9f*cubeverts[i].z;
	}
}

void makeleft()
{
	int i;
	for (i=0;i<4;i++) {
		cubeverts[i].z=-faceverts[i].x;
		cubeverts[i].x=-1;
		cubeverts[i].y=faceverts[i].y;
		cubeverts[i+4].x=-1;
		cubeverts[i+4].y=.9f*cubeverts[i].y;
		cubeverts[i+4].z=.9f*cubeverts[i].z;
	}
}

struct faceinfo cfaces[6]={
	{makeup,},
	{makedown,},
	{makefront,},
	{makeback,},
	{makeright,},
	{makeleft,},
};

void cubies_init()
{
	char str[16];
	C32 facedacs[256];
	tree2* cubi;
	pointf3 off;
	float sep=2.0f;
// copy over some face colors
	struct bitmap8 *dum8;
	int i,j,k;
	int x,y,z;
//	cube=alloctree(27,NULL);
//	mystrncpy(cube->name,"thecube",NAMESIZE);
	cube=new tree2("thecube");
	pushandsetdir("grouptheory");
//	fileopen("facecol.pcx",READ);
//	dum8=gfxread8(facedacs);
//	fileclose();
	dum8=gfxread8("facecol.pcx",facedacs);
	popdir();
	bitmap8free(dum8);
	for (i=0;i<6;i++) {
//		faces[i].col16bit=color24to16(facedacs[i],BPP);
//		faces[i].colf=rgb2float(facedacs[i]);
		cfaces[i].col32=facedacs[i];
		cfaces[i].colf=C32topointf3(facedacs[i]);
	}
// build a cubi
	k=0;
	for (z=0;z<3;z++) {
		off.z=(z-1)*sep;
		for (y=0;y<3;y++) {
			off.y=(y-1)*sep;
			for (x=0;x<3;x++) {
				off.x=(x-1)*sep;
				sprintf(str,"cubi%d%d%d",z,y,x);
				cubelocs[0][k]=x-1;
				cubelocs[1][k]=y-1;
				cubelocs[2][k]=z-1;
//				buildmodelstart(str);
				modelb* m=model_create(str);
				if (modelb::rc.getrc(m)==1) {
					vector<pointf3> mv(72);
//					S32 mvi=0;
					for (i=0;i<6;i++) {
						(*cfaces[i].func)();
						S32 ti=12*i;
						for (j=0;j<8;j++) {
							cubeverts[j].x+=off.x;
							cubeverts[j].y+=off.y;
							cubeverts[j].z+=off.z;
						}
//						buildmodeladdflat(cubeverts,8);
						copy(cubeverts+4,cubeverts+8,&mv[ti]);
						copy(cubeverts,cubeverts+8,&mv[ti+4]);
						sprintf(str,"sticker%d",i);
						if (z==0 && i==F_FRONT || z==2 && i==F_BACK ||
							x==0 && i==F_LEFT || x==2 && i==F_RIGHT ||
							y==0 && i==F_DOWN || y==2 && i==F_UP )
//							buildmodelsetmaterial(SMAT_HASWBUFF,NULL,NULL,str,&faces[i].colf);
//							m->addmat(str,SMAT_HASWBUFF,0,&F32YELLOW,8,2,4);
							m->addmat(str,SMAT_HASWBUFF,0,&cfaces[i].colf,8,2,4);
						else
//							buildmodelsetmaterial(SMAT_HASWBUFF,NULL,NULL,str,&zerov);
							m->addmat(str,SMAT_HASWBUFF,0,&F32BLACK,8,2,4);
						m->addface(ti+0,ti+1,ti+2);
						m->addface(ti+3,ti+2,ti+1);
						sprintf(str,"backing%d",i);
//						buildmodelsetmaterial(SMAT_HASWBUFF,NULL,NULL,str,&zerov);
						m->addmat(str,SMAT_HASWBUFF,0,&F32BLACK,8,8,8);
						m->addface(ti+5+4,ti+0+4,ti+1+4);
						m->addface(ti+0+4,ti+5+4,ti+4+4);
						m->addface(ti+7+4,ti+1+4,ti+3+4);
						m->addface(ti+1+4,ti+7+4,ti+5+4);
						m->addface(ti+6+4,ti+3+4,ti+2+4);
						m->addface(ti+3+4,ti+6+4,ti+7+4);
						m->addface(ti+4+4,ti+2+4,ti+0+4);
						m->addface(ti+2+4,ti+4+4,ti+6+4);
					}
					m->copyverts(mv);
					m->close();
				}
//				cubi=alloctreebuildmodelend(0);
				cubi=new tree2("cubi");
				cubi->buildo2p=O2P_FROMTRANSQUATSCALE;
				cubi->rot.w=1;
				cubi->setmodel(m);
//				cubi->rotvel.y=TWOPI/30.0f/4;
//				cubi->rotvel.x=TWOPI/30.0f/64;
//				linkchildtoparent(cubi,cube);
				cube->linkchild(cubi);
				k++;
			}
		}
	}
//	cube2=alloctree(1,NULL);
	cube2=new tree2("");
//	linkchildtoparent(cube,cube2);
	cube2->linkchild(cube);
//	linkchildtoparent(cube2,roottree);
	roottree->linkchild(cube2);
//	cube->buildo2p=O2P_FROMTRANSROTSCALE; // it's on by default
//	cube->rot.w=1;
	cube2->rot.x=-PI/6;
	cube->rot.y=PI/6;
	analyzed=0;
}

static void viewcube()
{
	static int lx,ly;
	static int lastbut;
	static pointf3 lq;
	pointf3 q;
	if (MBUT && 4*MX<3*WX && 4*MY<3*WY) {
		if (!lastbut) {
			lx=3*WX/8;
			ly=3*WY/8;
			lq.y=0;//cube->rot.y;
			lq.x=0;//cube2->rot.x;
		}
		q.y=(float)(lx-MX)*2*TWOPI/WY;
		q.x=(float)(ly-MY)*2*TWOPI/WY;
//		q.z=0;
		cube->rot.y=lq.y+q.y;
		cube2->rot.x=lq.x+q.x;
		cube->rot.y=snormalangrad(cube->rot.y);
		cube2->rot.x=snormalangrad(cube2->rot.x);
	}
	lastbut=MBUT;
}


} // end namespace

void rcubeinit()
{
//	struct tree2 *scn,*lit;
// setup video bitmap
	video_setupwindow(800,600);
// roottree
//	roottree=alloctree(500,NULL);
//	mystrncpy(roottree->name,"roottree",NAMESIZE);
	roottree=new tree2("roottree");
// init cube
	cubies_init();
// init moves
	initqueue();
	initmovequats();
// load a scene of different objects
/*	pushandsetdir("starfield");
	usescnlights=0; // skip lights
	scn=loadscene("stars.mxs");
	usescnlights=1;
	popdir();
	linkchildtoparent(scn,roottree); */
// build a viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25;
	mainvp.zback=100000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
//	mainvp.roottree=roottree;
	mainvp.camattach=NULL; //getlastcam(); // get cam from last scene
	mainvp.camzoom=3.2f; // it'll getit from tree camattach if you have one
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.camtrans.z=-18;
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;

//	vp1.backcolor=frgblightgreen;
	vp1.zfront=.25;
	vp1.zback=100000;
	vp1.xres=WX*3/4;
	vp1.yres=WY*3/4;
	vp1.xstart=0;
	vp1.ystart=0;
//	vp1.roottree=roottree;
	vp1.camattach=NULL; //getlastcam(); // get cam from last scene
	vp1.camzoom=3.2f; // it'll getit from tree camattach if you have one
	vp1.xsrc=640;
	vp1.ysrc=480;
	vp1.camtrans.z=-18;
//	vp1.flags=VP_CLEARBG|VP_CLEARWB;

	vp2.backcolor=C32LIGHTGREEN;
	vp2.zfront=.25;
	vp2.zback=100000;
	vp2.xres=WX/4;
	vp2.yres=WY/4;
	vp2.xstart=3*WX/4;
	vp2.ystart=3*WY/4;
//	vp2.roottree=roottree;
	vp2.camattach=NULL; //getlastcam(); // get cam from last scene
	vp2.camzoom=3.2f; // it'll getit from tree camattach if you have one
	vp2.xsrc=640;
	vp2.ysrc=480;
	vp2.camtrans.z=18;
	vp2.camrot.y=PI;
	vp2.flags=VP_CLEARBG;
	treeinfo.flycamspeed=.125f;
// setup lights
/*	lit=alloctree(0,NULL);
	mystrncpy(lit->name,"alite",NAMESIZE);
	lit->flags|=TF_ISLIGHT;
	lit->lightcolor=frgbwhite;
	lit->intensity=.35f;
	linkchildtoparent(lit,roottree);
	addlighttolist(lit);
	lit=alloctree(0,NULL);
	mystrncpy(lit->name,"anamblite",NAMESIZE);
	lit->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	lit->lightcolor=frgbwhite;
	lit->intensity=.65f;
	linkchildtoparent(lit,roottree);
	addlighttolist(lit); */
//	pushandsetdir("grouptheory");
//	rl=loadres("rcube.txt");
//	popdir();
	factory2<shape> fact;
	pushandsetdir("grouptheory");
	script sc("rcube.txt");
	popdir();
	rl=fact.newclass_from_handle(sc);
/*	po=rl->find<text>("TEXTPO");
	pb=rl->find<pbut>("PBUTQUIT");
	hsd=rl->find<hscroll>("SCLHD");
	pra=rl->find<text>("TEXTPA");
	prb=rl->find<text>("TEXTPB"); */
	bquit=rl->find<pbut>("QUIT1");
	focus=0;
}

void rcubeproc()
{
	perf_start(RUBIPROC);
	int i;
//	struct rmessage rm;
//	checkres(rl);
//	while(getresmess(rl,&rm)) {
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (focus) {
		S32 ret=focus->proc();
		if (ret==1) {
			if (focus==bquit) {
				poporchangestate(STATE_MAINMENU);
			} else {
				S32 id=atoi(focus->getname());
				if (id>=0 && id<12)
					queuepush(id);
			}
		}
/*		switch(rm.id) {
		case QUIT1:
			popstate();
			break;
		}
		if (rm.id>=MOVEU && rm.id<=MOVELI)
			queuepush(rm.id); */
	}
	if (curmove<0) {
		i=queuepop();
		if (i>=0) {
			addmove(i);
		}
	}
	procmove();
// handle input
	viewcube(); // rotate cube with the mouse
// animate scene
//	doanims(vp1.roottree);
	roottree->proc();
// manipulate scene
// hi level debug camera
	doflycam(&vp1);
// prepare scene
//	buildtreematrices(vp1.roottree); //roottree,camtree);
	video_buildworldmats(roottree);
#if 1
	if (!analyzed) {
		perf_start(GANAL);
		groupanalyze();
//		docayley();
		analyzed=1;
//		popstate();
		perf_end(GANAL);
	}
#endif
	perf_end(RUBIPROC);
}

void rcubedraw3d()
{
// draw scene
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
	video_setviewport(&vp2); // clear zbuf etc.
	video_drawscene(roottree);
/*	video_beginscene(&mainvp);
	video_drawscene(&vp1);
	video_endscene(&mainvp); // nothing right now
	video_beginscene(&vp2);
	video_drawscene(&vp2);
	video_endscene(&vp2); */
}

void rcubedraw2d()
{
#if 1
	S32 i;
//	video_lock();
	for (i=0;i<6;i++)
		cliprect32(B32,30+40*i,30,30,30,cfaces[i].col32);
//	drawres(rl);
	rl->draw();
//	video_unlock();
	if (!analyzed)
		groupanalyzedraw();
//	popstate();
#endif
}

void rcubeexit()
{
// current scene logged at exit
//	logviewport(&vp1,OPT_SOME);
//	roottree->log2();
//	logrc();
//	logviewport(&mainvp,OPT_SOME|OPT_VERTNORMS|OPT_VERTS|OPT_FACES|OPT_UVS|OPT_CVERTS);
//	freetree(roottree);
	delete roottree;
//	freeres(rl);
	delete rl;
}

