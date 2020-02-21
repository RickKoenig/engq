#include <m_eng.h>
#include "u_states.h"
#include "d2_font.h"
#include "m_perf.h"

#define NAMESPACE

#ifdef NAMESPACE
namespace jelly {
#endif

#define MAXBOARDX 14
#define MAXBOARDY 10

// file level tile types
// bit 0-3 piece number
// bit 4 isstuck
// bit 5 iswall
// bit 6 ispiece
// if bits 4 - 6 are all 0, then is a blank

#define MAXNUMPIECES 16 // a power of 2

#define BLANK 0
#define WALL 1
#define PIECESTART 2
// pieces start at 2
//#define STUCK 0x10
#if 0
#define STUCKLEFT 0x10
#define STUCKRIGHT 0x20
#define STUCKUP 0x40
#define STUCKDOWN 0x80
#endif
//#define WALL 0x100
//#define PIECE 0x200
//#define PIECEMASK (MAXNUMPIECES-1)
//#define FILLED

// graphics
#define STARTX 30
#define STARTY 30
#define SQSIZE 35

struct pointS8 {
	S8 x,y;
};

pointS8 zeropS8 = {0,0};

#define HASHSIZE 0x10000 // a power of 2
//#define HASHSIZE 0x200000 // a power of 2
//#define HASHSIZE 32768 // a power of 2
struct lrud {
	unsigned int lf:1;
	unsigned int rt:1;
	unsigned int up:1;
	unsigned int dn:1;
};

struct tile {
	unsigned int pce:4;
	unsigned int stuck:1;
	unsigned int filled:1;
	unsigned int visited:1;
	//int lr:2;
	//int ud:2;
	lrud hook;
	bool operator!=(const tile& rhs) const;
};

tile blanktile = {0,0,0,0,{0,0,0,0}};//,0};

bool tile::operator!=(const tile& rhs) const
{
	if (pce != rhs.pce)
		return true;
	//if (lr != rhs.lr)
	//	return true;
	//if (ud != rhs.ud)
	//	return true;
	if (hook.lf != rhs.hook.lf)
		return true;
	if (hook.rt != rhs.hook.rt)
		return true;
	if (hook.up != rhs.hook.up)
		return true;
	if (hook.dn != rhs.hook.dn)
		return true;
	return false;
}

struct ptile {
	pointS8 loc;
	int pce:4;
	//int lr:2;
	//int ud:2;
	lrud hook;
};


struct piece {
	//int pce;
	pointS8 off;
	vector<ptile> tls;
};

struct level {
	static int nbx,nby;
	tile tiles[MAXBOARDY][MAXBOARDX];
	//static vector<piece> pieces; // for move calculation
};
int level::nbx;
int level::nby;

const int testwid = 10;
const int testhit = 8;
const pointS8 teststartfill = {5,3};

// 0 empty
// 1 block
// 2 filled block
tile testarr[testhit][testwid] = {
	{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
	{{0,0,0},{0,0,0},{0,0,0},{2,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
	{{0,0,0},{0,0,0},{0,0,0},{2,0,0},{2,0,0},{0,0,0},{2,0,0},{0,0,0},{2,0,0},{0,0,0}},
	{{0,0,0},{2,0,0},{2,0,0},{0,0,0},{2,0,0},{2,0,0},{2,0,0},{0,0,0},{2,0,0},{0,0,0}},
	{{0,0,0},{2,0,0},{0,0,0},{0,0,0},{2,0,0},{0,0,0},{2,0,0},{2,0,0},{2,0,0},{0,0,0}},
	{{0,0,0},{2,0,0},{0,0,0},{2,0,0},{2,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
	{{0,0,0},{2,0,0},{2,0,0},{2,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
	{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};

// assume wall sentinals

//vector<pointS8> floodfill(tile* arr,int wid,const pointS8& fpnt,bool special)
vector<ptile> floodfill(tile* arr,int wid,const pointS8& fpnt,bool special)
{
	perf_start(TEST1);
	vector<int> fillstack;
	fillstack.reserve(20);
	//vector<pointS8> filllist;
	vector<ptile> fillt;
	fillt.reserve(20);
	int sp = fpnt.y*wid + fpnt.x;
	if (arr[sp].pce < PIECESTART) { // check if fpnt is a movable block
		perf_end(TEST1);
		return fillt;
	}
	//if (arr[sp].stuck) // can't move
	//	return filllist;
	int targpce = arr[sp].pce;
	//targpce = -1; // make non-stick
	fillstack.push_back(sp);
	while (!fillstack.empty()) {
		sp = fillstack.back();
		fillstack.pop_back();
		if (arr[sp].filled) // check if fpnt is already filled
			continue;
		// walk left
		int lf = sp; // - 1;
		while(true) {
			if (arr[lf].pce == arr[lf-1].pce || (special && arr[lf].hook.lf))
				--lf;
			else
				break;
		}
		//while(arr[--lf].pce == targpce)
		//	;
		//++lf; // back to first targpce
		// walk right
		int rt = sp;
		//while(arr[++rt].pce == targpce)
		//	;
		while(true) {
			if (arr[rt].pce == arr[rt+1].pce || (special && arr[rt].hook.rt))
				++rt;
			else
				break;
		}
		++rt;
		// stay 1 past last targpce
		// start filling
		int f;
		for (f=lf;f<rt;++f) {
			arr[f].filled = 1;
			pointS8 p;
			p.x = f%wid;
			p.y = f/wid;
			//filllist.push_back(p);
			ptile pt;
			pt.loc = p;
			//pt.lr = arr[f].lr;
			//pt.ud = arr[f].ud;
			pt.hook = arr[f].hook;
			pt.pce = arr[f].pce;
			fillt.push_back(pt);
			// look up and down
			int u = f - wid;
			int d = f + wid;
			if ((arr[u].pce == arr[f].pce || (special && arr[f].hook.up)) && !arr[u].filled) {
				fillstack.push_back(u);
			}
			if ((arr[d].pce == arr[f].pce || (special && arr[f].hook.dn)) && !arr[d].filled) {
				fillstack.push_back(d);
			}
		}
	}
	int i,n=fillt.size();
//	int i,n=filllist.size();
	for (i=0;i<n;++i)
		arr[fillt[i].loc.y*wid + fillt[i].loc.x].filled = 0;
		//arr[filllist[i].y*wid + filllist[i].x].filled = 0;
	perf_end(TEST1);
	return fillt;
	//return filllist;
}


// a board position
struct board {
	level lev;
	//pointS8 cp;

	int hashfunc() const;
	bool beq(const board& b2) const;
	board(const char* s);
	tile parse_tile(const char* t);
	static void cleanup();
};

void board::cleanup()
{
}

int board::hashfunc() const // mask is 2^n-1
{
	perf_start(TEST2);
	S32 i;
	S32 ret=0;
	for (i=0;i<MAXBOARDX*MAXBOARDY;++i) {
		//ret<<=4;
		S32 p = lev.tiles[0][i].pce;
		ret = 7*ret + p;
	}
	perf_end(TEST2);
	return ret & (HASHSIZE-1);
}

bool board::beq(const board& b2) const
{
	int i;
	perf_start(TEST3);
	for (i=0;i<MAXBOARDX*MAXBOARDY;++i) {
		if (lev.tiles[0][i] != b2.lev.tiles[0][i]) {
			perf_end(TEST3);
			return false;
		}
//		if (lev.tiles[0][i].stuck != b2.lev.tiles[0][i].stuck)
//			return false;
	}
	perf_end(TEST3);
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
	tile ret = blanktile;
	//const char* ts=t;
	while(*t) {
		if (*t=='.') {
			++t;
		} else if (*t=='W') {
			ret.pce += WALL;
			++t;
		} else if (*t=='L') {
			ret.hook.lf = 1;
			//ret+=STUCKLEFT;
			//ret.stuck = 1;
			++t;
		} else if (*t=='R') {
			ret.hook.rt = 1;
			//ret+=STUCKRIGHT;
			//ret.stuck = 1;
			++t;
		} else if (*t=='U') {
			ret.hook.up = -1;
			//ret+=STUCKUP;
			//ret.stuck = 1;
			++t;
		} else if (*t=='D') {
			ret.hook.dn = 1;
			//ret+=STUCKDOWN;
			//ret.stuck = 1;
			++t;
		} else if (*t=='S') {
			ret.stuck = 1;
			++t;
		} else {
			//ret+=PIECE;
			int pv = advance_atoi(&t);
			if (pv < PIECESTART)
				errorexit("piece must be 2 or greater");
			ret.pce += pv;
		}
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
	if (lsc.num() != sp + lev.nbx * lev.nby)
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
				if (lev.tiles[j][i].pce != WALL)
					errorexit("missing wall");
}

struct ms {
	pointS8 p;
	pointS8 d;
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
	perf_start(TEST4);
	int hash=o.hashfunc();
	int n=(int)bci[hash].size();
	int i;
	for (i=0;i<n;++i) {
		boardcollinfo& br=bci[hash][i];
		if (o.beq(br.objs)) {
			if (nm>=br.nummoves) {
				perf_end(TEST4);
				return true;
			} else
				errorexit("bad addboard");
		}
	}
	boardcollinfo b(o,nm,prvh,prvi,msa);
	bci[hash].push_back(b);
	++count;
	if (count%1000==0) {
//		video_lock();
//		outtextxybf32(B32,(WX>>1)+100,580-20,C32WHITE,C32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
		drawtextque_format_foreback(SMALLFONT,(WX>>1)+100,580-20,F32WHITE,F32BLACK,"SOLVING... try %3d,count %8d",nm+1,count);
//		video_unlock();
		winproc();
		if (KEY=='a') {
			perf_end(TEST4);
			return false;
		}
		if (wininfo.closerequested) {
			perf_end(TEST4);
			return false;
		}
	}
	perf_end(TEST4);
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

struct pickupall {
	tile col;
	vector<pointS8> tilepos;
};

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
	//bool move1(const pointS8& pos,const pointS8& dir);
	//bool movedir(const pointS8& pos,const pointS8 dir);
	bool movepickup(const vector<pointS8>&); // returns false if not a piece or stuck
	bool movecheck(const vector<pointS8>&,pointS8,vector<pointS8>& collinfo); // check new piece position
	void moveputdown(const vector<pointS8>&,pointS8,tile seedtile); // returns false if piece can't got there
	bool move(const pointS8& pos,pointS8 dir);
	//vector<pickupall> game::movepickupall();
	bool iswinner();
	bool isloser();
	void undo();
	void hint();
	void clearsolvestack();
	int getundossize();
	void addundo(const board& b);
	int gethintssize();
	board getcurpos() const;
//	void selectpiece();
//	pointS8 getcurpos() const {return curpos.cp;}
};

void game::draw(const pointi2& pos) const
{
	perf_start(TEST5);
	C32 BACKGND(C32YELLOW);
	BACKGND.r-=80;
	BACKGND.g-=80;
	BACKGND.b-=80;
	C32 WALLCOL = C32BLACK;
	C32 colors[] = {
		BACKGND,WALLCOL,
		C32RED,C32GREEN,C32BLUE,C32YELLOW,
		C32(32,64,32),
		C32(64,96,64),
		C32(96,128,96),
		C32(128,160,128),
		C32(160,192,160)
	};
	const int MAXCOLORS = sizeof(colors)/sizeof(colors[0]);
	int i,j;
	int si,sj;
	for (j=0;j<curpos.lev.nby;++j) {
		sj=j * SQSIZE + pos.y;
		for (i=0;i<curpos.lev.nbx;++i) {
			si=i * SQSIZE + pos.x;
			tile sq = curpos.lev.tiles[j][i];
			int pn = sq.pce;
			C32 pcol = pn < MAXCOLORS ? colors[pn] : C32BLACK;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,pcol);
			string ststr = "";
/*			if (sq&STUCKLEFT)
				ststr += 'L';
			if (sq&STUCKRIGHT)
				ststr += 'R';
			if (sq&STUCKUP)
				ststr += 'U';
			if (sq&STUCKDOWN)
				ststr += 'D'; */
			if (sq.stuck)
				ststr += 'S';
			if (sq.hook.lf)
				ststr += 'L';
			if (sq.hook.rt)
				ststr += 'R';
			if (sq.hook.up)
				ststr += 'U';
			if (sq.hook.dn)
				ststr += 'D';
			//if (sq.ud == -1)
			//	ststr += 'U';
			//if (sq.ud == 1)
			//	ststr += 'D';
			outtextxybf32(B32,si+6,sj+6,C32WHITE,C32BLACK,"%d%s",pn,ststr.c_str());
		}
	}
	perf_end(TEST5);
}

// move 1 block
/*bool game::move1(const pointS8& pos,const pointS8& dir)
{
	tile& dst = curpos.lev.tiles[pos.y + dir.y][pos.x + dir.x];
	if (dst.pce != BLANK)
		return false;
	dst = src;
	src = blanktile;
	return true;
}
*/

bool game::move(const pointS8& pos,pointS8 newdir)
{
	perf_start(TEST6);
	int i,j,k,m,n;
	vector<pointS8> newpnts; // points that need processing
	newpnts.reserve(20);
	//vector<pointS8> movedpnts; // points that are ready to move

	// simple pre checks
	if (pos.x < 1 || pos.y < 1 || pos.x >= curpos.lev.nbx-1 || pos.y >= curpos.lev.nby-1) {
		perf_end(TEST6);
		return false; // not on the board
	}
	// see if not a movable piece
	tile seedtile = curpos.lev.tiles[pos.y][pos.x];
	if (seedtile.stuck || seedtile.pce < PIECESTART) {
		perf_end(TEST6);
		return false;
	}
	 // clear visited
	for (j=0;j<curpos.lev.nby;++j) {
		for (i=0;i<curpos.lev.nbx;++i) {
			curpos.lev.tiles[j][i].visited = 0;
		}
	}
	vector<piece> movepieces;
	movepieces.reserve(20);
	vector<piece> otherpieces;
	otherpieces.reserve(20);
	// start it off
	newpnts.push_back(pos);
	while(!newpnts.empty()) {
		pointS8 p = newpnts.back();
		newpnts.pop_back();
		seedtile = curpos.lev.tiles[p.y][p.x];
		if (seedtile.visited)
			continue;
		// do some sticky
		vector<ptile> fl = floodfill(curpos.lev.tiles[0],MAXBOARDX,p,true); // points of piece to move
		// mark lev as 'visited'
		n = fl.size();
		for (i=0;i<n;++i) {
			tile& curtile = curpos.lev.tiles[fl[i].loc.y][fl[i].loc.x];
			if (curtile.stuck) {
				perf_end(TEST6);
				return false;
			}
			curtile.visited = 1;
			//movedpnts.push_back(fl[i]);
//			if (!curpos.lev.tiles[fl[i].y][fl[i].x].filled) {
				pointS8 dst;
				dst.x = fl[i].loc.x + newdir.x;
				dst.y = fl[i].loc.y + newdir.y;
				tile dsttile = curpos.lev.tiles[dst.y][dst.x];
				if (dsttile.pce == BLANK)
					continue;
				if (dsttile.pce == WALL) {
					perf_end(TEST6);
					return false;
				}
				if (dsttile.stuck) {
					perf_end(TEST6);
					return false;
				}
				newpnts.push_back(dst);
//			}
		}
		piece pce;
		pce.off = newdir;
		//pce.pce = seedtile.pce;
		pce.tls = fl;
		movepieces.push_back(pce);
	}
	// collect the other pieces now
	for (j=1;j<curpos.lev.nby-1;++j) {
		for (i=1;i<curpos.lev.nbx-1;++i) {
			tile& curtile = curpos.lev.tiles[j][i];
			if (curtile.stuck || curtile.visited || curtile.pce < PIECESTART)
				continue;
			pointS8 ij = {i,j};
			// do some sticky
			vector<ptile> fl = floodfill(curpos.lev.tiles[0],MAXBOARDX,ij,true); // points of piece to move
			// mark lev as 'visited'
			n = fl.size();
			bool isstuck = false;
			for (k=0;k<n;++k) {
				tile& curtile = curpos.lev.tiles[fl[k].loc.y][fl[k].loc.x];
				curtile.visited = 1;
				if (curtile.stuck)
					isstuck = true;
			}
			if (!isstuck) {
				piece pce;
				pce.off = zeropS8;
				//pce.pce = curtile.pce;
				pce.tls = fl;
				otherpieces.push_back(pce);
			}
		}
	}
	// don't forget to move the pieces
	m = movepieces.size();
	// erase old pieces
	for (i=0;i<m;++i) {
		const vector<ptile>& tlss = movepieces[i].tls;
		n = tlss.size();
		for (j=0;j<n;++j) {
			curpos.lev.tiles[tlss[j].loc.y][tlss[j].loc.x] = blanktile;
		}
	}
	// redraw new pieces
	for (i=0;i<m;++i) {
		const vector<ptile>& tlss = movepieces[i].tls;
		n = tlss.size();
		const pointS8& off = movepieces[i].off;
		for (j=0;j<n;++j) {
			tile& dst = curpos.lev.tiles[tlss[j].loc.y+off.y][tlss[j].loc.x+off.x];
			const ptile& src = tlss[j];
			dst.pce = src.pce;
			//dst.ud = src.ud;
			//dst.lr = src.lr;
			dst.hook = src.hook;
		}
	}
	// now do gravity
	vector<piece>& allpieces = movepieces;
	allpieces.insert(allpieces.end(),otherpieces.begin(),otherpieces.end());
	// clear all pieces from the board
	m = allpieces.size();
	for (i=0;i<m;++i) {
		const vector<ptile>& tlss = allpieces[i].tls;
		n = tlss.size();
		const pointS8& off = allpieces[i].off;
		for (j=0;j<n;++j) {
			curpos.lev.tiles[tlss[j].loc.y+off.y][tlss[j].loc.x+off.x] = blanktile;
		}
	}
	// do gravity until nothing moves
	bool dog = true;
	vector<U8> gpieces;
	while(dog) {
		dog = false;
		// do 1 gravity
		gpieces.assign(m,1); // all pieces are initially assumed to fall
		// draw all pieces back to board
	//	while(!pass) {
	//	}
		// run thru all pieces see if they can fall
		m = allpieces.size();
		bool pass = false; // check once
		while(!pass) {
			pass = true;
			for (i=0;i<m;++i) { // run thru all pieces
				if (!gpieces[i]) // we know this one can't fall
					continue;
				// now see if fallen piece can fit
				const vector<ptile>& tlss = allpieces[i].tls;
				n = tlss.size();
				const pointS8& off = allpieces[i].off;
				for (j=0;j<n;++j) {
					if (curpos.lev.tiles[tlss[j].loc.y + off.y + gpieces[i]][tlss[j].loc.x + off.x].pce != BLANK)
						break; // won't fall
				}
				if (j!=n) { // this piece can't fall
					pass = false; // do it all over again
					gpieces[i] = 0; // no can fall
					for (j=0;j<n;++j) { // draw non falling piece
						//curpos.lev.tiles[tlss[j].loc.y+off.y + gpieces[i]][tlss[j].loc.x + off.x].pce = tlss[j].pce;
						tile& dst = curpos.lev.tiles[tlss[j].loc.y+off.y + gpieces[i]][tlss[j].loc.x + off.x];
						const ptile& src = tlss[j];
						dst.pce = src.pce;
						//dst.ud = src.ud;
						//dst.lr = src.lr;
						dst.hook = src.hook;
					}
				} else {
					dog = true; // do more gravity
					//allpieces[i].off.y += 1;
				}
			}
		}
		// convert falling pieces to new offset
		for (i=0;i<m;++i)
			if (gpieces[i])
				allpieces[i].off.y += 1;
		//dog = false;
	}
	// draw all falling pieces
	for (i=0;i<m;++i) { // run thru all pieces
		if (!gpieces[i]) // don't draw non falling piece
			continue;
		const vector<ptile>& tlss = allpieces[i].tls;
		n = tlss.size();
		const pointS8& off = allpieces[i].off;
		for (j=0;j<n;++j) {
			//curpos.lev.tiles[tlss[j].loc.y+off.y /*+ gpieces[i]*/][tlss[j].loc.x + off.x].pce = tlss[j].pce;
			tile& dst = curpos.lev.tiles[tlss[j].loc.y+off.y /*+ gpieces[i]*/][tlss[j].loc.x + off.x];
			const ptile& src = tlss[j];
			dst.pce = src.pce;
			//dst.ud = src.ud;
			dst.hook = src.hook;
		}
	}
	perf_end(TEST6);
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
//	curpos.cp=thehint.p;
	if (move(thehint.p,thehint.d)) {
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
	perf_start(TEST7);
	int i,j,n,k;
	 // clear visited
	for (j=0;j<curpos.lev.nby;++j) {
		for (i=0;i<curpos.lev.nbx;++i) {
			curpos.lev.tiles[j][i].visited = 0;
		}
	}
	// collect the other pieces now
	vector<bool> pv(MAXNUMPIECES,false);
	for (j=1;j<curpos.lev.nby-1;++j) {
		for (i=1;i<curpos.lev.nbx-1;++i) {
			tile& curtile = curpos.lev.tiles[j][i];
			if (curtile.visited || curtile.pce < PIECESTART)
				continue;
			if (pv[curtile.pce]) {
				perf_end(TEST7);
				return false;
			}
			pointS8 ij = {i,j};
			// do some sticky
			vector<ptile> fl = floodfill(curpos.lev.tiles[0],MAXBOARDX,ij,false); // points of piece to move
			// mark lev as 'visited'
			n = fl.size();
			for (k=0;k<n;++k) {
				tile& curtile = curpos.lev.tiles[fl[k].loc.y][fl[k].loc.x];
				curtile.visited = 1;
			}
			pv[curtile.pce] = true;
		}
	}
	perf_end(TEST7);
	return true;
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
	perf_start(TEST8);
#if 1
	static pointS8 dirs[4]={{-1,0},{0,-1},{1,0},{0,1},};
//	pointi2 dum={0,0};
	ms dumms={{0,0},{0,0}};
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
					pointS8 b;
					for (b.y=1;b.y<curpos.lev.nby-1;++b.y) {
						for (b.x=1;b.x<curpos.lev.nbx-1;++b.x) {
							int d;
							for (d=0;d<4;++d) {
								curpos=hr[j].objs;
								//curpos.curpiece=b; // select piece
								if (move(b,dirs[d])) {
									if (isloser())
										continue;
									if (iswinner()) {
										logger("found a winner on move %d,currently at h%d,i%d \n",m,i,j);
										bc->showboards();
	//									stack<pointi2,vector<pointi2> > movesolvestack;
										ms pb;
										pb.d=dirs[d];
										pb.p=b;
										logger("pushing pos %d,%d dir %d,%d on move solve stack\n",pb.p.x,pb.p.y,pb.d.x,pb.d.y);
										movesolvestackr.push(pb);
										int ph=hr[j].prevhidx;
										int pi=hr[j].previidx;
										ms p=hr[j].mdb;
										while(--m) {
											logger("pushing pos %d,%d dir %d,%d\n",p.p.x,p.p.y,p.d.x,p.d.y);
											movesolvestackr.push(p);
											const boardcollinfo& hr2=bc->bci[ph][pi];
											ph=hr2.prevhidx;
											pi=hr2.previidx;
											p=hr2.mdb;
										}
										delete bc; // free up all those board positions
										curpos=bsave;
	//									curpiece=cbs;
										perf_end(TEST8);
										return;
									}
									ms nm;
									nm.d=dirs[d];
									nm.p=b;
									if (!bc->addboard(curpos,m,i,j,nm)) {
										delete bc;
										curpos=bsave;
	//									curpiece=cbs;
										perf_end(TEST8);
										return;
									}
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
#endif
	perf_end(TEST8);
}

/*void game::selectpiece()
{
//	S32 i,x,y;
	S32 mox=MX;
	S32 moy=MY;
	if (mox>=STARTX && moy>=STARTY) {
		mox-=STARTX;
		mox/=SQSIZE;
		moy-=STARTY;
		moy/=SQSIZE;
		curpos.cp.x = mox;
		curpos.cp.y = moy;
		return;
	}
} */

game* gp;
//int nsc;
//char** sc;
script* sc;
int curlevel;

#ifdef NAMESPACE
} // end namespace jelly

using namespace jelly;
#endif

void jellyinit()
{
	logger("sizeof tile = %d\n",sizeof(tile));
	vector<ptile> fl = floodfill(testarr[0],testwid,teststartfill,false);
	S32 i;
	video_setupwindow(800,600);
	pushandsetdir("jelly");
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
}

void jellyproc()
{
	pointS8 dir={0,0};
//	const pointi2 board2pos={STARTX,STARTY+BOARD2Y};
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
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
//	if (wininfo.mleftclicks)
//		gp->selectpiece();
	if (dir.x || dir.y) {
		const board o=gp->getcurpos();
	//	S32 i,x,y;
		S32 mox=MX;
		S32 moy=MY;
		if (MX>=STARTX && MY>=STARTY) {
			mox = (MX - STARTX)/SQSIZE;
			moy = (MY - STARTY)/SQSIZE;
		} else {
			mox = 0; // can't move a wall
			moy = 0;
		}
		pointS8 pos = {mox,moy};
		if (gp->move(pos,dir)) {
			gp->clearsolvestack();
			gp->addundo(o);
		}
	}
}

void jellydraw2d()
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

void jellyexit()
{
	delete gp;
	board::cleanup();
//	freescript(sc,nsc);
	delete sc;
	popdir();
}
