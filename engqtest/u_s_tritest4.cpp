// test sprites
#define D2_3D // all the 3d baggage
#include <m_eng.h>

#include "u_states.h"

//#include "u_modelutil.h" // handy helpers

namespace {
//#define DISABLEMODEL

U32 togvidmode;
tree2* roottree;
S32 uselightssave;
S32 frm;
textureb* sptex,*atex;
float rot;
// newer one with textures
struct pointf3 rectverts[]={
	{ -1, 1,0},
	{  1, 1,0},
	{ -1,-1,0},
	{  1,-1,0},
};

struct uv rectuvs[]={
//	{  0,  0},
//	{.5f,  0},
//	{  0,.5f},
//	{.5f,.5f},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
};

struct pointf3 rectnorms[]={
	{-.5773f, .5773f,-.5773f},
	{ .5773f, .5773f,-.5773f},
	{-.5773f,-.5773f,-.5773f},
	{ .5773f,-.5773f,-.5773f},
//	{0,0,-.05f},
//	{0,0,-1},
//	{0,0,-1},
//	{0,0,-1},
};

struct face rectfaces[]={
	{{0,1,2},0},
	{{2,1,3},0},
};

struct menuvar evars[]={
	{"@lightred@---- sprite test vars -------------------",0,D_VOID,0},
	{"point 0 x",&rectverts[0].x,D_FLOAT,FLOATUP/8},
	{"point 0 y",&rectverts[0].y,D_FLOAT,FLOATUP/8},
	{"point 0 z",&rectverts[0].z,D_FLOAT,FLOATUP/8},
	{"point 1 x",&rectverts[1].x,D_FLOAT,FLOATUP/8},
	{"point 1 y",&rectverts[1].y,D_FLOAT,FLOATUP/8},
	{"point 1 z",&rectverts[1].z,D_FLOAT,FLOATUP/8},
	{"point 2 x",&rectverts[2].x,D_FLOAT,FLOATUP/8},
	{"point 2 y",&rectverts[2].y,D_FLOAT,FLOATUP/8},
	{"point 2 z",&rectverts[2].z,D_FLOAT,FLOATUP/8},
	{"point 3 x",&rectverts[3].x,D_FLOAT,FLOATUP/8},
	{"point 3 y",&rectverts[3].y,D_FLOAT,FLOATUP/8},
	{"point 3 z",&rectverts[3].z,D_FLOAT,FLOATUP/8},
	{"uv 0 x",&rectuvs[0].u,D_FLOAT,FLOATUP/32},
	{"uv 0 y",&rectuvs[0].v,D_FLOAT,FLOATUP/32},
	{"uv 1 x",&rectuvs[1].u,D_FLOAT,FLOATUP/32},
	{"uv 1 y",&rectuvs[1].v,D_FLOAT,FLOATUP/32},
	{"uv 2 x",&rectuvs[2].u,D_FLOAT,FLOATUP/32},
	{"uv 2 y",&rectuvs[2].v,D_FLOAT,FLOATUP/32},
	{"uv 3 x",&rectuvs[3].u,D_FLOAT,FLOATUP/32},
	{"uv 3 y",&rectuvs[3].v,D_FLOAT,FLOATUP/32},
};
const S32 nevars=sizeof(evars)/sizeof(evars[0]);

tree2* onefacemodeltree,*onefacemodeltree2;
modelb* dumm;

modelb* buildthestaticmodel(const C8* modelname,const C8* matname,const C8* texname)
{
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		modb->copyverts(rectverts,4);
		modb->copyuvs0(rectuvs,4);
		modb->copynorms(rectnorms,4);
		pushandsetdir("gfxtest");
		textureb* texmat=texture_create(texname);
		if (texture_getrc(texmat)==1) {
//			colorkeyinfo.usecolorkey=0;
			texmat->load();
//			colorkeyinfo.usecolorkey=1;
		}
		popdir();
//		modb->addmat("tex",0,1.0f,texmat,&F32RED,2,4);
		modb->addmat(matname,SMAT_HASTEX|SMAT_HASWBUFF|SMAT_CALCLIGHTS|SMAT_HASSHADE/*|SMAT_CLAMP*/,texmat,&F32WHITE,30,2,4);
//		modb->addmat("tex",0,1.0f,0,&F32YELLOW,1,3);
		modb->addfaces(rectfaces,2,0);
		modb->close();
	}
	return modb;
}

// make a model move and also uv tweaking
void altermodel()
{
	pointf3 newv[4];
	uv newuv[4];
	memset(newuv,0,sizeof(newuv));
	U32 i;
	for (i=0;i<4;++i) {
		newv[i].x=rectverts[i].x+frm*.01f;
		newv[i].y=rectverts[i].y;
		newv[i].z=rectverts[i].z;
	}
	newuv[1].u=newuv[2].v=newuv[3].u=newuv[3].v=frm*.01f+1;
	onefacemodeltree->mod->copyverts(newv,4);
	onefacemodeltree->mod->copyuvs0(newuv,4);
	onefacemodeltree->mod->update();
}

fontq* beat,*huge,*med;
} // end namespace tritest4

