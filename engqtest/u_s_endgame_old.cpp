#include <m_eng.h>

#include "u_states.h"

namespace endgame {

#define BOARDX 8
#define BOARDY 8

// tile types
//		dynamic
#define WKING 1
#define BKING 9

#define MAXNUMBPIECES 32

#define STARTX 30
#define STARTY 30
#define SQSIZE 35

// #define BACKTRACE // record all moves to a solution
//#define HASHSIZE 0x10000 // a power of 2
#define HASHSIZE (1<<21) // a power of 2


// prototype level data, loaded from file
#include <pshpack1.h>
/*struct pointS82 {
	S8 x: 4;
	S8 y: 4;
};
*/
struct pointU82 {
	U8 x : 4;
	U8 y : 4;
};

struct piece {
	U8 color : 1; // 0 white, 1 black
	U8 rank : 3; // 1 king, 2 queen, 3 rook, 4 bishop, 5 knight, 6 pawn
};

struct bpiece {
	piece p;
	pointU82 pos;
};

struct level {
	U8 nbx,nby;
	piece tiles[BOARDY][BOARDX];
};

// a board position
struct objects {
	static level lev;
	static int numbpieces;
	bpiece bpieces[MAXNUMBPIECES];
	bool turn;

	int hashfunc() const;
	bool beq(const objects& b2) const;
	objects(const char* s);
	static bool mouse2board(S32 mx,S32 my,pointU82& bpos);
};

level objects::lev;
int objects::numbpieces; // includes captured

bool insolve;
S32 g_prvh;
S32 g_nm;
S32 g_acount;

int objects::hashfunc() const // mask is 2^n-1
{
	S32 i;
	S32 ret=0;
	for (i=0;i<numbpieces;++i) {
		ret<<=2;
		ret+=bpieces[i].pos.x+(bpieces[i].pos.y<<1);
	}
	return ret & (HASHSIZE-1);
}

bool objects::beq(const objects& b2) const
{
	if (turn != b2.turn)
		return false;
	int i;
	for (i=0;i<numbpieces;++i)
		if (bpieces[i].pos.x!=b2.bpieces[i].pos.x || bpieces[i].pos.y!=b2.bpieces[i].pos.y || bpieces[i].p.color != b2.bpieces[i].p.color || bpieces[i].p.rank != b2.bpieces[i].p.rank)
			return false;
	return true;
}

objects::objects(const char* levelfile)
{
	script lsc(levelfile);
	if (lsc.num()!=64)
		errorexit("bad level");
	int i,j;
	for (j=0;j<BOARDY;++j) {
		for (i=0;i<BOARDX;++i) {
			U8 v = atoi(lsc.idx(/*2+*/j*BOARDX+i).c_str());
			lev.tiles[j][i].color = v/8;
			lev.tiles[j][i].rank = v%8;
		}
	}
	numbpieces=0;
	for (j=0;j<BOARDY;++j) {
		for (i=0;i<BOARDX;++i) {
			piece& v=lev.tiles[j][i];
			if (v.rank) {
				if (numbpieces>=MAXNUMBPIECES)
					errorexit("too many bpieces");
				bpieces[numbpieces].pos.x = i;
				bpieces[numbpieces].pos.y = j;
				bpieces[numbpieces].p = v;
				++numbpieces;
			}
		}
	}
	turn = false;
}

bool objects::mouse2board(S32 mx,S32 my,pointU82& bpos)
{
#define STARTX 30
#define STARTY 30
#define SQSIZE 35
	if (mx<STARTX)
		return false;
	if (my<STARTY)
		return false;
	S32 bx = (mx-STARTX)/SQSIZE;
	if (bx >= BOARDX)
		return false;
	S32 by = (my-STARTY)/SQSIZE;
	if (by >= BOARDY)
		return false;
	S32 modx = (mx-STARTX)%SQSIZE;
	if (modx < SQSIZE/8 || modx > 7*SQSIZE/8)
		return false;
	S32 mody = (my-STARTY)%SQSIZE;
	if (mody < SQSIZE/8 || mody > 7*SQSIZE/8)
		return false;
	bpos.x = bx;
	bpos.y = by;
	return true;
}

struct boardcollinfo {
	objects objs;
	U16 nummoves;
	boardcollinfo(const objects& o,int nm) :
		objs(o),nummoves(nm) {}
};

// a large collection of boards
class boardcoll {
public:
	int acount;
//	vector<boardcollinfo> bci[HASHSIZE] ;
	vector<boardcollinfo> *bci;
	boardcoll() : acount(0)
	{
		bci = new vector<boardcollinfo>[HASHSIZE];
	}
	~boardcoll()
	{
		delete[] bci;
	}
	bool addboard(const objects& b,int movecount,int prvh,int prvi); // returns if already in collection
//	boardcoll() : count(0) {}
	void showboards();
};

bool boardcoll::addboard(const objects& o,int nm,int prvh,int prvi)
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
	boardcollinfo b(o,nm);
	bci[hash].push_back(b);
	++acount;
	if (acount%100000==1) {
		g_prvh=prvh;
		g_nm=nm;
		g_acount=acount;
		winproc();
		if (KEY=='a')
			return false;
		if (wininfo.closerequested)
			return false;
//		video_paintwindow(0);
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
			logger("\t\thash%d,amount %d   \n",i,nj);
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

struct amove {
	pointU82 from,to;
	amove(S32 c);
	amove(){}
};

amove::amove(S32 code)
{
	from.x = code % BOARDX;
	code /= BOARDX;

	from.y = code % BOARDY;
	code /= BOARDY;

	to.x = code % BOARDX;
	code /= BOARDX;

	to.y = code;
}

class game {
	objects resetpos;
	objects curpos;
	stack<objects,vector<objects> > undostack;
	stack<amove,vector<amove> > movesolvestackr;
	void backtrace(S32 m);
	void solve();
//	bool solve_rec();
	boardcoll* bc; // the solver data
public:
	game(const char* levelfile) : resetpos(levelfile),curpos(resetpos) {}
	void reset();
	void draw(const pointi2& pos) const;
//	bool move(const pointS82& dir);
	bool move(const amove& am);
	bool move1(const amove& am);
	bool move2(const amove& am);
	bool move3(const amove& am);
//	bool iswinner();
	bool isloser();
	void undo();
	void hint();
	void clearsolvestack();
	int getundossize();
	void addundo(const objects& b);
	int gethintssize();
	objects getcurpos() const;
};

void game::draw(const pointi2& pos) const
{
	int i,j;
	int si,sj;
	for (j=0;j<BOARDY;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<BOARDX;++i) {
			si=i*SQSIZE+pos.x;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32YELLOW);
		}
	}
	for (i=0;i<curpos.numbpieces;++i) {
		si=curpos.bpieces[i].pos.x*SQSIZE+pos.x;
		sj=curpos.bpieces[i].pos.y*SQSIZE+pos.y;
		cliprect32(B32,si+8,sj+8,SQSIZE-2-15,SQSIZE-2-15,C32(128,128,128));
		U8 side = curpos.bpieces[i].p.color;
		C32 fcol,bcol;
		if (side == 0) {
			fcol = C32WHITE;
			bcol = C32BLACK;
		} else {
			fcol = C32BLACK;
			bcol = C32WHITE;
		}
		//outtextxybf32(B32,si+(SQSIZE>>1)-4,sj+(SQSIZE>>1)-4,fcol,bcol,"K");
		outtextxyf32(B32,si+(SQSIZE>>1)-4,sj+(SQSIZE>>1)-4,fcol,"K");
	}
	outtextxy32(B32,120,330,C32WHITE,curpos.turn==0 ? "White to play." : "Black to play.");
}

