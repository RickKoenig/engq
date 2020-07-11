#include <m_eng.h>
// move from top to bottom, can move down, down left, down right
// find best path with min sum,
// 1) just the sum of best path DONE
// 2) just one best path DONE
// 3) all best paths DONE

class lowCostPath {
	// directions
	enum DIR {
		DOWN_LEFT, DOWN, DOWN_RIGHT, NUM_DIR
	};
	// cumulative
	struct cellInfo {
		S32 bestSum;
		bool dirs[NUM_DIR];
	};
	// accumulated results
	vector<vector<cellInfo>> know;
	// result
	S32 bestValue;
	vector<vector<S32>> bestPaths; // all the best paths
public:
	lowCostPath(const vector<vector<S32>>& board);
	S32 getBestValue() const;
	const vector<S32> getABestPath() const;
	const vector<vector<S32>> getBestPaths() const;
private:
	vector<cellInfo> buildFirstRow(const vector<S32>& first) const;
	vector<cellInfo> calcNext(const vector<cellInfo>& acc, const vector<S32>& next) const;
	void setupFork(S32 col, S32 row, vector<S32>& dirs, vector<S32>& path, bool useFirstDirs) const;
	bool doNext(vector<S32>& dirs, vector<S32>& path) const;
};

// now going in another direction, setup sub path, to the left most
void lowCostPath::setupFork(S32 col, S32 row, vector<S32>& dirs, vector<S32>& path, bool useFirstDirs) const
{
	S32 firstRow = row;
	while (row > 0) {
		const cellInfo& ci = know[row][col];
		S32 k;
		for (k = 0; k < NUM_DIR; ++k) {
			if (ci.dirs[k])
				break;
		}
		path[row] = col;
		if (!useFirstDirs || row != firstRow) {
			dirs[row] = DIR(k);
		} else {
			k = dirs[row];
		}
		col += k - 1;
		--row;
	}
	dirs[0] = DOWN;
	path[0] = col;
}

// move to next best path if available
bool lowCostPath::doNext(vector<S32>& dirs, vector<S32>& path) const
{
	if (know.size() <= 1)
		return false;
	S32 curCol = path[1];
	U32 curRow = 1;
	//while (curRow < S32(know.size())) {
	while (true) {
		const cellInfo ci = know[curRow][curCol];
		S32& dir = dirs[curRow];
		// find next dir, if not move up the chain
		while (true) {
			++dir;
			if (dir == NUM_DIR) {
				++curRow; // exhausted this path, move up a level in the chain
				if (curRow == know.size())
					return false; // no more paths left
				curCol = path[curRow];
				//cellInfo ci = know[curRow][curCol];
				break;
			} else { // switch over to another path
				if (!ci.dirs[dir])
					continue; // skip invalid dir
				setupFork(curCol, curRow, dirs, path, true);
				return true;
			}
		}
		//logger("fork");
		//break;
	}
	logger("never");
	return false;
}

lowCostPath::lowCostPath(const vector<vector<S32>>& board)
{
	// nothing to do!
	if (board.empty() || board[0].empty()) {
		bestValue = 0;
		return;
	}
	// make sure is a proper 2d array
	U32 wid = board[0].size();
	for (U32 j = 1; j < board.size(); ++j) {
		if (wid != board[j].size()) {
			logger("bad board!!\n");
			bestValue = 0;
			return;
		}
	}

	// build up accumulated info from each row
	know = vector<vector<cellInfo>>(board.size());
	know[0] = buildFirstRow(board[0]);
	for (U32 j = 1; j < board.size(); ++j) { // pick a row
		const auto row = board[j];
		know[j] = calcNext(know[j-1], row);
	}

	// best value is lowest number in last row
	// calc best value, no walking needed
	bestValue = INT_MAX;
	const vector<cellInfo>& bestValues = know[board.size() - 1];
	for (U32 i = 0; i < bestValues.size(); ++i) {
		const cellInfo& val = bestValues[i];
		if (val.bestSum < bestValue) {
			bestValue = val.bestSum;
		}
	}

	vector<S32> directions(know.size(),NUM_DIR); // iterate through this list of directions
	vector<S32> aBestPath = vector<S32>(know.size());
	// now build some best paths, some roots at the bottom
	for (U32 i = 0; i < bestValues.size(); ++i) {
		if (bestValues[i].bestSum != bestValue) {
			continue; // not a bestValue
		}
		S32 curCol = i;
		S32 curRow = know.size() - 1;
		// a good root is here
		// special test, REMOVE soon
		//directions[1] = DOWN_RIGHT;
		//setupFork(curCol, curRow, directions, aBestPath, true);
		setupFork(curCol, curRow, directions, aBestPath, false);
		logger("");
		bestPaths.push_back(aBestPath);

		while (doNext(directions, aBestPath)) {
			bestPaths.push_back(aBestPath);
		}
#if 0
		S32 curCol = i;
		vector<S32> aBestPath = vector<S32>(know.size());
		// now do multiple nested loops with 'directions'
		// going backwards
		// initialize directions that work 'are true'
		for (U32 j = know.size() - 1; j != 0; --j) {
			const vector<cellInfo>& knowRow = know[j];
			const cellInfo& ci = knowRow[curCol];
			aBestPath[j] = curCol;
			// find first true direction, could be 1 to 3 of them
			U32 idx = 0;
			for (U32 k = 0; k < NUM_DIR; ++k) { // tend to the left
				if (ci.dirs[k]) {
					idx = k;
					break;
				}
			}
			directions[j] = idx;
			curCol += idx - 1;
		}
		aBestPath[0] = curCol;
		bestPaths.push_back(aBestPath); // save left most path
		S32 curRow = 1;
		curCol = aBestPath[curRow];
		// iterate paths
		logger("");
		while (curRow < 2) {
			++directions[j];
			const vector<cellInfo>& knowRow = know[curRow];
			const cellInfo& ci = knowRow[curCol];
			bool dir = ci.dirs[directions[j]];
			if ()
			if (directions[i] == NUM_DIR) {
				directions[i] = DOWN_LEFT;
				++curRow;
			}
		}
#if 0
		for (U32 j = know.size() - 1; j != 0; --j) {
			const vector<cellInfo>& knowRow = know[j];
			const cellInfo& ci = knowRow[curCol];
			aBestPath[j] = curCol;
			// find first/last true direction, could be 1 to 3 of them
			U32 idx = 0;
			for (U32 k = 0; k < NUM_DIR; ++k) { // tend to the left
			//for (S32 k = NUM_DIR - 1; k >= 0; --k) { // tend to the right
				if (ci.dirs[k]) {
					idx = k;
					break;
				}
			}
			curCol += idx- 1;
		}
		aBestPath[0] = curCol;
		bestPaths.push_back(aBestPath);
#endif
#endif
	}
}

