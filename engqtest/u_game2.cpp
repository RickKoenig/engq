#include <m_eng.h>
#include "m_perf.h"
#include "u_game2.h"

piece2::piece2(FILE* fr)
{
#if 0
	who2 = filereadU8(fr);
	kind = filereadU8(fr);
	pos.x = filereadU8(fr);
	pos.y = filereadU8(fr);
#else
	fileread(fr,this,sizeof(*this));
#endif
}

void piece2::write(FILE* fw) const
{
	filewriteU8(fw,who2);
	filewriteU8(fw,kind);
	filewriteU8(fw,pos.x);
	filewriteU8(fw,pos.y);
}

pointi2 board::dim;
string board::boardtype;
enum board::boardmove board::bm;
const pointi2 board::OFF = {100,120};
bool board::boardflip;
board::symmetry board::sym;

board::board(string boardnamea) : nummoves(0)
{
	boardtype = "Generic board, Base class";
	boardflip = false; // board doesn't change after a turn
	sym = NONE; // no symmetry
	bm = MOVE; // move pieces, not plop
	st.stat = UNKNOWN;
	st.movesleft = -1;
	script sc(boardnamea.c_str());
	if (sc.num() < 3)
		errorexit("bad board '%s', too small",boardnamea.c_str());
	dim.x = atoi(sc.idx(0).c_str());
	dim.y = atoi(sc.idx(1).c_str());
	S32 npieces = atoi(sc.idx(2).c_str());
	if (sc.num() != 3 + 3*npieces)
		errorexit("bad board '%s', wrong number of pieces",boardnamea.c_str());
	S32 i;
	for (i=0;i<npieces;++i) {
		C8 o = 'A';
		const string s = sc.idx(3*i + 3);
		C8 pt = s[0];
		if (s.size() == 2) {
			o = s[1];
		}
		piece2 p = piece2(o,
			pt,
			atoi(sc.idx(3+3*i+1).c_str()),
			atoi(sc.idx(3+3*i+2).c_str())
		);
		pieces.push_back(p);
	}
	sort(pieces.begin(),pieces.end());
	logger("sizeof piece2 = %d\n",sizeof(piece2));
}

// assume file handle already open for read
board::board(FILE* fr,bool getstatnext) : nummoves(0)
{
	//dim.x = filereadU32LE(fr);
	fileread(fr,&dim.x,sizeof(dim.x));
	//dim.y = filereadU32LE(fr);
	fileread(fr,&dim.y,sizeof(dim.y));
	U32 i,ps;//= filereadU32LE(fr);
	fileread(fr,&ps,sizeof(ps));
	for (i=0;i<ps;++i) {
		piece2 p(fr);
		pieces.push_back(p);
	}
	if (getstatnext) {
		//nummoves = filereadU32LE(fr); // from start position
		fileread(fr,&nummoves,sizeof(nummoves));
		//st.movesleft = filereadU32LE(fr); // to final destination
		fileread(fr,&st.movesleft,sizeof(st.movesleft));
		//st.stat = (status)filereadU32LE(fr);
		fileread(fr,&st.stat,sizeof(st.stat));
		U32 i,ns;// = filereadU32LE(fr);
		fileread(fr,&ns,sizeof(ns));
		for (i=0;i<ns;++i) {
			U32 n;
			fileread(fr,&n,sizeof(n));
			next.push_back(n);
			//next.push_back(filereadU32LE(fr));
		}
	}
	::sort(pieces.begin(),pieces.end());
}

// assume file handle already open for write
void board::write(FILE* fw,bool putstatnext) const
{
	//filewriteU32LE(fw,dim.x);
	filewrite(fw,&dim.x,sizeof(dim.x));
	//filewriteU32LE(fw,dim.y);
	filewrite(fw,&dim.y,sizeof(dim.y));
	U32 i,ps = pieces.size();
	//filewriteU32LE(fw,ps);
	filewrite(fw,&ps,sizeof(ps));
	for (i=0;i<ps;++i) {
		const piece2& p = pieces[i];
		p.write(fw);
	}
	if (putstatnext) {
		//filewriteU32LE(fw,nummoves); // from start position
		filewrite(fw,&nummoves,sizeof(nummoves));
		//filewriteU32LE(fw,st.movesleft); // to final destination
		filewrite(fw,&st.movesleft,sizeof(st.movesleft));
		//filewriteU32LE(fw,st.stat);
		filewrite(fw,&st.stat,sizeof(st.stat));
		U32 i,ns = next.size();
		//filewriteU32LE(fw,ns);
		filewrite(fw,&ns,sizeof(ns));
		for (i=0;i<ns;++i) {
			//filewriteU32LE(fw,next[i]);
			filewrite(fw,&next[i],sizeof(next[i]));
		}
	}
}

