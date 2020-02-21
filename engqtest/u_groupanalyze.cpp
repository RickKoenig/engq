#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

namespace ga {

const U32 hashsize = 32768*64; // power of 2
const U32 maxgroupsize=7000000;	// stop big groups from overcalculating

const U32 nfacelet_corners = 8;
const C8* facelet_corners[nfacelet_corners] = {
	"flu","fur","fdl","frd",
	"bul","bru","bld","bdr"};
const U32 nfacelet_edges = 12;
const C8* facelet_edges[nfacelet_edges] = {
	"ul","ur","dl","dr",
	"fu","fd","fl","fr",
	"bu","bd","bl","br"};

void study64bit()
{
	S32 i,j;
	logger("in study64bit\n");
	S64 big = 1000000000000LL;
	logger("sizeof big is %d\n",sizeof(big));
	logger("value is %lld\n",big);
	LARGE_INTEGER a,b;
	QueryPerformanceFrequency(&b);
	logger("freq is %lld\n",b.QuadPart);
	for (i=0;i<100;++i) {
		for (j=0;j<1000000;++j)
			;
		QueryPerformanceCounter(&a);
		logger("value is %lld, millisec %lld\n",a.QuadPart,1000LL*a.QuadPart/b.QuadPart);
	}
}

class cycle
{
	vector<U32> c;
public:
	cycle(const S32* ca=0)
	{
		if (ca==0)
			return;
		U32 i=0;
		while(ca[i]!=-1)
			c.push_back(ca[i++]);
	}
	void addelement(S32 cv)
	{
		c.push_back(cv);
	}
	void print() const
	{
		U32 i;
		if (c.size()<=1)
			return;
		logger("( ",c.size());
		for (i=0;i<c.size();++i)
			logger("%d ",c[i]);
		logger(") ");
	}
	friend class permute;
};

U32 facelet2face(const C8* s,U32 fli)
{
	U32 i,r,k;
	if (strlen(s)==3) {
		for (k=0;k<nfacelet_corners;++k) {
			for (r=0;r<3;++r) {
				for (i=0;i<3;++i) {
					U32 m=(i+r)%3;
//					logger("checking s[%d]=%c against fc[%d][%d]=%c\n",
//						i,s[i],k,m,facelet_corners[k][m]);
					if (s[i]!=facelet_corners[k][m])
						break;
				}
				if (i==3)
					break;
			}
			if (r!=3) {
//				logger("ret %d\n",3*k + (r+fli)%3);
				return 3*k + (r+fli)%3;
			}
		}
		errorexit("fl2c: couldn't find corner '%s'",s);
	}
	if (strlen(s)==2) {
		for (k=0;k<nfacelet_edges;++k) {
			for (r=0;r<2;++r) {
				for (i=0;i<2;++i) {
					U32 m=(i+r)%2;
//					logger("checking s[%d]=%c against fc[%d][%d]=%c\n",
//						i,s[i],k,m,facelet_corners[k][m]);
					if (s[i]!=facelet_edges[k][m])
						break;
				}
				if (i==2)
					break;
			}
			if (r!=2) {
//				logger("ret %d\n",3*k + (r+fli)%3);
				return 24 + 2*k + (r+fli)%2;
			}
		}
		errorexit("fl2e: couldn't find edge '%s'",s);
	}
	errorexit("not a cube or edge",s);
	return 0;
}

string face2facelet(U32 fidx)
{
	string s;
	U32 i,j;
	if (fidx<24) {
		for (i=0;i<3;++i) {
			j=(i+fidx)%3;
			s+=facelet_corners[fidx/3][j];
		}
	} else if (fidx<48) {
		fidx-=24;
		for (i=0;i<2;++i) {
			j=(i+fidx)%2;
			s+=facelet_edges[fidx/2][j];
		}
	} else
		errorexit("bad fidx %u",fidx);
	return s;
}

class permute
{
	string name;
	U32 moves; // how many base moves it took to make this (0 ident, 1 base moves etc.)
	U32 hashval;
	void calchash();
	static string delimit; // printing separator
protected:
//	mutable vector<cycle> cs;
	vector<U8> next;
public:
	bool operator==(const permute& pr) const;
	permute operator*(const permute pr) const;
	permute(U32 nelea,string namea) : name(namea),moves(0)
	{
		
		U32 i;
		for (i=0;i<nelea;++i)
			next.push_back(i);
		calchash();
	}
	permute(U32 nelea,const S32* initcycle,string namea) : name(namea),moves(1)
	{
		U32 i;
		for (i=0;i<nelea;++i)
			next.push_back(i);
		if (initcycle) {
			i=0;
			while(initcycle[i]>=0) {
				U32 j=i+1;
				if (initcycle[j]==-1)
					j=0;
				next[initcycle[i]]=initcycle[j];
				++i;
			}
		}
		calchash();
	}
	permute(U32 nelea,const cycle& c,string namea) : name(namea),moves(1)
	{
		U32 i;
		for (i=0;i<nelea;++i)
			next.push_back(i);
		for (i=0;i<c.c.size();++i) {
			U32 j=i+1;
			if (j==c.c.size())
				j=0;
			next[c.c[i]]=c.c[j];
		}
		calchash();
	}
	permute(U32 nelea,U32 uc,U32 ue,const C8*const* ca,string namea) : name(namea),moves(1)
	{
		permute pt(nelea,"");
		U32 i,j;
		for (i=0;i<3;++i) {
			cycle cyc;
			for (j=0;j<uc;++j) {
				U32 cv = facelet2face(ca[j],i);
				cyc.addelement(cv);
			}
	//		cyc.print();
			permute p(nelea,cyc,"");
			pt = pt * p;
		}
		for (i=0;i<2;++i) {
			cycle cyc;
			for (j=0;j<ue;++j) {
				U32 cv = facelet2face(ca[j+uc],i);
				cyc.addelement(cv);
			}
	//		cyc.print();
			permute p(nelea,cyc,"");
			pt = pt * p;
		}
		next = pt.next;
		moves = 1;
		calchash();
	}
	static void setdelimit(string sa)
	{
		delimit=sa;
	}
	bool checkfilter(vector<U32> f) const // return true if SHOULD be filtered
	{
		vector<U32>::iterator i=f.begin();
		while(i!=f.end()) {
			if (*i != next[*i])
				return true;	// filter out, it changed.
			++i;
		}
		return false;
	}
	void print() const
	{
//		U32 i;
//		logger("nele %d, ",nele);
/*		for (i=0;i<nele;++i) {
			logger("%d ",next[i]);
		} */
		logger("M%-2d (%s)",moves,name.c_str());
	}
	void printn() const
	{
		U32 i;
		logger("(nele %d, ",next.size());
		for (i=0;i<next.size();++i) {
			logger("%d ",next[i]);
		} 
		logger(")");
	}
	vector<cycle> build_cs() const
	{
		vector<cycle> ret;
// use cur as 'marked'
		U32 i=0; // start at 0
		vector<U32> cur(next.size());
		fill(cur.begin(),cur.end(),0);
		while(i<next.size()) {
			if (cur[i]) { // marked
				++i;
				continue;
			}
			U32 j=i;
			cycle cy;
			while(1) {
				cur[j]=1; // mark
				cy.addelement(j);
				j=next[j]; // walk the cycle
				if (j==i)
					break;
				
			};
//			cy.print();
			if (cy.c.size()>1)
				ret.push_back(cy);
			++i;
		}
		return ret;
	}
	void printc() const
	{
		U32 i;
		vector<cycle> cs=build_cs();
		logger("C%d ",cs.size());
		for (i=0;i<cs.size();++i)
			cs[i].print();
	}
	void rubprintc() const
	{
		U32 i,j;
		vector<cycle> cs=build_cs();	// make basic cycles out of permutation
		vector<vector<U32> > sc; // super cycles
		vector<U32> twist;
		for (i=0;i<cs.size();++i) {
			if (cs[i].c[0]>=24) // it's an edge, skip
				break;
			for (j=0;j<twist.size();++j) {
				if (sc[j][0]/3 == cs[i].c[0]/3)
					break;	// already in the list
			}
			if (j==twist.size()) { // add new cycle to list
				vector<U32> vc;
				U32 tw=0;
				for (j=0;j<cs[i].c.size();++j) {
					if (j!=0 && cs[i].c[0]/3 == cs[i].c[j]/3) {
						tw = (3 + cs[i].c[j] - cs[i].c[0])%3;
						break;
					}
					vc.push_back(cs[i].c[j]);
				}
				sc.push_back(vc);
				twist.push_back(tw);
			}
		}
		for (i=0;i<cs.size();++i) {
			if (cs[i].c[0]<24) // it's a face, skip
				continue;
			for (j=0;j<twist.size();++j) {
				if (sc[j][0]/2 == cs[i].c[0]/2)
					break;	// already in the list
			}
			if (j==twist.size()) { // add new cycle to list
				vector<U32> vc;
				U32 tw=0;
				for (j=0;j<cs[i].c.size();++j) {
					if (j!=0 && cs[i].c[0]/2 == cs[i].c[j]/2) {
						tw = (2 + cs[i].c[j] - cs[i].c[0])%2;
						break;
					}
					vc.push_back(cs[i].c[j]);
				}
				sc.push_back(vc);
				twist.push_back(tw);
			}
		}
//		logger("RUBC%d ",cs.size());
		logger("RUBC%d ",sc.size());
		for (i=0;i<sc.size();++i) {
			if (twist[i]==0 && sc[i].size()<=1 || sc[i].size()<1)
				break;
			logger("( ",sc[i].size());
			if (sc[i][0]<24) {
				for (j=0;j<sc[i].size();++j) {
//					logger("%2d ",sc[i][j]);
					logger("%2s ",face2facelet(sc[i][j]).c_str());
				}
				logger(")%d/3 ",twist[i]);
			} else {
				for (j=0;j<sc[i].size();++j) {
//					logger("%2d ",sc[i][j]);
					logger("%2s ",face2facelet(sc[i][j]).c_str());
				}
				logger(")%d/2 ",twist[i]);
			}
		}
	}
	void setname(string namea)
	{
		name = namea;
		moves = 1;
	}
	friend class group;
	friend class rubpermute;
};
string permute::delimit;

permute permute::operator*(permute pr) const
{
	perf_start(GMULT);
	permute ret(pr);
	U32 i;
	for (i=0;i<next.size();++i) {
		ret.next[i]=pr.next[next[i]];
	}
	ret.moves = moves + pr.moves;
	if (!name.size())
		ret.name = pr.name;
	else if (!pr.name.size())
		ret.name = name;
	else
		ret.name = name + delimit + pr.name;
	ret.calchash();
	perf_end(GMULT);
	return ret;
}
bool permute::operator==(const permute& pr) const
{
	bool ret;
	perf_start(GEQ);
	ret = hashval==pr.hashval && next==pr.next;
	perf_end(GEQ);
	return ret;
}
void permute::calchash()
{
	if (next.size()<8)
		hashval=0;
	else if (next.size()<16) {
		hashval =	next[0]*12533+
					next[1]*22533+
					next[2]*(49*48*47*46*45)+
					next[3]*(49*48*47*46)+
					next[4]*(49*48*47)+
					next[5]*(49*48)+
					next[6]*49+
					next[7];
	} else {
		U32 i;
		hashval=0;
		for (i=0;i<next.size();++i) {
			hashval*=53;
			hashval+=next[i];
		}
	}
	hashval &= (hashsize-1);
}

/*
class rubpermute : public permute {
public:
//	rubpermute(U32 nelea,string namea) : permute(nelea,namea) {}
	rubpermute(U32 nelea,const C8* const* ca,string namea);
};
*/
class group
{
	U32 ele;	// 2 3  4   5 etc.
	vector<vector<U32> > gh;//[hashsize];
	vector<permute> gi;
	bool bailer;
	vector<U32> filter;
public:
	U32 nmoves;
	U32 naddpermutes;
	U32 naddfilt;
	group(U32 elea) : ele(elea),gh(hashsize),bailer(false),nmoves(0),naddpermutes(0),naddfilt(0)
	{
		addpermute(permute(ele,"I")); // identity
	}
	U32 size()
	{
		return gi.size();
	}
	bool addpermute(const permute& perma)//,U32 movesa,U32 lefta,U32 righta)
	{
		perf_start(GADD);
		++naddpermutes;
		if (gi.size()>=maxgroupsize) {
			logger("max permutes %d hit...\n",maxgroupsize);
			perf_end(GADD);
			return false;
		}
		if (naddpermutes%100000==0) { // abort watchdog
			winproc();
			if (KEY=='a') {
				perf_end(GADD);
				return false;
			} 
			if (wininfo.closerequested) {
				perf_end(GADD);
				return false;
			}
		}
		U32 i,h=perma.hashval;
		perf_start(GHASH);
		vector<U32>& ghh=gh[h];
		U32 hs=ghh.size();
		const vector<U32>& hb=ghh;
		for (i=0;i<hs;++i) {
			if (perma==gi[hb[i]]) {
				perf_end(GHASH);
				perf_end(GADD);
				return true;
			}
		}
		perf_end(GHASH);
		ghh.push_back(gi.size());
		gi.push_back(perma);
		perf_start(GFILTER);
		if (!perma.checkfilter(filter))
			++naddfilt;
		perf_end(GFILTER);
		perf_end(GADD);
		return true;
	}
	void addpermute3(const permute& p)
	{
		string na = p.name;
		string nam = na + "  ";
		permute F(p);
		permute Fn(p);
		Fn.setname(na + "  ");
		addpermute(Fn);
		Fn = Fn * F;
		Fn.setname(na + "2 ");
		addpermute(Fn);
		Fn = Fn * F;
		Fn.setname(na + "-1");
		addpermute(Fn);
	}
// add p1-1 * p2, p1*p1*p2*p2 and p1 * p2-1 (sorta simulate a 'slice')
	void addpermute2(const permute& p1,const permute& p2)
	{
		string na;
		permute pr = p1 * p2 * p2 * p2;
		pr.setname(p1.name + p2.name + "-1");
		addpermute(pr);
		pr = p1 * p1 * p2 * p2;
		pr.setname(p1.name + "2" + p2.name + "2");
		addpermute(pr);
		pr = p1 * p1 * p1 * p2;
		pr.setname(p1.name + "-1" + p2.name);
		addpermute(pr);
	}
	void calcgroup()
	{
		if (gi.size()<=1)
			return;
		U32 basestart=1;
		U32 baseend=gi.size();
		U32 oldsize=basestart;
		U32 newsize=baseend;
		U32 nm=2;
		U32 i,j;
		permute::setdelimit(",");
		while(oldsize!=newsize) {
			perf_start(GCALC);
			for (j=oldsize;j<newsize;++j) {
				for (i=basestart;i<baseend;++i) {
					permute pn(ele,"");
					pn = gi[j]*gi[i];
					if (!addpermute(pn)) {
						pn.setdelimit("");
						perf_end(GCALC);
						return;
					}
					nmoves=pn.moves;
				}
			}
			oldsize=newsize;
			newsize=gi.size();
			++nm;
			perf_end(GCALC);
		}
		permute::setdelimit("");
	}
	void setfilter(S32* f)
	{
		if (!f)
			return;
		while(*f!=-1) {
			filter.push_back(*f);
			++f;
		}
	}
	void setrubfilter(C8** f)
	{
		if (!f)
			return;
		while(*f) {
			U32 len=strlen(*f);
			U32 i;
			for (i=0;i<len;++i) {
				U32 v=facelet2face(*f,i);
				filter.push_back(v);
			}
			++f;
		};
//		filter = f;
	}
	void print() const
	{
		U32 i;
		logger("group, permutes %d, filtered\n",gi.size(),naddfilt);
		logger_indent();
		for (i=0;i<gi.size();++i) {
			if (!gi[i].checkfilter(filter)) {
				logger_disableindent();
				logger("hash %4d, ",gi[i].hashval);
				logger("permute %4d, ",i);
				gi[i].print();
				logger("   ");
#if 0
				gi[i].printn();
				logger("   ");
#endif
				gi[i].printc();
				logger("\n");
				logger_enableindent();
			}
		}
		logger_unindent();
	}
	void rubprint() const
	{
		U32 i;
		logger("group, permutes %d, filtered %d\n",gi.size(),naddfilt);
		logger_indent();
		for (i=0;i<gi.size();++i) {
			if (!gi[i].checkfilter(filter)) {
				logger_disableindent();
				gi[i].print();
				logger("\n");
				logger_enableindent();
				logger_indent();
				logger_disableindent();
				gi[i].rubprintc();
				logger("\n");
				logger_enableindent();
				logger_unindent();
			}
		}
		logger_unindent();
	}
	void hashprint() const
	{
		U32 mh=0,th=0,zh=0;
		U32 i;
		for (i=0;i<hashsize;++i) {
			const vector<U32> &ghi=gh[i];
			if (ghi.size()>mh)
				mh=ghi.size();
			if (ghi.empty())
				++zh;
			th+=ghi.size();
		}
		logger("maxhash %u, tot %u, hashsize %u, ratio (1 best) %f, num zeros %u, num nonzeros %u\n",
			mh,th,hashsize,(float)mh/th*hashsize,zh,hashsize-zh);
	}
};

group *g;

// called indirectly when winproc gets called
}
using namespace ga;

