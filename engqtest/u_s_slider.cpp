#include <m_eng.h>
#include "d2_font.h"

namespace slider {

#define MAXBOARDX 32
#define MAXBOARDY 32

// file level tile types
// bits 0-3 goal number
// bit 5 isgoal
// bit 6 iswall
// bit 7 ispiece
// bit 8-11 piece number
// if bits 5 - 7 are all 0, then is a blank

// board level tile types (after pieces have been extracted)
// bits 0-3 goal number
// bit 5 isgoal
// bit 6 iswall
// if bits 5 - 6 are all 0, then is a blank
#define MAXNUMPIECES 16 // a power of 2

#define BLANK 0
// #define GOALSHIFT 0
#define GOALMASK (MAXNUMPIECES-1)
#define GOAL 0x20
#define WALL 0x40
#define PIECE 0x80
#define PIECESHIFT 8
#define PIECEMASK (MAXNUMPIECES-1)*(1<<PIECESHIFT)

// graphics
#define STARTX 30
#define STARTY 30
#define SQSIZE 35


//#define HASHSIZE 0x10000 // a power of 2
#define HASHSIZE 0x200000 // a power of 2
//#define HASHSIZE 32768 // a power of 2

// prototype level data, loaded from file
struct piecedesc {
	int sx,sy;
	U8* data;
};

struct level {
	int nbx,nby;
	int tiles[MAXBOARDY][MAXBOARDX];
	piecedesc* piecetypes;
	int npiecetypes;
};

struct pointS82 {
	S8 x;
	S8 y;
};

struct piece {
	pointS82 p;
	int n;
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
	int parse_tile(const char* t);
	static void cleanup();
};

level board::lev;
int board::numpieces;
int board::ngoals;

void board::cleanup()
{
	if (!lev.npiecetypes)
		return;
	int k;
	for (k=0;k<lev.npiecetypes;++k) {
		delete[] lev.piecetypes[k].data;
	}
	delete[] lev.piecetypes;
	lev.piecetypes=0;
	lev.npiecetypes=0;
}

int board::hashfunc() const // mask is 2^n-1
{
	S32 i;
	S32 ret=0;
	for (i=0;i<numpieces;++i) {
		ret<<=4;
		ret+=pieces[i].p.x+(pieces[i].p.y<<2);
		ret+=pieces[i].n<<3;
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

int board::parse_tile(const char* t)
{
	int ret=0;
	const char* ts=t;
	while(*t) {
		if (*t=='.') {
			++t;
		} else if (*t=='W') {
			ret+=WALL;
			++t;
		} else if (*t=='G') {
			ret+=GOAL;
			++t;
			ret+=advance_atoi(&t);
			
		} else if (*t=='P') {
			++t;
			ret+=PIECE;
			ret+=(advance_atoi(&t)<<PIECESHIFT);
		} else
			errorexit("bad parsetile: '%s'",ts);
	}
	return ret;
}

board::board(const char* levelfile)
{
	cleanup();
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
	int i,j,k;
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			const C8* tp=lsc.idx(sp+j*lev.nbx+i).c_str();
			int& v=lev.tiles[j][i];
			v=parse_tile(tp);
		}
	}
	sp+=lev.nbx*lev.nby;
// make sure has a border of walls
	for (j=0;j<lev.nby;++j)
		for (i=0;i<lev.nbx;++i)
			if (i==0 || i==lev.nbx-1 || j==0 || j==lev.nby-1)
				if (lev.tiles[j][i]!=WALL)
					errorexit("missing wall");
// turn pieces into dynamic
	ngoals=0;
	numpieces=0;
	curpiece=0;
	S32 maxn=0;
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			int& v=lev.tiles[j][i];
			if (v&PIECE) {
				if (numpieces>=MAXNUMPIECES)
					errorexit("too many pieces");
				pieces[numpieces].p.x=i;
				pieces[numpieces].p.y=j;
				pieces[numpieces].n=(v&(PIECEMASK))>>PIECESHIFT;
				if (pieces[numpieces].n>maxn)
					maxn=pieces[numpieces].n;
				++numpieces;
				v&=~(PIECE|PIECEMASK);
			}
			if (v&GOAL)
				++ngoals;
		}
	}
	if (ngoals>numpieces)
		errorexit("ngoals = %d > npieces = %d",ngoals,numpieces);
