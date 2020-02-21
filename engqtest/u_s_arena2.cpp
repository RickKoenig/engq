/// test micro state manager
#define D2_3D
#include <m_eng.h>
#include "u_states.h"

//#include <d3d9.h>
//#include "m_vid_dx9.h"
//#include "d2_dx9.h"
#include "u_modelutil.h"
#include "d2_font.h"
#include "m_sagger.h"
#include "d2_dx9_font.h"

namespace u_s_arena2
{
#define FRESX 320
#define FRESY 240

#define VERSION2
#ifdef VERSION2
class shape;
template <typename T>
void deleteishapes(vector<T*> ss)
{
	typename vector<T*>::iterator i;
	for (i = ss.begin(); i != ss.end(); i++) {
		delete *i;
		*i = 0;
	}
}

template <typename T>
class factory {
public:
	T* newclass(const int *&argp)
	{
		errorexit("need a template");
		return 0;
	}
};

#endif

// shape set 1
enum shapeid {
	SID_BLANK,SID_CIR,SID_REC,SID_NUM,
};

const int shlist[] = {
	SID_CIR,11,20,5,
	SID_REC,30,40,2,4,
	SID_CIR,50,20,6,
	SID_CIR,500,200,60,
	SID_BLANK,
};

#ifdef VERSION2

class shape {
	int x,y;
public:
	shape(const int*& argp) : x(argp[0]),y(argp[1])
	{
		argp+=2;
		logger("shape::shape %d %d\n",x,y);
	}
	static void* create(const int*& argp)
	{
		return new shape(argp);
	}
	virtual ~shape()
	{
		logger("shape::~shape\n");
	}
	virtual void draw()
	{
		logger("shape::draw\n");
	}
	virtual void erase() 
	{
		logger("shape::erase\n");
	}
};

class circle : public shape {
	int r;
public:
	circle(const int*& argp) : shape(argp),r(argp[0])
	{
		++argp;
		logger("circle::circle %d\n",r);
	}
	static void* create(const int*& argp)
	{
		return new circle(argp);
	}
	virtual ~circle()
	{
		logger("circle::~circle\n");
	}
	virtual void draw()
	{
		logger("circle::draw\n");
	}
	virtual void erase() 
	{
		logger("circle::erase\n");
	}
};

class rectangle : public shape {
	int w,h;
public:
	rectangle(const int*& argp) : shape(argp),w(argp[0]),h(argp[1])
	{
		argp+=2;
		logger("rectangle::rectangle %d %d\n",w,h);
	}
	static void* create(const int*& argp)
	{
		return new rectangle(argp);
	}
	virtual ~rectangle()
	{
		logger("rectangle::~rectangle\n");
	}
	virtual void draw()
	{
		logger("rectangle::draw\n");
	}
	virtual void erase() 
	{
		logger("rectangle::erase\n");
	}
};

//	void create_circle()
//	{
//		return new circle(argp);
//	}
// this one template ties shape all together
typedef void* (*createfun)(const int *&argp);
template<>
class factory<shape> {
	createfun *createfuns;
//	shape* create_circle(const int *&argp) { return new circle(argp); }
//	shape* create_rectangle(const int *&argp) {	return new rectangle(argp);	}
public:
	factory()
	{
		createfuns = new createfun[SID_NUM];
		createfuns[SID_BLANK]=&shape::create;
		createfuns[SID_CIR]=&circle::create;
		createfuns[SID_REC]=&rectangle::create;
	}
	~factory() { delete[] createfuns; }
	shape* newclass(const int *&argp)
	{
		if (*argp<0 || *argp>=SID_NUM)
			errorexit("unknown shape");
		return(shape*)(*createfuns[*argp++])(argp);
	}
};

#endif

static U32 togvidmode;
static tree2* roottree,*orthoroottree;
static viewport2 orthovp;
static bool quitarena2;


float isect(float a,float b,float x)
{
	float t=(x-a)/(b-a);
	return t;
}

bool line2box3d(const pointf3& bmin,const pointf3& bmax,const pointf3& a,const pointf3& b,float& t)
{
	float tent,text; // entry and exit
	U32 outcodesa,outcodesb,/*orcodes=0,*/andcodes=~0;
	outcodesa=0;
	if (a.x<bmin.x) outcodesa|=1;
	if (a.x>bmax.x) outcodesa|=2;
	if (a.y<bmin.y) outcodesa|=4;
	if (a.y>bmax.y) outcodesa|=8;
	if (a.z<bmin.z) outcodesa|=16;
	if (a.z>bmax.z) outcodesa|=32;
	if (!outcodesa) { // start from inside, return near point
		t=0;
		return true;
	}
	outcodesb=0;
	if (b.x<bmin.x) outcodesb|=1;
	if (b.x>bmax.x) outcodesb|=2;
	if (b.y<bmin.y) outcodesb|=4;
	if (b.y>bmax.y) outcodesb|=8;
	if (b.z<bmin.z) outcodesb|=16;
	if (b.z>bmax.z) outcodesb|=32;
	andcodes = outcodesa & outcodesb;
	if (andcodes) // all outside on same side, no intersection
		return false;
	tent=0;
	text=1;
	U32 ent = outcodesa & ~outcodesb;
	U32 ext =~outcodesa &  outcodesb;
// some intersections to do..
	if (ent&1) {
		float tt=isect(a.x,b.x,bmin.x);
		if (tt>tent) tent=tt;
	}
	if (ent&2) {
		float tt=isect(a.x,b.x,bmax.x);
		if (tt>tent) tent=tt;
	}
	if (ent&4) {
		float tt=isect(a.y,b.y,bmin.y);
		if (tt>tent) tent=tt;
	}
	if (ent&8) {
		float tt=isect(a.y,b.y,bmax.y);
		if (tt>tent) tent=tt;
	}
	if (ent&16) {
		float tt=isect(a.z,b.z,bmin.z);
		if (tt>tent) tent=tt;
	}
	if (ent&32) {
		float tt=isect(a.z,b.z,bmax.z);
		if (tt>tent) tent=tt;
	}
	if (ext&1) {
		float tt=isect(a.x,b.x,bmin.x);
		if (tt<text) text=tt;
	}
	if (ext&2) {
		float tt=isect(a.x,b.x,bmax.x);
		if (tt<text) text=tt;
	}
	if (ext&4) {
		float tt=isect(a.y,b.y,bmin.y);
		if (tt<text) text=tt;
	}
	if (ext&8) {
		float tt=isect(a.y,b.y,bmax.y);
		if (tt<text) text=tt;
	}
	if (ext&16) {
		float tt=isect(a.z,b.z,bmin.z);
		if (tt<text) text=tt;
	}
	if (ext&32) {
		float tt=isect(a.z,b.z,bmax.z);
		if (tt<text) text=tt;
	}
	if (tent<=text) {
		t=tent;
		return true;
	}
	return false;
}

pointf3 interp3d(const pointf3& a,const pointf3& b,float t)
{
	return pointf3x(a.x+t*(b.x-a.x),a.y+t*(b.y-a.y),a.z+t*(b.z-a.z));
}


/*pointf3 interp(const pointf3& a,const pointf3& b,float t)
{
	pointf3 ret;
	return ret;
}
*/
// assumes o2w is built, use sphere for now..
// tree, near point, far point, returns tp if true
bool line2bboxmod(tree2* tr,const pointf3& n,const pointf3& f,float* tp)
{
	modelb* mod=tr->mod;
	if (!mod)
		return false;
//#define SDIST .25
// check against sphere first
	float t;
	pointf3 p1;
	pointf3 s;
	s.x=tr->o2w.e[3][0]-n.x;
	s.y=tr->o2w.e[3][1]-n.y;
	s.z=tr->o2w.e[3][2]-n.z;
	p1.x=f.x-n.x;
	p1.y=f.y-n.y;
	p1.z=f.z-n.z;
	t=dot3d(&s,&p1)/dot3d(&p1,&p1);
//	if (t<0)
	if (t<0 || t>1)
		return false;
	pointf3 r;
	r.x=t*p1.x-s.x;
	r.y=t*p1.y-s.y;
	r.z=t*p1.z-s.z;
	if (dot3d(&r,&r)>mod->boxrad*mod->boxrad)
		return false;
// now check bbox
	mat4 w2o;
	inversemat3d(&tr->o2w,&w2o);
	pointf3 on,of;
	xformvec(&w2o,&n,&on);
	xformvec(&w2o,&f,&of);
	bool ret=line2box3d(mod->boxmin,mod->boxmax,on,of,t);
/*
	pointf3 mins;//(pointf3x(-1e10,-1e10,-1e10));
	pointf3 maxs;//(pointf3x( 1e10, 1e10, 1e10));
	mins.x=isect(&on,&of,mod->boxmin.x);
	mins.y=isect(&on,&of,mod->boxmin.y);
	mins.z=isect(&on,&of,mod->boxmin.z);
	maxs.x=isect(&on,&of,mod->boxmax.x);
	maxs.y=isect(&on,&of,mod->boxmax.y);
	maxs.z=isect(&on,&of,mod->boxmax.z);
	if (mins.x<-1e10 || mins.x>1e10 || maxs.x<-1e10 || maxs.x>1e10) {
		mins.x=-1e10;
		maxs.x=1e10;
	} else if (mins.x>maxs.x)
		swap(mins.x,maxs.x);
	if (mins.y<-1e10 || mins.y>1e10 || maxs.y<-1e10 || maxs.y>1e10) {
		mins.y=-1e10;
		maxs.y=1e10;
	} else if (mins.y>maxs.y)
		swap(mins.y,maxs.y);
	if (mins.z<-1e10 || mins.z>1e10 || maxs.z<-1e10 || maxs.z>1e10) {
		mins.z=-1e10;
		maxs.z=1e10;
	} else if (mins.z>maxs.z)
		swap(mins.z,maxs.z);
	float mina=max(mins.x,max(mins.y,mins.z));
	float maxa=min(maxs.x,min(maxs.y,maxs.z));
	if (mina<=maxa)
		logger("");
	return mina<=maxa;
*/
//	logger("n %f %f %f , f %f %f %f, box min %f %f %f, max %f %f %f\n",
//		on.x,on.y,on.z,of.x,of.y,of.z,mod->boxmin.x,mod->boxmin.y,mod->boxmin.z,mod->boxmax.x,mod->boxmax.y,mod->boxmax.z);
//	return true;
	if (tp)
		*tp=t;
	return ret;
}

/*
// assumes o2w is built, use sphere for now..
bool line2bbox(tree2* tr,const pointf3& n,const pointf3& f,float* tp)
{
	modelb* mod=tr->mod;
	if (!mod)
		return false;
	mod->boxmin;
	mod->boxmax;
	mat4 w2o;
	inversemat3d(&tr->o2w,&w2o);
	pointf3 on,of;
	xformvec(&w2o,&n,&on);
	xformvec(&w2o,&f,&of);
	return false;
}
*/
static float p_best;
static tree2* p_ret;

void pick3dr(tree2* rt,const pointf3& n,const pointf3& f)
{
	float bt;
	if (rt->flags&TF_PICKABLE) {
		if (line2bboxmod(rt,n,f,&bt)) {
			if (!p_ret || bt<p_best) {
				p_best=bt;
				p_ret=rt;
			}
		}
	}
	list<tree2*>::iterator i;
	for (i=rt->children.begin();i!=rt->children.end();++i)
		pick3dr(*i,n,f);
}

static tree2* pick3d(tree2* rt,const pointf3& n,const pointf3& f,float* dist)
{
	p_best=1.1f;
	p_ret=0;
	pick3dr(rt,n,f);
	if (p_ret && dist)
		*dist=p_best;
	return p_ret;
}

void calcvpcast(viewport2* vp,float mox,float moy,float vzdistn,float vzdistf,pointf3* tn,pointf3* tf)
{
	calcviewportmats(vp,false);
	float czdistn =vzdistn*video3dinfo.v2c.e[2][2]+video3dinfo.v2c.e[3][2];
	float czdistnw=vzdistn*video3dinfo.v2c.e[2][3]+video3dinfo.v2c.e[3][3];
	czdistn/=czdistnw;
	float czdistf =vzdistf*video3dinfo.v2c.e[2][2]+video3dinfo.v2c.e[3][2];
	float czdistfw=vzdistf*video3dinfo.v2c.e[2][3]+video3dinfo.v2c.e[3][3];
	czdistf/=czdistfw;
	pointf3 n,f;
	n.x=-1.0f+(mox-vp->xstart)* 2.0f/vp->xres;
	n.y= 1.0f+(moy-vp->ystart)*-2.0f/vp->yres;
	n.z=czdistn;
	n.w=1.0f;
	f=n;
	f.z=czdistf;
	mat4 c2w;
	inversemat4(&video3dinfo.w2c,&c2w);
	mulmatvec4(&c2w,&n,tn);
	mulmatvec4(&c2w,&f,tf);
	float rw=1.0f/tn->w;
	tn->x*=rw;
	tn->y*=rw;
	tn->z*=rw;
	tn->w=1;
	rw=1.0f/tf->w;
	tf->x*=rw;
	tf->y*=rw;
	tf->z*=rw;
	tf->w=1;
}

// micro state manager
// class factory
////////////// generic class factory routines
template <typename T>
class mfact {
	typedef void* (*createfun)(); // handy typedef for the create function
	map<string,createfun> funmap2; // factory constructor fills this in
public:
// makes 1 object from handle, alters state of handle, whole heirarchy or whatever
	T* newclass_from_name(string n);
	mfact(); // specialize to eliminate linker errors
};

template <typename T>
T* mfact<T>::newclass_from_name(string n)
{
	T* ret=0;
	map<string,createfun>::iterator found=funmap2.find(n); // find the create function, given the id
	if (found==funmap2.end())
		errorexit("unknown shape '%s'",script2printc(n).c_str());
	createfun s=found->second;
	ret=(T*)(*s)(); // create object
	return ret;
}

// base
class us
{
protected:
	static list<us*> schildren; // proc list
	list<us*>::iterator it; // iterator to self
//	bool kill;
public:
	class us_state* man; // handle to state manager
	us();
	virtual ~us();
	virtual void proc();
	static void* create() {return new us;} // for factory
	static bool gproc(); // called by arena2 proc, procs all schildren
};
us::us() : man(0) // : kill(false)
{
//	logger("in us init\n");
	schildren.push_back(this);
	it=schildren.end();
	--it; // here is the precious iterator that points to self
}
us::~us()
{
//	logger("in us exit\n");
	schildren.erase(it);
}
void us::proc()
{
	//logger("in us proc\n");
}
bool us::gproc()
{
//	logger("in us gproc\n");
	if (!schildren.size())
		return false;
	list<us*>::iterator i=schildren.begin();
	while(i!=schildren.end()) {
		(*i)->proc();
		++i;
	}
	return true;
}
list<us*> us::schildren;
static mfact<us> * facto;

// some derived estates
// state driver, handles slave proc
class us_state: public us {
	bool dochange;
	us* cur;
	string next;
public:
	us_state();
	void changestate(string s);
	bool isdeadstate();
	~us_state();
	void proc();
	static void* create() {return new us_state;}
};
us_state::us_state() : dochange(false),cur(0)
{
//	logger("in us state init \n");
}
us_state::~us_state()
{ 
//	logger("in us state exit\n");
	if (cur)
		delete cur;
}
void us_state::proc()
{
//	logger("in us state proc\n");
	if (dochange) {
		if (cur)
			delete cur;
		us* st;
		if (!next.size())
			st=0;
		else
			st=facto->newclass_from_name(next);
		cur=st;
		if (st)
			st->man=this;
		dochange=false;
	}
}
void us_state::changestate(string newstatea)
{
//	logger("in us state changestate '%s'\n",newstatea.c_str());
	dochange=true;
	next=newstatea;
}
bool us_state::isdeadstate()
{
	return cur==0 && !dochange;
}

// title
class us_title : public us {
	tree2* tit;
	S32 cnt;
public:
	us_title();
	~us_title();
	void proc();
	static void* create() {return new us_title;}
};
us_title::us_title() : cnt(0)
{
//	logger("in us title init\n");
	drawtextque_string(SMALLFONT,0,0,F32WHITE,"Title");
	modelb* tm=d2_drawtextque_build(); // draw que
	tit=new tree2("texttree");
	tit->setmodel(tm);
	orthoroottree->linkchild(tit);
}
us_title::~us_title()
{
//	logger("in us title exit\n");
	delete tit;
}
void us_title::proc()
{
//	logger("in us title proc\n");
	++cnt;
	if (cnt==90 || (MBUT&~LMBUT)) {
		if (man)
			man->changestate("us_mainmenu");
	} else if (KEY==K_ESCAPE) {
		man->changestate(""); //quitarena2=true;
	}
}

// mainmenu
bool fader(tree2* t)
{
	t->treecolor.w-=.03f;
	if (t->treecolor.w<.5f)
		t->treecolor.w=.5f;
	return true;
}
#define NMAINMENU 4
class us_mainmenu : public us {
	S32 cnt;
	tree2* mt,*mmt[4],*spin,*d3cursm;
public:
	us_mainmenu();
	~us_mainmenu();
	void proc();
	static void* create() {return new us_mainmenu;}
};
us_mainmenu::us_mainmenu() : cnt(0)
{
	videoinfo.showcursor=0;
//	logger("in us mainmenu init\n");
	drawtextque_string(SMALLFONT,150,0,F32WHITE,"Main Menu");
	modelb* tm=d2_drawtextque_build(); // draw que
	tree2* mm=new tree2("texttree");
	mm->setmodel(tm);
	orthoroottree->linkchild(mm);
	mt=mm;

	pointf3 F32BLACKA=pointf3x(0,0,0,.5);
	drawtextque_string_foreback(SMALLFONT,100,32,F32WHITE,F32BLACKA,"Play");
	tm=d2_drawtextque_build(); // draw que
	mm=new tree2("texttree");
	mm->userproc=fader;
	mm->setmodel(tm);
	mm->flags|=TF_PICKABLE;
	mm->treecolor.w=.5f;
	orthoroottree->linkchild(mm);
	mmt[0]=mm;

	drawtextque_string_foreback(SMALLFONT,100,52,F32WHITE,F32BLACKA,"Help");
	tm=d2_drawtextque_build(); // draw que
	mm=new tree2("texttree");
	mm->userproc=fader;
	mm->setmodel(tm);
	mm->flags|=TF_PICKABLE;
	mm->treecolor.w=.5f;
	orthoroottree->linkchild(mm);
	mmt[1]=mm;

	drawtextque_string_foreback(SMALLFONT,100,72,F32WHITE,F32BLACKA,"Options");
	tm=d2_drawtextque_build(); // draw que
	mm=new tree2("texttree");
	mm->userproc=fader;
	mm->setmodel(tm);
	mm->flags|=TF_PICKABLE;
	mm->treecolor.w=.5f;
	orthoroottree->linkchild(mm);
	mmt[2]=mm;

	drawtextque_string_foreback(SMALLFONT,100,92,F32WHITE,F32BLACKA,"Quit");
	tm=d2_drawtextque_build(); // draw que
	mm=new tree2("texttree");
	mm->userproc=fader;
	mm->setmodel(tm);
	mm->flags|=TF_PICKABLE;
	mm->treecolor.w=.5f;
	orthoroottree->linkchild(mm);
	mmt[3]=mm;

	spin=new tree2("spinner");
	spin->rotvel=pointf3x(0,.05f,0);
	tree2* t=buildprism(pointf3x(3,1,.25),"maptestnck.tga","tex");
	t->trans.x=5;
	spin->linkchild(t);
	t=t->newdup();
	t->trans.x=-5;
	spin->linkchild(t);
	roottree->linkchild(spin);
	{
		modelb* mod2=model_create("sphere3o");
		if (model_getrc(mod2)==1)
		    buildpatch(mod2,15,15,1,1,spheref_surf(6.25f),"maptestnck.tga","tex");
		d3cursm=new tree2("spheretree3o");
		d3cursm->setmodel(mod2);
		mod2->mats[0].msflags&=~SMAT_HASWBUFF;
		orthoroottree->linkchild(d3cursm);
	}
}
us_mainmenu::~us_mainmenu()
{
//	logger("in us mainmenu exit\n");
	S32 i;
	for (i=0;i<NMAINMENU;++i)
		delete mmt[i];
	delete mt;
	delete spin;
	delete d3cursm;
	videoinfo.showcursor=1;
}
void us_mainmenu::proc()
{
	if (MBUT || KEY || DMX || DMY)
		cnt=0;
//	logger("in us mainmenu proc\n");
	++cnt;
	if (cnt==190) {
		if (man)
			man->changestate("us_demo");
	}
	if (KEY==K_ESCAPE) {
		if (man)
			man->changestate("us_title");
	}
	pointf3 mn,mf;
	calcvpcast(&orthovp,(float)MX,(float)MY,-25,25,&mn,&mf);
	float dst;
	d3cursm->trans=mn;
	tree2* pt=pick3d(orthoroottree,mn,mf,&dst);
	if (pt) {
		d3cursm->treecolor=F32RED;
		pt->treecolor.w+=.06f;
		if (pt->treecolor.w>1)
			pt->treecolor.w=1;
		if (MBUT&~LMBUT) {
			if (pt==mmt[0]) {
				man->changestate("us_demo"); // play
			} else if (pt==mmt[1]) {
				man->changestate("us_demo"); // help
			} else if (pt==mmt[2]) {
				man->changestate("us_demo"); // options
			} else if (pt==mmt[3]) {
				man->changestate(""); // quit
			}
		}
	} else
		d3cursm->treecolor=F32YELLOW;

}

// demo
class us_demo : public us {
	S32 cnt;
	tree2* mmt,*spin;
	tree2* d3curs;
public:
	us_demo();
	~us_demo();
	void proc();
	static void* create() {return new us_demo;}
};
us_demo::us_demo() : cnt(0)
{
//	logger("in us demo init\n");

	drawtextque_string(SMALLFONT,0,0,F32WHITE,"D E M O");
	modelb* tm=d2_drawtextque_build(); // draw que
	mmt=new tree2("texttree");
	mmt->setmodel(tm);
	orthoroottree->linkchild(mmt);

	spin=new tree2("spinner");
//	spin->rotvel=pointf3x(.025f,.05f,0);
	spin->rot=pointf3x(PI/4,PI/4,0);
	roottree->linkchild(spin);

	tree2* t=buildprism(pointf3x(3,1,.25),"maptestnck.tga","tex");
	t->trans.x=5;
	t->flags|=TF_PICKABLE;
	spin->linkchild(t);

	t=t->newdup();
	t->trans.x=-5;
	spin->linkchild(t);

	t=t->newdup();
	t->trans=pointf3x(0,5,0);
	spin->linkchild(t);

	t=t->newdup();
	t->trans=pointf3x(0,-5,0);
	spin->linkchild(t);

	t=t->newdup();
	t->trans=pointf3x(0,0,5);
	spin->linkchild(t);

	t=t->newdup();
	t->trans=pointf3x(0,0,-5);
	spin->linkchild(t); 
	{
		modelb* mod2=model_create("sphere3");
		if (model_getrc(mod2)==1)
		    buildpatch(mod2,15,15,1,1,spheref_surf(.0625f),"bark.tga","tex");
		d3curs=new tree2("spheretree3");
		d3curs->setmodel(mod2);
		mod2->mats[0].msflags&=~SMAT_HASWBUFF;
		roottree->linkchild(d3curs);
	}
	videoinfo.showcursor=0;
}
us_demo::~us_demo()
{
//	logger("in us demo exit\n");
	delete mmt;
	delete spin;
	delete d3curs;
	videoinfo.showcursor=1;
}

void us_demo::proc()
{
	pointf3 mn,mf;
	calcvpcast(&mainvp,(float)MX,(float)MY,4,20,&mn,&mf);
	d3curs->trans=mn;
	float dst;
	if (pick3d(roottree,mn,mf,&dst))
		d3curs->treecolor=F32RED;
	else
		d3curs->treecolor=F32YELLOW;
	++cnt;
	if (cnt==1900 || KEY==K_ESCAPE /*|| (MBUT&~LMBUT) */) {
		if (man)
			man->changestate("us_mainmenu");
	}
}

// pizazz
class us_fluff : public us {
	tree2* ft,*ft2;
//	S32 cnt;
public:
	us_fluff();
	~us_fluff();
	void proc();
	static void* create() {return new us_fluff;}
};
us_fluff::us_fluff() //: cnt(0)
{
//	logger("in us fluff init\n");
	drawtextque_string(SMALLFONT,0,16,F32WHITE,"Fluff");
	modelb* tm=d2_drawtextque_build(); // draw que
	ft=new tree2("flufftree");
	ft->setmodel(tm);
	orthoroottree->linkchild(ft);
	ft2=buildprism(pointf3x(1.5f,.5f,.1f),"maptestnck.tga","tex");
	ft2->trans=pointf3x(-5,0,10);
	roottree->linkchild(ft2);

}
us_fluff::~us_fluff()
{
//	logger("in us fluff exit\n");
	delete ft;
	delete ft2;
}
void us_fluff::proc()
{
//	logger("in us fluff proc\n");
//	++cnt;
//	if (cnt==60) {
//		logger("");//delete this;
//	}
}



// main driver
class us_main : public us {
	us_state* mst;
	us* piz;
public:
	us_main();
	~us_main();
	void proc();
	static void* create() {return new us_main;}
};
us_main::us_main()
{
//	logger("in us main init\n");
	piz=new us_fluff;
	mst=new us_state;
	mst->changestate("us_title");
//	mst->changestate("us_demo");
}
us_main::~us_main()
{ 
//	logger("in us main exit\n");
	delete mst;
	delete piz;
}
void us_main::proc()
{
//	logger("in us main proc\n");
	if (mst->isdeadstate())
		quitarena2=true;
}

template <>
mfact<us>::mfact()
{
	funmap2["us"]=us::create; // base
		funmap2["us_main"]=us_main::create;  //
		funmap2["us_fluff"]=us_fluff::create; // constant pizazz
		funmap2["us_state"]=us_state::create; // state driver
// states
		funmap2["us_title"]=us_title::create;
		funmap2["us_mainmenu"]=us_mainmenu::create;
		funmap2["us_demo"]=us_demo::create;
//		funmap2["us_options"]=us_options::create;
}

static us* usmain;
} // end namespace
using namespace u_s_arena2;
void arena2init()
{
	pushandsetdir("gfxtest");
	const int *argp=shlist;
	vector<shape*> shapes;
	factory<shape> sf;
	logger("***************** creating\n");
	while(*argp!=SID_BLANK) {
		shapes.push_back(sf.newclass(argp));
//		break;
	}
	logger("***************** drawing\n");
	for (vector<shape*>::iterator i = shapes.begin();i<shapes.end(); ++i)
		(*i)->draw();
	logger("***************** erasing\n");
	for (vector<shape*>::iterator i = shapes.begin();i<shapes.end(); ++i)
		(*i)->erase();
	logger("***************** deleting\n");
	deleteishapes<shape>(shapes);
//	wininfo.fpswanted=5;
// setup video
	video_setupwindow(GX,GY);
// setup root
	roottree=new tree2("roottree");
	orthoroottree=new tree2("orthoroottree");
// setup main viewport
	mainvp.backcolor=C32BLUE;
	mainvp.zfront=.25f;
	mainvp.zback=400;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
//	mainvp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
//	mainvp.lookat=0;
//	mainvp.isortho=true;
//	mainvp.ortho_size=7;
//	mainvp.camtrans=zerov;
	mainvp.camtrans=pointf3x(0,0,-10);
	mainvp.camrot.x=0;//1e-20f;
// setup ortho viewport
//	orthovp.backcolor=C32LIGHTGREEN;
	orthovp.zfront=-50.0f;
	orthovp.zback=50.0f;
	orthovp.xres=WX;
	orthovp.yres=WY;
	orthovp.xstart=0;
	orthovp.ystart=0;
	orthovp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	orthovp.flags=0;//|VP_CHECKER;
//	orthovp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	orthovp.xsrc=FRESX;
	orthovp.ysrc=FRESY;
	orthovp.useattachcam=false;
//	orthovp.lookat=0;
	orthovp.camtrans=pointf3x(FRESX/2,-FRESY/2,0);
	orthovp.camrot.x=0;//1e-20f;
	orthovp.ortho_size=FRESY;
	orthovp.isortho=true;
#ifdef USESHADOWMAP
// setup lightviewport
	lightvp.backcolor=C32WHITE;
	lightvp.zfront=-200;
	lightvp.zback=200;
//	lightvp.camzoom=.875f;//3.2f; // it'll getit from tree camattach if you have one
	lightvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
	lightvp.xsrc=1;
	lightvp.ysrc=1;
	lightvp.useattachcam=false;
	lightvp.lookat=0;
	lightvp.isortho=true;
	lightvp.ortho_size=25;
	lightvp.xstart=0;
	lightvp.ystart=0;
	lightvp.xres=SHADOWMAP_SIZE;
	lightvp.yres=SHADOWMAP_SIZE;
#endif
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
// set micro state
	facto=new mfact<us>;
	usmain=new us_main;
	quitarena2=false;
}

