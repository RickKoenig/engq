// from physics.h for c2cpnts

// pointf3log class, just adds log to pointf3
class pointf3log : public pointf3x {
public:
	string log() const;
};
// pointf3j class, buld and save pointf3log
class pointf3j : public pointf3log {
public:
	pointf3j(const json& js);
	pointf3j(const pointf3& in) { *((pointf3*)(this)) = in; } // cast constructor from pointf3 to pointf3j
	json save() const;
};
// pointi2log class, just adds log to pointi2
class pointi2log : public pointi2 {
public:
	string log() const;
};
// pointi2j class, just ints
class pointi2j : public pointi2log {
public:
	pointi2j(json& js);
	pointi2j(const pointi2& in) { *((pointi2*)(this)) = in; } // cast constructor from pointf3 to pointf3j
	json save();
};

// jmodelb class, NYI, just a thought
class modelj : public modelb {
	modelj(json& js);
};
// rect class, a test class for json to/from c++
class rectf3 {
protected:
	pointf3log topleft;
	pointf3log size;
	string name;
	S32 pri;
	vector<pointf3log> parr;
	vector<string> sarr;
	vector<float> farr;
	vector<S32> iarr;
public:
	string log() const;
};
// rectj class
class rectf3j : public rectf3 {
public:
	rectf3j(json& js);
	json save();
};

