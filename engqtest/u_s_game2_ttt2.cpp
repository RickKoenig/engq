#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include "u_states.h"
#include "u_game2.h"
#include "u_s_game2_ttt2.h"

// try to get 2 turn ttt 4 by 4 board working, it's hard!

class board_ttt2 : public board {
public:
	board_ttt2(const board& ba) : board(ba) {}
	board_ttt2(string levelnamea) : board(levelnamea)
	{
		boardtype = "TicTacToe board";
		//sym = FLIPX;
		sym = ROTFLIP;
		//sym = ROT180;
		bm = PLOP;
		if (dim.x != dim.y)
			errorexit("TTT must have a square board");
	}
	//bool move(const pointi2& start,const pointi2 end);
	//void draw();

	gamestatus getresult() const;
	S32 findcloseidx(bool f) const;
	void movepiece(S32 stidx,const pointi2b8& endpos);
};

gamestatus board_ttt2::getresult() const
{
	gamestatus gs = {-1,UNKNOWN};
	// get board into 2d array
	make_whos();
	// check for losing position
	S32 i,j;
	gs.stat = LOSE;
	gs.movesleft = 0;
	// white to play, see if black already won
	// across
	for (j=0;j<dim.y;++j) {
		for (i=0;i<dim.x;++i) {
			if (whos[j][i].who2 != 'B') {
				break; // not this row
			}
		}
		if (i == dim.x) {
			return gs; // a row is all black, you lost
		}
	}
	// down
	for (i=0;i<dim.x;++i) {
		for (j=0;j<dim.y;++j) {
			if (whos[j][i].who2 != 'B') {
				break; // not this column
			}
		}
		if (j == dim.y) {
			return gs; // a column is all black, you lost
		}
	}
	// diagonals
	for (i=0;i<dim.x;++i) {
		if (whos[i][i].who2 != 'B') {
			break; // not this row
		}
	}
	if (i == dim.x) {
		return gs; // a row is all black, you lost
	}
	for (i=0;i<dim.x;++i) {
		if (whos[i][dim.x - 1 - i].who2 != 'B') {
			break; // not this row
		}
	}
	if (i == dim.x) {
		return gs; // a row is all black, you lost
	}
#if 1
	// check for cats
	S32 n = pieces.size();
	if (n == dim.x*dim.y) {
		gs.stat = DRAW;
		gs.movesleft = 0;
		return gs;
	}
#endif
	// nothing left
	gs.stat = UNKNOWN;
	gs.movesleft = -1;
	return gs;
}

// return new piece
S32 board_ttt2::findcloseidx(bool f) const
{
	return NEWPIECEIDX;
}

void board_ttt2::movepiece(S32 stidx,const pointi2b8& endpos)
{
	piece2 np('W','*',endpos.x,endpos.y);
	pieces.push_back(np);
	sort(pieces.begin(),pieces.end());
	//flipturn();
}

#define USENAMESPACE
#ifdef USENAMESPACE
namespace game2_ttt2 {
#endif


game<board_ttt2>* agame;
S32 curlevel;
string curlevelname;
script* sc;

#ifdef USENAMESPACE
} // end namespace game2_ttt2
using namespace game2_ttt2;
#endif

void game2_ttt2init()
{
	video_setupwindow(800,600);
	pushandsetdir("game2_ttt2");
	sc = game<board_ttt2>::getlevels();
	S32 i;
	for (i=0;i<sc->num();++i)
		logger("game2 ttt level %2d: '%s'\n",i,(*sc).printidx(i).c_str());
	//curlevel = sc->num() - 1;
	curlevel = 1;
	curlevelname = sc->idx(curlevel);
	agame = new game<board_ttt2>(curlevelname);
	agame->studygames(false,false);
}

void game2_ttt2proc()
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
		agame = new game<board_ttt2>(curlevelname);
		agame->studygames(true,false);
		break;
	case '-':
		delete agame;
		--curlevel;
		if (curlevel < 0)
			curlevel += sc->num();
		curlevelname = sc->idx(curlevel);
		agame = new game<board_ttt2>(curlevelname);
		agame->studygames(true,false);
		break;
	}
	agame->proc();
}

void game2_ttt2draw2d()
{
	clipclear32(B32,C32BROWN);
	//outtextxyf32(B32,0,0,C32WHITE,"Game2 Endgame, level '%s'",curlevelname.c_str());
	agame->draw();
}

void game2_ttt2exit()
{
	popdir();
	delete agame;
	agame = 0;
	delete sc;
	sc = 0;
}
