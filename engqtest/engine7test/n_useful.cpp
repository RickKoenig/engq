#include <stdlib.h>
#include <string.h>

//#include <engine7cpp.h>
//#include "videoicpp.h"
#define D2_3D
#include <m_eng.h>

#include "n_usefulcpp.h"

char *cntl_names[MAX_CNTL]={"none","human","ai"};
char *energy_names[MAX_ENERGIES]={"fusion","electrical","magnetic","plasma","antimatter","chemical"};
char *wheel_names[MAX_WHEELS]={"tirefl","tirefr","tirebl","tirebr"};

char *rule_names[MAX_RULES]={"none","noweap","combat","CTF no weapons","CTF combat","stunt","noweap boxes","combat boxes"};
char *which_game_names[MAX_GAMENET]={"bailed","server","client","botgame"};
char *regpointnames[REGPOINT_NREGPOINTS]={
	"back",
	"tirebr",
	"bottom",
	"tirebl",
	"tirefr",
	"tirefl",
	"front",
	"hood",
	"right",
	"roof",
	"left",
	"trunk",
};

struct weapinfo weapinfos[MAX_WEAPKIND]={
	{"laser",NULL,0},
	{"klondyke",NULL,0},
	{"missiles",NULL,0},
	{"oil",NULL,0},
	{"emb",NULL,0},
	{"ecto",NULL,0},
	{"dielectric",NULL,0,2},
	{"transducer",NULL,0,1},
	{"plasma",NULL,0,1000000},
	{"ramjet",NULL,0},
	{"intake",NULL,0},
	{"spikes",NULL,0},
	{"bainite",NULL,0},
	{"aerodyne",NULL,0},
	{"ferrite",NULL,0},
	{"weapboost",NULL,1},
	{"rpd",NULL,1},
	{"nano",NULL,1},
	{"randomizer",NULL,1},
	{"icrms",NULL,0},
	{"bigtires",NULL,0},
	{"buzz",NULL,0},
	{"enleech",NULL,0},
	{"fsncan",NULL,0},
	{"grapple",NULL,0},
	{"magnet",NULL,0},
	{"mines",NULL,0},
	{"pouncer",NULL,0},
	{"prism",NULL,0,4},
	{"sonic",NULL,0},
	{"sledge","bashing mallet",0},
	{"energywall","static-ion barrier wall",0},
	{"stasis",NULL,0},
	{"supajump",NULL,0},
	{"forklift",NULL,1},
	{"bumpercar","repulsion barrier",1},
	{"gyroscope","precise landing device",1},
	{"gravitybomb",NULL,0},
	{"flasher","photonic retinal attack",0},
	{"charger",NULL,0},
	{"switcharoo",NULL,0},
	{"minigun",NULL,0},
	{"particle",NULL,0},
};

tree2 *findtreestrstrrec(tree2 *t,const C8 *str)
{
	tree2 *c;
	if (strstr(t->name.c_str(),str))
		return t;
//	S32 n=t->children.size();
//	for (i=0;i<n;i++)
//		if (c=findtreestrstrrec(t->children[i],str))
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it) {
//		(*it)->log2();
		if (c=findtreestrstrrec(*it,str))
			return c;
	}
	return 0;
}

void hideregpoints(tree2 *t)
{
	tree2 *c=findtreestrstrrec(t,"regpoints");
	if (c)
		c->flags|=TF_DONTDRAWC;
}

void findcarwheels(tree2 *t,tree2 **wheels)
{
	int i;
	for (i=0;i<MAX_WHEELS;i++) {
		wheels[i]=findtreestrstrrec(t,wheel_names[i]);
	}
}

void loadenergymatrix()
{
	int i=0,j,k;
	int cw=-1;
//	char **sc;
//	int nsc;
pushandsetdirdown("carenalobby");
//	sc=loadscript("energymatrix.txt",&nsc);
	script sc("energymatrix.txt");
popdir();
//	while(i<nsc) {
	while(i<sc.num()) {
		for (j=0;j<MAX_WEAPKIND;j++)
//			if (!strcmp(weapinfos[j].name,sc.idx(i).c_str()))
			if (!strcmp(weapinfos[j].name,sc.idx(i).c_str()))
				break;
		if (j!=MAX_WEAPKIND) {
			i++;
			cw=j;
			continue;
		}
		if (cw==-1)
			errorexit("loadenergymatrix: bad weapon '%s'",sc.idx(i).c_str());
		for (k=0;k<MAX_ENERGIES;k++)
			if (!strcmp(energy_names[k],sc.idx(i).c_str()))
				break;
		if (k!=MAX_ENERGIES) {
			i++;
//			if (i>=nsc)
			if (i>=sc.num())
				errorexit("loadenergymatrix: missing energy amount");
			weapinfos[cw].energyusage[k]=atoi(sc.idx(i).c_str());
			++i;
			continue;
		}
		errorexit("loadenergymatrix: bad energy '%s'",sc.idx(i).c_str());
	}
//	freescript(sc,nsc);
}

