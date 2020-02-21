#define D2_3D
#include <m_eng.h>
// test keyframes manually
struct kt keytester={0,0,1};

////// tension continuity bias section
// used with bez also
static float tcbcalckeyvalue(struct keyframe *kf,float time)
{
	int i;
	int nkeys=kf->nkeys;
	struct key *keys=kf->keys;
	if (time<=keys[0].time) {
		kf->curkey=0;
		return keys[0].value;
	}
	if (time>=keys[nkeys-1].time) {
		kf->curkey=nkeys-1;
		return keys[nkeys-1].value;
	}
// go down
	i=kf->curkey;
	if (time<keys[i].time) {
		do
			--i;
		 while (time<keys[i].time);
// go up
	} else {
		do
			++i;
		 while (time>keys[i].time);
		--i;
	}
	kf->curkey=i;
	time-=keys[i].time;
	return ((keys[i].ca * time + keys[i].cb) * time + keys[i].cc) * time + keys[i].cd;
}

static void tcbkeyframeproc(class tree2* t)
{
	if (!t->seqs)
        return;
	struct seq *s;
	float cf;
//	int lastkey;
	if (keytester.manualframe)
		t->curframe=keytester.framenum; // test
	s=&t->seqs[t->curseq];
/*	if (s->keys[TRANSX] && s->keys[TRANSX]->endstate==END_REP) {
		lastkey=s->keys[TRANSX]->nkeys-1;
		lasttime=s->keys[TRANSX]->keys[lastkey].time;
//		cf=t->curframe;
		if (lasttime>0)
			cf=(float)fmod(t->curframe-.01f,lasttime)+.01f;
		else
			cf=0;
	} else */
		cf=t->curframe;
	if (s->keysn[TRANSX])
		t->trans.x=tcbcalckeyvalue(s->keysn[TRANSX],cf);
	if (s->keysn[TRANSY])
		t->trans.y=tcbcalckeyvalue(s->keysn[TRANSY],cf);
	if (s->keysn[TRANSZ])
		t->trans.z=tcbcalckeyvalue(s->keysn[TRANSZ],cf);
	if (s->keysn[ROTX])
		t->rot.x=tcbcalckeyvalue(s->keysn[ROTX],cf);
	if (s->keysn[ROTY])
		t->rot.y=tcbcalckeyvalue(s->keysn[ROTY],cf);
	if (s->keysn[ROTZ])
		t->rot.z=tcbcalckeyvalue(s->keysn[ROTZ],cf);
	if (s->keysn[SCLX])
		t->scale.x=tcbcalckeyvalue(s->keysn[SCLX],cf);
	if (s->keysn[SCLY])
		t->scale.y=tcbcalckeyvalue(s->keysn[SCLY],cf);
	if (s->keysn[SCLZ])
		t->scale.z=tcbcalckeyvalue(s->keysn[SCLZ],cf);
/*	if (t->morphing>=0.0f && t->morph>0.0f) {
		float wa,wb,r;
		wb=t->morphing/t->morph;
		nanerr("morphing ?",wb);
		wa=1-wb;
		t->trans.x=wa*t->trans.x+wb*t->morphtrans.x;
		t->trans.y=wa*t->trans.y+wb*t->morphtrans.y;
		t->trans.z=wa*t->trans.z+wb*t->morphtrans.z;
//		t->scale.x=wa*t->scale.x+wb*t->morphscale.x;
//		t->scale.y=wa*t->scale.y+wb*t->morphscale.y;
//		t->scale.z=wa*t->scale.z+wb*t->morphscale.z;
		r=t->rot.x-t->morphrot.x;
		nanerr("r1",r);
		if (r>PI)
			r-=TWOPI;
		if (r<-PI)
			r+=TWOPI;
		t->rot.x=wa*r+t->morphrot.x;
		r=t->rot.y-t->morphrot.y;
		nanerr("r2",r);
		if (r>PI)
			r-=TWOPI;
		if (r<-PI)
			r+=TWOPI;
		t->rot.y=wa*r+t->morphrot.y;
		r=t->rot.z-t->morphrot.z;
		nanerr("r3",r);
		if (r>PI)
			r-=TWOPI;
		if (r<-PI)
			r+=TWOPI;
		t->rot.z=wa*r+t->morphrot.z;
		if (t->playing)
			t->morphing-=framestep;
	}
*/

/*	if (s->keys[EXTRA]) {
		if (t->flags&TF_ISCAMERA)
			t->zoom=tcbcalckeyvalue(s->keys[EXTRA],cf);
		else if (t->flags&TF_ISLIGHT)
			t->intensity=tcbcalckeyvalue(s->keys[EXTRA],cf);
		else
			t->treecolor.w=tcbcalckeyvalue(s->keys[EXTRA],cf);
	}
*/
/*	if (t->playing) {
		t->curframe+=wininfo.framestep;
		if (t->curframe>=s->endframe) {
//			if(t->endproc) t->endproc(t);
			if (t->currepcount>0) {
				t->currepcount--;
				if (t->currepcount==0) {
					t->curframe=s->endframe;
					t->playing=0;
					return;
				}
			}
			t->curframe-=s->endframe;
			t->curframe+=s->startframe;
		}
	} */
//	t->trans.x+=.1f;
//	py=calckeyvalue(&testkeyframe,px);
}

