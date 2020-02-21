// augmented reality using a web cam

// for vfw.h
#define INCLUDE_WINDOWS

#define RES3D // for 3d UI
#include <m_eng.h>
#include <l_misclibm.h>
#include <vfw.h>

#include "u_states.h"
#include "d2_font.h"
#include "u_modelutil.h"

#include "u_s_ar.h"
#include "u_ar_scan.h"
#include "u_ar_parse.h"

#include "helperobj.h"

using namespace ar_scan;

#define USENS // use namespace, turn off for debugging VS2005
#ifdef USENS
namespace u_s_ar {
#endif

//#define SHOWHELPERS // scan and reference spheres
#define SHOWVR // 3d object (animated)

// UI
shape *rl,*focus=0;
pbut* quit;
pbut* vcstart,*see,*click,*vcstop,*loadp,*loadm,*scan,*cscan,*del,*vcquit;
pbut* pvidcomp,*pvidformat,*pvidsource;
text* tstatus;
bool contscan;

doar *adoar;
arparseinfo arpi; // read only structure of state of ar calc
S32 resetrottrans = 0; // if non zero, reset the calc
S32 calcmatch; // how many to calc
S32 fastcalcmatch; // how many to calc in 1 frame


struct menuvar ar_edv[]={
// user vars
	{"@green@--- AR SCAN USER VARS ---",NULL,D_VOID,0},
	{"calc",&calcmatch,D_INT,1},
	{"fastcalc",&fastcalcmatch,D_INT,1},
	{"reset",&resetrottrans,D_INT,1},
	{"@lightgreen@--- AR SCAN USER VARS ---",NULL,D_VOID,0},
	{"rotx",&arpi.arrot.x,D_FLOAT|D_RDONLY},
	{"roty",&arpi.arrot.y,D_FLOAT|D_RDONLY},
	{"rotz",&arpi.arrot.z,D_FLOAT|D_RDONLY},
	{"transx",&arpi.artrans.x,D_FLOAT|D_RDONLY},
	{"transy",&arpi.artrans.y,D_FLOAT|D_RDONLY},
	{"transz",&arpi.artrans.z,D_FLOAT|D_RDONLY},
	{"match error",&arpi.matcherror,D_FLOAT|D_RDONLY},
	{"stable",&arpi.matchstable,D_INT|D_RDONLY},
	{"bigerror",&arpi.bigerror,D_INT|D_RDONLY},
	{"step",&arpi.matchstep,D_FLOAT|D_RDONLY},
	{"current",&arpi.currentstr,D_STRING},
	{"@lightblue@--- AR SCAN VISION VARS ---",NULL,D_VOID,0},
	{"blackwhitethresh",&blackwhitethresh,D_INT,1},
#ifdef FIVEDOTS
	{"darkratio",&darkratio,D_FLOAT,FLOATUP/256},
#endif
};

// bitmaps
struct bitmap32* cbm32; // owned by vcam
struct bitmap32* gallbm,*scanbm; // owned by u_s_ar.cpp

// results
string messageresult;

// counter
S32 frame;

// trees
tree2* roottree1,*roottree2,*backdrop,*backdrop2;
tree2* reftree;

// models
modelb* model0,*modelback;
// textures
textureb* texmat0,*texmatb;

// camera
float zoomf = 1.0f; // 90 degree FOV vertical
float camz = 3.0f; // how far back
float camxy;

// helper objects
helperobj* hobj;
// UI, layout defined using this system
#define SPTRESX 800
#define SPTRESY 600

// gallery
class gallery {
	vector<string> gall;
	S32 curidx; // -1 if none
	U32 nextsave; // unused, number
public:
	string load();
	string loadp();
	string loadm();
	string save();
	string del();
	string getname();
	void setname(string name);
	gallery();
};

gallery* galli;

// set curidx to this name, unchanged otherwise
void gallery::setname(string namea)
{
	U32 i,n=gall.size();
	for (i=0;i<n;++i) {
		if (!namea.compare(gall[i])) {
			curidx = i;
			return;
		}
	}
}

gallery::gallery() : curidx(-1),nextsave(0)
{
	scriptdir sc(0);
	S32 i,j;
	for (i=0;i<sc.num();++i) {
		bool goodext = false;
		C8 ext[256];
		mgetext(sc.idx(i).c_str(),ext,256);
		if (!my_stricmp(ext,"jpg"))
			goodext = true;
		else if (!my_stricmp(ext,"png"))
			goodext = true;
		if (goodext) {
			const string& f = sc.idx(i); // look for grcodennnnn.nnn, numbered .pngs
			if (f.size() == 15) {
				string f2 = f;
				for (j=0;j<(S32)f.size();++j) {
					if (f2[j] >= '0' && f2[j] <= '9')
						f2[j] = '0';
				}
				if (f2 == "arcode00000.png") {
					string ss = f.substr(6,5);
					U32 v = atoi(ss.c_str());
					// logger("found a numbered arcode '%s' %'\n",f.c_str(),v);
					if (v >= nextsave) // unused file
						nextsave = v + 1;
				}
			}
			gall.push_back(f);
		}
	}
	::sort(gall.begin(),gall.end());
//	for (i=0;i<(S32)gall.size();++i)
//		logger("gall[%d] = '%s'\n",i,gall[i].c_str());
	logger("next gallery save number = %d\n",nextsave);
}

// name of file to save click to
string gallery::save()
{
	C8 rc[100];
	sprintf(rc,"arcode%05d.png",nextsave);
	gall.push_back(rc);
	++nextsave;
	::sort(gall.begin(),gall.end());
	vector<string>::iterator res = ::find(gall.begin(),gall.end(),rc);
	curidx = res - gall.begin();
	return gall[curidx];
}

string gallery::del()
{
	if (gall.empty())
		return "";
	if (curidx == -1)
		return "";
	gall.erase(gall.begin()+curidx);
	if (curidx >= (S32)gall.size())
		curidx = gall.size()-1;
	if (curidx == -1)
		return "";
	return gall[curidx];
}

string gallery::loadp()
{
	if (gall.empty()) {
		curidx = -1;
		return "";
	}
	++curidx;
	if (curidx >= (S32)gall.size())
		curidx = 0;
	return gall[curidx];
}

string gallery::loadm()
{
	if (gall.empty()) {
		curidx = -1;
		return "";
	}
	--curidx;
	if (curidx < 0)
		curidx = (S32)gall.size() - 1;
	return gall[curidx];
}

string gallery::getname()
{
	if (gall.empty())
		return "";
	if (curidx == -1)
		return "";
	return gall[curidx];
}

string gallery::load()
{
	if (gall.empty()) {
		curidx = -1;
		return "";
	}
	if (curidx == -1)
		curidx = 0;
	return gall[curidx];
}

void freebms()
{
	if (scanbm) {
		bitmap32free(scanbm);
		scanbm = 0;
	}
	if (gallbm) {
		bitmap32free(gallbm);
		gallbm = 0;
	}
	contscan = false;
}

void vector_tests()
{
	vector<S32> a = vector<S32>(10);
	a[9] = 37;
}

// set models of tree to this color
void settreecolorsub(tree2* t,const pointf3& c)
{
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i) {
		tree2* subtree;
		subtree = *i;
		subtree->treecolor=c;
	}
}

