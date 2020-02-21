#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include "u_states.h"
#include "u_game2.h"
#include "u_s_game2_endgame.h"

class board_endgame : public board {
public:
	board_endgame(const board& ba) : board(ba) {}
	board_endgame(string levelnamea) : board(levelnamea)
	{
		boardtype = "Endgame board";
		boardflip = true;
		sym = ROTFLIP; // Z4 * Z2, 8 ways
		S32 i,n = pieces.size();
		for (i=0;i<n;++i) {
			if (pieces[i].kind == 'P') {
				sym = FLIPX; // pawns have no 'y' symmetry, 2 ways
				break;
			}
		}
	}
	// can capture kings
	gamestatus getresult1() const;
	bool checkmove1(S32 stidx,const pointi2b8& endpos) const;
	// can do check, checkmate, and stalemate
	gamestatus getresult() const;
	bool checkmove(S32 stidx,const pointi2b8& endpos) const;
	// move or capture
	void movepiece(S32 stidx,const pointi2b8x& endpos);
	bool incheck() const;
	bool isjump(S32 stidx,const pointi2b8& endpos) const; // did we just jump over a piece to move ?
	void draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const;
	void drawsquares(bool ismoving) const;
};

// does this board put you in check?
bool board_endgame::incheck() const
{
	board_endgame nb = *this;
	nb.flipturn();
	// black to play on this board
	U32 p,np = pieces.size();
	pointi2b8 e;
	for (p=0;p<np;++p) { // pick a black piece, (it's really white flipped)
		for (e.y=0;e.y<board::dim.y;++e.y) {
			for (e.x=0;e.x<board::dim.x;++e.x) {
				if (nb.checkmove1(p,e)) { // see if it can be moved
					nb.movepiece(p,e);
					nb.flipturn();
					gamestatus gs = nb.getresult1(); // back to white, any kings left
					if (gs.stat == LOSE && gs.movesleft == 0)
						return true;
					nb = *this; // put it back
					nb.flipturn();
				}
			}
		}
	}
	return false;
}

// are you jumping over a piece to make a move?
bool board_endgame::isjump(S32 stidx,const pointi2b8& endpos) const
{
	const pointi2b8& stpos = pieces[stidx].pos;
	if (stpos.y == endpos.y) { // horizontal
		U32 p,np = pieces.size();
		for (p=0;p<np;++p) {
			if (p == stidx)
				continue;
			const pointi2b8& cpos = pieces[p].pos;
			if (cpos.y == stpos.y && ((cpos.x - stpos.x) * (cpos.x - endpos.x) < 0))
				return true;
		}
	} else if (stpos.x == endpos.x) { // vertical
		U32 p,np = pieces.size();
		for (p=0;p<np;++p) {
			if (p == stidx)
				continue;
			const pointi2b8& cpos = pieces[p].pos;
			if (cpos.x == stpos.x && ((cpos.y - stpos.y) * (cpos.y - endpos.y) < 0))
				return true;
		}
	} else { // diagonally
		U32 p,np = pieces.size();
		for (p=0;p<np;++p) {
			if (p == stidx)
				continue;
			const pointi2b8& cpos = pieces[p].pos;
			S32 x0 = cpos.x - stpos.x;
			S32 x1 = cpos.x - endpos.x;
			if (x0*x1 >= 0)
				continue;
			S32 y0 = cpos.y - stpos.y;
			S32 y1 = cpos.y - endpos.y;
			if (x0*y1 == x1*y0)
				return true;
		}
	}
	return false;
}

