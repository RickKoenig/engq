//#include <complex>

#include <m_eng.h>

#include "u_qstate.h"
#include "u_qmat.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace qcomp {
#endif

// num dimensions constructor
qmat::qmat(S32 dima) : dim(dima)
{
	U32 numqubits = ilog2(dim);
	if ((1<<numqubits) != dim)
		errorexit("qmat::qmat   dim %d is not a power of 2",dim);
	ele.resize(dim);
	S32 i;
	for (i=0;i<dim;++i) {
		ele[i].resize(dim);
		fill(ele[i].begin(),ele[i].end(),compf());
		ele[i][i] = 1; // make identity
	}
}

// copy constructor
/*qmat::qmat(const qmat& rhs) : dim(rhs.dim)
{
	ele.resize(dim);
	S32 i;
	for (i=0;i<dim;++i) {
		ele[i].assign(rhs.ele[i].begin(),rhs.ele[i].end());
	}
}*/

qmat qmat::qmatqu(S32 numqubits)
{
	if (numqubits<0)
		numqubits = 0;
	return qmat(1<<numqubits);
}


// private reorder of binary bits
U32 qmat::mapbits(U32 inbits,const vector<U32>& pins) //,U32 numpins) 
{ 
	// reorder the bits of a binary number using 'pins'
	// order is reversed because qubit 0 is most significant bit
	U32 numpins = pins.size();
	U32 i;
	U32 ret = 0;
	for (i=0;i<pins.size();++i) {
		U32 inbit = getbitval(inbits,numpins - 1 - i);
		U32 outbit = setbitval(inbit,numpins -1 - pins[i]);
		ret += outbit;
	}
	return ret;
}

U32 qmat::getbitval(U32 val,U32 bitnum) 
{
	return (val>>bitnum)&1;
}

U32 qmat::setbitval(U32 bit,U32 bitnum) 
{
	return bit<<bitnum;
}

S32 qmat::filterprint(float v)
{
	if (v >= .125f) // half of 1/4
		return 1;
	if (v <= -.125f)
		return -1;
	return 0;
}
/*
// default constructor
qmat::qmat() : dim(0)
{
}
*/

compf qmat::det() const
{
#if 0
	if (dim !=2)
		return 1; // later
	return ele[0][0]*ele[1][1] - ele[1][0]*ele[0][1];
#else
	// quick check, return 0 for measurement gate, 1 for unitary gate
	// look for any column this is all 0's
	S32 i,j;
	for (i=0;i<dim;++i) {
		for (j=0;j<dim;++j) {
			compf v = ele[j][i];
			float p = getprob(v);
			if (p >= QEPSILON)
				break;
		}
		if (j == dim)
			return 0;
	}
	return 1;
#endif
}

// load dim^2 one dimensional array into qmat(dim)
void qmat::load(const compf* data,U32 datasize)
{
	U32 lgdatasize = ilog2(datasize);
	if (lgdatasize&1)
		errorexit("qmat::load1   datasize %d is not a power of 4",datasize);
	U32 numqbits = lgdatasize>>1;
	if ((1<<lgdatasize) != datasize)
		errorexit("qmat::load2   datasize %d is not a power of 4",datasize);
	dim = isqrt(datasize);
	ele.resize(dim);
	S32 i,j,k=0;
	for (j=0;j<dim;++j) {
		ele[j].resize(dim);
		for (i=0;i<dim;++i) {
			ele[j][i] = data[k++];
		}
	}
}

