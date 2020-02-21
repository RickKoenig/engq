void json_test();

//#define VERBOSE
// jason type
class json {
	static json nulljson;
public:
	enum jtype {J_STRING,J_NUMBER,J_OBJECT,J_ARRAY,J_TRUE,J_FALSE,J_NULL};
private:
// data
	static const C8* dp; // for loading, maybe get rid of static somehow...
	static const C8* typenames[];
	jtype jt;
	string str;
	float number; // also good for all ints in range -16777216 to 16777216, maybe add an 'int' type, we'll see
	typedef map<string,json> objele;
    objele object;
	vector<json> array;
// private functions
	static bool is_term(C8 c); // used for parsing, char is whitespace or ,}] etc.
	void load_value(const C8* data); // main parser, data!=0 means data=dp, data=0 means use dp
	void skip_whitespace() const;
	string get_token() const;
	string get_rest() const; // for errors
	void parse_string();
	void parse_number();
	void parse_object();
	void parse_array();
	void parse_true();
	void parse_false();
	void parse_null();
	string str_to_backslash_str(string rs) const; // convert raw string to string with '\' commands, for quote cr lf etc.
public:
// read
	json(){jt=J_NULL;} // default json object will be null
	json(const C8* data); // build by either or cstr, if data==0 then use dp
	json(string data);	// build by string
// create
	static json create_null() {return json();}
	static json create_true() {return json("true");}
	static json create_false() {return json("false");}
	static json create_float(float f) {json j=json(); j.jt=J_NUMBER;j.number=f;return j;}
	static json create_int(S32 i) {json j=json(); j.jt=J_NUMBER;j.number=float(i);return j;}
	static json create_string(string s) {json j=json(); j.jt=J_STRING;j.str=s;return j;}
	static json create_array() {return json("[]");}
	static json create_object() {return json("{}");}
// write
	string save(bool right_of_colon=false) const; // save as string, if rightofcolon==true then disable indentation
	void save(const C8* fname) const; // save as file
// type
	jtype get_type() const { return jt;}
	const C8* get_type_name() const { return typenames[jt];}
// iterator
	U32 size() const; // get size of array or object
//	map<string,json>& get_object() {return object;}
//	vector<json>& get_array() {return array;}
// find
	const json& find_object(string objname) const;
	const json& find_array(S32 idx=-1) const; // -1 returns last
// insert
	void insert_object(string name,json j);
	void insert_array(json j,S32 idx=-1); // -1 adds to end
// remove
	void erase_object(string objname);
	void erase_array(S32 idx); // -1 removes last
// get
	float get_float() const { return number; }
	S32 get_int() const { S32 ri; F2INT(number,ri); return ri; }
	string get_string() const { return str; }
// convert json to c++ class members, json struct is like {"name": {"type": {"name" etc... {"mathutil": {"constants": {"pi": 3.14}}}
	bool get_object_float(string name,float& f) const; // get_object_float("pi",pi);
	bool get_object_int(string name,S32& f) const; // for now, range of -16777216 to 16777216 around 16 million, maybe change later
	bool get_object_string(string name,string& s) const;
	const json& find_object_object(string name,string type) const; // like "firstrect","rect", double descent

template <typename B,typename D> // construct basetype from name and nameoftype
	bool get_object_object(string name,string nameoftype,B& basetype) const;

// use more templates !! (get rid of U8,int,float,string)
	bool get_object_array_U8(string name,U8* vs,U32 arrsize) const; // construct U8 array from name
	bool get_object_array_int(string name,S32* vs,U32 arrsize) const; // construct int array from name
	bool get_object_array_float(string name,float* vs,U32 arrsize) const; // construct float array from name
	bool get_object_array_string(string name,string* vs,U32 arrsize) const; // construct string array from name
	bool get_object_array_U8(string name,vector<U8>& vs) const; // construct int array from name
	bool get_object_array_int(string name,vector<S32>& vs) const; // construct int array from name
	bool get_object_array_float(string name,vector<float>& vs) const; // construct float array from name
	bool get_object_array_string(string name,vector<string>& vs) const; // construct string array from name
template <typename B,typename D> // construct basetype array from name and nameoftype
	bool get_object_array_object(string name,string nameoftype,vector<B>& basetypevector) const;
template <typename B,typename D> // construct basetype array from name and nameoftype
	bool get_object_array_object(string name,string nameoftype,B* basetypearray,U32 arrsize) const;

