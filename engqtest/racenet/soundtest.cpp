// test out some sounds
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <engine1.h>
#include <misclib.h>

#include "stubhelper.h"
#include "line2road.h"
#include "online_uplay.h"

#define NSLOTS 6
static int fore,back,wht;
static int mousemodesave;
// tags
static struct tag *tagslots[NSLOTS];

extern char *weapvags[MAXWEAPONSKIND];
char **carsounds;
int ncarsounds;
char **scenesounds;
int nscenesounds;
char **carsoundscommon;
int ncarsoundscommon;

void soundtest_init()
{
// config state
	showcursor(0);
	video_setupwindow(1024,768,565);
	fore=color24to16(rgbyellow,BPP);
	wht=color24to16(rgbwhite,BPP);
	back=color24to16(rgbblue,BPP);
	mousemodesave=mousemode;
	mousemode=0;

	pushandsetdir("weapsounds_deploy");
//	tagslots[1]=tagread("deploy.tag");
	tagslots[1]=vagread(weapvags,MAXWEAPONSKIND);
	popdir();

	pushandsetdir("scenesounds");
	scenesounds=doadir(&nscenesounds,0);
	tagslots[3]=vagread(scenesounds,nscenesounds);
	popdir();
	
// load a tag file
	pushandsetdir("st2_game");
//	pushandsetdir("midifiles");
	tagslots[0]=tagread("garden.tag");
//	tagslots[0]=tagread("igsounds.tag");
	popdir();

	pushandsetdir("weapsounds_activate");
	tagslots[2]=vagread(weapvags,MAXWEAPONSKIND);
//	tagslots[2]=tagread("activate.tag");
	popdir();

	pushandsetdir("carsounds");
	carsounds=doadir(&ncarsounds,0);
	tagslots[4]=vagread(carsounds,ncarsounds);
	popdir();
	
	pushandsetdir("carsoundscommon");
	carsoundscommon=doadir(&ncarsoundscommon,0);
	tagslots[5]=vagread(carsoundscommon,ncarsoundscommon);
	popdir();
	
/*	pushandsetdir("weapsounds_result");
	tagslots[3]=vagread(weapvags,MAXWEAPONSKIND);
//	tagslots[3]=tagread("result.tag");
	popdir();
*/
}

//carsounds
//scenesounds
//carsoundscommon

void soundtest_proc()
{
	int i,j;
	if (wininfo.mleftclicks)
		playasound(taggetwhbyidx(tagslots[MX*NSLOTS/WX],MY/8));
	if (KEY=='r')
		sound_resume();
	if (KEY=='p')
		sound_pause();
	if (KEY==K_ESCAPE)
		popstate();
	video_lock();
	cliprect16(B16,0,0,WX-1,WY-1,back);
	for (j=0;j<NSLOTS;j++)
		for (i=0;i<taggetnum(tagslots[j]);i++)
			outtextxyf16(B16,j*WX/NSLOTS,i*8,fore,"'%s'",taggetname(tagslots[j],i));
	clipline16(B16,MX-5,MY,MX+5,MY,wht);
	clipline16(B16,MX,MY-5,MX,MY+5,wht);
	video_unlock();
}

void soundtest_exit()
{
	int i;
	mousemode=mousemodesave;
// free a tag file
	for (i=0;i<NSLOTS;i++)
		tagfree(tagslots[i]);
//	tagfree(pondtags);
//	tagfree(treetags);
	showcursor(1);
	freescript(carsounds,ncarsounds);
	freescript(carsoundscommon,ncarsoundscommon);
	freescript(scenesounds,nscenesounds);
}

