/// test shadow maps
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

//#include <d3d9.h>
//#include "m_vid_dx9.h"
//#include "d2_dx9.h"
#include "u_modelutil.h"
#include "d2_font.h"
#include "m_sagger.h"

#define USESHADOWMAP
#define VIEWSHADOWMAP

static U32 togvidmode;
static tree2* roottree;
static tree2* spawntree,*explodetree;
//#define MINIMAL

//static string cubname = "cube.jpg";
//static string cubname = "cubemap_mountains.jpg";
static string cubname = "Footballfield";
//static string cubname = "cube.jpg";
//static string cubname = "cube.jpg";


static string cubcubname = string("CUB_") + cubname;

static backsndplayer* abacksndplayer;
static waveslotplayer* awaveslotplayer;

static tree2* buildenvironmentmapmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("env");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("skybox");
		buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(1.0f),cubcubname.c_str(),"normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
	return r;
}

static tree2* buildgroundmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("ground");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
	    buildpatch(modb,20,20,20,20,planexz_surf(200,200),"rengst.jpg",matname);
	}
	r->setmodel(modb);
	return r;
}

/// build the spheretree model if this is the first one
static tree2* buildsphere(const C8* modelname,const C8* matname)
{
	modelb* mod2=model_create("sphere3");
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,35,35,3,3,spheref_surf(.25f),"bark.tga","tex");
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree3");
	spheretree2->setmodel(mod2);
	return spheretree2;
}

#ifdef VIEWSHADOWMAP
static tree2* buildshadowmapviewermodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("sbm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
	    buildpatch(modb,1,1,1,1,planexz_surf(10.0f,10.0f),"shadowmap",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
	return r;
}
#endif

/*
class test {
	void (*userprocfun)(test* t);
public:
	string name;
	test(string namea) : name(namea) {}
	void proc()
	{
		logger("intree proc\n");
		if (userprocfun)
			(*userprocfun)(this);
	}
	void setuserprocfun(void (*userprocfuna)(test* t))
	{
		userprocfun=userprocfuna;
	}
};

void afun(test* t)
{
	logger("in afun '%s'\n",t->name.c_str());
}
*/

static bool carcam;
static tree2* cartree;

// tree userprocs

static bool cardrive(tree2* t)
{
	S32 leftright=0,foreback=0;
	if (wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT])
		++leftright;
	if (wininfo.keystate[K_LEFT]||wininfo.keystate[K_NUMLEFT])
		--leftright;
	if (wininfo.keystate[K_UP]||wininfo.keystate[K_NUMUP])
		++foreback;
	if (wininfo.keystate[K_DOWN]||wininfo.keystate[K_NUMDOWN])
		--foreback;
	if (foreback>=0)
		cartree->rot.y+=.025f*leftright;
	else
		cartree->rot.y-=.025f*leftright;
	cartree->trans.x+=foreback*.2f*sinf(cartree->rot.y);
	cartree->trans.z+=foreback*.2f*cosf(cartree->rot.y);
	return true;
}

// move and explode
static bool explode(tree2* t)
{
	++t->userint[0];
	if (t->userint[0]>=10) {
//		delete t;
		return false;
	}
	t->treecolor.w=1.0f-(float)t->userint[0]/10.0f;
	t->scale=pointf3x(t->userint[0]*2.5f,t->userint[0]*2.5f,t->userint[0]*2.5f);
	return true;
}

static bool flyers(tree2* t)
{
	if (t->userint[0]==0)
		t->userint[1]=4+mt_random(200);
	++t->userint[0];
	if (t->userint[0]*3>=t->userint[1] && !t->userint[2]) {
#if 1
		static const float explodevolume = .2f;
		awaveslotplayer->settrack(mt_random(awaveslotplayer->getnumtracks()),explodevolume);
		tree2* tt=explodetree->newdup();
		roottree->linkchild(tt);
		tt->trans=t->trans;
		t->userint[2]=1;
#endif
//		delete t;
		return false;
	}
	t->treecolor.w=1.0f-(float)t->userint[0]/(float)t->userint[1];
	return true;
}

