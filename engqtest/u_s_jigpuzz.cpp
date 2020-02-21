#include <m_eng.h>

#define MAXPOINTS 8
#define NPOLYS 5
struct polyj {
	S32 npoints;
	struct pointf2 pos;
	U32 oor,maxor;
	C32 col;
	struct pointf2 f[MAXPOINTS];
};
static struct polyj polys[NPOLYS]={
	{4,{0,0},0,2,C32BLUE     ,{{ 502, 409},{592, 409},{592,499},{ 502,499}}},
	{4,{0,0},0,8,C32BROWN    ,{{  24, 420},{153, 291},{153,499},{  24,499}}},
	{4,{0,0},0,8,C32RED      ,{{ 163, 435},{280, 318},{280,499},{ 163,499}}},
	{4,{0,0},0,8,C32GREEN    ,{{ 290, 381},{392, 279},{392,499},{ 290,499}}},
	{4,{0,0},0,8,C32DARKGRAY ,{{ 402, 397},{492, 307},{492,499},{ 402,499}}},
};
static struct polyj bigsquare={
	4,{0,0},0,0,C32BLACK ,{{-141,-141},{141,-141},{141,141},{-141,141}}
};
S32 selected,hmode;
float startx,endx,starty,endy;
void initpolys()
{
	S32 i,j;
//	logger("WX=%d,WY=%d\n",WX,WY);
	for (i=0;i<NPOLYS;++i) {
		float sumx=0,sumy=0;
		for (j=0;j<polys[i].npoints;++j) {
			sumx+=polys[i].f[j].x;
			sumy+=polys[i].f[j].y;
		}
		sumx/=(float)polys[i].npoints;
		sumy/=(float)polys[i].npoints;
//		sumx=sumy=0;
		for (j=0;j<polys[i].npoints;++j) {
			polys[i].f[j].x-=sumx;
			polys[i].f[j].y-=sumy;
		}
		polys[i].pos.x=i*WX/5.0f+WX/10.0f;
		polys[i].pos.y=WY/2.0f;
//		logger("hi, what's this a compiler bug?, works if this is kept??\n");
//		logger("setting polys[%d].pos.x to %f\n",i,polys[i].pos.x);
//		polys[i].pos.x=10; 
	} 
	bigsquare.pos.x=WX/2.0f;
	bigsquare.pos.y=WY/2.0f;
	selected=-1;
	startx=WX/2.0f+bigsquare.f[0].x;
	starty=WY/2.0f+bigsquare.f[0].y;
	endx=WX/2.0f-bigsquare.f[0].x;
	endy=WY/2.0f-bigsquare.f[0].y;
}

float rotarr[10]={
	0.0f,
	.7071f,
	1.0f,
	.7071f,
	0.0f,
	-.7071f,
	-1.0f,
	-.7071f,
	0.0f,
	.7071f,
};

// remove 'or' and 'pos'
void convertpoly(struct polyj* in,struct polyj* out)
{
	float s,c;
	S32 i;
	out->npoints=in->npoints;
	out->oor=0;
	out->pos.x=out->pos.y=0;
	out->col=in->col;
	if (!in->npoints)
		return;
	s=rotarr[in->oor];
	c=rotarr[in->oor+2];
	for (i=0;i<in->npoints;++i) {
		out->f[i].x=in->pos.x+c*in->f[i].x-s*in->f[i].y;
		out->f[i].y=in->pos.y+s*in->f[i].x+c*in->f[i].y;
	}
}

S32 inside(struct pointf2* p,struct pointf2* lin1,struct pointf2* lin2)
{
	struct pointf2 norm;
	float d,dp;
	norm.x=lin1->y-lin2->y;
	norm.y=lin2->x-lin1->x;
	d=lin1->x*norm.x+lin1->y*norm.y;
	dp=p->x*norm.x+p->y*norm.y;
	return dp>=d;
}

