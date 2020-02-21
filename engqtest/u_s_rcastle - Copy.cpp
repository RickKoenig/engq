/// castle //////////////////////////////////////

#define D2_3D

#include <m_eng.h>//the engine
#include "u_states.h"// main include file for whole engine
#include "u_modelutil.h" // helps build 3d models
#include "d2_font.h" // font queue


// these vars are static, ie private, global and permanent to this file.
static U32 togvidmode; // toggles vid mode with center mouse button
//hierarchy
static tree2* roottree; // for 3d tree2 (a class) is most imp data struct in the engine
static tree2* sky;
static tree2* ground;
static tree2* castle;
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
static tree2* ring;
static tree2* c;
static tree2* mycylinder;
static tree2* mytorus;
static tree2* me;

//static float specpowval = 0;//500;

// animation section, could be an animation class
static float curtime; // in seconds
static float timespeed; // 1 for 1 curtime unit in seconds, 2 for faster, -1 backwards, 0 stop etc.
static float maxtime = 30; // in seconds, before loops
static S32 curpath;


// some path functions

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
	pos = pointf3x(-1,-2,2.5f);
	rot = pointf3x(0,-PI/4.0f,0);
}

typedef void pathfunc(float t,pointf3& pos,pointf3& rot);
static pathfunc* pathfuncs[] = {
	pathforwardlookfront,
	pathforwardlookat,
	pathforwardlookatflyup,
	nopath,
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

static void changematerialcolor(tree2* t,pointf3 newcolor)
{
	if (t->mod)
		t->mod->mats[0].color = newcolor;
}

void rcastleinit() // build state before it runs
{
	curtime = 0;
	timespeed = 1; // 1 second per 1 second
	curpath = 2;


	//specpowval = 50; // 500

	extradebvars(edv,nedv);

	dolights();
	//lightinfo.dodefaultlights=true; // was true. optional in this state. no change if false

	video_setupwindow(GX,GY); // resolution of window of and on main menu

	pushandsetdir("gfxtest"); // default dir is data. new dir is this one

	const C8* shadernameflat = "spotlight";
	const C8* shadernameround = "pdiffspec";

//	mt_setseed(24);

/// build hierarchy
    roottree=new tree2("roottree"); // creating roottree(a huge class)
    roottree->trans=pointf3x(0,0,0); // new change we made as an alternative to k+x below. it moves camera back (+z)

	// CAMERA attached to tree
#if 0
	//me = new tree2("me");
	tree2* merot = buildcylinder_xz(pointf3x(.5f,1,.5f),"maptestnck.tga","omni");
	merot->rot.x = PI*.5f;
	merot->trans.z = -.5f;
	//me = buildsphere(.5,"shiney_copper_bumpy_wet.jpg", shadernameround);
	merot->name = "merot";
	//me = new tree2("me");
	me = new tree2("me");
	//me = buildsphere(.5,"shiney_copper_bumpy_wet.jpg", shadernameround);
	me->name = "me";
	me->linkchild(merot);
#else
	me = buildspotlight_xy(PI*.5f,1.5,2,"cvert"); // white ghostly model of cone shaped spotlight
	me->treedissolvecutoff = .001f;
#endif
	//me->trans = pointf3x(0,0,-10);
	roottree->linkchild(me);
	mainvp.camattach = me;
	mainvp.useattachcam = true;
	

	// LIGHTS attached to tree
	// ambient
	tree2* amb = new tree2("amb");
	amb->flags |= TF_ISAMBLIGHT|TF_ISLIGHT;
	amb->lightcolor = F32LIGHTBLUE;
	roottree->linkchild(amb);
	addlighttolist(amb);

	// directional
	tree2* dir = new tree2("dir");
	dir->flags |= TF_ISLIGHT;
	dir->lightcolor = F32LIGHTRED;
	//dir->rotvel.y = .02f;
	//roottree->linkchild(dir);
	me->linkchild(dir);
	addlighttolist(dir);


    pushandsetdir("skybox");
    sky=buildskybox(pointf3x(1,1,1), "ts18_2crop.jpg","tex");
    sky->name="sky";
    popdir();
    roottree->linkchild(sky);

    ground=buildplane_xy(pointf2x(100.,100.),"rusty.jpg", shadernameflat);
    ground->name="ground";
    ground->rot=pointf3x(PI/2,0,0);
    ground->trans=pointf3x(0,-5,0);
    roottree->linkchild(ground);

    castle=new tree2("castle");
    roottree->linkchild(castle);

    wall1=new tree2("wall1");

//    myfloor=new tree2("myfloor");


    wp4x10a=buildprism(pointf3x(1,7,4),"stone_wall.jpg",shadernameflat);
    wp4x10a->name="wp4x10a";
    wall1->linkchild(wp4x10a);
    wp4x10a->trans=pointf3x(0,0,0);
	changematerialcolor(wp4x10a,pointf3x(1,1,1,.5f)); // make translucent

    wp4x10b=buildprism(pointf3x(1,7,4),"stone_wall.jpg",shadernameflat);
    wp4x10b->name="wp4x10b";
    wall1->linkchild(wp4x10b);
    wp4x10b->trans=pointf3x(0,0,-7);

    wp11x3=buildprism(pointf3x(1,3,11),"stone_wall.jpg",shadernameflat);
    wp11x3->name="wp11x3";
    wall1->linkchild(wp11x3);
    wp11x3->trans=pointf3x(0,5,-3.5f);

    wall1->trans=pointf3x(0.f-5.5f,0,0+4);

    wall2=wall1->newdup();
//                wall2->name="wall2"; // works, but is undesirable. name s/b the orig name of duplicate.
    wall2->rot=pointf3x(0,PI/2,0);
    wall2->trans=pointf3x(9.5f-5.5f,0,1.5f+4);

    wall3=wall1->newdup();
    wall3->trans=pointf3x(11.f-5.5,0,-1.f+4);

    wall4=wall1->newdup();
    wall4->rot=pointf3x(0,PI/2,0);
    wall4->trans=pointf3x(8.5f-5.5f,0.f,-9.5f+4);

    roof=buildprism(pointf3x(1,10,10),"stone_wall.jpg",shadernameflat);
    roof->name="roof";
    roof->rot=pointf3x(0,0,PI/2);
    roof->trans=pointf3x(5.5f-5.5f,4.f,-4.f+4); // translate object to here

    myfloor=buildprism(pointf3x(1,10,10),"stone_wall.jpg",shadernameflat);
    myfloor->name="myfloor";
    myfloor->rot=pointf3x(0,0,PI/2);
    myfloor->trans=pointf3x(5.5-5.5f,-3.f,-4.f+4); // translate object to here

    magicball=buildsphere(.5,"maptestnck.png", shadernameround);
    magicball->name="magicball";
    magicball->treecolor=pointf3x(1,1,0);
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

    ring=buildtorus_xz(1,.1f,"white.pcx",shadernameround);
    ring->name="ring";
    ring->treecolor=pointf3x(1,.85f,0); // gold
    ring->rot=pointf3x(PI/2,0,0);
    ring->trans=pointf3x(0,0,0);
    ring->rotvel.x= -.02f;
    ring->rotvel.y= -.02f;
    ring->rotvel.z= -.02f;

	c=buildcylinder_xz(pointf3x(.5f,1,.5f),"white.pcx",shadernameround);
	c->trans = pointf3x(0,-.5f,0); // center cylinder
	mycylinder = new tree2("cyl_root");
	mycylinder->linkchild(c);
    mycylinder->name="mycylinder";
//    mycylinder->treecolor=pointf3x(0,1,0); // doesn't work
//    c->treecolor=pointf3x(0,1,0); // doesn't work
    mycylinder->rot=pointf3x(0,0,0);
    mycylinder->trans=pointf3x(3,0,-3);
    mycylinder->rotvel.x= -.02f;
    mycylinder->rotvel.y= -.02f;
    mycylinder->rotvel.z= -.02f;

    mytorus=buildtorus_xz(.25f,.125f,"white.pcx",shadernameround); // ? size
    mytorus->name="mytorus";
    mytorus->treecolor=pointf3x(1,0,1);
    mytorus->rot=pointf3x(PI/2,0,0);
    mytorus->trans=pointf3x(-3,0,3);
    mytorus->rotvel.x= -.02f;
    mytorus->rotvel.y= -.02f;
    mytorus->rotvel.z= -.02f;

    castle->linkchild(wall1);
    castle->linkchild(wall2);
    castle->linkchild(wall3);
    castle->linkchild(wall4);
    castle->linkchild(roof);
    castle->linkchild(myfloor);
    castle->linkchild(magicball);
    castle->linkchild(mycube);
    castle->linkchild(ring);
    castle->linkchild(mycylinder);
    castle->linkchild(mytorus);

    castle->trans=pointf3x(0,0,0);
    roottree->trans=pointf3x(0,0,0);

//    castle->rotvel.x=.01; //mt_frand()*.1f;//.005f; // 0 to 1 adjusts speed. + or - for direction.
    //castle->rotvel.y=.01; //mt_frand()*.01f;//.005f;
//    castle->rotvel.z=.01; //mt_frand()*.1f;//.005f;
	//castle->scalevel = pointf3x(1.002,1.002,1.002);

// setup viewport
	mainvp.backcolor=C32BLACK;
	mainvp.zfront=0.025f; // .25f clipping plane front and back
	mainvp.zback=1000; // 400
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER; // clearing the background, w buffer, checkered background in software mode
	mainvp.xsrc=WX;//4; // not used much.
	mainvp.ysrc=WY;//3;
	//mainvp.useattachcam=false; // can attatch cam to an object anywhere in the hierarchy
	mainvp.isortho=false; // true removes all perspective orthographic vs perspective
	mainvp.ortho_size=30; //

// keep track of current video device
	togvidmode=videoinfo.video_maindriver; // not important
// lights
	dolights(); // this will set ambient and directional light to default colors on first call only
	// save default lights
	ambcolsave = lightinfo.ambcolor;
	dircolsave = lightinfo.lightcolors[0];
	// change default lights
	lightinfo.ambcolor = pointf3x(.35f,.35f,.35f); // .125f,.125f,.125f
	lightinfo.lightcolors[0] = pointf3x(.75f,.75f,.75f); // .5f,.5f,.5f

//	lightinfo.deflightpos[0] = pointf3x(0,0,-10);

	popdir(); // puts us back in the previous folder
}

void rcastleproc()
{
    /*ground->mod->mats[0].specpow = specpowval;
    wp11x3->mod->mats[0].specpow = specpowval;
    wp4x10a->mod->mats[0].specpow = specpowval;
    wp4x10b->mod->mats[0].specpow = specpowval;
    myfloor->mod->mats[0].specpow = specpowval;
    roof->mod->mats[0].specpow = specpowval;
    magicball->mod->mats[0].specpow = 50;
    mycube->mod->mats[0].specpow = 50;
    mytorus->mod->mats[0].specpow = 50;
//    c->mod->mats[0].specpow = 50;
*/

	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU); // back to main menu
		break;
	case 'a': // show it
		showcursor(1);
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
	case 'p': // change path function
		++curpath;
		if (curpath >= npathfunc)
			curpath = 0;
		break;
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

	mainvp.xres=WX; // set viewport to accomodate the change above. grow winfo
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	doflycam(&mainvp); // enables arrow keys, and mouse l r buttons, and move mouse-- to move camera
	roottree->proc(); // func proc is a member of struct roottree


	//pathforwardlookfront(curtime,me->trans,me->rot);
	pathfunc* cpf = pathfuncs[curpath];
	cpf(curtime,me->trans,me->rot);

	drawtextque_format_foreback(LARGEFONT,0,10,F32RED,F32BLACK,"'p' to change path, curpath %d/%d, curtime = %f",curpath+1,npathfunc,curtime); // 2d print in a 3d world
	drawtextque_format_foreback(LARGEFONT,0,50,F32RED,F32BLACK,"'k' to change attachcam"); // 2d print in a 3d world

	curtime += timespeed*wininfo.framestep/INITFPS;
	if (curtime >= maxtime)
		curtime -= maxtime;
	if (curtime < 0)
		curtime += maxtime;
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
	lightinfo.lightcolors[0] = dircolsave;
	lightinfo.deflightpos[0] = pointf3x();
	extradebvars(0,0);
	roottree->log2();
	logger("logging reference lists\n");
	logrc(); // log reference counter

	delete roottree; // deallocate all memory
	logger("logging reference lists after free\n");
	logrc();
}

