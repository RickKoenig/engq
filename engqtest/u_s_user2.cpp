// test scaleblit
#include <m_eng.h>
#include "u_states.h"
#include "u_modelutil.h"

namespace user2 {

S32 hs=0,hw=360,ss=0,sw=38,vs=0,vw=63,sampwidth=75,sampheight=75; // hsv start and width, samp in percent
S32 redper=100,greenper=100,blueper=100; // amplify red green blue
#define NGRID 16 // number of color codes
bool interactivemode=true;
struct menuvar edv[]={ //'`' menu
// user vars (scan calibration)
	{"@lightred@---- SCANNER USER VARS -----------------",NULL,D_VOID,0},
	{"hue start",&hs,D_INT,1},
	{"hue width",&hw,D_INT,1},
	{"sat start",&ss,D_INT,1},
	{"sat width",&sw,D_INT,1},
	{"val start",&vs,D_INT,1},
	{"val width",&vw,D_INT,1},
	{"samp width %%",&sampwidth,D_INT,1},
	{"samp height %%",&sampheight,D_INT,1},
	{"red %%",&redper,D_INT,1},
	{"green %%",&greenper,D_INT,1},
	{"blue %%",&blueper,D_INT,1},
};
const int nedv=sizeof(edv)/sizeof(edv[0]);

struct mn {
	const C8* name;
	bitmap32* bm;
};
mn mani[]={
// original
	{"tlogo1_master.tga"},
// scanned jpegs
	{"tlogo1a.jpg"},
	{"tlogo1b.jpg"},
	{"tlogo1c.jpg"},
	{"tlogo1d.jpg"},
//	{"tlogo1e.jpg"}, // diff resolution, let's skip it
	{"tlogo1f.jpg"},
	{"tlogo1g.jpg"},
};
#define NMANI (sizeof(mani)/sizeof(mani[0]))

S32 gridx[NGRID+3]; // +1 for grid, +2 for blacks
S32 gridy[2];
C32 srccols[NGRID]; // source colors
pointi2 sampstart[NGRID];  // sample start rectangles (red)
C32 sampcols[NGRID]; // sampled colors
bitmap32* pbm; // processed bitmap
S32 curpic;
S32 saw,sah; // pixel sample width height

// some statistics
struct color {
	S32 r,g,b,h,s,v;
};
// check source at init time
void readsrc(bitmap32* bm)
{
// for the 108 by 12 bitmap
	if (bm->size.x!=108 || bm->size.y!=12)
		errorexit("bm not 108 by 12");
	S32 i;
	for (i=0;i<NGRID;++i) {
		C32 val=clipgetpixel32(bm,6+6*i,0);
		srccols[i]=val;
	}
// check mathhue against source, checked, within +- 1 degree
	for (i=0;i<NGRID;++i) {
		color src;
		S32 mathhue=i*360/NGRID;
		C32 val=srccols[i];
		src.r=val.r;
		src.g=val.g;
		src.b=val.b;
		rgb2hsv(src.r,src.g,src.b,&src.h,&src.s,&src.v);
		S32 diff1=(src.h-mathhue+360)%360; 
		if (diff1>180)
			diff1-=360;
		logger("pix %2d, math.h %3d, src.h %3d, diff.h %3d\n",i,mathhue,src.h,diff1);
	}
}
	
// check sample at runtime
void showhsv()
{
	S32 i,wdp=0,wdm=0;
// check source against sample
	for (i=0;i<NGRID;++i) {
		color src,samp;
		C32 val=srccols[i];
		src.r=val.r;
		src.g=val.g;
		src.b=val.b;
		rgb2hsv(src.r,src.g,src.b,&src.h,&src.s,&src.v);
		samp.r=sampcols[i].r;
		samp.g=sampcols[i].g;
		samp.b=sampcols[i].b;
		rgb2hsv(samp.r,samp.g,samp.b,&samp.h,&samp.s,&samp.v);
		S32 diff2=(src.h-samp.h+360)%360;
		if (diff2>180)
			diff2-=360;
		if (diff2<wdm)
			wdm=diff2;
		if (diff2>wdp)
			wdp=diff2;
		outtextxybf32(B32,48+32*i,42,C32BLACK,C32WHITE,"%3d",src.h);
		outtextxybf32(B32,48+32*i,90,C32BLACK,C32WHITE,"%3d",samp.h);
		outtextxybf32(B32,48+32*i,110,C32BLACK,C32WHITE,"%3d",diff2);
	}
	outtextxybf32(B32,8,42-16,C32BLACK,C32WHITE,"src");
	outtextxybf32(B32,8,90-16,C32BLACK,C32WHITE,"samp");
	outtextxybf32(B32,8,110,C32BLACK,C32WHITE,"diff");
	outtextxybf32(B32,32*(NGRID+1)+32,110,C32BLACK,C32WHITE,"worst diff %d %d",wdp,wdm);
	clipline32(B32,40,104,32*(NGRID+1)+8+8,104,C32YELLOW);
}

void scanbm(bitmap32* s,bitmap32* d)
{
	bool scaninited=false;
	S32 scanleft=0,scanright=0,scantop=0,scanbot=0;
	S32 i,j,k,prod=s->size.x*s->size.y;
	C32* sp=s->data;
	C32* dp=d->data;
// scan for black, get scanleft,scanright,scantop,scanbot
	for (j=0;j<s->size.y;++j) { 
		for (i=0;i<s->size.x;++i,++sp,++dp) {
/*			if (i<s->size.x/4 || i>3*s->size.x/4) { // skip all but middle area, faster
				*dp=*sp;
				continue;
			}
			if (j<s->size.y/4 || j>3*s->size.y/4) {
				*dp=*sp;
				continue;
			} */
			S32 r,g,b,h,s,v;
			r=sp->r;g=sp->g;b=sp->b;
			rgb2hsv(r,g,b,&h,&s,&v);
			if (h>=hs && h<hs+hw && s>=ss && s<ss+sw && v>=vs && v<vs+vw) { // adjustable black
				*dp=C32GREEN;
				if (!scaninited) {
					scanleft=scanright=i;
					scantop=scanbot=j;
					scaninited=true;
				}
				if (scanleft>i)
					scanleft=i;
				else if (scanright<i)
					scanright=i;
				if (scantop>j)
					scantop=j;
				else if (scanbot<j)
					scanbot=j;
			} else {
				*dp=*sp;
			}
		}
	}
// create grid from scan
	for(i=0;i<NGRID+3;++i)
		gridx[i]=scanleft+(scanright-scanleft)*i/(NGRID+2);
	gridy[0]=scantop;
	gridy[1]=scanbot;
// find upper left corners of sample rectangles
	sampwidth=range(1,sampwidth,100);
	sampheight=range(1,sampheight,100);
	S32 rwidth=gridx[1]-gridx[0];
	S32 rheight=gridy[1]-gridy[0];
	saw=rwidth*sampwidth/100;
	sah=rheight*sampheight/100;
	if (saw<=0)
		saw=1;
	if (sah<=0)
		sah=1;
	S32 ssy=(gridy[1]+gridy[0]-sah)/2;
	for (i=0;i<NGRID;++i) {
		sampstart[i].x=(gridx[i+2]+gridx[i+1]-saw)/2;
		sampstart[i].y=ssy;
	}
// sum then divide for average
	for (k=0;k<NGRID;++k) {
		U32 ar=0,ag=0,ab=0; 
		for (j=0;j<sah;++j) {
			for (i=0;i<saw;++i) {
				C32 v=clipgetpixel32(s,i+sampstart[k].x,j+sampstart[k].y);
				ar+=v.r;ag+=v.g;ab+=v.b;
			}
		}
		U32 sw2=saw*sah;
		ar/=sw2;
		ag/=sw2;
		ab/=sw2;
		ar=ar*redper/100;
		ag=ag*greenper/100;
		ab=ab*blueper/100;
		ar=range(0U,ar,255U);
		ag=range(0U,ag,255U);
		ab=range(0U,ab,255U);
		sampcols[k]=C32(ar,ag,ab);
	}
}
//C32 sampcols[NSAMP];

} // namespace user2

