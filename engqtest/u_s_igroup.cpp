#include <m_eng.h>
#include <l_misclibm.h>
#include "u_s_igroup.h"
#include "u_states.h"

#define USENS // use namespace, turn off for debugging
#ifdef USENS
namespace {
#endif

// ui
shape* rl;
shape* focus,*oldfocus;

pbut* bquit,*bbackup,*bsmaller,*bbigger,*breset;
// end ui;

#define SQSIZE 24
#define OFFX 50
#define OFFY 50

#define MAXBSIZE 21
S32 bsize=4;
S32 board[MAXBSIZE][MAXBSIZE];
S32 resetidx;
#if 0
#define BSIZE 4
//#define BSIZE 9

#if BSIZE==4
//#define BOARDC2xC2
#define BOARDC4
#ifdef BOARDC2xC2
S32 board[BSIZE][BSIZE]={
	{0,1,2,3},
	{1,0,3,2},
	{2,3,0,1},
	{3,2,1,0},
};
#endif
#ifdef BOARDC4 // notice how all but 27 entries are the same (54 the same)
S32 board[BSIZE][BSIZE]={
	{0,1,2,3},
	{1,2,3,0},
	{2,3,0,1},
	{3,0,1,2},
};
#endif
#endif
#if BSIZE==9
#define BOARDC3xC3
//#define BOARDC9
#ifdef BOARDC3xC3
S32 board[BSIZE][BSIZE]={
	{0,1,2, 3,4,5, 6,7,8},
	{1,2,0, 4,5,3, 7,8,6},
	{2,0,1, 5,3,4, 8,6,7},

	{3,4,5, 6,7,8, 0,1,2},
	{4,5,3, 7,8,6, 1,2,0},
	{5,3,4, 8,6,7, 2,0,1},

	{6,7,8, 0,1,2, 3,4,5},
	{7,8,6, 1,2,0, 4,5,3},
	{8,6,7, 2,0,1, 5,3,4}
};
#endif
#ifdef BOARDC9 // notice how all but 27 entries are the same (54 the same)
S32 board[BSIZE][BSIZE]={
	{0,1,2, 3,4,5, 6,7,8},
	{1,2,0, 4,5,3, 7,8,6},
	{2,0,1, 5,3,4, 8,6,7},

	{3,4,5, 6,7,8, 1,2,0},
	{4,5,3, 7,8,6, 2,0,1},
	{5,3,4, 8,6,7, 0,1,2},

	{6,7,8, 1,2,0, 4,5,3},
	{7,8,6, 2,0,1, 5,3,4},
	{8,6,7, 0,1,2, 3,4,5}
};
#endif
#endif
#endif
struct grouperror { // true if err
// must all be true to be a group
	bool identee; // ident is at 0,0
	bool identskel; // valid ident skeleton
	bool assoc; // passes associativity test
	S32 a,b,c; // if fail assoc
	bool complete;
	bool rows[MAXBSIZE];
	bool cols[MAXBSIZE];
// optional
	bool isabelian;
};
grouperror se;
//bool rowbuttons[MAXBSIZE];
bool colbuttons[MAXBSIZE];

void resetgroup()
{
	S32 i,j;
//	fill(rowbuttons,rowbuttons+MAXBSIZE,false);
	fill(colbuttons,colbuttons+MAXBSIZE,false);
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			if (i==0) {
				board[j][i]=j;
			} else if (j==0) {
				board[j][i]=i;
			} else if (i==j) {
				board[j][i]=0;
			} else {
				board[j][i]=-1;
			}
		
		}
		if (i!=bsize)
			break;
	}
}

void backupgroup() // take a non -1 and make a -1, skip 0's
{
	S32 i,j;
	for (j=bsize-1;j>0;--j) {
		for (i=bsize-1;i>0;--i) {
			S32 v=board[j][i];
			if (v!=-1 && v!=0) {
				board[j][i]=-1;
				return;
			}
		}
	}
}

