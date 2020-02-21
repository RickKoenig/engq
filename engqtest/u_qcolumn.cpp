// qcolumn CLASS /////
// build an empty qcolumn

//#include <complex>

#include <m_eng.h>

#include "u_qstate.h"
#include "u_qmat.h"

#include "u_qgatebase.h"
#include "u_qgatederived.h"
#include "u_qcolumn.h"
#include "u_qfieldui.h"

qcolumn::qcolumn(U32 numqubits) : linkback(-1)
{
	//logger("qcolumn constructor called with %d maxqubits\n",maxqubits);
	U32 i;
	// make a column of defaults, PASSTHRU
	for (i=0;i<numqubits;++i) {
		qgates.push_back(qgatebase::build(qgatebase::PASSTHRU,i)); // id of 1
	}
	qmacc = qmat::qmatqu(numqubits); // no need to calc, it's an identity
	//calc();
}

// copy constructor
qcolumn::qcolumn(const qcolumn& rhs) : linkback(rhs.linkback) // copy constructor
{
	qmcol = rhs.qmcol;
	qmacc = rhs.qmacc;
	U32 i,n=rhs.qgates.size();
	// use the virtual copy method to deep copy the entire column over
	for (i=0;i<n;++i)
		qgates.push_back(rhs.qgates[i]->copy());
}

// assignment
qcolumn qcolumn::operator=(const qcolumn& rhs) // copy assignment
{
    if (this != &rhs) { // self-assignment check expected
		qmcol = rhs.qmcol;
		qmacc = rhs.qmacc;
		U32 i,n=qgates.size();
		// free old pointers
		for (i=0;i<n;++i) {
			delete(qgates[i]);
			// make new ones from old ones, qgatebase
			qgates[i] = rhs.qgates[i]->copy();
		}
		linkback = rhs.linkback;
    }
    return *this;
}

const qgatebase* qcolumn::getqgatecol(U32 cub) const
{
	return qgates[cub];
}

// load a qfield file, have id
/*void qcolumn::changeqgatecol(U32 qub,qgatebase::qtype qt,U32 ida)
{
	qgatebase* qgb = qgates[qub]; // get old gate
	delete(qgb); // free it
	qgb = qgatebase::build(qt,ida); // build new one
	qgates[qub] = qgb; // assign it
	//calc();
}
*/
// load a qfield file, have name of qgate
void qcolumn::changeqgatecol(U32 qub,const string& qname,U32 ida)
{
	qgatebase* qgb = qgates[qub]; // get old gate
	delete(qgb); // free it
	qgatebase::qtype qt = qgatebase::findqtypebyname(qname);
	qgb = qgatebase::build(qt,ida); // build new one
	qgates[qub] = qgb; // assign it
	//calc();
}

// drop a qgate, figure out id
void qcolumn::changeqgatecolautoid(U32 qub,qgatebase::qtype qt)
{
	qgatebase* qgb = qgates[qub]; // get old gate
	delete(qgb); // free it
	qgates[qub] = 0; // no gate here right now
	U32 ida = calcid(qt);
	//U32 ida = 7; // calc id MAGIC
	qgb = qgatebase::build(qt,ida); // build new one
	qgates[qub] = qgb; // assign it
	calc(); // build qcolumn matrix, flag errors as red
}

// find an empty unique id for that qgate qtype
U32 qcolumn::calcid(qgatebase::qtype qt) const
{
	vector<U32> idlist;
	U32 i,ng = qgates.size();
	qgatebase* newqg = qgatebase::build(qt);
	U32 newnumparts = newqg->getnumparts();
	U32 newcurpart = newqg->getcurpart();
	delete newqg;

	for (i=0;i<ng;++i) {
		qgatebase* qg = qgates[i];
		// if not null and same kind(class), bump the id only if there is same gate, if multi gate, then bump if same curpart number
		if (!qg)
			continue;
		if (qg->getkind() == qt ||
			//false) {
		     (newnumparts>1 && qg->getnumparts()>1 && newcurpart==qg->getcurpart())) { 
		  //(newnumparts>1 && qg->getnumparts()>1 && newcurpart==1 && qg->getcurpart()==1)) { 
				idlist.push_back(qg->getid());
			//}
		}
	}
	U32 cid = 0,nidlist = idlist.size();
	//U32 watchdog = 0;
	while(true) {
		//++watchdog;
		//if (watchdog == 100)
		//	errorexit("calcid watchdog!!");
		for (i=0;i<nidlist;++i) {
			if (idlist[i] == cid)
				break; // cid not found, that's the one to use
		}
		if(i == nidlist)
			break; // do it again if found
		++cid;
	}
	return cid;
}

qgatebase::qtype qcolumn::getkind(U32 idx) const
{
	return qgates[idx]->getkind();
}

U32 qcolumn::getid(U32 idx) const
{
	return qgates[idx]->getid();
}

