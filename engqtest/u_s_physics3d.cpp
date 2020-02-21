#define D2_3D
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

#include "primcoll.h"
#include "collutil.h"
#include "objcollisions.h"
#include "u_s_physics3d.h"

namespace physics3d {
static int changevid;
static int vi=0;
static int vtm[2]={2,0};
static int vts[2]={1,0};

static string ascene;
static int nextvert[3]={1,2,0};
static U32 togvidmode;

static S32 framenum;
// trees
static tree2 *roottree,*footballfield,*cam;

enum {OBJ_BOX,OBJ_CYL,OBJ_SPH};
#define MAXPHYOBJECTS MAX3DBOX // 100
static struct phyobject phyobjects[MAXPHYOBJECTS];
VEC totangmomentum,totangcmmomentum,totangorgmomentum;
VEC totmomentum;
float tottransenergy,totrotenergy,totpotenergy,totenergy;
float littlegee=10.0f; //9.8f;

// for debprint
static int nphyobjects;
int curphyobjectnum;
static int lastcurphyobjectnum;
struct phyobject curphyobject;
#define MAXWORLDOBJECTS 10
static VEC moisqs[MAXWORLDOBJECTS]={
	{1.0f/12.0f,1.0f/12.0f,1.0f/12.0f},
	{1.0f/16.0f,1.0f/12.0f,1.0f/16.0f},
	{1.0f/20.0f,1.0f/20.0f,1.0f/20.0f}
};

static int numworldobjects;
static string objkindstr[MAXWORLDOBJECTS];
static script* worldobjectsscript;
static tree2 *worldobjectsscene;

float timestep=1.0f/30.0f;	// 1/30
int iterations=10;			// 10
VEC result0,result1;

int midpointmethod = 1; // 1
int oldgravity = 1; // 1 means avoiding resting contact problems

float timestep2=0;
int bisect;
float globalelast=1;
float sep;

struct menuvar edv[] = {
	{"@lightred@----- new test  physics --------------",NULL,D_VOID,0},
	{"littlegee",&littlegee,D_FLOAT,FLOATUP},
	{"oldgravity",&oldgravity,D_INT,1},
	{"iterations",&iterations,D_INT,1},
	{"bisect",&bisect,D_INT,1},
	{"globalelast",&globalelast,D_FLOAT,FLOATUP/16},
	{"midpointmethod",&midpointmethod,D_INT,1},
	{"ncolpairs",&ncolpairs,D_INT|D_RDONLY},
	{"testsvec.x",&testsvec.x,D_FLOAT,FLOATUP/16},
	{"testsvec.y",&testsvec.y,D_FLOAT,FLOATUP/16},
	{"testsvec.z",&testsvec.z,D_FLOAT,FLOATUP/16},
	{"testiter",&testiter,D_INT,1},

