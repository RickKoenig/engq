/*#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <engine7cpp.h>
#include <videoicpp.h>
#include <misclib7cpp.h>

#include "carenalobbycpp.h"
#include "newconstructorres.h"
#include "newnameres.h"
#include "perfcpp.h"
#include "trackhashcpp.h"
#include "tracklistcpp.h"

#include "meshtest3cpp.h"
#include "meshtest2cpp.h"

#include "usefulcpp.h"
*/
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
#include "newconstructor.h"
#include "system/u_states.h"

#define TRACKVIEWX 10
#define TRACKVIEWY 10
#define TRACKVIEWW 512
#define TRACKVIEWH 384
#define TRACKVIEWX2 550
#define TRACKVIEWY2 215
#define TRACKVIEWW2 240
#define TRACKVIEWH2 180

#define CAMBALLX 580
#define CAMBALLY 240
#define CAMBALLW 150
#define CAMBALLH 160


static int outline;
static text* TEXTTRACK;
static pbut* PBUTBACKSAVE;
static pbut* PBUTBACKNOSAVE;
static pbut* PBUTBACKDELETE;
static pbut* PBUTCOPY;
static pbut* PBUTCLEAR;
static pbut* PBUTOUTLINE;
static listbox* LISTTRACKSECT;
static pbut* PBUTTRACKTSECTADDAFTER;
static pbut* PBUTTRACKSECTADDBEFORE;
static pbut* PBUTDELSECT;
static listbox* LISTSECTPRIM;
static pbut* PBUTCOPYPRIM;
static pbut* PBUTDELPRIM;
static vscroll* VSLIDERLEN;
static vscroll* VSLIDERHEADING;
static vscroll* VSLIDERPITCH;
static vscroll* VSLIDERBANK;
static vscroll* VSLIDERINWID;
static vscroll* VSLIDEROUTWID;
static text* TEXTPRIM2;
static text* TEXTPRIMNAME;
static edit* EDITPRIMNAME;
static listbox* LISTSECTLIST;
static pbut* PBUTDELSECTLIST;
static pbut* PBUTSWAPVIEWPORT;
static vscroll* VSLIDERDIRHEADING;
static vscroll* VSLIDERDIRPITCH;
static text* TEXTLIGHTS;
static pbut* PBUTMM;
static vscroll* VSLIDERSMOOTHANG;
static vscroll* VSLIDERCAM;
static shape* focus;

static edit* EDITNAME;
static pbut* PBUTNAMEOK;
static pbut* PBUTNAMECAN;

#define MAXPRIM 1000
extern viewport2 constructor_viewport;
static viewport2 constructor_viewportn;//,constructor_viewport3;
extern tree2* constructor_viewport_roottree;
static int camtween,camx,camy;	// raw input cam values
static pointf3 camfocus;
static float camfocusdir;

#define FARDIST 500
#define CLOSEDIST 10
static tree2* trackscene;//,*trackcursor,*trackpiece;
static tree2* amblight,*dirlight;
static float tracksmoothang;

static shape *rl,*rlnn;
enum mode {MODE_NORMAL,MODE_NEWNAME};
static mode consmode;

static newtrack *nthetrack;
//static newtrack *onesection;
//static unsigned int trackhash[4];
//static int unique;
tracklist* tl;

// standard call to make a tree from a file for newtrk
static tree2* loadnewconstrack(const char *trackname)
{
	tree2* r;
	nthetrack=loadnewtrackscript(trackname);
	r=buildnewconsscene(nthetrack);
	return r;
}

/*static newtrack *initnewtrackscript()
{
//	char str[100];
	int i,k=1;
	newtrack *tk;
	tk=(newtrack *)memzalloc(sizeof(newtrack));
	logger("sizeof ntk is %d\n",sizeof(*tk));
	tk->sections=(section *)memzalloc(sizeof(section)*k);
	for (i=0;i<k;i++) {
		tk->sections[i].len=3;
		tk->sections[i].heading=0;
	}
	tk->nsections=k;
	return tk;
}
*/
static section minsect={2,-45,-45,-45,  1,  2};
static section maxsect={200, 45, 45, 45,100,200};

static int interp2slider(float v,float mins,float maxs,int vmin,int vmax)
{
	S32 ret;
	float fret=vmax+(v-mins)*(vmin-vmax)/(maxs-mins);
	F2INT(fret,ret);
	return ret;
}

static float interpfromslider(int v,int vmin,int vmax,float mins,float maxs)
{
	return float(maxs+(v-vmin)*(mins-maxs)/(vmax-vmin));
}

static void prim2sliders(section *sc)
{
	int vmin,vmax;
	VSLIDERLEN->getminmaxval(vmin,vmax);
	VSLIDERLEN->setidx(interp2slider(sc->len,minsect.len,maxsect.len,vmin,vmax));
	VSLIDERHEADING->setidx(interp2slider(sc->heading,minsect.heading,maxsect.heading,vmin,vmax));
	VSLIDERPITCH->setidx(interp2slider(sc->pitch,minsect.pitch,maxsect.pitch,vmin,vmax));
	VSLIDERBANK->setidx(interp2slider(sc->bank,minsect.bank,maxsect.bank,vmin,vmax));
	VSLIDERINWID->setidx(interp2slider(sc->inwid,minsect.inwid,maxsect.inwid,vmin,vmax));
	VSLIDEROUTWID->setidx(interp2slider(sc->outwid,minsect.outwid,maxsect.outwid,vmin,vmax));
}

static void sliders2prim(section *sc)
{
	int vmin,vmax;
	VSLIDERLEN->getminmaxval(vmin,vmax);
	sc->len=interpfromslider(VSLIDERLEN->getidx(),vmin,vmax,minsect.len,maxsect.len);
	sc->heading=interpfromslider(VSLIDERHEADING->getidx(),vmin,vmax,minsect.heading,maxsect.heading);
	sc->pitch=interpfromslider(VSLIDERPITCH->getidx(),vmin,vmax,minsect.pitch,maxsect.pitch);
	sc->bank=interpfromslider(VSLIDERBANK->getidx(),vmin,vmax,minsect.bank,maxsect.bank);
	sc->inwid=interpfromslider(VSLIDERINWID->getidx(),vmin,vmax,minsect.inwid,maxsect.inwid);
	sc->outwid=interpfromslider(VSLIDEROUTWID->getidx(),vmin,vmax,minsect.outwid,maxsect.outwid);
}

