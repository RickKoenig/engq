#define D2_3D
#include <m_eng.h>

const unchunker::typeinfo unchunker::chunktype_infos[]={
	 {1,"I8"},
	 {2,"U16o"},
	 {4,"I32"},
	 {1,"S8o"},
	 {2,"S16o"},
	 {4,"S32o"},
	 {8,"VEC2"},
	{12,"VEC3"},
	{16,"VEC4"},
	 {0,"CHUNK"},
	 {0,"ENDCHUNK"},
	 {0,"ARR"},
	 {8,"IDX2"},
	{12,"IDX3"},
	{16,"IDX3M"},
	 {4,"FLOAT"},
	{16,"VEC3M"}, // 3 floats and 1 int (for mat idx)
// keyframe types
	{44,"POS_BEZ"},
	{36,"POS_TCB"},
	{16,"POS_LIN"},
//	{28,"ROT_EULER_X"},
//	{28,"ROT_EULER_Y"},
//	{28,"ROT_EULER_Z"},
	{20,"ROT_BEZ"},
	{40,"ROT_TCB"},
	{20,"ROT_LIN"},
	{44,"SCL_BEZ"},
	{36,"SCL_TCB"},
	{16,"SCL_LIN"},

	{28,"FLOAT_BEZv2"},
	{68,"POS_BEZv2"},
	 {0,"ROT_BEZv2o"}, // don't use, use rot bez instead
	{68,"SCL_BEZv2"},
// new 5-20-09
	{28,"FLOAT_TCB"},
	 {8,"FLOAT_LINEAR"},
};

const C8* const unchunker::chunkname_strs[]={
// user section
	"NONE",
// chunks
	"OBJECT",
	"MATERIAL",
// misc
	"VERSION",
	"COMMENT",
	"NAME",
// materials
	"DTEX",
	"ATEX",
// objects
	"ID",
	"PID",
	"POS",
	"ROTo",
	"SCALE",
	"FL",
	"VL",
	"VN",
	"TV",
	"TFo", // will be obsolete
	"ROT_ROTAXIS",
	"ROT_QUAT",
	"ROT_EULER",
	"TV0", // use these soon, texture layers, 16 should be enough
	"TV1", // I want them to be contiguous
	"TV2",
	"TV3",
	"TV4",
	"TV5",
	"TV6",
	"TV7",
	"TV8",
	"TV9",
	"TV10",
	"TV11",
	"TV12",
	"TV13",
	"TV14",
	"TV15",
	"FN",	// will be obsolete 3 normals per face.. go for VN later
	"FS", // for groups
	"FO",
	"VS",
	"VO",
	"USERPROP",
	"MATRIX",
	"KIND",
	"TARGET",
// camera
	"CAMERA_FOV",
// light
	"LIGHT_COLOR",
	"LIGHT_INTENSITY",
	"LIGHT_HOTSIZE",
	"LIGHT_FALLSIZE",
	"LIGHT_USE_NEAR_ATTEN",
	"LIGHT_NEAR_ATTEN_START",
	"LIGHT_NEAR_ATTEN_END",
	"LIGHT_USE_ATTEN",
	"LIGHT_ATTEN_START",
	"LIGHT_ATTEN_END",
	"LIGHT_SHADOW",
	"LIGHT_OVERSHOOT",
// keyframes
	"KEYFRAMEo",
	"TRACKFLAGS",
	"POS_BEZ",
	"POS_TCB",
	"POS_LIN",
	"ROT_BEZ",
	"ROT_TCB",
	"ROT_LIN",
	"SCL_BEZ",
	"SCL_TCB",
	"SCL_LIN",
// new 5-7-05
	"ROT_EULER_X",
	"ROT_EULER_Y",
	"ROT_EULER_Z",
// new 12-2-05
	"DIFFUSE",
// new 12-5-05
	"POS_SAMP", // uses KID of KID_ROT/POS/SCL_LIN
	"ROT_SAMP",
	"SCALE_SAMP",
// new 12-7-05
	"WEIGHTS1",
// new 12-19-05
	"WEIGHTS2",
	"WEIGHTS2F",
// new 12-23-05
	"AMBIENT",
	"OPACITY", // a float
	"SPECUALAR",
	"SHINE",
	"EMIT",
// new 1-3-6
	"TILING",
// new 12-29-8
	"CVERT",
// new 5-20-09
	"VIS_SAMP",
// new 6-16-09
	"REFL_AMT", // reflection amount
	"RTEX",		// reflection texture
};

const C8* const unchunker::getchunktype_strs(chunktype idx)
{
	if (idx<0 || idx>=(S32)(sizeof(chunktype_infos)/sizeof(chunktype_infos[0])))
		errorexit("bad chunktype %d",idx);
	return chunktype_infos[idx].namestr;
}