// load up piecetypes
	if (lsc.num() < sp+1)
		errorexit("can't get num piecetypes");
	lev.npiecetypes=atoi(lsc.idx(sp++).c_str());
	if (lev.npiecetypes<=maxn)
		errorexit("not enough piece types");
	lev.piecetypes = new piecedesc[lev.npiecetypes];
	for (k=0;k<lev.npiecetypes;++k) {
		if (lsc.num() < sp+2)
			errorexit("can't load piece type %d",k);
		lev.piecetypes[k].sx=atoi(lsc.idx(sp++).c_str());
		lev.piecetypes[k].sy=atoi(lsc.idx(sp++).c_str());
		if (lsc.num() < sp+lev.piecetypes[k].sx*lev.piecetypes[k].sy)
			errorexit("can't load piece type %d element",k);
		lev.piecetypes[k].data=new U8[lev.piecetypes[k].sx*lev.piecetypes[k].sy];
		for (j=0;j<lev.piecetypes[k].sx*lev.piecetypes[k].sy;++j)
			lev.piecetypes[k].data[j]=atoi(lsc.idx(sp++).c_str());
	}
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
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
//		video_unlock();
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
	int i,j;
	int si,sj;
	C32 C32DARKYELLOW(C32YELLOW);
	C32DARKYELLOW.r-=60;
	C32DARKYELLOW.g-=60;
	C32DARKYELLOW.b-=60;
	C32 C32LIGHTERBLUE(C32LIGHTBLUE);
	C32LIGHTERBLUE.r+=80;
	C32LIGHTERBLUE.g+=80;