void qcolumn::morequbits()
{
	U32 newsize = qgates.size() + 1;
	qgates.resize(newsize);
	qgates[newsize-1] = new qgatebase(calcid(qgatebase::PASSTHRU));
	calc();
}

void qcolumn::lessqubits()
{
	U32 newsize = qgates.size() - 1;
	delete qgates[newsize];
	qgates.resize(newsize);
	calc();
}

void qcolumn::draw(U32 xoff,U32 xscrolloff,bool center) const // top left
{
	U32 j;
	U32 numqubits = qgates.size();
	const U32 idoffset = 4;
	xoff -= xscrolloff;
#if 0
	// draw extra background column if hilit enabled
	if (hilit)
		cliprect32(noB32,xoff,qfield::fpixtop,qcolumn::cpixwid,numqubits*qgatebase::gpixhit,qfield::hili);
#endif
	// draw the gates, connections, outline and passthru first
	for (j=0;j<numqubits;++j) {
		const qgatebase* qg = qgates[j];
		U32 yoff = qfield::fpixtop + qgatebase::gpixhit*j;
		qg->drawoutline(B32,xoff,yoff);
		//qg->drawpassthru(xoff,yoff);
		//qg->drawconnect(xoff,yoff);
		//qg->draw(xoff + qg->getid()*4,yoff);
	}
	// draw the gates, connections, outline and passthru first
	for (j=0;j<numqubits;++j) {
		const qgatebase* qg = qgates[j];
		U32 yoff = qfield::fpixtop + qgatebase::gpixhit*j;
		//qg->drawoutline(xoff,yoff);
		qg->drawpassthru(B32,xoff,yoff);
		if (qg->getnumparts() > 1)
			qg->drawconnect(B32,xoff+ qg->getid()*idoffset,yoff);
		//qg->draw(xoff + qg->getid()*4,yoff);
	}
	// draw the box for multi gates
	for (j=0;j<numqubits;++j) {
		const qgatebase* qg = qgates[j];
		U32 yoff = qfield::fpixtop + qgatebase::gpixhit*j;
		//qg->drawoutline(xoff,yoff);
		//qg->drawpassthru(xoff,yoff);
		//qg->draw(xoff + qg->getid()*4,yoff);
		if (qg->hasmultibox() && qg->getnumparts() > 1 && qg->getcurpart() == 0)
			qg->drawboxm(B32,xoff + qg->getid()*idoffset,yoff);
	}
	// next layer, draw main gate
	for (j=0;j<numqubits;++j) {
		const qgatebase* qg = qgates[j];
		U32 yoff = qfield::fpixtop + qgatebase::gpixhit*j;
		//qg->drawoutline(xoff,yoff);
		//qg->drawpassthru(xoff,yoff);
		U32 ioff = idoffset;
		if (qg->getnumparts() > 1)
			ioff = 4;
		else
			ioff = 1;
		qg->draw(B32,xoff + qg->getid()*ioff,yoff);
	}
	// another layer, draw passthru again for DEC, not used in dec
	for (j=0;j<numqubits;++j) {
		const qgatebase* qg = qgates[j];
		U32 yoff = qfield::fpixtop + qgatebase::gpixhit*j;
		//qg->drawoutline(xoff,yoff);
		if (qg->getkind() == qgatebase::PASSTHRU)
			qg->drawpassthru(B32,xoff,yoff);
		//qg->draw(xoff + qg->getid()*4,yoff);
	}
	// draw the links
/*	U32 yoff = qfield::fpixtop + qgatebase::gpixhit * getnumqubits();
	//clipcircle32(noB32,xc,yc,qcolumn::cpixwid/10,C32YELLOW); // smaller solid control circle
	if (linkback >= 0) {
		U32 xc = xoff + qcolumn::cpixwid/2;
		U32 yc = yoff + 10;// + gpixhit/2;
		//outtextxyfc32(B32,xc,yc,C32BLUE,"L=%d",linkback);
		U32 xc2 = qfield::fpixleft + qcolumn::cpixwid*linkback - xscrolloff;
		U32 yc2 = yoff + 80;
		clipline32(B32,xc,yc,xc2,yc2,C32BLUE);
	}*/
}

	// draw the links
void qcolumn::drawlinkback(U32 xoff,U32 xscrolloff) const
{
	xoff -= xscrolloff;
	U32 yoff = qfield::fpixtop + qgatebase::gpixhit * getnumqubits();
	//U32 xc2 = qfield::fpixleft + qcolumn::cpixwid*linkback - xscrolloff;
	U32 xc = xoff + qcolumn::cpixwid/2;
	U32 yc = yoff + 10;// + gpixhit/2;
#define TEXT
#ifdef TEXT
	outtextxyfc32(B32,xc,yc,C32BLUE,"L=%d",linkback);
#else
	U32 xc2 = qfield::fpixleft + qcolumn::cpixwid*linkback + qcolumn::cpixwid/2 - xscrolloff;
	U32 yc2 = yoff + 80;
	clipline32(B32,xc,yc,xc2,yc2,C32BLUE);
#endif
}

