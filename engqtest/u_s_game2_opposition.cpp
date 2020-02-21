#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include "u_states.h"
#include "u_game2.h"
#include "u_s_game2_opposition.h"

// played with 2 kings, if you can get 1 squares between them vertically or horizontally, you win
class board_opposition : public board {
public:
	board_opposition(const board& ba) : board(ba) {}
	board_opposition(string levelnamea) : board(levelnamea)
	{
		boardtype = "Opposition board";
		boardflip = true;
		sym = ROTFLIP; // Z4 * Z2
		S32 n = pieces.size();
		if (n != 2)
			errorexit("bad board, wrong number of pieces");
		if (pieces[0].who2 != 'W' || pieces[0].kind != 'K')
			errorexit("piece 0 not white or a king");
		if (pieces[1].who2 != 'B' || pieces[1].kind != 'K')
			errorexit("piece 1 not black or a king");
	}
	bool checkmove(S32 stidx,const pointi2b8& endpos) const;
	gamestatus getresult() const;
	void draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const;
	void drawsquares(bool ismoving) const;
};

// opposition board allows all moves to an adjacent empty square, but not next to other piece
bool board_opposition::checkmove(S32 stidx,const pointi2b8& endpos) const
{
	if (!board::checkmove(stidx,endpos)) // must move on the board to empty square and not pass
		return false;
	// other king
	S32 ok = 1 - stidx;
	// must be far enough away from other king
	const pointi2b8& otherpos = pieces[ok].pos;
	S32 squarecountx = abs(endpos.x - otherpos.x);
	S32 squarecounty = abs(endpos.y - otherpos.y);
	if (squarecountx < 2 && squarecounty < 2)
		return false;
	// can only move 1 square
	const pointi2b8& oldpos = pieces[stidx].pos;
	squarecountx = abs(endpos.x - oldpos.x);
	squarecounty = abs(endpos.y - oldpos.y);
	if (squarecountx == 1 && squarecounty == 1)
		return true;
	if (squarecountx == 0 && squarecounty == 1)
		return true;
	if (squarecountx == 1 && squarecounty == 0)
		return true;
	return false;
}

gamestatus board_opposition::getresult() const
{
	gamestatus gs = {-1,UNKNOWN};
	S32 squarecountx = abs(pieces[0].pos.x - pieces[1].pos.x);
	S32 squarecounty = abs(pieces[0].pos.y - pieces[1].pos.y);
	if (squarecountx == 0 && squarecounty == 2 || squarecountx == 2 && squarecounty == 0) {
		gs.stat = LOSE; // in opposition, lose
		gs.movesleft = 0;
	}
	return gs;
}

void board_opposition::drawsquares(bool ismoving) const
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

void board_opposition::draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const
{
	drawsquares(ismoving);
	drawpieces(mi,pp,ismoving,mp);
}
#define USENAMESPACE
#ifdef USENAMESPACE
namespace game2_opposition {
#endif

/*void board_opposition::draw()
{
	outtextxyf32(B32,0,16,C32GREEN,"opposition Board Draw");
}*/

game<board_opposition>* agame;
S32 curlevel;
string curlevelname;
script* sc;

#ifdef USENAMESPACE
} // end namespace game2_opposition
using namespace game2_opposition;
#endif

void game2_oppositioninit()
{
	video_setupwindow(800,600);
	pushandsetdir("game2_opposition");
	sc = game<board_opposition>::getlevels();
	S32 i;
	for (i=0;i<sc->num();++i)
		logger("game2 Opposition level %2d: '%s'\n",i,(*sc).printidx(i).c_str());
	curlevel = 1;//sc->num() - 1;
	curlevelname = sc->idx(curlevel);
	agame = new game<board_opposition>(curlevelname);
	agame->studygames();
}

void game2_oppositionproc()
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
		agame = new game<board_opposition>(curlevelname);
		agame->studygames();
		break;
	case '-':
		delete agame;
		--curlevel;
		if (curlevel < 0)
			curlevel += sc->num();
		curlevelname = sc->idx(curlevel);
		agame = new game<board_opposition>(curlevelname);
		agame->studygames();
		break;
	}
	agame->proc();
}

void game2_oppositiondraw2d()
{
	clipclear32(B32,C32BROWN);
	//outtextxyf32(B32,0,0,C32WHITE,"Game2 opposition, level '%s'",curlevelname.c_str());
	agame->draw();
}

void game2_oppositionexit()
{
	popdir();
	delete agame;
	agame = 0;
	delete sc;
	sc = 0;
}