static void extrakeyframeproc(class tree2* t)
{
	if (!t->seqs)
        return;
	struct seq *s;
//	int lastkey;
	if (keytester.manualframe)
		t->curframe=keytester.framenum; // test
	s=&t->seqs[t->curseq];
	if (s->keysn[EXTRA]) {
		float cf;//,lasttime;
		cf=t->curframe;
		if (t->flags&TF_ISCAMERA)
			t->zoom=tcbcalckeyvalue(s->keysn[EXTRA],cf);
		else if (t->flags&TF_ISLIGHT)
			t->intensity=tcbcalckeyvalue(s->keysn[EXTRA],cf);
		else
			t->treecolor.w=tcbcalckeyvalue(s->keysn[EXTRA],cf);
	}
}

static void quatcalckeyvalue(struct keyframe *kfx,struct keyframe *kfy,struct keyframe *kfz,struct keyframe *kfw,float time,struct pointf3 *rq)
{
	int i;
	int nkeys=kfx->nkeys;
	float t;
	struct pointf3 qa,qb;
	struct key *keysx=kfx->keys;
	struct key *keysy=kfy->keys;
	struct key *keysz=kfz->keys;
	struct key *keysw=kfw->keys;
	if (time<=keysx[0].time) {
		kfx->curkey=0;
		rq->x=keysx[0].value;
		rq->y=keysy[0].value;
		rq->z=keysz[0].value;
		rq->w=keysw[0].value;
		return;
	}
	if (time>=keysx[nkeys-1].time) {
		kfx->curkey=nkeys-1;
		rq->x=keysx[nkeys-1].value;
		rq->y=keysy[nkeys-1].value;
		rq->z=keysz[nkeys-1].value;
		rq->w=keysw[nkeys-1].value;
		return;
	}
// go down
	i=kfx->curkey;
	if (time<keysx[i].time) {
		do
			i--;
		 while (time<keysx[i].time);
// go up
	} else {
		do
			i++;
		 while (time>keysx[i].time);
		i--;
	}
	kfx->curkey=i;
	t=(time-keysx[i].time)/(keysx[i+1].time-keysx[i].time);
	qa.x=keysx[i].value;
	qa.y=keysy[i].value;
	qa.z=keysz[i].value;
	qa.w=keysw[i].value;
	qb.x=keysx[i+1].value;
	qb.y=keysy[i+1].value;
	qb.z=keysz[i+1].value;
	qb.w=keysw[i+1].value;
	quatinterp(&qa,&qb,t,rq);
//	*rq=qa;
}

