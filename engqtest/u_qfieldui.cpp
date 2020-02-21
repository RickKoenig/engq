//#include <complex>

#include <m_eng.h>

#include "u_qstate.h"
#include "u_qmat.h"

#include "u_qgatebase.h"
#include "u_qgatederived.h"
#include "u_qcolumn.h"
#include "u_qfieldui.h"

//#include "u_s_qcomp.h"

// qfield CLASS ///
// class that handles the ui of the circuit area of screen, drag delete etc..
// graphical representation of a quantum circuit
void qfield::basic(U32 numqubits)
{
	U32 i;
	blochbm = bitmap32alloc(blochsize,blochsize,brt);
	bakerbm = bitmap32alloc(blochsize,blochsize,brtbloch);
	// bake bloch sphere background
	const S32 nsteps = 4000;
	for (i=0;i<nsteps;++i) {
		float frac = (float)i/nsteps;
		float ang = frac*TWOPI;
		pointf3x sweep(sinf(ang),cosf(ang),0);
		drawvector(bakerbm,blochsize/2,sweep,bakecolor,.1f,false);
		sweep = pointf3x(sinf(ang),0,cosf(ang));
		drawvector(bakerbm,blochsize/2,sweep,bakecolor,.1f,false);
		sweep = pointf3x(0,sinf(ang),cosf(ang));
		drawvector(bakerbm,blochsize/2,sweep,bakecolor,.1f,false);

		sweep = pointf3x(0,0,frac);
		drawvector(bakerbm,blochsize/2,sweep,bakecolor,.1f,false);
		sweep = pointf3x(0,frac,0);
		drawvector(bakerbm,blochsize/2,sweep,bakecolor,.1f,false);
		sweep = pointf3x(frac,0,0);
		drawvector(bakerbm,blochsize/2,sweep,bakecolor,.1f,false);
	}
	// make some qcolumns and push those into the qfield
	qcolumns.clear();
	for (i=0;i<maxcolumns;++i) {
		qcolumn qc(numqubits);
		qc.calc();
		qcolumns.push_back(qc);
	}
	//hilitpos = -1;
	//hilitcenter = false;
	qfxoffset = 0;
	//logger("sizeof qcolumn = %d\n",sizeof(qcolumn));
	//fcalc();  // no need to calc, their all identities
}

qfield::qfield(U32 numqubits) : hilitpos(-1)
{
	basic(numqubits);
}

// load a qfield from a file
qfield::qfield(const string& fname,S32 hlp) : hilitpos(hlp) /* : qcolumns(vector<qcolumn>(maxcolumns)) */
{
	//if (hlp<=0)
	//	hlp = 20;
	//hilitpos = hlp;
	script sc = script(fname.c_str());
	U32 num = sc.num();
	U32 scwid = atoi(sc.read());
	U32 schit = atoi(sc.read());
	basic(schit);
	// hard coded for now... TODO
	enum mode {NO,VER1,VER2,VER3};
	mode m = NO;
	if (num == scwid*schit + 2)
		m = VER1; // no ids
#if 1
	if (num == 2*scwid*schit + 2)
		m = VER2; // added ids
#endif
	if (num == 2*scwid*schit + 2 + scwid) {
		m = VER3; // added linkbacks
	}
	if (m == NO) { // no luck figuring out file format
		//errorexit("bad .qcmp file 1 '%s'",fname.c_str());
		logger("bad .qcmp file 1 '%s'\n",fname.c_str());
		return;
	}
#if 0
	if (/*scwid != maxcolumns || */schit != qcolumn::maxqubits) {
		//errorexit("bad .qcmp file 2 '%s'",fname.c_str());
		logger("bad .qcmp file 2 '%s'\n",fname.c_str());
		basic();
		return;
	}
#endif
	U32 i,j;
	// make some qcolumns and push those into the qfield
	if (scwid > maxcolumns)
		scwid = maxcolumns;
	for (i=0;i<scwid;++i) {
		qcolumn qc(schit);
		for (j=0;j<schit;++j) {
			const C8* qgs = sc.read();
			//U32 qg = atoi(qgs);
			U32 gid = 6;
			if (m == VER2 || m == VER3)
				gid = atoi(sc.read());

			//if (isdigit(qgs[0])) { // number, qgatebase::qtype
			//if (true) {
				//qc.changeqgatecol(j,qgatebase::qtype(qg),gid);
			//} else { // name
			qc.changeqgatecol(j,qgs,gid);
			//}
		}
		if (m == VER3) {
			S32 lb = atoi(sc.read());
			qc.setlinkback(lb);
		}
		qc.calc();
		qcolumns[i] = qc;
	}
	calclinks();
	fcalc();
	//logger("sizeof qcolumn = %d\n",sizeof(qcolumn));
}

