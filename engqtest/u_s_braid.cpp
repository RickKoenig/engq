/// braid builder
/// this will be the parametric surface playground
#define D2_3D
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

namespace braid {

U32 togvidmode; // 0,1,2,3 gdi,ddraw,dx9win,dx9fs
S32 uselightssave;
tree2* roottree;

/// strand test func 1
/// a simple sine wave plotted along x,y axis
pointf3 sinxy_strand(float t)
{
	pointf3 ret;
	t*=PI;
	ret.x=10*t;
	ret.y=30*sinf(t);
	ret.z=0;
	return ret;
}

/// strand test functor 2
/// same as sinxy_strand except as a functor
class sinxyf_strand {
    float freq,width,height;
public:
    sinxyf_strand(float freqa,float widtha,float heighta) : freq(freqa),width(widtha),height(heighta) {}
    pointf3 operator()(float t)
    {
		pointf3 ret;
		ret.x=width*t;
		t*=freq;
		ret.y=height*sinf(t);
		ret.z=0;
		return ret;
	}
};

/// braid functor 3
/// a sine wave starting to the right, but moving in a big circle
class sincircle_strand {
    float osc,rad,ht,perdo;
public:
    sincircle_strand(float osca,float rada,float hta,float perdoa) :
      osc(osca),rad(rada),ht(hta),perdo(perdoa) {}
    pointf3 operator()(float t)
    {
		pointf3 ret;
		t*=2*PI*perdo;
		ret.x=rad*cosf(t);
		ret.z=rad*sinf(t);
		ret.y=ht*sinf(t*osc);
		return ret;
	}
};
/// braid functor 4
/// a braid piece going up
class braid_strand {
    const pointf3 freq,size,phase;
public:
    braid_strand(const pointf3& freqa,const pointf3& sizea,const pointf3& phasea) :
     freq(freqa),size(sizea),phase(phasea) {}
    pointf3 operator()(float t)
    {
    	pointf3 ret;
    	ret.y=size.y*t;
    	t*=2*PI;
    	ret.x=size.x*sinf(freq.x*t+phase.x);
    	ret.z=size.z*sinf(freq.z*t+phase.z);
    	return ret;
	}
};

tree2* buildbraid()
{
	const S32 pres=80;
	const S32 pcapres=2;
	const S32 qres=12;
	const float brad2=.05125f;
	const S32 nbraid=3;
	const C8* const shadlist[nbraid]={"pdiff","vdiffspec","pdiffspec"};
	tree2* ret=new tree2("braidtot");
	S32 i;
	pushandsetdir("gfxtest");
	for (i=0;i<nbraid;++i) {
//		if (i!=0)
//			continue;
		char str[50];
		pointf3 freq={1.0f,0,2.0f};
		pointf3 size={.5f,2,.155f};
		pointf3 phase={0,0,0};
		phase.x=2*PI*i/nbraid;
		phase.z=4*PI*i/nbraid;
		braid_strand bfs(freq,size,phase);
		tree2* sbrdtree=buildstrand(bfs,pres,qres,pcapres,brad2,shadlist[i]);
		sprintf(str,"braid_%d",i);
		sbrdtree->name=str;
		ret->linkchild(sbrdtree);
	}
	popdir();
	return ret;
}

} /// end namespace
using namespace braid;