void arena2proc()
{
//	logger("arena1proc\n");
	if (quitarena2) {
		poporchangestate(STATE_MAINMENU);
	}
	switch(KEY) {
// housekeeping
//	case K_ESCAPE:
//		poporchangestate(STATE_MAINMENU);
//		break;
	case 'a':
		showcursor(1);
		break;
	case 'h':
		showcursor(0);
		break;
	case ' ':
		video3dinfo.favorshading^=1;
		break;
	case 's':
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
		break;
	case '=':
		changeglobalxyres(1);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
		break;
	case '-':
		changeglobalxyres(-1);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
	}
//	usmain->proc();
	doflycam(&mainvp);
	roottree->proc();
	if (!us::gproc()) // the the micro state manager's global proc
		poporchangestate(STATE_MAINMENU);
	orthoroottree->proc();
// update viewport
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.xres=WX;
	mainvp.yres=WY;
	video_buildworldmats(roottree); // help dolights
	video_buildworldmats(orthoroottree); // help dolights

// draw some 3d/2d text
#ifndef MINIMAL
	static S32 frm;
	drawtextque_format(SMALLFONT,200,10,F32RED,"frame %d",frm);
	drawtextque_format_foreback(SMALLFONT,200,42,pointf3x(1.0f,1.0f,1.0f,.5f),pointf3x(0.0f,0.0f,0.0f,.5f),"fps avg  %f",wininfo.fpsavg);
	drawtextque_format_foreback(SMALLFONT,200,50,pointf3x(1.0f,1.0f,1.0f,.5f),pointf3x(0.0f,0.0f,0.0f,.5f),"fps avg2 %f",wininfo.fpsavg2);
	++frm;
#endif
}

