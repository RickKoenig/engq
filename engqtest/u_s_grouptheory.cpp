#include <m_eng.h>

namespace grouptheory {

#define HASHSIZE 0x200 // a power of 2

class cycle
{
	U32 orderc;
	vector<U32> c;
public:
	cycle(S32 oa=0,const S32* ca=0) : orderc(oa)
	{
		if (ca==0)
			return;
		U32 i=0;
		while(ca[i]!=-1)
			c.push_back(ca[i++]);
	}
	void setorder(U32 ord) {orderc=ord;}
	U32 size()
	{
		return c.size();
	}
	void addelement(S32 cv)
	{
		c.push_back(cv);
	}
	bool operator==(const cycle& cr) const;
	void print() const
	{
		U32 i;
		logger("( ",c.size());
		for (i=0;i<c.size();++i)
			logger("%d ",c[i]);
		logger(") ");
	}
	void clear(U32 ord)
	{
		c.clear();
		orderc=ord;
	}
	friend class permute;
};

bool cycle::operator ==(const cycle& cr) const
{
	return orderc==cr.orderc && c==cr.c;
}

class permute
{
	U32 orderp;
	vector<cycle> p;
	bool fuena;	// enable fixup for addcycle, default
	void fixup();
public:
	permute(U32 oa=0) : orderp(oa),fuena(true) {}
	bool operator==(const permute& pr) const;
	permute operator*(const permute pr) const;
	void setorder(U32 ord){orderp=ord;}
	U32 size()
	{
		return p.size();
	}
	void clear(U32 ord=0)
	{
		p.clear();
		orderp=ord;
	}
	void addcycle(const cycle& ca)
	{
		p.push_back(ca);
		if (fuena)
			fixup();
	}
	void print() const
	{
		U32 i;
		logger("%d cycles ",p.size());
		for (i=0;i<p.size();++i) {
			p[i].print();
		}
		logger("\n");
	}
	U32 hashfunc() const // mask is 2^n-1
	{
		return p.size() & (HASHSIZE-1);
	}
};

bool permute::operator==(const permute& pr) const
{
	return orderp==pr.orderp && p==pr.p;
}
// simplify and order, using permute array of orderp
void permute::fixup()
{
	vector<U32> cur(orderp);
	vector<U32> next(orderp);
	U32 i,j,k;
// init to identity
	for (i=0;i<orderp;++i)
		cur[i]=i;
// apply cycles to permute array, play shell game
	for (i=0;i<orderp;++i)
		next[i]=i;
	for (k=0;k<p.size();++k) {
		const vector<U32>& c=p[k].c;
		for (i=0;i<c.size();++i) {
			j=i+1;
			if (j==c.size())
				j=0;
			next[c[j]]=cur[c[i]];
		}
		cur=next;
	}
// convert to 'next' notation
	for (i=0;i<orderp;++i)
		next[cur[i]]=i;
/*	logger("fixup ");
	for (i=0;i<orderp;++i)
		logger("%d ",next[i]);
	logger("\n"); */
// convert 'next' notation back to cycle notation
	p.clear();
// use cur as 'marked'
	fill(cur.begin(),cur.end(),0);
	i=0; // start at 0
	while(i<orderp) {
		if (cur[i]) { // marked
			++i;
			continue;
		}
		U32 j=i;
		cycle cy(orderp);
		while(1) {
			cur[j]=1; // mark
			cy.addelement(j);
			j=next[j]; // walk the cycle
			if (j==i)
				break;
			
		};
		if (cy.size()>1)
			p.push_back(cy);
		++i;
	}
	
}

permute permute::operator*(permute pr) const
{
	permute ret(orderp);
	U32 i;
	ret.fuena=false;
	for (i=0;i<p.size();++i)
		ret.addcycle(p[i]);
	for (i=0;i<pr.p.size();++i)
		ret.addcycle(pr.p[i]);
	ret.fuena=true;
	ret.fixup();
	return ret;
}

// prototype level data, loaded from file
#include <pshpack1.h>
struct level {
	U32 lorder;
	permute goal;
	vector <permute> initperms; // initial permutations
	level(U32 ord) : lorder(ord),goal(ord) {}
	level(const C8* levelfile);
};

level::level(const C8* levelfile)
{
	// order,init,goal,cycleinit,cyclegoal,
	// collectnumbersinit,collectnumbersgoal
	enum modest{O,I,G,CI,CG,CDI,CDG}; 
	modest m=O;
	script lsc(levelfile);
	S32 i=0;
	cycle c;
	permute p;
	while(i<lsc.num()) {
		const C8* s=0;
		switch(m) {
		case O:
			if (lsc.idx(i)!="order")
				errorexit("expected order");
			lorder=atoi(lsc.idx(i+1).c_str());
			goal.setorder(lorder);
			i+=2;
			m=I; // change mode
			break;
		case I:
			if (lsc.idx(i)!="init") { // try for goal
				m=G; // change mode
				break;
			}
			p.clear(lorder);
			c.clear(lorder);
			++i;
			m=CI;
			break;
		case G:
			p.clear(lorder);
			if (lsc.idx(i)!="goal") { 
				errorexit("expected goal");
			}
			p.clear(lorder);
			c.clear(lorder);
			++i;
			m=CG;
			break;
		case CI:
			if(lsc.idx(i)!="cycle") {
				if (p.size()>0) {
					initperms.push_back(p);
					m=I;
					break;
				}
			}
			m=CDI;
			++i;
			break;
		case CG:
			if(lsc.idx(i)=="cycle") {
				m=CDG;
				++i;
				break;
			}
			if (c.size()>1)
				p.addcycle(c);
			if (p.size()>0)
				goal=p;
			m=I;
			break;
		case CDI:
			s=lsc.idx(i).c_str();
			if (s[0]<'0' || s[0]>'9') {
				m=CI;
				p.addcycle(c);
				break;
			}
		case CDG:
			break;
		};
	}
	if (c.size()>1)
		p.addcycle(c);
	if (p.size()>0) {
		if (m==CDG)
			goal=p;
	}
}

bool insolve;
S32 g_prvh;
S32 g_nm;
S32 g_acount;


#ifdef BACKTRACE
struct boardcollinfo {
	objects objs;
	U16 prevhidx;
	int previidx;
	pointS82 movedir;
	U8 nummoves;
	boardcollinfo(const objects& o,int nm,int phi,int pii,const pointS82& md) : 
		objs(o),prevhidx(phi),previidx(pii),movedir(md),nummoves(nm) {}
};
#else
struct boardcollinfo {
	objects objs;
	U16 nummoves;
	boardcollinfo(const objects& o,int nm) : 
		objs(o),nummoves(nm) {}
};
#endif
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
	bool addboard(const objects& b,int movecount,int prvh,int prvi,const pointS82& movedir); // returns if already in collection
//	boardcoll() : count(0) {}
	void showboards();
};