const C8* const unchunker::getchunkname_strs(chunkname idx)
{
	static const C8 unk[]="?";
	if (idx<=0 || idx>=(S32)(sizeof(chunkname_strs)/sizeof(chunkname_strs[0])))
		return unk;
	return chunkname_strs[idx];
}

int unchunker::getchunktype_bytesize(chunktype idx)
{
	if (idx<0 || idx>=(S32)(sizeof(chunktype_infos)/sizeof(chunktype_infos[0])))
		errorexit("bad chunktype %d",idx);
//	logger("idx = %d\n",idx);
	return chunktype_infos[idx].bytesize;
}

// open up a file for write
chunker::chunker(const C8* const name) :
	/*curchunk(UID_NONE),*/cf(0)//,inarray(false)//,arraychunk(UID_NONE)
{
	cf=fopen2(name,"wb");
}

// output some simple chunks
bool chunker::write(chunkname userid,I32 val)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_I32;
	fwrite(&t,1,4,cf);

	fwrite(&val,1,4,cf);

	return true;
}

// output some simple chunks
bool chunker::write(chunkname userid,float val)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_FLOAT;
	fwrite(&t,1,4,cf);

	fwrite(&val,1,4,cf);

	return true;
}

bool chunker::write(chunkname userid,const C8* const str)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_ARR;
	fwrite(&t,1,4,cf);

	unsigned int len=static_cast<int>(strlen(str)+1);
	fwrite(&len,1,4,cf);

	t=KID_I8;
	fwrite(&t,1,4,cf);

	fwrite(str,1,len,cf);  // includes the '\0'
	len&=3; // pad
	t=0;
	if (len) {
		len=4-len;
		while(len--)
			fwrite(&t,1,1,cf);
	}
	return true;
}

bool chunker::write(chunkname userid,const VC3& val)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_VEC3;
	fwrite(&t,1,4,cf);

	fwrite(&val.x,1,4,cf);
	fwrite(&val.y,1,4,cf);
	fwrite(&val.z,1,4,cf);

	return true;
}

bool chunker::write(chunkname userid,const VC4& val)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_VEC4;
	fwrite(&t,1,4,cf);

	fwrite(&val.x,1,4,cf);
	fwrite(&val.y,1,4,cf);
	fwrite(&val.z,1,4,cf);
	fwrite(&val.w,1,4,cf);

	return true;
}

bool chunker::write(chunkname userid,const MATRIX34& m)
{
	writearrstart(userid,4,KID_VEC3);
	for (int i=0;i<4;i++)
		writearr(m.rows[i]);
	writearrend();
	return true;
}

// write out the various types of keyframes
/*bool chunker::write(chunkname userid,const POS_BEZ& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_POS_BEZ;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.intan.x,1,4,cf);
	fwrite(&val.intan.y,1,4,cf);
	fwrite(&val.intan.z,1,4,cf);
	fwrite(&val.outtan.x,1,4,cf);
	fwrite(&val.outtan.y,1,4,cf);
	fwrite(&val.outtan.z,1,4,cf);
	fwrite(&val.flags,1,4,cf);
	return true;
}
*/
bool chunker::write(chunkname userid,const POS_BEZv2& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_POS_BEZv2;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.intan.x,1,4,cf);
	fwrite(&val.intan.y,1,4,cf);
	fwrite(&val.intan.z,1,4,cf);
	fwrite(&val.outtan.x,1,4,cf);
	fwrite(&val.outtan.y,1,4,cf);
	fwrite(&val.outtan.z,1,4,cf);
	fwrite(&val.inlength.x,1,4,cf);
	fwrite(&val.inlength.y,1,4,cf);
	fwrite(&val.inlength.z,1,4,cf);
	fwrite(&val.outlength.x,1,4,cf);
	fwrite(&val.outlength.y,1,4,cf);
	fwrite(&val.outlength.z,1,4,cf);
	fwrite(&val.flags,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const FLOAT_BEZv2& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_FLOAT_BEZv2;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value,1,4,cf);
	fwrite(&val.intan,1,4,cf);
	fwrite(&val.outtan,1,4,cf);
	fwrite(&val.inlength,1,4,cf);
	fwrite(&val.outlength,1,4,cf);
	fwrite(&val.flags,1,4,cf);
	return true;
}


bool chunker::write(chunkname userid,const POS_TCB& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_POS_TCB;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const POS_LIN& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_POS_LIN;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	return true;
}

