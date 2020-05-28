#define D2_3D
#include <m_eng.h>
#include <m_perf.h>
#define LOADBWO
#define LOADBWS

ti treeinfo;

static const C8 *flagnames[]={
    "killmec",
    "dontdraw",
    "dontdrawc",
    "dontanimate",
    "dontanimatec",
    "iscamera",
    "isbone",
    "islight",
    "isscene",
    "isnull",
    "hasbones",
    "noinfluence",
    "isamblight",
	"castshadow",
	"pickable",
    "alwaysfacing",
//  "calclightsonce",
//  "donecalclightsonce",
//   "isbiped",
//   "mixlights",
//   "treecolor",
//   "cameraspace",
};
#define NFLAGNAMES (sizeof(flagnames)/sizeof(flagnames[0]))

static const C8* o2pnames[]={"none","euler","quat"};
#define NO2PNAMES (sizeof(o2pnames)/sizeof(o2pnames[0]))

#define MAXDRAWOBJS 20000
static tree2* drawtrees[MAXDRAWOBJS];

/*
static void buildworldmats(tree2* w)
{
	S32 i;
	if (w->buildo2p==O2P_FROMTRANSROTSCALE)
		buildscalerottrans3d(&w->scale,&w->rot,&w->trans,&w->o2p);
	else if (w->buildo2p==O2P_FROMTRANSQUATSCALE)
//		quatscalerottrans3d(w,&w->o2p);
		quatscalerottrans3d(&w->scale,&w->rot,&w->trans,&w->o2p);
//	checkmat(w);
	if (w->parent) {
		mulmat3d(&w->o2p,&w->parent->o2w,&w->o2w);
//		logger("build worldmat with parent\n");
//		logmat4(&w->o2p,"o2p");
//		logmat4(&w->parent->o2w,"parent o2w");
//		logmat4(&w->o2w,"o2w");
	} else {
//		logger("build worldmat with no parent\n");
//		logmat4(&w->o2p,"o2p");
		w->o2w=w->o2p;
	}
	S32 n=w->children.size();
	for (i=0;i<n;i++)
		buildworldmats(w->children[i]);
}
*/
tree2::tree2(const C8* s) :
	flags(0),
	userproc(0),
// orientation
	trans(zerov),
	rot(zerov),
	scale(pointf3x(1.0f,1.0f,1.0f,1.0f)),
//	scale(onev),
	transvel(zerov),
	rotvel(zerov),
	scalevel(pointf3x(1,1,1)),
// animation/keyframes
	doanim(true),
	seqs(0),
	nseq(0),
	repcount(1),currepcount(1), // stop
	curframe(0),curseq(0),
	playing(0),
	buildo2p(O2P_FROMTRANSROTSCALE),
//	playo2p(O2P_NONE),
// heirarchy
	parent(0),
// for lights
	lightcolor(F32WHITE),
	intensity(1),
//////////// for cameras
	zoom(2),
	isedge(false),
	srt_drawpri(1),
//////////// for objects
//	visible(1),
//	dissolve(1.0f),
	treecolor(F32WHITE), // copied to mat[0] if flags set with TF_TREECOLOR
	mod(0),
	alttex(0),
	treedissolvecutoff(treeinfo.defaultdissolvecutoff),
	uvoffset(pointf2x(0,0)),
	uvvel(pointf2x(0,0))
{
	if (s)
		name=s;
	memset(userint,0,sizeof(userint));
	memset(userfloat,0,sizeof(userfloat));
// more orientation
	identmat4(&o2p);
	identmat4(&o2w);
//	identmat4(&o2c);
#ifdef LOADBWO
	{
		C8 ext[50];
		if (mgetext(name.c_str(),ext,50))
			if (!my_stricmp(ext,"bwo"))
				if (fileexist(name.c_str()))
					setmodel(loadbwomodel(name.c_str()));
	}
#endif
#ifdef LOADBWS
	{
		C8 ext[50];
		if (mgetext(name.c_str(),ext,50))
			if (!my_stricmp(ext,"bws"))
				if (fileexist(name.c_str()))
					loadbws(this);
	}
#endif
}

