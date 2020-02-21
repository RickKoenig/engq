#include <m_eng.h>
#include "json.h"
#include "newcarena/hash.h"
#include "jsonx.h"

// static
json json::nulljson; // for failed finds
const C8* json::dp; // for printing (indentation counter)
const C8* json::typenames[]={"string","number","object","array","true","false","null"}; // names of types

bool json::is_term(C8 c)
{
	if (isspace(c))
		return true;
	if (c==',')
		return true;
	if (c=='}')
		return true;
	if (c==']')
		return true;
	if (c=='\0')
		return true;
	return false;
}

void json::load_value(const C8* data) // non zero data is top level
{
	number=0.0f;
	if (data) {
		dp=data;
#ifdef VERBOSE
		logger("json loadvalue '%s'\n",dp);
#endif
	}
// skip whitespace
	skip_whitespace();
// EOF
	if (!*dp) {
//		errorexit("loadvalue: EOF in json loadvalue, no data");
		jt=J_NULL;
		return;
	}
#ifdef VERBOSE
	if (*dp=='"')
		logger("first char is '\\%c'\n",*dp);
	else
		logger("first char is '%c'\n",*dp);
#endif
	if (*dp>='0' && *dp<='9')
		parse_number();
	else {
		switch(*dp) {
			case '"':
				++dp; // skip quote
				parse_string();
				break;
			case '-':
				parse_number();
				break;
			case '.':
				parse_number();
				break;
			case '{': // skip open curly brace
				++dp;
				parse_object();
				break;
			case '[':
				++dp; // skip open bracket
				parse_array();
				break;
			case 't':
				parse_true();
				break;
			case 'f':
				parse_false();
				break;
			case 'n':
				parse_null();
				break;
			default:
				{
					string tok=get_rest();
					errorexit("loadvalue: don't know how to parse '%s'",tok.c_str());
				}
				break;
		}
	}
// skip whitespace
	skip_whitespace();
	if (*dp && data) {
		string tok=get_rest();
		errorexit("EOF in json loadvalue, extra data '%s'",tok.c_str());
//		return;
	}
}

void json::skip_whitespace() const
{
	while(*dp) {
		if (isspace(*dp)) {
			++dp;
		} else {
			break;
		}
	}
}

string json::get_token() const
{
	const C8* dpm=dp;
	while(true) {
		if (is_term(*dpm))
			break;
		++dpm;
	}
	string ns(dp,dpm-dp);
	dp=dpm;
	return ns;
}

string json::get_rest() const // for error message
{
	const C8* dpm=dp;
	while(true) {
		if (!*dpm)
			break;
		if (isspace(*dpm))
			break;
		++dpm;
	}
	string ns(dp,dpm-dp);
	dp=dpm;
	return ns;
}

void json::parse_string() // already past the
{
	jt=J_STRING;
	while(true) {
		if (*dp=='"') { // close quote
			++dp;
			break;
		} else if (*dp=='\\') { // special
			++dp;
			switch(*dp) {
			case '\"': // "
				++dp;
				str+='"';
				break;
			case '\\': // '\'
				++dp;
				str+='\\';
				break;
			case '/':  // /
				++dp;
				str+='/';
				break;
			case 'b':  // bs
				++dp;
				str+='\b';
				break;
			case 'f':  // ff
				++dp;
				str+='\f';
				break;
			case 'n':  // nl
				++dp;
				str+='\n';
				break;
			case 'r':  // cr
				++dp;
				str+='\r';
				break;
			case 't':  // tab
				++dp;
				str+='\t';
				break;
			case 'u':  // skip over hex
				++dp;
				if (!dp[0] || !dp[1] || !dp[2] || !dp[3]) {
#ifdef VERBOSE
					logger("\""); // keep script happy, not perfect
#endif
					errorexit("parsestring: EOF found in \\u");
				}
				dp+=4;
				break;
			default:
				errorexit("parsestring: unknown '\\%c'",*dp);
				break;
			}
		} else if (!*dp) { // null char
#ifdef VERBOSE
			logger("\""); // keep script happy, not perfect
#endif
			errorexit("parsestring: EOF");
		} else if (iscntrl(*dp)) { // unwanted control character
#ifdef VERBOSE
			logger("\""); // keep script happy, not perfect
#endif
			errorexit("parsestring: unwanted control character found '%d'",*dp);
		} else { // normal character
			str+=*dp++;
		}
	}
}

