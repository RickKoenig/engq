#include <m_eng.h>

// assume room in the array
// doesn't know how big the deck is going to get
void addOneCard(S32* deck, S32 curSize, S32 card)
{
	if (curSize < 1)
		return;
	S32 pos = mt_random(curSize + 1);
	swap(deck[pos], deck[curSize]);
}

void doShuffle(S32* cur, S32* end)
{
	S32* deck = cur;
	while (cur != end) {
		addOneCard(deck, cur - deck, *cur);
		++cur;
	}
}

void do_printDeck(S32* arr, U32 numArr, U32 hash)
{
	for (U32 i = 0; i < numArr; ++i) {
		logger("%d", arr[i]);
	}
	logger(" hash = %u\n", hash);
}

S32 doHash(S32* arr, U32 numArr)
{
	S32 h = 0;
	for (U32 i = 0; i < numArr; ++i) {
		h *= 10;
		h += arr[i];
	}
	return h;
}

void doPrintHisto(const vector<U32>& hashes)
{
	U32 maxHash = 0;
	for (auto h : hashes) {
		if (h > maxHash)
			maxHash = h;
	}
	//logger("maxhash = %u\n", maxHash);
	U32 histoSize = maxHash + 1;
	U32* histo = new U32[histoSize]();
	for (auto h : hashes) {
		++histo[h];
	}
	for (U32 i = 0; i < histoSize; ++i) {
		if (histo[i]) {
			logger("histo[%2d] = %u\n", i, histo[i]);
		}
	}
	delete[] histo;
}

void do_perfect_shuffle()
{
	logger("do perfect shuffle\n");
	mt_setseed(24688);
	const S32 deckSize = 4;
	S32 deck[deckSize]{ 1, 2, 3, 4 };
	S32 working[deckSize];
	const S32 numTests = 24000;
	vector<U32> hashes;
	for (S32 i = 0; i < numTests; ++i) {
		copy(deck, deck + deckSize, working);
		doShuffle(working, working + deckSize);
		U32 hash = doHash(working, deckSize);
		hashes.push_back(hash);
		//do_printDeck(working, deckSize, hash);
	}
	doPrintHisto(hashes);
}

void do_random_tests()
{
	logger("========\n");
	logger("in do_random_tests\n");
	logger("========\n");
	do_perfect_shuffle();
}
