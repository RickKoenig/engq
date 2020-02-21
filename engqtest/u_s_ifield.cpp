#include <m_eng.h>
#include <l_misclibm.h>
#include "u_s_ifield.h"
#include "u_z2p.h"
#include "u_states.h"

#include "ring2.h"
#include "augmatz.h"

#include "u_ply.h"
#include "u_group_field.h"

#include "math_abstract.h"
#include "augmatn.h"


#define USENS // use namespace, turn off for debugging
#ifdef USENS
namespace {
#endif

#define BERL // use Berlekamp's algorithm for factors

// ui
shape* rl;
shape* focus,*oldfocus;

pbut* bquit;
pbut* bsmaller,*bbigger;
pbut* bbackupa,*breseta,*bbackupm,*bresetm;
listbox* lbhomo;
pbut* bhomo;

edit* epolya,*epolyb,*epolym;
text* tpolyr,*tpolyrm;
pbut* bcalcring,*bcalcpolyfield;
pbut* bplus,*bminus,*bmult,*bdiv,*bmod,*bgcd,*bcompose,*bfactor,*bderiv;
text* tmod;
hscroll* hmod;
text* tpp;
pbut* bpp;

pbut* bm[5],*bc[15];
text* tc[15];
U32 mess,code,fixe;
text* tnbits;
// end ui;
struct deco {
	U32 cod;
	S32 nbits; // -1 if decode err, 0 if no err, else nbits err
};
struct deco* decode_data; // decode for a (15,5) scheme

const int INITSIZE=4;
const int GAOFFX=25;
const int GAOFFY=60;
const int GMOFFX=430;
const int GMOFFY=60;
const int GSQSIZE=24;

class group *ga,*gm;
fielderror fe;


S32 intpow(S32 b,S32 e)
{
	if (e < 0)
		return 0;
	S32 r = 1;
	while(e > 0) {
		r *= b;
		--e;
	}
	return r;
}


// build a field based upon poly
void calcpolyfield()
{
	const C8* str = epolym->gettname();
	ply pf(str);
	S32 d = pf.getdeg();
	S32 m = pf.getmod();
	if (d <= 0)
		return;
	if (!isprime(m))
		return;
	S32 gf = intpow(m,d);
	if (gf > group::MAXBSIZE)
		return;
	bool vis = gf <= group::MAXHOMOSIZE;
	lbhomo->setvis(vis);
	bhomo->setvis(vis);
	delete ga;
	delete gm;
	ga=new group(gf,GAOFFX,GAOFFY,GSQSIZE,false);
	gm=new group(gf,GMOFFX,GMOFFY,GSQSIZE,true);
	ga->resetgroup();
	gm->resetgroup();
	S32 i,j;
	for (j=0;j<gf;++j) {
		ply pj;
		pj.setrep(j);
		for (i=0;i<gf;++i) {
			ply pi;
			pi.setrep(i);
			ply pa = pi + pj;
			S32 r = pa.getrep();
			ga->setboard(i,j,r);
//			ga->setboard(i,j,(i+j)%m);
		}
	}
	for (j=1;j<gf;++j) {
		ply pj;
		pj.setrep(j);
		for (i=1;i<gf;++i) {
			ply pi;
			pi.setrep(i);
			ply pa = pi * pj;
			pa = pa % pf;
			S32 r = pa.getrep();
			gm->setboard(i,j,r);
//			gm->setboard(i,j,(i*j)%m);
		}
	}
}

// build a ring based upon mod
void calcring()
{
	ga->resetgroup();
	gm->resetgroup();
	S32 i,j;
	S32 m = ga->getorder();
	for (j=0;j<m;++j) {
		for (i=0;i<m;++i) {
			ga->setboard(i,j,(i+j)%m);
		}
	}
	for (j=1;j<m;++j) {
		for (i=1;i<m;++i) {
			gm->setboard(i,j,(i*j)%m);
		}
	}
}

void printpolymod(const ply& p)
{
	ply m(epolym->gettname());
	ply r = p % m;
	tpolyrm->settname(r.print().c_str());
}

void calcplus()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a + b;
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calcminus()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a - b;
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calcmult()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a * b;
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calcdiv()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a / b;
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calcmod()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a % b;
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calcgcd()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a.gcd(b);
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calccompose()
{
	ply a(epolya->gettname());
	ply b(epolyb->gettname());
	ply c = a.compose(b);
	tpolyr->settname(c.print().c_str());
	printpolymod(c);
}

void calcfactor(ply a)
{
	const vector<ply>& f = a.factors();
	S32 i,n=f.size();
	string s;
	bool oldprint = ply::getpackprint();
	ply::setpackprint(false);
	logger("\n----------------------------\nfactor of '%s' = \n",a.print().c_str());
	ply::setpackprint(oldprint);
	for (i=0;i<n;++i) {
		s += "(" + f[i].print() + ")";
		logger("factor[%3d] '%s' %s\n",i,f[i].print().c_str(),f[i].isprim(a.getdeg())?"P":"NP");
	}
	tpolyr->settname(s.c_str());
}

void calcderiv(ply a)
{
	ply d = a.deriv();
	tpolyr->settname(d.print().c_str());
	epolyb->settname(d.print().c_str());
	printpolymod(d);
}


S32 ptable[maxmod];

void inithmod()
{
	S32 j=0;
	S32 m = ply::getmod();
	S32 i;
	S32 idx = 0;
	for (i=2;i<=maxmod;++i) {
		if (isprime(i)) {
			if (i == m)
				idx = j;
			ptable[j++]=i;
		}
	}
	hmod->setminmaxval(0,j-1);
	hmod->setidx(idx);
}

void settmod()
{
	C8 smod[10];
	S32 m = hmod->getidx();
	S32 mm = ptable[m];
	ply::setmod(mm);
	sprintf(smod,"Mod %d",mm);
	tmod->settname(smod);
}

#if 1
void test_augmatn()
{

//#define FRAC
//#define FLOAT
#define FIELD2

#ifdef FLOAT
	typedef float T;
	augmatn<T> aug(2,2);
	const float d[6] = {1,1,1.1f,1,-1,1};
	aug.set(d,6);
#endif

#ifdef FRAC
	typedef fraction T;
	augmatn<T> aug(2,2);
	const fraction d[6] = {1,1,fraction(110,100),1,-1,1};
	aug.set(d,6);
#endif

#ifdef FIELD2
	typedef field2 T;
	augmatn<T> aug(2,2);
	field2::setprim("11111"); // not so good prim, harder to deal with
	const field2 d1[6] = {
//		"0110","1011","1010", // 110
//		"1011","1010","0111"
//		"1101","1100","1100", // 10010
//		"1100","1100","1101"
		"1011","0111","0110", // 10010
		"0111","0110","1010"
	};
	aug.set(d1,6);
#endif
	vector<vector<T> > vars;
#ifdef FIELD2
	aug.print();
	vars = aug.getvarsc();
	augmatn<T>::printvars(vars);

#ifdef FIELD2
	// find roots of error locator poly
// hardcode for now
	logger("find root of err loc poly1\n");
	// 1 + vars[1] * x + vars[0] * x^2
	for (S32 i=0;i<15;++i) { // w0 to w14
		field2 a = 1;
		field2 x = field2::p2f(i);
		a += x * vars[0][1];
		a += x * x * vars[0][0];
		if (a.iszero()) {
			logger("root found at w^%d\n",i);
			S32 di = field2::downgrade(i);
			if (di<0) {
				logger("can't downgrade %d, abort root check\n",i);
				break;
			}
			logger("downgrade root found at %d\n",i);
		}
	}
	logger("end find root of err loc poly1\n");
#endif

	field2::setprim("10011"); // good prim
	const field2 d2[6] = {
		"1010","1000","0010",
		"1000","0010","1100"
	};
	aug.set(d2,6);
#endif

	aug.print();
	vars = aug.getvarsc();
	augmatn<T>::printvars(vars);

#ifdef FIELD2
	// find roots of error locator poly
// hardcode for now
	logger("find root of err loc poly2\n");
	// 1 + vars[1] * x + vars[0] * x^2
	for (S32 i=0;i<15;++i) { // w0 to w14
		field2 a = 1;
		field2 x = field2::p2f(i);
		a += x * vars[0][1];
		a += x * x * vars[0][0];
		if (a.iszero()) {
			logger("root found at w^%d\n",i);
			S32 di = field2::downgrade(i);
			if (di<0) {
				logger("can't downgrade %d, abort root check\n",i);
				break;
			}
			logger("downgrade root found at %d\n",di);
		}
	}
	logger("end find root of err loc poly2\n");
#endif
}

#endif

vector<S32> calcerrors(const vector<ply>& syns,S32 moddeg,S32 edist,S32* res) // return true for unique solution
{
	vector<S32> ret;
	S32 fulldeg = field2::getprimdeg();
	fulldeg = (1<<fulldeg) - 1;
	//logger("moddeg %d, fulldeg %d\n",moddeg,fulldeg); // for x9+1 it's 9,63
	S32 t = (edist-1)>>1;
	typedef field2 T;
	augmatn<T> aug(t,t);
//	field2::setprim("11111"); // not so good prim, harder to deal with
	S32 ds = t*(t+1);
	vector<T> d1(ds);
	S32 i,j;
	for (j=0;j<t;++j) {
		for (i=0;i<=t;++i) {
			d1[j*(t+1)+i] = syns[1+i+j].getrep();
		}
	}
//	const T d1[6] = {1,2,3,4,5,6};
//		"0110","1011","1010", // 110
//		"1011","1010","0111"
//		"1101","1100","1100", // 10010
//		"1100","1100","1101"
//		"1011","0111","0110", // 10010
//		"0111","0110","1010"
//	};
	aug.set(&d1[0],ds);
	vector<vector<T> > vars;
	aug.print();
	vars = aug.getvarsc();
	augmatn<T>::printvars(vars);
	if (vars.size()!=1) {
		*res = 0;
		return ret;
	}
#if 1
		//logger("find root of err loc polye\n");
		// t = 0
		// 1

		// t = 1
		// 1 + vars[0]*x

		// t = 2
		// 1 + vars[1] * x + vars[0] * x^2
		// 1 + x*(vars[1] + vars[0]*x)

		// t = 3
		// 1 + vars[2]*x + vars[1]*x^2 + vars[0]*x^3
		// 1 + x*(vars[2]+ x*(vars[1]+ x*vars[0]))
		for (i=0;i<fulldeg;++i) { // w(0) to w(fulldeg-1)
			field2 a = 0U;
			field2 x = field2::p2f(i);
			for (j=0;j<t;++j) {
				a += vars[0][j];
				a *= x;
			}
			//a += x * vars[0][1];
			// a += x * x * vars[0][0];
			a += 1;
			if (a.iszero()) {
				//logger("root found at w^%d\n",i);
				S32 di = field2::downgrade(i);
				if (di<0) {
					//logger("can't downgrade %d, abort root check\n",i);
					ret.clear();
					*res = -1;
					return ret;
				}
				//logger("downgrade root found at %d\n",di);
				if (di)
					di = moddeg - di; // mult inverse
				//logger("INVERSE root found at < %d >\n",di);
				ret.push_back(di);
			}
		}
		//logger("end find root of err loc polye\n");
	*res = 1;
	return ret;
#endif
}

void calc_homo()
{
	S32 ord = ga->getorder();
	vector<S32> homo(ord,0);
	S32 i,j;
	lbhomo->clear();
	if (!ga->checkgroup())
		return;
	if (!gm->checkgroup())
		return;
	{ // add the 'zero' homomorphism
		stringstream ss;
		for (i=0;i<ord;++i) {
			if (homo[i] >= 10)
				ss << char('A'-10)  << " ";
			else
				ss << homo[i] << " ";
		}
		lbhomo->addstring(ss.str().c_str());
	}
	homo[1] = 1; // start with 0,1...
	while(true) {
		bool checkhomo = true;
		for (j=0;j<ord && checkhomo;++j) {
			for (i=0;i<ord;++i) {
				if (homo[ga->op(i,j)] != ga->op(homo[i],homo[j])) {
					checkhomo = false;
					break;
				}
				if (homo[gm->opm(i,j)] != gm->opm(homo[i],homo[j])) {
					checkhomo = false;
					break;
				}
			}
		}
		//if (true) {
		if (checkhomo) {
			stringstream ss;
			for (i=0;i<ord;++i) {
				if (homo[i] >= 10)
					ss << char('A'+homo[i]-10)  << " ";
				else
					ss << homo[i] << " ";
			}
			lbhomo->addstring(ss.str().c_str());
		}
		S32 dig = ord-1;
		while(dig>=2) {
			++homo[dig];
			if (homo[dig] >= ord) {
				homo[dig] = 0;
				--dig;
				if (dig>=ord)
					break;
			} else {
				break;
			}
		}
		if (dig<2)
			break;
	}
	//for (i=0;i<200;++i) {
	//	sprintf(s,"0 1 2 3 4 5 6 7 8 9 A",i);
	//}
	lbhomo->setidxc(0);
}

//#define WIDX

// returns what bits to flip
#ifdef WIDX
vector<S32> calcsyn(const ply& r,const ply& p,S32 estdist,const vector<S32>& widx,S32 moddeg,bool* allzeros)
#else
vector<S32> calcsyn(const ply& r,const ply& p,S32 estdist,S32 moddeg,bool* allzeros)
#endif
{
	vector<S32> ret; // what bits to flip
	vector<ply> syn(estdist);
	*allzeros = true;
	for (S32 j=1;j<estdist;++j) {
		ply w;
		w.monomial(1,j);
		syn[j] = r.compose(w)%p;
		if (!syn[j].iszero())
			*allzeros = false;
		S32 rp = syn[j].getrep();
#ifdef WIDX
//		logger("\t\tsyn at r(w(%d)) = '%s', w(%d)\n",j,syn[j].print().c_str(),widx[rp]);
#else
//		logger("\t\tsyn at r(w(%d)) = '%s'\n",j,syn[j].print().c_str());
#endif
	}
	if (*allzeros) {
		logger("all zeros\n");
		return ret;
	}
#if 0
	if (estdist == 3) {
		S32 div = widx[syn[2].getrep()] - widx[syn[1].getrep()];
		if (div < 0)
			div += moddeg;
		logger("\t\tbit %d is flipped\n",div);
	}
#endif
#if 1
	while (estdist >= 3 && (estdist&1)) {
		S32 res = 0;
		ret = calcerrors(syn,moddeg,estdist,&res);
		if (res>0)
			break;
		if (res<0) {
			ret.clear();
			return ret;
		}
		// logger("adjusting estdist from %d to %d\n",estdist,estdist-2);
		estdist -= 2;
	}
#endif
	return ret;
}



#if 1
S32 testgenf(const ply& g,const ply& h,const ply& pp,S32 estdist) // return mindist, 0 if can't be calculated
{
//	bool full = false;
	bool full = true;
	ply mod = g*h;
	S32 genl = mod.getdeg() - g.getdeg();
	S32 gd = g.getdeg();
	S32 gen = 1 << genl;
	logger_indent();
	logger("testgenf\n");
	logger("-----------------\n");
	logger("bitcode (%d,%d): g = '%s', h = '%s', g*h = '%s'\n",mod.getdeg(),mod.getdeg()-g.getdeg(),g.print().c_str(),h.print().c_str(),(g*h).print().c_str());
	if (genl > 17) {
		logger("gen too big skipping...\n");
		logger("end testgenf\n");
		logger_unindent();
		return 0;
	}
//	if (gd != 14) { // (15,1) dist = 15 
//	if (gd != 10) { // (11,1) dist = 11
//	if (gd != 10) { // (15,5) dist = 7 
//	if (gd != 10 && gd != 3 && gd != 2 && gd != 8) {
//	if (gd != 8) { // (15,7) dist = 5
//	if (gd != 3) { // (7,4) dist = 3
//	if (gd != 6) { // (9,3) dist = 3
//	if (gd != 4) { // (5,1) dist = 5
//	if (gd != 2) { // (3,1) dist = 3
	if (false) {
		logger("not right size generator, skipping...\n");
		logger("end testgenf\n");
		logger_unindent();
		return 0;
	}
//	S32 mindist = 1000000;
	S32 mindist = 1000;

	if (estdist>=3 && ply::getmod()==2 && ply::getpackprint()) {
		field2::setprim(pp.print().c_str());
	}
	logger("--------------------\n");
	const S32 maxerr = 100;
	const S32 badscan = maxerr - 1;
	const S32 badlogic = maxerr - 2;
	vector<S32> errcnt(maxerr);
	// calc widx
#ifdef WIDX
	vector<S32> widx(1<<pp.getdeg()); // build a way to get i from w^i (log) given getrep poly mod g, degree one less because it's a residue
	S32 maxprim = 1<<10; // too much?
	for (S32 j=2;j<maxprim;++j) {
//	for (S32 j=3;j<=3;++j) {
		S32 i;
		ply pb;
		for (i=0;i<(1<<pp.getdeg())-1;++i) {
			pb.setrep(j);
			ply p = pb.pow(i);
//			ply p = pb.pow(i % mod.getdeg()); // try this for speedup
//			p.monomial(1,i);
			p = p % pp;
			S32 rp = p.getrep();
			if (widx[rp] > 0) {
				logger("i = %d, j = %d, prim '%s' not prim, trying new prim...\n",i,j,pb.print().c_str());
				widx.assign(1<<pp.getdeg(),0);
				break;
			}
			//logger("w(%d) = '%s'\n",i,p.print().c_str());
			widx[rp] = i;
		}
		if (i == ((1<<pp.getdeg())-1)) {
			logger("prim w = '%s'\n",pb.print().c_str());
			break;
		}
	}
	widx[0] = -1; // zero poly
#endif
	// end calc widx
	for (S32 i=0;i<=gen;++i) {
		ply rp;
		rp.setrep(i);
		ply c = g*rp;
		ply hc = h*c;
		ply r,q = hc.divmod(mod,r);
		S32 d = c.dist0();
		if (d<mindist && i!=0 && i!=gen)
			mindist=d;
#if 1
		if (i == gen)
			logger("\n");
		logger("c = '%16s', h*c = '%24s', / '%16s', q = '%9s', r = '%16s', d=%d\n",
			c.print().c_str(),hc.print().c_str(),mod.print().c_str(),q.print().c_str(),r.print().c_str(),d);
		if (i!=gen) {
			bool az = false;
#ifdef WIDX
			vector<S32> bits = calcsyn(c,pp,estdist,widx,mod.getdeg(),&az);
#else
			vector<S32> bits = calcsyn(c,pp,estdist,mod.getdeg(),&az);
#endif
			if (az) { // returns true on zero syndrome
				++errcnt[0];
			} else
				++errcnt[badlogic];
		}
#endif
	}
	if (errcnt[0])
		logger("no error count = %5d\n",errcnt[0]);
	if (errcnt[badlogic])
		logger("badlogic count = %5d\n",errcnt[badlogic]);
	logger("total =          %5d\n",gen);

	logger("\n++++++++++++++++++++++++++++++++++++++\n");
//	if (full && mod.getdeg() == 15 && mod.getdeg()-g.getdeg() == 5) {
	if (full && mod.getdeg() == 15 && mod.getdeg()-g.getdeg() == 5 && !fileexist("decode_data.bin") ) {
		logger("building (15,5) decoder file\n");
		deco f = {0,-1};
		decode_data = new deco[32768];
		::fill(decode_data,decode_data+32768,f);
	}
	if (full && mod.getdeg()<=17) {
		errcnt.assign(maxerr,0);
		S32 full = 1 << mod.getdeg();
//		const S32 maxfull = 5000;
		const S32 maxfull = 10000000;
		if (full > maxfull) {
			logger("cutting back on full\n");
			full = maxfull;
		}
		for (S32 i=0;i<=full;++i) {
			if (i%100 == 0) {
				video_lock();
				clipclear32(B32,C32GREEN);
				outtextxybf32(B32,10,10,C32BLACK,C32WHITE,"full = %d",i);
				video_unlock();
				video_paintwindow(0);
//				waitframe();
				checkmessages();
				debprocesskey();
			}
			ply c;
			c.setrep(i);
			ply hc = h*c;
			ply r,q = hc.divmod(mod,r);
			if (i == full)
				logger("\n");
#if 1
			logger("======================\nc = '%16s', h*c = '%24s', / '%16s', q = '%9s', r = '%16s', d=%d\n",
			  c.print().c_str(),hc.print().c_str(),mod.print().c_str(),q.print().c_str(),r.print().c_str(),c.dist0());
			// calc syndromes, based on estdist
			if (i!=full) {
				bool az = false;
#ifdef WIDX
				vector<S32>bits = calcsyn(c,pp,estdist,widx,mod.getdeg(),&az); // returns true on zero syndrome
#else
				vector<S32>bits = calcsyn(c,pp,estdist,mod.getdeg(),&az); // returns true on zero syndrome
#endif
				if (az) {
					++errcnt[0]; // no errors
					if (decode_data) {
						decode_data[i].cod = i;
						decode_data[i].nbits = 0;
					}
				} else {
					if (bits.size()) {
						U32 k;
						// fix corrupted code word
						ply bf;
						for (k=0;k<bits.size();++k) {
							ply m;
							m.monomial(1,bits[k]);
							bf = bf + m;
						}
						logger(" non zero syndrome: %2d bit flips are ( '%s' )\n",bits.size(),bf.print().c_str());
						ply c2 = c + bf;
						q = c2.divmod(pp,r);
						logger("c2 = '%s', q = '%s', r = '%s'\n",
							c2.print().c_str(),q.print().c_str(),r.print().c_str());
						if (!r.iszero()) {
							logger("badlogic in decoder!\n");
							if (decode_data) {
								decode_data[i].cod = i;
								decode_data[i].nbits = -2; // very bad
							}
							++errcnt[badlogic];
						} else {
							if (decode_data) {
								decode_data[i].cod = c2.getrep();
								decode_data[i].nbits = bits.size();
							}
							++errcnt[bits.size()];
						}
					} else {
						logger("badscan in decoder!, rescan\n");
						if (decode_data) {
							decode_data[i].cod = i;
							decode_data[i].nbits = -1;
						}
						++errcnt[badscan];
					}
				}
			}
#endif
		}
		for (S32 i=0;i<maxerr-2;++i)
			if (errcnt[i])
				logger(" %2d bit error count =  %5d\n",i,errcnt[i]);
		if (errcnt[badscan])
				logger("badscan count =        %5d\n",errcnt[badscan]);
		if (errcnt[badlogic])
			logger("badlogic error count = %5d\n",errcnt[badlogic]);
		logger("total =                %5d\n",full);
		if (decode_data) {
			FILE* fh = fopen2("decode_data.bin","wb");
			for (S32 i=0;i<32768;++i) {
				filewriteU32LE(fh,decode_data[i].cod);
				filewriteU32LE(fh,decode_data[i].nbits);
			}
			fclose(fh);
			delete[] decode_data;
			decode_data = 0;
		}
	}
	logger("actual dist = %d, estimated dist = %d\n",mindist,estdist);
	if (mindist!=estdist)
		logger("dist mismatch!\n");
	logger("end testgenf\n");
	logger_unindent();
	return mindist;
}
#endif



#if 1
void testcyclo()
{
	S32 oldmod = ply::getmod();
	bool oldprint = ply::getpackprint();
	ply::setmod(2);
	S32 k;
	for (k=3;k<=9;k+=2) {
//	for (k=3;k<=11;k+=2) {
//	for (k=3;k<=34;k+=2) {
//	for (k=3;k<=64;k+=2) {

//	for (k=3;k<=3;k+=2) {
//	for (k=5;k<=5;k+=2) {
//	for (k=7;k<=7;k+=2) {
//	for (k=9;k<=9;k+=2) {
//	for (k=11;k<=11;k+=2) {
//	for (k=13;k<=13;k+=2) {
//	for (k=15;k<=15;k+=2) {
//	for (k=17;k<=17;k+=2) {
//	for (k=255;k<=255;k+=2) {	
				video_lock();
				clipclear32(B32,C32GREEN);
				outtextxybf32(B32,10,10,C32BLACK,C32WHITE,"x^%d+1",k);
				video_unlock();
				video_paintwindow(0);
//				waitframe();
				checkmessages();
				debprocesskey();
		ply cyc;
		cyc.monomial(1,k);
		cyc.inc();
		ply::setpackprint(false);
		logger("\n================================================\ntest cyclo1  c = '%s'\n",cyc.print().c_str());
// get factors and lowest prim poly factor
		ply::setpackprint(true);
		const vector<ply> f = cyc.factors();
		S32 i,n=f.size();
		const ply* pp = 0;
		for (i=0;i<n;++i) {
//		for (i=n-1;i>=0;--i) {
			const ply& p = f[i];
			if (p.isprim(k)) {
				pp = &p;
				break;
			}
		}
		if (!pp)
			errorexit("can't find minimal poly for '%s'",cyc.print().c_str());
		logger("pp is prim '%s'\n",pp->print().c_str());
#if 1
// calc primlist
		vector<S32> primlist(k); // index of poly in factor list, primlist[w(i)] = which factor
		::fill(primlist.begin(),primlist.end(),-1);
		S32 j; // w0 to w(k-1)
		for (j=0;j<k;++j) {
			if (primlist[j] >= 0)
				continue; // already visited
			ply c;
			c.monomial(1,j);
			for (i=0;i<n;++i) { // check diff prim polys
				ply eval = f[i].compose(c) % *pp; // see if a root a a factor
				if (eval.iszero()) {
					break;
				}
			}
			if (i == n)
				errorexit("can't solve poly i,j,k,n = %d,%d,%d",i,j,k,n);
			primlist[j] = i; // primlist[w(j)] = factor index
			// shortcut, run through squares, they are also roots of this factor
			S32 js = j;
			while(true) {
				js *= 2;
				js %= k;
				if (primlist[js] >= 0)
					break; // hit a visited, done
				primlist[js] = i;
			}
		}
		for (i=0;i<n;++i) { // run through factors, print which w's landed into which factor
			logger("min %c '%s' : ",f[i].isprim(k)?'P':' ',f[i].print().c_str());
			for (j=0;j<k;++j)
				if (primlist[j] == i)
					logger("%d ",j);
			logger("\n");
		}
		// build various generators
		{
			ply g=1,h;
			vector<bool> visited(n); // which factors are being used by generator
			vector<ply> gs;
			vector<ply> hs;
			vector<S32> estdists;
			i = 1; // w(i) run through all roots of x^k-1 mod 2
			while(i<k) {
				S32 fi = primlist[i];
				if (!visited[fi]) {
					visited[fi] = true;
					const ply& pf = f[fi];
					g = g*pf;
					h = cyc/g;
					//S32 md = testgenf(g,h);
					gs.push_back(g);
					hs.push_back(h);
					// look ahead for other prims already included (number of consecutive root prims in included factors)
					j = i;
					while(j < k-1) {
						++j;
						fi = primlist[j];
						if (!visited[fi]) {
							--j;
							break;
						}
					}
					// logger("estimated dist = %d\n",j+1); // some actual dist > est dist, rare see x17-1 mod2 with gen '100111001' act = 5, est = 3
					estdists.push_back(j+1);
//					if (md != j+1 && md != 0)
//						logger("dist mismatch estdist = %d, actdist = %d\n",j+1,md);
				}
				++i;
			}
			S32 ng = gs.size();
			for (i=0;i<ng;++i) {
				//S32 md = testgenf(gs[i],hs[i]);
				logger("g = '%s', h = '%s', prim = '%s', est dist = %d\n",gs[i].print().c_str(),hs[i].print().c_str(),pp->print().c_str(),estdists[i]);
				testgenf(gs[i],hs[i],*pp,estdists[i]);
			}
			
		}
		// end build various generators
// end calc primlist
#endif
		logger("\n");
	}
	ply::setmod(oldmod);
	ply::setpackprint(oldprint);
	logger("end test cyclo\n");
}
#endif

#if 1
void check15() // special check of 1011110 in (15,7) g = 111010001, h = 11010001
{
	S32 i,j;
#if 1
	ply g("111010001"); // check (15,7) dist 5 t=3, est err 0,1,2 does have some unique err of 3 bits, (include?)
	S32 ng = 128;
#else
	ply g("10100110111"); // check (15,5) dist 7 t=4, est err 0,1,2,3 no unique codes err of 4 bits
	S32 ng = 32;
#endif
	vector<ply> cds;
	for (i=0;i<ng;++i) {
		ply m;
		m.setrep(i);
		ply c = g*m;
		cds.push_back(c);
		logger("code = '%s'\n",c.print().c_str());
	}
	ply r;
	vector<S32> ds(16); // dist count 0 to 15
	while(r.getdeg()<15) {
		ds.assign(16,0);
		ply c;
		S32 bd = 1000000;
		for (i=0;i<ng;++i) {
			S32 d = r.dist(cds[i]);
			if (d<bd) {
				c = cds[i];
				bd = d;
			}
			++ds[d];
		}
		for (j=0;j<=15;++j) {
			if (ds[j])
				break;
		}
		{
			if (ds[j] == 1) {
				logger("dist at %d count is %3d received %s, unique closest code %s\n",j,ds[j],r.print().c_str(),c.print().c_str());
			} else {
				logger("dist at %d count is %3d received %s, non-unique\n",j,ds[j],r.print().c_str());
			}
			logger("\t\t\t");
			for (i=0;i<16;++i) {
				logger("%3d ",ds[i]);
			}
			logger("\n");
		}
		r.inc();
	}
}
#endif

#if 0
void testz2p()
{
	logger("test z2p\n");
	S32 i,j,n;
#if 1
	for (i=1000;i<1030;++i) {
		z2p p1;
		p1.setrep(i);
		logger("rep = %3d, deg = %d\n",i,p1.getdeg());
	}
	z2p p2;
	p2.setrep(~0);
	logger("rep = max, deg = %d\n",p2.getdeg());
#endif
	z2p p3("1011");
	logger("p3 = '%s'\n",p3.print().c_str());
	z2p p4(1,1,0,1);
	logger("p4 = '%s'\n",p4.print().c_str());
	vector<S32> c(5);
	c[0]=1;
	c[1]=1;
	c[2]=0;
	c[3]=0;
	c[4]=1;
	z2p p5(c);
	logger("p5 = '%s'\n",p5.print().c_str());
	z2p p6("1011");
	z2p p7("1101");
	z2p p8 = p6 * p7;
	logger("p8 = '%s'\n",p8.print().c_str());
	z2p p9 = p8 * 0;
	z2p p10 = p8 * 1;
	logger("p9 = '%s', p10 = '%s'\n",p9.print().c_str(),p10.print().c_str());
	z2p p11,p12;
	p11.monomial(1,31);
	p12.monomial(0,5);
	logger("p11 = '%s', p12 = '%s'\n",p11.print().c_str(),p12.print().c_str());
	z2p p13 = z2p("11").pow(31);
	logger("p13 = '%s'\n",p13.print().c_str());
	z2p p14 = z2p("11100000000000000010000000000001").compose("1");
	logger("p14 = '%s'\n",p14.print().c_str());
	logger("end test z2p\n\n");
	z2p p15,p16;
	for (j=1;j<16;++j) {
		p16.setrep(j);
		for (i=0;i<=16;++i) {
			z2p r,q;
			p15.setrep(i);
			q = p15.divmod(p16,r);
			//q = p15/p16;
			//r = p15%p16;
			logger("a = '%s', b = '%s', a/b = '%s', a%%b = '%s'\n",
			  p15.print().c_str(),p16.print().c_str(),q.print().c_str(),r.print().c_str());
		}
		logger("--------\n");
	}
	for (j=0;j<=31;++j) {
//		p16.setrep(j);
		p16.monomial(1,j);
		p16 = p16 + 1;
		logger_disableindent();
		logger("%s = ",p16.print().c_str());
		vector<z2p> p17v = p16.factors();
		n = p17v.size();
		for (i=0;i<n;++i)
			logger("(%s)",p17v[i].print().c_str());
		logger("\n");
		logger_enableindent();
	}
}
#endif

//#define havok
#ifdef havok
int fact1(int n)
{
	if (n==0)
		return 1;
	return fact1(n-1)*n;
}

int fact2(int n)
{
	int p=1;
	while(n)
		p *= n--;
	return p;
}

struct Node
{
	struct Node* next;
	int val;
};

Node fnode = {0,6};
Node enode = {&fnode,5};
Node dnode = {&enode,4};
Node cnode = {&dnode,3};
Node bnode = {&cnode,2};
Node anode = {&bnode,1};

/*struct Node* reverse(Node* n)
{
	return n;
}

void reverseAfter( struct Node* head, int val )
{
	while(head) {
		if (head->val == val) {
			head->next = reverse(head);
			return;
		}
		head = head->next;
	}
} */
#endif

#if 0
// jetttailfin, convert dds files to tga files
void dds2tga() 
{
	const S32 maxlog = 16;
	const S32 down[maxlog+1] = { // 0 to 16 // dds to tga downgrade table log2 dim
		0, // 0
		1, // 1
		2, // 2
		3, // 3

		4, // 4
		5, // 5
		6, // 6
		6, // 7

		7, // 8
		7, // 9
		8, // 10
		9, // 11

		9, // 12
		9, // 13
		9, // 14
		9, // 15

		9, // 16
	};
#if 1
	const S32 maxcnt = 10000;
#else
	const S32 maxcnt = 10;
#endif
	S32 maxdimx = 0;
	S32 maxdimy = 0;
	vector<string> names;
	pushandsetdir("/bnp_jetttailfin/vc_pcjet/jetttailfin/romfiles");
	scriptdir dirs(1);
	S32 cnt = 0;
	S32 m=dirs.num();
	for (S32 i=0;i<m;++i) {
		logger("dir is '%s'\n",dirs.idx(i).c_str());
		logger_indent();
		pushandsetdirdown(dirs.idx(i).c_str());
		scriptdir files(0);
		S32 j,n=files.num();
		for (j=0;j<n;++j) {
			if (isfileext(files.idx(j).c_str(),"dds")) {

				bitmap32* bm;
				bm = gfxread32(files.idx(j).c_str());
				S32 dx = bm->size.x;
				S32 dy = bm->size.y;
				logger("file is '%s', dim is %d,%d\n",files.idx(j).c_str(),dx,dy);
				if (dx > maxdimx)
					maxdimx = dx;
				if (dy > maxdimy)
					maxdimy = dy;
				S32 ldx = ilog2(dx);
				S32 ldy = ilog2(dy);
				if ((1<<ldx)!=dx || (1<<ldy)!=dy) {
					logger("\twarning: non power of 2 on '%s' %d,%d, adjusting\n",files.idx(j).c_str(),dx,dy);
					dx = 1<<ldx;
					dy = 1<<ldy;
					logger("\tnew power of 2 dim is %d,%d\n",dx,dy);
				}
				// downgrade
				if (ldx > maxlog || ldy > maxlog)
					errorexit("too big '%s' %d,%d",files.idx(j).c_str(),dx,dy);
				S32 ndx = 1<<down[ldx];
				S32 ndy = 1<<down[ldy];
					logger("\tnew downsized dim is %d,%d\n",ndx,ndy);
				bitmap32* nbm;
				if (ndx != dx || ndy != dy) {
					nbm = bitmap32alloc(ndx,ndy,C32BLACK);
					clipscaleblit32(bm,nbm);
					bitmap32free(bm);
				} else {
					nbm = bm; // pass ownership
				}
				// get new name and check with others
				C8 n[MAX_PATH];
				mgetname(files.idx(j).c_str(),n);
				string nf = n;
				// nf = "AbC";
				transform(nf.begin(), nf.end(), nf.begin(), ::tolower);
				vector<string>::iterator fnd = find(names.begin(),names.end(),nf);
				bool dup = false;
				if (fnd == names.end()) { // if new name
					logger("name '%s' added to the list\n",nf.c_str());
					names.push_back(nf);
				} else {
					logger("warning: name '%s' already in the list\n",fnd->c_str());
					dup = true;
				}
				// write tga file
				nf += ".tga";
				logger("\t write '%s'\n",nf.c_str());
#if 1
				if (!dup)
					gfxwrite32(nf.c_str(),nbm);
#endif
				bitmap32free(nbm);
				// count
				++cnt;
				if (cnt == maxcnt)
					break;
				// update os
				video_lock();
				clipclear32(B32,C32GREEN);
				outtextxybf32(B32,10,10,C32BLACK,C32WHITE,"file %d",cnt);
				video_unlock();
				video_paintwindow(0);
//				waitframe();
				checkmessages();
				debprocesskey();
			}
		}
		popdir();
		logger_unindent();
		if (cnt == maxcnt)
			break;
	}
	popdir();
	logger("count of .dds files is %d, maxdimx = %d, maxdimy = %d\n",cnt,maxdimx,maxdimy);
}
#endif
#if 0
void testhavok()
{
	int i;
	for (i=0;i<6;++i)
		logger("fact1(%d) = %d\n",i,fact1(i));
	for (i=0;i<6;++i)
		logger("fact2(%d) = %d\n",i,fact2(i));
//	reverseAfter(&anode,4);
	Node* n = &anode;
	logger("node = ");
	while(n) {
		logger("%d ",n->val);
		n = n->next;
	}
	logger("\n");
}
#endif
#if 0
void testgen()
{
	logger("testgen\n");
	struct gandh {
		ply g,h;
		bool full;
	};
	ply h1 = "11";
	ply h2 = "111";
	ply h3 = "10011";
	ply h4 = "11001";
	ply h5 = "11111";
	static gandh ghs[] = {
		// x3 + 1
		{ply("11"),ply("111"),true}, // (3,2) gen is (11) dist is 2
		{ply("111"),ply("11"),true}, // (3,2) gen is (11) dist is 2
		// x7 + 1
		{ply("1111111"),ply("11")},
		{ply("11101"),ply("1101")},
		{ply("1011"),ply("10111")},
		{ply("11"),ply("1111111")},
		// x9 + 1
		{ply("111111111"),ply("11")},
		{ply("1001001"),ply("1001")},
		{ply("1001"),ply("1001001")},
		{ply("11"),ply("111111111")},
/*		// x15 + 1
		 // with h1
		{h1,h3*h5*h2*h4},
		{h1*h3,h5*h2*h4},
		{h1*h3*h5,h2*h4},
		{h1*h3*h5*h2,h4},
		 // without h1
		{h3,h5*h2*h4*h1},
		{h3*h5,h2*h4*h1},
		{h3*h5*h2,h4*h1},
		{h3*h5*h2*h4,h1},
		 // extras
		{h5,h1*h2*h3*h4},
		{h5*h1,h2*h3*h4}, */
/*		{ply( 7),ply( 3),true},
		{ply("1011"),ply("10111"),true},
		{ply("1001001"),ply("1001")},
		{ply("111010001"),ply("11010001")},
		{ply("10100110111"),ply("101011")},
		{ply("11"),ply("111111")},
		{ply("101"),ply("10101")},
		{ply("111"),ply("11011")},
		{ply("10101"),ply("101")},
		{ply("1001"),ply("1001")} */
	};
	const S32 nghs = sizeof(ghs)/sizeof(ghs[0]);

//	static ply gs[] = {ply( 3),ply( 7),ply(11),ply("1001001"),ply("111010001"),ply("10100110111")};
//	static ply hs[] = {ply( 7),ply( 3),ply(23),ply("1001"),ply("11010001"),ply("101011")};
//	const S32 ngs = sizeof(gs)/sizeof(gs[0]);
	S32 j;
	for (j=0;j<nghs;++j) {
		ply g = ghs[j].g;
		ply h = ghs[j].h;
		ply mod = g*h;
		S32 gen = 1 << (mod.getdeg() - g.getdeg());
		logger("-----------------\n");
		logger("(%d,%d): g = '%s', h = '%s', g*h = '%s'\n",mod.getdeg(),mod.getdeg()-g.getdeg(),g.print().c_str(),h.print().c_str(),(g*h).print().c_str());
		if (ghs[j].full) {
			S32 full = 1 << mod.getdeg();
			for (S32 i=0;i<=full;++i) {
				ply c;
				c.setrep(i);
				ply hc = h*c;
				ply r,q = hc.divmod(mod,r);
				logger("c = '%16s', h*c = '%24s', / '%16s', q = '%9s', r = '%16s', d=%d\n",
					c.print().c_str(),hc.print().c_str(),mod.print().c_str(),q.print().c_str(),r.print().c_str(),c.dist0());
			}
			logger("\n");
		}
		S32 mindist = 1000;
		for (S32 i=0;i<=gen;++i) {
			ply rp;
			rp.setrep(i);
			ply c = g*rp;
			ply hc = h*c;
			ply r,q = hc.divmod(mod,r);
			S32 d = c.dist0();
			if (d<mindist && i!=0 && i!=gen)
				mindist=d;
			if (i == gen)
				logger("\n");
			logger("c = '%16s', h*c = '%24s', / '%16s', q = '%9s', r = '%16s', d=%d\n",
				c.print().c_str(),hc.print().c_str(),mod.print().c_str(),q.print().c_str(),r.print().c_str(),d);
		}
		logger("mindist = %d\n",mindist);
	}
	logger("end testgen\n");
}
#endif

#if 0
void testgen2()
{
	logger("testgen\n");
	struct gandh {
		z2p g,h;
		bool full;
	};
	static gandh ghs[] = {
		// x3 + 1
		{z2p( 3),z2p( 7),true}, // (3,2) gen is (11) dist is 2
//		{z2p( 7),z2p( 3),true}, // (3,2) gen is (11) dist is 2
		// x7 + 1
//		{z2p("1111111"),z2p("11")},
//		{z2p("11101"),z2p("1101")},
//		{z2p("1011"),z2p("10111")},
//		{z2p("11"),z2p("1111111")},
/*		{z2p( 7),z2p( 3),true},
		{z2p("1011"),z2p("10111"),true},
		{z2p("1001001"),z2p("1001")},
		{z2p("111010001"),z2p("11010001")},
		{z2p("10100110111"),z2p("101011")},
		{z2p("11"),z2p("111111")},
		{z2p("101"),z2p("10101")},
		{z2p("111"),z2p("11011")},
		{z2p("10101"),z2p("101")},
		{z2p("1001"),z2p("1001")} */
	};
	const S32 nghs = sizeof(ghs)/sizeof(ghs[0]);

//	static z2p gs[] = {z2p( 3),z2p( 7),z2p(11),z2p("1001001"),z2p("111010001"),z2p("10100110111")};
//	static z2p hs[] = {z2p( 7),z2p( 3),z2p(23),z2p("1001"),z2p("11010001"),z2p("101011")};
//	const S32 ngs = sizeof(gs)/sizeof(gs[0]);
	S32 j;
	for (j=0;j<nghs;++j) {
		z2p g = ghs[j].g;
		z2p h = ghs[j].h;
		z2p mod = g*h;
		S32 gen = 1 << (mod.getdeg() - g.getdeg());
		logger("-----------------\n");
		logger("(%d,%d): g = '%s', h = '%s', g*h = '%s'\n",mod.getdeg(),mod.getdeg()-g.getdeg(),g.print2().c_str(),h.print2().c_str(),(g*h).print2().c_str());
		if (ghs[j].full) {
			S32 full = 1 << mod.getdeg();
			for (S32 i=0;i<=full;++i) {
				z2p c(i);
				z2p hc = h*c;
				z2p r,q = hc.divmod(mod,r);
				logger("c = '%16s', h*c = '%24s', / '%16s', q = '%9s', r = '%16s', d=%d\n",
					c.print2().c_str(),hc.print2().c_str(),mod.print2().c_str(),q.print2().c_str(),r.print2().c_str(),c.dist0());
			}
			logger("\n");
		}
		S32 mindist = 1000;
		for (S32 i=0;i<=gen;++i) {
			z2p c = g*z2p(i);
			z2p hc = h*c;
			z2p r,q = hc.divmod(mod,r);
			S32 d = c.dist0();
			if (d<mindist && d!=0 && i!=gen)
				mindist=d;
			logger("c = '%16s', h*c = '%24s', / '%16s', q = '%9s', r = '%16s', d=%d\n",
				c.print2().c_str(),hc.print2().c_str(),mod.print2().c_str(),q.print2().c_str(),r.print2().c_str(),d);
		}
		logger("mindist = %d\n",mindist);
	}
	logger("end testgen\n");
}
#endif

#if 0
void testinc()
{
	bool oldprint = ply::getpackprint();
	ply::setpackprint(false);
	S32 oldmod = ply::getmod();
	ply::setmod(2);
	ply p;
	while(p.getdeg() != 5) {
		logger("poly inc = '%s'\n",p.print().c_str());
		p.inc();
	}
	ply::setmod(oldmod);
	ply::setpackprint(oldprint);
}
#endif

#if 0
void testmat()
{
	{
		augmatz am(11,2,2);
		static const U8 cd[] = {
			3,4, 5,
			6,7, 7
		};
		am.load(cd);
		am.print();
		vector<vector<zee2> > a = am.getvarsc();
		S32 i,j;
		S32 n = a.size();
		S32 m=0;
		if (n)
			m = a[0].size();
		logger("varsc\n");
		for (j=0;j<n;++j) {
				logger("var %3d: ",j);
			for (i=0;i<m;++i) {
				logger("%d ",a[j][i].v);
			}
			logger("\n");
		}
		logger("end varsc\n");
	}
	{
		augmatz am(2,5,5);
		static const U8 cd[] = {
			0,1,1,0,0, 0,
			1,1,0,0,0, 0,
			1,0,1,1,0, 0,
			1,1,0,1,0, 0,
			1,1,0,0,0, 0,
		};
		am.load(cd);
		am.print();
		vector<vector<zee2> > a = am.getvarsc();
		S32 i,j;
		S32 n = a.size();
		S32 m=0;
		if (n)
			m = a[0].size();
		logger("varsc\n");
		for (j=0;j<n;++j) {
				logger("var %3d: ",j);
			for (i=0;i<m;++i) {
				logger("%d ",a[j][i].v);
			}
			logger("\n");
		}
		logger("end varsc\n");
	}
// play with Berlekamp's algorithm
	{
		S32 i,j;
		S32 oldmod = ply::getmod();
		bool oldpp = ply::getpackprint();
		ply::setpackprint(true);
//		const S32 TESTMOD = 2;
		const S32 TESTMOD = 7;
		ply::setmod(TESTMOD);
//		ply f("x31+1"); // mod 2
//		ply f("x5+x4+1"); // mod 2
		ply f("x5+2x3++x2+x+1"); // mod 7
		S32 d = f.getdeg();
		augmatz am(TESTMOD,d,d);
		U8* cd = new U8[d*(d+1)];

		::fill(cd,cd+d*(d+1),0); // init to 0
		for (j=0;j<d;++j) { // powers, columns
			logger("X^%3d = ",j*TESTMOD);
			ply x(1,0); // X
			ply px = x.pow(j*TESTMOD); // X^TESTMOD
			px = px % f;
			logger("'%s'\n",px.print().c_str());
			for (i=0;i<d;++i) // rows
				//if (j==0 || j==1 || j==2)
					cd[(d+1)*i+j]=px.getcoef(i);
		}
		for (j=0;j<d;++j) { // subract identity, make px^TESTMOD - px mod f
			cd[(d+1)*j+j]=(cd[(d+1)*j+j]+TESTMOD-1)%TESTMOD;
		}

		am.load(cd);
		delete[] cd;
		am.print();
		vector<vector<zee2> > a = am.getvarsc();
		S32 n = a.size();
		S32 m=0;
		if (n)
			m = a[0].size();
		logger("varsc\n");
		for (j=0;j<n;++j) {
				logger("var %3d: ",j);
			for (i=0;i<m;++i) {
				logger("%d ",a[j][i].v);
			}
			logger("\n");
		}
		logger("end varsc\n");

		for (j=1;j<n;++j) {
			m = a[0].size();
			vector<S32> vc;
			for (i=0;i<m;++i)
				vc.push_back(a[j][i]);
			ply h = ply(vc);
			logger("h = '%s'\n",h.print().c_str());
			ply g = f.gcd(h);
			logger("g = '%s'\n",g.print().c_str());
		}
		ply::setmod(oldmod);
		ply::setpackprint(oldpp);
	}
}
#endif

#if 0
void testberl()
{
	S32 oldmod = ply::getmod();
	bool oldpp = ply::getpackprint();
	ply::setpackprint(true);
	ply f("x5+2x3+x2+x+1",7); // mod 7
	ply g = berl(f);
	logger("berl factor = '%s'\n",g.print().c_str());
	ply::setpackprint(oldpp);
	ply::setmod(oldmod);
}
#endif

#if 0
void testberl2()
{
	logger("testberl2\n");
	S32 oldmod = ply::getmod();
	bool oldpp = ply::getpackprint();
	ply::setpackprint(true);
	ply f("1",2);
	while(f.getdeg() < 10) {
		vector<ply> f1 = f.factors();
		ply::setberlfact(false);
		vector<ply> f2 = f.factors();
		ply::setberlfact(true); // default
		if (f1 != f2)
			errorexit("diff factors");
		f.inc();
	}
	ply::setpackprint(oldpp);
	ply::setmod(oldmod);
	logger("end testberl2\n");
}
#endif
#if 0
void testsquare()
{
	logger("testsquares\n");
	S32 oldmod = ply::getmod();
	bool oldpp = ply::getpackprint();
	ply::setmod(2);
	ply::setpackprint(false);
	ply f;
	S32 i;
	for (i=1;i<500;++i) {
		f.monomial(1,i);
		f.inc();
		logger("f = '%s', ",f.print().c_str());
		ply d = f.deriv();
		if (d.iszero())
			logger("perfect square\n");
		else {
			ply g = f.gcd(d);
			if (g.getdeg())
				logger("NON square free\n");
			else
				logger("square free\n");
		}
	}
	ply::setpackprint(oldpp);
	ply::setmod(oldmod);
	logger("end testsquares\n");
}
#endif
#if 0
void testprim()
{
	logger("testprim\n");
	S32 oldmod = ply::getmod();
	bool oldpp = ply::getpackprint();
	ply::setpackprint(true);
	ply f("10",2);
	while(f.getdeg() < 8) {
		vector<ply> f1 = f.factors();
		logger("ply = '%s",f.print().c_str());
		if (f1.size()>1) {
			S32 i,n=f1.size();
			logger("has factors: ");
			for (i=0;i<n;++i)
				logger("(%s)",f1[i].print().c_str());
			logger("\n");
		} else {
			logger("is minimal ");
			if (f.isprim())
				logger("primitive\n");
			else
				logger("not primitive\n");
		}
		f.inc();
	}
	ply::setpackprint(oldpp);
	ply::setmod(oldmod);
	logger("end testprim\n");
}
#endif

U32 encoder(U32 m)
{
	ply pm;
	pm.setrep(m<<10);
	ply g = "10100110111";
	ply pc = pm%g;
	pc = pc + pm;
	return pc.getrep();
}

U32 decoder(U32 c,S32* nbits,bool* err)
{
	if (!decode_data) {
		*nbits = 0;
		*err = true;
		return c;
	}
	U32 f;
	deco* d = decode_data + c;
	if (d->nbits>=0) {
		f = d->cod;
		*err = false;
		*nbits = d->nbits;
	} else {
		f = c;
		*err = true;
		*nbits = 0;
	}
	return f;
}

void calcbch_15_5(S32 knd,S32 bit)
{
	S32 savemod = ply::getmod();
	ply::setmod(2); // just for bch, LAME static setting !!
	static C8 s[2];
	bool err = false;
	S32 nbits = 0;
	if (knd == 0) { // message
		mess ^= (1<<bit);
		code = encoder(mess);
		fixe = decoder(code,&nbits,&err);
	} else { // code
		code ^= (1<<bit);
		fixe = decoder(code,&nbits,&err);
	}
	S32 i;
	for (i=0;i<5;++i) {
		U32 b = (mess>>i)&1;
		s[0] = '0' + b;
		bm[i]->settname(s);
	}
	for (i=0;i<15;++i) {
		U32 b = (code>>i)&1;
		s[0] = '0' + b;
		bc[i]->settname(s);
	}
	for (i=0;i<15;++i) {
		U32 b = (fixe>>i)&1;
//		if (err)
//			s[0] = 'E';
//		else
			s[0] = '0' + b;
		tc[i]->settname(s);
	}
	C8 str[100];
	if (err)
		sprintf(str,"decode err");
	else
		sprintf(str,"%d bit errs",nbits);
	tnbits->settname(str);
	ply::setmod(savemod);
}

#ifdef USENS
}
#endif
void ifield_init()
{
	pushandsetdir("ifield");
	ga=new group(INITSIZE,GAOFFX,GAOFFY,GSQSIZE,false);
	gm=new group(INITSIZE,GMOFFX,GMOFFY,GSQSIZE,true);
	video_setupwindow(1024,768);

	ply::setmod(2);
//	int i;
//	test_augmatn();
//	testcyclo();
//	check15();
//	testz2p();
//	testgen2();
//	testgen();
//	testhavok();
//	dds2tga();
//	testinc();
//	testmat();
//	testberl();
//	testberl2();
//	testsquare();
//	testprim(); // find primitive polys from list of irreducible polys

// ui
	rl=res_loadfile("ifieldres.txt");
	bquit=rl->find<pbut>("BQUIT");
	bsmaller=rl->find<pbut>("BSMALLER");
	bbigger=rl->find<pbut>("BBIGGER");
	breseta=rl->find<pbut>("BRESETA");
	bbackupa=rl->find<pbut>("BBACKUPA");
	bresetm=rl->find<pbut>("BRESETM");
	bbackupm=rl->find<pbut>("BBACKUPM");
	epolya=rl->find<edit>("EPOLYA");
	epolyb=rl->find<edit>("EPOLYB");
	epolym=rl->find<edit>("EPOLYM");
	tpolyr=rl->find<text>("TPOLYR");
	tpolyrm=rl->find<text>("TPOLYRM");
	bcalcpolyfield=rl->find<pbut>("BPOLY");
	bcalcring=rl->find<pbut>("BRING");
	lbhomo=rl->find<listbox>("HOMO");
	bhomo=rl->find<pbut>("BHOMO");

	bplus=rl->find<pbut>("BPLUS");
	bminus=rl->find<pbut>("BMINUS");
	bmult=rl->find<pbut>("BMULT");
	bdiv=rl->find<pbut>("BDIV");
	bmod=rl->find<pbut>("BMOD");
	bgcd=rl->find<pbut>("BGCD");
	bcompose=rl->find<pbut>("BCOMPOSE");
	bfactor=rl->find<pbut>("BFACTOR");
	bderiv=rl->find<pbut>("BDERIV");
	tmod=rl->find<text>("TMOD");
	hmod=rl->find<hscroll>("HMOD");
	bpp=rl->find<pbut>("BPP");
	tpp=rl->find<text>("TPP");
	S32 i;
	for (i=0;i<5;++i) {
		C8 s[100];
		sprintf(s,"BM%d",i);
		bm[i] = rl->find<pbut>(s);
	}
	for (i=0;i<15;++i) {
		C8 s[100];
		sprintf(s,"BC%d",i);
		bc[i] = rl->find<pbut>(s);
	}
	for (i=0;i<15;++i) {
		C8 s[100];
		sprintf(s,"T%d",i);
		tc[i] = rl->find<text>(s);
	}
	tnbits = rl->find<text>("TNBITS");
	mess = 0;
	code = 0;
	fixe = 0;
	if (fileexist("decode_data.bin")) {
		FILE* fh = fopen2("decode_data.bin","rb");
		decode_data = new deco[32768];
		S32 i;
		for (i=0;i<32768;++i) {
			decode_data[i].cod = filereadU32LE(fh);
			decode_data[i].nbits = filereadU32LE(fh);
		}
		fclose(fh);
	}
	inithmod();
	settmod();
	focus=oldfocus=0;
// end ui
//	resetidx=0;
	ga->resetgroup();
	gm->resetgroup();
}