// copy over ALL the animation data
void tree2::copyanim(const tree2* rhs)
{
	seqs=0; // no animation
#if 1
	S32 j;
	if (rhs->seqs && rhs->nseq > 0) {
		// copy animation
		// copy seqs (1 for now)
		seqs = new seq;
		const seq* rhsseqs = rhs->seqs;
		seqs->startframe = rhsseqs->startframe;
		seqs->endframe = rhsseqs->endframe;
		seqs->origendframe = rhsseqs->origendframe;
		for (j=0;j<NCHAN;++j) {
			const keyframe* rhsseqskeysn = rhsseqs->keysn[j];
			if (rhsseqskeysn) {
				// copy keyframes
				keyframe* lhsseqskeysn = new keyframe;
				seqs->keysn[j] = lhsseqskeysn;
				lhsseqskeysn->nkeys = rhsseqskeysn->nkeys;
				lhsseqskeysn->curkey = rhsseqskeysn->curkey;
				// copy keys
				if (rhsseqskeysn->nkeys) {
					lhsseqskeysn->keys = new key[rhsseqskeysn->nkeys];
					copy(&rhsseqskeysn->keys[0],&rhsseqskeysn->keys[rhsseqskeysn->nkeys],&lhsseqskeysn->keys[0]);
				} else {
					lhsseqskeysn->keys = 0;
				}
			} else {
				seqs->keysn[j] = 0; // no keyframes for this channel
			}
		}
	}

#endif
	nseq=rhs->nseq;
	curseq=rhs->curseq;
	currepcount=rhs->currepcount;
	repcount=rhs->repcount;
	curframe=rhs->curframe;
	playing=rhs->playing;
	doanim=rhs->doanim;
}

#if 0
	if (seqs) {
		S32 i;
		for (i=0;i<NCHAN;++i) {
			if (seqs->keysn[i]) {
				delete[] seqs->keysn[i]->keys;
				delete seqs->keysn[i];
			}
		}
		delete seqs;
	}
#endif

tree2* tree2::newdup()
{
	if (!this)
		return 0;
	tree2* r=new tree2("");
	r->name=name;
	r->flags=flags;
	r->userproc=userproc;
	memcpy(r->userfloat,userfloat,sizeof(userfloat));
	memcpy(r->userint,userint,sizeof(userint));
// orientation
	r->trans=trans;
	r->rot=rot;
	r->scale=scale;
	r->transvel=transvel;
	r->rotvel=rotvel;
	r->scalevel=scalevel;
	r->buildo2p=buildo2p;
//	r->playo2p=playo2p;
	r->o2p=o2p;
	r->o2w=o2w;
// animation
	r->copyanim(this);
// hierarchy
	r->parent=0;
////////////for lights
	r->lightcolor=lightcolor;
	r->intensity=intensity;
//////////// for cameras
	r->zoom=zoom;
	r->isedge=isedge;
	r->srt_drawpri=srt_drawpri;
//////////// for objects
//	r->visible=visible;
//	r->dissolve=dissolve;
	r->treecolor=treecolor; // copied to mat[0] if flags set with TF_TREECOLOR
	r->mod=modelb::rc.newrc(mod);
	r->alttex=textureb::rc.newrc(alttex);
	r->treedissolvecutoff=treedissolvecutoff;
	r->uvoffset=uvoffset;
	r->uvvel=uvvel;
// copy children too...
//	S32 i,n=children.size();
//	for (i=0;i<n;++i) {
//		tree2* newchild=children[i]->newdup();
	list<tree2*>::iterator i;
	for (i=children.begin();i!=children.end();++i) {
		tree2* newchild=(*i)->newdup();
		r->linkchild(newchild);
	}
	return r;
}

void tree2::linkchild(tree2* child)
{
	if (!this)
		errorexit("child '%s' parent is null\n",child->name.c_str());
	if (child->parent)
		errorexit("child '%s' already has a parent, '%s'\n",child->name.c_str(),child->parent->name.c_str());
	children.push_back(child);
	child->parent=this;
	child->parentit=children.end();
	--(child->parentit); // iterator that points to child just added to children
}

