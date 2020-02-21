#include <m_eng.h>

#define MAXSIZE 16384
#define MAXPICS 3000 // see gfxview.c , MAXGBLOCK
enum {EMPTY,FULL,PARTIAL};
struct quadnode {
	int cover;
	struct quadnode *ul,*ur,*ll,*lr;
};


static struct quadnode *topnode;
enum {HC,VC};
struct corner {
	int x,y,kind,bx,by;
};

static struct corner **corners;
static int ncorners;

int maxgfxx,maxgfxy;

static void addcorner(int x,int y,int kind)
{
	if (ncorners>=MAXPICS)
		errorexit("too many corners");
	corners[ncorners]=new corner; //memalloc(sizeof(struct corner));
	corners[ncorners]->x=x;
	corners[ncorners]->y=y;
	corners[ncorners]->kind=kind;
	ncorners++;
}

static void remcorner(int idx)
{
	ncorners--;
	delete corners[idx];
	if (idx!=ncorners)
		corners[idx]=corners[ncorners];
}

void initlocfinder()
{
	maxgfxx=maxgfxy=0;
//	memzalloc(sizeof(struct quadnode));
	topnode=new quadnode;
	memset(topnode,0,sizeof(*topnode));
//	corners=memalloc(sizeof(struct corner *)*MAXPICS);
	corners=new corner*[MAXPICS];
	ncorners=0;
	addcorner(0,0,HC);
}

static void freelocfinderr(struct quadnode *n)
{
	if (n->cover==PARTIAL) {
		freelocfinderr(n->ul);
		freelocfinderr(n->ur);
		freelocfinderr(n->ll);
		freelocfinderr(n->lr);
	}
	delete n;
}

void freelocfinder()
{
	int i;
	freelocfinderr(topnode);
	for (i=0;i<ncorners;i++)
		delete corners[i];
	delete[] corners;
}

static void addrectr(struct quadnode *n,int x0,int y0,int x1,int y1,int cx0,int cy0,int cx1,int cy1)
{
	int cover;
	if (n->cover==FULL)
		return;
	if (x0<=cx0 && y0<=cy0 && x1>=cx1 && y1>=cy1)
		cover=FULL;
	else if (x0>cx1 || y0>cy1 || x1<cx0 || y1<cy0)
		cover=EMPTY;
	else
		cover=PARTIAL;
	if (cover==EMPTY)
		return;
	if (n->cover==PARTIAL) {
		if (cover==PARTIAL) { // recurse
			int hx,hy;
			hx=(cx0+cx1+1)>>1;
			hy=(cy0+cy1+1)>>1;
			addrectr(n->ul,x0,y0,x1,y1,cx0,cy0,hx-1,hy-1);
			addrectr(n->ur,x0,y0,x1,y1,hx,cy0,cx1,hy-1);
			addrectr(n->ll,x0,y0,x1,y1,cx0,hy,hx-1,cy1);
			addrectr(n->lr,x0,y0,x1,y1,hx,hy,cx1,cy1);
		} else {// cover==FULL) {
			freelocfinderr(n->ul);
			freelocfinderr(n->ur);
			freelocfinderr(n->ll);
			freelocfinderr(n->lr);
			n->cover=FULL;
		}
	} else {// n->cover==EMPTY) {
		if (cover==PARTIAL) {
			int hx,hy;
			hx=(cx0+cx1+1)>>1;
			hy=(cy0+cy1+1)>>1;
			n->cover=PARTIAL;
			n->ul=new quadnode; //memzalloc(sizeof(struct quadnode));
			memset(n->ul,0,sizeof(quadnode));
			n->ur=new quadnode; //memzalloc(sizeof(struct quadnode));
			memset(n->ur,0,sizeof(quadnode));
			n->ll=new quadnode; //memzalloc(sizeof(struct quadnode));
			memset(n->ll,0,sizeof(quadnode));
			n->lr=new quadnode; //memzalloc(sizeof(struct quadnode));
			memset(n->lr,0,sizeof(quadnode));
			addrectr(n->ul,x0,y0,x1,y1,cx0,cy0,hx-1,hy-1);
			addrectr(n->ur,x0,y0,x1,y1,hx,cy0,cx1,hy-1);
			addrectr(n->ll,x0,y0,x1,y1,cx0,hy,hx-1,cy1);
			addrectr(n->lr,x0,y0,x1,y1,hx,hy,cx1,cy1);
		} else {// cover==FULL) {
			n->cover=FULL;
		}
	}
	return;
}

