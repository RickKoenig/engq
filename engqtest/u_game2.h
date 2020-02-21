// handy hash efficiency calculator
template<typename T>
void calchasheff(vector<T> h)
{
	S64 bn = 123456789123456789LL;
	logger("big number %lld\n",bn);
	S64 sum = 0;
	S64 sum2 = 0;
	S32 i,n = h.size();
	for (i=0;i<n;++i) {
		S32 v = h[i].size();
		sum += v;
		sum2 += v*v;
	}
	logger("hash efficiency best %lld, actual %lld, worst %lld\n",sum*sum/n,sum2,sum*sum);
}

// generic piece
class piece2 {
public:
	C8 who2; // who can move this piece // All, White, Black 'A','W','B'
	C8 kind; // ascii name of piece like X,O,K,P etc.
	pointi2b8x pos; // piece location  0,0 is upper left
	piece2(C8 whoa = 'A',C8 kinda = ' ',S32 pxa = -1,S32 pya = -1) : who2(whoa),kind(kinda),pos(pxa,pya) {}
	piece2(FILE* fr);
	piece2(const class piece2c& p) {}
	void write(FILE* fw) const;
	bool operator<(const piece2& rhs) const
	{
		if (pos.y < rhs.pos.y)
			return true;
		if (pos.y > rhs.pos.y)
			return false;
		if (pos.x < rhs.pos.x)
			return true;
		if (pos.x > rhs.pos.x)
			return false;
		if (who2 < rhs.who2)
			return true;
		if (who2 > rhs.who2)
			return false;
		if (kind < rhs.kind)
			return true;
		return false;
	}
	bool operator==(const piece2& rhs) const
	{
		perf_start(TEST7);
		bool ret = pos == rhs.pos && who2 == rhs.who2 && kind == rhs.kind;
		perf_end(TEST7);
		return ret;
	}
	//bool operator==(const piece& rhs) const { return who == rhs.who && k == rhs.k && pos == rhs.pos; }
	friend class board;
	friend class board_nim;
	friend class board_endgame;
	friend class board_ttt;
};

// generic piece
//#pragma pack(2)
class piece2c {
public:
	S16 posx:5;
	S16 posy:5;
	S16 who2:2; // who can move this piece // All, White, Black 'A','W','B'
	S16 kind:4; // ascii name of piece like X,O,K,P etc.
	//pointi2b4x pos; // piece location  0,0 is upper left
	piece2c(C8 whoa = 'A',C8 kinda = ' ',S32 pxa = -1,S32 pya = -1) : who2(whoa),kind(kinda),posx(pxa),posy(pya) {}
	piece2c(FILE* fr);
	piece2c(const piece2& p) {}
	void write(FILE* fw) const;
	bool operator<(const piece2c& rhs) const
	{
		if (posy < rhs.posy)
			return true;
		if (posy > rhs.posy)
			return false;
		if (posx < rhs.posx)
			return true;
		if (posx > rhs.posx)
			return false;
		if (who2 < rhs.who2)
			return true;
		if (who2 > rhs.who2)
			return false;
		if (kind < rhs.kind)
			return true;
		return false;
	}
	bool operator==(const piece2c& rhs) const
	{
		perf_start(TEST7);
		bool ret = posx == rhs.posx && posy == rhs.posy && who2 == rhs.who2 && kind == rhs.kind;
		perf_end(TEST7);
		return ret;
	}
	//bool operator==(const piece& rhs) const { return who == rhs.who && k == rhs.k && pos == rhs.pos; }
	friend class board;
	friend class board_nim;
	friend class board_endgame;
	friend class board_ttt;
};
//#pragma pack()
enum status {LOSE,DRAW,UNKNOWN,WIN,STATUSNUM}; // worst to best
struct gamestatus {
	S32 movesleft;
	status stat;
	bool operator==(const gamestatus& rhs) const {return movesleft == rhs.movesleft && stat == rhs.stat;}
	bool operator!=(const gamestatus& rhs) const {return movesleft != rhs.movesleft || stat != rhs.stat;}
};

// generic board
// board always shown from whites point of view, somtimes flip the board, flip the pieces
class board {
	// draw and input sizes
public:
	// instance
	vector<piece2> pieces;
	S32 nummoves; // from start position
	gamestatus st;
	vector<S32> next; // index into strategies
	// end instance
	static vector<vector<piece2> > whos;
	static bool boardflip; // no for nim, yes for chess endgame, no for ttt
	enum symmetry {NONE,FLIPX,ROT180,ROTFLIP}; // 1,2,2,8
		// NONE for nim (maybe FLIPX)
		// FLIPX chess endgames with pawns
		// ROT180 for games like hex
		// ROTFLIP for some chess endgame no pawns
		// ROTFLIP for ttt
	static symmetry sym;
	static const S32 HASHSIZE = (1<<16);
	static const S32 NEWPIECEIDX = 1000000;
	static pointi2 dim;
	static const S32 SQSIZE = 32;
	static const pointi2 OFF;
	static string boardtype; // what game  nim,ttt etc.
	enum boardmove {MOVE,PLOP};
	static boardmove bm;
	board(string boardnamea); // load a level from a file
	board(FILE* fr,bool getstat = false);
	board() {}
	void write(FILE* fw,bool putstat = false) const;
	U32 gethash() const;
	bool operator==(const board& rhs) const;
	bool operator<(const board& rhs) const;
	gamestatus getresult() const; // win,lose,draw now
	gamestatus getfuture() const; // win,lose,draw down the road
	pointi2b8 getpiecepos(S32 pidx) const;
	S32 findcloseidx(bool flipboard) const; // flip board
	bool findcloseemptypos(pointi2b8* pos,bool fb) const;
	bool checkmove(S32 stidx,const pointi2b8& endpos) const;
	void movepiece(S32 stidx,const pointi2b8& endpos);
	static pointi2 board2screen(const pointi2b8& pos);
	static C8 flipcolor(C8 c);
//	static piece::own flipcolor(piece::own c);
	void flipturn(); // no sort, used to change turns, change colors and optional 180 degree flip
	void flipx(); // sort, 'x' flip on 'y' axis, used for equality tests with symmetry
	void rot90(); // sort, rotate board clockwise 90 degrees, used for equality tests with symmetry
	piece2 getnewpiece() const;
	void draw(S32 pi,const piece2& pp,bool ismoving,const pointi2* mp) const;
	void drawsquares(bool ismoving) const;
	void drawpieces(S32 pi,const piece2& pp,bool ismoving,const pointi2* mp) const;
	static C8* predictm[STATUSNUM];//={"UNKNOWN","LOSING","DRAWING","WINNING"};
	static C8* predictma[STATUSNUM];//={"UNKNOWN","LOSING","DRAWING","WINNING"};
	static C8* predictmar[STATUSNUM];//={"UNKNOWN","LOSING","DRAWING","WINNING"};
	static C8* resultm[STATUSNUM];//={"UNKNOWN","LOSES","DRAWS","WINS"};
	void reset_whos() const;
	void make_whos() const;
	bool canmoveto_whos(S32 x,S32 y) const;
};

