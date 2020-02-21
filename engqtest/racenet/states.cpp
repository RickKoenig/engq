#include <windows.h>
//#include <winbase.h>
#include <stdio.h>
#include <engine1.h>
#include <misclib.h>

#include "stubhelper.h"
#include "stubcom.h"

#include "debprint.h" // for loadconfigfile

#include "newlobby.h" // 1
//#include "online_seltrack.h" // 1
#include "line2road.h"
#include "online_uplay.h" // 2
#include "mainmenu.h" // 3
#include "ddstest.h" // 4
#include "scenetest.h"// 5
#include "soundtest.h" // 6

// these are not needed by engine1.lib
static struct state states[]={
	NULL,NULL,NULL, // 0
	newlobby_init,newlobby_proc,newlobby_exit,	// 1 new lobby
	onlinerace_init,onlinerace_proc,onlinerace_exit, // 2 ONLINE race track
	mainmenu_init,mainmenu_proc,mainmenu_exit, // 3 used for debugging, testing hardware
	ddstest_init,ddstest_proc,ddstest_exit, // 4 dds tests
//	onlinecars_init,onlinecars_proc,onlinecars_exit, // 4 read in custom cars..
	testscene_init,testscene_proc,testscene_exit, // 5 test fish track, test new shiny cars
	soundtest_init,soundtest_proc,soundtest_exit, // 6
};
static int numstates=sizeof(states)/sizeof(states[0]);


int  dofilelogger; // globaled for config.txt
int startstate=-1; // setup for config.txt

// needed for engine1.lib
char winmain_datadir[]="sledca";	// your game directory
//char winmain_datadir[]="sledca";	// your game directory
char winmain_version[]="MULTI PURPOSE RACING 6.19";

void earlymaininit(char *cmd)
{
// get all stub info into mainstubinfo
//	stub_earlymaininit(cmd);
}

// called before the first state init called
void maininit(char *cmd)
{
	int stubinited;
// setup common directory (in case stubs don't work)
	setdir(".."); // up to .exe dir
	stubinited=stub_readStartupInfo(cmd);
	setdir("."); // back to data dir
	strcpy(commondir,"common\\");
// stub system
	setupstubdirs();
// read file manifest
	loadmanifest("allfiles.txt");
// calls 	stub_readStartupInfo(cmd); AND
// inits common,dswmediadir,prefsdir
	stub_maininit(stubinited);	// user defined, copy stub params into user structure			

	changeescbehavior(ESC_NONE); // enum {ESC_NONE,ESC_POP,ESC_QUIT}; // what the escape key does
	if (wininfo.releasemode)
		dofilelogger=0;
// setup state system
	registerstatelist(states,numstates);
	if (startstate>=0)
		changestate(startstate);
// file strategy
	fopen2setctl(FOPEN2_COMMON,1);			// if file not found, look in common dir
	if (dofilelogger) {
		fopen2setctl(FOPEN2_STARTMONITOR,1);	// if you want you can monitor all files accessed
		pushandsetdir("config");		// get config.txt in the list after the fact..
		fileexist("config.txt");
		fileexist("configca.txt");
		popdir();
	}
}

void mainexit()
{
	if (dofilelogger) {
		fopen2setctl(FOPEN2_LOGMONITOR,1);		// write log to logfile
		fopen2setctl(FOPEN2_RESETMONITOR,1);	// free memory used by log
	}
	freemanifest();
}

// called just before window destroyed, also called when errorexit called
// limited resources available!
void latemainexit()
{
	stub_latemainexit();
/* calls
// on game success
	stub_normalResult(mainstubinfo.gamesplayed,
			mainstubinfo.gameswon,mainstubinfo.names.credits,
			mainstubinfo.names.experience,mainstubinfo.highestscore,
			mainstubinfo.place,mainstubinfo.bots,mainstubinfo.connections);
		writetalkfile("done");
// on failure DNF
	stub_failedResult(); // did not finish
	writetalkfile("failed");
*/
}