void remove_backdrop()
{
	delete backdrop;
	delete backdrop2;
}

// 2 copies of backdrop for 'ghost' zbuffer effect
// alters uv so it works with a power of 2 texture
void add_backdrop(S32 cx,S32 cy,S32 px,S32 py,bool init)
{
	float uf = (float)cx/px;
	float vf = (float)cy/py;
// build camera backdrop plane
	float backsize = camxy*2.0f;
	const C8* tname = init ? "maptestnck.tga" : "backdrop";
	backdrop = buildplane_xy_uv(pointf2x(1.0f,1.0f),pointf2x(uf,vf),tname,"tex","backdrop1");
	modelback = backdrop->mod;
	modelback->mats[0].msflags &= ~SMAT_HASWBUFF;
	texmatb = modelback->mats[0].thetexarr[0];
	texmatb->name = "backdrop";
	backdrop->scale = pointf3x(backsize*(float)cx/cy,backsize,1.0f); // assume camera has same aspect ratio as window
	backdrop->trans = pointf3x(0,0,camz);
	roottree1->linkchild(backdrop);
	backdrop2 = buildplane_xy_uv(pointf2x(1.0f,1.0f),pointf2x(uf,vf),"backdrop","tex","backdrop2");
	backdrop2->scale = pointf3x(backsize*(float)cx/cy,backsize,1.0f); // assume camera has same aspect ratio as window
	backdrop2->trans = pointf3x(0,0,camz);
	backdrop2->treecolor = pointf3x(1.0f,1.0f,1.0f,.5f);
	backdrop2->setvis(false);
	roottree2->linkchild(backdrop2);
}