static void updatesectionlist(struct newtrack *s)
{
	char str[50];
	section *sc;
	int nsections=s->sections.size();
	int i,c;
//	c=getcurlistbox(rl,LISTSECTPRIM);
	c=LISTSECTPRIM->getidx();
//	while(getnumlistbox(rl,LISTSECTPRIM))
//		dellistboxname(rl,LISTSECTPRIM,0,VSCROLLSECTPRIM);
	LISTSECTPRIM->clear();
	if (c==-1)
		c=0;
	c=range(0,c,nsections-1);
// sync list with section
	for (i=0;i<nsections;i++) {
		sprintf(str,"%5.1f %5.1f %5.1f %5.1f %5.1f %5.1f",
			s->sections[i].len,s->sections[i].heading,s->sections[i].pitch,s->sections[i].bank,
			s->sections[i].inwid,s->sections[i].outwid);
//		addlistboxname(rl,LISTSECTPRIM,-1,str,VSCROLLSECTPRIM);
		LISTSECTPRIM->addstring(str);
	}
//	setcurlistbox(rl,LISTSECTPRIM,c,VSCROLLSECTPRIM);
	LISTSECTPRIM->setidxc(c);
	sc=&s->sections[c];
	sprintf(str,"%5.1f %5.1f %5.1f %5.1f %5.1f %5.1f",sc->len,sc->heading,sc->pitch,sc->bank,sc->inwid,sc->outwid);
	prim2sliders(sc);
//	setresname(rl,TEXTPRIM2,str);
	TEXTPRIM2->settname(str);
}

/*static void initsectionlist(struct newtrack *s)
{
// propagate changes
	updatesectionlist(s);
}
*/
static void copyprim(struct newtrack *s)
{
	int c;
	int nsections=s->sections.size();
	if (nsections>=MAXPRIM)
		return;
//	c=getcurlistbox(rl,LISTSECTPRIM);
	c=LISTSECTPRIM->getidx();
//	s->sections=(section *)memrealloc(s->sections,sizeof(section)*(s->nsections+1));
//	for (i=s->nsections-1;i>=c;i--)
//		s->sections[i+1]=s->sections[i];
//	s->sections[c+1].heading++;
//	s->nsections++;
	s->sections.insert(s->sections.begin()+c,s->sections[c]);
//	addlistboxname(rl,LISTSECTPRIM,c,"---",VSCROLLSECTPRIM); // so setcurlistbox works right
	LISTSECTPRIM->addstring("---");
//	setcurlistbox(rl,LISTSECTPRIM,c+1,VSCROLLSECTPRIM);
	LISTSECTPRIM->setidxc(c+1);
	updatesectionlist(s);
}

static void deleteprim(struct newtrack *s)
{
	int c;
	S32 nsections=s->sections.size();
	if (nsections<=1)
		return;
//	c=getcurlistbox(rl,LISTSECTPRIM);
	c=LISTSECTPRIM->getidx();
//	s->nsections--;
//	for (i=c;i<s->nsections;i++)
//		s->sections[i]=s->sections[i+1];
//	s->sections=(section *)memrealloc(s->sections,sizeof(section)*(s->nsections));
	s->sections.erase(s->sections.begin()+c);
	updatesectionlist(s);
}

static void setcamfocus(newtrack *t,int sectnum)
{
	pointf3 curpos;//,pyr;
//	section *sect;
	vector<section>::iterator sect=t->sections.begin();
	S32 nsections=t->sections.size();
	curpos=t->start;
//	pyr=t->pyr;
	float curheading=t->start.y;
	int j;
	for (/*sect=t->sections*/j=0;j<=sectnum;j++,sect++) {
//		float prevheading=curheading;
		if (j!=0 && j!=nsections)
			curheading+=sect->heading;
		if (j==sectnum) {
			curpos.x+=.5f*sect->len*dsin(curheading);
			curpos.z+=.5f*sect->len*dcos(curheading);
			curpos.y+=.5f*sect->len*dsin(sect->pitch);
		} else {
			curpos.x+=sect->len*dsin(curheading);
			curpos.z+=sect->len*dcos(curheading);
			curpos.y+=sect->len*dsin(sect->pitch);
		}
	}
	camfocus=curpos;
	camfocusdir=curheading;
}

static void modifyprim(struct newtrack *s)
{
	int c;
	section *sc;
//	c=getcurlistbox(rl,LISTSECTPRIM);
	c=LISTSECTPRIM->getidx();
	sc=&s->sections[c];
	sliders2prim(sc);
	updatesectionlist(s);
//	setcamfocus(nthetrack,getcurlistbox(rl,LISTSECTPRIM));
	setcamfocus(nthetrack,LISTSECTPRIM->getidx());
}

// load a section from a text file
newtrack *loadnewtrackscript(const char *trackname)
{
	char str[100];
	int i,k;
	newtrack *tk;
//	tk=(newtrack *)memzalloc(sizeof(newtrack));
	tk=new newtrack;
//	tk->nsections=0;
	tk->pyr=pointf3x(0,0,0);
	tk->start=pointf3x(0,0,0);
//	tk->sections=0;
	logger("sizeof ntk is %d\n",sizeof(*tk));
	sprintf(str,"%s.trk",trackname);
pushandsetdirdown("constructed");
	if (fileexist(str)) {
		script sc(str);
//		char **sc;
//		int nsc;
//		sc=loadscript(str,&nsc);
		S32 nsc=sc.num();
		int stride=0;
		if (nsc>0 && !strcmp("newtrack",sc.idx(0).c_str())) {
			stride=5;
		} else if (nsc>0 && !strcmp("newtrack2",sc.idx(0).c_str())) {
			stride=7;
		}
		if (nsc<11 ||
				strcmp("start",sc.idx(1).c_str()) ||
				strcmp("pyr",sc.idx(5).c_str()) ||
				strcmp("nsections",sc.idx(9).c_str())) {
//			freescript(sc,nsc);
			popdir();
			delete tk;
			return 0;
		}
		tk->start.x=(float)atof(sc.idx(2).c_str());
		tk->start.y=(float)atof(sc.idx(3).c_str());
		tk->start.z=(float)atof(sc.idx(4).c_str());
		tk->pyr.x=(float)atof(sc.idx(6).c_str());
		tk->pyr.y=(float)atof(sc.idx(7).c_str());
		tk->pyr.z=(float)atof(sc.idx(8).c_str());
		k=atoi(sc.idx(10).c_str());
//		tk->nsections=k;
		if (nsc<11+stride*k) {
//			freescript(sc,nsc);
			delete tk;
popdir();
			return 0;
		}

		for (i=0;i<k;i++) {
			section s;
			s.len=(float)atof(sc.idx(11+i*stride+1).c_str());
			s.heading=(float)atof(sc.idx(11+i*stride+2).c_str());
			s.pitch=(float)atof(sc.idx(11+i*stride+3).c_str());
			s.bank=(float)atof(sc.idx(11+i*stride+4).c_str());
			if (stride==7) {
			s.inwid=(float)atof(sc.idx(11+i*stride+5).c_str());
			s.outwid=(float)atof(sc.idx(11+i*stride+6).c_str());
			} else {
				s.inwid=2.0f;
				s.outwid=4.0f;
			}
			tk->sections.push_back(s);
		}
//		freescript(sc,nsc);
popdir();
		return tk;
	}
	delete tk;
popdir();
	return 0;
}

// free a track section
void freenewtrackscript(newtrack *t)
{
//	if (t->sections)
//		memfree(t->sections);
//	memfree(t);
	delete t;
}