gamestatus board_endgame::getresult() const
{
	gamestatus gs = {-1,UNKNOWN};
	U32 p,np = pieces.size();
#if 1
	if (np == 2) {
		gs.movesleft = 0;
		gs.stat = DRAW;
		return gs;
	}
	if (np == 3) {
		U32 i;
		for (i=0;i<np;++i) {
			if (pieces[i].kind == 'B' || pieces[i].kind == 'N') {
				gs.movesleft = 0;
				gs.stat = DRAW;
				return gs;
			}
		}
	}
#endif
	board_endgame nb = *this;
	// white to play on this board
	pointi2b8 e;
	for (p=0;p<np;++p) { // pick a white piece
		for (e.y=0;e.y<board::dim.y;++e.y) {
			for (e.x=0;e.x<board::dim.x;++e.x) {
				if (nb.checkmove1(p,e)) { // see if it can be moved
					nb.movepiece(p,e);
					if (!nb.incheck())
						return gs;
					nb = *this; // put it back
				}
			}
		}
	}
	// legal moves game must end
	gs.movesleft = 0;
	gs.stat = nb.incheck() ? LOSE : DRAW; // checkmate or stalemate
	return gs;
}

// chess move
bool board_endgame::checkmove(S32 stidx,const pointi2b8& endpos) const
{
	 if (!checkmove1(stidx,endpos))
		 return false;
	board_endgame bc = *this;
	bc.movepiece(stidx,endpos);
	return !bc.incheck(); // does this move put me in check, if so then it's an invalid move
}

gamestatus board_endgame::getresult1() const
{
	gamestatus gs = {-1,UNKNOWN};
	// check for any white king piece left
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) {
		const piece2& p = pieces[i];
		if (p.who2 == 'W' && p.kind == 'K') {
			return gs;
		}
	}
	// can't move, for now must be a LOSE
	gs.movesleft = 0;
	gs.stat = LOSE;
	return gs;
}

// chess move1
// can only move if white piece has legal move and dest square is non white
bool board_endgame::checkmove1(S32 stidx,const pointi2b8& endpos) const
{
	if (pieces[stidx].who2 == 'B') // cannot move black pieces
		return false;
	S32 i,n = pieces.size();
	// see if new position is empty
	bool capture = false;
	for (i=0;i<n;++i) {
		const piece2& pi = pieces[i];
		if (pi.pos == endpos) {
			if (pi.who2 == 'W') {
				return false; // not empty and white
			} else if (pi.who2 == 'B') {
				capture = true; // this move will capture a piece
				break;
			}
		}
	}
	const pointi2b8& oldpos = pieces[stidx].pos;
	S32 scx = endpos.x - oldpos.x;
	S32 scy = endpos.y - oldpos.y;
	S32 scax = abs(scx);
	S32 scay = abs(scy);
	switch(pieces[stidx].kind) {
	case 'Q':
		if (scax && scax == scay) // like a bishop
			return !isjump(stidx,endpos);
		if (scax == 0 || scay == 0) // like a rook
			return !isjump(stidx,endpos);
		break;
	case 'B':
		if (scax && scax == scay)
			return !isjump(stidx,endpos);
		break;
	case 'R':
		if (scax == 0 || scay == 0)
			return !isjump(stidx,endpos);
		break;
	// these don't need isjump since they only move 1 square
	case 'N':
		if ((scax == 1 && scay == 2) || (scax == 2 && scay == 1))
			return true;
		break;
	case 'P':
		if (capture) {
			if (scax == 1 && scy == 1) // diagonal capture
				return true;
		} else {
			if (scx == 0) {
				if (scy == 1) { // 1 square forward
					return true;
				}
				if (scy == 2 && oldpos.y == 1) { // 2 squares forward from start
					return !isjump(stidx,endpos);
				}
			}
		}
		break;
	case 'K':
		if (scax == 1 && scay == 1)
			return true;
		if (scax == 0 && scay == 1)
			return true;
		if (scax == 1 && scay == 0)
			return true;
		break;
	default:
		errorexit("bad piece to move '%c'",pieces[stidx].kind);
	}
	return false;
}

void board_endgame::movepiece(S32 stidx,const pointi2b8x& endpos)
{
	pieces[stidx].pos = endpos; // move the piece
	if (endpos.y == 7 && pieces[stidx].kind == 'P') // pawn promotion
		pieces[stidx].kind = 'Q';
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) { // any other pieces in this position?
		if (i == stidx)
			continue;
		if (endpos == pieces[i].pos) // find a piece in the dest move piece location
			break;
	}
	if (i != n) { // doing capture, good for chess like games
		pieces.erase(pieces.begin() + i);
	}
	sort(pieces.begin(),pieces.end()); // canonical
}

