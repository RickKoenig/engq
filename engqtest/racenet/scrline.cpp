#include <string.h>
#include <stdio.h>
#include <math.h>

#define D2_3D
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

textureb* track_top;
float track_topx=10;
float track_topy=100;
float track_topsclx;
float track_topscly;
float track_topoffx;
float track_topoffy;
float track_toprot;

textureb* trackpos[2]; // you,other players

pointf2 trip[3];
uv triuv[3];

float speedcx=157.80f;
float speedcy=578.25f;
float speedpicx=-104.58f;
float speedpicy=-77.016f;
float speedstartang=1.0354f;
float speedendang=5.295f;
float speedtest=1;

float etest=.5f,pcx=892,pcy=588;
float picx[6]={
	35,
	0,
	-56,
	-52,
	-61,
	-1.25f,
};
float picy[6]={
	-31,
	-60,
	18.75f,
	-60,
	-31,
	19,
};
float sizemul=1;

const S32 nhud=9;
pointf2 hudlocs[nhud]={
// back
	{6,0},
	{276,0},
	{858,0},
	{65,720},
	{802,508},
	{53,501}, // damage
	{53,501},
	{53,501},
	{525,317},
};
namespace scrline {

// back
textureb* huds[nhud];
const C8* hudnames[nhud]={
// back
	"background/br3k_hud_back_top_left.png",
	"background/br3k_hud_back_top_center.png",
	"background/br3k_hud_back_top_right.png",
	"background/br3k_hud_back_bottom_center.png",
	"bk3_battery_meter/battery_base.png",
	"speedo/speed_lifeforce.png", // damage
	"speedo/speed_base.png",
	"speedo/speed_meter.png",
	"wrongway256.png",
};
const pointf2 weapactiveloc={823,839};
// weapsel
textureb* weapsels[MAX_WEAPKIND];
const pointf2 weapselloc={170,727};
const float weapselxspace=78;
// weapsel hilit
const pointf2 weapselhilitloc={170,727};
const float weapselhilitxspace=78;
textureb* weapselhilit;
const C8* weapselhilitname="weapon_icons/weapon_hilight.png";
// energies
textureb* energys[MAXENERGIES];
textureb* energymeters[MAXENERGIES];
//textureb* energyhilit;
const pointf2 energylocs[MAXENERGIES]={
	{954,575}, // fusion
	{917,512}, // electrical
	{842,642}, // magnetic
	{841,511}, // plasma
	{806,577}, // antimatter
	{918,642}, // chemical
};
const pointf2 energymeterlocs[MAXENERGIES]={
	{927,557}, // fusion
	{891,527}, // electrical
	{838,606}, // magnetic
	{840,526}, // plasma
	{830,554}, // antimatter
	{891,607}, // chemical
};
//const pointf2 energyhilitoff={0,0};
const pointf2 energyprintoff={18,5};
//const C8* energyhilitname="active_energy/e_hilight.png";
const C8* energynames[MAXENERGIES]={
	"bk3_battery_meter/battery_fusion.png",
	"bk3_battery_meter/battery_electrical.png",
	"bk3_battery_meter/battery_magnetic.png",
	"bk3_battery_meter/battery_plasma.png",
	"bk3_battery_meter/battery_antimatter.png",
	"bk3_battery_meter/battery_chemical.png",
};
const C8* energymeternames[MAXENERGIES]={
	"bk3_battery_meter/battery_meter_fusion.png",
	"bk3_battery_meter/battery_meter_electrical.png",
	"bk3_battery_meter/battery_meter_magnetic.png",
	"bk3_battery_meter/battery_meter_plasma.png",
	"bk3_battery_meter/battery_meter_antimatter.png",
	"bk3_battery_meter/battery_meter_chemical.png",
};
C8* wstatenames[3]={
	"dep","act","res",
};

const C8* trackposinfos[2]={
	"trackpos/trackpos01_plyr.png",
	"trackpos/trackpos01_opp.png",
};

textureb* testtri;
textureb* cointex;
textureb* trophytex;
float wrongwayanim; // some effect
float wrongwayfade;

} // end namespace scrline

