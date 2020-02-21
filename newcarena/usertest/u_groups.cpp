#include <m_eng.h>
#include <cstdarg>

namespace u_groups {

#define CYC // cycle based constructor, else raw constructor
#define CYCPRINT // cycle based print, else raw print

class perm {
	vector<U32> p;
	mutable vector<bool> visited;
	U32 nele;
public:
	static const U32 END=-1;
	static const U32 SPLIT=-2;
private:
	void reset()
	{
		p.assign(nele,0);
		for (U32 i=0;i<nele;++i) {
			p[i]=i;
		}
	}
public:
#ifdef CYC
public:
	perm(U32 ne); // flag perm(ne) as an error
public:
	perm(U32 ne,...) : nele(ne) // cycle based, convert to raw
	{
		reset();
		va_list ap;
		va_start(ap, ne); 
		U32 i,j;
		vector<U32> c;
		vector<U32> ct;
		vector<vector<U32> > cc;
		while(true) {
			S32 v = va_arg(ap,S32);
			if (v==END || v==SPLIT) { // push cycle onto array of cycles and done
				if (c.size()>=2)
					cc.push_back(c);
				c.clear();
				if (v==END)
					break;
			} else {
				if ((U32)v>=nele)
					errorexit("arg %d too big in perm",v);
				c.push_back(v); // cycle vals
				ct.push_back(v); // total vals
			}
//			logger("%d ", v);
//			if (i>=nele)
//				errorexit("too many args in perm");
//			p[i++] = v;
		}
		va_end(ap);
		for (i=0;i<ct.size();++i) { // cycles must all have unique values
			for (j=i+1;j<ct.size();++j) {
				if (ct[i]==ct[j])
					errorexit("perm: constructor, dup value %d at %d and %d",ct[i],i,j);
			}
		}
		// run through all cycles and build raw
		for (j=0;j<cc.size();++j) {
			vector<U32>& lc = cc[j];
			for (i=0;i<lc.size();++i) {
				U32 cur = lc[i];
				U32 next;
				U32 ip1 = i+1;
				if (ip1 == lc.size())
					next = lc[0];
				else
					next = lc[ip1];
				p[cur] = next;
			}
		}

//		logger("\n");
	}
#else
	perm(U32 ne,...) : nele(ne) // raw
	{
		reset();
		va_list ap;
		va_start(ap, ne); 
		U32 i = 0;
		while(true) {
			S32 v = va_arg(ap,S32);
			if (v==END)
				break;
//			logger("%d ", v);
			if (i>=nele)
				errorexit("too many args in perm");
			if ((U32)v>=nele)
				errorexit("arg %d too big in perm",v);
			p[i++] = v;
		}
		va_end(ap);
//		logger("\n");
	}
#endif
	perm operator*(const perm& rhs) const
	{
		if (nele != rhs.nele)
			errorexit("perm: * different nele");
		perm prod(nele,END);
		U32 i;
		for (i=0;i<nele;++i)
			prod.p[i]= p[rhs.p[i]];
		return prod;
	}
	perm comm(const perm& rhs) const
	{
		if (nele != rhs.nele)
			errorexit("perm: comm different nele");
//		perm ret(nele,END);
		return inverse()*rhs.inverse()*(*this)*rhs;
	}
	perm conj(const perm& rhs) const
	{
		if (nele != rhs.nele)
			errorexit("perm: conj different nele");
//		perm ret(nele,END);
		return (*this)*rhs*inverse();
	}
	bool operator==(const perm& rhs) const
	{
		if (nele != rhs.nele)
			errorexit("perm: == different nele");
		return p == rhs.p;
	}
#ifdef CYCPRINT
	void print(int pn=END) const // cycle based
	{
		U32 j;
		visited.assign(nele,false);
		logger_disableindent();
		if (pn == END)
			logger("perm: ");
		else
			logger("perm %3d : ",pn);
		bool isi = true;
		for (j=0;j<nele;++j) {
			if (p[j]!=j && !visited[j]) {
				isi = false;
				logger("(%d,",j);
				int c = p[j];
				while(true) {
					visited[c] = true;
					logger("%d",c);
					c=p[c];
					if (c==j) {
						logger(") ");
						break;
					} else {
						logger(",");
					}
				}
			}
//			logger("%d ",p[j]);
		}
		if (isi)
			logger("I");
		logger("\n");
		logger_enableindent();
	}
#else
	void print(int pn=END) const // raw print
	{
		U32 i;
		logger_disableindent();
		if (pn == END)
			logger("perm: ");
		else
			logger("perm %3d: ",pn);
		for (i=0;i<nele;++i)
			logger("%d ",p[i]);
		logger_enableindent();
		logger("\n");
	}
#endif
	int getnele() const
	{
		return nele;
	}
	bool operator<(const perm& rhs) const
	{
		U32 i;
		for (i=0;i<rhs.nele;++i) {
			S32 v = p[i] - rhs.p[i];
			if (v<0)
				return true;
			if (v>0)
				return false;
		}
		return false;
	}
	perm inverse() const
	{
		perm ret(nele,END);
		U32 i;
		for (i=0;i<nele;++i)
			ret.p[p[i]]=i;
		return ret;
	}
};

U32 fact(U32 n)
{
	U32 i;
	U32 p = 1;
	for (i=2;i<=n;++i)
		p *= i;
	return p;
}

class group { // collection of 'perm's, calc to make a real group
	vector<perm> glist;
	U32 npermele;
public:
	void add(const perm& p)
	{
		if (glist.empty())
			npermele = p.getnele();
		else if (p.getnele() != npermele)
			errorexit("group: can't add perm diff nele");
		U32 i;
		for (i=0;i<glist.size();++i)
			if (glist[i] == p)
				return;
		glist.push_back(p);
	}
	bool ismember(const perm& p)
	{
		if (glist.empty())
			return false;
		else if (p.getnele() != npermele)
			errorexit("group: can't ismember perm diff nele");
		U32 i;
		for (i=0;i<glist.size();++i)
			if (glist[i] == p)
				return true;
		return false;
	}
	void calc() // 'join' all perms in the list to make a real group
	{
		if (glist.empty())
			return;
		U32 oldsize;
		U32 i,j;
		U32 st = 0;
		U32 maxsize = fact(npermele);
		while(true) {
			oldsize = glist.size();
			if (oldsize == maxsize)
				break;
			for (j=0;j<oldsize;++j) {
				for (i=st;i<oldsize;++i) {
					perm pp = glist[i]*glist[j];
					add(pp);
				}
			}
			for (j=st;j<oldsize;++j) {
				for (i=0;i<st;++i) {
					perm pp = glist[i]*glist[j];
					add(pp);
				}
			}
			if (oldsize == glist.size())
				break;
			st = oldsize;
		}
		sort(glist.begin(),glist.end());
	}
	void print() const
	{
		U32 i;
		logger("Group: subgroup of S%d\n",npermele);
		for (i = 0 ; i < glist.size() ; ++i) {
			glist[i].print(i);
//			logger("inv ");
//			glist[i].inverse().print(i);
		}
		logger("\n");
	}
	bool operator<(const group& rhs) const
	{
		return glist[0] < rhs.glist[0];
	}
	bool operator==(const group& rhs) const
	{
		if (npermele != rhs.npermele)
			errorexit("group: == different npermele");
		return glist == rhs.glist;
	}
	group commutator() const
	{
		U32 i,j;
		group gcm;
		for (j=0;j<glist.size();++j) {
			for (i=0;i<glist.size();++i) {
//				logger("comm of\n");
//				glist[j].print();
//				glist[i].print();
//				glist[j].comm(glist[i]).print();
				perm pcom = glist[j].comm(glist[i]);
				gcm.add(pcom);
			}
//			logger("\n");
		}
		sort(gcm.glist.begin(),gcm.glist.end());
		return gcm;
	}
	// entire group
	void gen(U32 pe)
	{
		U32 i;
		for (i=1;i<pe;++i) {
			perm p(pe,0,i,perm::END);
			add(p);
		}
		calc();
	}
	friend class cosets;
};
	
class cosets {
	vector<group> left;
	vector<group> right;
public:
	cosets(const group& g,const group& h)
	{
		if (g.glist.empty())
			errorexit("cosets: g is empty");
		if (h.glist.empty())
			errorexit("cosets: h is empty");
		U32 j;
		left.push_back(h);
		right.push_back(h);
		for (j=0;j<g.glist.size();++j) {
			U32 k;
			perm gH0 = g.glist[j]*h.glist[0];
			for (k=0;k<left.size();++k) {
				if (left[k].ismember(gH0))
					break;
			}
			if (k==left.size()) { // new coset
				U32 i;
				group ng;
				for (i=0;i<h.glist.size();++i) {
					perm gH = g.glist[j]*h.glist[i];
					ng.add(gH);
				}
				sort(ng.glist.begin(),ng.glist.end());
				left.push_back(ng);
			}
			perm H0g = h.glist[0]*g.glist[j];
			for (k=0;k<right.size();++k) {
				if (right[k].ismember(H0g))
					break;
			}
			if (k==right.size()) { // new coset
				U32 i;
				group ng;
				for (i=0;i<h.glist.size();++i) {
					perm Hg = h.glist[i]*g.glist[j];
					ng.add(Hg);
				}
				sort(ng.glist.begin(),ng.glist.end());
				right.push_back(ng);
			}
		}
		sort(left.begin(),left.end());
		sort(right.begin(),right.end());
	}
	void print() const
	{
		U32 i;
		logger("cosets:\n");
		for (i=0;i<left.size();++i) {
			logger("left coset %3d\n",i);
			left[i].print();
		}
		for (i=0;i<right.size();++i) {
			logger("right coset %3d\n",i);
			right[i].print();
		}
		if (isnormal())
			logger("normal\n");
		else
			logger("not normal\n");
		logger("\n\n");
	}
	bool isnormal() const
	{
		return left == right;
	}
};

}