// save to a file
void qfield::save(const string& fname) const
{
	U32 numqubits = getnumqubits();
//#define VER1
//#define VER2
#define VER3
	FILE* fw;
	fw=fopen2(fname.c_str(),"w");
	if (fw) {
		fprintf(fw,"%2d %2d\n",maxcolumns,numqubits);
		U32 i,j;
		for (j=0;j<maxcolumns;++j) {
			const qcolumn& qc = qcolumns[j];
			for (i=0;i<numqubits;++i) {
				//const qcolumn& qc = qcolumns[j];
				const qgatebase::qtype qt = qc.getkind(i);
#ifdef VER1
				fprintf(fw,"%2d ",qt);
#endif
#ifdef VER2
				U32 qid = qc.getid(i);
				fprintf(fw,"\"%s\" %2d   ",qgatebase::qgnames[qt],qid);
#endif
#ifdef VER3
				U32 qid = qc.getid(i);
				fprintf(fw,"\"%s\" %2d   ",qgatebase::qgnames[qt],qid);
#endif
			}
#ifdef VER3
			fprintf(fw,"%d",qc.getlinkback());
#endif
			fprintf(fw,"\n");
		}
		fclose(fw);
	}
}

const qgatebase* qfield::getqgate(U32 col,U32 qub) const
{
	if (col >= maxcolumns)
		return 0;
	if (qub >= getnumqubits())
		return 0;
	return qcolumns[col].getqgatecol(qub);
}

// replace a qgate inside the grid of qgates, used by UI drop gate
void qfield::changeqgate(U32 col,U32 qub,qgatebase::qtype qt)
{
	if (col >= maxcolumns)
		return;
	if (qub >= getnumqubits())
		return;
	// change qgate in this column
	qcolumns[col].changeqgatecolautoid(qub,qt);
	calclinks();
	fcalc();
}

// copy whole column of gates 'from' to column 'to'
void qfield::copycolumn(U32 from,U32 to)
{
	if (from >= maxcolumns)
		return;
	if (to >= maxcolumns)
		return;
	if (from == to)
		return;
	// fancy assignment happening here
	S32 oldlink = qcolumns[to].getlinkback();
	qcolumns[to] = qcolumns[from];
	qcolumns[to].setlinkback(oldlink);
}

void qfield::proc()
{
	// handle the UI for a qfield
}

U32 qfield::getnumqubits() const
{
	if (qcolumns.size() > 0)
		return qcolumns[0].getnumqubits();
	else
		return 0;
}

void qfield::morequbits()
{
	if (getnumqubits() >= qcolumn::maxiqubits)
		return;
	U32 i;
	for (i=0;i<maxcolumns;++i) {
		qcolumns[i].morequbits();
	}
	fcalc();
}

void qfield::lessqubits()
{
	if (getnumqubits() <= qcolumn::miniqubits)
		return;
	U32 i;
	for (i=0;i<maxcolumns;++i) {
		qcolumns[i].lessqubits();
	}
	fcalc();
}