using namespace scrline;

// draw an arc centered at pc, uv
static void video_draw_arc(textureb* t,float pcx,float pcy,float picx,float picy,float ang0,float ang1,float smul)
{
	float avgang=.5f*(ang0+ang1);
	if (avgang<PI*.25f || avgang>PI*1.75f) { // right
// method works well for angles near 0
		float sx=t->origsize.x*smul;
		float sy=t->origsize.y*smul;
		float rsx=1.0f/sx;
		float rsy=1.0f/sy;
		float calcy0=(picx+sx)*tanf(ang0);
		float calcy1=(picx+sx)*tanf(ang1);
		trip[0].x=pcx;
		trip[0].y=pcy;
		trip[1].x=pcx+picx+sx;
		trip[1].y=pcy+calcy0;
		trip[2].x=pcx+picx+sx;
		trip[2].y=pcy+calcy1;
		triuv[0].u=-picx*rsx;
		triuv[0].v=-picy*rsy;
		triuv[1].u=1;
		triuv[1].v=(trip[1].y-(picy+pcy))*rsy;
		triuv[2].u=1;
		triuv[2].v=(trip[2].y-(picy+pcy))*rsy;
	} else if (avgang<PI*.75f) { // down
// method works well for angles near PI/2
		float sx=t->origsize.x*smul;
		float sy=t->origsize.y*smul;
		float rsx=1.0f/sx;
		float rsy=1.0f/sy;
		float calcx0=(picy+sy)/tanf(ang0);
		float calcx1=(picy+sy)/tanf(ang1);
		trip[0].x=pcx;
		trip[0].y=pcy;
		trip[1].x=pcx+calcx0;
		trip[1].y=pcy+picy+sy;
		trip[2].x=pcx+calcx1;
		trip[2].y=pcy+picy+sy;
		triuv[0].u=-picx*rsx;
		triuv[0].v=-picy*rsy;
		triuv[1].u=(trip[1].x-(picx+pcx))*rsx;
		triuv[1].v=1;
		triuv[2].u=(trip[2].x-(picx+pcx))*rsx;
		triuv[2].v=1;
	} else if (avgang<PI*1.25f) { // left
// method works well for angles near PI
		float sx=t->origsize.x*smul;
		float sy=t->origsize.y*smul;
		float rsx=1.0f/sx;
		float rsy=1.0f/sy;
		float calcy0=picx*tanf(ang0);
		float calcy1=picx*tanf(ang1);
		trip[0].x=pcx;
		trip[0].y=pcy;
		trip[1].x=pcx+picx;
		trip[1].y=pcy+calcy0;
		trip[2].x=pcx+picx;
		trip[2].y=pcy+calcy1;
		triuv[0].u=-picx*rsx;
		triuv[0].v=-picy*rsy;
		triuv[1].u=0;
		triuv[1].v=(trip[1].y-(picy+pcy))*rsy;
		triuv[2].u=0;
		triuv[2].v=(trip[2].y-(picy+pcy))*rsy;
	} else { // up
// method works well for angles near 3*PI/2
		float sx=t->origsize.x*smul;
		float sy=t->origsize.y*smul;
		float rsx=1.0f/sx;
		float rsy=1.0f/sy;
		float calcx0=picy/tanf(ang0);
		float calcx1=picy/tanf(ang1);
		trip[0].x=pcx;
		trip[0].y=pcy;
		trip[1].x=pcx+calcx0;
		trip[1].y=pcy+picy;
		trip[2].x=pcx+calcx1;
		trip[2].y=pcy+picy;
		triuv[0].u=-picx*rsx;
		triuv[0].v=-picy*rsy;
		triuv[1].u=(trip[1].x-(picx+pcx))*rsx;
		triuv[1].v=0;
		triuv[2].u=(trip[2].x-(picx+pcx))*rsx;
		triuv[2].v=0;
	}
	video_tri_draw(t,pointf3x(1,1,1,1),trip,triuv);
}