C8* board::predictm[STATUSNUM]={"LOSING","DRAWING","UNKNOWN","WINNING"};
C8* board::predictma[STATUSNUM]={"L","D","U","W"};
C8* board::predictmar[STATUSNUM]={"W","D","U","L"};
C8* board::resultm[STATUSNUM]={"LOSES","DRAWS","UNKNOWN","WINS"};

bool board::operator<(const board& rhs) const
{
	bool ret = pieces < rhs.pieces;
	return ret;
}

bool board::operator==(const board& rhs) const
{
	perf_start(TEST6);
	// basic check
	bool ret = pieces == rhs.pieces;
	if (ret) {
		perf_end(TEST6);
		return true;
	}
	perf_end(TEST6);
	return false;
}

U32 board::gethash() const
{
	//return 0;
	U32 i,n = min(pieces.size(),4U);
	U32 h = 0;
	for (i=0;i<n;++i) {
		const piece2& p = pieces[i];
		h = 79*h + p.pos.x + 1137*p.pos.y + p.who2 + p.kind;
	}
	return h & (HASHSIZE-1);
}

gamestatus board::getresult() const
{
	gamestatus gs = {-1,UNKNOWN};
	return gs;
}

gamestatus board::getfuture() const
{
	return st;
}

pointi2b8 board::getpiecepos(S32 pidx) const
{
	return pieces[pidx].pos;
}

// find closest pieceidx to mouse, -1 if none
S32 board::findcloseidx(bool flip) const
{
	S32 n = pieces.size();
	S32 i;
	S32 bi = -1;
	S32 bd2 = 1000000; // BIG
	S32 d2;
	S32 delx,dely;
	for (i=0;i<n;++i) {
		S32 x,y;
		const piece2& pi = pieces[i];
		switch(pi.who2) {
		case 'B': // can't move black
			continue;
			break;
		default:
//		case 'W':
//		case piece::NONE:
			x = pi.pos.x;
			y = pi.pos.y;
			if (flip) {
				x = dim.x - 1 - x;
				y = dim.y - 1 - y;
			}
			x = board::OFF.x + board::SQSIZE*x + board::SQSIZE/2;
			y = board::OFF.y + board::SQSIZE*(dim.y - y - 1) + board::SQSIZE/2;
			delx = x - MX;
			dely = y - MY;
			d2 = delx*delx + dely*dely;
			S32 td = 7*SQSIZE/16;
			S32 td2 = td*td;
			if (d2 <= td2 && d2 < bd2) {
				bd2 = d2;
				bi = i;
			}
			break;
		}
	}
	return bi;
}

// return closest square to mouse in 'pos', return false if none
bool board::findcloseemptypos(pointi2b8* pos,bool f) const
{
	// too far left
	if (MX < board::OFF.x || MY < board::OFF.y)
		return false;
	// too far up
	S32 modx = (MX - board::OFF.x)%board::SQSIZE;
	// too close to left or right
	if (modx < board::SQSIZE/8 || modx >= board::SQSIZE*7/8)
		return false;
	S32 mody = (MY - board::OFF.y)%board::SQSIZE;
	// too close to top and bottom
	if (mody < board::SQSIZE/8 || mody >= board::SQSIZE*7/8)
		return false;
	S32 x = (MX - board::OFF.x)/board::SQSIZE;
	// too far right
	if (x >= board::dim.x)
		return false;
	S32 y = (MY - board::OFF.y)/board::SQSIZE;
	// too far left
	if (y >= board::dim.y)
		return false;
	// y starts at bottom
	y = board::dim.y - y - 1;
	if (f) {
		x = dim.x - 1 - x;
		y = dim.y - 1 - y;
	}
	pos->x = x;
	pos->y = y;
	return true;
}

// generic board allows all moves to an empty square, and must move
bool board::checkmove(S32 stidx,const pointi2b8& endpos) const
{
	if (stidx != NEWPIECEIDX && pieces[stidx].who2 == 'B') // cannot move black pieces
		return false;
	S32 i,n = pieces.size();
	// see if new position is empty
	for (i=0;i<n;++i) {
		const piece2& pi = pieces[i];
		if (pi.pos == endpos)
			return false; // not empty
	}
	return true;
}