//#define DOROTAXIS
#ifdef DOROTAXIS
void qfield::toBlochAngleAxisMat(const qmat& m,pointf3* axis,float* angleRet,float* phaseRet)
{
	if (m.getnumqubits() != 1)
		errorexit("toBlochAngleAxis, only 1 qubit please!");
	// read in the matrix
	compf a = m.ele[0][0];
	compf b = m.ele[0][1];
	compf c = m.ele[1][0];
	compf d = m.ele[1][1];

    // --- Part 1: convert to a quaternion ---
    // Phased components of quaternion.
	compf pfi = compf(0,.5f);
    compf wp = (a + d) * .5f;
    compf xp = -(b + c) * pfi;
    compf yp = (b - c) * .5f;
    compf zp = -(a - d) * pfi;

    // Arbitrarily use largest value to determine the global phase factor.
	// W
    // phase = max([wp, xp, yp, zp], key=abs)
	float bestnrm = norm(wp);
	compf phase = wp;
	// X
	float nrm = norm(xp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = xp;
	}
	// Y
	nrm = norm(yp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = yp;
	}
	// Z
	nrm = norm(zp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = zp;
	}
    phase /= abs(phase);
	compf phaseconj = conj(phase);


    // Cancel global phase factor, recovering quaternion components.
    float w = (wp * phaseconj).real();
    float x = (xp * phaseconj).real();
    float y = (yp * phaseconj).real();
    float z = (zp * phaseconj).real();

    // --- Part 2: convert from quaternion to angle-axis ---

    // Floating point error may have pushed w outside of [-1, +1]. Fix that.
	// w = min(max(w, -1.0f), 1.0f);
    w = range(-1.0f,w,1.0f);

    // Recover angle.
    *angleRet = -2*acosf(w);

    // Normalize axis.
    float n = sqrtf(x*x + y*y + z*z);
	if (n < 0.000001f) {
        // There's an axis singularity near angleRet=0.
        // Just default to no rotation around the Z axis in this case.
        *angleRet = 0;
        x = 0;
        y = 0;
        z = 1;
        n = 1;
	}
	float in = 1.0f/n;
    x *= in;
    y *= in;
    z *= in;

	// --- Part 3: (optional) canonicalize ---
    // Prefer angle in [-pi, pi]
	if (*angleRet <= -PI) {
        *angleRet += TWOPI;
        phase *= -1;
	}
    // Prefer axes that point positive-ward.
	if (x + y + z < 0) {
        x *= -1;
        y *= -1;
        z *= -1;
        *angleRet *= -1;
	}
	axis->x = x;
	axis->y = y;
	axis->z = z;
    //phase_angle = cmath.polar(phase)[1]
	*phaseRet = arg(phase);
    //return axis(x, y, z), angle, phase_angle
}
#endif