// show matrix
void qmat::print(string title,bool abbrev,con32* con) const
{
	if (con) {
		//con32_clear(con);
		if (abbrev)
			con32_printf(con,"QMAT '%s' super special print 1/4 * \n",title.c_str());
		else
			con32_printf(con,"QMAT '%s'\n",title.c_str());
		S32 i,j;
		for (j=0;j<dim;++j) {
			if (abbrev && !(j%4) && j)
				con32_printf(con,"\n\n");
			//con32_printf(con,"\t");
			for (i=0;i<dim;++i) {
				if (abbrev && !(i%4) && i)
					con32_printf(con,"      ");
				const compf& e = ele[j][i];
				if (abbrev) { // super short print if 1/4 values
					S32 fpr = filterprint(e.real());
					S32 fpi = filterprint(e.imag());
					//fpr = ((i+1)%3)-1; // test
					//fpi = ((j+1)%3)-1; // test
					if (!fpi) { // all real
						con32_printf(con,"%4d ",fpr);
					} else { // some imag
						if (!fpr) { // pure imag
							if (fpi > 0)
								con32_printf(con,"   i ");
							else
								con32_printf(con,"  -i ");
						} else { // complex
							if (fpi > 0)
								con32_printf(con,"%2d+i ",fpr);
							else
								con32_printf(con,"%2d-i ",fpr);
						}
					}
				} else { // normal float print
					C8 sr[50];
					C8 si[50];
					smallfloat(sr,e.real());
					smallfloat(si,e.imag());
					con32_printf(con,"%s+%si",sr,si);
					//con32_printf(con,".");
				}
			}
			con32_printf(con,"\n");
		}
	} else {
		if (abbrev)
			logger("QMAT '%s' super special print 1/4 * \n",title.c_str());
		else
			logger("QMAT '%s'\n",title.c_str());
		S32 i,j;
		for (j=0;j<dim;++j) {
			if (abbrev && !(j%4) && j)
				logger("\n\n");
			//logger("\t");
			for (i=0;i<dim;++i) {
				if (abbrev && !(i%4) && i)
					logger("      ");
				const compf& e = ele[j][i];
				if (abbrev) { // super short print if 1/4 values
					S32 fpr = filterprint(e.real());
					S32 fpi = filterprint(e.imag());
					//fpr = ((i+1)%3)-1; // test
					//fpi = ((j+1)%3)-1; // test
					if (!fpi) { // all real
						logger("%4d ",fpr);
					} else { // some imag
						if (!fpr) { // pure imag
							if (fpi > 0)
								logger("   i ");
							else
								logger("  -i ");
						} else { // complex
							if (fpi > 0)
								logger("%2d+i ",fpr);
							else
								logger("%2d-i ",fpr);
						}
					}
				} else { // normal float print
					logger("(%5.2f,%5.2fi) ",e.real(),e.imag());
				}
			}
			logger("\n");
		}
	}
}

// assignment
qmat qmat::operator=(const qmat& rhs)// copy assignment
{
    if (this != &rhs) { // self-assignment check expected
		dim = rhs.dim;
		ele.resize(dim);
		S32 i;
		for (i=0;i<dim;++i) {
			ele[i].assign(rhs.ele[i].begin(),rhs.ele[i].end());
		}
    }
    return *this;
}

// qstateb = qmat*qstatea
// multiply a qmat times qstate to return a qstate
//#define PROCNORMALIZE // try to keep this turned off
qstate qmat::operator*(const qstate& rhs) const
{
	if (dim != rhs.numstates)
		errorexit("qstate qmat *: lhs dim = %d, rhs numstates = %d",dim,rhs.numstates);
	qstate ret(rhs.numqubits);//dim);
	S32 i,j;
	for (j=0;j<dim;++j) {
		for (i=0;i<dim;++i) {
			ret.states[j] += ele[j][i]*rhs.states[i];
		}
	}
#ifdef PROCNORMALIZE
	ret.normalize(); // maybe use this for measurement gates since they are non-unitary, help detects entaglement after measurement
#endif
	return ret;
}

// multiply 2 matrices together
// this is where classical simulations get much slower
qmat qmat::operator*(const qmat& rhs) const
{
	if (dim != rhs.dim)
		errorexit("qmat qmat *: lhs dim = %d, rhs dim = %d",dim,rhs.dim);
	qmat ret(dim);
	ret.clear();
	S32 i,j,k;
	for (j=0;j<dim;++j) {
		for (i=0;i<dim;++i) {
			compf r = 0;
			for (k=0;k<dim;++k) {
				r += ele[j][k]*rhs.ele[k][i];
			}
			ret.ele[j][i] = r;
		}
	}
	return ret;
}

