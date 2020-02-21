/*#include <engine7cpp.h>
#include "usefulcpp.h"
#include "constructorcpp.h"
#include "newconstructorcpp.h"
#include "trackhashcpp.h"
#include "carenagamecpp.h"
*/
#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include "n_carenalobbycpp.h"
//#include "carenalobbyres.h"
#include "n_jrmcarscpp.h"
#include "n_loadweapcpp.h"
#include "n_usefulcpp.h"
#include "n_carclass.h"
#include "n_constructorcpp.h"
#include "n_newconstructorcpp.h"
#include "n_trackhashcpp.h"
#include "n_carenagamecpp.h"
#include "n_packetcpp.h"
#include "n_trackhashcpp.h"
#include "n_tracklistcpp.h"
#include "../u_states.h"
#include "n_aicpp.h"
#include "n_scrlinecpp.h"
#include "n_physicscpp.h"
#include "n_rematchcpp.h"
#include "n_carenaconnectcpp.h"
#include "n_line2roadcpp.h"
#include "n_twobjcpp.h"

struct nextseek {
	pointf3 loc;
	int nseek;
	int seektab[4];
};

#if 0
static struct nextseek nextseeks[]={
	{{    0,0,    0},4,1,3,4,6},
	{{    0,0, 12.5},2,0,2},
	{{ 12.5,0, 12.5},2,1,3},
	{{ 12.5,0,    0},2,0,2},
	{{-12.5,0,    0},2,0,5},
	{{-12.5,0,-12.5},2,4,6},
	{{    0,0,-12.5},2,0,5},
};
#else
static struct nextseek nextseeks[]={
	{{    0,0,    0},4,1,4,13,16},  // 0
	{{    0,0, 12.5},1,2},			// 1
	{{ 12.5,0, 12.5},1,3},			// 2
	{{ 12.5,0,    0},1,7},			// 3
	{{-12.5,0,    0},1,5},			// 4
	{{-12.5,0,-12.5},1,6},			// 5
	{{    0,0,-12.5},1,8},			// 6
	{{    0,0,    0},3,1,4,13},		// 7
	{{    0,0,    0},3,1,4,16},		// 8
	{{    0,0,    0},3,4,13,16},	// 9
	{{    0,0,    0},3,1,13,16},	// 10
	{{-12.5,0,    0},1,10},			// 11
	{{-12.5,0,-12.5},1,11},			// 12
	{{    0,0,-12.5},1,12},			// 13
	{{    0,0, 12.5},1,9},			// 14
	{{ 12.5,0, 12.5},1,14},			// 15
	{{ 12.5,0,    0},1,15},			// 16
};
#endif
#define NSEEKBOTTABLE (sizeof(nextseeks)/sizeof(nextseeks[0]))

void getbotkey(int pn)
{
	float dist;
	pointf3 *pos=&gos[pn].pos;
	int sm=gos[pn].seekmode;
	if (gp.gt.ti.validtrackhash!=TRACKTYPE_NONE) {
		gg.pk.kd=gg.pk.ku=gg.pk.kr=gg.pk.kl=0;
		return;
	}
	struct nextseek *ns=&nextseeks[sm];
	dist=dist3dsq(&ns->loc,pos);
	if (dist<4) {
//		gos[i].seekmode++;
//		if (gos[i].seekmode>=NSEEKBOTTABLE)
//			gos[i].seekmode=0;
		sm=(gg.clock/3+pn)%ns->nseek;
		sm=ns->seektab[sm];
		gos[pn].seekmode=sm;
		ns=&nextseeks[sm];
		con32_printf(gg.con,"gpk p %d: dist<4\n",pn);
//		gos[i].seekmode=random(NSEEKBOTTABLE);
	}
	float x,z,a;
//	con16_printf(gg.con,"gbk p %d: sk %d\n",pn,sm);
	if (gg.viewslot==pn)
		con32_printf(gg.con,"gpk p %d: seek %d dist %f\n",pn,gos[pn].seekmode,dist);
	x=ns->loc.x-pos->x;
	z=ns->loc.z-pos->z;
	gg.pk.ku=1;
	gg.pk.kd=0;
	a=ratan2(x,z);
	a-=gos[pn].rot.y;
	if (gg.viewslot==pn)
		con32_printf(gg.con,"rot %f\n",gos[pn].rot.y);
	a=snormalangrad(a);
	if (a>=.125f) {
		gg.pk.kl=1;
		gg.pk.kr=0;
	} else if (a<=-.125f) {
		gg.pk.kl=0;
		gg.pk.kr=1;
	} else {
		gg.pk.kl=0;
		gg.pk.kr=0;
	}
	gg.pk.wentbot=0;
	if (gg.viewslot==pn)
		con32_printf(gg.con,"a %f, l %d, r %d\n",a,gg.pk.kl,gg.pk.kr);
//	gg.pk.krematchyes=gg.pk.kresetgame=0;
}