	{"sep",&sep,D_FLOAT|D_RDONLY},
	{"timestep",&timestep,D_FLOAT,FLOATUP/64},
	{"timestep2",&timestep2,D_FLOAT,FLOATUP/4},
	{"T O T A L S",NULL,D_VOID,0},
	{"   total energy",&totenergy,D_FLOAT|D_RDONLY},
	{"   total trans energy",&tottransenergy,D_FLOAT|D_RDONLY},
	{"   total rot energy",&totrotenergy,D_FLOAT|D_RDONLY},
	{"   total pot energy",&totpotenergy,D_FLOAT|D_RDONLY},
	{"   total momentum.x",&totmomentum.x,D_FLOAT|D_RDONLY},
	{"   total momentum.y",&totmomentum.y,D_FLOAT|D_RDONLY},
	{"   total momentum.z",&totmomentum.z,D_FLOAT|D_RDONLY},
	{"   total ang cm momentum.x",&totangcmmomentum.x,D_FLOAT|D_RDONLY},
	{"   total ang cm momentum.y",&totangcmmomentum.y,D_FLOAT|D_RDONLY},
	{"   total ang cm momentum.z",&totangcmmomentum.z,D_FLOAT|D_RDONLY},
	{"   total ang org momentum.x",&totangorgmomentum.x,D_FLOAT|D_RDONLY},
	{"   total ang org momentum.y",&totangorgmomentum.y,D_FLOAT|D_RDONLY},
	{"   total ang org momentum.z",&totangorgmomentum.z,D_FLOAT|D_RDONLY},
	{"   total ang momentum.x",&totangmomentum.x,D_FLOAT|D_RDONLY},
	{"   total ang momentum.y",&totangmomentum.y,D_FLOAT|D_RDONLY},
	{"   total ang momentum.z",&totangmomentum.z,D_FLOAT|D_RDONLY},
	{"G E O M E T R Y",NULL,D_VOID,0},
	{"curphyobjectnum",&curphyobjectnum,D_INT,1},
	{"   kind",&curphyobject.kind,D_RDONLY,1},
	{"   scale.x",&curphyobject.scale.x,D_FLOAT|D_RDONLY},
	{"   scale.y",&curphyobject.scale.y,D_FLOAT|D_RDONLY},
	{"   scale.z",&curphyobject.scale.z,D_FLOAT|D_RDONLY},
	{"P O S",NULL,D_VOID,0},
	{"   pos.x",&curphyobject.s0.pos.x,D_FLOAT,FLOATUP/4},
	{"   pos.y",&curphyobject.s0.pos.y,D_FLOAT,FLOATUP/4},
	{"   pos.z",&curphyobject.s0.pos.z,D_FLOAT,FLOATUP/4},
	{"   posst.x",&curphyobject.st.pos.x,D_FLOAT|D_RDONLY},
	{"   posst.y",&curphyobject.st.pos.y,D_FLOAT|D_RDONLY},
	{"   posst.z",&curphyobject.st.pos.z,D_FLOAT|D_RDONLY},
	{"R O T",NULL,D_VOID,0},
	{"   qrot.x",&curphyobject.s0.rot.x,D_FLOAT,FLOATUP/16},
	{"   qrot.y",&curphyobject.s0.rot.y,D_FLOAT,FLOATUP/16},
	{"   qrot.z",&curphyobject.s0.rot.z,D_FLOAT,FLOATUP/16},
	{"   qrot.w",&curphyobject.s0.rot.w,D_FLOAT,FLOATUP/16},
	{"   qrotst.x",&curphyobject.st.rot.x,D_FLOAT|D_RDONLY},
	{"   qrotst.y",&curphyobject.st.rot.y,D_FLOAT|D_RDONLY},
	{"   qrotst.z",&curphyobject.st.rot.z,D_FLOAT|D_RDONLY},
	{"   qrotst.w",&curphyobject.st.rot.w,D_FLOAT|D_RDONLY},
	{"V E L",NULL,D_VOID,0},
	{"   vel.x",&curphyobject.s0.vel.x,D_FLOAT|D_RDONLY},
	{"   vel.y",&curphyobject.s0.vel.y,D_FLOAT|D_RDONLY},
	{"   vel.z",&curphyobject.s0.vel.z,D_FLOAT|D_RDONLY},
	{"   velst.x",&curphyobject.st.vel.x,D_FLOAT|D_RDONLY},
	{"   velst.y",&curphyobject.st.vel.y,D_FLOAT|D_RDONLY},
	{"   velst.z",&curphyobject.st.vel.z,D_FLOAT|D_RDONLY},
	{"R O T V E L",NULL,D_VOID,0},
	{"   rotvel.x",&curphyobject.s0.rotvel.x,D_FLOAT|D_RDONLY},
	{"   rotvel.y",&curphyobject.s0.rotvel.y,D_FLOAT|D_RDONLY},
	{"   rotvel.z",&curphyobject.s0.rotvel.z,D_FLOAT|D_RDONLY},
	{"   rotvelst.x",&curphyobject.st.rotvel.x,D_FLOAT|D_RDONLY},
	{"   rotvelst.y",&curphyobject.st.rotvel.y,D_FLOAT|D_RDONLY},
	{"   rotvelst.z",&curphyobject.st.rotvel.z,D_FLOAT|D_RDONLY},
	{"M O M E N T U M",NULL,D_VOID,0},
	{"   momentum.x",&curphyobject.s0.momentum.x,D_FLOAT,D_FLOAT*4},
	{"   momentum.y",&curphyobject.s0.momentum.y,D_FLOAT,D_FLOAT*4},
	{"   momentum.z",&curphyobject.s0.momentum.z,D_FLOAT,D_FLOAT*4},
	{"   momentumst.x",&curphyobject.st.momentum.x,D_FLOAT|D_RDONLY},
	{"   momentumst.y",&curphyobject.st.momentum.y,D_FLOAT|D_RDONLY},
	{"   momentumst.z",&curphyobject.st.momentum.z,D_FLOAT|D_RDONLY},
	{"A N G M O M E N T U M",NULL,D_VOID,0},
	{"   angmom.x",&curphyobject.s0.angmomentum.x,D_FLOAT,FLOATUP*4},
	{"   angmom.y",&curphyobject.s0.angmomentum.y,D_FLOAT,FLOATUP*4},
	{"   angmom.z",&curphyobject.s0.angmomentum.z,D_FLOAT,FLOATUP*4},
	{"   angmomst.x",&curphyobject.st.angmomentum.x,D_FLOAT|D_RDONLY},
	{"   angmomst.y",&curphyobject.st.angmomentum.y,D_FLOAT|D_RDONLY},
	{"   angmomst.z",&curphyobject.st.angmomentum.z,D_FLOAT|D_RDONLY},
	{"E N E R G Y",NULL,D_VOID,0},
	{"   transenergy",&curphyobject.transenergy,D_FLOAT|D_RDONLY},
	{"   rotenergy",&curphyobject.rotenergy,D_FLOAT|D_RDONLY},
	{"   potenergy",&curphyobject.potenergy,D_FLOAT|D_RDONLY},
	{"P A R A M E T E R S",NULL,D_VOID,0},
	{"   mass",&curphyobject.mass,D_FLOAT|D_RDONLY},
	{"   elasticity",&curphyobject.elast,D_FLOAT,FLOATUP/16},
	{"   friction",&curphyobject.frict,D_FLOAT,FLOATUP/16},
};
const int nedv = NUMELEMENTS(edv);

static void fixupmodelcm(modelb *mod,VEC *moisqs)
{
	int i,j,f;
#define SCANY 2
#define SCANZ 2
	VEC amin[SCANZ][SCANY];
	VEC amax[SCANZ][SCANY];
	const VEC *obmin,*obmax;
	VEC bmin,bmax;
	VEC *verts,*v0,*v1,*v2;
	const FACE *faces;
	int nfaces,nverts;
	int nintsect;
	VEC cm,rm;
	float m,sm;
	obmin=&mod->boxmin;
	obmax=&mod->boxmax;
	verts=mod->verts;
	nverts=mod->verts.size();
	faces = &*mod->faces.begin();
	//faces = mod->faces;
	nfaces=mod->faces.size();
	rm=zerov;
	sm=0;
	for (j=0;j<SCANZ;j++)
		for (i=0;i<SCANY;i++) {
			nintsect=0;
			amin[j][i].x=obmin->x*2-obmax->x;
			amin[j][i].y=obmin->y+(obmax->y-obmin->y)*(1+2*i)*(.5f/SCANY);
			amin[j][i].z=obmin->z+(obmax->z-obmin->z)*(1+2*j)*(.5f/SCANZ);
			amax[j][i].x=obmax->x*2-obmin->x;
			amax[j][i].y=amin[j][i].y;
			amax[j][i].z=amin[j][i].z;
			for (f=0;f<nfaces;f++) {
				v0=&verts[faces[f].vertidx[0]];
				v1=&verts[faces[f].vertidx[1]];
				v2=&verts[faces[f].vertidx[2]];
				if (line2btricull(v0,v1,v2,&amin[j][i],&amax[j][i],&bmin)) {
					nintsect++;
					break;
				}
			}
			for (f=0;f<nfaces;f++) {
				v0=&verts[faces[f].vertidx[0]];
				v1=&verts[faces[f].vertidx[1]];
				v2=&verts[faces[f].vertidx[2]];
				if (line2btricull(v0,v1,v2,&amax[j][i],&amin[j][i],&bmax)) {
					nintsect++;
					break;
				}
			}
			if (nintsect==2) {
				amax[j][i]=bmax;
				amin[j][i]=bmin;
				m=bmax.x-bmin.x;
				rm.x+=(bmax.x+bmin.x)*.5f*m;
				rm.y+=bmin.y*m;
				rm.z+=bmin.z*m;
				sm+=m;
//					addline(&tmin,&tmax,rgbwhite); */
			} else {
				amax[j][i]=amin[j][i]=zerov;
			}
		}
	cm.x=rm.x/sm;
	cm.y=rm.y/sm;
	cm.z=rm.z/sm;
	logger("'%s' cm %f %f %f\n",mod->name.c_str(),cm.x,cm.y,cm.z);
	for (j=0;j<SCANZ;j++)
		for (i=0;i<SCANY;i++) {
			amin[j][i].x-=cm.x;	
			amin[j][i].y-=cm.y;	
			amin[j][i].z-=cm.z;	
			amax[j][i].x-=cm.x;	
			amax[j][i].y-=cm.y;	
			amax[j][i].z-=cm.z;	
		}
	*moisqs=zerov;
	for (j=0;j<SCANZ;j++)
		for (i=0;i<SCANY;i++) {
			bmax=amax[j][i];
			bmin=amin[j][i];
			m=bmax.x-bmin.x;
			moisqs->x+=(bmax.x*bmax.x+bmin.x*bmax.x+bmin.x*bmin.x)*(1.0f/3.0f)*m;
			moisqs->y+=bmin.y*bmin.y*m;
			moisqs->z+=bmin.z*bmin.z*m;
		}
	moisqs->x/=sm;
	moisqs->y/=sm;
	moisqs->z/=sm;
	logger("'%s' moisqs %f %f %f\n",mod->name.c_str(),moisqs->x,moisqs->y,moisqs->z);
	for (j=0;j<nverts;j++) {
		verts[j].x-=cm.x;
		verts[j].y-=cm.y;
		verts[j].z-=cm.z;
	}
	mod->update();
	mod->setbbox(); // important, update bounding box
}


// templates ////////////////////////////////////////////////////////////
static void initworldobjects(char *worldobjsname)
{
	int i;
	char fullname[256];
	TREE *mt;
//	worldobjectsscript=loadscript(worldobjsname,&numworldobjects);
	worldobjectsscript = new script(worldobjsname);
	numworldobjects = worldobjectsscript->num();
	worldobjectsscene = new tree2("worldObjectScene");
	if (numworldobjects>=MAXWORLDOBJECTS)
		errorexit("too many world objects");
	for (i=0;i<numworldobjects;i++) {
		objkindstr[i]=worldobjectsscript->idx(i);
		sprintf(fullname,"%s.BWS",worldobjectsscript->idx(i).c_str());
		worldobjectsscene->linkchild(new tree2(fullname));
		if (i>=FIRSTMESHOBJ) {
			sprintf(fullname,"%s.bwo",worldobjectsscript->idx(i).c_str());
			mt = worldobjectsscene->find(fullname);
			if (!mt)
				errorexit("can't find '%s'",fullname);
			fixupmodelcm(mt->mod,&moisqs[i]);
		}
	}
}

static void freeworldobjects()
{
	delete worldobjectsscene;
	delete worldobjectsscript;
}

// angvel to angmom
static void domoi(struct phyobject *p,VEC *rot,VEC *angvelin,VEC *angmomout)
{
	VEC quat;
	if (p->norot)
		return;
	quatinverse(rot,&quat);
	quatrot(&quat,angvelin,angmomout);
	angmomout->x*=p->moivec.x;
	angmomout->y*=p->moivec.y;
	angmomout->z*=p->moivec.z;
	quatrot(rot,angmomout,angmomout);
}

// angmom to angvel  
static void doinvmoi(struct phyobject *p,VEC *rot,VEC *angmomin,VEC *angvelout)
{
	VEC quat;
	if (p->norot)
		return;
	quatinverse(rot,&quat);
	quatrot(&quat,angmomin,angvelout);
	angvelout->x/=p->moivec.x;
	angvelout->y/=p->moivec.y;
	angvelout->z/=p->moivec.z;
	quatrot(rot,angvelout,angvelout);
}

// always at st, sets p->rotenergy
static void calcangenergy(struct phyobject *p)
{
	VEC quat;
	VEC proprotvel;
	if (p->norot)
		return;
	quatinverse(&p->st.rot,&quat);
	quatrot(&quat,&p->st.rotvel,&proprotvel);
	p->rotenergy=.5f*(
		p->moivec.x*proprotvel.x*proprotvel.x+
		p->moivec.y*proprotvel.y*proprotvel.y+
		p->moivec.z*proprotvel.z*proprotvel.z);
}

// instances //////////////////////////////////////////////////
static void initphysicsobjects(const char *name)
{
	int i,j,k,m,n;
	//char **sc;
	char fullname[256];
	TREE *obj=NULL;
	int tp=0;
	struct phyobject *po;
	nphyobjects=-1;
	//sc=loadscript(name,&nsc);
	script sc(name);
	int nsc = sc.num();
// read script
	while(tp<nsc) {
		if (sc.idx(tp) == "object") {
			nphyobjects++;
			if (nphyobjects>=MAXPHYOBJECTS)
				errorexit("too many objects");
			po=&phyobjects[nphyobjects];
			memset(po,0,sizeof(struct phyobject));
			po->mass=1;
			po->s0.rot.w=1;
			po->elast=1;
			po->scale.x=1;
			po->scale.y=1;
			po->scale.z=1;
			tp++;
			if (tp+1>nsc)
				errorexit("end o file in 'name'",name);
			sprintf(fullname,"%s.bwo",sc.idx(tp).c_str());
			obj=new tree2(fullname);
//			obj->dissolve=.5f;
			obj->buildo2p=O2P_FROMTRANSQUATSCALE;
			//linkchildtoparent(obj,roottree);
			roottree->linkchild(obj);
			po->t=obj;
			for (i=0;i<numworldobjects;i++)
				if (sc.idx(tp) == objkindstr[i]) {
					po->kind=i;
					break;
				}
			if (i==numworldobjects)
				errorexit("unknown object '%s'",sc.idx(tp).c_str());
			tp++;
		} else if (sc.idx(tp) == "pos") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+3>nsc)
				errorexit("end o file in 'name'",name);
			po->s0.pos.x=(float)atof(sc.idx(tp++).c_str());
			po->s0.pos.y=(float)atof(sc.idx(tp++).c_str());
			po->s0.pos.z=(float)atof(sc.idx(tp++).c_str());
		} else if (sc.idx(tp) == "littlegee") {
			tp++;
			if (tp+1>nsc)
				errorexit("end o file in 'name'",name);
			littlegee=(float)atof(sc.idx(tp++).c_str());
		} else if (sc.idx(tp) == "elast") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+1>nsc)
				errorexit("end o file in 'name'",name);
			po->elast=(float)atof(sc.idx(tp++).c_str());
		} else if (sc.idx(tp) == "norot") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp>nsc)
				errorexit("end o file in 'name'",name);
			po->norot=1;
		} else if (sc.idx(tp) == "notrans") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp>nsc)
				errorexit("end o file in 'name'",name);
			po->notrans=1;
		} else if (sc.idx(tp) == "mass") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+1>nsc)
				errorexit("end o file in 'name'",name);
			po->mass=(float)atof(sc.idx(tp++).c_str());
		} else if (sc.idx(tp) == "rot") {
			float len;
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+3>nsc)
				errorexit("end o file in 'name'",name);
			po->s0.rot.x=(float)atof(sc.idx(tp++).c_str());
			po->s0.rot.y=(float)atof(sc.idx(tp++).c_str());
			po->s0.rot.z=(float)atof(sc.idx(tp++).c_str());
			len=normalize3d(&po->s0.rot,&po->s0.rot);
			if (len) {
				po->s0.rot.w=len*PIOVER180;
				rotaxis2quat(&po->s0.rot,&po->s0.rot);
			} else {
				po->s0.rot=zerov;
				po->s0.rot.w=1;
			}
		} else if (sc.idx(tp) == "rotvel") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+3>nsc)
				errorexit("end o file in 'name'",name);
			po->s0.rotvel.x=PIOVER180*(float)atof(sc.idx(tp++).c_str());
			po->s0.rotvel.y=PIOVER180*(float)atof(sc.idx(tp++).c_str());
			po->s0.rotvel.z=PIOVER180*(float)atof(sc.idx(tp++).c_str());
		} else if (sc.idx(tp) == "vel") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+3>nsc)
				errorexit("end o file in 'name'",name);
			po->s0.vel.x=(float)atof(sc.idx(tp++).c_str());
			po->s0.vel.y=(float)atof(sc.idx(tp++).c_str());
			po->s0.vel.z=(float)atof(sc.idx(tp++).c_str());
		} else if (sc.idx(tp) == "scale") {
			if (!obj)
				errorexit("use 'object' first");
			tp++;
			if (tp+3>nsc)
				errorexit("end o file in 'name'",name);
			po->scale.x=(float)atof(sc.idx(tp++).c_str());
			po->scale.y=(float)atof(sc.idx(tp++).c_str());
			po->scale.z=(float)atof(sc.idx(tp++).c_str());
		} else 
			errorexit("unknown obj script keyword '%s'",sc.idx(tp).c_str());
	}
	nphyobjects++;
	//freescript(sc,nsc);
	lastcurphyobjectnum=-1;
	curphyobjectnum=0;
