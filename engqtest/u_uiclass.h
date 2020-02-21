string getstringnum(string s,S32& num);
/// base class
class uitree {
/// private
	static class uitree* focus; /// should be on top, last place clicked, proc set if wanted
protected:
	bool doproc,dodraw;
	bool killc;					/// when set, killdeaddudes will call delete, don't call delete yourself!
	vector<uitree*> children;	/// hiearchy
	uitree* parent;				/// hiearchy
/// ui stuff
	static bool usedinput; 		/// input has been used by some node, used by clickfocus()
	static pointf2 fmxy; 		/// float mouse xy
	static pointf2 oldfmxy;		/// for mouseover
/*
	static S32 movecount;		/// for mouseover
	static const S32 MOUSEOVERCOUNT=30;
*/
	static const S32 BORDER=10;
	static S32 num_popup;		/// count of how many class popup their are.
	static class uitree* dest; 	/// where to move/copy tree 'to' , for some popup commands
	static class uitree* tview;	/// what treeview is looking at
/// end ui stuff
	static S32 indentamount;	/// for save pretty formatting
	string name;				/// name of object
//	string name2;				/// extra string, for text or file name (where the data is)
/// protected functions
//	void logrec();
private:
	void procrec();				/// want derived to call uitree::proc, not procrec
protected:
	uitree* focusnext();
	uitree* focusprev();
	virtual void showoutline(S32 grow,C32 color) {}
	void saverec(FILE* fh);
	void printindent(FILE* fh);
public:
/// static functions
	static void showfocus()
	{
		if (!focus)
			return;
		focus->showoutline(4,C32LIGHTMAGENTA);
	}
	static void showdest()
	{
		if (!dest)
			return;
		dest->showoutline(7,C32LIGHTGREEN);
	}
	static S32 get_num_popup()
	{
		return num_popup;
	}
	static void* create(script& sc)
	{
		return new uitree(sc);
	}
	static void setinput();
	static void setfocus(class uitree* f)
	{
//		logger("in setfocus: to %p\n",f);
		focus=f;
	}
	static uitree* getfocus()
	{
		return focus;
	}
	static void setdest(class uitree* d)
	{
		dest=d;
	}
	static uitree* getdest()
	{
		return dest;
	}
/// non virtual functions
	bool getdodraw() const {return dodraw;}
	void setdodraw(bool dd) {dodraw=dd;}
	bool getdoproc() const {return doproc;}
	void setdoproc(bool dd) {doproc=dd;}
	S32 getchildrensize() const { return children.size();}
	uitree* getchildrenidx(S32 i) { return children[i];}
	uitree* getparent() const { return parent;}
	uitree(const C8* na/*,const C8* na2*/) : doproc(true),dodraw(true),killc(false),parent(0),name(na)/*,name2(na2)*/ {}
	uitree(script& sc) : doproc(true),dodraw(true),killc(false),parent(0),name(sc.read())/*,name2(sc.read())*/ {}
	bool killdeaddudes();
	template <typename T>
	T* find(const C8* namef,S32 depth=-1) /// depth first, search depthlevel deep, -1 very deep
	{
//		logger("find '%s'\n",script2print(name).c_str());
		if (!namef || name==namef) {
			T* ret;
			ret=dynamic_cast<T*>(this);
			if (ret)
				return ret;
		}
		if (depth==0)
			return 0;
		--depth;
		S32 i,nc=children.size();
		for (i=0;i<nc;++i) {
			T* ret=children[i]->find<T>(namef,depth);
			if (ret)
				return ret;
		}
		return 0;
	}
	bool focustofront();
	virtual bool focustofrontenabled() {return true;}
	void linkchild(uitree* ch); // to the back
	void linkchildf(uitree* ch); // the dubious to the front
	void copychildrenrec(uitree* n);
	void dokillc() {killc=true;}
	string getname() const {return name;}
	void setname(string s) {name=s;}
//	string getname2() const {return name2;}
//	virtual void setname2(string s) {name2=s;}
//	virtual void loaddata2fn(string fn) {logger("in loaddata2fn base with '%s'\n",fn.c_str());}
//	virtual void savedata2fn(string fn) {}
	virtual void loadscript2fn(string fn);
	virtual void savescript2fn(string fn);
/// virtual functions
	virtual string parentgetstring() const;
//	virtual void parentsetstring(string s);
	virtual void parentloadsavestring(string s) {}
//	virtual void addentry(string s) {}
protected:
	virtual ~uitree();
public:
	virtual const C8* getclassname() const
	{
		return "uitree";
	}
	virtual uitree* copy()
	{
		uitree* n=new uitree(name.c_str()/*,name2.c_str()*/);
		copychildrenrec(n);
		return n;
	}
	virtual void buildo2w();
	virtual void proc();
	virtual void draw();
	virtual bool mouseover();
//	virtual void log()
//	{
//		logger("uitree '%s'\n",script2print(name).c_str());
//		logrec();
//	}
	virtual void save(FILE* fh,bool userec=true);
};

