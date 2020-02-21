/// test draw particles (new particles)
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

//#ifndef SFML
//#include <d3d9.h>
//#include "d2_dx9.h" // for seetmodel3version // temporary
//#endif

#include "u_modelutil.h"

static U32 togvidmode;
static tree2* roottree;

static uv parttriuvs[3]={
	{0,0},
	{1,0},
	{.5f,1},
};

static pointf3 parttrixyz[3]={
	{0,.5f},
	{1,.5f},
	{.5f,-.5f},
};

static pointf3 parttricvs[3]={
	{1,0,0,.91f},
	{0,1,0,.91f},
	{0,0,1,.91f},
};

#define NPARTS 1000

static void makeverts(tree2* t)
{
	modelb* m=t->mod;
	S32 i,j;
	//mt_setseed(69);
	pointf3*  p=new pointf3[NPARTS*3];
	pointf3* cv=new pointf3[NPARTS*3];
	mat4 v2o;
	mulmat3d(&t->o2w,&video3dinfo.w2v,&v2o); // o2v
	inversemat3d(&v2o,&v2o);
	v2o.e[3][0]=v2o.e[3][1]=v2o.e[3][2]=0.0f; // relative to center point (vector, not a point)
	v2o.e[3][3]=1.0f; // just in case
	for (i=0;i<NPARTS;++i) {
		float fi=float(i);
		pointf3 po;
		po.x=mt_frand()*10.0f-5.0f;
		po.y=mt_frand()*10.0f-5.0f;
		po.z=mt_frand()*10.0f-5.0f;
		for (j=0;j<3;++j) {
			pointf3 vo;
			xformvec(&v2o,&parttrixyz[j],&vo);
			p[3*i+j].x=po.x+vo.x;
			p[3*i+j].y=po.y+vo.y;
			p[3*i+j].z=po.z+vo.z;
			cv[3*i+j]=parttricvs[j];
		}
	}
	m->copyverts(p,NPARTS*3);
	m->copycverts(cv,NPARTS*3);
	delete[] p;
	delete[] cv;
}

static tree2* buildparticles(const pointf3& size,const C8* texname,const C8* matname)
{
	tree2* t=new tree2("particles");
	modelb* m=model_create(unique());
	t->setmodel(m);
	if (model_getrc(m)==1) {
		S32 i;
//		m->copynorms(prismnorms,24);
		makeverts(t);
 // create a texture
		if (texname) {
			uv* uvs=new uv[NPARTS*3];
			textureb* texmat0;
			for (i=0;i<NPARTS;++i) {
				uvs[3*i  ]=parttriuvs[0];
				uvs[3*i+1]=parttriuvs[1];
				uvs[3*i+2]=parttriuvs[2];
			}
			m->copyuvs0(uvs,NPARTS*3);
			texmat0=texture_create(texname);
			if (texture_getrc(texmat0)==1) {
				pushandsetdir("res3d");
				texmat0->load();
				popdir();
			} 
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_HASSHADE,texmat0,0,50,NPARTS,NPARTS*3);
			delete[] uvs;
		} else 
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASSHADE,0,0,50,NPARTS,NPARTS*3);
		for (i=0;i<NPARTS;++i)
			m->addface(3*i,3*i+1,3*i+2);
		m->close();
	}
	t->treecolor.w=.9f;
	return t;
}

////////////////////////// main
void particles_init()
{
	lightinfo.dodefaultlights=true;
	video_setupwindow(GX,GY);
	roottree=new tree2("roottree");
// build a cube
	pushandsetdir("gfxtest");
	tree2* m=buildparticles(pointf3x(1,1,1),"maptestnck.tga","cvert");
	roottree->linkchild(m);
// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=400;
	mainvp.camtrans=pointf3x(0,0,-10);
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
	//mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	popdir();
}

void particles_proc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_PARTICLES);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_PARTICLES);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
		changestate(STATE_PARTICLES);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		changestate(STATE_PARTICLES);
	}
	doflycam(&mainvp);
	roottree->proc();
}

void particles_draw3d()
{
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	tree2* t=*(roottree->children.begin());
	makeverts(t);
	t->mod->update();
	dolights();
	video_drawscene(roottree);
//	video_endscene(); // nothing right now
}

void particles_exit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
}