// turn data
struct turndata {
	S32 pieceidx;
	pointi2b8 dest;
};

template<typename B>
struct undo {
	B b; // board
	S32 t; // turn
	undo(const B& ba,S32 ta) : b(ba),t(ta) {}
};

// templated game with custom board
template<typename B>
class game {
	static const S32 NPLAYERS = 2; // 2 player game
// name
	string levelname;
// boards
	B startboard,curboard;
// undos
	vector<undo<B> > undos;
// log
	struct con32* acon;
public:
// instance strategy
	bool hint;
	S32 initturn; // white,black  player who goest first for next game
	S32 turn; // white,black
	float score[NPLAYERS];
// study, strategy
	//static const S32 MAXPOSITIONS = 10000;
	vector<B> boards;
	vector<vector<U32> >hashes;
// end instance strategy
	bool aborted;
	S32 statetimer;
	S32 startpieceidx; // -1 none, piece that's moving on a turn
	pointi2 curpiecepos; // screen space position of moving piece, used for drawing
	pointi2b8x startpiecepos,endpiecepos; // for tweening
// computer/human player
	enum playerkind {COMPUTER,HUMAN,NPLAYERKIND};
	S32 pplayers[NPLAYERKIND];	// COMPUTER,HUMAN
	static C8* playertype[NPLAYERKIND];
// players/turn
	static C8* turnm[NPLAYERS]; // white/black to play
	static C32 turnc[NPLAYERS]; // white/black to play
	static C8* playerm[NPLAYERS]; // white/black loses
// game draw/win/lose prediction
	static C8 *predictm[STATUSNUM]; // drawing/winning/losing
	static C8 *resultm[STATUSNUM]; // drawing/winning/losing
// game/move state
	enum gstate {GETPIECE,MOVEPIECE,ENDTURN,ENDGAME,GAMESTATENUM};
	static C8* gamestatem[GAMESTATENUM];
	gstate gamestate;
	static const S32 ENDGAMETIME = 60; // how long win/lose screen is up
	static const S32 MOVETIME = 10; // computer move time
	static const S32 DONETURNTIME = 2; // just wait before switching turn
	static const S32 NEWPIECEIDX = 1000000; // a brand new piece yet to be inserted
	mutable piece2 movingpiece;
public:
	static script* getlevels();
	game(const string startboarda);
	~game();
	void reset();
	void changeturn(S32 nt);
	B symlo(const B& b) const; // return a canonical symmetry of board, reduction in number of boards
	//S32 find_board(const B& b,bool dopush,bool dost); // finds / registers board and returns idx or -1 if not found and no dopush
	S32 push_board(const B& b); // return idx of board in boards, always pushes
	S32 push_board_uniq(const B& b); // return idx of board in boards
	S32 find_aboard(const B& b) const; // finds board and returns idx or -1 if not found
	void studygame();
	void studygames(bool fileread = true,bool filewrite = true); // both sides
	bool readstrategy(FILE* fr);
	void writestrategy(FILE* fw) const;
	void backtrack(); // get status of board positions from next moves
	void getcompturn(S32* stidx,pointi2b8* endpos) const;
	void getcompturnstupid(S32* stidx,pointi2b8* endpos) const;
	vector<turndata> getcompturnstupidlist() const;
	void showhint() const;
	//void showhintstupid() const;
	void draw() const;
	void proc();
	const B& findstrat(const B& b) const; // find a board in boards that matches, else just return the board
	//static vector<vector<piece2::own> > whos;
	void pushundo();
	void popundo();
};

template<typename B>
C8* game<B>::playertype[NPLAYERKIND] = {"Computer","Human"};
template<typename B>
C8* game<B>::playerm[NPLAYERS]={"WHITE","BLACK"};
template<typename B>
C8* game<B>::gamestatem[GAMESTATENUM]={"Get piece","Move piece","Drop piece","End game"};
template<typename B>
C8* game<B>::turnm[NPLAYERS]={"White to play","Black to play"};
template<typename B>
C32 game<B>::turnc[NPLAYERS]={C32WHITE,C32BLACK};

template<typename B>
script* game<B>::getlevels()
{
	script osc = scriptdir(0);
	osc.sort();
	script* nsc = new script();
	S32 i,n = osc.num();
	for (i=0;i<n;++i) {
		const C8* s = osc.idx(i).c_str();
		if (isfileext(s,"txt")) // return just level txt files
			nsc->addscript(s);
	}
	return nsc;
}

