/// CASTLE //////////////////////////////////////

#define D2_3D

#include <m_eng.h>//the engine
#include "u_states.h"// main include file for whole engine
#include "u_modelutil.h" // helps build 3d models
#include "d2_font.h" // font queue
#include <d3d9.h> // put this before next to fix
#include "m_vid_dx9.h"
#include "u_platonic.h"
#include "u_trackpath.h"

/// MOVIE EDITOR
#include <vector>

// tritest1
//#include "u_s_toon.h"
#define USESHADOWMAP
// arena1
#define VIEWSHADOWMAP
//static bool test;
static tree2* roottree;
using namespace std;
///


//tritest1
static C8 shadename[50]="pnormalmap"; // "useshadowmap";
static tree2* normalmaptree; // is *normalmaptree in tritest1. the * gives error here
// misc
static U32 togvidmode; // toggles vid mode with center mouse button
/// castle hierarchy
//static tree2* roottree; // declared above now
static tree2* sky;
static tree2* ground;
static tree2* origin;
static tree2* castle; // old
static tree2* castle2;
static tree2* oldcastles;
static tree2* newcastle;
// floors
static tree2* fp10x10;
static tree2* fp60x10;
static tree2* fp60x10a;
static tree2* fp40x10;
static tree2* fp40x10a;
static tree2* floor1; //
static tree2* floor2;
static tree2* tfp4x10;
static tree2* tfp4x10a;
static tree2* tfp2x4;
static tree2* tfp2x4a;
static tree2* tower_floor; // original floor with no trap door
static tree2* tower_floor1; // floor with trap door;
static tree2* floors;
// walls
static tree2* wp60x10;
static tree2* wp60x10a;
static tree2* wp60x10b;
static tree2* wp25x10;
static tree2* wp25x10a;
static tree2* wp14x10;
static tree2* wp14x10a;
static tree2* wp17x10;
static tree2* wp17x10a;
static tree2* wp18x10;
static tree2* wp18x10a;
static tree2* wp18x10b;
static tree2* wp18x10c;
static tree2* twp3p5x10;
static tree2* twp3p5x10a;
static tree2* twp3x3;
static tree2* iwp3p5x9p5;
static tree2* iwp3p5x9p5a;
static tree2* iwp3x2p5;
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
// towers
static tree2* alltowers;
static tree2* towersection1; // this configuration has 2 door walls and 2 window walls
static tree2* towersection2; // this configuration has 2 door walls and 2 solid walls
static tree2* towersection3; // this configuration has 4 window walls
static tree2* towersection4; // this configuration has 4 solid walls
static tree2* towersection5; // NYI this configuration has 4 door walls and could be used elsewhere
static tree2* tower0;
static tree2* tower0a;
static tree2* tower0b;
static tree2* tower1;
static tree2* tower2;
static tree2* tower3;
static tree2* tower_door_wall;
static tree2* tower_door_walla;
static tree2* tower_door_wallb;
static tree2* tower_door_wallc;
static tree2* tower_window_wall;
static tree2* tower_window_walla;
static tree2* tower_window_wallb;
static tree2* tower_window_wallc;
static tree2* twp3x4;
static tree2* twp3x4a;
static tree2* twp10x10;
static tree2* twp10x10a;
static tree2* twp10x10b;
static tree2* twp10x10c;

static tree2* twp2p5x10;
static tree2* twp2p5x10a;
static tree2* twp2p5x10b;
static tree2* twp1p25x4;
static tree2* twp1p25x4a;
static tree2* twp1p25x3;
static tree2* twp1p25x3a;
static tree2* tower_corners;
static tree2* castle_corners;
static tree2* turret_corners;
static tree2* tower_pyramid;
static tree2* tower_pyramid1;
static tree2* tower_pyramid2;
static tree2* tower_pyramid3;

// newdups of above
static tree2* tower_cornersx1;
static tree2* tower_cornersx2;
static tree2* tower_cornersx3;
static tree2* tower_door_wallx1;
static tree2* tower_door_wallax1;
static tree2* twp10x10copyx1;
static tree2* twp10x10acopyx1;
static tree2* tower_floor1x1;
static tree2* tower_floor1x2;
static tree2* tower_floor1x3;
static tree2* tower_turretsx1; // NU not used
static tree2* tower_window_wallx1;
static tree2* tower_window_wallax1;
static tree2* towersection3a;
// trim
static tree2* trim;
static tree2* trim4stone;
static tree2* trim4gold;
static tree2* trim4golda;
static tree2* trim4goldb;
static tree2* trim15;
static tree2* trim15a;
static tree2* trim15b;
static tree2* trim25;
static tree2* trim25a;
static tree2* trim25b;
static tree2* trim25c;
static tree2* trim25d;
static tree2* trim38;
static tree2* trim38a;
static tree2* trim38b;
static tree2* trim38c;

// old castle
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

/// objects

static tree2* objects;
// windows
static tree2* tower_windows;
static tree2* window;
static tree2* window1;
static tree2* window_panes;
static tree2* windowpane_xaxis;
static tree2* windowpane_yaxis;
// tritest1
static tree2* tt;
// wall lamps
static tree2* wall_lamp;
static tree2* wall_lamp_cone;
// multi-material
static tree2* multimat;
// spheres
static tree2* magicball;
// cubes
static tree2* mycube;
// pyramid
static tree2* pyramid;
// cones
static tree2* mycone;

// plutonic solids
static tree2* tetrahedron;
static tree2* octahedron;
// polygons made from torus
static tree2* digon;
static tree2* digon1;
static tree2* trigon;
static tree2* tetragon;
static tree2* pentagon;
static tree2* hexagon;
static tree2* heptagon;
static tree2* octagon;
static tree2* octacontagon;
// cylinders
static tree2* c;
static tree2* mycylinder;
// torus
static tree2* mytorus;
static tree2* ring;
// my view point with flashlight
static tree2* me,*me2;

// staircase
static tree2* stairs;
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
static tree2* staircase_side;
static tree2* sp3x10x0back;
static tree2* sp3x3x0landing;
static tree2* staircase;
static tree2* staircase1;
static tree2* staircases;
//old stairs
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
// jack
static tree2* xaxis_sphere;
static tree2* yaxis_sphere;
static tree2* zaxis_sphere;
static tree2* xaxis;
static tree2* yaxis;
static tree2* zaxis;
static tree2* jack;
// doors
static tree2* doors;
static tree2* door3x7;
static tree2* door3x7a;
static tree2* door4x10;
static tree2* door4x10a;
static tree2* door4x10b;
static tree2* door4x10c;
static tree2* door5x10;
static tree2* door5x10a;
static tree2* door5x10b;
static tree2* door5x10c;
static tree2* hinged_door;
static tree2* door_frame;
static tree2* door_frame_tree;
// turrets
static tree2* turrets;
static tree2* turrets40;
static tree2* turrets40a;
static tree2* turrets40b;
static tree2* turrets40c;
static tree2* turrets10;
static tree2* turrets10a;
static tree2* turrets10b;
static tree2* turrets10c;
static tree2* turret_low;
static tree2* turret_high;
static tree2* tower_turrets;
// misc
static tree2* many;
static tree2* bunkbeddup;

/// sound section
static backsndplayer* abacksndplayer;
/// animation section, could be an animation class
static float curtime; // in seconds
static float timespeed; // 1 for 1 curtime unit in seconds, 2 for faster, -1 backwards, 0 stop etc.
static float maxtime = 78; // in seconds, before loops // 78
static S32 curpath;

/// misc section
//static float uv0y, uv1y, transy, transz, rotx; // BAD !!! SHOULD BE LOCAL VARS IN STAIRS

/// helper functions

// tritest1
static tree2* buildnormalmapmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("stm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","243-normal.jpg",matname);
//	    buildpatch_tan(modb,24,12,18,9,spheref_surf_tan(5.0f),"stones.jpg","normal.jpg",matname);
	    buildpatch_tan(modb,24,12,18,9,spheref_surf_tan(5.0f),"stones.jpg","243-normal.jpg",matname);
//	    buildpatch_tan(modb,24,12,18,9,spheref_surf_tan(5.0f),"stones.jpg","ts18_2crop.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptestnck.tga","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
//	r->treecolor=pointf3x(1,1,0);
//	r->flags|=TF_TREECOLOR;
	r->flags|=TF_CASTSHADOW;
	return r;
}

// arena1
static tree2* buildgroundmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("ground");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
	    buildpatch(modb,20,20,20,20,planexz_surf(200,200),"grass.jpg",matname);
	}
	r->setmodel(modb);
	return r;
}


