#include <m_eng.h>
#include "json.h"
#include "newcarena/hash.h"
#include "jsonx.h"
////////// test user classes to aid in load and saving c++ structures with json scripts

string pointf3log::log() const
{
	stringstream ss;
	ss << "< " << x << " , " << y << " , " << z << " , " << w << " >";
	return ss.str();
}

pointf3j::pointf3j(const json& js)
{
	x=y=z=0;
	w=1;
	js.get_object_float("x",x);
	js.get_object_float("y",y);
	js.get_object_float("z",z);
	js.get_object_float("w",w);
}
json pointf3j::save() const // save a rectf3log class into a json (sub) script
{
	json ret=json::create_object();
	ret.insert_object("x",json::create_float(x));
	ret.insert_object("y",json::create_float(y));
	ret.insert_object("z",json::create_float(z));
	ret.insert_object("w",json::create_float(w));
	return ret;
}

string pointi2log::log() const
{
	stringstream ss;
	ss << "< " << x << " , " << y << " >";
	return ss.str();
}

pointi2j::pointi2j(json& js)
{
	x=y=0;
	js.get_object_int("x",x);
	js.get_object_int("y",y);
}
json pointi2j::save() // save a rectf3log class into a json (sub) script
{
//	json retn=json::create_object();
	json ret=json::create_object();
	ret.insert_object("x",json::create_int(x));
	ret.insert_object("y",json::create_int(y));
	return ret;
}
modelj::modelj(json& js) : modelb("")
{
}
string rectf3::log() const
{
	stringstream ss;
	ss << "loc: " << topleft.log() << " ";
	ss << "size: " << size.log() << " ";
	ss << "name: {" << name << "} ";
	ss << "priority: {" << pri << "} ";
	ss << "parr: [";
	U32 i,n=parr.size();
	for (i=0;i<n;++i) {
		ss << parr[i].log() << " ";
		if (i!=n-1)
			ss << ',';
		else
			;
	}
	ss << "] ";
	ss << "sarr:[";
	n=sarr.size();
	for (i=0;i<n;++i) {
		ss << "{" << sarr[i] << "} ";
		if (i!=n-1)
			ss << ',';
		else
			;
	}
	ss << "] ";
	ss << "farr:[";
	n=farr.size();
	for (i=0;i<n;++i) {
		ss << "{" << farr[i] << "} ";
		if (i!=n-1)
			ss << ',';
		else
			;
	}
	ss << "] ";
	ss << "iarr:[";
	n=iarr.size();
	for (i=0;i<n;++i) {
		ss << "{" << iarr[i] << "} ";
		if (i!=n-1)
			ss << ',';
		else
			;
	}
	ss << ']';
	return ss.str();
}

rectf3j::rectf3j(json& js) // read a json (sub) script into a rectf3log class
{
//	topleft=pointf3j(js.find_object_object("loc","pointf2"));
//	size=pointf3j(js.find_object_object("size","pointf2"));
	js.get_object_object<pointf3,pointf3j>("loc","pointf2",topleft);
	js.get_object_object<pointf3,pointf3j>("size","pointf2",size);
	js.get_object_string("data",name);
	js.get_object_int("priority",pri);
	js.get_object_array_object<pointf3log,pointf3j>("points","pointf2",parr); // arrays are tighter on types
	js.get_object_array_int("ints",iarr);
	js.get_object_array_float("floats",farr);
	js.get_object_array_string("strings",sarr);
}
json rectf3j::save() // save a rectf3log class into a json (sub) script
{
	json ret=json::create_object();
	ret.insert_object_object<pointf3,pointf3j>("loc","pointf2",topleft);
	ret.insert_object_object<pointf3,pointf3j>("size","pointf2",size);
	ret.insert_object("data",json::create_string(name));
	ret.insert_object("priority",json::create_int(pri));
	ret.insert_array_object<pointf3log,pointf3j>("points","pointf2",parr);
	ret.insert_array_int("ints",iarr);
	ret.insert_array_float("floats",farr);
	ret.insert_array_string("strings",sarr);
	return ret;
}

