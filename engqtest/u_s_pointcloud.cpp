// mess around with pointclouds and meshes

// engq API
#include <m_eng.h>
#include <d2_font.h> // font queue

// list of user states
#include "u_states.h"

// for buildprism
#include "u_modelutil.h"
#include "u_platonic.h"

#include "u_pointcloudkdtree.h"
// read .bin2 files
#include "u_pointcloudfile.h"

// play with median
#include "u_median.h"

//#define TESTMEDIAN
//#define TESTMEDIANPOINTS
#define TESTMEDIANSTRING


// 3d objects
static tree2* roottree;
static tree2* obj1,*obj2,*obj3; // test,points, lines, tris
static tree2* obj4; // read in file here
static modelb* obj4mod;

static U32 togvidmode; // 0,1,2,3 gdi,ddraw,dx9win,dx9fs

static bool play;
static U32 playcount; // count of play button presses
static U32 filenum;
static script* filelist;
static pointcloudfile* curpointcloud;

// test points
static pointf3 dpnts[2];
static struct menuvar edv[]={
// user vars
	{"@lightred@---- HELPEROBJ USER VARS -----------------",NULL,D_VOID,0},
	{"playcount",&playcount,D_INT|D_RDONLY},
	{"Point A x",&dpnts[0].x,D_FLOAT,FLOATUP/4},
	{"Point A y",&dpnts[0].y,D_FLOAT,FLOATUP/4},
	{"Point A z",&dpnts[0].z,D_FLOAT,FLOATUP/4},
	{"Point B x",&dpnts[1].x,D_FLOAT,FLOATUP/4},
	{"Point B y",&dpnts[1].y,D_FLOAT,FLOATUP/4},
	{"Point B z",&dpnts[1].z,D_FLOAT,FLOATUP/4},
};
#define NEDV sizeof(edv)/sizeof(edv[0])

// test sample points
static const S32 npointverts = 4;
static S32 curpoints = 0;
static pointf3 pointverts1[npointverts] = {
	{-1, 1,0},
	{ 1, 1,0},
	{-1,-1,0},
	{ 1,-1,0},
};

static pointf3 pointverts2[npointverts+1] = {
	{-1.2f, 1,0},
	{ 1, 1,0},
	{-1,-1,0},
	{ 1,-1,0},

	{ .4f,.4f,0},
};

static pointf3 pointverts3[npointverts+2] = {
	{-1, 1.2f,0},
	{ 1, 1,0},
	{-1,-1,0},
	{ 1,-1,0},

	{ .3f,.2f,0},
	{ .2f,.3f,0},
};

static pointf3 pointverts4[npointverts+3] = {
	{-1.2f, 1.2f,0},
	{ 1, 1,0},
	{-1,-1,0},
	{ 1,-1,0},

	{ .2f,0,0},
	{ .5f,.5f,0},
	{ .5f,0,0},
};
static pointf3* pointset[] = {pointverts1,pointverts2,pointverts3,pointverts4};


static pointf3 pointcverts[npointverts+3] = {
	{1,0,0,1},
	{0,1,0,1},
	{0,0,1,1},
	{1,1,1,1},

	{1,1,0,1},
	{1,0,1,1},
	{0,1,1,1},
};

// test sample lines
static const S32 nlines = 4;
static const S32 nlineverts = 2*nlines; // two points per line
static pointf3 lineverts[nlineverts] = {
	{-1, 1,0},
	{ 1, 1,0},
	{ 1, 1,0},
	{ 1,-1,0},
	{ 1,-1,0},
	{-1,-1,0},
	{-1,-1,0},
	{-1, 1,0},
};
static pointf3 linecverts[nlineverts] = {
	{1,0,0,1},
	{0,1,0,1},
	{0,0,1,1},
	{1,1,1,1},
	{1,0,0,1},
	{0,1,0,1},
	{0,0,1,1},
	{1,1,1,1},
};

