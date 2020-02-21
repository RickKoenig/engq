#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include "u_states.h"
#include "u_game2.h"
#include "u_s_game2_endgame.h"

class board_nim : public board {
public:
	board_nim(const board& ba) : board(ba) {}
	board_nim(string levelnamea) : board(levelnamea)
	{
		boardtype = "Nim board";
		sym = FLIPX;
	}
	bool checkmove(S32 stidx,const pointi2b8& endpos) const;
	gamestatus getresult() const;
};

gamestatus board_nim::getresult() const
{
	gamestatus gs = {-1,UNKNOWN};
#if 1
	S32 i,n = pieces.size();
	for (i=0;i<n;++i) {
		const piece2& pi = pieces[i];
		if (pi.pos.y != 0)
			return gs; // at least one piece can still move
	}
	gs.stat = LOSE; // can't move anymore, lose
	gs.movesleft = 0;
#endif
	return gs;
}

// nim checkmove adds more restrictions to movement
bool board_nim::checkmove(S32 stidx,const pointi2b8& endpos) const
{
	if (!board::checkmove(stidx,endpos)) // must move on the board to empty square and not pass
		return false;
	pointi2b8 stpos = pieces[stidx].pos;
	// for now allow movement upto 3 squares down
	S32 del = endpos.y - stpos.y;
	return stpos.x == endpos.x && del < 0 && del >= -3;
}

//#define USENAMESPACE
#ifdef USENAMESPACE
namespace game2_nim {
#endif

game<board_nim>* agame;
S32 curlevel;
string curlevelname;
script* sc;

#ifdef USENAMESPACE
} // end namespace game2_nim
using namespace game2_nim;
#endif

void game2_niminit()
{
	video_setupwindow(800,600);
	pushandsetdir("game2_nim");
	sc = game<board_nim>::getlevels();
	S32 i;
	for (i=0;i<sc->num();++i)
		logger("game2 nim level %2d: '%s'\n",i,(*sc).printidx(i).c_str());
	curlevel=0;
	curlevelname = sc->idx(curlevel);
	agame = new game<board_nim>(curlevelname);
	agame->studygames();
}

void game2_nimproc()
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
		agame = new game<board_nim>(curlevelname);
		agame->studygames();
		break;
	case '-':
		delete agame;
		--curlevel;
		if (curlevel < 0)
			curlevel += sc->num();
		curlevelname = sc->idx(curlevel);
		agame = new game<board_nim>(curlevelname);
		agame->studygames();
		break;
	}
	agame->proc();
}

void game2_nimdraw2d()
{
	clipclear32(B32,C32BROWN);
	agame->draw();
}

void game2_nimexit()
{
	popdir();
	delete agame;
	agame = 0;
	delete sc;
	sc = 0;
}