string doanarscan(string name,const bitmap32* bm,vector<circleScan>& cs)
{
	adoar->doresetrottrans();
	string mess = readarcode(name,bm,cs,&scanbm);
	//logger("mess result = '%s'\n",mess.c_str());
	hobj->reset();
// show the list of circles
//logger("list of scaned circles\n");
	S32 i;
#ifdef SHOWHELPERS
	for (i=0;i<(signed)cs.size();++i) {
//logger("scan %3d : x = %4d, y = %4d, r = %d\n",i,ret[i].x,ret[i].y,ret[i].r);
		ho->addsphere(roottree2,
		  pointf3x(
		    2.0f*camz*(cs[i].x-scanbm->size.x/2)/(float)scanbm->size.y/zoomf,
		    2.0f*camz*(scanbm->size.y/2-cs[i].y)/(float)scanbm->size.y/zoomf,
			camz),
		  (2.0f*camz/(float)scanbm->size.y)*cs[i].r/zoomf,
		  F32WHITE);
	}
#endif
// got 5 points, make sense of them
	if (cs.size() == doar::npc) {
		pointf2 sp[doar::npc];
		for (i=0;i<doar::npc;++i) {
			sp[i].x = (float)cs[i].x;
			sp[i].y = (float)cs[i].y;
		}
		adoar->setscanpoints(sp);
	} else {
		adoar->doresetrottrans();
	}
	adoar->getarparseinfo(arpi);
	return mess;
}

