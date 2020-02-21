/*#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <engine1.h>
#include <misclib.h>
*/
#include <m_eng.h>
#include "stubhelper.h"
//#include "stubcom.h"
#include "globalres.h"

struct stbinfo mainstubinfo;
//struct customstub carstubinfos[MAXSTUBCARS];
struct btinfo botinfo;


//void stub_earlymaininit(char *cmd)
//{
//}

void stub_maininit(int stubinited)
{
// phase 1
	if(!stubinited) {
//		stub_failedResult();
//		writetalkfile("failed");
//		strcpy(stubinfo.p1name,"jackers");
//		strcpy(stubinfo.p2name,"mwmonstersxyzxyz");
//		error("stub_readStartupInfo Failed! %s",lpszCmdParam);
//		return 1;
		strcpy(botinfo.botpaints[0],"paint01");
		strcpy(botinfo.botdecals[0],"decal01");
//		strcpy(botinfo.botbodys[0],"vulture23r");
		strcpy(botinfo.botpaints[1],"paint02");
		strcpy(botinfo.botdecals[1],"decal02");
//		strcpy(botinfo.botbodys[1],"vulture23r");
		strcpy(botinfo.botpaints[2],"paint03");
		strcpy(botinfo.botdecals[2],"decal03");
//		strcpy(botinfo.botbodys[2],"vulture23r");
	} else { //everything is fine
		int xres=0,yres=0,hard=0;
		unsigned int p1ip,p2ip;
//		char sname[100];
		const char *tmp;
		mainstubinfo.carid=-1;
		mainstubinfo.playerid=-1;
		mainstubinfo.ncars=-1;
		mainstubinfo.nplayers=-1;
		mainstubinfo.names.credits = stub_getValueInt("iCredits");
		mainstubinfo.names.experience = stub_getValueInt("iExper");
		mainstubinfo.highestscore = 0;
		mainstubinfo.port = stub_getValueInt("port");
		mainstubinfo.tracknum = stub_getValueInt("tracknum");
		if (mainstubinfo.tracknum==0)
			mainstubinfo.tracknum=1;
		mainstubinfo.rules = stub_getValueInt("rules");
		if (mainstubinfo.rules==0)
			mainstubinfo.rules=1;
//		strcpy(stubinfo.pnames[0],stub_getValueStr("p1name"));
//		strcpy(stubinfo.pnames[1],stub_getValueStr("p2name"));
		strcpy(mainstubinfo.names.name,stub_getValueStr("sName"));
		strcpy(mainstubinfo.names.paint,stub_getValueStr("sCarPaint"));
		strcpy(mainstubinfo.names.decal,stub_getValueStr("sCarDecal"));
		strcpy(mainstubinfo.names.body,stub_getValueStr("sCarBody"));
		strcpy(botinfo.botpaints[0],stub_getValueStr("botpaint"));
		strcpy(botinfo.botdecals[0],stub_getValueStr("botdecal"));
		strcpy(botinfo.botbodys[0],stub_getValueStr("botcar"));
		strcpy(botinfo.botpaints[1],stub_getValueStr("botpaint2"));
		strcpy(botinfo.botdecals[1],stub_getValueStr("botdecal2"));
		strcpy(botinfo.botbodys[1],stub_getValueStr("botcar2"));
		strcpy(botinfo.botpaints[2],stub_getValueStr("botpaint3"));
		strcpy(botinfo.botdecals[2],stub_getValueStr("botdecal3"));
		strcpy(botinfo.botbodys[2],stub_getValueStr("botcar3"));
		p1ip = str2ip((char *)stub_getValueStr("p1ip"));
		p2ip = str2ip((char *)stub_getValueStr("p2ip"));
		tmp=stub_getValueStr("numplayers");
		if (tmp==NULL || tmp[0]<'0' || tmp[0]>'9')
			mainstubinfo.ncars=4;
		else {
			mainstubinfo.ncars=atoi(tmp);
			// signal bots
			if (mainstubinfo.ncars>MAXSTUBCARS)
				mainstubinfo.ncars=MAXSTUBCARS;
			if (mainstubinfo.ncars<2)
				p1ip=0;
		}
		if (p1ip==0) { // bot mode
			mainstubinfo.nplayers=1;	// this disables networking (1 computer)
		} else if (p2ip==0xffffffff) { // server
			mainstubinfo.nplayers=mainstubinfo.ncars;
//			mainstubinfo.playerid=mainstubinfo.carid=0; // server only
		}
		mainstubinfo.serverip=p1ip;
		mainstubinfo.usestub=1;
//		strcpy(stubinfo.p2body,stubinfo.botbody);
//		strcpy(stubinfo.p2paint,stubinfo.botpaint);
//		strcpy(stubinfo.p2decal,stubinfo.botdecal);
		mainstubinfo.useslextra=stub_getValueInt("SleXtra");
		mainstubinfo.random1=stub_getValueInt("random1");
		mainstubinfo.random2=stub_getValueInt("random2");
		if (!mainstubinfo.random2)
			mainstubinfo.random1=0;	// just check random1 for 0
/*		if (!strcmp(stubinfo.pnames[0],sname)) { // server
			stubinfo.playerid=0;
		} else if (!strcmp(stubinfo.pnames[1],sname)) {
			stubinfo.playerid=-1; // client
			strcpy(stubinfo.pnames[0],stubinfo.pnames[1]);
			stubinfo.pnames[1][0]='\0';
		} else {
			stub_failedResult();
			writetalkfile("failed");
			error("stub_name mismatch '%s'",sname);
			return 1;
		} */
// lastly set the resolution and whether d3d hardware is to be used..
		xres=stub_getValueInt("xres");
		yres=stub_getValueInt("yres");
		hard=stub_getValueInt("vidmode"); 
		zoomin=stub_getValueInt("zoomin");
		if (hard==2) {
			video_init(VIDEO_D3D,1);
		} else if (hard==1) {
			video_init(VIDEO_GDI,0);
		}
		if (xres && yres) {
			extern int video_maindriver;
			globalxres=xres;
			globalyres=yres;
			softwarexres=xres;
			softwareyres=yres;
		}
	}
// phase 2
	if (mainstubinfo.usestub) {
		logger("credits %d, experience %d\n",mainstubinfo.names.credits,mainstubinfo.names.experience);
		logger("serverip %s\n",ip2str(mainstubinfo.serverip));
		logger("port %d\n",mainstubinfo.port);
		logger("nplayers %d, ncars %d\n",
			mainstubinfo.nplayers,mainstubinfo.ncars);
		logger("name '%s'\n",mainstubinfo.names.name);
		logger("body '%s'\n",mainstubinfo.names.body);
		logger("decal '%s'\n",mainstubinfo.names.decal);
		logger("paint '%s'\n",mainstubinfo.names.paint);
		logger("botbody '%s'\n",botinfo.botbodys[0]);
		logger("botdecal '%s'\n",botinfo.botdecals[0]);
		logger("botpaint '%s'\n",botinfo.botpaints[0]); 
		logger("useslextra %d\n",mainstubinfo.useslextra);
		logger("random1 %d\n",mainstubinfo.random1);
		logger("random2 %d\n",mainstubinfo.random2);
	}
	logger("dswmediadir '%s'\n",dswmediadir);
	logger("prefsdir '%s'\n",prefsdir);
	logger("commondir '%s'\n",commondir);

}

// called just before window destroyed, no logger, no memalloc, etc.
void stub_latemainexit()
{
	if (mainstubinfo.finished) {
		stub_normalResult(mainstubinfo.gamesplayed,
			mainstubinfo.gameswon,mainstubinfo.names.credits,
			mainstubinfo.names.experience,mainstubinfo.highestscore,
			mainstubinfo.place,mainstubinfo.bots,mainstubinfo.connections);
		writetalkfile("done");
	} else {
		stub_failedResult(); // did not finish
		writetalkfile("failed");
	}
}

