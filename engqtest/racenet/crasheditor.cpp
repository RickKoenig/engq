/*
#include <stdlib.h>
#include <string.h>

#include <engine1.h>
#include <misclib.h>

#include "stubhelper.h"

#include "line2road.h"
#include "online_uplay.h"

#include "crasheditor.h"
*/
#define D2_3D
#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "../engine7test/n_usefulcpp.h"
#include "gameinfo.h"
//#include "../engine7test/n_line2roadcpp.h"
#include "../hw35_line2road.h"
#include "crasheditor.h"
#include "online_uplay.h"
#include "online_seltrack.h"

struct crashinfo crashinfos;
struct checkinfo checkinfos;
//struct botaiinfo botaiinfos;

// a new home for crash reset, bot driving, checkpoints and laps

// controlled by debprint, trackscale assumed (for now) to be a 1, not anymore

// given checkpoints and crashresets, 
// figure out where to reset a car, given it place on the course
// generate standard n dot p = d for planes from a point and quat rot from z axis
static void calcplanes()
{
	pointf3 iv,n;
	int i;
	float d;
	iv=pointf3x(0,0,1);
	for (i=0;i<checkinfos.num;i++) {
		quatrot(&checkinfos.checklocs[i].rot,&iv,&n);
		normalize3d(&n,&checkinfos.checkplanes[i].n);
		d=dot3d(&checkinfos.checklocs[i].loc,&checkinfos.checkplanes[i].n);
		checkinfos.checkplanes[i].d=d;
	}
}

/*void calccrashresets()
{
}
*/
///////////// crash reset editor /////////////////////////////////////
void crasheditload()
{
/*
	char name[300],path[300];
	mgetpath(od.ol_uedittrackname,path);
pushandsetdir(path);
	mgetname(od.ol_uedittrackname,name);
	strcat(name,".chk");
*/
	trackinfo tinf;
	S32 tidx=tinf.findtrackidx(od.ntrackname);
	if (tidx<0)
		errorexit("neg tidx");
	string sname=tinf.gettrackname(tidx);
	string spath=tinf.gettrackdir(tidx);
pushandsetdir(spath.c_str());
	sname += ".rst";
	if (fileexist(sname.c_str())) {
//		fileopen(name,READ);
		FILE* fp=fopen2(sname.c_str(),"rb");
//		crashinfos.num=filereadlong2();
		fread(&crashinfos.num,sizeof(S32),1,fp);
		crashinfos.cur=0;
//		fileread(&crashinfos.crashlocs[0],sizeof(crashinfos.crashlocs[0])*crashinfos.num);
		fread(&crashinfos.crashlocs[0],sizeof(crashinfos.crashlocs[0]),crashinfos.num,fp);
//		fileclose();
		fclose(fp);
	} else {
		crashinfos.num=1;
		crashinfos.crashlocs[0].loc=zerov;
		crashinfos.crashlocs[0].rot=zerov;
		crashinfos.crashlocs[0].rot.w=1;
	}
	crashinfos.lastcur=-1;
popdir();
}

void crasheditsave()
{
/*
	char name[300],path[300];
	mgetpath(od.ol_uedittrackname,path);
pushandsetdir(path);
	mgetname(od.ol_uedittrackname,name);
	strcat(name,".chk");
*/
	trackinfo tinf;
	S32 tidx=tinf.findtrackidx(od.ntrackname);
	if (tidx<0)
		errorexit("neg tidx");
	string sname=tinf.gettrackname(tidx);
	string spath=tinf.gettrackdir(tidx);
pushandsetdir(spath.c_str());
	sname += ".rst";
//	fileopen(name,WRITE);
	FILE* fp=fopen2(sname.c_str(),"wb");
//	filewritelong2(crashinfos.num);
	fwrite(&crashinfos.num,sizeof(S32),1,fp);
//	filewrite(&crashinfos.crashlocs[0],sizeof(crashinfos.crashlocs[0])*crashinfos.num);
	fwrite(&crashinfos.crashlocs[0],sizeof(crashinfos.crashlocs[0]),crashinfos.num,fp);
//	fileclose();
	fclose(fp);
popdir();
}