/*
void setnextmaterial(char *matname,char *texname)
{
//	if (video_maindriver==VIDEO_D3D)
//		buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_HASSHADE|SMAT_CALCLIGHTS,
//			texname,0,matname,0);
		buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV|SMAT_HASSHADE|SMAT_CALCLIGHTS,
			texname,0,matname,0);
//	else
//		buildmodelsetmaterial(SMAT_HASTEX|SMAT_HASWBUFF|SMAT_WRAPU|SMAT_WRAPV,
//			texname,0,matname,0);
}
SMAT_CLAMP
*/
// no clipping yet
void drawlines(viewport2 *vp,modelb *m,C32 c)
{
#if 0
	mat4 w2c;
	pointf3 *tv;
	U32 i,j,k;
	int x0,y0,x1,y1;
	pointf3 *v;
//	face *f;
	vector<face>::iterator f;
	v=m->verts.begin();
	f=m->faces.begin();
	tv=(pointf3 *)memalloc(sizeof(pointf3)*m->nvert);
//	identmat4(&w2c);
	buildrottrans3d(&vp->camrot,&vp->camtrans,&w2c);
//	inversemat4(&w2c,&w2c);
	inversemat3d(&w2c,&w2c);
	registerproj((float)vp->xstart,(float)vp->ystart,(float)vp->xres,(float)vp->yres,0);

	for (k=0;k<m->nvert;k++) {
		xformvec(&w2c,&v[k],&tv[k]);
		tv[k].x*=vp->camzoom*vp->ysrc/vp->xsrc;
		tv[k].y*=vp->camzoom;
		projvec(&tv[k],&tv[k]);
	}
	for (i=0;i<m->nface;i++,f++) {
		for (j=0;j<3;j++) {
			k=(j+1)%3;
			x0=int(tv[f->vertidx[j]].x);
			y0=int(tv[f->vertidx[j]].y);
			x1=int(tv[f->vertidx[k]].x);
			y1=int(tv[f->vertidx[k]].y);
			clipline16(B16,x0,y0,x1,y1,c);
		}
	}
	memfree(tv);
#endif
}

/*
void drawpoint(viewport2 *vp,pointf3 *p,int c)
{
	mat4 w2c;
//	pointf3 *tv;
	int x0,y0;
	pointf3 v;
//	face *f;
//	identmat4(&w2c);
	buildrottrans3d(&vp->camrot,&vp->camtrans,&w2c);
//	inversemat4(&w2c,&w2c);
	inversemat3d(&w2c,&w2c);
	registerproj((float)vp->xstart,(float)vp->ystart,(float)vp->xres,(float)vp->yres,0);

	xformvec(&w2c,p,&v);
	v.x*=vp->camzoom*vp->ysrc/vp->xsrc;
	v.y*=vp->camzoom;
	projvec(&v,&v);
	x0=int(v.x);
	y0=int(v.y);
	clipcircle32(B32,x0,y0,4,c);
}
*/
// ignores y input, set y intersection between p0.y and p1.y
int intersectline2d(const pointf3 *p0,const pointf3 *p1,const pointf3 *q0,const pointf3 *q1,pointf3 *i,float *t,float *u)
{
	float a,b,c,d,e,f,det,lt,lu,cot,cou;
	a=p1->x-p0->x;
	b=q0->x-q1->x;
	d=p1->z-p0->z;
	e=q0->z-q1->z;
	det=a*e-b*d;
	c=q0->x-p0->x;
	f=q0->z-p0->z;
	cot=c*e-b*f;
	cou=a*f-c*d;
	if (det==0) { //if (det<EPSILON && det>-EPSILON)
		if (cot!=0 || cou!=0)
			return 0;
		return -1; // lines are colinear
	}
	det=1.0f/det;
	lt=cot*det;
	lu=cou*det;
	if (lt<0 || lt>1 || lu<0 || lu>1) // allow t's and u's to be 0 and 1 INCLUSIVE
		return 0;
	if (i) {
		i->x=p0->x+lt*(p1->x-p0->x);
		i->y=0;//p0->y+lt*(p1->y-p0->y);
		i->z=p0->z+lt*(p1->z-p0->z);
	}
	if (t)
		*t=lt;
	if (u)
		*u=lu;
	return 1;
}
