#include <string.h>
#include <stdio.h>
#include <math.h>

#if 0
//#include <engine1.h>
#include <thunderstruck.h>

#include "stubhelper.h"
#include "line2road.h"
#include "online_uplay.h"
#include "globalres.h"
#include "boxai.h"
#include "box2box.h"
#else
#define D2_3D
#include <m_eng.h>
#include "../engine7test/n_usefulcpp.h"
#include "stubhelper.h"
#include "../engine7test/n_line2roadcpp.h"
#include "online_uplay.h"
#include "hw35_boxai.h"
#include "box2box.h"
#endif

typedef tree2 TREE;
#define MAXPATH 5
#define fuser1 userfloat[1]
#define fuser2 userfloat[2]

static struct chkbox *chkboxheader;
static struct chkbox **chkboxes;
static U32 nchkboxes;
static TREE *boxtree;
static TREE *lasttrees[MAXPATH];
static U32 nlasttrees;
//static struct texture *boxtex;
//extern int useloopcam;

/*char *scantestnames[]={
	"chk5.mxo",
	"chk6.mxo",
	"chk7.mxo",
	"chk8.mxo",
	"chk5b.mxo",
	"chk6b.mxo",
	"chk7c.mxo",
	"chk5c.mxo",
	"chk6c.mxo",
	"chk3c.mxo",
	"chk4c.mxo",
	"chk3.mxo",
	"chk4.mxo",
	"chk2.mxo",
	"chk1.mxo",
};
#define NTESTNAMES (sizeof(scantestnames)/sizeof(scantestnames[0]))
*/
int showcheckboxes;
//static float pathnums[MAXPATH];

char *tcondstrs[]={"p1","pa"};
#define NTCONDSTRS (sizeof(tcondstrs)/sizeof(tcondstrs[0]))

struct cbsound *cbsounds;
U32 ncbsounds;

static void portalopeninit(TREE *t)
{
	t->fuser1=0;
	t->scale=pointf3x(t->fuser1,t->fuser1,t->fuser1);
	t->userproc=NULL;
}

static bool portalopenproc(TREE *t)
{
	t->fuser1+=wininfo.microseccurrent*(1.0f/1000000.0f);
	if (t->fuser1>=1)
		t->fuser1=1;
	t->scale=pointf3x(t->fuser1,t->fuser1,t->fuser1);
	return true;
}

static void portalcloseinit(TREE *t)
{
	t->fuser2=1;
	t->scale=pointf3x(t->fuser2,t->fuser2,t->fuser2);
	t->userproc=NULL;
}

static bool portalcloseproc(TREE *t)
{
	t->fuser2-=wininfo.microseccurrent*(1.0f/1000000.0f);
	if (t->fuser2<=0)
		t->fuser2=0;
	t->scale=pointf3x(t->fuser2,t->fuser2,t->fuser2);
	return true;
}

struct triggerproc {
	char *name;
	void (*init)(TREE *);
	bool (*userproc)(TREE *);
};

struct triggerproc triggerprocs[]={
	"portalopen",portalopeninit,portalopenproc,
	"portalclose",portalcloseinit,portalcloseproc,
};

#define NTRIGGERPROCS (sizeof(triggerprocs)/sizeof(triggerprocs[0]))
	