void arena2draw3d()
{
	dolights();
// draw 3d
//#ifdef SHADOWMAP // from engine
#ifdef USESHADOWMAP // from user
	if (video_rendertargetshadowmap()) {
		lightvp.usec2w=true;
		lightvp.c2w=lightinfo.light2world[0];
		video_setviewport(&lightvp); // clear zbuf etc.
		video_drawscene(roottree);
		video_rendertargetmain();
	}
#endif
//#endif
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
	video_setviewport(&orthovp); // clear zbuf etc.
	video_drawscene(orthoroottree);
}

void arena2exit()
{
	logger("logging roottree\n");
	roottree->log2();
	logger("logging orthoroottree roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete usmain;
// free
	delete roottree;
	delete orthoroottree;
	logger("logging reference lists after free\n");
	logrc();
	popdir();
	mainvp.usev2w=false;
	delete facto;

}

#if 0
static tree2* spawntree,*explodetree;
//#define MINIMAL

static backsndplayer* abacksndplayer;
static waveslotplayer* awaveslotplayer;

viewport2 orthovp;

static tree2* buildenvironmentmapmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("env");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(1.0f),"CUB_cube.jpg","normal.jpg",matname);
	}
	r->setmodel(modb);
	return r;
}

static tree2* buildgroundmodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("ground");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
	    buildpatch(modb,20,20,20,20,planexz_surf(200,200),"rengst.jpg",matname);
	}
	r->setmodel(modb);
	return r;
}

