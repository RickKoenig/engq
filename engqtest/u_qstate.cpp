//#include <complex>
//#include <cmath>

#include <m_eng.h>

#include "u_qstate.h"
#include "u_qmat.h"

#define DONAMESPACE
#ifdef DONAMESPACE
namespace qcomp {
#endif

const float SR2 = sqrtf(2.0f);
const float SR3 = sqrtf(3.0f);
const float SR2O2 = SR2*.5f; // O for Over, 1 over square root of 2, D divide right away
const float SR3O2 = SR3*.5f;
const float SR1D3 = sqrtf(1.0f/3.0f);
const float SR2D3 = sqrtf(2.0f/3.0f);

void printcompf(const string& title,const compf& cf)
{
	logger("COMPF '%s' (%8.5f,%8.5fi)\n",title.c_str(),cf.real(),cf.imag());
}

float getprob(const compf& a)
{
	return a.real()*a.real() + a.imag()*a.imag();
}

bool almostEqual(const compf& a,const compf& b)
{
	compf d = a - b;
	float p = getprob(d);
	return p < QEPSILON;
}

void printfactors(const vector<qstate>& factors,const string& title,con32* con) {
	U32 n = factors.size();
	if (con) {
		if (n > 0) {
			U32 i;
			con32_printf(con,"\n");
			con32_printf(con,title.c_str());
			con32_printf(con,"\n");
			for (i=0;i<n;++i) {
				con32_printf(con,"---- factor QUBIT %2d ---- : ",i);
				factors[i].print("factor",false,con,i);
			}
		//} else {
		//	con32_printf(con, (string("can't factor ") + title + "\n").c_str());
		}
	} else {
		if (n > 0) {
			U32 i;
			logger(title.c_str());
			logger("\n");
			for (i=0;i<n;++i) {
				logger("factor QUBIT %2d : ",i);
				factors[i].print("factor",false,0,i);
			}
		} else {
			logger( (string("can't factor ") + title + "\n").c_str());
		}
	}
}

void smallfloat(C8* str,float sf)
{
	//sprintf(str,"%f",sf);
	//strcpy(str,"abc");
	//str[0] = '\0';
	C8 lead = sf >= 0.0f ? ' ' : '-';
	sf = abs(sf);
	//C8 lead = ' ';
	//S32 intf = round(3.4f);
	//S32 intf = S32(100 * sf + 100);
	S32 intf = S32(floor(100*sf + .5f));
	if (intf >= 100)
		sprintf(str,"%c1.",lead);
	else
		sprintf(str,"%c%02d",lead,intf);
}

// defualt constuctor
qstate::qstate() : numstates(0),numqubits(0)
{
}

// num dimensions constructor
qstate::qstate(U32 numqubitsa) : numqubits(numqubitsa)
{
	numstates = 1<<numqubits;
	states.resize(numstates);
	//if (numstates > 0)
	//	states[0] = 1;
}

// copy constructor
qstate::qstate(const qstate& rhs) : numstates(rhs.numstates),numqubits(rhs.numqubits)
{
	states.assign(rhs.states.begin(),rhs.states.end());
}

// load numstates one dimensional array into qstate(numstates)
void qstate::load(const compf* data,U32 datasize)
{
	numstates = datasize;
	numqubits = ilog2(numstates);
	if ((1<<numqubits) != numstates)
		errorexit("qstate::load   datasize %d is not a power of 2",datasize);
/*	states.resize(numstates);
	U32 i;
	for (i=0;i<numstates;++i)
		states[i] = data[i]; */
	states.assign(data,data+datasize);
}

// convert 0,2 to 00
// convert 1,2 to 10
// convert 2,2 to 01
// convert 3,2 to 11
string qstate::idxtobinstring(U32 val,U32 numbits)
{
	string ret = "";
	U32 i;
	for (i=0;i<numbits;++i)
		ret = ((val&(1<<i)) ? "1" : "0") + ret; // lower number qubit is to the left of higher qubits
	return ret;
}

// convert 0,2 to Q.
// convert 1,2 to .Q
string qstate::qubittobinstring(U32 val,U32 numbits)
{
	string ret = "";
	U32 i;
	for (i=0;i<numbits;++i)
		ret += (val == i ? "Q" : ".");
	return ret;
}

void qstate::print(string title,bool measure,con32* con,S32 qoff) const
{
	if (con) {
		con32_printf(con,"QSTATE '%s'\n",title.c_str());
		U32 i,j;
		//U32 nqubits = ilog2(numstates);
		U32 nqubits = numqubits;
		string bitfields = "\tqubit                 Q";
		for (i=0;i<nqubits;++i)
			bitfields += itos(i + qoff);
		con32_printf(con,"%s\n",bitfields.c_str());
		for (i=0;i<numstates;++i) {
			const compf& e = states[i];
			float mprob = getprob(e);
			if (mprob >= QEPSILON) {
				con32_printf(con,"\t(%8.5f,%8.5fi) | %s> P = %8.5f\n",e.real(),e.imag(),idxtobinstring(i,nqubits).c_str(),mprob);
			}
		}
		if (measure) {
			measureall();
			for (i=0;i<nqubits;++i) {
				float mprob;
				vector<qstate> measresult1 = measure1(i,mprob);
				for (j=0;j<2;++j) {
					con32_printf(con,"\tmeasure qubit %d, value %d, prob %8.5f\n",i,j,mprob);
					if (mprob >= QEPSILON) {
						measresult1[j].print("measure",false);
					} else {
						con32_printf(con,"\t\t< no probability >\n");
					}
					mprob = 1 - mprob;
				}
			}
		}
		con32_printf(con,"\n");
	} else {
		if (!measure) {
			logger_indent();
			logger_indent();
		}
		logger("QSTATE '%s'\n",title.c_str());
		U32 i,j;
		U32 nqubits = ilog2(numstates);
		string bitfields = "\tqubit                 Q";
		for (i=0;i<nqubits;++i)
			bitfields += itos(i + qoff);
		logger("%s\n",bitfields.c_str());
		for (i=0;i<numstates;++i) {
			const compf& e = states[i];
			float mprob = getprob(e);
			if (mprob >= QEPSILON) {
				logger("\t(%8.5f,%8.5fi) | %s> P = %8.5f\n",e.real(),e.imag(),idxtobinstring(i,nqubits).c_str(),mprob);
			}
		}
		if (measure) {
			measureall();
			for (i=0;i<nqubits;++i) {
				float mprob;
				vector<qstate> measresult1 = measure1(i,mprob);
				for (j=0;j<2;++j) {
					logger("\tmeasure qubit %d, value %d, prob %8.5f\n",i,j,mprob);
					if (mprob >= QEPSILON) {
						measresult1[j].print("measure",false);
					} else {
						logger("\t\t< no probability >\n");
					}
					mprob = 1 - mprob;
				}
			}
		}
		if (!measure) {
			logger_unindent();
			logger_unindent();
		}
	}
}

/*
credit Craig Gidney, thanks
Breaksdown a matrix U into axis, angle, and phase_angle components satisfying
U = exp(i phase_angle) (I cos(angle/2) - axis sigma i sin(angle/2))

:param matrix: The 2x2 unitary matrix U
:return: The breakdown (axis(x, y, z), angle, phase_angle)
*/

pointf3 qstate::qstateToBloch() const
{
	if (numqubits != 1)
		errorexit("should bloch on just 1 qubit for now !!");
	pointf3x blochstate;
	qstate qsn = *this;
	qsn.normalize();
//	return ret;
//}
//#if 0
	// convert
	// TODO adjust phase
	float alen = abs(qsn.states[0]);
	compf ph;
	if (alen > EPSILON) {
		ph = compf(qsn.states[0].real()/alen,-qsn.states[0].imag()/alen);
		float AP = alen;
		compf BP = qsn.states[1] * ph;
		blochstate.z = 2.0f*AP*AP - 1;
		blochstate.x = 2.0f*AP*BP.real();
		blochstate.y = 2.0f*AP*BP.imag();
	} else {
		blochstate = pointf3x(0,0,-1);
	}
	return blochstate;
//#endif
}

// should be 1, unless measured
float qstate::gettotprob()
{
	float totprob = 0;
	U32 i;
	for (i=0;i<numstates;++i)
		totprob += getprob(states[i]);
	return totprob;
}

// remove 1 degree of freedom
void qstate::normalize()
{
	float totp = gettotprob();
	float rat = 1.0f/sqrtf(totp);
	U32 i;
	for (i=0;i<numstates;++i)
		states[i] *= rat;
}

// remove another degree of freedom, try to set first nonzero amplitude to a positive real number
void qstate::stdphase()
{
	U32 i;
	float r;
	compf u,uc;
	for (i=0;i<numstates;++i) {
		const compf& a = states[i];
		float p = getprob(a);
		if (p > QEPSILON) {
			r = sqrtf(p); // magnitude
			u = a/r; // unit length, a = r*u
			uc = compf(u.real(),-u.imag());
			break;
		}
	}
	if (i == numstates)
		return;
	for (i=0;i<numstates;++i) {
		states[i] *= uc;
	}
}

string qstate::itos(U32 i)
{
	stringstream sb;
	sb << i;
	return sb.str().c_str();
}

// assignment
qstate qstate::operator=(const qstate& rhs) // copy assignment
{
    if (this != &rhs) { // self-assignment check expected
		numstates = rhs.numstates;
		numqubits = rhs.numqubits;
		states.assign(rhs.states.begin(),rhs.states.end());
    }
    return *this;
}

// inner product
compf qstate::operator*(const qstate& rhs)
{
	if (numstates != rhs.numstates)
		errorexit("qstate *: lhs numstates = %d, rhs numstates = %d",numstates,rhs.numstates);
	compf ret = 0;
	U32 i;
	for (i=0;i<numstates;++i)
		ret += conj(states[i])*rhs.states[i];
	return ret;
}

// outer product (tensor)
qstate qstate::operator^(const qstate& rhs)
{
	//if (numstates != rhs.numstates)
	//	errorexit("qstate ^: lhs numstates = %d, rhs numstates = %d",numstates,rhs.numstates);
	qstate ret(numqubits+rhs.numqubits);//numstates*rhs.numstates);
	U32 i,j;
	for (j=0;j<numstates;++j)
		for (i=0;i<rhs.numstates;++i)
			ret.states[numstates*j+i] = rhs.states[i]*states[j];
	return ret;
}

// try to factor a qstate into disentangled qubits
vector<qstate> qstate::factor()
{
	vector<qstate> ret;
	if (numqubits == 1) {
		ret.push_back(*this); // trivial when num qubits == 1
		return ret;
	}
	if (numqubits != 2) // only factor 2 qubits for now
		return ret;
	
	// find highest amplitude
	U32 i,j,k;
	U32 mi,mj,mk;
	float maxprob = -1;
	for (j=0;j<2;++j) {
		for (i=0;i<2;++i) {
			k = 2*j + i;
			compf& amp = states[k];
			float p = getprob(amp);
			if (p > maxprob) {
				maxprob = p;
				mi = i;
				mj = j;
				mk = k;
			}
		}
	}
	if (maxprob < QEPSILON) {
		logger("qstate::factor prob was less than .2, should be .25 or greater\n");
		return ret;
	}
	const compf& mk0 = states[mk];
	const compf& mk1 = states[mk^1];
	const compf& mk2 = states[mk^2];
	const compf& mk3 = states[mk^3];
	compf rat0main = mk1/mk0;
	compf rat1main = mk2/mk0;
	//compf rat0alt = mk3/mk2;
	//compf rat1alt = mk3/mk1;
	compf mk30 = rat0main*mk2;
	compf mk31 = rat1main*mk1;
	if (!almostEqual(mk30,mk3)) {
		logger("can't be factored rat0\n");
		return ret;
	}
	if (!almostEqual(mk31,mk3)) {
		logger("can't be factored rat1\n");
		return ret;
	}
	// now factor
	qstate qb[2] = {qstate(1),qstate(1)};
	qb[0].states[0^mj] = 1; // highest
	qb[0].states[1^mj] = rat1main;
	qb[1].states[0^mi] = 1;
	qb[1].states[1^mi] = rat0main;
	qb[0].normalize();
	qb[1].normalize();
	qb[0].stdphase();
	qb[1].stdphase();
	ret.push_back(qb[0]);
	ret.push_back(qb[1]);
	return ret;
}

void qstate::clear()
{
	fill(states.begin(),states.end(),0.0f);
}

void qstate::init0()
{
	if (numstates) {
		clear();
		states[0] = 1;
	}
}

void qstate::initnm1()
{
	if (numstates) {
		clear();
		states[numstates-1] = 1;
	}
}

void qstate::setidx(U32 idx)
{
	if (numstates) {
		clear();
		if (idx < numstates)
			states[idx] = 1;
	}
}

void qstate::spread()
{
	if (numstates) {
		compf amp(1.0f/sqrtf((float)numstates));
		fill(states.begin(),states.end(),amp);
	}
}

void qstate::bell()
{
	static compf data[] = {
		SR2O2,
		0,
		0,
		SR2O2
	};
	load(data,NUMELEMENTS(data));
}

void qstate::test1()
{
	static compf data[] = {
		compf(10,3),
		compf(40,5),
	};
	load(data,NUMELEMENTS(data));
}

void qstate::test2()
{
	static compf data[] = {
		compf(2,3),
		compf(5,7)
	};
	load(data,NUMELEMENTS(data));
}

void qstate::test3()
{
	static compf data[] = {
		.6f,
		compf(0,.8f),
	};
	load(data,NUMELEMENTS(data));
}

void qstate::testmeasure1()
{
	static compf data[] = {
		sqrtf(.1f),
		compf(0,sqrtf(.2f)),
		-sqrtf(.3f),
		compf(0,-sqrtf(.4f)),
	};
	load(data,NUMELEMENTS(data));
}

// complete measurement of a quantum state (2^n qubits)
vector<float> qstate::measureall() const
{
	//logger("QSTATE '%s'\n",title.c_str());
	U32 i;
	U32 bits = ilog2(numstates);
	string bitfields = "\tmeasure ALL qubits     ";
	for (i=0;i<bits;++i)
		bitfields += itos(i);
	logger("%s\n",bitfields.c_str());
	for (i=0;i<numstates;++i) {
		const compf& e = states[i];
		float p = getprob(e);
		if (p >= QEPSILON)
			logger("\tprob  %8.5f       | %s>\n",p,idxtobinstring(i,bits).c_str());
	}
	//measureall();
	return vector<float>();
}

// measure just 1 qubit of quantum state and return 2 collapsed quantum states
vector<qstate> qstate::measure1(U32 qidx,float& prob0) const // probability of |0> of qubit(qidx)
{
	U32 i,j;
	U32 bits = ilog2(numstates);
	float p0 = 0;
	float p1 = 0;
	U32 qidxr = bits - 1 - qidx; // reverse order
	U32 bitpos = 1<<qidxr;
	for (i=0;i<numstates;++i) {
		const compf& e = states[i];
		float p = getprob(e);
		if (bitpos&i) {
			p1 += p;
		} else {
			p0 += p;
		}
	}
	// or p1 = 1 - p0, just checking p1 calc
	logger("\tmeasure1 qubit(%2d  |%s>) prob[0] %8.5f,   prob[1] %8.5f,   tot prob %8.5f\n",qidx,qubittobinstring(qidx,bits).c_str(),p0,p1,p0+p1);
	vector<qstate> ret(2);
	ret[0] = *this;
	ret[1] = *this;
	float mprob = p0;
	for (j=0;j<2;++j) { // measure comes up 0, then measure comes up 1
		float isqrtprob = 1.0f/sqrtf(mprob);
		for (i=0;i<numstates;++i) {
			bool selbit = (bitpos&i) != 0;
			if (selbit == (j != 0))
				ret[j].states[i] = states[i]*isqrtprob;
			else
				ret[j].states[i] = 0;
		}
		mprob = 1 - mprob; // p1
	}
	prob0 = p0;
	return ret;
}

#ifdef DONAMESPACE
} // end namespace qcomp
using namespace qcomp;
#endif
