#include <m_eng.h>

#include "u_states.h"

#include "u_factory.h"
#include "u_uipopup_info.h"
#include "u_uiclass.h"
/// aye, some globals

namespace u_s_ui {
uitree* auitree;
uitree* clonet;
//bool dosave;
} /// end namespace u_s_ui
using namespace u_s_ui;

/// test pointer to member functions
/*class Fred
{
protected:
	S32 v;
public:
	Fred(S32 va) : v(va) {}
	 void fun1()
	{
		logger("in fun 1 %d\n",v);
	}
	virtual void fun2()
	{
		logger("in fun 2 %d\n",v);
	}
};

class Freddy : public Fred
{
public:
	Freddy(S32 va) : Fred(va) {}
	void fun1()
	{
		logger("in derived fun 1 %d\n",v);
	}
	void fun2()
	{
		logger("in derived fun 2 %d\n",v);
	}
	void fun3()
	{
		logger("in derived fun 3 %d\n",v);
	}
};

void testpmf()
{
	class Fred fr1(11);
	class Freddy fr2(22);
	void (Fred::*fp1)() =&Fred::fun1;
	void (Fred::*fp2)() =&Fred::fun2;
	void (Freddy::*fp3)() =&Freddy::fun3;
//	void (Fred::*fp3f)() =fp3;
	(fr1.*fp1)();
	(fr1.*fp2)();
	(fr2.*fp1)();
	(fr2.*fp2)();
//	(fr2.*fp3)();
}
/// end test pointer to member functions
*/
void ui_init()
{
//	testpmf();
	video_setupwindow(1024,768);
	pushandsetdir("ui");
/*	S32 num;
	string res;
	res=getstringnum("abcdefg123",num);
	logger("getstringnum '%s' '%d'\n",res.c_str(),num);
	res=getstringnum("ab12345678",num);
	logger("getstringnum '%s' '%d'\n",res.c_str(),num);
	res=getstringnum("abcdefg",num);
	logger("getstringnum '%s' '%d'\n",res.c_str(),num);
	res=getstringnum("1234567",num);
	logger("getstringnum '%s' '%d'\n",res.c_str(),num);
	res=getstringnum("",num);
	logger("getstringnum '%s' '%d'\n",res.c_str(),num); */
/*	logger("wininfo.datadir is '%s'\n",wininfo.datadir);
	logger("curdir is '%s'\n",getcurdir());
	string ad;
	ad="c:\\boo\\hoo";
	logger("abs2rel '%s' is '%s'\n",ad.c_str(),absdir2reldirdisplay(ad).c_str());
	ad="c:\\srcw32\\engq\\engqtest\\data\\config";
	logger("abs2rel '%s' is '%s'\n",ad.c_str(),absdir2reldirdisplay(ad).c_str());
	ad="c:\\mingwstudio\\samples\\engq\\engqtest\\data\\config";
	logger("abs2rel '%s' is '%s'\n",ad.c_str(),absdir2reldirdisplay(ad).c_str());
	ad="c:\\srcw32";
	logger("abs2rel '%s' is '%s'\n",ad.c_str(),absdir2reldirdisplay(ad).c_str());
	string rd;
	rd="howdy";
	logger("rel2abs '%s' is '%s'\n",rd.c_str(),reldir2absdir(rd).c_str());
	rd="config";
	logger("rel2abs '%s' is '%s'\n",rd.c_str(),reldir2absdir(rd).c_str());
	rd="c:\\srcw32";
	logger("rel2abs '%s' is '%s'\n",rd.c_str(),reldir2absdir(rd).c_str());
	rd="gfxtest";
	logger("rel2abs '%s' is '%s'\n",rd.c_str(),reldir2absdir(rd).c_str());
*/
//	dosave=false;
	// default script
#define SCRIPT
//#define ONEOBJ
//#define HIER

#ifdef SCRIPT
//	script ns=script("testnewsave.uis");
//	pushandsetdir("ui");
	script ns=script("default2.uis");
	//script ns=script("testhier.uis");
	//script ns=script("overlap.uis");
//	popdir();
	factory<uitree,script> fs; // these 2 lines, this is where it all comes together
	auitree=fs.newclass_from_handle(ns);
#endif

#ifdef ONEOBJ
	// single object
	auitree=new uitree2drect("a uitree",10,20,100,200); // very simple
#endif

#ifdef HIER
	// simple hierarchy
	auitree = new uitree("root uitree"); // very simple
#if 1
	uitree2d* par = new uitree2d("parent uitree2d",400,120); // very simple
	auitree->linkchild(par);
	uitree* chld0 = new uitree2drect("chld0",10,20,100,200); // very simple
	uitree* chld1 = new uitree2drect("chld1",210,20,100,250); // very simple
	par->linkchild(chld0);
	par->linkchild(chld1);
	uitree2d* parc = par->copy();
	parc->seto2p(pointf2x(20,40));
	auitree->linkchild(parc);
	//delete par;
	//delete parc;
	//auitree->buildo2w();
#endif
#endif

	auitree->setfocus(0);
/*
	uitree2drect* mov1=auitree->find<uitree2drect>("mov1");
	logger("find mov1 %08x\n",mov1);
	logger("find mov2 %08x\n",auitree->find("mov2"));
	logger("find mov3 %08x\n",auitree->find("mov3"));
	logger("find mov4 %08x\n",auitree->find("mov4"));
	logger("find mov8 %08x\n",auitree->find("mov8",1));
*/
/*	script ns2=script("testscript.uis");
	S32 i,n=ns2.num();
	for (i=0;i<n;++i)
		logger("script %3d is \"%s\"\n",i,ns2.printidx(i).c_str()); */
}

void ui_proc()
{
/// input
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	auitree->setinput(); // reads MX,MY converts to float fmxy, and sets usedinput to false
/// proc
	auitree->proc();
	auitree->killdeaddudes();
	auitree->buildo2w();
	auitree->focustofront(); // this will alter order in hierarchy, never call this from a class member function
//	if (KEY=='s') {
//		dosave=true;
//		popstate();
//	}
}

void ui_draw2d()
{
/// draw
//	video_lock();
	clipclear32(B32,C32LIGHTBLUE);
	auitree->draw();		/// draw everything but below
	auitree->mouseover();	/// show window that mouse is over and handle right click menu
	auitree->showfocus();	/// show current window in focus (if any)
	auitree->showdest();	/// show destination for hiearchy control
//	outtextxybf32(B32,0,0,C32WHITE,C32BLACK,"popups %d",uitree::get_num_popup());
//	outtextxybf32(B32,0,0,C32WHITE,C32BLACK,"focus %p",uitree::getfocus());
	outtextxybf32(B32,0,0,C32WHITE,C32BLACK,"curdir '%s'",getcurdir());
//	video_unlock();
}

void ui_exit()
{
/// log the tree
//	auitree->log();
/// make a copy of the tree
	clonet=auitree->copy();
/// if 's' pressed, save the copy
//	if (dosave) {
//		FILE* fw=fopen2("testnewsave.uis","w");
//		clonet->save(fw);
//		fclose(fw);
//	}
/// mark roots for deletion
	auitree->dokillc();
	clonet->dokillc();
/// delete marked
	auitree->killdeaddudes();
	clonet->killdeaddudes();
	auitree=clonet=0;
	popdir();
}
