#define DONAMESPACE
#ifdef DONAMESPACE
namespace qcomp {
#endif

extern const float SR2;
extern const float SR3;
extern const float SR2O2; // for 45 degrees
extern const float SR3O2; // for 30 degrees and so on
extern const float SR1D3; // for W state G gate
extern const float SR2D3; // ..
//typedef complex<float> compf;
void printcompf(const string& title,const compf& cf);
float getprob(const compf& a);
//class qmat;
const float QEPSILON = .0001f; // more tolerance
bool almostEqual(const compf& a,const compf& b);
class qstate;
void printfactors(const vector<qstate>& factors,const string& title,con32 *con=0);
void smallfloat(C8* str,float sf);

// column vectors
// general quantum state, can be entangled
class qstate
{
	U32 numstates; // 1 << numqubits    OR    2 ^ numqubits
	U32 numqubits; // this state has this many qubits
	vector<compf> states; // an array of complex numbers describes this state
public:
	// constructors
	qstate(); // empty
	//qstate(U32 dim); // default to 0 values
	qstate(U32 numqubitsa);
	qstate(const qstate& rhs); // copy
	// load and print
	U32 getnumstates() const {return numstates;}
	const compf& getnumstate(U32 idx) const {return states[idx];} 
	void load(const compf* data,U32 n2size); // copy 1d n data array to the 1d n  qstate array, n2size must be a power of 2
	static string idxtobinstring(U32 val,U32 bits);
	static string qubittobinstring(U32 val,U32 bits);
	void print(string title,bool measure=true,con32* con=0,S32 qoff=0) const;
	pointf3 qstateToBloch() const; // convert quantum state to bloch sphere, only if num qubits == 1
	float gettotprob(); // should be 1, unless measured
	void normalize();
	void stdphase();
	// operators
	static string itos(U32 i);
	qstate operator=(const qstate& rhs);
	compf operator*(const qstate& rhs);
	qstate operator^(const qstate& rhs); // outer product (tensor)
	// try to factor a qstate into disentangled qubits
	vector<qstate> factor();
	// change qstate
	// keeps current numstates
	void clear(); // set to all zeros
	// common matrices, quantum gates
	void init0(); // top one |0> set to 1
	void initnm1(); // bottom one |(dim-1)> set to 1
	void setidx(U32 idx); // set idx to 1, all else 0's |idx> set to 1
	void spread(); // all real and evenly spread out
	// numstates = 4, 2 qubits
	void bell(); // standard bell state for entanglement
	void test1();
	void test2();
	void test3(); // .6 .8
	void testmeasure1(); // .1 .2 .3 .4
	vector<float> measureall() const; // measure all qubits returns prob array of all qubits
	// measure just one qubit, return array[2] of qstates with prob of measuring 0, measuring 1 is M1 = 1 - M0
	vector<qstate> measure1(U32 quidx,float& prob) const;
	friend class qmat; // qmat has access to qstate
};

#ifdef DONAMESPACE
} // end namespace qcomp
using namespace qcomp;
#endif
