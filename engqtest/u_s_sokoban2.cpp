#include <m_eng.h>
#include "d2_font.h"

namespace sokoban2 {

#define MAXBOARDX 16
#define MAXBOARDY 16

// tile types
//		dynamic
#define PLAYER 1
#define BLOCK 2

//		static
#define GOAL 4
#define WALL 8
#define BADL 9
#define BAD 16
#define TEMPBLOCK 32
#define TELEHACK 64

#define MAXNUMBLOCKS 12

#define STARTX 30
#define STARTY 30
#define SQSIZE 35


//#define HASHSIZE 0x10000 // a power of 2
#define HASHSIZE 0x200000 // a power of 2


// prototype level data, loaded from file
#include <pshpack1.h>
struct level {
	U8 nbx,nby;
	U8 tiles[MAXBOARDY][MAXBOARDX];
};

struct pointS82 {
	S8 x: 4;
	S8 y: 4;
};

struct pointU82 {
	U8 x : 4;
	U8 y : 4;
};

struct playermove {
	U16 nummoves;
	pointU82 pos;
};

static pointS82 dirs[4]={{-1,0},{0,-1},{1,0},{0,1},};

// a board position
struct objects {
	static level lev;
	static int numblocks;
//	static vector<playermove> playerrange;
//	static pointU82 cornerpos;
	pointU82 blocks[MAXNUMBLOCKS];
	/*static */pointU82 playerpos;

	objects(const char* s);
	int hashfunc() const;
	vector<playermove> calcplayerrange(pointU82 startpos,pointU82* cornerpos);
	vector<pointS82> calcmovesfromto(pointU82 startpos,pointU82 endpos);
//	pointU82 calcplayerrange(pointU82 startpos);
	pointU82 getulpos(); // takes player position and moves it to the upper left corner for comparisons
	bool beq(objects& b2);
	void mark();	// take blocks and mark tempblocks in tiles
	S32 move(const pointS82& dir);
	void draw(const pointi2& dir);
	bool iswinner();
	bool isloser();
	void logboard();
};

level objects::lev;
int objects::numblocks;
//pointU82 objects::cornerpos;
//vector<playermove> objects::playerrange;
//pointU82 objects::playerpos;

objects::objects(const char* levelfile)
{
	script lsc(levelfile);
//	int nlsc;
//	char**lsc=loadscript(levelfile,&nlsc);
	if (lsc.num()<=2)
		errorexit("bad level");
	lev.nbx=atoi(lsc.idx(0).c_str());
	lev.nby=atoi(lsc.idx(1).c_str());
	if (lev.nbx>MAXBOARDX || lev.nby>MAXBOARDY)
		errorexit("board too big");
	if (lsc.num() != 2 + lev.nbx * lev.nby)
		errorexit("bad level");
	int i,j;
	for (j=0;j<lev.nby;++j)
		for (i=0;i<lev.nbx;++i)
				lev.tiles[j][i]=atoi(lsc.idx(2+j*lev.nbx+i).c_str());
//	freescript(lsc,nlsc);
	for (j=0;j<lev.nby;++j)
		for (i=0;i<lev.nbx;++i)
			if (i==0 || i==lev.nbx-1 || j==0 || j==lev.nby-1)
				if (lev.tiles[j][i]!=WALL)
					errorexit("missing wall");
	int foundplayer=0;
	int ngoals=0;
	numblocks=0;
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			U8& v=lev.tiles[j][i];
			if (v>=BADL) {
				v-=BADL;
				v+=BAD;
				if (v==GOAL+BAD)
					v=TELEHACK|GOAL;
			}
			if ((v&WALL) && v!=WALL)
				errorexit("wall and something else");
			if ((v&(BLOCK|PLAYER))==(BLOCK|PLAYER))
				errorexit("block and player");
			if (v&PLAYER) {
				if (foundplayer)
					errorexit("already have player");
				playerpos.x=i;
				playerpos.y=j;
				foundplayer=1;
				v&=~PLAYER;
			}
			if (v&BLOCK) {
				if (numblocks>=MAXNUMBLOCKS)
					errorexit("too many blocks");
				blocks[numblocks].x=i;
				blocks[numblocks].y=j;
				++numblocks;
				v&=~BLOCK;
			}
			if (v&GOAL)
				++ngoals;
		}
	}
	if (ngoals!=numblocks)
		errorexit("ngoals = %d, nblocks = %d",ngoals,numblocks);
	if (numblocks<1)
		errorexit("numblocks(%d) < 1",numblocks);
}

