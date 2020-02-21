#include <m_eng.h>

struct bitmap32 *bitmap32alloc(S32 x,S32 y,C32 v)
{
	struct bitmap32 *p;
//	p=(struct bitmap32 *)memalloc(sizeof(struct bitmap32));
	p=new bitmap32;
//	p->data=(C32*)memalloc(sizeof(C32)*x*y);
	p->data=new C32[x*y];
	p->size.x=x;
	p->size.y=y;
	p->cliprect.topleft.x=0;
	p->cliprect.topleft.y=0;
	p->cliprect.size.x=x;
	p->cliprect.size.y=y;
	fastclear32(p,v);
	return p;
}

void bitmap32free(struct bitmap32 *p)
{
//	memfree(p->data);
	delete[] p->data;
//	memfree(p);
	delete p;
}

// discreet coords 0,0 to sizex-1,sizey-1

/////////////////////// pixels ////////////////////////////////
static U32 pclip(const bitmap32* b,S32 x,S32 y)
{
	if (x<b->cliprect.topleft.x)
		return 0;
	if (y<b->cliprect.topleft.y)
		return 0;
	if (x>=b->cliprect.topleft.x+b->cliprect.size.x)
		return 0;
	if (y>=b->cliprect.topleft.y+b->cliprect.size.y)
		return 0;
	return 1;
}

C32 fastgetpixel32(struct bitmap32* p,S32 x0,S32 y0)
{
	return p->data[x0+y0*p->size.x];
}

C32 clipgetpixel32(const bitmap32* p,S32 x0,S32 y0)
{
	if (pclip(p,x0,y0))
		return p->data[x0+y0*p->size.x];
	return C32LIGHTMAGENTA; // errorcolor
}

void fastputpixel32(struct bitmap32* p,S32 x0,S32 y0,C32 color)
{
	p->data[x0+y0*p->size.x]=color;
}

void clipputpixel32(const struct bitmap32* p,S32 x0,S32 y0,C32 color)
{
	if (pclip(p,x0,y0))
		p->data[x0+y0*p->size.x]=color;
}

//////////////////////////// lines ///////////////////////////////
static U32 lclip(const struct bitmap32* b,S32* x0,S32* y0,S32* x1,S32* y1)
{
	const S32 bigclip = 500000000;
	if (*x0<-bigclip)
		return 0;
	if (*x0>bigclip)
		return 0;
	if (*y0<-bigclip)
		return 0;
	if (*y0>bigclip)
		return 0;
	if (*x1<-bigclip)
		return 0;
	if (*x1>bigclip)
		return 0;
	if (*y1<-bigclip)
		return 0;
	if (*y1>bigclip)
		return 0;
	U32 code0,code1;
	S32 left,right,top,bot;
	left=b->cliprect.topleft.x;
	right=b->cliprect.topleft.x+b->cliprect.size.x;
	top=b->cliprect.topleft.y;
	bot=b->cliprect.topleft.y+b->cliprect.size.y;
	while(1) {
		code0=0;
		if (*x0<left)
			code0=1;
		else if (*x0>=right)
			code0=4;
		if (*y0<top)
			code0|=2;
		else if (*y0>=bot)
			code0|=8;
		code1=0;
		if (*x1<left)
			code1=1;
		else if (*x1>=right)
			code1=4;
		if (*y1<top)
			code1|=2;
		else if (*y1>=bot)
			code1|=8;
		if (!(code0|code1))
			return 1;
		if (code0&code1)
			return 0;
		if (!code0) {
			exch(*x0,*x1);
			exch(*y0,*y1);
			exch(code0,code1);
		}
		if (code0&1) { //left

			S32 ynew=*y0;
			LONGMULDIV(left-*x0,*y1-*y0,*x1-*x0,ynew);
			*y0+=ynew;
			*x0=left;
		}
		else if (code0&2) {	// top

			S32 xnew=*x0;
			LONGMULDIV(top-*y0,*x1-*x0,*y1-*y0,xnew);
			*x0+=xnew;
			*y0=top;
		}
		else if (code0&4) {	// right

			S32 ynew=*y0;
			LONGMULDIV(right-1-*x0,*y1-*y0,*x1-*x0,ynew);
			*y0+=ynew;
			*x0=right-1;
		}
		else { // bottom

			S32 xnew=*x0;
			LONGMULDIV(bot-1-*y0,*x1-*x0,*y1-*y0,xnew);
			*x0+=xnew;
			*y0=bot-1;
		}
	}
}

