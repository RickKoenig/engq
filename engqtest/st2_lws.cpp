#include <math.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine1.h"
#include "perf.h"

/*
FirstFrame 30
LastFrame 30
PreviewFirstFrame 1
PreviewLastFrame 60

void closetokfile();
void getnextline();
char *getrestline();
*/
//int usescnlights=1,usescnbones=1,usescncamera=1;

static float screenx,screeny;
static float startframe,endframe;
enum {OBJ_BONE,OBJ_OBJ,OBJ_NONE,OBJ_LIGHT,OBJ_CAMERA};
static int doingobject;

#define MAXLWSTREES 1000
#define EXTRACHILDREN 10
static struct tree *lwstrees[MAXLWSTREES],*ct;
static int parentidx[MAXLWSTREES],parent2clws[MAXLWSTREES];
static int objnum;
static int nlwstrees;
static int curlwstree;
static int targetsource,targetdest;

static int checkenvelope(char *tok)
{
	int nchan;
	int nkeys;
	struct seq *s;
	int j;
	if (strcmp("(envelope)",tok))
		return 0;
	nchan=atoi(gettok());
	if (nchan!=1)
		errorexit("bad nchan for envelope");
	nkeys=atoi(gettok());
	s=&ct->seqs[0];
	s->keys[EXTRA]=(struct keyframe *)memalloc(sizeof(struct keyframe));
	s->keys[EXTRA]->refcount=1;
	s->keys[EXTRA]->nkeys=nkeys;
	s->keys[EXTRA]->endstate=END_STOP; // not used yet
	s->keys[EXTRA]->curkey=0;
	s->keys[EXTRA]->keys=(struct key *)memalloc(sizeof(struct key)*nkeys);
	for (j=0;j<nkeys;j++) {
		s->keys[EXTRA]->keys[j].value=(float)atof(gettok());
		s->keys[EXTRA]->keys[j].time=(float)atof(gettok());
		s->keys[EXTRA]->keys[j].linear=atoi(gettok());
		s->keys[EXTRA]->keys[j].tens=(float)atof(gettok());
		s->keys[EXTRA]->keys[j].cont=(float)atof(gettok());
		s->keys[EXTRA]->keys[j].bias=(float)atof(gettok());
	}
	tok=gettok();
	if (strcmp(tok,"EndBehavior"))
		errorexit("no endbehavior");
	j=atoi(gettok());
	s->keys[EXTRA]->endstate=j; // not used yet
	tcbcalcallcoeffs(s->keys[EXTRA]);
	ct->proc=tcbkeyframeproc;
	return 1;
}

///// parse functions /////
static void firstframe()
{
	startframe=(float)atof(gettok());
}

static void lastframe()
{
	endframe=(float)atof(gettok());
}

static void loadobject()
{
	char nameext[256];
	struct seq *s;
	doingobject=OBJ_OBJ;
	nlwstrees++;
	objnum++;
	parent2clws[objnum]=nlwstrees;
	getnameext(nameext,getrestline());
	ct=lwstrees[nlwstrees]=alloctree(0,nameext);
	ct->seqs=s=memzalloc(sizeof(struct seq));
	ct->nseq=1;
	s->startframe=startframe;
	ct->curframe=startframe;
	s->endframe=endframe;
	s->seqbuildo2p=O2P_FROMTRANSROTSCALE;
}

static void addnullobject()
{
	struct seq *s;
	doingobject=OBJ_OBJ;
	nlwstrees++;
	objnum++;
	parent2clws[objnum]=nlwstrees;
//	ct=lwstrees[nlwstrees]=alloctree(0,NULL);
	ct=lwstrees[nlwstrees]=allochelper(0,HELPER_NULL,0);
	ct->flags|=TF_ISNULL;
	ct->seqs=s=memzalloc(sizeof(struct seq));
	ct->nseq=1;
	s->startframe=startframe;
	ct->curframe=startframe;
	s->endframe=endframe;
	s->seqbuildo2p=O2P_FROMTRANSROTSCALE;
	mystrncpy(ct->name,getrestline(),NAMESIZE);
}

static void showobject()
{
	int show=atoi(gettok());
	if (!show)
		ct->flags|=TF_DONTDRAW;
	gettok();
}

