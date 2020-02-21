// simple tests and one time utils
//#include <complex>

#include <m_eng.h>
#include <l_misclibm.h>

#include "u_states.h"

#include "u_qstate.h"
#include "u_qmat.h"

#include "u_qgatebase.h"
#include "u_qgatederived.h"
#include "u_qcolumn.h"
#include "u_qfieldui.h"

//#include "testclassfactory.h"

//#define BUILD_GATE_ART
#define LOG_SORT_PREBUILT_TXT // good for syncing up with web version's prebuilt.txt

#define DONAMESPACE
#ifdef DONAMESPACE
namespace qcomp {
#endif

// UI
shape* rl;
shape* focus2,*oldfocus;

listbox* lfiles;
edit* efile;
pbut* bload,*bsave,*bdelete,*breset;
//edit* econtents;

listbox* lqgates;

text* tcurmode;
pbut* bmorequbit;
pbut* blessqubit;
pbut* bcolmode;
pbut* baccmode;
pbut* bstatemode;
hscroll* hqfxoffset;
//const S32 numhpos = qfield::maxcolumns + qfield::fpixwid + qfield::leftlabels + qcolumn::cpixwid;//3000; // fine grain slider
const S32 hposfactor = 4;
const S32 numhpos = (qcolumn::cpixwid*qfield::maxcolumns - qfield::fpixwid + qfield::leftlabels)/hposfactor;// - qfield::fpixwid + qfield::leftlabels;//3000; // fine grain slider

pbut* bquit;
con32* qcon;

// qfield (circuit) UI
qfield* qf;
qgatebase* qgcur; // cursor with a qgate
S32 lastshowcol;
const S32 invalidcol = -2; // because -1 is valid for ACC and STATE, not COL
// links
S32 linkfrom;//,linkto;

enum showmode {COL,ACC,STATE};
showmode sm;
const C8* modestr[] = {"MATRIX COL","MATRIX ACC","Q STATE"};


bool playagame(const qmat& amat,const qmat& bmat,const qstate& ent,U32 rowidx,U32 colidx)
{
	const C8 *refa[] = {"TOP","MIDDLE","BOTTOM"};
	const C8 *refb[] = {"LEFT","MIDDLE","RIGHT"};
	logger("Ref picks ALICE %s row, BOB %s column\n",refa[rowidx],refb[colidx]);
	qmat circuit = amat^bmat;
	qstate resultgame = circuit*ent;
	U32 i,j,k;
	bool allpass = true;
	for (k=0;k<resultgame.getnumstates();++k) {
		// start measuring, use all possible measurements
		const compf& amp = resultgame.getnumstate(k);
		float nrm = norm(amp);
		// if some probability exists (equal)
		U32 gameboard[3][3];
		fill(gameboard[0],gameboard[3],0);
		if (nrm >= QEPSILON) {
			logger("\n");
			U32 lns = ilog2(ent.getnumstates());
			logger("found an amp, prob %10.5f, | %s>\n",nrm,qstate::idxtobinstring(k,lns).c_str());
			//| A0 A1 B0 B1>
			U32 abits = (k>>2)&3;
			U32 bbits = k&3;
			// | A0 A1 >
			switch(abits) {
				case 0: // 0
					break;
				case 1: // 2    Q1 set Q0 not, set col 1 and 2
					gameboard[rowidx][1] = 1;
					gameboard[rowidx][2] = 1;
					break;
				case 2: // 1    Q0 set Q1 not, set col 0 and 2
					gameboard[rowidx][0] = 1;
					gameboard[rowidx][2] = 1;
					break;
				case 3: // 3	Q0 and Q1 set, set col 0 and 1
					gameboard[rowidx][0] = 1;
					gameboard[rowidx][1] = 1;
					break;
			}
			// | B0 B1 >
			switch(bbits) {
				case 0: // 0
					break;
				case 1: // 2    Q1 set Q0 not, set row 1 and 2
					gameboard[1][colidx] |= 2;
					gameboard[2][colidx] |= 2;
					break;
				case 2: // 1    Q0 set Q1 not, set row 0 and 2
					gameboard[0][colidx] |= 2;
					gameboard[2][colidx] |= 2;
					break;
				case 3: // 3	Q0 and Q1 set, set row 0 and 1
					gameboard[0][colidx] |= 2;
					gameboard[1][colidx] |= 2;
					break;
			}
			U32 keysqr = gameboard[rowidx][colidx];
			bool pass = keysqr == 1 || keysqr == 2;
			logger("board %s\n",pass?"PASS":"FAIL");
			allpass = allpass && pass;
			for (j=0;j<3;++j) {
				for (i=0;i<3;++i) {
					logger("%c%d ",j==rowidx && i==colidx ? '>' : ' ',gameboard[j][i]);
				}
				logger("\n");
			}
		}
	}
	return allpass;
}

void playgames()
{
#if 1
	U32 i,j;

	qmat cnot;
	cnot.cnot();

	qmat split;
	split.splitter();

	qmat ident1;
	ident1.ident1();

	qmat swap;
	swap.swap();

	qmat had;
	had.hadamard();

	qmat snot;
	snot.snotto();

	qmat cnotswap = cnot;
	cnotswap.swapqubits(1,0);
	cnotswap.print("CNOTSWAP");

	qmat dec;
	dec.dec();

	qmat not1;
	not1.not1();

//#define TWISTED_OAK_WAY
#define MERMIN_WAY
#ifdef TWISTED_OAK_WAY
	// brute force circuits
	// ALICE
	// top row
	qmat toprow = swap*cnot*(split^ident1)*cnot;
	toprow.print("ALICE TOP ROW");

	// middle row
	qmat middlerow = cnot*(ident1^had)*cnotswap*(split^ident1);
	middlerow.print("ALICE MIDDLE ROW");

	// bottom row
	qmat botrow = (had^ident1)*cnot*(had^ident1);
	botrow.print("ALICE BOT ROW");

	// BOB
	// left column
	qmat leftcolumn = swap*(split^split)*cnotswap;
	leftcolumn.print("BOB LEFT COLUMN");

	// middle column
	qmat middlecolumn = (split^ident1)*cnot*(ident1^split);
	middlecolumn.print("BOB MIDDLE COLUMN");

	// right column, dec then snot on the bottom
	qmat rightcolumn = (ident1^snot)*dec;
	rightcolumn.print("BOB RIGHT COLUMN");
#endif
#ifdef MERMIN_WAY
	// circuits from Mermin magic square
	// ALICE
	// top row
	qmat toprow = swap*(had^had);
	toprow.print("ALICE TOP ROW");

	// middle row
	qmat middlerow = qmat(4); // no gates !!
	middlerow.print("ALICE MIDDLE ROW");

	// bottom row
	qmat botrow = (not1^not1)*(had^ident1)*cnot*(had^ident1);
	botrow.print("ALICE BOT ROW");

	// BOB
	// left column
	qmat leftcolumn = (not1^not1)*swap*(ident1^had);
	leftcolumn.print("BOB LEFT COLUMN");

	// middle column
	qmat middlecolumn = (not1^not1)*(had^ident1);
	middlecolumn.print("BOB MIDDLE COLUMN");

	// right column
	qmat rightcolumn = (not1^not1)*(had^ident1)*cnot;
	rightcolumn.print("BOB RIGHT COLUMN");
#endif

	// make 2 pairs of entangled qubits, Q0,Q2 and Q1,Q3
	qstate entang2(4);//16);
	entang2.init0();
	qmat had04 = had^ident1^ident1^ident1;
	qmat had14 = ident1^had^ident1^ident1;
	qmat cnot023 = cnot^ident1;
	cnot023.swapqubits(0,2,1);
	qmat cnot024 = cnot023^ident1;
	qmat cnot134 = ident1^cnot023;
	entang2 = cnot024*cnot134*had04*had14*entang2;
	entang2.print("ENTANG2");

	// play out ALICE bottom, BOB left
	qmat circuit = botrow^leftcolumn;
	circuit.print("CIRCUIT ALICE bottom, BOB left",true);

	// Q0 and Q1 go to ALICE, Q2 and Q3 go to BOB
	qstate resultgame = circuit*entang2;
	resultgame.print("RESULTGAME");

	logger("\nplay some games\n\n");
	bool globalpass = true;
	for (j=0;j<3;++j) {
		for (i=0;i<3;++i) {
			const qmat* rowmats[] = {&toprow,&middlerow,&botrow};
			const qmat* colmats[] = {&leftcolumn,&middlecolumn,&rightcolumn};
			bool resultpos = playagame(*rowmats[j],*colmats[i],entang2,j,i);
			globalpass = globalpass && resultpos;
			logger("\n");
			logger(resultpos?"ALL PASS":"SOME FAIL");
			logger("\n\n\n");
		}
	}
	logger("globalresult = %s\n",globalpass?"EVERYTHING PASSES!!!":"SOMETHING FAILED!");
	logger("\n\n");
#endif
}

// laid back VS needy, You don't know how Quantum Computers work!
void guess_gate()
{
	logger("----------- GUESS GATE -------------\n");
	const U32 numqubits = 2;
	const U32 numstates = 1<<numqubits;
	// 4 pure states and 1 state with qubit 0 Right, qubit 1 Up(0)
	vector<qstate> qpstates;
	qstate qsi(numqubits);
	U32 i,j;
	for (i=0;i<numstates;++i) {
		qsi.setidx(i);
		qpstates.push_back(qsi);
		//qs.print("pure states",false);
	}
	// 4 gates, 2 laid back, 2 needy
	qmat ident;
	ident.ident1(); // 1 qubit
	qmat cnot;
	cnot.cnot(); // 2 qubits
	qmat noti;
	noti.not1(); // 1 qubit
	qmat had;
	had.hadamard(); // 1 qubit

	// add 5th qstate to list
	qsi.init0();
	qstate leftup = (had^ident)*qsi;
	qpstates.push_back(leftup);

	vector<qmat> qgates;
	qmat qgate;
	// 2 qubit identity, laid back
	qgate = ident^ident;
	qgates.push_back(qgate);
	// 2 qubit flip qubit 1 (of 0 to 1), laid back
	qgate = ident^noti;
	qgates.push_back(qgate);
	// cnot, needy
	qgates.push_back(cnot);
	// cnot then flip qubit 1, needy
	qgate = (ident^noti)*cnot;
	qgates.push_back(qgate);

	// run the 4 gates with the 4 states
	for (j=0;j<qpstates.size();++j) {
		qstate qsi = qpstates[j];
		logger("------------ state %d  ---------\n",j);
		for (i=0;i<qgates.size();++i) {
			qmat qg = qgates[i];
			qstate qso = qg*qsi;
			logger("--- gate %d ---\n",i);
			qsi.print("input state",false);
			qso.print("output state",false);
		}
	}

	// run right,left thru 4 gates, then thru 2 hadamards
	logger("--- left right thru gates then thru 2 hadamards ---\n");
	qmat had2 = had^had;
	qstate rightleft(2);
	rightleft.setidx(1); // should now be |01>
	rightleft.print("up down should be |01>",false);
	rightleft = had2*rightleft;
	rightleft.print("right left",false);

	for (i=0;i<qgates.size();++i) {
		qmat qg = qgates[i];
		qstate qso = qg*rightleft;
		logger("--- gate %d ---\n",i);
		rightleft.print("input state",false);
		qso.print("output state",false);
		qso = had2*qso;
		qso.print("output state thru had2",false);
	}
}

void flip_left_and_right()
{
	logger("----- test flip left and right -----\n");
	qmat had;
	had.hadamard();
	qstate up(1);
	up.init0();
	qstate down(1);
	down.initnm1();
	qstate right = had*up;
	qstate left = had*down;
	up.print("up",false);
	down.print("down",false);
	right.print("right",false);
	left.print("left",false);
	qmat flip;
	flip.not1();
	qstate upf = flip*up;
	qstate downf = flip*down;
	qstate rightf = flip*right;
	qstate leftf = flip*left;
	upf.print("up flip",false);
	downf.print("down flip",false);
	rightf.print("right flip",false);
	leftf.print("left flip",false);
}

void testFactors()
{
	// test factoring qstate into separate qubits
	logger("---------- try factoring some qstates ----------\n");
#if 1

	compf qbas[][2] = {
		{-1,0},
		{compf(0,-1),0},
		{0,-1},
		{0,compf(.7071f,.7071f)},
		{-SR2O2,SR2O2},
		{SR2O2,SR2O2},
		{1,0},
		{0,1},
		{SR2O2,SR2O2},
		{0,compf(0,1)},
	};
	compf qbbs[][2] = {
		{1,0},
		{0,1},
		{1,0},
		{0,1},
		{1,0},
		{0,1},
		{SR2O2,SR2O2},
		{SR2O2,SR2O2},
		{SR2O2,SR2O2},
		{0,compf(0,1)},
	};
	U32 i;
	for (i=0;i<NUMELEMENTS(qbas);++i) {
		logger("...... factor test %2d ......\n",i);
		// test qubit 0
		qstate qb0;
		qb0.load(qbas[i],NUMELEMENTS(qbas[i]));
		qb0.print("QB0",false);
		// test qubit 1
		qstate qb1;
		qb1.load(qbbs[i],NUMELEMENTS(qbbs[i]));
		qb1.print("QB1",false);
		// tensor product
		qstate qb01 = qb0^qb1;
		qb01.print("QB0 X QB1",false);
		vector<qstate> factors = qb01.factor();
		printfactors(factors,"factors QB0 X QB1");
		//logger("\n");
	}
	logger("..... try to factor bell state .....\n");
	qstate qs;
	qs.bell();
	qs.print("try to factor bell state",false);
	vector<qstate> factors = qs.factor();
	printfactors(factors,"bell state");
#endif
}

// test the quantum computer
void qcomptest()
{
	//U32 i,j;
	qmat px;
	px.print("EMPTY");
	px.pauliX();
	px.print("PX");

	qmat py;
	py.pauliY();
	py.print("PY");

	qmat pz;
	pz.pauliZ();
	pz.print("PZ");

	qmat pxtpy = px*py;
	pxtpy.print("PXTPY");

	qmat ident1(4);
	ident1.print("FOUR");
	ident1.identn();
	ident1.print("IDENTN");
	ident1.ident1();
	ident1.print("IDENT1");

	qmat had;
	had.hadamard();
	had.print("HADAMARD");

	qmat split;
	split.splitter();
	split.print("SPLITTER");

	qmat snot;
	snot.snotto();
	snot.print("SNOT");

	qmat cnot;
	cnot.cnot();
	cnot.print("CNOT");
	qmat cnotswap = cnot;
	cnotswap.swapqubits(1,0);
	cnotswap.print("CNOTSWAP");

	qmat swap;
	swap.swap();
	swap.print("SWAP");

	qmat dec;
	dec.dec();
	dec.print("DEC");
	qmat decswap = dec;
	// swapqubits FIX maybe
	decswap.swapqubits(1,0);
	decswap.print("DEC SWAP");
	dec = qmat(8);
	dec.swapqubits(0,1,2);
	dec.swapqubits(1,0,2);
	dec.swapqubits(0,2,1);
	dec.swapqubits(2,1,0);
	dec.swapqubits(1,2,0);
	dec.swapqubits(2,0,1);


	qmat qmtest1;
	qmtest1.test1();
	qmtest1.print("TEST QMAT1");

	qmat qmtest2;
	qmtest2.test2();
	qmtest2.print("TEST QMAT2");

	qstate up;
	const compf qup[] = {1,0};
	up.load(qup,2);
	up.print("UP");

	qstate down;
	const compf qdown[] = {0,1};
	down.load(qdown,2);
	down.print("DOWN");

	// FIX make it print the |ab> state
	qstate pure(2);//4);
	pure.init0();
	pure.print("PURE UP");
	pure.initnm1();
	pure.print("PURE DOWN");
	pure.setidx(2);
	pure.print("PURE IDX 2"); // set to |01> (bits are reversed)

	qstate spread(3);//8);
	spread.spread();
	spread.print("SPREAD 8");
	spread = qstate(2);//4);
	spread.spread();
	spread.print("SPREAD 4");

	qstate bell = spread;
	bell.print("NOT YET BELL, STILL SPREAD");
	bell.bell();
	bell.print("BELL");

	qstate qstest1;
	qstest1.test1();
	qstest1.print("TEST QSTATE1");

	qstate qstest2;
	qstest2.test2();
	qstest2.print("TEST QSTATE2");

	qstest2.test3();
	qstest2.print("TEST QSTATE3");

	qmtest1.print("TEST QMAT1");
	qmtest2.print("TEST QMAT2");
	qstate result = qmtest1*qstest1;
	result.print("QMAT1*QSTATE1 = RESULT");

	compf resf = qstest1*qstest2;
	printcompf("resf q1*q2",resf);
	resf = qstest2*qstest1;
	printcompf("resf q2*q1",resf);

	qstate resqs = qstest1^qstest2;
	resqs.print("qs1 X qs2");
	resqs = qstest2^qstest1;
	resqs.print("qs2 X qs1");

	qmat qmouter = qmtest1^qmtest2;
	qmouter.print("qm1^qm2");
	qmouter = qmtest2^qmtest1;
	qmouter.print("qm2^qm1");

	// what is the convention of ordering qubits, top to bottom or bottom to top???
	qmat hadnone = ident1^ident1;
	hadnone.print("had none");
	qmat hadtop = had^ident1; // 2 qubits had on top ident on bottom
	hadtop.print("had top");
	qmat hadbot = ident1^had; // 2 qubits had on top ident on bottom
	hadbot.print("had bot");
	qmat hadboth = had^had; // 2 qubits had on top ident on bottom
	hadboth.print("had both");
	qmat hadtopswap = hadtop;
	hadtopswap.swapqubits(1,0);
	hadtopswap.print("had top swap");

	// 2 qubits, make an entangled qstate
	qstate zer(2);//4);
	zer.init0();
	qmat hadt = had^ident1;
	qstate step1 = hadt*zer;
	step1.print("step1");
	//qmat cnotb = cnot;
	//cnotb.swapqubits(1,0);
	//qstate ent = cnot*step1;
	qstate ent = cnot*step1;
	ent.print("entangle");

	// test swapqubits
	// std control pin0, data pin1 (Q0,Q1)
	qmat cnot3 = cnot^ident1;
	cnot3.print("CNOT3");
	// swap pins 0 and 1
	qmat cnot3s01 = cnot3;
	cnot3s01.swapqubits(1,0,2);
	cnot3s01.print("CNOT3 swap 0 and 1");
	// move CNOT down so Q0 is pass thru, Q1 is control, Q2 is data
	qmat cnot3s120 = cnot3;
	cnot3s120.swapqubits(1,2,0); // 0 to 1, 1 to 2, 2 to 0
	cnot3s120.print("CNOT3 move down");
	// move CNOT control from Q0 to Q2
	qmat cnot3s210 = cnot3;
	cnot3s210.swapqubits(2,1,0); // 0 to 2,1 same, 2 to 0
	cnot3s210.print("CNOT3 210");
	// move CNOT data from Q1 to Q2
	qmat cnot3s021 = cnot3;
	cnot3s021.swapqubits(0,2,1); // 0 to same, 1 to 2, 2 to 1
	cnot3s021.print("CNOT3 021");
	// move CNOT control from Q0 to Q2 and data fron Q1 to Q0
	qmat cnot3s201 = cnot3;
	cnot3s201.swapqubits(2,0,1); // 0 to 2,1 to 0, 2 to 1
	cnot3s201.print("CNOT3 201");

	// test more swapqubits
	dec.dec(); // reset back to original dec
	qmat dec3 = dec^ident1;
	dec3.print("DEC3");
	qmat dec3down = ident1^dec;
	dec3down.print("DEC3DOWN");
	qmat dec3swap120 = dec3;
	dec3swap120.swapqubits(1,2,0);
	dec3swap120.print("DEC3SWAP120");
	qmat dec3swap021 = dec3;
	dec3swap021.swapqubits(0,2,1);
	dec3swap021.print("DEC3SWAP021");

	logger("\nsimple\n");
	qstate start0(2);//4); // 2 qubit state
	start0.init0();
	qmat anot;
	anot.not1();
	qstate qbit00 = (ident1^ident1)*start0;
	qstate qbit01 = (ident1^anot)*start0;
	qstate qbit10 = (anot^ident1)*start0;
	qstate qbit11 = (anot^anot)*start0;
	qbit00.print("QBIT |00>");
	qbit01.print("QBIT |01>");
	qbit10.print("QBIT |10>");
	qbit11.print("QBIT |11>");

	logger("\ntest measure 1 qubit\n");
	qstate meas1;
	meas1.testmeasure1();
	meas1.print("MEASURE1");

	//guess_gate();
}

void refreshfilelist()
{
	U32 i;
	/*for (i=0;i<=100;++i) {
		C8 s[100];
		sprintf(s,"%3d * %3d = %5d",i,i,i*i);
		lfiles->addstring(s);
	}*/
	S32 oldidx = lfiles->getidx();
	lfiles->clear();
	scriptdir qdir(false);
	U32 n = qdir.num();
	for (i=0;i<n;++i) {
		const C8* fnameext = qdir.idx(i).c_str();
		C8 ext[200];
		mgetext(fnameext,ext,200);
		if (!my_stricmp(ext,"qcmp")) {
			C8 fname[200];
			fname[0] = '\0';
			mgetname(fnameext,fname);
			lfiles->addstring(fname);
		}
	}
	lfiles->setidxc(oldidx);
}

void changecursorqgate(U32 gt)
{
	delete qgcur;
	if (gt > 0) {
		qgcur = qgatebase::build(qgatebase::qtype(gt));
	} else {
		qgcur = 0;
	}
}

void loadqgatelist()
{
	U32 i;
	for (i=0;i<qgatebase::QTYPEENUM;++i) {
		lqgates->addstring(qgatebase::qgnames[i]);
	}
	qgatebase::qtype qt = qgatebase::HADAMARD; // startup cursor gate
	lqgates->setidxc(qt);
	changecursorqgate(qt);
}

void savelastfile(const string& lf)
{
	FILE* fw = fopen2("lastfile.txt","w");
	if (fw) {
		fprintf(fw,"\"%s\"\n",lf.c_str());
		fclose(fw);
	}
}

qgatebase::qtype nextcursorqgate()
{
	if (!qgcur)
		return qgatebase::qtype(0);
	U32 cp = qgcur->getcurpart();
	U32 mp = qgcur->getnumparts();
	++cp;
	if (cp == mp)
		cp = 0;
	return qgatebase::qtype(qgcur->getbasekind() + cp);
}

void updatemode()
{
	tcurmode->settname(modestr[sm]);
	lastshowcol = invalidcol;
}

#if 0
void con32_clear_title(con32* qcon)
{
	con32_clear(qcon);
	// show link status
	if (linkfrom >= 0)
		con32_printf(qcon,"--------------- Link status from %d -------------\n",linkfrom);
}
#endif

void resetui()
{
	lastshowcol = invalidcol;
	linkfrom = -1;
}

void printtoconsole(S32 cl)
{
	qmat qm;
	U32 nqb  = qf->getnumqubits();
	qstate qsin,qsout;
	if (cl >= qfield::maxcolumns)
		cl = qfield::maxcolumns - 1;
	if (cl != lastshowcol) { // only update when changed
		con32_clear(qcon);
		// show link status
		if (linkfrom >= 0)
			con32_printf(qcon,"--------------- Link status from %d -------------\n",linkfrom);
		float tb;
		compf qmdet;
		bool qmsing = false;
		switch(sm) {
		case STATE:
			con32_printf(qcon,"CUR STATE = %d   ",cl);
			qf->sethilitpos(cl,true);
			//con32_clear_title(qcon);
			qm = qf->getqmatacc(cl);
			//nqb = qm.getnumqubits();
			qsin = qstate(nqb);
			qsin.init0();
			qsout = qm*qsin;
			tb = qsout.gettotprob();
			if (tb < QEPSILON) {
				con32_printf(qcon,"MEASUREMENT PROBABILITY EQUALS ZERO  ");
				break;
			}
			qmdet = qm.det();
			if (getprob(qmdet) < QEPSILON)
				qmsing = true;
			if (qmsing || tb < 1 - QEPSILON) {
				con32_printf(qcon,"MEASUREMENT PROBABILITY = %f  ",tb);
				qsout.normalize();
			}
			con32_printf(qcon,"\n");
			if (nqb <= 6) {
				qsout.print(modestr[sm],false,qcon);
				vector<qstate> factors;
				if (nqb != 1)
					factors = qsout.factor();
				printfactors(factors,"factors CUR STATE\n",qcon);
				if (factors.empty() && nqb == 2) {
					//if (qsout.gettotprob() > QEPSILON) {
						con32_printf(qcon,"\nENTANGLED");
					//} else {
					//	con32_printf(qcon,"\nNOT ENTANGLED!");
					//}
				}
			} else {
				U32 dim = 1<<qm.getnumqubits();
				con32_printf(qcon,"%s, state %d, too big !!!",modestr[sm],dim);
			}
			break;
		case COL:
			qf->sethilitpos(cl,false);
			//con32_clear_title(qcon);
			con32_printf(qcon,"CUR COLUMN = %d\n",cl);
			qm = qf->getqmatcol(cl);
			//nqb = qm.getnumqubits();
			if (nqb <= 4) {
				qm.print(modestr[sm],false,qcon);
			} else {
				U32 dim = 1<<qm.getnumqubits();
				con32_printf(qcon,"%s, matrix %d %d, too big !!!",modestr[sm],dim,dim);
			}
			break;
		case ACC:
			qf->sethilitpos(cl,true);
			//con32_clear_title(qcon);
			con32_printf(qcon,"CUR ACCUMULATE = %d\n",cl);
			qm = qf->getqmatacc(cl);
			//nqb = qm.getnumqubits();
			if (nqb <= 4) {
				qm.print(modestr[sm],false,qcon);
			} else {
				U32 dim = 1<<qm.getnumqubits();
				con32_printf(qcon,"%s, matrix %d %d, too big !!!",modestr[sm],dim,dim);
			}
			break;
		}
		lastshowcol = cl;
	}
}

#ifdef BUILD_GATE_ART
// draw the bitmaps of all the gates to somewhere (screen, files, etc.)
void buildGateArt()
	{
	//qgatebase::qtype qt = qgatebase::findqtypebyname(qname);
	S32 split = 10;
	for (S32 it = 0 ; it < qgatebase::QTYPEENUM ; ++it) {
		S32 i = it % split;
		S32 j = it / split;
		qgatebase::qtype qt = qgatebase::qtype(it);
		qgatebase* qgb = qgatebase::build(qt,0); // build new one
		qgb->setdrawcolor(C32BLUE);
		//qgates[qub] = qgb; // assign it
		bitmap32* savebm = bitmap32alloc(qcolumn::cpixwid,qgatebase::gpixhit,C32(0,0,0,0));
		i = j = 0;
		qgb->draw(savebm,0,0);
		const C8* savename = qgatebase::qgnames[it];
		//savename += ".png";
		string sn = savename;
		replace(sn.begin(),sn.end(),' ','_');
		gfxwrite32((sn + ".png").c_str(),savebm);
		bitmap32free(savebm);
		delete qgb;
		//calc();
	}
}
#endif

#ifdef DONAMESPACE
} // end namespace qcomp