int objects::hashfunc() const // mask is 2^n-1
{
	S32 i;
	S32 ret=0;
//	ret=playerpos.x+(playerpos.y<<1);
	for (i=0;i<numblocks;++i) {
		ret<<=4;
		ret+=blocks[i].x+(blocks[i].y<<2);
	}
	return ret & (HASHSIZE-1);
}

vector<playermove> objects::calcplayerrange(pointU82 startpos,pointU82* cornerpos)
{
	S32 j;
	vector<playermove> playerrange;
	playerrange.clear();
	mark();
	playermove sp;
	sp.pos=startpos;
	sp.nummoves=0;
//	pointU82 cornerpos=startpos;
	*cornerpos=startpos;
	playerrange.push_back(sp);
	S32 count=1;
	S32 oldcount=0;
	S32 m=1;
	while(count>oldcount) {
//		if (count>=20)
//			break;
		oldcount=count;
		S32 nj=playerrange.size();
		for (j=0;j<nj;++j) {
			if (playerrange[j].nummoves==m-1) {
				int d;
				for (d=0;d<4;++d) {
					sp=playerrange[j];
					U32 x=sp.pos.x+dirs[d].x;
					U32 y=sp.pos.y+dirs[d].y;
					U8 til=lev.tiles[y][x];
					if (!(til&(TEMPBLOCK|WALL))) {
						S32 h;
						S32 nj2=playerrange.size();
						for (h=0;h<nj2;++h)
							if (playerrange[h].pos.x==x && playerrange[h].pos.y==y)
								break;
						if (h==nj2) {
							playermove spp;
							spp.nummoves=m;
							spp.pos.x=x;
							spp.pos.y=y;
							if (y<cornerpos->y || (y==cornerpos->y && x<cornerpos->x)) {
								cornerpos->x=x;
								cornerpos->y=y;
							}
							playerrange.push_back(spp);
							++count;
						}
					}
				}
			}
		}
		++m;
	}
	return playerrange;
}

vector<pointS82> objects::calcmovesfromto(pointU82 startpos,pointU82 endpos)
{
	pointU82 cp;
	vector<playermove> pr=calcplayerrange(startpos,&cp);
	S32 i,n=pr.size();
	S32 m=0;
// find the first matching one and get the nummoves from it
	for (i=0;i<n;++i) {
		playermove& pm=pr[i];
		if (pm.pos.x==endpos.x && pm.pos.y==endpos.y) {
//			logger("found a fromto at %d,%d on move %d\n",endpos.x,endpos.y,pm.nummoves);
			m=pm.nummoves;
			break;
		}
	}
	vector<pointS82> ret;
	if (i==n) // can't connect
		return ret;
	while(m>0) {
		--m;
		for (i=0;i<n;++i) {
			playermove& pm=pr[i];
			if (pm.nummoves==m) {
				S32 d;
				for (d=0;d<4;++d) {
					U32 x=pm.pos.x+dirs[d].x;
					U32 y=pm.pos.y+dirs[d].y;
					if (x==endpos.x && y==endpos.y) {
//						logger("%d,%d on move %d\n",pm.pos.x,pm.pos.y,m);
						ret.push_back(dirs[d]); // push the move
						endpos=pm.pos;
						break;
					}
				}
				if (d!=4)
					break;
			}
		}
		if (i==n)
			errorexit("can't backtrace move player");

	}
	return ret;
}

pointU82 objects::getulpos() // takes player position and moves it to the upper left corner for comparisons
{
	pointU82 cp;
	calcplayerrange(playerpos,&cp);
	return cp;
}