void crasheditor(int millisec,struct ol_playerdata *op)
{
	int i;
	if (!crashinfos.editenable && crashinfos.lasteditenable) { // leaving
		crasheditsave();
		buildcheckresets();
		od.lastmillisec=millisec-100*op->clocktickcount/6;
	}
	if (crashinfos.editenable && !crashinfos.lasteditenable) { // entering
		crashinfos.doload=crashinfos.dosave=crashinfos.doinsert=crashinfos.dodelete=0;
		if (crashinfos.num==0) { // create one if none exists
			crashinfos.num=1;
			crashinfos.lastcur=-1;
			crashinfos.crashlocs[0].loc=zerov;
			crashinfos.crashlocs[0].rot=zerov;
			crashinfos.crashlocs[0].rot.w=1;
			crashinfos.curedit=crashinfos.crashlocs[0];
		}
	}
	if (crashinfos.editenable) { // editing
		crashinfos.cur=range(0,crashinfos.cur,crashinfos.num-1);
		if (crashinfos.cur!=crashinfos.lastcur) {
			crashinfos.curedit=crashinfos.crashlocs[crashinfos.cur];
			crashinfos.lastcur=crashinfos.cur;
		}
		quatnormalize(&crashinfos.curedit.rot,&crashinfos.curedit.rot);
		crashinfos.curedit.checkidx=range(0,crashinfos.curedit.checkidx,max(0,checkinfos.num-1));
		crashinfos.crashlocs[crashinfos.cur]=crashinfos.curedit;
		if (crashinfos.doinsert) {
			if (crashinfos.num<MAXCRASHRESETS) {
				for (i=crashinfos.num-1;i>=crashinfos.cur;i--)
					crashinfos.crashlocs[i+1]=crashinfos.crashlocs[i];
				crashinfos.num++;
				crashinfos.cur++;
			}
			crashinfos.doinsert=0;
		}
		if (crashinfos.dodelete) {
			if (crashinfos.num>1) {
				crashinfos.num--;
				for (i=crashinfos.cur;i<crashinfos.num;i++)
					crashinfos.crashlocs[i]=crashinfos.crashlocs[i+1];
			}
			crashinfos.dodelete=0;
		} 
		crashinfos.cur=range(0,crashinfos.cur,crashinfos.num-1);
		if (crashinfos.doload) {
			crasheditload();
			crashinfos.doload=0;
		}
		if (crashinfos.dosave) {
			crasheditsave();
			crashinfos.dosave=0;
		} 
//		for (i=0;i<crashinfos.num;i++)
//			if (!checkinfos.editenable || crashinfos.crashlocs[i].checkidx==checkinfos.cur)
//				addnull(&crashinfos.crashlocs[i].loc,&crashinfos.crashlocs[i].rot);
		i=crashinfos.cur;
		op->pos=crashinfos.crashlocs[i].loc;
		op->carang=crashinfos.crashlocs[i].rot;
	}
	crashinfos.lasteditenable=crashinfos.editenable;
}

//////////////////////// checkpoint planes //////////////////////////////
void checkeditload()
{
/*
	char name[300],path[300];
	mgetpath(od.ol_uedittrackname,path);
pushandsetdir(path);
	mgetname(od.ol_uedittrackname,name);
	strcat(name,".chk");
*/
	trackinfo tinf;
	S32 tidx=tinf.findtrackidx(od.ntrackname);
	if (tidx<0)
		errorexit("neg tidx");
	string sname=tinf.gettrackname(tidx);
	string spath=tinf.gettrackdir(tidx);
pushandsetdir(spath.c_str());
	sname += ".chk";
	if (fileexist(sname.c_str())) {
//		fileopen(name,READ);
		FILE* fp=fopen2(sname.c_str(),"rb");
//		checkinfos.num=filereadlong2();
		fread(&checkinfos.num,sizeof(S32),1,fp);
		checkinfos.cur=0;
//		fileread(&checkinfos.checklocs[0],sizeof(checkinfos.checklocs[0])*checkinfos.num);
		fread(&checkinfos.checklocs[0],sizeof(checkinfos.checklocs[0]),checkinfos.num,fp);
//		fileclose();
		fclose(fp);
	} else {
		checkinfos.num=1;
		checkinfos.checklocs[0].loc=zerov;
		checkinfos.checklocs[0].rot=zerov;
		checkinfos.checklocs[0].rot.w=1;
	}
	checkinfos.lastcur=-1;
popdir();
	calcplanes();
}

void checkeditsave()
{
/*
	char name[300],path[300];
	mgetpath(od.ol_uedittrackname,path);
pushandsetdir(path);
	mgetname(od.ol_uedittrackname,name);
	strcat(name,".chk");
*/
	trackinfo tinf;
	S32 tidx=tinf.findtrackidx(od.ntrackname);
	if (tidx<0)
		errorexit("neg tidx");
	string sname=tinf.gettrackname(tidx);
	string spath=tinf.gettrackdir(tidx);
pushandsetdir(spath.c_str());
	sname += ".chk";

//	fileopen(name,WRITE);
	FILE* fp=fopen2(sname.c_str(),"wb");
//	filewritelong2(checkinfos.num);
	fwrite(&checkinfos.num,sizeof(S32),1,fp);
//	filewrite(&checkinfos.checklocs[0],sizeof(checkinfos.checklocs[0])*checkinfos.num);
	fwrite(&checkinfos.checklocs[0],sizeof(checkinfos.checklocs[0]),checkinfos.num,fp);
//	fileclose();
	fclose(fp);
popdir();
}

