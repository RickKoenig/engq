#include <m_eng.h>
#include "u_states.h"

namespace ttt {

con32 *acon; // console output

enum gamestate {WHOFIRST,TURN_X,TURN_O};
enum piece {P_X,P_BLANK,P_O}; // P_X, blank, P_O
enum status {XWIN,OPEN,OWIN,CATS};
enum status2 {WIN,OPEN2,LOSE,CATS2};

S32 firstmove=0; // computers first move 0-8
//piece board[9]; // 0 'P_X',  1 ' ',  2 'P_O'
//gamestate gs;

struct sboard {
	piece mboard[9]; // board
	gamestate mgs; // who turn is it to play
	status ms; // 
	status2 ms2;
};

sboard aboard;
// clear board to all 1's
void reset(gamestate whofirst)
{
	S32 i;
	for (i=0;i<9;++i)
		aboard.mboard[i] = P_BLANK;
	aboard.mgs = whofirst;
	aboard.ms = OPEN;
}

// only one square left return 1-9 else 0, (good for forcing the last turn)
S32 almost()
{
	S32 i,j,k=0;
	j=0;
	for (i=0;i<9;i++)
		if (aboard.mboard[i]==P_BLANK) {
			j++;
			k=i+1;
		}
	if (j==1) // one square?
		return k;
	return 0;
}

// X just moved
// see if P_X wins, returns XWINS,OPEN or CATS
status positX()
{
	S32 v;
	v=aboard.mboard[0]+aboard.mboard[1]+aboard.mboard[2];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[3]+aboard.mboard[4]+aboard.mboard[5];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[6]+aboard.mboard[7]+aboard.mboard[8];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[0]+aboard.mboard[3]+aboard.mboard[6];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[1]+aboard.mboard[4]+aboard.mboard[7];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[2]+aboard.mboard[5]+aboard.mboard[8];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[0]+aboard.mboard[4]+aboard.mboard[8];
	if (v==3*P_X)
		return XWIN;
	v=aboard.mboard[2]+aboard.mboard[4]+aboard.mboard[6];
	if (v==3*P_X)
		return(XWIN);
	S32 orcode =	aboard.mboard[0] | aboard.mboard[1] | aboard.mboard[2] |
					aboard.mboard[3] | aboard.mboard[4] | aboard.mboard[5] |
					aboard.mboard[6] | aboard.mboard[7] | aboard.mboard[8];
	if (orcode & P_BLANK) // bit 1 set only when a blank
		return OPEN;
	return CATS; // every square not a 1
}

// O just moved
// see if P_O wins, returns OPEN,OWINS or CATS
status positO()
{
	S32 v;
	v=aboard.mboard[0]+aboard.mboard[1]+aboard.mboard[2];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[3]+aboard.mboard[4]+aboard.mboard[5];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[6]+aboard.mboard[7]+aboard.mboard[8];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[0]+aboard.mboard[3]+aboard.mboard[6];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[1]+aboard.mboard[4]+aboard.mboard[7];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[2]+aboard.mboard[5]+aboard.mboard[8];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[0]+aboard.mboard[4]+aboard.mboard[8];
	if (v==3*P_O)
		return OWIN;
	v=aboard.mboard[2]+aboard.mboard[4]+aboard.mboard[6];
	if (v==3*P_O)
		return OWIN;
	S32 orcode =	aboard.mboard[0] | aboard.mboard[1] | aboard.mboard[2] |
					aboard.mboard[3] | aboard.mboard[4] | aboard.mboard[5] |
					aboard.mboard[6] | aboard.mboard[7] | aboard.mboard[8];
	if (orcode & P_BLANK) // bit 1 set only when a blank
		return OPEN;
	return CATS; // every square not a 1
}

void draw()
{
	S32 i,j;
	con32_printf(acon,"\n");
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {
			switch(aboard.mboard[j+i*3]) {
			case P_X:		con32_printf(acon," X ");
				break;
			case P_BLANK:	con32_printf(acon," %d ",j+i*3+1);
				break;
			case P_O:		con32_printf(acon," O ");
				break;
			}
			if (j!=2)
				con32_printf(acon,"|");
		}
		con32_printf(acon,"          ");
		for (j=0;j<3;j++) {
			switch(aboard.mboard[j+i*3]) {
			case P_X:		con32_printf(acon," X ");
				break;
			case P_BLANK:	con32_printf(acon,"   ");
				break;
			case P_O:		con32_printf(acon," O ");
				break;
			}
			if (j!=2)
				con32_printf(acon,"|");
		}
		con32_printf(acon,"\n");
		if (i!=2)
			con32_printf(acon,"---+---+---          ---+---+---\n");
	}
	con32_printf(acon,"\n");
}

status checkO();
status checkX() // check P_X's recent move, returns 0 X wins, 2 O wins, 3 cats
{
	status g;
	S32 r;
	status bestO=XWIN; // 0 P_X wins, or 3 cats
	g=positX(); // 1 not yet, 2 P_O wins, 3 cats
	if (g!=OPEN)
		return g; // returns 0 P_X wins, 3 cats
	for (r=0;r<9;r++) { // study P_O's moves
		if (aboard.mboard[r]==P_BLANK) {
			aboard.mboard[r]=P_O;
			g=checkO(); // 0 P_X wins, 2 P_O wins, 3 cats
			aboard.mboard[r]=P_BLANK;
			if (g==OWIN)
				return OWIN; // P_O can force a win
			if (g==CATS)
				bestO=CATS; // P_O can force cats
		}
	}
	// P_O has no winning move
	return bestO; // returns 0 P_X wins, or 3 cats
}