/// build the spheretree model if this is the first one
static tree2* buildsphere(const C8* modelname,const C8* matname)
{
	modelb* mod2=model_create("sphere3");
	if (model_getrc(mod2)==1) {
        pushandsetdir("gfxtest");
//	    buildpatch(mod,20,20,4,4,square1xy,"maptest.tga");
//	    buildpatch(mod,20,20,20,20,sphere1,"maptest.tga");
	    buildpatch(mod2,35,35,3,3,spheref_surf(.25f),"bark.tga","tex");
	    popdir();
	}
	tree2* spheretree2=new tree2("spheretree3");
	spheretree2->setmodel(mod2);
	return spheretree2;
}

/// build the spheretree model if this is the first one
static tree2* buildtext()
{
	drawtextque_string(0,0,F32WHITE,"Hello World!");
	S32 i;
	for (i=0;i<320;i+=8)
		drawtextque_string(0,i,F32WHITE,"0123456789012345678901234567890123456789");
	modelb* tm=d2_drawtextque_build(); // draw que
	tree2* tt=new tree2("texttree");
	tt->setmodel(tm);
	return tt;
}

#ifdef VIEWSHADOWMAP
static tree2* buildshadowmapviewermodel(const C8* modelname,const C8* matname)
{
	tree2* r=new tree2("sbm");
	modelb* modb=model_create(modelname);
	if (model_getrc(modb)==1) {
		pushandsetdir("gfxtest");
//	    buildpatch_tan(modb,12,6,12,6,spheref_surf_tan(5.0f),"bark.tga","ts18_2crop.jpg",matname);
	    buildpatch(modb,1,1,1,1,planexz_surf(10.0f,10.0f),"shadowmap",matname);
//	    buildpatch_tan(modb,24,12,6,3,spheref_surf_tan(5.0f),"maptest.tga","normal.jpg",matname);
		popdir();
	}
	r->setmodel(modb);
	return r;
}
#endif