bool boardcoll::addboard(const objects& o,int nm,int prvh,int prvi,const pointS82& movedir)
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
#ifdef BACKTRACE
	boardcollinfo b(o,nm,prvh,prvi,movedir);
#else
	boardcollinfo b(o,nm);
#endif
	bci[hash].push_back(b);
	++acount;
	if (acount%1000==1) {
/*		video_lock();
		outtextxybf32(B32,(WX>>1)+100,580-28,C32WHITE,C32BLACK,"h  %d / %d   ",prvh,HASHSIZE);
		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
		video_unlock(); */
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

class game {
	objects resetpos;
	objects curpos;
	stack<objects,vector<objects> > undostack;
	stack<pointi2,vector<pointS82> > movesolvestackr;
	void solve();
//	bool solve_rec();
	boardcoll* bc; // the solver data
public:
	game(const char* levelfile) : resetpos(levelfile),curpos(resetpos) {}
	void reset();
	void draw(const pointi2& pos) const;
	bool move(const pointS82& dir);
	bool iswinner();
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
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j*SQSIZE+pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i*SQSIZE+pos.x;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,C32YELLOW);
			int sq=curpos.lev.tiles[j][i];
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
	for (i=0;i<curpos.numblocks;++i) {
		si=curpos.blocks[i].x*SQSIZE+pos.x;
		sj=curpos.blocks[i].y*SQSIZE+pos.y;
		cliprect32(B32,si+8,sj+8,SQSIZE-2-15,SQSIZE-2-15,C32LIGHTBLUE);
//		outtextxyf16(B16,si+(SQSIZE>>1)-4,sj+(SQSIZE>>1)-4,hiwhite,"%d",i);
	}
	si=curpos.playerpos.x*SQSIZE+pos.x;
	sj=curpos.playerpos.y*SQSIZE+pos.y;
	clipcircle32(B32,si+(SQSIZE>>1),sj+(SQSIZE>>1),SQSIZE>>2,C32RED);
}