void fastline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,S32 y1,C32 color)
{
	C32* ptr;
	S32 dx=x1-x0;
	S32 dy=y1-y0;
	S32 ostep=b32->size.x;
	S32 cstep=1;
	S32 cnt;
	S32 err;
//	outtextxyf32(b32,x0,y0,color,"fastline from %d %d to %d %d",x0,y0,x1,y1);
	if (dx<0) {
		dx = -dx;
		dy = -dy;
		exch(x0,x1);
		exch(y0,y1);
	}
	ptr=b32->data+x0+y0*b32->size.x;
	if (dy<0) {
		ostep=-ostep;
		dy = -dy;
	}
	if (dx<dy) {
		exch(x0,y0);
		exch(x1,y1);
		exch(dx,dy);
		exch(ostep,cstep);
	}
	err=dx>>1;
	cnt=dx;
	do {
		*ptr=color;
		err-=dy;
		if (err<0) {
			err+=dx;
			ptr+=ostep;
		}
		ptr+=cstep;
	} while(cnt--);
}

void clipline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,S32 y1,C32 color)
{
	if (lclip(b32,&x0,&y0,&x1,&y1))
		fastline32(b32,x0,y0,x1,y1,color);
}

static U32 xclip(const struct bitmap32* b,S32* x0,S32* x1)
{
	S32 left,right;
	if (*x0>*x1) {
		exch(*x0,*x1);
	}
	left=b->cliprect.topleft.x;
	right=b->cliprect.topleft.x+b->cliprect.size.x;
	if (*x1<left)
		return 0;
	if (*x0>=right)
		return 0;
	if (*x0<left)
		*x0=left;
	if (*x1>=right)
		*x1=right-1;
	return 1;
}

void fasthline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,C32 color)
{
	STOSD(b32->data+x0+b32->size.x*y0,color.c32,x1-x0+1);
}

void cliphline32(const struct bitmap32* b32,S32 x0,S32 y0,S32 x1,C32 color)
{
	if (y0<b32->cliprect.topleft.y)
		return;
	if (y0>=b32->cliprect.topleft.y+b32->cliprect.size.y)
		return;
	if (xclip(b32,&x0,&x1))
		fasthline32(b32,x0,y0,x1,color);
}

static U32 yclip(struct bitmap32* b,S32* y0,S32* y1)
{
	S32 top,bot;
	if (*y0>*y1) {
		exch(*y0,*y1);
	}
	top=b->cliprect.topleft.y;
	bot=b->cliprect.topleft.y+b->cliprect.size.y;
	if (*y1<top)
		return 0;
	if (*y0>=bot)
		return 0;
	if (*y0<top)
		*y0=top;
	if (*y1>=bot)
		*y1=bot-1;
	return 1;
}

void fastvline32(struct bitmap32* b32,S32 x0,S32 y0,S32 y1,C32 color)
{
	S32 cnt=y1-y0+1;
	C32* ptr=b32->data+x0+b32->size.x*y0;
	S32 step=b32->size.x;
	while(cnt--) {
		*ptr=color;
		ptr+=step;
	}
}

void clipvline32(struct bitmap32* b32,S32 x0,S32 y0,S32 y1,C32 color)
{
	if (x0<b32->cliprect.topleft.x)
		return;
	if (x0>=b32->cliprect.topleft.x+b32->cliprect.size.x)
		return;
	if (yclip(b32,&y0,&y1))
		fastvline32(b32,x0,y0,y1,color);
}

/////////// circles ////////////////////////////////////
static S32 cir_xorg,cir_yorg;
static C32 cir_color;

void static octdot32o(struct bitmap32* b,S32 x,S32 y)
{
	S32 left1=cir_xorg-x;
	S32 left2=cir_xorg-y;
	S32 left3=cir_xorg+y;
	S32 left4=cir_xorg+x;
	S32 up1=cir_yorg-x;
	S32 up2=cir_yorg-y;
	S32 up3=cir_yorg+y;
	S32 up4=cir_yorg+x;
	clipputpixel32(b,left2,up1,cir_color);
	clipputpixel32(b,left1,up2,cir_color);
	clipputpixel32(b,left1,up3,cir_color);
	clipputpixel32(b,left2,up4,cir_color);
	clipputpixel32(b,left3,up1,cir_color);
	clipputpixel32(b,left4,up2,cir_color);
	clipputpixel32(b,left4,up3,cir_color);
	clipputpixel32(b,left3,up4,cir_color);
}

