#define D2_3D

#include <m_eng.h>
#include <l_misclibm.h>

#include "u_states.h"

#include "usertest/u_s_mainmenu.h"
#include "usertest/u_s_braid.h"
#include "usertest/u_s_browser.h"
#include "usertest/u_s_tritest1.h"
#include "usertest/u_s_many.h"

#include "utils/json.h"
#include "newcarena/hash.h"
#include "newcarena/enums.h"
#include "newcarena/carclass.h"
#include "newcarena/carenalobby.h"
#include "newcarena/connectstate.h"
#include "newcarena/constructor.h"
#include "newcarena/newconstructor.h"
#include "newcarena/carenagame.h"

//#include "newcarena/n_jsonstructs.h"

#include "usertest/u_s_playamovie.h"
#include "newcarena/n_marchingtest.h"
#include "usertest/u_s_plotter.h"

// game matching system
#include "newcarena/match_server.h"
#include "newcarena/match_logon.h"
#include "newcarena/match_lobby.h"
#include "newcarena/match_results.h"
#include "newcarena/match_hiscores.h"
#include "newcarena/match_pickgame.h"
#include "newcarena/match_options.h"
#include "newcarena/match_quickgame.h"
#include "newcarena/match_game1.h"
#include "newcarena/match_register.h"
#include "newcarena/match_pickserver.h"
#include "newcarena/match_title.h"

#include "usertest/u_s_newtons.h"
#include "usertest/u_s_maze.h"
#include "usertest/u_s_mattest.h"
#include "usertest/u_s_mattest2.h"
#include "usertest/u_s_lightup.h"
#include "usertest/u_s_riemann.h"

C8 winmain_datadir[]="data"; // folder off of .exe
C8 winmain_version[]="newcarena 2.00";

struct state states[]={
//	init				proc				draw3d			draw2d				exit
// 0
	{mainmenuinit		,mainmenuproc		,0					,mainmenudraw2d		,mainmenuexit},
	{braidinit			,braidproc			,braiddraw3d		,0					,braidexit},
	{browserinit		,browserproc		,0					,browserdraw2d		,browserexit},
	{tritest1init		,tritest1proc		,tritest1draw3d		,0					,tritest1exit},
	{manyinit			,manyproc			,manydraw3d			,0					,manyexit},
// 5
	{carenalobbyinit	,carenalobbyproc	,carenalobbydraw3d	,0/*carenalobbydraw2d*/,carenalobbyexit},		// e7t lobby
	{carenaconnectinit	,carenaconnectproc	,0					,carenaconnectdraw2d,carenaconnectexit},	// e7t carenaconnect
	{constructorinit	,constructorproc	,constructordraw3d	,constructordraw2d	,constructorexit},		// e7t cons
	{newconstructorinit	,newconstructorproc	,newconstructordraw3d,newconstructordraw2d,newconstructorexit}, // e7t new cons
	{carenastatusinit	,carenastatusproc	,0					,carenastatusdraw2d	,carenastatusexit},		// e7t carenastatus
// 10
	{carenagameinit		,carenagameproc		,carenagamedraw3d	,0/*carenagamedraw2d*/,carenagameexit},		// e7t carenagame
	{playamovieinit		,playamovieproc		,playamoviedraw3d	,0					,playamovieexit},		// test playamovie (theora)
	{marchingcubestest_init,marchingcubestest_proc,marchingcubestest_draw3d,0		,marchingcubestest_exit}, // QM simulator, varying potentials
	{plotterinit		,plotterproc		,0					,plotterdraw2d		,plotterexit},			// simple, scratch, float 2d plotter, grids etc.
	{match_server_init	,match_server_proc	,match_server_draw3d,0					,match_server_exit},		// carena lobby
// 15
	{match_logon_init	,match_logon_proc	,match_logon_draw3d	,0					,match_logon_exit},		// carena lobby
	{match_lobby_init	,match_lobby_proc	,0					,match_lobby_draw2d	,match_lobby_exit},		// carena lobby
	{match_results_init	,match_results_proc	,0					,match_results_draw2d,match_results_exit},	// carena lobby
	{match_hiscoresinit ,match_hiscoresproc ,0					,match_hiscoresdraw2d,match_hiscoresexit},// carena lobby
	{match_pickgameinit ,match_pickgameproc ,0					,match_pickgamedraw2d,match_pickgameexit},// carena lobby
// 20
	{match_optionsinit  ,match_optionsproc  ,0					,match_optionsdraw2d,match_optionsexit},// carena lobby
	{match_quickgameinit,match_quickgameproc,0					,match_quickgamedraw2d,match_quickgameexit},// carena lobby
	{match_game1init	,match_game1proc	,0					,match_game1draw2d	,match_game1exit},// carena lobby
	{match_registerinit	,match_registerproc	,match_registerdraw3d,0					,match_registerexit},// carena lobby
	{match_pickserverinit,match_pickserverproc,match_pickserverdraw3d,0				,match_pickserverexit},// carena lobby
// 25
	{match_titleinit	,match_titleproc	,match_titledraw3d	,0					,match_titleexit},// carena lobby
	{newtonsinit		,newtonsproc		,0					,newtonsdraw2d		,newtonsexit},			// newtons method
	{maze_init			,maze_proc			,maze_draw3d		,0					,maze_exit},			// marble maze
	{mattest_init		,mattest_proc		,0					,mattest_draw2d		,mattest_exit},			// matrix test
	{mattest2_init		,mattest2_proc		,0					,mattest2_draw2d	,mattest2_exit},		// matrix test
// 30
	{lightup_init		,lightup_proc		,0					,lightup_draw2d		,lightup_exit},			// matrix test
	{riemann_init		,riemann_proc		,0					,riemann_draw2d		,riemann_exit},			// matrix test
};
S32 numstates=sizeof(states)/sizeof(states[0]);

void maininit()
{
// allocate global stuff
	if (STATE_NUM!=numstates)
		errorexit("bad state table");
	changestate(wininfo.startstate);
}

bool mainproc()
{
	return stateproc();
}

void maindraw3d()
{
	statedraw3d();
}

void maindraw2d()
{
	statedraw2d();
}

bool mainhasdraw3d()
{
	return statehasdraw3d();
}

bool mainhasdraw2d()
{
	return statehasdraw2d();
}

void mainexit()
{
// free global stuff
// matching system
	if (matchcl) {
		logger("freeing an allocated matchcl\n"); // closewindow will do this (program abort)
		freesocker(matchcl);
		matchcl=0;
	}
// networking in general
	uninitsocker();
}