void ar_init3d()
{
// init 3d
	pushandsetdir("gfxtest");
	roottree1=new tree2("roottree1");
	roottree2=new tree2("roottree2");
// set fov from ar_parse
	zoomf = doar::zoomfactor;
	camxy = camz / zoomf;
// backdrop system
	add_backdrop(1,1,1,1,true);
#if 0
// build a 3d object base, coord axis
	tree2* testobjbox = buildprism(pointf3x(.25f/zoomf,.25f/zoomf,.25f/zoomf),"maptestnck.tga","tex","axis");
	model0 = testobjbox->mod;
	texmat0 = model0->mats[0].thetexarr[0];
	//testobjbase->rotvel.y=.01f;
	//testobjbase->rotvel.x=.002f;
	testobjbox->trans = pointf3x(0,0,camz);
// build a 3d object y
	tree2* testobjbase = buildcylinder_xz(pointf3x(.1f/zoomf,2.0f/zoomf),"maptestnck.tga","tex");
	tree2* testobj = testobjbase;
	settreecolorsub(testobj,pointf3x(0.0f,1.0f,0.0f));
	testobjbox->linkchild(testobj); // testobjbox owns testobj and reference testobjbase
// build a 3d object x, the dimensions don't matter since we have
	testobj = testobjbase->newdup(); // now test obj owns itself
	//buildcylinder_xz(pointf3x(1.0f,1.0f,1.0f),"maptestnck.tga","tex");
	settreecolorsub(testobj,pointf3x(1.0f,0.0f,0.0f));
	testobj->rot.z = -PI/2.0f;
	testobjbox->linkchild(testobj); // pass ownership
// build a 3d object z
	testobj = testobjbase->newdup();
	//testobj = buildcylinder_xz(pointf3x(1.0f,1.0f,1.0f),"maptestnck.tga","tex");
	settreecolorsub(testobj,pointf3x(0.0f,1.0f,1.0f));
	testobj->rot.x = PI/2.0f;
	testobjbox->linkchild(testobj); // everything owned by testobjbox, actually roottree2
	roottree2->linkchild(testobjbox);
#endif
// build reference tree scene
	reftree = new tree2("reftree");
	reftree->trans.z = 5.0f; // start transz from u_ar_parse

	tree2* chld = buildsphere(.35f/zoomf,"bark.tga","tex");
#ifndef SHOWHELPERS
	chld->flags |= TF_DONTDRAWC;
#endif
	chld->trans.x = 0;
	float refs = 1.0f; // camxy;
	chld->trans.y = refs;
	reftree->linkchild(chld);

	chld = chld->newdup();
	chld->trans.x = refs;
	chld->trans.y = refs;
	reftree->linkchild(chld);

	chld = chld->newdup();
	chld->trans.x = refs;
	chld->trans.y = -refs;
	reftree->linkchild(chld);

	chld = chld->newdup();
	chld->trans.x = -refs;
	chld->trans.y = -refs;
	reftree->linkchild(chld);

	chld = chld->newdup();
	chld->trans.x = -refs;
	chld->trans.y = refs;
	reftree->linkchild(chld);

	popdir();

#ifdef SHOWVR
// add a vr 3d object
	pushandsetdir("racenetdata/prehistoric");
	tree2* scene = new tree2("prehistoric.BWS");
	tree2* mod_scene;
	//mod_scene = scene->find("PTER_loco.bwo");
	mod_scene = scene->find("PTbody01.bwo");
	mod_scene->unlink();
	pushandsetdir("gfxtest");
	float tz = .05f;
	tree2* mod_base = buildprism(pointf3x(2.0f,2.0f,tz),"maptestnck.tga","tex","base");
	mod_base->trans.z = tz *-.5f;
	mod_base->treecolor = pointf3x(1.0,1.0,1.0,.5f);
	popdir();
	tree2* mod_scenep = new tree2("modsceneparent");
//	mod_scenep->rot = pointf3x(PI,0.0f,0.0f);
	mod_scenep->rot = pointf3x(PI,0.0f,PI*.5f);
//	mod_scenep->rot = pointf3x(0.0f,0.0f,PI*.5f);
	float sc = 5.0f;
	mod_scenep->scale = pointf3x(sc,sc,sc);
	mod_scenep->trans.z = -tz -.165f;
	mod_scenep->linkchild(mod_scene);
	reftree->linkchild(mod_base);
	reftree->linkchild(mod_scenep);
	delete scene;
	popdir();
#endif
	reftree->trans.x = 1000000.0f; // hide this scene by putting it far away
	roottree2->linkchild(reftree);

// build main viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.0025f;
	mainvp.zback=10000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camrot=pointf3x(0);
	mainvp.camzoom=zoomf;
//#define TRANS_PREHISTORIC // artists put the whole thing way over here
#ifdef TRANS_PREHISTORIC
	mainvp.camtrans.x=-937;
	mainvp.camtrans.y=463;
	mainvp.camtrans.z=203;
#else
	mainvp.camtrans.x=0;
	mainvp.camtrans.y=0;
	mainvp.camtrans.z=0;
#endif
	//mainvp.flags=VP_CLEARWB;
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
	// vector tests
	vector_tests();
	hobj = new helperobj();
	seq_start(roottree2); // turn on animation

	lightinfo.ambcolor = pointf3x(1.0f,1.0f,1.0f);
	lightinfo.dodefaultlights = 1;
}

#ifdef USENS
}
using namespace u_s_ar;
#endif

void ar_init()
{
	S32 ar_nedv = NUMELEMENTS(ar_edv);
	extradebvars(ar_edv,ar_nedv);
// setup state
	video_setupwindow(1024,768);
	pushandsetdir("ar");
	init_res3d(SPTRESX,SPTRESY);
	rl=res_loadfile("ar_res.txt");
// enum camera devices
	frame=0;
	galli = new gallery();
// find all buttons
	pvidcomp=rl->find<pbut>("PVIDCOMP");
	pvidformat=rl->find<pbut>("PVIDFORMAT");
	pvidsource=rl->find<pbut>("PVIDSOURCE");
	vcstart=rl->find<pbut>("VCSTART");
	see=rl->find<pbut>("SEE");
	click=rl->find<pbut>("SAVE");
	loadp=rl->find<pbut>("LOADP");
	loadm=rl->find<pbut>("LOADM");
	scan=rl->find<pbut>("SCAN");
	cscan=rl->find<pbut>("CSCAN");
	del=rl->find<pbut>("DEL");
	vcstop=rl->find<pbut>("VCSTOP");
	vcquit=rl->find<pbut>("VCQUIT");
	quit=rl->find<pbut>("QUIT");
	tstatus=rl->find<text>("TSTATUS");
	focus=0;
	gallbm = 0;
	scanbm = 0;
	ar_init3d();
	contscan = false;
	adoar = new doar(pointf2x(640,480),pointf2x(4,3));
	calcmatch = 1000000000;
	fastcalcmatch = 10000;
	galli->setname("arcode00030.png");
	ar_test_wider_neighbor1D();
}