////////////////////////// main
void braidinit()
{
//	tester hi;
//	hi.builder(2);
//    testrotinit();
	video_setupwindow(GX,GY);
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=1;
	togvidmode=videoinfo.video_maindriver;
/// setup trees
	roottree=new tree2("roottree");
	tree2* sbrdtree;
/// an amblight
#if 1
	tree2* amblit=new tree2("amblit");
	amblit->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	amblit->lightcolor=F32DARKGRAY;
	addlighttolist(amblit);
	roottree->linkchild(amblit);

/// a dirlight
	tree2* dirlit;
	dirlit=new tree2("dirlit");
	dirlit->name="dirlit";
	dirlit->flags|=TF_ISLIGHT;
	dirlit->lightcolor=F32GREEN;
//	dirlit->rotvel.x=0;
//	dirlit->rotvel.y=.01f;
//	dirlit->rotvel.z=0;
	addlighttolist(dirlit);
	roottree->linkchild(dirlit);

/// another dirlight
	dirlit=new tree2("dirlit2");
	dirlit->name="dirlit2";
	dirlit->rot.y=PI;
	dirlit->flags|=TF_ISLIGHT;
	dirlit->lightcolor=F32LIGHTRED;
//	dirlit->rot.y=PI/2;
	addlighttolist(dirlit);
	roottree->linkchild(dirlit);

/// build the spheretree model if this is the first one
	modelb* mod=model_create("sphere1");
	if (model_getrc(mod)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xz,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,torus1xz,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
//	    buildpatch(mod,40,40,20,20,sf,"rengst.jpg");
	    buildpatch(mod,40,40,20,20,spheref_surf(5.0f),"rengst.jpg","tex");
	    popdir();
	}
	const float rotvy=0;
	const float rotvx=0;
//	const float rotvy=.015f;
//	const float rotvx=.002f;
/// build a spheretree, pass model ownership to tree and hook to root
	tree2* spheretree=new tree2("spheretree");
	spheretree->setmodel(mod);
	spheretree->rotvel.y=rotvy;
	spheretree->rotvel.x=rotvx;
	roottree->linkchild(spheretree);

/// build the spheretree model if this is the first one
	modelb* mod2=model_create("sphere2");
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,35,35,20,20,spheref_surf(2.0f),"maptest.tga","tex");
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree2");
	spheretree2->setmodel(mod2);
	spheretree2->rotvel.y=rotvy;
	spheretree2->rotvel.x=rotvx;
    spheretree2->trans.x=5;
	roottree->linkchild(spheretree2);

/// build the cylinder root
	tree2* cyltree=new tree2("cyltree");
    cyltree->trans.x=10;
	cyltree->rotvel.y=rotvy;
	cyltree->rotvel.x=rotvx;
/// build the cylinder body
	modelb* modcm=model_create("cylm");
	if (model_getrc(modcm)==1) {
        pushandsetdir("gfxtest");
		buildpatch(modcm,35,35,20,20,cylinderxz_mid_surf(5.0f,2.0f),"rengst.jpg","tex");
	    popdir();
	}
	tree2* cyltreem=new tree2("cyltreem");
	cyltreem->setmodel(modcm);
 	cyltree->linkchild(cyltreem);
/// build cyl cap0
	modelb* modcc0=model_create("cylbot");
	if (model_getrc(modcc0)==1) {
        pushandsetdir("gfxtest");
		buildpatch(modcc0,35, 3,20, 2,cylinderxz_bot_surf(5.0f,2.0f),"rengst.jpg","tex");
	    popdir();
	}
	tree2* cyltreec0=new tree2("cyltreec0");
	cyltreec0->setmodel(modcc0);
	cyltree->linkchild(cyltreec0);
/// build cyl cap1
	modelb* modcc1=model_create("cyltop");
	if (model_getrc(modcc1)==1) {
        pushandsetdir("gfxtest");
		buildpatch(modcc1,35, 3,20, 2,cylinderxz_top_surf(5.0f,2.0f),"rengst.jpg","tex");
	    popdir();
	}
	tree2* cyltreec1=new tree2("cyltreec1");
	cyltreec1->setmodel(modcc1);
	cyltree->linkchild(cyltreec1);
	roottree->linkchild(cyltree);

/// build the braid test1
	const S32 pres=400;
	const S32 pcapres=2;
	const S32 qres=12;