bool objects::beq(objects& b2)
{
//	mark();
//	pointU82 b1p=getulpos();
//	pointU82 b2p=b2.getulpos();
//	if (b1p.x!=b2p.x || b1p.y!=b2p.y)
//		return false;
	if (playerpos.x!=b2.playerpos.x || playerpos.y!=b2.playerpos.y)
		return false;
	int i;
	for (i=0;i<numblocks;++i)
		if (blocks[i].x!=b2.blocks[i].x || blocks[i].y!=b2.blocks[i].y)
			return false;
	return true;
}

void objects::mark()
{
	S32 i,j;
	for (j=0;j<lev.nby;++j)
		for (i=0;i<lev.nbx;++i)
			lev.tiles[j][i]&=~TEMPBLOCK; // clear
	for (i=0;i<numblocks;++i) {
		S32 ni=blocks[i].x;
		S32 nj=blocks[i].y;
		lev.tiles[nj][ni]|=TEMPBLOCK; // mark
	}
}

S32 objects::move(const pointS82& dir)
{
	int i,j;
	U32 oldi=playerpos.x;
	U32 oldj=playerpos.y;
	U32 newi=oldi+dir.x;
	U32 newj=oldj+dir.y;
// get new square to move to
	U8& newv=lev.tiles[newj][newi];
	if (newv&WALL)
		return 0; // blocked by wall, can't move
// see if there's a block there.
	int sk;
	for (sk=0;sk<numblocks;++sk)
		if (blocks[sk].x==newi && blocks[sk].y==newj)
			break;
//	if (0) {
	if (sk==numblocks) {
		playerpos.x=newi;
		playerpos.y=newj;
		return 1;
	}
	U32 blocki;
	U32 blockj;
	blocki=newi+dir.x;
	blockj=newj+dir.y;
// see if there's a block or wall blocking the block
	U8& newblockv=lev.tiles[blockj][blocki];
	if (newblockv&WALL)
		return 0; // can't move block, because a wall is behind it
	int sk2;
	for (sk2=0;sk2<numblocks;++sk2)
		if (blocks[sk2].x==blocki && blocks[sk2].y==blockj)
			return false; // can't move block, because another block is behind it
// move block objects
	blocks[sk].x+=dir.x;
	blocks[sk].y+=dir.y;
// telehack
	i=blocks[sk].x;
	j=blocks[sk].y;
	U8* lp=&lev.tiles[j][i];
	if (*lp&TELEHACK)
		blocks[sk].x=1;
	playerpos.x=newi;
	playerpos.y=newj;
// sort the blocks
	j=0;
	while(j<numblocks-1) {
		if (blocks[j].y>blocks[j+1].y || 
			(blocks[j].y==blocks[j+1].y && blocks[j].x>blocks[j+1].x)) {
			int tx=blocks[j].x;
			int ty=blocks[j].y;
			blocks[j].x=blocks[j+1].x;
			blocks[j].y=blocks[j+1].y;
			blocks[j+1].x=tx;
			blocks[j+1].y=ty;
			--j;
			if (j<0)
				j=0;
		} else
			++j;
	}
	return 2;
}

void objects::draw(const pointi2& pos)
{
	int i,j;
	int si,sj;
	for (j=0;j<lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32YELLOW);
			int sq=lev.tiles[j][i];
			if (sq&WALL) 
				cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32DARKGRAY);
			if (sq&GOAL) {
				cliprect32(B32,si+4,sj+4,SQSIZE-2-7,SQSIZE-2-7,C32BLUE);
				cliprect32(B32,si+6,sj+6,SQSIZE-2-11,SQSIZE-2-11,C32YELLOW);
			}
			if (sq&BAD) {
				outtextxybf32(B32,si,sj,C32WHITE,C32BLACK,"B");
			}
			if (sq&TELEHACK) {
				outtextxybf32(B32,si,sj,C32WHITE,C32BLACK,"T");
			}
		}
	}
	for (i=0;i<numblocks;++i) {
		si=blocks[i].x*SQSIZE+pos.x;
		sj=blocks[i].y*SQSIZE+pos.y;
		cliprect32(B32,si+8,sj+8,SQSIZE-2-15,SQSIZE-2-15,C32LIGHTBLUE);
//		outtextxyf32(B32,si+9,sj+9,C32WHITE,"%d",i);
	}
	si=playerpos.x*SQSIZE+pos.x;
	sj=playerpos.y*SQSIZE+pos.y;
	clipcircle32(B32,si+(SQSIZE>>1),sj+(SQSIZE>>1),SQSIZE>>2,C32RED);