void tree2::unlink()
{
//	S32 i,n;
	if (!parent)
		errorexit("child '%s' has no parent to unlink\n",name.c_str());
//	n=parent->children.size();
//	for (i=0;i<n;++i)
//		if (parent->children[i]==this)
//			break;
//	if (i==n)
/*	list<tree2*>::iterator i;
	for (i=parent->children.begin();i!=parent->children.end();++i)
		if (*i == this)
			break;
	if (i==parent->children.end())
		errorexit("child '%s' doesn't belong to parent '%s'\n",name.c_str(),parent->name.c_str());
//	vector<tree2*>::iterator ti=parent->children.begin()+i; */
	list<tree2*>::iterator i=parentit; // this is the parent iterator you want to erase
	parent->children.erase(i);
	parent=0;
}

void tree2::proc()
{
//	static S32 level;
//	S32 i,n=children.size();
	if (buildo2p==O2P_FROMTRANSROTSCALE) {
		rot.x=normalangrad(rot.x+rotvel.x*wininfo.framestep);
		rot.y=normalangrad(rot.y+rotvel.y*wininfo.framestep);
		rot.z=normalangrad(rot.z+rotvel.z*wininfo.framestep);
	}
	trans.x+=transvel.x*wininfo.framestep;
	trans.y+=transvel.y*wininfo.framestep;
	trans.z+=transvel.z*wininfo.framestep;
	scale.x*=scalevel.x;
	scale.y*=scalevel.y;
	scale.z*=scalevel.z;
	seq_dostep(this);
//	if (!level && !parent) // doesn't work because proc may be called on many root trees per frame, see m_state.cpp
//			textureb::animtex();
//	for (i=0;i<n;) {
//		children[i]->proc();
//		S32 newn=children.size();
//		i+=1+newn-n;
//		n=newn;
	if (userproc) {
		if (!(*userproc)(this)) { // bail, delete occured
			if (parent)
				delete this;
			return;
		}
	}
	if (buildo2p==O2P_FROMTRANSROTSCALE)
		buildscalerottrans3d(&scale,&rot,&trans,&o2p);
	else if (buildo2p==O2P_FROMTRANSQUATSCALE)
		quatscalerottrans3d(&scale,&rot,&trans,&o2p);

// children last
	list<tree2*>::iterator i,n;
	for (i=children.begin();i!=children.end();i=n) {
		n=i;
		++n; // get to next
//		++level;
		(*i)->proc(); // proc may delete itself, good thing we saved the next before this call!
//		--level;
	}
}

tree2::~tree2()
{
	if (parent) {
		unlink();
//		errorexit("destructor: '%s' has a parent",name.c_str());
//		flags|=TF_KILLMEC;
//		return; // let parent delete
	}
// delete model
	modelb::rc.deleterc(mod);
// delete alt tex
	textureb::rc.deleterc(alttex);
// delete keyframes
	if (seqs) {
		S32 i;
		for (i=0;i<NCHAN;++i) {
			if (seqs->keysn[i]) {
				delete[] seqs->keysn[i]->keys;
				delete seqs->keysn[i];
			}
		}
		delete seqs;
	}
	if (flags&TF_ISLIGHT)
		removelightfromlist(this);
// delete children too
//	S32 i,n=children.size();
//	for (i=0;i<n;++i) {
//		children[i]->parent=0;
//		delete children[i];
	list<tree2*>::iterator i;
	for (i=children.begin();i!=children.end();++i) {
		(*i)->parent=0;
		delete (*i);
	}
}

