#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "u_uipopup_info.h"
#include "u_factory.h"
#include "u_uiclass.h"
#include "u_uiclass2.h"

template <typename T>
static void rect_changegnamefun(const C8* wname,uitree* uit,const pointf2& popup_loc)
{
	T* cn=new T(wname,50,70,300,200,"changename.uis");
	uit->linkchild(cn);
//	cn->addrem_controls();
}

template <typename T>
static void rect_loadsavegfun(const C8* wname,uitree* uit,const pointf2& popup_loc,bool issavea,bool prevua,const C8** extsa)
{
	T* cn;
	if (issavea)
		cn=new T(wname,getcurdir(),0,0,0,0,issavea,prevua,extsa,"save.uis");
	else
		cn=new T(wname,getcurdir(),0,0,0,0,issavea,prevua,extsa,"load.uis");
	uit->linkchild(cn);
//	cn->addrem_controls();
}

static void rect_changenamefun(uitree* uit,const pointf2& popup_loc)
{
	rect_changegnamefun<changename>("change name",uit,popup_loc);
}

static void rect_loadscriptfun(uitree* uit,const pointf2& popup_loc)
{
	static const C8 *scriptloadexts[]={"uis",0};
	rect_loadsavegfun<loadscriptdialog>("load script",uit,popup_loc,/*&uitree::loadscript2fn*/false,false,scriptloadexts);
}

static void rect_savescriptfun(uitree* uit,const pointf2& popup_loc)
{
	static const C8 *scriptsaveexts[]={"uis",0};
	rect_loadsavegfun<savescriptdialog>("save script",uit,popup_loc,/*&uitree::loadscript2fn*/true,false,scriptsaveexts);
//	rect_loadsavegfun("save script",uit,popup_loc,0,0,&uitree::savescript2fn,true,false);
}

static void rect_newrectfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newrect called\n");
	uitree2drect* bm=new uitree2drect("newrect",popup_loc.x,popup_loc.y,150,100);
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newbitmapfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbitmapname called\n");
	uitree2dbitmap* bm=new uitree2dbitmap("newbitmapname",popup_loc.x,popup_loc.y,150,100,.5f,.5f,"ui","gamesel.pcx");
	uit->linkchild(bm);
	bm->addrem_controls();
	bm->addrem_scroll();
	uitree::setfocus(bm);
}

static void rect_newbitmapscalefun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbitmapscalename called\n");
	uitree2dbitmap* bm=new uitree2dbitmapscale("newbitmapscale",popup_loc.x,popup_loc.y,150,100,0,0,"ui","rengst.jpg");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newtexterfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newtexter called\n");
	texter* bm=new texter("texter",popup_loc.x,popup_loc.y,150,100,"some text");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_neweditboxfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("neweditbox called\n");
	editbox* bm=new editbox("editbox",popup_loc.x,popup_loc.y,150,100,"an editbox");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newbuttonfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
	button* bm=new button("button",popup_loc.x,popup_loc.y,150,100,"a button");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newbuttest0fun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbuttest0 called\n");
	button* bm=new buttest0("buttest0",popup_loc.x,popup_loc.y,150,100,"move right");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newbuttest1fun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbuttest1 called\n");
	button* bm=new buttest1("buttest1",popup_loc.x,popup_loc.y,150,100,"move left");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newlistbox2dfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbuttest1 called\n");
	listbox2d* bm=new listbox2d("listbox2d",popup_loc.x,popup_loc.y,150,175);
	S32 i;
	for (i=0;i<25;++i) {
		C8 s[20];
		sprintf(s,"%d",i*i*i*i*i*i);
		bm->addentry(s);
	}
	uit->linkchild(bm);
	bm->addrem_controls();
	bm->addrem_scroll();
	uitree::setfocus(bm);
}

static void rect_newxsliderfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
	xslider* bm=new xslider("xslider",popup_loc.x,popup_loc.y,250,30,-100,100,0);
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newysliderfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
	yslider* bm=new yslider("yslider",popup_loc.x,popup_loc.y,30,250,-100,100,0);
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newxsliderxfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
/*	xslider* bm=new xsliderx("xsliderx",popup_loc.x,popup_loc.y,250,30,-100,100,0,"xsliderx.uis");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm); */
}

static void rect_newysliderxfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
/*	yslider* bm=new ysliderx("ysliderx",popup_loc.x,popup_loc.y,30,250,-100,100,0,"ysliderx.uis");
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm); */
}

