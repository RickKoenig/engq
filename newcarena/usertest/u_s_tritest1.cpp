/// test shadow maps
#define D2_3D
#include <m_eng.h>
#include "system/u_states.h"

#include <d3d9.h>
#include "m_vid_dx9.h"
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "utils/modelutil.h"
#include "d2_font.h"
#include "u_s_toon.h"

#define USESHADOWMAP

static U32 togvidmode;
static tree2* roottree;
static tree2* backgndtree;
static tree2* skyboxtree,*envtree,*normalmaptree;

//static S32 uselightssave;
static modelb* dumm;
static C8 shadename[50]="pnormalmap"; // "useshadowmap";

static char *cubetexlist[]={
	"cubemap_mountains.jpg",
	"cube2.jpg",
	"cubicmap.jpg",
	"cube.jpg",
	"footballfield",
	"fishpond",
	"skansen",
	"skansen4",
};
static S32 curcube;
#define NCUBETEXLIST ((S32)(sizeof(cubetexlist)/sizeof(cubetexlist[0])))

static tree2* buildnormalmapmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("stm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","243-normal.jpg",matname);
	    buildpatch_tan(modb,24,12,18,9,spheref_surf_tan(5.0f),"stones.jpg","normal.jpg",matname);
//	    buildpatch_tan(modb,24,12,18,9,spheref_surf_tan(5.0f),"stones.jpg","243-normal.jpg",matname);
//	    buildpatch_tan(modb,24,12,18,9,spheref_surf_tan(5.0f),"stones.jpg","ts18_2crop.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptestnck.tga","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
//	r->treecolor=pointf3x(1,1,0);
//	r->flags|=TF_TREECOLOR;
	r->flags|=TF_CASTSHADOW;
	return r;
}

static tree2* buildenvironmentmapmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("env");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("skybox");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","243-normal.jpg",matname);
	    string cub="CUB_";
		cub+=cubetexlist[curcube];
		buildpatch(modb,24,12,6,3,spheref_surf(2.0f),cub.c_str(),matname);
		popdir();
	}
	r->setmodel(modb);
//	r->treecolor=pointf3x(1,1,0);
//	r->flags|=TF_TREECOLOR;
	r->flags|=TF_CASTSHADOW;
	return r;
}

static tree2* buildbackgroundmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("sbm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
//		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
	    buildpatch(modb,1,1,3,3,planexz_surf(22.0f,22.0f),"rengst.jpg",matname);
//	    buildpatch(modb,10,10,3,3,planexz_surf(20.0f,20.0f),"alphacir.dds",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
//		popdir();
	}
	r->setmodel(modb);
	return r;
}

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
	r->flags|=TF_CASTSHADOW;
	return r;
}