template<typename B>
const B& game<B>::findstrat(const B& b) const
{
#if 1
//	S32 idx = find_board(b,false,true);
	S32 idx = find_aboard(b);
	if (idx >= 0)
		return boards[idx];
	return b;
#else
	B bc = symlo(b);
	U32 i,n = boards.size();
	for (i=0;i<n;++i) {
		if (bc == boards[i])
			return boards[i];
	}
#endif
	return b;
}

template<typename B>
game<B>::game(string levelnamea) :
levelname(levelnamea),startboard(levelnamea),curboard(startboard),aborted(false),hashes(board::HASHSIZE,vector<U32>())
{
	hint = false;
	pplayers[0] = pplayers[1] = HUMAN;
	gamestate = GETPIECE;
	initturn = 0; // turn when game gets reset
	turn = 0; // don't set directly, usechangeturn(newturn) instead, this will flip the curboard also
	score[0] = score[1] = 0;
	startpieceidx = -1;
	acon = con32_alloc(480,120,C32BLACK,C32WHITE);
	con32_printf(acon,"Hello!\n");
}

template<typename B>
game<B>::~game()
{
	con32_free(acon);
}

// reset whole game to clean state
template<typename B>
void game<B>::reset()
{
	curboard = startboard;
	turn = 0;
	gamestate = GETPIECE;
	startpieceidx = -1;
	undos.clear();
}

// turn logic, flip board and pieces if necessary
template<typename B>
void game<B>::changeturn(S32 nt)
{
	if (turn == nt)
		return;
	curboard.flipturn();
	sort(curboard.pieces.begin(),curboard.pieces.end());
	turn=nt;
}

 // return a canonical symmetry of board, reduction in number of boards
template<typename B>
B game<B>::symlo(const B& b) const
{
	if (board::sym == board::NONE) {
		return b;
	}
	B lob = b;
	if (board::sym == board::FLIPX) {
		board flipxboard = b;
		flipxboard.flipx();
		sort(flipxboard.pieces.begin(),flipxboard.pieces.end());
		if (flipxboard < lob)
			lob = flipxboard;
		return lob;
	}
	if (board::sym == board::ROT180) {
		board flipboard = b;
		flipboard.rot90();
		flipboard.rot90();
		sort(flipboard.pieces.begin(),flipboard.pieces.end());
		if (flipboard < lob)
			lob = flipboard;
		return lob;
	}
	if (board::sym == board::ROTFLIP) {
		board symboard = b;
		symboard.rot90(); // 90
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
		symboard.rot90(); // 180
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
		symboard.rot90(); // 270
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
		symboard = b;
		symboard.flipx(); // X
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
		symboard.rot90(); // X + 90
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
		symboard.rot90(); // X + 180
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
		symboard.rot90(); // X + 270
		sort(symboard.pieces.begin(),symboard.pieces.end());
		if (symboard < lob)
			lob = symboard;
	}
	return lob;
}

// take curboard and push into boards, return strategy idx
template<typename B>
S32 game<B>::push_board(const B& b)
{
	perf_start(TEST2);
	B hb = symlo(b); // get canonical representation of board after symmetry taken into account
	U32 h = hb.gethash();
	vector<U32>& hashrow = hashes[h];
	U32 n = boards.size();
	hashrow.push_back(n);
	boards.push_back(hb);
	perf_end(TEST2);
	return n;
}
// take curboard and return existing or new strategy idx, -1 if aborted
template<typename B>
S32 game<B>::push_board_uniq(const B& b)
{
	perf_start(TEST4);
	if (aborted) {
		perf_end(TEST4);
		return -1;
	}
	B hb = symlo(b); // get canonical representation of board after symmetry taken into account
	U32 h = hb.gethash();
	vector<U32>& hashrow = hashes[h];
	S32 i,nh = hashrow.size();
	for (i=0;i<nh;i++) {
		U32 idx = hashrow[i];
		if (boards[idx] == hb) {
			perf_end(TEST4);
			return idx;
		}
	}
	U32 n = boards.size();
	hashrow.push_back(n);
	hb.st = hb.getresult(); // seed backtrack
	boards.push_back(hb);
	perf_end(TEST4);
	return n;
}
// take curboard and return existing or new strategy idx, -1 if no push
template<typename B>
S32 game<B>::find_aboard(const B& b) const
{
	if (aborted)
		return -1;
	perf_start(TEST5);
	B hb = symlo(b); // get canonical representation of board after symmetry taken into account
	U32 h = hb.gethash();
	const vector<U32>& hashrow = hashes[h];
	S32 i,nh = hashrow.size();
	for (i=0;i<nh;i++) {
		U32 idx = hashrow[i];
		if (boards[idx] == hb) {
			perf_end(TEST5);
			return idx;
		}
	}
	perf_end(TEST5);
	return -1;
}

