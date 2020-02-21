#include <m_eng.h>
#include "d2_font.h"

namespace wriggler {

#define MAXBOARDX 32
#define MAXBOARDY 32

#define MAXNUMPIECES 32 // a power of 2

struct tile {
	bool iswall;
	bool ispiece;
	S32 piecenum;
	bool isgoal;
	S32 goalnum;
	S32 ndirs;
	pointi2 dirs[2];
	void adddir(S32 x,S32 y);
};

void tile::adddir(S32 x,S32 y)
{
	if (ndirs==2)
		errorexit("too many dirs");
	dirs[ndirs].x=x;
	dirs[ndirs].y=y;
	++ndirs;
}

// graphics
#define STARTX 30
#define STARTY 30
#define SQSIZE 35


//#define HASHSIZE 0x10000 // a power of 2
#define HASHSIZE 0x200000 // a power of 2
//#define HASHSIZE 32768 // a power of 2

// prototype level data, loaded from file

struct level {
	int nbx,nby;
	tile tiles[MAXBOARDY][MAXBOARDX];
};

union pointS82 {
	struct {
		S8 x: 4;
		S8 y: 4;
	};
	U8 xy;
};

struct pointU82 {
	U8 x : 4;
	U8 y : 4;
};

struct piece {
	pointS82 p;
	S8 n;
	S8 link[2];
//	pointS82 link[2];
};

// a board position
struct board {
	static level lev;
	static int numpieces;
	static int ngoals;
	int curpiece;
	piece pieces[MAXNUMPIECES];
//	pointi2 playerpos;

