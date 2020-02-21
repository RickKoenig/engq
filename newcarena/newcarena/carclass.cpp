#define D2_3D
#include <m_eng.h>
#include <d3d9.h>

#include "enums.h"
#include "carclass.h"
#include "jrmcars.h"

// handles the car
n_carclass::n_carclass(const C8* carname,S32 pnt,S32 dec) : cartree(0)
{
	int i;
pushandsetdir("newcarenadata/cardata");
	C8 hasfile[500];
	sprintf(hasfile,"%s.jrm",carname);
	tree2* carbody=0;
	if (fileexist(hasfile)) {
		carbody=loadnewjrms2(carname,pnt,dec);
		S32 hideregpointsv=1;
//		if (hideregpointsv)
//			hideregpoints(carbody);
	} else {
		logger("loading bws car '%s'\n",carname);
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

script* n_carclass::getcarlist()
{
pushandsetdirdown("cardata");
	script* sc2=new script;
#if 0
	sc2->addscript("muscletone");
popdir();
	return sc2;
#endif
	script* sc=new scriptdir(0);
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