//	C32LIGHTERBLUE.b;
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32YELLOW);
		}
	}
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			int sq=curpos.lev.tiles[j][i];
			if (sq&WALL) 
				cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32DARKGRAY);
			if (sq&GOAL) {
				piecedesc& pd=curpos.lev.piecetypes[sq&GOALMASK];
				S32 x,y;
//				bool drawninfo=false;
				for (y=0;y<pd.sy;++y) {
					for (x=0;x<pd.sx;++x) {
						S32 si2=(i+x)*SQSIZE+pos.x;
						S32 sj2=(j+y)*SQSIZE+pos.y;
						if (pd.data[x+y*pd.sx]) {
							cliprect32(B32,si2+4,sj2+4,SQSIZE-2-7,SQSIZE-2-7,C32DARKYELLOW);
							if (x<pd.sx-1 && (pd.data[x+1+y*pd.sx]))
								cliprect32(B32,si2+4,sj2+8,SQSIZE-2-7+(SQSIZE>>1),SQSIZE-2-7-8,C32DARKYELLOW);
							if (y<pd.sy-1 && (pd.data[x+(y+1)*pd.sx]))
								cliprect32(B32,si2+8,sj2+4,SQSIZE-2-7-8,SQSIZE-2-7+(SQSIZE>>1),C32DARKYELLOW);
//							if (!drawninfo) {
//								outtextxybf32(B32,si2+6,sj2+SQSIZE-8-4,C32WHITE,C32BLACK,"%d",sq&GOALMASK);
//								drawninfo=true;
//							}
						}
					}
				}  
//				cliprect32(B32,si+4,sj+4,SQSIZE-2-7,SQSIZE-2-7,C32DARKYELLOW);
//				cliprect32(B32,si+6,sj+6,SQSIZE-2-11,SQSIZE-2-11,C32YELLOW);
			}
		}
	}
	for (i=0;i<curpos.numpieces;++i) {
		S32 x,y;
		piecedesc& pd=curpos.lev.piecetypes[curpos.pieces[i].n];
		C32 c = i==curpos.curpiece ? C32LIGHTERBLUE : C32LIGHTBLUE;
		bool drawninfo=false;
		for (y=0;y<pd.sy;++y) {
			for (x=0;x<pd.sx;++x) {
				S32 si2=(curpos.pieces[i].p.x+x)*SQSIZE+pos.x;
				S32 sj2=(curpos.pieces[i].p.y+y)*SQSIZE+pos.y;
				if (pd.data[x+y*pd.sx]) {
					cliprect32(B32,si2+8,sj2+8,SQSIZE-2-15,SQSIZE-2-15,c);
					if (x<pd.sx-1 && (pd.data[x+1+y*pd.sx]))
						cliprect32(B32,si2+8,sj2+12,SQSIZE-2-15+(SQSIZE>>1),SQSIZE-2-15-8,c);
					if (y<pd.sy-1 && (pd.data[x+(y+1)*pd.sx]))
						cliprect32(B32,si2+12,sj2+8,SQSIZE-2-15-8,SQSIZE-2-15+(SQSIZE>>1),c);
					if (!drawninfo) {
						outtextxybf32(B32,si2+6,sj2+6,C32WHITE,C32BLACK,"%d",curpos.pieces[i].n);
//						outtextxybf32(B32,si2+6,sj2+6,C32WHITE,C32BLACK,"%d:%d",curpos.pieces[i].n,i);
						drawninfo=true;
					}
				}
			}
		}  
//		if (i==curpos.curpiece) {
//			cliprect32(B32,si+5,sj+5,SQSIZE-2-15+6,SQSIZE-2-15+6,C32WHITE);
//			cliprecto32(B32,si+6,sj+6,SQSIZE-2-15+4,SQSIZE-2-15+4,C32BLACK);
//			clipcircle32(B32,si+SQSIZE/2,sj+SQSIZE/2,5,C32WHITE);
//		}
	}
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			int sq=curpos.lev.tiles[j][i];
//			if (sq&WALL) 
//				cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32DARKGRAY);
			if (sq&GOAL) {
				piecedesc& pd=curpos.lev.piecetypes[sq&GOALMASK];
				S32 x,y;
				bool drawninfo=false;
				for (y=0;y<pd.sy;++y) {
					for (x=0;x<pd.sx;++x) {
						S32 si2=(i+x)*SQSIZE+pos.x;
						S32 sj2=(j+y)*SQSIZE+pos.y;
						if (pd.data[x+y*pd.sx]) {
							if (!drawninfo) {
								outtextxybf32(B32,si2+6,sj2+SQSIZE-8-4,C32WHITE,C32BLACK,"%d",sq&GOALMASK);
								drawninfo=true;
							}
						}
					}
				}  
//				cliprect32(B32,si+4,sj+4,SQSIZE-2-7,SQSIZE-2-7,C32DARKYELLOW);
//				cliprect32(B32,si+6,sj+6,SQSIZE-2-11,SQSIZE-2-11,C32YELLOW);
			}
		}
	}
//	si=curpos.playerpos.x*SQSIZE+pos.x;
//	sj=curpos.playerpos.y*SQSIZE+pos.y;
//	clipcircle32(B32,si+(SQSIZE>>1),sj+(SQSIZE>>1),SQSIZE>>2,C32RED);
}