// test sample tris
static const S32 ntris = 1;
static const S32 ntriverts = 3*ntris; // two points per line
static S32 curtris = 0;
static pointf3 triverts1[ntriverts] = {
	{-1.05f, 1,0},
	{ 1, 1,0},
	{-1,-1,0},

};
static pointf3 triverts2[ntriverts + 3] = {
	{-1.075f, 1,0},
	{ 1, 1,0},
	{-1,-1,0},

	{ 1,-1,0},
	{-1,-1,0},
	{ 1, 1,0},
};
static pointf3 triverts3[ntriverts + 6] = {
	{-1.1f, 1,0},
	{ 1, 1,0},
	{-1,-1,0},

	{ 1,-1,0},
	{-1,-1,0},
	{ 1, 1,0},

	{1, 1,0},
	{3, 1,0},
	{1,-1,0},
};
static pointf3 triverts4[ntriverts + 9] = {
	{-1, 1,0},
	{ 1, 1,0},
	{-1,-1,0},

	{ 1,-1,0},
	{-1,-1,0},
	{ 1, 1,0},

	{1, 1,0},
	{3, 1,0},
	{1,-1,0},

	{3,-1,0},
	{1,-1,0},
	{3, 1,0},
};
static pointf3* trivertset[] = {triverts1,triverts2,triverts3,triverts4};

static pointf3 tricverts[ntriverts + 9] = {
	{1,0,0,1},
	{0,1,0,1},
	{0,0,1,1},

	{0,1,1,1},
	{1,0,1,1},
	{1,1,0,1},

	{1,0,0,1},
	{0,1,0,1},
	{0,0,1,1},

	{0,1,1,1},
	{1,0,1,1},
	{1,1,0,1},
};

static face trifaces1[] = {
	{{0,1,2},0},
};

static face trifaces2[] = {
	{{0,1,2},0},
	{{3,4,5},0},
};

static face trifaces3[] = {
	{{0,1,2},0},
	{{3,4,5},0},
	{{6,7,8},0},
};

static face trifaces4[] = {
	{{0,3,2},0},
	{{3,0,1},0},
	{{6,7,8},0},
	{{9,10,11},0},
};
static face* trifaceset[] = {trifaces1,trifaces2,trifaces3,trifaces4};

static tree2* buildpoints(const C8* matname,const C8* modname,const pointf3* verts,const pointf3* cverts,S32 nverts)
{
	tree2* t = new tree2("testpoints");
	modelb* m;
	if (modname)
		m = model_create(modname);
	else
		m = model_create(unique());
	if (model_getrc(m) == 1) {
		m->copyverts(verts,nverts);
		m->copycverts(cverts,nverts);
		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASSHADE|SMAT_POINTS,0,0,50,0,nverts);
		//m->addfaces(prismfaces,12,0); // no faces for points
		m->close();
	}
	t->setmodel(m);
	return t;
}

static tree2* buildlines(const C8* matname,const C8* modname,const pointf3* verts,const pointf3* cverts,S32 nverts)
{
	if (nverts % 2)
		errorexit("odd verts for buildlines model '%s'",modname);
	tree2* t = new tree2("testlines");
	modelb* m;
	if (modname)
		m = model_create(modname);
	else
		m = model_create(unique());
	if (model_getrc(m) == 1) {
		m->copyverts(verts,nverts);
		m->copycverts(cverts,nverts);
		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASSHADE|SMAT_LINES,0,0,50,0,nverts);
		//m->addfaces(prismfaces,12,0); // no faces for lines
		m->close();
	}
	t->setmodel(m);
	return t;
}

static tree2* buildtris(const C8* matname,const C8* modname,const pointf3* verts,const pointf3* cverts,S32 nverts)
{
	if (nverts % 3)
		errorexit("verts not a multiple of 3 for buildtris model '%s'",modname);
	tree2* t = new tree2("testtris");
	modelb* m;
	if (modname)
		m = model_create(modname);
	else
		m = model_create(unique());
	if (model_getrc(m) == 1) {
		m->copyverts(verts,nverts);
		m->copycverts(cverts,nverts);
		m->addmat(matname,SMAT_HASWBUFF|SMAT_HASSHADE,0,0,50,nverts/3,nverts);

		m->genindifaces(); // generate faces for tris
		m->close();
	}
	t->setmodel(m);
	return t;
}

static void loadmorepoints()
{
	// update point file
	//pointcloudfile pcf(filelist->idx(filenum++).c_str());
	//if (filenum == filelist->num())
	//	filenum = 0;
	const pointf3* pnts;
	const pointf3* clrs;
	U32 npnts = curpointcloud->getnextframe(&pnts,&clrs);
	obj4mod->copyverts(pnts,npnts,true); // fixgroup verts
	obj4mod->copycverts(clrs,npnts);
	obj4mod->update();
}