void clipcircleo32(struct bitmap32* b,S32 x,S32 y,S32 r,C32 c)
{
	S32 e;
	if (r<=0) {
		clipputpixel32(b,x,y,c);
		return;
	}
	if (x-r>=b->cliprect.topleft.x+b->cliprect.size.x)
		return;
	if (x+r<b->cliprect.topleft.x)
		return;
	if (y-r>=b->cliprect.topleft.y+b->cliprect.size.y)
		return;
	if (y+r<b->cliprect.topleft.y)
		return;	// circle completely off bitmap, don't draw
	cir_xorg=x;
	cir_yorg=y;
	cir_color=c;
	x=0;
	y=r;
	e=(y<<1)-1;
	while(x<=y) {
		octdot32o(b,x,y);
		e-=(x<<2)+2;
		x++;
		if (e<0) {
			e+=(y<<2)+2;
			y--;
		}
	}
}

void clipcircle32(const struct bitmap32* b,S32 x,S32 y,S32 r,C32 c)
{
	S32 e;
	if (r<=0) {
		clipputpixel32(b,x,y,c);
		return;
	}
	if (x-r>=b->cliprect.topleft.x+b->cliprect.size.x)
		return;
	if (x+r<b->cliprect.topleft.x)
		return;
	if (y-r>=b->cliprect.topleft.y+b->cliprect.size.y)
		return;
	if (y+r<b->cliprect.topleft.y)
		return;	// circle completely off bitmap, don't draw
	cir_xorg=x;
	cir_yorg=y;
	cir_color=c;
	x=0;
	y=r;
	e=(y<<1)-1;
	while(x<=y) {
		cliphline32(b,cir_xorg-y,cir_yorg-x,cir_xorg+y,cir_color);
		cliphline32(b,cir_xorg-y,cir_yorg+x,cir_xorg+y,cir_color);
		e-=(x<<2)+2;
//		x++; // was here
		if (e<0) {
			e+=(y<<2)+2;
			cliphline32(b,cir_xorg-x,cir_yorg-y,cir_xorg+x,cir_color);
			cliphline32(b,cir_xorg-x,cir_yorg+y,cir_xorg+x,cir_color);
			y--;
		}
		x++;	// now here
	}
}

// continuous coords 0,0 to sizex,sizey from now on...

/////////////////////////// rectangles ///////////////////////////////////////

static U32 rclip32(const struct bitmap32* b,S32* x,S32* y,S32* sx,S32* sy)
{
	S32 move;
// trivial check
	if (*sx == 0 || *sy == 0)
		return 0;
	if (*sx<0) {
		*sx = - *sx;
		*x -= *sx;
	}
	if (*sy<0) {
		*sy = - *sy;
		*y -= *sy;
	}
// left
	move = b->cliprect.topleft.x - *x;
	if (move>0) {
		*x += move;
		*sx -= move;
	}
	if (*sx <= 0)
		return 0;
// top
	move = b->cliprect.topleft.y - *y;
	if (move>0) {
		*y += move;
		*sy -= move;
	}
	if (*sy <= 0)
		return 0;
// right
	move = (*x + *sx) - (b->cliprect.topleft.x + b->cliprect.size.x);
	if (move>0)
		*sx -= move;
	if (*sx <= 0)
		return 0;
// bottom
	move = (*y + *sy) - (b->cliprect.topleft.y + b->cliprect.size.y);
	if (move>0)
		*sy -= move;
	if (*sy <= 0)
		return 0;
	return 1;
}

void fastrect32(const struct bitmap32* b32,S32 x0,S32 y0,S32 sx,S32 sy,C32 color)
{
	S32 i,j;
	U32 dinc;
	register C32 *dp;
	register C32 val=color;
	dp=b32->data+b32->size.x*y0+x0;
	dinc=b32->size.x;
	for (j=0;j<sy;j++) {
		for (i=0;i<sx;i++)
			dp[i]=val;
		dp+=dinc;
	}
}

void cliprect32(const struct bitmap32* b32,S32 x0,S32 y0,S32 sx,S32 sy,C32 color)
{
	if (rclip32(b32,&x0,&y0,&sx,&sy))
		fastrect32(b32,x0,y0,sx,sy,color);
}