static void quatkeyframeproc(class tree2* t)
{
	if (!t->seqs)
        return;
	struct seq *s;
	float cf;
	if (keytester.manualframe)
		t->curframe=keytester.framenum; // test
	s=&t->seqs[t->curseq];
	cf=t->curframe;
	if (s->keysn[TRANSX])
		t->trans.x=/*t->newtrans.x=*/tcbcalckeyvalue(s->keysn[TRANSX],cf);
//	else
//		t->trans.x=0;
	if (s->keysn[TRANSY])
		t->trans.y=/*t->newtrans.y=*/tcbcalckeyvalue(s->keysn[TRANSY],cf);
//	else
//		t->trans.y=0;
	if (s->keysn[TRANSZ])
		t->trans.z=/*t->newtrans.z=*/tcbcalckeyvalue(s->keysn[TRANSZ],cf);
//	else
//		t->trans.z=0;
	if (s->keysn[SCLX])
		t->scale.x=/*t->newscale.x=*/tcbcalckeyvalue(s->keysn[SCLX],cf);
//	else
//		t->scale.x=1;
	if (s->keysn[SCLY])
		t->scale.y=/*t->newscale.y=*/tcbcalckeyvalue(s->keysn[SCLY],cf);
//	else
//		t->scale.y=1;
	if (s->keysn[SCLZ])
		t->scale.z=/*t->newscale.z=*/tcbcalckeyvalue(s->keysn[SCLZ],cf);
//	else
//		t->scale.z=1;
	if (s->keysn[ROTW]) {
		quatcalckeyvalue(s->keysn[ROTX],s->keysn[ROTY],s->keysn[ROTZ],s->keysn[ROTW],cf,&t->rot);

/*
	if (s->keys[EXTRA]) {
		if (t->flags&TF_ISCAMERA)
			t->zoom=tcbcalckeyvalue(s->keys[EXTRA],cf);
		else if (t->flags&TF_ISLIGHT)
			t->intensity=tcbcalckeyvalue(s->keys[EXTRA],cf);
		else
			t->treecolor.w=tcbcalckeyvalue(s->keys[EXTRA],cf);
	} 
*/
//		t->newrot=t->rot;
//	} else {
//		t->rot.x=t->rot.y=t->rot.z=0;
//		t->rot.w=1;
//		t->newrot=t->rot;
	}

/*	if (t->morphing>=0.0f && t->morph>0.0f) {
		float wa,wb;
		wb=t->morphing/t->morph;
		wa=1-wb;
		t->trans.x=wa*t->newtrans.x+wb*t->morphtrans.x;
		t->trans.y=wa*t->newtrans.y+wb*t->morphtrans.y;
		t->trans.z=wa*t->newtrans.z+wb*t->morphtrans.z;
//		if (s->keys[ROTW])
			quatinterp(&t->newrot,&t->morphrot,wb,&t->rot);
		if (t->playing)
			t->morphing-=framestep;
	}
*/
/*	if (t->playing) {
		t->curframe+=wininfo.framestep;
		if (t->curframe>=s->endframe) {
//			if(t->endproc) t->endproc(t);
			if (t->currepcount>0) {
				t->currepcount--;
				if (t->currepcount==0) {
					t->curframe=s->endframe;
					t->playing=0;
					return;
				}
			}
			t->curframe-=s->endframe;
			t->curframe+=s->startframe;
		}
	} */
//	t->trans.x+=.1f;
//	py=calckeyvalue(&testkeyframe,px);
}

/*struct key deftestkeys[]={
	{400,100,0,1},
	{450,-300},
	{500,100},
	{550,-300},
	{575,-300},
	{600,100,0,1},
};

struct keyframe deftestkeyframe={
	0,6,1,0,deftestkeys,
};
*/
static void tcbcalccoeffs(struct key *k,float sn,float snp1)
{
	float e,f,g,h;
	float m0,m1,u1;
	u1=k[1].time-k[0].time;
	m0=u1*sn;
	m1=u1*snp1;
	g=m0;
	h=k[0].value;
	f=3*k[1].value-m1-2*g-3*h;
	e=m1-2*k[1].value+g+2*h;
	k->ca=e/(u1*u1*u1);
	k->cb=f/(u1*u1);
	k->cc=g/u1;
	k->cd=h;
}

static void calclincoeffs(struct key *k,float m)
{
	k->ca=0;
	k->cb=0;
	k->cc=m;
	k->cd=k[0].value;
}

