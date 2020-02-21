#define DONAMESPACE_LISTTEST
#ifdef DONAMESPACE_LISTTEST
namespace listtest {
#endif

	struct node {
	S32 val;
	node* next;
};

void print(node*);
node* reverse(node*);
node* split(node*);

#ifdef DONAMESPACE_LISTTEST
} // end namespace listtest
using namespace listtest;
#endif

void testLists();
