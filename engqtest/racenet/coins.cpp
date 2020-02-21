#include <string.h>
#include <stdio.h>
#include <math.h>

#define D2_3D
#define RES3D
#define INCLUDE_WINDOWS
#include <m_eng.h>
#include <l_misclibm.h>
#include "../engine7test/n_usefulcpp.h"
#include "gameinfo.h"
#include "stubhelper.h"
#include "../hw35_line2road.h"
#include "online_uplay.h"
#include "crasheditor.h"
#include "newlobby.h"
#include "online_seltrack.h"
#include "../u_states.h"
#include "../engine7test/n_jrmcarscpp.h"
#include "hr_boxai.h"
#include "packet.h"
#include "online_uphysics.h"
#include "box2box.h"
#include "soundlst.h"
#include "gamestate.h"
#include "camera.h"
#include "scrline.h"
#include "tsp.h"

void coinsinit(TREE* track)
{
	char str[50];
	int i;
// coins
	od.numcoins=0;
	for (i=0;i<MAXCOINS;++i) {
		TREE* t;
		sprintf(str,"coin%02d.bwo",i+1);
		t=track->find(str);
		if (t) {
			od.cointrees[od.numcoins]=t;
			od.coins_caught[od.numcoins]=false;
			++od.numcoins;
		}
	}
// trophies
	od.numtrophies=0;
	for (i=0;i<MAXCOINS;++i) {
		TREE* t;
		sprintf(str,"trophy%02d.bwo",i+1);
		t=track->find(str);
		if (t) {
			od.trophytrees[od.numtrophies]=t;
			od.trophies_caught[od.numtrophies]=false;
			++od.numtrophies;
		}
	}
	if (od.numtrophies==0) {
		TREE* t;
		sprintf(str,"trophy.bwo");
		t=track->find(str);
		if (t) {
			od.trophytrees[od.numtrophies]=t;
			od.trophies_caught[od.numtrophies]=false;
			++od.numtrophies;
		}
	}
// superenergy
	od.numsuperenergies=0;
	for (i=0;i<MAXCOINS;++i) {
		TREE* t;
		sprintf(str,"superenergy%02d.bwo",i+1);
		t=track->find(str);
		if (t) {
			od.superenergytrees[od.numsuperenergies]=t;
			od.superenergies_caught[od.numsuperenergies]=false;
			++od.numsuperenergies;
		}
	}
	if (od.numsuperenergies==0) {
		TREE* t;
		sprintf(str,"superenergy.bwo");
		t=track->find(str);
		if (t) {
			od.superenergytrees[od.numsuperenergies]=t;
			od.superenergies_caught[od.numsuperenergies]=false;
			++od.numsuperenergies;
		}
	}
}

// this code is in time warp
void coinsproc()
{
	int i,j,ei;
	for (j=0;j<od.numcars;++j) {
		ol_playerdata* p=&opa[j];
// coins
		for (i=0;i<od.numcoins;++i) {
			if (!od.coins_caught[i]) {
				if (dist3dsq(&p->pos,&od.cointrees[i]->trans)<od.coincoldist*od.coincoldist) {
					od.coins_caught[i]=true;
					++p->ncoins_caught;
					if (!od.predicted && p->ncoins_caught==od.numcoins) { // only if certain
//						GAMENEWS->printf("Caught all coins : medal earned!");
//						p->medals[MED_COINS]=1;
						addmedal(j,MED_COINS);
					}
					tree2* par=particle::curparticle->genpart(PK_NULL,explosionproc);
					par->scale=pointf3x(10,10,10);
					par->treedissolvecutoff=.2f;
					p->ol_carnull->linkchild(par);
					ol_playatagsound(18,1,0);
				}
			}
			if (od.coins_caught[i]) {
				od.cointrees[i]->flags|=TF_DONTDRAWC;
			} else {
				od.cointrees[i]->flags&=~TF_DONTDRAWC;
			}
		}
// trophies
		for (i=0;i<od.numtrophies;++i) {
			if (!od.trophies_caught[i]) {
				if (dist3dsq(&p->pos,&od.trophytrees[i]->trans)<od.coincoldist*od.coincoldist) {
					od.trophies_caught[i]=true;
					++p->ntrophies_caught;
					if (!od.predicted && p->ntrophies_caught==od.numtrophies) {
//						GAMENEWS->printf("Caught all trophies : medal earned!");
//						p->medals[MED_TROPHY]=1;
						addmedal(j,MED_TROPHY);
					}
					tree2* par=particle::curparticle->genpart(PK_NULL,explosionproc);
					par->scale=pointf3x(10,10,10);
					par->treedissolvecutoff=.2f;
					p->ol_carnull->linkchild(par);
					ol_playatagsound(18,1,0);
				}
			}
			if (od.trophies_caught[i]) {
				od.trophytrees[i]->flags|=TF_DONTDRAWC;
			} else {
				od.trophytrees[i]->flags&=~TF_DONTDRAWC;
			}
		}
// superenergy
		for (i=0;i<od.numsuperenergies;++i) {
			if (!od.superenergies_caught[i]) {
				if (dist3dsq(&p->pos,&od.superenergytrees[i]->trans)<od.coincoldist*od.coincoldist) {
					od.superenergies_caught[i]=true;
					od.superenergytrees[i]->flags|=TF_DONTDRAWC;
					++p->nsuperenergies_caught;
					for (ei=0;ei<MAXENERGIES;++ei)
						if (p->tenergies[ei]<8)
							p->tenergies[ei]=8;
#if 0
					if (!od.predicted && p->superenergies_caught==od.numtrophies) {
//						GAMENEWS->printf("Caught all trophies : medal earned!");
//						p->medals[MED_TROPHY]=1;
						addmedal(j,MED_TROPHY);
					}
#endif
					tree2* par=particle::curparticle->genpart(PK_NULL,explosionproc);
					par->scale=pointf3x(10,10,10);
					par->treedissolvecutoff=.2f;
					p->ol_carnull->linkchild(par);
					ol_playatagsound(18,1,0);
				}
			}
			if (od.superenergies_caught[i]) {
				od.superenergytrees[i]->flags|=TF_DONTDRAWC;
			} else {
				od.superenergytrees[i]->flags&=~TF_DONTDRAWC;
			}
		}
	}
}

void coinsreinit()
{
	int i;
// coins
	for (i=0;i<od.numcars;++i)
		opa[i].ncoins_caught=0;
	for (i=0;i<od.numcoins;++i) {
		TREE* t;
		t=od.cointrees[i];
		t->flags&=~TF_DONTDRAWC;
		od.coins_caught[i]=false;
	}
// trophies
	for (i=0;i<od.numcars;++i)
		opa[i].ntrophies_caught=0;
	for (i=0;i<od.numtrophies;++i) {
		TREE* t;
		t=od.trophytrees[i];
		t->flags&=~TF_DONTDRAWC;
		od.trophies_caught[i]=false;
	}
// superenergies
	for (i=0;i<od.numcars;++i)
		opa[i].nsuperenergies_caught=0;
	for (i=0;i<od.numsuperenergies;++i) {
		TREE* t;
		t=od.superenergytrees[i];
		t->flags&=~TF_DONTDRAWC;
		od.superenergies_caught[i]=false;
	}
}

void coinsexit()
{
}
