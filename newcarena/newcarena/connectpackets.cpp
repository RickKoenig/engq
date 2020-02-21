#include <m_eng.h>

#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "timewarp.h"
#include "envj.h"
#include "enums.h"
#include "carclass.h"
#include "avaj.h"
#include "connectpackets.h"

// json packets
// s2c welcome
welcomej::welcomej() :
		numplayers(0),
		yourid(0)
{
}

welcomej::welcomej(const json& js) :
		e(js.find_object_object("environment","envj")),
		numplayers(0),
		yourid(0)
{
	js.get_object_int("numplayers",numplayers);
	js.get_object_int("yourid",yourid);
}

json welcomej::save() const
{
	json ret=json::create_object();
	ret.insert_object_object<envj,envj>("environment","envj",e);
	ret.insert_int("numplayers",numplayers);
	ret.insert_int("yourid",yourid);
	return ret;
}

// c2s packets
hij::hij(const json& js) :
	a(js.find_object_object("avatar","avaj"))
{
	S32 ht=0;
	js.get_object_int("hastrack",ht);
	hastrack = ht!=0;
}

json hij::save() const
{
	json ret=json::create_object();
	ret.insert_object_object<avaj,avaj>("avatar","avaj",a);
	ret.insert_int("hastrack",(int)hastrack);
	return ret;
}
