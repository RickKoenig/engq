#include <m_eng.h>

#include "ring2.h"
#include "augmatz.h"

bool augmatz::lasterr = false;
S32 augmatz::losum = -1;

// works well with prime number mod's
augmatz::augmatz (S32 moda,S32 neqa,S32 nvara) : nr(neqa),nc(nvara),mod(moda),noreset(false)
{
	zee2::setdefmod(mod);
	S32 j;
	if (moda>zee2::maxmod || moda<2 || nc>600 || nr > 600 || nc<1 || nr<1)
		errorexit("autmatn: bad args for constructor");
	vals.resize(nr);
	calcvals.resize(nr);
	for (j=0;j<nr;++j) { // row
		vals[j].resize(nc+1);
		calcvals[j].resize(nc+1);
	}
	reset();
}

void augmatz::swap(S32 r1,S32 r2)
{
//	gcon_printf("swap rows %d and %d\n",r1+1,r2+1);
	S32 i;
	for (i=0;i<=nc;++i) {
		T t;
		t = calcvals[r1][i];
		calcvals[r1][i] = calcvals[r2][i];
		calcvals[r2][i] = t;
	}
}

void augmatz::scale(S32 r1,T f)
{
//	gcon_printf("scale row %d by %d\n",r1+1,(S32)f);
	S32 i;
	for (i=0;i<=nc;++i)
		calcvals[r1][i] *= f;
}

void augmatz::add(S32 r1,S32 r2,T f)
{
//	gcon_printf("add %d of row %d to row %d\n",(S32)f,r1+1,r2+1);
	S32 i;
	for (i=0;i<=nc;++i)
		calcvals[r2][i] += f*calcvals[r1][i];
}

// reset the calculations
void augmatz::setnoreset(bool set)
{
	noreset = set;
}

void augmatz::reset()
{
//	gcon_printf("reset\n");
	pivotlistrow = vector<S32>(nr,-1);
	pivotlistcol = vector<S32>(nc,-1);
	if (noreset)
		return;
	S32 i,j;
	for (j=0;j<nr;++j) { // row
		for (i=0;i<=nc;++i) { // column, include augment column
			calcvals[j][i] = vals[j][i];
		}
	}
}

/*bool augmatz::hasinv(T v)
{
	return v.hasminv();
}
*/
void augmatz::calcpivot(S32 r,S32 c) // process this pivot
{
//	gcon_printf("<<<<<< calcpivot at %d,%d >>>>>>\n",r+1,c+1);
	S32 mi = calcvals[r][c].minv();
	scale(r,mi); // the only divide
	S32 j;
	for (j=0;j<nr;++j) {
		if (j!=r) {
			if (calcvals[j][c]) {
				add(r,j,-calcvals[j][c]);
			}
		}
	}
	pivotlistrow[r] = c;
	pivotlistcol[c] = r;
}

void augmatz::calc()
{
	gcon_printf("================ calc ================\n");
	reset();
	// find all pivots for each row
	S32 j;
	S32 c = 0;
	S32 r = 0;
	for (c=0;c<nc;++c) {
		for (j=r;j<nr;++j) { // find pivot in the column c
			if (calcvals[j][c]) // later on, find largest pivot...
				break;
		}
		if (j==nr) {
			;//gcon_printf("no pivot in column %d\n",c+1);
		} else if (j==r) {
			//gcon_printf("pivot %d in column %d in row %d no swap\n",(S32)calcvals[j][c],c+1,j+1);
			calcpivot(r,c);
			++r;
		} else {
			//gcon_printf("pivot %d in column %d swap rows %d and %d\n",(S32)calcvals[j][c],c+1,r+1,j+1);
			swap(j,r);
			calcpivot(r,c);
			++r;
		}
		// printcalc();
	}
	gcon_printf("calc done!\n");
	printcalc();
}

void augmatz::print() const
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<nc;++i) {
			gcon_printf("%3d ",(S32)vals[j][i]); // X Y Z ...
		}
		gcon_printf("| %3d\n",(S32)vals[j][nc]); // Q
	}
}

void augmatz::printcalc() const
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		if (gcon)
			logger_disableindent();
		for (i=0;i<nc;++i) {
			gcon_printf("%2d",(S32)calcvals[j][i]); // X Y Z ...
		}
		gcon_printf(" | %2d\n",(S32)calcvals[j][nc]); // Q
		if (gcon)
			logger_enableindent();
	}
}

