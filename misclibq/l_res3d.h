namespace res3d {
	extern float VMX,VMY;
// res classes
/////////// shape base class
class shape {
	void loglevel(S32 lev) const;
protected:
	string name;
	S32 x,y;
	/*mutable */ S32 vis;
	vector<shape*> children;
//	class shaperoot* root; // this is where 'focus' will be
public:
	shape* parent;
	const vector<shape*>& getchildren() { return children; }
	shape* findrec(const C8* namea); // returns 0 if not found
	shape* find(const C8* namea); // errorexit if not found
	template <typename T>
	T* find(const C8* namea)
	{
		shape* ret=findrec(namea);
		if (!ret)
			errorexit("can't find '%s'\n",namea);
		T* retdc=dynamic_cast<T*>(ret);
		if (!retdc) {
			errorexit("can't downcast '%s' from 'shape*' to '%s'\n",namea,typeid(T).name());
			return 0;
		}
		return retdc;
	}
	void addchild(shape* c)	{ children.push_back(c);c->parent=this; }
	shape(const C8* namea,S32 xa=0,S32 ya=0) : name(namea),x(xa),y(ya),vis(1),/*root(0),*/parent(0) {}
	shape(script& sc);
	static shape* createi()	{ return new shape("",0,0);	}
	virtual ~shape();
	static shape* createh(script& sc) {	return new shape(sc); }
	virtual void show() const;
//	virtual void draw() const;
	virtual void draw3d() const;
	void setvis(S32 visa) { vis=visa; }
	S32 getvis() const { return vis; }
	shape* getover();
	const C8* getname() { return name.c_str(); }
	virtual S32 procfocus();
	virtual void procover();
	virtual void procnotover();
	void over_no_over_proc(shape* focus,shape* over);
	virtual S32 isinside() { return 0; }
	S32 getx() const { return x;}
	S32 gety() const { return y;}
	void setx(S32 xa) { x=xa;}
	void sety(S32 ya) { y=ya;}
	virtual void deactivate() {}
};

/*
/////////// shaperoot derived class, only shaperoot should be root, holds focus
class shaperoot : public shape {
protected:
	const shape* focus;
public:
	shaperoot(script& sc);
	shaperoot(const C8* namea,S32 xa,S32 ya);
	static shape* createh(script& sc) {	return new shaperoot(sc); }
	void show() const;
	void setfocus(const shape* fa) {focus=fa;}
};
*/
/////////// back derived class
class back : public shape {
protected:
	C32 col;
public:
	S32 xs,ys;
	back(script& sc);
	back(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa);
	static shape* createh(script& sc) {	return new back(sc); }
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 isinside() { return VMX>=x && VMY>=y && VMX<x+xs && VMY<y+ys; }
};

/////////// text derived class
class text : public back {
protected:
	string texts;
public:
	text(script& sc);
	static shape* createh(script& sc) { return new text(sc); }
	void show() const;
//	void draw() const;
	void draw3d() const;
	virtual void settname(const C8* tnamea) { texts=tnamea; }
	const C8* gettname() const { return texts.c_str(); }
};

/////////// edit derived class
class edit : public text {
	static const C32 darkcol;
	static const C32 brightcol;
	static const C32 focuscol;
	mutable S32 active;
	mutable S32 blink;
	S32 pos;
	S32 pos2mx() const;
	void mx2pos(S32);
	void checkpos();
public:
	edit(script& sc) : text(sc),active(0),blink(0),pos(0) {col = darkcol;}
	static shape* createh(script& sc) { return new edit(sc); }
	void settname(const C8* tnamea);
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 procfocus();
	void procover();
	void procnotover();
//	virtual void deactivate();
};

/////////// pbut derived class
class pbut : public text {
	S32 down;
	static const C32 darkcol;
	static const C32 brightcol;
	static const C32 focuscol;
public:
	pbut(script& sc) : text(sc),down(0) { col = darkcol;}
	static shape* createh(script& sc) {	return new pbut(sc); }
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 procfocus();
	void procover();
	void procnotover();
};

/////////// vscroll derived class
class vscroll : public back {
protected:
	S32 idx;
	S32 numidx;
	S32 offset;
	S32 cnt;
	S32 pagedir;
	S32 indrag;
	void setidxi(S32 idxa); // without offset
public:
	vscroll(script& sc);
	vscroll(const C8* namea,S32 xa,S32 ya,S32 xsa,S32 ysa); // for listbox
	~vscroll();
	static shape* createh(script& sc) {	return new vscroll(sc);	}
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 procfocus();

	S32 getidx() const { return idx+offset; }
	S32 getnumidx() const { return numidx; }
	S32 getoffset() const { return offset; }
	void getminmaxval(S32& minv,S32& maxv) const { minv=getoffset(); maxv=getoffset()+getnumidx()-1; }

	void setidx(S32 idxa) { setidxi(idxa-offset); }
	void setnumidx(S32 numidxa) { numidx=numidxa; }
	void setoffset(S32 offseta) { offset=offseta; }
	void setminmaxval(S32 minv,S32 maxv) { setoffset(minv); setnumidx(maxv-minv+1); }
};

/////////// hscroll derived class from vscroll
class hscroll : public vscroll {
public:
	hscroll(script& sc);
	~hscroll();
	static shape* createh(script& sc) {	return new hscroll(sc);	}
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 procfocus();
};

/////////// listbox derived class
class listbox : public back {
	S32 idx;
	S32 numoffsets;
	vector<string> thelist;
	void rangeoffset();
	void setscroll();
	S32 mx;
	static const C32 darkcol;
	static const C32 brightcol;
//	bool fireamsg; // when vscroll changes, firea listbox msg
public:
	enum listsortkind {SK_NORMAL,SK_REVERSE,SK_IPS,SK_INTVAL};
	void setmax(S32 mxa) {mx=mxa;}
	S32 curoffset;
	listbox(script& sc) : back(sc),idx(-1),numoffsets(1),mx(-1),/*fireamsg(false),*/curoffset(0) { col = darkcol;}
	~listbox();
	static shape* createh(script& sc) {	return new listbox(sc);	}
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 procfocus();
	void procover();
	void procnotover();
	S32 sort(listsortkind lsk=SK_NORMAL); // returns where last ele went
	void addstring(const C8*); // add at the end, idx = the new size()-1
	void addstringstay(const C8*); // add at the end, idx untouched
	void printf(const C8*,...); // add at the end, idx untouched, just like addstringstay but with formmated text
	void addstring(const C8*,S32 where); // where = retval of listbox::sort
	void changestring(const C8*,S32 where);
	S32 findstring(const C8*) const;
	void setidxv(S32 idxa); // visible, ajdusts offset
	void setidxc(S32 idxa); // center adjusts offset
	void setidxv() {setidxv(idx);}
	void setidxc() {setidxc(idx);}
	S32 getidx() const { return idx; }
	S32 getnumidx() const { return thelist.size(); }
	void removeidx(S32 idx);
	void removeidx();
	const C8* getidxname() const;
	const C8* getidxname(S32 ind) const;
	void clear() { thelist.clear(); idx=-1; curoffset=0; numoffsets=1; setscroll(); }
//	S32 getnumoffsets() const { return numoffsets; }	
//	S32 getcuroffset() const { return curoffset; }	
//	void setcuroffset(S32 offa); // doesn't ajust offset
};

/////////// listbox derived class, multi columns
class listboxn : public back {
	S32 idx;
	S32 numoffsets;
	vector <vector<string> > thelist;
	void rangeoffset();
	void setscroll();
	S32 mx;
	static const C32 darkcol;
	static const C32 brightcol;
	vector<S32> columnsx;
public:
	enum listsortkind {SK_NORMAL,SK_REVERSE,SK_IPS,SK_INTVAL};
	void setmax(S32 mxa) {mx=mxa;}
	S32 curoffset;
	listboxn(script& sc) : back(sc),idx(-1),numoffsets(1),mx(-1),curoffset(0) { col = darkcol;}
	~listboxn();
	static shape* createh(script& sc) {	return new listboxn(sc);	}
	void show() const;
//	void draw() const;
	void draw3d() const;
	S32 procfocus();
	void procover();
	void procnotover();
	void sort(); // returns where last ele went
	void addstring(const vector<string> &); // add at the end, idx = the new size()-1
	void addstringstay(const vector<string> &); // add at the end, idx untouched
	void addstring(const vector<string> &,S32 where); // where = retval of listbox::sort
	void changestring(const vector<string> &,S32 where);
	S32 findstring(const C8*) const; // column 0
	void setidxv(S32 idxa); // visible, ajdusts offset
	void setidxc(S32 idxa); // center adjusts offset
	void setidxv() {setidxv(idx);}
	void setidxc() {setidxc(idx);}
	S32 getidx() const { return idx; }
	S32 getnumidx() const { return thelist.size(); }
	void removeidx(S32 idx);
	void removeidx();
	vector<string> getidxname() const;
	vector<string> getidxname(S32 ind) const;
	void clear() { thelist.clear(); idx=-1; curoffset=0; numoffsets=1; setscroll(); }
//	S32 getnumoffsets() const { return numoffsets; }	
//	S32 getcuroffset() const { return curoffset; }	
//	void setcuroffset(S32 offa); // doesn't ajust offset
	void setcolumnsx(const vector<S32>& colsx) {columnsx=colsx;}
	void setcol(S32 col,const C8* cs);
};

void init_res3d(float vx,float vy); // sizeof virtual screen, for mouse translation, like for 'video_sprite_begin'
void exit_res3d();
}

shape* res_loadfile(const C8* str);
C32 drift_col(const C32 src,const C32 dst,float t); // t from 0 to 1, 0 stop, 1 instant

