/// draw lots of stuff
#define D2_3D
#include <m_eng.h>
#include "system/u_states.h"

#include <d3d9.h>
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "utils/modelutil.h"

static U32 togvidmode;
static tree2* roottree;

C8* rndtex[]={
	"bark.tga",
	"clinton.pcx",
	"rengst.jpg",
	"stones.jpg",
	"gamesel.pcx"
};

#define NRNDTEX sizeof(rndtex)/sizeof(rndtex[0])
////////////////////////// main
void manyinit()
{
	lightinfo.dodefaultlights=true;
	video_setupwindow(GX,GY);
	roottree=new tree2("roottree");
// build a cube
	pushandsetdir("gfxtest");
	tree2* m=buildprism(pointf3x(1,1,1),"maptestnck.tga","pdiff");

//	m->mod->mats[0].msflags&=~SMAT_HASWBUFF;
	S32 i,j,k;
	for (k=0;k<10;++k) {
		for (j=0;j<10;++j) {
			for (i=0;i<10;++i) {
				tree2* p=m->newdup();
				p->alttex=texture_create(rndtex[mt_random(NRNDTEX)]);
				if (texture_getrc(p->alttex)==1) {
					p->alttex->load();
				} 
				p->trans=pointf3x(2.0f*i,2.0f*j,2.0f*k);
				p->rotvel.x=mt_frand()*.05f;
				p->rotvel.y=mt_frand()*.05f;
//				p->flags|=TF_TREECOLOR;
//				p->treecolor=pointf3x(mt_frand(),mt_frand(),mt_frand(),mt_frand());
				p->treecolor=pointf3x(mt_frand(),mt_frand(),mt_frand(),.65f);
				roottree->linkchild(p);
			}
		}
	}

	delete m;
// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=400;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	popdir();
}

void manyproc()
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
		changestate(STATE_MANY);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_MANY);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
		changestate(STATE_MANY);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		changestate(STATE_MANY);
	}
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	doflycam(&mainvp);
	roottree->proc();
}

void manydraw3d()
{
	video_buildworldmats(roottree);
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	dolights();
	video_drawscene(roottree);
//	video_endscene(); // nothing right now
}

void manyexit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
}