static void scanforboxes(TREE *t)
{
	struct chkbox *c;
	char name[100];
	char suffix[10];
	char numstr[10];
	U32 i,j,k;
	strcpy(name,t->name.c_str());
	my_strlwr(name);
	if (strlen(name)>=4) {
		if (!strncmp(name,"chk",3)) {
			if ((nchkboxes&7)==0)
				chkboxes=(struct chkbox **)memrealloc(chkboxes,sizeof(struct chkbox *)*(nchkboxes+8));
			c=chkboxes[nchkboxes]=(chkbox*)memalloc(sizeof(struct chkbox));
			memset(c,0,sizeof(struct chkbox));
			mystrncpy(c->name,name,NAMESIZE);
			i=strlen(name)-5;
			while(i>3 && name[i]>='a' && name[i]<='z')
				i--;
			i++;
			strcpy(suffix,&name[i]);
			name[i]='\0';
			strcpy(numstr,&name[3]);
			suffix[strlen(suffix)-4]='\0';
			c->num=(float)atof(numstr);
			logger("   box found, name '%s', num %f, suffix %s\n",t->name.c_str(),c->num,suffix);
//			c->t=t;
			if (!t->mod)
				errorexit("'%s' not a box (1)",t->name.c_str());
			c->bmin=t->mod->boxmin;
			c->bmax=t->mod->boxmax;
			c->pos=t->trans;
			c->rot=t->rot; 
//			if (!showcheckboxes)
// look for sounds
			c->soundnum=-1;
			U32 nuserprop=t->userprop.num();
			for (j=0;j<nuserprop;j++)
				if (!my_stricmp("cbsound",t->userprop.idx(j).c_str()))
					break;
			if (j+1<nuserprop) {
				logger("found a cbsound '%s'\n",t->userprop.idx(j+1).c_str());
				for (k=0;k<ncbsounds;k++)
					if (!my_stricmp(cbsounds[k].name,t->userprop.idx(j+1).c_str()))
						break;
				if (k==ncbsounds) {
					cbsounds=(cbsound*)memrealloc(cbsounds,(ncbsounds+1)*sizeof(struct cbsound));
					cbsounds[k].wh=wave_load(t->userprop.idx(j+1).c_str(),0);
					mystrncpy(cbsounds[k].name,t->userprop.idx(j+1).c_str(),50);
					ncbsounds++;
				}
				c->soundnum=k;
			}
			if (showcheckboxes) {
				TREE *t1,*t2;
//				t1=alloctree(1,NULL);
				t1=new tree2("t1");
				t1->trans=t->trans;
				t1->rot=t->rot;
				t1->buildo2p=O2P_FROMTRANSQUATSCALE;
//				t2=duptree(boxtree);
				t2=boxtree->newdup();
				t2->trans=t->mod->boxmin;
				t2->scale.x=t->mod->boxmax.x-t->mod->boxmin.x;
				t2->scale.y=t->mod->boxmax.y-t->mod->boxmin.y;
				t2->scale.z=t->mod->boxmax.z-t->mod->boxmin.z;
				t1->linkchild(t2);
				od.ol_root->linkchild(t1);
				c->bt=t1;
				c->bt->flags|=TF_DONTDRAWC;
			}
/*			else {
				if (t->mod && t->mod->nmat>0) {
					if (t->mod->mats[0].thetex) {
						logger("freeing tex %08x '%s' ref %d\n",
							t->mod->mats[0].thetex,t->mod->mats[0].thetex->name,t->mod->mats[0].thetex->refcount);
						freetexture(t->mod->mats[0].thetex);
					}
					t->mod->mats[0].thetex=boxtex;
					t->mod->usetexmat=1;
					t->mod->mats[0].st=STATE_TEX_WITH_ALPHA;
					boxtex->refcount++;
				} 
			} */
			i=0;
			while(suffix[i]) {
				if (suffix[i]>='a' && suffix[i]<'a'+MAXPATH-1)
					c->pathnum=suffix[i]-'a'+1;
				switch(suffix[i]) {
				case 'f':
					c->isfastweap=1;
					break;
//				case 's':
//					c->isslowreset=1;
//					break;
				case 'h':
					c->ishalfspeed=1;
					break;
				case 'i':
					c->isbotpath=1;
					break;
				case 'l':
					c->isloopcam=1;
					break;
				case 'm':
					c->ismerge=1;
					break;
				case 'r':
					c->isreset=1;
					break;
				case 't':
					c->isportal=1;
					break;
				case 'u':
					c->isboost=1;
					break;
				case 'v':
					c->isloopcam=2;
					break;
				case 'y':
					c->isfog=2;
					break;
				case 'z':
					c->isfog=1;
					break;
				}
				i++;
			}
			c->isslowreset=1;
			nchkboxes++;
		}
	}
	list<tree2*>::const_iterator it;
	for (it=t->children.begin();it!=t->children.end();++it)
		scanforboxes(*it);
}