/*	pointU82 cp;
	vector<playermove>pr=calcplayerrange(playerpos,&cp);
	S32 n=pr.size();
	for (i=0;i<n;++i) {
		si=pr[i].pos.x*SQSIZE+pos.x;
		sj=pr[i].pos.y*SQSIZE+pos.y;
		clipcircle32(B32,si+(SQSIZE>>1)-4,sj+(SQSIZE>>1)-4,4,C32WHITE);
		clipcircle32(B32,si+(SQSIZE>>1)-4,sj+(SQSIZE>>1)-4,2,C32BLACK);
		outtextxyf32(B32,si,sj,C32RED,"%d",pr[i].nummoves);
	}
	si=cp.x*SQSIZE+pos.x;
	sj=cp.y*SQSIZE+pos.y;
	clipcircle32(B32,si+(SQSIZE>>1),sj+(SQSIZE>>1),SQSIZE>>3,C32LIGHTMAGENTA); */
}

bool objects::iswinner()
{
//	return curpos.playerpos.x==1 && curpos.playerpos.y==2;
//	return curpos.playerpos.x==6 && curpos.playerpos.y==4;
	int i;
	for (i=0;i<numblocks;++i) {
		int ni=blocks[i].x;
		int nj=blocks[i].y;
		if (!(lev.tiles[nj][ni]&GOAL))
			return false;
	}
	return true;
}

bool objects::isloser()
{
	int i;
	for (i=0;i<numblocks;++i) {
		int ni=blocks[i].x;
		int nj=blocks[i].y;
		if (lev.tiles[nj][ni]&BAD)
			return true; // if any blocks on a bad square, loser
		if (!(lev.tiles[nj][ni]&GOAL))
			if ( (lev.tiles[nj+1][ni]&WALL) || (lev.tiles[nj-1][ni]&WALL))
				if ( (lev.tiles[nj][ni+1]&WALL) || (lev.tiles[nj][ni-1]&WALL))
					return true; // if a block is in the corner and not on a goal, loser
	}
	mark();
// 4 square
	for (i=0;i<numblocks;++i) {
		int ni=blocks[i].x;
		int nj=blocks[i].y;
/*
		U8 bmm=curpos.lev.tiles[nj-1][ni-1]; // check
		U8 bm0=curpos.lev.tiles[nj-1][ni  ]; // check
		U8 bm1=curpos.lev.tiles[nj-1][ni+1];
		U8 b0m=curpos.lev.tiles[nj  ][ni-1]; // check
		U8 b00=curpos.lev.tiles[nj  ][ni  ]; // check
		U8 b01=curpos.lev.tiles[nj  ][ni+1];
		U8 b1m=curpos.lev.tiles[nj+1][ni-1]; // check
		U8 b10=curpos.lev.tiles[nj+1][ni  ];
		U8 b11=curpos.lev.tiles[nj+1][ni+1];
*/
		U8 b00=lev.tiles[nj  ][ni  ]; // check
		U8 b01=lev.tiles[nj  ][ni+1];
		U8 b10=lev.tiles[nj+1][ni  ];
		U8 b11=lev.tiles[nj+1][ni+1];
		U8 band=b00&b01&b10&b11;
		if (band&TEMPBLOCK) // all blocks
			if (!(band&GOAL)) // and at least one of these 4 blocks is not on a goal
			   return true;
// 2 square, check 1
		U8 bb=b00&b01;
		U8 bw=b10&b11;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 2 square, check 2
		bb=b00&b10;
		bw=b01&b11;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 2 square, check 3
		U8 bm0=lev.tiles[nj-1][ni  ]; // check
		U8 bm1=lev.tiles[nj-1][ni+1];
		bb=b00&b01;
		bw=bm0&bm1;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 2 square, check 4
		U8 b0m=lev.tiles[nj  ][ni-1]; // check
		U8 b1m=lev.tiles[nj+1][ni-1];
		bb=b00&b10;
		bw=b0m&b1m;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 3 square, check 1
		bb=b00&b01&b10;
		bw=b11;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 3 square, check 2
		bb=b00&b01&bm0;
		bw=bm1;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 3 square, check 3
		bb=b00&b0m&b10;
		bw=b1m;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 3 square, check 4
		U8 bmm=lev.tiles[nj-1][ni-1]; // check
		bb=b00&b0m&bm0;
		bw=bmm;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
	}
	return false;
}