using namespace user2;

void user2init()
{
	stringstream ss;
	ss << 10;
	logger("s is '%s'\n",ss.str().c_str());
	video_setupwindow(GX,GY);
	pushandsetdir("pic_code/testimages/tlogo1"); // should be '/cbc' etc.
	S32 i;
	for (i=0;i<NMANI;++i) // read source and samples, assume samples has same size (x by y)
		mani[i].bm=gfxread32(mani[i].name);
	popdir();
	curpic=1;
	extradebvars(edv,nedv); // setup '`' debprint menu
	pbm=bitmap32alloc(mani[1].bm->size.x,mani[1].bm->size.y,C32BLACK);
	scanbm(mani[curpic].bm,pbm); // scan first pic
	readsrc(mani[0].bm);
}

void user2proc()
{
	S32 np=0; // 0 1 or -1 move to next pic, keys or mouse clicks
	switch(KEY) {
		case '=':
			np=1;
			break;
		case '-':
			np=-1;
			break; 
		case 'i':
			interactivemode=!interactivemode; // toggle slower on the fly sampling adjust
		case K_ESCAPE:
			break;
	}
#if 0
	if (wininfo.mleftclicks) {
		np=1;
	}
	if (wininfo.mrightclicks) {
		np=-1;
	}
#endif
	if (np) {
		curpic+=np;
		if (curpic>=NMANI)
			curpic=1;
		else if (curpic<1)
			curpic=NMANI-1; 
	}
	if (interactivemode || np)
		scanbm(mani[curpic].bm,pbm);
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
}

