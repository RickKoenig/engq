/// draw cubemaps and skyboxes in 3d
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

#include <d3d9.h>
#include "d2_dx9.h" // for seetmodel3version // temporary
#include "u_modelutil.h"

//#define USEGLOBALENV
//#define FMOD
#define PERSTEST

static U32 togvidmode;
static tree2* roottree;
static tree2* skyboxtree;
//static bool dosky = true;
/*
static const C8* cubemapfilelist[] = {
	"CUB_cube02",
	"CUB_cubemap_mountains.jpg",
	"CUB_cube2.jpg",
	"CUB_cubicmap.jpg",
	"CUB_cube.jpg",
	"CUB_footballfield",
	"CUB_fishpond",
	"CUB_skansen",
	"CUB_skansen4",
};
*/
static const C8* skyboxfilelist[] = {
	"cube02",
	"cubemap_mountains.jpg",
	"cube2.jpg",
	"cubicmap.jpg",
	"cube.jpg",
	"footballfield",
	"fishpond",
	"skansen",
	"skansen4",
};
static const S32 ncubemaplist = NUMELEMENTS(skyboxfilelist);
static S32 curcube = 0;

static tree2* buildtestsquarecubemap()
{
	const C8 treename[] = "tree_cubemaptest";
	const C8 modname[] = "mod_cubemaptest";
	const C8 matname[] = "cubemaptest";
	//const C8 texname[] = "CUB_cube02";
	//const C8 texname[] = "CUB_FishPond";
	string sname = string("CUB_") + skyboxfilelist[curcube];
	const C8 *texname = sname.c_str();
	logger("squarecubemapname = '%s'\n",texname);
	pushandsetdir("skybox");
	tree2* ret = new tree2(treename);
	modelb* m;
	m=model_create(modname);
	if (model_getrc(m)==1) { // new instance of model
		pointf3 verts[] = {
			{-.5f, .5f,0.0f},
			{ .5f, .5f,0.0f},
			{-.5f,-.5f,0.0f},
			{ .5f,-.5f,0.0f},
		};
		m->copyverts(verts,4);
		uv uvs[] = {
			{0.0f,0.0f},
			{1.0f,0.0f},
			{0.0f,1.0f},
			{1.0f,1.0f},
		};
		m->copyuvs0(uvs,4);
#if 1
		pointf3 norms[] = {
			{0.0f,0.0f,1.0f},
			{0.0f,0.0f,1.0f},
			{0.0f,0.0f,1.0f},
			{0.0f,0.0f,1.0f},
		};
#else
		pointf3 norms[] = {
			{-.5f, .5f,1.0f},
			{ .5f, .5f,1.0f},
			{-.5f,-.5f,1.0f},
			{ .5f,-.5f,1.0f},
		};
#endif
		m->copynorms(norms,4);
		face faces[]={
			{ 0, 1, 2},
			{ 3, 2, 1},
		};
 // create a texture
		if (texname) {
			textureb* texmat0;
			//m->copyuvs0(prismuvs,24);
			texmat0=texture_create(texname);
			if (texture_getrc(texmat0)==1) {
				texmat0->load();
			} 
			m->addmat(matname,SMAT_HASWBUFF|SMAT_HASTEX|SMAT_CALCLIGHTS|SMAT_HASSHADE,texmat0,0,50,2,4);
		} else 
			m->addmat(matname,SMAT_HASWBUFF,0,0,50,2,4);
		m->addfaces(faces,2,0);
		float itn = .75f;
		m->mats[0].color = pointf3x(itn,itn,itn,1.0f);
		m->close();

//	    popdir();
	}
	ret->setmodel(m);
	popdir();
	return ret;
}

