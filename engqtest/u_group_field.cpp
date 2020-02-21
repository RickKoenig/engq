#include <m_eng.h>
#include <l_misclibm.h>
#include "u_s_ifield.h"
//#include "u_z2p.h"
#include "u_states.h"

#include "ring2.h"
#include "augmatz.h"

#include "u_group_field.h"

// also any base upto 32
C8 hexchar(S32 i)
{
	if (i>=0 && i<10)
		return i+'0';
	else if (i>=10 && i<32)
		return i-10+'A';
	return '*';
}

S32 distsq(S32 a,S32 b)
{
	return a*a+b*b;
}

void group::setboard(S32 row,S32 col,S32 val)
{
	if (ismul) {
		--row;
		--col;
		--val;
	}
	board[row][col] = val;
}

int group::op(int a,int b) const
{
	if (a == -1)
		return -1;
	if (b == -1)
		return -1;
	int ret = board[a][b];
	return ret;
}

int group::opm(int a,int b) const
{
	if (a <= 0) // 0*b == 0 and -1*b == -1 (no number)
		return a;
	if (b <= 0) // a*0 == 0 and a*-1 == -1 (no number)
		return b;
	int ret = 1 + board[a-1][b-1]; // use index from plus group, answer is in plus group
	return ret;
}

void group::resetge() {
	memset(&ge,0,sizeof(ge)); // pod
}

void group::resetgroup() {
	S32 i,j;
//	fill(rowbuttons,rowbuttons+MAXBSIZE,false);
	fill(colbuttons,colbuttons+MAXBSIZE,false);
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			if (i==0) {
				board[j][i]=j;
			} else if (j==0) {
				board[j][i]=i;
#if 0
			} else if (i==j) {
				board[j][i]=0;
#endif
			} else {
				board[j][i]=-1;
			}

		}
		if (i!=bsize)
			break;
	}
}

void group::backupgroup() // take a non -1 and make a -1, skip 0's
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

bool group::checkgroup()
{
	bool ret = true;
	S32 i,j,k;
	resetge();//memset(&ge,0,sizeof(se)); // pod
// checkidentee, this one should always be true
	for (i=0;i<bsize;++i)
		if (op(0,i)!=i)
			break;
	for (j=0;j<bsize;++j)
		if (op(j,0)!=j)
			break;
	if (i==bsize && j==bsize)
		ge.identee=true;
	else
		ret = false;
// checkm1
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			if (op(j,i)==-1)
				break;
		}
		if (i!=bsize)
			break;
	}
	if (j==bsize)
		ge.complete=true;
	else
		ret = false;
