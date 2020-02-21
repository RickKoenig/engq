#define D2_3D
#include <m_eng.h>

#if 0
static float damasterscale=1;
void setmasterscale(float scl)
{
	damasterscale=scl;
}

float getmasterscale()
{
	return damasterscale;
}
#endif
#if 1
//const float MAXTIME_2_ENGTIME=(1.0f/160.0f);

// straight from max6sdk
// Track flags
#define TFLAG_CURVESEL			(1<<0)
#define TFLAG_RANGE_UNLOCKED	(1<<1)
#define TFLAG_LOOPEDIN			(1<<3)
#define TFLAG_LOOPEDOUT			(1<<4)
#define TFLAG_COLOR				(1<<5)	// Set for Bezier Point3/Point4 controlers that are color controllers
#define TFLAG_HSV				(1<<6)	// Set for color controls that interpolate in HSV
#define TRACK_XLOCKED			(1<<7)	// Used by controller to lock Y and Z to X.
#define KT_FLAG_DELAY_KEYSCHANGED (1<<8)
#define TFLAG_NOTKEYABLE		(1<<9)
#define TFLAG_TCBQUAT_NOWINDUP	(1<<10)

static pointf3 vc42quat(const VC4& v)
{
	return pointf3x(v.x,v.y,v.z,v.w);
}

static VC4 quat2vc4(const pointf3& q)
{
	VC4 r;
	r.x=q.x;
	r.y=q.y;
	r.z=q.z;
	r.w=q.w;
	return r;
}

static void resetvc4(VC4& v)
{
	v.x=0;
	v.y=0;
	v.z=0;
	v.w=1;
}

// is is one object chunk entry in a .bws file
struct tree_info {
	enum {UNKNOWN,GEOM,HELPER,CAMERA,LIGHT_AMB,BONE,BIPED,LIGHT_DIR,LIGHT_OMNI,LIGHT_SPOT}; // kind
	string name;
	string lm_name;	// lightmap
	vector<string> userprops;
	S32 id;
	S32 pid;
	S32 kind;
	mat4 matrix;
	pointf3 pos,rot,scl;
	string target;
// for cameras
	float fov;
// for lights
	float intensity;
	pointf3 color;
	float 	hotsize,fallsize;
	S32		useNearAtten;
	float	nearAttenStart,nearAttenEnd;
	S32   	useAtten;
	float 	attenStart,attenEnd;
	bool   	overshoot;
	bool   	shadow;
	S32		trackflags;
	vector <POS_BEZv2> pos_bez;
	vector <POS_TCB> pos_tcb;
	vector <POS_LIN> pos_lin;
	vector <ROT_BEZ> rot_bez;
	vector <ROT_TCB> rot_tcb;
	vector <ROT_LIN> rot_lin;
	vector <FLOAT_BEZv2> rot_euler_x;
	vector <FLOAT_BEZv2> rot_euler_y;
	vector <FLOAT_BEZv2> rot_euler_z;
	vector <SCL_BEZv2> scl_bez;
	vector <SCL_TCB> scl_tcb;
	vector <SCL_LIN> scl_lin;
	vector <FLOAT_LIN> vis_lin;
	tree_info() : id(-1),pid(-1),kind(UNKNOWN),fov(0),intensity(0),
				hotsize(0),fallsize(0),
				useNearAtten(false),nearAttenStart(0),nearAttenEnd(0),
				useAtten(false),attenStart(0),attenEnd(0),
				overshoot(false),shadow(false),trackflags(0)
	{
		color.x=color.y=color.z=0;
/*		pos_bez.clear();
		pos_tcb.clear();
		pos_lin.clear();
		rot_bez.clear();
		rot_tcb.clear();
		rot_lin.clear();
		scl_bez.clear();
		scl_tcb.clear();
		scl_lin.clear(); */
		pos=rot=zerov;
		scl=onev;
		rot.w=1;
	}

};