/// 2d derived class
class uitree2d : public uitree {
protected:
	pointf2 o2w;
	pointf2 o2p; // just trans for now
public:
/// static functions
	static void* create(script& sc)
	{
		return new uitree2d(sc);
	}
/// non virtual functions
	uitree2d(const C8* na/*,const C8* na2*/,float xa,float ya) : uitree(na/*,na2*/)
	{
		o2p.x=xa;
		o2p.y=ya;
		o2w=pointf2x(0,0);//zerov2d;
	}
	uitree2d(script& sc) : uitree(sc)
	{
		o2p.x=(float)atof(sc.read());
		o2p.y=(float)atof(sc.read());
		o2w=pointf2x(0,0);
	}
/// new non virtual functions
	pointf2 geto2p() const
	{
		return o2p;
	}
	pointf2 geto2w() const
	{
		return o2w;
	}
	void seto2p(const pointf2& o2pa)
	{
		o2p=o2pa;
	}
/// virtual functions
	const C8* getclassname() const
	{
		return "uitree2d";
	}
	uitree2d* copy()
	{
		uitree2d* n=new uitree2d(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y);
		copychildrenrec(n);
		return n;
	}
	void buildo2w();
	void draw()
	{
		if (!dodraw)
			return;
		uitree::draw();
	}
//	void log()
//	{
//		logger("uitree2d '%s' (%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
/// new non virtual functions for 2d on up
	bool clickfocus();
/// new virtual functions for 2d on up
	virtual bool isinside(const pointf2& p) const
//	virtual bool isinside(const pointf2& p,pointf2* w2o=0) const
	{
		return false;
	}
};