// prepare objects
	for (i=0;i<nphyobjects;i++) {
		FACE *f;
		VEC *v;
		int nf,nv,ne;
		VEC s;
// build 8 point bbox
		VEC *bmin;
		VEC *bmax;
		po=&phyobjects[i];
		bmin=&po->t->mod->boxmin;
		bmax=&po->t->mod->boxmax;
		for (j=0;j<NCORNERS;j++) {
			if (j&1) 
				po->pnts[j].x=bmax->x;
			else
				po->pnts[j].x=bmin->x;
			if (j&2)
				po->pnts[j].y=bmax->y;
			else
				po->pnts[j].y=bmin->y;
			if (j&4)
				po->pnts[j].z=bmax->z;
			else
				po->pnts[j].z=bmin->z;
		}
// calc moivec
		s=po->scale;
		if (po->mass) {
			s.x=moisqs[po->kind].x*s.x*s.x;
			s.y=moisqs[po->kind].y*s.y*s.y;
			s.z=moisqs[po->kind].z*s.z*s.z;
			po->moivec.x=po->mass*(s.y+s.z);
			po->moivec.y=po->mass*(s.x+s.z);
			po->moivec.z=po->mass*(s.x+s.y);
			logger("object '%s' moivec %f %f %f\n",po->t->name.c_str(),
				po->moivec.x,
				po->moivec.y,
				po->moivec.z);
// calc angmom
			domoi(po,&po->s0.rot,&po->s0.rotvel,&po->s0.angmomentum);
// calc mom
			po->s0.momentum.x=po->mass*po->s0.vel.x;
			po->s0.momentum.y=po->mass*po->s0.vel.y;
			po->s0.momentum.z=po->mass*po->s0.vel.z;
		} else {
			po->norot=po->notrans=1;
		}
		if (po->norot)
			po->moivec=zerov;
		if (po->notrans)
			po->mass=0;
// init collisions
		po->nwfaces=po->t->mod->faces.size();
		if (po->nwfaces>MAXF)
			errorexit("too many faces %d/%d '%s'",po->nwfaces,MAXF,po->t->name.c_str());
		po->lfaces=(FACE *)memalloc(sizeof(FACE)*po->nwfaces);
		po->lpnts=(VEC *)memalloc(sizeof(VEC)*po->t->mod->verts.size());
		// remove duplicate verts, compact mesh
		po->nwpnts=0;
		nf=po->nwfaces;
		//f=po->t->mod->faces;
		f = &*po->t->mod->faces.begin();
		v=po->t->mod->verts;
		for (j=0;j<nf;j++) {
			for (m=0;m<3;m++) {
				n=f[j].vertidx[m];
				for (k=0;k<po->nwpnts;k++)  {
					if (isequal3d(&v[n],&po->lpnts[k]))
						break;
				}
				po->lfaces[j].vertidx[m]=k;
				if (k==po->nwpnts) {
					po->lpnts[po->nwpnts]=v[n];
					po->nwpnts++;
				}
			}
		} 
		if (po->nwpnts>MAXV)
			errorexit("too many verts %d/%d '%s'",po->nwpnts,MAXV,po->t->name.c_str());
		//po->contacts=(contact*)memzalloc(sizeof(struct contact)*nphyobjects);
		po->lpnts=(VEC *)memrealloc(po->lpnts,sizeof(VEC)*po->nwpnts);
		po->wpnts=(VEC *)memalloc(sizeof(VEC)*po->nwpnts);
// build neighbors
		f=po->lfaces;
		nf=po->nwfaces;
		nv=po->nwpnts;
		v=po->lpnts;
		po->nbs=(struct nb *)memzalloc(sizeof(struct nb)*nv);
		for (j=0;j<nf;j++) {
			for (k=0;k<3;k++) {
				int vs,ve;
				struct nb *lmb;
				vs=f[j].vertidx[k];
				ve=f[j].vertidx[nextvert[k]];
				lmb=&po->nbs[vs];
				for (m=0;m<lmb->nnb;m++)
					if (lmb->nbs[m]==ve)
						break;
				if (m==lmb->nnb) {
					if (lmb->nnb>=lmb->nnballoced) {
						lmb->nnballoced+=10;
						lmb->nbs=(int*)memrealloc(lmb->nbs,sizeof(int)*lmb->nnballoced);
					}
					lmb->nnb++;
					lmb->nbs[m]=ve;
				}
				lmb=&po->nbs[ve];
				for (m=0;m<lmb->nnb;m++)
					if (lmb->nbs[m]==vs)
						break;
				if (m==lmb->nnb) {
					if (lmb->nnb>=lmb->nnballoced) {
						lmb->nnballoced+=10;
						lmb->nbs=(int*)memrealloc(lmb->nbs,sizeof(int)*lmb->nnballoced);
					}
					lmb->nnb++;
					lmb->nbs[m]=vs;
				}
			}
		}
// build face neighbors
		f=po->lfaces;
		nf=po->nwfaces;
		nv=po->nwpnts;
		v=po->lpnts;
		po->nbfs=(struct nbf *)memzalloc(sizeof(struct nbf)*nv);
		for (j=0;j<nf;j++) {
			for (k=0;k<3;k++) {
				int vs;
				struct nbf *lmb;
				vs=f[j].vertidx[k];
				lmb=&po->nbfs[vs];
				if (lmb->nnbf>=lmb->nnbfalloced) {
					lmb->nnbfalloced+=10;
					lmb->nbfs=(int*)memrealloc(lmb->nbfs,sizeof(int)*lmb->nnbfalloced);
				}
				lmb->nbfs[lmb->nnbf]=j;
				lmb->nnbf++;
			}
		}
// show faces and neighbors
		for (j=0;j<nf;j++) {
			int v0,v1,v2;
			v0=f[j].vertidx[0];
			v1=f[j].vertidx[1];
			v2=f[j].vertidx[2];
			logger("face %d, (%d %d %d)\n",j,v0,v1,v2);
		}
		ne=0;
		for (j=0;j<nv;j++) {
			ne+=po->nbs[j].nnb;
			logger("vert %d, %d neighbors ",j,po->nbs[j].nnb);
			for (k=0;k<po->nbs[j].nnb;k++)
				logger("%d ",po->nbs[j].nbs[k]);
			logger("\n");
		}
		if ((ne&1)==0)
			ne>>=1;
		logger("f %d + v %d = 2 + e %d\n",nf,nv,ne); 
		for (j=0;j<nv;j++) {
			logger("vert %d, %d faces ",j,po->nbfs[j].nnbf);
			for (k=0;k<po->nbfs[j].nnbf;k++)
				logger("%d ",po->nbfs[j].nbfs[k]);
			logger("\n");
		}
	}
	init3dbboxes(nphyobjects);
}

