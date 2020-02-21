// particle / fountain class
#define D2_3D
#include <m_eng.h>
#include "particles.h"

// particle class
particle* particle::curparticle; // should have only 1 instance of particle class
								 // this allows particles to generate particles

particle::particle()
{
	pushandsetdir("particles");
	sc=new script("particle_list.txt");
	S32 i;
	for (i=0;i<sc->num();++i) {
		tree2* partt=new tree2((sc->idx(i)+".bwo").c_str());
		partt->treedissolvecutoff=.2f;
		parttrees.push_back(partt);
	}
	popdir();
	curparticle=this;
}

tree2* particle::genpart(pk idx,bool (*tp)(tree2*),S32 iarg,float farg)
{
	if (idx<0 || idx>=PK_NUM)
		errorexit("bad particle %d",idx);
	tree2* rettree=parttrees[idx]->newdup();
	rettree->userproc=tp;
	return rettree; // pass ownership to caller
}

particle::~particle()
{
	U32 i;
	for (i=0;i<parttrees.size();++i)
		delete parttrees[i];
	delete sc;
	curparticle=0;
}
// end particle class

// userprocs
pointf3 getrandexplodevector() // magnitude is 1
{
	float lon=TWOPI*mt_frand(); // 0 to 2PI
	float lat=asinf(2.0f*mt_frand()-1.0f); // magic formula // -PI to PI
	float sy=sinf(lat); // -1 to 1
	float cy=cosf(lat); // 0 to 1 to 0
	float x=cy*cosf(lon);
	float y=sy;
	float z=cy*sinf(lon);
	return pointf3x(x,y,z);
}

// test procs
bool rightprocdie(tree2* t)
{
	++t->userint[0];
	if (t->userint[0]==50)
		return false;
	t->trans.x+=.005f;
	return true;
}

bool sinewaveproc(tree2* t)
{
	t->trans.y=.5f+.125f*sinf(t->userfloat[0]);
	t->userfloat[0]+=TWOPI*.25f/wininfo.fpswanted;
	++t->userint[0];
	if (t->userint[0]==10) {
		t->userint[0]=0;
		tree2* tc=particle::curparticle->genpart(PK_FIRE,rightprocdie);
		t->linkchild(tc);
	}
	return true;
}

// explosion effect
bool smokeproc(tree2* t)
{
	++t->userint[0];
	if (t->userint[0]>20) { // fade smoke
		t->treecolor.w=.1f*(30-t->userint[0]);
	}
	if (t->userint[0]==30)
		return false;
	return true;
}

bool fireproc(tree2* t)
{
	++t->userint[0];
	t->transvel.x*=.9f;
	t->transvel.y*=.9f;
	t->transvel.z*=.9f;
	if (t->userint[0]>=t->userint[1]-10) { // fade fire
		t->treecolor.w=.1f*(t->userint[1]-t->userint[0]);
	}
	if (t->userint[0]==t->userint[1]) { // turn into smoke
		t->rotvel=pointf3x();
		t->transvel=pointf3x();
		t->rotvel=pointf3x();
		t->rot=pointf3x();
//		tree2* tc=particle::curparticle->genpart(PK_SMOKE,smokeproc);
		S32 i;
		for (i=0;i<2;++i) {
			pointf3 v=getrandexplodevector();
			if (v.y<0) v.y=-v.y;
			tree2* tc=particle::curparticle->genpart(PK_SMOKE,smokeproc);
			tc->userint[1]=mt_random(20)+20;
			float mag=.008f*mt_frand();
			tc->trans.x=1*mag*v.x;
			tc->trans.y=1*mag*v.y;
			tc->trans.z=1*mag*v.z;
//			tc->transvel.x=mag*v.x;
//			tc->transvel.y=mag*v.y;
//			tc->transvel.z=mag*v.z;
			tc->transvel.y=.001f;
			tc->scale=pointf3x(2.5,2.5,2.5);
			tc->rotvel.x=mt_frand()*.1f;
			tc->rotvel.y=mt_frand()*.1f;
			tc->rotvel.z=mt_frand()*.1f;
			t->linkchild(tc);
		} 
//		tree2* tc=particle::curparticle->genpart(PK_SMOKE,smokeproc);
//		tc->transvel.y=.001f;
//		t->linkchild(tc);
		t->treecolor.w=0;
	}
	return true;
}

// a master
// try for 2 seconds
bool explosionproc(tree2* t)
{
	if (t->userint[0]==0) {
		S32 i;
		for (i=0;i<3;++i) { // make alot of fire
			pointf3 v=getrandexplodevector();
			if (v.y<0) v.y=-v.y;
			tree2* tc=particle::curparticle->genpart(PK_FIRE,fireproc);
			tc->userint[1]=mt_random(20)+5;
			float mag=.004f*mt_frand();
			tc->trans.x=10*mag*v.x;
			tc->trans.y=10*mag*v.y;
			tc->trans.z=10*mag*v.z;
			tc->transvel.x=mag*v.x;
			tc->transvel.y=mag*v.y;
			tc->transvel.z=mag*v.z;
			tc->scale=pointf3x(2,2,2);
			tc->rotvel.x=mt_frand()*.1f;
			tc->rotvel.y=mt_frand()*.1f;
			tc->rotvel.z=mt_frand()*.1f;
			t->linkchild(tc);
		}
	}
	++t->userint[0];
	if (t->userint[0]==60) // kill explosion
		return false;
	return true;
}
// end explosion effect

// fire an explosion every 3 seconds
bool spawnproc(tree2* t)
{
	++t->userint[0];
	if (t->userint[0]==90) {
		t->userint[0]=0;
		tree2* tc=particle::curparticle->genpart(PK_NULL,explosionproc);
		t->linkchild(tc); 
	}
	return true;
}
// end particle / fountain class
