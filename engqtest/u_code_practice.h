#define DONAMESPACE_CODE_PRACTICE
#ifdef DONAMESPACE_CODE_PRACTICE
namespace code_practice {
#endif

	struct node {
		S32 val;
		node* next;
};
	
#ifdef DONAMESPACE_CODE_PRACTICE
} // end namespace code_practice
using namespace code_practice;
#endif

void do_code_practice();
