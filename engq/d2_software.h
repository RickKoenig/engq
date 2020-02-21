class model2 : public modelb {
private:
//	clipresult checkbbox2(mat4* o2c_mod);
	void dolightsdestrec(mat4* o2w);
	clipresult checkandor();
protected:
private:
	model2();
	model2(model2&);
	model2 operator=(model2&);
public:
	model2(string namea) : modelb(namea) {}
public:
	void showline() // shows info about this instance when showref is called, and add this
	{
		logger("model2 ");
		modelb::showline();
	}
//	void addmat(const C8*,S32 flags,float trans,textureb* texname,pointf3* colora,S32 nface,S32 nvert);

/*	void copyverts(const pointf3* v,S32 nv);
	void copycverts(const pointf3* v,S32 nv);
	void copynorms(const pointf3* v,S32 nv);
	void copyuvs(const uv* v,S32 nv);
	void copytangents(const pointf3* tangents,S32 ntangents);
	void copybinorms(const pointf3* binorms,S32 nbinorms);

	void copyverts(const vector<pointf3>& v);
	void copycverts(const vector<pointf3>& v);
	void copynorms(const vector<pointf3>& v);
	void copyuvs(const vector<uv>& v);
	void copytangents(const vector<pointf3>& v);
	void copybinorms(const vector<pointf3>& v);

	void addface(S32 idx0,S32 idx1,S32 idx2);
	void addfaces(face* f,S32 nfaces,S32 vertoffset,bool flip=false); */
	void close();
	void update();
	void draw(mat4* o2w,mat4* o2c,pointf3* tc,float dco,textureb* tt,pointf2* tto,clipresult bboxchk);
};
class modelb* software_createmodel(const C8* name);

/// software scene prep and close
void software_beginscene();	// clear zbuf etc.
void software_setviewport(viewport2* vp);
//void software_drawscene(tree2* scene);
void software_endscene(bool dotextdump); // nothing right now

void software_init();
void software_uninit();

void software_rendertargetmainfunc();
bool software_rendertargetshadowmapfunc();

//void software_reload_mat2shader(const C8* fname);

class texture2 : public textureb {
protected:
	bitmap32* texdata;
	S32 logu,logv;
private:
	texture2();
	texture2(texture2&);
	texture2 operator=(texture2&);
public:
	texture2(string namea) : textureb(namea),texdata(0),logu(-1),logv(-1) {}
	void addbitmap(const bitmap32* b,bool iscube); // copy bitmap b into texture
	void load(); // copy bitmap b into texture
	~texture2();
	struct bitmap32* locktexture();
	void unlocktexture();
	void showline() // shows info about this instance when showref is called, and add this
	{
	    textureb::showline();
		logger("[texture2] sx %4d, sy %4d, orig wid hit %d %d ",1<<logu,1<<logv,origsize.x,origsize.y);
//		modelb::showline();
	}
	friend void registermat(struct mater2 *m);	// material to draw tris with (required)
};
class textureb* software_createtexture(const C8* name);