	int hashfunc() const;
	bool beq(const board& b2) const;
	board(const char* s);
	tile parse_tile(const char* t);
	S32 findpieceatxy(S32 x,S32 y);
	S32 getotherend(S32 stpn);
	void moveworm(S32 st,pointi2 dir);
};

level board::lev;
int board::numpieces;
int board::ngoals;

S32 board::findpieceatxy(S32 x,S32 y)
{
	S32 k;
	for (k=0;k<numpieces;++k)
		if (pieces[k].p.x==x && pieces[k].p.y==y)
			return k;
	return -1;
}

void board::moveworm(S32 st,pointi2 dir)
{
	pointS82 oldpos=pieces[st].p;
	pieces[st].p.x+=dir.x;
	pieces[st].p.y+=dir.y;
	pointS82 revbuff[MAXNUMPIECES];
	S32 watch=0;
//	logger("w %d, p %d %d\n",watch,pieces[st].p.x,pieces[st].p.y);
	revbuff[watch++]=pieces[st].p;
	S32 path; // 0 or 1
	if (pieces[st].link[0]==-1) {
		if (pieces[st].link[1]==-1)
			return;
		path=1;
	} else {
		if (pieces[st].link[1]!=-1)
			errorexit("can't move middle");
		path=0;
	}
	S32 cl=st;
	while(1) {
		cl=pieces[cl].link[path]; // move along worm
		pointS82 t=pieces[cl].p;
		pieces[cl].p=oldpos;
//		logger("w %d, p %d %d\n",watch,oldpos.x,oldpos.y);
		revbuff[watch++]=oldpos;
		if (watch==50)
			errorexit("worm watch %d",watch);
		oldpos=t;
		if (pieces[cl].link[path]==-1) { 
			break;
		}
	}
// sort
//	logger("st = %d, cl = %d\n",st,cl);
	bool dosort=false;
	if (st<cl) {
		if (pieces[st].p.y>pieces[cl].p.y)
			dosort=true;
		else if (pieces[st].p.y==pieces[cl].p.y && pieces[st].p.x>pieces[cl].p.x)
			dosort=true;
	} else {
		if (pieces[st].p.y<pieces[cl].p.y)
			dosort=true;
		else if (pieces[st].p.y==pieces[cl].p.y && pieces[st].p.x<pieces[cl].p.x)
			dosort=true;
	}
	if (dosort) {
		curpiece=cl;
		cl=st;
		while(1) {
			--watch;
			if (watch<0)
				errorexit("worm watch2",watch);
//			logger("w %d, po %d %d, pn %d %d\n",
//				watch,pieces[cl].p.x,pieces[cl].p.y,revbuff[watch].x,revbuff[watch].y);
			pieces[cl].p=revbuff[watch];
			if (pieces[cl].link[path]==-1)
				break;
//			logger("path %d to %d\n",cl,pieces[cl].link[path]);
			cl=pieces[cl].link[path]; // move along worm
		} 
	}
}

/*
// NO. this also alters 4 links
// YES. this also moves worm
S32 board::getotherend(S32 st)
{
	S32 path; // 0 or 1
	if (pieces[st].link[0]==-1) {
		if (pieces[st].link[1]==-1)
			return st;
		path=1;
	} else {
		if (pieces[st].link[1]!=-1)
			errorexit("can't move middle");
		path=0;
	}
	S32 cl=st;
	S32 watch=0;
	while(1) {
		cl=pieces[cl].link[path]; // move along worm
		++watch;
		if (watch==50)
			errorexit("worm watch %d",watch);
		if (pieces[cl].link[path]==-1) { // link surgery
			pieces[st].link[path]=-1;
			pieces[cl].link[1-path]=-1;
			pieces[st].link[1-path]=cl;
			pieces[cl].link[path]=st;
			return cl;
		}
	}
	return -1;
}
*/
int board::hashfunc() const // mask is 2^n-1
{
	S32 i;
	S32 ret=0;
	for (i=0;i<numpieces;++i) {
		ret<<=1;
		ret+=pieces[i].p.x+(pieces[i].p.y<<1);
		ret+=pieces[i].n<<1;
	}
	return ret & (HASHSIZE-1);
}

bool board::beq(const board& b2) const
{
	int i;
	for (i=0;i<numpieces;++i)
		if (pieces[i].p.x!=b2.pieces[i].p.x || pieces[i].p.y!=b2.pieces[i].p.y || pieces[i].n!=b2.pieces[i].n)
			return false;
	return true;
}

int advance_atoi(const char** sp)
{
	int ret=0;
	while (**sp>='0' && **sp<='9') { 
		ret = ret*10 + **sp-'0';
		++ *sp;
	}
	return ret;
}

tile board::parse_tile(const char* t)
{
	tile ret;
	memset(&ret,0,sizeof(ret));
	const char* ts=t;
	while(*t) {
		if (*t=='.') {
			++t;
		} else if (*t=='W') {
			ret.iswall=true;
			++t;
		} else if (*t=='G') {
			ret.isgoal=true;
			++t;
			ret.goalnum=advance_atoi(&t);
		} else if (*t=='P') {
			++t;
			ret.ispiece=true;
			ret.piecenum=advance_atoi(&t);
		} else if (*t=='U') {
			ret.adddir(0,-1);
			++t;
		} else if (*t=='D') {
			ret.adddir(0,1);
			++t;
		} else if (*t=='L') {
			ret.adddir(-1,0);
			++t;
		} else if (*t=='R') {
			ret.adddir(1,0);
			++t;
		} else if (*t=='X') {
			ret.adddir(0,0);
			++t;
		} else
			errorexit("bad parsetile: '%s'",ts);
	}
	return ret;
}

board::board(const char* levelfile)
{
	S32 sp=0;
	script lsc(levelfile);
	if (lsc.num()<2)
		errorexit("bad level");
	lev.nbx=atoi(lsc.idx(sp++).c_str());
	lev.nby=atoi(lsc.idx(sp++).c_str());
	if (lev.nbx>MAXBOARDX || lev.nby>MAXBOARDY)
		errorexit("board too big");
	if (lsc.num() < sp + lev.nbx * lev.nby)
		errorexit("bad level");
// load level
	int i,j;
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			const C8* tp=lsc.idx(sp+j*lev.nbx+i).c_str();
			tile& v=lev.tiles[j][i];
			v=parse_tile(tp);
		}
	}
	sp+=lev.nbx*lev.nby;
// make sure has a border of walls
	for (j=0;j<lev.nby;++j)
		for (i=0;i<lev.nbx;++i)
			if (i==0 || i==lev.nbx-1 || j==0 || j==lev.nby-1)
				if (!lev.tiles[j][i].iswall)
					errorexit("missing wall");
