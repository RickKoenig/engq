#include <m_eng.h>
#include <l_misclibm.h>

#include "utils/json.h"
#include "netdescj.h"

// net
netdescj::netdescj(const json& js) :
		gamenet(0),
		serverip(0),
		serverport(0)
{
	js.get_object_int("gamenet",gamenet);
	string ipstr;
	js.get_object_string("serverip",ipstr);
	serverip=str2ip(ipstr.c_str());
	js.get_object_int("serverport",serverport);
}

json netdescj::save() const
{
	json ret=json::create_object();
	ret.insert_int("gamenet",gamenet);
	string ipstr=ip2str(serverip);
	ret.insert_string("serverip",ipstr);
	ret.insert_int("serverport",serverport);
	return ret;
}