// make a facevert from the newtrk structure
static facevert *buildtracksection(const newtrack *t)
{
//	static int faceoffset[4]={0,2,3,5};
//	static int vertoffset[7]={0,1,1,2,3,3,4};
	facevert *fv;
//	fv=(facevert *)memzalloc(sizeof(facevert));
	fv=new facevert;
	S32 nsections=t->sections.size();
//	fv->nvert=(nsections+1)*5;
	S32 nvert=(nsections+1)*5;
//	fv->nface=nsections*8;
	S32 nface=nsections*8;
//	fv->faces=(face *)memzalloc(sizeof(face)*fv->nface);
//	fv->faces=new face[fv->nface];
	fv->faces.resize(nface);
//	fv->verts=(pointf3 *)memzalloc(sizeof(pointf3)*fv->nvert);
//	fv->verts=new pointf3[fv->nvert];
	fv->verts.resize(nvert);
//	fv->uvs=(uv *)memzalloc(sizeof(uv)*fv->nvert);
//	fv->uvs=new uv[fv->nvert];
	fv->uvs.resize(nvert);
	int i,j;
//	int foi;
//	face *tfaces=fv->faces;
	vector<face>::iterator tfaces=fv->faces.begin();
//	pointf3 *tverts=fv->verts;
	vector<pointf3>::iterator tverts=fv->verts.begin();
//	uv *tuvs=fv->uvs;
	vector<uv>::iterator tuvs=fv->uvs.begin();
// setup faces
	for (j=0;j<nsections;j++) {
		for (i=0;i<4;i++) {
//			foi=faceoffset[i];
			tfaces->vertidx[0]=i+j*5;
			tfaces->vertidx[1]=i+(j+1)*5+1;
			tfaces->vertidx[2]=i+j*5+1;
			if (i==0 || i==3)
				tfaces->fmatidx=1;
			else
				tfaces->fmatidx=0;
			tfaces++;
			tfaces->vertidx[0]=i+j*5;
			tfaces->vertidx[1]=i+(j+1)*5;
			tfaces->vertidx[2]=i+(j+1)*5+1;
			if (i==0 || i==3)
				tfaces->fmatidx=1;
			else
				tfaces->fmatidx=0;
			tfaces++;
		}
	}
// setup verts
	pointf3 curpos;//,pyr;
//	section *sect;
//	vector<section>::const_iterator sect=t->sections.begin();
	curpos=t->start;
//	pyr=t->pyr;
	float curheading=t->start.y;
	static float across[5];
	for (/*sect=t->sections,*/j=0;j<=nsections;j++) {
		if (j==nsections) {
			across[0]=-t->sections[j-1].outwid/2;
			across[1]=-t->sections[j-1].inwid/2;
			across[3]=t->sections[j-1].inwid/2;
			across[4]=t->sections[j-1].outwid/2;
		} else {
			across[0]=-t->sections[j].outwid/2;
			across[1]=-t->sections[j].inwid/2;
			across[3]=t->sections[j].inwid/2;
			across[4]=t->sections[j].outwid/2;
		}
		float prevheading=curheading;
		float anga,angu;
		float fcosa,fsina;  // across x z
		float fsinu;	// across y
		if (j!=0 && j!=nsections) {
			curheading+=t->sections[j].heading;
			angu=(t->sections[j].bank+t->sections[j-1].bank)*.5f;
		} else
			angu=0;
		anga=(curheading+prevheading)*.5f;
		fcosa=dcos(anga); // across x
		fsina=dsin(anga); // across z
		fsinu=dsin(angu); // across y
		for (i=0;i<5;i++) {
			if (i==0 || i==4)
			tverts->vmatidx=1;
		else
			tverts->vmatidx=0;

//			foi=vertoffset[i];
			tverts->x=curpos.x+across[i]*fcosa;
			tverts->y=curpos.y+across[i]*fsinu;
			tverts->z=curpos.z-across[i]*fsina;
//			tverts->x=curpos.x+float(i-2)*fcosa;
//			tverts->y=curpos.y+float(i-2)*fsinu;
//			tverts->z=curpos.z-float(i-2)*fsina;
			tuvs->u=float(i);
			tuvs->v=float(-j); // loss of precision, might be bad (large uvs)
			tverts++;
			tuvs++;
		}
//		pyr.x=sect->pitch;
//		pyr.z=sect->bank*.5f;
		if (j!=nsections) {
			curpos.x+=t->sections[j].len*dsin(curheading);
			curpos.z+=t->sections[j].len*dcos(curheading);
			curpos.y+=t->sections[j].len*dsin(t->sections[j].pitch);
		}
//		if (j!=t->nsections)
//			curheading+=sect->heading;
//		pyr.y+=sect->heading*.5f;
//		pyr.x+=sect->pitch*.5f;
//		pyr.z+=sect->bank*.5f;
	}
	return fv;
}