static void hilitpoints(bool back = false)
{
	// update point file
	//pointcloudfile pcf(filelist->idx(filenum++).c_str());
	//if (filenum == filelist->num())
	//	filenum = 0;
	//const pointf3* pnts;
	const pointf3* clrs;
	U32 npnts = curpointcloud->getnexthilit(&clrs,back);
	//obj4mod->copyverts(pnts,npnts,true); // fixgroup verts
	obj4mod->copycverts(clrs,npnts);
	obj4mod->update();
}

#ifdef TESTMEDIAN
static void test_median()
{
	logger(" ^v^v^v^v^v start test median ^v^v^v^v^v\n");
	median<S32>::unittest();
	logger(" ^v^v^v^v^v end test median ^v^v^v^v^v\n");
}
#endif

#ifdef TESTMEDIANPOINTS

static bool comppointsx(const pointf3& a,const pointf3& b)
{
	return a.x < b.x;
}

static bool comppointsy(const pointf3& a,const pointf3& b)
{
	return a.y < b.y;
}

static bool testmedianpntsx(const median<pointf3>& mp,const string& title,S32 testnumber = -1) // does a pretty good check of getvalue against slower getsortedvalue for consistancy
{
	U32 i,ne=mp.size();
	for (i=0;i<ne;++i) {
		if (mp.getvalue(i).x != mp.getsortedvalue(i).x)
			break;
	}
	if (i == ne) { // full match
		logger("--- %s %d ---, match !!!\n",title.c_str(),testnumber);
		//return true;
	}
	logger("--- %s %d ---, mismatch\n",title.c_str(),testnumber);
		for (i=0;i<ne;++i) {
		const pointf3& v = mp.getvalue(i);
		const pointf3& sv = mp.getsortedvalue(i);
		const pointf3& rv = mp.getrawvalue(i);
//		if (is_same<T,S32>::value) {
//		}
		logger("\tindex value at %6u = (%8.4f,%8.4f,%8.4f), test sorted value = (%8.4f,%8.4f,%8.4f), raw unsorted value = (%8.4f,%8.4f,%8.4f)\n",
			i,v.x,v.y,v.z,sv.x,sv.y,sv.z,rv.x,rv.y,rv.z);
	}
	return false;
}

static bool testmedianpntsy(const median<pointf3>& mp,const string& title,S32 testnumber = -1) // does a pretty good check of getvalue against slower getsortedvalue for consistancy
{
	U32 i,ne=mp.size();
	for (i=0;i<ne;++i) {
		if (mp.getvalue(i).y != mp.getsortedvalue(i).y)
			break;
	}
	if (i == ne) { // full match
		logger("--- %s %d ---, match !!!\n",title.c_str(),testnumber);
		//return true;
	}
	logger("--- %s %d ---, mismatch\n",title.c_str(),testnumber);
		for (i=0;i<ne;++i) {
		const pointf3& v = mp.getvalue(i);
		const pointf3& sv = mp.getsortedvalue(i);
		const pointf3& rv = mp.getrawvalue(i);
//		if (is_same<T,S32>::value) {
//		}
		logger("\tindex value at %6u = (%8.4f,%8.4f,%8.4f), test sorted value = (%8.4f,%8.4f,%8.4f), raw unsorted value = (%8.4f,%8.4f,%8.4f)\n",
			i,v.x,v.y,v.z,sv.x,sv.y,sv.z,rv.x,rv.y,rv.z);
	}
	return false;
}

static void test_median_points()
{
	logger(" ^v^v^v^v^v start test median points^v^v^v^v^v\n");
	vector<pointf3> testpoints;
	U32 i,n = 2000;
	for (i=0;i<n;++i) {
		pointf3 v;
#if 0
		v.x = mt_frand();
		v.y = mt_frand();
		v.z = mt_frand();
#else
		v.x = mt_random(200)/200.0f;
		v.y = mt_random(200)/200.0f;
		v.z = mt_random(200)/200.0f;

#endif
		testpoints.push_back(v);
	}
	median<pointf3> mx(testpoints,comppointsx);
	testmedianpntsx(mx,"testpointsx",-1);
	median<pointf3> my(testpoints,comppointsy);
	testmedianpntsy(my,"testpointsy",-1);
	logger(" ^v^v^v^v^v end test median points^v^v^v^v^v\n");
}
#endif