void objects::logboard()
{
	mark();
	S32 i,j;
	for (j=0;j<lev.nby;++j) {
		for (i=0;i<lev.nbx;++i) {
			if (playerpos.x==i && playerpos.y==j)
				logger("P");
			else {
				U8 v=lev.tiles[j][i];
				if (v&WALL)
					logger("W");
				else if (v&TEMPBLOCK)
					logger("B");
				else
					logger(".");
			}
		}
		logger("\n");
	}
	logger("\n");
}

struct boardcollinfo {
	objects objs;
	U16 nummoves;
	boardcollinfo(const objects& o,int nm) : 
		objs(o),nummoves(nm)
	{
		objs.playerpos=objs.getulpos();	
	}
	void logboard();
};

void boardcollinfo::logboard()
{
	logger(" -------- board --- nummoves %d -------\n",nummoves);
	objs.logboard();
}

// a large collection of boards
class boardcoll {
public:
	S32 count;
//	vector<boardcollinfo> bci[HASHSIZE] ;
	vector<boardcollinfo> *bci;
	boardcoll() : count(0)
	{
		bci = new vector<boardcollinfo>[HASHSIZE];
	}
	~boardcoll()
	{
		delete[] bci;
	}
	S32 addboard(objects& b,int movecount);
//	boardcoll() : count(0) {}
	void showboards();
};

S32 boardcoll::addboard(objects& o,int nm)
{
	int hash=o.hashfunc();
	int n=(int)bci[hash].size();
	int i;
	boardcollinfo b(o,nm);
	for (i=0;i<n;++i) {
		boardcollinfo& br=bci[hash][i];
		if (b.objs.beq(br.objs)) {
			if (nm>=br.nummoves) 
				return count;
			else
				errorexit("bad addboard");
		}
	}
//	b.logboard();
	bci[hash].push_back(b);
	++count;
	return count;
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

class game {
	objects resetpos;
	objects curpos;
	stack<objects,vector<objects> > undostack;
	stack<pointS82,vector<pointS82> > movesolvestackr;
	void solve();
//	bool solve_rec();
public:
	game(const char* levelfile) : resetpos(levelfile),curpos(resetpos) {}
	void reset();
	void undo();
	void hint();
	void clearsolvestack();
	int getundossize();
	void addundo(const objects& b);
	int gethintssize();
	objects* getcurpos() {return &curpos;}
	void backtrace(S32 m,const boardcoll& bc,objects cp,pointU82 pps);
	void domovesfromto(pointU82 start,pointU82 end); // on curpos, puts onto hint stack
};

void game::hint()
{
	if (curpos.iswinner())
		return;
	if (curpos.isloser())
		return;
	if (movesolvestackr.empty()) {
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"S O L V I N G");
		winproc();
		solve();
	}
	if (movesolvestackr.empty())
		return;
	pointS82 thehint;
	thehint=movesolvestackr.top();
	movesolvestackr.pop();
	const objects o=curpos;
	if (curpos.move(thehint)) {
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

void game::backtrace(S32 m,const boardcoll& bc,objects op,pointU82 pps)
{
	op.playerpos=op.getulpos();
//	logger("start backtrace on board\n");
//	op.logboard();
	pointU82 endpos;
	bool hasendpos=false;
	while(1) {
		--m;
		if (m<0)
			break;
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-36,C32WHITE,C32BLACK,"BACKTRACE move %d ",m,HASHSIZE);
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-36,F32WHITE,F32BLACK,"BACKTRACE move %d ",m,HASHSIZE);
//		video_unlock();
		winproc();
		int i;
		for (i=0;i<HASHSIZE;++i) {
			const vector<boardcollinfo>& hr=bc.bci[i];
			int nj=(int)hr.size();
			int j;
			for (j=0;j<nj;++j) {
				if (hr[j].nummoves==m) {
					curpos=hr[j].objs;
//					logger("possible backtrace before move\n");
//					curpos.logboard();
					pointU82 cp;
					vector<playermove> pr=curpos.calcplayerrange(curpos.playerpos,&cp);
					int d;
					for (d=0;d<4;++d) {
						S32 k,kn=pr.size();
						for (k=0;k<kn;++k) {
							curpos=hr[j].objs;
							curpos.playerpos=pr[k].pos;
							S32 movestat=curpos.move(dirs[d]);
//							if (movestat>0) {
							if (movestat==2) {
								curpos.playerpos=curpos.getulpos();	
//								logger("possible backtrace after move\n");
//								curpos.logboard();
								if (curpos.beq(op)) {
									pointU82 ds;
									ds.x=pr[k].pos.x+dirs[d].x;
									ds.y=pr[k].pos.y+dirs[d].y;
//									logger("move %d, push from %d,%d to %d,%d\n",
//									  m,pr[k].pos.x,pr[k].pos.y,ds.x,ds.y);
									op=hr[j].objs;
									if (hasendpos) {
//										curpos=hr[j].objs;
//										logger("player move from %d,%d to %d,%d\n",ds.x,ds.y,endpos.x,endpos.y);
										curpos.playerpos=pr[k].pos;
										domovesfromto(ds,endpos);
									}
									
									movesolvestackr.push(dirs[d]); // a block move
									hasendpos=true;
									endpos=pr[k].pos;
									break;
								}
							}
						}
						if (k!=kn)
							break;
					}
					if (d!=4)
						break;
				}
			}
			if (j!=nj)
				break;
		}
		if (i==HASHSIZE)
			errorexit("can't backtrace");
	}
	curpos=resetpos;
//	logger("last move from %d,%d to %d,%d\n",pps.x,pps.y,endpos.x,endpos.y);
	domovesfromto(pps,endpos);
}