// multiply 2 matrices together using tensor product
qmat qmat::operator^(const qmat& rhs) const
{
	// not needed
	//if (dim != rhs.dim)
	//	errorexit("qmat qmat ^: lhs dim = %d, rhs dim = %d",dim,rhs.dim);
	qmat ret(dim*rhs.dim);
	S32 i,j,u,v,x,y;
	for (v=0,y=0;v<dim;++v) {
		for (j=0;j<rhs.dim;++j,++y) {
			for (u=0,x=0;u<dim;++u) {
				for (i=0;i<rhs.dim;++i,++x) {
					ret.ele[y][x] = ele[v][u]*rhs.ele[j][i];
				}
			}
		}
	}
	return ret;
}

void qmat::swapqubits(U32 a,U32 b)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	swapqubits(arr);
}

void qmat::swapqubits(U32 a,U32 b,U32 c)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
	swapqubits(arr);
}

void qmat::swapqubits(U32 a,U32 b,U32 c,U32 d)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
	arr.push_back(d);
	swapqubits(arr);
}

void qmat::swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
	arr.push_back(d);
	arr.push_back(e);
	swapqubits(arr);
}

void qmat::swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e,U32 f)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
	arr.push_back(d);
	arr.push_back(e);
	arr.push_back(f);
	swapqubits(arr);
}

void qmat::swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e,U32 f,U32 g)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
	arr.push_back(d);
	arr.push_back(e);
	arr.push_back(f);
	arr.push_back(g);
	swapqubits(arr);
}

void qmat::swapqubits(U32 a,U32 b,U32 c,U32 d,U32 e,U32 f,U32 g,U32 h)
{
	vector<U32> arr;
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
	arr.push_back(d);
	arr.push_back(e);
	arr.push_back(f);
	arr.push_back(g);
	arr.push_back(h);
	swapqubits(arr);
}

void qmat::swapqubits(const vector<U32>& pins)
{
	// checks, invalid input
	// num qubits must match
	S32 numpins = pins.size();
	if ((1<<numpins) != dim)
		errorexit("swapqubits: bad input, dim = %d, pins.size() = %d\n",dim,numpins);
	vector<U32> counts(numpins);
	S32 i;
	for (i=0;i<numpins;++i) {
		S32 val = pins[i];
		if (val >= numpins)
			errorexit("swapqubits: bad input, pin %d is %d which is > numpins %d\n",i,val,numpins);
		++counts[val];
	}
	for (i=0;i<numpins;++i) {
		U32 val = counts[i];
		if (val != 1) {
			errorexit("swapqubits: bad input, pin %d count %d not a 1\n",i,val);
		}
	}
	// checking now done, build a swp-1 * qmat * swp matrix
	vector<U32> mapper(dim);
#if 0
	logger("swapqubits (");
	for (i=0;i<numpins-1;++i)
		logger("%d, ",pins[i]);
	logger("%d)",pins[i]);
	logger("\n");

	logger("map\n");
	// a new meaning for bit mapping
	//vector<U32> invmapper(dim);
	// map the bits of the input using 'pins' to an output
#endif
	for (i=0;i<dim;++i) {
		U32 m = mapbits(i,pins);//,numpins);
		mapper[i] = m;
		//invmapper[m] = i;
	}
#if 0
	for (i=0;i<dim;++i) {
		logger("mapper %d to %d\n",i,mapper[i]);
	}
#endif
	qmat frontmat(dim); // rearrange pins in front (first)
	qmat invmat(dim); // undo rearrange pins in back (last)
	frontmat.clear();
	invmat.clear();
	for (i=0;i<dim;++i) {
		U32 m = mapper[i];
		frontmat.ele[i][m] = 1;
		invmat.ele[m][i] = 1; // don't need conjugates because all numbers are 0 or 1 (real)
	}
	*this = invmat * *this * frontmat; // math goes right to left
}

// all entry set to 0
void qmat::clear()
{
	S32 j;
	for (j=0;j<dim;++j) {
		fill(ele[j].begin(),ele[j].end(),compf(0));
	}
}

// make this qmat an identity of dimension dim
void qmat::identn()
{
	S32 i;
	clear();
	for (i=0;i<dim;++i)
		ele[i][i] = 1;
}

// dim = 2, 1 qubit, 2 by 2 identity
void qmat::ident1()
{
	static compf data[] = {
		1,0,
		0,1
	};
	load(data,NUMELEMENTS(data));
}

