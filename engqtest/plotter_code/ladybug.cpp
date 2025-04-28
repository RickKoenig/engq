#include <Windows.h>

#define LADYBUG
con32* ladyCon;
bool running;
bool done;

// return true if found sum, false otherwise
// assumes ascending array
bool doSumCheck(vector<S32>& arr, S32 target) {
	const S32 len = arr.size();
	if (len < 2) {
		//console.log("too few elements");
		return false;
	}
	if (arr[0] * 2 > target) {
		//console.log("first element too big");
		return false;
	}
	if (arr[len - 1] * 2 < target) {
		//console.log("last element too small");
		return false;
	}
	S32 left = 0;
	S32 right = len - 1;
	while (left < right) {
		const S32 sum = arr[left] + arr[right];
		if (sum > target) {
			--right;
		} else if (sum < target) {
			++left;
		} else {
			//console.log("sum found");
			return true;
		}
	}
	//console.log("no sum found");
	return false;
}

class board {
public:
	S32 numLeaves;
	S32 maxTokens;
	S32 numTokens; // number of tokens on the board(played), same as last.numTokens played
	S32 leaf; // leaf where the next.numTokens will be placed
	vector<S32> tokenPos; // in.numTokens, out leaf
	vector<vector<S32>> leaves; // each leaf is an array of tokens
	S64 ladyCount;
	S32 maxTokensPlayed;
	board(S32 numLeaves, S32 maxTokens) :
		numLeaves(numLeaves), maxTokens(maxTokens)
		, numTokens(0), leaf(0), leaves(numLeaves), ladyCount(0)
		, maxTokensPlayed(0)
	{
	}

	// print a 2d vector of S32
	void printBoard() {
		con32_printf(ladyCon, "ladyCount = %lld\n", ladyCount);
		stringstream ss;
#define SHOW_LEAVES
#ifdef SHOW_LEAVES
		U32 j;
		for (j = 0; j < leaves.size(); ++j) {
			const vector<S32> leaf = leaves[j]; // do first leaf for now TODO other leaves
			ss << "[";
			U32 i;
			for (i = 0; i + 1 < leaf.size(); ++i) {
				ss << leaf[i];
				ss << ",";
			}
			if (i < leaf.size()) {
				ss << leaf[i];
			}
			ss << "]";
			if (j + 1 < leaves.size()) {
				ss << "\n";
			}
		}
		con32_printf(ladyCon, "%s\n", ss.str().c_str());
#endif 
//#define SHOW_TOKENPOS
#ifdef SHOW_TOKENPOS
		ss.str("");
		ss.clear();
		ss << "[";
		U32 i;
		for (i = 0; i + 1 < tokenPos.size(); ++i) {
			ss << tokenPos[i];
			ss << ",";
		}
		if (i < tokenPos.size()) {
			ss << tokenPos[i];
		}
		ss << "]";
		con32_printf(ladyCon, "%s\n", ss.str().c_str());
#endif
	}

	// depth first iteration for now, tricky
	// return true if keep going
	bool nextPos() {
		//printBoard();
		if (numTokens > maxTokensPlayed) {
			maxTokensPlayed = numTokens;
			con32_printf(ladyCon, "\nmaxTokensPlayed now at %d\n", maxTokensPlayed);
			printBoard();
		}
		//Sleep(200);
		++ladyCount;
		// try to go deeper
		if (numTokens < maxTokens) {
			if (checkToken()) {
				addToken();
				return true;
			} else {
				addToken(); // but move across
			}
		}
		S32 innerWatch = 0;
		while (numTokens > 0) {
			// watchdog
			++innerWatch;
			const S32 maxWatch = 400;
			if (innerWatch >= maxWatch) {
				con32_printf(ladyCon, "inner watch hit !!");
				return false;
			}
			// try to move.numTokens across
			removeToken();
			++leaf;
			if (leaf < numLeaves) {
				// no redundant cases, don't play if leaf to the left is empty
				if (leaves[leaf - 1].size() > 0) {
					//if (true) {
					if (checkToken()) {
						addToken(); // good move
						return true;
					} else {
						addToken(); // bad move, move across
						continue;
					}
				} else {
					continue;
				}
			}
			// try to go back up, then move across
			//console.log("going back up");
			continue;
		}
		con32_printf(ladyCon, "done sequence for numleaves = %d, maxTokensPlayed %d\n", numLeaves, maxTokensPlayed);
		//printBoard();
		return false;
	}
private:
	// check validity before calling, 'leaf' is reset to 0 for next.numTokens placment
	void addToken() {
		leaves[leaf].push_back(++numTokens);
		tokenPos.push_back(leaf);
		leaf = 0;
	}

