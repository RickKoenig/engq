#include <m_eng.h>
#include <l_misclibm.h>

#include "soundemit.h"
#include "soundfx.h"

// soundemit ////////////////////////////
soundemit::soundemit(soundemitman& sma,enum motor m) : sh(0),vol(0),rpm(1)
{
	sm = &sma;
	wh = sm->getwaveidx(m);
//	sh = wave_playvol(wh,0,1);
	sm->emits.push_back(this); // add to manager's list
}

void soundemit::update(const pointf3& posa,const pointf3& vela,float rpma,float vola)
{
	pos = posa;
	vel = vela;
	vol = vola;
//	pan = pana;
	rpm = rpma;
}

soundemit::~soundemit()
{
	sound_free(sh);
	if (!sm)
		return;
	vector<soundemit*>::iterator it = find(sm->emits.begin(),sm->emits.end(),this);
	if (it == sm->emits.end())
		errorexit("soundemit: destructor, can't find entry in soundemitman");
	sm->emits.erase(it); // remove from manager's list
}

//////////// soundemitman ////////////
// give wavehandle from wavelist to soundemit
wavehandle* soundemitman::getwaveidx(enum motor m)
{
	return wavese[m];
}

// make wave list
soundemitman::soundemitman()
{
pushandsetdirdown("soundmotor");
	sc = new script("soundmotor.txt");
	S32 i,n=sc->num();
	for (i=0;i<n;++i) {
		wavehandle* wh = wave_load(sc->idx(i).c_str(),0);
		wavese.push_back(wh);
	}
popdir();
}

// run thru all emitters
void soundemitman::update(const pointf3& earpos,const pointf3& earrot,const pointf3& earvel,float sos) // location of ears)
{
	vector<soundemit*>::iterator it;
	for (it = emits.begin(); it != emits.end() ; ++it) {
		soundemit* se = *it;
		float vol,pan;
		if (getvolpan(&se->pos,&earpos,&earrot,&vol,&pan)) {
			if (!se->sh) {
				se->sh = wave_playvol(se->wh,0,0);
			}
		} else {
			if (se->sh) {
				sound_free(se->sh);
				se->sh = 0;
			}
		}
		if (se->sh) {
			float freq = abs(se->rpm); //  motor rpm to sound pitch (freq)
			if (freq<1) // if commented, force freq to 1
				freq = 1;
			if (sos) {
				float dop = dodoppler(se->pos,se->vel,earpos,earvel,sos);
				freq *= dop;
			}
			setsfxfreq(se->sh,freq); // for now, just emit rpm (freq)
			setsfxpan(se->sh,pan);
			setsfxvolume(se->sh,se->vol*vol); // emit vol times distance factor 'vol'
		}

	}
}

soundemitman::~soundemitman()
{
	S32 i,n=sc->num();
	for (i=0;i<n;++i)
		wave_unload(wavese[i]);
	n=emits.size();
	for (i=0;i<n;++i) {
		emits[i]->sm = 0; // when deleting soundemit, don't erase from list
		delete emits[i];
	}
	delete sc;
}

S32 soundemitman::getnumwaves()
{
	return wavese.size();
}
