#include <m_eng.h>
#include <math_abstract.h>

#include "u_states.h"
#include "u_volley.h"

#define USENAMESPACE
#ifdef USENAMESPACE
namespace honeycomb {
#endif
//#define DO_VOLLEY

////////// misc globals ////////
con32 *acon; // console output
script files; // all the level filenames
int curlevel = 0;
const C8* boardname;
int aborted = 0; // has ESC hit
const int COMPSPEED=30; // how much time ai used to move a piece on the screen
struct bitmap32* d,*a,*x,*o; // board,arrow,'X','O'
struct bitmap8* d8; // used to scan board for positions
#define XSIZE 640 // size of game
#define YSIZE 480
C32 outdacs[256]; // convert .lbm to .pcx for board levels

// a board, array of 'squares'
// const
const int MAXNUMSQUARES=16;
int numsquares;
int onecolor=1; // 1: players move the same pieces,  2: players each have their own pieces ('X' 'O')
struct pointi2 xypos[MAXNUMSQUARES]; // location of squares, for drawing
enum PCE {B,X,O};
char* pcenames[]={".","X","O"};

// instances of board (enum PCE)
int startboard[MAXNUMSQUARES]; // when reset, use this board
int curboard[MAXNUMSQUARES]; // current board

// players
char* playertype[2]={"Computer","Human"};
enum {COMPUTER,HUMAN};
int pplayers[2]={HUMAN,HUMAN};

// turn logic
char *turnmessage[2]={"Rat's turn","Scorp's turn"};
int turn=0; // don't set directly, usechangeturn(newturn) instead, this will flip the curboard also
int initturn=0; // turn when game gets reset
int flip[3]={B,O,X}; // only used if not onecolor, swaps 'X' and 'O'
const int BIGLOSE = 30000;
const int STARTBACK = 15000;
// legal move table
int legalmovetable[MAXNUMSQUARES][MAXNUMSQUARES]; // [to][from]

// ai logic
const int MAXCHOICES=16;
enum estat {NOTSET,HWIN,HLOSE};
struct strategys {
	U32 id;
	S32 next[MAXCHOICES]; // idx into strategys
	U8 numnext;
	U8 status; // enum estat
	U16 winmoves;
	U16 losemoves;
	U16 back; // idx into strategys, but BIGLOSE is considered a lose
};
struct strategys zerpos;
#define MAXPOSITIONS 100000
struct strategys boards[MAXPOSITIONS];
int numstrategys;

// game stats
int score[2];
char *losemessage[2]={"Rat loses!","Scorp loses!"};
//int winning; // enum estat
char *wm[3]={"DRAW","WINNING","LOSING"};

// ui states and control
int mox,moy; // for ai mouse control
int piecesel=-1;
int startpiece=-1,endpiece;
enum gstate {GETPIECE,MOVEPIECE,ENDTURN,ENDGAME,GAMESTATENUM};
gstate getmovestate=GETPIECE;
int statetimer = 0; // used in MOVEPIECE(ai) and ENDTURN and ENDGAME

// board utils

// read board text data file
void loadconfig(char *s)
{
	con32_printf(acon,"Loading level '%s'\n",s);
	initturn = 0;
	numsquares = 0;
	piecesel = -1;
	numstrategys = 0;
	fill(boards,boards+MAXPOSITIONS,zerpos);
	int i,j,v,c,cnt;
	FILE* fr = fopen2(s,"r");
	onecolor = 1;
	while(1) {
		v=filereadU8(fr);
		if (v=='X' || v=='O' || v=='B') {
			if (numsquares>=MAXNUMSQUARES) {
				errorexit("too many circles, aborting..");
			}
			switch(v) {
			case 'X':
				c=X;
				break;
			case 'O':
				c=O;
				onecolor=0;
				break;
			case 'B':
				c=B;
				break;
			}
			startboard[numsquares++]=c;
		} else if (v=='1' || v=='0')
			break;
	}
	fseek(fr,-1,SEEK_CUR);
	for (j=0;j<numsquares;j++) {
		for (i=0;i<numsquares;i++) {
			cnt=0;
			while(1) {
				v=filereadU8(fr);
				if (v=='0'|| v=='1')
					break;
				cnt++;
				if (cnt==100) {
					errorexit("problem with dat table");
				}
			}
			legalmovetable[i][j]=v-'0';
		}
	}
	fclose(fr);
}

// read and scan for dots board graphic file
void loadboard(char *s) //,char* sout)
{
	const U32 HOTCOLOR=255;
	int i,j,n=0;
//	b = bitmap32alloc(XSIZE,YSIZE,C32WHITE);
	d = gfxread32(s);
	colorkeyinfo.usecolorkey = false;
	a = gfxread32("arrow.bbm");
	x = gfxread32("x.bbm");
	o = gfxread32("o.bbm");
	colorkeyinfo.usecolorkey = true;
	d8 = gfxread8(s,outdacs);
	//gfxwrite8(sout,d8,outdacs);
	for (j=0;j<YSIZE;j++) {
		for (i=0;i<XSIZE;i++) {
			if (clipgetpixel8(d8,i,j)==HOTCOLOR) {
				if (n>=numsquares) {
					errorexit("pcx %d had more points than dat %d",n,numsquares);
				}
				clipputpixel32(d,i,j,clipgetpixel32(d,i-1,j)); // cover up hot pixel with an adjacent pixel
				xypos[n].x=i;
				xypos[n].y=j;
				n++;
			}
		}
	}
	bitmap8free(d8);
	if (numsquares!=n) {
		errorexit("pcx %d had less points than dat %d",n,numsquares);
	}
}

// reset whole game to clean state
void initboard()
{
//	move32(startboard,curboard,sizeof(curboard));
	copy(startboard,startboard+MAXNUMSQUARES,curboard);
	turn=0;
	getmovestate=GETPIECE;
	piecesel=-1;
}

// turn a board array into a U32 id
U32 getid(int ab[16])
{
	U32 id=0;
	S32 i;
	for (i=0;i<numsquares;i++)
		id+=ab[i]<<(i<<1); // 16 of 2bits = 32bits
	return id;
}

// turn an id into a board
void breakid(U32 id,int *pos)
{
	int i;
	for (i=0;i<numsquares;i++)
		pos[i]=(id>>(i<<1))&3;
}

// can only move 'X'
int checkmove(int st,int end)
{
	return	curboard[st]==X &&
	    	curboard[end]==B &&
			legalmovetable[end][st];
	// return 0;
}

// moves 'X', do only when checkmove returns true
void movepiece(int st,int end)
{
	curboard[st]=B;
	curboard[end]=X;
}

// turn logic
void changeturn(int nt)
{
	int n;
	if (turn==nt)
		return;
	if (!onecolor)
		for (n=0;n<numsquares;n++)
			curboard[n]=flip[curboard[n]];
	turn=nt;
}

// ui utils

// get square closest to the cursor
int findclose()
{
	int dist,bdist=400;
	int n,bn=-1;
	for (n=0;n<numsquares;n++) {
		dist=(xypos[n].x-MX)*(xypos[n].x-MX)+(xypos[n].y-MY)*(xypos[n].y-MY);
		if (dist<bdist) {
			bdist=dist;
			bn=n;
		}
	}
	return bn;
}

// ai logic
// returns false when player cannot move anymore (lose)
bool isloser()
{
	//return false;
	int s,e;
	for (s=0;s<numsquares;s++)
		for (e=0;e<numsquares;e++)
			if (checkmove(s,e))
				return false;
	return true;
}

// take curboard and return existing or new idx in ptr, returns true if new
bool registerboard(int *brd)
{
	int i;
	int id;
	if (aborted)
		return 0;
	id=getid(curboard);
	for (i=0;i<numstrategys;i++) {
		if (boards[i].id==id) {
			*brd=i;
			return false;
		}
	}
	if (numstrategys>=MAXPOSITIONS) {
		errorexit("too many board boards");
	}
	boards[numstrategys].id=id;
	numstrategys++;
	if (numstrategys%250==0) {
		con32_printf(acon,"numstrategys = %d\n",numstrategys);
		winproc();
		if (KEY==K_ESCAPE || KEY=='a') {
			KEY = 0;
			aborted = 1;
			numstrategys = 0;
			return false;
		}
	}
	*brd=i;
	return true;
}

// returns index of curboard, if new then studies all possible moves from curboard
int studygame()
{
	static int level;
	int s,e,brd;
	if (!registerboard(&brd)) // register curboard
		return brd; // already visited
	for (s=0;s<numsquares;s++) {
		for (e=0;e<numsquares;e++) {
			if (checkmove(s,e)) {
				movepiece(s,e); // changes curboard
				changeturn(turn^1);
				int nm=boards[brd].numnext++;
				if (nm==MAXCHOICES)
					errorexit("too many moves!");
				++level;
				boards[brd].next[nm]=studygame();
				--level;
				changeturn(turn^1);
				movepiece(e,s); // puts curboard back
			}
		}
	}
	/* if (nm==0)
		boards[np].status=HLOSE; */
	return brd;
}

void filltree()
{
	if (aborted)
		return;
	int change;
	do { // fill out boards with WIN and LOSE, backtrack
		change=0;
		int np;
		for (np=0;np<numstrategys;np++) {
			int w=0,l=0;
			int stat=HLOSE; // stays LOSE if all nexts are WIN
			int* next=boards[np].next;
			int nm;
			for (nm=0;nm<boards[np].numnext;nm++) {
				switch(boards[next[nm]].status) {
				case HLOSE:
					w+=1<<nm; // this move makes other player lose, WIN sticky
					stat=HWIN;
					break;
				case HWIN: // this move makes other player win, let's try to avoid this one
					l+=1<<nm;
					break;
				default:
					if (stat!=HWIN) // don't change if we got a winning move, keep stat WIN sticky
						stat=NOTSET; // NOTSET if unknowns still out there and not any WIN on the next
					break;
				}
			}
			if (nm==0) // can't move, lose
				boards[np].back=BIGLOSE; // this is a big LOSE
			if (w!=boards[np].winmoves || l!=boards[np].losemoves || stat!=boards[np].status) { // update boards that have changed
				boards[np].winmoves=w;
				boards[np].losemoves=l;
				boards[np].status=stat;
				change=1;
			}
		}
	} while (change);
	do { // calc back
		change=0;
		int np;
		for (np=0;np<numstrategys;np++) {
			int stat=boards[np].status;
			int* next=boards[np].next;
			int bck=STARTBACK;
			if (stat==HWIN) {
				bck=0;
				int nm;
				for (nm=0;nm<boards[np].numnext;nm++) {
	/*				if (np==6)
						deb();
					if (np==7)
						deb(); */
					if (boards[next[nm]].status==HLOSE) {
						int nxt=boards[next[nm]].back;
						bck=max(bck,nxt-1);
					}
				}
			} else if (stat==HLOSE) {
				bck=BIGLOSE;
	/*			if (np==9)
					deb(); */
				int nm;
				for (nm=0;nm<boards[np].numnext;nm++) {
					if (boards[next[nm]].status==HWIN) {
						int nxt=boards[next[nm]].back;
						if (nxt)
							bck=min(bck,nxt-1);
					}
				}
			}
			if (boards[np].back!=bck) {
				boards[np].back=bck;
				change=1;
			}
		}
	} while (change);
}

void getcompturnstupid(int *s,int *e)
{
	vector<pointi2> moves;
	S32 i,j;
	for (j=0;j<numsquares;++j) {
		for (i=0;i<numsquares;++i) {
			if (checkmove(i,j)) {
				moves.push_back(pointi2x(i,j));
			}
		}
	}
	S32 nmoves = moves.size();
	if (!nmoves) {
		*s = *e = 0;
		return;
	}
	i = mt_random(nmoves);
	*s = moves[i].x;
	*e = moves[i].y;
}

int getstatus()
{
	int id=getid(curboard);
	int i;
	// find board idx in boards
	for (i=0;i<numstrategys;i++)
		if (boards[i].id==id)
			break;
	if (i==numstrategys) { // strategy doesn't exist, maybe you cheated
		return 0; // draw
	}
	return boards[i].status;
}

void getcompturn(int *s,int *e)
{
	if (aborted) {
		getcompturnstupid(s,e);
		return;
	}
	int id=getid(curboard);
	// find board idx in boards
	int i;
	for (i=0;i<numstrategys;i++)
		if (boards[i].id==id)
			break;
	if (i==numstrategys) { // strategy doesn't exist, maybe you cheated
		//error("no stategy");
		//*s = *e = 0;
		getcompturnstupid(s,e);
		return;
	}
	if (!boards[i].numnext) {
		//error("no moves, not moving");
		*s = *e = 0;
		return;
	}
	int* nextp=boards[i].next;
	int mv = 0;
	if (boards[i].status==HWIN) { // this is a winner
		int best=0; // find quickest win, highest back
		for (mv=0;mv<boards[i].numnext;mv++) {
			if (boards[nextp[mv]].back>best && boards[nextp[mv]].status==HLOSE)
				best=boards[nextp[mv]].back;
		}
		while(1) {
			mv=mt_random(boards[i].numnext);
			if (boards[nextp[mv]].status==HLOSE && boards[nextp[mv]].back==best)
				break;
		}
	} else if (boards[i].status==HLOSE) { // this is a loser
		int best=BIGLOSE; // find farthest win, lowest back
		for (mv=0;mv<boards[i].numnext;mv++) {
			if (boards[nextp[mv]].back<best)
				best=boards[nextp[mv]].back;
		}
		while(1) {
			mv=mt_random(boards[i].numnext);
			if (boards[nextp[mv]].back==best)
				break;
		}
	} else { // draw, next move has no lose in it, make this better
		while(1) {
			mv=mt_random(boards[i].numnext);
			if (boards[nextp[mv]].status!=HWIN) // stay away from any move that makes opponent win., keep in draw
				break;
		}
	}
	// get start and end move from next newid
	int newid=boards[nextp[mv]].id;
	int idpos[MAXNUMSQUARES];
	breakid(id,idpos);
	int newidpos[MAXNUMSQUARES];
	breakid(newid,newidpos);
	int j;
	for (j=0;j<numsquares;j++) {
		if (idpos[j]!=B && newidpos[j]==B)
			*s=j;
		if (idpos[j]==B && newidpos[j]!=B)
			*e=j;
	}
}

// called from state draw function
// pce >=0 if moving a piece, if so then use the mox moy for current pos of that piece
void showboard(int pce,int gstate,int mox,int moy)
{
	int n=0,i,id;
	char str[400];
	char *ss[GAMESTATENUM]={"get piece","move piece","drop piece","end game"};
	struct bitmap32* p;
	clipblit32(d,B32,0,0,0,0,XSIZE,YSIZE);
	outtextxy32(B32,0,0,C32WHITE,turnmessage[turn]);
	outtextxy32(B32,0,8,C32WHITE,ss[gstate]);
	outtextxy32(B32,0,24,C32WHITE,wm[getstatus()]);
//	outtextxy32(B32,0,24,C32WHITE,wm[winning]);
	for (n=0;n<numsquares;n++) {
		if (turn==0)
			if (curboard[n]==X)
				p=x;
			else
				p=o;
		else
			if (curboard[n]==X)
				p=o;
			else
				p=x;
		if ((curboard[n]==X || curboard[n]==O) && n!=pce)
			clipxpar32(p,B32,0,0,xypos[n].x-p->size.x/2,xypos[n].y-2*p->size.y/3,XSIZE,YSIZE,C32BLACK);
	}
	if (pce>=0) {
		if (turn==0)
			p=x;
		else
			p=o;
		clipxpar32(p,B32,0,0,mox-p->size.x/2,moy-2*p->size.y/3,XSIZE,YSIZE,C32BLACK);
	}
//	clipxpar32(a,B32,0,0,mox,moy,XSIZE,YSIZE,C32BLACK);
	id=getid(curboard);
	n = -1;
	for (i=0;i<numstrategys;i++)
		if (boards[i].id==id) {
			n=boards[i].back;
			break;
		}
	sprintf(str,"Score: Rat %3d    Scorp %3d    # %d",score[0],score[1],n);
	outtextxy32(B32,0,d->size.y-16,C32WHITE,str);
//	clipblit32(b,B32,0,0,0,0,XSIZE,YSIZE);
}

void logboards()
{
	logger("sizeof boards = %d\n",sizeof(boards));
	logger("# of board boards = %d\n",numstrategys);
	if (numstrategys > 1000) {
		logger("too many strategys, not logging\n");
		return;
	}
	int i,j;
	for (i=0;i<numstrategys;i++) {
		logger("%3d ******** curboard #\n",i);
		logger_indent();
		int brd[MAXNUMSQUARES];
		breakid(boards[i].id,brd);
		// assume x by 1 for now
		logger_disableindent();
		for (j=0;j<numsquares;++j) {
			logger("%s ",pcenames[brd[j]]);
		}
		logger("\n");
		if (!strcmp(boardname,"ten2")) {
			S32 sbi = 0; // calc a special board number for 10 by 10 table
			for (j=0;j<numsquares;++j) {
				switch(brd[j]) {
				case 1: // X
					sbi += j;
					break;
				case 2: // O
					sbi += 10*j;
					break;
				}
			}
			logger("in special level ten2 %d\n",sbi);
		}
		logger_enableindent();
		logger_indent();
		logger("numnext = %d, stat = %s, winm = %d, losem = %d, back = %d\n",
			boards[i].numnext,wm[boards[i].status],boards[i].winmoves,
				boards[i].losemoves,boards[i].back);
		logger_disableindent();
		logger("nextmoves =");
		for (j=0;j<boards[i].numnext;j++)
			logger(" %d",boards[i].next[j]);
		logger_enableindent();
		logger_unindent();
		logger_unindent();
		logger("\n\n");
	}
}

#ifdef USENAMESPACE
} // end namespace honeycomb
using namespace honeycomb;
#endif