	// 'leaf' is where.numTokens was removed from
	void removeToken() {
		leaf = tokenPos.back();
		--numTokens;
		tokenPos.pop_back();
		leaves[leaf].pop_back();
	}

	// make sure new.numTokens is not the sum of any other tokens on this leaf
	// return true if allowed
	bool checkToken() {
		//return true; // always good
		//return leaves[leaf].size() < 2; // can't be placed if 2 or more leafs already present
		const S32 newToken = numTokens + 1;
		return !doSumCheck(leaves[leaf], newToken); // leaf can't be sum of 2 other leaves
	}
};

enum stepRet { STEP_CONTINUE, STEP_DONE, STEP_ABORT };

class simLadybug {
	const S32 minLeaves;
	const S32 maxLeaves;
	const S32 maxTokens;
	const S32 maxTokensPlayed;
	S32 numLeaves;
	board* game;
public:
	simLadybug(S32 minLeaves, S32 maxLeaves, S32 maxTokens) : minLeaves(minLeaves)
		, maxLeaves(maxLeaves), maxTokens(maxTokens), maxTokensPlayed(0), numLeaves(minLeaves) {
		game = new board(numLeaves, maxTokens);
		con32_printf(ladyCon ,"\nNUM LEAVES = %d\n", numLeaves);
	}

	stepRet step(S64* lc, S32 numSteps) {
		winproc();
		if (KEY == 'p') {
			*lc = game->ladyCount;
			return STEP_ABORT;
		}
		con32_printf(ladyCon, "STEP ladyCount = %16lld\n", game->ladyCount);
		while (numSteps--) {
			bool go = game->nextPos();
			if (go) {
				*lc = game->ladyCount;
				continue;
				//return STEP_CONTINUE; // keep going
			}
			con32_printf(ladyCon, "\nfinal ladyCount = %lld\n", game->ladyCount);
			if (numLeaves == maxLeaves) {
				numLeaves = minLeaves;
				delete game;
				game = new board(numLeaves, maxTokens);
				return STEP_DONE;
			}
			++numLeaves;
			con32_printf(ladyCon, "\nNUM LEAVES = %d\n", numLeaves);
			delete game;
			game = new board(numLeaves, maxTokens);
			return STEP_CONTINUE;
		}
		return STEP_CONTINUE;
	}

	void reset() {
		numLeaves = minLeaves;
		delete game;
		game = new board(numLeaves, maxTokens);
	}

	~simLadybug() {
		delete game;
	}
};

simLadybug* sim;

void initladybug() {
	logger("--- init ladybug2 ---\n");
	ladyCon = con32_alloc(600, 400, C32WHITE, C32BLACK);
	con32_printf(ladyCon, "init ladybug2\n");
	running = false;
	done = false;
	const S32 minLeaves = 1;
	const S32 maxLeaves = 4;
	const S32 maxTokens = 1000;
	sim = new simLadybug(minLeaves, maxLeaves, maxTokens);
}

void procladybug() {
	//logger("proc ladybug\n");
	switch (KEY) {
	case 'c':
		con32_printf(ladyCon, "running\n");
		running = true;
		break;
	case 'p':
		con32_printf(ladyCon, "paused\n");
		running = false;
		break;
	case 'r':
		con32_printf(ladyCon, "reset\n");
		running = false;
		done = false;
		sim->reset();
		break;
	}
	while (running && !done) {
		S64 lc; // lady count
		const S32 numSteps = 100'000'000;
		stepRet ret = sim->step(&lc, numSteps);
		switch (ret) {
		case STEP_DONE:
			con32_printf(ladyCon, "ALL done !\n");
			done = true;
			running = false;
			//ladyCount = 0;
			break;
		case STEP_CONTINUE:
			break;
		case STEP_ABORT:
			con32_printf(ladyCon, "paused !, ladyCount = %lld\n", lc);
			running = false;
			break;
		}
	}
}

void drawladybug()
{
	bitmap32* cbm = con32_getbitmap32(ladyCon);
	clipblit32(cbm, B32, 0, 0, 20, WY - 32 - cbm->size.y, cbm->size.x, cbm->size.y);
	outtextxyb32(B32, 30, 30, C32WHITE, C32BLACK, "press 'c' to start / continue");
	outtextxyb32(B32, 30, 38, C32WHITE, C32BLACK, "press 'p' to pause");
	outtextxyb32(B32, 30, 46, C32WHITE, C32BLACK, "press 'r' to reset");
}

void exitladybug() {
	logger("--- exit ladybug2 ---\n");
	delete sim;
	con32_free(ladyCon);
}