// turn pieces into dynamic
	ngoals=0;
	numpieces=0;
	curpiece=0;
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			tile& v=lev.tiles[j][i];
			if (v.ispiece) {
				if (numpieces>=MAXNUMPIECES)
					errorexit("too many pieces");
				pieces[numpieces].p.x=i;
				pieces[numpieces].p.y=j;
				pieces[numpieces].n=v.piecenum;
				pieces[numpieces].link[0]=-1;
				pieces[numpieces].link[1]=-1;
				++numpieces;
			}
			if (v.isgoal)
				++ngoals;
		}
	}
	if (ngoals>numpieces)
		errorexit("ngoals = %d > npieces = %d",ngoals,numpieces);
// build links
	S32 k;
	for (k=0;k<numpieces;++k) {
		piece& pc=pieces[k];
		const tile& v=lev.tiles[pc.p.y][pc.p.x];
		for (j=0;j<2;++j) {
			if (v.dirs[j].x!=0 || v.dirs[j].y!=0) {
				S32 pcn=findpieceatxy(pc.p.x+v.dirs[j].x,pc.p.y+v.dirs[j].y);
				if (pcn>=0) {
					if (pieces[pcn].n!=pc.n)
						errorexit("bad piece link");
					pc.link[j]=pcn;
				}
			}
		}
// p n link
	}
/*
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			tile& v=lev.tiles[j][i];
			if (v.ispiece) {
				if (numpieces>=MAXNUMPIECES)
					errorexit("too many pieces");
				pieces[numpieces].p.x=i;
				pieces[numpieces].p.y=j;
				pieces[numpieces].n=v.piecenum;
//				pieces[numpieces].link[0].x=v.dirs[0].x;
//				pieces[numpieces].link[0].y=v.dirs[0].y;
//				pieces[numpieces].link[1].x=v.dirs[1].x;
//				pieces[numpieces].link[1].y=v.dirs[1].y;
//				++numpieces;
			}
			if (v.isgoal)
				++ngoals;
		}
	}
*/
}

struct ms {
	pointi2 p;
	S32 b;
};

struct boardcollinfo {
	board objs;
	int nummoves;
	int prevhidx,previidx;
	ms mdb;
	boardcollinfo(const board& o,int nm,int phi,int pii,const ms& mdba) : 
		objs(o),nummoves(nm),prevhidx(phi),previidx(pii),mdb(mdba) {}
};

// a large collection of boards
class boardcoll {
public:
	int count;
	vector<boardcollinfo>* bci;
	bool addboard(const board& b,int movecount,int prvh,int prvi,const ms& msa); // returns if already in collection
	void showboards();
	boardcoll() : count(0)
	{
		bci = new vector<boardcollinfo>[HASHSIZE];
	}
	~boardcoll()
	{
		delete[] bci;
	}
};

bool boardcoll::addboard(const board& o,int nm,int prvh,int prvi,const ms& msa)
{
	int hash=o.hashfunc();
	int n=(int)bci[hash].size();
	int i;
	for (i=0;i<n;++i) {
		boardcollinfo& br=bci[hash][i];
		if (o.beq(br.objs)) {
			if (nm>=br.nummoves) 
				return true;
			else
				errorexit("bad addboard");
		}
	}
	boardcollinfo b(o,nm,prvh,prvi,msa);
	bci[hash].push_back(b);
	++count;
	if (count%10000==0) {
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-28,C32WHITE,C32BLACK,"h  %d / %d   ",prvh,HASHSIZE);
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
//		video_unlock();
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-28,F32WHITE,F32BLACK,"h  %d / %d   ",prvh,HASHSIZE);
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
		winproc();
		if (KEY=='a')
			return false;
		if (wininfo.closerequested)
			return false;
	}
	return true;
}

void boardcoll::showboards()
{
#if 0
	logger("boardcoll::showboards:\n");
	int i,tot=0;
	logger("\tvisited:\n");
	for (i=0;i<HASHSIZE;++i) {
		int nj=(int)bci[i].size();
		if (nj)
			logger("\t\thash%d,amount %d\n",i,nj);
#if 0
		int j;
		for (j=0;j<nj;++j)
			logger("\t\t\t h%d,i%d, playerpos  %d,%d, moves %d, movedir %d,%d, prevh %d, previ %d\n",
				i,j,bci[i][j].objs.playerpos.x,bci[i][j].objs.playerpos.y,bci[i][j].nummoves,
				bci[i][j].movedir.x,bci[i][j].movedir.y,
				bci[i][j].prevhidx,bci[i][j].previidx);
#endif
		tot+=nj;
	}
	logger("\n\t total %d, count %d\n\n",tot,count);
#endif
}