void honeycombinit()
{
	pushandsetdir("HONEYCOM");
// get levels from directory
	if (!files.num()) {
		files = scriptdir(false);
		U32 i,n=files.num();
		script levels;
		for (i=0;i<n;++i) {
			if (isfileext(files.idx(i).c_str(),"txt")) {
				char levelname[500];
				mgetname(files.idx(i).c_str(),levelname);
				levels.addscript(levelname);
			}
		}
		files = levels;
	}
	aborted = 0;
#if 0
#endif
#ifdef DO_VOLLEY
	scoretest();
#endif
#if 1
	static bool found = false;
	if (!found) {
		C8* findboardname = "tritri";
		for(curlevel=0;curlevel<files.num();++curlevel)
			if (files.idx(curlevel) == findboardname)
				break;
		if (curlevel == files.num())
			curlevel = 0;
		found = true;
	}
#endif
	boardname = files.idx(curlevel).c_str();
//	pplayers[0]=COMPUTER;
//	pplayers[1]=HUMAN;
	video_setupwindow(800,600);
	acon=con32_alloc(480,120,C32BLACK,C32WHITE);
	con32_printf(acon,"honeycomb\n");
	char s[100];
	mt_setseed(getmillisec());
	sprintf(s,"%s.txt",boardname);
	loadconfig(s);
	sprintf(s,"%s.pcx",boardname);
	//char soutname[100];
	//sprintf(soutname,"%s.pcx",boardname);
	loadboard(s);
	popdir();

//	if (pplayers[0]==COMPUTER || pplayers[1]==COMPUTER) {
		initboard();
		studygame();

		initboard();
		changeturn(1);
		studygame();
		filltree();
		con32_printf(acon,"numstrategys = %d\n",numstrategys);
//	}
	initboard();
	changeturn(initturn);
	//initgraph();
//	delay(1000);
//	showcursor(0);
	logboards();
}