/*
bool chunker::write(chunkname userid,const ROT_EULER_X& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_ROT_EULER_X;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const ROT_EULER_Y& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_ROT_EULER_Y;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const ROT_EULER_Z& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_ROT_EULER_Z;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}
*/
bool chunker::write(chunkname userid,const ROT_BEZ& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_ROT_BEZ;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.value.w,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const ROT_TCB& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_ROT_TCB;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.value.w,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const ROT_LIN& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_ROT_LIN;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.value.w,1,4,cf);
	return true;
}

/*bool chunker::write(chunkname userid,const SCL_BEZ& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_SCL_BEZ;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.intan.x,1,4,cf);
	fwrite(&val.intan.y,1,4,cf);
	fwrite(&val.intan.z,1,4,cf);
	fwrite(&val.outtan.x,1,4,cf);
	fwrite(&val.outtan.y,1,4,cf);
	fwrite(&val.outtan.z,1,4,cf);
	fwrite(&val.flags,1,4,cf);
	return true;
}
*/

bool chunker::write(chunkname userid,const SCL_BEZv2& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_SCL_BEZv2;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.intan.x,1,4,cf);
	fwrite(&val.intan.y,1,4,cf);
	fwrite(&val.intan.z,1,4,cf);
	fwrite(&val.outtan.x,1,4,cf);
	fwrite(&val.outtan.y,1,4,cf);
	fwrite(&val.outtan.z,1,4,cf);
	fwrite(&val.inlength.x,1,4,cf);
	fwrite(&val.inlength.y,1,4,cf);
	fwrite(&val.inlength.z,1,4,cf);
	fwrite(&val.outlength.x,1,4,cf);
	fwrite(&val.outlength.y,1,4,cf);
	fwrite(&val.outlength.z,1,4,cf);
	fwrite(&val.flags,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const SCL_TCB& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_SCL_TCB;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const SCL_LIN& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_SCL_LIN;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value.x,1,4,cf);
	fwrite(&val.value.y,1,4,cf);
	fwrite(&val.value.z,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const FLOAT_TCB& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_FLOAT_TCB;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value,1,4,cf);
	fwrite(&val.tens,1,4,cf);
	fwrite(&val.cont,1,4,cf);
	fwrite(&val.bias,1,4,cf);
	fwrite(&val.easein,1,4,cf);
	fwrite(&val.easeout,1,4,cf);
	return true;
}

bool chunker::write(chunkname userid,const FLOAT_LIN& val)
{
	fwrite(&userid,1,4,cf);
	unsigned int t=KID_SCL_LIN;
	fwrite(&t,1,4,cf);

	fwrite(&val.time,1,4,cf);
	fwrite(&val.value,1,4,cf);
	return true;
}


bool chunker::writearrstart(chunkname userid,unsigned int numele,chunktype datatype)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_ARR;
	fwrite(&t,1,4,cf);

	fwrite(&numele,1,4,cf);

	fwrite(&datatype,1,4,cf);

	return true;
}

bool chunker::writearrend()
{
	return true;
}

bool chunker::writearr(const VC2& val)
{
	fwrite(&val.x,1,4,cf);
	fwrite(&val.y,1,4,cf);
	return true;
}

bool chunker::writearr(const VC3& val)
{
	fwrite(&val.x,1,4,cf);
	fwrite(&val.y,1,4,cf);
	fwrite(&val.z,1,4,cf);
	return true;
}

bool chunker::writearr(const IDX3M& val)
{
	fwrite(&val.idx[0],1,4,cf);
	fwrite(&val.idx[1],1,4,cf);
	fwrite(&val.idx[2],1,4,cf);
	fwrite(&val.matidx,1,4,cf);
	return true;
}

bool chunker::writearr(const IDX3& val)
{
	fwrite(&val.idx[0],1,4,cf);
	fwrite(&val.idx[1],1,4,cf);
	fwrite(&val.idx[2],1,4,cf);
	return true;
}

bool chunker::writearr(const I32& val)
{
	fwrite(&val,1,4,cf);
	return true;
}

bool chunker::writearr(const float& val)
{
	fwrite(&val,1,4,cf);
	return true;
}

bool chunker::startchunk(chunkname userid)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_CHUNK;
	fwrite(&t,1,4,cf);

	chunksizefileoffsetstack.push_back(ftell(cf));
	fseek(cf,4,SEEK_CUR);

	return true;
}

bool chunker::endchunk(chunkname userid)
{
	fwrite(&userid,1,4,cf);

	unsigned int t=KID_ENDCHUNK;
	fwrite(&t,1,4,cf);

	int csfo=chunksizefileoffsetstack.back();
	chunksizefileoffsetstack.pop_back();
	int chunksize=ftell(cf)-csfo-4;
	int fppossave=ftell(cf);
	fseek(cf,csfo,SEEK_SET);
	fwrite(&chunksize,1,4,cf);
	fseek(cf,fppossave,SEEK_SET);

	fppossave=0;

	return true;
}

