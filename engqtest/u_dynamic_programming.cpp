#include <m_eng.h>
// move from top to bottom, can move down, down left, down right
// find best path with min sum, for now just one best path
// or maybe easier just print the min value

enum DIR {
	DOWN_LEFT,DOWN,DOWN_RIGHT,NUM
};

struct rowInfo {
	S32 bestSum;
	bool dirs[NUM];
};

#if 1
vector<rowInfo> buildFirstRow(const vector<S32>& first) {
	vector<rowInfo> ret = vector<rowInfo>(first.size());
	for (U32 i = 0; i < first.size(); ++i) {
		ret[i].bestSum = first[i];
	}
	return ret;
}
#endif

vector<rowInfo> calcNext(const vector<rowInfo>& acc, vector<S32> next)
{
	if (acc.empty())
		return buildFirstRow(next);
	vector<rowInfo> ret = vector<rowInfo>(acc.size());
	// find shortest path between acc and next
	S32 i,j;
	// walk through next and add acc left,center,right finding least value
	for (j = 0; j < (signed)acc.size(); ++j) {
		S32 least = INT_MAX;
		rowInfo& node = ret[j];
		// find least sum from 3 directions
		for (i = -1; i <= 1; ++i) {
			S32 sum = next[j];
			S32 idx = j + i;
			if (idx >= 0 && idx < (signed)acc.size()) {
				sum += acc[idx].bestSum;
				if (sum < least) {
					least = sum;
				}
			}
		}
		// set dirs for least sum, can set 1 to 3 'dirs'
		for (i = -1; i <= 1; ++i) {
			S32 sum = next[j];
			S32 idx = j + i;
			if (idx >= 0 && idx < (signed)acc.size()) {
				sum += acc[idx].bestSum;
				if (sum == least) {
					node.dirs[i + 1] = true;
				}
			}
		}
		node.bestSum = least;
	}
	return ret;
}

void do_dynamic()
{
	vector<vector<vector<S32>>> boards{
		{ // should be 3,6 (9)
			{3,4,5},
			{6,7,8},
		},
		{ // all paths, 17 in all (3)
			{1,1,1},
			{1,1,1},
			{1,1,1},
		},
		{ // we'll see
			{ 3, 4, 5, 6, 7},
			{ 4, 5, 6, 7, 3},
			{ 5, 6, 7, 3, 4},
			{ 6, 7, 3, 4, 5},
			{ 7, 3, 4, 5, 6},
		},
		{ // 5,6,7,6,5 (29)
			{30, 4, 5, 6, 7},
			{ 3, 4, 5, 6,70},
			{93,40,50,60, 7},
			{ 3,14, 5, 6,17},
			{ 3, 4, 5,16, 7},
		},
	};
	logger("========= START do dynamic programming, minimize cost =======\n");
	for (auto board : boards) { // pick a board
		vector<rowInfo> bestValues;
		for (auto row : board) { // pick a row
			bestValues = calcNext(bestValues, row);
		}
		S32 best = INT_MAX;
		for (auto i : bestValues) {
			if (i.bestSum < best)
				best = i.bestSum;
		}
		logger("best value = %d\n", best);
	}
	logger("========= END do dynamic programming, minimize cost =======\n");
}
