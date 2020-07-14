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
	U32 width, height;
	// accumulated results
	vector<vector<cellInfo>> know;
	// result
	S32 bestValue;
	vector<vector<S32>> bestPaths; // all the best paths
public:
	lowCostPath(const vector<vector<S32>>& board);
	S32 getBestValue() const;
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
	if (height <= 1)
		return false;
	S32 curCol = path[1];
	U32 curRow = 1;
	while (true) {
		const cellInfo ci = know[curRow][curCol];
		S32& dir = dirs[curRow];
		// find next dir, if not move up the chain
		while (true) {
			++dir;
			if (dir == NUM_DIR) {
				++curRow; // exhausted this path, move up a level in the chain
				if (curRow == height)
					return false; // no more paths left
				curCol = path[curRow];
				break;
			} else { // switch over to another path
				if (!ci.dirs[dir])
					continue; // skip invalid dir
				setupFork(curCol, curRow, dirs, path, true);
				return true;
			}
		}
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
	height = board.size();
	width = board[0].size();
	for (U32 j = 1; j < height; ++j) {
		if (width != board[j].size()) {
			logger("bad board!!\n");
			bestValue = 0;
			return;
		}
	}

	// build up accumulated info from each row
	know = vector<vector<cellInfo>>(height);
	know[0] = buildFirstRow(board[0]);
	for (U32 j = 1; j < height; ++j) { // pick a row
		const auto row = board[j];
		know[j] = calcNext(know[j-1], row);
	}

	// best value is lowest number in last row
	// calc best value, no walking needed
	bestValue = INT_MAX;
	const vector<cellInfo>& bestValues = know[height - 1];
	for (U32 i = 0; i < width; ++i) {
		const cellInfo& val = bestValues[i];
		if (val.bestSum < bestValue) {
			bestValue = val.bestSum;
		}
	}

	vector<S32> directions(height,NUM_DIR); // iterate through this list of directions
	vector<S32> aBestPath = vector<S32>(height);
	// now build some best paths, some roots at the bottom
	for (U32 i = 0; i < width; ++i) {
		if (bestValues[i].bestSum != bestValue) {
			continue; // not a bestValue
		}
		S32 curCol = i;
		S32 curRow = height - 1;
		// a good root is here
		setupFork(curCol, curRow, directions, aBestPath, false);
		logger("");
		bestPaths.push_back(aBestPath);

		while (doNext(directions, aBestPath)) {
			bestPaths.push_back(aBestPath);
		}
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
	vector<cellInfo> ret = vector<cellInfo>(width);
	for (U32 i = 0; i < width; ++i) {
		ret[i].bestSum = first[i];
	}
	return ret;
}

vector<lowCostPath::cellInfo> lowCostPath::calcNext(const vector<cellInfo>& acc, const vector<S32>& next) const
{
	vector<cellInfo> ret = vector<cellInfo>(width);
	// find shortest path between acc and next
	// walk through next and add acc left,center,right finding least value
	for (S32 i = 0; i < (signed)width; ++i) {
		S32 least = INT_MAX;
		cellInfo& node = ret[i];
		// find least sum from 3 directions
		for (S32 k = -1; k <= 1; ++k) {
			S32 sum = next[i];
			S32 idx = k + i;
			if (idx >= 0 && idx < (signed)width) {
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
			if (idx >= 0 && idx < (signed)width) {
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
#if 1 // should be 1,1 2 in all (2)
		{
			{1,2,1},
			{2,1,2},
		},
#endif
#if 1
		{ // should be (4) in 2 ways
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

		// real cases (69)
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
		{ // 1,5,6,7,6,5,1 9 in all (20)
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
		{ // (7) , 5 different paths
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
