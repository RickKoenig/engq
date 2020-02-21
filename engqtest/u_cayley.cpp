// try to find cayley's that are non associative...
// but still have other grouplike properties
#include <m_eng.h>

namespace name_cayley
{

//#define DOCAYLEYS
#define DOCAYLEY
//#define TESTLOK

#ifdef DOCAYLEY
#define DOGET
#endif
#ifdef TESTLOK
#define LORD 4
#define DOGET
#endif
#ifdef DOGET
U32 getrch(U32 hy,U32 hx,U32 ord,U32 j)
{
	if (hy>j)
		return ord;
	else if (hy<j)
		return 0;
	return hx;
}

U32 getrcv(U32 hy,U32 hx,U32 ord,U32 i)
{
	if (hx>i)
		return hy+1;
	else
		return hy;
}

#endif

#ifdef DOCAYLEYS
class cayleys {
	U32 ord;
	vector<U32> ele;
public:
	cayleys(U32 orda) : ord(orda),ele(ord,0) {}
	bool ok(U32 ed)
	{
		U32 i,j;
		for (j=0;j<ord;++j) {
			U32 c=0;
			for (i=0;i<ed;++i) {
				if (ele[i]==j)
					++c;
			}
			if (c>1) {
				return false;
			}
		}
		return true;
	}
	void print() const
	{
		logger("cs %d: ",ord);
		U32 i;
		for (i=0;i<ele.size();++i)
			logger("%2d ",ele[i]);
		logger("\n");
	}
	friend class cayleyits;
};

class cayleyits {
	U32 order;
	cayleys* c;
public:
	cayleyits(U32 ordera) : order(ordera),c(0)
	{
		logger("in cayleyits cons\n");
		c=new cayleys(order);
		next();
	}
	const cayleys* getcayley()
	{
		return c;
	}
	void next()
	{
		if (order==0) {
			if (c)
				delete c;
			c=0;
			return;
		}
		if (!c)
			return;
		U32 head=order-1;
//		logger("start\n");
//		c->print();
		while(1) {
			++c->ele[head];
//			logger("inc %d\n",head);
//			c->print();
			bool ov=false;
			bool nlok=false;
			if (c->ele[head]==order) {
//				logger("overflow\n");
				ov=true;
			} else if (!c->ok(head)) {
//				logger("not left ok %d\n",head);
				nlok=true;
			}
			if (ov || nlok) {
				if (head==0) {
//					logger("no more\n");
					delete c;
					c=0;
					break;
				} else {
					c->ele[head]=0;
					--head;
//					logger("advacing left head now at %d\n",head);
//					c->print();
				}
			} else if (!c->ok(order)) {
				head=order-1;
//				logger("not ok, reset head to %d\n",head);
			} else {
//				logger("done\n");
//				c->print();
				break;
			}
		}
	}
	~cayleyits()
	{
		if (c)
			delete(c);
	}
};
#endif
#ifdef DOCAYLEY
class cayley {
	U32 ord;
	vector<vector<U32> > ele;
public:
	cayley(U32 orda) : ord(orda)
	{
		U32 i;
		vector<U32> row(ord,0);
		for (i=0;i<ord;++i)
			ele.push_back(row);
	}
	bool isassoc() const
	{
		U32 i,j,k;
		for (k=0;k<ord;++k)
			for (j=0;j<ord;++j)
				for (i=0;i<ord;++i)
					if (ele[ele[i][j]][k]!=ele[i][ele[j][k]])
						return false;
		return true;
	}
	bool ok(U32 hy,U32 hx) const
	{
		U32 i,j,k;
		U32 th;
		if (hy==ord && hx==0) { // special: check ident symmetry
//			if (!isassoc())
//				return false;
			for (j=1;j<ord;++j)
				for (i=0;i<j;++i) {
					if (ele[j][i]==0 && ele[i][j]!=0)
						return false;
					if (ele[j][i]!=0 && ele[i][j]==0)
						return false;
				}
		}
// horz zero row
		th=getrch(hy,hx,ord,0);
		for (k=0;k<th;++k)
			if (ele[0][k]!=k)
				return false;
// vert zero column
		th=getrcv(hy,hx,ord,0);
		for (k=0;k<th;++k)
			if (ele[k][0]!=k)
				return false;

// horz non zero rows
		for (j=0;j<ord;++j) {
			th=getrch(hy,hx,ord,j);
			for (k=0;k<ord;++k) {
				U32 c=0;
				for (i=0;i<th;++i) {
					if (ele[j][i]==k)
						++c;
				}
				if (c>1)
					return false;
			}
		}
// vert non zero columns
		for (i=0;i<ord;++i) {
			th=getrcv(hy,hx,ord,i);
			for (k=0;k<ord;++k) {
				U32 c=0;
				for (j=0;j<th;++j) {
					if (ele[j][i]==k)
						++c;
				}
				if (c>1)
					return false;
			}
		}
		return true;
	}
	void print() const
	{
		if (isassoc())
			logger("in c: order %d is associative\n",ord);
		else
			logger("in c: order %d is NOT associative\n",ord);
		U32 i,j;
		for (j=0;j<ele.size();++j) {
			for (i=0;i<ele[j].size();++i)
				logger("%2d ",ele[j][i]);
			logger("\n");
		}
	}
	friend class cayleyit;
};

class cayleyit {
	U32 order;
	cayley* c;
public:
	cayleyit(U32 ordera) : order(ordera),c(0)
	{
		logger("in cayleyit cons\n");
		c=new cayley(order);
		next();
	}
	const cayley* getcayley()
	{
		return c;
	}
	void next()
	{
		if (order==0) {
			if (c)
				delete c;
			c=0;
			return;
		}
		if (!c)
			return;
		U32 headx=order-1;
		U32 heady=order-1;
//		logger("start\n");
//		c->print();
		while(1) {
			++c->ele[heady][headx];
//			logger("inc %d %d\n",heady,headx);
//			c->print();
			bool ov=false;
			bool nlok=false;
			if (c->ele[heady][headx]==order) {
//				logger("overflow\n");
				ov=true;
			} else if (!c->ok(heady,headx)) {
//				logger("not left ok %d %d\n",heady,headx);
				nlok=true;
			} 
			if (ov || nlok) {
				if (headx==0 && heady==0) {
//					logger("no more\n");
					delete c;
					c=0;
					break;
				} else {
					c->ele[heady][headx]=0;
					if (headx==0) {
						headx=order-1;
						--heady;
					} else
						--headx;
//					logger("advacing left head now at %d %d\n",heady,headx);
//					c->print();
				}
			} else if (!c->ok(order,0)) {
				headx=order-1;
				heady=order-1;
//				logger("not ok, reset head to %d %d\n",heady,headx);
			} else {
//				logger("done\n");
//				c->print();
				break;
			}
		}
	}
	~cayleyit()
	{
		if (c)
			delete(c);
	}
};
#endif
#ifdef TESTLOK

void testlok(U32 hy,U32 hx,U32 ord)
{
	U32 i,j;
	logger("testlok: hy %d, hx %d, ord %d\n",hy,hx,ord);
	for (j=0;j<ord;++j) {
		U32 n=getrch(hy,hx,ord,j);
		logger("on row %d, do %d\n",j,n);
	}
	for (i=0;i<ord;++i) {
		U32 n=getrcv(hy,hx,ord,i);
		logger("on col %d, do %d\n",i,n);
	}
}
#endif

} // end namespace name_cayley

using namespace name_cayley;

void docayley()
{
	logger("################### in docayley #########################\n");
#ifdef DOCAYLEYS
	cayleyits* ci=new cayleyits(3);
	while(1) {
		const cayleys* c=ci->getcayley();
		if (!c)
			break;
		c->print();
		ci->next();
	}
	delete ci;
#endif
#ifdef DOCAYLEY
	U32 i;
	for (i=3;i<=6;++i) {
		cayleyit* ci=new cayleyit(i);
		while(1) {
			const cayley* c=ci->getcayley();
			if (!c)
				break;
			c->print();
			ci->next();
		}
		delete ci;
	}
#endif
#ifdef TESTLOK
	U32 i,j;
	for (j=0;j<LORD;++j)
		for (i=0;i<LORD;++i)
			testlok(j,i,LORD);
	testlok(LORD,0,LORD);
#endif
}
