// try and figure out small groups
namespace smallgroups {

class perm // a permutation
{
	// 0 index is on the left
	vector<S32> pd; // permutation data
public:
// constructors / casts
	perm() {} // default to 0 element pd
	perm(S32 n); // default to n element identity pd
	perm(const vector<S32>& ); // init with a whole vector, assume input is kosher
	perm(const S32*,S32 n); // init with a whole array, assume input is kosher
	operator vector<S32>() {return pd;} // cast back to whole vector
// operations
	perm operator*(const perm& rhs); // multiply, assume both are same size
	perm operator++(); // increment
	bool operator==(const perm& rhs); // are they the same
	bool operator!=(const perm& rhs) {return !(*this==rhs);} // are they different
	bool isident(); // true if identity
// info
	S32 size() const { return pd.size(); }
// print
	void printp();
	void printc(); // list of disjoint cycles
};

// a group with a core that is a vector of permutations
class group {
// a group built out of a list of permutations
// order of group is vector size
// elements of group are the index or the vector perms
	vector <perm> perms; // 0th perm is ident..
	S32 addperm(const perm& p); // for close, return index
	void close(); // fill in the gaps, multiply 'till the group is closed
public:
	group(S32,bool full); // generate a Sn symmetry group, if not full then create group with just identity perm
	group(const vector<perm>& permsa); // a list of permutations, auto close
// operations
	S32 times(S32 lhs,S32 rhs); // multiply
	perm timesp(S32 lhs,S32 rhs); // multiply
// info
	S32 getorder() { return perms.size(); }
// print
	void printc(const C8* titlea=0); // print a nice Cayley table
};

// a group with a core that is a Cayley table, can be incomplete, in which case -1's will be in table
class cgroup {
	static const S32 maxord=16;
//	vector<vector<S32> > ct; // Cayley table e=0
//	S32* dct;
	S32 dct[maxord*maxord]; // the table
	S32 order;
public:
	cgroup(S32 na); // generate an partially uninitialized order n group, -1 means uninitialized
	cgroup(const cgroup& oga);
	//~cgroup();
// operations
	S32 times(S32 lhs,S32 rhs) const; // multiply/read
	void fastwrite(S32 lhs,S32 rhs,S32 vala); // write, no checks
	bool write(S32 lhs,S32 rhs,S32 vala); // write, true if good, doesn't write if false, checks cancellation, assoc
	bool hasm1(S32* i,S32* j) const; // true if group has -1
	bool operator==(const cgroup& rhs) const;
	bool isiso(const cgroup& rhs,S32 a,S32 b) const; // returns true if after switching a and b are equal
// listbuilder
	vector<cgroup> genidentskel() const; // list of identity skeletons for this group size
	vector<cgroup> guessnext() const; // list of replacements for next -1
// info
	S32 getorder() { return order; } // print
	void printc(const C8* titlea=0) const; // print a nice Cayley table
};

}
