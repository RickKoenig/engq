#define DONAMESPACE
#ifdef DONAMESPACE
namespace qcomp {
#endif

// row major
// square matrices, should be a power of 2, not checked, 2^qubits = dim
class qmat
{
	S32 dim; // 2 ^ numqubits
public: // test
	vector<vector<compf> > ele; // total elements = 2 ^ ((num channels)*2)
	// constructors
	//qmat(); // empty
	qmat(S32 dima = 1); // default to identity
	static qmat qmatqu(S32 numqubits);
private:
	static U32 mapbits(U32 inbits,const vector<U32>& pins);/*,U32 numpins);*/ // reorder the bits of a binary number using 'pins'
	static U32 getbitval(U32 val,U32 bitnum); // returns a bit 0 or 1 if that bit is set or not
	static U32 setbitval(U32 bit,U32 bitnum); // returns a new int with the bit set or not
	static S32 filterprint(float v);
public:
	compf det() const;
	//qmat(const qmat& rhs); // copy
	U32 getnumqubits() const {return ilog2(dim);}
	// load and print
	void load(const compf* data,U32 n2size); // copy 1d n^2 data array to the 2d n by n qmat array, n2size must be a power of 4
	void print(string title,bool abbrev=false,con32* con=0) const; // abbrev is true, then use 1/4 * (very specialized)
	// operators
	qmat operator=(const qmat& rhs); // assign
	qstate operator*(const qstate& rhs) const; // multiply qmat * qstate to another qstate
	qmat operator*(const qmat& rhs) const; // multiply qmat * qmat to another qmat
	qmat operator^(const qmat& rhs) const; // multiply qmat * qmat to another larger qmat (tensor product)
	// alters pin assignments for a qmat, can move gates around between the wires(pins)
	// from is implicit, it's the index, the to is the value in args or array
	void swapqubits(U32 a,U32 b); // alters the matrix, swaps inputs and outputs of a qmat, 2 qubits
	void swapqubits(U32 a,U32 b,U32 c); // alters the matrix, swaps inputs and outputs of a qmat, 3 qubits
	void swapqubits(U32 a,U32 b,U32 c,U32 d); // alters the matrix, swaps inputs and outputs of a qmat, 4 qubits
	void swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e); // alters the matrix, swaps inputs and outputs of a qmat, 5 qubits
	void swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e,U32 f); // alters the matrix, swaps inputs and outputs of a qmat, 6 qubits
	void swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e,U32 f,U32 g); // alters the matrix, swaps inputs and outputs of a qmat, 7 qubits
	void swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e,U32 f,U32 g,U32 h); // alters the matrix, swaps inputs and outputs of a qmat, 8 qubits
	void swapqubits(const vector<U32>& arrmap); // alters the matrix, swaps inputs and outputs of a qmat, n qubits
	// change qmat
	// keeps current dim
	void clear(); // set to all zeros
	void identn(); // set this qmat to be identity
	// common matrices, quantum gates

	// sets dim = 2, 1 qubit
	void ident1(); // 1 0 0 1
	void pauliX(); // NOT
	void not1(); // invert 1 qubit
	void pauliY();
	void pauliZ();
	void S();
	void St();
	void T();
	void Tt();
	void hadamard();
	void m0();
	void m1();
	void splitter();
	void mirror();
	void snotto(); // square root of NOT twisted oak
	void snotwp(); // square root of NOT wikipedia
	void g1over3(); // G(1/3) for making a 3 qubit W state
	void gm1over3(); // G(-1/3) for making a 3 qubit W state
	void p45(); // test pure phase shifts
	void pm45();
	void p30();
	void pm30();
	void test1();
	void test2();

	// sets dim = 4, 2 qubits
	void cnot(); // entangler
	void cz(); // controlled PZ gate
	void cy(); // controlled PY gate
	void ch(); // controlled Hadamard gate
	void cs(); // controlled S gate
	void cst(); // controlled St gate
	void ct(); // controlled T gate
	void ctt(); // controlled Tt gate
	void cp45(); // controlled p 45
	void cpm45(); // controlled p -45
	void cp30(); // controlled p 30
	void cpm30(); // controlled p -30
	void swap(); // exchange 2 qubits
	void dec();

	// sets dim = 8, 3 qubits
	void fredkin3(); // smallest fredkin
#if 0
	void toffoli3(); // smallest toffoli
	// set dim = 16, 4 qubits
	void toffoli4();
	// set dim = 32, 5 qubits
	void toffoli5();
	// set dim = 64, 6 qubits
	void toffoli6();
#endif
	// set dim 2^qb size qb qubits
	void toffoli(U32 qb);
	//friend class qstate;
};

#ifdef DONAMESPACE
} // end namespace qcomp
using namespace qcomp;
#endif
