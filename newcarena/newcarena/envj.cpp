#include <m_eng.h>
//#include <l_misclibm.h>

#include "utils/json.h"
#include "utils/jsonx.h"
#include "hash.h"
#include "constructor.h"
#include "timewarp.h"
#include "enums.h"
#include "carclass.h"
#include "avaj.h"
#include "physics.h"
#include "envj.h"

calc::calc()
{
// POD
	memset(this,0,sizeof(*this));
}

calcj::calcj() : calc()
{
}

calcj::calcj(const json& js) : calc()
{
	memset(this,0,sizeof(*this)); // POD
	js.get_object_array_float("extrashocklen",extrashocklen,NWORKINGSHOCKS);
	js.get_object_array_float("normalshocklen",normalshocklen,NWORKINGSHOCKS);
	js.get_object_float("shockextra",shockextra);
	js.get_object_array_object<pointf3,pointf3j>("p","pointf3",p,NCARPOINTS);
//	js.get_object_array_object<pointf3,pointf3j>("pr","pointf3",pr,NCARPOINTS);
	js.get_object_float("shockspacingx",shockspacingx);
	js.get_object_float("shockspacingz",shockspacingz);
	js.get_object_float("shockoffsety",shockoffsety);
	js.get_object_object<pointf3,pointf3j>("carbboxmin","pointf3",carbboxmin);
	js.get_object_object<pointf3,pointf3j>("carbboxmax","pointf3",carbboxmax);
//	js.get_object_array_object<pointf3,pointf3j>("c2cpnts","pointf3",c2cpnts,NRECTPOINTS);
//	js.get_object_array_float("shocklen",shocklen,NWORKINGSHOCKS);
//	js.get_object_array_float("wheelvel",wheelvel,NWORKINGSHOCKS);
	js.get_object_float("shockstr",shockstr);
	js.get_object_float("shockdamp",shockdamp);
	js.get_object_float("moi",moi);
	js.get_object_float("mass",mass);
	js.get_object_float("elast",elast);
	js.get_object_float("airfric",airfric);
	js.get_object_float("littleg",littleg);
	js.get_object_float("wheelrad",wheelrad);
	js.get_object_float("wheelaccel",wheelaccel);
//	js.get_object_float("accelspin",accelspin);
	js.get_object_float("fricttireslide",fricttireslide);
	js.get_object_float("fricttireroll",fricttireroll);
	js.get_object_float("frictcarbody",frictcarbody);
	js.get_object_float("drivemode",drivemode);
	js.get_object_float("startaccelspin",startaccelspin);
	js.get_object_float("c2celast",c2celast);
//	js.get_object_float("carid",carid);
	js.get_object_float("defuextraheading",defuextraheading);
	js.get_object_float("maxuextraheading",maxuextraheading);
	js.get_object_float("maxturnspeed",maxturnspeed);
	js.get_object_object<pointf3,pointf3j>("testimpval","pointf3",testimpval);
	js.get_object_object<pointf3,pointf3j>("testimppnt","pointf3",testimppnt);
	js.get_object_int("maxnoshocktime",maxnoshocktime);
	js.get_object_int("maxnocheckpointtime",maxnocheckpointtime);
	js.get_object_int("maxnoresettime",maxnoresettime);
	js.get_object_float("carflipheight",carflipheight);
	js.get_object_float("carstartheight",carstartheight);
	js.get_object_object<pointf3,pointf3j>("camatt","pointf3",camatt);
	js.get_object_float("camattpitch",camattpitch);
	js.get_object_float("camattdist",camattdist);
// new
	js.get_object_float("car2cardist",car2cardist);
	js.get_object_int("candrivetime",candrivetime);
	js.get_object_int("rematchtime",rematchtime);
	js.get_object_int("norematchtime",norematchtime);
	js.get_object_float("sndc2cmag",sndc2cmag);
	js.get_object_float("speedofsound",speedofsound);
}