void cliprecto32(struct bitmap32* b32,S32 x0,S32 y0,S32 sx,S32 sy,C32 color)
{
	S32 x1,y1;
	if (sx==0 || sy==0)
		return;
	if (sx<0) {
		sx=-sx;
		x0-=sx;
	}
	if (sy<0) {
		sy=-sy;
		y0-=sy;
	}
	x1=x0+sx-1;
	y1=y0+sy-1;
	cliphline32(b32,x0,y0,x1,color);
	cliphline32(b32,x0,y1,x1,color);
	clipvline32(b32,x0,y0,y1,color);
	clipvline32(b32,x1,y0,y1,color);
}

void fastclear32(struct bitmap32* source,C32 color)
{
	U32 i;
	C32* p=source->data;
	U32 prod=source->size.x*source->size.y;
	for (i=0;i<prod;++i)
			p[i]=color;
}

void clipclear32(struct bitmap32* source,C32 color)
{
	if (source->cliprect.topleft.x==0 &&
	  source->cliprect.topleft.y==0 &&
	  source->cliprect.size.x==source->size.x &&
	  source->cliprect.size.y==source->size.y) {
		fastclear32(source,color);
	} else {
		fastrect32(source,source->cliprect.topleft.x,
		  source->cliprect.topleft.y,
		  source->cliprect.size.x,
		  source->cliprect.size.y,color);
	}
}

bool bclip32(const struct bitmap32* s,const struct bitmap32* d,S32* sx,S32* sy,S32* dx,S32* dy,S32* tx,S32* ty)
{
	S32 move;
// trivial check
	if ((*tx<=0)||(*ty<=0))
		return false;
// left source
	move = s->cliprect.topleft.x - *sx;
	if (move>0) {
		*sx += move;
		*dx += move;
		*tx -= move;
	}
	if (*tx <= 0)
		return false;
// left dest
	move = d->cliprect.topleft.x - *dx;
	if (move>0) {
		*sx += move;
		*dx += move;
		*tx -= move;
	}
	if (*tx <= 0)
		return false;
// top source
	move = s->cliprect.topleft.y - *sy;
	if (move>0) {
		*sy += move;
		*dy += move;
		*ty -= move;
	}
	if (*ty <= 0)
		return false;
// top dest
	move = d->cliprect.topleft.y - *dy;
	if (move>0) {
		*sy += move;
		*dy += move;
		*ty -= move;
	}
	if (*ty <= 0)
		return false;
// right source
	move = (*sx + *tx) - (s->cliprect.topleft.x + s->cliprect.size.x);
	if (move>0)
		*tx -= move;
	if (*tx <= 0)
		return false;
// right dest
	move = (*dx + *tx) - (d->cliprect.topleft.x + d->cliprect.size.x);
	if (move>0)
		*tx -= move;
	if (*tx <= 0)
		return false;
// bottom source
	move = (*sy + *ty) - (s->cliprect.topleft.y + s->cliprect.size.y);
	if (move>0)
		*ty -= move;
	if (*ty <= 0)
		return false;
// bottom dest
	move = (*dy + *ty) - (d->cliprect.topleft.y + d->cliprect.size.y);
	if (move>0)
		*ty -= move;
	if (*ty <= 0)
		return false;
	return true;
}


void fastblit32(const bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty)
{
	S32 i,j;
	U32 sinc,dinc;
	register C32 *sp,*dp;
	if (tx<=0 || ty<=0)
		return;
	sp=s->data+s->size.x*sy+sx;
	dp=d->data+d->size.x*dy+dx;
	sinc=s->size.x;
	dinc=d->size.x;
	for (j=0;j<ty;j++) {
		for (i=0;i<tx;i++)
			dp[i]=sp[i];
//		memcpy(dp,sp,tx<<2);
		sp+=sinc;
		dp+=dinc;
	}
}

void fastblit32alpha1(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty)
{
	S32 i,j;
	U32 sinc,dinc;
	register C32 *sp,*dp;
	if (tx<=0 || ty<=0)
		return;
	sp=s->data+s->size.x*sy+sx;
	dp=d->data+d->size.x*dy+dx;
	sinc=s->size.x;
	dinc=d->size.x;
	for (j=0;j<ty;j++) {
		for (i=0;i<tx;i++) {
			register C32 v;
			v=sp[i];
			if (v.a>=0x80)
				dp[i]=v;
		}
		sp+=sinc;
		dp+=dinc;
	}
}

void clipblit32(const bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty)
{
	if (bclip32(s,d,&sx,&sy,&dx,&dy,&tx,&ty))
		fastblit32(s,d,sx,sy,dx,dy,tx,ty);
}

void clipblit32alpha1(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty)
{
	if (bclip32(s,d,&sx,&sy,&dx,&dy,&tx,&ty))
		fastblit32alpha1(s,d,sx,sy,dx,dy,tx,ty);
}