// process an optimized object chunk from .bws file, return one tree_info
static tree_info getobjinfo(unchunker* uc)
{
	S32 tf=0; // accumulate track flags
	tree_info oio;
	chunkname cn;
	chunktype ct;
//	bool gotuvs=false;
	S32 numele,elesize,datasize;
	float ms=1;//getmasterscale();
	POS_LIN pl;
	POS_BEZv2 pb;
	POS_TCB pt;
	ROT_LIN rl;
	ROT_BEZ rb;
	ROT_TCB rt;
	pointf3 rlt=pointf3x(); // running total
	pointf3 rbt=pointf3x();
	pointf3 rtt=pointf3x();
	SCL_LIN sl;
	SCL_BEZv2 sb;
	FLOAT_BEZv2 fb;
	SCL_TCB st;
	FLOAT_LIN fl;
	while(uc->getchunkheader(cn,ct,numele,elesize,datasize)) {
		if (ct==KID_ENDCHUNK)
			break;
		MATRIX34 m;
		switch(cn) {
		case UID_USERPROP:
			if (numele && ct==KID_I8) {
				string uprop=uc->readI8v();

// parse string for whitespace
				bool addlaststr=false;
				string s;
				for (int i=0;uprop[i];++i) {
					if ((U32)uprop[i]>' ' && (U32)uprop[i]<=0x7f) {
						s+=uprop[i];
						addlaststr=true;
					} else {
						if (addlaststr) {
							oio.userprops.push_back(s);
							s="";
							addlaststr=false;
						}
					}
				}
				if (addlaststr)
					oio.userprops.push_back(s);
// done parse string for whitespace
//				oio.userprops.push_back(uprop);
			} else
				uc->skipdata();
			break;
		case UID_KIND:
			if (!numele && ct==KID_I32)
				uc->readI32(oio.kind);
			else
				uc->skipdata();
			break;
		case UID_NAME:
			if (numele && ct==KID_I8) {
				oio.name=uc->readI8v();
			} else
				uc->skipdata();
			break;
		case UID_ID:
			if (!numele && ct==KID_I32)
				uc->readI32(oio.id);
			else
				uc->skipdata();
			break;
		case UID_PID:
			if (!numele && ct==KID_I32)
				uc->readI32(oio.pid);
			else
				uc->skipdata();
			break;
		case UID_POS:
			if (!numele && ct==KID_VEC3) {
				VC3 pos;
				uc->readVC3(pos);
				oio.pos.x=pos.x*ms;
				oio.pos.y=pos.y*ms;
				oio.pos.z=pos.z*ms;
			} else
				uc->skipdata();
			break;
		case UID_ROT_QUAT:
			if (!numele && ct==KID_VEC4) {
				VC4 rot;
				uc->readVC4(rot);
				oio.rot.x=rot.x;oio.rot.y=rot.y;oio.rot.z=rot.z,oio.rot.w=rot.w;
			} else
				uc->skipdata();
			break;
		case UID_SCALE:
			if (!numele && ct==KID_VEC3) {
				VC3 scl;
				uc->readVC3(scl);
				oio.scl.x=scl.x;oio.scl.y=scl.y;oio.scl.z=scl.z;
			} else
				uc->skipdata();
			break;
		case UID_MATRIX:
			m=uc->readMATRIX34();
			for (S32 i=0;i<4;i++) {
				VC3& r=m.rows[i];
				oio.matrix.e[i][0]=r.x;oio.matrix.e[i][1]=r.y;oio.matrix.e[i][2]=r.z;oio.matrix.e[i][3]=0;
			}
			oio.matrix.e[3][3]=1;
			oio.matrix.e[3][0]*=ms;
			oio.matrix.e[3][1]*=ms;
			oio.matrix.e[3][2]*=ms;
			break;
		case UID_TARGET:
			if (numele && ct==KID_I8) {
				oio.target=uc->readI8v();
			} else
				uc->skipdata();
			break;
		case UID_CAMERA_FOV:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.fov); // everything is in radians
			else
				uc->skipdata();
			break;
		case UID_LIGHT_COLOR:
			if (!numele && ct==KID_VEC3) {
				VC3 c;
				uc->readVC3(c);
				oio.color.x=c.x;
				oio.color.y=c.y;
				oio.color.z=c.z;
			} else
				uc->skipdata();
			break;
		case UID_LIGHT_INTENSITY:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.intensity);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_HOTSIZE:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.hotsize); // everything is in radians
			else
				uc->skipdata();
			break;
		case UID_LIGHT_FALLSIZE:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.fallsize); // everything is in radians
			else
				uc->skipdata();
			break;
		case UID_LIGHT_NEAR_ATTEN_START:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.nearAttenStart);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_NEAR_ATTEN_END:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.nearAttenEnd);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_ATTEN_START:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.attenStart);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_ATTEN_END:
			if (!numele && ct==KID_FLOAT)
				uc->readFLOAT(oio.attenEnd);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_USE_NEAR_ATTEN:
			if (!numele && ct==KID_I32)
				uc->readI32((S32&)oio.useNearAtten);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_USE_ATTEN:
			if (!numele && ct==KID_I32)
				uc->readI32((S32&)oio.useAtten);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_SHADOW:
			if (!numele && ct==KID_I32)
				uc->readI32((S32&)oio.shadow);
			else
				uc->skipdata();
			break;
		case UID_LIGHT_OVERSHOOT:
			if (!numele && ct==KID_I32)
				uc->readI32((S32&)oio.overshoot);
			else
				uc->skipdata();
			break;
		case UID_POS_BEZ:
			uc->readPOS_BEZv2(pb);
			pb.value.x*=ms;
			pb.value.y*=ms;
			pb.value.z*=ms;
			pb.intan.x*=ms;
			pb.outtan.x*=ms;
			oio.pos_bez.push_back(pb);
			break;
		case UID_POS_TCB:
			uc->readPOS_TCB(pt);
			pt.value.x*=ms;
			pt.value.y*=ms;
			pt.value.z*=ms;
			oio.pos_tcb.push_back(pt);
			break;
		case UID_POS_SAMP:
			oio.trackflags|=TFLAG_LOOPEDIN|TFLAG_LOOPEDOUT;
		case UID_POS_LIN:
			uc->readPOS_LIN(pl);
			pl.value.x*=ms;
			pl.value.y*=ms;
			pl.value.z*=ms;
			oio.pos_lin.push_back(pl);
			break;
		case UID_ROT_BEZ:
			uc->readROT_BEZ(rb);
			if (wininfo.relquat) {
				quattimes(&rb.value,&rbt,&rbt);
				rb.value=rbt;
			}
			oio.rot_bez.push_back(rb);
			break;
		case UID_ROT_TCB:
			uc->readROT_TCB(rt);
			if (wininfo.relquat) {
				quattimes(&rt.value,&rtt,&rtt);
				rt.value=rtt;
			}
			oio.rot_tcb.push_back(rt);
			break;
		case UID_ROT_LIN:
			uc->readROT_LIN(rl);
			if (wininfo.relquat) {
				quattimes(&rl.value,&rlt,&rlt);
				rl.value=rlt;
			}
			oio.rot_lin.push_back(rl);
			break;
		case UID_ROT_SAMP:
			uc->readROT_LIN(rl);
