#include <iomanip>

#include <m_eng.h>

#include "md5.h"
#include "utils/json.h"
#include "hash.h"

static MD_CTX context; // not thread safe, 1 md5 context

hashi::hashi(unsigned char *trk,int len)
{
	int chunklen;
	MDInit (&context);
	while (len) {
		chunklen = len>=1024 ? 1024 : len;
		MDUpdate (&context, trk, chunklen);
		trk+=chunklen;
		len-=chunklen;
	}
	MDFinal (data, &context);
}

hashi::hashi()
{
	MDInit (&context);
	fill(data,data+NUMHASHBYTES,0);
}

void hashi::add(unsigned char *trk,int len) const
{
	int chunklen;
	while (len) {
		chunklen = len>=1024 ? 1024 : len;
		MDUpdate (&context, trk, chunklen);
		trk+=chunklen;
		len-=chunklen;
	}
}

void hashi::final()
{
	MDFinal (data, &context);
}

string hashi::dump() const
{
	stringstream ss;
	S32 i;
	for (i=0;i<NUMHASHBYTES;++i) {
		ss << hex << uppercase << setfill('0');
		ss.width(2);
		ss << U32(data[i]) << " ";
	}
	return ss.str();
}

bool hashi::operator==(const hashi& rhs)
{
	return equal(data,data+NUMHASHBYTES,rhs.data);
}

bool hashi::operator!=(const hashi& rhs)
{
	return !equal(data,data+NUMHASHBYTES,rhs.data);
}

// hashj
hashj::hashj(const json& js)
{
	js.get_object_array_U8("data",data,hashi::NUMHASHBYTES);
}

json hashj::save() const
{
	json ret=json::create_object();
	ret.insert_array_U8("data",data,hashi::NUMHASHBYTES);
	return ret;
}