static void scanfortriggers(TREE *t)
{
	struct chkbox *c;
	char name[100];
	U32 i,j,k,m,len;
	U32 nuserprop=t->userprop.num();
// look for trigger objects first
	for (i=0;i+3<nuserprop;i++) {
		if (!my_stricmp(t->userprop.idx(i).c_str(),"trigger")) {
			logger("trigger found in '%s',",t->name.c_str());
			for (k=0;k<NTCONDSTRS;k++)
				if (!my_stricmp(tcondstrs[k],t->userprop.idx(i+2).c_str()))
					break;
			if (k==NTCONDSTRS)
				errorexit("bad trigger condition '%s'",t->userprop.idx(i+2).c_str());
			for (j=0;j<NTRIGGERPROCS;j++)
				if (!my_stricmp(triggerprocs[j].name,t->userprop.idx(i+3).c_str()))
					break;
			if (j==NTRIGGERPROCS)
				errorexit("bad trigger function '%s'",t->userprop.idx(i+3).c_str());
			for (m=0;m<nchkboxes;m++) {
				strcpy(name,chkboxes[m]->name);
				len=strlen(name);
				if (len>=4)
					name[len-4]='\0';
				if (!my_stricmp(name,t->userprop.idx(i+1).c_str()))
					break;
			}
			if (m==nchkboxes)
				errorexit("bad trigger box name '%s'",t->userprop.idx(i+1).c_str());
			if (triggerprocs[j].init)
				(*triggerprocs[j].init)(t);
			t->userproc=NULL;
			c=chkboxes[m];
			c->triggers=(TREE **)memrealloc(c->triggers,sizeof(TREE *)*(c->ntriggers+1));			
			c->triggercond=(int *)memrealloc(c->triggercond,sizeof(int)*(c->ntriggers+1));			
			c->triggerprocidx=(int *)memrealloc(c->triggerprocidx,sizeof(int)*(c->ntriggers+1));			
			c->triggers[c->ntriggers]=t;
			c->triggercond[c->ntriggers]=k;
			c->triggerprocidx[c->ntriggers]=j;
			logger("source '%s', condition '%s', function '%s'\n",chkboxes[m]->name,tcondstrs[k],triggerprocs[j]);
			c->ntriggers++;
		}
	}
	list<tree2*>::const_iterator it;
	for (it=t->children.begin();it!=t->children.end();++it)
		scanfortriggers(*it);
}

/*static void scanforboxestest()
{
	struct chkbox *c;
	char name[100];
	char suffix[10];
	char numstr[10];
	int i,j;
	for (j=0;j<NTESTNAMES;j++) {
		strcpy(name,scantestnames[j]);
		my_strlwr(name);
		if (strlen(name)>=4) {
			if (!strncmp(name,"chk",3)) {
				i=strlen(name)-5;
				while(i>3 && name[i]>='a' && name[i]<='z')
					i--;
				i++;
				strcpy(suffix,&name[i]);
				name[i]='\0';
				strcpy(numstr,&name[3]);
				suffix[strlen(suffix)-4]='\0';
				if ((nchkboxes&7)==0)
					chkboxes=(struct chkbox **)memrealloc("boxestest",chkboxes,sizeof(struct chkbox *)*(nchkboxes+8));
				c=chkboxes[nchkboxes]=memzalloc("boxestest2",sizeof(struct chkbox));
				c->num=(float)atof(numstr);
				logger("   box found, name '%s', num %f, suffix '%s'\n",scantestnames[j],c->num,suffix);
//				c->t=t;
				i=0;
				while(suffix[i]) {
					if (suffix[i]>='a' && suffix[i]<'a'+MAXPATH-1)
						c->pathnum=suffix[i]-'a'+1;
					switch(suffix[i]) {
					case 'r':
						c->isreset=1;
						break;
					case 'f':
						c->isfastweap=1;
						break;
					case 's':
						c->isslowreset=1;
						break;
					case 'h':
						c->ishalfspeed=1;
						break;
					case 'l':
						c->isloopcam=1;
						break;
					case 'i':
						c->isbotpath=1;
						break;
					case 'm':
						c->ismerge=1;
						break;
					}
					i++;
				}
				nchkboxes++;
			}
		}
	}
} */

struct chkbox *findprev(float time)
{
	U32 i;
	struct chkbox *cur=NULL;
	float curlow=-1e20f;
	for (i=0;i<nchkboxes;i++) {
		if (chkboxes[i]->num>curlow && chkboxes[i]->num<time && chkboxes[i]->pathnum==0) {
			cur=chkboxes[i];
			curlow=cur->num;
		}
	}
	return cur;
}