void checkeditor(int millisec,struct ol_playerdata *op)
{
	int i;//,j;
	pointf3 v;//,vs[3];
	if (!checkinfos.editenable && checkinfos.lasteditenable) { // leaving
		checkeditsave();
		od.lastmillisec=millisec-100*op->clocktickcount/6;
		calcplanes();
		buildcheckresets();
	}
	if (checkinfos.editenable && !checkinfos.lasteditenable) { // entering
		checkinfos.doload=checkinfos.dosave=checkinfos.doinsert=checkinfos.dodelete=0;
		if (checkinfos.num==0) { // create one if none exists
			checkinfos.num=1;
			checkinfos.lastcur=-1;
			checkinfos.checklocs[0].loc=zerov;
			checkinfos.checklocs[0].rot=zerov;
			checkinfos.checklocs[0].rot.w=1;
			checkinfos.curedit=checkinfos.checklocs[0];
		}
	}
	if (checkinfos.editenable) { // editing
		checkinfos.cur=range(0,checkinfos.cur,checkinfos.num-1);
		if (checkinfos.cur!=checkinfos.lastcur) {
			checkinfos.curedit=checkinfos.checklocs[checkinfos.cur];
			checkinfos.lastcur=checkinfos.cur;
//			calccrashresets();
		}
		quatnormalize(&checkinfos.curedit.rot,&checkinfos.curedit.rot);
		checkinfos.checklocs[checkinfos.cur]=checkinfos.curedit;
		if (checkinfos.doinsert) {
			if (checkinfos.num<MAXCHECKPLANES) {
				for (i=checkinfos.num-1;i>=checkinfos.cur;i--)
					checkinfos.checklocs[i+1]=checkinfos.checklocs[i];
				for (i=0;i<crashinfos.num;i++)
					if (crashinfos.crashlocs[i].checkidx>=checkinfos.cur)
						crashinfos.crashlocs[i].checkidx++;
				checkinfos.num++;
				checkinfos.cur++;
			}
			checkinfos.doinsert=0;
		}
		if (checkinfos.dodelete) {
			if (checkinfos.num>1) {
				checkinfos.num--;
				for (i=checkinfos.cur;i<checkinfos.num;i++)
					checkinfos.checklocs[i]=checkinfos.checklocs[i+1];
				for (i=0;i<crashinfos.num;i++)
					if (crashinfos.crashlocs[i].checkidx>checkinfos.cur)
						crashinfos.crashlocs[i].checkidx--;
			}
			checkinfos.dodelete=0;
		} 
		checkinfos.cur=range(0,checkinfos.cur,checkinfos.num-1);
		if (checkinfos.doload) {
			checkeditload();
//			calccrashresets();
			checkinfos.doload=0;
		}
		if (checkinfos.dosave) {
			checkeditsave();
			checkinfos.dosave=0;
		} 
		for (i=0;i<checkinfos.num;i++) {
			v.x=0;
			v.y=0;
			v.z=2.5f;
			quatrot(&checkinfos.checklocs[i].rot,&v,&v);
//			if (i==checkinfos.cur)
//				addvec(&checkinfos.checklocs[i].loc,&v,rgbyellow);
//			else
//				addvec(&checkinfos.checklocs[i].loc,&v,rgbmagenta);
		}
/*		vs[0].x=-10;
		vs[0].y=10;
		vs[0].z=0;
		vs[1].x=10;
		vs[1].y=10;
		vs[1].z=0;
		vs[2].x=-10;
		vs[2].y=-10;
		vs[2].z=0; */
		i=checkinfos.cur;
//		quatrots(&checkinfos.checklocs[i].rot,vs,vs,3);
/*		for (i=0;i<3;i++) {
			vs[i].x+=checkinfos.checklocs[checkinfos.cur].loc.x;
			vs[i].y+=checkinfos.checklocs[checkinfos.cur].loc.y;
			vs[i].z+=checkinfos.checklocs[checkinfos.cur].loc.z;
		} */
//		i=checkinfos.cur;
//		addplane(&checkinfos.checklocs[i].loc,&checkinfos.checklocs[i].rot);
	}
	checkinfos.lasteditenable=checkinfos.editenable;
}