// simple nim
bool game::move1(const amove& am)
{
	U32 pi=curpos.bpieces[0].pos.x;
	U32 pj=curpos.bpieces[0].pos.y;
	if (pi != am.from.x || pj != am.from.y)
		return false;
	//if (curpos.turn != curpos.bpieces[0].p.color)
	//	return false; // must move correct color piece depending on who's turn it is
	// out of bounds
	if (am.to.x<0 || am.to.x>=BOARDX || am.to.y<1 || am.to.y>=BOARDY)
		return false;
	S32 dx = am.from.x - am.to.x;
	S32 dy = am.from.y - am.to.y;
	if (dx != 0)
		return false;
	if (dy < 1 || dy > 3) // only 1 2 or 3
		return false;
	// move the piece
	curpos.bpieces[0].pos = am.to;
	curpos.turn = !curpos.turn; // turn done..
	return true;
}

// opposition
bool game::move2(const amove& am)
{
	int jf;
	for (jf=0;jf<curpos.numbpieces;++jf) { // find a piece to move
		U32 pi=curpos.bpieces[jf].pos.x;
		U32 pj=curpos.bpieces[jf].pos.y;
		if (pi == am.from.x && pj == am.from.y) {
			break;
		}
	}
	if (jf == curpos.numbpieces)
		return false; // must have a piece to move
	if (curpos.turn != curpos.bpieces[jf].p.color)
		return false; // must move correct color piece depending on who's turn it is
	// out of bounds
	if (am.to.x<0 || am.to.x>=BOARDX || am.to.y<0 || am.to.y>=BOARDY)
		return false;
	S32 adx = abs(am.from.x - am.to.x);
	S32 ady = abs(am.from.y - am.to.y);
	if (adx == 0 && ady == 0) // no move is not a move
		return false;
	// for now a piece cannot move into another piece, or be close to it
	S32 i,j,k;
	for (k=0;k<curpos.numbpieces;++k) {
		if (k==jf)
			continue;
		U32 pi=curpos.bpieces[k].pos.x;
		U32 pj=curpos.bpieces[k].pos.y;
		for (j=-1;j<=1;++j) {
			for (i=-1;i<=1;++i) {
				if (pi == am.to.x + i && pj == am.to.y + j) {
					return false;
				}
			}
		}
	}

	// king
	if (adx>1) // too far
		return false;
	if (ady>1) // too far
		return false;

	// move the piece
	curpos.bpieces[jf].pos = am.to;
	// bubble sort the pieces by board position
	k=0;
	while(k<curpos.numbpieces-1) {
		if (curpos.bpieces[k].pos.y>curpos.bpieces[k+1].pos.y || (curpos.bpieces[k].pos.y==curpos.bpieces[k+1].pos.y && curpos.bpieces[k].pos.x>curpos.bpieces[k+1].pos.x)) {
			bpiece t=curpos.bpieces[k];
			curpos.bpieces[k]=curpos.bpieces[k+1];
			curpos.bpieces[k+1]=t;
			--k;
			if (k<0)
				k=0;
		} else
			++k;
	}
	curpos.turn = !curpos.turn; // turn done..
	return true;
}

