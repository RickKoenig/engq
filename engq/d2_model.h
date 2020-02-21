/// software model ////
#define NUMEXTRA 16
// allow for neg idx from -16 to -1, used for clipping
template <typename T>
class vectore
{
private:
	vectore(vectore& ve);
	vectore& operator=(vectore&);
	S32 numele;
	T* database;
	T* dataoff;
public:
	void setsize(S32 sizea)
	{
		delete[] database;
		numele=sizea;
		database = new T[numele+NUMEXTRA];
		dataoff=database+NUMEXTRA;
	}
	vectore() : numele(0),database(0),dataoff(0) {}
	vectore(S32 numelea) : numele(numelea)
	{
		database = new T[numele+NUMEXTRA];
		dataoff=database+NUMEXTRA;
	}
	S32 size() {return numele;}
	T& operator[](S32 idx) {return *(dataoff+idx);}
	T& operator*() {return *dataoff;}
	~vectore()
	{
		delete[] database;
	}
	// type conversion
	operator T* () { return dataoff; }
};

#define CODE_LEFT 1
#define CODE_RIGHT 2
#define CODE_BOT 4
#define CODE_TOP 8
#define CODE_NEAR 16
#define CODE_FAR 32

#define MAXTEX 4
struct mater2 {
	S32 msflags;		// software flags
//	float mtrans;			// 0 xpar, 1 opaque
// texture info
	textureb* thetexarr[MAXTEX];
//	uv texvel;
	string name;
// flat info
	struct pointf3 color,colorb; // color.w is mtrans
// for specular
	float specpow,specstrength;
	float refl;
};

struct group2 {
	S32 vertidx; // used for enviornment maps
	S32 nvert;
	S32 faceidx;
	S32 nface;
};

enum clipresult {CLIP_OUT,CLIP_IN,CLIP_CLIP};
//clipresult software_drawmodel(class tree2* t,struct viewport2* v,mat4* o2c);
class modelb {
public:
	string name;
//    S32 nverts;
//    S32 nfaces;
//	S32 drawpri; // 0 to 4
protected:
// more data to be defined in derived classes
	bool updatefaces;
public:
	vector<group2> groups;
	vector<mater2> mats;
	pointf3 boxmin;
	pointf3 boxmax;
	float boxrad;	// radius of sphere that covers bbox
private:
	modelb();
	modelb(modelb&);
	modelb& operator=(modelb&);
public:
	modelb(string namea);
	virtual ~modelb();
// model data, software copy
	vectore<pointf3> verts;
	vectore<pointf3> cverts;
	vectore<uv> uvs0;
	vectore<uv> uvs1;
	vector<pointf3> norms;
	vector<pointf3> tangents;
	vector <pointf3> binorms;
	vector<face> faces;
	vector<pointf3> verts2; // for edges


// for refcount
public:
//	string name; // every instance class should/could have a name, already there..
	static referencecount<modelb> rc; // the static list, add this
	S32 refidx;	// index into static vector list of this type, and this
// done refcount
// shows info about this instance when showref is called, and add this
	virtual void showline();
// normal addmat
	void addmat(const C8* matname,S32 flags,textureb* tex,const pointf3* colora,float specpow,S32 nface,S32 nvert,float refla=1,float specstr=1);
// 2 textures
	void addmat2t(const C8* matname,S32 flags,textureb* tex,textureb* tex2,const pointf3* colora,float specpow,S32 nface,S32 nvert,float refla=1);
// 2 mat colors
	void addmat2c(const C8* matname,S32 flags,textureb* tex,const pointf3* colora,const pointf3* colorb,float specpow,S32 nface,S32 nvert);
// like normal addmat, but pass a texture name instead of texture handle
	void addmatn(const C8* matname,S32 flags,const C8* texname,const pointf3* colora,float sp,S32 nface,S32 nvert);

	void copyverts(const pointf3* v,S32 nv,bool fixgroupvert = false);
	void copyverts2(const pointf3* v,S32 nv);
	void copycverts(const pointf3* v,S32 nv);
	void copynorms(const pointf3* v,S32 nv);
	void copyuvs0(const uv* v,S32 nv);
	void copyuvs1(const uv* v,S32 nv);
	void copytangents(const pointf3* v,S32 nv);
	void copybinorms(const pointf3* v,S32 nv);
	void copyfaces(const face* f,S32 nf,bool fixgroupface = false);

	void copyverts(const vector<pointf3>& v,bool fixgroupvert = false);
	void copyverts2(const vector<pointf3>& v);
	void copycverts(const vector<pointf3>& v);
	void copynorms(const vector<pointf3>& v);
	void copyuvs0(const vector<uv>& v);
	void copyuvs1(const vector<uv>& v);
	void copytangents(const vector<pointf3>& v);
	void copybinorms(const vector<pointf3>& v);
	void copyfaces(const vector<face>& f,bool fixgroupface = false);

	void calcnorms(); // call after verts and faces are set
	void calcuvs(const pointf3& offset = pointf3x(0,0,0)); // call after verts and faces are set, map xyz to longitude and latitude (spherical coords), also try to handle branch cut (prime meridian)

	const pointf3* getverts(U32* nv)
	{
		*nv=verts.size();
		return &verts[0];
	}
	const pointf3* getnorms(U32 *nn) const
	{
		*nn=norms.size();
		return &norms[0];
	}
	const pointf3* gettangents(U32 *nt) const
	{
		*nt=tangents.size();
		return &tangents[0];
	}
	const pointf3* getbinorms(U32 *nbn) const
	{
		*nbn=binorms.size();
		return &binorms[0];
	}
	const uv* getuvs0(U32 *nt0)
	{
		*nt0=uvs0.size();
		return &uvs0[0];
	}
	const uv* getuvs1(U32 *nt1)
	{
		*nt1=uvs1.size();
		return &uvs1[0];
	}
	const face* getfaces(U32 *nf) const
	{
		*nf=faces.size();
		return &faces[0];
	}

	void addface(S32 idx0,S32 idx1,S32 idx2,bool dsided = false);
	void addfaces(const face* f,S32 nfaces,S32 vertoffset,bool flip=false,bool dsided = false);
	void addfaces(const vector<face>,S32 vertoffset,bool flip=false,bool dsided = false);
	void genindifaces(bool ds = 0); // generate faces for indi vertex model
	virtual void close()=0;
	virtual void update()=0;
//	friend void video_drawtree(viewport2* vp);
//	friend clipresult checkbbox(model2* m,mat4* o2c);
//	friend clipresult checkandor(model2* m);
//	friend clipresult software_drawmodel(tree2* t,viewport2* v,mat4* o2c);
//	friend clipresult hardware_drawmodel(tree2* t,viewport2* v,mat4* o2c);
//	friend void dolightsdestrec(class model2* t);
//	friend class tree2;
//	void dolightsdestrec(mat4* o2w);
	virtual void draw(mat4* o2w,mat4* o2c,pointf3* tc,float dco,textureb* tt,pointf2* tto,clipresult bboxchk)=0;
//	S32 getdrawpri() {return drawpri;}
//	clipresult checkbbox(mat4* o2c);
	clipresult checkandor();
	void setbbox();
	clipresult checkbbox(mat4* o2c); // friends with model
	void changetex(textureb* tx,U32 matn=0,U32 texn=0);
};
