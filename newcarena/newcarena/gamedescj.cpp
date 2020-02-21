#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "constructor.h"
#include "carclass.h"

#include "timewarp.h"
#include "netdescj.h"
#include "envj.h"
#include "avaj.h"
#include "gamedescj.h"

// json stuff
// whole game from lobby
gamedescj::gamedescj(const json& js) :
		n(js.find_object_object("net","netdescj")),
		e(js.find_object_object("environment","envj")),
		a(js.find_object_object("avatar","avaj")),
		nhumanplayers(1),
		nbotplayers(0)
{
	js.get_object_int("nhumanplayers",nhumanplayers);
	js.get_object_int("nbotplayers",nbotplayers);
}

json gamedescj::save() const
{
	json ret=json::create_object();
	ret.insert_object_object<netdescj,netdescj>("net","netdescj",n);
	ret.insert_object_object<envj,envj>("environment","envj",e);
	ret.insert_object_object<avaj,avaj>("avatar","avaj",a);
	ret.insert_int("nhumanplayers",nhumanplayers);
	ret.insert_int("nbotplayers",nbotplayers);
	return ret;
}

