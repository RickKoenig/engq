#define D2_3D
#include <m_eng.h>
#include <d3d9.h>

//#include "gameinfo.h"
//#include "../u_modelutil.h"
//#include "../u_s_toon.h"
//#include "m_vid_dx9.h"

#include "n_usefulcpp.h"
#include "n_carclass.h"
#include "n_jrmcarscpp.h"

static void setjrmcar()
{
	jrmcarscale= .1f; // .014f; was
	jrmcarrot.x= -90;
	jrmcarrot.y= 0;
	jrmcarrot.z= 0;
	jrmcartrans.x= 0;
	jrmcartrans.y= 0;//-.07f;
	jrmcartrans.z= 0;
}

// handles the car
n_carclass::n_carclass(const C8* carname) : cartree(0)
{
	int i,pnt=0,dec=0;
//	listbox* lb;
//	p=findlistboxname(rl,LISTCARBODY,i);
//	i=getcurlistbox(rl,LISTCARBODY);
//	if (i<0)
//		errorexit("negative car index!");
	pushandsetdir("engine7testdata/cardata");
	C8 hasfile[500];
	sprintf(hasfile,"%s.jrm",carname);
	tree2* carbody=0;
	if (fileexist(hasfile)) {
		carbody=loadnewjrms2(carname,pnt,dec);
		S32 hideregpointsv=1;
		if (hideregpointsv)
			hideregpoints(carbody);
	} else {
		sprintf(hasfile,"%s.bws",carname);
		if (fileexist(hasfile)) {
			carbody=new tree2(hasfile);
		} else {
			carbody=new tree2("acarnull");
		}
	}
	for (i=0;i<MAX_WHEELS;++i) {
		wheels[i]=findtreestrstrrec(carbody,wheel_names[i]);
	}
	popdir();
	cartree=new tree2("n_carclass");
	cartree->linkchild(carbody);
}

n_carclass::~n_carclass() // make sure this gets called 'before' delete roottree...
{
	delete cartree;
}

/*static tree2 *findawheel(tree2 *t,char *tirename)
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
};*/

script* n_carclass::getcarlist()
{
pushandsetdirdown("cardata");
	script* sc=new scriptdir(0);
	script* sc2=new script;
popdir();
	int i;
	for (i=0;i<sc->num();i++) {
		char name[50];
		if (isfileext(sc->idx(i).c_str(),"jrm")) {
			mgetname(sc->idx(i).c_str(),name);
			sc2->addscript(name);
		}
		if (isfileext(sc->idx(i).c_str(),"bws")) {
			mgetname(sc->idx(i).c_str(),name);
			sc2->addscript(name);
		}
	}
	delete sc;
	sc2->sort();
	return sc2;
}
