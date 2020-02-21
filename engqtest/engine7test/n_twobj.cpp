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

// base class
class twobj {
	pointf3 pos,rot,vel,rotvel;
public:
//	virtual void draw()=0;
//	virtual void proc()=0;
	virtual void draw()
	{
		logger("in twobj draw\n");
	}
	virtual void proc()
	{
		logger("in twobj proc\n");
	}
};
class twobj *twobjs[MAX_GAMEOBJ];

// friends
void twadd(twobj* a)
{
	for (int i=0;i<MAX_GAMEOBJ;i++)
		if (!twobjs[i]) {
			twobjs[i]=a;
			return;
		}
	delete(a);
}

void twfree(twobj *a)
{
	for (int i=0;i<MAX_GAMEOBJ;i++)
		if (twobjs[i]==a) {
			twobjs[i]=0;
			break;
		}
	delete(a);
}

void drawobjs()
{
	for (int i=0;i<MAX_GAMEOBJ;i++)
		if (twobjs[i])
			twobjs[i]->draw();
}

void procobjs()
{
	for (int i=0;i<MAX_GAMEOBJ;i++)
		if (twobjs[i])
			twobjs[i]->proc();
}

void freeobjs()
{
	for (int i=0;i<MAX_GAMEOBJ;i++)
		if (twobjs[i]) {
			delete(twobjs[i]);
			twobjs[i]=0;
		}
}

////////////// flyobj class /////////////////
class twflyobj : public twobj {
	int fuel;
	float speed;
	float fuser0;
	int user0;
	float dissolve;
//	int kind;
	int hitcar,fromcar;
	int flags;
	void draw()
	{
		logger("in flyobj draw\n");
	}
	void proc()
	{
		logger("in flyobj proc\n");
	}
};

/////////// carobj class //////////////////
// derived classes
class twcarobj : public twobj {
	tree2* carnull,*carbody;
	int seekmode;
	int ownerid; // sorta like which player controls this car
	struct carinfo ci; // what this car carries
//	int slotnum;
//	type of player
	int cntl;
	void draw()
	{
		logger("in carobj draw\n");
	}
	void proc()
	{
		logger("in carobj proc\n");
	}
};

// test this thing
void twobjtest()
{
	twadd(new twobj);
	twadd(new twflyobj);
	twadd(new twcarobj);
	drawobjs();
	freeobjs();
}