void json::parse_number()
{
	jt=J_NUMBER;
	string numstr=get_token();
#ifdef VERBOSE
	logger("number to convert is '%s'\n",numstr.c_str());
#endif
	number=str2float(numstr.c_str());
}

void json::parse_object()
{
	jt=J_OBJECT;
	skip_whitespace();
	if (*dp=='}') { // empty object
		++dp;
		return;
	} else if (*dp==',') {
		errorexit("parseobject: early ','");
	} else if (*dp==',') {
		errorexit("parseobject: early ':'");
	}
	while(true) {
		json key(0); // get an element key string
		if (key.jt!=J_STRING)
			errorexit("parseobject: key is not a string");
		skip_whitespace();
		if (*dp!=':') // :
			errorexit("parseobject: missing ':'");
		++dp;
		json val(0); // get an element value
		object[key.str]=val;
		skip_whitespace();
		if (*dp=='}') { // done object
			++dp;
			break;
		} else if (*dp!=',') {
			if (!*dp)
				errorexit("parseobject: EOF");
			else
				errorexit("parseobject: missing ','");
		}
		++dp;
		skip_whitespace();
	}
}

void json::parse_array()
{
	jt=J_ARRAY;
	skip_whitespace();
	if (*dp==']') { // empty array
		++dp;
		return;
	} else if (*dp==',') {
		errorexit("parsearray: early ','");
	}
	while(true) {
		json ele(0); // get an element value
		array.push_back(ele);
		skip_whitespace();
		if (*dp==']') { // done array
			++dp;
			break;
		} else if (*dp!=',') {
			if (!*dp)
				errorexit("parsearray: EOF");
			else
				errorexit("parsearray: missing ','");
		}
		++dp;
		skip_whitespace();
	}
}

void json::parse_true()
{
	jt=J_TRUE;
	string truestr=get_token();
#ifdef VERBOSE
	logger("true to convert is '%s'\n",truestr.c_str());
#endif
	if (truestr!="true")
		errorexit("parsetrue: not 'true'");
}

void json::parse_false()
{
	jt=J_FALSE;
	string falsestr=get_token();
#ifdef VERBOSE
	logger("false to convert is '%s'\n",falsestr.c_str());
#endif
	if (falsestr!="false")
		errorexit("parsefalse: not 'false'");
}

void json::parse_null()
{
	jt=J_NULL;
	string nullstr=get_token();
#ifdef VERBOSE
	logger("null to convert is '%s'\n",nullstr.c_str());
#endif
	if (nullstr!="null")
		errorexit("parsenull: not 'null'");
}

string json::str_to_backslash_str(string rs) const // convert to string with '\' commands and quotes
{
	string ret("\"");
	U32 i,n=rs.size();
	for (i=0;i<n;++i) {
		C8 c=rs.at(i);
		if (c=='"') { // quote
				ret+="\\\"";
		} else if (c=='\\') { // backslash
				ret+="\\\\";
		} else if (iscntrl(c)) {
			switch(c) {
			case '\b': // backspace
				ret+="\\b";
				break;
			case '\f': // form feed
				ret+="\\f";
				break;
			case '\n': // new line
				ret+="\\n";
				break;
			case '\r': // carriage return
				ret+="\\r";
				break;
			case '\t': // tab
				ret+="\\t";
				break;
			default:
				errorexit("str_to_backslash_str: don't know how to convert code %d to backslash command",c);
				break;
			}
		} else {
			ret+=c;
		}
	}
	ret+='"';
	return ret;
}