unchunker::unchunker(const C8* const name) :
	cf(0),chunksize(0),lastnumele(0)
{
	cf=fopen2(name,"rb");
	if (!cf)
		errorexit("unchunker: can't open '%s'",name);
}

bool unchunker::getchunkheader(chunkname& cn,chunktype& ct,int& numele,int& elesize,int& datasize)
{
	lastnumele=numele=lastdatasize=0;
	if (!fread(&cn,1,4,cf))
		return false;
	if (!fread(&ct,1,4,cf))
		return false;
	datasize=getchunktype_bytesize(ct);
	if (!datasize) {
		int pad;
		switch(ct) {
		case KID_CHUNK:
			fread(&datasize,1,4,cf);
			break;
		case KID_ENDCHUNK:
			break;
		case KID_ARR:
			fread(&numele,1,4,cf);
			fread(&ct,1,4,cf);
			elesize=getchunktype_bytesize(ct);
			datasize=elesize*numele;
			pad=datasize&3;
			if (pad)
				datasize+=(4-pad);
			break;
		default:
			elesize=datasize;
			errorexit("unknown chunkkeyword %d",ct);
			break;
		};
	}
	lastnumele=numele;
	lastdatasize=datasize;
	return true;
}

// just pass over the data
void unchunker::skipdata()
{
	fseek(cf,lastdatasize,SEEK_CUR);
	lastdatasize=lastnumele=0;
}

// read in nonarray data
void unchunker::readI32(I32& i32)
{
	fread(&i32,1,4,cf);
}

void unchunker::readFLOAT(float& f32)
{
	fread(&f32,1,4,cf);
}

void unchunker::readVC2(VC2& vc2)
{
	fread(&vc2,1,sizeof(VC2),cf);
}

void unchunker::readVC3(VC3& vc3)
{
	fread(&vc3,1,sizeof(VC3),cf);
}

void unchunker::readVC4(VC4& vc4)
{
	fread(&vc4,1,sizeof(VC4),cf);
}

void unchunker::readIDX3(IDX3& idx3)
{
	fread(&idx3,1,sizeof(IDX3),cf);
}

void unchunker::readIDX3M(IDX3M& idx3m)
{
	fread(&idx3m,1,sizeof(IDX3M),cf);
}

void unchunker::readVC3M(VC3M& vc3m)
{
	fread(&vc3m,1,sizeof(VC3M),cf);
}

/*void unchunker::readPOS_BEZ(POS_BEZ& pb)
{
	fread(&pb.time,1,4,cf);
	fread(&pb.value.x,1,4,cf);
	fread(&pb.value.y,1,4,cf);
	fread(&pb.value.z,1,4,cf);
	fread(&pb.intan.x,1,4,cf);
	fread(&pb.intan.y,1,4,cf);
	fread(&pb.intan.z,1,4,cf);
	fread(&pb.outtan.x,1,4,cf);
	fread(&pb.outtan.y,1,4,cf);
	fread(&pb.outtan.z,1,4,cf);
	fread(&pb.flags,1,4,cf);
}
*/

void unchunker::readPOS_BEZv2(POS_BEZv2& pb)
{
	fread(&pb.time,1,4,cf);
	fread(&pb.value.x,1,4,cf);
	fread(&pb.value.y,1,4,cf);
	fread(&pb.value.z,1,4,cf);
	fread(&pb.intan.x,1,4,cf);
	fread(&pb.intan.y,1,4,cf);
	fread(&pb.intan.z,1,4,cf);
	fread(&pb.outtan.x,1,4,cf);
	fread(&pb.outtan.y,1,4,cf);
	fread(&pb.outtan.z,1,4,cf);
	fread(&pb.inlength.x,1,4,cf);
	fread(&pb.inlength.y,1,4,cf);
	fread(&pb.inlength.z,1,4,cf);
	fread(&pb.outlength.x,1,4,cf);
	fread(&pb.outlength.y,1,4,cf);
	fread(&pb.outlength.z,1,4,cf);
	fread(&pb.flags,1,4,cf);
}


void unchunker::readPOS_LIN(POS_LIN& pl)
{
	fread(&pl.time,1,4,cf);
	fread(&pl.value.x,1,4,cf);
	fread(&pl.value.y,1,4,cf);
	fread(&pl.value.z,1,4,cf);
}