struct chkbox *findnext(float time)
{
	U32 i;
	struct chkbox *cur=NULL;
	float curlow=1e20f;
	for (i=0;i<nchkboxes;i++) {
		if (chkboxes[i]->num<curlow && chkboxes[i]->num>time && chkboxes[i]->pathnum==0) {
			cur=chkboxes[i];
			curlow=cur->num;
		}
	}
	return cur;
}

static void sortboxes()
{
	U32 i,j;
	float lowestnum=-1e20f,curlow;
	struct chkbox *last=NULL,*cur;
	chkboxheader=NULL;
// first build path 0
	while(1) {
		curlow=1e20f;
		cur=NULL;
		for (i=0;i<nchkboxes;i++) {
			if (chkboxes[i]->num<curlow && chkboxes[i]->num>lowestnum && chkboxes[i]->pathnum==0) {
				cur=chkboxes[i];
				curlow=cur->num;
			}
		}
		if (cur==NULL)
			break;
		lowestnum=curlow;
		if (chkboxheader==NULL)
			last=chkboxheader=cur;
		else {
			cur->prev[0]=last;
			cur->nprev=1;
			last->next[0]=cur;
			last->nnext=1;
		}
		last=cur;
	}
// some other paths
	for (j=1;j<MAXPATH;j++) {
		lowestnum=-1e20f;
		last=NULL;
		while(1) {
			curlow=1e20f;
			cur=NULL;
			for (i=0;i<nchkboxes;i++) {
				if (chkboxes[i]->num<curlow && chkboxes[i]->num>lowestnum && chkboxes[i]->pathnum==j) {
					cur=chkboxes[i];
					curlow=cur->num;
				}
			}
			if (cur==NULL)
				break;
			lowestnum=curlow;
			if (last==NULL)
				last=findprev(curlow);
			cur->prev[0]=last;
			cur->nprev=1;
			last->next[last->nnext]=cur;
			last->nnext++;
			last=cur;
			if (last->ismerge) {
				cur=findnext(last->num);
				last->next[0]=cur;
				last->nnext=1;
				cur->prev[cur->nprev]=last;
				cur->nprev++;
				last=NULL;
			}
		}
		if (last) {
			cur=findnext(last->num);
			last->next[0]=cur;
			last->nnext=1;
			cur->prev[cur->nprev]=last;
			cur->nprev++;
		}
	} 
// find places to check times
	i=0;
	cur=chkboxheader;
	while(cur) {
		if (cur!=chkboxheader) {
			i-=(cur->nprev-1);
			if (i==0)
				cur->istimer=1;
		}
		i+=(cur->nnext-1);
		cur=cur->next[0];
	}
// print out paths
	cur=chkboxheader;
	for (j=0;j<nchkboxes;j++) {
		cur=chkboxes[j];
		logger("cur %f(%d) (t%d):   ",cur->num,cur->pathnum,cur->istimer);
		for (i=0;i<cur->nnext;i++)
			logger("next[%d] %f(%d), ",i,cur->next[i]->num,cur->next[i]->pathnum);
		for (i=0;i<cur->nprev;i++)
			logger("prev[%d] %f(%d), ",i,cur->prev[i]->num,cur->prev[i]->pathnum);
		logger("\n");
	}
}

void boxai_init()
{
	if (showcheckboxes) {
//		TREE *t;
		pushandsetdir("helperobj");
//		boxtex=loadfindtexture("wiref.dds","wiref.dds");
//		t=loadscene("cornerbox1.mxs");
//		t = new tree2();
//		boxtree=duptree(findtreenamerec(t,"cornerbox1.mxo"));
//		delete t;	
		boxtree=new tree2("boxtree");
		popdir();
	} 
	nchkboxes=0;
	logger("scan for boxes...\n");
	scanforboxes(od.ol_root);
	scanfortriggers(od.ol_root);
//	scanforboxestest();
	sortboxes();
	if (showcheckboxes) {
		delete boxtree; //freetree(boxtree);
//		freetexture(boxtex);
//		boxtex=NULL;
	} 
	nlasttrees=0;
}