void game::domovesfromto(pointU82 from,pointU82 to)
{
//	logger("doing a domovesfromto with from %d,%d and to %d,%d\n",from.x,from.y,to.x,to.y);
//	curpos.logboard();
	vector<pointS82> moves=curpos.calcmovesfromto(from,to);
	S32 i,n=moves.size();
	for (i=0;i<n;++i) {
//		logger("movelist %d,%d\n",moves[i].x,moves[i].y);
		movesolvestackr.push(moves[i]);
	}
}

void game::solve()
{
	static int pcnt=-1;
	objects bsave=curpos;
	boardcoll bc; // the solver data
	bc.addboard(curpos,0);
	int m=1;
	int oldcount=bc.count;
	logger("count for moves = 0 is %d\n",bc.count);
	while(1) {
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d count %8d",m+1,0);
//		video_unlock();
//		winproc();
		int i;
		for (i=0;i<HASHSIZE;++i) {
			const vector<boardcollinfo>& hr=bc.bci[i];
			int nj=(int)hr.size();
			int j;
			for (j=0;j<nj;++j) {
				if (hr[j].nummoves==m-1) {
					curpos=hr[j].objs;
					pointU82 cp;
					vector<playermove> pr=curpos.calcplayerrange(curpos.playerpos,&cp);
					int d;
					for (d=0;d<4;++d) {
						S32 k,kn=pr.size();
						for (k=0;k<kn;++k) {
							curpos=hr[j].objs;
							curpos.playerpos=pr[k].pos;
							S32 movestat=curpos.move(dirs[d]);
//							if (movestat>0) {
							if (movestat==2) {
//								if (movestat==2) {
//									logger("movestat %d, on old ppos %d,%d, new ppos %d,%d\n",
//									  movestat,pr[k].pos.x,pr[k].pos.y,
//									  curpos.playerpos.x,curpos.playerpos.y);
//								}
								if (curpos.isloser())
									continue;
								if (curpos.iswinner()) {
									logger("found a winner on move %d,currently at h%d,i%d \n",m,i,j);
									bc.showboards();
//									logger("pushing %d,%d on move solve stack\n",dirs[d].x,dirs[d].y);
//									movesolvestackr.push(dirs[d]); // the final move
									backtrace(m,bc,curpos,bsave.playerpos);
//									delete bc; // free up all those board positions
									curpos=bsave;
									return;
								} 
//								logger("begin addboard\n");
								S32 cnt=bc.addboard(curpos,m);
// check back with the OS once in a while
								if (cnt%1000==0 && pcnt!=cnt) {
									bool kill=false;
//									video_lock();
//									outtextxybf32(B32,(WX>>1)+100,580-28,C32WHITE,C32BLACK,"h  %d / %d       ",i,HASHSIZE);
//									outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",m,cnt);
									drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-28,F32WHITE,F32BLACK,"h  %d / %d       ",i,HASHSIZE);
									drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"SOLVING... try %3d,count %8d",m,cnt);
//									video_unlock();
									winproc();
									if (KEY=='a')
										kill=true;
									if (wininfo.closerequested)
										kill=true;
									if (kill) {
//										delete bc; // free up all those board positions
										curpos=bsave;
										return;
									}
								}
								pcnt=cnt;
//								logger("end addboard\n");
							} // moved a block
						} // move list
					} // 4 directions
				} // found correct prev move
			} // num in hash slot
		} // num hash slots