void board::movepiece(S32 stidx,const pointi2b8& endpos)
{
	pieces[stidx].pos = endpos;
	sort(pieces.begin(),pieces.end());
}

pointi2 board::board2screen(const pointi2b8& pos)
{
	pointi2 ret;
	ret.x = OFF.x + SQSIZE*pos.x;
	ret.y = OFF.y + SQSIZE*(dim.y - pos.y - 1);
	return ret;
}

// flip piece color during a turn change
C8 board::flipcolor(C8 c)
{
	switch(c) {
	case 'W':
		c = 'B';
		break;
	case 'B':
		c = 'W';
		break;
	}
	return c;
}

// color change and optional 180
void board::flipturn()
{
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) {
		piece2& p = pieces[i];
		C8& o = p.who2;
		o = flipcolor(o);
		if (boardflip) {
			p.pos.x = dim.x - 1 - p.pos.x;
			p.pos.y = dim.y - 1 - p.pos.y;
		}
	}
}

// 90 degrees clockwise
// assume dim.x == dim.y
void board::rot90()
{
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) {
		piece2& p = pieces[i];
		S8 t = p.pos.x;
		p.pos.x = p.pos.y;
		p.pos.y = dim.x - 1 - t;
	}
}

// left and right
void board::flipx()
{
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) {
		piece2& p = pieces[i];
		p.pos.x = dim.x - 1 - p.pos.x;
	}
}

piece2 board::getnewpiece() const
{
	return piece2('W','.',-1,-1);
}

void board::drawsquares(bool ismoving) const
{
	S32 i,j;
	// draw the board
	C32 rc = ismoving ? C32GREEN : C32YELLOW;
	for (j=0;j<dim.y;++j) {
		S32 sj = SQSIZE*j + OFF.y;
		for (i=0;i<dim.x;++i) {
			S32 si = SQSIZE*i + OFF.x;
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,rc);
		}
	}
}

void board::drawpieces(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const
{
	//const S32 SQSIZE = 32;
	//outtextxyf32(B32,0,16,C32GREEN,"Generic Board Draw");
	// draw the pieces
	S32 i,n = pieces.size();
	if (ismoving) // 1 more piece for moving
		++n;
	C32 cbcol = C32(100,130,160);
	for (i=0;i<n;++i) {
		C32 cf;
		 //pi;
		piece2 pi;
		if (ismoving && i == n-1) // this is the moving piece
			pi = pp;
		else
			pi = pieces[i];
		switch(pi.who2) {
		case 'W':
			cf = C32WHITE;
			break;
		case 'B':
			cf = C32BLACK;
			break;
		default:
			cf = C32BLUE;
			break;
		}
		S32 x,y;
		if (ismoving && i == n-1) { // this is the moving piece
			x = mp->x;
			y = mp->y;
		} else {
			if (mi != i) { // not the selected piece, draw normally
				x = OFF.x + SQSIZE*pi.pos.x;
				y = OFF.y + SQSIZE*(dim.y - pi.pos.y - 1);
			} else {
				continue; // handled by moving piece
			}
		}
		bitmap32* tiny = bitmap32alloc(8,8,C32BLACK);
		bitmap32* big = bitmap32alloc(SQSIZE,SQSIZE,C32BLACK);
		outtextxybf32(tiny,0,0,cf,cbcol,"%c",pi.kind);
		clipscaleblit32(tiny,big);
		outtextxybf32(big,0,0,C32WHITE,C32BLACK,"%d",i);
		clipblit32(big,B32,0,0,x,y,big->size.x,big->size.y);
		bitmap32free(tiny);
		bitmap32free(big);
	}
}

void board::draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const
{
	drawsquares(ismoving);
	drawpieces(mi,pp,ismoving,mp);
}

void board::reset_whos() const
{
	if (whos.size() != dim.y || whos[0].size() != dim.x) {
		vector<piece2> arow(dim.x,piece2());
		whos.assign(dim.y,arow);
	}
	S32 i,j;
	for (j=0;j<dim.y;++j)
		for (i=0;i<dim.x;++i)
			whos[j][i] = ' ';
}

void board::make_whos() const
{
	reset_whos();
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) {
		const piece2& p = pieces[i];
		whos[p.pos.y][p.pos.x] = p;
	}
}

// non capturing moves
bool board::canmoveto_whos(S32 x,S32 y) const
{
	if (x<0 || y<0 || x >= dim.x || y >= dim.y) 
		return false;
	return whos[y][x].who2 == ' ';
}

vector<vector<piece2> > board::whos;
