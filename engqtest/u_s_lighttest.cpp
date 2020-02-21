// simple sphere with spinning lights
#define INCLUDE_WINDOWS
#define D2_3D
#include <m_eng.h>
#include "m_perf.h"
#include "u_states.h"

static U32 togvidmode;

static tree2* roottree;
static bool defsave;

#define LONGI 7
#define LATI 7
#define RAD 10.0f
//#define HEIGHT 20.0f

/*static void setxyz(struct pointf3* v,S32 i,S32 j)
{
	float anglo,angla,sanglo,canglo,sangla,cangla;
	if (i==LONGI)
		i=0;
	anglo=i*(TWOPI/LONGI);
	angla=PI/2-j*(PI/LONGI);
	sanglo=rsin(anglo);
	canglo=-rcos(anglo);
	sangla=rsin(angla);
	cangla=rcos(angla);
	v->x=RAD*sanglo*cangla;
	v->z=RAD*canglo*cangla;
	v->y=RAD*sangla;
}
*/
static void setxyznorms(struct pointf3* v,struct pointf3* n,S32 i,S32 j)
{
	float anglo,angla,sanglo,canglo,sangla,cangla;
	if (i==LONGI)
		i=0;
	anglo=i*(TWOPI/LONGI);
	angla=PI/2-j*(PI/LONGI);
	sanglo=sinf(anglo);
	canglo=-cosf(anglo);
	sangla=sinf(angla);
	cangla=cosf(angla);
	v->x=RAD*sanglo*cangla;
	v->z=RAD*canglo*cangla;
	v->y=RAD*sangla;
	normalize3d(v,n);
}