void qfield::toBlochQuatMat(const qmat& m,pointf3* q,float* phaseRet)
{
	if (m.getnumqubits() != 1)
		errorexit("toBlochAngleAxis, only 1 qubit please!");
	compf dt = m.det();
	float pm = getprob(dt);
	if (pm < QEPSILON) {
		*q = pointf3x(); // identity quaternion
		*phaseRet = 0;
		return;
	}
	// read in the matrix
	compf a = m.ele[0][0];
	compf b = m.ele[0][1];
	compf c = m.ele[1][0];
	compf d = m.ele[1][1];

    // --- Part 1: convert to a quaternion ---
    // Phased components of quaternion.
	compf pfi = compf(0,.5f);
    compf wp = (a + d) * .5f  * -1.0f; // a fix ?
    compf xp = -(b + c) * pfi;
    compf yp = (b - c) * .5f;
    compf zp = -(a - d) * pfi;

    // Arbitrarily use largest value to determine the global phase factor.
	// W
    // phase = max([wp, xp, yp, zp], key=abs)
	float bestnrm = norm(wp);
	compf phase = wp;
	// X
	float nrm = norm(xp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = xp;
	}
	// Y
	nrm = norm(yp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = yp;
	}
	// Z
	nrm = norm(zp);
	if (nrm > bestnrm) {
		bestnrm = nrm;
		phase = zp;
	}
    phase /= abs(phase);
	compf phaseconj = conj(phase);


    // Cancel global phase factor, recovering quaternion components.
    q->w = (wp * phaseconj).real();
    q->x = (xp * phaseconj).real();
    q->y = (yp * phaseconj).real();
    q->z = (zp * phaseconj).real();

    // --- Part 2: convert from quaternion to angle-axis ---
#if 0
    // Floating point error may have pushed w outside of [-1, +1]. Fix that.
	// w = min(max(w, -1.0f), 1.0f);
    w = range(-1.0f,w,1.0f);

    // Recover angle.
    *angleRet = -2*acosf(w);

    // Normalize axis.
    float n = sqrtf(x*x + y*y + z*z);
	if (n < 0.000001f) {
        // There's an axis singularity near angleRet=0.
        // Just default to no rotation around the Z axis in this case.
        *angleRet = 0;
        x = 0;
        y = 0;
        z = 1;
        n = 1;
	}
	float in = 1.0f/n;
    x *= in;
    y *= in;
    z *= in;
#endif
#if 1
	// --- Part 3: (optional) canonicalize ---
    // Prefer angle in [-pi, pi]
	if (q->w >= 0) {
        phase *= -1;
	}
#endif
#if 0
	// Prefer axes that point positive-ward.
	if (q->x + q->y + q->z < 0) {
        q->x *= -1;
        q->y *= -1;
        q->z *= -1;
		q->w *= -1;
        //*angleRet *= -1;
	}
#endif
#if 0
	axis->x = x;
	axis->y = y;
	axis->z = z;
#endif
    //phase_angle = cmath.polar(phase)[1]
	*phaseRet = arg(phase);
	quatnormalize(q,q);
    //return axis(x, y, z), angle, phase_angle
}

void qfield::drawvector(bitmap32 *bm,S32 bsh,const pointf3& p,C32 col,float scl,bool drawline) const
{
	//float forback = range(-1.0f,p.x,1.0f); // bound for sqrt
	float skew = p.x;//sqrtf(1.0f - forback*forback);
	const float skewStrength = .08f;
	S32 px = bsh + S32(bsh*(p.y - skewStrength*skew));
	S32 py = bsh - S32(bsh*(p.z - skewStrength*skew));
	S32 r = S32(scl*(6 + p.x*3));
	if (r<=1) r=1;
	if (drawline)
		clipline32(bm,bsh,bsh,px,py,col);
	clipcircle32(bm,px,py,r,col);
}

//#define METHOD1
#define METHOD2