void groupanalyzedraw()
{
	if (g)
		outtextxyf32(B32,8,8,C32WHITE,"npermutes %d, %d %d, filt %d",g->size(),g->naddpermutes,g->nmoves,g->naddfilt);
}

void groupanalyze()
{
	logger("in groupanalyze\n");
// test realloc
	U8* data=(U8*)memalloc(10);
	data=(U8*)memrealloc(data,25);
	memfree(data);
#if 0
	study64bit();
#endif
#if 0 // do a real rubik's cube
	const U32 ele=48;
// create the group, it automatically gets the identity
	g = new group(ele);
#if 1 // turn filter on/off
#if 0
// only solved can pass
	C8* filter[]={"flu","fur","fdl","frd","bul","bru","bld","bdr",
		"ul","ur","dl","dr","fu","fd","fl","fr","bu","bd","bl","br",0};
#else
// only if 2 bottom tiers solved
	C8* filter[]={"fdl","frd","bld","bdr",
		"dl","dr","fd","fl","fr","bd","bl","br",0};
#endif
	g->setrubfilter(filter);
#endif
	static C8* cylF[]={"flu","fur","frd","fdl","fu","fr","fd","fl"};
	static C8* cylR[]={"rfu","rub","rbd","rdf","ru","rb","rd","rf"};
	static C8* cylU[]={"ufl","ulb","ubr","urf","uf","ul","ub","ur"};
	static C8* cylB[]={"bru","bul","bld","bdr","bu","bl","bd","br"};
	static C8* cylL[]={"lbu","luf","lfd","ldb","lu","lf","ld","lb"};
	static C8* cylD[]={"drb","dbl","dlf","dfr","df","dr","db","dl"};
	static C8* cylFc[]={"flu","fur","frd","fdl"};
	static C8* cylRc[]={"rfu","rub","rbd","rdf"};
	static C8* cylUc[]={"ufl","ulb","ubr","urf"};
	static C8* cylBc[]={"bru","bul","bld","bdr"};
	static C8* cylLc[]={"lbu","luf","lfd","ldb"};
	static C8* cylDc[]={"drb","dbl","dlf","dfr"};
	static C8* cylFe[]={"fu","fr","fd","fl"};
	static C8* cylRe[]={"ru","rb","rd","rf"};
	static C8* cylUe[]={"uf","ul","ub","ur"};
	static C8* cylBe[]={"bu","bl","bd","br"};
	static C8* cylLe[]={"lu","lf","ld","lb"};
	static C8* cylDe[]={"df","dr","db","dl"};
// create basic moves
	permute F(ele,4,4,cylF,"F");
	permute R(ele,4,4,cylR,"R");
	permute U(ele,4,4,cylU,"U");
	permute B(ele,4,4,cylB,"B");
	permute L(ele,4,4,cylL,"L");
	permute D(ele,4,4,cylD,"D");
// corners only
	permute Fc(ele,4,0,cylFc,"Fc");
	permute Rc(ele,4,0,cylRc,"Rc");
	permute Uc(ele,4,0,cylUc,"Uc");
	permute Bc(ele,4,0,cylBc,"Bc");
	permute Lc(ele,4,0,cylLc,"Lc");
	permute Dc(ele,4,0,cylDc,"Dc");
// edges only
	permute Fe(ele,0,4,cylFe,"Fe");
	permute Re(ele,0,4,cylRe,"Re");
	permute Ue(ele,0,4,cylUe,"Ue");
	permute Be(ele,0,4,cylBe,"Be");
	permute Le(ele,0,4,cylLe,"Le");
	permute De(ele,0,4,cylDe,"De");
// create 180 moves
	permute F2=F*F;
	F2.setname("F2");
	permute R2=R*R;
	R2.setname("R2");
	permute U2=U*U;
	U2.setname("U2");
	permute B2=B*B;
	B2.setname("B2");
	permute L2=L*L;
	L2.setname("L2");
	permute D2=D*D;
	D2.setname("D2");
// create double opposite 180 moves (simulate middle moves 180)
	permute F2B2=F2*B2;
	F2B2.setname("F2B2");
	permute U2D2=U2*D2;
	U2D2.setname("U2D2");
	permute R2L2=R2*L2;
	R2L2.setname("R2L2");
// create FR move
	permute FR=F*R;
// create furball move
	permute FRBL=F*R*B*L;
#if 1 // add basic moves to group (this includes inverse and 180)
	g->addpermute3(F);
	g->addpermute3(R);
//	g->addpermute3(U);
// 	g->addpermute3(B);
//	g->addpermute3(L);
//	g->addpermute3(D);
#endif
#if 0 // just a couple...
	g->addpermute3(Fc);
	g->addpermute3(Rc);
//	g->addpermute3(Lc);
#endif
#if 0 // add FR to group
	g->addpermute(FR);
#endif
#if 0 // add furball to group
	g->addpermute(FRBL);
#endif
#if 0 // add double opposite 180 moves (simulate middle moves 180)
	g->addpermute(F2B2);
	g->addpermute(U2D2);
	g->addpermute(R2L2);
#endif
#if 0 // add double 90 opposites (simulate middle moves 90)
	g->addpermute2(F,B);
	g->addpermute2(R,L);
	g->addpermute2(U,D);
#endif
#if 0 // add 'FRU' to group
	g->addpermute(F*R*U);
#endif
#if 0 // right slice, and up
	g->addpermute2(R,L);
	g->addpermute3(U);
#endif
	g->calcgroup();	// careful here
	perf_start(GPRINT);
	g->rubprint();
	g->hashprint();
	perf_end(GPRINT);
#endif
// calculate the entire 'corner moves only' group order 40320 or 8!
#if 0 // play with just the corners (order 8)
	const U32 ele=8;
	g = new group(ele);
//	S32 filter[]={0,1,2,3,4,5,6,7,-1}; // keep the same
	S32 filter[]={4,5,6,7,-1}; // keep the same
//	S32 filter[]={1,2,3,4,5,6,-1}; // keep the same
	g->setfilter(filter);
	static S32 cylF[]={0,1,3,2,-1};
	static S32 cylB[]={4,6,7,5,-1};
	static S32 cylU[]={0,4,5,1,-1};
	static S32 cylD[]={2,3,7,6,-1};
	static S32 cylR[]={1,5,7,3,-1};
	static S32 cylL[]={0,2,6,4,-1};
	permute F(ele,cylF,"F");
	permute B(ele,cylB,"B");
	permute U(ele,cylU,"U");
	permute D(ele,cylD,"D");
	permute R(ele,cylR,"R");
	permute L(ele,cylL,"L");
 	g->addpermute3(F);
 	g->addpermute3(B);
 	g->addpermute3(U);
 	g->addpermute3(D);
	g->addpermute3(R);
	g->addpermute3(L); 
	g->calcgroup();	// careful here
	g->print();
#endif
// simple stuff
#if 1 // play with just the corners (order 8)
//	S32 filter[]={0,1,2,3,4,5,6,7,-1}; // keep the same
//	S32 filter[]={4,5,6,7,-1}; // keep the same
//	S32 filter[]={1,2,3,4,5,6,-1}; // keep the same
//	g->setfilter(filter);
#if 0
	const U32 ele=4;
	static S32 cylL[]={0,1,2,-1};
	static S32 cylR[]={1,3,2,-1};
#endif
#if 0
	const U32 ele=5;
	static S32 cylL[]={0,1,2,-1};
	static S32 cylR[]={1,3,4,-1};
#endif
#if 0
	const U32 ele=6;
	static S32 cylL[]={0,1,2,3,-1};
	static S32 cylR[]={1,4,5,2,-1};

	permute L(ele,cylL,"L");
	permute R(ele,cylR,"R");
	g = new group(ele);
 	g->addpermute3(L);
 	g->addpermute3(R);
	g->calcgroup();	// careful here
	g->print();
#endif
	const U32 ele=4;
	static S32 cyl1[]={0,1,-1};
	static S32 cyl2[]={0,2,-1};
	static S32 cyl3[]={0,3,-1};

	g = new group(ele);
	g->addpermute(permute(ele,cyl1,"A"));
	g->addpermute(permute(ele,cyl2,"B"));
	g->addpermute(permute(ele,cyl3,"C"));
	g->calcgroup();	// careful here
	g->print();
#endif
	delete g;
	g=0;
}