static void freephysicsobjects()
{
	for (int i=0;i<nphyobjects;i++) {
		struct phyobject *po = &phyobjects[i];
		if (po->wpnts)
			memfree(po->wpnts);
//		if (po->wfnorms)
//			memfree(po->wfnorms);
		//if (po->contacts)
		//	memfree(po->contacts);
		if (po->lpnts)
			memfree(po->lpnts);
		if (po->lfaces)
			memfree(po->lfaces);
		if (po->nbs) {
			for (int j=0;j<po->nwpnts;j++)
				if (po->nbs[j].nbs)
					memfree(po->nbs[j].nbs);
			memfree(po->nbs);
		}
		if (po->nbfs) {
			for (int j=0;j<po->nwpnts;j++)
				if (po->nbfs[j].nbfs)
					memfree(po->nbfs[j].nbfs);
			memfree(po->nbfs);
		}
	}
}

static int getcolpoint(VEC *pnts,int npnts,struct phyobject *p,VEC *loc,VEC *norm)
{
	int i;
	float dsum=0,d;
	VEC p2o;
	VEC v[MAXBOX2BOX/2];
	VEC vc[MAXBOX2BOX/2];
	float da[MAXBOX2BOX/2];
	VEC crs;
	*loc=zerov;
	for (i=0;i<npnts;i+=2) {
		d=dist3dsq(&pnts[i],&pnts[i+1]);
		da[i>>1]=d;
		loc->x+=pnts[i].x*d;
		loc->y+=pnts[i].y*d;
		loc->z+=pnts[i].z*d;
		loc->x+=pnts[i+1].x*d;
		loc->y+=pnts[i+1].y*d;
		loc->z+=pnts[i+1].z*d;
		dsum+=2*d;
	}
	loc->x/=dsum;
	loc->y/=dsum;
	loc->z/=dsum; 
	npnts/=2;
	for (i=0;i<npnts;i++) {
		v[i].x=pnts[i*2+1].x-pnts[i*2].x;
		v[i].y=pnts[i*2+1].y-pnts[i*2].y;
		v[i].z=pnts[i*2+1].z-pnts[i*2].z;
		vc[i].x=pnts[i*2].x-loc->x;
		vc[i].y=pnts[i*2].y-loc->y;
		vc[i].z=pnts[i*2].z-loc->z;
	}
	*norm=zerov;
	for (i=0;i<npnts;i++) {
		cross3d(&vc[i],&v[i],&crs);
		norm->x+=crs.x*da[i];
		norm->y+=crs.y*da[i];
		norm->z+=crs.z*da[i];
	}
	d=normalize3d(norm,norm);
	if (d<EPSILON) {
//		logger("getcolpoint ret 0\n");
		return 0;
	}
	p2o.x=loc->x-p->st.pos.x;
	p2o.y=loc->y-p->st.pos.y;
	p2o.z=loc->z-p->st.pos.z;
	if (dot3d(&p2o,norm)>0) {
		norm->x=-norm->x;
		norm->y=-norm->y;
		norm->z=-norm->z;
	}
	return 1; 
}

 // generate forces st -> st
