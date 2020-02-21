#include <m_eng.h>

#include "u_listtest.h"

#ifdef DONAMESPACE_LISTTEST
namespace listtest {
#endif
	
node* makeListFromVector(const vector<S32>& arr)
{
	node* ret = nullptr;
	for (S32 i = arr.size() - 1; i >= 0; --i) {
		node* oldRet = ret;
		ret = new node();
		ret->val = arr[i];
		ret->next = oldRet;
	}
	return ret;
}

void print(node* n)
{
	logger("node print\n");
	if (n == nullptr) {
		logger("\t   null, length 0\n");
	} else {
		S32 length = 0;
		logger("\t");
		while (n) {
			logger("%d ", n->val);
			n = n->next;
			++length;
		}
		logger(", length %d\n", length);
	}
}

void freeHeapNodes(node* n)
{
	while (n) {
		node* nextNode = n->next;
		delete n;
		n = nextNode;
	}
}

#define RECURSE_REVERSE
#ifdef RECURSE_REVERSE
// using recursion
node* reverse(node* inNode)
{
	if (inNode == nullptr || inNode->next == nullptr)
		return inNode;
	node* rest = inNode->next;
	inNode->next = nullptr;
	node* rev = reverse(rest);
	rest->next = inNode;
	return rev;
}
#else
// using iteration
node* reverse(node* walk)
{
	node* ret = nullptr;
	while (walk) {
		node* oldret = ret;
		ret = walk;
		walk = walk->next;
		ret->next = oldret;
	}
	return ret;
}
#endif

node* split(node* n)
{
	if (!n)
		return n;
	node* fast = n;
	node* slow = n;
	while (true) {
		fast = fast->next;
		if (!fast)
			break;
		fast = fast->next;
		if (!fast)
			break;
		slow = slow->next;
	}
	node* right = slow->next;
	slow->next = nullptr;
	return right;
}

node* merge(node* left, node* right)
{
	// serious shortcuts
	if (!right)
		return left;
	if (!left)
		return right;

	// which node will be returned
	// the one with the smallest first value
	// set node ret once
	node* ret;
	if (left->val < right->val) {
		ret = left;
		left = left->next;
	} else {
		ret = right;
		right = right->next;
	}
	node* walk = ret;

	// process with 2 active lists
	while (left && right) {
		if (left->val < right->val) {
			// left goes next
			walk->next = left;
			walk = left;
			left = left->next;
		} else {
			// right goes next
			walk->next = right;
			walk = right;
			right = right->next;
		}
	}

	// just some left ones to copy over
	while (left) {
		walk->next = left;
		walk = left;
		left = left->next;
	}

	// just some right ones to copy over
	while (right) {
		walk->next = right;
		walk = right;
		right = right->next;
	}
	return ret;
}

node* mergeSort(node* in)
{
	if (!in || !in->next)
		return in;
	node* left = in;
	node* right = split(in);
	left = mergeSort(left);
	right = mergeSort(right);
	return merge(left, right);
}

node somenodes[] = {
	// len = 1
	{2,nullptr},
	// len = 2

	{3,somenodes + 2},
	{5,nullptr},

	// len = 3
	{7,somenodes + 4},
	{11,somenodes + 5},
	{13,nullptr},

	// len = 4
	{17,somenodes + 7},
	{19,somenodes + 8},
	{23,somenodes + 9},
	{29,nullptr},

};


#ifdef DONAMESPACE_LISTTEST
} // end namespace listtest
using namespace listtest;
#endif

void testLists()
{
	logger("--- in listtest ---\n");
	node* nodes[] = {
		nullptr,
		somenodes + 0,
		somenodes + 1,
		somenodes + 3,
		somenodes + 6,
	};
	U32 nodesize = NUMELEMENTS(nodes);
	for (U32 i = 0; i < nodesize; ++i) {
		print(nodes[i]);
	}
	for (U32 i = 0; i < nodesize; ++i) {
		nodes[i] = reverse(nodes[i]);
	}
	logger("---- reverse ----\n");
	for (U32 i = 0; i < nodesize; ++i) {
		print(nodes[i]);
	}

	// make a list from a vector
	logger("--- arrNode ---\n");
	//vector<S32> arr = { 30, 40, 50, 60, 70 };
	vector<S32> arr = { 3, 10, 13, 17, 4, 6, 8, 20 };	//vector<S32> arr = { 30, 40, 50, 60};
	//vector<S32> arr = { 30, 40, 50};
	//vector<S32> arr = { 30, 40};
	//vector<S32> arr = { 30};
	//vector<S32> arr;
	mt_setseed(0);
	for (auto i = 0; i < 0; ++i) {
		arr.push_back(mt_random(10));
	}
	node* arrNode = makeListFromVector(arr);
	print(arrNode);
#if 0
	arrNode = reverse(arrNode);
	logger("--- reverse arrNode ---\n");
	print(arrNode);
#endif
	// split the nodes
	node* splitLeft = arrNode;
	node* splitRight = split(arrNode);
	logger("--- split left ---\n");
	print(splitLeft);
	//freeHeapNodes(splitLeft);
	logger("--- split right ---\n");
	print(splitRight);
	//freeHeapNodes(splitRight);

	// merge the nodes
	node* merged = merge(splitLeft, splitRight);
	logger("--- merged ---\n");
	print(merged);
	freeHeapNodes(merged);
	logger("--- to sort ---\n");
	arr.clear();
	for (auto i = 0; i < 17; ++i) {
		arr.push_back(mt_random(1000));
	}
	arrNode = makeListFromVector(arr);
	print(arrNode);
	logger("--- sorted --- \n");
	arrNode = mergeSort(arrNode);
	print(arrNode);
	freeHeapNodes(arrNode);
}