static void objectmotion()
{
	char *tok;
	int i,j;
	int nchan;
	int nkeys;
	float framenumber;
	struct seq *s;
	int linear;
	float t,c,b;
	getrestline();
	if ((!usescnbones && doingobject==OBJ_BONE)||
		(!usescnlights && doingobject==OBJ_LIGHT)||
		(!usescncamera && doingobject==OBJ_CAMERA))
		return;
	nchan=atoi(gettok());
	if (nchan!=9)
		errorexit("bad nchan");
	nkeys=atoi(gettok());
	s=&ct->seqs[0];
	for (i=0;i<nchan;i++) {
		s->keys[i]=(struct keyframe *)memalloc(sizeof(struct keyframe));
		s->keys[i]->refcount=1;
		s->keys[i]->nkeys=nkeys;
		s->keys[i]->endstate=END_STOP; // not used yet
		s->keys[i]->curkey=0;
		s->keys[i]->keys=(struct key *)memalloc(sizeof(struct key)*nkeys);
	}
	for (j=0;j<nkeys;j++) {
		s->keys[TRANSX]->keys[j].value=(float)atof(gettok());
		s->keys[TRANSY]->keys[j].value=(float)atof(gettok());
		s->keys[TRANSZ]->keys[j].value=(float)atof(gettok());
		s->keys[ROTY]->keys[j].value=PIOVER180*(float)atof(gettok());
		s->keys[ROTX]->keys[j].value=PIOVER180*(float)atof(gettok());
		s->keys[ROTZ]->keys[j].value=PIOVER180*(float)atof(gettok());
		s->keys[SCLX]->keys[j].value=(float)atof(gettok());
		s->keys[SCLY]->keys[j].value=(float)atof(gettok());
		s->keys[SCLZ]->keys[j].value=(float)atof(gettok());
		framenumber=(float)atof(gettok());
		linear=atoi(gettok());
		t=(float)atof(gettok());
		c=(float)atof(gettok());
		b=(float)atof(gettok());
		for (i=0;i<nchan;i++) {
			s->keys[i]->keys[j].time=framenumber;
			s->keys[i]->keys[j].linear=linear;
			s->keys[i]->keys[j].tens=t;
			s->keys[i]->keys[j].cont=c;
			s->keys[i]->keys[j].bias=b;
		}
	}
	tok=gettok();
	if (strcmp(tok,"EndBehavior"))
		errorexit("no endbehavior");
	j=atoi(gettok());
	for (i=0;i<nchan;i++)
		s->keys[i]->endstate=j; // not used yet
	for (i=0;i<nchan;i++)
		tcbcalcallcoeffs(s->keys[i]);
	ct->proc=tcbkeyframeproc;
}

//static void endbehavior()
//{
//}

static void parentobject()
{
	int p=atoi(gettok());
//	lwstrees[nlwstrees]->parent=lwstrees[p];
	if (doingobject==OBJ_OBJ)
		parentidx[nlwstrees]=p; // fixup later when all objects found..
	else if (doingobject==OBJ_BONE) {
		if (usescnbones)
			parentidx[nlwstrees]=parent2clws[objnum]+p;
	} else if (doingobject==OBJ_LIGHT) {
		if (usescnlights)
			parentidx[nlwstrees]=p; // fixup later when all objects found..
	} else if (doingobject==OBJ_CAMERA) {
		if (usescncamera)
			parentidx[nlwstrees]=p; // fixup later when all objects found..
	}
}

static void  objdissolve()
{
	char *tok;
	tok=gettok();
	if (!checkenvelope(tok))
		ct->dissolve=1-(float)atof(tok);
}

static void customsize()
{
	screenx=(float)atof(gettok());
	screeny=(float)atof(gettok());
}

static char vbonename[256];
static int vboneactive;
static struct pointf3 vbonerestposition;
static struct pointf3 vbonerestdirection;
static float vbonerestlength;
static void bonename()
{
	doingobject=OBJ_BONE;
	getnameext(vbonename,getrestline());
}

static void boneactive()
{
	vboneactive=atoi(gettok());
}

static void bonerestposition()
{
	vbonerestposition.x=(float)atof(gettok());
	vbonerestposition.y=(float)atof(gettok());
	vbonerestposition.z=(float)atof(gettok());
}

static void bonerestdirection()
{
	vbonerestdirection.y=PIOVER180*(float)atof(gettok());
	vbonerestdirection.x=PIOVER180*(float)atof(gettok());
	vbonerestdirection.z=PIOVER180*(float)atof(gettok());
}

static void bonerestlength()
{
	struct seq *s;
	vbonerestlength=(float)atof(gettok());
	if (usescnbones) {
		nlwstrees++;
//		ct=lwstrees[nlwstrees]=alloctree(0,nameext);
		ct=lwstrees[nlwstrees]=allochelper(0,HELPER_BONE,vbonerestlength);
		ct->seqs=s=memzalloc(sizeof(struct seq));
		ct->nseq=1;
		s->startframe=startframe;
		ct->curframe=startframe;
		s->endframe=endframe;
		s->seqbuildo2p=O2P_FROMTRANSROTSCALE;
		mystrncpy(ct->name,vbonename,NAMESIZE);
		ct->flags|=TF_ISBONE;
		parentidx[nlwstrees]=parent2clws[objnum];
		lwstrees[parentidx[nlwstrees]]->flags|=TF_HASBONES;
		if (!vboneactive)
			ct->flags|=TF_NOINFLUENCE;
		ct->restpos=vbonerestposition;
		ct->restdir=vbonerestdirection;
		ct->restlen=vbonerestlength;
	}
}