//	const float bheight=100.0f;
//	float brad=2.0f;
//	const float osc=PI;
//	const float ht=30;
//	const float wd=20;
	const float osc2=20;
	const float rad2=12;
	const float ht2=1.5f;
	const float perdo2=.99f;
	const float brad2=.125f;
	sincircle_strand scs=sincircle_strand(osc2,rad2,ht2,perdo2);
	pushandsetdir("gfxtest");
	sbrdtree=buildstrand(scs,pres,qres,pcapres,brad2,"tex");
	sbrdtree->name="braidt1";
    sbrdtree->trans.x=40;
	sbrdtree->rotvel.y=rotvy;
	sbrdtree->rotvel.x=rotvx;
	roottree->linkchild(sbrdtree);

/// build another braid test2
	sbrdtree=buildstrand(&sinxy_strand,pres,qres,pcapres,brad2,"tex");
//	sbrdtree=buildstrand(bf3,pres,qres,pcapres,brad2);
	sbrdtree->name="braidt2";
    sbrdtree->trans.x=41;
	sbrdtree->rotvel.y=rotvy;
	sbrdtree->rotvel.x=rotvx;
	roottree->linkchild(sbrdtree);

/// build another braid test3
	sinxyf_strand sxys=sinxyf_strand(5*2*PI,30,5);
	sbrdtree=buildstrand(sxys,pres,qres,pcapres,brad2,"tex");
//	sbrdtree=buildstrand(bf3,pres,qres,pcapres,brad2);
	sbrdtree->name="braidt3";
    sbrdtree->trans.x=42;
	sbrdtree->rotvel.y=rotvy;
	sbrdtree->rotvel.x=rotvx;
	roottree->linkchild(sbrdtree);
#endif
/// build a complete braid
	sbrdtree=buildbraid();
//	sbrdtree=buildstrand(bf3,pres,qres,pcapres,brad2);
	sbrdtree->name="braidall";
    sbrdtree->trans.x=50;
    sbrdtree->trans.z=-14;
//	sbrdtree->rotvel.y=0;
//	sbrdtree->rotvel.x=0;
	roottree->linkchild(sbrdtree);
#if 1
/// build the torus
	modelb* modd=model_create("torus2");
	if (model_getrc(modd)==1) {
        pushandsetdir("gfxtest");
		buildpatch(modd,35,35,20,20,torusxzf_surf(5.0f,3.0f),"maptest.tga","pdiff");
	    popdir();
	}
	tree2* tor2tree=new tree2("tor2tree");
	tor2tree->setmodel(modd);
    tor2tree->trans.x=20;
	tor2tree->rotvel.y=rotvy;
	tor2tree->rotvel.x=rotvx;
	roottree->linkchild(tor2tree);
#endif
/// dup that tree, slightly modify it and hook to root
//   spheretree=spheretree->newdup(); // a copy of spheretree
//	spheretree->rotvel.y=.007f;
//	spheretree->rotvel.x=.004f;
//	roottree->linkchild(spheretree);

/// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.01f;
	mainvp.zback=100;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z=-15;
	mainvp.camtrans.x=50;
	mainvp.camtrans.y=.5f;
	mainvp.flags=VP_CLEARBG|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
/*	mainvp.zfront=.01f;
	mainvp.zback=272.0f;
	mainvp.camtrans.x=49.82542f;
	mainvp.camtrans.y=1.032948f;
	mainvp.camtrans.z=-14.0519f;
	mainvp.camrot.x=.399244f;
	mainvp.camrot.y=.235619f;
	mainvp.camrot.z=0.0f; */
	popdir();
}

void braidproc()
{
//	testrotproc();
/// input
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
	case 's':
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_BRAID);
		break;
	case '=':
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_BRAID);
		break;
	case '-':
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_BRAID);
		break;
	case ' ':
		video3dinfo.favorshading^=1;
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_BRAID);
	}
/// incase window changed size
	mainvp.xres=WX;
	mainvp.yres=WY;
/// proc
	roottree->proc();
	doflycam(&mainvp);
}

void braiddraw3d()
{
/// draw
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
//	popstate();
}

void braidexit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree; // free the main man
	lightinfo.uselights=uselightssave;
}