/*
class test {
	void (*userprocfun)(test* t);
public:
	string name;
	test(string namea) : name(namea) {}
	void proc()
	{
		logger("intree proc\n");
		if (userprocfun)
			(*userprocfun)(this);
	}
	void setuserprocfun(void (*userprocfuna)(test* t))
	{
		userprocfun=userprocfuna;
	}
};

void afun(test* t)
{
	logger("in afun '%s'\n",t->name.c_str());
}
*/

static bool carcam;
static tree2* cartree;

// tree userprocs

static bool cardrive(tree2* t)
{
	S32 leftright=0,foreback=0;
	if (wininfo.keystate[K_RIGHT]||wininfo.keystate[K_NUMRIGHT])
		++leftright;
	if (wininfo.keystate[K_LEFT]||wininfo.keystate[K_NUMLEFT])
		--leftright;
	if (wininfo.keystate[K_UP]||wininfo.keystate[K_NUMUP])
		++foreback;
	if (wininfo.keystate[K_DOWN]||wininfo.keystate[K_NUMDOWN])
		--foreback;
	if (foreback>=0)
		cartree->rot.y+=.025f*leftright;
	else
		cartree->rot.y-=.025f*leftright;
	cartree->trans.x+=foreback*.2f*sinf(cartree->rot.y);
	cartree->trans.z+=foreback*.2f*cosf(cartree->rot.y);
	return true;
}