using namespace qcomp;
#endif

void atest();
void qcompinit()
{
	video_setupwindow(1366,768);
	pushandsetdir("qcomp");

#if 1
// test normalize of hadamard eigenvectors
	logger("test normalize...\n");
//	pointf2 ev1 = pointf2x(3,4); // an unnormalized hadamard eigenvector ..
	pointf2 ev1 = pointf2x(1.0f,SR2 - 1.0f); // an unnormalized hadamard eigenvector ..
	logger("ev1 = (%f,%f)\n",ev1.x,ev1.y);
	float lensq = len2dsq(&ev1);
	float len = sqrtf(lensq);
	logger("ev1 lensq = %f, len = %f\n",lensq,len);
	pointf2 ev1n = pointf2x(ev1.x/len,ev1.y/len);
	logger("ev1n = (%f,%f)\n",ev1n.x,ev1n.y);
	float lensqn = len2dsq(&ev1n);
	logger("ev1n lensqn = %f\n",lensqn);
#endif

// ui
	sm = STATE;
	qcon = con32_alloc(WX,180,C32BLACK,C32(200,255,255));
	rl = res_loadfile("qcompres.txt");
	lfiles = rl->find<listbox>("LFILES");
	efile = rl->find<edit>("EFILE");
	bload = rl->find<pbut>("BLOAD");
	bsave = rl->find<pbut>("BSAVE");
	bdelete = rl->find<pbut>("BDELETE");
	breset = rl->find<pbut>("RESET");
	lqgates = rl->find<listbox>("LQGATES");
	tcurmode = rl->find<text>("SHOWMODE");
	bmorequbit = rl->find<pbut>("MOREQB");
	blessqubit = rl->find<pbut>("LESSQB");
	bcolmode = rl->find<pbut>("MATCOL");
	baccmode = rl->find<pbut>("MATACC");
	bstatemode = rl->find<pbut>("QSTATE");
	hqfxoffset = rl->find<hscroll>("SLIDEQFXOFFSET");
	//hqfxoffset->setoffset(10);
	hqfxoffset->setnumidx(numhpos);
//	econtents = rl->find<edit>("ECONTENTS");
	bquit = rl->find<pbut>("BQUIT");
	focus2 = lfiles;
	oldfocus = 0;
	refreshfilelist(); // update listbox from file directory
	qgcur = 0;
	loadqgatelist();
	resetui();
	updatemode();
#if 0
#define DOLOAD
#ifdef DOLOAD
	qf = new qfield("testcircuit1.qcmp");
#else
	qf = new qfield();
	qgatebase::qtype qt = qgatebase::HADAMARD;//(qgate::HADAMARD);
	qf->changeqgate(0,0,qt);
	qf->changeqgate(2,1,qt);
	qf->changeqgate(2,2,qgatebase::SPLITTER);
	qf->changeqgate(2,3,qt);
	qf->changeqgate(4,2,qt);
	qf->changeqgate(7,3,qt);
	qf->changeqgate(9,2,qt);
	qf->changeqgate(1,0,qgatebase::SPLITTER);
	qf->changeqgate(2,0,qgatebase::SNOT);
#endif
	qf->copycolumn(2,5);
#else
	bool go = false;
	script* lf = new script("lastfile.txt");
	string fname;
	if (lf->num() == 1) {
		fname = lf->read();
		if (fileexist(fname.c_str()))
			go = true;
	}
	if (go) {
		qf = new qfield(fname,-1);
		C8 name[100];
		mgetname(fname.c_str(),name);
		efile->settname(name);
		S32 idx = lfiles->findstring(name);
		if (idx >=0)
			lfiles->setidxc(idx);
	} else {
		qf = new qfield(qcolumn::defaultqubits);
	}
	delete lf;
	//qf->setxoffset(200);
#endif
	//qgcur = 0;//qgatebase::build(qgatebase::HADAMARD);

	logger("start qcomp ---------->>>>>>>>>>>>\n");
// more pronounced
	logger_unindent();

// unit tests
	//atest();
	//testClassFactory();
	//qcomptest();
	//flip_left_and_right();
	//guess_gate();
	//testFactors(); // try to factor qstates,    whoa, play with camel case
	playgames();
#if 0
	qmat had;
	had.hadamard();
	qmat id;
	id.ident1();
	//qmat m4 = had^had^had^had;
	qmat m4 = had^id^id^id;
	m4.print("some hads, up to 4",false,qcon);
#endif
#if 0
	S32 i;
	for (i=-10000;i<=10000;i+=5) {
		float f = i/10000.0f;
		C8 fs[50];
		smallfloat(fs,f);
		logger("small float of '%8.5f' = '%s'\n",f,fs);
	}
#endif
#if 1
#ifdef BUILD_GATE_ART
	buildGateArt();
#endif
#ifdef LOG_SORT_PREBUILT_TXT
	{
		script sc("prebuilt.txt");
		sc.sort();
		S32 i,n=sc.num();
		logger("prebuilt.txt\n");
		for (i=0;i<n;++i)
			logger("idx[%4d] '%s'\n",i,sc.idx(i).c_str());

	}
#endif

#endif
}