/// 2d rectangle derived class
class uitree2drect : public uitree2d {
protected:
	pointf2 size;
	pointf2 offset; /// 0 to 1
protected:
public:
	void showoutline(S32 grow,C32 color);
/// static functions
	static void* create(script& sc)
	{
		return new uitree2drect(sc);
	}
/// non virtual functions
	void addrem_controls();
	void addrem_scroll();
//	void remove_edit_controls();
	uitree2drect(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa) : uitree2d(na/*,na2*/,xa,ya)
	{
		size.x=xsa;
		size.y=ysa;
		offset.x=0;
		offset.y=0;
	}
	uitree2drect(script& sc) : uitree2d(sc)
	{
		size.x=(float)atof(sc.read());
		size.y=(float)atof(sc.read());
		offset.x=0;
		offset.y=0;
	}
/// new non virtual functions
	void scriptbuildcopy(const C8* ncscripta);
	const pointf2& getsize() const
	{
		return size;
	}
	float getsizex() const
	{
		return size.x;
	}
	float getsizey() const
	{
		return size.y;
	}
	const pointf2& getoffset() const
	{
		return offset;
	}
	float getoffsetx() const
	{
		return offset.x;
	}
	float getoffsety() const
	{
		return offset.y;
	}
	void setoffset(const pointf2& offseta)
	{
		offset=offseta;
	}
	void setoffsetx(float offxa)
	{
		offset.x=offxa;
	}
	void setoffsety(float offya)
	{
		offset.y=offya;
	}
/// virtual functions
	virtual void add_popup_options(class popup* p) const;
	const C8* getclassname() const
	{
		return "uitree2drect";
	}
	uitree2drect* copy()
	{
		uitree2drect* n=new uitree2drect(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
		n->setoffset(offset);
		copychildrenrec(n);
		return n;
	}
	void proc();
	void draw();
	bool mouseover();
//	void log()
//	{
//		logger("uitree2drect '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
	bool isinside(const pointf2& p) const;
//	bool isinside(const pointf2& p,pointf2* w2o=0) const;
	virtual void setsize(const pointf2& sizea) /// virtual because can trigger class specific stuff (like size changed for bitmaps)
	{
		size=sizea;
	}
	void keyoffset(pointf2 step);
};

/// 2d bitmap derived class
class uitree2dbitmap : public uitree2drect {
protected:
	bitmap32* bdata;
	string dirname;
	string fname;
public:
/// static functions
	static void* create(script& sc)
	{
		return new uitree2dbitmap(sc);
	}
/// non virtual functions
	uitree2dbitmap(const C8* na,float xa,float ya,float xsa,float ysa,float offsetx,float offsety,const C8* dira,const C8* fnamea);
	uitree2dbitmap(script& sc);
/// virtual functions
	virtual bitmap32* getbitmap()
	{
		return bdata;
	}
	void add_popup_options(class popup* p) const;
	const C8* getclassname() const
	{
		return "uitree2dbitmap";
	}
	~uitree2dbitmap()
	{
		if (bdata)
			bitmap32free(bdata);
	}
	uitree2dbitmap* copy()
	{
		uitree2dbitmap* nb=new uitree2dbitmap(name.c_str(),o2p.x,o2p.y,size.x,size.y,offset.x,offset.y,dirname.c_str(),fname.c_str());
		if (size.x>0 && size.y>0 && bdata && nb->bdata)
			clipblit32(bdata,nb->bdata,0,0,0,0,bdata->size.x,bdata->size.y);
		copychildrenrec(nb);
		return nb;
	}
	void proc();
	void draw();
//	void log()
//	{
//		logger("uitree2dbitmap '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
/// new non virtual functions
	virtual void loaddata2fn(string fn);
	virtual void savedata2fn(string fn);
	string getbitmapname() const { return fname; }
	string getbitmapdir() const { return dirname; }
};

/// 2d bitmap with file name
class uitree2dbitmapscale : public uitree2dbitmap {
protected:
	bitmap32* bout;
	bool update;
public:
/// static functions
	static void* create(script& sc)
	{
		return new uitree2dbitmapscale(sc);
	}
/// non virtual functions
	uitree2dbitmapscale(const C8* na,float xa,float ya,float xsa,float ysa,float offsetxa,float offsetya,const C8* diraa,const C8* fna);
	uitree2dbitmapscale(script& sc);
/// virtual functions
	bitmap32* getbitmap()
	{
		update=1;
		return uitree2dbitmap::getbitmap();
	}
	const C8* getclassname() const
	{
		return "uitree2dbitmapscale";
	}
	~uitree2dbitmapscale()
	{
		if (bout)
			bitmap32free(bout);
	}
	uitree2dbitmapscale* copy()
	{
		uitree2dbitmapscale* n=new uitree2dbitmapscale(name.c_str(),o2p.x,o2p.y,size.x,size.y,offset.x,offset.y,dirname.c_str(),fname.c_str());
		copychildrenrec(n);
		return n;
	}
//	void log()
//	{
//		logger("uitree2dbitmapscale '%s' '%s' (%f,%f,(%f,%f))\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void draw();
//	void save(FILE* fh,bool usename=true);
	void setsize(const pointf2& sizea);
	void loaddata2fn(string fn)
	{
//		logger("in loaddata2fn scale with '%s'\n",fn.c_str());
		uitree2dbitmap::loaddata2fn(fn);
		update=true;
	} 
};

/// 2d rectangle derived class
class texter : public uitree2drect {
protected:
	string tname;
public:
/// static functions
	static void* create(script& sc)
	{
		return new texter(sc);
	}
/// non virtual functions
	texter(const C8* na,float xa,float ya,float xsa,float ysa,const C8* tna) :
	  uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),tname(tna) {}
	texter(script& sc) : uitree2drect(sc) {tname=sc.read();}
/// virtual functions
	void add_popup_options(class popup* p) const;
	const C8* getclassname() const
	{
		return "texter";
	}
	texter* copy()
	{
		texter* n=new texter(name.c_str(),o2p.x,o2p.y,size.x,size.y,tname.c_str());
		copychildrenrec(n);
		return n;
	}
	void draw();
//	void log()
//	{
//		logger("texter '%s' (%f,%f,%f,%f,'%s')\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y,script2print(name2).c_str());
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
/// new virtual functions
	virtual void settextname(string s) {tname=s;}
	string gettextname() const {return tname;}
};

