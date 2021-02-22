#define D2_3D

#include <m_eng.h>
#include <l_misclibm.h>

#include "u_s_mainmenu.h"
#include "u_s_user0.h"
#include "u_s_user1.h"
#include "u_s_user2.h"
#include "u_s_lightsout.h"

#include "u_s_mattest.h"
#include "u_s_vidcap.h"
#include "u_s_scratch.h"
#include "u_s_jigpuzz.h"
#include "u_s_3dtest.h"

#include "u_s_gfxtest.h"
#include "u_s_renamer.h"
#include "u_s_lighttest.h"
#include "u_s_vidtest.h"
#include "u_s_luatest.h"

#include "u_s_gametheory.h"
#include "u_s_onebit.h"
#include "u_s_braid.h"
#include "u_s_browser.h"
#include "u_s_joetest.h"

#include "u_s_occserver.h"
#include "u_s_occclient.h"
#include "u_s_sokoban.h"
#include "u_s_slider.h"
#include "u_s_occclient2.h"

#include "u_s_joetest2.h"
#include "u_s_sokoban2.h"
#include "u_s_wriggler.h"
#include "u_s_ui.h"
#include "u_s_tritest1.h"

#include "u_s_tritest2.h"
#include "u_s_tritest3.h"
#include "u_s_tritest4.h"
#include "u_s_tritest5.h"
#include "u_s_plotter.h"

#include "u_s_plot2simple.h"
#include "u_s_plot2quant.h"
#include "u_s_plot2quantsho.h"
#include "u_s_plot2lens.h"
#include "u_s_plot2poles.h"
#include "u_s_plot2zeta.h"
#include "u_s_plot2power.h"
#include "u_s_plot2eigen.h"
#include "u_s_plot2fourducks.h"
#include "u_s_plot2bezier.h"
#include "u_s_plot2octmirror.h"
#include "u_s_plot2linear.h"
#include "u_s_plot2soccergoal.h"
#include "u_s_plot2bresenham.h"
#include "u_s_plot2collisions1d.h"
#include "u_s_plot2catmullrom.h"
#include "u_s_plot2random.h"
#include "u_s_plot2networkflow.h"

#include "u_s_many.h"
#include "u_s_arena1.h"
#include "u_s_arena2.h"
#include "u_s_testres3d.h"
#include "u_s_basic3d.h"

#include "u_s_stoid.h"

//#include "u_plotter2.h"
//#include "u_goldbach.h"

#include "engine7test/n_carenalobbycpp.h"
#include "engine7test/n_facevertscpp.h"
#include "engine7test/n_polylistcpp.h"
#include "engine7test/n_meshtestcpp.h"
#include "engine7test/n_constructorcpp.h"
#include "engine7test/n_newconstructorcpp.h"
#include "engine7test/n_carenaconnectcpp.h"
//#include "engine7test/carenastatuscpp.h"
#include "engine7test/n_trackhashcpp.h"
#include "engine7test/n_usefulcpp.h"
#include "engine7test/n_carclass.h"
#include "engine7test/n_carenagamecpp.h"

#include "racenet/gameinfo.h"
#include "racenet/newlobby.h"

//#include "engine7test/n_line2roadcpp.h"
#include "hw35_line2road.h"

#include "racenet/updater.h"
#include "racenet/online_uplay.h"

#include "u_states.h"
#include "u_testasmmacros.h"

//#include "u_goldbach.h"

#include "u_s_rubikscube.h"
#include "u_s_halfpipe.h"
#include "u_s_syncro.h"
#include "u_s_spawner.h"
#include "u_s_searcher.h"
#include "u_s_toon.h"
#include "u_s_playamovie.h"
#include "u_s_gfxview.h"
#include "u_s_spritetest2.h"
#include "u_s_spritetest3.h"
#include "u_s_joytest.h"

#include "racenet/match_server.h"
#include "racenet/match_logon.h"
#include "racenet/match_lobby.h"
#include "racenet/match_results.h"
#include "racenet/match_hiscores.h"
#include "racenet/match_pickgame.h"
#include "racenet/match_options.h"
#include "racenet/match_quickgame.h"
#include "racenet/match_game1.h"
#include "racenet/match_register.h"
#include "racenet/match_pickserver.h"
#include "racenet/match_title.h"