	void insert_int(string name,S32 i); // add a class/struct to json object
	void insert_float(string name,float f); // add a class/struct to json object
	void insert_string(string name,string s); // add a class/struct to json object
template <typename B,typename D> // construct object from name and nameoftype
	void insert_object_object(string name,string nametype,const D& d); // add a class/struct to json object

	void insert_array_U8(string name,const U8* u8s,U32 arrsize); // add a class/struct to json object
	void insert_array_int(string name,const S32* ints,U32 arrsize); // add a class/struct to json object
	void insert_array_float(string name,const float* floats,U32 arrsize); // add a class/struct to json object
	void insert_array_string(string name,const string *strings,U32 arrsize); // add a class/struct to json object
	void insert_array_U8(string name,const vector<U8>& u8s); // add a class/struct to json object
	void insert_array_int(string name,const vector<S32>& ints); // add a class/struct to json object
	void insert_array_float(string name,const vector<float>& floats); // add a class/struct to json object
	void insert_array_string(string name,const vector<string>& strings); // add a class/struct to json object
template <typename B,typename D> // construct object array from name and nameoftype
	void insert_array_object(string name,string nametype,const vector<B>& basetypevector); // add a class/struct to json object
template <typename B,typename D> // construct object array from name and nameoftype
	void insert_array_object(string name,string nametype,const B* basetypearray,U32 arrsize); // add a class/struct to json object
};

template<typename B,typename D>
bool json::get_object_object(string name,string nameoftype,B& basetype) const
{
	const json& jm=find_object_object(name,nameoftype);
	basetype = D(jm);
	if (jm.get_type()==J_OBJECT) {
		return true;
	} else
		return false;
}

template <typename B,typename D>
bool json::get_object_array_object(string name,string nameoftype,vector<B>& basetypevector) const
{
	const json& jm=find_object_object(name,nameoftype);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		basetypevector.clear();
		for (i=0;i<n;++i) {
			B e = D(jm.find_array(i));
			basetypevector.push_back(e);
		}
		return true;
	} else
		return false;
}

template <typename B,typename D>
bool json::get_object_array_object(string name,string nameoftype,B* basetypearray,U32 arrsize) const
{
	const json& jm=find_object_object(name,nameoftype);
	if (jm.get_type()==J_ARRAY) {
		U32 i,n=jm.size();
		if (n>arrsize)
			n=arrsize;
		for (i=0;i<n;++i) {
			B e = D(jm.find_array(i));
			basetypearray[i]=e;
		}
		return true;
	} else 
		return false;
}

#if 1
template <typename B,typename D> // construct basetype array from name and nameoftype
void json::insert_object_object(string name,string nametype,const D& d) // add a class/struct to json object
{
	json ins = create_object();
	json ins2 = d.save();
	ins.insert_object(nametype,ins2);
	insert_object(name,ins);
}
#endif

template <typename B,typename D> // construct basetype array from name and nameoftype
void json::insert_array_object(string name,string nametype,const vector<B>& b) // add a class/struct to json object
{
	json ins = create_object();
	json ins2 = create_array();
	U32 i,n=b.size();
	for (i=0;i<n;++i) {
		D d=b[i];
		ins2.insert_array(d.save());
	}
	ins.insert_object(nametype,ins2);
	insert_object(name,ins);
}

template <typename B,typename D> // construct basetype array from name and nameoftype
void json::insert_array_object(string name,string nametype,const B* b,U32 size) // add a class/struct to json object
{
	json ins = create_object();
	json ins2 = create_array();
	U32 i,n=size;
	for (i=0;i<n;++i) {
		D d=b[i];
		ins2.insert_array(d.save());
	}
	ins.insert_object(nametype,ins2);
	insert_object(name,ins);
}