void qmat::pauliX()
{
#if 0
	static compf data[] = {
		0,1,
		1,0
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(1);
#endif
}

void qmat::not1() // invert 1 qubit
{
	pauliX();
}

void qmat::pauliY()
{
	static compf data[] = {
		0,compf(0,-1),
		compf(0,1),0
	};
	load(data,NUMELEMENTS(data));
}

void qmat::pauliZ()
{
	static compf data[] = {
		1,0,
		0,-1
	};
	load(data,NUMELEMENTS(data));
}

void qmat::S()
{
	static compf data[] = {
		1,0,
		0,compf(0,1)
	};
	load(data,NUMELEMENTS(data));
}

void qmat::St()
{
	static compf data[] = {
		1,0,
		0,compf(0,-1)
	};
	load(data,NUMELEMENTS(data));
}

void qmat::T()
{
	static compf data[] = {
		1,0,
		0,compf(SR2O2,SR2O2)
	};
	load(data,NUMELEMENTS(data));
}

void qmat::Tt()
{
	static compf data[] = {
		1,0,
		0,compf(SR2O2,-SR2O2)
	};
	load(data,NUMELEMENTS(data));
}

void qmat::hadamard()
{
	static compf data[] = {
		SR2O2,SR2O2,
		SR2O2,-SR2O2
	};
	load(data,NUMELEMENTS(data));
}

void qmat::m0()
{
	static compf data[] = {
		1,0,
		0,0
	};
	load(data,NUMELEMENTS(data));
}

void qmat::m1()
{
	static compf data[] = {
		0,0,
		0,1
	};
	load(data,NUMELEMENTS(data));
}

void qmat::splitter()
{
	static compf data[] = {
		SR2O2,compf(0,SR2O2),
		compf(0,SR2O2),SR2O2
	};
	load(data,NUMELEMENTS(data));
}

void qmat::mirror()
{
	static compf data[] = {
		compf(0,1),0,
		0,compf(0,1),
	};
	load(data,NUMELEMENTS(data));
}

void qmat::snotto()
{
	const bool TWISTEDOAK = true;
	// play with phases
//#define RNDM // alter the phase of any row or column of the last gate and it sometimes works
//#define WIKI // alter the phase of any row or column of the last gate and it doesn't work
#define NONE // don't alter phases
	// almost, not quite, try angle of 4 radians
#ifdef RNDM
	float r0 = 0; // test any values here
	float r1 = 0;
	float c0 = .01f;
	float c1 = 0;
#endif
#ifdef WIKI
	float r0 =  PI*.25f;
	float r1 = -PI*.25f;
	float c0 = 0;
	float c1 = PI*.5f;
#endif
#ifdef NONE
	float r0 = 0;
	float r1 = 0;
	float c0 = 0;
	float c1 = 0;
#endif
	const compf pr0 = exp(compf(0,r0));
	const compf pr1 = exp(compf(0,r1));
	const compf pc0 = exp(compf(0,c0));
	const compf pc1 = exp(compf(0,c1));
	// let's just do phase test to the twisted
	static compf data_twist[] = {
		pr0*pc0*SR2O2,pr0*pc1*-SR2O2,
		pr1*pc0*SR2O2,pr1*pc1* SR2O2
	};
	compf opi(.5f, .5f);
	compf omi(.5f,-.5f);
	static compf data_wiki[] = {
		opi,omi,
		omi,opi,
	};
	if (TWISTEDOAK) {
		load(data_twist,NUMELEMENTS(data_twist));
	} else { // WIKIPEDIA
		load(data_wiki,NUMELEMENTS(data_wiki));
	}
}

void qmat::snotwp()
{
	static compf data_wiki[] = {
		compf(.5f,.5f),compf(.5f,-.5f),
		compf(.5f,-.5f),compf(.5f,.5f)
	};
	load(data_wiki,NUMELEMENTS(data_wiki));
}

void qmat::g1over3()
{
	static compf data[] = {
		SR2D3,-SR1D3,
		SR1D3,SR2D3,
	};
	load(data,NUMELEMENTS(data));
}

void qmat::gm1over3()
{
	static compf data[] = {
		SR2D3,SR1D3,
		-SR1D3,SR2D3,
	};
	load(data,NUMELEMENTS(data));
}

void qmat::p45()
{
	static compf data[] = {
		compf(SR2O2,SR2O2),0,
		0,compf(SR2O2,SR2O2),
	};
	load(data,NUMELEMENTS(data));
}

void qmat::pm45()
{
	static compf data[] = {
		compf(SR2O2,-SR2O2),0,
		0,compf(SR2O2,-SR2O2),
	};
	load(data,NUMELEMENTS(data));
}

void qmat::p30()
{
	static compf data[] = {
		compf(SR3O2,.5f),0,
		0,compf(SR3O2,.5f),
	};
	load(data,NUMELEMENTS(data));
}

void qmat::pm30()
{
	static compf data[] = {
		compf(SR3O2,-.5f),0,
		0,compf(SR3O2,-.5f),
	};
	load(data,NUMELEMENTS(data));
}

void qmat::test1()
{
	static compf data[] = {
		2,3,
		5,7		
	};
	load(data,NUMELEMENTS(data));
}

void qmat::test2()
{
	static compf data[] = {
		11,13,
		17,19
	};
	load(data,NUMELEMENTS(data));
}

// dim = 4, 2 qubits
void qmat::cnot()
{
#if 0
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,0,1,
		0,0,1,0
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::cz()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,-1,
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::cy()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,0,compf(0,-1),
		0,0,compf(0,1),0,
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::ch()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,SR2O2,SR2O2,
		0,0,SR2O2,-SR2O2,
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::cs()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,compf(0,1),
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::cst()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,compf(0,-1),
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::ct()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,compf(SR2O2,SR2O2)
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::ctt()
{
#if 1
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,compf(SR2O2,-SR2O2)
	};
	load(data,NUMELEMENTS(data));
#else
	toffoli(2);
#endif
}

// dim = 4, 2 qubits
void qmat::cp45()
{
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,compf(SR2O2,SR2O2),0,
		0,0,0,compf(SR2O2,SR2O2),
	};
	load(data,NUMELEMENTS(data));
}