static tree2* track,*backgnd;
//static tree2* tracks;
#if 0
tree2* buildnewconsscene(const newtrack *t)
{
	tree2* ret;
	static struct backgndsectioninfo bs={{3,0,4},{15,0,15},2,2};
// build track into 2 formats
	facevert *trkfv=buildtracksection(t);
	polylist *plt=convertfaceverts2polylist(trkfv);
// build backgnd into 2 formats
	facevert *backfv=buildbackgndsectionfv(&bs);
	polylist *plb=convertfaceverts2polylist(backfv);
// subdivide triangles of track with outer edges, (where it meshes with background)
	polylist *pltc=boolabintsectm1(plt,backfv->verts,backfv->edges);//,backfv->nedge);
// move outer vertices 'y' of track to match background
	setm1tobackgnd(pltc);
// build track tree
	track=buildapolylist(pltc,"track2","track02b.dds","grnd02m.dds");
//	if (!track)
//		errorexit("can't make track");
// set backgnd 'y' to backgnd
	backypolylist(plb);
// a scene with track and backgnd
//	ret=alloctree(2,0);
	ret=new tree2("newtrackscene");
	if (track)
//		linkchildtoparent(track,ret);
		ret->linkchild(track);
// make a polylist of the outline of the track
	polylist *plto=convertfaceverts2polylisto(trkfv);
// subtrack outline of track from backgnd
	polylist *plbc=boolab(plb,plto,OP_MINUS);
// set backgnd 'y' to backgnd again
	backypolylist(plbc);
// apply texture to backgnd
	addlinearuvspolylist(plbc,3,4,1/16.0f,-1/20.0f);
// finally make tree of backgnd
	backgnd=buildapolylist(plbc,"backgnd2","rvrbnkshrubs.dds");
//	if (!backgnd)
//		errorexit("no terrain background");
// link to scene
	if (backgnd)
//		linkchildtoparent(backgnd,ret);
		ret->linkchild(backgnd);
// free all resources used
//	freepolylist(plt);
	delete plt;
//	freefacevert(trkfv);
//	freefacevert(backfv);
	delete trkfv;
	delete backfv;
//	freepolylist(pltc);
//	freepolylist(plb);
//	freepolylist(plto);
//	freepolylist(plbc);
	delete pltc;
	delete plb;
	delete plto;
	delete plbc;
	return ret;
}
#elif 0
tree2* buildnewconsscene(const newtrack *t)
{
	tree2* ret;
	static struct backgndsectioninfo bs={{3,0,4},{15,0,15},2,2};
// build track into 2 formats
	facevert *trkfv=buildtracksection(t);
	polylist *plt=convertfaceverts2polylist(trkfv);
// build backgnd into 2 formats
	facevert *backfv=buildbackgndsectionfv(&bs);
	polylist *plb=convertfaceverts2polylist(backfv);
// subdivide triangles of track with outer edges, (where it meshes with background)
	polylist *pltc=boolabintsectm1(plt,backfv->verts,backfv->edges);//,backfv->nedge);
// move outer vertices 'y' of track to match background
	setm1tobackgnd(pltc);
// build track tree
	track=buildapolylist(pltc,"track2","track02b.dds","grnd02m.dds");
//	if (!track)
//		errorexit("can't make track");
// set backgnd 'y' to backgnd
	backypolylist(plb);
// a scene with track and backgnd
//	ret=alloctree(2,0);
	ret=new tree2("newtrackscene");
	if (track)
//		linkchildtoparent(track,ret);
		ret->linkchild(track);
// make a polylist of the outline of the track
	polylist *plto=convertfaceverts2polylisto(trkfv);
// subtrack outline of track from backgnd
	polylist *plbc=boolab(plb,plto,OP_MINUS);
// set backgnd 'y' to backgnd again
	backypolylist(plbc);
// apply texture to backgnd
	addlinearuvspolylist(plbc,3,4,1/16.0f,-1/20.0f);
// finally make tree of backgnd
	backgnd=buildapolylist(plbc,"backgnd2","rvrbnkshrubs.dds");
//	if (!backgnd)
//		errorexit("no terrain background");
// link to scene
	if (backgnd)
//		linkchildtoparent(backgnd,ret);
		ret->linkchild(backgnd);
// free all resources used
//	freepolylist(plt);
	delete plt;
//	freefacevert(trkfv);
//	freefacevert(backfv);
	delete trkfv;
	delete backfv;
//	freepolylist(pltc);
//	freepolylist(plb);
//	freepolylist(plto);
//	freepolylist(plbc);
	delete pltc;
	delete plb;
	delete plto;
	delete plbc;
	return ret;
}
#elif 0
tree2* buildnewconsscene(const newtrack *t)
{
	tree2* ret;
// build track into 2 formats
	facevert *trkfv=buildtracksection(t);
	polylist *plt=convertfaceverts2polylist(trkfv);
// subdivide triangles of track with outer edges, (where it meshes with background)
//	polylist *pltc=boolabintsectm1(plt,backfv->verts,backfv->edges);//,backfv->nedge);
// move outer vertices 'y' of track to match background
//	setm1tobackgnd(pltc);
// build track tree
	track=buildapolylist(plt,"track2","track02b.dds","grnd02m.dds");
// a scene with track and backgnd
	ret=new tree2("newtrackscene");
	if (track)
		ret->linkchild(track);
// free all resources used
	delete plt;
	delete trkfv;
	return ret;
}
#else
tree2* buildnewconsscene(const newtrack *t)
{
	tree2* ret;
	static struct backgndsectioninfo bs={{4,0,4},{160,0,160},2,2}; // 320 by 320 square meters
// build track into 2 formats
	facevert *trkfv=buildtracksection(t);
	polylist *plt=convertfaceverts2polylist(trkfv);
// build backgnd into 2 formats
	facevert *backfv=buildbackgndsectionfv(&bs);
	polylist *plb=convertfaceverts2polylist(backfv);
// subdivide triangles of track with outer edges, (where it meshes with background)
	polylist *pltc=boolabintsectm1(plt,backfv->verts,backfv->edges);//,backfv->nedge);
// move outer vertices 'y' of track to match background
	setm1tobackgnd(pltc);
// build track tree
	track=buildapolylist(pltc,"track2","track02b.dds","grnd02m.dds");
//	if (!track)
//		errorexit("can't make track");
// set backgnd 'y' to backgnd
	backypolylist(plb);
// a scene with track and backgnd
//	ret=alloctree(2,0);
	ret=new tree2("newtrackscene");
	if (track)
//		linkchildtoparent(track,ret);
		ret->linkchild(track);
// make a polylist of the outline of the track
	polylist *plto=convertfaceverts2polylisto(trkfv);
// subtrack outline of track from backgnd
	polylist *plbc=boolab(plb,plto,OP_MINUS);
// set backgnd 'y' to backgnd again
	backypolylist(plbc);
// apply texture to backgnd
	addlinearuvspolylist(plbc,3,4,1/16.0f,-1/20.0f);
// finally make tree of backgnd
	backgnd=buildapolylist(plbc,"backgnd2","rvrbnkshrubs.dds");
//	if (!backgnd)
//		errorexit("no terrain background");
// link to scene
	if (backgnd)
//		linkchildtoparent(backgnd,ret);
		ret->linkchild(backgnd);
// free all resources used
//	freepolylist(plt);
	delete plt;
//	freefacevert(trkfv);
//	freefacevert(backfv);
	delete trkfv;
	delete backfv;
//	freepolylist(pltc);
//	freepolylist(plb);
//	freepolylist(plto);
//	freepolylist(plbc);
	delete pltc;
	delete plb;
	delete plto;
	delete plbc;
	return ret;
}

#endif

#if 0
tree2* buildtrackpiece(newtrack *t)
{
	tree2* ret,*trackl,*backgndl;
	static struct backgndsectioninfo bs={{3,0,4},{15,0,15},2,2};
// build track into 2 formats
	facevert *trkfv=buildtracksection(t);
	polylist *plt=convertfaceverts2polylist(trkfv);
// build backgnd into 2 formats
	facevert *backfv=buildbackgndsectionfv(&bs);
	polylist *plb=convertfaceverts2polylist(backfv);
// subdivide triangles of track with outer edges, (where it meshes with background)
	polylist *pltc=boolabintsectm1(plt,backfv->verts,backfv->edges,backfv->nedge);
// move outer vertices 'y' of track to match background
	setm1tobackgnd(pltc);
// build track tree
	trackl=buildapolylist(pltc,"tracks","track02b.dds","grnd02m.dds");
//	if (!trackl)
//		errorexit("can't make track");
// set backgnd 'y' to backgnd
	backypolylist(plb);
// a scene with track and backgnd
	ret=alloctree(2,0);
	if (trackl) {
//		if (trackl->mod)
//			setmodelnormsang(trackl->mod,3*PIOVER180,NULL); // bad idea
		linkchildtoparent(trackl,ret);
	}
// make a polylist of the outline of the track
	polylist *plto=convertfaceverts2polylisto(trkfv);
// subtract outline of track from backgnd
	polylist *plbc=boolab(plb,plto,OP_MINUS);
// set backgnd 'y' to backgnd again
	backypolylist(plbc);
// apply texture to backgnd
	addlinearuvspolylist(plbc,3,4,1/16.0f,-1/20.0f);
// finally make tree of backgnd
	backgndl=buildapolylist(plbc,"backgnd3","rvrbnkshrubs.dds");
//	if (!backgndl)
//		errorexit("no terrain background");
// link to scene
	if (backgndl)
		linkchildtoparent(backgndl,ret);
// free all resources used
	freepolylist(plt);
	freefacevert(trkfv);
	freefacevert(backfv);
	freepolylist(pltc);
	freepolylist(plb);
	freepolylist(plto);
	freepolylist(plbc);
	return ret;
}
#endif
string savenewconstrack(const char *name,const newtrack *td,int u)
{
	vector<section>::const_iterator s;
	int i;
	FILE *fw;
	char str[70];
	string uniq;
pushandsetdirdown("constructed");
	if (u) {
		uniq=tl->uniquetrackname(name);
		sprintf(str,"%s.trk",uniq.c_str());
	} else
		sprintf(str,"%s.trk",name);
	fw=fopen2(str,"w");
	fprintf(fw,"newtrack2\n");
	fprintf(fw,"start 0 0 0\n");
	fprintf(fw,"pyr 0 0 0\n");
	S32 nsections=td->sections.size();
	fprintf(fw,"nsections %d\n",nsections);
	for (i=0,s=td->sections.begin();i<nsections;i++,s++)
		fprintf(fw,"section %f %f %f %f %f %f\n",s->len,s->heading,s->pitch,s->bank,s->inwid,s->outwid);
	fclose(fw);
popdir();
	return uniq;
}

