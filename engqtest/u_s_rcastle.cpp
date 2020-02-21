///// castle //////////////////////////////////////

#define D2_3D

#include <m_eng.h>//the engine
#include "u_states.h"// main include file for whole engine
#include "u_modelutil.h" // helps build 3d models
#include "u_platonic.h"
#include "u_trackpath.h"
#include "d2_font.h" // font queue


// these vars are static, ie private, global and permanent to this file.
static U32 togvidmode; // toggles vid mode with center mouse button
//hierarchy
static tree2* roottree; // for 3d tree2 (a class) is most imp data struct in the engine
static tree2* sky;
static tree2* ground;
static tree2* origin;
static tree2* castle;
static tree2* newcastle;
static tree2* fp58x10;
static tree2* fp58x10a;
static tree2* fp38x10;
static tree2* fp38x10a;
static tree2* floor1;
static tree2* floor2;
static tree2* wp58x10;
static tree2* wp58x10a;
static tree2* wp58x10b;
static tree2* wp24x10;
static tree2* wp24x10a;
static tree2* wp13x10;
static tree2* wp13x10a;
static tree2* wp16x10;
static tree2* wp16x10a;
static tree2* wp17x10;
static tree2* wp17x10a;
static tree2* wp17x10b;
static tree2* wp17x10c;
static tree2* iwp3p5x10;
static tree2* iwp3p5x10a;
static tree2* iwp3x3;
static tree2* inner_wall;
static tree2* inner_walla;
static tree2* inner_wallb;
static tree2* inner_wallc;
static tree2* inner_walld;
static tree2* inner_walle;
static tree2* inner_wallf;
static tree2* inner_wallg;
static tree2* inner_wallh;
static tree2* inner_walli;
static tree2* inner_wallj;
static tree2* inner_wallk;
static tree2* inner_walll;
static tree2* inner_wallm;
static tree2* inner_walln;
static tree2* inner_wallo;

static tree2* walls1;
static tree2* wall1;
static tree2* wall2;
static tree2* wall3;
static tree2* wall4;
static tree2* roof;
static tree2* wp11x3;
static tree2* wp4x10a;
static tree2* wp4x10b;
static tree2* myfloor;
static tree2* magicball;
static tree2* mycube;
static tree2* mycone;
//polygons
static tree2* ring;
static tree2* digon;
static tree2* trigon;
static tree2* tetragon;
static tree2* pentagon;
static tree2* hexagon;
static tree2* heptagon;
static tree2* octagon;
static tree2* octacontagon;

static tree2* c;
static tree2* mycylinder;
static tree2* mytorus;
static tree2* me,*me2;
static tree2* castle2;
// staircase
static tree2* staircase;
static tree2* staircase_side;
static tree2* staircase_back;
static tree2* sp1x3;
static tree2* sp2x3;
static tree2* sp3x3;
static tree2* sp4x3;
static tree2* sp5x3;
static tree2* sp6x3;
static tree2* sp7x3;
static tree2* sp8x3;
static tree2* sp9x3;
static tree2* sp10x3;
static tree2* sp3x10x3;
static tree2* sp1x1x0;
static tree2* sp1x2x0;
static tree2* sp1x3x0;
static tree2* sp1x4x0;
static tree2* sp1x5x0;
static tree2* sp1x6x0;
static tree2* sp1x7x0;
static tree2* sp1x8x0;
static tree2* sp1x9x0;
static tree2* sp1x10x0;
static tree2* sp3x10x0;
static tree2* sp3x10x0back;
// jack
static tree2* xaxis_sphere;
static tree2* yaxis_sphere;
static tree2* zaxis_sphere;
static tree2* xaxis;
static tree2* yaxis;
static tree2* zaxis;
static tree2* jack;
// old door
static tree2* door;
static tree2* door_frame;
static tree2* door_frame_tree;

// sound section
static backsndplayer* abacksndplayer;
// animation section, could be an animation class
static float curtime; // in seconds
static float timespeed; // 1 for 1 curtime unit in seconds, 2 for faster, -1 backwards, 0 stop etc.
static float maxtime = 78; // in seconds, before loops
static S32 curpath;

static float uv0y, uv1y, transy, transz, rotx;

// helper functions
///reflects environment (skybox only) onto sphere
static tree2* buildenvironmentmapmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("env");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("skybox");
		buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(1.0f),"CUB_space.jpg","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
	return r;
}

/// some path functions
typedef void pathfunc(float t,pointf3& pos,pointf3& rot);
//  lissajous curve, look forward
static void pathforwardlookfront(float t,pointf3& pos,pointf3& rot)
{
	const float size = 4;
	float ang = TWOPI*t/maxtime;
	pos.x = size*sinf(4*ang);
	pos.y = 0;
	pos.z = size*sinf(5*ang);
	pointf3 del; // direction of motion
	del.x = size*4*cosf(4*ang); // derivative
	del.y = 0;
	del.z = size*5*cosf(5*ang);	// derivative
	getlookat(&zerov,&del,&rot); // look forward
}

//  lissajous curve, look at mycube
static void pathforwardlookat(float t,pointf3& pos,pointf3& rot)
{
	const float size = 20;
	float ang = TWOPI*t/maxtime;
	pos.x = size*sinf(4*ang);
	pos.y = 0;
	pos.z = size*sinf(5*ang);
	getlookat(&pos,&mycube->trans,&rot); // look at mycube
}

//  lissajous curve, look at mycube, when far away, get altitude 'y'
static void pathforwardlookatflyup(float t,pointf3& pos,pointf3& rot)
{
	const float size = 20;
	float ang = TWOPI*t/maxtime;
	pos.x = size*sinf(4*ang);
	pos.z = size*sinf(5*ang);
	pos.y = .03f*(pos.x*pos.x + pos.z*pos.z); // parabolic arcs
	getlookat(&pos,&mycube->trans,&rot); // look at mycube
}

//  don't move
static void nopath(float t,pointf3& pos,pointf3& rot)
{
	pos = pointf3x(0,0,0);
	rot = pointf3x(0,0,0);
}

//  super simple rotate in place
static void onerevpath(float t,pointf3& pos,pointf3& rot)
{
    pos = pointf3x(30,5,-30);
	rot = pointf3x(0,t*TWOPI/maxtime,0);
}

// rotate at any speed in place, function object
class rotator_fo {
	float rotspeed;
public:
	rotator_fo(float rotspeeda) : rotspeed(rotspeeda) {}
	void operator()(float t,pointf3& pos,pointf3& rot)
	{
	    pos = pointf3x(30,5,-30);
		rot = pointf3x(0,t*rotspeed*TWOPI/maxtime,0);
	}
};

static rotator_fo rotfo1(1); // rotate 1 per maxtime
static rotator_fo rotfo2(10); // rotate 10 per maxtime

// simple tests
static PIECES pieces1[] = {
	PAUSE,
	STRAIGHT,
	PAUSE,
	STRAIGHT,
};
static trackpath_fo trackpathfo1(pointf3x(30,5,6),PI/2.0f,pieces1,NUMELEMENTS(pieces1),maxtime); // test init rot to the right