status checkO() // check P_O's recent move, returns 0 P_X wins, 2 P_O wins, 3 cats
{
	status g;
	S32 r;
	status bestX=OWIN; // 2 P_O wins, or 3 cats
	g=positO(); // 1 not yet, 2 P_O wins, 3 cats
	if (g!=OPEN)
		return g; // returns 2 P_O wins, 3 cats
	for (r=0;r<9;r++) { // study P_X's moves
		if (aboard.mboard[r]==P_BLANK) {
			aboard.mboard[r]=P_X;
			g=checkX(); // 0 P_X wins, 2 P_O wins, 3 cats
			aboard.mboard[r]=P_BLANK;
			if (g==XWIN)
				return XWIN; // P_X can force a win
			if (g==CATS)
				bestX=CATS; // P_X can force cats
		}
	}
	// P_X has no winning move
	return bestX; // returns 2 P_O wins, or 3 cats
}

bool isempty()
{
	S32 i;
	for (i=0;i<9;++i)
		if (aboard.mboard[i]!=P_BLANK)
			return false;
	return true;
}

S32 compute()
{
	// see if empty game, if so random first move
	if (isempty()) {
		S32 ret = firstmove; // pick any square for first move
		++firstmove;
		if (firstmove==9)
			firstmove=0;
		return ret;
	}
	S32 v;
	status g;
	const S32 NOTSET = 9;
	S32 xwins=NOTSET; // 0, 9 means not set
	S32 cats=NOTSET;  // 3, 9 means not set
	// run thru all the possible moves for P_O
	for (v=0;v<9;v++) {
		if (aboard.mboard[v]==P_BLANK) {
			aboard.mboard[v]=P_O;
			g=checkO(); // 0 P_X wins, 2 P_O wins, 3 cats
			aboard.mboard[v]=P_BLANK;
			if (g==OWIN)
				return v; // returns first winning move
			if (xwins==NOTSET && g==XWIN)
				xwins = v; // found a P_X wins
			else if (cats==NOTSET && g==CATS)
				cats = v; // found a cats
		}
	}
	if (cats!=NOTSET)
		return cats; // returns first recorded cats move
	return xwins; // worst case, return first recorded P_X wins move
}

void startagame()
{
	con32_printf(acon,"human is x, computer is o\n");
	con32_printf(acon,"who is first, x, o or r reset, ESC to quit? ");
//	aboard.mgs = WHOFIRST;
	reset(WHOFIRST);
	draw();
}

void endagame(status p)
{
	con32_printf(acon,"Game Over\n");
	switch (p) {
	case XWIN:	con32_printf(acon,"X wins!\n");
		break;
	case OWIN:	con32_printf(acon,"O wins!\n");
		break;
	case CATS:	con32_printf(acon,"Cats\n");
		break;
	case OPEN:	con32_printf(acon,"Reset\n");
		break;
	default:
		break;
	}
}

void humango()
{
	con32_printf(acon,"your turn\n");
	con32_printf(acon,"just enter a number from 1 to 9 ");
	aboard.mgs = TURN_X;
}

void aigo()
{
	con32_printf(acon,"my turn\n");
	aboard.mgs = TURN_O;
}

} // end namespace ttt
using namespace ttt;

void tttinit()
{
	video_setupwindow(800,600);
	acon=con32_alloc(480,480,C32BLACK,C32WHITE);
	con32_printf(acon,"tic tac toe\n");
	startagame();
}

void tttproc()
{
	switch (KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
	S32 k;
	switch(KEY) {
	case 'r':
		endagame(OPEN);
		startagame();
		break;
	}
	status p;
	switch (aboard.mgs) {
	case WHOFIRST: // who first, if computer then make that move and go to yourturn
		switch(KEY) {
		case 'x':
			humango();
			break;
		case 'o':
			aigo();
			break;
		}
		break;
	case TURN_X: // get your input on your turn, but 'auto' if only one open postion left
		k = almost(); // return 1-9 if only one open square, 0 otherwise
		if (!k) {
			if (KEY>='1' && KEY<='9') {
				k = KEY - '0';
				if (aboard.mboard[k-1]!=P_BLANK) // empty?
					k = 0;
			}
		}
		if (k) { // your move
			aboard.mboard[k-1]=P_X;
			draw();
			p = positX();
			if (p==OPEN) {
				aigo();
			} else {
			endagame(p);
			startagame();
			}
		}
		break;
	case TURN_O:
		aboard.mboard[compute()]=P_O; // P_O
		draw();
		p = positO();
		if (p==1) {
			humango();
		} else {
			endagame(p);
			startagame();
		}
		break;
	}
}

void tttdraw2d()
{
	clipclear32(B32,C32BROWN);
	bitmap32* cbm=con32_getbitmap32(acon);
	clipblit32(cbm,B32,0,0,20,20,cbm->size.x,cbm->size.y);
}

void tttexit()
{
	con32_free(acon);
	acon = 0;
}