#ifdef FMOD
static float maxerr;
static void ft(float div)
{
	logger("fmodtest %f\n",div);
	float i;
	for (i = -6.0f;i <= 6.0f;i += .0625f) {
		float q = i/div;
		//q = floorf(q);
		q = (float)(int)q;
		float r = fmodf(i,div);
		bool baddiv = fabsf(r) >= fabsf(div);
		float ni = q*div + r;
		float err = i - ni;
		float abserr = fabsf(err);
		if (abserr > maxerr)
			maxerr = abserr;
		//if (abserr > .0001f)
			logger("%f/%f = %f,fmod(%f,%f) = %f, back = %f, err = %f, baddiv = %s\n",i,div,q,i,div,r,ni,i - ni,baddiv ? "true" : "false");
	}
}

static void fmodtest()
{
	logger("fmodtest\n");
	maxerr = -1e20f;
	float tests[] = {7.3f,5.5f,6.0f,2.1f,2.0f,.3f,-.3f,-2.0f,-2.1f,-6.0f,-5.5f,-7.3f};
	const S32 ntests = NUMELEMENTS(tests);
	S32 i;
	for (i=0;i<ntests;++i)
		ft(tests[i]);
	logger("maxerr = %f\n",maxerr);
}
/*	float i = fragCoord.x / iResolution.x; // 0 to 1
    i -= .5; // -.5 to .5
    i *= 4.0; // -2 to 2
    //i = fract(i);
    i = mod(i,2.0);
	fragColor = vec4(vec3(i),1.0);
*/
#endif

#ifdef PERSTEST
static void perstest()
{
	logger("in perstest\n");
	float maxerr = 0.0f;
	float s;
	for (s=0.0f;s<=1.0f;s+=(1.0f/64.0f)) {
		float p0z = 17.0f;
		float p0x = -p0z * .85f;
		float p1z = 7.0f;
		float p1x = p1z * .75f;
		// formula 1 projection
		float r0x = p0x/p0z;
		float r1x = p1x/p1z;
		float rx = r0x + (r1x - r0x)*s;
		float t1 = (p0z*rx - p0x)/((p1x - p0x) - (p1z - p0z)*rx);
		// formula 2 '1/w'
		float w0 = 1.0f/p0z;
		float w1 = 1.0f/p1z;
		float w = w0 + (w1 - w0)*s;
		// say s goes from 0 to 1
		float tw0 = 0.0f*w0;
		float tw1 = 1.0f*w1;
		float tw = tw0 + (tw1 - tw0)*s;
		float t2 = tw/w;
		// formula 3 intermediate
		float t3 = ((p0z*p1x/p1z - p0x)*s)
			/
			(p1x - p1z*p0x/p0z + (-p1x + p1z*p0x/p0z + p0z*p1x/p1z - p0x)*s);
		// formula 4 intermediate
		float t4 = ((p1x*p0z/p1z - p0x)*s)
			/
			(p1x*(1 - s + p0z/p1z*s) + p0x*(-p1z/p0z + p1z/p0z*s - s));
		// formula 5 intermediate, play with x, x doesn't seem to matter 'invariant'
		p1x *= 2.1414f + 41.0f;
		p0x *= 1.8f + 3.14f;
		float t5num = p1x*p0z/p1z*s - p0x*s;
		float t5den = p1x*(1 + s*(p0z/p1z - 1)) + p0x*(-p1z/p0z + s*(p1z/p0z - 1));
		//logger("t5num = %f, t5den = %f\n",t5num,t5den);
		float t5 = t5num/t5den;
		// formula 6 intermediate
		//p1x = p0x*p1z/p0z; // divide by 0
		//p1x += .01f; // offset from 0
		float t6num = p1x*p0z*p0z*s - p0x*p1z*p0z*s;
		float t6den = p1x*(p0z*p1z - s*p0z*p1z + s*p0z*p0z) + p0x*(-p1z*p1z + s*p1z*p1z - s*p0z*p1z);
		float t6 = t6num/t6den;
		// formula 7 serious factoring
		float t7num = s*p0z;
		float t7den = p1z - s*p1z + s*p0z;
		float t7 = t7num/t7den;

		// calc error
		float err = t2 - t7;
		float abserr = fabsf(err);
		if (abserr > maxerr)
			maxerr = abserr;
		logger("s = %8.5f, t2 = %8.5f, t7 = %8.5f, err = %8.5f\n",s,t2,t7,err);
	}
	logger("maxerr = %f\n",maxerr);
}