void unchunker::readPOS_TCB(POS_TCB& pt)
{
	fread(&pt.time,1,4,cf);
	fread(&pt.value.x,1,4,cf);
	fread(&pt.value.y,1,4,cf);
	fread(&pt.value.z,1,4,cf);
	fread(&pt.tens,1,4,cf);
	fread(&pt.cont,1,4,cf);
	fread(&pt.bias,1,4,cf);
	fread(&pt.easein,1,4,cf);
	fread(&pt.easeout,1,4,cf);
}

void unchunker::readROT_BEZ(ROT_BEZ& rb)
{
	fread(&rb.time,1,4,cf);
	fread(&rb.value.x,1,4,cf);
	fread(&rb.value.y,1,4,cf);
	fread(&rb.value.z,1,4,cf);
	fread(&rb.value.w,1,4,cf);
}

void unchunker::readROT_LIN(ROT_LIN& rl)
{
	fread(&rl.time,1,4,cf);
	fread(&rl.value.x,1,4,cf);
	fread(&rl.value.y,1,4,cf);
	fread(&rl.value.z,1,4,cf);
	fread(&rl.value.w,1,4,cf);
}

void unchunker::readROT_TCB(ROT_TCB& rt)
{
	fread(&rt.time,1,4,cf);
	fread(&rt.value.x,1,4,cf);
	fread(&rt.value.y,1,4,cf);
	fread(&rt.value.z,1,4,cf);
	fread(&rt.value.w,1,4,cf);
	fread(&rt.tens,1,4,cf);
	fread(&rt.cont,1,4,cf);
	fread(&rt.bias,1,4,cf);
	fread(&rt.easein,1,4,cf);
	fread(&rt.easeout,1,4,cf);
}

/*void unchunker::readSCL_BEZ(SCL_BEZ& sb)
{
	fread(&sb.time,1,4,cf);
	fread(&sb.value.x,1,4,cf);
	fread(&sb.value.y,1,4,cf);
	fread(&sb.value.z,1,4,cf);
	fread(&sb.intan.x,1,4,cf);
	fread(&sb.intan.y,1,4,cf);
	fread(&sb.intan.z,1,4,cf);
	fread(&sb.outtan.x,1,4,cf);
	fread(&sb.outtan.y,1,4,cf);
	fread(&sb.outtan.z,1,4,cf);
	fread(&sb.flags,1,4,cf);
}
*/

void unchunker::readSCL_BEZv2(SCL_BEZv2& sb)
{
	fread(&sb.time,1,4,cf);
	fread(&sb.value.x,1,4,cf);
	fread(&sb.value.y,1,4,cf);
	fread(&sb.value.z,1,4,cf);
	fread(&sb.intan.x,1,4,cf);
	fread(&sb.intan.y,1,4,cf);
	fread(&sb.intan.z,1,4,cf);
	fread(&sb.outtan.x,1,4,cf);
	fread(&sb.outtan.y,1,4,cf);
	fread(&sb.outtan.z,1,4,cf);
	fread(&sb.inlength.x,1,4,cf);
	fread(&sb.inlength.y,1,4,cf);
	fread(&sb.inlength.z,1,4,cf);
	fread(&sb.outlength.x,1,4,cf);
	fread(&sb.outlength.y,1,4,cf);
	fread(&sb.outlength.z,1,4,cf);
	fread(&sb.flags,1,4,cf);
}

void unchunker::readFLOAT_BEZv2(FLOAT_BEZv2& sb)
{
	fread(&sb.time,1,4,cf);
	fread(&sb.value,1,4,cf);
	fread(&sb.intan,1,4,cf);
	fread(&sb.outtan,1,4,cf);
	fread(&sb.inlength,1,4,cf);
	fread(&sb.outlength,1,4,cf);
	fread(&sb.flags,1,4,cf);
}

void unchunker::readSCL_LIN(SCL_LIN& sl)
{
	fread(&sl.time,1,4,cf);
	fread(&sl.value.x,1,4,cf);
	fread(&sl.value.y,1,4,cf);
	fread(&sl.value.z,1,4,cf);
}

void unchunker::readSCL_TCB(SCL_TCB& st)
{
	fread(&st.time,1,4,cf);
	fread(&st.value.x,1,4,cf);
	fread(&st.value.y,1,4,cf);
	fread(&st.value.z,1,4,cf);
	fread(&st.tens,1,4,cf);
	fread(&st.cont,1,4,cf);
	fread(&st.bias,1,4,cf);
	fread(&st.easein,1,4,cf);
	fread(&st.easeout,1,4,cf);
}

void unchunker::readFLOAT_TCB(FLOAT_TCB& st)
{
	fread(&st.time,1,4,cf);
	fread(&st.value,1,4,cf);
	fread(&st.tens,1,4,cf);
	fread(&st.cont,1,4,cf);
	fread(&st.bias,1,4,cf);
	fread(&st.easein,1,4,cf);
	fread(&st.easeout,1,4,cf);
}