using namespace u_groups;

#define PE perm::END
#define PS perm::SPLIT
void testgroups()
{
	logger("start test groups\n");

// order S3
#ifdef CYC
	{
//		g.add(p1);
//		g.add(p2);
		group g;
		g.gen(3); // S3
		g.calc(); // closure
		g.print(); // S3

//		perm m;
		group h1;
		perm p1(3,0,1,PE);
		h1.add(p1);
		h1.calc();
		h1.print(); // Z2
		
		cosets c1(g,h1); // order 3
		c1.print();

		group h2;
		perm p2(3,0,1,2,PE); // cycle based format..  nele,  0 to 1,  1 to 1,  2 to 0,  done
		h2.add(p2);
		h2.calc();
		h2.print(); // Z3

		cosets c2(g,h2); // order 2
		c2.print();

		group cm = g.commutator();
		logger("commutator\n");
		cm.print();
	}
#endif

// order S4
#ifdef CYC
//	perm m;
//	perm p0(4,PE);
//	perm p1(4,0,1,2,3,PE); // cycle based format..  nele,  0 to 1,  1 to 1,  2 to 0,  done
//	perm p2(4,0,2,PE);
//	perm p3 = perm(4,0,1,PE);
#else
#endif
//	perm p4 = p2 * p1;
	group g;
//	g.add(p1);
//	g.add(p2);
//	g.add(p3);
//	g.add(p4);
	g.gen(4); // S4
//	g.print();
	g.calc(); // closure
	g.print(); // S4
	group cm4 = g.commutator();
	logger("commutator S4\n");
	cm4.print();
	group cm4b = cm4.commutator();
	logger("commutator A4\n");
	cm4b.print();

#ifdef CYC
//	perm s1(4,0,1,PE);
//	perm s2(4,2,3,PE);
//	perm p1(4,1,2,3,0,perm::END); // raw format..  nele,  0 to 1,  1 to 1,  2 to 0,  done
//	perm p2(4,2,1,0,3,PE);
//	perm p3 = perm(4,1,0,2,3,PE);
	group h;
	perm s1(4,0,1,PS,2,3,PE);
	h.add(s1);
	perm s2(4,0,2,PS,1,3,PE);
	h.add(s2);
	h.print();
	h.calc();
	h.print();

	cosets c(g,h);
	c.print();
#else
	perm s1(4,1,0,3,2,PE);
	perm s2(4,2,3,0,1,PE);
	group h;
	h.add(s1);
	h.add(s2);
	h.print();
	h.calc();
	h.print();
#endif
//#define BIGORDER
#ifdef BIGORDER
#ifdef CYC
//	perm f1(6,0,1,4,3,PE);
//	perm f2(6,1,2,5,4,PE);
//	perm f3(6,0,1,PE);
	group tf;
//	tf.add(f1);
//	tf.add(f2);
//	tf.add(f3);
	tf.gen(6);
	tf.print();
	tf.calc();
	tf.print();
#else
	perm f1(6,1,4,2,0,3,5,PE);
	perm f2(6,0,2,5,3,1,4,PE);
	perm f3(6,1,0,2,3,4,5,PE);
	group tf;
	tf.add(f1);
	tf.add(f2);
	tf.add(f3);
	tf.print();
	tf.calc();
	tf.print();
#endif
#endif
#define D8 // order 8
#ifdef CYC
	perm f1(4,0,1,3,2,PE);
	perm f2(4,0,3,PE);
	group d8;
	d8.add(f1);
	d8.add(f2);
	logger("d8\n");
	d8.print();
	d8.calc();
	d8.print();
#endif
	logger("done test groups\n");
}