#ifdef METHOD1
void qfield::drawbloch() const 
	{
		S32 col;
		float frac;
		bool res = getcolumncursor(MX,MY,&col,&frac);
		if (!res) {
			res = true;
			col = gethilitpos();
			if (col < 0) {
				//res = false;
				//col = 0;
			}
			if (col >= (S32)maxcolumns)
				col = maxcolumns - 1;
			frac = .999f;
		}
		if (res) {
//#define FORCE
#ifdef FORCE
			col = 1;
			frac = .25f;
#endif
			//clipclear32(blochbm,brtbloch);
			clipblit32(bakerbm,blochbm,0,0,0,0,blochsize,blochsize);
			outtextxyf32(blochbm,8,8,C32BLACK,"C=%d",col);
			outtextxyf32(blochbm,8,16,C32BLACK,"F=%6.3f",frac);
			S32 bsh = blochsize / 2;
			const qmat& curqmat = getqmatcol(col);
			const qmat& curqmata = getqmatacc(col - 1);

#ifdef DOROTAXIS
			pointf3 axis;
			float angleRet;
			float phaseRet;
#endif

//#define TOQUAT


//#ifdef TOQUAT


			pointf3 q,qa; // current and accumulated quat
			float phaseRet2,phaseRet2a;
			toBlochQuatMat(curqmat,&q,&phaseRet2);
			toBlochQuatMat(curqmata,&qa,&phaseRet2a);
			// Prefer axes that point positive-ward.
			pointf3 axis2;
			quat2rotaxis(&q,&axis2);
			if (axis2.x + axis2.y + axis2.z < 0) {
				axis2.x *= -1;
				axis2.y *= -1;
				axis2.z *= -1;
				axis2.w *= -1;
				//*angleRet *= -1;

			}

			// if angle close to -PI, make PI, fix errors from using a low precision PI
			const float howClose = QEPSILON;
			if (axis2.w < -PI + howClose) {
				axis2.w = PI;
				quatinverse(&q,&q);
			}
			pointf3 q0 = pointf3x();
			pointf3 qi;
			quatinterp(&q0,&q,frac,&qi);

			// invert
			//axis2.w = TWOPI - axis2.w;
			// store
			float angleRet2 = axis2.w;
//#else


#ifdef DOROTAXIS
			toBlochAngleAxisMat(curqmat,&axis,&angleRet,&phaseRet);
#endif

//#endif
			struct basis {
				pointf3 ax;
				C32 col;
			};
			basis bas[3] = {
				{{1,0,0},C32GREEN},
				{{0,1,0},C32BLUE},
				{{0,0,1},C32RED},
			};
			//float ang = TWOPI*frac;
			cliprect32(blochbm,-5 + S32(frac*blochsize),blochsize-10,10,10,C32DARKGRAY);

			pointf3 qt;
			quattimes(&qi,&qa,&qt);
			//drawvector(blochbm,bsh,pointf3x(0,sinf(ang),cosf(ang)),C32LIGHTGRAY);
			if (abs(angleRet2) > QEPSILON)
				drawvector(blochbm,bsh,axis2,C32LIGHTGRAY,1.75f);
			for(S32 i=0;i<3;++i) {
				pointf3& vi = bas[i].ax;
				pointf3 vo;
				quatrot(&qt,&vi,&vo);
				drawvector(blochbm,bsh,vo,bas[i].col);
			}




			S32 curs = 4;
#ifdef DOROTAXIS
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"ROTAXIS");
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"X=%6.3f",axis.x);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"Y=%6.3f",axis.y);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"Z=%6.3f",axis.z);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"A=%6.3f",angleRet);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"P=%6.3f",phaseRet);
			curs++;
#endif

			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"QUAT");
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"X=%6.3f",axis2.x);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"Y=%6.3f",axis2.y);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"Z=%6.3f",axis2.z);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"A=%6.3f",angleRet2);
			outtextxyf32(blochbm,8,8*curs++,C32BLACK,"P=%6.3f",phaseRet2);

		}
		clipblit32(blochbm,B32,0,0,945,210,blochbm->size.x,blochbm->size.y);
	}
#endif