//			if (wininfo.relquat) {
			if (0) { // sampled appears to be absolute quats
				quattimes(&rl.value,&rlt,&rlt);
				rl.value=rlt;
			}
			oio.trackflags|=TFLAG_LOOPEDIN|TFLAG_LOOPEDOUT;
			oio.rot_lin.push_back(rl);
			break;
		case UID_SCL_BEZ:
			uc->readSCL_BEZv2(sb);
			oio.scl_bez.push_back(sb);
			break;
		case UID_SCL_TCB:
			uc->readSCL_TCB(st);
			oio.scl_tcb.push_back(st);
			break;
		case UID_SCL_SAMP:
			oio.trackflags|=TFLAG_LOOPEDIN|TFLAG_LOOPEDOUT;
		case UID_SCL_LIN:
			uc->readSCL_LIN(sl);
			oio.scl_lin.push_back(sl);
			break;
		case UID_ROT_EULER_X:
			uc->readFLOAT_BEZv2(fb);
			oio.rot_euler_x.push_back(fb);
			break;
		case UID_ROT_EULER_Y:
			uc->readFLOAT_BEZv2(fb);
			oio.rot_euler_y.push_back(fb);
			break;
		case UID_ROT_EULER_Z:
			uc->readFLOAT_BEZv2(fb);
			oio.rot_euler_z.push_back(fb);
			break;
		case UID_VIS_SAMP:
			uc->readFLOAT_LIN(fl);
			oio.trackflags|=TFLAG_LOOPEDIN|TFLAG_LOOPEDOUT;
			oio.vis_lin.push_back(fl);
			break;

/*		case UID_KEYFRAME:
			switch(ct) {
			case KID_POS_TCB:
				break;
			case KID_POS_BEZ:
				break;
			case KID_POS_LIN:
				break;
			case KID_ROT_TCB:
				break;
			case KID_ROT_BEZ:
				break;
			case KID_ROT_LIN:
				break;
			case KID_SCL_TCB:
				break;
			case KID_SCL_BEZ:
				break;
			case KID_SCL_LIN:
				break;
			};
			break; */
		case UID_TRACKFLAGS:
			uc->readI32(tf);
			oio.trackflags|=tf;
			break;
		default:
			uc->skipdata();
			break;
		}
	}
// check for lightmaps (fortpoint)
//	if (video3dinfo.is3dhardware) { // only hardware will use a lightmap
		U32 p=0;
		while (p+1<oio.userprops.size()) {
			if (oio.userprops[p]=="lightmap") {
				oio.lm_name=oio.userprops[p+1]+".dds";
				logger("found lightmap '%s'\n",oio.lm_name.c_str());
			}
/*			U32 fp=oio.userprops[p].find(' ');
			if (fp!=string::npos) {
				string fst=oio.userprops[p].substr(0,fp);
				string snd=oio.userprops[p].substr(fp+1);
				logger("split userprop into '%s' and '%s'\n",fst.c_str(),snd.c_str());
				if (fst=="lightmap") {
					oio.lm_name=snd+".dds";
					logger("found lightmap '%s'\n",oio.lm_name.c_str());
				}
			} */
			++p;
		}
