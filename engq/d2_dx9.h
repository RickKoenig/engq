void dx9_beginscene();	// clear zbuf etc.
void dx9_setviewport(viewport2* vp);	// clear zbuf etc.
//void dx9_drawscene(tree2* scene);
void dx9_endscene(bool dotextdump); // nothing right now
void dx9_rendertargetmainfunc();
bool dx9_rendertargetshadowmapfunc();

//void dx9_reload_mat2shader(const C8* fname);

void dx9_init();
void dx9_uninit();
//#define SHADOWMAP_CLAMP_BORDER 0

class model3;
typedef void (model3::*cvp)();

class model3 : public modelb {
	struct vdecl {
		const C8* name;
		D3DVERTEXELEMENT9* ve;
		S32 sizeofvertex;
		cvp cfunp;
		IDirect3DVertexDeclaration9* vd; // filled in by createshaders
	};
	static struct vdecl vdecls[];
	static const S32 nvdecls;
//	clipresult checkbbox3(mat4* o2c_mod);
	static S32 findshaderidx(const C8* name);
public:
	static const C8* getnextshadername(const C8* oldname);
private:
	void dolightsdestrec(mat4* o2w);
protected:
/*	vector<face> tempfaces; /// this stuff is for biuld a model, free upon a call to model3::close
	vector<pointf3> tempverts;
	vector<pointf3> tempnorms;
	vector<pointf3> tempcverts;
	vector<pointf3> temptangents;
	vector <pointf3> tempbinorms;
	vector<uv> tempuvs; */
	LPDIRECT3DVERTEXBUFFER9 vb;
	LPDIRECT3DINDEXBUFFER9 ib;
//	S32 vsi;	/// vertex shader index
//	S32 psi;	/// pixel shader index
	struct dx9_mater2 {
		S32 shaderidx;
//		S32 dum;
	};
	vector<dx9_mater2> dx9mater2;
	S32 vertdeclidx;
	S32 vertbytesize; // current vertex buffer size
	S32 hardvertbytesize; // hardware vertex buffer size (max)
	S32 facebytesize; // current face buffer size
	S32 hardfacebytesize; // hardware face buffer size (max)
	void close_VCT();
	void close_VT();
	void close_VT2();
	void close_VN();
	void close_VNT();
	void close_VNT2();
	void close_VNTM();
	void close_VE();
	void close_ib();
public:
	static void createshaders();
	static void destroyshaders();
	model3(string namea) : modelb(namea),vb(0),ib(0),vertdeclidx(0),hardfacebytesize(0),facebytesize(0) {}
	~model3();
	void showline() // shows info about this instance when showref is called, and add this
	{
		logger("model3 ");
		modelb::showline();
	} 
	void close();
	void update();
	void draw(mat4* o2w,mat4* o2c,pointf3* tc,float dco,textureb* tt,pointf2* tto,clipresult bboxchk);
};

class texture3 : public textureb {
	LPDIRECT3DBASETEXTURE9 texdata;
	S32 alphabits;
	S32 levels;
//	pointi2 size;
	const C8* print4cc(U32 format);
	bool isshadowmap;
	bool iscubemap;
	static D3DCUBEMAP_FACES curcubeface;
public:
	static void setcurcubeface(D3DCUBEMAP_FACES ccf)
	{
		curcubeface=ccf;
	}
	texture3(string namea) : textureb(namea),texdata(0),alphabits(0),levels(0),isshadowmap(0),iscubemap(0)
	{
//		size.x=0;
//		size.y=0;
	}
	void showline() // shows info about this instance when showref is called, and add this
	{
	    textureb::showline();
		logger("[texture3] width %4d, height %4d, orig wid hit %d %d,4cc '%s' miplevels %d, alphabits %d, isshadowmap %d",
			tsize.x,tsize.y,origsize.x,origsize.y,print4cc(texformat),levels,alphabits,isshadowmap);
	}
	void load();
	bitmap32* locktexture();
	void unlocktexture();
	void addbitmap(const bitmap32* b,bool iscube);
	~texture3();
	LPDIRECT3DBASETEXTURE9 gettexhandle() {return texdata;}
	friend void model3::draw(mat4* o2w,mat4* o2c,pointf3* color,float dco,textureb* tt,pointf2* tto,clipresult bbchk);
	friend class model3b;
};

modelb* dx9_createmodel(const C8* name);
textureb* dx9_createtexture(const C8* name);
void release_d3dpool_default();
void reset_d3dpool_default();