void honeycombproc()
{
	const int ENDGAMETIME=120;
	const int MOVETIME=10;
	const int DONETURNTIME=2;
	if (isloser() && getmovestate==GETPIECE) {
		++score[turn^1];
		getmovestate = ENDGAME;
		//winning = 2;
		statetimer = ENDGAMETIME;
		return;
	}
	switch (KEY) {
	case K_ESCAPE:
//		if (!aborted)
			poporchangestate(STATE_MAINMENU);
		break;
	case '-':
			--curlevel;
			if (curlevel<0)
				curlevel=files.num()-1;
			changestate(STATE_HONEYCOMB);
			break;
		case '=':
			++curlevel;
			if (curlevel>=files.num())
				curlevel=0;
			changestate(STATE_HONEYCOMB);
			break;
	case 'r':
		initboard();
		initturn^=1;
		changeturn(initturn);
		// changeturn(mt_random(2));
		return;
	case 't':
		if (getmovestate==GETPIECE)
			changeturn(turn^1); // cheating
		return;
	case '1':
		pplayers[0]^=1;
//		getmovestate=GETPIECE;
//		piecesel=-1;
		break;
	case '2':
		pplayers[1]^=1;
//		getmovestate=GETPIECE;
//		piecesel=-1;
		break;
	}
	switch (getmovestate) {
/*			if (pplayers[turn]==COMPUTER) {
//				showcompturn(startpiece,endpiece);
				winning=0;
				movepiece(startpiece,endpiece);
				changeturn(turn^1);
				return;
			} */
	case GETPIECE:
		getcompturn(&startpiece,&endpiece);
		if (pplayers[turn]==COMPUTER) {
			if (startpiece == endpiece) {
				statetimer=DONETURNTIME; // can't move, skip
				getmovestate=ENDTURN;
				changeturn(turn^1);
				piecesel=-1;
			} else {
				mox = xypos[startpiece].x;
				moy = xypos[startpiece].y;
				getmovestate=MOVEPIECE;
				piecesel = startpiece;
				statetimer = MOVETIME;
			}
		} else { // HUMAN
			if (MBUT&!LMBUT) {
				startpiece=findclose();
				if (startpiece>=0 &&
					curboard[startpiece]==X) {
					getmovestate=MOVEPIECE;
					piecesel = startpiece;
					mox = MX;
					moy = MY;
				} else {
					startpiece=-1;
				}
			}
		}
		break;
	case MOVEPIECE:
		if (pplayers[turn]==COMPUTER) {
			if (statetimer>0) {
				--statetimer;
				mox=(statetimer*xypos[startpiece].x+(MOVETIME-statetimer)*xypos[endpiece].x)/MOVETIME;
				moy=(statetimer*xypos[startpiece].y+(MOVETIME-statetimer)*xypos[endpiece].y)/MOVETIME;
			} else {
				movepiece(startpiece,endpiece);
				statetimer=DONETURNTIME;
				getmovestate=ENDTURN;
				changeturn(turn^1);
				piecesel=-1;
			}
		} else { // HUMAN
			mox = MX;
			moy = MY;
			if (!MBUT) {
				endpiece=findclose();
	  			if (endpiece>=0) {
					if (checkmove(startpiece,endpiece)) {
						movepiece(startpiece,endpiece);
						statetimer=DONETURNTIME;
						getmovestate=ENDTURN;
						changeturn(turn^1);
						piecesel=-1;
					} else {
						getmovestate=GETPIECE;
						startpiece=-1;
						piecesel=-1;
					}
				}
			}
		}
		break;
	case ENDTURN:
		if (statetimer>0) {
			--statetimer;
		} else {
			getmovestate=GETPIECE;
			//winning = 0;
		}
		return;
	case ENDGAME:
		if (statetimer>0) {
			--statetimer;
		} else {
			initboard();
			initturn ^= 1;
			changeturn(initturn);
			//winning = 0;
			break;
		}
	}
}