// arena1
#ifdef VIEWSHADOWMAP
static tree2* buildshadowmapviewermodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("sbm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
	    buildpatch(modb,1,1,1,1,planexz_surf(10.0f,10.0f),"shadowmap",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
//	r->flags|=TF_CASTSHADOW;
	return r;
}
#endif

/*
// tritest1 used in arena 1 above. identical except for shadow flag is not set
static tree2* buildshadowmapviewermodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("sbm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
	    buildpatch(modb,1,1,1,1,planexz_surf(10.0f,10.0f),"shadowmap",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
	r->flags|=TF_CASTSHADOW;
	return r;
}
*/
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
typedef void pathfunc(float t,pointf3& pos,pointf3& rot); // ricks new path function code has this up here
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

/* // now in trackpath.c and .h
enum PIECES {
	PAUSE,
	STRAIGHT,
	ROT_RIGHT,
	ROT_LEFT,
	TURN_RIGHT, // NYI // Not Implemented Yet
	RIGHT = TURN_RIGHT,
	TURN_LEFT, // NYI
	LEFT = TURN_LEFT,
};
///
class trackpath_fo {
	static const float piecesize;
	pointf3* basepos;
	float* baserot;
	const PIECES* const pieceset;
	U32 numpieceset;
///
public:
	trackpath_fo(const pointf3& startpos,float startrot,const PIECES* const pces,U32 numpces) : // calcs the pos and rot of each piece
	  pieceset(pces),
	  numpieceset(numpces)
	{
		if (!numpieceset)
			errorexit("track size must be > 0 !!");
		// calculate the start pos and rot for each piece
		basepos = new pointf3[numpieceset];
		baserot = new float[numpieceset];
		pointf3 curpos = startpos;
		float currot = startrot;
		for (U32 i=0;i<numpieceset;++i) {
			basepos[i] = curpos;
			baserot[i] = currot;
			switch(pieceset[i]) {
			case PAUSE:
				break;
			case STRAIGHT:
			curpos.x += piecesize*sinf(currot);
			curpos.z += piecesize*cosf(currot);
				break;
			case ROT_RIGHT:
				currot += PI/2.0f;
				currot = normalangrad(currot);
				break;
			case ROT_LEFT:
				currot -= PI/2.0f;
				currot = normalangrad(currot);
				break;
			case TURN_RIGHT:
				break;
			case TURN_LEFT:
				break;
			}
		}

	}
	void operator()(float t,pointf3& pos,pointf3& rot)
	{
		t *= numpieceset/maxtime; // now t is 0 to numpieceset, 1 unit for each piece
		float idxf;
		float tfract = modf(t,&idxf); // split t into fraction and integer parts, tfract 0 to 1, where on a piece
		U32 idx = U32(idxf); // piece index
		const pointf3& bp = basepos[idx];
		float br = baserot[idx];
		float poff = tfract*piecesize; // where on piece we are, timelike
		switch(pieceset[idx]) {
			case PAUSE:
				pos = bp;
				rot = pointf3x(0,br,0);
				break;
			case STRAIGHT:
				pos.x = bp.x + poff*sinf(br);
				pos.y = bp.y;
				pos.z = bp.z + poff*cosf(br);
				rot = pointf3x(0,br,0);
				break;
			case ROT_RIGHT:
				pos = bp;
				br += tfract*PI/2.0f;
				rot = pointf3x(0,br,0);
				break;
			case ROT_LEFT:
				pos = bp;
				br -= tfract*PI/2.0f;
				rot = pointf3x(0,br,0);
				break;
			case TURN_RIGHT:
				pos = bp;
				rot = pointf3x(0,br,0);
				break;
			case TURN_LEFT:
				pos = bp;
				rot = pointf3x(0,br,0);
				break;
		}
	}
	~trackpath_fo()
	{
		ERASE[] basepos;
		ERASE[] baserot;
	}
};
*/
//const float trackpath_fo::piecesize = 12.0f; // distance between walls // COMPILER ERROR RIGHT ON THIS LINE error: ld returned 1 exit status
//  const float trackpath_fo::piecesize = 12.0f; // ??? same code above as rcastle.cpp code here ???
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
static PIECES pieces2[] = { // ricks new path
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
static trackpath_fo trackpathfo2(pointf3x(30,5,-31.5f),0,pieces2,NUMELEMENTS(pieces2),maxtime,12.5f);

// use TURN_LEFT and TURN_RIGHT // NYI
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
static trackpath_fo trackpathfo3(pointf3x(31,5,-12),0,pieces3,NUMELEMENTS(pieces3),maxtime);

/// my trackpath
static PIECES pieces4[] = {
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
	ROT_RIGHT,
    ROT_RIGHT,
};                                                  // tried adding and subtracting 1/2 piece size ie 6.25 from this z value
static trackpath_fo trackpathfo4(pointf3x(31,5,-31.5f),0,pieces4,NUMELEMENTS(pieces4),maxtime,12.5f); // 30,5,-24

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

static void my_trackpath(float t,pointf3& pos,pointf3& rot)
{
	trackpathfo4(t,pos,rot);
}

static void mypath(float t,pointf3& pos,pointf3& rot)
{
    float inc = .125; // .1 is not exact, use powers of two in denominators

//    if (t)
  //      rot.y-=PI/512;

    if (t<2){
        pos.x=30;
        pos.y=5.f;
        pos.z=-30;
        rot.x=PI/8; // PI/8
    }

    if (t>=2 && t<12 && pos.z<6){ //  && t<12
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

}



/// ricks many.cpp
#define ALTTEX
#define ALTCOLOR
#define DOROTVEL
#define DOSPEC
static vector<tree2*> masters; // ricks many
static vector<tree2*> masters1; // my many
static float specpowval = 500;//500;

static pointf3 ambcolsave,dircolsave;

/// ricks many.cpp
static C8* rndtex[]={
    "white.pcx"
/*	"clinton.pcx",
	"bark.tga",
	"rengst.jpg",
	"stones.jpg",
	"gamesel.pcx"*/
};
#define NRNDTEX sizeof(rndtex)/sizeof(rndtex[0])

/// MOVIE EDITOR
// NOTES:
#define RES3D // alternate shape class, also declared in match_pickserver.cpp
#include <l_misclibm.h> // for the l_res3d.c and .h files

static FILE *fpin;
static FILE *fpout;

struct capture_path { /// the most basic unit, camera motions per frame
    pointf3 cp_camtrans;
    pointf3 cp_camrot;
};
capture_path cp;

vector<capture_path>::iterator itA;
vector<capture_path>::iterator itB;

/// the two vectors, A and B
static vector<capture_path> vectorA; // type is name of struct not its instance
float vectorA_i = 0.f; // probably dont need the dot f

static vector<capture_path> vectorB; ////
float vectorB_i = 0.f;

vector<capture_path>* curvector = &vectorA;; // default initializations
float* curvectori = &vectorA_i;
vector<capture_path>* sourcevector = &vectorB; // default usually from B, but could be reversed

static float speed; // for playslider

/// the funtionalities of the UI ie the states
static bool record;
static bool frev; // NLU
static bool rev; // NLU
static bool play; // NLU ????
static bool fwd; // NLU
static bool ffwd; // NLU
// no bool stop needed
static bool vectorAorB;
static bool clearvector; // NLU
static bool load; // NLU
static bool save; // NLU
static bool insertinto;
static bool erasefrom;
static bool selectcount;
static S32 insertframe; // these 3 should be U32
static S32 eraseframe1;
static S32 eraseframe2;

/// the shapes handles,  in the UI
static shape* rl;
static shape* focus;

static text* TEXTSELECTFILE;
static listbox* SELECTFILE;
static text* TEXTSAVEFILENAME;
static edit* SAVEFILENAME;

static pbut* REC; //

static pbut* FREV; // NLU
static pbut* REV; // NLU
static pbut* PLAY;
static pbut* FWD; // NLU
static pbut* FFWD; // NLU
static pbut* STOP;

static hscroll* PLAYSLIDER; // the slider
static text* TEXTPLAYSLIDER;

static hscroll* FRAMESLIDER; // the slider
static text* TEXTFRAMESLIDER;

static pbut* VECTOR;
static pbut* CLEAR;
static pbut* LOAD;
static pbut* SAVE;
static pbut* INSERT;
static pbut* ERASE;
static pbut* FRAMESELECT;
static pbut* QUIT; // the quit button

#define SPTRESX 640 // 640 set virtual screensize of the sprite system. set higher muliples to decrease the UI size in castle.
#define SPTRESY 480// 480
/*
/// I/O
static const char* tempconstcharptr; // from gettname()
static char tempcharptr [100]; // to concat the extension ".me"
static const char* savefilename; // to fopen()
static const char* loadfilename; // to fread()
*/

// set position text and 3d object position depending on the position slider
static void defaultFrametext() // NLU ???>
{
	C8 str[50]; // should use 'string' class instead
	S32 rmval = FRAMESLIDER->getidx();
//	sprintf(str,"IDX = %d",rmval);
	float xpos = rmval; // rmval * .1f;
	sprintf(str,"frame number = %6.0f",xpos); // string defaults to upper case // %6.3f
	TEXTFRAMESLIDER->settname(str); // change name each frame
}

static void updatePlaySlidertext()
{
	C8 playspeed[50]; // should use 'string' class instead
	S32 playval = PLAYSLIDER->getidx();
//	sprintf(str,"IDX = %d",rmval);
	float playpos = playval * .1f; // rmval * .1f; // this allows tenths of playpos
	sprintf(playspeed,"PLAYSPEED = %6.1f",playpos); // string defaults to upper case // %6.3f
	TEXTPLAYSLIDER->settname(playspeed); // change name each frame
}

///
static void updateFrameSlidertext()
{
	C8 str[50]; // should use 'string' class instead

    if(curvector->size() != 0){
        sprintf(str,"frame number %6.0f of %6d",*curvectori+1, curvector->size()); // string defaults to upper case // %6.3f
        TEXTFRAMESLIDER->settname(str); // change name each frame
    }
    else{
        sprintf(str,"frame number 0 of 0"); // string defaults to upper case // %6.3f
        TEXTFRAMESLIDER->settname(str); // change name each frame
        FRAMESLIDER->setidx(0);
    }
}
/// MOVIE EDITOR in debprint
static struct menuvar edv[] = { // a static array of type struct menuvar called edv[]
	{"@cyan@----- MOVIE EDITOR --------------",NULL,D_VOID,0},
//	{"@cyan@----- adjust parameters of castle --------------",NULL,D_VOID,0},
	//{"wininfo.defaultfps",&wi::defaultfps,D_FLOAT|D_RDONLY},
	{"wininfo.framestep",&wininfo.framestep,D_FLOAT|D_RDONLY},
	//{"specpowval",&specpowval,D_FLOAT,FLOATUP/4},
	{"curtime",&curtime,D_FLOAT,FLOATUP/4},
	{"timespeed",&timespeed,D_FLOAT,FLOATUP/4},
	{"mainvp.useattachcam",&mainvp.useattachcam,D_INT,1},
	{"RECORD",&record,D_INT,1},
	{"PLAY",&play,D_INT,1},
	{"FRAME",&vectorA_i,D_FLOAT,1},
};
static const int nedv = NUMELEMENTS(edv);
///
enum test1 {IDLE,PLAYIT,RECORD,EDIT}; // enums can not be static. wow, certain names give errors. test works
test1 UIstates = IDLE; // enum must be initialized or error

void proc_play(void){
//float speed = PLAYSLIDER->getidx() * .1f;

//curvector->
//*curvectori

/// IF PLAY IS PAUSED
    if (PLAYSLIDER->getidx() == 0){ // if playslider is moved to the middle. // && vectorA_i == 0 ){
        UIstates = IDLE;
        return;
    }
/// IF SOME LEFT TO PLAY
    if(*curvectori < curvector->size()){
        mainvp.camtrans = curvector->at(*curvectori).cp_camtrans; // curvector-> // vectorA. // -> and dot syntax used together ???
        mainvp.camrot = curvector->at(*curvectori).cp_camrot;
        speed = PLAYSLIDER->getidx() * .1f; // the only local variable. now a static global
        *curvectori += speed; // ???
        *curvectori = range(0.0f, *curvectori, float(curvector->size()-1)); // range(min,num,max) if num<min ret min, if num>max ret max
//            FRAMESLIDER->setminmaxval(1,vectorA.size()); // why is this here ???
        updateFrameSlidertext(); //
        FRAMESLIDER->setidx(*curvectori+1); //
        updatePlaySlidertext(); // does this need to be here ???
    }
/// IF NONE LEFT TO PLAY
    if((*curvectori+1 == curvector->size())  || (*curvectori == 0)){ // && PLAYSLIDER->getidx() < 0)){
        PLAYSLIDER->setidx(0);
        updatePlaySlidertext();
        updateFrameSlidertext();
        FRAMESLIDER->setidx(*curvectori+1);
        UIstates = IDLE;
    }
}

void proc_record(void){
    cp.cp_camtrans = mainvp.camtrans;
    cp.cp_camrot = mainvp.camrot;
    curvector->push_back(cp);
    FRAMESLIDER->setminmaxval(1,curvector->size());
    FRAMESLIDER->setidx(*curvectori+1);
    updateFrameSlidertext();


}

void proc_edit(void){
/*
    if(curvector = &vectorA)
        sourcevector = &vectorB;
    else
        sourcevector = &vectorA;

    if(insertinto){
        itA = curvector->begin();
        insertframe = FRAMESLIDER->getidx();
        curvector->insert(itA+insertframe, sourcevector->begin(), sourcevector->end()); // itA+insertframe-1
        FRAMESLIDER->setminmaxval(1,curvector->size());
        updateFrameSlidertext();
        FRAMESLIDER->setidx(insertframe);
        insertinto = 0;
        UIstates = IDLE;
    }

    if(erasefrom){
//        itA = curvector->begin();
        curvector->erase(curvector->begin()+eraseframe1-1, curvector->begin()+eraseframe2);
        selectcount = 0;
//        *curvectori = eraseframe1-2;
        FRAMESLIDER->setminmaxval(1,curvector->size());
        updateFrameSlidertext();
        FRAMESLIDER->setidx(eraseframe1-1);
        erasefrom = 0;
        UIstates = IDLE;
    }
}
*/

    if(!vectorAorB){ // A
        if(insertinto){
            itA = vectorA.begin();
            insertframe = FRAMESLIDER->getidx();
            vectorA.insert(itA+insertframe, vectorB.begin(), vectorB.end()); // itA+insertframe-1
            FRAMESLIDER->setminmaxval(1,curvector->size());
            updateFrameSlidertext();
            FRAMESLIDER->setidx(insertframe);
            insertinto = 0;
            UIstates = IDLE;
        }

        if(erasefrom){

            itA = vectorA.begin();
            vectorA.erase(vectorA.begin()+eraseframe1-1, vectorA.begin()+eraseframe2);
            selectcount = 0;
            erasefrom = 0;
            vectorA_i = eraseframe1-2;
//            vectorA_i = 0;
            FRAMESLIDER->setminmaxval(1,curvector->size());
            updateFrameSlidertext();
            FRAMESLIDER->setidx(eraseframe1-1);
//            PLAYSLIDER->setidx(0);
            UIstates = IDLE;
        }
    }
    else{ // B

        if(insertinto){ // N/A on vectorB to vectorA
//            itB = vectorB.begin();
//            insertframe = FRAMESLIDER->getidx();
//            vectorB.insert(itB+insertframe-1, vectorA.begin(), vectorA.end());
            insertinto = 0;
            UIstates = IDLE;
        }

        if(erasefrom){

            itB = vectorB.begin();
            vectorB.erase(vectorB.begin()+eraseframe1-1, vectorB.begin()+eraseframe2);
            selectcount = 0;
            erasefrom = 0;
            vectorB_i = eraseframe1-2;
//                vectorB_i = 0;
            FRAMESLIDER->setminmaxval(1,curvector->size());
            updateFrameSlidertext();
            FRAMESLIDER->setidx(eraseframe1-1);
//            PLAYSLIDER->setidx(0);
            UIstates = IDLE;
        }

    }
}

/// END OF MOVIE EDITOR

static void UIpath(float t,pointf3& pos,pointf3& rot) // NEEDS FINISHING
{
        mainvp.camtrans = vectorA.at(vectorA_i) .cp_camtrans;
        mainvp.camrot = vectorA.at(vectorA_i).cp_camrot;
//        ++vectorA_i;
}
//typedef void pathfunc(float t,pointf3& pos,pointf3& rot); // mine is down here
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
	my_trackpath,
	UIpath,
};
static const int npathfunc = NUMELEMENTS(pathfuncs); //
/// SOUND
    wavehandle* twh;
/// FORTPOINT
    tree2* fortpoint;
//    logger ("fortpoint ptr = \n");
//    logger ("fortpoint ptr = %p\n", 0);
//    logger ("BWS file exists = %d\n", fileexist("fp7opt.BWS"));
    tree2* bwofile;
	tree2* bunkbeddup;
void castleinit() // build state before it runs
{
/// SOUND
    pushandsetdir("racenetdata/Tarpits");
    logger ("fileexist = %d\n", fileexist("mammothTrumpet"));
    twh = wave_load("mammothTrumpet.ogg",0);
    logger ("mammothTrumpet = %p\n", twh);
    popdir();
//    playasound(twh);

/// MOVIE EDITOR
	pushandsetdir("1mydata");
// testres3d stuff
	logger("testres3d init\n");
	video_setupwindow(GX,GY); // G is Global
    roottree=new tree2("roottree"); // WHY ON EARTH IS THIS HERE ????
    roottree->trans=pointf3x(0,0,0);
	// build ui
	init_res3d(SPTRESX,SPTRESY); // values from above
	rl = res_loadfile("movie_editorUI.txt");
	focus = 0; // think it meant focus when hovering ???

/// the shape's handles,  in the UI

    /// select file list box
	TEXTSELECTFILE=rl->find<text>("TEXTSELECTFILE"); // text above
	SELECTFILE=rl->find<listbox>("SELECTFILE"); // now populate it
        /// load listbox
	scriptdir sd(0); // get directory of files. // true if you want directories, false if you want files ???
	for (int i=0;i<sd.num();++i) {
		const C8* str = sd.idx(i).c_str();
		if (isfileext(str,"me")) // filter by file extension
			SELECTFILE->addstring(str);
	}
/*	// mock up more files
	for (int i=0;i<20;++i) {
		char str[500];
		sprintf(str,"file %2d.cp",i);
		if (isfileext(str,"cp")) // filter
			SELECTFILE->addstring(str);
	}*/
	SELECTFILE->setidxc(0); // set default highlight of 1st file
	const C8* loadfilename = SELECTFILE->getidxname(); // display current highlighted file on text bar AND save it to loadfilename
	if (loadfilename){
		TEXTSELECTFILE->settname(loadfilename);
	}

    /// save file name
	TEXTSAVEFILENAME=rl->find<text>("TEXTSAVEFILENAME");
	SAVEFILENAME=rl->find<edit>("SAVEFILENAME");

    /// PLAY AND RECORD function buttons
	REC =rl->find<pbut>("REC");

//	FREV=rl->find<pbut>("FREV");
//	REV=rl->find<pbut>("REV");
	PLAY=rl->find<pbut>("PLAY");
//	FWD=rl->find<pbut>("FWD");
//	FFWD=rl->find<pbut>("FFWD");

    TEXTPLAYSLIDER=rl->find<text>("TEXTPLAYSLIDER");
	PLAYSLIDER=rl->find<hscroll>("PLAYSLIDER"); // horizontal slider
	PLAYSLIDER->setminmaxval(-300,300); // range of // (-40,40)
	PLAYSLIDER->setidx(0); // starting point of // (18)


	STOP=rl->find<pbut>("STOP");

    /// frame number slider
	TEXTFRAMESLIDER=rl->find<text>("TEXTFRAMESLIDER"); // text above slider
	FRAMESLIDER=rl->find<hscroll>("FRAMESLIDER"); // horizontal slider
	FRAMESLIDER->setminmaxval(0,0); // range of slider // set once in init, then set elsewhere
	FRAMESLIDER->setidx(1); // starting point of // (18)
	defaultFrametext(); // of slider

    /// select vector A or B button
	VECTOR =rl->find<pbut>("VECTOR");

    /// clear vector button
	CLEAR =rl->find<pbut>("CLEAR");

    /// load button
	LOAD =rl->find<pbut>("LOAD");

    /// save button
	SAVE =rl->find<pbut>("SAVE");

    /// insert button
	INSERT =rl->find<pbut>("INSERT");

    /// erase button
	ERASE =rl->find<pbut>("ERASE");

    /// frame select button
	FRAMESELECT =rl->find<pbut>("FRAMESELECT");

    /// cancel  button
	QUIT=rl->find<pbut>("QUIT"); // find is ricks method in the shape class ??? ie find the active children of shape named rl.

    popdir(); // from pushandsetdir("1mydata") above
/*
    /// code to load a vector from file
    pushandsetdir("1mydata");
    if((fpout = fopen("hurray1", "rb")) != NULL){ // if file exists
        while (fread(&cp, sizeof(capture_path), 1, fpout)){
            vectorA.push_back(cp);
        }
        fclose(fpout);
    }
    popdir();
    updatePlaySlidertext();
*/
/// END OF MOVIE EDITOR INIT

/// tritest1
	userfloata=.995f;

/// ricks many.cpp
	specpowval = 500;

/// ricks new path function code // ???
	pathfunc* path1 = mypath;
	pathfunc* path2 = mypath;

/// sound
	pushandsetdir("audio/backsnd");
	scriptdir* sc=new scriptdir(0); // scriptdir* sc=new scriptdir(0);
	sc->sort();
	abacksndplayer=new backsndplayer(*sc);
	delete sc;
	popdir();

/// path functions
	curtime = 0;	timespeed = 1; // 1 second per 1 second
	curpath = 3; // array element number, one less than path # on screen

/// debprint
	extradebvars(edv,nedv);

/// video
	video_setupwindow(GX,GY); // resolution of window of and on main menu

/// shaders
	pushandsetdir("gfxtest"); // default dir is data. new dir is this one. only need to access textures, not shaders
	const C8* shadernameflat = "spotlight"; //  "pdiff" "omni" spotlight
	const C8* shadernameround = "pdiffspec"; // "pdiffspec"
//	popdir(); // has to go at end of init()

/// random
	mt_setseed(24);

/// build hierarchy
//    roottree=new tree2("roottree"); // moved to MOVIE EDITOR above
//    roottree->trans=pointf3x(0,0,0);

    oldcastles=new tree2("oldcastles");
    oldcastles->setvis(false);
    roottree->linkchild(oldcastles);

    castle=new tree2("castle"); // old
    oldcastles->linkchild(castle);

    newcastle=new tree2("newcastle");
    newcastle->setvis(true);
    roottree->linkchild(newcastle);

	// CAMERA attached to spot light tree, with a flashlight type model
// original version
/*
	me = buildspotlight_xy(PI*.5f,1.5,2,"cvert"); // white ghostly model of cone shaped spotlight//(PI*.5f,1.5,2,"cvert"
    me->setvis(true);
	me->name = "me";
	me->trans = pointf3x(0,0,-.001f); // to get rid of halo, doesnt work //
	me->treedissolvecutoff = .001f;
*/
// ricks version gets rid of sparkly circle on spotlight
	me2 = buildspotlight_xy(PI/8,9,10,"cvert"); // PI/4,1.5,2,"cvert", white ghostly model of cone shaped spotlight//(PI*.5f,1.5,2,"cvert"
	me2->name = "me2";
	me2->treedissolvecutoff = .001f;
	me2->trans = pointf3x(0,0,-.001f); // minus z gets rid of sparkly circle on spotlight
	me = new tree2("me");
    me->setvis(true);
	me->linkchild(me2);
	roottree->linkchild(me);
                                                                  // maptestnck.jpg  // ??? why omni ???
	tree2* flashlight = buildcylinderc_xz(pointf3x(.5f,1,.5f),"maptestnck.jpg","omni", pointf3x(0,0,0) , pointf3x(0,0,0), pointf3x(0,0,0)); // why omni ???
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
	flashlight->trans.z = -1.f; // why  this exact number? has to be some value or undesirable headlamp visible.
	me->linkchild(flashlight);
    flashlight->setvis(true);

/// jack
	jack = new tree2("jack");
	xaxis = buildprism(pointf3x(.01f,.01f,2), "white.pcx", shadernameround);
	xaxis->name = "xaxis";
    xaxis->treecolor = pointf3x(1,0,0);

	yaxis = buildprism(pointf3x(.01f,.01f,2), "white.pcx", shadernameround);
	yaxis->name = "yaxis";
    yaxis->treecolor = pointf3x(0,1,0);

	zaxis = buildprism(pointf3x(.01f,.01f,2), "white.pcx", shadernameround);
	zaxis->name = "zaxis";
    zaxis->rot = pointf3x(0,0,0);
    zaxis->treecolor = pointf3x(0,0,1);

	xaxis_sphere = buildsphere(.05f, "white.pcx", shadernameround);
	xaxis_sphere->name = "xaxis_sphere";
    xaxis_sphere->trans = pointf3x(0,0,1);
    xaxis_sphere->treecolor = pointf3x(1,0,0);

    yaxis_sphere = buildsphere(.05f, "white.pcx", shadernameround);
	yaxis_sphere->name = "yaxis_sphere";
    yaxis_sphere->trans = pointf3x(0,0,-1);
    yaxis_sphere->treecolor = pointf3x(0,1,0);

    zaxis_sphere = buildsphere(.05f, "white.pcx", shadernameround);
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

/// CAMERA
	mainvp.camattach = me;
    mainvp.useattachcam = false;  // duplicate, also in proc()=true, 2x in init()=false


/// LIGHTS attached to tree
// with spotlight
	// ambient
	tree2* amb = new tree2("amb");
	amb->flags |= TF_ISAMBLIGHT|TF_ISLIGHT; //=  doesnt work 4096;//|= TF_ISAMBLIGHT|TF_ISLIGHT;
	amb->lightcolor = F32LIGHTBLUE;
	roottree->linkchild(amb);
	addlighttolist(amb);
	// directional spotlight
//	dir->flags |= TF_ISLIGHT;
	tree2* dir = new tree2("dir");
	dir->flags |= TF_ISLIGHT;
	dir->lightcolor = F32LIGHTGREEN;
	me->linkchild(dir);
	addlighttolist(dir);
	me->trans=pointf3x(31.f,5.5f,-25.f);
/*
// arena1
// add a directional light
	tt=new tree2("adirlight");
	tt->flags|=TF_ISLIGHT;
//	tt->rot.x=PI/4; // PI/4
//	tt->rot.y=PI/2; // PI/4
//	tt->rotvel.y=PI/528;
	tt->trans=pointf3x(31.f,5.5f,-25.f);
	tt->lightcolor=F32LIGHTBLUE; // .75f,.75f,.75f
	addlighttolist(tt);
	roottree->linkchild(tt);

// add an amblight
	tt=new tree2("aamblight");
	tt->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	tt->lightcolor=F32LIGHTGRAY; // .25f,.25f,.25f
	addlighttolist(tt);
	roottree->linkchild(tt);
*/
/// earth and sky
    pushandsetdir("skybox");
    sky=buildskybox(pointf3x(1,1,1), "space.jpg","tex");
    sky->name="sky";
    popdir();
    roottree->linkchild(sky);
/*
// mine
    ground=buildplane_xy(pointf2x(100.,100.),"grass.jpg",shadernameflat); // buildplane_xy_uv
    ground->setvis(true);
    ground->name="ground";
    ground->rot=pointf3x(PI/2,0,0);
    ground->trans=pointf3x(30,-.1,30);
    roottree->linkchild(ground);
*/
/// origin marker
    origin=buildsphere(.05f,"white.pcx",shadernameround);
    roottree->linkchild(origin);

/// old castle room
    wall1=new tree2("wall1");

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

    castle->linkchild(wall1);
    castle->linkchild(wall2);
    castle->linkchild(wall3);
    castle->linkchild(wall4);
    castle->linkchild(roof);
    castle->linkchild(myfloor);

    castle2 = castle->newdup();
    castle2->name = "castle2";
    oldcastles->linkchild(castle2);
    castle2->trans=pointf3x(0,0,12);

/// NEW CASTLE
/// FLOORS
    floors=new tree2("floors");
    floors->setvis(true);
    newcastle->linkchild(floors);
/// first floor stones
    floor1=new tree2("floor1");
    floor1->setvis(true);
    floors->linkchild(floor1);

    uv uv0=uvx(0,0);
    uv uv1=uvx(12.f,2);
    fp60x10=buildprismuvs(pointf3x(60,10,0),"stone_wall.jpg","useshadowmap",uv0,uv1);
    fp60x10->name="fp60x10";
    floor1->linkchild(fp60x10);
    fp60x10->rot=pointf3x(PI/2,0,0);
    fp60x10->trans=pointf3x(31,0,6);

    fp60x10a=fp60x10->newdup();
    floor1->linkchild(fp60x10a);
    fp60x10a->trans=pointf3x(31,0,56);

    uv0=uvx(0,0); // 0,0 is the origin of texture, the upper left. 1,1 is the lower right.
    uv1=uvx(8,2);
    fp40x10=buildprismuvs(pointf3x(40,10,0),"stone_wall.jpg","useshadowmap",uv0,uv1);
    fp40x10->name="fp40x10";
    floor1->linkchild(fp40x10);
    fp40x10->rot=pointf3x(PI/2,PI/2,0);
    fp40x10->trans=pointf3x(6,0,31);

    fp40x10a=fp40x10->newdup();
    floor1->linkchild(fp40x10a);
    fp40x10a->trans=pointf3x(56,0,31);

    floor1->trans = pointf3x(0,0,0); // -.001f

/// second floor wood
    floor2=new tree2("floor2");
    floor2->setvis(true);
    floors->linkchild(floor2);

    uv0=uvx(0,0);
    uv1=uvx(1,2);
    fp60x10=buildprismuvs(pointf3x(60,10,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    fp60x10->name="fp60x10";
    floor2->linkchild(fp60x10);
    fp60x10->rot=pointf3x(PI/2,0,0);
    fp60x10->trans=pointf3x(31,0,6);

    fp60x10a=fp60x10->newdup();
    floor2->linkchild(fp60x10a);
    fp60x10a->trans=pointf3x(31,0,56);

    uv0=uvx(0,0); // 0,0 is the origin of texture, the upper left. 1,1 is the lower right.
    uv1=uvx(1,2);
    fp40x10=buildprismuvs(pointf3x(40,10,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    fp40x10->name="fp40x10";
    floor2->linkchild(fp40x10);
    fp40x10->rot=pointf3x(PI/2,PI/2,0);
    fp40x10->trans=pointf3x(6,0,31);

    fp40x10a=fp40x10->newdup();
    floor2->linkchild(fp40x10a);
    fp40x10a->trans=pointf3x(56,0,31);

    floor2->trans = pointf3x(0,9.75f,0);

/// tower floor wood, orig NLU
/*    tower_floor = new tree2("tower_floor");
    tower_floor->setvis(false);
//    floors->linkchild(tower_floor); // is linked to tower elsewhere

    uv0=uvx(0,0);
    uv1=uvx(1,2);
    fp10x10=buildprismuvs(pointf3x(10,10,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    fp10x10->name="fp10x10";
    tower_floor->linkchild(fp10x10);
    fp10x10->rot=pointf3x(PI/2,0,0);
    fp10x10->trans=pointf3x(6,19.75f,6);
*/
/// tower floor wood, new with trapdoor
    tower_floor1 = new tree2("tower_floor1");
    tower_floor1->setvis(true);
//    floors->linkchild(tower_floor); // is linked to tower elsewhere

    uv0=uvx(0,0);
    uv1=uvx(.4f,1);
    tfp4x10=buildprismuvs(pointf3x(4,10,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    tfp4x10->name="tfp4x10";
    tower_floor1->linkchild(tfp4x10);
    tfp4x10->rot=pointf3x(0,0,0);
    tfp4x10->trans=pointf3x(0,29.75f,0);

    uv0=uvx(.6f,0);
    uv1=uvx(1,1);
    tfp4x10a=buildprismuvs(pointf3x(4,10,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    tfp4x10a->name="tfp4x10a";
    tower_floor1->linkchild(tfp4x10a);
    tfp4x10a->rot=pointf3x(0,0,0);
    tfp4x10a->trans=pointf3x(6,29.75f,0);

    uv0=uvx(.4f,0);
    uv1=uvx(.6f,.4f);
    tfp2x4=buildprismuvs(pointf3x(2,4,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    tfp2x4->name="tfp2x4";
    tower_floor1->linkchild(tfp2x4);
    tfp2x4->rot=pointf3x(0,0,0);
    tfp2x4->trans=pointf3x(3,32.75f,0);

    uv0=uvx(.4f,.6f);
    uv1=uvx(.6f,1);
    tfp2x4a=buildprismuvs(pointf3x(2,4,.5f),"wood_floor_ceiling.jpg",shadernameround,uv0,uv1);
    tfp2x4a->name="tfp2x4a";
    tower_floor1->linkchild(tfp2x4a);
    tfp2x4a->rot=pointf3x(0,0,0);
    tfp2x4a->trans=pointf3x(3,26.75f,0);

    tower_floor1->rot=pointf3x(PI/2,0,0);
    tower_floor1->trans=pointf3x(3.f,19.75f,-23.75f );

/// WALLS
    walls1=new tree2("walls1");
    walls1->setvis(true);

    uv0=uvx(0,0);
    uv1=uvx(6,1);
    wp60x10=buildprismuvs(pointf3x(60,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1); // "useshadowmap"
    wp60x10->name="wp60x10";
    walls1->linkchild(wp60x10);
    wp60x10->rot=pointf3x(0,PI/2,0);
    wp60x10->trans=pointf3x(.5f,5,31);

    wp60x10a=wp60x10->newdup();
    wp60x10a->name="wp60x10a";
    walls1->linkchild(wp60x10a);
    wp60x10a->rot=pointf3x(0,PI/2,0);
    wp60x10a->trans=pointf3x(61.5f,5,31);

    wp60x10b=wp60x10->newdup();
    wp60x10b->name="wp60x10b";
    wp60x10b->setvis(true);
    walls1->linkchild(wp60x10b);
    wp60x10b->rot=pointf3x(0,0,0);
    wp60x10b->trans=pointf3x(31,5,61.5f);

/// front entrance outer wall
    uv0=uvx(0,0);
    uv1=uvx(2.5f,1);
    wp25x10=buildprismuvs(pointf3x(25,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp25x10->name="wp25x10";
    walls1->linkchild(wp25x10);
    wp25x10->rot=pointf3x(0,0,0);
    wp25x10->trans=pointf3x(13.5f,5,.5f);

    uv0=uvx(3.5f,0);
    uv1=uvx(6.0f,1);
    wp25x10a=buildprismuvs(pointf3x(25,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
//    wp25x10a=wp25x10->newdup(); /// need buildprismuvs with correct uvs from x3.5 to x6.0
    wp25x10a->name="wp25x10a";
    walls1->linkchild(wp25x10a);
    wp25x10a->rot=pointf3x(0,0,0);
    wp25x10a->trans=pointf3x(48.5f,5,.5f);
/// front entrance courtyard wall
    uv0=uvx(0,0);
    uv1=uvx(1.4f,1);
    wp14x10=buildprismuvs(pointf3x(14,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp14x10->name="wp14x10";
    walls1->linkchild(wp14x10);
    wp14x10->rot=pointf3x(0,0,0);
    wp14x10->trans=pointf3x(19.f,5,11.5f);

    wp14x10a=wp14x10->newdup();
    wp14x10a->name="wp14x10a";
    walls1->linkchild(wp14x10a);
    wp14x10a->rot=pointf3x(0,0,0);
    wp14x10a->trans=pointf3x(43.f,5,11.5f);
/// the 3 other courtyard walls
    uv0=uvx(0,0);
    uv1=uvx(1.7f,1);
    wp17x10=buildprismuvs(pointf3x(17,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp17x10->name="wp17x10";
    walls1->linkchild(wp17x10);
    wp17x10->rot=pointf3x(0,0,0);
    wp17x10->trans=pointf3x(20.5f,5,50.5f);

    wp17x10a=wp17x10->newdup();
    wp17x10a->name="wp17x10a";
    walls1->linkchild(wp17x10a);
    wp17x10a->rot=pointf3x(0,0,0);
    wp17x10a->trans=pointf3x(41.5f,5,50.5f);

    uv0=uvx(0,0);
    uv1=uvx(1.8f,1);
    wp18x10=buildprismuvs(pointf3x(18,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    wp18x10->name="wp18x10";
    walls1->linkchild(wp18x10);
    wp18x10->rot=pointf3x(0,PI/2,0);
    wp18x10->trans=pointf3x(11.5f,5,20.f);

    wp18x10a=wp18x10->newdup();
    wp18x10a->name="wp18x10a";
    walls1->linkchild(wp18x10a);
    wp18x10a->rot=pointf3x(0,PI/2,0);
    wp18x10a->trans=pointf3x(11.5f,5,42.f);

    wp18x10b=wp18x10->newdup();
    wp18x10b->name="wp18x10b";
    walls1->linkchild(wp18x10b);
    wp18x10b->rot=pointf3x(0,PI/2,0);
    wp18x10b->trans=pointf3x(50.5f,5,20.f);

    wp18x10c=wp18x10->newdup();
    wp18x10c->name="wp18x10c";
    walls1->linkchild(wp18x10c);
    wp18x10c->rot=pointf3x(0,PI/2,0);
    wp18x10c->trans=pointf3x(50.5f,5,42.f);

/// inner wall with doorway
    inner_wall=new tree2("inner_wall");
    inner_wall->name="inner_wall";

    uv0=uvx(0,.05f);
    uv1=uvx(.35f,1);
    iwp3p5x9p5=buildprismuvs(pointf3x(3.5f,9.5f,1),"stone_wall.jpg",shadernameflat,uv0,uv1); // 3.5f,10,1
    iwp3p5x9p5->name="iwp3p5x9p5";
    inner_wall->linkchild(iwp3p5x9p5);
//    iwp3p5x9p5->rot=pointf3x(0,PI/2,0);
    iwp3p5x9p5->trans=pointf3x(0,15.75f,0); // .5f,16.f,1.75f

    uv0=uvx(.65f,.05f);
    uv1=uvx(1,1);
    iwp3p5x9p5a=buildprismuvs(pointf3x(3.5f,9.5f,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    iwp3p5x9p5a->name="iwp3p5x9p5a";
    inner_wall->linkchild(iwp3p5x9p5a);
//    iwp3p5x9p5a->rot=pointf3x(0,PI/2,0);
    iwp3p5x9p5a->trans=pointf3x(6.5f,15.75f,0); // .5f,16.f,8.25f

    uv0=uvx(.35f,.05f);
    uv1=uvx(.65f,.3f);
    iwp3x2p5=buildprismuvs(pointf3x(3,2.5f,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    iwp3x2p5->name="iwp3x2p5";
    inner_wall->linkchild(iwp3x2p5);
    iwp3x2p5->trans=pointf3x(3.25f,19.25f,0);
//    iwp3x2p5->rot=pointf3x(0,PI/2,0);

//    inner_wall->rot=pointf3x(0,PI/2,0);
    inner_wall->trans=pointf3x(2.75,-11,11.5);

    inner_walla=inner_wall->newdup();
    inner_walla->name="inner_walla";
    inner_walla->trans=pointf3x(52.75f,-11,11.5f);

    inner_wallb=inner_wall->newdup();
    inner_wallb->name="inner_wallb";
    inner_wallb->trans=pointf3x(52.75f,-11,25.5f);

    inner_wallc=inner_wall->newdup();
    inner_wallc->name="inner_wallc";
    inner_wallc->trans=pointf3x(52.75f,-11,36.5f);

    inner_walld=inner_wall->newdup();
    inner_walld->name="inner_walld";
    inner_walld->trans=pointf3x(52.75f,-11,50.5f);

    inner_walle=inner_wall->newdup();
    inner_walle->name="inner_walle";
    inner_walle->trans=pointf3x(2.75f,-11,25.5f);

    inner_wallf=inner_wall->newdup();
    inner_wallf->name="inner_wallf";
    inner_wallf->trans=pointf3x(2.75f,-11,36.5f);

    inner_wallg=inner_wall->newdup();
    inner_wallg->name="inner_wallg";
    inner_wallg->trans=pointf3x(2.75f,-11,50.5f);

    inner_wallh=inner_wall->newdup();
    inner_wallh->name="inner_wallh";
    inner_wallh->rot=pointf3x(0,PI/2,0); // bug ??? newdup needs rot to work and 0,0,0 is original rot ???
    inner_wallh->trans=pointf3x(11.5f,-11,59.25f);


    inner_walli=inner_wall->newdup(); // these 3 wont display ???
    inner_walli->name="inner_walli";
    inner_walli->rot=pointf3x(0,PI/2,0);
    inner_walli->trans=pointf3x(25.5f,-11,59.25f);

    inner_wallj=inner_wall->newdup();
    inner_wallj->name="inner_wallj";
    inner_wallj->rot=pointf3x(0,PI/2,0);
    inner_wallj->trans=pointf3x(36.5f,-11,59.25f);

    inner_wallk=inner_wall->newdup();
    inner_wallk->name="inner_wallk";
    inner_wallk->rot=pointf3x(0,PI/2,0);
    inner_wallk->trans=pointf3x(50.5f,-11,59.25f);

    inner_walll=inner_wall->newdup();
    inner_walll->name="inner_walll";
    inner_walll->rot=pointf3x(0,PI/2,0);
    inner_walll->trans=pointf3x(25.5f,-11,9.25f);

    inner_wallm=inner_wall->newdup();
    inner_wallm->name="inner_wallm";
    inner_wallm->rot=pointf3x(0,PI/2,0);
    inner_wallm->trans=pointf3x(36.5f,-11,9.25f);

    inner_walln=inner_wall->newdup();
    inner_walln->name="inner_walln";
    inner_walln->rot=pointf3x(0,PI/2,0);
    inner_walln->trans=pointf3x(50.5f,-11,9.25f);

    inner_wallo=inner_wall->newdup();
    inner_wallo->name="inner_wallo";
    inner_wallo->rot=pointf3x(0,PI/2,0);
    inner_wallo->trans=pointf3x(11.5f,-11,9.25f);


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

///turrets
    turrets=new tree2("turrets");  /// need to rename this to castle turrets
    turrets->setvis(true);
    newcastle->linkchild(turrets);

    turrets40=new tree2("turrets40");
    turrets->linkchild(turrets40);

    turrets10=new tree2("turrets10");

// turrets on top of outer walls
    char turrets40_i_j[100] = "turrets40_";
    float heighth = 4;
    float uv0x = .1f;
    float uv0y;
    float uv1x = .2f;
    float uv1y = 0;
    float transx = 1;
    float transy = 0;
    float transz = 0;

    for (int i=1; i<20; i++){
        for (int j=0; j<2; ++j){
            turrets40_i_j[11] = i+'0';
            turrets40_i_j[12] = '_';
            turrets40_i_j[13] = j+'0';

            if (j==1){ // higher turret
                uv0y = -heighth/10;
                uv0=uvx(uv0x,uv0y);
                uv1=uvx(uv1x,uv1y);
                tree2* turrets40_i_j = buildprismuvs(pointf3x(1,heighth,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
                turrets40_i_j->name="turrets40_";
                turrets40->linkchild(turrets40_i_j);
                turrets40_i_j->trans = pointf3x(0,transy,transz);
                turrets40_i_j->trans.x = transx;
                turrets40_i_j->trans.y = transy;
                transx += 1;
                transy -= .5f;
                heighth -= 1;
                uv0x += .1f;
                uv0y += .1f;
                uv1x += .1f;
            }

            else{ // lower turret
                uv0y = -heighth/10;
                uv0=uvx(uv0x,uv0y);
                uv1=uvx(uv1x,uv1y);
                tree2* turrets40_i_j = buildprismuvs(pointf3x(1,heighth,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
                turrets40_i_j->name="turrets40_";
                turrets40->linkchild(turrets40_i_j);
                turrets40_i_j->trans.x = transx;
                turrets40_i_j->trans.y = transy;
                transx += 1;
                transy += .5f;
                heighth += 1;
                uv0x += .1f;
                uv0y -= .1f;
                uv1x += .1f;
            }
        }
    }
    turrets40->trans=pointf3x(11.5f,12.f,.5f);

    turrets40a=turrets40->newdup();
    turrets->linkchild(turrets40a);
    turrets40a->rot=pointf3x(0,PI,0);
    turrets40a->trans=pointf3x(50.5f,12.f,61.5f);

    turrets40b=turrets40->newdup();
    turrets->linkchild(turrets40b);
    turrets40b->rot=pointf3x(0,PI/2,0);
    turrets40b->trans=pointf3x(.5f,12.f,50.5f);

    turrets40c=turrets40->newdup();
    turrets->linkchild(turrets40c);
    turrets40c->rot=pointf3x(0,-PI/2,0);
    turrets40c->trans=pointf3x(61.5f,12.f,11.5f);

/// turrets on top of towers
    tower_turrets=new tree2("tower_turrets");
    tower_turrets->setvis(true);

    char turrets10_ia_ja[100] = "turrets10_";
    heighth = 4;
    uv0x = 0;
    uv0y;
    uv1x = .1f;
    uv1y = 0;
    transx = 1;
    transy = 0;

    for (int ia=1; ia<6; ia++){
        for (int ja=0; ja<2; ++ja){
            turrets10_ia_ja[11] = ia+'0';
            turrets10_ia_ja[12] = '_';
            turrets10_ia_ja[13] = ja+'0';

            if (ja==1){
                uv0y = -heighth/10;
                uv0=uvx(uv0x,uv0y);
                uv1=uvx(uv1x,uv1y);
                tree2* turrets10_ia_ja = buildprismuvs(pointf3x(1,heighth,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
                turrets10_ia_ja->name="turrets10_";
                turrets10->linkchild(turrets10_ia_ja);
                turrets10_ia_ja->trans = pointf3x(0,transy,transz);
                turrets10_ia_ja->trans.x = transx;
                turrets10_ia_ja->trans.y = transy;
                transx += 1;
                transy -= .5f;
                heighth -= 1;
                uv0x += .1f;
                uv0y += .1f;
                uv1x += .1f;
            }

            else{
                uv0y = -heighth/10;
                uv0=uvx(uv0x,uv0y);
                uv1=uvx(uv1x,uv1y);
                tree2* turrets10_ia_ja = buildprismuvs(pointf3x(1,heighth,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
                turrets10_ia_ja->name="turrets10_";
                turrets10->linkchild(turrets10_ia_ja);
                turrets10_ia_ja->trans.x = transx;
                turrets10_ia_ja->trans.y = transy;
                transx += 1;
                transy += .5f;
                heighth += 1;
                uv0x += .1f;
                uv0y -= .1f;
                uv1x += .1f;
            }
        }
    }

    turrets10->trans=pointf3x(.5f,22.f,.5f); // NOTE y=22.

    turrets10a=turrets10->newdup();
    turrets10a->rot=pointf3x(0,PI/2,0);
    turrets10a->trans=pointf3x(.5f,22.f,11.5f);

    turrets10b=turrets10->newdup();
    turrets10b->rot=pointf3x(0,PI,0);
    turrets10b->trans=pointf3x(11.5f,22.f,11.5f);

    turrets10c=turrets10->newdup();
    turrets10c->rot=pointf3x(0,-PI/2,0);
    turrets10c->trans=pointf3x(11.5f,22.f,.5f);

/// towers
    tower0=new tree2("tower0");
    tower0->setvis(true);

    towersection1=new tree2("towersection1");
    towersection1->setvis(true);

    towersection2=new tree2("towersection2");
    towersection2->setvis(true);

    towersection3=new tree2("towersection3");
    towersection3->setvis(true);

    towersection4=new tree2("towersection4");
    towersection4->setvis(true);
/*
    towersection5=new tree2("towersection5"); // NYI
    towersection5->setvis(false);
*/
/*
    towersection1->linkchild(turrets10);
    towersection1->linkchild(turrets10a);
    towersection1->linkchild(turrets10b);
    towersection1->linkchild(turrets10c);
*/
/// tower wall piece with door (10x10 w/ 3x7 door opening), same as inner_wall--DUPLICATE
    tower_door_wall=new tree2("tower_door_wall");
    tower_door_wall->setvis(true);

    uv0=uvx(0,0);
    uv1=uvx(.35f,1);
    twp3p5x10=buildprismuvs(pointf3x(3.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp3p5x10->name="twp3p5x10";
    tower_door_wall->linkchild(twp3p5x10);
    twp3p5x10->trans=pointf3x(0,16,0);

    uv0=uvx(.65f,0);
    uv1=uvx(1,1);
    twp3p5x10a=buildprismuvs(pointf3x(3.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp3p5x10a->name="twp3p5x10a";
    tower_door_wall->linkchild(twp3p5x10a);
    twp3p5x10a->trans=pointf3x(6.5f,16,0);

    uv0=uvx(.35f,0);
    uv1=uvx(.65f,.3f);
    twp3x3=buildprismuvs(pointf3x(3,3,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp3x3->name="twp3x3";
    tower_door_wall->linkchild(twp3x3);
    twp3x3->trans=pointf3x(3.25f,19.5f,0);

    tower_door_wall->rot=pointf3x(0,PI,0);
    tower_door_wall->trans=pointf3x(9.25,-1,11.5);

    tower_door_walla = tower_door_wall->newdup();
    tower_door_walla->name = "tower_door_walla";
    tower_door_walla->setvis(true);
    tower_door_walla->rot=pointf3x(0,-PI/2,0);
    tower_door_walla->trans=pointf3x(11.5f,-1,2.75f);
/* NLU or could be used later for a 10x10 room with 4 doors
    tower_door_wallb = tower_door_wall->newdup();
    tower_door_wallb->name = "tower_door_wallb";
    tower_door_walla->setvis(false);
    tower_door_wallb->rot=pointf3x(0,0,0);
    tower_door_wallb->trans=pointf3x(2.75,-1,.5f);

    tower_door_wallc = tower_door_wall->newdup();
    tower_door_wallc->name = "tower_door_wallc";
    tower_door_walla->setvis(false);
    tower_door_wallc->rot=pointf3x(0,PI/2,0);
    tower_door_wallc->trans=pointf3x(.5f,-1,9.25f);
*/

/// castle,tower and turret corners
// NOTE: need to change trim15 and trim25 names to trim10stone and trim10gold respectively
    /// corners for tower sections
    tower_corners=new tree2("tower_corners");
    tower_corners->setvis(true);

    uv0=uvx(0,0);
    uv1=uvx(1,10);
    trim25=buildprismuvs(pointf3x(1,10,1),"sand.jpg",shadernameround,uv0,uv1); // specular on this one
    trim25->name="trim25";
	tower_corners->linkchild(trim25);
    trim25->treecolor=pointf3x(1,.85f,0); // gold
    trim25->trans=pointf3x(.5f,15.f,.5f);

    trim15=buildprism(pointf3x(1,10,1),"white.pcx",shadernameround); // specular on this one
    trim15->name="trim15";
	tower_corners->linkchild(trim15);
    trim15->treecolor=pointf3x(1,.85f,0); // gold
    trim15->trans=pointf3x(.5f,15.f,11.5f);

    trim15a = trim15->newdup();
    trim15a->name="trim15a";
	tower_corners->linkchild(trim15a);
    trim15a->trans=pointf3x(11.5f,15.f,.5f);

    trim15b = trim15->newdup();
    trim15b->name="trim15b";
	tower_corners->linkchild(trim15b);
    trim15b->trans=pointf3x(11.5f,15.f,11.5f);

    /// corners for castle
    castle_corners=new tree2("castle_corners");
    castle_corners->setvis(true);
    newcastle->linkchild(castle_corners);

    trim25a=trim25->newdup();
    trim25a->name="trim25a";
	castle_corners->linkchild(trim25a);
//    trim25a->treecolor=pointf3x(1,.85f,0); // gold
    trim25a->trans=pointf3x(.5f,5.f,.5f);

    trim25b=trim25->newdup();
    trim25b->name="trim25b";
	castle_corners->linkchild(trim25b);
//    trim25b->treecolor=pointf3x(1,.85f,0); // gold
    trim25b->trans=pointf3x(61.5f,5.f,.5f);

    trim25c=trim25->newdup();
    trim25c->name="trim25c";
	castle_corners->linkchild(trim25c);
//    trim25c->treecolor=pointf3x(1,.85f,0); // gold
    trim25c->trans=pointf3x(.5f,5.f,61.5f);

    trim25d=trim25->newdup();
    trim25d->name="trim25d";
	castle_corners->linkchild(trim25d);
//    trim25d->treecolor=pointf3x(1,.85f,0); // gold
    trim25d->trans=pointf3x(61.5f,5.f,61.5f);

/// corners for tower turrets
    turret_corners=new tree2("turret_corners");
    turret_corners->setvis(true);
    // linked down below
    uv0=uvx(0,0);
    uv1=uvx(1,5);
    trim4stone=buildprismuvs(pointf3x(1,5,1),"sand.jpg",shadernameround,uv0,uv1); // specular on this one
    trim4stone->name="trim4stone";
	turret_corners->linkchild(trim4stone);
    trim4stone->treecolor=pointf3x(1,.85f,0); // gold
    trim4stone->trans=pointf3x(.5f,22.5f,.5f); // all y=15

    trim4gold=buildprism(pointf3x(1,5,1),"white.pcx",shadernameround); // specular on this one
    trim4gold->name="trim4gold";
	turret_corners->linkchild(trim4gold);
    trim4gold->treecolor=pointf3x(1,.85f,0); // gold
    trim4gold->trans=pointf3x(.5f,22.5f,11.5f);

    trim4golda = trim4gold->newdup();
    trim4golda->name="trim4golda";
	turret_corners->linkchild(trim4golda);
    trim4golda->trans=pointf3x(11.5f,22.5f,.5f);

    trim4goldb = trim4gold->newdup();
    trim4goldb->name="trim4goldb";
	turret_corners->linkchild(trim4goldb);
    trim4goldb->trans=pointf3x(11.5f,22.5f,11.5f);

/// tower wall piece, solid
    uv0=uvx(0,0);
    uv1=uvx(1,1);
    twp10x10=buildprismuvs(pointf3x(10,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp10x10->name="twp10x10";
    twp10x10->setvis(true);
//    towersection1->linkchild(twp10x10);
    twp10x10->trans=pointf3x(6,15,.5f);

    twp10x10a = twp10x10->newdup();
    twp10x10a->name="twp10x10a";
    twp10x10a->setvis(true);
//    towersection1->linkchild(twp10x10);
    twp10x10a->rot=pointf3x(0,PI/2,0);
    twp10x10a->trans=pointf3x(.5f,15,6.f);

    twp10x10b = twp10x10->newdup();
    twp10x10b->name="twp10x10b";
    twp10x10b->setvis(true);
    twp10x10b->rot=pointf3x(0,PI,0);
    twp10x10b->trans=pointf3x(6.f,15,11.5f);

    twp10x10c = twp10x10->newdup();
    twp10x10c->name="twp10x10c";
    twp10x10c->setvis(true);
    twp10x10c->rot=pointf3x(0,-PI/2,0);
    twp10x10c->trans=pointf3x(11.5f,15,6.f);

/// tower wall piece, windowed
    tower_window_wall=new tree2("tower_window_wall");
    tower_window_wall->setvis(true);

    uv0=uvx(0,0);
    uv1=uvx(.25f,1);
    twp2p5x10=buildprismuvs(pointf3x(2.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp2p5x10->name="twp2p5x10";
    tower_window_wall->linkchild(twp2p5x10);
    twp2p5x10->trans=pointf3x(0.f,5,-.5f);

    uv0=uvx(.375f,0);
    uv1=uvx(.625f,1);
    twp2p5x10a = buildprismuvs(pointf3x(2.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp2p5x10a->name="twp2p5x10a";
    tower_window_wall->linkchild(twp2p5x10a);
    twp2p5x10a->trans=pointf3x(3.75f,5,-.5f);

    uv0=uvx(.75f,0);
    uv1=uvx(1,1);
    twp2p5x10b = buildprismuvs(pointf3x(2.5f,10,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp2p5x10b->name="twp2p5x10b";
    tower_window_wall->linkchild(twp2p5x10b);
    twp2p5x10b->trans=pointf3x(7.5f,5,-.5f);

    uv0=uvx(.25f,.6f);
    uv1=uvx(.375f,1);
    twp1p25x4 = buildprismuvs(pointf3x(1.25f,4,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp1p25x4->name="twp1p25x4";
    tower_window_wall->linkchild(twp1p25x4);
    twp1p25x4->trans=pointf3x(1.875f,2,-.5f);

    uv0=uvx(.625,.6f);
    uv1=uvx(.75,1);
    twp1p25x4a = buildprismuvs(pointf3x(1.25f,4,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp1p25x4a->name="twp1p25x4a";
    tower_window_wall->linkchild(twp1p25x4a);
    twp1p25x4a->trans=pointf3x(5.625f,2,-.5f);

    uv0=uvx(.25f,0.f);
    uv1=uvx(.375,.3f);
    twp1p25x3 = buildprismuvs(pointf3x(1.25f,3,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp1p25x3->name="twp1p25x3";
    tower_window_wall->linkchild(twp1p25x3);
    twp1p25x3->trans=pointf3x(1.875f,8.5f,-.5f);

    uv0=uvx(.625f,0.f);
    uv1=uvx(.75f,.3f);
    twp1p25x3a = buildprismuvs(pointf3x(1.25f,3,1),"stone_wall.jpg",shadernameflat,uv0,uv1);
    twp1p25x3a->name="twp1p25x3a";
    tower_window_wall->linkchild(twp1p25x3a);
    twp1p25x3a->trans=pointf3x(5.625f,8.5f,-.5f);

/// tower windows
    tower_windows=new tree2("tower_windows");
    tower_windows->setvis(true);
    tower_window_wall->linkchild(tower_windows);

    window=buildprism(pointf3x(1.25f,3,0),"white.pcx",shadernameround); // specular on this one
    window->name="window";
	tower_windows->linkchild(window);
    window->treecolor=pointf3x(1,1,1,.5f);
    window->trans=pointf3x(1.875f,5.5f,-1);

	window_panes = new tree2("window_panes");
	windowpane_xaxis = buildprism(pointf3x(1.25f,.05f,.05f), "white.pcx", shadernameround);
	windowpane_xaxis->name = "windowpane_xaxis";
    windowpane_xaxis->treecolor = pointf3x(0,0,0);
    windowpane_xaxis->trans=pointf3x(0,0,0); // 1.875f,5.5f,-1.1f
	window_panes->linkchild(windowpane_xaxis);

	windowpane_yaxis = buildprism(pointf3x(.05f,3.f,.05f), "white.pcx", shadernameround);
	windowpane_yaxis->name = "windowpane_yaxis";
    windowpane_yaxis->treecolor = pointf3x(0,0,0);
    windowpane_yaxis->trans=pointf3x(0,0,0);
	window_panes->linkchild(windowpane_yaxis);

	window->linkchild(window_panes);

    window1 = window->newdup();
    window1->name="window1";
	tower_windows->linkchild(window1);
    window1->treecolor=pointf3x(1,1,1,.5f);
    window1->trans=pointf3x(5.625f,5.5f,-1);

    tower_window_wall->trans=pointf3x(2.25f,10,1.f);
/// the other 3 tower window walls
    tower_window_walla=tower_window_wall->newdup();
    tower_window_walla->name="tower_window_walla";
    tower_window_walla->setvis(true);
    tower_window_walla->rot=pointf3x(0,PI/2,0);
    tower_window_walla->trans=pointf3x(1,10,9.75f);

    tower_window_wallb=tower_window_wall->newdup();
    tower_window_wallb->name="tower_window_wallb";
    tower_window_wallb->setvis(true);
    tower_window_wallb->rot=pointf3x(0,PI+PI/2,0);
    tower_window_wallb->trans=pointf3x(11,10,2.25f);

    tower_window_wallc=tower_window_wall->newdup();
    tower_window_wallc->name="tower_window_wallc";
    tower_window_wallc->setvis(true);
    tower_window_wallc->rot=pointf3x(0,PI,0);
    tower_window_wallc->trans=pointf3x(9.75f,10,11.f);

/// tower top corner pyramids on top of tower turrets
    tower_pyramid=buildpyramid(/*pointf3x(.5f,.5f,.5f),*/"white.pcx",shadernameround); // specular on this one
    tower_pyramid->name="tower_pyramid";
    tower_pyramid->treecolor=pointf3x(1,.85f,0); // gold
    tower_pyramid->scale = pointf3x(.5f,2,.5f);
    tower_pyramid->trans=pointf3x(.5f,25,.5f);
	tower_pyramid->flags|=TF_CASTSHADOW;

    tower_pyramid1=tower_pyramid->newdup();
    tower_pyramid1->trans=pointf3x(11.5f,25,11.5f);

    tower_pyramid2=tower_pyramid->newdup();
    tower_pyramid2->trans=pointf3x(.5f,25,11.5f);

    tower_pyramid3=tower_pyramid->newdup();
    tower_pyramid3->trans=pointf3x(11.5f,25,.5f);

/// link towers and tower sections
/// tower turrets
    tower_turrets->linkchild(turrets10);
    tower_turrets->linkchild(turrets10a);
    tower_turrets->linkchild(turrets10b);
    tower_turrets->linkchild(turrets10c);
    tower_turrets->linkchild(turret_corners);
    tower_turrets->linkchild(tower_pyramid);
    tower_turrets->linkchild(tower_pyramid1);
    tower_turrets->linkchild(tower_pyramid2);
    tower_turrets->linkchild(tower_pyramid3);

/// towersection1 // this configuration has 2 door walls and 2 window walls
    towersection1->linkchild(tower_door_wall);
    towersection1->linkchild(tower_door_walla);
    towersection1->linkchild(tower_window_wall);
    towersection1->linkchild(tower_window_walla);
    towersection1->linkchild(tower_floor1);
    towersection1->linkchild(tower_corners);
//    towersection1->linkchild(tower_turrets);

/// towersection2 // this configuration has 2 door walls and 2 solid walls
    tower_door_wallx1=tower_door_wall->newdup();
    tower_door_wallax1=tower_door_walla->newdup();
    tower_floor1x1=tower_floor1->newdup();
    tower_cornersx1=tower_corners->newdup();

    towersection2->linkchild(tower_door_wallx1); // these links go with the newdups above
    towersection2->linkchild(tower_door_wallax1);
    towersection2->linkchild(twp10x10);
    towersection2->linkchild(twp10x10a);
    towersection2->linkchild(tower_floor1x1);
//    towersection2->linkchild(tower_turrets);
    towersection2->linkchild(tower_cornersx1);

/// towersection3 // this configuration has 4 window walls
    tower_window_wallx1=tower_window_wall->newdup();
    tower_window_wallax1=tower_window_walla->newdup();
    tower_floor1x2=tower_floor1->newdup();
    tower_cornersx2=tower_corners->newdup();

    towersection3->linkchild(tower_window_wallx1);
    towersection3->linkchild(tower_window_wallax1);
    towersection3->linkchild(tower_window_wallb);
    towersection3->linkchild(tower_window_wallc);
    towersection3->linkchild(tower_floor1x2);
    towersection3->linkchild(tower_cornersx2);
//    towersection3->linkchild(tower_turrets);

/// towersection4// this configuration has 4 solid walls
    twp10x10copyx1 = twp10x10->newdup();
    twp10x10acopyx1 = twp10x10a->newdup();
    tower_cornersx3=tower_corners->newdup();
    tower_floor1x3=tower_floor1->newdup();

    towersection4->linkchild(twp10x10copyx1);
    towersection4->linkchild(twp10x10acopyx1);
    towersection4->linkchild(twp10x10b);
    towersection4->linkchild(twp10x10c);
    towersection4->linkchild(tower_floor1x3);
    towersection4->linkchild(tower_cornersx3);
    towersection4->linkchild(tower_turrets);

/// towersection5 // this configuration has 4 door walls and could be used elsewhere
/*    towersection5->linkchild(tower_door_wall); // not finished yet
    towersection5->linkchild(tower_door_walla);
    towersection5->linkchild(tower_door_wallb);
    towersection5->linkchild(tower_door_wallc);
    towersection5->linkchild(tower_floor1);
*/
    towersection1->setvis(false);
    towersection1->trans = pointf3x(0,10,0);

    towersection3a = towersection3->newdup(); // topmost tower windowed section

/// stack the desired tower configuration
    towersection2->trans = pointf3x(0,0,0);
    towersection3a->trans = pointf3x(0,10,0);
    towersection3->trans = pointf3x(0,20,0);
    towersection4->trans = pointf3x(0,30,0);

/// set vis on tower sections
    towersection2->setvis(true);
    towersection3->setvis(true);
    towersection3a->setvis(true);
    towersection4->setvis(true);

/// link tower sections to any or all towers
    tower0->linkchild(towersection1);
    tower0->linkchild(towersection2);
    tower0->linkchild(towersection3);
    tower0->linkchild(towersection3a);
    tower0->linkchild(towersection4);

/// duplicate all towers
    tower1 =tower0->newdup();
    tower1->name = "tower1";
    tower1->setvis(true);
    tower1->rot=pointf3x(0,PI+PI/2,0);
    tower1->trans=pointf3x(62,0,0);

    tower2 =tower0->newdup();
    tower2->name = "tower2";
    tower2->setvis(true);
    tower2->rot=pointf3x(0,PI/2,0);
    tower2->trans=pointf3x(0,0,62);

    tower3 =tower0->newdup();
    tower3->name = "tower3";
    tower3->setvis(true);
    tower3->rot=pointf3x(0,-PI,0);
    tower3->trans=pointf3x(62,0,62);

    alltowers=new tree2("alltowers");
    alltowers->setvis(true);

    alltowers->linkchild(tower0);
    alltowers->linkchild(tower1);
    alltowers->linkchild(tower2);
    alltowers->linkchild(tower3);
    newcastle->linkchild(alltowers);

/// castle courtyard trim
    trim=new tree2("trim");
    newcastle->linkchild(trim);
    trim->setvis(true);

    trim38=buildprism(pointf3x(38,1,1),"white.pcx",shadernameround); // specular on this one
    trim38->name="trim38";
	trim->linkchild(trim38);
    trim38->treecolor=pointf3x(1,.85f,0); // gold
    trim38->trans=pointf3x(31.f,10.5f,11.5f);

    trim38a = trim38->newdup();
    trim38a->name="trim38a";
	trim->linkchild(trim38a);
    trim38a->trans=pointf3x(31.f,10.5f,50.5f);

    trim38b = trim38->newdup();
    trim38b->name="trim38b";
	trim->linkchild(trim38b);
    trim38b->rot=pointf3x(0,PI/2,0);
    trim38b->trans=pointf3x(50.5f,10.5f,31.f);

    trim38c = trim38->newdup();
    trim38c->name="trim38c";
	trim->linkchild(trim38c);
    trim38c->rot=pointf3x(0,PI/2,0);
    trim38c->trans=pointf3x(11.5f,10.5f,31.f);

/// castle objects ///
    objects=new tree2("objects");
    objects->setvis(false);
    newcastle->linkchild(objects);

/// multi-material
	const C8* texlist[] = {
		"stone_wall.jpg",
		"Stones.jpg",
		"maptestnck.tga",
		"sand.jpg",
		"pencil.jpg",
		"Bark.tga"
	};

    multimat=buildprism6t(pointf3x(.5,.5,.5),texlist,shadernameround);
    multimat->name="multimat";
	objects->linkchild(multimat);
    multimat->treecolor=pointf3x(1,.75f,.75f);
//    multimat->rot=pointf3x(0,0,PI/3);
    multimat->trans=pointf3x(23,5,-15);
//    multimat->rotvel.x= -.02f;
    multimat->rotvel.y= -.02f;
//    multimat->rotvel.z= -.02f;
    multimat->scale = pointf3x(10,10,10);
	multimat->flags|=TF_CASTSHADOW;

/// front doors wall lamps
/*	wall_lamp = new tree2("wall_lamp");
	wall_lamp->name = "wall_lamp"; //
    wall_lamp->setvis(true);
*/
/*	wall_lamp_cone = buildcone_xy(PI/8,1,1,"cvert"); // PI/4,1.5,2,"cvert", white ghostly model of cone shaped spotlight//(PI*.5f,1.5,2,"cvert"
	wall_lamp_cone->name = "wall_lamp_cone";
//	wall_lamp_cone->treedissolvecutoff = .001f;
	wall_lamp->rot.x = PI*.5f;
	wall_lamp->trans = pointf3x(25,8,-.5f);
	objects->linkchild(wall_lamp_cone);
*/
/*
	wall_lamp->rot.x = PI*.5f;
	wall_lamp->trans = pointf3x(31,8,-.5f);
	objects->linkchild(wall_lamp);
*/


///  TRITEST1 sphere with bumpmap and black 'normals' spikes,  and also shadow mapping
//	tree2* tt; // is static above

/// build a shadowmapviewer
	tt=buildshadowmapviewermodel("shadowmapviewer","shadowmapviewer");
	tt->trans=pointf3x(68,6,0); // 7.071f,-7.071f,15
	tt->rot.x=-PI/2.0f;
	roottree->linkchild(tt);

/// build a normalmap model
	tt=buildnormalmapmodel("shadermodel",shadename);
//    tt->setvis(false);
	tt->scale=pointf3x(.5f,.5f,.5f);
	tt->trans=pointf3x(55,5,5); // -8.071f,8.071f,7.071f*2
	tt->rotvel.x=.003f;
	tt->rotvel.y=.0035f;
	objects->linkchild(tt); // was linked to roottree
	normalmaptree=tt;
/*	if (video3dinfo.cantoon) { // comment out to get rid of black toon normals following the camera
		tree2* n=tt->newdup();
		normalify(n);
		objects->linkchild(n);
		tree2* t=tt->newdup();
		normalify(t,1);
		objects->linkchild(t);
		tree2* bn=tt->newdup();
		normalify(bn,2);
		objects->linkchild(bn);
	}*/

// spheres

	tree2* tt; // is this syntax necessary ?
    tt = buildenvironmentmapmodel("envmodel","env");
    tt->name = "reflection_sphere";
	objects->linkchild(tt);
	tt->trans=pointf3x(57,5,6);;
//    tt->transvel.z = -.05;

    magicball=buildsphere(.5,"white.pcx", shadernameround);
    magicball->name="magicball";
	objects->linkchild(magicball);
    magicball->treecolor=pointf3x(1,.85f,0);
    magicball->trans=pointf3x(53,5,3);
    magicball->rotvel.x= -.02f;
    magicball->rotvel.y= -.02f;
    magicball->rotvel.z= -.02f;
	magicball->scale = pointf3x(3,2,1);
// cubes
    mycube=buildprism(pointf3x(.5,.5,.5),"white.pcx",shadernameround); // specular on this one
    mycube->name="mycube";
	objects->linkchild(mycube);
    mycube->treecolor=pointf3x(0,0,1);
    mycube->rot=pointf3x(0,0,PI/3);
    mycube->trans=pointf3x(46,5,-10);
    mycube->rotvel.x= -.02f;
    mycube->rotvel.y= -.02f;
    mycube->rotvel.z= -.02f;
    mycube->scale = pointf3x(3,3,3);
	mycube->flags|=TF_CASTSHADOW;
/// pyramid
    pyramid=buildpyramid(/*pointf3x(.5f,.5f,.5f),*/"white.pcx",shadernameround); // specular on this one
    pyramid->name="pyramid";
	objects->linkchild(pyramid);
    pyramid->treecolor=pointf3x(0,1,0);
//    pyramid->rot=pointf3x(0,0,PI/3);
    pyramid->trans=pointf3x(31,5,-15);
//    pyramid->rotvel.x= -.02f;
    pyramid->rotvel.y= -.02f;
//    pyramid->rotvel.z= -.02f;
    pyramid->scale = pointf3x(2,10,2);
	pyramid->flags|=TF_CASTSHADOW;
/// tetrahedron
    tetrahedron=buildtetrahedron(/*pointf3x(.5f,.5f,.5f),*/"white.pcx",shadernameround,"buildtetrahedron"); // specular on this one
    tetrahedron->name="tetrahedron";
	objects->linkchild(tetrahedron);
    tetrahedron->treecolor=pointf3x(0,1,0);
//    tetrahedron->rot=pointf3x(0,0,PI/3);
    tetrahedron->trans=pointf3x(15,5,-15);
//    tetrahedron->rotvel.x= -.02f;
    tetrahedron->rotvel.y= -.02f;
 //   tetrahedron->rotvel.z= -.02f;
    tetrahedron->scale = pointf3x(4,4,4);
	tetrahedron->flags|=TF_CASTSHADOW;
/// octahedron
    octahedron=buildoctahedron(/*pointf3x(.5f,.5f,.5f),*/"maptestnck.tga",shadernameround,"buildoctahedron"); // specular on this one
    octahedron->name="octahedron";
	objects->linkchild(octahedron);
    octahedron->treecolor=pointf3x(0,1,0);
//    octahedron->rot=pointf3x(0,0,PI/3);
    octahedron->trans=pointf3x(0,6,-15);
//    octahedron->rotvel.x= -.02f;
    octahedron->rotvel.y= -.02f;
//    octahedron->rotvel.z= -.02f;
    octahedron->scale = pointf3x(4,4,4);
	octahedron->flags|=TF_CASTSHADOW;
    octahedron->mod->mats[0].specpow = specpowval; // copied from many code here.
// tori
    ring=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 300, 300);
    ring->name="ring";
	objects->linkchild(ring);
    ring->treecolor=pointf3x(1,.85f,0); // gold
    ring->rot=pointf3x(PI/2,0,0);
    ring->trans=pointf3x(5,5,5);
    ring->rotvel.x= -.02f;
    ring->rotvel.y= -.02f;
    ring->rotvel.z= -.02f;
    ring->scale = pointf3x(2,2,2);
	ring->flags|=TF_CASTSHADOW;

    mytorus=buildtorus_xz(.25f,.125f,"white.pcx",shadernameround);
    mytorus->name="mytorus";
	objects->linkchild(mytorus);
    mytorus->treecolor=pointf3x(1,0,1);
    mytorus->rot=pointf3x(PI/2,0,0);
    mytorus->trans=pointf3x(3,8,8);
    mytorus->rotvel.x= -.02f;
    mytorus->rotvel.y= -.02f;
    mytorus->rotvel.z= -.02f;
    mytorus->scale = pointf3x(3,3,3);
	mytorus->flags|=TF_CASTSHADOW;

// polygons built out of tori
    digon=buildtorus_xz(1,.45f,"white.pcx",shadernameround, 2, 10);
    digon->name="digon";
	objects->linkchild(digon);
    digon->treecolor=pointf3x(1,.85f,0); // gold
    digon->rot=pointf3x(PI/2,0,0);
    digon->trans=pointf3x(6,5,6);
    digon->rotvel.x= -.03f;
    digon->rotvel.y= -.03f;
    digon->rotvel.z= -.03f;
    digon->scale = pointf3x(1,1,1);
	digon->flags|=TF_CASTSHADOW;

    digon1 = digon->newdup();
    digon1->name="digon1";
	objects->linkchild(digon1);
    digon1->trans=pointf3x(31,31,31);

    trigon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 3, 300);
	objects->linkchild(trigon);
    trigon->name="trigon";
    trigon->treecolor=pointf3x(1,.85f,0); // gold
    trigon->rot=pointf3x(PI/2,0,0);
    trigon->trans=pointf3x(6,5,6);
    trigon->rotvel.x= -.02f;
    trigon->rotvel.y= -.02f;
    trigon->rotvel.z= -.02f;
    trigon->scale = pointf3x(3,3,3);
	trigon->flags|=TF_CASTSHADOW;

    tetragon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 4, 300);
    tetragon->name="tetragon";
	objects->linkchild(tetragon);
    tetragon->treecolor=pointf3x(1,.85f,0); // gold
    tetragon->rot=pointf3x(PI/2,0,0);
    tetragon->trans=pointf3x(3,5,3);
    tetragon->rotvel.x= -.02f;
    tetragon->rotvel.y= -.02f;
    tetragon->rotvel.z= -.02f;
    tetragon->scale = pointf3x(1,1,1);
	tetragon->flags|=TF_CASTSHADOW;

    pentagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 5, 300);
    pentagon->name="pentagon";
	objects->linkchild(pentagon);
    pentagon->treecolor=pointf3x(1,.85f,0); // gold
    pentagon->rot=pointf3x(PI/2,0,0);
    pentagon->trans=pointf3x(8,5,3);
    pentagon->rotvel.x= -.02f;
    pentagon->rotvel.y= -.02f;
    pentagon->rotvel.z= -.02f;
    pentagon->scale = pointf3x(1,1,1);
	pentagon->flags|=TF_CASTSHADOW;

    hexagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 6, 300);
    hexagon->name="hexagon";
	objects->linkchild(hexagon);
    hexagon->treecolor=pointf3x(1,.85f,0); // gold
    hexagon->rot=pointf3x(PI/2,0,0);
    hexagon->trans=pointf3x(3,5,8);
    hexagon->rotvel.x= -.02f;
    hexagon->rotvel.y= -.02f;
    hexagon->rotvel.z= -.02f;
    hexagon->scale = pointf3x(1,1,1);
	hexagon->flags|=TF_CASTSHADOW;

    heptagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 7, 300);
    heptagon->name="heptagon";
	objects->linkchild(heptagon);
    heptagon->treecolor=pointf3x(1,.85f,0); // gold
    heptagon->rot=pointf3x(PI/2,0,0);
    heptagon->trans=pointf3x(8,5,8);
    heptagon->rotvel.x= -.02f;
    heptagon->rotvel.y= -.02f;
    heptagon->rotvel.z= -.02f;
    heptagon->scale = pointf3x(1,1,1);
	heptagon->flags|=TF_CASTSHADOW;

    octagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 8, 300);
    octagon->name="octagon";
	objects->linkchild(octagon);
    octagon->treecolor=pointf3x(1,.85f,0); // gold
    octagon->rot=pointf3x(PI/2,0,0);
    octagon->trans=pointf3x(31,31,31);
    octagon->rotvel.x= -.02f;
    octagon->rotvel.y= -.02f;
    octagon->rotvel.z= -.02f;
    octagon->scale = pointf3x(4,4,4);

    octacontagon=buildtorus_xz(1,.1f,"white.pcx",shadernameround, 80, 3);
    octacontagon->name="octacontagon";
	objects->linkchild(octacontagon);
    octacontagon->treecolor=pointf3x(1,.85f,0); // gold
    octacontagon->rot=pointf3x(PI/2,0,0);
    octacontagon->trans=pointf3x(31,31,31);
//    octacontagon->rotvel.x= -.02f;
//    octacontagon->rotvel.y= -.02f;
//    octacontagon->rotvel.z= -.02f;
    octacontagon->scale = pointf3x(1,1,1);
// cylinders
	c=buildcylinderc_xz(pointf3x(.25f,1,.75f),"white.pcx",shadernameround, pointf3x(1,0,0), pointf3x(0,1,0), pointf3x(0,0,1));
	c->trans = pointf3x(0,-.5f,0); // center cylinder
	mycylinder = new tree2("cyl_root");
	mycylinder->linkchild(c);
	objects->linkchild(mycylinder);
    mycylinder->rot=pointf3x(0,0,0);
    mycylinder->trans=pointf3x(53,5,53);
    mycylinder->rotvel.x= -.02f;
    mycylinder->rotvel.y= -.02f;
    mycylinder->rotvel.z= -.02f;
    mycylinder->scale = pointf3x(2,2,2);

// cones TBI further. needs cone_surf class to be made, similar to spotlightxy_surf                                            // 1.5, 2
/*    mycone=buildcone_xy(PI/8,10,10,"cvert"); // C:\Users\HP\Documents\Rick\codeblocks_code-4\codeblocks_code-4\codeblocks_code\engq\engqtest\u_modelutil.h|419|undefined reference to `conexy_surf::operator()(float, float, pointf3*, pointf3*)'|
    mycone->name="mycone";
//    mycone->treecolor=pointf3x(1,1,1); // doesnt work, color is set in buildcone()
//    mycone->rot=pointf3x(0,PI/2,0);
    mycone->trans=pointf3x(10,5,-10);
//    mycone->rotvel.x= -.02f;
  //  mycone->rotvel.y= -.02f;
    //mycone->rotvel.z= -.02f;
    mycone->scale = pointf3x(1,1,1);
	mycone->flags|=TF_CASTSHADOW;
    objects->linkchild(mycone);
*/
/// staircases
    staircases=new tree2("staircases");
    staircases->setvis(true);
    newcastle->linkchild(staircases);

    staircase=new tree2("staircase");
/// new automated staircase steps building process
    stairs=new tree2("stairs");
    staircase->linkchild (stairs);

    char stair_i_j[100] = "stair_";
    uv0y = .9f;
    uv1y = 1.f;
    transy = .5f;
    transz = -.5f;
    float rotx = 0.f;

    for (int i=1; i<11; ++i)
        for (int j=0; j<2; ++j){

            uv0=uvx(0,uv0y);
            uv1=uvx(.3f,uv1y);
            stair_i_j[11] = i+'0';
            stair_i_j[12] = '_';
            stair_i_j[13] = j+'0';

            if ( j == 0){
               tree2* stair_i_j = buildprismuvs(pointf3x(3,1,0),"sand.jpg",shadernameround,uv0,uv1);
                stair_i_j->name = "stair_i_j";
                stairs->linkchild (stair_i_j);
                stair_i_j->trans = pointf3x(0,transy,transz);
            }
            else{
                tree2* stair_i_j = buildprismuvs(pointf3x(3,1,0),"sand.jpg",shadernameround,uv0,uv1);
                stair_i_j->name = "stair_i_j";
                stairs->linkchild (stair_i_j);
                stair_i_j->rot.x = PI/2;
                stair_i_j->trans = pointf3x(0,transy,transz);
            }
            uv0y -= .1f;
            uv1y -= .1f;
            transy += .5f;
            transz += .5f;
            rotx -= PI/2;
        }

/// original steps building process
/*
    uv0=uvx(0,.9F);
    uv1=uvx(.3f,1);
    sp1x3=buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp1x3->name = "sp1x3";
    staircase->linkchild(sp1x3);
    sp1x3->trans = pointf3x(0,.5f,0);
    uv0=uvx(0,.8f);
    uv1=uvx(.3f,.9f);
    sp2x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp2x3->name = "sp2x3";
    staircase->linkchild(sp2x3);
    sp2x3->trans = pointf3x(0,1.5f,1);
    uv0=uvx(0,.7f);
    uv1=uvx(.3f,.8f);
    sp3x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp3x3->name = "sp3x3";
    staircase->linkchild(sp3x3);
    sp3x3->trans = pointf3x(0,2.5f,2);
    uv0=uvx(0,.6f);
    uv1=uvx(.3f,.7f);
    sp4x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp4x3->name = "sp4x3";
    staircase->linkchild(sp4x3);
    sp4x3->trans = pointf3x(0,3.5f,3);
    uv0=uvx(0,.5f);
    uv1=uvx(.3f,.6f);
    sp5x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp5x3->name = "sp5x3";
    staircase->linkchild(sp5x3);
    sp5x3->trans = pointf3x(0,4.5f,4);
    uv0=uvx(0,.4f);
    uv1=uvx(.3f,.5f);
    sp6x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp6x3->name = "sp6x3";
    staircase->linkchild(sp6x3);
    sp6x3->trans = pointf3x(0,5.5f,5);
    uv0=uvx(0,.3f);
    uv1=uvx(.3f,.4f);
    sp7x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp7x3->name = "sp7x3";
    staircase->linkchild(sp7x3);
    sp7x3->trans = pointf3x(0,6.5f,6);
    uv0=uvx(0,.2f);
    uv1=uvx(.3f,.3f);
    sp8x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp8x3->name = "sp8x3";
    staircase->linkchild(sp8x3);
    sp8x3->trans = pointf3x(0,7.5f,7);
    uv0=uvx(0,.1f);
    uv1=uvx(.3f,.2f);
    sp9x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp9x3->name = "sp9x3";
    staircase->linkchild(sp9x3);
    sp9x3->trans = pointf3x(0,8.5f,8);
    uv0=uvx(0,0);
    uv1=uvx(.3f,.1f);
    sp10x3 = buildprismuvs(pointf3x(3,1,1),"sand.jpg",shadernameround,uv0,uv1);
    sp10x3->name = "sp10x3";
    staircase->linkchild(sp10x3);
    sp10x3->trans = pointf3x(0,9.5f,9);
*/
/// staircase sides
    staircase_side=new tree2("staircase_side");
    staircase->linkchild(staircase_side);
    uv0=uvx(0,.9F);
    uv1=uvx(.1f,1);
    sp1x1x0=buildprismuvs(pointf3x(1,1,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x1x0->name = "sp1x1x0";
    staircase_side->linkchild(sp1x1x0);
    sp1x1x0->trans = pointf3x(0,0,0);
    uv0=uvx(.1f,.8f);
    uv1=uvx(.2f,1);
    sp1x2x0 = buildprismuvs(pointf3x(1,2,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x2x0->name = "sp1x2x0";
    staircase_side->linkchild(sp1x2x0);
    sp1x2x0->trans = pointf3x(1,.5f,0);
    uv0=uvx(.2f,.7f);
    uv1=uvx(.3f,1);
    sp1x3x0 = buildprismuvs(pointf3x(1,3,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x3x0->name = "sp1x3x0";
    staircase_side->linkchild(sp1x3x0);
    sp1x3x0->trans = pointf3x(2,1,0);
    uv0=uvx(.3f,.6f);
    uv1=uvx(.4f,1);
    sp1x4x0 = buildprismuvs(pointf3x(1,4,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x4x0->name = "sp1x4x0";
    staircase_side->linkchild(sp1x4x0);
    sp1x4x0->trans = pointf3x(3,1.5f,0);
    uv0=uvx(.4f,.5f);
    uv1=uvx(.5f,1);
    sp1x5x0 = buildprismuvs(pointf3x(1,5,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x5x0->name = "sp1x5x0";
    staircase_side->linkchild(sp1x5x0);
    sp1x5x0->trans = pointf3x(4,2,0);
    uv0=uvx(.5f,.4f);
    uv1=uvx(.6f,1);
    sp1x6x0 = buildprismuvs(pointf3x(1,6,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x6x0->name = "sp1x6x0";
    staircase_side->linkchild(sp1x6x0);
    sp1x6x0->trans = pointf3x(5,2.5f,0);
    uv0=uvx(.6f,.3f);
    uv1=uvx(.7f,1);
    sp1x7x0 = buildprismuvs(pointf3x(1,7,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x7x0->name = "sp1x7x0";
    staircase_side->linkchild(sp1x7x0);
    sp1x7x0->trans = pointf3x(6,3,0);
    uv0=uvx(.7f,.2f);
    uv1=uvx(.8f,1);
    sp1x8x0 = buildprismuvs(pointf3x(1,8,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x8x0->name = "sp1x8x0";
    staircase_side->linkchild(sp1x8x0);
    sp1x8x0->trans = pointf3x(7,3.5f,0);
    uv0=uvx(.8f,.1f);
    uv1=uvx(.9f,1);
    sp1x9x0 = buildprismuvs(pointf3x(1,9,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x9x0->name = "sp1x9x0";
    staircase_side->linkchild(sp1x9x0);
    sp1x9x0->trans = pointf3x(8,4,0);
    uv0=uvx(.9f,0);
    uv1=uvx(1,1);
    sp1x10x0 = buildprismuvs(pointf3x(1,10,0),"sand.jpg",shadernameround,uv0,uv1);
    sp1x10x0->name = "sp1x10x0";
    staircase_side->linkchild(sp1x10x0);
    sp1x10x0->trans = pointf3x(9,4.5f,0);
    uv0=uvx(0,0);
    uv1=uvx(.3f,1);
    sp3x10x0 = buildprismuvs(pointf3x(3,10,0),"sand.jpg",shadernameround,uv0,uv1);
    sp3x10x0->name = "sp3x10x0";
    staircase_side->linkchild(sp3x10x0);
    sp3x10x0->trans = pointf3x(11.f,4.5f,0);

    staircase_side->rot = pointf3x(0,-PI/2,0);
    staircase_side->trans = pointf3x(1.50f,.5f,0);


/*
/// staircase back, no longer used, not visible
    uv0=uvx(.3f,0);
    uv1=uvx(.6f,1);
    sp3x10x0back = buildprismuvs(pointf3x(3,10,0),"sand.jpg",shadernameround,uv0,uv1);
    sp3x10x0back->name = "sp3x10x0back";
    staircase->linkchild(sp3x10x0back);
    sp3x10x0back->rot = pointf3x(0,-PI,0);
    sp3x10x0back->trans = pointf3x(0,5.f,12.5f);
*/
/// staircase landing
    uv0=uvx(0,.7f);
    uv1=uvx(.3f,1);
    sp3x3x0landing = buildprismuvs(pointf3x(3,3,0),"sand.jpg",shadernameround,uv0,uv1);
    sp3x3x0landing->name = "sp3x3x0landing";
    staircase->linkchild(sp3x3x0landing);
    sp3x3x0landing->rot = pointf3x(PI/2,0,0);
    sp3x3x0landing->trans = pointf3x(0,10,11);
/// move and dup staircases
    staircases->linkchild(staircase);
    staircase->rot = pointf3x(0,0,0);
    staircase->trans = pointf3x(13.5f,0.f,37.5f);

    staircase1 = staircase->newdup();
    staircase1->name = "staircase1";
    staircases->linkchild(staircase1);
    staircase1->rot = pointf3x(0,PI,0);
    staircase1->trans = pointf3x(48.5f,0.f,24.5f);

/// doors
    doors=new tree2("doors");
    newcastle->linkchild(doors);
    doors->setvis(true);

	hinged_door = new tree2("hinged_door");
    doors->linkchild(hinged_door);

    door3x7 = buildprism(pointf3x(3,7,.25f),"wood_door.jpg",shadernameround);
    door3x7->name = "door3x7";
    hinged_door->linkchild(door3x7);
    door3x7->rot = pointf3x(0,0,0); // commented out for hinge door test
    door3x7->trans = pointf3x(1.5f,0,0); // 1.5f,28.5f,0 for hinge door test
//    hinged_door->rot.y = 0; // use for hinge door test
    hinged_door->trans = pointf3x(11.5f,3.5f,7.5f); // for hinge door test
//    door3x7->rotvel.y = .01;

//	hinged_door = new tree2("hinged_door");
//	hinged_door->name = "hinged_door";
//    doors->linkchild(hinged_door);
/*
    door3x7a = door3x7->newdup();
    door3x7a->name = "door3x7a";
    hinged_door->linkchild(door3x7a);//
    door3x7a->setvis(true); // sets original false too
    door3x7a->rot.y = PI/2; // use for hinge door test
    door3x7a->trans = pointf3x(11.5f,38.5f,7.5f);
*/

//    door3x7a->rotvel.y = .01f;
//    hinged_door->trans = pointf3x(-11.5f,18.5f,-6.f);
//    hinged_door->trans = pointf3x(0.f,15.f,0.f);
//    hinged_door->rotvel.y = .01f;

    door4x10 = buildprism(pointf3x(4,10,.5f),"wood_door.jpg",shadernameround);
    door4x10->name = "door4x10";
    doors->linkchild(door4x10);
    door4x10->rot = pointf3x(0,PI/2,0);
    door4x10->trans = pointf3x(11.25f,5.f,31.f);
    door4x10->rotvel.y = .01;

    door4x10a = door4x10->newdup();
    door4x10a->name = "door4x10";
    doors->linkchild(door4x10a);
    door4x10a->rot = pointf3x(0,PI/2,0);
    door4x10a->trans = pointf3x(50.75f,5.f,31.f);
    door4x10a->rotvel.y = .01;

    door4x10b = door4x10->newdup();
    door4x10b->name = "door4x10";
    doors->linkchild(door4x10b);
//    door4x10b->rot = pointf3x(0,0,0);
    door4x10b->trans = pointf3x(31.f,5.f,50.75f);
    door4x10b->rotvel.y = .01;

    door5x10 = buildprism(pointf3x(5,10,1),"wood_door.jpg",shadernameround);
    door5x10->name = "door5x10";
    doors->linkchild(door5x10);
//    door5x10->rot = pointf3x(0,PI/2,0);
    door5x10->trans = pointf3x(28.5f,5.f,.5f);
    door5x10->rotvel.y = .01;

    door5x10a = door5x10->newdup();
    door5x10a->name = "door5x10a";
    doors->linkchild(door5x10a);
//    door5x10a->rot = pointf3x(0,PI/2,0);
    door5x10a->trans = pointf3x(33.5f,5.f,.5f);
    door5x10a->rotvel.y = .01;

    door5x10b = door5x10->newdup();
    door5x10b->name = "door5x10b";
    doors->linkchild(door5x10b);
//    door5x10b->rot = pointf3x(0,PI/2,0);
    door5x10b->trans = pointf3x(28.5f,5.f,11.5f);
    door5x10b->rotvel.y = .01;

    door5x10c = door5x10->newdup();
    door5x10c->name = "door5x10c";
    doors->linkchild(door5x10c);
//    door5x10c->rot = pointf3x(0,PI/2,0);
    door5x10c->trans = pointf3x(33.5f,5.f,11.5f);
    door5x10c->rotvel.y = .01;
/// hinged door
//	hinged_door = new tree2("hinged_door");
//	hinged_door->name = "hinged_door";
/*
    hinge = buildcylinder_xz(pointf3x(.01f,7,.01f),"white.pcx","tex");
	hinge->name = "hinge";
    hinge->rot = pointf3x(0,0,0);
    hinge->linkchild(door3x7a);
    newcastle->linkchild(hinge);
    hinge->trans = pointf3x(30,30,0);
*/
///door frame, extruded becomes a tunnel, crashes
/*
	door_frame = new tree2("door_frame");
	door_frame->name = "door_frame";
    door_frame=buildtorus_xz(2.2f,.1f,"white.pcx",shadernameround, 4, 25); // 4, 300);

    door_frame->scale = pointf3x(1,20.5f,1); // creates a tunnel model
    door_frame->treecolor=pointf3x(1,.85f,0); // gold
    door_frame->rot=pointf3x(PI/4,PI/2,PI/2); // fixed by rick
//    door_frame->rot=pointf3x(0,0,PI/4); // rot z doesnt work with torus
    door_frame->trans=pointf3x(-5,5,-5);
    objects->linkchild(door_frame);
    door_frame_tree->scale = pointf3x(1,2.37f,1); // thickness of upper and lower elements increase with y scale
*/
/// ricks many.cpp code

	tree2* m,*c1;
/*#ifdef DOSPEC  // NLU
	const C8* shadernameround = "pdiffspec"; //  error already declared above
#else
	const C8* shadername = "tex";
#endif
*/
	m = buildprism(pointf3x(1.f,1.f,1.f),"maptestnck.tga",shadernameround);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);
		// sphere
	m = buildsphere(.5f,"maptestnck.tga",shadernameround);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);
		// cylinder
	c1 = buildcylinder_xz(pointf3x(.5f,1,.5f),"maptestnck.tga",shadernameround);
	c1->trans = pointf3x(0,-.5f,0); // center cylinder
	m = new tree2("cyl_root");
	m->linkchild(c1);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);
		// torus
	m = buildtorus_xz(.25f,.125f,"maptestnck.tga",shadernameround);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);

	m = buildcone_xy(PI/8,1,1,"cvert"/*, pointf3x(1,0,0)*/);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);

	m = buildpyramid("maptestnck.tga",shadernameround);
    m->scale = pointf3x(.5f,.5f,.5f);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);

	m = buildtruncatedhexahedron("maptestnck.tga","pdiffspec","a truncatedhexahedron");
    m->scale = pointf3x(.5f,.5f,.5f);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);

	m = buildrhombicuboctahedron("maptestnck.tga","pdiffspec","a rhombicuboctahedron");
    m->scale = pointf3x(.5f,.5f,.5f);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);
/*
	m = buildtetrahedron("maptestnck.tga",shadernameround,"buildtetrahedron");
//    m->scale = pointf3x(.5f,.5f,.5f);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);

	m = buildoctahedron("maptestnck.tga",shadernameround,"buildoctahedron");
    m->scale = pointf3x(.25f,.5f,.25f);
	m->flags|=TF_CASTSHADOW;
	masters.push_back(m);
*/
//	m->mod->mats[0].msflags&=~SMAT_HASWBUFF; // turn off z buffer
    many=new tree2("many"); // i added this parent of many
    many->setvis(true);
    newcastle->linkchild(many);

	mt_setseed(curtime); // set seed to current time float to get more random
	U32 i,j,k,el, numberof=5; /// number of objects cubed
	for (k=0;k<numberof;++k) {
		for (j=0;j<numberof;++j) {
			for (i=0;i<numberof;++i) {
				el = mt_random(masters.size());
				tree2* p=masters[el]->newdup(); // tree p is local to what, the init() function ???
				many->linkchild(p);
				p->trans=pointf3x(1.5f*i,1.5f*j,1.5f*k); // 2.0f distance between each many

#ifdef ALTTEX
				p->alttex=texture_create(rndtex[mt_random(NRNDTEX)]);
				if (texture_getrc(p->alttex)==1) {
					p->alttex->load();
				}
#endif

#ifdef DOROTVEL
				p->rotvel.x=mt_frand()*.005f;
				p->rotvel.y=mt_frand()*.005f;
#endif
//				p->flags|=TF_TREECOLOR; // NOTE THIS FLAG USAGE
#ifdef ALTCOLOR
				p->treecolor=pointf3x(mt_frand(),mt_frand(),mt_frand(),1.f); // .65f alpha set to semi transparent
#endif
                many->trans = pointf3x(3,2,28.5f);
//                p->trans = pointf3x(6,5,31); // weird, but very interesting looking BUG that creates all objects in one place.
			}
		}
	}

/// my many
	tree2* aplatonic;

	aplatonic = buildtetrahedron("maptestnck.tga",shadernameround,"buildtetrahedron");
    aplatonic->scale = pointf3x(.4f,.4f,.4f);
	aplatonic->flags|=TF_CASTSHADOW;
	masters1.push_back(aplatonic);

	aplatonic = buildhexahedron("maptestnck.tga","pdiffspec","a hexahedron");
	aplatonic->scale = pointf3x(.4f,.4f,.4f);
	aplatonic->flags|=TF_CASTSHADOW;
	masters1.push_back(aplatonic);

	aplatonic = buildoctahedron("maptestnck.tga",shadernameround,"buildoctahedron");
    aplatonic->scale = pointf3x(.4f,.4f,.4f);
	aplatonic->flags|=TF_CASTSHADOW;
	masters1.push_back(aplatonic);

	aplatonic = builddodecahedron("maptestnck.tga","pdiffspec","a dodecahedron");
	aplatonic->scale = pointf3x(.4f,.4f,.4f);
	aplatonic->flags|=TF_CASTSHADOW;
	masters1.push_back(aplatonic);

	aplatonic = buildicosahedron("maptestnck.tga","pdiffspec","an icosahedron");
	aplatonic->scale = pointf3x(.4f,.4f,.4f);
	aplatonic->flags|=TF_CASTSHADOW;
	masters1.push_back(aplatonic);

    tree2* many1;
    many1=new tree2("many");
    many1->setvis(true);
    newcastle->linkchild(many1);

	mt_setseed(curtime); // set seed to current time float to get more random
    numberof=5; /// number of objects cubed
	for (k=0;k<numberof;++k) {
		for (j=0;j<numberof;++j) {
			for (i=0;i<numberof;++i) {
				el = mt_random(masters1.size());
				tree2* p=masters1[el]->newdup(); // tree p is local to what, the init() function ???
				many1->linkchild(p);
				p->trans=pointf3x(1.7f*i,1.7f*j,1.7f*k); // 2.0f distance between each many

#ifdef ALTTEX
				p->alttex=texture_create(rndtex[mt_random(NRNDTEX)]);
				if (texture_getrc(p->alttex)==1) {
					p->alttex->load();
				}
#endif

#ifdef DOROTVEL
				p->rotvel.x=mt_frand()*.005f;
				p->rotvel.y=mt_frand()*.005f;
#endif
//				p->flags|=TF_TREECOLOR; // NOTE THIS FLAG USAGE
#ifdef ALTCOLOR
				p->treecolor=pointf3x(mt_frand(),mt_frand(),mt_frand(),1.f); // .65f alpha set to semi transparent
#endif
                many1->trans = pointf3x(2.75f,1.5f,52.5f);
//                p->trans = pointf3x(6,5,31); // weird, but very interesting looking BUG that creates all objects in one place.
			}
		}
	}

/// viewport
	mainvp.backcolor=C32BLACK;
	mainvp.zfront=0.025f; // .25f clipping plane front and back
	mainvp.zback=1000; // 400
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER; // clearing the background, w buffer, checkered background in software mode
	mainvp.xsrc=WX;//4; // not used much.
	mainvp.ysrc=WY;//3;
	mainvp.useattachcam=false; // can attatch cam to an object anywhere in the hierarchy
	mainvp.isortho=false; // true removes all perspective orthographic vs perspective
	mainvp.ortho_size=30; //
/// MOVIE EDITOR
	mainvp.xstart=0;
	mainvp.ystart=0;

// arena1 code for shadowmap is here in init, in tritest1 (and castle) its in draw()
#ifdef USESHADOWMAP
// setup lightviewport
	lightvp.backcolor=C32WHITE;
	lightvp.zfront=-200;
	lightvp.zback=200;
//	lightvp.camzoom=.875f;//3.2f; // it'll getit from tree camattach if you have one
	lightvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
	lightvp.xsrc=1;
	lightvp.ysrc=1;
	lightvp.useattachcam=false;
//	lightvp.lookat=0;
	lightvp.isortho=true;
	lightvp.ortho_size=25;
	lightvp.xstart=0;
	lightvp.ystart=0;
	lightvp.xres=SHADOWMAP_SIZE;
	lightvp.yres=SHADOWMAP_SIZE;
#endif


// arena1
// build a ground
#ifdef USESHADOWMAP
	tt=buildgroundmodel("backgnd","useshadowmap");
	tt->trans.y= -.001;
#else
	tt=buildgroundmodel("backgnd","pdiffspec");
#endif
	roottree->linkchild(tt);
//    tt->treecolor = pointf3x(.1f,.1f,.1f); // i added

// arena1
#ifndef MINIMAL
#ifdef VIEWSHADOWMAP
// build shadowmap viewer
	tt=buildshadowmapviewermodel("smv","shadowmapviewer");
	tt->trans.y=6;
	tt->trans.x=-6;
	tt->trans.z=0;
	tt->rot.x=-PI/2;
	roottree->linkchild(tt);
#endif
#endif

// keep track of current video device
	togvidmode=videoinfo.video_maindriver; // not important

/// lights
	dolights(); // this will set ambient and directional light to default colors on first call only
	// save default lights
	ambcolsave = lightinfo.ambcolor;
	dircolsave = lightinfo.lightcolors[0];
	// change default lights
	lightinfo.ambcolor = pointf3x(1.f,1.f,1.f); // not working ???
	lightinfo.lightcolors[0] = pointf3x(.075f,.075f,.075f); // not working ???
/// fog
    videoinfodx9.fog.enable = true;
    videoinfodx9.fog.mode = 3;
    videoinfodx9.fog.density = 0;
    videoinfodx9.fog.start = 0;
    videoinfodx9.fog.end = 60;
    videoinfodx9.fog.color = pointf3x(1.f,0,0);




//	lightinfo.deflightpos[0] = pointf3x(0,0,-10);

// sound
	abacksndplayer->settrack(7);

	popdir(); // this one is associated with the one above the declar of shadernameflat and round above

/// declaring a model from a .bwo file
//from vidtest.cpp
    C8 dir1[MAX_PATH]="racenetdata/tarpits";
	pushandsetdir(dir1);

    C8 fname[MAX_PATH] = "tiger.bwo";
	tree2* tiger = new tree2(fname); // a name with a .bwo or .bws will load and build them
	roottree->linkchild(tiger);
    tiger->scale = pointf3x(.05f,.05f,.05f);
    tiger->rot = pointf3x(0,PI+PI/2,0);
    tiger->trans = pointf3x(25,4.1f,30);

    C8 fname1[MAX_PATH] = "tiger01.bwo";
	tree2* tiger01 = new tree2(fname1); // a name with a .bwo or .bws will load and build them
	roottree->linkchild(tiger01);
    tiger01->scale = pointf3x(.05f,.05f,.05f);
    tiger01->rot = pointf3x(0,PI+PI/2,0);
    tiger01->trans = pointf3x(30,4.1f,30);

    C8 fname2[MAX_PATH] = "tiger02.bwo";
	tree2* tiger02 = new tree2(fname2); // a name with a .bwo or .bws will load and build them
	roottree->linkchild(tiger02);
    tiger02->scale = pointf3x(.05f,.05f,.05f);
    tiger02->rot = pointf3x(0,PI+PI/2,0);
    tiger02->trans = pointf3x(35,4.1f,30);

	popdir();

    C8 dir2[MAX_PATH]="fortpoint";
	pushandsetdir(dir2);


    logger ("fortpoint ptr = %p\n", fortpoint);
    fortpoint = new tree2("fp7opt.BWS");
    bwofile = new tree2("bwofile");
    logger ("bwofile ptr = %p\n", bwofile);
    bwofile = fortpoint->find("Bunk_bed.bwo");
//    logger ("bwo file exists = %d\n", fileexist("Bunk_bed.bwo"));
	bunkbeddup = new tree2("bunkbeddup");
    bunkbeddup = bwofile->newdup();
	roottree->linkchild(bunkbeddup);
//    bunkbeddup->scale = pointf3x(2,2,2);
//    bunkbeddup->rot = pointf3x(0,PI+PI/2,PI); // PI+PI/2
//    bunkbeddup->trans = pointf3x(21,2,9);

	popdir();


} /// /// ///

void castleproc()
{
/// SOUND
    // sound plays partially at start, at load and then only when decrementing frameslider or rev play
//    if(vectorA.size() == (FRAMESLIDER->getidx())) //ERROR curvector.end() OR *(curvector.size()) == FRAMESLIDER->getidx())
//        playasound(twh);
//    if(mainvp.camtrans.x > 30.f) // not working
//        playasound(twh);
//    if (FRAMESLIDER->getidx() == FRAMESLIDER->getidx()/2) // not working
//        playasound(twh);


/// MOVIE EDITOR
//    vectorA.size() = vectorA.size(); // check size once each proc cycle // NLU
//    vectorB.size() = vectorB.size();

// the nested tests of each shape within each state
	shape* over=rl->getover();
	if (wininfo.mleftclicks) // focus is on the object
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over); // blends colors as you hover over it

    drawtextque_format_foreback(SMALLFONT,1000,700,F32WHITE,F32BLACK,"UI STATE = %d", UIstates);
    shape* focusa = 0;
    // THE UI
	if (focus) { // clicking left button, ui makes the object do something
		S32 ret=focus->procfocus(); // return non-neg number if action has happened in the whole ui
		focusa = ret>=0 ? focus : 0; // that special in line if/else called what now ???
//	}
        // INSIDE OF THE UI
        switch(UIstates){
    ///
            case IDLE: // check all the buttons and sliders
                updatePlaySlidertext(); // put these here to guarantee that they are updated at return to IDLE
                updateFrameSlidertext();
                if (focusa==QUIT){ // cancel button
                    poporchangestate(STATE_MAINMENU);

                } else if (focusa==SELECTFILE) { // if click a file in list box its name is settname to the text bar above
                    TEXTSELECTFILE->settname(SELECTFILE->getidxname());

                } else if (focusa==SAVEFILENAME) {
                    ; // ?????????????
                }

                else if (focusa==REC){  // record button 30 FPS
                    UIstates = RECORD;
                }

                else if (focusa==PLAY){ // play button 30 FPS
                    UIstates = PLAYIT;

                } else if (focusa==PLAYSLIDER) { // play slider
                    UIstates = PLAYIT;

//                } else if (focusa == STOP) {
//                    UIstates = IDLE;

                } else if (focusa==FRAMESLIDER) { // frame slider
                    *curvectori = FRAMESLIDER->getidx()-1;
                    mainvp.camtrans = curvector->at(*curvectori).cp_camtrans;
                    mainvp.camrot = curvector->at(*curvectori).cp_camrot;
                    UIstates = IDLE;

                } else if (focusa==VECTOR) { // vector selector // name set to 'A' at startup in UI.txt, vectorAorB = 0 at startup.
                    if(vectorAorB){ // very confusing.  ie if previously in B go to A
                        curvector = &vectorA;
                        curvectori = &vectorA_i;
                        VECTOR->settname("A");
                        FRAMESLIDER->setminmaxval(1,curvector->size());
                        FRAMESLIDER->setidx(*curvectori+1);
                    }
                    else{  // if previously in A go to B
                        curvector = &vectorB;
                        curvectori = &vectorB_i;
                        VECTOR->settname("B");
                        FRAMESLIDER->setminmaxval(1,curvector->size());
                        FRAMESLIDER->setidx(*curvectori+1);
                    }
                    updateFrameSlidertext();
                    updatePlaySlidertext(); // why is this here ???
                    vectorAorB = !vectorAorB;

                } else if (focusa==CLEAR) { // vector selector
                        curvector->clear();
                        *curvectori = 0;
                        updateFrameSlidertext();
                        FRAMESLIDER->setidx(1);

                } else if (focusa==LOAD) { // needs load from M or S button
                    const char* loadfilename = SELECTFILE->getidxname(); // display current highlighted file on text bar AND save it to loadfilename
                    if (loadfilename)
                        TEXTSELECTFILE->settname(loadfilename);
                    pushandsetdir("1mydata");
                    if((fpin = fopen(loadfilename, "rb")) != NULL){ // if file exists
                        curvector->clear(); // added this to prevent a reload on top of what was already in the vector
                        while (fread(&cp, sizeof(capture_path), 1, fpin)){
                            curvector->push_back(cp);
                        }
/// add range function here rick says
                        fclose(fpin);
                    }
                    popdir();
                    FRAMESLIDER->setminmaxval(1,curvector->size()); // should be if(vectorB.size())
                    updateFrameSlidertext();
                    FRAMESLIDER->setidx(1);
                    *curvectori = 0;

                }else if (focusa==SAVE) { // // needs save to either M or S button
                    pushandsetdir("1mydata");
                    const char* tempconstcharptr = SAVEFILENAME->gettname(); // |3472|error: invalid conversion from 'const C8* {aka const char*}' to 'char*' [-fpermissive]|
                    unsigned int temp = strlen(tempconstcharptr); // get length of it
                    int i = 0;
                    char tempcharptr [100]; // to concat the extension ".me"
                    while((temp)>0){
                        tempcharptr[i++] = *tempconstcharptr++; // copy each element over to char*, note dereference
                        --temp;
                    }
                    tempcharptr[i] = '\0'; // added this to prevent multiple extensions. s/b after strcat???
                    strcat(tempcharptr, ".me"); // add extension. strcat removes null, then adds it after ext.
                    char* savefilename = tempcharptr; // init the final const char* and send it to fopen(savefilename, "wb") in castle....

                    fpout = fopen(savefilename, "wb");
                    for (*curvectori=0; *curvectori < curvector->size(); *curvectori++){
                        fwrite(&curvector[U32(*curvectori)], sizeof(capture_path), 1, fpout);  // &vectorA is wrong. use & on arrays only, not vectors
                    }
                    fclose(fpout);
                    popdir();
                    SELECTFILE->addstring(savefilename); // update the list box

                } else if (focusa==INSERT) {
                    insertinto = 1; // !insertinto;
                    UIstates = EDIT;

                } else if (focusa==ERASE) {
                        if(curvector->size()){ // if there is a size
                            erasefrom = 1; //
//                            selectcount = 0; // not needed here, is in proc_edit
                            UIstates = EDIT;
                        }

                } else if (focusa==FRAMESELECT) {
                    if(!selectcount){
                        eraseframe1 = FRAMESLIDER->getidx();
                        selectcount = 1;
                    }
                    else{
                        eraseframe2 = FRAMESLIDER->getidx();
                        selectcount = 0;
                    }
                }

                break; // from IDLE case
    ///
            case PLAYIT:
                if (focusa == STOP)
                    UIstates = IDLE;
                break;
    ///
            case RECORD:
                if (focusa == STOP || wininfo.mrightclicks)
                    UIstates = IDLE;
                break;
    ///
            case EDIT:
                if (focusa == STOP)
                    UIstates = IDLE;
                break;
        }
    }
///
    switch(UIstates){ // OUTSIDE OF THE UI
        case IDLE: // IDLE IS NOT USED HERE
            break;
        case PLAYIT:
            treeinfo.flycam = 0;
            proc_play(); //
            break;
        case RECORD:
            proc_record();
            break;
        case EDIT:
            proc_edit();
            break;
    }
/// END MOVIE EDITOR


/// tritest1
	normalmaptree->treecolor.w=userfloata;

/// ricks many.cpp
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
/// keyboard
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
		changestate(STATE_CASTLE);
		break;
// ricks newest path function
	case 'n': // change path function to next
		++curpath;
		if (curpath >= npathfunc)
			curpath = 0;
		curtime = 0;
		break;
	case 'p': // change path function to previous
		--curpath;
		if (curpath < 0)
			curpath = npathfunc - 1;
		curtime = 0;
		break;
	case '=':
		changeglobalxyres(1);
		changestate(STATE_CASTLE);
		break;
	case '-':
		changeglobalxyres(-1);
		changestate(STATE_CASTLE);
		break;
	case 'k':
		mainvp.useattachcam ^= 1;
		break;
	case 't': // reset time
		curtime = 0;
		break;
	case 'R': // same as record button
		UIstates = RECORD;
		break;
	case 'S': // same as stop button
		UIstates = IDLE;
		break;
    case 'w':
        playasound(twh);
		break;
	}

///
	if (wininfo.mmiddleclicks) { // change vid from windod to full screen
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_CASTLE);
	}
//    mainvp.useattachcam = true; // overides the debp variable // commented out so 'k' works in castle.
//    mainvp.camtrans = pointf3x(0,0,.001f);  // CAMERA POSIT ION !!! gets rid of spotlight ring but w/res
	mainvp.xres=WX; // set viewport to accomodate the change above. grow winfo
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	doflycam(&mainvp); // enables 'c' key arrow keys, and mouse l r buttons, and move mouse-- to move camera
///
//    treeinfo.flycam = 1;
	roottree->proc(); // func proc is a member of struct roottree

/// ricks newest path function code
	//pathforwardlookfront(curtime,me->trans,me->rot);
/// ???
	if (curtime >= maxtime) // check first, incase debprint modified these
		curtime -= maxtime;
	if (curtime < 0)
		curtime += maxtime;

	//pathforwardlookfront(curtime,me->trans,me->rot);
//    logger("curtime=%f",curtime);
	pathfunc* cpf = pathfuncs[curpath];
	cpf(curtime,me->trans,me->rot);

/// code to adjust orientation of jack as camera moves
//    jack->rot.x = -(me->rot.x);
//    jack->rot.y = -(me->rot.y);
//    jack->rot.z = -(me->rot.z);

/// my code to open and then close one door. cant get it to close ???
/*
    if (hinged_door->rot.y == 0 || hinged_door->rot.y < PI/2)
        hinged_door->rot.y += PI/256;
    else
        hinged_door->rot.y == 0;
//        if (hinged_door->rot.y <= PI/2 )
  //          if (hinged_door->rot.y != 0)
    //            hinged_door->rot.y -= PI/256;
*/

/// ricks code for opening, pausing, closing, and pausing the door, each done during an even percentage of maxtime
    if (curtime<maxtime/4.f) // use this code to set curtime< anytime
        hinged_door->rot.y = (curtime * PI/2) / (maxtime/4.f);
    else if (curtime < maxtime/2.f)
        hinged_door->rot.y = PI/2;
    else if (curtime < 3*maxtime/4.f)
        hinged_door->rot.y = ((maxtime * 3/4) - curtime) * PI/2 / (maxtime/4.f);
    else
        hinged_door->rot.y = 0;
/*
    if (curtime<maxtime/4.f) // ricks original code
        hinged_door->rot.y = (curtime * PI/2) / (maxtime/4.f);
    else if (curtime < maxtime/2.f)
        hinged_door->rot.y = PI/2;
    else if (curtime < 3 * maxtime/4.f)
        hinged_door->rot.y = ((maxtime * 3/4) - curtime) * PI/2 / (maxtime/4.f);
    else
        hinged_door->rot.y = 0;
*/
/// draw our own debug text on screen
// ricks newest path function code now has 'n' next and 'p' previous, and also 'r' to reset curtime. i added it here.
	drawtextque_format_foreback(SMALLFONT,0,10,F32WHITE,F32BLACK,"'n' to change to next path, 'p' to change to previous path, curpath %d/%d, curtime = %f, maxtime = %f",curpath+1,npathfunc,curtime,maxtime); // 2d print in a 3d world
//	drawtextque_format_foreback(SMALLFONT,0,10,F32WHITE,F32BLACK,"'p' to change path, curpath %d/%d, curtime = %f",curpath+1,npathfunc,curtime); // 2d print in a 3d world
	drawtextque_format_foreback(SMALLFONT,0,50,F32WHITE,F32BLACK,"'k' to change attachcam"); // 2d print in a 3d world
	drawtextque_format_foreback(SMALLFONT,0,90,F32WHITE,F32BLACK,"'b' to change background track, current track = %d",abacksndplayer->gettrack());
	drawtextque_format_foreback(SMALLFONT,0,130,F32WHITE,F32BLACK,"position of camera = %f, %f, %f rotation of camera = %f, %f, %f",mainvp.camtrans.x,mainvp.camtrans.y,mainvp.camtrans.z,mainvp.camrot.x,mainvp.camrot.y,mainvp.camrot.z); // me->trans.x,me->trans.y,me->trans.z,me->rot.x,me->rot.y,me->rot.z
	drawtextque_format_foreback(SMALLFONT,0,170,F32WHITE,F32BLACK,"'t' to reset curtime");
	drawtextque_format_foreback(SMALLFONT,0,210,F32WHITE,F32BLACK,"vectorA.size() = %d, vectorB.size() = %d",vectorA.size(), vectorB.size());
	drawtextque_format_foreback(SMALLFONT,0,230,F32WHITE,F32BLACK,"vectorA_i = %f, vectorB_i = %f",vectorA_i,vectorB_i);
	drawtextque_format_foreback(SMALLFONT,0,250,F32WHITE,F32BLACK,"frame slider idx number = %f",FRAMESLIDER->getidx());
	drawtextque_format_foreback(SMALLFONT,0,330,F32WHITE,F32BLACK,"insertframe = %d",insertframe);
	drawtextque_format_foreback(SMALLFONT,0,350,F32WHITE,F32BLACK,"eraseframe1 = %d",eraseframe1);
	drawtextque_format_foreback(SMALLFONT,0,370,F32WHITE,F32BLACK,"eraseframe2 = %d",eraseframe2);
	drawtextque_format_foreback(SMALLFONT,0,400,F32WHITE,F32BLACK,"gettname = %s",SAVEFILENAME->gettname());
//	drawtextque_format_foreback(SMALLFONT,0,420,F32WHITE,F32BLACK,"tempconstcharptr = %s",tempconstcharptr);
//	drawtextque_format_foreback(SMALLFONT,0,440,F32WHITE,F32BLACK,"tempcharptr = %s",tempcharptr);
//	drawtextque_format_foreback(SMALLFONT,0,460,F32WHITE,F32BLACK,"savefilename = %s",savefilename);
//	drawtextque_format_foreback(SMALLFONT,0,480,F32WHITE,F32BLACK,"loadfilename = %s",SELECTFILE->getidxname());
//	drawtextque_format_foreback(SMALLFONT,100,330,F32WHITE,F32BLACK,"test = %d",test);
// ricks newest path function code comments this out and moves the ifs up a bit
//	curtime += timespeed*wininfo.framestep/INITFPS; // timespeed*wininfo.framestep/INITFPS; // timespeed/INITFPS;
/*	if (curtime >= maxtime)
		curtime -= maxtime;
	if (curtime < 0)
		curtime += maxtime;
*/

// ricks newest path function code comments this out and moves this here
	curtime += timespeed/*wininfo.framestep*//INITFPS;
}

void castledraw3d()
{
	video_buildworldmats(roottree); // generate matrices
	video_setviewport(&mainvp); // clear zbuf etc. apply flags from above
	dolights(); // 2nd occurence of dolights() is not default. 1st is in castleinit()
/*
// tritest1 causes scene to go black but drawtextcue still works
#if 1
#ifdef USESHADOWMAP
//#ifdef SHADOWMAP
	if (video_rendertargetshadowmap()) {
//		lightvp.camtrans=lightinfo.worldlightpos[0];
//		lightvp.camrot=lightinfo.worldlightdirs[0];
		lightvp.usev2w=true;
		lightvp.v2w=lightinfo.light2world[0];
//		video_beginscene(); // clear zbuf etc.
//	lightvp.xstart=0;
//	lightvp.ystart=0;
//	lightvp.xres=WX;
//	lightvp.yres=WY;
		video_setviewport(&lightvp); // clear zbuf etc.
//		logger("rendertarget set to shadowmap\n");
		video_drawscene(roottree);
//		video_endscene(0); // nothing right now
		video_rendertargetmain();
	}
//#endif
#endif
#endif
*/

// arena1
	if (video_rendertargetshadowmap()) {
		lightvp.usev2w=true; // default true. light above that rotates. if false its front fixed light.
		lightvp.v2w=lightinfo.light2world[0];
		video_setviewport(&lightvp); // clear zbuf etc. // first render pass with lightvp
		video_drawscene(roottree);
		video_rendertargetmain();
	}
	video_setviewport(&mainvp); // clear zbuf etc. // second render pass with mainvp
    // draw the 3d first
	video_drawscene(roottree); // draw the roottree
/// MOVIE EDITOR
	// draw the ui next, in front of the 3d
	video_sprite_begin(
	  SPTRESX,SPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  0,
	  0);
	rl->draw3d(); // draw the whole UI in front of the 3d
	//video_sprite_draw(trackpic,F32WHITE, 25,166,140,105);
	video_sprite_end();

}

void castleexit() // free resources when escape or x clicked
{
/// SOUND
//    playasound(twh);
    wave_unload(twh);
/// MOVIE EDITOR

// from testres3d
	mainvp.flags=VP_CLEARWB|VP_CLEARBG;
	delete rl; //  the UI
	exit_res3d();
/// MOVIE EDITOR
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

	U32 i; /// ricks many.cpp code
	for (i=0;i<masters.size();++i)
		delete masters[i];
	masters.clear();

	for (i=0;i<masters1.size();++i)
		delete masters1[i];
	masters1.clear();
///

	delete roottree; // deallocate all memory
	delete fortpoint;
	delete bwofile;
//	delete bunkbeddup;

	logger("logging reference lists after free\n");
	logrc();
	delete abacksndplayer;


}