void unchunker::readFLOAT_LIN(FLOAT_LIN& sl)
{
	fread(&sl.time,1,4,cf);
	fread(&sl.value,1,4,cf);
}

string unchunker::readI8v()
{
	if (!lastnumele)
		errorexit("not an array");
	string s;
	C8* as = new C8[lastnumele];
	fread(as,1,lastnumele,cf);
	lastnumele&=3;
	if (lastnumele) {
		lastnumele=4-lastnumele;
		fseek(cf,lastnumele,SEEK_CUR);
	}
	lastnumele=0;
	s=as;
	delete[] as;
	return s;
}

MATRIX34 unchunker::readMATRIX34()
{
	if (lastnumele!=4)
		errorexit("not an matrix34");
	MATRIX34 m;
	for (int i=0;i<4;i++)
		readVC3(m.rows[i]);
	return m;
}

vector<VC2> unchunker::readVC2v()
{
	if (!lastnumele)
		errorexit("not an array");
	vector<VC2> v;
	for (int i=0;i<lastnumele;i++) {
		VC2 vc2;
		readVC2(vc2);
		v.push_back(vc2);
	}
	lastnumele=0;
	return v;
}

vector<VC3> unchunker::readVC3v()
{
	if (!lastnumele)
		errorexit("not an array");
	vector<VC3> v;
	for (int i=0;i<lastnumele;i++) {
		VC3 vc3;
		readVC3(vc3);
		v.push_back(vc3);
	}
	lastnumele=0;
	return v;
}

vector<IDX3> unchunker::readIDX3v()
{
	if (!lastnumele)
		errorexit("not an array");
	vector<IDX3> v;
	for (int i=0;i<lastnumele;i++) {
		IDX3 idx3;
		readIDX3(idx3);
		v.push_back(idx3);
	}
	lastnumele=0;
	return v;
}

vector<IDX3M> unchunker::readIDX3Mv()
{
	if (!lastnumele)
		errorexit("not an array");
	vector<IDX3M> v;
	for (int i=0;i<lastnumele;i++) {
		IDX3M idx3m;
		readIDX3M(idx3m);
		v.push_back(idx3m);
	}
	lastnumele=0;
	return v;
}

vector<VC3M> unchunker::readVC3Mv()
{
	if (!lastnumele)
		errorexit("not an array");
	vector<VC3M> v;
	for (int i=0;i<lastnumele;i++) {
		VC3M vc3m;
		readVC3M(vc3m);
		v.push_back(vc3m);
	}
	lastnumele=0;
	return v;
}

vector<I32> unchunker::readI32v()
{
	if (!lastnumele)
		errorexit("not an array");
	vector<I32> v;
	for (int i=0;i<lastnumele;i++) {
		I32 i32m;
		readI32(i32m);
		v.push_back(i32m);
	}
	lastnumele=0;
	return v;
}