void ifield_proc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
// ui
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
// quit
	if (ret==1) {
		if (focus == bquit) {
			poporchangestate(STATE_MAINMENU);
		} else if (focus == bsmaller) {
			int orda = ga->getorder();
			if (orda>2) {
				--orda;
				delete ga;
				ga = new group(orda,GAOFFX,GAOFFY,GSQSIZE,false);
			}
			int ordm = gm->getorder();
			if (ordm>2) {
				--ordm;
				delete gm;
				gm = new group(ordm,GMOFFX,GMOFFY,GSQSIZE,true);
			}
			if (orda <= group::MAXHOMOSIZE) {
				lbhomo->setvis(true);
				bhomo->setvis(true);
			}
		} else if (focus == bbigger) {
			int orda = ga->getorder();
			if (orda<group::MAXBSIZE) {
				++orda;
				delete ga;
				ga = new group(orda,GAOFFX,GAOFFY,GSQSIZE,false);
			}
			int ordm = gm->getorder();
			if (ordm<group::MAXBSIZE) {
				++ordm;
				delete gm;
				gm = new group(ordm,GMOFFX,GMOFFY,GSQSIZE,true);
			}
			if (orda > group::MAXHOMOSIZE) {
				lbhomo->setvis(false);
				bhomo->setvis(false);
			}
		} else if (focus == bbackupa) {
			ga->backupgroup();
		} else if (focus == breseta) {
			ga->resetgroup();
		} else if (focus == bbackupm) {
			gm->backupgroup();
		} else if (focus == bresetm) {
			gm->resetgroup();
		} else if (focus == bcalcring) {
			calcring();
		} else if (focus == bcalcpolyfield) {
			calcpolyfield();
		} else if (focus == bplus) {
			calcplus();
		} else if (focus == bminus) {
			calcminus();
		} else if (focus == bmult) {
			calcmult();
		} else if (focus == bdiv) {
			calcdiv();
		} else if (focus == bmod) {
			calcmod();
		} else if (focus == bgcd) {
			calcgcd();
		} else if (focus == bcompose) {
			calccompose();
		} else if (focus == bfactor) {
			calcfactor(ply(epolya->gettname()));
		} else if (focus == bderiv) {
			calcderiv(ply(epolya->gettname()));
		} else if (focus == bpp) {
			bool pp = ply::getpackprint();
			pp = !pp;
			ply::setpackprint(pp);
			tpp->settname(pp?"Pack Print":"Wide Print");
		} else if (focus == bhomo) {
			calc_homo();
		} else {
			S32 i;
			for (i=0;i<5;++i) {
				if (focus == bm[i]) {
					calcbch_15_5(0,i);
					break;
				}
			} 
			if (i == 5) {
				for (i=0;i<15;++i) {
					if (focus == bc[i]) {
						calcbch_15_5(1,i);
						break;
					}
				} 
			}

		}
	}
	if (focus == hmod) {
		settmod();
	}
