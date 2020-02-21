#define USEHELPER

class helperobj {
#ifdef USEHELPER
	vector<tree2*> helplist;
	tree2* preload; // prevent reload of models every frame
	static bool linger;
#endif
	helperobj(const helperobj&);
public:
	helperobj();
// don't free 'rt' trees that have helpers in them, call 'reset' first
	void addbox(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c);
	void addline(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c);
	void addvector(tree2* rt,const pointf3& p0,const pointf3& p1,const pointf3& c);
	void addsphere(tree2* rt,const pointf3& p,float s,const pointf3& c);
	void addjack(tree2* rt,const pointf3& p,float s,const pointf3& c);
	void setlinger(bool); // if true, then reset won't remove

	void reset();
	~helperobj();
};

extern S32 usehelper;