// to and from packet format
/*
unsigned char *writenewtrack(newtrack *t,int *len)
{
	unsigned char *data;
	unsigned int *ns;
	*len=sizeof(*ns)+sizeof(section)*t->nsections;
	data=(unsigned char *)memalloc(*len);
	ns=(unsigned int *)data;
	section *sd=(section *)(data+sizeof(*ns));
	*ns=t->nsections;
	memcpy(sd,t->sections,sizeof(section)*t->nsections);
	return data;
}
*/

// U32 nsections, array of sections, section is 4 floats
// make a packet
U8* writenewtrack(const newtrack* t,U32* len)
{
	U8 *data;
	U32 *nsp;
	U32 nus=t->sections.size();
	*len = sizeof(*nsp) + sizeof(section) * nus;
//	data=(unsigned char *)memalloc(*len);
	data=new U8[*len];
	nsp=(U32*)data;
	section *sd=(section *)(data+sizeof(*nsp));
//	*ns=t->nsections;
	*nsp=nus;
	memcpy(sd,&t->sections[0],sizeof(section)*nus);
	return data;
}

/*
newtrack *readnewtrack(unsigned char *buff,int len)
{
	unsigned int *ns=(unsigned int *)buff;
	section *sd=(section *)(buff+sizeof(*ns));
	if ((unsigned)len!=*ns*sizeof(section)+sizeof(*ns))
		errorexit("bad len in readnewtrack");
	newtrack *t=(newtrack *)memzalloc(sizeof(newtrack));
	t->nsections=*ns;
	t->sections=(section *)memalloc(sizeof(section)*t->nsections);
	memcpy(t->sections,sd,sizeof(section)*t->nsections);
	return t;
}
*/

// parse a packet
newtrack* readnewtrack(const U8* buff,U32 len)
{
	unsigned int *ns=(unsigned int *)buff;
	section *sd=(section *)(buff+sizeof(*ns));
	if ((unsigned)len!=*ns*sizeof(section)+sizeof(*ns))
		errorexit("bad len in readnewtrack");
//	newtrack *t=(newtrack *)memzalloc(sizeof(newtrack));
	newtrack* t=new newtrack;
//	t->nsections=*ns;
	S32 nsections=*ns;
//	t->sections=(section *)memalloc(sizeof(section)*t->nsections);
	t->sections.resize(nsections);
	memcpy(&t->sections[0],sd,sizeof(section)*nsections);
	return t;
}
/*
unsigned char *writenewtrack(newtrk *t,int *len)
{
	int i;
	int prod=t->px*t->pz;
	int headerlen=2*sizeof(int);
	*len=prod+headerlen;
	unsigned char *buff=(unsigned char *)memzalloc(*len);
	int *ibuff=(int *)buff;
	ibuff[0]=t->px;
	ibuff[1]=t->pz;
	for (i=0;i<prod;i++)
		buff[headerlen+i]=t->dum[i];
	return buff;
}

newtrk *readnewtrack(unsigned char *buff,int len)
{
	int i;
	int headerlen=2*sizeof(int);
	int *ibuff=(int *)buff;
	newtrk *r;
	r=(newtrk *)memzalloc(sizeof(newtrk));
	r->px=ibuff[0];
	r->pz=ibuff[1];
	int prod=r->px*r->pz;
	r->dum=(int *)memzalloc(sizeof(int)*prod);
	for (i=0;i<prod;i++)
		r->dum[i]=buff[headerlen+i];
	return r;
}
*/
static void initcamball()
{
	camx=CAMBALLW>>1;
	camy=CAMBALLH>>3;
}

static void handlecamball()
{
	float dist;
	if (MBUT) {
		if (MX>=CAMBALLX && MX<CAMBALLX+CAMBALLW && MY>=CAMBALLY && MY<CAMBALLY+CAMBALLH) {
			camx=MX-CAMBALLX;
			camy=MY-CAMBALLY;
		}
	}
	dist=(CLOSEDIST*float(camtween)+FARDIST*(100-float(camtween)))/100;
	constructor_viewportn.camrot.x=TWOPI*((float(camy)/CAMBALLH-.5f)*.49f+.25f);
	constructor_viewportn.camrot.y=TWOPI*(float(camx)/CAMBALLW+.5f)+camfocusdir*PIOVER180;
	float cosy=cosf(constructor_viewportn.camrot.x);
	float siny=sinf(constructor_viewportn.camrot.x);
	constructor_viewportn.camtrans.x=camfocus.x-dist*cosy*sinf(constructor_viewportn.camrot.y);
	constructor_viewportn.camtrans.y=camfocus.y+dist*siny;
	constructor_viewportn.camtrans.z=camfocus.z-dist*cosy*cosf(constructor_viewportn.camrot.y);
}

static void drawcamball()
{
	cliprect32(B32,580,240,150,160,C32BLUE);
	clipcircle32(B32,camx+CAMBALLX,camy+CAMBALLY,5,C32RED);
}

#define NUMDT 3
static const C8* dumtexn[NUMDT]={	"rvrbnkshrubs.dds","track02b.dds","grnd02m.dds"};
static modelb* dumm;
static textureb* dumt[NUMDT];
static void builddum()
{
pushandsetdirdown("constructed");
	dumm=model_create("dumm");
	S32 i;
	for (i=0;i<NUMDT;++i) {
		dumt[i]=texture_create(dumtexn[i]);
		if (textureb::rc.getrc(dumt[i])==1)
			dumt[i]->load();
	}
popdir();
}

static void freedum()
{
	modelb::rc.deleterc(dumm);
	S32 i;
	for (i=0;i<NUMDT;++i)
		textureb::rc.deleterc(dumt[i]);
}