// returns index of curboard, if new then studies all possible moves from curboard
template<typename B>
void game<B>::studygame()
{
	S32 nm = 0;
	S32 oldnumstrat = 0;
//	find_board(curboard,true,true); // register curboard with nummoves == 0, SEED
	push_board_uniq(curboard);
	changeturn(turn^1); // flip the board
	//find_board(curboard,true,true); // and black to play first
	push_board_uniq(curboard);
	U32 numstrat = boards.size();
	while(true) {
		con32_printf(acon,"nummoves %3d, boards %d\n",nm,numstrat);
		U32 i;
		for (i=oldnumstrat;i<numstrat;++i) {
			if (i%3000 == 0) {
				con32_printf(acon,"nummoves %4d, numstrategies = %d/%d new %d\n",nm,i,numstrat,boards.size());
				winproc();
				if (KEY==K_ESCAPE || KEY=='a' || wininfo.closerequested) {
					KEY = 0;
					aborted = 1;
					break;
				}
			}
			if (aborted)
				return;
			// B& bi = boards[i]; // this doesn't work because of find_curboard can invalidate the reference
			gamestatus gs = boards[i].getresult();
			//if (gs.movesleft == 0)
			//	logger("movesleft = 0\n");
			boards[i].next.clear(); // this was copied from a board that had some next, let's clear it
			if (boards[i].nummoves == nm && gs.movesleft != 0) {
				pointi2b8 e;
				S32 newbrd;
				perf_start(TEST3);
				if (board::bm == board::PLOP) { // MOVE like TTT
					for (e.y=0;e.y<board::dim.y;++e.y) {
						for (e.x=0;e.x<board::dim.x;++e.x) {
							if (boards[i].checkmove(NEWPIECEIDX,e)) {
								curboard = boards[i];
								//curboard.next.clear(); // new board shouldn't have any nexts
								curboard.nummoves = nm + 1;
								curboard.movepiece(NEWPIECEIDX,e); // changes curboard
								changeturn(turn^1); // changes curboard
								//if (register_curboard(&newbrd))
								//newbrd = find_board(curboard,true,true);
								//find(boards[i].next.begin(),boards[i].next.end(),newbrd);
								newbrd = push_board_uniq(curboard);
								boards[i].next.push_back(newbrd);
							}
						}
					}
				} else { // MOVE like chess,nim
					U32 p,np = boards[i].pieces.size();
					for (p=0;p<np;++p) {
						for (e.y=0;e.y<board::dim.y;++e.y) {
							for (e.x=0;e.x<board::dim.x;++e.x) {
								if (boards[i].checkmove(p,e)) {
									curboard = boards[i];
									//curboard.next.clear(); // new board shouldn't have any nexts
									curboard.nummoves = nm + 1;
									curboard.movepiece(p,e); // changes curboard
									changeturn(turn^1); // changes curboard
									//if (register_curboard(&newbrd))
									//newbrd = find_board(curboard,true,true);
									newbrd = push_board_uniq(curboard);
									boards[i].next.push_back(newbrd);
								}
							}
						}
					}
				}
				// remove dups
				sort(boards[i].next.begin(),boards[i].next.end());
				vector<S32>::iterator it = unique(boards[i].next.begin(),boards[i].next.end());
				boards[i].next.resize(distance(boards[i].next.begin(),it));
				perf_end(TEST3);
			}
		}
		oldnumstrat = numstrat;
		U32 newnumstrat = boards.size();
		if (newnumstrat == numstrat)
			break;
		numstrat = newnumstrat;
		++nm;
	}
}

// figures out all gamestatus (st) of all possible board positions
// TODO maybe add a 2nd pass to handle draws
template<typename B>
void game<B>::backtrack()
{
	bool newstat;
	U32 newstatdraw,newstatlose,newstatwin;
	U32 newstatdrawm,newstatlosem,newstatwinm;
	U32 numstrat = boards.size();
	U32 firstdiff;
	con32_printf(acon,"Backtracking\n");
	winproc();
	U32 pass = 0;
	do {
		if (aborted)
			break;
		newstat = false;
		newstatdraw = newstatwin = newstatlose = 0;
		newstatdrawm = newstatwinm = newstatlosem = 0;
		firstdiff = 0;
		con32_printf(acon,"=== backtrack pass %d ===\n",pass);
		winproc();
		if (KEY==K_ESCAPE || KEY=='a' || wininfo.closerequested) {
			KEY = 0;
			aborted = 1;
			break;
		}
		U32 i;
		for (i=0;i<numstrat;++i) {
			B& bd = boards[i];
//			if (bd.st.stat != UNKNOWN && bd.st.stat != WIN) // already figured out
//			if (bd.st.stat != UNKNOWN) // already figured out
//				continue;
			// this board is unknown, let's try to figure it out
			U32 nn = bd.next.size();
#if 1
			if (!nn) // no legal moves left, let's leave it alone, it might be a lose in certain games
				continue;
#else
			if (!nn) { // no legal moves left, make it a lose
				bd.st.stat = LOSE; 
				bd.st.movesleft = 0;
				continue;
			}
#endif
			U32 j;
			// 4 rules, applied in this order
			// rule 1, any next lose is a cur win
			gamestatus ngs = {-1,UNKNOWN};
			for (j=0;j<nn;++j) { 
				B& nbdd = boards[bd.next[j]];
				if (nbdd.st.stat == LOSE) { // any next move that is a lose makes this move a win
					if (ngs.stat == UNKNOWN) {
						ngs.movesleft = nbdd.st.movesleft + 1;
					} else {
						ngs.movesleft = min(ngs.movesleft,nbdd.st.movesleft + 1); // quickest path to victory
					}
					ngs.stat = WIN;
					if (ngs.movesleft > 10000)
						error("big movesleft 1");
					//break;
				}
			}
			if (ngs.stat == WIN) {
				if (bd.st != ngs) {
					newstat |= true;
					if (bd.st.stat != ngs.stat)
						++newstatwin;
					if (bd.st.movesleft != ngs.movesleft)
						++newstatwinm;
					bd.st = ngs;
					if (!firstdiff)
						firstdiff = i;
				}
				continue; // done with this board position
			}
			// rule 2, all next win is a cur lose
			// but what if nn == 0 ???
			//bd.st.movesleft = -1;
			ngs.movesleft = -1;
			ngs.stat = UNKNOWN;
			for (j=0;j<nn;++j) { 
				B& nbdd = boards[bd.next[j]];
				if (nbdd.st.stat != WIN) { // all next moves that is a win makes this move a lose, rare
					break;
				}
				if (ngs.movesleft == -1)
					ngs.movesleft = nbdd.st.movesleft + 1;
				else
					ngs.movesleft = max(ngs.movesleft,nbdd.st.movesleft + 1); // slowest path to defeat
				if (ngs.movesleft > 10000)
					error("big movesleft 2");
			}
			if (j == nn) {
				ngs.stat = LOSE;
				if (bd.st != ngs) {
					newstat |= true;
					if (bd.st.stat != ngs.stat)
						++newstatlose;
					if (bd.st.movesleft != ngs.movesleft)
						++newstatlosem;
					bd.st = ngs;
					if (!firstdiff)
						firstdiff = i;
				}
				continue; // done with this board position
			}
			//} else
			//	bd.st.movesleft = -1; // put it back
			// rule 3, any next draw is an cur draw
			if (bd.st.stat == DRAW) { // avoid draw vicious circle
				ngs = bd.st;
			} else {
				ngs.movesleft = -1;
				ngs.stat = UNKNOWN;
			}
			for (j=0;j<nn;++j) {
				B& nbdd = boards[bd.next[j]];
				if (nbdd.st.stat == DRAW) {
					if (ngs.stat == UNKNOWN)
						ngs.movesleft = nbdd.st.movesleft + 1;
					else
						ngs.movesleft = min(ngs.movesleft,nbdd.st.movesleft + 1); // quickest path to draw
					if (ngs.movesleft > 10000)
						error("big movesleft 3");
					ngs.stat = DRAW;
				}
			}
			if (ngs.stat == DRAW) {
				if (bd.st != ngs) {
					newstat |= true;
					if (bd.st.stat != ngs.stat)
						++newstatdraw;
					if (bd.st.movesleft != ngs.movesleft)
						++newstatdrawm;
					bd.st = ngs;
					if (!firstdiff)
						firstdiff = i;
				}
				continue; // done with this board position
			}
			// rule 4, the rest is unknown
#if 0
			ngs.movesleft = -1;
			ngs.stat = DRAW;
			newstat |= bd.st != ngs;
			bd.st = ngs;
#else
			ngs.movesleft = -1;
			ngs.stat = UNKNOWN;
#endif
		}
		++pass;
		con32_printf(acon,"W %d,L %d,D %d\n",newstatwin,newstatlose,newstatdraw);
		con32_printf(acon,"Wm %d,Lm %d,Dm %d\n",newstatwinm,newstatlosem,newstatdrawm);
		con32_printf(acon,"progress %d/%d\n",firstdiff,numstrat);
	} while(newstat);
}