// move and explode
static bool explode(tree2* t)
{
	++t->userint[0];
	if (t->userint[0]>=10) {
		delete t;
		return false;
	}
	t->treecolor.w=1.0f-(float)t->userint[0]/10.0f;
	t->scale=pointf3x(t->userint[0]*2.5f,t->userint[0]*2.5f,t->userint[0]*2.5f);
	return true;
}

static bool flyers(tree2* t)
{
	if (t->userint[0]==0)
		t->userint[1]=4+mt_random(200);
	++t->userint[0];
	if (t->userint[0]*3>=t->userint[1]) {
		awaveslotplayer->settrack(mt_random(awaveslotplayer->getnumtracks()));
		tree2* tt=explodetree->newdup();
		roottree->linkchild(tt);
		tt->trans=t->trans;
		delete t;
		return false;
	}
	t->treecolor.w=1.0f-(float)t->userint[0]/(float)t->userint[1];
	return true;
}

static bool textfun(tree2* t)
{
	pointf3 c=F32WHITE;
	c.w=(t->userint[0]&0xff)/255.0f;
	drawtextque_format(0,0,c,"user var is '%d'",t->userint[0]++);
	modelb* tm=d2_drawtextque_build(); // draw que
	t->setmodel(tm);
	return true;
}

// just move back and forth, spawn flyers
static bool backforth(tree2* t)
{
	bool spawn=false;
	if (t->userfloat[0]==0)
		t->userfloat[0]=1;
	if (t->userfloat[0]==1) {
		if (t->trans.x>5) {
			t->userfloat[0]=-1;
			spawn=true;
		}
	} else {
		if (t->trans.x<-5) {
			t->userfloat[0]=1;
			spawn=true;
		}
	}
	t->transvel.x=.1f*t->userfloat[0];
//	spawn=false;
	if (spawn) {
		awaveslotplayer->settrack(mt_random(awaveslotplayer->getnumtracks()));
		S32 i;
		for (i=0;i<12;++i) {
			tree2* tt=spawntree->newdup();
			roottree->linkchild(tt);
			tt->trans=t->trans;
			tt->transvel.x=.4f*(mt_frand()*2-1);
			tt->transvel.y=.4f*(mt_frand());
			tt->transvel.z=.4f*(mt_frand()*2-1);
			tt->rotvel.x=(PI/40)*(mt_frand()*2-1);
			tt->rotvel.y=(PI/40)*(mt_frand()*2-1);
			tt->rotvel.z=(PI/40)*(mt_frand()*2-1);
		}
	}
	return true;
}