void tree2::log2()
{
//	S32 i,j,n=children.size();
	S32 i,j;
    logger_indent();
    logger_disableindent();
    logger("'%s'",name.c_str());
	logger(", o2p '%s', drawpri %d ",o2pnames[buildo2p],srt_drawpri);
	logger("dco %f ",treedissolvecutoff);
	if (alttex)
		logger("alttex '%s', ",alttex->name.c_str());
    if (flags) {
        logger(", flags ");
        for (i=0,j=1;i<(signed)NFLAGNAMES;++i,j<<=1)
            if (flags&j)
                logger("'%s' ",flagnames[i]);
    }
//	logmat4(&o2p,"o2p");
//	logmat4(&o2w,"o2w");
	if (buildo2p!=O2P_NONE) {
		logger("prs (%f %f %f),(%f %f %f %f),(%f %f %f) ",
			trans.x,trans.y,trans.z,rot.x,rot.y,rot.z,rot.w,scale.x,scale.y,scale.z);
	} else {
		logger("matrix (%f %f %f %f),(%f %f %f %f),(%f %f %f %f),(%f %f %f %f) ",
			o2p.e[0][0],o2p.e[0][1],o2p.e[0][2],o2p.e[0][3],
			o2p.e[1][0],o2p.e[1][1],o2p.e[1][2],o2p.e[1][3],
			o2p.e[2][0],o2p.e[2][1],o2p.e[2][2],o2p.e[2][3],
			o2p.e[3][0],o2p.e[3][1],o2p.e[3][2],o2p.e[3][3]);
	}
	if (mod)
        logger(", modelname '%s'",mod->name.c_str());
	if (seqs && nseq>0) {
		logger(", nseq %d, startframe %6.1f, origendframe %6.1f, endframe %6.1f",nseq,seqs[0].startframe,seqs[0].origendframe,seqs[0].endframe);
	}
    logger("\n");
	logger_enableindent();
//	for (i=0;i<n;++i)
//		children[i]->log2();
	list<tree2*>::iterator it;
	for (it=children.begin();it!=children.end();++it)
		(*it)->log2();
    logger_unindent();
}

void tree2::setmodel(modelb* m)
{
	modelb::rc.deleterc(mod);
	mod=m;
}

// drawpri's
// 0 // cool blur, NYI
// 1 // skybox
// 2 // opaque
// 3 // alpha

void video_buildworldmats(tree2* t)
{
	static S32 level;
//	if (!videoinfo.didbuildworldmats) {
		if (t->buildo2p==O2P_FROMTRANSROTSCALE)
			buildscalerottrans3d(&t->scale,&t->rot,&t->trans,&t->o2p);
		else if (t->buildo2p==O2P_FROMTRANSQUATSCALE)
			quatscalerottrans3d(&t->scale,&t->rot,&t->trans,&t->o2p);
		if (t->parent) {
			mulmat3d(&t->o2p,&t->parent->o2w,&t->o2w);
//			logger("build worldmat with parent\n");
//			logmat4(&w->o2p,"o2p");
//			logmat4(&w->parent->o2w,"parent o2w");
//			logmat4(&w->o2w,"o2w");
		} else {
//			logger("build worldmat with no parent\n");
//			logmat4(&w->o2p,"o2p");
			t->o2w=t->o2p;
		}
//	}
	if ((t->flags&TF_DONTDRAWC)==0) {
//		S32 i,n=t->children.size();
//		for (i=0;i<n;++i)
//			video_buildtreelist(t->children[i]);
		list<tree2*>::iterator i;
		for (i=t->children.begin();i!=t->children.end();++i) {
			++level;
			video_buildworldmats(*i);
			--level;
		}
	}
//	if (!level)
//		videoinfo.didbuildworldmats=true;
}

// build world mats up the chain
void video_buildworldmatearly(const tree2* t)
{
	if (t->buildo2p==O2P_FROMTRANSROTSCALE)
		buildscalerottrans3d(&t->scale,&t->rot,&t->trans,&t->o2p);
	else if (t->buildo2p==O2P_FROMTRANSQUATSCALE)
		quatscalerottrans3d(&t->scale,&t->rot,&t->trans,&t->o2p);
	if (t->parent) {
		video_buildworldmatearly(t->parent);
		mulmat3d(&t->o2p,&t->parent->o2w,&t->o2w);
	} else {
		t->o2w=t->o2p;
	}
}