S32 lowCostPath::getBestValue() const
{
	return bestValue;
}

const vector<vector<S32>> lowCostPath::getBestPaths() const
{
	return bestPaths;
}

vector<lowCostPath::cellInfo> lowCostPath::buildFirstRow(const vector<S32>& first) const
{
	vector<cellInfo> ret = vector<cellInfo>(first.size());
	for (U32 i = 0; i < first.size(); ++i) {
		ret[i].bestSum = first[i];
	}
	return ret;
}

vector<lowCostPath::cellInfo> lowCostPath::calcNext(const vector<cellInfo>& acc, const vector<S32>& next) const
{
	vector<cellInfo> ret = vector<cellInfo>(acc.size());
	// find shortest path between acc and next
	// walk through next and add acc left,center,right finding least value
	for (S32 i = 0; i < (signed)acc.size(); ++i) {
		S32 least = INT_MAX;
		cellInfo& node = ret[i];
		// find least sum from 3 directions
		for (S32 k = -1; k <= 1; ++k) {
			S32 sum = next[i];
			S32 idx = k + i;
			if (idx >= 0 && idx < (signed)acc.size()) {
				sum += acc[idx].bestSum;
				if (sum < least) {
					least = sum;
				}
			}
		}
		// set dirs for least sum, can set 1 to 3 'dirs'
		for (S32 k = -1; k <= 1; ++k) {
			S32 sum = next[i];
			S32 idx = k + i;
			if (idx >= 0 && idx < (signed)acc.size()) {
				sum += acc[idx].bestSum;
				if (sum == least) {
					node.dirs[k + 1] = true;
				}
			}
		}
		node.bestSum = least;
	}
	return ret;
}

// test lowCostPath
void do_dynamic()
{
	vector<vector<vector<S32>>> boards{
#if 1
		{
			{1,2,1},
			{2,1,2},
		},
#endif
#if 1
		{
			{1,2,1},
			{1,2,1},
			{2,1,2},
			{2,1,2},
		},
#endif
#if 1
		// degenerate cases
		{ // (0)
		},
		{ // (0)
			{},
			{},
			{},
		},
		{ // bad board, (0)
			{34},
			{},
		},

		// real cases
		{ // (69)
			{69},
		},
		{ // should be 3,6 (9)
			{3,4,5},
			{6,7,8},
		},
		{ // all paths, 17 in all (3)
			{1,1,1},
			{1,1,1},
			{1,1,1},
		},
#endif
#if 1
		{ // all paths, 68 in all (4)
			{1,1,1,1},
			{1,1,1,1},
			{1,1,1,1},
			{1,1,1,1},
		},
#endif
#if 1
		{ // 5,6,7,6,5 (29)
			{1,1,1,1,1},
			{3,4,5,6,7},
			{4,5,6,7,3},
			{5,6,7,3,4},
			{6,7,3,4,5},
			{7,3,4,5,6},
			{1,1,1,1,1},
		},
#endif
#if 1
		{ // 5,6,7,6,5 (29)
			{30, 4, 5, 6, 7},
			{ 3, 4, 5, 6,70},
			{93,40,50,60, 7},
			{ 3,14, 5, 6,17},
			{ 3, 4, 5,16, 7},
		},
#endif
#if 1
		{ // 7 , 5 different paths
			{8,8,1,8,8},
			{8,1,8,1,8},
			{8,8,1,8,1},
			{8,1,8,8,1},
			{8,1,8,1,8},
			{1,8,1,8,8},
			{8,1,8,8,8},
		},
#endif
	};
	logger("========= START do dynamic programming, minimize cost =======\n");
	for (const auto& board : boards) { // pick a board
		logger("\n===========\n");
		lowCostPath lcp(board);
		S32 bestValue = lcp.getBestValue();
		logger("best value = %d\n", bestValue);
		const vector<vector<S32>> bestPaths = lcp.getBestPaths();
		logger("num paths = %d\n", bestPaths.size());
		for (auto bp : bestPaths) {
			logger("[");
			for (auto v : bp) {
				logger("%d", v);
			}
			logger("]\n");
		}
	}
	logger("========= END do dynamic programming, minimize cost =======\n");
}
