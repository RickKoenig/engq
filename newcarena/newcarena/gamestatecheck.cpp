#include <m_eng.h>
#include <l_misclibm.h>

#include "utils/json.h"
#include "netdescj.h"
#include "hash.h"
#include "constructor.h"
#include "timewarp.h"
#include "envj.h"
#include "enums.h"
#include "carclass.h"
#include "avaj.h"
#include "gamerunj.h"
#include "gamedescj.h"
#include "netobj.h"
#include "carenagame.h"
#include "gamestatecheck.h"

#ifdef CHECKGAMESTATE

gamestate* gs;

// add valid game state, calculate it's hash
void gamestate::writegamestate(const vector<keys>& ks)
{
	hashi h;
	U32 i;
	U32 np=(U32)gg->numplayers;
	for (i=0;i<np;++i) {
		U8 p = ks[i].getpack();
		h.add(&p,sizeof(p));
	}
	gg->hashadd(h);
	h.final();
	vector<U8> mess(h.data,h.data+h.NUMHASHBYTES);
	no->writegen(PACK_GAMESTATE,mess);
}
// check new gamestates, compare gamestate hashes
void gamestate::readgamestate(U32 from,const vector<U8>& mess)
{
	if (mess.size()!=sizeof(hashi))
		errorexit("bad gamestate packet");
	hashi mh;
	copy(mess.begin(),mess.end(),&mh.data[0]);
//	logger("read gamestate from %3d: hash '%s'\n",from,mh.dump().c_str());
	U32 lastfromtime=gamestates[from].size();
	gamestates[from].push_back(mh);
	U32 j;
	hashi c0 = gamestates[from][lastfromtime];
	for (j=0;j<(U32)gg->numplayers;++j) {
		if (from!=j && lastfromtime<gamestates[j].size()) {
			hashi c1 = gamestates[j][lastfromtime];
//			logger("comparing clk %6d: p%3d with p%3d : '%s' '%s'\n",lastfromtime,from,j,c0.dump().c_str(),c1.dump().c_str());
			if (c0!=c1) {
				logger("bad compare!, bailing!\n");
				logger("bad compare clk %6d: p%3d with p%3d : '%s' '%s'\n",lastfromtime,from,j,c0.dump().c_str(),c1.dump().c_str());
				if (!gg->bail) {
					gg->bail=10;
				}
			}
		}
	}
}

#endif