////////////////////////// main
void lighttestinit()
{
	defsave=lightinfo.dodefaultlights;
	lightinfo.dodefaultlights=false;
	video_setupwindow(GX,GY);
//	video_setupwindow(320,240);
//	video_setupwindow(800,600);
	roottree=new tree2("roottree");
// build some lights
	tree2* spheretree=new tree2("oneface1");
	spheretree->treecolor=F32WHITE;
//	spheretree->flags|=TF_TREECOLOR;
	pushandsetdir("gfxtest");
	modelb* mod;
//	mod=model2::rc.newrc("sphere1");
	mod=model_create("sphere1");
#if 1
//	if (model2::rc.getrc(mod)==1) {
	if (model_getrc(mod)==1) {
		struct pointf3* verts;
		struct uv* uvs;
//		struct pointf3* shades;
		struct pointf3* norms;
		verts=new pointf3[LONGI*LATI*4];
		uvs=new uv[LONGI*LATI*4];
//		shades=new pointf3[LONGI*LATI*4];
		norms = new pointf3[LONGI*LATI*4];
		S32 i,j;
		for (j=0;j<LATI;++j) {
			for (i=0;i<LONGI;++i) {
				static struct uv uv00={0,0};
				static struct uv uv10={1,0};
				static struct uv uv01={0,1};
				static struct uv uv11={1,1};
//				static struct pointf3 shade00={.99f,.01f,.01f};
//				static struct pointf3 shade10={.01f,.99f,.01f};
//				static struct pointf3 shade01={.01f,.01f,.99f};
//				static struct pointf3 shade11={.99f,.99f,.99f};
				setxyznorms(&verts[4*(i+j*LONGI)+0],&norms[4*(i+j*LONGI)+0],i,j);
				setxyznorms(&verts[4*(i+j*LONGI)+1],&norms[4*(i+j*LONGI)+1],i+1,j);
				setxyznorms(&verts[4*(i+j*LONGI)+2],&norms[4*(i+j*LONGI)+2],i,j+1);
				setxyznorms(&verts[4*(i+j*LONGI)+3],&norms[4*(i+j*LONGI)+3],i+1,j+1);
				uvs[4*(i+j*LONGI)+0]=uv00;
				uvs[4*(i+j*LONGI)+1]=uv10;
				uvs[4*(i+j*LONGI)+2]=uv01;
				uvs[4*(i+j*LONGI)+3]=uv11;
//				shades[4*(i+j*LONGI)+0]=shade00;
//				shades[4*(i+j*LONGI)+1]=shade10;
//				shades[4*(i+j*LONGI)+2]=shade01;
//				shades[4*(i+j*LONGI)+3]=shade11;
			}
		}
		mod->copyverts(verts,LONGI*LATI*4);
		mod->copyuvs0(uvs,LONGI*LATI*4);
//		mod->copycverts(shades,LONGI*LATI*4);
		mod->copynorms(norms,LONGI*LATI*4);
		delete[] verts;
		delete[] uvs;
//		delete[] shades;
		delete[] norms;
//		texture2* texmat0=texture2::rc.newrc("maptest.tga");
//		if (texture2::rc.getrc(texmat0)==1) {
		textureb* texmat0=texture_create("maptest.tga");
//		textureb* texmat0=texture_create("mayor1.jpg");
//		textureb* texmat0=texture_create("rengst.jpg");
		if (texture_getrc(texmat0)==1) {
			texmat0->load();
		}
//		mod->addmat("vdiff",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,
		mod->addmat("pdiff",SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,
		  texmat0,&F32WHITE,30,2*LONGI*(LATI-1),LONGI*LATI*4);
		for (j=0;j<LATI;++j) {
			for (i=0;i<LONGI;++i) {
				if (j!=0)
					mod->addface(4*(i+j*LONGI)+0,4*(i+j*LONGI)+1,4*(i+j*LONGI)+2);
				if (j!=LATI-1)
					mod->addface(4*(i+j*LONGI)+3,4*(i+j*LONGI)+2,4*(i+j*LONGI)+1);
			}
		}
		mod->close();
	}
#endif
	spheretree->setmodel(mod);
//	spheretree->flags|=TF_CALCLIGHTSONCE;
	spheretree->rotvel.y=.001f;
	spheretree->rotvel.x=.0004f;
	roottree->linkchild(spheretree);
	popdir();
	tree2* amblit=new tree2("amblit");
	amblit->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	amblit->lightcolor=pointf3x(.25,.25,.25,0);
	addlighttolist(amblit);
	roottree->linkchild(amblit);
	tree2* dirlit;
	dirlit=new tree2("dirlit");
	dirlit->name="dirlit";
	dirlit->flags|=TF_ISLIGHT;
	dirlit->lightcolor=F32WHITE;
	dirlit->rotvel.x=0;
	dirlit->rotvel.y=.04f;
	dirlit->rotvel.z=0;
	addlighttolist(dirlit);
	roottree->linkchild(dirlit);
/*	dirlit=new tree2("dirlit2");
	dirlit->name="dirlit2";
	dirlit->flags|=TF_ISLIGHT;
	dirlit->lightcolor=F32WHITE;
	dirlit->rot.y=PI/2;
	dirlit->rotvel.x=.004f;
	dirlit->rotvel.y=0;
	dirlit->rotvel.z=0;
	addlighttolist(dirlit);
	roottree->linkchild(dirlit); */
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.125f;
	mainvp.zback=1000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z=-30;
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	togvidmode=videoinfo.video_maindriver;
}

void lighttestproc()
{
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY== K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_LIGHTTEST);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_LIGHTTEST);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
//		video_setupwindow(GX,GY);
		changestate(STATE_LIGHTTEST);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
//		video_setupwindow(GX,GY);
		changestate(STATE_LIGHTTEST);
	}
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	mainvp.xres=WX; // incase res has changed..
	mainvp.yres=WY;
	roottree->proc();
	doflycam(&mainvp);
}

void lighttestdraw3d()
{
//	buildtreematrices(roottree); //roottree,camtree);
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	dolights();
	video_buildworldmats(roottree);
	video_drawscene(roottree);
//	video_endscene(0); // nothing right now
//	video_lock();
//	video_unlock();
}

/*
void checkheaps()
{
//	if (!_CrtCheckMemory())
//		errorexit("heap bad");
	HANDLE heaps[50];
	S32 nheaps=50;
	nheaps=GetProcessHeaps(nheaps,heaps);
	S32 i;
	for (i=0;i<nheaps;++i) {
		logger("heap %d: %08x\n",i,heaps[i]);
		S32 r=HeapValidate(heaps[i],0,0);
		if (!r)
			errorexit("bad heap!");
	}
}
*/
//_CRTIMP int __cdecl _CrtCheckMemory();
void lighttestexit()
{
	lightinfo.dodefaultlights=defsave;
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
//	checkheaps();
	delete roottree;
}