// micro state manager
class us
{
public:
	virtual void init()=0;
	virtual void proc()=0;
	virtual void exit()=0;
};

class ush {
	us* cs;
public:
	void sproc();
//	void add(

};

class us_title : us {
};

////////////////////////// main
void arena2init()
{
	pushandsetdir("audio/backsnd");
	scriptdir* sc=new scriptdir(0);
	sc->sort();
	abacksndplayer=new backsndplayer(*sc);
	delete sc;
	popdir();

	pushandsetdir("audio/weapsounds_deploy_vag");
//	pushandsetdir("audio/weapsounds_activate_vag");
//	pushandsetdir("audio/waves");
	sc=new scriptdir(0);
	sc->sort();
	awaveslotplayer=new waveslotplayer(*sc);
	delete sc;
	popdir();
/*	test* a = new test("hi");
	a->setuserprocfun(afun);
	a->proc();
	delete a; */
// setup video
	video_setupwindow(GX,GY);

// setup root
	roottree=new tree2("roottree");
	orthoroottree=new tree2("orthoroottree");
	tree2* tt;
	pushandsetdir("gfxtest");

#ifndef MINIMAL
// build a environmentmap model
	tt=buildenvironmentmapmodel("envmodel","env");
	tt->trans=pointf3x(0,1,10);
	tt->setuserproc(backforth);
	tt->flags|=TF_CASTSHADOW;
	roottree->linkchild(tt);
#endif	


// build a ground
#ifdef USESHADOWMAP
	tt=buildgroundmodel("backgnd","useshadowmap");
#else
	tt=buildgroundmodel("backgnd","pdiffspec");
#endif
	roottree->linkchild(tt);
//	tt->rotvel.x=.01f;
//	tt->rotvel.y=.01f;
//	tt->rotvel.z=.01f;

#ifndef MINIMAL
// build a spawntree
	spawntree=buildprism(pointf3x(1,1,1),"maptestnck.tga","pdiffspec");
	spawntree->flags|=TF_CASTSHADOW;
	spawntree->setuserproc(flyers);
	explodetree=buildsphere("sphere3","pdiffspec");
	explodetree->treecolor=F32YELLOW;
	explodetree->setuserproc(explode);

// build a skybox
	pushandsetdir("skybox");
	tt=buildskybox(pointf3x(10,10,10),"cube.jpg","tex");
	popdir();
	roottree->linkchild(tt);
//	tt->mod->mats[0].msflags|=SMAT_HASNOFOG; 
	
// build a car
	tt=buildprism(pointf3x(5,3,10),"CUB_cube.jpg","env");
	roottree->linkchild(tt);
	tt->flags|=TF_CASTSHADOW;
	tt->setuserproc(cardrive);
	cartree=tt;

// build some text
	tt=buildtext();
	tt->setuserproc(textfun);
	orthoroottree->linkchild(tt);
	
// build a cylinder
	tt=buildcylinder_xz(pointf3x(1,3,1),"bark.tga","pdiffspec");
	tt->trans=pointf3x(5,0,10);
	setcastshadow(tt);	// will set TF_CASTSHADOW for hierarchy
	roottree->linkchild(tt);

#ifdef VIEWSHADOWMAP
// build shadowmap viewer
	tt=buildshadowmapviewermodel("smv","shadowmapviewer");
	tt->trans.y=6;
	tt->trans.x=10;
	tt->trans.z=30;
	tt->rot.x=-PI/2;
	roottree->linkchild(tt);
#endif

// add a directional light
	tt=new tree2("adirlight");
	tt->flags|=TF_ISLIGHT;
	tt->rot.x=PI/4;
	tt->rotvel.y=PI/800;
	tt->lightcolor=pointf3x(.75f,.75f,.75f);
	addlighttolist(tt);
	roottree->linkchild(tt);
	
// add an amblight
	tt=new tree2("aamblight");
	tt->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
	tt->lightcolor=pointf3x(.25f,.25f,.25f);
	addlighttolist(tt);
	roottree->linkchild(tt);
#endif
// setup main viewport
	mainvp.backcolor=C32LIGHTGREEN;
	mainvp.zfront=.125f;
	mainvp.zback=400;
	mainvp.xres=WX;
	mainvp.yres=WY;
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.camzoom=2;//3.2f; // it'll getit from tree camattach if you have one
	mainvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
//	mainvp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	mainvp.xsrc=4;
	mainvp.ysrc=3;
	mainvp.useattachcam=false;
	mainvp.lookat=0;
	mainvp.camtrans.y=2.5;
	mainvp.camrot.x=0;//1e-20f;
// setup ortho viewport
//	orthovp.backcolor=C32LIGHTGREEN;
	orthovp.zfront=-.5f;
	orthovp.zback=.5f;
	orthovp.xres=WX;
	orthovp.yres=WY;
	orthovp.xstart=0;
	orthovp.ystart=0;
	orthovp.camzoom=1;//3.2f; // it'll getit from tree camattach if you have one
	orthovp.flags=0;//|VP_CHECKER;
//	orthovp.flags=/*VP_CLEARBG|*/VP_CLEARWB;//|VP_CHECKER;
	orthovp.xsrc=FRESX;
	orthovp.ysrc=FRESY;
	orthovp.useattachcam=false;
	orhtovp.lookat=0;
	orthovp.camtrans=pointf3x(FRESX/2,-FRESY/2,0);
	orthovp.camrot.x=0;//1e-20f;
	orthovp.ortho_size=FRESY;
	orthovp.isortho=true;
#ifdef USESHADOWMAP
// setup lightviewport
	lightvp.backcolor=C32WHITE;
	lightvp.zfront=-200;
	lightvp.zback=200;
//	lightvp.camzoom=.875f;//3.2f; // it'll getit from tree camattach if you have one
	lightvp.flags=VP_CLEARBG|VP_CLEARWB;//|VP_CHECKER;
	lightvp.xsrc=1;
	lightvp.ysrc=1;
	lightvp.useattachcam=false;
	lightvp.lookat=0;
	lightvp.isortho=true;
	lightvp.ortho_size=25;
	lightvp.xstart=0;
	lightvp.ystart=0;
	lightvp.xres=SHADOWMAP_SIZE;
	lightvp.yres=SHADOWMAP_SIZE;
#endif
// keep track of current video device
	togvidmode=videoinfo.video_maindriver;
	abacksndplayer->settrack(5);
	carcam=true;
}