void user2draw2d()
{
// clear background
	clipclear32(B32,C32BLUE);
// draw background
	bitmap32* cp=pbm;
	S32 px=cp->size.x;
	S32 py=cp->size.y;
//	S32 dx=(WX-px)/2; // center
//	S32 dy=(WY-py)/2;
	S32 dx=WX-px;
	S32 dy=WY-py;
	clipblit32(cp,B32,0,0,dx,dy,px,py);
// draw grid
	S32 i,j;
	for (i=0;i<NGRID+3;++i) // vert
		clipline32(B32,gridx[i]+dx,gridy[0]+dy,gridx[i]+dx,gridy[1]+dy,C32YELLOW);
	for (j=0;j<2;++j) // horz
		clipline32(B32,gridx[0]+dx,gridy[j]+dy,gridx[NGRID+2]+dx,gridy[j]+dy,C32YELLOW);
// draw sample rectangles
	for (i=0;i<NGRID;++i)
		cliprecto32(B32,sampstart[i].x+dx,sampstart[i].y+dy,saw,sah,C32RED);
// draw source
	for (i=0;i<NGRID;++i)
		cliprect32(B32,48+32*i,16,24,24,srccols[i]);
// draw samples
	for (i=0;i<NGRID;++i)
		cliprect32(B32,48+32*i,64,24,24,sampcols[i]);
	outtextxybfc32(B32,WX/2,4,C32WHITE,C32BLACK,"'%s'",mani[curpic].name);
	showhsv();
}

void user2exit()
{
	S32 i;
	for (i=0;i<NMANI;++i)
		bitmap32free(mani[i].bm);
	extradebvars(0,0);
	bitmap32free(pbm);
}

#if 0 // defined elsewhere, here just for reference
// rgb to hsv and back

void rgb2hsv(S32 r,S32 g,S32 b,S32* h,S32* s,S32* v)
{
	S32 vmin=min(min(r,g),b);
	S32 vmax=max(max(r,g),b);
	if (vmin==vmax) {
		*h=0;
	} else if (r==vmax) {
		*h=(60*(g-b)/(vmax-vmin)+360)%360;
	} else if (g==vmax) {
		*h=60*(b-r)/(vmax-vmin)+120;
	} else { // b==vmax
		*h=60*(r-g)/(vmax-vmin)+240;
	}
	if (vmax==0)
		*s=0;
	else
		*s=255*(vmax-vmin)/vmax;
	*v=vmax;
}

void hsv2rgb(S32 h,S32 s,S32 v,S32* r,S32* g,S32* b)
{
	S32 hint=h/60%6;
	S32 f60=h-hint*60;
	S32 p=v*(255-s)/255;
	S32 q=v*(60*255-f60*s)/(60*255);
	S32 t=v*(60*255-(59-f60)*s)/(60*255);
	switch(hint) {
		case 0:
			*r=v;
			*g=t;
			*b=p;
			break;
		case 1:
			*r=q;
			*g=v;
			*b=p;
			break;
		case 2:
			*r=p;
			*g=v;
			*b=t;
			break;
		case 3:
			*r=p;
			*g=q;
			*b=v;
			break;
		case 4:
			*r=t;
			*g=p;
			*b=v;
			break;
		case 5:
			*r=v;
			*g=p;
			*b=q;
			break;
		default:
			break;
	}
}
#endif
