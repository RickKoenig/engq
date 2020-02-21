#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "carenalobby.h"
#include "jrmcars.h"
#include "n_loadweapcpp.h"
#include "enums.h"
#include "carclass.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "newconstructor.h"
#include "carenagame.h"
//#include "n_packetcpp.h"
#include "tracklist.h"
#include "system/u_states.h"
#include "ai.h"
#include "scrline.h"
#include "rematch.h"
#include "connectstate.h"
#include "line2road.h"
#include "timewarp.h"
#include "netdescj.h"
#include "envj.h"
#include "avaj.h"
#include "gamerunj.h"

// convert 1/60 sec to 
string getmsh(S32 t) // mm:ss:hh	// h is 1/100 sec
{
	C8 neg[2];
	if (t < 0) {
		neg[0]='-';
		neg[1]='\0';
		t = -t;
	} else {
		neg[0]='\0';
	}
	C8 str[100];
	S32 h = (10*t + 3)/6;
	S32 s = h/100;
	h%=100;
	S32 m = s/60;
	s%=60;
	sprintf(str,"%s%02d:%02d:%02d",neg,m,s,h);
	return str;
}

string getsh(S32 t) // ss:hh
{
	if (t < 0)
		t = 0;
	C8 str[100];
	S32 h = (10*t + 3)/6;
	S32 s = h/100;
	h%=100;
	sprintf(str,"%02d:%02d",s,h);
	return str;
}

string getplacestr(S32 p)
{
	const C8* pstrs[4] = {"th","st","nd","rd"};
	stringstream ss;
	S32 d2 = p%100;
	S32 tens = d2/10;
	S32 ones = d2%10;
	S32 idx = 0; // th
	if (ones < 4) // 0th,1st,2nd,3rd
		idx = ones;
	if (tens == 1) // 11th,12th, etc.
		idx = 0;
	ss << p << pstrs[idx];
	return ss.str();
}

//#include "n_jsonstructs.h"

tree2* scrlinenull;
static fontq* fnt;
void initscrline()
{
	scrlinenull=new tree2("scrlinenull");
#if 0
	logger("test clock\n");
	S32 i;
	for (i=0;i<100000;++i)
		logger("%6d: '%s'\n",i,getmsh(i).c_str());
#endif
#if 0
	logger("test place string\n");
	S32 p;
	for (p=0;p<300;++p)
		logger("%6d: '%s'\n",p,getplacestr(p).c_str());
#endif
	pushandsetdir("fonts");
	fnt = new fontq("beat");
	fnt->setcenter(true);
	popdir();
}

void procscrline()
{
}

// in spritedraw with GAMESPTX,GAMESPTY
void drawsptscrline(S32 resx,S32 resy)
{
	gg->gs.clock;
	fnt->setscale(1.0f);
//	fnt->setfixedwidth(20.0f);
	const S32 sizex = 200;
	const S32 sizey = 32;
	const S32 startx = (resx-sizex)/2;
	const S32 starty = 0;
	S32 clk = gg->gs.clock - gg->gs.clockoffset - acv2.candrivetime;
	if (clk<0)
		clk = 0;
	const twcaro& cs = gg->caros[gg->viewslot].cs;
	if (cs.fintime)
		clk = cs.fintime;
	S32 ft = cs.fintime;
// clock
	fnt->print(resx - sizex - 2.0f,0,(float)sizex,(float)sizey,F32BLACK,"%s",getmsh(clk).c_str());
	fnt->print(resx - sizex - 2.0f,2,(float)sizex,(float)sizey,F32YELLOW,"%s",getmsh(clk).c_str());
	if (ft) { // final place
		clk = ft;
		fnt->print((float)startx,(float)sizey,(float)sizex,(float)sizey,F32YELLOW,"%s",getplacestr(cs.finplace).c_str());
		fnt->print((float)startx,(float)starty,(float)sizex,(float)sizey,F32YELLOW,"%s",getmsh(clk).c_str());
	} else { // current place
		S32 p = gg->plc->getplace(gg->viewslot);
		if (p>0) {
			fnt->print((float)startx,(float)sizey,(float)sizex,(float)sizey,F32YELLOW,"%s",getplacestr(p).c_str());
			S32 tm = gg->plc->gettime(gg->viewslot);
			if (tm)
				fnt->print((float)startx,(float)starty,(float)sizex,(float)sizey,tm>0?F32RED:F32GREEN,"%s",getmsh(tm).c_str());
		}
	}
#if 0
	void setcenter(bool ca) {centerx=ca;centery=ca;}
	void setcenterx(bool cax) {centerx=cax;}
	void setcentery(bool cay) {centery=cay;}
	void setfixedwidth(float fwa);
	void setscale(float sa) {scl=sa;}
	void setspace(float spca) {spc=spca;}
	void setcharspace(float sca) {sc=sca;}
	void setleftmarg(float lma) {leftmarg=lma;}
	void settopmarg(float tma) {topmarg=tma;}
	void print(float x,float y,float xs,float ys,const pointf3& colf,const C8* fmt,...);
#endif
}

void exitscrline()
{
	delete scrlinenull;
	delete fnt;
	fnt = 0;
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