void fastxpar32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 xp)
{
	S32 i,j;
	U32 sinc,dinc;
	register C32 *sp,*dp;
	if (tx<=0 || ty<=0)
		return;
	sp=s->data+s->size.x*sy+sx;
	dp=d->data+d->size.x*dy+dx;
	sinc=s->size.x;
	dinc=d->size.x;
	for (j=0;j<ty;j++) {
		for (i=0;i<tx;i++) {
			register C32 v;
			v=sp[i];
			if (v.c32!=xp.c32)
				dp[i]=v;
		}
		sp+=sinc;
		dp+=dinc;
	}
}

void clipxpar32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 xpar)
{
	if (bclip32(s,d,&sx,&sy,&dx,&dy,&tx,&ty))
		fastxpar32(s,d,sx,sy,dx,dy,tx,ty,xpar);
}

void fastfore32(const struct bitmap32* s,const struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore)
{
	S32 i,j;
	U32 sinc,dinc;
	register C32 *sp,*dp;
	if (tx<=0 || ty<=0)
		return;
	sp=s->data+s->size.x*sy+sx;
	dp=d->data+d->size.x*dy+dx;
	sinc=s->size.x;
	dinc=d->size.x;
	for (j=0;j<ty;j++) {
		for (i=0;i<tx;i++) {
			register C32 v;
			v=sp[i];
			if (v.c32!=0)
				dp[i]=fore;
			//dp[i] = C32GREEN;
		}
		sp+=sinc;
		dp+=dinc;
	}
}

void clipfore32(const struct bitmap32* s,const struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore)
{
	if (bclip32(s,d,&sx,&sy,&dx,&dy,&tx,&ty))
		fastfore32(s,d,sx,sy,dx,dy,tx,ty,fore);
}

void fastforeback32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore,C32 back)
{
	S32 i,j;
	U32 sinc,dinc;
	register C32 *sp,*dp;
	if (tx<=0 || ty<=0)
		return;
	sp=s->data+s->size.x*sy+sx;
	dp=d->data+d->size.x*dy+dx;
	sinc=s->size.x;
	dinc=d->size.x;
	for (j=0;j<ty;j++) {
		for (i=0;i<tx;i++) {
			register C32 v;
			v=sp[i];
			if (v.c32!=0)
				dp[i]=fore;
			else
				dp[i]=back;
		}
		sp+=sinc;
		dp+=dinc;
	}
}

void clipforeback32(struct bitmap32* s,struct bitmap32* d,S32 sx,S32 sy,S32 dx,S32 dy,S32 tx,S32 ty,C32 fore,C32 back)
{
	if (bclip32(s,d,&sx,&sy,&dx,&dy,&tx,&ty))
		fastforeback32(s,d,sx,sy,dx,dy,tx,ty,fore,back);
}

static void clipscaleblit32x(const bitmap32* bi,bitmap32* bo)
{
	S32 step=bi->size.x<<1;
	S32 j;
	C32 *pin=bi->data;
	C32 *pout=bo->data;
	for (j=0;j<bo->size.y;++j,pin+=bi->size.x,pout+=bo->size.x) {
		S32 den=bo->size.x<<1;
		S32 num=bi->size.x-bo->size.x;
		S32 xi=num/den;
		S32 xf=num%den;
		S32 stepi=step/den;
		S32 stepf=step%den;
		if (xf<0) {
			xf+=den;
			--xi;
		}
		S32 w;
		for (w=0;w<bo->size.x;++w) {
			C32 val;
			if (xi<0)
				val=pin[0];
			else if (xi>=bi->size.x-1)
				val=pin[bi->size.x-1];
			else {
				C32 pi=pin[xi];
				C32 pi1=pin[xi+1];
				val.r=(S32)(pi.r) + (xf*((S32)(pi1.r)- (S32)(pi.r)) )/den;
				val.g=(S32)(pi.g) + (xf*((S32)(pi1.g)- (S32)(pi.g)) )/den;
				val.b=(S32)(pi.b) + (xf*((S32)(pi1.b)- (S32)(pi.b)) )/den;
				val.a=(S32)(pi.a) + (xf*((S32)(pi1.a)- (S32)(pi.a)) )/den;
			}
			pout[w]=val;
//			logger("xi = %d+%d/%d, xo = %d+%d/%d   ",w,1,2,xi,xf,den);
//			logger("value at x = %d\n",val);
			xi+=stepi;
			xf+=stepf;
			if (xf>=den) {
				xf-=den;
				++xi;
			}
		}
	}
}

