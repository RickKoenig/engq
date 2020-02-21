// a class to handle all networking aspects of any network game, hiding socker layer
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
#include "gamedescj.h"
#include "gamerunj.h"
#include "netobj.h"
#include "netobjbot.h"

// netbot
netbotgameobj::netbotgameobj(gamedescj* gd) : netobj(gd)
{
/*	grj = new gamerunj();
	grj->n=cgd->n;
	grj->n.gamenet=GAMENET_BOTGAME; // no network
	grj->e=cgd->e;
	grj->yourid=0; // your computer id
	grj->yourcntl=CNTL_HUMAN;
	grj->viewslot=0;
	grj->numplayers=cgd->nhumanplayers+cgd->nbotplayers;
// copy over car 0, make rest random bot cars
	grj->caros.resize(grj->numplayers);
	grj->caros[0]=cgd->a;
	grj->caros[0].ownerid=0;
	for (S32 i=1;i<grj->numplayers;++i) {
		grj->caros[i].randombot();
	} */
}

string netbotgameobj::proc()
{
	if (letsgo) { // onshot switch to letsgo=true
		return "";
	}
	letsgo = true;
	buildkeyques();
	return "BOT GAME\n";
}

S32 netbotgameobj::getnumsockets()
{
	return 0;
}

S32 netbotgameobj::getwritebuffsize(S32 slot)
{
	return 0;
}

S32 netbotgameobj::getwritebuffused(S32 slot)
{
	return 0;
}
