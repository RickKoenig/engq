/// draw 3d and test 3d res
#include <m_eng.h>
#include "u_states.h"

#define RES3D
//#define D2_3D
#include <l_misclibm.h>
//#include <d3d9.h>

#include "u_modelutil.h"

static tree2* roottree,*cub;

static shape* rl;
static shape* focus;

static pbut* PBUTCANCEL;
static hscroll* SLIDENUMP;
static text* TEXTNUMP;
#if 0
listbox* LISTSELTRACKNAME,*RULES,*LISTIP;
listbox* LISTWEAP,*LISTWEAPHAVE;//,*LISTALTWEAP;,,*LISTALTWEAPHAVE;
hscroll* SLIDE2009CARFIRST;
hscroll* SLIDENUMBOTS;
edit* EDITIP,*EDITNAME;
text* TEXTNUMP,*TEXTIP;
text* TEXTWEAPS;//,*TEXTALTWEAPS;
pbut* PIPDEL;
pbut* PLAYBOT,*PLAYSERVER,*PLAYBCLIENT,*PLAYCLIENT;
text* TEXT2009CARFIRST;
text* TEXTNUMBOTS;

#define NUMHSVS 3
hscroll* HUES[NUMHSVS];
hscroll* SATS[NUMHSVS];
hscroll* VALS[NUMHSVS];
text* HUETEXT[NUMHSVS];
text* SATTEXT[NUMHSVS];
text* VALTEXT[NUMHSVS];

hscroll* SLIDERIMS;
text* TEXTRIMS;

hscroll* SLIDEGAMEID;
text* TEXTGAMEID;
hscroll* SLIDEGAMETYPE;
text* TEXTGAMETYPE;
hscroll* SLIDEPLAYERID;
text* TEXTPLAYERID;
#endif
#define SPTRESX 640
#define SPTRESY 480

// set position text and 3d object position depending on the position slider
static void updatePos()
{
	C8 str[50]; // should use 'string' class instead
	S32 rmval = SLIDENUMP->getidx();
//	sprintf(str,"IDX = %d",rmval);
	float xpos = rmval * .1f;
	sprintf(str,"XPOS = %6.3f",xpos);
	TEXTNUMP->settname(str);
	cub->trans.x = xpos;
}

////////////////////////// main
void testres3dinit()
{
	logger("testres3d init\n");
	video_setupwindow(GX,GY);

	// build simple 3d
	roottree=new tree2("roottree");
	roottree->trans = pointf3x(0,0,4);

	// build a cube
	pushandsetdir("gfxtest");
	cub = buildprism(pointf3x(1,1,1),"maptestnck.tga","tex");
	cub->rotvel.y = PI/600;
	popdir();
	roottree->linkchild(cub);

	// build ui
	init_res3d(SPTRESX,SPTRESY);
	pushandsetdir("testres3d");
	rl = res_loadfile("testres3dres.txt");
	popdir();
	focus = 0;
	PBUTCANCEL=rl->find<pbut>("PBUTCANCEL");
	TEXTNUMP=rl->find<text>("TEXTNUMP");
	SLIDENUMP=rl->find<hscroll>("SLIDENUMP");
	SLIDENUMP->setminmaxval(-40,40);
	SLIDENUMP->setidx(18);
	updatePos();

// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.01f;
	mainvp.zback=400;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xsrc=WX;
	mainvp.ysrc=WY;
	mainvp.useattachcam=false;
	mainvp.isortho=false;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	//mainvp.flags=VP_CLEARWB;
}

void testres3dproc()
{
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);

	shape* over=rl->getover();
	if (wininfo.mleftclicks)
		focus=over;
// call over or not over proc
	rl->over_no_over_proc(focus,over);
	if (focus) {
		S32 ret=focus->procfocus();
		shape* focusa = ret>=0 ? focus : 0;
		if (focusa==PBUTCANCEL) {
			//whichgame=NET_BOT;
			poporchangestate(STATE_MAINMENU);
		} else if (focusa==SLIDENUMP) {
			updatePos(); // update text and 3D object
		}
	}
	doflycam(&mainvp);
	roottree->proc();
}

void testres3ddraw3d()
{
	// draw the 3d
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);
	// draw the ui
	video_sprite_begin(
	  SPTRESX,SPTRESY,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	  0,
	  0);
	rl->draw3d();
	//video_sprite_draw(trackpic,F32WHITE, 25,166,140,105);
	video_sprite_end(); 
}

void testres3dexit()
{
	//mainvp.flags=VP_CLEARWB|VP_CLEARBG;
	delete rl;
	exit_res3d();
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	logger("logging reference lists after free\n");
	logrc();
}