S32 augmatz::getsum(vector<T> t)
{
	S32 i,n=t.size();
	S32 sum=0;
	for (i=0;i<n;++i) {
//		gcon_printf("%2d",(S32)t[i]);
		sum += t[i];
	}
	return sum;
}

void augmatz::printvec(vector<T> t)
{
	S32 i,n=t.size();
	if (gcon)
		logger_disableindent();
	for (i=0;i<n;++i) {
		gcon_printf("%2d",(S32)t[i]);
	}
	S32 sum=getsum(t);
	gcon_printf(" sum = %d\n",sum); // Q
	if (gcon)
		logger_enableindent();
}

void augmatz::printvars(vector<vector<T> > vars)
{
	S32 i;
	if (vars.size()>10000) {
		gcon_printf("too many vars %d\n",vars.size());
		return;
	}
	gcon_printf("num vars %d\n",vars.size());
	for (i=0;i<(S32)vars.size();++i)
		printvec(vars[i]);
}

vector<T> augmatz::getnonhom() const // = column, return size in rows
{
	vector<T> ret(nr);
	S32 i;
	for (i=0;i<nr;++i)
		ret[i] = vals[i][nc];
	return ret;
}

vector<vector<T> > augmatz::getvarsc() // calc, return size in columns 'solution', or 0 size if no 'solution'
{
	vector<vector<T> >ret;
	calc();
	S32 i,j;
	for (i=0;i<nr;++i) { // check '0' rows for 'non 0 ' nonhom
		if (pivotlistrow[i]==-1) {
			if (calcvals[i][nc]) {
				return ret; // something like 0x + 0y + 0z = 5, no solution
			}
		}
	}
	// build nonhom
	vector<T> reti = vector<T>(nc);
	for (i=0;i<nr;++i) {
		j = pivotlistrow[i]; // get col(var)
		if (j>=0)
			reti[j] = calcvals[i][nc];
	}
	ret.push_back(reti);
	// build hom
	for (i=0;i<nc;++i) {
		if (pivotlistcol[i] == -1) { // this column has no pivot
			reti = vector<T>(nc);
			reti[i] = (T)1; // var
			for (j=0;j<nr;++j) {
				S32 pc = pivotlistrow[j];
				if (pc>=0) {
					reti[pc] = -calcvals[j][i];
				}
			}
			ret.push_back(reti);
		}
	}
	return ret;
}

