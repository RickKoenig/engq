#define INCLUDE_WINDOWS // for m_input.h
#include <m_eng.h>

#include "u_states.h"

#include "u_packer.h"

#if 0
void gfxviewinit()
{
	video_setupwindow(640,480);
}

void gfxviewproc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
}

void gfxviewdraw2d()
{
	clipclear32(B32,C32GREEN);
	clipcircle32(B32,WX/2,WY/2,WY/2,C32WHITE);
}

void gfxviewexit()
{
}
#endif
#if 1

static int textname=0,showalpha=0;

static int sclx,scly;

struct gblock {
	int locx,locy;
	int sx,sy;
	struct bitmap32* b32,*a32;
	char *name;
	int isfile;
};
#define MAXGBLOCK 3000
static struct gblock *glist;
static int nglist;

#define WINX 1024
#define WINY 768


static void clearpics()
{
	int i;
	for (i=0;i<nglist;i++) {
		if (glist[i].b32) {
			bitmap32free(glist[i].b32);
			glist[i].b32=0;
		}
		if (glist[i].a32) {
			bitmap32free(glist[i].a32);
			glist[i].a32=0;
		}
		delete[] glist[i].name;
	}
	nglist=0;
	freelocfinder();
	initlocfinder();
}

static C8* strclone(const C8* orig)
{
	C8* r=new C8[strlen(orig)+1];
	strcpy(r,orig);
	return r;
}

static bitmap32* makealpha(const bitmap32* s)
{
	bitmap32* r=bitmap32alloc(s->size.x,s->size.y,C32BLACK);
	C32* sc=s->data;
	C32* dc=r->data;
	U32 cnt=s->size.x*s->size.y;
	U32 i;
	for (i=0;i<cnt;++i,++sc,++dc)
		dc->r=dc->g=dc->b=sc->a;
	return r;
}

//#define DOHIST
#ifdef DOHIST
void doHist(bitmap32* bm)
{
	U32 i, prod = bm->size.x*bm->size.y;
	C32* data = bm->data;
	map<U32, U32> hist;
	U32 totalCount = 0;
	U32 maxCount = 0;
	U32 maxVal = 0;
	for (i = 0; i < prod; ++i) {
		U32 val = data[i].c32;
		if (hist.find(val) == hist.end()) {
			hist[val] = 1;
		} else {
			++hist[val];
		}
	}
	for (auto it : hist) {
		U32 val = it.first;
		U32 cnt = it.second;
		totalCount += cnt;
		if (cnt > maxCount) {
			maxCount = cnt;
			maxVal = val;
		}
	}
	logger("numpix = %u, totalCount = %u, maxCount = %u, maxVal = $%08x\n",
		prod, totalCount, maxCount, maxVal);
}
#endif

static void addpic(const char *fname)
{
	if (nglist>=MAXGBLOCK)
		return;
/*	fileopen(fname,READ);
	if (bitsperpix==565) {
		glist[nglist].b16=gfxread16(mode16);
		glist[nglist].sx=glist[nglist].b16->x;
		glist[nglist].sy=glist[nglist].b16->y;
	} else {
		glist[nglist].b24=gfxread24();
		glist[nglist].sx=glist[nglist].b24->x;
		glist[nglist].sy=glist[nglist].b24->y;
	}
	fileclose(); */
	bitmap32* b = gfxread32(fname);
#ifdef DOHIST
	doHist(b);
#endif
//#define PUSHSTREAMHACK // alpha a white from pushstream.com, (another cool android icon)
//#define DMGHACK // alpha a yellow from dhingana.com, (cool android icon)
#ifdef PUSHSTREAMHACK
	S32 i,j;
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			C32 vp = clipgetpixel32(b,i,j);
//			S32 cutoff = 3*256*3/4;
//			S32 v = vp.r + vp.g + vp.b;
//			if (v >= cutoff) {
			S32 r = (i-32)*(i-32)+(j-32)*(j-32);
			if (r >= 29*29) {
				vp.a = 0;
				clipputpixel32(b,i,j,vp);
			}
		}
	}
	bitmap32* bs = bitmap32alloc(128,128,C32BLACK);
	clipscaleblit32(b,bs); // nice filter
	bitmap32free(b);
	b = bs;
	gfxwrite32("c:/download/pushstream.png",b);