static void clipscaleblit32y(bitmap32* bi,bitmap32* bo)
{
	S32 step=bi->size.y<<1;
	S32 j;
	C32 *pin=bi->data;
	C32 *pout=bo->data;
	S32 prod=(bi->size.y-1)*bo->size.x;
	S32 prodo=bo->size.x*bo->size.y;
	for (j=0;j<bo->size.x;++j,++pin,++pout) {
		S32 den=bo->size.y<<1;
		S32 num=bi->size.y-bo->size.y;
		S32 xi=num/den*bi->size.x;
		S32 xf=num%den;
		S32 stepi=step/den*bi->size.x;
		S32 stepf=step%den;
		if (xf<0) {
			xf+=den;
			xi-=bi->size.x;
		}
		S32 w;
		for (w=0;w<prodo;w+=bo->size.x) {
			C32 val;
			if (xi<0)
				val=pin[0];
			else if (xi>=prod)
				val=pin[prod];
			else {
				C32 pi=pin[xi];
				C32 pi1=pin[xi+bo->size.x];
				val.r=(S32)(pi.r) + (xf*((S32)(pi1.r)- (S32)(pi.r)) )/den;
				val.g=(S32)(pi.g) + (xf*((S32)(pi1.g)- (S32)(pi.g)) )/den;
				val.b=(S32)(pi.b) + (xf*((S32)(pi1.b)- (S32)(pi.b)) )/den;
				val.a=(S32)(pi.a) + (xf*((S32)(pi1.a)- (S32)(pi.a)) )/den;
			}
			pout[w]=val;
//			logger("xi = %d+%d/%d, xo = %d+%d/%d   ",w,1,2,xi,xf,den);
//			logger("value at x = %d\n",val);
			xi+=stepi;
			xf+=stepf;
			if (xf>=den) {
				xf-=den;
				xi+=bo->size.x;
			}
		}
	}
}

void clipscaleblit32(const bitmap32* bi,bitmap32* bo)
{
	if (bi->size.x==bo->size.x && bi->size.y==bo->size.y) {
		clipblit32(bi,bo,0,0,0,0,bi->size.x,bi->size.y);
		return;
	}
	bitmap32* interm=bitmap32alloc(bo->size.x,bi->size.y,C32BLACK);
	if (bi->size.x==bo->size.x)
		clipblit32(bi,interm,0,0,0,0,bi->size.x,bi->size.y);
	else
		clipscaleblit32x(bi,interm);
	if (bi->size.y==bo->size.y)
		clipblit32(interm,bo,0,0,0,0,bo->size.x,bo->size.y);
	else
		clipscaleblit32y(interm,bo);
	bitmap32free(interm);
}

bitmap32* bitmap32copy(const bitmap32* b)
{
	bitmap32* bret=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
	memcpy(bret->data,b->data,sizeof(C32)*b->size.x*b->size.y);
	return bret;
}

void bitmap32double(struct bitmap32* bi,struct bitmap32* bo)
{
	// check
	if (!bi || !bo || bi->size.x*2 != bo->size.x || bi->size.y*2 != bo->size.y)
		errorexit("bitmap32double: invalid args");
	// endcheck
	C32* sp = bi->data;
	C32* dp = bo->data;
	S32 i, j;
	S32 w = bi->size.x;
	S32 h = bi->size.y;
	for (j = 0; j < h; ++j) {
		for (i = 0; i < w; ++i) {
			C32 val = *sp++;
			*dp++ = val;
			*dp++ = val;
		}
		sp -= w;
		for (i = 0; i < w; ++i) {
			C32 val = *sp++;
			*dp++ = val;
			*dp++ = val;
		}
	}
}

bitmap32* bitmap32double(struct bitmap32* b)
{
	bitmap32* bret=bitmap32alloc(b->size.x<<1,b->size.y<<1,C32BLACK);
	bitmap32double(b, bret);
	return bret;
}