void honeycombdraw2d()
{
	clipclear32(B32,C32BROWN);
	bitmap32* cbm=con32_getbitmap32(acon);
	clipblit32(cbm,B32,0,0,20,WY-32-cbm->size.y,cbm->size.x,cbm->size.y);
	showboard(piecesel,getmovestate,mox,moy);
	outtextxyf32(B32,30,576,C32WHITE,
		"'r' reset  'u' undo  'h' hint  'a' abort hint  '=' levelup  '-' leveldown  undos(%d), hints(%d)",
		0,0);
	outtextxyf32(B32,30,584,C32WHITE,"'1' change player 1 (%s) '2' change player 2 (%s)  current level (%s)",
		playertype[pplayers[0]],playertype[pplayers[1]],files.idx(curlevel).c_str());
	if (getmovestate==ENDGAME)
		outtextxy32(B32,170,32,C32WHITE,losemessage[turn]);
	//clipxpar32(a,B32,0,0,MX,MY,XSIZE,YSIZE,C32BLACK); // draw an arrow
}

void honeycombexit()
{
//	bitmap32free(b);
	bitmap32free(d); // picture of board level
	bitmap32free(a); // arrow
	bitmap32free(x); // player 1
	bitmap32free(o); // player 2
	con32_free(acon); // console
	acon = 0;
//	showcursor(1);
}