//		int nc=bc->count;
		logger("count for moves = %d is %d\n",m,bc.count);
		if (bc.count==oldcount) { // no new moves
			logger("no new moves found during move %d\n",m);
			break;
		}
		oldcount=bc.count;
		++m;
//		if (m==100)
//			break;
	} // new counts
//	delete bc;
	curpos=bsave;
}

game* gp;
script* sc;
int curlevel;

} // end namespace sokoban

using namespace sokoban2;

void sokoban2init()
{
	S32 i;
	video_setupwindow(800,600);
	pushandsetdir("sokoban");
	logger("sizeof objects %d\n",sizeof(objects));
	logger("sizeof boardcollinfo %d\n",sizeof(boardcollinfo));
	sc=new scriptdir(0);
	sc->sort();
//	sc=doadir(&nsc,0);
//	sortscript(sc,nsc);
	for (i=0;i<sc->num();++i)
		logger("level %2d: \"%s\"\n",i,(*sc).idx(i).c_str());
	curlevel=0;
	curlevel=sc->num()-2;
	if (curlevel<0) curlevel=0;
	gp=new game((*sc).idx(curlevel).c_str());
//	gp=new game(level1);
//	gp=new game(levele);
}

static S32 lastmovestatus;
void sokoban2proc()
{
	lastmovestatus=0;
	pointS82 dir={0,0};
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
	case 't':
		{
			pointU82 from={3,4};
			pointU82 to={9,4};
			gp->domovesfromto(from,to);
			break;
		}
	};
	if (dir.x || dir.y) {
		objects* o=gp->getcurpos();
		objects old=*o;
		lastmovestatus=o->move(dir);
		if (lastmovestatus) {
			gp->clearsolvestack();
			gp->addundo(old);
		}
	}
}

void sokoban2draw2d()
{
	const pointi2 board1pos={STARTX,STARTY};
//	video_lock();
	clipclear32(B32,C32BROWN);
//	cliprect32(B32,0,0,WX,WX,C32BROWN);
	gp->getcurpos()->draw(board1pos);
//	gp->draw(board2pos,true);
	outtextxyf32(B32,WX/2-36,8,C32WHITE,"SOKOBAN 2");
	if (gp->getcurpos()->iswinner())
		outtextxyf32(B32,750,560,mt_randU32(),"WINNER!");
	outtextxyf32(B32,10,580,C32WHITE,
		"'r' reset  'u' undo  'h' hint  'a' abort hint  '=' levelup  '-' leveldown  undos(%d), hints(%d)",
		gp->getundossize(),gp->gethintssize());
	outtextxyf32(B32,10,560,C32WHITE,"LEVEL : '%s'",(*sc).idx(curlevel).c_str());
	outtextxyf32(B32,10,568,C32WHITE,"last move status : %d",lastmovestatus);
//	video_unlock();
}

void sokoban2exit()
{
	delete gp;
//	freescript(sc,nsc);
	delete sc;
	popdir();
}
#include <poppack.h>