static void showcamera()
{
	struct seq *s;
	doingobject=OBJ_CAMERA;
	gettok();
	gettok();
	if (!usescncamera)
		return;
	nlwstrees++;
//	ct=lwstrees[nlwstrees]=alloctree(0,NULL);
	ct=lwstrees[nlwstrees]=allochelper(0,HELPER_CAMERA,0);
	ct->seqs=s=memzalloc(sizeof(struct seq));
	ct->nseq=1;
	ct->flags=TF_ISCAMERA;
	s->startframe=startframe;
	ct->curframe=startframe;
	s->endframe=endframe;
	s->seqbuildo2p=O2P_FROMTRANSROTSCALE;
	targetsource=nlwstrees;
	mystrncpy(ct->name,"lwscamera",NAMESIZE);
}

static void targetobject()
{
	int p=atoi(gettok());
//	lwstrees[nlwstrees]->parent=lwstrees[p];
	if (!usescncamera)
		return;
	if (doingobject==OBJ_CAMERA) {
		targetdest=p;
	}
}

static void zoomfactor()
{
	char *tok;
	tok=gettok();
	if (!usescncamera)
		return;
	if (!checkenvelope(tok))
		ct->zoom=(float)atof(tok);
}

static void ambientcolor()
{
	struct seq *s;
	doingobject=OBJ_LIGHT;
	if (!usescnlights) {
		getrestline();
		return;
	}
	nlwstrees++;
	objnum++;
	parent2clws[objnum]=nlwstrees;
//	ct=lwstrees[nlwstrees]=alloctree(0,NULL);
	ct=lwstrees[nlwstrees]=allochelper(0,HELPER_AMBLIGHT,0);
	ct->seqs=s=memzalloc(sizeof(struct seq));
	ct->nseq=1;
	ct->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	s->startframe=startframe;
	ct->curframe=startframe;
	s->endframe=endframe;
	s->seqbuildo2p=O2P_FROMTRANSROTSCALE;
	mystrncpy(ct->name,"lwsambientlight",NAMESIZE);
	ct->lightcolor.x=(1/256.0f)*(float)(.5f+atof(gettok()));
	ct->lightcolor.y=(1/256.0f)*(float)(.5f+atof(gettok()));
	ct->lightcolor.z=(1/256.0f)*(float)(.5f+atof(gettok()));
}

static void ambintensity()
{
	char *tok;
	if (!usescnlights) {
		getrestline();
		return;
	}
	tok=gettok();
	if (!checkenvelope(tok))
		ct->intensity=(float)atof(tok);
}

static void lightname()
{
	struct seq *s;
	doingobject=OBJ_LIGHT;
	if (!usescnlights) {
		getrestline();
		return;
	}
	nlwstrees++;
	objnum++;
	parent2clws[objnum]=nlwstrees;
//	ct=lwstrees[nlwstrees]=alloctree(0,NULL);
	ct=lwstrees[nlwstrees]=allochelper(0,HELPER_LIGHT,0);
	ct->seqs=s=memzalloc(sizeof(struct seq));
	ct->nseq=1;
	ct->flags|=TF_ISLIGHT;
	s->startframe=startframe;
	ct->curframe=startframe;
	s->endframe=endframe;
	s->seqbuildo2p=O2P_FROMTRANSROTSCALE;
	mystrncpy(ct->name,getrestline(),NAMESIZE);
}

static void lightcolor()
{
	if (!usescnlights) {
		getrestline();
		return;
	}
	ct->lightcolor.x=(1/256.0f)*(float)(.5f+atof(gettok()));
	ct->lightcolor.y=(1/256.0f)*(float)(.5f+atof(gettok()));
	ct->lightcolor.z=(1/256.0f)*(float)(.5f+atof(gettok()));
}

static void lgtintensity()
{
	char *tok;
	if (!usescnlights) {
		getrestline();
		return;
	}
	tok=gettok();
	if (!checkenvelope(tok))
		ct->intensity=(float)atof(tok);
}

struct lwscommand {
	char *name;
	void (*func)();
};