#ifdef TESTMEDIANSTRING

static bool testmedianstring(const median<string>& mp,const string& title,S32 testnumber = -1) // does a pretty good check of getvalue against slower getsortedvalue for consistancy
{
	U32 i,ne=mp.size();
	for (i=0;i<ne;++i) {
		if (mp.getvalue(i) != mp.getsortedvalue(i))
			break;
	}
	if (i == ne) { // full match
		logger("--- %s %d ---, match !!!\n",title.c_str(),testnumber);
		//return true;
	}
	logger("--- %s %d ---, mismatch\n",title.c_str(),testnumber);
		for (i=0;i<ne;++i) {
		const string& v = mp.getvalue(i);
		const string& sv = mp.getsortedvalue(i);
		const string& rv = mp.getrawvalue(i);
//		if (is_same<T,S32>::value) {
//		}
		logger("\tindex value at %6u = '%8s', test sorted value = '%8s', raw unsorted value = '%8s'\n",
			i,v.c_str(),sv.c_str(),rv.c_str());
	}
	return false;
}

static void test_median_string()
{
	logger(" ^v^v^v^v^v start test median strings^v^v^v^v^v\n");
	vector<string> teststring;
	teststring.push_back("this");
	teststring.push_back("is");
	teststring.push_back("hi");
	teststring.push_back("is");
	teststring.push_back("ho");
	teststring.push_back("hhum");
	teststring.push_back("is");
	teststring.push_back("a");
	teststring.push_back("test");
	teststring.push_back("testthat");
	teststring.push_back("of");

	median<string> ms(teststring);
	testmedianstring(ms,"teststring",-1);
	vector<string> l,m,r;
	U32 i,j,n = ms.size();
	for (i=0;i<n;++i) {
		ms.splitidx(l,m,r,i);
		logger("---- split idx %d\n",i);
		for (j=0;j<l.size();++j) {
			logger("\t left %3d, %s\n",j,l[j].c_str());
		}
		for (j=0;j<m.size();++j) {
			logger("\t middle %3d, %s\n",j,m[j].c_str());
		}
		for (j=0;j<r.size();++j) {
			logger("\t right %3d, %s\n",j,r[j].c_str());
		}
	}
	logger(" ^v^v^v^v^v end test median points^v^v^v^v^v\n");
}
#endif


