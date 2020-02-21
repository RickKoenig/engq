// the main entry point for the nfs shell

#define INCLUDE_WINDOWS // to change window title
//#define D2_3D
#include <m_eng.h>
//#include <l_misclibm.h>
//#include <vfw.h>
#include "nfs_utils.h"
#include "u_states.h"

#include "nfs_shell.h"
#include "nfs_choose.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace nfs_shell {
#endif

vector<nfs_player> nfs_players;

S32 frame;
tree2* roottree;
S32 uselightssave;
C8 oldwinname[256];
textureb* backpic;
float bb = 1.0f; // brightness effect
fontq* nfsfont;
S32 choice;

struct item {
	float x,y;
	float sx,sy;
	C8* text;
};

vector<item> chitems;

#ifdef USENAMESPACE
} // end namespace nfs_shell
using namespace nfs_shell;
using namespace nfs_utils;
using namespace nfs_choose;
#endif

void nfsshell_init()
{
	playeridx = 0;
	chitems.clear();
	if (nfs_players.empty()) {
		struct nfs_player np;
		np.name = "Bob";
		nfs_players.push_back(np);
		np.name = "Joe";
		nfs_players.push_back(np);
		np.name = "Gabe";
		nfs_players.push_back(np);
	}
	if (!nfs_players.empty()) {
		item itm2 = {640/2,50,400,20,"Choose Racer"};
		chitems.push_back(itm2);
	}
	item itm = {640/2,75,400,20,"Create Racer"};
	chitems.push_back(itm);
	choice = 0;
	showcursor(0);
	//bb = 1.0f;
	frame=0;
// setup state
	video_setupwindow(GX,GY);//,565);
//	video_setupwindow(SX,SY);//,565);
	pushandsetdir("nfs_shell");
// init 3d
	uselightssave=lightinfo.uselights;
	lightinfo.uselights=0;
	pushandsetdir("gfxtest");
	roottree=new tree2("roottree");
	backpic = texture_create("MostWanted_05_656x369.jpg");
	popdir();
	backpic->load();

	popdir();

	pushandsetdir("fonts");
	nfsfont = new fontq("beat");
	popdir();
	
	GetWindowText((HWND)wininfo.MainWindow,oldwinname,256);
	SetWindowText((HWND)wininfo.MainWindow,"NFS Shell");
}

void nfsshell_proc()
{
// if something selected...
	switch(KEY) {
	case K_ESCAPE:
		showcursor(1);
		SetWindowText((HWND)wininfo.MainWindow,oldwinname);
		changestate(STATE_MAINMENU);
		break;
	case K_RETURN:
	case K_NUMENTER:
		switch(choice) {
		case 0:
			changestate(STATE_NFSCHOOSE);
			break;
		case 1:
			changestate(STATE_NFSENTERNAME);
			break;
		}
		break;
	case K_UP:
	case K_NUMUP:
		choice = range(0,choice-1,(S32)chitems.size()-1);
		break;
	case K_DOWN:
	case K_NUMDOWN:
		choice = range(0,choice+1,(S32)chitems.size()-1);
		break;
	}
	bb -= .06f;
	if (bb < .25f)
		bb = .25f;
	frame++;
}

void nfsshell_draw3d()
{
	//video_setviewport(&mainvp); // clear zbuf etc.
	video_sprite_begin();
	video_sprite_draw(backpic,pointf3x(bb,bb,bb,1.0f),0,0,640,480);
	if (bb == .25f) {
		nfsfont->setcenter(true);
		S32 i;
		for (i=0;i<(S32)chitems.size();++i) {
			const item& im = chitems[i];
			float hix = im.x;
			pointf3 col = F32GREEN;
			float sx = im.sx;
			float sy = im.sy;

			if (i == choice) {
				//hix -= 10.0f;
				col = F32WHITE;
				S32 ef = hiliteff(frame);
				sx += ef;
				sy += ef;
			}
			nfsfont->print(hix - sx/2,im.y - sy/2,sx,sy,col,"%s",im.text);
		}
		nfsfont->setcenter(false);
		nfsfont->print(640/4,125,400,20,F32YELLOW,"%s","Leaderboard:");
		for (i=0;i<(S32)nfs_players.size();++i) {
			nfsfont->print(640/4 + 25.0f,125 + 25.0f + 25*i,400 + 25.0f,20,F32LIGHTGRAY,"%s",nfs_players[i].name.c_str());
		}
	}
	video_sprite_end();
}

void nfsshell_exit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	delete nfsfont;
	lightinfo.uselights=uselightssave;
	backpic->rc.deleterc(backpic);
}