void newconstructorinit()
{
pushandsetdir("newcarenadata");
	consmode=MODE_NORMAL;
//	struct tree *rt;//,*scn,*obj;
	logger("---------------------- constructor init -------------------------------\n");
// video
	video_setupwindow(800,600);//,565);
	builddum();
/*	MX=MY=0; // hack to get the cursor off of the track initialy
	if (video_maindriver==VIDEO_MAINDRIVER_D3D) {
		uselights=1;
		usescnlights=1;
		dodefaultlights=0;
	} else {
		uselights=0;
		usescnlights=0;
//		uselights=1;
//		dodefaultlights=0;
//		favorshading=1;
	} */
//	perf_start(TEST3);
// setup viewport
	constructor_viewportn=constructor_viewport;
	constructor_viewportn.backcolor=C32GREEN;
	constructor_viewportn.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
//	rt=alloctree(10000,0);
//	mystrncpy(rt->name,"roottree",NAMESIZE);
	constructor_viewport_roottree=new tree2("roottree");
//	constructor_viewport.roottree=rt;
	constructor_viewportn.xstart=TRACKVIEWX;
	constructor_viewportn.ystart=TRACKVIEWY;
	constructor_viewportn.xres=TRACKVIEWW;
	constructor_viewportn.yres=TRACKVIEWH;
	constructor_viewportn.zfront=.1f;
	constructor_viewportn.zback=2000;
//	constructor_viewport.camattach=0;//getlastcam();
/*	constructor_viewport3.backcolor=frgbbrown;
	constructor_viewport3.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
	rt=alloctree(10000,0);
	mystrncpy(rt->name,"roottree",NAMESIZE);
	constructor_viewport3.roottree=rt;
	constructor_viewport3.xstart=TRACKVIEWX2;
	constructor_viewport3.ystart=TRACKVIEWY2;
	constructor_viewport3.xres=TRACKVIEWW2;
	constructor_viewport3.yres=TRACKVIEWH2;
	constructor_viewport3.zfront=.1f;
	constructor_viewport3.zback=2000;
	constructor_viewport3.camattach=0;//getlastcam(); */
//	vp.camattach->camtarget=mt;
//	setviewportsrc(&constructor_viewport); // user calls this
	constructor_viewportn.xsrc=4;constructor_viewportn.ysrc=3;
	constructor_viewportn.useattachcam=false;
//	constructor_viewportn.lookat=0;
//	setviewportsrc(&constructor_viewport3); // user calls this
// resource
pushandsetdirdown("constructed");
//	rl=loadres("newconstructorres.txt");
//	rlnn=loadres("newnameres.txt");
	factory2<shape> fact;
	script* msc=new script("newconstructorres.txt");
	rl=fact.newclass_from_handle(*msc);
	delete msc;
	msc=new script("newnameres.txt");
	rlnn=fact.newclass_from_handle(*msc);
	delete msc;
	TEXTTRACK=rl->find<text>("TEXTTRACK");
	PBUTBACKSAVE=rl->find<pbut>("PBUTBACKSAVE");
	PBUTBACKNOSAVE=rl->find<pbut>("PBUTBACKNOSAVE");
	PBUTBACKDELETE=rl->find<pbut>("PBUTBACKDELETE");
	PBUTCOPY=rl->find<pbut>("PBUTCOPY");
	PBUTCLEAR=rl->find<pbut>("PBUTCLEAR");
	PBUTOUTLINE=rl->find<pbut>("PBUTOUTLINE");
	LISTTRACKSECT=rl->find<listbox>("LISTTRACKSECT");
	PBUTTRACKTSECTADDAFTER=rl->find<pbut>("PBUTTRACKTSECTADDAFTER");
	PBUTTRACKSECTADDBEFORE=rl->find<pbut>("PBUTTRACKSECTADDBEFORE");
	PBUTDELSECT=rl->find<pbut>("PBUTDELSECT");
	LISTSECTPRIM=rl->find<listbox>("LISTSECTPRIM");
	PBUTCOPYPRIM=rl->find<pbut>("PBUTCOPYPRIM");
	PBUTDELPRIM=rl->find<pbut>("PBUTDELPRIM");
	VSLIDERLEN=rl->find<vscroll>("VSLIDERLEN");
	VSLIDERHEADING=rl->find<vscroll>("VSLIDERHEADING");
	VSLIDERPITCH=rl->find<vscroll>("VSLIDERPITCH");
	VSLIDERBANK=rl->find<vscroll>("VSLIDERBANK");
	VSLIDERINWID=rl->find<vscroll>("VSLIDERINWID");
	VSLIDEROUTWID=rl->find<vscroll>("VSLIDEROUTWID");
	TEXTPRIM2=rl->find<text>("TEXTPRIM2");
	TEXTPRIMNAME=rl->find<text>("TEXTPRIMNAME");
	EDITPRIMNAME=rl->find<edit>("EDITPRIMNAME");
	LISTSECTLIST=rl->find<listbox>("LISTSECTLIST");
	PBUTDELSECTLIST=rl->find<pbut>("PBUTDELSECTLIST");
	PBUTSWAPVIEWPORT=rl->find<pbut>("PBUTSWAPVIEWPORT");
	VSLIDERDIRHEADING=rl->find<vscroll>("VSLIDERDIRHEADING");
	VSLIDERDIRPITCH=rl->find<vscroll>("VSLIDERDIRPITCH");
	TEXTLIGHTS=rl->find<text>("TEXTLIGHTS");
	PBUTMM=rl->find<pbut>("PBUTMM");
	VSLIDERSMOOTHANG=rl->find<vscroll>("VSLIDERSMOOTHANG");
	VSLIDERCAM=rl->find<vscroll>("VSLIDERCAM");
	EDITNAME=rlnn->find<edit>("EDITNAME");
	PBUTNAMEOK=rlnn->find<pbut>("PBUTNAMEOK");
	PBUTNAMECAN=rlnn->find<pbut>("PBUTNAMECAN");
	VSLIDERLEN->setnumidx(101);
	VSLIDERHEADING->setnumidx(101);
	VSLIDERPITCH->setnumidx(101);
	VSLIDERBANK->setnumidx(101);
	VSLIDERINWID->setnumidx(101);
	VSLIDEROUTWID->setnumidx(101);
	if (!lobby_track[0])
//		strcpy(lobby_track,"newdefault");
		lobby_track="newdefault";
//	setresname(rl,TEXTTRACK,lobby_track);
	TEXTTRACK->settname(lobby_track.c_str());
//	setresrange(rl,VSLIDERDIRHEADING,0,36);
	VSLIDERDIRHEADING->setminmaxval(0,36);
//	setresrange(rl,VSLIDERDIRPITCH,-9,9);
	VSLIDERDIRPITCH->setminmaxval(-9,9);
//	setresrange(rl,VSLIDERSMOOTHANG,0,90);
	VSLIDERSMOOTHANG->setminmaxval(0,90);
//	setresrange(rl,VSLIDERCAM,0,100);
	VSLIDERCAM->setminmaxval(0,100);
	camtween=90;
	VSLIDERCAM->setidx((S32)camtween);
	initcamball();
//	inittracklist();
	tl = new tracklist;
//	perf_end(TEST3);
// load and build track
	trackscene=loadnewconstrack(lobby_track.c_str());
	if (trackscene)
//		linkchildtoparent(trackscene,constructor_viewport.roottree);
		constructor_viewport_roottree->linkchild(trackscene);
//	LISTSECTPRIM->setidxc(0);
	updatesectionlist(nthetrack);
//	setcamfocus(nthetrack,getcurlistbox(rl,LISTSECTPRIM));
	setcamfocus(nthetrack,LISTSECTPRIM->getidx());
//	setcurlistbox(rl,LISTSECTPRIM,nthetrack->nsections-1,VSCROLLSECTPRIM);
// build a track piece
/*	onesection=initnewtrackscript();
	tracks=buildtrackpiece(onesection);
	if (tracks)
		linkchildtoparent(tracks,constructor_viewport3.roottree);*/
//	initsectionlist(nthetrack);
//	setresval(rl,LISTPIECETYPES,0);
	if (tl->size()<=1)
//		setresvis(rl,PBUTBACKDELETE,0);
		PBUTBACKDELETE->setvis(false);
//	amblight=allochelper(0,HELPER_AMBLIGHT,0);
	amblight=new tree2("amblight");
	amblight->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	amblight->intensity=.25f;
	addlighttolist(amblight);
	constructor_viewport_roottree->linkchild(amblight);
//	linkchildtoparent(amblight,ret);
//	dirlight=allochelper(0,HELPER_LIGHT,0);
	dirlight=new tree2("dirlight");
	dirlight->flags|=TF_ISLIGHT;
	dirlight->intensity=.95f;
	addlighttolist(dirlight);
	constructor_viewport_roottree->linkchild(dirlight);
//	linkchildtoparent(dirlight,ret);
//	logviewport(&constructor_viewport,OPT_SOME);
//	logrc();
//	constructor_viewport_roottree->log2();
//	logviewport(&constructor_viewport3,OPT_SOME);
popdir();
	focus=LISTSECTPRIM;
}