template<typename B>
bool game<B>::readstrategy(FILE* fr) // returns if valid strategy
{
	board rb(fr);
	if (rb == curboard) {
		con32_printf(acon,"load board stategy\n");
		perf_start(TEST8);
		U32 i,nb = filereadU32LE(fr); // get num strategies
		for (i=0;i<nb;++i) {
			board b(fr,true);
			//find_board(b,true,false); // push and hash this fileread board
			push_board(b);
			if (i%30000 == 0) {
				//con32_printfnow(acon,"load board %d/%d\n",i,nb);
				con32_printf(acon,"load board %d/%d\n",i,nb);
				winproc();
			}
		}
		perf_end(TEST8);
		con32_printf(acon,"done load board stategy\n");
		return true;
	} else {
		return false;
	}
}

template<typename B>
void game<B>::writestrategy(FILE *fw) const
{
	curboard.write(fw);
	U32 nb = boards.size();
	filewriteU32LE(fw,nb); // put num strategies
	U32 i;
	for (i=0;i<nb;++i) {
		const board& b = boards[i];//board b(fr,true);
		b.write(fw,true);
	}
}

template<typename B>
void game<B>::studygames(bool fileread,bool filewrite)
{
//	return;
//#define FILEREAD
//#define FILEWRITE
	// see if .bin strategy file exists
	bool filestrat = false;
	C8 name[100],binfilename[100];
	mgetname(levelname.c_str(),name);
	mbuildfilename(0,name,"bin",binfilename);
	logger("opening bin strat file '%s' for read\n",binfilename);
	if (fileread) {
//#ifdef FILEREAD
		if (fileexist(binfilename)) {
//#else
//	if (false) {
//#endif
			FILE* fr = fopen2(binfilename,"rb");
			logger("success, reading bin file\n");
			if (readstrategy(fr)) {
				logger("boards match, no need to write out or study game\n");
				filestrat = true;
			} else {
				logger("boards don't match, re study and re write game\n");
			}
			fclose(fr);
		} else {
			logger("failed to read strategy file\n");
		}
	}
	//filestrat = false;
#if 0
	// test hash efficiency
	S32 i;
	vector<vector<S32> >hash(4);
	for (i=0;i<3;++i)
		hash[0].push_back(i);
	for (i=0;i<5;++i)
		hash[1].push_back(i+100);
	for (i=0;i<7;++i)
		hash[2].push_back(i+200);
	for (i=0;i<11;++i)
		hash[3].push_back(i+300);
	calchasheff(hash);
#endif
	perf_start(TEST1);
	if (!filestrat) {
		reset();
		changeturn(0);
		con32_printf(acon,"Study Game\n");
		studygame();
		backtrack();
		reset();
		changeturn(initturn);
		if (aborted)
			boards.clear();
		con32_printf(acon,"finished studys\nnumstrategies = %d\n",boards.size());
	} else {
		logger("skipping study, already have a valid strategy file\n");
	}
#if 0
	U32 i,j,n;
	// show hashes
	n = board::HASHSIZE;
	for (i=0;i<n;++i)
		logger("hash %4d = %u\n",i,hashes[i].size());
#endif
	calchasheff(hashes);
	perf_end(TEST1);
#if 0
	// show everything
	U32 i,j,n;
	n = boards.size();
	for (i=0;i<n;++i) {
		const B& b = boards[i];
		//logger("board %d\n",i);
		logger("board %d, status '%s', movesleft %d\n",i,board::resultm[b.st.stat],b.st.movesleft);
		for (j=0;j<b.next.size();++j) {
			logger("\tnext = %d\n",boards[i].next[j]);
		}
		S32 j,pn = b.pieces.size();
		for (j=0;j<pn;++j) {
			const piece2& p = b.pieces[j];
			logger("  piece %d who '%c' kind '%c' x %d y %d\n",j,p.who2,p.kind,p.pos.x,p.pos.y);
		}
	}
#endif
//#ifdef FILEWRITE
	if (filewrite) {
		if (!filestrat && !aborted) {
			logger("writing strategies to bin\n");
			FILE* fw = fopen2(binfilename,"wb");
			writestrategy(fw);
			fclose(fw);
		} else {
			logger("no need to write strat bin file\n");
		}
	}
//#endif
}

