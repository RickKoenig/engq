// state game is in when game is launched and running

#define INCLUDE_WINDOWS // to handle processes

#include <m_eng.h>
#include "u_states.h"

#include "nfs_utils.h"
#include "nfs_shell.h"
#include "nfs_choose.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace nfs_game {
#endif

STARTUPINFO si;
PROCESS_INFORMATION pi;
#if 1
string cmd = "engqtestd.exe -startstate 34";
string dir = "c:/srcw32/engq/engqtest";
#endif
#if 0
string cmd = "calc.exe";
string dir = "c:/srcw32/engq/engqtest";
#endif
#if 0
string cmd = "F:/source/Games/NFS/P4/depot/Alaska/Runtime/REL/FINALPC/Code/GameProjects/Alaska/Alaska_PC_Internal.exe -window -width 1024 -height 768 -racername";
string dir = "F:/source/Games/NFS/P4/depot/Alaska/Runtime/REL/FINALPC/Code/GameProjects/Alaska";
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
using namespace nfs_game;
using namespace nfs_utils;
using namespace nfs_shell;
using namespace nfs_choose;
#endif

void nfsgame_init()
{
	chitems.clear();
	item itm = {640/2,50,400,20,"Continue"};
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

	memset(&si,0,sizeof(si));
	si.cb = sizeof(si);
	memset(&pi,0,sizeof(pi));

}

//static struct bitmap32* cbm32;
void nfsgame_proc()
{
// if something selected...
	switch(KEY) {
	case K_ESCAPE:
		//changestate(STATE_MAINMENU);
		break;
	case K_RETURN:
	case K_NUMENTER:
		//changestate(STATE_NFSRESULT);
		break;
	}
	if (frame == 100) {
		C8 cmdcstr[1000];
#if 0
		string catstr = cmd + " " + nfs_players[playeridx].name;
#else
		string catstr = cmd;
#endif
		strcpy(cmdcstr,catstr.c_str());
#if 1
		// Start the child process. 
		if( !CreateProcess( NULL,   // No module name (use command line)
			cmdcstr,	    // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			dir.c_str(),    // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
		{
			errorexit( "CreateProcess failed (%d).\n", GetLastError() );
		}
		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
#endif
	}
	if (frame == 200) {
		changestate(STATE_NFSRESULT);
	}
	frame++;
}

void nfsgame_draw3d()
{
	//video_setviewport(&mainvp); // clear zbuf etc.
	video_sprite_begin();
	video_sprite_draw(backpic,pointf3x(.25f,.25f,.25f,1.0f),0,0,640,480);
	nfsfont->setcenter(false);
	nfsfont->print(640/4,125,400,20,F32YELLOW,"%s","In Game ...");
		
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

void nfsgame_exit()
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

#if 0
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

void _tmain( int argc, TCHAR *argv[] )
{

 
    if( argc != 2 )
    {
        printf("Usage: %s [cmdline]\n", argv[0]);
        return;
    }


#endif