// use ROT_LEFT and ROT_RIGHT
static PIECES pieces2[] = {
	PAUSE,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_LEFT,
	STRAIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	ROT_LEFT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_RIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_LEFT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	ROT_LEFT,
	ROT_LEFT,
	PAUSE,
};
static trackpath_fo trackpathfo2(pointf3x(30,5,-30),0,pieces2,NUMELEMENTS(pieces2),maxtime,12); // 12 is default piece size

// use TURN_LEFT and TURN_RIGHT
static PIECES pieces3[] = {
	STRAIGHT,
	TURN_LEFT,
	STRAIGHT,
	TURN_RIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	TURN_RIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	TURN_RIGHT,
	STRAIGHT,
	STRAIGHT,
	STRAIGHT,
	TURN_RIGHT,
	STRAIGHT,
	TURN_LEFT,
	TURN_LEFT,
	TURN_RIGHT,
	TURN_RIGHT,
	TURN_RIGHT,
};
static trackpath_fo trackpathfo3(pointf3x(30,5,-12),0,pieces3,NUMELEMENTS(pieces3),maxtime);

// wrap into functions
static void rotator1(float t,pointf3& pos,pointf3& rot)
{
	rotfo1(t,pos,rot);
}

static void rotator2(float t,pointf3& pos,pointf3& rot)
{
	rotfo2(t,pos,rot);
}

static void trackpath1(float t,pointf3& pos,pointf3& rot)
{
	trackpathfo1(t,pos,rot);
}

static void trackpath2(float t,pointf3& pos,pointf3& rot)
{
	trackpathfo2(t,pos,rot);
}

static void trackpath3(float t,pointf3& pos,pointf3& rot)
{
	trackpathfo3(t,pos,rot);
}

static void mypath(float t,pointf3& pos,pointf3& rot)
{
    float inc = .125; // .1 is not exact, use powers of two in denominators

//    if (t)
  //      rot.y-=PI/512;

    if (t<2){
        pos.x=30;
        pos.y=5;
        pos.z=-30;
    }

    if (t>=2 && t<12 && pos.z<6){
        pos.z+=inc;
    }
    if (pos.z==6 && pos.x==30)
        rot.y = -PI/2;

    if (t>=12 && t<19 && pos.x>6){
        pos.x-=inc;
//        if(rot.y != TWOPI)
  //          rot.y+=PI/64; //
    }
    if (pos.z==6 && pos.x==6)
        rot.y = TWOPI;

    if (t>=19 && t<32 && pos.z<54){
        pos.z+=inc;
    }
    if (pos.z==54 && pos.x==6)
        rot.y = PI/2;

    if (t>=32 && t<45 && pos.x<54){
        pos.x+=inc;
    }
    if (pos.z==54 && pos.x==54)
        rot.y = PI;

    if (t>=45 && t<59 && pos.z>6){
        pos.z-=inc;
    }
    if (pos.z==6 && pos.x==54)
        rot.y = -PI/2;

    if (t>=59 && t<66 && pos.x>30){
        pos.x-=inc;
    }
    if (t>=66 && t<67 && pos.z==6 && pos.x==30) // similar to previous so had to add addditional time tests
        rot.y = PI;

    if (t>=66 && t<77 && pos.z>-30){
        pos.z-=inc;
    }
    if (pos.z==-30 && pos.x==30)
        rot.y = TWOPI;
/*
    float inc = .125; // .1 is not exact us
    if (pos.z <= 12)
        pos.z += inc; // speed only goes so slow
	else
        rot = pointf3x(0,-PI,0);
    if (pos.z > 12 && pos.z < 25)
        pos.z += inc; // speed only goes so slow
//    else if (pos.y <= 6)
//        pos.y += .1;
//    if (pos.z >= 25){
//        getlookat(&pos,&octacontagon->trans,&rot); // from, to, rpy (roll,pitch, yaw)
//        pos.y = .001;
//    }

//    if ((pos.z >= 25) && pos.y <= 15)
//        pos.x =
//        pos.y += inc;
//        pos.z -= .1;

//    else
//        rot.y -= 100;


//    if(door is not fully open)
//        open it a degree until it is
//    if (pos.y > 15)
//        ; // rot = pointf3x(0,0,0)
*/
}


static pathfunc* pathfuncs[] = {
	pathforwardlookfront,
	pathforwardlookat,
	pathforwardlookatflyup,
	nopath,
	mypath,
	onerevpath, // simple rotator
	rotator1, // function object rotator
	rotator2,
	trackpath1,
	trackpath2,
	trackpath3,
};
static const int npathfunc = NUMELEMENTS(pathfuncs);


static struct menuvar edv[] = {
	{"@cyan@----- adjust parameters of castle --------------",NULL,D_VOID,0},
	//{"wininfo.defaultfps",&wi::defaultfps,D_FLOAT|D_RDONLY},
	{"wininfo.framestep",&wininfo.framestep,D_FLOAT|D_RDONLY},
	//{"specpowval",&specpowval,D_FLOAT,FLOATUP/4},
	{"curtime",&curtime,D_FLOAT,FLOATUP/4},
	{"timespeed",&timespeed,D_FLOAT,FLOATUP/4},
	{"mainvp.useattachcam",&mainvp.useattachcam,D_INT,1},
};
static const int nedv = NUMELEMENTS(edv);

static pointf3 ambcolsave,dircolsave;