// king and pawn vs king
bool game::move3(const amove& am)
{
	return false;
}

bool game::move(const amove& am) // /*S32 pidx,*/const pointS82& dir)
{
	switch(curpos.numbpieces) {
	case 1:
		return move1(am);
	case 2:
		return move2(am);
	case 3:
		return move3(am);
	default:
		return false;
	}
}

void game::hint()
{
//	if (iswinner())
//		return;
	if (isloser())
		return;
	if (movesolvestackr.empty())
		solve();
	if (movesolvestackr.empty())
		return;
	amove thehint=movesolvestackr.top();
	movesolvestackr.pop();
	const objects o=curpos;
	if (move(thehint)) {
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

void game::addundo(const objects& b)
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

bool game::isloser()
{
	S32 dx,dy;
	switch(curpos.numbpieces) {
	case 1: // simple nim
		return curpos.bpieces[0].pos.y == 1;
	case 2: // opposition
		dx = abs(curpos.bpieces[0].pos.x - curpos.bpieces[1].pos.x);
		dy = abs(curpos.bpieces[0].pos.y - curpos.bpieces[1].pos.y);
		if (dx == 0 && dy == 2)
			return true;
		if (dx == 2 && dy == 0)
			return true;
		return false;
	case 3: // king and pawn vs king
	default:
		return false;
	}
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

objects game::getcurpos() const
{
	return curpos;
}

void game::backtrace(S32 m) {
	objects curpost=curpos;
	while(1) {
		--m;
		if (m<0)
			break;
		S32 i;
		for (i=0;i<HASHSIZE;++i) { // scan hash table for one less move than current
			const vector<boardcollinfo>& hr=bc->bci[i];
			int nj=(int)hr.size();
			int j;
			for (j=0;j<nj;++j) {
				if (hr[j].nummoves==m) {
					int d; // found one
					for (d=0;d<BOARDX*BOARDX*BOARDY*BOARDY;++d) { // generate move
						curpos=hr[j].objs;
						amove am(d);
						if (move(am)) { // legal?
//													if (move(dirs[d])) { // legal?
							if (curpos.beq(curpost)) { // same?
								movesolvestackr.push(am); // save move
//															movesolvestackr.push(dirs[d]); // save move
								curpost=hr[j].objs; // earlier position
								break;
							}
						}
					}
					if (d!=BOARDX*BOARDX*BOARDY*BOARDY)
						break; // we found a move
				}
			}
			if (j!=nj)
				break; // we found a move
		}
		if (i==HASHSIZE) // we didn't find a move, shouldn't happen
			errorexit("couldn't backtrace");
	}
}

void game::solve()
{
	insolve=true;
	objects bsave=curpos;
	bc=new boardcoll;
	bc->addboard(curpos,0,-1,-1);
	int m=1;
	int oldcount=bc->acount;
	logger("count for moves = 0 is %d\n",bc->acount);
	while(1) {
		int i;
		for (i=0;i<HASHSIZE;++i) { // scan hash table for one less move than current
			const vector<boardcollinfo>& hr=bc->bci[i];
			int nj=(int)hr.size();
			int j;
			for (j=0;j<nj;++j) {
				if (hr[j].nummoves==m-1) {
					int d; // found one
					for (d=0;d<BOARDX*BOARDX*BOARDY*BOARDY;++d) { // generate moves
						curpos=hr[j].objs;
						amove am(d);
						if (move(am)) { // legal?
//						if (move(dirs[d])) {
#if 0 // 1 player
							if (isloser())
								continue; // don't consider losers
							if (iswinner()) { // see if done
#else // 2 players
							if (isloser()) { // a winning move to give other player a losing position..
#endif
								logger("found a winner on move %d,currently at h%d,i%d \n",m,i,j);
								bc->showboards();
//								logger("pushing %d,%d on move solve stack\n",dirs[d].x,dirs[d].y);
								logger("pushing (%d,%d) (%d,%d) on move solve stack\n",am.from.x,am.from.y,am.to.x,am.to.y);
								movesolvestackr.push(am); // start building solve stack
//								movesolvestackr.push(dirs[d]);
								backtrace(m);
								delete bc; // free up all those board positions
								curpos=bsave; // we have a winner
								insolve=false;
								return;
							}
							if (!bc->addboard(curpos,m,i,j)) { // check for abort
								delete bc; // free up all those board positions
								curpos=bsave; // early out
								insolve=false;
								return;
							}
						}
					}
				}
			}
		}
//		int nc=bc->count;
		logger("count for moves = %d is %d\n",m,bc->acount);
		if (bc->acount==oldcount) { // no new moves
			logger("no new moves found during move %d\n",m);
			break;
		}
		oldcount=bc->acount;
		++m;
//		if (m==100)
//			break;
	}
	delete bc;
	curpos=bsave;
	insolve=false;
}


game* gp;
//int nsc;
//char** sc;
script* sc;
int curlevel;

bool inuimove;
pointi2 uip0,uip1;


} // end namespace endgame

using namespace endgame;

void endgameinit()
{
	insolve=false;
	S32 i;
	video_setupwindow(800,600);
	pushandsetdir("endgame");
	logger("sizeof objects %d\n",sizeof(objects));
	logger("sizeof boardcollinfo %d\n",sizeof(boardcollinfo));
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
	inuimove = false;
}

void endgameproc()
{
//	pointS82 dir={0,0};
//	const pointi2 board2pos={STARTX,STARTY+BOARD2Y};
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
/*
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

	case K_HOME:
		--dir.x;
		--dir.y;
		break;
	case K_PAGEUP:
		++dir.x;
		--dir.y;
		break;
	case K_END:
		--dir.x;
		++dir.y;
		break;
	case K_PAGEDOWN:
		++dir.x;
		++dir.y;
		break;
*/
	case 'r':
		gp->reset();
		break;
	case 'u':
		gp->undo();
		break;
	case 'h':
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
	// move
	if (MBUT && !LMBUT) { // up to down
		pointU82 f;
		if (objects::mouse2board(MX,MY,f)) {
			inuimove = true;
			uip0.x = MX;
			uip0.y = MY;
		}
	}
	if (MBUT) { // down
		if (inuimove) {
			uip1.x = MX;
			uip1.y = MY;
		}
	}
	if (!MBUT && LMBUT) { // down to up, make a move
		if (inuimove) {
			const objects o=gp->getcurpos();
			amove am; // TODO: set am
			if (objects::mouse2board(uip0.x,uip0.y,am.from)) {
				if (objects::mouse2board(MX,MY,am.to)) {
//					if (gp->move(dir)) {
					if (gp->move(am)) {
						gp->clearsolvestack();
						gp->addundo(o);
					}
				}
			}
			inuimove = false;
		}
	}
}

void endgamedraw2d()
{
	const pointi2 board1pos={STARTX,STARTY};
//	video_lock();
	clipclear32(B32,C32BROWN);
//	cliprect32(B32,0,0,WX,WX,C32BROWN);
	gp->draw(board1pos);
//	gp->draw(board2pos,true);
	if (gp->isloser())
		outtextxyf32(B32,550,560,mt_randU32(),"You Lost!");
	outtextxyf32(B32,30,580,C32WHITE,
		"'r' reset  'u' undo  'h' hint  'a' abort hint  '=' levelup  '-' leveldown  undos(%d), hints(%d)",
		gp->getundossize(),gp->gethintssize());
	outtextxyf32(B32,30,560,C32WHITE,"LEVEL : '%s'",(*sc).idx(curlevel).c_str());
//	video_unlock();
	if (insolve) {
		outtextxybf32(B32,(WX>>1)+100,580-28,C32WHITE,C32BLACK,"h  %d / %d   ",g_prvh,HASHSIZE);
		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",g_nm+1,g_acount);
	}
	if (inuimove) {
		clipline32(B32,uip0.x,uip0.y,uip1.x,uip1.y,C32LIGHTMAGENTA);
	}
}

void endgameexit()
{
	delete gp;
//	freescript(sc,nsc);
	delete sc;
	popdir();
}
#include <poppack.h>