void findresetpoint(VEC *p)
{
	VEC top,bot,intsect;
	top=bot=*p;
	bot.y+=-1.0f;
	top.y+=1.0f;
	if (st2_line2road(&top,&bot,&intsect,NULL)) {
		*p=intsect;
		p->y+=od.hipoint;
	} else {
		bot.y=-5.0f;
		top.y=5.0f;
		if (st2_line2road(&top,&bot,&intsect,NULL)) {
			*p=intsect;
			p->y+=od.hipoint;
		}
	}
}
// one car at a time, drive ai cars
void boxai_ai()
{
	VEC d;
	VEC qi;
	float dh;
	struct chkbox *cb;
	cb=op->boxchk;
	op->pi.ol_uup=0;
	op->pi.ol_udown=0;
	op->pi.ol_uright=0;
	op->pi.ol_uleft=0;
	if (!cb)
		return;
	if (!chkboxheader)
		return;
	if (!cb->isslowreset)
		op->stuckwatchdog=0;
	else if (op->clocktickcount<INITCANTSTARTDRIVE)
		op->stuckwatchdog=0;
	else if (op->venableicrms)
		op->stuckwatchdog=0;
	else if (op->ol_accelspin>.5f*op->ol_startaccelspin || op->ol_airtime>10)
		op->stuckwatchdog=0;
	else
		op->stuckwatchdog++;
	if (op->stuckwatchdog>od.boxaimaxwatchdog) {
		if (op->cntl==CNTL_AI || od.playernet[op->carid].onebotplayer) {// && !od.disablespace)
//			logger("car reset: space generated for ai car that's stuck\n");
			op->pi.uspace=1;
		}
		op->stuckwatchdog=0;
	}
//	if (op->ol_accelspin>.5f*op->ol_startaccelspin) {
	if (cb->ishalfspeed && op->ol_accelspin>.5f*op->ol_startaccelspin) {
		op->pi.ol_uup=0;
		op->pi.ol_udown=1;
	} else
		op->pi.ol_uup=1;
	d.x=cb->pos.x-op->pos.x;
	d.y=cb->pos.y-op->pos.y;
	d.z=cb->pos.z-op->pos.z;
	quatinverse(&op->carang,&qi);
	quatrot(&qi,&d,&d);
	op->dh=(float)(-atan2(d.x,d.z));
//	op->dh=1.580275;
//	logger("op dh is %f\n",op->dh);
	if (op->dh>od.aiturndeadzone)
		dh=op->dh-od.aiturndeadzone;
	else if (op->dh<-od.aiturndeadzone)
		dh=op->dh+od.aiturndeadzone;
	else
		dh=0;
	if (dh>3*PI/4 || dh<-3*PI/4)
		od.wrongway++;
	else
		od.wrongway=0;
	if (op->ol_airtime)
		dh*=.25f;
	op->aiturnval+=dh*od.aiturnratio;
//	if (op->useloopcam!=1) {
		if (op->aiturnval>=1) {
			op->pi.ol_uleft=1;
			op->pi.ol_uright=0;
			op->aiturnval=0;
		} else if (op->aiturnval<=-1) {
			op->pi.ol_uleft=0;
			op->pi.ol_uright=1;
			op->aiturnval=0;
		}
//	}
// stunts
	if (op->ol_airtime>2)
		op->pi.ol_uup=0;
	if (op->ol_startstunt /* && op->useloopcam!=1 */) {
		if (op->doaistunt[0] && op->ol_uready) {
			op->pi.ol_uup=op->pi.ol_udown=0;
			if (op->curstuntangs.x<op->stuntgoals.x-40) {
				op->pi.ol_uup=1;
			} else if (op->curstuntangs.x>op->stuntgoals.x+40) {
				op->pi.ol_udown=1;
			} else {
				op->doaistunt[0]=0;
			}
		}
		if (op->doaistunt[1]) {
			op->pi.ol_uleft=op->pi.ol_uright=0;
			if (op->curstuntangs.y<op->stuntgoals.y-40) {
				op->pi.ol_uright=1;
			} else if (op->curstuntangs.y>op->stuntgoals.y+40) {
				op->pi.ol_uleft=1;
			} else {
				op->doaistunt[1]=0;
			}
		}
		if (op->doaistunt[2]) {
			op->pi.ol_urollleft=op->pi.ol_urollright=0;
			if (op->curstuntangs.z<op->stuntgoals.z-40) {
				op->pi.ol_urollleft=1;
			} else if (op->curstuntangs.z>op->stuntgoals.z+40) {
				op->pi.ol_urollright=1;
			} else {
				op->doaistunt[2]=0;
			}
		}
	}
}

