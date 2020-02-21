#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <engine1.h>
#include <misclib.h>
#include "vidd3d.h"

#include "onlinecars.h"
///////////////// test state ...............
extern struct viewport mainvp;
#if 0
#include "debprint.h" // for loadconfigfile
#include "scrline.h" // for flattenmodel

static struct viewport mainvp2;
extern int dodefaultlights,video_maindriver;
static float testzval=240.0001f,lasttestzval,testu=0,testv=0;
static float testus=40,testvs=30;
static int shown=4,lastshown;
static struct menuvar oc_debvars[]={
	{"testzval",&testzval,D_FLOAT,FLOATUP/16},
	{"testu",&testu,D_FLOAT,FLOATUP/4},
	{"testv",&testv,D_FLOAT,FLOATUP/4},
	{"testus",&testus,D_FLOAT,FLOATUP},
	{"testvs",&testvs,D_FLOAT,FLOATUP},
	{"shown",&shown,D_INT,1},
};
#define OC_NDEBVARS (sizeof(oc_debvars)/sizeof(oc_debvars[0]))
static int scx,scy;
//static int mousemodesave;
static struct tree *thecars[16];
static int carorder[16]={
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
};
#define TESTNUMCARS 16

//static struct tree *scrlinescn;
char *scrlinenames[]={
	"scrlnebtm.mxs",
	"scrlnebtm2.mxs",
	"scrlnebtm3.mxs",
	"scrlnetop.mxs",
	"scrlnetop2.mxs",
	"scrlnetop3.mxs",
};
#define NSCRLINENAMES (sizeof(scrlinenames)/sizeof(scrlinenames[0]))

struct texture *getcartex(TREE *t);
void onlinecars_init()
{
	int i,j,h;
	char str[80];
	struct tree *roottree,*scene;//,*checkerscn;
// setup video bitmap
	video_setupwindow(640,480,565);
// roottree
	roottree=alloctree(7000,NULL);
//	scrlinescn=alloctree(20,NULL);
	mystrncpy(roottree->name,"roottree",NAMESIZE);
// load up background scene
	pushandsetdir("st2_edpieces");
//	if (video_maindriver==VIDEO_D3D)
		uselights=1;
//	else
//		uselights=0;
	usescnlights=0;
	scene=loadscene("tstraight.mxs");
	scene->rot.y=PI/2;
	usescnlights=0;
	usescncamera=0;
	dodefaultlights=1;
	linkchildtoparent(scene,roottree);
// load up scoreline
//	checkerscn=loadscene("checker1.mxs");
//	linkchildtoparent(checkerscn,scrlinescn);
	popdir();
//	usenewprs=1;
/*	pushandsetdir("maxscrline");
	for (i=0;i<NSCRLINENAMES;i++) {
		checkerscn=loadscene(scrlinenames[i]);
		linkchildtoparent(checkerscn,scrlinescn);
	}
	popdir(); */
	for (i=0;i<TESTNUMCARS;i++) {
		sprintf(str,"player%d",carorder[i]);
		thecars[i]=loadnewjrms(str,str);
		if (!thecars[i])
			errorexit("can't read thecars");
		getcartex(thecars[i]);
		linkchildtoparent(thecars[i],roottree);
		setVEC(&thecars[i]->scale,.0075f,.0075f,.0075f);
		j=i&3;
		h=i>>2;
		thecars[i]->trans.x=((float)j-1.5f)*.3f;
		thecars[i]->trans.y=0;
		thecars[i]->trans.z=-2-.5f*h;
		thecars[i]->rot.x=-PI/2;
	}
//	usenewprs=0;
// setup viewport
	mainvp.backcolor=frgbblue;
	mainvp.zfront=.125f;
	mainvp.zback=100000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.roottree=roottree;
	mainvp.camattach=NULL; //getlastcam();
	mainvp.camzoom=3.2f; // it'll getit from tree camattach if you have one
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	setVEC(&mainvp.camtrans,0,.6f,-4.7f);
	setVEC(&mainvp.camrot,.26f,0,0);
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;

/*	mainvp2.zfront=.125f;
	mainvp2.zback=100000;
	mainvp2.xres=WX;
	mainvp2.yres=WY;
	mainvp2.xstart=0;
	mainvp2.ystart=0;
	mainvp2.roottree=scrlinescn;
	mainvp2.camattach=NULL; //getlastcam();
	mainvp2.camzoom=1; // it'll getit from tree camattach if you have one
	mainvp2.xsrc=640;
	mainvp2.ysrc=480;
//	setVEC(&mainvp2.camtrans,0,0,-4.7f);
	lastshown=-1;
	lasttestzval=-1; */
}