#endif
#ifdef DMGHACK
	S32 i,j;
	const C32 rv(0xfe,0xde,0x03); // a particular yellow
	for (j=0;j<b->size.y;++j) {
		for (i=0;i<b->size.x;++i) {
			C32 v = clipgetpixel32(b,i,j);
			if (v == rv) {
				v.a = 0;
				clipputpixel32(b,i,j,v);
			}
		}
	}
	bitmap32* bs = bitmap32alloc(128,128,C32BLACK);
	clipscaleblit32(b,bs); // nice filter
	bitmap32free(b);
	b = bs;
	gfxwrite32("c:/download/dmg.png",b);
#endif
	glist[nglist].b32 = b;
//#define TESTWRITE
#ifdef TESTWRITE
	static S32 cnt;
	C8 str[500];
	pushandsetdir("testpng");
	sprintf(str,"testwrite_%04d.png",cnt++);
	cliprect32(glist[nglist].b32,0,0,16,16,C32BLUE);
	gfxwrite32(str,glist[nglist].b32);
	popdir();
#endif
	glist[nglist].sx=b->size.x;
	glist[nglist].sy=b->size.y;
	glist[nglist].name=strclone(fname);
	glist[nglist].isfile=1;
	getnewloc(glist[nglist].sx,glist[nglist].sy,&glist[nglist].locx,&glist[nglist].locy);
	glist[nglist].a32=makealpha(glist[nglist].b32);
	nglist++;
}
#if 0
static void addrectangle(int i)
{
	static int rectx[8]={400,60,300,80,200,120,80,120};
	static int recty[8]={400,470,160,80,350,120,120,80};
	static char *rectname[8]={"400 400","60 470","300 160","80 80","200 350","120 120","80 120","120 80"};
	if (nglist>=MAXGBLOCK)
		return;
	glist[nglist].b32=bitmap32alloc(rectx[i],recty[i],C32BLACK);
	glist[nglist].sx=glist[nglist].b32->size.x;
	glist[nglist].sy=glist[nglist].b32->size.y;
	cliprecto32(glist[nglist].b32,0,0,glist[nglist].sx-1,glist[nglist].sy-1,C32GREEN);
	clipline32(glist[nglist].b32,0,0,glist[nglist].sx-1,glist[nglist].sy-1,C32GREEN);
	clipline32(glist[nglist].b32,0,glist[nglist].sy-1,glist[nglist].sx-1,0,C32GREEN);
	glist[nglist].name=strclone(rectname[i]);
	glist[nglist].isfile=0;
	getnewloc(glist[nglist].sx,glist[nglist].sy,&glist[nglist].locx,&glist[nglist].locy);
	nglist++;
}
#endif
void gfxviewinit()
{
	wininfo.fpswanted=60;
	sclx=scly=0;
	maxgfxx=maxgfxy=0;
//	glist=memzalloc(sizeof(struct gblock)*MAXGBLOCK);
	glist=new gblock[MAXGBLOCK];
	memset(glist,0,sizeof(gblock)*MAXGBLOCK);
	nglist=0;
//	pushandsetdir("gfxview");
//	rl=loadres("gfxresview.txt");
//	popdir();
	video_setupwindow(WINX,WINY);
//	mode16=BPP;
	initlocfinder();
}

//static void changekey()
//{
//	colorkey^=1;
//}

