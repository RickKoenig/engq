#define INCLUDE_WINDOWS
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include <vfw.h>

static struct pointf3 texvertsvc[]={
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1,-1},
	{ 1,-1,-1},
	{ 1, 1, 1},
	{-1, 1, 1},
	{ 1,-1, 1},
	{-1,-1, 1},

	{ 1, 1,-1},
	{ 1, 1, 1},
	{ 1,-1,-1},
	{ 1,-1, 1},
	{-1, 1, 1},
	{-1, 1,-1},
	{-1,-1, 1},
	{-1,-1,-1},
	
	{-1, 1, 1},
	{ 1, 1, 1},
	{-1, 1,-1},
	{ 1, 1,-1},
	{-1,-1,-1},
	{ 1,-1,-1},
	{-1,-1, 1},
	{ 1,-1, 1},
};

static struct uv texuvsvc[]={
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
	{0,0},
	{1,0},
	{0,1},
	{1,1},
};

static struct face texfacesvc[]={
	{{0,1,2},0},
	{{3,2,1},0},
};


static shape *rl,*focus=0;
static pbut* quit,*vcstart,*vcstop,*grabf,*vcquit,*pvidcomp,*pviddisplay,*pvidformat,*pvidsource,*p3d;
static text* tstatus;

static S32 frame;
static U32 in3d;
static tree2* roottree;
static S32 uselightssave;
static textureb* texmat0;
void vidcap_init()
{
// enum camera devices
	in3d=0;
	frame=0;
// setup state
	video_setupwindow(800,600);//,565);
	pushandsetdir("vidcap");
	script sc("vcres.txt");
	factory2<shape> fs;
	rl=fs.newclass_from_handle(sc);
	popdir();
// find all buttons
	quit=rl->find<pbut>("QUIT");
	vcstart=rl->find<pbut>("VCSTART");
	vcstop=rl->find<pbut>("VCSTOP");
	grabf=rl->find<pbut>("GRABF");
	vcquit=rl->find<pbut>("VCQUIT");
	pvidcomp=rl->find<pbut>("PVIDCOMP");
	pviddisplay=rl->find<pbut>("PVIDDISPLAY");
	pvidformat=rl->find<pbut>("PVIDFORMAT");
	pvidsource=rl->find<pbut>("PVIDSOURCE");
	p3d=rl->find<pbut>("P3D");
	tstatus=rl->find<text>("TSTATUS");
	focus=0;
// init 3d
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=0;
	pushandsetdir("gfxtest");
	roottree=new tree2("roottree");
	tree2* testobj=new tree2("built");
	modelb* mod=model_create("amod");
	if (model_getrc(mod)==1) {
		mod->copyverts(texvertsvc,24);
		mod->copyuvs0(texuvsvc,24);
		texmat0=texture_create("bark.tga");
		if (texture_getrc(texmat0)==1) {
			colorkeyinfo.usecolorkey=0;
			texmat0->load();
			colorkeyinfo.usecolorkey=1;
		}
		mod->addmat("tex",SMAT_HASTEX,texmat0,0,30,12,24);
		S32 i;
		for (i=0;i<6;++i)
			mod->addfaces(texfacesvc,2,4*i);
		mod->close();
	}
	testobj->setmodel(mod);
	popdir();
	roottree->linkchild(testobj);
	testobj->rotvel.y=.01f;
	testobj->rotvel.x=.002f;
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=10000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z=-3;
	mainvp.camtrans.x=0;
	mainvp.camtrans.y=0;
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
}

static struct bitmap32* cbm32;
void vidcap_proc()
{
// set focus
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
// if something selected...
	if (KEY==K_ESCAPE)
		popstate();
	if (focus) {
		S32 ret=focus->proc();
		if (focus==quit) { // quit button
			if (ret==1)
				popstate();
		} else if (focus==vcstart) { 
			if (ret==1) {
				if (vcaminit())
					tstatus->settname("vcam driver inited");
				else
					tstatus->settname("can't init vcam driver");
			}
		} else if (focus==vcstop) { 
			if (ret==1) {
				vcamstopstream();
				tstatus->settname("done streaming");
			}
		} else if (focus==grabf) { 
			if (ret==1) {
				if (vcamstartstream())	
					tstatus->settname("vcam streaming");
				else
					tstatus->settname("can't stream vcam");
			}
		} else if (focus==vcquit) { 
			if (ret==1) {
				vcamstopstream();
				vcamexit();
				tstatus->settname("done with driver");
			}
		} else if (focus==pvidcomp) { 
			if (ret==1) {
				vcamcompression();
			}
		} else if (focus==pviddisplay) { 
			if (ret==1) {
				vcamdisplay();
			}
		} else if (focus==pvidformat) { 
			if (ret==1) {
				vcamformat();
			}
		} else if (focus==pvidsource) { 
			if (ret==1) {
				vcamsource();
			}
		} else if (focus==p3d) { 
			if (ret==1) {
				in3d^=1;
			}
		}
	}
	cbm32=vcamgrabframe();
	roottree->proc();
	video_buildworldmats(roottree);
	doflycam(&mainvp);
	frame++;
}

void vidcap_draw3d()
{
	video_setviewport(&mainvp); // clear zbuf etc.
	if (cbm32 && in3d) {
		bitmap32* lt=texmat0->locktexture();
		clipblit32(cbm32,lt,
		max(0,(cbm32->size.x-lt->size.x)>>1),max(0,(cbm32->size.y-lt->size.y)>>1),
		max(0,(lt->size.x-cbm32->size.x)>>1),max(0,(lt->size.y-cbm32->size.y)>>1),
		min(cbm32->size.x,lt->size.x),min(cbm32->size.y,lt->size.y));
		texmat0->unlocktexture();
		video_drawscene(roottree);
	}
}

void vidcap_draw2d()
{
	if (cbm32 && !in3d)
		clipblit32(cbm32,B32,0,0,(WX-cbm32->size.x)/2,(WY-cbm32->size.y)/2,cbm32->size.x,cbm32->size.y);
	rl->draw();
	outtextxyf32(B32,10,10,C32WHITE,"vidcap frame %6d, in3d --",frame);
}

void vidcap_exit()
{
	vcamstopstream();
	vcamexit();
	delete rl;
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	lightinfo.uselights=uselightssave;
}