void checkgroup()
{
	S32 i,j,k;
	memset(&se,0,sizeof(se));
// checkidentee
	for (i=0;i<bsize;++i)
		if (board[0][i]!=i)
			return;
	for (j=0;j<bsize;++j)
		if (board[j][0]!=j)
			return;
	se.identee=true;
// checkm1
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			if (board[j][i]==-1)
				break;
		}
		if (i!=bsize)
			break;
	}
	if (j==bsize)
		se.complete=true;
// checkassoc
	for (i=0;i<bsize;++i) {
		for (j=0;j<bsize;++j) {
			for (k=0;k<bsize;++k) {
				S32 ab=board[i][j];
				if (ab==-1)
					continue;
				S32 v1=board[ab][k];
				S32 bc=board[j][k];
				if (bc==-1)
					continue;
				S32 v2=board[i][bc];
				if (v1 != v2 && v1!=-1 && v2!=-1) {
					se.a=i;
					se.b=j;
					se.c=k;
					break;
				}
			}
			if (k!=bsize)
				break;
		}
		if (j!=bsize)
			break;
	}
	if (i==bsize)
		se.assoc=true;
// check rows
	for (j=0;j<bsize;++j) {
		bool used[MAXBSIZE];
		fill(used,used+bsize,false);
		for (i=0;i<bsize;++i) {
			S32 v=board[j][i];
			if (v==-1)
				continue;
			if (used[v]) {
				se.rows[j]=true;
				break;
			}
			used[v]=true;
		}
	}
// check cols
	for (i=0;i<bsize;++i) {
		bool used[MAXBSIZE];
		fill(used,used+bsize,false);
		for (j=0;j<bsize;++j) {
			S32 v=board[j][i];
			if (v==-1)
				continue;
			if (used[v]) {
				se.cols[i]=true;
				break;
			}
			used[v]=true;
		}
	}
// check ident skel
	S32 rowcnt[MAXBSIZE]; // ident in each row
	S32 colcnt[MAXBSIZE]; // ident in each col
	fill(rowcnt,rowcnt+bsize,0);
	fill(colcnt,colcnt+bsize,0);
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			if (board[j][i]!=0)
				continue; // ignore
			if (board[i][j]!=0) // does ab = e = ba ?
				break; // fail
			++rowcnt[j]; // 1 ident per row/col
			++colcnt[i];
		}
		if (i!=bsize)
			break;
	}
	if (j==bsize) { // is diagonal symmetrical
		for (j=0;j<bsize;++j) {
			if (rowcnt[j]!=1) {
				break;
			}
		}
		if (j==bsize) { // all rows check out
			for (i=0;i<bsize;++i) {
				if (colcnt[i]!=1) {
					break;
				}
			}
			if (i==bsize) { // all cols check out
				se.identskel=true;
			}
		}
	}
// check abelian, communative law
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			S32 v1=board[j][i];
			S32 v2=board[i][j];
			if (v1!=v2 && v1!=-1 && v2!=-1) // does ab == ba ?
				break; // fail
		}
		if (i!=bsize)
			break;
	}
	if (j==bsize) { // is diagonal symmetrical
		se.isabelian=true;
	}
}

S32 distsq(S32 a,S32 b)
{
	return a*a+b*b;
}

void swaprows(S32 a,S32 b)
{
	S32 j;
	for (j=0;j<bsize;++j) {
		exch(board[a][j],board[b][j]);
	}
}

void swapcols(S32 a,S32 b)
{
	S32 i;
	for (i=0;i<bsize;++i) {
		exch(board[i][a],board[i][b]);
	}
}

void swapsymbols(S32 a,S32 b)
{
	S32 i,j;
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			S32 v=board[j][i];
			if (v==a)
				v=b;
			else if (v==b)
				v=a;
			board[j][i]=v;
		}
	}
}