template<typename B>
vector<turndata> game<B>::getcompturnstupidlist() const
{
	S32 i,x,y,n = curboard.pieces.size();
	vector<turndata> turnlist;
	if (board::bm == board::PLOP) {
		for (y=0;y<board::dim.y;++y) {
			for (x=0;x<board::dim.x;++x) {
				turndata td = {NEWPIECEIDX,pointi2b8x(x,y)};
				if (curboard.checkmove(td.pieceidx,td.dest))
					turnlist.push_back(td);
			}
		}
	} else {
		for (i=0;i<n;++i) {
			for (y=0;y<board::dim.y;++y) {
				for (x=0;x<board::dim.x;++x) {
					turndata td = {i,pointi2b8x(x,y)};
					if (curboard.checkmove(td.pieceidx,td.dest))
						turnlist.push_back(td);
				}
			}
		}
	}
	return turnlist;
}

// set stidx to -1 for no valid turn
template<typename B>
void game<B>::getcompturnstupid(S32* stidx,pointi2b8* endpos) const
{
	vector<turndata> tl = getcompturnstupidlist();
	if (tl.empty()) {
		*stidx = -1;
	} else {
		S32 p = mt_random(tl.size());
		*stidx = tl[p].pieceidx;
		*endpos = tl[p].dest;
	}
}

template<typename B>
void game<B>::getcompturn(S32* stidx,pointi2b8* endpos) const
// set stidx to -1 for no valid turn
{
//	S32 idx = find_board(curboard,false,true); // find the board in the strategy
	S32 idx = find_aboard(curboard);
	if (idx == -1) {
		getcompturnstupid(stidx,endpos);
		return;
	}
	const B& bd = boards[idx];
	S32 ml = -1;
	S32 i;
	S32 n = bd.next.size();
	vector<S32> nbdl; // list of next boards to move to
	switch(bd.st.stat) {
	case LOSE: // all next moves are wins, find highest movesleft to drag out the game
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (ml == -1)
				ml = nbd.st.movesleft;
			else
				ml = max(ml,nbd.st.movesleft);
		}
		// run through it again with ml
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (nbd.st.movesleft == ml)
				nbdl.push_back(bd.next[i]);
		}
		break;
	case DRAW:
		// next moves some draws, no loses, maybe some wins or unknowns, get to draw quickly to draw the game
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (nbd.st.stat == DRAW) {
				if (ml == -1) {
					ml = nbd.st.movesleft;
				} else {
					ml = min(ml,nbd.st.movesleft);
				}
			}
		}
		// run through it again with ml
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (nbd.st.stat == DRAW && nbd.st.movesleft == ml)
				nbdl.push_back(bd.next[i]);
		}
		break;
	case UNKNOWN:
		// next moves some unknowns, maybe some wins, no draws, unknowns don't have a movesleft
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (nbd.st.stat != WIN) {
				nbdl.push_back(bd.next[i]);
			}
		}
		break;
	case WIN: // some next moves are lose, find lowest movesleft amongst loses to quickly finish the game
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (nbd.st.stat == LOSE) {
				if (ml == -1) {
					ml = nbd.st.movesleft;
				} else {
					ml = min(ml,nbd.st.movesleft);
				}
			}
		}
		// run through it again with ml
		for (i=0;i<n;++i) {
			const board& nbd = boards[bd.next[i]];
			if (nbd.st.stat == LOSE && nbd.st.movesleft == ml)
				nbdl.push_back(bd.next[i]);
		}
		break;
	}
	// now run all valid moves against nbdl
	vector<turndata> tl = getcompturnstupidlist();
	n = tl.size();
	vector<turndata> stl; // smart turn list
	for (i=0;i<n;++i) { // filter stupid moves to smart moves using nbdl list
		B sb = curboard;
		sb.movepiece(tl[i].pieceidx,tl[i].dest);
		sb.flipturn();
		sort(sb.pieces.begin(),sb.pieces.end());
		sb = symlo(sb);
		S32 j,m = nbdl.size();
		for (j=0;j<m;++j) {
			if (sb == boards[nbdl[j]]) {
				stl.push_back(tl[i]);
			}
		}
	}
	// now have a list of smart moves
	S32 s = stl.size();
	if (!s)
		errorexit("no legal moves and also no getresult with movesleft == 0");
	S32 p = mt_random(s);
	*stidx = stl[p].pieceidx;
	*endpos = stl[p].dest;
}

template<typename B>
void game<B>::pushundo()
{
	undo<B> ud(curboard,turn);
	undos.push_back(ud);
}

template<typename B>
void game<B>::popundo()
{
	if (undos.size()) {
		const undo<B>& ud = undos.back();
		turn = ud.t;
		curboard = ud.b;
		undos.pop_back();
	}
}