//	}
	return oio;
}

template <class T>
void pos2key(const vector<T>& p,tree2* t)
{
	S32 j;
	if (!t->seqs) {
		t->seqs=new seq;
		memset(t->seqs,0,sizeof(seq));
		t->nseq=1;
//		t->playo2p=O2P_FROMTRANSQUATSCALE;
		t->buildo2p=O2P_FROMTRANSQUATSCALE;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
	keyframe* kx=new keyframe;
	keyframe* ky=new keyframe;
	keyframe* kz=new keyframe;
	t->seqs->keysn[TRANSX]=kx;
	t->seqs->keysn[TRANSY]=ky;
	t->seqs->keysn[TRANSZ]=kz;
	//kx->refcnt=1;
	S32 nk=p.size();
	kx->nkeys=nk;
	kx->curkey=0;
	kx->keys=new key[nk];
	//ky->refcnt=1;
	ky->nkeys=nk;
	ky->curkey=0;
	ky->keys=new key[nk];
	//kz->refcnt=1;
	kz->nkeys=nk;
	kz->curkey=0;
	kz->keys=new key[nk];
	for (j=0;j<static_cast<S32>(p.size());++j) {
		kx->keys[j].time=p[j].time;
		kx->keys[j].value=p[j].value.x;
		kx->keys[j].linear=1;
		ky->keys[j].time=p[j].time;
		ky->keys[j].value=p[j].value.y;
		ky->keys[j].linear=1;
		kz->keys[j].time=p[j].time;
		kz->keys[j].value=p[j].value.z;
		kz->keys[j].linear=1;
	}
	if (p[0].time<t->seqs->startframe)
		t->seqs->startframe=p[0].time;
	if (p[j-1].time>t->seqs->endframe)
		t->seqs->endframe=p[j-1].time;
	tcbcalcallcoeffs(kx);
	tcbcalcallcoeffs(ky);
	tcbcalcallcoeffs(kz);
}

#if 1
void pos2key(const vector<POS_BEZv2>& p,tree2* t)
{
	logger("specialized pos2key POS_BEZv2\n");
	if (!t->seqs) {
		t->seqs=new seq;
//		t->seqs=(seq *)memzalloc(sizeof(seq));
		memset(t->seqs,0,sizeof(seq));
		t->nseq=1;
//		t->playo2p=O2P_FROMTRANSQUATSCALE;
		t->buildo2p=O2P_FROMTRANSQUATSCALE;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
	keyframe* kx=new keyframe;
	keyframe* ky=new keyframe;
	keyframe* kz=new keyframe;
//	keyframe* kx=(keyframe *)memzalloc(sizeof(keyframe));
//	keyframe* ky=(keyframe *)memzalloc(sizeof(keyframe));
//	keyframe* kz=(keyframe *)memzalloc(sizeof(keyframe));
	t->seqs->keysn[TRANSX]=kx;
	t->seqs->keysn[TRANSY]=ky;
	t->seqs->keysn[TRANSZ]=kz;
	//kx->refcnt=1;
	S32 nk=p.size();
	kx->nkeys=nk;
	kx->curkey=0;
	kx->keys=new key[nk];
	//ky->refcnt=1;
	ky->nkeys=nk;
	kz->curkey=0;
	ky->keys=new key[nk];
	//kz->refcnt=1;
	kz->nkeys=nk;
	kz->curkey=0;
	kz->keys=new key[nk];
	S32 j;
	for (j=0;j<nk;++j) {
		kx->keys[j].time=p[j].time;
		kx->keys[j].value=p[j].value.x;
		kx->keys[j].intan=p[j].intan.x;
		kx->keys[j].outtan=p[j].outtan.x;
		kx->keys[j].linear=0;

		ky->keys[j].time=p[j].time;
		ky->keys[j].value=p[j].value.y;
		ky->keys[j].intan=p[j].intan.y;
		ky->keys[j].outtan=p[j].outtan.y;
		ky->keys[j].linear=0;

		kz->keys[j].time=p[j].time;
		kz->keys[j].value=p[j].value.z;
		kz->keys[j].intan=p[j].intan.z;
		kz->keys[j].outtan=p[j].outtan.z;
		kz->keys[j].linear=0;
	}
	if (p[0].time<t->seqs->startframe)
		t->seqs->startframe=p[0].time;
	if (p[j-1].time>t->seqs->endframe)
		t->seqs->endframe=p[j-1].time;
	bezcalcallcoeffs(kx);
	bezcalcallcoeffs(ky);
	bezcalcallcoeffs(kz);
}

template <class T>
void rot2key(const vector<T>& r,tree2* t)
{
	if (!t->seqs) {
		t->seqs=new seq;
		t->nseq=1;
		memset(t->seqs,0,sizeof(seq));
//		t->playo2p=O2P_FROMTRANSQUATSCALE;
		t->buildo2p=O2P_FROMTRANSQUATSCALE;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
	keyframe* kx=new keyframe;
	keyframe* ky=new keyframe;
	keyframe* kz=new keyframe;
	keyframe* kw=new keyframe;
	t->seqs->keysn[ROTX]=kx;
	t->seqs->keysn[ROTY]=ky;
	t->seqs->keysn[ROTZ]=kz;
	t->seqs->keysn[ROTW]=kw;
	//kx->refcnt=1;
	S32 nk=r.size();
	kx->nkeys=nk;
	kx->curkey=0;
	kx->keys=new key[nk];
	//ky->refcnt=1;
	ky->nkeys=nk;
	ky->curkey=0;
	ky->keys=new key[nk];
	//kz->refcnt=1;
	kz->nkeys=nk;
	kz->curkey=0;
	kz->keys=new key[nk];
	//kw->refcnt=1;
	kw->nkeys=nk;
	kw->curkey=0;
	kw->keys=new key[nk];
	S32 j;
	for (j=0;j<nk;++j) {
		kx->keys[j].time=r[j].time;
		kx->keys[j].value=r[j].value.x;
		kx->keys[j].linear=1;
		ky->keys[j].time=r[j].time;
		ky->keys[j].value=r[j].value.y;
		ky->keys[j].linear=1;
		kz->keys[j].time=r[j].time;
		kz->keys[j].value=r[j].value.z;
		kz->keys[j].linear=1;
		kw->keys[j].time=r[j].time;
		kw->keys[j].value=r[j].value.w;
		kw->keys[j].linear=1;
	}
	if (r[0].time<t->seqs->startframe)
		t->seqs->startframe=r[0].time;
	if (r[j-1].time>t->seqs->endframe)
		t->seqs->endframe=r[j-1].time;
}

template <class T>
void scl2key(const vector<T>& s,tree2* t)
{
	if (!t->seqs) {
		t->seqs=new seq;
		memset(t->seqs,0,sizeof(seq));
		t->nseq=1;
//		t->playo2p=O2P_FROMTRANSQUATSCALE;
		t->buildo2p=O2P_FROMTRANSQUATSCALE;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
	keyframe* kx=new keyframe;
	keyframe* ky=new keyframe;
	keyframe* kz=new keyframe;
	t->seqs->keysn[SCLX]=kx;
	t->seqs->keysn[SCLY]=ky;
	t->seqs->keysn[SCLZ]=kz;
	//kx->refcnt=1;
	S32 nk=s.size();
	kx->nkeys=nk;
	kx->curkey=0;
	kx->keys=new key[nk];
	//ky->refcnt=1;
	ky->nkeys=nk;
	ky->curkey=0;
	ky->keys=new key[nk];
	//kz->refcnt=1;
	kz->nkeys=nk;
	kz->curkey=0;
	kz->keys=new key[nk];
	S32 j;
	for (j=0;j<static_cast<S32>(s.size());++j) {
		kx->keys[j].time=s[j].time;
		kx->keys[j].value=s[j].value.x;
		kx->keys[j].linear=1;
		ky->keys[j].time=s[j].time;
		ky->keys[j].value=s[j].value.y;
		ky->keys[j].linear=1;
		kz->keys[j].time=s[j].time;
		kz->keys[j].value=s[j].value.z;
		kz->keys[j].linear=1;
	}
	if (s[0].time<t->seqs->startframe)
		t->seqs->startframe=s[0].time;
	if (s[j-1].time>t->seqs->endframe)
		t->seqs->endframe=s[j-1].time;
	tcbcalcallcoeffs(kx);
	tcbcalcallcoeffs(ky);
	tcbcalcallcoeffs(kz);
}

//specialized
void scl2key(const vector<SCL_BEZv2>& s,tree2* t)
{
	S32 j;
	logger("specialized scl2key SCL_BEZv2\n");
	if (!t->seqs) {
		t->seqs=new seq;
//		t->seqs=(seq *)memzalloc(sizeof(seq));
		memset(t->seqs,0,sizeof(seq));
		t->nseq=1;
//		t->playo2p=O2P_FROMTRANSQUATSCALE;
		t->buildo2p=O2P_FROMTRANSQUATSCALE;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
	keyframe* kx=new keyframe;
	keyframe* ky=new keyframe;
	keyframe* kz=new keyframe;
//	keyframe* kx=(keyframe *)memzalloc(sizeof(keyframe));
//	keyframe* ky=(keyframe *)memzalloc(sizeof(keyframe));
//	keyframe* kz=(keyframe *)memzalloc(sizeof(keyframe));
	t->seqs->keysn[SCLX]=kx;
	t->seqs->keysn[SCLY]=ky;
	t->seqs->keysn[SCLZ]=kz;
	//kx->refcnt=1;
	S32 nk=s.size();
	kx->nkeys=nk;
	kx->curkey=0;
	kx->keys=new key[nk];
//	kx->keys=(key *)memzalloc(sizeof(key)*kx->nkeys);
	//ky->refcnt=1;
	ky->nkeys=nk;
	ky->curkey=0;
	ky->keys=new key[nk];
	//kz->refcnt=1;
	kz->nkeys=nk;
	kz->curkey=0;
	kz->keys=new key[nk];
	for (j=0;j<static_cast<S32>(s.size());++j) {
		kx->keys[j].time=s[j].time;
		kx->keys[j].value=s[j].value.x;
		kx->keys[j].intan=s[j].intan.x;
		kx->keys[j].outtan=s[j].outtan.x;
		kx->keys[j].linear=0;

		ky->keys[j].time=s[j].time;
		ky->keys[j].value=s[j].value.y;
		ky->keys[j].intan=s[j].intan.y;
		ky->keys[j].outtan=s[j].outtan.y;
		ky->keys[j].linear=0;

		kz->keys[j].time=s[j].time;
		kz->keys[j].value=s[j].value.z;
		kz->keys[j].intan=s[j].intan.z;
		kz->keys[j].outtan=s[j].outtan.z;
		kz->keys[j].linear=0;
	}
	if (s[0].time<t->seqs->startframe)
		t->seqs->startframe=s[0].time;
	if (s[j-1].time>t->seqs->endframe)
		t->seqs->endframe=s[j-1].time;
	bezcalcallcoeffs(kx);
	bezcalcallcoeffs(ky);
	bezcalcallcoeffs(kz);
}

template <class T>
void float2key(const vector<T>& s,tree2* t,int keychannel)
{
	if (!t->seqs) {
		t->seqs=new seq;
		memset(t->seqs,0,sizeof(seq));
		t->nseq=1;
//		t->playo2p=O2P_FROMTRANSQUATSCALE;
//		t->buildo2p=O2P_FROMTRANSQUATSCALE;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
	keyframe* kc=new keyframe;
	//kc->refcnt=1;
	if (t->seqs->keysn[keychannel])
		delete[] t->seqs->keysn[keychannel];//memfree(t->seqs->keys[keychannel]);
	t->seqs->keysn[keychannel]=kc;
	S32 nk=s.size();
	kc->nkeys=nk;
	kc->curkey=0;
	kc->keys=new key[nk];
	S32 j;
	for (j=0;j<static_cast<S32>(s.size());++j) {
		kc->keys[j].time=s[j].time;
		kc->keys[j].value=s[j].value;
		kc->keys[j].linear=1;
	}
	if (s[0].time<t->seqs->startframe)
		t->seqs->startframe=s[0].time;
	if (s[j-1].time>t->seqs->endframe)
		t->seqs->endframe=s[j-1].time;

	tcbcalcallcoeffs(kc);
//	tcbcalcallcoeffs(kx);
//	tcbcalcallcoeffs(ky);
//	tcbcalcallcoeffs(kz);
}

#if 0
// specialized
void float2key(vector<FLOAT_BEZv2>& k,tree2* t,S32 keychannel)
{
	if (!t->seqs) {
		t->seqs=new seq;
		memset(t->seqs,0,sizeof(seq));
		t->nseq=1;
		t->seqs->startframe=10000000;
		t->seqs->endframe=-10000000;
	}
//	t->playo2p=O2P_FROMTRANSROTSCALE;
	t->buildo2p=O2P_FROMTRANSROTSCALE; // this will override the QUAT
	keyframe* kc=new keyframe;
	kc->refcnt=1;
	if (t->seqs->keys[keychannel])
		delete[] t->seqs->keys[keychannel];//memfree(t->seqs->keys[keychannel]);
	t->seqs->keys[keychannel]=kc;
	S32 nk=k.size();
	kc->nkeys=nk;
	kc->curkey=0;
	kc->keys=new key[nk];
	S32 j;
	for (j=0;j<static_cast<S32>(k.size());++j) {
		logger("float bezv2 key found idx %d, chan %d, time %f, value %f, intan %f, outtan %f\n",
			j,keychannel,k[j].time,k[j].value,k[j].intan,k[j].outtan);
		kc->keys[j].time=k[j].time;
		kc->keys[j].value=k[j].value;
		kc->keys[j].intan=k[j].intan;
		kc->keys[j].outtan=k[j].outtan;
		kc->keys[j].linear=0;
	}
	if (k[0].time<t->seqs->startframe)
		t->seqs->startframe=k[0].time;
	if (k[j-1].time>t->seqs->endframe)
		t->seqs->endframe=k[j-1].time;
	bezcalcallcoeffs(kc);
}
#endif

#endif
// load a .bws chunk scene file into tree hierarchy
void loadbws(tree2* scene)
{
	scene->flags|=TF_ISSCENE;
//	const S32 extrachildren=10;
//	const S32 rootextrachildren=1000;
	vector<tree_info> objlist;
	chunkname cn;
	chunktype ct;
	S32 datasize,numele,elesize;
	logger("loadbws ------------------ '%s' -------\n",scene->name.c_str());
// first load .bws into objlist
	if (wininfo.dumpbws)
		unchunktest(scene->name.c_str(),10);
	unchunker* uc = new unchunker(scene->name.c_str());
	while(uc->getchunkheader(cn,ct,numele,elesize,datasize)) {
		switch(cn) {
		case UID_VERSION:	// check version
			if (ct==KID_I32) { // only i32 types
				S32 ver;
				uc->readI32(ver);
				logger("version %d\n",ver);
			} else
				uc->skipdata();
			break;
		case UID_OBJECT: // we like object chunks
			if (ct==KID_CHUNK)
				objlist.push_back(getobjinfo(uc)); // build up objlist
			else
				uc->skipdata();
			break;
		default:
			uc->skipdata(); // skip data of unknown chunks
			break;
		}
	}
	delete uc;
// build hierarchy, count children from parents, assumes objlist_optimized[i].id == i
	vector<S32> nchildren(objlist.size(),0);
	vector<tree2*> part(objlist.size(),0);
	S32 rootnchildren=0;
// count up children
	S32 i;
	for (i=0;i<static_cast<S32>(objlist.size());i++) {
		if (objlist[i].id!=i) // sanity check
			errorexit("bad id");
		S32 pid=objlist[i].pid;
		if (pid==-2) // exporter sets this error when it can't find parent
			errorexit("bad pid");
		if (pid==-1)
			++rootnchildren;	// object has no parent, one more child of the whole scene
		else if (pid<0 || pid>=static_cast<S32>(objlist.size()))
			errorexit("pid out of range");
		else
			++nchildren[pid];	// this parent has one more child
	}
// build scene tree
//	tree2* scene=alloctree(rootnchildren+rootextrachildren,0);
//	identmat4(&scene->o2p);
//	scene->buildo2p=O2P_NONE;
//	scene->flags|=TF_ISSCENE;
//	mystrncpy(scene->name,fname,NAMESIZE);
//	S32 endstate=END_STOP;
// build children trees
	for (i=0;i<static_cast<S32>(objlist.size());i++) {
		string bwoname=objlist[i].name+".bwo";
		setsuggestlightmap(objlist[i].lm_name);
		switch(objlist[i].kind) {
		case tree_info::GEOM:
			part[i]=new tree2(bwoname.c_str()); // will call loadbwo
//			part[i]=alloctree(nchildren[i]+extrachildren,bwoname.c_str()); // will call loadbwo
			break;
		case tree_info::BONE:
		case tree_info::BIPED:
		case tree_info::HELPER:
		case tree_info::CAMERA:
			part[i]=new tree2(bwoname.c_str()); // will call loadbwo
//			part[i]=alloctree(nchildren[i]+extrachildren,0); // null/helper object
//			strncpy(part[i]->name,bwoname.c_str(),NAMESIZE);
			part[i]->flags|=TF_ISNULL;
			break;
		case tree_info::LIGHT_DIR:
		case tree_info::LIGHT_SPOT:
			part[i]=new tree2(bwoname.c_str()); // will call loadbwo
//			part[i]=alloctree(nchildren[i]+extrachildren,0); // lightdir object
//			strncpy(part[i]->name,bwoname.c_str(),NAMESIZE);
			part[i]->flags|=TF_ISLIGHT;
			logger("dir light color %f %f %f\n",objlist[i].color.x,objlist[i].color.y,objlist[i].color.z);
			part[i]->lightcolor.x=objlist[i].color.x;
			part[i]->lightcolor.y=objlist[i].color.y;
			part[i]->lightcolor.z=objlist[i].color.z;
			part[i]->intensity=objlist[i].intensity;
			addlighttolist(part[i]);
			break;
		case tree_info::LIGHT_AMB:
		case tree_info::LIGHT_OMNI:
			part[i]=new tree2(bwoname.c_str()); // will call loadbwo
//			part[i]=alloctree(nchildren[i]+extrachildren,0); // lightamb object
//			strncpy(part[i]->name,bwoname.c_str(),NAMESIZE);
			part[i]->flags|=TF_ISLIGHT|TF_ISAMBLIGHT;
			logger("amb light color %f %f %f\n",objlist[i].color.x,objlist[i].color.y,objlist[i].color.z);
			part[i]->lightcolor.x=objlist[i].color.x;
			part[i]->lightcolor.y=objlist[i].color.y;
			part[i]->lightcolor.z=objlist[i].color.z;
			part[i]->intensity=objlist[i].intensity;
			addlighttolist(part[i]);
			break;
/*		case tree_info::CAMERA:
			part[i]=alloctree(nchildren[i]+extrachildren,0); // camera object
			strncpy(part[i]->name,bwoname.c_str(),NAMESIZE);
			part[i]->flags|=TF_ISCAMERA;
			part[i]->zoom=1/tan(objlist[i].fov/2)*4/3.0f;	// assumes fov set on x axis and aspect in max is 4/3
			break; */
		default:
			errorexit("unknown object kind %d",objlist[i].kind);
			break;
		};
		setsuggestlightmap("");
// build keyframes
		part[i]->buildo2p=O2P_NONE;
#if 1
		if (objlist[i].trackflags&(TFLAG_LOOPEDIN|TFLAG_LOOPEDOUT))
//		if (objlist[i].trackflags)
			part[i]->repcount=part[i]->currepcount=0; // loop
		else
			part[i]->repcount=part[i]->currepcount=1; // stop
// pos
		if (objlist[i].pos_lin.size())
			pos2key(objlist[i].pos_lin,part[i]);
		else if (objlist[i].pos_bez.size())
			pos2key(objlist[i].pos_bez,part[i]);
		else if (objlist[i].pos_tcb.size())
			pos2key(objlist[i].pos_tcb,part[i]);
// rot
		if (objlist[i].rot_lin.size())
			rot2key(objlist[i].rot_lin,part[i]);
		else if (objlist[i].rot_bez.size())
			rot2key(objlist[i].rot_bez,part[i]);
		else if (objlist[i].rot_tcb.size())
			rot2key(objlist[i].rot_tcb,part[i]);
		else {
			if (objlist[i].rot_euler_x.size())
				float2key(objlist[i].rot_euler_x,part[i],ROTX);
			if (objlist[i].rot_euler_y.size())
				float2key(objlist[i].rot_euler_y,part[i],ROTY);
			if (objlist[i].rot_euler_z.size())
				float2key(objlist[i].rot_euler_z,part[i],ROTZ);
		}
// scale
		if (objlist[i].scl_lin.size())
			scl2key(objlist[i].scl_lin,part[i]);
		else if (objlist[i].scl_bez.size())
			scl2key(objlist[i].scl_bez,part[i]);
		else if (objlist[i].scl_tcb.size())
			scl2key(objlist[i].scl_tcb,part[i]);
// vis
		if (objlist[i].vis_lin.size())
			float2key(objlist[i].vis_lin,part[i],EXTRA);
#endif
		if (part[i]->seqs) {
			part[i]->seqs->origendframe=part[i]->seqs->endframe;
		}
// tm
		part[i]->o2p=objlist[i].matrix;
		part[i]->trans=objlist[i].pos;
		part[i]->rot=objlist[i].rot;
		part[i]->scale=objlist[i].scl;
		U32 j;
		for (j=0;j<objlist[i].userprops.size();++j)
			part[i]->userprop.addscript(objlist[i].userprops[j].c_str());
	}
// handle userprops, except see 'lightmap' above, maybe should all be in one place
	for (i=0;i<static_cast<S32>(objlist.size());i++) {
		tree2* t=part[i];
		if (t->userprop.num()) {
			S32 j;
			logger("userprops for '%s'\n",t->name.c_str());
			for (j=0;j<t->userprop.num();++j) {
				logger(" '%s'\n",t->userprop.idx(j).c_str());
// see if texvels in userprop
				if (t->userprop.idx(j)=="texvel") {
					if (j+2<t->userprop.num()) {
						t->uvvel.x=(float)atof(t->userprop.idx(j+1).c_str());
						t->uvvel.y=(float)atof(t->userprop.idx(j+2).c_str());
					}
// nofog
				} else if (t->userprop.idx(j)=="nofog") {
					if (j<t->userprop.num()) {
						if (t->mod) {
							U32 k;
							for (k=0;k<t->mod->mats.size();++k)
								t->mod->mats[k].msflags|=SMAT_HASNOFOG;
						}
					}
// dissolvecutoff
				} else if (t->userprop.idx(j)=="dissolvecutoff") {
					if (j+1<t->userprop.num()) {
						t->treedissolvecutoff=(float)atof(t->userprop.idx(j+1).c_str());
					}
				}
			}
		}
	}
// lastly, build heirachy
	for (i=0;i<static_cast<S32>(objlist.size());i++) {
		S32 pid=objlist[i].pid;
		if (pid==-1)
			scene->linkchild(part[i]);
//			linkchild(part[i],scene);
		else
			part[pid]->linkchild(part[i]);
//			linkchild(part[i],part[pid]);
	}
	logger("end loadbws ------------------\n");
}
#endif