// checkassoc
	for (i=0;i<bsize;++i) {
		for (j=0;j<bsize;++j) {
			for (k=0;k<bsize;++k) {
				S32 v1=op(op(i,j),k);
				S32 v2=op(i,op(j,k));
				if (v1 != v2 && v1!=-1 && v2!=-1) {
					ge.ac=hexchar(i);
					ge.bc=hexchar(j);
					ge.cc=hexchar(k);
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
		ge.assoc=true;
	else
		ret = false;
// check rows
	for (j=0;j<bsize;++j) {
		bool used[MAXBSIZE];
		fill(used,used+bsize,false);
		for (i=0;i<bsize;++i) {
			S32 v=board[j][i];
			if (v==-1)
				continue;
			if (used[v]) {
				ge.rows[j]=true;
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
				ge.cols[i]=true;
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
			break; // propagate fail
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
				ge.identskel=true;
			}
		}
	}
// check abelian, commutative law
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			S32 v1=board[j][i];
			S32 v2=board[i][j];
			if (v1!=-1 && v2!=-1 && v1!=v2) // does ab == ba ?
				break; // fail
		}
		if (i!=bsize)
			break;
	}
	if (j==bsize) { // is diagonal symmetrical
		ge.isabelian=true;
	}
	return ret;
}

void group::swaprows(S32 a,S32 b)
{
	S32 j;
	for (j=0;j<bsize;++j) {
		exch(board[a][j],board[b][j]);
	}
}

void group::swapcols(S32 a,S32 b)
{
	S32 i;
	for (i=0;i<bsize;++i) {
		exch(board[i][a],board[i][b]);
	}
}

void group::swapsymbols(S32 a,S32 b)
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

void group::procbuttons(group* alt) {
// process input
//	static const S32 thr=SQSIZE/3;
	if (!wininfo.mleftclicks && !wininfo.mrightclicks && !wininfo.mmiddleclicks)
		return;
// got a click
	S32 i,j,x,y,xa;
#ifdef LOCKGROUPS
// scan col buttons for input
//	S32 bs = ismul?bsize:bsize-1;
//	S32 offx=OFFX;
	S32 offx=OFFX+SQSIZE;
	S32 offxa = alt->OFFX + SQSIZE;
//	S32 offx=ismul?OFFX+SQSIZE:OFFX;
	S32 ss = 1;
//	S32 ss = ismul?1:2;
	S32 is = ismul?0:1;
	y=OFFY-SQSIZE;
	for (i=ss;i<bsize;++i) {
		x=offx+SQSIZE*i+SQSIZE/2;
		xa=offxa+SQSIZE*i+SQSIZE/2;
		if (distsq(x-MX,y-MY)<SQSIZE*SQSIZE/3/3) {
			colbuttons[i+is]=!colbuttons[i+is];
			break;
		}
		if (distsq(xa-MX,y-MY)<SQSIZE*SQSIZE/3/3) {
			colbuttons[i+is]=!colbuttons[i+is];
			break;
		}
	}
#else
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
	S32 offx=ismul?OFFX+SQSIZE:OFFX;
	y=OFFY-SQSIZE;
	for (i=1;i<bsize;++i) {
		x=offx+SQSIZE*i+SQSIZE/2;
		if (distsq(x-MX,y-MY)<SQSIZE*SQSIZE/3/3) {
			colbuttons[i]=!colbuttons[i];
			break;
		}
	}
#endif
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
	S32 off=ismul?1:0;
	for (j=1;j<bsize;++j) {
		S32 y=OFFY+SQSIZE*(j+off)+SQSIZE/2  + 10 - 4;
		for (i=1;i<bsize;++i) {
			S32 x=OFFX+SQSIZE*(i+off)+SQSIZE/2;
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

void group::drawboard() {
	S32 i,j,w,h;
	S32 offx=ismul?OFFX+SQSIZE:OFFX;
	S32 offy=ismul?OFFY+SQSIZE:OFFY;
	offy += 10;
// draw squares // , double space every 3
	outchar32(B32,OFFX-SQSIZE/2 - 4,OFFY-SQSIZE/2 + 10 - 4,C32WHITE,ismul?'*':'+');
	S32 bs = ismul?bsize+1:bsize;
	for (i=0;i<bs;++i) {
		outchar32(B32,OFFX-SQSIZE/2 - 4 + SQSIZE*i + SQSIZE,OFFY-SQSIZE/2 + 10 - 4,C32WHITE,hexchar(i)); // cols
		outchar32(B32,OFFX-SQSIZE/2 - 4,OFFY-SQSIZE/2 + 10 - 4 + SQSIZE*i + SQSIZE,C32WHITE,hexchar(i)); // rows
	}
	for (i=0;i<bs;++i) {
//		if (i%3==2)
//			w=2;
//		else
			w=1;
		for (j=0;j<bs;++j) {
//			if (j%3==2)
//				h=2;
//			else
				h=1;
			cliprect32(B32,OFFX+SQSIZE*i,OFFY+SQSIZE*j+10,SQSIZE-w,SQSIZE-h,C32WHITE);
		}
	}
// draw numbers
	if (ismul) { // draw 'zeros' for * table
		outtextxybf32(B32,offx+SQSIZE/2-SQSIZE-4,offy+SQSIZE/2-SQSIZE-4,C32BLACK,C32WHITE,"0");
//		outtextxybf32(B32,offx+SQSIZE*i+SQSIZE/2-4,offy+SQSIZE*j+SQSIZE/2-4,C32BLACK,C32WHITE,"%d",v);
		for (i=0;i<bsize;++i) {
			outtextxybf32(B32,offx+SQSIZE*i+SQSIZE/2-4,offy+SQSIZE/2-SQSIZE-4,C32BLACK,C32WHITE,"0");
		}
		for (j=0;j<bsize;++j) {
			outtextxybf32(B32,offx+SQSIZE/2-SQSIZE-4,offy+SQSIZE*j+SQSIZE/2-4,C32BLACK,C32WHITE,"0");
		}
	}
	for (j=0;j<bsize;++j) {
		for (i=0;i<bsize;++i) {
			S32 v=board[j][i];
			if (ismul) {
				//if (v!=-1)
					++v;
			}
			if (v>=0 && v<group::MAXBSIZE)
				outtextxybf32(B32,offx+SQSIZE*i+SQSIZE/2-4,offy+SQSIZE*j+SQSIZE/2-4,C32BLACK,C32WHITE,"%c",hexchar(v));
			else
				outtextxybf32(B32,offx+SQSIZE*i+SQSIZE/2-4,offy+SQSIZE*j+SQSIZE/2-4,C32BLACK,C32WHITE,".",v);
		}
	}
// show row errs
	for (j=0;j<bsize;++j) {
		if (ge.rows[j]) {
			clipline32(B32,offx+SQSIZE/4,offy+SQSIZE*j+SQSIZE/2,offx+SQSIZE*bsize-SQSIZE/4,offy+SQSIZE*j+SQSIZE/2,C32RED);
		}
	}
// show col errs
	for (i=0;i<bsize;++i) {
		if (ge.cols[i]) {
			clipline32(B32,offx+SQSIZE*i+SQSIZE/2,offy+SQSIZE/4,offx+SQSIZE*i+SQSIZE/2,offy+SQSIZE*bsize-SQSIZE/4,C32RED);
		}
	}
}

void group::drawbuttons() {
#ifdef LOCKGROUPS
	S32 i;//,j;
//	S32 bs = ismul?bsize:bsize-1;
	S32 offx=ismul?OFFX+SQSIZE:OFFX;
	S32 ss = ismul?1:2;
	if (bsize - ss < 2)
		return;
// show col buttons
	for (i=ss;i<bsize;++i) {
		C32 c=colbuttons[i]?C32WHITE:C32BLACK;
		clipcircle32(B32,offx+SQSIZE*i+SQSIZE/2,OFFY-SQSIZE,SQSIZE/2,C32MAGENTA);
		clipcircle32(B32,offx+SQSIZE*i+SQSIZE/2,OFFY-SQSIZE,SQSIZE/2-2,c);
	}
#else
S32 i;//,j;
	S32 offx=ismul?OFFX+SQSIZE:OFFX;
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
		clipcircle32(B32,offx+SQSIZE*i+SQSIZE/2,OFFY-SQSIZE,SQSIZE/3,C32MAGENTA);
		clipcircle32(B32,offx+SQSIZE*i+SQSIZE/2,OFFY-SQSIZE,SQSIZE/3-3,c);
	}
#endif
}

//group::grouperror group::geterror() {
//	return ge;
//}

void group::drawerror() {
	const char ascchar=ismul?'*':'+';
	const int RIGHT=20;
	const int DOWN=420;
	S32 OFFX = 20; // override group
	if (ismul)
		OFFX += 300;
	clipcircle32(B32,OFFX+RIGHT,OFFY+DOWN,10,ge.identee?C32GREEN:C32RED);
	outtextxy32(B32,OFFX+30+RIGHT,OFFY-4+DOWN,C32WHITE,"Ident ee");
	clipcircle32(B32,OFFX+RIGHT,OFFY+30+DOWN,10,ge.identskel?C32GREEN:C32RED);
	outtextxy32(B32,OFFX+30+RIGHT,OFFY+30-4+DOWN,C32WHITE,"Ident Skeleton");
	clipcircle32(B32,OFFX+RIGHT,OFFY+60+DOWN,10,ge.assoc?C32GREEN:C32RED);
	outtextxy32(B32,OFFX+30+RIGHT,OFFY+60-4+DOWN,C32WHITE,"Assoc");
	if (!ge.assoc)
		if (ismul)
			outtextxyf32(B32,OFFX+30+6*8+RIGHT,OFFY+60-4+DOWN,C32WHITE,"(%c%c%c)%c%c != %c%c(%c%c%c)",
			  ge.ac+1,ascchar,ge.bc+1,ascchar,ge.cc+1,ge.ac+1,ascchar,ge.bc+1,ascchar,ge.cc+1);
		else
			outtextxyf32(B32,OFFX+30+6*8+RIGHT,OFFY+60-4+DOWN,C32WHITE,"(%c%c%c)%c%c != %c%c(%c%c%c)",
			  ge.ac,ascchar,ge.bc,ascchar,ge.cc,ge.ac,ascchar,ge.bc,ascchar,ge.cc);
	clipcircle32(B32,OFFX+RIGHT,OFFY+90+DOWN,10,ge.complete?C32GREEN:C32RED);
	outtextxy32(B32,OFFX+30+RIGHT,OFFY+90-4+DOWN,C32WHITE,ismul? "Complete (no 0's)" : "Complete (no blanks's)");
	outtextxy32(B32,ge.isabelian?OFFX+30+8+RIGHT:OFFX+30+RIGHT,OFFY+120-4+DOWN,C32WHITE,ge.isabelian?"   ABELIAN":"   NON-ABELIAN");
}


fielderror fe;

fielderror checkfield(const group* ga,const group* gm) {
	fielderror fe;
	memset(&fe,0,sizeof(fe)); // POD
//	int bs=ga->getorder();
	fe.leftdistrib=false;
	fe.rightdistrib=false;
	int a,b,c,ord=ga->getorder();
// left distrib a*(b+c) = a*b + a*c
	for (c=1;c<ord;++c) { // in plus group
		for (b=1;b<ord;++b) {
			for (a=1;a<ord;++a) {
				int v1 = gm->opm(a,ga->op(b,c));
				int v2=ga->op(gm->opm(a,b),gm->opm(a,c));
				if (v1 != v2 && v1!=-1 && v2!=-1) {
					fe.lac=hexchar(a);
					fe.lbc=hexchar(b);
					fe.lcc=hexchar(c);
					break;
				}
#if 0
				int bpc=ga->op(b,c);
				if (bpc==-1)
					continue;
				int atbpc;
				if (bpc==0)
					atbpc=0;
				else
					atbpc=gm->op(a-1,bpc-1)+1;
				if (atbpc==0)
					continue;
				int atb=gm->op(a-1,b-1)+1;
				if (atb==0)
					continue;
				int atc=gm->op(a-1,c-1)+1;
				if (atc==0)
					continue;
				int atbpatc=ga->op(atb,atc);
				if (atbpatc==-1)
					continue;
				if (atbpc!=atbpatc) {
					fe.la=a;
					fe.lb=b;
					fe.lc=c;
					break;
				}
#endif
			}
			if (a!=ord)
				break;
		}
		if (b!=ord)
			break;
	}
	if (c==ord)
		fe.leftdistrib=true;
// right distrib (a+b)*c = a*c + b*c
	for (c=1;c<ord;++c) {
		for (b=1;b<ord;++b) {
			for (a=1;a<ord;++a) {
				int v1 = gm->opm(ga->op(a,b),c);
				int v2=ga->op(gm->opm(a,c),gm->opm(b,c));
				if (v1 != v2 && v1!=-1 && v2!=-1) {
					fe.rac=hexchar(a);
					fe.rbc=hexchar(b);
					fe.rcc=hexchar(c);
					break;
				}
#if 0
				int apb=ga->op(a,b);
				if (apb==-1)
					continue;
				int apbtc;
				if (apb==0)
					apbtc=0;
				else
					apbtc=gm->op(apb-1,c-1)+1;
				if (apbtc==0)
					continue;
				int atc=gm->op(a-1,c-1)+1;
				if (atc==0)
					continue;
				int btc=gm->op(b-1,c-1)+1;
				if (btc==0)
					continue;
				int atcpbtc=ga->op(atc,btc);
				if (atcpbtc==-1)
					continue;
				if (apbtc!=atcpbtc) {
					fe.ra=a;
					fe.rb=b;
					fe.rc=c;
					break;
				}
#endif
			}
			if (a!=ord)
				break;
		}
		if (b!=ord)
			break;
	}
	if (c==ord)
		fe.rightdistrib=true;
	return fe;
}
