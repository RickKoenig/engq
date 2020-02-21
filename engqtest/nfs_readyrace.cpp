// state just before launching game

#include <m_eng.h>
#include "u_states.h"

#include "nfs_utils.h"
#include "nfs_shell.h"
#include "nfs_choose.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace nfs_readyrace {
#endif

S32 frame;
tree2* roottree;
textureb* backpic;
fontq* nfsfont;

struct item {
	float x,y;
	float sx,sy;
	C8* text;
};

vector<item> chitems;

#ifdef USENAMESPACE
} // end namespace
using namespace nfs_readyrace;
using namespace nfs_utils;
using namespace nfs_shell;
using namespace nfs_choose;
#endif

void nfsreadyrace_init()
{
	chitems.clear();
	item itm = {640/2,50,400,20,"Go!"};
	chitems.push_back(itm);
	//choice = 0;
	frame=0;
// setup state
	//video_setupwindow(GX,GY);//,565);
//	video_setupwindow(SX,SY);//,565);
	pushandsetdir("nfs_shell");
// init 3d
	//uselightssave=lightinfo.uselights;
	//lightinfo.uselights=0;
	pushandsetdir("gfxtest");
	roottree=new tree2("roottree");
	backpic = texture_create("MostWanted_05_656x369.jpg");
	popdir();
	backpic->load();

	popdir();

	pushandsetdir("fonts");
	nfsfont = new fontq("beat");
	popdir();
	
	//GetWindowText((HWND)wininfo.MainWindow,oldwinname,256);
	//SetWindowText((HWND)wininfo.MainWindow,"NFS Shell");
}

//static struct bitmap32* cbm32;
void nfsreadyrace_proc()
{
// if something selected...
	switch(KEY) {
	case K_ESCAPE:
		changestate(STATE_NFSCHOOSE);
		break;
	case K_RETURN:
	case K_NUMENTER:
		//switch(choice) {
		//case 0:
		changestate(STATE_NFSGAME);
			//break;
		/*case 1:
			changestate(STATE_NFSCHOOSE);
			break;
		}*/
		break;
/*	case K_UP:
	case K_NUMUP:
		choice = range(0,choice-1,nchoice-1);
		break;
	case K_DOWN:
	case K_NUMDOWN:
		choice = range(0,choice+1,nchoice-1);
		break; */
	}
/*	bb -= .06f;
	if (bb < .25f)
		bb = .25f; */
	frame++;
}

void nfsreadyrace_draw3d()
{
	//video_setviewport(&mainvp); // clear zbuf etc.
	video_sprite_begin();
	video_sprite_draw(backpic,pointf3x(.25f,.25f,.25f,1.0f),0,0,640,480);
	nfsfont->setcenter(false);
	nfsfont->print(640/4,125,400,20,F32YELLOW,"%s","Get Ready to Race");
		
	nfsfont->print(640/4 + 25.0f,175 + 25.0f,400 + 25.0f,20,F32LIGHTGRAY,"%s",nfs_players[playeridx].name.c_str());

	nfsfont->setcenter(true);
	S32 i;
	for (i=0;i<(S32)chitems.size();++i) {
		const item& im = chitems[i];
		float hix = im.x;
		pointf3 col = F32GREEN;
		float sx = im.sx;
		float sy = im.sy;

//		if (i == choice) {
			//hix -= 10.0f;
			col = F32WHITE;
			S32 ef = hiliteff(frame);
			sx += ef;
			sy += ef;
//		}
		nfsfont->print(hix - sx/2,im.y - sy/2,sx,sy,col,"%s",im.text);
		nfsfont->setcenter(false);
	}
	video_sprite_end();
}

void nfsreadyrace_exit()
{
	//showcursor(1);
	//SetWindowText((HWND)wininfo.MainWindow,oldwinname);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	delete nfsfont;
	//lightinfo.uselights=uselightssave;
	backpic->rc.deleterc(backpic);
}