void qcompproc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case K_RIGHT:
	case K_LEFT:
		focus2 = hqfxoffset;
		break;
	}
// proc ui
	if (wininfo.mleftclicks)
		focus2=rl->getfocus();
	S32 ret=-1;
	if (focus2) {
		ret=focus2->proc();
	}
	if (oldfocus && oldfocus!=focus2)
		oldfocus->deactivate();
	oldfocus=focus2;
	if (ret == 1) {
	// quit
		if (focus2 == bquit) {
			poporchangestate(STATE_MAINMENU);
	// load file
		} else if (focus2 == bload) {
#if 1
			//hqfxoffset->setidx(0);
			S32 idx = lfiles->getidx();
			if (idx >= 0) {
				const C8* idxname = lfiles->getidxname();
				efile->settname(idxname);
				string fileextname = string(idxname) + ".qcmp";
				const C8* fname = fileextname.c_str();
				if (fileexist(fname)) {
					// switch UI to qstate mode for this newly loaded file
					//sm = STATE;
					S32 lasthlp = qf->gethilitpos(); // from last qfield, last hilight point, try to use on new load
					if (lasthlp < 0)
						lasthlp = 16;
					delete qf; // remove old qfield
					qf = new qfield(fname,lasthlp);
					S32 xoff = lasthlp*qcolumn::cpixwid - 15*qfield::fpixwid/16;
					if (xoff > 0)
						qf->setxoffset(xoff);
					//qf->sethilitpos(lasthlp+1,false);
					//hqfxoffset->setidx(hqfxoffset->getnumidx()*lasthlp/qcolumn::cpixwid); // this is wrong
					C8 qcontents[200];
					sprintf(qcontents,"loaded %s with %d columns and %d qubits",fname,qfield::maxcolumns,qf->getnumqubits());//qcolumn::maxqubits);
					savelastfile(fileextname);
					// fire off a console update
					resetui();
					printtoconsole(lasthlp);
				} else {
					qf = new qfield(qcolumn::defaultqubits);
				}
				//lastshowcol = invalidcol;
			}
			focus2 = lfiles;
			//resetui();
			//lastshowcol = invalidcol; // update console
			updatemode();
			qf->fcalc();
			lastshowcol = invalidcol; // update console
#endif
	// save file
		} else if (focus2 == bsave) {
			//const C8* idxname = lfiles->getidxname();
			//efile->settname(idxname);
			const C8* savename = efile->gettname();
			if (savename[0] != '\0') { // don't save 0 length names
				string fileextname = string(savename) + ".qcmp";
#if 0
				filesave_string(fileextname.c_str(),econtents->gettname());
				//econtents->settname(filedata);
				//delete[] filedata;
#else
				qf->save(fileextname);
				savelastfile(fileextname);
#endif
				refreshfilelist();
				S32 idx = lfiles->findstring(savename);
				if (idx >= 0)
					lfiles->setidxc(idx);
			focus2 = lfiles;
			}
	// delete file
		} else if (focus2 == bdelete) {
			string fileextnamedel = string(efile->gettname()) + ".qcmp";
			remove(fileextnamedel.c_str());
			refreshfilelist();
	// reset to just wires (passthru, identity)
		} else if (focus2 == breset) {
			U32 numqubits = qf->getnumqubits();
			delete(qf);
			qf = new qfield(numqubits);
			hqfxoffset->setidx(0);
			//resetui();
			sm = STATE;
			updatemode();
	// show current matrix column
		} else if (focus2 == bcolmode) {
			sm = COL;
			updatemode();
	// accumulate matrices
		} else if (focus2 == baccmode) {
			sm = ACC;
			updatemode();
	// show current quantum state
		} else if (focus2 == bstatemode) {
			sm = STATE;
			updatemode();
		} else if (focus2 == bmorequbit) {
			qf->morequbits();
			resetui();
		} else if (focus2 == blessqubit) {
			qf->lessqubits();
			resetui();
		}
	}
	// pick a gate from the gate list
	if (focus2 == lqgates) {
		if (ret >= 0) {
			changecursorqgate(ret);
		}
	// change qf xoffset
	} else if (focus2 == hqfxoffset) {
		//hoffset = ret;
		//lastshowcol = invalidcol;
		qf->setxoffset(ret*hposfactor);
	}
	// proc qfield (circuit)
	qf->proc();
	// proc qgate cursor
	// drop cursor qgate onto qfield by clicking mouse buttons
	// or pick up gate to cursor
	if (wininfo.mleftclicks || wininfo.mrightclicks || wininfo.mmiddleclicks) {
		qgatebase::qtype qg = qgatebase::QTYPEENUM; // invalid gate enum
		if (wininfo.mleftclicks) { // drop
			if (qgcur)
				qg = qgcur->getkind(); // cursor gate enum
		}
		if (wininfo.mrightclicks) {
			qg = qgatebase::PASSTHRU; // pick up gate, make a wire
		}
		if (wininfo.mmiddleclicks) { // link columns
			S32 col;
			if (qf->getcolumncursor(MX,MY,&col)) {
				if (linkfrom == -1) {
					linkfrom = col;
				} else {
					U32 linkto = col;
					qf->makelink(linkfrom,linkto);
					resetui();
				}
			}
		}

		if (qg != qgatebase::QTYPEENUM) {
			pointi2 p;
			if (qf->getrowcolumncursor(MX,MY,&p)) {
				const qgatebase* oldtype = qf->getqgate(p.x,p.y); // get a reference to gate from circuit
				U32 oldtypekind = oldtype->getkind();
				qf->freelink(p.x); // better
				qf->changeqgate(p.x,p.y,qg); // update circuit from qg
				focus2 = lqgates; // keep list of qgates active
				if (wininfo.mleftclicks) { // drop, update circuit gate with cursor
					// step to the next gate in the sequence (multi gate)
					qgatebase::qtype /*ng = qgatebase::qtype(lqgates->getidx());*/
					ng = nextcursorqgate();
					if (ng >= lqgates->getnumidx())
						ng = qgatebase::qtype(0);
					lqgates->setidxc(ng);
					changecursorqgate(ng);
					//focus2 = hqfxoffset;
					//return;
					resetui();
				}
				if (wininfo.mrightclicks) { // pickup, update cursor with circuit gate
					if (oldtype) {
						lqgates->setidxc(oldtypekind);
						changecursorqgate(oldtypekind);
					}
				}
			}
		}
		lastshowcol = invalidcol; // update console
	}
	// draw matrices or state to console
	S32 cl;
	switch(sm) {
	case STATE:
	case ACC:
		if (qf->getaccumcursor(MX,MY,&cl)) {
			printtoconsole(cl);
		}
		break;
	case COL:
		if (qf->getcolumncursor(MX,MY,&cl)) {
			printtoconsole(cl);
		}
		break;
	}
}

void qcompdraw2d()
{
	clipclear32(B32,C32(0,0,255)); // clear screen
	outtextxyc32(B32,WX/2,10,C32WHITE,"Q COMP"); // draw title
	rl->draw(); // draw UI
#ifdef BUILD_GATE_ART
#if 0
	buildGateArt();
#endif
#else
	//qf->draw(qgcur); // draw circuit
#endif

	qf->draw(qgcur); // draw circuit

	// draw console
	bitmap32* cbm = con32_getbitmap32(qcon);
	clipblit32(cbm,B32,0,0,0,550,cbm->size.x,cbm->size.y);
}

void qcompexit()
{
	logger_indent(); // back to normal
	logger("end qcomp ---------->>>>>>>>>>>>\n");
	rl->show();
	delete rl; // free UI
	popdir();
	delete qf; // free circuit
	delete qgcur; // free qgate cursor
	con32_free(qcon);
}