static struct lwscommand lwscommands[]={
// scene section
	"FirstFrame",firstframe,
	"LastFrame",lastframe,
	"PreviewFirstFrame",firstframe,
	"PreviewLastFrame",lastframe,
// objects section
	"LoadObject",loadobject,
	"AddNullObject",addnullobject,
	"ShowObject",showobject,
	"ObjectMotion",objectmotion,
//	"EndBehavior",endbehavior,
	"ParentObject",parentobject,
	"ObjDissolve",objdissolve,
// object Skeleton
	"BoneName",bonename,
	"BoneActive",boneactive,
	"BoneRestPosition",bonerestposition,
	"BoneRestDirection",bonerestdirection,
	"BoneRestLength",bonerestlength,
	"BoneMotion",objectmotion,
// lights section
	"LightName",lightname,
	"LightMotion",objectmotion,
	"AmbientColor",ambientcolor,
	"AmbIntensity",ambintensity,
	"LightColor",lightcolor,
	"LgtIntensity",lgtintensity,
// camera section
	"ShowCamera",showcamera,
	"TargetObject",targetobject,
	"ZoomFactor",zoomfactor,
	"CameraMotion",objectmotion,
	"CustomSize",customsize,
// effects section NO
// render section NO
// layout options section NO
};

#define NLWSCOMMAND (sizeof(lwscommands)/sizeof(lwscommands[0]))

static void fixuptreechildren()
{
	int i;
	struct tree *n,*p;
// first count up # of children and put it into t->nmaxchildren
	for (i=1;i<nlwstrees;i++)
		lwstrees[parentidx[i]]->maxchildren++;
// now allocate and copy over to trees with right # of children
	for (i=0;i<nlwstrees;i++) {
		n=alloctree(lwstrees[i]->maxchildren+EXTRACHILDREN,NULL); // get tree with right # of children
		memcpy(n,lwstrees[i],sizeof(struct tree)); // copy old tree over, messing up maxchildren
		n->maxchildren+=EXTRACHILDREN; // fix up maxchildren
		memfree(lwstrees[i]); // get rid of old tree
		lwstrees[i]=n; // new tree now set, maxchildren and nchildren=0, just gotta set children
	}
	for (i=1;i<nlwstrees;i++) {
		p=lwstrees[parentidx[i]];
		p->children[p->nchildren++]=lwstrees[i];
		lwstrees[i]->parent=p;
	}
}

struct tree *loadlws(char *lwsname)
{
	char nameext[256];
	int i,j;
	char *tok;
	float framestepsave;
	perf_start(LOADLWS);
	nlwstrees=0;
	targetsource=targetdest=0;
	objnum=0;
	screenx=640;
	screeny=480;
	clear32((char *)parentidx,sizeof(parentidx),0);
	lwstrees[nlwstrees]=alloctree(0,NULL);
	getnameext(nameext,lwsname);
	mystrncpy(lwstrees[nlwstrees]->name,nameext,NAMESIZE);
	opentokfile(lwsname);
	tok=gettok();
	if (strcmp(tok,"LWSC"))
		errorexit("'%s' is not an lws file",lwsname);
	tok=gettok();
	if (strcmp(tok,"1"))
		errorexit("'%s' wrong version of lws file",lwsname);
	while(tok=gettok()) {
//		logger("processing token '%s'\n",tok);
		for (i=0;i<NLWSCOMMAND;i++) {
			if (!strcmp(lwscommands[i].name,tok)) {
				(*lwscommands[i].func)();
				break;
			}
		} 
		if (i==NLWSCOMMAND) {
			getrestline();
		}
	}
	objnum++;
	nlwstrees++;
//	lwstrees[0]->startframe=startframe;
//	lwstrees[0]->endframe=endframe;
	lwstrees[0]->curframe=startframe;
	closetokfile();
//	for (i=0;i<nlwstrees;i++)
//		memfree(lwstrees[i]);
	for (i=1;i<objnum;i++) {
		j=parent2clws[i];
		parentidx[j]=parent2clws[parentidx[j]]; // fixup now when all objects found..
	}
	fixuptreechildren();
	if (targetdest)
		lwstrees[targetsource]->camtarget=lwstrees[targetdest];
	setvpsrc(screenx,screeny); // lws.c calls this
	if (usescncamera)
		setlastcam(lwstrees[targetsource]);
	else
		setlastcam(NULL);
	lwstrees[0]->flags|=TF_ISSCENE;
	buildinfluencelists(lwstrees[0],NULL,NULL,0);
	for (i=1;i<nlwstrees;i++)
		if (lwstrees[i]->flags&TF_ISLIGHT)
			addlighttolist(lwstrees[i]);
	framestepsave=framestep;
	framestep=0;
	doanims(lwstrees[0]);
	framestep=framestepsave;
	perf_end(LOADLWS);
	return lwstrees[0];
}

