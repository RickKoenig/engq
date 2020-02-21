#define D2_3D
#include <m_eng.h>
#include <d3d9.h>

#include "gameinfo.h"
#include "../u_modelutil.h"
#include "../u_s_toon.h"
#include "m_vid_dx9.h"

#include "carclass.h"

static void applyhsv(const bitmap32* basepic,const bitmap8* palpic,textureb* outtex,const hsv* curhsvs)
{
	if (!basepic)
		return;
	if (!palpic)
		return;
	if (basepic->size.x!=palpic->size.x || basepic->size.y!=palpic->size.y)
		return;
	if (outtex->tsize.x!=palpic->size.x || outtex->tsize.y!=palpic->size.y)
		return;
	S32 prod=basepic->size.x*basepic->size.y;
	S32 i,j;
	C32* src=basepic->data;
	U8* palp=palpic->data;
	bitmap32* dest=bitmap32copy(basepic);
	C32* dst=dest->data;
	for (j=0;j<NUMHSVS;++j) {
		for (i=0;i<prod;++i) {
			U32 pv=254-j;
			if (pv==palp[i]) {
				S32 r,g,b,a,h,s,v;
				r=src[i].r;
				g=src[i].g;
				b=src[i].b;
				a=src[i].a;
				S32 r2,g2,b2;
				S32 s2=supersat(curhsvs[j].s,r,g,b,&r2,&g2,&b2); // sat 0-319, 256-319 (colorizes gray), returns sat 0-255
				rgb2hsv(r2,g2,b2,&h,&s,&v);
				h+=curhsvs[j].h;
				s=s*s2/255;
				v=v*curhsvs[j].v/255;
				h%=360;
				hsv2rgb(h,s,v,&r,&g,&b);
				dst[i].r=r;
				dst[i].g=g;
				dst[i].b=b;
				dst[i].a=a;
			}
		}
	}
	outtex->addbitmap(dest,false);
	bitmap32free(dest);
}

static void applyrgb(const bitmap32* basepic,const bitmap8* palpic,textureb* outtex,const hsv* currgbs)
{
	if (!basepic)
		return;
	if (!palpic)
		return;
	if (basepic->size.x!=palpic->size.x || basepic->size.y!=palpic->size.y)
		return;
	if (outtex->tsize.x!=palpic->size.x || outtex->tsize.y!=palpic->size.y)
		return;
	S32 prod=basepic->size.x*basepic->size.y;
	S32 i,j;
	C32* src=basepic->data;
	U8* palp=palpic->data;
	bitmap32* dest=bitmap32copy(basepic);
	C32* dst=dest->data;
	for (j=0;j<NUMHSVS;++j) {
		for (i=0;i<prod;++i) {
			U32 pv=254-j;
			if (pv==palp[i]) {
				S32 r,g,b,a;
				r=src[i].r;
				g=src[i].g;
				b=src[i].b;
				a=src[i].a;
				r=range(0U,4*r*currgbs[j].h/255,255U);
				g=range(0U,4*g*currgbs[j].s/255,255U);
				b=range(0U,4*b*currgbs[j].v/255,255U);
				dst[i].r=r;
				dst[i].g=g;
				dst[i].b=b;
				dst[i].a=a;
			}
		}
	}
	outtex->addbitmap(dest,false);
	bitmap32free(dest);
}