bool game::move(const pointi2& dir)
{
	int j;
	piece& cpm=curpos.pieces[curpos.curpiece];
	int oldi=cpm.p.x;
	int oldj=cpm.p.y;
	int newi=oldi+dir.x;
	int newj=oldj+dir.y;
// see if there's a wall blocking the piece
	S32 x,y;
	piecedesc& pdm=curpos.lev.piecetypes[cpm.n];
	for (y=0;y<pdm.sy;++y) {
		for (x=0;x<pdm.sx;++x) {
			if (pdm.data[x+y*pdm.sx]) {
				int newpiecev=curpos.lev.tiles[newj+y][newi+x];
				if (newpiecev&WALL)
					return false; // can't move piece, because a wall is behind it
			}
		}
	}  
// see if there's a piece blocking the piece
	int sk2;
	for (sk2=0;sk2<curpos.numpieces;++sk2) {
		if (curpos.curpiece==sk2)
			continue;
		piece& cpo=curpos.pieces[sk2];
		piecedesc& pdo=curpos.lev.piecetypes[cpo.n];


		for (y=0;y<pdm.sy;++y) {
			for (x=0;x<pdm.sx;++x) {
				if (pdm.data[x+y*pdm.sx]) {
					S32 xp=x+(newi-cpo.p.x);
					S32 yp=y+(newj-cpo.p.y);
					if (xp>=0 && yp>=0 && xp<pdo.sx && yp<pdo.sy) {
						if (pdo.data[xp+yp*pdo.sx]) {
							return false;
						}
					}
/*					int newpiecev=curpos.lev.tiles[newj+y][newi+x];
					if (newpiecev&WALL)
						return false; // can't move piece, because a wall is behind it */
				}
			}
		}  
//		if (curpos.pieces[sk2].p.x==newi && curpos.pieces[sk2].p.y==newj)
//			return false; // can't move piece, because another piece is behind it
	}
// we're fine. move the piece..
	cpm.p.x=newi;
	cpm.p.y=newj;
// sort the pieces after a move (sort by y then x), this reduces the number of board positions
	j=0;
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
		int v=curpos.lev.tiles[nj][ni];
		if (v&GOAL)
			if (curpos.pieces[i].n==(v&GOALMASK))
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
	S32 i,x,y;
	S32 mox=MX;
	S32 moy=MY;
	if (mox>=STARTX && moy>=STARTY) {
		mox-=STARTX;
		mox/=SQSIZE;
		moy-=STARTY;
		moy/=SQSIZE;
		for (i=0;i<curpos.numpieces;++i) {
			piecedesc& pd=curpos.lev.piecetypes[curpos.pieces[i].n];
			for (y=0;y<pd.sy;++y) {
				for (x=0;x<pd.sx;++x) {
					if (pd.data[x+y*pd.sx]) {
						if ( curpos.pieces[i].p.x+x==mox && curpos.pieces[i].p.y+y==moy) {
							curpos.curpiece=i;
							return;
						}
					}
				}
			}  
			
		}
	}
}

game* gp;
//int nsc;
//char** sc;
script* sc;
int curlevel;

} // end namespace slider

using namespace slider;

void sliderinit()
{
	S32 i;
	video_setupwindow(800,600);
	pushandsetdir("slider");
	sc=new scriptdir(0);
	sc->sort();
//	sc=doadir(&nsc,0);
//	sortscript(sc,nsc);
	for (i=0;i<sc->num();++i)
		logger("level %2d: \"%s\"\n",i,(*sc).printidx(i).c_str());
	curlevel=0;
	curlevel=sc->num()-1;
	gp=new game((*sc).idx(curlevel).c_str());
//	gp=new game(level1);
//	gp=new game(levele);
}

void sliderproc()
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
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING");
//		video_unlock();
//		video_paintwindow(0);
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"S O L V I N G");
//		video_unlock();
		winproc();
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

void sliderdraw2d()
{
	const pointi2 board1pos={STARTX,STARTY};
//	video_lock();
	clipclear32(B32,C32BROWN);
//	cliprect32(B32,0,0,WX,WX,C32BROWN);
	gp->draw(board1pos);
//	gp->draw(board2pos,true);
	if (gp->iswinner())
		outtextxyf32(B32,750,560,mt_randU32(),"WINNER!");
	outtextxyf32(B32,30,580,C32WHITE,
		"'r' reset  'u' undo  'h' hint  'a' abort hint  '=' levelup  '-' leveldown  undos(%d), hints(%d)",
		gp->getundossize(),gp->gethintssize());
	outtextxyf32(B32,30,560,C32WHITE,"LEVEL : '%s'",(*sc).idx(curlevel).c_str());
//	video_unlock();
}

void sliderexit()
{
	delete gp;
	board::cleanup();
//	freescript(sc,nsc);
	delete sc;
	popdir();
}
