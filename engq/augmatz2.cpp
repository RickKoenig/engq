#include <m_eng.h>
#include "ring2.h"
#include "augmatz.h"

// works well with all mod's
vector<T> augmatz::axeb(T a,T b)
{
	S32 i;
	vector<T> ret;
	if (a.v == 0) {
		if (b.v == 0) {
			for (i=0;i<mod;++i) {
				ret.push_back(i);
			}
		}
		return ret;
	}
	S32 g = gcd(a,mod);
	if (g == 1) { // 1 solution
		S32 r = a.minv()*b;
		ret.push_back(r);
		return ret;
	}
	if (b%g != 0) // 0 solutions
		return ret;
	S32 s = mod/g;
	T::setdefmod(s);
	T ap = a.v/g;
	T bp = b.v/g;
	S32 r = ap.minv()*bp;
	T::setdefmod(mod);
//	r *= g;
	for (i=0;i<g;++i) {
		ret.push_back(r);
		r += s;
	}
	return ret;
}

bool augmatz::calcpivot2(S32 r,S32 c) // process this pivot, return true when done/works
{
//	gcon_printf("<<<<<< calc2pivot at %d,%d >>>>>>\n",r+1,c+1);
	T va = calcvals[r][c];
	bool ret = va.hasminv();
	S32 mul = 0;
	if (ret) {
		mul = va.minv();
	}
//	scale(r,mi); // the only divide
	S32 j;
	for (j=r+1;j<nr;++j) {
		if (calcvals[j][c]) {
			if (ret == true)
				add(r,j,mul * -calcvals[j][c]);
			else
				add(r,j,1);
		}
	}
	bool allzero = true;
	for (j=r+1;j<nr;++j) {
		if (calcvals[j][c]) {
			allzero = false;
			break;
		}
	}
	if (allzero == true)
		ret = true;
	if (ret) {
		pivotlistrow[r] = c;
		pivotlistcol[c] = r;
		lastpivotrow = r;
	}
	return ret;
}

void augmatz::calc2()
{
	gcon_printf("================ calc2 ================\n");
	reset();
	lastpivotrow = -1;
	printcalc();
	// find all pivots for each row
	S32 j;
	S32 c = 0; // walk
	S32 r = 0;
	for (c=0;c<nc;++c) {
		S32 watch = 0;
		bool done = false;
		while(!done) {
			++watch;
			if (watch == 100) {
				gcon_printf("big watch in calc2\n");
				done = true;
			}
			S32 bestg = mod;
			S32 bj = nr;
			for (j=r;j<nr;++j) { // find pivot in the column c
				if (calcvals[j][c].v == 1) { // later on, find pivot with lowest gcd(piv,mod)
					bestg = 1;
					bj = j;
					break;
				} else if (calcvals[j][c].v > 1) {
					S32 g = gcd(calcvals[j][c].v,mod);
					if (g < bestg) {
						bestg = g;
						bj = j;
					}
				}
			}
			if (bj == nr) {
//				gcon_printf("no pivot2 in column %d\n",c+1);
				done = true;
			} else if (bj == r) {
//				gcon_printf("pivot2 %d in column %d in row %d no swap\n",(S32)calcvals[bj][c],c+1,bj+1);
				if (calcpivot2(r,c)) {
					++r;
					done = true;
				}
			} else {
//				gcon_printf("pivot2 %d in column %d swap rows %d and %d\n",(S32)calcvals[bj][c],c+1,r+1,bj+1);
				swap(bj,r);
				if (calcpivot2(r,c)) {
					++r;
					done = true;
				}
			}
//			gcon_printf("calc piv2\n");
			// printcalc();
//			done = true;
		}
//		gcon_printf("calc piv2 done!\n");
	}
	gcon_printf("calc2 done!\n");
	printcalc();
}

vector<vector<T> > augmatz::getvars2(bool dosumsa)
{
	vector<vector<T> > ret;
	if (!dosumsa) {
		losum = -1;
		ret = getvars2sum();
		return ret;
	}
	S32 maxsum = (mod-1)*nc;
	S32 s;
	for (s=0;s<=maxsum;++s) {
//		gcon_printf("------------- losum = %d --------------\n",s);
		losum = s;
		ret = getvars2sum();
		if (lasterr) {
			gcon_printf("matherr at sum %d\n",s);
			return ret;
		}
		if (ret.size()) {
			if (s>0)
				gcon_printf("solutions with nonzero sum %d found\n",s);
			else 
				gcon_printf("solutions with sum %d found\n",s);
			losum = -1;
			return ret;
		}
	}
	gcon_printf("no solutions of any sum found\n");
	losum = -1;
	return ret;
}