void obj2world(const tree2* t, const pointf3* in, pointf3* out)
{
	video_buildworldmatearly(t);
	xformvec(&t->o2w, in, out);
}

void obj2worldn(const tree2* t, const pointf3* in, pointf3* out, U32 nv)
{
	video_buildworldmatearly(t);
	xformvecs(&t->o2w, in, out, nv);
}

void world2obj(const tree2* t, const pointf3* in, pointf3* out)
{
	video_buildworldmatearly(t);
	mat4 w2o;
	inversemat3d(&t->o2w, &w2o);
	xformvec(&w2o, in, out);
}

void world2objn(const tree2* t, const pointf3* in, pointf3* out, U32 nv)
{
	video_buildworldmatearly(t);
	mat4 w2o;
	inversemat3d(&t->o2w, &w2o);
	xformvecs(&w2o, in, out, nv);
}


// build all world mats in the hiearchy
void video_buildtreelist(tree2* t)
{
//	if (!videoinfo.didbuildworldmats) {
//		if (t->parent) {
//			mulmat3d(&t->o2p,&t->parent->o2w,&t->o2w);
//			logger("build worldmat with parent\n");
//			logmat4(&w->o2p,"o2p");
//			logmat4(&w->parent->o2w,"parent o2w");
//			logmat4(&w->o2w,"o2w");
//		} else {
//			logger("build worldmat with no parent\n");
//			logmat4(&w->o2p,"o2p");
//			t->o2w=t->o2p;
//		}
//	}
	if ((t->flags&TF_DONTDRAWC)==0) {
//		S32 i,n=t->children.size();
//		for (i=0;i<n;++i)
//			video_buildtreelist(t->children[i]);
		list<tree2*>::iterator i;
		for (i=t->children.begin();i!=t->children.end();++i)
			video_buildtreelist(*i);
	}
	if (t->flags&(TF_DONTDRAW|TF_DONTDRAWC))
		return;
	if (video3dinfo.inshadowdraw && !(t->flags&TF_CASTSHADOW))
		return;
	modelb* m=t->mod;
	if (!m)
		return;
	S32 nm = m->mats.size();
	if (!nm)
		return;
	mater2* mt2=&m->mats[0];
	if (!m->faces.size() && !(mt2->msflags&(SMAT_POINTS|SMAT_LINES)))
		return; // don't draw meshes with 0 faces, unless it's a point cloud or line web
	if (video3dinfo.inshadowdraw && (mt2->msflags&SMAT_ISSKY))
		return;
	float dco = treeinfo.treedissolvecutoffenable ? t->treedissolvecutoff : treeinfo.defaultdissolvecutoff;
	if (t->treecolor.w<dco) // don't draw almost xpar objects past alpha cutoff
		return;
//	mulmat3d(&t->o2w,w2c,&t->o2c);
	S32 i;
	t->srt_drawpri=2; // assume opaque
	for (i=0;i<1;++i,++mt2) {
//	for (i=0;i<nm;++i,++mt2) {
		if (mt2->msflags&SMAT_ISSKY) {
			t->srt_drawpri=1; // skybox
			break;
		}
		if (!(mt2->msflags&SMAT_HASWBUFF)) {
			t->srt_drawpri=4; // no zbuff
			break;
		}
		if (t->isedge) {
			t->srt_drawpri=4; // edges come after alpha
			break;
		}
		if (mt2->msflags&SMAT_SHADOW) {
			t->srt_drawpri=5; // shadows come after everything else
			break;
		}
		float dc=range(0.0f,t->treecolor.w,1.0f)*mt2->color.w;
		bool hb = (mt2->thetexarr[0] && mt2->thetexarr[0]->hasalpha) || dc<treeinfo.dissolvestart;
		if (hb) {
			t->srt_drawpri=3; // alpha
			//return;
			break;
		} else {
		}
	}
//	if (t->srt_drawpri!=4)
//		return;
	if (t->srt_drawpri==1)
		mulmat4(&t->o2w,&video3dinfo.w2c_sky,&t->srt_o2c);
	else
		mulmat4(&t->o2w,&video3dinfo.w2c,&t->srt_o2c);
	t->srt_clipresult=t->mod->checkbbox(&t->srt_o2c);
	if (t->srt_clipresult==CLIP_OUT)
		return;
//	if (t->srt_clipresult==CLIP_CLIP)
//		return;
	if (treeinfo.ndrawtree>=MAXDRAWOBJS)
		errorexit("too many drawtrees");
	drawtrees[treeinfo.ndrawtree++]=t;
}
/*
	drawpris
	1	skybox
	2	opaque
	3	alpha
	4	nozbuff
*/