template<typename B>
void game<B>::proc()
{
//	if (gamestate == GETPIECE) {
		switch (KEY) {
		case '1': // human/ai player 1
			pplayers[0] ^= 1;
			gamestate = GETPIECE;
			startpieceidx = -1;
			break;
		case '2': // human/ai player 2
			pplayers[1] ^= 1;
			gamestate = GETPIECE;
			startpieceidx = -1;
			break;
		case 'r': // reset the game and let other player go first
			reset();
			initturn ^= 1;
			changeturn(initturn);
			gamestate = GETPIECE;
			return;
		case 't':
			pushundo();
			changeturn(turn^1); // cheating
			gamestate = GETPIECE;
			return;
		case 'u':
			popundo();
			gamestate = GETPIECE;
			if (pplayers[turn] == COMPUTER)
				popundo();
			break;
		case 'h':
			hint ^= 1;
			break;
		}
//	}
// score the game if required
	gamestatus gs = curboard.getresult();
	if (gs.movesleft == 0 /*&& gs.stat != UNKNOWN*/ && gamestate == GETPIECE) {
		switch(gs.stat) {
		case LOSE:
			score[turn^1] += 1.0f;
			break;
		case DRAW:
			score[0] += .5f;
			score[1] += .5f;
			break;
		case WIN: // shouldn't happen ?
			score[turn] += 1.0f;
			break;
		}
		gamestate = ENDGAME;
		statetimer = ENDGAMETIME;
		return;
	}
// go through all the steps to move the piece
	switch (gamestate) {
	case GETPIECE:
		if (pplayers[turn] == COMPUTER) {
			pushundo();
			getcompturn(&startpieceidx,&endpiecepos);
			if (startpieceidx == NEWPIECEIDX) {
				movingpiece.kind = '*';
				movingpiece.who2 = 'W';
				startpiecepos = pointi2b8x(-2,1);
				curpiecepos = board::board2screen(startpiecepos);
				gamestate = MOVEPIECE;
				statetimer = MOVETIME;
			} else  if (startpieceidx != -1) {
				startpiecepos = curboard.getpiecepos(startpieceidx);
				if (turn == 1 && board::boardflip) {
					pointi2b8x fs(board::dim.x - 1 - startpiecepos.x,board::dim.y - 1 - startpiecepos.y);
					curpiecepos = board::board2screen(fs);
				} else {
					curpiecepos = board::board2screen(startpiecepos);
				}
				movingpiece = curboard.pieces[startpieceidx]; // copy an old piece
				gamestate = MOVEPIECE;
				statetimer = MOVETIME;
			} else { // can't find piece to move
				changeturn(turn^1);
				gamestate = ENDTURN;
				statetimer = DONETURNTIME; // can't move, skip, but change turn
			}
		} else { // HUMAN
			if (MBUT&!LMBUT) { // pick up piece
				pushundo();
				startpieceidx = curboard.findcloseidx(turn == 1 && board::boardflip);
				curpiecepos.x = MX - board::SQSIZE/2;
				curpiecepos.y = MY - board::SQSIZE/2;
				if (startpieceidx == NEWPIECEIDX) { // tenative new piece
					movingpiece.kind = '*';
					movingpiece.who2 = 'W';
					gamestate = MOVEPIECE;
				} else if (startpieceidx != -1) { // got an old piece
					movingpiece = curboard.pieces[startpieceidx]; // copy an old piece
					gamestate = MOVEPIECE;
				}
			}
		}
		break;
	case MOVEPIECE:
		if (pplayers[turn] == COMPUTER) {
			if (statetimer>0) {
				--statetimer;
				pointi2 spps;
				pointi2 epps;
				if (turn == 1 && board::boardflip) {
					pointi2b8x fs(board::dim.x - 1 - startpiecepos.x,board::dim.y - 1 - startpiecepos.y);
					pointi2b8x fe(board::dim.x - 1 - endpiecepos.x,board::dim.y - 1 - endpiecepos.y);
					spps = board::board2screen(fs);
					epps = board::board2screen(fe);
				} else {
					spps = board::board2screen(startpiecepos);
					epps = board::board2screen(endpiecepos);
				}
				curpiecepos.x = (statetimer*spps.x+(MOVETIME - statetimer)*epps.x)/MOVETIME;
				curpiecepos.y = (statetimer*spps.y+(MOVETIME - statetimer)*epps.y)/MOVETIME;
			} else {
				curboard.movepiece(startpieceidx,endpiecepos);
				statetimer = DONETURNTIME;
				gamestate = ENDTURN;
				changeturn(turn^1);
				startpieceidx = -1;
			}
		} else { // HUMAN
			curpiecepos.x = MX - board::SQSIZE/2;
			curpiecepos.y = MY - board::SQSIZE/2;
			if (!MBUT) { // place piece
				if (curboard.findcloseemptypos(&endpiecepos,turn == 1 && board::boardflip) && curboard.checkmove(startpieceidx,endpiecepos)) {
					curboard.movepiece(startpieceidx,endpiecepos);
					statetimer = DONETURNTIME;
					changeturn(turn^1);
					startpieceidx = -1;
					gamestate = ENDTURN;
				} else { // invalid move, try again
					startpieceidx = -1;
					gamestate = GETPIECE;
				}
			}
		}
		break;
	case ENDTURN:
		if (statetimer>0) {
			--statetimer;
		} else {
			gamestate = GETPIECE;
		}
		return;
	case ENDGAME:
		if (statetimer>0) {
			--statetimer;
		} else {
			reset();
			initturn ^= 1;
			changeturn(initturn);
			break;
		}
	}
}