vector<vector<T> > augmatz::getvars() // convert getvarsc to getvars
{
	lasterr = false;
//	S32 i;
//	return vars;
	if (mod%3 == 0 && mod!=6 && mod!=3) {
		augmatz augmod3 = modreduce(3); // down
		gcon_printf("augmod3\n");
		augmod3.print();
		vector<vector<T> > vars3 = augmod3.getvars(); // list of vars, math
/*		gcon_printf("varsmod2 math\n");
		for (i=0;i<(S32)vars2.size();++i) {
			augmod2.printvec(vars2[i]);
		} */
		vector<vector<T> > vars9_3 = upvars(vars3); // up
		vector<vector<T> > vars = checkvars(vars9_3);  // remove wrong solutions
		return vars;
	} else if (mod%2 == 0 && mod!=6 && mod!=2) {
		augmatz augmod2 = modreduce(2); // down
		gcon_printf("augmod2\n");
		augmod2.print();
		vector<vector<T> > vars2 = augmod2.getvars(); // list of vars, math
/*		gcon_printf("varsmod2 math\n");
		for (i=0;i<(S32)vars2.size();++i) {
			augmod2.printvec(vars2[i]);
		} */
		vector<vector<T> > vars4_2 = upvars(vars2); // up
		vector<vector<T> > vars = checkvars(vars4_2);  // remove wrong solutions
		return vars;
	} else if (mod == 6) {
		augmatz augmod2 = modreduce(2); // down
		gcon_printf("augmod2\n");
		augmod2.print();
		vector<vector<T> > vars2 = augmod2.getvars(); // list of vars, math
/*		gcon_printf("varsmod2 math\n");
		for (i=0;i<(S32)vars2.size();++i) {
			augmod2.printvec(vars2[i]);
		} */
		vector<vector<T> > vars6_2 = upvars(vars2); // up
/*		gcon_printf("varsmod6_2 math\n");
		for (i=0;i<(S32)vars6_2.size();++i) {
			augmod2.printvec(vars6_2[i]);
		} */
		augmatz augmod3 = modreduce(3); // down
		gcon_printf("augmod3\n");
		augmod3.print();
		vector<vector<T> > vars3 = augmod3.getvars(); // list of vars, math
/*		gcon_printf("varsmod3 math\n");
		for (i=0;i<(S32)vars3.size();++i) {
			augmod3.printvec(vars3[i]);
		} */
		vector<vector<T> > vars6_3 = upvars(vars3); // up
/*		gcon_printf("varsmod6_3 math\n");
		for (i=0;i<(S32)vars6_3.size();++i) {
			augmod3.printvec(vars6_3[i]);
		} */
//		return vars6_2;
//		return vars6_3;
		vector<vector<T> > vars = intersectsortvars(vars6_2,vars6_3); // merge, intersection
		return vars;
	}

	vector<vector<T> > varsc = getvarsc();
	if (varsc.size() <= 1)
		return varsc;
	vector<vector<T> > ret;
	vector<T> cs(varsc.size() - 1); // many solution const multipliers v0 + C1*v1 + C2*v2 etc...
	S32 watch = 0;
	while(true) {
		// make new var from a combination of cs
//		gcon_printf("cs = ");
//		printvec(cs);
		vector<T> newvar(nc);
		S32 i,j;
		for (j=0;j<nc;++j) {
			newvar[j] = varsc[0][j];
			for (i=0;i<(S32)cs.size();++i) {
				newvar[j] += cs[i]*varsc[i+1][j];
			}
		}
		ret.push_back(newvar);
		++watch;
		if (watch>maxwatch) {
			gcon_printf("watch hit in getvars\n");
			ret.clear();
			lasterr = true;
			return ret;
		}
		// walk thru the c's , break when done
		for (i=0;i<(S32)cs.size();++i) {
			T v = cs[i];
			++v;
			cs[i] = v;
			if ((S32)v != 0) {
				break;
			}
		}
		if (i==(S32)cs.size())
			break;
	}
	::sort(ret.begin(),ret.end());
	return ret;
}

vector<vector<T> > augmatz::getvarscomb(S32 mul) // convert getvarsc to getvars
{
	vector<vector<T> > ret;
	reset();
	setnoreset(true);
	add(0,1,mul);
//	scale(0,mul);
	ret = getvars();
	setnoreset(false);
	return ret;
}

vector<vector<T> > augmatz::getvarscomb2(S32 mul) // convert getvarsc to getvars
{
	vector<vector<T> > ret;
	reset();
	setnoreset(true);
	add(1,0,mul);
//	scale(0,mul);
	ret = getvars();
	setnoreset(false);
	return ret;
}

/*vector<vector<T> >augmatz::getvarsc() // calc, return size in columns 'solution', or 0 size if no 'solution'
{
	calc();
	vector<vector<T> >ret;
	S32 i;
	for (i=pivotlist.size();i<nr;++i) { // check '0' rows for 'non 0 ' nonhom
		if (calcvals[i][nc]!=0)
			return ret; // something like 0x + 0y + 0z = 5, no solution
	}
	vector<T> reti = vector<T>(nc);
	for (i=0;i<(S32)pivotlist.size();++i) {
		S32 j = pivotlist[i];
		reti[j] = calcvals[i][nc];
	}
	ret.push_back(reti);
	::sort(ret.begin(),ret.end());
	return ret;
}
*/
vector<vector<T> >augmatz::getvarsbrute() // calc, return size in columns 'solution', or 0 size if no 'solution'
{
	lasterr = false;
//	calc();
	vector<vector<T> >ret;
	S32 i;
	vector<T> testvar(nc);
	vector<T> nonhom = getnonhom();
	S32 watch = 0;
	while(true) {
		// test the var
		vector<T> mul = getmult(testvar);
		// if (true)
		if (mul == nonhom) {
			ret.push_back(testvar);
			++watch;
			if (watch>maxwatch) {
				gcon_printf("watch hit in getvarsbrute\n");
				ret.clear();
				lasterr = true;
				return ret;
			}
		}
		// inc the var, break when done
		for (i=0;i<(S32)testvar.size();++i) {
			T v = testvar[i];
			++v;
			testvar[i] = v;
			if ((S32)v != 0) {
				break;
			}
		}
		if (i==(S32)testvar.size())
			break;
	}
	::sort(ret.begin(),ret.end());
	return ret;
}