#endif
////////////////////////// main
void cubemaptestinit()
{
#ifdef FMOD
	fmodtest();
#endif
#ifdef PERSTEST
	perstest();
#endif
	//const string cubename = "FishPond";
	//const string cubename = "cube02";
	//const string cubename = "cubemap_mountains.jpg";
	lightinfo.dodefaultlights = true;
	video_setupwindow(GX,GY);
	pushandsetdir("gfxtest");
	textureb* mt = texture_create("maptestnck.tga");
	if (texture_getrc(mt)==1) {
		mt->load();
	} 
	popdir();
	roottree = new tree2("roottree");
// build a cube
	pushandsetdir("skybox");
	tree2* m;
#if 1
#ifdef USEGLOBALENV
	make_envv2tex("CUB_cube02");
	tree2* m=buildprism(pointf3x(1,1,1),"maptestnck.tga","envv2");
	m->mod->mats[0].refl = .785f;
	m->mod->mats[0].specpow = 600.0f;
#else
	m = buildprism(pointf3x(1,1,1),(string("CUB_") + skyboxfilelist[curcube]).c_str(),"cubemaptest");
	float itn = .45f;
	m->mod->mats[0].color = pointf3x(itn,itn,itn,1.0f);
#endif
	//popdir();
	m->trans.z = 2.0f;
	m->rotvel.y = .01f;
	roottree->linkchild(m);
#endif
	skyboxtree = buildskybox(pointf3x(10,10,10),skyboxfilelist[curcube],"tex");
	//m = buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
	roottree->linkchild(skyboxtree);

#if 1
// build a test cubemap
	//pushandsetdir("gfxtest");
	m = buildtestsquarecubemap();

	m->trans.z = .5f;
	//m->rot.x = PI*.025f;
	//m->rot.y = PI*.025f;
	//float sv = .9f;
	//m->scale = pointf3x(sv,sv,sv);
	roottree->linkchild(m);
	popdir();
#endif
	textureb::rc.deleterc(mt);
// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.01f;
	mainvp.zback=400;
	static bool moved = false;
	if (!moved) { // set this just once
		mainvp.camtrans.z = -10.0f; // test/temp move camera back some
		moved = true;
	}
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xsrc=WX;
	mainvp.ysrc=WY;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
	mainvp.xres=WX;
	mainvp.yres=WY;
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
}

void cubemaptestproc()
{
	switch(KEY) {
	case 't':
		++curcube;
		if (curcube == ncubemaplist)
			curcube = 0;
		changestate(STATE_CUBEMAPTEST);
		break;
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
		changestate(STATE_CUBEMAPTEST);
		break;
	case '=':
		changeglobalxyres(1);
		changestate(STATE_CUBEMAPTEST);
		break;
	case '-':
		changeglobalxyres(-1);
		changestate(STATE_CUBEMAPTEST);
		break;
	case 'k':
/*		dosky = !dosky;
		if (dosky)
			skyboxtree->mod->mats[0].msflags = SMAT_ISSKY|SMAT_HASWBUFF;
		else
			skyboxtree->mod->mats[0].msflags &= ~(SMAT_ISSKY|SMAT_HASWBUFF); */
		skyboxtree->mod->mats[0].msflags ^= SMAT_ISSKY|SMAT_HASWBUFF;
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_CUBEMAPTEST);
	}
	//mainvp.xres=WX;
	//mainvp.yres=WY;
	//mainvp.xstart=0;
	//mainvp.ystart=0;
	doflycam(&mainvp);
	roottree->proc();
}

void cubemaptestdraw3d()
{
//	video_beginscene(); // clear zbuf etc.
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);
//	video_endscene(); // nothing right now
}

void cubemaptestexit()
{
#ifdef USEGLOBALENV
	free_envv2tex();
#endif
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
}
