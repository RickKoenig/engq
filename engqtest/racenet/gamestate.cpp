#include <stdio.h>
#include <stdarg.h>
//#include "../thunderstruck/thunderstruck.h"
//#include <engine1.h>
#define D2_3D
#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "gameinfo.h"
//#include "line2road.h"
#include "../engine7test/n_usefulcpp.h"
//#include "../engine7test/n_line2roadcpp.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "packet.h"
#include "gamestate.h"

// look for out of sync problems

#ifdef CHECKGSTATE
static FILE *fwgs;

#define MAXCHECKSUMS 36000
static int localchecksums[MAXCHECKSUMS];
static int remotechecksums[MAXCHECKSUMS];
struct playerstatepacket *localplayerstates[MAXCHECKSUMS][2];
struct globalstate *localglobalstates[MAXCHECKSUMS];
static int cursumidx;
static void dumplongs(FILE *fw,unsigned int *data,int ndata)
{
	int i;
	for (i=0;i<ndata;i++) {
		if ((i&3)==0)
			fprintf(fw,"   %08X : ",i<<2);
		fprintf(fw,"%08X ",data[i]);
		if ((i&3)==3)
			fprintf(fw,"\n");
	}
	fprintf(fw,"\n");
}

static int getchecksum(unsigned int *data,int ndata)
{
	int i,cs=0;
	for (i=0;i<ndata;i++)
		cs+=data[i];
	if (cs==0)
		cs=1;
	if (cs==-1)
		cs=1;
	return cs;
}

static void initfwgs()
{
	if (!fwgs) {
		pushandsetdir("");
		if (wininfo.isalreadyrunning)
			fwgs=fopen2("gamestateb.txt","w");
		else
			fwgs=fopen2("gamestate.txt","w");
		popdir();
	}
}

void loggamestate(char *fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	if (!fwgs)
		initfwgs();
	if (fwgs)
		vfprintf(fwgs,fmt,arglist);
	va_end(arglist);
}

static void fetchplayer(int clk,struct playerstatepacket *pk,int pn)
{
	if (!localplayerstates[clk][pn])
		errorexit("bad fetchplayer");
	memcpy(pk,localplayerstates[clk][pn],sizeof(*pk));
}

static void fetchglobalstate(int clk,struct globalstate *gs)
{
	if (!localglobalstates[clk])
		errorexit("bad fetchglobalstate");
	memcpy(gs,localglobalstates[clk],sizeof(*gs));
}

static void freeplayer(int clk,int pn)
{
	if (localplayerstates[clk][pn]) {
		memfree(localplayerstates[clk][pn]);
		localplayerstates[clk][pn]=NULL;
	}
}

static void freeglobalstate(int clk)
{
	if (localglobalstates[clk]) {
		memfree(localglobalstates[clk]);
		localglobalstates[clk]=NULL;
	}
}

static void addplayer(int clk,struct playerstatepacket *pk,int pn)
{
	if (localplayerstates[clk][pn])
		errorexit("bad addplayer");
	localplayerstates[clk][pn]=(playerstatepacket*)memalloc(sizeof(*pk));
	memcpy(localplayerstates[clk][pn],pk,sizeof(*pk));
}

static void addglobalstate(int clk,struct globalstate *gs)
{
	if (localglobalstates[clk])
		errorexit("bad addglobal");
	localglobalstates[clk]=(globalstate *)memalloc(sizeof(*gs));
	memcpy(localglobalstates[clk],gs,sizeof(*gs));
}

static void checkchecksums()
{
//		fprintf(fwgs,"-------------- checksum error between game states clock %d, cslocal %08x, csremote %08x\n",
//			*clk,checksums[*clk],*cs);	
//	initfwgs();
	struct playerstatepacket pk;
	struct globalstate gs;
	unsigned int *data;
	int ndata;
	while (localchecksums[cursumidx] && remotechecksums[cursumidx] && cursumidx<MAXCHECKSUMS) {
		if (localchecksums[cursumidx]!=remotechecksums[cursumidx]) {
			fprintf(fwgs,"-------------- checksum between game states clock %d, cslocal %08x, csremote %08x\n",
				cursumidx,localchecksums[cursumidx],remotechecksums[cursumidx]);
			if (!od.quittimer)
				od.quittimer=od.quittime-20;
			fprintf(fwgs,"player 0\n");
			fetchplayer(cursumidx,&pk,0);
//			saveplayerstate(&opa[0],&pk);
			ndata=sizeof(pk)/sizeof(int);
			data=(U32*)&pk;
			dumplongs(fwgs,data,ndata);
			fprintf(fwgs,"player 1\n");
			fetchplayer(cursumidx,&pk,1);
//			saveplayerstate(&opa[1],&pk);
			ndata=sizeof(pk)/sizeof(int);
			data=(U32*)&pk;
			dumplongs(fwgs,data,ndata);
#ifdef SHOWGLOBALS
			fprintf(fwgs,"global data\n");
			fetchglobalstate(cursumidx,&gs);
//			saveglobalstate(&gs);
			data=(U32 *)&gs;
			ndata=sizeof(gs)/sizeof(int);
			dumplongs(fwgs,data,ndata); 
#endif
		}
		cursumidx++;
	}
}