// checkpoint_reset system, and finish
void boxai_check()
{
/*
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
*/
//	op->chklap=od.laps;
//int line2box(VEC *objpos,VEC *objrot,VEC *oabbmin,VEC *oabbmax,VEC *p0,VEC *p1,VEC *intsect)
	U32 i;
	U32 bi;
	float distsq,bdistsq;
	int id;
	struct chkbox *cb,*pcb;
	cb=op->boxchk;
	if (!cb)
		return;
	if (cb->nprev) {
		pcb=cb->prev[0];
		if (pcb->nnext>=2) {
			for (bi=0;bi<pcb->nnext;bi++)
				if (cb==pcb->next[bi])
					break;
			bdistsq=dist3dsq(&op->pos,&pcb->next[bi]->pos);
			for (i=0;i<pcb->nnext;i++) {
				distsq=dist3dsq(&op->pos,&pcb->next[i]->pos);
				if (distsq<.5f*bdistsq) {
					bdistsq=distsq;
					bi=i;
				}
				if (bi>=0)
					cb=pcb->next[bi];
			}
		}
	}
	id=op->carid;
	while(1) {
		if (!line2box(&cb->pos,&cb->rot,&cb->bmin,&cb->bmax,&op->ol_ulastpos,&op->pos,NULL))
			break;
		for (i=0;i<nlasttrees;i++)
			lasttrees[i]->flags|=TF_DONTDRAWC;
		if (op==of && cb->isfog)
			setfog(cb->isfog);
		if (op==of && cb->soundnum>=0 && !od.intimewarp)
			playasound(cbsounds[cb->soundnum].wh);
		nlasttrees=0;
		if (showcheckboxes) {
			nlasttrees=cb->nnext;
			for (i=0;i<nlasttrees;i++) {
				lasttrees[i]=cb->next[i]->bt;
				lasttrees[i]->flags&=~TF_DONTDRAWC;
			}
		}
		if (cb->nprev && cb->isreset) {
			op->ol_crashresetloc=cb->pos;
			findresetpoint(&op->ol_crashresetloc);
			op->ol_crashresetrot=cb->rot;
			op->ol_crashresetbox=cb;
		}
		if (op==of) {
//			if (cb->isloopcam)
//				op->useloopcam=1;
//			else
//				op->useloopcam=0;
		}
//		op->useloopcam=cb->isloopcam;
//		op->chkboost=cb->isboost;
//		if (!od.predicted) {
//		if (1) {
			if (cb->istimer && cb->caridchk[id]==0) {
				cb->caridchk[id]=1;
				if (cb->npassed==0)
					cb->ftime=op->clocktickcount;
				cb->npassed++;		
				if (cb->npassed==2)
					od.signalfirst=cb->ftime-op->clocktickcount;
				if (of==op) {
					int p;
					p=cb->npassed;
					if (p>=2)
						updatedifftime(op->clocktickcount-cb->ftime);
				}
			}
//		}
		if (cb->isportal && cb->next[0]) {
			op->pos.x+=cb->next[0]->pos.x-cb->pos.x;
			op->pos.y=cb->next[0]->pos.y;
			op->pos.z+=cb->next[0]->pos.z-cb->pos.z;
//			findresetpoint(&op->pos);
			op->pos.y+=.5f;
		}
//		if (!od.intimewarp) {
			for (i=0;i<cb->ntriggers;i++) {
				if (cb->npassed==1 && cb->triggercond[i]==TCOND_P1 || 
					cb->npassed==od.numcars && cb->triggercond[i]==TCOND_PA)
					if (triggerprocs[cb->triggerprocidx[i]].userproc)
						cb->triggers[i]->userproc=triggerprocs[cb->triggerprocidx[i]].userproc;
			}
//		}
		if (cb->isbotpath && cb->nnext>1) {
			cb=cb->next[id%cb->nnext];
		} else {
			cb=cb->next[0];
		}
		if (cb==NULL) {
			op->chklap=od.laps;
			op->boxchk=NULL;
			break;
		}
	}
	op->boxchk=cb;
}