/// sort test
/*
struct as {
	int i;
	float f;
};


bool lessthani(as a,as b)
{
	return a.i<b.i;
}

bool lessthanf(as a,as b)
{
	return a.f<b.f;
}

static void sorttest()
{
	as td[10]={
		{47,3.14f},
		{32,2.72f},
		{16,1.41f},
		{24,5.23f},
		{12,6.14f},
		{94,2.14f},
		{63,5.14f},
		{37,9.14f},
		{27,4.54f},
		{17,2.54f},
	};
	logger("before sort\n");
	S32 i;
	for (i=0;i<10;++i)
		logger("'%2d' , '%4.2f'\n",td[i].i,td[i].f);
	sort(td,td+10,lessthani);
	logger("after int sort\n");
	for (i=0;i<10;++i)
		logger("'%2d' , '%4.2f'\n",td[i].i,td[i].f);
	sort(td,td+10,lessthanf);
	logger("after float sort\n");
	for (i=0;i<10;++i)
		logger("'%2d' , '%4.2f'\n",td[i].i,td[i].f);
}
*/
////////////////////////// main
void tritest1init()
{
//	sorttest();
	userfloata=.995f;
// fog test
/*	videoinfodx9.fog_enable=true;
	videoinfodx9.fog_mode=2;
	videoinfodx9.fog_color=pointf3x(.5f,.5f,.5f);
	videoinfodx9.fog_start=1.0f;
	videoinfodx9.fog_end=100.0f;
	videoinfodx9.fog_density=.03225f; */
// end fog test
	video_setupwindow(GX,GY);
//	uselightssave=lightinfo.uselights;
	lightinfo.uselights=1;
	roottree=new tree2("roottree");
// add a dummy model to make a smaller logfile sometimes
	dumm=model_create("dumm");
	pushandsetdir("gfxtest");
	textureb* texmat=texture_create("bark.tga");
	if (texture_getrc(texmat)==1) {
		texmat->load();
	}
	dumm->addmat("tex",SMAT_HASTEX,texmat,&F32RED,30,0,0);
	dumm->close();
	tree2* tt;
// build a environmentmap model
	tt=buildenvironmentmapmodel("envmodel","env");
//	tt=new tree2("built");
//	tt->setmodel(mod);
	tt->trans=pointf3x(-3.071f,-8.071f,7.071f*2);
	tt->rotvel.x=.003f;
	tt->rotvel.y=.0035f;
	roottree->linkchild(tt);
	envtree=tt;
// build a background
	tt=buildbackgroundmodel("backgnd","useshadowmap");
//	tt=new tree2("built");
//	tt->setmodel(mod);
	tt->trans.z=17.5;
	tt->trans.y=-3;
	tt->rot.x=3*PI/2.0f;
	tt->rotvel.x=(2*PI/32)/wininfo.fpswanted;
	roottree->linkchild(tt);
	backgndtree=tt;	// grab it
// build a cube
	tt=buildprism2t(pointf3x(1,2,3),"rengst.jpg","maptest.tga","pdiffspec");
//	tt=new tree2("built");
//	tt->setmodel(mod);
	tt->trans=pointf3x(7.071f,7.071f,7.071f*2);
	tt->rotvel.x=.005f/2;
	tt->rotvel.y=.05f/2;
	tt->flags|=TF_CASTSHADOW;
	roottree->linkchild(tt);
// build a cylinder to represent light at start position
	tt=buildcylinder_xz(pointf3x(.25f,.5f,.25f),"nada","tex");
//	tt=new tree2("built");
//	tt->setmodel(mod);
//	tt->trans=pointf3x(10.071f,7.071f,7.071f*2);
	tt->rot.x=PI/2;
// hack to get yellow
	list<tree2*>::iterator it=tt->children.begin();
	(*it++)->mod->mats[0].color=F32GREEN;
	(*it++)->mod->mats[0].color=F32RED;
	(*it++)->mod->mats[0].color=F32YELLOW;
//	tt->rotvel.y=.05f/2;
	roottree->linkchild(tt);
// build a shadowmapviewer
	tt=buildshadowmapviewermodel("shadowmapviewer","shadowmapviewer");
//	tt=new tree2("built");
//	tt->setmodel(mod);
	tt->trans=pointf3x(7.071f,-7.071f,15);
//	tt->rotvel.x=.005f/2;
//	tt->rotvel.y=.05f/2;
	tt->rot.x=-PI/2.0f;
	roottree->linkchild(tt);
// build a skybox
	pushandsetdir("skybox");
	tt=buildskybox(pointf3x(10,10,10),cubetexlist[curcube],"tex");
	popdir();
//	tt=buildskybox(pointf3x(100,100,100),"cubicmap.jpg","tex");
	roottree->linkchild(tt);
	skyboxtree=tt;
//	skyboxtree->mod->mats[0].msflags|=SMAT_HASNOFOG; 
/*	bitmap32* b=tt->mod->mats[0].thetexarr[0]->locktexture();
	clipputpixel32(b,0,349,C32GREEN);
	tt->mod->mats[0].thetexarr[0]->unlocktexture(); */
// build a normalmap model
	tt=buildnormalmapmodel("shadermodel",shadename);
//	tt=new tree2("built");
//	tt->setmodel(mod);
	tt->trans=pointf3x(-8.071f,8.071f,7.071f*2);
	tt->rotvel.x=.003f;
	tt->rotvel.y=.0035f;
	roottree->linkchild(tt);
	normalmaptree=tt;
	if (video3dinfo.cantoon) {
		tree2* n=tt->newdup();
		normalify(n);
		roottree->linkchild(n);
		tree2* t=tt->newdup();
		normalify(t,1);
		roottree->linkchild(t);
		tree2* bn=tt->newdup();
		normalify(bn,2);
		roottree->linkchild(bn);
	}
// add a directional light
	tt=new tree2("adirlight");
//	lightinfo.dodefaultlights=0;
	tt->flags|=TF_ISLIGHT;
//	tt->rotvel.y=.05f;
	tt->lightcolor=pointf3x(.75f,.75f,.75f);
	addlighttolist(tt);
	roottree->linkchild(tt);
// add an amblight
	tt=new tree2("aamblight");
//	lightinfo.dodefaultlights=0;
	tt->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	tt->lightcolor=pointf3x(.25f,.25f,.25f);
	addlighttolist(tt);
	roottree->linkchild(tt);
// setup main viewport
	mainvp.backcolor=C32LIGHTGREEN;
	mainvp.zfront=.25f;
	mainvp.zback=400;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
//	mainvp.isortho=false;
	if (!mainvp.ortho_size)
		mainvp.ortho_size=5;
// setup lightviewport
	lightvp.backcolor=C32WHITE;
	lightvp.zfront=4.25f;
	lightvp.zback=400;
	lightvp.camzoom=.875f;//3.2f; // it'll getit from tree camattach if you have one
	lightvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
	lightvp.xsrc=1;
	lightvp.ysrc=1;
	lightvp.useattachcam=false;
//	lightvp.lookat=0;
	lightvp.isortho=false;
	lightvp.ortho_size=5;
	lightvp.xstart=0;
	lightvp.ystart=0;
	lightvp.xres=SHADOWMAP_SIZE;
	lightvp.yres=SHADOWMAP_SIZE;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
//	extradebvars(evars,nevars);
}