// read any chunker file and display everything about it to the logfile
#define CHUNKTESTDEPTH 12
void unchunktest(const C8* fname,int chunktestdepth)
{
	int depth=0;
	unchunker* uc = new unchunker(fname);
	chunkname cn;
	chunktype ct;
	int datasize,numele,elesize;
	while(uc->getchunkheader(cn,ct,numele,elesize,datasize)) {
		string indentstr;
		for (int i=0;i<depth;++i)
			indentstr+="    ";
		if (depth<0)
			indentstr="???";
		if (!numele)
			logger("%schunk: name '%s', type '%s', datasize %d, ",
				indentstr.c_str(),uc->getchunkname_strs(cn),uc->getchunktype_strs(ct),datasize);
		else
			logger("%schunk: name '%s', array[%d] of type '%s', eledatasize %d, totaldatasize %d, ",
				indentstr.c_str(),uc->getchunkname_strs(cn),numele,uc->getchunktype_strs(ct),elesize,datasize);
		if (ct==KID_CHUNK) {	// don't skip subchunk data
			if (depth<chunktestdepth) {
				logger("ignoring data size of chunk, entering chunk\n");
				++depth; // enter subchunk data
				continue;
			}
		}
// else skip subchunk data
		if (!numele) { // it's not an array
			int i32;
			float f32;
			VC2 vc2;
			VC3 vc3;
			VC4 vc4;
			IDX3M idx3m;
			VC3M vc3m;
			POS_LIN pl;
			POS_TCB pt;
			POS_BEZv2 pb;
			ROT_LIN rl;
			ROT_TCB rt;
			ROT_BEZ rb;
			SCL_LIN sl;
			SCL_TCB st;
			SCL_BEZv2 sb;
			FLOAT_BEZv2 fb;
			FLOAT_LIN fl;
			FLOAT_TCB ft;
			switch(ct) {
			case KID_I32: // object id's (maybe)
				uc->readI32(i32);
				logger("DATA: i32 %d\n",i32);
				break;
			case KID_FLOAT: // object id's (maybe)
				uc->readFLOAT(f32);
				logger("DATA: f32 %f\n",f32);
				break;
			case KID_VEC2: // 2 floats (for uvs, maybe)
				uc->readVC2(vc2);
				logger("DATA: vec2 %f %f\n",vc2.x,vc2.y);
				break;
			case KID_VEC3: // 3 floats (for unpadded 3d points)
				uc->readVC3(vc3);
				logger("DATA: vec3 %f %f %f\n",vc3.x,vc3.y,vc3.z);
				break;
			case KID_VEC4: // 4 floats (for quats or rotaxis)
				uc->readVC4(vc4);
				logger("DATA: vec4 %f %f %f %f\n",vc4.x,vc4.y,vc4.z,vc4.w);
				break;
			case KID_IDX3M: // for faces
				uc->readIDX3M(idx3m);
				logger("DATA: face with matidx %d %d %d %d\n",idx3m.idx[0],idx3m.idx[1],idx3m.idx[2],idx3m.matidx);
				break;
			case KID_VEC3M: // 3 floats and 1 int (for mat idx)
				uc->readVC3M(vc3m);
				logger("DATA: vert with matidx %f %f %f %d\n",vc3m.x,vc3m.y,vc3m.z,vc3m.matidx);
				break;
			case KID_POS_LIN:
				uc->readPOS_LIN(pl);
				logger("DATA: pos_lin time %f, pos (%f,%f,%f)\n",pl.time,pl.value.x,pl.value.y,pl.value.z);
				break;
			case KID_POS_BEZv2:
				uc->readPOS_BEZv2(pb);
				logger("DATA: pos_bezv2 time %f, pos (%f,%f,%f), intan (%f,%f,%f), outtan (%f,%f,%f), inlength (%f,%f,%f), outlength (%f,%f,%f), flags %d\n",
					pb.time,pb.value.x,pb.value.y,pb.value.z,
					pb.intan.x,pb.intan.y,pb.intan.z,
					pb.outtan.x,pb.outtan.y,pb.outtan.z,
					pb.inlength.x,pb.inlength.y,pb.inlength.z,
					pb.outlength.x,pb.outlength.y,pb.outlength.z,pb.flags);
				break;
			case KID_POS_TCB:
				uc->readPOS_TCB(pt);
				logger("DATA: pos_tcb time %f, pos (%f,%f,%f), tens %f, cont %f, bias %f, easein %f, easeout %f\n",
					pt.time,pt.value.x,pt.value.y,pt.value.z,pt.tens,pt.cont,pt.bias,pt.easein,pt.easeout);
				break;
			case KID_ROT_LIN:
				uc->readROT_LIN(rl);
				logger("DATA: rot_lin time %f, quat (%f,%f,%f,%f)\n",
					rl.time,rl.value.x,rl.value.y,rl.value.z,rl.value.w);
				break;
			case KID_ROT_BEZ:
				uc->readROT_BEZ(rb);
				logger("DATA: rot_bez time %f, quat (%f,%f,%f,%f)\n",
					rb.time,rb.value.x,rb.value.y,rb.value.z,rb.value.w);
				break;
			case KID_ROT_TCB:
				uc->readROT_TCB(rt);
				logger("DATA: rot_tcb time %f, quat (%f,%f,%f,%f), tens %f, cont %f, bias %f, easein %f, easeout %f\n",
					rt.time,rt.value.x,rt.value.y,rt.value.z,rt.value.w,rt.tens,rt.cont,rt.bias,rt.easein,rt.easeout);
				break;
			case KID_SCL_LIN:
				uc->readSCL_LIN(sl);
				logger("DATA: scl_lin time %f, pos (%f,%f,%f)\n",sl.time,sl.value.x,sl.value.y,sl.value.z);
				break;
			case KID_SCL_BEZv2:
				uc->readSCL_BEZv2(sb);
				logger("DATA: scl_bezv2 time %f, pos (%f,%f,%f), intan (%f,%f,%f), outtan (%f,%f,%f), inlength (%f,%f,%f), outlength (%f,%f,%f), flags %d\n",
					sb.time,sb.value.x,sb.value.y,sb.value.z,
					sb.intan.x,sb.intan.y,sb.intan.z,
					sb.outtan.x,sb.outtan.y,sb.outtan.z,
					sb.inlength.x,sb.inlength.y,sb.inlength.z,
					sb.outlength.x,sb.outlength.y,sb.outlength.z,sb.flags);
				break;
			case KID_FLOAT_BEZv2:
				uc->readFLOAT_BEZv2(fb);
				logger("DATA: scl_bezv2 time %f, value %f, intan %f, outtan %f, inlength %f, outlength %f, flags %d\n",
					fb.time,fb.value,fb.intan,fb.outtan,fb.inlength,fb.outlength,fb.flags);
				break;
			case KID_SCL_TCB:
				uc->readSCL_TCB(st);
				logger("DATA: scl_tcb time %f, pos (%f,%f,%f), tens %f, cont %f, bias %f, easein %f, easeout %f\n",
					st.time,st.value.x,st.value.y,st.value.z,st.tens,st.cont,st.bias,st.easein,st.easeout);
				break;
			case KID_FLOAT_LIN:
				uc->readFLOAT_LIN(fl);
				logger("DATA: float_lin time %f, val %f\n",fl.time,fl.value);
				break;
			case KID_FLOAT_TCB:
				uc->readFLOAT_TCB(ft);
				logger("DATA: float_tcb time %f, val %f, tens %f, cont %f, bias %f, easein %f, easeout %f\n",
					ft.time,ft.value,ft.tens,ft.cont,ft.bias,ft.easein,ft.easeout);
				break;
			case KID_ENDCHUNK:
				logger("ENDCHUNK: SKIPPING\n");
				uc->skipdata();
				--depth;
				break;
			case KID_CHUNK:
				logger("CHUNK: SKIPPING\n");
				uc->skipdata();
				break;
			default:
				logger("DATA: SKIPPING\n");
				uc->skipdata();
				break;
			}
		} else { // it's an array
			I32 i;
			string i8v;
			vector<I32> i32v;
			vector<VC2> vc2v;
			vector <VC3> vc3v;
			vector<IDX3> idx3v;
			vector<IDX3M> idx3mv;
			vector<VC3M> vc3mv;
			switch(ct) {
			case KID_I8:
				i8v=uc->readI8v();
				logger("DATA ARRAY: chars '%s'\n",i8v.c_str());
				break;
			case KID_I32:
				i32v=uc->readI32v();
				logger("\n");
				for (i=0;i<static_cast<int>(i32v.size());i++)
					logger("%s   DATA ARRAY[%d]: i32 %d\n",indentstr.c_str(),i,i32v[i]);
				break;
			case KID_VEC2: // 2 floats (for uvs, maybe)
				logger("\n");
				vc2v=uc->readVC2v();
				for (i=0;i<static_cast<int>(vc2v.size());i++)
					logger("%s   DATA ARRAY[%d]: vec2 %f %f\n",indentstr.c_str(),i,vc2v[i].x,vc2v[i].y);
				break;
			case KID_VEC3: // 3 floats (for unpadded 3d points)
				logger("\n");
				vc3v=uc->readVC3v();
				for (i=0;i<static_cast<int>(vc3v.size());i++)
					logger("%s   DATA ARRAY[%d]: vec3 %f %f %f\n",indentstr.c_str(),i,vc3v[i].x,vc3v[i].y,vc3v[i].z);
				break;
			case KID_IDX3: //%s for tfaces
				logger("\n");
				idx3v=uc->readIDX3v();
				for (i=0;i<static_cast<int>(idx3v.size());i++)
					logger("%s   DATA ARRAY[%d]: tface %d %d %d\n",indentstr.c_str(),i,idx3v[i].idx[0],idx3v[i].idx[1],idx3v[i].idx[2]);
				break;
			case KID_IDX3M: // for faces
				logger("\n");
				idx3mv=uc->readIDX3Mv();
				for (i=0;i<static_cast<int>(idx3mv.size());i++)
					logger("%s   DATA ARRAY[%d]: face with matidx %d %d %d %d\n",indentstr.c_str(),i,idx3mv[i].idx[0],idx3mv[i].idx[1],idx3mv[i].idx[2],idx3mv[i].matidx);
				break;
			case KID_VEC3M: // 3 floats and 1 int (for mat idx)
				logger("\n");
				vc3mv=uc->readVC3Mv();
				for (i=0;i<static_cast<int>(vc3mv.size());i++)
					logger("%s   DATA ARRAY[%d]: vert with matidx %f %f %f %d\n",indentstr.c_str(),i,vc3mv[i].x,vc3mv[i].y,vc3mv[i].z,vc3mv[i].matidx);
				break;
			default:
				logger("DATA ARRAY: skipping\n");
				uc->skipdata();
				break;
			}
		}
	}
	delete uc;
}