static void calcimpulseo2o(struct phyobject *p0,struct phyobject *p1,VEC *loc,VEC *norm)
{
	float k;
	VEC vang0,vang1;
	VEC vrel,vrel0,vrel1;	// obj 1 rel to obj 0 (obj0 space), norm is toward obj 0 away from obj 1
	VEC rc0,rc1;
	VEC force10,torque;
	float top,bot=0;
	VEC rcn0,rcn1,rcnr0,rcnr1;
	rc0.x=loc->x-p0->st.pos.x;
	rc0.y=loc->y-p0->st.pos.y;
	rc0.z=loc->z-p0->st.pos.z;
	cross3d(&p0->st.rotvel,&rc0,&vang0);
	vrel0.x=p0->st.vel.x+vang0.x;
	vrel0.y=p0->st.vel.y+vang0.y;
	vrel0.z=p0->st.vel.z+vang0.z;
	rc1.x=loc->x-p1->st.pos.x;
	rc1.y=loc->y-p1->st.pos.y;
	rc1.z=loc->z-p1->st.pos.z;
	cross3d(&p1->st.rotvel,&rc1,&vang1);
	vrel1.x=p1->st.vel.x+vang1.x;
	vrel1.y=p1->st.vel.y+vang1.y;
	vrel1.z=p1->st.vel.z+vang1.z;
	vrel.x=vrel1.x-vrel0.x;
	vrel.y=vrel1.y-vrel0.y;
	vrel.z=vrel1.z-vrel0.z;
	top=2.0f*dot3d(norm,&vrel);
	if (top<=0)
		return; // moving away
	if (p0->notrans==0)
		bot+=1.0f/p0->mass;
	if (p1->notrans==0)
		bot+=1.0f/p1->mass;
	if (p0->norot==0) {
		cross3d(&rc0,norm,&rcn0);
		doinvmoi(p0,&p0->st.rot,&rcn0,&rcnr0);
		bot+=dot3d(&rcn0,&rcnr0);
	}
	if (p1->norot==0) {
		cross3d(&rc1,norm,&rcn1);
		doinvmoi(p1,&p1->st.rot,&rcn1,&rcnr1);
		bot+=dot3d(&rcn1,&rcnr1);
	}
	if (bot<EPSILON)
		return;
	k=top/bot;
	k*=.5f+.5f*p0->elast*p1->elast*globalelast;
	force10.x=k*norm->x;
	force10.y=k*norm->y;
	force10.z=k*norm->z;
	if (p0->notrans==0) {
		p0->st.momentum.x+=force10.x;
		p0->st.momentum.y+=force10.y;
		p0->st.momentum.z+=force10.z;
		p0->st.vel.x=p0->st.momentum.x/p0->mass;
		p0->st.vel.y=p0->st.momentum.y/p0->mass;
		p0->st.vel.z=p0->st.momentum.z/p0->mass;
	}
	if (p1->notrans==0) {
		p1->st.momentum.x-=force10.x;
		p1->st.momentum.y-=force10.y;
		p1->st.momentum.z-=force10.z;
		p1->st.vel.x=p1->st.momentum.x/p1->mass;
		p1->st.vel.y=p1->st.momentum.y/p1->mass;
		p1->st.vel.z=p1->st.momentum.z/p1->mass;
	}
	if (p0->norot==0) {
		cross3d(&rc0,&force10,&torque);
		p0->st.angmomentum.x+=torque.x;
		p0->st.angmomentum.y+=torque.y;
		p0->st.angmomentum.z+=torque.z;
		doinvmoi(p0,&p0->st.rot,&p0->st.angmomentum,&p0->st.rotvel);
	}
	if (p1->norot==0) {
		cross3d(&rc1,&force10,&torque);
		p1->st.angmomentum.x-=torque.x;
		p1->st.angmomentum.y-=torque.y;
		p1->st.angmomentum.z-=torque.z;
		doinvmoi(p1,&p1->st.rot,&p1->st.angmomentum,&p1->st.rotvel);
	}
}