void procbuttons()
{
// process input
	static const S32 thr=SQSIZE/3;
	if (!wininfo.mleftclicks && !wininfo.mrightclicks && !wininfo.mmiddleclicks)
		return;
// got a click
	S32 i,j,x,y;
#if 0
// scan row buttons for intput
	x=OFFX-SQSIZE;
	for (j=1;j<bsize;++j) {
		y=OFFY+SQSIZE*j+SQSIZE/2;
		if (distsq(x-MX,y-MY)<SQSIZE*SQSIZE/3/3) {
			rowbuttons[j]=!rowbuttons[j];
			break;
		}
	}
#endif
// scan col buttons for input
	y=OFFY-SQSIZE;
	for (i=1;i<bsize;++i) {
		x=OFFX+SQSIZE*i+SQSIZE/2;
		if (distsq(x-MX,y-MY)<SQSIZE*SQSIZE/3/3) {
			colbuttons[i]=!colbuttons[i];
			break;
		}
	}
#if 0
// see if 2 row buttons pressed
	i=-1;
	for (j=0;j<bsize;++j) {
		if (rowbuttons[j]) {
			if (i==-1)
				i=j;
			else {
				rowbuttons[j]=false;
				rowbuttons[i]=false;
				swaprows(i,j);
				break;
			}
		}
	}
#endif
// see if 2 col buttons pressed
	j=-1;
	for (i=0;i<bsize;++i) {
		if (colbuttons[i]) {
			if (j==-1)
				j=i;
			else {
				colbuttons[i]=false;
				colbuttons[j]=false;
				swapcols(i,j);
				swaprows(i,j);
				swapsymbols(i,j);
				break;
			}
		}
	}
// board buttons
	for (j=1;j<bsize;++j) {
		S32 y=OFFY+SQSIZE*j+SQSIZE/2;
		for (i=1;i<bsize;++i) {
			S32 x=OFFX+SQSIZE*i+SQSIZE/2;
			if (distsq(x-MX,y-MY)<SQSIZE*SQSIZE/2/2) {
				S32 v=board[j][i];
				if (wininfo.mrightclicks) {
					++v;
					if (v==bsize)
						v=-1;
				} else if (wininfo.mleftclicks) {
					--v;
					if (v<-1)
						v=bsize-1;
				} else if (wininfo.mmiddleclicks) {
					v=-1;
				}
				board[j][i]=v;
				break;
			}
		}
	}
}

void drawboard()
{
	S32 i,j,w,h;
// draw squares // , double space every 3
	for (i=0;i<bsize;++i) {
//		if (i%3==2)
//			w=2;
//		else
			w=1;
		for (j=0;j<bsize;++j) {
//			if (j%3==2)
//				h=2;
//			else
				h=1;
			cliprect32(B32,OFFX+SQSIZE*i,OFFY+SQSIZE*j,SQSIZE-w,SQSIZE-h,C32WHITE);
		}
	}
// show row errs
	for (j=0;j<bsize;++j) {
		if (se.rows[j]) {
			clipline32(B32,OFFX+SQSIZE/4,OFFY+SQSIZE*j+SQSIZE/2,OFFX+SQSIZE*bsize-SQSIZE/4,OFFY+SQSIZE*j+SQSIZE/2,C32RED);
		}
	}
// show col errs
	for (i=0;i<bsize;++i) {
		if (se.cols[i]) {
			clipline32(B32,OFFX+SQSIZE*i+SQSIZE/2,OFFY+SQSIZE/4,OFFX+SQSIZE*i+SQSIZE/2,OFFY+SQSIZE*bsize-SQSIZE/4,C32RED);
		}
	}
// draw numbers
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			S32 v=board[j][i];
			if (v<10 && v>=0)
				outtextxybf32(B32,OFFX+SQSIZE*i+SQSIZE/2-4,OFFY+SQSIZE*j+SQSIZE/2-4,C32BLACK,C32WHITE,"%d",v);
			else
				outtextxybf32(B32,OFFX+SQSIZE*i+SQSIZE/2-8,OFFY+SQSIZE*j+SQSIZE/2-4,C32BLACK,C32WHITE,"%2d",v);
		}
	}
}

