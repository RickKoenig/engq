// pick the player to race

#include <m_eng.h>

#include "nfs_utils.h"
#include "u_states.h"

#include "nfs_shell.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace nfs_choose {
#endif


S32 frame;
textureb* backpic;
fontq* nfsfont;
S32 playeridx;


#ifdef USENAMESPACE
} // end namespace nfs_shell
using namespace nfs_shell;
using namespace nfs_utils;
using namespace nfs_choose;
#endif

void nfschoose_init()
{
	//playeridx = 0;
	frame=0;
// setup state
	pushandsetdir("nfs_shell");
// init 3d
	pushandsetdir("gfxtest");
	backpic = texture_create("MostWanted_05_656x369.jpg");
	popdir();
	backpic->load();

	popdir();

	pushandsetdir("fonts");
	nfsfont = new fontq("beat");
	popdir();
}

void nfschoose_proc()
{
// if something selected...
	switch(KEY) {
	case K_ESCAPE:
		changestate(STATE_NFSSHELL);
		break;
	case K_RETURN:
	case K_NUMENTER:
		changestate(STATE_NFSREADYRACE);
		break;
	case K_UP:
	case K_NUMUP:
		playeridx = range(0,playeridx-1,(S32)nfs_players.size()-1);
		break;
	case K_DOWN:
	case K_NUMDOWN:
		playeridx = range(0,playeridx+1,(S32)nfs_players.size()-1);
		break;
	}
	frame++;
}

void nfschoose_draw3d()
{
	//video_setviewport(&mainvp); // clear zbuf etc.
	video_sprite_begin();
	video_sprite_draw(backpic,pointf3x(.25f,.25f,.25f,1.0f),0,0,640,480);
	nfsfont->setcenter(true);
	S32 i;
	nfsfont->setcenter(false);
	nfsfont->print(640/4,125,400,20,F32YELLOW,"%s","Choose Racer:");
	for (i=0;i<(S32)nfs_players.size();++i) {
		pointf3 col = i == playeridx ? F32WHITE : F32LIGHTGRAY;
		nfsfont->print(640/4 + 25.0f,125 + 25.0f + 25*i,400 + 25.0f,20,col,"%s",nfs_players[i].name.c_str());
	}
	video_sprite_end();
}

void nfschoose_exit()
{
	logger("logging reference lists\n");
	logrc();
	delete nfsfont;
	backpic->rc.deleterc(backpic);
}