#define MINREDUCE 64
#define MINREDUCEX MINREDUCE
#define MINREDUCEY MINREDUCE
bitmap32* bitmap32reduce(const bitmap32* b)
{
	if (b->size.x<=MINREDUCEX || b->size.y<=MINREDUCEY) { // too low a resolution keep it.
		bitmap32* r=bitmap32alloc(b->size.x,b->size.y,C32BLACK);
		clipblit32(b,r,0,0,0,0,b->size.x,b->size.y);
		return r;
	}
	bitmap32* r=bitmap32alloc(b->size.x>>1,b->size.y>>1,C32BLACK); // reduce by 2 on 2 dimensions (1/4 size)
#if 1
	S32 i,j;
	for (j=0;j<r->size.y;++j) {
		for (i=0;i<r->size.x;++i) {
			C32 i0,i1,i2,i3;
			i0=clipgetpixel32(b,2*i,2*j);
			i1=clipgetpixel32(b,2*i+1,2*j);
			i2=clipgetpixel32(b,2*i,2*j+1);
			i3=clipgetpixel32(b,2*i+1,2*j+1);
			U32 sr=((U32)i0.r+i1.r+i2.r+i3.r)>>2;
			U32 sg=((U32)i0.g+i1.g+i2.g+i3.g)>>2;
			U32 sb=((U32)i0.b+i1.b+i2.b+i3.b)>>2;
			U32 sa=((U32)i0.a+i1.a+i2.a+i3.a)>>2;
			C32 v=C32(sr,sg,sb,sa);
			clipputpixel32(r,i,j,v);
		}
	}
// fixup odds
	if (b->size.x&1) { // x has odd, run thru y
		i=b->size.x-1;
		S32 ir=r->size.x-1;
		for (j=0;j<r->size.y;++j) {
			C32 i0,i1;
			i0=clipgetpixel32(b,i,2*j);
			i1=clipgetpixel32(b,i,2*j+1);
			U32 sr=((U32)i0.r+i1.r)>>1;
			U32 sg=((U32)i0.g+i1.g)>>1;
			U32 sb=((U32)i0.b+i1.b)>>1;
			U32 sa=((U32)i0.a+i1.a)>>1;
			C32 v=C32(sr,sg,sb,sa);
			clipputpixel32(r,ir,j,v);
		}
	}
	if (b->size.y&1) { // y has odd, run thru x
		j=b->size.y-1;
		S32 jr=r->size.y-1;
		for (i=0;i<r->size.x;++i) {
			C32 i0,i1;
			i0=clipgetpixel32(b,2*i  ,j);
			i1=clipgetpixel32(b,2*i+1,j);
			U32 sr=((U32)i0.r+i1.r)>>1;
			U32 sg=((U32)i0.g+i1.g)>>1;
			U32 sb=((U32)i0.b+i1.b)>>1;
			U32 sa=((U32)i0.a+i1.a)>>1;
			C32 v=C32(sr,sg,sb,sa);
			clipputpixel32(r,i,jr,v);
		}
	}
	if (b->size.x&1 && b->size.y&1) {
		clipputpixel32(r,r->size.x-1,r->size.y-1,clipgetpixel32(b,b->size.x-1,b->size.y-1));
	}
#else
	clipblit32(b,r,r->size.x>>1,r->size.y>>1,0,0,r->size.x,r->size.y);
#endif
	return r;
}

static recti2 sv;
void setcliprect32(struct bitmap32* s,S32 x,S32 y,S32 xs,S32 ys)
{
	sv=s->cliprect;
//	logger("setting cr to %d %d %d %d\n",x,y,xs,ys);
	if (rclip32(s,&x,&y,&xs,&ys)) {
//		logger("\tset cr to %d %d %d %d\n",x,y,xs,ys);
		s->cliprect.topleft.x=x;
		s->cliprect.topleft.y=y;
		s->cliprect.size.x=xs;
		s->cliprect.size.y=ys;
	} else {
//		logger("\tset cr to zero\n",x,y,xs,ys);
		s->cliprect.topleft.x=0;
		s->cliprect.topleft.y=0;
		s->cliprect.size.x=0;
		s->cliprect.size.y=0;
	}
}

void resetcliprect32(struct bitmap32* s)
{
/*	s->cliprect.topleft.x=0;
	s->cliprect.topleft.y=0;
	s->cliprect.size.x=B32->size.x;
	s->cliprect.size.y=B32->size.y; */
	s->cliprect=sv;
}

////////// console ////////////////////
con32* con32_alloc(S32 w,S32 h,C32 fc,C32 bc)
{
	con32* con;
//	con=(con32*)memalloc(sizeof(con32));
	con=new con32;
	con->f=fc;
	con->b=bc;
	con->x=0;
	con->y=0;
	con->b32=bitmap32alloc(w,h,bc);
	return con;
}

void con32_clear(con32* c)
{
	//return;
	if (!c)
		return;
	fastclear32(c->b32,c->b);
	c->x=0;
	c->y=0;
}

