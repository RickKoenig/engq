#define LADYBUG2
S64 ladyCount;
const S64 numSteps =       100'000'000L;
const S64 maxLadyCount =   700'000'000L;
//const S64 numSteps = 1'000'000'000L;
//const S64 maxLadyCount = 7'000'000'000L;
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
	board(S32 numLeaves, S32 maxTokens) :
		numLeaves(numLeaves), maxTokens(maxTokens)
		, numTokens(0), leaf(0), leaves(numLeaves)
	{
	}

	// print a 2d vector of S32
	void printBoard() {
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
				ss << ",";
			}
		}
		con32_printf(ladyCon, "%s", ss.str().c_str());
		con32_printf(ladyCon, ">\n");
#endif 
#define SHOW_TOKENPOS
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
	bool nextPos() {
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
			if (numTokens == 0) {
				return false;
			}
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
		//console.log("done sequence");
		return false;
	}
/*
	// check validity before calling, 'leaf' is reset to 0 for next.numTokens placment
	#addToken() {
		this.leaves[this.leaf].push(++this.numTokens);
		//this.tokenPos[this.numTokens] = this.leaf;
		this.tokenPos.push(this.leaf);
		this.leaf = 0;
	}

	// 'leaf' is where.numTokens was removed from
	#removeToken() {
		this.leaf = this.tokenPos.pop();
		--this.numTokens;
		//this.leaf = this.tokenPos[this.numTokens];
		//this.tokenPos[this.numTokens--] = null;
		this.leaves[this.leaf].pop();
	}
*/
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
		//return leaves[leaf].length < 2; // can't be placed if 2 or more leafs already present
		const S32 newToken = numTokens + 1;
		return !doSumCheck(leaves[leaf], newToken); // leaf can't be sum of 2 other leaves
	}
};

void doLadybug() {
	con32_printf(ladyCon, "doing ladybug puzzle\n");
	const S32 minLeaves = 1;
	const S32 maxLeaves = 3;
	const S32 maxTokens = 1000;
	S32 maxTokensPlayed = 0;
	for (S32 numLeaves = minLeaves; numLeaves <= maxLeaves; ++numLeaves) {
		con32_printf(ladyCon, "\nNUM LEAVES = %d\n", numLeaves);
		board* game = new board(numLeaves, maxTokens);
		S64 outerWatch = 0L;
		const S64 maxOuterWatch = 20'000'000'000L;
		do {
			if (game->numTokens > maxTokensPlayed) {
				maxTokensPlayed = game->numTokens;
				con32_printf(ladyCon, "maxTokensPlayed now at %d\n", maxTokensPlayed);
			}
			//if (true) {
			if (false) {
			//if (outerWatch % 20000000 == 0) {
			//if (game.numTokens >= maxTokensPlayed) {
				con32_printf(ladyCon, "\nouterWatch now at %lld\n", outerWatch);
				game->printBoard();
			}
			++outerWatch;
			if (outerWatch >= maxOuterWatch) {
				con32_printf(ladyCon, "outer watch hit\n");
				break;
			}
		} while (game->nextPos());
		con32_printf(ladyCon, "total positions = %lld\n", outerWatch);
		delete game;
	}
	con32_printf(ladyCon, "\ndone ladybug puzzle\n");
}


void stepLadybug2() {
	con32_printf(ladyCon, "starting ladybug puzzle");
	const S32 numLeaves = 1;
	const S32 maxTokens = 1'000;
	S32 maxTokensPlayed = 0;
	S64 outerWatch = 0L;
	const S64 maxOuterWatch = 20'000'000L;
	con32_printf(ladyCon, "\nNUM LEAVES = " + numLeaves);
	board* game = new board(numLeaves, maxTokens);
	do {
		if (game->numTokens > maxTokensPlayed) {
			maxTokensPlayed = game->numTokens;
			con32_printf(ladyCon, "maxTokensPlayed now at " + maxTokensPlayed);
		}
		if (outerWatch % 20'000'000 == 0) {
			//if (game.numTokens >= maxTokensPlayed) {
			con32_printf(ladyCon, "outerWatch now at " + outerWatch);
			game->printBoard();
		}
		++outerWatch;
		if (outerWatch >= maxOuterWatch) {
			con32_printf(ladyCon, "outer watch hit");
			break;
		}
	} while (game->nextPos());
	con32_printf(ladyCon, "total positions = " + outerWatch);
	con32_printf(ladyCon, "\ndone ladybug puzzle");
	delete game;
}