class game {
	board resetpos;
	board curpos;
	stack<board,vector<board> > undostack;
	stack<ms,vector<ms> > movesolvestackr;
	void solve();
//	S32 curpiece;
//	bool solve_rec();
public:
	game(const char* levelfile) : resetpos(levelfile),curpos(resetpos) {}
	void reset();
	void draw(const pointi2& pos) const;
	bool move(const pointi2& dir);
	bool iswinner();
	bool isloser();
	void undo();
	void hint();
	void clearsolvestack();
	int getundossize();
	void addundo(const board& b);
	int gethintssize();
	board getcurpos() const;
	void selectpiece();
	S32 getcurpiece() const {return curpos.curpiece;}
};

void game::draw(const pointi2& pos) const
{
	static S32 path;
	static C32 ctab[40]={C32(0,0,165),C32(165,0,0),C32(0,165,0),C32(165,165,0)};
	int i,j;
	int si,sj;
//	C32 C32DARKYELLOW(C32YELLOW);
//	C32DARKYELLOW.r-=60;
//	C32DARKYELLOW.g-=60;
//	C32DARKYELLOW.b-=60;
//	C32 C32LIGHTERBLUE(C32LIGHTBLUE);
//	C32LIGHTERBLUE.r+=80;
//	C32LIGHTERBLUE.g+=80;
//	C32LIGHTERBLUE.b;
// draw blank squares
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32WHITE);
		}
	}
// draw walls and goals
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			const tile& sq=curpos.lev.tiles[j][i];
			if (sq.iswall) 
				cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32DARKGRAY);
			if (sq.isgoal) {
//				bool drawninfo=false;
				S32 si2=i*SQSIZE+pos.x;
				S32 sj2=j*SQSIZE+pos.y;
				C32 c=ctab[sq.goalnum];
				cliprecto32(B32,si2+4,sj2+4,SQSIZE-2-7,SQSIZE-2-7,c);
				cliprecto32(B32,si2+5,sj2+5,SQSIZE-2-9,SQSIZE-2-9,c);
				cliprecto32(B32,si2+6,sj2+6,SQSIZE-2-11,SQSIZE-2-11,c);
			}
		}
	}
// draw piece links
	path=1-path;
	for (i=0;i<curpos.numpieces;++i) {
//		C32 c = i==curpos.curpiece ? C32LIGHTERBLUE : C32LIGHTBLUE;
		C32 c=ctab[curpos.pieces[i].n];
		S32 si2=curpos.pieces[i].p.x;
		S32 sj2=curpos.pieces[i].p.y;
		S32 si2m=si2*SQSIZE+pos.x;
		S32 sj2m=sj2*SQSIZE+pos.y;
		S32 lnk=curpos.pieces[i].link[path];
		if (lnk>=0) {
			S32 si2lnk=curpos.pieces[lnk].p.x;
			S32 sj2lnk=curpos.pieces[lnk].p.y;
			if (si2lnk>si2)
				cliprect32(B32,si2m+8,sj2m+12,SQSIZE+10,SQSIZE-2-15-8,c); // right
			if (sj2lnk>sj2)
				cliprect32(B32,si2m+12,sj2m+8,SQSIZE-2-15-8,SQSIZE+10,c); // down
			if (si2lnk<si2)
				cliprect32(B32,si2m+8-SQSIZE+10,sj2m+12,SQSIZE,SQSIZE-2-15-8,c); // left
			if (sj2lnk<sj2)
				cliprect32(B32,si2m+12,sj2m+8-SQSIZE+10,SQSIZE-2-15-8,SQSIZE,c); // up
//		outtextxybf32(B32,si2+6,sj2+6,C32WHITE,C32BLACK,"%d",curpos.pieces[i].n);
//		outtextxybf32(B32,si2+6+16,sj2+6,C32WHITE,C32BLACK,"%d",i);
//		if (curpos.pieces[i].link[0]>=0)
//			outtextxybf32(B32,si2+6,sj2+6+16,C32WHITE,C32BLACK,"%d",curpos.pieces[i].link[0]);
//		if (curpos.pieces[i].link[1]>=0)
//			outtextxybf32(B32,si2+6+8,sj2+6+16,C32WHITE,C32BLACK,"%d",curpos.pieces[i].link[1]);
		}
	}