// i'm not sure how to do variable width keyframes
static float gettcbslopeleft(float psx,float psy,float nsx,float nsy,float t,float c,float b)
{
	float /*mp,mn,m,*/x,y;
//	mp=psy/psx;
//	mn=nsy/nsx;
//	m=nsx/(psx+nsx)*(1-t)*((1+c)*(1-b)*mn+(1-c)*(1+b)*mp);
//	m=.5f*(1-t)*((1+c)*(1-b)*mn+(1-c)*(1+b)*mp);
//	return m;
	x=(1+c)*(1-b)*nsx+(1-c)*(1+b)*psx;
	y=(1-t)*((1+c)*(1-b)*nsy+(1-c)*(1+b)*psy);
	return y/x;
}

static float gettcbsloperight(float psx,float psy,float nsx,float nsy,float t,float c,float b)
{
	float /*mp,mn,m,*/x,y;
//	mp=psy/psx;
//	mn=nsy/nsx;
//	m=psx/(psx+nsx)*(1-t)*((1-c)*(1-b)*mn+(1+c)*(1+b)*mp);
//	m=.5f*(1-t)*((1-c)*(1-b)*mn+(1+c)*(1+b)*mp);
//	m=psx/(psx+nsx)*(1-t)*((1-c)*(1-b)*mn+(1+c)*(1+b)*mp);
//	return m;
	x=(1-c)*(1-b)*nsx+(1+c)*(1+b)*psx;
	y=(1-t)*((1-c)*(1-b)*nsy+(1+c)*(1+b)*psy);
	return y/x;
}

