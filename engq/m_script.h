string script2printc(string s); // convert \ and " to \\ and \"

class script {
	string fname;
//	script& operator=(const class script&);
protected:
	// size of input
	S32 inCharSize;
 // the only data
	vector<string> data;
	U32 ridx; // for read
	enum state {GETCHAR,SKIPWS,HASHMODE,QUOTEMODE,SLASH1,STAR1,STAR2,BAIL,BACKSLASH1,BACKSLASHQ};
	enum chartype {CHARS,WS,CRLF,ISEOF,HASH,QUOTE,SLASH,STAR,BACKSLASH};
	chartype getchartype(C8 c);
public:
	string getfname() const {return fname;} 
/*	virtual ~scriptnoinc()
	{
		logger("in scriptnoinc destructor\n");
	} */
//	S32 refcount;	// for experimental refcounter class, ignore
	string& idx(S32 i)
//	const C8* operator[](S32 i)
	{
		if (i<0 || i>=(S32)data.size())
			errorexit("bad script index [%d]",i);
		return data[i];
//		return data[i].c_str();
	}
	const string& idx(S32 i) const
//	const C8* operator[](S32 i)
	{
		if (i<0 || i>=(S32)data.size())
			errorexit("bad script index [%d]",i);
		return data[i];
//		return data[i].c_str();
	}
	string printidx(S32 i) const
	{
		return script2printc(idx(i));
	}
	script(const C8* fname=0,bool iscstr=false);
//	~script() // not needed
//	{
//	}
	void addscript(const C8* add)
	{
		data.push_back(string(add));
	}
	S32 inchars() const {
		return inCharSize;
	}

	S32 num() const {
		return data.size();
	}
	S32 numchars() const {
		int i,n=data.size();
		int ret = 0;
		for (i=0;i<n;++i)
			ret += data[i].size();
		return ret;
	}
	const C8* read()
	{
		if (ridx==data.size())
			return "";
		return data[ridx++].c_str();
	}
	void backup()
	{
		--ridx;
		if (ridx<0)
			ridx=0;
	}
	void resetread()
	{
		ridx=0;
	}
//	void showline(); // for refcounter class
//	friend class script;
	void sort();
};

class scriptinc : public script {
public:
	scriptinc(const C8* fname=0,const C8* inctweek=0);
};

class scriptdir : public script {
public:
	scriptdir(S32 dirs); // true if you want directories, false if you want files
};