bool game::move(const pointS82& dir)
{
	int i,j;
	U32 oldi=curpos.playerpos.x;
	U32 oldj=curpos.playerpos.y;
	U32 newi=oldi+dir.x;
	U32 newj=oldj+dir.y;
// get new square to move to
	U8& newv=curpos.lev.tiles[newj][newi];
	if (newv&WALL)
		return false; // in wall, can't move
// see if there's a block there.
	int sk;
	for (sk=0;sk<curpos.numblocks;++sk)
		if (curpos.blocks[sk].x==newi && curpos.blocks[sk].y==newj)
			break;
//	if (0) {
	if (sk!=curpos.numblocks) {
		U32 blocki;
		U32 blockj;
		blocki=newi+dir.x;
		blockj=newj+dir.y;
// see if there's a block or wall blocking the block
		U8& newblockv=curpos.lev.tiles[blockj][blocki];
		if (newblockv&WALL)
			return false; // can't move block, because a wall is behind it
		int sk2;
		for (sk2=0;sk2<curpos.numblocks;++sk2)
			if (curpos.blocks[sk2].x==blocki && curpos.blocks[sk2].y==blockj)
				return false; // can't move block, because another block is behind it
// move block objects
		curpos.blocks[sk].x+=dir.x;
		curpos.blocks[sk].y+=dir.y;
// telehack
		i=curpos.blocks[sk].x;
		j=curpos.blocks[sk].y;
		U8* lp=&curpos.lev.tiles[j][i];
		if (*lp&TELEHACK)
			curpos.blocks[sk].x=1;
	}
	curpos.playerpos.x=newi;
	curpos.playerpos.y=newj;
// sort the blocks
	j=0;
	while(j<curpos.numblocks-1) {
		if (curpos.blocks[j].y>curpos.blocks[j+1].y || 
			(curpos.blocks[j].y==curpos.blocks[j+1].y && curpos.blocks[j].x>curpos.blocks[j+1].x)) {
			int tx=curpos.blocks[j].x;
			int ty=curpos.blocks[j].y;
			curpos.blocks[j].x=curpos.blocks[j+1].x;
			curpos.blocks[j].y=curpos.blocks[j+1].y;
			curpos.blocks[j+1].x=tx;
			curpos.blocks[j+1].y=ty;
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
	pointS82 thehint;
	thehint=movesolvestackr.top();
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

bool game::iswinner()
{
//	return curpos.playerpos.x==1 && curpos.playerpos.y==2;
//	return curpos.playerpos.x==6 && curpos.playerpos.y==4;
	int i;
	for (i=0;i<curpos.numblocks;++i) {
		int ni=curpos.blocks[i].x;
		int nj=curpos.blocks[i].y;
		if (!(curpos.lev.tiles[nj][ni]&GOAL))
			return false;
	}
	return true;
}

bool game::isloser()
{
/*
	struct level {
	U8 nbx,nby;
	U8 tiles[MAXBOARDY][MAXBOARDX];
};
*/
	int i,j;
	for (i=0;i<curpos.numblocks;++i) {
		int ni=curpos.blocks[i].x;
		int nj=curpos.blocks[i].y;
		if (curpos.lev.tiles[nj][ni]&BAD)
			return true; // if any blocks on a bad square, loser
		if (!(curpos.lev.tiles[nj][ni]&GOAL))
			if ( (curpos.lev.tiles[nj+1][ni]&WALL) || (curpos.lev.tiles[nj-1][ni]&WALL))
				if ( (curpos.lev.tiles[nj][ni+1]&WALL) || (curpos.lev.tiles[nj][ni-1]&WALL))
					return true; // if a block is in the corner and not on a goal, loser
	}
// check for 4 square and 2 on the side and 3 on a corner
	for (j=0;j<curpos.lev.nby;++j)
		for (i=0;i<curpos.lev.nbx;++i)
			curpos.lev.tiles[j][i]&=~TEMPBLOCK; // clear
	for (i=0;i<curpos.numblocks;++i) {
		int ni=curpos.blocks[i].x;
		int nj=curpos.blocks[i].y;
		curpos.lev.tiles[nj][ni]|=TEMPBLOCK; // mark
	}
// 4 square
	for (i=0;i<curpos.numblocks;++i) {
		int ni=curpos.blocks[i].x;
		int nj=curpos.blocks[i].y;
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
		U8 b00=curpos.lev.tiles[nj  ][ni  ]; // check
		U8 b01=curpos.lev.tiles[nj  ][ni+1];
		U8 b10=curpos.lev.tiles[nj+1][ni  ];
		U8 b11=curpos.lev.tiles[nj+1][ni+1];
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
		U8 bm0=curpos.lev.tiles[nj-1][ni  ]; // check
		U8 bm1=curpos.lev.tiles[nj-1][ni+1];
		bb=b00&b01;
		bw=bm0&bm1;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
// 2 square, check 4
		U8 b0m=curpos.lev.tiles[nj  ][ni-1]; // check
		U8 b1m=curpos.lev.tiles[nj+1][ni-1];
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
		U8 bmm=curpos.lev.tiles[nj-1][ni-1]; // check
		bb=b00&b0m&bm0;
		bw=bmm;
		if ((bb&TEMPBLOCK)&&(bw&WALL)) // 2 blocks on a wall
			if (!(bb&GOAL)) // and at least one of these 2 blocks is not on a goal
			   return true;
	}
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

objects game::getcurpos() const
{
	return curpos;
}

void game::solve()
{
	insolve=true;
	static pointS82 dirs[4]={{-1,0},{0,-1},{1,0},{0,1},};
	pointS82 dum={0,0};
	objects bsave=curpos;
	bc=new boardcoll;
	bc->addboard(curpos,0,-1,-1,dum);
	int m=1;
	int oldcount=bc->acount;
	logger("count for moves = 0 is %d\n",bc->acount);
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
					int d;
					for (d=0;d<4;++d) {
						curpos=hr[j].objs;
						if (move(dirs[d])) {
							if (isloser())
								continue;
							if (iswinner()) {
								logger("found a winner on move %d,currently at h%d,i%d \n",m,i,j);
								bc->showboards();
								logger("pushing %d,%d on move solve stack\n",dirs[d].x,dirs[d].y);
								movesolvestackr.push(dirs[d]);
#ifdef BACKTRACE
								int ph=hr[j].prevhidx;
								int pi=hr[j].previidx;
								pointS82 p=hr[j].movedir;
								while(--m) {
									logger("pushing %d,%d\n",p.x,p.y);
									movesolvestackr.push(p);
									const boardcollinfo& hr2=bc->bci[ph][pi];
									ph=hr2.prevhidx;
									pi=hr2.previidx;
									p=hr2.movedir;
								}
#else // recreate hint backtrace from boardcollinfo
								objects curpost=curpos;
								while(1) {
									--m;
									if (m<0)
										break;
									for (i=0;i<HASHSIZE;++i) {
										const vector<boardcollinfo>& hr=bc->bci[i];
										int nj=(int)hr.size();
										int j;
										for (j=0;j<nj;++j) {
											if (hr[j].nummoves==m) {
												int d;
												for (d=0;d<4;++d) {
													curpos=hr[j].objs;
													if (move(dirs[d])) {
														if (curpos.beq(curpost)) {
															movesolvestackr.push(dirs[d]);
															curpost=hr[j].objs;
															break;
														}
													}
												}
												if (d!=4)
													break;
											}
										}
										if (j!=nj)
											break;
									}
									if (i==HASHSIZE)
										errorexit("couldn't backtrace");
								}
#endif
								delete bc; // free up all those board positions
								curpos=bsave;
								insolve=false;
								return;
							} 
							if (!bc->addboard(curpos,m,i,j,dirs[d])) {
								delete bc; // free up all those board positions
								curpos=bsave;
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
scriptnoinc* sc;
int curlevel;

} // end namespace grouptheory

using namespace grouptheory;

void grouptheoryinit()
{
	insolve=false;
	S32 i;
	video_setupwindow(800,600);
	pushandsetdir("grouptheory2");
	logger("sizeof objects %d\n",sizeof(objects));
	logger("sizeof boardcollinfo %d\n",sizeof(boardcollinfo));
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

void grouptheoryproc()
{
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
	};
	if (dir.x || dir.y) {
		const objects o=gp->getcurpos();
		if (gp->move(dir)) {
			gp->clearsolvestack();
			gp->addundo(o);
		}
	}
}

void grouptheorydraw2d()
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
	if (insolve) {
		outtextxybf32(B32,(WX>>1)+100,580-28,C32WHITE,C32BLACK,"h  %d / %d   ",g_prvh,HASHSIZE);
		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",g_nm+1,g_acount);
	}
}

void grouptheoryexit()
{
	delete gp;
//	freescript(sc,nsc);
	delete sc;
	popdir();
}