#ifdef METHOD2
void qfield::drawbloch() const 
{
	S32 col;
	float frac;
	bool res = getcolumncursor(MX,MY,&col,&frac);
	if (!res) {
		res = true;
		col = gethilitpos();
		if (col >= (S32)maxcolumns)
			col = maxcolumns - 1;
		frac = .999f;
	}
	clipblit32(bakerbm,blochbm,0,0,0,0,blochsize,blochsize);
	outtextxyf32(blochbm,8,8,C32BLACK,"C=%d",col);
	outtextxyf32(blochbm,8,16,C32BLACK,"F=%6.3f",frac);
	S32 bsh = blochsize / 2;
	const qmat& curqmat = getqmatcol(col);
	const qmat& curqmata = getqmatacc(col - 1);

	compf detreta = curqmata.det();
	float proba = getprob(detreta);
	bool singa = false;
	if (proba < QEPSILON) {
		//logger("Must be in a measurement gate!");
		singa = true;
	}
	compf detretc = curqmat.det();
	float probc = getprob(detretc);
	bool singc = false;
	if (probc < QEPSILON) {
		//logger("Must be in a measurement gate!");
		singc = true;
	}

	pointf3 q;//,qa; // current and accumulated quat
	float phaseRet2;// = .47f;//,phaseRet2a;
	//if (true) {
	//	toBlochQuatMat(curqmat,&q,&phaseRet2);
	//} else {
		toBlochQuatMat(curqmat,&q,&phaseRet2);
	//} 
	//toBlochQuatMat(curqmata,&qa,&phaseRet2a);
	// Prefer axes that point positive-ward.
	pointf3 axis2;
	quat2rotaxis(&q,&axis2);
	if (axis2.x + axis2.y + axis2.z < 0) {
		axis2.x *= -1;
		axis2.y *= -1;
		axis2.z *= -1;
		axis2.w *= -1;
	}
	// if angle close to -PI, make PI, fix errors from using a low precision PI
	const float howClose = QEPSILON;
	if (axis2.w < -PI + howClose) {
		axis2.w = PI;
		quatinverse(&q,&q);
	}
	pointf3 q0 = pointf3x();
	pointf3 qi;
	quatinterp(&q0,&q,frac,&qi);
	// store
	float angleRet2 = axis2.w;
	struct basis {
		qstate ax;
		C32 col;
	};
/*	basis bas[3] = {
		{{1,0,0},C32GREEN},
		{{0,1,0},C32BLUE},
		{{0,0,1},C32RED},
	}; */
	basis bas[3] = { // run a whole basis thru the machine, not just Z red, but also X green, Y blue until any measurement is made
		{qstate(1),C32GREEN}, // X
		{qstate(1),C32BLUE}, // Y
		{qstate(1),C32RED}, // Z, this is the main one
	};
	static compf xdata[2] = {compf(SR2O2),compf(SR2O2)};
	static compf ydata[2] = {compf(SR2O2),compf(0,SR2O2)};
	bas[0].ax.load(xdata,2);
	bas[1].ax.load(ydata,2);
	bas[2].ax.init0(); // Z

	// check Z qstate first
	qstate Zsacc = curqmata*bas[2].ax;
	if (frac >= .5f) { // RIGHT SIDE, after applied current gate
		Zsacc = curqmat*Zsacc;
	}
	float zprob = Zsacc.gettotprob();
	bool singZ = false;
	bool measZ = false;
	if (zprob < QEPSILON) {
		singZ = true;
	}
	if (zprob < 1 - QEPSILON)
		measZ = true;
	if (!singZ) { // any qstate left ??
		cliprect32(blochbm,-5 + S32(frac*blochsize),blochsize-10,10,10,C32DARKGRAY);
		//pointf3 qt;
	//	quattimes(&qi,&qa,&qt);
		if (abs(angleRet2) > QEPSILON)
			drawvector(blochbm,bsh,axis2,C32LIGHTGRAY,1.75f);
			bool draw3 = false;
			if (!singa && !measZ && (frac < .5f || !singc))
				draw3 = true;
			S32 starti;
			if (draw3)
				starti = 0; // all three X,Y,Z
			else
				starti = 2; // just the Z because of a measurement
//			if (!singa && !singc) {
			for(S32 i=starti;i<3;++i) {
#if 1
				qstate sacc = curqmata*bas[i].ax;
				if (singc && frac >= .5f)
					sacc = curqmat*sacc;
				pointf3 vi = sacc.qstateToBloch();
				//pointf3& vi = bas[i].ax;
				pointf3 vo;
				quatrot(&qi,&vi,&vo);
				drawvector(blochbm,bsh,vo,bas[i].col);
#endif
			}
//			}
		S32 curs = 4;
		outtextxyf32(blochbm,8,8*curs++,C32BLACK,"QUAT");
		outtextxyf32(blochbm,8,8*curs++,C32BLACK,"X=%6.3f",axis2.x);
		outtextxyf32(blochbm,8,8*curs++,C32BLACK,"Y=%6.3f",axis2.y);
		outtextxyf32(blochbm,8,8*curs++,C32BLACK,"Z=%6.3f",axis2.z);
		outtextxyf32(blochbm,8,8*curs++,C32BLACK,"A=%6.3f",angleRet2);
		outtextxyf32(blochbm,8,8*curs++,C32BLACK,"P=%6.3f",phaseRet2);
		clipblit32(blochbm,B32,0,0,945,210,blochbm->size.x,blochbm->size.y);
	}
}
#endif


