/// 2d rectangle derived class
class sizer : public uitree2drect {
	pointf2 ref;
public:
/// static functions
	static void* create(script& sc)
	{
		return new sizer(sc);
	}
/// non virtual functions
	sizer(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa) : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa) { ref.x=0;ref.y=0;}
	sizer(script& sc) : uitree2drect(sc) { ref.x=0;ref.y=0;}
/// virtual functions
	const C8* getclassname() const
	{
		return "sizer";
	}
	sizer* copy()
	{
		sizer* n=new sizer(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
		copychildrenrec(n);
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("sizer '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
};

/// 2d rectangle derived class
class mover : public uitree2drect {
	pointf2 ref;
	float rbord;
public:
/// static functions
	static void* create(script& sc)
	{
		return new mover(sc);
	}
/// non virtual functions
	mover(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa) : uitree2drect(na/*,na2*/,xa,ya,xsa,ysa),rbord(xsa) { ref.x=0;ref.y=0;}
	mover(script& sc) : uitree2drect(sc) { ref.x=0;ref.y=0;rbord=size.x;}
/// virtual functions
	const C8* getclassname() const
	{
		return "mover";
	}
	mover* copy()
	{
		mover* n=new mover(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,rbord,size.y);
		copychildrenrec(n);
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("mover '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
	void setsize(const pointf2& sizea);
};

/// 2d rectangle derived class
class xscroll : public uitree2drect {
//	float yval;
public:
/// static functions
	static void* create(script& sc)
	{
		return new xscroll(sc);
	}
/// non virtual functions
	xscroll(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa);
	xscroll(script& sc);
/// virtual functions
	const C8* getclassname() const
	{
		return "xscroll";
	}
	xscroll* copy()
	{
		xscroll* n=new xscroll(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("xscroll '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
//	void save(FILE* fh,bool userec=true);
};

class yscroll : public uitree2drect {
//	float yval;
public:
/// static functions
	static void* create(script& sc)
	{
		return new yscroll(sc);
	}
/// non virtual functions
	yscroll(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa);
	yscroll(script& sc);
/// virtual functions
	const C8* getclassname() const
	{
		return "yscroll";
	}
	yscroll* copy()
	{
		yscroll* n=new yscroll(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("yscroll '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
//	void save(FILE* fh,bool userec=true);
};

class xslider : public uitree2drect {
protected:
	float topval,botval,val;
	float i2e(float i) const
	{
		return topval+i*(botval-topval);
	}
	float e2i(float e) const
	{
//		if (topval>=botval)
//			return 0;
		float reti=(e-topval)/(botval-topval);
		return range(0.0f,reti,1.0f);
	}
public:
/// static functions
	static void* create(script& sc)
	{
		return new xslider(sc);
	}
/// non virtual functions
	xslider(const C8* na,float xa,float ya,float xsa,float ysa,float topvala,float botvala,float vala);
	xslider(script& sc);
/// virtual functions
	const C8* getclassname() const
	{
		return "xslider";
	}
	xslider* copy()
	{
		xslider* n=new xslider(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,topval,botval,getval());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("xslider '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
	virtual float gettopval() const {return topval;}
	virtual float getbotval() const {return botval;}
	virtual float getval() const {return i2e(val);}
	virtual void settopval(float topvala)
	{
		float e=getval();
		topval=topvala; //min(topvala,botval);
		setval(e);
	}
	virtual void setbotval(float botvala)
	{
		float e=getval();
		botval=botvala; //max(botvala,topval);
		setval(e);
	}
	virtual void setval(float vala) {val=e2i(vala);}
};

class yslider : public xslider {
//	float yval;
public:
/// static functions
	static void* create(script& sc)
	{
		return new yslider(sc);
	}
/// non virtual functions
	yslider(const C8* na,float xa,float ya,float xsa,float ysa,float topvala,float botvala,float vala);
	yslider(script& sc);
/// virtual functions
	const C8* getclassname() const
	{
		return "yslider";
	}
	yslider* copy()
	{
		yslider* n=new yslider(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,topval,botval,getval());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("yslider '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
//	void save(FILE* fh,bool userec=true);
};
#if 0
class xsliderx : public ui2drect {
public:
/// non virtual functions
	xsliderx(const C8* na,float xa,float ya,float xsa,float ysa,float topvala,float botvala,float vala,const C8* scriptstr) :
//	  xslider(na,xa,ya,xsa,ysa,topvala,botvala,vala) {}
	  ui2drect(xa,ya,xsa,ysa) {}
	xsliderx(script& sc) {}
/// static functions
	static void* create(script& sc)
	{
		return new xsliderx(sc);
	}
/// virtual functions
	const C8* getclassname() const
	{
		return "xsliderx";
	}
	xsliderx* copy()
	{
		xsliderx* n=new xsliderx(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,topval,botval,getval(),0);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
//	void draw();
//	void log()
//	{
//		logger("xslider '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
	virtual float gettopval() const {return topval;}
	virtual float getbotval() const {return botval;}
	virtual float getval() const {return i2e(val);}
	virtual void settopval(float topvala);
	virtual void setbotval(float botvala);
	virtual void setval(float vala);
};

class ysliderx : public yslider {
public:
	ysliderx(const C8* na,float xa,float ya,float xsa,float ysa,float topvala,float botvala,float vala,const C8* scriptstr) :
	  yslider(na,xa,ya,xsa,ysa,topvala,botvala,vala) {}
};
#endif
class treeview : public listbox2d {
	vector<uitree*> tp;
	S32 blink;
	S32 newsel;
void populate(uitree* from);
public:
/// static functions
	static void* create(script& sc)
	{
		return new treeview(sc);
	}
/// non virtual functions
	treeview(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa);
	treeview(script& sc);
/// virtual functions
	const C8* getclassname() const
	{
		return "treeview";
	}
	treeview* copy()
	{
		treeview* n=new treeview(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
//		copydata(n);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	void proc();
	void draw();
//	void log()
//	{
//		logger("xslider '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
//	void save(FILE* fh,bool userec=true);
};

class popup : public uitree2drect {
//	void setup_popup(uitree2drect* par);
	vector <popup_info> piv;
//	S32 nbut;
	S32 curwid;
public:
	void setup_popup(const popup_info* choices);
/// static functions
/*	static void* create(script& sc)
	{
		return new popup(sc);
	} */
/// non virtual functions
	popup(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa);
//	popup(script& sc);
/// virtual functions
	~popup()
	{
		--num_popup;
	}
	const C8* getclassname() const
	{
		return "popup";
	}
	popup* copy()
	{
//		errorexit("don't copy popups");
		popup* n=new popup(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y);
//		n->pi=pi;
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
//	void log()
//	{
//		logger("popup '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true);
};

//typedef	void (uitree::*msetnamefunp)(string s);
//typedef	string (uitree::*mgetnamefunp)() const;
//typedef void (uitree::*mloadsavedatafunp)(string lsn);
class changename : public uitree2drect {
	editbox* eb;
	button* ok,*can;
//	typedef S32 chf;
//	chf chnamefun;
//	mgetnamefunp gn;
//	msetnamefunp sn;
//	float yval;
protected:
	string cnscript;
public:
/// static functions
/// non virtual functions
//	changename(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa/*,mgetnamefunp gna,msetnamefunp sna*/);
	changename(const C8* na/*,const C8* na2*/,float xa,float ya,float xsa,float ysa,const C8* cnscripta);
/// virtual functions
	const C8* getclassname() const
	{
		return "changename";
	}
	changename* copy()
	{
		changename* n=new changename(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,cnscript.c_str()/*,gn,sn*/);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
//	void log()
//	{
//		logger("changename '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true) {return;}	// don't save changename
//	parentloadsavestring()
	void parentloadsavestring(string s);
};	

class changetextname : public changename {
public:
/// static functions
/// non virtual functions
	changetextname(const C8* na,float xa,float ya,float xsa,float ysa,const C8* cnscripta) : changename(na,xa,ya,xsa,ysa,cnscripta) {}
/// virtual functions
	string parentgetstring() const;
	void parentloadsavestring(string s);
	const C8* getclassname() const
	{
		return "changetextname";
	}
	changetextname* copy()
	{
		changetextname* n=new changetextname(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,cnscript.c_str()/*,gn,sn*/);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
};	

class changebuttonname : public changename {
public:
/// static functions
/// non virtual functions
	changebuttonname(const C8* na,float xa,float ya,float xsa,float ysa,const C8* cnscripta) : changename(na,xa,ya,xsa,ysa,cnscripta) {}
/// virtual functions
	string parentgetstring() const;
	void parentloadsavestring(string s);
	const C8* getclassname() const
	{
		return "changebuttonname";
	}
	changebuttonname* copy()
	{
		changebuttonname* n=new changebuttonname(name.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,cnscript.c_str()/*,gn,sn*/);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
};	

class changeslidertopval : public changename {
public:
/// static functions
/// non virtual functions
	changeslidertopval(const C8* na,float xa,float ya,float xsa,float ysa,const C8* cnscripta) : changename(na,xa,ya,xsa,ysa,cnscripta) {}
/// virtual functions
	string parentgetstring() const;
	void parentloadsavestring(string s);
	const C8* getclassname() const
	{
		return "changeslidertopval";
	}
	changeslidertopval* copy()
	{
		changeslidertopval* n=new changeslidertopval(name.c_str(),o2p.x,o2p.y,size.x,size.y,cnscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
};	

class changesliderbotval : public changename {
public:
/// static functions
/// non virtual functions
	changesliderbotval(const C8* na,float xa,float ya,float xsa,float ysa,const C8* cnscripta) : changename(na,xa,ya,xsa,ysa,cnscripta) {}
/// virtual functions
	string parentgetstring() const;
	void parentloadsavestring(string s);
	const C8* getclassname() const
	{
		return "changesliderbotval";
	}
	changesliderbotval* copy()
	{
		changesliderbotval* n=new changesliderbotval(name.c_str(),o2p.x,o2p.y,size.x,size.y,cnscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
};	

class listbox2daddentry : public changename {
public:
/// static functions
/// non virtual functions
	listbox2daddentry(const C8* na,float xa,float ya,float xsa,float ysa,const C8* cnscripta) : changename(na,xa,ya,xsa,ysa,cnscripta) {}
/// virtual functions
	string parentgetstring() const;
	void parentloadsavestring(string s);
	const C8* getclassname() const
	{
		return "listbox2daddentry";
	}
	listbox2daddentry* copy()
	{
		listbox2daddentry* n=new listbox2daddentry(name.c_str(),o2p.x,o2p.y,size.x,size.y,cnscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
};	

class loadscriptdialog : public uitree2drect {
protected:
//	editbox* eb;
	listbox2d* lb;
	button* ok,*can;
//	typedef S32 chf;
//	chf chnamefun;
//	mgetnamefunp gn;
//	msetnamefunp sn;
//	mloadsavedatafunp lsd;
//	float yval;
	string revert;
	bool issave;
	texter* tb;
	editbox* eb;
	texter *tbd;
	string curdir,revertdir;
	S32 nups;	/// 0 or 1
	S32 ndirs;
	S32 nfiles;
	bool prevu;
	const C8** exts;
	void populateloadsavelist();
protected:
	bool checkext(string fname);
	string lsscript;
public:
/// static functions
/// non virtual functions
	loadscriptdialog(const C8* na,const C8* startdir/*,const C8* na2*/,float xa,float ya,float xsa,float ysa/*,mgetnamefunp gna,msetnamefunp sna,mloadsavedatafunp lsna*/,bool issavea,bool prevua,const C8** extsa,const C8* lsscripta);
/// virtual functions
	const C8* getclassname() const
	{
		return "loadscriptdialog";
	}
	loadscriptdialog* copy()
	{
		loadscriptdialog* n=new loadscriptdialog(name.c_str(),curdir.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,/*gn,sn,lsd,*/issave,prevu,exts,lsscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
//	void log()
//	{
//		logger("loadscriptdialog '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
	void save(FILE* fh,bool userec=true) { return; }
	string parentgetstring() const { return ""; }
	void parentloadsavestring(string s);
//	void parentloadsavestring(string s);
};	

class savescriptdialog : public loadscriptdialog {
public:
/// static functions
/// non virtual functions
	savescriptdialog(const C8* na,const C8* dira,float xa,float ya,float xsa,float ysa,bool issavea,bool prevua,const C8** extsa,const C8* cnscripta) :
		loadscriptdialog(na,dira,xa,ya,xsa,ysa,issavea,prevua,extsa,cnscripta) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "savescriptdialog";
	}
	savescriptdialog* copy()
	{
		savescriptdialog* n=new savescriptdialog(name.c_str(),curdir.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,/*gn,sn,lsd,*/issave,prevu,exts,lsscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	void parentloadsavestring(string s);
};	

class loadbitmapdialog : public loadscriptdialog {
public:
/// static functions
/// non virtual functions
	loadbitmapdialog(const C8* na,const C8* dira,float xa,float ya,float xsa,float ysa,bool issavea,bool prevua,const C8** exta,const C8* cnscripta) :
		loadscriptdialog(na,dira,xa,ya,xsa,ysa,issavea,prevua,exta,cnscripta) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "loadbitmapdialog";
	}
	loadbitmapdialog* copy()
	{
		loadbitmapdialog* n=new loadbitmapdialog(name.c_str(),curdir.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,/*gn,sn,lsd,*/issave,prevu,exts,lsscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	string parentgetstring() const;
	void parentloadsavestring(string s);
};	

class savebitmapdialog : public loadscriptdialog {
public:
/// static functions
/// non virtual functions
	savebitmapdialog(const C8* na,const C8* dira,float xa,float ya,float xsa,float ysa,bool issavea,bool prevua,const C8** extsa,const C8* cnscripta) :
		loadscriptdialog(na,dira,xa,ya,xsa,ysa,issavea,prevua,extsa,cnscripta) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "savescriptdialog";
	}
	savebitmapdialog* copy()
	{
		savebitmapdialog* n=new savebitmapdialog(name.c_str(),curdir.c_str()/*,name2.c_str()*/,o2p.x,o2p.y,size.x,size.y,/*gn,sn,lsd,*/issave,prevu,exts,lsscript.c_str());
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	string parentgetstring() const;
	void parentloadsavestring(string s);
};	

class cloner : public button {
public:
/// static functions
	static void* create(script& sc)
	{
		return new cloner(sc);
	}
/// non virtual functions
	cloner(const C8* na,float xa,float ya,float xsa,float ysa,const C8* bna) :
	  button(na,xa,ya,xsa,ysa,bna) {}
	cloner(script& sc) : button(sc) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "cloner";
	}
	cloner* copy()
	{
		cloner* n=new cloner(name.c_str(),o2p.x,o2p.y,size.x,size.y,bname.c_str());
		copychildrenrec(n);
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
	void save(FILE* fh,bool userec=true);
//	void log()
//	{
//		logger("cloner '%s' '%s' (%f,%f,%f,%f)\n",name.c_str(),name2.c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
};

/// 2d rectangle derived class
class deleter : public button {
public:
/// static functions
	static void* create(script& sc)
	{
		return new deleter(sc);
	}
/// non virtual functions
	deleter(const C8* na,float xa,float ya,float xsa,float ysa,const C8* bna) :
	  button(na,xa,ya,xsa,ysa,bna) {}
	deleter(script& sc) : button(sc) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "deleter";
	}
	deleter* copy()
	{
		deleter* n=new deleter(name.c_str(),o2p.x,o2p.y,size.x,size.y,bname.c_str());
		copychildrenrec(n);
		return n;
	}
//	void add_popup_options(class popup* p) const;
	void proc();
	void draw();
	void save(FILE* fh,bool userec=true);
//	void log()
//	{
//		logger("deleter '%s' '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
};

/// 2d rectangle derived class
class buttest0 : public button {
public:
/// static functions
	static void* create(script& sc)
	{
		return new buttest0(sc);
	}
/// non virtual functions
	buttest0(const C8* na,float xa,float ya,float xsa,float ysa,const C8* bna) :
	  button(na,xa,ya,xsa,ysa,bna) {}
	buttest0(script& sc) : button(sc) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "buttest0";
	}
	buttest0* copy()
	{
		buttest0* n=new buttest0(name.c_str(),o2p.x,o2p.y,size.x,size.y,bname.c_str());
		copychildrenrec(n);
		return n;
	}
	void proc();
//	void save(FILE* fh,bool userec=true);
//	void log()
//	{
//		logger("buttest0 '%s' '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
};
/// 2d rectangle derived class
class buttest1 : public button {
public:
/// static functions
	static void* create(script& sc)
	{
		return new buttest1(sc);
	}
/// non virtual functions
	buttest1(const C8* na,float xa,float ya,float xsa,float ysa,const C8* bna) :
	  button(na,xa,ya,xsa,ysa,bna) {}
	buttest1(script& sc) : button(sc) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "buttest1";
	}
	buttest1* copy()
	{
		buttest1* n=new buttest1(name.c_str(),o2p.x,o2p.y,size.x,size.y,bname.c_str());
		copychildrenrec(n);
		return n;
	}
	void proc();
//	void save(FILE* fh,bool userec=true);
//	void log()
//	{
//		logger("buttest1 '%s' '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
};

/// 2d rectangle derived class
class checkbox : public texter {
	bool checked;
protected:
	bool down;
public:
/// static functions
	static void* create(script& sc)
	{
		return new checkbox(sc);
	}
/// non virtual functions
	checkbox(const C8* na,float xa,float ya,float xsa,float ysa,const C8* tna,bool checkeda) :
	  texter(na,xa,ya,xsa,ysa,tna),checked(checkeda),down(false) {}
	checkbox(script& sc) : texter(sc),down(false)
	{
		checked=(atoi(sc.read())!=0);
	}
/// virtual functions
	const C8* getclassname() const
	{
		return "checkbox";
	}
	checkbox* copy()
	{
		checkbox* n=new checkbox(name.c_str(),o2p.x,o2p.y,size.x,size.y,tname.c_str(),checked);
		copychildrenrec(n);
		return n;
	}
	bool getcheck() const {return checked;}
	virtual void setcheck(bool v) {checked=v;}
	void proc();
	void draw();
	void save(FILE* fh,bool userec=true);
//	void log()
//	{
//		logger("checkbox '%s' '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
};

class radiobutton : public checkbox {
public:
/// static functions
	static void* create(script& sc)
	{
		return new radiobutton(sc);
	}
/// non virtual functions
	radiobutton(const C8* na,float xa,float ya,float xsa,float ysa,const C8* tna,bool checkeda) :
	  checkbox(na,xa,ya,xsa,ysa,tna,checkeda) {}
	radiobutton(script& sc) : checkbox(sc) {}
/// virtual functions
	const C8* getclassname() const
	{
		return "radiobutton";
	}
	radiobutton* copy()
	{
		radiobutton* n=new radiobutton(name.c_str(),o2p.x,o2p.y,size.x,size.y,tname.c_str(),0);
		copychildrenrec(n);
		return n;
	}
	void setcheck(bool v);
//	void proc();
	void draw();
//	void save(FILE* fh,bool userec=true);
//	void log()
//	{
//		logger("checkbox '%s' '%s' (%f,%f,%f,%f)\n",script2print(name).c_str(),script2print(name2).c_str(),o2p.x,o2p.y,size.x,size.y);
//		logrec();
//	}
};

class tabs : public uitree2drect {
	S32 idx;
public:
/// static functions
	static void* create(script& sc)
	{
		return new tabs(sc);
	} 
/// non virtual functions
	tabs(const C8* na,float xa,float ya,float xsa,float ysa) : uitree2drect(na,xa,ya,xsa,ysa),idx(-1) {}
	tabs(script& sc) : uitree2drect(sc),idx(-1) {}
/// virtual functions
	virtual bool focustofrontenabled() {return false;}
	const C8* getclassname() const
	{
		return "tabs";
	}
	tabs* copy()
	{
		tabs* n=new tabs(name.c_str(),o2p.x,o2p.y,size.x,size.y);
		copychildrenrec(n); /// popup has enough children already!
		return n;
	}
	void proc();
	void draw(); 
};