template<typename B>
void game<B>::showhint() const
{
	vector<turndata> td = getcompturnstupidlist();
	S32 i,n = td.size();
	S32 midx = -1;
	// find piece that mouse is over
	// see if mouse over a src piece
	if (td.size() && td[0].pieceidx != NEWPIECEIDX) {
		for (i=0;i<n;++i) {
			// src
			pointi2b8 pbs = curboard.pieces[td[i].pieceidx].pos;
			if (curboard.boardflip && turn == 1) {
				pbs.x = curboard.dim.x - 1 - pbs.x;
				pbs.y = curboard.dim.y - 1 - pbs.y;
			}
			pointi2 ps = board::board2screen(pbs);
			ps.x += curboard.SQSIZE/2;
			ps.y += curboard.SQSIZE/2;
			S32 dx = MX - ps.x;
			S32 dy = MY - ps.y;
			if (dx*dx + dy*dy < curboard.SQSIZE*curboard.SQSIZE/2/2) {
				midx = td[i].pieceidx;
			}
		}
	}
	for (i=0;i<n;++i) {
		// dest
		pointi2b8 pb = td[i].dest;
		if (curboard.boardflip && turn == 1) {
			pb.x = curboard.dim.x - 1 - pb.x;
			pb.y = curboard.dim.y - 1 - pb.y;
		}
		pointi2 p = board::board2screen(pb);
		p.x += curboard.SQSIZE/2;
		p.y += curboard.SQSIZE/2;
		// plop
		if (td[i].pieceidx == NEWPIECEIDX) {
			B nb(curboard);
			nb.movepiece(NEWPIECEIDX,td[i].dest);
			nb.flipturn();
			sort(nb.pieces.begin(),nb.pieces.end());
			S32 idx = find_aboard(nb);
			if (idx >= 0) {
				gamestatus gsf = findstrat(boards[idx]).getfuture();
				if (gsf.movesleft >= 0)
					outtextxybf32(B32,p.x-4,p.y-4,C32WHITE,C32BLACK,"%s%d",board::predictmar[gsf.stat],gsf.movesleft+1);
				else
					outtextxybf32(B32,p.x-4,p.y-4,C32WHITE,C32BLACK,"%s",board::predictmar[gsf.stat]);
			} else {
				outtextxybf32(B32,p.x-4,p.y-4,C32WHITE,C32BLACK,"?");
			}
		// move
		} else {
			// src
			pointi2b8 pbs = curboard.pieces[td[i].pieceidx].pos;
			if (curboard.boardflip && turn == 1) {
				pbs.x = curboard.dim.x - 1 - pbs.x;
				pbs.y = curboard.dim.y - 1 - pbs.y;
			}
			pointi2 ps = board::board2screen(pbs);
			ps.x += curboard.SQSIZE/2;
			ps.y += curboard.SQSIZE/2;
			S32 dx = MX - ps.x;
			S32 dy = MY - ps.y;
			// dst
			if (td[i].pieceidx == midx || midx == -1) {
//			if (dx*dx + dy*dy < curboard.SQSIZE*curboard.SQSIZE/2/2) {
				clipline32(B32,ps.x,ps.y,p.x,p.y,C32RED);
				clipcircle32(B32,p.x,p.y,5,C32RED);
				B nb(curboard);
				nb.movepiece(td[i].pieceidx,td[i].dest);
				nb.flipturn();
				sort(nb.pieces.begin(),nb.pieces.end());
				S32 idx = find_aboard(nb);
				if (idx >= 0) {
					gamestatus gsf = findstrat(boards[idx]).getfuture();
					if (gsf.movesleft >= 0)
						outtextxybf32(B32,p.x-4,p.y-4,C32WHITE,C32BLACK,"%s%d",board::predictmar[gsf.stat],gsf.movesleft+1);
					else
						outtextxybf32(B32,p.x-4,p.y-4,C32WHITE,C32BLACK,"%s",board::predictmar[gsf.stat]);
				} else {
					outtextxybf32(B32,p.x-4,p.y-4,C32WHITE,C32BLACK,"?");
				}
			}
		}
	}
}

template<typename B>
void game<B>::draw() const
{
	// draw the game description
	outtextxyf32(B32,8,8,C32WHITE,"Levelname '%s', Gametype '%s'",levelname.c_str(),board::boardtype.c_str());
	// who's turn
	outtextxy32(B32,8,16,turnc[turn],turnm[turn]);
	// what player is doing
	outtextxy32(B32,8,24,C32WHITE,gamestatem[gamestate]);
	// score
	outtextxyf32(B32,8,40,C32WHITE,"Score %6.1f",score[0]);
	outtextxyf32(B32,112,40,C32BLACK,"%6.1f",score[1]);
#if 1
	if (gamestate==ENDGAME) {
		// draw endgame message
		gamestatus gsn = curboard.getresult();
		if (gsn.stat == DRAW)
			outtextxy32(B32,8,32,C32WHITE,"Game results: DRAW");
		else
			outtextxyf32(B32,8,32,C32WHITE,"Game results: %s %s",playerm[turn],board::resultm[gsn.stat]);
	} else {
		// who's winning, moves left
		gamestatus gsf = findstrat(curboard).getfuture();
		outtextxyf32(B32,8,32,C32WHITE,"Future is '%s', Moves left %d",board::predictm[gsf.stat],gsf.movesleft);
	}
	// draw the console near the bottom
#endif
	bitmap32* cbm=con32_getbitmap32(acon);
	clipblit32(cbm,B32,0,0,20,WY-32-cbm->size.y,cbm->size.x,cbm->size.y);
	// draw the instructions at the bottom
	outtextxyf32(B32,30,WY - 24,C32WHITE,"'r' reset,  'a' abort solve,  '=' levelup,  '-' leveldown, 'u' undo, 'h' hint %s",hint?"ON":"OFF");
	// show players type (human/computer) at the bottom
	outtextxyf32(B32,30,WY - 16,C32WHITE,"'1' Change player 1 (%s), '2' Change player 2 (%s), undos (%d)",
	  playertype[pplayers[0]],playertype[pplayers[1]],undos.size());
	// draw the current board
	if (turn == 1) { // black
		B flipboard = curboard;
		flipboard.flipturn();
		//sort(flipboard.pieces.begin(),flipboard.pieces.end()); // don't sort
		movingpiece.who2 = board::flipcolor(movingpiece.who2);
		flipboard.draw(startpieceidx,movingpiece,gamestate==MOVEPIECE,&curpiecepos);
		movingpiece.who2 = board::flipcolor(movingpiece.who2);
	} else {
		curboard.draw(startpieceidx,movingpiece,gamestate==MOVEPIECE,&curpiecepos);
	}
	// show hints
	if (hint && gamestate == GETPIECE && pplayers[turn] == HUMAN) {
		showhint();
	}
}