static void addrect(int x0,int y0,int xs,int ys)
{
	addrectr(topnode,x0,y0,x0+xs-1,y0+ys-1,0,0,MAXSIZE-1,MAXSIZE-1);
}

static int checkrectr(struct quadnode *n,int x0,int y0,int x1,int y1,int cx0,int cy0,int cx1,int cy1)
{
	int hx,hy;
	if (x0>cx1 || y0>cy1 || x1<cx0 || y1<cy0)
		return 1;	// rectangle not on this node
// rectange on this node
	if (n->cover==FULL)
		return 0;
	if (n->cover==EMPTY)
		return 1;
	hx=(cx0+cx1+1)>>1;
	hy=(cy0+cy1+1)>>1;
	if (!checkrectr(n->ul,x0,y0,x1,y1,cx0,cy0,hx-1,hy-1))
		return 0;
	if (!checkrectr(n->ur,x0,y0,x1,y1,hx,cy0,cx1,hy-1))
		return 0;
	if (!checkrectr(n->ll,x0,y0,x1,y1,cx0,hy,hx-1,cy1))
		return 0;
	if (!checkrectr(n->lr,x0,y0,x1,y1,hx,hy,cx1,cy1))
		return 0;
	return 1;
}

static int checkrect(int x,int y,int sx,int sy)
{
	if (x+sx>MAXSIZE)
		return 0;
	if (y+sy>MAXSIZE)
		return 0;
	return checkrectr(topnode,x,y,x+sx-1,y+sy-1,0,0,MAXSIZE-1,MAXSIZE-1);
}

// draw a line from x,y to 0,y and return last empty x,(y)
static int checkbackxr(struct quadnode *n,int x,int y,int cx0,int cy0,int cx1,int cy1)
{
	int ret1,ret2,ret3,ret4,hx,hy;
	if (x<cx0)
		return 0;
	if (y<cy0 || y>cy1) // line outside of square
		return 0;
// line passes thru square
	if (n->cover==EMPTY)
		return 0;
	if (n->cover==FULL)
		return min(x,cx1+1);
	hx=(cx0+cx1+1)>>1;
	hy=(cy0+cy1+1)>>1;
	ret1=checkbackxr(n->ul,x,y,cx0,cy0,hx-1,hy-1);
	ret2=checkbackxr(n->ur,x,y,hx,cy0,cx1,hy-1);
	ret3=checkbackxr(n->ll,x,y,cx0,hy,hx-1,cy1);
	ret4=checkbackxr(n->lr,x,y,hx,hy,cx1,cy1);
	return max(max(ret1,ret2),max(ret3,ret4));	
}

static int checkbackx(int x,int y)
{
	return checkbackxr(topnode,x,y,0,0,MAXSIZE-1,MAXSIZE-1);
}

static int checkbackyr(struct quadnode *n,int x,int y,int cx0,int cy0,int cx1,int cy1)
{
	int ret1,ret2,ret3,ret4,hx,hy;
	if (y<cy0)
		return 0;
	if (x<cx0 || x>cx1) // line outside of square
		return 0;
// line passes thru square
	if (n->cover==EMPTY)
		return 0;
	if (n->cover==FULL)
		return min(y,cy1+1);
	hx=(cx0+cx1+1)>>1;
	hy=(cy0+cy1+1)>>1;
	ret1=checkbackyr(n->ul,x,y,cx0,cy0,hx-1,hy-1);
	ret2=checkbackyr(n->ur,x,y,hx,cy0,cx1,hy-1);
	ret3=checkbackyr(n->ll,x,y,cx0,hy,hx-1,cy1);
	ret4=checkbackyr(n->lr,x,y,hx,hy,cx1,cy1);
	return max(max(ret1,ret2),max(ret3,ret4));	
}

static int checkbacky(int x,int y)
{
	return checkbackyr(topnode,x,y,0,0,MAXSIZE-1,MAXSIZE-1);
}

