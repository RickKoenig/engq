/*#include <stdio.h>
#include <string.h>

#include <engine7cpp.h>
*/
#define D2_3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "n_deflatecpp.h"
#include "n_usefulcpp.h"

float weapscale;

/*void freeweap(tree2* weaptree)
{
	if (weaptree) {
		freetree(weaptree);
		weaptree=0;
	}
}
*/

// return keep, selects regpoint or -1 if none
static int getweapuserinfo(tree2* t,int *regidx)
{
	int i;
	char tokstr[100];
	char *tok;
	*regidx=-1;
//	if (!strncmp(t->name,"mdl",3)) // throw out mdl's
//		return 0;
//	if (!strncmp(t->name,"dum",3)) // throw out dum's
//		return 0;
//	if (!strncmp(t->name,"tp",2)) // throw out tp's
//		return 0;
// keep grp's
	for (i=0;i<t->userprop.num();i++) // look for partlinkinf
		if (!strncmp(t->userprop.idx(i).c_str(),"partLinkInf",11))
			break;
	if (i==t->userprop.num())
		return 1;
	strcpy(tokstr,&t->userprop.idx(i).c_str()[12]);
	tok=strtok(tokstr,",\n\r ");
	for (i=0;i<REGPOINT_NREGPOINTS;i++)
		if (!my_stricmp(tok,regpointnames[i]))
			break;
	if (i!=REGPOINT_NREGPOINTS) {
		*regidx=i;
		logger("weaponlinkinfo '%s' linked to '%s'\n",t->name.c_str(),regpointnames[i]);
	}
	return 1;
}

tree2* loadweap(tree2* carnull,tree2* carbody,const char *weapname)
{
	pointf3 regpointsoffset[REGPOINT_NREGPOINTS];
	char str[50];
	struct mat4 regmat;
	tree2* xxx;
	list<tree2*>::iterator it;
//	S32 j;
	sprintf(str,"weapons/%s",weapname);
	pushandsetdir(str);
	char s[50];
	tree2* ret;
	sprintf(s,"%s.mxs",weapname);
//	ret=loadscene(s);
	ret=new tree2(str);
//	seq_stop(ret);
	seq_setframe(ret,0);
	popdir();
/*	od.cartex[op->carid]=getcartex(op->onlinecar);
	if (od.cartex[op->carid]) {
		struct bitmap16 *b;
		logger("cartex %d found, name '%s' lg size %d %d\n",
			op->carid,od.cartex[op->carid]->name,od.cartex[op->carid]->logu,od.cartex[op->carid]->logv);
		b=locktexture(od.cartex[op->carid]);
		od.cartexsave[op->carid]=bitmap16alloc(b->x,b->y,-1);
		clipblit16(b,od.cartexsave[op->carid],0,0,0,0,b->x,b->y);
		unlocktexture(od.cartex[op->carid]);
		if (od.cartex[op->carid]->texformat==TEX565NCK)
			od.cardarktexsave[op->carid]=darkentex565(od.cartexsave[op->carid]);
		if (od.cartex[op->carid]->texformat==TEX555NCK)
			od.cardarktexsave[op->carid]=darkentex555(od.cartexsave[op->carid]);
	}
	linkchildtoparent(op->onlinecar,op->ol_carnull); */
	xxx=findtreestrstrrec(carbody,"regpoints");
	if (!xxx)
		errorexit("can't find onlinecar 'regpoints'");
	xxx->name="theregpoints";
	if (xxx->children.size()!=REGPOINT_NREGPOINTS)
		errorexit("number of regpoints != %d",REGPOINT_NREGPOINTS);
//	for (i=0;i<REGPOINT_NREGPOINTS;i++) {
	S32 i;
	for (i=0,it=xxx->children.begin();it!=xxx->children.end();++i,++it) {
//		extern float helperscale;
//		int nv;
		modelb *m;
		pointf3 center;
//		m=xxx->children[i]->mod;
		m=(*it)->mod;
		if (!m)
			errorexit("no regpoint");
//		S32 nv=m->nverts;
//		if (nv!=4 && nv!=8)
//			errorexit("bad regpoint, nvert = %d, name = '%s'",nv,m->name.c_str());
/*		center=zerov;
		for (j=0;j<nv;j++) {
			center.x+=m->verts[j].x;
			center.y+=m->verts[j].y;
			center.z+=m->verts[j].z;
		}
		center.x/=nv;
		center.y/=nv;
		center.z/=nv; */
		center=zerov;
		buildscalerottrans3d(&xxx->scale,&xxx->rot,&xxx->trans,&regmat);
		xformvec(&regmat,&center,&center);
		buildscalerottrans3d(&carbody->scale,&carbody->rot,&carbody->trans,&regmat);
		xformvec(&regmat,&center,&regpointsoffset[i]);
		logger("regpoint %2d: %f %f %f\n",i,regpointsoffset[i].x,regpointsoffset[i].y,regpointsoffset[i].z);
	}
	xxx->flags|=TF_DONTDRAWC;
//	freeweap();
//	for (i=0;i<ret->nchildren;i++) {
	for (it=ret->children.begin();it!=ret->children.end();++it) {
		tree2* t;
		int keep,regidx;
//		t=ret->children[i];
		t=(*it);
		t->userint[0]=-1;
		logger("weapgroup name '%s'\n",t->name.c_str());
		keep=getweapuserinfo(t,&regidx);
		if (keep) {
			if (regidx>=0) {
				t->userproc=0;
				t->trans=regpointsoffset[regidx];
				t->userint[0]=regidx;
			}
			t->scale.x*=weapscale;
			t->scale.y*=weapscale;
			t->scale.z*=weapscale;
		} else
//			freetree(ret->children[j]);
//			delete ret->children[i];
			delete *it;
	}

// free up extra weapon hierarchy
/*	for (j=0;j<od.nfreeweaplist;j++) {
		xxx=findtreenamerec(op->ol_carnull,od.freeweaplist[j]);
		if (xxx)
//				errorexit("can't find '%s' to free",od.freeweaplist[j]);
		freetree(xxx);
	} */
	return ret;
}

/*

*/