void ar_proc3d()
{
	roottree1->proc();
	roottree2->proc();
}

void ar_proc()
{
	S32 i;
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
#if 1
	scan->setvis(gallbm!=0);
	click->setvis(cbm32!=0 && gallbm==0 && (scanbm==0 || contscan));
	del->setvis(gallbm!=0 && scanbm==0);
	//url->setvis(scanbm!=0 && !messageresult.empty());
	cscan->setvis(cbm32!=0 && gallbm==0);
// handle text for CScan button
	if (cscan->getvis()) {
		const S32 maxanim = 8; // assume a power of 2
		const S32 slowdown = 3; // slow down animation of continuous scan button
		if (contscan) {
			// power of 2
			static C8* animstrs[maxanim] = {
				"CS....",
				".CS...",
				"..CS..",
				"...CS.",
				"....CS",
				"...CS.",
				"..CS..",
				".CS...",
			};
			S32 idx = (maxanim-1)&(frame>>slowdown);
			cscan->settname(animstrs[idx]);
		} else {
			cscan->settname("Cscan"); // same as ar_res.txt
		}
	}
#endif
// if something selected...
	bool ss = false;
	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
// set focus
	if (focus) {
		S32 ret=focus->procfocus();
		shape* focusa = ret>=0 ? focus : 0;
		if (focusa==quit) { // quit button
			popstate();
		} else if (focusa==pvidcomp) {
			vcamcompression();
		} else if (focusa==pvidformat) {
			vcamformat();
		} else if (focusa==pvidsource) {
			vcamsource();
		} else if (focusa==vcstart) {
			if (vcaminit())
				tstatus->settname("vcam driver inited");
			else
				tstatus->settname("can't init vcam driver");
		} else if (focusa==loadp) { // load a file into snapshot
			string lf = gallbm && !scanbm ? galli->loadp() : galli->load(); // if gallbm visible then loadp
			freebms();
			gallbm = gfxread32(lf.c_str());
		} else if (focusa==loadm) { // load a file into snapshot
			string lf = gallbm && !scanbm ? galli->loadm() : galli->load(); // if gallbm visible then loadm
			freebms();
			gallbm = gfxread32(lf.c_str());
		} else if (focusa==scan) { // scan a snapshot
			if (scanbm) {
				bitmap32free(scanbm);
				scanbm = 0;
			}
			if (gallbm) {
				vector<circleScan> cs;
				string mess = doanarscan(galli->getname(),gallbm,cs);
				logger("mess result = '%s'\n",mess.c_str());
				messageresult = mess;
// log list of scanned circles
				C8 name[100];
				mgetname(galli->getname().c_str(),name);
				logger("%s = {\n",name);
				logger_indent();
				for (i=0;i<(signed)cs.size();++i) {
					logger("{%d, %d},\n",cs[i].x,cs[i].y);
				}
				logger_unindent();
				logger("};\n");
			}
		} else if (focusa==cscan) { // toggle continuous scan a live preview
			contscan = !contscan;
//		} else if (focusa==url) { // launch a browser from the message
//			ShellExecute(0,"open",messageresult.c_str(),0,0,SW_SHOWNORMAL);
		} else if (focusa==del) { // delete a snapshot
			string delf = galli->getname();
			if (!delf.empty())
				logger("remove '%s'\n",delf.c_str());
			remove(delf.c_str());
			string lf = galli->del(); // remove from list and find next index
			freebms();
			if (!lf.empty())
				gallbm = gfxread32(lf.c_str());
		} else if (focusa==see) { // see live, null out snapshot
			freebms();
			if (vcamstartstream()) {
				tstatus->settname("vcam streaming");
			} else
				tstatus->settname("can't stream vcam");
		} else if (focusa==click) { // load camera to snapshot and save snapshot later
			ss = true;

		} else if (focusa==vcstop) {
			vcamstopstream();
			tstatus->settname("done streaming");
		} else if (focusa==vcquit) {
			vcamstopstream();
			vcamexit();
			tstatus->settname("done with driver");
		}
	}
	cbm32=vcamgrabframe();
	if (contscan) {
			if (scanbm) {
				bitmap32free(scanbm);
				scanbm = 0;
			}
			if (cbm32) {
				vector<circleScan> cs;
				string mess = doanarscan("contscan",cbm32,cs);
				logger("mess result = '%s'\n",mess.c_str());
				messageresult = mess;
			}
	}
	if (cbm32 && ss) {
		freebms();
		gallbm = bitmap32copy(cbm32);
		string ss = galli->save();
		gfxwrite32(ss.c_str(),gallbm);
	}
	frame++;
	ar_proc3d();
	doflycam(&mainvp);
// calc rot and trans by stepping and checking for errors
	if (calcmatch <= 0)
		calcmatch = 0;//fastcalcmatch*10;
	fastcalcmatch = range(1,fastcalcmatch,10000);
	if (resetrottrans) {
		adoar->doresetrottrans();
		resetrottrans = 0;
	}
	for (i=0;i<fastcalcmatch;++i) {
		if (calcmatch > 0) {
			adoar->calcmatchstep();
			--calcmatch;
		}
	}
	adoar->getarparseinfo(arpi);
	if(adoar->isdone()) {
		reftree->rot = arpi.arrot;
		reftree->trans = arpi.artrans;
#ifdef SHOWHELPERS
		//reftree->flags &= ~TF_DONTDRAWC;
#endif
	}
}

