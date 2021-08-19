/// draw lots of stuff
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

//#include <d3d9.h>
//#include "d2_dx9.h" // for seetmodel3version // temporary
#include "u_modelutil.h"

//#include "d2_font.h" // font queue, test font queue etc.

//#define ALTTEX
//#define ALTCOLOR
#define DOROTVEL
#define DOSPEC

static U32 togvidmode;
static tree2* roottree;
static vector<tree2*> masters;
static float specpowval = 0;//500;

static struct menuvar edv[] = {
	{"@lightred@----- adjust parameters of many --------------",NULL,D_VOID,0},
	{"specpowval",&specpowval,D_FLOAT,FLOATUP/4},
};
static const int nedv = NUMELEMENTS(edv);

static C8* rndtex[]={
	"white.pcx",
/*	"clinton.pcx",
	"bark.tga",
	"rengst.jpg",
	"stones.jpg",
	"gamesel.pcx" */
};

static pointf3 ambcolsave,dircolsave;

#define NRNDTEX sizeof(rndtex)/sizeof(rndtex[0])
////////////////////////// main
void manyinit()
{
    //int* a = new int;
    //delete a;
	specpowval = 500;
	extradebvars(edv,nedv);
	lightinfo.dodefaultlights=true;
	video_setupwindow(GX,GY);
	roottree=new tree2("roottree");
	roottree->trans = pointf3x(0,0,10);
// build a cube
	pushandsetdir("gfxtest");
	tree2* m,*c;
#ifdef DOSPEC
	//const C8* shadername = "pdiffspec";
	const C8* shadername = "omni";
#else
	const C8* shadername = "tex";
#endif
	//const C8* shadername = "tex";
	// add some objects to master list
		// cube
	m = buildprism(pointf3x(1,1,1),"maptestnck.tga",shadername);
	masters.push_back(m);
		// sphere
	m = buildsphere(.5f,"maptestnck.tga",shadername);
	masters.push_back(m);
		// cylinder
	c = buildcylinder_xz(pointf3x(.5f,1,.5f),"maptestnck.tga",shadername);
	//c = buildcylinder_xz(pointf3x(.5f,1,.5f),"white.pcx",shadername);
	c->trans = pointf3x(0,-.5f,0); // center cylinder
	m = new tree2("cyl_root");
	m->linkchild(c);
	masters.push_back(m);
		// torus
	m = buildtorus_xz(.25f,.125f,"maptestnck.tga",shadername);
	masters.push_back(m);

//	m->mod->mats[0].msflags&=~SMAT_HASWBUFF; // turn off z buffer
	mt_setseed(24);
	U32 i,j,k,el;
	U32 numcnt = 10;
	for (k=0;k<numcnt;++k) {
		for (j=0;j<numcnt;++j) {
			for (i=0;i<numcnt;++i) {
				el = mt_random(masters.size());
				tree2* p=masters[el]->newdup();
#ifdef ALTTEX
				p->alttex=texture_create(rndtex[mt_random(NRNDTEX)]);
				if (texture_getrc(p->alttex)==1) {
					p->alttex->load();
				}
#endif
				p->trans=pointf3x(2.0f*i,2.0f*j,2.0f*k);
#ifdef DOROTVEL
				p->rotvel.x=mt_frand()*.005f;
				p->rotvel.y=mt_frand()*.005f;
#endif
//				p->flags|=TF_TREECOLOR;
//				p->treecolor=pointf3x(mt_frand(),mt_frand(),mt_frand(),mt_frand());
#ifdef ALTCOLOR
				p->treecolor=pointf3x(mt_frand(),mt_frand(),mt_frand(),.65f);
#endif
				roottree->linkchild(p);
			}
		}
	}
// setup viewport
	mainvp.backcolor=C32RED;
	mainvp.zfront=.01f;
	mainvp.zback=400;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xsrc=WX;
	mainvp.ysrc=WY;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	dolights(); // this will set ambient and directional light to default colors on first call only
	// save default lights
	ambcolsave = lightinfo.ambcolor;
	dircolsave = lightinfo.lightcolors[0];
	// change default lights
	lightinfo.ambcolor = pointf3x(.125f,.125f,.125f);
	lightinfo.lightcolors[0] = pointf3x(.5f,.5f,.5f);

	lightinfo.deflightpos[0] = pointf3x(10,10,12);

	popdir();
}

void manyproc()
{
	U32 i;
	for (i=0;i<masters.size();++i) {
		tree2* m = masters[i];
		if (m->mod)
			m->mod->mats[0].specpow = specpowval;
		// it might be a tree with childeren, cylinder (top,middle,bottom)
		else {
			tree2* c = *m->children.begin();
			list<tree2*>::iterator j;
			for (j=c->children.begin();j!=c->children.end();++j) {
				tree2* mc = *j;
				mc->mod->mats[0].specpow = specpowval;
			}

		}
	}
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
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);
#if 0
	LARGEFONT->outtextxybf32(B32,200,200,C32GREEN,C32BLACK,"Hello world!");
	SMALLFONT->outtextxybf32(B32,300,300,C32GREEN,C32BLACK,"Hello world!");
	clipblit32(video3dinfo.sysfont2,B32,0,0,400,400,B32->size.x,B32->size.y);
	drawtextque_string_foreback(LARGEFONT,600,600,F32GREEN,F32MAGENTA,"textque");
#endif

//	video_endscene(); // nothing right now
}

void manyexit()
{
	logger("logging roottree\n");
	// restore default lights back to original values
	lightinfo.ambcolor = ambcolsave;
	lightinfo.lightcolors[0] = dircolsave;
	lightinfo.deflightpos[0] = pointf3x();
	extradebvars(0,0);
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	U32 i;
	for (i=0;i<masters.size();++i)
		delete masters[i];
	masters.clear();
	delete roottree;
	logger("logging reference lists after free\n");
	logrc();
}
