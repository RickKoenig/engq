// enter new name state
#include <m_eng.h>
#include <l_misclibm.h>

#include "u_states.h"

#include "nfs_utils.h"
#include "nfs_shell.h"
#include "nfs_choose.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace nfs_entername {
#endif

string nfs_name;
const U32 max_nfs_name = 16;

S32 frame;
tree2* roottree;
textureb* backpic;
fontq* nfsfont;
S32 choice;
const float NAMEX = 350;
const float NAMEY = 50;

struct item {
	float x,y;
	float sx,sy;
	C8* text;
};

/*item items[] = {
	{300/2,50,400,20,"Enter Name :"},
	{300/2,150,400,20,"Back, Cancel Enter Name"},
};
S32 nitems = sizeof(items)/sizeof(items[0]);
*/
vector<item> items;
#ifdef USENAMESPACE
} // end namespace nfs_shell
using namespace nfs_shell;
using namespace nfs_entername;
using namespace nfs_utils;
using namespace nfs_choose;
#endif

void nfsentername_init()
{
	items.clear();
	item item1 = {300/2,50,400,20,"Enter Name :"};
	item item2 = {300/2,150,400,20,"Back, Cancel Enter Name"};
	items.push_back(item1);
	items.push_back(item2);

	nfs_name = "";
	choice = 0;
	//nchoice = 2;
	//showcursor(0);
// enum camera devices
	frame=0;
// setup state
//	video_setupwindow(GX,GY);//,565);
//	video_setupwindow(SX,SY);//,565);
	pushandsetdir("nfs_shell");
// init 3d
//	uselightssave=lightinfo.uselights;
//	lightinfo.uselights=0;
	pushandsetdir("gfxtest");
	roottree=new tree2("roottree");
	backpic = texture_create("MostWanted_05_656x369.jpg");
	popdir();
	backpic->load();

	popdir();

	pushandsetdir("fonts");
	nfsfont = new fontq("beat");
	popdir();
	
/*	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.25f;
	mainvp.zback=10000;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.camtrans.z=-3;
	mainvp.camtrans.x=0;
	mainvp.camtrans.y=0;
	//mainvp.flags=VP_CLEARBG|VP_CLEARWB;
	mainvp.xsrc=640;
	mainvp.ysrc=480;
	mainvp.useattachcam=false; */
//	mainvp.lookat=0;
//	GetWindowText((HWND)wininfo.MainWindow,oldwinname,256);
//	SetWindowText((HWND)wininfo.MainWindow,"NFS Shell");
}

//static struct bitmap32* cbm32;
void nfsentername_proc()
{
	if (choice == 0) {
		U32 ns = nfs_name.size();
		if (ns < max_nfs_name) {
			bool gotchar = false; // type in your name
			if (KEY >= 'a' && KEY <= 'z')
				gotchar = true;
			if (KEY >= 'A' && KEY <= 'Z')
				gotchar = true;
			if (KEY == ' ')
				gotchar = true;
			if (gotchar)
				nfs_name += KEY;
		}
		if (ns > 0) {
			if (KEY == K_BACKSPACE)
				nfs_name.resize(ns - 1);
			if (KEY == K_RETURN) {
				nfs_player np;
				np.name = nfs_name;
				nfs_players.push_back(np);
				playeridx = nfs_players.size() - 1;
				changestate(STATE_NFSSMILE);
			}
		}

	}
// if something selected...
	switch(KEY) {
	case K_ESCAPE:
	changestate(STATE_NFSSHELL);
		break;
	case K_RETURN:
	case K_NUMENTER:
		switch(choice) {
		case 0:
			//changestate(STATE_NFSSHELL);
			break;
		case 1:
			changestate(STATE_NFSSHELL);
			break;
		}
		break;
	case K_UP:
	case K_NUMUP:
		choice = range(0,choice-1,(S32)items.size()-1);
		break;
	case K_DOWN:
	case K_NUMDOWN:
		choice = range(0,choice+1,(S32)items.size()-1);
		break;
	}
//	cbm32=vcamgrabframe();
//	roottree->proc(); 
//	video_buildworldmats(roottree);
//	doflycam(&mainvp);
	frame++;
}

void nfsentername_draw3d()
{
	//video_setviewport(&mainvp); // clear zbuf etc.
	video_sprite_begin();
	video_sprite_draw(backpic,pointf3x(.25f,.25f,.25f,1.0f),0,0,640,480);
	nfsfont->setcenter(false);
	S32 i;
	for (i=0;i<(S32)items.size();++i) {
		const item& im = items[i];
		float hix = im.x;
		float hiy = im.y;
		pointf3 col = F32GREEN;
		float sx = im.sx;
		float sy = im.sy;

		if (i == choice) {
			//hix -= 10.0f;
			col = F32WHITE;
			S32 ef = hiliteff(frame);
			sx += ef;
			sy += ef;
			hiy -= ef/2.0f;
		}
		nfsfont->print(hix,hiy ,sx,sy,col,"%s",im.text);
	}
	nfsfont->setcenter(false);
	if (choice == 0) {
		if ((frame & 4) && nfs_name.length() < max_nfs_name) {
			nfsfont->print(NAMEX,NAMEY,300,20,F32WHITE,"%s",(nfs_name + ".").c_str());
		} else {
			nfsfont->print(NAMEX,NAMEY,300,20,F32WHITE,"%s",nfs_name.c_str());
		}
	} else {
		nfsfont->print(NAMEX,NAMEY,300,20,F32GREEN,"%s",nfs_name.c_str());
	}
	video_sprite_end();
}

void nfsentername_exit()
{
	showcursor(1);
//	SetWindowText((HWND)wininfo.MainWindow,oldwinname);
	vcamstopstream();
	vcamexit();
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	delete nfsfont;
//	lightinfo.uselights=uselightssave;
	backpic->rc.deleterc(backpic);
}