void qfield::draw(const qgatebase* cur) const
{
	U32 i;
	U32 numqubits = getnumqubits();
	static const bool doclip = true;

	// clip everything inside white box, the qfield
	cliprect32(B32,fpixleft-leftlabels,fpixtop,fpixwid+leftlabels,fpixhit,brt);
	if (doclip)
		setcliprect32(B32,fpixleft-leftlabels,fpixtop,fpixwid+leftlabels,fpixhit);
	// draw the labels
	if (qfxoffset == 0) {
		for (i=0;i<numqubits;++i) {
			outtextxyf32(B32,fpixleft - leftlabels,fpixtop + i*qgatebase::gpixhit + qgatebase::gpixhit/2 - 4,C32BLACK,"Q%d |0>",i);
		}
	}
	// draw the columns
	// if half mode (acc or qstate) and -1, then show initial state or acc matrix
	if (hilitpos == -1 && hilitcenter) {
		U32 xoff = qfield::fpixleft - qfxoffset;
		//U32 xoc;
		U32 xwid = (qcolumn::cpixwid - qcolumn::cpixwid/2)/2;
			//xoc = xoff + qcolumn::cpixwid/2 + qcolumn::cpixwid/4;
		//xoc = 550;
		cliprect32(B32,xoff,qfield::fpixtop,xwid,numqubits*qgatebase::gpixhit,qfield::hili);
		//clipcircleo32(B32,500,600,55,C32CYAN);
	}
	for (i=0;i<qcolumns.size();++i) {
		U32 xoff = qfield::fpixleft + qcolumn::cpixwid*i;
		//U32 xoc = hilitcenter ? xoff : xoff + 10;
	// draw extra background column if hilit enabled
		bool hilit = i == hilitpos;
		if (hilit) {
			U32 xoc;
			U32 xwid;
			if (hilitcenter) {
				xoc = xoff + qcolumn::cpixwid/2 + qcolumn::cpixwid/4 - qfxoffset;
				xwid = qcolumn::cpixwid/2;
			} else {
				xoc = xoff - qfxoffset;
				xwid = qcolumn::cpixwid;
			}
			cliprect32(B32,xoc,qfield::fpixtop,xwid,numqubits*qgatebase::gpixhit,qfield::hili);
		}
		if (xoff + 2*qcolumn::cpixwid/3 >= qfxoffset + fpixleft - leftlabels)
			if (xoff + 2*qcolumn::cpixwid/3 < qfxoffset + fpixleft + fpixwid + qcolumn::cpixwid/3)
				qcolumns[i].draw(xoff,qfxoffset,hilitcenter);
		if (qcolumns[i].getlinkback() >= 0) {
			qcolumns[i].drawlinkback(xoff,qfxoffset);
		}
	}

	// draw bloch
	if (numqubits == 1 && hilitcenter) { // don't draw COL mode
		drawbloch();
	}

	//if (cur) {
	if (cur && isinside(MX,MY)) {
		U32 xoff = MX - qcolumn::cpixwid/2;
		U32 yoff = MY - qgatebase::gpixhit/2;
		if (cur->hasmultibox())
			cur->drawboxm(B32,xoff,yoff);
		cur->draw(B32,xoff,yoff); // draw qgate cursor
	}
	if (isinside(MX,MY))
		clipline32(B32,MX,fpixtop + 2,MX,fpixtop + fpixhit - 1 - 2,C32GREEN);
	// clip back to whole window/screen
	if (doclip)
		resetcliprect32(B32);

}

