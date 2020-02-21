//chunktypes, just add to this, don't change values..
enum chunktype {
// keyword section
	KID_I8, // chars
	KID_U16o,
	KID_I32, // object id's (maybe), bools
	KID_S8o,
	KID_S16o,
	KID_S32o,  // signed ints
	KID_VEC2, // 2 floats (for uvs, maybe)
	KID_VEC3, // 3 floats (for unpadded 3d points)
	KID_VEC4, // 4 floats (for quats or rotaxis)
	KID_CHUNK,
	KID_ENDCHUNK,
	KID_ARR, // arrays
	KID_IDX2,
	KID_IDX3,
	KID_IDX3M, // for faces
	KID_FLOAT,
	KID_VEC3M, // 3 floats and 1 int (for mat idx)
// keyframe types
	KID_POS_BEZ, // obsolete
	KID_POS_TCB,
	KID_POS_LIN,
//	KID_ROT_EULER_X,
//	KID_ROT_EULER_Y,
//	KID_ROT_EULER_Z,
	KID_ROT_BEZ,
	KID_ROT_TCB,
	KID_ROT_LIN,
	KID_SCL_BEZ, // obsolete
	KID_SCL_TCB,
	KID_SCL_LIN,
// new 5-7-05
	KID_FLOAT_BEZv2, // used in euler xyz controller (sp.)
	KID_POS_BEZv2, // now includes the intan and outtan lengths, good choice, given only a max6 sdk
	KID_ROT_BEZv2o, // not much support, should use euler,
					// bez and quats don't mix, don't use
	KID_SCL_BEZv2, // now includes the intan and outtan lengths, good choice, given only a max6 sdk
// new 5-20-09
	KID_FLOAT_TCB,
	KID_FLOAT_LIN,
};

// supported types
typedef char I8;
typedef int I32;
struct VC2 {float x,y; } ;
 struct VC3{float x,y,z; } ;
// struct VC4{float x,y,z,w; } ;
 typedef struct pointf3 VC4;
 struct IDX3{int idx[3]; } ;
 struct IDX3M{int idx[3],matidx; } ;
 struct VC3M{float x,y,z; int matidx; } ;
// keyframe types
//typedef struct {float time; VC3 value; VC3 intan,outtan; int flags; } POS_BEZ;
 struct POS_BEZv2{float time; VC3 value,intan,outtan,inlength,outlength; int flags; } ;
 struct POS_TCB{float time; VC3 value; float tens,cont,bias,easein,easeout; } ;
 struct POS_LIN{float time; VC3 value; } ;

//typedef struct {float time; float value; float tens,cont,bias,easein,easeout; } ROT_EULER_X;
//typedef struct {float time; float value; float tens,cont,bias,easein,easeout; } ROT_EULER_Y;
//typedef struct {float time; float value; float tens,cont,bias,easein,easeout; } ROT_EULER_Z;
 struct FLOAT_BEZv2{float time,value,intan,outtan,inlength,outlength; int flags; } ;
 struct FLOAT_TCB{float time,value,tens,cont,bias,easein,easeout; } ;
 struct FLOAT_LIN{float time,value;} ;

 struct ROT_BEZ{float time; VC4 value; } ;
 struct ROT_TCB{float time; VC4 value; float tens,cont,bias,easein,easeout; } ;
 struct ROT_LIN{float time; VC4 value; } ;

//typedef struct {float time; VC3 value; VC3 intan,outtan; int flags;} SCL_BEZ;
 struct SCL_BEZv2{float time; VC3 value,intan,outtan,inlength,outlength; int flags;} ;
 struct SCL_TCB{float time; VC3 value; float tens,cont,bias,easein,easeout; } ;
 struct SCL_LIN{float time; VC3 value; } ;

 struct MATRIX34 {
	VC3 rows[4];
	const MATRIX34 operator*(const MATRIX34& rhs) const;
	const MATRIX34 inverse() const;
};