static float cr,sr;
void loadscoreline()
{
// load font
	pushandsetdir("fonts");
	od.hugefont=new fontq("beat");
	od.hugefont->setspace(4);
	od.hugefont->setfixedwidth(15);
	popdir();
// load dirfinder
setdirdown("2009_dirfinder");  // down to 2009_dirfinder
	od.cface=texture_create("cface.pcx");
	od.cface->load();
	od.carrow=texture_create("carrow.pcx");
	od.carrow->load();
//	popdir();	// back to st2_uplay
	setdirup();
// load hud
	setdirdown("hud");
// load testtri texture
#if 0
	testtri=texture_create("bk3_battery_meter/battery_meter_chemical.png");
	testtri->load();
#else
	pushandsetdir("gfxtest");
	testtri=texture_create("maptest.tga");
	testtri->load();
	popdir();
#endif
	S32 i;
	for (i=0;i<nhud;++i) {
		huds[i]=texture_create(hudnames[i]);
		huds[i]->load();
	}
	for (i=0;i<MAX_WEAPKIND;++i) {
		string weapselname=string("weapon_icons/") +weapinfos[i].name + ".png";
		weapsels[i]=texture_create(weapselname.c_str());
		weapsels[i]->load();
	}
	weapselhilit=texture_create(weapselhilitname);
	weapselhilit->load();
	for (i=0;i<MAXENERGIES;++i) {
		energys[i]=texture_create(scrline::energynames[i]);
		energys[i]->load();
	}
	for (i=0;i<MAXENERGIES;++i) {
		energymeters[i]=texture_create(scrline::energymeternames[i]);
		energymeters[i]->load();
	}
	cointex=texture_create("coin_logo.png");
	cointex->load();
	trophytex=texture_create("trophy_logo.png");
	trophytex->load();
	trackinfo* tinf=new trackinfo;
	string path=tinf->gettrackdir(od.trackidx);
	string topname=tinf->gettracktopname(od.trackidx);
	pushandsetdir(path.c_str());
	if (fileexist(topname.c_str())) {
		track_top=texture_create(topname.c_str());
		track_top->load();
	}
	popdir();
	delete tinf;
	for (i=0;i<2;++i) {
		trackpos[i]=texture_create(trackposinfos[i]);
		trackpos[i]->load();
	}
	cr = cosf(track_toprot*PIOVER180);
	sr = sinf(track_toprot*PIOVER180);
}

#define FONTWIDTH 16
#define FONTHEIGHT 20

static void getsctimes(int tt,char *str) // skip hundredths of second 5 chars
{
	int m,s,n;
	tt=range(0,tt,99*60*60-1);
	n=tt%60;
	s=tt/60;
	m=s/60;
	s%=60;
	sprintf(str,"%2d:%02d.%02d",m,s,n*100/60);
}

static float eangles[6]={
	0*PI/3,
	5*PI/3,
	2*PI/3,
	4*PI/3,
	3*PI/3,
	1*PI/3,
};

static void coinsdraw(struct ol_playerdata *pdq)
{

	pointf3x swhitedim(.5,.5,.5,1);
	pointf3x swhite(1,1,1,1);
	int i;
	for (i=0;i<od.numcoins;++i) {
		pointf3x c = i < pdq->ncoins_caught ? swhite : swhitedim;
		video_sprite_draw(cointex,c,235.0f+i*52,50);
	}
	for (i=0;i<od.numtrophies;++i) {
		pointf3x c = i < pdq->ntrophies_caught ? swhite : swhitedim;
		video_sprite_draw(trophytex,c,235.0f+i*52,120);
	}
}