/// 2d rectangle derived class
class editbox : public texter {
	static const S32 BLINKPER=30;
	S32 cursorpos;
	bool tbpressed,crpressed;
	S32 blink;
public:
/// static functions
	static void* create(script& sc)
	{
		return new editbox(sc);
	}
/// non virtual functions
	editbox(const C8* na,float xa,float ya,float xsa,float ysa,const C8* tna) :
	  texter(na,xa,ya,xsa,ysa,tna),cursorpos(tname.size()),tbpressed(0),crpressed(0),blink(0) {}
	editbox(script& sc) : texter(sc),cursorpos(tname.size()),tbpressed(0),crpressed(0),blink(0) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "editbox";
	}
	void proc();
	editbox* copy()
	{
		editbox* n=new editbox(name.c_str(),o2p.x,o2p.y,size.x,size.y,tname.c_str());
		copychildrenrec(n);
		return n;
	}
	void draw();
//	void log()
//	{
//		logger("editbox '%s' (%f,%f,%f,%f,'%s')\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y,script2print(name2).c_str());
//		logrec();
//	}
//	void save(FILE* fh,bool usename=true);
	void settextname(string s)
	{
		texter::settextname(s);
		cursorpos=tname.size();
	}
	bool tabpressed()
	{
		bool ret=tbpressed;
		tbpressed=false;
		return ret;
	}
	bool enterpressed()
	{
		bool ret=crpressed;
		crpressed=false;
		return ret;
	}
};

/// 2d rectangle derived class
class button : public uitree2drect {
	bool waspressed;
protected:
	bool down;
	string bname;
public:
/// static functions
	static void* create(script& sc)
	{
		return new button(sc);
	}
/// non virtual functions
	button(const C8* na,float xa,float ya,float xsa,float ysa,const C8* bna) :
	  uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),waspressed(false),down(false),bname(bna) {}
	button(script& sc) : uitree2drect(sc),waspressed(false),down(false)	{ bname=sc.read();}
/// virtual functions
	void add_popup_options(class popup* p) const;
	const C8* getclassname() const
	{
		return "button";
	}
	button* copy()
	{
		button* n=new button(name.c_str(),o2p.x,o2p.y,size.x,size.y,bname.c_str());
		copychildrenrec(n);
		return n;
	}
	void proc();
	void draw();
	void save(FILE* fh,bool usename=true);
//	void log()
//	{
//		logger("button '%s' '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
/// new virtual functions for buttons on up
	virtual bool pressed()
	{
		bool ret=waspressed;
		waspressed=false;
		return ret;
	}
	virtual string getbuttonname() const { return bname;}
	void setbuttonname(string s) { bname=s;}
};
/// listbox2d derived class
class listbox2d : public uitree2drect {
protected:
	vector<string> listdata;
	S32 sel;
	bool wassel,wasselmouse;
	S32 maxx;
//	pointf2 offset;
	void copydata(listbox2d* dest)
	{
		S32 i;
		S32 nc=listdata.size();
		for (i=0;i<nc;++i)
			dest->addentry(listdata[i]);
	}
public:
/// static functions
	static void* create(script& sc)
	{
		return new listbox2d(sc);
	}
/// non virtual functions
	listbox2d(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa);
	listbox2d(script& sc);
	void addentry(string s);
	void clearlist() {listdata.clear();sel=-1;maxx=0;}
/// virtual functions
	void add_popup_options(class popup* p) const;
	const C8* getclassname() const
	{
		return "listbox2d";
	}
	listbox2d* copy()
	{
		listbox2d* nb=new listbox2d(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
		copydata(nb);
		nb->setoffset(offset);
		copychildrenrec(nb);
		return nb;
	}
	void proc();
	void draw();
//	void log()
//	{
//		logger("listbox2d '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
//	void save(FILE* fh,bool usename=true);
	S32 getsel() const {return sel;}
	void setsel(S32 sela,bool scrollya);
	bool pressed(bool* pmouse)
	{
		bool ret=wassel;
		wassel=false;
		if (pmouse)
			*pmouse=wasselmouse;
		wasselmouse=false;
		return ret;
	}
//	bool pressedmouse()
//	{
//		return wasselmouse;
//	}
	string getselstr() const
	{
		if (sel>=0)
			return listdata[sel];
		else
			return "";
	}
};