////////////////////////// main state init
void pointcloudinit()
{
#ifdef TESTMEDIAN
	test_median();
#endif
#ifdef TESTMEDIANPOINTS
	test_median_points();
#endif
#ifdef TESTMEDIANSTRING
	test_median_string();
#endif
// setup window size
	video_setupwindow(GX,GY);
	togvidmode = videoinfo.video_maindriver;
	logger("XXXXX running point cloud state XXXXX\n");

	play = false;
	playcount = 0;
	filenum = 3;
	filelist = 0;
	curpointcloud = 0;

// read in '.bin2' point cloud data files
	pushandsetdir("meshpointcloud");

	// get a list of those files in the meshpointcloud directory
	// TODO: parse these files and populate point cloud etc.
	filelist = new script();
	scriptdir filedir(false);
	S32 i;
	for (i=0;i<filedir.num();++i) {
		// for now, just get the size of the .bin2 files and log them
		const C8* fname = filedir.idx(i).c_str();
		if (isfileext(fname,"bin2")) {
			filelist->addscript(fname);//pointcloudfile pcf(fname);
		}
	}

// setup realtime debugger
	extradebvars(edv,NEDV);

// setup trees
	// root tree
	//pushandsetdir("gfxtest"); // default graphics directory
	roottree = new tree2("roottree");
	roottree->trans.z = 5; // move world forward to see objects

	// point test
	obj1 = buildpoints("cvert","testpoints",pointverts1,pointcverts,npointverts);
	//obj1->flags |= TF_DONTDRAWC;
	obj1->trans.x = -3;
	obj1->trans.y = 3;
	roottree->linkchild(obj1);

	// line test
	obj2 = buildlines("cvert","testlines",lineverts,linecverts,nlineverts);
	//obj2->flags |= TF_DONTDRAWC;
	obj2->trans.y = 3;
	roottree->linkchild(obj2);

	// tri test
	obj3 = buildtris("cvert","testtris",triverts1,tricverts,ntriverts);
	//obj3->flags |= TF_DONTDRAWC;
	//obj3 = buildsphere(1,"maptestnck.tga","tex");
	obj3->trans.x = 3;
	obj3->trans.y = 3;
	roottree->linkchild(obj3);


	// point file
	obj4 = buildpoints("cvert","testfilepoints",pointverts1,pointcverts,npointverts);
	//obj4->flags |= TF_DONTDRAWC;
	obj4->treecolor = pointf3x(1,1,1,.98f);
	obj4mod = obj4->mod;
	obj4->trans.y = 0;//-1.875;
	//obj4->trans.z = .75;
	obj4->scale = pointf3x(4,4,4,1);
	roottree->linkchild(obj4);
	//vector<pointf3> verts;
	//vector<pointf3> cverts;
	//popdir();
	curpointcloud = new pointcloudfile(filelist->idx(filenum).c_str());

	loadmorepoints();

// setup viewport
	mainvp.xres = WX;
	mainvp.yres = WY;
	mainvp.xstart = 0;
	mainvp.ystart = 0;
	static bool once = true;
		mainvp.backcolor = C32BLUE;
	if (once) {
		mainvp.zfront = .0125f;
		mainvp.zback = 2000;
		mainvp.camzoom = 1;//3.2f; // it'll getit from tree camattach if you have one
		mainvp.camtrans.z = 0;//-100;
		mainvp.camtrans.x = 0;
		mainvp.camtrans.y = 0;// 50;
		mainvp.camrot = pointf3x(0,0,0);
		once = false;
	}
	mainvp.flags = VP_CLEARBG|VP_CHECKER|VP_CLEARWB;
	mainvp.xsrc = 640;
	mainvp.ysrc = 480;
	mainvp.useattachcam = false;

	// init test points
	dpnts[0] = pointf3x(0,0,3);
	dpnts[1] = pointf3x(1,1,4);

	curpoints = 0;
	curtris = 0;
}

void pointcloudproc()
{
// input
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 's': // toggle software/hardware mode
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode ^= 2;
		video_init(togvidmode,0);
		changestate(STATE_MESHPOINTCLOUD);
		break;
	case 'p': // pause / play
		play = !play;
		++playcount;
		break;
	case 'n': // next .bin2 file
		++filenum;
		if ((S32)filenum == filelist->num())
			filenum = 0;
		delete curpointcloud;
		curpointcloud = new pointcloudfile(filelist->idx(filenum).c_str());
		loadmorepoints();
		break;
	case 't': // play around with dynamic verts in point cloud
		++curpoints;
		if (NUMELEMENTS(pointset) == curpoints)
			curpoints = 0;
		obj1->mod->copyverts(pointset[curpoints],npointverts + curpoints,true); // fixgroup verts
		obj1->mod->copycverts(pointcverts,npointverts + curpoints);
		obj1->mod->update();
		break;
	case 'u': // play around with dynamic faces and verts in tri faces
		++curtris;
		if (NUMELEMENTS(trivertset) == curtris)
			curtris = 0;
		obj3->mod->copyverts(trivertset[curtris],ntriverts + curtris*3,true); // fixgroup verts
		obj3->mod->copycverts(tricverts,ntriverts + curtris*3);
		obj3->mod->copyfaces(trifaceset[curtris],curtris + 1,true); // fixgroup faces
		obj3->mod->update();
		break;
	case 'h':
		hilitpoints();
		break;
	case 'j':
		hilitpoints(true);
		break;
	}
// proc scene
	if (play) {
		loadmorepoints();
	}
	roottree->proc();
	doflycam(&mainvp);
	drawtextque_format_foreback(SMALLFONT,8,8,F32WHITE,F32BLACK,"loadfilename = %s",filelist->idx(filenum).c_str());
}

void pointclouddraw3d()
{
// draw
	video_buildworldmats(roottree); // help dolights
	dolights();
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
}

void pointcloudexit()
{
	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree; // free the main man
	popdir();
	delete filelist;
	filelist = 0;
	delete curpointcloud;
	curpointcloud = 0;
}
