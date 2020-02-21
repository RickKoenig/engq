#define INCLUDE_WINDOWS
#include <m_eng.h>
#include "m_perf.h"

#include "u_states.h"
#include "u_game2.h"
#include "u_s_game2_foxgoose.h"

// played with 2 kings, if you can get 1 squares between them vertically or horizontally, you win
class board_foxgoose : public board {
	static const S32 numfoxmoves = 2;
	static pointi2 foxmoves[numfoxmoves];
	static const S32 numgoosemoves = 4;
	static pointi2 goosemoves[numgoosemoves];
public:
	board_foxgoose(const board& ba) : board(ba) {}
	board_foxgoose(string levelnamea) : board(levelnamea)
	{
		boardtype = "Foxgoose board";
		boardflip = true;
		sym = FLIPX;
	}
	bool checkmove(S32 stidx,const pointi2b8& endpos) const;
	gamestatus getresult() const;
	void draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const;
	void drawsquares(bool ismoving) const;
};

// foxgoose board, goose moves 4 directions one square diagonally, fox can move 2 forward diagonal directions
bool board_foxgoose::checkmove(S32 stidx,const pointi2b8& endpos) const
{
	if (!board::checkmove(stidx,endpos)) // must move non black on the board to empty square and not pass
		return false;
	const piece2& startp = pieces[stidx];
	// can only move 1 square diagonally
	switch(startp.kind) {
	case 'F':
		{
			S32 dax = abs(endpos.x - startp.pos.x);
			S32 dy = endpos.y - startp.pos.y;
			if (dax ==1 && dy == 1)
				return true;
		}
		break;
	case 'G':
		{
			S32 dax = abs(endpos.x - startp.pos.x);
			S32 day = abs(endpos.y - startp.pos.y);
			if (dax ==1 && day == 1)
				return true;
		}
		break;
	default:
		errorexit("not a fox or goose to move, instead it's '%c'",startp.kind);
		break;
	}
	return false;
}

gamestatus board_foxgoose::getresult() const
{
	gamestatus gs = {-1,UNKNOWN};
	// get board into 2d array
	make_whos();
	S32 i,j,m,n = pieces.size();
	// scan for piece
	for (j=0;j<dim.y;++j) {
		for (i=0;i<dim.x;++i) {
			const piece2& w = whos[j][i];
			if (w.who2 != 'W')
				continue;
			if (w.kind == 'F') { // found a fox
				for (m=0;m<numfoxmoves;++m) {
					const pointi2& f = foxmoves[m];
					if (canmoveto_whos(i+f.x,j+f.y)) {
						return gs;
					}
				}
			}
			if (w.kind == 'G') { // found a goose
				for (m=0;m<numgoosemoves;++m) {
					const pointi2& g = goosemoves[m];
					if (canmoveto_whos(i+g.x,j+g.y)) {
						return gs;
					}
				}
			}
		}
	}
	// no moves left
	gs.stat = LOSE;
	gs.movesleft = 0;
#if 0
	S32 squarecountx = abs(pieces[0].pos.x - pieces[1].pos.x);
	S32 squarecounty = abs(pieces[0].pos.y - pieces[1].pos.y);
	if (squarecountx == 0 && squarecounty == 2 || squarecountx == 2 && squarecounty == 0) {
		gs.stat = LOSE; // in foxgoose, lose
		gs.movesleft = 0;
	}
#endif
	return gs;
}

void board_foxgoose::drawsquares(bool ismoving) const
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

void board_foxgoose::draw(S32 mi,const piece2& pp,bool ismoving,const pointi2* mp) const
{
	drawsquares(ismoving);
	drawpieces(mi,pp,ismoving,mp);
}

pointi2 board_foxgoose::foxmoves[numfoxmoves] = {
		{-1, 1},
		{ 1, 1},
};

pointi2 board_foxgoose::goosemoves[numgoosemoves] = {
		{-1, 1},
		{ 1, 1},
		{-1,-1},
		{ 1,-1},
};

#define USENAMESPACE
#ifdef USENAMESPACE
namespace game2_foxgoose {
#endif

/*void board_foxgoose::draw()
{
	outtextxyf32(B32,0,16,C32GREEN,"Foxgoose Board Draw");
}*/

game<board_foxgoose>* agame;
S32 curlevel;
string curlevelname;
script* sc;

#ifdef USENAMESPACE
} // end namespace game2_foxgoose
using namespace game2_foxgoose;
#endif

void game2_foxgooseinit()
{
	video_setupwindow(800,600);
	pushandsetdir("game2_foxgoose");
	sc = game<board_foxgoose>::getlevels();
	S32 i;
	for (i=0;i<sc->num();++i)
		logger("game2 Foxgoose level %2d: '%s'\n",i,(*sc).printidx(i).c_str());
	curlevel = 0;//sc->num() - 1;
	curlevelname = sc->idx(curlevel);
	agame = new game<board_foxgoose>(curlevelname);
	agame->studygames();
}

void game2_foxgooseproc()
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
		agame = new game<board_foxgoose>(curlevelname);
		agame->studygames();
		break;
	case '-':
		delete agame;
		--curlevel;
		if (curlevel < 0)
			curlevel += sc->num();
		curlevelname = sc->idx(curlevel);
		agame = new game<board_foxgoose>(curlevelname);
		agame->studygames();
		break;
	}
	agame->proc();
}

void game2_foxgoosedraw2d()
{
	clipclear32(B32,C32BROWN);
	//outtextxyf32(B32,0,0,C32WHITE,"Game2 Foxgoose, level '%s'",curlevelname.c_str());
	agame->draw();
}

void game2_foxgooseexit()
{
	popdir();
	delete agame;
	agame = 0;
	delete sc;
	sc = 0;
}