// return should keep going, opposite of done
enum stepRet { STEP_CONTINUE, STEP_DONE, STEP_ABORT };
stepRet stepladybug1() {
	++ladyCount;
	if ((ladyCount % numSteps) == 0) {
		winproc();
	}
	switch (KEY) {
	case 'p':
		con32_printf(ladyCon, "steplaybug1 pause, go back to main loop\n");
		return STEP_ABORT;
		break;
	}
	return ladyCount < maxLadyCount ? STEP_CONTINUE : STEP_DONE;
}

void initladybug() {
	logger("--- init ladybug ---\n");
	ladyCon = con32_alloc(600, 400, C32WHITE, C32BLACK);
	con32_printf(ladyCon, "init ladybug\n");
	con32_printf(ladyCon, "sizeof ladyCount = %d\n", sizeof ladyCount);
	ladyCount = 0L;
	running = false;
	done = false;
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
		ladyCount = 0L;
		break;
	}
	if (running) {
		if (done) {
			con32_printf(ladyCon, "done !, ladyCount = %lld\n", ladyCount);
			running = false;
		} else {
			while (true) {
				stepRet ret = stepladybug1();
				switch (ret) {
				case STEP_DONE:
					//con32_printf(ladyCon, "done");
					done = true;
					return;
					break;
				case STEP_CONTINUE:
					break;
				case STEP_ABORT:
					running = false;
					return;
					break;
				}
				if ((ladyCount % numSteps) == 0) {
					con32_printf(ladyCon, "ladyCount = %lld\n", ladyCount);
				}

			}
			done = true;
			//con32_printf(ladyCon, "count = %lld\n", ladyCount);
		}
	}
}

void drawladybug()
{
	//logger("draw ladybug\n");
	bitmap32* cbm = con32_getbitmap32(ladyCon);
	clipblit32(cbm, B32, 0, 0, 20, WY - 32 - cbm->size.y, cbm->size.x, cbm->size.y);
	outtextxyb32(B32, 30, 30, C32WHITE, C32BLACK, "press 'c' to start / continue");
	outtextxyb32(B32, 30, 38, C32WHITE, C32BLACK, "press 'p' to pause");
	outtextxyb32(B32, 30, 46, C32WHITE, C32BLACK, "press 'r' to reset");
}

void exitladybug() {
	logger("--- exit ladybug ---\n");
	con32_free(ladyCon);
}

void initladybug2() {
	logger("--- init ladybug2 ---\n");
	ladyCon = con32_alloc(600, 400, C32WHITE, C32BLACK);
	con32_printf(ladyCon, "init ladybug2\n");
	con32_printf(ladyCon, "sizeof ladyCount = %d\n", sizeof ladyCount);
	doLadybug();
	ladyCount = 0L;
	running = false;
	done = false;
}

void procladybug2() {
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
		ladyCount = 0L;
		break;
	}
	if (running) {
		if (done) {
			con32_printf(ladyCon, "done !, ladyCount = %lld\n", ladyCount);
			running = false;
		} else {
			while (true) {
				stepRet ret = stepladybug1();
				switch (ret) {
				case STEP_DONE:
					//con32_printf(ladyCon, "done");
					done = true;
					return;
					break;
				case STEP_CONTINUE:
					break;
				case STEP_ABORT:
					running = false;
					return;
					break;
				}
				if ((ladyCount % numSteps) == 0) {
					con32_printf(ladyCon, "ladyCount = %lld\n", ladyCount);
				}

			}
			done = true;
			//con32_printf(ladyCon, "count = %lld\n", ladyCount);
		}
	}
}

void drawladybug2()
{
	//logger("draw ladybug\n");
	bitmap32* cbm = con32_getbitmap32(ladyCon);
	clipblit32(cbm, B32, 0, 0, 20, WY - 32 - cbm->size.y, cbm->size.x, cbm->size.y);
	outtextxyb32(B32, 30, 30, C32WHITE, C32BLACK, "press 'c' to start / continue");
	outtextxyb32(B32, 30, 38, C32WHITE, C32BLACK, "press 'p' to pause");
	outtextxyb32(B32, 30, 46, C32WHITE, C32BLACK, "press 'r' to reset");
}

void exitladybug2() {
	logger("--- exit ladybug2 ---\n");
	con32_free(ladyCon);
}