#include "racenet/broadcast_testserver.h"
#include "racenet/broadcast_testclient.h"
#include "racenet/broadcast_server.h"
#include "u_s_post.h"

#include "wat_physics/quant3/quant3.h"
#include "wat_physics/quant4/quant4.h"
#include "u_s_igroup.h"
#include "u_s_polynomial.h"
#include "u_s_ifield.h"

#include "engine7test/n_marchingtest.h"
#include "u_s_ttt.h"
#include "u_s_game2_ttt2.h"
#include "u_s_honeycomb.h"
#include "u_s_simplex.h"
#include "u_s_mandl.h"
#include "u_s_poly_sym.h"

#include "u_s_qr_code.h"
#include "u_s_qr_code2.h"
#include "u_s_penrose.h"

#include "u_s_physics3d.h"
#include "u_s_physics2d.h"
#include "u_s_heathouse.h"
#include "u_s_jelly.h"

#include "u_s_game2_endgame.h"
#include "u_s_game2_ttt.h"
#include "u_s_game2_nim.h"
#include "u_s_game2_opposition.h"
#include "u_s_game2_foxgoose.h"

#include "wat_physics/quant3/quant_sho.h"

#include "nfs_shell.h"
#include "nfs_smile.h"
#include "nfs_entername.h"
#include "nfs_choose.h"
#include "nfs_readyrace.h"
#include "nfs_game.h"
#include "nfs_result.h"
#include "u_s_particles.h"
#include "u_s_riemann.h"

#include "u_s_ar.h"

#include "u_s_conduit.h"
#include "u_s_cubemaptest.h"
#include "u_s_cubemaptest2.h"

#include "u_s_qcomp.h"
#include "u_s_laplace_heat.h"
#include "u_s_castle.h"
#include "u_s_rcastle.h"

#include "u_s_turing.h"
#include "u_s_digital.h"

#include "u_s_pointcloud.h"
#include "u_s_plot2pointcloud.h"
#include "u_s_chattertest.h"
#include "u_s_holoview2d.h"
#include "u_s_holoview3d.h"
#include "u_s_holoview2db.h"

#include "st2_uplay3.h"

C8 winmain_datadir[]="data"; // folder off of .exe
C8 winmain_version[]="engqtest 1.00";