void isect(struct pointf2* p1,struct pointf2* p2,struct pointf2* lin1,struct pointf2* lin2,struct pointf2* is)
{
	struct pointf2 norm;
	float d,dp1,dp2;
	norm.x=lin1->y-lin2->y;
	norm.y=lin2->x-lin1->x;
	d=lin1->x*norm.x+lin1->y*norm.y;
	dp1=p1->x*norm.x+p1->y*norm.y;
	dp2=p2->x*norm.x+p2->y*norm.y;
	float dq=(d-dp1)/(dp2-dp1);
	is->x=p1->x+dq*(p2->x-p1->x);
	is->y=p1->y+dq*(p2->y-p1->y);
}

void intsectpoly(struct polyj* a,struct polyj* b,struct polyj* out)
{
	S32 i,j;
	struct polyj cvb;
	struct polyj in;
	out->oor=0;
	out->col=C32BLACK;
	out->pos.x=out->pos.y=0;
	convertpoly(b,&cvb);
	convertpoly(a,&in);
	for (j=0;j<cvb.npoints;++j) {
		S32 bsi=j;
		S32 bei=j+1;
		if (bei==cvb.npoints)
			bei=0;
		out->npoints=0;
		for (i=0;i<in.npoints;++i) {
			struct pointf2 isp;
			S32 asi=i;
			S32 aei=i+1;
			if (aei==in.npoints)
				aei=0;
			if (inside(&in.f[asi],&cvb.f[bsi],&cvb.f[bei])) {
				if (inside(&in.f[aei],&cvb.f[bsi],&cvb.f[bei])) {
					out->f[out->npoints++]=in.f[asi];
				} else {
					out->f[out->npoints++]=in.f[asi];
					isect(&in.f[asi],&in.f[aei],&cvb.f[bsi],&cvb.f[bei],&isp);
					out->f[out->npoints++]=isp;
				}
			} else {
				if (inside(&in.f[aei],&cvb.f[bsi],&cvb.f[bei])) {
					isect(&in.f[asi],&in.f[aei],&cvb.f[bsi],&cvb.f[bei],&isp);
					out->f[out->npoints++]=isp;
				} else {
				}
			}
		}
		convertpoly(out,&in);
	}
}

float areapoly(struct polyj* p)
{
	struct polyj cp;
	float a=0;
	if (p->npoints<3)
		return 0.0f;
	S32 i;
	convertpoly(p,&cp);
	for (i=1;i<p->npoints-1;++i) {
		float v1x=cp.f[i  ].x-cp.f[0].x;
		float v1y=cp.f[i  ].y-cp.f[0].y;
		float v2x=cp.f[i+1].x-cp.f[0].x;
		float v2y=cp.f[i+1].y-cp.f[0].y;
		a+=v2x*v1y-v1x*v2y;
	}
	return -.5f*a;
}

void drawpoly(struct polyj* p)
{
	S32 i;
	struct polyj cv;
	if (!p->npoints)
		return;
	convertpoly(p,&cv);
	for (i=0;i<cv.npoints;++i) {
		S32 si=i;
		S32 ei=i+1;
		if (ei==cv.npoints)
			ei=0;
		clipline32(B32,
			(S32)cv.f[si].x,
			(S32)cv.f[si].y,
			(S32)cv.f[ei].x,
			(S32)cv.f[ei].y,
			cv.col);
	}
	clipcircle32(B32,(S32)p->pos.x,(S32)p->pos.y,5,p->col);
	outtextxyf32(B32,(S32)p->pos.x-4,(S32)p->pos.y+20,p->col,"%d",p->oor);
}

// returns true if some overlap
float totareapoly(S32 mpolys)
{
	S32 i,j;
	float area=0;
	for (j=0;j<mpolys;++j) {
		struct polyj is;
		for (i=j+1;i<mpolys;++i) {
			intsectpoly(&polys[i],&polys[j],&is);
			drawpoly(&is);
			area+=areapoly(&is);
		}
		intsectpoly(&polys[j],&bigsquare,&is);
//		drawpoly(&is);
		area-=areapoly(&is);
		area+=areapoly(&polys[j]);
	}
	return area;
}

#define STEPX 10
#define STEPY 10
// returns true if wrapped
S32 bump(S32 idx)
{
	polys[idx].pos.x+=STEPX;
	if (polys[idx].pos.x>endx) {
		polys[idx].pos.x=startx;
		polys[idx].pos.y+=STEPY;
		if (polys[idx].pos.y>endy) {
			polys[idx].pos.y=starty;
			++polys[idx].oor;
			if (polys[idx].oor==polys[idx].maxor) {
				polys[idx].oor=0;
				return 1;
			}
		}
	}
	return 0;
}

