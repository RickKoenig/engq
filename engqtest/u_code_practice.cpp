#include "m_eng.h"

#include "u_code_practice.h"

#ifdef DONAMESPACE_CODE_PRACTICE
namespace code_practice {
#endif

node* makeAList(const vector<S32>& vec)
{
	node* head = nullptr;
	for (S32 i = vec.size() - 1; i >= 0; --i) {
		node* next = new node;
		next->val = vec[i];
		next->next = head;
		head = next;
	}
	return head;
}

node* reverseAList(node* n)
{
	node* ret = nullptr;
	node* rest = n;

	while (rest != nullptr) {
		node* oldret = ret;
		ret = rest;
		rest = rest->next;
		ret->next = oldret;
	}

	return ret;
}

node* reverseAListRecurse(node* n)
{
	if (n == nullptr) {
		return nullptr;
	}
	if (n->next == nullptr) {
		return n;
	}
	node *next = n->next;
	node *rev = reverseAListRecurse(next);
	n->next = nullptr;
	next->next = n;
	return rev;
}

// split a list in place, no copies, modifies the original list
// the left split is the parameter passed in 'll' 
// the right split is the return value
// if odd, 1st list is 1 longer then 2nd
node* splitLinkList(node* ll) {
	if (!ll || !ll->next)
		return nullptr; // nothing to split
	// list has at least 2 nodes, so we can split them
	node* fast = ll;
	node* slow = ll;
	node* lastSlow;
	while (fast) {
		fast = fast->next;
		if (fast) {
			fast = fast->next;
		}
		lastSlow = slow;
		slow = slow->next;
	}
	lastSlow->next = nullptr;
	return slow;
}

// merge 2 sorted lists, in place, no copies
node* mergeLinkLists(node* first, node* second) {
	// serious shortcuts
	if (!first)
		return second;
	if (!second)
		return first;
	// first and second lists have at least 1 node each
	node* head; // return merged list
	if (first->val < second->val) {
		head = first;
		first = first->next;
	} else {
		head = second;
		second = second->next;
	}
	node* walk = head; // use walk to build the return list 'head'

	// process with 2 active lists
	while (first && second) {
		if (first->val < second->val) {
			// first goes next
			walk->next = first;
			walk = first;
			first = first->next;
		} else {
			// second goes next
			walk->next = second;
			walk = second;
			second = second->next;
		}
	}

	// the other one is null, hook up the rest
	if (first) {
		walk->next = first;
	} else if (second) {
		walk->next = second;
	}
	return head;
}

node* mergeSortAList(node* lst)
{
	if (!lst || !lst->next) { // 0 or 1 elements
		return lst;
	}
	node* left = lst;
	node* right = splitLinkList(lst);
	left = mergeSortAList(left);
	right = mergeSortAList(right);
	return mergeLinkLists(left, right);
}
/*
node* mergeSortAList(node* n) {
	return n;
}
*/
// ascending order
bool isSorted(node* n) {
	if (n == nullptr) {
		return true; // 0 elements
	}
	while (true) {
		if (n->next == nullptr) {
			return true; // 1 element
		}
		// 2 or more elements
		if (n->next->val < n->val) {
			return false;
		}
		n = n->next;
	}
	return true;
}

void printAList(const node* n)
{
	logger("( ");
	while (n) {
		logger("%2d ", n->val);
		n = n->next;
	}
	logger(" )\n");
}

void freeAList(const node* n)
{
	while (n) {
		node* next = n->next;
		delete n;
		n = next;
	}
}

const C8* strtokTest(C8* strArg, const C8* delim)
{
	// non null first arg and early outs
	if (delim == nullptr) {
		return strArg;
	}
	static C8* str;
	if (strArg) {
		str = strArg;
	}
	if (str == nullptr) {
		return nullptr;
	}

	// skip over delim chars
	while (*str) {
		const C8* d = delim;
		while (*d) {
			if (*d == *str) {
				++str; // is a delim, skip
				break;
			}
			++d;
		}
		if (!*d) {
			break; // first non delim, use
		}
	}
	if (!*str) {
		str = nullptr; // no more tokens
		return nullptr; // no more non delims
	}

	// first non delim
	const C8* left = str++;
	// walk non delims
	do {
		const C8* d = delim;
		while (*d) {
			if (*d == *str) { // found end of token
				break;
			}
			++d;
		}
		if (*d) {
			// first delim
			break;
		}
		++str;
	} while (*str);
	if (*str) {
		// null terminate token and step
		*str++ = '\0';
	}
	return left;
}

#ifdef DONAMESPACE_CODE_PRACTICE
} // end namespace listtest
using namespace code_practice;
#endif

void do_code_practice()
{
	logger("start coding practice\n");

	// test singularly linked lists


	// test reverse
	//node* aList = nullptr;// makeAList({  });
	node* aList = makeAList({ 2, 3, 5, 7 ,11 });
	//node* aList = makeAList({ 2 });
	printAList(aList);
	aList = reverseAListRecurse(aList);
	printAList(aList);
	aList = reverseAList(aList);
	printAList(aList);
	freeAList(aList);


	// test split
	const U32 maxSplitSize = 5;
	for (U32 i = 0; i < maxSplitSize; ++i) {
		vector<S32> vecSeq;
		for (U32 j = 0; j < i; ++j) {
			vecSeq.push_back(j);
		}
		aList = makeAList(vecSeq);
		logger("\nlist to split ");
		printAList(aList);
		node* right = splitLinkList(aList);
		node* left = aList;
		printAList(left);
		printAList(right);
		logger("\n");
		freeAList(left);
		freeAList(right);
	}


	// test sort
	const U32 numSort = 10000;
	U32 goodSort = 0;
	U32 badSort = 0;
	const U32 randRange = 100;
	const U32 seqSize = 200;
	bool verbose = false;
	mt_setseed(3456);
	//mt_setseed(getmicrosec());
	for (U32 i = 0; i < numSort; ++i) {
		vector<S32> vecSeq;
		for (U32 j = 0; j < seqSize; ++j) {
			vecSeq.push_back(mt_random(randRange));
		}
		aList = makeAList(vecSeq);
		if (verbose) {
			logger("\nUNsorted list ");
			printAList(aList);
		}
		aList = mergeSortAList(aList);
		if (verbose) {
			logger("Sorted list ");
			printAList(aList);
		}
		bool sorted = isSorted(aList);
		if (sorted) {
			++goodSort;
		} else {
			++badSort;
		}
		if (verbose) {
			logger(sorted ? "GOOD\n" : "BAD\n");
		}
		freeAList(aList);
	}
	logger("numsort = %d, goodsort = %d, badsort = %d\n\n", numSort, goodSort, badSort);


	// test strtok
	const C8 delim[] = " #.";
	C8 testStr[] = ".#. hello  #.. world#..#";
	C8 *str = testStr;
	const C8* ret;
	while (ret = strtokTest(str, delim)) {
		str = nullptr;
		logger("tok = '%s'\n", ret);
	}


	logger("end coding practice\n");
}