void arena2proc()
{
//	logger("arena1proc\n");
	switch(KEY) {
// housekeeping
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case 'b':
		{
			S32 t=abacksndplayer->gettrack();
			if (t<0)
				abacksndplayer->settrack(4);
			else
				abacksndplayer->settrack(t+1);
		}
		break;
	case 'l':
		carcam^=1;
		break;
	case 'a':
		showcursor(1);
		break;
	case 'h':
		showcursor(0);
		break;
	case ' ':
		video3dinfo.favorshading^=1;
		break;
	case 's':
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^2);
		togvidmode^=2;
		video_init(togvidmode,0);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
		break;
	case '=':
		changeglobalxyres(1);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
		break;
	case '-':
		changeglobalxyres(-1);
//			video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
		break;
	}
	if (wininfo.mmiddleclicks) {
		logger("setting vidmode from %d to %d\n",togvidmode,togvidmode^1);
		togvidmode^=1;
		video_init(togvidmode,0);
//		video_setupwindow(GX,GY);
		changestate(STATE_ARENA2);
	}
	roottree->proc();
	orthoroottree->proc();
// update viewport
	mainvp.xstart=0;
	mainvp.ystart=0;
	mainvp.xres=WX;
	mainvp.yres=WY;
	doflycam(&mainvp);

// draw some 3d/2d text
#ifndef MINIMAL
	static S32 frm;
	drawtextque_format(200,10,F32RED,"frame %d",frm);
	drawtextque_format_foreback(200,42,pointf3x(1.0f,1.0f,1.0f,.5f),pointf3x(0.0f,0.0f,0.0f,.5f),"fps avg  %f",wininfo.fpsavg);
	drawtextque_format_foreback(200,50,pointf3x(1.0f,1.0f,1.0f,.5f),pointf3x(0.0f,0.0f,0.0f,.5f),"fps avg2 %f",wininfo.fpsavg2);
	drawtextque_format_foreback(200,70,F32GREEN,F32BLACK,"current backgnd track %d",abacksndplayer->gettrack());
	++frm;
#endif
}

void arena2draw3d()
{
	video_buildworldmats(roottree); // help dolights
	if (carcam) {
		mat4 co;
		identmat4(&co);
		co.e[3][0]=0; co.e[3][1]=5; co.e[3][2]=-20;
		mulmat3d(&co,&cartree->o2w,&mainvp.c2w);
		mainvp.usec2w=true;
	} else {
		mainvp.usec2w=false;
	}
	dolights();
// draw 3d
#ifdef SHADOWMAP // from engine
#ifdef USESHADOWMAP // from user
	if (video_rendertargetshadowmap()) {
		lightvp.usec2w=true;
		lightvp.c2w=lightinfo.light2world[0];
		video_setviewport(&lightvp); // clear zbuf etc.
		video_drawscene(roottree);
		video_rendertargetmain();
	}
#endif
#endif
	video_setviewport(&mainvp); // clear zbuf etc.
	video_drawscene(roottree);
	video_setviewport(&orthovp); // clear zbuf etc.
	video_drawscene(orthoroottree);
}

void arena2exit()
{
// log and free
	logger("logging roottree\n");
	roottree->log2();
	logger("logging reference lists\n");
	logrc();
	delete roottree;
	delete orthoroottree;
#ifndef MINIMAL
	delete spawntree;
	delete explodetree;
#endif
	popdir();
	delete abacksndplayer;
	delete awaveslotplayer;
	mainvp.usec2w=false;
}
#endif