json::json(const C8* data) : jt(J_NULL)
{
	load_value(data);
}

json::json(string data) : jt(J_NULL)
{
	load_value(data.c_str());
}

string json::save(bool rightofcolon) const
{
	static U32 indent;
	string ret;
	S32 i,n;
	objele::const_iterator it,end;
	bool isempty;
	if (!rightofcolon)
		ret+=string(indent,'\t');
	switch(jt) {
		case J_STRING:
			ret+=str_to_backslash_str(str);
			break;
		case J_NUMBER:
			ret+=float2str(number);
			break;
		case J_OBJECT:
			ret+='{';
			// Iterate over the map and print out all key/value pairs.
			// Using a const_iterator since we are not going to change the values.
			it = object.begin();
			end = object.end();
			isempty = it==end;
			if (!isempty) {
				ret+="\r\n";
			}
			++indent;
			while(it != end) {
				ret+=string(indent,'\t');
				ret+=str_to_backslash_str(it->first);
				ret+=" : ";
				ret+=it->second.save(true);
				++it;
				if (it!=end)
					ret+=',';
				ret+="\r\n";
			}
			--indent;
			if (!isempty)
				ret+=string(indent,'\t');
			ret+='}';
			break;
		case J_ARRAY:
			ret+='[';
			n=array.size();
			if (n)
				ret+="\r\n";
			++indent;
			for (i=0;i<n;++i) {
//				ret+=string(indent,'\t');
				ret+=array[i].save();
				if (i!=n-1)
					ret+=',';
				ret+="\r\n";
			}
			--indent;
			if (n)
				ret+=string(indent,'\t');
			ret+=']';
			break;
		case J_TRUE:
			ret+="true";
			break;
		case J_FALSE:
			ret+="false";
			break;
		case J_NULL:
			ret+="null";
			break;
		default:
			errorexit("save: bad type %d",jt);
			break;
	}
//	if (!indent)
//		ret+="\r\n";
	return ret;
}

void json::save(const C8* fname) const
{
	string s=save();
	filesave_string(fname,s);
}

U32 json::size() const
{
	switch(jt) {
	case J_NUMBER:
	case J_STRING:
	case J_TRUE:
	case J_FALSE:
	case J_NULL:
		return 0;
		break;
	case J_OBJECT:
		return object.size();
		break;
	case J_ARRAY:
		return array.size();
		break;
	default:
		errorexit("size: bad type %d",jt);
		return 0;
		break;
	}
}

// find
const json& json::find_object(string objname) const
{
	objele::const_iterator it=object.find(objname);
	if (it==object.end())
		return nulljson;
	return it->second;
}

const json& json::find_array(S32 idx) const // -1 returns last
{
	S32 n=array.size();
	if (!n)
		return nulljson;
	if (idx==-1)
		return nulljson;
	if (idx>=n || idx<0)
		return nulljson;
	return array[idx];
}

// add
void json::insert_object(string objname,json j)
{
	if (jt==J_OBJECT)
		object[objname]=j;
}

void json::insert_array(json j,S32 idx) // -1 adds to end
{
	if (jt==J_ARRAY) {
		if (idx==-1)
			array.push_back(j);
		else
			array.insert(array.begin()+idx,j);
	}
}

// remove
void json::erase_object(string objname)
{
	object.erase(objname);
}

void json::erase_array(S32 idx) // -1 removes last
{
	if (idx==-1)
		array.pop_back();
	else
		array.erase(array.begin()+idx);
}

// convert json to c++ class members, json struct is like {"name": {"type": {"name" etc... {"mathutil": {"constants": {"pi": 3.14}}}
bool json::get_object_float(string name,float& fr) const
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_NUMBER) {
		fr=jm.get_float();
		return true;
	} else
		return false;
}

// for now, range of -16777216 to 16777216 around 16 million, maybe change later
bool json::get_object_int(string name,S32& ir) const
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_NUMBER) {
		float f=jm.get_float();
		S32 i;
		F2INT(f,i);
		ir=i;
		return true;
	} else