////////////////////////// main
void tritest4init()
{
	pushandsetdir("gfxtest");
	video_setupwindow(GX,GY);
//	video_setupwindow(800,800);
// a sprite
//	sptex=texture_create("243-normal.jpg");
	sptex=texture_create("clinton.pcx");
	if (texture_getrc(sptex)==1) {
		colorkeyinfo.usecolorkey=0;
		sptex->load();
		colorkeyinfo.usecolorkey=1;
	} 
	pushandsetdir("fonts");
//	atex=texture_create("xpar.png");
	atex=texture_create("beat/065.png");
	if (texture_getrc(atex)==1)
		atex->load();
	beat = new fontq("beat");
	med = new fontq("med");
	huge = new fontq("huge");
	popdir();
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=1;
	roottree=new tree2("roottree");
	onefacemodeltree=new tree2("built");
	onefacemodeltree2=new tree2("built2");
	dumm=model_create("dumm");
#ifndef DISABLEMODEL
	textureb* texmat=texture_create("bark.tga");
	if (texture_getrc(texmat)==1) {
//		colorkeyinfo.usecolorkey=0;
		texmat->load();
//		colorkeyinfo.usecolorkey=1;
	} 
#endif
	popdir();
#ifndef DISABLEMODEL
	dumm->addmat("tex",SMAT_HASTEX,texmat,&F32RED,50,0,0);
#endif
	dumm->close();
#ifndef DISABLEMODEL
//		textureb* texmat=texture_create("bark.tga");
//		textureb* texmat=texture_create("letters.pcx");
//		textureb* texmat=texture_create("europeflag.jpg");
	modelb* mod=buildthestaticmodel("static","tex","maptest.tga");
	modelb* mod2=buildthestaticmodel("static2","tex","bark.tga");
	onefacemodeltree->setmodel(mod);
	onefacemodeltree2->setmodel(mod2);
//	onefacemodeltree->trans.z=100.5f;
#endif
#if 1
	S32 i;
	for (i=0;i<10;++i) {
		tree2* dt=onefacemodeltree->newdup();
		dt->trans.z=(float)i;
		dt->rot.y=-PI/8;
		roottree->linkchild(dt); 
	}
	for (i=0;i<10;++i) {
		tree2* dt=onefacemodeltree2->newdup();
		dt->trans.z=(float)(i);
		dt->rot.y=PI/8;
		roottree->linkchild(dt); 
	}
#endif
	mainvp.backcolor=C32LIGHTMAGENTA;
	mainvp.zfront=.025f;
	mainvp.zback=10000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	static bool once;
	if (!once) {
		mainvp.camtrans=pointf3x(0,3,-1);
		mainvp.camrot=pointf3x(.9f,.7f,0);
		once=true;
	}

	togvidmode=videoinfo.video_maindriver;
//	extradebvars(evars,nevars);
	rot=0;
	frm=0;
//	wininfo.fpswanted=5;
}

void tritest4proc()
{
/*	delete onefacemodeltree;
	onefacemodeltree=new tree2("built");
	modelb* mod=buildthestaticmodel("static","test4");
	onefacemodeltree->setmodel(mod);
	roottree->linkchild(onefacemodeltree); */
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST4);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST4);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST4);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_TRITEST4);
	}
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.xres=WX;
	mainvp.yres=WY;
	altermodel();
	roottree->proc();
	doflycam(&mainvp);
	rot+=PI/30/4;
//	rot=normalangrad(rot);
	if (rot>PI/2)
		rot=0;
	++frm;
}

void tritest4draw3d()
{
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree); // help dolights
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
//	list<tree2*>::iterator it=roottree->children.begin();
//	if (it!=roottree->children.end())
//		(*it)->trans.y+=5;
//	video_buildworldmats(roottree); // help dolights
	mainvp.flags=0;//|VP_CHECKER;
	mainvp.camtrans.x-=5;
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
//	if (it!=roottree->children.end())
//		(*it)->trans.y-=5;
	video_sprite_begin(800,600); // set virtual screen size (matches resolution of art) , (640 480 maybe)
	video_sprite_draw(sptex,F32GREEN,400,300,400,300,.25f,.25f,rot,.6f,.6f,.85f,.95f);
	video_sprite_draw(sptex,F32BLUE,10,10,(float)MX-10,(float)MY-10);
	video_sprite_draw(sptex,F32BLUE,10,310,(float)MX-10,(float)MY-10);
	med->setscale(1.5f);
	med->setcenter(true);
	med->setleftmarg(10);
	med->settopmarg(10);
	med->print(10,10,(float)MX-10,(float)MY-10,F32RED,"WIMIW");
	med->print(10,90,(float)MX-10,(float)MY-10,F32RED,"ABCDE");
	med->setfixedwidth(80);
	med->print(10,310,(float)MX-10,(float)MY-10,F32RED,"WIMIW");
	med->print(10,390,(float)MX-10,(float)MY-10,F32RED,"ABCDE");
	med->setfixedwidth(0); // back to var width
	video_sprite_end();
	mainvp.camtrans.x+=5;
}

void tritest4exit()
{
//	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete onefacemodeltree;
	delete onefacemodeltree2;
	delete roottree;
	lightinfo.uselights=uselightssave;
	modelb::rc.deleterc(dumm);
	textureb::rc.deleterc(sptex);
	textureb::rc.deleterc(atex);
	wininfo.fpswanted=30;
	delete beat;
	delete med;
	delete huge;
}