// dim = 4, 2 qubits
void qmat::cpm45()
{
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,compf(SR2O2,-SR2O2),0,
		0,0,0,compf(SR2O2,-SR2O2),
	};
	load(data,NUMELEMENTS(data));
}

// dim = 4, 2 qubits
void qmat::cp30()
{
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,compf(SR3O2,.5f),0,
		0,0,0,compf(SR3O2,.5f),
	};
	load(data,NUMELEMENTS(data));
}

// dim = 4, 2 qubits
void qmat::cpm30()
{
	static compf data[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,compf(SR3O2,-.5f),0,
		0,0,0,compf(SR3O2,-.5f),
	};
	load(data,NUMELEMENTS(data));
}

void qmat::swap()
{
	static compf data[] = {
		1,0,0,0,
		0,0,1,0,
		0,1,0,0,
		0,0,0,1
	};
	load(data,NUMELEMENTS(data));
}

void qmat::dec()
{
	static compf data[] = {
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
		1,0,0,0
	};
	load(data,NUMELEMENTS(data));
}

// dim = 8, 3 qubits
void qmat::fredkin3()
{
	static compf data[] = {
		1,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,0,0,0,1,
	};
	load(data,NUMELEMENTS(data));
}
#if 0
void qmat::toffoli3()
{
#if 1
	*this = qmat(1<<3);
	ele[6][6] = 0;
	ele[6][7] = 1;
	ele[7][6] = 1;
	ele[7][7] = 0;
#else
	static compf data[] = {
		1,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
	};
	load(data,NUMELEMENTS(data));
#endif
}

void qmat::toffoli4()
{
	*this = qmat(1<<4);
	ele[14][14] = 0;
	ele[14][15] = 1;
	ele[15][14] = 1;
	ele[15][15] = 0;
}
#else
void qmat::toffoli(U32 qb)
{
	U32 p = 1<<qb;
	*this = qmat(p);
	U32 pm1 = p - 1;
	U32 pm2 = p - 2;
	ele[pm2][pm2] = 0;
	ele[pm2][pm1] = 1;
	ele[pm1][pm2] = 1;
	ele[pm1][pm1] = 0;
/*	ele[14][14] = 0;
	ele[14][15] = 1;
	ele[15][14] = 1;
	ele[15][15] = 0; */
}
#endif

#ifdef DONAMESPACE
} // end namespace qcomp

using namespace qcomp;
#endif