void ar_draw3d()
{
	if (scanbm) { // result from image processing
		drawtextque_string_foreback(SMALLFONT,10,30,F32WHITE,F32BLACK,"Scanned image");
		drawtextque_format_foreback(SMALLFONT,10,40,F32WHITE,F32BLACK,"Message = '%s'",messageresult.c_str());
	} else if (gallbm) { // from gallery
		drawtextque_format_foreback(SMALLFONT,10,30,F32WHITE,F32BLACK,"Gallery filename = '%s'",galli->getname().c_str());
	} else if (cbm32) { // live feed
		const C8* format = getformatname();
		drawtextque_format_foreback(SMALLFONT,10,30,F32WHITE,F32BLACK,"Live feed, hit save to put in gallery read to scan '%s'",format);
	} else { // nothing
		drawtextque_format_foreback(SMALLFONT,10,30,F32WHITE,F32BLACK,"Init driver and see");
	}
	drawtextque_format_foreback(SMALLFONT,10,10,F32WHITE,F32BLACK,"vidcap frame %6d",frame);
	video_setviewport(&mainvp); // clear zbuf etc.
	const bitmap32* gbm = cbm32;
	if (scanbm)
		gbm = scanbm;
	else if (gallbm)
		gbm = gallbm;
	if (gbm) {
		clipputpixel32(gbm,0,0,C32BLUE);
		S32 cx = gbm->size.x;
		S32 cy = gbm->size.y;
		S32 px = 1<<ilog2(cx);
		S32 py = 1<<ilog2(cy);
		if (cx > px)
			px <<= 1;
		if (cy > py)
			py <<= 1;
		S32 tx = texmatb->tsize.x;
		S32 ty = texmatb->tsize.y;
		if (tx != px || ty != py) {
// remove backplane
			remove_backdrop();
// add new backplane
			add_backdrop(cx,cy,px,py,false);
			texmatb->build(px,py);
		}
		bitmap32* lt=texmatb->locktexture();
		clipblit32(gbm,lt,0,0,0,0,cx,cy);
		texmatb->unlocktexture();
	}
	video_buildworldmats(roottree1);
	video_buildworldmats(roottree2);
	video_drawscene(roottree1);
	video_drawscene(roottree2);
	video_sprite_begin(
		SPTRESX,SPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
		0,
		0);
	rl->draw3d();
	video_sprite_end();
}

void ar_exit3d()
{
	extradebvars(0,0);
	logger("logging roottree1\n");
	roottree1->log2();
	logger("logging roottree2\n");
	roottree2->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree1;
	delete roottree2;
	delete hobj;
	delete adoar;
}

void ar_exit()
{
	vcamstopstream();
	vcamexit();
	delete rl;
	exit_res3d();
	popdir();
	freebms();
	delete galli;
	ar_exit3d();
}