void setnewcrashloc(struct ol_playerdata *p)
{
	int cri,ncr,cri2;
	float crf;
	cri=p->chkpnt;
//	logger("in setnewcrashloc with carid %d, cri in %d, ",p->carid,cri);
	if (cri>0 || p->chklap>0) {
		ncr=checkinfos.ncrashresets[cri];
		if (ncr) {
			if (ncr>=2) {
				cri2=p->carid&1;
			} else {
				cri2=0;
			}
//			cri2=(p->clocktickcount>>6)%ncr;
//			logger("fetching [%d][%d] ",cri,cri2);
			cri=checkinfos.crashresets[cri][cri2];
//			logger("got %d\n",cri);
			p->ol_crashresetloc=crashinfos.crashlocs[cri].loc;
			p->ol_crashresetrot=crashinfos.crashlocs[cri].rot;
		}
		cri=p->chkpnt;
//		cri=p->chkpnt+checkinfos.num*p->chklap;
		crf=checkinfos.botseekframeresets[cri];
		if (crf) {
			p->ol_crashresetframe=crf;
//			if (p->cntl==CNTL_HUMAN)
			p->seekframe=p->ol_crashresetframe+od.pathbotresetadd;
//			logger("cri %d,got 'crf %f' add %f, seekframeset %f\n",
//				cri,crf,od.pathbotresetadd,p->seekframe);
			p->stuckwatchdog=0;
		}
	}
}

// new lap logic
void docheckpoints(struct ol_playerdata *p)
{
	struct checkplane *cp;
	if (p->chklap==od.laps)
		return;
	cp=&checkinfos.checkplanes[p->chkpnt];
	if (dot3d(&p->pos,&cp->n)>=cp->d) {
		setnewcrashloc(p);
		p->chkpnt++;
		if (p->chkpnt>=checkinfos.num) {
			p->chklap++;
		if (p->chklap!=od.laps)
			p->chkpnt-=checkinfos.num;
		}
	}
}

/////////////////// bot paths ////////////////////
#if 0
void boteditload()
{
	int i;
	char name[300],path[300];
	getpath(path,od.ol_uedittrackname);
	getname(name,od.ol_uedittrackname);
	pushandsetdir(path);
	strcat(name,".bot");
	if (fileexist(name)) {
		fileopen(name,READ);
		botaiinfos.num=filereadlong2();
		botaiinfos.cur=0;
		fileread(botaiinfos.botlocs[0],sizeof(botaiinfos.botlocs[0])*botaiinfos.num);
		fileclose();
	} else {
		botaiinfos.num=1;
		for (i=0;i<MAXBOTPATHS;i++)
			botaiinfos.botlocs[0][i].loc=zerov;
	}
	botaiinfos.lastcur=-1;
	botaiinfos.lastpathid=-1;
	popdir();
}

void boteditsave()
{
	char name[300],path[300];
	getpath(path,od.ol_uedittrackname);
	pushandsetdir(path);
	getname(name,od.ol_uedittrackname);
	strcat(name,".bot");
	fileopen(name,WRITE);
	filewritelong2(botaiinfos.num);
	filewrite(botaiinfos.botlocs[0],sizeof(botaiinfos.botlocs[0])*botaiinfos.num);
	fileclose();
	popdir();
}

