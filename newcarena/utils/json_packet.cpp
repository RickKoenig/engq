#include <m_eng.h>
#include <l_misclibm.h>

#include "json.h"
#include "json_packet.h"
//#include "newcarena/n_packetcpp.h"

// send a json to a socket
void writejsontoken(socker* s,S32 tokid,const json& js)
{
	string sv = js.save();
	tcpwritetoken32(s,tokid,sv.c_str(),sv.size()+1); // include '\0'
}

// get a json from a cstr
json readjsontoken(const C8* tstr,S32 len)
{
	if (len<=0 || tstr[len-1]!=0)
		errorexit("bad readjsontoken");
	return json(tstr);
}