void tritest1proc()
{
/*	delete tt;
	tt=new tree2("built");
	modelb* mod=buildshadermodel("shadermodel","tex");
	tt->setmodel(mod); 
	roottree->linkchild(tt); */
// do something
	if (backgndtree->rot.x>3*PI/2+PI/2*.4f)
		backgndtree->rotvel.x=-(2*PI/32)/wininfo.fpswanted;
	if (backgndtree->rot.x<3*PI/2-PI/2*.4f)
		backgndtree->rotvel.x=(2*PI/32)/wininfo.fpswanted; 
//	logger("rvx= %f\n",backgndtree->rot.x);
	normalmaptree->treecolor.w=userfloata;
	const C8* nn;
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
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
		changestate(STATE_TRITEST1);
		break;
	case '=':
		changeglobalxyres(1);
//			video_setupwindow(GX,GY);
		changestate(STATE_TRITEST1);
		break;
	case '-':
		changeglobalxyres(-1);
//			video_setupwindow(GX,GY);
		changestate(STATE_TRITEST1);
		break;
	case 'n':
		nn=model3::getnextshadername(shadename);
		if (nn)
			strcpy(shadename,nn);
//			video_setupwindow(GX,GY);
		changestate(STATE_TRITEST1);
		break;
	case 't': // change cube map and sky map
		++curcube;
		if (curcube>=NCUBETEXLIST)
			curcube=0;
		delete(skyboxtree);
		pushandsetdir("skybox");
		skyboxtree=buildskybox(pointf3x(10,10,10),cubetexlist[curcube],"tex");
		popdir();
//	tt=buildskybox(pointf3x(100,100,100),"cubicmap.jpg","tex");
//		skyboxtree->mod->mats[0].msflags|=SMAT_HASNOFOG;
		roottree->linkchild(skyboxtree);
		delete(envtree);
		envtree=buildenvironmentmapmodel("envmodel","env");
//	tt=new tree2("built");
//	tt->setmodel(mod);
		envtree->trans=pointf3x(-3.071f,-8.071f,7.071f*2);
		envtree->rotvel.x=.003f;
		envtree->rotvel.y=.0035f;
		roottree->linkchild(envtree);
/*		if (skyboxtree && skyboxtree->mod) {
			mater2* m=&skyboxtree->mod->mats[0];
			textureb* &t=m->thetexarr[0];
			if (t)
				textureb::rc.deleterc(t);
			t=texture_create(cubetexlist[curcube]);
			if (texture_getrc(t)==1)
				t->load();
			m=&envtree->mod->mats[0];
			textureb* &t2=m->thetexarr[0];
			if (t2)
				textureb::rc.deleterc(t2);
			string cub="CUB_";
			cub+=cubetexlist[curcube];
			t2=texture_create(cub.c_str());
			if (texture_getrc(t2)==1)
				t2->load();
		} */
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST1);
	}
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.xres=WX;
	mainvp.yres=WY;
	roottree->proc();
	doflycam(&mainvp);

	static S32 frm;
	drawtextque_format(SMALLFONT,10,10,F32RED,"frame %d",frm);
	drawtextque_format_foreback(SMALLFONT,10,50,pointf3x(0.0f,1.0f,0.0f,.25f),pointf3x(1.0f,0.0f,0.0f,.25f),"fps avg2 %f",wininfo.fpsavg2);
	++frm;
}

void tritest1draw3d()
{
	video_buildworldmats(roottree); // help dolights
	dolights();
#if 1
#ifdef USESHADOWMAP
//#ifdef SHADOWMAP
	if (video_rendertargetshadowmap()) {
//		lightvp.camtrans=lightinfo.worldlightpos[0];
//		lightvp.camrot=lightinfo.worldlightdirs[0];
		lightvp.usev2w=true;
		lightvp.v2w=lightinfo.light2world[0];
//		video_beginscene(); // clear zbuf etc.
//	lightvp.xstart=0;
//	lightvp.ystart=0;
//	lightvp.xres=WX;
//	lightvp.yres=WY;
		video_setviewport(&lightvp); // clear zbuf etc.
//		logger("rendertarget set to shadowmap\n");
		video_drawscene(roottree);
//		video_endscene(0); // nothing right now
		video_rendertargetmain();
	}
//#endif
#endif
#endif
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
//	logger("rendertarget set to main\n");
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
}

void tritest1exit()
{
//	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
//	lightinfo.uselights=uselightssave;
	modelb::rc.deleterc(dumm);
	popdir();
}