static void rect_newcheckboxfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
	checkbox* bm=new checkbox("checkbox",popup_loc.x,popup_loc.y,200,40,"checkbox",0);
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newradiobuttonfun(uitree* uit,const pointf2& popup_loc)
{
//	logger("newbutton called\n");
	radiobutton* bm=new radiobutton("radiobutton",popup_loc.x,popup_loc.y,200,40,"radiobutton",0);
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_newtreeviewfun(uitree* uit,const pointf2& popup_loc)
{
	treeview* bm=new treeview("treeview",popup_loc.x,popup_loc.y,400,400);
	uit->linkchild(bm);
	bm->addrem_controls();
	bm->addrem_scroll();
	uitree::setfocus(bm);
}

static void rect_newtabsfun(uitree* uit,const pointf2& popup_loc)
{
	tabs* bm=new tabs("tabs",popup_loc.x,popup_loc.y,400,400);
	uit->linkchild(bm);
	bm->addrem_controls();
	uitree::setfocus(bm);
}

static void rect_addremove_controls(uitree* uit,const pointf2& popup_loc)
{
	uitree2drect* uitr=dynamic_cast<uitree2drect*>(uit);
	if (uitr)
		uitr->addrem_controls();
//	uitree::setfocus(uit);
}

static void rect_addremove_scroll(uitree* uit,const pointf2& popup_loc)
{
	uitree2drect* uitr=dynamic_cast<uitree2drect*>(uit);
	if (uitr)
		uitr->addrem_scroll();
}

static void rect_uplevel(uitree* uit,const pointf2& popup_loc)
{
	uitree* uitp=uit->getparent();
	if (!uitp)
		return;
	uitree* uitpp=uitp->getparent();
	if (!uitpp)
		return;
	uitree* uitc=uit->copy();
	uitree2d* uitc2d=dynamic_cast<uitree2d*>(uitc);
	uitree2d* uitp2d=dynamic_cast<uitree2d*>(uitp);
	if (uitc2d && uitp2d) {
		pointf2 o2p1=uitc2d->geto2p();
		pointf2 o2p2=uitp2d->geto2p();
		o2p1.x+=o2p2.x;
		o2p1.y+=o2p2.y;
		uitc2d->seto2p(o2p1);
	}
	uit->dokillc();
	uitpp->linkchild(uitc);
	uitree::setfocus(uitc);
//	logger("uplevel: setting focus to %p\n",cp);
}

static void rect_setdest(uitree* uit,const pointf2& popup_loc)
{
	uitree::setdest(uit);
}

static void rect_cleardest(uitree* uit,const pointf2& popup_loc)
{
	uitree::setdest(0);
}

static void rect_todest(uitree* uit,const pointf2& popup_loc)
{
	uitree* dst=uitree::getdest();
	if (dst) {
		uitree* dstp=dst;
		while(dstp) {
			if (dstp==uit)
				return;
			dstp=dstp->getparent();
		}
//		dstp=dst->getparent();
//		if (!dstp)
//			return;
		uitree* uitc=uit->copy();
		uitree2d* dst2d=dynamic_cast<uitree2d*>(dst);
		uitree2d* uit2d=dynamic_cast<uitree2d*>(uit);
		uitree2d* uitc2d=dynamic_cast<uitree2d*>(uitc);
		if (dst2d && uit2d) {
			pointf2 o2w1=dst2d->geto2w();
			pointf2 o2w2=uit2d->geto2w();
			o2w2.x-=o2w1.x;
			o2w2.y-=o2w1.y;
			uitc2d->seto2p(o2w2);
		} 
		uit->dokillc();
		dst->linkchild(uitc);
		uitree::setfocus(uitc);
//		logger("uplevel: setting focus to %p\n",cp);
	}
}

const popup_info rect_new_class[]={
	{"change name",rect_changenamefun},
//	{"change name2",rect_changename2fun},
	{"load script",rect_loadscriptfun},
	{"save script",rect_savescriptfun},
//	{"load data",rect_loaddatafun},
//	{"save data",rect_savedatafun},
	{"new rect",rect_newrectfun},
	{"new bitmap",rect_newbitmapfun},
//	{"new bitmapname",rect_newbitmapnamefun},
	{"new bitmapscale",rect_newbitmapscalefun},
	{"new texter",rect_newtexterfun},
	{"new editbox",rect_neweditboxfun},
	{"new button",rect_newbuttonfun},
	{"new buttest0",rect_newbuttest0fun},
	{"new buttest1",rect_newbuttest1fun},
	{"new listbox2d",rect_newlistbox2dfun},
	{"new xslider",rect_newxsliderfun},
	{"new yslider",rect_newysliderfun},
	{"new xsliderx",rect_newxsliderxfun},
	{"new ysliderx",rect_newysliderxfun},
	{"new checkbox",rect_newcheckboxfun},
	{"new radiobutton",rect_newradiobuttonfun},
	{"new treeview",rect_newtreeviewfun},
	{"new tabs",rect_newtabsfun},
	{"add/remove controls",rect_addremove_controls},
	{"add/remove scroll",rect_addremove_scroll},
	{"move up one level",rect_uplevel},
	{"set dest",rect_setdest},
	{"clear dest",rect_cleardest},
	{"move to dest",rect_todest},
	{0,0},
};

/// bitmap class
static void bitmap_color_fun(uitree* uit,C32 c)
{
	uitree2dbitmap* uitbm=dynamic_cast<uitree2dbitmap*>(uit);
	if (!uitbm)
		return;
	bitmap32* b=uitbm->getbitmap();
	clipclear32(b,c);
}

static void bitmap_red_fun(uitree* uit,const pointf2& popup_loc)
{
	bitmap_color_fun(uit,C32RED);
}

static void bitmap_green_fun(uitree* uit,const pointf2& popup_loc)
{
	bitmap_color_fun(uit,C32GREEN);
}

static void bitmap_blue_fun(uitree* uit,const pointf2& popup_loc)
{
	bitmap_color_fun(uit,C32BLUE);
}

static void bitmap_load_fun(uitree* uit,const pointf2& popup_loc)
{
	static const C8 *bitmaploadexts[]={"pcx","tga","jpg","dds",0};
	rect_loadsavegfun<loadbitmapdialog>("load bitmap",uit,popup_loc,/*&uitree::loadscript2fn*/false,true,bitmaploadexts);
}

static void bitmap_save_fun(uitree* uit,const pointf2& popup_loc)
{
	static const C8 *bitmapsaveexts[]={"tga",0};
	rect_loadsavegfun<savebitmapdialog>("save bitmap",uit,popup_loc,/*&uitree::loadscript2fn*/true,false,bitmapsaveexts);
//	rect_loadsavegfun("save script",uit,popup_loc,0,0,&uitree::savescript2fn,true,false);
}

const popup_info bitmap_change_color1[]={
	{"red",bitmap_red_fun},
	{"green",bitmap_green_fun},
	{"blue",bitmap_blue_fun},
	{"load bitmap",bitmap_load_fun},
	{"save bitmap",bitmap_save_fun},
	{0,0},
};

/// texter class
static void texter_changename_fun(uitree* uit,const pointf2& popup_loc)
{
//	bitmap_color_fun(uit,C32BLUE);
	rect_changegnamefun<changetextname>("change text name",uit,popup_loc);
}

const popup_info texter_changename[]={
	{"textbox changename",texter_changename_fun},
	{0,0},
};

/// button class
static void button_changename_fun(uitree* uit,const pointf2& popup_loc)
{
//	bitmap_color_fun(uit,C32BLUE);
	rect_changegnamefun<changebuttonname>("change button name",uit,popup_loc);
}

const popup_info button_changename[]={
	{"button changename",button_changename_fun},
	{0,0},
};

/// listbox class
static void listbox2d_addstring_fun(uitree* uit,const pointf2& popup_loc)
{
//	bitmap_color_fun(uit,C32BLUE);
	rect_changegnamefun<listbox2daddentry>("add entry",uit,popup_loc);
}

const popup_info listbox2d_addstring[]={
	{"listbox add string",listbox2d_addstring_fun},
	{0,0},
};

/// slider class
static void slider_changetopval_fun(uitree* uit,const pointf2& popup_loc)
{
	rect_changegnamefun<changeslidertopval>("change top val",uit,popup_loc);
}

static void slider_changebotval_fun(uitree* uit,const pointf2& popup_loc)
{
	rect_changegnamefun<changesliderbotval>("change bot val",uit,popup_loc);
}

const popup_info slider_changeleftrightval[]={
	{"slider changeleftval",slider_changetopval_fun},
	{"slider changerightval",slider_changebotval_fun},
	{0,0},
};

const popup_info slider_changetopbotval[]={
	{"slider changetopval",slider_changetopval_fun},
	{"slider changebotval",slider_changebotval_fun},
	{0,0},
};

