/*#include <string.h>
#include <stdio.h>
#include <math.h>

#include <engine1.h>

#include "stubhelper.h"
#include "line2road.h"
#include "online_uplay.h"
#include "globalres.h"
*/
#define D2_3D
#include <m_eng.h>
#include "../engine7test/n_usefulcpp.h"
#include "stubhelper.h"
#include "../engine7test/n_line2roadcpp.h"
#include "online_uplay.h"

#define MAXPATH 5
struct chkbox {
	struct chkbox *next[MAXPATH];
	struct chkbox *prev[MAXPATH];
	int nnext;
	int nprev;
	unsigned char isreset,pathnum,isloopcam,isfast,isslowreset,ishalfspeed,isbotpath,ismerge;
	float num;
//	int inum;
	pointf3 pos,rot;
	pointf3 bmin,bmax;
	tree2* t;
};

static struct chkbox *chkboxheader;
//static struct chkbox **chkboxes;
//static int nchkboxes;
vector<chkbox> chkboxes;

char *scantestnames[]={
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

static float pathnums[MAXPATH];
static void scanforboxes(tree2* t)
{
	struct chkbox *c;
	char name[100];
	char suffix[10];
	char numstr[10];
	int i;
	strcpy(name,t->name.c_str());
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
//			if ((nchkboxes&7)==0)
//				chkboxes=(struct chkbox **)memrealloc(chkboxes,sizeof(struct chkbox *)*(nchkboxes+8));
//			c=chkboxes[nchkboxes]=memzalloc(sizeof(struct chkbox));
			chkbox ca;
			memset(&ca,0,sizeof(ca));
			c=&ca;
			c->num=(float)atof(numstr);
			logger("   box found, name '%s', num %f, suffix %s\n",t->name.c_str(),c->num,suffix);
			c->t=t;
			if (!t->mod)
				errorexit("'%s' not a box (1)",t->name.c_str());
			c->bmin=t->mod->boxmin;
			c->bmax=t->mod->boxmax;
			c->pos=t->trans;
			c->rot=t->rot;
			i=0;
			while(suffix[i]) {
				if (suffix[i]>='a' && suffix[i]<'a'+MAXPATH-1)
					c->pathnum=suffix[i]-'a'+1;
				switch(suffix[i]) {
				case 'r':
					c->isreset=1;
					break;
				case 'f':
					c->isfast=1;
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
//			nchkboxes++;
			chkboxes.push_back(ca);
		}
	}
	list<tree2*>::iterator it;
//	for (i=0;i<t->nchildren;i++)
//		scanforboxes(t->children[i]);
	for (it=t->children.begin();it!=t->children.end();++it)
		scanforboxes(*it);
}

static void scanforboxestest()
{
	struct chkbox *c;
	char name[100];
	char suffix[10];
	char numstr[10];
	U32 i,j;
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
//				if ((nchkboxes&7)==0)
//					chkboxes=(struct chkbox **)memrealloc(chkboxes,sizeof(struct chkbox *)*(nchkboxes+8));
//				c=chkboxes[nchkboxes]=memzalloc(sizeof(struct chkbox));
				chkbox ca;
				memset(&ca,0,sizeof(ca));
				c=&ca;
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
						c->isfast=1;
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
//				nchkboxes++;
				chkboxes.push_back(ca);
			}
		}
	}
}

struct chkbox *findprev(float time)
{
	int i;
	struct chkbox *cur=NULL;
	float curlow=-1e20f;
	S32 nchkboxes=chkboxes.size();
	for (i=0;i<nchkboxes;i++) {
		if (chkboxes[i].num>curlow && chkboxes[i].num<time && chkboxes[i].pathnum==0) {
			cur=&chkboxes[i];
			curlow=cur->num;
		}
	}
	return cur;
}

struct chkbox *findnext(float time)
{
	int i;
	struct chkbox *cur=NULL;
	float curlow=1e20f;
	S32 nchkboxes=chkboxes.size();
	for (i=0;i<nchkboxes;i++) {
		if (chkboxes[i].num<curlow && chkboxes[i].num>time && chkboxes[i].pathnum==0) {
			cur=&chkboxes[i];
			curlow=cur->num;
		}
	}
	return cur;
}

static void sortboxes()
{
	int i,j;
	float lowestnum=-1e20f,curlow;
	struct chkbox *last=NULL,*cur;
	chkboxheader=NULL;
// first build path 0
	while(1) {
		curlow=1e20f;
		cur=NULL;
		S32 nchkboxes=chkboxes.size();
		for (i=0;i<nchkboxes;i++) {
			if (chkboxes[i].num<curlow && chkboxes[i].num>lowestnum && chkboxes[i].pathnum==0) {
				cur=&chkboxes[i];
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
			S32 nchkboxes=chkboxes.size();
			for (i=0;i<nchkboxes;i++) {
				if (chkboxes[i].num<curlow && chkboxes[i].num>lowestnum && chkboxes[i].pathnum==j) {
					cur=&chkboxes[i];
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
// print out paths
	cur=chkboxheader;
	S32 nchkboxes=chkboxes.size();
	for (j=0;j<nchkboxes;j++) {
		cur=&chkboxes[j];
		logger("cur %f(%d), ",cur->num,cur->pathnum);
		for (i=0;i<cur->nnext;i++)
			logger("next[%d] %f(%d), ",i,cur->next[i]->num,cur->next[i]->pathnum);
		for (i=0;i<cur->nprev;i++)
			logger("prev[%d] %f(%d), ",i,cur->prev[i]->num,cur->prev[i]->pathnum);
		logger("\n");
	}
}

void boxai_init()
{
//	nchkboxes=0;
	chkboxes.clear();
	logger("scan for boxes...\n");
	scanforboxes(od.ol_root);
//	scanforboxestest();
	sortboxes();
}

// one car at a time, drive ai cars
void boxai_ai()
{
	op->pi.ol_uup=1;
	op->pi.ol_uright=1;
}

// checkpoint_reset system
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
}

void boxai_exit()
{
//	int i;
//	for (i=0;i<nchkboxes;i++)
//		memfree(chkboxes[i]);
//	if (chkboxes)
//		memfree(chkboxes);
//	chkboxes=NULL;
	chkboxes.clear();
}

#if 0
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
#endif