static void showlocfinderr(struct quadnode *n,int sclx,int scly,int cx0,int cy0,int cx1,int cy1)
{
	int x0,y0,x1,y1;
	if (n->cover==EMPTY) {
		x0=cx0-sclx;
		y0=cy0-scly;
		x1=cx1-sclx;
		y1=cy1-scly;
		cliprect32(B32,x0,y0,x1,y1,C32LIGHTGRAY);
		cliprecto32(B32,x0,y0,x1,y1,C32BLACK);
		clipline32(B32,x0,y0,x1,y1,C32BLACK);
		clipline32(B32,x0,y1,x1,y0,C32BLACK);
		return;
	}
/*	if (n->cover==FULL) {
		x0=cx0-sclx;
		y0=cy0-scly;
		x1=cx1-sclx;
		y1=cy1-scly;
		cliprect16(B16,x0,y0,x1,y1,hilightblue);
		cliprecto16(B16,x0,y0,x1,y1,hiblack);
		clipline16(B16,x0,y0,x1,y1,hiblack);
		clipline16(B16,x0,y1,x1,y0,hiblack);
		return;
	} */
	if (n->cover==PARTIAL) {
		int hx,hy;
		hx=(cx0+cx1+1)>>1;
		hy=(cy0+cy1+1)>>1;
		showlocfinderr(n->ul,sclx,scly,cx0,cy0,hx-1,hy-1);
		showlocfinderr(n->ur,sclx,scly,hx,cy0,cx1,hy-1);
		showlocfinderr(n->ll,sclx,scly,cx0,hy,hx-1,cy1);
		showlocfinderr(n->lr,sclx,scly,hx,hy,cx1,cy1);
	}
}

void showlocfinder(int sclx,int scly)
{
	static char cc[2]={'H','V'};
	int i;
	showlocfinderr(topnode,sclx,scly,0,0,MAXSIZE-1,MAXSIZE-1);
	for (i=0;i<ncorners;i++) {
		outtextxybf32(B32,corners[i]->x-sclx,corners[i]->y-scly,C32WHITE,C32BLACK,"%d %c",i,cc[corners[i]->kind]);
//		cliprect16(B16,corners[i]->x-sclx,corners[i]->y-scly,
//			corners[i]->x+4-sclx,corners[i]->y+4-scly,hiwhite);
	}
	outtextxybf32(B32,0,0,C32BLACK,C32WHITE,"%d",checkbackx(80,440));
}


static int csx,csy;
static bool compcorners(struct corner *a,struct corner* b)
{
	int amx,bmx,amy,bmy;
	amx=a->x+csx;
	amy=a->y+csy;
	bmx=b->x+csx;
	bmy=b->y+csy;
	if (amx>0 || amy>0)
		if (bmx>0 || bmy>0)
			return (max(bmx+maxgfxx,bmy+maxgfxy)-max(amx+maxgfxx,amy+maxgfxy))>0;
		else
			return false;
	else
		if (bmx>0 || bmy>0)
			return true;
		else
			return ((max(bmx,bmy)-max(amx,amy))>0); 
}

void getnewloc(int sizex,int sizey,int *locx,int *locy)
{
	int i,x,y;
	*locx=maxgfxx;
	*locy=0;
	for (i=0;i<ncorners;i++) {
		if (corners[i]->kind==HC) {
			corners[i]->bx=checkbackx(corners[i]->x,corners[i]->y);
			corners[i]->by=corners[i]->y;
		} else {
			corners[i]->bx=corners[i]->x;
			corners[i]->by=checkbacky(corners[i]->x,corners[i]->y);
		}
	}
	csx=sizex-maxgfxx;
	csy=sizey-maxgfxy;
//	nuqsort(corners,ncorners,compcorners);
	sort(corners,corners+ncorners,compcorners);
	for (i=0;i<ncorners;i++) {
		x=corners[i]->bx;
		y=corners[i]->by;
		if (checkrect(x,y,sizex,sizey)) {
			addrect(x,y,sizex,sizey);
			if (x+sizex>maxgfxx)
				maxgfxx=x+sizex;
			if (y+sizey>maxgfxy)
				maxgfxy=y+sizey;
			*locx=x;
			*locy=y;
			remcorner(i);
			addcorner(x+sizex,y,VC);
			addcorner(x,y+sizey,HC);
			break;
		}
	}
	if (i==ncorners)
		errorexit("no room");
}