/*	S32 i;
	for (i=pivotlist.size();i<nr;++i) { // check '0' rows for 'non 0 ' nonhom
		if (calcvals[i][nc]!=0)
			return ret; // something like 0x + 0y + 0z = 5, no solution
	}
	vector<T> reti = vector<T>(nc);
	for (i=0;i<(S32)pivotlist.size();++i) {
		S32 j = pivotlist[i];
		reti[j] = calcvals[i][nc];
	}
	ret.push_back(reti);
	::sort(ret.begin(),ret.end());
	return ret;
} */

vector<T> augmatz::getmult(vector<T> vars) // run some vars to get = column, return size in rows
{
	vector<T> ret(nr);
	S32 i,j;
	for (j=0;j<nr;++j) {
		T v = 0;
		for (i=0;i<nc;++i) {
			v += vals[j][i]*vars[i];
		}
		ret[j] = v;
	}
	return ret;
}

bool augmatz::iter() // next aug matrix, iterate through all of them
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<=nc;++i) {
			T v = vals[j][i];
			++v;
			vals[j][i] = v;
			if ((S32)v != 0) {
				return true;
			}
		}
	}
	return false;
}

void augmatz::setrc(S32 r,S32 c,T v)
{
	vals[r][c] = v;
}

// reduce entire problem to a lower mod, default to that lower mod
augmatz augmatz::modreduce(S32 newmod)
{
	if (mod % newmod !=0)
		errorexit("bad modreduce %d to %d",mod,newmod);
	gcon_printf("mod down %d to %d\n",mod,newmod);
	augmatz ret(newmod,nr,nc);
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<=nc;++i) {
			ret.setrc(j,i,vals[j][i]%newmod);
		}
	}
	return ret;
}

// convert to a higher mod, default to that higher mod
vector<vector<T> > augmatz::upvars(vector<vector<T> > invars)
{
	S32 mod0 = T::getdefmod();
	T::setdefmod(mod);
	vector<vector<T> > ret;
	U32 k,up;
	up = mod/mod0;
	// up factor
	gcon_printf("mod up %d to %d, up %d\n",mod0,mod,up);
	S32 watch = 0;
	for (k=0;k<invars.size();++k) {
		vector<T> ele = invars[k];
		while(true) {
			// iterate over each invar
			ret.push_back(ele);
			++watch;
			if (watch>maxwatch) {
				gcon_printf("watch hit in upvars\n");
				ret.clear();
				lasterr = true;
				return ret;
			}
			S32 i = 0;
			while(true) {
				S32 val = ele[i].v;
				val += mod0;
				if (val<mod) {
					ele[i].v = val;
					break;
				}
				val -= mod;
				ele[i].v = val;
				++i;
				if (i>=nc)
					break;
			}
			if (i>=nc)
				break;
		}
	}
	::sort(ret.begin(),ret.end());
	return ret;
}

vector<vector<T> > augmatz::intersectsortvars(vector<vector<T> > invars0,vector<vector<T> > invars1)
{
	vector<vector<T> > ret;
	S32 i = 0;
	S32 j = 0;
	S32 m = invars0.size();
	S32 n = invars1.size();
	while(i<m && j<n) {
		if (invars0[i] == invars1[j]) {
			ret.push_back(invars0[i]);
			++i;
			++j;
		} else if (invars0[i] > invars1[j]) {
			++j;
		} else {
			++i;
		}
	}
	return ret;
}

vector<vector<T> > augmatz::checkvars(vector<vector<T> > invars)
{
	vector<vector<T> > ret;
	vector<T> nonhom = getnonhom();
	S32 i,n = invars.size();
	for (i=0;i<n;++i) {
		vector<T> res = getmult(invars[i]);
		if (res == nonhom)
			ret.push_back(invars[i]);
	}
	return ret;
}

void augmatz::load(const U8* data)
{
	S32 i,j;
	for (j=0;j<nr;++j) {
		for (i=0;i<=nc;++i) {
			vals[j][i] = data[i+(nc+1)*j];
		}
	}
}