void onlinecars_proc()
{
//	int perscorrectsave;
// handle input
	if (KEY==K_ESCAPE)
		popstate();
/*	if (shown!=lastshown || lasttestzval!=testzval) {
		flattenmodel(scrlinescn,testzval);
		lastshown=shown;
		lasttestzval=testzval;
	} */
// animate scene
	doanims(mainvp.roottree);
//	doanims(mainvp2.roottree);
// hi level camera
	doflycam(&mainvp);
	// prepare scene
	buildtreematrices(mainvp.roottree); //roottree,camtree);
//	buildtreematrices(mainvp2.roottree); //roottree,camtree);
// draw scene
	video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
	video_drawscene(&mainvp);
//	perscorrectsave=perscorrect;
//	perscorrect=0;
//	video_drawscene(&mainvp2);
//	perscorrect=perscorrectsave;
	video_endscene(&mainvp); // nothing right now
	extradebvars(oc_debvars,OC_NDEBVARS);
}

void onlinecars_exit()
{
	logviewport(&mainvp,OPT_SOME);
	freetree(mainvp.roottree);
//	freetree(mainvp2.roottree);
	extradebvars(NULL,0);
}
#endif


///////////////// generic test scene //////////////////////

static struct tree *roottree,*scene;
void testscene_init()
{
	extern int clipmap;
	extern float flycamspeed;
// setup video bitmap
//	video_setupwindow(1280,1024,565);
//	clipmap=CLIPMAP_COLORKEY;
//	clipmap=CLIPMAP_NONE;
	video_setupwindow(640,480,565);
// roottree
	roottree=alloctree(70,NULL);
	mystrncpy(roottree->name,"roottree",NAMESIZE);
// load up background scene
//	pushandsetdir("shinycars");
//	pushandsetdir("shinycars2");
	pushandsetdir("shinycars3");
//	pushandsetdir("4pl_tfish");
	uselights=0;
	usescnlights=0;
	usescncamera=1;
//	scene=loadscene("4pl_tfish_fs.mxs");
//	scene=loadscene("boxwhale1_fs.mxs");
//	scene=loadscene("lisaguetest.mxs");
//	scene=loadscene("zwritetest.mxs");
//	keepnorms=1; // in case lighting is off
//	scene=loadscene("mdlcrvulture23r.mxs");
//	scene=loadscene("speculartest.mxs");
//	scene=loadscene("dualtex1.mxs");
//	tmod=findtreenamerec(scene,"GeoSphere01.mxo");
//	if (tmod) {
//		addenvmap(tmod,"profits.tga",1);
//		addenvmap(tmod,"@chrome.jpg",0);
//	}
	scene=loadshinycar("mdlcrvulture.mxs");
	linkchildtoparent(scene,roottree);
	scene=loadnewjrms("player1","player1");
	linkchildtoparent(scene,roottree);
	scene->trans.x=.4f;

	pushandsetdir("shinycars2");
	scene=loadscene("asphalt1.mxs");
	linkchildtoparent(scene,roottree);
//	setVEC(&scene->scale,1/.007813f,1/.007813f,1/.007813f);
	popdir();

//	scene->rotvel.y=.01f;
//	scene=loadscene("boxwhale1_kf.mxs");
//	linkchildtoparent(scene,roottree);
//	scene->trans.x=50;
	popdir();
//	if (scene->mod) {
//		if (!scene->mod->vertnorms)
//			logger("no norms found in '%s'\n",scene->name);
//	}
// setup viewport
	mainvp.backcolor=frgblightblue;
	mainvp.zfront=.125f;
	mainvp.zback=100000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.roottree=roottree;
	mainvp.camattach=getlastcam();
	mainvp.camzoom=3.2f; // it'll getit from tree camattach if you have one
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;
//	uselights=1;
//	initvecs(roottree);
	if (!mainvp.camattach) {
		setVEC(&mainvp.camtrans,0,1,-2);
		setVEC(&mainvp.camrot,.4375,0,0);
	}
//	flycamspeed=.001f;
}

//static VEC loc1={10,0,0},loc2={0,10,0},loc3={0,0,10};
void testscene_proc()
{
// handle input
//	resetvecs();
	if (KEY==K_ESCAPE)
		popstate();
/*
	addline(&zerov,&loc1,rgbred);
	addline(&zerov,&loc2,rgbgreen);
	addline(&zerov,&loc3,rgbblue);
*/
// animate scene
	doanims(mainvp.roottree);
// hi level camera
	doflycam(&mainvp);
// prepare scene
	buildtreematrices(mainvp.roottree); //roottree,camtree);
// draw scene
	video_beginscene(&mainvp); // clear zbuf etc., this one clears zbuff, and will call Begin
	video_drawscene(&mainvp);
	video_endscene(&mainvp); // nothing right now
}

void testscene_exit()
{
	logviewport(&mainvp,OPT_SOME);
	freetree(mainvp.roottree);
}