void drawbuttons()
{
	S32 i;//,j;
#if 0
// show row buttons
	for (j=1;j<BSIZE;++j) {
		C32 c=rowbuttons[j]?C32WHITE:C32BLACK;
		clipcircle32(B32,OFFX-SQSIZE,OFFY+SQSIZE*j+SQSIZE/2,SQSIZE/3,C32MAGENTA);
		clipcircle32(B32,OFFX-SQSIZE,OFFY+SQSIZE*j+SQSIZE/2,SQSIZE/3-3,c);
	}
#endif
// show col buttons
	for (i=1;i<bsize;++i) {
		C32 c=colbuttons[i]?C32WHITE:C32BLACK;
		clipcircle32(B32,OFFX+SQSIZE*i+SQSIZE/2,OFFY-SQSIZE,SQSIZE/3,C32MAGENTA);
		clipcircle32(B32,OFFX+SQSIZE*i+SQSIZE/2,OFFY-SQSIZE,SQSIZE/3-3,c);
	}
}

#ifdef USENS
}
#endif

void igroup_init()
{
	video_setupwindow(800,600);
	pushandsetdir("igroup");
// ui
	rl=res_loadfile("igroupres.txt");
	bquit=rl->find<pbut>("BQUIT");
	bsmaller=rl->find<pbut>("BSMALLER");
	bbigger=rl->find<pbut>("BBIGGER");
	breset=rl->find<pbut>("BRESET");
	bbackup=rl->find<pbut>("BBACKUP");

	focus=oldfocus=0;
// end ui
	resetidx=0;
	resetgroup();
}

void igroup_proc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
// ui
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
// quit
	if (focus == bquit) {
		if (ret==1) {
			poporchangestate(STATE_MAINMENU);
		}
	} else if (focus == bbackup) {
		if (ret==1) {
			backupgroup();
		}
	} else if (focus == bsmaller) {
		if (ret==1) {
			if (bsize>1) {
				--bsize;
				resetgroup();
			}
		}
	} else if (focus == bbigger) {
		if (ret==1) {
			if (bsize<MAXBSIZE) {
				++bsize;
				resetgroup();
			}
		}
	} else if (focus == breset) {
		if (ret==1) {
			resetgroup();
		}
	}
// end ui
	procbuttons();
	checkgroup();
}

void igroup_draw2d()
{
	clipclear32(B32,C32BLUE);
//	C32 bs=goodboard?C32GREEN:C32RED;
	clipcircle32(B32,600,30,10,se.identee?C32GREEN:C32RED);
	outtextxy32(B32,630,30-4,C32WHITE,"Ident ee");
	clipcircle32(B32,600,70,10,se.identskel?C32GREEN:C32RED);
	outtextxy32(B32,630,70-4,C32WHITE,"Ident Skeleton");
	clipcircle32(B32,600,110,10,se.assoc?C32GREEN:C32RED);
	outtextxy32(B32,630,110-4,C32WHITE,"Association");
	if (!se.assoc)
		outtextxyf32(B32,630+12*8,110-4,C32WHITE,"%d*%d*%d",se.a,se.b,se.c);
	clipcircle32(B32,600,150,10,se.complete?C32GREEN:C32RED);
	outtextxy32(B32,630,150-4,C32WHITE,"Complete (no -1's)");
	outtextxy32(B32,se.isabelian?630+8:630,190-4,C32WHITE,se.isabelian?"ABELIAN":"NON-ABELIAN");
	drawboard();
	drawbuttons();
	rl->draw();
}

void igroup_exit()
{
	popdir();
	delete rl;
}
 