json calcj::save() const
{
	json ret=json::create_object();
	ret.insert_array_float("extrashocklen",extrashocklen,NWORKINGSHOCKS);
	ret.insert_array_float("normalshocklen",normalshocklen,NWORKINGSHOCKS);
	ret.insert_float("shockextra",shockextra);
	ret.insert_array_object<pointf3,pointf3j>("p","pointf3",p,NCARPOINTS);
//	ret.insert_array_object<pointf3,pointf3j>("pr","pointf3",pr,NCARPOINTS);
	ret.insert_float("shockspacingx",shockspacingx);
	ret.insert_float("shockspacingz",shockspacingz);
	ret.insert_float("shockoffsety",shockoffsety);
	ret.insert_object_object<pointf3,pointf3j>("carbboxmin","pointf3",carbboxmin);
	ret.insert_object_object<pointf3,pointf3j>("carbboxmax","pointf3",carbboxmax);
//	ret.insert_array_object<pointf3,pointf3j>("c2cpnts","pointf3",c2cpnts,NRECTPOINTS);
//	ret.insert_array_float("shocklen",shocklen,NWORKINGSHOCKS);
//	ret.insert_array_float("wheelvel",wheelvel,NWORKINGSHOCKS);
	ret.insert_float("shockstr",shockstr);
	ret.insert_float("shockdamp",shockdamp);
	ret.insert_float("moi",moi);
	ret.insert_float("mass",mass);
	ret.insert_float("elast",elast);
	ret.insert_float("airfric",airfric);
	ret.insert_float("littleg",littleg);
	ret.insert_float("wheelrad",wheelrad);
	ret.insert_float("wheelaccel",wheelaccel);
//	ret.insert_float("accelspin",accelspin);
	ret.insert_float("fricttireslide",fricttireslide);
	ret.insert_float("fricttireroll",fricttireroll);
	ret.insert_float("frictcarbody",frictcarbody);
	ret.insert_float("drivemode",drivemode);
	ret.insert_float("startaccelspin",startaccelspin);
	ret.insert_float("c2celast",c2celast);
//	ret.insert_float("carid",carid);
	ret.insert_float("defuextraheading",defuextraheading);
	ret.insert_float("maxuextraheading",maxuextraheading);
	ret.insert_float("maxturnspeed",maxturnspeed);
	ret.insert_object_object<pointf3,pointf3j>("testimpval","pointf3",testimpval);
	ret.insert_object_object<pointf3,pointf3j>("testimppnt","pointf3",testimppnt);
	ret.insert_int("maxnoshocktime",maxnoshocktime);
	ret.insert_int("maxnocheckpointtime",maxnocheckpointtime);
	ret.insert_int("maxnoresettime",maxnoresettime);
	ret.insert_float("carflipheight",carflipheight);
	ret.insert_float("carstartheight",carstartheight);
	ret.insert_object_object<pointf3,pointf3j>("camatt","pointf3",camatt);
	ret.insert_float("camattpitch",camattpitch);
	ret.insert_float("camattdist",camattdist);
// new
	ret.insert_float("car2cardist",car2cardist);
	ret.insert_int("candrivetime",candrivetime);
	ret.insert_int("rematchtime",rematchtime);
	ret.insert_int("norematchtime",norematchtime);
	ret.insert_float("sndc2cmag",sndc2cmag);
	ret.insert_float("speedofsound",speedofsound);
	return ret;
}

// environment
envj::envj(const json& js) :
		oldtrackj(0),
		newtrackj(0),
		rules(0),
		nlaps(0)
{
	if (!pcnet) {
pushandsetdirdown(getphysicsdir());
		json jsp=json(fileload_string("physics.txt"));
popdir();
		pc = calcj(jsp);
	}
// convert to struct
	js.get_object_string("trackname",trackname);
	js.get_object_int("rules",rules);
	js.get_object_int("nlaps",nlaps);
	js.get_object_object<hashj,hashj>("trackhash","hash",trackhash);
	if (pcnet) {
		js.get_object_object<calc,calcj>("pc","calc",pc); // physics constants
		pcnet = false;
	}
}

bool envj::pcnet = false;

json envj::save() const
{
	json ret=json::create_object();
	ret.insert_string("trackname",trackname);
	ret.insert_int("rules",rules);
	ret.insert_int("nlaps",nlaps);
	ret.insert_object_object<hashj,hashj>("trackhash","hash",trackhash);
	if (pcnet) {
		ret.insert_object_object<calc,calcj>("pc","calc",pc); // physics constants
		pcnet = false;
	}
	return ret;
}

void envj::buildtrackhash()
{
	cleantrack();
pushandsetdirdown(getconstructeddir());
	json js=json(fileload_string((trackname+".trkoc").c_str()));
popdir();
	oldtrackj=new trkt(js);
	trackhash=oldtrackj->gettrackhash();
}

void envj::buildtrackhashtree()
{
	cleantrack();
pushandsetdirdown(getconstructeddir());
	json js=json(fileload_string((trackname+".trkoc").c_str()));
popdir();
	oldtrackj=new trkt(js);
	trackhash=oldtrackj->gettrackhash();
	oldtrackj->buildtrack();
} // fill out old/new trackj, new/change

envj& envj::operator=(const envj& rhs)
{
	if (this!=&rhs) {
		trackname=rhs.trackname;
		trackhash=rhs.trackhash;
		rules=rhs.rules;
		nlaps=rhs.nlaps;
		delete oldtrackj;
		oldtrackj = 0;
		newtrackj = 0;
		pc = rhs.pc;
	}
	return *this;
}