// calc matrix and also set qgates colors if incomplete gates (errors, red)
void qcolumn::calc()
{
	U32 i,j,ng = qgates.size();
	qmcol = qmat(1<<ng);
	//return;
	//static qmat onequbit(2); // 1 qubit
	U32 numqubits = ng;
	qmat filler[maxiqubits+1] = {
		qmat::qmatqu(numqubits-0), // filler, max qubits
		qmat::qmatqu(numqubits-1), // filler, max qubits - 1, for simple gates
		qmat::qmatqu(numqubits-2), // filler, max qubits - 2, for 2 in/out gates
		qmat::qmatqu(numqubits-3), // filler, max qubits - 3, for 3 in/out gates
		qmat::qmatqu(numqubits-4), // filler, max qubits - 4, for 4 in/out gates
		qmat::qmatqu(numqubits-5), // filler, max qubits - 5, for 5 in/out gates
		qmat::qmatqu(numqubits-6), // filler, max qubits - 6, for 6 in/out gates
	};
	//return;
#if 0 // set qmat to something, test, enum of first gate goes to ele 3 2, test
	qgatebase*qg = qgates[0]; // top gate
	U32 kind = qg->getbasekind(); // kind maps to a value in the matrix, test
	qm.ele[3][2] = float(kind) * .01f; // show top gate in matrix, test
	//qmat nqm; // should be 1x1 matrix set to 1, 0 qubits
	qmat nqm = qmat::qmatqu(numqubits);
	//qmat onepassthru(2); // one qubit
#endif
	qmat nqm = qmat::qmatqu(numqubits);
	vector<U32> swapbits(numqubits);
	vector<bool> usedbits(numqubits);
	for (i=0;i<ng;++i) {
		qgatebase* qg = qgates[i]; // check to see if gate is 'whole'
		U32 curid = qg->getid(); // id of current 'gate' with all it's parts
		qgatebase::qtype kind = qg->getkind();
		qgatebase::qtype bkind = qg->getbasekind();
		U32 numparts = qg->getnumparts();
		U32 curpart = qg->getcurpart();
		vector<U32> parts(numparts); // count how many diff parts used, should all be '1'
		vector<U32> ypos(numparts); // which qubit this part is in, good for vertical lines, and calc gates qubit index position
		// see if gate is 'whole' by running thru 1 gate (single/multi)
		for (j=0;j<ng;++j) {
			qgatebase* qg2 = qgates[j];
			U32 curid2 = qg2->getid();
			qgatebase::qtype bkind2 = qg2->getbasekind();
			if (curid == curid2 && bkind == bkind2) {
				U32 curpart2 = qg2->getcurpart();
				++parts[curpart2];
				ypos[curpart2] = j;
			}
		}
		for (j=0;j<numparts;++j) {
			if (parts[j] != 1) {
				break;
			}
		}
		bool goodpart = j == numparts;
		qg->setdrawcolor(goodpart ? C32BLUE : C32RED);
		if (goodpart && curpart + 1 < numparts) { // connect first and last one
			//qg->setnextyoffset(ypos[numparts-1] - ypos[curpart]);
			qg->setnextyoffset(ypos[curpart+1] - ypos[curpart]);
		} else {
			qg->setnextyoffset(0);
		}
#if 0
		qmat sqm;
		// for now multi gate is ident
		if (qg->getnumparts() > 1)
			sqm = onequbit;
		else
			sqm = qg->getqmat();
		nqm = nqm^sqm;
#else
		for (j=0;j<numqubits;++j) {
			swapbits[j] = j;
			usedbits[j] = false;
		}
		// handle multi qubit gates
		if (goodpart && curpart == 0 && kind != qgatebase::PASSTHRU) {
			qmat sqm = qg->getqmat();
			qmat full = sqm^filler[numparts];
			for (j=0;j<numparts;++j) {
				//U32 t;
				//t = swapbits[j];
				//swapbits[j] = swapbits[ypos[j]];
				//swapbits[ypos[j]] = t;
				U32 dest = ypos[j];
				swapbits[j] = dest;
				usedbits[dest] = true;
			}
			// move passthru into old positions, (invertable)
			for(;j<numqubits;++j) {
				U32 u = 0;
				while(usedbits[u]) {
					++u;
				}
				swapbits[j] = u;
				usedbits[u] = true;
			}
			//swapbits[0] = i;
			//swapbits[i] = 0;
			full.swapqubits(swapbits);
			nqm = nqm*full;
		}
#endif
		//qm = qg->getqmat();
		//string title = string("row = ") + qstate::itos(i) + " " + qgatebase::qgnames[qg->getkind()];
		//qm.print(title,false,qcon);
	}
	qmcol = nqm;
}

qcolumn::~qcolumn()
{
	// free qgate pointers in this column
	U32 i;
	for (i=0;i<qgates.size();++i)
		delete(qgates[i]);
}