void newconstructorproc()
{
	static int swapviewport;
	char str[70];
//	struct rmessage rm;
// process resources
	switch(consmode) {
	case MODE_NORMAL:
		if (wininfo.mleftclicks)
			focus=rl->getfocus();
		if (!focus)
			focus=LISTSECTPRIM;
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
//			case PBUTBACKSAVE:
			if (focusb==PBUTBACKSAVE) {
//				mystrncpy(lobby_track,getresname(rl,TEXTTRACK),NAMESIZE);
				lobby_track=TEXTTRACK->gettname();
				savenewconstrack(lobby_track.c_str(),nthetrack,0);
				popstate();
//				break;
//			case PBUTBACKNOSAVE:
			} else if (focusb==PBUTBACKNOSAVE) {
				popstate();
//				break;
//			case PBUTBACKDELETE:
			} else if (focusb==PBUTBACKDELETE) {
				sprintf(str,"%s.trk",lobby_track.c_str());
pushandsetdirdown("constructed");
				int ret=remove(str);
popdir();
				logger("remove '%s' ret is %d\n",str,ret);
				if (ret!=0) {
					logger("errno = %d\n",errno);
				}
				popstate();
//				break;
// copy name (rename)
//			case PBUTCOPY:
			} else if (focusb==PBUTCOPY) {
				consmode=MODE_NEWNAME;
//				settextfocus(rlnn,EDITNAME);
				focus=EDITNAME;
//				setresname(rlnn,EDITNAME,uniquetrackname(getresname(rl,TEXTTRACK)));
				EDITNAME->settname(tl->uniquetrackname(TEXTTRACK->gettname()).c_str());
//				setresname(rlnn,EDITNAME,"newname");
//				break;
// clear
//			case PBUTCLEAR:
			} else if (focusb==PBUTCLEAR) {
//				cleartrack();
//				rebuildtrackandpiece();
//				break;
// change draw mode
//			case PBUTOUTLINE:
			} else if (focusb==PBUTOUTLINE) {
				outline^=1;
//				break;
// change basetex
//			case VSLIDERBASETEX:
//				thetrack.trkdata.basetex=rm.val;
//				rebuildtrackandpiece();
//				updatebasetexres();
//				break;
//			case LISTPIECETYPES:
//				cpce.pt=piecetype(rm.val);
//				rebuildpiece();
//				break;
// primitive builder
//			case LISTSECTPRIM:
			} else if (focusl==LISTSECTPRIM) {
				updatesectionlist(nthetrack);
//				setcurlistbox(rl,LISTSECTPRIM,getcurlistbox(rl,LISTSECTPRIM),VSCROLLSECTPRIM);
//				setcamfocus(nthetrack,getcurlistbox(rl,LISTSECTPRIM));
				setcamfocus(nthetrack,LISTSECTPRIM->getidx());
//				break;
//			case VSCROLLSECTPRIM:
//				setlistboxoffset(rl,LISTSECTPRIM,rm.val);
//				break;
//			case VSLIDERLEN:
			} else if (focush==VSLIDERLEN) {
				modifyprim(nthetrack);
//				break;
//			case VSLIDERHEADING:
			} else if (focush==VSLIDERHEADING) {
				modifyprim(nthetrack);
//				break;
//			case VSLIDERPITCH:
			} else if (focush==VSLIDERPITCH) {
				modifyprim(nthetrack);
//				break;
//			case VSLIDERBANK:
			} else if (focush==VSLIDERBANK) {
				modifyprim(nthetrack);
//				break;
//			case VSLIDERINWID:
			} else if (focush==VSLIDERINWID) {
				modifyprim(nthetrack);
//				break;
//			case VSLIDEROUTWID:
			} else if (focush==VSLIDEROUTWID) {
				modifyprim(nthetrack);
//				break;
//			case PBUTCOPYPRIM:
			} else if (focusb==PBUTCOPYPRIM) {
				copyprim(nthetrack);
//				setcamfocus(nthetrack,getcurlistbox(rl,LISTSECTPRIM));
				setcamfocus(nthetrack,LISTSECTPRIM->getidx());
//				break;
//			case PBUTDELPRIM:
			} else if (focusb==PBUTDELPRIM) {
				deleteprim(nthetrack);
//				setcamfocus(nthetrack,getcurlistbox(rl,LISTSECTPRIM));
//				setcamfocus(nthetrack,getcurlistbox(rl,LISTSECTPRIM));
				setcamfocus(nthetrack,LISTSECTPRIM->getidx());
//				break;
//			case PBUTSWAPVIEWPORT:
			} else if (focusb==PBUTSWAPVIEWPORT) {
				swapviewport^=1;
//			case VSLIDERDIRHEADING:
			} else if (focush==VSLIDERHEADING) {
				S32 rm=VSLIDERHEADING->getidx();
				dirlight->rot.y=rm*10.0f*PIOVER180;
//				break;
//			case VSLIDERDIRPITCH:
			} else if (focush==VSLIDERDIRPITCH) {
				S32 rm=VSLIDERPITCH->getidx();
				dirlight->rot.x=rm*10.0f*PIOVER180;
//				break;
//			case VSLIDERSMOOTHANG:
			} else if (focush==VSLIDERSMOOTHANG) {
				S32 rm=VSLIDERSMOOTHANG->getidx();
				tracksmoothang=rm*PIOVER180;
//				break;
//			case PBUTMM:
			} else if (focusb==PBUTMM) {
				pushchangestate(STATE_MAINMENU);
//				break;
//			case VSLIDERCAM:
			} else if (focush==VSLIDERCAM) {
				S32 rm=VSLIDERCAM->getidx();
				camtween=rm;
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
//			case EDITNAME:
			if (focuse==EDITNAME || focusb==PBUTNAMEOK) {
//				setresname(rlnn,EDITNAME,uniquetrackname(getresname(rlnn,EDITNAME)));
				EDITNAME->settname(tl->uniquetrackname(EDITNAME->gettname()).c_str());
				consmode=MODE_NORMAL;
				focus=0;
//				setresname(rl,TEXTTRACK,getresname(rlnn,EDITNAME));
				TEXTTRACK->settname(EDITNAME->gettname());
//				break;
			} else if (focusb==PBUTNAMECAN) {
				consmode=MODE_NORMAL;
				focus=0;
			}

		}
		break;
	}
// handle cam ball
	handlecamball();
// rebuild track
//	setmodelsmoothang(tracksmoothang);
	if (trackscene) {
//		unhooktree(dirlight);
		dirlight->unlink();
//		unhooktree(amblight);
		amblight->unlink();
//		unhooktree(trackscene);
//		freetree(trackscene);
		delete trackscene;
	}
	trackscene=buildnewconsscene(nthetrack);
	if (trackscene)
//		linkchildtoparent(trackscene,constructor_viewport.roottree);
		constructor_viewport_roottree->linkchild(trackscene);
//	linkchildtoparent(dirlight,constructor_viewport.roottree);
	constructor_viewport_roottree->linkchild(dirlight);
//	linkchildtoparent(amblight,constructor_viewport.roottree);
	constructor_viewport_roottree->linkchild(amblight);
/*	if (tracks) {
		unhooktree(tracks);
		freetree(tracks);
	}
	tracks=buildtrackpiece(onesection);
	if (tracks)
		linkchildtoparent(tracks,constructor_viewport3.roottree); */
//	calcgrid(&constructor_viewport2,MX,MY,&grid);
// update display
//	updatecursor();
// handle swap viewport
/*	if (swapviewport) {
		constructor_viewport3.xstart=TRACKVIEWX;
		constructor_viewport3.ystart=TRACKVIEWY;
		constructor_viewport3.xres=TRACKVIEWW;
		constructor_viewport3.yres=TRACKVIEWH;
		constructor_viewport.xstart=TRACKVIEWX2;
		constructor_viewport.ystart=TRACKVIEWY2;
		constructor_viewport.xres=TRACKVIEWW2;
		constructor_viewport.yres=TRACKVIEWH2;
	} else {
		constructor_viewport.xstart=TRACKVIEWX;
		constructor_viewport.ystart=TRACKVIEWY;
		constructor_viewport.xres=TRACKVIEWW;
		constructor_viewport.yres=TRACKVIEWH;
		constructor_viewport3.xstart=TRACKVIEWX2;
		constructor_viewport3.ystart=TRACKVIEWY2;
		constructor_viewport3.xres=TRACKVIEWW2;
		constructor_viewport3.yres=TRACKVIEWH2;
	}
*/
	doflycam(&constructor_viewportn);
//	doflycam(&constructor_viewport3);
//	doanims(constructor_viewport_roottree);
	constructor_viewport_roottree->proc();
//	buildtreematrices(constructor_viewport_roottree);
	video_buildworldmats(constructor_viewport_roottree);
//	doanims(constructor_viewport3.roottree);
//	buildtreematrices(constructor_viewport3.roottree);
}