/*void showgamestate()
{
	unsigned int *data;
	int ndata;
	struct playerstatepacket pk;
	return;
	initfwgs();
	if (!fwgs)
		return;
	if (od.predicted)
		return;
	fprintf(fwgs,"--------- clocktickcount %d\n",opa[0].clocktickcount);
	saveplayerstate(&opa[0],&pk);
	ndata=sizeof(pk)/sizeof(int);
	data=(int*)&pk;
	dumplongs(fwgs,data,ndata);
	fprintf(fwgs,"offset for 'botbigscan' is %08x\n",&((struct playerstatepacket*)0)->botbigscan);
}
*/

// recieve a gamestate packet from other player
void checkgstate(int frm,U8 *str)
{
	int *clk=(int *)(&str[0]);
	int *cs=(int *)(&str[4]);
//	initfwgs();
	if (od.numplayers==od.numcurplayers)
		remotechecksums[*clk]=*cs;
//	logger("setting remote checksum for clk %d to %08x\n",*clk,*cs);
	if (1) {
/*		fprintf(fwgs,"player 0\n");
		saveplayerstate(&opa[0],&pk);
		ndata=sizeof(pk)/sizeof(int);
		data=(int*)&pk;
		dumplongs(fwgs,data,ndata);
		fprintf(fwgs,"player 1\n");
		saveplayerstate(&opa[1],&pk);
		ndata=sizeof(pk)/sizeof(int);
		data=(int*)&pk;
		dumplongs(fwgs,data,ndata);
		fprintf(fwgs,"global data\n");
		data=(int *)od.weapflys;
		ndata=sizeof(od.weapflys)/sizeof(int);
		dumplongs(fwgs,data,ndata); */
	}
}

// send a gamestate packet to other player
void addgstate()
{
	int i;
	struct playerstatepacket pk;
	struct globalstate gs;
	unsigned int *data;
	int ndata;
	int chksum=0;
	char str[8];
	int *clk=(int *)(&str[0]);
	int *cs=(int *)(&str[4]);
	*clk=opa[0].clocktickcount;
	if (*clk>=MAXCHECKSUMS)
		return;
	saveplayerstate(&opa[0],&pk);
	pk.ol_crashresetbox=0;
	pk.boxchk=0;
	addplayer(*clk,&pk,0);
	ndata=sizeof(pk)/sizeof(int);
	data=(U32*)&pk;
	pk.stuckwatchdog=0;
	chksum+=getchecksum(data,ndata);
//	fprintf(fwgs,"player 1\n");
	saveplayerstate(&opa[1],&pk);
	pk.ol_crashresetbox=0;
	pk.boxchk=0;
	addplayer(*clk,&pk,1);
	ndata=sizeof(pk)/sizeof(int);
	data=(U32*)&pk;
	pk.stuckwatchdog=0;
	chksum+=getchecksum(data,ndata);
//	fprintf(fwgs,"global data\n");
	saveglobalstate(&gs);
	ndata=sizeof(gs)/sizeof(int);
	data=(U32 *)&gs;
	for (i=0;i<MAXWEAPFLY;i++)
		gs.weapflys[i].proc=NULL;
	chksum+=getchecksum(data,ndata);
	*cs=chksum;
	sendapacket(od.playerid,-1,PKT_GSTATE,8,str); // if 'to' is -1 then to all
	localchecksums[*clk]=*cs;
//	logger("setting local checksum for clk %d to %08x\n",*clk,*cs);
	addglobalstate(*clk,&gs);
	checkchecksums();
}

void initgstate()
{
	int i;
	initfwgs();
	for (i=0;i<MAXCHECKSUMS;i++) {
		localplayerstates[i][0]=NULL;
		localplayerstates[i][1]=NULL;
		localglobalstates[i]=NULL;
	}
	cursumidx=1;
	logger("sizeof globalstate is %d\n",sizeof(struct globalstate));
	fprintf(fwgs,"carena trackchecksum is %08x\n",gettrackchecksum());
}

void exitgstate()
{
	int i;
	for (i=0;i<MAXCHECKSUMS;i++) {
		if (localplayerstates[i][0])
			memfree(localplayerstates[i][0]);
		localplayerstates[i][0]=NULL;
		if (localplayerstates[i][1]) {
			memfree(localplayerstates[i][1]);
		}
		localplayerstates[i][1]=NULL;
		if (localglobalstates[i]) {
			memfree(localglobalstates[i]);
		}
		localglobalstates[i]=NULL;
		localchecksums[i]=0;
		remotechecksums[i]=0;
	}
}

#else
void initgstate()
{
}

void addgstate()
{
}

void exitgstate()
{
}

#endif