// just move back and forth, spawn flyers
static bool backforth(tree2* t)
{
	bool spawn=false;
	if (t->userfloat[0]==0)
		t->userfloat[0]=1;
	if (t->userfloat[0]==1) {
		if (t->trans.x>5) {
			t->userfloat[0]=-1;
			spawn=true;
		}
	} else {
		if (t->trans.x<-5) {
			t->userfloat[0]=1;
			spawn=true;
		}
	}
	t->transvel.x=.1f*t->userfloat[0];
//	spawn=false;
	if (spawn) {
		awaveslotplayer->settrack(mt_random(awaveslotplayer->getnumtracks()),.25f);
		S32 i;
		for (i=0;i<12;++i) {
			tree2* tt=spawntree->newdup();
			roottree->linkchild(tt);
			tt->trans=t->trans;
			tt->transvel.x=.4f*(mt_frand()*2-1);
			tt->transvel.y=.4f*(mt_frand());
			tt->transvel.z=.4f*(mt_frand()*2-1);
			tt->rotvel.x=(PI/40)*(mt_frand()*2-1);
			tt->rotvel.y=(PI/40)*(mt_frand()*2-1);
			tt->rotvel.z=(PI/40)*(mt_frand()*2-1);
		}
	}
	return true;
}

////////////////////////// main
void arena1init()
{
	pushandsetdir("audio/backsnd");
	scriptdir* sc=new scriptdir(0);
	sc->sort();
	abacksndplayer=new backsndplayer(*sc);
	delete sc;
	popdir();

	pushandsetdir("racenetdata/weapsounds_deploy");
//	pushandsetdir("audio/weapsounds_deploy_vag");
//	pushandsetdir("audio/weapsounds_activate_vag");
//	pushandsetdir("audio/waves");
	sc=new scriptdir(0);
	sc->sort();
	awaveslotplayer=new waveslotplayer(*sc);
	delete sc;
	popdir();
/*	test* a = new test("hi");
	a->setuserprocfun(afun);
	a->proc();
	delete a; */
// setup video
	video_setupwindow(GX,GY);

// setup root
	roottree=new tree2("roottree");
	tree2* tt;
	pushandsetdir("gfxtest");

#ifndef MINIMAL
// build a environmentmap model
	tt=buildenvironmentmapmodel("envmodel","env");
	tt->trans=pointf3x(0,1,10);
	tt->setuserproc(backforth);
	tt->flags|=TF_CASTSHADOW;
	roottree->linkchild(tt);
#endif


// build a ground
#ifdef USESHADOWMAP
	tt=buildgroundmodel("backgnd","useshadowmap");
#else
	tt=buildgroundmodel("backgnd","pdiffspec");
#endif
	roottree->linkchild(tt);
//	tt->rotvel.x=.01f;
//	tt->rotvel.y=.01f;
//	tt->rotvel.z=.01f;

#ifndef MINIMAL
// build a spawntree
	pushandsetdir("gfxtest");
	spawntree=buildprism(pointf3x(1,1,1),"maptestnck.tga","pdiffspec");
	popdir();
	spawntree->flags|=TF_CASTSHADOW;
	spawntree->setuserproc(flyers);
	explodetree=buildsphere("sphere3","pdiffspec");
	explodetree->treecolor=F32YELLOW;
	explodetree->setuserproc(explode);

// build a skybox
	pushandsetdir("skybox");
	tt=buildskybox(pointf3x(10,10,10),cubname.c_str(),"tex");
	popdir();
	roottree->linkchild(tt);
//	tt->mod->mats[0].msflags|=SMAT_HASNOFOG;

// build a car
	pushandsetdir("skybox");
	tt=buildprism(pointf3x(5,3,10),cubcubname.c_str(),"env");
	popdir();
	roottree->linkchild(tt);
//	tt->mod->mats[0].msflags|=SMAT_HASNOFOG;
	tt->flags|=TF_CASTSHADOW;
	tt->setuserproc(cardrive);
	cartree=tt;


// build a cylinder
	pushandsetdir("gfxtest");
	tt=buildcylinder_xz(pointf3x(1,3,1),"bark.tga","pdiffspec");
	tt->trans=pointf3x(5,0,10);
	setcastshadow(tt);	// will set TF_CASTSHADOW for hierarchy
	roottree->linkchild(tt);
	tt=tt->newdup();
	tt->trans.x=-5;
	roottree->linkchild(tt);
	popdir();

#ifdef VIEWSHADOWMAP
// build shadowmap viewer
	tt=buildshadowmapviewermodel("smv","shadowmapviewer");
	tt->trans.y=6;
	tt->trans.x=10;
	tt->trans.z=30;
	tt->rot.x=-PI/2;
	roottree->linkchild(tt);
#endif

// add a directional light
	tt=new tree2("adirlight");
	tt->flags|=TF_ISLIGHT;
	tt->rot.x=PI/4;
	tt->rotvel.y=PI/800;
	tt->lightcolor=pointf3x(.75f,.75f,.75f);
	addlighttolist(tt);
	roottree->linkchild(tt);

// add an amblight
	tt=new tree2("aamblight");
	tt->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	tt->lightcolor=pointf3x(.25f,.25f,.25f);
	addlighttolist(tt);
	roottree->linkchild(tt);
#endif
// setup main viewport
	mainvp.backcolor=C32LIGHTGREEN;
	mainvp.zfront=.125f;
	mainvp.zback=400;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
//	mainvp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	mainvp.xsrc=WX;
	mainvp.ysrc=WY;
	mainvp.useattachcam=true;
//	mainvp.lookat=0;
//	mainvp.camtrans.y=2.5;
	mainvp.camtrans=pointf3x(0,5,-20);
	mainvp.usev2w=false;
	mainvp.camattach=cartree;

	mainvp.camrot.x=0;//1e-20f;
#ifdef USESHADOWMAP
// setup lightviewport
	lightvp.backcolor=C32WHITE;
	lightvp.zfront=-200;
	lightvp.zback=200;
//	lightvp.camzoom=.875f;//3.2f; // it'll getit from tree camattach if you have one
	lightvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
	lightvp.xsrc=1;
	lightvp.ysrc=1;
	lightvp.useattachcam=false;
//	lightvp.lookat=0;
	lightvp.isortho=true;
	lightvp.ortho_size=25;
	lightvp.xstart=0;
	lightvp.ystart=0;
	lightvp.xres=SHADOWMAP_SIZE;
	lightvp.yres=SHADOWMAP_SIZE;
#endif
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	abacksndplayer->settrack(5);
	carcam=true;
	lightinfo.dodefaultlights=false;
}