enum chunkname {
// user section
	UID_NONE, // for I'm in 'no chunk'
// chunks
	UID_OBJECT,
	UID_MATERIAL,
// misc
	UID_VERSION,
	UID_COMMENT,
	UID_NAME,
// materials
	UID_DTEX,
	UID_ATEX,
// objects
	UID_ID,
	UID_PID,
	UID_POS,
	UID_ROTo,
	UID_SCALE,
	UID_FL,
	UID_VL,
	UID_VN,  // vertex normals
	UID_TV, // texture uvs obsolete switch to tv0 to tv15
	UID_TFo, // will be obsolete
	UID_ROT_ROTAXIS, // change to quat later
	UID_ROT_QUAT,
	UID_ROT_EULER,
	UID_TV0, // use these soon, texture layers, 16 should be enough
	UID_TV1, // I want them to be contiguous
	UID_TV2,
	UID_TV3,
	UID_TV4,
	UID_TV5,
	UID_TV6,
	UID_TV7,
	UID_TV8,
	UID_TV9,
	UID_TV10,
	UID_TV11,
	UID_TV12,
	UID_TV13,
	UID_TV14,
	UID_TV15,
	UID_FN,	// will be obsolete 3 normals per face.. go for VN later
	UID_FS, // for groups
	UID_FO,
	UID_VS,
	UID_VO,
	UID_USERPROP,
	UID_MATRIX,
	UID_KIND, // GEOM,HELPER,BONE etc. look in objects.h
	UID_TARGET,
// camera
	UID_CAMERA_FOV,
// light
	UID_LIGHT_COLOR,
	UID_LIGHT_INTENSITY,
	UID_LIGHT_HOTSIZE,
	UID_LIGHT_FALLSIZE,
	UID_LIGHT_USE_NEAR_ATTEN, 
	UID_LIGHT_NEAR_ATTEN_START, 
	UID_LIGHT_NEAR_ATTEN_END,
	UID_LIGHT_USE_ATTEN,
	UID_LIGHT_ATTEN_START,
	UID_LIGHT_ATTEN_END,
	UID_LIGHT_SHADOW,
	UID_LIGHT_OVERSHOOT,
// keyframes
	UID_KEYFRAMEo, // obsolete
	UID_TRACKFLAGS,
	UID_POS_BEZ, // now uses KID_SCL_BEZv2 5-7-05, pick this one
	UID_POS_TCB,
	UID_POS_LIN,
	UID_ROT_BEZ,
	UID_ROT_TCB,
	UID_ROT_LIN,
	UID_SCL_BEZ, // now uses KID_SCL_BEZv2 5-7-05, pick this one
	UID_SCL_TCB,
	UID_SCL_LIN,
// new 5-7-05
	UID_ROT_EULER_X, // these three use KID_FLOAT_BEZv2, pick this one, go bez all the way..
	UID_ROT_EULER_Y,
	UID_ROT_EULER_Z,
// new 12-2-05
	UID_DIFFUSE,
// new 12-5-05
	UID_POS_SAMP, // uses KID of KID_ROT/POS/SCL_LIN
	UID_ROT_SAMP,
	UID_SCL_SAMP, 
// new 12-7-05
	UID_WEIGHTS1, // non blended verts
// new 12-19-05
	UID_WEIGHTS2, // blended verts, bone index
	UID_WEIGHTS2F, // blended verts, weight amount
// new 12-23-05
	UID_AMBIENT,
	UID_OPACITY, // a float
	UID_SPECULAR,
	UID_SHINE,
	UID_EMIT,
// new 1-3-6
	UID_TILING, // int with flags, 1 uwrap, 2 vwrap, etc... 
// new 12-29-8
	UID_VC,  // vertex colors
// new 5-20-09
	UID_VIS_SAMP,  // visibility track
// new 6-16-09
	UID_REFL_AMT,	// reflection amount
	UID_RTEX,		// reflection texture (NYI)
};