void boxai_exit()
{
	U32 i;
	for (i=0;i<nchkboxes;i++) {
		if (chkboxes[i]->ntriggers) {
			memfree(chkboxes[i]->triggers);
			memfree(chkboxes[i]->triggercond);
			memfree(chkboxes[i]->triggerprocidx);
		}
		memfree(chkboxes[i]);
	}
	if (chkboxes)
		memfree(chkboxes);
	chkboxes=NULL;
	chkboxheader=NULL;
	nchkboxes=0;
	for (i=0;i<ncbsounds;i++)
		wave_unload(cbsounds[i].wh);
	if (cbsounds)
		memfree(cbsounds);
	cbsounds=NULL;
	ncbsounds=0;
}

//void boxai_getstartpos(VEC *sp,VEC *rot,int car)
void boxai_getstartpos()
//VEC *sp,VEC *rot,int car)
{
	int car;
//	struct chkbox *cb;
	int i,j,nrows,nc;
	car=op->carid;
	if (!chkboxheader) {
		op->pos.z=-.75f*((car+10)/5);
		op->pos.x=.4f*((car+1)%5)-.25f*(4-1);
		op->carang.x=op->carang.y=op->carang.z=0;
		op->carang.w=1;
		return;
	}
	j=car/5;
	i=car%5;
	nrows=(od.numcars+4)/5;
	op->pos.z=1.15f-1.05f*(j+4-nrows);
	if (nrows==j+1)
		nc=(od.numcars+4)%5+1;
	else
		nc=5;
	op->pos.x=.6f*i-.3f*(nc-1);

/*	if (chkboxheader->nnext>0) {
		VEC d;
		cb=chkboxheader->next[0];
		d.x=cb->pos.x-chkboxheader->pos.x;
		d.y=cb->pos.y-chkboxheader->pos.y;
		d.z=cb->pos.z-chkboxheader->pos.z;
		op->carang.y=(float)atan2(d.x,d.z);
		op->carang.x=op->carang.z=0;
		rpy2quat(&op->carang,&op->carang);
	} else {
		op->carang.x=op->carang.y=op->carang.z=0;
		op->carang.w=1;
	} */
	op->carang=chkboxheader->rot;
	quatrot(&op->carang,&op->pos,&op->pos);
	op->pos.x+=chkboxheader->pos.x;
	op->pos.z+=chkboxheader->pos.z;
	op->pos.y=chkboxheader->pos.y;
	findresetpoint(&op->pos);
	op->boxchk=chkboxheader;
}

// resets the spacetime breadcrumbs for determining diff times and current place..
void boxai_reinit()
{
	U32 i,j,k;
	for (i=0;i<nchkboxes;i++) {
		chkboxes[i]->ftime=0;
		chkboxes[i]->npassed=0;
		for (j=0;j<OL_MAXCARSLOTS;j++)
			chkboxes[i]->caridchk[j]=0;
		if (chkboxes[i]->ntriggers) {
			for (j=0;j<chkboxes[i]->ntriggers;j++) {
				k=chkboxes[i]->triggerprocidx[j];
				if (triggerprocs[k].init)
					(*triggerprocs[k].init)(chkboxes[i]->triggers[j]);
			}
		}
		if (chkboxes[i]->bt)
			chkboxes[i]->bt->flags|=TF_DONTDRAWC;
		nlasttrees=0;
	}
}

int boxai_slowlead()
{
	int i,bi;
	struct chkbox *cb=0,*bestcb=NULL,*mycb=0;
	float bestnum=-1,bestdist=-1,dist;
	if (od.numcars<2)
		return 0;
	for (i=0;i<od.numcars;i++) {
		cb=opa[i].boxchk;
		if (!cb)
			return 0;
		while (!cb->istimer && cb->nnext)
			cb=cb->next[0];
		if (op->carid==i)
			mycb=cb;
		if (bestcb==NULL || cb->num>bestnum) {
			bestcb=cb;
			bestnum=cb->num;
		}
	}
	if (mycb!=bestcb)
		return 0;
	bi=-1;
	for (i=0;i<od.numcars;i++) {
		if (opa[i].boxchk==bestcb) {
			dist=dist3dsq(&opa[i].pos,&cb->pos);
			if (bi==-1 || dist<bestdist) {
				bi=i;
				bestdist=dist;
			}
		}
	}
	if (bi==op->carid)
		return 1;
	return 0;
}