// draw pieces and piece numbers
	for (i=0;i<curpos.numpieces;++i) {
//		C32 c = i==curpos.curpiece ? C32LIGHTERBLUE : C32LIGHTBLUE;
		C32 c=ctab[curpos.pieces[i].n];
		if (i==curpos.curpiece) {
			c.r+=90;
			c.g+=90;
			c.b+=90;
		}
		S32 si2=curpos.pieces[i].p.x*SQSIZE+pos.x;
		S32 sj2=curpos.pieces[i].p.y*SQSIZE+pos.y;
		cliprect32(B32,si2+8,sj2+8,SQSIZE-2-15,SQSIZE-2-15,c);
//		outtextxybf32(B32,si2+6,sj2+6,C32WHITE,C32BLACK,"%d",curpos.pieces[i].n);
//		outtextxybf32(B32,si2+6+16,sj2+6,C32WHITE,C32BLACK,"%d",i);
//		if (curpos.pieces[i].link[0]>=0)
//			outtextxybf32(B32,si2+6,sj2+6+16,C32WHITE,C32BLACK,"%d",curpos.pieces[i].link[0]);
//		if (curpos.pieces[i].link[1]>=0)
//			outtextxybf32(B32,si2+6+8,sj2+6+16,C32WHITE,C32BLACK,"%d",curpos.pieces[i].link[1]);
	}
/*
// draw goal numbers
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			const tile& sq=curpos.lev.tiles[j][i];
//			if (sq&WALL) 
//				cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32DARKGRAY);
			S32 si2=i*SQSIZE+pos.x;
			S32 sj2=j*SQSIZE+pos.y;
//			if (sq.isgoal) {
//				outtextxybf32(B32,si2+6,sj2+SQSIZE-8-4,C32WHITE,C32BLACK,"%d",sq.goalnum);
//			}
//			outtextxybf32(B32,si2+6+16,sj2+SQSIZE-8-4,C32WHITE,C32BLACK,"%d",sq.ndirs);
		}
	}
*/
}

bool game::move(const pointi2& dir)
{
	piece& cpm=curpos.pieces[curpos.curpiece]; // the current selected piece
// can only move the ends
	if (cpm.link[0]!=-1 && cpm.link[1]!=-1)
		return false; 
	int oldi=cpm.p.x;
	int oldj=cpm.p.y;
	int newi=oldi+dir.x;
	int newj=oldj+dir.y;
// see if there's a wall blocking the piece
	const tile& newpiecev=curpos.lev.tiles[newj][newi];
	if (newpiecev.iswall)
		return false; // can't move piece, because a wall is behind it
// see if there's a piece blocking the piece
	int sk2;
	for (sk2=0;sk2<curpos.numpieces;++sk2) {
		if (curpos.curpiece==sk2)
			continue;
		piece& cpo=curpos.pieces[sk2];
		if (newi==cpo.p.x && newj==cpo.p.y) 
			return false;
	}
// we're fine. move the piece..
	curpos.moveworm(curpos.curpiece,dir);
/*	S32 otherend=curpos.getotherend(curpos.curpiece);
	piece& cpm2=curpos.pieces[otherend];
	cpm2.p.x=newi;
	cpm2.p.y=newj; */
// gotta fixup 4 links
	
// sort the pieces after a move (sort by y then x), this reduces the number of board positions
#if 0
	S32 j=0;
	while(j<curpos.numpieces-1) {
		if (curpos.pieces[j].p.y>curpos.pieces[j+1].p.y || 
		  (curpos.pieces[j].p.y==curpos.pieces[j+1].p.y && curpos.pieces[j].p.x>curpos.pieces[j+1].p.x)) {
			if (curpos.curpiece==j)
				curpos.curpiece=j+1;
			else if (curpos.curpiece==j+1)
				curpos.curpiece=j;
			piece t=curpos.pieces[j];
			curpos.pieces[j]=curpos.pieces[j+1];
			curpos.pieces[j+1]=t;
			--j;
			if (j<0)
				j=0;
		} else
			++j;
	}
#endif
	return true;
}

