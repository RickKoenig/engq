#include <m_eng.h>

#include "helper.h"
#include "utils/modelutil.h"

void helper::dontdraw(tree2* t)
{
	t->flags |= TF_DONTDRAW;
}

helper::helper(tree2* rta) : rt(rta),nvis(0)
{
	tree2* sph = buildsphere(3,"maptestnck.tga","tex");
	dontdraw(sph);
	rt->linkchild(sph);
	sphs.push_back(sph);
}

void helper::resethelper()
{
	for_each(sphs.begin(),sphs.end(),dontdraw);
	nvis = 0;
}

void helper::addhelper(const pointf3& posa)
{
	S32 ns = sphs.size();
	tree2* sph;
	if (nvis >= ns) { // need more room
		sph = sphs[0];
		sph = sph->newdup(); // a copy
		rt->linkchild(sph);
		sphs.push_back(sph);
	} else {
		sph = sphs[nvis];
	}
// enable
	sph->trans = posa;
	sph->flags &= ~TF_DONTDRAW;
	++nvis;
}