void boteditor(int millisec,struct ol_playerdata *op)
{
	int i,j;
	if (!botaiinfos.editenable && botaiinfos.lasteditenable) { // leaving
		boteditsave();
		od.lastmillisec=millisec-100*op->clocktickcount/6;
	}
	if (botaiinfos.editenable && !botaiinfos.lasteditenable) { // entering
		botaiinfos.doload=botaiinfos.dosave=botaiinfos.doinsert=botaiinfos.dodelete=0;
		if (botaiinfos.num==0) { // create one if none exists
			botaiinfos.num=1;
			botaiinfos.lastcur=-1;
			botaiinfos.lastpathid=-1;
			for (i=0;i<MAXBOTPATHS;i++)
				botaiinfos.botlocs[0][i].loc=zerov;
			botaiinfos.curedit=botaiinfos.botlocs[0][0];
		}
	}
	if (botaiinfos.editenable) { // editing
		botaiinfos.cur=range(0,botaiinfos.cur,botaiinfos.num-1);
		botaiinfos.pathid=range(0,botaiinfos.pathid,MAXBOTPATHS-1);
		if (botaiinfos.pathid!=botaiinfos.lastpathid || botaiinfos.cur!=botaiinfos.lastcur) {
			botaiinfos.curedit=botaiinfos.botlocs[botaiinfos.cur][botaiinfos.pathid];
			botaiinfos.lastcur=botaiinfos.cur;
			botaiinfos.lastpathid=botaiinfos.pathid;
		}
		botaiinfos.botlocs[botaiinfos.cur][botaiinfos.pathid]=botaiinfos.curedit;
		if (botaiinfos.doinsert) {
			if (botaiinfos.num<MAXBOTLOC) {
				for (i=botaiinfos.num-1;i>=botaiinfos.cur;i--)
					for (j=0;j<MAXBOTPATHS;j++)
						botaiinfos.botlocs[i+1][j]=botaiinfos.botlocs[i][j];
				botaiinfos.num++;
				botaiinfos.cur++;
			}
			botaiinfos.doinsert=0;
		}
		if (botaiinfos.dodelete) {
			if (botaiinfos.num>1) {
				botaiinfos.num--;
				for (i=botaiinfos.cur;i<botaiinfos.num;i++)
					for (j=0;j<MAXBOTPATHS;j++)
						botaiinfos.botlocs[i][j]=botaiinfos.botlocs[i+1][j];
			}
			botaiinfos.dodelete=0;
		} 
		botaiinfos.cur=range(0,botaiinfos.cur,botaiinfos.num-1);
		if (botaiinfos.doload) {
			boteditload();
			botaiinfos.doload=0;
		}
		if (botaiinfos.dosave) {
			boteditsave();
			botaiinfos.dosave=0;
		} 
		for (i=0;i<botaiinfos.num;i++) {
			for (j=0;j<MAXBOTPATHS;j++)
				addnull(&botaiinfos.botlocs[i][j].loc,NULL);
//			if (i==checkinfos.cur) {
//				pointf3 q;
//				q.x=q.y=q.z=q.w=.25f;
//				addnull(&botaiinfos.botlocs[i].loc,&q);
//			}
		}
		i=botaiinfos.cur;
		j=botaiinfos.pathid;
		op->pos=botaiinfos.botlocs[i][j].loc;
	}
	botaiinfos.lasteditenable=botaiinfos.editenable;
}
#endif
// also builds nbot paths
void buildcheckresets()
{
	tree2* pt=od.path1;
	struct checkplane *pl;
	int cp=0;
	int cl=0;
	float f;
	int i,ci,ncr;//,j;
	memset(checkinfos.botseekframeresets,0,sizeof(checkinfos.botseekframeresets));
	freecheckresets();
// 3 passes
// pass 1, calc storage
	for (i=0;i<crashinfos.num;i++) {
		ci=crashinfos.crashlocs[i].checkidx;
		ncr=checkinfos.ncrashresets[ci];
		ncr++;
		checkinfos.ncrashresets[ci]=ncr;
	}
// pass 2, alloc storage
	for (i=0;i<checkinfos.num;i++)
		if (checkinfos.ncrashresets[i]) {
			ncr=checkinfos.ncrashresets[i];
//			checkinfos.crashresets[i]=memalloc(sizeof(int)*ncr);
			checkinfos.crashresets[i]=new int[ncr]; //memalloc(sizeof(int)*ncr);
			checkinfos.ncrashresets[i]=0;
		}
// pass 3, xfer data
	for (i=0;i<crashinfos.num;i++) {
		ci=crashinfos.crashlocs[i].checkidx;
		ncr=checkinfos.ncrashresets[ci];
		checkinfos.crashresets[ci][ncr++]=i;
		checkinfos.ncrashresets[ci]=ncr;
	}
// okay, now build the sync up checkpoints with the old path frames
	if (pt) { // && pt->proc) {
		for (f=0;f<=od.trackend;f+=.25f) {
			pt->curframe=f;
			(pt->proc)();
			pointf3 ptt=pt->trans;
			ptt.x*=od.ntrackscale;
			ptt.y*=od.ntrackscale;
			ptt.z*=od.ntrackscale;
			pl=&checkinfos.checkplanes[cp];
			if (dot3d(&ptt,&pl->n)>=pl->d) {
				checkinfos.botseekframeresets[cp+checkinfos.num*cl]=f;
				logger("checkplane %d, gets seekframe %f\n",cp+checkinfos.num*cl,f);
				cp++;
				if (cp>=checkinfos.num) {
					cp-=checkinfos.num;
					cl++;
				}
			}
		}
	}
}

void freecheckresets()
{
	int i;
	for (i=0;i<MAXCHECKPLANES;i++) {
		checkinfos.ncrashresets[i]=0;
		if (checkinfos.crashresets[i]) {
//			memfree(checkinfos.crashresets[i]);
			delete[] checkinfos.crashresets[i];
			checkinfos.crashresets[i]=NULL;
		}
	}
}