void newconstructordraw2d()
{
//	video_lock();
	switch(consmode) {
	case MODE_NORMAL:
//		drawres(rl);
		rl->draw();
		drawcamball();
		break;
	case MODE_NEWNAME:
//		drawres(rlnn);
		rlnn->draw();
		break;
	}
	if (outline) {
		static int cnt;
		if (cnt&32) {
			if (track)
				drawlines(&constructor_viewportn,track->mod,C32RED);
			if (backgnd)
				drawlines(&constructor_viewportn,backgnd->mod,C32WHITE);
		} else {
			if (backgnd)
				drawlines(&constructor_viewportn,backgnd->mod,C32WHITE);
			if (track)
				drawlines(&constructor_viewportn,track->mod,C32RED);
		}
/*		if (tracks->mod)
			drawlines(&constructor_viewport3,tracks->mod,hired); */
		cnt++;
	}
//	video_unlock();
}

void newconstructordraw3d()
{
	if (!outline) {
//		video_beginscene(&constructor_viewport);
//		video_drawscene(&constructor_viewport);
//		video_endscene(&constructor_viewport);
/*		video_beginscene(&constructor_viewport3);
		video_drawscene(&constructor_viewport3);
		video_endscene(&constructor_viewport3); */

		constructor_viewportn.flags=VP_CLEARWB|VP_CHECKER|VP_CLEARBG;
		constructor_viewportn.xres=WX;
		constructor_viewportn.yres=WY;
		constructor_viewportn.xstart=0;
		constructor_viewportn.ystart=0;
		constructor_viewportn.backcolor=C32(0,85,0);
		video_setviewport(&constructor_viewportn); // clear zbuf etc.

		lobby_viewport.flags=VP_CHECKER|VP_CLEARBG;
		constructor_viewportn.xres=TRACKVIEWW;
		constructor_viewportn.yres=TRACKVIEWH;
		constructor_viewportn.xstart=TRACKVIEWX;
		constructor_viewportn.ystart=TRACKVIEWY;
		constructor_viewportn.backcolor=C32GREEN;
		video_setviewport(&constructor_viewportn); // clear zbuf etc.
		video_drawscene(constructor_viewport_roottree);
	}
}

void newconstructorexit()
{
	if (nthetrack) {
		freenewtrackscript(nthetrack);
		nthetrack=0;
	}
/*	if (onesection) {
		freenewtrackscript(onesection);
		onesection=0;
	} */
	logger("---------------------- constructor exit -------------------------------\n");
//	logviewport(&constructor_viewport,OPT_SOME);
//	logviewport(&constructor_viewport3,OPT_SOME);
	logrc();
	constructor_viewport_roottree->log2();
// free everything
//	freeres(rl);
	delete rl;
//	freeres(rlnn);
	delete rlnn;
//	freetree(constructor_viewport.roottree);
	delete constructor_viewport_roottree;
	constructor_viewport_roottree=0;
//	freetree(constructor_viewport3.roottree);
//	constructor_viewport3.roottree=0;
//	popdir();
//	freetracklist();
	delete tl;
popdir();
	freedum();
}
