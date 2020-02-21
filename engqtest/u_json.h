//#define VERBOSE
// jason type
class json {
	static json nulljson;
public:
	enum jtype {J_STRING,J_NUMBER,J_OBJECT,J_ARRAY,J_TRUE,J_FALSE,J_NULL};
private:
// data
	static const C8* dp; // for loading
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
	S32 size(); // get size of array or object
//	map<string,json>& get_object() {return object;}
//	vector<json>& get_array() {return array;}
// find
	json& find_object(string objname);
	json& find_array(S32 idx=-1); // -1 returns last
// insert
	void insert_object(string name,json j);
	void insert_array(json j,S32 idx=-1); // -1 adds to end
// remove
	void erase_object(string objname);
	void erase_array(S32 idx); // -1 removes last
// get
	float get_float() { return number; }
	S32 get_int() { S32 ri; F2INT(number,ri); return ri; }
	string get_string() { return str; }
// convert json to c++ class members, json struct is like {"name": {"type": {"name" etc... {"mathutil": {"constants": {"pi": 3.14}}}
	bool get_object_float(string name,float& f); // get_object_float("pi",pi);
	bool get_object_int(string name,S32& f); // for now, range of -16777216 to 16777216 around 16 million, maybe change later
	bool get_object_string(string name,string& s);
	json& find_object_object(string name,string type); // like "firstrect","rect", double descent

template <typename B,typename D> // construct basetype from name and nameoftype
	bool get_object_object(string name,string nameoftype,B& basetype);

	bool get_object_array_string(string name,vector<string>& vs); // construct string array from name
	bool get_object_array_float(string name,vector<float>& vs); // construct float array from name
	bool get_object_array_int(string name,vector<S32>& vs); // construct int array from name
template <typename B,typename D> // construct basetype array from name and nameoftype
	bool get_object_array_object(string name,string nameoftype,vector<B>& basetypevector);

template <typename B,typename D> // construct object from name and nameoftype
	void insert_object_object(string name,string nametype,D d); // add a class/struct to json object

	void insert_array_int(string name,vector<S32>& ints); // add a class/struct to json object
	void insert_array_float(string name,vector<float>& floats); // add a class/struct to json object
	void insert_array_string(string name,vector<string>& strings); // add a class/struct to json object
template <typename B,typename D> // construct object array from name and nameoftype
	void insert_array_object(string name,string nametype,vector<B>& basetypevector); // add a class/struct to json object
};

void json_test();