/*
static void clipkey16(struct bitmap16 *b,int x,int y)
{
	struct bitmap16 *bk;
	unsigned short *s,*d;
	int prod;
	prod=b->x*b->y;
	bk=bitmap16alloc(b->x,b->y,-1);
	s=b->plane;
	d=bk->plane;
	while(prod--) {
		if (!*s)
			*d=hilightgreen;
		else
			*d=*s;
		s++;
		d++;
	}
	clipblit16(bk,B16,0,0,x,y,b->x,b->y);
	bitmap16free(bk);
}

static void clipkey24(struct bitmap24 *b,int x, int y)
{
	struct bitmap24 *bk;
	unsigned char *sr,*sg,*sb,*dr,*dg,*db;
	int prod;
	prod=b->x*b->y;
	bk=bitmap24alloc(b->x,b->y,rgbblack);
	sr=b->r->plane;
	sg=b->g->plane;
	sb=b->b->plane;
	dr=bk->r->plane;
	dg=bk->g->plane;
	db=bk->b->plane;
	while(prod--) {
		if (*sr || *sg || *sb) {
			*dr=*sr;*dg=*sg;*db=*sb;
		} else {
			*dr=0;*dg=255;*db=0;
		}
		sr++;sg++;sb++;dr++;dg++;db++;
	}
	clipblit24(bk,B24,0,0,x,y,b->x,b->y);
	bitmap24free(bk);
}
*/
void gfxviewproc()
{
	S32 i;
//	struct rmessage rm;
/*	if (!fullscn)
		checkres(rl);
	while(getresmess(rl,&rm)) {
		switch(rm.id) {
		case PTEXT:
			textname^=1;
			break;
		case PCLEAR:
			clearpics();
			break;
		case PQUIT:
			popstate();
			break;
		case PVRES:
			fullscn^=1;
			if (fullscn)
				video_setupwindow(SX,SY,bitsperpix);
			else
				video_setupwindow(WINX,WINY,bitsperpix);
			break;
		case PBPP:
			bitsperpix^=565^24;
			if (fullscn)
				video_setupwindow(SX,SY,bitsperpix);
			else
				video_setupwindow(WINX,WINY,bitsperpix);
			changemode();
			break;
		case PKEY:
			changekey();
			break;
		}
		if (rm.id>=SQ1 && rm.id<=SQ8)
			addrectangle(rm.id-SQ1);
	}
*/
	if (wininfo.mrightclicks)
		setmousexy(WX/2,WY/2);
	if (MBUT==2) {
		sclx+=(MX-WX/2)>>3;
		scly+=(MY-WY/2)>>3;
	}
	sclx=range(0,sclx,range(0,maxgfxx-WX,maxgfxx));
	scly=range(0,scly,range(0,maxgfxy-WY,maxgfxy));
	switch(KEY) {
	case 't':
		textname^=1;
		break;
	case 'c':
		clearpics();
		break;
	case 'a':
		showalpha^=1;
		break;
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
	}

	if (wininfo.justdropped) {
//		clearpics();
		for (i=0;i<wininfo.args->num();++i)
			addpic(wininfo.args->idx(i).c_str());
	}
}

void gfxviewdraw2d()
{
	S32 x,y;
	char nameext[250];
	cliprect32(B32,0,0,WX,WY,C32MAGENTA);
	S32 i;
	if (!nglist) {
		outtextxybfc32(B32,WX/2,WY/2,C32WHITE,C32BLACK,"drag and drop graphic files here.. ");
		outtextxybfc32(B32,WX/2,WY/2+16,C32WHITE,C32BLACK,"'t'     toggle graphic name     ");
		outtextxybfc32(B32,WX/2,WY/2+26,C32WHITE,C32BLACK,"'c'     clear                   ");
		outtextxybfc32(B32,WX/2,WY/2+36,C32WHITE,C32BLACK,"'esc'   go back                 ");
		outtextxybfc32(B32,WX/2,WY/2+46,C32WHITE,C32BLACK,"'mouse' scroll with right button");
		outtextxybfc32(B32,WX/2,WY/2+56,C32WHITE,C32BLACK,"'a'     toggle alpha channel    ");
	}
	for (i=0;i<nglist;i++) {
		if (showalpha) {
			clipblit32(glist[i].a32,B32,
				0,0,glist[i].locx-sclx,glist[i].locy-scly,glist[i].sx,glist[i].sy);
		} else {
			clipblit32(glist[i].b32,B32,
				0,0,glist[i].locx-sclx,glist[i].locy-scly,glist[i].sx,glist[i].sy);
		}
		if (textname) {
			if (mgetnameext(glist[i].name,nameext)) {
				x=glist[i].locx+glist[i].sx/2-strlen(nameext)*4-8-sclx;
				y=glist[i].locy+glist[i].sy-8-scly;
				outtextxybf32(B32,x,y,C32WHITE,C32BLACK,"'%s'",nameext);
			}
		}
//		showlocfinder(sclx,scly);
	}
}

void gfxviewexit()
{
	clearpics();
	delete[] glist;
//	freeres(rl);
	wininfo.fpswanted=30;
	freelocfinder();
}
#endif