void tcbcalcallcoeffs(struct keyframe *kf)
{
	int i;
	float sn,snp1;
	float mxp,myp,mxn,myn;
	int nkeys=kf->nkeys;
	struct key *keys=kf->keys;
// 1 key
	kf->curkey=0;
	if (nkeys==1)
		return;
// only 2 keys
	if (nkeys==2) {
		if (keys[1].linear) {
			mxp=keys[1].time-keys[0].time;
			myp=keys[1].value-keys[0].value;
			calclincoeffs(&kf->keys[0],myp/mxp);
		} else {
			mxp=keys[1].time-keys[0].time;
			myp=keys[1].value-keys[0].value;
			mxn=keys[1].time-keys[0].time;
			myn=keys[1].value-keys[0].value;
			sn=gettcbsloperight(mxp,myp,mxn,myn,keys[0].tens,0,0);
			mxp=keys[1].time-keys[0].time;
			myp=keys[1].value-keys[0].value;
			mxn=keys[1].time-keys[0].time;
			myn=keys[1].value-keys[0].value;
			snp1=gettcbslopeleft(mxp,myp,mxn,myn,keys[1].tens,0,0);
	//		sn=(keys[1].value-keys[0].value)/(keys[1].time-keys[0].time);
	//		snp1=(keys[1].value-keys[0].value)/(keys[1].time-keys[0].time);
			tcbcalccoeffs(&kf->keys[0],sn,snp1);
		}
		return;
	}
// keyframe 0 of 3 or more keys
	if (keys[1].linear) {
		mxp=keys[1].time-keys[0].time;
		myp=keys[1].value-keys[0].value;
		calclincoeffs(&kf->keys[0],myp/mxp);
	} else {
		mxp=keys[1].time-keys[0].time;
		myp=keys[1].value-keys[0].value;
		mxn=keys[1].time-keys[0].time;
		myn=keys[1].value-keys[0].value;
		sn=gettcbsloperight(mxp,myp,mxn,myn,keys[0].tens,0,0);
		mxp=keys[1].time-keys[0].time;
		myp=keys[1].value-keys[0].value;
		mxn=keys[2].time-keys[1].time;
		myn=keys[2].value-keys[1].value;
		snp1=gettcbslopeleft(mxp,myp,mxn,myn,keys[1].tens,keys[1].cont,keys[1].bias);
	//	sn=(keys[1].value-keys[0].value)/(keys[1].time-keys[0].time);
	//	snp1=(keys[2].value-keys[0].value)/(keys[2].time-keys[0].time);
		tcbcalccoeffs(&kf->keys[0],sn,snp1);
	}
// keyframe 1 to n-3 of 4 or more keys
	for (i=1;i<kf->nkeys-2;i++) {
		if (keys[i+1].linear) {
			mxp=keys[i+1].time-keys[i].time;
			myp=keys[i+1].value-keys[i].value;
			calclincoeffs(&kf->keys[i],myp/mxp);
		} else {
			mxp=keys[i].time-keys[i-1].time;
			myp=keys[i].value-keys[i-1].value;
			mxn=keys[i+1].time-keys[i].time;
			myn=keys[i+1].value-keys[i].value;
			sn=gettcbsloperight(mxp,myp,mxn,myn,keys[i].tens,keys[i].cont,keys[i].bias);
	//		sn=(keys[i+1].value-keys[i-1].value)/(keys[i+1].time-keys[i-1].time);
			mxp=keys[i+1].time-keys[i].time;
			myp=keys[i+1].value-keys[i].value;
			mxn=keys[i+2].time-keys[i+1].time;
			myn=keys[i+2].value-keys[i+1].value;
			snp1=gettcbslopeleft(mxp,myp,mxn,myn,keys[i+1].tens,keys[i+1].cont,keys[i+1].bias);
	//		snp1=(keys[i+2].value-keys[i].value)/(keys[i+2].time-keys[i].time);
			tcbcalccoeffs(&kf->keys[i],sn,snp1);
		}
	}
// keyframe n-2 of n keys, 3 or more keys
	if (keys[nkeys-1].linear) {
		mxp=keys[nkeys-1].time-keys[nkeys-2].time;
		myp=keys[nkeys-1].value-keys[nkeys-2].value;
		calclincoeffs(&kf->keys[nkeys-2],myp/mxp);
	} else {
		mxp=keys[nkeys-2].time-keys[nkeys-3].time;
		myp=keys[nkeys-2].value-keys[nkeys-3].value;
		mxn=keys[nkeys-1].time-keys[nkeys-2].time;
		myn=keys[nkeys-1].value-keys[nkeys-2].value;
	//	sn=(keys[nkeys-1].value-keys[nkeys-3].value)/(keys[nkeys-1].time-keys[nkeys-3].time);
		sn=gettcbsloperight(mxp,myp,mxn,myn,keys[nkeys-2].tens,keys[nkeys-2].cont,keys[nkeys-2].bias);
		mxp=keys[nkeys-1].time-keys[nkeys-2].time;
		myp=keys[nkeys-1].value-keys[nkeys-2].value;
		mxn=keys[nkeys-1].time-keys[nkeys-2].time;
		myn=keys[nkeys-1].value-keys[nkeys-2].value;
	//	snp1=(keys[nkeys-1].value-keys[nkeys-2].value)/(keys[nkeys-1].time-keys[nkeys-2].time);
		snp1=gettcbslopeleft(mxp,myp,mxn,myn,keys[nkeys-1].tens,0,0);
		tcbcalccoeffs(&kf->keys[nkeys-2],sn,snp1);
	}
// keyframe n-1 not needed (except for the time and value)
}

static void bezcalccoeffs(struct key *k)
{
	float d=k[1].time-k[0].time;
	float v=k[1].value-k[0].value;
	float s1=k[0].intan;
	float s2=k[1].intan;
	k->ca=(d*(s2+s1)-2*v)/(d*d*d);
	k->cb=(3*v-d*(2*s1+s2))/(d*d);
	k->cc=s1;
	k->cd=k[0].value;
}

void bezcalcallcoeffs(struct keyframe *kf)
{
	int i;
// 1 key
	kf->curkey=0;
	if (kf->nkeys<=1)
		return;
// 2 or more keys
/// keyframe 0 to n-2 of 2 or more keys
	for (i=0;i<kf->nkeys-1;i++)
		bezcalccoeffs(&kf->keys[i]);
// keyframe n-1 not needed (except for the time and value)
}