// decreasing z, but skybox must be first
/* make drawpris, all decreasing z
	1 skybox
	2 opaque,alpha
	3 nozbuff */
static bool srt_z_nozbuff(tree2* a,tree2* b)
{
	S32 ap=a->srt_drawpri;
	if (ap==4)
		ap=3;
	else if (ap==3)
		ap=2;
	S32 bp=b->srt_drawpri;
	if (bp>=4)
		bp=3;
	else if (bp==3)
		bp=2;
	S32 res=ap-bp;
	if (res)
		return res<0;
	return a->srt_o2c.e[3][2]>b->srt_o2c.e[3][2];
}

/*
	drawpris
	1	skybox dec z
	2	opaque dec z
	3	alpha  inc z
	4	nozbuff inc z
*/
// increasing z for opaque, decreasing z for alpha, skybox first (assumed to be a z=0)
static bool srt_z(tree2* a,tree2* b)
{
	S32 ap=a->srt_drawpri;
	S32 bp=b->srt_drawpri;
	S32 res=ap-bp;
	if (res)
		return res<0;
	if (ap<3) // ap==1 or ap==2 // opaque
		return a->srt_o2c.e[3][2]<b->srt_o2c.e[3][2];
	else // ap==3 or 4 // alpha
		return a->srt_o2c.e[3][2]>b->srt_o2c.e[3][2];
}

void video_sorttreelist()
{
	if (video3dinfo.inshadowdraw)
		return;
	if (!video3dinfo.zenable)
		sort(drawtrees,drawtrees+treeinfo.ndrawtree,srt_z_nozbuff);
	else
		sort(drawtrees,drawtrees+treeinfo.ndrawtree,srt_z);
}

void video_drawtreelist()
{
	S32 i;//,j,dp;
	tree2* t;
//	for (j=0;j<4;j++) {
//		if (treeinfo.showpri>=0 && treeinfo.showpri<4 && treeinfo.showpri!=j)
//			continue;
		for (i=0;i<treeinfo.ndrawtree;i++) {
			t=drawtrees[i];
//			dp=t->mod->getdrawpri();
//			if (t->dissolve<=.99f)
//				dp=3;
//			if (dp!=j)
//				continue;
//			pointf3* tc;
//			if (t->flags&TF_TREECOLOR)
//			tc=&t->treecolor;
//			else
//				tc=0;
//			mat4 to2c;
//			mulmat4(&t->o2w,&video3dinfo.w2c,&to2c);
			t->mod->draw(&t->o2w,&t->srt_o2c,&t->treecolor,t->treedissolvecutoff,t->alttex,&t->uvoffset,t->srt_clipresult); // o2w is for lights
		}
//	}
}

void setcastshadow(tree2* t)
{
	t->flags|=TF_CASTSHADOW;
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		setcastshadow(*i);
}

tree2* tree2::find(const C8* fname)
{
	static U32 level;
	if (name==fname)
		return this;
	list<tree2*>::iterator i;
	++level;
	for (i=children.begin();i!=children.end();++i) {
		tree2* r=(*i)->find(fname);
		if (r) {
			--level;
			return r;
		}
	}
	--level;
//	if (!level)
//		errorexit("tree2::find, can't find '%s'",fname);
	return 0;
}

tree2* tree2::findsome(const C8* fname)
{
	static U32 level;
	if (name.find(fname)!=string::npos)
		return this;
	list<tree2*>::iterator i;
	++level;
	for (i=children.begin();i!=children.end();++i) {
		tree2* r=(*i)->findsome(fname);
		if (r) {
			--level;
			return r;
		}
	}
	--level;
//	if (!level)
//		errorexit("tree2::find, can't find '%s'",fname);
	return 0;
}