void arena1proc()
{
//	logger("arena1proc\n");
	switch(KEY) {
// housekeeping
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 'b':
		{
			S32 t=abacksndplayer->gettrack();
			if (t<0)
				abacksndplayer->settrack(4);
			else
				abacksndplayer->settrack(t+1);
		}
		break;
	case 'l':
		carcam^=1;
		mainvp.useattachcam=(carcam!=0);
		break;
	case 'a':
		showcursor(1);
		break;
	case 'h':
		showcursor(0);
		break;
	case ' ':
		video3dinfo.favorshading^=1;
		break;
	case 's':
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA1);
		break;
	case '=':
		changeglobalxyres(1);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA1);
		break;
	case '-':
		changeglobalxyres(-1);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA1);
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_ARENA1);
	}
// update viewport
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.xres=WX;
	mainvp.yres=WY;
	doflycam(&mainvp);
	roottree->proc();

// draw some 3d/2d text
#ifndef MINIMAL
	static S32 frm;
	drawtextque_format(SMALLFONT,200,10,F32RED,"frame %d",frm);
	drawtextque_format_foreback(SMALLFONT,200,42,pointf3x(1.0f,1.0f,1.0f,.5f),pointf3x(0.0f,0.0f,0.0f,.5f),"fps avg  %f",wininfo.fpsavg);
	drawtextque_format_foreback(SMALLFONT,200,50,pointf3x(1.0f,1.0f,1.0f,.5f),pointf3x(0.0f,0.0f,0.0f,.5f),"fps avg2 %f",wininfo.fpsavg2);
	drawtextque_format_foreback(SMALLFONT,200,70,F32GREEN,F32BLACK,"current backgnd track %d",abacksndplayer->gettrack());
	++frm;
#endif
}

void arena1draw3d()
{
	video_buildworldmats(roottree); // help dolights
/*	if (carcam) {
		mat4 vo;
		identmat4(&vo);
		vo.e[3][0]=0; vo.e[3][1]=5; vo.e[3][2]=-20;
		mulmat3d(&vo,&cartree->o2w,&mainvp.v2w);
		mainvp.usev2w=true;
	} else {
		mainvp.usev2w=false;
	} */
	dolights();
// draw 3d
//#ifdef SHADOWMAP // from engine
//#ifdef USESHADOWMAP // from user
	if (video_rendertargetshadowmap()) {
		lightvp.usev2w=true;
		lightvp.v2w=lightinfo.light2world[0];
		video_setviewport(&lightvp); // clear zbuf etc.
		video_drawscene(roottree);
		video_rendertargetmain();
	}
//#endif
//#endif
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
}

void arena1exit()
{
// log and free
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
#ifndef MINIMAL
	delete spawntree;
	delete explodetree;
#endif
	popdir();
	delete abacksndplayer;
	delete awaveslotplayer;
	mainvp.usev2w=false;
	mainvp.useattachcam=false;
}