// step all tree related animation, pos, rot ,scale, extra, texvel (maybe animtex, see ?)
void seq_dostep(class tree2* t)
{
// first handle texvels
//	t->uvoffset.x += t->uvvel.x*wininfo.framestep/30.0f; // 3dsmax fps is 30, maybe make a symbol for it
//	t->uvoffset.y += t->uvvel.y*wininfo.framestep/30.0f;
	t->uvoffset.x += t->uvvel.x*wininfo.framestep; // 3dsmax fps is 30, maybe make a symbol for it
	t->uvoffset.y += t->uvvel.y*wininfo.framestep;
	if (t->uvoffset.x>=1.0f)
		t->uvoffset.x-=1.0f;
	if (t->uvoffset.y>=1.0f)
		t->uvoffset.y-=1.0f;
	if (t->uvoffset.x<0.0f)
		t->uvoffset.x+=1.0f;
	if (t->uvoffset.y<0.0f)
		t->uvoffset.y+=1.0f;
	if (!t->doanim)
		return;
// build frame n
	if (t->buildo2p==O2P_FROMTRANSQUATSCALE)
		quatkeyframeproc(t);
	else if (t->buildo2p==O2P_FROMTRANSROTSCALE)
		tcbkeyframeproc(t);
	extrakeyframeproc(t);
// step to frame n+1
	if (t->playing) {
		if (t->seqs) {
			seq* s=&t->seqs[t->curseq];
			t->curframe+=wininfo.framestep;
			if (t->curframe>=s->endframe) {
	//			if(t->endproc) t->endproc(t);
				if (t->currepcount>0) {
					t->currepcount--;
					if (t->currepcount==0) {
						t->curframe=s->endframe;
						t->playing=0;
						return;
					}
				}
				t->curframe-=s->endframe;
				t->curframe+=s->startframe;
			}
		}
	}
}

static void seq_startrec(tree2* t)
{
//	int i,n=t->children.size();
//	for (i=0;i<n;++i)
//		seq_start(t->children[i]);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_startrec(*i);
	t->playing=1;
	t->currepcount=t->repcount;
}

static float seq_findmaxendframe(tree2* t)
{
	float curmax=0;
	if (t->seqs)
		curmax=t->seqs->endframe;
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i) {
		float cmax=seq_findmaxendframe(*i);
		if (cmax>curmax)
			curmax=cmax;
	}
	return curmax;
}

static void seq_setendframe(tree2* t,float ef)
{
	if (t->seqs) {
		if (t->seqs->endframe!=ef) {
			logger("changing endframe of '%s' from %6.1f to %6.1f\n",t->name.c_str(),t->seqs->endframe,ef);
			t->seqs->endframe=ef;
		}
	}
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_setendframe(*i,ef);
}

static void seq_findrootanims(tree2* t)
{
	if (t->seqs) {
		logger("root anim '%s' ",t->name.c_str());
		float maxendframe=seq_findmaxendframe(t);
		logger("maxendframe = %6.1f\n",maxendframe);
		seq_setendframe(t,maxendframe);
		return;
	}
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_findrootanims(*i);
}

void seq_start(tree2* t)
{
// fixup endframe (cyclops and tiger)
	logger("fixup endframe ...............\n");
	seq_findrootanims(t);
	logger("end fixup endframe ...........\n");
// end fixup endframe
	seq_startrec(t);
}

void seq_stop(class tree2* t)
{
//	int i,n=t->children.size();
//	for (i=0;i<n;++i)
//		seq_stop(t->children[i]);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_stop(*i);
	t->playing=0;
}

void seq_setseq(class tree2* t,int seq)
{
//	int i;
	struct seq *s;
//	static int level;
//	if (level==0)
//		logger("seq setseq '%s' %d\n",t->name,seq);
//	level++;
//	S32 n=t->children.size();
//	for (i=0;i<n;++i)
//		seq_setseq(t->children[i],seq);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_setseq(*i,seq);
	if (t->nseq) {
		t->curseq=range(0,seq,t->nseq-1);
		s=&t->seqs[t->curseq];
		t->curframe=s->startframe;
//		t->curframe=range(s->startframe,0,s->endframe);
/*		if (t->morph) {
//		if (t->morph &&t->playing) {
			t->morphing=t->morph;
			t->morphrot=t->rot; // this might not be valid, gotta get it from the o2p matrix..
			t->morphscale=t->scale;
			t->morphtrans=t->trans;
		}
*/

/*		if (t->proc) {
			if (s->seqbuildo2p==O2P_NONE)
				t->o2p=s->seqo2p;
			else if (s->seqbuildo2p==O2P_FROMTRANSQUATSCALE) {
				t->trans=t->newtrans=s->seqtrans;
				t->rot=t->newrot=s->seqrot;
				t->scale=t->newscale=s->seqscl;
			}
			t->buildo2p=s->seqbuildo2p;
		} */

	} else
		t->curseq=seq; // i'll do this the easy way
//	level--;
}

