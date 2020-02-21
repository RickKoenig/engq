extern bitmap32 dummyb;

class textureb {
protected:
	S32 islocked;
	S32 texformat;
private:
	textureb();
	textureb(textureb&);
	textureb& operator=(textureb&);
public:
	bool hasalpha;
	pointi2 origsize;
	pointi2 tsize;
	string name;
	static referencecount<textureb> rc; // the static list, add this
	S32 refidx;
	textureb(string namea);
//	void addbitmap(bitmap32* b); // copy bitmap b into texture
	void build(S32 x,S32 y); // don't load, just make an empty one
//	friend void registermat(struct mater2 *m);	// material to draw tris with (required)
	bool checkname(string instr,string& outstr,bool& iscub,bool& issix);
	bitmap32* gfxread32c(const C8* name,bool issix);
	virtual textureb* snapshot() {return this;}	// for anim tex, if anim tex 'texturebifl' then return current texture
	virtual void showline() // shows info about this instance when showref is called, and add this
	{
		logger("[textureb] format %2d, name '%-12s' size %4d,%4d orig size wid hit %d %d,hasalpha %d",texformat,name.c_str(),tsize.x,tsize.y,origsize.x,origsize.y,hasalpha);
	}
	virtual ~textureb() {}
	virtual void load()=0; // copy bitmap b into texture
	virtual void addbitmap(const bitmap32* b,bool iscube)=0;
	virtual struct bitmap32* locktexture()=0;
	virtual void unlocktexture()=0;
	static void animtex();	// scan through rclist and step all texturebifl's
};

class texturebifl : public textureb
{
	vector<textureb*> texs;
	vector<U32> cnts;
	float curcnt;
	U32 curtex;
private:
	texturebifl();
	texturebifl(texturebifl&);
	texturebifl operator=(texturebifl&);
public:
	texturebifl(string namea);
//	void addbitmap(bitmap32* b); // copy bitmap b into texture
	void build(S32 x,S32 y); // don't load, just make an empty one
	void showline(); // shows info about this instance when showref is called, and add this
	~texturebifl();
	void load(); // copy bitmap b into texture
	void addbitmap(const bitmap32* b,bool iscube);
	struct bitmap32* locktexture();
	void unlocktexture();
	textureb* snapshot();	// for anim tex, if anim tex 'texturebifl' then return current texture
//	friend void registermat(struct mater2 *m);	// material to draw tris with (required)
//	bool checkname(string instr,string& outstr,bool& iscub,bool& issix);
//	bitmap32* gfxread32c(const C8* name,bool issix);
	void step();
};

textureb* ifl_createtexture(const C8* n);
