#include <m_eng.h>
#include "soundfx.h"

soundfx::soundfx() : doears(false)
{
pushandsetdirdown("soundfx");
	sc = new script("soundfx.txt");
	S32 i,n=sc->num();
	for (i=0;i<n;++i) {
		wavehandle* wh = wave_load(sc->idx(i).c_str(),0);
		waves.push_back(wh);
	}
popdir();
}

soundfx::~soundfx()
{
	S32 i,n=sc->num();
	for (i=0;i<n;++i)
		wave_unload(waves[i]);
	delete sc;
}

void soundfx::playsnd(const pointf3* emitpos,fxe se)
{
	if (!emitpos) {
		playasound(waves[se]);
	} else {
		float vol,pan;
		if (getvolpan(emitpos,&earpos,&earrot,&vol,&pan)) {
			soundhandle* sh = wave_play(waves[se],1);
			if (sh) {
				setsfxpan(sh,pan);
				setsfxvolume(sh,vol);
				sh->isplayasound = 1;
			}
		}
	}
}

void soundfx::setears(const pointf3& pos,const pointf3& rot)
{
	earpos = pos;
	earrot = rot;
}

bool getvolpan(const pointf3* emitpos,const pointf3* earpos,const pointf3* earrot,float* vol,float* pan)
{
	bool ret = false;
	float rvol=0,rpan=0,rpitch=1;
	const float neardist = 15;
	const float fardist = 80; // vol cutoff, (for now)
// first do distance to volume
	float dist2 = dist3dsq(emitpos,earpos);
	if (dist2<neardist*neardist) {
		rvol = 1;
		ret = true;
	} else if (dist2<fardist*fardist) {
		if (vol) { // calc volume given dist between emitpos and earpos
			rvol = (neardist*neardist)/dist2;
//			rvol = 1;
			ret = true;
		}
	} else {
		rvol = 0;
		ret = false;
	}
	if (pan && earrot && ret) {
		pointf3x pn(1,0,0,0);
		pointf3 quatear;
		rpy2quat(earrot,&quatear);
		quatrot(&quatear,&pn,&pn);
		pointf3x n(emitpos->x-earpos->x,emitpos->y-earpos->y,emitpos->z-earpos->z);
		if (normalize3d(&n,&n)) {
			rpan = dot3d(&n,&pn);
		}
	}
	if (vol)
		*vol=rvol;
	if (pan)
		*pan=rpan;
	return ret;
}

float dodoppler(const pointf3& srcpos,const pointf3& srcvel,const pointf3& destpos,const pointf3& destvel,float sos)
{
	pointf3 tr;
	tr.x=destpos.x-srcpos.x;
	tr.y=destpos.y-srcpos.y;
	tr.z=destpos.z-srcpos.z;
	if (!normalize3d(&tr,&tr))
		return 1.0f;
	float vr = dot3d(&tr,&destvel);
	float vs = dot3d(&tr,&srcvel);
	return (sos - vr)/(sos - vs);
}
#if 0
				op->carveldp.x=(op->pos.x-op->ol_ulastpos.x);
				op->carveldp.y=(op->pos.y-op->ol_ulastpos.y);
				op->carveldp.z=(op->pos.z-op->ol_ulastpos.z);
//				op->carveldp.x=(op->pos.x-op->ol_ulastpos.x)/od.ol_uloop;
//				op->carveldp.y=(op->pos.y-op->ol_ulastpos.y)/od.ol_uloop;
//				op->carveldp.z=(op->pos.z-op->ol_ulastpos.z)/od.ol_uloop;
				dr=dot3d(&tr,&od.camveldp);
				dt=dot3d(&tr,&op->carveldp);
				if (dt<od.speedosound) {
					f=(od.speedosound-dr)/(od.speedosound-dt); // doppler formula
					if (f>.2f && f<5.0f)
						op->dpf=f;
				}
#endif