vector<vector<T> > augmatz::getvars2sum()
{
//	dosumsa = true;
	lasterr = false;
	vector<vector<T> > ret;
	S32 i,j;
	if (nc == 1 && nr == 1) { // just do a 1 by 1 right here
		vector<T> vrs = axeb(vals[0][0],vals[0][1]);
		vector<T> r(1);
		S32 n = vrs.size();
		for (i=0;i<n;++i) {
			r[0] = vrs[i];
			ret.push_back(r);
		}
		gcon_printf("one equation, 1 unknown\n");
//		dosumsa = false;
		return ret;
	}
	calc2();
	// check for no solutions
	gcon_printf("last pivot row is %d\n",lastpivotrow+1);
	// walk down '0' rows
	for (j=lastpivotrow+1;j<nr;++j) {
		if (calcvals[j][nc].v!=0) { // example: 0x + 0y = 3
			gcon_printf("no solution: example: 0x + 0y = 3\n");
//			dosumsa = false;
			return ret;
		}
	}
	if (lastpivotrow==-1) { // no pivots at all
		ret = allvars(nc);
		gcon_printf("all solutions: example: 0x + 0y = 0\n");
//		dosumsa = false;
		return ret;
	}

	// backtrace, work backwards from last non trivial row
	S32 curpivotcol = nc-1;
	for (j=lastpivotrow;j>=0;--j) {
		S32 cpi = pivotlistrow[j];
		S32 nf = curpivotcol-cpi;
		if (curpivotcol == nc-1) {
			ret = allvars(nf);
			gcon_printf("after allvars %d\n",nf);
			printvars(ret);
		} else {
			ret = anyfront(ret,nf);
			gcon_printf("after anyfront %d\n",nf);
			printvars(ret);
		}
		ret = solve(ret,j,cpi);
		gcon_printf("after solve\n");
		printvars(ret);
		if (ret.empty()) {
			gcon_printf("no solution, aborting\n");
//			dosumsa = false;
			return ret;
		}
		curpivotcol = cpi-1;
	}
	ret = anyfront(ret,curpivotcol+1);
	gcon_printf("after last anyfront %d\n",curpivotcol+1);
	printvars(ret);
//	::sort(ret.begin(),ret.end());
//	dosumsa = false;
	return ret;
}

vector<vector<T> > augmatz::allvars(S32 n)
{
	gcon_printf("allvars %d, losum = %d\n",n,losum);
	vector<vector<T> > ret;
	if (n==0)
		return ret;
	vector<T> var(n);
	S32 i;
	S32 watch = 0;
	S32 cursum = 0;
	while(true) {
		ret.push_back(var);
		++watch;
		if (watch>maxwatch) {
			gcon_printf("watch hit in allvars\n");
			ret.clear();
			lasterr = true;
			return ret;
		}
		for (i=n-1;i>=0;--i) {
			cursum -= var[i];
			++var[i];
			cursum += var[i];
			if (losum>=0) {
				if (cursum>losum) {
					cursum -= var[i];
					var[i] = 0;
					continue;
				}
			}
			if (var[i].v != 0)
				break;
		}
		if (i<0)
			break;
	}
//	::sort(ret.begin(),ret.end()); // already sorted
	return ret;
}