static void calcimpulseo(struct phyobject *p0,VEC *loc,VEC *norm)
{
	float k;
	VEC vang0;
	VEC vrel;	// obj 1 rel to obj 0 (obj0 space), norm is toward obj 0 away from obj 1
	VEC rc0;
	VEC force10,torque;
	float top,bot=0;
	VEC rcn0,rcnr0;
	rc0.x=loc->x-p0->st.pos.x;
	rc0.y=loc->y-p0->st.pos.y;
	rc0.z=loc->z-p0->st.pos.z;
	cross3d(&p0->st.rotvel,&rc0,&vang0);
	vrel.x=-(p0->st.vel.x+vang0.x);
	vrel.y=-(p0->st.vel.y+vang0.y);
	vrel.z=-(p0->st.vel.z+vang0.z);
	top=2.0f*dot3d(norm,&vrel);
	if (top<=0)
		return; // moving away
	if (p0->notrans==0)
		bot+=1.0f/p0->mass;
	if (p0->norot==0) {
		cross3d(&rc0,norm,&rcn0);
		doinvmoi(p0,&p0->st.rot,&rcn0,&rcnr0);
		bot+=dot3d(&rcn0,&rcnr0);
	}
	if (bot<EPSILON)
		return;
	k=top/bot;
	k*=.5f+.5f*p0->elast*globalelast;
	force10.x=k*norm->x;
	force10.y=k*norm->y;
	force10.z=k*norm->z;
	if (p0->notrans==0) {
		p0->st.momentum.x+=force10.x;
		p0->st.momentum.y+=force10.y;
		p0->st.momentum.z+=force10.z;
		p0->st.vel.x=p0->st.momentum.x/p0->mass;
		p0->st.vel.y=p0->st.momentum.y/p0->mass;
		p0->st.vel.z=p0->st.momentum.z/p0->mass;
	}
	if (p0->norot==0) {
		cross3d(&rc0,&force10,&torque);
		p0->st.angmomentum.x+=torque.x;
		p0->st.angmomentum.y+=torque.y;
		p0->st.angmomentum.z+=torque.z;
		doinvmoi(p0,&p0->st.rot,&p0->st.angmomentum,&p0->st.rotvel);
		//logger("calcimpulseo with loc (%f %f %f), norm (%f %f %f)    ",loc->x,loc->y,loc->z,norm->x,norm->y,norm->z);
        //logger("force = (%f %f %f) torque = (%f %f %f)\n",force10.x,force10.y,force10.z,torque.x,torque.y,torque.z);
	}
}

static float collideobjects(struct phyobject *p0,struct phyobject *p1,/*struct contact *ct,*/int imp)
{
	//return 1;
	int cp;
	int didcoll=0;
	VEC loc,norm;
	VEC resv[MAXBOX2BOX];
	if (p0->kind!=OBJ_BOX || p1->kind!=OBJ_BOX)
		return 0;
	cp=box2box(p0->rpnts,p1->rpnts,resv);
	if (cp)
		if (getcolpoint(resv,cp,p0,&loc,&norm)!=0) {
			didcoll=1;
			if (imp)
				calcimpulseo2o(p0,p1,&loc,&norm); // generate forces
		}
	if (didcoll)
		return 0;
	return 1;

/*	float sp;
	VEC norm,loc;
	sp=meshmesh(p0,p1,ct,&norm,&loc);
	sep=sp;
	if (sp<=0)
		calcimpulseo2o(p0,p1,&loc,&norm); // generate forces
	return sp; */
}

#define NWALLS 6
static VEC wallnorms[NWALLS]={
	{ 0, 1, 0}, // bot
	{ 1, 0, 0}, // left
	{-1, 0, 0}, // right
	{ 0, 0,-1}, // back
	{ 0, 0, 1}, // front
	{ 0,-1, 0}, // top
};
static VEC walllocs[NWALLS]={
	{  0,  0,  0},
	{-50,  0,  0},
	{ 50,  0,  0},
	{  0,  0, 30},
	{  0,  0,-30},
	{  0,100,  0},
};

// calc the force of p1 on p0
static float collideground(struct phyobject *p,int imp)
{
	float minsep=1e20f,sep;
	int k,i;
	VEC loc;//,norm;
//	VEC resv[MAXBOX2PLANE];
	float bmin,d,planed;
	VEC *b;
	b=p->rpnts;
// collide with walls
	for (k=0;k<NWALLS;k++) {
// check bbox with plane
		bmin=dot3d(&b[0],&wallnorms[k]);
		for (i=1;i<NCORNERS;i++) {
			d=dot3d(&b[i],&wallnorms[k]);
			if (d<bmin)
				bmin=d;
		}
		planed=dot3d(&walllocs[k],&wallnorms[k]);
		if (bmin>=planed)
			continue;
/*		if (p->kind==OBJ_BOX) {
			sep=box2plane(p->rpnts,&walllocs[k],&wallnorms[k],resv);
			if (sep<=0)
				if (getcolpoint(resv,cp,p,&loc,&norm)!=0) {
					if (imp)
						calcimpulseo(p,&loc,&norm); // generate forces
				}
		} else if (p->kind>=FIRSTMESHOBJ) { */
			sep=meshplane(p,&walllocs[k],&wallnorms[k],&loc);
			if (sep<=0) {
				if (imp)
					calcimpulseo(p,&loc,&wallnorms[k]); // generate forces
			}
//		}
		if (sep<minsep)
			minsep=sep;
	}
	return minsep;
}