void bumpr(S32 idx)
{
	while(1) {
		if (!bump(idx))
			break;
		if (idx==0)
			break;
		--idx;
	}
}

// returns true to keep iterating
S32 iterpolys()
{
	S32 pc=0;
	while(1) {
		if (totareapoly(pc+1)>1000.0f) {
			bumpr(pc);
			return 1;
		}
		++pc;
		if (pc==NPOLYS)
			return 0;
	}
	return 0;
}

void resetpolys()
{
	S32 i;
	for (i=0;i<NPOLYS;++i) {
		polys[i].oor=0;
		polys[i].pos.x=startx;
		polys[i].pos.y=starty;
	}
}

void jigpuzzinit()
{
	video_setupwindow(800,600);
	initpolys();
	hmode=0;
	selected=-1;
//	fpswantedsave=wininfo.fpswanted;
//	wininfo.fpswanted=1000;
}

void jigpuzzproc()
{
	S32 i;
	static U32 lmbut;
	if (MBUT && lmbut) {
		if (selected>=0) {
			polys[selected].pos.x=(float)MX;
			polys[selected].pos.y=(float)MY;
		}
	}
	if (MBUT && !lmbut) {
		S32 bi=-1;
		float bd=0,d;
		for (i=0;i<NPOLYS;++i) {
			d=(polys[i].pos.x-(float)MX)*(polys[i].pos.x-(float)MX)
			 +(polys[i].pos.y-(float)MY)*(polys[i].pos.y-(float)MY);
			if (d<400 && (bi==-1 || d<bd)) {
				bd=d;
				bi=i;
			}
		}
		selected=bi;
	}
	if (!MBUT && lmbut) {
		selected=-1;
	}
	if (wininfo.mrightclicks) {
		if (selected>=0) {
			polys[selected].oor=(polys[selected].oor+1)&7;
		}
	}
	if (wininfo.mmiddleclicks) {
		if (selected>=0) {
			polys[selected].oor=(polys[selected].oor-1)&7;
		}
	}
	switch (KEY) {
	case K_ESCAPE:
		popstate();
		break;
	case '.':
	case ' ':
		if (selected>=0) {
			polys[selected].oor=(polys[selected].oor+1)&7;
		}
		break;
	case ',':
		if (selected>=0) {
			polys[selected].oor=(polys[selected].oor-1)&7;
		}
		break;
	case 'r':
		resetpolys();
		selected-=1;
		break;
	case 'h':
		selected=-1;
		if (hmode) {
			hmode=0;
		} else {
			bumpr(NPOLYS-1);
			hmode=1;
//			resetpolys();
		}
		break;
	};
	if (hmode) {
		for (i=0;i<50000;++i) {
			if (!iterpolys()) {
				hmode=0;
				break;
			}
		}
	}
	lmbut=MBUT;
}

void jigpuzzdraw2d()
{
	S32 i;
	float area=totareapoly(NPOLYS);
//	video_lock();
	cliprect32(B32,0,0,WX-1,WX-1,C32WHITE);
	for (i=0;i<NPOLYS;++i)
		drawpoly(&polys[i]);
	drawpoly(&bigsquare);
/*	if (selected>=0) {
	for (j=0;j<NPOLYS;++j)
		poly is;
		for (i=0;i<NPOLYS;++i) {
			if (i==selected)
				continue;
			intsectpoly(&polys[i],&polys[selected],&is);
			drawpoly(&is);
			area+=areapoly(&is);
		}
		intsectpoly(&polys[selected],&bigsquare,&is);
		drawpoly(&is);
		area-=areapoly(&is);
		area+=areapoly(&polys[selected]);
		outtextxyf16(B16,30,560,hiblack,"area %f",area);
	}
*/	
	outtextxyf32(B32,30,560,C32BLACK,"area %f",area);
//	video_unlock();
}

void jigpuzzexit()
{
//	wininfo.fpswanted=fpswantedsave;
}