#if 1
// assume vars all same size
vector<vector<T> > augmatz::anyfront(vector<vector<T> > invars,S32 n) { // prepend 'n' 'any's to list of invars
	S32 kn=invars.size();
	vector<vector<T> > ret;
	if (kn == 0) // no invars
		return ret;
	if (n == 0)
		return invars;
	gcon_printf("anyfront %d\n",n);
	S32 sv = invars[0].size();
	if (sv == 0)
		errorexit("bad invars[0].size()");
	// get to work
	vector<T> newvars(n+sv);
	S32 i,k;
	S32 watch = 0;
	for (k=0;k<kn;++k) {
		copy(invars[k].begin(),invars[k].end(),&newvars[n]);
		S32 cursum = getsum(invars[k]);
		vector<T> var(n);
		while(true) {
			// newvar
			copy(var.begin(),var.end(),newvars.begin());
			for (i=n-1;i>=0;--i) {
				cursum -= var[i];
				++var[i];
				cursum += var[i];
				if (losum>=0) {
					if (cursum>losum) {
						cursum -= var[i];
						var[i] = 0;
						continue;
					}
				}
				if (var[i].v != 0)
					break;
			} 
	/*		for (i=n-1;i>=0;--i) {
				++var[i];
				if (var[i].v != 0)
					break;
			} */

			if (i<0)
				break;
			ret.push_back(newvars);
			++watch;
			if (watch>maxwatch) {
				gcon_printf("watch hit in anyfront %d\n",n);
				ret.clear();
				lasterr = true;
				return ret;
			}
		}
	}
//	::sort(ret.begin(),ret.end()); // already sorted if invars is already sorted
	return ret;
}
#else
// assume vars all same size
vector<vector<T> > augmatz::anyfront(vector<vector<T> > invars,S32 n) { // prepend 'n' 'any's to list of invars
	S32 kn=invars.size();
	vector<vector<T> > ret;
	if (kn == 0) // no invars
		return ret;
	if (n == 0)
		return invars;
	gcon_printf("anyfront %d\n",n);
	S32 sv = invars[0].size();
	if (sv == 0)
		errorexit("bad invars[0].size()");
	// get to work
	vector<T> newvars(n+sv);
	vector<T> var(n);
	S32 i,k;
	S32 watch = 0;
	while(true) {
		// newvar
		copy(var.begin(),var.end(),newvars.begin());
		for (k=0;k<kn;++k) {
			copy(invars[k].begin(),invars[k].end(),&newvars[n]);
			ret.push_back(newvars);
			++watch;
			if (watch>maxwatch) {
				gcon_printf("watch hit in anyfront\n");
				ret.clear();
				lasterr = true;
				return ret;
			}
		}

		for (i=n-1;i>=0;--i) {
			++var[i];
			if (var[i].v != 0)
				break;
		}

		if (i<0)
			break;
	}
//	::sort(ret.begin(),ret.end()); // already sorted if invars is already sorted
	return ret;
}
#endif
// solve 1 row
vector<vector<T> > augmatz::solve(vector<vector<T> > invars,S32 rpi,S32 cpi) { // prepend solution's to list of invars
	S32 i,j,k;
//	gcon_printf("solve\n");
	if (!invars.size() && cpi != nc - 1) // only solve if on far right pivot or no invars
		return invars;
/*	if (cpi == nc-1) {
		if (invars.size() != 0)
			errorexit("bad var size: vars found with a far right pivot");
	} else {
		if ((S32)invars[0].size() != nc-1-cpi)
			errorexit("bad var size: varwidth %d, cpi %d, nc = %d",invars[0].size(),cpi,nc);
	} */
	vector<vector<T> > ret;
	S32 n=invars.size();
	if (n==0) { // no invars, just axeb and then return all solutions of this
		vector<T> reti(1);
		vector<T> vrs = axeb(calcvals[rpi][cpi],calcvals[rpi][nc]);
		S32 nvrs = vrs.size();
		for (i=0;i<nvrs;++i) {
			reti[0] = vrs[i];
			ret.push_back(reti);
		}
		gcon_printf("far right pivot, no invars\n");
		return ret;
	}
	// eval invars, solve axeb
	S32 watch = 0;
	for (j=0;j<n;++j) { // for each invar
		vector<T> reti(invars[0].size()+1);
		S32 sum=calcvals[rpi][nc];
		// eval and setup for axeb
		for (i=cpi;i<nc-1;++i) {
			sum -= calcvals[rpi][i+1]*invars[j][i-cpi];
		}
		vector<T> vrs = axeb(calcvals[rpi][cpi],sum);
		S32 nvrs = vrs.size();
		// paste invars
		::copy(invars[j].begin(),invars[j].end(),&reti[1]);
		for (k=0;k<nvrs;++k) { // solutions
			reti[0] = vrs[k]; // prepend solutions to each invar
			ret.push_back(reti);
			++watch;
			if (watch>maxwatch) {
				gcon_printf("watch hit in solve\n");
				ret.clear();
				lasterr = true;
				return ret;
			}
		}
	}
	::sort(ret.begin(),ret.end());
#if 0
	vector<vector<T> >::iterator it = std::unique(ret.begin(),ret.end()); // remove duplicates
	ret.erase(it,ret.end());
#endif
	return ret;
}