void seq_setframe(class tree2* t,float frame)
{
	struct seq *s;
//	S32 i,n=t->children.size();
//	for (i=0;i<n;++i)
//		seq_setframe(t->children[i],frame);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_setframe(*i,frame);
	if (t->nseq) {
		s=&t->seqs[t->curseq];
//		t->curframe=range(s->startframe,frame,s->endframe);
		t->curframe=(float)frame;
	}
}

/*
void seq_mergekeyframes(class tree2 *s,struct tree *m)
{
	int i;
	if (s->nchildren!=m->nchildren)
		errorexit("seq_mergekeyframes: children mismatch '%s' %d, '%s' %d\n",
		s->name,s->nchildren,m->name,m->nchildren);
	if (s->nseq>1)
		errorexit("seq_mergekeyframes: nseq>1 '%s' %d\n",s->name,s->nseq);
	if (s->nseq==1) {
		if (m->nseq==0)
			errorexit("seq_mergekeyframes: no seq '%s' %d, '%s' %d\n",
			s->name,s->nseq,m->name,m->nseq);
		m->seqs=(struct seq *)memrealloc(m->seqs,sizeof(struct seq)*(m->nseq+1));
		memcpy(&m->seqs[m->nseq],&s->seqs[0],sizeof(struct seq));
		for (i=0;i<NCHANNELS;i++)
			if (s->seqs[0].keys[i])
				s->seqs[0].keys[i]->refcount++;
		if (s->proc)
			m->proc=s->proc;
		m->nseq++;
	}
	for (i=0;i<s->nchildren;i++)
		seq_mergekeyframes(s->children[i],m->children[i]);
}

void seq_setmorph(struct tree2* t,float morph)
{
	int i;
//	static int level;
//	if (level==0)
//		logger("seq setmorph '%s' %f\n",t->name,morph);
//	return;
//	level++;
	for (i=0;i<t->nchildren;i++)
		seq_setmorph(t->children[i],morph);
//	level--;
	if (t->nseq)
		t->morph=morph;
}
*/
int seq_getseq(class tree2* t)
{
	return t->curseq;	// i'll do this the easy way, until we have an ANIMTABLE * or such...
}

static int ifoundit=0;
static int foundplay;
static float foundframe;

static void seq_isplayingrec(class tree2* t)
{
	if (t->nseq) { // && t->proc) {
		ifoundit=1;
		foundplay=t->playing;
		return;
	}
//	S32 i,n=t->children.size();
//	for (i=0;i<n;i++) {
//		seq_isplayingrec(t->children[i]);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i) {
		seq_isplayingrec(*i);
		if (ifoundit)
			break;
	}
}

int seq_isplaying(class tree2* t)
{
	ifoundit=0;
	seq_isplayingrec(t);
	if (!ifoundit)
		errorexit("can't find animation");
	return foundplay;
}

static void seq_getframerec(class tree2* t)
{
	if (t->nseq) {
		ifoundit=1;
		foundframe=t->curframe;
		return;
	}
//	S32 i,n=t->children.size();
//	for (i=0;i<n;i++) {
//		seq_getframerec(t->children[i]);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i) {
		seq_getframerec(*i);
		if (ifoundit)
			break;
	}
}

float seq_getframe(class tree2* t)
{
	ifoundit=0;
	seq_getframerec(t);
	if (!ifoundit)
		errorexit("can't find animation");
	return foundframe;
}

void seq_setrepcount(class tree2* t,int rep)
{
//	S32 i,n=t->children.size();
//	for (i=0;i<n;i++)
//		seq_setrepcount(t->children[i],rep);
	list<tree2*>::iterator i;
	for (i=t->children.begin();i!=t->children.end();++i)
		seq_setrepcount(*i,rep);
	if (t->nseq) {
		t->repcount=rep;
		t->currepcount=rep;
	}
}