void board_endgame::drawsquares(bool ismoving) const
{
	S32 i,j;
	// draw the board
	for (j=0;j<dim.y;++j) {
		S32 sj = SQSIZE*j + OFF.y;
		for (i=0;i<dim.x;++i) {
			S32 si = SQSIZE*i + OFF.x;
			C32 rc = ismoving ? C32GREEN : C32YELLOW;
			if (1&(i+j)) {
				rc.r = rc.r * 3 / 4;
				rc.g = rc.g * 3 / 4;
				rc.b = rc.b * 3 / 4;
			}
			cliprect32(B32,si,sj,SQSIZE-1,SQSIZE-1,rc);
		}
	}
}

void board_endgame::draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const
{
	drawsquares(ismoving);
	drawpieces(mi,pp,ismoving,mp);
}


#define USENAMESPACE
#ifdef USENAMESPACE
namespace game2_endgame {
#endif

game<board_endgame>* agame;
S32 curlevel;
string curlevelname;
script* sc;

//#define TESTBIGMEM
#ifdef TESTBIGMEM
U8* bigmemtest;
U8* bigmemtest2;
U32 BIGMEMSIZE = 2800000000U;
//U8 bigmemtest[BIGMEMSIZE];
#endif

#ifdef USENAMESPACE
} // end namespace game2_endgame
using namespace game2_endgame;
#endif

void game2_endgameinit()
{
	logger("sizeof piece2 = %d\n",sizeof (piece2));
	logger("sizeof piece2c = %d\n",sizeof (piece2c));
#ifdef TESTBIGMEM
	bigmemtest = new U8[BIGMEMSIZE/2];
	bigmemtest2 = new U8[BIGMEMSIZE/2];
	U32 j;
	for (j=0;j!=BIGMEMSIZE/2;++j)
		bigmemtest[j] = 11*j;
	for (j=0;j!=BIGMEMSIZE/2;++j)
		bigmemtest2[j] = 11*j;
	//bigmemtest = (U8*)HeapAlloc(GetProcessHeap(),0,BIGMEMSIZE);
	//bigmemtest = malloc(BIGMEMSIZE);
	//bigmemtest = new U8[BIGMEMSIZE/2];
	//bigmemtest2 = new U8[BIGMEMSIZE/2];
	//if (!bigmemtest)
	//	errorexit("malloc failed");
	//free(bigmemtest);
#endif
	video_setupwindow(800,600);
	pushandsetdir("game2_endgame");
	sc = game<board_endgame>::getlevels();
	S32 i;
	for (i=0;i<sc->num();++i)
		logger("game2 endgame level %2d: '%s'\n",i,(*sc).printidx(i).c_str());
	curlevel = 11;//sc->num() - 1;
	curlevelname = sc->idx(curlevel);
	agame = new game<board_endgame>(curlevelname);
	agame->studygames();
}

void game2_endgameproc()
{
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	case '=':
		delete agame;
		++curlevel;
		if (curlevel >= sc->num())
			curlevel -= sc->num();
		curlevelname = sc->idx(curlevel);
		agame = new game<board_endgame>(curlevelname);
		agame->studygames();
		break;
	case '-':
		delete agame;
		--curlevel;
		if (curlevel < 0)
			curlevel += sc->num();
		curlevelname = sc->idx(curlevel);
		agame = new game<board_endgame>(curlevelname);
		agame->studygames();
		break;
	}
	agame->proc();
}

void game2_endgamedraw2d()
{
	clipclear32(B32,C32BROWN);
	//outtextxyf32(B32,0,0,C32WHITE,"Game2 Endgame, level '%s'",curlevelname.c_str());
	agame->draw();
}

void game2_endgameexit()
{
	popdir();
	delete agame;
	agame = 0;
	delete sc;
	sc = 0;
#ifdef TESTBIGMEM
	delete[] bigmemtest;
	delete[] bigmemtest2;
#endif
}