void rcastleinit() // build state before it runs
{
// sound
	//pointf3 pos,rot;
	//trackpath(3.14f,zerov,zerov);
	pathfunc* path1 = mypath;
	pathfunc* path2 = mypath;

	pushandsetdir("audio/backsnd");
	scriptdir* sc=new scriptdir(0);
	sc->sort();
	abacksndplayer=new backsndplayer(*sc);
	delete sc;
	popdir();
// for path functions
	curtime = 0; // start at time 0
	timespeed = 1; // 1 second per 1 second
	curpath = 10; // array element number, one less than path # on screen


	//specpowval = 50; // 500

	extradebvars(edv,nedv);

	dolights();
	//lightinfo.dodefaultlights=true; // was true. optional in this state. no change if false

	video_setupwindow(GX,GY); // resolution of window of and on main menu

	pushandsetdir("gfxtest"); // default dir is data. new dir is this one. only need to access textures, not shaders

	const C8* shadernameflat = "spotlight"; //  "pdiff" "omni" spotlight
	const C8* shadernameround = "pdiffspec"; // "pdiffspec"


//	mt_setseed(24);

/// build hierarchy
    roottree=new tree2("roottree");
    roottree->trans=pointf3x(0,0,0);

    castle=new tree2("castle");
    castle->setvis(false);
    roottree->linkchild(castle);

    newcastle=new tree2("newcastle");
    newcastle->setvis(true);
    roottree->linkchild(newcastle);

	// CAMERA attached to spot light tree, with a flashlight type model
	me2 = buildspotlight_xy(PI*.5f,1.5,2,"cvert"); // white ghostly model of cone shaped spotlight//(PI*.5f,1.5,2,"cvert"
	me2->name = "me2";
	me2->treedissolvecutoff = .001f;
	me2->trans = pointf3x(0,0,-.1f); // get rid of sparkly circle on spotlight
	me = new tree2("me");
    me->setvis(true);
	me->linkchild(me2);

	tree2* flashlight = buildcylinder_xz(pointf3x(.5f,1,.5f),"maptestnck.tga","omni"/*, pointf3x(0,0,0) , pointf3x(0,0,0), pointf3x(0,0,0)*/); // why omni ???
	flashlight->name = "flashlight"; //

	tree2* headlamp = buildsphere(.5f, "white.pcx", shadernameround);
	headlamp->name = "headlamp";
    headlamp->treecolor = pointf3x(100,100,100,.5f);
    flashlight->linkchild(headlamp);

	tree2* lightbulb = buildsphere(.05f, "white.pcx", shadernameround);
	lightbulb->name = "lightbulb";
    lightbulb->treecolor = pointf3x(100,100,100);
    headlamp->linkchild(lightbulb);

    headlamp->trans=pointf3x(0,1.f,0); // from 1 to 1.1 got rid of the serated junction of headlamp and cylinder
	flashlight->rot.x = PI*.5f;         // but created an undesirable white circle
	flashlight->trans.z = -1.f; // why this exact number? has to be some value or undesirable headlamp visible.
	me->linkchild(flashlight);
    flashlight->setvis(true);

	jack = new tree2("jack");
	xaxis = buildprism(pointf3x(.01f,.01f,2), "white.pcx", "shadernameround");
	xaxis->name = "xaxis";
    xaxis->treecolor = pointf3x(1,0,0);

	yaxis = buildprism(pointf3x(.01f,.01f,2), "white.pcx", "shadernameround");
	yaxis->name = "yaxis";
    yaxis->treecolor = pointf3x(0,1,0);

	zaxis = buildprism(pointf3x(.01f,.01f,2), "white.pcx", "shadernameround");
	zaxis->name = "zaxis";
    zaxis->rot = pointf3x(0,0,0); // working but not
    zaxis->treecolor = pointf3x(0,0,1);


	xaxis_sphere = buildsphere(.05f, "white.pcx", "shadernameround");
	xaxis_sphere->name = "xaxis_sphere";
    xaxis_sphere->trans = pointf3x(0,0,1);
    xaxis_sphere->treecolor = pointf3x(1,0,0);

    yaxis_sphere = buildsphere(.05f, "white.pcx", "shadernameround");
	yaxis_sphere->name = "yaxis_sphere";
    yaxis_sphere->trans = pointf3x(0,0,-1);
    yaxis_sphere->treecolor = pointf3x(0,1,0);

    zaxis_sphere = buildsphere(.05f, "white.pcx", "shadernameround");
	zaxis_sphere->name = "zaxis_sphere";
    zaxis_sphere->trans = pointf3x(0,0,1);
    zaxis_sphere->treecolor = pointf3x(0,0,1);

	xaxis->linkchild(xaxis_sphere);
	yaxis->linkchild(yaxis_sphere);
	zaxis->linkchild(zaxis_sphere);

    xaxis->rot = pointf3x(0,PI/2,0);
    yaxis->rot = pointf3x(PI/2,0,0);

	jack->linkchild(xaxis);
	jack->linkchild(yaxis);
	jack->linkchild(zaxis);

	roottree->linkchild(jack);


	mainvp.camattach = me;

	// LIGHTS attached to tree
	// ambient
	tree2* amb = new tree2("amb");
	amb->flags |= TF_ISAMBLIGHT|TF_ISLIGHT;
	amb->lightcolor = F32DARKGRAY;//F32WHITE;
	roottree->linkchild(amb);
	addlighttolist(amb);
	// directional spotlight
//	dir->flags |= TF_ISLIGHT;
	tree2* dir = new tree2("dir");
	dir->flags |= TF_ISLIGHT;
	dir->lightcolor = F32GREEN;
	me->linkchild(dir);
	addlighttolist(dir);


    pushandsetdir("skybox");
    sky=buildskybox(pointf3x(1,1,1), "space.jpg","tex");
    sky->name="sky";
    popdir();
    roottree->linkchild(sky);

    ground=buildplane_xy(pointf2x(100.,100.),"grass.jpg", shadernameflat); // buildplane_xy_uv
    ground->setvis(true);
    ground->name="ground";
    ground->rot=pointf3x(PI/2,0,0);
    ground->trans=pointf3x(30.0f,-.01f,30.0f);
    roottree->linkchild(ground);

/// origin marker
    origin=buildsphere(.05f,"white.pcx","tex");
    roottree->linkchild(origin);

/// old castle room
    wall1=new tree2("wall1");
    wall1->name="wall1";
    wp4x10a=buildprism(pointf3x(1,7,4.f),"stone_wall.jpg",shadernameflat);
    wp4x10a->name="wp4x10a";
    wall1->linkchild(wp4x10a);
    wp4x10a->trans=pointf3x(0,0,0);

    wp4x10b=buildprism(pointf3x(1,7,4.f),"stone_wall.jpg",shadernameflat);
    wp4x10b->name="wp4x10b";
    wall1->linkchild(wp4x10b);
    wp4x10b->trans=pointf3x(0,0,-7.);

    wp11x3=buildprism(pointf3x(1,3,11),"stone_wall.jpg",shadernameflat);
    wp11x3->name="wp11x3";
    wall1->linkchild(wp11x3);
    wp11x3->trans=pointf3x(0,5,-3.5f);

    wall1->trans=pointf3x(0.f-5.5f,0,0+4); //0.f-5.5f,0,0+4 an offset to center/rotate castle on the world origin

    wall2=wall1->newdup(); // newdup at previous trans
    wall2->name="wall2"; // works, but is undesirable. name s/b the orig name of duplicate.
    wall2->rot=pointf3x(0,PI/2,0);
    wall2->trans=pointf3x(9.5f-5.5f,0,1.5f+4);

    wall3=wall1->newdup();
    wall3->name="wall3";
    wall3->trans=pointf3x(11.f-5.5f,0,-1.f+4);

    wall4=wall1->newdup();
    wall4->name="wall4";
    wall4->rot=pointf3x(0,-PI/2,0); // originally +PI and
    wall4->trans=pointf3x(8.5f-12.5f,0.f,-9.5f+4); // pointf3x(8.5f-5.5f,0.f,-9.5f+4)

    roof=buildprism(pointf3x(1,10,10),"stone_wall.jpg",shadernameflat);
    roof->name="roof";
    roof->rot=pointf3x(0,0,PI/2);
    roof->trans=pointf3x(5.5f-5.5f,4.f,-4.f+4); // translate object to here

    myfloor=buildprism(pointf3x(1,10,10),"stone_wall.jpg",shadernameflat);
    myfloor->name="myfloor";
    myfloor->rot=pointf3x(0,0,PI/2);
    myfloor->trans=pointf3x(5.5-5.5f,-3.f,-4.f+4); // translate object to here


/// NEW CASTLE
/// FLOORS
    floor1=new tree2("floor1");
    uv uv0=uvx(0,0); // origin of texture
    uv uv1=uvx(11.6f,2); // number of tiles u by v
    fp58x10=buildprismuvs(pointf3x(58,10,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    fp58x10->name="fp58x10";
    floor1->linkchild(fp58x10);
    fp58x10->rot=pointf3x(PI/2,0,0);
    fp58x10->trans=pointf3x(30,0,6);

    fp58x10a=fp58x10->newdup();
    floor1->linkchild(fp58x10a);
    fp58x10a->trans=pointf3x(30,0,54);

    uv0=uvx(0,0); // 0,0 is the origin of texture, the upper left. 1,1 is the lower right.
    uv1=uvx(7.6f,2);
    fp38x10=buildprismuvs(pointf3x(38,10,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    fp38x10->name="fp38x10";
    floor1->linkchild(fp38x10);
    fp38x10->rot=pointf3x(PI/2,PI/2,0);
    fp38x10->trans=pointf3x(6,0,30);

    fp38x10a=fp38x10->newdup();
    floor1->linkchild(fp38x10a);
    fp38x10a->trans=pointf3x(54,0,30);

    newcastle->linkchild(floor1);

    floor2=floor1->newdup();
    floor2->setvis(false);
    floor2->trans=pointf3x(0,10,0);
    newcastle->linkchild(floor2);

/// WALLS
    walls1=new tree2("walls1");

    uv0=uvx(0,0); // origin of texture
    uv1=uvx(5.8f,1); // number of tiles u by v
    wp58x10=buildprismuvs(pointf3x(58,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp58x10->name="wp58x10";
    walls1->linkchild(wp58x10);
    wp58x10->rot=pointf3x(0,PI/2,0);
    wp58x10->trans=pointf3x(.5f,5,30);

    wp58x10a=wp58x10->newdup();
    wp58x10a->name="wp58x10a";
    walls1->linkchild(wp58x10a);
    wp58x10a->rot=pointf3x(0,PI/2,0);
    wp58x10a->trans=pointf3x(59.5f,5,30);

    wp58x10b=wp58x10->newdup();
    wp58x10b->name="wp58x10b";
    walls1->linkchild(wp58x10b);
    wp58x10b->rot=pointf3x(0,0,0);
    wp58x10b->trans=pointf3x(30,5,59.5f);

/// front entrance outer wall
    uv0=uvx(0,0); // origin of texture
    uv1=uvx(2.4f,1); // number of tiles u by v
    wp24x10=buildprismuvs(pointf3x(24,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp24x10->name="wp24x10";
    walls1->linkchild(wp24x10);
    wp24x10->rot=pointf3x(0,0,0);
    wp24x10->trans=pointf3x(13.f,5,.5f);

    wp24x10a=wp24x10->newdup();
    wp24x10a->name="wp24x10a";
    walls1->linkchild(wp24x10a);
    wp24x10a->rot=pointf3x(0,0,0);
    wp24x10a->trans=pointf3x(47.f,5,.5f);
/// front entrance courtyard wall
    uv0=uvx(0,0); // origin of texture
    uv1=uvx(1.3f,1); // number of tiles u by v
    wp13x10=buildprismuvs(pointf3x(13,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp13x10->name="wp13x10";
    walls1->linkchild(wp13x10);
    wp13x10->rot=pointf3x(0,0,0);
    wp13x10->trans=pointf3x(18.5f,5,11.5f);

    wp13x10a=wp13x10->newdup();
    wp13x10a->name="wp13x10a";
    walls1->linkchild(wp13x10a);
    wp13x10a->rot=pointf3x(0,0,0);
    wp13x10a->trans=pointf3x(41.5f,5,11.5f);
/// the 3 other courtyard walls
    uv0=uvx(0,0); // origin of texture
    uv1=uvx(1.6f,1); // number of tiles u by v
    wp16x10=buildprismuvs(pointf3x(16,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp16x10->name="wp16x10";
    walls1->linkchild(wp16x10);
    wp16x10->rot=pointf3x(0,0,0);
    wp16x10->trans=pointf3x(20.f,5,48.5f);

    wp16x10a=wp16x10->newdup();
    wp16x10a->name="wp16x10a";
    walls1->linkchild(wp16x10a);
    wp16x10a->rot=pointf3x(0,0,0);
    wp16x10a->trans=pointf3x(40.f,5,48.5f);

    uv0=uvx(0,0); // origin of texture
    uv1=uvx(1.7f,1); // number of tiles u by v
    wp17x10=buildprismuvs(pointf3x(17,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp17x10->name="wp17x10";
    walls1->linkchild(wp17x10);
    wp17x10->rot=pointf3x(0,PI/2,0);
    wp17x10->trans=pointf3x(11.5f,5,19.5f);

    wp17x10a=wp17x10->newdup();
    wp17x10a->name="wp17x10a";
    walls1->linkchild(wp17x10a);
    wp17x10a->rot=pointf3x(0,PI/2,0);
    wp17x10a->trans=pointf3x(11.5f,5,40.5f);

    wp17x10b=wp17x10->newdup();
    wp17x10b->name="wp17x10b";
    walls1->linkchild(wp17x10b);
    wp17x10b->rot=pointf3x(0,PI/2,0);
    wp17x10b->trans=pointf3x(48.5f,5,19.5f);

    wp17x10c=wp17x10->newdup();
    wp17x10c->name="wp17x10c";
    walls1->linkchild(wp17x10c);
    wp17x10c->rot=pointf3x(0,PI/2,0);
    wp17x10c->trans=pointf3x(48.5f,5,40.5f);

/// inner wall with doorway
    inner_wall=new tree2("inner_wall");
    inner_wall->name="inner_wall";

    uv0=uvx(0,0); // origin of texture
    uv1=uvx(.35f,1);
    iwp3p5x10=buildprismuvs(pointf3x(3.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1); // 3.5f,10,1
    iwp3p5x10->name="iwp3p5x10";
    inner_wall->linkchild(iwp3p5x10);
//    iwp3p5x10->rot=pointf3x(0,PI/2,0);
    iwp3p5x10->trans=pointf3x(0,16.f,0); // .5f,16.f,1.75f

    uv0=uvx(.65f,0); // origin of texture
    uv1=uvx(1,1);
    iwp3p5x10a=buildprismuvs(pointf3x(3.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    iwp3p5x10a->name="iwp3p5x10a";
    inner_wall->linkchild(iwp3p5x10a);
//    iwp3p5x10a->rot=pointf3x(0,PI/2,0);
    iwp3p5x10a->trans=pointf3x(6.5f,16.f,0); // .5f,16.f,8.25f

    uv0=uvx(.35f,0); // origin of texture
    uv1=uvx(.65f,.3f);
    iwp3x3=buildprismuvs(pointf3x(3,3,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    iwp3x3->name="iwp3x3";
    inner_wall->linkchild(iwp3x3);
    iwp3x3->trans=pointf3x(3.25f,19.5f,0);
//    iwp3x3->rot=pointf3x(0,PI/2,0);

//    inner_wall->rot=pointf3x(0,PI/2,0);
    inner_wall->trans=pointf3x(2.75,-11,11.5);

    inner_walla=inner_wall->newdup();
    inner_walla->name="inner_walla";
    inner_walla->trans=pointf3x(50.75f,-11,11.5f);

    inner_wallb=inner_wall->newdup();
    inner_wallb->name="inner_wallb";
    inner_wallb->trans=pointf3x(50.75f,-11,24.5f);

    inner_wallc=inner_wall->newdup();
    inner_wallc->name="inner_wallc";
    inner_wallc->trans=pointf3x(50.75f,-11,35.5f);

    inner_walld=inner_wall->newdup();
    inner_walld->name="inner_walld";
    inner_walld->trans=pointf3x(50.75f,-11,48.5f);

    inner_walle=inner_wall->newdup();
    inner_walle->name="inner_walle";
    inner_walle->trans=pointf3x(2.75f,-11,24.5f);

    inner_wallf=inner_wall->newdup();
    inner_wallf->name="inner_wallf";
    inner_wallf->trans=pointf3x(2.75f,-11,35.5f);

    inner_wallg=inner_wall->newdup();
    inner_wallg->name="inner_wallg";
    inner_wallg->trans=pointf3x(2.75f,-11,48.5f);

    inner_wallh=inner_wall->newdup();
    inner_wallh->name="inner_wallh";
    inner_wallh->rot=pointf3x(0,PI/2,0); // bug ??? newdup needs rot to work and 0,0,0 is original rot ???
    inner_wallh->trans=pointf3x(11.5f,-11,57.25f);


    inner_walli=inner_wall->newdup(); // these 3 wont display ???
    inner_walli->name="inner_walli";
    inner_walli->rot=pointf3x(0,PI/2,0);
    inner_walli->trans=pointf3x(24.5f,-11,57.25f);

    inner_wallj=inner_wall->newdup();
    inner_wallj->name="inner_wallj";
    inner_wallj->rot=pointf3x(0,PI/2,0);
    inner_wallj->trans=pointf3x(35.5f,-11,57.25f);

    inner_wallk=inner_wall->newdup();
    inner_wallk->name="inner_wallk";
    inner_wallk->rot=pointf3x(0,PI/2,0);
    inner_wallk->trans=pointf3x(48.5f,-11,57.25f);

    inner_walll=inner_wall->newdup();
    inner_walll->name="inner_walll";
    inner_walll->rot=pointf3x(0,PI/2,0);
    inner_walll->trans=pointf3x(24.5f,-11,9.25f);

    inner_wallm=inner_wall->newdup();
    inner_wallm->name="inner_wallm";
    inner_wallm->rot=pointf3x(0,PI/2,0);
    inner_wallm->trans=pointf3x(35.5f,-11,9.25f);

    inner_walln=inner_wall->newdup();
    inner_walln->name="inner_walln";
    inner_walln->rot=pointf3x(0,PI/2,0);
    inner_walln->trans=pointf3x(48.5f,-11,9.25f);

    inner_wallo=inner_wall->newdup();
    inner_wallo->name="inner_wallo";
    inner_wallo->rot=pointf3x(0,PI/2,0);
    inner_wallo->trans=pointf3x(11.5f,-11,9.25f);

///turrets

    walls1->linkchild(inner_wall);

    walls1->linkchild(inner_walla);
    walls1->linkchild(inner_wallb);
    walls1->linkchild(inner_wallc);
    walls1->linkchild(inner_walld);
    walls1->linkchild(inner_walle);
    walls1->linkchild(inner_wallf);
    walls1->linkchild(inner_wallg);
    walls1->linkchild(inner_wallh);
    walls1->linkchild(inner_walli);
    walls1->linkchild(inner_wallj);
    walls1->linkchild(inner_wallk);
    walls1->linkchild(inner_walll);
    walls1->linkchild(inner_wallm);
    walls1->linkchild(inner_walln);
    walls1->linkchild(inner_wallo);

    newcastle->linkchild(walls1);


/// castle objects
	tree2* tt;
    tt = buildenvironmentmapmodel("envmodel","env");
    tt->name = "reflection_sphere";
	roottree->linkchild(tt);
	tt->trans.z = 30;
    tt->transvel.z = -.05f;

    magicball=buildsphere(.5,"white.pcx", shadernameround);
    magicball->name="magicball";
    magicball->treecolor=pointf3x(1,.85f,0);
    magicball->trans=pointf3x(3,0,3);
    magicball->rotvel.x= -.02f;
    magicball->rotvel.y= -.02f;
    magicball->rotvel.z= -.02f;
	magicball->scale = pointf3x(3,2,1);
	//mainvp.camattach = magicball;
	//mainvp.useattachcam = true;

    mycube=buildprism(pointf3x(.5,.5,.5),"white.pcx",shadernameround); // specular on this one
    mycube->name="mycube";
    mycube->treecolor=pointf3x(0,0,1);
    mycube->rot=pointf3x(0,0,PI/3);
    mycube->trans=pointf3x(-3,0,-3);
    mycube->rotvel.x= -.02f;
    mycube->rotvel.y= -.02f;
    mycube->rotvel.z= -.02f;
    mycube->scale = pointf3x(3,3,3);

    ring=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 300, 300);
    ring->name="ring";
    ring->treecolor=pointf3x(1,.85f,0); // gold
    ring->rot=pointf3x(PI/2,0,0);
    ring->trans=pointf3x(0,0,0);
    ring->rotvel.x= -.02f;
    ring->rotvel.y= -.02f;
    ring->rotvel.z= -.02f;
    ring->scale = pointf3x(2,2,2);

    digon=buildtorus_xz(1,.45f,"white.pcx",shadernameround, 2, 10);
    digon->name="digon";
    digon->treecolor=pointf3x(1,.85f,0); // gold
    digon->rot=pointf3x(PI/2,0,0);
    digon->trans=pointf3x(0,0,0);
    digon->rotvel.x= -.03f;
    digon->rotvel.y= -.03f;
    digon->rotvel.z= -.03f;
    digon->scale = pointf3x(1,1,1);

    trigon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 3, 300);
    trigon->name="trigon";
    trigon->treecolor=pointf3x(1,.85f,0); // gold
    trigon->rot=pointf3x(PI/2,0,0);
    trigon->trans=pointf3x(0,0,0);
    trigon->rotvel.x= -.02f;
    trigon->rotvel.y= -.02f;
    trigon->rotvel.z= -.02f;
    trigon->scale = pointf3x(3,3,3);

    tetragon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 4, 300);
    tetragon->name="tetragon";
    tetragon->treecolor=pointf3x(1,.85f,0); // gold
    tetragon->rot=pointf3x(PI/2,0,0);
    tetragon->trans=pointf3x(3,0,3);
    tetragon->rotvel.x= -.02f;
    tetragon->rotvel.y= -.02f;
    tetragon->rotvel.z= -.02f;
    tetragon->scale = pointf3x(1,1,1);

    pentagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 5, 300);
    pentagon->name="pentagon";
    pentagon->treecolor=pointf3x(1,.85f,0); // gold
    pentagon->rot=pointf3x(PI/2,0,0);
    pentagon->trans=pointf3x(-3,0,-3);
    pentagon->rotvel.x= -.02f;
    pentagon->rotvel.y= -.02f;
    pentagon->rotvel.z= -.02f;
    pentagon->scale = pointf3x(1,1,1);

    hexagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 6, 300);
    hexagon->name="hexagon";
    hexagon->treecolor=pointf3x(1,.85f,0); // gold
    hexagon->rot=pointf3x(PI/2,0,0);
    hexagon->trans=pointf3x(3,0,-3);
    hexagon->rotvel.x= -.02f;
    hexagon->rotvel.y= -.02f;
    hexagon->rotvel.z= -.02f;
    hexagon->scale = pointf3x(1,1,1);

    heptagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 7, 300);
    heptagon->name="heptagon";
    heptagon->treecolor=pointf3x(1,.85f,0); // gold
    heptagon->rot=pointf3x(PI/2,0,0);
    heptagon->trans=pointf3x(-3,0,3);
    heptagon->rotvel.x= -.02f;
    heptagon->rotvel.y= -.02f;
    heptagon->rotvel.z= -.02f;
    heptagon->scale = pointf3x(1,1,1);

    octagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 8, 300);
    octagon->name="octagon";
    octagon->treecolor=pointf3x(1,.85f,0); // gold
    octagon->rot=pointf3x(PI/2,0,0);
    octagon->trans=pointf3x(0,15,0);
    octagon->rotvel.x= -.02f;
    octagon->rotvel.y= -.02f;
    octagon->rotvel.z= -.02f;
    octagon->scale = pointf3x(4,4,4);

    octacontagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 80, 3);
    octacontagon->name="octacontagon";
    octacontagon->treecolor=pointf3x(1,.85f,0); // gold
    octacontagon->rot=pointf3x(PI/2,0,0);
    octacontagon->trans=pointf3x(0,15,0);
//    octacontagon->rotvel.x= -.02f;
//    octacontagon->rotvel.y= -.02f;
//    octacontagon->rotvel.z= -.02f;
    octacontagon->scale = pointf3x(1,1,1);

	c=buildcylinder_xz(pointf3x(.25f,1,.75f),"white.pcx",shadernameround/*, pointf3x(1,0,0), pointf3x(0,1,0), pointf3x(0,0,1)*/);
	c->trans = pointf3x(0,-.5f,0); // center cylinder
	mycylinder = new tree2("cyl_root");
	mycylinder->linkchild(c);
    mycylinder->name="mycylinder";
    mycylinder->rot=pointf3x(0,0,0);
    mycylinder->trans=pointf3x(3,0,-3);
    mycylinder->rotvel.x= -.02f;
    mycylinder->rotvel.y= -.02f;
    mycylinder->rotvel.z= -.02f;
    mycylinder->scale = pointf3x(2,2,2);

    mytorus=buildtorus_xz(.25f,.125f,"white.pcx",shadernameround); // ? size
    mytorus->name="mytorus";
    mytorus->treecolor=pointf3x(1,0,1);
    mytorus->rot=pointf3x(PI/2,0,0);
    mytorus->trans=pointf3x(-3,0,3);
    mytorus->rotvel.x= -.02f;
    mytorus->rotvel.y= -.02f;
    mytorus->rotvel.z= -.02f;
    mytorus->scale = pointf3x(3,3,3);
/*
    mycone=buildcone_xy(PI/2,1.5,2,"cvert"); // C:\Users\HP\Documents\Rick\codeblocks_code-4\codeblocks_code-4\codeblocks_code\engq\engqtest\u_modelutil.h|419|undefined reference to `conexy_surf::operator()(float, float, pointf3*, pointf3*)'|
    mycone->name="mycone";
    mycone->treecolor=pointf3x(1,1,1);
    mycone->rot=pointf3x(PI/2,0,0);
    mycone->trans=pointf3x(-20,0,0);
//    mycone->rotvel.x= -.02f;
  //  mycone->rotvel.y= -.02f;
    //mycone->rotvel.z= -.02f;
    mycone->scale = pointf3x(1,1,1);
*/
/// original staircase steps building process
    staircase=new tree2("staircase");
    staircase->name="staircase";
//    staircase->treecolor = pointf3x(0,0,0);

/// new unfinished automated staircase steps building process

    char stair_i_j[100] = "stair_";
    //int* ipntr;
    //int* jpntr;
    uv0y = .9f;
    uv1y = 1.f;
    transy = .5f;
    transz = 0.f;
    rotx = 0.f;

    for (int i=1; i<11; ++i)
        for (int j=0; j<2; ++j){

            uv0=uvx(0,uv0y);
            uv1=uvx(.3f,uv1y);
            //*ipntr = i;
            //*jpntr = j;
            stair_i_j[11] = i + '0';
            stair_i_j[12] = '_';
            stair_i_j[13] = j + '0';

            if ( j == 0){
               tree2* stair_i_j = buildprismuvs(pointf3x(3,1,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
//                stair_i_j->name = "stair_i_j"; //errors
                staircase->linkchild (stair_i_j);
                stair_i_j->trans = pointf3x(0,transy,transz);
            }
            else{
                tree2* stair_i_j = buildprismuvs(pointf3x(3,1,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
//                stair_i_j->name = "stair_i_j"
               staircase->linkchild (stair_i_j);
                stair_i_j->rot.x = PI/2;
                stair_i_j->trans = pointf3x(0,transy,transz);
            }
            uv0y -= .1f;
            uv1y -= .1f;
            transy = transy+1;
            transz = transz+1;
            rotx -= PI/2;
        }

/// original steps building process
/*
    uv0=uvx(0,.9F);
    uv1=uvx(.3f,1);
    sp1x3=buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x3->name = "sp1x3";
    staircase->linkchild(sp1x3);
    sp1x3->trans = pointf3x(0,.5f,0);
    uv0=uvx(0,.8f);
    uv1=uvx(.3f,.9f);
    sp2x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp2x3->name = "sp2x3";
    staircase->linkchild(sp2x3);
    sp2x3->trans = pointf3x(0,1.5f,1);
    uv0=uvx(0,.7f);
    uv1=uvx(.3f,.8f);
    sp3x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp3x3->name = "sp3x3";
    staircase->linkchild(sp3x3);
    sp3x3->trans = pointf3x(0,2.5f,2);
    uv0=uvx(0,.6f);
    uv1=uvx(.3f,.7f);
    sp4x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp4x3->name = "sp4x3";
    staircase->linkchild(sp4x3);
    sp4x3->trans = pointf3x(0,3.5f,3);
    uv0=uvx(0,.5f);
    uv1=uvx(.3f,.6f);
    sp5x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp5x3->name = "sp5x3";
    staircase->linkchild(sp5x3);
    sp5x3->trans = pointf3x(0,4.5f,4);
    uv0=uvx(0,.4f);
    uv1=uvx(.3f,.5f);
    sp6x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp6x3->name = "sp6x3";
    staircase->linkchild(sp6x3);
    sp6x3->trans = pointf3x(0,5.5f,5);
    uv0=uvx(0,.3f);
    uv1=uvx(.3f,.4f);
    sp7x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp7x3->name = "sp7x3";
    staircase->linkchild(sp7x3);
    sp7x3->trans = pointf3x(0,6.5f,6);
    uv0=uvx(0,.2f);
    uv1=uvx(.3f,.3f);
    sp8x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp8x3->name = "sp8x3";
    staircase->linkchild(sp8x3);
    sp8x3->trans = pointf3x(0,7.5f,7);
    uv0=uvx(0,.1f);
    uv1=uvx(.3f,.2f);
    sp9x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp9x3->name = "sp9x3";
    staircase->linkchild(sp9x3);
    sp9x3->trans = pointf3x(0,8.5f,8);
    uv0=uvx(0,0);
    uv1=uvx(.3f,.1f);
    sp10x3 = buildprismuvs(pointf3x(3,1,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp10x3->name = "sp10x3";
    staircase->linkchild(sp10x3);
    sp10x3->trans = pointf3x(0,9.5f,9);
*/
/// staircase sides
    staircase_side=new tree2("staircase_side");
    staircase_side->name="staircase_side";
    uv0=uvx(0,.9F);
    uv1=uvx(.1f,1);
    sp1x1x0=buildprismuvs(pointf3x(1,1,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x1x0->name = "sp1x1x0";
    staircase_side->linkchild(sp1x1x0);
    sp1x1x0->trans = pointf3x(0,0,0);
    uv0=uvx(.1f,.8f);
    uv1=uvx(.2f,1);
    sp1x2x0 = buildprismuvs(pointf3x(1,2,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x2x0->name = "sp1x2x0";
    staircase_side->linkchild(sp1x2x0);
    sp1x2x0->trans = pointf3x(1,.5f,0);
    uv0=uvx(.2f,.7f);
    uv1=uvx(.3f,1);
    sp1x3x0 = buildprismuvs(pointf3x(1,3,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x3x0->name = "sp1x3x0";
    staircase_side->linkchild(sp1x3x0);
    sp1x3x0->trans = pointf3x(2,1,0);
    uv0=uvx(.3f,.6f);
    uv1=uvx(.4f,1);
    sp1x4x0 = buildprismuvs(pointf3x(1,4,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x4x0->name = "sp1x4x0";
    staircase_side->linkchild(sp1x4x0);
    sp1x4x0->trans = pointf3x(3,1.5f,0);
    uv0=uvx(.4f,.5f);
    uv1=uvx(.5f,1);
    sp1x5x0 = buildprismuvs(pointf3x(1,5,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x5x0->name = "sp1x5x0";
    staircase_side->linkchild(sp1x5x0);
    sp1x5x0->trans = pointf3x(4,2,0);
    uv0=uvx(.5f,.4f);
    uv1=uvx(.6f,1);
    sp1x6x0 = buildprismuvs(pointf3x(1,6,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x6x0->name = "sp1x6x0";
    staircase_side->linkchild(sp1x6x0);
    sp1x6x0->trans = pointf3x(5,2.5f,0);
    uv0=uvx(.6f,.3f);
    uv1=uvx(.7f,1);
    sp1x7x0 = buildprismuvs(pointf3x(1,7,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x7x0->name = "sp1x7x0";
    staircase_side->linkchild(sp1x7x0);
    sp1x7x0->trans = pointf3x(6,3,0);
    uv0=uvx(.7f,.2f);
    uv1=uvx(.8f,1);
    sp1x8x0 = buildprismuvs(pointf3x(1,8,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x8x0->name = "sp1x8x0";
    staircase_side->linkchild(sp1x8x0);
    sp1x8x0->trans = pointf3x(7,3.5f,0);
    uv0=uvx(.8f,.1f);
    uv1=uvx(.9f,1);
    sp1x9x0 = buildprismuvs(pointf3x(1,9,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x9x0->name = "sp1x9x0";
    staircase_side->linkchild(sp1x9x0);
    sp1x9x0->trans = pointf3x(8,4,0);
    uv0=uvx(.9f,0);
    uv1=uvx(1,1);
    sp1x10x0 = buildprismuvs(pointf3x(1,10,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp1x10x0->name = "sp1x10x0";
    staircase_side->linkchild(sp1x10x0);
    sp1x10x0->trans = pointf3x(9,4.5f,0);
    uv0=uvx(0,0);
    uv1=uvx(.3f,1);
    sp3x10x0 = buildprismuvs(pointf3x(3,10,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp3x10x0->name = "sp3x10x0";
    staircase_side->linkchild(sp3x10x0);
    sp3x10x0->trans = pointf3x(11.f,4.5f,0);

/// staircase back
    uv0=uvx(.3f,0);
    uv1=uvx(.6f,1);
    sp3x10x0back = buildprismuvs(pointf3x(3,10,0),"stone_wall.jpg",shadernameflat,uv0,uv1);
    sp3x10x0back->name = "sp3x10x0back";
    sp3x10x0back->rot = pointf3x(0,-PI,0);
    sp3x10x0back->trans = pointf3x(0,5.f,12.5f);
/*
    uv0={0,0};
    uv1={1.f,.1f};
    sp3x10x3 = buildprismuvs(pointf3x(3,10,3),"white.pcx",shadernameround,uv0,uv1);
    sp3x10x3->name = "sp3x10x3";
    staircase->linkchild(sp3x10x3);
    sp3x10x3->trans = pointf3x(10.f,5,0);
*/
//    staircase->rot = pointf3x(0,PI/2,0);

    staircase_side->rot = pointf3x(0,-PI/2,0);
    staircase_side->trans = pointf3x(1.505f,.5f,0); // trying to get rid of z fighting, but edge is visible

    staircase->linkchild(staircase_side);
    staircase->linkchild(sp3x10x0back);
    staircase->rot = pointf3x(0,0,0);
    staircase->trans = pointf3x(30,30,30);

    newcastle->linkchild(staircase);

//    staircase->treecolor = pointf3x(1,.85f,0); // not working ???

/// door
    door = buildprism(pointf3x(1,7,3),"stone_wall.jpg","tex");
    door->name = "door";
    door->trans = pointf3x(.5f,0,5.5f);
    door->rot = pointf3x(0,PI/2,0);
    door->rotvel.y = .01f;
///door frame
	door_frame_tree = new tree2("door_frame_tree");
	door_frame_tree->name = "door_frame_tree";
    door_frame=buildtorus_xz(2.2f,.1f,"white.pcx",shadernameround, 4, 300);
    door_frame->name="door_frame";
    door_frame->scale = pointf3x(1,20.5f,1); // creates a tunnel model
    door_frame->treecolor=pointf3x(1,.85f,0); // gold
    door_frame->rot=pointf3x(PI/4,PI/2,PI/2); // fixed by rick
//    door_frame->rot=pointf3x(0,0,PI/4); // rot z doesnt work with torus
    door_frame->trans=pointf3x(.5f,0,6.1f);
    door_frame_tree->linkchild(door_frame);
    door_frame_tree->scale = pointf3x(1,2.37f,1); // thickness of upper and lower elements increase with y scale

    castle->linkchild(wall1);
    castle->linkchild(wall2);
    castle->linkchild(wall3);
    castle->linkchild(wall4);
    castle->linkchild(roof);
    castle->linkchild(myfloor);

    castle2 = castle->newdup();
    castle2->name = "castle2";
    castle2->linkchild(digon);
    castle2->linkchild(trigon);
    castle2->linkchild(tetragon);
    castle2->linkchild(pentagon);
    castle2->linkchild(hexagon);
    castle2->linkchild(heptagon);
    castle2->linkchild(octagon);
    castle2->linkchild(octacontagon);
//    newcastle->linkchild(staircase);
    castle2->linkchild(door);
    castle2->linkchild(door_frame_tree);

//    castle2->linkchild(mycone);
    roottree->linkchild(castle2);
    castle2->trans=pointf3x(0,0,12); // now load castle2 with its own new models

	roottree->linkchild(me);

    castle->linkchild(magicball);
    castle->linkchild(mycube);
    castle->linkchild(ring);
    castle->linkchild(mycylinder);
    castle->linkchild(mytorus);

	// platonics and test the facesNtovertfaces function, to help make much simpler mesh tables
	tree2* aplatonic;
#if 1
	aplatonic = buildtetrahedron("maptestnck.tga","pdiffspec","a tetrahedron");
	aplatonic->trans = pointf3x(0,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);
#endif
#if 1
	aplatonic = buildhexahedron("maptestnck.tga","pdiffspec","a hexahedron");
	aplatonic->trans = pointf3x(-.5f,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);
#endif
#if 1
	aplatonic = buildoctahedron("maptestnck.tga","pdiffspec","an octahedron");
	aplatonic->trans = pointf3x(-1,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);

	aplatonic = builddodecahedron("maptestnck.tga","pdiffspec","a dodecahedron");
	aplatonic->trans = pointf3x(-1.5f,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);

	aplatonic = buildicosahedron("maptestnck.tga","pdiffspec","an icosahedron");
	aplatonic->trans = pointf3x(-2,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);

	aplatonic = buildtruncatedhexahedron("maptestnck.tga","pdiffspec","a truncatedhexahedron");
	aplatonic->trans = pointf3x(-2.5f,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);

	aplatonic = buildrhombicuboctahedron("maptestnck.tga","pdiffspec","a rhombicuboctahedron");
	aplatonic->trans = pointf3x(-3,.3f,.5f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);
#endif
#if 0 // test calc uvs with a sphere
	aplatonic = buildsphere(1,"maptestnck.tga","pdiffspec");
	aplatonic->trans = pointf3x(0,.3f,1.3f);
	aplatonic->scale = pointf3x(.1f,.1f,.1f);
	roottree->linkchild(aplatonic);
#endif

//    castle->linkchild(jack);

//    castle->rotvel.x=.01; //mt_frand()*.1f;//.005f; // 0 to 1 adjusts speed. + or - for direction.
    //castle->rotvel.y=.01; //mt_frand()*.01f;//.005f;
//    castle->rotvel.z=.01; //mt_frand()*.1f;//.005f;
	//castle->scalevel = pointf3x(1.002,1.002,1.002);
//	roottree->linkchild(me);
// setup viewport
	mainvp.backcolor=C32BLACK;
	mainvp.zfront=0.025f; // .25f clipping plane front and back
	mainvp.zback=1000; // 400
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER; // clearing the background, w buffer, checkered background in software mode
	mainvp.xsrc=WX;//4; // not used much.
	mainvp.ysrc=WY;//3;
	mainvp.useattachcam=true; // can attatch cam to an object anywhere in the hierarchy
	mainvp.isortho=false; // true removes all perspective orthographic vs perspective
	mainvp.ortho_size=30; //

#if 0
	static bool firsttime = true;
	if (firsttime) { // only do once
		mainvp.camtrans = pointf3x(0,.3f,0);
		firsttime = false;
	}
#endif

// keep track of current video device
	togvidmode=videoinfo.video_maindriver; // not important

// lights
	dolights(); // this will set ambient and directional light to default colors on first call only
	// save default lights
	ambcolsave = lightinfo.ambcolor;
	dircolsave = lightinfo.lightcolors[0];
	// change default lights
	//lightinfo.ambcolor = pointf3x(1.f,1.f,1.f); // not working ???
	//lightinfo.lightcolors[0] = pointf3x(.075f,.075f,.075f); // not working ???
// fog
/*    videoinfodx9.fog.enable = true;
    videoinfodx9.fog.mode =
    videoinfodx9.fog.density =
    videoinfodx9.fog.color =
*/



//	lightinfo.deflightpos[0] = pointf3x(0,0,-10);

// sound
	abacksndplayer->settrack(6);

	popdir(); // puts us back in the previous folder
}

void rcastleproc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU); // back to main menu
		break;
	case 'a': // show it
		showcursor(1);
		break;
	case 'b':
		{
			S32 t=abacksndplayer->gettrack();
			if (t<0)
				abacksndplayer->settrack(4);
			else
				abacksndplayer->settrack(t+1);
		}
		break;
	case 'h': // hides it
		showcursor(0);
		break;
	case ' ':
		video3dinfo.favorshading^=1; // if in software mode only, turn off texture and enable lighting
		break;
	case 's': // toggle between hardware and software mode
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_RCASTLE);
		break;
	case 'n': // change path function
		++curpath;
		if (curpath >= npathfunc)
			curpath = 0;
		curtime = 0;
		break;
	case 'p': // change path function
		--curpath;
		if (curpath < 0)
			curpath = npathfunc - 1;
		curtime = 0;
		break;
	//case 'r': // reset time
	//	curtime = 0;
	//	break;
	case '=':
		changeglobalxyres(1);
		changestate(STATE_RCASTLE);
		break;
	case '-':
		changeglobalxyres(-1);
		changestate(STATE_RCASTLE);
		break;
	case 'k':
		mainvp.useattachcam ^= 1;
		break;
	}
	if (wininfo.mmiddleclicks) { // change vid from windod to full screen
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_RCASTLE);
	}
//mainvp.useattachcam = true; // overides the debp variable
	mainvp.xres=WX; // set viewport to accomodate the change above. grow winfo
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	doflycam(&mainvp); // enables arrow keys, and mouse l r buttons, and move mouse-- to move camera
	roottree->proc(); // func proc is a member of struct roottree


	//pathforwardlookfront(curtime,me->trans,me->rot);
	while (curtime >= maxtime) // check first, incase debprint modified these
		curtime -= maxtime;
	while (curtime < 0)
		curtime += maxtime;

	pathfunc* cpf = pathfuncs[curpath];
	cpf(curtime,me->trans,me->rot);
	//me->rot = pointf3x(0,PI/6,0);
// code to adjust orientation of jack as camera moves
//    jack->rot.x = -(me->rot.x);
//    jack->rot.y = -(me->rot.y);
//    jack->rot.z = -(me->rot.z);

	drawtextque_format_foreback(SMALLFONT,0,10,F32WHITE,F32BLACK,"'n' to change to next path, 'p' to change to previous path, curpath %d/%d, curtime = %f",curpath+1,npathfunc,curtime); // 2d print in a 3d world
	drawtextque_format_foreback(SMALLFONT,0,50,F32WHITE,F32BLACK,"'k' to change attachcam"); // 2d print in a 3d world
	drawtextque_format_foreback(SMALLFONT,0,90,F32WHITE,F32BLACK,"'b' to change background track, current track = %d",abacksndplayer->gettrack());
	drawtextque_format_foreback(SMALLFONT,0,130,F32WHITE,F32BLACK,"position of camera = %f, %f, %f, rotation of camera = %f %f %f",me->trans.x,me->trans.y,me->trans.z,me->rot.x,me->rot.y,me->rot.z);

	//timespeed = range(
	curtime += timespeed/*wininfo.framestep*//INITFPS;
}

void rcastledraw3d()
{
	video_buildworldmats(roottree); // generate matrices
	video_setviewport(&mainvp); // clear zbuf etc. apply flags from above
	dolights(); // 2nd occurence of dolights() is not default. 1st is in castleinit()
	video_drawscene(roottree); // draw the roottree
}

void rcastleexit() // free resources when escape or x clicked
{
    mainvp.useattachcam = false;
    mainvp.camattach = 0;
	logger("logging roottree\n");
	lightinfo.ambcolor = ambcolsave;
	// restore default lights
	lightinfo.lightcolors[0] = dircolsave;
	lightinfo.deflightpos[0] = pointf3x();
	extradebvars(0,0); //
	roottree->log2();
	logger("logging reference lists\n");
	logrc(); // log reference counter

	delete roottree; // deallocate all memory
	logger("logging reference lists after free\n");
	logrc();
	delete abacksndplayer;
}