void game::hint()
{
	if (iswinner())
		return;
	if (isloser())
		return;
	if (movesolvestackr.empty())
		solve();
	if (movesolvestackr.empty())
		return;
	ms thehint=movesolvestackr.top();
	movesolvestackr.pop();
//	S32 thehintbm=movebmsolvestackr.top();
//	movebmsolvestackr.pop();
	const board o=curpos;
	curpos.curpiece=thehint.b;
	if (move(thehint.p)) {
		addundo(o);
	}
}

void game::undo()
{
	if (!undostack.empty()) {
		curpos=undostack.top();
		undostack.pop();
		clearsolvestack();
	}
}

void game::addundo(const board& b)
{
	undostack.push(b);
}

void game::reset()
{
	curpos=resetpos;
	while(!undostack.empty())
		undostack.pop();
	clearsolvestack();
}

bool game::iswinner()
{
//	return curpos.playerpos.x==1 && curpos.playerpos.y==2;
//	return curpos.playerpos.x==6 && curpos.playerpos.y==4;
	S32 ngg=0;
	int i;
	for (i=0;i<curpos.numpieces;++i) {
		int ni=curpos.pieces[i].p.x;
		int nj=curpos.pieces[i].p.y;
		const tile& v=curpos.lev.tiles[nj][ni];
		if (v.isgoal)
			if (curpos.pieces[i].n==(v.goalnum))
				++ngg;
	}
	return ngg==curpos.ngoals;
}

bool game::isloser()
{
/*	int i;
	for (i=0;i<curpos.numpieces;++i) {
		int ni=curpos.pieces[i].x;
		int nj=curpos.pieces[i].y;
		if (!(curpos.lev.tiles[nj][ni]&GOAL))
			if ( (curpos.lev.tiles[nj+1][ni]&WALL) || (curpos.lev.tiles[nj-1][ni]&WALL))
				if ( (curpos.lev.tiles[nj][ni+1]&WALL) || (curpos.lev.tiles[nj][ni-1]&WALL))
					return true;
	} */
	return false;
}

void game::clearsolvestack()
{
	while(!movesolvestackr.empty())
		movesolvestackr.pop();
}

int game::gethintssize()
{
	return (int)movesolvestackr.size();
}

int game::getundossize()
{
	return (int)undostack.size();
}

board game::getcurpos() const
{
	return curpos;
}

void game::solve()
{
	static pointi2 dirs[4]={{-1,0},{0,-1},{1,0},{0,1},};
//	pointi2 dum={0,0};
	ms dumms={{0,0},0};
	board bsave=curpos;
//	S32 cbs=curpiece;
	boardcoll* bc=new boardcoll; // the solver data
	bc->addboard(curpos,0,-1,-1,dumms);
	int m=1;
	int oldcount=bc->count;
	logger("count for moves = 0 is %d\n",bc->count);
	while(1) {
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d count %8d",m+1,0);
//		video_unlock();
//		winproc();
		int i;
		for (i=0;i<HASHSIZE;++i) {
			const vector<boardcollinfo>& hr=bc->bci[i];
			int nj=(int)hr.size();
			int j;
			for (j=0;j<nj;++j) {
				if (hr[j].nummoves==m-1) {
					int b;
					for (b=0;b<curpos.numpieces;++b) {
						int d;
						for (d=0;d<4;++d) {
							curpos=hr[j].objs;
							curpos.curpiece=b; // select piece
							if (move(dirs[d])) {
								if (isloser())
									continue;
								if (iswinner()) {
									logger("found a winner on move %d,currently at h%d,i%d \n",m,i,j);
									bc->showboards();
//									stack<pointi2,vector<pointi2> > movesolvestack;
									logger("pushing %d,%d on move solve stack\n",dirs[d].x,dirs[d].y);
									ms pb;
									pb.p=dirs[d];
									pb.b=b;
									movesolvestackr.push(pb);
									int ph=hr[j].prevhidx;
									int pi=hr[j].previidx;
									ms p=hr[j].mdb;
									while(--m) {
										logger("pushing %d,%d bn %d\n",p.p.x,p.p.y,p.b);
										movesolvestackr.push(p);
										const boardcollinfo& hr2=bc->bci[ph][pi];
										ph=hr2.prevhidx;
										pi=hr2.previidx;
										p=hr2.mdb;
									}
									delete bc; // free up all those board positions
									curpos=bsave;
//									curpiece=cbs;
									return;
								} 
								ms nm;
								nm.p=dirs[d];
								nm.b=b;
								if (!bc->addboard(curpos,m,i,j,nm)) {
									delete bc;
									curpos=bsave;
//									curpiece=cbs;
									return;
								}
							}
						}
					}
				}
			}
		}
//		int nc=bc->count;
		logger("count for moves = %d is %d\n",m,bc->count);
		if (bc->count==oldcount) { // no new moves
			logger("no new moves found during move %d\n",m);
			break;
		}
		oldcount=bc->count;
		++m;
//		if (m==100)
//			break;
	}
	delete bc;
	curpos=bsave;
//	curpiece=cbs;
}