static int collidephysicsobjects(int doimpulse)
{
	//return false;
	struct phyobject *p;
	int i,j;
	int didcoll=0;
	VEC *b0,b0max,b0min;
	for (i=0,p=phyobjects;i<nphyobjects;i++,p++) {
// get bbox
		p->haswf=0;
		for (j=0;j<NCORNERS;j++) {
			p->rpnts[j].x=p->scale.x*p->pnts[j].x;
			p->rpnts[j].y=p->scale.y*p->pnts[j].y;
			p->rpnts[j].z=p->scale.z*p->pnts[j].z;
		}
		quatrots(&p->st.rot,p->rpnts,p->rpnts,NCORNERS);
		for (j=0;j<NCORNERS;j++) {
			p->rpnts[j].x+=p->st.pos.x;
			p->rpnts[j].y+=p->st.pos.y;
			p->rpnts[j].z+=p->st.pos.z;
		}
		b0=p->rpnts;
		b0max=b0min=b0[0];
		for (j=1;j<NCORNERS;j++) {
			if (b0[j].x>b0max.x)
				b0max.x=b0[j].x;
			if (b0[j].y>b0max.y)
				b0max.y=b0[j].y;
			if (b0[j].z>b0max.z)
				b0max.z=b0[j].z;
			if (b0[j].x<b0min.x)
				b0min.x=b0[j].x;
			if (b0[j].y<b0min.y)
				b0min.y=b0[j].y;
			if (b0[j].z<b0min.z)
				b0min.z=b0[j].z;
		}
// set bbox
//		setVEC(&bboxs3d[i].b,-10,-10,-10);
//		setVEC(&bboxs3d[i].e,10,10,10);
		bboxs3d[i].b=b0min;
		bboxs3d[i].e=b0max;
	}
	collide3dboxes();
// collide with each other
/*
	for (i=0;i<nphyobjects;i++)
		for (j=i+1;j<nphyobjects;j++)
			didcoll|=collideobjects(&phyobjects[i],&phyobjects[j],doimpulse);
*/
	for (i=0;i<ncolpairs;i++) {
		struct phyobject *p0,*p1;
		p0=&phyobjects[colpairs3d[i].a];
		p1=&phyobjects[colpairs3d[i].b];
		if (collideobjects(p0,p1/*,&p0->contacts[colpairs3d[i].b]*/,doimpulse)<=0)
			didcoll=1;
	}
// collide on the ground
	for (i=0;i<nphyobjects;i++) {
		float sep = collideground(&phyobjects[i],doimpulse);
		if (sep<=0) {
			didcoll=1;
			//logger("collision with ground, sep = %f\n",sep);
		} else {
			//logger("no collision with ground\n");
		}
	}
	return didcoll;
}

// convert vels to delta distances
static void movephysicsobjects(float timestep)
{
	int i;
	struct pointf3 q;
	struct phyobject *po;
	for (i=0;i<nphyobjects;i++) {
		po=&phyobjects[i];
		po->st.momentum=po->s0.momentum;
		po->st.pos=po->s0.pos;
		po->st.vel=po->s0.vel;
		if (po->notrans)
			continue;
		po->st.pos.x=po->s0.pos.x+
			timestep*po->s0.momentum.x/po->mass;
		po->st.pos.y=po->s0.pos.y+
			timestep*po->s0.momentum.y/po->mass;
		if (!oldgravity) {
			po->st.pos.y-=.5f*littlegee*timestep*timestep;
			po->st.momentum.y-=littlegee*po->mass*timestep;
		}
		po->st.pos.z=po->s0.pos.z+
			timestep*po->s0.momentum.z/po->mass;
		po->st.vel.x=po->st.momentum.x/po->mass;
		po->st.vel.y=po->st.momentum.y/po->mass;
		po->st.vel.z=po->st.momentum.z/po->mass;
		if (oldgravity) {
			po->st.vel.y-=timestep*littlegee;
			po->st.momentum.y=po->mass*po->st.vel.y;
		}
	}
	for (i=0;i<nphyobjects;i++) {
		po=&phyobjects[i];
		po->st.rot=po->s0.rot;
		po->st.rotvel=po->s0.rotvel;
		po->st.angmomentum=po->s0.angmomentum;
		if (po->norot)
			continue;
		if (midpointmethod) { // actually midpoint seems adequate
			VEC sk0;
			VEC w1,k1;
			sk0.w=normalize3d(&po->s0.rotvel,&sk0);
			if (sk0.w) {
				sk0.w*=timestep*.5f;
				rotaxis2quat(&sk0,&sk0);
				quattimes(&sk0,&po->s0.rot,&po->st.rot); // rt = r0 + 1/2 r0
				doinvmoi(&phyobjects[i],&po->st.rot,&po->s0.angmomentum,&w1); //w(1/2)
				k1.w=normalize3d(&w1,&k1);
				if (k1.w) {
					k1.w*=timestep;
					rotaxis2quat(&k1,&k1);
					quattimes(&k1,&po->s0.rot,&po->st.rot); // rt = r0 + w(1/2) 
					quatnormalize(&po->st.rot,&po->st.rot);
					doinvmoi(&phyobjects[i],
						&po->st.rot,&po->st.angmomentum,&po->st.rotvel);
				}
			}
		} else { // euler method
			q.w=normalize3d(&po->s0.rotvel,&q);
			q.w*=timestep;
			if (q.w) {
				rotaxis2quat(&q,&q);
				quattimes(&q,&po->s0.rot,&po->st.rot); // world rel, rt = r0 + w0
				quatnormalize(&po->st.rot,&po->st.rot);
			}
			doinvmoi(&phyobjects[i],
				&po->st.rot,&po->st.angmomentum,&po->st.rotvel);
		}
	}
}

// move data from physics struct to tree struct
static void drawprepphysicsobjects()
{
	int k;
	struct phyobject *po;
	for (k=0;k<nphyobjects;k++) {
		po=&phyobjects[k];
		po->t->trans=po->st.pos; // world rel
		po->t->scale=po->scale; // world rel
		po->t->rot=po->st.rot;
		logger("frame = %4d, trans = (%f,%f,%f), rot = (%f,%f,%f,%f)\n",framenum,po->st.pos.x,po->st.pos.y,po->st.pos.z,po->st.rot.x,po->st.rot.y,po->st.rot.z,po->st.rot.w);
		++framenum;
	} 
}

// get data in and out of debprint
static void	getdebprintphysicsobjects()
{
// get data from debprint
	if (nphyobjects<=0)
		return;
	quatnormalize(&curphyobject.s0.rot,&curphyobject.s0.rot);
	if (!curphyobject.notrans) {
		curphyobject.s0.vel.x=curphyobject.s0.momentum.x/curphyobject.mass;
		curphyobject.s0.vel.y=curphyobject.s0.momentum.y/curphyobject.mass;
		curphyobject.s0.vel.z=curphyobject.s0.momentum.z/curphyobject.mass;
	}
	if (!curphyobject.norot)
		doinvmoi(&curphyobject,
			&curphyobject.s0.rot,&curphyobject.s0.angmomentum,&curphyobject.s0.rotvel);
	curphyobjectnum=range(0,curphyobjectnum,nphyobjects-1);
	if (lastcurphyobjectnum==curphyobjectnum)
		phyobjects[curphyobjectnum]=curphyobject;
}

static void	setdebprintphysicsobjects()
{
// send data to debprint;
	if (nphyobjects<=0)
		return;
	lastcurphyobjectnum=curphyobjectnum;
	curphyobject=phyobjects[curphyobjectnum];
}

static void copynew2old()
{
	int i;
	for (i=0;i<nphyobjects;i++)
		phyobjects[i].s0=phyobjects[i].st; // world rel
}

