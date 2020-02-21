#include <m_eng.h>
#include "m_perf.h"
#include "u_smallgroups.h"

// try and figure out small groups
namespace smallgroups {

/////////// perm ////////////
perm::perm(const vector<S32>& va)
{
	pd=va;
}

perm::perm(const S32* a,S32 n)
{
	pd=vector<S32>(a,a+n);
}

perm::perm(S32 n)
{
	S32 i;
	for (i=0;i<n;++i)
		pd.push_back(i);
}

perm perm::operator*(const perm& rhs) // right to left
{
	S32 i,n=pd.size();
	perm p(n);
	for (i=0;i<n;++i) {
		S32 v=rhs.pd[i];
		p.pd[i]=pd[v];
	}
	return p;
}

bool perm::operator==(const perm& rhs) // are they the same
{
	S32 i,n=pd.size();
	for (i=0;i<n;++i)
		if (pd[i]!=rhs.pd[i])
			return false;
	return true;
}

perm perm::operator++() // maybe a simpler way
{
	S32 i,j,n=pd.size();
	if (n<=1)
		return *this; // must have 2 or more digits
// sorta like counting, but each digit has a different base
// convert to a form easier for counting, digits each have base like 4321
// rightmost digit is always 0 (base 1)
// next to right is 0 or 1 (base 2)
// digit is how many digits to the right of the digit are less than the digit
// 2013 becomes 2000, 0321 becomes 0210, 3210 becomes 3210
	vector<S32> cv;
	for (i=0;i<n;++i) {
		S32 v=pd[i];
		S32 vltc=0; // less than counter
		//S32 ncv=cv.size();
		for (j=i+1;j<n;++j)
			if (pd[j]<v)
				++vltc; // adjust down when lower digit to the left found
		cv.push_back(vltc);
	}
#if 0
	logger("reduced       :");
	for (j=0;j<n;++j)
		logger(" %2d",cv[j]);
	logger("\n");
#endif
// add 1 to 2nd to rightmost digit, it works
	S32 d=n-2;
	while(d>=0) {
		++cv[d];
		if (cv[d]==n-d) {
			cv[d]=0;
			--d;
		} else
			break;
	}
#if 0
	logger("added reduced :");
	for (j=0;j<n;++j)
		logger(" %2d",cv[j]);
	logger("\n");
#endif
// convert back, uses isused
	pd.clear(); // rewrite pd
	vector<bool> isused;
	isused.assign(n,false); // clear static dig used array and set to right size
	for (i=0;i<n;++i) { // run thru cv digits
		//S32 npd=pd.size();
		S32 v=0;
		S32 chk=0; // watchdog
		S32 cd=cv[i]+1; // countdown
		while(true) {
			if (chk>=n)
				errorexit("chk %d is >= n %d",chk,n);
			if (!isused[chk++]) { // don't count used digits
				--cd;
				if (!cd)
					break;
			}
			++v;
		}
		pd.push_back(v);
		isused[v]=true;
	}
	return *this;
}

bool perm::isident()
{
	S32 i,n=pd.size();
	for (i=0;i<n;++i)
		if (i!=pd[i])
			return false;
	return true;
}

void perm::printp()
{
	S32 i,n=pd.size();
	logger_disableindent();
//	logger("perm %3d      :",n);
	for (i=0;i<n;++i)
		logger(" %2d",pd[i]);
	logger("\n");
	logger_enableindent();
}

void perm::printc() // list of disjoint cycles
{
	logger_disableindent();
	if (isident()) {
		logger("e");
		logger_enableindent();
		logger("\n");
		return;
	}
	S32 i,n=pd.size();
	vector<S32> isused;
	isused.assign(n,false); // clear static dig used array and set to right size
	for (i=0;i<n;++i) {
		if (isused[i])
			continue;
		S32 sfv=i;
		S32 fv=pd[i];
		if (sfv==fv) // skip 1 cycles
			continue;
		logger("(");
		logger("%d",sfv);
		isused[sfv]=true;
		while(sfv!=fv) {
			isused[fv]=true;
			logger(" %d",fv);
			fv=pd[fv];
		}
		logger(")  ");
	}
	logger("\n");
	logger_enableindent();
}

/////////// group ////////////
group::group(S32 soa,bool full)
{
	if (soa > 8)
		errorexit("creating group > 8, too much");
	perm p(soa);
	//S32 i;
	while(true) {
//	for (i=0;i<100000;++i) { // watchdog, good for Sn <= S8
//	for (i=0;i<0x7fffffff;++i) { // watchdog, good for Sn <=S12
		perms.push_back(p);
		if (!full)
			return;
		++p;
		if (p.isident()) // early out
			break;
	}
}

// create a group from a vector of perms, and close it
group::group(const vector<perm>& permsa)
{
	if (permsa.empty())
		errorexit("creating group with 0 size perms");
	S32 soa = permsa[0].size();
	perm p(soa); // add the identity
	addperm(p);
	//perms.assign(1,perm(4));
	S32 i;
	for (i=0;i<(int)permsa.size();++i) {
		if (permsa[i].size() != soa)
			errorexit("creating group with mismatched perm size");
	}
	perms.insert(perms.end(),permsa.begin(),permsa.end());
	close();
}

// returns index
S32 group::addperm(const perm& p) // add permutation if not in list already
{
	S32 i,n=perms.size();
	for (i=0;i<n;++i)
		if (perms[i]==p)
			return i;
	perms.push_back(p);
	return n;
}

void group::close() // multiply until closure is satisfied
{
	S32 oldn=0;
	S32 i,j;
	S32 n=perms.size();
	while(oldn!=n) {
		for (j=0;j<n;++j) {
			for (i=0;i<n;++i) {
				perm p=timesp(j,i);
				addperm(p);
			}
		}
		oldn=n;
		n=perms.size();
	}
}

// return perm index
// if not in list, adds it
int group::times(S32 lhs,S32 rhs) // right to left
{
	perm p=perms[lhs]*perms[rhs];
	S32 i,n=perms.size();
	for (i=0;i<n;++i)
		if (perms[i]==p)
			return i;
	addperm(p);
	//errorexit("can't find perm in times");
	//return -1;
	return n;
}

// returns product perm
// if not in group, adds it
perm group::timesp(S32 lhs,S32 rhs) // right to left
{
	S32 i,n=perms.size();
	perm p=perms[lhs]*perms[rhs];
	for (i=0;i<n;++i)
		if (perms[i]==p)
			return p;
	addperm(p);
	return p;
}

void group::printc(const C8* title) // print a nice Cayley table
{
	S32 n=perms.size();
	if (!title)
		title="";
	logger("Cayley table '%s' order %d\n",title,n);
	S32 i,j;
	for (j=0;j<n;++j) {
		logger_disableindent();
		for (i=0;i<n;++i) {
			logger(" %3d",times(j,i));
		}
		logger("\n");
		logger_enableindent();
	}
}

cgroup::cgroup(S32 na) : order(na) { // just initialize e*a and a*e
	perf_start(TEST5);
	if (na>maxord)
		errorexit("max group order is %d, was given %d",maxord,na);
#if 0
	vector<S32> iv(na,-1);
	ct.assign(na,iv);
	S32 i;
	S32 n=ct.size();
	for (i=0;i<n;++i) {
		write(0,i,i);
		write(i,0,i);
	}
#else
	S32 n2=order*order;
//	dct=new S32[n2];
	S32 i;
	// set all to uninitialized
	for (i=0;i<n2;++i)
		dct[i]=-1;
	// set first row and column to identity
	for (i=0;i<order;++i) {
		dct[i]=i;
		dct[order*i]=i;
	}
#endif
	perf_end(TEST5);
}

cgroup::cgroup(const cgroup& oga) : order(oga.order)
{
	perf_start(TEST4);
#if 0
	ct=oga.ct;
#else
	S32 n2=order*order;
//	dct=new S32[n2];
	memcpy(dct,oga.dct,sizeof(S32)*n2);
#endif
	perf_end(TEST4);
}
/*inline cgroup::~cgroup()
{
//	delete[] dct;
}
*/
inline S32 cgroup::times(S32 lhs,S32 rhs) const
{
//perf_start(GMULT);
#if 0 // safety check
	if (lhs==-1)
		errorexit("bad times %d %d",lhs,rhs);
	if (rhs==-1)
		errorexit("bad times %d %d",lhs,rhs);
#endif
#if 0
	if (lhs==-1)
		return -1;
	if (rhs==-1)
		return -1;
#endif
//	S32 ret=ct[lhs][rhs];
	S32 ret=dct[order*lhs+rhs];
//	perf_end(GMULT);
	return ret;
}

inline void cgroup::fastwrite(S32 lhs,S32 rhs,S32 vala)
{
	dct[lhs*order+rhs]=vala;
}
bool cgroup::write(S32 lhs,S32 rhs,S32 vala)
{
	perf_start(GCALC);
// check dups in row/col, cancellation laws
	S32 k;
	for (k=0;k<order;++k) {
		S32 v=times(lhs,k);
		if (v==-1)
			continue;
		if (v==vala) {
			perf_end(GCALC);
			return false; // found a dup in row
		}
	}
	for (k=0;k<order;++k) {
		S32 v=times(k,rhs);
		if (v==-1)
			continue;
		if (v==vala) {
			perf_end(GCALC);
			return false; // found a dup in column
		}
	}
	fastwrite(lhs,rhs,vala);
 // associativity check, tricky , just check new entry, temporarily add and if fails, remove and return false
// let's say new entry is p*q=r
// generate all new left assoc, check against existing right assoc (ab)c vs. a(bc)
// first check (a*b)*c against a*(b*c) where a*b=lhs and c=rhs, harder
	S32 a,b,c;
	for (a=0;a<order;++a) {
		for (b=0;b<order;++b) {
			S32 ab=times(a,b);
			if (ab==lhs) {
				c=rhs;
				S32 ab_c=times(ab,c);
				if (ab_c!=-1) {
					S32 bc=times(b,c);
					if (bc!=-1) {
						S32 a_bc=times(a,bc);
						if (a_bc!=-1) {
							if (a_bc!=ab_c) {
								fastwrite(lhs,rhs,-1);
								perf_end(GCALC);
								return false;
							}
						}
					}
				}
			}
		}
	}
// second check (a*b)*c against a*(b*c) where a=lhs and b=rhs, easier
	a=lhs;
	b=rhs;
	S32 ab=vala; // or times(a,b);
	for (c=0;c<order;++c) {
		S32 ab_c=times(ab,c);
		if (ab_c!=-1) {
			S32 bc=times(b,c);
			if (bc!=-1) {
				S32 a_bc=times(a,bc);
				if (a_bc!=-1) {
					if (a_bc!=ab_c) {
						fastwrite(lhs,rhs,-1);
						perf_end(GCALC);
						return false;
					}
				}
			}
		}
	}
	perf_end(GCALC);
	return true;
}

bool cgroup::hasm1(S32* ia,S32* ja) const // return true if -1 found
{
	perf_start(TEST1);
	S32 i,j;
	for (j=0;j<order;++j) {
		for (i=0;i<order;++i) {
			S32 val=times(j,i);
			if (val==-1) {
				if (ia)
					*ia=i;
				if (ja)
					*ja=j;
				perf_end(TEST1);
				return true;
			}
		}
	}
	perf_end(TEST1);
	return false;
}
bool cgroup::operator==(const cgroup& rhs) const
{
	perf_start(GEQ);
#if 0
	bool ret = ct==rhs.ct; // maybe alittle slow
#else
	bool ret = memcmp(dct,rhs.dct,sizeof(S32)*order*order)==0;
#endif
	perf_end(GEQ);
	return ret;
}
vector<cgroup> cgroup::genidentskel() const // return set of identity skeletons
{
	vector<cgroup> skels;
	if (order == 1) {
		cgroup cg(1);
		skels.push_back(cg);
		return skels;
	}
	S32 maxflips;
	bool isodd=order&1;
	if (isodd)
		maxflips=1;
	else
		maxflips=(order-1)/2;
	S32 f;
	for (f=maxflips;f>=0;--f) {
//	for (f=0;f<=maxflips;++f) {
		cgroup cg(order);
		if (!isodd) // even, add another identity in diagonal
			cg.write(1,1,0);
		S32 i;
		for (i=0;i<maxflips;++i) {
			if (i>=f) { // no flip
				cg.write(order-2-2*i,order-2-2*i,0);
				cg.write(order-2-2*i+1,order-2-2*i+1,0);
			} else { // flip
				cg.write(order-2-2*i+1,order-2-2*i,0);
				cg.write(order-2-2*i,order-2-2*i+1,0);
			}
		}
		skels.push_back(cg);
	}
	return skels;
}
bool cgroup::isiso(const cgroup& rhs,S32 a,S32 b) const
{
	perf_start(GHASH);
#if 1
	cgroup tg=rhs;
	S32 i,j;
	for (j=0;j<order;++j)
		exch(tg.dct[j*order+a],tg.dct[j*order+b]);
	for (i=0;i<order;++i)
		exch(tg.dct[a*order+i],tg.dct[b*order+i]);
	for (j=0;j<order;++j) {
		for (i=0;i<order;++i) {
			S32 v=tg.times(j,i);
			if (v==a)
				v=b;
			else if (v==b)
				v=a;
			tg.fastwrite(j,i,v);
		}
	}
	perf_end(GHASH);
	return (*this)==tg;
#else
	perf_end(GHASH);
	return false;
#endif
}
vector<cgroup> cgroup::guessnext() const // return set of guesses for first -1 found
{
	perf_start(GFILTER);
	vector<cgroup> gss; // guess groups
	S32 ia,ja;
	bool fnd=hasm1(&ia,&ja);
	if (!fnd) { // not a single -1 found, return empty set, no guesses to do
		perf_end(GFILTER);
		return gss;
	}
	vector<S32> gssn; // guess numbers
	S32 a;
	perf_start(TEST2);
	for (a=0;a<order;++a) { // start guessing
		cgroup cg = *this;
		if (cg.write(ja,ia,a)) { // 'write' checks cancellation and associativity
			S32 k; // we've got a guess, check other guesses for isomorphism
			S32 ngss=gssn.size();
			for (k=0;k<ngss;++k) {
				S32 b=gssn[k];
				if (gss[k].isiso(cg,a,b))
					break;
			}
		perf_start(TEST3);
			if (k==ngss) {
				gss.push_back(cg);
				gssn.push_back(a);
			}
		perf_end(TEST3);
		}
	}
	perf_end(TEST2);
// please check for isomorphisms here, no instead let's check above
	perf_end(GFILTER);
	return gss;
}
void cgroup::printc(const C8* title) const // print a nice Cayley table
{
	perf_start(GPRINT);
	bool showraw = true; // don't do guessnext and hasm1
	S32 i,j;
	if (!title)
		title="";
	vector<cgroup> gv=guessnext();
	S32 gn=gv.size();
	if (gn && !showraw) { // have a guess list, print this instead
// prune this list down by using incomplete isomorphism check
		for (i=0;i<gn;++i) {
			gv[i].printc(title);
		}
	} else if (!hasm1(&i,&j) || showraw) { // print if no guesses and no uninitialized vars
		logger("Cayley table '%s' order %d\n",title,order);
		for (j=0;j<order;++j) {
			logger_disableindent();
			for (i=0;i<order;++i) {
				logger(" %3d",times(j,i));
			}
			logger("\n");
			logger_enableindent();
		}
	}
	perf_end(GPRINT);
}

} // end namespace smallgroups
