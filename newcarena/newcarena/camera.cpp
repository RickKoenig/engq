#define RES3D
#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "hash.h"
#include "timewarp.h"
#include "carclass.h"
#include "avaj.h"
#include "physics.h"
#include "netdescj.h"
#include "constructor.h"
#include "envj.h"
#include "gamerunj.h"
#include "gamedescj.h"
#include "netobj.h"
#include "carenagame.h"
#include "scrline.h"
#include "rematch.h"
#include "ai.h"
#include "system/u_states.h"
#include "line2road.h"
#include "gamestatecheck.h"
#include "utils/modelutil.h"
#include "d2_font.h"
#include "rabbits.h"
#include "camera.h"

using namespace n_physics;

camatt::camatt(tree2* cata) : newpos(true),cat(cata),mo(FOLLOWPNT2ROAD)
{
}

void camatt::proc(S32 uloop)
{
	lastpos = cat->trans;
	pointf3 rpy;
	const tree2* cn = gg->caros[gg->viewslot].carnull; // follow carnull's
	pointf3x cnt;
	switch(mo) {
	case TV:
		cat->trans = acv2.camatt;
		break;
	case FOLLOW:
		quat2rpy(&cn->rot,&rpy);
//		gg->con->printf("rpy %f %f %f",rpy.z,rpy.x,rpy.y);
//	float camattpitch;
//	float camattdist;
		cat->trans.x = cn->trans.x-acv2.camattdist*sinf(rpy.y);
		cat->trans.y = cn->trans.y+acv2.camatt.y;
		cat->trans.z = cn->trans.z-acv2.camattdist*cosf(rpy.y);
		break;
	case FOLLOWPNT2ROAD:
		quat2rpy(&cn->rot,&rpy);
//		gg->con->printf("rpy %f %f %f",rpy.z,rpy.x,rpy.y);
//	float camattpitch;
//	float camattdist;
		cat->trans.x = cn->trans.x-acv2.camattdist*sinf(rpy.y);
		cat->trans.z = cn->trans.z-acv2.camattdist*cosf(rpy.y);
		gg->e.oldtrackj->line2roadvert(&cat->trans);
		cnt = cn->trans;
		gg->e.oldtrackj->line2roadvert(&cnt);
		cat->trans.y += cn->trans.y-cnt.y+acv2.camatt.y;
		break;
	case NUM:
		break;
	}
	getlookat(&cat->trans,&cn->trans,&cat->rot);
//	cat->rot = pointf3x(0,PI,0,0);
	if (newpos) { // big jump
		lastpos = cat->trans;
		newpos = false;
	}
}

void camatt::changemode()
{
	mo = (mode)(mo+1);
	if (mo==NUM)
		mo=(mode)0;
	newpos = true;
}

pointf3 camatt::getvel(S32 uloop) const // maybe not realtime ??
{
	if (uloop == 0)
		return pointf3x();
	else
		return pointf3x(
			(cat->trans.x - lastpos.x)/TIMEINC/uloop,
			(cat->trans.y - lastpos.y)/TIMEINC/uloop,
			(cat->trans.z - lastpos.z)/TIMEINC/uloop);
}

pointf3 camatt::getpos() const
{
	return cat->trans;
}

pointf3 camatt::getrot() const
{
	return cat->rot;
}
