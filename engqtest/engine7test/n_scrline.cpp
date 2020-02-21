
/*
#include <engine7cpp.h>

#include "usefulcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "carenagamecpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_carclass.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"
#include "n_aicpp.h"
#include "n_scrlinecpp.h"
#include "n_physicscpp.h"
#include "n_rematchcpp.h"
#include "n_carenaconnectcpp.h"
#include "n_line2roadcpp.h"
#include "n_twobjcpp.h"

void loadscrline()
{
	gg.scrlinenull=new tree2("scrlinenull");
}

void updatescrline()
{
}

void freescrline()
{
	delete gg.scrlinenull;
}


#if 0
static void fiddletex(tree2* s)
{
	int i,j,k,m;
	struct bitmap32* b;
	modelb* mod;
	mater2* mat;
	if (mod=s->mod) {
		S32 nmat=mod->mats.size();
		for (k=0,mat=&mod->mats[0];k<nmat;k++,mat++) {
			mat->msflags&=~SMAT_HASWBUFF; // no wbuff
//			mat->mtrans=1; // no xpar
			mat->color.w=1; // no xpar
//			if (mat->thetex) {
			if (mat->thetexarr[0]) {
/*				for (m=0;m<mat->thetex->nmipmap;m++) {
					b=locktexturei(mat->thetex,m); // black and white checker pattern
					for (j=0;j<b->y;j++)
						for (i=0;i<b->x;i++) {
							int c=((i+j)&1) ? 0xffff : 1;
							c=1;
							clipputpixel16(b,i,j,c);
						}
					unlocktexturei(mat->thetex,m);
				} */
			}
		}
	}
//	for (i=0;i<(S32)(s->children.size());i++)
//		fiddletex(s->children[i]);
	list<tree2*>::iterator it;
	for (it=s->children.begin();it!=s->children.end();++it)
		fiddletex(*it);
}

void loadscrline()
{
//	int k,i,j;
	struct bitmap32* b;
	struct mater2* mt;
	tree2* s,*top;
//	int clipmapsave=clipmap; // use colorkey only
//	clipmap=CLIPMAP_COLORKEY; // colorkey
//	gg.scrlinenull=alloctree(2,0); // 2 parts
//	mystrncpy(gg.scrlinenull->name,"scrlinenull",NAMESIZE); // name it
	gg.scrlinenull=new tree2("scrlinenull");
pushandsetdirdown("maxscrline");
	;//s=loadscene("scrlnebtm3.mxs"); // bottom fix..
	fiddletex(s); // see above
//	linkchildtoparent(s,gg.scrlinenull);
	gg.scrlinenull->linkchild(s);
	;//s=loadscene("scrlnetop2.mxs"); // top, fix..
	fiddletex(s); // see above
	top=findtreenamerec(s,"scrlnetop2.mxo"); // look for clock texture
	if (!top)
		errorexit("can't find top");
	mt=findmaterial(top,"trnscn02.right");
	if (!mt)
		errorexit("can't find vs clock");
	gg.clocktex=mt->thetex;
	b=locktexture(gg.clocktex);
	gg.clocksave=bitmap32alloc(b->size.x,b->size.y,10); // save old clock tex
	clipblit16(b,gg.clocksave,0,0,0,0,b->size.x,b->size.y);
	unlocktexture(gg.clocktex); 
	linkchildtoparent(s,gg.scrlinenull);
	clocktsp=loadtspo("clock.tsp",0,gg.clocktex->texformat,0); // load clock digits
	popdir();
//	clipmap=clipmapsave;
	gg.oldtt=-1;
/*	for (k=0;k<clocktsp->nframe;k++) {
		struct bitmap16 *b;
		b=clocktsp->frames[k];
		if
	} */
}

void updatescrline()
{
	gg.scrlinenull->trans.z=gcfg.scrlinez;
	int tt,m,shi,slo;
/*	if (of->clocktickcount<INITCANTSTARTDRIVE)
		tt=0;
	else {
		if (of->dofinish)
			tt=of->dofinish-INITCANTSTARTDRIVE;
		else
			tt=of->clocktickcount-INITCANTSTARTDRIVE;
		tt/=60;
	} */
	tt=(gg.clock-gg.clockoffset)/60;
	tt=range(0,tt,10*60-1);
	if (gg.oldtt!=tt) {
		struct bitmap16 *ct,*b16;
		gg.oldtt=tt;
		m=tt/60;
		shi=slo=tt%60;
		shi/=10;
		slo%=10;
		b16=locktexture(gg.clocktex);
//		cliprect16(b16,0,0,b16->x-1,b16->y-1,0xff);
//		clipxpar16(gg.clocksave,b16,0,0,0,0,b16->x,b16->y);
		clipblit16(gg.clocksave,b16,0,0,0,0,b16->x,b16->y);
		ct=clocktsp->frames[m];
		clipxpar32(ct,b16,0,0,10,1,ct->x,ct->y);
		ct=clocktsp->frames[shi];
		clipxpar32(ct,b16,0,0,50,1,ct->x,ct->y);
		ct=clocktsp->frames[slo];
		clipxpar32(ct,b16,0,0,90,1,ct->x,ct->y);
		unlocktexture(gg.clocktex); 
	}
}

void freescrline()
{
//	freetree(gg.scrlinenull);
	delete gg.scrlinenull;
//	freetsp(clocktsp);
	bitmap32free(gg.clocksave);
}
#endif