void game::selectpiece()
{
	S32 i;
	S32 mox=MX;
	S32 moy=MY;
	if (mox>=STARTX && moy>=STARTY) {
		mox-=STARTX;
		mox/=SQSIZE;
		moy-=STARTY;
		moy/=SQSIZE;
		for (i=0;i<curpos.numpieces;++i) {
			if (curpos.pieces[i].p.x==mox && curpos.pieces[i].p.y==moy) {
				curpos.curpiece=i;
				return;
			}
		}
	}
}

game* gp;
//int nsc;
//char** sc;
script* sc;
int curlevel;

} // end namespace wriggler

using namespace wriggler;

void wrigglerinit()
{
	S32 i;
	video_setupwindow(800,600);
	pushandsetdir("wriggler");
	sc=new scriptdir(0);
	sc->sort();
//	sc=doadir(&nsc,0);
//	sortscript(sc,nsc);
	for (i=0;i<sc->num();++i)
		logger("level %2d: \"%s\"\n",i,(*sc).printidx(i).c_str());
	curlevel=0;
//	curlevel=sc->num()-1;
	gp=new game((*sc).idx(curlevel).c_str());
//	gp=new game(level1);
//	gp=new game(levele);
	logger("sizeof boardcollinfo is %d\n",sizeof(boardcollinfo));
}

void wrigglerproc()
{
	pointi2 dir={0,0};
//	const pointi2 board2pos={STARTX,STARTY+BOARD2Y};
	switch (KEY) {
	case K_ESCAPE:
		popstate();
		break;
	case K_LEFT:
		--dir.x;
		break;
	case K_RIGHT:
		++dir.x;
		break;
	case K_UP:
		--dir.y;
		break;
	case K_DOWN:
		++dir.y;
		break;
	case 'r':
		gp->reset();
		break;
	case 'u':
		gp->undo();
		break;
	case 'h':
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"S O L V I N G");
		winproc();
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING");
//		video_unlock();
//		video_paintwindow(0);
		gp->hint();
		break;
	case '=':
		delete gp;
		++curlevel;
		if (curlevel>=sc->num())
			curlevel-=sc->num();
//		gp=new game(level1);
		gp=new game((*sc).idx(curlevel).c_str());
		break;
	case '-':
		delete gp;
		--curlevel;
		if (curlevel<0)
			curlevel+=sc->num();
//		gp=new game(levele);
		gp=new game((*sc).idx(curlevel).c_str());
		break;
	};
	if (wininfo.mleftclicks)
		gp->selectpiece();
	if (dir.x || dir.y) {
		const board o=gp->getcurpos();
		if (gp->move(dir)) {
			gp->clearsolvestack();
			gp->addundo(o);
		}
	}
}

void wrigglerdraw2d()
{
	const pointi2 board1pos={STARTX,STARTY};
//	video_lock();
	clipclear32(B32,C32BROWN);
//	cliprect32(B32,0,0,WX,WX,C32BROWN);
	gp->draw(board1pos);
//	gp->draw(board2pos,true);
	outtextxyf32(B32,WX/2-36,8,C32WHITE,"WRIGGLER");
	if (gp->iswinner())
		outtextxyf32(B32,750,560,mt_randU32(),"WINNER!");
	outtextxyf32(B32,30,580,C32WHITE,
		"'r' reset  'u' undo  'h' hint  'a' abort hint  '=' levelup  '-' leveldown  undos(%d), hints(%d)",
		gp->getundossize(),gp->gethintssize());
	outtextxyf32(B32,30,560,C32WHITE,"LEVEL : '%s'",(*sc).idx(curlevel).c_str());
//	video_unlock();
}

void wrigglerexit()
{
	delete gp;
//	freescript(sc,nsc);
	delete sc;
	popdir();
}