mater2* tree2::findmaterial(const C8* name)
{
	if (!mod) {
//		errorexit("tree2::findmaterial('%s'): no mod",name);
		return 0;
	}
	U32 i,n=mod->mats.size();
	for (i=0;i<n;++i)
		if (mod->mats[i].name==name)
			return &mod->mats[i];
//	errorexit("tree2::findmaterial('%s'): can't find",name);
	return 0;
}

///////////// for reference ////////////////////////
#if 0
void video_drawtreelist()
{
	S32 i,j,dp;
	tree2* t;
//	pointf3* tempvert;
//	pointf3 bboxsavemin,bboxsavemax;
	for (j=0;j<4;j++) {
		if (treeinfo.showpri>=0 && treeinfo.showpri<4 && treeinfo.showpri!=j)
			continue;
		for (i=0;i<treeinfo.ndrawtree;i++) {
			t=drawtrees[i];
			dp=t->mod->getdrawpri();
			if (t->dissolve<=.99f)
				dp=3;
			if (dp!=j)
				continue;
#if 0
			if (t->flags&TF_HASBONES) {
				S32 clipcode;
	//			mulmat3d(&t->o2w,&curvp->w2c,&t->o2c);
				tempvert=t->mod->verts;
				t->mod->verts=t->deformverts;
				bboxsavemin=t->mod->boxmin;
				bboxsavemax=t->mod->boxmax;
				setmodelbbox(t->mod);
				registermodelcverts(t->cverts);
#if 0
				if (video_maindriver==VIDEO_D3D) {
					alphacutoff=t->treealphacutoff;
					registermodeld3dverts(t->d3dverts);
				}
#endif
//				video_drawmodel(t->mod,curvp,&o2c,t->dissolve,&t->o2w,t->zoff); // worry about drawpass later...
				clipcode=video_drawmodel(t,curvp,&t->o2c); // worry about drawpass later...
				if (!clipcode && treeinfo.speedupdeformobjects)
					setdontdrawhasbones(t);
	//			deformvert=t->mod->verts;
				t->mod->verts=tempvert;
				t->mod->boxmin=bboxsavemin;
				t->mod->boxmax=bboxsavemax;
			} else {
#endif
//				mulmat3d(&t->o2w,&curvp->w2c,&t->o2c);
//				registermodelcverts(t->cverts);
#if 0
				if (video_maindriver==VIDEO_D3D) {
					alphacutoff=t->treealphacutoff;
					registermodeld3dverts(t->d3dverts);
				}
#endif
				pointf3* tc;
				if (t->flags&TF_TREECOLOR)
					tc=&t->treecolor;
				else
					tc=0;
				t->mod->draw(&t->o2c,&t->o2w,t->dissolve,tc); // o2w is for lights
//				video_drawmodel(t,vp,&t->o2c); // worry about drawpass later...
	//			video_drawmodel(t->mod,curvp,&o2c,t->dissolve,&t->o2w,t->zoff); // worry about drawpass later...
//			}
		}
	}
}
#endif

static string sglm;
void setsuggestlightmap(string s)
{
	sglm=s;
}

string getsuggestlightmap()
{
	return sglm;
}

void getlookat(const pointf3* from,const pointf3* to,pointf3* rpy) // look at 'to' from 'from'
{
	rpy->z = 0; // roll
	pointf3x d(to->x-from->x,to->y-from->y,to->z-from->z);
	float len = sqrtf(d.x*d.x+d.z*d.z);
	if (len<EPSILON) { // camera directly over or under car
		if (d.y>=0) {
			rpy->x = -PI/2.0f; // look up
		} else {
			rpy->x = PI/2.0f; // look down
		}
		rpy->y = 0; // yaw

	} else {
		rpy->x = atan2f(-d.y,len); // pitch
		rpy->y = atan2f(d.x,d.z); // yaw
	}
}