class chunker {
	FILE *cf;
	vector<int> chunksizefileoffsetstack; // place in file to rewind for writing chunklens
public:
	chunker(const char* const name);
	~chunker()
	{
		if (cf)
			fclose(cf);
	}
	bool status()
	{
		return cf!=0;
	}
// non array versions
	bool write(chunkname userid,I32 val); // ids
	bool write(chunkname userid,float val); // ids
	bool write(chunkname userid,const char* const str); // names
	bool write(chunkname userid,const VC3& val); // pos,scale
	bool write(chunkname userid,const VC4& val); // rotaxis, quats
	bool write(chunkname userid,const MATRIX34& val); // matrix
// keyframes
//	bool write(chunkname userid,const POS_BEZ& val); 
	bool write(chunkname userid,const POS_BEZv2& val); 
	bool write(chunkname userid,const POS_TCB& val); 
	bool write(chunkname userid,const POS_LIN& val); 
//	bool write(chunkname userid,const ROT_EULER_X& val);
//	bool write(chunkname userid,const ROT_EULER_Y& val);
//	bool write(chunkname userid,const ROT_EULER_Z& val);
	bool write(chunkname userid,const ROT_BEZ& val); 
	bool write(chunkname userid,const ROT_TCB& val); 
	bool write(chunkname userid,const ROT_LIN& val); 
//	bool write(chunkname userid,const SCL_BEZ& val); 
	bool write(chunkname userid,const SCL_BEZv2& val); 
	bool write(chunkname userid,const SCL_TCB& val); 
	bool write(chunkname userid,const SCL_LIN& val); 
	bool write(chunkname userid,const FLOAT_BEZv2& val); 
	bool write(chunkname userid,const FLOAT_TCB& val); 
	bool write(chunkname userid,const FLOAT_LIN& val); 
// array versions
	bool writearrstart(chunkname userid,unsigned int numele,chunktype arrtype);
	bool writearr(const VC2& val); // for uvs
	bool writearr(const VC3& val); // for uvs
	bool writearr(const IDX3M& val); // for faces, 4 ints
	bool writearr(const IDX3& val); // for tfaces, 3 ints, will be obsolete
	bool writearr(const I32& val); // for bone influence, 1 32bit int
	bool writearr(const float& val); // for bone influence, 1 32bit int
	bool writearrend();

	bool startchunk(chunkname userid);
	bool endchunk(chunkname userid);
};

class unchunker {
	struct typeinfo {
		int bytesize;
		const char* const namestr;
	};
	static const typeinfo chunktype_infos[];
	static const char* const chunkname_strs[];
	FILE *cf;
	int chunksize;
	int lastnumele,lastdatasize;
public:
	static const char* const getchunktype_strs(chunktype idx);
	static const char* const getchunkname_strs(chunkname idx);
	static int getchunktype_bytesize(chunktype idx);
	unchunker(const char* const name);
	~unchunker()
	{
		if (cf)
			fclose(cf);
	}
	bool status()
	{
		return cf!=0;
	}
	bool getchunkheader(chunkname& cn,chunktype& ct,int& numele,int& elesize,int& datasize); // numele!=0 is an array
// non array versions
	void readI32(I32& u32);
	void readFLOAT(float& f32);
	void readVC2(VC2& vc2);
	void readVC3(VC3& vc3);
	void readVC4(VC4& vc4);
	void readIDX3(IDX3& idx3);
	void readIDX3M(IDX3M& idx3m);
	void readVC3M(VC3M& vc3m);
// keyframes
//	void readPOS_BEZ(POS_BEZ& pb);
	void readPOS_BEZv2(POS_BEZv2& pb);
	void readPOS_LIN(POS_LIN& pl);
	void readPOS_TCB(POS_TCB& pt);
	void readROT_BEZ(ROT_BEZ& rb);
	void readROT_LIN(ROT_LIN& rl);
	void readROT_TCB(ROT_TCB& rt);
//	void readSCL_BEZ(SCL_BEZ& sb);
	void readSCL_BEZv2(SCL_BEZv2& sb);
	void readSCL_LIN(SCL_LIN& sl);
	void readSCL_TCB(SCL_TCB& st);
	void readFLOAT_BEZv2(FLOAT_BEZv2& sb);
// new stuff
	void readFLOAT_LIN(FLOAT_LIN& sl);
	void readFLOAT_TCB(FLOAT_TCB& st);

// vector versions
	string readI8v();
	vector<VC2> readVC2v();
	vector<VC3> readVC3v();
	vector<IDX3> readIDX3v();
	vector<IDX3M> readIDX3Mv();
	vector<VC3M> readVC3Mv();
	vector<I32> readI32v();
// array version, 4 VC3's
	MATRIX34 readMATRIX34();
// skip unwanted data
	void skipdata();
};

// print contents of any chunkfile to the logfile
void unchunktest(const char* fname,int chunktestdepth);