bool qfield::isinside(U32 mx,U32 my)
{
	//return true;
	if (mx <fpixleft - leftlabels || my < fpixtop)
		return false;
	if (mx >= fpixleft + fpixwid || my >= fpixtop + fpixhit)
		return false;
	return true;
}

bool qfield::getcolumncursor(U32 mx,U32 my,S32* cl,float* frac) const
{
	if (!isinside(mx,my))
		return false;
	S32 xo = mx - fpixleft + qfxoffset;
	S32 i = xo/qcolumn::cpixwid;
	if (i >= (S32)maxcolumns)
		return false;
	//if (i < 0)
	//	return false;
	*cl = i;
	if (frac) {
		S32 rem = xo - i*qcolumn::cpixwid;
		float f = (float)rem/qcolumn::cpixwid;
#define DEADZONE
#ifdef DEADZONE
		float deadzone = .1f;
		float m = 1.0f/(1.0f - 2.0f*deadzone);
		f = m*(f - deadzone);
		f = range(0.0f,f,.9999f);
#endif
		*frac = f;
	}
	return true;
}

bool qfield::getaccumcursor(U32 mx,U32 my,S32* cl) const
{
	if (!isinside(mx,my))
		return false;
	S32 i = (mx - fpixleft + qfxoffset + qcolumn::cpixwid/2)/qcolumn::cpixwid - 1;
	if (i >= S32(maxcolumns))
		return false;
	//if (i < -1)
	//	return false;
	*cl = i;
	return true;
}

bool qfield::getrowcolumncursor(U32 mx,U32 my,pointi2* p) const
{
	if (!isinside(mx,my))
		return false;
	U32 i = (mx - fpixleft + qfxoffset)/qcolumn::cpixwid;
	U32 j = (my - fpixtop)/qgatebase::gpixhit;
	if (i >= maxcolumns)
		return false;
	if (j >= getnumqubits())
		return false;
	p->x = i;
	p->y = j;
	return true;
}

const qmat qfield::getqmatcol(S32 col) const
{
	if (col >= 0)
		return qcolumns[col].getqmatcol();
	else
		return qmat(1<<(getnumqubits())); // -1 return identity of correct size
}

const qmat qfield::getqmatacc(S32 col) const
{
	if (col >= 0)
		return qcolumns[col].getqmatacc();
	else
		return qmat(1<<(getnumqubits())); // -1 return identity of correct size
}

qfield::~qfield()
{
	//logger("shutdown a qfield\n");
	bitmap32free(blochbm);
	bitmap32free(bakerbm);
}

void qfield::fcalc()
{
	U32 i;
	qmat qma(1<<getnumqubits());
	for (i=0;i<qcolumns.size();++i) {
		qmat qm = qcolumns[i].getqmatcol();
		qma = qm * qma;
		qcolumns[i].setqmatacc(qma);
	}
}

// what is this ???
void qfield::setqmatacc(S32 col,const qmat& qma)
{
}

void qfield::makelink(S32 lf,S32 lt)
{
	if (lf > lt)
		qcolumns[lf].setlinkback(lt);
	if (lf == lt)
		freelink(lf);
	calclinks();
	fcalc();
}

void qfield::freelink(S32 col)
{
	qcolumns[col].setlinkback(-1);
}

void qfield::calclinks()
{
	//return;
	U32 i;
	for (i=0;i<qcolumns.size();++i) {
		S32 to = qcolumns[i].getlinkback();
		if (to >= 0)
			copycolumn(to,i);
	}
}

C32 qfield::brt = C32(230,230,230);
C32 qfield::brtbloch = C32(200,200,200);
C32 qfield::bakecolor = C32(190,190,190);
C32 qfield::hili = C32(210,210,210);
//S32 qfield::blochsize = 80;