// handles the car and it's hsv, maybe later the regpoints also...
carclass::carclass(const C8* carname) : 
	carbasepic(0),carpalpic(0),
	carbasepic2(0),carpalpic2(0),
	cargraypic(0),cargraypic2(0),
	cartex(0),cartex2(0),canhsv(COLNONE)
{
	bool maindds=false,palpcx=false,graydds=false;
	C8 bwsdirstr[250];
	C8 bwsnamestr[250];
	C8 graystr[250];
	C8 grayalphastr[250];
	C8 mainstr[250];
	C8 alphastr[250];
	C8 palstr[250];
	C8 palalphastr[250];
// dirs/bws
	sprintf(bwsdirstr,"racenetdata/2009_cars/%s",carname);
	sprintf(bwsnamestr,"%s.bws",carname);
// textures/bitmaps
	sprintf(mainstr,"%s.dds",carname); 
	sprintf(alphastr,"%s_w.dds",carname); 
	sprintf(palstr,"%s.pcx",carname);
	sprintf(palalphastr,"%s_w.pcx",carname);
	sprintf(graystr,"%s_grayscale.dds",carname); 
	sprintf(grayalphastr,"%s_grayscale_w.dds",carname); 
	sprintf(graystr,"%s.dds",carname); 
	sprintf(grayalphastr,"%s_w.dds",carname); 
pushandsetdir(bwsdirstr); // this is where all cardata resides (except common)
// hierarchy and model
	cartree=new tree2(bwsnamestr);
// textures
// get orig main texture and hide it's name
	cartex=texture_create(mainstr);
	cartex->name=unique();
// get orig alpha texture and hide its name
	cartex2=texture_create(alphastr);
	cartex2->name=unique();
// bitmaps
// get pal bitmap
	C32 dcs[256];
	if (fileexist(palstr)) {
		carpalpic=gfxread8(palstr,dcs);
		palpcx=true;
	}
// get gray bitmap
	if (fileexist(graystr)) {
		graydds=true;
		cargraypic=gfxread32(graystr);
	}
// if we got gray and pal, then canrgb
	if (graydds && palpcx)
		canhsv=COLRGB;
	if (canhsv!=COLRGB) {
// get orig main bitmap
		if (fileexist(mainstr)) {
			maindds=true;
			carbasepic=gfxread32(mainstr);
		}
// if we got main and pal, then canhsv
		if (maindds && palpcx)
			canhsv=COLHSV;
	}
// get pal alpha bitmap
	if (fileexist(palalphastr)) {
		carpalpic2=gfxread8(palalphastr,dcs);
	}
// get orig gray alpha bitmap
	if (fileexist(grayalphastr)) {
		cargraypic2=gfxread32(grayalphastr);
	}
// get orig alpha bitmap
	if (canhsv!=COLRGB) {
		if (fileexist(alphastr)) {
			carbasepic2=gfxread32(alphastr);
		}
	}
popdir();
// create edge models if tooning
if (videoinfodx9.tooningame) {
		edgeify(cartree);
	}
//	if (rt)
//		rt->linkchild(cartree);
}

void carclass::changehsv(const hsv* curhsvs)
{
	if (canhsv==COLHSV) {
		colorkeyinfo.lasthascolorkey=0;
		applyhsv(carbasepic,carpalpic,cartex,curhsvs);
		colorkeyinfo.lasthascolorkey=1;
		applyhsv(carbasepic2,carpalpic2,cartex2,curhsvs);
	} else if (canhsv==COLRGB) {
		colorkeyinfo.lasthascolorkey=0;
		applyrgb(cargraypic,carpalpic,cartex,curhsvs);
		colorkeyinfo.lasthascolorkey=1;
		applyrgb(cargraypic2,carpalpic2,cartex2,curhsvs);
	}
}

carclass::~carclass() // make sure this gets called 'before' delete roottree...
{
	delete cartree;
	if (carbasepic)
		bitmap32free(carbasepic);
	if (carpalpic)
		bitmap8free(carpalpic);
	if (carbasepic2)
		bitmap32free(carbasepic2);
	if (carpalpic2)
		bitmap8free(carpalpic2);
	if (cargraypic)
		bitmap32free(cargraypic);
	if (cargraypic2)
		bitmap32free(cargraypic2);
	textureb::rc.deleterc(cartex);
	textureb::rc.deleterc(cartex2);
}

static tree2 *findawheel(tree2 *t,char *tirename)
{
	tree2 *r;
	U32 p=t->name.find(tirename);
	if (p!=string::npos)
		return t;
	list<tree2*>::iterator it;
	for (it=t->children.begin();it!=t->children.end();++it) {
		r=findawheel(*it,tirename);
		if (r)
			return r;
	}
	return 0;
}

static char *wheelnames[6]={
	"frontleft",
	"frontright",
	"backleft",
	"backright",
	"middleleft",
	"middleright",
};

void carclass::changerims(S32 rimnum)
{
	if (rimnum<1 || rimnum>12)
		return;
	tree2* rimnulls[6];
	S32 i;
	for (i=0;i<6;++i) {
		rimnulls[i]=findawheel(cartree,wheelnames[i]); // look for a wheelname
		if (rimnulls[i] && !rimnulls[i]->mod) { // is it null?
			while(rimnulls[i]->children.size()) { // remove everything attached to null wheel
				list<tree2*>::iterator it=rimnulls[i]->children.begin();
				delete *it;
			}
			pushandsetdir("racenetdata/rims");
			C8 str[50];
			sprintf(str,"rim%02d.bwo",rimnum);
			tree2* rim=new tree2(str);
			if (videoinfodx9.tooningame)
				edgeify(rim);
			rimnulls[i]->linkchild(rim);
			popdir();

		}
	}
}