struct state states[STATE_NUM]={
//	init				proc				draw3d			draw2d				exit
// 0
	{mainmenuinit		,mainmenuproc		,0					,mainmenudraw2d		,mainmenuexit},
	{user0init			,user0proc			,0					,user0draw2d		,user0exit},
	{user1init			,user1proc			,0					,user1draw2d		,user1exit},
	{user2init			,user2proc			,0					,user2draw2d		,user2exit},
	{userlightsoutinit	,userlightsoutproc	,0					,userlightsoutdraw2d,userlightsoutexit},
// 5
	{s_mattest_init		,s_mattest_proc		,0					,s_mattest_draw2d	,s_mattest_exit},
	{vidcap_init		,vidcap_proc		,vidcap_draw3d		,vidcap_draw2d		,vidcap_exit},
	{scratchinit		,scratchproc		,0					,scratchdraw2d		,scratchexit},
	{jigpuzzinit		,jigpuzzproc		,0					,jigpuzzdraw2d		,jigpuzzexit},
	{d3testinit			,d3testproc			,d3testdraw3d		,0					,d3testexit},
// 10
	{gfxtestinit		,gfxtestproc		,0					,gfxtestdraw2d		,gfxtestexit},
	{renamerinit		,renamerproc		,0					,renamerdraw2d		,renamerexit},
	{lighttestinit		,lighttestproc		,lighttestdraw3d	,0					,lighttestexit},
	{vidtestinit		,vidtestproc		,vidtestdraw3d		,0					,vidtestexit},
	{luatestinit		,luatestproc		,0					,luatestdraw2d		,luatestexit},
// 15
	{gametheoryinit		,gametheoryproc		,0					,gametheorydraw2d	,gametheoryexit},
	{onebitinit			,onebitproc			,0					,onebitdraw2d		,onebitexit},
	{braidinit			,braidproc			,braiddraw3d		,0					,braidexit},
	{browserinit		,browserproc		,0					,browserdraw2d		,browserexit},
	{joetestinit		,joetestproc		,0					,joetestdraw2d		,joetestexit},
// 20
	{occserverinit		,occserverproc		,0					,occserverdraw2d	,occserverexit},
	{occclientinit		,occclientproc		,0					,occclientdraw2d	,occclientexit},
	{sokobaninit		,sokobanproc		,0					,sokobandraw2d		,sokobanexit},
	{sliderinit			,sliderproc			,0					,sliderdraw2d		,sliderexit},
	{occclient2init		,occclient2proc		,0					,occclient2draw2d	,occclient2exit},
// 25
	{joetest2init		,joetest2proc		,0					,joetest2draw2d		,joetest2exit},
	{sokoban2init		,sokoban2proc		,0					,sokoban2draw2d		,sokoban2exit},
	{wrigglerinit		,wrigglerproc		,0					,wrigglerdraw2d		,wrigglerexit},
	{ui_init			,ui_proc			,0					,ui_draw2d			,ui_exit},
	{tritest1init		,tritest1proc		,tritest1draw3d		,0					,tritest1exit},
// 30
	{tritest2init		,tritest2proc		,tritest2draw3d		,tritest2draw2d		,tritest2exit},
	{tritest3init		,tritest3proc		,tritest3draw3d		,0					,tritest3exit},
	{tritest4init		,tritest4proc		,tritest4draw3d		,0					,tritest4exit},
	{tritest5init		,tritest5proc		,tritest5draw3d		,0					,tritest5exit},
	{plotterinit		,plotterproc		,0					,plotterdraw2d		,plotterexit},
// 35
	{manyinit			,manyproc			,manydraw3d			,0					,manyexit},
	{arena1init			,arena1proc			,arena1draw3d		,0					,arena1exit},
	{arena2init			,arena2proc			,arena2draw3d		,0					,arena2exit},
// plotter2 stuff
	{plot2simpleinit	,plot2simpleproc	,0					,plot2simpledraw2d	,plot2simpleexit},
	{plot2quantinit		,plot2quantproc		,0					,plot2quantdraw2d	,plot2quantexit},
// 40
	{plot2quantshoinit	,plot2quantshoproc	,0					,plot2quantshodraw2d,plot2quantshoexit},
	{plot2lensinit		,plot2lensproc		,0					,plot2lensdraw2d	,plot2lensexit},
	{plot2polesinit		,plot2polesproc		,0					,plot2polesdraw2d	,plot2polesexit},
	{plot2zetainit		,plot2zetaproc		,0					,plot2zetadraw2d	,plot2zetaexit},
	{plot2powerinit		,plot2powerproc		,0					,plot2powerdraw2d	,plot2powerexit},
// 45
	{plot2eigeninit		,plot2eigenproc		,0					,plot2eigendraw2d	,plot2eigenexit},
	{plot2fourducksinit	,plot2fourducksproc	,0					,plot2fourducksdraw2d,plot2fourducksexit},
	{plot2bezierinit	,plot2bezierproc	,0					,plot2bezierdraw2d	,plot2bezierexit},
	{plot2octmirrorinit	,plot2octmirrorproc	,0					,plot2octmirrordraw2d,plot2octmirrorexit},
	{plot2linearinit	,plot2linearproc	,0					,plot2lineardraw2d	,plot2linearexit},
// 50
	{plot2soccergoalinit,plot2soccergoalproc,0					,plot2soccergoaldraw2d,plot2soccergoalexit},
	{plot2bresenhaminit,plot2bresenhamproc,0					,plot2bresenhamdraw2d,plot2bresenhamexit},
	{plot2collisions1dinit,plot2collisions1dproc,0				,plot2collisions1ddraw2d,plot2collisions1dexit},
	{plot2catmullrominit,plot2catmullromproc,0					,plot2catmullromdraw2d,plot2catmullromexit},
	{plot2randominit	,plot2randomproc	,0					,plot2randomdraw2d	,plot2randomexit},
// 55
	{plot2networkflowinit,plot2networkflowproc,0				,plot2networkflowdraw2d,plot2networkflowexit},
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
// 60
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
	{0					,0					,0					,0					,0},
// 65
	{0					,0					,0					,0					,0},
	// end plotter2 stuff
	{uplay3_init		,uplay3_proc		,uplay3_draw3d		,0					,uplay3_exit}, // 88 stunt2 demo
	{stoidinit			,stoidproc			,0					,stoiddraw2d		,stoidexit},
	{testres3dinit		,testres3dproc		,testres3ddraw3d	,0					,testres3dexit},
	{basic3dinit		,basic3dproc		,basic3ddraw3d		,0					,basic3dexit},
// 70
	{carenalobbyinit	,carenalobbyproc	,carenalobbydraw3d	,carenalobbydraw2d	,carenalobbyexit},		// e7t lobby
	{carenaconnectinit	,carenaconnectproc	,0					,carenaconnectdraw2d,carenaconnectexit},	// e7t carenaconnect
	{constructorinit	,constructorproc	,constructordraw3d	,constructordraw2d	,constructorexit},		// e7t cons
	{newconstructorinit	,newconstructorproc	,newconstructordraw3d,newconstructordraw2d,newconstructorexit}, // e7t new cons
	{meshtestinit		,meshtestproc 		,meshtestdraw3d		,meshtestdraw2d		,meshtestexit},			// e7t meshtest
// 75
	{carenastatusinit	,carenastatusproc	,0					,carenastatusdraw2d	,carenastatusexit},		// e7t carenastatus
	{carenagameinit		,carenagameproc		,carenagamedraw3d	,carenagamedraw2d	,carenagameexit},		// e7t carenagame
	{newlobby_init		,newlobby_proc		,newlobby_draw3d	,0					,newlobby_exit},		// carena lobby
	{updater_init		,updater_proc		,updater_draw3d		,0					,updater_exit},			// carena updater
	{onlinerace_init	,onlinerace_proc	,onlinerace_draw3d	,0					,onlinerace_exit},		// carena connect
// 80
	{carextractstate_init,carextractstate_proc,carextractstate_draw3d,0				,carextractstate_exit},	// carena car extract(sleextra=1)
	{rcubeinit			,rcubeproc 			,rcubedraw3d		,rcubedraw2d		,rcubeexit},			// rubikscube
	{halfpipeinit		,halfpipeproc 		,0					,halfpipedraw2d		,halfpipeexit},			// halfpipe builder
	{syncro_init		,syncro_proc 		,0					,syncro_draw2d		,syncro_exit},			// mutex/semaphores etc.
	{spawner_init		,spawner_proc 		,0					,spawner_draw2d		,spawner_exit},			// mutex/semaphores etc.
// 85
	{searcher_init		,searcher_proc 		,0					,searcher_draw2d	,searcher_exit},		// searcher
	{tooninit			,toonproc			,toondraw3d			,0					,toonexit},				// test toon effect
	{playamovieinit		,playamovieproc		,playamoviedraw3d	,0					,playamovieexit},		// test playamovie (theora)
	{gfxviewinit		,gfxviewproc		,0					,gfxviewdraw2d		,gfxviewexit},			// collage viewer
	{spritetest2init	,spritetest2proc	,0					,spritetest2draw2d	,spritetest2exit},		// sprite test in 2d (using software blits old school) // slow in hardware mode
// 90
	{spritetest3init	,spritetest3proc	,spritetest3draw3d	,0					,spritetest3exit},		// collage viewer in 3d (using 3d pipeline hard/soft) // slow in software mode
	{joytestdiinit		,joytestdiproc 		,0					,joytestdidraw2d	,joytestdiexit},		// mutex/semaphores etc.
	{joytestmminit		,joytestmmproc 		,0					,joytestmmdraw2d	,joytestmmexit},		// mutex/semaphores etc.
	{match_server_init	,match_server_proc	,match_server_draw3d,0					,match_server_exit},		// carena lobby
	{match_logon_init	,match_logon_proc	,match_logon_draw3d	,0					,match_logon_exit},		// carena lobby
// 95
	{match_lobby_init	,match_lobby_proc	,0					,match_lobby_draw2d	,match_lobby_exit},		// carena lobby
	{match_results_init	,match_results_proc	,0					,match_results_draw2d,match_results_exit},	// carena lobby
	{match_hiscoresinit ,match_hiscoresproc ,0					,match_hiscoresdraw2d,match_hiscoresexit},// carena lobby
	{match_pickgameinit ,match_pickgameproc ,0					,match_pickgamedraw2d,match_pickgameexit},// carena lobby
	{match_optionsinit  ,match_optionsproc  ,0					,match_optionsdraw2d,match_optionsexit},// carena lobby
// 100
	{match_quickgameinit,match_quickgameproc,0					,match_quickgamedraw2d,match_quickgameexit},// carena lobby
	{match_game1init	,match_game1proc	,0					,match_game1draw2d	,match_game1exit},// carena lobby
	{match_registerinit	,match_registerproc	,match_registerdraw3d,0					,match_registerexit},// carena lobby
	{match_pickserverinit,match_pickserverproc,match_pickserverdraw3d,0				,match_pickserverexit},// carena lobby
	{match_titleinit	,match_titleproc	,match_titledraw3d	,0					,match_titleexit},// carena lobby
// 105
	{onlineracemain_init,onlineracemain_proc,onlineracemain_draw3d,0				,onlineracemain_exit},    // carena maingame
	{broadcast_testserver_init	,broadcast_testserver_proc	,broadcast_testserver_draw3d,0,broadcast_testserver_exit},    // carena maingame
	{broadcast_testclient_init	,broadcast_testclient_proc	,broadcast_testclient_draw3d,0,broadcast_testclient_exit},    // carena maingame
	{broadcast_server_init		,broadcast_server_proc		,broadcast_server_draw3d	,0,broadcast_server_exit},    // carena maingame
	{post_init			,post_proc			,0					,post_draw2d		,post_exit}, // POST tester
// 110
	{quant3_init		,quant3_proc		,0					,quant3_draw2d		,quant3_exit}, // QM simulator
	{quant4_init		,quant4_proc		,0					,quant4_draw2d		,quant4_exit}, // QM simulator, varying potentials
	{igroup_init		,igroup_proc		,0					,igroup_draw2d		,igroup_exit},
	{polynomial_init	,polynomial_proc	,0					,polynomial_draw2d	,polynomial_exit},
	{ifield_init		,ifield_proc		,0					,ifield_draw2d		,ifield_exit},
// 115
	{marchingcubestest_init,marchingcubestest_proc,marchingcubestest_draw3d,0		,marchingcubestest_exit},
	{jellyinit			,jellyproc			,0					,jellydraw2d		,jellyexit},
	{tttinit			,tttproc			,0					,tttdraw2d			,tttexit},
	{game2_ttt2init		,game2_ttt2proc		,0					,game2_ttt2draw2d	,game2_ttt2exit},
	{honeycombinit		,honeycombproc		,0					,honeycombdraw2d	,honeycombexit},
// 120
	{simplexinit		,simplexproc		,0					,simplexdraw2d		,simplexexit},
	{mandlinit			,mandlproc			,0					,mandldraw2d		,mandlexit},
	{polysyminit		,polysymproc		,0					,polysymdraw2d		,polysymexit},
	{qrcodeinit			,qrcodeproc			,0					,qrcodedraw2d		,qrcodeexit},
	{qrcodeinit2		,qrcodeproc2		,0					,qrcodedraw2d2		,qrcodeexit2},
// 125
	{penrose_init		,penrose_proc		,penrose_draw3d		,0					,penrose_exit},
	{physics3dinit		,physics3dproc		,physics3ddraw3d	,0					,physics3dexit},
	{physics2dinit		,physics2dproc		,physics2ddraw3d	,0					,physics2dexit},
	{heathouseinit		,heathouseproc		,0					,heathousedraw2d	,heathouseexit},
	{game2_endgameinit	,game2_endgameproc	,0					,game2_endgamedraw2d,game2_endgameexit},
// 130
	{game2_tttinit		,game2_tttproc		,0					,game2_tttdraw2d	,game2_tttexit},
	{game2_niminit		,game2_nimproc		,0					,game2_nimdraw2d	,game2_nimexit},
	{game2_oppositioninit,game2_oppositionproc,0				,game2_oppositiondraw2d,game2_oppositionexit},
	{game2_foxgooseinit	,game2_foxgooseproc	,0					,game2_foxgoosedraw2d,game2_foxgooseexit},
	{quantsho_init		,quantsho_proc		,0					,quantsho_draw2d	,quantsho_exit}, // QM simulator, varying potentials
// 135
	{nfsshell_init		,nfsshell_proc		,nfsshell_draw3d	,0					,nfsshell_exit},
	{nfssmile_init		,nfssmile_proc		,nfssmile_draw3d	,0					,nfssmile_exit},
	{nfsentername_init	,nfsentername_proc	,nfsentername_draw3d,0					,nfsentername_exit},
	{nfschoose_init		,nfschoose_proc		,nfschoose_draw3d	,0					,nfschoose_exit},
	{nfsreadyrace_init	,nfsreadyrace_proc	,nfsreadyrace_draw3d,0					,nfsreadyrace_exit},
// 140
	{nfsgame_init		,nfsgame_proc		,nfsgame_draw3d		,0					,nfsgame_exit},
	{nfsresult_init		,nfsresult_proc		,nfsresult_draw3d	,0					,nfsresult_exit},
	{particles_init		,particles_proc		,particles_draw3d	,0					,particles_exit},
	{riemann_init		,riemann_proc		,0					,riemann_draw2d		,riemann_exit},
	{ar_init			,ar_proc			,ar_draw3d			,0/*ar_draw2d*/		,ar_exit},
// 145
	{conduit_init		,conduit_proc		,0					,conduit_draw2d		,conduit_exit},
	{cubemaptestinit	,cubemaptestproc	,cubemaptestdraw3d	,0					,cubemaptestexit},
	{cubemaptest2init	,cubemaptest2proc	,0					,cubemaptest2draw2d	,cubemaptest2exit},
	{qcompinit			,qcompproc			,0					,qcompdraw2d		,qcompexit},
	{laplaceinit		,laplaceproc		,0					,laplacedraw2d		,laplaceexit},
// 150
	{castleinit			,castleproc			,castledraw3d		,0					,castleexit},
	{rcastleinit		,rcastleproc		,rcastledraw3d		,0					,rcastleexit},
	{turinginit			,turingproc			,0					,turingdraw2d		,turingexit},
	{digital_init		,digital_proc		,0					,digital_draw2d		,digital_exit},
	{pointcloudinit		,pointcloudproc		,pointclouddraw3d	,0					,pointcloudexit},
// 155
	{plot2pointcloudinit,plot2pointcloudproc,0					,plot2pointclouddraw2d,plot2pointcloudexit},
	{chatter_test_init	,chatter_test_proc	,0					,chatter_test_draw2d,chatter_test_exit},
	{holoview2dinit		,holoview2dproc		,0					,holoview2ddraw2d	,holoview2dexit},
	{holoview3dinit		,holoview3dproc		,holoview3ddraw3d	,0					,holoview3dexit},
	{holoview2dbinit	,holoview2dbproc	,0					,holoview2dbdraw2d	,holoview2dbexit },
};

S32 numstates=sizeof(states)/sizeof(states[0]);

void maininit()
{
// allocate global stuff
// test 'globalgame' with a file if Tim hasn't loaded globalgame already
	const C8* fn="racenetdata/onlineopt/gameinfo.txt";
	if (!globalgame && fileexist(fn)) {
		const C8* git=fileload(fn);
		gameinf* gi=new gameinf(git);
		delete git;
		if (gi->isvalid) {
			globalgame=gi; // pass ownership to globalgame
		} else {
			delete gi;
		}
	}
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
	uninitsocker();
	delete globalgame; // free the global gameinf class
	globalgame=0;		// just being safe..
}