static void drawpos(ol_playerdata* pd,S32 you)
{
	cr = cosf(track_toprot*PIOVER180);
	sr = sinf(track_toprot*PIOVER180);
	float cx =  cr*pd->pos.x + sr*pd->pos.z;
	float cy = -sr*pd->pos.x + cr*pd->pos.z;
	cx = cx*track_topsclx + track_topoffx;
	cy = cy*track_topscly + track_topoffy;
	video_sprite_draw(trackpos[you],F32WHITE,cx,cy);
}

static bool canfire(ol_playerdata* pd,int ws) // return true 
{
	int i,j;
	j=carstubinfos[pd->carid].cd.c_weaps[ws];
	if (j<0)
		return false;
	for (i=0;i<MAXENERGIES;++i)
		if (pd->tenergies[i]<olracecfg.energymatrix[j][i])
			return false;
	return true;
}

void updatescoreline(ol_playerdata* pd)
{
	S32 i,j;
	S32 curweapsel=pd->xcurweapslot;
	S32 nw=carstubinfos[pd->carid].cd.c_nweap;
	S32 cws=pd->xcurweapstate;
	float wf=pd->xweapframe;
// backgnd
	video_sprite_draw(huds[0],pointf3x(1,1,1,1),hudlocs[0].x,hudlocs[0].y);
	video_sprite_draw(huds[1],pointf3x(1,1,1,1),hudlocs[1].x,hudlocs[1].y);
	video_sprite_draw(huds[2],pointf3x(1,1,1,1),hudlocs[2].x,hudlocs[2].y);
	video_sprite_draw(huds[3],pointf3x(1,1,1,1),hudlocs[3].x,hudlocs[3].y);
	video_sprite_draw(huds[4],pointf3x(1,1,1,1),hudlocs[4].x,hudlocs[4].y);

//	video_sprite_draw(huds[5],pointf3x(1,1,1,1),hudlocs[5].x,hudlocs[5].y); // damage
	video_sprite_draw(huds[6],pointf3x(1,1,1,1),hudlocs[6].x,hudlocs[6].y);
//	video_sprite_draw(huds[7],pointf3x(1,1,1,1),hudlocs[7].x,hudlocs[7].y); // speedo
// draw wrong way
	if (pd->ol_airtime==0 && od.wrongway>250) {
		wrongwayfade+=.0625f;
	} else {
		wrongwayfade-=.0625f;
	}
	wrongwayfade=range(0.0f,wrongwayfade,1.0f);
	wrongwayanim+=.025f;
	wrongwayanim=normalangrad(wrongwayanim);
	if (wrongwayfade>0) {
		float wr=.875f+.125f*sinf(wrongwayanim);
		float wg=.875f+.125f*sinf(wrongwayanim+2*PI/3);
		float wb=.875f+.125f*sinf(wrongwayanim+4*PI/3);
		float xs=(float)huds[8]->origsize.x;
		float ys=(float)huds[8]->origsize.y;
		float sc=1.25f+.5f*sinf(wrongwayanim*2);
		xs*=sc;
		ys*=sc;
		video_sprite_draw(huds[8],pointf3x(wr,wg,wb,wrongwayfade),
			hudlocs[8].x-xs*.5f,
			hudlocs[8].y-ys*.5f,
			xs,ys); // wrongway
	}
// done draw wrongway
// speed
	static S32 ospeedf;
	static float speedrat;
	if (pd->ol_airtime==0) {
		float spdf=fabs(pd->ol_accelspin/pd->ol_startaccelspin);
//		spdf=speedtest;
		ospeedf=S32(150*spdf);
		speedrat=range(0.0f,spdf,1.0f);
	}
	float curang=speedstartang+(speedendang-speedstartang)*speedrat;
	if (curang<.75f*PI) {
		video_draw_arc(huds[7],speedcx,speedcy,speedpicx,speedpicy,speedstartang,curang,1);
	} else if (curang<1.25f*PI) {
		video_draw_arc(huds[7],speedcx,speedcy,speedpicx,speedpicy,speedstartang,.75f*PI,1);
		video_draw_arc(huds[7],speedcx,speedcy,speedpicx,speedpicy,.75f*PI,curang,1);
	} else {
		video_draw_arc(huds[7],speedcx,speedcy,speedpicx,speedpicy,speedstartang,.75f*PI,1);
		video_draw_arc(huds[7],speedcx,speedcy,speedpicx,speedpicy,.75f*PI,1.25f*PI,1);
		video_draw_arc(huds[7],speedcx,speedcy,speedpicx,speedpicy,1.25f*PI,curang,1);
	}
	od.hugefont->print(125,561,60,30,F32WHITE,"%3d",ospeedf);
// weap
	if (curweapsel>=0) // show current weapon
		video_sprite_draw(weapselhilit,pointf3x(1,1,1,.5f),weapselhilitloc.x+curweapsel*weapselhilitxspace,weapselhilitloc.y-10,62,45);
	for (i=0;i<nw;++i) {
		// hilit weapon that has enough energies
		if (canfire(pd,i))
			video_sprite_draw(weapselhilit,pointf3x(1,1,1,.5f),weapselhilitloc.x+i*weapselhilitxspace,weapselhilitloc.y+10,62,45);
		S32 weapkind=carstubinfos[pd->carid].cd.c_weaps[i];
		video_sprite_draw(weapsels[weapkind],pointf3x(1,1,1,1),weapselloc.x+i*weapselxspace-10,weapselloc.y-30,80,80);
		if (!weapsels[weapkind]->origsize.x)
			od.hugefont->print(weapselloc.x+i*weapselxspace+6,weapselloc.y+5,50,15,F32WHITE,"%s",weapinfos[weapkind].name);
	}
	if (nw>0) {
		S32 curweapkind=carstubinfos[pd->carid].cd.c_weaps[curweapsel];
		video_sprite_draw(weapsels[curweapkind],pointf3x(1,1,1,1),weapactiveloc.x+20,weapactiveloc.y-300);
		od.hugefont->print(weapactiveloc.x+20,weapactiveloc.y-35-350,100,20,F32WHITE,"%s",weapinfos[curweapkind].name);
		od.hugefont->print(weapactiveloc.x+10,weapactiveloc.y-10-350,100,20,F32WHITE,"%s %3.1f",wstatenames[cws],wf);
		for (i=0;i<MAXENERGIES;++i) {
			if (olracecfg.energymatrix[curweapkind][i]) {
				video_sprite_draw(energys[i],pointf3x(1,1,1,1),energylocs[i].x,energylocs[i].y);
				etest=pd->tenergies[i]*.1f;
				float ang0=eangles[i]-etest*(PI/6);
				float ang1=eangles[i]+etest*(PI/6);
				video_draw_arc(energymeters[i],pcx,pcy,picx[i],picy[i],ang0,ang1,sizemul);
				od.hugefont->print(
					energylocs[i].x+energyprintoff.x+10,energylocs[i].y+energyprintoff.y,32,16,F32BLACK,"%d/%d",
					pd->tenergies[i],olracecfg.energymatrix[curweapkind][i]);
			}
		}
	}
// show messages (tricks, ping etc.)
	for (i=0;i<PTEXT_MAXTEXTSTRINGS;i++) {
		int sx;
		sx=0;
		bool ddr=true;
		if (od.tscoret[i]) {
			od.cscoret[i]+=.5f;
			if (od.cscoret[i]<20) {
				if ((int)(od.cscoret[i])&4) {
					sx=200*FONTWIDTH;
					ddr=false;
				}
			} else if (od.cscoret[i]>od.tscoret[i]) {
				sx=FONTWIDTH*200;
				od.tscoret[i]=0;od.cscoret[i]=0;
				ddr=false;
			} else if (od.cscoret[i]+20>od.tscoret[i])
				sx=int((od.cscoret[i]+200-od.tscoret[i])*FONTWIDTH);
			if (ddr)
				od.hugefont->print(float(320-20*FONTWIDTH+sx),float(FONTHEIGHT*i+200),200,8,F32LIGHTMAGENTA,od.scorestrings[i]);
		}
	}
// show finishers and their times
	if (pd->finplace) {
		char name[100];
		for (i=0;i<od.numcars;i++) {
			j=opa[i].finplace;
			if (j) {
				j--;
				strcpy(name,carstubinfos[i].cd.c_name);
				name[14]='\0';
				my_strupr(name);
				if (pd!=&opa[i] || (pd->clocktickcount&32))
					od.hugefont->print(float(50),float(FONTHEIGHT*(j+6)),100,8,F32GREEN,name);
				if (currule->ctf)
					sprintf(name,"%2d",opa[i].ol_numflags);
				else
					getsctimes(opa[i].dofinish-INITCANTSTARTDRIVE,name);
				od.hugefont->print(float(320-100),float(FONTHEIGHT*(j+6)),100,8,F32LIGHTGREEN,name);
			}
		}
	}
// show place
	od.hugefont->setcenter(true);
	if (od.jplace>0) {
		od.hugefont->print(945,22,30,45,F32WHITE,"%d",od.jplace);
		od.hugefont->print(975,34,12,32,F32WHITE,"/");
		od.hugefont->print(988,35,20,30,F32WHITE,"%d",od.numcars);
	}
	od.hugefont->print(15,42,50,25,F32WHITE,"LAP:");
	od.hugefont->print(107,42,30,25,F32WHITE,"%d",getlapnum(pd));
	od.hugefont->print(130,39,15,30,F32WHITE,"/");
	od.hugefont->print(138,42,30,25,F32WHITE,"%d",od.laps); 
// direction finder
	od.hugefont->setcenter(false);
	video_sprite_draw(od.carrow,F32BLACK, 930, 740, 0, 0,  3.0f/ 7.0f, 15.0f/22.0f,pd->dh);
// clock
	S32 tt,m,s;
	if (pd->clocktickcount<INITCANTSTARTDRIVE)
		tt=0;
	else {
		if (pd->dofinish)
			tt=pd->dofinish-INITCANTSTARTDRIVE;
		else
			tt=pd->clocktickcount-INITCANTSTARTDRIVE;
		tt/=60;
		tt=range(0,tt,99*60-1);
	}
	m=tt/60;
	s=tt%60;
	od.hugefont->print(15,66,140,25,F32WHITE,"TIME: %2d:%02d",m,s);
	od.hugefont->setcenter(true);
	od.hugefont->print(322,12,388,20,F32WHITE,"%s",carstubinfos[pd->carid].cd.c_name);
	od.hugefont->setcenter(false);
// coins
	coinsdraw(pd);
// map
	if (track_top) {
		video_sprite_draw(track_top,F32WHITE,track_topx,track_topy);
		for (i=0;i<od.numcars;i++) {
			if (i!=pd->carid)
				drawpos(&opa[i],1); // opp
		}
		drawpos(pd,0); // you
	}
}

void freescoreline()
{
	textureb::rc.deleterc(od.cface);
	textureb::rc.deleterc(od.carrow);
	delete od.hugefont;
	od.hugefont=0;
	S32 i;
	for (i=0;i<nhud;++i) {
		textureb::rc.deleterc(huds[i]);
		huds[i]=0;
	}
	for (i=0;i<MAX_WEAPKIND;++i) {
		textureb::rc.deleterc(weapsels[i]);
		weapsels[i]=0;
	}
	textureb::rc.deleterc(weapselhilit);
	weapselhilit=0;
	for (i=0;i<MAXENERGIES;++i) {
		textureb::rc.deleterc(energys[i]);
		energys[i]=0;
		textureb::rc.deleterc(energymeters[i]);
		energymeters[i]=0;
	}
	textureb::rc.deleterc(testtri);
	textureb::rc.deleterc(cointex);
	textureb::rc.deleterc(trophytex);
	textureb::rc.deleterc(track_top);
	track_top=0;
	for (i=0;i<2;++i)
		textureb::rc.deleterc(trackpos[i]);
}
