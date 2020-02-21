#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"


namespace {
//const S32 screenx=800;
//const S32 screeny=515;
shape* rl;
shape* focus;
pbut* bquit;
const C8* fnames[]={
	"sb_back_1.jpg",
	"sb_persback.dds",
	"sb_jump.dds",
	"sb_land.tga",
};
const S32 nbks=sizeof(fnames)/sizeof(fnames[0]);
bitmap32* bks[nbks];
//bitmap32* bk;
S32 curbk;

float crad=327;
float cratio=1;
const float pbot=-.820313f;
const float ptop=pbot*.55f;
//const float ptop=0;
const float pleft=pbot;
const float pright=-pbot;
pointf3 cam={0,pbot/2,-1};
S32 pers2=1;
const float zres=8;
float zstep=1/zres;

struct menuvar evars[]={

	{"@lightcyan@camx",&cam.x,D_FLOAT,FLOATUP/16},
	{"camy",&cam.y,D_FLOAT,FLOATUP/16},
	{"camz",&cam.z,D_FLOAT,FLOATUP/16},

	{"crad",&crad,D_FLOAT,FLOATUP},
	{"cratio (y/x)",&cratio,D_FLOAT,FLOATUP},
	{"do pers2",&pers2,D_INT,1},
	{"zstep",&zstep,D_FLOAT,FLOATUP/16},

};
const S32 nevars=sizeof(evars)/sizeof(evars[0]);

pointf3 persxform1(pointf3& p,pointf3& c)
{
	pointf3 ps;
	float pn=p.z-c.z;
	if (pn<.01f)
		pn=.01f; // too close
	ps.z=1/pn;
	ps.x=WX/2+WX/2*(p.x-c.x)*ps.z;
	ps.y=WY/2-WX/2*(p.y-c.y)*ps.z;
	return ps;
}

pointf3 persxform2(pointf3& p,pointf3& c)
{
	pointf3 ps;
	float pn=p.z-c.z;
	if (pn<.01f)
		pn=.01f; // too close
	ps.z=expf(1-pn);
	ps.x=WX/2+WX/2*(p.x-c.x)*ps.z;
	ps.y=WY/2-WX/2*(p.y-c.y)*ps.z;
	return ps;
}

void buildbk()
{
	S32 k;//i,j,k;
//	bk=bitmap32alloc(screenx,screeny,C32(60,80,100));
/*	S32 i,j;
	for (j=0;j<screeny;++j) {
		for (i=0;i<screenx;++i) {
			C32 v;
			clipputpixel32(bk,i,j,v);
		}
	} */
	for (k=0;k<nbks;++k) {
		bks[k]=gfxread32(fnames[k]);
/*		for (j=0;j<bks[k]->size.y;++j) {
			for (i=0;i<bks[k]->size.x;++i) {
				C32 v=clipgetpixel32(bks[k],i,j);
				v.r=v.g=v.b=v.a;
				v.a=255;
				clipputpixel32(bks[k],i,j,v);
			}
		} */

//		cliprect32(bks[i],100,100,100,100,C32(50,250,30,0));
	}
	curbk=1;
}

void drawbk()
{
	clipblit32alpha1(bks[curbk],B32,
		0,0,
		WX/2-bks[curbk]->size.x/2,WY/2-bks[curbk]->size.y/2,
		bks[curbk]->size.x,bks[curbk]->size.y);
}

void freebk()
{
//	bitmap32free(bk);
	S32 i;
	for (i=0;i<nbks;++i)
		bitmap32free(bks[i]);
}

// uses global cx,cy,rad,ratio
void drawsemiellipse(float cxa,float cya,float rada,float ratioa)
{
	static const int cnt=100;
	S32 i;
	S32 opx=0;
	S32 opy=0;
	for (i=0;i<=cnt/2;++i) {
		float a=i*TWOPI/cnt;
		S32 px=S32(rada*cosf(a)+cxa);
		S32 py=S32(rada*ratioa*sinf(a)+cya);
		if (i>0)
			clipline32(B32,px,py,opx,opy,C32RED);
		opx=px;
		opy=py;
	}
	clipcircle32(B32,S32(cxa),S32(cya),3,C32GREEN);

}

void drawhp()
{
	float i;
	for (i=0;i<=zres;i+=1) {
		pointf3 pi=pointf3x(0,0,i*zstep);
		pointf3 po;
		if (pers2==0)
			po=persxform1(pi,cam);
		else
			po=persxform2(pi,cam);
		drawsemiellipse(po.x,po.y,crad*po.z,cratio);
	}
}

void drawflatbot()
{
	float i;
	for (i=0;i<=zres;i+=1) {
		pointf3 pil=pointf3x(pleft,pbot,i*zstep);
		pointf3 pir=pointf3x(pright,pbot,i*zstep);
		pointf3 pol,por;
		if (pers2==0) {
			pol=persxform1(pil,cam);
			por=persxform1(pir,cam);
		} else {
			pol=persxform2(pil,cam);
			por=persxform2(pir,cam);
		}
		clipline32(B32,S32(pol.x),S32(pol.y),S32(por.x),S32(por.y),C32RED);
	}
}

void drawflattop()
{
	float i;
	for (i=0;i<=zres;i+=1) {
		pointf3 pil=pointf3x(pleft,ptop,i*zstep);
		pointf3 pir=pointf3x(pright,ptop,i*zstep);
		pointf3 pol,por;
		if (pers2==0) {
			pol=persxform1(pil,cam);
			por=persxform1(pir,cam);
		} else {
			pol=persxform2(pil,cam);
			por=persxform2(pir,cam);
		}
		clipline32(B32,S32(pol.x),S32(pol.y),S32(por.x),S32(por.y),C32RED);
	}
}

void drawflatup()
{
	float i;
	for (i=0;i<=zres;i+=1) {
		float j=2*(i-zres/2)+zres/2;
		if (j<0)
			j=0;
		else if (j>zres)
			j=zres;
		pointf3 pil=pointf3x(pleft,pbot+j*zstep*(ptop-pbot),i*zstep);
		pointf3 pir=pointf3x(pright,pbot+j*zstep*(ptop-pbot),i*zstep);
		pointf3 pol,por;
		if (pers2==0) {
			pol=persxform1(pil,cam);
			por=persxform1(pir,cam);
		} else {
			pol=persxform2(pil,cam);
			por=persxform2(pir,cam);
		}
		clipline32(B32,S32(pol.x),S32(pol.y),S32(por.x),S32(por.y),C32RED);
	}
}

void drawflatdown()
{
	float i;
	for (i=0;i<=zres;i+=1) {
		pointf3 pil=pointf3x(pleft,ptop+i*zstep*(pbot-ptop),i*zstep);
		pointf3 pir=pointf3x(pright,ptop+i*zstep*(pbot-ptop),i*zstep);
		pointf3 pol,por;
		if (pers2==0) {
			pol=persxform1(pil,cam);
			por=persxform1(pir,cam);
		} else {
			pol=persxform2(pil,cam);
			por=persxform2(pir,cam);
		}
		clipline32(B32,S32(pol.x),S32(pol.y),S32(por.x),S32(por.y),C32RED);
	}
}

}