//		ir=0;
		return false;
}

bool json::get_object_string(string name,string& s) const
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_STRING) {
		s=jm.get_string();
		return true;
	} else
		return false;
}

const json& json::find_object_object(string objname,string objtype) const
{
	return find_object(objname).find_object(objtype);
}

// dump a byte array in pretty decimal (todo: use more templates!)
bool json::get_object_array_U8(string name,U8* vs,U32 arrsize) const // construct U8 array from name
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		if (n>arrsize)
			n=arrsize;
		fill(vs,vs+arrsize,0U);
		for (i=0;i<n;++i) {
			U8 e = jm.find_array(i).get_int();
			vs[i]=e;
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_int(string name,S32* vs,U32 arrsize) const // construct int array from name
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		if (n>arrsize)
			n=arrsize;
		fill(vs,vs+arrsize,0U);
		for (i=0;i<n;++i) {
			U8 e = jm.find_array(i).get_int();
			vs[i]=e;
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_float(string name,float* vs,U32 arrsize) const // construct float array from name
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		if (n>arrsize)
			n=arrsize;
		fill(vs,vs+arrsize,0.0f);
		for (i=0;i<n;++i) {
			float e = jm.find_array(i).get_float();
			vs[i]=e;
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_string(string name,string* vs,U32 arrsize) const // construct string array from name
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		if (n>arrsize)
			n=arrsize;
		fill(vs,vs+arrsize,string());
		for (i=0;i<n;++i) {
			string e = jm.find_array(i).get_string();
			vs[i]=e;
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_U8(string name,vector<U8>& vs) const
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		vs.clear();
		for (i=0;i<n;++i) {
			U32 e = jm.find_array(i).get_int();
			vs.push_back(e);
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_int(string name,vector<S32>& vs) const
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		vs.clear();
		for (i=0;i<n;++i) {
			S32 e = jm.find_array(i).get_int();
			vs.push_back(e);
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_float(string name,vector<float>& vs) const
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		vs.clear();
		for (i=0;i<n;++i) {
			float e = jm.find_array(i).get_float();
			vs.push_back(e);
		}
		return true;
	} else
		return false;
}

bool json::get_object_array_string(string name,vector<string>& vs) const 
{
	const json& jm=find_object(name);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		vs.clear();
		for (i=0;i<n;++i) {
			string e = jm.find_array(i).get_string();
			vs.push_back(e);
		}
		return true;
	} else
		return false;
}

void json::insert_int(string name,S32 i)// add an int to json object
{
	insert_object(name,json::create_int(i));
}

void json::insert_float(string name,float f) // add a float to json object
{
	insert_object(name,json::create_float(f));
}

void json::insert_string(string name,string s) // add a string to json object
{
	insert_object(name,json::create_string(s));
}
#if 0
template <typename B,typename D> // construct basetype array from name and nameoftype
void json::insert_object_object(string name,string nametype,D d) // add a class/struct to json object
{
	json ins = create_object();
	json ins2 = d.save();
	ins.insert_object(nametype,ins2);
	insert_object(name,ins);
}
#endif
void json::insert_array_U8(string name,const U8* b,U32 arrsize) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=arrsize;
	for (i=0;i<n;++i) {
		ins.insert_array(create_int(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_U8(string name,const vector<U8>& b) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=b.size();
	for (i=0;i<n;++i) {
		ins.insert_array(create_int(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_int(string name,const S32* b, U32 arrsize) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=arrsize;
	for (i=0;i<n;++i) {
		ins.insert_array(create_int(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_int(string name,const vector<S32>& b) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=b.size();
	for (i=0;i<n;++i) {
		ins.insert_array(create_int(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_float(string name,const float* b,U32 arrsize) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=arrsize;
	for (i=0;i<n;++i) {
		ins.insert_array(create_float(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_float(string name,const vector<float>& b) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=b.size();
	for (i=0;i<n;++i) {
		ins.insert_array(create_float(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_string(string name,const string* b,U32 arrsize) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=arrsize;
	for (i=0;i<n;++i) {
		ins.insert_array(create_string(b[i]));
	}
	insert_object(name,ins);
}

void json::insert_array_string(string name,const vector<string>& b) // add a class/struct to json object
{
	json ins = create_array();
	U32 i,n=b.size();
	for (i=0;i<n;++i) {
		ins.insert_array(create_string(b[i]));
	}
	insert_object(name,ins);
}

// done classes, main json test center
void json_test()
{
/*	int ii=3,jj=5;
	int& r=ii;
	r=jj; */
	pushandsetdir("jsontest");
// straight binary copy
	const C8 file1[]="testres1.txt";
	const C8 file2[]="testres3.txt";
	C8* jdata=fileload(file1);
	filesave("test2.txt",jdata);
	delete[] jdata;
// json original
	json js(fileload_string(file1));
// json copy
	json js2=js;
// json manipulation
	const json& js3=js2.find_object("listofpoints");
	const json& js4=js3.find_array(1);
	json js5=js4.find_object("pointf2");
	js5.insert_object("z",json::create_string("zee"));
// json save
	string js5savestr=js5.save();
	logger("js5savestr = '%s'\n",js5savestr.c_str());
	if (js2.get_type()==json::J_OBJECT)
		js2.insert_object("aaa",json::create_object());
	js2.save(file2);
// json pointf3 from file
	const json& jr=js.find_object("p0");
	pointf3j jp(jr);
	logger("p0 '%s'\n",jp.log().c_str());
// json pointf3 from scratch
	json j("{ \"x\" : 16777217 , \"y\" : 16777216, \"z\" :16777215 }");
	jp=pointf3j(j);
	logger("p0 '%s'\n",jp.log().c_str());
// json pointf3 from scratch
	j=json("{ \"x\" : 16777214 , \"y\" : 16777213, \"z\" :16777212 }");
	jp=pointf3j(j);
	logger("p0 '%s'\n",jp.log().c_str());
// json rect from textres1.txt
#if 1
	j=js.find_object_object("atext","text");
	string jsavestr=j.save();
	logger(" atext.text '%s'\n",jsavestr.c_str());
	rectf3j rj(j);
#else
	json jj=json(fileload_string("testrjsave.txt"));
	rectf3j rj(jj);
#endif
	logger("rectj log '%s'\n",rj.log().c_str());
	const json& rjs=rj.save();
	rjs.save("testrjsave.txt");

// compare the two files
	C8* f1=fileload(file1);
	C8* f2=fileload(file2);
	U32 i=0;
	while(true) {
		if (f1[i]=='\0' && f2[i]=='\0') {
			logger("files are the same\n");
			break;
		}
		if (f1[i]=='\0') {
			logger("file1 is shorter\n");
			break;
		}
		if (f2[i]=='\0') {
			logger("file2 is shorter\n");
			break;
		}
		if (f1[i]!=f2[i]) {
			C8 s1[3],s2[3];
			if (f1[i]=='"') { // make logfile.txt script read happy, change " to \"
				s1[0]='\\';
				s1[1]='"';
				s1[2]='\0';
			} else {
				s1[0]=f1[i];
				s1[1]='\0';
			}
			if (f2[i]=='"') {
				s2[0]='\\';
				s2[1]='"';
				s2[2]='\0';
			} else {
				s2[0]=f2[i];
				s2[1]='\0';
			}
			logger("files are different at the %d character, c1 = %d '%s', c2 = %d '%s'\n",
			  i,f1[i],s1,f2[i],s2);
			break;
		}
		++i;
	}
	delete[] f1;
	delete[] f2;
	popdir();
}


#if 0
// the seven json types (value)
"a string with \" \\ \/ \b \f \n \r \t x\u89Abx"
.142857
{"pi":3.14,"e":2.72}
[1,1,2,3,5,8,13,21,34]
true
false
null

// first char of a value
quote
0123456789.-
{
[
t
f
n
#endif