bitmap32* con32_getbitmap32(const con32* c)
{
	if (c)
		return c->b32;
	return 0;
}

void con32_printfvs(const struct con32* con,const C8* fmt,va_list arglist)
{
	static C8 str[40960];
	C8 *strptr=str; // danger, danger!
	vsprintf(str,fmt,arglist);
	//logger("%s",str); // sometimes commented out
	if (!con)
		return;
	while(*strptr) {
		if (*strptr>=' ' && *strptr<=0x7e) {
			while (con->y+8>con->b32->size.y) { // reached bottom, scroll up
				clipblit32(con->b32,con->b32,0,8,0,0,con->b32->size.x,con->b32->size.y-8);
				con->y -= 8;
				cliprect32(con->b32,0,con->b32->size.y-8,con->b32->size.x,8,con->b);
			}
//			logger("U8 %d %d\n",con->x,con->y);
			outchar32(con->b32,con->x,con->y,con->f,*strptr);
			con->x+=8;
			if (con->x + 8 > con->b32->size.x) {
				con->x = 0;
				con->y += 8; // wrap
			}
		} else if (*strptr=='\n') {
			con->x=0;
			con->y+=8;
		}
		strptr++;
	}
}

void con32_printf(const struct con32 *con,const C8* fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	con32_printfvs(con,fmt,arglist);
	va_end(arglist);
}

void con32_printfnow(const struct con32 *con,const C8* fmt,...)
{
	static C8 str[1024];//,*strptr=str;
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(str,fmt,arglist);
	va_end(arglist);
	if (!con)
		return;
	con32_printf(con,"%s",str);
	video_lock();
	clipblit32(con32_getbitmap32(con),B32,0,0,0,0,con->b32->size.x,con->b32->size.y);
	video_unlock();
	checkmessages();
	video_paintwindow(0);
}

void con32_free(struct con32* cn)
{
	if (!cn)
		return;
	bitmap32free(cn->b32);
//	memfree(cn);
	delete cn;
}

struct con32* gcon;

void gcon_printf(const C8* fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	if (gcon) {
		con32_printfvs(gcon,fmt,arglist);
	} else {
		;//loggervs(fmt,arglist); // sometimes commented out
	}
	va_end(arglist);
}

// some kind of checkerboard patterns
void pattern1(bitmap32* b,int pattern)
{
	const C8 stripe = 8;
	C32* dp = b->data;
	S32 i,j;
	S32 w = b->size.x;
	S32 h = b->size.y;
	for (j=0;j<h;++j) {
		for (i=0;i<w;++i) {
			if (1&(i*stripe/w))
				dp->c32 ^= ~0;
			if (1&(j*stripe/h))
				dp->c32 ^= ~0;
			if (pattern)
				dp->c32 ^= ~0;
			++dp;
		}
	}
}

void pattern2(bitmap32* b,int patternx,int patterny)
{
	const C8 stripe = 8;
	C32* dp = b->data;
	S32 i,j;
	S32 w = b->size.x;
	S32 h = b->size.y;
	for (j=0;j<h;++j) {
		for (i=0;i<w;++i) {
			if ((1&(i*stripe/w)) == patternx)
				dp->r = 0xc0;
			if ((1&(j*stripe/h)) == patterny)
				dp->g = 0xc0;
			++dp;
		}
	}
}

// turn opaque black to transparent alpha
map<U32,S32> histo;

void histoshow(string name)
{
	logger("start histo show %s\n",name.c_str());
	S32 histocount = 0;
	map<U32,S32>::iterator it;
	for (it=histo.begin();it!=histo.end();++it) {
		++histocount;
		logger("histo[%08x] = %d\n",it->first,it->second);
	}
	logger("histo count = %d\n",histocount);
	logger("end histo show\n");
}

void makeblackxpar(bitmap32* b,string name)
{
	histo.clear();
#if 0
	histo[3] = 21;
	histo[-1] = 15;
	histo[-1] -= 13;
	histo[5] += 6;
	histo[5] += 5;
	++histo[3];
	++histo[9];
#endif
#if 1
	S32 npix = b->size.x*b->size.y;
	for (S32 i=0;i<npix;++i) {
		U32 v = b->data[i].c32;
		if ((v & 0xffffff) == 0) {
			v = 0;
			b->data[i].c32 = v;
		}
		++histo[v];
		//if ((v.c32 & 0xff) == 0)
		//	v.c32 = 0;
	}
#endif
	//histoshow(name);
}