void halfpipeinit()
{
	video_setupwindow(800,515);
	pushandsetdir("halfpipe");
	script sc("halfpipeui.txt");
	factory2<shape> fact;
	rl=fact.newclass_from_handle(sc);
	bquit=rl->find<pbut>("QUIT1");
	focus=0;
	buildbk();
	extradebvars(evars,nevars);
//	cx=float(WX/2);
//	cy=float(WY/2);
//	crad=100;
}

void halfpipeproc()
{
	if (wininfo.mleftclicks) {
		focus=rl->getfocus();
		++curbk;
		if (curbk>=nbks)
			curbk=0;
	}
	if (wininfo.mrightclicks) {
		--curbk;
		if (curbk<0)
			curbk=nbks-1;
	}
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
	if (focus) {
		S32 ret=focus->proc();
		if (ret==1) {
			if (focus==bquit) {
				poporchangestate(STATE_MAINMENU);
			}
		}
	}
}

void halfpipedraw2d()
{
	C32 cc=C32LIGHTGRAY;
	cc.a=0;
	clipclear32(B32,cc);
//	cliprecto32(B32,(WX-screenx)/2-1,(WY-screeny)/2-1,2+screenx,2+screeny,C32(60,120,180));
//	drawbk();
//	if (curbk==1)
//		drawhp();
//		drawflatbot();
//		drawflattop();
		drawflatup();
//		drawflatdown();
	rl->draw();
}

void halfpipeexit()
{
	bitmap32* sv=bitmap32alloc(bks[1]->size.x,bks[1]->size.y,C32BLACK);
	video_lock();
	clipblit32(B32,sv,
		WX/2-bks[1]->size.x/2,WY/2-bks[1]->size.y/2,
		0,0,
		bks[1]->size.x,bks[1]->size.y);
	video_unlock();
//	gfxwrite32("flatbot.tga",sv);
//	gfxwrite32("flattop.tga",sv);
//	gfxwrite32("flatup.tga",sv);
//	gfxwrite32("flatdown.tga",sv);
	bitmap32free(sv);
	extradebvars(0,0);
	freebk();
	delete rl;
	popdir();
}

#if 0
void drawhp()
{
	float i,j,k;
	for (k=-1;k<=1;k+=1) {
		for (j=-1;j<=1;j+=1) {
			for (i=-1;i<=1;i+=1) {
//				i=j=k=0;
				pointf3 pi=pointf3x(i,j,k);
				pointf3 po;
					if (pers2==0)
					po=persxform1(pi,cam);
				else
					po=persxform2(pi,cam);
				drawsemiellipse(po.x,po.y,crad*po.z,cratio);
			}
		}
	}
}
#endif