// end ui
	ga->procbuttons(gm);
	gm->procbuttons(ga);
	ga->checkgroup();
	gm->checkgroup();
	fe = checkfield(ga,gm);
}

void drawfielderror()
{
	const int RIGHT=20;
	const int DOWN=440;
	int offx = GAOFFX+30;
	int offy = (GAOFFY+GMOFFY)/2+150;
	offx -= 48;
	offy -= 24;
	const int EXT = 300;
	
	clipcircle32(B32,offx   +RIGHT,offy     +DOWN,10,fe.leftdistrib?C32GREEN:C32RED);
	if (!fe.leftdistrib)
		outtextxyf32(B32,offx+30+RIGHT,offy-4+DOWN,C32WHITE,"Left Dist %c*(%c+%c) != %c*%c+%c*%c",
		  fe.lac,fe.lbc,fe.lcc,
		  fe.lac,fe.lbc,fe.lac,fe.lcc);
	else
		outtextxy32 (B32,offx+30+RIGHT,offy   -4+DOWN,C32WHITE,"Left Dist a*(b+c)=a*b+a*c");
	
	clipcircle32(B32,offx   +RIGHT+EXT,offy  +DOWN,10,fe.rightdistrib?C32GREEN:C32RED);
	if (!fe.rightdistrib)
		outtextxyf32(B32,offx+30+RIGHT+EXT,offy-4+DOWN,C32WHITE,"Right Dist (%c+%c)*%c != %c*%c+%c*%c",
		  fe.rac,fe.rbc,fe.rcc,
		  fe.rac,fe.rcc,fe.rbc,fe.rcc);
	else
		outtextxy32 (B32,offx+30+RIGHT+EXT,offy-4+DOWN,C32WHITE,"Right Dist (a+b)*c=a*c+b*c");
}

void ifield_draw2d()
{
//	group::grouperror se=ga->geterror();
	clipclear32(B32,C32BLUE);
//	C32 bs=goodboard?C32GREEN:C32RED;
	rl->draw();
	ga->drawboard();
	gm->drawboard();
	ga->drawbuttons();
	gm->drawbuttons();
	ga->drawerror();
	gm->drawerror();
	drawfielderror();
	outtextxyf32(B32,WX/3,8,C32WHITE,"R %d",ga->getorder());
}

void ifield_exit()
{
	popdir();
	delete rl;
	delete ga;
	delete gm;
	delete[] decode_data;
	decode_data = 0;
}