static void calcenergynew()
{
	int i;
	VEC t;
	struct phyobject *po;
	totenergy=totrotenergy=tottransenergy=totpotenergy=0;
	totangmomentum=totmomentum=totangorgmomentum=totangcmmomentum=zerov;
	for (i=0;i<nphyobjects;i++) {
		po=&phyobjects[i];
		if (!po->notrans) {
			po->transenergy=.5f*len3dsq(&po->st.vel)*po->mass;
			po->potenergy=po->mass*littlegee*po->st.pos.y;
		}
		if (!po->norot) {
			calcangenergy(&phyobjects[i]);
			cross3d(&po->st.pos,&po->st.vel,&t);
			t.x*=po->mass;
			t.y*=po->mass;
			t.z*=po->mass;
			totangorgmomentum.x+=t.x;
			totangorgmomentum.y+=t.y;
			totangorgmomentum.z+=t.z;
			totangmomentum.x+=po->st.angmomentum.x+t.x;
			totangmomentum.y+=po->st.angmomentum.y+t.y;
			totangmomentum.z+=po->st.angmomentum.z+t.z;
		}
		tottransenergy+=po->transenergy;
		totpotenergy+=po->potenergy;
		totrotenergy+=po->rotenergy;
		totenergy+=po->transenergy+po->potenergy+po->rotenergy;
		totmomentum.x+=po->st.momentum.x;
		totmomentum.y+=po->st.momentum.y;
		totmomentum.z+=po->st.momentum.z;
		totangcmmomentum.x+=po->st.angmomentum.x;
		totangcmmomentum.y+=po->st.angmomentum.y;
		totangcmmomentum.z+=po->st.angmomentum.z;
	}
}

static void procphysicsobjects(float timestep,int iterations)
{
	int i;
	float thresh;
	float t0,t1,timeleft;
	getdebprintphysicsobjects();
	if (iterations<=0) { // analyse 1 timestep (timestep2)
		movephysicsobjects(timestep2); // s0 -> st
		calcenergynew(); // st
	} else {
		timestep/=iterations;
		if (bisect>0)
			thresh=timestep/(1<<bisect);
		for (i=0;i<iterations;i++) {
			if (bisect>0) { // finer collision time checking, objects don't touch (maybe alittle)
				timeleft=timestep;
				while (timeleft>0) {
					movephysicsobjects(0);
					if (collidephysicsobjects(0)) { // trouble, objects touch at t=0
						if (timeleft<thresh) {
							movephysicsobjects(timeleft); // s0 -> st, just doit the oldway
							collidephysicsobjects(1); // st -> st
							calcenergynew(); // st
							copynew2old(); // st -> s0
							timeleft=0;
						} else {
							movephysicsobjects(thresh); // s0 -> st, just doit the oldway
							collidephysicsobjects(1); // st -> st
							calcenergynew(); // st
							copynew2old(); // st -> s0
							timeleft-=thresh;
						}
					} else {
						movephysicsobjects(timeleft);
						if (!collidephysicsobjects(0)) { // no collisions during whole timestep
							movephysicsobjects(timeleft); // s0 -> st
							calcenergynew(); // st
							copynew2old(); // st -> s0
							timeleft=0;
						} else { // collision happened sometime inbetween
							t0=0;
							t1=timeleft;
							timestep=(t0+t1)*.5f;
							while (t1-t0>thresh) {
								movephysicsobjects(timestep);
								if (collidephysicsobjects(0))
									t1=timestep;
								else
									t0=timestep;
								timestep=(t0+t1)*.5f;
							}
							movephysicsobjects(timestep); // or t1
							collidephysicsobjects(1); // st -> st
							calcenergynew(); // st
							copynew2old(); // st -> s0
							timeleft-=timestep;
						}
					}
				}
			} else { // collision with some penatration, no bisection
				movephysicsobjects(timestep); // s0 -> st
				collidephysicsobjects(1); // st -> st
				calcenergynew(); // st
				copynew2old(); // st -> s0
			}
		}
	}
	drawprepphysicsobjects();
	setdebprintphysicsobjects();
}

} // end namespace physics3d

using namespace physics3d;

void physics3dinit()
{
// setup video bitmap
	video_setupwindow(GX,GY);
// set extra debprint vars
	extradebvars(edv,nedv);
// setup viewport
	mainvp.backcolor=C32LIGHTBLUE;
	mainvp.zfront=.025f;
	mainvp.zback=10000;
	mainvp.camzoom=3.2f; // 1; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
	mainvp.isortho=false;
	mainvp.ortho_size=30;
	mainvp.camtrans.x = 0;
	mainvp.camtrans.y = 27;
	mainvp.camtrans.z = -100;
	mainvp.camrot.x = 0;
	mainvp.camrot.y = 0;
	mainvp.camrot.z = 0;
// build scene heirarchy
	roottree=new tree2("roottree");
	pushandsetdir("physics3d");
// backgnd
	footballfield = new tree2("footballfield.BWS");
	roottree->linkchild(footballfield);
// worldobjects
	initworldobjects("objects.txt");
// instances of worldobjects
	if (ascene.empty()) {
		script scenes("pickscene.txt");
		int nscene = scenes.num();
		if (nscene!=1)
			errorexit("pick just 1 scene");
		ascene = scenes.idx(0);
	}
	initphysicsobjects(ascene.c_str());
	popdir();
	framenum = 0;
}

void physics3dproc()
{
// handle input
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (KEY=='a')
		showcursor(1);
	if (KEY=='h')
		showcursor(0);
	if (KEY==' ')
		video3dinfo.favorshading^=1;
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
		changestate(STATE_PHYSICS3D);
	}
	if (KEY=='s') {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
		changestate(STATE_PHYSICS3D);
	}
	if (KEY=='=') {
		changeglobalxyres(1);
		changestate(STATE_PHYSICS3D);
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		changestate(STATE_PHYSICS3D);
	}
	if (wininfo.justdropped) {
		ascene = wininfo.args->idx(0);
		C8 nameext[500];
		mgetnameext(ascene.c_str(),nameext);
		if (strcmp(nameext,"objects.txt") && strcmp(nameext,"pickscene.txt")) // if not the start scene and not the list of physics objects
			changestate(STATE_PHYSICS3D); // must be an actual physics scene
	}
// handle physics objs
	static int slow;
	if (slow == 0) {
		procphysicsobjects(timestep,iterations);
		slow = 1;
	}
	--slow;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	doflycam(&mainvp);
	roottree->proc();
}

void physics3ddraw3d()
{
	video_setviewport(&mainvp); // clear zbuf etc.
	video_buildworldmats(roottree);
	dolights();
	video_drawscene(roottree);
}

void physics3dexit()
{
	// free custom debprint
	extradebvars(0,0);
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
// free heirarchy
	delete roottree;
	roottree = 0;
	freeworldobjects();
	freephysicsobjects();
	for (int i=0;i<MAXWORLDOBJECTS;i++)
		objkindstr[i] = "";
	logger("logging reference lists after free\n");
	logrc();
}
